/***********************************************************************************************************************
 * 
 *      fm-dir-tree-view.c
 *
 *      Copyright 2011 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
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
 *      Purpose: 
 * 
 * 
 * 
 **********************************************************************************************************************/
#include "fm-dir-tree-view.h"
#include "fm-dir-tree-model.h"
#include "fm-cell-renderer-pixbuf.h"
#include "fm-dir-tree-item.h"

#include "fm-debug.h"

#include "fm-mount.h"

#include <gdk/gdkkeysyms.h>
#include <string.h>


G_DEFINE_TYPE(FmDirTreeView, fm_dir_tree_view, GTK_TYPE_TREE_VIEW)

enum
{
    DIRECTORY_CHANGED,
    N_SIGNALS
};

static guint signals [N_SIGNALS];


// Forward declarations...
static void     fm_dir_tree_view_finalize           (GObject *object);

static gboolean fm_dir_tree_view_select_function    (GtkTreeSelection *selection, GtkTreeModel *model,
                                                     GtkTreePath *path, gboolean path_currently_selected,
                                                     gpointer data);

static void     on_folder_loaded                    (FmFolder *folder, FmDirTreeView *tree_view);
static void     expand_pending_path                 (FmDirTreeView *tree_view, GtkTreeModel *model,
                                                     GtkTreeIter *parent_iter);
static void     cancel_pending_chdir                (FmDirTreeView *tree_view);
static void     on_sel_changed                      (GtkTreeSelection *tree_selection, FmDirTreeView *tree_view);
static void     emit_chdir_if_needed                (FmDirTreeView *tree_view, GtkTreeSelection *tree_selection,
                                                     int button);
static gboolean on_test_expand_row                  (GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path);
static gboolean on_key_press_event                  (GtkWidget *widget, GdkEventKey *event_key);
static void     on_row_collapsed                    (GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path);
static void     on_row_activated                    (GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *col);

static gboolean find_iter_by_path                   (GtkTreeModel *model, GtkTreeIter *it, GtkTreeIter *parent,
                                                     FmPath *path);


/*****************************************************************************************
 *  Creation/destruction...
 * 
 * 
 ****************************************************************************************/
GObject *fm_dir_tree_view_new (void)
{
    return g_object_new (FM_TYPE_DIR_TREE_VIEW, NULL);
}

static void fm_dir_tree_view_class_init (FmDirTreeViewClass *klass)
{
    GObjectClass *g_object_class = G_OBJECT_CLASS (klass);
    GtkTreeViewClass *tree_view_class = GTK_TREE_VIEW_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    g_object_class->finalize = fm_dir_tree_view_finalize;

    widget_class->key_press_event =         on_key_press_event;
    // widget_class->button_press_event =   on_button_press_event;

    tree_view_class->test_expand_row =      on_test_expand_row;
    // tree_view_class->row_expanded =      on_row_expanded;
    tree_view_class->row_collapsed =        on_row_collapsed;
    tree_view_class->row_activated =        on_row_activated;

    signals [DIRECTORY_CHANGED] = g_signal_new ("directory-changed",
                                                G_TYPE_FROM_CLASS (klass),
                                                G_SIGNAL_RUN_LAST,
                                                G_STRUCT_OFFSET (FmDirTreeViewClass, directory_changed),
                                                NULL, NULL,
                                                g_cclosure_marshal_VOID__UINT_POINTER,
                                                G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_POINTER);
}

static void fm_dir_tree_view_init (FmDirTreeView *tree_view)
{
    GtkTreeSelection *tree_selection;
    GtkTreeViewColumn *col;
    GtkCellRenderer *render;
    gtk_tree_view_set_headers_visible ((GtkTreeView*) tree_view, FALSE);
    
    // gtk_tree_view_set_enable_tree_lines (tree_view, TRUE);

    col = gtk_tree_view_column_new ();
    
    render = fm_cell_renderer_pixbuf_new ();
    fm_cell_renderer_pixbuf_set_fixed_size (FM_CELL_RENDERER_PIXBUF (render), 16, 16);
    
    gtk_tree_view_column_pack_start (col, render, FALSE);
    gtk_tree_view_column_set_attributes (col, render, "pixbuf", FM_DIR_TREE_MODEL_COL_ICON,
                                                      "info",   FM_DIR_TREE_MODEL_COL_INFO, NULL);

    render = gtk_cell_renderer_text_new ();
    gtk_tree_view_column_pack_start (col, render, TRUE);
    gtk_tree_view_column_set_attributes (col, render, "text",   FM_DIR_TREE_MODEL_COL_DISP_NAME, NULL);

    gtk_tree_view_append_column ((GtkTreeView*) tree_view, col);

    tree_selection = gtk_tree_view_get_selection ((GtkTreeView*) tree_view);
    gtk_tree_selection_set_mode (tree_selection, GTK_SELECTION_BROWSE);
    
    gtk_tree_selection_set_select_function (tree_selection, fm_dir_tree_view_select_function, tree_view, NULL);
    
    g_signal_connect (tree_selection, "changed", G_CALLBACK (on_sel_changed), tree_view);
}

static gboolean fm_dir_tree_view_select_function (GtkTreeSelection *selection, GtkTreeModel *model, GtkTreePath *path,
                                                  gboolean path_currently_selected, gpointer data)
{
    GtkTreeIter it;
    fm_dir_tree_model_get_iter (model, &it, path);
    
    GList *node = (GList*) it.user_data;
    if (!node)
        return FALSE;
    
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) node->data;
    
    return (dir_tree_item->file_info != NULL);
}

static void fm_dir_tree_view_finalize (GObject *object)
{
    FmDirTreeView *tree_view;

    g_return_if_fail (object != NULL);
    g_return_if_fail (FM_IS_DIR_TREE_VIEW (object));

    tree_view = FM_DIR_TREE_VIEW (object);
    
    if (G_UNLIKELY (tree_view->paths_to_expand))
        cancel_pending_chdir (tree_view);

    if (tree_view->current_directory)
        fm_path_unref (tree_view->current_directory);

    G_OBJECT_CLASS (fm_dir_tree_view_parent_class)->finalize (object);
}


/*****************************************************************************************
 *  Set/Get The Current Directory...
 * 
 * 
 ****************************************************************************************/
void fm_dir_tree_view_set_current_directory (FmDirTreeView *tree_view, FmPath *path)
{
    GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (tree_view));
    
    if (!model || fm_path_equal (tree_view->current_directory, path))
    {
        //NO_DEBUG ("fm_dir_tree_view_set_current_directory: same path !!!\n");
        return;
    }
    
    GtkTreeIter it;
    if (!gtk_tree_model_get_iter_first (model, &it))
    {
        //NO_DEBUG ("fm_dir_tree_view_set_current_directory: can't find first iter !!!\n");
        return;
    }

    // Find a root item containing this path...
    FmPath *root;
    do {
        
        gtk_tree_model_get (model, &it, FM_DIR_TREE_MODEL_COL_PATH, &root, -1);
        
        if (fm_path_has_prefix (path, root))
        {
            //NO_DEBUG ("fm_dir_tree_view_set_current_directory: root item found !!!\n");
            break;
        }
        
        root = NULL;
    
    } while (gtk_tree_model_iter_next (model, &it));
    
    
    // Cancel previous pending tree expansion...
    if (tree_view->paths_to_expand)
        cancel_pending_chdir (tree_view);

    // Add path elements one by one to a list...
    do {
        
        tree_view->paths_to_expand = g_slist_prepend (tree_view->paths_to_expand, fm_path_ref (path));
        
        char *temp_path = fm_path_to_str (path);
        TREEVIEW_DEBUG ("TREEVIEW_DEBUG: fm_dir_tree_view_set_current_directory: path to expend = %s\n", temp_path);
        g_free (temp_path);
        
        if (fm_path_equal (path, root))
        {
            //NO_DEBUG ("fm_dir_tree_view_set_current_directory: fm_path_equal (path, root) !!!\n");
            break;
        }
        
        path = path->parent;
    
    } while (path);

    expand_pending_path (tree_view, model, NULL);
}

FmPath *fm_dir_tree_view_get_current_directory (FmDirTreeView *tree_view)
{
    return tree_view->current_directory;
}


/*****************************************************************************************
 * ...
 * 
 * 
 ****************************************************************************************/
static void cancel_pending_chdir (FmDirTreeView *tree_view)
{
    if (tree_view->cur_expanded_folder)
    {
        g_signal_handlers_disconnect_by_func (tree_view->cur_expanded_folder, on_folder_loaded, tree_view);
        g_object_unref (tree_view->cur_expanded_folder);
        tree_view->cur_expanded_folder = NULL;
    }
    
    memset (&tree_view->cur_expanded_it, 0, sizeof (GtkTreeIter));

    g_slist_foreach (tree_view->paths_to_expand, (GFunc) fm_path_unref, NULL);
    g_slist_free (tree_view->paths_to_expand);
    
    tree_view->paths_to_expand = NULL;
}

static void expand_pending_path (FmDirTreeView *tree_view, GtkTreeModel *model, GtkTreeIter *parent_iter)
{
    g_return_if_fail (tree_view->paths_to_expand);
    
    FmPath *path = FM_PATH (tree_view->paths_to_expand->data);

    TREEVIEW_DEBUG ("TREEVIEW_DEBUG: expand_pending_path: expend %s\n", fm_path_display_basename (path));
    
    GtkTreeIter it;
    if (!find_iter_by_path (model, &it, parent_iter, path))
    {
        TREEVIEW_DEBUG ("TREEVIEW_DEBUG: expand_pending_path: find_iter_by_path () returned NULL\n");
        return;
    }
    
    tree_view->cur_expanded_it = it;

    // It now points to the root item...
    GtkTreePath *tree_path = gtk_tree_model_get_path (model, &it);
    
    char *temp_tp = gtk_tree_path_to_string (tree_path);
    TREEVIEW_DEBUG ("TREEVIEW_DEBUG: expand_pending_path: expend tree path %s\n", temp_tp);
    g_free (temp_tp);
    
    gtk_tree_view_expand_row ((GtkTreeView*) tree_view, tree_path, FALSE);
    gtk_tree_path_free (tree_path);
    
    // After being expanded, the row now owns a FmFolder object.
    FmFolder *folder;
    gtk_tree_model_get (model, &it, FM_DIR_TREE_MODEL_COL_FOLDER, &folder, -1);
    
    g_return_if_fail (folder != NULL);
    
    // This should not happen, otherwise it's a bug...
    if (tree_view->cur_expanded_folder)
        g_object_unref (tree_view->cur_expanded_folder);
    
    tree_view->cur_expanded_folder = FM_FOLDER (g_object_ref (folder));

    TREEVIEW_DEBUG ("TREEVIEW_DEBUG: expand_pending_path: %s expended\n", fm_path_display_basename (path));
    
    // The folder is already loaded...
    if (fm_folder_get_is_loaded (folder))
        on_folder_loaded (folder, tree_view);
    else
        g_signal_connect (folder, "loaded", G_CALLBACK (on_folder_loaded), tree_view);
}

static gboolean find_iter_by_path (GtkTreeModel *model, GtkTreeIter *it, GtkTreeIter *parent, FmPath *path)
{
    if (gtk_tree_model_iter_children (model, it, parent))
    {
        do {
            FmPath *path2;
            
            gtk_tree_model_get (model, it, FM_DIR_TREE_MODEL_COL_PATH, &path2, -1);
            
            if (path2 && fm_path_equal (path, path2))
                return TRUE;
                
        } while (gtk_tree_model_iter_next (model, it));
    }
    
    return FALSE;
}


/*****************************************************************************************
 * ...
 * 
 * 
 ****************************************************************************************/
static void on_folder_loaded (FmFolder *folder, FmDirTreeView *tree_view)
{
    TREEVIEW_DEBUG ("TREEVIEW_DEBUG: on_folder_loaded: %s loaded\n", fm_path_display_basename (folder->dir_path));
    
    // disconnect the handler since we only need it once
    g_signal_handlers_disconnect_by_func (folder, on_folder_loaded, tree_view);
    g_object_unref (tree_view->cur_expanded_folder);
    tree_view->cur_expanded_folder = NULL;

    // After the folder is loaded, the files should have been added to the model...

    // remove the expanded path from pending list
    FmPath *path = FM_PATH (tree_view->paths_to_expand->data);
    tree_view->paths_to_expand = g_slist_delete_link (tree_view->paths_to_expand, tree_view->paths_to_expand);

    // continue expanding next pending path
    if (tree_view->paths_to_expand)
    {
        expand_pending_path (tree_view, gtk_tree_view_get_model ((GtkTreeView*) tree_view),
                             &tree_view->cur_expanded_it);
    }
    
    // this is the last one and we're done, select the item
    else
    {
        GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (tree_view));
        
        GtkTreeSelection *tree_selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree_view));
        
        GtkTreePath *tree_path = gtk_tree_model_get_path (model, &tree_view->cur_expanded_it);
        gtk_tree_selection_select_iter (tree_selection, &tree_view->cur_expanded_it);
        
        gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (tree_view), tree_path, NULL, TRUE, 0.5, 0.5);
        gtk_tree_view_set_cursor (GTK_TREE_VIEW (tree_view), tree_path, NULL, FALSE);
        
        gtk_tree_path_free (tree_path);
        memset (&tree_view->cur_expanded_it, 0, sizeof (tree_view->cur_expanded_it));
    }

    fm_path_unref (path);
}


/*****************************************************************************************
 * ...
 * 
 * 
 ****************************************************************************************/
static gboolean on_test_expand_row (GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path)
{
    FmDirTreeView *dir_tree_view = FM_DIR_TREE_VIEW (tree_view);
    FmDirTreeModel *model = FM_DIR_TREE_MODEL (gtk_tree_view_get_model (tree_view));
    
    fm_dir_tree_model_expand_row (model, iter, path);

    // if a pending selection via previous call to chdir is in progress, cancel it.
    if (dir_tree_view->paths_to_expand
        && dir_tree_view->cur_expanded_it.user_data != iter->user_data)
    {
        cancel_pending_chdir (dir_tree_view);
    }

    return FALSE;
}

/**
static void on_row_expanded (GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path)
{
    FmDirTreeView *tree_view = FM_DIR_TREE_VIEW (tree_view);
}
**/

static void on_row_collapsed (GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path)
{
    FmDirTreeModel *model = FM_DIR_TREE_MODEL (gtk_tree_view_get_model (tree_view));
    fm_dir_tree_model_collapse_row (model, iter, path);
}

static void on_row_activated (GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *col)
{
    if (gtk_tree_view_row_expanded (tree_view, path))
        gtk_tree_view_collapse_row (tree_view, path);
    else
        gtk_tree_view_expand_row (tree_view, path, FALSE);
}


/*****************************************************************************************
 * ...
 * 
 * 
 ****************************************************************************************/
static gboolean on_key_press_event (GtkWidget *widget, GdkEventKey *event_key)
{
    GtkTreeView         *tree_view = GTK_TREE_VIEW (widget);
    
    GtkTreeSelection    *tree_selection;
    GtkTreeModel        *model;
    GtkTreeIter         it;
    GtkTreePath         *tree_path;

    switch (event_key->keyval)
    {
        case GDK_KEY_Left:
        {
            tree_selection = gtk_tree_view_get_selection (tree_view);
            if (gtk_tree_selection_get_selected (tree_selection, &model, &it))
            {
                tree_path = gtk_tree_model_get_path (model, &it);
                if (gtk_tree_view_row_expanded (tree_view, tree_path))
                    gtk_tree_view_collapse_row (tree_view, tree_path);
                else
                {
                    gtk_tree_path_up (tree_path);
                    gtk_tree_view_set_cursor (tree_view, tree_path, NULL, FALSE);
                    gtk_tree_selection_select_path (tree_selection, tree_path);
                }
                gtk_tree_path_free (tree_path);
            }
        }
        break;
        
        case GDK_KEY_Right:
        {
            tree_selection = gtk_tree_view_get_selection (tree_view);
            if (gtk_tree_selection_get_selected (tree_selection, &model, &it))
            {
                tree_path = gtk_tree_model_get_path (model, &it);
                gtk_tree_view_expand_row (tree_view, tree_path, FALSE);
                gtk_tree_path_free (tree_path);
            }
        }
        break;
    }
    
    return GTK_WIDGET_CLASS (fm_dir_tree_view_parent_class)->key_press_event (widget, event_key);
}

static void on_sel_changed (GtkTreeSelection *tree_selection, FmDirTreeView *tree_view)
{
    // if a pending selection via previous call to chdir is in progress, cancel it.
    if (tree_view->paths_to_expand)
        cancel_pending_chdir (tree_view);

    emit_chdir_if_needed (tree_view, tree_selection, 1);
}

static void emit_chdir_if_needed (FmDirTreeView *tree_view, GtkTreeSelection *tree_selection, int button)
{
    GtkTreeIter     it;
    GtkTreeModel    *model;
    
    if (!gtk_tree_selection_get_selected (tree_selection, &model, &it))
        return;
    
    FmFileInfo *file_info;
    
    FmPath *path;
    gtk_tree_model_get (model, &it, FM_DIR_TREE_MODEL_COL_PATH, &path, FM_DIR_TREE_MODEL_COL_INFO, &file_info, -1);

    if (path && tree_view->current_directory && fm_path_equal (path, tree_view->current_directory))
        return;

    
    if (fm_file_info_is_mountable (file_info))
    {
        char *target = fm_file_info_get_target (file_info);
        
        if (target)
        {
            path = fm_path_new_for_str (target);
            if (tree_view->current_directory)
                fm_path_unref (tree_view->current_directory);

            tree_view->current_directory = path;

            g_signal_emit (tree_view, signals [DIRECTORY_CHANGED], 0, button, tree_view->current_directory);
            
            return;
        }
    }
    
    if (tree_view->current_directory)
        fm_path_unref (tree_view->current_directory);

    tree_view->current_directory = G_LIKELY (path) ? fm_path_ref (path) : NULL;

    g_signal_emit (tree_view, signals [DIRECTORY_CHANGED], 0, button, tree_view->current_directory);
}




