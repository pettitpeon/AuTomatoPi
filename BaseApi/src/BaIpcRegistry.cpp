/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaIpcRegistry.cpp
 *   Date     : 17.03.2017
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include "BaIpcRegistry.h"
#include "CBaIpcRegistry.h"
#include "BaBool.h"
/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define C_HDL_ ((IBaIpcRegistry*) hdl)

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
//
TBaIpcRegistryHdl BaIpcRegistryCreate() {
   return CBaIpcRegistry::Create();
}

//
TBaBoolRC BaIpcRegistryDestroy(TBaIpcRegistryHdl hdl) {
   return IBaIpcRegistryDestroy(C_HDL_);
}

//
TBaBoolRC BaIpcRegistryRegisterFun(TBaIpcRegistryHdl hdl, const char* name,
      TBaIpcRegFun fun) {
   if (!hdl || !name) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->RegisterFun(name, fun) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcRegistryUnregisterFun(TBaIpcRegistryHdl hdl, const char* name) {
   if (!hdl || !name) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->UnregisterFun(name) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcRegistryClearFunRegistry(TBaIpcRegistryHdl hdl) {
   if (!hdl) {
      return eBaBoolRC_Error;
   }
   C_HDL_->ClearFunRegistry();

   return  eBaBoolRC_Success;
}

//
TBaBoolRC BaIpcRegistryCallFun(TBaIpcRegistryHdl hdl, const char* name,
      TBaIpcFunArg a, TBaIpcArg *pRet) {
   if (!hdl || !name) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->CallFun(name, a, pRet) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

/*------------------------------------------------------------------------------
    C++ Factories
 -----------------------------------------------------------------------------*/
//
IBaIpcRegistry * IBaIpcRegistryCreate() {
   return CBaIpcRegistry::Create();
}

//
TBaBoolRC IBaIpcRegistryDestroy(IBaIpcRegistry *pHdl) {
   CBaIpcRegistry *p = dynamic_cast<CBaIpcRegistry*>(pHdl);
   if (p) {
      return CBaIpcRegistry::Destroy(p) ? eBaBoolRC_Success : eBaBoolRC_Error;
   }

   return eBaBoolRC_Error;
}

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
