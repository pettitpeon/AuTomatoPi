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

typedef enum EBaIpcVarType {
   eBaIpcVarTypeUnkn = 0,
   eBaIpcVarTypeU8,
   eBaIpcVarTypeU16,
   eBaIpcVarTypeU32,
   eBaIpcVarTypeU64,
   eBaIpcVarTypeI8,
   eBaIpcVarTypeI16,
   eBaIpcVarTypeI32,
   eBaIpcVarTypeI64,
   eBaIpcVarTypeFlt,
   eBaIpcVarTypeDou,
   eBaIpcVarTypeStr,
   eBaIpcVarTypePtr,
   eBaIpcVarTypeMax = eBaIpcVarTypePtr
} EBaIpcVarType;

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
typedef struct TBaIpcRegVar {
   void *pVar;
   int type;
   size_t sz;
} TBaIpcRegVar;

// s, i, f, d, I
// The primitive type int32 is enough to represent everything from char to
// int32 signed or unsigned because the relying memory is the same
// 64bit quantities are handled extra
// Float and double must be handled extra as well because the relying memory
// is different
// return type, parameters: "v,idf"
typedef struct TBaIpcRegFun {
   void *pFun;
   std::string type;
} TBaIpcRegFun;


///
typedef struct TBaIpcArg {
   union {
      void *p;
      float f;
      double d;
      int32_t i;
      int64_t I;
   };
} TBaIpcArg;

///
typedef struct TBaIpcFunArg {
   TBaIpcArg a[3];
} TBaIpcFunArg;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/


//template <class R, class... A>
//class CBaIpcFunctor {
//public:
//
//   typedef R(*TFun)(A...);
//
//   void SaveArgs(A... arg) {
//      mArgs = std::make_tuple(arg...);
//   };
//
//   R operator()() {
////      return mFun(mArgs...);
//      return callFunc(typename gens<sizeof...(A)>::type());
//   };
//
//private:
//
//   // Magic
//   template<int...>
//   struct seq {};
//
//   template<int N, int... S>
//   struct gens : gens<N-1, N-1, S...> {};
//
//   template<int... S>
//   struct gens<0, S...> {
//      typedef seq<S...> type;
//   };
//
//   template<R, int ...S>
//   R callFunc(seq<S...>)
//   {
//     return func(std::get<S>(mArgs) ...);
//   }
//   TFun mFun;
//   std::tuple<A...> mArgs;
//};


class CBaIpcRegistry {
public:
   static CBaIpcRegistry* Create();

   static bool Destroy(
         CBaIpcRegistry *pHadl
         );


   virtual bool RegisterFun(std::string name, TBaIpcRegFun fun);

   virtual bool RemoveFun(std::string name) {
      return mFunReg.erase(name) > 0;
   };

   virtual bool CallFun(std::string name, TBaIpcFunArg a);

   virtual bool RegisterVar(std::string name, TBaIpcRegVar var) {
      if (varIsValid(var)) {
         return mVarReg.emplace(name, var).second;
      }

      return false;
   };

   virtual bool RemoveVar(std::string name) {
      return mVarReg.erase(name) > 0;
   };

   CBaIpcRegistry();
   virtual ~CBaIpcRegistry() {};

private:
   bool varIsValid(const TBaIpcRegVar &rVar) {
      return rVar.pVar && rVar.type > eBaIpcVarTypeUnkn && rVar.type <= eBaIpcVarTypeMax &&
            // if str || ptr, force sz != 0
            ((rVar.type != eBaIpcVarTypeStr && rVar.type != eBaIpcVarTypePtr) || !rVar.sz);
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
