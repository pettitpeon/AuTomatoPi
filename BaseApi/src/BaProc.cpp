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
#include <string>
#include <fstream> // std::ifstream

#include "BaProc.h"
#include "BaUtils.hpp"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#if __WIN32
# define PIDPATH "\\var\\run\\BaseApi\\"
#else
# define PIDPATH "/var/run/BaseApi/"
#endif
#define CTRLTASK "BaseApiCtrlTask"

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/

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
      GetModuleFileName(NULL, szFileName, MAX_PATH );
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
   static char sShortOwnName[BAPROC_SHORTNAMELEN] = {0};
   if (!sShortOwnName[0]) {
      strncpy(sShortOwnName, BaProcGetOwnFullName(), BAPROC_SHORTNAMELEN);
   }

   return sShortOwnName;
}


//
pid_t BaProcReadPidFile(const char *progName, TBaBool internal) {
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
TBaBoolRC BaProcTestPidFile(const char *progName) {
   if (!progName) {
      return eBaBoolRC_Success;
   }
   std::string nameOfPID;

   // Get the PID from the PID file
   pid_t pid = BaProcReadPidFile(progName, eBaBool_true);

   // Check if I am myself
   if ((pid < 0) || (pid == getpid())) {
      // OK, I can overwrite my own file
      return eBaBoolRC_Success;
   }

   // Fake kill
#ifdef __WIN32

#else

   // Check if the process from the PID is running
   if (kill(pid, 0) && errno == ESRCH) {
      // process not running, can overwrite file
      return eBaBoolRC_Success;
   }
#endif

   // Process in PID is running
   // Open proc file from PID: /proc/[PID]/comm
   std::ifstream commFile("/proc/" + std::to_string(pid) + "/comm");
   if(commFile.fail()) {
      return eBaBoolRC_Success;
   }

   // Read the executable name and compare it with the given name
   std::getline(commFile, nameOfPID);
   if (nameOfPID != progName) {
      return eBaBoolRC_Success;
   }

   // There is another process called progName running
   return eBaBoolRC_Error;
}

//
TBaBoolRC BaProcWritePidFile(const char *progName) {
   if (!progName) {
      return eBaBoolRC_Error;
   }

   if (!BaFS::Exists(PIDPATH)) {
      BaFS::MkDir(PIDPATH);
   }

   std::string pidfile = PIDPATH;
   pidfile.append(CTRLTASK);
   std::ofstream myfile (pidfile);

   if (!myfile.is_open()) {
      // todo: log?
      return eBaBoolRC_Error;
   }

   pid_t pid = getpid();
   myfile << pid << std::endl;
   myfile << progName << std::endl;
   myfile.close();
   return myfile.fail() ? eBaBoolRC_Error : eBaBoolRC_Success;
}

//
TBaBoolRC BaProcRemovePidFile(const char *progName) {
   if (!progName) {
      return eBaBoolRC_Error;
   }
   std::string pidfile = PIDPATH;
   pidfile.append(progName);
   return unlink(pidfile.c_str()) == 0 ? eBaBoolRC_Success : eBaBoolRC_Error;
}

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
