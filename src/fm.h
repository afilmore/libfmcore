/*
 *      fm.h
 *
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
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
 */

#ifndef __LIBFM_FM_H__
#define __LIBFM_FM_H__


// Fm Base.
#include "fm-app-info.h"
#include "fm-archiver.h"
#include "fm-config.h"
#include "fm-dummy-monitor.h"
#include "fm-file-info.h"
#include "fm-folder.h"
#include "fm-icon.h"
#include "fm-list.h"
#include "fm-marshal.h"
#include "fm-mime-type.h"
#include "fm-monitor.h"
#include "fm-path.h"
#include "fm-path-list.h"
#include "fm-utils.h"

// Fm Job
#include "fm-deep-count-job.h"
#include "fm-dir-list-job.h"
#include "fm-file-info-job.h"
#include "fm-simple-job.h"

// Fm Gtk
#include "fm-app-chooser-combo-box.h"
#include "fm-app-chooser-dlg.h"
#include "fm-app-menu-view.h"
#include "fm-cell-renderer-pixbuf.h"
#include "fm-file-menu.h"
#include "fm-gtk-launcher.h"
#include "fm-gtk-marshal.h"
#include "fm-gtk-utils.h"
#include "fm-icon-pixbuf.h"
#include "fm-thumbnail.h"


G_BEGIN_DECLS

typedef enum
{
    FM_WP_COLOR,
    FM_WP_STRETCH,
    FM_WP_FIT,
    FM_WP_CENTER,
    FM_WP_TILE
}FmWallpaperMode;

extern GQuark fm_qdata_id; /* a quark value used to associate data with objects */

gboolean fm_init(FmConfig* config);
void fm_finalize();

G_END_DECLS

#endif

