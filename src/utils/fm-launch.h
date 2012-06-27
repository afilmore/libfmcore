/***********************************************************************************************************************
 * 
 *      fm-launch.h
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
#ifndef __FM_GTK_LAUNCHER_H__
#define __FM_GTK_LAUNCHER_H__

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <stdarg.h>

#include "fm-file-info.h"
#include "fm-path.h"
#include "fm-utils.h"

G_BEGIN_DECLS


typedef gboolean (*FmLaunchFolderFunc)          (GAppLaunchContext *ctx, GList *folder_infos,
                                                 gpointer user_data, GError **err);

gboolean fm_launch_file                         (GtkWindow *parent, GAppLaunchContext *ctx, FmFileInfo *file_info,
                                                 FmLaunchFolderFunc func, gpointer user_data);

gboolean fm_launch_multiple_files               (GtkWindow *parent, GAppLaunchContext *ctx, GList *file_infos,
                                                 FmLaunchFolderFunc func, gpointer user_data);
                                 
G_END_DECLS
#endif



