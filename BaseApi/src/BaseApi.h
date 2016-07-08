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
 *  - Control task
 *
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
/// Options structure for the control task started with #BaApiStartCtrlTask()
typedef struct TBaApiCtrlTaskOpts {
   const char* name; ///< Task name
   EBaCorePrio prio; ///< Task priority
   uint32_t cyleTimeMs; ///< Desired cycle time in ms
   TBaLogDesc log; ///< The logger descriptor union

   // Control task callbacks
   TBaBoolRC (*init)(void*); ///< Optional initialization function
   void* initArg; ///< Optional argument init function
   void (*update)(void*); /**< Function that will be called cyclically every
    #cyleTimeMs */
   void* updateArg; ///< Optional argument update function
   TBaBoolRC (*exit)(void*); ///< Optional exit function
   void* exitArg; ///< Optional argument exit function
} TBaApiCtrlTaskOpts;

/// Control task statistics
typedef struct TBaApiCtrlTaskStats {
   TBaBool  imRunning; ///< The task is running
   uint64_t updCnt; ///<  No. of updates so far
   uint64_t lastDurUs; ///< Duration of the last @c update()
   uint64_t lastCycleUs; ///< Duration of the last cycle (including sleep)
} TBaApiCtrlTaskStats;

/*------------------------------------------------------------------------------
 *  C interface
 */
#ifdef __cplusplus
extern "C" {
#endif

/// @name Resource Management
//@{
/******************************************************************************/
/** todo: stub
 *  @return Error or success
 */
TBaBoolRC BaApiInit();

/******************************************************************************/
/** todo: stub
 *  @return Error or success
 */
TBaBoolRC BaApiExit();

/******************************************************************************/
/** Initializes the logger that will be used throughout this API with default
 *  options.
 *  @return Error or success
 */
TBaBoolRC BaApiInitLoggerDef(
      const char* name ///< [in] Name of the default logger
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

/// @name Control Task
//@{
/******************************************************************************/
/** Starts the one and only control task as a new process. This also
 *  automatically initializes the default logger if it not previously
 *  initialized via #BaApiInitLogger(). If a specialized logger is required, one
 *  could initialize it in the init() callback
 *  @return Error or success
 */
TBaBoolRC BaApiStartCtrlTask(
      const TBaApiCtrlTaskOpts* pOpts ///< [in] Task options
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
      const TBaApiCtrlTaskOpts* pOpts ///< [in] Task options
      );

/******************************************************************************/
/** Stops the one and only control thread
 *  @return Error or success
 */
TBaBoolRC BaApiStopCtrlThread();

/******************************************************************************/
/** Gets the task statistics. This is mostly meant to be called within the
 *  @c update() function so it gets the statistics from the control task
 *  @return Error or success
 */
TBaBoolRC BaApiGetCtrlTaskStats(
      TBaApiCtrlTaskStats *pStats  ///< [out] Statistics
      );
//@}

#ifdef __cplusplus
} // extern c

#endif // __cplusplus
#endif /* BASEAPI_H_ */
