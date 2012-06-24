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
        

        private Fm.DirTreeModel?    global_dir_tree_model = null;
        
        
        private Fm.Path                 _current_dir;
        
        // UI widgets...
        private Gtk.HPaned              _hpaned;
        private Fm.DirTreeView          _tree_view;
        private Fm.FolderView           _folder_view;
        
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
            if (global_dir_tree_model == null) {

                global_dir_tree_model = new Fm.DirTreeModel ();
                global_dir_tree_model.set_show_hidden (true);
                
                Fm.FileInfoJob job = new Fm.FileInfoJob (null, Fm.FileInfoJobFlags.NONE);
                
                unowned List<Fm.FileInfo>? l;
                
                
                /*************************************************************************
                 * Add TreeView Root Items....
                 * 
                 * 
                 ************************************************************************/
                // Desktop...
                job.add (Fm.Path.get_desktop ());
                
                // Documents...
                Fm.Path path = new Fm.Path.for_str (Environment.get_user_special_dir (UserDirectory.DOCUMENTS));
                job.add (path);
                
                // Computer...
                path = new Fm.Path.for_uri ("computer:///");
                job.add (path);
                
                // Trash Can...
                job.add (Fm.Path.get_trash ());
                
                // Root FileSystem...
                job.add (Fm.Path.get_root ());
                
                // Administration Programs...
                job.add (new Fm.Path.for_uri ("menu://applications/system/Administration"));
                
                job.run_sync_with_mainloop ();

                Fm.FileInfoList file_infos = job.get_list ();
                
                unowned List<Fm.FileInfo>? list = (List<Fm.FileInfo>) ((Queue) file_infos).head;
                
                for (l = list; l != null; l = l.next) {
                    
                    Fm.FileInfo? fi = (Fm.FileInfo) l.data;
                    
                    //bool expand = (fi.get_path ().is_virtual () == false);
                    bool expand = true;
                    if (fi.get_path ().is_virtual ()) {
                        expand = false;
                    }
                    
                    global_dir_tree_model.add_root (fi, null, expand);
                }
            }
            
            
            // The model is loaded, attach a view to it and connect signals...
            _tree_view.set_model (global_dir_tree_model);
            //_tree_view.directory_changed.connect (_tree_view_on_change_directory);
            //_tree_view.button_release_event.connect (_tree_view_on_button_release);
            


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
            
            
            _folder_view.chdir (_current_dir);
            //_folder_view.grab_focus ();
            //_folder_view.show_all ();
            
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
        
        
        /*********************************************************************************
         * 
         * 
         * 
         ********************************************************************************/
        private void _change_directory (Fm.Path path, 
                                        DirChangeCaller caller = DirChangeCaller.NONE,
                                        bool save_history = false) {

            if (caller != DirChangeCaller.DIR_TREEVIEW)
                _tree_view.set_current_directory (path);
            
            if (caller != DirChangeCaller.FOLDER_VIEW)                
                _folder_view.chdir (path);
        }
    }
}



