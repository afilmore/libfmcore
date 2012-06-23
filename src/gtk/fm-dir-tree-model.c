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

#include "fm-debug.h"
#include "fm-utils.h"
#include "fm-dir-tree-model.h"
#include "fm-dir-tree-item.h"


static void fm_dir_tree_model_tree_model_init           (GtkTreeModelIface *iface);

G_DEFINE_TYPE_WITH_CODE (FmDirTreeModel,
                         fm_dir_tree_model, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, fm_dir_tree_model_tree_model_init)
                         /*** 
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_DRAG_SOURCE, fm_dir_tree_model_drag_source_init)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_DRAG_DEST, fm_dir_tree_model_drag_dest_init)
                         ***/
                       )

static GType column_types [N_FM_DIR_TREE_MODEL_COLS];

// Forward Declarations...
static GtkTreeModelFlags fm_dir_tree_model_get_flags    (GtkTreeModel *tree_model);
static gint fm_dir_tree_model_get_n_columns             (GtkTreeModel *tree_model);
static GType fm_dir_tree_model_get_column_type          (GtkTreeModel *tree_model, gint index);
static GtkTreePath *fm_dir_tree_model_get_path          (GtkTreeModel *tree_model, GtkTreeIter *iter);
static void fm_dir_tree_model_get_value     (GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, GValue *value);
static gboolean fm_dir_tree_model_iter_next (GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean fm_dir_tree_model_iter_has_child        (GtkTreeModel *tree_model, GtkTreeIter *iter);
static gint fm_dir_tree_model_iter_n_children           (GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean fm_dir_tree_model_iter_children         (GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         GtkTreeIter *parent);
static gboolean fm_dir_tree_model_iter_nth_child        (GtkTreeModel *tree_model, 
                                                         GtkTreeIter *iter,
                                                         GtkTreeIter *parent,
                                                         gint n);
static gboolean fm_dir_tree_model_iter_parent           (GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         GtkTreeIter *child);

static void fm_dir_tree_model_finalize                  (GObject *object);
static void fm_dir_tree_model_add_place_holder_child_item (FmDirTreeModel *model, GList *parent_l, GtkTreePath *tp,
                                                           gboolean emit_signal);
static void on_theme_changed                            (GtkIconTheme *theme, FmDirTreeModel *model);
static void fm_dir_tree_model_item_reload_icon (FmDirTreeModel *model, FmDirTreeItem *dir_tree_item, GtkTreePath *tp);
static gboolean subdir_check_job                        (GIOSchedulerJob *job, GCancellable *cancellable,
                                                         gpointer user_data);
/** TODO_axl: this may be useless now... may remove it...
static gboolean subdir_check_finish_has_subdir (FmDirTreeModel *model);
**/
static gboolean subdir_check_remove_place_holder (FmDirTreeModel *model);
static gboolean subdir_check_finish (FmDirTreeModel *model);


/*****************************************************************************************
 * ...
 * 
 * 
 ****************************************************************************************/
FmDirTreeModel *fm_dir_tree_model_new ()
{
    TREEVIEW_DEBUG ("fm_dir_tree_model_new\n");
    
    FmDirTreeModel *dir_tree_model = (FmDirTreeModel*) g_object_new (FM_TYPE_DIR_TREE_MODEL, NULL);
    
    //dir_tree_model->show_hidden = show_hidden;
    
    return dir_tree_model;
}

static void fm_dir_tree_model_class_init (FmDirTreeModelClass *klass)
{
    TREEVIEW_DEBUG ("fm_dir_tree_model_class_init\n");
    
    GObjectClass *g_object_class;
    g_object_class = G_OBJECT_CLASS (klass);
    g_object_class->finalize = fm_dir_tree_model_finalize;
}

static void fm_dir_tree_model_tree_model_init (GtkTreeModelIface *iface)
{
    TREEVIEW_DEBUG ("fm_dir_tree_model_tree_model_init\n");
    
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

static void fm_dir_tree_model_init (FmDirTreeModel *model)
{
    TREEVIEW_DEBUG ("fm_dir_tree_model_init\n");

    model->icon_size = 16;
    model->stamp = g_random_int ();
    
    // Check Subdirectories...
    model->check_subdir = TRUE;
//    model->check_subdir_mode2 = TRUE;
    
    g_queue_init (&model->subdir_checks);
    model->subdir_checks_mutex = g_mutex_new ();
    model->subdir_cancellable = g_cancellable_new ();
    
    g_signal_connect (gtk_icon_theme_get_default (), "changed", G_CALLBACK (on_theme_changed), model);
}

static void fm_dir_tree_model_finalize (GObject *object)
{
    FmDirTreeModel *model;

    g_return_if_fail (object != NULL);
    g_return_if_fail (FM_IS_DIR_TREE_MODEL (object));

    model = FM_DIR_TREE_MODEL (object);

    fm_foreach (model->roots, (GFunc) fm_dir_tree_item_free_l, NULL);
    g_list_free (model->roots);

    // Check Subdirectories...
    g_object_unref (model->subdir_cancellable); 
    
    g_signal_handlers_disconnect_by_func (gtk_icon_theme_get_default (), on_theme_changed, model);

    G_OBJECT_CLASS (fm_dir_tree_model_parent_class)->finalize (object);
}


/*****************************************************************************************
 * GtkTreeModel Implementation...
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
    return column_types[index];
}

gboolean fm_dir_tree_model_get_iter (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path)
{
    FmDirTreeModel *model;
    gint *indices, i, depth;
    GList *children, *child = NULL;

    g_assert (FM_IS_DIR_TREE_MODEL (tree_model));
    g_assert (path != NULL);

    model = FM_DIR_TREE_MODEL (tree_model);
    if (G_UNLIKELY (!model || !model->roots))
        return FALSE;

    indices = gtk_tree_path_get_indices (path);
    depth   = gtk_tree_path_get_depth (path);

    children = model->roots;
    for (i = 0; i < depth; ++i)
    {
        FmDirTreeItem *dir_tree_item;
        child = g_list_nth (children, indices[i]);
        if (!child)
            return FALSE;
        
        dir_tree_item = (FmDirTreeItem*)child->data;
        children = dir_tree_item->children;
    }
    
    fm_dir_tree_model_item_to_tree_iter (model, child, iter);
    return TRUE;
}

static GtkTreePath *fm_dir_tree_model_get_path (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    GList *item_list;
    GList *children;
    FmDirTreeItem *dir_tree_item;
    GtkTreePath *path;
    int i;
    FmDirTreeModel *model = FM_DIR_TREE_MODEL (tree_model);

    g_return_val_if_fail (model, NULL);
    g_return_val_if_fail (iter->stamp == model->stamp, NULL);
    g_return_val_if_fail (iter != NULL, NULL);
    g_return_val_if_fail (iter->user_data != NULL, NULL);

    item_list = (GList*)iter->user_data;
    dir_tree_item = (FmDirTreeItem*)item_list->data;

    // Root Item... 
    if (dir_tree_item->parent == NULL)
    {
        i = g_list_position (model->roots, item_list);
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
        children = model->roots;
        i = g_list_position (children, item_list);
        gtk_tree_path_prepend_index (path, i);
    }
    return path;
}

static void fm_dir_tree_model_get_value (GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, GValue *value)
{
    FmDirTreeModel  *model = FM_DIR_TREE_MODEL(tree_model);
    GList           *item_list;
    FmDirTreeItem   *dir_tree_item;

    g_return_if_fail (iter->stamp == model->stamp);

    g_value_init (value, column_types[column]);
    
    item_list = (GList*) iter->user_data;
    g_return_if_fail (item_list);
    
    dir_tree_item = (FmDirTreeItem*) item_list->data;
    g_return_if_fail (dir_tree_item);
    
    switch (column)
    {
        case FM_DIR_TREE_MODEL_COL_ICON:
        {
            if (dir_tree_item->fi && fm_file_info_get_fm_icon (dir_tree_item->fi))
            {
                g_value_set_object (value, fm_dir_tree_item_get_pixbuf (dir_tree_item, model->icon_size));
            }
            else
            {
                g_value_set_object (value, NULL);
            }
        }
        break;
        
        case FM_DIR_TREE_MODEL_COL_DISP_NAME:
        {
            if (dir_tree_item->fi)
                g_value_set_string (value, fm_file_info_get_disp_name (dir_tree_item->fi));
        }    
        break;
        
        case FM_DIR_TREE_MODEL_COL_INFO:
            g_value_set_pointer (value, dir_tree_item->fi);
        break;
        
        case FM_DIR_TREE_MODEL_COL_PATH:
            g_value_set_pointer (value, dir_tree_item->fi ? dir_tree_item->fi->path : NULL);
        break;
        
        case FM_DIR_TREE_MODEL_COL_FOLDER:
            //~ g_value_set_pointer (value, dir_tree_item->folder ? dir_tree_item->folder : dir_tree_item->fi->folder);
            g_value_set_pointer (value, dir_tree_item->folder);
        break;
    }
}

static gboolean fm_dir_tree_model_iter_next (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    FmDirTreeModel *model;
    GList *item_list;
    
    g_return_val_if_fail (FM_IS_DIR_TREE_MODEL (tree_model), FALSE);
    
    if (iter == NULL || iter->user_data == NULL)
        return FALSE;

    item_list = (GList*)iter->user_data;
    
    // Is this the last child in the parent node ?
    if (!item_list->next)
        return FALSE;

    model = FM_DIR_TREE_MODEL (tree_model);
    fm_dir_tree_model_item_to_tree_iter (model, item_list->next, iter);
    
    return TRUE;
}

static gboolean fm_dir_tree_model_iter_children (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent)
{
    FmDirTreeModel *model;
    GList *first_child;

    g_return_val_if_fail (parent == NULL || parent->user_data != NULL, FALSE);
    g_return_val_if_fail (FM_IS_DIR_TREE_MODEL (tree_model), FALSE);
    
    model = FM_DIR_TREE_MODEL (tree_model);

    if (parent)
    {
        GList *parent_l = (GList*) parent->user_data;
        FmDirTreeItem *parent_item = (FmDirTreeItem*) parent_l->data;
        first_child = parent_item->children;
    }
    
    // Toplevel item... 
    else
    {
        // parent == NULL is a special case; we need to return the first top-level row 
        first_child = model->roots;
    }
    
    if (!first_child)
        return FALSE;

    // Set iter to first item in model 
    fm_dir_tree_model_item_to_tree_iter (model, first_child, iter);
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
    FmDirTreeModel *model;
    GList *children;
    g_return_val_if_fail (FM_IS_DIR_TREE_MODEL (tree_model), -1);

    model = FM_DIR_TREE_MODEL (tree_model);
    
    // special case: if iter == NULL, return number of top-level rows 
    if (!iter)
        children = model->roots;
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
    FmDirTreeModel *model;
    GList *children;
    GList *child_l;

    g_return_val_if_fail (FM_IS_DIR_TREE_MODEL (tree_model), FALSE);
    model = FM_DIR_TREE_MODEL (tree_model);

    if (G_LIKELY (parent))
    {
        GList *parent_l = (GList*) parent->user_data;
        FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) parent_l->data;
        children = dir_tree_item->children;
    }
    // special case: if parent == NULL, set iter to n-th top-level row 
    else
    {
        children = model->roots;
    }
    child_l = g_list_nth (children, n);
    if (!child_l)
        return FALSE;

    fm_dir_tree_model_item_to_tree_iter (model, child_l, iter);
    return TRUE;
}

static gboolean fm_dir_tree_model_iter_parent (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child)
{
    GList *child_l;
    FmDirTreeItem *child_item;
    FmDirTreeModel *model;
    g_return_val_if_fail (iter != NULL && child != NULL, FALSE);

    model = FM_DIR_TREE_MODEL (tree_model);
    child_l = (GList*)child->user_data;
    child_item = (FmDirTreeItem*)child_l->data;

    if (G_LIKELY (child_item->parent))
    {
        fm_dir_tree_model_item_to_tree_iter (model, child_item->parent, iter);
        return TRUE;
    }
    return FALSE;
}


/*****************************************************************************************
 * ...
 * 
 * 
 ****************************************************************************************/
inline void fm_dir_tree_model_item_to_tree_iter (FmDirTreeModel *model, GList *item_list, GtkTreeIter *it)
{
    it->stamp = model->stamp;
    
    // We simply store a GList pointer in the iter 
    it->user_data = item_list;
    it->user_data2 = it->user_data3 = NULL;
}


inline GtkTreePath *fm_dir_tree_model_item_to_tree_path (FmDirTreeModel *model, GList *item_list)
{
    GtkTreeIter it;
    fm_dir_tree_model_item_to_tree_iter (model, item_list, &it);
    return fm_dir_tree_model_get_path ((GtkTreeModel*) model, &it);
}

// find child item by filename, and retrive its index if idx is not NULL. 
GList *fm_dir_tree_model_children_by_name (FmDirTreeModel *model, GList *children, const char *name, int *idx)
{
    GList *l;
    int i = 0;
    for (l = children; l; l=l->next, ++i)
    {
        FmDirTreeItem *dir_tree_item = (FmDirTreeItem*)l->data;
        if (G_LIKELY (dir_tree_item->fi) &&
           G_UNLIKELY (strcmp (dir_tree_item->fi->path->name, name) == 0))
        {
            if (idx)
                *idx = i;
            return l;
        }
    }
    return NULL;
}


/*****************************************************************************************
 * ...
 * 
 * 
 ****************************************************************************************/
void fm_dir_tree_model_add_root (FmDirTreeModel *model, FmFileInfo *root, GtkTreeIter *iter, gboolean expand)
{
    GtkTreeIter it;
    GtkTreePath *tp;
    GList *item_list;
    
    FmDirTreeItem *dir_tree_item = fm_dir_tree_item_new (model, NULL, root);
    
    model->roots = g_list_append (model->roots, dir_tree_item);
    
    item_list = g_list_last (model->roots);
    
    if (expand)
        fm_dir_tree_model_add_place_holder_child_item (model, item_list, NULL, FALSE);

    // Emit row-inserted signal for the new root item...
    fm_dir_tree_model_item_to_tree_iter (model, item_list, &it);
    tp = fm_dir_tree_model_item_to_tree_path (model, item_list);
    
    gtk_tree_model_row_inserted (GTK_TREE_MODEL (model), tp, &it);

    if (iter)
        *iter = it;
    
    gtk_tree_path_free (tp);
}

static void fm_dir_tree_model_add_place_holder_child_item (FmDirTreeModel *model, GList *parent_l, GtkTreePath *tp, gboolean emit_signal)
{
    
    FmDirTreeItem *parent_item = (FmDirTreeItem*) parent_l->data;
    
    FmDirTreeItem *dir_tree_item = fm_dir_tree_item_new (model, parent_l, NULL);
    
    parent_item->children = g_list_prepend (parent_item->children, dir_tree_item);

    if (emit_signal)
    {
        GtkTreeIter it;
        fm_dir_tree_model_item_to_tree_iter (model, parent_item->children, &it);
        gtk_tree_path_append_index (tp, 0);
        gtk_tree_model_row_inserted (GTK_TREE_MODEL (model), tp, &it);
        gtk_tree_path_up (tp);
    }
}


/*****************************************************************************************
 * Add a new node to parent node to proper position.
 * GtkTreePath tp is the tree path of parent node.
 * Note that value of tp will be changed inside the function temporarily
 * to generate GtkTreePath for child nodes, and then restored to its
 * original value before returning from the function.
 * 
 * 
 ****************************************************************************************/
static GList *fm_dir_tree_model_insert_item (FmDirTreeModel *model, GList *parent_l, GtkTreePath *tp, FmDirTreeItem *new_item)
{
    GList *item_list;
    FmDirTreeItem *parent_item = (FmDirTreeItem*) parent_l->data;
    int n = 0;
    GList *last_l = NULL;
    const char *new_key = fm_file_info_get_collate_key (new_item->fi);
    
    for (item_list = parent_item->children; item_list; item_list=item_list->next, ++n)
    {
        FmDirTreeItem *dir_tree_item = (FmDirTreeItem*) item_list->data;
        const char *key;
        last_l = item_list;
        
        if (G_UNLIKELY (!dir_tree_item->fi))
            continue;
        
        key = fm_file_info_get_collate_key (dir_tree_item->fi);
        
        if (strcmp (new_key, key) <= 0)
            break;
    }

    parent_item->children = g_list_insert_before (parent_item->children, item_list, new_item);
    
    // get the GList* of the newly inserted item 
    // the new item becomes the last item of the list 
    GList *new_item_list;
    if (!item_list)
        new_item_list = last_l ? last_l->next : parent_item->children;
    
    // the new item is just previous item of its sibling. 
    else
        new_item_list = item_list->prev;

    g_assert (new_item->fi != NULL);
    g_assert (new_item == new_item_list->data);
    g_assert (((FmDirTreeItem*) new_item_list->data)->fi != NULL);

    
    GtkTreeIter it;
    
    // emit row-inserted signal for the new item 
    fm_dir_tree_model_item_to_tree_iter (model, new_item_list, &it);
    gtk_tree_path_append_index (tp, n);
    gtk_tree_model_row_inserted ((GtkTreeModel*) model, tp, &it);

    /** TODO_axl: test and remove...
    if (model->check_subdir)
    {
        if (model->check_subdir_mode2)
            fm_dir_tree_model_add_place_holder_child_item (model, new_item_list, tp, TRUE);
        
        gtk_tree_path_up (tp);

        // Check Subdirectories: check if the dir has subdirs and make it expandable if needed. 
        fm_dir_tree_model_item_queue_subdir_check (model, new_item_list);

    }
    else
    {
        // add a placeholder child item to make the node expandable 
        fm_dir_tree_model_add_place_holder_child_item (model, new_item_list, tp, TRUE);
        
        gtk_tree_path_up (tp);
    }**/
    
    
    fm_dir_tree_model_add_place_holder_child_item (model, new_item_list, tp, TRUE);
    gtk_tree_path_up (tp);

    // Check Subdirectories: check if the dir has subdirs and make it expandable if needed. 
    if (model->check_subdir)
        fm_dir_tree_model_item_queue_subdir_check (model, new_item_list);
    
    return new_item_list;
}


/*****************************************************************************************
 * Add file info to parent node to proper position.
 * GtkTreePath tp is the tree path of parent node.
 * Note that value of tp will be changed inside the function temporarily
 * to generate GtkTreePath for child nodes, and then restored to its
 * original value before returning from the function.
 * 
 ****************************************************************************************/
GList *fm_dir_tree_model_insert_file_info (FmDirTreeModel *model, GList *parent_l, GtkTreePath *tp, FmFileInfo *fi)
{
    GList *item_list;
    
    FmDirTreeItem *parent_item = (FmDirTreeItem*) parent_l->data;
    
    FmDirTreeItem *dir_tree_item = fm_dir_tree_item_new (model, parent_l, fi);

    // Show Hidden Files...
    if (!model->show_hidden && fi->path->name[0] == '.')
    {
        parent_item->hidden_children = g_list_prepend (parent_item->hidden_children, dir_tree_item);
        item_list = parent_item->hidden_children;
    }
    else
        item_list = fm_dir_tree_model_insert_item (model, parent_l, tp, dir_tree_item);
    
    return item_list;
}


/*****************************************************************************************
 * ...
 * 
 * 
 ****************************************************************************************/
void fm_dir_tree_model_remove_item (FmDirTreeModel *model, GList *item_list)
{
    GtkTreePath *tp = fm_dir_tree_model_item_to_tree_path (model, item_list);
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*)item_list->data;
    FmDirTreeItem *parent_item = (FmDirTreeItem*)dir_tree_item->parent ? dir_tree_item->parent->data : NULL;
    
    fm_dir_tree_item_free_l (item_list);
    
    if (parent_item)
        parent_item->children = g_list_delete_link (parent_item->children, item_list);
    
    // signal the view that we removed the placeholder item. 
    char *tmp_path = gtk_tree_path_to_string (tp);
    TREEVIEW_DEBUG ("fm_dir_tree_model_remove_item %s\n", tmp_path);
    g_free (tmp_path);
    
    gtk_tree_model_row_deleted (GTK_TREE_MODEL (model), tp);
    
    gtk_tree_path_free (tp);
}

static void fm_dir_tree_model_remove_all_children (FmDirTreeModel *model, GList *item_list, GtkTreePath *tp)
{
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*)item_list->data;
    if (G_UNLIKELY (!dir_tree_item->children))
        return;
    
    gtk_tree_path_append_index (tp, 0);
    
    while (dir_tree_item->children)
    {
        fm_dir_tree_item_free_l (dir_tree_item->children);
        dir_tree_item->children = g_list_delete_link (dir_tree_item->children, dir_tree_item->children);
        
        // signal the view that we removed the placeholder item.
        char *tmp_path = gtk_tree_path_to_string (tp);
        TREEVIEW_DEBUG ("fm_dir_tree_model_remove_all_children %s\n", tmp_path);
        g_free (tmp_path);
        
        
        gtk_tree_model_row_deleted (GTK_TREE_MODEL (model), tp);
        // everytime we remove the first item, its next item became the
        // first item, so there is no need to update tp.
    }

    if (dir_tree_item->hidden_children)
    {
        g_list_foreach (dir_tree_item->hidden_children, (GFunc)fm_dir_tree_item_free, NULL);
        g_list_free (dir_tree_item->hidden_children);
        dir_tree_item->hidden_children = NULL;
    }
    
    gtk_tree_path_up (tp);
    
    GtkTreeIter it;
    gtk_tree_model_row_has_child_toggled (GTK_TREE_MODEL (model), tp, &it);
    
    //~ gtk_tree_model_row_changed ((GtkTreeModel*) model, tp, &it);
}


/*****************************************************************************************
 * ...
 * 
 * 
 ****************************************************************************************/
void fm_dir_tree_model_expand_row (FmDirTreeModel *model, GtkTreeIter *it, GtkTreePath *tp)
{
    
    TREEVIEW_DEBUG ("fm_dir_tree_model_expand_row\n");
    
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
            FmDirTreeModel *model = dir_tree_item->model;
            
            GtkTreePath *tp = fm_dir_tree_model_item_to_tree_path (model, item_list);
            
            GList *file_l;
            for (file_l = fm_list_peek_head_link (folder->files); file_l; file_l = file_l->next)
            {
                FmFileInfo *fi = file_l->data;
                if (!fm_file_info_is_dir (fi))
                    continue;
                
                /*********************************************************************
                 * Later (in 2050) we can try to support adding files to the tree
                 * too, so this model will be still useless but with cooool features.
                 * 
                 * :)
                 * 
                 ********************************************************************/
                fm_dir_tree_model_insert_file_info (model, item_list, tp, fi);
                
            }
            
            gtk_tree_path_free (tp);
            
            fm_dir_tree_item_on_folder_loaded (dir_tree_item);
        }
    }
    
    ++dir_tree_item->n_expand;
}

void fm_dir_tree_model_collapse_row (FmDirTreeModel *model, GtkTreeIter *it, GtkTreePath *tp)
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
        fm_dir_tree_model_remove_all_children (model, item_list, tp);

        /*** now, GtkTreeView think that we have no child since all
         * child items are removed. So we add a place holder child
         * item to keep the expander around. ***/
        
        fm_dir_tree_model_add_place_holder_child_item (model, item_list, tp, TRUE);
    }
}


void fm_dir_tree_model_set_icon_size (FmDirTreeModel *model, guint icon_size)
{
    if (model->icon_size != icon_size)
    {
        // Reload existing icons...
        GtkTreePath *tp = gtk_tree_path_new_first ();
        GList *l;
        for (l = model->roots; l; l=l->next)
        {
            fm_dir_tree_model_item_reload_icon (model, (FmDirTreeItem*) l, tp);
            gtk_tree_path_next (tp);
        }
        gtk_tree_path_free (tp);
    }
}

guint fm_dir_tree_get_icon_size (FmDirTreeModel *model)
{
    return model->icon_size;
}

void fm_dir_tree_model_set_show_hidden (FmDirTreeModel *model, gboolean show_hidden)
{
    g_return_if_fail (model);
    
    model->show_hidden = show_hidden;
    return;
    
    if (show_hidden != model->show_hidden)
    {
        // Filter the model to hide hidden folders...
        if (model->show_hidden)
        {

        }
        else
        {

        }
    }
}

gboolean fm_dir_tree_model_get_show_hidden (FmDirTreeModel *model)
{
    return model->show_hidden;
}

// Currently Unused...
#if 0
static void item_show_hidden_children (FmDirTreeModel* model, GList* item_l, gboolean show_hidden)
{
    FmDirTreeItem* item = (FmDirTreeItem*)item_l->data;
//    GList* child_l;
    // TODO_axl: show hidden items
    if (show_hidden)
    {
        while (item->hidden_children)
        {

        }
    }
    else
    {
        while (item->children)
        {

        }
    }
}
#endif


static void on_theme_changed (GtkIconTheme *theme, FmDirTreeModel *model)
{
    GList *l;
    GtkTreePath *tp = gtk_tree_path_new_first ();
    for (l = model->roots; l; l=l->next)
    {
        fm_dir_tree_model_item_reload_icon (model, l->data, tp);
        gtk_tree_path_next (tp);
    }
    gtk_tree_path_free (tp);
}

static void fm_dir_tree_model_item_reload_icon (FmDirTreeModel *model, FmDirTreeItem *dir_tree_item, GtkTreePath *tp)
{
    GtkTreeIter it;
    GList *l;
    FmDirTreeItem *child;

    if (dir_tree_item->icon)
    {
        g_object_unref (dir_tree_item->icon);
        dir_tree_item->icon = NULL;
        gtk_tree_model_row_changed (GTK_TREE_MODEL (model), tp, &it);
    }

    if (dir_tree_item->children)
    {
        gtk_tree_path_append_index (tp, 0);
        for (l = dir_tree_item->children; l; l=l->next)
        {
            child = (FmDirTreeItem*) l->data;
            fm_dir_tree_model_item_reload_icon (model, child, tp);
            gtk_tree_path_next (tp);
        }
        gtk_tree_path_up (tp);
    }

    for (l = dir_tree_item->hidden_children; l; l=l->next)
    {
        child = (FmDirTreeItem*) l->data;
        if (child->icon)
        {
            g_object_unref (child->icon);
            child->icon = NULL;
        }
    }
}


/*****************************************************************************************
 * Check Subdirectories:
 * 
 * check if dirs contain sub dir in another thread and make
 * the tree nodes expandable when needed.
 *
 * NOTE From PCMan: Doing this can improve usability, but due to limitation of UNIX-
 * like systems, this can result in great waste of system resources.
 * This requires continuous monitoring of every dir listed in the tree.
 * With Linux, inotify supports this well, and GFileMonitor uses inotify.
 * However, in other UNIX-like systems, monitoring a file uses a file
 * descriptor. So the max number of files which can be monitored is limited
 * by number available file descriptors. This may potentially use up
 * all available file descriptors in the process when there are many
 * directories expanded in the dir tree.
 * So, after considering and experimenting with this, we decided not to
 * support this feature.
 *
 ****************************************************************************************/
void fm_dir_tree_model_item_queue_subdir_check (FmDirTreeModel *model, GList *item_list)
{
    FmDirTreeItem *dir_tree_item = (FmDirTreeItem*)item_list->data;
    g_return_if_fail (dir_tree_item->fi != NULL);

    g_mutex_lock (model->subdir_checks_mutex);
    
    g_queue_push_tail (&model->subdir_checks, item_list);
    NO_DEBUG ("queue subdir check for %s\n", fm_file_info_get_disp_name (dir_tree_item->fi));
    
    if (!model->job_running)
    {
        model->job_running = TRUE;
        model->current_subdir_check = (GList*)g_queue_peek_head (&model->subdir_checks);
        g_cancellable_reset (model->subdir_cancellable);
        g_io_scheduler_push_job (subdir_check_job,
                                g_object_ref (model),
                                (GDestroyNotify)g_object_unref,
                                G_PRIORITY_DEFAULT,
                                model->subdir_cancellable);
        NO_DEBUG ("push job\n");
    }
    
    g_mutex_unlock (model->subdir_checks_mutex);
}

static gboolean subdir_check_job (GIOSchedulerJob *job, GCancellable *cancellable, gpointer user_data)
{
    FmDirTreeModel *model = FM_DIR_TREE_MODEL (user_data);
    GList *item_list;
    FmDirTreeItem *dir_tree_item;
    GFile *gf;
    GFileEnumerator *enu;
    gboolean has_subdir = FALSE;

    g_mutex_lock (model->subdir_checks_mutex);
    item_list = (GList*)g_queue_pop_head (&model->subdir_checks);
    dir_tree_item = (FmDirTreeItem*)item_list->data;
    model->current_subdir_check = item_list;
    
    // check if this item has subdir 
    gf = fm_path_to_gfile (dir_tree_item->fi->path);
    g_mutex_unlock (model->subdir_checks_mutex);
    
    NO_DEBUG ("check subdir for: %s\n", g_file_get_parse_name (gf));
    
    enu = g_file_enumerate_children (gf,
                            G_FILE_ATTRIBUTE_STANDARD_NAME","
                            G_FILE_ATTRIBUTE_STANDARD_TYPE","
                            G_FILE_ATTRIBUTE_STANDARD_IS_HIDDEN,
                            0, cancellable, NULL);
    if (enu)
    {
        while (!g_cancellable_is_cancelled (cancellable))
        {
            GFileInfo *fi = g_file_enumerator_next_file (enu, cancellable, NULL);
            if (G_LIKELY (fi))
            {
                GFileType type = g_file_info_get_file_type (fi);
                gboolean is_hidden = g_file_info_get_is_hidden (fi);
                g_object_unref (fi);

                if (type == G_FILE_TYPE_DIRECTORY)
                {
                    if (model->show_hidden || !is_hidden)
                    {
                        has_subdir = TRUE;
                        break;
                    }
                }
            }
            else
                break;
        }
        GError *error = NULL;
        g_file_enumerator_close (enu, cancellable, &error);
        g_object_unref (enu);
    }
    
    NO_DEBUG ("check result - %s has_dir: %d\n", g_file_get_parse_name (gf), has_subdir);
    g_object_unref (gf);
    
    /** TODO_axl: this may be useless now... may remove it...
    if (has_subdir)
        return g_io_scheduler_job_send_to_mainloop (job, (GSourceFunc) subdir_check_finish_has_subdir, model, NULL);
    **/
        
        
    if (!has_subdir)
        return g_io_scheduler_job_send_to_mainloop (job, (GSourceFunc) subdir_check_remove_place_holder, model, NULL);
        
    return subdir_check_finish (model);
}


/** TODO_axl: this may be useless now... may remove it...
static gboolean subdir_check_finish_has_subdir (FmDirTreeModel *model)
{
    GList *item_list = model->current_subdir_check;
    
    if (!g_cancellable_is_cancelled (model->subdir_cancellable) && item_list)
    {
//        FmDirTreeItem *dir_tree_item = (FmDirTreeItem*)item_list->data;
        GtkTreePath *tp = fm_dir_tree_model_item_to_tree_path (model, item_list);
        
        // TODO_axl: test and remove...
        //~ if (!model->check_subdir_mode2)
            //~ fm_dir_tree_model_add_place_holder_child_item (model, item_list, tp, TRUE);
        
        gtk_tree_path_free (tp);
        
        NO_DEBUG ("finished for item with subdir: %s\n", fm_file_info_get_disp_name (dir_tree_item->fi));
    }
    return subdir_check_finish (model);
} **/

static gboolean subdir_check_remove_place_holder (FmDirTreeModel *model)
{
    GList *item_list = model->current_subdir_check;
    if (!g_cancellable_is_cancelled (model->subdir_cancellable) && item_list)
    {
        FmDirTreeItem *dir_tree_item = (FmDirTreeItem*)item_list->data;
        
        if (dir_tree_item->children) // remove existing subdirs or place holder item if needed. 
        {
            
            // Remove the place holder...
            TREEVIEW_DEBUG ("remove place holder for %s\n", fm_file_info_get_disp_name (dir_tree_item->fi));
            
            GtkTreePath *tp = fm_dir_tree_model_item_to_tree_path (model, item_list);
            fm_dir_tree_model_remove_all_children (model, item_list, tp);
            gtk_tree_path_free (tp);
            
            //~ fm_dir_tree_model_remove_item (model, item_list);
            
        }
    }
    return subdir_check_finish (model);
}

static gboolean subdir_check_finish (FmDirTreeModel *model)
{
    model->current_subdir_check = NULL;
    if (g_queue_is_empty (&model->subdir_checks))
    {
        model->job_running = FALSE;
        TREEVIEW_DEBUG ("subdir_check_finish: all subdir checks are finished !\n");
        return FALSE;
    }
    else // still has queued items 
    {
        if (g_cancellable_is_cancelled (model->subdir_cancellable))
            g_cancellable_reset (model->subdir_cancellable);
    }
    return TRUE;
}



