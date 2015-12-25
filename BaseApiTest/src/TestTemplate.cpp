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
#include "BaGenMacros.h"
#include "BaCore.h"

#include "iniparser.h"

CPPUNIT_TEST_SUITE_REGISTRATION( CTestTemplate );

static void create_example_ini_file(void);

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

/* ****************************************************************************/
/*  ...
 */
void CTestTemplate::Config() {
//   dictionary *dd = iniparser_load("res\\twisted.ini");
//   iniparser_dump_ini(dd, stdout);
//   iniparser_dump(dd, stdout);

   IBaIniParser *pPars = BaIniParserCreate("res\\example.ini");

   dictionary  *   ini ;

   /* Some temporary variables to hold query results */
   int             b ;
   int             i ;
   double          d ;
   const char  *   s ;

   create_example_ini_file();

   ini = iniparser_load("res\\example.ini");
//   if (ini==NULL) {
//       fprintf(stderr, "cannot parse file: %s\n", "res\\example.ini");
//       return ;
//   }
   iniparser_dump(ini, stdout);

   pPars->DumpSecLess(stdout);
   pPars->DumpIniSec("pizza", stdout);

   /* Get pizza attributes */
   printf("Pizza:\n");


   b = iniparser_getboolean(ini, "pizza:ham", -1);
   printf("Ham:       [%d][%d]\n", b, pPars->GetBool("pizza:ham", -1));
   b = iniparser_getboolean(ini, "pizza:mushrooms", -1);
   printf("Mushrooms: [%d]\n", b);
   b = iniparser_getboolean(ini, "pizza:capres", -1);
   printf("Capres:    [%d]\n", b);
   b = iniparser_getboolean(ini, "pizza:cheese", -1);
   printf("Cheese:    [%d][%d]\n", b, pPars->GetBool("pizza:cheese", -1));

   /* Get wine attributes */
   printf("Wine:\n");
   s = iniparser_getstring(ini, "wine:grape", NULL);
   printf("Grape:     [%s]\n", s ? s : "UNDEF");

   i = iniparser_getint(ini, "wine:year", -1);
   printf("Year:      [%d]\n", i);

   s = iniparser_getstring(ini, "wine:country", NULL);
   printf("Country:   [%s]\n", s ? s : "UNDEF");

   d = iniparser_getdouble(ini, "wine:alcohol", -1.0);
   printf("Alcohol:   [%g]\n", d);

   iniparser_freedict(ini);
   return;


}

static void create_example_ini_file(void)
{
    FILE    *   ini ;

    if ((ini=fopen("res\\example.ini", "w"))==NULL) {
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
    "Year      = 1989 ;\n"
    "Country   = Spain ;\n"
    "Alcohol   = 12.5  ;\n"
    "\n");
    fclose(ini);
}
