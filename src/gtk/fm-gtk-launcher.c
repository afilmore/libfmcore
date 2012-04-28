/***********************************************************************************************************************
 * fm-gtk-launcher.c
 * 
 * Copyright 2010 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
 * Copyright 2012 Axel FILMORE <axel.filmore@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 2.
 * http://www.gnu.org/licenses/gpl-2.0.txt
 * 
 * This software is an experimental rewrite of PcManFm originally written by Hong Jen Yee aka PCMan for LXDE project.
 * 
 * Purpose: 
 * 
 * 
 **********************************************************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n-lib.h>
#include <gio/gdesktopappinfo.h>
/* for open () */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/* for read () */
#include <unistd.h>

#include "fm-gtk-launcher.h"
#include "fm-gtk-utils.h"
#include "fm-app-chooser-dlg.h"
#include "fm-config.h"
#include "fm-file-info-job.h"
#include "fm-app-info.h"

extern const char EXEC_FILE_DLG[];

typedef struct _LaunchData LaunchData;
struct _LaunchData
{
    GtkWindow* parent;
    FmLaunchFolderFunc folder_func;
    gpointer user_data;
};

static gboolean _fm_launch_files (GAppLaunchContext* ctx,
                                 GList* file_infos,
                                 FmFileLauncher* launcher,
                                 gpointer user_data);

static GAppInfo* choose_app (GList* file_infos, FmMimeType* mime_type, gpointer user_data, GError** err)
{
    LaunchData* data = (LaunchData*)user_data;
    return fm_choose_app_for_mime_type (data->parent, mime_type, mime_type != NULL);
}

static gboolean file_is_executable_script (FmFileInfo* file)
{
    /* We don't execute remote files */
    if (fm_path_is_local (file->path) && fm_file_info_is_text (file) && fm_file_info_get_size (file) > 2)
    {
        /* check if the first two bytes of the file is #! */
        char* path = fm_path_to_str (file->path);
        int fd = open (path, O_RDONLY);
        g_free (path);
        if (fd != -1)
        {
            char buf[4];
            ssize_t r = read (fd, buf, 2);
            close (fd);
            if (r == 2 && buf[0] == '#' && buf[1] == '!')
                return TRUE; /* the file contains #! in first two bytes */
        }
    }
    return FALSE;
}

static FmFileLauncherExecAction on_exec_file (FmFileInfo* file, gpointer user_data)
{
    //LaunchData* data = (LaunchData*) user_data;
    
    GtkBuilder* builder = gtk_builder_new ();
    
    gtk_builder_set_translation_domain (builder, GETTEXT_PACKAGE);
    gtk_builder_add_from_string (builder, EXEC_FILE_DLG, -1, NULL);
    
    GtkDialog *dlg = (GtkDialog*) gtk_builder_get_object (builder, "dlg");
    GtkWidget *msg = (GtkWidget*) gtk_builder_get_object (builder, "msg");
    GtkWidget *icon = (GtkWidget*) gtk_builder_get_object (builder, "icon");
    
    gtk_image_set_from_gicon (GTK_IMAGE (icon), file->icon->gicon, GTK_ICON_SIZE_DIALOG);
    gtk_box_set_homogeneous (GTK_BOX (gtk_dialog_get_action_area (GTK_DIALOG (dlg))), FALSE);

    char* msg_str;
    
    // Ask before opening scripts
    if (file_is_executable_script (file))
    {
        msg_str = g_strdup_printf (
            _("This text file '%s' seems to be an executable script.\nWhat do you want to do with it?"),
            fm_file_info_get_disp_name (file));
        
        gtk_dialog_set_default_response (GTK_DIALOG (dlg), FM_FILE_LAUNCHER_EXEC_IN_TERMINAL);
    }
    else
    {
        GtkWidget* open = (GtkWidget*) gtk_builder_get_object (builder, "open");
        gtk_widget_destroy (open);
        msg_str = g_strdup_printf (_("This file '%s' is executable. Do you want to execute it?"),
                                   fm_file_info_get_disp_name (file));
        
        gtk_dialog_set_default_response (GTK_DIALOG (dlg), FM_FILE_LAUNCHER_EXEC);
    }
    
    gtk_label_set_text (GTK_LABEL (msg), msg_str);
    g_free (msg_str);

    int res = gtk_dialog_run (GTK_DIALOG (dlg));    // MIGRATION: will be deprecated...
                                                    // They should rename their stuff Dtk, the Deprecation Tool Kit...
    gtk_widget_destroy ((GtkWidget*) dlg);
    g_object_unref (builder);

    return ((res <=0) ? FM_FILE_LAUNCHER_EXEC_CANCEL : res);
}

static gboolean on_launch_error (GAppLaunchContext* ctx, GError* err, gpointer user_data)
{
    LaunchData* data = (LaunchData*) user_data;
    
    g_return_val_if_fail (data && data->parent, FALSE);
    
    fm_show_error (data->parent, NULL, err->message);
    
    return TRUE;
}

static int on_launch_ask (const char* msg, const char** btn_labels, int default_btn, gpointer user_data)
{
    LaunchData* data = (LaunchData*)user_data;
    
    g_return_val_if_fail (data && data->parent, 0);
    
    /* FIXME: set default button properly */
    
    return fm_askv (data->parent, NULL, msg, btn_labels);
}

gboolean fm_launch_paths (GAppLaunchContext* ctx, GList* paths, FmFileLauncher* launcher, gpointer user_data)
{
    FmJob* job = fm_file_info_job_new (NULL, 0);
    
    GList* l;
    gboolean ret;
    
    for (l=paths;l;l=l->next)
        fm_file_info_job_add (FM_FILE_INFO_JOB (job), (FmPath*)l->data);
    
    ret = fm_job_run_sync_with_mainloop (job);
    
    if (ret)
    {
        GList* file_infos = fm_list_peek_head_link (FM_FILE_INFO_JOB (job)->file_infos);
        if (file_infos)
            ret = _fm_launch_files (ctx, file_infos, launcher, user_data);
        else
            ret = FALSE;
    }
    
    g_object_unref (job);
    return ret;
}

gboolean fm_launch_desktop_entry (GAppLaunchContext* ctx,
                                  const char* file_or_id,
                                  GList* uris,
                                  FmFileLauncher* launcher,
                                  gpointer user_data)
{
    gboolean ret = FALSE;
    GAppInfo* app;
    gboolean is_absolute_path = g_path_is_absolute (file_or_id);
    GList* _uris = NULL;
    GError* err = NULL;

    /* Let GDesktopAppInfo try first. */
    if (is_absolute_path)
        app = g_desktop_app_info_new_from_filename (file_or_id);
    else
        app = g_desktop_app_info_new (file_or_id);

    if (!app) /* gio failed loading it. Let's see what's going on */
    {
        printf ("fm_launch_desktop_entry: GIO Failed to load %s entry file !!!\n", file_or_id);
        
        gboolean loaded;
        GKeyFile* kf = g_key_file_new ();

        /* load the desktop entry file ourselves */
        if (is_absolute_path)
            loaded = g_key_file_load_from_file (kf, file_or_id, 0, &err);
        else
        {
            char* tmp = g_strconcat ("applications/", file_or_id, NULL);
            loaded = g_key_file_load_from_data_dirs (kf, tmp, NULL, 0, &err);
            g_free (tmp);
        }
        
        if (loaded)
        {
            char* type = g_key_file_get_string (kf, G_KEY_FILE_DESKTOP_GROUP, "Type", NULL);
            
            /* gio only supports "Application" type. Let's handle other types ourselves. */
            if (type)
            {
                if (strcmp (type, "Link") == 0)
                {
                    char* url = g_key_file_get_string (kf, G_KEY_FILE_DESKTOP_GROUP, "URL", &err);
                    if (url)
                    {
                        char* scheme = g_uri_parse_scheme (url);
                        if (scheme)
                        {
                            if (strcmp (scheme, "file") == 0 ||
                               strcmp (scheme, "trash") == 0 ||
                               strcmp (scheme, "network") == 0 ||
                               strcmp (scheme, "computer") == 0 ||
                               strcmp (scheme, "menu") == 0)
                            {
                                /* OK, it's a file. We can handle it! */
                                /* FIXME: prevent recursive invocation of desktop entry file.
                                 * e.g: If this URL points to the another desktop entry file, and it
                                 * points to yet another desktop entry file, this can create a
                                 * infinite loop. This is a extremely rare case. */
                                FmPath* path = fm_path_new_for_uri (url);
                                _uris = g_list_prepend (_uris, path);
                                ret = fm_launch_paths (ctx, _uris, launcher, user_data);
                                g_list_free (_uris);
                                fm_path_unref (path);
                                _uris = NULL;
                            }
                            else
                            {
                                /* Damn! this actually relies on gconf to work. */
                                /* FIXME: use our own way to get a usable browser later. */
                                app = g_app_info_get_default_for_uri_scheme (scheme);
                                uris = _uris = g_list_prepend (NULL, url);
                            }
                            g_free (scheme);
                        }
                    }
                }
                else if (strcmp (type, "Directory") == 0)
                {
                    /* FIXME: how should this work? It's not defined in the spec. :- ( */
                }
                g_free (type);
            }
        }
        g_key_file_free (kf);
    }

    if (app)
        ret = fm_app_info_launch_uris (app, uris, ctx, &err);

    if (err)
    {
        if (launcher->error)
            launcher->error (ctx, err, user_data);
        g_error_free (err);
    }

    if (_uris)
    {
        g_list_foreach (_uris, (GFunc)g_free, NULL);
        g_list_free (_uris);
    }

    return ret;
}

static gboolean _fm_launch_files (GAppLaunchContext* ctx, GList* file_infos, FmFileLauncher* launcher, gpointer user_data)
{
    GHashTable* hash = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, NULL);
    
    GList* l;
    //GList* folders = NULL;
    FmFileInfo* fi;
    GError* err = NULL;
    GAppInfo* app;

    for (l = file_infos; l; l=l->next)
    {
        GList* fis;
        fi = (FmFileInfo*) l->data;
        
        /* FIXME: handle shortcuts, such as the items in menu:// */
        if (fm_path_is_native (fi->path))
        {
            char* filename;
            
            // Open Desktop Entries...
            if (fm_file_info_is_desktop_entry (fi))
            {
                filename = fm_path_to_str (fi->path);
                fm_launch_desktop_entry (ctx, filename, NULL, launcher, user_data);
                continue;
            }
            else if (fm_file_info_is_executable_type (fi))
            {
                filename = fm_path_to_str (fi->path);
                
                if (g_file_test (filename, G_FILE_TEST_IS_EXECUTABLE)) // FIXME: use eaccess/euidaccess...
                {
                    if (launcher->exec_file)
                    {
                        FmFileLauncherExecAction act = launcher->exec_file (fi, user_data);
                        GAppInfoCreateFlags flags = 0;
                        
                        switch (act)
                        {
                            case FM_FILE_LAUNCHER_EXEC_IN_TERMINAL:
                                flags |= G_APP_INFO_CREATE_NEEDS_TERMINAL;
                            case FM_FILE_LAUNCHER_EXEC:
                            {
                                /* filename may contain spaces. Fix #3143296 */
                                char* quoted = g_shell_quote (filename);
                                app = fm_app_info_create_from_commandline (quoted, NULL, flags, NULL);
                                g_free (quoted);
                                if (app)
                                {
                                    if (!fm_app_info_launch (app, NULL, ctx, &err))
                                    {
                                        if (launcher->error)
                                            launcher->error (ctx, err, user_data);
                                        g_error_free (err);
                                        err = NULL;
                                    }
                                    g_object_unref (app);
                                    continue;
                                }
                            break;
                            }
                            
                            case FM_FILE_LAUNCHER_EXEC_OPEN:
                            break;
                            
                            case FM_FILE_LAUNCHER_EXEC_CANCEL:
                            continue;
                        }
                    }
                }
                g_free (filename);
            }
        }
        // Not A Native Path...
        else if (fm_file_info_is_shortcut (fi)
                 && !fm_file_info_is_dir (fi)
                 && fm_path_is_xdg_menu (fi->path)
                 && fi->target)
        {
            fm_launch_desktop_entry (ctx, fi->target, NULL, launcher, user_data); // FIXME: shortcuts handling...
            continue;
        }
        
        // Add to the hash table...
        if (fi->type && fi->type->type)
        {
            fis = g_hash_table_lookup (hash, fi->type->type);
            fis = g_list_prepend (fis, fi);
            g_hash_table_insert (hash, fi->type->type, fis);
        }
    }

    // Launch from the hash table...
    if (g_hash_table_size (hash) > 0)
    {
        GHashTableIter it;
        const char* type;
        GList* fis;
        g_hash_table_iter_init (&it, hash);
        
        while (g_hash_table_iter_next (&it, &type, &fis))
        {
            GAppInfo* app = g_app_info_get_default_for_type (type, FALSE);
            if (!app && launcher->get_app)
            {
                FmMimeType* mime_type = ((FmFileInfo*)fis->data)->type;
                app = launcher->get_app (fis, mime_type, user_data, NULL);
            }
            
            if (app)
            {
                for (l=fis; l; l=l->next)
                {
                    char* uri;
                    fi = (FmFileInfo*)l->data;
                    uri = fm_path_to_uri (fi->path);
                    l->data = uri;
                    printf ("fm-gtk-launcher.c: _fm_launch_files (%s)\n", uri);
                }
                fis = g_list_reverse (fis);
                
                
                fm_app_info_launch_uris (app, fis, ctx, err);
                
                /* free URI strings */
                g_list_foreach (fis, (GFunc)g_free, NULL);
                g_object_unref (app);
            }
            g_list_free (fis);
        }
    }
    g_hash_table_destroy (hash);

    return TRUE;
}

gboolean fm_launch_file (GtkWindow *parent,
                         GAppLaunchContext *ctx,
                         FmFileInfo *file_info,
                         FmLaunchFolderFunc func,
                         gpointer user_data)
{
    gboolean ret;
    GList* files = g_list_prepend (NULL, file_info);
    ret = fm_launch_multiple_files (parent, ctx, files, func, user_data);
    g_list_free (files);
    return ret;
}

gboolean fm_launch_multiple_files (GtkWindow* parent,
                                   GAppLaunchContext* ctx,
                                   GList* file_infos,
                                   FmLaunchFolderFunc func,
                                   gpointer user_data)
{
    FmFileLauncher launcher = {
        choose_app,
        NULL,
        on_exec_file,
        on_launch_error,
        on_launch_ask
    };
    
    LaunchData data = {parent, func, user_data};
    
    GAppLaunchContext *_ctx = NULL;

    if (ctx == NULL)
    {
        _ctx = (GAppLaunchContext*) gdk_app_launch_context_new ();
        gdk_app_launch_context_set_screen (GDK_APP_LAUNCH_CONTEXT (_ctx),
                                           parent ?
                                           gtk_widget_get_screen (GTK_WIDGET (parent))
                                           : gdk_screen_get_default ());
        
        gdk_app_launch_context_set_timestamp (GDK_APP_LAUNCH_CONTEXT (_ctx), gtk_get_current_event_time ());
        // how to handle gdk_app_launch_context_set_icon ?
        ctx = _ctx;
    }
    
    gboolean ret = _fm_launch_files (ctx, file_infos, &launcher, &data);
    
    if (_ctx)
        g_object_unref (_ctx);
    
    return ret;
}


