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
 * TODO_axl: need to define some flags specific to FmFileInfo...
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
};
typedef enum _FmFileInfoFlag FmFileInfoFlag;
#endif


/*********************************************************************
 * Path Flags
 * 
 * 
 ********************************************************************/
enum _FmPathFlags
{
    FM_PATH_NONE            = 0,            // 0    No flags set, means we have an invalid path...
    FM_PATH_IS_NATIVE       = 1 << 0,       // 1    a native path like "/home"...
    FM_PATH_IS_VIRTUAL      = 1 << 1,       // 2    a virtual path, it doesn't exist on the real filesystem...
    FM_PATH_IS_LOCAL        = 1 << 2,       // 4    a file on the local filesystem...
    FM_PATH_IS_ROOT         = 1 << 3,       // 8    A predefines path root, trash can, menu:// etc...
    FM_PATH_IS_SPECIAL      = 1 << 4,       // 16
    FM_PATH_IS_COMPUTER     = 1 << 5,       // 16
    FM_PATH_IS_TRASH        = 1 << 6,       // 32   Trash Can file or root path (trash:/// or trash:///file)...
    FM_PATH_IS_DESKTOP      = 1 << 7,       // 
    FM_PATH_IS_DOCUMENTS    = 1 << 8,       // 
    FM_PATH_IS_XDG_MENU     = 1 << 9        //      This path is under menu:///
};
typedef enum _FmPathFlags FmPathFlags;


struct _FmPath
{
    gint    n_ref;
    FmPath  *parent;
    
    uint    flags;      // FmPathFlags flags : 32;
    char    name [1];
};


void _fm_path_init ();


// Reference counting...
FmPath *fm_path_ref                     (FmPath *path);
void    fm_path_unref                   (FmPath *path);


// Path creating methods...
FmPath *fm_path_new_for_str             (const char *path_str);
FmPath *fm_path_new_for_path            (const char *path_name);
FmPath *fm_path_new_for_uri             (const char *uri);
FmPath *fm_path_new_for_display_name    (const char *path_name);
FmPath *fm_path_new_for_commandline_arg (const char *arg);
FmPath *fm_path_new_for_gfile           (GFile *gf);

FmPath *fm_path_new_child               (FmPath *parent, const char *basename);
FmPath *fm_path_new_child_len           (FmPath *parent, const char *basename, int name_len);
FmPath *fm_path_new_relative            (FmPath *parent, const char *relative_path);


// Get Predefined Paths...
FmPath *fm_path_get_root                (); // /
FmPath *fm_path_get_home                (); // home directory
FmPath *fm_path_get_desktop             (); // $HOME/Desktop
FmPath *fm_path_get_documents             (); // $HOME/Desktop
FmPath *fm_path_get_trash               (); // trash:///
FmPath *fm_path_get_apps_menu           (); // menu://applications.menu/


// Path Flags Testing...
#define fm_path_is_native(path)         (fm_path_get_flags(path) & FM_PATH_IS_NATIVE)
#define fm_path_is_virtual(path)        (fm_path_get_flags(path) & FM_PATH_IS_VIRTUAL)
#define fm_path_is_local(path)          (fm_path_get_flags(path) & FM_PATH_IS_LOCAL)
#define fm_path_is_root(path)           (fm_path_get_flags(path) & FM_PATH_IS_ROOT)
#define fm_path_is_special(path)        (fm_path_get_flags(path) & FM_PATH_IS_SPECIAL)
#define fm_path_is_computer(path)       (fm_path_get_flags(path) & FM_PATH_IS_COMPUTER)
#define fm_path_is_desktop(path)        (fm_path_get_flags(path) & FM_PATH_IS_DESKTOP)
#define fm_path_is_documents(path)      (fm_path_get_flags(path) & FM_PATH_IS_DOCUMENTS)
#define fm_path_is_trash(path)          (path == fm_path_get_trash())
#define fm_path_is_xdg_menu(path)       (fm_path_get_flags(path) & FM_PATH_IS_XDG_MENU)


// Accessor Methods...
gboolean    fm_path_has_prefix          (FmPath *path, FmPath *prefix);
int         fm_path_depth               (FmPath *path);
FmPath     *fm_path_get_parent          (FmPath *path);
const char *fm_path_get_basename        (FmPath *path);
FmPathFlags fm_path_get_flags           (FmPath *path);
char       *fm_path_display_name        (FmPath *path, gboolean human_readable);
char       *fm_path_display_basename    (FmPath *path);


// Path Conversions...
char    *fm_path_to_str                 (FmPath *path);
char    *fm_path_to_uri                 (FmPath *path);
GFile   *fm_path_to_gfile               (FmPath *path);


// To Use In Hash Tables...
guint       fm_path_hash                (FmPath *path);
gboolean    fm_path_equal               (FmPath *p1, FmPath *p2);


// Used for completion in fm_path_entry...
gboolean fm_path_equal_str              (FmPath *path, const gchar *str, int n);


// New Method...
char    *fm_path_get_trash_real_path    (FmPath* path);

G_END_DECLS
#endif




