/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaProc.h
 *   Date     : Jun 26, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BAPROC_H_
#define BAPROC_H_

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include "BaBool.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define BAPROC_SHORTNAMELEN 16
#define BAPROC_FULLNAMELEN  256

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/** Get the short name of own process
 *  @return on success, the process name, otherwise, 0
 */
const char* BaProcGetOwnShortName();

/******************************************************************************/
/** Get the full name of own process
 *  @return on success, the process name, otherwise, 0
 */
const char* BaProcGetOwnFullName();

/******************************************************************************/
/** ...
 *  @return on success, the process name, otherwise, 0
 */
const char* BaProcGetPIDShortName(
      pid_t pid, ///< Process ID
      char buf[BAPROC_SHORTNAMELEN] ///< [in,out] Optional buffer
      );

/******************************************************************************/
/** ...
 *  @return on success, the process name, otherwise, 0
 */
const char* BaProcGetPIDFullName(
      pid_t pid, ///< Process ID
      char buf[BAPROC_FULLNAMELEN] ///< [in,out] Optional buffer
      );

/// @name PID Files
//@{
/******************************************************************************/
/** Tries to read the PID from an internal program, or from a path
 *  @return on success, the PID, otherwise, -1
 */
pid_t BaProcReadPidFile(
      const char *progName, ///< [in] Internal program name or path to PID file
      TBaBool internal /**< [in] Flag to signal that the program was created
      with this API */
      );

/******************************************************************************/
/** TODO: Checks if the internal program is running
 *  @return Success or error
 */
TBaBoolRC BaProcTestPidFile(
      const char *progName
      );

/******************************************************************************/
/** Writes the PID file of an internal program. The own name can be read with
 *  #BaProcGetOwnName().
 *  @return Success or error
 */
TBaBoolRC BaProcWritePidFile(
      const char *progName ///< [in] Internal program name
      );

/******************************************************************************/
/** Remover the PID file of an internal program. The own name can be read with
 *  #BaProcGetOwnName().
 *  @return Success or error
 */
TBaBoolRC BaProcRemovePidFile(
      const char *progName ///< [in] Internal program name
      );
//@} PID Files


#ifdef __cplusplus
} // extern c
#endif // __cplusplus

#endif // BAPROC_H_
