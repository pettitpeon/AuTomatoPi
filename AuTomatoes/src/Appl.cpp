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
#include "HwCom.h"

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
int fd;
constexpr int ADS1115_ADR = 0x48;
constexpr int GPIO_23 = 23;
constexpr int ADS1115_CONV = 0;
constexpr int ADS1115_CONF = 1;
constexpr float CAPT_TO_VOLT = 6.144/32767.0;
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

   if (!HwComI2CInit()) {
      TRACE_("Couldn't init I2C");
      HwComI2CExit();
      return eBaBoolRC_Error;
   }

   if (!HwComI2CSelectDev(ADS1115_ADR))    {
      TRACE_("Couldn't select device");
      return eBaBoolRC_Error;
   }

   const uint16_t confReg   = 0b1000'0011'1100'0000; // 0x83c0
   //  5 Data rate -128-samps/s-^^^| |||| |||| ||||
   //  4 Comp Mode -Traditional----+ |||| |||| ||||
   //  3 Comp Pol -active-low--------+||| |||| ||||
   //  2 Comp Lat -Non-latching-------+|| |||| ||||
   //  0 Comp Q --Comp-disabled--------++ |||| ||||
   // 15 Op Status -Start-conversion------+||| ||||
   // 14 Mux -In0-to-Gnd-------------------+++ ||||
   //  9 Gain -6.144V--------------------------+++|
   //  8 Op mode -Continuous-conversion-----------+
   if (!HwComI2CWriteReg16(ADS1115_CONF, confReg)) {
      TRACE_("Write error config setup");
   }

   TRACE_("Init successful");
   return eBaBoolRC_Success;
}

//
void ApplUpd(void *pArg) {
   OsProcGetCtrlThreadStats(&taskStats);
   TBaBool err = eBaBool_false;

   auto conversion = bswap_16(HwComI2CReadReg16(ADS1115_CONV, &err));
   if (err) {
      TRACE_("Read conversion failed");
   }

   float volts = conversion * CAPT_TO_VOLT;
   TRACE_("Log % 3llu, GPIO_23: %i, Values: 0x%02x %d  %4.3f V."
            , taskStats.updCnt, pGpio23->Get(), conversion, conversion, volts);

}

//
TBaBoolRC ApplExit(void *pArg) {
   TRACE_("Exit");
   return eBaBoolRC_Success;
}




