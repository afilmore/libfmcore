/***********************************************************************************************************************
 * 
 *      fm-file-menu.h
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
 * 
 **********************************************************************************************************************/
#ifndef __FM_FILE_MENU__
#define __FM_FILE_MENU__

#include <gtk/gtk.h>

#include "fm-file-info-list.h"
#include "fm-gtk-launcher.h"


G_BEGIN_DECLS

typedef struct _FmFileMenu FmFileMenu;

struct _FmFileMenu
{
    // Input Parameters...
    GtkWindow           *parent;
    FmPath              *current_directory;
    FmFileInfoList      *file_infos;
    gboolean            auto_destroy : 1;
    
    // Open Folder Function...
    FmLaunchFolderFunc  folder_func;
    gpointer            folder_func_data;

    // Gtk Builder...
    GtkUIManager        *ui;
    GtkActionGroup      *action_group;
    
    // Returned Menu...
    GtkMenu             *menu;

    // Same MimeType...
    gboolean            same_type : 1;
};


FmFileMenu      *fm_file_menu_new_for_files         (GtkWindow *parent, FmFileInfoList *files, FmPath *current_directory,
                                                     gboolean auto_destroy);
                                            
void            fm_file_menu_set_folder_func        (FmFileMenu *file_menu,
                                                     FmLaunchFolderFunc func, gpointer user_data);

GtkMenu         *fm_file_menu_get_menu              (FmFileMenu *file_menu);

void            fm_file_menu_destroy                (FmFileMenu *file_menu);

GtkUIManager    *fm_file_menu_get_ui                (FmFileMenu *file_menu);
GtkActionGroup  *fm_file_menu_get_action_group      (FmFileMenu *file_menu);


/*****************************************************************************************
 *  Note: Call fm_list_ref () if you need to own a reference to the returned list...
 * 
 * 
 ****************************************************************************************/
FmFileInfoList  *fm_file_menu_get_file_info_list    (FmFileMenu *file_menu);

G_END_DECLS
#endif



