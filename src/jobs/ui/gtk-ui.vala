namespace Fm {

    const string PROGRESS_XML = """
          <?xml version='1.0' encoding='UTF-8'?>
          <interface>
          <!-- interface-requires gtk+ 3.0 -->
          <object class="GtkDialog" id="dlg">
            <property name="visible">True</property>
            <property name="can_focus">False</property>
            <property name="border_width">5</property>
            <property name="default_width">480</property>
            <property name="type_hint">dialog</property>
            <property name="gravity">north-east</property>
            <child internal-child="vbox">
              <object class="GtkVBox" id="dialog-vbox">
                <property name="visible">True</property>
                <property name="can_focus">False</property>
                <property name="spacing">6</property>
                <child internal-child="action_area">
                  <object class="GtkHButtonBox" id="dialog-action_area">
                    <property name="visible">True</property>
                    <property name="can_focus">False</property>
                    <property name="layout_style">end</property>
                    <child>
                      <object class="GtkButton" id="cancel">
                        <property name="label">gtk-cancel</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="can_default">True</property>
                        <property name="receives_default">False</property>
                        <property name="use_action_appearance">False</property>
                        <property name="use_stock">True</property>
                        <signal name="clicked" handler="on_cancel_button_clicked" object="fileOperation" swapped="yes"/>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">True</property>
                    <property name="pack_type">end</property>
                    <property name="position">0</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkVBox" id="vbox1">
                    <property name="visible">True</property>
                    <property name="can_focus">False</property>
                    <property name="border_width">6</property>
                    <property name="spacing">18</property>
                    <child>
                      <object class="GtkLabel" id="msg">
                        <property name="can_focus">False</property>
                        <property name="xalign">0</property>
                        <property name="wrap">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">True</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkHBox" id="hbox1">
                        <property name="visible">True</property>
                        <property name="can_focus">False</property>
                        <property name="spacing">12</property>
                        <child>
                          <object class="GtkImage" id="icon">
                            <property name="visible">True</property>
                            <property name="can_focus">False</property>
                            <property name="stock">gtk-file</property>
                            <property name="icon-size">6</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">True</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkTable" id="table">
                            <property name="visible">True</property>
                            <property name="can_focus">False</property>
                            <property name="border_width">6</property>
                            <property name="n_rows">5</property>
                            <property name="n_columns">2</property>
                            <property name="column_spacing">12</property>
                            <property name="row_spacing">6</property>
                            <child>
                              <object class="GtkLabel" id="src">
                                <property name="visible">True</property>
                                <property name="can_focus">False</property>
                                <property name="xalign">0</property>
                                <property name="ellipsize">end</property>
                              </object>
                              <packing>
                                <property name="left_attach">1</property>
                                <property name="right_attach">2</property>
                                <property name="x_options">GTK_FILL</property>
                                <property name="y_options"></property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkLabel" id="dest">
                                <property name="visible">True</property>
                                <property name="can_focus">False</property>
                                <property name="xalign">0</property>
                                <property name="ellipsize">middle</property>
                              </object>
                              <packing>
                                <property name="left_attach">1</property>
                                <property name="right_attach">2</property>
                                <property name="top_attach">1</property>
                                <property name="bottom_attach">2</property>
                                <property name="x_options">GTK_FILL</property>
                                <property name="y_options"></property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkLabel" id="current">
                                <property name="visible">True</property>
                                <property name="can_focus">False</property>
                                <property name="xalign">0</property>
                                <property name="label" translatable="yes" comments="Preparing to do some file operation (Copy, Move, Delete...) ">Preparing...</property>
                                <property name="ellipsize">middle</property>
                              </object>
                              <packing>
                                <property name="left_attach">1</property>
                                <property name="right_attach">2</property>
                                <property name="top_attach">2</property>
                                <property name="bottom_attach">3</property>
                                <property name="x_options">GTK_FILL</property>
                                <property name="y_options"></property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkProgressBar" id="progress">
                                <property name="visible">True</property>
                                <property name="can_focus">False</property>
                                <property name="pulse_step">0.10000000149</property>
                              </object>
                              <packing>
                                <property name="left_attach">1</property>
                                <property name="right_attach">2</property>
                                <property name="top_attach">3</property>
                                <property name="bottom_attach">4</property>
                                <property name="y_options"></property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkLabel" id="label4">
                                <property name="visible">True</property>
                                <property name="can_focus">False</property>
                                <property name="xalign">0</property>
                                <property name="label" translatable="yes" comments="Processing: &lt;Name of currently proccesed file&gt;">&lt;b&gt;Processing:&lt;/b&gt;</property>
                                <property name="use_markup">True</property>
                              </object>
                              <packing>
                                <property name="top_attach">2</property>
                                <property name="bottom_attach">3</property>
                                <property name="x_options">GTK_FILL</property>
                                <property name="y_options"></property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkLabel" id="to_label">
                                <property name="visible">True</property>
                                <property name="can_focus">False</property>
                                <property name="xalign">0</property>
                                <property name="label" translatable="yes" comments="To: &lt;Destination folder&gt; ex. Copy file to..., Move file to...etc.">&lt;b&gt;To:&lt;/b&gt;</property>
                                <property name="use_markup">True</property>
                              </object>
                              <packing>
                                <property name="top_attach">1</property>
                                <property name="bottom_attach">2</property>
                                <property name="x_options">GTK_FILL</property>
                                <property name="y_options"></property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkLabel" id="label3">
                                <property name="visible">True</property>
                                <property name="can_focus">False</property>
                                <property name="xalign">0</property>
                                <property name="label" translatable="yes">&lt;b&gt;Progress:&lt;/b&gt;</property>
                                <property name="use_markup">True</property>
                              </object>
                              <packing>
                                <property name="top_attach">3</property>
                                <property name="bottom_attach">4</property>
                                <property name="x_options">GTK_FILL</property>
                                <property name="y_options"></property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkLabel" id="action">
                                <property name="visible">True</property>
                                <property name="can_focus">False</property>
                                <property name="xalign">0</property>
                                <property name="use_markup">True</property>
                              </object>
                              <packing>
                                <property name="x_options">GTK_FILL</property>
                                <property name="y_options"></property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkLabel" id="label2">
                                <property name="visible">True</property>
                                <property name="can_focus">False</property>
                                <property name="xalign">0</property>
                                <property name="label" translatable="yes">&lt;b&gt;Time remaining:&lt;/b&gt;</property>
                                <property name="use_markup">True</property>
                              </object>
                              <packing>
                                <property name="top_attach">4</property>
                                <property name="bottom_attach">5</property>
                                <property name="x_options">GTK_FILL</property>
                                <property name="y_options"></property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkLabel" id="remaining_time">
                                <property name="visible">True</property>
                                <property name="can_focus">False</property>
                                <property name="xalign">0</property>
                              </object>
                              <packing>
                                <property name="left_attach">1</property>
                                <property name="right_attach">2</property>
                                <property name="top_attach">4</property>
                                <property name="bottom_attach">5</property>
                              </packing>
                            </child>
                          </object>
                          <packing>
                            <property name="expand">True</property>
                            <property name="fill">True</property>
                            <property name="position">1</property>
                          </packing>
                        </child>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">True</property>
                        <property name="position">1</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkVBox" id="error_pane">
                        <property name="can_focus">False</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkLabel" id="label1">
                            <property name="visible">True</property>
                            <property name="can_focus">False</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">&lt;b&gt;Some errors occurred:&lt;/b&gt;</property>
                            <property name="use_markup">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">True</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkAlignment" id="alignment1">
                            <property name="visible">True</property>
                            <property name="can_focus">False</property>
                            <property name="left_padding">12</property>
                            <child>
                              <object class="GtkScrolledWindow" id="scrolledwindow1">
                                <property name="visible">True</property>
                                <property name="can_focus">True</property>
                                <property name="hscrollbar_policy">automatic</property>
                                <property name="vscrollbar_policy">automatic</property>
                                <property name="shadow_type">etched-in</property>
                                <child>
                                  <object class="GtkTextView" id="error_msg">
                                    <property name="height_request">120</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="cursor_visible">False</property>
                                  </object>
                                </child>
                              </object>
                            </child>
                          </object>
                          <packing>
                            <property name="expand">True</property>
                            <property name="fill">True</property>
                            <property name="position">1</property>
                          </packing>
                        </child>
                      </object>
                      <packing>
                        <property name="expand">True</property>
                        <property name="fill">True</property>
                        <property name="position">2</property>
                      </packing>
                    </child>
                  </object>
                  <packing>
                    <property name="expand">True</property>
                    <property name="fill">True</property>
                    <property name="position">1</property>
                  </packing>
                </child>
              </object>
            </child>
            <action-widgets>
              <action-widget response="-6">cancel</action-widget>
            </action-widgets>
          </object>
        </interface>
    """;



public class GtkUI : Object, UI {
	public GtkUI(Gtk.Window? parent_window = null) {
		parent_win = parent_window;
		if(parent_win != null) {
			parent_win.destroy.connect(() => {
				parent_win = null;
			});
		}
	}

	public void show_error(string message, string? title = null, bool use_markup = true) {
		stderr.printf("title: %s\nmessage: %s\n", title, message);
	}

	public bool ask_ok_cancel(string question, string? title = null, bool use_markup = true) {
		stderr.printf("title: %s\nquestion: %s\n", title, question);
		return false;
	}

	public bool ask_yes_no(string question, string? title = null, bool use_markup = true) {
		stderr.printf("title: %s\nquestion: %s\n", title, question);
		return false;
	}

	public int ask(string question, string? title, bool use_markup, ...) {
		
		return 0;
	}

	public void open_folder(Fm.Path folder) {
		// FIXME: how to do this correctly?
	}
	
	public Gtk.Window? get_parent_window() {
		return parent_win;
	}

	protected weak Gtk.Window? parent_win;
}

public class GtkFileJobUI : GtkUI, FileJobUI {

	public GtkFileJobUI(Gtk.Window? parent_window = null) {
		base(parent_window);
		default_rename_result = RenameResult.NONE;
	}
	
	public override void dispose() {
		job = null;
	}

	~GtkFileJobUI() {
		if(show_dlg_timeout_id != 0)
			Source.remove(show_dlg_timeout_id);
	}

	// initialize the ui with the job
	public void init_with_job(FileJob job, string title, string action, bool has_dest) {
		this.title = title;
		this.action_label_text = action;
		this.has_dest = has_dest;
		this.job = job;
	}

	// called by FileJob to inform the start of the job
	public void start() {
		show_dlg_timeout_id = Timeout.add(1000, on_show_dlg_timeout);
	}

	private void finish_show_error() {
		current_file_label.set_text("");
		remaining_time_label.set_text("00:00:00");
		dlg.set_response_sensitive(Gtk.ResponseType.CANCEL, false);
		dlg.add_button(Gtk.Stock.CLOSE, Gtk.ResponseType.CLOSE);
		msg_label.show();
		icon_widget.set_from_stock(Gtk.Stock.DIALOG_WARNING, Gtk.IconSize.DIALOG);
		if(job.is_cancelled()) {
			msg_label.set_text(_("The file operation is cancelled and there are some errors."));
			dlg.set_title(_("Cancelled"));
		}
		else {
			msg_label.set_text(_("The file operation is finished, but there are some errors."));
			dlg.set_title(_("Finished"));
		}
	}

	// called by FileJob to inform the end of the job
	// though FileJob emits "finished" signal, it's easier to just call a function
	public void finish() {
		if(dlg != null) {
			// only destroy the dialog if there are no errors to shown
			if(error_pane.get_visible() == true) {
				finish_show_error();
			}
			else {
				dlg.destroy();
				dlg = null;
			}
		}

		if(show_dlg_timeout_id != 0) {
			Source.remove(show_dlg_timeout_id);
			show_dlg_timeout_id = 0;
		}

		// the job will be freed after we return, so don't reference it anymore.
		job = null;
	}

	// called by FileJob to show total amount of the job, may be called for many times
	public void set_total_amount(uint64 size, int n_files, int n_dirs) {
		// total_size = size;
		// n_total_files = n_files;
		// n_total_dirs = n_dirs;
	}

	// get prepared and ready to do the file operation
	public void set_ready() {
	}

	// progress info, in %
	public void set_percent(int percent) {
		if(progress_bar != null) {
			progress_bar.set_fraction((double)percent/100);
			progress_bar.set_text("%d %%".printf(percent));
		}
		this.percent = percent;
	}

	// estimated time
	public void set_times(uint elapsed, uint remaining) {
		if(remaining_time_label != null) {
			char time_str[32];
			uint secs = remaining;
			uint mins = 0;
			uint hrs = 0;
			if(secs > 60)
			{
				mins = secs / 60;
				secs %= 60;
				if(mins > 60)
				{
					hrs = mins / 60;
					mins %= 60;
				}
			}
			remaining_time_label.set_text("%02u:%02u:%02u".printf(hrs, mins, secs));
		}
	}

	// set src/dest paths currently being handled
	public void set_current_src_dest(Path src_path, Path? dest_path = null) {
		// stdout.printf("%s\n", src_path.display_name(true));
		current_src_path = src_path;
		current_dest_path = dest_path;
		if(src_path_label != null && src_path != null)
			src_path_label.set_label(src_path.display_name(true));
		if(dest_path_label != null && dest_path != null)
			dest_path_label.set_label(dest_path.display_name(true));
	}

	// set currently processed child file/folder and optionally, its dest
	public void set_currently_processed(File src, GLib.FileInfo? src_info, File? dest = null) {
		// consider showing basename here
		current_src_file = src;
		current_src_info = src_info;
		current_dest_file = dest;
		if(current_file_label != null && src_info != null) {
			current_file_label.set_label(src_info.get_display_name());
		}
	}

	// ask for a new file name
	public RenameResult ask_rename(File src_file, GLib.FileInfo src_info, File dest_file, GLib.FileInfo dest_info, out File new_dest) {
		new_dest = null;
		// if we have default action, don't bother the user with further dialogs
		if(default_rename_result == RenameResult.OVERWRITE ||
			default_rename_result == RenameResult.SKIP)
			return default_rename_result;

		show_dialog(); // show the progress dialog and use it as the parent window

		var rename_dlg = new RenameDialog(dlg);
		if(src_info != null) { // display info of the source file
			var src_path = new Path.for_gfile(src_file);
			var fi = new Fm.FileInfo.from_gfileinfo(src_path, src_info);
			rename_dlg.set_src_info(fi);
		}
		if(dest_info != null) { // display info of the destination file which already exists
			var dest_path = new Path.for_gfile(dest_file);
			var fi = new Fm.FileInfo.from_gfileinfo(dest_path, dest_info);
			rename_dlg.set_dest_info(fi);
		}

		// show a dialog for the user to enter a new filename
		var result = rename_dlg.run();
		if(result == RenameResult.RENAME) {
			var new_name = rename_dlg.get_new_name();
			if(new_name != null)
				new_dest = dest_file.get_parent().get_child(new_name);
		}
		// if the user choose to apply the option to all existing files, set it default
		if(rename_dlg.get_apply_to_all())
			default_rename_result = result;

		return result;
	}

	public ErrorAction handle_error(Error err, Severity severity) {
		// ensure the dialog is visible
		show_dialog();

		if(!error_pane.get_visible())
			error_pane.show();

		// append the error message
		Gtk.TextIter it;
		error_buf.get_end_iter(out it);
		if(current_src_info != null) {
			var filename = current_src_info.get_display_name();
			error_buf.insert_with_tags(it, filename, -1, bold_tag, null);
			error_buf.get_end_iter(out it);
			error_buf.insert_with_tags(it, ": ", 2, bold_tag, null);
			error_buf.get_end_iter(out it);
		}
		error_buf.insert(ref it, err.message, -1);
		error_buf.get_end_iter(out it);
		error_buf.insert(ref it, "\n", 1);
		// debug("error: %s\n", err.message);
		return ErrorAction.CONTINUE;
	}

	private void on_dlg_response(Gtk.Dialog dialog, int response) {
		if(response == Gtk.ResponseType.CANCEL || response == Gtk.ResponseType.DELETE_EVENT)
		{
			if(job != null) {
				job.cancel();
			}
			dlg.destroy();
			dlg = null;
		}
		else if(response == Gtk.ResponseType.CLOSE) {
			dlg.destroy();
			dlg = null;
		}
	}

	private bool on_show_dlg_timeout() {
		show_dlg_timeout_id = 0;
		show_dialog();
		return false;
	}

	private void show_dialog() {
		if(show_dlg_timeout_id != 0) {
			Source.remove(show_dlg_timeout_id);
			show_dlg_timeout_id = 0;
		}

		if(dlg != null) {
			dlg.show();
			return;
		}

		var builder = new Gtk.Builder();
		
        //const string ui_file = Config.PACKAGE_UI_DIR + "/progress.ui";
		
        // !!!! builder.set_translation_domain(Config.GETTEXT_PACKAGE);
		
        builder.add_from_string(PROGRESS_XML, -1);
		
        dlg = (Gtk.Dialog)builder.get_object("dlg");
		dlg.response.connect(on_dlg_response);

		var dest_label = (Gtk.Label)builder.get_object("to_label");
		dest_path_label = (Gtk.Label)builder.get_object("dest");
		icon_widget = (Gtk.Image)builder.get_object("icon");
		msg_label = (Gtk.Label)builder.get_object("msg");
		var action_label = (Gtk.Label)builder.get_object("action");
		src_path_label = (Gtk.Label)builder.get_object("src");
		current_file_label = (Gtk.Label)builder.get_object("current");
		progress_bar = (Gtk.ProgressBar)builder.get_object("progress");
		error_pane = (Gtk.VBox)builder.get_object("error_pane");
		error_msg_text_view = (Gtk.TextView)builder.get_object("error_msg");
		remaining_time_label = (Gtk.Label)builder.get_object("remaining_time");

		var tag_table = new Gtk.TextTagTable();
		bold_tag = new Gtk.TextTag("bold");
		bold_tag.weight = Pango.Weight.BOLD;
		tag_table.add(bold_tag);
		error_buf = new Gtk.TextBuffer(tag_table);
		error_msg_text_view.set_buffer(error_buf);

		// set the src & dest label
		dlg.set_title(title);
		action_label.set_markup(@"<b>$action_label_text</b>");
		if(!has_dest) {
			dest_label.destroy();
			dest_path_label.destroy();
			dest_path_label = null;
		}
		if(current_src_path != null)
			set_current_src_dest(current_src_path, current_dest_path);
		if(current_src_file != null && current_src_info != null)
			set_currently_processed(current_src_file, current_src_info, current_dest_file);
		if(percent > 0.0)
			set_percent(percent);

		dlg.present();

		// When the GUI is shown to the user, we need to make sure that 
		// the UI object is still reachable. Otherwise errors can occur.
		// So here we add a reference to UI object for dlg.
		// Doing manual ref count is very terrible in Vala.
		// If someone knows a better solution, please fix this.
		ref(); // add ref count of UI object for the dialog
		dlg.destroy.connect(() => {
			dlg = null;
			unref(); // the dialog is destroyed, free the UI object, too.
		});
	}

	private string title;
	private string action_label_text;
	private bool has_dest;
	private int percent;
	private Path current_src_path;
	private Path? current_dest_path;
	private File current_src_file;
	private GLib.FileInfo? current_src_info;
	private File? current_dest_file;

	private uint64 total_size;
	private int total_n_files;
	private int total_n_dirs;
	private time_t time_remaining;
	
	private RenameResult default_rename_result;

	private unowned Gtk.Dialog? dlg;
	private unowned Gtk.Label dest_path_label;
	private unowned Gtk.Label msg_label;
	private unowned Gtk.Image icon_widget;
	private unowned Gtk.Label src_path_label;
	private unowned Gtk.Label current_file_label;
	private unowned Gtk.ProgressBar progress_bar;
	private unowned Gtk.VBox error_pane;
	private unowned Gtk.TextView error_msg_text_view;
	private unowned Gtk.Label remaining_time_label;
	private Gtk.TextTag bold_tag;
	private Gtk.TextBuffer error_buf;

	private uint show_dlg_timeout_id;

	private unowned FileJob? job;
}

}
