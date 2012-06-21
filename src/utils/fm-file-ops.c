/***********************************************************************************************************************
 * 
 *      fm-file-ops.c
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

#include "fm-file-ops.h"

#include <glib/gi18n-lib.h>

#include "fm-jobs.h"

#include "fm-user-input-dlg.h"
#include "fm-msgbox.h"


void fm_copy_files (GtkWindow *parent, FmPathList *path_list, FmPath *dest_dir, FmCopyJobMode copy_job_mode)
{
	g_return_if_fail (path_list != NULL);
	g_return_if_fail (dest_dir != NULL);
    
    FmList* dest_paths = fm_path_list_new ();
	
    GList* list = fm_list_peek_head_link (path_list);

    GList* src_path_it;
    for (src_path_it = list; src_path_it != NULL; src_path_it = src_path_it->next) {
        
        const gchar* _tmp5_  = fm_path_get_basename ((FmPath*) src_path_it->data);
        
        FmPath* dest_path = fm_path_new_child (dest_dir, _tmp5_);
        
        fm_list_push_tail (dest_paths, dest_path);
        
        fm_path_unref (dest_path);
    }
	
	
	FmGtkFileJobUI *ui = fm_gtk_file_job_ui_new (parent);
	
    FmJob* job = (FmJob*) fm_copy_job_new (copy_job_mode, path_list, dest_paths, ui);
	fm_job_run_async ((FmJob*) job);
	
    fm_list_unref (dest_paths);
    
    g_object_unref (ui);
	g_object_unref (job);
}

void fm_link_files (GtkWindow *parent, FmPath *file)
{
    GFile *gf = fm_path_to_gfile (file), *parent_gf, *dest;
    GError *err = NULL;
    
    //~ gchar *new_name = fm_get_user_input_rename (parent, _("Rename File"), _("Please enter a new name:"), file->name);
    //~ if (!new_name)
        //~ return;
    //~ 
    
    gchar *new_name = g_build_filename ("Link to ", file->name);
    if (!new_name)
        return;
    
    parent_gf = g_file_get_parent (gf);
    dest = g_file_get_child (G_FILE (parent_gf), new_name);
    g_object_unref (parent_gf);
    
    //~ if (!g_file_make_symbolic_link (gf,
                      //~ dest,
                      //~ G_FILE_COPY_ALL_METADATA
                      //~ | G_FILE_COPY_NO_FALLBACK_FOR_MOVE
                      //~ | G_FILE_COPY_NOFOLLOW_SYMLINKS,
                      //~ NULL, // make this cancellable later.
                      //~ NULL,
                      //~ NULL,
                      //~ &err))
    //~ {
        //~ fm_show_error (parent, NULL, err->message);
        //~ g_error_free (err);
    //~ }
    
    g_object_unref (dest);
    g_object_unref (gf);
    
    //
    
}

void fm_rename_file (GtkWindow *parent, FmPath *file)
{
    GFile *gf = fm_path_to_gfile (file), *parent_gf, *dest;
    GError *err = NULL;
    
    gchar *new_name = fm_get_user_input_rename (parent, _("Rename File"), _("Please enter a new name:"), file->name);
    if (!new_name)
        return;
    
    parent_gf = g_file_get_parent (gf);
    dest = g_file_get_child (G_FILE (parent_gf), new_name);
    g_object_unref (parent_gf);
    
    if (!g_file_move (gf,
                      dest,
                      G_FILE_COPY_ALL_METADATA
                      | G_FILE_COPY_NO_FALLBACK_FOR_MOVE
                      | G_FILE_COPY_NOFOLLOW_SYMLINKS,
                      NULL, // make this cancellable later.
                      NULL,
                      NULL,
                      &err))
    {
        fm_show_error (parent, NULL, err->message);
        g_error_free (err);
    }
    
    g_object_unref (dest);
    g_object_unref (gf);
}




