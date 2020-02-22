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

#define TAG "APPL"

//
static TOsProcCtrlTaskStats taskStats = {0};
static IHwGpio* pGpio23 = 0;

TBaBoolRC ApplInit(void *pArg) {
   TRACE_("Init");
   pGpio23 = IHwGpioCreate(23);
   TRACE_("Init: %p", pGpio23);
   pGpio23->SetInp();
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




