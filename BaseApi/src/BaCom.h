/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaCom.h
 *   Date     : Nov 30, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  Communications API
 *   - I2C
 *   - SPI
 *   - Serial
 */
/*------------------------------------------------------------------------------
 */
#ifndef SRC_BACOM_H_
#define SRC_BACOM_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "BaBool.h"

/// Serial device path
#define BACOM_SERIALDEV "/dev/ttyAMA0"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
typedef int32_t TBaComHdl;
typedef void* TBaComSerHdl;

/// Baud enumeration
typedef enum EBaComBaud {
   eBaComBaud_50 = 0,
   eBaComBaud_75,
   eBaComBaud_110,
   eBaComBaud_134,
   eBaComBaud_150,
   eBaComBaud_200,
   eBaComBaud_300,
   eBaComBaud_600,
   eBaComBaud_1200,
   eBaComBaud_1800,
   eBaComBaud_2400,
   eBaComBaud_9600,
   eBaComBaud_19200,
   eBaComBaud_38400,
   eBaComBaud_57600,
   eBaComBaud_115200,
   eBaComBaud_230400
} EBaComBaud;

/*------------------------------------------------------------------------------
 *  C interface
 */
#ifdef __cplusplus
extern "C" {
#endif

// Stubs!
TBaComHdl BaComI2CInit();
TBaBoolRC BaComI2CExit(TBaComHdl hdl);

TBaComHdl BaComSPIInit();
TBaBoolRC BaComSPIExit(TBaComHdl hdl);
// end stubs!


/// @name Init/Exit Functions
//@{

/******************************************************************************/
/** Initializes the resources and reserves GPIOs 14 and 15 for
 *  @return Handle on success, otherwise error
 */
TBaBoolRC BaCom1WInit();

/******************************************************************************/
/** ...
 *  @return
 */
TBaBoolRC BaCom1WExit();

/******************************************************************************/
/** ...
 *  @return
 */
uint16_t BaCom1WGetDevices();

/******************************************************************************/
/** ...
 *  @return
 */
TBaBoolRC BaCom1WGetTemp(float *pTemp);

/******************************************************************************/
/** Initializes the resources and reserves GPIOs 14 and 15 for
 *  @return Handle on success, otherwise error
 */
TBaComSerHdl BaComSerInit(
      const char *device, ///< Device path, e.g. BACOM_SERIALDEV
      EBaComBaud baud     ///< Baud rate
      );

/******************************************************************************/
/** Frees resources
 *  @return Error or success
 */
TBaBoolRC BaComSerExit(
      TBaComSerHdl hdl  ///< Serial communication handle
      );
//@} Init/Exit Functions


TBaBoolRC BaComSerPutC(
      TBaComSerHdl hdl, uint8_t c
      );

int BaComSerPend(
      TBaComSerHdl hdl
      );

uint8_t BaComSerGetC(
      TBaComSerHdl hdl
      );


#ifdef __cplusplus
} // extern c


/*------------------------------------------------------------------------------
 *  C++ Interface
 */
#endif // __cplusplus
#endif /* SRC_BACOM_H_ */
