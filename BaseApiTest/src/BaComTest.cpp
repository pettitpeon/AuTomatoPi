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
#include "CppU.h"

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
   BaFS::MkDir("C:\\tmp\\");
   BaFS::MkDir(DEVDIR);
   BaFS::MkDir(DEVDIR DEV1W1);
   BaFS::MkDir(DEVDIR DEV1W2);
   BaFS::MkDir(DEVDIR DEV1W3);
   {
      std::ofstream os(SENSOR1W(DEV1W1));
      // Real sensor example data
      os << "96 01 4b 46 7f ff 0c 10 a0 : crc=a0 YES" << std::endl;
      os << "96 01 4b 46 7f ff 0c 10 a0 t=25375" << std::endl;
   }
   // Copy to other 2 sensors
   std::cout << (BaFS::CpFile(SENSOR1W(DEV1W1), SENSOR1W(DEV1W2)) ? "OK" : "ER") << std::endl;
   std::cout << (BaFS::CpFile(SENSOR1W(DEV1W1), SENSOR1W(DEV1W3)) ? "OK" : "ER") << std::endl;
#endif
}

/* ****************************************************************************/
/*  ...
 */
void CBaComTest::I2c() {
   TBaBool err = 0;
   BaComI2CInit();
   BaComI2CSelectDev(72);
   printf("0x%04x, e:%i\n", BaComI2CReadReg16(0, &err), err);
   printf("0x%04x, e:%i\n", BaComI2CReadReg16(1, &err), err);
   printf("0x%04x, e:%i\n", BaComI2CReadReg16(2, &err), err);
   printf("0x%04x, e:%i\n", BaComI2CReadReg16(3, &err), err);
   printf("0x%04x, e:%i\n", BaComI2CReadReg16(4, &err), err);
   printf("0x%04x, e:%i\n", BaComI2CReadReg16(5, &err), err);
   printf("0x%04x, e:%i\n", BaComI2CReadReg16(6, &err), err);
   printf("0x%04x, e:%i\n", BaComI2CReadReg16(7, &err), err);
//   printf("0x%04x, e:%i\n", BaComI2CReadReg8(8, &err), err);

   BaComI2CExit();
}

/* ****************************************************************************/
/*  ...
 */
void CBaComTest::Bus1W() {
   TBaBool error1 = eBaBool_false;
   TBaBool error2 = eBaBool_false;
   TBaCoreMonTStampUs ts = 0;
   const char *pAsyncVal = 0;
   const char* out = 0;
   float temp = 0;
   ASS(BaCom1WInit());
   ASS(BaCom1WExit());

   // Uninitialized
   ASS(!BaCom1WRdAsync("28-0215c2c4bcff", &ts));
   ASS(!BaCom1WGetValue("28-0215c2c4bcff", rdDvr, 0));
   ASS_D_EQ(-300.0, BaCom1WGetTemp("28-0215c2c4bcff", 0), 0.001);


   ASS(BaCom1WInit());

   // Threads not ready
   ASS(!BaCom1WRdAsync("28-0215c2c4bcff", &ts));
   ASS(!BaCom1WRdAsync("28-0315c2c4bcff", 0));
   ASS(!BaCom1WRdAsync("28-0415c2c4bcff", &ts));

   // After sleeping threads are ready
   BaCoreMSleep(900);

// Meant for RPI
#ifdef __arm__
   pAsyncVal = BaCom1WRdAsync("28-0215c2c4bcff", &ts);
   temp      = BaCom1WGetTemp("28-0215c2c4bcff", &error1);
   temp      = BaCom1WGetTemp(0, &error1);
   temp      = BaCom1WGetTemp("28-xxx", &error2);
   ASS(!BaCom1WGetValue("28-xxx", rdDvr, 0));
   ASS(!BaCom1WGetValue("28-xxx", 0, 0));
   out = (const char*) BaCom1WGetValue("28-0215c2c4bcff", rdDvr, &error1);

// Meant for PC (Lin, Win)
#else
   // Test async reading
   // Only overwrite if no error
   pAsyncVal = BaCom1WRdAsync("28-0215c2c4bcff", &ts);
   pAsyncVal = pAsyncVal ? BaCom1WRdAsync("28-0315c2c4bcff", &ts) : 0;
   pAsyncVal = pAsyncVal ? BaCom1WRdAsync("28-0415c2c4bcff", &ts) : 0;

   // Test sync reading
   // No error
   temp = BaCom1WGetTemp("28-0215c2c4bcff", &error1);
   std::cout << temp << ": "<< (error1 ? "T" : "F") << std::endl;
   temp = BaCom1WGetTemp(0, &error1);
   std::cout << temp << ": "<< (error1 ? "T" : "F") << std::endl;
   out = (const char*) BaCom1WGetValue(0, rdDvr,  &error1);
   std::cout << out << ": "<< (error1 ? "T" : "F") << std::endl;
   free((void*)out);

   // Error
   temp = BaCom1WGetTemp("28-xxx", &error2);
   std::cout << temp << ": "<< (error2 ? "T" : "F") << std::endl;
#endif

   ASS(BaCom1WExit());

   // Test at the end so all functions are always called
   if (TEST1W) {
      ASS(pAsyncVal);
      ASS(!error1);
      ASS(error2);
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
   remove(SENSOR1W(DEV1W3));
   remove(SENSOR1W(DEV1W2));
   remove(SENSOR1W(DEV1W1));
   rmdir(DEVDIR DEV1W3);
   rmdir(DEVDIR DEV1W2);
   rmdir(DEVDIR DEV1W1);
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
