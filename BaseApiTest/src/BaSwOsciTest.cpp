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
#include <unistd.h>
#include <iostream>
#include <fstream>
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
void CBaSwOsciTest::ConcreteNiceWeather() {
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
   // Let the thread flush it
   BaCoreSleep(1);

   // Re-sample
   d *= 2;
   llu *= 2;
   usi *= 2;
   c *= 2;
   ASS(p->Sample());
   ASS(CBaSwOsci::Destroy(p));

   // Test the header
   std::ifstream iS(RESPATH "BaSwOsciTest.csv");
   ASS(iS.good());
   std::string s;
   std::getline(iS, s, ','); // Header
   ASS(s == "Time");
   std::getline(iS, s, ',');
   ASS(s == " Timestamp");
   std::getline(iS, s, ',');
   ASS(s == " d");
   std::getline(iS, s, ',');
   ASS(s == " llu");
   std::getline(iS, s, ',');
   ASS(s == " usi");
   std::getline(iS, s);
   ASS(s == " c");

   // Test the first line
   std::getline(iS, s, ',');
   std::getline(iS, s, ',');
   std::getline(iS, s, ',');
#ifdef __WIN32
   ASS(s == " 3.14e-005");
#else
   ASS(s == " 3.14e-05");
#endif
   std::getline(iS, s, ',');
   ASS(s == " 999999999999");
   std::getline(iS, s, ',');
   ASS(s == " 300");
   std::getline(iS, s);
   ASS(s == " 56");

   // Test the second line
   std::getline(iS, s, ',');
   std::getline(iS, s, ',');
   std::getline(iS, s, ',');
#ifdef __WIN32
   ASS(s == " 6.28e-005");
#else
   ASS(s == " 6.28e-05");
#endif
   std::getline(iS, s, ',');
   ASS(s == " 1999999999998");
   std::getline(iS, s, ',');
   ASS(s == " 600");
   std::getline(iS, s);
   ASS(s == " 112");

}

/* ****************************************************************************/
/*  ...
 */
void CBaSwOsciTest::Interface() {
   IBaSwOsci *p = IBaSwOsciCreate("BaSwOsciTest", RESPATH, true);
   ASS(p);
   double d = 3.14e-5;
   uint64_t llu = 999999999999;
   uint16_t usi = 300;
   int8_t c = 56;

   ASS(p->Register(&d, eBaSwOsci_double, "d", "double"));
   ASS(p->Register(&llu, eBaSwOsci_uint64, "llu", "unsigned ll"));
   ASS(p->Register(&usi, eBaSwOsci_uint16, "usi", "unsigned s"));
   ASS(p->Register(&c, eBaSwOsci_int8, "c,", "signed char"));
   ASS(p->Sample());

   // Re-sample
   d *= 2;
   llu *= 2;
   usi *= 2;
   c *= 2;
   ASS(p->Sample());
   BaCoreMSleep(1);
   ASS(IBaSwOsciDestroy(p));

   // Test the header
   std::ifstream iS(RESPATH "BaSwOsciTest.csv");
   ASS(iS.good());
   std::string s;
   std::getline(iS, s, ','); // Header
   ASS(s == "Time");
   std::getline(iS, s, ',');
   ASS(s == " Timestamp");
   std::getline(iS, s, ',');
   ASS(s == " d");
   std::getline(iS, s, ',');
   ASS(s == " llu");
   std::getline(iS, s, ',');
   ASS(s == " usi");
   std::getline(iS, s);
   ASS(s == " c.");

   // Test the first line
   std::getline(iS, s, ',');
   std::getline(iS, s, ',');
   std::getline(iS, s, ',');
#ifdef __WIN32
   ASS(s == " 3.14e-005");
#else
   ASS(s == " 3.14e-05");
#endif
   std::getline(iS, s, ',');
   ASS(s == " 999999999999");
   std::getline(iS, s, ',');
   ASS(s == " 300");
   std::getline(iS, s);
   ASS(s == " 56");

   // Test the second line
   std::getline(iS, s, ',');
   std::getline(iS, s, ',');
   std::getline(iS, s, ',');
#ifdef __WIN32
   ASS(s == " 6.28e-005");
#else
   ASS(s == " 6.28e-05");
#endif
   std::getline(iS, s, ',');
   ASS(s == " 1999999999998");
   std::getline(iS, s, ',');
   ASS(s == " 600");
   std::getline(iS, s);
   ASS(s == " 112");

}

/* ****************************************************************************/
/*  ...
 */
void CBaSwOsciTest::Errors() {
   IBaSwOsci *p = IBaSwOsciCreate(0, RESPATH, true);
   ASS(!p);
   TBaSwOsciHdl h = BaSwOsciCreate("BaSwOsciTest", 0, eBaBool_false);
   ASS(h);
   ASS(BaSwOsciDestroy(h));

   std::cout << remove("BaSwOsciTest.csv") << ":";
   std::cout << errno << std::endl;

   ASS(!IBaSwOsciDestroy(0));
}

/* ****************************************************************************/
/*  Release resources
 */
void CBaSwOsciTest::Exit() {
   remove(RESPATH "BaSwOsciTest.csv");
   rmdir(RESPATH);
   BaApiExitLogger();
   BaLogDestroy(sLog.hdl, eBaBool_false);
}

