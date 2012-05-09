/***********************************************************************************************************************
 * 
 *      fm-file-info.h
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
#ifndef _FM_FILE_INFO_H_
#define _FM_FILE_INFO_H_

#include <glib.h>
#include <gio/gio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fm-path.h"
#include "fm-mime-type.h"
#include "fm-icon.h"
#include "fm-list.h"

G_BEGIN_DECLS

typedef struct _FmFileInfo FmFileInfo;

#define FM_FILE_INFO(ptr) ((FmFileInfo*) ptr)

struct _FmFileInfo
{
    
    /*****************************************************************
     * File Path, that's the most important field of the FileInfo,
     * other ones are set from this path...
     * 
     ****************************************************************/
    // UTF-8 Displayed Name
    char            *disp_name;
    char            *collate_key;   // Used to sort files by name
    
    FmPath          *path;
    
    FmMimeType      *type;          // TODO_axl: rename to mime_type...
    
    // The GIcon Cache (see base/fm-icon.h)...
    
    FmIcon          *icon;          // TODO_axl: rename to fm_icon... avoid any direct member access...

    // Target of shortcut or mountable...
    char            *target;

    // FileSystem Informations...
    mode_t          mode;
    
    union {
        const char  *fs_id;
        dev_t       dev;
    };
    
    uid_t           uid;
    gid_t           gid;
    
    char            *disp_size;     // Displayed human-readable file size
    goffset         size;
    
    char            *disp_mtime;    // Displayed last modification time
    time_t          mtime;
    time_t          atime;
    
    gulong          blksize;
    goffset         blocks;
    
    // Private...
    int             n_ref;
};

// Intialize the file info system
void _fm_file_info_init ();
void _fm_file_info_finalize ();

// TODO_axl: 
/* *******************************************************************
 * TODO_axl: This function should be private, there should be some
 * dedicated functions instead to avoid direct members access...
 * 
 * fm-file-info-job.c and fm-dir-list-job.c use it...
 * 
 ********************************************************************/
FmFileInfo *fm_file_info_new (); 


/*********************************************************************
 * Create Special Items To Display On The Desktop View...
 * 
 * 
 ********************************************************************/
FmFileInfo *fm_file_info_new_computer ();
FmFileInfo *fm_file_info_new_trash_can ();
FmFileInfo *fm_file_info_new_user_special_dir   (GUserDirectory directory);

FmFileInfo *fm_file_info_new_from_gfileinfo     (FmPath *path, GFileInfo *inf);

void fm_file_info_set_from_gfileinfo            (FmFileInfo *fi, GFileInfo *inf);

void fm_file_info_set_from_desktop_entry        (FmFileInfo *file_info);

void fm_file_info_set_fm_icon                   (FmFileInfo *file_info, FmIcon *fm_icon);
GIcon *fm_file_info_get_gicon                   (FmFileInfo *file_info);

FmFileInfo *fm_file_info_ref                    (FmFileInfo *fi);
void fm_file_info_unref                         (FmFileInfo *fi);
void fm_file_info_copy                          (FmFileInfo *fi, FmFileInfo *src);


void fm_file_info_set_path                      (FmFileInfo *fi, FmPath *path);
FmPath *fm_file_info_get_path                   (FmFileInfo *fi);

const char *fm_file_info_get_name               (FmFileInfo *fi);
void fm_file_info_set_disp_name                 (FmFileInfo *fi, const char *name);
const char *fm_file_info_get_disp_name          (FmFileInfo *fi);

goffset fm_file_info_get_size                   (FmFileInfo *fi);
const char *fm_file_info_get_disp_size          (FmFileInfo *fi);
goffset fm_file_info_get_blocks                 (FmFileInfo *fi);
mode_t fm_file_info_get_mode                    (FmFileInfo *fi);
FmMimeType *fm_file_info_get_mime_type          (FmFileInfo *fi);
const char *fm_file_info_get_target             (FmFileInfo *fi);

const char *fm_file_info_get_collate_key        (FmFileInfo *fi);
const char *fm_file_info_get_desc               (FmFileInfo *fi);
const char *fm_file_info_get_disp_mtime         (FmFileInfo *fi);
time_t *fm_file_info_get_mtime                  (FmFileInfo *fi);
time_t *fm_file_info_get_atime                  (FmFileInfo *fi);


gboolean fm_file_info_is_dir                    (FmFileInfo *fi);
gboolean fm_file_info_is_symlink                (FmFileInfo *fi);
gboolean fm_file_info_is_shortcut               (FmFileInfo *fi);
gboolean fm_file_info_is_mountable              (FmFileInfo *fi);
gboolean fm_file_info_is_image                  (FmFileInfo *fi);
gboolean fm_file_info_is_text                   (FmFileInfo *fi);
gboolean fm_file_info_is_desktop_entry          (FmFileInfo *fi);
gboolean fm_file_info_is_unknown_type           (FmFileInfo *fi);
gboolean fm_file_info_is_hidden                 (FmFileInfo *fi);
gboolean fm_file_info_is_executable_type        (FmFileInfo *fi);
gboolean fm_file_info_can_thumbnail             (FmFileInfo *fi);


G_END_DECLS
#endif


