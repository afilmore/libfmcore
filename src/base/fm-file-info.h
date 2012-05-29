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
    FmPath          *path;
    
    char            *disp_name;     // UTF-8 Displayed Name
    char            *collate_key;   // Used to sort files by name
    
    FmMimeType      *mime_type;
    
    FmIcon          *fm_icon;       // A GIcon Cache (see base/fm-icon.h)...
    
    char            *target;        // Target of shortcut or mountable...

    mode_t          mode;           // FileSystem Informations...
    
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
    
    int             n_ref;          // Private...
};


// Intialize the file info system
void _fm_file_info_init ();
void _fm_file_info_finalize ();


/*********************************************************************
 * Create Special Items To Display On The Desktop View...
 * 
 * 
 ********************************************************************/
FmFileInfo  *fm_file_info_new_computer          ();
FmFileInfo  *fm_file_info_new_trash_can         ();
FmFileInfo  *fm_file_info_new_user_special_dir  (GUserDirectory directory);

FmFileInfo  *fm_file_info_new_for_path          (FmPath *path);
FmFileInfo  *fm_file_info_new_from_gfileinfo    (FmPath *path, GFileInfo *inf);

void        fm_file_info_set_from_gfileinfo     (FmFileInfo *file_info, GFileInfo *inf);

void        fm_file_info_set_from_desktop_entry (FmFileInfo *file_info);

void        fm_file_info_set_fm_icon            (FmFileInfo *file_info, FmIcon *fm_icon);
FmIcon      *fm_file_info_get_fm_icon           (FmFileInfo *file_info);
GIcon       *fm_file_info_get_gicon             (FmFileInfo *file_info);

FmFileInfo  *fm_file_info_ref                   (FmFileInfo *file_info);
void        fm_file_info_unref                  (FmFileInfo *file_info);
void        fm_file_info_copy                   (FmFileInfo *file_info, FmFileInfo *src);


void        fm_file_info_set_path               (FmFileInfo *file_info, FmPath *path);
FmPath      *fm_file_info_get_path              (FmFileInfo *file_info);

const char  *fm_file_info_get_name              (FmFileInfo *file_info);
void        fm_file_info_set_disp_name          (FmFileInfo *file_info, const char *name);
const char  *fm_file_info_get_disp_name         (FmFileInfo *file_info);

goffset     fm_file_info_get_size               (FmFileInfo *file_info);
const char  *fm_file_info_get_disp_size         (FmFileInfo *file_info);
goffset     fm_file_info_get_blocks             (FmFileInfo *file_info);
mode_t      fm_file_info_get_mode               (FmFileInfo *file_info);
FmMimeType  *fm_file_info_get_mime_type         (FmFileInfo *file_info, gboolean reference);
const char  *fm_file_info_get_target            (FmFileInfo *file_info);

const char  *fm_file_info_get_collate_key       (FmFileInfo *file_info);
const char  *fm_file_info_get_desc              (FmFileInfo *file_info);
const char  *fm_file_info_get_disp_mtime        (FmFileInfo *file_info);
time_t      *fm_file_info_get_mtime             (FmFileInfo *file_info);
time_t      *fm_file_info_get_atime             (FmFileInfo *file_info);


gboolean    fm_file_info_is_dir                 (FmFileInfo *file_info);
gboolean    fm_file_info_is_desktop_entry       (FmFileInfo *file_info);
gboolean    fm_file_info_is_symlink             (FmFileInfo *file_info);
gboolean    fm_file_info_is_shortcut            (FmFileInfo *file_info);
gboolean    fm_file_info_is_mountable           (FmFileInfo *file_info);
gboolean    fm_file_info_is_executable_type     (FmFileInfo *file_info);
gboolean    fm_file_info_is_hidden              (FmFileInfo *file_info);

gboolean    fm_file_info_is_image               (FmFileInfo *file_info);
gboolean    fm_file_info_is_text                (FmFileInfo *file_info);

gboolean    fm_file_info_can_thumbnail          (FmFileInfo *file_info);

gboolean    fm_file_info_is_unknown_type        (FmFileInfo *file_info);

G_END_DECLS
#endif


