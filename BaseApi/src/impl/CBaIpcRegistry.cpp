/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CBaIpcRegistry.cpp
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
#include <string.h>
#include "CBaIpcRegistry.h"
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


LOCAL bool callVoid(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i);

template <typename TA1>
LOCAL bool callVoid(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
      TA1 a1);

template <typename TA1, typename TA2>
LOCAL bool callVoid(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2);

template <typename TA1, typename TA2, typename TA3>
LOCAL bool callVoid(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, TA3 a3);

template <typename TA1, typename TA2, typename TA3, typename TA4>
LOCAL bool callVoid(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, TA3 a3, TA4 a4);

template <typename TR>
LOCAL TR callAny(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
      bool &rRet);

template <typename TR, typename TA1>
LOCAL TR callAny(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
      TA1 a1, bool &rRet);

template <typename TR, typename TA1, typename TA2>
LOCAL TR callAny(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, bool &rRet);

template <typename TR, typename TA1, typename TA2, typename TA3>
LOCAL TR callAny(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, TA3 a3, bool &rRet);

template <typename TR, typename TA1, typename TA2, typename TA3, typename TA4>
LOCAL TR callAny(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, TA3 a3, TA4 a4, bool &rRet);

/*------------------------------------------------------------------------------
    Local variables
 -----------------------------------------------------------------------------*/
static CBaIpcRegistry* spReg = CBaIpcRegistry::Create();


/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

//
CBaIpcRegistry* CBaIpcRegistry::Create() {
   return new CBaIpcRegistry();
}

//
bool CBaIpcRegistry::Destroy(CBaIpcRegistry* pHdl) {
   delete pHdl;
   return true;
}

//
bool CBaIpcRegistry::SInitRegistry() {
   if (spReg) {
      return true;
   }

   spReg = CBaIpcRegistry::Create();
   return spReg ? true : false;
}

//
bool CBaIpcRegistry::SExitRegistry() {
   if (!spReg) {
      return true;
   }
   CBaIpcRegistry* p = spReg;
   spReg = 0;
   return CBaIpcRegistry::Destroy(p);
}

//
bool CBaIpcRegistry::SRegisterFun(std::string name, TBaIpcRegFun fun) {
   if (!spReg) {
      return false;
   }

   return spReg->RegisterFun(name, fun);
}

//
bool CBaIpcRegistry::SUnregisterFun(std::string name) {
   if (!spReg) {
      return false;
   }

   return spReg->UnregisterFun(name);
}

//
bool CBaIpcRegistry::SCallFun(std::string name, TBaIpcFunArg a, TBaIpcArg *pRet) {
   if (!spReg) {
      return false;
   }

   return spReg->CallFun(name, a, pRet);
}

//
bool CBaIpcRegistry::SClearFunRegistry() {
   if (!spReg) {
      return false;
   }

   spReg->ClearFunRegistry();
   return true;
}

//
bool CBaIpcRegistry::SRegisterVar(std::string name, const TBaIpcRegVar &rVar) {
   if (!spReg) {
      return false;
   }

   return spReg->RegisterVar(name, rVar);
}

//
bool CBaIpcRegistry::SUnregisterVar(std::string name) {
   if (!spReg) {
      return false;
   }

   return spReg->UnregisterVar(name);
}

//
bool CBaIpcRegistry::SClearVarRegistry() {
   if (!spReg) {
      return false;
   }

   spReg->ClearVarRegistry();
   return true;
}

//
bool CBaIpcRegistry::SCallVar(std::string name, TBaIpcRegVarOut &rVar) {
   if (!spReg) {
      return false;
   }

   return spReg->CallVar(name, rVar);
}

//
bool CBaIpcRegistry::SCallVarInternal(std::string name, TBaIpcRegVar &rVar) {
   if (!spReg) {
      return false;
   }

   return spReg->CallVarInternal(name, rVar);
}

//
bool CBaIpcRegistry::SSetVar(std::string name, const TBaIpcRegVar &rVar) {
   if (!spReg) {
      return false;
   }

   return spReg->SetVar(name, rVar);
}

//
bool CBaIpcRegistry::CallFun(std::string name, TBaIpcFunArg a, TBaIpcArg *pRet) {
   auto it = mFunReg.find(name);
   if (it == mFunReg.end()) {
      return false;
   }

   // Output
   bool rc = false;
   TBaIpcArg tRet = {0};
   pRet = pRet ? pRet : &tRet;

   TBaIpcRegFun fun = it->second;

   if (!fun.type || strlen(fun.type) < 3 || fun.type[1] != ':' || strlen(fun.type) > BAIPCMAXARG + 2) {
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
bool CBaIpcRegistry::CallVar(std::string name, TBaIpcRegVarOut &rVar) {
   TBaIpcRegVar var = {0};


   if (!CallVarInternal(name, var) || !var.pVar || var.sz > BAIPCMAXVARSZ) {
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
bool CBaIpcRegistry::CallVarInternal(std::string name, TBaIpcRegVar &rVar) {
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
bool CBaIpcRegistry::SetVar(std::string name, const TBaIpcRegVar &rVar) {
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
LOCAL TR callAny(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, TA3 a3, TA4 a4, bool &rRet) {
   if (i+2 >= strlen(rFun.type)) {
      rRet = true;
      return ((TR(*)(TA1, TA2, TA3, TA4))rFun.pFun)(a1, a2, a3, a4);
   }
   return 0;
}

//
template <typename TR, typename TA1, typename TA2, typename TA3>
LOCAL TR callAny(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
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
LOCAL TR callAny(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
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
LOCAL TR callAny(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
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
LOCAL TR callAny(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
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
LOCAL bool callVoid(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
      TA1 a1, TA2 a2, TA3 a3, TA4 a4) {
   if (i+2 >= strlen(rFun.type)) {
      ((void (*)(TA1, TA2, TA3, TA4))rFun.pFun)(a1, a2, a3, a4);
      return true;
   }

   return false;
}

//
template <typename TA1, typename TA2, typename TA3>
LOCAL bool callVoid(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
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
LOCAL bool callVoid(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
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
LOCAL bool callVoid(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i,
      TA1 a1) {
   if (i+2 >= strlen(rFun.type)) {
      ((void (*)(TA1))rFun.pFun)(a1);
      return true;
   }

   ANYVOIDSWITCH(a1);
   return false;
}

//
LOCAL bool callVoid(const TBaIpcRegFun &rFun, const TBaIpcFunArg &as, uint32_t i) {
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
