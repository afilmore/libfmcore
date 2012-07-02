/***********************************************************************************************************************
 * 
 *      fm-dir-tree-model.c
 * 
 *      Copyright 2010 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
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

#include "fm-dir-tree-model.h"

#include "fm-debug.h"
#include "fm-utils.h"

#include "fm-dir-tree-item.h"


static GType column_types [N_FM_DIR_TREE_MODEL_COLS];


static void fm_dir_tree_model_tree_model_init (GtkTreeModelIface *iface);

G_DEFINE_TYPE_WITH_CODE (FmDirTreeModel,
                         fm_dir_tree_model, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, fm_dir_tree_model_tree_model_init)
                         /*** 
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_DRAG_SOURCE, fm_dir_tree_model_drag_source_init)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_DRAG_DEST, fm_dir_tree_model_drag_dest_init)
                         ***/
                       )


// Forward Declarations...
static void fm_dir_tree_model_finalize                      (GObject *object);
static void on_theme_changed                                (GtkIconTheme *theme, FmDirTreeModel *dir_tree_model);

static void fm_dir_tree_model_add_place_holder_child_item   (FmDirTreeModel *dir_tree_model, GList *parent_node,
                                                             GtkTreePath *tree_path, gboolean emit_signal);
                                                             
static GList *fm_dir_tree_model_insert_sorted                 (FmDirTreeModel *dir_tree_model, GList *parent_node,
                                                             GtkTreePath *tree_path, FmDirTreeItem *new_item);




static void fm_dir_tree_model_item_reload_icon              (FmDirTreeModel *dir_tree_model,
                                                             FmDirTreeItem *dir_tree_item,
                                                             GtkTreePath *tree_path);

static gboolean subdir_check_job                            (GIOSchedulerJob *job, GCancellable *cancellable,
                                                             gpointer user_data);

static gboolean subdir_check_remove_place_holder            (FmDirTreeModel *dir_tree_model);
static gboolean subdir_check_finish                         (FmDirTreeModel *dir_tree_model);

// GtkTreeModel Implementation...
static GtkTreeModelFlags fm_dir_tree_model_get_flags        (GtkTreeModel *tree_model);
static gint fm_dir_tree_model_get_n_columns                 (GtkTreeModel *tree_model);
static GType fm_dir_tree_model_get_column_type              (GtkTreeModel *tree_model, gint index);
static GtkTreePath *fm_dir_tree_model_get_path              (GtkTreeModel *tree_model, GtkTreeIter *iter);
static void fm_dir_tree_model_get_value                     (GtkTreeModel *tree_model, GtkTreeIter *iter,
                                                             gint column, GValue *value);

// GtkTreeModel Iterators...
static gboolean fm_dir_tree_model_iter_next                 (GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean fm_dir_tree_model_iter_has_child            (GtkTreeModel *tree_model, GtkTreeIter *iter);
static gint fm_dir_tree_model_iter_n_children               (GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean fm_dir_tree_model_iter_children             (GtkTreeModel *tree_model,
                                                             GtkTreeIter *iter,
                                                             GtkTreeIter *parent);
static gboolean fm_dir_tree_model_iter_nth_child            (GtkTreeModel *tree_model, 
                                                             GtkTreeIter *iter,
                                                             GtkTreeIter *parent,
                                                             gint n);
static gboolean fm_dir_tree_model_iter_parent               (GtkTreeModel *tree_model,
                                                             GtkTreeIter *iter,
                                                             GtkTreeIter *child);


static void fm_dir_tree_model_remove_all_children (FmDirTreeModel *dir_tree_model, GList *item_list,
                                                   GtkTreePath *tree_path);


/*****************************************************************************************
 *  ...
 * 
 * 
 ****************************************************************************************/
FmDirTreeModel *fm_dir_tree_model_new ()
{
    FmDirTreeModel *dir_tree_model = (FmDirTreeModel*) g_object_new (FM_TYPE_DIR_TREE_MODEL, NULL);
    
    return dir_tree_model;
}

static void fm_dir_tree_model_class_init (FmDirTreeModelClass *klass)
{
    GObjectClass *g_object_class;
    g_object_class = G_OBJECT_CLASS (klass);
    g_object_class->finalize = fm_dir_tree_model_finalize;
}

static void fm_dir_tree_model_tree_model_init (GtkTreeModelIface *iface)
{
    iface->get_flags =          fm_dir_tree_model_get_flags;
    iface->get_n_columns =      fm_dir_tree_model_get_n_columns;
    iface->get_column_type =    fm_dir_tree_model_get_column_type;
    iface->get_iter =           fm_dir_tree_model_get_iter;
    iface->get_path =           fm_dir_tree_model_get_path;
    iface->get_value =          fm_dir_tree_model_get_value;
    iface->iter_next =          fm_dir_tree_model_iter_next;
    iface->iter_children =      fm_dir_tree_model_iter_children;
    iface->iter_has_child =     fm_dir_tree_model_iter_has_child;
    iface->iter_n_children =    fm_dir_tree_model_iter_n_children;
    iface->iter_nth_child =     fm_dir_tree_model_iter_nth_child;
    iface->iter_parent =        fm_dir_tree_model_iter_parent;

    column_types [FM_DIR_TREE_MODEL_COL_ICON] =         GDK_TYPE_PIXBUF;
    column_types [FM_DIR_TREE_MODEL_COL_DISP_NAME] =    G_TYPE_STRING;
    column_types [FM_DIR_TREE_MODEL_COL_INFO] =         G_TYPE_POINTER;
    column_types [FM_DIR_TREE_MODEL_COL_PATH] =         G_TYPE_POINTER;
    column_types [FM_DIR_TREE_MODEL_COL_FOLDER] =       G_TYPE_POINTER;
}


/*****************************************************************************************
 *  ...
 * 
 * 
 ****************************************************************************************/
static void fm_dir_tree_model_init (FmDirTreeModel *dir_tree_model)
{
    dir_tree_model->stamp = g_random_int ();
    
    // Set default values...
    dir_tree_model->icon_size = 16;
    dir_tree_model->show_hidden = FALSE;
    dir_tree_model->show_symlinks = FALSE;
    dir_tree_model->check_subdir = TRUE;
    
    // Check Subdirectories job...
    g_queue_init (&dir_tree_model->subdir_checks);
    dir_tree_model->subdir_checks_mutex = g_mutex_new ();
    dir_tree_model->subdir_cancellable = g_cancellable_new ();
    
    g_signal_connect (gtk_icon_theme_get_default (), "changed", G_CALLBACK (on_theme_changed), dir_tree_model);
}

static void fm_dir_tree_model_finalize (GObject *object)
{
    FmDirTreeModel *dir_tree_model;

    g_return_if_fail (object != NULL);
    g_return_if_fail (FM_IS_DIR_TREE_MODEL (object));

    dir_tree_model = FM_DIR_TREE_MODEL (object);

    fm_foreach (dir_tree_model->root_list, (GFunc) fm_dir_tree_item_free_l, NULL);
    g_list_free (dir_tree_model->root_list);

    // Check Subdirectories job...
    g_object_unref (dir_tree_model->subdir_cancellable); 
    
    g_signal_handlers_disconnect_by_func (gtk_icon_theme_get_default (), on_theme_changed, dir_tree_model);

    G_OBJECT_CLASS (fm_dir_tree_model_parent_class)->finalize (object);
}


/*****************************************************************************************
 *  Create Direct Root Items...
 * 
 * 
 ****************************************************************************************/
void fm_dir_tree_model_load (FmDirTreeModel *dir_tree_model)
{
    FmPath *path;
    FmFileInfo *file_info;
    FmDirTreeItem *dir_tree_item;
    
    
    // Desktop...
    path = fm_path_get_desktop ();
    file_info = fm_file_info_new_for_path (path);
    fm_file_info_query (file_info, NULL, NULL);
    fm_dir_tree_model_add_root (dir_tree_model, file_info, NULL, TRUE);


    // Root FileSystem...
    path = fm_path_get_root ();
    file_info = fm_file_info_new_for_path (path);
    fm_file_info_query (file_info, NULL, NULL);
    fm_dir_tree_model_add_root (dir_tree_model, file_info, NULL, TRUE);
    
    
    // Settings...
    path = fm_path_new_for_uri ("menu://Applications/DesktopSettings");
    file_info = fm_file_info_new_for_path (path);
    fm_file_info_query (file_info, NULL, NULL);
    fm_dir_tree_model_add_root (dir_tree_model, file_info, NULL, FALSE);
    fm_path_unref (path);
    
    // System...
    path = fm_path_new_for_uri ("menu://Applications/System");
    file_info = fm_file_info_new_for_path (path);
    fm_file_info_query (file_info, NULL, NULL);
    fm_dir_tree_model_add_root (dir_tree_model, file_info, NULL, FALSE);
    fm_path_unref (path);
    
    // What's the purpose of weak pointers ??? :-P
    g_object_add_weak_pointer (dir_tree_model, &dir_tree_model);
    return;
}

void fm_dir_tree_model_add_root (FmDirTreeModel *dir_tree_model, FmFileInfo *root, GtkTreeIter *iter,
                                 gboolean can_expand)
{
    FmDirTreeItem *dir_tree_item = fm_dir_tree_item_new (dir_tree_model, NULL, root);
    
    dir_tree_model->root_list = g_list_append (dir_tree_model->root_list, dir_tree_item);
    
    GList *item_list = g_list_last (dir_tree_model->root_list);
    
    // TODO_axl : add a can_expand flag in the file info with fm_file_info_set_can_expand () accessor...
    if (can_expand)
        fm_dir_tree_model_add_place_holder_child_item (dir_tree_model, item_list, NULL, FALSE);

    // Convert the new item to an iterator...
    GtkTreeIter it;
    fm_dir_tree_model_item_to_tree_iter (dir_tree_model, item_list, &it);
    GtkTreePath *tree_path = fm_dir_tree_model_item_to_tree_path (dir_tree_model, item_list);
    
    // Emit row-inserted signal for the new root item...
    gtk_tree_model_row_inserted (GTK_TREE_MODEL (dir_tree_model), tree_path, &it);

    // Output the iterator...
    if (iter)
        *iter = it;
    
    gtk_tree_path_free (tree_path);
}

static void fm_dir_tree_model_add_place_holder_child_item (FmDirTreeModel *dir_tree_model, GList *parent_node,
                                                           GtkTreePath *tree_path, gboolean emit_signal)
{
    FmDirTreeItem *parent_item = (FmDirTreeItem*) parent_node->data;
    
    // Check if the parent node can expand...
    FmFileInfo *file_info = parent_item->file_info;
    FmPath *path = fm_file_info_get_path (file_info);
    
    TREEVIEW_DEBUG ("TREEVIEW_DEBUG: fm_dir_tree_model_add_place_holder_child_item: file = %s\t emit_signal = %d\n",
                    fm_file_info_get_name (file_info), emit_signal);
    
    
    // don't expand the trash can...
    // TODO_axl: add a can_expand flag into the file_info
    if (fm_path_is_trash (path) && fm_path_is_root (path))
        return;
    
    TREEVIEW_DEBUG ("TREEVIEW_DEBUG: fm_dir_tree_model_add_place_holder_child_item: create place holder\n\n");
    
    FmDirTreeItem *dir_tree_item = fm_dir_tree_item_new (dir_tree_model, parent_node, NULL);
    
    parent_item->children = g_list_prepend (parent_item->children, dir_tree_item);

    if (!emit_signal)
        return;
        
    GtkTreeIter it;
    fm_dir_tree_model_item_to_tree_iter (dir_tree_model, parent_item->children, &it);
    
    gtk_tree_path_append_index (tree_path, 0);
    
    gtk_tree_model_row_inserted (GTK_TREE_MODEL (dir_tree_model), tree_path, &it);
    gtk_tree_path_up (tree_path);
}


/*****************************************************************************************
 *  Add a FileInfo to the parent node to the proper position.
 *  tree_path is the tree path of the parent node.
 * 
 ****************************************************************************************/
GList *fm_dir_tree_model_insert_file_info (FmDirTreeModel *dir_tree_model, GList *parent_node,
                                           GtkTreePath *tree_path, FmFileInfo *file_info)
{
    FmDirTreeItem *parent_item = (FmDirTreeItem*) parent_node->data;
    
    FmDirTreeItem *dir_tree_item = fm_dir_tree_item_new (dir_tree_model, parent_node, file_info);

    // Don't show hidden files...
    if (!dir_tree_model->show_hidden && file_info->path->name[0] == '.')
    {
        parent_item->hidden_children = g_list_prepend (parent_item->hidden_children, dir_tree_item);
        return parent_item->hidden_children;
    }
    
    // Don't show links...
    if (!dir_tree_model->show_symlinks && fm_file_info_is_symlink (file_info))
    {
        parent_item->hidden_children = g_list_prepend (parent_item->hidden_children, dir_tree_item);
        return parent_item->hidden_children;
    }
    
    GList *item_list = fm_dir_tree_model_insert_sorted (dir_tree_model, parent_node, tree_path, dir_tree_item);
    
    char *tmp_path = gtk_tree_path_to_string (tree_path);
    TREEVIEW_DEBUG ("TREEVIEW_DEBUG: fm_dir_tree_model_insert_file_info: file = %s\t index = %d\n\n",
                    fm_file_info_get_name (file_info), file_info->sorting_index);
    g_free (tmp_path);
    
    return item_list;
}

static GList *fm_dir_tree_model_insert_sorted (FmDirTreeModel *dir_tree_model, GList *parent_node,
                                               GtkTreePath *tree_path, FmDirTreeItem *new_item)
{
    FmDirTreeItem *parent_item = (FmDirTreeItem*) parent_node->data;
    
    // Get a collate key for the new item, it's used to find the correct insertion position...
    const char *new_key = fm_file_info_get_collate_key (new_item->file_info);
    
    /*****************************************************************
     * Parse the list of items and insert at the correct position,
     * This loop is critical for TreeView sorting performance...
     * 
     **/
    GList *item_list;
    int n = 0;
    GList *last_l = NULL;
    for (item_list = parent_item->children; item_list; item_list=item_list->next, ++n)
    {
        FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) item_list->data;
        const char *key;
        last_l = item_list;
        
        if (G_UNLIKELY (!dir_tree_item->file_info))
            continue;
        
        //~ TREEVIEW_DEBUG ("TREEVIEW_DEBUG: fm_dir_tree_model_insert_sorted: comparing %s and %s\n",
                    //~ fm_file_info_get_name (new_item->file_info), fm_file_info_get_name (dir_tree_item->file_info));

        key = fm_file_info_get_collate_key (dir_tree_item->file_info);
        
        if (strcmp (new_key, key) <= 0)
            break;
    }

    parent_item->children = g_list_insert_before (parent_item->children, item_list, new_item);
    
    // get the GList* of the newly inserted item 
    // the new item becomes the last item of the list 
    GList *new_item_list;
    if (!item_list)
    {
        new_item_list = last_l ? last_l->next : parent_item->children;
    }
    
    // the new item is just previous item of its sibling. 
    else
    {
        new_item_list = item_list->prev;
    }
    
    g_return_val_if_fail (new_item->file_info != NULL, NULL);
    g_return_val_if_fail (((FmDirTreeItem*) new_item_list->data)->file_info != NULL, NULL);
    g_return_val_if_fail (new_item == new_item_list->data, NULL);

    // Signal the view that we inserted a new item...
    GtkTreeIter it;
    fm_dir_tree_model_item_to_tree_iter (dir_tree_model, new_item_list, &it);
    gtk_tree_path_append_index (tree_path, n);
    gtk_tree_model_row_inserted ((GtkTreeModel*) dir_tree_model, tree_path, &it);

    // Add a place holder...
    fm_dir_tree_model_add_place_holder_child_item (dir_tree_model, new_item_list, tree_path, TRUE);
    gtk_tree_path_up (tree_path);

    // Check if the directory has subdirs and make it expandable if needed. 
    if (dir_tree_model->check_subdir)
        fm_dir_tree_model_item_queue_subdir_check (dir_tree_model, new_item_list);
    
    return new_item_list;
}


/*****************************************************************************************
 *  ...
 * 
 * 
 ****************************************************************************************/
void fm_dir_tree_model_expand_row (FmDirTreeModel *dir_tree_model, GtkTreeIter *it, GtkTreePath *tree_path)
{
    //TREEVIEW_DEBUG ("TREEVIEW_DEBUG: fm_dir_tree_model_expand_row\n");
    
    GList *item_list = (GList*) it->user_data;
    
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) item_list->data;
    
    g_return_if_fail (dir_tree_item != NULL);
    
    if (dir_tree_item->n_expand == 0)
    {
        // Dynamically load content of the folder...
        
        FmFolder *folder = fm_dir_tree_item_set_folder (item_list);

        // If the folder is already loaded, call "loaded" handler ourselves...
        if (fm_folder_get_is_loaded (folder))
        {
            FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) item_list->data;
            
            GSList *files = fm_list_peek_head_link (folder->files);
            
            fm_dir_tree_item_load_folder (folder, files, item_list, FALSE);
            
            fm_dir_tree_item_on_folder_loaded (dir_tree_item);
        }
        else
        {
            //TREEVIEW_DEBUG ("NOT loaded !!!\n");
        }
    }
    
    dir_tree_item->n_expand++;
}

void fm_dir_tree_model_collapse_row (FmDirTreeModel *dir_tree_model, GtkTreeIter *it, GtkTreePath *tree_path)
{
    return;
    
    GList *item_list = (GList*) it->user_data;
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) item_list->data;
    g_return_if_fail (dir_tree_item);
    
    --dir_tree_item->n_expand;
    
    if (dir_tree_item->n_expand == 0)
    {
        /*** remove all children, and replace them with a dummy child
         * item to keep expander in the tree view around.***/
        fm_dir_tree_model_remove_all_children (dir_tree_model, item_list, tree_path);

        /*** now, GtkTreeView think that we have no child since all
         * child items are removed. So we add a place holder child
         * item to keep the expander around. ***/
        
        fm_dir_tree_model_add_place_holder_child_item (dir_tree_model, item_list, tree_path, TRUE);
    }
}


/*****************************************************************************************
 *  ...
 * 
 * 
 ****************************************************************************************/
void fm_dir_tree_model_remove_item (FmDirTreeModel *dir_tree_model, GList *item_list)
{
    GtkTreePath *tree_path = fm_dir_tree_model_item_to_tree_path (dir_tree_model, item_list);
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*)item_list->data;
    FmDirTreeItem *parent_item = (FmDirTreeItem*)dir_tree_item->parent ? dir_tree_item->parent->data : NULL;
    
    fm_dir_tree_item_free_l (item_list);
    
    if (parent_item)
        parent_item->children = g_list_delete_link (parent_item->children, item_list);
    
    char *tmp_path = gtk_tree_path_to_string (tree_path);
    //TREEVIEW_DEBUG ("TREEVIEW_DEBUG: fm_dir_tree_model_remove_item %s\n", tmp_path);
    g_free (tmp_path);
    
    // signal the view that we removed the placeholder item. 
    gtk_tree_model_row_deleted (GTK_TREE_MODEL (dir_tree_model), tree_path);
    
    gtk_tree_path_free (tree_path);
}

static void fm_dir_tree_model_remove_all_children (FmDirTreeModel *dir_tree_model, GList *item_list,
                                                   GtkTreePath *tree_path)
{
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*)item_list->data;
    if (G_UNLIKELY (!dir_tree_item->children))
        return;
    
    gtk_tree_path_append_index (tree_path, 0);
    
    while (dir_tree_item->children)
    {
        fm_dir_tree_item_free_l (dir_tree_item->children);
        dir_tree_item->children = g_list_delete_link (dir_tree_item->children, dir_tree_item->children);
        
        char *tmp_path = gtk_tree_path_to_string (tree_path);
        //TREEVIEW_DEBUG ("TREEVIEW_DEBUG: fm_dir_tree_model_remove_all_children %s\n", tmp_path);
        g_free (tmp_path);
        
        // signal the view that we removed the placeholder item.
        // everytime we remove the first item, its next item became the
        // first item, so there is no need to update tree_path.
        gtk_tree_model_row_deleted (GTK_TREE_MODEL (dir_tree_model), tree_path);
    }

    if (dir_tree_item->hidden_children)
    {
        g_list_foreach (dir_tree_item->hidden_children, (GFunc)fm_dir_tree_item_free, NULL);
        g_list_free (dir_tree_item->hidden_children);
        dir_tree_item->hidden_children = NULL;
    }
    
    gtk_tree_path_up (tree_path);
    
    GtkTreeIter it;
    gtk_tree_model_row_has_child_toggled (GTK_TREE_MODEL (dir_tree_model), tree_path, &it);
    
    // ???
    //~ gtk_tree_model_row_changed ((GtkTreeModel*) dir_tree_model, tree_path, &it);
}


/*****************************************************************************************
 *  ...
 * 
 * 
 ****************************************************************************************/
void fm_dir_tree_model_set_icon_size (FmDirTreeModel *dir_tree_model, guint icon_size)
{
    if (dir_tree_model->icon_size == icon_size)
        return;
    
    on_theme_changed (NULL, dir_tree_model);
}

guint fm_dir_tree_get_icon_size (FmDirTreeModel *dir_tree_model)
{
    return dir_tree_model->icon_size;
}

static void on_theme_changed (GtkIconTheme *theme, FmDirTreeModel *dir_tree_model)
{
    GtkTreePath *tree_path = gtk_tree_path_new_first ();
    
    GList *l;
    for (l = dir_tree_model->root_list; l; l = l->next)
    {
        fm_dir_tree_model_item_reload_icon (dir_tree_model, (FmDirTreeItem*) l->data, tree_path);
        gtk_tree_path_next (tree_path);
    }
    gtk_tree_path_free (tree_path);
}

static void fm_dir_tree_model_item_reload_icon (FmDirTreeModel *dir_tree_model, FmDirTreeItem *dir_tree_item,
                                                GtkTreePath *tree_path)
{
    g_return_if_fail (dir_tree_item != NULL);
    
    GtkTreeIter it;
    GList *l;

    //~ if (dir_tree_item->icon)
    //~ {
        //~ g_object_unref (dir_tree_item->icon);
        //~ dir_tree_item->icon = NULL;
        //~ gtk_tree_model_row_changed (GTK_TREE_MODEL (dir_tree_model), tree_path, &it);
    //~ }

    if (dir_tree_item->fm_icon)
    {
        fm_icon_unref (dir_tree_item->fm_icon);
        dir_tree_item->fm_icon = NULL;
        gtk_tree_model_row_changed (GTK_TREE_MODEL (dir_tree_model), tree_path, &it);
    }

    if (dir_tree_item->children)
    {
        gtk_tree_path_append_index (tree_path, 0);
        for (l = dir_tree_item->children; l; l=l->next)
        {
            fm_dir_tree_model_item_reload_icon (dir_tree_model, (FmDirTreeItem*) l->data, tree_path);
            gtk_tree_path_next (tree_path);
        }
        gtk_tree_path_up (tree_path);
    }

    for (l = dir_tree_item->hidden_children; l; l=l->next)
    {
        FmDirTreeItem *child = (FmDirTreeItem*) l->data;
        if (child && child->fm_icon)
        {
            fm_icon_unref (child->fm_icon);
            child->fm_icon = NULL;
        }
    }
}


/*****************************************************************************************
 *  ...
 * 
 * 
 ****************************************************************************************/
void fm_dir_tree_model_set_show_hidden (FmDirTreeModel *dir_tree_model, gboolean show_hidden)
{
    g_return_if_fail (dir_tree_model);
    
    dir_tree_model->show_hidden = show_hidden;
    
    return;
    
    if (show_hidden != dir_tree_model->show_hidden)
    {
        // Filter the model to hide hidden folders...
        //~ if (dir_tree_model->show_hidden)
        //~ {
//~ 
        //~ }
        //~ else
        //~ {
//~ 
        //~ }
    }
}

gboolean fm_dir_tree_model_get_show_hidden (FmDirTreeModel *dir_tree_model)
{
    return dir_tree_model->show_hidden;
}

void fm_dir_tree_model_set_show_symlinks (FmDirTreeModel *dir_tree_model, gboolean show_symlinks)
{
    g_return_if_fail (dir_tree_model);
    
    dir_tree_model->show_symlinks = show_symlinks;
}

gboolean fm_dir_tree_model_get_show_symlinks (FmDirTreeModel *dir_tree_model)
{
    return dir_tree_model->show_symlinks;
}


/*****************************************************************************************
 *  Check Subdirectories:
 * 
 *  check if dirs contain sub dir in another thread and make
 *  the tree nodes expandable when needed.
 *
 *
 ****************************************************************************************/
void fm_dir_tree_model_item_queue_subdir_check (FmDirTreeModel *dir_tree_model, GList *item_list)
{
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*)item_list->data;
    g_return_if_fail (dir_tree_item->file_info != NULL);

    g_mutex_lock (dir_tree_model->subdir_checks_mutex);
    
    g_queue_push_tail (&dir_tree_model->subdir_checks, item_list);
    // NO_DEBUG ("queue subdir check for %s\n", fm_file_info_get_disp_name (dir_tree_item->file_info));
    
    if (!dir_tree_model->job_running)
    {
        dir_tree_model->job_running = TRUE;
        dir_tree_model->current_subdir_check = (GList*)g_queue_peek_head (&dir_tree_model->subdir_checks);
        g_cancellable_reset (dir_tree_model->subdir_cancellable);
        g_io_scheduler_push_job (subdir_check_job,
                                g_object_ref (dir_tree_model),
                                (GDestroyNotify)g_object_unref,
                                G_PRIORITY_DEFAULT,
                                dir_tree_model->subdir_cancellable);
        // NO_DEBUG ("push job\n");
    }
    
    g_mutex_unlock (dir_tree_model->subdir_checks_mutex);
}

static gboolean subdir_check_job (GIOSchedulerJob *job, GCancellable *cancellable, gpointer user_data)
{
    FmDirTreeModel *dir_tree_model = FM_DIR_TREE_MODEL (user_data);

    
    // Lock ----------------------------------------------------------------------------------------
    g_mutex_lock (dir_tree_model->subdir_checks_mutex);
    
    GList *item_list = (GList*) g_queue_pop_head (&dir_tree_model->subdir_checks);
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) item_list->data;
    dir_tree_model->current_subdir_check = item_list;
    
    GFile *gfile = fm_path_to_gfile (fm_file_info_get_path (dir_tree_item->file_info));
    
    //gboolean is_mountable = fm_file_info_is_mountable (dir_tree_item->file_info);
    //gboolean is_drive = fm_file_info_is_drive (dir_tree_item->file_info);
    
    g_mutex_unlock (dir_tree_model->subdir_checks_mutex);
    // Unlock --------------------------------------------------------------------------------------
    
	//~ if (is_mountable)
    //~ {
        //~ TREEVIEW_DEBUG ("JOB_DEBUG: subdir_check_job: %s is mountable type !!!\n\n", directory);
    //~ }
    
    //~ GError *gerror = NULL;
    //~ gfile_info = g_file_query_info (gfile, gfile_info_query_attribs, 0, fm_job_get_cancellable (fmjob), &gerror);
    
    /**
     * Parse input directory...
     * 
     */
    char *directory = fm_file_info_get_name (dir_tree_item->file_info);
    JOB_DEBUG ("JOB_DEBUG: subdir_check_job: check \"%s\"\n", directory);
    
    GFileEnumerator *enumerator = g_file_enumerate_children (gfile,
                                                             G_FILE_ATTRIBUTE_STANDARD_NAME","
                                                             G_FILE_ATTRIBUTE_STANDARD_TYPE","
                                                             G_FILE_ATTRIBUTE_STANDARD_IS_HIDDEN,
                                                             0, cancellable, NULL);
    
    gboolean has_subdir = FALSE;
    if (enumerator)
    {
        while (!g_cancellable_is_cancelled (cancellable))
        {
            GFileInfo *gfile_info = g_file_enumerator_next_file (enumerator, cancellable, NULL);
            if (G_LIKELY (gfile_info))
            {
                GFileType g_file_type = g_file_info_get_file_type (gfile_info);
                gboolean is_hidden = g_file_info_get_is_hidden (gfile_info);
                
                //~ TREEVIEW_DEBUG ("TREEVIEW_DEBUG: subdir_check_job: GFileInfo for %s = %d\n",
                                //~ g_file_info_get_name (gfile_info), g_file_type);
                
                g_object_unref (gfile_info);
                
                if (g_file_type == G_FILE_TYPE_DIRECTORY || g_file_type == G_FILE_TYPE_MOUNTABLE)
                {
                    if (dir_tree_model->show_hidden || !is_hidden)
                    {
                        JOB_DEBUG ("JOB_DEBUG: subdir_check_job: A directory found in \"%s\" !!!\n\n", directory);
                        has_subdir = TRUE;
                        break;
                    }
                }
            }
            else
            {
                break;
            }
        }
        
        GError *error = NULL;
        g_file_enumerator_close (enumerator, cancellable, &error);
        g_object_unref (enumerator);
    }
    else
    {
        JOB_DEBUG ("JOB_DEBUG: subdir_check_job: Error: can't read \"%s\"...\n", directory);
    }
    
    // NO_DEBUG ("check result - %s has_dir: %d\n", g_file_get_parse_name (gfile), has_subdir);
    g_object_unref (gfile);
    
    if (!has_subdir)
    {
        JOB_DEBUG ("JOB_DEBUG: subdir_check_job: No directory found in \"%s\"\n\t\t\t  > Remove place holder\n\n",
                   directory);
        
        return g_io_scheduler_job_send_to_mainloop (job,
                                                    (GSourceFunc) subdir_check_remove_place_holder,
                                                    dir_tree_model, NULL);
    }
    
    return subdir_check_finish (dir_tree_model);
}


static gboolean subdir_check_remove_place_holder (FmDirTreeModel *dir_tree_model)
{
    GList *item_list = dir_tree_model->current_subdir_check;
    
    if (!g_cancellable_is_cancelled (dir_tree_model->subdir_cancellable) && item_list)
    {
        FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) item_list->data;
        
        // remove existing subdirs or place holder item if needed. 
        if (dir_tree_item->children)
        {
            // Remove the place holder...
            JOB_DEBUG ("JOB_DEBUG: subdir_check_remove_place_holder: remove place holder for %s\n\n",
                       fm_file_info_get_disp_name (dir_tree_item->file_info));
            
            GtkTreePath *tree_path = fm_dir_tree_model_item_to_tree_path (dir_tree_model, item_list);
            fm_dir_tree_model_remove_all_children (dir_tree_model, item_list, tree_path);
            gtk_tree_path_free (tree_path);
            
            // why i added this ??? doesn't seem needed...
            //~ fm_dir_tree_model_remove_item (dir_tree_model, item_list);
        }
    }
    
    return subdir_check_finish (dir_tree_model);
}

static gboolean subdir_check_finish (FmDirTreeModel *dir_tree_model)
{
    dir_tree_model->current_subdir_check = NULL;
    
    if (g_queue_is_empty (&dir_tree_model->subdir_checks))
    {
        dir_tree_model->job_running = FALSE;
        //TREEVIEW_DEBUG ("TREEVIEW_DEBUG: subdir_check_finish: all subdir checks are finished !\n");
        return FALSE;
    }
    // still has queued items 
    else
    {
        if (g_cancellable_is_cancelled (dir_tree_model->subdir_cancellable))
            g_cancellable_reset (dir_tree_model->subdir_cancellable);
    }
    
    return TRUE;
}


/*****************************************************************************************
 *  ...
 * 
 * 
 ****************************************************************************************/
inline void fm_dir_tree_model_item_to_tree_iter (FmDirTreeModel *dir_tree_model, GList *item_list, GtkTreeIter *it)
{
    it->stamp = dir_tree_model->stamp;
    
    // We simply store a GList pointer in the iter 
    it->user_data = item_list;
    it->user_data2 = it->user_data3 = NULL;
}


inline GtkTreePath *fm_dir_tree_model_item_to_tree_path (FmDirTreeModel *dir_tree_model, GList *item_list)
{
    GtkTreeIter it;
    
    fm_dir_tree_model_item_to_tree_iter (dir_tree_model, item_list, &it);
    
    return fm_dir_tree_model_get_path ((GtkTreeModel*) dir_tree_model, &it);
}

// Find child item by filename, and retrive its index if idx is not NULL...
GList *fm_dir_tree_model_children_by_name (FmDirTreeModel *dir_tree_model, GList *children, const char *name, int *idx)
{
    GList *l;
    int i = 0;
    for (l = children; l; l=l->next, ++i)
    {
        FmDirTreeItem *dir_tree_item = (FmDirTreeItem*)l->data;
        if (G_LIKELY (dir_tree_item->file_info) &&
           G_UNLIKELY (strcmp (dir_tree_item->file_info->path->name, name) == 0))
        {
            if (idx)
                *idx = i;
            return l;
        }
    }
    return NULL;
}


/*****************************************************************************************
 *  GtkTreeModel Implementation...
 * 
 * 
 * 
 ****************************************************************************************/
static GtkTreeModelFlags fm_dir_tree_model_get_flags (GtkTreeModel *tree_model)
{
    return GTK_TREE_MODEL_ITERS_PERSIST;
}

static gint fm_dir_tree_model_get_n_columns (GtkTreeModel *tree_model)
{
    return N_FM_DIR_TREE_MODEL_COLS;
}

static GType fm_dir_tree_model_get_column_type (GtkTreeModel *tree_model, gint index)
{
    g_return_val_if_fail (index < G_N_ELEMENTS (column_types) && index >= 0, G_TYPE_INVALID);
    return column_types [index];
}

gboolean fm_dir_tree_model_get_iter (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path)
{
    FmDirTreeModel *dir_tree_model;
    gint *indices, i, depth;
    GList *children, *child = NULL;

    g_assert (FM_IS_DIR_TREE_MODEL (tree_model));
    g_assert (path != NULL);

    dir_tree_model = FM_DIR_TREE_MODEL (tree_model);
    
    if (G_UNLIKELY (!dir_tree_model || !dir_tree_model->root_list))
        return FALSE;

    indices = gtk_tree_path_get_indices (path);
    depth   = gtk_tree_path_get_depth (path);

    children = dir_tree_model->root_list;
    for (i = 0; i < depth; ++i)
    {
        FmDirTreeItem *dir_tree_item;
        child = g_list_nth (children, indices[i]);
        if (!child)
            return FALSE;
        
        dir_tree_item = (FmDirTreeItem*)child->data;
        children = dir_tree_item->children;
    }
    
    fm_dir_tree_model_item_to_tree_iter (dir_tree_model, child, iter);
    
    return TRUE;
}

static GtkTreePath *fm_dir_tree_model_get_path (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    GList *item_list;
    GList *children;
    FmDirTreeItem *dir_tree_item;
    GtkTreePath *path;
    int i;
    FmDirTreeModel *dir_tree_model = FM_DIR_TREE_MODEL (tree_model);

    g_return_val_if_fail (dir_tree_model, NULL);
    g_return_val_if_fail (iter->stamp == dir_tree_model->stamp, NULL);
    g_return_val_if_fail (iter != NULL, NULL);
    g_return_val_if_fail (iter->user_data != NULL, NULL);

    item_list = (GList*)iter->user_data;
    dir_tree_item = (FmDirTreeItem*)item_list->data;

    // Root Item... 
    if (dir_tree_item->parent == NULL)
    {
        i = g_list_position (dir_tree_model->root_list, item_list);
        path = gtk_tree_path_new_first ();
        gtk_tree_path_get_indices (path)[0] = i;
    }
    else
    {
        path = gtk_tree_path_new ();
        do
        {
            FmDirTreeItem *parent_item = (FmDirTreeItem*)dir_tree_item->parent->data;
            children = parent_item->children;
            i = g_list_position (children, item_list);
            if (G_UNLIKELY (i == -1))
            {
                gtk_tree_path_free (path);
                return NULL;
            }
            
            gtk_tree_path_prepend_index (path, i);
            
            // Go One Level Up...
            item_list = dir_tree_item->parent;
            dir_tree_item = (FmDirTreeItem*)item_list->data;
        
        } while (G_UNLIKELY (dir_tree_item->parent));

        // We Have Reached Toplevel...
        children = dir_tree_model->root_list;
        i = g_list_position (children, item_list);
        gtk_tree_path_prepend_index (path, i);
    }
    return path;
}

static void fm_dir_tree_model_get_value (GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, GValue *value)
{
    FmDirTreeModel  *dir_tree_model = FM_DIR_TREE_MODEL(tree_model);
    GList           *item_list;
    FmDirTreeItem   *dir_tree_item;

    g_return_if_fail (iter->stamp == dir_tree_model->stamp);

    g_value_init (value, column_types[column]);
    
    item_list = (GList*) iter->user_data;
    g_return_if_fail (item_list);
    
    dir_tree_item = (FmDirTreeItem*) item_list->data;
    g_return_if_fail (dir_tree_item);
    
    switch (column)
    {
        case FM_DIR_TREE_MODEL_COL_ICON:
        {
            if (dir_tree_item->file_info && fm_file_info_get_fm_icon (dir_tree_item->file_info))
            {
                g_value_set_object (value, fm_dir_tree_item_get_pixbuf (dir_tree_item, dir_tree_model->icon_size));
            }
            else
            {
                g_value_set_object (value, NULL);
            }
        }
        break;
        
        case FM_DIR_TREE_MODEL_COL_DISP_NAME:
        {
            if (dir_tree_item->file_info)
                g_value_set_string (value, fm_file_info_get_disp_name (dir_tree_item->file_info));
        }    
        break;
        
        case FM_DIR_TREE_MODEL_COL_INFO:
            g_value_set_pointer (value, dir_tree_item->file_info);
        break;
        
        case FM_DIR_TREE_MODEL_COL_PATH:
            g_value_set_pointer (value, dir_tree_item->file_info ? dir_tree_item->file_info->path : NULL);
        break;
        
        case FM_DIR_TREE_MODEL_COL_FOLDER:
            
            // ????
            //~ g_value_set_pointer (value, dir_tree_item->folder ? dir_tree_item->folder :
                                                                //~ dir_tree_item->file_info->folder);
            
            g_value_set_pointer (value, dir_tree_item->folder);
        
        break;
    }
}

static gboolean fm_dir_tree_model_iter_next (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    FmDirTreeModel *dir_tree_model;
    GList *item_list;
    
    g_return_val_if_fail (FM_IS_DIR_TREE_MODEL (tree_model), FALSE);
    
    if (iter == NULL || iter->user_data == NULL)
        return FALSE;

    item_list = (GList*)iter->user_data;
    
    // Is this the last child in the parent node ?
    if (!item_list->next)
        return FALSE;

    dir_tree_model = FM_DIR_TREE_MODEL (tree_model);
    fm_dir_tree_model_item_to_tree_iter (dir_tree_model, item_list->next, iter);
    
    return TRUE;
}

static gboolean fm_dir_tree_model_iter_children (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent)
{
    FmDirTreeModel *dir_tree_model;
    GList *first_child;

    g_return_val_if_fail (parent == NULL || parent->user_data != NULL, FALSE);
    g_return_val_if_fail (FM_IS_DIR_TREE_MODEL (tree_model), FALSE);
    
    dir_tree_model = FM_DIR_TREE_MODEL (tree_model);

    if (parent)
    {
        GList *parent_node = (GList*) parent->user_data;
        FmDirTreeItem *parent_item = (FmDirTreeItem*) parent_node->data;
        first_child = parent_item->children;
    }
    
    // Toplevel item... 
    else
    {
        // parent == NULL is a special case; we need to return the first top-level row 
        first_child = dir_tree_model->root_list;
    }
    
    if (!first_child)
        return FALSE;

    // Set iter to first item in model 
    fm_dir_tree_model_item_to_tree_iter (dir_tree_model, first_child, iter);
    
    return TRUE;
}

static gboolean fm_dir_tree_model_iter_has_child (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    GList *item_list;
    FmDirTreeItem *dir_tree_item;
    
    // Is NULL iter allowed here? 
    g_return_val_if_fail (iter != NULL, FALSE);
    g_return_val_if_fail (iter->stamp == FM_DIR_TREE_MODEL (tree_model)->stamp, FALSE);

    item_list = (GList*)iter->user_data;
    dir_tree_item = (FmDirTreeItem*)item_list->data;
    return (dir_tree_item->children != NULL);
}

static gint fm_dir_tree_model_iter_n_children (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    FmDirTreeModel *dir_tree_model;
    GList *children;
    g_return_val_if_fail (FM_IS_DIR_TREE_MODEL (tree_model), -1);

    dir_tree_model = FM_DIR_TREE_MODEL (tree_model);
    
    // special case: if iter == NULL, return number of top-level rows 
    if (!iter)
        children = dir_tree_model->root_list;
    else
    {
        GList *item_list = (GList*)iter->user_data;
        FmDirTreeItem *dir_tree_item = (FmDirTreeItem*)item_list->data;
        children = dir_tree_item->children;
    }
    return g_list_length (children);
}

static gboolean fm_dir_tree_model_iter_nth_child (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent,
                                                  gint n)
{
    FmDirTreeModel *dir_tree_model;
    GList *children;
    GList *child_l;

    g_return_val_if_fail (FM_IS_DIR_TREE_MODEL (tree_model), FALSE);
    dir_tree_model = FM_DIR_TREE_MODEL (tree_model);

    if (G_LIKELY (parent))
    {
        GList *parent_node = (GList*) parent->user_data;
        FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) parent_node->data;
        children = dir_tree_item->children;
    }
    // special case: if parent == NULL, set iter to n-th top-level row 
    else
    {
        children = dir_tree_model->root_list;
    }
    child_l = g_list_nth (children, n);
    if (!child_l)
        return FALSE;

    fm_dir_tree_model_item_to_tree_iter (dir_tree_model, child_l, iter);
    return TRUE;
}

static gboolean fm_dir_tree_model_iter_parent (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child)
{
    GList *child_l;
    FmDirTreeItem *child_item;
    FmDirTreeModel *dir_tree_model;
    g_return_val_if_fail (iter != NULL && child != NULL, FALSE);

    dir_tree_model = FM_DIR_TREE_MODEL (tree_model);
    child_l = (GList*)child->user_data;
    child_item = (FmDirTreeItem*)child_l->data;

    if (G_LIKELY (child_item->parent))
    {
        fm_dir_tree_model_item_to_tree_iter (dir_tree_model, child_item->parent, iter);
        return TRUE;
    }
    return FALSE;
}


