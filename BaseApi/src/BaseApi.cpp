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

//
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

   // Deactivate the log so no one can use it anymore.
   CBaLog *tpLog = spLog;
   spLog = 0;

   // This acquires and releases the internal log mutex thereby ensuring that
   // after releasing it, it was the last entry with BaseApi.
   tpLog->Log(eBaLogPrio_Trace, TAG, "Exit BaApiLog");

   // Only destroy the logger if you created it. NEVER mess with data owned by
   // the user
   bool ret = sExtLogger ? true : CBaLog::Destroy(tpLog);
   sExtLogger = false;
   return ret ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaApiLogF(EBaLogPrio prio, const char* tag, const char* fmt, ...) {
   if (!spLog || !fmt) {
      return eBaBoolRC_Error;
   }

   va_list arg;
   va_start(arg, fmt);

   // Check again that the logger exists.
   TBaBoolRC ret = spLog ? spLog->LogV(prio, tag, fmt, arg) : eBaBoolRC_Error;
   va_end(arg);
   return ret;
}




