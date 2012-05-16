/***********************************************************************************************************************
 * 
 *      fm-utils.h
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
#ifndef __FM_UTILS_H__
#define __FM_UTILS_H__

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <stdarg.h>

#include "fm-path-list.h"
#include "fm-file-ops-job.h"

G_BEGIN_DECLS

inline void fm_foreach (GList *list, GFunc func, gpointer user_data);

char *fm_file_size_to_str ( char *buf, goffset size, gboolean si_prefix );
gboolean fm_key_file_get_int (GKeyFile *kf, const char *grp, const char *key, int *val);
gboolean fm_key_file_get_bool (GKeyFile *kf, const char *grp, const char *key, gboolean *val);
char *fm_canonicalize_filename (const char *filename, const char *cwd);
char *fm_str_replace (char *str, char *old, char *new);

// Mount
gboolean fm_mount_path (GtkWindow *parent, FmPath *path, gboolean interactive);
gboolean fm_mount_volume (GtkWindow *parent, GVolume *vol, gboolean interactive);
gboolean fm_unmount_mount (GtkWindow *parent, GMount *mount, gboolean interactive);
gboolean fm_unmount_volume (GtkWindow *parent, GVolume *vol, gboolean interactive);
gboolean fm_eject_mount (GtkWindow *parent, GMount *mount, gboolean interactive);
gboolean fm_eject_volume (GtkWindow *parent, GVolume *vol, gboolean interactive);

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

void fm_trash_files (GtkWindow *parent, FmPathList *files);
void fm_delete_files (GtkWindow *parent, FmPathList *files);

// trash or delete files according to FmConfig::use_trash.
void fm_trash_or_delete_files (GtkWindow *parent, FmPathList *files);

void fm_untrash_files (GtkWindow *parent, FmPathList *files);

// void fm_rename_files (FmPathList *files);
void fm_rename_file (GtkWindow *parent, FmPath *file);

void fm_empty_trash (GtkWindow *parent);

G_END_DECLS
#endif



