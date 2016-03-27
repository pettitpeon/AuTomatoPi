/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaLogTest.cpp
 *   Date     : Dec 9, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#include <sys/stat.h>
//#include <unistd.h>

#include <iostream>
#include "BaLogTest.h"
#include "BaGenMacros.h"
#include "CppU.h"
#include "impl/CBaLog.h"
#include "BaLog.h"
#include "BaCore.h"
#include "BaUtils.hpp"

#ifdef _WIN32
# define RESPATH CPPU_RESPATH "BaIniParseTest\\"
#else
# define RESPATH CPPU_RESPATH "BaIniParseTest/"
#endif


#define LOGCFG RESPATH "TestLog.cgf"
#define ASS             CPPUNIT_ASSERT
#define ASS_EQ          CPPUNIT_ASSERT_EQUAL

#define STAMPSZ         32




CPPUNIT_TEST_SUITE_REGISTRATION( CBaLogTest );

/* ****************************************************************************/
/*  ...
 */
void CBaLogTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaLogTest::tearDown() {
}


/* ****************************************************************************/
/*  Test creation, normal logging, destruction saving the state and reuse
 */
void CBaLogTest::CreateReuseDestroy() {
   // Create default log, Should be overwritten and not appended
   TBaLogInfo info;
   CBaLog *pDef = CBaLog::Create("LogDef");
   ASS(pDef);

   // Check that the file was overwritten
   pDef->GetLogInfo(&info);
   std::string fullPath(info.fullPath);
   uint32_t sz = BaFS::Size(fullPath);
   ASS_EQ((uint32_t)0, info.fileSizeB);
   ASS_EQ((uint32_t)0, sz);

   // Log some messages
   ASS(pDef->Trace("DefTag", "35"));
   ASS(pDef->Trace("DefTag", "70"));
   ASS(pDef->Trace("DefTag", "106"));
   pDef->Flush();

   // Get info and check file size
   pDef->GetLogInfo(&info);
   ASS_EQ(info.fileSizeB, (uint32_t)STAMPSZ * 3 + 3 + 3 + 4); // 106

   // Destroy and save state, file should remain
   ASS(CBaLog::Destroy(pDef, true));
   pDef = 0;
   ASS(BaFS::Exists(fullPath));

   // Check the file size of the remaining file
   sz = BaFS::Size(fullPath);
   ASS_EQ(info.fileSizeB, sz);

   // Retrieve the same log without overwriting it
   pDef = CBaLog::CreateFromCfg("LogDef");
   ASS(pDef);

   // Check that the size is still the same
   pDef->GetLogInfo(&info);
   ASS_EQ(info.fileSizeB, sz);

   // Log two more messages
   ASS(pDef->Trace("DefTag", "142"));
   ASS(pDef->Trace("DefTag", "178"));

   // Destroy without saving it to the CFG. File should remain
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   sz = BaFS::Size(fullPath);

   // Test the final size
   ASS_EQ((uint32_t)178, sz);
}

/* ****************************************************************************/
/*  Test the tags are written properly
 */
void CBaLogTest::Tags() {
   // Create default log
   TBaLogInfo info;
   CBaLog *pDef = CBaLog::Create("LogDef");
   ASS(pDef);

   // Get the log info and save the full path. It is not available after
   // destruction
   pDef->GetLogInfo(&info);
   std::string fullPath(info.fullPath);

   // Log some messages
   ASS(pDef->Trace(0,           "35"));
   ASS(pDef->Trace("Def",       "70"));
   ASS(pDef->Trace("DefTag",    "106"));
   ASS(pDef->Trace("DefTagg",   "142"));
   ASS(pDef->Trace("DefTagggg", "178"));

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint32_t)178, BaFS::Size(fullPath));
}

/* ****************************************************************************/
/*  Test the priorities are written properly
 */
void CBaLogTest::Prios() {
   // Create default log
   TBaLogInfo info;
   CBaLog *pDef = CBaLog::Create("LogDef");
   ASS(pDef);

   // Get the log info and save the full path. It is not available after
   // destruction
   pDef->GetLogInfo(&info);
   std::string fullPath(info.fullPath);

   // Test all prios and out of range
   ASS(pDef->Trace("DefTag",              "35"));
   ASS(pDef->Warning("DefTag",            "70"));
   ASS(pDef->Error("DefTag",             "106"));
   ASS(pDef->Log(eBaLogPrio_UpsCrash, "DefTag",                    "142"));
   ASS(pDef->Log((EBaLogPrio)(eBaLogPrio_UpsCrash + 20), "DefTag", "178"));
   ASS(pDef->Log((EBaLogPrio)(eBaLogPrio_Trace - 1), "DefTag",     "214"));

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint32_t)214, BaFS::Size(fullPath));

   // Create again underflow
   pDef = CBaLog::Create("LogDef", "", (EBaLogPrio)(eBaLogPrio_Trace - 1));
   ASS(pDef);

   // Test all prios and out of range
   ASS(pDef->Trace("DefTag",              "35"));
   ASS(pDef->Warning("DefTag",            "70"));
   ASS(pDef->Error("DefTag",             "106"));
   ASS(pDef->Log(eBaLogPrio_UpsCrash, "DefTag", "142"));

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint32_t)142, BaFS::Size(fullPath));

   // Create again
   pDef = CBaLog::Create("LogDef", "", eBaLogPrio_Warning);
   ASS(pDef);

   // Test all prios and out of range
   ASS(pDef->Trace("DefTag",    "0")); // No traces
   ASS(pDef->Warning("DefTag", "35"));
   ASS(pDef->Error("DefTag",   "70"));
   ASS(pDef->Log(eBaLogPrio_UpsCrash, "DefTag", "106"));

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint32_t)106, BaFS::Size(fullPath));

   // Create again
   pDef = CBaLog::Create("LogDef", "", eBaLogPrio_Error);
   ASS(pDef);

   // Test all prios and out of range
   ASS(pDef->Trace("DefTag",    "0")); // No traces
   ASS(pDef->Warning("DefTag",  "0")); // No traces
   ASS(pDef->Error("DefTag",   "35"));
   ASS(pDef->Log(eBaLogPrio_UpsCrash, "DefTag", "70"));

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint32_t)70, BaFS::Size(fullPath));

   // Create again
   pDef = CBaLog::Create("LogDef", "", eBaLogPrio_UpsCrash);
   ASS(pDef);

   // Test all prios and out of range
   ASS(pDef->Trace("DefTag",   "0")); // No traces
   ASS(pDef->Warning("DefTag", "0")); // No traces
   ASS(pDef->Error("DefTag",   "0")); // No traces
   ASS(pDef->Log(eBaLogPrio_UpsCrash, "DefTag", "35"));

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint32_t)35, BaFS::Size(fullPath));

   // Create again overflow
   pDef = CBaLog::Create("LogDef", "", (EBaLogPrio)(eBaLogPrio_UpsCrash + 1));
   ASS(pDef);

   // Test all prios and out of range
   ASS(pDef->Trace("DefTag",   "0")); // No traces
   ASS(pDef->Warning("DefTag", "0")); // No traces
   ASS(pDef->Error("DefTag",   "0")); // No traces
   ASS(pDef->Log(eBaLogPrio_UpsCrash, "DefTag", "35"));

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint32_t)35, BaFS::Size(fullPath));
}

/* ****************************************************************************/
/*  ...
 */
void CBaLogTest::Test() {
   CPPUNIT_ASSERT(true);

//   CBaLog *log1 = CBaLog::Create("TestLog1", "", 40, 2, 0);
//   log1->Log(eBaLogPrio_Trace, "msg 1.1");
//   log1->LogF(eBaLogPrio_Trace, "msg 1.%i", 2);
//   log1->LogF(eBaLogPrio_Trace, "msg 1.%i", 3);
//   log1->LogF(eBaLogPrio_Trace, "msg 1.%i", 4);
//   log1->LogF(eBaLogPrio_Warning, "msg 1.%i", 5);
//   CBaLog::Delete(log1, true);
//   log1 = CBaLog::CreateFromCfg("C:\\log\\config\\TestLog1.cfg");
//   log1->Log(eBaLogPrio_Trace, "msg 1.6");
//   log1->LogF(eBaLogPrio_Trace, "msg 1.%i", 7);
//   CBaLog::Delete(log1, true);


   CBaLog *log2 = CBaLog::Create("TestLog2", "", eBaLogPrio_Trace, eBaLogOut_LogAndConsole, 100024, 2, 0);
   BaCoreNSleep(100);
   log2->Log(eBaLogPrio_Trace, "tag", 0);
   log2->Log(eBaLogPrio_Trace, 0, "msg 2.1");
   log2->Log(eBaLogPrio_Trace, 0, 0);

   log2->Log(eBaLogPrio_Trace, "tagfgfdgfdgg", "msg 2.1");
   log2->Log(eBaLogPrio_Trace, "tag", "msg 2.1");
   log2->LogF(eBaLogPrio_Trace, "tagTag", "msg 2.%i", 2);
   log2->LogF(eBaLogPrio_Trace, "tagTag", 0, 2);
   log2->LogF(eBaLogPrio_Trace, 0, "msg 2.%i", 2);

   IBaLog *iLog = CBaLogCreateDef("ILog");
   iLog->LogF(eBaLogPrio_Trace, 0, "iLog 2.%i", 2);

   for (int i = 100; i < 2000; ++i) {
      log2->LogF(eBaLogPrio_Trace, "taTaTaTa", "msg 2.%i", i);
      std::cout << i << std::endl;
      if (i % 300 == 0) {
         BaCoreSleep(1);
      }
   }

   CBaLog *log3 = CBaLog::Create("TestLog3", "", eBaLogPrio_Trace, eBaLogOut_LogAndConsole, 40, 2, 0);
   log3->Log(eBaLogPrio_Trace, "tag", "LOOOOOOOOOOOOOOOOOOOOOOOOOOOONG msg 3.1");
   log3->LogF(eBaLogPrio_Trace, "tag", "msg 3.%i", 2);


   CBaLogDestroy(iLog, eBaBool_false);
   CBaLog::Destroy(log2);
   CBaLog::Destroy(log3);


}

/* ****************************************************************************/
/*  ...
 */
void CBaLogTest::FromCfg() {
   //
}
