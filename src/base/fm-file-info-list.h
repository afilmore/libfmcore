/***********************************************************************************************************************
 * 
 *      fm-file-info-list.h
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
#ifndef _FM_FILE_INFO_LIST_H_
#define _FM_FILE_INFO_LIST_H_

#include <glib.h>
#include <sys/types.h>

#include "fm-list.h"
#include "fm-file-info.h"

G_BEGIN_DECLS


typedef FmList FmFileInfoList;


gboolean        fm_list_is_file_info_list               (FmList *list);
FmFileInfoList *fm_file_info_list_new ();
FmFileInfoList *fm_file_info_list_new_from_glist        ();

// Return TRUE if all files in the list are of the same type
// used by file properties and file popup...
gboolean        fm_file_info_list_is_same_type          (FmFileInfoList *list);

// Return TRUE if all files in the list are on the same fs
// used by popup menu...
gboolean        fm_file_info_list_is_same_fs            (FmFileInfoList *list);

int             fm_file_info_list_get_flags             (FmFileInfoList *list, uint *or_flags, uint *and_flags);


G_END_DECLS
#endif


