/***********************************************************************************************************************
 * 
 *      fm-file-menu.c
 *
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
 *      Copyright 2012 Axel FILMORE <axel.filmore@gmail.com>
 * 
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
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

#include "fm-file-menu.h"

#include <glib/gi18n-lib.h>

#include "fm-qdata.h"

#include "fm-archiver.h"
#include "fm-clipboard.h"
#include "fm-app-info.h"

#include "fm-app-chooser-dlg.h"
#include "fm-file-properties.h"

#include "fm-trash.h"
#include "fm-file-ops.h"


// Forward declarations...
static void fm_file_menu_open_with_app  (FmFileMenu *file_menu, GAppInfo *app);

static void action_open                 (GtkAction *action, gpointer user_data);
static void action_restore              (GtkAction *action, gpointer user_data);

static void action_open_with_app        (GtkAction *action, gpointer user_data);
static void action_open_with            (GtkAction *action, gpointer user_data);

static void action_cut                  (GtkAction *action, gpointer user_data);
static void action_copy                 (GtkAction *action, gpointer user_data);
static void action_paste                (GtkAction *action, gpointer user_data);
static void action_delete               (GtkAction *action, gpointer user_data);
static void action_rename               (GtkAction *action, gpointer user_data);

static void action_link                 (GtkAction *action, gpointer user_data);
static void action_send_to              (GtkAction *action, gpointer user_data);

static void action_empty_trash          (GtkAction *action, gpointer user_data);

static void action_compress             (GtkAction *action, gpointer user_data);
static void action_extract_here         (GtkAction *action, gpointer user_data);
static void action_extract_to           (GtkAction *action, gpointer user_data);

static void action_properties           (GtkAction *action, gpointer user_data);


const char filefolder_popup_xml [] =
    "<popup>"
        "<menuitem action='Open'/>"
        "<separator/>"
        
        "<placeholder name='SPECIAL_ACTIONS'/>"
        "<separator/>"
        
        "<menuitem action='Restore'/>"
        "<menuitem action='EmptyTrash'/>"
        "<separator/>"
        
        "<placeholder name='OPEN_WITH'/>"
        "<separator/>"
        
        "<menuitem action='Cut'/>"
        "<menuitem action='Copy'/>"
        "<menuitem action='Paste'/>"
        "<separator/>"

        "<menuitem action='Delete'/>"
        "<menuitem action='Rename'/>"
        "<menuitem action='Link'/>"
        "<menuitem action='SendTo'/>"
        "<separator/>"
        
        "<placeholder name='ARCHIVER'/>"
        "<separator/>"
        
        "<menuitem action='Properties'/>"
    "</popup>"
;


GtkActionEntry file_menu_actions [] =
{
    {"Open",            GTK_STOCK_OPEN, NULL, NULL, NULL,               G_CALLBACK (action_open)},
    
    {"Restore",         NULL, N_("Restore"), NULL, NULL,                G_CALLBACK (action_restore)},
    {"EmptyTrash",      NULL, N_("Empty Trash"), NULL, NULL,            G_CALLBACK (action_empty_trash)},
    
    {"OpenWithMenu",    NULL, N_("Open With..."), NULL, NULL,           NULL},
    {"OpenWith",        NULL, N_("Open With..."), NULL, NULL,           G_CALLBACK (action_open_with)},
    
    {"Cut",             GTK_STOCK_CUT, NULL, "<Ctrl>X", NULL,           G_CALLBACK (action_cut)},
    {"Copy",            GTK_STOCK_COPY, NULL, "<Ctrl>C", NULL,          G_CALLBACK (action_copy)},
    {"Paste",           GTK_STOCK_PASTE, NULL, "<Ctrl>V", NULL,         G_CALLBACK (action_paste)},
    
    {"Delete",          GTK_STOCK_DELETE, NULL, NULL, NULL,             G_CALLBACK (action_delete)},
    {"Rename",          NULL, N_("Rename"), "F2", NULL,                 G_CALLBACK (action_rename)},
    
    {"Link",            NULL, N_("Create Symlink"), NULL, NULL,         G_CALLBACK (action_link)},
    {"SendTo",          NULL, N_("Send To"), NULL, NULL,                G_CALLBACK (action_send_to)},
    
    {"Compress",        NULL, N_("Compress..."), NULL, NULL,            G_CALLBACK (action_compress)},
    
    {"Extract",         NULL, N_("Extract Here"), NULL, NULL,           G_CALLBACK (action_extract_here)},
    {"Extract2",        NULL, N_("Extract To..."), NULL, NULL,          G_CALLBACK (action_extract_to)},
    
    {"Properties",      GTK_STOCK_PROPERTIES, NULL, NULL, NULL,         G_CALLBACK (action_properties)}
};


/*****************************************************************************************
 * Menu Creation...
 *
 *
 ****************************************************************************************/
FmFileMenu *fm_file_menu_new_for_files (GtkWindow *parent, FmFileInfoList *files, FmPath *current_directory,
                                        gboolean auto_destroy)
{
    GtkUIManager    *ui;
    
    GtkWidget       *menu;
    
    GtkActionGroup  *action_group;
    
    GtkAction       *action;
    FmFileMenu      *file_menu;
    GString         *xml;
    
    
    g_return_val_if_fail (files != NULL, NULL);
    
    
    // Get Paths flags...
    uint            have_flags; // OR
    uint            all_flags;  // AND

    int num_files = fm_file_info_list_get_flags (files, &have_flags, &all_flags);
    
    g_return_val_if_fail (num_files > 0, NULL);
    
    
    /*****************************************************************
     * NOTE: It may be possible to connect to the "destroy" signal
     * of the parent and set file_menu->parent to NULL when it's
     * detroyed...
     * 
     ****************************************************************/
    file_menu = g_slice_new0 (FmFileMenu);
    file_menu->parent = g_object_ref (parent);
    file_menu->file_infos = fm_list_ref (files);
    file_menu->auto_destroy = auto_destroy;

    // The current working directory is used to extract archives...
    if (current_directory)
        file_menu->current_directory = fm_path_ref (current_directory);

    
    /*****************************************************************
     * 
     * 
     * 
     ****************************************************************/
    file_menu->same_type = fm_file_info_list_is_same_type (files);
    gboolean same_fs = fm_file_info_list_is_same_fs (files);
    
    gboolean multiple_files = (num_files > 1);
    gboolean have_virtual = (have_flags & FM_PATH_IS_VIRTUAL);
    gboolean all_trash_files = ((all_flags & FM_PATH_IS_TRASH) && !(all_flags & FM_PATH_IS_ROOT));
    
    gboolean trash_root = (!multiple_files && (all_flags & FM_PATH_IS_ROOT) && (all_flags & FM_PATH_IS_TRASH));
    
    FmFileInfo *first_file_info = (FmFileInfo*) fm_list_peek_head (files);
    FmMimeType *fi_mime_type = fm_file_info_get_mime_type (first_file_info, FALSE);
    
    gboolean show_open_with = (file_menu->same_type && fi_mime_type && !fm_file_info_is_dir (first_file_info)
                               && !have_virtual);
    
    gboolean can_restore = TRUE;
    
    if (all_trash_files)
    {
        GList *l;
        
        // Only immediate children of "trash:///" can be restored...
        for (l = fm_list_peek_head_link (files); l; l = l->next)
        {
            FmPath *trash_path = fm_file_info_get_path (FM_FILE_INFO (l->data));
            FmPath *parent = fm_path_get_parent (trash_path);
            if (!parent || !fm_path_is_trash_root (parent))
            {
                can_restore = FALSE;
                break;
            }
        }
    }


    /*****************************************************************
     * Add Default Menu Items...
     * 
     * 
     ****************************************************************/
    file_menu->ui = ui = gtk_ui_manager_new ();
    file_menu->action_group = action_group = gtk_action_group_new ("Popup");
    gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);

    // Create Files/Folders Popup Menu...
    gtk_action_group_add_actions (action_group, file_menu_actions, G_N_ELEMENTS (file_menu_actions),
                                  file_menu);
    gtk_ui_manager_add_ui_from_string (ui, filefolder_popup_xml, -1, NULL);
    gtk_ui_manager_insert_action_group (ui, action_group, 0);


    /*****************************************************************
     * OpenWith items...
     * 
     * 
     ****************************************************************/
    xml = g_string_new ("");
    
    if (show_open_with)
    {
        GList *apps = g_app_info_get_all_for_type (fi_mime_type->type);
        GList *app_list;
        gboolean use_sub = g_list_length (apps) > 5;
        
        g_string_append (xml, "<popup>\n<placeholder name='OPEN_WITH'>\n");
        
        if (use_sub)
            g_string_append (xml, "<menu action='OpenWithMenu'>\n");

        for (app_list = apps; app_list; app_list = app_list->next)
        {
            GAppInfo *app = app_list->data;

            /**g_debug ("app %s, executable %s, command %s\n",
                g_app_info_get_name (app),
                g_app_info_get_executable (app),
                g_app_info_get_commandline (app));**/

            gchar *program_path = g_find_program_in_path (g_app_info_get_executable (app));
            if (!program_path)
                continue;
            
            g_free (program_path);

            action = gtk_action_new (g_app_info_get_id (app), g_app_info_get_name (app), g_app_info_get_description (app),
                                  NULL);
            
            g_signal_connect (action, "activate", G_CALLBACK (action_open_with_app), file_menu);
            gtk_action_set_gicon (action, g_app_info_get_icon (app));
            gtk_action_group_add_action (action_group, action);
            
            // associate the app info object with the action
            g_object_set_qdata_full (G_OBJECT (action), fm_qdata_id, app, (GDestroyNotify)g_object_unref);
            g_string_append_printf (xml, "<menuitem action='%s'/>\n", g_app_info_get_id (app));
        }
        g_list_free (apps);
        
        if (use_sub)
        {
            g_string_append (xml,
                             "<separator/>\n"
                             "<menuitem action='OpenWith'/>\n"
                             "</menu>\n");
        }
        else
        {
            g_string_append (xml, "<menuitem action='OpenWith'/>\n");
        }
        
        g_string_append (xml, "</placeholder>\n</popup>\n");

    }
    else
    {
        g_string_append (xml, "<popup>\n<placeholder name='OPEN_WITH'>\n");
        g_string_append (xml, "<menuitem action='OpenWith'/>\n");
        g_string_append (xml, "</placeholder>\n</popup>\n");
    }

    /**
    printf ("have flags (OR) = %d, or all flags (AND) = %d\n", have_flags, all_flags);
    
    if (have_flags & FM_PATH_IS_NATIVE)
    {
        printf ("FM_PATH_IS_NATIVE\n");
    }
    
    if (have_flags & FM_PATH_IS_VIRTUAL)
    {
        printf ("FM_PATH_IS_VIRTUAL\n");
    }
    
    if (have_flags & FM_PATH_IS_LOCAL)
    {
        printf ("FM_PATH_IS_LOCAL\n");
    }
    
    if (have_flags & FM_PATH_IS_ROOT)
    {
        printf ("FM_PATH_IS_ROOT\n");
    }
    
    if (have_flags & FM_PATH_IS_TRASH)
    {
        printf ("FM_PATH_IS_TRASH\n");
    }
    
    if (have_flags & FM_PATH_IS_XDG_MENU)
    {
        printf ("FM_PATH_IS_XDG_MENU\n");
    }
    
    if (have_flags != all_flags)
    {
        printf ("different flags\n");
    }**/
    
    
    // Archiver integration
    if (!have_virtual)
    {
        g_string_append (xml, "<popup>\n<placeholder name='ARCHIVER'>\n");
        if (file_menu->same_type)
        {
            FmArchiver *archiver = fm_archiver_get_default ();
            if (archiver)
            {
                if (fm_archiver_is_mime_type_supported (archiver, fi_mime_type->type))
                {
                    if (current_directory && archiver->extract_to_cmd)
                        g_string_append (xml, "<menuitem action='Extract'/>\n");
                    
                    if (archiver->extract_cmd)
                        g_string_append (xml, "<menuitem action='Extract2'/>\n");
                }
                else
                    g_string_append (xml, "<menuitem action='Compress'/>\n");
            }
        }
        else
        { 
            g_string_append (xml, "<menuitem action='Compress'/>\n");
        }
        
        g_string_append (xml, "</placeholder>\n</popup>\n");
    }

    gtk_ui_manager_add_ui_from_string (ui, xml->str, xml->len, NULL);

    g_string_free (xml, TRUE);

    
    action = gtk_ui_manager_get_action (ui, "/popup/Open");
    gtk_action_set_visible (action, !all_trash_files);
    
    action = gtk_ui_manager_get_action (ui, "/popup/Restore");
    gtk_action_set_visible (action, all_trash_files && can_restore);
    
    action = gtk_ui_manager_get_action (ui, "/popup/EmptyTrash");
    gtk_action_set_visible (action, trash_root);
    
    action = gtk_ui_manager_get_action (ui, "/popup/OPEN_WITH/OpenWith");
    gtk_action_set_visible (action, !have_virtual);
    
    action = gtk_ui_manager_get_action (ui, "/popup/Cut");
    gtk_action_set_visible (action, !have_virtual);
    action = gtk_ui_manager_get_action (ui, "/popup/Copy");
    gtk_action_set_visible (action, !have_virtual);

    action = gtk_ui_manager_get_action (ui, "/popup/Paste");
    gtk_action_set_visible (action, (!multiple_files && fm_file_info_is_dir (first_file_info) && !have_virtual));

    action = gtk_ui_manager_get_action (ui, "/popup/Delete");
    gtk_action_set_visible (action, !have_virtual);
    
    action = gtk_ui_manager_get_action (ui, "/popup/Rename");
    gtk_action_set_visible (action, (!multiple_files && !have_virtual));
    
    action = gtk_ui_manager_get_action (ui, "/popup/Link");
    gtk_action_set_visible (action, TRUE);
    
    action = gtk_ui_manager_get_action (ui, "/popup/SendTo");
    gtk_action_set_visible (action, TRUE);
    
    action = gtk_ui_manager_get_action (ui, "/popup/Properties");
    gtk_action_set_visible (action, !have_virtual);
    
   
    return file_menu;
}

void fm_file_menu_set_folder_func (FmFileMenu *menu, FmLaunchFolderFunc func, gpointer user_data)
{
    menu->folder_func =         func;
    menu->folder_func_data =    user_data;
}

GtkMenu *fm_file_menu_get_menu (FmFileMenu *menu)
{
    if (menu->menu)
        return menu->menu;
    
    menu->menu = (GtkMenu*) gtk_ui_manager_get_widget (menu->ui, "/popup");
    
    gtk_menu_attach_to_widget (GTK_MENU (menu->menu), GTK_WIDGET (menu->parent), NULL);

    if (menu->auto_destroy)
        g_signal_connect_swapped (menu->menu, "selection-done", G_CALLBACK (fm_file_menu_destroy), menu);
    
    return menu->menu;
}


/*****************************************************************************************
 *
 *
 *
 *
 ****************************************************************************************/
void fm_file_menu_destroy (FmFileMenu *menu)
{
    if (menu->parent)
        g_object_unref (menu->parent);

    if (menu->menu)
        gtk_widget_destroy ((GtkWidget*) menu->menu);

    if (menu->file_infos)
        fm_list_unref (menu->file_infos);

    if (menu->current_directory)
        fm_path_unref (menu->current_directory);

    if (menu->action_group)
        g_object_unref (menu->action_group);
        
    if (menu->ui)
        g_object_unref (menu->ui);
    
    g_slice_free (FmFileMenu, menu);
}

GtkUIManager *fm_file_menu_get_ui (FmFileMenu *menu)
{
    return menu->ui;
}

GtkActionGroup *fm_file_menu_get_action_group (FmFileMenu *menu)
{
    return menu->action_group;
}

FmFileInfoList *fm_file_menu_get_file_info_list (FmFileMenu *menu)
{
    return menu->file_infos;
}


/*****************************************************************************************
 *  Menu Actions
 *
 *
 *
 ****************************************************************************************/
void action_open (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    
    g_return_if_fail (file_menu || file_menu->file_infos);
    
    GList *files_list = fm_list_peek_head_link (file_menu->file_infos);
    
    fm_launch_multiple_files (file_menu->parent, NULL, files_list, file_menu->folder_func, file_menu->folder_func_data);
}

void action_restore (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    
    FmPathList *files = fm_path_list_new_from_file_info_list (file_menu->file_infos);
    
    fm_trash_restore (file_menu->parent, files);
    
    fm_list_unref (files);
}

void action_open_with_app (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    
    GAppInfo *app = (GAppInfo*) g_object_get_qdata (G_OBJECT(action), fm_qdata_id);
    
    fm_file_menu_open_with_app (file_menu, app);
}

void action_open_with (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    FmFileInfoList *files = file_menu->file_infos;
    
    FmFileInfo *file_info = (FmFileInfo*) fm_list_peek_head (files);
    
    FmMimeType *mime_type;
    FmMimeType *fi_mime_type = fm_file_info_get_mime_type (file_info, FALSE);

    if (file_menu->same_type && fi_mime_type && fi_mime_type->type)
        mime_type = fi_mime_type;
    else
        mime_type = NULL;

    GAppInfo *app = fm_choose_app_for_mime_type (file_menu->parent, mime_type, TRUE);

    if (!app)
        return;
        
    fm_file_menu_open_with_app (file_menu, app);
    
    // Add to applications that support this file type.
    if (mime_type)
        g_app_info_add_supports_type (app, mime_type->type, NULL);
    
    g_object_unref (app);
}

static void fm_file_menu_open_with_app (FmFileMenu *file_menu, GAppInfo *app)
{
    GdkAppLaunchContext *ctx;
    
    FmFileInfoList *files = file_menu->file_infos;
    
    GList *l = fm_list_peek_head_link (files);
    GList *uris = NULL;
    
    int i;
    for (i=0; l; ++i, l=l->next)
    {
        FmFileInfo *file_info = (FmFileInfo*) l->data;
        FmPath *path = file_info->path;
        
        char *uri = fm_path_to_uri (path);
        uris = g_list_prepend (uris, uri);
    }
    
    uris = g_list_reverse (uris);

    ctx = gdk_display_get_app_launch_context    (gtk_widget_get_display ((GtkWidget*) file_menu->menu));
    gdk_app_launch_context_set_screen           (ctx, gtk_widget_get_screen ((GtkWidget*) file_menu->menu));
    gdk_app_launch_context_set_icon             (ctx, g_app_info_get_icon (app));
    gdk_app_launch_context_set_timestamp        (ctx, gtk_get_current_event_time ());

    fm_app_info_launch_uris (app, uris, (GAppLaunchContext*) ctx, NULL);
    
    g_object_unref (ctx);

    g_list_foreach (uris, (GFunc) g_free, NULL);
    g_list_free (uris);
}

void action_cut (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    FmPathList *files;
    
    files = fm_path_list_new_from_file_info_list (file_menu->file_infos);
    fm_clipboard_cut_files ((GtkWidget*) file_menu->parent, files);
    
    fm_list_unref (files);
}

void action_copy (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    FmPathList *files;
    
    files = fm_path_list_new_from_file_info_list (file_menu->file_infos);
    fm_clipboard_copy_files ((GtkWidget*) file_menu->parent, files);
    
    fm_list_unref (files);
}

void action_paste (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    
    FmFileInfo *first_file_info = fm_list_peek_head (file_menu->file_infos);
    if (first_file_info)
    {
        fm_clipboard_paste_files (file_menu->parent, first_file_info->path);
    }
}

void action_delete (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    FmPathList *files;
    files = fm_path_list_new_from_file_info_list (file_menu->file_infos);
    
    fm_trash_delete (GTK_WINDOW (file_menu->parent), files, FM_DELETE_FLAGS_TRASH_OR_DELETE, TRUE);
    
    fm_list_unref (files);
}

void action_rename (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    FmFileInfo *first_file_info = fm_list_peek_head (file_menu->file_infos);
    
    if (first_file_info)
        fm_rename_file (file_menu->parent, first_file_info->path);
}

void action_link (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    
    g_return_if_fail (file_menu->file_infos != NULL);
    g_return_if_fail (file_menu->current_directory != NULL);
    
    FmPathList *files = fm_path_list_new_from_file_info_list (file_menu->file_infos);
    fm_link_files (file_menu->parent, files, file_menu->current_directory);
    
    fm_list_unref (files);
}

void action_send_to (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    
    g_return_if_fail (file_menu->file_infos != NULL);
    g_return_if_fail (file_menu->current_directory != NULL);
    
    FmPathList *files = fm_path_list_new_from_file_info_list (file_menu->file_infos);
    fm_link_files (file_menu->parent, files, fm_path_get_desktop ());
    
    fm_list_unref (files);
}

void action_empty_trash (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    fm_trash_empty (file_menu->parent);
}

void action_compress (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    FmPathList *files;
    FmArchiver *archiver = fm_archiver_get_default ();
    
    if (!archiver)
        return;
        
    GAppLaunchContext *ctx = (GAppLaunchContext*) gdk_display_get_app_launch_context (gdk_display_get_default ());
    
    files = fm_path_list_new_from_file_info_list (file_menu->file_infos);
    fm_archiver_create_archive (archiver, ctx, files);
    
    fm_list_unref (files);
    g_object_unref (ctx);
}

void action_extract_here (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    FmPathList *files;
    
    FmArchiver *archiver = fm_archiver_get_default ();
    if (!archiver)
        return;
    
    GAppLaunchContext *ctx = (GAppLaunchContext*) gdk_display_get_app_launch_context (gdk_display_get_default ());
    
    files = fm_path_list_new_from_file_info_list (file_menu->file_infos);
    fm_archiver_extract_archives_to (archiver, ctx, files, file_menu->current_directory);
    
    fm_list_unref (files);
    g_object_unref (ctx);
}

void action_extract_to (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    FmPathList *files;
    
    FmArchiver *archiver = fm_archiver_get_default ();
    
    if (!archiver)
        return;
    
    GAppLaunchContext *ctx = (GAppLaunchContext*) gdk_display_get_app_launch_context (gdk_display_get_default ());

    files = fm_path_list_new_from_file_info_list (file_menu->file_infos);
    fm_archiver_extract_archives (archiver, ctx, files);

    fm_list_unref (files);
    g_object_unref (ctx);
}

void action_properties (GtkAction *action, gpointer user_data)
{
    FmFileMenu *file_menu = (FmFileMenu*) user_data;
    
    g_return_if_fail (file_menu || file_menu->file_infos);
    
    FmFileInfoList *files = file_menu->file_infos;
    
    /**uint flags;
    fm_file_info_list_get_flags (files, &flags, NULL);
    
    if (flags & FM_PATH_IS_VIRTUAL)
    {
        // TODO_axl: printf ("NEEDS A VIRTUAL DIALOG !!!!!\n");
        return;
    }**/
    
    fm_show_file_properties (file_menu->parent, files);
}




