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

#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <inttypes.h>

// Portability headers
#ifdef __linux

#elif __WIN32
# include <windows.h>
#endif

#include "BaseApi.h"
#include "CBaLog.h"
#include "BaGenMacros.h"
#include "BaLogMacros.h"
#include "BaProc.h"

/*------------------------------------------------------------------------------
 *  Defines
 */
#define TAG      "BaApi"
#define CTRLTASK "BaseApiCtrlTask"
#define DEFDIR "/"
#define MINSLEEP_US 10000
#define MAXSLEEP_US 50000
#define LASTCYCLE_US std::chrono::duration_cast<std::chrono::microseconds> \
   (std::chrono::steady_clock::now() - start).count()

/*------------------------------------------------------------------------------
 *  Type definitions
 */
typedef std::chrono::steady_clock::time_point TTimePoint;
typedef std::chrono::duration<std::chrono::steady_clock::rep, std::chrono::steady_clock::period> TDuration;

/*------------------------------------------------------------------------------
 *  Local functions
 */
LOCAL void ctrlThreadRout(TBaCoreThreadArg* pArg);
LOCAL void signalHdlr(int sig);
LOCAL TBaBoolRC checkCtrlStart(const TBaApiCtrlTaskOpts* pOpts);
LOCAL TBaBoolRC registerSignals();
LOCAL TBaBoolRC unregisterSignals();
LOCAL void resetStats(TBaApiCtrlTaskStats &rStats);

/*------------------------------------------------------------------------------
 *  Static variables
 */

static CBaLog *spLog = 0;
static bool   sExtLogger = false;

static TBaApiCtrlTaskStats sStats = {0};

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
TBaBoolRC BaApiInitLogger(TBaLogDesc log) {
   if (spLog) {
      return eBaBoolRC_Success;
   }

   spLog = dynamic_cast<CBaLog*>(log.pLog);
   sExtLogger = true;
   return spLog ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaApiExitLogger() {
   if (!spLog) {
      return eBaBoolRC_Success;
   }

   // Only destroy the logger if you created it. NEVER mess with data owned by
   // the user
   bool ret = sExtLogger ? true : CBaLog::Destroy(spLog);
   sExtLogger = false;
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
TBaBoolRC BaApiStartCtrlTask(const TBaApiCtrlTaskOpts* pOpts) {
   if (!checkCtrlStart(pOpts)) {
      return eBaBoolRC_Error;
   }
#ifdef __WIN32
   return eBaBoolRC_Error;
#else

   if (BaProcPidFileIsRunning(CTRLTASK, eBaBool_true)) {
      ERROR_("Process already running");
      return eBaBoolRC_Error;
   }

   // Reset exit flag
   sExit = 0;

   // Set signals before forking to the child inherits the signals
   if (!registerSignals()) {
      resetStats(sStats);
      return eBaBoolRC_Error;
   }

   // Lets replicate. Block future replications with the flag
   pid_t pid = fork();

   // An error occurred, return
   if (pid < 0) {
      unregisterSignals();
      ERROR_("Fork failed");
      BASYSLOG(TAG, "Fork failed");
      resetStats(sStats);
      return eBaBoolRC_Success;
   }

   // Success: Let the parent return
   if (pid > 0) {
      unregisterSignals();
      TRACE_("Fork successful: Luke, I am you father");
      sStats.imRunning = eBaBool_true;
//      resetStats(sStats);
      return eBaBoolRC_Success;
   }

   // Now Luke is in command ////////////////////////////////////////

   // Write PID file
   BaProcWriteCtrlTaskPidFile();

   // Change directory to default
   chdir(DEFDIR);

   TRACE_("prio: %i", BaProcSetOwnPrio(pOpts->prio));
   TRACE_("NOOOO!!");


   TTimePoint start;
   uint64_t sampTimeUs = MAX(pOpts->cyleTimeMs, 10) * 1000;
   void (*updFun)(void*) = pOpts->update;
   void *pArg = pOpts->updateArg;

   // This is the actual control loop ////////////////////////////////////
   for ( ; !sExit; sStats.updCnt++, sStats.lastCycleUs = LASTCYCLE_US) {
      start = std::chrono::steady_clock::now();

      sStats.lastDurUs = BaCoreTimedUs(updFun, pArg);
      if (sStats.lastDurUs + MINSLEEP_US > sampTimeUs) {
         BaCoreUSleep(MINSLEEP_US);
      } else {
         BaCoreUSleep(sampTimeUs - sStats.lastDurUs);
      }
   }
   // ////////////////////////////////////////////////////////////////////

   if (pOpts->exit) {
      pOpts->exit(pOpts->exitArg);
   }
   TRACE_("Luke: I finished your quest father");
   BaProcDelCtrlTaskPidFile();
   resetStats(sStats);

   // This is the child process, should not continue
   exit(EXIT_SUCCESS);

#endif
}

//
TBaBoolRC BaApiStopCtrlTask() {
   BaApiExitLogger();
#ifdef __WIN32
   resetStats(sStats);
   return eBaBoolRC_Error;
#else

   pid_t pid = BaProcReadCtrlTaskPidFile(0);
   if (pid) {
      int killRc = kill(pid, SIGRTMIN);
      if (killRc == 0) {
         resetStats(sStats);
         return eBaBool_true;
      }
      ERROR_("Kill failed (%i)", killRc);
   } else {
      WARN_("Failed to read the PID of CtrlTask");
   }

   resetStats(sStats);
   return eBaBoolRC_Error;
#endif
}

//
TBaBoolRC BaApiStartCtrlThread(const TBaApiCtrlTaskOpts* pOpts) {
   if (!checkCtrlStart(pOpts)) {
      return eBaBoolRC_Error;
   }

   sStats.imRunning = eBaBool_true;
   sCtrlThreadArg.pArg = (void*)pOpts;
   sCtrlThread = BaCoreCreateThread(pOpts->name, ctrlThreadRout, &sCtrlThreadArg, pOpts->prio);
   if (!sCtrlThread) {
      sStats.imRunning = eBaBool_false;
   }
   return sCtrlThread ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaApiStopCtrlThread() {
   sExit = true;
   //todo
   TBaBoolRC rc = BaCoreDestroyThread(sCtrlThread, 50);
   sCtrlThread = 0;
   resetStats(sStats);
   // todo: exit function here instead? we do not have opts here with the callback
   return rc;
}

//
TBaBoolRC BaApiGetCtrlTaskStats(TBaApiCtrlTaskStats *pStats) {
   if (!pStats) {
      return eBaBoolRC_Error;
   }

   *pStats = sStats;
   return eBaBoolRC_Success;
}

//
LOCAL TBaBoolRC checkCtrlStart(const TBaApiCtrlTaskOpts* pOpts) {
   // todo: copy options locally?

   // Initialize the general logger. If the user already initialized it
   // somewhere else, this will have no effect
   if(pOpts->log.pLog) {
      BaApiInitLogger(pOpts->log);
   } else {
      BaApiInitLoggerDef(CTRLTASK);
   }

   if (pOpts->init && !pOpts->init(pOpts->initArg)) {
      if (pOpts->exit) {
         pOpts->exit(pOpts->exitArg);
      }
      WARN_("User init failed");
      return eBaBoolRC_Error;
   }

   sStats.imRunning = eBaBool_true;
   return eBaBoolRC_Success;
}

//
LOCAL TBaBoolRC registerSignals() {
#ifdef __WIN32
   return eBaBoolRC_Error;
#else
   int rc = 0;
   struct sigaction act = {0};
   act.sa_handler = signalHdlr;

   // Termination signals from user
   rc  = sigaction(SIGTERM,  &act, 0); // Polite termination signal
   rc |= sigaction(SIGINT,   &act, 0); // Interrupt. 'Ctrl-C'
   rc |= sigaction(SIGRTMIN, &act, 0); // User real-time signal
   return rc == 0 ? eBaBoolRC_Success : eBaBoolRC_Error;
#endif
}

//
LOCAL TBaBoolRC unregisterSignals() {
#ifdef __WIN32
   return eBaBoolRC_Error;
#else
   int rc = 0;
   struct sigaction act = {0};
   act.sa_handler = SIG_DFL;
   rc  = sigaction(SIGTERM , &act, 0); // Polite termination signal
   rc |= sigaction(SIGINT,   &act, 0); // Interrupt. 'Ctrl-C'
   rc |= sigaction(SIGRTMIN, &act, 0); // User real-time signal
   return rc == 0 ? eBaBoolRC_Success : eBaBoolRC_Error;
#endif
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
   TTimePoint start;
   const TBaApiCtrlTaskOpts* pOpts = (const TBaApiCtrlTaskOpts*) pArg->pArg;
   auto update  = pOpts->update;
   void * updateArg = pOpts->updateArg;
   uint64_t sampTimeUs = MAX(pOpts->cyleTimeMs, 10) * 1000;
   uint64_t cycleCumUs = MAXSLEEP_US;
   TRACE_("Ctrl thread started");
   uint64_t toSleep;

   // This is the actual control loop ////////////////////////////////////
   for ( ; !sExit; sStats.updCnt++, cycleCumUs += LASTCYCLE_US) {
      start = std::chrono::steady_clock::now();

      // update Fun
      if (cycleCumUs >= sampTimeUs) {
         sStats.lastCycleUs = cycleCumUs;
         sStats.lastDurUs = BaCoreTimedUs(update, updateArg);
         cycleCumUs = sStats.lastDurUs + (cycleCumUs- MAXSLEEP_US);
         if (cycleCumUs > sampTimeUs) {
            // todo Log with state
            WARN_("Update() exceeded the sample time: %" PRIu64 "> %" PRIu64,
                  cycleCumUs, sampTimeUs);
         }
      }

      // Cycle > sample time
      if (cycleCumUs + MAXSLEEP_US <= sampTimeUs) {
         BaCoreUSleep(MAXSLEEP_US);
         break;
      // Cycle
      }

      toSleep = sampTimeUs - cycleCumUs;
      BaCoreUSleep(toSleep < MINSLEEP_US ? MINSLEEP_US : toSleep);


   }
   // ////////////////////////////////////////////////////////////////////

   if (pOpts->exit) {
      pOpts->exit(pOpts->exitArg);
   }
}

//
LOCAL void resetStats(TBaApiCtrlTaskStats &rStats) {
   memset(&rStats, 0, sizeof(rStats));
}

