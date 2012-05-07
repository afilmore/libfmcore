/***********************************************************************************************************************
 * 
 *      fm-mime-type.h
 *      
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
 *      Copyright 2012 Axel FILMORE <axel.filmore@gmail.com>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
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
#ifndef _FM_MIME_TYPE_H_
#define _FM_MIME_TYPE_H_

#include "fm-icon.h"

#include <glib.h>
#include <glib/gstdio.h>

G_BEGIN_DECLS

// Mime Type Initialisation...
void fm_mime_type_init ();
void fm_mime_type_finalize ();

typedef struct _FmMimeType FmMimeType;

struct _FmMimeType
{
    char    *type;         // Mime type name
    char    *description;  // Description of the mime type
	
    FmIcon  *icon;

    int     n_ref;
};

FmMimeType *fm_mime_type_new                    (const char *type_name);

// file name used in this API should be encoded in UTF-8.
FmMimeType *fm_mime_type_get_for_file_name      (const char *ufile_name);
FmMimeType *fm_mime_type_get_for_type           (const char *type);

// file_path should be on-disk encoding, base_name should be UTF-8, pstat can be null.
FmMimeType *fm_mime_type_get_for_native_file    (const char *file_path, const char *base_name, struct stat *pstat);

FmMimeType *fm_mime_type_ref                    (FmMimeType *mime_type);
void fm_mime_type_unref                         (gpointer mime_type_);

// Get The GIcon...
FmIcon *fm_mime_type_get_icon                   (FmMimeType *mime_type);

// Get mime-type string...
const char *fm_mime_type_get_type               (FmMimeType *mime_type);

// Get human-readable description of mime-type
const char *fm_mime_type_get_desc               (FmMimeType *mime_type);

G_END_DECLS
#endif


