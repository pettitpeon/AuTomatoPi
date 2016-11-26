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
   R callFunc(seq<S...>)
   {
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
   bool Register(CBaIpcFunctor<R, A...> &ftor) {

      mFunReg["0"] = &ftor;

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

int main(int argc, char* argv[]) {

   CBaIpcRegistry reg;
   CBaIpcFunctor<double, int, double> ftor(1, 2.0);
   ftor.mFun = iTimesD;
   reg.Register(ftor);

   std::cout << reg.Call<double>(4, 5.0) << " hello IPC " << ftor(2, 3.0) << std::endl;


   return 0;
}



#endif
