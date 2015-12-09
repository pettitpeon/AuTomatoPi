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

#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <termios.h>

#include "BaCom.h"
#include "BaCore.h"
#include "BaGpio.h"
#include "BaGenMacros.h"

#define TXD0    14
#define RXD0    15
#define SER_ALT 0

LOCAL inline speed_t baud2Speed(EBaComBaud baud);

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

//
TBaComHdl BaComI2CInit() {
   int rev, fd ;
   const char *device;
   int devId = 0;

   if ((rev = 2) < 0)
   {
//     fprintf (stderr, "wiringPiI2CSetup: Unable to determine Pi board revision\n") ;
//     exit (1) ;
   }

   if (rev == 1)
     device = "/dev/i2c-0" ;
   else
     device = "/dev/i2c-1" ;

   if ((fd = open (device, O_RDWR)) < 0)
     return -1 ;

   if (ioctl (fd, I2C_SLAVE, devId) < 0)
     return -1 ;

//   return fd ;
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
