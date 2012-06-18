//      
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

    public const string PROGRESS_XML = """
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




}