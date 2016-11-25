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
class CBaIpcRegistry {
public:
   static CBaIpcRegistry* Create();

   static bool Destroy(
         CBaIpcRegistry *pHadl
         );

   CBaIpcRegistry();
   virtual ~CBaIpcRegistry() {};

private:

   std::map<std::string, void *> mFunReg;
   std::map<std::string, TBaIpcRegVar> mVarReg;

};


#endif /* CBAIPCREGISTRY_H_ */
