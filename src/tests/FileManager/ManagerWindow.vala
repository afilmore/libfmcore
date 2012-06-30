/***********************************************************************************************************************
 *      
 *      ManagerWindow.vala
 * 
 *      Copyright 2012 Axel FILMORE <axel.filmore@gmail.com>
 * 
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License Version 2.
 *      http://www.gnu.org/licenses/gpl-2.0.txt
 * 
 * 
 *      Purpose: A simple file manager demo in Vala.
 * 
 * 
 * 
 **********************************************************************************************************************/
namespace Manager {
    
    
    private enum DirChangeCaller {
        NONE,
        PATH_ENTRY,
        DIR_TREEVIEW,
        FOLDER_VIEW
    }
    
    public class Window : Gtk.Window {
        
        
        private Fm.Path         _current_dir;
        
        // UI widgets...
        private Gtk.HPaned      _hpaned;
        private Fm.DirTreeModel _dir_tree_model;
        private Fm.DirTreeView  _tree_view;
        private Fm.FolderView   _folder_view;
        
        private Fm.FileMenu?    _fm_file_menu;
        private Gtk.Menu?       _file_popup;
        
        public Window () {
            
            this.destroy.connect ( () => {
                
                    Gtk.main_quit ();
            });
        }
        
        
        /*********************************************************************************
         * Widget Creation...
         * 
         * 
         ********************************************************************************/
        public bool create () {
            
            this.set_default_size ((screen.get_width() / 4) * 3, (screen.get_height() / 4) * 3);
            this.set_position (Gtk.WindowPosition.CENTER);

            _current_dir = new Fm.Path.for_str (Environment.get_user_special_dir (UserDirectory.DESKTOP));
            
            
            /*****************************************************************************
             * Main Window Container...
             * 
             * 
             ****************************************************************************/
            Gtk.VBox main_vbox = new Gtk.VBox (false, 0);


            /*****************************************************************************
             * Create The Paned Container...
             * 
             * 
             ****************************************************************************/
            _hpaned = new Gtk.HPaned ();
            _hpaned.set_position (200);
            main_vbox.pack_start (_hpaned, true, true, 0);
            
            
            /*****************************************************************************
             * Create The Left Side ScrolledWindow....
             * 
             * 
             ****************************************************************************/
            Gtk.VBox side_pane_vbox = new Gtk.VBox (false, 0);
            _hpaned.add1 (side_pane_vbox);
            
            Gtk.ScrolledWindow scrolled_window = new Gtk.ScrolledWindow (null, null);
            scrolled_window.set_policy (Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC);
            side_pane_vbox.pack_start (scrolled_window, true, true, 0);
            
            
            /*****************************************************************************
             * Create the DirTreeView add it to the ScrolledWindow...
             * 
             * 
             ****************************************************************************/
            _tree_view = new Fm.DirTreeView ();
            scrolled_window.add (_tree_view);
            
            
            // Fill The TreeView Model...
            _dir_tree_model = new Fm.DirTreeModel ();
            
//~             _dir_tree_model.set_show_hidden (true);
//~             _dir_tree_model.set_show_symlinks (true);
            
//~             _dir_tree_model.load ();
            
            _dir_tree_model.load_testing ();
            
            
            // The model is loaded, attach a view to it and connect signals...
            _tree_view.set_model (_dir_tree_model);
            _tree_view.directory_changed.connect (_tree_view_on_change_directory);
            _tree_view.button_release_event.connect (_tree_view_on_button_release);
            


            /*****************************************************************************
             * Create the ViewContainer Notebook, connect the signals and add it to
             * the Gtk.Paned...
             * 
             * 
             ****************************************************************************/
            
            _folder_view = new Fm.FolderView (Fm.FolderViewMode.LIST_VIEW);      
            
            _folder_view.small_icon_size =  16;
            _folder_view.big_icon_size =    36;
            _folder_view.single_click =     false;
            
            _folder_view.set_show_hidden (true);
            _folder_view.sort (Gtk.SortType.ASCENDING, Fm.FileColumn.NAME);
            _folder_view.set_selection_mode (Gtk.SelectionMode.MULTIPLE);
            
            _folder_view.clicked.connect (_folder_view_on_file_clicked);
            _hpaned.add2 (_folder_view);
            
            
            /*****************************************************************************
             * Add The Container To The Main Window...
             * 
             * 
             ****************************************************************************/
            this.add (main_vbox);
            
            _folder_view.grab_focus ();
            
            this._change_directory (_current_dir);

            this.show_all ();

            return true;
        }
        
        
        private void _tree_view_on_change_directory (uint button, Fm.Path path) {
        
            /*** NO_DEBUG ("_tree_view_on_change_directory: %u, %s\n", button, path.to_str ()); ***/
            
            this._change_directory (path, DirChangeCaller.DIR_TREEVIEW, false);
        }

        /*********************************************************************************
         * 
         * 
         * 
         ********************************************************************************/
        private void _change_directory (Fm.Path path, 
                                        DirChangeCaller caller = DirChangeCaller.NONE,
                                        bool save_history = false) {

            // NO_DEBUG ("Change Directory: %s\n", path.to_str ());
            
            if (caller != DirChangeCaller.DIR_TREEVIEW)
                _tree_view.set_current_directory (path);
            
            if (caller != DirChangeCaller.FOLDER_VIEW)                
                _folder_view.chdir (path);
        }
        
        private bool _tree_view_on_button_release (Gdk.EventButton event) {
        
            /*** stdout.printf ("_tree_view_on_button_release\n"); ***/ 
            
            if (event.button != 3)
                return false;
            
            Gtk.TreePath path;
            
            if (!_tree_view.get_path_at_pos ((int) event.x, (int) event.y, out path, null, null, null))
                return true;
            
            //~ select = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
            //~ gtk_tree_selection_unselect_all (select);
            //~ gtk_tree_selection_select_path (select, path);
            //~ gtk_tree_path_free (path);
            
            Gtk.TreeSelection sel = _tree_view.get_selection ();
            List<Gtk.TreePath>? sels = sel.get_selected_rows (null);
            if (sels == null)
                return true;
                
            Gtk.TreeIter it;
            if (!_dir_tree_model.get_iter (out it, sels.data))
                return true;
            
            // Get The Selected File...
            unowned Fm.FileInfo? file_info;
            _dir_tree_model.get (it, 2, out file_info, -1);
            if (file_info == null)
                return true;
                
            // Create A FileInfoList Containing The Selected File...
            Fm.FileInfoList<Fm.FileInfo> files = new Fm.FileInfoList<Fm.FileInfo> ();
            files.push_tail (file_info);
            
            _file_popup = _file_menu_get_menu ((Gtk.Widget) this, _tree_view.get_current_directory (), files);
            
            if (_file_popup != null)
                _file_popup.popup (null, null, null, 3, Gtk.get_current_event_time ());
            
            return true;
        }
        
        private void _folder_view_on_file_clicked (Fm.FolderViewClickType type, Fm.FileInfo? file_info) {

            switch (type) {
                
                // Double click on an item in the Folder View...
                case Fm.FolderViewClickType.ACTIVATED: {
                    
                    if (file_info == null || file_info.get_path ().is_trash ())
                        return;
                    
                    string? target = file_info.get_target ();
                    
                    // A directory...
                    if (file_info.is_dir ()) {
                        
                        this._change_directory (file_info.get_path (), DirChangeCaller.FOLDER_VIEW);
                             
                    } else if (file_info.is_mountable ()) {
                        
                        if (target == null) {
                        
                            DEBUG ("TODO: mount items on the fly...\n");
                            
                        } else {
                        
                            Fm.Path path = new Fm.Path.for_str (target);
                            this._change_directory (path, DirChangeCaller.FOLDER_VIEW);
                        }
                    
                    } else {
                        
                        Fm.launch_file (this, null, file_info, null);
                    }
                }
                break;
                
                case Fm.FolderViewClickType.CONTEXT_MENU: {
                    
                    // File/Folder Popup Menu...
                    if (file_info != null) {
                        
                        Fm.FileInfoList<Fm.FileInfo>? files = _folder_view.get_selected_files ();
                        if (files == null)
                            return;
            
                        _file_popup = _file_menu_get_menu ((Gtk.Widget) this, _folder_view.get_cwd (), files);
                        
                        if (_file_popup != null)
                            _file_popup.popup (null, null, null, 3, Gtk.get_current_event_time ());
                    }
                }
                break;
            }
        }
        
        private Gtk.Menu _file_menu_get_menu (Gtk.Widget owner,
                                  Fm.Path destination,
                                  Fm.FileInfoList<Fm.FileInfo>? file_info_list) {
            
            //_owner_widget = owner;
            
            // Create The Popup Menu.
            _fm_file_menu = new Fm.FileMenu.for_files ((Gtk.Window) owner,
                                                       file_info_list,
                                                       destination, false);
            
            Gtk.ActionGroup action_group = _fm_file_menu.get_action_group ();
            action_group.set_translation_domain ("");
            
            _fm_file_menu.set_folder_func (this._open_folder_func);
            
            return _fm_file_menu.get_menu ();
        }
        
        public bool _open_folder_func (GLib.AppLaunchContext ctx, GLib.List<Fm.FileInfo>? folder_infos,
                                             void *user_data) {
            
            unowned List<Fm.FileInfo>? folder_list = (GLib.List<Fm.FileInfo>) folder_infos;
            
            foreach (Fm.FileInfo file_info in folder_list) {
                
                /**
                string[] folders = new string [1];
                folders[0] = file_info.get_path ().to_str ();
                global_app.new_manager_tab (folders);
                **/
            }
            return true;
        }
    }
}



