/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaseApiTest.cpp
 *   Date     : 23/06/2015
 *------------------------------------------------------------------------------
 *   Module description:
 */

#include <iostream>
#include <stdio.h>

#include "cppunit/TestRunner.h"
#include "cppunit/TestResult.h"
#include "cppunit/TestResultCollector.h"
#include "cppunit/CompilerOutputter.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "BaGenMacros.h"
#include "ProgressListener.h"

#include "TestTemplate.h"
#include "BaCoreTest.h"
#include "BaGpioTest.h"
#include "BaComTest.h"
#include "BaLogTest.h"
#include "BaIniParseTest.h"

//
enum TTestSelection {
   eSingleTests   = 0x1,
   eSingleSuites  = 0x2,
   eFullRegistry  = 0x4,
};

static TTestSelection sSelection =
//      eSingleTests;
//      eSingleSuites;
      eFullRegistry;


LOCAL CPPUNIT_NS::TestSuite* AddSuites(CPPUNIT_NS::TestSuite* pSuite);
LOCAL CPPUNIT_NS::TestSuite* AddTests(CPPUNIT_NS::TestSuite* pSuite);


/* ***************************************************************************/
/*  ...
**/
int main() {
   setbuf(stdout, 0); // this disables buffering for stdout.

   CPPUNIT_NS::ProgressListener progressListener;
   CPPUNIT_NS::TestResult result;
   CPPUNIT_NS::TestRunner testRunner;
   CPPUNIT_NS::TestResultCollector resultCollector;
   CPPUNIT_NS::TestSuite  *pSuite = new CPPUNIT_NS::TestSuite( "SingleTests" );
   CPPUNIT_NS::CompilerOutputter compOut(&resultCollector, std::cout, "%p:%l:");

   result.addListener(&progressListener);
   result.addListener(&resultCollector);

   switch (sSelection) {
      case eSingleTests:
         testRunner.addTest(AddTests(pSuite));
         break;
      case eSingleSuites:
         testRunner.addTest(AddSuites(pSuite));
         break;
      case eFullRegistry:
         /* no break */
      default:
         testRunner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
         break;
   }

   testRunner.run(result);
   compOut.write();

	return 0;
}

/* ***************************************************************************/
/*  ...
**/
LOCAL CPPUNIT_NS::TestSuite* AddSuites(CPPUNIT_NS::TestSuite* pSuite) {
//   pSuite->addTest(CTestTemplate::suite());
//   pSuite->addTest(CBaComTest::suite());
//   pSuite->addTest(CBaLogTest::suite());
//   pSuite->addTest(CBaGpioTest::suite());
//   pSuite->addTest(CBaCoreTest::suite());
   pSuite->addTest(CBaIniParse::suite());
   return pSuite;
}

/* ***************************************************************************/
/*  ...
**/
LOCAL CPPUNIT_NS::TestSuite* AddTests(CPPUNIT_NS::TestSuite* pSuite) {
   pSuite->addTest( new CPPUNIT_NS::TestCaller<CBaCoreTest>("ThreadsSpecialCases", &CBaCoreTest::ThreadsSpecialCases));
   return pSuite;
}

