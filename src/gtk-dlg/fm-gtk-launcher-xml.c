/***********************************************************************************************************************
 * 
 *      fm-gtk-launcher.c
 *
 *      Copyright 2010 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
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
//~ #ifdef HAVE_CONFIG_H
//~ #include <config.h>
//~ #endif
//~ 
//~ #include <glib/gi18n-lib.h>
//~ #include <gio/gdesktopappinfo.h>
//~ 
//~ // for open ()
//~ #include <sys/types.h>
//~ #include <sys/stat.h>
//~ #include <fcntl.h>
//~ 
//~ // for read ()
//~ #include <unistd.h>
//~ 
//~ #include "fm-gtk-launcher.h"
//~ #include "fm-utils.h"
//~ #include "fm-msgbox.h"
//~ #include "fm-app-chooser-dlg.h"
//~ #include "fm-vala.h"
//~ #include "fm-file-info-job.h"
//~ #include "fm-app-info.h"


/*****************************************************************************************
 * Execute File Dialog
 * 
 * 
 ****************************************************************************************/
const char EXEC_FILE_DLG [] =
"<?xml version='1.0' encoding='UTF-8'?>"
"<interface>"
"<!-- interface-requires gtk+ 3.0 -->"
"<object class='GtkDialog' id='dlg'>"
"<property name='border_width'>5</property>"
"<property name='title' translatable='yes'>Execute File</property>"
"<property name='window_position'>center</property>"
"<property name='type_hint'>normal</property>"
"<child internal-child='vbox'>"
"<object class='GtkVBox' id='dialog-vbox1'>"
"<property name='visible'>True</property>"
"<property name='orientation'>vertical</property>"
"<property name='spacing'>2</property>"
"<child>"
"<object class='GtkHBox' id='hbox1'>"
"<property name='visible'>True</property>"
"<property name='spacing'>12</property>"
"<child>"
"<object class='GtkImage' id='icon'>"
"<property name='visible'>True</property>"
"<property name='stock'>gtk-missing-image</property>"
"</object>"
"<packing>"
"<property name='expand'>False</property>"
"<property name='fill'>False</property>"
"<property name='position'>0</property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkLabel' id='msg'>"
"<property name='visible'>True</property>"
"<property name='xalign'>0</property>"
"</object>"
"<packing>"
"<property name='position'>1</property>"
"</packing>"
"</child>"
"</object>"
"<packing>"
"<property name='expand'>False</property>"
"<property name='position'>1</property>"
"</packing>"
"</child>"
"<child internal-child='action_area'>"
"<object class='GtkHButtonBox' id='dialog-action_area'>"
"<property name='visible'>True</property>"
"<property name='layout_style'>end</property>"
"<child>"
"<object class='GtkButton' id='exec'>"
"<property name='label'>gtk-execute</property>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='can_default'>True</property>"
"<property name='receives_default'>True</property>"
"<property name='use_stock'>True</property>"
"</object>"
"<packing>"
"<property name='expand'>False</property>"
"<property name='fill'>False</property>"
"<property name='position'>0</property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkButton' id='exec_term'>"
"<property name='label' translatable='yes'>Execute in _Terminal</property>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='can_default'>True</property>"
"<property name='receives_default'>True</property>"
"<property name='use_underline'>True</property>"
"</object>"
"<packing>"
"<property name='expand'>False</property>"
"<property name='fill'>False</property>"
"<property name='position'>1</property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkButton' id='open'>"
"<property name='label'>gtk-open</property>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='receives_default'>True</property>"
"<property name='use_stock'>True</property>"
"</object>"
"<packing>"
"<property name='expand'>False</property>"
"<property name='fill'>False</property>"
"<property name='position'>2</property>"
"</packing>"
"</child>"
"<child>"
"<object class='GtkButton' id='cancel'>"
"<property name='label'>gtk-cancel</property>"
"<property name='visible'>True</property>"
"<property name='can_focus'>True</property>"
"<property name='receives_default'>True</property>"
"<property name='use_stock'>True</property>"
"</object>"
"<packing>"
"<property name='expand'>False</property>"
"<property name='fill'>False</property>"
"<property name='position'>3</property>"
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
"<action-widget response='1'>exec</action-widget>"
"<action-widget response='2'>exec_term</action-widget>"
"<action-widget response='3'>open</action-widget>"
"<action-widget response='-7'>cancel</action-widget>"
"</action-widgets>"
"</object>"
"</interface>"
;


