/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaProc.cpp
 *   Date     : Jun 26, 2016
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <string.h>
#include <signal.h>
#include <unistd.h>

#ifdef __WIN32
# include <windef.h>
# include <winbase.h>
#endif

#include <string>
#include <fstream> // std::ifstream

#include "BaProc.h"
#include "BaUtils.hpp"
#include "BaGenMacros.h"


/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#if __WIN32
# define PIDPATH "C:\\var\\run\\BaseApi\\"
#else
# define PIDPATH "/var/run/BaseApi/"
#endif
#define PIDEXT ".pid"
#define CTRLTASK "BaseApiCtrlTask"
#define CTRLPIDFILE PIDPATH CTRLTASK PIDEXT
#define SHLEN BAPROC_SHORTNAMELEN
#define FULLLEN BAPROC_FULLNAMELEN

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
LOCAL std::string getPIDName(pid_t pid);
LOCAL int prio2Prio(EBaCorePrio prio);
LOCAL EBaCorePrio prioFromPrio(int prio);

/*------------------------------------------------------------------------------
    Local variables
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/

//
const char* BaProcGetOwnFullName() {
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
const char* BaProcGetOwnShortName() {
   static char sShortOwnName[SHLEN] = {0};

   // Only write the first time
   if (!sShortOwnName[0]) {

      // strncpy() does not write the terminating null if size of from >= size
      // The string is initialized with 0s so we just do not write the last char
      strncpy(sShortOwnName, BaProcGetOwnFullName(), SHLEN-1);
   }

   return sShortOwnName;
}

//
TBaBoolRC BaProcWriteCtrlTaskPidFile() {
   if (!BaFS::Exists(PIDPATH)) {
      BaFS::MkDir(PIDPATH);
   }

   std::ofstream ofile(CTRLPIDFILE);
   if (!ofile.is_open()) {
      // todo: log?
      return eBaBoolRC_Error;
   }

   ofile << getpid() << std::endl;
   ofile << BaProcGetOwnShortName() << std::endl;
   ofile.close();
   return ofile.fail() ? eBaBoolRC_Error : eBaBoolRC_Success;
}

//
pid_t BaProcReadCtrlTaskPidFile(char buf[SHLEN]) {
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
TBaBoolRC BaProcDelCtrlTaskPidFile() {
   return remove(CTRLPIDFILE) == 0 ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
const char* BaProcGetPIDName(pid_t pid, char buf[SHLEN]) {
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
TBaBoolRC BaProcWriteOwnPidFile() {
   if (!BaFS::Exists(PIDPATH)) {
      BaFS::MkDir(PIDPATH);
   }

   std::string pidfile = PIDPATH +
         BaPath::ChangeFileExtension(BaProcGetOwnShortName(), PIDEXT);
   std::ofstream ofile(pidfile);

   if (!ofile.is_open()) {
      // todo: log?
      return eBaBoolRC_Error;
   }

   ofile << getpid() << std::endl;
   ofile.close();
   return ofile.fail() ? eBaBoolRC_Error : eBaBoolRC_Success;
}

//
pid_t BaProcReadPidFile(const char *progName, TBaBool internal) {
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
TBaBoolRC BaProcDelPidFile(const char *progName, TBaBool internal) {
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
TBaBool BaProcPidFileIsRunning(const char *progName, TBaBool internal) {
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
      pid = BaProcReadCtrlTaskPidFile(buf);
      if (pid) {
         binName = buf;
      }
   } else {
      pid = BaProcReadPidFile(progName, internal);
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
TBaBoolRC BaProcSetOwnPrio(EBaCorePrio prio) {
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
EBaCorePrio BaProcGetOwnPrio() {
   // In windows it is only a stub
#ifdef __WIN32
   return eBaCorePrio_Normal;
#else
   struct sched_param schedPrio;
   sched_getparam(0, &schedPrio);
   return prioFromPrio(schedPrio.__sched_priority);
#endif

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

// todo: this function is double think about a solution
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
