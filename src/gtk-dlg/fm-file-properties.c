/***********************************************************************************************************************
 * 
 *      fm-file-properties.c
 *
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
 *      Copyright 2012 Axel FILMORE <axel.filmore@gmail.com>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *       (at your option) any later version.
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
#include <config.h>
#include <glib/gi18n-lib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include <ctype.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>

#include "fm-file-info-list.h"
#include "fm-file-properties.h"
#include "fm-deep-count-job.h"
#include "fm-file-ops-job.h"
#include "fm-utils.h"
#include "fm-dlg-utils.h"
#include "fm-path.h"

#include "fm-progress-dlg.h"
#include "fm-utils.h"

#include "fm-app-chooser-combo-box.h"

#define GET_WIDGET(name) data->name = (GtkWidget*) gtk_builder_get_object (builder, #name);

//extern const char PROPERTIES_DLG[];
/***********************************************************************************************************************
 * File Properties Dialog
 * 
 * 
 **********************************************************************************************************************/
const char PROPERTIES_DLG [] =
"<?xml version='1.0' encoding='UTF-8'?>"
"<interface>"
"<!-- interface-requires gtk+ 3.0 -->"
"<object class='GtkDialog' id='dlg'>"
"<property name='border_width'>6</property>"
"<property name='title' translatable='yes'>File Properties</property>"
"<property name='resizable'>False</property>"
"<property name='window_position'>center-on-parent</property>"
"<property name='default_width'>360</property>"
"<property name='type_hint'>dialog</property>"
"<property name='gravity'>center</property>"
"<signal name='response' handler='on_filePropertiesDlg_response'/>"
"<child internal-child='vbox'>"
"<object class='GtkVBox' id='dialog_vbox'>"
"<property name='visible'>True</property>"
"<property name='orientation'>vertical</property>"
"<property name='spacing'>12</property>"
"<child>"
"<object class='GtkNotebook' id='notebook'>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='border_width'>6</property>"
"<child>"
"<object class='GtkAlignment' id='alignment2'>"
"<property name='visible'>True</property>"
"<property name='top_padding'>6</property>"
"<property name='bottom_padding'>6</property>"
"<property name='left_padding'>12</property>"
"<child>"
"<object class='GtkTable' id='general_table'>"
"<property name='visible'>True</property>"
"<property name='border_width'>4</property>"
"<property name='n_rows'>9</property>"
"<property name='n_columns'>2</property>"
"<property name='column_spacing'>12</property>"
"<property name='row_spacing'>6</property>"
"<child>"
"<object class='GtkLabel' id='type'>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='xalign'>0</property>"
"<property name='wrap'>True</property>"
"<property name='selectable'>True</property>"
"<property name='ellipsize'>end</property>"
"</object>"
"<packing>"
"<property name='left_attach'>1</property>"
"<property name='right_attach'>2</property>"
"<property name='top_attach'>3</property>"
"<property name='bottom_attach'>4</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='label4'>"
"<property name='visible'>True</property>"
"<property name='xalign'>0</property>"
"<property name='label' translatable='yes'>&lt;b&gt;File Type:&lt;/b&gt;</property>"
"<property name='use_markup'>True</property>"
"<property name='use_underline'>True</property>"
"</object>"
"<packing>"
"<property name='top_attach'>3</property>"
"<property name='bottom_attach'>4</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='label5'>"
"<property name='visible'>True</property>"
"<property name='xalign'>0</property>"
"<property name='label' translatable='yes'>&lt;b&gt;Total Size of Files:&lt;/b&gt;</property>"
"<property name='use_markup'>True</property>"
"<property name='use_underline'>True</property>"
"</object>"
"<packing>"
"<property name='top_attach'>5</property>"
"<property name='bottom_attach'>6</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='label7'>"
"<property name='visible'>True</property>"
"<property name='xalign'>0</property>"
"<property name='label' translatable='yes'>&lt;b&gt;Last Modification:&lt;/b&gt;</property>"
"<property name='use_markup'>True</property>"
"<property name='use_underline'>True</property>"
"</object>"
"<packing>"
"<property name='top_attach'>7</property>"
"<property name='bottom_attach'>8</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='total_size'>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='xalign'>0</property>"
"<property name='selectable'>True</property>"
"</object>"
"<packing>"
"<property name='left_attach'>1</property>"
"<property name='right_attach'>2</property>"
"<property name='top_attach'>5</property>"
"<property name='bottom_attach'>6</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='mtime'>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='xalign'>0</property>"
"<property name='selectable'>True</property>"
"</object>"
"<packing>"
"<property name='left_attach'>1</property>"
"<property name='right_attach'>2</property>"
"<property name='top_attach'>7</property>"
"<property name='bottom_attach'>8</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='label13'>"
"<property name='visible'>True</property>"
"<property name='xalign'>0</property>"
"<property name='label' translatable='yes'>&lt;b&gt;Last Access:&lt;/b&gt;</property>"
"<property name='use_markup'>True</property>"
"<property name='use_underline'>True</property>"
"</object>"
"<packing>"
"<property name='top_attach'>8</property>"
"<property name='bottom_attach'>9</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='atime'>"
"<property name='visible'>True</property>"
"<property name='xalign'>0</property>"
"</object>"
"<packing>"
"<property name='left_attach'>1</property>"
"<property name='right_attach'>2</property>"
"<property name='top_attach'>8</property>"
"<property name='bottom_attach'>9</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='label20'>"
"<property name='visible'>True</property>"
"<property name='xalign'>0</property>"
"<property name='label' translatable='yes'>&lt;b&gt;Location:&lt;/b&gt;</property>"
"<property name='use_markup'>True</property>"
"<property name='use_underline'>True</property>"
"</object>"
"<packing>"
"<property name='top_attach'>1</property>"
"<property name='bottom_attach'>2</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='dir'>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='xalign'>0</property>"
"<property name='selectable'>True</property>"
"<property name='ellipsize'>middle</property>"
"</object>"
"<packing>"
"<property name='left_attach'>1</property>"
"<property name='right_attach'>2</property>"
"<property name='top_attach'>1</property>"
"<property name='bottom_attach'>2</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkEntry' id='name'>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"</object>"
"<packing>"
"<property name='left_attach'>1</property>"
"<property name='right_attach'>2</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='label21'>"
"<property name='visible'>True</property>"
"<property name='xalign'>0</property>"
"<property name='label' translatable='yes'>&lt;b&gt;Size on Disk:&lt;/b&gt;</property>"
"<property name='use_markup'>True</property>"
"<property name='use_underline'>True</property>"
"</object>"
"<packing>"
"<property name='top_attach'>6</property>"
"<property name='bottom_attach'>7</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='size_on_disk'>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='xalign'>0</property>"
"<property name='selectable'>True</property>"
"</object>"
"<packing>"
"<property name='left_attach'>1</property>"
"<property name='right_attach'>2</property>"
"<property name='top_attach'>6</property>"
"<property name='bottom_attach'>7</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='open_with_label'>"
"<property name='visible'>True</property>"
"<property name='xalign'>0</property>"
"<property name='label' translatable='yes'>&lt;b&gt;Open _with:&lt;/b&gt;</property>"
"<property name='use_markup'>True</property>"
"<property name='use_underline'>True</property>"
"<property name='mnemonic_widget'>open_with</property>"
"</object>"
"<packing>"
"<property name='top_attach'>4</property>"
"<property name='bottom_attach'>5</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkComboBox' id='open_with'>"
"<property name='visible'>True</property>"
"<property name='tooltip_text' translatable='yes'>Select the application used to open files of this type </property>"
"</object>"
"<packing>"
"<property name='left_attach'>1</property>"
"<property name='right_attach'>2</property>"
"<property name='top_attach'>4</property>"
"<property name='bottom_attach'>5</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'>GTK_FILL</property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='target_label'>"
"<property name='visible'>True</property>"
"<property name='xalign'>0</property>"
"<property name='label' translatable='yes'>&lt;b&gt;Target:&lt;/b&gt;</property>"
"<property name='use_markup'>True</property>"
"<property name='use_underline'>True</property>"
"</object>"
"<packing>"
"<property name='top_attach'>2</property>"
"<property name='bottom_attach'>3</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='target'>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='xalign'>0</property>"
"<property name='selectable'>True</property>"
"<property name='ellipsize'>middle</property>"
"</object>"
"<packing>"
"<property name='left_attach'>1</property>"
"<property name='right_attach'>2</property>"
"<property name='top_attach'>2</property>"
"<property name='bottom_attach'>3</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkImage' id='icon'>"
"<property name='visible'>True</property>"
"<property name='stock'>gtk-missing-image</property>"
"<property name='icon-size'>6</property>"
"</object>"
"<packing>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'>GTK_FILL</property>"
"</packing>"
"</child>"
"</object>"
"</child>"
"</object>"
"</child>"
"<child type='tab'>"
"<object class='GtkLabel' id='label1'>"
"<property name='visible'>True</property>"
"<property name='label' translatable='yes'>_General</property>"
"<property name='use_underline'>True</property>"
"</object>"
"<packing>"
"<property name='tab_fill'>False</property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkAlignment' id='alignment1'>"
"<property name='visible'>True</property>"
"<property name='top_padding'>6</property>"
"<property name='bottom_padding'>6</property>"
"<property name='left_padding'>12</property>"
"<child>"
"<object class='GtkVBox' id='vbox1'>"
"<property name='visible'>True</property>"
"<property name='orientation'>vertical</property>"
"<property name='spacing'>6</property>"
"<child>"
"<object class='GtkTable' id='table3'>"
"<property name='visible'>True</property>"
"<property name='border_width'>2</property>"
"<property name='n_rows'>2</property>"
"<property name='n_columns'>2</property>"
"<property name='column_spacing'>12</property>"
"<property name='row_spacing'>6</property>"
"<child>"
"<object class='GtkLabel' id='owner_label'>"
"<property name='visible'>True</property>"
"<property name='xalign'>0</property>"
"<property name='label' translatable='yes'>&lt;b&gt;_Owner:&lt;/b&gt;</property>"
"<property name='use_markup'>True</property>"
"<property name='use_underline'>True</property>"
"</object>"
"<packing>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='group_label'>"
"<property name='visible'>True</property>"
"<property name='xalign'>0</property>"
"<property name='label' translatable='yes'>&lt;b&gt;_Group:&lt;/b&gt;</property>"
"<property name='use_markup'>True</property>"
"<property name='use_underline'>True</property>"
"</object>"
"<packing>"
"<property name='top_attach'>1</property>"
"<property name='bottom_attach'>2</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'></property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkEntry' id='owner'>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='invisible_char'>&#x2022;</property>"
"</object>"
"<packing>"
"<property name='left_attach'>1</property>"
"<property name='right_attach'>2</property>"
"<property name='y_options'>GTK_FILL</property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkEntry' id='group'>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='invisible_char'>&#x2022;</property>"
"</object>"
"<packing>"
"<property name='left_attach'>1</property>"
"<property name='right_attach'>2</property>"
"<property name='top_attach'>1</property>"
"<property name='bottom_attach'>2</property>"
"<property name='y_options'>GTK_FILL</property>"
"</packing>"
"</child>"
"</object>"
"<packing>"
"<property name='expand'>False</property>"
"<property name='fill'>False</property>"
"<property name='position'>0</property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkFrame' id='frame1'>"
"<property name='visible'>True</property>"
"<property name='label_xalign'>0</property>"
"<property name='shadow_type'>none</property>"
"<child>"
"<object class='GtkAlignment' id='alignment3'>"
"<property name='visible'>True</property>"
"<property name='left_padding'>12</property>"
"<child>"
"<object class='GtkTable' id='table1'>"
"<property name='visible'>True</property>"
"<property name='border_width'>6</property>"
"<property name='n_rows'>4</property>"
"<property name='n_columns'>2</property>"
"<property name='column_spacing'>12</property>"
"<property name='row_spacing'>6</property>"
"<child>"
"<object class='GtkLabel' id='label6'>"
"<property name='visible'>True</property>"
"<property name='xalign'>0</property>"
"<property name='label' translatable='yes'>&lt;b&gt;Owner:&lt;/b&gt;</property>"
"<property name='use_markup'>True</property>"
"</object>"
"<packing>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'>GTK_FILL</property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='label8'>"
"<property name='visible'>True</property>"
"<property name='xalign'>0</property>"
"<property name='label' translatable='yes'>&lt;b&gt;Group:&lt;/b&gt;</property>"
"<property name='use_markup'>True</property>"
"</object>"
"<packing>"
"<property name='top_attach'>1</property>"
"<property name='bottom_attach'>2</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'>GTK_FILL</property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='label9'>"
"<property name='visible'>True</property>"
"<property name='xalign'>0</property>"
"<property name='label' translatable='yes'>&lt;b&gt;Other:&lt;/b&gt;</property>"
"<property name='use_markup'>True</property>"
"</object>"
"<packing>"
"<property name='top_attach'>2</property>"
"<property name='bottom_attach'>3</property>"
"<property name='x_options'>GTK_FILL</property>"
"<property name='y_options'>GTK_FILL</property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkComboBox' id='owner_perm'>"
"<property name='visible'>True</property>"
"<property name='tooltip_text' translatable='yes'>Operations that the owner of the file can do</property>"
"<property name='model'>access_model</property>"
"<child>"
"<object class='GtkCellRendererText' id='cellrenderertext1'/>"
"<attributes>"
"<attribute name='text'>0</attribute>"
"</attributes>"
"</child>"
"</object>"
"<packing>"
"<property name='left_attach'>1</property>"
"<property name='right_attach'>2</property>"
"<property name='y_options'>GTK_FILL</property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkComboBox' id='group_perm'>"
"<property name='visible'>True</property>"
"<property name='tooltip_text' translatable='yes'>Operations that users in the same group as the owner of the file can do</property>"
"<property name='model'>access_model</property>"
"<child>"
"<object class='GtkCellRendererText' id='cellrenderertext2'/>"
"<attributes>"
"<attribute name='text'>0</attribute>"
"</attributes>"
"</child>"
"</object>"
"<packing>"
"<property name='left_attach'>1</property>"
"<property name='right_attach'>2</property>"
"<property name='top_attach'>1</property>"
"<property name='bottom_attach'>2</property>"
"<property name='y_options'>GTK_FILL</property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkComboBox' id='other_perm'>"
"<property name='visible'>True</property>"
"<property name='tooltip_text' translatable='yes'>Operations that all other users can do</property>"
"<property name='model'>access_model</property>"
"<child>"
"<object class='GtkCellRendererText' id='cellrenderertext3'/>"
"<attributes>"
"<attribute name='text'>0</attribute>"
"</attributes>"
"</child>"
"</object>"
"<packing>"
"<property name='left_attach'>1</property>"
"<property name='right_attach'>2</property>"
"<property name='top_attach'>2</property>"
"<property name='bottom_attach'>3</property>"
"<property name='y_options'>GTK_FILL</property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkCheckButton' id='exec'>"
"<property name='label' translatable='yes'>Make the file executable</property>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='receives_default'>False</property>"
"<property name='draw_indicator'>True</property>"
"</object>"
"<packing>"
"<property name='right_attach'>2</property>"
"<property name='top_attach'>3</property>"
"<property name='bottom_attach'>4</property>"
"<property name='y_options'>GTK_FILL</property>"
"</packing>"
"</child>"
"</object>"
"</child>"
"</object>"
"</child>"
"<child type='label'>"
"<object class='GtkLabel' id='label3'>"
"<property name='visible'>True</property>"
"<property name='label' translatable='yes'>&lt;b&gt;Access Control&lt;/b&gt;</property>"
"<property name='use_markup'>True</property>"
"</object>"
"</child>"
"</object>"
"<packing>"
"<property name='expand'>False</property>"
"<property name='position'>1</property>"
"</packing>"
"</child>"
"</object>"
"</child>"
"</object>"
"<packing>"
"<property name='position'>1</property>"
"</packing>"
"</child>"
"<child type='tab'>"
"<object class='GtkLabel' id='label2'>"
"<property name='visible'>True</property>"
"<property name='label' translatable='yes'>_Permissions</property>"
"<property name='use_underline'>True</property>"
"</object>"
"<packing>"
"<property name='position'>1</property>"
"<property name='tab_fill'>False</property>"
"</packing>"
"</child>"
"</object>"
"<packing>"
"<property name='position'>2</property>"
"</packing>"
"</child>"
"<child internal-child='action_area'>"
"<object class='GtkHButtonBox' id='dialog_action_area'>"
"<property name='visible'>True</property>"
"<property name='layout_style'>end</property>"
"<child>"
"<object class='GtkButton' id='cancel'>"
"<property name='label'>gtk-cancel</property>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='can_default'>True</property>"
"<property name='receives_default'>False</property>"
"<property name='use_stock'>True</property>"
"<signal name='clicked' handler='gtk_widget_destroy' object='filePropertiesDlg'/>"
"</object>"
"<packing>"
"<property name='expand'>False</property>"
"<property name='fill'>False</property>"
"<property name='position'>0</property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkButton' id='ok'>"
"<property name='label'>gtk-ok</property>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='can_default'>True</property>"
"<property name='has_default'>True</property>"
"<property name='receives_default'>False</property>"
"<property name='use_stock'>True</property>"
"<signal name='clicked' handler='gtk_widget_destroy' object='filePropertiesDlg'/>"
"</object>"
"<packing>"
"<property name='expand'>False</property>"
"<property name='fill'>False</property>"
"<property name='position'>1</property>"
"</packing>"
"</child>"
"</object>"
"<packing>"
"<property name='expand'>False</property>"
"<property name='pack_type'>end</property>"
"<property name='position'>0</property>"
"</packing>"
"</child>"
"</object>"
"</child>"
"<action-widgets>"
"<action-widget response='-6'>cancel</action-widget>"
"<action-widget response='-5'>ok</action-widget>"
"</action-widgets>"
"</object>"
"<object class='GtkListStore' id='access_model'>"
"<columns>"
"<!-- column-name Access -->"
"<column type='gchararray'/>"
"</columns>"
"<data>"
"<row>"
"<col id='0' translatable='yes'>Read and Write</col>"
"</row>"
"<row>"
"<col id='0' translatable='yes'>Read only</col>"
"</row>"
"<row>"
"<col id='0' translatable='yes'>Write only</col>"
"</row>"
"<row>"
"<col id='0' translatable='yes'>None</col>"
"</row>"
"<row>"
"<col id='0' translatable='yes'>---</col>"
"</row>"
"</data>"
"</object>"
"</interface>";



enum {
    READ_WRITE,
    READ_ONLY,
    WRITE_ONLY,
    NONE,
    NO_CHANGE
};

typedef struct _FmFilePropData FmFilePropData;
struct _FmFilePropData
{
    GtkWidget *dlg;

    // General page
    GtkWidget *icon;
    GtkWidget *name;
    GtkWidget *dir;
    GtkWidget *target;
    GtkWidget *target_label;
    GtkWidget *type;
    GtkWidget *open_with_label;
    GtkWidget *open_with;
    GtkWidget *total_size;
    GtkWidget *size_on_disk;
    GtkWidget *mtime;
    GtkWidget *atime;

    // Permissions page
    GtkWidget *owner;
    char *orig_owner;
    GtkWidget *group;
    char *orig_group;
    GtkWidget *owner_perm;
    int owner_perm_sel;
    GtkWidget *group_perm;
    int group_perm_sel;
    GtkWidget *other_perm;
    int other_perm_sel;
    GtkWidget *exec;
    int exec_state;

    FmFileInfoList *files;
    FmFileInfo *file_info;
    gboolean single_type;
    gboolean single_file;
    gboolean all_native;
    gboolean has_dir;
    FmMimeType *mime_type;

    uid_t uid;
    gid_t gid;

    guint timeout;
    FmJob *dc_job;
};


static gboolean on_timeout (FmFilePropData *data)
{
    char size_str[128];
    FmDeepCountJob *dc = (FmDeepCountJob*) data->dc_job;

    //gdk_threads_enter ();

    if (G_LIKELY (dc && !fm_job_is_cancelled (FM_JOB (dc))))
    {
        char *str;
        fm_file_size_to_str (size_str, dc->total_size, TRUE);
        str = g_strdup_printf ("%s  (%'llu %s)", size_str, dc->total_size, ngettext ("byte", "bytes", dc->total_size));
        gtk_label_set_text (GTK_LABEL (data->total_size), str);
        g_free (str);

        fm_file_size_to_str (size_str, dc->total_block_size, TRUE);
        str = g_strdup_printf ("%s  (%'llu %s)", size_str, dc->total_block_size, ngettext ("byte", "bytes", dc->total_block_size));
        gtk_label_set_text (GTK_LABEL (data->size_on_disk), str);
        g_free (str);
    }
    //gdk_threads_leave ();
    return TRUE;
}

static void on_finished (FmDeepCountJob *job, FmFilePropData *data)
{
    on_timeout (data); // update display
    if (data->timeout)
    {
        g_source_remove (data->timeout);
        data->timeout = 0;
    }
    data->dc_job = NULL;
}

static void fm_file_prop_data_free (FmFilePropData *data)
{
    g_free (data->orig_owner);
    g_free (data->orig_group);

    if (data->timeout)
        g_source_remove (data->timeout);
    if (data->dc_job) // FIXME_pcm: check if it's running
        fm_job_cancel (data->dc_job);
    fm_list_unref (data->files);
    g_slice_free (FmFilePropData, data);
}

static gboolean ensure_valid_owner (FmFilePropData *data)
{
    gboolean ret = TRUE;
    const char *tmp = gtk_entry_get_text (GTK_ENTRY (data->owner));

    data->uid = -1;
    if (tmp && *tmp)
    {
        if (data->all_native && !isdigit (tmp[0])) // entering names instead of numbers is only allowed for local files.
        {
            struct passwd *pw;
            if (!tmp || !*tmp || ! (pw = getpwnam (tmp)))
                ret = FALSE;
            else
                data->uid = pw->pw_uid;
        }
    }
    else
        ret = FALSE;

    if (!ret)
    {
        fm_show_error (GTK_WINDOW (data->dlg), NULL, _ ("Please enter a valid user name or numeric id."));
        gtk_widget_grab_focus (data->owner);
    }

    if (data->uid == -1)
        data->uid = atoi (tmp);

    return ret;
}

static gboolean ensure_valid_group (FmFilePropData *data)
{
    gboolean ret = TRUE;
    const char *tmp = gtk_entry_get_text (GTK_ENTRY (data->group));

    data->gid = -1;
    if (tmp && *tmp)
    {
        if (data->all_native && !isdigit (tmp[0])) // entering names instead of numbers is only allowed for local files.
        {
            struct group *gr;
            if (!tmp || !*tmp || ! (gr = getgrnam (tmp)))
                ret = FALSE;
            else
                data->gid = gr->gr_gid;
        }
    }
    else
        ret = FALSE;

    if (!ret)
    {
        fm_show_error (GTK_WINDOW (data->dlg), NULL, _ ("Please enter a valid group name or numeric id."));
        gtk_widget_grab_focus (data->group);
    }
    if (data->gid == -1)
        data->gid = atoi (tmp);

    return ret;
}


static void on_response (GtkDialog *dlg, int response, FmFilePropData *data)
{
    if ( response == GTK_RESPONSE_OK )
    {
        int sel;
        const char *new_owner = gtk_entry_get_text (GTK_ENTRY (data->owner));
        const char *new_group = gtk_entry_get_text (GTK_ENTRY (data->group));
        guint32 uid = -1, gid = -1;
        mode_t new_mode = 0, new_mode_mask = 0;

        if (!ensure_valid_owner (data) || !ensure_valid_group (data))
        {
            g_signal_stop_emission_by_name (dlg, "response");
            return;
        }

        /* FIXME_pcm: if all files are native, it's possible to check
         * if the names are legal user and group names on the local
         * machine prior to chown. */
        if (new_owner && *new_owner && g_strcmp0 (data->orig_owner, new_owner))
        {
            // change owner
            g_debug ("change owner to: %d", data->uid);
        }
        else
            data->uid = -1;

        if (new_group && *new_group && g_strcmp0 (data->orig_group, new_group))
        {
            // change group
            g_debug ("change group to: %d", data->gid);
        }
        else
            data->gid = -1;

        // check if chmod is needed here.
        sel = gtk_combo_box_get_active (GTK_COMBO_BOX (data->owner_perm));
        if ( sel != NO_CHANGE ) // need to change owner permission
        {
            if (data->owner_perm_sel != sel) // new value is different from original
            {
                new_mode_mask |= S_IRUSR|S_IWUSR;
                data->owner_perm_sel = sel;
                switch (sel)
                {
                case READ_WRITE:
                    new_mode |= S_IRUSR|S_IWUSR;
                    break;
                case READ_ONLY:
                    new_mode |= S_IRUSR;
                    break;
                case WRITE_ONLY:
                    new_mode |= S_IWUSR;
                    break;
                }
            }
            else // otherwise, no change
                data->owner_perm_sel = NO_CHANGE;
        }
        else
            data->owner_perm_sel = NO_CHANGE;

        sel = gtk_combo_box_get_active (GTK_COMBO_BOX (data->group_perm));
        if ( sel != NO_CHANGE ) // need to change group permission
        {
            if (data->group_perm_sel != sel) // new value is different from original
            {
                new_mode_mask |= S_IRGRP|S_IWGRP;
                data->group_perm_sel = sel;
                switch (sel)
                {
                case READ_WRITE:
                    new_mode |= S_IRGRP|S_IWGRP;
                    break;
                case READ_ONLY:
                    new_mode |= S_IRGRP;
                    break;
                case WRITE_ONLY:
                    new_mode |= S_IWGRP;
                    break;
                }
            }
            else // otherwise, no change
                data->group_perm_sel = NO_CHANGE;
        }
        else
            data->group_perm_sel = NO_CHANGE;

        sel = gtk_combo_box_get_active (GTK_COMBO_BOX (data->other_perm));
        if ( sel != NO_CHANGE ) // need to change other permission
        {
            if (data->other_perm_sel != sel) // new value is different from original
            {
                new_mode_mask |= S_IROTH|S_IWOTH;
                switch (sel)
                {
                case READ_WRITE:
                    new_mode |= S_IROTH|S_IWOTH;
                    break;
                case READ_ONLY:
                    new_mode |= S_IROTH;
                    break;
                case WRITE_ONLY:
                    new_mode |= S_IWOTH;
                    break;
                }
                data->other_perm_sel = sel;
            }
            else // otherwise, no change
                data->other_perm_sel = NO_CHANGE;
        }
        else
            data->other_perm_sel = NO_CHANGE;

        if (!data->has_dir
           && !gtk_toggle_button_get_inconsistent (GTK_TOGGLE_BUTTON (data->exec))
           && gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (data->exec)) != data->exec_state)
        {
            new_mode_mask |=  (S_IXUSR|S_IXGRP|S_IXOTH);
            if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (data->exec)))
                new_mode |=  (S_IXUSR|S_IXGRP|S_IXOTH);
        }

        if (new_mode_mask || data->uid != -1 || data->gid != -1)
        {
            FmPathList *paths = fm_path_list_new_from_file_info_list (data->files);
            FmFileOpsJob *job = (FmFileOpsJob*) fm_file_ops_job_new (FM_FILE_OP_CHANGE_ATTR, paths);

            // need to chown
            if (data->uid != -1 || data->gid != -1)
                fm_file_ops_job_set_chown (job, data->uid, data->gid);

            // need to do chmod
            if (new_mode_mask)
                fm_file_ops_job_set_chmod (job, new_mode, new_mode_mask);

            if (data->has_dir)
            {
                if (fm_yes_no (GTK_WINDOW (data->dlg), NULL, _ ( "Do you want to recursively apply these changes to all files and sub-folders?" ), TRUE))
                    fm_file_ops_job_set_recursive (job, TRUE);
            }

            // show progress dialog
            fm_file_ops_job_run_with_progress (GTK_WINDOW (data->dlg), job);
            fm_list_unref (paths);
        }

        // change default application for the mime-type if needed
        if (data->mime_type && data->mime_type->type && data->open_with)
        {
            GAppInfo *app;
            gboolean default_app_changed = FALSE;
            GError *err = NULL;
            app = fm_app_chooser_combo_box_get_selected (GTK_COMBO_BOX (data->open_with), &default_app_changed);
            if (app)
            {
                if (default_app_changed)
                {
                    g_app_info_set_as_default_for_type (app, data->mime_type->type, &err);
                    if (err)
                    {
                        fm_show_error (GTK_WINDOW (dlg), NULL, err->message);
                        g_error_free (err);
                    }
                }
                g_object_unref (app);
            }
        }

        if (data->single_file) // when only one file is shown
        {
            // if the user has changed its name
            if ( g_strcmp0 (data->file_info->disp_name, gtk_entry_get_text (GTK_ENTRY (data->name))) )
            {
                // FIXME_pcm: rename the file or set display name for it.
            }
        }
    }
    gtk_widget_destroy (GTK_WIDGET (dlg));
}

static void on_exec_toggled (GtkToggleButton *btn, FmFilePropData *data)
{
    // Bypass the default handler
    g_signal_stop_emission_by_name ( btn, "toggled" );
    /* Block this handler while we are changing the state of buttons,
      or this handler will be called recursively. */
    g_signal_handlers_block_matched ( btn, G_SIGNAL_MATCH_FUNC, 0,
                                     0, NULL, on_exec_toggled, NULL );

    if ( gtk_toggle_button_get_inconsistent ( btn ) )
    {
        gtk_toggle_button_set_inconsistent ( btn, FALSE );
        gtk_toggle_button_set_active ( btn, TRUE );
    }
    else if ( gtk_toggle_button_get_active ( btn ) )
    {
        gtk_toggle_button_set_inconsistent ( btn, TRUE );
    }

    g_signal_handlers_unblock_matched ( btn, G_SIGNAL_MATCH_FUNC, 0,
                                       0, NULL, on_exec_toggled, NULL );
}

// FIXME_pcm: this is too dirty. Need some refactor later.
static void update_permissions (FmFilePropData *data)
{
    FmFileInfo *file_info =  (FmFileInfo*)fm_list_peek_head (data->files);
    GList *l;
    int sel;
    char *tmp;
    mode_t owner_perm =  (file_info->mode & S_IRWXU);
    mode_t group_perm =  (file_info->mode & S_IRWXG);
    mode_t other_perm =  (file_info->mode & S_IRWXO);
    mode_t exec_perm =  (file_info->mode &  (S_IXUSR|S_IXGRP|S_IXOTH));
    uid_t uid = file_info->uid;
    gid_t gid = file_info->gid;
    struct group *grp = NULL;
    struct passwd *pw = NULL;

    data->all_native = fm_path_is_native (file_info->path);
    data->has_dir = S_ISDIR (file_info->mode) != FALSE;

    for (l=fm_list_peek_head_link (data->files)->next; l; l=l->next)
    {
        FmFileInfo *file_info =  (FmFileInfo*)l->data;

        if ( !fm_path_is_native (file_info->path) )
            data->all_native = FALSE;

        if (S_ISDIR (file_info->mode))
            data->has_dir = TRUE;

        if ( uid != file_info->uid )
            uid = -1;
        if ( gid != file_info->gid )
            gid = -1;

        if ( owner_perm != -1 && owner_perm !=  (file_info->mode & S_IRWXU) )
            owner_perm = -1;
        if ( group_perm != -1 && group_perm !=  (file_info->mode & S_IRWXG) )
            group_perm = -1;
        if ( other_perm != -1 && other_perm !=  (file_info->mode & S_IRWXO) )
            other_perm = -1;

        if ( exec_perm !=  (file_info->mode &  (S_IXUSR|S_IXGRP|S_IXOTH)) )
            exec_perm = -1;
    }

    if ( data->all_native )
    {
        if ( uid >= 0 )
        {
            pw = getpwuid (uid);
            if (pw)
                gtk_entry_set_text (GTK_ENTRY (data->owner), pw->pw_name);
        }
        if ( gid >= 0 )
        {
            grp = getgrgid (gid);
            if (grp)
                gtk_entry_set_text (GTK_ENTRY (data->group), grp->gr_name);
        }
    }

    if ( uid >=0 && !pw )
    {
        tmp = g_strdup_printf ("%u", uid);
        gtk_entry_set_text (GTK_ENTRY (data->owner), tmp);
        g_free (tmp);
    }

    if ( gid >=0 && !grp )
    {
        tmp = g_strdup_printf ("%u", gid);
        gtk_entry_set_text (GTK_ENTRY (data->group), tmp);
        g_free (tmp);
    }

    data->orig_owner = g_strdup (gtk_entry_get_text (GTK_ENTRY (data->owner)));
    data->orig_group = g_strdup (gtk_entry_get_text (GTK_ENTRY (data->group)));

    // on local filesystems, only root can do chown.
    if ( data->all_native && geteuid () != 0 )
    {
        gtk_editable_set_editable (GTK_EDITABLE (data->owner), FALSE);
        gtk_editable_set_editable (GTK_EDITABLE (data->group), FALSE);
    }

    sel = NO_CHANGE;
    if (owner_perm != -1)
    {
        if (  (owner_perm &  (S_IRUSR|S_IWUSR)) ==  (S_IRUSR|S_IWUSR) )
            sel = READ_WRITE;
        else if (  (owner_perm &  (S_IRUSR|S_IWUSR)) == S_IRUSR )
            sel = READ_ONLY;
        else if (  (owner_perm &  (S_IRUSR|S_IWUSR)) == S_IWUSR )
            sel = WRITE_ONLY;
        else
            sel = NONE;
    }
    gtk_combo_box_set_active (GTK_COMBO_BOX (data->owner_perm), sel);
    data->owner_perm_sel = sel;

    sel = NO_CHANGE;
    if (group_perm != -1)
    {
        if (  (group_perm &  (S_IRGRP|S_IWGRP)) ==  (S_IRGRP|S_IWGRP) )
            sel = READ_WRITE;
        else if (  (group_perm &  (S_IRGRP|S_IWGRP)) == S_IRGRP )
            sel = READ_ONLY;
        else if (  (group_perm &  (S_IRGRP|S_IWGRP)) == S_IWGRP )
            sel = WRITE_ONLY;
        else
            sel = NONE;
    }
    gtk_combo_box_set_active (GTK_COMBO_BOX (data->group_perm), sel);
    data->group_perm_sel = sel;

    sel = NO_CHANGE;
    if (other_perm != -1)
    {
        if (  (other_perm &  (S_IROTH|S_IWOTH)) ==  (S_IROTH|S_IWOTH) )
            sel = READ_WRITE;
        else if (  (other_perm &  (S_IROTH|S_IWOTH)) == S_IROTH )
            sel = READ_ONLY;
        else if (  (other_perm &  (S_IROTH|S_IWOTH)) == S_IWOTH )
            sel = WRITE_ONLY;
        else
            sel = NONE;
    }
    gtk_combo_box_set_active (GTK_COMBO_BOX (data->other_perm), sel);
    data->other_perm_sel = sel;

    if (data->has_dir)
        gtk_widget_hide ( data->exec );

    if ( exec_perm != -1 )
    {
        gboolean xusr =  (exec_perm & S_IXUSR) != 0;
        gboolean xgrp =  (exec_perm & S_IXGRP) != 0;
        gboolean xoth =  (exec_perm & S_IXOTH) != 0;
        if ( xusr == xgrp && xusr == xoth ) // executable
        {
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (data->exec), xusr);
            data->exec_state = xusr;
        }
        else // inconsistent
        {
            gtk_toggle_button_set_inconsistent (GTK_TOGGLE_BUTTON (data->exec), TRUE);
            g_signal_connect (data->exec, "toggled", G_CALLBACK (on_exec_toggled), data);
            data->exec_state = -1;
        }
    }
    else // inconsistent
    {
        gtk_toggle_button_set_inconsistent (GTK_TOGGLE_BUTTON (data->exec), TRUE);
        g_signal_connect (data->exec, "toggled", G_CALLBACK (on_exec_toggled), data);
        data->exec_state = -1;
    }
}

static void update_ui (FmFilePropData *data)
{
    GtkImage *img =  (GtkImage*) data->icon;

    if ( data->single_type ) // all files are of the same mime-type
    {
        GIcon *icon = NULL;
        // FIXME_pcm: handle custom icons for some files

        /* FIXME_pcm: display special property pages for special files or
         * some specified mime-types. */
        if ( data->single_file ) // only one file is selected.
        {
            FmFileInfo *file_info =  (FmFileInfo*)fm_list_peek_head (data->files);
            
            icon = fm_file_info_get_gicon (file_info);
        }

        if (data->mime_type)
        {
            if (!icon)
            {
                FmIcon *ficon = fm_mime_type_get_icon (data->mime_type);
                if (ficon)
                    icon = ficon->gicon;
            }
            gtk_label_set_text (GTK_LABEL (data->type), fm_mime_type_get_desc (data->mime_type));
        }

        if (icon)
            gtk_image_set_from_gicon (img, icon, GTK_ICON_SIZE_DIALOG);

        if ( data->single_file && fm_file_info_is_symlink (data->file_info) )
        {
            gtk_widget_show (data->target_label);
            gtk_widget_show (data->target);
            gtk_label_set_text (GTK_LABEL (data->target), data->file_info->target);
            // gtk_label_set_text (data->type, fm_mime_type_get_desc (data->mime_type));
        }
        else
        {
            gtk_widget_destroy (data->target_label);
            gtk_widget_destroy (data->target);
        }
    }
    else
    {
        gtk_image_set_from_stock (img, GTK_STOCK_DND_MULTIPLE, GTK_ICON_SIZE_DIALOG);
        gtk_widget_set_sensitive (data->name, FALSE);

        gtk_label_set_text (GTK_LABEL (data->type), _ ("Files of different types"));

        gtk_widget_destroy (data->target_label);
        gtk_widget_destroy (data->target);

        gtk_widget_destroy (data->open_with_label);
        gtk_widget_destroy (data->open_with);
        data->open_with = data->open_with_label = NULL;
    }

    // FIXME_pcm: check if all files has the same parent dir, mtime, or atime
    if ( data->single_file )
    {
        char buf[128];
        FmPath *parent = fm_path_get_parent (fm_file_info_get_path (data->file_info));
        char *parent_str = parent ? fm_path_display_name (parent, TRUE) : NULL;
        gtk_entry_set_text (GTK_ENTRY (data->name), fm_file_info_get_disp_name (data->file_info));
        if (parent_str)
        {
            gtk_label_set_text (GTK_LABEL (data->dir), parent_str);
            g_free (parent_str);
        }
        else
            gtk_label_set_text (GTK_LABEL (data->dir), "");
        gtk_label_set_text (GTK_LABEL (data->mtime), fm_file_info_get_disp_mtime (data->file_info));

        // FIXME_pcm: need to encapsulate this in an libfm API.
        strftime ( buf, sizeof ( buf ),
                  "%x %R",
                  localtime ( &data->file_info->atime ) );
        gtk_label_set_text (GTK_LABEL (data->atime), buf);
    }
    else
    {
        gtk_entry_set_text (GTK_ENTRY (data->name), _ ("Multiple Files"));
        gtk_widget_set_sensitive (data->name, FALSE);
    }

    update_permissions (data);

    on_timeout (data);
}

static void init_application_list (FmFilePropData *data)
{
    if (data->single_type && data->mime_type)
    {
        if (g_strcmp0 (data->mime_type->type, "inode/directory"))
            fm_app_chooser_combo_box_setup_for_mime_type ((GtkComboBox*) data->open_with, data->mime_type);
        else // shouldn't allow set file association for folders.
        {
            gtk_widget_destroy (data->open_with_label);
            gtk_widget_destroy (data->open_with);
            data->open_with = data->open_with_label = NULL;
        }
    }
}

GtkWidget *fm_file_properties_widget_new (FmFileInfoList *files, gboolean toplevel)
{
    GtkBuilder *builder=gtk_builder_new ();
    GtkWidget *dlg, *total_size;
    FmFilePropData *data;
    FmPathList *paths;

    gtk_builder_set_translation_domain (builder, GETTEXT_PACKAGE);
    data = g_slice_new0 (FmFilePropData);

    data->files = fm_list_ref (files);
    data->single_type = fm_file_info_list_is_same_type (files);
    data->single_file =  (fm_list_get_length (files) == 1);
    data->file_info = fm_list_peek_head (files);
    
    FmMimeType *fi_mime_type = fm_file_info_get_mime_type (data->file_info, FALSE);
    
    if (data->single_type)
        data->mime_type = fi_mime_type; // FIXME_pcm: do we need ref counting here?
    
    paths = fm_path_list_new_from_file_info_list (files);
    data->dc_job = fm_deep_count_job_new (paths, FM_DC_JOB_DEFAULT);
    fm_list_unref (paths);

    if (toplevel)
    {
        gtk_builder_add_from_string (builder, PROPERTIES_DLG, -1, NULL);
        GET_WIDGET (dlg);
        gtk_dialog_set_alternative_button_order (GTK_DIALOG (data->dlg), GTK_RESPONSE_OK, GTK_RESPONSE_CANCEL, -1);
    }
    else
    {
        #if 0
        // FIXME_pcm: is this really useful?
        const char *names[]={"notebook", NULL};
        gtk_builder_add_objects_from_file (builder, UI_FILE, names, NULL);
        data->dlg =  (GtkWidget*)gtk_builder_get_object (builder, "notebook");
        #endif
    }

    dlg = data->dlg;

    GET_WIDGET (icon);
    GET_WIDGET (name);
    GET_WIDGET (dir);
    GET_WIDGET (target);
    GET_WIDGET (target_label);
    GET_WIDGET (type);
    GET_WIDGET (open_with_label);
    GET_WIDGET (open_with);
    GET_WIDGET (total_size);
    GET_WIDGET (size_on_disk);
    GET_WIDGET (mtime);
    GET_WIDGET (atime);

    GET_WIDGET (owner);
    GET_WIDGET (group);

    GET_WIDGET (owner_perm);
    GET_WIDGET (group_perm);
    GET_WIDGET (other_perm);
    GET_WIDGET (exec);

    g_object_unref (builder);

    init_application_list (data);

    data->timeout = g_timeout_add (600,  (GSourceFunc)on_timeout, data);
    g_signal_connect (dlg, "response", G_CALLBACK (on_response), data);
    g_signal_connect_swapped (dlg, "destroy", G_CALLBACK (fm_file_prop_data_free), data);
    g_signal_connect (data->dc_job, "finished", G_CALLBACK (on_finished), data);

    fm_job_run_async (data->dc_job);

    update_ui (data);

    return dlg;
}

gboolean fm_show_file_properties (GtkWindow *parent, FmFileInfoList *files)
{
    GtkWidget *dlg = fm_file_properties_widget_new (files, TRUE);
    if (parent)
        gtk_window_set_transient_for (GTK_WINDOW (dlg), parent);
    gtk_widget_show (dlg);
    return TRUE;
}

