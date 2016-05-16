/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaseApi.cpp
 *   Date     : May 8, 2016
 *------------------------------------------------------------------------------
 */


/*------------------------------------------------------------------------------
 *  Includes
 */

#include <stdarg.h>
# include <signal.h>
// Portability headers
#if __linux

#elif __WIN32

#endif

#include "BaseApi.h"
#include "CBaLog.h"
#include "BaGenMacros.h"

/*------------------------------------------------------------------------------
 *  Defines
 */


/*------------------------------------------------------------------------------
 *  Local functions
 */
LOCAL void ctrlTaskRout(TBaCoreThreadArg* pArg);
LOCAL void signalHdlr(int sig);

/*------------------------------------------------------------------------------
 *  Static variables
 */
static CBaLog *spLog = 0;
// Handler in progress flag
static volatile sig_atomic_t sHandlerInProgress = 0;

/*------------------------------------------------------------------------------
 *  C Interface
 */

//
TBaBoolRC BaApiInit() {


   return eBaBoolRC_Success;
}


//
TBaBoolRC BaApiExit() {

   return eBaBoolRC_Success;
}

//
TBaBoolRC BaApiInitLoggerDef(const char* name) {
   if (spLog) {
      return eBaBoolRC_Success;
   } else if (!name) {
      return eBaBoolRC_Error;
   }

   spLog = CBaLog::Create(name);
   return spLog ? eBaBoolRC_Success : eBaBoolRC_Error;
}

// todo: multi-thread?
TBaBoolRC BaApiInitLogger(TBaLogHdl hdl) {
   if (spLog) {
      return eBaBoolRC_Success;
   } else if (!hdl) {
      return eBaBoolRC_Error;
   }
   spLog = dynamic_cast<CBaLog*>((IBaLog*)hdl);
   return spLog ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaApiExitLogger() {
   if (!spLog) {
      return eBaBoolRC_Success;
   }

   bool ret = CBaLog::Destroy(spLog);
   spLog = 0;
   return ret ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaApiLogF(EBaLogPrio prio, const char* tag, const char* fmt, ...) {
   if (!spLog) {
      return eBaBoolRC_Error;
   }

   va_list arg;
   va_start(arg, fmt);
   TBaBoolRC ret = spLog->LogV(prio, tag, fmt, arg);
   va_end(arg);
   return ret;
}

//
void BaApiStartCtrlTask(TBaApiCtrlTaskOpts* pOpts) {
   if (!pOpts) {
      return;
   }

   if (!pOpts->init(0) || !pOpts->start(0)) {
      return;
   }

   TBaCoreThreadHdl hdl = BaCoreCreateThread(pOpts->name, ctrlTaskRout, 0, pOpts->prio);
}

//
int BaApiDaemonize() {
   int rc = 0;

//   SIGRTMIN;

   // Termination signals from user
//   sigaction(SIGTERM,0,NULL);

   rc  = (int)signal(SIGTERM, signalHdlr); // Polite termination signal
   rc |= (int)signal(SIGINT,  signalHdlr); // Interrupt. 'Ctrl-C'
//   rc |= (int)signal(SIGQUIT, signalHdlr); // Quit. Produces core dump, 'Ctrl-\'
//   rc |= (int)signal(SIGHUP,  signalHdlr); // Hang up. Controlling process terminated

   // Program Error Signals. Produce core dumps
   rc |= (int)signal(SIGFPE,  signalHdlr); // Arithmetic error
   rc |= (int)signal(SIGILL,  signalHdlr); // Illegal instruction
   rc |= (int)signal(SIGSEGV, signalHdlr); // Segmentation violation
//   rc |= (int)signal(SIGBUS,  signalHdlr); // Bus error. Invalid pointer
   rc |= (int)signal(SIGABRT, signalHdlr); // Abort. Internal error detected
//   rc |= (int)signal(SIGSYS,  signalHdlr); // Bad system call

   // Operation Error Signals. Cause termination
//   rc |= (int)signal(SIGPIPE, signalHdlr); // Broken pipe
//   rc |= (int)signal(SIGXCPU, signalHdlr); // CPU time limit exceeded. Soft limit
//   rc |= (int)signal(SIGXFSZ, signalHdlr); // File size limit exceeded. Soft limit

   // SIG_ERR = -1
//   sInit = (rc != (int)SIG_ERR) ? 1 : 0;
   return rc;
}

//
// Signal handler to free/reset resources
LOCAL void signalHdlr(int sig) {

   // Queue the signal if the handler is busy
   if (sHandlerInProgress) {
      raise(sig);
   }
   sHandlerInProgress = 1;


   // Print the signal to console
//   psignal(sig, "KILLED, signal");

   // Reset the signal handler to default action. Core dumps will be written
   signal(sig, SIG_DFL);
   raise(sig);
}

//
LOCAL void ctrlTaskRout(TBaCoreThreadArg* pArg) {
   if (!pArg || !pArg->pArg) {
      return;
   }

   TBaApiCtrlTaskOpts* pOpts = (TBaApiCtrlTaskOpts*) pArg->pArg;
   void (* update  )(void*) = pOpts->update;
   while (!pArg->exitTh) {
      uint64_t duration = BaCoreTimedUs(update, 0);
      if (duration + 9 > pOpts->cyleTimeMs) {
         BaCoreMSleep(10);
      } else {
         BaCoreMSleep(pOpts->cyleTimeMs - duration);
      }
   }
}
