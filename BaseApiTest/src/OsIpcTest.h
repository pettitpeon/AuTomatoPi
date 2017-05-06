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
#ifndef OSIPCTEST_H_
#define OSIPCTEST_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */
class COsIpcTest : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(COsIpcTest);

   CPPUNIT_TEST(Init);
   CPPUNIT_TEST(VarRegNiceWeather);
   CPPUNIT_TEST(FunRegNiceWeather);
   CPPUNIT_TEST(FunRegErrors);
   CPPUNIT_TEST(IPCServer);
   CPPUNIT_TEST(IPCRealFunClientServer);
   CPPUNIT_TEST(Exit);

   CPPUNIT_TEST_SUITE_END();

public:
   void Init();
   void VarRegNiceWeather();
   void FunRegNiceWeather();
   void FunRegErrors();
   void IPCServer();
   void IPCRealFunClientServer();
   void IPCRealVarClientServer();
   void Exit();
};

#endif // OSIPCTEST_H_
