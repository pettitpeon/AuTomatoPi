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


typedef struct TBaApiCtrlTaskOpts {
   const char* name;
   EBaCorePrio prio;
   uint32_t cyleTimeMs;

   TBaBoolRC (* init  )(void*);
   void* initArg;
   void      (* update)(void*);
   void* updateArg;
   TBaBoolRC (* exit  )(void*);
   void* exitArg;
} TBaApiCtrlTaskOpts;

/******************************************************************************/
/** ...
 */
TBaBoolRC BaApiStartCtrlTask(TBaApiCtrlTaskOpts* pOpts);

TBaBoolRC BaApiStopCtrlTask();


#ifdef __cplusplus
} // extern c

/*------------------------------------------------------------------------------
 *  C++ Interface
 */

// Cpp factory

#endif // __cplusplus
#endif /* BASEAPI_H_ */
