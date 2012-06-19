/***********************************************************************************************************************
 * 
 *      RenameDialog.vala
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



    private const int RESPONSE_OVERWRITE = 1;
    private const int RESPONSE_RENAME = 2;
    private const int RESPONSE_SKIP = 3;

    public class RenameDialog : Object {

        Gtk.Window?     _parent;
        Gtk.Dialog      _dlg;
        Fm.FileInfo?    _src_info;
        Fm.FileInfo?    _dest_info;
        bool            _apply_to_all;
        string?         _old_name;
        string?         _new_name;
        
        public RenameDialog (Gtk.Window? parent = null) {
            this._parent = parent;
        }

        ~RenameDialog () {
        }

        public void set_src_info (Fm.FileInfo src_info) {
            this._src_info = src_info;
        }

        public void set_dest_info (Fm.FileInfo dest_info) {
            this._dest_info = dest_info;
        }

        public bool get_apply_to_all () {
            return _apply_to_all;
        }

        public unowned string? get_new_name () {
            return _new_name;
        }

        private void on_filename_entry_changed (Gtk.Editable entry) {
            
            unowned string _new_name =  ((Gtk.Entry) entry).get_text ();
            
            // the new name should not be empty, the same as the old one, or contain '/'.
            bool can_rename = (_new_name != "" && _old_name != _new_name && _new_name.index_of_char ('/') == -1);
            
            _dlg.set_response_sensitive (RESPONSE_RENAME, can_rename);
            
            if (can_rename)
                _dlg.set_default_response (RESPONSE_RENAME);
        }

// unused...
//~         private void on_parent_destroy (Object parent_win) {
//~             _parent = null;
//~         }

        public RenameResult run () {
            
            RenameResult ret;
            Gtk.Builder builder;
            
            try {
                builder = new Gtk.Builder ();
                
                /* TODO_axl: add config vapi...
                builder.set_translation_domain (Config.GETTEXT_PACKAGE);
                */
            
                builder.add_from_string (ASK_RENAME_XML, -1);
            
            } catch (Gtk.BuilderError err) {
                return RenameResult.NONE;
            
            } catch (GLib.Error err) {
                return RenameResult.NONE;
            }

            _dlg = builder.get_object ("dlg") as Gtk.Dialog;
            
            Gtk.Image src_icon =        builder.get_object ("src_icon") as Gtk.Image;
            Gtk.Label src_fi_label =    builder.get_object ("src_fi") as Gtk.Label;
            Gtk.Image dest_icon =       builder.get_object ("dest_icon") as Gtk.Image;
            Gtk.Label dest_fi_label =   builder.get_object ("dest_fi") as Gtk.Label;
            Gtk.Entry filename_entry =  builder.get_object ("filename") as Gtk.Entry;
            Gtk.CheckButton apply_all_check_button = builder.get_object ("apply_all") as Gtk.CheckButton;
            
            _dlg.set_transient_for (_parent);

            if (_src_info != null) {
                
                
                // TODO_axl: set the icon...                
                //~ src_icon.set_from_gicon (_src_info.get_icon ().gicon, Gtk.IconSize.DIALOG);
                
                string disp_size = _src_info.get_disp_size ();
                string text;
                
                if (disp_size != null)
                    text = _("Type: %s\nSize: %s\nModified: %s").printf (_src_info.get_desc (), disp_size, _src_info.get_disp_mtime ());
                else
                    text = _("Type: %s\nModified: %s").printf (_src_info.get_desc (), _src_info.get_disp_mtime ());
                
                src_fi_label.set_text (text);
            
            } else {
                src_icon.hide ();
                src_fi_label.hide ();
            }

            if (_dest_info != null) {
                
                // TODO_axl: set the icon...                
                //~ dest_icon.set_from_gicon (_dest_info.get_icon ().gicon, Gtk.IconSize.DIALOG);
                
                
                string disp_size = _dest_info.get_disp_size ();
                string text;
                
                if (disp_size != null)
                    text = _("Type: %s\nSize: %s\nModified: %s").printf (_dest_info.get_desc (), disp_size, _dest_info.get_disp_mtime ());
                else
                    text = _("Type: %s\nModified: %s").printf (_dest_info.get_desc (), _dest_info.get_disp_mtime ());
                
                dest_fi_label.set_text (text);

                _old_name = _dest_info.get_path ().display_basename ();
                filename_entry.set_text (_old_name);			
            
            } else {
                dest_icon.hide ();
                dest_fi_label.hide ();
            }

            filename_entry.changed.connect (on_filename_entry_changed);

            switch (_dlg.run ()) {
                
                case RESPONSE_RENAME:
                    _new_name = filename_entry.get_text ();
                    if (_new_name == null || _new_name == "") // is this possible?
                        _new_name = _old_name; // just don't leave it null
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

            _apply_to_all = apply_all_check_button.get_active ();
            _dlg.destroy ();
            return ret;
        }
    }
}



