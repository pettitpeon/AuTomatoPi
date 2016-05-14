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
#include "BaseApi.h"
#include "CBaLog.h"
#include "BaGenMacros.h"

/*------------------------------------------------------------------------------
 *  Defines
 */


/*------------------------------------------------------------------------------
 *  Local functions
 */
LOCAL void ctrlTaskRout(TBaCoreThreadArg* pArg);

/*------------------------------------------------------------------------------
 *  Static variables
 */
static CBaLog *spLog = 0;

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
TBaBoolRC BaApiInitLogger(TBaLogHdl hdl) {
   if (spLog) {
      return eBaBoolRC_Success;
   } else if (!hdl) {
      return eBaBoolRC_Error;
   }
   spLog = dynamic_cast<CBaLog*>((IBaLog*)hdl);
   return spLog ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaApiExitLogger() {
   if (!spLog) {
      return eBaBoolRC_Success;
   }

   bool ret = CBaLog::Destroy(spLog);
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

void BaApiStartCtrlTask(TBaApiCtrlTaskOpts* pOpts) {
   if (!pOpts) {
      return;
   }

   if (!pOpts->init() || !pOpts->start()) {
      return;
   }

   TBaCoreThreadHdl hdl = BaCoreCreateThread(pOpts->name, ctrlTaskRout, 0, pOpts->prio);
}

LOCAL void ctrlTaskRout(TBaCoreThreadArg* pArg) {
   if (!pArg || !pArg->pArg) {
      return;
   }

   TBaApiCtrlTaskOpts* pOpts = (TBaApiCtrlTaskOpts*) pArg->pArg;
   void (* update  )(void*) = pOpts->update;
   while (!pArg->exitTh) {
      uint64_t duration = BaCoreTimedUs(update, 0);
      if (duration + 9 > pOpts->cyleTimeMs) {
         BaCoreMSleep(10);
      } else {
         BaCoreMSleep(pOpts->cyleTimeMs - duration);
      }
   }
}
