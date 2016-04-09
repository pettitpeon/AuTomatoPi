/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : TestTemplate.cpp
 *   Date     : 19.10.2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  Test template
 */
/*------------------------------------------------------------------------------
 */


//#ifdef __linux // Semaphores return ENOSYS (not implemented)on windows

#include <iostream>
#include <stdio.h>
#include <math.h>
#include "BaGpioTest.h"
#include "BaGenMacros.h"
#include "BaCore.h"
#include "BaGpio.h"

#define GPIO 18

CPPUNIT_TEST_SUITE_REGISTRATION( CBaGpioTest );

static IBaGpio *spGpio = 0;

/* ****************************************************************************/
/*  ...
 */
void CBaGpioTest::setUp() {
   spGpio = IBaGpioCreate(GPIO);
   CPPUNIT_ASSERT(spGpio);
}

/* ****************************************************************************/
/*  ...
 */
void CBaGpioTest::tearDown() {
   CPPUNIT_ASSERT(IBaGpioDelete(spGpio));
   spGpio = 0;
}

/* ****************************************************************************/
/*  ...
 */
void CBaGpioTest::HardwarePWM() {
   std::cout << "Hello HW PWM\n";
   CPPUNIT_ASSERT(IBaGpioDelete(spGpio));
   spGpio = 0;

   CPPUNIT_ASSERT(BaGpioInit());
   CPPUNIT_ASSERT(BaGpioInitHWPWM());
   CPPUNIT_ASSERT(BaGpioStartHWPWM(GPIO, 0.10));
   CPPUNIT_ASSERT(BaGpioResetHWPWMDuCy(GPIO, 0.40));
   CPPUNIT_ASSERT(BaGpioResetHWPWMDuCy(GPIO, 0.60));
   CPPUNIT_ASSERT(BaGpioResetHWPWMDuCy(GPIO, 0.80));
   CPPUNIT_ASSERT(BaGpioResetHWPWMDuCy(GPIO, 1));
   CPPUNIT_ASSERT(BaGpioStopHWPWM(GPIO));
   CPPUNIT_ASSERT(BaGpioExitHWPWM());
   CPPUNIT_ASSERT(BaGpioExit());

   spGpio = IBaGpioCreate(GPIO);
   CPPUNIT_ASSERT(spGpio);

   CPPUNIT_ASSERT(spGpio->InitHWPWM());
   CPPUNIT_ASSERT(spGpio->StartHWPWM(0.1));
   BaCoreSleep(1);
   CPPUNIT_ASSERT(spGpio->ResetHWPWMDuCy(0.4));
   BaCoreSleep(1);
   CPPUNIT_ASSERT(spGpio->ResetHWPWMDuCy(0.6));
   BaCoreSleep(1);
   CPPUNIT_ASSERT(spGpio->ResetHWPWMDuCy(0.8));
   BaCoreSleep(1);
   CPPUNIT_ASSERT(spGpio->ResetHWPWMDuCy(1));
   BaCoreSleep(1);
   CPPUNIT_ASSERT(spGpio->StopHWPWM());
   CPPUNIT_ASSERT(spGpio->ExitHWPWM());
}

/* ****************************************************************************/
/*  ...
 */
void CBaGpioTest::SoftwarePWM() {
   std::cout << "Hello SW PWM\n";
   CPPUNIT_ASSERT(IBaGpioDelete(spGpio));
   spGpio = 0;

   // C API
   CPPUNIT_ASSERT(BaGpioInit());
   TBaGpioSWPWMHdl hdl = BaGpioStartSWPWM(GPIO, 0.10, 20);
   CPPUNIT_ASSERT(hdl);
   CPPUNIT_ASSERT(BaGpioResetSWPWMDuCy(hdl, 0.40));
   BaCoreMSleep(1);
   CPPUNIT_ASSERT(BaGpioResetSWPWMDuCy(hdl, 0.60));
   CPPUNIT_ASSERT(BaGpioResetSWPWMDuCy(hdl, 0.80));
   CPPUNIT_ASSERT(BaGpioResetSWPWMDuCy(hdl, 1));
   CPPUNIT_ASSERT(BaGpioStopSWPWM(hdl));
   CPPUNIT_ASSERT(BaGpioExit());

   // C++ API
   spGpio = IBaGpioCreate(GPIO);
   CPPUNIT_ASSERT(spGpio);
   CPPUNIT_ASSERT(spGpio->StartSWPWM(0.1, 10));
   BaCoreSleep(1);
   CPPUNIT_ASSERT(spGpio->ResetSWPWMDuCy(0.4));
   BaCoreSleep(1);
   CPPUNIT_ASSERT(spGpio->ResetSWPWMDuCy(0.6));
   BaCoreSleep(1);
   CPPUNIT_ASSERT(spGpio->ResetSWPWMDuCy(0.8));
   BaCoreSleep(1);
   CPPUNIT_ASSERT(spGpio->ResetSWPWMDuCy(1));
   BaCoreSleep(1);
   CPPUNIT_ASSERT(spGpio->StopSWPWM());

   // Sleeping LED
   int slp = 1;
   CPPUNIT_ASSERT(spGpio->StartSWPWMSleepingLED(1/4.0));
   BaCoreSleep(slp);
   CPPUNIT_ASSERT(spGpio->ResetSWPWMSleepingLED(1/2.0));
   BaCoreSleep(slp);
   CPPUNIT_ASSERT(spGpio->ResetSWPWMSleepingLED(1));
   BaCoreSleep(slp);
   CPPUNIT_ASSERT(spGpio->ResetSWPWMSleepingLED(2));
   BaCoreSleep(slp);
   CPPUNIT_ASSERT(spGpio->StopSWPWMSleepingLED());
}

/* ****************************************************************************/
/*  ...
 */
void CBaGpioTest::NormalOperations() {
   std::cout << "Hello GPIO test\n";
   CPPUNIT_ASSERT(spGpio->SetOut());
   spGpio->Reset(); // TODO: Check, It was added extra on Windows
   CPPUNIT_ASSERT(!spGpio->Get());
   CPPUNIT_ASSERT(spGpio->Set());
   CPPUNIT_ASSERT(spGpio->Get());
}

//#endif
