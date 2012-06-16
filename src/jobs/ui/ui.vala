//      ui.vala
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

public interface UI : Object {

	[Flags]
	enum Flags {
		NONE,
		USE_MARKUP
	}

	public abstract void show_error(string message, string? title = null, bool use_markup = true);

	public abstract bool ask_ok_cancel(string question, string? title = null, bool use_markup = true);

	public abstract bool ask_yes_no(string question, string? title = null, bool use_markup = true);

	// the remaining arguments should be button/numeric id pairs, terminated with null
	public abstract int ask(string question, string? title, bool use_markup, ...);

	public abstract void open_folder(Fm.Path folder);

}

public enum RenameResult {
	NONE,
	RENAME,
	OVERWRITE,
	SKIP,
	CANCEL
}

public interface FileJobUI : Object, UI {

	// initialize the ui with the job
	public abstract void init_with_job(FileJob job, string title, string from_label, bool has_dest);

	// called by FileJob to inform the start of the job
	public abstract void start();

	// called by FileJob to inform the end of the job
	// though FileJob emits "finished" signal, it's easier to just call a function
	public abstract void finish();

	// called by FileJob to show total amount of the job, may be called for many times
	public abstract void set_total_amount(uint64 size, int n_files, int n_dirs);

	// get prepared and ready to do the file operation
	public abstract void set_ready();

	// progress info, in %
	public abstract void set_percent(int percent);

	// estimated time
	public abstract void set_times(uint elapsed, uint remaining);

	// set src/dest paths currently being handled
	public abstract void set_current_src_dest(Path src_path, Path? dest_path = null);

	// set currently processed child file/folder and optionally, its dest
	public abstract void set_currently_processed(File src, GLib.FileInfo? src_info, File? dest = null);

	// ask for a new file name
	public abstract RenameResult ask_rename(File src, GLib.FileInfo src_info, File dest, GLib.FileInfo dest_info, out File new_dest);

	// show the error to the user and do some interaction if needed.
	// return true to continue the job and ignore the error and return 
	// false to cancel the job.
	public abstract ErrorAction handle_error(Error err, Severity severity);
}

}
