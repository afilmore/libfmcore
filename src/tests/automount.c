/***********************************************************************************************************************
 * 
 * 
 * 
 * 
 * 
 **********************************************************************************************************************/
#include <fm.h>

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
        //~ printf ("success: %s\n", g_file_get_display_name (target));
        g_object_unref (target);
    }
        
    g_main_loop_quit (loop);
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
        
        FmPath *path = fm_file_info_get_path ((FmFileInfo*) l->data);
        GFile *gfile = fm_path_to_gfile (path);
        
        g_return_val_if_fail (gfile != NULL, 1);
        
        printf ("path2: %s\n", fm_path_to_str (path));
        
        g_file_mount_mountable (gfile, 0, NULL, NULL, mount_mountable_done_cb, NULL);
        g_object_unref (gfile);
        
        if (g_main_loop_is_running (loop))
        {
            GDK_THREADS_LEAVE ();
            g_main_loop_run (loop);
            GDK_THREADS_ENTER ();
        }

        g_main_loop_unref (loop);
    }
    
    g_object_unref (dir_list_job);
    
    return 0;
}





