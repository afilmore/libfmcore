/***********************************************************************************************************************
 * 
 *      fm-dnd-dest.h
 *
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
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
#ifndef __FM_DND_DEST_H__
#define __FM_DND_DEST_H__

#include <gtk/gtk.h>
#include "fm-file-info-list.h"

G_BEGIN_DECLS

#define FM_TYPE_DND_DEST            (fm_dnd_dest_get_type ())
#define FM_DND_DEST(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),  FM_TYPE_DND_DEST, FmDndDest))
#define FM_DND_DEST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),   FM_TYPE_DND_DEST, FmDndDestClass))
#define FM_IS_DND_DEST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),  FM_TYPE_DND_DEST))
#define FM_IS_DND_DEST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),   FM_TYPE_DND_DEST))

//default droppable targets
enum
{
    FM_DND_DEST_TARGET_FM_LIST,     // direct pointer of FmList
    FM_DND_DEST_TARGET_URI_LIST,    // text/uri-list
    FM_DND_DEST_TARGET_XDS,         // X direct save
    N_FM_DND_DEST_DEFAULT_TARGETS
};

extern GtkTargetEntry fm_default_dnd_dest_targets[];

typedef struct _FmDndDest           FmDndDest;
typedef struct _FmDndDestClass      FmDndDestClass;

struct _FmDndDestClass
{
    GObjectClass parent_class;
    gboolean (*files_dropped) (FmDndDest *dd, int x, int y, guint action, guint info_type, FmFileInfoList *files);
};

GType       fm_dnd_dest_get_type         (void);
FmDndDest * fm_dnd_dest_new          (GtkWidget *w);

void fm_dnd_dest_set_widget (FmDndDest *dd, GtkWidget *w);

// the returned list can be either FmPathList or FmFileInfoList
// check with fm_list_is_path_list () and fm_list_is_file_info_list ().
FmList *fm_dnd_dest_get_src_files (FmDndDest *dd);

void fm_dnd_dest_set_dest_file (FmDndDest *dd, FmFileInfo *dest_file);
FmFileInfo *fm_dnd_dest_get_dest_file (FmDndDest *dd);
FmPath *fm_dnd_dest_get_dest_path (FmDndDest *dd);

#define fm_drag_context_has_target(ctx, target) \
    (g_list_find(gdk_drag_context_list_targets (ctx), target) != NULL)

#define fm_drag_context_has_target_name(ctx, name)  \
    fm_drag_context_has_target(ctx, gdk_atom_intern_static_string (name))

gboolean fm_dnd_dest_drag_data_received (FmDndDest *dd, GdkDragContext *drag_context,
             gint x, gint y, GtkSelectionData *sel_data, guint info, guint time);

gboolean fm_dnd_dest_is_target_supported (FmDndDest *dd, GdkAtom target);

GdkAtom fm_dnd_dest_find_target (FmDndDest *dd, GdkDragContext *drag_context);

gboolean fm_dnd_dest_drag_drop (FmDndDest *dd, GdkDragContext *drag_context,
                               GdkAtom target, int x, int y, guint time);

GdkDragAction fm_dnd_dest_get_default_action (FmDndDest *dd,
                                             GdkDragContext *drag_context,
                                             GdkAtom target);

void fm_dnd_dest_drag_leave (FmDndDest *dd, GdkDragContext *drag_context, guint time);

G_END_DECLS
#endif


