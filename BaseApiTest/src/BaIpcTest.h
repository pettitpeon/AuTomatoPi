/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaIpcTest.h
 *   Date     : 12.03.2017
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BAIPCTEST_H_
#define BAIPCTEST_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
class CBaIpcTest : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(CBaIpcTest);

   CPPUNIT_TEST(FunRegNiceWeather);
   CPPUNIT_TEST(FunRegErrors);
   //CPPUNIT_TEST(IPC);

   CPPUNIT_TEST_SUITE_END();

public:
   void FunRegNiceWeather();
   void FunRegErrors();
   void IPC();
};

#endif // BAIPCTEST_H_
