//      jobs.vala
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

//~     public FileJob copy_files2(PathList path_list, PathList dest_paths, FileJobUI? ui = null) {
//~         var job = new CopyJob(CopyJobMode.COPY, path_list, dest_paths, ui);
//~         job.run_async();
//~         return job;
//~     }

//~     public FileJob copy_files_to_dir(PathList path_list, Path dest_dir, FileJobUI? ui = null) {
//~         
//~         var dest_paths = new PathList();
//~         
//~         foreach(unowned Path src_path in path_list.peek_head_link()) {
//~             
//~             var dest_path = new Path.child(dest_dir, src_path.get_basename());
//~             dest_paths.push_tail(dest_path);
//~         }
//~         
//~         
//~         var job = new CopyJob (CopyJobMode.COPY, path_list, dest_paths, ui);
//~         job.run_async();
//~         return job;
//~         
//~         //return copy_files2(path_list, dest_paths, ui);
//~     }
//~ 
    
    /*public FileJob move_files2(PathList path_list, PathList dest_paths, FileJobUI? ui = null) {
        var job = new CopyJob(CopyJobMode.MOVE, path_list, dest_paths, ui);
        job.run_async();
        return job;
    }

    public FileJob move_files_to_dir(PathList path_list, Path dest_dir, FileJobUI? ui = null) {
        var dest_paths = new PathList();
        foreach(unowned Path src_path in path_list.peek_head_link()) {
            var dest_path = new Path.child(dest_dir, src_path.get_basename());
            dest_paths.push_tail(dest_path);
        }
        return move_files2(path_list, dest_paths, ui);
    }*/

}


