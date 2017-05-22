/*------------------------------------------------------------------------------
 *                             (c) 2017 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : Appl.cpp
 *   Date     : 22.05.2017
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  AuTomatoes application module
 */
/*------------------------------------------------------------------------------
 */

#include "BaLogMacros.h"
#include "BaGenMacros.h"
#include "OsProc.h"

#define TAG "APPL"

static TOsProcCtrlTaskStats taskStats = {0};
TBaBoolRC ApplInit(void *pArg) {
   TRACE_("Init");
   return eBaBoolRC_Success;
}

void ApplUpd(void *pArg) {
   OsProcGetCtrlTaskStats(&taskStats);
   TRACE_("Log %llu", taskStats.updCnt);

   if (taskStats.updCnt > 10) {
      OsProcStopCtrlTask();
   }

}

TBaBoolRC ApplExit(void *pArg) {
   TRACE_("Exit");
   return eBaBoolRC_Success;
}



