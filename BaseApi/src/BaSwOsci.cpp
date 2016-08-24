/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaSwOsci.cpp
 *   Date     : Aug 24, 2016
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include "BaSwOsci.h"
#include "CBaSwOsci.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define C_HDL_ ((IBaSwOsci*) hdl)

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
//
TBaSwOsciHdl BaSwOsciCreate(const char *name, const char *path, TBaBool toCnsole) {
   return CBaSwOsci::Create(name, path, toCnsole);
}

//
TBaBoolRC BaSwOsciDestroy(TBaSwOsciHdl hdl) {
   return CBaSwOsci::Destroy(C_HDL_) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaSwOsciRegister(TBaSwOsciHdl hdl, void* pVar, EBaSwOsciType type,
      const char *name, const char *desc) {
   if (!hdl) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->Register(pVar, type, name, desc);
}

//
TBaBoolRC BaSwOsciSample(TBaSwOsciHdl hdl) {
   if (!hdl) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->Sample();
}

//

/*------------------------------------------------------------------------------
    C++ Factories
 -----------------------------------------------------------------------------*/
//
IBaSwOsci *IBaSwOsciCreate(const char *name, const char *path, TBaBool toCnsole) {
   return CBaSwOsci::Create(name, path, toCnsole);
}

//
TBaBoolRC IBaSwOsciDestroy(IBaSwOsci *pHdl) {
   return CBaSwOsci::Destroy(pHdl) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
