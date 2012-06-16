/***********************************************************************************************************************
 * 
 *      fm-path.h
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
#ifndef __FM_PATH_H__
#define __FM_PATH_H__

#include <glib.h>
#include <gio/gio.h>

#include "fm-list.h"

G_BEGIN_DECLS

#define FM_PATH(path)           ((FmPath*)path)
#define FM_PATH_URI_COMPUTER    "computer:///"
#define FM_PATH_URI_TRASH_CAN   "trash:///"

typedef struct _FmPath FmPath;


/*********************************************************************
 * File Info Flags
 * 
 * 
 * 
 * 
 ********************************************************************/
#if 0
enum _FmFileInfoFlag
{
    FM_FILE_INFO_NONE           = 0,
    FM_FILE_INFO_HOME_DIR       = (1 << 0),
    FM_FILE_INFO_DESKTOP_DIR    = (1 << 1),
    FM_FILE_INFO_DESKTOP_ENTRY  = (1 << 2),
    FM_FILE_INFO_MOUNT_POINT    = (1 << 3),
    FM_FILE_INFO_REMOTE         = (1 << 4),
    FM_FILE_INFO_VIRTUAL        = (1 << 5),
    FM_FILE_INFO_TRASH_CAN      = (1 << 6)
};
typedef enum _FmFileInfoFlag FmFileInfoFlag;
#endif

enum _FmPathFlags
{
    FM_PATH_NONE            = 0,
    FM_PATH_IS_NATIVE       = 1 << 0,     // This is a native path to UNIX, like /home
    FM_PATH_IS_LOCAL        = 1 << 1,     // This path refers to a file on local filesystem
    FM_PATH_IS_VIRTUAL      = 1 << 2,     // This path is virtual and it doesn't exist on real filesystem
    FM_PATH_IS_TRASH_ROOT   = 1 << 3,     // Trash Can root...
    FM_PATH_IS_TRASH_FILE   = 1 << 4,     // A file in the trash can (trash:///)...
    FM_PATH_IS_XDG_MENU     = 1 << 5      // This path is under menu:///
};
typedef enum _FmPathFlags FmPathFlags;

struct _FmPath
{
    gint    n_ref;
    
    FmPath  *parent;
    
    uint    flags;       // FmPathFlags flags : 32;
    
    char    name [1];
};


void _fm_path_init ();


FmPath *fm_path_new_for_str             (const char *path_str);
FmPath *fm_path_new_for_path            (const char *path_name);
FmPath *fm_path_new_for_uri             (const char *uri);
FmPath *fm_path_new_for_display_name    (const char *path_name);
FmPath *fm_path_new_for_commandline_arg (const char *arg);

FmPath *fm_path_new_child               (FmPath *parent, const char *basename);
FmPath *fm_path_new_child_len           (FmPath *parent, const char *basename, int name_len);
FmPath *fm_path_new_relative            (FmPath *parent, const char *relative_path);
FmPath *fm_path_new_for_gfile           (GFile *gf);

// predefined paths
FmPath *fm_path_get_root                (); // /
FmPath *fm_path_get_home                (); // home directory
FmPath *fm_path_get_desktop             (); // $HOME/Desktop
FmPath *fm_path_get_trash               (); // trash:///
FmPath *fm_path_get_apps_menu           (); // menu://applications.menu/

FmPath *fm_path_ref                     (FmPath *path);
void fm_path_unref                      (FmPath *path);

FmPath *fm_path_get_parent              (FmPath *path);
const char *fm_path_get_basename        (FmPath *path);
FmPathFlags fm_path_get_flags           (FmPath *path);
gboolean fm_path_has_prefix             (FmPath *path, FmPath *prefix);

#define fm_path_is_native(path)         (fm_path_get_flags(path) & FM_PATH_IS_NATIVE)
#define fm_path_is_virtual(path)        (fm_path_get_flags(path) & FM_PATH_IS_VIRTUAL)
#define fm_path_is_local(path)          (fm_path_get_flags(path) & FM_PATH_IS_LOCAL)
#define fm_path_is_trash_root(path)     (path == fm_path_get_trash())
#define fm_path_is_trash_file(path)     (fm_path_get_flags(path) & FM_PATH_IS_TRASH_FILE)
#define fm_path_is_xdg_menu(path)       (fm_path_get_flags(path) & FM_PATH_IS_XDG_MENU)

char *fm_path_to_str                    (FmPath *path);
char *fm_path_to_uri                    (FmPath *path);
GFile *fm_path_to_gfile                 (FmPath *path);

char *fm_path_display_name              (FmPath *path, gboolean human_readable);
char *fm_path_display_basename          (FmPath *path);

// For used in hash tables
guint fm_path_hash                      (FmPath *path);
gboolean fm_path_equal                  (FmPath *p1, FmPath *p2);

// used for completion in fm_path_entry
gboolean fm_path_equal_str              (FmPath *path, const gchar *str, int n);

// calculate how many elements are in this path.
int fm_path_depth                       (FmPath *path);

char* fm_path_get_trash_real_path(FmPath* path);

G_END_DECLS
#endif


