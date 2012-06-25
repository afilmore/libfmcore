/***********************************************************************************************************************
 * 
 *      libfmcore-jobs.vapi
 * 
 *      Copyright 2012 Axel FILMORE <axel.filmore@gmail.com>
 * 
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License Version 2.
 *      http://www.gnu.org/licenses/gpl-2.0.txt
 * 
 *      Purpose: Binding file for libfmcore.
 * 
 *      Version: 0.3
 * 
 * 
 **********************************************************************************************************************/
namespace Fm {
    
    

    [CCode (cheader_filename = "fm-jobs.h")]
    public enum Severity {
        NONE,
        WARNING,
        MILD,
        MODERATE,
        SEVERE,
        CRITICAL
    }

    [CCode (cheader_filename = "fm-jobs.h")]
    public enum ErrorAction {
        CONTINUE,
        RETRY,
        ABORT
    }

    [CCode (cheader_filename = "fm-jobs.h")]
    public abstract class Job : GLib.Object {

        public Job();

        public bool run_sync();

        public bool run_sync_with_mainloop();

        public virtual void run_async();

        protected abstract bool run();

        private bool job_func(GLib.IOSchedulerJob job, GLib.Cancellable? cancellable);

        public void cancel();

        public bool is_cancelled();

        public bool is_running();

        protected unowned GLib.Cancellable get_cancellable();

        public Fm.ErrorAction emit_error(GLib.Error err, Fm.Severity severity);

        public virtual signal void finished();

        public signal Fm.ErrorAction error(GLib.Error err, Fm.Severity severity);

    }


}


