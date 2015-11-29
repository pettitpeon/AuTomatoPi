/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaGpioTest.h
 *   Date     : 19.10.2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  Test template
 */
/*------------------------------------------------------------------------------
 */


#ifndef BAGPIOTEST_H_
#define BAGPIOTEST_H_

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

class CBaGpioTest : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(CBaGpioTest);

   CPPUNIT_TEST(HardwarePWM);
   CPPUNIT_TEST(SoftwarePWM);
   CPPUNIT_TEST(NormalOperations);

   CPPUNIT_TEST_SUITE_END();

protected:
   void HardwarePWM();
   void SoftwarePWM();
   void NormalOperations();
};

#endif /* BAGPIOTEST_H_ */
