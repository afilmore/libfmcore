/***********************************************************************************************************************
 * 
 *      exo-lxde.c
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

#include "exo-lxde.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// LXDE_CHANGES: #ifdef HAVE_MATH_H
#include <math.h>
// LXDE_CHANGES: #endif
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif


GType
exo_icon_view_layout_mode_get_type (void)
{
    static GType type = 0;
    if (type == 0) {
    static const GEnumValue values[] = {
    { EXO_ICON_VIEW_LAYOUT_ROWS, "EXO_ICON_VIEW_LAYOUT_ROWS", "rows" },
    { EXO_ICON_VIEW_LAYOUT_COLS, "EXO_ICON_VIEW_LAYOUT_COLS", "cols" },
    { 0, NULL, NULL }
    };
    type = g_enum_register_static ("ExoIconViewLayoutMode", values);
  }
    return type;
}



