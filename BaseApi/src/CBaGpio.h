/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaGpio.h
 *   Date     : 23/06/2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *   GPIOs API
 */
/*------------------------------------------------------------------------------
 */

#ifndef CBAGPIO_H_
#define CBAGPIO_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "BaGpio.h"

class CBaGpio : public IBaGpio {
public:
   // Factory
   static IBaGpio* Create(uint8_t gpioNo);
   static bool Delete (
         IBaGpio* hdl
         );

   // Hardware PWM setup functions
   virtual bool InitHWPWM();
   virtual bool ExitHWPWM();
   virtual bool SetClkHWPWM(
         double   frequency, //< [in] desired frequency in Hz
         uint32_t resolution, //< [in] desired resolution in steps
         EBaGpioHWPWMMode mode //< [in] PWM mode
         );

   // Basic GPIO operations
   virtual void CleanUp();
   virtual bool SetInp();
   virtual bool SetOut();
   virtual bool SetAlt(
         int alt //< [in] Alternative function of GPIO
         );
   virtual bool Set();
   virtual bool Reset();
   virtual bool Get();

   // PWM Functions
   virtual bool StartHWPWM(
         float   dutyCycle //< [in] Desired duty cycle [0..1]
         );
   virtual bool ResetHWPWMDuCy(
         float   dutyCycle //< [in] Desired duty cycle [0..1]
         );
   virtual bool StopHWPWM();
   virtual bool StartSWPWM(
         float dutyC,     //< [in] Initial duty cycle [0..1]
         uint16_t periodMs //< [in] PWM period in ms [10..10000]
         );
   virtual bool ResetSWPWMDuCy(
         float dutyC //< [in] Desired duty cycle [0..1]
         );
   virtual bool StopSWPWM();
   virtual bool StartSWPWMSleepingLED(
         float sleepPeriod //< [in] Initial sleeping period in s
         );
   virtual bool ResetSWPWMSleepingLED(
         float sleepingCycleS //< [in] Sleeping cycle in s
         );
   virtual bool StopSWPWMSleepingLED();

private:
   // Private constructor because a public factory method is used
   CBaGpio() : mpImpl(0) {};

   // Typical object oriented destructor must be virtual!
   virtual ~CBaGpio() {};

   // Make this object non-copyable.
   // This is important because the implementation is in a dynamic pointer and
   // a deep copy of the implementation is explicitly required
   CBaGpio(const CBaGpio&);
   CBaGpio& operator=(const CBaGpio&);

   // IF THIS IS AN EXTERNAL INTERFACE, NO MEMBER VARIABLES!
   // Pimpl idiom (Pointer to Implementation) http://c2.com/cgi/wiki?PimplIdiom
   class Impl;
   Impl *mpImpl;

};

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
#endif /* CBAGPIO_H_ */
