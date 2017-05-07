/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaLogTest.cpp
 *   Date     : Dec 9, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/* * @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#include <unistd.h>
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

#define LOGRS_SZ        20
#define LOGCFG RESPATH "TestLog.cgf"
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
}

/* ****************************************************************************/
/*  Initialize resources
 */
void CBaLogTest::Init() {
   Exit();
   BaFS::MkDir(RESPATH);
   BaFS::MkDir(STRSDIR);
}

/* ****************************************************************************/
/*  For quick tests
 */
void CBaLogTest::Test() {
   CPPUNIT_ASSERT(true);
}

/* ****************************************************************************/
/*  Test creation, normal logging, destruction saving the state and reuse
 */
void CBaLogTest::CreateReuseDestroy() {
   // Create default log, Should be overwritten and not appended
   TBaLogInfo info;
   CBaLog *pDef = CBaLog::Create("LogDef", RESPATH);
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
/*  Test the format functions
 */
void CBaLogTest::TracesF() {
   // Create default log
   TBaLogInfo info;
   CBaLog *pDef = CBaLog::Create("LogDef", RESPATH);
   ASS(pDef);

   // Get the log info and save the full path. It is not available after
   // destruction
   pDef->GetLogInfo(&info);
   std::string fullPath(info.fullPath);

   // Log some messages
   ASS(!pDef->TraceF("tag", 0));
   ASS(pDef->TraceF(0,           "%s", "35"));
   ASS(pDef->TraceF("Def",       "%s", "70"));
   ASS(pDef->TraceF("DefTag",    "%s", "106"));
   ASS(pDef->TraceF("DefTagg",   "%s", "142"));
   ASS(pDef->TraceF("DefTagggg", "%s", "178"));

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint64_t)178, BaFS::Size(fullPath));
}

/* ****************************************************************************/
/*  Test the tags are written properly
 */
void CBaLogTest::Tags() {
   // Create default log
   TBaLogInfo info;
   CBaLog *pDef = CBaLog::Create("LogDef", RESPATH);
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
   ASS_EQ((uint64_t)178, BaFS::Size(fullPath));
}

/* ****************************************************************************/
/*  Test the priorities are written properly
 */
void CBaLogTest::Prios() {
   // Create default log
   TBaLogInfo info;
   CBaLog *pDef = CBaLog::Create("LogDef", RESPATH);
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
   ASS_EQ((uint64_t)214, BaFS::Size(fullPath));

   // Create again underflow
   pDef = CBaLog::Create("LogDef", RESPATH, (EBaLogPrio)(eBaLogPrio_Trace - 1));
   ASS(pDef);

   // Test all prios and out of range
   ASS(pDef->Trace("DefTag",              "35"));
   ASS(pDef->Warning("DefTag",            "70"));
   ASS(pDef->Error("DefTag",             "106"));
   ASS(pDef->Log(eBaLogPrio_UpsCrash, "DefTag", "142"));

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint64_t)142, BaFS::Size(fullPath));

   // Create again
   pDef = CBaLog::Create("LogDef", RESPATH, eBaLogPrio_Warning);
   ASS(pDef);

   // Test all prios and out of range
   ASS(pDef->Trace("DefTag",    "0")); // No traces
   ASS(pDef->Warning("DefTag", "35"));
   ASS(pDef->Error("DefTag",   "70"));
   ASS(pDef->Log(eBaLogPrio_UpsCrash, "DefTag", "106"));

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint64_t)106, BaFS::Size(fullPath));

   // Create again
   pDef = CBaLog::Create("LogDef", RESPATH, eBaLogPrio_Error);
   ASS(pDef);

   // Test all prios and out of range
   ASS(pDef->Trace("DefTag",    "0")); // No traces
   ASS(pDef->Warning("DefTag",  "0")); // No traces
   ASS(pDef->Error("DefTag",   "35"));
   ASS(pDef->Log(eBaLogPrio_UpsCrash, "DefTag", "70"));

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint64_t)70, BaFS::Size(fullPath));

   // Create again
   pDef = CBaLog::Create("LogDef", RESPATH, eBaLogPrio_UpsCrash);
   ASS(pDef);

   // Test all prios and out of range
   ASS(pDef->Trace("DefTag",   "0")); // No traces
   ASS(pDef->Warning("DefTag", "0")); // No traces
   ASS(pDef->Error("DefTag",   "0")); // No traces
   ASS(pDef->Log(eBaLogPrio_UpsCrash, "DefTag", "35"));

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint64_t)35, BaFS::Size(fullPath));

   // Create again overflow
   pDef = CBaLog::Create("LogDef", RESPATH, (EBaLogPrio)(eBaLogPrio_UpsCrash + 1));
   ASS(pDef);

   // Test all prios and out of range
   ASS(pDef->Trace("DefTag",   "0")); // No traces
   ASS(pDef->Warning("DefTag", "0")); // No traces
   ASS(pDef->Error("DefTag",   "0")); // No traces
   ASS(pDef->Log(eBaLogPrio_UpsCrash, "DefTag", "35"));

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint64_t)35, BaFS::Size(fullPath));
}

/* ****************************************************************************/
/*  Test if it is logged and or printed to console
 */
void CBaLogTest::LogVsPrint() {
   // Create default log
   TBaLogInfo info;
   CBaLog *pDef = CBaLog::Create("LogDef", RESPATH, eBaLogPrio_Trace, eBaLogOut_LogAndConsole);
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
   ASS_EQ((uint64_t)35, BaFS::Size(fullPath));

   // Create again
   pDef = CBaLog::Create("LogDef", RESPATH, eBaLogPrio_Trace, eBaLogOut_Log);
   ASS(pDef);

   // Test all prios and out of range
   ASS(pDef->Trace("LogDef", "35")); // No traces

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint64_t)35, BaFS::Size(fullPath));

   // Create again
   pDef = CBaLog::Create("LogPrint", RESPATH, eBaLogPrio_Trace, eBaLogOut_Console);
   ASS(pDef);
   pDef->GetLogInfo(&info);
   fullPath = info.fullPath;

   // Test all prios and out of range
   ASS(pDef->Trace("LogPrt", "0")); // No traces

   // Destroy and test size
   ASS(CBaLog::Destroy(pDef));
   pDef = 0;
   ASS_EQ((uint64_t)0, BaFS::Size(fullPath));
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
   ASS_EQ((uint64_t)37, BaFS::Size(fullPath));
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
   ASS_EQ((uint64_t)35, BaFS::Size(fullPath));

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
   ASS_EQ((uint64_t)70, BaFS::Size(fullPath));
   ASS_EQ((uint64_t)70, BaFS::Size(OPTSDIR "LogOpts_1.log"));
   ASS_EQ((uint64_t)71, BaFS::Size(OPTSDIR "LogOpts_2.log"));
   ASS_EQ((uint64_t)72, BaFS::Size(OPTSDIR "LogOpts_3.log"));
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

   opts.name = "LogStress";
   opts.path = STRSDIR;
   opts.prioFilt = eBaLogPrio_Trace;
   opts.out = eBaLogOut_Log; // do not output to console. too much spam
   opts.maxFileSizeB = 1024*1024; // 1 MiB
   opts.maxNoFiles   = 3; // Maximum 3 extra files
   opts.maxBufLength = 1000;

   // Create loggers
   CBaLog* pObj = CBaLog::Create(opts);
   ASS(pObj);
   BaCoreMSleep(50); // Sleep to give time for the logging thread to run
   pObj->GetLogInfo(&info);
   std::string fullPath(info.fullPath);
   // Create many loggers
   std::vector<CBaLog*> loggers(LOGRS_SZ);
   std::string name;
   for(uint32_t i = 0; i < loggers.size(); i++) {
      name = "strs4_" + std::to_string(i);
      loggers[i] = CBaLog::Create(name, opts.path, opts.prioFilt, opts.out);
      ASS(loggers[i]);
   }

//   thArg4.exitTh = false;
//   thArg4.pArg = &loggers;
//   stresser4Rout(&thArg4);

   // 1. stresser
   arg1.pLog = pObj;
   arg1.tag = "stres1";
   thArg1.exitTh = eBaBool_false;
   thArg1.pArg = &arg1;
   stresser1 = BaCoreCreateThread("logStresser1", stresserRout, &thArg1, eBaCorePrio_Normal);
   ASS(stresser1);

   // 2. stresser
   arg2.pLog = pObj;
   arg2.tag = "stres2";
   thArg2.exitTh = eBaBool_false;
   thArg2.pArg = &arg2;
   stresser2 = BaCoreCreateThread("logStresser2", stresserRout, &thArg2, eBaCorePrio_Normal);
   ASS(stresser2);

   // 3. stresser
   arg3.pLog = pObj;
   arg3.tag = "stres3";
   thArg3.exitTh = eBaBool_false;
   thArg3.pArg = &arg3;
   stresser3 = BaCoreCreateThread("logStresser3", stresserRout, &thArg3, eBaCorePrio_Normal);
   ASS(stresser3);

   // 4. stresser
   thArg4.exitTh = false;
   thArg4.pArg = &loggers;
   stresser4 = BaCoreCreateThread("logStresser4", stresser4Rout, &thArg4, eBaCorePrio_Normal);
   ASS(stresser4);

   // Wait and stop all threads
   BaCoreSleep(4);
   ASS(BaCoreDestroyThread(stresser1, 100));
   ASS(BaCoreDestroyThread(stresser2, 100));
   ASS(BaCoreDestroyThread(stresser3, 100));
   ASS(BaCoreDestroyThread(stresser4, 100));

   // Destroy and test size
   ASS(CBaLog::Destroy(pObj));
   pObj = 0;
   for(uint32_t i = 0; i < loggers.size(); i++) {
      ASS(CBaLog::Destroy(loggers[i]));
   }

   uint64_t size = BaFS::DirSize(STRSDIR, (uint32_t)-1);
   std::cout << size << " B, "<< size/1024 << " kiB"<< std::endl;
   std::string msg = std::to_string(size);
   ASS_MSG(msg.c_str(), size > 2500000);
}

/* ****************************************************************************/
/*  SysLog
 */
void CBaLogTest::SysLog() {
   BASYSLOG(0, "%s", "Message");
   BASYSLOG("", "%s", "Message");
   BASYSLOG("tag", "%s", "Message");
   BASYSLOG("tagtag", "%s", "Message");
   BASYSLOG("tagtagtag", "%s", "Message");
}

/* ****************************************************************************/
/*  Test the interface
 */
void CBaLogTest::TestIface() {
   // Create default log, Should be overwritten and not appended
   TBaLogInfo info;
   IBaLog *pDef = IBaLogCreateDef("LogDef");
   ASS(pDef);

   // Check that the file was overwritten
   pDef->GetLogInfo(&info);
   std::string fullPath(info.fullPath);
   uint64_t sz = BaFS::Size(fullPath);
   ASS_EQ((uint32_t)0, info.fileSizeB);
   ASS_EQ((uint64_t)0, sz);

   // Log some messages
   ASS(pDef->Trace("DefTag", "35"));
   ASS(pDef->Trace("DefTag", "70"));
   ASS(pDef->Trace("DefTag", "106"));
   pDef->Flush();

   // Get info and check file size
   pDef->GetLogInfo(&info);
   ASS_EQ(info.fileSizeB, (uint32_t)STAMPSZ * 3 + 3 + 3 + 4); // 106

   // Destroy and save state, file should remain
   ASS(IBaLogDestroy(pDef, true));
   pDef = 0;
   ASS(BaFS::Exists(fullPath));
   remove(fullPath.c_str());

   // Create with C API
   TBaLogOptions opts;
   info.fullPath = 0;
   BaLogSetDefOpts(&opts);
   opts.name = "LogDef";
   opts.path = RESPATH;
   TBaLogHdl hdl = BaLogCreate(&opts);

   // Get info
   BaLogGetLogInfo(hdl, &info);
   fullPath = info.fullPath;
   sz = BaFS::Size(fullPath);
   ASS_EQ((uint32_t)0, info.fileSizeB);
   ASS_EQ((uint64_t)0, sz);

   // Log some
   ASS(BaLogError(hdl, "tag", "35"));
   ASS(BaLogWarning(hdl, "tag", "70"));
   BaLogFlush(hdl);

   // Get info and check file size
   BaLogGetLogInfo(hdl, &info);
   ASS_EQ(info.fileSizeB, (uint32_t)STAMPSZ * 2 + 3 + 3); // 70

   // Destroy and save state, file should remain
   ASS(BaLogDestroy(hdl, true));
   hdl = 0;
   ASS(BaFS::Exists(fullPath));
}

/* ****************************************************************************/
/*  Release resources
 */
void CBaLogTest::Exit() {
   remove(OPTSDIR "LogOpts.log");
   remove(OPTSDIR "LogOpts_1.log");
   remove(OPTSDIR "LogOpts_2.log");
   remove(OPTSDIR "LogOpts_3.log");
   rmdir(OPTSDIR);

   remove(STRSDIR "LogStress.log");
   std::string name;
   for(uint32_t i = 0; i < LOGRS_SZ; i++) {
      name = STRSDIR + ("strs4_" + std::to_string(i) + ".log");
      remove(name.c_str());

   }
   rmdir(STRSDIR);
   remove(RESPATH "LogDef.log");
   remove(RESPATH "LogPrint.log");
   rmdir(RESPATH);
}

// Stresser routine
static void stresserRout(TBaCoreThreadArg *pArg) {
   CBaLog* pObj = ((TTemp*) pArg->pArg)->pLog;
   const char* tag = ((TTemp*) pArg->pArg)->tag;
   int i = 0;
   for (i = 0; i < 5000 && !pArg->exitTh; ++i) {
      pObj->TraceF(tag, "Message No. %04i", i);

      // Sleep every 50 messages to give time to the buffers to be emptied
      if (i % 50 == 0) {
         BaCoreMSleep(50);
      }
   }

   std::cout << i << std::endl;
}

// Stresser routine for logger 4
static void stresser4Rout(TBaCoreThreadArg *pArg) {
   std::vector<CBaLog*> &loggers = *((std::vector<CBaLog*>*) pArg->pArg);
   int i = 0;
   uint32_t j = 0;
   for (i = 0; i < 5000 && !pArg->exitTh; ++i) {
      for (j = 0; j < LOGRS_SZ; ++j) {
         loggers[j]->TraceF("stres4", "Message No. %04i", i);
      }

      // Sleep every 50 messages to give time to the buffers to be emptied
      if (i % 50 == 0) {
         BaCoreMSleep(50);
      }
   }

   std::cout << i << std::endl;
}

