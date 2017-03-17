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
LOCAL void callVoid(const TBaIpcRegFun &rFun, const TBaIpcFunArg &rA);

template <typename T>
LOCAL T callAny(const TBaIpcRegFun &rFun, const TBaIpcFunArg &rA);

/*------------------------------------------------------------------------------
    Local variables
 -----------------------------------------------------------------------------*/


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
bool CBaIpcRegistry::CallFun(std::string name, TBaIpcFunArg a, TBaIpcArg *pOut) {
   auto it = mFunReg.find(name);
   if (it == mFunReg.end()) {
      return false;
   }

   // Output
   TBaIpcArg tOut = {0};
   pOut = pOut ? pOut : &tOut;

   TBaIpcRegFun fun = it->second;

   // 'I' has pointer, I8, I16, I32 and I64. Also unsigned
   // d and f have to be handled separately.
   switch (fun.type[0]) {
      case 'v':
         callVoid(fun, a);
         return true;
         break;
      case 'I':
         pOut->I = callAny<int64_t>(fun, a);
         return true;
      case 'f':
         pOut->f = callAny<float>(fun, a);
         break;
      case 'd':
         pOut->d = callAny<double>(fun, a);
         break;
      default : return false;
   }

   return false;
};


// "v:idf"
LOCAL void callVoid(const TBaIpcRegFun &rFun, const TBaIpcFunArg &rA) {

   std::string sType = rFun.type;

   //
   if (rFun.type[1] != ':') {
      // TODO: error
      return;
   }

   // No parameters
   if (rFun.type[2] == 'v')          { ((void(*)()) rFun.pFun)(); return; }

   // 3
   if (sType.substr(2) == "I")   { ((void(*)(int64_t))rFun.pFun)(rA.a[0].I); return; }
   if (sType.substr(2) == "f")   { ((void(*)(float))  rFun.pFun)(rA.a[0].f); return; }
   if (sType.substr(2) == "d")   { ((void(*)(double)) rFun.pFun)(rA.a[0].d); return; }

   // 3 x 3 = 9
   if (sType.substr(2) == "II")  { ((void(*)(int64_t, int64_t))rFun.pFun)(rA.a[0].I, rA.a[1].I); return; }
   if (sType.substr(2) == "If")  { ((void(*)(int64_t, float))  rFun.pFun)(rA.a[0].I, rA.a[1].f); return; }
   if (sType.substr(2) == "Id")  { ((void(*)(int64_t, double)) rFun.pFun)(rA.a[0].I, rA.a[1].d); return; }

   if (sType.substr(2) == "fI")  { ((void(*)(int64_t, int64_t))rFun.pFun)(rA.a[0].f, rA.a[1].I); return; }
   if (sType.substr(2) == "ff")  { ((void(*)(int64_t, float))  rFun.pFun)(rA.a[0].f, rA.a[1].f); return; }
   if (sType.substr(2) == "fd")  { ((void(*)(int64_t, double)) rFun.pFun)(rA.a[0].f, rA.a[1].d); return; }

   if (sType.substr(2) == "dI")  { ((void(*)(int64_t, int64_t))rFun.pFun)(rA.a[0].d, rA.a[1].I); return; }
   if (sType.substr(2) == "df")  { ((void(*)(int64_t, float))  rFun.pFun)(rA.a[0].d, rA.a[1].f); return; }
   if (sType.substr(2) == "dd")  { ((void(*)(int64_t, double)) rFun.pFun)(rA.a[0].d, rA.a[1].d); return; }

   // 3 x 3 x 3 = 27
   if (sType.substr(2) == "III") { ((void(*)(int64_t, int64_t, int64_t))rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].I); return; }
   if (sType.substr(2) == "IIf") { ((void(*)(int64_t, int64_t, float))  rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].f); return; }
   if (sType.substr(2) == "IId") { ((void(*)(int64_t, int64_t, double)) rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].d); return; }

   if (sType.substr(2) == "IfI") { ((void(*)(int64_t, float, int64_t))rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].I); return; }
   if (sType.substr(2) == "Iff") { ((void(*)(int64_t, float, float))  rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].f); return; }
   if (sType.substr(2) == "Ifd") { ((void(*)(int64_t, float, double)) rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].d); return; }

   if (sType.substr(2) == "IdI") { ((void(*)(int64_t, double, int64_t))rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].I); return; }
   if (sType.substr(2) == "Idf") { ((void(*)(int64_t, double, float))  rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].f); return; }
   if (sType.substr(2) == "Idd") { ((void(*)(int64_t, double, double)) rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].d); return; }
   //
   if (sType.substr(2) == "fII") { ((void(*)(float, int64_t, int64_t))rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].I); return; }
   if (sType.substr(2) == "fIf") { ((void(*)(float, int64_t, float))  rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].f); return; }
   if (sType.substr(2) == "fId") { ((void(*)(float, int64_t, double)) rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].d); return; }

   if (sType.substr(2) == "ffI") { ((void(*)(float, float, int64_t))rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].I); return; }
   if (sType.substr(2) == "fff") { ((void(*)(float, float, float))  rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].f); return; }
   if (sType.substr(2) == "ffd") { ((void(*)(float, float, double)) rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].d); return; }

   if (sType.substr(2) == "fdI") { ((void(*)(float, double, int64_t))rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].I); return; }
   if (sType.substr(2) == "fdf") { ((void(*)(float, double, float))  rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].f); return; }
   if (sType.substr(2) == "fdd") { ((void(*)(float, double, double)) rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].d); return; }
   //
   if (sType.substr(2) == "dII") { ((void(*)(double, int64_t, int64_t))rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].I); return; }
   if (sType.substr(2) == "dIf") { ((void(*)(double, int64_t, float))  rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].f); return; }
   if (sType.substr(2) == "dId") { ((void(*)(double, int64_t, double)) rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].d); return; }

   if (sType.substr(2) == "dfI") { ((void(*)(double, float, int64_t))rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].I); return; }
   if (sType.substr(2) == "dff") { ((void(*)(double, float, float))  rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].f); return; }
   if (sType.substr(2) == "dfd") { ((void(*)(double, float, double)) rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].d); return; }

   if (sType.substr(2) == "ddI") { ((void(*)(double, double, int64_t))rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].I); return; }
   if (sType.substr(2) == "ddf") { ((void(*)(double, double, float))  rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].f); return; }
   if (sType.substr(2) == "ddd") { ((void(*)(double, double, double)) rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].d); return; }

   // TODO: here error!
}

// "I:idf"
template <typename T>
LOCAL T callAny(const TBaIpcRegFun &rFun, const TBaIpcFunArg &rA) {
   std::string sType = rFun.type;

   //
   if (rFun.type[1] != ':') {
      // TODO: error
      return 0;
   }

   // No parameters
   if (rFun.type[2] == 'v')          { return ((int64_t(*)()) rFun.pFun)(); }

   // 3
   if (sType.substr(2) == "I")   { return ((T(*)(int64_t)) rFun.pFun)(rA.a[0].I); }
   if (sType.substr(2) == "f")   { return ((T(*)(float))   rFun.pFun)(rA.a[0].f); }
   if (sType.substr(2) == "d")   { return ((T(*)(double))  rFun.pFun)(rA.a[0].d); }

   // 3 x 3 = 9
   if (sType.substr(2) == "II")  { return ((T(*)(int64_t, int64_t)) rFun.pFun)(rA.a[0].I, rA.a[1].I); }
   if (sType.substr(2) == "If")  { return ((T(*)(int64_t, float))   rFun.pFun)(rA.a[0].I, rA.a[1].f); }
   if (sType.substr(2) == "Id")  { return ((T(*)(int64_t, double))  rFun.pFun)(rA.a[0].I, rA.a[1].d); }
   if (sType.substr(2) == "fI")  { return ((T(*)(int64_t, int64_t)) rFun.pFun)(rA.a[0].f, rA.a[1].I); }
   if (sType.substr(2) == "ff")  { return ((T(*)(int64_t, float))   rFun.pFun)(rA.a[0].f, rA.a[1].f); }
   if (sType.substr(2) == "fd")  { return ((T(*)(int64_t, double))  rFun.pFun)(rA.a[0].f, rA.a[1].d); }
   if (sType.substr(2) == "dI")  { return ((T(*)(int64_t, int64_t)) rFun.pFun)(rA.a[0].d, rA.a[1].I); }
   if (sType.substr(2) == "df")  { return ((T(*)(int64_t, float))   rFun.pFun)(rA.a[0].d, rA.a[1].f); }
   if (sType.substr(2) == "dd")  { return ((T(*)(int64_t, double))  rFun.pFun)(rA.a[0].d, rA.a[1].d); }

   // 3 x 3 x 3 = 27
   if (sType.substr(2) == "III") { return ((T(*)(int64_t, int64_t, int64_t)) rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].I); }
   if (sType.substr(2) == "IIf") { return ((T(*)(int64_t, int64_t, float))   rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].f); }
   if (sType.substr(2) == "IId") { return ((T(*)(int64_t, int64_t, double))  rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].d); }
   if (sType.substr(2) == "IfI") { return ((T(*)(int64_t, float, int64_t))   rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].I); }
   if (sType.substr(2) == "Iff") { return ((T(*)(int64_t, float, float))     rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].f); }
   if (sType.substr(2) == "Ifd") { return ((T(*)(int64_t, float, double))    rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].d); }
   if (sType.substr(2) == "IdI") { return ((T(*)(int64_t, double, int64_t))  rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].I); }
   if (sType.substr(2) == "Idf") { return ((T(*)(int64_t, double, float))    rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].f); }
   if (sType.substr(2) == "Idd") { return ((T(*)(int64_t, double, double))   rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].d); }
   if (sType.substr(2) == "fII") { return ((T(*)(float, int64_t, int64_t))   rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].I); }
   if (sType.substr(2) == "fIf") { return ((T(*)(float, int64_t, float))     rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].f); }
   if (sType.substr(2) == "fId") { return ((T(*)(float, int64_t, double))    rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].d); }
   if (sType.substr(2) == "ffI") { return ((T(*)(float, float, int64_t))     rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].I); }
   if (sType.substr(2) == "fff") { return ((T(*)(float, float, float))       rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].f); }
   if (sType.substr(2) == "ffd") { return ((T(*)(float, float, double))      rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].d); }
   if (sType.substr(2) == "fdI") { return ((T(*)(float, double, int64_t))    rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].I); }
   if (sType.substr(2) == "fdf") { return ((T(*)(float, double, float))      rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].f); }
   if (sType.substr(2) == "fdd") { return ((T(*)(float, double, double))     rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].d); }
   if (sType.substr(2) == "dII") { return ((T(*)(double, int64_t, int64_t))  rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].I); }
   if (sType.substr(2) == "dIf") { return ((T(*)(double, int64_t, float))    rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].f); }
   if (sType.substr(2) == "dId") { return ((T(*)(double, int64_t, double))   rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].d); }
   if (sType.substr(2) == "dfI") { return ((T(*)(double, float, int64_t))    rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].I); }
   if (sType.substr(2) == "dff") { return ((T(*)(double, float, float))      rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].f); }
   if (sType.substr(2) == "dfd") { return ((T(*)(double, float, double))     rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].d); }
   if (sType.substr(2) == "ddI") { return ((T(*)(double, double, int64_t))   rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].I); }
   if (sType.substr(2) == "ddf") { return ((T(*)(double, double, float))     rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].f); }
   if (sType.substr(2) == "ddd") { return ((T(*)(double, double, double))    rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].d); }

   // TODO: here error!
   return 0;
}


