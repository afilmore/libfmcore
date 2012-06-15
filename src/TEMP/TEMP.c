


fm-trash...


    /**if (delete_flags == FM_DELETE_FLAGS_TRASH)
    {
        if (!fm_config->confirm_delete || fm_yes_no (parent, NULL, _("Do you want to move the selected files to trash can?"), TRUE))
        {
            FmJob *job = fm_file_ops_job_new (FM_FILE_OP_TRASH, path_list);
            fm_file_ops_job_run_with_progress (parent, FM_FILE_OPS_JOB (job));
        }
    }
    else if (delete_flags == FM_DELETE_FLAGS_TRASH_OR_DELETE)
    {
        if (fm_list_is_empty (path_list))
            return;
        
        
        // TODO_axl: add a function to FmPath to do this...
        gboolean all_in_trash = TRUE;
        if (fm_config->use_trash_can)
        {
            GList *l = fm_list_peek_head_link (path_list);
            for (;l;l=l->next)
            {
                FmPath *path = FM_PATH (l->data);
                if (!fm_path_is_trash_file (path))
                    all_in_trash = FALSE;
            }
        }

        
        // files already in trash:/// should only be deleted and cannot be trashed again.
        if (fm_config->use_trash_can && !all_in_trash)
            _fm_trash_files (parent, path_list);
        else
            _fm_delete_files (parent, path_list);
    }**/


/*void fm_trash_or_delete_files (GtkWindow *parent, FmPathList *path_list)
{
    if (!fm_list_is_empty (path_list))
    {
        gboolean all_in_trash = TRUE;
        if (fm_config->use_trash_can)
        {
            GList *l = fm_list_peek_head_link (path_list);
            for (;l;l=l->next)
            {
                FmPath *path = FM_PATH (l->data);
                if (!fm_path_is_trash_file (path))
                    all_in_trash = FALSE;
            }
        }

        // files already in trash:/// should only be deleted and cannot be trashed again.
        if (fm_config->use_trash_can && !all_in_trash)
            fm_trash_files (parent, path_list);
        else
            _fm_delete_files (parent, path_list);
    }
}*/








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




from FmPath...

#if 0
static inline FmPath *_fm_path_reuse_existing_paths (FmPath *parent, const char *basename, int name_len)
{
    FmPath *current;
    /* This is a way to reuse cached FmPath objects created for $HOME and desktop dir.
     * Since most of the files a user may use are under $HOME, reusing this can
     * more or less reduce memory usage. However, this may slow things down a little. */
    for (current = desktop_path; current; current = current->parent)
    {
        if (fm_path_equal (current->parent, parent))
        {
            if (strncmp (basename, current->name, name_len) == 0 && current->name[name_len] == '\0')
                return fm_path_ref (current);
            break;
        }
    }
    return NULL;
}
#endif




#if 0
/*
 * fm_path_new
 * DEPRECATED function
 */
FmPath *fm_path_new (const char *path)
{
    // FIXME_pcm: need to canonicalize paths

    if ( path[0] == '/' ) // if this is a absolute native path
    {
        if  (path[1])
            return fm_path_new_relative (root, path + 1);
        else
            // special case: handle root dir
            return fm_path_ref ( root );
    }
    else if  ( path[0] == '~' &&  (path[1] == '\0' || path[1]=='/') ) // home dir
    {
        ++path;
        return *path ? fm_path_new_relative (home, path) : fm_path_ref (home);
    }
    else // then this should be a URL
    {
        FmPath *parent, *ret;
        char *colon = strchr (path, ':');
        char *hier_part;
        char *rest;
        int root_len;

        // return root instead of NULL for invalid URIs. fix #2988010.
        if ( !colon ) // this shouldn't happen
            return fm_path_ref (root); // invalid path FIXME_pcm: should we treat it as relative path?

        // FIXME_pcm: convert file:/// to local native path
        hier_part = colon+1;
        if ( hier_part[0] == '/' )
        {
            if (hier_part[1] == '/') // this is a scheme:// form URI
                rest = hier_part + 2;
            else // a malformed URI
                rest = hier_part + 1;

            if (*rest == '/') // :/// means there is no authoraty part
                ++rest;
            else // we are now at autority part, something like <username>@domain/
            {
                while ( *rest && *rest != '/' )
                    ++rest;
                if (*rest == '/')
                    ++rest;
            }

            if ( strncmp (path, "trash:", 6) == 0 ) // in trash://
            {
                if (*rest)
                    return fm_path_new_relative (trash_root, rest);
                else
                    return fm_path_ref (trash_root);
            }
            // other URIs which requires special handling, like computer:///
        }
        else // this URI doesn't have //, like mailto:
        {
            // FIXME_pcm: is this useful to file managers?
            rest = colon + 1;
        }
        root_len =  (rest - path);
        parent = fm_path_new_child_len (NULL, path, root_len);
        if (*rest)
        {
            ret = fm_path_new_relative (parent, rest);
            fm_path_unref (parent);
        }
        else
            ret = parent;
        return ret;
    }
    return fm_path_new_relative (NULL, path);
}
#endif




