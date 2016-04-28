/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaMsgTest.h
 *   Date     : Apr 27, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/* * @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BAMSGTEST_H_
#define BAMSGTEST_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */

class CBaMsgTest : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(CBaMsgTest);

   CPPUNIT_TEST(Init);
   CPPUNIT_TEST(TestImpl);
   CPPUNIT_TEST(TestIface);
   CPPUNIT_TEST(Exit);

   CPPUNIT_TEST_SUITE_END();

public:
   void Init();
   void TestImpl();
   void TestIface();
   void Exit();
};

#endif // BAMSGTEST_H_

