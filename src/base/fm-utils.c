/***********************************************************************************************************************
 * 
 *      fm-utils.c
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n-lib.h>
#include <gio/gdesktopappinfo.h>

#include "fm-utils.h"
#include "fm-dlg-utils.h"
#include "fm-file-ops-job.h"
#include "fm-progress-dlg.h"
#include "fm-app-chooser-dlg.h"

#include "fm-config.h"

#define BI_KiB  ((gdouble)1024.0)
#define BI_MiB  ((gdouble)1024.0 * 1024.0)
#define BI_GiB  ((gdouble)1024.0 * 1024.0 * 1024.0)
#define BI_TiB  ((gdouble)1024.0 * 1024.0 * 1024.0 * 1024.0)

#define SI_KB   ((gdouble)1000.0)
#define SI_MB   ((gdouble)1000.0 * 1000.0)
#define SI_GB   ((gdouble)1000.0 * 1000.0 * 1000.0)
#define SI_TB   ((gdouble)1000.0 * 1000.0 * 1000.0 * 1000.0)


/*********************************************************************
 * Free Item List Functions...
 * _g_list_foreach_l is a variant of g_list_foreach which passes
 * GList *element as the first parameter to the given function
 * instead of the element data.
 * 
 * 
 ********************************************************************/
inline void _g_list_foreach_l (GList *list, GFunc func, gpointer user_data)
{
    while (list)
    {
        GList *next = list->next;
        (*func) (list, user_data);
        list = next;
    }
}


char *fm_file_size_to_str ( char *buf, goffset size, gboolean si_prefix )
{
    const char  *unit;
    gdouble val;

    if ( si_prefix ) // 1000 based SI units
    {
        if (size <  (goffset)SI_KB)
        {
            sprintf ( buf, ngettext ("%u byte", "%u bytes",  (guint)size),  (guint)size);
            return buf;
        }
        val =  (gdouble)size;
        if (val < SI_MB)
        {
            val /= SI_KB;
            unit = _ ("KB");
        }
        else if (val < SI_GB)
        {
            val /= SI_MB;
            unit = _ ("MB");
        }
        else if (val < SI_TB)
        {
            val /= SI_GB;
            unit = _ ("GB");
        }
        else
        {
            val /= SI_TB;
            unit = _ ("TB");
        }
    }
    else // 1024-based binary prefix
    {
        if (size <  (goffset)BI_KiB)
        {
            sprintf ( buf, ngettext ("%u byte", "%u bytes",  (guint)size),  (guint)size);
            return buf;
        }
        val =  (gdouble)size;
        if (val < BI_MiB)
        {
            val /= BI_KiB;
            unit = _ ("KiB");
        }
        else if (val < BI_GiB)
        {
            val /= BI_MiB;
            unit = _ ("MiB");
        }
        else if (val < BI_TiB)
        {
            val /= BI_GiB;
            unit = _ ("GiB");
        }
        else
        {
            val /= BI_TiB;
            unit = _ ("TiB");
        }
    }
    sprintf ( buf, "%.1f %s", val, unit );
    return buf;
}

gboolean fm_key_file_get_int (GKeyFile *kf, const char *grp, const char *key, int *val)
{
    char *str = g_key_file_get_value (kf, grp, key, NULL);
    if (G_LIKELY (str))
    {
        *val = atoi (str);
        g_free (str);
    }
    return str != NULL;
}

gboolean fm_key_file_get_bool (GKeyFile *kf, const char *grp, const char *key, gboolean *val)
{
    char *str = g_key_file_get_value (kf, grp, key, NULL);
    if (G_LIKELY (str))
    {
        *val =  (str[0] == '1' || str[0] == 't');
        g_free (str);
    }
    return str != NULL;
}

char *fm_canonicalize_filename (const char *filename, const char *cwd)
{
    char *_cwd = NULL;
    int len = strlen (filename);
    int i = 0;
    char *ret = g_malloc (len + 1), *p = ret;
    if (!cwd)
        cwd = _cwd = g_get_current_dir ();
    for (; i < len; )
    {
        if (filename[i] == '.')
        {
            if (filename[i+1] == '.' &&  (filename[i+2] == '/' || filename[i+2] == '\0') ) // ..
            {
                if (i == 0) // .. is first element
                {
                    int cwd_len;
                    const char *sep;
                    if (!cwd)
                        cwd = _cwd = g_get_current_dir ();

                    sep = strrchr (cwd, '/');
                    if (sep && sep != cwd)
                        cwd_len =  (sep - cwd);
                    else
                        cwd_len = strlen (cwd);
                    ret = g_realloc (ret, len + cwd_len + 1 - 1);
                    memcpy (ret, cwd, cwd_len);
                    p = ret + cwd_len;
                }
                else // other .. in the path
                {
                    --p;
                    if (p > ret && *p == '/') // strip trailing / if it's not root
                        --p;
                    while (p > ret && *p != '/') // strip basename
                        --p;
                    if (*p != '/' || p == ret) // strip trailing / if it's not root
                        ++p;
                }
                i += 2;
                continue;
            }
            else if (filename[i+1] == '/' || filename[i+1] == '\0' ) // .
            {
                if (i == 0) // first element
                {
                    int cwd_len;
                    cwd_len = strlen (cwd);
                    ret = g_realloc (ret, len + cwd_len + 1);
                    memcpy (ret, cwd, cwd_len + 1);
                    p = ret + cwd_len;
                }
                ++i;
                continue;
            }
        }
        for (; i < len; ++p)
        {
            // prevent duplicated /
            if (filename[i] == '/' &&  (p > ret &&  *(p-1) == '/'))
            {
                ++i;
                break;
            }
            *p = filename[i];
            ++i;
            if (*p == '/')
            {
                ++p;
                break;
            }
        }
    }
    if ( (p-1) > ret &&  *(p-1) == '/') // strip trailing /
        --p;
    *p = 0;
    if (_cwd)
        g_free (_cwd);
    return ret;
}

char *fm_str_replace (char *str, char *old, char *new)
{
    int i;
    int len = strlen (str);
    char *found;
    GString *buf = g_string_sized_new (len);
    while (found = strstr (str, old))
    {
        g_string_append_len (buf, str,  (found - str));
        g_string_append (buf, new);
        str = found + 1;
    }
    for (; *str; ++str)
        g_string_append_c (buf, *str);
    return g_string_free (buf, FALSE);
}

typedef enum
{
    MOUNT_VOLUME,
    MOUNT_GFILE,
    UMOUNT_MOUNT,
    EJECT_MOUNT,
    EJECT_VOLUME
}MountAction;

struct MountData
{
    GMainLoop *loop;
    MountAction action;
    GError *err;
    gboolean ret;
};

static void on_mount_action_finished (GObject *src, GAsyncResult *res, gpointer user_data)
{
    struct MountData *data = user_data;
g_debug ("on_mount_action_finished");
    switch (data->action)
    {
    case MOUNT_VOLUME:
        data->ret = g_volume_mount_finish (G_VOLUME (src), res, &data->err);
        break;
    case MOUNT_GFILE:
        data->ret = g_file_mount_enclosing_volume_finish (G_FILE (src), res, &data->err);
        break;
    case UMOUNT_MOUNT:
        #if GLIB_CHECK_VERSION (2, 22, 0)
        data->ret = g_mount_unmount_with_operation_finish (G_MOUNT (src), res, &data->err);
        #else
        data->ret = g_mount_unmount_finish (G_MOUNT (src), res, &data->err);
        #endif
        break;
    case EJECT_MOUNT:
        #if GLIB_CHECK_VERSION (2, 22, 0)
        data->ret = g_mount_eject_with_operation_finish (G_MOUNT (src), res, &data->err);
        #else
        data->ret = g_mount_eject_finish (G_MOUNT (src), res, &data->err);
        #endif
        break;
    case EJECT_VOLUME:
        #if GLIB_CHECK_VERSION (2, 22, 0)
        data->ret = g_volume_eject_with_operation_finish (G_VOLUME (src), res, &data->err);
        #else
        data->ret = g_volume_eject_finish (G_VOLUME (src), res, &data->err);
        #endif
        break;
    }
    g_main_loop_quit (data->loop);
}

static void prepare_unmount (GMount *mount)
{
    // ensure that CWD is not on the mounted filesystem.
    char *cwd_str = g_get_current_dir ();
    GFile *cwd = g_file_new_for_path (cwd_str);
    GFile *root = g_mount_get_root (mount);
    g_free (cwd_str);
    
    /*FIXME_pcm: This cannot cover 100% cases since symlinks are not checked.
      *There may be other cases that cwd is actually under mount root
      *but checking prefix is not enough. We already did our best, though. */
    
    if (g_file_has_prefix (cwd, root))
        g_chdir ("/");
    g_object_unref (cwd);
    g_object_unref (root);
}

static gboolean fm_do_mount (GtkWindow *parent, GObject *obj, MountAction action, gboolean interactive)
{
    gboolean ret;
    struct MountData *data = g_new0 (struct MountData, 1);
    GMountOperation *op = interactive ? gtk_mount_operation_new (parent) : NULL;
    GCancellable *cancellable = g_cancellable_new ();

    data->loop = g_main_loop_new  (NULL, TRUE);
    data->action = action;

    switch (data->action)
    {
    case MOUNT_VOLUME:
        g_volume_mount (G_VOLUME (obj), 0, op, cancellable, on_mount_action_finished, data);
        break;
    case MOUNT_GFILE:
        g_file_mount_enclosing_volume (G_FILE (obj), 0, op, cancellable, on_mount_action_finished, data);
        break;
    case UMOUNT_MOUNT:
        prepare_unmount (G_MOUNT (obj));
        #if GLIB_CHECK_VERSION (2, 22, 0)
        g_mount_unmount_with_operation (G_MOUNT (obj), G_MOUNT_UNMOUNT_NONE, op, cancellable, on_mount_action_finished, data);
        #else
        g_mount_unmount (G_MOUNT (obj), G_MOUNT_UNMOUNT_NONE, cancellable, on_mount_action_finished, data);
        #endif
        break;
    case EJECT_MOUNT:
        prepare_unmount (G_MOUNT (obj));
        #if GLIB_CHECK_VERSION (2, 22, 0)
        g_mount_eject_with_operation (G_MOUNT (obj), G_MOUNT_UNMOUNT_NONE, op, cancellable, on_mount_action_finished, data);
        #else
        g_mount_eject (G_MOUNT (obj), G_MOUNT_UNMOUNT_NONE, cancellable, on_mount_action_finished, data);
        #endif
        break;
    case EJECT_VOLUME:
        {
            GMount *mnt = g_volume_get_mount (G_VOLUME (obj));
            prepare_unmount (mnt);
            g_object_unref (mnt);
            #if GLIB_CHECK_VERSION (2, 22, 0)
            g_volume_eject_with_operation (G_VOLUME (obj), G_MOUNT_UNMOUNT_NONE, op, cancellable, on_mount_action_finished, data);
            #else
            g_volume_eject (G_VOLUME (obj), G_MOUNT_UNMOUNT_NONE, cancellable, on_mount_action_finished, data);
            #endif
        }
        break;
    }

    if  (g_main_loop_is_running (data->loop))
    {
        GDK_THREADS_LEAVE ();
        
        g_main_loop_run (data->loop);
        
        GDK_THREADS_ENTER ();
    }

    g_main_loop_unref (data->loop);

    ret = data->ret;
    if (data->err)
    {
        if (interactive)
        {
            if (data->err->domain == G_IO_ERROR)
            {
                if (data->err->code == G_IO_ERROR_FAILED)
                {
                    // Generate a more human-readable error message instead of using a gvfs one.

                    /*The original error message is something like:
                      *Error unmounting: umount exited with exit code 1:
                      *helper failed with: umount: only root can unmount
                      *UUID=18cbf00c-e65f-445a-bccc-11964bdea05d from /media/sda4 */

                    /*Why they pass this back to us?
                      *This is not human-readable for the users at all. */

                    if (strstr (data->err->message, "only root can "))
                    {
                        g_debug ("%s", data->err->message);
                        g_free (data->err->message);
                        data->err->message = g_strdup (_ ("Only system administrators have the permission to do this."));
                    }
                }
                else if (data->err->code == G_IO_ERROR_FAILED_HANDLED)
                    interactive = FALSE;
            }
            if (interactive)
                fm_show_error (parent, NULL, data->err->message);
        }
        g_error_free (data->err);
    }

    g_free (data);
    g_object_unref (cancellable);
    if (op)
        g_object_unref (op);
        
    return ret;
}

gboolean fm_mount_path (GtkWindow *parent, FmPath *path, gboolean interactive)
{
    GFile *gf = fm_path_to_gfile (path);
    gboolean ret = fm_do_mount (parent, G_OBJECT (gf), MOUNT_GFILE, interactive);
    g_object_unref (gf);
    return ret;
}

gboolean fm_mount_volume (GtkWindow *parent, GVolume *vol, gboolean interactive)
{
    return fm_do_mount (parent, G_OBJECT (vol), MOUNT_VOLUME, interactive);
}

gboolean fm_unmount_mount (GtkWindow *parent, GMount *mount, gboolean interactive)
{
    return fm_do_mount (parent, G_OBJECT (mount), UMOUNT_MOUNT, interactive);
}

gboolean fm_unmount_volume (GtkWindow *parent, GVolume *vol, gboolean interactive)
{
    GMount *mount = g_volume_get_mount (vol);
    gboolean ret;
    if (!mount)
        return FALSE;
    ret = fm_do_mount (parent, G_OBJECT (vol), UMOUNT_MOUNT, interactive);
    g_object_unref (mount);
    return ret;
}

gboolean fm_eject_mount (GtkWindow *parent, GMount *mount, gboolean interactive)
{
    return fm_do_mount (parent, G_OBJECT (mount), EJECT_MOUNT, interactive);
}

gboolean fm_eject_volume (GtkWindow *parent, GVolume *vol, gboolean interactive)
{
    return fm_do_mount (parent, G_OBJECT (vol), EJECT_VOLUME, interactive);
}


// File operations
/*FIXME_pcm: only show the progress dialog if the job isn't finished
  *in 1 sec. */

void fm_copy_files (GtkWindow *parent, FmPathList *files, FmPath *dest_dir)
{
    FmJob *job = fm_file_ops_job_new (FM_FILE_OP_COPY, files);
    
    fm_file_ops_job_set_dest (FM_FILE_OPS_JOB (job), dest_dir);
    fm_file_ops_job_run_with_progress (parent, FM_FILE_OPS_JOB (job));
}

void fm_move_files (GtkWindow *parent, FmPathList *files, FmPath *dest_dir)
{
    FmJob *job = fm_file_ops_job_new (FM_FILE_OP_MOVE, files);
    
    fm_file_ops_job_set_dest (FM_FILE_OPS_JOB (job), dest_dir);
    fm_file_ops_job_run_with_progress (parent, FM_FILE_OPS_JOB (job));
}

void fm_trash_files (GtkWindow *parent, FmPathList *files)
{
    if (!fm_config->confirm_delete || fm_yes_no (parent, NULL, _ ("Do you want to move the selected files to trash can?"), TRUE))
    {
        FmJob *job = fm_file_ops_job_new (FM_FILE_OP_TRASH, files);
        fm_file_ops_job_run_with_progress (parent, FM_FILE_OPS_JOB (job));
    }
}

void fm_untrash_files (GtkWindow *parent, FmPathList *files)
{
    FmJob *job = fm_file_ops_job_new (FM_FILE_OP_UNTRASH, files);
    fm_file_ops_job_run_with_progress (parent, FM_FILE_OPS_JOB (job));
}

static void fm_delete_files_internal (GtkWindow *parent, FmPathList *files)
{
    FmJob *job = fm_file_ops_job_new (FM_FILE_OP_DELETE, files);
    fm_file_ops_job_run_with_progress (parent, FM_FILE_OPS_JOB (job));
}

void fm_delete_files (GtkWindow *parent, FmPathList *files)
{
    if (!fm_config->confirm_delete || fm_yes_no (parent, NULL, _ ("Do you want to delete the selected files?"), TRUE))
        fm_delete_files_internal (parent, files);
}

void fm_trash_or_delete_files (GtkWindow *parent, FmPathList *files)
{
    if (!fm_list_is_empty (files))
    {
        gboolean all_in_trash = TRUE;
        if (fm_config->use_trash_can)
        {
            GList *l = fm_list_peek_head_link (files);
            for (;l;l=l->next)
            {
                FmPath *path = FM_PATH (l->data);
                if (!fm_path_is_trash (path))
                    all_in_trash = FALSE;
            }
        }

        // files already in trash:/// should only be deleted and cannot be trashed again.
        if (fm_config->use_trash_can && !all_in_trash)
            fm_trash_files (parent, files);
        else
            fm_delete_files (parent, files);
    }
}

void fm_move_or_copy_files_to (GtkWindow *parent, FmPathList *files, gboolean is_move)
{
    FmPath *dest = fm_select_folder (parent, NULL);
    if (dest)
    {
        if (is_move)
            fm_move_files (parent, files, dest);
        else
            fm_copy_files (parent, files, dest);
        fm_path_unref (dest);
    }
}


void fm_rename_file (GtkWindow *parent, FmPath *file)
{
    GFile *gf = fm_path_to_gfile (file), *parent_gf, *dest;
    GError *err = NULL;
    
    gchar *new_name = fm_get_user_input_rename (parent, _ ("Rename File"), _ ("Please enter a new name:"), file->name);
    if (!new_name)
        return;
    
    parent_gf = g_file_get_parent (gf);
    dest = g_file_get_child (G_FILE (parent_gf), new_name);
    g_object_unref (parent_gf);
    
    if (!g_file_move (gf, dest,
                G_FILE_COPY_ALL_METADATA|
                G_FILE_COPY_NO_FALLBACK_FOR_MOVE|
                G_FILE_COPY_NOFOLLOW_SYMLINKS,
                NULL, // make this cancellable later.
                NULL, NULL, &err))
    {
        fm_show_error (parent, NULL, err->message);
        g_error_free (err);
    }
    
    g_object_unref (dest);
    g_object_unref (gf);
}

void fm_empty_trash (GtkWindow *parent)
{
    if (fm_yes_no (parent, NULL, _ ("Are you sure you want to empty the trash can?"), TRUE))
    {
        FmPathList *paths = fm_path_list_new ();
        fm_list_push_tail (paths, fm_path_get_trash ());
        fm_delete_files_internal (parent, paths);
        fm_list_unref (paths);
    }
}


