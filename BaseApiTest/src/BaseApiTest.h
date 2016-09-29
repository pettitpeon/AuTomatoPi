/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaseApiTest.h
 *   Date     : May 24, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BASEAPITEST_H_
#define BASEAPITEST_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
class CBaseApiTest : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(CBaseApiTest);

   CPPUNIT_TEST(Init);
   CPPUNIT_TEST(ControlTask);
   CPPUNIT_TEST(LongControlTask);
   CPPUNIT_TEST(Logger);
   CPPUNIT_TEST(Exit);

   CPPUNIT_TEST_SUITE_END();

public:
   void Init();
   void ControlTask();
   void LongControlTask();
   void Logger();
   void Exit();
};

#endif // BASEAPITEST_H_
