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
#include "BaUtils.hpp"
#include "Extras/iniparser.h"
#include "Extras/dictionary.h"

#ifdef _WIN32
# define RESPATH   "res\\BaIniParseTest\\"
#else
# define RESPATH "res/BaIniParseTest/"
#endif

#define EXINI      RESPATH "example.ini"
#define TWISTEDINI RESPATH "twisted.ini"
#define ERRORINI   RESPATH "twisted-errors.ini"
#define OFKEYINI   RESPATH "twisted-ofkey.ini"
#define OFVALINI   RESPATH "twisted-ofval.ini"

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
   IBaIniParser *pHdl = CBaIniParserCreate(EXINI);
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
   TBaIniParseHdl hdl = BaIniParseCreate(EXINI);
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

/* ****************************************************************************/
/*  ...
 */
void CBaIniParse::TwistedIni() {

   IBaIniParser *pNewHdl = 0;
   dictionary   *pOrgHdl = 0;
   std::string   tmp;

   CPPUNIT_ASSERT(!CBaIniParserCreate("You/Shall/Not/Path"));
   CPPUNIT_ASSERT(!iniparser_load("You/Shall/Not/Path"));
   CPPUNIT_ASSERT(!CBaIniParserCreate(ERRORINI));
   CPPUNIT_ASSERT(!iniparser_load(ERRORINI));

   CPPUNIT_ASSERT(!iniparser_load(OFKEYINI));
   CPPUNIT_ASSERT(!CBaIniParserCreate(OFKEYINI));


   CPPUNIT_ASSERT(!CBaIniParserCreate(OFVALINI));
   CPPUNIT_ASSERT(!iniparser_load(OFVALINI));

   pNewHdl = CBaIniParserCreate(TWISTEDINI);
   pOrgHdl = iniparser_load(TWISTEDINI);
   CPPUNIT_ASSERT(pNewHdl);
   CPPUNIT_ASSERT(pOrgHdl);

   pNewHdl->Dump(stdout);
   std::cout << "a==============================================================\n\n";
   iniparser_dump(pOrgHdl, stdout);
   std::cout << "a==============================================================\n\n";

   pNewHdl->DumpIni(stdout);
   std::cout << "a==============================================================\n\n";
   iniparser_dump_ini(pOrgHdl, stdout);


   CPPUNIT_ASSERT_EQUAL(
         (bool) iniparser_find_entry(pOrgHdl, "open["),
         pNewHdl->Exists("open["));

   tmp = pNewHdl->GetString("multi line key", "");
   CPPUNIT_ASSERT_MESSAGE(pNewHdl->GetString("multi line key", ""),
         tmp == iniparser_getstring(pOrgHdl,"multi line key", ""));

   CPPUNIT_ASSERT(CBaIniParserDestroy(pNewHdl));
   iniparser_freedict(pOrgHdl);
   pNewHdl = 0;
   pOrgHdl = 0;

   pNewHdl = CBaIniParserCreate(0);
   pOrgHdl = dictionary_new(10);
   CPPUNIT_ASSERT(pNewHdl);
   CPPUNIT_ASSERT(pOrgHdl);

   CPPUNIT_ASSERT_EQUAL(pNewHdl->Set(0, 0), !iniparser_set(pOrgHdl, 0, 0));

   CPPUNIT_ASSERT_EQUAL(pNewHdl->Set("section", 0),
         !iniparser_set(pOrgHdl, "section", 0));

   CPPUNIT_ASSERT_EQUAL(pNewHdl->Set("section:key", "value"),
         !iniparser_set(pOrgHdl, "section:key", "value"));

   tmp = pNewHdl->GetString("section:key", 0);
   CPPUNIT_ASSERT_MESSAGE(tmp, tmp == iniparser_getstring(pOrgHdl, "section:key", 0));
   /* reset the key's value*/
   CPPUNIT_ASSERT_EQUAL(pNewHdl->Set("section:key", 0),
         !iniparser_set(pOrgHdl, "section:key", 0));

   tmp = iniparser_getstring(pOrgHdl, "section:key", "dummy") ?
         iniparser_getstring(pOrgHdl, "section:key", "dummy") : "";
   CPPUNIT_ASSERT_MESSAGE(tmp, tmp == pNewHdl->GetString("section:key", "dummy"));
   CPPUNIT_ASSERT_EQUAL(pNewHdl->Set("section:key", "value"),
         !iniparser_set(pOrgHdl, "section:key", "value"));

   tmp = pNewHdl->GetString("section:key", 0);
   CPPUNIT_ASSERT_MESSAGE(tmp, tmp == iniparser_getstring(pOrgHdl, "section:key", 0));

   iniparser_unset(pOrgHdl, "section:key");
   pNewHdl->Reset("section:key");

   tmp = pNewHdl->GetString("section:key",  "dummy");
   CPPUNIT_ASSERT_MESSAGE(tmp, tmp == iniparser_getstring(pOrgHdl, "section:key",  "dummy"));

   CPPUNIT_ASSERT_EQUAL(pNewHdl->Set("section:key", 0), !iniparser_set(pOrgHdl, "section:key", 0));
   CPPUNIT_ASSERT_EQUAL(pNewHdl->Set("section:key1", 0), !iniparser_set(pOrgHdl, "section:key1", 0));
   CPPUNIT_ASSERT_EQUAL(pNewHdl->Set("section:key2", 0), !iniparser_set(pOrgHdl, "section:key2", 0));


}

LOCAL void create_example_ini_file() {
    FILE *ini ;

    if ((ini = fopen(EXINI, "w")) == NULL) {
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
