/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaComTest.h
 *   Date     : Dec 7, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BACOMTEST_H_
#define BACOMTEST_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
class CBaComTest : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(CBaComTest);

//   CPPUNIT_TEST(Bus1W);
//   CPPUNIT_TEST(Config);
//   CPPUNIT_TEST(Serial);

   CPPUNIT_TEST_SUITE_END();

public:
   void Bus1W();
   void Config();
   void Serial();
};

#endif // BACOMTEST_H_
