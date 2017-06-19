/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaIniParseTest.h
 *   Date     : Dec 9, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BAINIPARSETEST_H_
#define BAINIPARSETEST_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
class CBaIniParse : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(CBaIniParse);

   CPPUNIT_TEST(CPPTest);
   CPPUNIT_TEST(CTest);
   CPPUNIT_TEST(TwistedIni);

   CPPUNIT_TEST_SUITE_END();

public:
   void CPPTest();
   void CTest();
   void TwistedIni();
   void CreateErrors();
};

#endif // BAINIPARSETEST_H_


