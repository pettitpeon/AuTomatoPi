/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaTestTest.cpp
 *   Date     : Dec 7, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */

#include <string.h>
#include <iostream>
#include "BaTestTest.h"
#include "BaGenMacros.h"
#include "BaseApi.h"

LOCAL TBaBoolRC initStart(void* arg);
LOCAL void update(void *arg);

CPPUNIT_TEST_SUITE_REGISTRATION( CBaTestTest );

/* ****************************************************************************/
/*  ...
 */
void CBaTestTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaTestTest::tearDown() {
}


/* ****************************************************************************/
/*  ...
 */
void CBaTestTest::Test() {
   CPPUNIT_ASSERT(true);
   TBaApiCtrlTaskOpts opts;
   memset(&opts, 0, sizeof(opts));
   opts.init = initStart;
   opts.update = update;
   opts.exit = initStart;
   opts.cyleTimeMs = 1000;
   opts.prio = eBaCorePrio_RT_Normal;


   BaApiStartCtrlThread(&opts);
   BaCoreSleep(5);
   BaApiStopCtrlThread();
   BaCoreSleep(5);

   BaApiStartCtrlTask(&opts);
   BaCoreSleep(5);
   BaApiStopCtrlTask();
   BaCoreSleep(5);

   CPPUNIT_ASSERT(true);
}

LOCAL TBaBoolRC initStart(void *arg) {

   puts("initstart");

   return eBaBoolRC_Success;
}

LOCAL void update(void *arg) {

   puts("update");

   return;
}

