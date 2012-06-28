/***********************************************************************************************************************
 * 
 * 
 * 
 * 
 * 
 **********************************************************************************************************************/
#include <fm.h>

#include "fm-debug.h"

GMainLoop *loop;


static void mount_mountable_done_cb (GObject *object, GAsyncResult *res, gpointer user_data)
{
    GError *error = NULL;

    GFile *target = g_file_mount_mountable_finish (G_FILE (object), res, &error);

    if (target == NULL)
    {
        g_print ("Error mounting location: %s\n", error->message);
    }
    else
    {
        // NO_DEBUG ("Succesfull !!!!\n");
        
        g_object_unref (target);
        
        g_main_loop_quit (loop);
    }
}

int main (int argc, char *argv[])
{
    
    gtk_init (&argc, &argv);
    fm_init (NULL);
    
    FmPath *path = fm_path_new_for_uri ("computer:///");
    FmDirListJob *dir_list_job = (FmDirListJob *) fm_dir_list_job_new (path, FALSE);
    fm_job_run_sync ((FmJob*) dir_list_job);
    fm_path_unref (path);
    
    GList *l;
    for (l = fm_list_peek_head_link (dir_list_job->files); l; l = l->next)
    {
        loop = g_main_loop_new  (NULL, TRUE);
        
        FmFileInfo *file_info = (FmFileInfo*) l->data;
        
        // NO_DEBUG ("path2 : %s\n", fm_path_to_str (fm_file_info_get_path (file_info)));
        
        char *target = fm_file_info_get_target (file_info);
        
        //if (target)
            // NO_DEBUG ("target : %s\n", target);
        
        GFile *gfile = fm_path_to_gfile (fm_file_info_get_path (file_info));
        
        g_return_val_if_fail (gfile != NULL, 1);
        
        g_file_mount_mountable (gfile, 0, NULL, NULL, mount_mountable_done_cb, NULL);
        
        
        g_object_unref (gfile);
        //g_object_unref (gfile_info);
        
        if  (g_main_loop_is_running (loop))
        {
            GDK_THREADS_LEAVE ();
            
            g_main_loop_run (loop);
            
            GDK_THREADS_ENTER ();
        }

        g_main_loop_unref (loop);
    }
    
    g_object_unref (dir_list_job);
    

    /**
        
        //GFile *gfile = fm_path_to_gfile (path);
        
        //fm_mount_path (NULL, path, TRUE);
        
        //~ g_file_mount_enclosing_volume (gfile, 0, NULL, NULL, NULL, NULL);
        //~ 
        //~ GFileInfo *gfile_info = g_file_query_info (gfile, G_FILE_ATTRIBUTE_MOUNTABLE_UNIX_DEVICE_FILE, G_FILE_QUERY_INFO_NONE, NULL, NULL);
        //~ if (gfile_info != NULL) {
                //~ gchar *device_file = g_file_info_get_attribute_as_string (gfile_info, G_FILE_ATTRIBUTE_MOUNTABLE_UNIX_DEVICE_FILE);
                //~ if (device_file)
                    //~ NO_DEBUG ("Device file = %s\n", device_file);
                //~ g_object_unref (gfile_info);
        //~ }
        //~ g_object_unref (gfile);
//~ 
//~ 
        //~ 

     **/



    /** Trash can job...
    FmDirListJob *trash_job = (FmDirListJob *) fm_dir_list_job_new (fm_path_get_trash (), FALSE);
    fm_job_run_sync ((FmJob*) trash_job);
    
    GList *l;
    
    for (l = fm_list_peek_head_link (trash_job->files); l; l = l->next)
    {
        FmFileInfo *inf = (FmFileInfo*) l->data;
        
        GFile *gfile = fm_path_to_gfile (inf->path);
        
        g_return_val_if_fail (gfile != NULL, 1);
        
        GFileInfo *gfile_info = g_file_query_info (gfile, "trash::*", 0, NULL, NULL);
        
        g_return_val_if_fail (gfile_info != NULL, 1);
        
        const char *orig_path = g_file_info_get_attribute_byte_string (gfile_info, G_FILE_ATTRIBUTE_TRASH_ORIG_PATH);
        
        // NO_DEBUG ("orig path : %s\n", orig_path);
        
        g_object_unref (gfile);
        g_object_unref (gfile_info);
    }
    
    g_object_unref (trash_job);
    **/
    
    return 0;
}





