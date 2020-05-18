/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : SensAds1115.cpp
 *   Date     : Aug 11, 2016
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include "SensAds1115.h"
#include "HwCom.h"
#include "BaUtils.hpp"
#include "BaLogMacros.h"

#include <array>
#include <byteswap.h>


/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define TAG "HwCom"

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/
namespace {
constexpr int ADS1115_CONV = 0;
constexpr int ADS1115_CONF = 1;
constexpr float CAPT_TO_VOLT = 6.144/32767.0;
constexpr auto SZ = BaToUnderlying(SensAds1115::Gain::Max0_256V) + 1;
constexpr std::array<float, SZ> CAP_TO_VOLT = {
      6.144/32767.0,
      4.096/32767.0, // todo: I think is wrong (one more bit)
      2.048/32767.0,
      1.024/32767.0,
      0.512/32767.0,
      0.256/32767.0,
};

}

SensAds1115::SensAds1115(uint16_t devAddr, Mode mode, SampsRate sampsRate) :
      mAddr(devAddr),
      mMode(mode),
      mSampsRate(sampsRate)
{
   /*
000 : FSR = ±6.144 V
001 : FSR = ±4.096 V
010 : FSR = ±2.048 V (default)
011 : FSR = ±1.024 V
100 : FSR = ±0.512 V
101 : FSR = ±0.256 V
110 : FSR = ±0.256 V
111 : FSR = ±0.256 V
    */
}

bool SensAds1115::Init() {
   if (!HwComI2CInit()) {
      return false;
   }

   if (!HwComI2CSelectDev(mAddr)) {
      return false;
   }

   return true;
}

TBaBoolRC SensAds1115::Config(AnInput in, Gain gain) {
   //        0x83c0         = 0b1000'0011'1100'0000;
   //  5 Data rate -128-samps/s-^^^| |||| |||| ||||
   //  4 Comp Mode -Traditional----+ |||| |||| ||||
   //  3 Comp Pol -active-low--------+||| |||| ||||
   //  2 Comp Lat -Non-latching-------+|| |||| ||||
   //  0 Comp Q --Comp-disabled--------++ |||| ||||
   // 15 Op Status -Start-conversion------+||| ||||
   // 14 Mux -In0-to-Gnd-------------------+++ ||||
   //  9 Gain -6.144V--------------------------+++|
   //  8 Op mode -Continuous-conversion-----------+

   uint8_t sampsRateAndComp = (BaToUnderlying(mSampsRate) << 5u) | 0b00011;
   uint16_t opts = 0b10000000; // start capture
   opts |= (BaToUnderlying(in) + 4) << 4u; // ADC Input
   opts |= BaToUnderlying(gain) << 1u; // Gain
   opts |= BaToUnderlying(mMode); // continuous or single shot

   opts |= (sampsRateAndComp << 8u); // Combine both bytes

   TRACE_("input %i", (uint16_t)((BaToUnderlying(in) + 4)));

   // Todo: lock interface?
   if (!HwComI2CSelectDev(mAddr) || !HwComI2CWriteReg16(ADS1115_CONF, opts)) {
      return eBaBoolRC_Error;
   }

   return eBaBoolRC_Success;
}

float SensAds1115::ConfigCapture(AnInput in, Gain gain, TBaBool *pError) {
   if (!Config(in, gain)) {
      if (pError) {
         *pError = eBaBool_true;
      }

      return 0;
   }

   // Gotta sleep between writing config and reading capture
   BaCoreMSleep(15);
   return Capture(gain, pError);
}

float SensAds1115::Capture(Gain gain, TBaBool *pError) {
   return (int16_t)bswap_16(HwComI2CReadReg16(ADS1115_CONV, pError)) * CAP_TO_VOLT[BaToUnderlying(gain)];
}

SensAds1115::~SensAds1115() {
   HwComI2CExit();
}

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
