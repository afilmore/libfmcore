/***********************************************************************************************************************
 * 
 *      fm-file-info-list.c
 *
 *      Copyright 2009 - 2010 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "fm-file-info-list.h"


static FmListFuncs fm_list_funcs = {fm_file_info_ref, fm_file_info_unref};

FmFileInfoList *fm_file_info_list_new ()
{
    return fm_list_new (&fm_list_funcs);
}

gboolean fm_list_is_file_info_list (FmList *list)
{
    return list->funcs == &fm_list_funcs;
}

gboolean fm_file_info_list_is_same_type (FmFileInfoList *list)
{
    // FIXME: handle virtual files without mime-types
    if (! fm_list_is_empty (list))
    {
        GList *l = fm_list_peek_head_link (list);
        FmFileInfo *fi = (FmFileInfo*)l->data;
        l = l->next;
        for (;l;l=l->next)
        {
            FmFileInfo *fi2 = (FmFileInfo*)l->data;
            if (fi->type != fi2->type)
                return FALSE;
        }
    }
    return TRUE;
}

gboolean fm_file_info_list_is_same_fs (FmFileInfoList *list)
{
    if (! fm_list_is_empty (list))
    {
        GList *l = fm_list_peek_head_link (list);
        FmFileInfo *fi = (FmFileInfo*)l->data;
        l = l->next;
        for (;l;l=l->next)
        {
            FmFileInfo *fi2 = (FmFileInfo*)l->data;
            gboolean is_native = fm_path_is_native (fi->path);
            if (is_native != fm_path_is_native (fi2->path))
                return FALSE;
            if (is_native)
            {
                if (fi->dev != fi2->dev)
                    return FALSE;
            }
            else
            {
                if (fi->fs_id != fi2->fs_id)
                    return FALSE;
            }
        }
    }
    return TRUE;
}

uint fm_file_info_list_get_flags (FmFileInfoList *list)
{
    uint flags = FM_PATH_NONE;
    
    if (fm_list_is_empty (list))
        return flags;
    
    GList *l;
    for (l = fm_list_peek_head_link (list); l; l=l->next)
    {
        FmFileInfo *fi = (FmFileInfo*)l->data;
        
        //printf ("fm_file_info_list_get_flags: path name = %s\n", fi->path->name);
        
        if (fm_path_is_trash_root (fi->path))
        {
            flags |= FM_PATH_IS_TRASH_CAN;
            flags |= FM_PATH_IS_VIRTUAL;
        }
        else if (fm_path_is_virtual (fi->path))
        {
            flags |= FM_PATH_IS_VIRTUAL;
        }
    }
    return flags;
}



