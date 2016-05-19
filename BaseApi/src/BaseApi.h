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
 *  stub
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
///
typedef struct TBaApiCtrlTaskOpts {
   const char* name; ///< Task name
   EBaCorePrio prio; ///< Task priority
   uint32_t cyleTimeMs; ///< Desired cycle time in ms

   TBaBoolRC (*init)(void*); ///< Optional initialization function
   void* initArg; ///< Optional arguments init function
   void (*update)(void*); ///<
   void* updateArg; ///< Optional arguments update function
   TBaBoolRC (*exit)(void*); ///< Optional exit function
   void* exitArg; ///< Optional arguments exit function

   TBaLogHdl log;
} TBaApiCtrlTaskOpts;

/*------------------------------------------------------------------------------
 *  C interface
 */
#ifdef __cplusplus
extern "C" {
#endif

/// @name Resource management
//@{
/******************************************************************************/
/** ...
 */
TBaBoolRC BaApiInit();

/******************************************************************************/
/** ...
 */
TBaBoolRC BaApiExit();

/******************************************************************************/
/** ...
 */
TBaBoolRC BaApiInitLoggerDef(
      const char* name
      );

/******************************************************************************/
/** ...
 */
TBaBoolRC BaApiInitLogger(
      TBaLogHdl hdl
      );

/******************************************************************************/
/** ...
 */
TBaBoolRC BaApiExitLogger();
//@}

/******************************************************************************/
/** ...
 */
TBaBoolRC BaApiLogF(EBaLogPrio prio, const char* tag, const char* fmt, ...);

/// @name Control task
//@{
/******************************************************************************/
/** Starts the one and only control task as a new process
 *  @return Error or success
 */
TBaBoolRC BaApiStartCtrlTask(
      TBaApiCtrlTaskOpts* pOpts ///< [in] Task options
      );

/******************************************************************************/
/** Stops the one and only control task
 *  @return Error or success
 */
TBaBoolRC BaApiStopCtrlTask();

/******************************************************************************/
/** Starts the one and only control thread as a new thread. This is mainly for
 *  debugging and development. For proper releases use #BaApiStartCtrlTask().
 *  @return Error or success
 */
TBaBoolRC BaApiStartCtrlThread(
      TBaApiCtrlTaskOpts* pOpts ///< [in] Task options
      );

/******************************************************************************/
/** Stops the one and only control thread
 *  @return Error or success
 */
TBaBoolRC BaApiStopCtrlThread();
//@}

#ifdef __cplusplus
} // extern c

/*------------------------------------------------------------------------------
 *  C++ Interface
 */

// Cpp factory

#endif // __cplusplus
#endif /* BASEAPI_H_ */
