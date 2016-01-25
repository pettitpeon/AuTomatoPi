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
#include "BaLog.h"
#include "impl/CBaLog.h"


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

   CBaLog *log1 = CBaLog::Create("TestLog1");
   log1->Log("msg 1.1");
   log1->Logf("msg 1.%i", 2);
   log1->Logf("msg 1.%i", 3);
   log1->Logf("msg 1.%i", 4);
   log1->Logf("msg 1.%i", 5);


   CBaLog *log2 = CBaLog::Create("TestLog2");
   log2->Log("msg 2.1");
   log2->Logf("msg 2.%i", 2);

   CBaLog *log3 = CBaLog::Create("TestLog3");
   log3->Log("LOOOOOOOOOOOOOOOOOOOOOOOOOOOONG msg 3.1");
   log3->Logf("msg 3.%i", 2);

   CBaLog::Delete(log1);
   CBaLog::Delete(log2);
   CBaLog::Delete(log3);


}
