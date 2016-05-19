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
#include <string.h>

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
#define TAG      "BaApi"
#define CTRLTASK "BaseApiCtrlTask"
#define DEFDIR "/"
#define MINSAMPTIME_US 10000
#define TRACE_(fmt, ...) BaApiLogF(eBaLogPrio_Trace, TAG, fmt, ##__VA_ARGS__)
#define WARN_(fmt, ...) BaApiLogF(eBaLogPrio_Warning, TAG, fmt, ##__VA_ARGS__)
#define ERROR_(fmt, ...) BaApiLogF(eBaLogPrio_Error, TAG, fmt, ##__VA_ARGS__)

/*------------------------------------------------------------------------------
 *  Type definitions
 */
typedef struct TStats {
   bool     imRunning;
   uint64_t updCnt;
   uint64_t actDurUs;
} TStats;

/*------------------------------------------------------------------------------
 *  Local functions
 */
LOCAL void ctrlThreadRout(TBaCoreThreadArg* pArg);
LOCAL void signalHdlr(int sig);
LOCAL TBaBoolRC registerSignals();
LOCAL TBaBoolRC unregisterSignals();
LOCAL void resetStats(TStats &rStats);

/*------------------------------------------------------------------------------
 *  Static variables
 */

static CBaLog *spLog = 0;

static TStats sStats = {0};

static TBaCoreThreadHdl sCtrlThread = 0;
static TBaCoreThreadArg sCtrlThreadArg = {0};

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
   if (!pOpts || sStats.imRunning || !pOpts->update) {
      // todo: log?
      return eBaBoolRC_Error;
   }

   if (pOpts->init && !pOpts->init(pOpts->initArg)) {
      if (pOpts->exit) {
         pOpts->exit(pOpts->exitArg);
      }
      // todo: log?
      return eBaBoolRC_Error;
   }

   if(pOpts->log) {
      BaApiInitLogger(pOpts->log);
   } else {
      BaApiInitLoggerDef(CTRLTASK);
   }

   if (!BaCoreTestPidFile(CTRLTASK)) {
      ERROR_("Process already running");
   }

   sExit = 0;

   if (!registerSignals()) {
      return eBaBoolRC_Error;
   }

   // Lets replicate
   sStats.imRunning = true;
   pid_t pid = fork();

   // An error occurred, return
   if (pid < 0) {
      unregisterSignals();
      TRACE_("Parent talking: Error");
      resetStats(sStats);
      return eBaBoolRC_Success;
   }

   // Success: Let the parent return
   if (pid > 0) {
      unregisterSignals();
      TRACE_("Luke, I am you father");
      resetStats(sStats);
      return eBaBoolRC_Success;
   }

   // Now Luke is in command ////////////////////////////////////////

   // Write PID file
   BaCoreWritePidFile(CTRLTASK);

   // Change directory to default
   chdir(DEFDIR);

   TRACE_("prio: %i", BaCoreSetOwnProcPrio(pOpts->prio));
   TRACE_("NOOOO!!");


   uint64_t sampTimeUs = MAX(pOpts->cyleTimeMs, 10) * 1000;
   void (*updFun)(void*) = pOpts->update;
   void *pArg = pOpts->updateArg;

   // This is the actual control loop ////////////////////////////////////
   for ( ; !sExit; sStats.updCnt++) {
      sStats.actDurUs = BaCoreTimedUs(updFun, pArg);
      if (sStats.actDurUs + MINSAMPTIME_US > sampTimeUs) {
         BaCoreUSleep(MINSAMPTIME_US);
      } else {
         BaCoreUSleep(sampTimeUs - sStats.actDurUs);
      }
   }
   // ////////////////////////////////////////////////////////////////////

   if (pOpts->exit) {
      pOpts->exit(pOpts->exitArg);
   }
   TRACE_("Luke: I finished your quest father");
   BaCoreRemovePidFile(CTRLTASK);
   resetStats(sStats);

   // This is the child process, should not continue
   exit(EXIT_SUCCESS);

#endif
}


//
TBaBoolRC BaApiStopCtrlTask() {
   int rc = kill(BaCoreReadPidFile(CTRLTASK), SIGRTMIN);
   resetStats(sStats);
   return rc == 0 ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaApiStartCtrlThread(TBaApiCtrlTaskOpts* pOpts) {

   // todo: copy options locally?
   if (!pOpts || sStats.imRunning || !pOpts->update) {
      // todo: log?
      return eBaBoolRC_Error;
   }

   if (pOpts->init && !pOpts->init(pOpts->initArg)) {
      if (pOpts->exit) {
         pOpts->exit(pOpts->exitArg);
      }
      // todo: log?
      return eBaBoolRC_Error;
   }

   sStats.imRunning = true;
   sCtrlThreadArg.pArg = pOpts;
   sCtrlThread = BaCoreCreateThread(pOpts->name, ctrlThreadRout, &sCtrlThreadArg, pOpts->prio);
   if (!sCtrlThread) {
      sStats.imRunning = false;
   }
   return sCtrlThread ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaApiStopCtrlThread() {
   TBaBoolRC rc = BaCoreDestroyThread(sCtrlThread, 50);
   sCtrlThread = 0;
   resetStats(sStats);
   // todo: exit function here instead?
   return rc;
}

//
LOCAL TBaBoolRC registerSignals() {
   int rc = 0;
   struct sigaction act = {0};
   act.sa_handler = signalHdlr;

   // Termination signals from user
   rc  = sigaction(SIGTERM,  &act, 0); // Polite termination signal
   rc |= sigaction(SIGINT,   &act, 0); // Interrupt. 'Ctrl-C'
//   rc |= sigaction(SIGQUIT,  &act, 0); // Quit. Produces core dump, 'Ctrl-\'
//   rc |= sigaction(SIGUSR1,  &act, 0); // User signal 1
//   rc |= sigaction(SIGUSR2,  &act, 0); // User signal 2
   rc |= sigaction(SIGRTMIN, &act, 0); // User real-time signal
   return rc == 0 ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
LOCAL TBaBoolRC unregisterSignals() {
   int rc = 0;
   struct sigaction act = {0};
   act.sa_handler = SIG_DFL;
   rc  = sigaction(SIGTERM , &act, 0); // Polite termination signal
   rc |= sigaction(SIGINT,   &act, 0); // Interrupt. 'Ctrl-C'
//   rc |= sigaction(SIGQUIT,  &act, 0); // Quit. Produces core dump, 'Ctrl-\'
//   rc |= sigaction(SIGUSR1,  &act, 0); // User signal 1
//   rc |= sigaction(SIGUSR2,  &act, 0); // User signal 2
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
}

//
LOCAL void ctrlThreadRout(TBaCoreThreadArg* pArg) {

   TBaApiCtrlTaskOpts* pOpts = (TBaApiCtrlTaskOpts*) pArg->pArg;
   void (* update  )(void*) = pOpts->update;
   TRACE_("Ctrl Started");

   while (!pArg->exitTh) {
      uint64_t duration = BaCoreTimedUs(update, 0);
      if (duration + 9 > pOpts->cyleTimeMs) {
         BaCoreMSleep(10);
      } else {
         BaCoreMSleep(pOpts->cyleTimeMs - duration);
      }
   }

   if (pOpts->exit) {
      pOpts->exit(pOpts->exitArg);
   }
}

//
LOCAL void resetStats(TStats &rStats) {
   memset(&rStats, 0, sizeof(rStats));
}

