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
#include <string.h>
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
TBaBoolRC BaIpcRegistryLocalInit() {
   return CBaIpcRegistry::SInitRegistry() ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcRegistryLocalExit() {
   return CBaIpcRegistry::SExitRegistry() ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcRegistryLocalRegisterFun(const char* name, TBaIpcRegFun fun) {
   if (!name) {
      return eBaBoolRC_Error;
   }

   return CBaIpcRegistry::SRegisterFun(name, fun) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcRegistryLocalUnregisterFun(const char* name) {
   if (!name) {
      return eBaBoolRC_Error;
   }

   return CBaIpcRegistry::SUnregisterFun(name) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcRegistryLocalCallFun(const char* name, TBaIpcFunArg a, TBaIpcArg *pRet) {
   if (!name) {
      return eBaBoolRC_Error;
   }

   return CBaIpcRegistry::SCallFun(name, a , pRet) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcRegistryLocalClearFunRegistry() {
   return CBaIpcRegistry::SClearFunRegistry() ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcRegistryLocalRegisterVar(const char* name, TBaIpcRegVar var) {
   if (!name) {
      return eBaBoolRC_Error;
   }

   return CBaIpcRegistry::SRegisterVar(name, var) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcRegistryLocalUnregisterVar(const char* name) {
   if (!name) {
      return eBaBoolRC_Error;
   }

   return CBaIpcRegistry::SUnregisterVar(name) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcRegistryLocalCallVar(const char* name, TBaIpcRegVarOut *pVar) {
   if (!name || !pVar) {
      return eBaBoolRC_Error;
   }

   return CBaIpcRegistry::SCallVar(name, *pVar) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcRegistryLocalSetVar(const char* name, TBaIpcRegVar var) {
   if (!name) {
      return eBaBoolRC_Error;
   }

   return CBaIpcRegistry::SSetVar(name, var) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

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
TBaBoolRC BaIpcRegistryClearFunReg(TBaIpcRegistryHdl hdl) {
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

//
//
TBaBoolRC BaIpcRegistryCallVar(TBaIpcRegistryHdl hdl, const char* name, TBaIpcRegVarOut *pVar) {
   if (!hdl || !name || !pVar) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->CallVar(name, *pVar) ? eBaBoolRC_Success : eBaBoolRC_Error;
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
