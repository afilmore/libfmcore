/***********************************************************************************************************************
 * 
 *      fm-dlg-utils.h
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
#ifndef __FM_DLG_UTILS_H__
#define __FM_DLG_UTILS_H__

#include <gtk/gtk.h>
//~ #include <gio/gio.h>
//~ #include <stdarg.h>

#include "fm-path.h"
//~ #include "fm-file-ops-job.h"

G_BEGIN_DECLS

// Convinient dialog functions
gchar *fm_get_user_input_rename (GtkWindow *parent, const char *title, const char *msg, const char *default_text);

// Display an error message to the user
void fm_show_error (GtkWindow *parent, const char *title, const char *msg);

// Ask the user a yes-no question.
gboolean fm_yes_no (GtkWindow *parent, const char *title, const char *question, gboolean default_yes);
gboolean fm_ok_cancel (GtkWindow *parent, const char *title, const char *question, gboolean default_ok);

// Ask the user a question with a NULL-terminated array of
// options provided. The return value was index of the selected option.
int fm_ask (GtkWindow *parent, const char *title, const char *question, ...);
int fm_askv (GtkWindow *parent, const char *title, const char *question, const char **options);
int fm_ask_valist (GtkWindow *parent, const char *title, const char *question, va_list options);

char *fm_get_user_input (GtkWindow *parent, const char *title, const char *msg, const char *default_text);
FmPath *fm_get_user_input_path (GtkWindow *parent, const char *title, const char *msg, FmPath *default_path);

// Ask the user to select a folder.
FmPath *fm_select_folder (GtkWindow *parent, const char *title);



G_END_DECLS
#endif



