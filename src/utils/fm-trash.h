/***********************************************************************************************************************
 * 
 *      fm-trash.h
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
#ifndef __FM_TRASH_H__
#define __FM_TRASH_H__

#include <gtk/gtk.h>

#include "fm-path-list.h"

G_BEGIN_DECLS

void fm_trash_files (GtkWindow *parent, FmPathList *files);
void fm_delete_files (GtkWindow *parent, FmPathList *files);

// trash or delete files according to FmConfig::use_trash.
void fm_trash_or_delete_files (GtkWindow *parent, FmPathList *files);

void fm_untrash_files (GtkWindow *parent, FmPathList *files);

void fm_empty_trash (GtkWindow *parent);



G_END_DECLS
#endif



