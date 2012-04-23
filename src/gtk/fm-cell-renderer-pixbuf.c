/*
 *      fm-cell-renderer-pixbuf.c
 *      
 *      Copyright 2010 PCMan <pcman.tw@gmail.com>
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
 */

#include "fm-cell-renderer-pixbuf.h"


static GdkPixbuf* link_icon = NULL;

/* GdkPixbuf RGB C-Source image dump 1-byte-run-length-encoded */

#ifdef __SUNPRO_C
#pragma align 4  (link_icon_data)
#endif
#ifdef __GNUC__
static const guint8 link_icon_data[] __attribute__  ((__aligned__  (4))) = 
#else
static const guint8 link_icon_data[] = 
#endif
{ ""
  /* Pixbuf magic  (0x47646b50) */
  "GdkP"
  /* length: header  (24) + pixel_data  (148) */
  "\0\0\0\254"
  /* pixdata_type  (0x2010001) */
  "\2\1\0\1"
  /* rowstride  (30) */
  "\0\0\0\36"
  /* width  (10) */
  "\0\0\0\12"
  /* height  (10) */
  "\0\0\0\12"
  /* pixel_data: */
  "\1\0\0\0\211\200\200\200\1\0\0\0\210\377\377\377\2\200\200\200\0\0\0"
  "\202\377\377\377\205\0\0\0\3\377\377\377\200\200\200\0\0\0\203\377\377"
  "\377\204\0\0\0\3\377\377\377\200\200\200\0\0\0\202\377\377\377\205\0"
  "\0\0\4\377\377\377\200\200\200\0\0\0\377\377\377\206\0\0\0\4\377\377"
  "\377\200\200\200\0\0\0\377\377\377\203\0\0\0\202\377\377\377\5\0\0\0"
  "\377\377\377\200\200\200\0\0\0\377\377\377\203\0\0\0\204\377\377\377"
  "\2\200\200\200\0\0\0\210\377\377\377\1\200\200\200\212\0\0\0"};



static void fm_cell_renderer_pixbuf_finalize (GObject *object);

static void fm_cell_renderer_pixbuf_set_property (GObject *object,
                                                  guint param_id,
                                                  const GValue *value,
                                                  GParamSpec *pspec);

static void fm_cell_renderer_pixbuf_get_property (GObject *object,
                                                  guint param_id,
                                                  GValue *value,
                                                  GParamSpec *pspec);

static void fm_cell_renderer_pixbuf_get_size (GtkCellRenderer *cell,
                                              GtkWidget *widget,
                                              GdkRectangle *rectangle,
                                              gint *x_offset,
                                              gint *y_offset,
                                              gint *width,
                                              gint *height);

enum
{
    PROP_INFO = 1,
    N_PROPS
};

G_DEFINE_TYPE (FmCellRendererPixbuf, fm_cell_renderer_pixbuf, GTK_TYPE_CELL_RENDERER_PIXBUF);

static void fm_cell_renderer_pixbuf_class_init (FmCellRendererPixbufClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);
    GtkCellRendererClass *cell_class = GTK_CELL_RENDERER_CLASS (klass);

	g_object_class->finalize = fm_cell_renderer_pixbuf_finalize;
    g_object_class->get_property = fm_cell_renderer_pixbuf_get_property;
    g_object_class->set_property = fm_cell_renderer_pixbuf_set_property;

    cell_class->get_size = fm_cell_renderer_pixbuf_get_size;
    cell_class->render = fm_cell_renderer_pixbuf_render;


    g_object_class_install_property (g_object_class,
                                     PROP_INFO,
                                     g_param_spec_pointer ("info",
                                                           "File info",
                                                           "File info",
                                                           G_PARAM_READWRITE));
}

static void fm_cell_renderer_pixbuf_finalize (GObject *object)
{
	FmCellRendererPixbuf *self;

	g_return_if_fail (object != NULL);
	g_return_if_fail (FM_IS_CELL_RENDERER_PIXBUF (object));

	self = FM_CELL_RENDERER_PIXBUF (object);
    if (self->fi)
        fm_file_info_unref (self->fi);

	G_OBJECT_CLASS (fm_cell_renderer_pixbuf_parent_class)->finalize (object);
}


static void fm_cell_renderer_pixbuf_init (FmCellRendererPixbuf *self)
{
    if (!link_icon)
    {
        link_icon = gdk_pixbuf_new_from_inline (sizeof (link_icon_data),
                                                link_icon_data,
                                                FALSE,
                                                NULL);
                                                
        g_object_add_weak_pointer ((GObject*)link_icon, (gpointer)&link_icon);
    }
    else
    {
        g_object_ref (link_icon);
    }
}


GtkCellRenderer *fm_cell_renderer_pixbuf_new (void)
{
	return g_object_new (FM_TYPE_CELL_RENDERER_PIXBUF, NULL);
}

static void fm_cell_renderer_pixbuf_set_property (GObject *object,
                                                  guint param_id,
                                                  const GValue *value,
                                                  GParamSpec *psec)
{
    FmCellRendererPixbuf* renderer =  (FmCellRendererPixbuf*)object;
    switch (param_id)
    {
    case PROP_INFO:
        if (renderer->fi)
            fm_file_info_unref (renderer->fi);
        renderer->fi = fm_file_info_ref ((FmFileInfo*)g_value_get_pointer (value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, psec);
        break;
    }
}

static void fm_cell_renderer_pixbuf_get_property (GObject *object,
                                                  guint param_id,
                                                  GValue *value,
                                                  GParamSpec *psec)
{
    FmCellRendererPixbuf *renderer =  (FmCellRendererPixbuf*) object;
    
    switch (param_id)
    {
        case PROP_INFO:
            g_value_set_pointer (value, renderer->fi ? fm_file_info_ref (renderer->fi) : NULL);
        break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, psec);
        break;
    }
}

void fm_cell_renderer_pixbuf_set_fixed_size (FmCellRendererPixbuf* render, gint w, gint h)
{
    render->fixed_w = w;
    render->fixed_h = h;
}

void fm_cell_renderer_pixbuf_get_size (GtkCellRenderer *cell,
						               GtkWidget *widget,
						               GdkRectangle  *rectangle,
                                       gint *x_offset,
						               gint *y_offset,
						               gint *width,
						               gint *height)
{
    FmCellRendererPixbuf *render =  (FmCellRendererPixbuf*) cell;
    if (render->fixed_w > 0 && render->fixed_h > 0)
    {
        *width = render->fixed_w;
        *height = render->fixed_h;
    }
    else
    {
        GTK_CELL_RENDERER_CLASS (fm_cell_renderer_pixbuf_parent_class)->get_size (cell, widget, rectangle, x_offset, y_offset, width, height);
    }
}

/***
 * In Gtk3 the GdkWindow is replaced by a Cairo context...
 * 
 * 
    http://developer.gnome.org/gtk/2.24/GtkCellRenderer.html#gtk-cell-renderer-render
*       http://developer.gnome.org/gtk3/stable/GtkCellRenderer.html#gtk-cell-renderer-render

void                gtk_cell_renderer_render            (GtkCellRenderer *cell,
                                                     GdkWindow *window,
                                                     GtkWidget *widget,
                                                     const GdkRectangle *background_area,
                                                     const GdkRectangle *cell_area,
                                                     const GdkRectangle *expose_area,
                                                     GtkCellRendererState flags);


void                gtk_cell_renderer_render            (GtkCellRenderer *cell,
                                                     cairo_t *cr,
                                                     GtkWidget *widget,
                                                     const GdkRectangle *background_area,
                                                     const GdkRectangle *cell_area,
                                                     GtkCellRendererState flags);
***/

#if GTK_CHECK_VERSION (3, 0, 8)
void fm_cell_renderer_pixbuf_render (GtkCellRenderer *cell,
						             cairo_t *cr,
						             GtkWidget *widget,
						             GdkRectangle *background_area,
						             GdkRectangle *cell_area,
						             GtkCellRendererState flags)
{
    
    FmCellRendererPixbuf* render =  (FmCellRendererPixbuf*)cell;
    
    /* we don't need to follow state for prelit items */
    if (flags & GTK_CELL_RENDERER_PRELIT)
        flags &= ~GTK_CELL_RENDERER_PRELIT;
    
    GTK_CELL_RENDERER_CLASS (fm_cell_renderer_pixbuf_parent_class)->render (cell,
                                                                            cr,
                                                                            widget,
                                                                            background_area,
                                                                            cell_area,
                                                                            flags);

    if (render->fi && G_UNLIKELY (fm_file_info_is_symlink (render->fi)))
    {
        GdkRectangle pix_rect;
        GdkPixbuf* pix;
        
        g_object_get (render, "pixbuf", &pix, NULL);
        
        if (pix)
        {
            int x = cell_area->x;
            int y = (cell_area->y +  (cell_area->height - gdk_pixbuf_get_height (pix))/2) + gdk_pixbuf_get_height (pix) -10;
            
            pix_rect.x = x;
            pix_rect.y = y;
            pix_rect.width = gdk_pixbuf_get_width (pix);
            pix_rect.height = gdk_pixbuf_get_height (pix);
            
            //cairo_t *cr = gdk_cairo_create (window);
            gdk_cairo_set_source_pixbuf (cr, link_icon, x, y);
            gdk_cairo_rectangle (cr, &pix_rect);
            cairo_fill (cr);
            
            //cairo_destroy (cr);
            
            //~ gdk_draw_pixbuf (GDK_DRAWABLE (window), NULL, link_icon, 0, 0,
                              //~ x, y, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);
            
            g_object_unref (pix);
        }
    }

#else
void fm_cell_renderer_pixbuf_render (GtkCellRenderer *cell,
						             GdkWindow *window,
						             GtkWidget *widget,
						             GdkRectangle *background_area,
						             GdkRectangle *cell_area,
						             GdkRectangle *expose_area,
						             GtkCellRendererState flags)
{
    
    FmCellRendererPixbuf* render =  (FmCellRendererPixbuf*)cell;
    
    /* we don't need to follow state for prelit items */
    if (flags & GTK_CELL_RENDERER_PRELIT)
        flags &= ~GTK_CELL_RENDERER_PRELIT;
    
    GTK_CELL_RENDERER_CLASS (fm_cell_renderer_pixbuf_parent_class)->render (cell,
                                                                            window,
                                                                            widget,
                                                                            background_area,
                                                                            cell_area,
                                                                            expose_area,
                                                                            flags);

    if (render->fi && G_UNLIKELY (fm_file_info_is_symlink (render->fi)))
    {
        GdkRectangle pix_rect;
        GdkPixbuf* pix;
        
        g_object_get (render, "pixbuf", &pix, NULL);
        
        if (pix)
        {
            //~ int x = cell_area->x +  (cell_area->width - gdk_pixbuf_get_width (pix))/2;
            //~ int y = cell_area->y +  (cell_area->height - gdk_pixbuf_get_height (pix))/2;
            
            int x = cell_area->x;
            int y = (cell_area->y +  (cell_area->height - gdk_pixbuf_get_height (pix))/2) + gdk_pixbuf_get_height (pix) -10;
            
            pix_rect.x = x;
            pix_rect.y = y;
            pix_rect.width = gdk_pixbuf_get_width (pix);
            pix_rect.height = gdk_pixbuf_get_height (pix);
            
            cairo_t *cr = gdk_cairo_create (window);
            gdk_cairo_set_source_pixbuf (cr, link_icon, x, y);
            gdk_cairo_rectangle (cr, &pix_rect);
            cairo_fill (cr);
            
            cairo_destroy (cr);
            
            //~ gdk_draw_pixbuf (GDK_DRAWABLE (window), NULL, link_icon, 0, 0,
                              //~ x, y, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);
            
            g_object_unref (pix);
        }
    }
#endif
}


