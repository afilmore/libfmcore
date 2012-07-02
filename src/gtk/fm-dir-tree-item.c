/***********************************************************************************************************************
 * 
 *      fm-dir-tree-item.c
 * 
 *      Copyright 2010 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
 *      Copyright 2012 Axel FILMORE <axel.filmore@gmail.com>
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
 *      Purpose: 
 * 
 * 
 * 
 **********************************************************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n-lib.h>
#include <string.h>

#include "fm-debug.h"

#include "fm-dir-tree-item.h"
#include "fm-icon-pixbuf.h"

#include "fm-utils.h"


// Forward declarations...
static inline void  fm_dir_tree_item_free_folder    (GList *item_list);

static void         on_folder_loaded                (FmFolder *folder, GList *item_list);
static void         on_folder_files_added           (FmFolder *folder, GSList *files, GList *item_list);
static void         on_folder_files_removed         (FmFolder *folder, GSList *files, GList *item_list);
static void         on_folder_files_changed         (FmFolder *folder, GSList *files, GList *item_list);


/*********************************************************************
 *  Creation/Destruction...
 * 
 * 
 ********************************************************************/
inline FmDirTreeItem *fm_dir_tree_item_new (FmDirTreeModel *model, GList *parent_node, FmFileInfo *file_info)
{
    FmDirTreeItem *item = g_slice_new0 (FmDirTreeItem);
    
    item->model = model;
    item->parent = parent_node;
    item->file_info = file_info ? fm_file_info_ref (file_info) : NULL;
    
    return item;
}

inline void fm_dir_tree_item_free (FmDirTreeItem *dir_tree_item)
{
    if (dir_tree_item->file_info)
        fm_file_info_unref (dir_tree_item->file_info);
    
    //~ if (dir_tree_item->icon)
        //~ g_object_unref (dir_tree_item->icon);

    if (dir_tree_item->fm_icon)
        fm_icon_unref (dir_tree_item->fm_icon);

    // In most cases this should have been freed in the list free folder function...
    if (dir_tree_item->folder)
        g_object_unref (dir_tree_item->folder);

    if (dir_tree_item->children)
    {
        fm_foreach (dir_tree_item->children, (GFunc) fm_dir_tree_item_free_l, NULL);
        g_list_free (dir_tree_item->children);
    }
    
    if (dir_tree_item->hidden_children)
    {
        g_list_foreach (dir_tree_item->hidden_children, (GFunc) fm_dir_tree_item_free, NULL);
        g_list_free (dir_tree_item->hidden_children);
    }
    
    g_slice_free (FmDirTreeItem, dir_tree_item);
}

void fm_dir_tree_item_free_l (GList *item_list)
{
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) item_list->data;
    
    fm_dir_tree_item_free_folder (item_list);
    
    fm_dir_tree_item_free (dir_tree_item);
}


/*********************************************************************
 *  ...
 * 
 * 
 ********************************************************************/
GdkPixbuf *fm_dir_tree_item_get_pixbuf (FmDirTreeItem *dir_tree_item, int icon_size)
{
    g_return_val_if_fail (dir_tree_item->file_info, NULL);
    
    //~ if (!dir_tree_item->icon)
        //~ dir_tree_item->icon = fm_icon_get_pixbuf (fm_file_info_get_fm_icon (dir_tree_item->file_info), MAX (icon_size, 16));
    
    if (dir_tree_item->fm_icon)
        return fm_icon_get_pixbuf (dir_tree_item->fm_icon, MAX (icon_size, 16));
    
    return fm_icon_get_pixbuf (fm_file_info_get_fm_icon (dir_tree_item->file_info), MAX (icon_size, 16));
}


/*********************************************************************
 *  Folder Functions/Signals Callbacks...
 * 
 * 
 ********************************************************************/
FmFolder *fm_dir_tree_item_set_folder (GList *item_list)
{
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) item_list->data;
    
    FmPath *path;
    if (fm_file_info_is_mountable (dir_tree_item->file_info))
    {
        path = fm_path_new_for_str (fm_file_info_get_target (dir_tree_item->file_info));
    }
    else
    {
        path = fm_path_ref (fm_file_info_get_path (dir_tree_item->file_info));
    }
    
    g_return_val_if_fail (path != NULL, NULL);
    
    FmFolder *folder = fm_folder_get (path);
    fm_path_unref (path);
    
    dir_tree_item->folder = folder;

    // Associate the data with loaded handler...
    g_signal_connect (folder, "loaded",         G_CALLBACK (on_folder_loaded),          item_list);
    g_signal_connect (folder, "files-added",    G_CALLBACK (on_folder_files_added),     item_list);
    g_signal_connect (folder, "files-removed",  G_CALLBACK (on_folder_files_removed),   item_list);
    g_signal_connect (folder, "files-changed",  G_CALLBACK (on_folder_files_changed),   item_list);
    
    return folder;
}

static inline void fm_dir_tree_item_free_folder (GList *item_list)
{
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) item_list->data;
    
    if (!dir_tree_item->folder)
        return;
        
    FmFolder *folder = dir_tree_item->folder;
    
    g_signal_handlers_disconnect_by_func (folder, on_folder_loaded,         item_list);
    g_signal_handlers_disconnect_by_func (folder, on_folder_files_added,    item_list);
    g_signal_handlers_disconnect_by_func (folder, on_folder_files_removed,  item_list);
    g_signal_handlers_disconnect_by_func (folder, on_folder_files_changed,  item_list);
    
    g_object_unref (folder);
    
    dir_tree_item->folder = NULL;
}

void fm_dir_tree_item_on_folder_loaded (FmDirTreeItem *dir_tree_item)
{
    FmDirTreeModel *model = dir_tree_item->model;
    GList *place_holder = dir_tree_item->children;
    
    g_return_if_fail (place_holder != NULL);
    
    // If we have loaded sub dirs, remove the place holder...
    if (place_holder->next)
    {
        fm_dir_tree_model_remove_item (model, place_holder);
    }
    
    /** If we have no sub dirs, leave the place holder and let it show "Empty" 
    else
    {
        GtkTreeIter it;
        
        GtkTreePath *tp = fm_dir_tree_model_item_to_tree_path (model, place_holder);
        
        fm_dir_tree_model_item_to_tree_iter (model, place_holder, &it);
        
        gtk_tree_model_row_changed ((GtkTreeModel*) model, tp, &it);
        
        gtk_tree_path_free (tp);
    }**/
}

static void on_folder_loaded (FmFolder *folder, GList *item_list)
{
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) item_list->data;
    
    //TREEVIEW_DEBUG ("TREEVIEW_DEBUG: FmDirTreeItem: on_folder_loaded: %s loaded\n", fm_path_display_basename (folder->dir_path));
    fm_dir_tree_item_on_folder_loaded (dir_tree_item);
    
    /**
    FmDirTreeModel *model = dir_tree_item->model;
    GList *place_holder_l = dir_tree_item->children;
    
    // If we have loaded sub dirs, remove the place holder...
    if (place_holder_l->next)
    {
        fm_dir_tree_model_remove_item (model, place_holder_l);
    }
    
     If we have no sub dirs, leave the place holder and let it show "Empty" 
    else
    {
        GtkTreeIter it;
        
        GtkTreePath *tp = fm_dir_tree_model_item_to_tree_path (model, place_holder_l);
        
        fm_dir_tree_model_item_to_tree_iter (model, place_holder_l, &it);
        
        gtk_tree_model_row_changed ((GtkTreeModel*) model, tp, &it);
        
        gtk_tree_path_free (tp);
    }**/
}

static void on_folder_files_added (FmFolder *folder, GSList *files, GList *item_list)
{
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) item_list->data;
    FmDirTreeModel *model = dir_tree_item->model;
    
    fm_dir_tree_item_load_folder (folder, files, item_list, TRUE);
}

void fm_dir_tree_item_load_folder (FmFolder *folder, GSList *files, GList *item_list, gboolean check_exits)
{
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) item_list->data;
    FmDirTreeModel *model = dir_tree_item->model;
    
    GtkTreePath *tree_path = fm_dir_tree_model_item_to_tree_path (model, item_list);
    
    GSList *l;
    for (l = files; l; l = l->next)
    {
        FmFileInfo *file_info = FM_FILE_INFO (l->data);
        
        // Check if we have a directory, a drive or a virtual item...
        FmPath *path = fm_file_info_get_path (file_info);
        if (!fm_file_info_is_dir (file_info) && !fm_file_info_is_mountable (file_info) && !fm_path_is_virtual (path))
        {
            DEBUG ("%s\n", fm_path_get_basename (path));
            continue;
        }
        
        // Ensure that the file is not yet in our model
        GList *new_item_list = NULL;
        if (check_exits)
        {
            new_item_list = fm_dir_tree_model_children_by_name (model, dir_tree_item->children,
                                                                file_info->path->name, NULL);
        }
            
        if (new_item_list)
        {
            TREEVIEW_DEBUG ("TREEVIEW_DEBUG: CRITICAL: on_folder_files_added: %s already in the model !!!\n",
                            file_info->path->name);
        }
        else
        {
            new_item_list = fm_dir_tree_model_insert_file_info (model, item_list, tree_path, file_info);
        }
    }
    
    gtk_tree_path_free (tree_path);
}

static void on_folder_files_removed (FmFolder *folder, GSList *files, GList *item_list)
{
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) item_list->data;
    FmDirTreeModel *model = dir_tree_item->model;
    
    GSList *l;

    for (l = files; l; l = l->next)
    {
        FmFileInfo *file_info = FM_FILE_INFO (l->data);
        
        GList *rm_item_list = fm_dir_tree_model_children_by_name (model, dir_tree_item->children, file_info->path->name, NULL);
        
        if (rm_item_list)
            fm_dir_tree_model_remove_item (model, rm_item_list);
    }
}

static void on_folder_files_changed (FmFolder *folder, GSList *files, GList *item_list)
{
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) item_list->data;
    FmDirTreeModel *model = dir_tree_item->model;

    GtkTreePath *tree_path = fm_dir_tree_model_item_to_tree_path (model, item_list);

    // NO_DEBUG ("fm-dir-tree-item: on_folder_files_changed: files changed!!\n");

    GSList *l;
    for (l = files; l; l = l->next)
    {
        FmFileInfo *file_info = FM_FILE_INFO (l->data);
        
        int idx;
        GList *changed_item_list = fm_dir_tree_model_children_by_name (model, dir_tree_item->children, file_info->path->name, &idx);
        
        // g_debug ("changed file: %s", file_info->path->name); 
        
        if (changed_item_list)
        {
            FmDirTreeItem *changed_item = (FmDirTreeItem*) changed_item_list->data;
            if (changed_item->file_info)
                fm_file_info_unref (changed_item->file_info);
            
            changed_item->file_info = fm_file_info_ref (file_info);
            
            // FIXME_pcm: inform gtk tree view about the change 

            // Check Subdirectories: check if we have sub folder 
            if (model->check_subdir)
                fm_dir_tree_model_item_queue_subdir_check (model, changed_item_list);
        }
    }
    
    gtk_tree_path_free (tree_path);
}





