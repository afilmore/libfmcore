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
    // FIXME_pcm: handle virtual files without mime-types
    
    if (fm_list_is_empty (list))
        return TRUE;
        
    GList *l = fm_list_peek_head_link (list);
    
    FmFileInfo *file_info = (FmFileInfo*) l->data;
    l = l->next;
    
    for ( ; l; l=l->next)
    {
        FmFileInfo *next_file_info = (FmFileInfo*) l->data;
        
        if (fm_file_info_get_mime_type (file_info, FALSE)
            != fm_file_info_get_mime_type (next_file_info, FALSE))
            return FALSE;
    }
    
    return TRUE;
}

gboolean fm_file_info_list_is_same_fs (FmFileInfoList *list)
{
    if (fm_list_is_empty (list))
        return TRUE;
        
    GList *l = fm_list_peek_head_link (list);
    FmFileInfo *file_info = (FmFileInfo*) l->data;
    gboolean is_native = fm_path_is_native (fm_file_info_get_path (file_info));
    
    l = l->next;
    
    for ( ; l; l = l->next)
    {
        FmFileInfo *next_file_info = (FmFileInfo*) l->data;
        
        // TODO_axl: a fm_file_info_same_fs (file_info1, file_info2);
        
        if (is_native != fm_path_is_native (fm_file_info_get_path (next_file_info)))
            return FALSE;
        
        if (is_native)
        {
            if (file_info->dev != next_file_info->dev)
                return FALSE;
        }
        else if (file_info->fs_id != next_file_info->fs_id)
        {
                return FALSE;
        }
    }
    
    return TRUE;
}


int fm_file_info_list_get_flags (FmFileInfoList *list, uint *have_flags, uint *all_flags)
{
    uint _have_flags = FM_PATH_NONE;
    uint _all_flags = ~FM_PATH_NONE;
    
    int count = 0;
    
    if (fm_list_is_empty (list))
        return count;
    
    GList *l;
    for (l = fm_list_peek_head_link (list); l; l = l->next)
    {
        FmFileInfo *file_info = (FmFileInfo*) l->data;
        FmPath *path = fm_file_info_get_path (file_info);
        
        uint flags = fm_path_get_flags (path);
        
        _have_flags |= flags;
        _all_flags &= flags;
        
        count++;
    }
    
    if (have_flags)
        *have_flags = _have_flags;
    
    if (all_flags)
        *all_flags = _all_flags;
    
    return count;
}



