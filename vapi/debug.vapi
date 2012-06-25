/***********************************************************************************************************************
 * 
 *      debug.vapi
 * 
 *      Copyright 2012 Axel FILMORE <axel.filmore@gmail.com>
 * 
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License Version 2.
 *      http://www.gnu.org/licenses/gpl-2.0.txt
 * 
 *      Purpose: Binding file for libfmcore.
 * 
 *      Version: 1.0
 * 
 * 
 **********************************************************************************************************************/
//~ namespace Fm {
//~     
//~     
    /*************************************************************************************
     *  
     * 
     * 
     ************************************************************************************/
	[CCode (cheader_filename = "fm-debug.h")]
    void DEBUG (...);

	[CCode (cheader_filename = "fm-debug.h")]
    void NO_DEBUG (...);

	[CCode (cheader_filename = "fm-debug.h")]
    void TREEVIEW_DEBUG (...);

//~ }


