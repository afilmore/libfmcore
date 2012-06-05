/***********************************************************************************************************************
 * 
 *      fm-files-ops.h
 *
 *      Copyright 2009 PCMan <pcman@debian>
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
 * 
 **********************************************************************************************************************/
#ifndef __FM_FILE_OPS_H__
#define __FM_FILE_OPS_H__

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <stdarg.h>

#include "fm-path-list.h"
#include "fm-file-ops-job.h"

G_BEGIN_DECLS

// File operations
void fm_copy_files (GtkWindow *parent, FmPathList *files, FmPath *dest_dir);
void fm_move_files (GtkWindow *parent, FmPathList *files, FmPath *dest_dir);

#define fm_copy_file(parent, file, dest_dir) \
    G_STMT_START {    \
        FmPathList *files = fm_path_list_new (); \
        fm_list_push_tail (files, file); \
        fm_copy_files (parent, files, dest_dir); \
        fm_list_unref (files);   \
    } G_STMT_END

#define fm_move_file(parent, file, dest_dir) \
    G_STMT_START {    \
    FmPathList *files = fm_path_list_new (); \
    fm_list_push_tail (files, file); \
    fm_move_files (parent, files, dest_dir); \
    fm_list_unref (files);   \
    } G_STMT_END

void fm_move_or_copy_files_to (GtkWindow *parent, FmPathList *files, gboolean is_move);
#define fm_move_files_to(parent, files) fm_move_or_copy_files_to (parent, files, TRUE)
#define fm_copy_files_to(parent, files) fm_move_or_copy_files_to (parent, files, FALSE)

// void fm_rename_files (FmPathList *files);
void fm_rename_file (GtkWindow *parent, FmPath *file);



G_END_DECLS
#endif



