/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaIpcRegistry.h
 *   Date     : 17.03.2017
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BAIPCREGISTRY_H_
#define BAIPCREGISTRY_H_

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <stddef.h>
#include <string>
#include "BaBool.h"
/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
/// C message handle
typedef void* TBaIpcRegistryHdl;

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
   char *type;
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
#ifdef __cplusplus
extern "C" {
#endif

/// @name Factory
//@{
/******************************************************************************/
/** Create factory for ...
 *  @return Handle if success, otherwise, null
 */
TBaIpcRegistryHdl BaIpcRegistryCreate();

/******************************************************************************/
/** Destroy and release resources
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryDestroy(
      TBaIpcRegistryHdl hdl ///< [in] handle to destroy
      );
//@}


#ifdef __cplusplus
} // extern c

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

class IBaIpcRegistry {
public:

   /***************************************************************************/
   /** ...
    *  @return ...
    */
   virtual bool RegisterFun(std::string name, TBaIpcRegFun fun) = 0;

   virtual bool RemoveFun(std::string name) = 0;

   virtual bool CallFun(std::string name, TBaIpcFunArg a, TBaIpcArg *pOut) = 0;

   virtual bool RegisterVar(std::string name, TBaIpcRegVar var) = 0;

   virtual bool RemoveVar(std::string name) = 0;

   // Typical object oriented destructor must be virtual!
   virtual ~IBaIpcRegistry() {};
};

/// @name C++ Factory
//@{
/******************************************************************************/
/** Create factory for ...
 *  @return Handle if success, otherwise, null
 */
extern "C" IBaIpcRegistry * IBaIpcRegistryCreate();

/******************************************************************************/
/** Destroy and release resources
 *  @return True if success, otherwise, false
 */
extern "C" TBaBoolRC IBaIpcRegistryDestroy(
      IBaIpcRegistry *pHdl ///< [in] handle to destroy
      );
//@}


#endif // __cplusplus
#endif // BAIPCREGISTRY_H_
