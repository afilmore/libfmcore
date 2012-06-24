/***********************************************************************************************************************
 * 
 *      fm-mount.c
 *
 *      Copyright 2009 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
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

#include "fm-mount.h"

#include <glib/gstdio.h>
#include <glib/gi18n-lib.h>
#include <string.h>

#include "fm-dir-list-job.h"
#include "fm-msgbox.h"

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


GMainLoop *loop;

static void on_automount (GObject *object, GAsyncResult *res, gpointer user_data);

void fm_mount_automount ()
{    
    
    FmPath *path = fm_path_new_for_uri ("computer:///");
    FmDirListJob *dir_list_job = (FmDirListJob *) fm_dir_list_job_new (path, FALSE);
    fm_job_run_sync ((FmJob*) dir_list_job);
    fm_path_unref (path);
    
    GList *l;
    for (l = fm_list_peek_head_link (dir_list_job->files); l; l = l->next)
    {
        loop = g_main_loop_new  (NULL, TRUE);
        
        FmPath *path = fm_file_info_get_path ((FmFileInfo*) l->data);
        GFile *gfile = fm_path_to_gfile (path);
        
        g_return_val_if_fail (gfile != NULL, 1);
        
        printf ("path2: %s\n", fm_path_to_str (path));
        
        g_file_mount_mountable (gfile, 0, NULL, NULL, on_automount, NULL);
        g_object_unref (gfile);
        
        if (g_main_loop_is_running (loop))
        {
            //GDK_THREADS_LEAVE ();
            g_main_loop_run (loop);
            //GDK_THREADS_ENTER ();
        }

        g_main_loop_unref (loop);
    }
    
    g_object_unref (dir_list_job);
    
    return;
}

static void on_automount (GObject *object, GAsyncResult *res, gpointer user_data)
{
    GError *error = NULL;

    GFile *target = g_file_mount_mountable_finish (G_FILE (object), res, &error);

    if (target == NULL)
    {
        g_print ("Error mounting location: %s\n", error->message);
    }
    else
    {
        //~ printf ("success: %s\n", g_file_get_display_name (target));
        g_object_unref (target);
    }
        
    g_main_loop_quit (loop);
}


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
                        data->err->message = g_strdup (_("Only system administrators have the permission to do this."));
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


