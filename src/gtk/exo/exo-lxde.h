/***********************************************************************************************************************
 * 
 *      exo-lxde.h
 *      
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
 *      Purpose: Changes done by LXDE developers to the original Libexo.
 *
 * 
 **********************************************************************************************************************/
#ifndef __EXO_LXDE_H__
#define __EXO_LXDE_H__

#include <glib/gi18n-lib.h>

#include "exo-icon-view.h"
#include "exo-tree-view.h"
#include "exo-marshal.h"
#include "exo-private.h"

G_BEGIN_DECLS;

// From exo/exo-marshal.h
#if defined(G_PARAM_STATIC_NAME) && defined(G_PARAM_STATIC_NICK) && defined(G_PARAM_STATIC_BLURB)
#define EXO_PARAM_READABLE  (G_PARAM_READABLE \
                           | G_PARAM_STATIC_NAME \
                           | G_PARAM_STATIC_NICK \
                           | G_PARAM_STATIC_BLURB)
#define EXO_PARAM_WRITABLE  (G_PARAM_WRITABLE \
                           | G_PARAM_STATIC_NAME \
                           | G_PARAM_STATIC_NICK \
                           | G_PARAM_STATIC_BLURB)
#define EXO_PARAM_READWRITE (G_PARAM_READWRITE \
                           | G_PARAM_STATIC_NAME \
                           | G_PARAM_STATIC_NICK \
                           | G_PARAM_STATIC_BLURB)
#else
#define EXO_PARAM_READABLE  (G_PARAM_READABLE)
#define EXO_PARAM_WRITABLE  (G_PARAM_WRITABLE)
#define EXO_PARAM_READWRITE (G_PARAM_READWRITE)
#endif

#define             I_(string)  g_intern_static_string(string)

#define exo_noop_false    gtk_false

// From exo/exo-enum-types.h
GType
exo_icon_view_layout_mode_get_type (void);
#define EXO_TYPE_ICON_VIEW_LAYOUT_MODE (exo_icon_view_layout_mode_get_type())


G_END_DECLS;
#endif



