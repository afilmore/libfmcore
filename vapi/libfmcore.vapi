/***********************************************************************************************************************
 * libfmcore.vapi
 * 
 * Copyright 2012 Axel FILMORE <axel.filmore@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 2.
 * http://www.gnu.org/licenses/gpl-2.0.txt
 * 
 * Purpose: Binding file for libfmcore.
 * 
 * Version: 1.0
 * 
 * 
 **********************************************************************************************************************/
namespace Fm {
    
    
    /*******************************************************************************************************************
     * LibFm Core headers and Configuration.
     * 
     * 
     * 
     ******************************************************************************************************************/
	[CCode (cheader_filename = "fm.h", cprefix = "fm_")]
	public static bool init (Fm.Config config);
	
	[CCode (cheader_filename = "fm.h", cprefix = "fm_")]
	public static void finalize ();
	
    [CCode (cheader_filename = "fm.h", cprefix = "FM_WP_")]
    public enum WallpaperMode {
        COLOR,
        STRETCH,
        FIT,
        CENTER,
        TILE
    }

	[CCode (cheader_filename = "fm-config.h", type = "FmConfig*")]
	public class Config : GLib.Object {
		public weak string archiver;
		public uint big_icon_size;
		public bool confirm_del;
		public uint pane_icon_size;
		public bool show_internal_volumes;
		public bool show_thumbnail;
		public bool si_unit;
		public bool single_click;
		public uint small_icon_size;
		public weak string terminal;
		public bool thumbnail_local;
		public uint thumbnail_max;
		public uint thumbnail_size;
		public bool use_trash;
		[CCode (has_construct_function = false)]
		public Config ();
		[NoWrapper]
		public virtual void changed ();
	}

    
    /*******************************************************************************************************************
     * FmPath, FmIcon, FmMimeType, FmFileinfo and FileInfoList.
     * 
     * These are base objects used everywhere in the library, particularly Path and FileInfo.
     * 
     * 
     ******************************************************************************************************************/
	[CCode (cheader_filename =  "fm-path.h",
            cname =             "FmPath",
            cprefix =           "fm_path_",
            ref_function =      "fm_path_ref",
            unref_function =    "fm_path_unref")]
	[Compact]
	public class Path {
		
        [CCode (has_construct_function = false)]
		public Path ();
		
        public Path.for_path (string path_name);
		public static unowned Fm.Path get_desktop ();
        
        public Path.child (Fm.Path parent, string basename);
		
        [CCode (has_construct_function = false)]
		public Path.child_len (Fm.Path parent, string basename, int name_len);
		public int depth ();
		
        public unowned string display_basename ();
		public unowned string display_name (bool human_readable);
		public bool equal (Fm.Path p2);
		public bool equal_str (string str, int n);
		
        [CCode (has_construct_function = false)]
		public Path.for_commandline_arg (string arg);
		
        [CCode (has_construct_function = false)]
		public Path.for_display_name (string path_name);
		
        [CCode (has_construct_function = false)]
		public Path.for_gfile (GLib.File gf);
		
        [CCode (has_construct_function = false)]
		public Path.for_str (string path_str);
		
        [CCode (has_construct_function = false)]
		public Path.for_uri (string uri);
		
        public static unowned Fm.Path get_apps_menu ();
		public unowned string get_basename ();
		public int get_flags ();
		public static unowned Fm.Path get_home ();
		public unowned Fm.Path get_parent ();
		public static unowned Fm.Path get_root ();
		public static unowned Fm.Path get_trash ();
		public bool has_prefix (Fm.Path prefix);
		public uint hash ();
		
        [CCode (has_construct_function = false)]
		public Path.relative (Fm.Path parent, string relative_path);
		public unowned GLib.File to_gfile ();
		public unowned string to_str ();
		public unowned string to_uri ();

        public inline bool is_virtual ();
        public inline bool is_trash_root ();
        public inline bool is_trash ();
//~         #define fm_path_is_native(path) (fm_path_get_flags(path)&FM_PATH_IS_NATIVE)
//~         #define fm_path_is_local(path) (fm_path_get_flags(path)&FM_PATH_IS_LOCAL)
//~         #define fm_path_is_xdg_menu(path) (fm_path_get_flags(path)&FM_PATH_IS_XDG_MENU)
	}

	[CCode (cheader_filename = "libfm.h")]
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
	}

	[CCode (cheader_filename =  "fm-icon.h",
            ref_function =      "fm_icon_ref",
            unref_function =    "fm_icon_unref")]
	[Compact]
	public class Icon {
		public static unowned Fm.Icon from_gicon (GLib.Icon gicon);
		public static unowned Fm.Icon from_name (string name);
		public unowned Gdk.Pixbuf get_pixbuf (int size);
		public void* get_user_data ();
		public void set_user_data (void* user_data);
		public static void set_user_data_destroy (GLib.DestroyNotify func);
		public static void unload_cache ();
		public static void unload_user_data_cache ();
	}

	[CCode (cheader_filename =  "fm-mime-type.h",
            ref_function =      "fm_mime_type_ref",
            unref_function =    "fm_mime_type_unref")]
	[Compact]
	public class MimeType {
		
        [CCode (has_construct_function = false)]
		public MimeType (string type_name);
		
		public static void init ();
        public static void finalize ();
		
        public unowned string get_desc ();
		public static unowned Fm.MimeType get_for_file_name (string ufile_name);
		public static unowned Fm.MimeType get_for_native_file (string file_path, string base_name, void* pstat);
		public static unowned Fm.MimeType get_for_type (string type);
		public unowned Fm.Icon get_icon ();
	}

    [CCode (cheader_filename =  "fm-file-info.h",
            ref_function =      "fm_file_info_ref",
            unref_function =    "fm_file_info_unref")]
	[Compact]
	public class FileInfo {
		
        [CCode (has_construct_function = false)]
		public FileInfo ();
		
        [CCode (has_construct_function = false)]
		public FileInfo.computer ();
		
        [CCode (has_construct_function = false)]
		public FileInfo.trash_can ();
		
        [CCode (has_construct_function = false)]
		public FileInfo.user_special_dir (GLib.UserDirectory directory);
		
        [CCode (has_construct_function = false)]
		public FileInfo.from_gfileinfo (Fm.Path path, GLib.FileInfo inf);
		
        public bool can_thumbnail ();
		public void copy (Fm.FileInfo src);
		
        public ulong get_atime ();
		public int64 get_blocks ();
		public unowned string get_collate_key ();
		public unowned string get_desc ();
		public unowned string get_disp_mtime ();
		public unowned string get_disp_name ();
		public unowned string get_disp_size ();
		public unowned Fm.MimeType get_mime_type ();
		public uint get_mode ();
		public ulong get_mtime ();
		public unowned string get_name ();
		public unowned Fm.Path get_path ();
		public int64 get_size ();
		public unowned string get_target ();
		public bool is_desktop_entry ();
		public bool is_dir ();
		public bool is_executable_type ();
		public bool is_hidden ();
		public bool is_image ();
		public bool is_mountable ();
		public bool is_shortcut ();
		public bool is_symlink ();
		public bool is_text ();
		public bool is_unknown_type ();
		public void set_disp_name (string name);
		public void set_from_gfileinfo (GLib.FileInfo inf);
		public void set_path (Fm.Path path);
	}
    
	[CCode (cheader_filename = "fm-file-info.h", cname = "FmFileInfoList", cprefix = "fm_file_info_list_")]
	[Compact]
	public class FileInfoList<G> : Fm.List<G> {
		[CCode (has_construct_function = false)]
		public FileInfoList ();
		[CCode (has_construct_function = false)]
		public FileInfoList.from_glist ();
		public bool is_same_fs ();
		public bool is_same_type ();
	}

    
    /*******************************************************************************************************************
     * A generic list container supporting reference counting.
     * 
     * 
     ******************************************************************************************************************/
	[CCode (cheader_filename =  "fm-list.h",
            cprefix =           "fm_",
            ref_function =      "fm_list_ref",
            unref_function =    "fm_list_unref")]
	[Compact]
	public class List<G> : GLib.Queue<G> {
		
        [CCode (has_construct_function = false)]
		public List (Fm.ListFuncs funcs);
		public static void clear (void* list);
		public static void delete_link (void* list, void* l_);
		public bool is_file_info_list ();
		public bool is_path_list ();
		public static void remove (void* list, void* data);
		public static void remove_all (void* list, void* data);
	}
	
    [CCode (cheader_filename = "fm-list.h")]
	[Compact]
	public class ListFuncs {
		public weak GLib.Callback item_ref;
		public weak GLib.Callback item_unref;
	}
	

    /* *****************************************************************************************************************
     * File Launcher functions.
     * 
     * WARNING !!!! there's a problem in the delegate function definition..... see lxdesktop/src/DesktopWindow.vala
     * In action_open_folder_func ()
     * 
     * 
     ******************************************************************************************************************/
    /* C definition :
                        typedef gboolean (*FmLaunchFolderFunc) (GAppLaunchContext* ctx,
                                                                GList* folder_infos,
                                                                gpointer user_data,
                                                                GError** err);
    */
                                           
	/* public delegate bool LaunchFolderFunc (GLib.AppLaunchContext ctx,
                                              GLib.List<Fm.FileInfo> folder_infos,
                                              void* user_data) throws GLib.Error;
    */

	[CCode (cheader_filename = "fm-gtk-launcher.h")]
	public delegate bool LaunchFolderFunc (GLib.AppLaunchContext ctx,
                                           GLib.List<Fm.FileInfo> folder_infos,
                                           void* user_data,
                                           GLib.Error error);

	[CCode (cprefix = "fm_", cheader_filename = "fm-gtk-launcher.h")]
	public static bool launch_file (Gtk.Window parent,
                                           GLib.AppLaunchContext ctx,
                                           Fm.FileInfo file_info,
                                           Fm.LaunchFolderFunc func);

	[CCode (cprefix = "fm_", cheader_filename = "fm-gtk-launcher.h")]
	public static bool launch_multiple_files (Gtk.Window parent,
                                                 GLib.AppLaunchContext ctx,
                                                 GLib.List file_infos,
                                                 Fm.LaunchFolderFunc func);
    
    /* include these when needed...
	[CCode (cprefix = "fm_", cheader_filename = "fm-gtk-launcher.h")]
	public static bool launch_path_simple (Gtk.Window parent,
                                           GLib.AppLaunchContext ctx,
                                           Fm.Path path,
                                           Fm.LaunchFolderFunc func);

	[CCode (cprefix = "fm_", cheader_filename = "fm-gtk-launcher.h")]
	public static bool launch_paths_simple (Gtk.Window parent,
                                            GLib.AppLaunchContext ctx,
                                            GLib.List paths,
                                            Fm.LaunchFolderFunc func);

    [CCode (cprefix = "fm_", cheader_filename = "fm-gtk-launcher.h")]
	public static bool launch_desktop_entry (GLib.AppLaunchContext ctx,
                                             string file_or_id,
                                             GLib.List uris,
                                             Fm.FileLauncher launcher);

	[CCode (cprefix = "fm_", cheader_filename = "fm-gtk-launcher.h")]
	public static bool launch_files (GLib.AppLaunchContext ctx,
                                     GLib.List file_infos,
                                     Fm.FileLauncher launcher);

	[CCode (cprefix = "fm_", cheader_filename = "fm-gtk-launcher.h")]
	public static bool launch_paths (GLib.AppLaunchContext ctx,
                                     GLib.List paths,
                                     Fm.FileLauncher launcher);
    */
    
    
    /*******************************************************************************************************************
     * Gtk Folder Model.
     * 
     * 
     ******************************************************************************************************************/
    [CCode (cheader_filename = "fm-folder.h", cprefix = "COL_FILE_")]
    public enum FileColumn {
        GICON = 0,
        ICON,
        NAME,
        SIZE,
        DESC,
        PERM,
        OWNER,
        MTIME,
        INFO,
        [CCode (cheader_filename = "fm-folder.h", cprefix = "")]
        N_FOLDER_MODEL_COLS
    }

	[CCode (cheader_filename = "fm-folder.h")]
	public class Folder : GLib.Object {
		
		public weak Fm.FileInfo dir_fi;

        [CCode (has_construct_function = false)]
		protected Folder ();
		
        public static unowned Fm.Folder @get (Fm.Path path);
		public unowned Fm.FileInfo get_file_by_name (string name);
		public unowned Fm.FileInfoList get_files ();
		public bool get_filesystem_info (uint64 total_size, uint64 free_size);
		public static unowned Fm.Folder get_for_gfile (GLib.File gf);
		
        [CCode (has_construct_function = false)]
		public static unowned Fm.Folder get_for_path_name (string path);
		public static unowned Fm.Folder get_for_uri (string uri);
		
        public bool get_is_loaded ();
		public void query_filesystem_info ();
		public void reload ();
		public virtual signal void changed ();
		public virtual signal void content_changed ();
		public virtual signal int error (void* err, int severity);
		public virtual signal void files_added (void* files);
		public virtual signal void files_changed (void* files);
		public virtual signal void files_removed (void* files);
		public virtual signal void fs_info ();
		public virtual signal void loaded ();
		public virtual signal void removed ();
		public virtual signal void unmount ();
	}
    
    [CCode (cheader_filename = "fm-folder-model.h")]
	public class FolderModel : GLib.Object, Gtk.TreeModel, Gtk.TreeSortable, Gtk.TreeDragSource, Gtk.TreeDragDest {

		public weak Fm.Folder dir;

		[CCode (has_construct_function = false)]
		public FolderModel (Fm.Folder dir, bool show_hidden);
		
        public void file_changed (Fm.FileInfo file);
		public void file_created (Fm.FileInfo file);
		public void file_deleted (Fm.FileInfo file);
		
        public bool find_iter_by_filename (Gtk.TreeIter it, string name);
		
        public void get_common_suffix_for_prefix (string prefix, GLib.Callback file_info_predicate, string common_suffix);
		
        public uint get_icon_size ();
		public bool get_is_loaded ();
		public bool get_show_hidden ();
		
        public void set_folder (Fm.Folder dir);
		public void set_icon_size (uint icon_size);
		public void set_show_hidden (bool show_hidden);
		
        public virtual signal void loaded ();
	}
    
    
    /*******************************************************************************************************************
     * File/Folder contextual menu.
     * 
     * 
     ******************************************************************************************************************/
	[CCode (cheader_filename =  "fm-file-menu.h",
            cprefix =           "fm_file_menu_",
            cname =             "FmFileMenu",
            free_function =     "fm_file_menu_destroy")]
	[Compact]
	public class FileMenu {
		[CCode (has_construct_function = false)]
		public FileMenu.for_file (Gtk.Window parent, Fm.FileInfo fi, Fm.Path cwd, bool auto_destroy);
		[CCode (has_construct_function = false)]
		public FileMenu.for_files (Gtk.Window parent, Fm.FileInfoList files, Fm.Path cwd, bool auto_destroy);
		public unowned Gtk.ActionGroup get_action_group ();
		public unowned Fm.FileInfoList get_file_info_list ();
		public unowned Gtk.Menu get_menu ();
		public unowned Gtk.UIManager get_ui ();
		public bool is_single_file_type ();
		public void set_folder_func (Fm.LaunchFolderFunc func);
	}
    
    
    /*******************************************************************************************************************
     * Drag And Drop...
     * 
     * 
     ******************************************************************************************************************/
    /*
    #define fm_drag_context_has_target(ctx, target) \
        (g_list_find(ctx->targets, target) != NULL)
    */
	[CCode (cheader_filename = "fm-dnd-dest.h", cprefix = "fm_")]
    public inline bool drag_context_has_target (Gdk.DragContext drag_context, Gdk.Atom target);

	[CCode (cheader_filename = "fm-dnd-dest.h", cprefix = "fm_")]
    extern Gtk.TargetEntry default_dnd_dest_targets[];

    /*
    #define fm_drag_context_has_target_name(ctx, name)  \
        fm_drag_context_has_target(ctx, gdk_atom_intern_static_string(name))
    */

    /* default droppable targets */
    //~ enum
    //~ {
    //~     FM_DND_DEST_TARGET_FM_LIST, /* direct pointer of FmList */
    //~     FM_DND_DEST_TARGET_URI_LIST, /* text/uri-list */
    //~     FM_DND_DEST_TARGET_XDS, /* X direct save */
    //~     N_FM_DND_DEST_DEFAULT_TARGETS
    //~ };

    [CCode (cheader_filename = "fm-dnd-dest.h", cprefix = "FM_DND_DEST_TARGET_")]
    public enum DndDestTarget {
        FM_LIST,
        URI_LIST,
        XDS,
        [CCode (cheader_filename = "fm-dnd-dest.h", cname = "N_FM_DND_DEST_DEFAULT_TARGETS", cprefix = "")]
        DEFAULT
    }
    
    [CCode (cheader_filename = "fm-dnd-dest.h", cprefix = "fm_dnd_dest_", cname = "FmDndDest")]
	public class DndDest : GLib.Object {
		
        [CCode (has_construct_function = false)]
		public DndDest (Gtk.Widget w);
		
        public bool drag_data_received (Gdk.DragContext drag_context,
                                        int x,
                                        int y,
                                        Gtk.SelectionData sel_data,
                                        uint info,
                                        uint time);
		
        public bool drag_drop (Gdk.DragContext drag_context, Gdk.Atom target, int x, int y, uint time);
		public void drag_leave (Gdk.DragContext drag_context, uint time);
		public Gdk.Atom find_target (Gdk.DragContext drag_context);
		public Gdk.DragAction get_default_action (Gdk.DragContext drag_context, Gdk.Atom target);
		public unowned Fm.FileInfo get_dest_file ();
		public unowned Fm.Path get_dest_path ();
		public unowned Fm.List get_src_files ();
		public bool is_target_supported (Gdk.Atom target);
		public void set_dest_file (Fm.FileInfo? dest_file);
		public void set_widget (Gtk.Widget w);
		public virtual signal bool files_dropped (int x, int y, uint action, uint info_type, void* files);
	}
	
    [CCode (cheader_filename = "fm-dnd-src.h", cprefix = "fm_dnd_src_", cname = "FmDndSrc")]
	public class DndSrc : GLib.Object {
		
        [CCode (has_construct_function = false)]
		public DndSrc (Gtk.Widget w);
		
        public unowned Fm.FileInfoList get_files ();
		public void set_file (Fm.FileInfo file);
		public void set_files (Fm.FileInfoList files);
		public void set_widget (Gtk.Widget w);
		public virtual signal void data_get ();
	}
    
    
    /*******************************************************************************************************************
     * Misc Usefull Functions and Dialogs...
     * 
     * 
     ******************************************************************************************************************/
    [CCode (cheader_filename = "fm.h")]
    public static string? get_user_input (Gtk.Window parent, string title, string msg, string default_text);


    namespace Clipboard {
        //~ #define fm_clipboard_cut_files(src_widget, files)	\
        //~ fm_clipboard_cut_or_copy_files(src_widget, files, TRUE)
        //~ 
        //~ #define fm_clipboard_copy_files(src_widget, files)	\
        //~ fm_clipboard_cut_or_copy_files(src_widget, files, FALSE)
        //~ 
        //~ gboolean fm_clipboard_cut_or_copy_files(GtkWidget* src_widget, FmPathList* files, gboolean _is_cut);
        
        [CCode (cheader_filename = "fm-clipboard.h", cprefix = "fm_clipboard_")]
        public bool paste_files (Gtk.Widget dest_widget, Fm.Path dest_dir);
    }

	[CCode (cheader_filename = "fm-gtk-utils.h")]
	public static void copy_files (Gtk.Window parent, Fm.PathList files, Fm.Path dest_dir);

	[CCode (cheader_filename = "fm-gtk-utils.h")]
	public inline void copy_file (Gtk.Window parent, Fm.Path files, Fm.Path dest_dir);


}


