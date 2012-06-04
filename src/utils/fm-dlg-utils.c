/***********************************************************************************************************************
 * 
 *      fm-dlg-utils.c
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

#include "fm-dlg-utils.h"

#include <glib/gi18n-lib.h>
//~ #include <gio/gdesktopappinfo.h>
//~ 
//~ #include "fm-file-ops-job.h"
//~ #include "fm-progress-dlg.h"
//~ #include "fm-app-chooser-dlg.h"
//~ 
//~ #include "fm-config.h"



static GtkDialog *  _fm_get_user_input_dialog    (GtkWindow *parent, const char *title, const char *msg);
static gchar *      _fm_user_input_dialog_run    (GtkDialog *dialog, GtkEntry *entry);

void fm_show_error (GtkWindow *parent, const char *title, const char *msg)
{
    GtkWidget *dialog = gtk_message_dialog_new (parent, 0,
                                            GTK_MESSAGE_ERROR,
                                            GTK_BUTTONS_OK, "%s", msg);
    gtk_window_set_title ((GtkWindow*)dialog, title ? title : _ ("Error"));
    gtk_dialog_run ((GtkDialog*)dialog);
    gtk_widget_destroy (dialog);
}

gboolean fm_yes_no (GtkWindow *parent, const char *title, const char *question, gboolean default_yes)
{
    int ret;
    GtkWidget *dialog = gtk_message_dialog_new_with_markup (parent, 0,
                                GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", question);
    gtk_window_set_title (GTK_WINDOW (dialog), title ? title : _ ("Confirm"));
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), default_yes ? GTK_RESPONSE_YES : GTK_RESPONSE_NO);
    ret = gtk_dialog_run ((GtkDialog*)dialog);
    gtk_widget_destroy (dialog);
    return ret == GTK_RESPONSE_YES;
}

gboolean fm_ok_cancel (GtkWindow *parent, const char *title, const char *question, gboolean default_ok)
{
    int ret;
    GtkWidget *dialog = gtk_message_dialog_new_with_markup (parent, 0,
                                GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL, "%s", question);
    gtk_window_set_title (GTK_WINDOW (dialog), title ? title : _ ("Confirm"));
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), default_ok ? GTK_RESPONSE_OK : GTK_RESPONSE_CANCEL);
    ret = gtk_dialog_run ((GtkDialog*)dialog);
    gtk_widget_destroy (dialog);
    return ret == GTK_RESPONSE_OK;
}

/**
  *fm_ask
  *Ask the user a question with several options provided.
  *@parent: toplevel parent widget
  *@question: the question to show to the user
  *@...: a NULL terminated list of button labels
  *Returns: the index of selected button, or -1 if the dialog is closed.
 */
int fm_ask (GtkWindow *parent, const char *title, const char *question, ...)
{
    int ret;
    va_list args;
    va_start  (args, question);
    ret = fm_ask_valist (parent, title, question, args);
    va_end  (args);
    return ret;
}

/**
  *fm_askv
  *Ask the user a question with several options provided.
  *@parent: toplevel parent widget
  *@question: the question to show to the user
  *@options: a NULL terminated list of button labels
  *Returns: the index of selected button, or -1 if the dialog is closed.
 */
int fm_askv (GtkWindow *parent, const char *title, const char *question, const char **options)
{
    int ret;
    guint id = 1;
    GtkWidget *dialog = gtk_message_dialog_new_with_markup (parent, 0,
                                GTK_MESSAGE_QUESTION, 0, "%s", question);
    gtk_window_set_title (GTK_WINDOW (dialog), title ? title : _ ("Question"));
    /*FIXME_pcm: need to handle defualt button and alternative button
      *order problems. */
    while (*options)
    {
        // FIXME_pcm: handle button image and stock buttons
        GtkWidget *btn = gtk_dialog_add_button (GTK_DIALOG (dialog), *options, id);
        ++options;
        ++id;
    }
    ret = gtk_dialog_run ((GtkDialog*)dialog);
    if (ret >= 1)
        ret -= 1;
    else
        ret = -1;
    gtk_widget_destroy (dialog);
    return ret;
}

/**
  *fm_ask_valist
  *Ask the user a question with several options provided.
  *@parent: toplevel parent widget
  *@question: the question to show to the user
  *@options: a NULL terminated list of button labels
  *Returns: the index of selected button, or -1 if the dialog is closed.
 */
int fm_ask_valist (GtkWindow *parent, const char *title, const char *question, va_list options)
{
    GArray *opts = g_array_sized_new (TRUE, TRUE, sizeof (char*), 6);
    gint ret;
    const char *opt = va_arg (options, const char*);
    while (opt)
    {
        g_array_append_val (opts, opt);
        opt = va_arg  (options, const char *);
    }
    ret = fm_askv (parent, title, question, (const char**) opts->data);
    g_array_free (opts, TRUE);
    return ret;
}



gchar *fm_get_user_input (GtkWindow *parent, const char *title, const char *msg, const char *default_text)
{
    GtkDialog *dialog = _fm_get_user_input_dialog (parent, title, msg);
    GtkWidget *entry = gtk_entry_new ();
    gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);

    if (default_text && default_text[0])
        gtk_entry_set_text (GTK_ENTRY (entry), default_text);

    return _fm_user_input_dialog_run (dialog,  GTK_ENTRY (entry));
}

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
}


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

FmPath *fm_select_folder (GtkWindow *parent, const char *title)
{
    FmPath *path;
    GtkFileChooser *chooser;
    chooser =  (GtkFileChooser*)gtk_file_chooser_dialog_new (
                                        title ? title : _ ("Please select a folder"),
                                        parent, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OK, GTK_RESPONSE_OK,
                                        NULL);
    gtk_dialog_set_alternative_button_order ((GtkDialog*)chooser,
                                        GTK_RESPONSE_CANCEL,
                                        GTK_RESPONSE_OK, NULL);
    if (gtk_dialog_run ((GtkDialog*)chooser) == GTK_RESPONSE_OK)
    {
        GFile *file = gtk_file_chooser_get_file (chooser);
        path = fm_path_new_for_gfile (file);
        g_object_unref (file);
    }
    else
        path = NULL;
    gtk_widget_destroy ((GtkWidget*)chooser);
    return path;
}

