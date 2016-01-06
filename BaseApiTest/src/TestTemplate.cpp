/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : TestTemplate.cpp
 *   Date     : 23/06/2015
 *------------------------------------------------------------------------------
 *   Module description:
 */

#include <iostream>
#include "TestTemplate.h"

#include <BaIniParse.h>
#include "BaGenMacros.h"
#include "BaCore.h"


CPPUNIT_TEST_SUITE_REGISTRATION( CTestTemplate );


/* ****************************************************************************/
/*  ...
 */
void CTestTemplate::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CTestTemplate::tearDown() {
}

/* ****************************************************************************/
/*  ...
 */
void CTestTemplate::Test() {
   std::cout << "Hello test template\n";
}


