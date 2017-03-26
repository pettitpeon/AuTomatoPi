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
#include "BaBool.h"
#ifdef __cplusplus
# include <string>
#endif


/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define BAIPCMAXARG 4

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
/// C message handle
typedef void* TBaIpcRegistryHdl;

///
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
   const char *type;
} TBaIpcRegFun;

///
typedef union TBaIpcArg {
      void    *p;
      float    f;
      double   d;
      int32_t  i;
      uint32_t u;
      int64_t  I;
      uint64_t U;
} TBaIpcArg;

///
typedef struct TBaIpcFunArg {
   TBaIpcArg a[BAIPCMAXARG];
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

   /// @name Functions registry
   //@{
   /***************************************************************************/
   /** Register a function to the IPC registry
    *  @return true if success
    */
   virtual bool RegisterFun(
         std::string name, ///< [in] Function name
         TBaIpcRegFun fun ///< [in] Function descriptor
         ) = 0;

   /***************************************************************************/
   /** Unregister a function from the IPC registry
    *  @return true if success
    */
   virtual bool UnregisterFun(
         std::string name ///< [in] Function name
         ) = 0;

   /***************************************************************************/
   /** Unregister all functions from the registry
    *  @return true if success
    */
   virtual void ClearFunRegistry() = 0;

   /***************************************************************************/
   /** Register a function to the IPC registry
    *  @return true if success
    */
   virtual bool CallFun(
         std::string name, ///< [in] Function name
         TBaIpcFunArg a, ///< [in] Function arguments
         TBaIpcArg *pRet ///< [out] Function return value
         ) = 0;
   //@}

   /// @name Variables registry
   //@{
   virtual bool RegisterVar(std::string name, TBaIpcRegVar var) = 0;

   virtual bool RemoveVar(std::string name) = 0;
   //@}

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
