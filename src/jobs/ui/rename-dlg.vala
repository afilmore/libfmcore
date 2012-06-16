//      rename-dlg.vala
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

    const string ASK_RENAME_XML = """
        <?xml version='1.0' encoding='UTF-8'?>
        <interface>
        <!-- interface-requires gtk+ 3.0 -->
          <object class="GtkDialog" id="dlg">
            <property name="border_width">10</property>
            <property name="title" translatable="yes">Confirm to replace files</property>
            <property name="type_hint">normal</property>
            <child internal-child="vbox">
              <object class="GtkVBox" id="dialog-vbox">
                <property name="visible">True</property>
                <property name="orientation">vertical</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkHBox" id="hbox">
                    <property name="visible">True</property>
                    <property name="spacing">12</property>
                    <child>
                      <object class="GtkImage" id="image1">
                        <property name="visible">True</property>
                        <property name="yalign">0</property>
                        <property name="stock">gtk-dialog-question</property>
                        <property name="icon-size">6</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkVBox" id="vbox1">
                        <property name="visible">True</property>
                        <property name="orientation">vertical</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkLabel" id="label1">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">&lt;b&gt;There is already a file with the same name in this location.&lt;/b&gt;

        Do you want to replace the existing file</property>
                            <property name="use_markup">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkHBox" id="hbox2">
                            <property name="visible">True</property>
                            <property name="spacing">12</property>
                            <child>
                              <object class="GtkImage" id="dest_icon">
                                <property name="visible">True</property>
                                <property name="stock">gtk-missing-image</property>
                                <property name="icon-size">6</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="position">0</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkLabel" id="dest_fi">
                                <property name="visible">True</property>
                                <property name="xalign">0</property>
                              </object>
                              <packing>
                                <property name="position">1</property>
                              </packing>
                            </child>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="position">1</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label2">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">with the following file?</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="position">2</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkHBox" id="hbox3">
                            <property name="visible">True</property>
                            <property name="spacing">12</property>
                            <child>
                              <object class="GtkImage" id="src_icon">
                                <property name="visible">True</property>
                                <property name="stock">gtk-missing-image</property>
                                <property name="icon-size">6</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="position">0</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkLabel" id="src_fi">
                                <property name="visible">True</property>
                                <property name="xalign">0</property>
                              </object>
                              <packing>
                                <property name="position">1</property>
                              </packing>
                            </child>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="position">3</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkHBox" id="hbox4">
                            <property name="visible">True</property>
                            <property name="spacing">12</property>
                            <child>
                              <object class="GtkLabel" id="label3">
                                <property name="visible">True</property>
                                <property name="xalign">0</property>
                                <property name="label" translatable="yes">File Name:</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="position">0</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkEntry" id="filename">
                                <property name="visible">True</property>
                                <property name="can_focus">True</property>
                                <property name="invisible_char">&#x2022;</property>
                                <property name="activates_default">True</property>
                              </object>
                              <packing>
                                <property name="position">1</property>
                              </packing>
                            </child>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="position">4</property>
                          </packing>
                        </child>
                      </object>
                      <packing>
                        <property name="position">1</property>
                      </packing>
                    </child>
                  </object>
                  <packing>
                    <property name="position">1</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="apply_all">
                    <property name="label" translatable="yes">Apply this option to all existing files</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="position">2</property>
                  </packing>
                </child>
                <child internal-child="action_area">
                  <object class="GtkHButtonBox" id="dialog-action_area">
                    <property name="visible">True</property>
                    <property name="layout_style">end</property>
                    <child>
                      <object class="GtkButton" id="cancel">
                        <property name="label">gtk-cancel</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">True</property>
                        <property name="use_stock">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkButton" id="skip">
                        <property name="label" translatable="yes">_Skip</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="can_default">True</property>
                        <property name="has_default">True</property>
                        <property name="receives_default">True</property>
                        <property name="use_underline">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">1</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkButton" id="rename">
                        <property name="label" translatable="yes">_Rename</property>
                        <property name="visible">True</property>
                        <property name="sensitive">False</property>
                        <property name="can_focus">True</property>
                        <property name="can_default">True</property>
                        <property name="receives_default">True</property>
                        <property name="use_underline">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">2</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkButton" id="overwrite">
                        <property name="label" translatable="yes">_Overwrite</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">True</property>
                        <property name="use_underline">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">3</property>
                      </packing>
                    </child>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="pack_type">end</property>
                    <property name="position">0</property>
                  </packing>
                </child>
              </object>
            </child>
            <action-widgets>
              <action-widget response="-6">cancel</action-widget>
              <action-widget response="3">skip</action-widget>
              <action-widget response="2">rename</action-widget>
              <action-widget response="1">overwrite</action-widget>
            </action-widgets>
          </object>
        </interface>
    """;


private const int RESPONSE_OVERWRITE = 1;
private const int RESPONSE_RENAME = 2;
private const int RESPONSE_SKIP = 3;

public class RenameDialog : Object {

	public RenameDialog(Gtk.Window? parent = null) {
		this.parent = parent;
	}

	public void set_src_info(Fm.FileInfo src_info) {
		this.src_info = src_info;
	}

	public void set_dest_info(Fm.FileInfo dest_info) {
		this.dest_info = dest_info;
	}

	public bool get_apply_to_all() {
		return apply_to_all;
	}

	public unowned string? get_new_name() {
		return new_name;
	}

	private void on_filename_entry_changed(Gtk.Editable entry) {
		unowned string new_name = ((Gtk.Entry)entry).get_text();
		// the new name should not be empty, the same as the old one, or contain '/'.
		bool can_rename = (new_name != "" && old_name != new_name && new_name.index_of_char('/') == -1);
		dlg.set_response_sensitive(RESPONSE_RENAME, can_rename);
		if(can_rename)
			dlg.set_default_response(RESPONSE_RENAME);
	}

	private void on_parent_destroy(Object parent_win) {
		parent = null;
	}

	public RenameResult run() {
		RenameResult ret;
		Gtk.Builder builder;
		try {
			builder = new Gtk.Builder();
			
            /*
            builder.set_translation_domain(Config.GETTEXT_PACKAGE);
            */
        
			builder.add_from_string(ASK_RENAME_XML, -1);
        }
		catch(Gtk.BuilderError err) {
			return RenameResult.NONE;
		}
		catch(GLib.Error err) {
			return RenameResult.NONE;
		}

		dlg = (Gtk.Dialog)builder.get_object("dlg");
		var src_icon = (Gtk.Image)builder.get_object("src_icon");
		var src_fi_label = (Gtk.Label)builder.get_object("src_fi");
		var dest_icon = (Gtk.Image)builder.get_object("dest_icon");
		var dest_fi_label = (Gtk.Label)builder.get_object("dest_fi");
		var filename_entry = (Gtk.Entry)builder.get_object("filename");
		var apply_all_check_button = (Gtk.CheckButton)builder.get_object("apply_all");
		dlg.set_transient_for(parent);

		if(src_info != null) {
// !!!!
//			src_icon.set_from_gicon(src_info.get_icon().gicon, Gtk.IconSize.DIALOG);
			var disp_size = src_info.get_disp_size();
			string text;
			if(disp_size != null)
				text = _("Type: %s\nSize: %s\nModified: %s").printf(src_info.get_desc(), disp_size, src_info.get_disp_mtime());
			else
				text = _("Type: %s\nModified: %s").printf(src_info.get_desc(), src_info.get_disp_mtime());
			src_fi_label.set_text(text);
		}
		else {
			src_icon.hide();
			src_fi_label.hide();
		}

		if(dest_info != null) {
// !!!!!			
//            dest_icon.set_from_gicon(dest_info.get_icon().gicon, Gtk.IconSize.DIALOG);
			
            var disp_size = dest_info.get_disp_size();
			string text;
			if(disp_size != null)
				text = _("Type: %s\nSize: %s\nModified: %s").printf(dest_info.get_desc(), disp_size, dest_info.get_disp_mtime());
			else
				text = _("Type: %s\nModified: %s").printf(dest_info.get_desc(), dest_info.get_disp_mtime());
			dest_fi_label.set_text(text);

			old_name = dest_info.get_path().display_basename();
			filename_entry.set_text(old_name);			
		}
		else {
			dest_icon.hide();
			dest_fi_label.hide();
		}

		filename_entry.changed.connect(on_filename_entry_changed);

		switch(dlg.run())
		{
		case RESPONSE_RENAME:
			new_name = filename_entry.get_text();
			if(new_name == null || new_name == "") // is this possible?
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

		apply_to_all = apply_all_check_button.get_active();
		dlg.destroy();
		return ret;
	}

	~RenameDialog() {
	}

	Gtk.Window? parent;
	Gtk.Dialog dlg;
	Fm.FileInfo? src_info;
	Fm.FileInfo? dest_info;
	bool apply_to_all;
	string? old_name;
	string? new_name;
}

}
