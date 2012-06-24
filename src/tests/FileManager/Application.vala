/***********************************************************************************************************************
 *      
 *      Application.vala
 * 
 *      Copyright 2012 Axel FILMORE <axel.filmore@gmail.com>
 * 
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License Version 2.
 *      http://www.gnu.org/licenses/gpl-2.0.txt
 * 
 * 
 *      Purpose: A simple file manager demo in Vala.
 * 
 * 
 * 
 **********************************************************************************************************************/
namespace Demo {

    public class Application {
        

        public Application () {
            
        }
        
        public bool run_local () {
        
            // Create the Desktop configuration and initialize LibFmCore.
            Fm.init (null);
            
            Fm.mount_automount ();
            
            Manager.Window manager_window = new Manager.Window ();
            manager_window.create ();
            
            Gtk.main ();
            
            Fm.finalize ();

            return true;
        }
        
        
        /*********************************************************************************
         * Application's entry point.
         *
         * The program trie to run as the first instance in run_local (), if it's not
         * the first instance it calls GApplication.run () and sends arguments to the
         * first instance via DBus.
         * 
         ********************************************************************************/
        private static int main (string[] args) {
            
            
            Demo.Application application = new Demo.Application ();
            
            Gtk.init (ref args);
            
            application.run_local ();
            
            return 0;
        }
    }
}


