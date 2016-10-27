/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaCom.cpp
 *   Date     : Nov 30, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/*------------------------------------------------------------------------------
 */
#ifndef __WIN32

#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <mutex>


#ifdef __linux
# include <sys/ioctl.h>
# include <linux/i2c-dev.h>
# include <linux/i2c.h>
# include <termios.h>
# include <linux/spi/spidev.h>
#elif __WIN32
 typedef uint32_t speed_t;
#endif

#include "BaCom.h"
#include "BaCore.h"
#include "BaGpio.h"
#include "BaGenMacros.h"
#include "BaLogMacros.h"
#include "BaUtils.hpp"
#include "BaGpioPinout.h"

#define ERRTEMP -300
#define FOREVER  500
#define TAG "BaCom"

#ifdef __WIN32
# define DEVPATH  "C:\\tmp\\devices\\"
#else
# define DEVPATH  "/sys/bus/w1/devices/"
#endif

#define W1TEMPFAM 28


 // I2C definitions
//#define I2C_SLAVE 0x0703
//#define I2C_SMBUS 0x0720   /* SMBus-level access */
//#define I2C_SMBUS_READ  1
//#define I2C_SMBUS_WRITE 0
// // SMBus transaction types
////#define I2C_SMBUS_QUICK           0
//#define I2C_SMBUS_BYTE            1
//#define I2C_SMBUS_BYTE_DATA       2
//#define I2C_SMBUS_WORD_DATA       3
//#define I2C_SMBUS_PROC_CALL       4
//#define I2C_SMBUS_BLOCK_DATA      5
//#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
//#define I2C_SMBUS_BLOCK_PROC_CALL   7     /* SMBus 2.0 */
//#define I2C_SMBUS_I2C_BLOCK_DATA    8
// // SMBus messages
//#define I2C_SMBUS_BLOCK_MAX   32 /* As specified in SMBus standard */
//#define I2C_SMBUS_I2C_BLOCK_MAX  32 /* Not specified but we use same structure */

// Serial descriptor
typedef struct TSerialDesc {
   int fd;
   IBaGpio *pTXD0;
   IBaGpio *pRXD0;

   TSerialDesc() : fd(0), pTXD0(0), pRXD0(0) {
      pTXD0 = IBaGpioCreate(SERTXD0);
      pRXD0 = IBaGpioCreate(SERRXD0);
   }
   ~TSerialDesc() {
      IBaGpioDelete(pTXD0);
      IBaGpioDelete(pRXD0);
   }
} TSerialDesc;

// 1W descriptor
typedef struct T1wDev {
   std::ifstream* pIs;
   std::string serNo;
   std::string actVal;
   bool valid; // Flag to check that the value is valid
   bool run; // to run the async thread. This is more like a pause flag
   TBaCoreThreadHdl updThread;
   TBaCoreThreadArg threadArg;
   uint32_t heartBeat;
   TBaCoreMonTStampUs ts;

   T1wDev() : pIs(0), serNo(""), actVal(""), valid(false), run(false), updThread(0),
         threadArg{0}, heartBeat(0), ts(0) {
      threadArg.pArg = this;
   }
} T1wDev;

// A map of pointers to vectors of device pointers. Careful with mem mgmt.
typedef std::map<uint16_t, std::vector<T1wDev*>* > T1WDevs;

// I2C structures
typedef struct i2c_smbus_ioctl_data TI2cData;

typedef union i2c_smbus_data UI2cData;

//typedef union i2c_smbus_data {
//  uint8_t  byte;
//  uint16_t word;
//  uint8_t  block[I2C_SMBUS_BLOCK_MAX + 2]; // block [0] is used for length + one more for PEC
//} UI2cData;

// I2C descriptor
typedef struct TI2cDesc {
   int fd;
   std::mutex mtx;
   IBaGpio *pSDA = 0;
   IBaGpio *pSCL = 0;
   TI2cDesc() : fd(0), pSDA(0), pSCL(0) {}
} TI2cDesc;

LOCAL inline speed_t srBaud2Speed(EBaComBaud baud);

// One wire bus local functions
LOCAL inline float   w1ReadTemp(const char *dvrStr, TBaBool *pError);
LOCAL inline TBaBool w1ReadDevice(T1wDev* pDev, std::string &contents);
LOCAL inline void    w1RdAsyncRout(TBaCoreThreadArg *pArg);
LOCAL inline uint8_t w1GetFamId(std::string serNo);
LOCAL inline T1wDev* w1GetFirstFamDev(uint8_t famID);
LOCAL inline T1wDev* w1GetDev(const char* serNo);
LOCAL inline int i2cAccess(int fd, uint8_t rw, uint8_t cmd, int size, i2c_smbus_data *data);

// GPIO for the 1w bus. default GPIO 4, pin 7
static IBaGpio *sp1w = 0;

// Map of family IDs and devices vector
static T1WDevs s1WDevs;

// The SPI bus parameters
// Variables as they need to be passed as pointers later on

const static char       *spiDev0  = "/dev/spidev0.0" ;
const static char       *spiDev1  = "/dev/spidev0.1" ;
const static uint8_t     spiBPW   = 8 ;
const static uint16_t    spiDelay = 0 ;
static uint32_t    spiSpeeds [2] ;
static int         spiFds [2] ;

// I2C variables
static TI2cDesc sI2cHdl;

//
TBaBoolRC BaComI2CInit() {
   if (sI2cHdl.fd) {
      return eBaBoolRC_Success;
   }

   std::lock_guard<std::mutex> lck(sI2cHdl.mtx);

   EBaPiModel mod = BaPiGetBoardModel();
   const char* dev = mod < eBaPiModel2 ? "/dev/i2c-0" : "/dev/i2c-1";

   sI2cHdl.pSDA = IBaGpioCreate(I2CSDA1);
   if(!sI2cHdl.pSDA) {
      return eBaBoolRC_Error;
   }

   sI2cHdl.pSCL = IBaGpioCreate(I2CSCL1);
   if(!sI2cHdl.pSCL) {
      IBaGpioDelete(sI2cHdl.pSDA);
      sI2cHdl.pSDA = 0;
      return eBaBoolRC_Error;
   }

   sI2cHdl.pSDA->SetAlt(0);
   sI2cHdl.pSCL->SetAlt(0);

   if ((sI2cHdl.fd = open(dev, O_RDWR)) < 0) {
      WARN_("Unable to open I2C device: %s", strerror(errno));
      sI2cHdl.fd = 0;
      IBaGpioDelete(sI2cHdl.pSDA);
      sI2cHdl.pSDA = 0;
      IBaGpioDelete(sI2cHdl.pSCL);
      sI2cHdl.pSCL = 0;
      return eBaBoolRC_Error;
   }

   return eBaBoolRC_Success;
}

//
TBaBoolRC BaComI2CExit() {
   if (!sI2cHdl.fd) {
      return eBaBoolRC_Success;
   }

   std::lock_guard<std::mutex> lck(sI2cHdl.mtx);
   if (close((int) sI2cHdl.fd) < 0) {
      WARN_("Unable to close I2C device: %s", strerror(errno));
      sI2cHdl.fd = 0;
      return eBaBoolRC_Error;
   }

   sI2cHdl.fd = 0;
   IBaGpioDelete(sI2cHdl.pSDA);
   sI2cHdl.pSDA = 0;
   IBaGpioDelete(sI2cHdl.pSCL);
   sI2cHdl.pSCL = 0;
   return eBaBoolRC_Success;
}

TBaBoolRC BaComI2CSelectDev(uint16_t devAddr) {
   if (!sI2cHdl.fd) {
      if (!BaComI2CInit()) {
         return eBaBoolRC_Error;
      }
   }

   std::lock_guard<std::mutex> lck(sI2cHdl.mtx);
   if (ioctl (sI2cHdl.fd, I2C_SLAVE, devAddr) < 0) {
      WARN_("Unable to select I2C device(%x): %s", devAddr, strerror(errno));
      return eBaBoolRC_Error;
   }

   return eBaBoolRC_Success;
}

//
// linux/i2c.h
// Bit Macro
// 00: I2C_FUNC_SMBUS_WRITE_I2C_BLOCK
// 01: I2C_FUNC_SMBUS_READ_I2C_BLOCK
// 02: I2C_FUNC_SMBUS_WRITE_BLOCK_DATA
// 03: I2C_FUNC_SMBUS_READ_BLOCK_DATA
// 04: I2C_FUNC_SMBUS_PROC_CALL
// 05: I2C_FUNC_SMBUS_WRITE_WORD_DATA
// 06: I2C_FUNC_SMBUS_READ_WORD_DATA
// 07: I2C_FUNC_SMBUS_WRITE_BYTE_DATA
// 08: I2C_FUNC_SMBUS_READ_BYTE_DATA
// 09: I2C_FUNC_SMBUS_WRITE_BYTE
// 10: I2C_FUNC_SMBUS_READ_BYTE
// 11: I2C_FUNC_SMBUS_QUICK
// 12: I2C_FUNC_SMBUS_BLOCK_PROC_CALL
// ...
// 22: I2C_FUNC_SMBUS_BLOCK_PROC_CALL
// 23: I2C_FUNC_NOSTART
// 24: I2C_FUNC_SMBUS_PEC
// 25: I2C_FUNC_PROTOCOL_MANGLING
// 26: I2C_FUNC_10BIT_ADDR
// 27: I2C_FUNC_I2C
uint64_t BaComI2CFuncs() {
   if (!sI2cHdl.fd) {
      return BaComI2CInit();
   }

   uint64_t funcs = 0;
   if (ioctl (sI2cHdl.fd, I2C_FUNCS, &funcs) < 0) {
      WARN_("Unable to check funcs I2C device: %s", strerror(errno));
      return eBaBoolRC_Error;
   }

   return funcs;
}

//
uint8_t BaComI2CRead8(TBaBool *pError) {
   if (!sI2cHdl.fd) {
      if (pError) {
         *pError = eBaBool_true;
      }
      return 0;
   }

   UI2cData data = {0};
   if(i2cAccess(sI2cHdl.fd, I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data) < 0) {
      if (pError) {
         *pError = eBaBool_true;
      }

      // todo: implement a msg with state here?
      return 0;
   }

   return data.byte;
}

//
uint8_t BaComI2CReadReg8(uint32_t reg, TBaBool *pError) {
   if (!sI2cHdl.fd) {
      if (pError) {
         *pError = eBaBool_true;
      }
      return 0;
   }

   UI2cData data = {0};
   if(i2cAccess(sI2cHdl.fd, I2C_SMBUS_READ, reg, I2C_SMBUS_BYTE_DATA, &data) < 0) {
      if (pError) {
         *pError = eBaBool_true;
      }
      return 0;
   }

   return data.byte;
}

//
uint16_t BaComI2CReadReg16(uint32_t reg, TBaBool *pError) {
   if (!sI2cHdl.fd) {
      if (pError) {
         *pError = eBaBool_true;
      }
      return 0;
   }

   UI2cData data = {0};
   if(i2cAccess(sI2cHdl.fd, I2C_SMBUS_READ, reg, I2C_SMBUS_WORD_DATA, &data) < 0) {
      if (pError) {
         *pError = eBaBool_true;
      }
      return 0;
   }

   return data.word;
}

//
TBaBoolRC BaComI2CWrite8(uint8_t val) {
   if (!sI2cHdl.fd) {
      return eBaBoolRC_Error;
   }

   UI2cData data = {0};
   data.word = val;

   if(i2cAccess(sI2cHdl.fd, I2C_SMBUS_WRITE, 0, I2C_SMBUS_BYTE, &data) < 0) {
      return eBaBoolRC_Error;
   }

   return eBaBoolRC_Success;
}

//
TBaBoolRC BaComI2CWriteReg8(uint32_t reg, uint8_t val) {
   if (!sI2cHdl.fd) {
      return eBaBoolRC_Error;
   }

   UI2cData data = {0};
   data.word = val;

   if(i2cAccess(sI2cHdl.fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_BYTE_DATA, &data) < 0) {
      return eBaBoolRC_Error;
   }

   return eBaBoolRC_Success;
}

//
TBaBoolRC BaComI2CWriteReg16(uint32_t reg, uint16_t val) {
   if (!sI2cHdl.fd) {
      return eBaBoolRC_Error;
   }

   UI2cData data = {0};
   data.word = val;

   if(i2cAccess(sI2cHdl.fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_WORD_DATA, &data) < 0) {
      return eBaBoolRC_Error;
   }

   return eBaBoolRC_Success;
}

//
TBaBoolRC BaComI2CWriteRegBlock(uint32_t reg, uint8_t *pBuf, uint32_t size) {
   if (!sI2cHdl.fd) {
      return eBaBoolRC_Error;
   }

   UI2cData data = {0};
   memcpy(data.block, pBuf, size);

   if(i2cAccess(sI2cHdl.fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_BLOCK_DATA, &data) < 0) {
      return eBaBoolRC_Error;
   }

   return eBaBoolRC_Success;
}

#ifdef __linux
//
TBaComHdl BaComSPIInit() {
   int fd ;
   int channel;
   int speed;
   int mode;

   mode    &= 3 ;  // Mode is 0, 1, 2 or 3
   channel &= 1 ;  // Channel is 0 or 1

   if ((fd = open (channel == 0 ? spiDev0 : spiDev1, O_RDWR)) < 0)
//     return wiringPiFailure (WPI_ALMOST, "Unable to open SPI device: %s\n", strerror (errno)) ;

   spiSpeeds [channel] = speed ;
   spiFds    [channel] = fd ;

 // Set SPI parameters.

   if (ioctl (fd, SPI_IOC_WR_MODE, &mode)            < 0)
//     return wiringPiFailure (WPI_ALMOST, "SPI Mode Change failure: %s\n", strerror (errno)) ;

   if (ioctl (fd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW) < 0)
//     return wiringPiFailure (WPI_ALMOST, "SPI BPW Change failure: %s\n", strerror (errno)) ;

   if (ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed)   < 0)
//     return wiringPiFailure (WPI_ALMOST, "SPI Speed Change failure: %s\n", strerror (errno)) ;

   return fd ;
   return 0;
}
#endif

//
TBaBoolRC BaComSPIExit(TBaComHdl hdl) {
   return 0;
}

//
TBaBoolRC BaCom1WInit() {
   if (sp1w) {
      return eBaBoolRC_Success;
   }

   // This reserves the GPIO in the c++ interface
   sp1w = IBaGpioCreate(BUS1W);
   if (!sp1w) {
      return eBaBoolRC_Error;
   }

   // This reserves the GPIO in the C interface
   sp1w->SetAlt(0);
   BaCom1WGetDevices();

   return eBaBoolRC_Success;
}

//
TBaBoolRC BaCom1WExit() {
   if (!sp1w) {
      return eBaBoolRC_Success;
   }
   TBaBoolRC rc = eBaBoolRC_Success;

   // Iterate the map of vectors
   for (auto kv : s1WDevs) {
      if (!kv.second) {
         continue;
      }

      // Tell all threads to exit so they can start exiting in parallel
      for (auto pDev : *kv.second) {
         if (pDev && pDev->pIs) {

            // The exit flag should not delete anything! see w1RdAsyncRout()
            if (pDev->updThread) {
               pDev->threadArg.exitTh = eBaBool_true;
            }
         }
      }

      // Destroy all threads
      for (auto pDev : *kv.second) {
         if (pDev) {
            if (BaCoreDestroyThread(pDev->updThread, FOREVER)) {
               // Only here is sure that the thread ended
               pDev->updThread = 0;
               delete pDev;
            } else {
               TBaCoreThreadInfo info = {0};
               BaCoreGetThreadInfo(pDev->updThread, &info);
               if(!WARN_("Th:%s(%i) did not exit correctly", info.name, info.tid)) {
                  BASYSLOG(TAG, "Th:%s(%i) did not exit correctly", info.name, info.tid);
               }
               rc = false;
            }

         }
      }

      // Delete the vector of erased devices
      delete kv.second;
      kv.second = 0;

   }

   // Clear the map of device families
   s1WDevs.clear();

   // Delete the GPIO. Zero it first so nobody can access it "in between"
   IBaGpio *tmp = sp1w;
   sp1w = 0;

   // Bitwise & desired!
   return rc & IBaGpioDelete(tmp);
}

//
uint16_t BaCom1WGetDevices(){
   if (!BaFS::Exists(DEVPATH)) {
      return eBaBoolRC_Error;
   }

   // directory entry
   struct dirent *de = 0;
   DIR *d = 0;
   uint16_t famId = 0;

   d = opendir(DEVPATH);
   if (!d) {
      return 0;
   }

   uint16_t i = 0;
   for (de = readdir(d); de != 0; de = readdir(d)) {

      // Skip "." and ".."
      if ((strcmp(de->d_name, "..") == 0) || (strcmp(de->d_name, ".") == 0)) {
         continue;
      }

      // Get open devices with a valid family ID in a input file stream
      // eg. 28-0215c2c4bcff [devFam]-[devID]
      // The information is under /sys/bus/w1/devices/28-0215c2c4bcff/w1_slave
      std::stringstream ss;
      ss << de->d_name;

      // If the famId is successfully extracted...
      if (ss >> famId) {

         T1wDev *pDev = new T1wDev;
         pDev->serNo = de->d_name;
         pDev->pIs = new std::ifstream();

         std::string path = DEVPATH + std::string(de->d_name) + "/w1_slave";

         pDev->pIs->open(path);
         if(!pDev->pIs->fail()) {
            // This is a map of family IDs and file streams
            // push_back() copies the object
            pDev->updThread = BaCoreCreateThread("name", w1RdAsyncRout,
                  &pDev->threadArg, eBaCorePrio_Low);

            if (!s1WDevs[famId]) {
               s1WDevs[famId] = new std::vector<T1wDev*>;
            }

            s1WDevs[famId]->push_back(pDev);
            i++;
         } else {
            delete pDev->pIs;
            pDev->pIs = 0;
         }
      }
   }
   return i;
}

//
const char* BaCom1WRdAsync(const char *serNo, TBaCoreMonTStampUs *pTs) {
   if(!sp1w || s1WDevs.empty()) {
      return 0;
   }

   // If no serNo, the first device is used
   T1wDev *pDev = w1GetDev(serNo);
   if (pDev) {
      pDev->run = true;
      if (pDev->valid) {

         // Copy the timestamp
         if (pTs) {
            *pTs = pDev->ts;
         }
         return pDev->actVal.c_str();
      }
   }

   return 0;
}

//
TBaBoolRC BaCom1WPauseAsyncThread(const char *serNo) {
   if(!sp1w || s1WDevs.empty()) {
      return eBaBoolRC_Error;
   }

   // If no serNo, the first device is used
   T1wDev *pDev = w1GetDev(serNo);
   if (!pDev) {
      return eBaBoolRC_Error;
   }

   pDev->run = false;
   return eBaBoolRC_Success;
}

//
float BaCom1WGetTemp(const char* serNo, TBaBool *pError) {
   TBaBool terror;
   pError = pError ? pError : &terror;
   std::string contents;

   if (!sp1w || s1WDevs.empty()) {
      *pError = eBaBoolRC_Error;
      return ERRTEMP;
   }

   T1wDev *pDev = serNo ? w1GetDev(serNo) : w1GetFirstFamDev(W1TEMPFAM);

   if (!w1ReadDevice(pDev, contents)) {
      *pError = eBaBool_true;
      return ERRTEMP;
   }

   return w1ReadTemp(contents.c_str(), pError);
}

//
void *BaCom1WGetValue(const char* serNo, TBaCom1wReadFun cb, TBaBool *pError) {
   TBaBool tError;
   pError = pError ? pError : &tError;
   std::string contents;

   if (!cb || !sp1w || s1WDevs.empty()) {
      *pError = eBaBoolRC_Error;
      return 0;
   }

//   uint8_t famID = w1GetFamId(serNo);
   T1wDev *pDev = w1GetDev(serNo);
   if (!pDev) {
      *pError = eBaBoolRC_Error;
      return 0;
   }

   if (!w1ReadDevice(pDev, contents)) {
      *pError = eBaBoolRC_Error;
      return 0;
   }

   // fixme: modularize it correctly.
   // rename correctly the functions with prefixes eg: w1, sr, sp, i2
   return cb(contents.c_str(), contents.size());
}

//
TBaComSerHdl BaComSerInit(const char *dev, EBaComBaud baud) {
   TSerialDesc *p = new TSerialDesc();

   termios options;
   speed_t myBaud;
   int     status;

   // Validate and try opening the file descriptor
   if(!(myBaud = srBaud2Speed(baud)) || !p || !p->pRXD0 || !p->pTXD0 ||
         (p->fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1) {
      delete p;
      return 0;
   }

   p->pTXD0->SetAlt(SER_ALT);
   p->pRXD0->SetAlt(SER_ALT);

   fcntl(p->fd, F_SETFL, O_RDWR) ;

   // Get and modify current options:
   tcgetattr(p->fd, &options) ;

   cfmakeraw(&options) ;
   cfsetispeed(&options, myBaud) ;
   cfsetospeed(&options, myBaud) ;

   options.c_cflag |= (CLOCAL | CREAD) ;
   options.c_cflag &= ~PARENB ;
   options.c_cflag &= ~CSTOPB ;
   options.c_cflag &= ~CSIZE ;
   options.c_cflag |= CS8 ;
   options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
   options.c_oflag &= ~OPOST ;

   options.c_cc [VMIN]  =   0;
   options.c_cc [VTIME] = 100; // Ten seconds (100 deciseconds)

   // Todo: blocks if serial is set to access shell (raspi-config)
   tcsetattr(p->fd, TCSANOW | TCSAFLUSH, &options) ;

   ioctl(p->fd, TIOCMGET, &status);

   status |= TIOCM_DTR ;
   status |= TIOCM_RTS ;

   ioctl(p->fd, TIOCMSET, &status);

   BaCoreMSleep(10);   // 10mS
   return (TBaComSerHdl)p;
}

//
TBaBoolRC BaComSerExit(TBaComSerHdl p) {
   if (!p) {
      return eBaBoolRC_Error;
   }

   close(((TSerialDesc*)p)->fd) ;
   delete (TSerialDesc*)p;
   return 0;
}

//
TBaBoolRC BaComSerPutC(TBaComSerHdl p, uint8_t c) {
   if (!p) {
      return eBaBoolRC_Error;
   }

   write(((TSerialDesc*)p)->fd, &c, 1);
   return 0;
}

//
int BaComSerPend(TBaComSerHdl p) {
   int result = 0;
   if (!p || ioctl(((TSerialDesc*)p)->fd, FIONREAD, &result) == -1) {
      return 0;
   }

   return result ;
}

//
uint8_t BaComSerGetC(TBaComSerHdl p) {
   uint8_t x;

   if (!p || read(((TSerialDesc*)p)->fd, &x, 1) != 1)
     return -1 ;

   return ((int)x) & 0xFF ;
}

//
LOCAL inline speed_t srBaud2Speed(EBaComBaud baud) {
   switch (baud) {
   case eBaComBaud_50    : return     B50;
   case eBaComBaud_75    : return     B75;
   case eBaComBaud_110   : return    B110;
   case eBaComBaud_134   : return    B134;
   case eBaComBaud_150   : return    B150;
   case eBaComBaud_200   : return    B200;
   case eBaComBaud_300   : return    B300;
   case eBaComBaud_600   : return    B600;
   case eBaComBaud_1200  : return   B1200;
   case eBaComBaud_1800  : return   B1800;
   case eBaComBaud_2400  : return   B2400;
   case eBaComBaud_9600  : return   B9600;
   case eBaComBaud_19200 : return  B19200;
   case eBaComBaud_38400 : return  B38400;
   case eBaComBaud_57600 : return  B57600;
   case eBaComBaud_115200: return B115200;
   case eBaComBaud_230400: return B230400;
   default:
      return 0;
   }
   return 0;
}

//
LOCAL inline float w1ReadTemp(const char *dvrStr, TBaBool *pError) {
   if (!dvrStr) {
      if (pError) {
         *pError = eBaBool_true;
      }
      return ERRTEMP;
   }

   // Extract the values. Temp in milli �C
   // 96 01 4b 46 7f ff 0c 10 a0 : crc=a0 YES
   // 96 01 4b 46 7f ff 0c 10 a0 t=25375

   std::string contents(dvrStr);
   if (contents.find("YES") == std::string::npos) {
      if (pError) {
         *pError = eBaBool_true;
      }
      return ERRTEMP;
   }

   return (BaToNumber(
         contents.substr(contents.find("t=") + 2), ERRTEMP, (bool*)pError)/1000.0f);
}

//
LOCAL inline TBaBoolRC w1ReadDevice(T1wDev* pDev, std::string &contents) {
   if (!pDev) {
      return eBaBoolRC_Error;
   }

   // Save the run state
   bool runTemp = pDev->run;

   // Pause the async thread
   pDev->run = false;

   // Read the size of the contents from the driver file (ifstream)
   pDev->pIs->seekg(0, std::ios::end);
   std::string::size_type size = pDev->pIs->tellg();

   // Check if failed
   if (size == std::string::npos) {
      return eBaBoolRC_Error;
   }

   // Size the string to the appropriate size and read
   contents.resize(size);
   pDev->pIs->seekg(0, std::ios::beg);
   pDev->pIs->read(&contents[0], contents.size());

   // Rewind the stream
   pDev->pIs->clear();
   pDev->pIs->seekg(0, std::ios::beg);

   // Resume the async thread
   pDev->run = runTemp;
   return eBaBoolRC_Success;
}

//
LOCAL void w1RdAsyncRout(TBaCoreThreadArg *pArg) {
   if (!pArg || !pArg->pArg) {
      return;
   }

   T1wDev *pDev = (T1wDev*)pArg->pArg;
   std::ifstream &r1wIn =  *pDev->pIs;

   while (!pArg->exitTh) {
      if(pDev->run) {

         // Check file size
         r1wIn.seekg(0, std::ios::end);
         auto size = r1wIn.tellg();

         if (size != -1) {
            // resize string to size
            pDev->actVal.resize(size);

            // Rewind stream
            r1wIn.seekg(0, std::ios::beg);

            // Read contents to string and save the timestamp
            r1wIn.read(&pDev->actVal[0], pDev->actVal.size());
            pDev->ts = BaCoreGetMonTStamp();
            pDev->valid = true;

            // Rewind the stream
            r1wIn.clear();
            r1wIn.seekg(0, std::ios::beg);

         } else {
            pDev->valid = false;
         }

      } else {
         pDev->valid = false;
      }
      pDev->heartBeat++;
//      if (pDev->heartBeat % 50) {
//         printf("%ui \n", pDev->heartBeat);
//      }
      BaCoreMSleep(10);
   }

   // Release the resources used in this thread. Only the thread knows when it
   // stopped using them
   delete pDev->pIs;
   pDev->pIs = 0;
//   pDev->updThread = 0;
//   delete pDev;
}

//
LOCAL uint8_t w1GetFamId(std::string serNo) {
   // eg. 28-0215c2c4bcff [devFam]-[devID]
   std::stringstream ss;

   // This is not char because it extracts one character
   uint16_t famId;
   ss << serNo;
   if (ss >> famId) {
      return famId;
   }
   return 0;
}

//
LOCAL inline T1wDev* w1GetFirstFamDev(uint8_t famID) {
   if (s1WDevs.empty()) {
      return 0;
   }

   auto famSensors = s1WDevs.find(famID);
   if (famSensors == s1WDevs.end() || famSensors->second->empty()) {
      return 0;
   }

   return (*famSensors->second)[0];
}

//
LOCAL inline T1wDev* w1GetDev(const char* serNo) {
   if (s1WDevs.empty()) {
      return 0;
   }

   // if no serNo, return the first device
   if (!serNo) {
      return s1WDevs.begin()->second->empty() ? 0 : (*s1WDevs.begin()->second)[0];
   }

   // Find family sensors
   auto famSensors = s1WDevs.find(w1GetFamId(serNo));
   if (famSensors == s1WDevs.end() || famSensors->second->empty()) {
      return 0;
   }

   for (auto pDev : *famSensors->second) {
      if (pDev && pDev->serNo == serNo) {
         return pDev;
      }
   }

   return 0;
}

//
LOCAL inline int i2cAccess(int fd, uint8_t rw, uint8_t cmd, int size, UI2cData *data) {
   TI2cData args;

   args.read_write = rw ;
   args.command    = cmd ;
   args.size       = size ;
   args.data       = data ;
   return ioctl(fd, I2C_SMBUS, &args) ;
}

#endif // __arm__
