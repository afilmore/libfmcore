/***********************************************************************************************************************
 * 
 *      fm-folder-model.h
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
#ifndef _FM_FOLDER_MODEL_H_
#define _FM_FOLDER_MODEL_H_

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <sys/types.h>

#include "fm-folder.h"

G_BEGIN_DECLS

#define FM_TYPE_FOLDER_MODEL             (fm_folder_model_get_type())
#define FM_FOLDER_MODEL(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),  FM_TYPE_FOLDER_MODEL, FmFolderModel))
#define FM_FOLDER_MODEL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  FM_TYPE_FOLDER_MODEL, FmFolderModelClass))
#define FM_IS_FOLDER_MODEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FM_TYPE_FOLDER_MODEL))
#define FM_IS_FOLDER_MODEL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  FM_TYPE_FOLDER_MODEL))
#define FM_FOLDER_MODEL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj),  FM_TYPE_FOLDER_MODEL, FmFolderModelClass))


// Columns Of The Folder Model...
enum {
    COL_FILE_GICON = 0,
    COL_FILE_ICON,
    COL_FILE_NAME,
    COL_FILE_SIZE,
    COL_FILE_DESC,
    COL_FILE_PERM,
    COL_FILE_OWNER,
    COL_FILE_MTIME,
    COL_FILE_INFO,
    N_FOLDER_MODEL_COLS
};

#define FM_FOLDER_MODEL_COL_IS_VALID(col) (col >= COL_FILE_GICON && col < N_FOLDER_MODEL_COLS)

typedef struct _FmFolderModel       FmFolderModel;
typedef struct _FmFolderModelClass  FmFolderModelClass;

struct _FmFolderModel
{
    GObject     parent;
    
    FmFolder    *directory;
    GSequence   *items;
    GSequence   *hidden;    // items hidden by filter...

    gboolean    show_hidden : 1;

    int         sort_col;
    GtkSortType sort_order;
    
    gint        stamp;      // random integer to check whether an iter belongs to our model...

    guint       theme_change_handler;
    guint       icon_size;

    guint       thumbnail_max;
    GList       *thumbnail_requests;
};

struct _FmFolderModelClass
{
    GObjectClass parent;
    
    void (*loaded) (FmFolderModel *model);  // default signal handler...

};

FmFolderModel *fm_folder_model_new                  (FmFolder *directory, gboolean show_hidden);
GType fm_folder_model_get_type                      ();

void fm_folder_model_set_folder                     (FmFolderModel *model, FmFolder *directory);
gboolean fm_folder_model_get_is_loaded              (FmFolderModel *model);

void fm_folder_model_set_icon_size                  (FmFolderModel *model, guint icon_size);
guint fm_folder_model_get_icon_size                 (FmFolderModel *model);

void fm_folder_model_set_show_hidden                (FmFolderModel *model, gboolean show_hidden);
gboolean fm_folder_model_get_show_hidden            (FmFolderModel *model);

void fm_folder_model_file_created                   (FmFolderModel *model, FmFileInfo *file);
void fm_folder_model_file_deleted                   (FmFolderModel *model, FmFileInfo *file);
void fm_folder_model_file_changed                   (FmFolderModel *model, FmFileInfo *file);

void fm_folder_model_get_common_suffix_for_prefix   (FmFolderModel *model, const gchar *prefix,
                                                     gboolean (*file_info_predicate) (FmFileInfo*),
                                                     gchar *common_suffix);

gboolean fm_folder_model_find_iter_by_filename      (FmFolderModel *model, GtkTreeIter *it, const char *name);


/*** void fm_folder_model_set_thumbnail_size(FmFolderModel* model, guint size); ***/
/*** gboolean fm_folder_model_find_iter(  FmFolderModel* list, GtkTreeIter* it, FmFileInfo* file_info ); ***/

G_END_DECLS
#endif


