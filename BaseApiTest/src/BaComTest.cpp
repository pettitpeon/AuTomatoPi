/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaComTest.cpp
 *   Date     : Dec 7, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */

#ifdef __linux

#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "BaComTest.h"
#include "BaCom.h"
#include "BaGpio.h"
#include "BaCore.h"
#include "BaGenMacros.h"

#define TEST1W true

LOCAL void* rdDvr(const char* str, size_t n);

CPPUNIT_TEST_SUITE_REGISTRATION( CBaComTest );

/* ****************************************************************************/
/*  ...
 */
void CBaComTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaComTest::tearDown() {
   BaCom1WExit();
}

/* ****************************************************************************/
/*  ...
 */
void CBaComTest::Bus1W() {
   TBaBool noError = eBaBool_false;
   TBaBool error = eBaBool_false;
   float temp = 0;
   CPPUNIT_ASSERT(BaCom1WInit());

   // No error
   temp = BaCom1WGetTemp("28-0215c2c4bcff", &noError);
   std::cout << temp << ": "<< (noError ? "T" : "F") << std::endl;
   temp = BaCom1WGetTemp(0, &noError);
   std::cout << temp << ": "<< (noError ? "T" : "F") << std::endl;
   const char* out = 0;
   out = (const char*) BaCom1WGetValue(28, 0, rdDvr,  &noError);
   std::cout << out << ": "<< (noError ? "T" : "F") << std::endl;
   free((void*)out);

   // Error
   temp = BaCom1WGetTemp("28-xxx", &error);
   std::cout << temp << ": "<< (error ? "T" : "F") << std::endl;

   CPPUNIT_ASSERT(BaCom1WExit());

   // Test at the end so all functions are always called
   if (TEST1W) {
      CPPUNIT_ASSERT(!noError);
      CPPUNIT_ASSERT(error);
   }
}

/* ****************************************************************************/
/*  ...
 */
void CBaComTest::Serial() {
   TBaComSerHdl hdl = BaComSerInit(BACOM_SERIALDEV, eBaComBaud_115200);
   CPPUNIT_ASSERT(hdl);

   int delay = 11;
   for (int count = 0 ; count < 50 ; delay++) {
      if (delay > 10) {
         printf ("\nOut: %3d: ", count) ;
         fflush (stdout) ;
         BaComSerPutC(hdl, count) ;
         ++count;
         delay = 0;
      }

      BaCoreMSleep(3) ;

      while (BaComSerPend(hdl)) {
         printf (" -> %3d", BaComSerGetC(hdl));
         fflush (stdout) ;
      }
   }

   printf ("\n") ;
   CPPUNIT_ASSERT(BaComSerExit(hdl));
}

/* ****************************************************************************/
/*  ...
 */
void CBaComTest::Config() {
   CPPUNIT_ASSERT(true);
}

LOCAL void* rdDvr(const char* str, size_t n) {
   char * out = (char *) malloc(n);
   strncpy(out, str, n);
   return out;
}

#endif // __linux
