//      rename-dlg.vala
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



    private const int RESPONSE_OVERWRITE = 1;
    private const int RESPONSE_RENAME = 2;
    private const int RESPONSE_SKIP = 3;

    public class RenameDialog : Object {

        Gtk.Window? parent;
        Gtk.Dialog dlg;
        Fm.FileInfo? src_info;
        Fm.FileInfo? dest_info;
        bool apply_to_all;
        string? old_name;
        string? new_name;
        
        public RenameDialog (Gtk.Window? parent = null) {
            this.parent = parent;
        }

        ~RenameDialog () {
        }

        public void set_src_info (Fm.FileInfo src_info) {
            this.src_info = src_info;
        }

        public void set_dest_info (Fm.FileInfo dest_info) {
            this.dest_info = dest_info;
        }

        public bool get_apply_to_all () {
            return apply_to_all;
        }

        public unowned string? get_new_name () {
            return new_name;
        }

        private void on_filename_entry_changed (Gtk.Editable entry) {
            unowned string new_name =  ( (Gtk.Entry)entry).get_text ();
            // the new name should not be empty, the same as the old one, or contain '/'.
            bool can_rename =  (new_name != "" && old_name != new_name && new_name.index_of_char ('/') == -1);
            dlg.set_response_sensitive (RESPONSE_RENAME, can_rename);
            if (can_rename)
                dlg.set_default_response (RESPONSE_RENAME);
        }

        private void on_parent_destroy (Object parent_win) {
            parent = null;
        }

        public RenameResult run () {
            RenameResult ret;
            Gtk.Builder builder;
            try {
                builder = new Gtk.Builder ();
                
                /*
                builder.set_translation_domain (Config.GETTEXT_PACKAGE);
                */
            
                builder.add_from_string (ASK_RENAME_XML, -1);
            }
            catch (Gtk.BuilderError err) {
                return RenameResult.NONE;
            }
            catch (GLib.Error err) {
                return RenameResult.NONE;
            }

            dlg =  (Gtk.Dialog)builder.get_object ("dlg");
            var src_icon =  (Gtk.Image)builder.get_object ("src_icon");
            var src_fi_label =  (Gtk.Label)builder.get_object ("src_fi");
            var dest_icon =  (Gtk.Image)builder.get_object ("dest_icon");
            var dest_fi_label =  (Gtk.Label)builder.get_object ("dest_fi");
            var filename_entry =  (Gtk.Entry)builder.get_object ("filename");
            var apply_all_check_button =  (Gtk.CheckButton)builder.get_object ("apply_all");
            dlg.set_transient_for (parent);

            if (src_info != null) {
    // !!!!
    //			src_icon.set_from_gicon (src_info.get_icon ().gicon, Gtk.IconSize.DIALOG);
                var disp_size = src_info.get_disp_size ();
                string text;
                if (disp_size != null)
                    text = _ ("Type: %s\nSize: %s\nModified: %s").printf (src_info.get_desc (), disp_size, src_info.get_disp_mtime ());
                else
                    text = _ ("Type: %s\nModified: %s").printf (src_info.get_desc (), src_info.get_disp_mtime ());
                src_fi_label.set_text (text);
            }
            else {
                src_icon.hide ();
                src_fi_label.hide ();
            }

            if (dest_info != null) {
    // !!!!!			
    //            dest_icon.set_from_gicon (dest_info.get_icon ().gicon, Gtk.IconSize.DIALOG);
                
                var disp_size = dest_info.get_disp_size ();
                string text;
                if (disp_size != null)
                    text = _ ("Type: %s\nSize: %s\nModified: %s").printf (dest_info.get_desc (), disp_size, dest_info.get_disp_mtime ());
                else
                    text = _ ("Type: %s\nModified: %s").printf (dest_info.get_desc (), dest_info.get_disp_mtime ());
                dest_fi_label.set_text (text);

                old_name = dest_info.get_path ().display_basename ();
                filename_entry.set_text (old_name);			
            }
            else {
                dest_icon.hide ();
                dest_fi_label.hide ();
            }

            filename_entry.changed.connect (on_filename_entry_changed);

            switch (dlg.run ())
            {
            case RESPONSE_RENAME:
                new_name = filename_entry.get_text ();
                if (new_name == null || new_name == "") // is this possible?
                    new_name = old_name; // just don't leave it null
                ret = RenameResult.RENAME;
                break;
            case RESPONSE_OVERWRITE:
                ret = RenameResult.OVERWRITE;
                break;
            case RESPONSE_SKIP:
                ret = RenameResult.SKIP;
                break;
            default:
                ret = RenameResult.CANCEL;
                break;
            }

            apply_to_all = apply_all_check_button.get_active ();
            dlg.destroy ();
            return ret;
        }

    }

}
