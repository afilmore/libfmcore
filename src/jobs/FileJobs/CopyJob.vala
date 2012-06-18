//      copy-job.vala
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

public enum CopyJobMode {
	COPY, // copy files
	MOVE, // move files
	LINK, // create symlinks
	UNTRASH // restore trashed files
}

public class CopyJob : FileJob {

	public CopyJob (CopyJobMode mode,
					 PathList src_paths,
					 PathList? dest_paths,
					 FileJobUI? ui = null) {
		base (ui);
		this.mode = mode;
		this.src_paths = src_paths;
		this.dest_paths = dest_paths;

		bool has_dest = true;
		if (ui != null) {
			unowned string title = null;
			switch (mode) {
			case CopyJobMode.COPY:
				title = _ ("Copying files");
				break;
			case CopyJobMode.MOVE:
				title = _ ("Moving files");
				break;
			case CopyJobMode.LINK:
				title = _ ("Creating symlinks");
				break;
			case CopyJobMode.UNTRASH:
				// FIXME: how should I name this?
				title = _ ("Moving files");
				break;
			}
			ui.init_with_job (this, title, title, has_dest);
		}
	}


	private bool check_paths (File src_file, GLib.FileInfo src_info, File dest_file) throws IOError {
		IOError err = null;
		if (mode == CopyJobMode.MOVE && src_file.equal (dest_file))
			err = new IOError.FAILED (_ ("Source and destination are the same."));
		else if (src_info.get_file_type () == FileType.DIRECTORY
				&& dest_file.has_prefix (src_file) ) {
			unowned string? msg = null;
			if (mode == CopyJobMode.MOVE)
				msg = _ ("Cannot move a folder into its sub folder");
			else if (mode == CopyJobMode.COPY)
				msg = _ ("Cannot copy a folder into its sub folder");
			else
				msg = _ ("Destination is a sub folder of source");
			err = new IOError.FAILED (msg);
		}
		if (err != null)
			throw err;
		return  (err == null);
	}

	// calculate total amount of the job for progress display
	protected new bool calculate_total () {
		bool ret = true;
		// calculate total size & file numbers
		switch (mode) {
		case CopyJobMode.COPY:
			// default generic function provided by FileJob is suitable
			ret = base.calculate_total ();
			break;
		case CopyJobMode.MOVE:
			
            // move operations is unique and works quite differently if
			// different filesystems/devices are involved.
			unowned GLib.List<Path> dest_l = dest_paths.peek_head_link ();
			
            foreach (unowned Path src_path in src_paths.peek_head_link ()) {
				
                File src_file = src_path.to_gfile ();
				
                unowned Path dest_path = dest_l.data;
				
                File dest_dir = dest_path.get_parent ().to_gfile (); // FIXME: get_parent () might return null?
				
                try {
					GLib.FileInfo src_info = src_file.query_info (file_attributes, FileQueryInfoFlags.NOFOLLOW_SYMLINKS, cancellable);
					
                    // NOTE: we cannot use FileQueryInfoFlags.NOFOLLOW_SYMLINKS here for dest dir
					
                    GLib.FileInfo dest_dir_info = dest_dir.query_info ("id::filesystem", FileQueryInfoFlags.NONE, cancellable);
					unowned string src_fs = src_info.get_attribute_string ("id::filesystem");
					unowned string  dest_fs = dest_dir_info.get_attribute_string ("id::filesystem");

					if (src_fs == dest_fs) { // on the same filesystem
						// total_size += get_file_size (src_info);
						// time taken by rename () is not related to file size.
						total_size += DEFAULT_PROCESSED_AMOUNT;
						if (src_info.get_file_type () == FileType.DIRECTORY)
							++n_total_dirs;
						else
							++n_total_files;
					}
					else { // not on the same filesystem
						// treat as copy
						int n_dirs, n_files;
						total_size += calculate_total_for_file (src_file, src_info, out n_dirs, out n_files);
						n_total_dirs += n_dirs;
						n_total_files += n_files;
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
		case CopyJobMode.LINK:
		case CopyJobMode.UNTRASH:
			// create symlinks for every source file
			foreach (unowned Path src_path in src_paths.peek_head_link ()) {
				File file = src_path.to_gfile ();
				try {
					var info = file.query_info (file_attributes, FileQueryInfoFlags.NOFOLLOW_SYMLINKS, cancellable);
					total_size += get_file_size (info);
					if (info.get_file_type () == FileType.DIRECTORY)
						++n_total_dirs;
					else
						++n_total_files;
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

	private bool copy_dir (File src_file, GLib.FileInfo src_info, File _dest_file) {
		bool retry_mkdir = false;
		File dest_file = _dest_file;
		do {
			try {
				// create the dir
				dest_file.make_directory (cancellable);
				retry_mkdir = false;
			}
			catch (Error err) {
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
				}
				else if (err is IOError.CANCELLED)
					return false; // the job is cancelled
				else { // other errors
					// FIXME: show error to the user
					return false;
				}
			}
		}while (retry_mkdir == true);

		uint32 unix_mode = src_info.get_attribute_uint32 ("unix::mode");
		// ensure that we have rw permission to this file.
		unix_mode |=  (Posix.S_IRUSR|Posix.S_IWUSR);
		try {
			// set the file attributes and ensure that it's writable
			dest_file.set_attribute_uint32 ("unix::mode", unix_mode, FileQueryInfoFlags.NOFOLLOW_SYMLINKS, cancellable);

			// copy files in the directory recursively
			var enu = src_file.enumerate_children (file_attributes, 0, cancellable);
			while (!cancellable.is_cancelled ()) {
				try {
					var child_info = enu.next_file (cancellable);
					if (child_info == null) // end of list
						break;
					File child_src_file = src_file.get_child (child_info.get_name ());
					File child_dest_file = dest_file.get_child (child_info.get_name ());
					if (copy_file (child_src_file, child_info, child_dest_file)) {
					}
					else {
					}
				}
				catch (Error err) {
					stdout.printf ("error: %s\n", err.message);
				}
			}
			enu.close ();
			++n_processed_dirs;

			// if this is actually a cross-device move operation, delete the source file.
			if (mode == CopyJobMode.MOVE) {
				src_file.delete (cancellable);
			}
		}
		catch (Error err) {
			// FIXME: report the error to the user
			stdout.printf ("error: %s\n", err.message);
		}

		return true;
	}

	private void copy_progress_cb (int64 current_num_bytes, int64 total_num_bytes) {
		// calculate percent;
		double fraction =  (double) (processed_size + current_num_bytes) / total_size;
		set_percent (fraction);
		update_progress_display ();
	}

	private bool copy_special_file (File src_file, GLib.FileInfo src_info, File dest_file) throws IOError {
		bool ret = false;
		// only handle FIFO for local files
		if (src_file.is_native () && dest_file.is_native ())
		{
			string src_path = src_file.get_path ();
			Posix.Stat src_st;
			int r = Posix.lstat (src_path, out src_st);
			if (r == 0)
			{
				// Handle FIFO on native file systems.
				if (Posix.S_ISFIFO (src_st.st_mode))
				{
					var dest_path = dest_file.get_path ();
					r = Posix.mkfifo (dest_path, src_st.st_mode);
					if ( r == 0)
						ret = true;
					else {
						// g_io_error_from_errno (errno);
						// FIXME: Vala bug: g_io_error_from_errno doesn't work.
						throw new IOError.FAILED (strerror (errno));
					}
				}
				else {
					// FIXME: how about blcok device, char device, and socket?
					// FIXME: add proper error message
					throw new IOError.NOT_SUPPORTED ("");
				}
			}
			else {
				// FIXME: error handling
				// g_io_error_from_errno (errno);
				// FIXME: Vala bug: g_io_error_from_errno doesn't work.
				throw new IOError.FAILED (strerror (errno));
			}
		}
		else {
			// FIXME: add proper error message
			throw new IOError.NOT_SUPPORTED ("");
		}
		return ret;
	}

	// TODO: optimize for UNIX native files since GFile.copy is extremely inefficient
	// This function is not enabled now due to lack of testing.
	/**private bool copy_native_file (File src_file, GLib.FileInfo src_info, File dest_file) throws IOError {
		bool ret = false;
		string src_path = src_file.get_path ();
		string dest_path = dest_file.get_path ();
		// Because gio is too slow and inefficient, for optimization, we 
		// did an optimized version with POSIX APIs for native files
		Posix.Stat src_st;
		if (Posix.lstat (src_path, out src_st) == 0) {
			if (Posix.S_ISDIR (src_st.st_mode)) {
				// copy_native_dir ();
			}
			else if (Posix.S_ISREG (src_st.st_mode)) { // regular file
				// open the source file for read
				int infd = Posix.open (src_path, Posix.O_RDONLY);
				if (infd != -1) {
					// open the destination file for write
					int outfd = Posix.creat (dest_path, src_st.st_mode);
					if (outfd != -1) {
						// allocate a buffer according to preferred block size
						size_t buf_size = src_st.st_blksize, read_size;
						char[] buf = new char[buf_size];
						// tell the filesystem that we'll do sequential read with fadvice ()
						Posix.posix_fadvise (infd, 0, 0, Posix.POSIX_FADV_SEQUENTIAL);
						// read the source file and write to the destination file
						current_file_processed_size = 0;
						current_file_size = src_st.st_size;

						while ( (read_size = Posix.read (infd, buf, buf_size)) > 0) {
							Posix.write (outfd, buf, read_size);
							current_file_processed_size += read_size;
							// call progress callback
							copy_progress_cb ( (int64)current_file_processed_size,  (int64)current_file_size);
						}
						Posix.close (outfd); // close output destination file
					}
					else {
						ret = false;
					}
					Posix.close (infd); // close input source file
				}
				else {
					throw new IOError.FAILED (strerror (errno));
				}
			}
			else { // special files, such as fifo, socket, and devide files
				ret = copy_special_file (src_file, src_info, dest_file);
			}
		}
		else {
			throw new IOError.FAILED (strerror (errno));
		}
		return ret;
	}**/

	private bool copy_file (File src_file, GLib.FileInfo src_info, File _dest_file) {
		bool ret = false;
		File dest_file = _dest_file;
		// stdout.printf ("%s -> %s\n", src_file.get_parse_name (), dest_file.get_parse_name ());
		set_currently_processed (src_file, src_info, dest_file);
		update_progress_display ();
		// Thread.usleep (2000); // delay for ease of debugging

		/*
		// TODO: optimize for UNIX native files since gio is extremely inefficient
		if (src_file.is_native () && dest_file.is_native ()) {
			return copy_native_file (src_file, src_info, dest_file);
		}
		*/

		FileType type = src_info.get_file_type ();
		if (type == FileType.DIRECTORY) {
			ret = copy_dir (src_file, src_info, dest_file);
		}
		else {
			bool retry_copy = false;
			FileCopyFlags flags = FileCopyFlags.ALL_METADATA|FileCopyFlags.NOFOLLOW_SYMLINKS;
			do {
				try {
					switch (type) {
					case FileType.SPECIAL:
						// handle special files
						ret = copy_special_file (src_file, src_info, dest_file);
						break;
					default:
						ret = src_file.copy (dest_file, flags, cancellable, copy_progress_cb);
						// if this is a cross-device move, delete the source file
						if (ret == true && mode == CopyJobMode.MOVE) {
							src_file.delete (cancellable);
						}
						break;
					}
					retry_copy = false;
					++n_processed_files;
					processed_size += get_file_size (src_info);
				}
				catch (Error err) {
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
							processed_size += get_file_size (src_info);
							break;
						case RenameResult.CANCEL: // cancel the job
							cancel ();
							return false;
						}
					}
					else {
						// present the error to the user
						if (handle_error (err, Severity.MODERATE) == ErrorAction.ABORT) {
							return false;
						}
					}
				}
			}while (retry_copy == true);
		}

		// calculate percent;
		double fraction =  (double)processed_size / total_size;
		set_percent (fraction);
		update_progress_display ();

		return ret;
	}

	private bool move_file (File src_file, GLib.FileInfo src_info, File _dest_file) {
		bool ret = false;
		File dest_file = _dest_file;
		stdout.printf ("move_file %s -> %s\n", src_file.get_parse_name (), dest_file.get_parse_name ());

		set_currently_processed (src_file, src_info, dest_file);
		update_progress_display ();

		// Thread.usleep (2000); // delay for ease of debugging
		bool retry_move = false;
		FileCopyFlags flags = FileCopyFlags.ALL_METADATA|FileCopyFlags.NOFOLLOW_SYMLINKS;
		do {
			try {
				var type = src_info.get_file_type ();
				ret = src_file.move (dest_file, flags, cancellable, copy_progress_cb);
				if (type == FileType.DIRECTORY)
					++n_processed_files;
				else
					++n_processed_files;
				retry_move = false;
				processed_size += get_file_size (src_info);
			}
			catch (Error err) {
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
						processed_size += get_file_size (src_info);
						break;
					case RenameResult.CANCEL: // cancel the job
						cancel ();
						return false;
					}
				}
				else {
					// present the error to the user
					if (handle_error (err, Severity.MODERATE) == ErrorAction.ABORT) {
						return false;
					}
				}
				// processed_size += ...;
			}
		}while (retry_move == true);

		// calculate percent;
		double fraction =  (double)processed_size / total_size;
		set_percent (fraction);
		update_progress_display ();

		return ret;
	}

	private bool link_file (File src_file, GLib.FileInfo src_info, File dest_file) {
		// TODO: implement symlink support later after 1.0 release.
		return false;
	}

	/*
	private bool trash_file (File file, GLib.FileInfo info) {
		bool ret = false;
		set_currently_processed (file, info, null);
		update_progress_display ();

		// Thread.usleep (2000); // delay for ease of debugging
		try {
			file.trash (cancellable);
			ret = true;
			++n_processed_files;
			processed_size += get_file_size (info);
		}
		catch (IOError err) {
			if (handle_error (err) == ErrorAction.ABORT)
				return false;
		}

		// calculate percent;
		double fraction =  (double) (n_processed_files + n_processed_dirs) /  (n_total_dirs + n_total_files);
		set_percent (fraction);
		update_progress_display ();

		return ret;
	}

	// get the real path of the trashed file
	private Path? get_dest_for_trash (Mount home_mount, Path src_path) {
		Path? ret = null;
		var src_file = src_path.to_gfile ();
		if (src_file.is_native ()) {
			try {
				Path? trash_dir = null;
				// var info = file.query_info ("unix::*", 0, null);
				var mount = src_file.find_enclosing_mount (cancellable);
				if (mount != home_mount) { // not in home
					// try top dir
					
				}
				// fallback to home trash dir
				if (trash_dir == null) {
					var trash_dir_name = GLib.Path.build_filename (Environment.get_user_data_dir (), "Trash", "files", null);
					trash_dir = new Path.for_path (trash_dir_name);
				}
				
				if (trash_dir != null) {
					ret = new Path.child (trash_dir, src_path.get_basename ());
				}
			}
			catch (IOError err) {
			}
		}
		return ret;
	}
	*/

	// ensure that we have proper dest paths
	private bool ensure_dest () {
		
        // FIXME: handle cancellable here
		switch (mode) {
            /*
            case CopyJobMode.TRASH:
                dest_paths = new PathList ();
                var home_file = File.new_for_path (Environment.get_home_dir ());
                var home_mount = home_file.find_enclosing_mount (cancellable);
                debug ("home_mount = %p",  (void*)home_mount);
                foreach (unowned Path src_path in src_paths.peek_head_link ()) {
                    if (is_cancelled ())
                        break;
                    // FIXME: the dest path is not correct, but since we do not use it
                    //   this won't cause problems. However, if we implement trashing
                    //   ourselves later, this path should be correct.
                    // var dest_path = new Path.child (Path.get_trash (), path.get_basename ());
                    var dest_path = get_dest_for_trash (home_mount, src_path);
                    debug ("trash dest: %s", dest_path != null ? dest_path.to_str ():"null");
                    dest_paths.push_tail (dest_path);
                }
                break;
            */
            
            case CopyJobMode.UNTRASH: {
                
                dest_paths = new PathList ();
                
                // get original paths of the trashed files
                foreach  (unowned Fm.Path path in src_paths.peek_head_link ()) {
                    
                    if (is_cancelled ())
                        break;
                    
                    //stdout.printf  ("Fm.CopyJob: untrash %s\n", path.to_str ());
                    File file = path.to_gfile ();
                    
                    try {
                        
                        stdout.printf  ("Fm.CopyJob: file.query_info %s\n", path.to_str ());
                        
                        GLib.FileInfo info = file.query_info ("trash::*", 0, null);
                        
                        //stdout.printf  ("Fm.CopyJob: file.query_info %s\n", path.to_str ());
                        
                        unowned string dest_path_str = info.get_attribute_byte_string ("trash::orig-path");
                        
                        Fm.Path dest_path = new Fm.Path.for_str (dest_path_str);
                        
                        //stdout.printf  ("Fm.CopyJob: untrash orig path = orig-path: %s\n", dest_path.to_str ());
                        
                        dest_paths.push_tail (dest_path);
                    }
                    catch (Error err) {
                        
                        stdout.printf  ("Fm.CopyJob: error %s\n", err.message);
                        // FIXME: emit the error properly
                        return false;
                    }
                }
            }
            break;
		}
		return true;
	}

	protected override bool run () {
		
        stdout.printf  ("FmCopyJob.run\n");
        
        if (calculate_total () == false || ensure_dest () == false) // calculate total amount of work
			return false;

		set_ready (); // tell the UI that we're ready
		stdout.printf ("total: %llu, %d, %d\n", total_size, n_total_files, n_total_dirs);

		// ready to copy/move files
		unowned GLib.List<Path> dest_l = dest_paths.peek_head_link ();
		foreach (unowned Path src_path in src_paths.peek_head_link ()) {
			if (is_cancelled ())
				break;
			unowned Path dest_path = dest_l.data;
			var src_file = src_path.to_gfile ();
			var dest_file = dest_path.to_gfile ();
			try {
				// query info of source file and update progress display
				var src_info = src_file.query_info (file_attributes, FileQueryInfoFlags.NOFOLLOW_SYMLINKS, cancellable);
				set_current_src_dest (src_path, dest_path);
				set_currently_processed (src_file, src_info, dest_file);
				update_progress_display ();

				// check if the operation is valid. for example, one cannot
				// move a folder into itself.
				check_paths (src_file, src_info, dest_file);

				// maybe we don't need to call update_progress_display () here 
				// since it will soon be called inside copy_file ()/move_file ().
				switch (mode) {
				case CopyJobMode.COPY:
					copy_file (src_file, src_info, dest_file);
					break;
				case CopyJobMode.MOVE: {
					var dest_dir = dest_file.get_parent ();
					var dest_dir_info = dest_dir.query_info ("id::filesystem", FileQueryInfoFlags.NONE, cancellable);
					var src_fs = src_info.get_attribute_string ("id::filesystem");
					var dest_fs = dest_dir_info.get_attribute_string ("id::filesystem");
					if (src_fs == dest_fs) // on the same filesystem
						move_file (src_file, src_info, dest_file);
					else // cross-device move = copy + delete source
						copy_file (src_file, src_info, dest_file);
					break;
				}
				case CopyJobMode.LINK: // TODO: create symlinks
					link_file (src_file, src_info, dest_file);
					break;
				/*
				case CopyJobMode.TRASH:
					trash_file (src_file, src_info);
					break;
				*/
				case CopyJobMode.UNTRASH:
					move_file (src_file, src_info, dest_file);
					break;
				}
			}
			catch (Error err) {
				if (handle_error (err, Severity.MODERATE) == ErrorAction.ABORT) {
					return false;
				}
			}

			// emit a fake notification signal for file creation for
			// filesystems which don't have file monitor support.
			var dest_dir = dest_file.get_parent (); // get parent folder of dest file
			
            var dest_mon = monitor_lookup_dummy_monitor (dest_dir);
			if (dest_mon != null) {
				dest_mon.changed (dest_file, null, FileMonitorEvent.CREATED);
			}

			dest_l = dest_l.next; // get next dest path
		}

		return true;
	}

	private CopyJobMode mode;
	protected PathList? dest_paths;
}

}
