/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaSwOsciTest.cpp
 *   Date     : Aug 15, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#include <iostream>
#include "BaSwOsciTest.h"
#include "impl/CBaSwOsci.h"
#include "BaGenMacros.h"
#include "BaseApi.h"
#include "CppU.h"
#include "BaUtils.hpp"
#include "BaLog.h"
#include "BaLogMacros.h"

#define TAG "SwOTest"

#ifdef _WIN32
# define RESPATH CPPU_RESPATH "BaSwOsciTest\\"
#else
# define RESPATH CPPU_RESPATH "BaSwOsciTest/"
#endif

static TBaLogDesc sLog;

CPPUNIT_TEST_SUITE_REGISTRATION( CBaSwOsciTest );

/* ****************************************************************************/
/*  ...
 */
void CBaSwOsciTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaSwOsciTest::tearDown() {
}

/* ****************************************************************************/
/*  Initialize resources
 */
void CBaSwOsciTest::Init() {
   Exit();
   BaFS::MkDir(RESPATH);
   BaApiExitLogger();
   TBaLogOptions lOpts;
   BaLogSetDefOpts(&lOpts);
   lOpts.name = "defTestLog";
   lOpts.out = eBaLogOut_Console;
   sLog.hdl = BaLogCreate(&lOpts);
   BaApiInitLogger(sLog);
   TRACE_("Init");
}

/* ****************************************************************************/
/*  ...
 */
void CBaSwOsciTest::Test() {
   CBaSwOsci *p = CBaSwOsci::Create("BaSwOsciTest", RESPATH, true);
   ASS(p);
   double d = 3.14e-5;
   uint64_t llu = 999999999999;
   uint16_t usi = 300;
   int8_t c = 56;

   ASS(p->Register(&d, eBaSwOsci_double, "d", "double"));
   ASS(p->Register(&llu, eBaSwOsci_uint64, "llu", "unsigned ll"));
   ASS(p->Register(&usi, eBaSwOsci_uint16, "usi", "unsigned s"));
   ASS(p->Register(&c, eBaSwOsci_int8, "c", "signed char"));

   ASS(p->Sample());
   d *= 2;
   llu *= 2;
   usi *= 2;
   c *= 2;

   BaCoreSleep(2);
   ASS(p->Sample());
   p->Flush();

   ASS(CBaSwOsci::Destroy(p));

}

/* ****************************************************************************/
/*  Release resources
 */
void CBaSwOsciTest::Exit() {
   remove(RESPATH "BaSwOsciTest.csv");
   rmdir(RESPATH);
   BaLogDestroy(sLog.hdl, eBaBool_false);
}

