/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaLogTest.h
 *   Date     : Dec 9, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BALOGTEST_H_
#define BALOGTEST_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
class CBaLogTest : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();
   void startSuite() {};


   CPPUNIT_TEST_SUITE(CBaLogTest);

   CPPUNIT_TEST(Init);
   CPPUNIT_TEST(Test);
   CPPUNIT_TEST(CreateReuseDestroy);
   CPPUNIT_TEST(Tags);
   CPPUNIT_TEST(Prios);
   CPPUNIT_TEST(LogVsPrint);
   CPPUNIT_TEST(FilesAndSizesOpts);
   CPPUNIT_TEST(Stress);
   CPPUNIT_TEST(Exit);

   CPPUNIT_TEST_SUITE_END();

public:
   void Init();
   void Test();
   void CreateReuseDestroy();
   void Tags();
   void Prios();
   void LogVsPrint();
   void FilesAndSizesOpts();
   void Stress();
   void SysLog();
   void Exit();
};

#endif // BALOGTEST_H_


