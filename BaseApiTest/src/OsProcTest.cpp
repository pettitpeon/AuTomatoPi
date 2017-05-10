/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : OsProcTest.cpp
 *   Date     : Jun 28, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  TODO:
 */
/*------------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <OsProc.h>
#include "BaGenMacros.h"
#include "CppU.h"
#include "BaUtils.hpp"
#include "OsProcTest.h"
#include "BaLogMacros.h"
#include "BaseApi.h"

#define TAG "OsTst"
#if __WIN32
# define PIDPATH "C:\\var\\run\\OsProc\\"
# define BINNAME "OsProcTest.exe"
#else
# define PIDPATH "/var/run/OsProc/"
# define BINNAME "BaseApiTest"
#endif
#define PIDEXT ".pid"

// Local functions
LOCAL TBaBoolRC initExit(void* arg);
LOCAL void update(void *arg);
LOCAL void loongUpdate(void *arg);

static TOsProcCtrlTaskOpts sOpts = {0};

CPPUNIT_TEST_SUITE_REGISTRATION( COsProcTest );

/* ****************************************************************************/
/*  ...
 */
void COsProcTest::setUp() {
   sOpts.update = update;
}

/* ****************************************************************************/
/*  ...
 */
void COsProcTest::tearDown() {
}

/* ****************************************************************************/
/*  ...
 */
void COsProcTest::Init() {
#ifdef __WIN32
   if (!BaFS::Exists("C:\\var")) {
      BaFS::MkDir("C:\\var");
   }

   if (!BaFS::Exists("C:\\var\\run")) {
         BaFS::MkDir("C:\\var\\run");
      }
#endif

   sOpts.init = initExit;
   sOpts.update = update;
   sOpts.exit = initExit;
   sOpts.cyleTimeMs = 100;
   sOpts.prio = eBaCorePrio_RT_Normal;
   sOpts.log.pLog = IBaLogCreateDef("COsProcTest");
}




/* ****************************************************************************/
/*  ...
 */
void COsProcTest::OwnNames() {

   ASS(OsProcGetOwnShortName());
   ASS(OsProcGetOwnFullName());
   std::string nameS = OsProcGetOwnShortName();
   std::string nameF = OsProcGetOwnFullName();
   ASS_MSG(nameS, nameS == BINNAME);
   ASS_MSG(nameF, nameF == BINNAME);
}

/* ****************************************************************************/
/*  ...
 */
void COsProcTest::CtrlTaskPID() {

   char buf[BAPROC_SHORTNAMELEN];

   // No CtrlTask PID at the beginning
   ASS(!OsProcReadCtrlTaskPidFile(buf));
   ASS(!OsProcReadCtrlTaskPidFile(0));

   // Write task PID
   ASS(OsProcWriteCtrlTaskPidFile());

   // Ctrl Task PID available
   ASS(OsProcReadCtrlTaskPidFile(buf));
   ASS(OsProcReadCtrlTaskPidFile(0));

   std::cout << buf << std::endl;
   ASS(std::string(buf) == OsProcGetOwnShortName());

   ASS(BaFS::Exists(PIDPATH "OsProcCtrlTask.pid"));
   ASS(OsProcDelCtrlTaskPidFile());
   ASS(!BaFS::Exists(PIDPATH "OsProcCtrlTask.pid"));
   ASS(!OsProcDelCtrlTaskPidFile());
}

/* ****************************************************************************/
/*  ...
 */
void COsProcTest::PIDFiles() {
   std::string path = PIDPATH +
         BaPath::ChangeFileExtension(OsProcGetOwnShortName(), PIDEXT);

   ASS(OsProcWriteOwnPidFile());
   ASS(OsProcReadPidFile(OsProcGetOwnShortName(), eBaBool_true));
   ASS(OsProcReadPidFile(path.c_str(), eBaBool_false));
   ASS(!OsProcReadPidFile("BadApp", eBaBool_true));
   ASS(!OsProcReadPidFile("BadApp", eBaBool_false));

   ASS(OsProcDelPidFile(OsProcGetOwnShortName(), eBaBool_true));
   ASS(OsProcWriteOwnPidFile());
   ASS(OsProcWriteOwnPidFile());
   ASS(OsProcDelPidFile(path.c_str(), eBaBool_false));
   ASS(!OsProcDelPidFile(OsProcGetOwnShortName(), eBaBool_true));
   ASS(!OsProcDelPidFile(path.c_str(), eBaBool_false));
   ASS(!OsProcReadPidFile(0, eBaBool_true));

   // Test external PID files!
   ASS(!OsProcReadPidFile(PIDPATH "BaseApiTest.pid", eBaBool_false));
   ASS(OsProcWriteOwnPidFile());
   ASS(OsProcReadPidFile(PIDPATH "BaseApiTest.pid", eBaBool_false));
   ASS(OsProcDelPidFile(PIDPATH "BaseApiTest.pid", eBaBool_false));
   ASS(!OsProcDelPidFile(PIDPATH "BaseApiTest.pid", eBaBool_false));
   ASS(!OsProcReadPidFile(0, eBaBool_false));
}

/* ****************************************************************************/
/*  ...
 */
void COsProcTest::NameFromPID() {
   std::string binName = BINNAME;
   char buf[BAPROC_FULLNAMELEN];
   const char *name = OsProcGetPIDName(getpid(), buf);
   ASS(buf == name);
   name = OsProcGetPIDName(getpid(), buf);
   ASS(buf == name);

#ifndef __WIN32
   name = OsProcGetPIDName(getpid(), buf);
   ASS_MSG(name, name == binName);
   name = OsProcGetPIDName(getpid(), 0);
   ASS_MSG(name, name == binName);

   // Test that freeing the mallocated string does not crash
   free((void*) name);
   ASS(!OsProcGetPIDName(0, 0));
   ASS(!OsProcGetPIDName(0, buf));
#endif
}

/* ****************************************************************************/
/*  ...
 */
void COsProcTest::Prio() {
   ASS(OsProcSetOwnPrio(eBaCorePrio_RT_Highest));
   // if out of range
   ASS(!OsProcSetOwnPrio((EBaCorePrio) (eBaCorePrio_Minimum - 1)));
   ASS(!OsProcSetOwnPrio((EBaCorePrio) (eBaCorePrio_RT_Highest + 1)));

   // If not windows test deeper
#ifndef __WIN32
   ASS(OsProcSetOwnPrio(eBaCorePrio_Normal));
   ASS_EQ(eBaCorePrio_Normal, OsProcGetOwnPrio());
#endif

}

/* ****************************************************************************/
/*  Test the control task
 */
void COsProcTest::ControlTask() {
   TOsProcCtrlTaskStats stats;

   // Logger should not be set before starting
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(BaApiInitLogger(sOpts.log));
   CPPUNIT_ASSERT(TRACE_("test2"));
   CPPUNIT_ASSERT(BaApiExitLogger());

   uint64_t slpMs = 500;
   CPPUNIT_ASSERT(OsApiStartCtrlThread(&sOpts));
   BaCoreMSleep(slpMs);
   CPPUNIT_ASSERT(!OsApiGetCtrlTaskStats(0));
   CPPUNIT_ASSERT(OsApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(stats.imRunning);
   CPPUNIT_ASSERT(OsApiStopCtrlThread());
   CPPUNIT_ASSERT(OsApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(!stats.imRunning);

#ifdef __WIN32
   CPPUNIT_ASSERT(!OsApiStartCtrlTask(&sOpts));
   BaCoreMSleep(slpMs);
   CPPUNIT_ASSERT(OsApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(stats.imRunning);
   CPPUNIT_ASSERT(!OsApiStopCtrlTask());
   CPPUNIT_ASSERT(OsApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(!stats.imRunning);
#else

   CPPUNIT_ASSERT(OsApiStartCtrlTask(&sOpts));
   BaCoreMSleep(slpMs);
   CPPUNIT_ASSERT(OsApiGetCtrlTaskStats(&stats));

   // This is the parent. therefore in the
   CPPUNIT_ASSERT(stats.imRunning);

   // should not start
   CPPUNIT_ASSERT(!OsApiStartCtrlTask(&sOpts));

   CPPUNIT_ASSERT(OsApiStopCtrlTask());
   CPPUNIT_ASSERT(OsApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(!stats.imRunning);

#endif

   // Logger should not be set after stop
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(BaApiInitLogger(sOpts.log));
   CPPUNIT_ASSERT(TRACE_("test2"));
   CPPUNIT_ASSERT(BaApiExitLogger());
}

/* ****************************************************************************/
/*  Test the control task
 */
void COsProcTest::LongControlTask() {
   TOsProcCtrlTaskStats stats;

   // Logger should not be set before starting
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(BaApiInitLogger(sOpts.log));
   CPPUNIT_ASSERT(TRACE_("test2"));
   CPPUNIT_ASSERT(BaApiExitLogger());

   uint64_t slpMs = 500;
   sOpts.update = loongUpdate;

   CPPUNIT_ASSERT(OsApiStartCtrlThread(&sOpts));
   BaCoreMSleep(slpMs);
   CPPUNIT_ASSERT(OsApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(stats.imRunning);

   // It will not be able to destroy the thread without timeout because the
   // update function takes too long, so do not test it
   OsApiStopCtrlThread();
   CPPUNIT_ASSERT(OsApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(!stats.imRunning);

#ifdef __WIN32
   CPPUNIT_ASSERT(!OsApiStartCtrlTask(&sOpts));
   BaCoreMSleep(slpMs);
   CPPUNIT_ASSERT(OsApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(stats.imRunning);
   CPPUNIT_ASSERT(!OsApiStopCtrlTask());
   CPPUNIT_ASSERT(OsApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(!stats.imRunning);
#else

   CPPUNIT_ASSERT(OsApiStartCtrlTask(&sOpts));
   BaCoreMSleep(slpMs);
   CPPUNIT_ASSERT(OsApiGetCtrlTaskStats(&stats));

   // This is the parent. therefore in the
   CPPUNIT_ASSERT(stats.imRunning);

   // should not start
   CPPUNIT_ASSERT(!OsApiStartCtrlTask(&sOpts));

   CPPUNIT_ASSERT(OsApiStopCtrlTask());
   CPPUNIT_ASSERT(OsApiGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(!stats.imRunning);

#endif

   // Logger should not be set after stop
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(BaApiInitLogger(sOpts.log));
   CPPUNIT_ASSERT(TRACE_("test2"));
   CPPUNIT_ASSERT(BaApiExitLogger());
}

/* ****************************************************************************/
/*  ...
 */
void COsProcTest::Exit() {
   IBaLogDestroy(sOpts.log.pLog, eBaBool_false);
}

// init and exit
LOCAL TBaBoolRC initExit(void *arg) {
   TRACE_("initExit");
   return eBaBoolRC_Success;
}

//
LOCAL void update(void *arg) {
   TOsProcCtrlTaskStats stats;
   OsApiGetCtrlTaskStats(&stats);
   TRACE_("update(%s): cnt=%llu, dur=%llu us, cycle=%llu us",
         stats.imRunning ? "T" : "F", stats.updCnt, stats.lastDurUs, stats.lastCycleUs);
   BaCoreMSleep(1);
   return;
}

//
LOCAL void loongUpdate(void *arg) {
   TOsProcCtrlTaskStats stats;
   OsApiGetCtrlTaskStats(&stats);
   TRACE_("update(%s): cnt=%llu, dur=%llu us, cycle=%llu us",
         stats.imRunning ? "T" : "F", stats.updCnt, stats.lastDurUs, stats.lastCycleUs);
   BaCoreMSleep(150);
   return;
}
