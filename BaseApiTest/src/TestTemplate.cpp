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
#include "impl/CBaIpcSvr.h"
#include "BaLogMacros.h"

#include "BaIpc.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TAG "tplTst"

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
   TBaLogOptions opt = {0};
   BaLogSetDefOpts(&opt);
   opt.out = eBaLogOut_Console;
   opt.name = "Tmpl";

   TBaLogDesc log = {0};
   log.pLog = IBaLogCreate(&opt);

   BaApiInitLogger(log);


   TRACE_("Hello test template");

   TRACE_("svr(%i)", BaIpcInitSvr());

   BaCoreMSleep(500);
   TRACE_("clnt(%i)", BaIpcInitClnt());

   BaIpcWritePipe(0,0,0);

   BaCoreMSleep(500);

//   char buf[1024] = {0};
//
//   TBaBoolRC rc = BaIpcCreatePipeReader();
//   rc = BaIpcCreatePipeWriter();

//   rc = BaIpcReadPipe(&buf, 1024);
//
//   /* write "Hi" to the FIFO */
//   rc = BaIpcWritePipe("Hi", sizeof("Hi"));
//
//   rc = BaIpcReadPipe(&buf, 1024);
//   std::cout << buf << std::endl;
}



