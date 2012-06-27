/***********************************************************************************************************************
 * 
 *      fm-debug.h
 *      
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
#ifndef __FM_DEBUG_H__
#define __FM_DEBUG_H__

#include <stdio.h>

G_BEGIN_DECLS

#define NO_DEBUG(...)

#ifdef ENABLE_DEBUG
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#ifdef ENABLE_JOB_DEBUG
#define JOB_DEBUG(...) DEBUG(__VA_ARGS__)
#else
#define JOB_DEBUG(...)
#endif

#ifdef ENABLE_TREEVIEW_DEBUG
#define TREEVIEW_DEBUG(...) DEBUG(__VA_ARGS__)
#else
#define TREEVIEW_DEBUG(...)
#endif

G_END_DECLS
#endif


