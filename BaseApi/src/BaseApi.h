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
/// Options structure for the control task started with #BaApiStartCtrlTask()
typedef struct TBaApiCtrlTaskOpts {
   const char* name; ///< Task name
   EBaCorePrio prio; ///< Task priority
   uint32_t cyleTimeMs; ///< Desired cycle time in ms

   TBaBoolRC (*init)(void*); ///< Optional initialization function
   void* initArg; ///< Optional argument init function
   void (*update)(void*); /**< Function that will be called cyclically every
    #cyleTimeMs */
   void* updateArg; ///< Optional argument update function
   TBaBoolRC (*exit)(void*); ///< Optional exit function
   void* exitArg; ///< Optional argument exit function

   TBaLogDesc log; ///< The logger descriptors

} TBaApiCtrlTaskOpts;

///
typedef struct TBaApiCtrlTaskStats {
   TBaBool  imRunning;
   uint64_t updCnt;
   uint64_t actDurUs;
   uint64_t lastCycleUs;
} TBaApiCtrlTaskStats;

/*------------------------------------------------------------------------------
 *  C interface
 */
#ifdef __cplusplus
extern "C" {
#endif

/// @name Resource management
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

/// @name Control task
//@{
/******************************************************************************/
/** Starts the one and only control task as a new process. This also
 *  automatically initializes the default logger if it not previously
 *  initialized via #BaApiInitLogger(). If a specialized logger is required, one
 *  could initialize it in the init() callback
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

/******************************************************************************/
/** Gets the task statistics. This is mostly meant to be called within the
 *  @c update() function so it gets the stats from the task
 *  @return Error or success
 */
TBaBoolRC BaApiGetCtrlTaskStats(
      TBaApiCtrlTaskStats *pStats  ///< [out] Statistics
      );
//@}

#ifdef __cplusplus
} // extern c

/*------------------------------------------------------------------------------
 *  C++ Interface
 */

// Cpp factory

#endif // __cplusplus
#endif /* BASEAPI_H_ */
