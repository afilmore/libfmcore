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

        // Icons Sizes...
        public uint small_icon_size         = 16;       // size of small icons
        public uint big_icon_size           = 36;       // size of big icons
        public uint pane_icon_size          = 16;       // size of side pane icons

        // Trash Can Settings...
        public bool use_trash               = true;     // delete file to trash can
        public bool confirm_del             = true;     // ask before deleting files
        
        // Show Internal Volumes...
        public bool show_internal_volumes   = false;    // show system internal volumes in side pane. (udisks-only)

        // Thumbnails...
        public bool show_thumbnail          = false;    // show thumbnails
        public uint thumbnail_size          = 128;      // size of thumbnail icons
        public uint thumbnail_max           = 2048;     // show thumbnails for files smaller than 'thumb_max' KB
        public bool thumbnail_local         = true;     // show thumbnails for local files only

        // Single Click...
        public bool single_click            = false;    // single click to open file

        // Default Applications...
        public string terminal;
        public string filemanager;
        public string panel;
        public string run;
        public string taskmanager;
        public string archiver;
        
        // SI Prefix...
        public bool si_unit;                            // use SI prefix for file sizes

        
        construct {
            
            Settings settings;
            
            settings = new Settings ("desktop.noname.applications.terminal");
            this.terminal = settings.get_string ("default");
            
            settings = new Settings ("desktop.noname.applications.filemanager");
            this.filemanager = settings.get_string ("default");
            
            settings = new Settings ("desktop.noname.applications.panel");
            this.panel = settings.get_string ("default");
            
            settings = new Settings ("desktop.noname.applications.run");
            this.run = settings.get_string ("default");
            
            settings = new Settings ("desktop.noname.applications.taskmanager");
            this.taskmanager = settings.get_string ("default");
            
            settings = new Settings ("desktop.noname.applications.archiver");
            this.archiver = settings.get_string ("default");
            
        }
    }
}

