// Header

/*------------------------------------------------------------------------------
 *  System includes
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>

// Portability headers
#if __linux
 #include <sys/syscall.h>
#elif __WIN32
 #include <processthreadsapi.h>
 #include <windef.h>
 #include <winbase.h>
#endif

#include <iostream> // uncomment for debugging
#include <fstream>      // std::ifstream
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>


/*------------------------------------------------------------------------------
 *  Includes
 */
#include "BaCore.h"
#include "BaGenMacros.h"
#include "BaUtils.hpp"
#include <BaTmpTime.hpp>

/*------------------------------------------------------------------------------
 *  Defines and macros
 */
#define TAG "BaCore"
#define CHRONO_ std::chrono
#define SYSCLOCK_ CHRONO_::system_clock
#define CHRONOHRC CHRONO_::high_resolution_clock

#if __linux
# define PIDPATH "/var/run/BaseApi/"
#elif __WIN32
# define PIDPATH "\\var\\run\\BaseApi\\"
#endif

/*------------------------------------------------------------------------------
 *  Type definitions
 */
typedef CHRONO_::high_resolution_clock::time_point TTimePoint;
typedef CHRONO_::duration<SYSCLOCK_::rep, SYSCLOCK_::period> TDuration;

typedef enum EStatus {
   eInitializing = 0,
   eRunning      = 1,
   eDetached     = 2,
   eFinished     = 3
} EStatus;

typedef CHRONO_::high_resolution_clock::time_point TTimePoint;
typedef CHRONO_::duration<SYSCLOCK_::rep, SYSCLOCK_::period> TDuration;
typedef struct TThreadDesc {
   std::string             name;
   TBaCoreThreadFun        routine;
   TBaCoreThreadArg        *pArg;
   EBaCorePrio             prio;
   std::thread             *pThread;
   std::mutex              mtx;
   std::condition_variable cv;
   pid_t                   tid;
   EStatus                 status;
   TThreadDesc() : name(""), routine(0), pArg(0), prio(eBaCorePrio_Normal),
         pThread(0), mtx(), cv(), tid(0), status(eInitializing) {}
 } TThreadDesc;

 /*------------------------------------------------------------------------------
  *  Local functions and static variables
  */

LOCAL TDuration timed(TBaCoreFun func, void* pArg);
LOCAL void threadRoutine(TThreadDesc *pDesc);
LOCAL int prio2Prio(EBaCorePrio prio);
LOCAL EBaCorePrio prioFromPrio(int prio);

/*------------------------------------------------------------------------------
 *  Interface implementation
 */

//
int64_t BaCoreTimedS(TBaCoreFun fun, void* pArg) {
   return CHRONO_::duration_cast<CHRONO_::seconds>(timed(fun, pArg)).count();
}

//
int64_t BaCoreTimedMs(TBaCoreFun fun, void* pArg) {
   return CHRONO_::duration_cast<CHRONO_::milliseconds>(timed(fun, pArg)).count();
}

//
int64_t BaCoreTimedUs(TBaCoreFun fun, void* pArg) {
   return CHRONO_::duration_cast<CHRONO_::microseconds>(timed(fun, pArg)).count();
}

//
void BaCoreGetTStamp(TBaCoreTimeStamp *pStamp) {
   auto nowT = CHRONOHRC::now();
   pStamp->tt = CHRONOHRC::to_time_t(nowT);
   pStamp->micros = CHRONO_::duration_cast<CHRONO_::microseconds>(nowT.time_since_epoch()).count() % 1000000;
   pStamp->millis = pStamp->micros % 1000;
}

//
const char* BaCoreTStampToStr(const TBaCoreTimeStamp *pStamp) {
   if (!pStamp) {
      return 0;
   }

   struct tm timeStruct = tmp_4_9_2::localtime(pStamp->tt);
   std::string entry = tmp_4_9_2::put_time(&timeStruct, "%y/%m/%d %H:%M:%S") +
         "." + std::to_string(pStamp->millis);

   char * p = (char *) malloc(22);
   strncpy(p, entry.c_str(), 22-1);
   p[22 - 1] = 0;
   return 0;
}

//
void BaCoreSleep(int64_t s) {
   if(!s) {
      return;
   }

   std::this_thread::sleep_for(CHRONO_::seconds(s));
}

//
void BaCoreMSleep(int64_t ms) {
   if (!ms) {
      return;
   }

   std::this_thread::sleep_for(CHRONO_::milliseconds(ms));
}

//
void BaCoreUSleep(int64_t us) {
   if (!us) {
      return;
   }

   std::this_thread::sleep_for(CHRONO_::microseconds(us));
}

//
void BaCoreNSleep(int64_t ns) {
   std::this_thread::sleep_for(CHRONO_::nanoseconds(ns));
}

//
TBaCoreThreadHdl BaCoreCreateThread(const char *name, TBaCoreThreadFun routine,
      TBaCoreThreadArg *pArg, EBaCorePrio prio) {
   if (!routine) {
      return 0;
   }

   TThreadDesc *pDesc = new TThreadDesc();
   pDesc->name = name ? name : "";
   pDesc->pArg = pArg;
   pDesc->prio = (EBaCorePrio)prio;
   pDesc->routine = routine;

   // Lock until the thread pointer is assigned
   {
      std::lock_guard<std::mutex> lck(pDesc->mtx);
      pDesc->pThread = new std::thread(threadRoutine, pDesc);
   }

   return pDesc;
}

//
TBaBoolRC BaCoreDestroyThread(TBaCoreThreadHdl hdl, uint32_t timeOutMs) {
   TThreadDesc *pDesc = (TThreadDesc*)hdl;
   if (!pDesc || !pDesc->pThread) {
      return eBaBoolRC_Error;
   }

   // Mutex RAII lock
   {
      std::unique_lock<std::mutex> lck(pDesc->mtx);
      pDesc->pArg->exitTh = eBaBool_true;

      // Wait for the thread to end with a timeout
      if (pDesc->cv.wait_for(lck, CHRONO_::milliseconds(timeOutMs),
            // [cap list] (args) { body }
                [&pDesc] () { return pDesc->status == eFinished; })
          ) {
         pDesc->pThread->join();
      } else {
         // Detach it, let it live, and release the memory
         pDesc->status = eDetached;
         pDesc->pThread->detach();

      }
      delete pDesc->pThread;
      pDesc->pThread = 0;
   }

   // Do not delete pDesc inside the mutex. REMEMBER THAT!
   if (pDesc->status == eFinished) {
      delete pDesc;
   }

   return eBaBoolRC_Success;
}

//
TBaBoolRC BaCoreGetThreadInfo(TBaCoreThreadHdl hdl, TBaCoreThreadInfo *pInfo) {
   if (!hdl || !pInfo) {
      return eBaBoolRC_Error;
   }
   TThreadDesc *pDesc = (TThreadDesc*)hdl;

   // Get name
   size_t n = sizeof(pInfo->name);
   memset(pInfo->name, 0, n);
   strncpy(pInfo->name, pDesc->name.c_str(), n-1);

   //Get other info
   pInfo->prio      = pDesc->prio;
   pInfo->tid       = pDesc->tid;
   pInfo->isRunning = pDesc->status != eFinished;

   return eBaBoolRC_Success;
}

//
TBaBoolRC BaCoreSetOwnProcPrio(EBaCorePrio prio) {

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
EBaCorePrio BaCoreGetOwnProcPrio() {
   // In windows it is only a stub
#ifdef __WIN32
   return eBaCorePrio_Normal;
#else
   struct sched_param schedPrio;
   sched_getparam(0, &schedPrio);
   return prioFromPrio(schedPrio.__sched_priority);
#endif

}

//
const char* BaCoreGetOwnName() {
#ifdef __linux
   extern char *__progname;
   return __progname;
#else
   static std::string name = "";
   if (name == "") {
      TCHAR szFileName[MAX_PATH];
      GetModuleFileName(NULL, szFileName, MAX_PATH );
      name = BaPath::GetFilename(szFileName);
   }
   return name.c_str();
#endif
}


//
pid_t BaCoreReadPidFile(const char *progName, TBaBool internal) {
   if (!progName) {
      return -1;
   }

   std::string pidPath = PIDPATH;
   std::ifstream file(internal ? pidPath + progName : progName);
   pid_t pid = 0;

   // Check error
   if(!(file >> pid)) {
      return -1;
   }

   return pid;
}

//
TBaBoolRC BaCoreTestPidFile(const char *progName) {
   if (!progName) {
      return -1;
   }
   std::string nameOfPID;

   pid_t pid = BaCoreReadPidFile(progName, eBaBool_true);

   // Check if I am myself
   if ((pid < 0) || (pid == getpid())) {
      return eBaBoolRC_Success;
   }

   // Fake kill
#ifdef __WIN32

#else
   if (kill(pid, 0) && errno == ESRCH) {
      // process not running!
      return eBaBoolRC_Success;
   }
#endif

   // Process in PID is running
   // TODO: check if it is another instance of yourself
   // /proc/[PID]/comm
   std::string path = "/proc/" + std::to_string(pid) + "/comm";
   std::ifstream commFile(path, std::ios_base::binary);
   if(commFile.fail()) {
      return eBaBoolRC_Success;
   }

//   while(commFile.good())
//       std::cout << (char)commFile.get();

   std::getline(commFile, nameOfPID);

   if (nameOfPID != progName) {
      return eBaBoolRC_Success;
   }

   // There is another process called progName running
   return eBaBoolRC_Error;
}

//
TBaBoolRC BaCoreWritePidFile(const char *progName) {
   if (!progName) {
      return eBaBoolRC_Error;
   }

   if (!BaFS::Exists(PIDPATH)) {
      BaFS::MkDir(PIDPATH);
   }

   std::string pidfile = PIDPATH;
   pidfile.append(progName);
   std::ofstream myfile (pidfile);

   if (!myfile.is_open()) {
      // todo: log?
      return eBaBoolRC_Error;
   }

   pid_t pid = getpid();
   myfile << pid << std::endl;
   myfile.close();
   return myfile.fail() ? eBaBoolRC_Error : eBaBoolRC_Success;
}

//
TBaBoolRC BaCoreRemovePidFile(const char *progName) {
   if (!progName) {
      return eBaBoolRC_Error;
   }
   std::string pidfile = PIDPATH;
   pidfile.append(progName);
   return unlink(pidfile.c_str()) == 0 ? eBaBoolRC_Success : eBaBoolRC_Error;
}


/*------------------------------------------------------------------------------
 *  Local functions
 */
//
LOCAL void threadRoutine(TThreadDesc *pDesc) {
   if (!pDesc) {
      return;
   }

   // Get the native OS TID
#ifdef __linux
   pDesc->tid = syscall(SYS_gettid);
#elif __WIN32
   pDesc->tid = GetCurrentThreadId();
#endif

   pDesc->status = eRunning;

   // Wait for the pThread pointer to be assigned
   {
      std::lock_guard<std::mutex> lck(pDesc->mtx);
   }

   // Define the priority and scheduler
   int8_t sched = SCHED_OTHER;
   struct sched_param prio;
#ifdef __linux
   prio.__sched_priority = prio2Prio(pDesc->prio);
   // Set the soft real-time scheduler if required
   sched = pDesc->prio > eBaCorePrio_Highest ? SCHED_FIFO : SCHED_OTHER;
#elif __WIN32
   prio.sched_priority = prio2Prio(pDesc->prio);
#endif


   // Set thread name, scheduler, and priority
   pthread_t hld = pDesc->pThread->native_handle();
   pthread_setname_np(hld, pDesc->name.c_str());
   pthread_setschedparam(hld, sched, &prio);


   // Call the actual thread entry function /////
   pDesc->routine(pDesc->pArg);
   // ///////////////////////////////////// /////

   // Check if the thread was detached to either notify of release the descriptor
   if (pDesc->status != eDetached) {
      pDesc->status = eFinished;
      pDesc->cv.notify_all();
   } else {
      // Wait for detach to avoid a race condition
      {
         std::lock_guard<std::mutex> lck(pDesc->mtx);
      }
      delete pDesc;
   }
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
#elif __WIN32
      case eBaCorePrio_Minimum:    return THREAD_PRIORITY_IDLE;
      case eBaCorePrio_Low:        return THREAD_PRIORITY_LOWEST;
      case eBaCorePrio_Normal:     return THREAD_PRIORITY_NORMAL;
      case eBaCorePrio_High:       return THREAD_PRIORITY_ABOVE_NORMAL;
      case eBaCorePrio_Highest:    return THREAD_PRIORITY_HIGHEST;
      case eBaCorePrio_RT_Normal:  return THREAD_PRIORITY_TIME_CRITICAL - 6;
      case eBaCorePrio_RT_High:    return THREAD_PRIORITY_TIME_CRITICAL - 3;
      case eBaCorePrio_RT_Highest: return THREAD_PRIORITY_TIME_CRITICAL;
#endif
      default : return 1;
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
LOCAL TDuration timed(TBaCoreFun func, void* pArg) {
   const TTimePoint start = std::chrono::high_resolution_clock::now();
   func(pArg);
   const TTimePoint end = std::chrono::high_resolution_clock::now();
   return end - start;
}

