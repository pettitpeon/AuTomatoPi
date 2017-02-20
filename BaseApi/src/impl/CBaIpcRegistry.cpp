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
LOCAL void callInt(const TBaIpcRegFun &rFun, const TBaIpcFunArg &rA);


/*------------------------------------------------------------------------------
    Local variables
 -----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/
//
CBaIpcRegistry* CBaIpcRegistry::Create() {


   return 0;
}

//
bool CBaIpcRegistry::Destroy(CBaIpcRegistry* pHdl) {

   return true;
}

// types: v, i, I, f, d, s
bool CBaIpcRegistry::RegisterFun(std::string name, TBaIpcRegFun fun) {
   if (!fun.pFun || fun.type.length() < 3 || fun.type[1] != ':') {
      return false;
   }

   // Todo: Parse the correctness of the type

   return mFunReg.emplace(name, fun).second;
}

// "v,idf"
LOCAL void callVoid(const TBaIpcRegFun &rFun, const TBaIpcFunArg &rA) {
   int i = 0;

   //
   if (rFun.type[1] != ',') {
      // TODO: error
      return;
   }

   // No parameters
   if (rFun.type[2] == 'v')          { ((void(*)()) rFun.pFun)(); return; }

   // 3
   if (rFun.type.substr(2) == "I")   { ((void(*)(int64_t))rFun.pFun)(rA.a[0].I); return; }
   if (rFun.type.substr(2) == "f")   { ((void(*)(float))  rFun.pFun)(rA.a[0].f); return; }
   if (rFun.type.substr(2) == "d")   { ((void(*)(double)) rFun.pFun)(rA.a[0].d); return; }

   // 3 x 3 = 9
   if (rFun.type.substr(2) == "II")  { ((void(*)(int64_t, int64_t))rFun.pFun)(rA.a[0].I, rA.a[1].I); return; }
   if (rFun.type.substr(2) == "If")  { ((void(*)(int64_t, float))  rFun.pFun)(rA.a[0].I, rA.a[1].f); return; }
   if (rFun.type.substr(2) == "Id")  { ((void(*)(int64_t, double)) rFun.pFun)(rA.a[0].I, rA.a[1].d); return; }

   if (rFun.type.substr(2) == "fI")  { ((void(*)(int64_t, int64_t))rFun.pFun)(rA.a[0].f, rA.a[1].I); return; }
   if (rFun.type.substr(2) == "ff")  { ((void(*)(int64_t, float))  rFun.pFun)(rA.a[0].f, rA.a[1].f); return; }
   if (rFun.type.substr(2) == "fd")  { ((void(*)(int64_t, double)) rFun.pFun)(rA.a[0].f, rA.a[1].d); return; }

   if (rFun.type.substr(2) == "dI")  { ((void(*)(int64_t, int64_t))rFun.pFun)(rA.a[0].d, rA.a[1].I); return; }
   if (rFun.type.substr(2) == "df")  { ((void(*)(int64_t, float))  rFun.pFun)(rA.a[0].d, rA.a[1].f); return; }
   if (rFun.type.substr(2) == "dd")  { ((void(*)(int64_t, double)) rFun.pFun)(rA.a[0].d, rA.a[1].d); return; }

   // 3 x 3 x 3 = 27
   if (rFun.type.substr(2) == "III") { ((void(*)(int64_t, int64_t, int64_t))rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].I); return; }
   if (rFun.type.substr(2) == "IIf") { ((void(*)(int64_t, int64_t, float))  rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].f); return; }
   if (rFun.type.substr(2) == "IId") { ((void(*)(int64_t, int64_t, double)) rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].d); return; }

   if (rFun.type.substr(2) == "IfI") { ((void(*)(int64_t, float, int64_t))rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].I); return; }
   if (rFun.type.substr(2) == "Iff") { ((void(*)(int64_t, float, float))  rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].f); return; }
   if (rFun.type.substr(2) == "Ifd") { ((void(*)(int64_t, float, double)) rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].d); return; }

   if (rFun.type.substr(2) == "IdI") { ((void(*)(int64_t, double, int64_t))rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].I); return; }
   if (rFun.type.substr(2) == "Idf") { ((void(*)(int64_t, double, float))  rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].f); return; }
   if (rFun.type.substr(2) == "Idd") { ((void(*)(int64_t, double, double)) rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].d); return; }
   //
   if (rFun.type.substr(2) == "fII") { ((void(*)(float, int64_t, int64_t))rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].I); return; }
   if (rFun.type.substr(2) == "fIf") { ((void(*)(float, int64_t, float))  rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].f); return; }
   if (rFun.type.substr(2) == "fId") { ((void(*)(float, int64_t, double)) rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].d); return; }

   if (rFun.type.substr(2) == "ffI") { ((void(*)(float, float, int64_t))rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].I); return; }
   if (rFun.type.substr(2) == "fff") { ((void(*)(float, float, float))  rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].f); return; }
   if (rFun.type.substr(2) == "ffd") { ((void(*)(float, float, double)) rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].d); return; }

   if (rFun.type.substr(2) == "fdI") { ((void(*)(float, double, int64_t))rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].I); return; }
   if (rFun.type.substr(2) == "fdf") { ((void(*)(float, double, float))  rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].f); return; }
   if (rFun.type.substr(2) == "fdd") { ((void(*)(float, double, double)) rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].d); return; }
   //
   if (rFun.type.substr(2) == "dII") { ((void(*)(double, int64_t, int64_t))rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].I); return; }
   if (rFun.type.substr(2) == "dIf") { ((void(*)(double, int64_t, float))  rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].f); return; }
   if (rFun.type.substr(2) == "dId") { ((void(*)(double, int64_t, double)) rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].d); return; }

   if (rFun.type.substr(2) == "dfI") { ((void(*)(double, float, int64_t))rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].I); return; }
   if (rFun.type.substr(2) == "dff") { ((void(*)(double, float, float))  rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].f); return; }
   if (rFun.type.substr(2) == "dfd") { ((void(*)(double, float, double)) rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].d); return; }

   if (rFun.type.substr(2) == "ddI") { ((void(*)(double, double, int64_t))rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].I); return; }
   if (rFun.type.substr(2) == "ddf") { ((void(*)(double, double, float))  rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].f); return; }
   if (rFun.type.substr(2) == "ddd") { ((void(*)(double, double, double)) rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].d); return; }

   // TODO: here error!
}

// "v,idf"
LOCAL int64_t callInt(const TBaIpcRegFun &rFun, const TBaIpcFunArg &rA) {
   int i = 0;

   //
   if (rFun.type[1] != ',') {
      // TODO: error
      return 0;
   }

   // No parameters
   if (rFun.type[2] == 'v')          { return ((int64_t(*)()) rFun.pFun)(); }

   // 3
   if (rFun.type.substr(2) == "I")   { return ((int64_t(*)(int64_t)) rFun.pFun)(rA.a[0].I); }
   if (rFun.type.substr(2) == "f")   { return ((int64_t(*)(float))   rFun.pFun)(rA.a[0].f); }
   if (rFun.type.substr(2) == "d")   { return ((int64_t(*)(double))  rFun.pFun)(rA.a[0].d); }

   // 3 x 3 = 9
   if (rFun.type.substr(2) == "II")  { return ((int64_t(*)(int64_t, int64_t)) rFun.pFun)(rA.a[0].I, rA.a[1].I); }
   if (rFun.type.substr(2) == "If")  { return ((int64_t(*)(int64_t, float))   rFun.pFun)(rA.a[0].I, rA.a[1].f); }
   if (rFun.type.substr(2) == "Id")  { return ((int64_t(*)(int64_t, double))  rFun.pFun)(rA.a[0].I, rA.a[1].d); }
   if (rFun.type.substr(2) == "fI")  { return ((int64_t(*)(int64_t, int64_t)) rFun.pFun)(rA.a[0].f, rA.a[1].I); }
   if (rFun.type.substr(2) == "ff")  { return ((int64_t(*)(int64_t, float))   rFun.pFun)(rA.a[0].f, rA.a[1].f); }
   if (rFun.type.substr(2) == "fd")  { return ((int64_t(*)(int64_t, double))  rFun.pFun)(rA.a[0].f, rA.a[1].d); }
   if (rFun.type.substr(2) == "dI")  { return ((int64_t(*)(int64_t, int64_t)) rFun.pFun)(rA.a[0].d, rA.a[1].I); }
   if (rFun.type.substr(2) == "df")  { return ((int64_t(*)(int64_t, float))   rFun.pFun)(rA.a[0].d, rA.a[1].f); }
   if (rFun.type.substr(2) == "dd")  { return ((int64_t(*)(int64_t, double))  rFun.pFun)(rA.a[0].d, rA.a[1].d); }

   // 3 x 3 x 3 = 27
   if (rFun.type.substr(2) == "III") { return ((int64_t(*)(int64_t, int64_t, int64_t)) rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].I); }
   if (rFun.type.substr(2) == "IIf") { return ((int64_t(*)(int64_t, int64_t, float))   rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].f); }
   if (rFun.type.substr(2) == "IId") { return ((int64_t(*)(int64_t, int64_t, double))  rFun.pFun)(rA.a[0].I, rA.a[1].I, rA.a[2].d); }
   if (rFun.type.substr(2) == "IfI") { return ((int64_t(*)(int64_t, float, int64_t))   rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].I); }
   if (rFun.type.substr(2) == "Iff") { return ((int64_t(*)(int64_t, float, float))     rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].f); }
   if (rFun.type.substr(2) == "Ifd") { return ((int64_t(*)(int64_t, float, double))    rFun.pFun)(rA.a[0].I, rA.a[1].f, rA.a[2].d); }
   if (rFun.type.substr(2) == "IdI") { return ((int64_t(*)(int64_t, double, int64_t))  rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].I); }
   if (rFun.type.substr(2) == "Idf") { return ((int64_t(*)(int64_t, double, float))    rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].f); }
   if (rFun.type.substr(2) == "Idd") { return ((int64_t(*)(int64_t, double, double))   rFun.pFun)(rA.a[0].I, rA.a[1].d, rA.a[2].d); }
   if (rFun.type.substr(2) == "fII") { return ((int64_t(*)(float, int64_t, int64_t))   rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].I); }
   if (rFun.type.substr(2) == "fIf") { return ((int64_t(*)(float, int64_t, float))     rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].f); }
   if (rFun.type.substr(2) == "fId") { return ((int64_t(*)(float, int64_t, double))    rFun.pFun)(rA.a[0].f, rA.a[1].I, rA.a[2].d); }
   if (rFun.type.substr(2) == "ffI") { return ((int64_t(*)(float, float, int64_t))     rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].I); }
   if (rFun.type.substr(2) == "fff") { return ((int64_t(*)(float, float, float))       rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].f); }
   if (rFun.type.substr(2) == "ffd") { return ((int64_t(*)(float, float, double))      rFun.pFun)(rA.a[0].f, rA.a[1].f, rA.a[2].d); }
   if (rFun.type.substr(2) == "fdI") { return ((int64_t(*)(float, double, int64_t))    rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].I); }
   if (rFun.type.substr(2) == "fdf") { return ((int64_t(*)(float, double, float))      rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].f); }
   if (rFun.type.substr(2) == "fdd") { return ((int64_t(*)(float, double, double))     rFun.pFun)(rA.a[0].f, rA.a[1].d, rA.a[2].d); }
   if (rFun.type.substr(2) == "dII") { return ((int64_t(*)(double, int64_t, int64_t))  rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].I); }
   if (rFun.type.substr(2) == "dIf") { return ((int64_t(*)(double, int64_t, float))    rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].f); }
   if (rFun.type.substr(2) == "dId") { return ((int64_t(*)(double, int64_t, double))   rFun.pFun)(rA.a[0].d, rA.a[1].I, rA.a[2].d); }
   if (rFun.type.substr(2) == "dfI") { return ((int64_t(*)(double, float, int64_t))    rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].I); }
   if (rFun.type.substr(2) == "dff") { return ((int64_t(*)(double, float, float))      rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].f); }
   if (rFun.type.substr(2) == "dfd") { return ((int64_t(*)(double, float, double))     rFun.pFun)(rA.a[0].d, rA.a[1].f, rA.a[2].d); }
   if (rFun.type.substr(2) == "ddI") { return ((int64_t(*)(double, double, int64_t))   rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].I); }
   if (rFun.type.substr(2) == "ddf") { return ((int64_t(*)(double, double, float))     rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].f); }
   if (rFun.type.substr(2) == "ddd") { return ((int64_t(*)(double, double, double))    rFun.pFun)(rA.a[0].d, rA.a[1].d, rA.a[2].d); }

   // TODO: here error!
   return 0;
}

//
bool CBaIpcRegistry::CallFun(std::string name, TBaIpcFunArg a) {
   auto it = mFunReg.find(name);
   if (it == mFunReg.end()) {
      return false;
   }

   TBaIpcRegFun fun = it->second;

   // todo unpack parameters??
   // prepare return types
   // there are 2 basic types 32 bit for everything, or 64 bit for double and long int
   // are floats really 32 bit?? or is it best to upcast it?
   // parameters can be simplified to i,f,d

   switch (fun.type[0]) {
      case 'v':
         break;
      case 'i':
         break;
      case 'I':
         break;
      case 'f':
         break;
      case 'd':
         break;
      default : return false;
   }
};


