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


}
