// Header

/*------------------------------------------------------------------------------
 *  System includes
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>

#ifndef __WIN32
 #include <sys/syscall.h>
#else
 #include <processthreadsapi.h>
 #include <windef.h>
 #include <winbase.h>
#endif

//#include <iostream> // uncomment for debugging
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
#include "BaTmpTime.hpp"

/*------------------------------------------------------------------------------
 *  Defines and macros
 */
#define TAG "BaCore"
#define CHRONO_ std::chrono
#define SYSCLOCK_ CHRONO_::system_clock
#define CHRONOHRC CHRONO_::high_resolution_clock
#define CTRLTASK "BaseApiCtrlTask"

#if __linux
# define PIDPATH "/var/run/BaseApi/"
#elif __WIN32
# define PIDPATH "C:\\var\\run\\BaseApi\\"
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
   if (!pStamp) {
      return;
   }

   auto nowT = CHRONOHRC::now();
   pStamp->tt = CHRONOHRC::to_time_t(nowT);
   pStamp->micros = CHRONO_::duration_cast<CHRONO_::microseconds>(nowT.time_since_epoch()).count() % 1000000;

   // Get the millis part of the second with rounding to the nearest milli
   pStamp->millis = pStamp->micros / 1000;
   if (pStamp->micros % 1000 >= 500) {
      if (pStamp->millis == 999) {
         pStamp->millis = 0;
         pStamp->tt++;
      } else {
         pStamp->millis++;
      }
   }

}

// Stamp length is 22
const char* BaCoreTStampToStr(const TBaCoreTimeStamp *pStamp, char *pBuf) {
   if (!pStamp || pStamp->micros >= 1000000 || pStamp->millis >= 1000) {
      return 0;
   }

   if (!pBuf) {
      pBuf = (char *) malloc(BACORE_TSTAMPLEN);
   }

   struct tm timeStruct = tmp_4_9_2::localtime(pStamp->tt);
   std::string epochStr = tmp_4_9_2::put_time(&timeStruct, "%y/%m/%d %H:%M:%S");

   // Write the time stamp up to seconds and include the terminating null
   strncpy(pBuf, epochStr.c_str(), BACORE_TSTAMPLEN-4);

   // Append the millis part overwriting the former terminating null
   // WIN: 4 chars + ending null = 5
   // LIN: 4 chars + ending null = 5
   uint8_t off = BACORE_TSTAMPLEN - 5;
   snprintf(pBuf+off, 5, ".%03d", pStamp->millis);
   pBuf[BACORE_TSTAMPLEN-1] = 0; // safety? necessary?
   return pBuf;
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
   // strncpy() does not write the terminating null if size of from >= size
   memset(pInfo->name, 0, n);
   strncpy(pInfo->name, pDesc->name.c_str(), n-1);

   //Get other info
   pInfo->prio      = pDesc->prio;
   pInfo->tid       = pDesc->tid;
   pInfo->isRunning = pDesc->status != eFinished;

   return eBaBoolRC_Success;
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
LOCAL TDuration timed(TBaCoreFun func, void* pArg) {
   const TTimePoint start = std::chrono::high_resolution_clock::now();
   func(pArg);
   const TTimePoint end = std::chrono::high_resolution_clock::now();
   return end - start;
}

