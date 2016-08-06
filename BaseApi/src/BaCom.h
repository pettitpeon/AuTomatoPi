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
 *   - One Wire Bus
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
typedef int32_t TBaComHdl; ///< BaCom Handle
typedef void* TBaComSerHdl; ///< Serial interface handle

/// Callback function to parse the string returned by the driver from
/// /sys/bus/w1/devices/<SerialNo>/w1_slave
typedef void* (*TBaCom1wReadFun)(
      const char* str, /**< [in] Contents of driver file. Note: it gets
      destroyed after leaving the function, so do not use it outside this cb */
      size_t n ///< [in] String length
      );

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


/// @name One Wire bus
//@{
/******************************************************************************/
/** Initializes the resources, reserves GPIO 4 and calls #BaCom1WGetDevices()
 *  @return Error of success
 */
TBaBoolRC BaCom1WInit();

/******************************************************************************/
/** Releases the resources
 *  @return Error of success
 */
TBaBoolRC BaCom1WExit();

/******************************************************************************/
/** Scans for devices and saves them internally. This is automatically called
 *  by #BaCom1WInit()
 *  @return the number of devices found
 */
uint16_t BaCom1WGetDevices();

/******************************************************************************/
/** Read the value of the 1W sensor asynchronously
 *  @return On success, the contents of the sensor file, otherwise 0
 */
const char* BaCom1WRdAsync(
      const char* serNo ///< [in] Serial number of the sensor
      );

///******************************************************************************/
///** Stop the asynchronous thread of 1W devices
// *  @return Error of success
// */
//TBaBoolRC BaCom1WPauseAsyncThread(
//      const char* serNo ///< [in] Serial number of the sensor
//      );

/******************************************************************************/
/** Gets the temperature from the sensor. This is a slow synchronous read. It
 *  takes about 1s. For soft real-time applications, the asynchronous call is
 *  suggested.
 *  @return Temperature in °C on success, otherwise -300
 */
float BaCom1WGetTemp(
      const char* serNo, /**< [in] Optional serial number of the sensor eg:
       "28-0215c2c4bcff". If null, the first sensor with family ID 28 is used*/
      TBaBool *pError ///< [out] Optional error flag. Only modified if error
      );

/******************************************************************************/
/** Gets the data from a generic one wire device by calling user callback.
 *  todo: return the first sensor if serNO == 1
 *  @return Null if error, or the data returned by @c cb
 */
void* BaCom1WGetValue(
      const char* serNo, /**< [in] Optional serial number of the sensor eg:
         "28-0215c2c4bcff".*/
      TBaCom1wReadFun cb, /**< [in] Callback function that parses the string
         returned by the driver */
      TBaBool *pError ///< [out] Optional error flag. Only modified if error
      );
//@} One Wire Bus

/// @name Serial Interface
//@{
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


TBaBoolRC BaComSerPutC(
      TBaComSerHdl hdl, uint8_t c
      );

int BaComSerPend(
      TBaComSerHdl hdl
      );

uint8_t BaComSerGetC(
      TBaComSerHdl hdl
      );
//@} Serial Interface

#ifdef __cplusplus
} // extern c

#endif // __cplusplus
#endif /* SRC_BACOM_H_ */
