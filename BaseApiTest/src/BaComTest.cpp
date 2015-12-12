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

#include <iostream>
#include "BaComTest.h"
#include "BaCom.h"
#include "BaGpio.h"
#include "BaCore.h"
#include "BaGenMacros.h"

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

#endif // __linux
