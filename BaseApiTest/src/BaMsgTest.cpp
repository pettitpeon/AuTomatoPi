/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaMsgTest.cpp
 *   Date     : Apr 27, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/* * @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */

#include "BaMsgTest.h"
#include "impl/CBaMsg.h"
#include "CppU.h"
#include "BaUtils.hpp"

#ifdef _WIN32
# define RESPATH CPPU_RESPATH "BaMsgTest\\"
#else
# define RESPATH CPPU_RESPATH "BaMsgTest/"
#endif

CPPUNIT_TEST_SUITE_REGISTRATION( CBaMsgTest );

/* ****************************************************************************/
/*  ...
 */
void CBaMsgTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaMsgTest::tearDown() {
}

/* ****************************************************************************/
/*  Initialize resources
 */
void CBaMsgTest::Init() {
   Exit();
   BaFS::MkDir(RESPATH);
}

/* ****************************************************************************/
/*  Initialize resources
 */
void CBaMsgTest::TestImpl() {
   CBaMsg msg;
   TBaLogOptions logOpt;
   CBaLogSetDef(&logOpt);
   logOpt.name = "msglog";
   logOpt.path = RESPATH;
   IBaLog * pLog = CBaLogCreate(&logOpt);
   ASS(pLog);

   // To console
   ASS(!msg.Get());
   msg.SetPrintF("%s\n", "Set message");
   msg.SetPrintF("%s\n", "Set message");
   ASS(msg.Get());
   msg.Reset();
   ASS(!msg.Get());

   // To syslog
   ASS(!msg.Get());
   msg.SetSysLogF("tag", __LINE__, "%s", "Set message");
   msg.SetSysLogF("tag", __LINE__, "%s", "Set message");
   ASS(msg.Get());
   msg.Reset();
   ASS(!msg.Get());

   // To log
   ASS(!msg.Get());
   msg.SetLogF(pLog, eBaLogPrio_Trace, "tag", "%s", "Set message");
   msg.SetLogF(pLog, eBaLogPrio_Trace, "tag", "%s", "Set message");
   ASS(msg.Get());
   msg.Reset();
   ASS(!msg.Get());

   ASS(CBaLogDestroy(pLog, eBaBool_false));
   ASS_EQ((uint32_t)44, BaFS::Size(RESPATH "msglog.log"));
}

/* ****************************************************************************/
/*  Release resources
 */
void CBaMsgTest::Exit() {
   remove(RESPATH "msglog.log");
   rmdir(RESPATH);
}

