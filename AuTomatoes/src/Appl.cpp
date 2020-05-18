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

#include "BaCore.h"
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
#include <array>
#include <cmath>

#define TAG "AUTOM"
#define APPL_NAME "AuTomato PI"

//
using TGpioPair = std::pair<IHwGpio*, int>;
using TBcdGpio = std::array<TGpioPair, 4>;

namespace {
TOsProcCtrlTaskStats taskStats = {0};

//                                      A               B               C                   D
TBcdGpio BCD_GPIOs = {TGpioPair{0, 06}, TGpioPair{0, 13}, TGpioPair{0, 19}, TGpioPair{0, 26}};
std::array<TGpioPair, 2> DIGITS = {TGpioPair{0, 21}, TGpioPair{0, 20}};
constexpr int ADS1115_ADR = 0x48;
constexpr int ADS1115_CONV = 0;
constexpr int ADS1115_CONF = 1;
constexpr float CAPT_TO_VOLT = 6.144/32767.0;

SensAds1115 ADC(ADS1115_ADR, SensAds1115::Mode::Continuous, SensAds1115::SampsRate::SR128);

float LAST_CONVERSION = 0;

TBaCoreThreadArg threadArg;
TBaCoreThreadHdl pThread = 0;

}

static void intToBcdGpio(uint8_t dec, TBcdGpio& gpios) {
   for (auto i = 0u; i < gpios.size(); ++i) {
      if(dec & (1 << i)) {
         gpios[i].first->Set();
      } else {
         gpios[i].first->Reset();
      }
   }
}

static std::pair<int8_t, uint8_t> floatTo2Digits(float in) {
   return std::make_pair((int8_t)fmod(in, 10), (uint8_t)std::lround(fmod(in*10, 10)));
}

static void routMultiplexFloat(TBaCoreThreadArg *pArg) {
   bool firstDigit = false;

   float &in = *(float*)pArg->pArg;
   while(!threadArg.exitTh) {
      auto digs = floatTo2Digits(in);
      if (firstDigit) {
         DIGITS[1].first->Reset();
         intToBcdGpio(digs.first, BCD_GPIOs);
         DIGITS[0].first->Set();
      } else {
         DIGITS[0].first->Reset();
         intToBcdGpio(digs.second, BCD_GPIOs);
         DIGITS[1].first->Set();
      }

      firstDigit = !firstDigit;
      BaCoreMSleep(1);
   }
}


//
TBaBoolRC ApplInit(void *pArg) {
   TOsProcCtrlTaskOpts* pOpts = static_cast<TOsProcCtrlTaskOpts*>(pArg);

   if (!pOpts || !pOpts->name) {
      ERROR_("No app name");
      return eBaBoolRC_Error;
   }

   if (!OsIpcInitSvr()) {
      ERROR_("OsIpcInitSvr");
      return eBaBoolRC_Error;
   }

   if (!OsIpcRegistryLocalInit()) {
      OsIpcExitSvr();
      ERROR_("OsIpcRegistryLocalInit");
      return eBaBoolRC_Error;
   }

   TOsIpcRegVar var= {&LAST_CONVERSION, sizeof(LAST_CONVERSION), false};
   if (!OsIpcRegistryLocalRegisterVar("last_conversion", &var)) {
      OsIpcRegistryLocalExit();
      OsIpcExitSvr();
      ERROR_("OsIpcRegistryLocalRegisterVar");
      return eBaBoolRC_Error;
   }

   TRACE_("========================");
   TRACE_(pOpts->name);
   TRACE_("========================");

   for (auto i = 0u; i < DIGITS.size(); ++i) {
      DIGITS[i].first = IHwGpioCreate(DIGITS[i].second);
      DIGITS[i].first->SetOut();
   }

   for (auto i = 0u; i < BCD_GPIOs.size(); ++i)
   {
      BCD_GPIOs[i].first = IHwGpioCreate(BCD_GPIOs[i].second);
      if (!BCD_GPIOs[i].first) {
         return eBaBoolRC_Error;
      }

      if (!BCD_GPIOs[i].first->SetOut()) {
         return eBaBoolRC_Error;
      }

      BCD_GPIOs[i].first->Set();
   }

   if (!ADC.Init() || !ADC.Config(SensAds1115::AnInput::AnIn1, SensAds1115::Gain::Max6_144V)) {
      return eBaBoolRC_Error;
   }

   threadArg.exitTh = eBaBool_false;
   threadArg.pArg = (void*)&LAST_CONVERSION;
   pThread = BaCoreCreateThread("LedMultp", routMultiplexFloat, &threadArg, eBaCorePrio_Low);

   TRACE_("Init successful");
   return eBaBoolRC_Success;
}

//
void ApplUpd(void *pArg) {
   OsProcGetCtrlThreadStats(&taskStats);
   TBaBool err = eBaBool_false;
   static bool toggle = false;
   toggle = !toggle;

   LAST_CONVERSION = ADC.Capture(SensAds1115::Gain::Max6_144V, &err);
   LAST_CONVERSION = rand() % 10 + (rand() % 10)/10.0;

   if (err) {
      TRACE_("Read conversion failed");
   }

   TRACE_("Log %3llu, Value: %4.3f V" ,taskStats.updCnt, LAST_CONVERSION);
   BaApiFlushLog();
}

//
TBaBoolRC ApplExit(void *pArg) {

   threadArg.exitTh = eBaBool_true;
   BaCoreDestroyThread(pThread, 100);

   TRACE_("Exit");
   return eBaBoolRC_Success;
}

