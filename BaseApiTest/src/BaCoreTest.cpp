/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaCoreTest.cpp
 *   Date     : 14.10.2015
 *------------------------------------------------------------------------------
 *   Module description:
 */

#include <iostream>
#include "BaseApi.h"
#include "BaCoreTest.h"
#include "BaCore.h"
#include "BaGenMacros.h"
#include "CppU.h"
#include "BaLogMacros.h"

#define TAG "Test"

CPPUNIT_TEST_SUITE_REGISTRATION( CBaCoreTest );

LOCAL void testTimingFun(void *arg);
LOCAL void testThreadNiceWeatherFun(TBaCoreThreadArg *pArg);
LOCAL void testInfThreadFun(TBaCoreThreadArg *pArg);
//LOCAL void writePidRout(void*);

char sBuf[BACORE_TSTAMPLEN];

/* ****************************************************************************/
/*  ...
 */
void CBaCoreTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaCoreTest::tearDown() {
}


/* ****************************************************************************/
/*  ...
 */
void CBaCoreTest::SleepAndTiming() {
   int64_t dur = 0;
   dur = BaCoreTimedS(testTimingFun, (void*)1);
   std::cout << "1s == " << dur <<"s \n";
   CPPUNIT_ASSERT_EQUAL(1LL, dur);

   dur = BaCoreTimedMs(testTimingFun, (void*)10);
   std::cout << "10ms == " << dur <<"ms \n";
   CPPUNIT_ASSERT_EQUAL(10LL, dur);

   dur = BaCoreTimedUs(testTimingFun, (void*)300);
#ifdef __linux
   CPPUNIT_ASSERT_MESSAGE("300us == " + std::to_string(dur) + "us",
         dur > 300 && dur < 400);
#endif
   // This uses the ns function, therefore, duration is in ns.
   dur = BaCoreTimedUs(testTimingFun, (void*)1000000);
#ifdef __linux
   CPPUNIT_ASSERT_MESSAGE("1000us == " + std::to_string(dur) + "us",
         dur > 1000 && dur < 1100);
#endif
}

/* ****************************************************************************/
/*  Test the time stamp functions
 */
void CBaCoreTest::TimeStamps() {
   TBaCoreTimeStamp tStamp;

   // Test the time stamp structure
   BaCoreGetTStamp(&tStamp);
   ASS(tStamp.micros < 1000000);
   ASS(tStamp.millis < 1000);

   // 11/04/2016
   ASS_MSG(std::to_string(tStamp.tt) + " > 1460366790", tStamp.tt > 1460366790);

   // Assert it returns a pointer
   ASS(BaCoreTStampToStr(&tStamp, sBuf));

   // Test the mallocated function
   const char * tss = BaCoreTStampToStr(&tStamp, 0);
   ASS(tss);
   std::cout << tss << std::endl;
   free((void*)tss);

   std::cout << BaCoreTStampToStr(&tStamp, sBuf) << std::endl;

   // Bad timestamp
   tStamp.millis = 1234;
   ASS(!BaCoreTStampToStr(&tStamp, sBuf));

}

/* ****************************************************************************/
/*  ...
 */
void CBaCoreTest::ThreadsNiceWeather() {
   TBaCoreThreadHdl hdl = 0;
   TBaCoreThreadArg arg;
   arg.pArg = (void*) 0;
   arg.exitTh = eBaBool_false;
   TBaCoreThreadInfo info;
   EBaCorePrio testPrio = eBaCorePrio_Minimum;
   std::string name = "";

   // Test creation and get info
   testPrio = eBaCorePrio_Normal;
   name = "eBaCorePrio_Normal";
   arg.exitTh = eBaBool_false;
   hdl = BaCoreCreateThread(name.c_str(), testThreadNiceWeatherFun, &arg, testPrio);
   CPPUNIT_ASSERT(hdl);
   BaCoreMSleep(150);
   CPPUNIT_ASSERT(BaCoreGetThreadInfo(hdl, &info));
   CPPUNIT_ASSERT(info.isRunning);
   CPPUNIT_ASSERT(arg.pArg);
   CPPUNIT_ASSERT_EQUAL(testPrio, info.prio);
   CPPUNIT_ASSERT(name == info.name);

   // Test thread finishing by itself
   arg.exitTh = true;
   BaCoreMSleep(250);
   BaCoreGetThreadInfo(hdl, &info);
   CPPUNIT_ASSERT(!info.isRunning);
   CPPUNIT_ASSERT(arg.pArg);
   CPPUNIT_ASSERT_EQUAL(testPrio, info.prio);

   // Destroy thread
   CPPUNIT_ASSERT(BaCoreDestroyThread(hdl, 0));
   hdl = 0;

   // Test destroying a running thread
   arg.exitTh = eBaBool_false;
   hdl = BaCoreCreateThread(name.c_str(), testThreadNiceWeatherFun, &arg, testPrio);
   CPPUNIT_ASSERT(hdl);
   BaCoreGetThreadInfo(hdl, &info);
   BaCoreMSleep(100);
   CPPUNIT_ASSERT(info.isRunning);
   CPPUNIT_ASSERT(arg.pArg);
   CPPUNIT_ASSERT_EQUAL(testPrio, info.prio);
   CPPUNIT_ASSERT(BaCoreDestroyThread(hdl, 100));
   hdl = 0;


   // eBaCorePrio_Highest /////////////////////////////
   testPrio = eBaCorePrio_Highest;
   name = "eBaCorePrio_Highest";
   arg.exitTh = eBaBool_false;
   hdl = BaCoreCreateThread(name.c_str(), testThreadNiceWeatherFun, &arg, testPrio);
   CPPUNIT_ASSERT(hdl);
   BaCoreGetThreadInfo(hdl, &info);
   BaCoreMSleep(100);
   CPPUNIT_ASSERT(info.isRunning);
   CPPUNIT_ASSERT(arg.pArg);
   CPPUNIT_ASSERT_EQUAL(testPrio, info.prio);

   // Test thread finishing by itself
   arg.exitTh = true;
   BaCoreMSleep(250);
   BaCoreGetThreadInfo(hdl, &info);
   CPPUNIT_ASSERT(!info.isRunning);
   CPPUNIT_ASSERT(arg.pArg);
   CPPUNIT_ASSERT_EQUAL(testPrio, info.prio);
   CPPUNIT_ASSERT(name == info.name);

   // Destroy thread with 0 timeout
   CPPUNIT_ASSERT(BaCoreDestroyThread(hdl, 0));
   hdl = 0;

   // Test destroying a running thread
   arg.exitTh = eBaBool_false;
   hdl = BaCoreCreateThread(name.c_str(), testThreadNiceWeatherFun, &arg, testPrio);
   CPPUNIT_ASSERT(hdl);
   BaCoreGetThreadInfo(hdl, &info);
   BaCoreMSleep(100);
   CPPUNIT_ASSERT(info.isRunning);
   CPPUNIT_ASSERT(arg.pArg);
   CPPUNIT_ASSERT_EQUAL(testPrio, info.prio);
   CPPUNIT_ASSERT(BaCoreDestroyThread(hdl, 100));
   hdl = 0;

   // eBaCorePrio_RT_High /////////////////////////////
   testPrio = eBaCorePrio_RT_High;
   name = "eBaCorePrio_RT_High";
   arg.exitTh = eBaBool_false;
   hdl = BaCoreCreateThread(name.c_str(), testThreadNiceWeatherFun, &arg, testPrio);
   CPPUNIT_ASSERT(hdl);
   BaCoreGetThreadInfo(hdl, &info);
   BaCoreMSleep(100);
   CPPUNIT_ASSERT(info.isRunning);
   CPPUNIT_ASSERT(arg.pArg);
   CPPUNIT_ASSERT_EQUAL(testPrio, info.prio);

   // Test thread finishing by itself
   arg.exitTh = true;
   BaCoreMSleep(250);
   BaCoreGetThreadInfo(hdl, &info);
   CPPUNIT_ASSERT(!info.isRunning);
   CPPUNIT_ASSERT(arg.pArg);
   CPPUNIT_ASSERT_EQUAL(testPrio, info.prio);

   // Destroy thread
   CPPUNIT_ASSERT(BaCoreDestroyThread(hdl, 0));
   hdl = 0;

   // Test destroying a running thread
   arg.exitTh = eBaBool_false;
   hdl = BaCoreCreateThread(name.c_str(), testThreadNiceWeatherFun, &arg, testPrio);
   CPPUNIT_ASSERT(hdl);
   BaCoreGetThreadInfo(hdl, &info);
   BaCoreMSleep(100);
   CPPUNIT_ASSERT(info.isRunning);
   CPPUNIT_ASSERT(arg.pArg);
   CPPUNIT_ASSERT_EQUAL(testPrio, info.prio);
   CPPUNIT_ASSERT(BaCoreDestroyThread(hdl, 100));
   hdl = 0;
}

/* ****************************************************************************/
/*  ...
 */
void CBaCoreTest::ThreadsSpecialCases() {
   TBaCoreThreadHdl hdl = 0;
   TBaCoreThreadArg arg;
   arg.pArg = (void*) 0;
   arg.exitTh = eBaBool_false;
   TBaCoreThreadInfo info;
   EBaCorePrio testPrio = eBaCorePrio_Normal;

   // Test destroying a running infinite thread
   hdl = BaCoreCreateThread("testThreadNorm", testInfThreadFun, &arg, testPrio);
   CPPUNIT_ASSERT(hdl);
   BaCoreGetThreadInfo(hdl, &info);
   BaCoreMSleep(100);
   CPPUNIT_ASSERT(info.isRunning);
   CPPUNIT_ASSERT_EQUAL(testPrio, info.prio);
   CPPUNIT_ASSERT(BaCoreDestroyThread(hdl, 10));
   hdl = 0;

   // Test destroying a running thread which uses an exit flag
   arg.exitTh = eBaBool_false;
   hdl = BaCoreCreateThread("testThreadNorm", testThreadNiceWeatherFun, &arg, testPrio);
   CPPUNIT_ASSERT(hdl);
   BaCoreGetThreadInfo(hdl, &info);
   BaCoreMSleep(100);
   CPPUNIT_ASSERT(info.isRunning);
   CPPUNIT_ASSERT(arg.pArg);
   CPPUNIT_ASSERT_EQUAL(testPrio, info.prio);
   CPPUNIT_ASSERT(BaCoreDestroyThread(hdl, 100));
   hdl = 0;

   // Destroy a null handle
   CPPUNIT_ASSERT(!BaCoreDestroyThread(0, 0));
}


/* ****************************************************************************/
/*  ...
 */
void CBaCoreTest::PidFiles() {

//   TBaApiCtrlTaskOpts opts = {0};
//   opts.cyleTimeMs = 1000;
//   opts.prio = eBaCorePrio_Normal;
//   opts.update = writePidRout;
//   TBaCoreTimeStamp ts;
//   BaCoreGetTStamp(&ts);
//   std::cout << BaCoreTStampToStr(&ts, 0) << std::endl;
//   TRACE_("Now!");
//
//   // Create a second process with the same name
//   ASS(BaApiStartCtrlTask(&opts));
//
//   // Give it chance to be born
//   BaCoreMSleep(500);
//
//   // Check if the PID in the file is running, is not ourself, and is called
//   // the same.
//   ASS(!BaCoreTestPidFile("BaseApiTest"));
//   BaCoreGetTStamp(&ts);
//   std::cout << BaCoreTStampToStr(&ts, 0) << std::endl;
//   ASS(BaApiStopCtrlTask());
}

// Auxiliary timing function
LOCAL void testTimingFun(void * arg) {
   int64_t dur = (int64_t) arg;

   if (dur < 10) {
      BaCoreSleep(dur);
   } else if (dur < 100) {
      BaCoreMSleep(dur);
   } else if (dur < 1000) {
      BaCoreUSleep(dur);
   } else {
      BaCoreNSleep(dur);
   }

}

// Auxiliary function for nice weather thread tests
LOCAL void testThreadNiceWeatherFun(TBaCoreThreadArg *pArg) {
   for (int i = 0; i < 1000 && !pArg->exitTh; ++i) {
      pArg->pArg = (void*) ((int)pArg->pArg + 1);
      std::cout << "Thread " << (int64_t) pArg->pArg << " s\n";
      BaCoreMSleep(100);
   }
   std::cout << "testThreadNiceWeatherFun exit\n";
}

//  Auxiliary function for infinite thread test
LOCAL void testInfThreadFun(TBaCoreThreadArg *pArg) {
   for (int i = 0; i < 6; ++i) {
      std::cout << "InfThread " << i << " s\n";
      BaCoreMSleep(100);
   }
   std::cout << "testInfThreadFun exit\n";
}

//LOCAL void writePidRout(void*) {
//   static int sInit = 0;
//   if (!sInit) {
////      BaCoreWritePidFile("BaseApiTest");
//      BaProcWriteOwnPidFile();
//      TBaCoreTimeStamp ts;
//      BaCoreGetTStamp(&ts);
//      std::cout << BaCoreTStampToStr(&ts, 0) << std::endl;
//   }
//   if (sInit >= 100) {
//      exit(0);
//   }
//   sInit++;
//}
