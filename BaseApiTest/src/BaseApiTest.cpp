/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaseApiTest.cpp
 *   Date     : May 24, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#include <iostream>
#include "BaseApiTest.h"
#include "BaseApi.h"
#include "BaLogMacros.h"
#include "BaGenMacros.h"
#include "BaLog.h"

// Todo: delete
#include "OsProc.h"

#define TAG "BaTst"

TOsProcCtrlTaskOpts sOpts = {0};

CPPUNIT_TEST_SUITE_REGISTRATION( CBaseApiTest );

/* ****************************************************************************/
/*  ...
 */
void CBaseApiTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaseApiTest::tearDown() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaseApiTest::Init() {

}


/* ****************************************************************************/
/*  Test the general logger
 */
void CBaseApiTest::Logger() {
   TBaLogDesc bad = {0};

   // No logger set
   CPPUNIT_ASSERT(!TRACE_("test"));

   // Bad arguments
   CPPUNIT_ASSERT(!BaApiInitLoggerDef(0));
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(!BaApiInitLoggerDef(""));
   CPPUNIT_ASSERT(!TRACE_("test"));
   CPPUNIT_ASSERT(!BaApiInitLogger(bad));
   CPPUNIT_ASSERT(!TRACE_("test3"));

   // Good init
   CPPUNIT_ASSERT(BaApiInitLoggerDef("testDef"));
   CPPUNIT_ASSERT(WARN_("test2"));

   // Re-inits are always graceful and do nothing
   CPPUNIT_ASSERT(BaApiInitLoggerDef(0));
   CPPUNIT_ASSERT(BaApiInitLogger(bad));
   CPPUNIT_ASSERT(ERROR_("test2"));

   // Exit
   CPPUNIT_ASSERT(BaApiExitLogger());
   CPPUNIT_ASSERT(!TRACE_("test3"));

}

/* ****************************************************************************/
/*  ...
 */
void CBaseApiTest::Exit() {

}




