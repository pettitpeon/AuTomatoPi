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
# define BINNAME "BaseApiTest.exe"
#else
# define PIDPATH "/var/run/OsProc/"
# define BINNAME "BaseApiTest"
#endif
#define PIDEXT ".pid"

// Local functions
LOCAL TBaBoolRC initExit(void* arg);
LOCAL void update(void *arg);
LOCAL void slowUpdate(void *arg);

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
   sOpts.cyleTimeUs = 100000;
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
/*  Test the control thread
 */
void COsProcTest::ControlThread() {
   TOsProcCtrlTaskStats stats;

   // Logger should not be set before starting
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(BaApiInitLogger(sOpts.log));
   CPPUNIT_ASSERT(TRACE_("test2"));
   CPPUNIT_ASSERT(BaApiExitLogger());

   uint64_t slpMs = 500;
   CPPUNIT_ASSERT(OsProcStartCtrlThread(&sOpts));
   BaCoreMSleep(slpMs);
   CPPUNIT_ASSERT(!OsProcGetCtrlThreadStats(0));
   CPPUNIT_ASSERT(OsProcGetCtrlThreadStats(&stats));
   CPPUNIT_ASSERT(stats.imRunning);

   CPPUNIT_ASSERT(OsProcStopCtrlThread());
   CPPUNIT_ASSERT(OsProcGetCtrlThreadStats(&stats));
   CPPUNIT_ASSERT(!stats.imRunning);

   // Logger should not be set after stop
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(BaApiInitLogger(sOpts.log));
   CPPUNIT_ASSERT(TRACE_("test2"));
   CPPUNIT_ASSERT(BaApiExitLogger());
}

/* ****************************************************************************/
/*  Test the control task
 */
void COsProcTest::ControlTask() {
   TOsProcCtrlTaskStats stats;
   uint64_t slpMs = 100;

   // Logger should not be set before starting
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(BaApiInitLogger(sOpts.log));
   CPPUNIT_ASSERT(TRACE_("test2"));
   CPPUNIT_ASSERT(BaApiExitLogger());


#ifdef __WIN32
   CPPUNIT_ASSERT(!OsProcStartCtrlTask(&sOpts));
   BaCoreMSleep(slpMs);
   CPPUNIT_ASSERT(OsProcGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(stats.imRunning);
   CPPUNIT_ASSERT(!OsProcStopCtrlTask());
   CPPUNIT_ASSERT(OsProcGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(!stats.imRunning);
#else

   // Check that the process is not running via the PID file
   CPPUNIT_ASSERT(!OsProcCtrlTaskPidIsRunning());

   // Start the control task
   CPPUNIT_ASSERT(OsProcStartCtrlTask(&sOpts));
   BaCoreMSleep(slpMs);

   // This is the parent, but it knows the child is running
   CPPUNIT_ASSERT(OsProcGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(stats.imRunning);

   // Check that PID file is correct
   CPPUNIT_ASSERT(OsProcCtrlTaskPidIsRunning());
   char buf[BAPROC_SHORTNAMELEN] = {0};
   pid_t pid = OsProcReadCtrlTaskPidFile(buf);
   std::string tskName = "BaseApiTest";
   ASS(pid);
   ASS_EQ(tskName, std::string(buf));

   // Should not start because the control task is running
   CPPUNIT_ASSERT(!OsProcStartCtrlTask(&sOpts));

   CPPUNIT_ASSERT(OsProcStopCtrlTask());
   CPPUNIT_ASSERT(OsProcGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(!stats.imRunning);
   BaCoreMSleep(slpMs * 5);
   ASS(!OsProcCtrlTaskPidIsRunning());
   ASS(!OsProcReadCtrlTaskPidFile(0));
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
void COsProcTest::ControlThreadOvertime() {
   TOsProcCtrlTaskStats stats;

   // Logger should not be set before starting
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(BaApiInitLogger(sOpts.log));
   CPPUNIT_ASSERT(TRACE_("test2"));
   CPPUNIT_ASSERT(BaApiExitLogger());

   // Set the control thread options
   sOpts.update = slowUpdate;
   sOpts.cyleTimeUs = 40000;
   uint64_t slpUs = sOpts.cyleTimeUs * 2; // 80ms

   // Start the control thread
   CPPUNIT_ASSERT(OsProcStartCtrlThread(&sOpts));
   BaCoreUSleep(slpUs);

   // The parent process knows that the thread is running
   CPPUNIT_ASSERT(OsProcGetCtrlThreadStats(&stats));
   CPPUNIT_ASSERT(stats.imRunning);

   // Stop the thread
   ASS(OsProcStopCtrlThread());
   CPPUNIT_ASSERT(OsProcGetCtrlThreadStats(&stats));
   CPPUNIT_ASSERT(!stats.imRunning);

   // Logger should not be set after stop
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(BaApiInitLogger(sOpts.log));
   CPPUNIT_ASSERT(TRACE_("test2"));
   CPPUNIT_ASSERT(BaApiExitLogger());
}

/* ****************************************************************************/
/*  Test the control task
 */
void COsProcTest::ControlTaskOvertime() {
   TOsProcCtrlTaskStats stats;

   // Logger should not be set before starting
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(BaApiInitLogger(sOpts.log));
   CPPUNIT_ASSERT(TRACE_("test2"));
   CPPUNIT_ASSERT(BaApiExitLogger());

   sOpts.update = slowUpdate;
   sOpts.cyleTimeUs = 40000;
   uint64_t slpUs = sOpts.cyleTimeUs * 2; // 80

#ifdef __WIN32
   CPPUNIT_ASSERT(!OsProcStartCtrlTask(&sOpts));
   BaCoreUSleep(slpUs);
   CPPUNIT_ASSERT(OsProcGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(stats.imRunning);
   CPPUNIT_ASSERT(!OsProcStopCtrlTask());
   CPPUNIT_ASSERT(OsProcGetCtrlTaskStats(&stats));
   CPPUNIT_ASSERT(!stats.imRunning);
#else

   CPPUNIT_ASSERT(OsProcStartCtrlTask(&sOpts));
   BaCoreUSleep(slpUs);
   CPPUNIT_ASSERT(OsProcGetCtrlTaskStats(&stats));

   // This is the parent. But knows that the child process is running
   CPPUNIT_ASSERT(stats.imRunning);

   // Should not start
   CPPUNIT_ASSERT(!OsProcStartCtrlTask(&sOpts));

   // Let the task run for a bit
   BaCoreUSleep(slpUs*3);

   CPPUNIT_ASSERT(OsProcStopCtrlTask());
   CPPUNIT_ASSERT(OsProcGetCtrlTaskStats(&stats));
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
   OsProcGetCtrlTaskStats(&stats);
   TRACE_("update(%s): cnt=%llu, dur=%llu us, cycle=%llu us",
         stats.imRunning ? "T" : "F", stats.updCnt, stats.lastDurUs, stats.lastCycleUs);
   BaCoreMSleep(1);
   return;
}

//
LOCAL void slowUpdate(void *arg) {
   TOsProcCtrlTaskStats stats;
   OsProcGetCtrlTaskStats(&stats);
   TRACE_("update(%s): cnt=%llu, dur=%llu us, cycle=%llu us",
         stats.imRunning ? "T" : "F", stats.updCnt, stats.lastDurUs, stats.lastCycleUs);
   BaCoreMSleep(50);
   return;
}
