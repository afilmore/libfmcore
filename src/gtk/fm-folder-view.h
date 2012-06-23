/***********************************************************************************************************************
 * 
 *      folder-view.h
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
 *      Purpose: The folder view is a ScrolledWindow, it creates an ExoIconView or an ExoTreeView according
 *               to the selected mode.
 * 
 * 
 **********************************************************************************************************************/
#ifndef __FOLDER_VIEW_H__
#define __FOLDER_VIEW_H__

#include <gtk/gtk.h>

#include "fm-folder.h"
#include "fm-folder-model.h"

#include "fm-file-info.h"
#include "fm-path.h"

#include "fm-dnd-src.h"
#include "fm-dnd-dest.h"


G_BEGIN_DECLS

#define FM_TYPE_FOLDER_VIEW             (fm_folder_view_get_type ())
#define FM_FOLDER_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), FM_TYPE_FOLDER_VIEW, FmFolderView))
#define FM_FOLDER_VIEW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  FM_TYPE_FOLDER_VIEW, FmFolderViewClass))
#define IS_FM_FOLDER_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FM_TYPE_FOLDER_VIEW))
#define IS_FM_FOLDER_VIEW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  FM_TYPE_FOLDER_VIEW))

enum _FmFolderViewMode
{
    FM_FV_ICON_VIEW,
    FM_FV_COMPACT_VIEW,
    FM_FV_THUMBNAIL_VIEW,
    FM_FV_LIST_VIEW
};

typedef enum _FmFolderViewMode FmFolderViewMode;

#define FM_FOLDER_VIEW_MODE_IS_VALID(mode) (mode >= FM_FV_ICON_VIEW && mode <= FM_FV_LIST_VIEW)

enum _FmFolderViewClickType
{
    FM_FV_CLICK_NONE,
    FM_FV_ACTIVATED,
    FM_FV_MIDDLE_CLICK,
    FM_FV_CONTEXT_MENU
};

typedef enum _FmFolderViewClickType FmFolderViewClickType;

#define FM_FOLDER_VIEW_CLICK_TYPE_IS_VALID(type) (type > FM_FV_CLICK_NONE && type <= FM_FV_CONTEXT_MENU)

typedef struct _FmFolderView        FmFolderView;
typedef struct _FmFolderViewClass   FmFolderViewClass;

struct _FmFolderView
{
    GtkScrolledWindow   parent;

    GtkTreeModel        *model;
    GtkWidget           *current_view;
    FmPath              *cwd;
    FmFolder            *folder;
    GtkCellRenderer     *renderer_pixbuf;
    guint               icon_size_changed_handler;

    FmDndSrc            *dnd_src;
    FmDndDest           *dnd_dest;

    FmFolderViewMode    mode;
    GtkSelectionMode    sel_mode;
    GtkSortType         sort_type;
    int                 sort_by;
    gboolean            show_hidden;

    uint                small_icon_size;
    uint                big_icon_size;
    gboolean            single_click;
    
    /*** Wordarounds a gtk+ bug introduced in gtk+ 2.20:
         https://bugzilla.gnome.org/show_bug.cgi?id=612802 ***/
    GtkTreeRowReference *activated_row_ref;
    guint               row_activated_idle;
};

struct _FmFolderViewClass
{
    GtkScrolledWindowClass      parent_class;
    
    void (*directory_changed)   (FmFolderView *folder_view, FmPath *dir_path);
    void (*loaded)              (FmFolderView *folder_view, FmPath *dir_path);
    void (*status)              (FmFolderView *folder_view, const char *msg);
    void (*clicked)             (FmFolderView *folder_view, FmFolderViewClickType type, FmFileInfo *file);
    void (*sel_changed)         (FmFolderView *folder_view, FmFileInfoList *sels);
    void (*sort_changed)        (FmFolderView *folder_view);
};


/***************************************************************************************************
 * The set_mode function creates the ExoIconView or ExoTreeView according to the given mode...
 * fm_folder_view_new () calls the set_mode function to create a view widget.
 * 
 **************************************************************************************************/
GtkWidget *         fm_folder_view_new              (FmFolderViewMode mode);
GType               fm_folder_view_get_type         ();

void                fm_folder_view_set_mode         (FmFolderView *folder_view, FmFolderViewMode mode);
FmFolderViewMode    fm_folder_view_get_mode         (FmFolderView *folder_view);

FmFolderModel *     fm_folder_view_get_model        (FmFolderView *folder_view);

void                fm_folder_view_set_show_hidden  (FmFolderView *folder_view, gboolean show);
gboolean            fm_folder_view_get_show_hidden  (FmFolderView *folder_view);


/***************************************************************************************************
 * 
 * 
 * 
 **************************************************************************************************/
FmFolder *fm_folder_view_get_folder                 (FmFolderView *folder_view);
gboolean fm_folder_view_get_is_loaded               (FmFolderView *folder_view);
gboolean fm_folder_view_chdir                       (FmFolderView *folder_view, FmPath *path);
gboolean fm_folder_view_chdir_by_name               (FmFolderView *folder_view, const char *path_str);
FmPath *fm_folder_view_get_cwd                      (FmFolderView *folder_view);
FmFileInfo *fm_folder_view_get_cwd_info             (FmFolderView *folder_view);


/***************************************************************************************************
 * View Selection...
 * 
 * 
 **************************************************************************************************/
void fm_folder_view_set_selection_mode              (FmFolderView *folder_view, GtkSelectionMode mode);
GtkSelectionMode fm_folder_view_get_selection_mode  (FmFolderView *folder_view);

void fm_folder_view_select_all                      (FmFolderView *folder_view);
void fm_folder_view_select_invert                   (FmFolderView *folder_view);

void fm_folder_view_select_file_path                (FmFolderView *folder_view, FmPath *path);
void fm_folder_view_select_file_paths               (FmFolderView *folder_view, FmPathList *paths);

FmFileInfoList *fm_folder_view_get_selected_files   (FmFolderView *folder_view);
FmPathList *fm_folder_view_get_selected_file_paths  (FmFolderView *folder_view);


/***************************************************************************************************
 * View Sorting...
 * 
 * 
 **************************************************************************************************/
void                fm_folder_view_sort             (FmFolderView *folder_view, GtkSortType type, int by);
GtkSortType         fm_folder_view_get_sort_type    (FmFolderView *folder_view);
int                 fm_folder_view_get_sort_by      (FmFolderView *folder_view);


// select files by custom func, not yet implemented
void fm_folder_view_custom_select                   (FmFolderView *folder_view, GFunc filter, gpointer user_data);


G_END_DECLS
#endif



