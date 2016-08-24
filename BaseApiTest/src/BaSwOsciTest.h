/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaSwOsciTest.h
 *   Date     : Aug 15, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BASWOSCITEST_H_
#define BASWOSCITEST_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
class CBaSwOsciTest : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(CBaSwOsciTest);

   CPPUNIT_TEST(Init);
   CPPUNIT_TEST(ConcreteNiceWeather);
   CPPUNIT_TEST(Interface);
   CPPUNIT_TEST(Errors);
   CPPUNIT_TEST(Exit);

   CPPUNIT_TEST_SUITE_END();

public:
   void Init();
   void ConcreteNiceWeather();
   void Interface();
   void Errors();
   void Exit();
};

#endif // BASWOSCITEST_H_
