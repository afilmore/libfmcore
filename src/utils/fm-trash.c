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


static void fm_delete_files_internal (GtkWindow *parent, FmPathList *files)
{
    FmJob *job = fm_file_ops_job_new (FM_FILE_OP_DELETE, files);
    fm_file_ops_job_run_with_progress (parent, FM_FILE_OPS_JOB (job));
}

static void _fm_delete_files (GtkWindow *parent, FmPathList *files)
{
    if (!fm_config->confirm_delete
        || fm_yes_no (parent, NULL, _("Do you want to delete the selected files?"), TRUE))
    {
        fm_delete_files_internal (parent, files);
    }
}

static void _fm_trash_files (GtkWindow *parent, FmPathList *files)
{
    if (!fm_config->confirm_delete
        || fm_yes_no (parent, NULL, _("Do you want to move the selected files to trash can?"), TRUE))
    {
        FmJob *job = fm_file_ops_job_new (FM_FILE_OP_TRASH, files);
        fm_file_ops_job_run_with_progress (parent, FM_FILE_OPS_JOB (job));
    }
}


void fm_delete_files (GtkWindow *parent, FmPathList *files, FmDeleteFlags delete_flags)
{
    
    switch (delete_flags)
    {
        case FM_DELETE_FLAGS_TRASH:
        {
            if (!fm_config->confirm_delete
                || fm_yes_no (parent, NULL, _("Do you want to move the selected files to trash can?"), TRUE))
            {
                FmJob *job = fm_file_ops_job_new (FM_FILE_OP_TRASH, files);
                fm_file_ops_job_run_with_progress (parent, FM_FILE_OPS_JOB (job));
            }
        }
        break;
        
        case FM_DELETE_FLAGS_TRASH_OR_DELETE:
        {
            if (fm_list_is_empty (files))
                return;
            
            
            // TODO_axl: add a function to FmPath to do this...
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
                _fm_trash_files (parent, files);
            else
                _fm_delete_files (parent, files);
        }
        break;
    }
    
    /**if (delete_flags == FM_DELETE_FLAGS_TRASH)
    {
        if (!fm_config->confirm_delete || fm_yes_no (parent, NULL, _("Do you want to move the selected files to trash can?"), TRUE))
        {
            FmJob *job = fm_file_ops_job_new (FM_FILE_OP_TRASH, files);
            fm_file_ops_job_run_with_progress (parent, FM_FILE_OPS_JOB (job));
        }
    }
    else if (delete_flags == FM_DELETE_FLAGS_TRASH_OR_DELETE)
    {
        if (fm_list_is_empty (files))
            return;
        
        
        // TODO_axl: add a function to FmPath to do this...
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
            _fm_trash_files (parent, files);
        else
            _fm_delete_files (parent, files);
    }**/
}


/*void fm_trash_or_delete_files (GtkWindow *parent, FmPathList *files)
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
            _fm_delete_files (parent, files);
    }
}*/



void fm_untrash_files (GtkWindow *parent, FmPathList *files)
{
    FmJob *job = fm_file_ops_job_new (FM_FILE_OP_UNTRASH, files);
    fm_file_ops_job_run_with_progress (parent, FM_FILE_OPS_JOB (job));
}

void fm_empty_trash (GtkWindow *parent)
{
    if (!fm_yes_no (parent, NULL, _("Are you sure you want to empty the trash can?"), TRUE))
        return;
    
    FmPathList *paths = fm_path_list_new ();
    fm_list_push_tail (paths, fm_path_get_trash ());
    fm_delete_files_internal (parent, paths);
    fm_list_unref (paths);
}


