/***********************************************************************************************************************
 * 
 *      fm-user-input-dlg.c
 *
 *      Copyright 2009 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
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

#include "fm-user-input-dlg.h"

#include <glib/gi18n-lib.h>


static GtkDialog *  _fm_get_user_input_dialog    (GtkWindow *parent, const char *title, const char *msg);
static gchar *      _fm_user_input_dialog_run    (GtkDialog *dialog, GtkEntry *entry);


gchar *fm_get_user_input (GtkWindow *parent, const char *title, const char *msg, const char *default_text)
{
    GtkDialog *dialog = _fm_get_user_input_dialog (parent, title, msg);
    GtkWidget *entry = gtk_entry_new ();
    gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);

    if (default_text && default_text[0])
        gtk_entry_set_text (GTK_ENTRY (entry), default_text);

    return _fm_user_input_dialog_run (dialog,  GTK_ENTRY (entry));
}

/** currently unused...
FmPath *fm_get_user_input_path (GtkWindow *parent, const char *title, const char *msg, FmPath *default_path)
{

    GtkDialog *dialog = _fm_get_user_input_dialog (parent, title, msg);
    GtkWidget *entry = gtk_entry_new ();
    char *str, *path_str = NULL;
    FmPath *path;

    gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);

    if (default_path)
    {
        path_str = fm_path_display_name (default_path, FALSE);
        gtk_entry_set_text (GTK_ENTRY (entry), path_str);
    }

    str = _fm_user_input_dialog_run (dialog,  GTK_ENTRY (entry));
    path = fm_path_new_for_str (str);

    g_free (path_str);
    g_free (str);
    return path;
} **/


gchar *fm_get_user_input_rename (GtkWindow *parent, const char *title, const char *msg, const char *default_text)
{
    GtkDialog *dialog = _fm_get_user_input_dialog (parent, title, msg);
    GtkWidget *entry = gtk_entry_new ();
    gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);

    if (default_text && default_text[0])
    {
        gtk_entry_set_text (GTK_ENTRY (entry), default_text);
        // only select filename part without extension name.
        if (default_text[1])
        {
            /**FIXME_pcm: handle the special case for *.tar.gz or *.tar.bz2
              *We should exam the file extension with g_content_type_guess, and
              *find out a longest valid extension name.
              *For example, the extension name of foo.tar.gz is .tar.gz, not .gz. */
            const char *dot = g_utf8_strrchr (default_text, -1, '.');
            if (dot)
                gtk_editable_select_region (GTK_EDITABLE (entry), 0, g_utf8_pointer_to_offset (default_text, dot));
            else
                gtk_editable_select_region (GTK_EDITABLE (entry), 0, -1);
            /*
            const char *dot = default_text;
            while (dot = g_utf8_strchr (dot + 1, -1, '.'))
            {
                gboolean uncertain;
                char *type = g_content_type_guess (dot-1, NULL, 0, &uncertain);
                if (!g_content_type_is_unknown (type))
                {
                    g_free (type);
                    gtk_editable_select_region (entry, 0, g_utf8_pointer_to_offset (default_text, dot));
                    break;
                }
                g_free (type);
            }
            */
        }
    }

    return _fm_user_input_dialog_run (dialog,  GTK_ENTRY (entry));
}

static GtkDialog *_fm_get_user_input_dialog (GtkWindow *parent, const char *title, const char *msg)
{
    GtkWidget *dialog = gtk_dialog_new_with_buttons (title,
                                                  parent,
                                                  0,
                                                  GTK_STOCK_CANCEL,
                                                  GTK_RESPONSE_CANCEL,
                                                  GTK_STOCK_OK,
                                                  GTK_RESPONSE_OK,
                                                  NULL);
    GtkWidget *label = gtk_label_new (msg);
    gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);

    gtk_dialog_set_alternative_button_order (GTK_DIALOG (dialog), GTK_RESPONSE_OK, GTK_RESPONSE_CANCEL, -1);
    gtk_box_set_spacing ((GtkBox*)gtk_dialog_get_content_area (GTK_DIALOG (dialog)), 6);
    gtk_box_pack_start ((GtkBox*)gtk_dialog_get_content_area (GTK_DIALOG (dialog)), label, FALSE, TRUE, 6);

    gtk_container_set_border_width (GTK_CONTAINER ((GtkBox*)gtk_dialog_get_content_area (GTK_DIALOG (dialog))), 12);
    gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
    gtk_window_set_default_size (GTK_WINDOW (dialog), 480, -1);

    return (GtkDialog*) dialog;
}

static gchar *_fm_user_input_dialog_run (GtkDialog *dialog, GtkEntry *entry)
{
    char *str = NULL;
    int sel_start, sel_end;
    gboolean has_sel;

    /*FIXME_pcm: this workaround is used to overcome bug of gtk+.
      *gtk+ seems to ignore select region and select all text for entry in dialog. */
    
    has_sel = gtk_editable_get_selection_bounds (GTK_EDITABLE (entry), &sel_start, &sel_end);

    GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG(dialog));
    gtk_container_add (GTK_CONTAINER(content_area), GTK_WIDGET(entry));
    
    // original code...
    // gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), GTK_WIDGET (entry), FALSE, TRUE, 6);
    
    gtk_widget_show_all (GTK_WIDGET (dialog));

    if (has_sel)
        gtk_editable_select_region (GTK_EDITABLE (entry), sel_start, sel_end);

    while (gtk_dialog_run (dialog) == GTK_RESPONSE_OK)
    {
        const char *pstr = gtk_entry_get_text (entry);
        if (pstr && *pstr)
        {
            str = g_strdup (pstr);
            break;
        }
    }
    gtk_widget_destroy (GTK_WIDGET (dialog));
    return str;
}



