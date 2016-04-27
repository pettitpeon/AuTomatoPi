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
 *  API for messages with state. A message with state can be set and rest. If it
 *  is set, it cannot be set again before reseting it. These messages are mainly
 *  used to avoid cyclic messages
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
 *  Defines
 */
/// Syslog macro for C API
#define BAMSGSETSYSLOGF(log, tag, fmt, ...) BaMsgSetSysLogF(log, tag, __LINE__, fmt, __VA_ARGS__)

/// Syslog macro for C API
#define BAMSGSETSYSLOG(log, tag, msg) BaMsgSetSysLog(log, tag, __LINE__, msg)

/// Syslog macro for C++ API
#define IBAMSGSETSYSLOGF(tag, fmt, ...) SetSysLogF(tag, __LINE__, fmt, __VA_ARGS__)

/// Syslog macro for C++ API
#define IBAMSGSETSYSLOG(tag, msg) SetSysLog(tag, __LINE__, msg)

/*------------------------------------------------------------------------------
 *  Type definitions
 */
/// C logger handle
typedef void* TBaMsgHdl;

/*------------------------------------------------------------------------------
 *  C interface
 */
#ifdef __cplusplus
extern "C" {
#endif

/// @name Factory
//@{
/******************************************************************************/
/** Create factory for a msg.
 *  @return Handle if success, otherwise, null
 */
TBaMsgHdl BaMsgCreate();

/******************************************************************************/
/** Destroy and release resources of msg
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaMsgDestroy(
      TBaMsgHdl hdl ///< [in] BaLog handle to destroy
      );
//@}

/// @name Set to console
//@{
/******************************************************************************/
/** Prints a message to the console with state
 */
void BaMsgSetPrintF(
      TBaMsgHdl  hdl,  ///< [in] Handle
      const char *fmt, ///< [in] Message format
      ...              ///< [in] Format arguments
      );

/******************************************************************************/
/** Prints a message to the console with state
 */
void BaMsgSetPrint(
      TBaMsgHdl  hdl, ///< [in] Handle
      const char *msg ///< [in] Message format
      );

/// @name Set to syslog
//@{
/******************************************************************************/
/** Logs a message to the syslog with state and maximum length of 65534 chars
 */
void BaMsgSetSysLogF(
      TBaMsgHdl  hdl,  ///< [in] Handle
      const char *tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      int line,        ///< [in] Line number, useful for debugging
      const char *fmt, ///< [in] Message format
      ...              ///< [in] Format arguments
      );

/******************************************************************************/
/** Logs a message to the syslog with state and maximum length of 65534 chars
 */
void BaMsgSetSysLog(
      TBaMsgHdl  hdl,  ///< [in] Handle
      const char *tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      int line,        ///< [in] Line number, useful for debugging
      const char *msg  ///< [in] Message format
      );
//@}

/// @name Set to log
//@{
/******************************************************************************/
/** Logs a message to @c pLog with state and maximum length of 65534 chars
 */
void BaMsgSetLogF(
      TBaMsgHdl  hdl,  ///< [in] Handle
      TBaLogHdl  log,  ///< [in] Handle of the target logger
      EBaLogPrio prio, ///< [in] Message priority
      const char *tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      const char *fmt, ///< [in] Message format
      ...              ///< [in] Format arguments
      );

/******************************************************************************/
/** Logs a message to @c pLog with state and maximum length of 65534 chars
 */
void BaMsgSetLog(
      TBaMsgHdl  hdl,  ///< [in] Handle
      TBaLogHdl  log,  ///< [in] Handle of the target logger
      EBaLogPrio prio, ///< [in] Message priority
      const char *tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      const char *msg  ///< [in] Message format
      );
//@}

/// @name Get, reset
/******************************************************************************/
/** Reset a set message
 */
void BaMsgReset(
      TBaMsgHdl  hdl ///< [in] Handle
      );

/******************************************************************************/
/** Get the state of the message
 *  @return true if set, false if reset
 */
TBaBool BaMsgGet(
      TBaMsgHdl hdl ///< [in] Handle
      );
//@}


#ifdef __cplusplus
} // extern c

/*------------------------------------------------------------------------------
 *  C++ Interface
 */
class IBaMsg {
public:

   /// @name Set to console
   //@{
   /***************************************************************************/
   /** Prints a message to the console with state
    */
   virtual void SetPrintF(
         const char *fmt, ///< [in] Message format
         ...              ///< [in] Format arguments
         ) = 0;

   /***************************************************************************/
   /** Prints a message to the console with state
    */
   virtual void SetPrint(
         const char *msg ///< [in] Message format
         ) = 0;
   //@}

   /// @name Set to syslog
   //@{
   /***************************************************************************/
   /** Logs a message to the syslog with state and maximum length of 65534 chars
    */
   virtual void SetSysLogF(
         const char *tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         int line,        ///< [in] Line number, useful for debugging
         const char *fmt, ///< [in] Message format
         ...              ///< [in] Format arguments
         ) = 0;

   /***************************************************************************/
   /** Logs a message to the syslog with state and maximum length of 65534 chars
    */
   virtual void SetSysLog(
         const char *tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         int line,        ///< [in] Line number, useful for debugging
         const char *msg  ///< [in] Message format
         ) = 0;
   //@}

   /// @name Set to log
   //@{
   /***************************************************************************/
   /** Logs a message to @c pLog with state and maximum length of 65534 chars
    */
   virtual void SetLogF(
         IBaLog* pLog,    ///< [in] Handle of the target logger
         EBaLogPrio prio, ///< [in] Message priority
         const char *tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char *fmt, ///< [in] Message format
         ...              ///< [in] Format arguments
         ) = 0;

   /***************************************************************************/
   /** Logs a message to @c pLog with state and maximum length of 65534 chars
    */
   virtual void SetLog(
         IBaLog* pLog,    ///< [in] Handle of the target logger
         EBaLogPrio prio, ///< [in] Message priority
         const char *tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char *msg  ///< [in] Message format
         ) = 0;
   //@}

   /// @name Get, reset
   //@{
   /***************************************************************************/
   /** Reset a set message
    */
   virtual void Reset() = 0;

   /***************************************************************************/
   /** Get the state of the message
    *  @return true if set, false if reset
    */
   virtual bool Get() = 0;
   //@}

   // Typical object oriented destructor must be virtual!
   virtual ~IBaMsg() {};

};

/// @name C++ Factory
//@{
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
//@}

#endif // __cplusplus
#endif // BAMSG_H_
