/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CtrlPT1Test.h
 *   Date     : Aug 12, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef CTRLPT1TEST_H_
#define CTRLPT1TEST_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
class CCtrlPT1Test : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(CCtrlPT1Test);

   CPPUNIT_TEST(Cpp);
   CPPUNIT_TEST(C);

   CPPUNIT_TEST_SUITE_END();

public:
   void Cpp();
   void C();
};

#endif // CTRLPT1TEST_H_
