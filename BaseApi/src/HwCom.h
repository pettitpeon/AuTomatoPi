/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : HwCom.h
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
#ifndef HWCOM_H_
#define HWCOM_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "HwPi.h"
#include "BaBool.h"
#include "BaCore.h"

/// Serial device path
#define HWCOM_SERIALDEV "/dev/ttyAMA0"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
typedef int32_t THwComHdl; ///< HwCom Handle
typedef void* THwComSerHdl; ///< Serial interface handle

/// Callback function to parse the string returned by the driver from
/// /sys/bus/w1/devices/<SerialNo>/w1_slave
typedef void* (*THwCom1wReadFun)(
      const char* str, /**< [in] Contents of driver file. Note: it gets
      destroyed after leaving the function, so do not use it outside this cb */
      size_t n ///< [in] String length
      );

/// Baud enumeration
typedef enum EHwComBaud {
   eHwComBaud_50 = 0,
   eHwComBaud_75,
   eHwComBaud_110,
   eHwComBaud_134,
   eHwComBaud_150,
   eHwComBaud_200,
   eHwComBaud_300,
   eHwComBaud_600,
   eHwComBaud_1200,
   eHwComBaud_1800,
   eHwComBaud_2400,
   eHwComBaud_9600,
   eHwComBaud_19200,
   eHwComBaud_38400,
   eHwComBaud_57600,
   eHwComBaud_115200,
   eHwComBaud_230400
} EHwComBaud;

/*------------------------------------------------------------------------------
 *  C interface
 */
#ifdef __cplusplus
extern "C" {
#endif

/// @name I2C bus Management
//@{

/******************************************************************************/
/** Initializes the resources, reserves GPIOs SDA (2), SCL (3). todo: ID_SC, ID_SD
 *  @return Error of success
 */
TBaBoolRC HwComI2CInit();

/******************************************************************************/
/** Releases the resources
 *  @return Error or success
 */
TBaBoolRC HwComI2CExit();

/******************************************************************************/
/** Select a device to work with and initializes the bus if not initialized
 *  @return Error of success
 */
TBaBoolRC HwComI2CSelectDev(
      uint16_t devAddr ///< [in] Device address. see todo
      );

/******************************************************************************/
/** Gets the functionality of the device as described in linux/i2c.h
 *  @return Mask of functions
 */
uint64_t HwComI2CFuncs();
//@} i2c bus management

/// @name I2C bus Read
//@{
/******************************************************************************/
/** Simple 8-bit read
 *  @return Data
 */
uint8_t HwComI2CRead8(
      TBaBool *pError ///< [out] Optional error flag
      );

/******************************************************************************/
/** Read 8-bit register
 *  @return Data
 */
uint8_t HwComI2CReadReg8(
      uint32_t reg, ///< [in] Register number
      TBaBool *pError ///< [out] Optional error flag
      );

/******************************************************************************/
/** Read 16-bit register
 *  @return Data
 */
uint16_t HwComI2CReadReg16(
      uint32_t reg, ///< [in] Register number
      TBaBool *pError ///< [out] Optional error flag
      );
//@} i2c bus read

/// @name I2C bus write
//@{
/******************************************************************************/
/** Simple 8-bit Write
 *  @return Error or success
 */
TBaBoolRC HwComI2CWrite8(
      uint8_t val ///< [in] value to write
      );

/******************************************************************************/
/** Write 8-bit register
 *  @return Error or success
 */
TBaBoolRC HwComI2CWriteReg8(
      uint32_t reg, ///< [in] Register number
      uint8_t val ///< [in] value to write
      );

/******************************************************************************/
/** Write 16-bit register
 *  @return Error or success
 */
TBaBoolRC HwComI2CWriteReg16(
      uint32_t reg, ///< [in] Register number
      uint16_t val ///< [in] value to write
      );
//@} i2c bus write

THwComHdl HwComSPIInit();
TBaBoolRC HwComSPIExit(THwComHdl hdl);


/// @name One Wire bus
//@{
/******************************************************************************/
/** Initializes the resources, reserves GPIO 4 and calls #HwCom1WGetDevices()
 *  @return Error of success
 */
TBaBoolRC HwCom1WInit();

/******************************************************************************/
/** Releases the resources
 *  @return Error or success
 */
TBaBoolRC HwCom1WExit();

/******************************************************************************/
/** Scans for devices and saves them internally. This is automatically called
 *  by #HwCom1WInit()
 *  @return The number of devices found
 */
uint16_t HwCom1WGetDevices();

/******************************************************************************/
/** Read the value of the 1W sensor asynchronously. This works with an internal
 *  worker thread. The @c serNo must have the form:@n
 *  XX-XXXXXXXXXXXX [devFam]-[devID]
 *  @return On success, the contents of the sensor file, otherwise 0
 */
const char* HwCom1WRdAsync(
      const char* serNo, /**< [in] Optional serial number of the sensor eg:
       "28-0215c2c4bcff". If null, the first sensor with family ID 28 is used*/
      TBaCoreMonTStampUs *pTs ///< [out] optional timestamp of the reading
      );

/******************************************************************************/
/** Gets the temperature from the sensor with family "28". This is a slow
 *  synchronous read. It takes about 1s. For soft real-time applications,
 *  the asynchronous call is suggested. The @c serNo must have the form:@n
 *  XX-XXXXXXXXXXXX [devFam]-[devID]
 *  @return Temperature in �C on success, otherwise -300
 */
float HwCom1WGetTemp(
      const char* serNo, /**< [in] Optional serial number of the sensor eg:
       "28-0215c2c4bcff". If null, the first sensor with family ID 28 is used*/
      TBaBool *pError ///< [out] Optional error flag. Only modified if error
      );

/******************************************************************************/
/** Gets the data from a generic one wire device by calling user callback.
 *  todo: return the first sensor if serNO == 1. The @c serNo must have the form:@n
 *  XX-XXXXXXXXXXXX [devFam]-[devID]
 *  @return Null if error, or the data returned by @c cb
 */
void* HwCom1WGetValue(
      const char* serNo, /**< [in] Optional serial number of the sensor eg:
         "28-0215c2c4bcff".*/
      THwCom1wReadFun cb, /**< [in] Callback function that parses the string
         returned by the driver*/
      TBaBool *pError ///< [out] Optional error flag. Only modified if error
      );
//@} One Wire Bus

/// @name Serial Interface
//@{
/******************************************************************************/
/** Initializes the resources and reserves GPIOs 14 and 15 for
 *  @return Handle on success, otherwise error
 */
THwComSerHdl HwComSerInit(
      const char *device, ///< Device path, e.g. HWCOM_SERIALDEV
      EHwComBaud baud     ///< Baud rate
      );

/******************************************************************************/
/** Frees resources
 *  @return Error or success
 */
TBaBoolRC HwComSerExit(
      THwComSerHdl hdl  ///< Serial communication handle
      );


TBaBoolRC HwComSerPutC(
      THwComSerHdl hdl, uint8_t c
      );

int HwComSerPend(
      THwComSerHdl hdl
      );

uint8_t HwComSerGetC(
      THwComSerHdl hdl
      );
//@} Serial Interface

#ifdef __cplusplus
} // extern c

#endif // __cplusplus
#endif /* HWCOM_H_ */
