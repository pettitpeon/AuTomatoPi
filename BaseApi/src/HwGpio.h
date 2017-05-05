/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : HwGpio.h
 *   Date     : 23/06/2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  GPIOs APIs.
 *  C API:@n
 *  - The C API is thread-safe unless explicitly specified otherwise.
 *  - The C API has an initializations counter. Do not call the #HwGpioExit()
 *     function without having called #HwGpioInit() first.
 *  - The C API is not multi-process safe. Processes can overwrite each other
 *
 *  C++ API@n
 *  The object oriented C++ API is multiprocess safe between processes using
 *  this API
 */
/*------------------------------------------------------------------------------
 */

#ifndef HWGPIO_H_
#define HWGPIO_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "BaBool.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
/// GPIOs range from 1 to 27
typedef uint8_t THwGpio;

/// Software PWM handle
typedef void* THwGpioSWPWMHdl;

/// Forward declaration
class IHwGpio;

/// HW PWM mode flag
typedef enum EHwGpioHWPWMMode {
   EHwGpioHWPWMMode_MarkedSpace = 0, ///< Traditional PWM mode
   EHwGpioHWPWMMode_Balanced         ///< Default mode
} EHwGpioHWPWMMode;

/*------------------------------------------------------------------------------
 *  C interface
 */
#ifdef __cplusplus
extern "C" {
#endif

/// @name Init/Exit Functions
//@{
/******************************************************************************/
/** Initializes and frees resources
 *  @return Error or success
 */
TBaBoolRC HwGpioInit();
TBaBoolRC HwGpioExit();
//@} Init/Exit Functions

/// @name Hardware PWM setup functions
//@{
/******************************************************************************/
/** Initialize the hardware PWM functionality with default frequency 1000 Hz and
 *  resolution 256 steps.
 *  WARNING:   The RPI uses the PWM subsystem to produce audio. As such please
 *             refrain from playing audio on the RPI while this code is running
 */
TBaBoolRC HwGpioInitHWPWM();

/******************************************************************************/
/** Exit and release resources of the hardware PWM functionality
 *  @return Error or success
 */
TBaBoolRC HwGpioExitHWPWM();

/******************************************************************************/
/** Set the frequency and resolution of the hardware PWM.
 *  The clock frequency is 19.2 MHz
 *  The maximum resolution is 4095 steps (12-bit)
 *  Please choose these wisely and bare in mind that the actual frequency is
 *  an estimate because only positive clock frequency divisors can be used.
 *  freq_max @ min resolution (1-step) is 9.6 MHz
 *  freq_max @ max resolution (12-bit) is 2.34 kHz
 *  Examples:
 *  freq_max @ 1024-steps (10-bit) is 9.375 kHz
 *  freq_max @ 256-steps (8-bit) is 37.5 kHz
 *  Conditions:
 *  frequency * resolution * 2 < 19.2 MHz
 *  resolution <= 4095
 *  @return Error or success
 */
TBaBoolRC HwGpioSetClkHWPWM(
      double   frequency, ///< [in] desired frequency in Hz
      uint32_t resolution, ///< [in] desired resolution in steps
      EHwGpioHWPWMMode mode ///< [in] PWM mode
      );

//@} Hardware PWM setup functions


/// @name Basic GPIO operations
//@{
/******************************************************************************/
/** Cleans up the GPIO and leaves it on default state
 *  @return Error or success
 */
TBaBoolRC HwGpioCleanUp(
      THwGpio gpio ///< [in] GPIO number
      );

/******************************************************************************/
/** Sets GPIO as input
 *  @return Error or success
 */
TBaBoolRC HwGpioSetInp(
      THwGpio gpio ///< [in] GPIO number
      );

/******************************************************************************/
/** Sets GPIO as output
 *  @return Error or success
 */
TBaBoolRC HwGpioSetOut(
      THwGpio gpio ///< [in] GPIO number
      );

/******************************************************************************/
/** Sets GPIO as alternative function
 *  @return Error or success
 */
TBaBoolRC HwGpioSetAlt(
      THwGpio gpio, ///< [in] GPIO number
      int alt       ///< [in] Alternative function of GPIO
      );

/******************************************************************************/
/** Sets GPIO high. Not thread safe for performance reasons
 *  @return Error or success
 */
TBaBoolRC HwGpioSet(
      THwGpio gpio ///< [in] GPIO number
      );

/******************************************************************************/
/** Sets GPIO low. Not thread safe for performance reasons
 *  @return Error or success
 */
TBaBoolRC HwGpioReset(
      THwGpio gpio ///< [in] GPIO number
      );

/******************************************************************************/
/** Gets the GPIO state high/low.
 *  @return high true, or low false
 */
TBaBool HwGpioGet(
      THwGpio gpio ///< [in] GPIO number
      );
//@} Basic GPIO operations

/// @name PWM Functions
//@{

/******************************************************************************/
/** Start the hardware PWM. The #HwGpioInitHWPWM() function must be called
 *  before starting the HW PWM. Only GPIOs 12, 13, 18, and 19 can be rerouted to
 *   the PWM.
 *  @return Error or success
 */
TBaBoolRC HwGpioStartHWPWM(
      THwGpio gpio,     ///< [in] GPIO number
      float   dutyCycle ///< [in] Desired duty cycle [0..1]
      );

/******************************************************************************/
/** Reset the duty cycle of the hardware PWM. #HwGpioStartHWPWM() must be called
 *  before reseting the duty cycle. Not thread safe for performance reasons.
 *  @return Error or success
 */
TBaBoolRC HwGpioResetHWPWMDuCy(
      THwGpio gpio,     ///< [in] GPIO number
      float   dutyCycle ///< [in] Desired duty cycle [0..1]
      );

/******************************************************************************/
/** Stop the hardware PWM
 *  @return Error or success
 */
TBaBoolRC HwGpioStopHWPWM(
      THwGpio gpio ///< [in] GPIO number
      );

/******************************************************************************/
/** Start software PWM on the desired GPIO with normal soft real-time priority.
 *  The period must between 10 and 10000 ms. Input outside range will be
 *  limited. The faster the period, the more CPU usage.
 *  @return The software PWM handle if success, otherwise, null
 */
THwGpioSWPWMHdl HwGpioStartSWPWM(
      THwGpio gpio,     ///< [in] Desired GPIO
      float dutyC,      ///< [in] Initial duty cycle [0..1]
      uint16_t periodMs ///< [in] PWM period in ms [10..10000]
      );

/******************************************************************************/
/** Reset the duty cycle of the software PWM.
 *  @return Error or success
 */
TBaBoolRC HwGpioResetSWPWMDuCy(
      THwGpioSWPWMHdl hdl, ///< [in] Software PWM handle
      float dutyC          ///< [in] Desired duty cycle [0..1]
      );

/******************************************************************************/
/** Stop the software PWM
 *  @return Error or success
 */
TBaBoolRC HwGpioStopSWPWM(
      THwGpioSWPWMHdl hdl ///< [in] Software PWM handle
      );
//@} PWM Functions

/// @name C-Linkage object oriented factory
//@{
/******************************************************************************/
/** C++ interface object oriented factory
 *  @return Pointer to C++ interface if success, otherwise, error
 */
IHwGpio* IHwGpioCreate(
      THwGpio gpio ///< [in] GPIO number
      );

/******************************************************************************/
/** C++ interface object oriented destructor
 *  @return Error or success
 */
TBaBoolRC IHwGpioDelete(
      IHwGpio* pHdl ///< [in] Interface handle
      );
//@} C-Linkage OO factory

#ifdef __cplusplus
} // extern c


/*------------------------------------------------------------------------------
 *  C++ Interface
 */

/******************************************************************************/
/** C++ interface
 *  This is a C++ wrapper of the C API above adding multi process safety and a
 *  few extra functions. The multi process safety only works within processes
 *  using this interface. This means that two or more processes using this
 *  interface can only use each of the GPIOs one process at the time. If another
 *  process uses the C interface or accesses the device memory directly it will
 *  conflict with the process using this API.
 */
class IHwGpio {
public:
   /// @name Hardware PWM setup functions
   //@{
   /***************************************************************************/
   /** Initialize the hardware PWM functionality with default frequency 1000 Hz and
    *  resolution 256 steps.
    *  WARNING:   The RPI uses the PWM subsystem to produce audio. As such please
    *             refrain from playing audio on the RPI while this code is running
    *  @return Error or success
    */
   virtual bool InitHWPWM() = 0;

   /***************************************************************************/
   /** Exit and release resources of the hardware PWM functionality
    *  @return Error or success
    */
   virtual bool ExitHWPWM() = 0;

   /***************************************************************************/
   /** Set the frequency and resolution of the hardware PWM.
    *  The clock frequency is 19.2 MHz
    *  The maximum resolution is 4095 steps (12-bit)
    *  Please choose these wisely and bare in mind that the actual frequency is
    *  an estimate because only positive clock frequency divisors can be used.
    *  freq_max @ min resolution (1-step) is 9.6 MHz
    *  freq_max @ max resolution (12-bit) is 2.34 kHz
    *  Examples:
    *  freq_max @ 1024-steps (10-bit) is 9.375 kHz
    *  freq_max @ 256-steps (8-bit) is 37.5 kHz
    *  Conditions:
    *  frequency * resolution * 2 < 19.2 MHz
    *  resolution <= 4095
    *  @return Error or success
    */
   virtual bool SetClkHWPWM(
         double   frequency, ///< [in] desired frequency in Hz
         uint32_t resolution, ///< [in] desired resolution in steps
         EHwGpioHWPWMMode mode ///< [in] PWM mode
         ) = 0;
   //@} Hardware PWM setup functions


   /// @name Basic GPIO operations
   //@{
   /***************************************************************************/
   /** Cleans up the GPIO and leaves it on default state
    *  @return Error or success
    */
   virtual void CleanUp() = 0;

   /***************************************************************************/
   /** Sets GPIO as input
    *  @return Error or success
    */
   virtual bool SetInp() = 0;

   /***************************************************************************/
   /** Sets GPIO as output
    *  @return Error or success
    */
   virtual bool SetOut() = 0;

   /***************************************************************************/
   /** Sets GPIO as alternative function
    *  @return Error or success
    */
   virtual bool SetAlt(
         int alt ///< [in] Alternative function of GPIO
         ) = 0;

   /***************************************************************************/
   /** Sets GPIO high. Not thread safe for performance reasons
    *  @return Error or success
    */
   virtual bool Set() = 0;

   /***************************************************************************/
   /** Sets GPIO low. Not thread safe for performance reasons
    *  @return Error or success
    */
   virtual bool Reset() = 0;

   /***************************************************************************/
   /** Gets the GPIO state high/low.
    *  @return high true, or low false
    */
   virtual bool Get() = 0;
   //@} Basic GPIO operations

   /// @name PWM Functions
   //@{

   /***************************************************************************/
   /** Start the hardware PWM. The #InitHWPWM() function must be called
    *  before starting the HW PWM. Only GPIOs 12, 13, 18, and 19 can be rerouted
    *  to the PWM.
    *  @return Error or success
    */
   virtual bool StartHWPWM(
         float dutyCycle ///< [in] Desired duty cycle [0..1]
         ) = 0;

   /***************************************************************************/
   /** Reset the duty cycle of the hardware PWM. #StartHWPWM() must be called
    *  before reseting the duty cycle. Not thread safe for performance reasons.
    *  @return Error or success
    */
   virtual bool ResetHWPWMDuCy(
         float dutyCycle ///< [in] Desired duty cycle [0..1]
         ) = 0;

   /***************************************************************************/
   /** Stop the hardware PWM
    *  @return Error or success
    */
   virtual bool StopHWPWM() = 0;

   /***************************************************************************/
   /** Start software PWM on the desired GPIO with normal soft real-time priority.
    *  The period must between 10 and 10000 ms. Input outside range will be
    *  limited. The faster the period, the more CPU usage.
    *   @return Error or success
    */
   virtual bool StartSWPWM(
         float dutyC,     ///< [in] Initial duty cycle [0..1]
         uint16_t periodMs ///< [in] PWM period in ms [10..10000]
         ) = 0;

   /***************************************************************************/
   /** Reset the duty cycle of the software PWM.
    *  @return Error or success
    */
   virtual bool ResetSWPWMDuCy(
         float dutyC ///< [in] Desired duty cycle [0..1]
         ) = 0;

   /***************************************************************************/
   /** Stop the software PWM
    *  @return Error or success
    */
   virtual bool StopSWPWM() = 0;

   /***************************************************************************/
   /** Start function to dim on and off a LED as if it were sleeping
    *  @return Error or success
    */
   virtual bool StartSWPWMSleepingLED(
         float sleepPeriod ///< [in] Initial sleeping period in s
         ) = 0;

   /***************************************************************************/
   /** Reset the period of the sleeping LED
    *  @return Error or success
    */
   virtual bool ResetSWPWMSleepingLED(
         float sleepingCycleS ///< [in] Sleeping cycle in s
         ) = 0;

   /***************************************************************************/
   /** Stop the sleeping LED and release resources
    *  @return Error or success
    */
   virtual bool StopSWPWMSleepingLED() = 0;
   //@} PWM Functions

   // In interfaces and abstract classes, ALWAYS declare a virtual destructor,
   // and implement / inline it
   virtual ~IHwGpio() {};

};

#endif // __cplusplus
#endif /* HWGPIO_H_ */
