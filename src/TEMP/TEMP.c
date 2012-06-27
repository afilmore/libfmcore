fm-file-info-job.c


gboolean fm_file_info_job_run (FmJob *fmjob)
{
	FmFileInfoJob *file_info_job = (FmFileInfoJob*) fmjob;
    
    GError *gerror = NULL;

    // gio is really slower, also there's a problem with symlinks, the panel launcher no longer works...
    //~ gboolean use_gio = TRUE;
    gboolean use_gio = FALSE;
    
    GList *l;
	for (l = fm_list_peek_head_link (file_info_job->file_info_list); !fm_job_is_cancelled (fmjob) && l; )
	{
		FmFileInfo *file_info = (FmFileInfo*) l->data;
        
        GList *next = l->next;

        
        // TODO_axl: it's possible ot create a file info here for the input FmPath...
        
        file_info_job->current = file_info->path;

		
        // This is a xdg menu
        if (fm_path_is_xdg_menu (file_info->path))
        {
            g_return_val_if_fail (global_menu_cache != NULL, FALSE);
            
            
            // Menu path as "menu://applications/system/Administration"...
            char *path_str = fm_path_to_str (file_info->path);
            
            //DEBUG ("DEBUG: fm_file_info_job_run: %s\n", path_str);
            
            // Get the file menu name...
            char *menu_name = path_str + 5;
            
            while (*menu_name == '/')
                ++menu_name;
            
            // Get the directory name such as "Administration"...
            char *dir_name = menu_name;
            
            while (*dir_name && *dir_name != '/')
                ++dir_name;
            
            char *ch = *dir_name;
            *dir_name = '\0';
            
            
            /** Menu name as "applications.menu"...
            menu_name = g_strconcat (menu_name, ".menu", NULL);
            
            
            //DEBUG ("DEBUG: fm_file_info_job_run: menu name = %s\n", menu_name);
            
            DEBUG ("DEBUG: fm_file_info_job_run: enter menu_cache_lookup_sync ()\n");
            
            //~ MenuCache *mc;
            //~ if (fm_config->application_menu)
                //~ mc = menu_cache_lookup_sync (fm_config->application_menu);
            //~ else
                //~ mc = menu_cache_lookup_sync ("/etc/xdg/menus/applications.menu");
            
            
            //~ MenuCache *mc = menu_cache_lookup_sync (menu_name);
            //~ MenuCache *mc = menu_cache_lookup_sync ("/etc/xdg/menus/applications.menu");
            
            DEBUG ("DEBUG: fm_file_info_job_run: leave menu_cache_lookup_sync ()\n");
            
            g_free (menu_name);
            **/
            
            MenuCacheDir *menu_cache_dir;
            if (*dir_name && !(*dir_name == '/' && dir_name[1] == '\0'))
            {
                DEBUG ("DEBUG: fm_file_info_job_run: dir_name = %s\n", dir_name);
                char *tmp = g_strconcat (
                    "/",
                    menu_cache_item_get_id (MENU_CACHE_ITEM (menu_cache_get_root_dir (global_menu_cache))),
                    dir_name,
                    NULL
                );
                
                menu_cache_dir = menu_cache_get_dir_from_path (global_menu_cache, tmp);
                
                g_free (tmp);
            }
            else
            {
                DEBUG ("DEBUG: fm_file_info_job_run: get root dir\n");
                menu_cache_dir = menu_cache_get_root_dir (global_menu_cache);
            }
            
            DEBUG ("DEBUG: fm_file_info_job_run: menu cache dir = %s\n", menu_cache_item_get_name (menu_cache_dir));
            DEBUG ("DEBUG: fm_file_info_job_run: icon = %s\n", menu_cache_item_get_icon (menu_cache_dir));
            
            if (menu_cache_dir)
            {
                fm_file_info_set_for_menu_cache_item (file_info, (MenuCacheItem*) menu_cache_dir);
            }
            else
            {
                next = l->next;
                fm_list_delete_link (file_info_job->file_info_list, l); // Also calls unref...
            }
            
            g_free (path_str);
            
            //menu_cache_unref (global_menu_cache);
            
            l = l->next;
            continue;
        
        }
        
        // Query virtual items with GIO...
        else if (use_gio || fm_path_is_virtual (file_info->path))
        {
            
            
            if (!fm_file_info_query (file_info, fm_job_get_cancellable (FM_JOB (file_info_job)), &gerror))
            {
                FmErrorAction error_action = fm_job_emit_error (FM_JOB (file_info_job), gerror, FM_SEVERITY_MILD);
                
                g_error_free (gerror);
                gerror = NULL;
                
                if (error_action == FM_ERROR_ACTION_RETRY)
                    continue;

                next = l->next;
                
                fm_list_delete_link (file_info_job->file_info_list, l);   // Also calls unref...
            }
			
        
            l = next;
            continue;
        
        }
        
        // A native file, query file infos with posix...
        else if (fm_path_is_native (file_info->path))
		{
			//char *path_str = fm_path_to_str (file_info->path);
			
            //~ if (!fm_file_info_query_native_file (file_info))
            
            if (!fm_file_info_query (file_info, NULL, NULL))
            {
                /** TODO_axl: error handling...
                FmErrorAction error_action = fm_job_emit_error (FM_JOB(file_info_job), gerror, FM_SEVERITY_MILD);
                
                g_error_free (gerror);
                gerror = NULL;
                
                if (error_action == FM_ERROR_ACTION_RETRY)
                    continue;
                **/
                
                //DEBUG ("fm_file_info_set_for_native_file: error reading %s\n", NULL);
                
                next = l->next;
                
                fm_list_delete_link (file_info_job->file_info_list, l); // Also calls unref...
            }
			
            //g_free (path_str);
            
            l = next;
            continue;
		}
        else
        {
            DEBUG ("FmFileInfoJob: ERROR !!!!\n");
        }
        
        l = next;
	
    }
	
    return TRUE;
}





fm-path.c


/**
static int fm_path_strlen (FmPath *path)
{
    int len = 0;
    for (;;)
    {
        len += strlen (path->name);
        if (G_UNLIKELY (!path->parent))
            break;
        if (path->parent->parent)
            ++len; // add a character for separator
        path = path->parent;
    }
    return len;
}**/





fm-file-ops.h

//~ void fm_move_files (GtkWindow *parent, FmPathList *path_list, FmPath *dest_dir)
//~ {
	//~ FmGtkFileJobUI* ui = fm_gtk_file_job_ui_new(parent);
	//~ 
    //~ FmJob* job = fm_move_files_to_dir(path_list, dest_dir, ui);
	//~ 
    //~ g_object_unref(ui);
	//~ g_object_unref(job);
//~ }
//~ 

//~ #define fm_copy_file(parent, file, dest_dir) \
    //~ G_STMT_START {    \
        //~ FmPathList *files = fm_path_list_new (); \
        //~ fm_list_push_tail (files, file); \
        //~ fm_copy_files (parent, files, dest_dir); \
        //~ fm_list_unref (files);   \
    //~ } G_STMT_END

//~ #define fm_move_file(parent, file, dest_dir) \
    //~ G_STMT_START {    \
    //~ FmPathList *files = fm_path_list_new (); \
    //~ fm_list_push_tail (files, file); \
    //~ fm_move_files (parent, files, dest_dir); \
    //~ fm_list_unref (files);   \
    //~ } G_STMT_END

//~ void fm_move_or_copy_files_to (GtkWindow *parent, FmPathList *files, gboolean is_move);
//~ 
//~ #define fm_move_files_to (parent, files) fm_move_or_copy_files_to (parent, files, TRUE)
//~ #define fm_copy_files_to (parent, files) fm_move_or_copy_files_to (parent, files, FALSE)
//~ 

/**
void fm_move_or_copy_files_to (GtkWindow *parent, FmPathList *files, gboolean is_move)
{
    FmPath *dest = fm_select_folder (parent, NULL);
    if (dest)
    {
        if (is_move)
            fm_move_files (parent, files, dest);
        else
            fm_copy_files (parent, files, dest);
        fm_path_unref (dest);
    }
}**/





vapi file...

	/**[CCode (cheader_filename = "fm.h")]
	[Compact]
	public class PathList {
		
        [CCode (has_construct_function = false)]
		public PathList ();
		
        [CCode (has_construct_function = false)]
		public PathList.from_file_info_glist (GLib.List fis);
		
        [CCode (has_construct_function = false)]
		public PathList.from_file_info_gslist (GLib.SList fis);
		
        [CCode (has_construct_function = false)]
		public PathList.from_file_info_list (Fm.List fis);
		
        [CCode (has_construct_function = false)]
		public PathList.from_uri_list (string uri_list);
		
        [CCode (has_construct_function = false)]
		public PathList.from_uris (out unowned string uris);
		
        public unowned string to_uri_list ();
		public void write_uri_list (GLib.StringBuilder buf);
	}**/
    

    /*************************************************************************************
     * 
     * 
     * 
     ***********************************************************************************
	[CCode (cheader_filename = "fm.h")]
	public class Job : GLib.Object {
		
        [CCode (has_construct_function = false)]
		protected Job ();
		
        public int ask (string question);
		public int ask_valist (string question, void* options);
		public int askv (string question, out unowned string options);
        
//		public void* call_main_thread (Fm.JobCallMainThreadFunc func);
		
		public void init_cancellable ();
        public unowned GLib.Cancellable get_cancellable ();
        public virtual void cancel ();
		public void emit_cancelled ();
		public bool is_cancelled ();
		
        public int emit_error (GLib.Error err, int severity);
		
        public void emit_finished ();
		public void finish ();
		
        [NoWrapper]
		public virtual bool run ();
		public virtual bool run_async ();
		public bool run_sync ();
		public bool run_sync_with_mainloop ();
		
        public bool is_running ();
		
        public void set_cancellable (GLib.Cancellable cancellable);
		
        // FIXME_axl: rename this signal...
        public virtual signal int ask2 (void* question, void* options);
		public virtual signal void cancelled ();
		public virtual signal int error (void* err, int severity);
		public virtual signal void finished ();
	}*/






fm-file-menu...

debug flags...

    /**
    printf ("have flags (OR) = %d, or all flags (AND) = %d\n", have_flags, all_flags);
    
    if (have_flags & FM_PATH_IS_NATIVE)
    {
        printf ("FM_PATH_IS_NATIVE\n");
    }
    
    if (have_flags & FM_PATH_IS_VIRTUAL)
    {
        printf ("FM_PATH_IS_VIRTUAL\n");
    }
    
    if (have_flags & FM_PATH_IS_LOCAL)
    {
        printf ("FM_PATH_IS_LOCAL\n");
    }
    
    if (have_flags & FM_PATH_IS_ROOT)
    {
        printf ("FM_PATH_IS_ROOT\n");
    }
    
    if (have_flags & FM_PATH_IS_TRASH)
    {
        printf ("FM_PATH_IS_TRASH\n");
    }
    
    if (have_flags & FM_PATH_IS_XDG_MENU)
    {
        printf ("FM_PATH_IS_XDG_MENU\n");
    }
    
    if (have_flags != all_flags)
    {
        printf ("different flags\n");
    }**/
    
debug apps...

            /**g_debug ("app %s, executable %s, command %s\n",
                g_app_info_get_name (app),
                g_app_info_get_executable (app),
                g_app_info_get_commandline (app));**/






// TODO_axl: move this to a TEMP_file...
/*** Needs a rework...
FmFileMenu *fm_file_menu_new_for_files (GtkWindow *parent, FmFileInfoList *files, FmPath *cwd, gboolean auto_destroy)
{
    // Special handling for some virtual filesystems
    g_string_append (xml, "<popup><placeholder name='ph1'>\n");
    if (file_menu->all_virtual)
    {
        // if all of the files are in trash
        if (file_menu->all_trash)
        {
            gboolean can_restore = TRUE;
            GList *l;
            
            // only immediate children of trash:/// can be restored.
            for (l = fm_list_peek_head_link (files);l;l=l->next)
            {
                FmPath *trash_path = FM_FILE_INFO (l->data)->path;
                if (!trash_path->parent || !fm_path_is_trash_root (trash_path->parent))
                {
                    can_restore = FALSE;
                    break;
                }
            }

            if (can_restore)
            {
                action = gtk_action_new ("UnTrash",
                                    _("_Restore"),
                                    _("Restore trashed files to original paths"),
                            NULL);
                
                g_signal_connect (action, "activate", G_CALLBACK (on_restaure), file_menu);
                
                gtk_action_group_add_action (action_group, action);
                
                g_string_append (xml, "<menuitem action='UnTrash'/>\n");
            }

            action = gtk_ui_manager_get_action (ui, "/popup/Open");
            gtk_action_set_visible (action, FALSE);
        }
        else
        {
            // do not provide these items for other virtual files
            action = gtk_ui_manager_get_action (ui, "/popup/Cut");
            gtk_action_set_visible (action, FALSE);
            action = gtk_ui_manager_get_action (ui, "/popup/Copy");
            gtk_action_set_visible (action, FALSE);
            action = gtk_ui_manager_get_action (ui, "/popup/Paste");
            gtk_action_set_visible (action, FALSE);
            action = gtk_ui_manager_get_action (ui, "/popup/Delete");
            gtk_action_set_visible (action, FALSE);
        }
        
        action = gtk_ui_manager_get_action (ui, "/popup/Rename");
        gtk_action_set_visible (action, FALSE);
    }
    g_string_append (xml, "</placeholder></popup>\n");
}
***/

/***
 * Add Custom Actions Later...
 * #include "fm-actions.h"
 * 
 ***/


FmFileMenu *fm_file_menu_new_for_files (GtkWindow *parent, FmFileInfoList *files, FmPath *cwd, gboolean auto_destroy)
    #if 0
	// add custom file actions
	if (show_custom_actions)
        fm_file_menu_add_custom_actions (file_menu, xml, files);
    #endif


/*****************************************************************************************
 * Customs Actions...
 *
 * Add This Later...
 *
 *
 ****************************************************************************************/
#if 0
static void add_custom_action_item (FmFileMenu *file_menu, GString *xml, FmFileActionItem *item)
{
	GtkAction *action;
	if (!item) // separator
	{
		g_string_append (xml, "<separator/>");
		return;
	}

	if (fm_file_action_item_is_action (item))
	{
		if (! (fm_file_action_item_get_target (item) & FM_FILE_ACTION_TARGET_CONTEXT))
			return;
	}

	action = gtk_action_new (fm_file_action_item_get_id (item),
						  fm_file_action_item_get_name (item),
						  fm_file_action_item_get_desc (item),
						  NULL);

	if (fm_file_action_item_is_action (item))
		g_signal_connect (action, "activate", G_CALLBACK (on_custom_action), file_menu);

	gtk_action_set_icon_name (action, fm_file_action_item_get_icon (item));
	gtk_action_group_add_action (file_menu->action_group, action);
	
    // associate the app info object with the action
	g_object_set_qdata_full (G_OBJECT (action), fm_qdata_id, fm_file_action_item_ref (item),
							 (GDestroyNotify) fm_file_action_item_unref);
	
    if (fm_file_action_item_is_menu (item))
	{
		GList *subitems = fm_file_action_item_get_sub_items (item);
		GList *l;
		
        g_string_append_printf (xml, "<menu action='%s'>", fm_file_action_item_get_id (item));
		
        for (l=subitems; l; l=l->next)
		{
			FmFileActionItem *subitem = FM_FILE_ACTION_ITEM (l->data);
			add_custom_action_item (file_menu, xml, subitem);
		}
		g_string_append (xml, "</menu>");
	}
	else
	{
		g_string_append_printf (xml, "<menuitem action='%s'/>", fm_file_action_item_get_id (item));
	}
}

static void fm_file_menu_add_custom_actions (FmFileMenu *file_menu, GString *xml, FmFileInfoList *files)
{
	GList *files_list = fm_list_peek_head_link (files);
	GList *items = fm_get_actions_for_files (files_list);

    if (items)
    {
		g_string_append (xml, "<popup><placeholder name='ARCHIVER'>");
		GList *l;
		for (l=items; l; l=l->next)
		{
			FmFileActionItem *item = FM_FILE_ACTION_ITEM (l->data);
			add_custom_action_item (file_menu, xml, item);
		}
		g_string_append (xml, "</placeholder></popup>");
    }
	
    g_list_foreach (items, (GSourceFunc) fm_file_action_item_unref, NULL);
	g_list_free (items);
}

static void on_custom_action (GtkAction *action, FmFileMenu *file_menu)
{
    FmFileActionItem *item = FM_FILE_ACTION_ITEM (g_object_get_qdata (action, fm_qdata_id));
    GdkAppLaunchContext *ctx = gdk_app_launch_context_new ();
    GList *files = fm_list_peek_head_link (file_menu->file_infos);
    char **output = NULL;
    gboolean ret;
    gdk_app_launch_context_set_screen (ctx, gtk_widget_get_screen (file_menu->menu));
    gdk_app_launch_context_set_timestamp (ctx, gtk_get_current_event_time ());

	g_debug ("item: %s is activated, id:%s", fm_file_action_item_get_name (item),
			fm_file_action_item_get_id (item));
	ret = fm_file_action_item_launch (item, ctx, files, output);
	if (output)
	{
		fm_show_error (NULL, "output", output);
		g_free (output);
	}
}

#endif











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




