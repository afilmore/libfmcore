/***********************************************************************************************************************
 * 
 *      fm-file-ops.c
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
//~ #include <gio/gdesktopappinfo.h>

#include "fm-file-ops.h"

#include "fm-dlg-utils.h"
#include "fm-msgbox.h"
//~ #include "fm-dlg-utils.h"
//~ #include "fm-file-ops-job.h"
#include "fm-progress-dlg.h"
//~ #include "fm-app-chooser-dlg.h"
//~ 
#include "fm-config.h"

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


