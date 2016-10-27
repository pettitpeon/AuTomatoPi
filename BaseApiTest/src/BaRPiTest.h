/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaRPiTest.h
 *   Date     : Oct 27, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BARPITEST_H_
#define BARPITEST_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
class CBaRPiTest : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(CBaRPiTest);

   CPPUNIT_TEST(Test);

   CPPUNIT_TEST_SUITE_END();

public:
   void Test();
};

#endif // BARPITEST_H_
