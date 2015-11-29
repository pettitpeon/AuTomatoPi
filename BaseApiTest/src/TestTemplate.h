/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : TestTemplate.h
 *   Date     : 23/06/2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  Test template
 */
/*------------------------------------------------------------------------------
 */


#ifndef TESTTEMPLATE_H_
#define TESTTEMPLATE_H_

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestCase.h"

//#include "Template.h"


class CTestTemplate : public CppUnit::TestCase {
public:
   void setUp();
   void tearDown();

   CPPUNIT_TEST_SUITE(CTestTemplate);

   CPPUNIT_TEST(Test);

   CPPUNIT_TEST_SUITE_END();

protected:
   void Test();
};

#endif /* TestTemplate */
