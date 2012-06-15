/***********************************************************************************************************************
 * 
 *      fm-msgbox.c
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

#include <glib/gi18n-lib.h>

#include "fm-msgbox.h"


// Forward declarations...
int fm_ask_valist (GtkWindow *parent, const char *title, const char *question, va_list options);
int fm_askv (GtkWindow *parent, const char *title, const char *question, const char **options);


gboolean fm_yes_no (GtkWindow *parent, const char *title, const char *question, gboolean default_yes)
{
    int ret;
    GtkWidget *dialog = gtk_message_dialog_new_with_markup (parent,
                                                            0,
                                                            GTK_MESSAGE_QUESTION,
                                                            GTK_BUTTONS_YES_NO,
                                                            "%s", question);

    gtk_window_set_title (GTK_WINDOW (dialog), title ? title : _("Confirm"));
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), default_yes ? GTK_RESPONSE_YES : GTK_RESPONSE_NO);
    ret = gtk_dialog_run ((GtkDialog*) dialog);
    
    gtk_widget_destroy (dialog);
    
    return ret == GTK_RESPONSE_YES;
}

void fm_show_error (GtkWindow *parent, const char *title, const char *msg)
{
    GtkWidget *dialog = gtk_message_dialog_new (parent, 0,
                                            GTK_MESSAGE_ERROR,
                                            GTK_BUTTONS_OK, "%s", msg);
    gtk_window_set_title ((GtkWindow*)dialog, title ? title : _("Error"));
    gtk_dialog_run ((GtkDialog*)dialog);
    gtk_widget_destroy (dialog);
}

/** currently unused...
gboolean fm_ok_cancel (GtkWindow *parent, const char *title, const char *question, gboolean default_ok)
{
    int ret;
    GtkWidget *dialog = gtk_message_dialog_new_with_markup (parent, 0,
                                GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL, "%s", question);
    gtk_window_set_title (GTK_WINDOW (dialog), title ? title : _("Confirm"));
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), default_ok ? GTK_RESPONSE_OK : GTK_RESPONSE_CANCEL);
    ret = gtk_dialog_run ((GtkDialog*)dialog);
    gtk_widget_destroy (dialog);
    return ret == GTK_RESPONSE_OK;
}**/


int fm_askv (GtkWindow *parent, const char *title, const char *question, const char **options)
{
    int ret;
    guint id = 1;
    GtkWidget *dialog = gtk_message_dialog_new_with_markup (parent, 0,
                                GTK_MESSAGE_QUESTION, 0, "%s", question);
    gtk_window_set_title (GTK_WINDOW (dialog), title ? title : _("Question"));
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

int fm_ask (GtkWindow *parent, const char *title, const char *question, ...)
{
    int ret;
    va_list args;
    va_start  (args, question);
    ret = fm_ask_valist (parent, title, question, args);
    va_end  (args);
    return ret;
}



