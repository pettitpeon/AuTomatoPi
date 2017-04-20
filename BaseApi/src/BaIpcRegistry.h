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
 *  API for functions and variables registries. The API gives the freedom to
 *  create registries at the user's discretion, or use the embedded "local"
 *  registry. The IPC API uses the local registry to call functions and
 *  variables remotely.
 *
 *  // TODO: The variables registry is not concurrency-safe
 */
/*------------------------------------------------------------------------------
 */
#ifndef BAIPCREGISTRY_H_
#define BAIPCREGISTRY_H_

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <stddef.h>
#ifdef __cplusplus
# include <string>
#endif
#include "BaBool.h"


/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
/// MAximum number of arguments in registry function
#define BAIPCMAXARG 4
#define BAIPCMAXVARSZ 1020

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
/// C message handle
typedef void* TBaIpcRegistryHdl;

/// Variable descriptor of the variables registry
typedef struct TBaIpcRegVar {
   void *pVar; ///< Pointer to the variable to be registered
   size_t sz; ///< Size in bytes of the variable to be registered. Must be <= BAIPCMAXVARSZ
   TBaBool wr; ///< Flag to determine if the variable can be overwritten
} TBaIpcRegVar;

/** Function descriptor of the functions registry.
 *  The @c type string is defined as one character, a semicolon and a string,
 *  eg.: i:IIf. The first character is the return type of the function. The
 *  string of characters after the semicolon are the parameter types in order.
 *  The valid characters and types are
 *   - v: void
 *   - i: integer types of 4 bytes or less. It includes unsigned types and pointers
 *   - I: 64 bit integer types including unsigned
 *   - f: float
 *   - d: double
 *  If there are no parameters, only a single 'v' is allowed after the
 *  semicolon, e.g., f:v
 *
 */
typedef struct TBaIpcRegFun {
   void *pFun; ///< Pointer to the function casted to void
   const char *type; ///< Function type definition
} TBaIpcRegFun;

/// Variable descriptor buffer for retrieving values of the variables registry
/// without getting access to the underlying memory.
typedef struct TBaIpcRegVarOut {
   union {
      char data[BAIPCMAXVARSZ];
      void    *p;
      float    f;
      double   d;
      int32_t  i; ///< This includes all the smaller types
      uint32_t u;
      int64_t  I;
      uint64_t U;
   } dat;
   size_t sz; ///< Size in bytes of the variable retrieved variable
} TBaIpcRegVarOut;

/// Argument type union. It is used to cast return values and single arguments.
typedef union TBaIpcArg {
      void    *p;
      float    f;
      double   d;
      int32_t  i; ///< This includes all the smaller types
      uint32_t u;
      int64_t  I;
      uint64_t U;
} TBaIpcArg;

/// Structure to hold the arguments of a function to be called
typedef struct TBaIpcFunArg {
   TBaIpcArg a[BAIPCMAXARG]; ///< Array of arguments
} TBaIpcFunArg;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/// @name Local Functions registry
//@{
/******************************************************************************/
/** Initialize the local registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryLocalInit();

/******************************************************************************/
/** Exit the local registry and release resources
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryLocalExit();

/******************************************************************************/
/** Register a function in the local registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryLocalRegisterFun(
      const char* name, ///< [in] Function name
      TBaIpcRegFun fun ///< [in] Function to register
      );

/******************************************************************************/
/** Remove a function from the local registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryLocalUnregisterFun(
      const char* name ///< [in] Function name
      );

/******************************************************************************/
/** Call a function from the local registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryLocalCallFun(
      const char* name, ///< [in] Function name
      TBaIpcFunArg a, ///< [in] Function arguments
      TBaIpcArg *pRet ///< [out] Function return value
      );

/******************************************************************************/
/** Clear the local registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryLocalClearFunReg();

/******************************************************************************/
/** Register a variable in the local registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryLocalRegisterVar(
      const char* name, ///< [in] Variable name
      const TBaIpcRegVar *pVar ///< [in] Variable descriptor
      );

/******************************************************************************/
/** Remove a variable from the local registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryLocalUnregisterVar(
      const char* name ///< [in] Variable name
      );

/******************************************************************************/
/** Clear the local registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryLocalClearVarReg();

/******************************************************************************/
/** Gets the value of a variable in the registry by copying the value to the
 *  @c *pVar buffer.
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryLocalCallVar(
      const char* name, ///< [in] Variable name
      TBaIpcRegVarOut *pVar  ///< [out] Variable descriptor
      );

/******************************************************************************/
/** Sets the values of a variable from the registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryLocalSetVar(
      const char* name, ///< [in] Variable name
      const TBaIpcRegVar *pVar ///< [in] Variable descriptor
      );
//@}


/// @name Registry Factory
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

/// @name Functions registry
//@{
/******************************************************************************/
/** Register a function in the registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryRegisterFun(
      TBaIpcRegistryHdl hdl, ///< [in] Handle
      const char* name, ///< [in] Function name
      TBaIpcRegFun fun ///< [in] Function to register
      );

/******************************************************************************/
/** Remove a function from the registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryUnregisterFun(
      TBaIpcRegistryHdl hdl, ///< [in] Handle
      const char* name ///< [in] Function name
      );

/******************************************************************************/
/** Clear the functions registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryClearFunReg(
      TBaIpcRegistryHdl hdl ///< [in] Handle
      );

/******************************************************************************/
/** Call a function from the registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryCallFun(
      TBaIpcRegistryHdl hdl, ///< [in] Handle
      const char* name, ///< [in] Function name
      TBaIpcFunArg a, ///< [in] Function arguments
      TBaIpcArg *pRet ///< [out] Function return value
      );
//@}

/// @name Variables registry
//@{
/******************************************************************************/
/** Register a variable in the registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryRegisterVar(
      TBaIpcRegistryHdl hdl, ///< [in] Handle
      const char* name, ///< [in] Function name
      const TBaIpcRegVar *pVar ///< [in] Variable to register
      );

/******************************************************************************/
/** Register a variable in the registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryUnregisterVar(
      TBaIpcRegistryHdl hdl, ///< [in] Handle
      const char* name ///< [in] Function name
      );

/******************************************************************************/
/** Clear the variables registry
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryClearVarReg(
      TBaIpcRegistryHdl hdl ///< [in] Handle
      );

/******************************************************************************/
/** Gets the value of a variable in the registry by copying the value to the
 *  @c *pVar buffer.
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistryCallVar(
      TBaIpcRegistryHdl hdl, ///< [in] Handle
      const char* name, ///< [in] Variable name
      TBaIpcRegVarOut *pVar  ///< [out] Variable descriptor
      );

/******************************************************************************/
/** Set a variable from the registry to a new value if allowed
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcRegistrySetVar(
      TBaIpcRegistryHdl hdl, ///< [in] Handle
      const char* name, ///< [in] Function name
      const TBaIpcRegVar *pVar ///< [in] Variable descriptor
      );


//@}
#ifdef __cplusplus
} // extern c

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

/******************************************************************************/
/** Interface to use a registry object. This interface is used under the hood
 *  for the "local" registry. If you are interested in using the local registry
 *  for the IPC server, please use the @c BaIpcRegistryLocal C-interface.
 */
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
   /** Call a function from the functions registry
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
   /***************************************************************************/
   /** Register a variable to the registry
    *  @return true if success
    */
   virtual bool RegisterVar(
         std::string name, ///< [in] Variable name
         const TBaIpcRegVar &rVar ///< [in] Variable descriptor
         ) = 0;

   /***************************************************************************/
   /** Unregister a variable from the registry
    *  @return true if success
    */
   virtual bool UnregisterVar(
         std::string name ///< [in] Variable name
         ) = 0;

   /***************************************************************************/
   /** Unregister all variables from the registry
    *  @return true if success
    */
   virtual void ClearVarRegistry() = 0;

   /***************************************************************************/
   /** Retrieve the value of a registry variable
    *  @return true if success
    */
   virtual bool CallVar(
         std::string name, ///< [in] Variable name
         TBaIpcRegVarOut &rVar ///< [out] Variable descriptor
         ) = 0;

   /***************************************************************************/
   /** Set a variable from the registry to a new value if allowed
    *  @return true if success
    */
   virtual bool SetVar(
         std::string name,
         const TBaIpcRegVar &rVar
         ) = 0;
   //@}

   // Typical object oriented destructor must be virtual!
   virtual ~IBaIpcRegistry() {};
};

/// @name C++ Factory
//@{
/******************************************************************************/
/** Create factory for a functions and variables registry
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

