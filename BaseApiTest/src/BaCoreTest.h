/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaCoreTest.h
 *   Date     : 14.10.2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BACORETEST_H_
#define BACORETEST_H_

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

class CBaCoreTest : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(CBaCoreTest);

   CPPUNIT_TEST(Init);
   CPPUNIT_TEST(SleepAndTiming);
   CPPUNIT_TEST(TimeStamps);
   CPPUNIT_TEST(ThreadsNiceWeather);
   CPPUNIT_TEST(ThreadsSpecialCases);
   CPPUNIT_TEST(Exit);

   CPPUNIT_TEST_SUITE_END();

public:
   void Init();
   void SleepAndTiming();
   void TimeStamps();
   void ThreadsNiceWeather();
   void ThreadsSpecialCases();
   void Exit();
};



#endif /* BACORETEST_H_ */

