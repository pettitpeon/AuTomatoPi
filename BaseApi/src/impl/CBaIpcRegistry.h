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

   typedef R(*TFun)(A...);

   void SaveArgs(A... arg) {
      mArgs = std::make_tuple(arg...);
   };

   R operator()() {
//      return mFun(mArgs...);
      return callFunc(typename gens<sizeof...(A)>::type());
   };

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

   template<R, int ...S>
   R callFunc(seq<S...>)
   {
     return func(std::get<S>(mArgs) ...);
   }
   TFun mFun;
   std::tuple<A...> mArgs;
};


class CBaIpcRegistry {
public:
   static CBaIpcRegistry* Create();

   static bool Destroy(
         CBaIpcRegistry *pHadl
         );

   template <class R, class... A>
   bool Register(CBaIpcFunctor<R, A...> &ftor) {

      mFunReg["0"] = &ftor;

      return false;
   };

   template <class R, class... A>
   R Call(A... arg) {
      return mFunReg["0"](arg...);
   };

   CBaIpcRegistry();
   virtual ~CBaIpcRegistry() {};

private:

   std::map<std::string, void *> mFunReg;
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
