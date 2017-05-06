/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaIpcRegistryTest.h
 *   Date     : 19.04.2017
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef OSIPCREGISTRYTEST_H_
#define OSIPCREGISTRYTEST_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
class COsIpcRegistryTest : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(COsIpcRegistryTest);

   CPPUNIT_TEST(Init);
   CPPUNIT_TEST(CppInterfaceFunRegistry);
   CPPUNIT_TEST(CInterfaceFunRegistry);
   CPPUNIT_TEST(LocalFunRegistry);

   CPPUNIT_TEST(CppInterfaceVarRegistry);
   CPPUNIT_TEST(CInterfaceVarRegistry);
   CPPUNIT_TEST(LocalVarRegistry);
   CPPUNIT_TEST(Exit);

   CPPUNIT_TEST_SUITE_END();

public:
   void Init();
   void CppInterfaceFunRegistry();
   void CInterfaceFunRegistry();
   void LocalFunRegistry();
   void CppInterfaceVarRegistry();
   void CInterfaceVarRegistry();
   void LocalVarRegistry();
   void Exit();
};

#endif // OSIPCREGISTRYTEST_H_
