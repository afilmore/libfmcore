/***********************************************************************************************************************
 * gtk3-migration.h
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
    
    #define gdk_x11_window_get_xid(win) (GDK_WINDOW_XID(win))
    
    /*** Change GtkEditable typedef from GtkEditableClass to GtkEditabeInterface
    http://mail.gnome.org/archives/commits-list/2010-September/msg07032.html ***/
    
    #define GtkEditableInterface GtkEditableClass

    /***
     * Replace GDK_<keyname> with GDK_KEY_<keyname>
     * Key constants have gained a _KEY_ infix. For example, GDK_a is now GDK_KEY_a. In GTK+ 2,
     * the old names continue to be available. In GTK+ 3 however, the old names will require an
     * explicit include of the gdkkeysyms-compat.h header.
    #define GDK_KEY_a               GDK_a
    #define GDK_KEY_f               GDK_f
    #define GDK_KEY_F               GDK_F
    #define GDK_KEY_g               GDK_g
    #define GDK_KEY_G               GDK_G
    #define GDK_KEY_n               GDK_n
    #define GDK_KEY_p               GDK_p
    
    #define GDK_KEY_Left            GDK_Left
    #define GDK_KEY_Right           GDK_Right
    #define GDK_KEY_KP_Right        GDK_KP_Right
    #define GDK_KEY_KP_Left         GDK_KP_Left
    #define GDK_KEY_Up              GDK_Up
    #define GDK_KEY_KP_Up           GDK_KP_Up
    #define GDK_KEY_Down            GDK_Down
    #define GDK_KEY_KP_Down         GDK_KP_Down
    #define GDK_KEY_Home            GDK_Home
    #define GDK_KEY_KP_Home         GDK_KP_Home
    #define GDK_KEY_Page_Up         GDK_Page_Up
    #define GDK_KEY_Page_KP_Up      GDK_Page_KP_Up
    #define GDK_KEY_Page_Down       GDK_Page_Down
    #define GDK_KEY_Page_KP_Down    GDK_Page_KP_Down
    #define GDK_KEY_End             GDK_End
    #define GDK_KEY_KP_End          GDK_KP_End
    
    #define GDK_KEY_Tab             GDK_Tab
    #define GDK_KEY_space           GDK_space
    #define GDK_KEY_Return          GDK_Return
    #define GDK_KEY_ISO_Enter       GDK_ISO_Enter
    #define GDK_KEY_KP_Enter        GDK_KP_Enter
    #define GDK_KEY_Escape          GDK_Escape
    ***/

#endif


G_END_DECLS

#endif


