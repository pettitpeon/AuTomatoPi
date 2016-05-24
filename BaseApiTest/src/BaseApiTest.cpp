/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaseApiTest.cpp
 *   Date     : May 24, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#include <iostream>
#include "BaseApiTest.h"
#include "BaseApi.h"
#include "BaLogMacros.h"
#include "BaGenMacros.h"

#define TAG "test"

LOCAL TBaBoolRC initStart(void* arg);
LOCAL void update(void *arg);

CPPUNIT_TEST_SUITE_REGISTRATION( CBaseApiTest );

/* ****************************************************************************/
/*  ...
 */
void CBaseApiTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaseApiTest::tearDown() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaseApiTest::Init() {

}


/* ****************************************************************************/
/*  ...
 */
void CBaseApiTest::Test() {
   CPPUNIT_ASSERT(true);
   TBaApiCtrlTaskOpts opts = {0};
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

/* ****************************************************************************/
/*  ...
 */
void CBaseApiTest::Exit() {

}

LOCAL TBaBoolRC initStart(void *arg) {
   TRACE_("initStart");
   return eBaBoolRC_Success;
}

LOCAL void update(void *arg) {
   TRACE_("update");
   return;
}
