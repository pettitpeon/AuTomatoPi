/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaCore.h
 *   Date     : 23/06/2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *   OS level API. Core functions include:
 *    - Timers
 *    - Threads
 *    - Sleeps
 */
/*------------------------------------------------------------------------------
 */

#ifndef _BACORE_H
#define _BACORE_H

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "BaBool.h"
#include <unistd.h>


/*------------------------------------------------------------------------------
 *  Type definitions
 */
/// Time stamp structure
typedef struct TBaCoreTimeStamp {
   time_t tt;
   uint32_t micros;
   uint32_t millis;
} TBaCoreTimeStamp;


/// Thread priority and scheduler. RT is FIFO
// Windows does not have RT
typedef enum EBaCorePrio {
   eBaCorePrio_Minimum = 0, ///< 0
   eBaCorePrio_Low,         ///< 1
   eBaCorePrio_Normal,      ///< 2
   eBaCorePrio_High,        ///< 3
   eBaCorePrio_Highest,     ///< 4
   eBaCorePrio_RT_Normal,   ///< 5
   eBaCorePrio_RT_High,     ///< 6
   eBaCorePrio_RT_Highest   ///< 7
} EBaCorePrio;

/// Thread function arguments
typedef struct TBaCoreThreadArg {
   void    *pArg; ///< General arguments used in the thread
   TBaBool exitTh;  ///< Exit flag to inform that it must return
} TBaCoreThreadArg;

/// Thread info
typedef struct TBaCoreThreadInfo {
   char         name[64];  ///< Thread name
   EBaCorePrio  prio;      ///< Thread priority and scheduler
   pid_t        tid;       ///< Thread ID
   TBaBool      isRunning; ///< Is running flag
} TBaCoreThreadInfo;


/// General function of the API
typedef void (*TBaCoreFun)(void* arg);

/// Functions for threads
typedef void (*TBaCoreThreadFun)(TBaCoreThreadArg* pArg);

/// Thread handle.
typedef const void* TBaCoreThreadHdl;

/*------------------------------------------------------------------------------
 *  API Functions
 */
#ifdef __cplusplus
extern "C" {
#endif


/// @name Sleep functions
//@{
/******************************************************************************/
/** Puts the thread to sleep and causes context change.
 *  if s = 0 || ms = 0 || us = 0, there is no sleep
 *  if ns = 0, the thread sleeps for one tick as specified by nanosleep()
 */
void BaCoreSleep(
      int64_t s ///< [in] Sleep time in s
      );
void BaCoreMSleep(
      int64_t ms ///< [in] Sleep time in ms
      );
void BaCoreUSleep(
      int64_t us ///< [in] Sleep time in us
      );
void BaCoreNSleep(
      int64_t ns ///< [in] Sleep time in ns
      );
//@} Sleep functions


/// @name Timing functions
//@{
/******************************************************************************/
/** Timing function
 *  @return Time in the defined unit
 */
int64_t BaCoreTimedS(
      TBaCoreFun fun, ///< [in] Function to be timed
      void* pArg      ///< [in] Function arguments
      );
int64_t BaCoreTimedMs(
      TBaCoreFun fun, ///< [in] Function to be timed
      void* pArg      ///< [in] Function arguments
      );
int64_t BaCoreTimedUs(
      TBaCoreFun fun, ///< [in] Function to be timed
      void* pArg      ///< [in] Function arguments
      );

void BaCoreGetTStamp(
      TBaCoreTimeStamp *pStamp ///< [out] Time stamp
      );

const char* BaCoreGetTStampStr(
      const TBaCoreTimeStamp *pStamp
      );
//@} Timing functions


/// @name Multi-threading
//@{
/******************************************************************************/
/** Thread factory
 *  @return Thread handle
 */
TBaCoreThreadHdl BaCoreCreateThread(
      const char *name,         ///< [in] Thread name
      TBaCoreThreadFun routine, ///< [in] Thread entry function
      TBaCoreThreadArg *pArg,   ///< [in] Arguments of thread function
      EBaCorePrio prio          ///< [in] Thread priority
      );

/******************************************************************************/
/** Thread destructor. It signals the thread to end and waits a maximum of
 *  @c timeOutMs ms. If it timeouts, the thread detaches, stays alive, and
 *  releases the resources when the thread routine returns.
 *  @return Error if bad handle, or success
 */
TBaBoolRC BaCoreDestroyThread(
      TBaCoreThreadHdl hdl, ///< [in] Thread handle
      uint32_t timeOutMs    ///< [in] Timeout to wait in ms.
      );

/******************************************************************************/
/** Get the thread info
 *  @return Error or success
 */
TBaBoolRC BaCoreGetThreadInfo(
      const TBaCoreThreadHdl hdl, ///< [in] Thread handle
      TBaCoreThreadInfo *pInfo    ///< [out] Thread info
      );
//@} Multi-threading

/// @name Other getter setters
//@{
/******************************************************************************/
/** Set the priority of the calling process. TODO: unit test
 *  @return Error or success
 */
TBaBoolRC BaCoreSetOwnProcPrio(
      EBaCorePrio prio    ///< [in] New priority
      );

/******************************************************************************/
/** Get the priority of the calling process. TODO: unit test
 *  @return the priority
 */
EBaCorePrio BaCoreGetOwnProcPrio();

/******************************************************************************/
/** Gets the name of the calling process. Mangling with the returned pointer
 *  leads to undefined behavior, please do not mess with it
 *  @return The name of the calling process
 */
const char* BaCoreGetOwnName();
//@} Other getter setters

/// @name PID Files
//@{
/******************************************************************************/
/** Tries to read the PID from an internal program, or from a path
 *  @return on success, the PID, otherwise, -1
 */
pid_t BaCoreReadPidFile(
      const char *progName, ///< [in] Internal program name or path to PID file
      TBaBool internal /**< [in] Flag to signal that the program was created
      with this API */
      );

/******************************************************************************/
/** TODO: Checks if the internal program is running
 *  @return Success or error
 */
TBaBoolRC BaCoreTestPidFile(
      const char *progName
      );

/******************************************************************************/
/** Writes the PID file of an internal program. The own name can be read with
 *  #BaCoreGetOwnName().
 *  @return Success or error
 */
TBaBoolRC BaCoreWritePidFile(
      const char *progName ///< [in] Internal program name
      );

/******************************************************************************/
/** Remover the PID file of an internal program. The own name can be read with
 *  #BaCoreGetOwnName().
 *  @return Success or error
 */
TBaBoolRC BaCoreRemovePidFile(
      const char *progName ///< [in] Internal program name
      );
//@} PID Files

#ifdef __cplusplus
}
#endif
#endif // _BACORE_H

