/***********************************************************************************************************************
 * fm-config.vala
 * 
 * Copyright 2012 Axel FILMORE <axel.filmore@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 2.
 * http://www.gnu.org/licenses/gpl-2.0.txt
 * 
 * This software is an experimental rewrite of PcManFm originally written by Hong Jen Yee aka PCMan for LXDE project.
 * 
 * Purpose: 
 * 
 * 
 * 
 **********************************************************************************************************************/

namespace Fm {
    
    public Fm.Config config;
    
    public class Config : Object {

        public bool single_click    = false;            // single click to open file
        public bool use_trash       = true;             // delete file to trash can
        public bool confirm_del     = true;             // ask before deleting files

        public uint big_icon_size   = 36;               // size of big icons
        public uint small_icon_size = 16;               // size of small icons
        public uint pane_icon_size  = 16;               // size of side pane icons
        public uint thumbnail_size  = 128;              // size of thumbnail icons

        public bool show_thumbnail  = false;            // show thumbnails
        public bool thumbnail_local = true;             // show thumbnails for local files only
        public uint thumbnail_max   = 2048;             // show thumbnails for files smaller than 'thumb_max' KB

        public bool show_internal_volumes   = false;    // show system internal volumes in side pane. (udisks-only)

        public string terminal;                         // command line to launch terminal emulator
        public bool si_unit;                            // use SI prefix for file sizes

        public string archiver;                         // desktop_id of the archiver used
        
        construct {
            
            Settings settings = new Settings ("desktop.noname.applications.archiver");

            // Getting keys
            this.archiver = settings.get_string ("default");
            //print ("archiver = %s\n", this.archiver);
        }
    }
}

