/***********************************************************************************************************************
 * 
 *      fm-dir-tree-view.h
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
#ifndef __FM_DIR_TREE_VIEW_H__
#define __FM_DIR_TREE_VIEW_H__

#include <gtk/gtk.h>

#include "fm-path.h"
#include "fm-folder.h"


G_BEGIN_DECLS

#define FM_TYPE_DIR_TREE_VIEW               (fm_dir_tree_view_get_type())
#define FM_DIR_TREE_VIEW(obj)               (G_TYPE_CHECK_INSTANCE_CAST((obj),  FM_TYPE_DIR_TREE_VIEW, FmDirTreeView))
#define FM_DIR_TREE_VIEW_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST((klass),   FM_TYPE_DIR_TREE_VIEW, \
                                                                                FmDirTreeViewClass))
#define FM_IS_DIR_TREE_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_TYPE((obj),  FM_TYPE_DIR_TREE_VIEW))
#define FM_IS_DIR_TREE_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE((klass),   FM_TYPE_DIR_TREE_VIEW))
#define FM_DIR_TREE_VIEW_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS((obj),   FM_TYPE_DIR_TREE_VIEW, \
                                                                                FmDirTreeViewClass))

typedef struct _FmDirTreeView               FmDirTreeView;
typedef struct _FmDirTreeViewClass          FmDirTreeViewClass;


struct _FmDirTreeView
{
    GtkTreeView     parent;
    
    FmPath          *current_directory;

    // Used for chdir...
    GSList          *paths_to_expand;
    GtkTreeIter     cur_expanded_it;
    FmFolder        *cur_expanded_folder;
};


struct _FmDirTreeViewClass
{
    GtkTreeViewClass            parent_class;
    void (*directory_changed)   (FmDirTreeView *tree_view, guint button, FmPath *path);
};


GObject     *fm_dir_tree_view_new                   ();
GType       fm_dir_tree_view_get_type               ();

void        fm_dir_tree_view_set_current_directory  (FmDirTreeView *tree_view, FmPath *path);
FmPath      *fm_dir_tree_view_get_current_directory (FmDirTreeView *tree_view);


G_END_DECLS
#endif




