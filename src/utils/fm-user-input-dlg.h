/***********************************************************************************************************************
 * 
 *      fm-user-input-dlg.h
 *
 *      Copyright 2009 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
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
#ifndef __FM_USER_INPUT_DLG_H__
#define __FM_USER_INPUT_DLG_H__

#include <gtk/gtk.h>


G_BEGIN_DECLS

// Convinient dialog functions
gchar *fm_get_user_input_rename (GtkWindow *parent, const char *title, const char *msg, const char *default_text);

char *fm_get_user_input (GtkWindow *parent, const char *title, const char *msg, const char *default_text);

//FmPath *fm_get_user_input_path (GtkWindow *parent, const char *title, const char *msg, FmPath *default_path);



G_END_DECLS
#endif



