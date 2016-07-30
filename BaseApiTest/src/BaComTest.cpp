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

#if 1

#include <string.h>
#include <stdlib.h>
#include <iostream>

#include "BaComTest.h"
#include "BaCom.h"
#include "BaGpio.h"
#include "BaCore.h"
#include "BaGenMacros.h"
#include "dbg/BaDbgMacros.h"
#include "BaUtils.hpp"

#ifdef __WIN32
# define DEVDIR "C:\\tmp\\devices\\"
# define DEV1W1 "28-0215c2c4bcff"
# define DEV1W2 "28-0315c2c4bcff"
# define DEV1W3 "28-0415c2c4bcff"
# define SENSOR1W(dev) DEVDIR dev "\\w1_slave"
#else
# define DEVDIR "/bus/devices/"
# define DEV1W1 "28-0215c2c4bcff"
# define DEV1W2 "28-0315c2c4bcff"
# define DEV1W3 "28-0415c2c4bcff"
# define SENSOR1W(dev) DEVDIR dev "/w1_slave"
#endif
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
void CBaComTest::init() {
#ifndef __arm__
   BaFS::MkDir(DEVDIR);
   BaFS::MkDir(DEVDIR DEV1W1);
   BaFS::MkDir(DEVDIR DEV1W2);
   BaFS::MkDir(DEVDIR DEV1W3);
   std::ofstream os(SENSOR1W(DEV1W1));

   // Real sensor example data
   os << "96 01 4b 46 7f ff 0c 10 a0 : crc=a0 YES" << std::endl;
   os << "96 01 4b 46 7f ff 0c 10 a0 t=25375" << std::endl;

   // Copy to other 2 sensors
   BaFS::CpFile(SENSOR1W(DEV1W1), SENSOR1W(DEV1W2));
   BaFS::CpFile(SENSOR1W(DEV1W1), SENSOR1W(DEV1W3));
#endif
}

/* ****************************************************************************/
/*  ...
 */
void CBaComTest::Bus1W() {

   TBaBool yesError = eBaBool_false;
   TBaBool error = eBaBool_false;
   const char *pAsyncVal = 0;

   float temp = 0;
   CPPUNIT_ASSERT(BaCom1WInit());
   CPPUNIT_ASSERT(BaCom1WExit());
   CPPUNIT_ASSERT(BaCom1WInit());

   CPPUNIT_ASSERT(!BaCom1WRdAsync(28, "28-0215c2c4bcff"));
   CPPUNIT_ASSERT(!BaCom1WRdAsync(28, "28-0315c2c4bcff"));
   CPPUNIT_ASSERT(!BaCom1WRdAsync(28, "28-0415c2c4bcff"));
   BaCoreMSleep(900);

   // todo: moretest missing
   CPPUNIT_ASSERT(BaCom1WStopAsyncThread(28, "28-0215c2c4bcff"));

   // Only overwrite if no error
   pAsyncVal = BaCom1WRdAsync(28, "28-0215c2c4bcff");
   pAsyncVal = pAsyncVal ? BaCom1WRdAsync(28, "28-0315c2c4bcff") : 0;
   pAsyncVal = pAsyncVal ? BaCom1WRdAsync(28, "28-0415c2c4bcff") : 0;

   // No error
   temp = BaCom1WGetTemp("28-0215c2c4bcff", &yesError);
   std::cout << temp << ": "<< (yesError ? "T" : "F") << std::endl;
   temp = BaCom1WGetTemp(0, &yesError);
   std::cout << temp << ": "<< (yesError ? "T" : "F") << std::endl;
   const char* out = 0;
   out = (const char*) BaCom1WGetValue(28, 0, rdDvr,  &yesError);
   std::cout << out << ": "<< (yesError ? "T" : "F") << std::endl;
   free((void*)out);

   // Error
   temp = BaCom1WGetTemp("28-xxx", &error);
   std::cout << temp << ": "<< (error ? "T" : "F") << std::endl;

   CPPUNIT_ASSERT(BaCom1WExit());

   // Test at the end so all functions are always called
   if (TEST1W) {
      CPPUNIT_ASSERT(pAsyncVal);
      CPPUNIT_ASSERT(!yesError);
      CPPUNIT_ASSERT(error);
   }
}
#ifdef __linux
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

#endif // __linux

/* ****************************************************************************/
/*  ...
 */
void CBaComTest::exit() {
#ifndef __arm__
   remove(SENSOR1W(DEV1W1));
   remove(SENSOR1W(DEV1W2));
   remove(SENSOR1W(DEV1W3));
   rmdir(DEVDIR DEV1W1);
   rmdir(DEVDIR DEV1W3);
   rmdir(DEVDIR DEV1W2);
   rmdir(DEVDIR);
#endif
}

//
LOCAL void* rdDvr(const char* str, size_t n) {
   char * out = (char *) malloc(n);
   strncpy(out, str, n);
   return out;
}


#endif // __linux
