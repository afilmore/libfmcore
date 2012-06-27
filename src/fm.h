/***********************************************************************************************************************
 * 
 *      fm.h
 *
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
 *      Copyright 2012 Axel FILMORE <axel.filmore@gmail.com>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
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
#ifndef __LIBFM_FM_H__
#define __LIBFM_FM_H__


// Fm Base...
#include "fm-app-info.h"
#include "fm-archiver.h"
#include "fm-dummy-monitor.h"
#include "fm-file-info.h"
#include "fm-file-info-list.h"
#include "fm-folder.h"
#include "fm-icon.h"
#include "fm-list.h"
#include "fm-marshal.h"
#include "fm-mime-type.h"
#include "fm-monitor.h"
#include "fm-path.h"
#include "fm-path-list.h"

// Fm Vala...
#include "fm-vala.h"

// Fm Gtk...
#include "fm-clipboard.h"
#include "fm-dnd-auto-scroll.h"
#include "fm-dnd-dest.h"
#include "fm-dnd-src.h"
#include "fm-file-menu.h"
#include "fm-folder-model.h"
#include "fm-gtk-marshal.h"
#include "fm-icon-pixbuf.h"
#include "fm-thumbnail.h"

// Fm Gtk Dlg...
#include "fm-app-chooser-combo-box.h"
#include "fm-app-chooser-dlg.h"
#include "fm-app-menu-view.h"
#include "fm-file-properties.h"


// Fm Jobs
#include "fm-deep-count-job.h"
#include "fm-dir-list-job.h"
#include "fm-file-info-job.h"


// Fm Utils
#include "fm-debug.h"
#include "fm-file-ops.h"
#include "fm-launch.h"
#include "fm-mount.h"
#include "fm-msgbox.h"
#include "fm-select-folder-dlg.h"
#include "fm-trash.h"
#include "fm-user-input-dlg.h"
#include "fm-utils.h"


G_BEGIN_DECLS


gboolean fm_init (FmConfig *config);
void fm_finalize ();

G_END_DECLS
#endif



