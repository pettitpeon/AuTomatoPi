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

//#include <stdio.h>


#include <iostream>
#include "BaLogTest.h"
#include "BaGenMacros.h"
#include "CppU.h"
#include "impl/CBaLog.h"
#include "BaLog.h"
#include "BaCore.h"
#include "BaUtils.hpp"
#include "dbg/BaDbgMacros.h"

#ifdef _WIN32
# define RESPATH CPPU_RESPATH "BaLogTest\\"
# define OPTSDIR RESPATH      "opts\\"
# define STRSDIR RESPATH      "stress\\"
#else
# define RESPATH CPPU_RESPATH "BaLogTest/"
# define OPTSDIR RESPATH      "opts/"
# define STRSDIR RESPATH      "stress/"
#endif


#define LOGCFG RESPATH "TestLog.cgf"
#define ASS             CPPUNIT_ASSERT
#define ASS_EQ          CPPUNIT_ASSERT_EQUAL

#define STAMPSZ         32

typedef struct TTemp {
   CBaLog* pLog;
   const char* tag;
} TTemp;

static void stresserRout(TBaCoreThreadArg *pArg);
static void stresser4Rout(TBaCoreThreadArg *pArg);

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
   remove(OPTSDIR "LogOpts.log");
   remove(OPTSDIR "LogOpts_1.log");
   remove(OPTSDIR "LogOpts_2.log");
   remove(OPTSDIR "LogOpts_3.log");
   rmdir(OPTSDIR);

   remove(STRSDIR "LogStress.log");
   std::string name;
   for(uint32_t i = 0; i < 20; i++) {
      name = STRSDIR + ("strs4_" + std::to_string(i) + ".log");
      remove(name.c_str());

   }

   rmdir(STRSDIR);
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
/*  Test if it is logged and or printed to console
 */
void CBaLogTest::LogVsPrint() {
   // Create default log
   TBaLogInfo info;
   CBaLog *pDef = CBaLog::Create("LogDef", "", eBaLogPrio_Trace, eBaLogOut_LogAndConsole);
   ASS(pDef);

   // Get the log info and save the full path. It is not available after
   // destruction
   pDef->GetLogInfo(&info);
   std::string fullPath(info.fullPath);

   // Log some messages
   ASS(pDef->Trace("DefTag", "35"));

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint32_t)35, BaFS::Size(fullPath));

   // Create again
   pDef = CBaLog::Create("LogDef", "", eBaLogPrio_Trace, eBaLogOut_Log);
   ASS(pDef);

   // Test all prios and out of range
   ASS(pDef->Trace("LogDef", "35")); // No traces

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint32_t)35, BaFS::Size(fullPath));

   // Create again
   pDef = CBaLog::Create("LogPrint", "", eBaLogPrio_Trace, eBaLogOut_Console);
   ASS(pDef);
   pDef->GetLogInfo(&info);
   fullPath = info.fullPath;

   // Test all prios and out of range
   ASS(pDef->Trace("LogPrt", "0")); // No traces

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint32_t)0, BaFS::Size(fullPath));
}

/* ****************************************************************************/
/*  Test the files, file sizes, and buffer options
 */
void CBaLogTest::FilesAndSizesOpts() {
   // Create default log
   TBaLogInfo info;
   TBaLogOptions opts;

   opts.name = "LogOpts";
   opts.path = OPTSDIR;
   opts.prioFilt = eBaLogPrio_Trace;
   opts.out = eBaLogOut_LogAndConsole;
   opts.maxFileSizeB = 0; // Maximum 1 entry per file
   opts.maxNoFiles = 0;   // Maximum 0 extra files
   opts.maxBufLength = 0; // No buffer limit

   // Directory does not exist
   CBaLog *pObj = CBaLog::Create(opts);
   ASS(!pObj);

   // Try again with directory
   ASS(BaFS::MkDir(OPTSDIR) == 0);
   pObj = CBaLog::Create(opts, true);
   ASS(pObj);
   pObj->GetLogInfo(&info);
   std::string fullPath(info.fullPath);

   // Log some messages
   ASS(pObj->Trace("LogOpt",   "35"));
   ASS(pObj->Trace("LogOpt",  " 36"));
   ASS(pObj->Trace("LogOpt", "  37"));
   pObj->Flush();

   // Destroy and test size
   ASS(CBaLog::Destroy(pObj));
   pObj = 0;
   ASS_EQ((uint32_t)37, BaFS::Size(fullPath));
   ASS(!BaFS::Exists(OPTSDIR "LogOpts_1.log"));

   // New options
   opts.maxFileSizeB = 0; // Maximum 1 entry per file
   opts.maxNoFiles   = 0; // Maximum 0 extra files
   opts.maxBufLength = 1; // No buffer limit of 1
   pObj = CBaLog::Create(opts, true);
   ASS(pObj);

   // Log some messages
   ASS(pObj->Trace("LogOpt",   "35"));
   ASS(!pObj->Trace("LogOpt",  "0")); // This one is logged before flushing
   pObj->Flush();

   // Destroy and test size
   ASS(CBaLog::Destroy(pObj));
   pObj = 0;
   ASS_EQ((uint32_t)35, BaFS::Size(fullPath));

   // New options
   opts.maxFileSizeB = 100; // Maximum 100 entries per file
   opts.maxNoFiles   =   3; // Maximum 3 extra files
   opts.maxBufLength = 100; // No buffer limit of 100
   pObj = CBaLog::Create(opts, true);
   ASS(pObj);

   // Log some messages file 1
   ASS(pObj->Trace("LogOpt",  "35"));
   ASS(pObj->Trace("LogOpt",  "70"));
   pObj->Flush();

   // Log some messages file 2
   ASS(pObj->Trace("LogOpt",   "35"));
   ASS(pObj->Trace("LogOpt",  " 71"));
   pObj->Flush();

   // Log some messages file 3
   ASS(pObj->Trace("LogOpt",    "35"));
   ASS(pObj->Trace("LogOpt",  "  72"));
   pObj->Flush();

   // Log some messages file 0
   ASS(pObj->Trace("LogOpt",  "35"));
   ASS(pObj->Trace("LogOpt",  "70"));
   pObj->Flush();

   // Destroy and test size
   ASS(CBaLog::Destroy(pObj));
   pObj = 0;
   ASS_EQ((uint32_t)70, BaFS::Size(fullPath));
   ASS_EQ((uint32_t)70, BaFS::Size(OPTSDIR "LogOpts_1.log"));
   ASS_EQ((uint32_t)71, BaFS::Size(OPTSDIR "LogOpts_2.log"));
   ASS_EQ((uint32_t)72, BaFS::Size(OPTSDIR "LogOpts_3.log"));
   ASS(!BaFS::Exists(OPTSDIR "LogOpts_4.log"));
}

/* ****************************************************************************/
/*  Stress the damn logger!
 */
void CBaLogTest::Stress() {
   // Create default log
   TBaLogInfo info;
   TBaLogOptions opts;
   TBaCoreThreadHdl stresser1 = 0;
   TBaCoreThreadHdl stresser2 = 0;
   TBaCoreThreadHdl stresser3 = 0;
   TBaCoreThreadHdl stresser4 = 0;
   TBaCoreThreadArg thArg1;
   TBaCoreThreadArg thArg2;
   TBaCoreThreadArg thArg3;
   TBaCoreThreadArg thArg4;

   TTemp arg1;
   TTemp arg2;
   TTemp arg3;

   BaFS::MkDir(STRSDIR);

   opts.name = "LogStress";
   opts.path = STRSDIR;
   opts.prioFilt = eBaLogPrio_Trace;
   opts.out = eBaLogOut_Log;
   opts.maxFileSizeB =  1024*1024; // 1 MiB
   opts.maxNoFiles   =  3;    // Maximum 0 extra files
   opts.maxBufLength = 1000;  // No buffer limit

   // Create loggers
   CBaLog* pObj = CBaLog::Create(opts);
   ASS(pObj);
   pObj->GetLogInfo(&info);
   std::string fullPath(info.fullPath);
   std::vector<CBaLog*> loggers(20);
   std::string name;
   for(uint32_t i = 0; i < loggers.size(); i++) {
      name = "strs4_" + std::to_string(i);
      loggers[i] = CBaLog::Create(name, opts.path, opts.prioFilt, opts.out);
      ASS(loggers[i]);
   }

   // 1. stresser
   arg1.pLog = pObj;
   arg1.tag = "stres1";
   thArg1.exitTh = eBaBool_false;
   thArg1.pArg = &arg1;
   stresser1 = BaCoreCreateThread("logStresser1", stresserRout, &thArg1, eBaCorePrio_Normal);

   // 2. stresser
   arg2.pLog = pObj;
   arg2.tag = "stres2";
   thArg2.exitTh = eBaBool_false;
   thArg2.pArg = &arg2;
   stresser2 = BaCoreCreateThread("logStresser2", stresserRout, &thArg2, eBaCorePrio_Normal);

   // 3. stresser
   arg3.pLog = pObj;
   arg3.tag = "stres3";
   thArg3.exitTh = eBaBool_false;
   thArg3.pArg = &arg3;
   stresser3 = BaCoreCreateThread("logStresser3", stresserRout, &thArg3, eBaCorePrio_Normal);

   // 4. stresser
   thArg4.exitTh = false;
   thArg4.pArg = &loggers;
   stresser4 = BaCoreCreateThread("logStresser4", stresser4Rout, &thArg4, eBaCorePrio_Normal);

   // Wait and stop all threads
   BaCoreSleep(5);
   BaCoreDestroyThread(stresser1, 100);
   BaCoreDestroyThread(stresser2, 100);
   BaCoreDestroyThread(stresser3, 100);
   BaCoreDestroyThread(stresser4, 100);

   // Destroy and test size
   ASS(CBaLog::Destroy(pObj));
   for(uint32_t i = 0; i < loggers.size(); i++) {
      CBaLog::Destroy(loggers[i]);
   }
   pObj = 0;
   uint64_t size = BaFS::DirSize(STRSDIR, (uint32_t)-1);
   std::cout << size << std::endl;
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

static void stresserRout(TBaCoreThreadArg *pArg) {
   CBaLog* pObj = ((TTemp*) pArg->pArg)->pLog;
   const char* tag = ((TTemp*) pArg->pArg)->tag;
   int i = 0;
   for (i = 0; i < 5000 && !pArg->exitTh; ++i) {
      pObj->TraceF(tag, "Message No. %04i", i);
      if (i % 50 == 0) {
         BaCoreMSleep(50);
      }
   }

   std::cout << i << std::endl;
}

static void stresser4Rout(TBaCoreThreadArg *pArg) {
   std::vector<CBaLog*> &loggers = *((std::vector<CBaLog*>*) pArg->pArg);
   int i = 0;
   uint32_t j = 0;
   for (i = 0; i < 5000 && !pArg->exitTh; ++i) {
      for (j = 0; j < loggers.size(); ++j) {
         loggers[j]->TraceF("stres4", "Message No. %04i", i);
      }
      if (i % 50 == 0) {
         BaCoreMSleep(50);
      }
   }

   std::cout << i << std::endl;
}

