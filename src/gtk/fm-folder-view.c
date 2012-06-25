/***********************************************************************************************************************
 * 
 *      folder-view.c
 *
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
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
 *      Purpose: The folder view is a ScrolledWindow, it creates an ExoIconView or an ExoTreeView according
 *               to the selected mode.
 * 
 * 
 **********************************************************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n-lib.h>

#include "fm-debug.h"

#include "fm-vala.h"
#include "fm-folder-view.h"
#include "fm-folder.h"
#include "fm-folder-model.h"
#include "fm-gtk-marshal.h"
#include "fm-cell-renderer-text.h"
#include "fm-cell-renderer-pixbuf.h"

#include "fm-mount.h"
#include "fm-msgbox.h"

#include "exo-icon-view.h"
#include "exo-tree-view.h"

#include "fm-dnd-auto-scroll.h"


G_DEFINE_TYPE (FmFolderView, fm_folder_view, GTK_TYPE_SCROLLED_WINDOW);

enum {
    DIRECTORY_CHANGED,
    LOADED,
    CLICKED,
    SEL_CHANGED,
    SORT_CHANGED,
    N_SIGNALS
};

static guint signals[N_SIGNALS];

// TODO_axl: Add this to Dconf...
#define SINGLE_CLICK_TIMEOUT    600


// Forward declarations...
static void fm_folder_view_finalize     (GObject *object);

static inline void create_icon_view     (FmFolderView *folder_view, GList *sels);
static inline void create_list_view     (FmFolderView *folder_view, GList *sels);



static GList *fm_folder_view_get_selected_tree_paths (FmFolderView *folder_view);



static gboolean on_folder_view_focus_in (GtkWidget *widget, GdkEventFocus *evt);
static void on_chdir (FmFolderView *folder_view, FmPath *dir_path);
static void on_loaded (FmFolderView *folder_view, FmPath *dir_path);
static void on_model_loaded (FmFolderModel *model, FmFolderView *folder_view);
static FmErrorAction on_folder_err (FmFolder *folder, GError *err, FmSeverity severity, FmFolderView *folder_view);



static gboolean on_btn_pressed (GtkWidget *current_view, GdkEventButton *evt, FmFolderView *folder_view);
static void on_sel_changed (GObject *obj, FmFolderView *folder_view);
static void on_sort_col_changed (GtkTreeSortable *sortable, FmFolderView *folder_view);



static void on_dnd_src_data_get (FmDndSrc *ds, FmFolderView *folder_view);



static void on_single_click_changed (FmConfig *cfg, FmFolderView *folder_view);
static void on_big_icon_size_changed (FmConfig *cfg, FmFolderView *folder_view);
//static void on_small_icon_size_changed (FmConfig *cfg, FmFolderView *folder_view);
static void on_thumbnail_size_changed (FmConfig *cfg, FmFolderView *folder_view);



static void cancel_pending_row_activated (FmFolderView *folder_view);



static void item_clicked (FmFolderView *folder_view, GtkTreePath *path, FmFolderViewClickType type);



static gboolean on_drag_motion (GtkWidget *dest_widget,
                                 GdkDragContext *drag_context,
                                 gint x,
                                 gint y,
                                 guint time,
                                 FmFolderView *folder_view);

static void on_drag_leave (GtkWidget *dest_widget,
                                GdkDragContext *drag_context,
                                guint time,
                                FmFolderView *folder_view);

static gboolean on_drag_drop (GtkWidget *dest_widget,
                               GdkDragContext *drag_context,
                               gint x,
                               gint y,
                               guint time,
                               FmFolderView *folder_view);


static void on_drag_data_received (GtkWidget *dest_widget,
                                    GdkDragContext *drag_context,
                                    gint x,
                                    gint y,
                                    GtkSelectionData *sel_data,
                                    guint info,
                                    guint time,
                                    FmFolderView *folder_view);



static void on_folder_loaded            (FmFolder *folder, FmFolderView *folder_view);
static void on_folder_unmounted         (FmFolder *folder, FmFolderView *folder_view);

static void on_icon_view_item_activated (ExoIconView *iv, GtkTreePath *path, FmFolderView *folder_view);
static void on_tree_view_row_activated  (GtkTreeView *tv, GtkTreePath *path, GtkTreeViewColumn *col,
                                         FmFolderView *folder_view);
static gboolean on_idle_tree_view_row_activated (FmFolderView *folder_view);



/*****************************************************************************************
 * 
 * 
 * 
 ****************************************************************************************/
static void fm_folder_view_class_init (FmFolderViewClass *klass)
{
    GObjectClass        *g_object_class;
    GtkWidgetClass      *widget_class;
    FmFolderViewClass   *fv_class;
    
    g_object_class = G_OBJECT_CLASS (klass);
    g_object_class->finalize = fm_folder_view_finalize;
    
    widget_class = GTK_WIDGET_CLASS (klass);
    widget_class->focus_in_event = on_folder_view_focus_in;
    
    fv_class = FM_FOLDER_VIEW_CLASS (klass);
    fv_class->directory_changed = on_chdir;
    fv_class->loaded = on_loaded;

    fm_folder_view_parent_class =  (GtkScrolledWindowClass*) g_type_class_peek (GTK_TYPE_SCROLLED_WINDOW);

    signals[DIRECTORY_CHANGED] = g_signal_new ("directory-changed",
                                               G_TYPE_FROM_CLASS (klass),
                                               G_SIGNAL_RUN_FIRST,
                                               G_STRUCT_OFFSET (FmFolderViewClass, directory_changed),
                                               NULL, NULL,
                                               g_cclosure_marshal_VOID__POINTER,
                                               G_TYPE_NONE, 1, G_TYPE_POINTER);

    signals[LOADED] = g_signal_new ("loaded",
                                    G_TYPE_FROM_CLASS (klass),
                                    G_SIGNAL_RUN_FIRST,
                                    G_STRUCT_OFFSET (FmFolderViewClass, loaded),
                                    NULL, NULL,
                                    g_cclosure_marshal_VOID__POINTER,
                                    G_TYPE_NONE, 1, G_TYPE_POINTER);

    signals[CLICKED] = g_signal_new ("clicked",
                                     G_TYPE_FROM_CLASS (klass),
                                     G_SIGNAL_RUN_FIRST,
                                     G_STRUCT_OFFSET (FmFolderViewClass, clicked),
                                     NULL, NULL,
                                     g_cclosure_marshal_VOID__UINT_POINTER,
                                     G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_POINTER);
    
    
    /*****************************************************************
     * Emitted when selection of the view got changed.
     * Currently selected files are passed as the parameter.
     * If there is no file selected, NULL is passed instead.
     *
     ****************************************************************/
    signals[SEL_CHANGED] = g_signal_new ("sel-changed",
                                         G_TYPE_FROM_CLASS (klass),
                                         G_SIGNAL_RUN_FIRST,
                                         G_STRUCT_OFFSET (FmFolderViewClass, sel_changed),
                                         NULL, NULL,
                                         g_cclosure_marshal_VOID__POINTER,
                                         G_TYPE_NONE, 1, G_TYPE_POINTER);

    // Emitted when sorting of the view got changed...
    signals[SORT_CHANGED] = g_signal_new ("sort-changed",
                                          G_TYPE_FROM_CLASS (klass),
                                          G_SIGNAL_RUN_FIRST,
                                          G_STRUCT_OFFSET (FmFolderViewClass, sort_changed),
                                          NULL, NULL,
                                          g_cclosure_marshal_VOID__VOID,
                                          G_TYPE_NONE, 0);
}

static void fm_folder_view_init (FmFolderView *self)
{
    gtk_scrolled_window_set_hadjustment ((GtkScrolledWindow*) self, NULL);
    gtk_scrolled_window_set_vadjustment ((GtkScrolledWindow*) self, NULL);
    gtk_scrolled_window_set_policy ((GtkScrolledWindow*) self, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    /*** Config change notifications
    g_signal_connect (fm_config, "changed::single_click", G_CALLBACK (on_single_click_changed), self);
    ***/
    
    // Drag and drop support...
    self->dnd_src = fm_dnd_src_new (NULL);
    g_signal_connect (self->dnd_src, "data-get", G_CALLBACK (on_dnd_src_data_get), self);

    self->dnd_dest = fm_dnd_dest_new (NULL);

    self->mode      = -1;
    self->sort_type = GTK_SORT_ASCENDING;
    self->sort_by   = COL_FILE_NAME;
}


/*** GtkWidget *fm_folder_view_new (FmFolderViewMode mode, small_icon_size, big_icon_size, single_click) ***/
GtkWidget *fm_folder_view_new (FmFolderViewMode mode)
{
    
    NO_DEBUG ("FM_FOLDER_VIEW: fm_folder_view_new: mode = %d\n", mode);
    
    FmFolderView *folder_view =  (FmFolderView*) g_object_new (FM_TYPE_FOLDER_VIEW, NULL);
    
    fm_folder_view_set_mode (folder_view, mode);
    
    // TODO_axl: pass settings as arguments and add accessor functions...
    folder_view->small_icon_size =  16;
    folder_view->big_icon_size =    36;
    folder_view->single_click =     FALSE;
    
    return (GtkWidget*) folder_view;
}


static void fm_folder_view_finalize (GObject *object)
{
    FmFolderView *self;

    g_return_if_fail (object != NULL);
    g_return_if_fail (IS_FM_FOLDER_VIEW (object));

    self = FM_FOLDER_VIEW (object);
    
    if (self->folder)
    {
        g_object_unref (self->folder);
        if (self->model)
            g_object_unref (self->model);
    }
    
    g_object_unref (self->dnd_src);
    g_object_unref (self->dnd_dest);

    if (self->cwd)
        fm_path_unref (self->cwd);

    g_signal_handlers_disconnect_by_func (fm_config, on_single_click_changed, object);

    cancel_pending_row_activated (self);

    if (self->icon_size_changed_handler)
        g_signal_handler_disconnect (fm_config, self->icon_size_changed_handler);

    if (G_OBJECT_CLASS (fm_folder_view_parent_class)->finalize)
         (*G_OBJECT_CLASS (fm_folder_view_parent_class)->finalize) (object);
}

void fm_folder_view_set_mode (FmFolderView *folder_view, FmFolderViewMode mode)
{
    if (mode == folder_view->mode)
        return;
    
    gboolean has_focus = FALSE;
    GList *sels = NULL;

    if (G_LIKELY(folder_view->current_view))
    {
        has_focus = gtk_widget_has_focus (folder_view->current_view);
        
        // Preserve old selections...
        sels = fm_folder_view_get_selected_tree_paths (folder_view);

        g_signal_handlers_disconnect_by_func (folder_view->current_view, on_drag_motion,        folder_view);
        g_signal_handlers_disconnect_by_func (folder_view->current_view, on_drag_leave,         folder_view);
        g_signal_handlers_disconnect_by_func (folder_view->current_view, on_drag_drop,          folder_view);
        g_signal_handlers_disconnect_by_func (folder_view->current_view, on_drag_data_received, folder_view);

        fm_dnd_unset_dest_auto_scroll (folder_view->current_view);

        // It may be better to hide the widget instead of destroying it...
        gtk_widget_destroy (folder_view->current_view);
    }

    if (folder_view->icon_size_changed_handler)
    {
        g_signal_handler_disconnect (fm_config, folder_view->icon_size_changed_handler);
        folder_view->icon_size_changed_handler = 0;
    }

    folder_view->mode = mode;
    
    switch (folder_view->mode)
    {
        case FM_FV_COMPACT_VIEW:
        case FM_FV_ICON_VIEW:
        case FM_FV_THUMBNAIL_VIEW:
            create_icon_view (folder_view, sels);
        break;
        
        case FM_FV_LIST_VIEW:
            create_list_view (folder_view, sels);
        break;
    }
    
    g_list_foreach (sels, (GFunc) gtk_tree_path_free, NULL);
    g_list_free (sels);

    // Maybe calling set_icon_size here is a good idea...

    gtk_drag_source_set (folder_view->current_view,
                         GDK_BUTTON1_MASK,
                         fm_default_dnd_src_targets,
                         N_FM_DND_SRC_DEFAULT_TARGETS,
                         GDK_ACTION_COPY | GDK_ACTION_MOVE | GDK_ACTION_LINK | GDK_ACTION_ASK);
                         
    fm_dnd_src_set_widget (folder_view->dnd_src, folder_view->current_view);

    gtk_drag_dest_set (folder_view->current_view,
                       0,
                       fm_default_dnd_dest_targets,
                       N_FM_DND_DEST_DEFAULT_TARGETS,
                       GDK_ACTION_COPY | GDK_ACTION_MOVE | GDK_ACTION_LINK | GDK_ACTION_ASK);
    
    fm_dnd_dest_set_widget (folder_view->dnd_dest, folder_view->current_view);
    
    g_signal_connect_after  (folder_view->current_view, "drag-motion",           G_CALLBACK (on_drag_motion),
                             folder_view);
    g_signal_connect        (folder_view->current_view, "drag-leave",            G_CALLBACK (on_drag_leave),
                             folder_view);
    g_signal_connect        (folder_view->current_view, "drag-drop",             G_CALLBACK (on_drag_drop),
                             folder_view);
    g_signal_connect        (folder_view->current_view, "drag-data-received",    G_CALLBACK (on_drag_data_received),
                             folder_view);
    g_signal_connect        (folder_view->current_view, "button-press-event",    G_CALLBACK (on_btn_pressed),
                             folder_view);

    fm_dnd_set_dest_auto_scroll (folder_view->current_view,
                                 gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (folder_view)),
                                 gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (folder_view)));

    gtk_widget_show (folder_view->current_view);
    gtk_container_add ((GtkContainer*) folder_view, folder_view->current_view);

    // Restore the focus if needed...
    if (has_focus)
        gtk_widget_grab_focus (folder_view->current_view);

}

FmFolderViewMode fm_folder_view_get_mode (FmFolderView *folder_view)
{
    return folder_view->mode;
}

static inline void create_icon_view (FmFolderView *folder_view, GList *sels)
{
    FmFolderModel *model = (FmFolderModel*) folder_view->model;
    int icon_size = 0;

    folder_view->current_view = exo_icon_view_new ();

    GtkCellRenderer *render = fm_cell_renderer_pixbuf_new ();
    folder_view->renderer_pixbuf = render;

    g_object_set ((GObject*) render, "follow-state", TRUE, NULL);
    
    gtk_cell_layout_pack_start ((GtkCellLayout*)    folder_view->current_view, render, TRUE);
    gtk_cell_layout_add_attribute ((GtkCellLayout*) folder_view->current_view, render, "pixbuf",    COL_FILE_ICON);
    gtk_cell_layout_add_attribute ((GtkCellLayout*) folder_view->current_view, render, "info",      COL_FILE_INFO);

    // Compact View...
    if (folder_view->mode == FM_FV_COMPACT_VIEW)
    {
        /*** folder_view->icon_size_changed_handler = g_signal_connect (fm_config,
                                                                        "changed::small_icon_size",
                                                                        G_CALLBACK (on_small_icon_size_changed),
                                                                        folder_view); ***/
        
        icon_size = folder_view->small_icon_size;
        
        fm_cell_renderer_pixbuf_set_fixed_size (FM_CELL_RENDERER_PIXBUF (folder_view->renderer_pixbuf),
                                                icon_size,
                                                icon_size);
        
        if (model)
            fm_folder_model_set_icon_size (model, icon_size);

        render = fm_cell_renderer_text_new ();
        
        g_object_set ((GObject*) render,
                      "xalign", 1.0,
                      "yalign", 0.5,
                      NULL);
        
        exo_icon_view_set_layout_mode ((ExoIconView*) folder_view->current_view, EXO_ICON_VIEW_LAYOUT_COLS);
        exo_icon_view_set_orientation ((ExoIconView*) folder_view->current_view, GTK_ORIENTATION_HORIZONTAL);
    }
    
    // Big Icon View or Thumbnail View...
    else
    {
        if (folder_view->mode == FM_FV_ICON_VIEW)
        {
            folder_view->icon_size_changed_handler = g_signal_connect (fm_config,
                                                                       "changed::big_icon_size",
                                                                       G_CALLBACK (on_big_icon_size_changed),
                                                                       folder_view);
            
            icon_size = folder_view->big_icon_size;
            fm_cell_renderer_pixbuf_set_fixed_size (FM_CELL_RENDERER_PIXBUF (folder_view->renderer_pixbuf),
                                                    icon_size,
                                                    icon_size);
            
            if (model)
                fm_folder_model_set_icon_size (model, icon_size);

            render = fm_cell_renderer_text_new ();
            
            // TODO_axl: Set the sizes of cells according to iconsize...
            g_object_set ((GObject*)render,
                          "wrap-mode", PANGO_WRAP_WORD_CHAR,
                          "wrap-width", 90,
                          "alignment", PANGO_ALIGN_CENTER,
                          "xalign", 0.5,
                          "yalign", 0.0,
                          NULL);
            
            exo_icon_view_set_column_spacing ( (ExoIconView*)folder_view->current_view, 4);
            exo_icon_view_set_item_width  ( (ExoIconView*)folder_view->current_view, 110);
        }
        
        // Tumbnail View...
        else
        {
            folder_view->icon_size_changed_handler = g_signal_connect (fm_config,
                                                                       "changed::thumbnail_size",
                                                                       G_CALLBACK (on_thumbnail_size_changed),
                                                                       folder_view);
            
            icon_size = fm_config->thumbnail_size;
            
            fm_cell_renderer_pixbuf_set_fixed_size (FM_CELL_RENDERER_PIXBUF (folder_view->renderer_pixbuf),
                                                    icon_size,
                                                    icon_size);
            
            if (model)
                fm_folder_model_set_icon_size (model, icon_size);

            render = fm_cell_renderer_text_new ();
            
            // TODO_axl: Set the sizes of cells according to iconsize...
            g_object_set ((GObject*)render,
                          "wrap-mode", PANGO_WRAP_WORD_CHAR,
                          "wrap-width", 180,
                          "alignment", PANGO_ALIGN_CENTER,
                          "xalign", 0.5,
                          "yalign", 0.0,
                          NULL);
            
            exo_icon_view_set_column_spacing    ((ExoIconView*) folder_view->current_view, 8);
            exo_icon_view_set_item_width        ((ExoIconView*) folder_view->current_view, 200);
        }
    }
    
    gtk_cell_layout_pack_start ((GtkCellLayout*) folder_view->current_view, render, TRUE);
    gtk_cell_layout_add_attribute ((GtkCellLayout*) folder_view->current_view,
                                   render,
                                   "text", COL_FILE_NAME);
    
    exo_icon_view_set_item_width ((ExoIconView*) folder_view->current_view, 96);
    exo_icon_view_set_search_column ((ExoIconView*) folder_view->current_view, COL_FILE_NAME);
    
    g_signal_connect (folder_view->current_view, "item-activated",      G_CALLBACK (on_icon_view_item_activated),
                      folder_view);
    g_signal_connect (folder_view->current_view, "selection-changed",   G_CALLBACK (on_sel_changed), folder_view);
    
    exo_icon_view_set_model ((ExoIconView*) folder_view->current_view, folder_view->model);
    exo_icon_view_set_selection_mode ((ExoIconView*) folder_view->current_view, folder_view->sel_mode);
    exo_icon_view_set_single_click ((ExoIconView*) folder_view->current_view, folder_view->single_click);
    exo_icon_view_set_single_click_timeout ((ExoIconView*) folder_view->current_view, SINGLE_CLICK_TIMEOUT);

    GList *l;
    for (l = sels; l; l=l->next)
        exo_icon_view_select_path ((ExoIconView*) folder_view->current_view, (GtkTreePath*) l->data);
}

static inline void create_list_view (FmFolderView *folder_view, GList *sels)
{
    GtkTreeSelection    *ts;
    FmFolderModel       *model = (FmFolderModel*) folder_view->model;
    
    int icon_size = 0;
    
    folder_view->current_view = exo_tree_view_new ();

    GtkCellRenderer *render = fm_cell_renderer_pixbuf_new ();
    folder_view->renderer_pixbuf = render;
    
    /*** folder_view->icon_size_changed_handler = g_signal_connect (fm_config,
                                                                    "changed::small_icon_size",
                                                                    G_CALLBACK (on_small_icon_size_changed),
                                                                    folder_view); ***/
    
    icon_size = folder_view->small_icon_size;
    
    fm_cell_renderer_pixbuf_set_fixed_size (FM_CELL_RENDERER_PIXBUF (folder_view->renderer_pixbuf),
                                            icon_size,
                                            icon_size);
    
    if (model)
        fm_folder_model_set_icon_size (model, icon_size);

    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (folder_view->current_view), TRUE);
    
    GtkTreeViewColumn *col = gtk_tree_view_column_new ();
    
    gtk_tree_view_column_set_title (col, _("Name"));
    gtk_tree_view_column_pack_start (col, render, FALSE);
    gtk_tree_view_column_set_attributes (col, render,
                                         "pixbuf",   COL_FILE_ICON,
                                         "info",     COL_FILE_INFO,
                                         NULL);
    
    render = gtk_cell_renderer_text_new ();
    g_object_set (render, "ellipsize", PANGO_ELLIPSIZE_END, NULL);
    
    gtk_tree_view_column_pack_start (col, render, TRUE);
    gtk_tree_view_column_set_attributes (col, render, "text", COL_FILE_NAME, NULL);
    gtk_tree_view_column_set_sort_column_id (col, COL_FILE_NAME);
    gtk_tree_view_column_set_expand (col, TRUE);
    gtk_tree_view_column_set_resizable (col, TRUE);
    gtk_tree_view_column_set_sizing (col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width (col, 200);
    
    gtk_tree_view_append_column ((GtkTreeView*) folder_view->current_view, col);
    
    // Only this column is activable...
    exo_tree_view_set_activable_column ((ExoTreeView*) folder_view->current_view, col);

    render = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes (_("Description"), render, "text", COL_FILE_DESC, NULL);
    gtk_tree_view_column_set_resizable (col, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, COL_FILE_DESC);
    gtk_tree_view_append_column ((GtkTreeView*) folder_view->current_view, col);

    render = gtk_cell_renderer_text_new ();
    g_object_set (render, "xalign", 1.0, NULL);
    col = gtk_tree_view_column_new_with_attributes (_("Size"), render, "text", COL_FILE_SIZE, NULL);
    gtk_tree_view_column_set_sort_column_id (col, COL_FILE_SIZE);
    gtk_tree_view_column_set_resizable (col, TRUE);
    gtk_tree_view_append_column ((GtkTreeView*) folder_view->current_view, col);

    render = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes (_("Modified"), render, "text", COL_FILE_MTIME, NULL);
    gtk_tree_view_column_set_resizable (col, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, COL_FILE_MTIME);
    gtk_tree_view_append_column ((GtkTreeView*) folder_view->current_view, col);

    gtk_tree_view_set_search_column ((GtkTreeView*) folder_view->current_view, COL_FILE_NAME);

    gtk_tree_view_set_rubber_banding ((GtkTreeView*) folder_view->current_view, TRUE);
    exo_tree_view_set_single_click ((ExoTreeView*) folder_view->current_view, folder_view->single_click);
    exo_tree_view_set_single_click_timeout ((ExoTreeView*) folder_view->current_view, SINGLE_CLICK_TIMEOUT);

    ts = gtk_tree_view_get_selection ((GtkTreeView*) folder_view->current_view);
    g_signal_connect (folder_view->current_view, "row-activated", G_CALLBACK (on_tree_view_row_activated), folder_view);
    g_signal_connect (ts, "changed", G_CALLBACK (on_sel_changed), folder_view);
    
    /*** cancel_pending_row_activated (folder_view); // Is this needed ? ***/
    
    gtk_tree_view_set_model ((GtkTreeView*) folder_view->current_view, folder_view->model);
    gtk_tree_selection_set_mode (ts, folder_view->sel_mode);

    GList *l;
    for (l = sels; l; l=l->next)
        gtk_tree_selection_select_path (ts, (GtkTreePath*) l->data);
}

FmFolderModel *fm_folder_view_get_model (FmFolderView *folder_view)
{
    return FM_FOLDER_MODEL (folder_view->model);
}

void fm_folder_view_set_show_hidden (FmFolderView *folder_view, gboolean show)
{
    if (show != folder_view->show_hidden)
    {
        folder_view->show_hidden = show;
        if (G_LIKELY (folder_view->model))
            fm_folder_model_set_show_hidden (FM_FOLDER_MODEL (folder_view->model), show);
    }
}

gboolean fm_folder_view_get_show_hidden (FmFolderView *folder_view)
{
    return folder_view->show_hidden;
}


/*****************************************************************************************
 * Current Working Directory...
 * 
 * 
 ****************************************************************************************/
gboolean fm_folder_view_get_is_loaded (FmFolderView *folder_view)
{
    return folder_view->folder && fm_folder_get_is_loaded (folder_view->folder);
}



FmFolder *fm_folder_view_get_folder (FmFolderView *folder_view)
{
    return folder_view->folder;
}

gboolean fm_folder_view_chdir_by_name (FmFolderView *folder_view, const char *path_str)
{
    gboolean ret;
    FmPath *path;

    if (G_UNLIKELY (!path_str))
        return FALSE;

    path = fm_path_new_for_str (path_str);
    
    // might be a malformed path
    if (!path)
        return FALSE;
    
    ret = fm_folder_view_chdir (folder_view, path);
    fm_path_unref (path);
    
    return ret;
}


gboolean fm_folder_view_chdir (FmFolderView *folder_view, FmPath *path)
{
    g_return_val_if_fail (folder_view, FALSE);

    FmFolderModel *model;
    FmFolder *folder;

    if (folder_view->folder)
    {
        g_signal_handlers_disconnect_by_func (folder_view->folder, on_folder_loaded, folder_view);
        g_signal_handlers_disconnect_by_func (folder_view->folder, on_folder_unmounted, folder_view);
        g_signal_handlers_disconnect_by_func (folder_view->folder, on_folder_err, folder_view);
        g_object_unref (folder_view->folder);
        folder_view->folder = NULL;
        
        if (folder_view->model)
        {
            model = FM_FOLDER_MODEL (folder_view->model);
            
            g_signal_handlers_disconnect_by_func (model, on_sort_col_changed, folder_view);
            
            // TODO_axl: use an accessor function...
            if (model->directory)
                g_signal_handlers_disconnect_by_func (model->directory, on_folder_err, folder_view);
            
            g_object_unref (model);
            folder_view->model = NULL;
        }
    }

    // FIXME_pcm: the signal handler should be able to cancel the loading.
    g_signal_emit (folder_view, signals[DIRECTORY_CHANGED], 0, path);
    if (folder_view->cwd)
        fm_path_unref (folder_view->cwd);
    
    folder_view->cwd = fm_path_ref (path);

    folder_view->folder = folder = fm_folder_get (path);
    
    if (!folder)
        return TRUE;
    
    // connect error handler
    g_signal_connect (folder, "loaded", (GCallback) on_folder_loaded, folder_view);
    g_signal_connect (folder, "unmount", (GCallback) on_folder_unmounted, folder_view);
    g_signal_connect (folder, "error", (GCallback) on_folder_err, folder_view);
    
    if (fm_folder_get_is_loaded (folder))
    {
        on_folder_loaded (folder, folder_view);
    }
    else
    {
        switch (folder_view->mode)
        {
            case FM_FV_LIST_VIEW:
                cancel_pending_row_activated (folder_view);
                gtk_tree_view_set_model (GTK_TREE_VIEW (folder_view->current_view), NULL);
            break;
            
            case FM_FV_ICON_VIEW:
            case FM_FV_COMPACT_VIEW:
            case FM_FV_THUMBNAIL_VIEW:
                exo_icon_view_set_model (EXO_ICON_VIEW (folder_view->current_view), NULL);
            break;
        }
        folder_view->model = NULL;
    }
    
    return TRUE;
}

FmPath *fm_folder_view_get_cwd (FmFolderView *folder_view)
{
    return folder_view->cwd;
}

FmFileInfo *fm_folder_view_get_cwd_info (FmFolderView *folder_view)
{
    // TODO_axl: use an accessor function...
    return FM_FOLDER_MODEL (folder_view->model)->directory->dir_fi;
}





// View Sorting...
/*****************************************************************************************
 * 
 * 
 * 
 ****************************************************************************************/
void fm_folder_view_sort (FmFolderView *folder_view, GtkSortType type, int by)
{
    //  (int) is needed here since enum seems to be treated as unsigned int so -1 becomes > 0
    
    if ((int) type >= 0)
        folder_view->sort_type = type;
    
    if (by >= 0)
        folder_view->sort_by = by;
    
    if (folder_view->model)
        gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (folder_view->model), folder_view->sort_by, folder_view->sort_type);
}

GtkSortType fm_folder_view_get_sort_type (FmFolderView *folder_view)
{
    return folder_view->sort_type;
}

int fm_folder_view_get_sort_by (FmFolderView *folder_view)
{
    return folder_view->sort_by;
}







/*****************************************************************************************
 * View Selections...
 * 
 * 
 ****************************************************************************************/
void fm_folder_view_set_selection_mode (FmFolderView *folder_view, GtkSelectionMode mode)
{
    if (folder_view->sel_mode == mode)
        return;
        
    folder_view->sel_mode = mode;
    switch (folder_view->mode)
    {
        case FM_FV_LIST_VIEW:
        {
            GtkTreeSelection *sel = gtk_tree_view_get_selection ((GtkTreeView*)folder_view->current_view);
            gtk_tree_selection_set_mode (sel, mode);
        }
        break;
        
        case FM_FV_ICON_VIEW:
        case FM_FV_COMPACT_VIEW:
        case FM_FV_THUMBNAIL_VIEW:
            exo_icon_view_set_selection_mode ((ExoIconView*)folder_view->current_view, mode);
        break;
    }
}

GtkSelectionMode fm_folder_view_get_selection_mode (FmFolderView *folder_view)
{
    return folder_view->sel_mode;
}

GList *fm_folder_view_get_selected_tree_paths (FmFolderView *folder_view)
{
    GList *sels = NULL;
    
    switch (folder_view->mode)
    {
        case FM_FV_LIST_VIEW:
        {
            GtkTreeSelection *sel;
            sel = gtk_tree_view_get_selection ((GtkTreeView*)folder_view->current_view);
            sels = gtk_tree_selection_get_selected_rows (sel, NULL);
        }
        break;
        
        case FM_FV_ICON_VIEW:
        case FM_FV_COMPACT_VIEW:
        case FM_FV_THUMBNAIL_VIEW:
            sels = exo_icon_view_get_selected_items ((ExoIconView*)folder_view->current_view);
        break;
    }
    return sels;
}

FmFileInfoList *fm_folder_view_get_selected_files (FmFolderView *folder_view)
{
    FmFileInfoList *fis;
    GList *sels = fm_folder_view_get_selected_tree_paths (folder_view);
    GList *l, *next;
    
    if (!sels)
        return NULL;
    
    fis = fm_file_info_list_new ();
    
    for (l = sels; l; l = next)
    {
        FmFileInfo *file_info;
        GtkTreeIter it;
        GtkTreePath *tp =  (GtkTreePath*)l->data;
        gtk_tree_model_get_iter (folder_view->model, &it, l->data);
        gtk_tree_model_get (folder_view->model, &it, COL_FILE_INFO, &file_info, -1);
        gtk_tree_path_free (tp);
        next = l->next;
        l->data = fm_file_info_ref (file_info);
        l->prev = l->next = NULL;
        fm_list_push_tail_link (fis, l);
    }
    return fis;
}

FmPathList *fm_folder_view_get_selected_file_paths (FmFolderView *folder_view)
{
    FmFileInfoList *files = fm_folder_view_get_selected_files (folder_view);
    FmPathList *list;
    if (files)
    {
        list = fm_path_list_new_from_file_info_list (files);
        fm_list_unref (files);
    }
    else
        list = NULL;
    return list;
}


void fm_folder_view_select_all (FmFolderView *folder_view)
{
    GtkTreeSelection  *tree_sel;
    switch (folder_view->mode)
    {
    case FM_FV_LIST_VIEW:
        tree_sel = gtk_tree_view_get_selection ((GtkTreeView*)folder_view->current_view);
        gtk_tree_selection_select_all (tree_sel);
        break;
    case FM_FV_ICON_VIEW:
    case FM_FV_COMPACT_VIEW:
    case FM_FV_THUMBNAIL_VIEW:
        exo_icon_view_select_all ((ExoIconView*)folder_view->current_view);
        break;
    }
}


void fm_folder_view_select_invert (FmFolderView *folder_view)
{
    switch (folder_view->mode)
    {
    case FM_FV_LIST_VIEW:
        {
            GtkTreeSelection *tree_sel;
            GtkTreeIter it;
            if (!gtk_tree_model_get_iter_first (folder_view->model, &it))
                return;
            tree_sel = gtk_tree_view_get_selection ((GtkTreeView*)folder_view->current_view);
            do
            {
                if (gtk_tree_selection_iter_is_selected (tree_sel, &it))
                    gtk_tree_selection_unselect_iter (tree_sel, &it);
                else
                    gtk_tree_selection_select_iter (tree_sel, &it);
            }while (gtk_tree_model_iter_next (folder_view->model, &it));
            break;
        }
    case FM_FV_ICON_VIEW:
    case FM_FV_COMPACT_VIEW:
    case FM_FV_THUMBNAIL_VIEW:
        {
            GtkTreePath *path;
            int i, n;
            n = gtk_tree_model_iter_n_children (folder_view->model, NULL);
            if (n == 0)
                return;
            path = gtk_tree_path_new_first ();
            for (i=0; i<n; ++i, gtk_tree_path_next (path))
            {
                if  (exo_icon_view_path_is_selected ((ExoIconView*)folder_view->current_view, path))
                    exo_icon_view_unselect_path ((ExoIconView*)folder_view->current_view, path);
                else
                    exo_icon_view_select_path ((ExoIconView*)folder_view->current_view, path);
            }
            break;
        }
    }
}

void fm_folder_view_select_file_path (FmFolderView *folder_view, FmPath *path)
{
    if (fm_path_equal (path->parent, folder_view->cwd))
    {
        FmFolderModel *model =  (FmFolderModel*)folder_view->model;
        GtkTreeIter it;
        if (fm_folder_model_find_iter_by_filename (model, &it, path->name))
        {
            switch (folder_view->mode)
            {
            case FM_FV_LIST_VIEW:
                {
                    GtkTreeSelection *sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (folder_view->current_view));
                    gtk_tree_selection_select_iter (sel, &it);
                }
                break;
            case FM_FV_ICON_VIEW:
            case FM_FV_COMPACT_VIEW:
            case FM_FV_THUMBNAIL_VIEW:
                {
                    GtkTreePath *tp = gtk_tree_model_get_path (GTK_TREE_MODEL (model), &it);
                    if (tp)
                    {
                        exo_icon_view_select_path (EXO_ICON_VIEW (folder_view->current_view), tp);
                        gtk_tree_path_free (tp);
                    }
                }
                break;
            }
        }
    }
}

void fm_folder_view_select_file_paths (FmFolderView *folder_view, FmPathList *paths)
{
    GList *l;
    for (l = fm_list_peek_head_link (paths);l; l=l->next)
    {
        FmPath *path = FM_PATH (l->data);
        fm_folder_view_select_file_path (folder_view, path);
    }
}

// FIXME_pcm: select files by custom func, not yet implemented
void fm_folder_view_custom_select (FmFolderView *folder_view, GFunc filter, gpointer user_data)
{

}

// Icon / Thumbnail Size
/*****************************************************************************************
 * 
 * 
 * 
 ****************************************************************************************/
static void set_icon_size (FmFolderView *folder_view, guint icon_size)
{
    FmCellRendererPixbuf *render =  (FmCellRendererPixbuf*)folder_view->renderer_pixbuf;
    fm_cell_renderer_pixbuf_set_fixed_size (render, icon_size, icon_size);

    if (!folder_view->model)
        return;

    fm_folder_model_set_icon_size (FM_FOLDER_MODEL (folder_view->model), icon_size);

    if (folder_view->mode != FM_FV_LIST_VIEW) // this is an ExoIconView
    {
        // FIXME_pcm: reset ExoIconView item sizes
    }
}

static void on_big_icon_size_changed (FmConfig *cfg, FmFolderView *folder_view)
{
    set_icon_size (folder_view, folder_view->big_icon_size);
}

#if 0
static void on_small_icon_size_changed (FmConfig *cfg, FmFolderView *folder_view)
{
    set_icon_size (folder_view, cfg->small_icon_size);
}
#endif

static void on_thumbnail_size_changed (FmConfig *cfg, FmFolderView *folder_view)
{
    
    // FIXME_pcm: thumbnail and icons should have different sizes
    // maybe a separate API: fm_folder_model_set_thumbnail_size ()
    
    set_icon_size (folder_view, cfg->thumbnail_size);
}

// Drag And Drop...
/*****************************************************************************************
 * 
 * 
 * 
 ****************************************************************************************/
static void on_drag_data_received (GtkWidget *dest_widget,
                                    GdkDragContext *drag_context,
                                    gint x,
                                    gint y,
                                    GtkSelectionData *sel_data,
                                    guint info,
                                    guint time,
                                    FmFolderView *folder_view)
{
    fm_dnd_dest_drag_data_received (folder_view->dnd_dest, drag_context, x, y, sel_data, info, time);
}

static gboolean on_drag_drop (GtkWidget *dest_widget,
                               GdkDragContext *drag_context,
                               gint x,
                               gint y,
                               guint time,
                               FmFolderView *folder_view)
{
    gboolean ret = FALSE;
    GdkAtom target = gtk_drag_dest_find_target (dest_widget, drag_context, NULL);
    if (target != GDK_NONE)
        ret = fm_dnd_dest_drag_drop (folder_view->dnd_dest, drag_context, target, x, y, time);
    return ret;
}

static GtkTreePath *get_drop_path (FmFolderView *folder_view, gint x, gint y)
{
    GtkTreePath *tp = NULL;

//    gboolean droppable = TRUE;
    
    switch (folder_view->mode)
    {
    case FM_FV_LIST_VIEW:
        {
            //GtkTreeViewDropPosition pos;
            
            GtkTreeViewColumn *col;
            gtk_tree_view_convert_widget_to_bin_window_coords (GTK_TREE_VIEW (folder_view->current_view), x, y, &x, &y);
            
            // if (gtk_tree_view_get_dest_row_at_pos ((GtkTreeView*)folder_view->current_view, x, y, &tp, NULL))
            
            if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (folder_view->current_view), x, y, &tp, &col, NULL, NULL))
            {
                if (gtk_tree_view_column_get_sort_column_id (col)!=COL_FILE_NAME)
                {
                    gtk_tree_path_free (tp);
                    tp = NULL;
                }
            }
            gtk_tree_view_set_drag_dest_row (GTK_TREE_VIEW (folder_view->current_view), tp, GTK_TREE_VIEW_DROP_INTO_OR_AFTER);
            break;
        }
    case FM_FV_ICON_VIEW:
    case FM_FV_COMPACT_VIEW:
    case FM_FV_THUMBNAIL_VIEW:
        {
            tp = exo_icon_view_get_path_at_pos ((ExoIconView*) folder_view->current_view, x, y);
            exo_icon_view_set_drag_dest_item (EXO_ICON_VIEW (folder_view->current_view), tp, EXO_ICON_VIEW_DROP_INTO);
            break;
        }
    }

    return tp;
}

static gboolean on_drag_motion (GtkWidget *dest_widget,
                                 GdkDragContext *drag_context,
                                 gint x,
                                 gint y,
                                 guint time,
                                 FmFolderView *folder_view)
{
    gboolean ret;
    GdkDragAction action = 0;
    GdkAtom target = gtk_drag_dest_find_target (dest_widget, drag_context, NULL);

    if (target == GDK_NONE)
        return FALSE;

    ret = FALSE;
    
    // files are being dragged
    if (fm_dnd_dest_is_target_supported (folder_view->dnd_dest, target))
    {
        GtkTreePath *tp = get_drop_path (folder_view, x, y);
        if (tp)
        {
            GtkTreeIter it;
            if (gtk_tree_model_get_iter (folder_view->model, &it, tp))
            {
                FmFileInfo *file_info;
                gtk_tree_model_get (folder_view->model, &it, COL_FILE_INFO, &file_info, -1);
                fm_dnd_dest_set_dest_file (folder_view->dnd_dest, file_info);
            }
            gtk_tree_path_free (tp);
        }
        else
        {
            // FIXME_pcm: prevent direct access to data members.
            FmFolderModel *model =  (FmFolderModel*)folder_view->model;
            
            // TODO_axl: use an accessor function...
//            FmPath *dir_path =  model->directory->dir_path;
            fm_dnd_dest_set_dest_file (folder_view->dnd_dest, model->directory->dir_fi);
        }
        action = fm_dnd_dest_get_default_action (folder_view->dnd_dest, drag_context, target);
        ret = action != 0;
    }

    if  (action)
    gdk_drag_status (drag_context, action, time);

    return ret;
}

static void on_drag_leave (GtkWidget *dest_widget,
                                GdkDragContext *drag_context,
                                guint time,
                                FmFolderView *folder_view)
{
    fm_dnd_dest_drag_leave (folder_view->dnd_dest, drag_context, time);
}

static void on_dnd_src_data_get (FmDndSrc *ds, FmFolderView *folder_view)
{
    FmFileInfoList *files = fm_folder_view_get_selected_files (folder_view);
    if (files)
    {
        fm_dnd_src_set_files (ds, files);
        fm_list_unref (files);
    }
}



/*****************************************************************************************
 * 
 * 
 * 
 ****************************************************************************************/
static void on_sel_changed (GObject *obj, FmFolderView *folder_view)
{
    // FIXME_pcm: this is inefficient, but currently there is no better way
    FmFileInfo *files = (FmFileInfo*) fm_folder_view_get_selected_files (folder_view);
    g_signal_emit (folder_view, signals[SEL_CHANGED], 0, files);
    if (files)
        fm_list_unref (files);
}

static void on_sort_col_changed (GtkTreeSortable *sortable, FmFolderView *folder_view)
{
    int col;
    GtkSortType order;
    if (gtk_tree_sortable_get_sort_column_id (sortable, &col, &order))
    {
        folder_view->sort_by = col;
        folder_view->sort_type = order;
        g_signal_emit (folder_view, signals[SORT_CHANGED], 0);
    }
}

/*****************************************************************************************
 * 
 * 
 * 
 ****************************************************************************************/
static void on_folder_unmounted (FmFolder *folder, FmFolderView *folder_view)
{
    switch (folder_view->mode)
    {
    case FM_FV_LIST_VIEW:
        cancel_pending_row_activated (folder_view);
        gtk_tree_view_set_model (GTK_TREE_VIEW (folder_view->current_view), NULL);
        break;
    case FM_FV_ICON_VIEW:
    case FM_FV_COMPACT_VIEW:
    case FM_FV_THUMBNAIL_VIEW:
        exo_icon_view_set_model (EXO_ICON_VIEW (folder_view->current_view), NULL);
        break;
    }
    if (folder_view->model)
    {
        g_signal_handlers_disconnect_by_func (folder_view->model, on_sort_col_changed, folder_view);
        g_object_unref (folder_view->model);
        folder_view->model = NULL;
    }
}

static void on_folder_loaded (FmFolder *folder, FmFolderView *folder_view)
{
    FmFolderModel *model;
    guint icon_size = 0;

    model = fm_folder_model_new (folder, folder_view->show_hidden);
    gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (model), folder_view->sort_by, folder_view->sort_type);
    g_signal_connect (model, "sort-column-changed", G_CALLBACK (on_sort_col_changed), folder_view);

    switch (folder_view->mode)
    {
    case FM_FV_LIST_VIEW:
        cancel_pending_row_activated (folder_view);
        gtk_tree_view_set_model (GTK_TREE_VIEW (folder_view->current_view), (GtkTreeModel*) model);
        icon_size = folder_view->small_icon_size;
        fm_folder_model_set_icon_size (model, icon_size);
        break;
    case FM_FV_ICON_VIEW:
        icon_size = folder_view->big_icon_size;
        fm_folder_model_set_icon_size (model, icon_size);
        exo_icon_view_set_model (EXO_ICON_VIEW (folder_view->current_view), (GtkTreeModel*) model);
        break;
    case FM_FV_COMPACT_VIEW:
        icon_size = folder_view->small_icon_size;
        fm_folder_model_set_icon_size (model, icon_size);
        exo_icon_view_set_model (EXO_ICON_VIEW (folder_view->current_view), (GtkTreeModel*) model);
        break;
    case FM_FV_THUMBNAIL_VIEW:
        icon_size = fm_config->thumbnail_size;
        fm_folder_model_set_icon_size (model, icon_size);
        exo_icon_view_set_model (EXO_ICON_VIEW (folder_view->current_view), (GtkTreeModel*) model);
        break;
    }
    folder_view->model = (GtkTreeModel*) model;
    on_model_loaded (model, folder_view);
}

static FmErrorAction on_folder_err (FmFolder *folder, GError *err, FmSeverity severity, FmFolderView *folder_view)
{
    GtkWindow *parent =  (GtkWindow*)gtk_widget_get_toplevel ((GtkWidget*)folder_view);
    if (err->domain == G_IO_ERROR)
    {
        if (err->code == G_IO_ERROR_NOT_MOUNTED && severity < FM_SEVERITY_CRITICAL)
        {
            if (fm_mount_path (parent, folder->dir_path, TRUE))
                return FM_ERROR_ACTION_RETRY;
        }
        else if (err->code == G_IO_ERROR_FAILED_HANDLED)
        {
            return FM_ERROR_ACTION_CONTINUE;
        }
    }
    fm_show_error (parent, NULL, err->message);
    return FM_ERROR_ACTION_CONTINUE;
}

/*****************************************************************************************
 * 
 * 
 * 
 ****************************************************************************************/
static gboolean on_btn_pressed (GtkWidget *current_view, GdkEventButton *evt, FmFolderView *folder_view)
{
    GList *sels;
    FmFolderViewClickType type = 0;
    GtkTreePath *tp;

    if (!folder_view->model)
        return FALSE;

    // FIXME_pcm: handle single click activation
    if (evt->type == GDK_BUTTON_PRESS)
    {
        // special handling for ExoIconView
        if (evt->button != 1)
        {
            if (folder_view->mode==FM_FV_ICON_VIEW || folder_view->mode==FM_FV_COMPACT_VIEW || folder_view->mode==FM_FV_THUMBNAIL_VIEW)
            {
                // select the item on right click for ExoIconView
                if (exo_icon_view_get_item_at_pos (EXO_ICON_VIEW (current_view), evt->x, evt->y, &tp, NULL))
                {
                    // if the hit item is not currently selected
                    if (!exo_icon_view_path_is_selected (EXO_ICON_VIEW (current_view), tp))
                    {
                        sels = exo_icon_view_get_selected_items ((ExoIconView*) current_view);
                        if (sels) // if there are selected items
                        {
                            exo_icon_view_unselect_all (EXO_ICON_VIEW (current_view)); // unselect all items
                            g_list_foreach (sels,  (GFunc)gtk_tree_path_free, NULL);
                            g_list_free (sels);
                        }
                        exo_icon_view_select_path (EXO_ICON_VIEW (current_view), tp);
                        exo_icon_view_set_cursor (EXO_ICON_VIEW (current_view), tp, NULL, FALSE);
                    }
                    gtk_tree_path_free (tp);
                }
            }
            else if (folder_view->mode == FM_FV_LIST_VIEW
                     && evt->window == gtk_tree_view_get_bin_window (GTK_TREE_VIEW (current_view)))
            {
                // special handling for ExoTreeView
                // Fix #2986834: MAJOR PROBLEM: Deletes Wrong File Frequently.
                GtkTreeViewColumn *col;
                if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (current_view), evt->x, evt->y, &tp, &col, NULL, NULL))
                {
                    GtkTreeSelection *tree_sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (current_view));
                    if (!gtk_tree_selection_path_is_selected (tree_sel, tp))
                    {
                        gtk_tree_selection_unselect_all (tree_sel);
                        if (col == exo_tree_view_get_activable_column (EXO_TREE_VIEW (current_view)))
                        {
                            gtk_tree_selection_select_path (tree_sel, tp);
                            gtk_tree_view_set_cursor (GTK_TREE_VIEW (current_view), tp, NULL, FALSE);
                        }
                    }
                    gtk_tree_path_free (tp);
                }
            }
        }

        // middle click
        if (evt->button == 2)
            type = FM_FV_MIDDLE_CLICK;
        
        // right click
        else if (evt->button == 3)
            type = FM_FV_CONTEXT_MENU;
    }

    if (type != FM_FV_CLICK_NONE)
    {
        sels = fm_folder_view_get_selected_tree_paths (folder_view);
        if (sels || type == FM_FV_CONTEXT_MENU)
        {
            item_clicked (folder_view, sels ? sels->data : NULL, type);
            if (sels)
            {
                g_list_foreach (sels, (GFunc) gtk_tree_path_free, NULL);
                g_list_free (sels);
            }
        }
    }
    
    return FALSE;
}

static void cancel_pending_row_activated (FmFolderView *folder_view)
{
    if (folder_view->row_activated_idle)
    {
        g_source_remove (folder_view->row_activated_idle);
        folder_view->row_activated_idle = 0;
        gtk_tree_row_reference_free (folder_view->activated_row_ref);
        folder_view->activated_row_ref = NULL;
    }
}

// Callbacks...
/*****************************************************************************************
 * 
 * 
 * 
 ****************************************************************************************/
static gboolean on_folder_view_focus_in (GtkWidget *widget, GdkEventFocus *evt)
{
    FmFolderView *folder_view =  (FmFolderView*)widget;
    if (folder_view->current_view)
    {
        gtk_widget_grab_focus (folder_view->current_view);
        return TRUE;
    }
    return FALSE;
}

static void on_chdir (FmFolderView *folder_view, FmPath *dir_path)
{
    GtkWidget *toplevel = gtk_widget_get_toplevel ((GtkWidget*)folder_view);
    
    if (gtk_widget_get_realized (toplevel))
    {
        GdkCursor *cursor = gdk_cursor_new (GDK_WATCH);
        gdk_window_set_cursor (gtk_widget_get_window (toplevel), cursor);
    }
}

static void on_loaded (FmFolderView *folder_view, FmPath *dir_path)
{
    GtkWidget *toplevel = gtk_widget_get_toplevel ((GtkWidget*)folder_view);
    
    if (gtk_widget_get_realized (toplevel))
        gdk_window_set_cursor (gtk_widget_get_window (toplevel), NULL);
}

static void on_model_loaded (FmFolderModel *model, FmFolderView *folder_view)
{
    // TODO_axl: use an accessor function...
    FmFolder *folder = model->directory;

    // FIXME_pcm: prevent direct access to data members
    
    g_signal_emit (folder_view, signals [LOADED], 0, folder->dir_path);
}


static void on_single_click_changed (FmConfig *cfg, FmFolderView *folder_view)
{
    switch (folder_view->mode)
    {
    case FM_FV_LIST_VIEW:
        exo_tree_view_set_single_click ((ExoTreeView*)folder_view->current_view, folder_view->single_click);
        break;
    case FM_FV_ICON_VIEW:
    case FM_FV_COMPACT_VIEW:
    case FM_FV_THUMBNAIL_VIEW:
        exo_icon_view_set_single_click ((ExoIconView*)folder_view->current_view, folder_view->single_click);
        break;
    }
}

static void item_clicked (FmFolderView *folder_view, GtkTreePath *path, FmFolderViewClickType type)
{
    GtkTreeIter it;
    if (path)
    {
        if (gtk_tree_model_get_iter (folder_view->model, &it, path))
        {
            FmFileInfo *file_info;
            gtk_tree_model_get (folder_view->model, &it, COL_FILE_INFO, &file_info, -1);
            g_signal_emit (folder_view, signals[CLICKED], 0, type, file_info);
        }
    }
    else
        g_signal_emit (folder_view, signals[CLICKED], 0, type, NULL);
}

static void on_icon_view_item_activated (ExoIconView *iv, GtkTreePath *path, FmFolderView *folder_view)
{
    item_clicked (folder_view, path, FM_FV_ACTIVATED);
}

static void on_tree_view_row_activated (GtkTreeView *tv, GtkTreePath *path, GtkTreeViewColumn *col, FmFolderView *folder_view)
{
    /* Due to GTK+ and libexo bugs, here a workaround is needed.
     * https://bugzilla.gnome.org/show_bug.cgi?id=612802
     * http://bugzilla.xfce.org/show_bug.cgi?id=6230
     * Gtk+ 2.20+ changed its behavior, which is really bad.
     * row-activated signal is now issued in the second button-press events
     * rather than double click events. The content of the view and model
     * gets changed in row-activated signal handler before button-press-event
     * handling is finished, and this breaks button-press handler of ExoTreeView
     * and causing some selection-related bugs since select function cannot be reset.*/

    cancel_pending_row_activated (folder_view);
    
    if (folder_view->model)
    {
        folder_view->activated_row_ref = gtk_tree_row_reference_new (GTK_TREE_MODEL(folder_view->model), path);
        g_idle_add ((GSourceFunc) on_idle_tree_view_row_activated, folder_view);
    }
}

static gboolean on_idle_tree_view_row_activated (FmFolderView *folder_view)
{
    GtkTreePath *path;
    if (gtk_tree_row_reference_valid (folder_view->activated_row_ref))
    {
        path = gtk_tree_row_reference_get_path (folder_view->activated_row_ref);
        item_clicked (folder_view, path, FM_FV_ACTIVATED);
    }
    gtk_tree_row_reference_free (folder_view->activated_row_ref);
    folder_view->activated_row_ref = NULL;
    folder_view->row_activated_idle = 0;
    return FALSE;
}



