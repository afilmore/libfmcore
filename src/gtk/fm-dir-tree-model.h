/***********************************************************************************************************************
 * 
 *      fm-dir-tree-model.h
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
 * 
 **********************************************************************************************************************/
#ifndef __FM_DIR_TREE_MODEL_H__
#define __FM_DIR_TREE_MODEL_H__

#include <gtk/gtk.h>
#include <glib-object.h>
#include "fm-file-info.h"


G_BEGIN_DECLS

#define FM_TYPE_DIR_TREE_MODEL              (fm_dir_tree_model_get_type())
#define FM_DIR_TREE_MODEL(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj),  FM_TYPE_DIR_TREE_MODEL, FmDirTreeModel))
#define FM_DIR_TREE_MODEL_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass),   FM_TYPE_DIR_TREE_MODEL, \
                                             FmDirTreeModelClass))
#define FM_IS_DIR_TREE_MODEL(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj),  FM_TYPE_DIR_TREE_MODEL))
#define FM_IS_DIR_TREE_MODEL_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE((klass),   FM_TYPE_DIR_TREE_MODEL))
#define FM_DIR_TREE_MODEL_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj),   FM_TYPE_DIR_TREE_MODEL, \
                                             FmDirTreeModelClass))

// Columns of dir tree view...
enum {
    FM_DIR_TREE_MODEL_COL_ICON,
    FM_DIR_TREE_MODEL_COL_DISP_NAME,
    FM_DIR_TREE_MODEL_COL_INFO,
    FM_DIR_TREE_MODEL_COL_PATH,
    FM_DIR_TREE_MODEL_COL_FOLDER,
    N_FM_DIR_TREE_MODEL_COLS
};

typedef struct _FmDirTreeModel              FmDirTreeModel;
typedef struct _FmDirTreeModelClass         FmDirTreeModelClass;

struct _FmDirTreeModel
{
    GObject         parent;
    GList           *roots;
    gint            stamp;
    int             icon_size;
    gboolean        show_hidden;
    
    gboolean        check_subdir;
    gboolean        check_subdir_mode2;
    
    
    // Subdirectory Check Job, this permits to expend only folders which have subfolders...
    gboolean        job_running;
    
    GQueue          subdir_checks;
    GList           *current_subdir_check;
    
    GMutex          *subdir_checks_mutex;
    GCancellable    *subdir_cancellable;
};

struct _FmDirTreeModelClass
{
    GObjectClass parent_class;
};


FmDirTreeModel *fm_dir_tree_model_new       ();
GType fm_dir_tree_model_get_type            ();

void fm_dir_tree_model_add_root             (FmDirTreeModel *model, FmFileInfo *root, GtkTreeIter *it, gboolean expand);

void fm_dir_tree_model_expand_row           (FmDirTreeModel *model, GtkTreeIter *it, GtkTreePath *tp);
void fm_dir_tree_model_collapse_row         (FmDirTreeModel *model, GtkTreeIter *it, GtkTreePath *tp);

void fm_dir_tree_model_set_icon_size        (FmDirTreeModel *model, guint icon_size);
guint fm_dir_tree_get_icon_size             (FmDirTreeModel *model);

void fm_dir_tree_model_set_show_hidden      (FmDirTreeModel *model, gboolean show_hidden);
gboolean fm_dir_tree_model_get_show_hidden  (FmDirTreeModel *model);

// for FmDirTreeView, called in fm_dir_tree_view_init () 
gboolean fm_dir_tree_model_get_iter  (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path);
//~ gboolean fm_dir_tree_view_select_function   (GtkTreeSelection *selection, GtkTreeModel *model, GtkTreePath *path,
                                             //~ gboolean path_currently_selected, gpointer data);

void fm_dir_tree_model_remove_item          (FmDirTreeModel *model, GList *item_list);
GList *fm_dir_tree_model_insert_file_info   (FmDirTreeModel *model, GList *parent_l, GtkTreePath *tp, FmFileInfo *fi);
inline void fm_dir_tree_model_item_to_tree_iter (FmDirTreeModel *model, GList *item_list, GtkTreeIter *it);
inline GtkTreePath *fm_dir_tree_model_item_to_tree_path (FmDirTreeModel *model, GList *item_list);
void fm_dir_tree_model_item_queue_subdir_check (FmDirTreeModel *model, GList *item_list);
GList *fm_dir_tree_model_children_by_name   (FmDirTreeModel *model, GList *children, const char *name, int *idx);


G_END_DECLS
#endif


