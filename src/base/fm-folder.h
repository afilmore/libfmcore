/***********************************************************************************************************************
 * 
 *      fm-folder.h
 *
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
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
#ifndef __FM_FOLDER_H__
#define __FM_FOLDER_H__


#include <glib-object.h>
#include <gio/gio.h>

#include "fm-path-list.h"
#include "fm-dir-list-job.h"
#include "fm-file-info-list.h"
#include "fm-vala.h"
#include "fm-file-info-job.h"


G_BEGIN_DECLS

#define FM_TYPE_FOLDER              (fm_folder_get_type())
#define FM_FOLDER(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj),  FM_TYPE_FOLDER, FmFolder))
#define FM_FOLDER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass),   FM_TYPE_FOLDER, FmFolderClass))
#define FM_IS_FOLDER(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj),  FM_TYPE_FOLDER))
#define FM_IS_FOLDER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE((klass),   FM_TYPE_FOLDER))


typedef struct _FmFolder            FmFolder;
typedef struct _FmFolderClass       FmFolderClass;

struct _FmFolder
{
    GObject         parent;

    // Directory informations...
    GFile          *gfile;
    
    FmPath         *dir_path;
    FmFileInfo     *dir_file_info;
    
    GFileMonitor   *file_monitor;

    // Job to query files/folders into that directory...
    FmDirListJob   *dir_list_job;
    FmFileInfoList *files;
    gboolean        is_loaded : 1;
    
    
    
    // For the file monitor...
    guint           idle_handler;
    GSList         *files_to_add;
    GSList         *files_to_update;
    GSList         *files_to_del;
    
    GSList         *pending_jobs;
    
    // Filesystem infos...
    guint64         fs_total_size;
    guint64         fs_free_size;
    GCancellable   *fs_size_cancellable;
    gboolean        has_fs_info : 1;
    gboolean        fs_info_not_avail : 1;
};

struct _FmFolderClass
{
    GObjectClass    parent_class;

    void            (*files_added)      (FmFolder *folder, GSList *files);
    void            (*files_removed)    (FmFolder *folder, GSList *files);
    void            (*files_changed)    (FmFolder *folder, GSList *files);
    
    void            (*loaded)           (FmFolder *folder);
    void            (*unmount)          (FmFolder *folder);
    void            (*changed)          (FmFolder *folder);
    void            (*removed)          (FmFolder *folder);
    
    void            (*content_changed)  (FmFolder *folder);
    
    void            (*fs_info)          (FmFolder *folder);
    
    FmErrorAction   (*error)            (FmFolder *folder, GError *err, FmSeverity severity);
};


GType           fm_folder_get_type              (void);

FmFolder        *fm_folder_get                  (FmPath *path);

FmFolder        *fm_folder_get_for_path_name    (const char *path);
FmFolder        *fm_folder_get_for_gfile        (GFile *gfile);
FmFolder        *fm_folder_get_for_uri          (const char *uri);

FmFileInfo      *fm_folder_get_directory_info   (FmFolder *folder);

FmFileInfoList  *fm_folder_get_files            (FmFolder *folder);
FmFileInfo      *fm_folder_get_file_by_name     (FmFolder *folder, const char *name);

gboolean        fm_folder_get_is_loaded         (FmFolder *folder);

void            fm_folder_query                 (FmFolder *folder);

gboolean        fm_folder_get_filesystem_info   (FmFolder *folder, guint64 *total_size, guint64 *free_size);
void            fm_folder_query_filesystem_info (FmFolder *folder);

G_END_DECLS
#endif




