/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : COsIpcRegistry.cpp
 *   Date     : Nov 24, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */


/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <COsIpcRegistry.h>
#include <string.h>
#include "BaLogMacros.h"
#include "BaGenMacros.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define TAG "IpcReg"
#define ANYVOIDSWITCH(args...)                          \
   switch (rFun.type[i+2]) {                            \
   case 'i':                                            \
      return callVoid(rFun, as, i+1, args, as.a[i].i);  \
   case 'I':                                            \
      return callVoid(rFun, as, i+1, args, as.a[i].I);  \
   case 'd':                                            \
      return callVoid(rFun, as, i+1, args, as.a[i].d);  \
   case 'f':                                            \
      return callVoid(rFun, as, i+1, args, as.a[i].f);  \
                                                        \
    default:                                            \
      break;                                            \
   }

#define ANYCALLSWITCH(args...)                                     \
   switch (rFun.type[i+2]) {                                       \
   case 'i':                                                       \
      return callAny<TR>(rFun, as, i+1, args, as.a[i].i, rRet);    \
   case 'I':                                                       \
      return callAny<TR>(rFun, as, i+1, args, as.a[i].I, rRet);    \
   case 'd':                                                       \
      return callAny<TR>(rFun, as, i+1, args, as.a[i].d, rRet);    \
   case 'f':                                                       \
      return callAny<TR>(rFun, as, i+1, args, as.a[i].f, rRet);    \
                                                                   \
    default:                                                       \
      break;                                                       \
   }

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/


LOCAL bool callVoid(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i);

template <typename TA1>
LOCAL bool callVoid(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1);

template <typename TA1, typename TA2>
LOCAL bool callVoid(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2);

template <typename TA1, typename TA2, typename TA3>
LOCAL bool callVoid(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, TA3 a3);

template <typename TA1, typename TA2, typename TA3, typename TA4>
LOCAL bool callVoid(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, TA3 a3, TA4 a4);

template <typename TR>
LOCAL TR callAny(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      bool &rRet);

template <typename TR, typename TA1>
LOCAL TR callAny(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1, bool &rRet);

template <typename TR, typename TA1, typename TA2>
LOCAL TR callAny(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, bool &rRet);

template <typename TR, typename TA1, typename TA2, typename TA3>
LOCAL TR callAny(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, TA3 a3, bool &rRet);

template <typename TR, typename TA1, typename TA2, typename TA3, typename TA4>
LOCAL TR callAny(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, TA3 a3, TA4 a4, bool &rRet);

/*------------------------------------------------------------------------------
    Local variables
 -----------------------------------------------------------------------------*/
static COsIpcRegistry* spReg = COsIpcRegistry::Create();


/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

//
COsIpcRegistry* COsIpcRegistry::Create() {
   return new COsIpcRegistry();
}

//
bool COsIpcRegistry::Destroy(COsIpcRegistry* pHdl) {
   delete pHdl;
   return true;
}

//
bool COsIpcRegistry::SInitRegistry() {
   if (spReg) {
      return true;
   }

   spReg = COsIpcRegistry::Create();
   return spReg ? true : false;
}

//
bool COsIpcRegistry::SExitRegistry() {
   if (!spReg) {
      return true;
   }
   COsIpcRegistry* p = spReg;
   spReg = 0;
   return COsIpcRegistry::Destroy(p);
}

//
bool COsIpcRegistry::SRegisterFun(std::string name, TOsIpcRegFun fun) {
   if (!spReg) {
      return false;
   }

   return spReg->RegisterFun(name, fun);
}

//
bool COsIpcRegistry::SUnregisterFun(std::string name) {
   if (!spReg) {
      return false;
   }

   return spReg->UnregisterFun(name);
}

//
bool COsIpcRegistry::SCallFun(std::string name, TOsIpcFunArg a, TOsIpcArg *pRet) {
   if (!spReg) {
      return false;
   }

   return spReg->CallFun(name, a, pRet);
}

//
bool COsIpcRegistry::SClearFunRegistry() {
   if (!spReg) {
      return false;
   }

   spReg->ClearFunRegistry();
   return true;
}

//
bool COsIpcRegistry::SRegisterVar(std::string name, const TOsIpcRegVar &rVar) {
   if (!spReg) {
      return false;
   }

   return spReg->RegisterVar(name, rVar);
}

//
bool COsIpcRegistry::SUnregisterVar(std::string name) {
   if (!spReg) {
      return false;
   }

   return spReg->UnregisterVar(name);
}

//
bool COsIpcRegistry::SClearVarRegistry() {
   if (!spReg) {
      return false;
   }

   spReg->ClearVarRegistry();
   return true;
}

//
bool COsIpcRegistry::SCallVar(std::string name, TOsIpcRegVarOut &rVar) {
   if (!spReg) {
      return false;
   }

   return spReg->CallVar(name, rVar);
}

//
bool COsIpcRegistry::SCallVarInternal(std::string name, TOsIpcRegVar &rVar) {
   if (!spReg) {
      return false;
   }

   return spReg->CallVarInternal(name, rVar);
}

//
bool COsIpcRegistry::SSetVar(std::string name, const TOsIpcRegVar &rVar) {
   if (!spReg) {
      return false;
   }

   return spReg->SetVar(name, rVar);
}

//
bool COsIpcRegistry::CallFun(std::string name, TOsIpcFunArg a, TOsIpcArg *pRet) {
   auto it = mFunReg.find(name);
   if (it == mFunReg.end()) {
      return false;
   }

   // Output
   bool rc = false;
   TOsIpcArg tRet = {0};
   pRet = pRet ? pRet : &tRet;

   TOsIpcRegFun fun = it->second;

   if (!fun.type || strlen(fun.type) < 3 || fun.type[1] != ':' || strlen(fun.type) > OSIPC_MAXARG + 2) {
      return false;
   }

   // 'I' has pointer, I8, I16, I32 and I64. Also unsigned
   // d and f have to be handled separately.

   // todo: use memcopy to copy the whole 8 bytes of return value?
   switch (fun.type[0]) {
      case 'v':
         return callVoid(fun, a, 0);
         break;
      case 'i':
         pRet->i = callAny<int32_t>(fun, a, 0, rc);
         return rc;
      case 'I':
         pRet->I = callAny<int64_t>(fun, a, 0, rc);
         return rc;
      case 'f':
         pRet->f = callAny<float>(fun, a, 0, rc);
         return rc;
      case 'd':
         pRet->d = callAny<double>(fun, a, 0, rc);
         return rc;
      default : return false;
   }

   return false;
};


//
bool COsIpcRegistry::CallVar(std::string name, TOsIpcRegVarOut &rVar) {
   TOsIpcRegVar var = {0};


   if (!CallVarInternal(name, var) || !var.pVar || var.sz > OSIPC_MAXVARSZ) {
      return false;
   }

   // Set the output memory to null to avoid trouble
   memset(&rVar, 0, sizeof(rVar));

   // Copy the data from the registry pointer to the output data
   memcpy(rVar.dat.data, var.pVar, var.sz);
   rVar.sz = var.sz;
   return true;
}

//
bool COsIpcRegistry::CallVarInternal(std::string name, TOsIpcRegVar &rVar) {
   if (name == "") {
      return false;
   }

   auto it = mVarReg.find(name);
   if (it == mVarReg.end() || !varIsValid(it->second)) {
      return false;
   }


   rVar = (it->second);
   return true;
}

//
bool COsIpcRegistry::SetVar(std::string name, const TOsIpcRegVar &rVar) {
   if (name == "" || rVar.wr != eBaBool_true || !varIsValid(rVar)) {
      return false;
   }

   auto it = mVarReg.find(name);
   if (it == mVarReg.end() || !varIsValid(it->second)) {
      return false;
   }

   // Do not allow to write more than the original size
   if (it->second.sz < rVar.sz) {
      return false;
   }

   memcpy(it->second.pVar, rVar.pVar, rVar.sz);
   return true;
}

//
template <typename TR, typename TA1, typename TA2, typename TA3, typename TA4>
LOCAL TR callAny(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, TA3 a3, TA4 a4, bool &rRet) {
   if (i+2 >= strlen(rFun.type)) {
      rRet = true;
      return ((TR(*)(TA1, TA2, TA3, TA4))rFun.pFun)(a1, a2, a3, a4);
   }
   return 0;
}

//
template <typename TR, typename TA1, typename TA2, typename TA3>
LOCAL TR callAny(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, TA3 a3, bool &rRet) {

   if (i+2 >= strlen(rFun.type)) {
      rRet = true;
      return ((TR(*)(TA1, TA2, TA3))rFun.pFun)(a1, a2, a3);
   }

   ANYCALLSWITCH(a1, a2, a3);
   return 0;
}

//
template <typename TR, typename TA1, typename TA2>
LOCAL TR callAny(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, bool &rRet) {
   if (i+2 >= strlen(rFun.type)) {
      rRet = true;
      return ((TR(*)(TA1, TA2))rFun.pFun)(a1, a2);
   }

   ANYCALLSWITCH(a1, a2);
   return 0;
}

//
template <typename TR, typename TA1>
LOCAL TR callAny(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1, bool &rRet) {
   if (i+2 >= strlen(rFun.type)) {
      rRet = true;
      return ((TR(*)(TA1))rFun.pFun)(a1);
   }

   ANYCALLSWITCH(a1);
   return 0;
}

//
template <typename TR>
LOCAL TR callAny(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      bool &rRet) {
   if (i+2 >= strlen(rFun.type)) {
      rRet = true;
      return ((TR(*)())rFun.pFun)();
   }

   switch (rFun.type[i+2]) {
   case 'i':
      return callAny<TR>(rFun, as, i+1, as.a[i].i, rRet);
   case 'I':
      return callAny<TR>(rFun, as, i+1, as.a[i].I, rRet);
   case 'd':
      return callAny<TR>(rFun, as, i+1, as.a[i].d, rRet);
   case 'f':
      return callAny<TR>(rFun, as, i+1, as.a[i].f, rRet);
   case 'v':
      rRet = true;
      return ((TR(*)())rFun.pFun)();
    default:
      break;
   }
   return 0;
}

//
template <typename TA1, typename TA2, typename TA3, typename TA4>
LOCAL bool callVoid(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, TA3 a3, TA4 a4) {
   if (i+2 >= strlen(rFun.type)) {
      ((void (*)(TA1, TA2, TA3, TA4))rFun.pFun)(a1, a2, a3, a4);
      return true;
   }

   return false;
}

//
template <typename TA1, typename TA2, typename TA3>
LOCAL bool callVoid(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, TA3 a3) {
   if (i+2 >= strlen(rFun.type)) {
      ((void (*)(TA1, TA2, TA3))rFun.pFun)(a1, a2, a3);
      return true;
   }

   ANYVOIDSWITCH(a1, a2, a3)
   return false;
}

//
template <typename TA1, typename TA2>
LOCAL bool callVoid(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2) {
   if (i+2 >= strlen(rFun.type)) {
      ((void (*)(TA1, TA2))rFun.pFun)(a1, a2);
      return true;
   }

   ANYVOIDSWITCH(a1, a2);
   return false;
}

//
template <typename TA1>
LOCAL bool callVoid(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i,
      TA1 a1) {
   if (i+2 >= strlen(rFun.type)) {
      ((void (*)(TA1))rFun.pFun)(a1);
      return true;
   }

   ANYVOIDSWITCH(a1);
   return false;
}

//
LOCAL bool callVoid(const TOsIpcRegFun &rFun, const TOsIpcFunArg &as, uint32_t i) {
   if (i+2 >= strlen(rFun.type)) {
      ((void (*)())rFun.pFun)();
      return true;
   }

   switch (rFun.type[i+2]) {
   case 'i':
      return callVoid(rFun, as, i+1, as.a[i].i);
   case 'I':
      return callVoid(rFun, as, i+1, as.a[i].I);
   case 'd':
      return callVoid(rFun, as, i+1, as.a[i].d);
   case 'f':
      return callVoid(rFun, as, i+1, as.a[i].f);
   case 'v':
      ((void (*)(void))rFun.pFun)();
      return true;
    default:
      break;
   }
   return false;
}

