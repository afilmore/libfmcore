/***********************************************************************************************************************
 * 
 *      fm-clipboard.c
 *
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
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
#include "fm-clipboard.h"
//~ #include "fm-utils.h"
#include "fm-file-ops.h"
#include "fm-debug.h"
#include <string.h>

enum {
    URI_LIST = 1,
    GNOME_COPIED_FILES,
    KDE_CUT_SEL,
    UTF8_STRING
};

static GtkTargetEntry targets [] =
{
    {"text/uri-list",                   0, URI_LIST},
    {"x-special/gnome-copied-files",    0, GNOME_COPIED_FILES},
    {"application/x-kde-cutselection",  0, KDE_CUT_SEL},
    {"UTF8_STRING",                     0, UTF8_STRING}
};

static gboolean _cut_files = FALSE;

static void get_data (GtkClipboard *clipboard, GtkSelectionData *selection_data, guint info, gpointer user_data)
{
    FmPathList *files = (FmPathList*) user_data;
    GString *uri_list;
    GdkAtom target = gtk_selection_data_get_target (selection_data);
    
    if (info == KDE_CUT_SEL)
    {
        // set application/kde-cutselection data
        if (_cut_files)
            gtk_selection_data_set (selection_data, target, 8, "1", 2);
        return;
    }

    uri_list = g_string_sized_new (4096);
    if (info == GNOME_COPIED_FILES)
        g_string_append (uri_list, _cut_files ? "cut\n" : "copy\n");
    if (info == UTF8_STRING)
    {
        GList *l = fm_list_peek_head_link (files);
        while (l)
        {
            FmPath *path =  (FmPath*)l->data;
            char *str = fm_path_to_str (path);
            g_string_append (uri_list, str);
            g_string_append_c (uri_list, '\n');
            g_free (str);
            l=l->next;
        }
    }
    else   // text/uri-list format
    {
        fm_path_list_write_uri_list (files, uri_list);
    }

    gtk_selection_data_set (selection_data, target, 8, uri_list->str, uri_list->len + 1);

    g_string_free (uri_list, TRUE);
}

static void clear_data (GtkClipboard *clipboard, gpointer user_data)
{
    FmPathList *files =  (FmPathList*)user_data;
    fm_list_unref (files);
    _cut_files = FALSE;
}

gboolean fm_clipboard_cut_or_copy_files (GtkWidget *src_widget, FmPathList *files, gboolean cut_files)
{
    GdkDisplay *dpy = src_widget ? gtk_widget_get_display (src_widget) : gdk_display_get_default ();
    GtkClipboard *clipboard = gtk_clipboard_get_for_display (dpy, GDK_SELECTION_CLIPBOARD);
    gboolean ret = gtk_clipboard_set_with_data (clipboard, targets, G_N_ELEMENTS (targets),
                                               get_data, clear_data, fm_list_ref (files));
    _cut_files = cut_files;
    return ret;
}

gboolean check_kde_curselection (GtkClipboard *clipboard)
{
    /*Check application/x-kde-cutselection:
      *If the content of this format is string "1", that means the
      *file is cut in KDE  (Dolphin). */
    
    gboolean ret = FALSE;
    GdkAtom atom = gdk_atom_intern_static_string (targets[KDE_CUT_SEL-1].target);
    
    GtkSelectionData *selection_data = gtk_clipboard_wait_for_contents (clipboard, atom);
    const guchar *data = gtk_selection_data_get_data (selection_data);
    gint data_length = gtk_selection_data_get_length (selection_data);
    gint data_format = gtk_selection_data_get_format (selection_data);
    
    if (selection_data)
    {
        if (data_length > 0 && data_format == 8 && data[0] == '1')
            ret = TRUE;
        
        gtk_selection_data_free (selection_data);
    }
    return ret;
}

gboolean fm_clipboard_paste_files (GtkWidget *dest_widget, FmPath *dest_dir)
{
    GdkDisplay *dpy = dest_widget ? gtk_widget_get_display (dest_widget) : gdk_display_get_default ();
    GtkClipboard *clipboard = gtk_clipboard_get_for_display (dpy, GDK_SELECTION_CLIPBOARD);
    FmPathList *files;
    char **uris, **uri;
    GdkAtom atom;
    int type = 0;
    GdkAtom *avail_targets;
    int n, i;

    // get all available targets currently in the clipboard.
    if (!gtk_clipboard_wait_for_targets (clipboard, &avail_targets, &n))
        return FALSE;

    // check gnome and xfce compatible format first
    atom = gdk_atom_intern_static_string (targets[GNOME_COPIED_FILES-1].target);
    for (i = 0; i < n; ++i)
    {
        if (avail_targets[i] == atom)
        {
            type = GNOME_COPIED_FILES;
            break;
        }
    }
    if (0 == type) // x-special/gnome-copied-files is not found.
    {
        // check uri-list
        atom = gdk_atom_intern_static_string (targets[URI_LIST-1].target);
        for (i = 0; i < n; ++i)
        {
            if (avail_targets[i] == atom)
            {
                type = URI_LIST;
                break;
            }
        }
        if (0 == type) // text/uri-list is not found.
        {
            // finally, fallback to UTF-8 string
            atom = gdk_atom_intern_static_string (targets[UTF8_STRING-1].target);
            for (i = 0; i < n; ++i)
            {
                if (avail_targets[i] == atom)
                {
                    type = UTF8_STRING;
                    break;
                }
            }
        }
    }
    g_free (avail_targets);

    if (type)
    {
        
        GtkSelectionData *selection_data = gtk_clipboard_wait_for_contents (clipboard, atom);
        const guchar *data = gtk_selection_data_get_data (selection_data);
        gint data_length = gtk_selection_data_get_length (selection_data);
        gint data_format = gtk_selection_data_get_format (selection_data);
        
        char *pdata = (char*) data;
        pdata [data_length] = '\0'; //make sure the data is null-terminated.
        
        _cut_files = FALSE;

        switch (type)
        {
            case GNOME_COPIED_FILES:
                _cut_files = g_str_has_prefix (pdata, "cut\n");
                
                while (*pdata && *pdata != '\n')
                    ++pdata;
                
                ++pdata;
                //the following parts is actually a uri-list, so don't break here.
            
            case URI_LIST:
                uris = g_uri_list_extract_uris (pdata);
                if (type != GNOME_COPIED_FILES)
                {
                    /*if we're not handling x-special/gnome-copied-files, check
                      *if information from KDE is available. */
                    _cut_files = check_kde_curselection (clipboard);
                }
            break;
            
            case UTF8_STRING:
                //FIXME_pcm: how should we treat UTF-8 strings? URIs or filenames?
                uris = g_uri_list_extract_uris (pdata);
            break;
        }
        
        gtk_selection_data_free (selection_data);

        if (uris)
        {
            GtkWindow *parent;
            if (dest_widget)
                parent = (GtkWindow*) gtk_widget_get_toplevel (GTK_WIDGET (dest_widget));
            else
                parent = NULL;

            
            // we'll have a list of uri as "file:///home/hotnuma/Bureau/TODO"
            #ifdef DEBUG
            const char **uri_list;
            for (uri_list = (const char **) uris; *uri_list; ++uri_list)
            {
                const char *puri = *uri_list;
                if (puri[0] != '\0') // ensure that it's not an empty string
                {
                    if (puri[0] == '/')
                        DEBUG ("fm_clipboard_paste_files: path = %s\n", puri);
                    else if (strstr (puri, "://"))
                        DEBUG ("fm_clipboard_paste_files: uri = %s\n", puri);
                    else // it's not a valid path or URI
                        DEBUG ("fm_clipboard_paste_files: invalid uri = %s\n", puri);
                }
            }
            #endif
            
            files = fm_path_list_new_from_uris ((const char **) uris);
            g_strfreev (uris);

            if (!fm_list_is_empty (files))
            {
                if (_cut_files)
                    fm_move_files (parent, files, dest_dir);
                else
                    fm_copy_files (parent, files, dest_dir);
            }
            
            fm_list_unref (files);
            return TRUE;
        }
    }
    
    return FALSE;
}



