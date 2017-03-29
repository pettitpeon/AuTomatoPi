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
#include "BaIpcRegistry.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
/// C message handle
typedef void* TBaIpcHdl;



// IPC message
//typedef struct TBaIpcVarReq {
//   int32_t type;
//   union {
//      uint64_t i;
//      char str[1000];
//      char dat[1000];
//   } data;
//} TBaIpcVarReq;


/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/// @name Server Interface
//@{
/******************************************************************************/
/** Initializes and starts the server thread. It does not wait to check if it
 *  started successfully
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcInitSvr();

/******************************************************************************/
/** Get the server status. Returns only if the server started or stopped
 *  successfully, and no heart beat of any kind.
 *  @return True if the server thread started running, false if stopped.
 */
TBaBoolRC BaIpcSvrRunning();

/******************************************************************************/
/** Sends exit signal to the IPC server and waits with a timeout for it to stop.
 *  If it timeouts, there is a small memory leak, but the thread exits when it
 *  frees itself.
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcExitSvr();
//@}

/// @name Client Interface
//@{
/******************************************************************************/
/** Initialize the IPC client. This should be called in an external program
 *  which wishes to communicate with the control task.
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcInitClnt();

/******************************************************************************/
/** Exit the IPC client releasing the resources. This should be called in an
 *  external program which wishes to close the communication with the control
 *  task.
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcExitClnt();

/******************************************************************************/
/** Remotely call (RPC) a registered function from the control task
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIpcCallFun(
      const char* name, ///< [in] Function name
      TBaIpcFunArg a, ///< [in] Function arguments
      TBaIpcArg *pRet ///< [out] Function return value
      );
//@}



#ifdef __cplusplus
} // extern c

#endif // __cplusplus
#endif // BAIPC_H_
