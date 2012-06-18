/***********************************************************************************************************************
 * 
 *      fm-path-list.h
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
#ifndef __FM_PATH_LIST_H__
#define __FM_PATH_LIST_H__

//~ #include <glib.h>
//~ #include <gio/gio.h>

#include "fm-list.h"
#include "fm-path.h"

G_BEGIN_DECLS

typedef FmList FmPathList;

// path list
FmPathList *fm_path_list_new ();
FmPathList *fm_path_list_new_from_uri_list          (const char *uri_list);
FmPathList *fm_path_list_new_from_uris              (const char **uris);
FmPathList *fm_path_list_new_from_file_info_list    (FmList *fis);
FmPathList *fm_path_list_new_from_file_info_glist   (GList *fis);
FmPathList *fm_path_list_new_from_file_info_gslist  (GSList *fis);

gboolean    fm_list_is_path_list                    (FmList *list);

char *      fm_path_list_to_uri_list                (FmPathList *path_list);
void        fm_path_list_write_uri_list             (FmPathList *path_list, GString *buf);

int         fm_path_list_get_flags                  (FmPathList *path_list, uint *or_flags, uint *and_flags);
gboolean    fm_path_list_all_in_trash_can           (FmPathList *path_list);

// char **  fm_path_list_to_uris                      (FmPathList *path_list);

G_END_DECLS
#endif


