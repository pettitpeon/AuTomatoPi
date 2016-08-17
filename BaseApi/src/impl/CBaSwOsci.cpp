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
#include <iostream>
#include <chrono>
#include "CBaSwOsci.h"
#include "BaCore.h"
#include "BaLogMacros.h"
#include "BaUtils.hpp"
#include "BaGenMacros.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define TAG "BaSwO"
#define MINSLEEP_US  10000
#define CYCLE_US     50000
#define CYCLECUM_US 500000
#define LASTCYCLE_US std::chrono::duration_cast<std::chrono::microseconds> \
   (std::chrono::steady_clock::now() - start).count()
/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
typedef struct CBaSwOsci::TSWOsci {
   void *pVar;
   EBaSwOsciType type;
   const char *name;
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
CBaSwOsci* CBaSwOsci::Create(const char *name, const char *path, bool toCnsole) {
   if (!name) {
      return 0;
   }

   CBaSwOsci *p = new CBaSwOsci(name, path, toCnsole);
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
bool CBaSwOsci::Destroy(CBaSwOsci* pHdl) {
   CBaSwOsci *p = dynamic_cast<CBaSwOsci*>(pHdl);
   if (!p ) {
      return false;
   }

//   p->Flush();
//
//   {
//      std::lock_guard<std::mutex> lck(p->mMtx);
//      p->mLog.close();
//   }

   delete pHdl;
   return true;
}

//
bool CBaSwOsci::Register(void* pVar, EBaSwOsciType type, const char *name, const char *desc) {
   if (!pVar || type <= eBaSwOsci_undef || type > eBaSwOsci_max || !name || mSampling) {
      return false;
   }
   std::lock_guard<std::mutex> lck(mMtx);
   mRegister.push_back(new CBaSwOsci::TSWOsci(pVar, type, name, desc));
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
   if (!mSampling) {
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
   int64_t dur = 0;
   std::chrono::steady_clock::time_point start;
   for ( ; !pArg->exitTh ; cycleCum += LASTCYCLE_US) {
      start = std::chrono::steady_clock::now();

      if (cycleCum > CYCLECUM_US) {
         p->Flush();
         TRACE_("flushed");
         cycleCum -= CYCLECUM_US;
      }



      dur = (start - std::chrono::steady_clock::now()).count();
      if (dur + MINSLEEP_US > CYCLE_US) {
         TRACE_("%I64d: %I64d", MINSLEEP_US, cycleCum);
         BaCoreUSleep(MINSLEEP_US);
      } else {
         TRACE_("%I64d: %I64d", CYCLE_US - dur, cycleCum);
         BaCoreUSleep(CYCLE_US - dur);
      }
   }

}
