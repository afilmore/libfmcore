//      trash-job.vala
//      
//      Copyright 2011 Hong Jen Yee  (PCMan) <pcman.tw@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//       (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.
//      
//      

namespace Fm {

    public class TrashJob : FileJob {

        PathList? failed_paths;

        public TrashJob (PathList paths, FileJobUI? ui) {
            base (ui);
            _src_paths = paths;
            unowned string title = _("Trashing files");
            ui.init_with_job (this, title, title, false);
        }

        private bool trash_file (File file, GLib.FileInfo info) {
            bool ret = false;
            set_currently_processed (file, info, null);
            update_progress_display ();

            // Thread.usleep (2000); // delay for ease of debugging
            try {
                file.trash (cancellable);
                ret = true;
                ++_n_processed_files;
                _processed_size += get_file_size (info);
            }
            catch (Error err) {
                if (handle_error (err) == ErrorAction.ABORT)
                    return false;
            }

            // calculate percent;
            double fraction =  (double) (_n_processed_files + _n_processed_dirs) /  (_n_total_dirs + _n_total_files);
            set_percent (fraction);
            update_progress_display ();

            return ret;
        }

        protected override bool run () {

            // show the first file in the UI
            set_current_src_dest (_src_paths.peek_head (), null);
            update_progress_display ();

            // calculate total amount of work for progress display
            // if (calculate_total () == false)
                // return false;
            // GLib.File.trash does not support progress display,
            // so we can just count file numbers and use it as total amount
            _n_total_files = _src_paths.get_length ();

            // inform the UI that we're ready
            set_ready ();
            stdout.printf ("total: %llu, %d, %d\n", _total_size, _n_total_files, _n_total_dirs);

            // trash all source files one by one
            foreach (unowned Path src_path in _src_paths.peek_head_link ()) {
                
                File file = src_path.to_gfile ();
                
                try {
                    
                    GLib.FileInfo info = file.query_info (_file_attributes, FileQueryInfoFlags.NOFOLLOW_SYMLINKS, cancellable);
                    // show currently processed file in UI
                    set_current_src_dest (src_path, null);
                    set_currently_processed (file, info, null);
                    update_progress_display ();

                    trash_file (file, info);
                
                } catch (Error err) {
                    if (failed_paths == null)
                        failed_paths = new PathList ();
                    failed_paths.push_tail (src_path);
                }
            }

            return true;
        }

        public unowned PathList? get_failed_paths () {
            return failed_paths;
        }
    }
}



