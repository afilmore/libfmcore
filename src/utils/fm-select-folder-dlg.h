/***********************************************************************************************************************
 * 
 *      fm-select-folder-dlg.h
 *
 *      Copyright 2009 PCMan <pcman@debian>
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
#ifndef __FM_SELECT_FOLDER_DLG_H__
#define __FM_SELECT_FOLDER_DLG_H__

#include <gtk/gtk.h>

#include "fm-path.h"

G_BEGIN_DECLS

// Ask the user to select a folder.
FmPath *fm_select_folder (GtkWindow *parent, const char *title);


G_END_DECLS
#endif




