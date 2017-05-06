/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : OsProcTest.h
 *   Date     : Jun 28, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef OSPROCTEST_H_
#define OSPROCTEST_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
class COsProcTest : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(COsProcTest);

   CPPUNIT_TEST(Init);
   CPPUNIT_TEST(OwnNames);
   CPPUNIT_TEST(CtrlTaskPID);
   CPPUNIT_TEST(PIDFiles);
   CPPUNIT_TEST(NameFromPID);
   CPPUNIT_TEST(Prio);

   CPPUNIT_TEST(ControlTask);
   CPPUNIT_TEST(LongControlTask);

   CPPUNIT_TEST(Exit);

   CPPUNIT_TEST_SUITE_END();

public:
   void Init();
   void OwnNames();
   void CtrlTaskPID();
   void PIDFiles();
   void NameFromPID();
   void Prio();

   void ControlTask();
   void LongControlTask();
   void Exit();
};

#endif // OSPROCTEST_H_



