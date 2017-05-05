/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : HwComTest.cpp
 *   Date     : Dec 7, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */

#ifndef __WIN32

#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>    // read/write usleep toDelete
#include <byteswap.h>
#include <linux/i2c.h>

#include "HwComTest.h"
#include <HwCom.h>
#include <HwGpio.h>
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
#define CONVREG 0
#define CONFREG 1
#define ADDRADS1115 72

LOCAL void* rdDvr(const char* str, size_t n);

CPPUNIT_TEST_SUITE_REGISTRATION( CHwComTest );


/* ****************************************************************************/
/*  ...
 */
void CHwComTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CHwComTest::tearDown() {
   HwCom1WExit();
}

/* ****************************************************************************/
/*  ...
 */
void CHwComTest::init() {
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
void CHwComTest::I2c() {
   TBaBool err = 0;
   uint16_t readReg = 0;
   uint16_t confReg = 0;

   ASS(HwComI2CInit());
   ASS(HwComI2CSelectDev(ADDRADS1115));

   // Test functions
   uint64_t funcs = HwComI2CFuncs();
   ASS(funcs & I2C_FUNC_SMBUS_WRITE_I2C_BLOCK);
   ASS(funcs & I2C_FUNC_SMBUS_READ_I2C_BLOCK  );
   ASS(funcs & I2C_FUNC_SMBUS_WRITE_BLOCK_DATA);
   ASS(!(funcs & I2C_FUNC_SMBUS_READ_BLOCK_DATA));
   ASS(funcs & I2C_FUNC_SMBUS_PROC_CALL      );
   ASS(funcs & I2C_FUNC_SMBUS_WRITE_WORD_DATA);
   ASS(funcs & I2C_FUNC_SMBUS_READ_WORD_DATA );
   ASS(funcs & I2C_FUNC_SMBUS_WRITE_BYTE_DATA);
   ASS(funcs & I2C_FUNC_SMBUS_READ_BYTE_DATA );
   ASS(funcs & I2C_FUNC_SMBUS_WRITE_BYTE     );
   ASS(funcs & I2C_FUNC_SMBUS_READ_BYTE      );
   ASS(funcs & I2C_FUNC_SMBUS_QUICK          );
   ASS(!(funcs & I2C_FUNC_SMBUS_BLOCK_PROC_CALL));
   // ...
   ASS(!(funcs & I2C_FUNC_SMBUS_BLOCK_PROC_CALL));
   ASS(!(funcs & I2C_FUNC_NOSTART));
   ASS(funcs & I2C_FUNC_SMBUS_PEC          );
   ASS(!(funcs & I2C_FUNC_PROTOCOL_MANGLING));
   ASS(!(funcs & I2C_FUNC_10BIT_ADDR));
   ASS(funcs & I2C_FUNC_I2C          );

   // ADS1115: http://www.ti.com/lit/ds/symlink/ads1113.pdf
   // Configuration register
   //                Byte 0    Byte 1
   //               7654 3210 5432 1098
   // 0x83C0:       1000 0011 1100 0000: 33728
   //  5 Data rate -^^^| |||| |||| ||||
   //  4 Comp Mode ----+ |||| |||| ||||
   //  3 Comp Pol -------+||| |||| ||||
   //  2 Comp Lat --------+|| |||| ||||
   //  0 Comp Q -----------++ |||| ||||
   // 15 Op Status -----------+||| ||||
   // 14 Mux ------------------+++ ||||
   //  9 Gain ---------------------+++|
   //  8 Op mode ---------------------+

   // Continuous Mode
   confReg = 0b1000001111000000;
   ASS(HwComI2CWriteReg16(CONFREG, confReg));
   readReg = HwComI2CReadReg16(CONFREG, &err);
   ASS_EQ(readReg, (uint16_t)0x8340);
   ASS(!err);
   printf("0x%04X, e:%i\n", readReg, err);

   // Voltage read
   BaCoreMSleep(5);
   readReg = bswap_16(HwComI2CReadReg16(CONVREG, &err));
   // Same as original register, but without status flag
   ASS(!err);
   printf("%f V\n", 6.144*readReg/32767.0);
   BaCoreMSleep(5);
   // Byte order must be swapped to get the right int
   readReg = bswap_16(HwComI2CReadReg16(CONVREG, &err));
   printf("%f V\n", 6.144*readReg/32767.0);
   ASS(!err);

   // One Shot Mode
   confReg = 0b1000001111000001;
   ASS(HwComI2CWriteReg16(CONFREG, confReg));
   readReg = HwComI2CReadReg16(CONFREG, &err);
   ASS(!err);
   printf("0x%04X, e:%i\n", readReg, err);

   // voltage read
   BaCoreMSleep(5);
   // Byte order must be swapped to get the right int
   readReg = bswap_16(HwComI2CReadReg16(CONVREG, &err));
   ASS(!err);
   printf("%f V, 0x%04x\n", 6.144*readReg/32767.0, readReg);
   ASS_EQ(uint8_t(readReg >> 8), HwComI2CRead8(&err));
   ASS(!err);
   ASS_EQ(uint8_t(readReg >> 8), HwComI2CReadReg8(CONVREG, &err));
   ASS(!err);

   ASS(HwComI2CExit());
}

/* ****************************************************************************/
/*  ...
 */
void CHwComTest::I2cError() {
   TBaBool err = eBaBool_false;
   HwComI2CRead8(&err);
   ASS(err);
   err = eBaBool_false;
   HwComI2CReadReg8(0, &err);
   ASS(err);
   err = eBaBool_false;
   HwComI2CReadReg16(0, &err);
   ASS(err);
   err = eBaBool_false;

   ASS(!HwComI2CWrite8(0));
   ASS(!HwComI2CWriteReg8(0, 0));
   ASS(!HwComI2CWriteReg16(0, 0));

   ASS(HwComI2CSelectDev(ADDRADS1115));
   HwComI2CRead8(&err);
   ASS(!err);
   ASS(HwComI2CExit());
}

/* ****************************************************************************/
/*  ...
 */
void CHwComTest::Bus1W() {
   TBaBool error1 = eBaBool_false;
   TBaBool error2 = eBaBool_false;
   TBaCoreMonTStampUs ts = 0;
   const char *pAsyncVal = 0;
   const char* out = 0;
   float temp = 0;
   ASS(HwCom1WInit());
   ASS(HwCom1WExit());

   // Uninitialized
   ASS(!HwCom1WRdAsync("28-0215c2c4bcff", &ts));
   ASS(!HwCom1WGetValue("28-0215c2c4bcff", rdDvr, 0));
   ASS_D_EQ(-300.0, HwCom1WGetTemp("28-0215c2c4bcff", 0), 0.001);


   ASS(HwCom1WInit());

   // Threads not ready
   ASS(!HwCom1WRdAsync("28-0215c2c4bcff", &ts));
   ASS(!HwCom1WRdAsync("28-0315c2c4bcff", 0));
   ASS(!HwCom1WRdAsync("28-0415c2c4bcff", &ts));

   // After sleeping threads are ready
   BaCoreMSleep(900);

// Meant for RPI
#ifdef __arm__
   pAsyncVal = HwCom1WRdAsync("28-0215c2c4bcff", &ts);
   temp      = HwCom1WGetTemp("28-0215c2c4bcff", &error1);
   temp      = HwCom1WGetTemp(0, &error1);
   temp      = HwCom1WGetTemp("28-xxx", &error2);
   ASS(!HwCom1WGetValue("28-xxx", rdDvr, 0));
   ASS(!HwCom1WGetValue("28-xxx", 0, 0));
   out = (const char*) HwCom1WGetValue("28-0215c2c4bcff", rdDvr, &error1);

// Meant for PC (Lin, Win)
#else
   // Test async reading
   // Only overwrite if no error
   pAsyncVal = HwCom1WRdAsync("28-0215c2c4bcff", &ts);
   pAsyncVal = pAsyncVal ? HwCom1WRdAsync("28-0315c2c4bcff", &ts) : 0;
   pAsyncVal = pAsyncVal ? HwCom1WRdAsync("28-0415c2c4bcff", &ts) : 0;

   // Test sync reading
   // No error
   temp = HwCom1WGetTemp("28-0215c2c4bcff", &error1);
   std::cout << temp << ": "<< (error1 ? "T" : "F") << std::endl;
   temp = HwCom1WGetTemp(0, &error1);
   std::cout << temp << ": "<< (error1 ? "T" : "F") << std::endl;
   out = (const char*) HwCom1WGetValue(0, rdDvr,  &error1);
   std::cout << out << ": "<< (error1 ? "T" : "F") << std::endl;
   free((void*)out);

   // Error
   temp = HwCom1WGetTemp("28-xxx", &error2);
   std::cout << temp << ": "<< (error2 ? "T" : "F") << std::endl;
#endif

   ASS(HwCom1WExit());

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
void CHwComTest::Serial() {
   THwComSerHdl hdl = HwComSerInit(HWCOM_SERIALDEV, eHwComBaud_115200);
   CPPUNIT_ASSERT(hdl);

   int delay = 11;
   for (int count = 0 ; count < 50 ; delay++) {
      if (delay > 10) {
         printf ("\nOut: %3d: ", count) ;
         fflush (stdout) ;
         HwComSerPutC(hdl, count) ;
         ++count;
         delay = 0;
      }

      BaCoreMSleep(3) ;

      while (HwComSerPend(hdl)) {
         printf (" -> %3d", HwComSerGetC(hdl));
         fflush (stdout) ;
      }
   }

   printf ("\n") ;
   CPPUNIT_ASSERT(HwComSerExit(hdl));
}

/* ****************************************************************************/
/*  ...
 */
void CHwComTest::Config() {
   CPPUNIT_ASSERT(true);
}

#endif // __linux

/* ****************************************************************************/
/*  ...
 */
void CHwComTest::exit() {
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

// linux/i2c.h
// Bit ADS115 Macro
// 00: 1      I2C_FUNC_SMBUS_WRITE_I2C_BLOCK SMBus write_i2c_block_data comman
// 01: 1      I2C_FUNC_SMBUS_READ_I2C_BLOCK
// 02: 1      I2C_FUNC_SMBUS_WRITE_BLOCK_DATA
// 03: 0      I2C_FUNC_SMBUS_READ_BLOCK_DATA
// 04: 1      I2C_FUNC_SMBUS_PROC_CALL
// 05: 1      I2C_FUNC_SMBUS_WRITE_WORD_DATA
// 06: 1      I2C_FUNC_SMBUS_READ_WORD_DATA
// 07: 1      I2C_FUNC_SMBUS_WRITE_BYTE_DATA
// 08: 1      I2C_FUNC_SMBUS_READ_BYTE_DATA
// 09: 1      I2C_FUNC_SMBUS_WRITE_BYTE
// 10: 1      I2C_FUNC_SMBUS_READ_BYTE
// 11: 1      I2C_FUNC_SMBUS_QUICK
// 12: 0      I2C_FUNC_SMBUS_BLOCK_PROC_CALL
// ...
// 22: 0      I2C_FUNC_SMBUS_BLOCK_PROC_CALL
// 23: 0      I2C_FUNC_NOSTART
// 24: 1      I2C_FUNC_SMBUS_PEC
// 25: 0      I2C_FUNC_PROTOCOL_MANGLING
// 26: 0      I2C_FUNC_10BIT_ADDR
// 27: 1      I2C_FUNC_I2C


#endif // __WIN32

