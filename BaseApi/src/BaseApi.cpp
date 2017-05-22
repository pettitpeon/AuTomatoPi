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

// Portability headers
#ifdef __linux

#elif __WIN32
# include <windows.h>
#endif

#include "BaseApi.h"
#include "CBaLog.h"
#include "BaGenMacros.h"
#include "BaLogMacros.h"

/*------------------------------------------------------------------------------
 *  Defines
 */
#define TAG      "BaApi"


/*------------------------------------------------------------------------------
 *  Type definitions
 */


/*------------------------------------------------------------------------------
 *  Local functions
 */


/*------------------------------------------------------------------------------
 *  Static variables
 */
static CBaLog *spLog = 0;
static bool   sExtLogger = false;

/*------------------------------------------------------------------------------
 *  C Interface
 */

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




