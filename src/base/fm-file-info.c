/***********************************************************************************************************************
 * 
 *      fm-file-info.c
 *
 *      Copyright 2009 - 2010 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "fm-file-info.h"
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <grp.h>                // Query group name
#include <pwd.h>                // Query user name
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fm-utils.h"
#include <menu-cache.h>

static gboolean use_si_prefix = TRUE;
static FmMimeType *desktop_entry_type = NULL;
static FmMimeType *shortcut_type = NULL;
static FmMimeType *mountable_type = NULL;



static void fm_file_info_clear (FmFileInfo *file_info);



// intialize the file info system
void _fm_file_info_init ()
{
    fm_mime_type_init ();
    
    desktop_entry_type = fm_mime_type_get_for_type ("application/x-desktop");

    // fake mime-types for mountable and shortcuts
    shortcut_type = fm_mime_type_get_for_type ("inode/x-shortcut");
    shortcut_type->description = g_strdup (_ ("Shortcuts"));

    mountable_type = fm_mime_type_get_for_type ("inode/x-mountable");
    mountable_type->description = g_strdup (_ ("Mount Point"));
}

void _fm_file_info_finalize ()
{
    // FIXME: FmMimeTypes ar not freed, is it normal ?
}

FmFileInfo *fm_file_info_ref (FmFileInfo *file_info)
{
    g_atomic_int_inc (&file_info->n_ref);
    return file_info;
}

void fm_file_info_unref (FmFileInfo *file_info)
{
    // g_debug ("unref file info: %d", file_info->n_ref);
    if (g_atomic_int_dec_and_test (&file_info->n_ref))
    {
        fm_file_info_clear (file_info);
        g_slice_free (FmFileInfo, file_info);
    }
}


// TODOaxl: this function should be private... fm-file-info-job.c and fm-dir-list-job.c use it...
FmFileInfo *fm_file_info_new ()
{
    FmFileInfo *file_info = g_slice_new0 (FmFileInfo);
    file_info->n_ref = 1;
    return file_info;
}


FmFileInfo *fm_file_info_new_computer ()
{
    FmFileInfo *file_info = fm_file_info_new ();
    FmPath *path = fm_path_new_for_uri (FM_PATH_URI_COMPUTER);
    fm_file_info_set_path (file_info, path);
    
    // ensures that it's set as virtual...
    path->flags |= FM_PATH_IS_VIRTUAL;

    fm_path_unref (path);
    
    return file_info;
}

FmFileInfo *fm_file_info_new_trash_can ()
{
    FmFileInfo *file_info = fm_file_info_new ();
    FmPath *path = fm_path_new_for_uri (FM_PATH_URI_TRASH_CAN);
    fm_file_info_set_path (file_info, path);

    // fm_path_new_for_uri may also define FM_PATH_IS_LOCAL
    path->flags |= FM_PATH_IS_VIRTUAL;
    path->flags |= FM_PATH_IS_TRASH;
    path->flags |= FM_PATH_IS_TRASH_CAN;
    
    fm_path_unref (path);
    
    return file_info;
}

FmFileInfo *fm_file_info_new_user_special_dir (GUserDirectory directory)
{
    const gchar *path_name = g_get_user_special_dir (directory);
    GFile *file = g_file_new_for_path (path_name);
    if (!file)
        return NULL;
    
    GFileInfo *ginfo = g_file_query_info (file,
                                          "standard::*,unix::*,time::*,access::*,id::filesystem",
                                          G_FILE_QUERY_INFO_NONE, // G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS ???
                                          NULL,
                                          NULL);

    FmPath *path = fm_path_new_for_path (path_name);
    FmFileInfo *file_info = fm_file_info_new_from_gfileinfo (path, ginfo);
    
    
    
    // TODO: must set some path flags here.....
    
    
    
    g_object_unref (ginfo);
    g_object_unref (file);
    
    return file_info;
}

void fm_file_info_set_from_gfileinfo (FmFileInfo *file_info, GFileInfo *inf)
{
    const char *tmp;
    GIcon *gicon;
    GFileType type;

    g_return_if_fail (file_info->path);

    // if display name is the same as its name, just use it.
    tmp = g_file_info_get_display_name (inf);
    if (strcmp (tmp, file_info->path->name) == 0)
        file_info->disp_name = file_info->path->name;
    else
        file_info->disp_name = g_strdup (tmp);

    file_info->size = g_file_info_get_size (inf);

    tmp = g_file_info_get_content_type (inf);
    if (tmp)
        file_info->type = fm_mime_type_get_for_type (tmp);

    file_info->mode = g_file_info_get_attribute_uint32 (inf, G_FILE_ATTRIBUTE_UNIX_MODE);

    file_info->uid = g_file_info_get_attribute_uint32 (inf, G_FILE_ATTRIBUTE_UNIX_UID);
    file_info->gid = g_file_info_get_attribute_uint32 (inf, G_FILE_ATTRIBUTE_UNIX_GID);

    type = g_file_info_get_file_type (inf);
    if (0 == file_info->mode) // if UNIX file mode is not available, compose a fake one.
    {
        switch (type)
        {
        case G_FILE_TYPE_REGULAR:
            file_info->mode |= S_IFREG;
            break;
        case G_FILE_TYPE_DIRECTORY:
            file_info->mode |= S_IFDIR;
            break;
        case G_FILE_TYPE_SYMBOLIC_LINK:
            file_info->mode |= S_IFLNK;
            break;
        case G_FILE_TYPE_SHORTCUT:
            break;
        case G_FILE_TYPE_MOUNTABLE:
            break;
        }

        // if it's a special file but it doesn't have UNIX mode, compose a fake one.
        if (type == G_FILE_TYPE_SPECIAL && 0 == file_info->mode)
        {
            if (strcmp (tmp, "inode/chardevice")==0)
                file_info->mode |= S_IFCHR;
            else if (strcmp (tmp, "inode/blockdevice")==0)
                file_info->mode |= S_IFBLK;
            else if (strcmp (tmp, "inode/fifo")==0)
                file_info->mode |= S_IFIFO;
        #ifdef S_IFSOCK
            else if (strcmp (tmp, "inode/socket")==0)
                file_info->mode |= S_IFSOCK;
        #endif
        }
    }

    // set file icon according to mime-type
    if (!file_info->type || !file_info->type->icon)
    {
        gicon = g_file_info_get_icon (inf);
        file_info->icon = fm_icon_from_gicon (gicon);
        
        /* g_object_unref (gicon); this is not needed since
         * g_file_info_get_icon didn't increase ref_count.
         * the object returned by g_file_info_get_icon is
         * owned by GFileInfo. */
    }
    else
        file_info->icon = fm_icon_ref (file_info->type->icon);

    if (type == G_FILE_TYPE_MOUNTABLE || G_FILE_TYPE_SHORTCUT)
    {
        const char *uri = g_file_info_get_attribute_string (inf, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI);
        if (uri)
        {
            if (g_str_has_prefix (uri, "file:/"))
                file_info->target = g_filename_from_uri (uri, NULL, NULL);
            else
                file_info->target = g_strdup (uri);
        }

        if (!file_info->type)
        {
            // FIXME: is this appropriate?
            if (type == G_FILE_TYPE_SHORTCUT)
                file_info->type = fm_mime_type_ref (shortcut_type);
            else
                file_info->type = fm_mime_type_ref (mountable_type);
        }
        // FIXME: how about target of symlinks?
    }

    if (fm_path_is_native (file_info->path))
    {
        file_info->dev = g_file_info_get_attribute_uint32 (inf, G_FILE_ATTRIBUTE_UNIX_DEVICE);
    }
    else
    {
        tmp = g_file_info_get_attribute_string (inf, G_FILE_ATTRIBUTE_ID_FILESYSTEM);
        file_info->fs_id = g_intern_string (tmp);
    }

    file_info->mtime = g_file_info_get_attribute_uint64 (inf, G_FILE_ATTRIBUTE_TIME_MODIFIED);
    file_info->atime = g_file_info_get_attribute_uint64 (inf, G_FILE_ATTRIBUTE_TIME_ACCESS);
}

FmFileInfo *fm_file_info_new_from_gfileinfo (FmPath *path, GFileInfo *inf)
{
    FmFileInfo *file_info = fm_file_info_new ();
    file_info->path = fm_path_ref (path);
    fm_file_info_set_from_gfileinfo (file_info, inf);
    return file_info;
}

void fm_file_info_set_from_desktop_entry (FmFileInfo *file_info)
{
    // Special handling for desktop entries...
    char *fpath = fm_path_to_str (file_info->path);
    GKeyFile *kf = g_key_file_new ();
    
    FmIcon *icon = NULL;
    if (g_key_file_load_from_file (kf, fpath, 0, NULL))
    {
        char *icon_name = g_key_file_get_locale_string (kf, "Desktop Entry", "Icon", NULL, NULL);
        char *title = g_key_file_get_locale_string (kf, "Desktop Entry", "Name", NULL, NULL);
        
        if (icon_name)
        {
            // This is an icon name, not a full path to icon file.
            if (icon_name[0] != '/')
            {
                // remove file extension
                char *dot = strrchr (icon_name, '.');
                if (dot)
                {
                    ++dot;
                    if (strcmp (dot, "png") == 0
                        || strcmp (dot, "svg") == 0
                        || strcmp (dot, "xpm") == 0)
                    {
                        *(dot-1) = '\0';
                    }
                }
            }
            
            icon = fm_icon_from_name (icon_name);
            g_free (icon_name);
            
        }
        
        if (title)
            file_info->disp_name = title;
    }
    
    if (icon)
        file_info->icon = icon;
    else
        file_info->icon = file_info->type ? fm_icon_ref (file_info->type->icon) : NULL;
    
}

void fm_file_info_set_fm_icon (FmFileInfo *file_info, FmIcon *fm_icon)
{
    // TODOaxl: test and remove...
    //~ if (G_LIKELY(!fm_file_info_is_desktop_entry (file_info)))
    //~ {
        file_info->icon = fm_icon ? fm_icon_ref (fm_icon) : NULL;
        //return TRUE;
    //~ }
}

GIcon *fm_file_info_get_gicon (FmFileInfo *file_info)
{
    return file_info->icon ? file_info->icon->gicon : NULL;
}

void _fm_file_info_set_from_menu_cache_item (FmFileInfo *file_info, MenuCacheItem *item)
{
    const char *icon_name = menu_cache_item_get_icon (item);
    file_info->disp_name = g_strdup (menu_cache_item_get_name (item));
    if (icon_name)
    {
        char *tmp_name = NULL;
        if (icon_name[0] != '/') // this is a icon name, not a full path to icon file.
        {
            char *dot = strrchr (icon_name, '.');
            // remove file extension, this is a hack to fix non-standard desktop entry files
            if (G_UNLIKELY (dot))
            {
                ++dot;
                if (strcmp (dot, "png") == 0 ||
                   strcmp (dot, "svg") == 0 ||
                   strcmp (dot, "xpm") == 0)
                {
                    tmp_name = g_strndup (icon_name, dot - icon_name - 1);
                    icon_name = tmp_name;
                }
            }
        }
        
        file_info->icon = fm_icon_from_name (icon_name);
        
        if (G_UNLIKELY (tmp_name))
            g_free (tmp_name);
    }
    if (menu_cache_item_get_type (item) == MENU_CACHE_TYPE_DIR)
    {
        file_info->mode |= S_IFDIR;
    }
    else if (menu_cache_item_get_type (item) == MENU_CACHE_TYPE_APP)
    {
        file_info->mode |= S_IFREG;
        file_info->target = menu_cache_item_get_file_path (item);
    }
    file_info->type = fm_mime_type_ref (shortcut_type);
}

FmFileInfo *_fm_file_info_new_from_menu_cache_item (FmPath *path, MenuCacheItem *item)
{
    FmFileInfo *file_info = fm_file_info_new ();
    file_info->path = fm_path_ref (path);
    _fm_file_info_set_from_menu_cache_item (file_info, item);
    return file_info;
}

static void fm_file_info_clear (FmFileInfo *file_info)
{
    if (file_info->collate_key)
    {
        if (file_info->collate_key != file_info->disp_name)
            g_free (file_info->collate_key);
        file_info->collate_key = NULL;
    }

    if (file_info->path)
    {
        if (G_LIKELY (file_info->disp_name) && file_info->disp_name != file_info->path->name)
        {
            g_free (file_info->disp_name);
            file_info->disp_name = NULL;
        }

        fm_path_unref (file_info->path);
        file_info->path = NULL;
    }
    else
    {
        if (file_info->disp_name)
        {
            g_free (file_info->disp_name);
            file_info->disp_name = NULL;
        }
    }

    if (file_info->disp_size)
    {
        g_free (file_info->disp_size);
        file_info->disp_size = NULL;
    }

    g_free (file_info->target);

    if (file_info->type)
    {
        fm_mime_type_unref (file_info->type);
        file_info->type = NULL;
    }
    if (file_info->icon)
    {
        fm_icon_unref (file_info->icon);
        file_info->icon = NULL;
    }
}

void fm_file_info_copy (FmFileInfo *file_info, FmFileInfo *src)
{
    FmPath *tmp_path = fm_path_ref (src->path);
    FmMimeType *tmp_type = fm_mime_type_ref (src->type);
    FmIcon *tmp_icon = fm_icon_ref (src->icon);
    /* NOTE: we need to ref source first. Otherwise,
     * if path, mime_type, and icon are identical in src
     * and file_info, calling fm_file_info_clear () first on file_info
     * might unref that. */
    fm_file_info_clear (file_info);
    file_info->path = tmp_path;
    file_info->type = tmp_type;
    file_info->icon = tmp_icon;

    file_info->mode = src->mode;
    if (fm_path_is_native (file_info->path))
        file_info->dev = src->dev;
    else
        file_info->fs_id = src->fs_id;
    file_info->uid = src->uid;
    file_info->gid = src->gid;
    file_info->size = src->size;
    file_info->mtime = src->mtime;
    file_info->atime = src->atime;

    file_info->blksize = src->blksize;
    file_info->blocks = src->blocks;

    if (src->disp_name == src->path->name)
        file_info->disp_name = src->disp_name;
    else
        file_info->disp_name = g_strdup (src->disp_name);

    file_info->collate_key = g_strdup (src->collate_key);
    file_info->disp_size = g_strdup (src->disp_size);
    file_info->disp_mtime = g_strdup (src->disp_mtime);
    file_info->type = fm_mime_type_ref (src->type);
    file_info->icon = fm_icon_ref (src->icon);
}

FmPath *fm_file_info_get_path (FmFileInfo *file_info)
{
    return file_info->path;
}

const char *fm_file_info_get_name (FmFileInfo *file_info)
{
    return file_info->path->name;
}

// Get displayed name encoded in UTF-8
const char *fm_file_info_get_disp_name (FmFileInfo *file_info)
{
    if (G_UNLIKELY (!file_info->disp_name))
    {
        /* FIXME: this is not guaranteed to be UTF-8.
         * Encoding conversion is needed here. */
        return file_info->path->name;
    }
    return file_info->disp_name;
}

void fm_file_info_set_path (FmFileInfo *file_info, FmPath *path)
{
    if (file_info->path)
    {
        if (file_info->path->name == file_info->disp_name)
            file_info->disp_name = NULL;
        fm_path_unref (file_info->path);
    }

    if (path)
    {
        file_info->path = fm_path_ref (path);
        
        // FIXME: need to handle UTF-8 issue here
        file_info->disp_name = file_info->path->name;
    }
    else
        file_info->path = NULL;
}

void fm_file_info_set_disp_name (FmFileInfo *file_info, const char *name)
{
    if (file_info->disp_name && file_info->disp_name != file_info->path->name)
        g_free (file_info->disp_name);
    file_info->disp_name = g_strdup (name);
}

goffset fm_file_info_get_size (FmFileInfo *file_info)
{
    return file_info->size;
}

const char *fm_file_info_get_disp_size (FmFileInfo *file_info)
{
    if (G_UNLIKELY (!file_info->disp_size))
    {
        if (S_ISREG (file_info->mode))
        {
            char buf[ 64 ];
            fm_file_size_to_str (buf, file_info->size, use_si_prefix);
            file_info->disp_size = g_strdup (buf);
        }
    }
    return file_info->disp_size;
}

goffset fm_file_info_get_blocks (FmFileInfo *file_info)
{
    return file_info->blocks;
}

FmMimeType *fm_file_info_get_mime_type (FmFileInfo *file_info)
{
    return file_info->type ? fm_mime_type_ref (file_info->type) : NULL;
}

mode_t fm_file_info_get_mode (FmFileInfo *file_info)
{
    return file_info->mode;
}

gboolean fm_file_info_is_dir (FmFileInfo *file_info)
{
    return (S_ISDIR (file_info->mode) || (S_ISLNK (file_info->mode)
                                   && file_info->type
                                   && (strcmp (file_info->type->type, "inode/directory") == 0)));
}

gboolean fm_file_info_is_symlink (FmFileInfo *file_info)
{
    return S_ISLNK (file_info->mode) ? TRUE : FALSE;
}

gboolean fm_file_info_is_shortcut (FmFileInfo *file_info)
{
    return file_info->type == shortcut_type;
}

gboolean fm_file_info_is_mountable (FmFileInfo *file_info)
{
    return file_info->type == mountable_type;
}

gboolean fm_file_info_is_image (FmFileInfo *file_info)
{
    // FIXME: We had better use functions of xdg_mime to check this
    if (! strncmp ("image/", file_info->type->type, 6))
        return TRUE;
    return FALSE;
}

gboolean fm_file_info_is_text (FmFileInfo *file_info)
{
    if (g_content_type_is_a (file_info->type->type, "text/plain"))
        return TRUE;
    return FALSE;
}

gboolean fm_file_info_is_desktop_entry (FmFileInfo *file_info)
{
    return file_info->type == desktop_entry_type;
    // return g_strcmp0 (file_info->type->type, "application/x-desktop") == 0;
}

gboolean fm_file_info_is_unknown_type (FmFileInfo *file_info)
{
    if (!file_info->type)
        return TRUE;
    return g_content_type_is_unknown (file_info->type->type);
}

// full path of the file is required by this function
gboolean fm_file_info_is_executable_type (FmFileInfo *file_info)
{
    // FIXME: didn't check access rights.
//    return mime_type_is_executable_file (file_path, file_info->type->type);
    return g_content_type_can_be_executable (file_info->type->type);
}

gboolean fm_file_info_is_hidden (FmFileInfo *file_info)
{
    const char *name = file_info->path->name;
    /* files with . prefix or ~ suffix are regarded as hidden files.
     * dirs with . prefix are regarded as hidden dirs. */
    return (name[0] == '.' ||
       (!fm_file_info_is_dir (file_info) && g_str_has_suffix (name, "~")));
}

gboolean fm_file_info_can_thumbnail (FmFileInfo *file_info)
{
    // We cannot use S_ISREG here as this exclude all symlinks
    if (! (file_info->mode & S_IFREG) ||
        fm_file_info_is_desktop_entry (file_info) ||
        fm_file_info_is_unknown_type (file_info))
        return FALSE;
    return TRUE;
}

const char *fm_file_info_get_collate_key (FmFileInfo *file_info)
{
    if (G_UNLIKELY (!file_info->collate_key))
    {
        char *casefold = g_utf8_casefold (file_info->disp_name, -1);
        char *collate = g_utf8_collate_key_for_filename (casefold, -1);
        g_free (casefold);
        if (strcmp (collate, file_info->disp_name))
            file_info->collate_key = collate;
        else
        {
            file_info->collate_key = file_info->disp_name;
            g_free (collate);
        }
    }
    return file_info->collate_key;
}

const char *fm_file_info_get_target (FmFileInfo *file_info)
{
    return file_info->target;
}

const char *fm_file_info_get_desc (FmFileInfo *file_info)
{
    // FIXME: how to handle descriptions for virtual files without mime-tyoes?
    return file_info->type ? fm_mime_type_get_desc (file_info->type) : NULL;
}

const char *fm_file_info_get_disp_mtime (FmFileInfo *file_info)
{
    // FIXME: This can cause problems if the file really has mtime=0.
    //        We'd better hide mtime for virtual files only.
    if (file_info->mtime > 0)
    {
        if (! file_info->disp_mtime)
        {
            char buf[ 128 ];
            strftime (buf, sizeof (buf),
                      "%x %R",
                      localtime (&file_info->mtime));
            file_info->disp_mtime = g_strdup (buf);
        }
    }
    return file_info->disp_mtime;
}

time_t *fm_file_info_get_mtime (FmFileInfo *file_info)
{
    return &file_info->mtime;
}

time_t *fm_file_info_get_atime (FmFileInfo *file_info)
{
    return &file_info->atime;
}







// TODOaxl: remove...
/*
static FmListFuncs fm_list_funcs = {fm_file_info_ref, fm_file_info_unref};

FmFileInfoList* fm_file_info_list_new ()
{
    return fm_list_new (&fm_list_funcs);
}

gboolean fm_list_is_file_info_list (FmList* list)
{
    return list->funcs == &fm_list_funcs;
}

// return TRUE if all files in the list are of the same type
gboolean fm_file_info_list_is_same_type (FmFileInfoList* list)
{
    // FIXME: handle virtual files without mime-types
    if (! fm_list_is_empty (list))
    {
        GList* l = fm_list_peek_head_link (list);
        FmFileInfo* fi = (FmFileInfo*)l->data;
        l = l->next;
        for (;l;l=l->next)
        {
            FmFileInfo* fi2 = (FmFileInfo*)l->data;
            if (fi->type != fi2->type)
                return FALSE;
        }
    }
    return TRUE;
}

// return TRUE if all files in the list are on the same fs
gboolean fm_file_info_list_is_same_fs (FmFileInfoList* list)
{
    if (! fm_list_is_empty (list))
    {
        GList* l = fm_list_peek_head_link (list);
        FmFileInfo* fi = (FmFileInfo*)l->data;
        l = l->next;
        for (;l;l=l->next)
        {
            FmFileInfo* fi2 = (FmFileInfo*)l->data;
            gboolean is_native = fm_path_is_native (fi->path);
            if (is_native != fm_path_is_native (fi2->path))
                return FALSE;
            if (is_native)
            {
                if (fi->dev != fi2->dev)
                    return FALSE;
            }
            else
            {
                if (fi->fs_id != fi2->fs_id)
                    return FALSE;
            }
        }
    }
    return TRUE;
}

uint fm_file_info_list_get_flags (FmFileInfoList* list)
{
    uint flags = FM_PATH_NONE;
    
    if (fm_list_is_empty (list))
        return flags;
    
    GList* l;
    for (l = fm_list_peek_head_link (list); l; l=l->next)
    {
        FmFileInfo* fi = (FmFileInfo*)l->data;
        
        //printf ("fm_file_info_list_get_flags: path name = %s\n", fi->path->name);
        
        if (fm_path_is_trash_root (fi->path))
        {
            flags |= FM_PATH_IS_TRASH_CAN;
            flags |= FM_PATH_IS_VIRTUAL;
        }
        else if (fm_path_is_virtual (fi->path))
        {
            flags |= FM_PATH_IS_VIRTUAL;
        }
    }
    return flags;
}

*/

