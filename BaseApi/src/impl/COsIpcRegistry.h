/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : COsIpcRegistry.h
 *   Date     : Nov 24, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef COSIPCREGISTRY_H_
#define COSIPCREGISTRY_H_

#include <OsIpcRegistry.h>
#include <map>
#include <string>

// todo: Delete
#include "BaLogMacros.h"
#define TAG "CIPCReg"

#define OSIPC_FUNNAMESZ (OSIPC_MSGDATASZ - sizeof(TOsIpcFunArg))

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
//
typedef struct TOsIpcFunCallData {
   char name[OSIPC_FUNNAMESZ];
   TOsIpcFunArg a;
} TOsIpcFunCallData;


/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

class COsIpcRegistry : public IOsIpcRegistry {
public:
   static COsIpcRegistry* Create();

   static bool Destroy(
         COsIpcRegistry *pHdl
         );

   static bool SInitRegistry();

   static bool SExitRegistry();

   static bool SRegisterFun(std::string name, TOsIpcRegFun fun);

   static bool SUnregisterFun(std::string name);

   static bool SCallFun(std::string name, TOsIpcFunArg a, TOsIpcArg *pRet);

   static bool SClearFunRegistry();

   static bool SRegisterVar(std::string name, const TOsIpcRegVar &rVar);

   static bool SUnregisterVar(std::string name);

   static bool SClearVarRegistry();

   static bool SCallVar(std::string name, TOsIpcRegVarOut &rVar);

   // todo: necessary?
   static bool SCallVarInternal(std::string name, TOsIpcRegVar &rVar);

   static bool SSetVar(std::string name, const TOsIpcRegVar &rVar);

   virtual bool RegisterFun(std::string name, TOsIpcRegFun fun) {
      std::string sType = fun.type;

      if (!fun.pFun || sType.length() < 3 || fun.type[1] != ':' || sType.length() > OSIPC_MAXARG + 2) {
         // todo: Delete
         TRACE_("%p, %i, %i, %i", fun.pFun, sType.length() < 3, fun.type[1] != ':', sType.length() > OSIPC_MAXARG + 2);
         return false;
      }
      return mFunReg.emplace(name, fun).second;
   }

   virtual bool UnregisterFun(std::string name) {
      return mFunReg.erase(name) > 0;
   };

   virtual void ClearFunRegistry() { mFunReg.clear(); };

   virtual bool CallFun(std::string name, TOsIpcFunArg a, TOsIpcArg *pRet);

   virtual bool RegisterVar(std::string name, const TOsIpcRegVar &rVar) {
      if (varIsValid(rVar)) {
         return mVarReg.emplace(name, rVar).second;
      }

      return false;
   };

   virtual bool UnregisterVar(std::string name) {
      return mVarReg.erase(name) > 0;
   };

   virtual void ClearVarRegistry() { mVarReg.clear(); };

   virtual bool CallVar(std::string name, TOsIpcRegVarOut &rVar);

   bool CallVarInternal(std::string name, TOsIpcRegVar &rVar);

   virtual bool SetVar(std::string name, const TOsIpcRegVar &rVar);

private:
   COsIpcRegistry() {};
   virtual ~COsIpcRegistry() {};

   bool varIsValid(const TOsIpcRegVar &rVar) {
      return (rVar.pVar && rVar.sz > 0 && rVar.sz < OSIPC_MAXVARSZ);
   }

   std::map<std::string, TOsIpcRegFun> mFunReg;
   std::map<std::string, TOsIpcRegVar> mVarReg;
};


#endif /* COSIPCREGISTRY_H_ */

