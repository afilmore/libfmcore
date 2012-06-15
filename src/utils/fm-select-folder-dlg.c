/***********************************************************************************************************************
 * 
 *      fm-select-folder-dlg.c
 *
 *      Copyright 2009 PCMan <pcman@debian>
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "fm-select-folder-dlg.h"

#include <glib/gi18n-lib.h>


FmPath *fm_select_folder (GtkWindow *parent, const char *title)
{
    GtkFileChooser *chooser;
    
    chooser = (GtkFileChooser*) gtk_file_chooser_dialog_new (title ? title : _("Please select a folder"),
                                                             parent,
                                                             GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                             GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                             GTK_STOCK_OK, GTK_RESPONSE_OK,
                                                             NULL);
    
    gtk_dialog_set_alternative_button_order ((GtkDialog*) chooser,
                                             GTK_RESPONSE_CANCEL,
                                             GTK_RESPONSE_OK,
                                             NULL);
    
    FmPath *path;
    
    if (gtk_dialog_run ((GtkDialog*) chooser) == GTK_RESPONSE_OK)
    {
        GFile *file = gtk_file_chooser_get_file (chooser);
        path = fm_path_new_for_gfile (file);
        g_object_unref (file);
    }
    else
    {
        path = NULL;
    }
    
    gtk_widget_destroy ((GtkWidget*) chooser);
    
    return path;
}


