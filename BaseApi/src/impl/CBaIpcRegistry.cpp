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
#include "CBaIpcRegistry.h"
#include "BaLogMacros.h"
#include "BaGenMacros.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define TAG "IpcReg"

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
LOCAL bool callVoid(const TBaIpcRegFun &rFun, const TBaIpcFunArg &rA);

template <typename T>
LOCAL bool callAny(const TBaIpcRegFun &rFun, const TBaIpcFunArg &rA, T &rRet);

/*------------------------------------------------------------------------------
    Local variables
 -----------------------------------------------------------------------------*/
static CBaIpcRegistry* spReg = CBaIpcRegistry::Create();

/*------------------------------------------------------------------------------
    Test function
 -----------------------------------------------------------------------------*/
double TestRegFun(uint32_t i, float f) {
   return f + i;
}

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
bool CBaIpcRegistry::SRegisterFun(std::string name, TBaIpcRegFun fun) {
   return spReg->RegisterFun(name, fun);;
}

//
bool CBaIpcRegistry::SCallFun(std::string name, TBaIpcFunArg a, TBaIpcArg *pRet) {
   return spReg->CallFun(name, a, pRet);
}

//
bool CBaIpcRegistry::CallFun(std::string name, TBaIpcFunArg a, TBaIpcArg *pRet) {
   auto it = mFunReg.find(name);
   if (it == mFunReg.end()) {
      return false;
   }

   // Output
   TBaIpcArg tRet = {0};
   pRet = pRet ? pRet : &tRet;

   TBaIpcRegFun fun = it->second;

   // 'I' has pointer, I8, I16, I32 and I64. Also unsigned
   // d and f have to be handled separately.

   // todo: use memcopy to copy the whole 8 bytes of return value?
   switch (fun.type[0]) {
      case 'v':
         callVoid(fun, a);
         return true;
         break;
      case 'I':
         callAny<int64_t>(fun, a, pRet->I);
         return true;
      case 'f':
         callAny<float>(fun, a, pRet->f);
         break;
      case 'd':
         callAny<double>(fun, a, pRet->d);
         break;
      default : return false;
   }

   return false;
};

// "v:idf"
LOCAL bool callVoid(const TBaIpcRegFun &rFun, const TBaIpcFunArg &rA) {

   std::string sType = rFun.type;

   //
   if (rFun.type[1] != ':') {
      // TODO: error
      return false;
   }

   // No parameters
   if (rFun.type[2] == 'v')      { ((void(*)()) rFun.pFun)(); return true; }

   // 3
   if (sType.substr(2) == "I")   { ((void(*)(int64_t))rFun.pFun)(rA.a[0].I); return true; }
   if (sType.substr(2) == "f")   { ((void(*)(float))  rFun.pFun)(rA.a[0].f); return true; }
   if (sType.substr(2) == "d")   { ((void(*)(double)) rFun.pFun)(rA.a[0].d); return true; }

   // 3 x 3 = 9
   if (sType.substr(2) == "II")  { ((void(*)(int64_t, int64_t))rFun.pFun)(rA.a[0].I, rA.a[1].I); return true; }
   if (sType.substr(2) == "If")  { ((void(*)(int64_t, float))  rFun.pFun)(rA.a[0].I, rA.a[1].f); return true; }
   if (sType.substr(2) == "Id")  { ((void(*)(int64_t, double)) rFun.pFun)(rA.a[0].I, rA.a[1].d); return true; }

   if (sType.substr(2) == "fI")  { ((void(*)(int64_t, int64_t))rFun.pFun)(rA.a[0].f, rA.a[1].I); return true; }
   if (sType.substr(2) == "ff")  { ((void(*)(int64_t, float))  rFun.pFun)(rA.a[0].f, rA.a[1].f); return true; }
   if (sType.substr(2) == "fd")  { ((void(*)(int64_t, double)) rFun.pFun)(rA.a[0].f, rA.a[1].d); return true; }

   if (sType.substr(2) == "dI")  { ((void(*)(int64_t, int64_t))rFun.pFun)(rA.a[0].d, rA.a[1].I); return true; }
   if (sType.substr(2) == "df")  { ((void(*)(int64_t, float))  rFun.pFun)(rA.a[0].d, rA.a[1].f); return true; }
   if (sType.substr(2) == "dd")  { ((void(*)(int64_t, double)) rFun.pFun)(rA.a[0].d, rA.a[1].d); return true; }

   // 3 x 3 x 3 = 27
   if (sType.substr(2) == "III") { ((void(*)(int64_t, int64_t, int64_t))rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].I); return true; }
   if (sType.substr(2) == "IIf") { ((void(*)(int64_t, int64_t, float))  rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].f); return true; }
   if (sType.substr(2) == "IId") { ((void(*)(int64_t, int64_t, double)) rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].d); return true; }

   if (sType.substr(2) == "IfI") { ((void(*)(int64_t, float, int64_t))rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].I); return true; }
   if (sType.substr(2) == "Iff") { ((void(*)(int64_t, float, float))  rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].f); return true; }
   if (sType.substr(2) == "Ifd") { ((void(*)(int64_t, float, double)) rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].d); return true; }

   if (sType.substr(2) == "IdI") { ((void(*)(int64_t, double, int64_t))rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].I); return true; }
   if (sType.substr(2) == "Idf") { ((void(*)(int64_t, double, float))  rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].f); return true; }
   if (sType.substr(2) == "Idd") { ((void(*)(int64_t, double, double)) rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].d); return true; }
   //
   if (sType.substr(2) == "fII") { ((void(*)(float, int64_t, int64_t))rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].I); return true; }
   if (sType.substr(2) == "fIf") { ((void(*)(float, int64_t, float))  rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].f); return true; }
   if (sType.substr(2) == "fId") { ((void(*)(float, int64_t, double)) rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].d); return true; }

   if (sType.substr(2) == "ffI") { ((void(*)(float, float, int64_t))rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].I); return true; }
   if (sType.substr(2) == "fff") { ((void(*)(float, float, float))  rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].f); return true; }
   if (sType.substr(2) == "ffd") { ((void(*)(float, float, double)) rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].d); return true; }

   if (sType.substr(2) == "fdI") { ((void(*)(float, double, int64_t))rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].I); return true; }
   if (sType.substr(2) == "fdf") { ((void(*)(float, double, float))  rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].f); return true; }
   if (sType.substr(2) == "fdd") { ((void(*)(float, double, double)) rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].d); return true; }
   //
   if (sType.substr(2) == "dII") { ((void(*)(double, int64_t, int64_t))rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].I); return true; }
   if (sType.substr(2) == "dIf") { ((void(*)(double, int64_t, float))  rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].f); return true; }
   if (sType.substr(2) == "dId") { ((void(*)(double, int64_t, double)) rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].d); return true; }

   if (sType.substr(2) == "dfI") { ((void(*)(double, float, int64_t))rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].I); return true; }
   if (sType.substr(2) == "dff") { ((void(*)(double, float, float))  rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].f); return true; }
   if (sType.substr(2) == "dfd") { ((void(*)(double, float, double)) rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].d); return true; }

   if (sType.substr(2) == "ddI") { ((void(*)(double, double, int64_t))rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].I); return true; }
   if (sType.substr(2) == "ddf") { ((void(*)(double, double, float))  rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].f); return true; }
   if (sType.substr(2) == "ddd") { ((void(*)(double, double, double)) rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].d); return true; }

   // TODO: here error!
   return false;
}

// "I:idf"
template <typename T>
LOCAL bool callAny(const TBaIpcRegFun &rFun, const TBaIpcFunArg &rA, T &rRet) {
   std::string sType = rFun.type;

   //
   if (rFun.type[1] != ':') {
      // TODO: error
      return 0;
   }

   // No parameters
   if (rFun.type[2] == 'v')      { rRet =  ((T(*)()) rFun.pFun)(); return true; }

   // 3
   if (sType.substr(2) == "I")   { rRet = ((T(*)(int64_t)) rFun.pFun)(rA.a[0].I); return true; }
   if (sType.substr(2) == "f")   { rRet = ((T(*)(float))   rFun.pFun)(rA.a[0].f); return true; }
   if (sType.substr(2) == "d")   { rRet = ((T(*)(double))  rFun.pFun)(rA.a[0].d); return true; }

   // 3 x 3 = 9
   if (sType.substr(2) == "II")  { rRet = ((T(*)(int64_t, int64_t)) rFun.pFun)(rA.a[0].I, rA.a[1].I); return true; }
   if (sType.substr(2) == "If")  { rRet = ((T(*)(int64_t, float))   rFun.pFun)(rA.a[0].I, rA.a[1].f); return true; }
   if (sType.substr(2) == "Id")  { rRet = ((T(*)(int64_t, double))  rFun.pFun)(rA.a[0].I, rA.a[1].d); return true; }
   if (sType.substr(2) == "fI")  { rRet = ((T(*)(int64_t, int64_t)) rFun.pFun)(rA.a[0].f, rA.a[1].I); return true; }
   if (sType.substr(2) == "ff")  { rRet = ((T(*)(int64_t, float))   rFun.pFun)(rA.a[0].f, rA.a[1].f); return true; }
   if (sType.substr(2) == "fd")  { rRet = ((T(*)(int64_t, double))  rFun.pFun)(rA.a[0].f, rA.a[1].d); return true; }
   if (sType.substr(2) == "dI")  { rRet = ((T(*)(int64_t, int64_t)) rFun.pFun)(rA.a[0].d, rA.a[1].I); return true; }
   if (sType.substr(2) == "df")  { rRet = ((T(*)(int64_t, float))   rFun.pFun)(rA.a[0].d, rA.a[1].f); return true; }
   if (sType.substr(2) == "dd")  { rRet = ((T(*)(int64_t, double))  rFun.pFun)(rA.a[0].d, rA.a[1].d); return true; }

   // 3 x 3 x 3 = 27
   if (sType.substr(2) == "III") { rRet = ((T(*)(int64_t, int64_t, int64_t)) rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].I); return true; }
   if (sType.substr(2) == "IIf") { rRet = ((T(*)(int64_t, int64_t, float))   rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].f); return true; }
   if (sType.substr(2) == "IId") { rRet = ((T(*)(int64_t, int64_t, double))  rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].d); return true; }
   if (sType.substr(2) == "IfI") { rRet = ((T(*)(int64_t, float, int64_t))   rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].I); return true; }
   if (sType.substr(2) == "Iff") { rRet = ((T(*)(int64_t, float, float))     rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].f); return true; }
   if (sType.substr(2) == "Ifd") { rRet = ((T(*)(int64_t, float, double))    rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].d); return true; }
   if (sType.substr(2) == "IdI") { rRet = ((T(*)(int64_t, double, int64_t))  rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].I); return true; }
   if (sType.substr(2) == "Idf") { rRet = ((T(*)(int64_t, double, float))    rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].f); return true; }
   if (sType.substr(2) == "Idd") { rRet = ((T(*)(int64_t, double, double))   rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].d); return true; }
   if (sType.substr(2) == "fII") { rRet = ((T(*)(float, int64_t, int64_t))   rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].I); return true; }
   if (sType.substr(2) == "fIf") { rRet = ((T(*)(float, int64_t, float))     rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].f); return true; }
   if (sType.substr(2) == "fId") { rRet = ((T(*)(float, int64_t, double))    rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].d); return true; }
   if (sType.substr(2) == "ffI") { rRet = ((T(*)(float, float, int64_t))     rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].I); return true; }
   if (sType.substr(2) == "fff") { rRet = ((T(*)(float, float, float))       rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].f); return true; }
   if (sType.substr(2) == "ffd") { rRet = ((T(*)(float, float, double))      rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].d); return true; }
   if (sType.substr(2) == "fdI") { rRet = ((T(*)(float, double, int64_t))    rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].I); return true; }
   if (sType.substr(2) == "fdf") { rRet = ((T(*)(float, double, float))      rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].f); return true; }
   if (sType.substr(2) == "fdd") { rRet = ((T(*)(float, double, double))     rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].d); return true; }
   if (sType.substr(2) == "dII") { rRet = ((T(*)(double, int64_t, int64_t))  rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].I); return true; }
   if (sType.substr(2) == "dIf") { rRet = ((T(*)(double, int64_t, float))    rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].f); return true; }
   if (sType.substr(2) == "dId") { rRet = ((T(*)(double, int64_t, double))   rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].d); return true; }
   if (sType.substr(2) == "dfI") { rRet = ((T(*)(double, float, int64_t))    rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].I); return true; }
   if (sType.substr(2) == "dff") { rRet = ((T(*)(double, float, float))      rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].f); return true; }
   if (sType.substr(2) == "dfd") { rRet = ((T(*)(double, float, double))     rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].d); return true; }
   if (sType.substr(2) == "ddI") { rRet = ((T(*)(double, double, int64_t))   rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].I); return true; }
   if (sType.substr(2) == "ddf") { rRet = ((T(*)(double, double, float))     rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].f); return true; }
   if (sType.substr(2) == "ddd") { rRet = ((T(*)(double, double, double))    rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].d); return true; }

   return false;
}


