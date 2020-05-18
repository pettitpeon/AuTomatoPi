/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : SensAds1115.h
 *   Date     : Aug 11, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  First order delay element PT1
 */
/*------------------------------------------------------------------------------
 */
#ifndef SENSADS1115_H_
#define SENSADS1115_H_

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include "BaBool.h"

#include <stdint.h>
#include <map>

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/


#ifdef __cplusplus
/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

class SensAds1115 {
public:
   enum class SampsRate : uint8_t {
      SR8 = 0,
      SR16,
      SR32,
      SR64,
      SR128,
      SR250,
      SR475,
      SR860
   };

   enum class Gain : uint8_t {
      Max6_144V = 0,
      Max4_096V,
      Max2_048V,
      Max1_024V,
      Max0_512V,
      Max0_256V
   };

   enum class Mode : uint8_t {
      Continuous = 0,
      SingleShot
   };

   enum class AnInput : uint8_t {
      AnIn1 = 0,
      AnIn2,
      AnIn3,
      AnIn4
   };

   static const uint16_t ADS1115_ADR = 0x48;
   SensAds1115(uint16_t devAddr, Mode mode, SampsRate mSampsRate);

   bool Init();
   TBaBoolRC Config(AnInput in, Gain gain);
   float ConfigCapture(AnInput in, Gain gain, TBaBool *pError);
   float Capture(Gain gain, TBaBool *pError);

   // Typical object oriented destructor must be virtual!
   virtual ~SensAds1115();

private:
   uint16_t mAddr;
   Mode mMode;
   SampsRate mSampsRate;
};


#endif // __cplusplus
#endif // SENSADS1115_H_
