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

    public const string ASK_RENAME_XML = """
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



}
