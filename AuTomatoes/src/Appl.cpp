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
constexpr auto LEDsCNT = 4;
IHwGpio* pGpios23_25[LEDsCNT] = {0, 0, 0, 0};
constexpr int GPIOS_23_26[LEDsCNT] = {23, 24, 25, 26};
constexpr int ADS1115_ADR = 0x48;
constexpr int ADS1115_CONV = 0;
constexpr int ADS1115_CONF = 1;
constexpr float CAPT_TO_VOLT = 6.144/32767.0;

SensAds1115 ADC(ADS1115_ADR, SensAds1115::Mode::Continuous, SensAds1115::SampsRate::SR128);

float LAST_CONVERSION = 0;
}

//
TBaBoolRC ApplInit(void *pArg) {
   TOsProcCtrlTaskOpts* pOpts = static_cast<TOsProcCtrlTaskOpts*>(pArg);

   if (!pOpts || !pOpts->name)
   {
      ERROR_("No app name");
   }

   if (!OsIpcInitSvr())
   {
      ERROR_("OsIpcInitSvr");
      return eBaBoolRC_Error;
   }

   if (!OsIpcRegistryLocalInit())
   {
      OsIpcExitSvr();
      ERROR_("OsIpcRegistryLocalInit");
      return eBaBoolRC_Error;
   }

   TOsIpcRegVar var = {&LAST_CONVERSION, sizeof(LAST_CONVERSION), false};

   if (!OsIpcRegistryLocalRegisterVar("last_conversion", &var))
   {
      OsIpcRegistryLocalExit();
      OsIpcExitSvr();
      ERROR_("OsIpcRegistryLocalRegisterVar");
      return eBaBoolRC_Error;
   }

   TRACE_("========================");
   TRACE_(pOpts->name);
   TRACE_("========================");

   for (int i = 0; i < LEDsCNT; ++i)
   {
      pGpios23_25[i] = IHwGpioCreate(GPIOS_23_26[i]);
      if (!pGpios23_25[i]) {
         return eBaBoolRC_Error;
      }

      if (!pGpios23_25[i]->SetOut()) {
         return eBaBoolRC_Error;
      }

      pGpios23_25[i]->Set();
   }

   ADC.Init();

   TRACE_("Init successful");
   return eBaBoolRC_Success;
}

//
void ApplUpd(void *pArg) {
   OsProcGetCtrlThreadStats(&taskStats);
   TBaBool err = eBaBool_false;
   LAST_CONVERSION = ADC.Capture(SensAds1115::AnInput::AnIn1, SensAds1115::Gain::Max6_144V, &err);

   if (err) {
      TRACE_("Read conversion failed");
   }

   for (int i = 0; i < LEDsCNT; ++i)
   {
      if (LAST_CONVERSION >= i + 1) {
         pGpios23_25[i]->Reset();
      }
      else {
         pGpios23_25[i]->Set();
      }
   }

   TRACE_("Log %3llu, Value: %4.3f V." ,taskStats.updCnt, LAST_CONVERSION);

   BaApiFlushLog();

}

//
TBaBoolRC ApplExit(void *pArg) {
   TRACE_("Exit");
   return eBaBoolRC_Success;
}




