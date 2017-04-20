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
#include "BaIpcRegistry.h"

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
///
typedef struct TBaIpcFunCall {
   const char* name;
   TBaIpcFunArg a;
} TBaIpcFunCall;


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

   static bool SClearFunRegistry();

   static bool SRegisterVar(std::string name, const TBaIpcRegVar &rVar);

   static bool SUnregisterVar(std::string name);

   static bool SClearVarRegistry();

   static bool SCallVar(std::string name, TBaIpcRegVarOut &rVar);

   // todo: necessary?
   static bool SCallVarInternal(std::string name, TBaIpcRegVar &rVar);

   static bool SSetVar(std::string name, const TBaIpcRegVar &rVar);

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

   virtual void ClearFunRegistry() { mFunReg.clear(); };

   virtual bool CallFun(std::string name, TBaIpcFunArg a, TBaIpcArg *pRet);

   virtual bool RegisterVar(std::string name, const TBaIpcRegVar &rVar) {
      if (varIsValid(rVar)) {
         return mVarReg.emplace(name, rVar).second;
      }

      return false;
   };

   virtual bool UnregisterVar(std::string name) {
      return mVarReg.erase(name) > 0;
   };

   virtual void ClearVarRegistry() { mVarReg.clear(); };

   virtual bool CallVar(std::string name, TBaIpcRegVarOut &rVar);

   bool CallVarInternal(std::string name, TBaIpcRegVar &rVar);

   virtual bool SetVar(std::string name, const TBaIpcRegVar &rVar);

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

