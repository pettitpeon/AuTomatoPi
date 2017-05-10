/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : OsProc.h
 *   Date     : Jun 26, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  Process functions
 *   - Names
 *   - PIDs
 *   - Priorities
 *   - Control Task
 */
/*------------------------------------------------------------------------------
 */
#ifndef OSPROC_H_
#define OSPROC_H_

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include "BaBool.h"
#include "BaCore.h"
#include "BaLog.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define BAPROC_SHORTNAMELEN 16 ///< Standard length of program names 'proc/comm'
#define BAPROC_FULLNAMELEN  256 ///< Maximum length of a file's name

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
/// Options structure for the control task started with #BaApiStartCtrlTask()
typedef struct TOsProcCtrlTaskOpts {
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
} TOsProcCtrlTaskOpts;

/// Control task statistics
typedef struct TOsProcCtrlTaskStats {
   TBaBool  imRunning; ///< The task is running
   uint64_t updCnt; ///<  No. of updates so far
   uint64_t lastDurUs; ///< Duration of the last @c update()
   uint64_t lastCycleUs; ///< Duration of the last cycle (including sleep)
} TOsProcCtrlTaskStats;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/// @name Name of Processes
//@{
/******************************************************************************/
/** Get the short name of own process
 *  @return on success, the process name, otherwise, 0
 */
const char* OsProcGetOwnShortName();

/******************************************************************************/
/** Get the full name of own process
 *  @return on success, the process name, otherwise, 0
 */
const char* OsProcGetOwnFullName();

/******************************************************************************/
/** Gets the process name from the PID. If no @c buf is provided, the pointer is
 *  mallocated and has to be freed by the user to avoid memory leaks
 *  @return on success, the process name, otherwise, 0
 */
const char* OsProcGetPIDName(
      pid_t pid, ///< [in] Process ID
      char buf[BAPROC_SHORTNAMELEN] ///< [out] Optional buffer
      );
//@} Name of Processes

/// @name Control TasK PID File
//@{
/******************************************************************************/
/** Write/overwrite the Control Task PID file which is a special PID file. This
 *  function is meant to be called from within the forked control task.
 *  @return Success or error
 */
TBaBoolRC OsProcWriteCtrlTaskPidFile();

/******************************************************************************/
/** Read the Control Task PID file. The special control task PID file contains
 *  name of the binary. This name is written to @c buf if the pointer is
 *  provided
 *  @return The PID on success or 0 on error
 */
pid_t OsProcReadCtrlTaskPidFile(
      char buf[BAPROC_SHORTNAMELEN] /**< [out] Optional buffer to read the name
      of the binary*/
      );

/******************************************************************************/
/** Deletes the Control Task PID file.
 *  @return Success or error
 */
TBaBoolRC OsProcDelCtrlTaskPidFile();
//@}  Control TasK PID File

/// @name PID Files
//@{
/******************************************************************************/
/** Writes the PID file of an internal program.
 *  @return Success or error
 */
TBaBoolRC OsProcWriteOwnPidFile();

/******************************************************************************/
/** Tries to read the PID from an internal program, or from a path
 *  @return on success, the PID, otherwise, 0
 */
pid_t OsProcReadPidFile(
      const char *progName, ///< [in] Internal program name or full path to PID file
      TBaBool internal /**< [in] Flag to signal that the program was created
      with this API */
      );

/******************************************************************************/
/** Delete the PID file
 *  @return on success, the PID, otherwise, 0
 */
TBaBoolRC OsProcDelPidFile(
      const char *progName, ///< [in] Internal program name or full path to PID file
      TBaBool internal /**< [in] Flag to signal that the program was created
      with this API */
      );

/******************************************************************************/
/** Test if a program with the PID inside the PID file is running and has the
 *  same name as @c progName
 *  @return true or false
 */
TBaBool OsProcPidFileIsRunning(
      const char *progName, ///< [in] Internal program name or full path to PID file
      TBaBool internal /**< [in] Flag to signal that the program was created
      with this API */
      );
//@} PID Files

/// @name Process Priority
//@{
/******************************************************************************/
/** Set the priority of the calling process.
 *  @return Error or success
 */
TBaBoolRC OsProcSetOwnPrio(
      EBaCorePrio prio    ///< [in] New priority
      );

/******************************************************************************/
/** Get the priority of the calling process.
 *  @return the priority
 */
EBaCorePrio OsProcGetOwnPrio();
//@} Process Priority


// ======================== =================================

/// @name Control Task
//@{
/******************************************************************************/
/** Starts the one and only control task as a new process. This also
 *  automatically initializes the default logger if it not previously
 *  initialized via #BaApiInitLogger(). If a specialized logger is required, one
 *  could initialize it in the init() callback
 *  @return Error or success
 */
TBaBoolRC OsApiStartCtrlTask(
      const TOsProcCtrlTaskOpts* pOpts ///< [in] Task options
      );

/******************************************************************************/
/** Stops the one and only control task
 *  @return Error or success
 */
TBaBoolRC OsApiStopCtrlTask();

/******************************************************************************/
/** Starts the one and only control thread as a new thread. This is mainly for
 *  debugging and development. For proper releases use #BaApiStartCtrlTask().
 *  @return Error or success
 */
TBaBoolRC OsApiStartCtrlThread(
      const TOsProcCtrlTaskOpts* pOpts ///< [in] Task options
      );

/******************************************************************************/
/** Stops the one and only control thread
 *  @return Error or success
 */
TBaBoolRC OsApiStopCtrlThread();

/******************************************************************************/
/** Gets the task statistics. This is mostly meant to be called within the
 *  @c update() function so it gets the statistics from the control task
 *  @return Error or success
 */
TBaBoolRC OsApiGetCtrlTaskStats(
      TOsProcCtrlTaskStats *pStats  ///< [out] Statistics
      );
//@}


#ifdef __cplusplus
} // extern c
#endif // __cplusplus

#endif // OSPROC_H_
