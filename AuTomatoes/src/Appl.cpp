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
IHwGpio* pGpio23 = 0;
constexpr int ADS1115_ADR = 0x48;
constexpr int GPIO_23 = 23;
constexpr int ADS1115_CONV = 0;
constexpr int ADS1115_CONF = 1;
constexpr float CAPT_TO_VOLT = 6.144/32767.0;

SensAds1115 ADC(ADS1115_ADR, SensAds1115::Mode::Continuous, SensAds1115::SampsRate::SR128);
}

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

   pGpio23 = IHwGpioCreate(GPIO_23);
   if (!pGpio23) {
      return eBaBoolRC_Error;
   }

   if (!pGpio23->SetInp()) {
      return eBaBoolRC_Error;
   }

   ADC.Init();

   TRACE_("Init successful");
   return eBaBoolRC_Success;
}

//
void ApplUpd(void *pArg) {
   OsProcGetCtrlThreadStats(&taskStats);
   TBaBool err = eBaBool_false;
   float volts = ADC.Capture(SensAds1115::AnInput::AnIn1, SensAds1115::Gain::Max6_144V, &err);

   if (err) {
      TRACE_("Read conversion failed");
   }

   TRACE_("Log % 3llu, GPIO_23: %i, Value: %4.3f V."
            , taskStats.updCnt, pGpio23->Get(), volts);

}

//
TBaBoolRC ApplExit(void *pArg) {
   TRACE_("Exit");
   return eBaBoolRC_Success;
}




