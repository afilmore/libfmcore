/***********************************************************************************************************************
 * 
 *      CopyJob.vala
 *
 *      Copyright 2011 Hong Jen Yee  (PCMan) <pcman.tw@gmail.com>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 *
 * 
 **********************************************************************************************************************/
namespace Fm {


    public enum CopyJobMode {
        COPY,
        MOVE,
        LINK,
        UNTRASH
    }


    public class CopyJob : Fm.FileJob {

        protected Fm.PathList?  _dest_path_list;
        
        private Fm.CopyJobMode  _copy_mode;
        
        
        public CopyJob (CopyJobMode mode, Fm.PathList src_paths, Fm.PathList? dest_paths, Fm.FileJobUI? ui = null) {
            
            base (ui);
            
            this._copy_mode = mode;

            this._src_paths =       src_paths;
            this._dest_path_list =  dest_paths;
            
            bool has_dest = true;
            
            if (ui != null) {
                
                unowned string title = null;
                
                switch (mode) {
                    
                    case CopyJobMode.COPY:
                        title = _("Copying files");
                    break;
                    
                    case CopyJobMode.MOVE:
                        title = _("Moving files");
                    break;
                    
                    case CopyJobMode.LINK:
                        title = _("Creating symlinks");
                    break;
                    
                    case CopyJobMode.UNTRASH:
                        title = _("Restore files");
                    break;
                }
                
                ui.init_with_job (this, title, title, has_dest);
            }
        }

        protected override bool run () {
            
            stdout.printf  ("FmCopyJob.run\n");
            
            if (this._calculate_total () == false || this._ensure_dest () == false) // calculate total amount of work
                return false;

            set_ready (); // tell the UI that we're ready
            
            stdout.printf ("total: %llu, %d, %d\n", _total_size, _n_total_files, _n_total_dirs);

            // ready to copy/move files
            unowned GLib.List<Path> dest_l = _dest_path_list.peek_head_link ();
            
            foreach (unowned Fm.Path src_path in _src_paths.peek_head_link ()) {
                
                if (is_cancelled ())
                    break;
                
                File src_file = src_path.to_gfile ();
                
                unowned Fm.Path dest_path = dest_l.data;
                File dest_file = dest_path.to_gfile ();
                
                stdout.printf ("CopyJobMode = %d: %s > %s\n", _copy_mode, src_path.to_str (), dest_path.to_str ());
                
                try {
                    
                    // query info of source file and update progress display
                    GLib.FileInfo src_info = src_file.query_info (_file_attributes,
                                                                  FileQueryInfoFlags.NOFOLLOW_SYMLINKS, cancellable);
                    
                    set_current_src_dest (src_path, dest_path);
                    
                    set_currently_processed (src_file, src_info, dest_file);
                    
                    update_progress_display ();

                    
                    
                    // check if the operation is valid. for example, one cannot
                    // move a folder into itself.
                    this._check_paths (src_file, src_info, dest_file);

                    
                    
                    
                    // maybe we don't need to call update_progress_display () here 
                    // since it will soon be called inside copy_file ()/move_file ().
                    switch (_copy_mode) {
                        
                        case CopyJobMode.COPY:
                            this._copy_file (src_file, src_info, dest_file);
                        break;
                        
                        case CopyJobMode.MOVE: {
                            
                            File dest_dir = dest_file.get_parent ();
                            GLib.FileInfo dest_dir_info = dest_dir.query_info ("id::filesystem",
                                                                               FileQueryInfoFlags.NONE, cancellable);
                            
                            string src_fs = src_info.get_attribute_string ("id::filesystem");
                            string dest_fs = dest_dir_info.get_attribute_string ("id::filesystem");
                            
                            if (src_fs == dest_fs) // on the same filesystem
                                this._move_file (src_file, src_info, dest_file);
                            else // cross-device move = copy + delete source
                                this._copy_file (src_file, src_info, dest_file);
                        
                        }
                        break;
                        
                        case CopyJobMode.LINK:
                            this._link_file (src_file, src_info, dest_file);
                        break;
                        
                        case CopyJobMode.UNTRASH:
                            this._move_file (src_file, src_info, dest_file);
                        break;
                    }
                
                } catch (Error err) {

                    if (handle_error (err, Severity.MODERATE) == ErrorAction.ABORT)
                        return false;
                }

                // emit a fake notification signal for file creation for
                // filesystems which don't have file monitor support.
                File dest_dir = dest_file.get_parent (); // get parent folder of dest file
                
                FileMonitor dest_mon = monitor_lookup_dummy_monitor (dest_dir);
                
                if (dest_mon != null)
                    dest_mon.changed (dest_file, null, FileMonitorEvent.CREATED);

                dest_l = dest_l.next; // get next dest path
            }

            return true;
        }

        // ensure that we have proper dest paths
        private bool _ensure_dest () {
            
            // FIXME: handle cancellable here
            switch (_copy_mode) {
                
                case CopyJobMode.UNTRASH: {
                    
                    _dest_path_list = new PathList ();
                    
                    // get original paths of the trashed files
                    foreach  (unowned Fm.Path path in _src_paths.peek_head_link ()) {
                        
                        if (is_cancelled ())
                            break;
                        
                        File file = path.to_gfile ();
                        
                        try {
                            
                            GLib.FileInfo info = file.query_info ("trash::*", 0, null);
                            
                            unowned string dest_path_str = info.get_attribute_byte_string ("trash::orig-path");
                            
                            Fm.Path dest_path = new Fm.Path.for_str (dest_path_str);
                            
                            _dest_path_list.push_tail (dest_path);
                        }
                        catch (Error err) {
                            
                            // TODO_axl: emit the error properly...
                            stdout.printf  ("Fm.CopyJob: error %s\n", err.message);
                            return false;
                        }
                    }
                }
                break;
            }
            return true;
        }
        
        private bool _check_paths (File src_file, GLib.FileInfo src_info, File dest_file) throws IOError {
            
            IOError err = null;
            
            if (_copy_mode == CopyJobMode.MOVE && src_file.equal (dest_file)) {
                
                err = new IOError.FAILED (_("Source and destination are the same."));
            
            } else if (_copy_mode == CopyJobMode.LINK) {
                
                // what to test ???
                
            } else if (src_info.get_file_type () == FileType.DIRECTORY && dest_file.has_prefix (src_file) ) {
                
                unowned string? msg = null;
                
                if (_copy_mode == CopyJobMode.MOVE)
                    msg = _("Cannot move a folder into its sub folder");
                else if (_copy_mode == CopyJobMode.COPY)
                    msg = _("Cannot copy a folder into its sub folder");
                else
                    msg = _("Destination is a sub folder of source");
                
                err = new IOError.FAILED (msg);
            }
            
            if (err != null)
                throw err;
            
            return  (err == null);
        }

        
        private bool _copy_dir (File src_file, GLib.FileInfo src_info, File _dest_file) {
            
            bool retry_mkdir = false;
            File dest_file = _dest_file;
            
            do {
                
                try {
                    // create the dir
                    dest_file.make_directory (cancellable);
                    retry_mkdir = false;
                
                } catch (Error err) {
                    
                    if (err is IOError.EXISTS) { // destination file already exists
                        File new_dest;
                        
                        switch (ask_rename (src_file, dest_file, out new_dest)) {
                            case RenameResult.RENAME:
                                dest_file = new_dest;
                                retry_mkdir = true;
                            break;
                            
                            case RenameResult.OVERWRITE:
                            break;
                            
                            case RenameResult.SKIP:
                                // skip this dir
                                // FIXME: add total size of the dir to processed_size
                            break;
                            
                            case RenameResult.CANCEL:
                                cancel (); // cancel the job
                            return false;
                        }
                    
                    } else if (err is IOError.CANCELLED) {
                        return false; // the job is cancelled
                    
                    } else { // other errors
                        // FIXME: show error to the user
                        return false;
                    }
                }
            
            } while (retry_mkdir == true);

            uint32 unix_mode = src_info.get_attribute_uint32 ("unix::mode");
            
            // ensure that we have rw permission to this file.
            unix_mode |= (Posix.S_IRUSR|Posix.S_IWUSR);
            
            try {
                // set the file attributes and ensure that it's writable
                dest_file.set_attribute_uint32 ("unix::mode", unix_mode,
                                                FileQueryInfoFlags.NOFOLLOW_SYMLINKS, cancellable);

                // copy files in the directory recursively
                FileEnumerator enu = src_file.enumerate_children (_file_attributes, 0, cancellable);
                
                while (!cancellable.is_cancelled ()) {
                    
                    try {
                        GLib.FileInfo child_info = enu.next_file (cancellable);
                        
                        if (child_info == null) // end of list
                            break;
                        
                        File child_src_file = src_file.get_child (child_info.get_name ());
                        File child_dest_file = dest_file.get_child (child_info.get_name ());
                        
                        if (this._copy_file (child_src_file, child_info, child_dest_file)) {
                        
                        } else {
                        
                        }
                    
                    } catch (Error err) {
                        stdout.printf ("error: %s\n", err.message);
                    }
                }
                
                enu.close ();
                ++_n_processed_dirs;

                // if this is actually a cross-device move operation, delete the source file.
                if (_copy_mode == CopyJobMode.MOVE)
                    src_file.delete (cancellable);
            
            } catch (Error err) {
                // FIXME: report the error to the user
                stdout.printf ("error: %s\n", err.message);
            }

            return true;
        }

        private bool _copy_file (File src_file, GLib.FileInfo src_info, File _dest_file) {
            
            bool ret = false;
            File dest_file = _dest_file;
            
            // stdout.printf ("%s -> %s\n", src_file.get_parse_name (), dest_file.get_parse_name ());
            set_currently_processed (src_file, src_info, dest_file);
            update_progress_display ();
            
            // Thread.usleep (2000); // delay for ease of debugging

            FileType type = src_info.get_file_type ();
            
            if (type == FileType.DIRECTORY) {
                ret = this._copy_dir (src_file, src_info, dest_file);
            
            } else {
                
                bool retry_copy = false;
                FileCopyFlags flags = FileCopyFlags.ALL_METADATA|FileCopyFlags.NOFOLLOW_SYMLINKS;
                
                do {
                    
                    try {
                        switch (type) {
                            
                            case FileType.SPECIAL:
                                // handle special files
                                ret = this._copy_special_file (src_file, src_info, dest_file);
                            break;
                            
                            default:

                                ret = src_file.copy (dest_file, flags, cancellable, this._copy_progress_cb);

                                // if this is a cross-device move, delete the source file
                                if (ret == true && _copy_mode == CopyJobMode.MOVE)
                                    src_file.delete (cancellable);

                            break;
                        }
                        
                        retry_copy = false;
                        ++_n_processed_files;
                        _processed_size += get_file_size (src_info);
                    
                    } catch (Error err) {
                        
                        if (err is IOError.EXISTS) { // destination file already exists
                            
                            flags &= ~FileCopyFlags.OVERWRITE; // clear overwrite flag
                            
                            File new_dest;
                            switch (ask_rename (src_file, dest_file, out new_dest)) { // ask for rename
                                
                                case RenameResult.RENAME:
                                    dest_file = new_dest;
                                    retry_copy = true;
                                break;
                                
                                case RenameResult.OVERWRITE: // overwrite existing file
                                    flags |= FileCopyFlags.OVERWRITE;
                                    retry_copy = true;
                                break;
                                
                                case RenameResult.SKIP: // skip the file
                                    // retry_copy = false;
                                    _processed_size += get_file_size (src_info);
                                break;
                                
                                case RenameResult.CANCEL: // cancel the job
                                    cancel ();
                                return false;
                            }
                        
                        } else {
                            // present the error to the user
                            if (handle_error (err, Severity.MODERATE) == ErrorAction.ABORT) {
                                return false;
                            }
                        }
                    }
                
                } while (retry_copy == true);
            }

            // calculate percent;
            double fraction =  (double)_processed_size / _total_size;
            set_percent (fraction);
            update_progress_display ();

            return ret;
        }

        private bool _copy_special_file (File src_file, GLib.FileInfo src_info, File dest_file) throws IOError {
            
            bool ret = false;
            
            // only handle FIFO for local files
            if (src_file.is_native () && dest_file.is_native ()) {
                
                string src_path = src_file.get_path ();
                Posix.Stat src_st;
                int r = Posix.lstat (src_path, out src_st);
                
                if (r == 0) {
                    // Handle FIFO on native file systems.
                    if (Posix.S_ISFIFO (src_st.st_mode)) {
                        
                        string dest_path = dest_file.get_path ();
                        r = Posix.mkfifo (dest_path, src_st.st_mode);
                        
                        if ( r == 0) {
                            ret = true;
                        } else {
                            // g_io_error_from_errno (errno);
                            // FIXME: Vala bug: g_io_error_from_errno doesn't work.
                            throw new IOError.FAILED (strerror (errno));
                        }
                    
                    } else {
                        // FIXME: how about blcok device, char device, and socket?
                        // FIXME: add proper error message
                        throw new IOError.NOT_SUPPORTED ("");
                    }
                
                } else {
                    // FIXME: error handling
                    // g_io_error_from_errno (errno);
                    // FIXME: Vala bug: g_io_error_from_errno doesn't work.
                    throw new IOError.FAILED (strerror (errno));
                }
            
            } else {
                // FIXME: add proper error message
                throw new IOError.NOT_SUPPORTED ("");
            }
            return ret;
        }

        private bool _move_file (File src_file, GLib.FileInfo src_info, File _dest_file) {
            
            bool ret = false;
            File dest_file = _dest_file;
            stdout.printf ("move_file %s -> %s\n", src_file.get_parse_name (), dest_file.get_parse_name ());

            set_currently_processed (src_file, src_info, dest_file);
            update_progress_display ();

            // Thread.usleep (2000); // delay for ease of debugging
            bool retry_move = false;
            
            FileCopyFlags flags = FileCopyFlags.ALL_METADATA | FileCopyFlags.NOFOLLOW_SYMLINKS;
            
            do {
                try {
                    FileType type = src_info.get_file_type ();
                    
                    ret = src_file.move (dest_file, flags, cancellable, this._copy_progress_cb);
                    if (type == FileType.DIRECTORY)
                        ++_n_processed_files;
                    else
                        ++_n_processed_files;
                    
                    retry_move = false;
                    _processed_size += get_file_size (src_info);
                
                } catch (Error err) {
                    
                    if (err is IOError.EXISTS) { // destination file already exists
                        flags &= ~FileCopyFlags.OVERWRITE; // clear overwrite flag
                        File new_dest;
                        
                        switch (ask_rename (src_file, dest_file, out new_dest)) { // ask for rename
                            
                            case RenameResult.RENAME:
                                dest_file = new_dest;
                                retry_move = true;
                            break;
                            
                            case RenameResult.OVERWRITE: // overwrite existing file
                                flags |= FileCopyFlags.OVERWRITE;
                                retry_move = true;
                            break;
                            
                            case RenameResult.SKIP: // skip the file
                                // retry_move = false;
                                _processed_size += get_file_size (src_info);
                            break;
                            
                            case RenameResult.CANCEL: // cancel the job
                                cancel ();
                            return false;
                        }
                    
                    } else {
                        
                        // present the error to the user
                        if (handle_error (err, Severity.MODERATE) == ErrorAction.ABORT)
                            return false;
                    }
                    // _processed_size += ...;
                }
            
            } while (retry_move == true);

            // calculate percent;
            double fraction =  (double)_processed_size / _total_size;
            set_percent (fraction);
            update_progress_display ();

            return ret;
        }

        private bool _link_file (File src_file, GLib.FileInfo src_info, File _dest_file) {
            
            stdout.printf ("_link_file %s -> %s\n", src_file.get_parse_name (), _dest_file.get_parse_name ());
            
            _dest_file.make_symbolic_link (src_file.get_parse_name (), null);
            
            return false;
        }

        // calculate total amount of the job for progress display
        protected new bool _calculate_total () {
            
            bool ret = true;
            
            // calculate total size & file numbers
            switch (_copy_mode) {
                
                case CopyJobMode.COPY:
                    // default generic function provided by FileJob is suitable
                    ret = base._calculate_total ();
                break;
                
                case CopyJobMode.MOVE:
                    
                    // move operations is unique and works quite differently if
                    // different filesystems/devices are involved.
                    unowned GLib.List<Path> dest_l = _dest_path_list.peek_head_link ();
                    
                    foreach (unowned Path src_path in _src_paths.peek_head_link ()) {
                        
                        File src_file = src_path.to_gfile ();
                        
                        unowned Path dest_path = dest_l.data;
                        
                        File dest_dir = dest_path.get_parent ().to_gfile (); // FIXME: get_parent () might return null?
                        
                        try {
                            GLib.FileInfo src_info = src_file.query_info (_file_attributes,
                                                                          FileQueryInfoFlags.NOFOLLOW_SYMLINKS,
                                                                          cancellable);
                            
                            // NOTE: we cannot use FileQueryInfoFlags.NOFOLLOW_SYMLINKS here for dest dir
                            
                            GLib.FileInfo dest_dir_info = dest_dir.query_info ("id::filesystem",
                                                                               FileQueryInfoFlags.NONE,
                                                                               cancellable);
                            unowned string src_fs = src_info.get_attribute_string ("id::filesystem");
                            unowned string  dest_fs = dest_dir_info.get_attribute_string ("id::filesystem");

                            if (src_fs == dest_fs) { // on the same filesystem
                                
                                // _total_size += get_file_size (src_info);
                                // time taken by rename () is not related to file size.
                                _total_size += _DEFAULT_PROCESSED_AMOUNT;
                                
                                if (src_info.get_file_type () == FileType.DIRECTORY)
                                    ++_n_total_dirs;
                                else
                                    ++_n_total_files;
                            
                            } else { // not on the same filesystem
                                
                                // treat as copy
                                int n_dirs, n_files;
                                _total_size += calculate_total_for_file (src_file, src_info, out n_dirs, out n_files);
                                _n_total_dirs += n_dirs;
                                _n_total_files += n_files;
                            }
                        }
                        catch (Error err) {
                        }
                        
                        if (cancellable.is_cancelled () == true) {
                            ret = false;
                            break;
                        }
                        dest_l = dest_l.next;
                    }
                break;
                
                //case CopyJobMode.LINK:
                case CopyJobMode.UNTRASH:
                    
                    // create symlinks for every source file
                    foreach (unowned Path src_path in _src_paths.peek_head_link ()) {
                        
                        File file = src_path.to_gfile ();
                        
                        try {
                        
                            GLib.FileInfo info = file.query_info (_file_attributes,
                                                                  FileQueryInfoFlags.NOFOLLOW_SYMLINKS, cancellable);
                            _total_size += get_file_size (info);
                        
                            if (info.get_file_type () == FileType.DIRECTORY)
                                ++_n_total_dirs;
                            else
                                ++_n_total_files;
                        }
                        catch (Error err) {
                        }
                        
                        if (cancellable.is_cancelled () == true) {
                            ret = false;
                            break;
                        }
                    }
                break;
            }
            return ret;
        }
        
        private void _copy_progress_cb (int64 current_num_bytes, int64 total_num_bytes) {
            
            // calculate percent;
            double fraction =  (double) (_processed_size + current_num_bytes) / _total_size;
            
            set_percent (fraction);
            
            update_progress_display ();
        }
    }
}



