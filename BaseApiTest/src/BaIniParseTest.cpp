/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaIniParseTest.cpp
 *   Date     : Dec 9, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/*  @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#include <iostream>
#include "BaGenMacros.h"
#include "BaIniParseTest.h"
#include "BaIniParse.h"

LOCAL void create_example_ini_file(void);

CPPUNIT_TEST_SUITE_REGISTRATION( CBaIniParse );


/* ****************************************************************************/
/*  ...
 */
void CBaIniParse::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaIniParse::tearDown() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaIniParse::CPPTest() {
   CPPUNIT_ASSERT(true);
   create_example_ini_file();
   IBaIniParser *pHdl = CBaIniParserCreate("res\\example.ini");
   CPPUNIT_ASSERT(pHdl);

   pHdl->Dump(stdout);
   pHdl->DumpIni(stdout);

   CPPUNIT_ASSERT(pHdl->GetBool("pizza:ham", false));
   CPPUNIT_ASSERT_EQUAL(pHdl->GetInt("pizza:capres", 1), 0);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(pHdl->GetDouble("wine:alcohol", 0.0), 12.5, 0.0001);
   CPPUNIT_ASSERT_MESSAGE(pHdl->GetString("extra", "BAD"), pHdl->GetString("extra", "BAD") == "glass");

   // Look for bad entry and check default
   CPPUNIT_ASSERT_MESSAGE(pHdl->GetString(":", "BAD"), pHdl->GetString(":", "BAD") == "BAD");
}

/* ****************************************************************************/
/*  ...
 */
void CBaIniParse::CTest() {
   CPPUNIT_ASSERT(true);
   create_example_ini_file();
   TBaIniParseHdl hdl = BaIniParseCreate("res\\example.ini");
   CPPUNIT_ASSERT(hdl);

   BaIniParseDump(hdl, stdout);
   BaIniParseDumpIni(hdl, stdout);

   CPPUNIT_ASSERT(BaIniParseGetBool(hdl, "pizza:ham", false));
   CPPUNIT_ASSERT_EQUAL(BaIniParseGetInt(hdl, "pizza:capres", 1), 0);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(BaIniParseGetDouble(hdl, "wine:alcohol", 0.0), 12.5, 0.0001);
   std::string tmp = BaIniParseGetString(hdl, "extra", "BAD");

   CPPUNIT_ASSERT_MESSAGE(BaIniParseGetString(hdl, "extra", "BAD"), tmp == "glass");

   // Look for bad entry and check default
   tmp = BaIniParseGetString(hdl, ":", "BAD");
   CPPUNIT_ASSERT_MESSAGE(BaIniParseGetString(hdl, ":", "BAD"), tmp == "BAD");
}

LOCAL void create_example_ini_file() {
    FILE *ini ;

    if ((ini = fopen("res\\example.ini", "w")) == NULL) {
        fprintf(stderr, "iniparser: cannot create example.ini\n");
        return ;
    }

    fprintf(ini,
    "#\n"
    "# This is an example of ini file\n"
    "#\n"
    "\n"
    "Extra     = glass ;\n"
    "Extra2    = plates ;\n"
    "[Pizza]\n"
    "\n"
    "Ham       = yes ;\n"
    "Mushrooms = TRUE ;\n"
    "Capres    = 0 ;\n"
    "Cheese    = Non ;\n"
    "\n"
    "\n"
    "[Wine]\n"
    "\n"
    "Grape     = Cabernet Sauvignon ;\n"
    "Year      = 1989;\n"
    "Country   = Spain ;\n"
    "Alcohol   = 12.5  ;\n"
    "\n");
    fclose(ini);
}
