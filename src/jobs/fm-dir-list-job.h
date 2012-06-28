/***********************************************************************************************************************
 * 
 *      fm-dir-list-job.h
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
#ifndef __FM_DIR_LIST_JOB_H__
#define __FM_DIR_LIST_JOB_H__

#include "fm-vala.h"
#include "fm-file-info-list.h"


G_BEGIN_DECLS

#define FM_TYPE_DIR_LIST_JOB            (fm_dir_list_job_get_type())
#define FM_DIR_LIST_JOB(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),  FM_TYPE_DIR_LIST_JOB, FmDirListJob))
#define FM_DIR_LIST_JOB_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),   FM_TYPE_DIR_LIST_JOB, FmDirListJobClass))
#define FM_IS_DIR_LIST_JOB(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),  FM_TYPE_DIR_LIST_JOB))
#define FM_IS_DIR_LIST_JOB_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),   FM_TYPE_DIR_LIST_JOB))

typedef struct _FmDirListJob            FmDirListJob;
typedef struct _FmDirListJobClass       FmDirListJobClass;


struct _FmDirListJob
{
	FmJob           parent;
	
    gboolean        dir_only;
    FmPath         *directory;
    FmFileInfo     *dir_info;
	
    FmFileInfoList *files;
};

struct _FmDirListJobClass
{
	FmJobClass parent_class;
};


GType           fm_dir_list_job_get_type		        ();

FmJob          *fm_dir_list_job_new	                    (FmPath *path, gboolean dir_only);

FmPath         *fm_dir_dist_job_get_directory           (FmDirListJob *dir_list_job);
FmFileInfo     *fm_dir_dist_job_get_directory_info      (FmDirListJob *dir_list_job);
FmFileInfoList *fm_dir_dist_job_get_files               (FmDirListJob *dir_list_job);

G_END_DECLS
#endif



