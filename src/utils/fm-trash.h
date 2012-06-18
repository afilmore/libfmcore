/***********************************************************************************************************************
 * 
 *      fm-trash.h
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
#ifndef __FM_TRASH_H__
#define __FM_TRASH_H__

#include <gtk/gtk.h>

#include "fm-path-list.h"

G_BEGIN_DECLS

enum _FmDeleteFlags
{
    FM_DELETE_FLAGS_NONE,
    FM_DELETE_FLAGS_TRASH_OR_DELETE,
    FM_DELETE_FLAGS_TRASH
};
typedef enum _FmDeleteFlags FmDeleteFlags;


void fm_trash_delete    (GtkWindow *parent, FmPathList *path_list, FmDeleteFlags delete_flags, gboolean confim_delete);
void fm_trash_restore   (GtkWindow *parent, FmPathList *path_list);
void fm_trash_empty     (GtkWindow *parent);


G_END_DECLS
#endif



