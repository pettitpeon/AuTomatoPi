/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CBaIpcRegistry.h
 *   Date     : Nov 24, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef CBAIPCREGISTRY_H_
#define CBAIPCREGISTRY_H_

#include <map>
#include <string>
#include <functional>
#include <tuple>
#include "BaIpcRegistry.h"

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
///
typedef struct TBaIpcFunCall {
   const char* name;
   TBaIpcFunArg a;
} TBaIpcFunCall;


double TestRegFun(uint32_t i, float f);

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

class CBaIpcRegistry : public IBaIpcRegistry {
public:
   static CBaIpcRegistry* Create();

   static bool Destroy(
         CBaIpcRegistry *pHdl
         );

   static bool SInitRegistry();

   static bool SExitRegistry();

   static bool SRegisterFun(std::string name, TBaIpcRegFun fun);

   static bool SUnregisterFun(std::string name);

   static bool SCallFun(std::string name, TBaIpcFunArg a, TBaIpcArg *pRet);

   static void SClearFunRegistry();

   virtual bool RegisterFun(std::string name, TBaIpcRegFun fun) {
      std::string sType = fun.type;

      if (!fun.pFun || sType.length() < 3 || fun.type[1] != ':' || sType.length() > BAIPCMAXARG + 2) {
         return false;
      }
      return mFunReg.emplace(name, fun).second;
   }

   virtual bool UnregisterFun(std::string name) {
      return mFunReg.erase(name) > 0;
   };

   virtual void ClearFunRegistry() {
      return mFunReg.clear();
   };

   virtual bool CallFun(std::string name, TBaIpcFunArg a, TBaIpcArg *pRet);

   virtual bool RegisterVar(std::string name, TBaIpcRegVar var) {
      if (varIsValid(var)) {
         return mVarReg.emplace(name, var).second;
      }

      return false;
   };

   virtual bool RemoveVar(std::string name) {
      return mVarReg.erase(name) > 0;
   };

   virtual bool CallVar(std::string name, TBaIpcRegVar *pVar);
   virtual bool SetVar(std::string name, TBaIpcRegVar *pVar);

private:
   CBaIpcRegistry() {};
   virtual ~CBaIpcRegistry() {};

   bool varIsValid(const TBaIpcRegVar &rVar) {
      return (rVar.pVar && rVar.sz > 0 && rVar.sz < BAIPCMAXVARSZ);
   }

   std::map<std::string, TBaIpcRegFun> mFunReg;
   std::map<std::string, TBaIpcRegVar> mVarReg;
};


#endif /* CBAIPCREGISTRY_H_ */

#if 0

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
typedef struct TBaIpcRegVar {
   void *pVar;
   int type;
   size_t sz;
} TBaIpcRegVar;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

template <class R, class... A>
class CBaIpcFunctor {
public:

   static_assert(
         std::is_fundamental<R>::value ||
         std::is_same<R, const char *>::value
         , "Bad return type");

   typedef R(*TFun)(A...);

   void SaveArgs(A... arg) {
      mArgs = std::make_tuple(arg...);
   };

   R operator()() {
      return callFunc(typename gens<sizeof...(A)>::type());
   };

   R operator()(A... arg) {
      SaveArgs(arg...);
      return callFunc(typename gens<sizeof...(A)>::type());
   };

   CBaIpcFunctor(TFun fun) : mFun(fun) { };
   CBaIpcFunctor() : mFun(0) { };

   CBaIpcFunctor(A... arg) : mFun(0), mArgs(std::make_tuple(arg...)) { };
private:

   // Magic
   template<int...>
   struct seq {};

   template<int N, int... S>
   struct gens : gens<N-1, N-1, S...> {};

   template<int... S>
   struct gens<0, S...> {
      typedef seq<S...> type;
   };

   template<int ...S>
   R callFunc(seq<S...>) {
//      std::string ret = static_cast<std::string> (mFun(std::get<S>(mArgs) ...));
//      R ret2;
      return mFun(std::get<S>(mArgs) ...);

   }

public:
   TFun mFun;

private:
   std::tuple<A...> mArgs;
};


class CBaIpcRegistry {
public:


   static CBaIpcRegistry* Create();

   static bool Destroy(
         CBaIpcRegistry *pHadl
         );

   template <class R, class... A>
   bool Register(R(*fun)(A...)) {
      typename CBaIpcFunctor<R, A...>::TFun fun2;
      fun2 = fun;

      CBaIpcFunctor<R, A...> *pFtor = new CBaIpcFunctor<R, A...>(fun);
      mFunReg["0"] = pFtor;

      return false;
   };

   template <class R, class... A>
   R Call(A... arg) {
      CBaIpcFunctor<R, A...> &rFtor = *(CBaIpcFunctor<R, A...> *)mFunReg["0"];
      rFtor.SaveArgs(arg...);
      return rFtor();
   };

//   CBaIpcRegistry();
   virtual ~CBaIpcRegistry() {};

private:

   std::map<std::string, void *> mFunReg;
   std::map<std::string, TBaIpcRegVar> mVarReg;

};

double iTimesD(int i, double d) {
   return i*d;
}

const char* strFun(const char* s) {
   return s;
}

int main(int argc, char* argv[]) {

   CBaIpcRegistry reg;
   CBaIpcFunctor<double, int, double> ftor(iTimesD);
   reg.Register(strFun);

// std::cout << reg.Call<double>(4, 5.0) << " hello IPC " << ftor(2, 3.0) << std::endl;

   std::cout << reg.Call<const char *>("Hello registry") << " hello IPC" << std::endl;

   return 0;
}


#endif
