/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : OsIpcRegistry.cpp
 *   Date     : 17.03.2017
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <string.h>
#include "OsIpcRegistry.h"
#include "COsIpcRegistry.h"
#include "BaBool.h"
/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define C_HDL_ ((IOsIpcRegistry*) hdl)

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/

//
TBaBoolRC OsIpcRegistryLocalInit() {
   return COsIpcRegistry::SInitRegistry() ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistryLocalExit() {
   return COsIpcRegistry::SExitRegistry() ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistryLocalRegisterFun(const char* name, TOsIpcRegFun fun) {
   if (!name) {
      return eBaBoolRC_Error;
   }

   return COsIpcRegistry::SRegisterFun(name, fun) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistryLocalUnregisterFun(const char* name) {
   if (!name) {
      return eBaBoolRC_Error;
   }

   return COsIpcRegistry::SUnregisterFun(name) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistryLocalCallFun(const char* name, TOsIpcFunArg a, TOsIpcArg *pRet) {
   if (!name) {
      return eBaBoolRC_Error;
   }

   return COsIpcRegistry::SCallFun(name, a , pRet) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistryLocalClearFunReg() {
   return COsIpcRegistry::SClearFunRegistry() ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistryLocalRegisterVar(const char* name, const TOsIpcRegVar *pVar) {
   if (!name || !pVar) {
      return eBaBoolRC_Error;
   }

   return COsIpcRegistry::SRegisterVar(name, *pVar) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistryLocalUnregisterVar(const char* name) {
   if (!name) {
      return eBaBoolRC_Error;
   }

   return COsIpcRegistry::SUnregisterVar(name) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistryLocalClearVarReg() {
   return COsIpcRegistry::SClearVarRegistry() ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistryLocalCallVar(const char* name, TOsIpcRegVarOut *pVar) {
   if (!name || !pVar) {
      return eBaBoolRC_Error;
   }

   return COsIpcRegistry::SCallVar(name, *pVar) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistryLocalSetVar(const char* name, const TOsIpcRegVar *pVar) {
   if (!name || !pVar) {
      return eBaBoolRC_Error;
   }

   return COsIpcRegistry::SSetVar(name, *pVar) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TOsIpcRegistryHdl OsIpcRegistryCreate() {
   return COsIpcRegistry::Create();
}

//
TBaBoolRC OsIpcRegistryDestroy(TOsIpcRegistryHdl hdl) {
   return IOsIpcRegistryDestroy(C_HDL_);
}

//
TBaBoolRC OsIpcRegistryRegisterFun(TOsIpcRegistryHdl hdl, const char* name,
      TOsIpcRegFun fun) {
   if (!hdl || !name) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->RegisterFun(name, fun) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistryUnregisterFun(TOsIpcRegistryHdl hdl, const char* name) {
   if (!hdl || !name) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->UnregisterFun(name) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistryClearFunReg(TOsIpcRegistryHdl hdl) {
   if (!hdl) {
      return eBaBoolRC_Error;
   }
   C_HDL_->ClearFunRegistry();

   return  eBaBoolRC_Success;
}

//
TBaBoolRC OsIpcRegistryCallFun(TOsIpcRegistryHdl hdl, const char* name,
      TOsIpcFunArg a, TOsIpcArg *pRet) {
   if (!hdl || !name) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->CallFun(name, a, pRet) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistryRegisterVar(TOsIpcRegistryHdl hdl, const char* name,
      const TOsIpcRegVar *pVar) {
   if (!hdl || !name) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->RegisterVar(name, *pVar) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistryUnregisterVar(TOsIpcRegistryHdl hdl, const char* name) {
   if (!hdl || !name) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->UnregisterVar(name) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistryClearVarReg(TOsIpcRegistryHdl hdl) {
   if (!hdl) {
      return eBaBoolRC_Error;
   }

   C_HDL_->ClearVarRegistry();
   return eBaBoolRC_Success;
}

//
TBaBoolRC OsIpcRegistryCallVar(TOsIpcRegistryHdl hdl, const char* name, TOsIpcRegVarOut *pVar) {
   if (!hdl || !name || !pVar) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->CallVar(name, *pVar) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcRegistrySetVar(TOsIpcRegistryHdl hdl, const char* name,
      const TOsIpcRegVar *pVar
      ) {
   if (!hdl || !name || !pVar) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->SetVar(name, *pVar) ? eBaBoolRC_Success : eBaBoolRC_Error;

}


/*------------------------------------------------------------------------------
    C++ Factories
 -----------------------------------------------------------------------------*/
//
IOsIpcRegistry * IOsIpcRegistryCreate() {
   return COsIpcRegistry::Create();
}

//
TBaBoolRC IOsIpcRegistryDestroy(IOsIpcRegistry *pHdl) {
   COsIpcRegistry *p = dynamic_cast<COsIpcRegistry*>(pHdl);
   if (p) {
      return COsIpcRegistry::Destroy(p) ? eBaBoolRC_Success : eBaBoolRC_Error;
   }

   return eBaBoolRC_Error;
}

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
