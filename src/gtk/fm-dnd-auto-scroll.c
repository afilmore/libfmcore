/*
 *      fm-dnd-auto-scroll.c
 *
 *      Copyright 2010 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
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

#include "fm-dnd-auto-scroll.h"

#define SCROLL_EDGE_SIZE 15

typedef struct _FmDndAutoScroll FmDndAutoScroll;
struct _FmDndAutoScroll
{
    GtkWidget* widget;
    guint timeout;
    GtkAdjustment* hadj;
    GtkAdjustment* vadj;
};

static GQuark data_id = 0;

static gboolean on_auto_scroll (FmDndAutoScroll* dnd_autoscroll)
{
    int x, y;
    GtkAdjustment* vadjustment = dnd_autoscroll->vadj;
    GtkAdjustment* hadjustment = dnd_autoscroll->hadj;
    GtkWidget* widget = dnd_autoscroll->widget;

    GdkDeviceManager *device_manager;
    GdkDevice *pointer;
 
    device_manager = gdk_display_get_device_manager (gtk_widget_get_display (widget));
    pointer = gdk_device_manager_get_client_pointer (device_manager);
    gdk_window_get_device_position (gtk_widget_get_window (widget), pointer, &x, &y, NULL);

    /*
       HACK.
       Sometimes we do not get drag-leave signal. (Why?)
       This check prevents infinite scrolling.
    */

    GtkAllocation allocation = {0};
    gtk_widget_get_allocation (widget, &allocation);
    
    if (y < 0
        || y > allocation.height
        || x < 0
        || x > allocation.width)
    {
        dnd_autoscroll->timeout = 0;
        return FALSE;
    }

    if (vadjustment)
    {
        gdouble vadjustment_step_increment  = gtk_adjustment_get_step_increment (vadjustment);
        gdouble vadjustment_page_size       = gtk_adjustment_get_page_size (vadjustment);
        gdouble vadjustment_lower           = gtk_adjustment_get_lower (vadjustment);
        gdouble vadjustment_upper           = gtk_adjustment_get_upper (vadjustment);
        gdouble vadjustment_value           = gtk_adjustment_get_value (vadjustment);
        
        if (y < SCROLL_EDGE_SIZE) /* scroll up */
        {
            if (vadjustment_value > vadjustment_lower)
            {
                gtk_adjustment_set_value (vadjustment, vadjustment_value - vadjustment_step_increment);
                
                vadjustment_value = gtk_adjustment_get_value (vadjustment);
                if (vadjustment_value < vadjustment_lower)
                    gtk_adjustment_set_value (vadjustment, vadjustment_lower);
            }
        }
        else if (y > (allocation.height - SCROLL_EDGE_SIZE))
        {
            /* scroll down */
            if (gtk_adjustment_get_value (vadjustment) < vadjustment_upper - vadjustment_page_size)
            {
                gtk_adjustment_set_value (vadjustment, vadjustment_value + vadjustment_step_increment);
                
                vadjustment_value = gtk_adjustment_get_value (vadjustment);
                if (vadjustment_value > vadjustment_upper - vadjustment_page_size)
                    gtk_adjustment_set_value (vadjustment, vadjustment_upper - vadjustment_page_size);
            }
        }
        
        gtk_adjustment_value_changed (vadjustment);
    }

    if (hadjustment)
    {
        gdouble hadjustment_step_increment  = gtk_adjustment_get_step_increment (hadjustment);
        gdouble hadjustment_page_size       = gtk_adjustment_get_page_size (hadjustment);
        gdouble hadjustment_lower           = gtk_adjustment_get_lower (hadjustment);
        gdouble hadjustment_upper           = gtk_adjustment_get_upper (hadjustment);
        gdouble hadjustment_value           = gtk_adjustment_get_value (hadjustment);
        
        if (x < SCROLL_EDGE_SIZE) /* scroll to left */
        {
            if (gtk_adjustment_get_value (hadjustment) > hadjustment_lower)
            {
                gtk_adjustment_set_value (hadjustment, hadjustment_value - hadjustment_step_increment);
                
                hadjustment_value = gtk_adjustment_get_value (hadjustment);
                if (gtk_adjustment_get_value (hadjustment) < hadjustment_lower)
                    gtk_adjustment_set_value (hadjustment, hadjustment_lower);
            }
        }
        else if (x > (allocation.width - SCROLL_EDGE_SIZE))
        {
            /* scroll to right */
            if (hadjustment_value < hadjustment_upper - hadjustment_page_size)
            {
                gtk_adjustment_set_value (hadjustment, hadjustment_value + hadjustment_step_increment);
                
                hadjustment_value = gtk_adjustment_get_value (hadjustment);
                if (hadjustment_value > hadjustment_upper - hadjustment_page_size)
                    gtk_adjustment_set_value (hadjustment, hadjustment_upper - hadjustment_page_size);
            }
        }
        
        gtk_adjustment_value_changed (hadjustment);
    }
//~ #endif
    return TRUE;
}

static gboolean on_drag_motion (GtkWidget *widget, GdkDragContext *drag_context,
                               gint x, gint y, guint time, gpointer user_data)
{
    FmDndAutoScroll* dnd_autoscroll = (FmDndAutoScroll*)user_data;
    /* FIXME: this is a dirty hack for GTK_TREE_MODEL_ROW. When dragging GTK_TREE_MODEL_ROW
     * we cannot receive "drag-leave" message. So weied! Is it a gtk+ bug? */
    GdkAtom target = gtk_drag_dest_find_target (widget, drag_context, NULL);
    if (target == GDK_NONE)
        return FALSE;
    if (0 == dnd_autoscroll->timeout) /* install a scroll timeout if needed */
    {
        dnd_autoscroll->timeout = gdk_threads_add_timeout (150, (GSourceFunc)on_auto_scroll, dnd_autoscroll);
    }
    return FALSE;
}

static void on_drag_leave (GtkWidget *widget, GdkDragContext *drag_context,
                          guint time, gpointer user_data)
{
    FmDndAutoScroll* dnd_autoscroll = (FmDndAutoScroll*)user_data;
    if (dnd_autoscroll->timeout)
    {
        g_source_remove (dnd_autoscroll->timeout);
        dnd_autoscroll->timeout = 0;
    }
}

static void fm_dnd_auto_scroll_free (FmDndAutoScroll* dnd_autoscroll)
{
    if (dnd_autoscroll->timeout)
        g_source_remove (dnd_autoscroll->timeout);
    if (dnd_autoscroll->hadj)
        g_object_unref (dnd_autoscroll->hadj);
    if (dnd_autoscroll->vadj)
        g_object_unref (dnd_autoscroll->vadj);

    g_signal_handlers_disconnect_by_func (dnd_autoscroll->widget, on_drag_motion, dnd_autoscroll);
    g_signal_handlers_disconnect_by_func (dnd_autoscroll->widget, on_drag_leave, dnd_autoscroll);
    g_slice_free (FmDndAutoScroll, dnd_autoscroll);
}

/**
 * fm_dnd_set_dest_auto_scroll
 * @drag_dest_widget a drag destination widget
 * @hadj: horizontal GtkAdjustment
 * @vadj: vertical GtkAdjustment
 *
 * This function installs a "drag-motion" handler to the dest widget
 * to support auto-scroll when the dragged item is near the margin
 * of the destination widget. For example, when a user drags an item
 * over the bottom of a GtkTreeView, the desired behavior should be
 * to scroll up the content of the tree view and to expose the items
 * below currently visible region. So the user can drop on them.
 */
void fm_dnd_set_dest_auto_scroll (GtkWidget* drag_dest_widget,
                                 GtkAdjustment* hadj, GtkAdjustment* vadj)
{
    FmDndAutoScroll* dnd_autoscroll;
    if (G_UNLIKELY (data_id == 0))
        data_id = g_quark_from_static_string ("FmDndAutoScroll");

    if (G_UNLIKELY (!hadj && !vadj))
    {
        g_object_set_qdata_full (G_OBJECT (drag_dest_widget), data_id, NULL, NULL);
        return;
    }

    dnd_autoscroll = g_slice_new (FmDndAutoScroll);
    dnd_autoscroll->widget = drag_dest_widget; /* no g_object_ref is needed here */
    dnd_autoscroll->timeout = 0;
    dnd_autoscroll->hadj = hadj ? GTK_ADJUSTMENT (g_object_ref (hadj)) : NULL;
    dnd_autoscroll->vadj = vadj ? GTK_ADJUSTMENT (g_object_ref (vadj)) : NULL;

    g_object_set_qdata_full ( (GObject*) drag_dest_widget, data_id,
            dnd_autoscroll, (GDestroyNotify)fm_dnd_auto_scroll_free);

    g_signal_connect (drag_dest_widget, "drag-motion",
                     G_CALLBACK (on_drag_motion), dnd_autoscroll);
    g_signal_connect (drag_dest_widget, "drag-leave",
                     G_CALLBACK (on_drag_leave), dnd_autoscroll);
}

/**
 * fm_dnd_unset_dest_auto_scroll
 * @drag_dest_widget drag destination widget.
 *
 * Unsets what has been done by fm_dnd_set_dest_auto_scroll ()
 */
void fm_dnd_unset_dest_auto_scroll (GtkWidget* drag_dest_widget)
{
    if (G_UNLIKELY (data_id == 0))
        data_id = g_quark_from_static_string ("FmDndAutoScroll");
    g_object_set_qdata_full (G_OBJECT (drag_dest_widget), data_id, NULL, NULL);
}



