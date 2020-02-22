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
#include "HwGpio.h"

#define TAG "AUTOM"
#define APPL_NAME "AuTomato PI"

//
static TOsProcCtrlTaskStats taskStats = {0};
static IHwGpio* pGpio23 = 0;

//
TBaBoolRC ApplInit(void *pArg) {
   TOsProcCtrlTaskOpts* pOpts = static_cast<TOsProcCtrlTaskOpts*>(pArg);

   if (!pOpts || !pOpts->name)
   {
      ERROR_("No app name");
   }

   TRACE_("========================");
   TRACE_(pOpts->name);
   TRACE_("========================");

   pGpio23 = IHwGpioCreate(23);
   if (!pGpio23) {
      // Try again, it might have been dirty
      pGpio23 = IHwGpioCreate(23);

      if (!pGpio23) {
         return eBaBoolRC_Error;
      }
   }

   if (!pGpio23->SetInp()) {
            return eBaBoolRC_Error;
   }

   TRACE_("Init successful");
   return eBaBoolRC_Success;
}

//
void ApplUpd(void *pArg) {
   OsProcGetCtrlTaskStats(&taskStats);
   TRACE_("Log %llu, GPIO_23: %i", taskStats.updCnt, pGpio23->Get());
}

//
TBaBoolRC ApplExit(void *pArg) {
   TRACE_("Exit");
   return eBaBoolRC_Success;
}




