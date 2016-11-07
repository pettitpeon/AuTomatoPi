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
#include "BaRPi.h"
#include "BaGenMacros.h"
#include "CppU.h"
#include "BaUtils.hpp"

#include "BaIpc.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


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

   char buf[1024] = {0};

   TBaBoolRC rc = BaIpcCreatePipeReader();
   rc = BaIpcCreatePipeWriter();

   rc = BaIpcReadPipe(&buf, 1024);

   /* write "Hi" to the FIFO */
   rc = BaIpcWritePipe("Hi", sizeof("Hi"));

   rc = BaIpcReadPipe(&buf, 1024);
   std::cout << buf << std::endl;
}


