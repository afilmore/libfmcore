//      job.vala
//      
//      Copyright 2011 Hong Jen Yee  (PCMan) <pcman.tw@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//       (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.
//      
//      

namespace Fm {

    // defines severity of errors
    public enum Severity {
        NONE,
        WARNING, // not an error, just a warning
        MILD, // no big deal, can be ignored most of the time
        MODERATE, // moderate errors
        SEVERE, // severe errors, whether to abort operation depends on error handlers
        CRITICAL // critical errors, the operation is aborted
    }

    // actions to take once errors happen
    public enum ErrorAction {
        CONTINUE, // ignore the error and continue remaining work
        RETRY, // retry the previously failed operation.  (not every kind of job support this) */
        ABORT // abort the whole job
    }

    public abstract class Job : Object {

        protected unowned IOSchedulerJob? job;
        protected Cancellable cancellable;
        protected bool running;
        private ulong cancelled_handler;

        public Job () {
            this.cancellable = new Cancellable ();
        }

        ~Job () {
            cancellable.disconnect (cancelled_handler);
        }

        // Run a job in current thread in a blocking fashion.
        public bool run_sync () {
            running = true;
            var ret = run ();
            running = false;
            finished ();
            return ret;
        }

        // Run a job in current thread in a blocking fashion and an additional 
        // mainloop being created to prevent blocking of user interface.
        // A job running synchronously with this function should be unrefed
        // later with g_object_unref when no longer needed.
        public bool run_sync_with_mainloop () {
            bool ret = true;
            var mainloop = new MainLoop ();
            run_async ();
            ulong handler_id = finished.connect ( () => {
                mainloop.quit ();
            });
            mainloop.run ();
            disconnect (handler_id);
            return ret;
        }

        // Run a job asynchronously in another working thread, and 
        // emit 'finished' signal in the main thread after its termination.
        // The default implementation of FmJob::run_async () create a working
        // thread in thread pool, and calls FmJob::run () in it.
        public virtual void run_async () {
            
            stdout.printf  ("Fm.Job.run_async\n");
            
            running = true;
            //g_io_scheduler_push_job (job_func, Priority.DEFAULT, cancellable);
            IOSchedulerJob.push  (job_func, Priority.DEFAULT, cancellable);
        }

        // call from the "worker thread" to do the real work
        // should be overriden by derived classes
        protected abstract bool run ();

        private bool job_func (IOSchedulerJob job, Cancellable? cancellable) {
            stdout.printf  ("Fm.Job.job_func\n");
            this.job = job;
            run ();
            this.job = null;
            running = false;
            job.send_to_mainloop ( () => {
                finished ();
                return true;
            });
            return false;
        }

        // Cancel the running job. can be called from any thread.
        public void cancel () {
            cancellable.cancel ();
        }

        // return true if the job is already cancelled
        public bool is_cancelled () {
            return cancellable.is_cancelled ();
        }

        // return true if the job is still running
        public bool is_running () {
            return running;
        }

        protected unowned Cancellable get_cancellable () {
            return cancellable;
        }

        // Emit an 'error' signal to notify the main thread when an error occurs.
        // The return value of this function is the return value returned by
        // the connected signal handlers.
        // If severity is Severity.CRITICAL, the returned value is ignored and
        // cancel () is called to abort the job. Otherwise, the signal
        // handler of this error can return ErrorAction.RETRY to ask for retrying the
        // failed operation, return ErrorAction.CONTINUE to ignore the error and
        // continue the remaining job, or return ErrorAction.ABORT to abort the job.
        // If ErrorAction.ABORT is returned by the signal handler, cancel ()
        // will be called in emit_error ().
        // This should only be called from worker thread
        public ErrorAction emit_error (Error err, Severity severity) {
            ErrorAction ret = ErrorAction.CONTINUE;
            job.send_to_mainloop ( () => {
                ret = error (err, severity);
                return true;
            });
            return ret;
        }

        // The "finished" signal is emitted when the job is cancelled as well.
        // Just check if the job is cancelled with is_cancelled ().
        public virtual signal void finished () {
        }

        public signal ErrorAction error (Error err, Severity severity);

    }

}
