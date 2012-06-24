/***********************************************************************************************************************
 * 
 *      fm-mount.h
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
#ifndef __FM_MOUNT_H__
#define __FM_MOUNT_H__

#include <gtk/gtk.h>
#include <gio/gio.h>

#include "fm-path.h"


G_BEGIN_DECLS

// Mount
void fm_mount_automount ();

gboolean fm_mount_path      (GtkWindow *parent, FmPath *path, gboolean interactive);
gboolean fm_mount_volume    (GtkWindow *parent, GVolume *vol, gboolean interactive);
gboolean fm_unmount_mount   (GtkWindow *parent, GMount *mount, gboolean interactive);
gboolean fm_unmount_volume  (GtkWindow *parent, GVolume *vol, gboolean interactive);
gboolean fm_eject_mount     (GtkWindow *parent, GMount *mount, gboolean interactive);
gboolean fm_eject_volume    (GtkWindow *parent, GVolume *vol, gboolean interactive);


G_END_DECLS
#endif



