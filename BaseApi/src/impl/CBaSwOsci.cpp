/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CBaSwOsci.cpp
 *   Date     : Aug 15, 2016
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <stdio.h>
//#ifdef __WIN32
//# define __STDC_FORMAT_MACROS
//#endif
#include <inttypes.h>
#include <iostream>
#include <chrono>
#include <algorithm>
#include "CBaSwOsci.h"
#include "BaCore.h"
#include "BaLogMacros.h"
#include "BaUtils.hpp"
#include "BaGenMacros.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define TAG "BaSwO"
#define MINSLEEP_US  10000 // 10 ms Minimum time that the thread will sleep
#define CYCLE_US     50000 // 50 ms Regular cycle time
#define CYCLECUM_US 500000 // 500 ms Duration after which the data will be processed
#define STEADYCLK   std::chrono::steady_clock
#define LASTCYCLE_US std::chrono::duration_cast<std::chrono::microseconds> \
   (STEADYCLK::now() - start).count()
/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
typedef struct CBaSwOsci::TSWOsci {
   void *pVar;
   EBaSwOsciType type;
   std::string name;
   const char *desc;

   TSWOsci() : pVar(0), type(eBaSwOsci_undef), name(0), desc(0) {};
   TSWOsci(void *pVar, EBaSwOsciType type, const char *name, const char *desc) :
      pVar(pVar), type(type), name(name), desc(desc) {};
} TSWOsci;

/*------------------------------------------------------------------------------
    Static variables
 -----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
    Local Functions
 -----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/
// todo: should it be 100% multi threading?
CBaSwOsci* CBaSwOsci::Create(const char *name, const char *path, bool toCnsole) {
   if (!name) {
      return 0;
   }

   CBaSwOsci *p = new CBaSwOsci(name, path ? path : "", toCnsole);
   std::ios_base::openmode om = std::ios_base::binary | std::ios_base::out;

   // //////////////// Open ////////////////
   p->mLog.open(BaPath::Concatenate(p->mPath, p->mName) + ".csv", om);
   if (p->mLog.fail()) {
      WARN_("Cannot open log file: %s", name);
      delete p;
      return 0;
   }
   // //////////////// Open ////////////////

   std::string thName = p->mName + "wt";
   p->mThrArg.pArg = p;
   p->mThread = BaCoreCreateThread(thName.c_str(), thRout, &p->mThrArg, eBaCorePrio_High);
   return p;
}

//
bool CBaSwOsci::Destroy(IBaSwOsci* pHdl) {
   CBaSwOsci *p = dynamic_cast<CBaSwOsci*>(pHdl);
   if (!p ) {
      return false;
   }

   p->Flush();

   // No mutexes beyond this point
   BaCoreDestroyThread(p->mThread, 0);
   p->mThread = 0;

   // delete p;
   // The handle is deleted inside the thread. This is important because
   // deleting the handle deletes the ofstream and if the thread tries to access
   // it, CRASHHH!!. Deleting it in the thread result in one of the 2 following:
   // 1. The thread releases the memory when it no longer needs it
   // 2. The thread gets stuck and memory leaks

   return true;
}

//
bool CBaSwOsci::Register(void* pVar, EBaSwOsciType type, const char *name, const char *desc) {
   if (!pVar || type <= eBaSwOsci_undef || type > eBaSwOsci_max || !name || mSampling) {
      return false;
   }

   // replace all ',' with '.'
   std::string s = name;
   std::replace(s.begin(), s.end(), ',', '.');

   std::lock_guard<std::mutex> lck(mMtx);
   mRegister.push_back(new CBaSwOsci::TSWOsci(pVar, type, s.c_str(), desc));
   return true;
}

//
bool CBaSwOsci::Header() {
   if (mRegister.empty()) {
      return false;
   }

   std::string s = "Time, Timestamp, ";
   for (TSWOsci *p : mRegister) {
      if (p != mRegister.back()) {
         s.append(p->name).append(", ");
      } else {
         s.append(p->name);
      }
   }

   mLog << s << std::endl;
   if (mToCnsole) {
      std::cout << s << std::endl;
   }
   return true;
}

//
bool CBaSwOsci::Sample() {
   if (mRegister.empty()) {
      return false;
   }
   std::lock_guard<std::mutex> lck(mMtx);

   if (!mSampling) {
      Header();
   }
   mSampling = true;
   TBaCoreTimeStamp ts = {0};
   char pBuf[BACORE_TSTAMPLEN];
   TBaCoreMonTStampUs tsm = BaCoreGetMonTStamp();
   BaCoreGetTStamp(&ts);

   std::string s = BaFString("%s, %lli, ", BaCoreTStampToStr(&ts, pBuf), tsm);

   for (TSWOsci *p : mRegister) {
      switch (p->type) {
         case eBaSwOsci_int8  : s += BaFString("%i", *((int8_t*)p->pVar)); break;
         case eBaSwOsci_uint8 : s += BaFString("%i", *((uint8_t*)p->pVar)); break;
         case eBaSwOsci_int16 : s += BaFString("%i", *((int16_t*)p->pVar)); break;
         case eBaSwOsci_uint16: s += BaFString("%i", *((uint16_t*)p->pVar)); break;
         case eBaSwOsci_int32 : s += BaFString("%i", *((int32_t*)p->pVar)); break;
         case eBaSwOsci_uint32: s += BaFString("%u", *(uint32_t*)p->pVar); break;
         case eBaSwOsci_int64 : s += BaFString("%lli", *(int64_t*)p->pVar); break;
         case eBaSwOsci_uint64: s += BaFString("%llu", *(uint64_t*)p->pVar); break;
         case eBaSwOsci_float : s += BaFString("%g", *(float*)p->pVar); break;
         case eBaSwOsci_double: s += BaFString("%g", *(double*)p->pVar); break;
         default: s += BaFString("N/A"); break;
      }

      if (p != mRegister.back()) {
         s += ", ";
      }
   }

   mBuf.push_back(s);
   if (mToCnsole) {
      std::cout << s << std::endl;
   }

   return true;
}

inline void CBaSwOsci::Flush() {
   if (!mSampling || mBuf.empty()) {
      return;
   }

   // Avoid messing around with the buffer while it is being printed
   std::lock_guard<std::mutex> lck(mMtx);

   // Iterate messages in buffer
   for (auto &msg : mBuf) {
      // /////////// Log to disc ///////////////////////
      mLog << msg << "\n";
//      std::cout << msg << std::endl; // For testing and debugging
      // ///////////////////////////////////////////////
   }

   mLog.flush();
   mBuf.clear();
}

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
void CBaSwOsci::thRout(TBaCoreThreadArg *pArg) {
   if (!pArg || !pArg->pArg) {
      return;
   }

   CBaSwOsci* p = (CBaSwOsci*)(pArg->pArg);
   int64_t cycleCum = 0;
   int64_t cycleDur = 0;
   STEADYCLK::time_point start;

   // Each cycle is CYCLE_US long. When the cumulative cycle is > CYCLECUM_US,
   // then the actual process is executed. This is made to avoid long sleeps
   // that might hurt the main thread when exiting this thread. This way the
   // sleep is limited to a maximum of CYCLE_US
   for ( ; !pArg->exitTh ; cycleCum += LASTCYCLE_US) {
      start = STEADYCLK::now();

      // If the cumulative cycle is over the limit, process!!!
      if (cycleCum > CYCLECUM_US) {
         p->Flush();

         // Keep the difference in the accumulator for more accuracy
         cycleCum -= CYCLECUM_US;
      }

      cycleDur = (start - STEADYCLK::now()).count();
      if (cycleDur + MINSLEEP_US > CYCLE_US) {
         BaCoreUSleep(MINSLEEP_US);
      } else {
         BaCoreUSleep(CYCLE_US - cycleDur);
      }
   }

   // Delete everything from the register
   for(auto item : p->mRegister) {
      delete item;
   }
   p->mRegister.clear();
   p->mThread = 0;

   // Delete the underlying object!!
   delete p;
}

