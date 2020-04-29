/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaseApi.h
 *   Date     : May 8, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  Includes the following:
 *  - API wide logger
 *  - The init/exit functions are not multi-thread safe and are NOT meant to be.
 *    The exit function has to be called after the logging functions are not
 *    used anymore.
 */
/*------------------------------------------------------------------------------
 */
#ifndef BASEAPI_H_
#define BASEAPI_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "BaBool.h"
#include "BaLog.h"
#include "BaCore.h"

/*------------------------------------------------------------------------------
 *  Defines
 */

/*------------------------------------------------------------------------------
 *  Type definitions
 */


/*------------------------------------------------------------------------------
 *  C interface
 */
#ifdef __cplusplus
extern "C" {
#endif

/// @name Resource Management
//@{
/******************************************************************************/
/** Initializes the logger that will be used throughout this API with default
 *  options.
 *  @return Error or success
 */
TBaBoolRC BaApiInitLoggerDef(
      const char* name ///< [in] Name of the default logger
      );

/******************************************************************************/
/** Initializes the logger that will be used throughout this API with default
 *  options.
 *  @return Error or success
 */
TBaBoolRC BaApiInitLoggerDefPath(
      const char* name, ///< [in] Name of the default logger
      const char* path ///< [in] Path of the default logger
      );

/******************************************************************************/
/** Initializes the logger that will be used throughout this API
 *  @return Error or success
 */
TBaBoolRC BaApiInitLogger(
      TBaLogDesc log ///< [in] Logger handle
      );

/******************************************************************************/
/** Releases the resources of the general logger
 *  @return Error or success
 */
TBaBoolRC BaApiExitLogger();
//@}

/******************************************************************************/
/** Logging function that uses the the logger initialized with
 *  #BaApiInitLogger() or #BaApiInitLoggerDef()
 *  @return Error or success
 */
TBaBoolRC BaApiLogF(
      EBaLogPrio prio, ///< [in] Priority
      const char* tag, ///< [in] Tag to track or group the message
      const char* fmt, ///< [in] The message format
      ... ///< [in] Variable arguments
      );

/******************************************************************************/
/** Flush the default logger
 */
TBaBoolRC BaApiFlushLog();


#ifdef __cplusplus
} // extern c

#endif // __cplusplus
#endif /* BASEAPI_H_ */
