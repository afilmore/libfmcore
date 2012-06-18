/***********************************************************************************************************************
 * 
 *      fm-trash.c
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

#include "fm-trash.h"

#include <glib/gi18n-lib.h>

#include "fm-jobs.h"
#include "fm-vala.h"
#include "fm-msgbox.h"


static void fm_trash_delete_internal (GtkWindow *parent, FmPathList *path_list, gboolean confim_delete)
{
    if (!confim_delete
        || !fm_config->confirm_delete
        || fm_yes_no (parent, NULL, _("Do you want to delete the selected files?"), TRUE))
    {
        FmGtkFileJobUI *ui = fm_gtk_file_job_ui_new (parent);
        
        //FmJob* job = fm_delete_files2(path_list, (FmFileJobUI*) ui);
        
        FmJob *job = (FmJob*) fm_delete_job_new (path_list, ui);
        fm_job_run_async ((FmJob*) job);
        
        g_object_unref (ui);
        g_object_unref (job);
    }
}

static void fm_trash_internal (GtkWindow *parent, FmPathList *path_list, gboolean confim_delete)
{
    if (!confim_delete
        || !fm_config->confirm_delete
        || fm_yes_no (parent, NULL, _("Do you want to move the selected files to trash can?"), TRUE))
    {
		FmGtkFileJobUI *ui = fm_gtk_file_job_ui_new (parent);
		
        //FmJob* job = fm_trash_files2(path_list, (FmFileJobUI*) ui);

        FmJob *job = (FmJob*) fm_trash_job_new (path_list, ui);
        fm_job_run_async ((FmJob*) job);
        
        g_object_unref (ui);
		g_object_unref (job);
    }
}


void fm_trash_delete (GtkWindow *parent, FmPathList *path_list, FmDeleteFlags delete_flags, gboolean confim_delete)
{
    if (fm_list_is_empty (path_list))
        return;
    
    switch (delete_flags)
    {
        case FM_DELETE_FLAGS_TRASH:
        {
            fm_trash_internal (parent, path_list, confim_delete);
        }
        break;
        
        case FM_DELETE_FLAGS_TRASH_OR_DELETE:
        {
            // Files that are in the trash can must be deleted and not trashed again...
            if (!fm_config->use_trash_can || fm_path_list_all_in_trash_can (path_list))
                fm_trash_delete_internal (parent, path_list, confim_delete);
            else
                fm_trash_internal (parent, path_list, confim_delete);
        }
        break;
    }
}

void fm_trash_restore (GtkWindow *parent, FmPathList *path_list)
{
	FmGtkFileJobUI *ui = fm_gtk_file_job_ui_new (parent);
	
    //~ FmJob* job = fm_untrash_files2(path_list, (FmFileJobUI*) ui);

	FmJob *job = (FmJob*) fm_copy_job_new (FM_COPY_JOB_MODE_UNTRASH, path_list, NULL, ui);
	fm_job_run_async ((FmJob*) job);
    
    g_object_unref (ui);
	g_object_unref (job);
}

void fm_trash_empty (GtkWindow *parent)
{
    if (!fm_yes_no (parent, NULL, _("Are you sure you want to empty the trash can?"), TRUE))
        return;
    
    FmPathList *paths = fm_path_list_new ();
    fm_list_push_tail (paths, fm_path_get_trash ());
    fm_trash_delete_internal (parent, paths, FALSE);
    fm_list_unref (paths);
}



