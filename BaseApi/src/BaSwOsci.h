/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaSwOsci.h
 *   Date     : Aug 24, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  Software oscilloscope. Logs samples of registered variables to a file
 */
/*------------------------------------------------------------------------------
 */
#ifndef BASWOSCI_H_
#define BASWOSCI_H_

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include "BaBool.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
/// C message handle
typedef void* TBaSwOsciHdl;

/// Data types of the variables logger
typedef enum EBaSwOsciType {
   eBaSwOsci_undef = -1, ///< -1
   eBaSwOsci_int8 = 0, ///< 0
   eBaSwOsci_uint8,  ///< 1
   eBaSwOsci_int16,  ///< 2
   eBaSwOsci_uint16, ///< 3
   eBaSwOsci_int32,  ///< 5
   eBaSwOsci_uint32, ///< 6
   eBaSwOsci_int64,  ///< 7
   eBaSwOsci_uint64, ///< 8
   eBaSwOsci_float,  ///< 9
   eBaSwOsci_double,  ///< 10
   eBaSwOsci_max = eBaSwOsci_double///< 10
} EBaSwOsciType;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/// @name Factory
//@{
/******************************************************************************/
/** Create factory
 *  @return Handle if success, otherwise, null
 */
TBaSwOsciHdl BaSwOsciCreate(
      const char *name, ///< [in] Name
      const char *path, ///< [in] Directory where to save the file
      TBaBool toCnsole ///< [in] Flag to print to console too
      );

/******************************************************************************/
/** Destroy and release resources. Should not take more than 100ms to complete.
 *  returns with error after 2s
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaSwOsciDestroy(
      TBaSwOsciHdl hdl ///< [in] Handle to destroy
      );

/******************************************************************************/
/** Sends signal to destroy and release resources asynchronously. Use this if
 *  the task is time critical such as a real-time application.
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaSwOsciDestroyAsync(
      TBaSwOsciHdl hdl ///< [in] Handle to destroy
      );
//@}

/// @name Interface
//@{
/******************************************************************************/
/** Registers a variable to the logger so it can be sampled. After the first
 *  @c Sample() is called, registering is no longer possible
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaSwOsciRegister(
      TBaSwOsciHdl hdl, ///< [in] Handle
      void* pVar, ///< [in] Pointer to the variable
      EBaSwOsciType type, ///< [in] Type of the variable pointer
      const char *name, /**< [in] Variable name. Any commas in the name will
      be replaced*/
      const char *desc ///< [in] Variable description
      );

/******************************************************************************/
/** Samples the registered variables to be written to a file
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaSwOsciSample(
      TBaSwOsciHdl hdl ///< [in] Handle
      );
//@}

#ifdef __cplusplus
} // extern c

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

/******************************************************************************/
/** Software oscilloscope C++ interface
 */
class IBaSwOsci {
public:

   /***************************************************************************/
   /** Registers a variable to the logger so it can be sampled. After the first
    *  @c Sample() is called, registering is no longer possible
    *  @return True if success, otherwise, false
    */
   virtual bool Register(
         void* pVar, ///< [in] Pointer to the variable
         EBaSwOsciType type, ///< [in] Type of the variable pointer
         const char *name, /**< [in] Variable name. Any commas in the name will
         be replaced*/
         const char *desc ///< [in] Variable description
         ) = 0;


   /***************************************************************************/
   /** Samples the registered variables to be written to a file
    *  @return True if success, otherwise, false
    */
   virtual bool Sample() = 0;

   // Typical object oriented destructor must be virtual!
   virtual ~IBaSwOsci() {};
};

/// @name C++ Factory
//@{
/******************************************************************************/
/** Create factory
 *  @return Handle if success, otherwise, null
 */
extern "C" IBaSwOsci *IBaSwOsciCreate(
      const char *name, ///< [in] Name
      const char *path, ///< [in] Directory where to save the file
      TBaBool toCnsole ///< [in] Flag to print to console too
      );

/******************************************************************************/
/** Destroy and release resources. Should not take more than 100ms to complete.
 *  returns with error after 2s
 *  @return false if error or timeout and @c timeoutMs != 0, otherwise, true
 */
extern "C" TBaBoolRC IBaSwOsciDestroy(
      IBaSwOsci *pHdl ///< [in] handle to destroy
      );

/******************************************************************************/
/** Destroy and release resources. If @c timeoutMs = 0, it behaves like an
 *  asynchronous call.
 *  @return false if error or timeout and @c timeoutMs != 0, otherwise, true
 */
extern "C" TBaBoolRC IBaSwOsciDestroyAsync(
      IBaSwOsci *pHdl ///< [in] handle to destroy
      );
//@}


#endif // __cplusplus
#endif // BASWOSCI_H_
