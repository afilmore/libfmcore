/***********************************************************************************************************************
 * 
 * 
 * 
 * 
 * 
 **********************************************************************************************************************/
#include <fm.h>

int main (int argc, char *argv[])
{
    
    gtk_init (&argc, &argv);
    fm_init (NULL);
    
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
        
        printf ("orig path : %s\n", orig_path);
        
        g_object_unref (gfile);
        g_object_unref (gfile_info);
    }
    
    g_object_unref (trash_job);
    
    return 0;
}





