/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : OsProc.cpp
 *   Date     : Jun 26, 2016
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <signal.h>
#include <unistd.h>

#ifdef __WIN32
# include <windef.h>
# include <winbase.h>
#endif

// TODo: delete?
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <chrono>
#include "OsProc.h"
#include "BaUtils.hpp"
#include "BaGenMacros.h"
#include "BaLogMacros.h"
#include "BaMsg.h"


/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define TAG "OsProc"
#if __WIN32
# define PIDPATH "C:\\var\\run\\OsProc\\"
#else
# define PIDPATH "/var/run/OsProc/"
#endif
#define PIDEXT ".pid"
#define CTRLTASK "OsProcCtrlTask"
#define CTRLPIDFILE PIDPATH CTRLTASK PIDEXT
#define SHLEN BAPROC_SHORTNAMELEN
#define FULLLEN BAPROC_FULLNAMELEN
#define DEFDIR "/"
#define MINSLEEP_US  1000 // 1ms
#define MAXSLEEP_US 50000
#define MINCYCLET_US 2000 // 2ms
#define LASTCYCLE_US std::chrono::duration_cast<std::chrono::microseconds> \
   (std::chrono::steady_clock::now() - start).count()
#define MINCYCLET_US 2000

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
typedef std::chrono::steady_clock::time_point TTimePoint;
typedef std::chrono::duration<
      std::chrono::steady_clock::rep, std::chrono::steady_clock::period> TDuration;

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
LOCAL std::string getPIDName(pid_t pid);
LOCAL int prio2Prio(EBaCorePrio prio);
LOCAL EBaCorePrio prioFromPrio(int prio);

//
LOCAL void ctrlThreadRout(TBaCoreThreadArg* pArg);
LOCAL void signalHdlr(int sig);
LOCAL TBaBoolRC checkCtrlStart(const TOsProcCtrlTaskOpts* pOpts);
LOCAL TBaBoolRC registerSignals();
LOCAL TBaBoolRC unregisterSignals();
LOCAL void resetStats(TOsProcCtrlTaskStats &rStats);

/*------------------------------------------------------------------------------
    Local variables
 -----------------------------------------------------------------------------*/

static TBaCoreThreadHdl sCtrlThread = 0;
static TBaCoreThreadArg sCtrlThreadArg = {0};
static TOsProcCtrlTaskStats sTaskStats = {0};
static TOsProcCtrlTaskStats sThreadStats = {0};

// Handler in progress flag
static volatile sig_atomic_t sHandlerInProgress = 0;
static volatile sig_atomic_t sExit = 0;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/

//
const char* OsProcGetOwnFullName() {
#ifdef __WIN32
   static std::string name = "";
   if (name == "") {
      TCHAR szFileName[MAX_PATH];
      GetModuleFileName(NULL, szFileName, MAX_PATH);
      GetModuleFileNameA(NULL, szFileName, MAX_PATH);
      name = BaPath::GetFilename(szFileName);
   }
   return name.c_str();
#else
   extern char *__progname;
   return __progname;
#endif
}

//
const char* OsProcGetOwnShortName() {
   static char sShortOwnName[SHLEN] = {0};

   // Only write the first time
   if (!sShortOwnName[0]) {

      // strncpy() does not write the terminating null if size of from >= size
      // The string is initialized with 0s so we just do not write the last char
      strncpy(sShortOwnName, OsProcGetOwnFullName(), SHLEN-1);
   }

   return sShortOwnName;
}

//
TBaBoolRC OsProcWriteCtrlTaskPidFile() {
   if (!BaFS::Exists(PIDPATH)) {
      BaFS::MkDir(PIDPATH);
   }

   std::ofstream ofile(CTRLPIDFILE);
   if (!ofile.is_open()) {
      TRACE_("Cannot open PID file: " CTRLPIDFILE);
      return eBaBoolRC_Error;
   }

   ofile << getpid() << std::endl;
   ofile << OsProcGetOwnShortName() << std::endl;
   ofile.close();
   return ofile.fail() ? eBaBoolRC_Error : eBaBoolRC_Success;
}

//
pid_t OsProcReadCtrlTaskPidFile(char buf[SHLEN]) {
   std::string taskName(SHLEN, 0);
   std::ifstream ifile(CTRLPIDFILE);
   pid_t pid = 0;

   // Check error
   if(!(ifile >> pid)) {
      return 0;
   }

   if (buf) {
      if(!(ifile >> taskName)) {
         return 0;
      }

      // strncpy() does not write the terminating null if size of from >= size
      strncpy(buf, taskName.c_str(), SHLEN-1);
      buf[SHLEN-1] = 0;
   }

   return pid;
}

//
TBaBoolRC OsProcDelCtrlTaskPidFile() {
   return remove(CTRLPIDFILE) == 0 ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
const char* OsProcGetPIDName(pid_t pid, char buf[SHLEN]) {
   if (!buf) {
      buf = (char*) malloc(SHLEN);
      if(!buf) {
         return 0;
      }
   }

   std::string namePID = getPIDName(pid);
   if (namePID.empty()) {
      return 0;
   }

   strncpy(buf, namePID.c_str(), SHLEN-1);
   return buf;
}

//
TBaBoolRC OsProcWriteOwnPidFile() {
   if (!BaFS::Exists(PIDPATH)) {
      BaFS::MkDir(PIDPATH);
   }

   std::string pidfile = PIDPATH +
         BaPath::ChangeFileExtension(OsProcGetOwnShortName(), PIDEXT);
   std::ofstream ofile(pidfile);

   if (!ofile.is_open()) {
      TRACE_("Cannot open PID file: %s", pidfile.c_str());
      return eBaBoolRC_Error;
   }

   ofile << getpid() << std::endl;
   ofile.close();
   return ofile.fail() ? eBaBoolRC_Error : eBaBoolRC_Success;
}

//
pid_t OsProcReadPidFile(const char *progName, TBaBool internal) {
   if (!progName) {
      return 0;
   }

   std::string pidPath;
   if (internal) {
      pidPath = PIDPATH +
            BaPath::ChangeFileExtension(progName, PIDEXT);
   } else {
      pidPath = progName;
   }

   std::ifstream file(pidPath);
   pid_t pid = 0;

   // Check error
   if(!(file >> pid)) {
      return 0;
   }

   return pid;
}

//
TBaBoolRC OsProcDelPidFile(const char *progName, TBaBool internal) {
   if (!progName) {
      return eBaBoolRC_Error;
   }

   std::string pidPath = "";
   if (internal) {
      pidPath = PIDPATH +
            BaPath::ChangeFileExtension(progName, PIDEXT);
   } else {
      pidPath.append(progName);
   }

   return remove(pidPath.c_str()) == 0 ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBool OsProcPidFileIsRunning(const char *progName, TBaBool internal) {
   if (!progName) {
      return eBaBool_false;
   }

   std::string binName = progName;
   char buf[BAPROC_SHORTNAMELEN];
   pid_t pid = 0;

   if (!internal) {
      binName = BaPath::GetFilename(binName);
   }

   if (binName == CTRLTASK) {
      pid = OsProcReadCtrlTaskPidFile(buf);
      if (pid) {
         binName = buf;
      }
   } else {
      pid = OsProcReadPidFile(progName, internal);
   }

   // Check if I am myself
   if (!pid || (pid == getpid())) {
      // OK, I can overwrite my own file
      return eBaBool_false;
   }

   // Fake kill
#ifdef __WIN32
   return eBaBool_false;
#else

   // Check if the process from the PID is running
   if (kill(pid, 0) && errno == ESRCH) {
      // Orocess not running, can overwrite file
      return eBaBool_false;
   }
#endif

   // Process in PID is running
   // Open proc file from PID: /proc/[PID]/comm

   // Read the executable name and compare it with the given name
   if (getPIDName(pid) != binName) {
      return eBaBool_false;
   }

   // There is another process called progName running
   return eBaBool_true;
}

//
TBaBool OsProcCtrlTaskPidIsRunning() {
   return OsProcPidFileIsRunning(CTRLTASK, eBaBool_true);
}

//
TBaBoolRC OsProcSetOwnPrio(EBaCorePrio prio) {
   if (prio < eBaCorePrio_Minimum || prio > eBaCorePrio_RT_Highest) {
      return eBaBoolRC_Error;
   }

   // Define the priority and scheduler
   int8_t sched = SCHED_OTHER;
   struct sched_param schedPrio;
#ifdef __linux
   schedPrio.__sched_priority = prio2Prio(prio);
   // Set the soft real-time scheduler if required
   sched = prio > eBaCorePrio_Highest ? SCHED_FIFO : SCHED_OTHER;
#elif __WIN32
   schedPrio.sched_priority = prio2Prio(prio);
#endif

   int ret = sched_setscheduler(0, sched, &schedPrio);
   return ret == 0 ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
EBaCorePrio OsProcGetOwnPrio() {
   // In windows it is only a stub
#ifdef __WIN32
   return eBaCorePrio_Normal;
#else
   struct sched_param schedPrio;
   sched_getparam(0, &schedPrio);
   return prioFromPrio(schedPrio.__sched_priority);
#endif

}


// ===================================================
//
TBaBoolRC OsApiStartCtrlTask(const TOsProcCtrlTaskOpts* pOpts) {
   if (!checkCtrlStart(pOpts)) {
      return eBaBoolRC_Error;
   }
#ifdef __WIN32
   return eBaBoolRC_Error;
#else

   if (OsProcPidFileIsRunning(CTRLTASK, eBaBool_true)) {
      ERROR_("Process already running");
      return eBaBoolRC_Error;
   }

   // Reset exit flag
   sExit = 0;

   // Set signals before forking to the child inherits the signals
   if (!registerSignals()) {
      resetStats(sTaskStats);
      return eBaBoolRC_Error;
   }

   // Lets replicate. Block future replications with the flag
   pid_t pid = fork();

   // An error occurred, return
   if (pid < 0) {
      unregisterSignals();
      ERROR_("Fork failed");
      BASYSLOG(TAG, "Fork failed");
      resetStats(sTaskStats);
      return eBaBoolRC_Success;
   }

   // Success: Let the parent return
   // Luke, I am your father
   if (pid > 0) {
      unregisterSignals();
      TRACE_("Parent: Fork successful");
      sTaskStats.imRunning = eBaBool_true;
      return eBaBoolRC_Success;
   }

   // Luke:  NOooooo!!!
   // Now Luke is in command ////////////////////////////////////////
   TRACE_("Child: Fork successful");

   // Write PID file
   OsProcWriteCtrlTaskPidFile();

   // Change directory to default
   chdir(DEFDIR);
   TRACE_("Prio set: %s", OsProcSetOwnPrio(pOpts->prio) ? "Ok" :"Not ok");

   TTimePoint start;
   uint64_t sampTimeUs = MAX(pOpts->cyleTimeUs, MINCYCLET_US);
   void (*updFun)(void*) = pOpts->update;
   void *pArg = pOpts->updateArg;
   IBaMsg *pTaskCycleMsg = IBaMsgCreate();

   // This is the actual control loop ////////////////////////////////////
   for ( ; !sExit; sTaskStats.updCnt++, sTaskStats.lastCycleUs = LASTCYCLE_US) {
      start = std::chrono::steady_clock::now();

      // ==== Update function call and duration ====
      sTaskStats.lastDurUs = BaCoreTimedUs(updFun, pArg);
      // ==== ====End call =======

      // There is no need to make short sleeps like in the control thread
      // because nobody waits for the task to end.
      // TODO: Eventually this could be necessary if persistent variables are
      // implemented. In this case, the task has to end relatively quickly and
      // save the persistent state before power goes out.
      if (sTaskStats.lastDurUs + MINSLEEP_US > sampTimeUs) {
         // Log error
         if (pTaskCycleMsg) {
            pTaskCycleMsg->SetDefLogF(eBaLogPrio_Warning, TAG,
                  "Update() exceeded the sample time: %" PRIu64 ">%" PRIu64,
                                                   sTaskStats.lastDurUs, sampTimeUs);
         }
         BaCoreUSleep(MINSLEEP_US);
      } else {
         BaCoreUSleep(sampTimeUs - sTaskStats.lastDurUs);
      }
   }
   // ////////////////////////////////////////////////////////////////////

   if (pOpts->exit) {
      pOpts->exit(pOpts->exitArg);
   }

   OsProcDelCtrlTaskPidFile();
   resetStats(sTaskStats);

   // This is the child process, should not continue
   TRACE_("Control task exit");
   exit(EXIT_SUCCESS);

#endif
}

//
TBaBoolRC OsApiStopCtrlTask() {
   BaApiExitLogger();
#ifdef __WIN32
   resetStats(sTaskStats);
   return eBaBoolRC_Error;
#else

   pid_t pid = OsProcReadCtrlTaskPidFile(0);
   if (pid) {
      int killRc = kill(pid, SIGRTMIN);
      if (killRc == 0) {
         resetStats(sTaskStats);
         return eBaBool_true;
      }
      ERROR_("Kill failed (%i)", killRc);
   } else {
      WARN_("Failed to read the PID of CtrlTask");
   }

   resetStats(sTaskStats);
   return eBaBoolRC_Error;
#endif
}

//
TBaBoolRC OsApiStartCtrlThread(const TOsProcCtrlTaskOpts* pOpts) {
   if (sCtrlThread || !checkCtrlStart(pOpts)) {
      return eBaBoolRC_Error;
   }

   // Reset exit flag
   sExit = 0;

   sThreadStats.imRunning = eBaBool_true;
   sCtrlThreadArg.pArg = (void*)pOpts;
   sCtrlThread = BaCoreCreateThread(pOpts->name, ctrlThreadRout, &sCtrlThreadArg, pOpts->prio);
   if (!sCtrlThread) {
      sThreadStats.imRunning = eBaBool_false;
   }
   return sCtrlThread ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsApiStopCtrlThread() {
   sExit = 1;

   // Wait for thread to end
   TBaBoolRC rc = BaCoreDestroyThread(sCtrlThread, 2*MAXSLEEP_US/1000);
   sCtrlThread = 0;
   resetStats(sThreadStats);
   return rc;
}

//
TBaBoolRC OsApiGetCtrlTaskStats(TOsProcCtrlTaskStats *pStats) {
   if (!pStats) {
      return eBaBoolRC_Error;
   }

   *pStats = sTaskStats;
   return eBaBoolRC_Success;
}

//
TBaBoolRC OsApiGetCtrlThreadStats(TOsProcCtrlTaskStats *pStats) {
   if (!pStats) {
      return eBaBoolRC_Error;
   }

   *pStats = sThreadStats;
   return eBaBoolRC_Success;
}


/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
LOCAL std::string getPIDName(pid_t pid) {
   std::string nameOfPID;
#ifdef __WIN32
   nameOfPID = "ImAWinStub";
#else
   // Process in PID is running
   // Open proc file from PID: /proc/[PID]/comm
   std::ifstream commFile("/proc/" + std::to_string(pid) + "/comm");
   if(commFile.fail()) {
      return "";
   }

   // Read the executable name
   std::getline(commFile, nameOfPID);

#endif
   return nameOfPID;
}

//
LOCAL int prio2Prio(EBaCorePrio prio) {
   //  Linux
   // Scheduler  def min max
   // SCHED_FIFO      1  99
   // SCHED_OTHER 0  -20 19
   //  Windows, Only SCHED_OTHER available
   // Min -15
   // Max  15
   switch (prio) {
#ifdef __linux
      case eBaCorePrio_Minimum:    return -20;
      case eBaCorePrio_Low:        return -10;
      case eBaCorePrio_Normal:     return   0;
      case eBaCorePrio_High:       return  10;
      case eBaCorePrio_Highest:    return  19;
      case eBaCorePrio_RT_Normal:  return  20;
      case eBaCorePrio_RT_High:    return  45;
      case eBaCorePrio_RT_Highest: return  70;
      default : return 0;
#elif __WIN32
      case eBaCorePrio_Minimum:    return THREAD_PRIORITY_IDLE;
      case eBaCorePrio_Low:        return THREAD_PRIORITY_LOWEST;
      case eBaCorePrio_Normal:     return THREAD_PRIORITY_NORMAL;
      case eBaCorePrio_High:       return THREAD_PRIORITY_ABOVE_NORMAL;
      case eBaCorePrio_Highest:    return THREAD_PRIORITY_HIGHEST;
      case eBaCorePrio_RT_Normal:  return THREAD_PRIORITY_TIME_CRITICAL - 6;
      case eBaCorePrio_RT_High:    return THREAD_PRIORITY_TIME_CRITICAL - 3;
      case eBaCorePrio_RT_Highest: return THREAD_PRIORITY_TIME_CRITICAL;
      default : return THREAD_PRIORITY_NORMAL;
#endif
   }

}

//
LOCAL EBaCorePrio prioFromPrio(int prio) {
   //  Linux
   // Scheduler  def min max
   // SCHED_FIFO      1  99
   // SCHED_OTHER 0  -20 19
   //  Windows, Only SCHED_OTHER available
   // Min -15
   // Max  15


#ifdef __linux
//      case eBaCorePrio_Minimum:    return -20;
//      case eBaCorePrio_Low:        return -10;
//      case eBaCorePrio_Normal:     return   0;
//      case eBaCorePrio_High:       return  10;
//      case eBaCorePrio_Highest:    return  19;
//      case eBaCorePrio_RT_Normal:  return  20;
//      case eBaCorePrio_RT_High:    return  45;
//      case eBaCorePrio_RT_Highest: return  70;
   if (prio ==  0) return eBaCorePrio_Normal;
   if (prio == 20) return eBaCorePrio_RT_Normal;

   if (prio < -10) return eBaCorePrio_Minimum;
   if (prio <   0) return eBaCorePrio_Low;
   if (prio <  10) return eBaCorePrio_High;
   if (prio <  20) return eBaCorePrio_Highest;

   if (prio <  50) return eBaCorePrio_RT_High;
   return eBaCorePrio_RT_Highest;

#elif __WIN32
   switch (prio) {
      case THREAD_PRIORITY_IDLE:              return eBaCorePrio_Minimum;
      case THREAD_PRIORITY_LOWEST:            return eBaCorePrio_Low;
      case THREAD_PRIORITY_NORMAL:            return eBaCorePrio_Normal;
      case THREAD_PRIORITY_ABOVE_NORMAL:      return eBaCorePrio_High;
      case THREAD_PRIORITY_HIGHEST:           return eBaCorePrio_Highest;
      case THREAD_PRIORITY_TIME_CRITICAL - 6: return eBaCorePrio_RT_Normal;
      case THREAD_PRIORITY_TIME_CRITICAL - 3: return eBaCorePrio_RT_High;
      case THREAD_PRIORITY_TIME_CRITICAL:     return eBaCorePrio_RT_Highest;
      default : return eBaCorePrio_Normal;
   }
#endif

}

//
LOCAL TBaBoolRC checkCtrlStart(const TOsProcCtrlTaskOpts* pOpts) {

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

   sTaskStats.imRunning = eBaBool_true;
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
   const TOsProcCtrlTaskOpts* pOpts = (const TOsProcCtrlTaskOpts*) pArg->pArg;
   auto update  = pOpts->update;
   void * updateArg = pOpts->updateArg;
   uint64_t sampTimeUs = MAX(pOpts->cyleTimeUs, MINCYCLET_US);
   uint64_t cycleCumUs = MAXSLEEP_US;
   TRACE_("Ctrl thread started");
   uint64_t toSleep;
   uint8_t goodCnt = 0;
   IBaMsg *pThreadCycleMsg = IBaMsgCreate();

   // This is the actual control loop ////////////////////////////////////
   for ( ; !sExit; sThreadStats.updCnt++, cycleCumUs += LASTCYCLE_US) {
      start = std::chrono::steady_clock::now();

      // The cycle time has elapsed. Call update
      if (cycleCumUs >= sampTimeUs) {
         sThreadStats.lastCycleUs = cycleCumUs;
         sThreadStats.lastDurUs = BaCoreTimedUs(update, updateArg);

         // The new cumulated cycle is
         cycleCumUs = (cycleCumUs - sampTimeUs);

         // If the update takes longer than the cycle time, log it.
         if (sTaskStats.lastDurUs > sampTimeUs) {

            // Log error
            if (pThreadCycleMsg) {
               pThreadCycleMsg->SetDefLogF(eBaLogPrio_Warning, TAG,
                     "Update() exceeded the sample time: %" PRIu64 ">%" PRIu64,
                        sThreadStats.lastDurUs, sampTimeUs);
            }

            BaCoreUSleep(MINSLEEP_US);
            continue;
         } else {
            ++goodCnt;
            // Reset log message
            if (goodCnt > 5 && pThreadCycleMsg) {
               pThreadCycleMsg->Reset();
            }
         }

      }

      // Cycle + MaxSleep < sample time: sleep the maximum possible
      if (cycleCumUs + MAXSLEEP_US <= sampTimeUs) {
         BaCoreUSleep(MAXSLEEP_US);
         continue;
      }

      // Sleep the  required time or the minimum minimum possible
      toSleep = sampTimeUs - cycleCumUs;
      BaCoreUSleep(toSleep < MINSLEEP_US ? MINSLEEP_US : toSleep);

   }
   // ////////////////////////////////////////////////////////////////////

   if (pThreadCycleMsg) {
      BaMsgDestroy(pThreadCycleMsg);
   }

   if (pOpts->exit) {
      pOpts->exit(pOpts->exitArg);
   }

   BaApiExitLogger();
}

//
LOCAL void resetStats(TOsProcCtrlTaskStats &rStats) {
   memset(&rStats, 0, sizeof(rStats));
}
