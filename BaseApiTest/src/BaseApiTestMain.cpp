/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaseApiTestMain.cpp
 *   Date     : 23/06/2015
 *------------------------------------------------------------------------------
 *   Module description:
 */

#include <OsProc.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <chrono>

#include "cppunit/TestRunner.h"
#include "cppunit/TestResult.h"
#include "cppunit/TestResultCollector.h"
#include "cppunit/CompilerOutputter.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "BaGenMacros.h"
#include "ProgressListener.h"
#include "TestTemplate.h"

#include "BaCoreTest.h"
#include "BaLogTest.h"
#include "BaIniParseTest.h"
#include "BaMsgTest.h"
#include "BaseApiTest.h"
#include "BaSwOsciTest.h"
#include "BaUtilsTest.h"

#include "CtrlPT1Test.h"

#include "HwComTest.h"
#include "HwGpioTest.h"
#include "HwPiTest.h"
#include "OsIpcRegistryTest.h"
#include "OsIpcTest.h"
#include "OsProcTest.h"

//
enum TTestSelection {
   eSingleTests   = 0x1,
   eSingleSuites  = 0x2,
   eFullRegistry  = 0x4,
};

// Global variable with current working directory
char gCWD[1024];

static TTestSelection sSelection =
//      eSingleTests;
//      eSingleSuites;
      eFullRegistry;


LOCAL CPPUNIT_NS::TestSuite* AddSuites(CPPUNIT_NS::TestSuite* pSuite);
LOCAL CPPUNIT_NS::TestSuite* AddTests(CPPUNIT_NS::TestSuite* pSuite);


/* ***************************************************************************/
/*  ...
**/
int main(int argc, char* argv[]) {
   auto start = std::chrono::steady_clock::now();

   // This disables buffering for stdout.
   setbuf(stdout, 0);

#ifdef _NDEBUG
   std::cout << "Debugging mode" << std::endl;
#endif

   // Print arguments
   for(int i = 0; i < argc; i++) {
      std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
      std::cout << "Full Name: " << OsProcGetOwnFullName() << std::endl;
   }

   // Get the working directory
   gCWD[1024 - 1] = 0;
   std::cout << "CWD: " << getcwd(gCWD, 1024) << std::endl;

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
   std::chrono::duration<double> durSec = std::chrono::steady_clock::now() - start;

   std::cout << "Duration: " << durSec.count() << " s" <<std::endl;
	return 0;
}

/* ***************************************************************************/
/*  ...
**/
LOCAL CPPUNIT_NS::TestSuite* AddSuites(CPPUNIT_NS::TestSuite* pSuite) {
//   pSuite->addTest(CTestTemplate::suite());
//   pSuite->addTest(CBaLogTest::suite());
//   pSuite->addTest(CBaCoreTest::suite());
//   pSuite->addTest(CBaIniParse::suite());
//   pSuite->addTest(CBaMsgTest::suite());
//   pSuite->addTest(CBaseApiTest::suite());
//   pSuite->addTest(CBaUtilsTest::suite());
//   pSuite->addTest(COsProcTest::suite());
//   pSuite->addTest(CCtrlPT1Test::suite());
//   pSuite->addTest(CBaSwOsciTest::suite());
//   pSuite->addTest(CHwComTest::suite());
//   pSuite->addTest(CHwGpioTest::suite());
//   pSuite->addTest(CHwPiTest::suite());
   pSuite->addTest(COsIpcTest::suite());
   pSuite->addTest(COsIpcRegistryTest::suite());
   pSuite->addTest(COsProcTest::suite());
   return pSuite;
}

/* ***************************************************************************/
/*  ...
**/
LOCAL CPPUNIT_NS::TestSuite* AddTests(CPPUNIT_NS::TestSuite* pSuite) {
   pSuite->addTest( new CPPUNIT_NS::TestCaller<CBaLogTest>("Init", &CBaLogTest::Init));
   pSuite->addTest( new CPPUNIT_NS::TestCaller<CBaLogTest>("FilesAndSizesOpts", &CBaLogTest::FilesAndSizesOpts));
   pSuite->addTest( new CPPUNIT_NS::TestCaller<CBaLogTest>("Exit", &CBaLogTest::Exit));
   return pSuite;
}


