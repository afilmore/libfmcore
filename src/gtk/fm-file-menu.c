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

#include <glib/gi18n-lib.h>

#include "fm.h"
#include "fm-config.h"

#include "fm-file-menu.h"
#include "fm-path.h"

#include "fm-clipboard.h"
#include "fm-file-properties.h"

#include "fm-utils.h"

#include "fm-app-chooser-dlg.h"
#include "fm-archiver.h"
#include "fm-app-info.h"
#include "fm-gtk-launcher.h"


/** Add Custom Actions Later...
 *  #include "fm-actions.h"
 **/


/*****************************************************************************************
 * Popup Place Holders :
 * 
 * ph2 > OpenWith Menu...
 *
 * 
 ****************************************************************************************/
const char filefolder_popup_xml[] =
    "<popup>"
        "<menuitem action='Open'/>"
        "<separator/>"
        
        "<placeholder name='OPEN_WITH'/>"
        "<separator/>"
        
        "<menuitem action='Cut'/>"
        "<menuitem action='Copy'/>"
        "<menuitem action='Paste'/>"
        "<menuitem action='Del'/>"
        "<separator/>"
        
        "<menuitem action='Rename'/>"
        "<separator/>"
        
        "<menuitem action='EmptyTrash'/>"
        "<separator/>"
        
        "<placeholder name='ARCHIVER'/>"
        "<separator/>"
        
        "<menuitem action='Properties'/>"
    "</popup>"
;


// Forward declarations...
static void on_open             (GtkAction *action, gpointer user_data);
static void on_restaure         (GtkAction *action, gpointer user_data);

static void on_open_with_app    (GtkAction *action, gpointer user_data);
static void on_open_with        (GtkAction *action, gpointer user_data);

static void on_cut              (GtkAction *action, gpointer user_data);
static void on_copy             (GtkAction *action, gpointer user_data);
static void on_paste            (GtkAction *action, gpointer user_data);
static void on_delete           (GtkAction *action, gpointer user_data);
static void on_rename           (GtkAction *action, gpointer user_data);

static void on_empty_trash      (GtkAction *action, gpointer user_data);

static void on_compress         (GtkAction *action, gpointer user_data);
static void on_extract_here     (GtkAction *action, gpointer user_data);
static void on_extract_to       (GtkAction *action, gpointer user_data);

static void on_properties       (GtkAction *action, gpointer user_data);


GtkActionEntry filefolder_menu_actions[] =
{
    {"Open",            GTK_STOCK_OPEN, NULL, NULL, NULL,               G_CALLBACK (on_open)},
    
    {"OpenWithMenu",    NULL, N_ ("Open With..."), NULL, NULL,          NULL},
    {"OpenWith",        NULL, N_ ("Open With..."), NULL, NULL,          G_CALLBACK (on_open_with)},
    
    {"Cut",             GTK_STOCK_CUT, NULL, "<Ctrl>X", NULL,           G_CALLBACK (on_cut)},
    {"Copy",            GTK_STOCK_COPY, NULL, "<Ctrl>C", NULL,          G_CALLBACK (on_copy)},
    {"Paste",           GTK_STOCK_PASTE, NULL, "<Ctrl>V", NULL,         G_CALLBACK (on_paste)},
    {"Del",             GTK_STOCK_DELETE, NULL, NULL, NULL,             G_CALLBACK (on_delete)},
    
    {"Rename",          NULL, N_ ("Rename"), "F2", NULL,                G_CALLBACK (on_rename)},
    
    /** TODO_axl
    {"Link",            NULL, N_ ("Create Symlink"), NULL, NULL,        NULL},
    {"SendTo",          NULL, N_ ("Send To"), NULL, NULL,               NULL},
    **/
    
    {"EmptyTrash",      NULL, N_ ("Empty Trash"), NULL, NULL,           G_CALLBACK (on_empty_trash)},
    
    {"Compress",        NULL, N_ ("Compress..."), NULL, NULL,           G_CALLBACK (on_compress)},
    
    {"Extract",         NULL, N_ ("Extract Here"), NULL, NULL,          G_CALLBACK (on_extract_here)},
    {"Extract2",        NULL, N_ ("Extract To..."), NULL, NULL,         G_CALLBACK (on_extract_to)},
    
    {"Properties",      GTK_STOCK_PROPERTIES, NULL, NULL, NULL,         G_CALLBACK (on_properties)}
};


/*****************************************************************************************
 * Menu Creation/Destruction...
 *
 *
 ****************************************************************************************/
FmFileMenu *fm_file_menu_new_for_files (GtkWindow *parent, FmFileInfoList *files, FmPath *cwd, gboolean auto_destroy)
{
    GtkWidget       *menu;
    GtkUIManager    *ui;
    GtkActionGroup  *act_grp;
    GtkAccelGroup   *accel_grp;
    GtkAction       *act;
    FmFileMenu      *data;
    GString         *xml;
    
    g_return_val_if_fail (files && !fm_list_is_empty (files), NULL);
    
    data = g_slice_new0 (FmFileMenu);
    
    /***
     * Is this really needed ? Should we connect to "destroy" signal of the parent
     * and set data->parent to NULL when it's detroyed ?
     * 
     ***/
    data->parent = g_object_ref (parent);
    
    data->file_infos = fm_list_ref (files);

    // check if the files are of the same type
    data->same_type = fm_file_info_list_is_same_type (files);

    // check if the files are on the same filesystem
    data->same_fs = fm_file_info_list_is_same_fs (files);

    
    gboolean multiple_files = (fm_list_get_length (files) > 1);
    
    
    // check if selected files contains the Trash Can
    uint flags = fm_file_info_list_get_flags (files);
    
    gboolean hide_empty_trash_can = FALSE;
    
    if (multiple_files || !(flags & FM_PATH_IS_TRASH_CAN))
        hide_empty_trash_can = TRUE;

    gboolean hide_file_actions = FALSE;
    gboolean hide_archiver = FALSE;
    
    if (flags & FM_PATH_IS_VIRTUAL) {
        hide_file_actions = TRUE;
        hide_archiver = TRUE;
    }
    
    
    //~ GList *file_list = (GList *) fm_list_peek_head (files);
    //~ g_return_val_if_fail (file_list, NULL);
    //GList *first_file = g_list_first (file_list);
    
    FmFileInfo *first_file_info = (FmFileInfo*) fm_list_peek_head (files);
    
    data->all_virtual = data->same_fs && fm_path_is_virtual (first_file_info->path);
    data->all_trash = data->same_fs && fm_path_is_trash (first_file_info->path);
    
    //printf ("fm_file_menu_new_for_files: data->all_trash = %s\n", data->all_trash ? "TRUE" : "FALSE");
    
    data->auto_destroy = auto_destroy;
    
    // the current working directory is used to extract archives.
    if (cwd)
        data->cwd = fm_path_ref (cwd);

    // Add Default Menu Items...
    data->ui = ui = gtk_ui_manager_new ();
    data->act_grp = act_grp = gtk_action_group_new ("Popup");
    gtk_action_group_set_translation_domain (act_grp, GETTEXT_PACKAGE);

    // Create Files/Folders Popup Menu...
    gtk_action_group_add_actions (act_grp, filefolder_menu_actions, G_N_ELEMENTS (filefolder_menu_actions), data);
    gtk_ui_manager_add_ui_from_string (ui, filefolder_popup_xml, -1, NULL);
    gtk_ui_manager_insert_action_group (ui, act_grp, 0);



    /* Maybe we should hide all and show only needed items instead of show every thing and hide some items...
        
        act = gtk_ui_manager_get_action (ui, "/popup/Cut");
        gtk_action_set_visible (act, FALSE);
        act = gtk_ui_manager_get_action (ui, "/popup/Copy");
        gtk_action_set_visible (act, FALSE);
        act = gtk_ui_manager_get_action (ui, "/popup/Paste");
        gtk_action_set_visible (act, FALSE);
        act = gtk_ui_manager_get_action (ui, "/popup/Del");
        gtk_action_set_visible (act, FALSE);
    
    */



    // OpenWith items...
    xml = g_string_new ("");
    
     /***the file has a valid mime-type, its not virtual, it's no a directory ***/
     if (data->same_type
        && first_file_info->type
        && !fm_file_info_is_dir (first_file_info)
        && !data->all_virtual )
    {
        GList *apps = g_app_info_get_all_for_type (first_file_info->type->type);
        GList *l;
        gboolean use_sub = g_list_length (apps) > 5;
        
        g_string_append (xml, "<popup>\n<placeholder name='OPEN_WITH'>\n");
        
        if (use_sub)
            g_string_append (xml, "<menu action='OpenWithMenu'>\n");

        for (l=apps;l;l=l->next)
        {
            GAppInfo *app = l->data;

            /*g_debug ("app %s, executable %s, command %s\n",
                g_app_info_get_name (app),
                g_app_info_get_executable (app),
                g_app_info_get_commandline (app));*/

            gchar  *program_path = g_find_program_in_path (g_app_info_get_executable (app));
            if (!program_path)
                continue;
            
            g_free (program_path);

            act = gtk_action_new (g_app_info_get_id (app), g_app_info_get_name (app), g_app_info_get_description (app),
                                  NULL);
            
            g_signal_connect (act, "activate", G_CALLBACK (on_open_with_app), data);
            gtk_action_set_gicon (act, g_app_info_get_icon (app));
            gtk_action_group_add_action (act_grp, act);
            
            // associate the app info object with the action
            g_object_set_qdata_full (G_OBJECT (act), fm_qdata_id, app, (GDestroyNotify)g_object_unref);
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

    #if 0
	// add custom file actions
	fm_file_menu_add_custom_actions (data, xml, files);
    #endif

    // archiver integration
    if (!hide_archiver)
    {
        g_string_append (xml, "<popup>\n<placeholder name='ARCHIVER'>\n");
        if (data->same_type)
        {
            FmArchiver *archiver = fm_archiver_get_default ();
            if (archiver)
            {
                first_file_info = (FmFileInfo*)fm_list_peek_head (files);
                if (fm_archiver_is_mime_type_supported (archiver, first_file_info->type->type))
                {
                    if (data->cwd && archiver->extract_to_cmd)
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

    // Crappy...
    #if 0
    // Special handling for some virtual filesystems
    g_string_append (xml, "<popup><placeholder name='ph1'>\n");
    if (data->all_virtual)
    {
        // if all of the files are in trash
        if (data->all_trash)
        {
            gboolean can_restore = TRUE;
            GList *l;
            
            // only immediate children of trash:/// can be restored.
            for (l = fm_list_peek_head_link (files);l;l=l->next)
            {
                FmPath *trash_path = FM_FILE_INFO (l->data)->path;
                if (!trash_path->parent || !fm_path_is_trash_root (trash_path->parent))
                {
                    can_restore = FALSE;
                    break;
                }
            }

            if (can_restore)
            {
                act = gtk_action_new ("UnTrash",
                                    _ ("_Restore"),
                                    _ ("Restore trashed files to original paths"),
                            NULL);
                
                g_signal_connect (act, "activate", G_CALLBACK (on_restaure), data);
                
                gtk_action_group_add_action (act_grp, act);
                
                g_string_append (xml, "<menuitem action='UnTrash'/>\n");
            }

            act = gtk_ui_manager_get_action (ui, "/popup/Open");
            gtk_action_set_visible (act, FALSE);
        }
        else
        {
            // do not provide these items for other virtual files
            act = gtk_ui_manager_get_action (ui, "/popup/Cut");
            gtk_action_set_visible (act, FALSE);
            act = gtk_ui_manager_get_action (ui, "/popup/Copy");
            gtk_action_set_visible (act, FALSE);
            act = gtk_ui_manager_get_action (ui, "/popup/Paste");
            gtk_action_set_visible (act, FALSE);
            act = gtk_ui_manager_get_action (ui, "/popup/Del");
            gtk_action_set_visible (act, FALSE);
        }
        
        act = gtk_ui_manager_get_action (ui, "/popup/Rename");
        gtk_action_set_visible (act, FALSE);
    }
    g_string_append (xml, "</placeholder></popup>\n");
    #endif
    
    if (hide_empty_trash_can)
    {
        act = gtk_ui_manager_get_action (ui, "/popup/EmptyTrash");
        gtk_action_set_visible (act, FALSE);
    }
    
    if (hide_file_actions)
    {
        act = gtk_ui_manager_get_action (ui, "/popup/Cut");
        gtk_action_set_visible (act, FALSE);
        act = gtk_ui_manager_get_action (ui, "/popup/Copy");
        gtk_action_set_visible (act, FALSE);
        act = gtk_ui_manager_get_action (ui, "/popup/Paste");
        gtk_action_set_visible (act, FALSE);
        act = gtk_ui_manager_get_action (ui, "/popup/Del");
        gtk_action_set_visible (act, FALSE);
        act = gtk_ui_manager_get_action (ui, "/popup/Rename");
        gtk_action_set_visible (act, FALSE);
        
    }
    
    gtk_ui_manager_add_ui_from_string (ui, xml->str, xml->len, NULL);

    g_string_free (xml, TRUE);

    return data;
}

void fm_file_menu_set_folder_func (FmFileMenu *menu, FmLaunchFolderFunc func, gpointer user_data)
{
    menu->folder_func = func;
    menu->folder_func_data = user_data;
}


/*****************************************************************************************
 *
 *
 *
 *
 ****************************************************************************************/
GtkMenu *fm_file_menu_get_menu (FmFileMenu *menu)
{
    if (menu->menu)
        return menu->menu;
    
    menu->menu = (GtkMenu*) gtk_ui_manager_get_widget (menu->ui, "/popup");
    
    gtk_menu_attach_to_widget (GTK_MENU (menu->menu), GTK_WIDGET (menu->parent), NULL);

    if (menu->auto_destroy)
    {
        g_signal_connect_swapped (menu->menu, "selection-done", G_CALLBACK (fm_file_menu_destroy), menu);
    }
    
    return menu->menu;
}

void fm_file_menu_destroy (FmFileMenu *menu)
{
    if (menu->parent)
        g_object_unref (menu->parent);

    if (menu->menu)
        gtk_widget_destroy ((GtkWidget*) menu->menu);

    if (menu->file_infos)
        fm_list_unref (menu->file_infos);

    if (menu->cwd)
        fm_path_unref (menu->cwd);

    if (menu->act_grp)
        g_object_unref (menu->act_grp);
        
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
    return menu->act_grp;
}

gboolean fm_file_menu_is_single_file_type (FmFileMenu *menu)
{
    return menu->same_type;
}

FmFileInfoList *fm_file_menu_get_file_info_list (FmFileMenu *menu)
{
    return menu->file_infos;
}


/*****************************************************************************************
 * Customs Actions...
 *
 * Add This Later...
 *
 *
 ****************************************************************************************/
#if 0
static void add_custom_action_item (FmFileMenu *data, GString *xml, FmFileActionItem *item)
{
	GtkAction *act;
	if (!item) // separator
	{
		g_string_append (xml, "<separator/>");
		return;
	}

	if (fm_file_action_item_is_action (item))
	{
		if (! (fm_file_action_item_get_target (item) & FM_FILE_ACTION_TARGET_CONTEXT))
			return;
	}

	act = gtk_action_new (fm_file_action_item_get_id (item),
						  fm_file_action_item_get_name (item),
						  fm_file_action_item_get_desc (item),
						  NULL);

	if (fm_file_action_item_is_action (item))
		g_signal_connect (act, "activate", G_CALLBACK (on_custom_action), data);

	gtk_action_set_icon_name (act, fm_file_action_item_get_icon (item));
	gtk_action_group_add_action (data->act_grp, act);
	
    // associate the app info object with the action
	g_object_set_qdata_full (G_OBJECT (act), fm_qdata_id, fm_file_action_item_ref (item),
							 (GDestroyNotify) fm_file_action_item_unref);
	
    if (fm_file_action_item_is_menu (item))
	{
		GList *subitems = fm_file_action_item_get_sub_items (item);
		GList *l;
		
        g_string_append_printf (xml, "<menu action='%s'>", fm_file_action_item_get_id (item));
		
        for (l=subitems; l; l=l->next)
		{
			FmFileActionItem *subitem = FM_FILE_ACTION_ITEM (l->data);
			add_custom_action_item (data, xml, subitem);
		}
		g_string_append (xml, "</menu>");
	}
	else
	{
		g_string_append_printf (xml, "<menuitem action='%s'/>", fm_file_action_item_get_id (item));
	}
}

static void fm_file_menu_add_custom_actions (FmFileMenu *data, GString *xml, FmFileInfoList *files)
{
	GList *files_list = fm_list_peek_head_link (files);
	GList *items = fm_get_actions_for_files (files_list);

    if (items)
    {
		g_string_append (xml, "<popup><placeholder name='ARCHIVER'>");
		GList *l;
		for (l=items; l; l=l->next)
		{
			FmFileActionItem *item = FM_FILE_ACTION_ITEM (l->data);
			add_custom_action_item (data, xml, item);
		}
		g_string_append (xml, "</placeholder></popup>");
    }
	
    g_list_foreach (items, (GSourceFunc) fm_file_action_item_unref, NULL);
	g_list_free (items);
}

static void on_custom_action (GtkAction *act, FmFileMenu *data)
{
    FmFileActionItem *item = FM_FILE_ACTION_ITEM (g_object_get_qdata (act, fm_qdata_id));
    GdkAppLaunchContext *ctx = gdk_app_launch_context_new ();
    GList *files = fm_list_peek_head_link (data->file_infos);
    char **output = NULL;
    gboolean ret;
    gdk_app_launch_context_set_screen (ctx, gtk_widget_get_screen (data->menu));
    gdk_app_launch_context_set_timestamp (ctx, gtk_get_current_event_time ());

	g_debug ("item: %s is activated, id:%s", fm_file_action_item_get_name (item),
			fm_file_action_item_get_id (item));
	ret = fm_file_action_item_launch (item, ctx, files, output);
	if (output)
	{
		fm_show_error (NULL, "output", output);
		g_free (output);
	}
}

#endif


/*****************************************************************************************
 *  Menu Actions
 *
 *
 *
 ****************************************************************************************/
void on_open (GtkAction *action, gpointer user_data)
{
    FmFileMenu *data = (FmFileMenu*) user_data;
    
    g_return_if_fail (data || data->file_infos);
    
    GList *files_list = fm_list_peek_head_link (data->file_infos);
    
    fm_launch_multiple_files (data->parent, NULL, files_list, data->folder_func, data->folder_func_data);
}

void on_restaure (GtkAction *action, gpointer user_data)
{
    FmFileMenu *data = (FmFileMenu*) user_data;
    
    FmPathList *files = fm_path_list_new_from_file_info_list (data->file_infos);
    
    fm_untrash_files (data->parent, files);
    
    fm_list_unref (files);
}

// Forward Declaration...
static void open_with_app (FmFileMenu *data, GAppInfo *app);

void on_open_with_app (GtkAction *action, gpointer user_data)
{
    FmFileMenu *data = (FmFileMenu*) user_data;
    
    GAppInfo *app = (GAppInfo*) g_object_get_qdata (G_OBJECT(action), fm_qdata_id);
    
    g_debug ("%s", gtk_action_get_name (action));
    
    open_with_app (data, app);
}

void on_open_with (GtkAction *action, gpointer user_data)
{
    FmFileMenu *data = (FmFileMenu*) user_data;
    FmFileInfoList *files = data->file_infos;
    
    FmFileInfo *fi = (FmFileInfo*) fm_list_peek_head (files);
    
    FmMimeType *mime_type;

    if (data->same_type && fi->type && fi->type->type)
        mime_type = fi->type;
    else
        mime_type = NULL;

    GAppInfo *app = fm_choose_app_for_mime_type (data->parent, mime_type, TRUE);

    if (!app)
        return;
        
    open_with_app (data, app);
    
    // Add to applications that support this file type.
    if (mime_type)
        g_app_info_add_supports_type (app, mime_type->type, NULL);
    
    g_object_unref (app);
}

static void open_with_app (FmFileMenu *data, GAppInfo *app)
{
    GdkAppLaunchContext *ctx;
    
    FmFileInfoList *files = data->file_infos;
    
    GList *l = fm_list_peek_head_link (files);
    GList *uris = NULL;
    
    int i;
    for (i=0; l; ++i, l=l->next)
    {
        FmFileInfo *fi = (FmFileInfo*) l->data;
        FmPath *path = fi->path;
        
        char *uri = fm_path_to_uri (path);
        uris = g_list_prepend (uris, uri);
    }
    
    uris = g_list_reverse (uris);

    ctx = gdk_display_get_app_launch_context    (gtk_widget_get_display ((GtkWidget*) data->menu));
    gdk_app_launch_context_set_screen           (ctx, gtk_widget_get_screen ((GtkWidget*) data->menu));
    gdk_app_launch_context_set_icon             (ctx, g_app_info_get_icon (app));
    gdk_app_launch_context_set_timestamp        (ctx, gtk_get_current_event_time ());

    fm_app_info_launch_uris (app, uris, (GAppLaunchContext*) ctx, NULL);
    
    g_object_unref (ctx);

    g_list_foreach (uris, (GFunc) g_free, NULL);
    g_list_free (uris);
}

void on_cut (GtkAction *action, gpointer user_data)
{
    FmFileMenu *data = (FmFileMenu*) user_data;
    FmPathList *files;
    
    files = fm_path_list_new_from_file_info_list (data->file_infos);
    fm_clipboard_cut_files ((GtkWidget*) data->parent, files);
    
    fm_list_unref (files);
}

void on_copy (GtkAction *action, gpointer user_data)
{
    FmFileMenu *data = (FmFileMenu*) user_data;
    FmPathList *files;
    
    files = fm_path_list_new_from_file_info_list (data->file_infos);
    fm_clipboard_copy_files ((GtkWidget*) data->parent, files);
    
    fm_list_unref (files);
}

void on_paste (GtkAction *action, gpointer user_data)
{
    FmFileMenu *data = (FmFileMenu*) user_data;
    
    /*** Commented In Original Code... how it works then ? :-D
     
     fm_clipboard_paste_files (data->menu, );
     
     ***/
}

void on_delete (GtkAction *action, gpointer user_data)
{
    FmFileMenu *data = (FmFileMenu*) user_data;
    FmPathList *files;
    files = fm_path_list_new_from_file_info_list (data->file_infos);
    
    fm_trash_or_delete_files (data->parent, files);
    
    fm_list_unref (files);
}

void on_rename (GtkAction *action, gpointer user_data)
{
    // FIXME_axl: Rename should be available only for single files...
    
    FmFileMenu *data = (FmFileMenu*) user_data;
    FmFileInfo *fi = fm_list_peek_head (data->file_infos);
    
    if (fi)
        fm_rename_file (data->parent, fi->path);
    
}

void on_empty_trash (GtkAction *act, gpointer user_data)
{
    FmFileMenu *data = (FmFileMenu*) user_data;
    fm_empty_trash (data->parent);
}

void on_compress (GtkAction *action, gpointer user_data)
{
    FmFileMenu *data = (FmFileMenu*) user_data;
    FmPathList *files;
    FmArchiver *archiver = fm_archiver_get_default ();
    
    if (!archiver)
        return;
        
    GAppLaunchContext *ctx = (GAppLaunchContext*) gdk_display_get_app_launch_context (gdk_display_get_default ());
    
    files = fm_path_list_new_from_file_info_list (data->file_infos);
    fm_archiver_create_archive (archiver, ctx, files);
    
    fm_list_unref (files);
    g_object_unref (ctx);
}

void on_extract_here (GtkAction *action, gpointer user_data)
{
    FmFileMenu *data = (FmFileMenu*) user_data;
    FmPathList *files;
    
    FmArchiver *archiver = fm_archiver_get_default ();
    if (!archiver)
        return;
    
    GAppLaunchContext *ctx = (GAppLaunchContext*) gdk_display_get_app_launch_context (gdk_display_get_default ());
    
    files = fm_path_list_new_from_file_info_list (data->file_infos);
    fm_archiver_extract_archives_to (archiver, ctx, files, data->cwd);
    
    fm_list_unref (files);
    g_object_unref (ctx);
}

void on_extract_to (GtkAction *action, gpointer user_data)
{
    FmFileMenu *data = (FmFileMenu*) user_data;
    FmPathList *files;
    
    FmArchiver *archiver = fm_archiver_get_default ();
    
    if (!archiver)
        return;
    
    GAppLaunchContext *ctx = (GAppLaunchContext*) gdk_display_get_app_launch_context (gdk_display_get_default ());

    files = fm_path_list_new_from_file_info_list (data->file_infos);
    fm_archiver_extract_archives (archiver, ctx, files);

    fm_list_unref (files);
    g_object_unref (ctx);
}

void on_properties (GtkAction *action, gpointer user_data)
{
    FmFileMenu *data = (FmFileMenu*) user_data;
    
    g_return_if_fail (data || data->file_infos);
    
    FmFileInfoList *files = data->file_infos;
    
    uint flags = fm_file_info_list_get_flags (files);
    
    if ((flags & FM_PATH_IS_TRASH) || (flags & FM_PATH_IS_VIRTUAL))
    {
        // TODO_axl: printf ("NEEDS A VIRTUAL DIALOG !!!!!\n");
        return;
    }
    
    fm_show_file_properties (data->parent, files);
}


