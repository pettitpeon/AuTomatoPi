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
#include <stdlib.h>
#include <signal.h>
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
#define CTRLTASK "BaseApiCtrlTask"
#define DEFDIR "/"
#define MINSAMPTIME_US 10000

/*------------------------------------------------------------------------------
 *  Local functions
 */
LOCAL void ctrlTaskRout(TBaCoreThreadArg* pArg);
LOCAL void signalHdlr(int sig);
LOCAL TBaBoolRC registerSignals();
LOCAL TBaBoolRC unregisterSignals();

/*------------------------------------------------------------------------------
 *  Static variables
 */
static CBaLog *spLog = 0;
static uint64_t sUpdCnt = 0;
static uint64_t sActDurUs = 0;

// Handler in progress flag
static volatile sig_atomic_t sHandlerInProgress = 0;
static volatile sig_atomic_t sExit = 0;


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
TBaBoolRC BaApiStartCtrlTask(TBaApiCtrlTaskOpts* pOpts) {
#ifdef __WIN32
   return eBaBoolRC_Error;
#else
   if (!pOpts) {
      return eBaBoolRC_Error;
   }

   if (!pOpts->init(pOpts->initArg)) {
      return eBaBoolRC_Error;
   }
//   TBaCoreThreadHdl hdl = 0; //BaCoreCreateThread(pOpts->name, ctrlTaskRout, 0, pOpts->prio);

   if (!BaCoreTestPidFile(CTRLTASK)) {
      printf("Process already running");
   }

   if (!registerSignals()) {
      return eBaBoolRC_Error;
   }

   // Lets replicate
   pid_t pid = fork();

   // An error occurred, return
   if (pid < 0) {
      unregisterSignals();
      printf("Parent talking: Error\n");
      return eBaBoolRC_Success;
   }

   // Success: Let the parent return
   if (pid > 0) {
      unregisterSignals();
      printf("Luke, I am you father\n");
      return eBaBoolRC_Success;
   }

   // Now Luke is on command ////////////////////////////////////////

   // Write PID file
   BaCoreWritePidFile(CTRLTASK);

   // Change directory to default
   chdir(DEFDIR);

   printf("prio: %i\n", BaCoreSetOwnProcPrio(eBaCorePrio_RT_Normal));
   printf("NOOOO!!\n");


   uint64_t sampTimeUs = MAX(pOpts->cyleTimeMs, 10) * 1000;
   void (*updFun)(void*) = pOpts->update;
   void *pArg = pOpts->updateArg;

   // This is the actual control loop ////////////////////////////////////
   for ( ; !sExit; sUpdCnt++) {
      sActDurUs = BaCoreTimedUs(updFun, pArg);
      if (sActDurUs + MINSAMPTIME_US > sampTimeUs) {
         BaCoreUSleep(MINSAMPTIME_US);
      } else {
         BaCoreUSleep(sampTimeUs - sActDurUs);
      }
   }
   // ////////////////////////////////////////////////////////////////////

   pOpts->exit(pOpts->exitArg);
   puts("Luke: I finished your quest father\n");
   BaCoreRemovePidFile(CTRLTASK);

   // This is the child process, should not continue
   exit(EXIT_SUCCESS);

#endif
}


//
TBaBoolRC BaApiStopCtrlTask() {
   int rc = kill(BaCoreReadPidFile(CTRLTASK), SIGRTMIN);
   return rc == 0 ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
LOCAL TBaBoolRC registerSignals() {
   int rc = 0;
   struct sigaction act = {0};
   act.sa_handler = signalHdlr;

   // Termination signals from user
   rc  = sigaction(SIGTERM,  &act, 0); // Polite termination signal
   rc |= sigaction(SIGINT,   &act, 0); // Interrupt. 'Ctrl-C'
   rc |= sigaction(SIGQUIT,  &act, 0); // Quit. Produces core dump, 'Ctrl-\'
   rc |= sigaction(SIGUSR1,  &act, 0); // User signal 1
   rc |= sigaction(SIGUSR2,  &act, 0); // User signal 2
   rc |= sigaction(SIGRTMIN, &act, 0); // User real-time signal


//   rc |= (int)signal(SIGHUP,  signalHdlr); // Hang up. Controlling process terminated

   // Program Error Signals. Produce core dumps
//   rc |= (int)signal(SIGFPE,  signalHdlr); // Arithmetic error
//   rc |= (int)signal(SIGILL,  signalHdlr); // Illegal instruction
//   rc |= (int)signal(SIGSEGV, signalHdlr); // Segmentation violation
//   rc |= (int)signal(SIGBUS,  signalHdlr); // Bus error. Invalid pointer
//   rc |= (int)signal(SIGABRT, signalHdlr); // Abort. Internal error detected
//   rc |= (int)signal(SIGSYS,  signalHdlr); // Bad system call

   // Operation Error Signals. Cause termination
//   rc |= (int)signal(SIGPIPE, signalHdlr); // Broken pipe
//   rc |= (int)signal(SIGXCPU, signalHdlr); // CPU time limit exceeded. Soft limit
//   rc |= (int)signal(SIGXFSZ, signalHdlr); // File size limit exceeded. Soft limit

   // SIG_ERR = -1
//   sInit = (rc != (int)SIG_ERR) ? 1 : 0;
   return rc == 0 ? eBaBoolRC_Success : eBaBoolRC_Error;
}
//
LOCAL TBaBoolRC unregisterSignals() {
   int rc = 0;
   struct sigaction act = {0};
   act.sa_handler = SIG_DFL;
   rc  = sigaction(SIGTERM , &act, 0); // Polite termination signal
   rc |= sigaction(SIGINT,   &act, 0); // Interrupt. 'Ctrl-C'
   rc |= sigaction(SIGQUIT,  &act, 0); // Quit. Produces core dump, 'Ctrl-\'
   rc |= sigaction(SIGUSR1,  &act, 0); // User signal 1
   rc |= sigaction(SIGUSR2,  &act, 0); // User signal 2
   rc |= sigaction(SIGRTMIN, &act, 0); // User real-time signal

   return rc == 0 ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
// Signal handler to free/reset resources
LOCAL void signalHdlr(int sig) {

   // Queue the signal if the handler is busy
   if (sHandlerInProgress) {
      raise(sig);
   }
   sHandlerInProgress = 1;
   sExit = 1;

   signal(sig, SIG_DFL);
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

