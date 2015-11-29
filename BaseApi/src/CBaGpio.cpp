// Header

#define _USE_MATH_DEFINES
#include <math.h>

#include <string>
#include <iostream>    // For debugging traces
#include <semaphore.h>
#include <fcntl.h>     // O-flags
#include <sys/stat.h>  // File permissions

#include "BaGpio.h"
#include "CBaGpio.h"
#include "BaCore.h"
#include "BaGenMacros.h"

/* ****************************************************************************/
/*  Defines
 */
#define TAG          "CBaGpio"
#define MAX1000S     2000000
#define WRR_         S_IRUSR & S_IWUSR & S_IRGRP & S_IROTH
#define SEM_LOCKED   EAGAIN
#define SEM_UNLOCKED 0

/* ****************************************************************************/
/*  Types
 */
typedef struct TSleepLED {
   CBaGpio *pGpio;
   float    sCycleS;
} TSleepLED;


/* ****************************************************************************/
/*  Pimpl
 */
class CBaGpio::Impl {
public:

   // Constructors
   Impl(uint8_t gpioNo) : mGpioNo(gpioNo), mPWM(0), mSleepLED(0),
   mpSleepLEDArg(0), mpLock(0) {
   }

   LOCAL void SleepLEDRoutine(TBaCoreThreadArg *pArg) {
      CBaGpio *pGpio = ((TSleepLED*)pArg->pArg)->pGpio;
      float &rCylcleS = ((TSleepLED*)pArg->pArg)->sCycleS;
      for (int i = 0; !pArg->exitTh; ++i) {
         if(i == MAX1000S) {
            i = 0;
         }
         pGpio->ResetSWPWMDuCy( (sin(i/rCylcleS/1000.0 * M_PI) + 1) / 2.0 );
         BaCoreMSleep(1);
      }
   }

   const uint8_t    mGpioNo;        // GPIO No.
   TBaGpioSWPWMHdl  mPWM;           // PWM thread handle
   TBaCoreThreadHdl mSleepLED;      // Sleeping LED thread handle
   TBaCoreThreadArg *mpSleepLEDArg; // Sleeping LED thread arg
   sem_t            *mpLock;        // GPIO semaphore linked to the GPIO No.
};

/* ****************************************************************************/
/*  Factory
 */
//
IBaGpio* CBaGpio::Create(uint8_t gpioNo) {

   BaGpioInit();

   CBaGpio* p = new CBaGpio();
   if (p) {
      p->mpImpl = new Impl(gpioNo);
      if (p->mpImpl) {
         std::string name = "/GPIO_" + std::to_string(gpioNo);
#ifdef __linux
         p->mpImpl->mpLock = sem_open(name.c_str(), O_CREAT, WRR_, 1);

         // Check if we can own this GPIO and lock it
         if (p->mpImpl->mpLock != SEM_FAILED &&
             sem_trywait(p->mpImpl->mpLock) == SEM_UNLOCKED) {
#elif __WIN32
         p->mpImpl->mpLock = (sem_t*)1;
         {
#endif
            BaGpioCleanUp(p->mpImpl->mGpioNo);
            return p;
         }
      }
   }

   Delete(p);
   return 0;
}

//
bool CBaGpio::Delete(IBaGpio *pHdl) {
   CBaGpio *p = dynamic_cast<CBaGpio*>(pHdl);
   if (!p ) {
      return false;
   }

   BaGpioCleanUp(p->mpImpl->mGpioNo);
   BaGpioExit();
   if (p->mpImpl->mpLock != SEM_FAILED) {
#ifdef __linux
      sem_unlink( ("/GPIO_" + std::to_string(p->mpImpl->mGpioNo)).c_str() );
      sem_close(p->mpImpl->mpLock);
#endif
      p->mpImpl->mpLock = SEM_FAILED;
   }
   delete p->mpImpl;
   delete p;
   return true;
}

/* ****************************************************************************/
/*  Hardware PWM setup functions
 */
//
bool CBaGpio::InitHWPWM() {
   return BaGpioInitHWPWM();
}

//
bool CBaGpio::ExitHWPWM() {
   return BaGpioExitHWPWM();
}

//
bool CBaGpio::SetClkHWPWM(double frequency, uint32_t resolution, EBaGpioHWPWMMode mode) {
   return BaGpioSetClkHWPWM(frequency, resolution, mode);
}

/* ****************************************************************************/
/* Basic GPIO operations
 */
//
void CBaGpio::CleanUp() {
   BaGpioCleanUp(mpImpl->mGpioNo);
}

//
bool CBaGpio::SetInp() {
   BaGpioCleanUp(mpImpl->mGpioNo);
   return BaGpioSetInp(mpImpl->mGpioNo);
}

//
bool CBaGpio::SetOut() {
   BaGpioCleanUp(mpImpl->mGpioNo);
   return BaGpioSetOut(mpImpl->mGpioNo);
}

//
bool CBaGpio::SetAlt(int alt) {
   BaGpioCleanUp(mpImpl->mGpioNo);
   return BaGpioSetAlt(mpImpl->mGpioNo, alt);
}

//
bool CBaGpio::Set() {
   return BaGpioSet(mpImpl->mGpioNo);
}

//
bool CBaGpio::Reset() {
   return BaGpioReset(mpImpl->mGpioNo);
}

//
bool CBaGpio::Get() {
   return BaGpioGet(mpImpl->mGpioNo);
}

/* ****************************************************************************/
/*  PWM Functions
 */
//
bool CBaGpio::StartHWPWM(float dutyCycle) {
  return BaGpioStartHWPWM(mpImpl->mGpioNo, dutyCycle);
}

//
bool CBaGpio::ResetHWPWMDuCy(float dc) {
   return BaGpioResetHWPWMDuCy(mpImpl->mGpioNo, dc);
}

//
bool CBaGpio::StopHWPWM() {
   return BaGpioStopHWPWM(mpImpl->mGpioNo);
}

//
bool CBaGpio::StartSWPWM(float dutyC, uint16_t cycleMs) {
   if (mpImpl->mPWM) {
      return false;
   }
   BaGpioCleanUp(mpImpl->mGpioNo);
   mpImpl->mPWM = BaGpioStartSWPWM(mpImpl->mGpioNo, dutyC, cycleMs);
   return mpImpl->mPWM ? true : false;
}

//
bool CBaGpio::ResetSWPWMDuCy(float dutyC) {
   return BaGpioResetSWPWMDuCy(mpImpl->mPWM, dutyC);
}

//
bool CBaGpio::StopSWPWM() {
   bool ret = BaGpioStopSWPWM(mpImpl->mPWM);
   mpImpl->mPWM = 0;
   return ret;
}

//
bool CBaGpio::StartSWPWMSleepingLED(float sCycleS) {
   if (mpImpl->mPWM || mpImpl->mSleepLED) {
      return false;
   }

   StartSWPWM(0, 10);
   std::string name = "SleepLED" +  std::to_string(mpImpl->mGpioNo);
   mpImpl->mpSleepLEDArg = new TBaCoreThreadArg{new TSleepLED{this, sCycleS}, eBaBool_false};
   mpImpl->mSleepLED = BaCoreCreateThread(name.c_str(),
         CBaGpio::Impl::SleepLEDRoutine, mpImpl->mpSleepLEDArg, eBaCorePrio_Normal);
   return true;
}

//
bool CBaGpio::ResetSWPWMSleepingLED(float sCycleS) {
   if (!mpImpl->mSleepLED || !mpImpl->mpSleepLEDArg || !mpImpl->mpSleepLEDArg->pArg) {
      return false;
   }
   ((TSleepLED*) mpImpl->mpSleepLEDArg->pArg)->sCycleS = sCycleS;
   return true;
}

//
bool CBaGpio::StopSWPWMSleepingLED() {
   if (!mpImpl->mSleepLED || !mpImpl->mpSleepLEDArg || !mpImpl->mpSleepLEDArg->pArg) {
      return false;
   }

   bool ret = false;
   if (BaCoreDestroyThread(mpImpl->mSleepLED, 100)) {
      delete (TSleepLED*)mpImpl->mpSleepLEDArg->pArg;
      mpImpl->mpSleepLEDArg->pArg = 0;
      delete mpImpl->mpSleepLEDArg;
      mpImpl->mpSleepLEDArg = 0;

      ret = true;
   }

   return ret && StopSWPWM();
}

