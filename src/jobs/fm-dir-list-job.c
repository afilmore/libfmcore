/***********************************************************************************************************************
 * 
 *      fm-dir-list-job.c
 *
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
 *      Copyright 2012 Axel FILMORE <axel.filmore@gmail.com>
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
 *      Purpose: Directory Parsing Job.
 *      
 *      A FmDirListJob takes a directory path in input and parses that directory,
 *      the result is a FmFileInfoList containing a file info object for any file,
 *      folder, virtual item in that directory.
 * 
 *      The job also queries the file information of the parsed directory,
 *      it's possible to get it with fm_dir_dist_job_get_directory_info ().
 * 
 *      There is a "dir_only" option to parse only directories, this option is
 *      currently unused in the library, that's a bit confusing and I don't know
 *      if it's really usefull.
 * 
 * 
 * 
 **********************************************************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "fm-dir-list-job.h"

#include <menu-cache.h>

#include "fm-debug.h"

#include <glib/gi18n-lib.h>
#include <gio/gio.h>
#include <string.h>
#include <glib/gstdio.h>


extern MenuCache *global_menu_cache;

G_DEFINE_TYPE (FmDirListJob, fm_dir_list_job, FM_TYPE_JOB);


// Forward declarations...
static void     fm_dir_list_job_finalize        (GObject *object);

static gboolean fm_dir_list_job_run             (FmDirListJob *dir_list_job);
static gboolean fm_dir_list_job_run_posix       (FmDirListJob *dir_list_job);
static gboolean fm_dir_list_job_run_menu_cache  (gpointer user_data);
static gboolean fm_dir_list_job_run_gio         (FmDirListJob *dir_list_job);


/*********************************************************************
 *  Create a new DirList job...
 * 
 *  Note: The "dir_only" option is currently unused and untested...
 * 
 * 
 ********************************************************************/
FmJob *fm_dir_list_job_new (FmPath *path, gboolean dir_only)
{
	FmDirListJob *dir_list_job = (FmDirListJob*) g_object_new (FM_TYPE_DIR_LIST_JOB, NULL);
	
    dir_list_job->dir_only =    dir_only;
    
    dir_list_job->directory =   fm_path_ref (path);
	dir_list_job->files =       fm_file_info_list_new ();
	
    return (FmJob*) dir_list_job;
}

static void fm_dir_list_job_class_init (FmDirListJobClass *klass)
{
	GObjectClass *g_object_class;
	FmJobClass *job_class = FM_JOB_CLASS (klass);
	g_object_class = G_OBJECT_CLASS (klass);
	g_object_class->finalize = fm_dir_list_job_finalize;

	job_class->run = (void*) fm_dir_list_job_run;
}

static void fm_dir_list_job_init (FmDirListJob *self)
{
    //fm_job_init_cancellable (FM_JOB (self));
}

static void fm_dir_list_job_finalize (GObject *object)
{
	FmDirListJob *self;

	g_return_if_fail (object != NULL);
	g_return_if_fail (FM_IS_DIR_LIST_JOB (object));

	self = FM_DIR_LIST_JOB (object);

	if (self->directory)
		fm_path_unref (self->directory);

    if (self->dir_info)
        fm_file_info_unref (self->dir_info);

	if (self->files)
		fm_list_unref (self->files);

	if (G_OBJECT_CLASS (fm_dir_list_job_parent_class)->finalize)
		(*G_OBJECT_CLASS (fm_dir_list_job_parent_class)->finalize) (object);
}


/*********************************************************************
 *  Accessor funtions, get the result directory info and a
 *  file info list...
 * 
 * 
 ********************************************************************/
FmPath *fm_dir_dist_job_get_directory (FmDirListJob *dir_list_job)
{
	g_return_val_if_fail (dir_list_job != NULL, NULL);
	return dir_list_job->directory;
}

FmFileInfo *fm_dir_dist_job_get_directory_info (FmDirListJob *dir_list_job)
{
	g_return_val_if_fail (dir_list_job != NULL, NULL);
	return dir_list_job->dir_info;
}

FmFileInfoList *fm_dir_dist_job_get_files (FmDirListJob *dir_list_job)
{
	g_return_val_if_fail (dir_list_job != NULL, NULL);
	return dir_list_job->files;
}


/*********************************************************************
 *  Execute the job, it will create special items if needed and will
 *  use posix, libmenu-cache or GIO depending of the given input
 *  directory...
 * 
 ********************************************************************/
static gboolean fm_dir_list_job_run (FmDirListJob *dir_list_job)
{
	g_return_val_if_fail (dir_list_job != NULL, NULL);
    
    /**
     * It's possible to use GIO only instead of posix, then we see that GIO
     * is really slower.
     * 
     * Note: With this flag set, there's a problem with symlinks, the panel
     * launcher no longer works for example...
     * 
     **/
    
    gboolean use_gio = FALSE;
    
    FmPath *directory = dir_list_job->directory;
    
    /**
     * Parse the Desktop directory, we need to create special items on the desktop,
     * such as "My Computer", "Trash Can", "My Documents".
     * 
     **/
    if (fm_path_is_root (directory) && fm_path_is_desktop (directory))
    {
        // Computer...
        FmPath *path = fm_path_get_computer ();
        FmFileInfo *file_info = fm_file_info_new_for_path (path);
        fm_file_info_query (file_info, NULL, NULL);
        fm_list_push_tail_noref (dir_list_job->files, file_info);
        
        // Documents...
        path = fm_path_get_documents ();
        file_info = fm_file_info_new_for_path (path);
        fm_path_unref (path);
        fm_file_info_query (file_info, NULL, NULL);
        fm_list_push_tail_noref (dir_list_job->files, file_info);
        
        // Trash Can...
        path = fm_path_get_trash ();
        file_info = fm_file_info_new_for_path (path);
        fm_file_info_query (file_info, NULL, NULL);
        fm_list_push_tail_noref (dir_list_job->files, file_info);
    
    }
    
    /**
     * Add predefined user directories such as "Music", "Pictures" etc...
     * into "My Documents".
     * 
     **/
    else if (fm_path_is_root (directory) && fm_path_is_documents (directory))
    {
        // Download...
        FmPath *path = fm_path_get_download ();
        FmFileInfo *file_info = fm_file_info_new_for_path (path);
        fm_file_info_query (file_info, NULL, NULL);
        fm_list_push_tail_noref (dir_list_job->files, file_info);
        
        // Music...
        path = fm_path_get_music ();
        file_info = fm_file_info_new_for_path (path);
        fm_file_info_query (file_info, NULL, NULL);
        fm_list_push_tail_noref (dir_list_job->files, file_info);
        
        // Pictures...
        path = fm_path_get_pictures ();
        file_info = fm_file_info_new_for_path (path);
        fm_file_info_query (file_info, NULL, NULL);
        fm_list_push_tail_noref (dir_list_job->files, file_info);
        
        // Por... euh no Videos sorry... :-D
        path = fm_path_get_videos ();
        file_info = fm_file_info_new_for_path (path);
        fm_file_info_query (file_info, NULL, NULL);
        fm_list_push_tail_noref (dir_list_job->files, file_info);
    }
    
    
    // Parse a native directory, on the real file system...
    if (!use_gio && fm_path_is_native (dir_list_job->directory))
    {
        return fm_dir_list_job_run_posix (dir_list_job);
	
    
    // Parse a Menu Cache directory...
    }
    else if (fm_path_is_xdg_menu (dir_list_job->directory))
    {
        g_return_val_if_fail (global_menu_cache != NULL, FALSE);
        
        g_io_scheduler_job_send_to_mainloop (FM_JOB(dir_list_job),
                                             (GSourceFunc) fm_dir_list_job_run_menu_cache, dir_list_job, NULL);    
        return TRUE;
    }
    
    
    // Parse a virtual or remote directory...
    else
    {
        return fm_dir_list_job_run_gio (dir_list_job);
    }
}

static gboolean fm_dir_list_job_run_posix (FmDirListJob *dir_list_job)
{
    char *directory = fm_path_to_str (dir_list_job->directory);
    
    JOB_DEBUG ("--------------------------------------------------------------------------------\n");
    JOB_DEBUG ("JOB_DEBUG: fm_dir_list_job_run_posix:\t%s\n", directory);
    JOB_DEBUG ("\n");
    JOB_DEBUG ("--------------------------------------------------------------------------------\n");
    

	FmFileInfo *file_info = fm_file_info_new_for_path (dir_list_job->directory);
	
	GError *gerror = NULL;

    // TODO_axl: handle errors...
    if (fm_file_info_query (file_info, NULL, NULL))
    {
        dir_list_job->dir_info = file_info;
        if (!fm_file_info_is_dir (file_info))
        {
            gerror = g_error_new (G_IO_ERROR, G_IO_ERROR_NOT_DIRECTORY, _("The specified directory is not valid"));
            
            // ????
            //fm_file_info_unref (file_info);
            
            fm_job_emit_error (FM_JOB (dir_list_job), gerror, FM_SEVERITY_CRITICAL);
            g_error_free (gerror);
            
            return FALSE;
        }
    }
    else
    {
        gerror = g_error_new (G_IO_ERROR, G_IO_ERROR_NOT_DIRECTORY, _("The specified directory is not valid"));
        
        fm_file_info_unref (file_info);
        
        fm_job_emit_error (FM_JOB (dir_list_job), gerror, FM_SEVERITY_CRITICAL);
        
        g_error_free (gerror);
        return FALSE;
    }

    GDir *dir = g_dir_open (directory, 0, &gerror);
    if (dir)
    {
        char *name;
        
        GString *fpath = g_string_sized_new (4096);
        
        int dir_len = strlen (directory);
        
        g_string_append_len (fpath, directory, dir_len);
        if (fpath->str [dir_len-1] != '/')
        {
            g_string_append_c (fpath, '/');
            dir_len++;
        }
        
        // Parse the directory...
        while (!fm_job_is_cancelled (FM_JOB (dir_list_job)) &&  (name = (char*) g_dir_read_name (dir)))
        {
            g_string_truncate (fpath, dir_len);
            g_string_append (fpath, name);

            // The "dir_only" option is currently unused and untested...
            if (dir_list_job->dir_only)
            {
                struct stat st;
                
                // This results in an additional stat () call, which is inefficient...
                if (stat (fpath->str, &st) == -1 || !S_ISDIR (st.st_mode))
                    continue;
            }

            FmPath *child_path = fm_path_new_child (dir_list_job->directory, name);
            file_info = fm_file_info_new_for_path (child_path);
            fm_path_unref (child_path);
            

            _retry:
            
            // TODO_axl: handle errors...
            if (fm_file_info_query (file_info, NULL, NULL))
            {
                JOB_DEBUG ("JOB_DEBUG: fm_dir_list_job_run_gio: %s\n", fm_file_info_get_disp_name (file_info));
                fm_list_push_tail_noref (dir_list_job->files, file_info);
            }
            
            // Failed !
            else
            {
                FmErrorAction act = fm_job_emit_error (FM_JOB (dir_list_job), gerror, FM_SEVERITY_MILD);
                
                g_error_free (gerror);
                gerror = NULL;
                
                if (act == FM_ERROR_ACTION_RETRY)
                    goto _retry;

                fm_file_info_unref (file_info);
            }
        }
        
        g_string_free (fpath, TRUE);
        g_dir_close (dir);
    }
    else
    {
        fm_job_emit_error (FM_JOB (dir_list_job), gerror, FM_SEVERITY_CRITICAL);
        g_error_free (gerror);
    }
    
    g_free (directory);
    return TRUE;
}

static gboolean fm_dir_list_job_run_menu_cache (gpointer user_data)
{
    FmDirListJob *dir_list_job = (FmDirListJob*) user_data;
    char *directory = fm_path_to_str (dir_list_job->directory);
    
    JOB_DEBUG ("--------------------------------------------------------------------------------\n");
    JOB_DEBUG ("JOB_DEBUG: fm_dir_list_job_run_menu_cache: %s\n", directory);
    JOB_DEBUG ("\n");
    JOB_DEBUG ("--------------------------------------------------------------------------------\n");
    
    

    // Strip "menu://"...
    char *p = directory + 5;
    while (*p == '/')
        p++;
    
    char *cache_dir_path = p - 1;
    
    JOB_DEBUG ("\nJOB_DEBUG: fm_dir_list_job_run_menu_cache: cache_dir_path = %s\n", cache_dir_path);
    
    MenuCacheDir *menu_cache_dir = menu_cache_get_dir_from_path (global_menu_cache, cache_dir_path);
    g_free (directory);
    
    g_return_val_if_fail (menu_cache_dir != NULL, FALSE);
    
    const char *desktop_name = g_getenv ("XDG_CURRENT_DESKTOP");
    
    guint32 desktop_flag;
    if (desktop_name)
        desktop_flag = menu_cache_get_desktop_env_flag (global_menu_cache, desktop_name);
    else
        desktop_flag = (guint32) -1;

    GList* l;
    for (l = (GList*) menu_cache_dir_get_children (menu_cache_dir); l; l = l->next)
    {
        MenuCacheItem *item = MENU_CACHE_ITEM (l->data);
        
        // Also hide menu items which should be hidden in current DE.
        if (!item || menu_cache_item_get_type (item) == MENU_CACHE_TYPE_SEP)
            continue;
        
        if (menu_cache_item_get_type (item) == MENU_CACHE_TYPE_APP
            && !menu_cache_app_get_is_visible (MENU_CACHE_APP (item), desktop_flag))
            continue;

        // The "dir_only" option is currently unused and untested...
        if (G_UNLIKELY (dir_list_job->dir_only) && menu_cache_item_get_type (item) != MENU_CACHE_TYPE_DIR)
            continue;
        
        FmPath *item_path = fm_path_new_child (dir_list_job->directory, menu_cache_item_get_id (item));
        FmFileInfo *file_info = fm_file_info_new_for_path (item_path);
        
        JOB_DEBUG ("JOB_DEBUG: list_menu_items_new: query infos for %s\n", menu_cache_item_get_id (item));
        
        if (!fm_file_info_query (file_info, NULL, NULL))
        {
            JOB_DEBUG ("JOB_DEBUG: list_menu_items_new: ERROR\n");
            fm_path_unref (item_path);
            fm_file_info_unref (file_info);
            return FALSE;
        }
        
        fm_path_unref (item_path);
        
        fm_list_push_tail_noref (dir_list_job->files, file_info);
    }

    return TRUE;
}

static gboolean fm_dir_list_job_run_gio (FmDirListJob *dir_list_job)
{
    FmJob *fmjob = FM_JOB (dir_list_job);
	
    JOB_DEBUG ("--------------------------------------------------------------------------------\n");
    JOB_DEBUG ("JOB_DEBUG: fm_dir_list_job_run_gio\n");
    JOB_DEBUG ("\n");
    JOB_DEBUG ("--------------------------------------------------------------------------------\n");
    
    
    GFileEnumerator *enumerator;
    
    GFile *gfile = fm_path_to_gfile (dir_list_job->directory);

    GFileInfo *gfile_info;
	GError *gerror = NULL;
    
    _retry:
	
    gfile_info = g_file_query_info (gfile, gfile_info_query_attribs, 0, fm_job_get_cancellable (fmjob), &gerror);
    if (!gfile_info)
    {
        FmErrorAction act = fm_job_emit_error (fmjob, gerror, FM_SEVERITY_MODERATE);
        if (act == FM_ERROR_ACTION_RETRY)
        {
            g_error_free (gerror);
            gerror = NULL;
            goto _retry;
        }
        else
        {
            g_object_unref (gfile);
            g_error_free (gerror);
            return FALSE;
        }
    }

    GFileType gfile_type = g_file_info_get_file_type (gfile_info);
    
    if (gfile_type != G_FILE_TYPE_DIRECTORY && gfile_type != G_FILE_TYPE_MOUNTABLE)
    {
        JOB_DEBUG ("GFileType = %d\n", gfile_type);
        
        gerror = g_error_new (G_IO_ERROR, G_IO_ERROR_NOT_DIRECTORY, _("The specified directory is not valid"));
        fm_job_emit_error (fmjob, gerror, FM_SEVERITY_CRITICAL);
        g_error_free (gerror);
        g_object_unref (gfile);
        g_object_unref (gfile_info);
        return FALSE;
    }
    
    
    // Query the directory informations...
    dir_list_job->dir_info = fm_file_info_new_for_path (dir_list_job->directory);
    fm_file_info_query (dir_list_job->dir_info, NULL, NULL);
    g_object_unref (gfile_info);
    
    
    // The "dir_only" option is currently unused and untested...
    const char *query;
    if (G_UNLIKELY (dir_list_job->dir_only))
    {
        query = G_FILE_ATTRIBUTE_STANDARD_TYPE","G_FILE_ATTRIBUTE_STANDARD_NAME","
                G_FILE_ATTRIBUTE_STANDARD_IS_HIDDEN","G_FILE_ATTRIBUTE_STANDARD_IS_BACKUP","
                G_FILE_ATTRIBUTE_STANDARD_IS_SYMLINK","G_FILE_ATTRIBUTE_STANDARD_IS_VIRTUAL","
                G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME","G_FILE_ATTRIBUTE_STANDARD_ICON","
                G_FILE_ATTRIBUTE_STANDARD_SIZE","G_FILE_ATTRIBUTE_STANDARD_TARGET_URI","
                "unix::*,time::*,access::*,id::filesystem";
    }
    else
    {
        query = gfile_info_query_attribs;
    }
    
    // List children files...
    enumerator = g_file_enumerate_children (gfile, query, 0, fm_job_get_cancellable (fmjob), &gerror);
    
    g_object_unref (gfile);
    if (!enumerator)
    {
        fm_job_emit_error (fmjob, gerror, FM_SEVERITY_CRITICAL);
        g_error_free (gerror);
        return FALSE;
    }
        
    
    /**
     * Get an enumerator for the directory, parse files/folders in that directory and add a file info
     * object to the output list.
     * 
     **/
    while (!fm_job_is_cancelled (FM_JOB (dir_list_job)))
    {
        gfile_info = g_file_enumerator_next_file (enumerator, fm_job_get_cancellable (fmjob), &gerror);
        
        if (gfile_info)
        {
            // The "dir_only" option is currently unused and untested...
            if (G_UNLIKELY (dir_list_job->dir_only))
            {
                if (g_file_info_get_file_type (gfile_info) != G_FILE_TYPE_DIRECTORY)
                {
                    g_object_unref (gfile_info);
                    continue;
                }
            }

            // Insert a child item...
            JOB_DEBUG ("fm_dir_list_job_run_gio: file name = %s\n", g_file_info_get_name (gfile_info));
            
            FmPath *child = fm_path_new_child (dir_list_job->directory, g_file_info_get_name (gfile_info));
            FmFileInfo *file_info = fm_file_info_new_for_path (child);
            fm_file_info_query (file_info, NULL, NULL);
            
            fm_path_unref (child);
            
            fm_list_push_tail_noref (dir_list_job->files, file_info);
        }
        else
        {
            if (gerror)
            {
                JOB_DEBUG ("fm_dir_list_job_run_gio: Error (%s): %s\n",
                           g_file_info_get_name (gfile_info), gerror->message);
                
                FmErrorAction act = fm_job_emit_error (fmjob, gerror, FM_SEVERITY_MILD);
                g_error_free (gerror);
                
                // FM_ERROR_ACTION_RETRY is not supported...
                if (act == FM_ERROR_ACTION_ABORT)
                    fm_job_cancel (FM_JOB (dir_list_job));
            }
            break;
        }
        
        g_object_unref (gfile_info);
    }

    g_file_enumerator_close (enumerator, NULL, &gerror);
    g_object_unref (enumerator);

    return TRUE;
}



