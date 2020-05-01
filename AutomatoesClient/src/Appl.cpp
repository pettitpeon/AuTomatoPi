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
#include "SensAds1115.h"
#include "OsIpc.h"

#include <linux/i2c-dev.h> // I2C bus definitions
#include <fcntl.h>     // open
#include <unistd.h>    // read/write usleep
#include <sys/ioctl.h>
#include <byteswap.h>

#define TAG "AUTOM"
#define APPL_NAME "AuTomato PI"

//
namespace {
TOsProcCtrlTaskStats taskStats = {0};
}

//
TBaBoolRC ApplInit(void *pArg) {
   TOsProcCtrlTaskOpts* pOpts = static_cast<TOsProcCtrlTaskOpts*>(pArg);

   if (!pOpts || !pOpts->name)
   {
      ERROR_("No app name");
   }

   if (!OsIpcInitClnt())
   {
      ERROR_("OsIpcInitClnt");
      return eBaBoolRC_Error;
   }

   TRACE_("========================");
   TRACE_(pOpts->name);
   TRACE_("========================");


   TRACE_("Init successful");
   return eBaBoolRC_Success;
}

//
void ApplUpd(void *pArg) {
   OsProcGetCtrlThreadStats(&taskStats);

   TOsIpcRegVarOut var{};
   TBaBoolRC rc = OsIpcCallVar("last_conversion", &var);

   TRACE_("Log %3llu, (%i)Value: %4.3f V." ,taskStats.updCnt, rc, var.dat.f);

   BaApiFlushLog();

}

//
TBaBoolRC ApplExit(void *pArg) {
   TRACE_("Exit");
   return eBaBoolRC_Success;
}




