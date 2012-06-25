/***********************************************************************************************************************
 * 
 *      FileJob.vala
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

    
    public abstract class FileJob : Job {

        [Flags]
        private enum UpdateFlags {
            CURRENT_SRC_DEST,
            CURRENTLY_PROCESSED,
            PERCENT,
            TIME,
            ALL_TEXT =  (CURRENT_SRC_DEST | CURRENTLY_PROCESSED | TIME)
        }

        protected FileJobUI?            _file_job_ui;
        
        protected PathList?             _src_paths;              // source file paths
        
        private Timer?                  _timer;
        
        protected unowned Path          _current_src_path;
        protected File?                 _current_src_file;       // current source file being processed
        protected GLib.FileInfo?        _current_src_info;

        protected unowned Path          _current_dest_path;
        protected File?                 _current_dest_file;      // current destination file being processed
        
        protected uint64                _total_size;                     // total size of source file
        protected uint64                _processed_size;                 // currently processed size
        protected uint64                _current_file_size;              // size of current file being processed
        protected uint64                _current_file_processed_size;    // size processed of current file
        
        protected int                   _n_total_files;                  // total number of files
        protected int                   _n_total_dirs;                   // total number of dirs
        protected int                   _n_processed_files;              // number of processed files
        protected int                   _n_processed_dirs;               // number of processed dirs
        
        protected int                   _percent;                // percent  (0-100), for progress bar display
        protected double                _finished_fraction;      //  (0.0 - 1.0)
        
        private double                  _last_elapsed;
        private uint                    _remaining_time;
        private UpdateFlags             _update_flags;

        
        // FIXME: different job should requires different attributes
        protected static unowned string _file_attributes = 
            "standard::type,standard::size,standard::allocated-size,standard::name,standard::display-name,standard::symlink-target,unix::*,id::*";

        protected const uint64          _DEFAULT_PROCESSED_AMOUNT = 4096;

        
        public FileJob (FileJobUI? ui = null) {
            this._file_job_ui = ui;
        }

        ~FileJob () {
            NO_DEBUG ("job is deleted!!!\n");
        }

        public override void dispose () {
            _file_job_ui = null;
        }

        public unowned FileJobUI get_ui () {
            return _file_job_ui;
        }

        protected RenameResult ask_rename (File src_file, File dest_file, out File new_dest) {
            
            _timer.stop ();
            
            RenameResult ret = RenameResult.CANCEL;
            File out_new_dest = null;
            
            if (_file_job_ui != null) {
                
                try {
                    
                    // get file infos for display in the rename dialog
                    
                    GLib.FileInfo src_info = src_file.query_info ("standard::*,time::*", FileQueryInfoFlags.NOFOLLOW_SYMLINKS, cancellable);
                    
                    GLib.FileInfo dest_info = dest_file.query_info ("standard::*,time::*", FileQueryInfoFlags.NOFOLLOW_SYMLINKS, cancellable);

                    // call the ui from mainloop to show the rename dialog
                    job.send_to_mainloop (() => {
                        ret = _file_job_ui.ask_rename (src_file, src_info, dest_file, dest_info, out out_new_dest);
                        return true;
                    });
                }
                catch (Error err) {
                    handle_error (err);
                }
            }
            
            new_dest = out_new_dest;
            
            _timer.continue ();
            
            return ret;
        }

        protected void show_error (string message, string? title = null, bool use_markup = true) {
            
            _timer.stop ();
            
            if (_file_job_ui != null) {
                job.send_to_mainloop (() => {
                    _file_job_ui.show_error (message, title, use_markup);
                    return true;
                });
            }
            
            _timer.continue ();
        }

        protected bool ask_ok_cancel (string question, string? title = null, bool use_markup = true) {
            
            _timer.stop ();
            bool ret = false;
            
            if (_file_job_ui != null) {
                ret = job.send_to_mainloop (() => {
                    return _file_job_ui.ask_ok_cancel (question, title, use_markup);
                });
            }
            
            _timer.@continue ();
            return ret;
        }

        protected bool ask_yes_no (string question, string? title = null, bool use_markup = true) {
            
            _timer.stop ();
            bool ret = false;
            
            if (_file_job_ui != null) {
                ret = job.send_to_mainloop (() => {
                    return _file_job_ui.ask_yes_no (question, title, use_markup);
                });
            }
            
            _timer.@continue ();
            return ret;
        }

        protected void set_current_src_dest (Path src_path, Path? dest_path) {
            
            _current_src_path = src_path;
            _current_dest_path = dest_path;
            
            _update_flags |= UpdateFlags.CURRENT_SRC_DEST;
        }

        protected void set_currently_processed (File src, GLib.FileInfo? src_info, File? dest) {
            
            _current_src_file = src;
            _current_src_info = src_info;
            _current_dest_file = dest;
            
            _update_flags |= UpdateFlags.CURRENTLY_PROCESSED;
        }

        protected void set_percent (double fraction) {
            
            _finished_fraction = fraction;
            fraction *= 100;
            
            int new_percent =  (int)fraction;
            
            if (new_percent > 100)
                new_percent = 100;
            
            if (new_percent != _percent) {
                _percent = new_percent;
                // NO_DEBUG ("%d, %d, percent: %d\n", n_processed_files, n_processed_dirs, _percent);
                _update_flags |= UpdateFlags.PERCENT;
            }
        }

        
        // Call UI to update current progress display, "if needed".
        // to improve performance, update_progress_display () check the timer
        // and only really update the progress display in UI when
        // elapsed time > 0.5 seconds or percent changes.
        // Everytime we ask the UI to update display, this requires
        // blocking and waiting for main thread to complete the painting.
        // Besides, showing filename in main thread takes much time due to 
        // pango. So updating the UI too often greatly decrease performance.
        // Everytime when the job has some progress, it calls update_progress_display ()
        // The function then check if enough time has passed and really 
        // update the UI display periodically at interval > 0.6 sec.
        protected void update_progress_display () {
            
            double elapsed = _timer.elapsed ();
            
            bool enough_time_elapsed = ((elapsed - _last_elapsed) > 0.6);
            bool need_update_text = false;
            
            if (enough_time_elapsed) {
                
                // estimate remaining time
                double total_time = elapsed / _finished_fraction;
                double remaining = total_time *  (1 - _finished_fraction);
                
                // FIXME: need to find out a better way to update estimated time
                _remaining_time = (uint) remaining;
                // NO_DEBUG ("remaining: %u, %lf, %lf, %lf\n", _remaining_time, total_time, remaining, _finished_fraction);
                
                _update_flags |= UpdateFlags.TIME;
                if ((_update_flags & UpdateFlags.ALL_TEXT) !=0)
                    need_update_text = true;
            }

            // NO_DEBUG ("update_progress: %d\n",  (_update_flags & UpdateFlags.CURRENT_SRC_DEST));
            if (need_update_text || (_update_flags & UpdateFlags.PERCENT) !=0) {
                
                if (_file_job_ui != null) {
                    
                    // call the UI to update display, "in main thread"
                    job.send_to_mainloop (() => {
                        
                        if (enough_time_elapsed) {
                            
                            if ((_update_flags & UpdateFlags.CURRENT_SRC_DEST) != 0) {
                                _file_job_ui.set_current_src_dest (_current_src_path, _current_dest_path);
                                _update_flags &= ~UpdateFlags.CURRENT_SRC_DEST;
                            }
                            
                            if ((_update_flags & UpdateFlags.CURRENTLY_PROCESSED) !=0) {
                                _file_job_ui.set_currently_processed (_current_src_file, _current_src_info, _current_dest_file);
                                _update_flags &= ~UpdateFlags.CURRENTLY_PROCESSED;
                            }
                            
                            if ((_update_flags & UpdateFlags.TIME) != 0) {
                                // set remaining time
                                _file_job_ui.set_times ((uint)elapsed, _remaining_time);
                                _update_flags &= ~UpdateFlags.TIME;
                            }
                        }
                        
                        if ((_update_flags & UpdateFlags.PERCENT) != 0) {
                            _file_job_ui.set_percent (_percent);
                            _update_flags &= ~UpdateFlags.PERCENT;
                        }
                        return true;
                    });
                }
                
                if (enough_time_elapsed)
                    _last_elapsed = elapsed;
            }
        }

        public override void run_async () {
            
            // this function is still called from main thread
            _timer = new Timer ();
            
            if (_file_job_ui != null)
                _file_job_ui.start (); // inform the ui that we started the job
            
            // chain up base to really launch the job thread
            base.run_async ();
        }

        protected inline static uint64 get_file_size (GLib.FileInfo info) {
            
            // prefer block_size x blocks, and fallback to size or DEFAULT_PROCESSED_AMOUNT
            uint64 size = info.get_attribute_uint32 ("unix::block_size") * info.get_attribute_uint64 ("unix::blocks");
            
            if (size == 0)
                size = info.get_size ();
            
            if (size == 0)
                size = _DEFAULT_PROCESSED_AMOUNT;
            
            return size;
        }

        // recursively calculate total size of a file/dir and its children
        protected uint64 calculate_total_for_file (File file, GLib.FileInfo info, out int n_dirs, out int n_files) {
            
            FileType type = info.get_file_type ();
            uint64 size = get_file_size (info);

            if (type == FileType.DIRECTORY) {
                
                try {
                    FileEnumerator enu = file.enumerate_children (_file_attributes, 0, cancellable);
                    
                    while (cancellable.is_cancelled () == false) {
                        
                        GLib.FileInfo child_info = enu.next_file (cancellable);
                        
                        if (child_info == null) // end of file list
                            break;
                        
                        File child = file.get_child (child_info.get_name ());
                        int child_n_dirs, child_n_files;
                        
                        uint64 child_size = calculate_total_for_file (child, child_info, out child_n_dirs, out child_n_files);
                        size += child_size;
                        
                        n_dirs += child_n_dirs;
                        n_files += child_n_files;
                    }
                    enu.close ();
                
                } catch (Error err) {
                }
                ++n_dirs;
            
            } else {
                ++n_files;
            }
            
            return size;
        }

        // calculate total amount of the job for progress display
        protected bool _calculate_total () {
            
            foreach (unowned Fm.Path src_path in _src_paths.peek_head_link ()) {
                
                File file = src_path.to_gfile ();
                
                try {
                    
                    GLib.FileInfo info = file.query_info (_file_attributes, 0, cancellable);
                    
                    int n_dirs, n_files;
                    
                    _total_size += calculate_total_for_file (file, info, out n_dirs, out n_files);
                    _n_total_dirs += n_dirs;
                    _n_total_files += n_files;
                
                } catch (Error err) {
                }
                
                if (cancellable.is_cancelled () == true)
                    return false;
            }
            return true;
        }

        protected void set_ready () {
            
            if (_file_job_ui != null) {
                
                job.send_to_mainloop (() => {
                    _file_job_ui.set_ready (); // inform the UI that we're ready to do the real work
                    return true;
                });
            }

            // now, the total amount of work is known
            // reset the timer for speed estimating speed
            _timer.start ();
        }

        protected ErrorAction handle_error (Error err, Severity severity = Severity.MODERATE) {
            
            _timer.stop ();
            
            ErrorAction ret = ErrorAction.CONTINUE;
            
            if (_file_job_ui != null) {
                
                job.send_to_mainloop (() => {
                    
                    // set src and dest paths prior to showing the error.
                    _file_job_ui.set_current_src_dest (_current_src_path, _current_dest_path);
                    _file_job_ui.set_currently_processed (_current_src_file, _current_src_info, _current_dest_file);

                    ret = _file_job_ui.handle_error (err, severity);
                    return true;
                });

                // if we need to abort
                if (ret == ErrorAction.ABORT || severity == Severity.CRITICAL)
                    cancel (); // cancel the job
            }
            
            _timer.continue ();
            return ret;
        }

        public override /*signal*/ void finished () {
            
            if (_file_job_ui != null)
                _file_job_ui.finish ();
        }
    }
}



