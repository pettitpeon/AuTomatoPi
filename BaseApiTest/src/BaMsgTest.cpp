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

#include <unistd.h>

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
   BaLogSetDefOpts(&logOpt);
   logOpt.name = "msglog";
   logOpt.path = RESPATH;
   IBaLog * pLog = IBaLogCreate(&logOpt);
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
   msg._L_SETSYSLOGF("tag", "%s", "Set message");
   msg._L_SETSYSLOGF("tag", "%s", "Set message");
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

   ASS(IBaLogDestroy(pLog, eBaBool_false));
   ASS_EQ((uint32_t)44, BaFS::Size(RESPATH "msglog.log"));
}

/* ****************************************************************************/
/*  Initialize resources
 */
void CBaMsgTest::TestIface() {
   IBaMsg *pMsg = 0;
   TBaMsgHdl msg = BaMsgCreate();
   TBaLogOptions logOpt;
   BaLogSetDefOpts(&logOpt);
   logOpt.name = "msglog";
   logOpt.path = RESPATH;
   IBaLog * pLog = IBaLogCreate(&logOpt);
   ASS(pLog);

   // To console
   ASS(!BaMsgGet(msg));
   BaMsgSetPrintF(msg, "%s\n", "Set message");
   BaMsgSetPrintF(msg, "%s\n", "Set message");
   ASS(BaMsgGet(msg));
   BaMsgReset(msg);
   ASS(!BaMsgGet(msg));

   // To syslog
   ASS(!BaMsgGet(msg));
   BAMSGSETSYSLOGF(msg, "tag", "%s", "Set message");
   BAMSGSETSYSLOGF(msg, "tag", "%s", "Set message");
   ASS(BaMsgGet(msg));
   BaMsgReset(msg);
   ASS(!BaMsgGet(msg));

   // To log
   ASS(!BaMsgGet(msg));
   BaMsgSetLogF(msg, pLog, eBaLogPrio_Trace, "tag", "%s", "Set message");
   BaMsgSetLogF(msg, pLog, eBaLogPrio_Trace, "tag", "%s", "Set message");
   ASS(BaMsgGet(msg));
   BaMsgReset(msg);
   ASS(!BaMsgGet(msg));

   ASS(BaMsgDestroy(msg));
   ASS(IBaLogDestroy(pLog, eBaBool_false));
   ASS_EQ((uint32_t)44, BaFS::Size(RESPATH "msglog.log"));

   pMsg = IBaMsgCreate();
   ASS(pMsg);
   ASS(IBaMsgDestroy(pMsg));
}

/* ****************************************************************************/
/*  Release resources
 */
void CBaMsgTest::Exit() {
   remove(RESPATH "msglog.log");
   rmdir(RESPATH);
}

