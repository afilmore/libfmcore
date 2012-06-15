/***********************************************************************************************************************
 * 
 *      fm-trash.c
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

#include "fm-trash.h"

#include <glib/gi18n-lib.h>

#include "fm-vala.h"
#include "fm-progress-dlg.h"
#include "fm-msgbox.h"


static void fm_delete_files_internal (GtkWindow *parent, FmPathList *path_list, gboolean confim_delete)
{
    if (!confim_delete
        || !fm_config->confirm_delete
        || fm_yes_no (parent, NULL, _("Do you want to delete the selected files?"), TRUE))
    {
        FmJob *job = fm_file_ops_job_new (FM_FILE_OP_DELETE, path_list);
        fm_file_ops_job_run_with_progress (parent, FM_FILE_OPS_JOB (job));
    }
}

static void fm_trash_files (GtkWindow *parent, FmPathList *path_list, gboolean confim_delete)
{
    if (!confim_delete
        || !fm_config->confirm_delete
        || fm_yes_no (parent, NULL, _("Do you want to move the selected files to trash can?"), TRUE))
    {
        FmJob *job = fm_file_ops_job_new (FM_FILE_OP_TRASH, path_list);
        fm_file_ops_job_run_with_progress (parent, FM_FILE_OPS_JOB (job));
    }
}


void fm_delete_files (GtkWindow *parent, FmPathList *path_list, FmDeleteFlags delete_flags, gboolean confim_delete)
{
    if (fm_list_is_empty (path_list))
        return;
    
    switch (delete_flags)
    {
        case FM_DELETE_FLAGS_TRASH:
        {
            fm_trash_files (parent, path_list, confim_delete);
        }
        break;
        
        case FM_DELETE_FLAGS_TRASH_OR_DELETE:
        {
            // Files that are in the trash can must be deleted and not trashed again...
            if (!fm_config->use_trash_can || fm_path_list_all_in_trash_can (path_list))
                fm_delete_files_internal (parent, path_list, confim_delete);
            else
                fm_trash_files (parent, path_list, confim_delete);
        }
        break;
    }
}

void fm_untrash_files (GtkWindow *parent, FmPathList *path_list)
{
    FmJob *job = fm_file_ops_job_new (FM_FILE_OP_UNTRASH, path_list);
    fm_file_ops_job_run_with_progress (parent, FM_FILE_OPS_JOB (job));
}

void fm_empty_trash (GtkWindow *parent)
{
    if (!fm_yes_no (parent, NULL, _("Are you sure you want to empty the trash can?"), TRUE))
        return;
    
    FmPathList *paths = fm_path_list_new ();
    fm_list_push_tail (paths, fm_path_get_trash ());
    fm_delete_files_internal (parent, paths, FALSE);
    fm_list_unref (paths);
}



