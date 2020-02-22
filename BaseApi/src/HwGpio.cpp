
/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
//#include <stdio.h>  // printf()
#include <math.h> // for windows needs _USE_MATH_DEFINES
#include <fcntl.h>   // open() rd wr flags
#include <unistd.h>  // open()
//#include <time.h>
#ifdef __linux
#  include <sys/mman.h> // mmap()
#elif __WIN32
#  include <stdlib.h>   // malloc()
#endif
#include <string.h>  // memset()
#include <semaphore.h>
#include <sys/stat.h>  // File permissions
#include <mutex>
#include <string>
#include <iostream>    // For debugging traces

// Local includes
#include "HwGpio.h"
#include "HwPi.h"
#include "BaCore.h"
#include "BaGenMacros.h"

/*------------------------------------------------------------------------------
    General defines
 -----------------------------------------------------------------------------*/
#define TAG        "GPIO"
#define VUINT      volatile uint32_t
// Maximum No. of initializations
#define INITMAX    1048575
#ifdef __WIN32
#  define          MAP_FAILED ((void *) 0)
#endif
#define INIT_      spGpio_map
#define INITHWPWM_ spPWM_map[PWM_CTL]
#define USEDGPIO_  sUsedIos[gpio]
#define GPIOMAX_   27
#define GPIOLIMIT  !gpio || gpio > GPIOMAX_

/*------------------------------------------------------------------------------
    Memory define
 -----------------------------------------------------------------------------*/
// BCM Magic
#define BCM_PASSWORD      0x5A000000
// Offset to peripherals
#define BCM2708_PERI_BASE_OLD  0x20000000
#define BCM2708_PERI_BASE 0x3F000000
#define TIMER_BASE        (BCM2708_PERI_BASE + 0x000B000)
#define PADS_BASE         (BCM2708_PERI_BASE + 0x0100000)
#define CLOCK_BASE        (BCM2708_PERI_BASE + 0x0101000)
#define GPIO_BASE         (BCM2708_PERI_BASE + 0x0200000)
#define PWM_BASE          (BCM2708_PERI_BASE + 0x020C000)
// Block size (4*1024)
#define BLOCK_SIZE        4096

/*------------------------------------------------------------------------------
    Mappings offsets define
 -----------------------------------------------------------------------------*/
// Word offsets into the PWM control region
#define PWM_CTL  0
#define PWM_STAT 1
#define PWM0_RNG 4   // to set resolution
#define PWM0_DAT 5   // To set duty cycle
#define PWM1_RNG 8
#define PWM1_DAT 9
// Clock register offsets
#define CLK_CNTL 40
#define CLK_DIV  41

/*------------------------------------------------------------------------------
    PWM commands defines
 -----------------------------------------------------------------------------*/
#define PWM_STARTCLK  0x0011
#define PWM_STOPCLK   0x0001
#define CLK_BUSY      0x0080
// PWM0
#define PWM0_MS_MODE  0x0080  // Run in MS mode
#define PWM0_USEFIFO  0x0020  // Data from FIFO
#define PWM0_REVPOLAR 0x0010  // Reverse polarity
#define PWM0_OFFSTATE 0x0008  // Output Off state
#define PWM0_REPEATFF 0x0004  // Repeat last value if FIFO empty
#define PWM0_SERIAL   0x0002  // Run in serial mode
#define PWM0_ENABLE   0x0001  // Channel Enable
// PWM1
#define PWM1_MS_MODE  0x8000  // Run in MS mode
#define PWM1_USEFIFO  0x2000  // Data from FIFO
#define PWM1_REVPOLAR 0x1000  // Reverse polarity
#define PWM1_OFFSTATE 0x0800  // Ouput Off state
#define PWM1_REPEATFF 0x0400  // Repeat last value if FIFO empty
#define PWM1_SERIAL   0x0200  // Run in serial mode
#define PWM1_ENABLE   0x0100  // Channel Enable


#define MAX1000S     2000000
#define WRR_         S_IRUSR & S_IWUSR & S_IRGRP & S_IROTH
#define SEM_LOCKED   EAGAIN
#define SEM_UNLOCKED 0



/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
// Software PWM structure
typedef struct TSWPWM {
   TBaCoreThreadArg threadArg;
   TBaCoreThreadHdl threadHdl;
   THwGpio          gpio;
   float            dutyC;
   int64_t          periodUs;
   TSWPWM() : threadArg(), threadHdl(0), gpio(), dutyC(0), periodUs(0) {}
} TPWM;

/*------------------------------------------------------------------------------
    Static variables
 -----------------------------------------------------------------------------*/
// Volatile:
// This means that the compiler will assume that it is possible for the variable
// that p is pointing at to have changed even if there is nothing in the source
// code to suggest that this might occur.
static VUINT      *spGpio_map = 0;
static VUINT      *spClk_map  = 0;
static VUINT      *spPWM_map  = 0;
static int        sIntCnt     = 0;
static TBaBool    sUsedIos[64] = {0};
static std::mutex sMtx;
static uint16_t   sResolution = 0;

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
LOCAL void pwmRoutine(TBaCoreThreadArg *pArg);
LOCAL void setUsed(THwGpio gpioNo, TBaBool used);
LOCAL VUINT *mapAddr(unsigned long baseAddr);
LOCAL bool unmapAddr(VUINT * addr);
LOCAL uint32_t freq2Divisor(float freq);
LOCAL inline void cleanUp(THwGpio gpio);
LOCAL inline void setAlt(THwGpio gpio, int alt);

/*------------------------------------------------------------------------------
    C interface
 -----------------------------------------------------------------------------*/
//
TBaBoolRC HwGpioInit() {

   // Lock the region for concurrency RAII
   std::unique_lock<std::mutex> lck(sMtx);

   EHwPiModel model = HwPiGetBoardModel();
   uint32_t offset = 0;

   // RPI 1 compatibility
   if (model < eHwPiModelBp) {
      offset = BCM2708_PERI_BASE - BCM2708_PERI_BASE_OLD;
   }


   // Already init
   if (spGpio_map) {

      // Too many inits!!!
      if (sIntCnt > INITMAX) {
         return eBaBoolRC_Error;
      }
      sIntCnt++;
      return eBaBoolRC_Success;
   }

   spGpio_map = mapAddr(GPIO_BASE - offset);
   if (spGpio_map == MAP_FAILED) {
      return eBaBoolRC_Error;
   }

   spClk_map = mapAddr(CLOCK_BASE - offset);
   if (spClk_map == MAP_FAILED) {
      unmapAddr(spGpio_map);
      return eBaBoolRC_Error;
   }

   spPWM_map = mapAddr(PWM_BASE - offset);
   if (spPWM_map == MAP_FAILED) {
      unmapAddr(spGpio_map);
      unmapAddr(spClk_map);
      return eBaBoolRC_Error;
   }

   sIntCnt++;
   return eBaBoolRC_Success;
}

//
TBaBoolRC HwGpioExit() {
   // Lock the region for concurrency RAII
   std::unique_lock<std::mutex> lck(sMtx);

   if(!INIT_) {
      return eBaBoolRC_Success;
   }

   if (sIntCnt < 2) {
      unmapAddr(spGpio_map);
      unmapAddr(spClk_map);
      unmapAddr(spPWM_map);
      spGpio_map = 0;
      spClk_map  = 0;
      spClk_map  = 0;
      memset(sUsedIos, 0, sizeof(sUsedIos));
   }

   sIntCnt--;
   return eBaBoolRC_Success;
}

//
TBaBoolRC HwGpioInitHWPWM() {
   return HwGpioSetClkHWPWM(1000.0, 256, EHwGpioHWPWMMode_Balanced);
}

//
TBaBoolRC HwGpioExitHWPWM() {
   // Lock the region for concurrency RAII
   std::unique_lock<std::mutex> lck(sMtx);

   if (!INIT_) {
      return eBaBoolRC_Error;
   }

   spPWM_map[PWM_CTL]  = 0;
   spPWM_map[PWM0_DAT] = 0;
   spPWM_map[PWM0_RNG] = 0x20;
   spPWM_map[PWM1_DAT] = 0;
   spPWM_map[PWM1_RNG] = 0x20;

   return eBaBoolRC_Success;
}

//
TBaBoolRC HwGpioSetClkHWPWM(double frequency, uint32_t resolution, EHwGpioHWPWMMode mode) {
   // Lock the region for concurrency RAII
   std::unique_lock<std::mutex> lck(sMtx);

   if (!INIT_) {
      return eBaBoolRC_Error;
   }

   uint32_t pwm_ctl = 0;

   // Restrict to the maximum and copy it
   sResolution = resolution & 4095;

   // Calculate divisor value for PWM1 clock...base frequency is 19.2MHz
   // Default frequency and resolution
   uint32_t divisor = freq2Divisor(frequency);

   // Save the PWM control register
   pwm_ctl = spPWM_map[PWM_CTL];

   // Stop the PWM
   // We need to stop PWM prior to stopping PWM clock in MS mode otherwise BUSY
   // stays high.
   spPWM_map[PWM_CTL] = 0;

   // Stop the PWM clock
   spClk_map[CLK_CNTL] = BCM_PASSWORD | PWM_STOPCLK;

   // Stop PWM clock before changing divisor. The delay after this does need to
   // this big (95uS occasionally fails, 100uS OK), it's almost as though the BUSY
   // flag is not working properly in balanced mode. Without the delay when DIV is
   // adjusted the clock sometimes switches to very slow, once slow further DIV
   // adjustments do nothing and it's difficult to get out of this mode.
   BaCoreUSleep(110); // WiringPi.c // prevents clock going sloooow

   // Wait until busy flag is set
   while (spClk_map[CLK_CNTL] & CLK_BUSY) {
      BaCoreUSleep(1);
   }

   //set divisor
   spClk_map[CLK_DIV] = BCM_PASSWORD | (divisor << 12);

   // Enable clock
   spClk_map[CLK_CNTL] = BCM_PASSWORD | PWM_STARTCLK;

   // Restore the PWM control
   spPWM_map[PWM_CTL] = pwm_ctl;

   // set the number of counts that constitute a period with 0 for 20 milliseconds = 320 bits
   spPWM_map[PWM0_RNG] = sResolution;
   BaCoreUSleep(10);
   spPWM_map[PWM1_RNG] = sResolution;
   BaCoreUSleep(10);

   // set duty cycle
   spPWM_map[PWM0_DAT] = 0;
   spPWM_map[PWM1_DAT] = 0;

   // enable and set mode
   if (mode == EHwGpioHWPWMMode_Balanced) {
      spPWM_map[PWM_CTL] |=  PWM0_ENABLE | PWM1_ENABLE;
   } else {
      spPWM_map[PWM_CTL] |=  PWM0_ENABLE | PWM1_ENABLE | PWM0_MS_MODE | PWM1_MS_MODE;
   }

   return eBaBoolRC_Success;
}

//
TBaBoolRC HwGpioCleanUp(THwGpio gpio) {
   // Lock the region for concurrency RAII
   std::unique_lock<std::mutex> lck(sMtx);

   if (!INIT_) {
      return eBaBoolRC_Error;
   }
   cleanUp(gpio);
   return eBaBoolRC_Success;
}

//
TBaBoolRC HwGpioSetInp(THwGpio gpio) {
   // Lock the region for concurrency RAII
   std::unique_lock<std::mutex> lck(sMtx);

   if (!INIT_ || USEDGPIO_ || GPIOLIMIT) {
      return eBaBoolRC_Error;
   }

   setUsed(gpio, eBaBool_true);
   spGpio_map[gpio/10] &= ~(7 << ((gpio % 10) * 3));
   return eBaBoolRC_Success;
}

//
TBaBoolRC HwGpioSetOut(THwGpio gpio) {
   // Lock the region for concurrency RAII
   std::unique_lock<std::mutex> lck(sMtx);

   if (!INIT_ || USEDGPIO_ || GPIOLIMIT) {
      return eBaBoolRC_Error;
   }

   setUsed(gpio, eBaBool_true);
   // Set low
   spGpio_map[10] = 1 << gpio;
   spGpio_map[gpio/10] |=  1 << ((gpio % 10) *3);
   return eBaBoolRC_Success;
}

//
TBaBoolRC HwGpioSetAlt(THwGpio gpio, int alt) {
   // Lock the region for concurrency RAII
   std::unique_lock<std::mutex> lck(sMtx);

   if (!INIT_ || USEDGPIO_ || GPIOLIMIT) {
      return eBaBoolRC_Error;
   }

   setAlt(gpio, alt);
   return eBaBoolRC_Success;
}

//
TBaBoolRC HwGpioSet(THwGpio gpio) {
   if (!INIT_ || !USEDGPIO_ || GPIOLIMIT) {
      return eBaBoolRC_Error;
   }

   spGpio_map[7] = 1 << gpio;
   return eBaBoolRC_Success;
}

//
TBaBoolRC HwGpioReset(THwGpio gpio) {
   if (!INIT_ || !USEDGPIO_ || GPIOLIMIT) {
      return eBaBoolRC_Error;
   }

   spGpio_map[10] = 1 << gpio;
   return eBaBoolRC_Success;
}

//
TBaBool HwGpioGet(THwGpio gpio) {
   if (!INIT_ || !USEDGPIO_ || GPIOLIMIT) {
      return eBaBool_false;
   }

   return spGpio_map[13] & ( 1 << gpio) ? eBaBool_true : eBaBool_false;
}

//
TBaBoolRC HwGpioStartHWPWM(THwGpio gpio, float dutyCycle) {
   // Lock the region for concurrency RAII
   std::unique_lock<std::mutex> lck(sMtx);

   // The switch already checks for the GPIO limits
   if (!INIT_ || USEDGPIO_ || !INITHWPWM_) {
      return eBaBoolRC_Error;
   }

   TBaBoolRC ret = eBaBoolRC_Error;
   uint8_t pwmDat  = PWM1_DAT;
   switch (gpio) {
   case 12:
      pwmDat  = PWM0_DAT;
      /* no break */
   case 13:
      setAlt(gpio, 0);
      ret = eBaBoolRC_Success;
      break;
   case 18:
      pwmDat  = PWM0_DAT;
      /* no break */
   case 19:
      setAlt(gpio, 5);
      ret = eBaBoolRC_Success;
      break;
   default:
      break;
   }

   if (ret) {
      // Lock the region for concurrency RAII
      spPWM_map[pwmDat] = (uint32_t) (MINMAX(dutyCycle, 0, 1) * sResolution);
   }

   return ret;
}

//
TBaBoolRC HwGpioResetHWPWMDuCy(THwGpio gpio, float dutyCycle) {
   // Lock the region for concurrency RAII
   std::unique_lock<std::mutex> lck(sMtx);

   // The switch already checks for the GPIO limits
   if (!INIT_ || !INITHWPWM_ || !USEDGPIO_) {
      return eBaBoolRC_Error;
   }

   uint8_t pwmDat  = PWM1_DAT;
   switch (gpio) {
   case 12:
   case 18:
      pwmDat  = PWM0_DAT;
      /* no break */
   case 19:
   case 13:
      spPWM_map[pwmDat] = (uint32_t) (MINMAX(dutyCycle, 0, 1) * sResolution);
      return eBaBoolRC_Success;
      break;
   default:
      break;
   }

   return eBaBoolRC_Error;
}

//
TBaBoolRC HwGpioStopHWPWM(THwGpio gpio) {
   // Lock the region for concurrency RAII
   std::unique_lock<std::mutex> lck(sMtx);

   if (!INIT_ || !INITHWPWM_) {
      return eBaBoolRC_Error;
   }
   //lets put the PWM peripheral registers in their original state
   uint8_t pwmDat  = PWM1_DAT;
   uint8_t pwmRng  = PWM1_RNG;
   switch (gpio) {
   case 12:
   case 18:
      pwmDat  = PWM0_DAT;
      pwmRng  = PWM0_RNG;
      /* no break */
   case 13:
   case 19:

      // Only GPIOs 12, 13, 18 and 19 are valid
      cleanUp(gpio);
      spPWM_map[pwmDat] = 0;
      spPWM_map[pwmRng] = 0x20;
      return eBaBoolRC_Success;
   default:
      break;
   }

   return eBaBoolRC_Error;
}

// Max 100 Hz with 1% resolution
THwGpioSWPWMHdl HwGpioStartSWPWM(THwGpio gpio, float dutyC, uint16_t periodMs) {
   if (!HwGpioSetOut(gpio)) {
      return 0;
   }

   dutyC   = MINMAX(dutyC, 0, 1);
   periodMs = MINMAX(periodMs, 10, 10000);

   TSWPWM *pwm = new TSWPWM();
   pwm->gpio = gpio;
   pwm->dutyC = dutyC;
   pwm->periodUs = periodMs*1000; // to us
   pwm->threadArg.pArg = pwm;
   pwm->threadArg.exitTh = false;
   std::string name = "PWM" +  std::to_string(gpio);
   pwm->threadHdl = BaCoreCreateThread(name.c_str(), pwmRoutine,
         &pwm->threadArg, eBaCorePrio_RT_Normal);
   return pwm;
}

//
TBaBoolRC HwGpioResetSWPWMDuCy(THwGpioSWPWMHdl hdl, float dutyC) {
   if (!INIT_ || !hdl || dutyC < 0) {
      return eBaBoolRC_Error;
   }
   dutyC   = MINMAX(dutyC, 0, 1);
   ((TSWPWM*)hdl)->dutyC = dutyC;

   return eBaBoolRC_Success;
}

//
TBaBoolRC HwGpioStopSWPWM(THwGpioSWPWMHdl hdl) {

   // No need to check for the init flag because even if the API was exit,
   // the thread could be running
   if (!hdl) {
      return eBaBoolRC_Error;
   }
   TSWPWM *pwm = (TSWPWM*)hdl;

   // Destroy PWM thread
   pwm->dutyC = 0;
   if (BaCoreDestroyThread(pwm->threadHdl, pwm->periodUs/1000 * 2)) {
      cleanUp(pwm->gpio);
      delete pwm;
      pwm = 0;
   } else {

      // pwm structure is leaked, but the leakage is minimal
      pwm->gpio = 0;
      cleanUp(pwm->gpio);
   }


   return pwm ? eBaBoolRC_Error : eBaBoolRC_Success;
}

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
//
LOCAL void pwmRoutine(TBaCoreThreadArg *pArg) {
   if (!INIT_ || !pArg || !pArg->pArg) {
      return;
   }

   TSWPWM *pwm = (TSWPWM*) pArg->pArg;
   while (!pArg->exitTh) {
      // Off
      HwGpioReset(pwm->gpio);
      BaCoreUSleep((int64_t) (pwm->periodUs * (1 - pwm->dutyC)));

      // On
      HwGpioSet(pwm->gpio);
      BaCoreUSleep((int64_t) (pwm->periodUs * pwm->dutyC));
   }
}

//
LOCAL void setUsed(THwGpio gpio, TBaBool used){
   sUsedIos[gpio] = used;
}

//
LOCAL VUINT *mapAddr(unsigned long baseAddr) {
   void *addrMap = MAP_FAILED;

#ifdef __linux
   int mem_fd = 0;
   // open /dev/mem
   if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
      printf("/dev/mem open error\n");
      return 0;
   }

   // mmap GPIO
   addrMap = mmap(
         NULL,             //Any address in our space will do
         BLOCK_SIZE,       //Map length
         PROT_READ | PROT_WRITE, // Enable reading & writing to mapped memory
         MAP_SHARED,       //Shared with other processes
         mem_fd,           //File to map
         baseAddr
   );

   // No need to keep mem_fd open after mmap
   close(mem_fd);
#elif __WIN32
   addrMap = malloc(BLOCK_SIZE);
#endif

   return (volatile unsigned *)addrMap;
}

//
LOCAL bool unmapAddr(VUINT * addr) {
#ifdef __linux
   // If null, error
   return !munmap((void *)addr, BLOCK_SIZE);
#elif __WIN32
   free((void *)addr);
#endif
   return true;
}

// Calculate divisor value for PWM1 clock...base frequency is 19.2MHz
LOCAL uint32_t freq2Divisor(float freq) {
   double period = 1.0/freq;
   double minPulseWidth = period/(sResolution * 1.0);
   return (uint32_t)(19200000.0f / (1.0/minPulseWidth));
}

// Mutex-less
LOCAL inline void cleanUp(THwGpio gpio) {
   spGpio_map[gpio/10] &= ~(7 << ((gpio % 10) * 3));
   setUsed(gpio, eBaBool_false);
}

// Mutex-less
LOCAL inline void setAlt(THwGpio gpio, int alt) {
   setUsed(gpio, eBaBool_true);
   spGpio_map[gpio/10] |= (alt <= 3 ? alt + 4 : alt == 4 ? 3 : 2) << ((gpio % 10) * 3);
}


/*------------------------------------------------------------------------------
    C++ interface implementation
 -----------------------------------------------------------------------------*/
class CHwGpio : public IHwGpio {
public:

   // Sleeping LED auxiliary struct
   typedef struct TSleepLED {
      CHwGpio *pGpio;
      float    sCycleS;
   } TSleepLED;

   /* *************************************************************************/
   /*  Factory
    */
   //
   static IHwGpio* Create(uint8_t gpioNo) {
      HwGpioInit();
      CHwGpio* p = new CHwGpio(gpioNo);

      if (p) {
         std::string name = "/GPIO_" + std::to_string(gpioNo);
#ifdef __linux
         p->mpLock = sem_open(name.c_str(), O_CREAT, WRR_, 1);

         // Check if we can own this GPIO and lock it
         if (p->mpLock != SEM_FAILED &&
               sem_trywait(p->mpLock) == SEM_UNLOCKED) {
#elif __WIN32
         p->mpLock = (sem_t*)1;
         {
#endif
            HwGpioCleanUp(p->mGpioNo);
            return p;
         }
      }

      Delete(p);
      return 0;
   }

   //
   static bool Delete(IHwGpio *pHdl, bool dirty = false) {
      CHwGpio *p = dynamic_cast<CHwGpio*>(pHdl);
      if (!p ) {
         return false;
      }

      // If dirty flag, do not cleanup
      if (!dirty) {
         HwGpioCleanUp(p->mGpioNo);
      }

      HwGpioExit();
      if (p->mpLock != SEM_FAILED) {
#ifdef __linux
         sem_unlink( ("/GPIO_" + std::to_string(p->mGpioNo)).c_str() );
         sem_close(p->mpLock);
#endif
         p->mpLock = SEM_FAILED;
      }
      delete p;
      return true;
   }

   // Worker thread routine
   static void SleepLEDRoutine(TBaCoreThreadArg *pArg) {
      CHwGpio *pGpio = ((TSleepLED*)pArg->pArg)->pGpio;
      float &rCylcleS = ((TSleepLED*)pArg->pArg)->sCycleS;
      for (int i = 0; !pArg->exitTh; ++i) {
         if(i == MAX1000S) {
            i = 0;
         }
         pGpio->ResetSWPWMDuCy( (sin(i/rCylcleS/1000.0 * M_PI) + 1) / 2.0 );
         BaCoreMSleep(1);
      }
   }

   /* *************************************************************************/
   /*  Hardware PWM setup functions
    */
   //
   virtual bool InitHWPWM() {
      return HwGpioInitHWPWM();
   }

   //
   virtual bool ExitHWPWM() {
      return HwGpioExitHWPWM();
   }

   //
   virtual bool SetClkHWPWM(double frequency, uint32_t resolution, EHwGpioHWPWMMode mode) {
      return HwGpioSetClkHWPWM(frequency, resolution, mode);
   }

/* ****************************************************************************/
/* Basic GPIO operations
 */
   //
   virtual void CleanUp() {
      HwGpioCleanUp(mGpioNo);
   }

   //
   virtual bool SetInp() {
      HwGpioCleanUp(mGpioNo);
      return HwGpioSetInp(mGpioNo);
   }

   //
   virtual bool SetOut() {
      HwGpioCleanUp(mGpioNo);
      return HwGpioSetOut(mGpioNo);
   }

   //
   virtual bool SetAlt(int alt) {
      HwGpioCleanUp(mGpioNo);
      return HwGpioSetAlt(mGpioNo, alt);
   }

   //
   virtual bool Set() {
      return HwGpioSet(mGpioNo);
   }

   //
   virtual bool Reset() {
      return HwGpioReset(mGpioNo);
   }

   //
   virtual bool Get() {
      return HwGpioGet(mGpioNo);
   }

   /* *************************************************************************/
   /*  PWM Functions
    */
   //
   virtual bool StartHWPWM(float dutyCycle) {
     return HwGpioStartHWPWM(mGpioNo, dutyCycle);
   }

   //
   virtual bool ResetHWPWMDuCy(float dc) {
      return HwGpioResetHWPWMDuCy(mGpioNo, dc);
   }

   //
   virtual bool StopHWPWM() {
      return HwGpioStopHWPWM(mGpioNo);
   }

   //
   virtual bool StartSWPWM(float dutyC, uint16_t cycleMs) {
      if (mPWM) {
         return false;
      }
      HwGpioCleanUp(mGpioNo);
      mPWM = HwGpioStartSWPWM(mGpioNo, dutyC, cycleMs);
      return mPWM ? true : false;
   }

   //
   virtual bool ResetSWPWMDuCy(float dutyC) {
      return HwGpioResetSWPWMDuCy(mPWM, dutyC);
   }

   //
   virtual bool StopSWPWM() {
      bool ret = HwGpioStopSWPWM(mPWM);
      mPWM = 0;
      return ret;
   }

   //
   virtual bool StartSWPWMSleepingLED(float sCycleS) {
      if (mPWM || mSleepLED) {
         return false;
      }

      StartSWPWM(0, 10);
      std::string name = "SleepLED" +  std::to_string(mGpioNo);
      mpSleepLEDArg = new TBaCoreThreadArg{new TSleepLED{this, sCycleS}, eBaBool_false};
      mSleepLED = BaCoreCreateThread(name.c_str(),
            SleepLEDRoutine, mpSleepLEDArg, eBaCorePrio_Normal);
      return true;
   }

   //
   virtual bool ResetSWPWMSleepingLED(float sCycleS) {
      if (!mSleepLED || !mpSleepLEDArg || !mpSleepLEDArg->pArg) {
         return false;
      }
      ((TSleepLED*) mpSleepLEDArg->pArg)->sCycleS = sCycleS;
      return true;
   }

   //
   virtual bool StopSWPWMSleepingLED() {
      if (!mSleepLED || !mpSleepLEDArg || !mpSleepLEDArg->pArg) {
         return false;
      }

      bool ret = false;
      if (BaCoreDestroyThread(mSleepLED, 100)) {
         delete (TSleepLED*)mpSleepLEDArg->pArg;
         mpSleepLEDArg->pArg = 0;
         delete mpSleepLEDArg;
         mpSleepLEDArg = 0;

         ret = true;
      }

      return ret && StopSWPWM();
   }

   // Constructor
   CHwGpio(uint8_t gpioNo) : mGpioNo(gpioNo), mPWM(0), mSleepLED(0),
         mpSleepLEDArg(0), mpLock(0) {};

   // Typical object oriented destructor must be virtual!
   virtual ~CHwGpio() {};

   // Make this object non-copyable.
   // This is important because the implementation is in a dynamic pointer and
   // a deep copy of the implementation is explicitly required
   CHwGpio(const CHwGpio&);
   CHwGpio& operator=(const CHwGpio&);

   const uint8_t    mGpioNo;        // GPIO No.
   THwGpioSWPWMHdl  mPWM;           // PWM thread handle
   TBaCoreThreadHdl mSleepLED;      // Sleeping LED thread handle
   TBaCoreThreadArg *mpSleepLEDArg; // Sleeping LED thread arg
   sem_t            *mpLock;        // GPIO semaphore linked to the GPIO No.
};

/*------------------------------------------------------------------------------
    C++ factory
 -----------------------------------------------------------------------------*/
//
IHwGpio* IHwGpioCreate(THwGpio gpio) {
   return CHwGpio::Create(gpio);
}

//
TBaBoolRC IHwGpioDelete(IHwGpio* pHdl) {
   return CHwGpio::Delete(pHdl);
}

//
TBaBoolRC IHwGpioDeleteDirty(IHwGpio* pHdl) {
   return CHwGpio::Delete(pHdl, true);
}


