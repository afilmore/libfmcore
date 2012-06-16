//      create-file-job.vala
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

public enum CreateFileJobMode {
	FILE,
	DIRECTORY
}

// TODO: replace direct calls to g_file_create() in libfm/pcmanfm
public class CreateFileJob : FileJob {
	public CreateFileJob(FileJobUI? ui = null) {
		base(ui);
		if(ui != null) {
			//unowned string title = _("Creating File");
			//ui.init_with_job(this, title, title, false);
		}
	}

	public override bool run() {
		return true;
	}

	private CreateFileJobMode mode;
}

}
