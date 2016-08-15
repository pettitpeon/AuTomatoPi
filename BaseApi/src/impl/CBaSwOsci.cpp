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
#include "CBaSwOsci.h"
#include "BaCore.h"
#include "BaUtils.hpp"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/

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
CBaSwOsci* CBaSwOsci::Create() {
   return new CBaSwOsci();
}


bool CBaSwOsci::Register(void* pVar, EBaSwOsciType type, const char *name, const char *desc) {
   if (!pVar || type <= eBaSwOsci_undef || type > eBaSwOsci_max || !name) {
      return false;
   }

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

      if (p == mRegister.back()) {
         s.append(p->name).append("\n");
      } else {
         s.append(p->name).append(", ");
      }
   }

   printf(s.c_str());
   return true;
}

//
bool CBaSwOsci::Sample() {
   if (mRegister.empty()) {
      return false;
   }
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

      if (p == mRegister.back()) {
         printf("%s\n", s.c_str());
      } else {
         s += ", ";
      }
   }

   return true;
}

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
