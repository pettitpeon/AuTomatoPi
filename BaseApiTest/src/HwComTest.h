/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : HwComTest.h
 *   Date     : Dec 7, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef HWCOMTEST_H_
#define HWCOMTEST_H_

#ifndef __WIN32

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
class CHwComTest : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(CHwComTest);
   CPPUNIT_TEST(init);
   CPPUNIT_TEST(I2c);
   CPPUNIT_TEST(I2cError);
   CPPUNIT_TEST(Bus1W);
   CPPUNIT_TEST(Config);
   CPPUNIT_TEST(Serial);
   CPPUNIT_TEST(exit);

   CPPUNIT_TEST_SUITE_END();

public:
   void init();
   void I2c();
   void I2cError();
   void Bus1W();
   void Config();
   void Serial();
   void exit();
};

#endif // __WIN32
#endif // HWCOMTEST_H_
