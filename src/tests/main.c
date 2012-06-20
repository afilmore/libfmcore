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
    
    GList  *l;
    
    for (l = fm_list_peek_head_link (trash_job->files); l; l = l->next)
    {
        FmFileInfo *inf = (FmFileInfo*) l->data;
        
        /**char *path_str = fm_path_to_str (inf->path);
        
        printf ("file name : %s\n", path_str);
        
        gboolean replace = TRUE;
        
        if (!g_utf8_validate (path_str, -1, NULL))
        {
            printf ("invalid !!!!\n");
        }

        char *tmp_str;
        if (replace)
        {
            //tmp_str = fm_str_replace (path_str, "%", "%25");
            //printf ("replace : %s\n", tmp_str);
            
            tmp_str = g_uri_escape_string (path_str, G_URI_RESERVED_CHARS_ALLOWED_IN_PATH, TRUE);
            printf ("g_uri_escape_string : %s\n", tmp_str);
            
            
        }
        else
        {
            tmp_str = g_utf8_normalize (path_str, -1, G_NORMALIZE_ALL);
            //tmp_str = g_strdup (path_str);
        }
        
        
        g_free (path_str);
        
        if (!g_utf8_validate (tmp_str, -1, NULL))
        {
            printf ("invalid : %s\n", tmp_str);
        }
        else
        {
            printf ("valid : %s\n", tmp_str);
        }
        
        GFile *gfile = g_file_new_for_uri (tmp_str); 
        
        
        g_free (tmp_str);**/
        
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





