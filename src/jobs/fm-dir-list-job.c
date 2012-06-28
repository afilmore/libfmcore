/***********************************************************************************************************************
 * 
 *      fm-dir-list-job.c
 *
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *       (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 *
 * 
 **********************************************************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "fm-dir-list-job.h"

#include <menu-cache.h>

#include "fm-debug.h"

#include <glib/gi18n-lib.h>
#include <gio/gio.h>
#include <string.h>
#include <glib/gstdio.h>


G_DEFINE_TYPE (FmDirListJob, fm_dir_list_job, FM_TYPE_JOB);


extern MenuCache *global_menu_cache;

// Forward declarations...
static void     fm_dir_list_job_finalize        (GObject *object);

static gboolean fm_dir_list_job_run             (FmDirListJob *job);
static gboolean fm_dir_list_job_run_posix       (FmDirListJob *job);
static gboolean fm_dir_list_job_run_gio         (FmDirListJob *job);
static gboolean fm_dir_list_job_list_xdg_menu   (FmDirListJob *job);

static gboolean list_menu_items                 (gpointer user_data /*FmJob *fmjob*/);


FmJob *fm_dir_list_job_new (FmPath *path, gboolean dir_only)
{
	FmDirListJob *job = (FmDirListJob*) g_object_new (FM_TYPE_DIR_LIST_JOB, NULL);
	
    job->dir_path = fm_path_ref (path);
    job->dir_only = dir_only;
	job->files = fm_file_info_list_new ();
	
    return (FmJob*) job;
}

FmJob *fm_dir_list_job_new_for_gfile (GFile *gfile)
{
	/* FIXME_pcm: should we cache this with hash table? Or, the cache
	 * should be done at the level of FmFolder instead? */
	
    FmDirListJob *job = (FmDirListJob*) g_object_new (FM_TYPE_DIR_LIST_JOB, NULL);
	job->dir_path = fm_path_new_for_gfile (gfile);
	
    return (FmJob*) job;
}


static void fm_dir_list_job_class_init (FmDirListJobClass *klass)
{
	GObjectClass *g_object_class;
	FmJobClass *job_class = FM_JOB_CLASS (klass);
	g_object_class = G_OBJECT_CLASS (klass);
	g_object_class->finalize = fm_dir_list_job_finalize;

	job_class->run = (void*) fm_dir_list_job_run;
}


static void fm_dir_list_job_init (FmDirListJob *self)
{
    //fm_job_init_cancellable (FM_JOB (self));
}

static void fm_dir_list_job_finalize (GObject *object)
{
	FmDirListJob *self;

	g_return_if_fail (object != NULL);
	g_return_if_fail (FM_IS_DIR_LIST_JOB (object));

	self = FM_DIR_LIST_JOB (object);

	if (self->dir_path)
		fm_path_unref (self->dir_path);

    if (self->dir_fi)
        fm_file_info_unref (self->dir_fi);

	if (self->files)
		fm_list_unref (self->files);

	if (G_OBJECT_CLASS (fm_dir_list_job_parent_class)->finalize)
		(*G_OBJECT_CLASS (fm_dir_list_job_parent_class)->finalize) (object);
}


gboolean fm_dir_list_job_run (FmDirListJob *job)
{
    
    // gio is really slower, also there's a problem with symlinks, the panel launcher no longer works...
    //~ gboolean use_gio = TRUE;
    gboolean use_gio = FALSE;
    
    // A native file on the real file system...
    if (!use_gio && fm_path_is_native (job->dir_path))
    {
        return fm_dir_list_job_run_posix (job);
	
    // A virtual path or remote file system path...
    }
    else
    {
        return fm_dir_list_job_run_gio (job);
    }
}

FmFileInfoList *fm_dir_dist_job_get_files (FmDirListJob *job)
{
	return job->files;
}



static gboolean fm_dir_list_job_run_posix (FmDirListJob *job)
{
	GError *gerror = NULL;
    char *dir_path;
    GDir *dir;

    NO_DEBUG ("fm_dir_list_job_run_posix\n");


    dir_path = fm_path_to_str (job->dir_path);

	FmFileInfo *file_info = fm_file_info_new_for_path (job->dir_path);
	
    // FileInfo rework: new function for testing...
    // this one is not cancellable and doesn't handle errors...
    // if (fm_file_info_job_get_info_for_native_file (FM_JOB (job), file_info, dir_path, NULL))
    
    //~ if (fm_file_info_query_native_file (file_info))
    
    if (fm_file_info_query (file_info, NULL, NULL))
    {
        job->dir_fi = file_info;
        if (!fm_file_info_is_dir (file_info))
        {
            gerror = g_error_new (G_IO_ERROR, G_IO_ERROR_NOT_DIRECTORY, _("The specified directory is not valid"));
            
            //fm_file_info_unref (file_info);
            
            fm_job_emit_error (FM_JOB (job), gerror, FM_SEVERITY_CRITICAL);
            g_error_free (gerror);
            return FALSE;
        }
    }
    else
    {
        gerror = g_error_new (G_IO_ERROR, G_IO_ERROR_NOT_DIRECTORY, _("The specified directory is not valid"));
        
        fm_file_info_unref (file_info);
        
        fm_job_emit_error (FM_JOB (job), gerror, FM_SEVERITY_CRITICAL);
        
        g_error_free (gerror);
        return FALSE;
    }

    dir = g_dir_open (dir_path, 0, &gerror);
    if (dir)
    {
        char *name;
        
        GString *fpath = g_string_sized_new (4096);
        
        int dir_len = strlen (dir_path);
        
        g_string_append_len (fpath, dir_path, dir_len);
        if (fpath->str[dir_len-1] != '/')
        {
            g_string_append_c (fpath, '/');
            ++dir_len;
        }
        
        while (! fm_job_is_cancelled (FM_JOB (job)) &&  (name = (char*) g_dir_read_name (dir)))
        {
            g_string_truncate (fpath, dir_len);
            g_string_append (fpath, name);

            if (job->dir_only) // if we only want directories
            {
                struct stat st;
                
                // FIXME_pcm: this results in an additional stat () call, which is inefficient
                if (stat (fpath->str, &st) == -1 || !S_ISDIR (st.st_mode))
                    continue;
            }

            
            FmPath *child_path = fm_path_new_child (job->dir_path, name);
            
            file_info = fm_file_info_new_for_path (child_path);
            
            fm_path_unref (child_path);
            
            
            _retry:
            
            // FileInfo rework: new function for testing...
            // this one is not cancellable and doesn't handle errors...
            // if (fm_file_info_job_get_info_for_native_file (FM_JOB (job), file_info, fpath->str, &gerror))
            
            //~ if (fm_file_info_query_native_file (file_info))
            
            if (fm_file_info_query (file_info, NULL, NULL))
            {
                fm_list_push_tail_noref (job->files, file_info);
            }
            
            else // failed!
            {
                FmErrorAction act = fm_job_emit_error (FM_JOB (job), gerror, FM_SEVERITY_MILD);
                
                g_error_free (gerror);
                gerror = NULL;
                
                if (act == FM_ERROR_ACTION_RETRY)
                    goto _retry;

                fm_file_info_unref (file_info);
            }
        }
        
        g_string_free (fpath, TRUE);
        
        g_dir_close (dir);
    }
    else
    {
        fm_job_emit_error (FM_JOB (job), gerror, FM_SEVERITY_CRITICAL);
        g_error_free (gerror);
    }
    
    g_free (dir_path);
    return TRUE;
}

static gboolean fm_dir_list_job_run_gio (FmDirListJob *job)
{
    FmJob *fmjob = FM_JOB (job);
	
    GFileEnumerator *file_enumerator;
	GError *gerror = NULL;
    GFileInfo *gfile_info;
    
    NO_DEBUG ("--------------------------------------------------------------------------------\n");
    NO_DEBUG ("fm_dir_list_job_run_gio\n");
    
    
    
    
    
    // Xdg Menu...
    //~ if (fm_path_is_xdg_menu (job->dir_path))
    //~ {
        //~ return fm_dir_list_job_list_xdg_menu (job);
    //~ }
    
    if (fm_path_is_xdg_menu (job->dir_path))
    {
    
        //~ JOB_DEBUG ();
        
        //job->dir_fi = fm_file_info_new_for_path (job->dir_path);
        
        //fm_file_info_query (job->dir_fi, NULL, NULL);
        
    
        return fm_dir_list_job_list_xdg_menu (job);
    
    
    }
    
    
    
    
    
    GFile *gfile = fm_path_to_gfile (job->dir_path);
    
    
    
    _retry:
    
	
    gfile_info = g_file_query_info (gfile, gfile_info_query_attribs, 0, fm_job_get_cancellable (fmjob), &gerror);
    if (!gfile_info)
    {
        FmErrorAction act = fm_job_emit_error (fmjob, gerror, FM_SEVERITY_MODERATE);
        if (act == FM_ERROR_ACTION_RETRY)
        {
            g_error_free (gerror);
            gerror = NULL;
            goto _retry;
        }
        else
        {
            g_object_unref (gfile);
            g_error_free (gerror);
            return FALSE;
        }
    }

    GFileType gfile_type = g_file_info_get_file_type (gfile_info);
    
    if (gfile_type != G_FILE_TYPE_DIRECTORY && gfile_type != G_FILE_TYPE_MOUNTABLE)
    {
        NO_DEBUG ("GFileType = %d\n", gfile_type);
        
        gerror = g_error_new (G_IO_ERROR, G_IO_ERROR_NOT_DIRECTORY, _("The specified directory is not valid"));
        fm_job_emit_error (fmjob, gerror, FM_SEVERITY_CRITICAL);
        g_error_free (gerror);
        g_object_unref (gfile);
        g_object_unref (gfile_info);
        return FALSE;
    }

    
    
    
    job->dir_fi = fm_file_info_new_for_path (job->dir_path);
    fm_file_info_query (job->dir_fi, NULL, NULL);
    
    //~ fm_file_info_set_for_gfileinfo (job->dir_fi, gfile_info);
    //~ 
    g_object_unref (gfile_info);

    
    
    const char *query;

    if (G_UNLIKELY (job->dir_only))
    {
        query = G_FILE_ATTRIBUTE_STANDARD_TYPE","G_FILE_ATTRIBUTE_STANDARD_NAME","
                G_FILE_ATTRIBUTE_STANDARD_IS_HIDDEN","G_FILE_ATTRIBUTE_STANDARD_IS_BACKUP","
                G_FILE_ATTRIBUTE_STANDARD_IS_SYMLINK","G_FILE_ATTRIBUTE_STANDARD_IS_VIRTUAL","
                G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME","G_FILE_ATTRIBUTE_STANDARD_ICON","
                G_FILE_ATTRIBUTE_STANDARD_SIZE","G_FILE_ATTRIBUTE_STANDARD_TARGET_URI","
                "unix::*,time::*,access::*,id::filesystem";
    }
    else
        query = gfile_info_query_attribs;

    
    // List children files...
    file_enumerator = g_file_enumerate_children  (gfile, query, 0, fm_job_get_cancellable (fmjob), &gerror);
    
    FmFileInfo *file_info;
    g_object_unref (gfile);
    if (!file_enumerator)
    {
        fm_job_emit_error (fmjob, gerror, FM_SEVERITY_CRITICAL);
        g_error_free (gerror);
        return FALSE;
    }
        
    while (! fm_job_is_cancelled (FM_JOB (job)))
    {
        gfile_info = g_file_enumerator_next_file (file_enumerator, fm_job_get_cancellable (fmjob), &gerror);
        
        if (gfile_info)
        {
            FmPath *sub;
            if (G_UNLIKELY (job->dir_only))
            {
                // FIXME_pcm: handle symlinks
                if (g_file_info_get_file_type (gfile_info) != G_FILE_TYPE_DIRECTORY)
                {
                    g_object_unref (gfile_info);
                    continue;
                }
            }

            
            
            
            NO_DEBUG ("fm_dir_list_job_run_gio: file name = %s\n", g_file_info_get_name (gfile_info));
            //NO_DEBUG ("fm_dir_list_job_run_gio: file display name = %s\n", g_file_info_get_display_name (gfile_info));
            //NO_DEBUG ("fm_dir_list_job_run_gio: file edit name = %s\n", g_file_info_get_edit_name (gfile_info));
            
            sub = fm_path_new_child (job->dir_path, g_file_info_get_name (gfile_info));
            
            
            
            
            //~ file_info = fm_file_info_new_for_path (sub);
            //~ fm_file_info_set_for_gfileinfo (file_info, gfile_info);
            //~ 
            //~ fm_path_unref (sub);
            //~ 
            
            file_info = fm_file_info_new_for_path (sub);
            fm_file_info_query (file_info, NULL, NULL);
            
            fm_path_unref (sub);
            
            
            
            
            
            fm_list_push_tail_noref (job->files, file_info);
        }
        else
        {
            if (gerror)
            {
                FmErrorAction act = fm_job_emit_error (fmjob, gerror, FM_SEVERITY_MILD);
                g_error_free (gerror);
                
                // FM_ERROR_ACTION_RETRY is not supported.
                if (act == FM_ERROR_ACTION_ABORT)
                    fm_job_cancel (FM_JOB (job));
                
                // FIXME_pcm: error handling
            }
            break;
        }
        
        g_object_unref (gfile_info);
    }

    g_file_enumerator_close (file_enumerator, NULL, &gerror);
    g_object_unref (file_enumerator);

    return TRUE;
}

static gboolean fm_dir_list_job_list_xdg_menu (FmDirListJob *job)
{
    g_return_val_if_fail (job != NULL, FALSE);
    
    g_return_val_if_fail (global_menu_cache != NULL, FALSE);
    
    // Calling libmenu-cache is only allowed in main thread.
    g_io_scheduler_job_send_to_mainloop (FM_JOB(job)->job, (GSourceFunc) list_menu_items, job, NULL);    
    
    return TRUE;
}


static gboolean list_menu_items (gpointer user_data /*FmJob *fmjob*/)
{
    FmDirListJob *job = (FmDirListJob*) user_data;

    
    // needs a function for this...
    char *path_str = fm_path_to_str (job->dir_path);
    
    // Strip "menu://"
    char *p = path_str + 5;
    while (*p == '/')
        p++;
    
    char *cache_dir_path = p - 1;
    
    //~ 
    //~ while (*p && *p != '/')
        //~ p++;
    //~ 
    //~ char *ch = *p;
    //~ *p = '\0';
    //~ 
    //~ const char *cache_dir_path = g_strconcat ("/", menu_name, NULL);
    
    
    
    //const char *cache_dir_path = "/Applications";
    //const char *cache_dir_path = "/Applications/DesktopSettings";
    //~ const char *cache_dir_path = "/Applications/System";
    //~ const char *cache_dir_path = "/Applications/DesktopSettings/Administration";
    
    JOB_DEBUG ("\nJOB_DEBUG: list_menu_items: cache_dir_path = %s\n", cache_dir_path);
    
    MenuCacheDir *menu_cache_dir = menu_cache_get_dir_from_path (global_menu_cache, cache_dir_path);
    g_free (path_str);
    
    g_return_val_if_fail (menu_cache_dir != NULL, FALSE);
    
    const char *desktop_name = g_getenv ("XDG_CURRENT_DESKTOP");
    
    guint32 desktop_flag;
    if (desktop_name)
        desktop_flag = menu_cache_get_desktop_env_flag (global_menu_cache, desktop_name);
    else
        desktop_flag = (guint32) -1;

    GList* l;
    for (l = (GList*) menu_cache_dir_get_children (menu_cache_dir); l; l = l->next)
    {
        MenuCacheItem *item = MENU_CACHE_ITEM (l->data);
        
        // Also hide menu items which should be hidden in current DE.
        if (!item || menu_cache_item_get_type (item) == MENU_CACHE_TYPE_SEP)
            continue;
        
        if (menu_cache_item_get_type (item) == MENU_CACHE_TYPE_APP
            && !menu_cache_app_get_is_visible (MENU_CACHE_APP (item), desktop_flag))
            continue;

        if (G_UNLIKELY (job->dir_only) && menu_cache_item_get_type (item) != MENU_CACHE_TYPE_DIR)
            continue;
        
        FmPath *item_path = fm_path_new_child (job->dir_path, menu_cache_item_get_id (item));
        FmFileInfo *file_info = fm_file_info_new_for_path (item_path);
        
        JOB_DEBUG ("JOB_DEBUG: list_menu_items_new: query infos for %s\n", menu_cache_item_get_id (item));
        
        if (!fm_file_info_query_cache_item (file_info))
        {
            JOB_DEBUG ("JOB_DEBUG: list_menu_items_new: ERROR\n");
            fm_path_unref (item_path);
            fm_file_info_unref (file_info);
            return FALSE;
        }
        //fm_file_info_set_for_menu_cache_item (file_info, item);
        
        fm_path_unref (item_path);
        
        fm_list_push_tail_noref (job->files, file_info);
    }

    return TRUE;
}











/**
static gboolean list_menu_items (gpointer user_data) //FmJob *fmjob
{
    // it's tested in the public function...
    //g_return_val_if_fail (user_data != NULL, FALSE);
    
    FmDirListJob *job =  (FmDirListJob*) user_data;
    FmFileInfo *file_info;
    MenuCache *mc;
    MenuCacheDir *menu_cache_dir;
    GList *l;
    char *path_str, *p, ch;
    char *menu_name;
    const char *dir_path;
    guint32 desktop_flag;
    const char *desktop_name;
    
    
    // example: menu://applications.menu/DesktopSettings

    path_str = fm_path_to_str (job->dir_path);
    p = path_str + 5; // skip menu:
    while (*p == '/')
        ++p;
    menu_name = p;
    while (*p && *p != '/')
        ++p;
    ch = *p;
    *p = '\0';
    menu_name = g_strconcat (menu_name, ".menu", NULL);
    
    
    
    mc = menu_cache_lookup_sync (menu_name);
    
    
    // ensure that the menu cache is loaded
    if (!mc) // if it's not loaded
    {
        // try to set $XDG_MENU_PREFIX to "lxde-" for lxmenu-data
        
        const char *menu_prefix = g_getenv ("XDG_MENU_PREFIX");
        
        if (g_strcmp0 (menu_prefix, "lxde-")) // if current value is not lxde-
        {
            char *old_prefix = g_strdup (menu_prefix);
            
            g_setenv ("XDG_MENU_PREFIX", "lxde-", TRUE);
            
            mc = menu_cache_lookup_sync (menu_name);
            
            // restore original environment variable
            if (old_prefix)
            {
                g_setenv ("XDG_MENU_PREFIX", old_prefix, TRUE);
                g_free (old_prefix);
            }
            else
                g_unsetenv ("XDG_MENU_PREFIX");

            if (!mc)
            {
                g_free (menu_name);
                return FALSE;
            }
        }
        else
        {
            g_free (menu_name);
            return FALSE;
        }
    }
    
    g_free (menu_name);
    *p = ch;
    dir_path = p; // path of menu dir, such as: /Internet

    
    desktop_name = g_getenv ("XDG_CURRENT_DESKTOP");
    if (desktop_name)
        desktop_flag = menu_cache_get_desktop_env_flag (mc, desktop_name);
    else
        desktop_flag =  (guint32)-1;

    
    // the menu should be loaded now
    if (*dir_path && ! (*dir_path == '/' && dir_path[1]=='\0'))
    {
        char *tmp = g_strconcat ("/", menu_cache_item_get_id (MENU_CACHE_ITEM (menu_cache_get_root_dir (mc))), dir_path, NULL);
        menu_cache_dir = menu_cache_get_dir_from_path (mc, tmp);
        g_free (tmp);
    }
    else
        menu_cache_dir = menu_cache_get_root_dir (mc);

    
    if (menu_cache_dir)
    {
        
        job->dir_fi = fm_file_info_new_for_path (job->dir_path);
        
        fm_file_info_query_cache_item (job->dir_fi);
        
        //fm_file_info_set_for_menu_cache_item (job->dir_fi, (MenuCacheItem*) menu_cache_dir);
        
        
        for (l = (GList*) menu_cache_dir_get_children (menu_cache_dir); l; l=l->next)
        {
            MenuCacheItem *item = MENU_CACHE_ITEM (l->data);
            FmPath *item_path;
            //~ GIcon *gicon;
            
            
            // also hide menu items which should be hidden in current DE.
            if (!item || menu_cache_item_get_type (item) == MENU_CACHE_TYPE_SEP)
                continue;
            
            if (menu_cache_item_get_type (item) == MENU_CACHE_TYPE_APP && !menu_cache_app_get_is_visible (MENU_CACHE_APP (item), desktop_flag))
                continue;

            if (G_UNLIKELY (job->dir_only) && menu_cache_item_get_type (item) != MENU_CACHE_TYPE_DIR)
                continue;
            
            item_path = fm_path_new_child (job->dir_path, menu_cache_item_get_id (item));
            
            
            file_info = fm_file_info_new_for_path (item_path);
            
            
            
            fm_file_info_query_cache_item (file_info);
            
            //fm_file_info_set_for_menu_cache_item (file_info, item);
            
            
            fm_path_unref (item_path);
            fm_list_push_tail_noref (job->files, file_info);
        }
    }
    
    menu_cache_unref (mc);

    g_free (path_str);
    return TRUE;
}

**/



