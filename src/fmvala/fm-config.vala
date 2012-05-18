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

        // Trash Can Settings...
        public bool use_trash_can           = true;     // delete file to trash can
        public bool confirm_delete          = true;     // ask before deleting files
        
        // Thumbnails...
        public bool show_thumbnail          = false;    // show thumbnails
        public uint thumbnail_size          = 128;      // size of thumbnail icons
        public uint thumbnail_max           = 2048;     // show thumbnails for files smaller than 'thumb_max' KB
        public bool thumbnail_local         = true;     // show thumbnails for local files only

        // Default Applications...
        public string archiver;
        public string terminal;
        
        public string panel;
        public string run;
        public string taskmanager;
        
        // SI Prefix...
        public bool si_unit;                            // use SI prefix for file sizes

        construct {
            
            Settings settings;
            
            settings = new Settings ("desktop.noname.applications.terminal");
            this.terminal = settings.get_string ("default");
            
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


