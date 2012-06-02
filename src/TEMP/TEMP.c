



// moved to FmFileInfo...
gboolean    fm_file_info_job_get_info_for_native_file   (FmJob *job, FmFileInfo *file_info,
                                                         const char *path, GError **err);

gboolean fm_file_info_job_get_info_for_native_file (FmJob *job, FmFileInfo *file_info, const char *path, GError **err)
{
	struct stat st;
    gboolean is_link;
    
_retry:
	
    if (lstat (path, &st) != 0)
    {
        g_set_error (err, G_IO_ERROR, g_io_error_from_errno (errno), "%s", g_strerror (errno));
		return FALSE;
    }
	
    char *type;
    
    file_info->disp_name = file_info->path->name;
    file_info->mode = st.st_mode;
    file_info->mtime = st.st_mtime;
    file_info->atime = st.st_atime;
    file_info->size = st.st_size;
    file_info->dev = st.st_dev;
    file_info->uid = st.st_uid;
    file_info->gid = st.st_gid;

    if (fm_job_is_cancelled (FM_JOB (job)))
        return TRUE;
        
    // Get Link Target...
    if (S_ISLNK (st.st_mode))
    {
        stat (path, &st);
        file_info->target = g_file_read_link (path, NULL);
    }

    FmMimeType *mime_type = fm_mime_type_get_for_native_file (path, file_info->disp_name, &st);
    
    g_return_val_if_fail (mime_type, FALSE);
    
    // FIXME_axl: avoid direct member access !!! there's direct members access everywhere anyway... nasty code...
    file_info->mime_type = mime_type;
    
    // TODO_axl: Create one function.....
    if (G_LIKELY (!fm_file_info_is_desktop_entry (file_info)))
    {
        fm_file_info_set_fm_icon (file_info, mime_type->icon);
        return TRUE;
    }
    
    fm_file_info_set_from_desktop_entry (file_info);
    
    return TRUE;
}


