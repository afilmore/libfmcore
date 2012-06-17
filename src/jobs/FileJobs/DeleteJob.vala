//      delete-job.vala
//      
//      Copyright 2011 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
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

public class DeleteJob : FileJob {

	public DeleteJob(PathList paths, FileJobUI? ui) {
		base(ui);
		src_paths = paths;
		unowned string title = _("Deleting files");
		ui.init_with_job(this, title, title, false);
	}

	private bool delete_file(File file, GLib.FileInfo info) {
		bool ret = false;
		set_currently_processed(file, info, null);
		update_progress_display();

		// Thread.usleep(2000); // delay for ease of debugging
		if(info.get_file_type() == FileType.DIRECTORY) {
			
            FileEnumerator enu;
            try {
                enu = file.enumerate_children(file_attributes, FileQueryInfoFlags.NOFOLLOW_SYMLINKS, cancellable);
			while(cancellable.is_cancelled() == false) {
				GLib.FileInfo child_info = enu.next_file(cancellable);
				if(child_info == null) // end of file list
					break;
				
                var child = file.get_child(child_info.get_name());

				delete_file(child, child_info);
			}
			enu.close();
			++n_processed_dirs;
            } catch (Error error) {
            }
		}
		else {
			++n_processed_files;
		}
		processed_size += get_file_size(info);
		try {
			file.delete(cancellable);
			ret = true;
		}
		catch(Error err) {
		}

		// calculate percent;
		double fraction = (double)(n_processed_files + n_processed_dirs) / (n_total_dirs + n_total_files);
		set_percent(fraction);
		update_progress_display();

		return ret;
	}

	protected override bool run() {

		// show the first file in the UI
		set_current_src_dest(src_paths.peek_head(), null);
		update_progress_display();

		// calculate total amount of work for progress display
		if(calculate_total() == false)
			return false;

		// inform the UI that we're ready
		set_ready();
		stdout.printf("total: %llu, %d, %d\n", total_size, n_total_files, n_total_dirs);

		// delete all source files one by one
		foreach(unowned Path src_path in src_paths.peek_head_link()) {
			File file = src_path.to_gfile();
			try {
				var info = file.query_info(file_attributes, FileQueryInfoFlags.NOFOLLOW_SYMLINKS, cancellable);
				// show currently processed file in UI
				set_current_src_dest(src_path, null);
				set_currently_processed(file, info, null);
				update_progress_display();

				delete_file(file, info);
			}
			catch(Error err) {
				if(handle_error(err) == ErrorAction.ABORT)
					return false;
				// TODO: handle ErrorAction.RETRY later
			}

			// emit a fake notification signal for file deletion for
			// filesystems which don't have file monitor support.
			// FIXME: should we do this to regular GFileMonitor as well?
			var parent_dir = file.get_parent(); // get parent folder of src file
			var parent_mon = monitor_lookup_dummy_monitor(parent_dir);
			if(parent_mon != null) {
				parent_mon.changed(file, null, FileMonitorEvent.DELETED);
			}
		}

		return true;
	}
}

}
