/***********************************************************************************************************************
 * fm-config.vala
 * 
 * Copyright 2012 Axel FILMORE <axel.filmore@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 2.
 * http://www.gnu.org/licenses/gpl-2.0.txt
 * 
 * This software is an experimental rewrite of PcManFm originally written by Hong Jen Yee aka PCMan for LXDE project.
 * 
 * Purpose: 
 * 
 * 
 * 
 **********************************************************************************************************************/

#ifndef __GTK3_MIGRATION_H__
#define __GTK3_MIGRATION_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS


#if !ENABLE_GTK3
    
    /*** gdk_app_launch_context_new () is deprecated since GDK 3.0 ***/
    #define gdk_display_get_app_launch_context(dpy) gdk_app_launch_context_new ()
    
    
    #define GDK_KEY_Left    GDK_Left
    #define GDK_KEY_Right   GDK_Right

#endif


G_END_DECLS

#endif


