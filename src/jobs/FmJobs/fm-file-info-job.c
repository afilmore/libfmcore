/***********************************************************************************************************************
 * 
 *      fm-file-info-job.c
 *
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
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
#include "fm-file-info-job.h"

#include <menu-cache.h>

#include "fm-debug.h"

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>


G_DEFINE_TYPE (FmFileInfoJob, fm_file_info_job, FM_TYPE_JOB);


// Forward declarations...
static void fm_file_info_job_finalize (GObject *object);
static gboolean fm_file_info_job_run (FmJob *fmjob);
static gboolean fm_file_info_job_get_info_for_gfile (FmJob *job, FmFileInfo *file_info, GFile *gfile, GError **gerror);


/*********************************************************************
 * ...
 * 
 * 
 ********************************************************************/
FmJob *fm_file_info_job_new (FmPathList *files_to_query, FmFileInfoJobFlags flags)
{
	FmFileInfoJob *file_info_job = (FmFileInfoJob*) g_object_new (FM_TYPE_FILE_INFO_JOB, NULL);

    file_info_job->flags = flags;
	
    if (!files_to_query)
        return (FmJob*) file_info_job;
	
    FmFileInfoList *file_infos = file_info_job->file_info_list;
    
	GList *l;
    for (l = fm_list_peek_head_link (files_to_query); l; l = l->next)
    {
        FmPath *path = (FmPath*) l->data;
        
        FmFileInfo *file_info = fm_file_info_new_for_path (path);
        
        fm_list_push_tail_noref (file_infos, file_info);
    }
	
	return (FmJob*) file_info_job;
}

static void fm_file_info_job_class_init (FmFileInfoJobClass *klass)
{
	GObjectClass *g_object_class;
	FmJobClass *job_class;

	g_object_class = G_OBJECT_CLASS (klass);
	g_object_class->finalize = fm_file_info_job_finalize;

	job_class = FM_JOB_CLASS (klass);
	job_class->run = fm_file_info_job_run;
}

static void fm_file_info_job_init (FmFileInfoJob *self)
{
	self->file_info_list = fm_file_info_list_new ();
    
    //fm_job_init_cancellable (FM_JOB (self));
}

static void fm_file_info_job_finalize (GObject *object)
{
	FmFileInfoJob *self;

	g_return_if_fail (object != NULL);
	g_return_if_fail (IS_FM_FILE_INFO_JOB (object));

	self = FM_FILE_INFO_JOB (object);
    
    fm_list_unref (self->file_info_list);

	G_OBJECT_CLASS (fm_file_info_job_parent_class)->finalize (object);
}


/*********************************************************************
 * ...
 * 
 * 
 ********************************************************************/
// This can only be called before running the job.
void fm_file_info_job_add (FmFileInfoJob *file_info_job, FmPath *path)
{
	FmFileInfo *file_info = fm_file_info_new_for_path (path);
    
    fm_list_push_tail_noref (file_info_job->file_info_list, file_info);
}

void fm_file_info_job_add_gfile (FmFileInfoJob *file_info_job, GFile *gfile)
{
    FmPath *path = fm_path_new_for_gfile (gfile);
	
	FmFileInfo *file_info = fm_file_info_new_for_path (path);
    
    fm_path_unref (path);
    
    fm_list_push_tail_noref (file_info_job->file_info_list, file_info);
}


/*********************************************************************
 * ...
 * 
 * 
 ********************************************************************/
gboolean fm_file_info_job_run (FmJob *fmjob)
{
	FmFileInfoJob *file_info_job = (FmFileInfoJob*) fmjob;
    
    GError *gerror = NULL;

	GList *l;
	for (l = fm_list_peek_head_link (file_info_job->file_info_list); !fm_job_is_cancelled (fmjob) && l; )
	{
		FmFileInfo *file_info = (FmFileInfo*) l->data;
        
        GList *next = l->next;

        
        // TODO_axl: it's possible ot create a file info here for the input FmPath...
        
        file_info_job->current = file_info->path;

		
        
        // A native file, query file infos with posix...
        if (fm_path_is_native (file_info->path))
		{
			char *path_str = fm_path_to_str (file_info->path);
			
            if (!fm_file_info_set_for_native_file (file_info, path_str))
            {
                /** TODO_axl: error handling...
                FmErrorAction error_action = fm_job_emit_error (FM_JOB(file_info_job), gerror, FM_SEVERITY_MILD);
                
                g_error_free (gerror);
                gerror = NULL;
                
                if (error_action == FM_ERROR_ACTION_RETRY)
                    continue;
                **/
                
                DEBUG ("fm_file_info_set_for_native_file: error reading %s\n", path_str);
                
                next = l->next;
                
                fm_list_delete_link (file_info_job->file_info_list, l); // Also calls unref...
            }
			
            g_free (path_str);
            
            l = next;
            continue;
		}
		
        
        // This is a xdg menu
        else if (fm_path_is_xdg_menu (file_info->path))
        {
            MenuCache *mc;
            MenuCacheDir *dir;
            
            char *path_str = fm_path_to_str (file_info->path);
            char *menu_name = path_str + 5, ch;
            char *dir_name;
            
            while (*menu_name == '/')
                ++menu_name;
            
            dir_name = menu_name;
            
            while (*dir_name && *dir_name != '/')
                ++dir_name;
            
            ch = *dir_name;
            *dir_name = '\0';
            
            menu_name = g_strconcat (menu_name, ".menu", NULL);
            mc = menu_cache_lookup_sync (menu_name);
            g_free (menu_name);

            if (*dir_name && !(*dir_name == '/' && dir_name[1] == '\0'))
            {
                char *tmp = g_strconcat ("/",
                                         menu_cache_item_get_id (MENU_CACHE_ITEM(menu_cache_get_root_dir (mc))),
                                         dir_name, NULL);
                
                dir = menu_cache_get_dir_from_path (mc, tmp);
                
                g_free (tmp);
            }
            else
            {
                dir = menu_cache_get_root_dir (mc);
            }
            
            if (dir)
            {
                fm_file_info_set_for_menu_cache_item (file_info, (MenuCacheItem*) dir);
            }
            else
            {
                next = l->next;
                fm_list_delete_link (file_info_job->file_info_list, l); // Also calls unref...
            }
            
            g_free (path_str);
            menu_cache_unref (mc);
            
            l = l->next;
            continue;
        
        }
        else if (fm_path_is_virtual (file_info->path))
        {
            GFile *gfile = fm_path_to_gfile (file_info->path);
			
            if (!fm_file_info_query_info (file_info, gfile, fm_job_get_cancellable (FM_JOB (file_info_job)), &gerror))
            {
                FmErrorAction error_action = fm_job_emit_error (FM_JOB (file_info_job), gerror, FM_SEVERITY_MILD);
                
                g_error_free (gerror);
                gerror = NULL;
                
                if (error_action == FM_ERROR_ACTION_RETRY)
                    continue;

                next = l->next;
                
                fm_list_delete_link (file_info_job->file_info_list, l);   // Also calls unref...
            }
			
            g_object_unref (gfile);
        
            l = next;
            continue;
        
        }
        else
        {
            NO_DEBUG ("FmFileInfoJob: ERROR !!!!\n");
        }
        
        l = next;
	
    }
	
    return TRUE;
}


/*********************************************************************
 * ...
 * 
 * 
 ********************************************************************/
// This API should only be called in error handler
FmPath *fm_file_info_job_get_current (FmFileInfoJob *file_info_job)
{
    return file_info_job->current;
}

FmFileInfoList *fm_file_info_job_get_list (FmFileInfoJob *file_info_job)
{
    return file_info_job->file_info_list;
}





