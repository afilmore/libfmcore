//      
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


    public class GtkUI : Object, UI {
        
        protected weak Gtk.Window? parent_win;
        
        public GtkUI (Gtk.Window? parent_window = null) {
            parent_win = parent_window;
            if (parent_win != null) {
                parent_win.destroy.connect ( () => {
                    parent_win = null;
                });
            }
        }

        public void show_error (string message, string? title = null, bool use_markup = true) {
            stderr.printf ("title: %s\nmessage: %s\n", title, message);
        }

        public bool ask_ok_cancel (string question, string? title = null, bool use_markup = true) {
            stderr.printf ("title: %s\nquestion: %s\n", title, question);
            return false;
        }

        public bool ask_yes_no (string question, string? title = null, bool use_markup = true) {
            stderr.printf ("title: %s\nquestion: %s\n", title, question);
            return false;
        }

        public int ask (string question, string? title, bool use_markup, ...) {
            
            return 0;
        }

        public void open_folder (Fm.Path folder) {
            // FIXME: how to do this correctly?
        }
        
        public Gtk.Window? get_parent_window () {
            return parent_win;
        }

    }

}
