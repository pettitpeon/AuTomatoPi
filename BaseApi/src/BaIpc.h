/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaIpc.h
 *   Date     : Nov 7, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BAIPC_H_
#define BAIPC_H_

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
typedef void* TBaIpcHdl;

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
TBaBoolRC BaIpcCreatePipeReader(
      );

/******************************************************************************/
/** Create factory for ...
 *  @return Handle if success, otherwise, null
 */
TBaBoolRC BaIpcCreatePipeWriter(
      );

/******************************************************************************/
/** Destroy and release resources
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcDestroyPipe(
      );
//@}

TBaBoolRC BaIpcReadPipe(
      void* pData,
      size_t size
      );

TBaBoolRC BaIpcWritePipe(
      const void* pData,
      size_t size
      );


#ifdef __cplusplus
} // extern c

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

class IBaIpc {
public:

   /***************************************************************************/
   /** ...
    *  @return ...
    */
	virtual void Iface(
         const char *s ///< [in] string
			) = 0;

   // Typical object oriented destructor must be virtual!
   virtual ~IBaIpc() {};
};

/// @name C++ Factory
//@{
/******************************************************************************/
/** Create factory for ...
 *  @return Handle if success, otherwise, null
 */
extern "C" IBaIpc * IBaIpcCreate();

/******************************************************************************/
/** Destroy and release resources
 *  @return True if success, otherwise, false
 */
extern "C" TBaBoolRC IBaIpcDestroy(
      IBaIpc *pHdl ///< [in] handle to destroy
      );
//@}


#endif // __cplusplus
#endif // BAIPC_H_
