/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaMsg.h
 *   Date     : Apr 26, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BAMSG_H_
#define BAMSG_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "BaLog.h"

/*------------------------------------------------------------------------------
 *  Type definitions
 */

/*------------------------------------------------------------------------------
 *  C interface
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} // extern c

/*------------------------------------------------------------------------------
 *  C++ Interface
 */
class IBaMsg {
public:

   /***************************************************************************/
   /** Prints a message to the console with state
    */
   virtual void SetPrintF(
         const char *fmt, ///> [in] Message format
         ...              ///> [in] Format arguments
         ) = 0;

   /***************************************************************************/
   /** Logs a message to the syslog with state
    */
   virtual void SetSysLogF(
         const char *tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         int line,        ///< [in] Line number, useful for debugging
         const char *fmt, ///< [in] Message format
         ...              ///< [in] Format arguments
         ) = 0;

   /***************************************************************************/
   /** Logs a message to @c pLog with state
    */
   virtual void SetLogF(
         IBaLog* pLog,    ///< [in] Handle of the target logger
         EBaLogPrio prio, ///< [in] Message priority
         const char *tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char *fmt, ///> [in] Message format
         ...              ///> [in] Format arguments
         ) = 0;

   virtual void Reset() = 0;

   // Typical object oriented destructor must be virtual!
   virtual ~IBaMsg() {};

};


/******************************************************************************/
/** Create factory for a message with state.
 *  @return Handle if success, otherwise, null
 */
extern "C" IBaMsg * CBaMsgCreate(
      );

/******************************************************************************/
/** Destroy and release resources of Message with state
 *  @return True if success, otherwise, false
 */
extern "C" TBaBoolRC CBaMsgDestroy(
      IBaMsg *pHdl ///< [in] BaLog handle to destroy
      );

#endif // __cplusplus
#endif // BAMSG_H_
