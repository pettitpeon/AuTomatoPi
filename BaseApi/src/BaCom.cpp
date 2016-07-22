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
#ifdef __linux

#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <termios.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>

#include "BaCom.h"
#include "BaCore.h"
#include "BaGpio.h"
#include "BaGenMacros.h"
#include "BaUtils.hpp"

#define BUS1W   04
#define TXD0    14
#define RXD0    15
#define SER_ALT  0
#define DEVPATH  "/sys/bus/w1/devices/"
#define TEMPFAM 28

// Serial descriptor
typedef struct TSerialDesc {
   int fd;
   IBaGpio *pTXD0;
   IBaGpio *pRXD0;

   TSerialDesc() : fd(0), pTXD0(0), pRXD0(0) {
      pTXD0 = IBaGpioCreate(TXD0);
      pRXD0 = IBaGpioCreate(RXD0);
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
   bool init;
   TBaCoreThreadHdl updThread;
   TBaCoreThreadArg threadArg;
   bool syncRead;

   T1wDev() : pIs(0), serNo(""), actVal(""), init(false), updThread(0),
         syncRead(false) { threadArg = {0}; }
} T1wDev;

typedef std::map<uint16_t, std::vector<T1wDev>> T1WDevs;
LOCAL inline speed_t baud2Speed(EBaComBaud baud);
LOCAL inline float readTemp(const char *dvrStr, TBaBool *pError);
LOCAL inline TBaBool read1wDevice(uint8_t devFam, const char *serNo, std::string &contents);
LOCAL inline std::ifstream* find1wDevIs(const char *serNo, std::vector<T1wDev> &rTherms);
LOCAL inline T1wDev* find1wDev(const char *serNo, uint8_t devFam);
LOCAL void rdAsync1WRout(TBaCoreThreadArg *pArg);

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

//
TBaComHdl BaComI2CInit() {
   return 0;
}

//
TBaBoolRC BaComI2CExit(TBaComHdl hdl) {
   return 0;
}

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

   for (auto kv : s1WDevs) {
      for (auto dev : kv.second) {
         if (dev.pIs) {
            delete dev.pIs; // deleting closes the file =)
         }
      }
   }
   s1WDevs.clear();

   TBaBoolRC rc = IBaGpioDelete(sp1w);
   sp1w = 0;
   return rc;
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

   T1wDev dev;

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
      std::stringstream ss;
      ss << de->d_name;
      if (ss >> famId) {
         dev.serNo = de->d_name;
         dev.pIs = new std::ifstream();
         if (dev.pIs) {
            std::string path = DEVPATH + std::string(de->d_name) + "/w1_slave";
            dev.pIs->open(path);
            if(!dev.pIs->fail()) {
               // This is a map of family IDs and file streams
               s1WDevs[famId].push_back(dev);
               i++;
            } else {
               delete dev.pIs;
               dev.pIs = 0;
            }
         }
      }
   }
   return i;
}

//
int32_t BaCom1WRdAsync(uint8_t famID, const char *serNo, TBaBool *pError) {
   TBaBool terror;
   pError = pError ? pError : &terror;
   T1wDev *pDev = 0;
   if(!sp1w || s1WDevs.size() == 0) {
      *pError = eBaBool_true;
      return 0;
   }

   auto family = s1WDevs.find(famID);
   if (family == s1WDevs.end() || family->second.size() == 0) {
      return eBaBoolRC_Error;
   }

   for (auto dev : family->second) {
      if (dev.serNo == serNo) {
         pDev = &dev;
      }
   }

   if (!pDev->updThread) {
      pDev->threadArg.pArg = pDev;
      pDev->updThread = BaCoreCreateThread(serNo, rdAsync1WRout, &pDev->threadArg, eBaCorePrio_Normal);
   }
   return 0;
}

//
float BaCom1WGetTemp(const char* serNo, TBaBool *pError) {
   TBaBool terror;
   pError = pError ? pError : &terror;
   std::string contents;

   if (!read1wDevice(TEMPFAM, serNo, contents)) {
      *pError = eBaBool_true;
      return -300;
   }

   return readTemp(contents.c_str(), pError);
}

//
void *BaCom1WGetValue(uint8_t famID, const char *serNo, TBaCom1wReadFun cb,
      TBaBool *pError) {
   TBaBool terror;
   pError = pError ? pError : &terror;

   if (!cb) {
      *pError = eBaBoolRC_Error;
      return 0;
   }
   std::string contents;

   if (!read1wDevice(famID, serNo, contents)) {
      *pError = eBaBoolRC_Error;
      return 0;
   }

   return cb(contents.c_str(), contents.size());
}

//
TBaComSerHdl BaComSerInit(const char *dev, EBaComBaud baud) {
   TSerialDesc *p = new TSerialDesc();

   termios options;
   speed_t myBaud;
   int     status;

   // Validate and try opening the file descriptor
   if(!(myBaud = baud2Speed(baud)) || !p || !p->pRXD0 || !p->pTXD0 ||
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
LOCAL inline speed_t baud2Speed(EBaComBaud baud) {
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
LOCAL inline float readTemp(const char *dvrStr, TBaBool *pError) {
   if (!dvrStr) {
      if (pError) {
         *pError = eBaBool_true;
      }
      return -300;
   }

   // Extract the values. Temp in milli °C
   // 96 01 4b 46 7f ff 0c 10 a0 : crc=a0 YES
   // 96 01 4b 46 7f ff 0c 10 a0 t=25375

   std::string contents(dvrStr);
   if (contents.find("YES") == std::string::npos) {
      if (pError) {
         *pError = eBaBool_true;
      }
      return -300;
   }

   return (BaToNumber(contents.substr(contents.find("t=") + 2), -300, (bool*)pError)/1000.0f);
}

//
LOCAL inline TBaBool read1wDevice(uint8_t devFam, const char *serNo, std::string &contents) {
   if (!sp1w) {
      return eBaBoolRC_Error;
   }

   // Find thermometers
   auto therms = s1WDevs.find(devFam);
   if (therms == s1WDevs.end() || therms->second.size() == 0) {
      return eBaBoolRC_Error;
   }

   // Find the thermometer
   std::ifstream *p1wIn =  find1wDevIs(serNo, therms->second);
   if (!p1wIn) {
      return eBaBoolRC_Error;
   }

   std::ifstream &r1wIn = *p1wIn;

   r1wIn.seekg(0, std::ios::end);
   auto size = r1wIn.tellg();
   if (size == -1) {
      return eBaBoolRC_Error;
   }

   contents.resize(size);
   r1wIn.seekg(0, std::ios::beg);
   r1wIn.read(&contents[0], contents.size());

   // Rewind the stream
   r1wIn.clear();
   r1wIn.seekg(0, std::ios::beg);

   return eBaBoolRC_Success;
}

//
LOCAL inline std::ifstream* find1wDevIs(const char *serNo, std::vector<T1wDev> &rTherms) {
   if (!serNo) {
      return rTherms[0].pIs;
   }

   // todo: no info about the serno =(
   for (auto dev : rTherms) {
      if (dev.serNo == serNo) {
         return dev.pIs;
      }
   }

   return 0;
}

//
LOCAL inline T1wDev* find1wDev(const char *serNo, uint8_t devFam) {
//   if (!s1WDevs) {
//      return 0;
//   }
//
//   for (auto dev : s1WDevs) {
//      if (dev.second == serNo) {
//         return dev;
//      }
//   }
//
   return 0;
}

//
LOCAL void rdAsync1WRout(TBaCoreThreadArg *pArg) {
   if (!pArg || !pArg->pArg) {
      return;
   }

   T1wDev *pDev = (T1wDev*)pArg->pArg;
   std::ifstream &r1wIn =  *pDev->pIs;
   while (!pArg->exitTh) {
      if(!pDev->syncRead) {

         r1wIn.seekg(0, std::ios::end);
         auto size = r1wIn.tellg();

         if (size != -1) {
            pDev->actVal.resize(size);
            r1wIn.seekg(0, std::ios::beg);
            r1wIn.read(&pDev->actVal[0], pDev->actVal.size());

            // Rewind the stream
            r1wIn.clear();
            r1wIn.seekg(0, std::ios::beg);
         }

      }
      BaCoreMSleep(10);
   }
}

#endif // __linux
