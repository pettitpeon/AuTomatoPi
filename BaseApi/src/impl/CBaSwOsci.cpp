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
#include "CBaSwOsci.h"
#include "BaCore.h"
#include "BaLogMacros.h"
#include "BaUtils.hpp"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define TAG "BaSwO"

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
    C++ Interface
 -----------------------------------------------------------------------------*/
CBaSwOsci* CBaSwOsci::Create(const char *name, bool toCnsole) {
   if (!name) {
      return 0;
   }

   CBaSwOsci *p = new CBaSwOsci(name, toCnsole);
   std::ios_base::openmode om = std::ios_base::binary | std::ios_base::out;

   // //////////////// Open ////////////////
   p->mLog.open(name, om);
   if (p->mLog.fail()) {
      WARN_("Cannot open log file: %s", name);
      delete p;
      return 0;
   }
   // //////////////// Open ////////////////

   return p;
}

//
bool CBaSwOsci::Destroy(CBaSwOsci* pHdl) {
   CBaSwOsci *p = dynamic_cast<CBaSwOsci*>(pHdl);
   if (!p ) {
      return false;
   }

//   p->Flush();
   p->mLog.close();

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

}

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
