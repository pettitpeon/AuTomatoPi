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
#ifndef BAIPCREGISTRYTEST_H_
#define BAIPCREGISTRYTEST_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
class CBaIpcRegistryTest : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(CBaIpcRegistryTest);

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

#endif // BAIPCREGISTRYTEST_H_
