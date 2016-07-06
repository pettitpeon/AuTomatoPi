/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaProcTest.h
 *   Date     : Jun 28, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BAPROCTEST_H_
#define BAPROCTEST_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
class CBaProcTest : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(CBaProcTest);

   CPPUNIT_TEST(Init);
   CPPUNIT_TEST(OwnNames);
   CPPUNIT_TEST(CtrlTaskPID);
   CPPUNIT_TEST(PIDFiles);
   CPPUNIT_TEST(NameFromPID);

   CPPUNIT_TEST_SUITE_END();

public:
   void Init();
   void OwnNames();
   void CtrlTaskPID();
   void PIDFiles();
   void NameFromPID();
};

#endif // BAPROCTEST_H_



