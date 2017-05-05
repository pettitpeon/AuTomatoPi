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

#include <HwGpio.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include "BaGenMacros.h"
#include "BaCore.h"
#include "HwGpioTest.h"

#define GPIO 18

CPPUNIT_TEST_SUITE_REGISTRATION( CHwGpioTest );

static IHwGpio *spGpio = 0;

/* ****************************************************************************/
/*  ...
 */
void CHwGpioTest::setUp() {
   spGpio = IHwGpioCreate(GPIO);
   CPPUNIT_ASSERT(spGpio);
}

/* ****************************************************************************/
/*  ...
 */
void CHwGpioTest::tearDown() {
   CPPUNIT_ASSERT(IHwGpioDelete(spGpio));
   spGpio = 0;
}

/* ****************************************************************************/
/*  ...
 */
void CHwGpioTest::HardwarePWM() {
   std::cout << "Hello HW PWM\n";
   CPPUNIT_ASSERT(IHwGpioDelete(spGpio));
   spGpio = 0;

   CPPUNIT_ASSERT(HwGpioInit());
   CPPUNIT_ASSERT(HwGpioInitHWPWM());
   CPPUNIT_ASSERT(HwGpioStartHWPWM(GPIO, 0.10));
   CPPUNIT_ASSERT(HwGpioResetHWPWMDuCy(GPIO, 0.40));
   CPPUNIT_ASSERT(HwGpioResetHWPWMDuCy(GPIO, 0.60));
   CPPUNIT_ASSERT(HwGpioResetHWPWMDuCy(GPIO, 0.80));
   CPPUNIT_ASSERT(HwGpioResetHWPWMDuCy(GPIO, 1));
   CPPUNIT_ASSERT(HwGpioStopHWPWM(GPIO));
   CPPUNIT_ASSERT(HwGpioExitHWPWM());
   CPPUNIT_ASSERT(HwGpioExit());

   spGpio = IHwGpioCreate(GPIO);
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
void CHwGpioTest::SoftwarePWM() {
   std::cout << "Hello SW PWM\n";
   CPPUNIT_ASSERT(IHwGpioDelete(spGpio));
   spGpio = 0;

   // C API
   CPPUNIT_ASSERT(HwGpioInit());
   THwGpioSWPWMHdl hdl = HwGpioStartSWPWM(GPIO, 0.10, 20);
   CPPUNIT_ASSERT(hdl);
   CPPUNIT_ASSERT(HwGpioResetSWPWMDuCy(hdl, 0.40));
   BaCoreMSleep(1);
   CPPUNIT_ASSERT(HwGpioResetSWPWMDuCy(hdl, 0.60));
   CPPUNIT_ASSERT(HwGpioResetSWPWMDuCy(hdl, 0.80));
   CPPUNIT_ASSERT(HwGpioResetSWPWMDuCy(hdl, 1));
   CPPUNIT_ASSERT(HwGpioStopSWPWM(hdl));
   CPPUNIT_ASSERT(HwGpioExit());

   // C++ API
   spGpio = IHwGpioCreate(GPIO);
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
void CHwGpioTest::NormalOperations() {
   std::cout << "Hello GPIO test\n";
   CPPUNIT_ASSERT(spGpio->SetOut());
   spGpio->Reset(); // TODO: Check, It was added extra on Windows
   CPPUNIT_ASSERT(!spGpio->Get());
   CPPUNIT_ASSERT(spGpio->Set());
   CPPUNIT_ASSERT(spGpio->Get());
}

//#endif
