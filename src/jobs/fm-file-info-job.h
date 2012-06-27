/***********************************************************************************************************************
 * 
 *      fm-file-info-job.h
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
 *      Purpose: A FileInfo job.
 * 
 *      The FileInfoJob is used by the FmFolder, FmDirTreeModel and 
 * 
 * 
 **********************************************************************************************************************/
#ifndef __FM_FILE_INFO_JOB_H__
#define __FM_FILE_INFO_JOB_H__

#include "fm-vala.h"
#include "fm-file-info-list.h"
#include "fm-path-list.h"


G_BEGIN_DECLS

#define FM_TYPE_FILE_INFO_JOB			    (fm_file_info_job_get_type())

#define FM_FILE_INFO_JOB(obj)               (G_TYPE_CHECK_INSTANCE_CAST((obj), FM_TYPE_FILE_INFO_JOB, FmFileInfoJob))
#define FM_FILE_INFO_JOB_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST((klass), \
                                             FM_TYPE_FILE_INFO_JOB, FmFileInfoJobClass))

#define IS_FM_FILE_INFO_JOB(obj)            (G_TYPE_CHECK_INSTANCE_TYPE((obj), FM_TYPE_FILE_INFO_JOB))
#define IS_FM_FILE_INFO_JOB_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE((klass), FM_TYPE_FILE_INFO_JOB))

typedef struct _FmFileInfoJob			    FmFileInfoJob;
typedef struct _FmFileInfoJobClass          FmFileInfoJobClass;


enum _FmFileInfoJobFlags
{
    FM_FILE_INFO_JOB_NONE = 0,
    FM_FILE_INFO_JOB_FOLLOW_SYMLINK = 1 << 0,       // Not implemented yet...
    FM_FILE_INFO_JOB_EMIT_FOR_EACH_FILE = 1 << 1    // Not implemented yet...
};

typedef enum _FmFileInfoJobFlags FmFileInfoJobFlags;


struct _FmFileInfoJob
{
	FmJob               parent;
    FmFileInfoJobFlags  flags;
    FmFileInfoList      *file_info_list;
    
    FmPath              *current;
};

struct _FmFileInfoJobClass
{
	FmJobClass parent_class;
};


FmJob           *fm_file_info_job_new           (FmPathList *files_to_query, FmFileInfoJobFlags flags);

GType           fm_file_info_job_get_type       ();

                // This can only be called before running the job...
void            fm_file_info_job_add            (FmFileInfoJob *job, FmPath *path);
void            fm_file_info_job_add_gfile      (FmFileInfoJob *job, GFile *gfile);

                // Should only be called in error handler...
FmPath          *fm_file_info_job_get_current   (FmFileInfoJob *job);

FmFileInfoList  *fm_file_info_job_get_list      (FmFileInfoJob *job);


G_END_DECLS
#endif




