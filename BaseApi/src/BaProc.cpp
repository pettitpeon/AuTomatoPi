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
#define CTRLPIDFILE PIDPATH CTRLTASK
#define SHLEN BAPROC_SHORTNAMELEN
#define FULLLEN BAPROC_FULLNAMELEN

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
LOCAL std::string getPIDName(pid_t pid);

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
      return eBaBool_true;
   }
   // Get the PID from the PID file
   pid_t pid = BaProcReadPidFile(progName, eBaBool_true);

   // Check if I am myself
   if ((pid < 0) || (pid == getpid())) {
      // OK, I can overwrite my own file
      return eBaBool_true;
   }

   // Fake kill
#ifdef __WIN32
   return eBaBool_false;
#else

   // Check if the process from the PID is running
   if (kill(pid, 0) && errno == ESRCH) {
      // Orocess not running, can overwrite file
      return eBaBool_true;
   }
#endif

   // Process in PID is running
   // Open proc file from PID: /proc/[PID]/comm

   // Read the executable name and compare it with the given name
   if (getPIDName(pid) != progName) {
      return eBaBool_true;
   }

   // There is another process called progName running
   return eBaBool_false;
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

