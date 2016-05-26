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
#include "BaLog.h"

#define TAG "BaTst"

LOCAL TBaBoolRC initExit(void* arg);
LOCAL void update(void *arg);


TBaApiCtrlTaskOpts sOpts = {0};

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
   sOpts.init = initExit;
   sOpts.update = update;
   sOpts.exit = initExit;
   sOpts.cyleTimeMs = 100;
   sOpts.prio = eBaCorePrio_RT_Normal;
   sOpts.log.pLog = IBaLogCreateDef("CBaseApiTest");
}


/* ****************************************************************************/
/*  Test the control task
 */
void CBaseApiTest::ControlTask() {
   TBaApiCtrlTaskStats stats;

   // Logger should not be set before starting
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(BaApiInitLogger(sOpts.log));
   CPPUNIT_ASSERT(TRACE_("test2"));
   CPPUNIT_ASSERT(BaApiExitLogger());

   uint64_t slpMs = 500;
   CPPUNIT_ASSERT(BaApiStartCtrlThread(&sOpts));
   BaCoreMSleep(slpMs);
   CPPUNIT_ASSERT(!BaApiGetCtrlTaskStats(0));
   CPPUNIT_ASSERT(BaApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(stats.imRunning);
   CPPUNIT_ASSERT(BaApiStopCtrlThread());
   CPPUNIT_ASSERT(BaApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(!stats.imRunning);

#ifdef __WIN32
   CPPUNIT_ASSERT(!BaApiStartCtrlTask(&sOpts));
   BaCoreMSleep(slpMs);
   CPPUNIT_ASSERT(BaApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(stats.imRunning);
   CPPUNIT_ASSERT(!BaApiStopCtrlTask());
   CPPUNIT_ASSERT(BaApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(!stats.imRunning);
#else
   CPPUNIT_ASSERT(!BaApiStartCtrlTask(&sOpts));
   BaCoreMSleep(slpMs);
   CPPUNIT_ASSERT(BaApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(stats.imRunning);
   CPPUNIT_ASSERT(!BaApiStopCtrlTask());
   CPPUNIT_ASSERT(BaApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(!stats.imRunning);
#endif

   // Logger should not be set after stop
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(BaApiInitLogger(sOpts.log));
   CPPUNIT_ASSERT(TRACE_("test2"));
   CPPUNIT_ASSERT(BaApiExitLogger());
}

/* ****************************************************************************/
/*  Test the general logger
 */
void CBaseApiTest::Logger() {
   TBaLogDesc bad = {0};

   // No logger set
   CPPUNIT_ASSERT(!TRACE_("test"));

   // Bad arguments
   CPPUNIT_ASSERT(!BaApiInitLoggerDef(0));
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(!BaApiInitLoggerDef(""));
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(!BaApiInitLogger(bad));
   CPPUNIT_ASSERT(!TRACE_("test3"));

   // Good init
   CPPUNIT_ASSERT(BaApiInitLoggerDef("testDef"));
   CPPUNIT_ASSERT(WARN_("test2"));

   // Re-inits are always graceful and do nothing
   CPPUNIT_ASSERT(BaApiInitLoggerDef(0));
   CPPUNIT_ASSERT(BaApiInitLogger(bad));
   CPPUNIT_ASSERT(ERROR_("test2"));

   // Exit
   CPPUNIT_ASSERT(BaApiExitLogger());
   CPPUNIT_ASSERT(!TRACE_("test3"));

}

/* ****************************************************************************/
/*  ...
 */
void CBaseApiTest::Exit() {
   IBaLogDestroy(sOpts.log.pLog, eBaBool_false);
}

// init and exit
LOCAL TBaBoolRC initExit(void *arg) {
   TRACE_("initExit");
   return eBaBoolRC_Success;
}

//
LOCAL void update(void *arg) {
   TBaApiCtrlTaskStats stats;
   BaApiGetCtrlTaskStats(&stats);
   TRACE_("update(%s): cnt=%llu, dur=%llu us, cycle=%llu us",
         stats.imRunning ? "T" : "F", stats.updCnt, stats.actDurUs, stats.lastCycleUs);
   BaCoreMSleep(1);
   return;
}
