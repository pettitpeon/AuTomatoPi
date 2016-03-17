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
#include <iostream>
#include "BaLogTest.h"
#include "BaGenMacros.h"
#include "CppU.h"
#include "impl/CBaLog.h"
#include "BaCore.h"

#ifdef _WIN32
# define RESPATH CPPU_RESPATH "BaIniParseTest\\"
#else
# define RESPATH CPPU_RESPATH "BaIniParseTest/"
#endif


#define LOGCFG RESPATH "TestLog.cgf"

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


   CBaLog *log2 = CBaLog::Create("TestLog2", "", 100024, 2, 0);
   BaCoreNSleep(100);
   log2->Log(eBaLogPrio_Trace, "tagfgfdgfdgg", "msg 2.1");
   log2->Log(eBaLogPrio_Trace, "tag", "msg 2.1");
   log2->LogF(eBaLogPrio_Trace, "tagTag", "msg 2.%i", 2);

   for (int i = 100; i < 2000; ++i) {
      log2->LogF(eBaLogPrio_Trace, "taTaTaTa", "msg 2.%i", i);
      std::cout << i << std::endl;
      if (i % 300 == 0) {
         BaCoreSleep(1);
      }
   }

   CBaLog *log3 = CBaLog::Create("TestLog3", "", 40, 2, 0);
   log3->Log(eBaLogPrio_Trace, "tag", "LOOOOOOOOOOOOOOOOOOOOOOOOOOOONG msg 3.1");
   log3->LogF(eBaLogPrio_Trace, "tag", "msg 3.%i", 2);



   CBaLog::Delete(log2);
   CBaLog::Delete(log3);


}

/* ****************************************************************************/
/*  ...
 */
void CBaLogTest::FromCfg() {
   //
}
