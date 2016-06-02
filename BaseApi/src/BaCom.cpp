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

LOCAL inline speed_t baud2Speed(EBaComBaud baud);
LOCAL inline float readTemp(const char *dvrStr, TBaBool *pError);
LOCAL inline TBaBool read1wDevice(uint8_t devFam, const char *serNo, std::string &contents);

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


// GPIO for the 1w bus. default GPIO 4, pin 7
static IBaGpio *sp1w = 0;

// Map of family IDs and devices vector
static std::map<uint16_t, std::vector<std::ifstream*>> sDevs;

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

   for (auto kv : sDevs) {
      for (auto pIs : kv.second) {
         if (pIs) {
            delete pIs; // deleting closes the file =)
         }
      }
   }
   sDevs.clear();

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

   std::ifstream * pIs = 0;

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
         pIs = new std::ifstream();
         if (pIs) {
            std::string path = DEVPATH + std::string(de->d_name) + "/w1_slave";
            pIs->open(path);
            if(!pIs->fail()) {
               // This is a map of family IDs and file streams
               sDevs[famId].push_back(pIs);
               i++;
            }
         }
      }
   }
   return i;
}

//
float BaCom1WGetTemp(TBaBool *pError) {
   TBaBool terror;
   pError = pError ? pError : &terror;
   std::string contents;

   if (!read1wDevice(TEMPFAM, 0, contents)) {
      *pError = eBaBoolRC_Error;
      return -300;
   }

   return readTemp(contents.c_str(), pError);
}

//
void *BaCom1WGetValue(uint8_t famID, const char *serNo, TBaCom1wReadFun cb,
      TBaBool *pError) {
   TBaBool terror;
   pError = pError ? pError : &terror;
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
   uint8_t x ;

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
      return eBaBoolRC_Error;
   }

   // Extract the values. Temp in milli °C
   // 96 01 4b 46 7f ff 0c 10 a0 : crc=a0 YES
   // 96 01 4b 46 7f ff 0c 10 a0 t=25375

   std::string contents(dvrStr);
   if (contents.find("YES") == std::string::npos) {
      return eBaBoolRC_Error;
   }

   return (BaToNumber(contents.substr(contents.find("t=") + 2), -300, (bool*)pError)/1000.0f);
}

LOCAL inline TBaBool read1wDevice(uint8_t devFam, const char *serNo, std::string &contents) {
   if (!sp1w) {
      return eBaBoolRC_Error;
   }

   // Find thermometers
   auto therms = sDevs.find(devFam);
   if (therms == sDevs.end() || therms->second.size() == 0) {
      return eBaBoolRC_Error;
   }

   // Get the first thermometer
   std::ifstream *p1wIn = 0;
   if (!serNo) {
      p1wIn = therms->second[0];
   } else {
      // todo: no info about the serno =(
      p1wIn = therms->second[0];
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

#endif // __linux
