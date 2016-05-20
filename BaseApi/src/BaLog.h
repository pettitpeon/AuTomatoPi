/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaLog.h
 *   Date     : Dec 9, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  Logger API. Handles logging to files and to syslog. The logger state can be
 *  saved to an .ini file to be retrieved later. Among other this API offers:@n
 *   - Priority filter
 *   - Logging to console and/or file
 *   - Message buffering for better performance
 *   - Manual flushing
 *   - File size limit
 *   - Ring files
 */
/*------------------------------------------------------------------------------
 */
#ifndef BALOG_H_
#define BALOG_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "BaBool.h"

/*------------------------------------------------------------------------------
 *  Defines
 */
/// Syslog macro that includes the line number.
#define BASYSLOG(tag, fmt, ...) BaLogSysLogF(tag, __LINE__, fmt, ##__VA_ARGS__)

/*------------------------------------------------------------------------------
 *  Type definitions
 */
/// Message priority or severeness
typedef enum EBaLogPrio {
   eBaLogPrio_Trace = 0, ///< 0
   eBaLogPrio_Warning,   ///< 1
   eBaLogPrio_Error,     ///< 2
   eBaLogPrio_UpsCrash   ///< 3
} EBaLogPrio;


/// Logger output enumeration
typedef enum EBaLogOut {
   eBaLogOut_Log = 1,           ///< 1
   eBaLogOut_Console = 2,       ///< 2
   eBaLogOut_LogAndConsole = 3, ///< 3
} EBaLogOut;

/// Logger options
typedef struct TBaLogOptions {
   const char *name; ///< Name of the logger
   const char *path; ///< Directory path for saving the logger
   EBaLogPrio  prioFilt; ///< Priority filter
   EBaLogOut   out; ///< Output specifier
   uint32_t    maxFileSizeB; ///< Maximum file size in bytes
   uint16_t    maxNoFiles; ///< Maximum number of extra files
   uint16_t    maxBufLength; ///< Maximum number of messages in the buffer
} TBaLogOptions;

/// Logger information
typedef struct TBaLogInfo {
   const char *name; ///< Name of the logger
   const char *fullPath; ///< Directory path for saving the logger
   EBaLogPrio  prioFilt; ///< Priority filter
   EBaLogOut   out; ///< Output specifier
   uint32_t    maxFileSizeB; ///< Maximum file size in bytes
   uint16_t    maxNoFiles; ///< Maximum number of extra files
   uint16_t    maxBufLength; ///< Maximum number of messages in the buffer
   uint32_t    fileSizeB; ///< File size of current file in bytes
} TBaLogInfo;

/// C logger handle
typedef void* TBaLogHdl;

/*------------------------------------------------------------------------------
 *  C interface
 */
#ifdef __cplusplus
extern "C" {
#endif

/// @name Factory
//@{
/******************************************************************************/
/** Create factory for a logger with defaults
 *  @return Handle if success, otherwise, null
 */
TBaLogHdl BaLogCreateDef(
      const char *name ///< [in] Name of the logger
      );

/******************************************************************************/
/** Create factory for a logger.
 *  @return Handle if success, otherwise, null
 */
TBaLogHdl BaLogCreate(
      const TBaLogOptions *pOpts ///< [in] Logger options
      );

/******************************************************************************/
/** Sets the a TBaLogOptions structure to default values
 */
void BaLogSetDefOpts(
      TBaLogOptions *pOpts ///< [out] Pointer to struct
      );

/******************************************************************************/
/** Destroy and release resources of logger
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaLogDestroy(
      TBaLogHdl hdl,  ///< [in] BaLog handle to destroy
      TBaBool saveCfg ///< [in] Flag to specify if the state should be saved in a cfg file
      );
//@}

/// @name Logging functions
//@{
/******************************************************************************/
/** Logs a message into the logger and adds a @c tag and a time stamp.
 *  The @c BaLogTrace() and family functions have an implied priority
 *  @return true if success, otherwise, false. False if buffer is full or @c msg
 *  is null
 */
TBaBoolRC BaLogLog(
      TBaLogHdl hdl,    ///< [in] Handle
      EBaLogPrio  prio, ///< [in] Message priority
      const char* tag,  ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      const char* msg   ///< [in] Message to log
      );
TBaBoolRC BaLogTrace(
      TBaLogHdl hdl,   ///< [in] Handle
      const char* tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      const char* msg  ///< [in] Message to log
      );
TBaBoolRC BaLogWarning(
      TBaLogHdl hdl,   ///< [in] Handle
      const char* tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      const char* msg  ///< [in] Message to log
      );
TBaBoolRC BaLogError(
      TBaLogHdl hdl,   ///< [in] Handle
      const char* tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      const char* msg  ///< [in] Message to log
      );
//@}


/// @name Logging functions with format
//@{
/***************************************************************************/
/** Logs a message into the logger like @c printf() and adds a @c tag and a
 *  time stamp. These functions have a limit of 65534 characters per message.
 *  The @c BaLogTraceF() and family functions have an implied priority
 *  @return true if success, otherwise, false. False if buffer is full or @c fmt
 *  is null
 */
TBaBoolRC BaLogLogF(
      TBaLogHdl   hdl,  ///< [in] Handle
      EBaLogPrio  prio, ///< [in] Message priority
      const char* tag,  ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      const char* fmt,  ///< [in] Message format
      ...               ///< [in] Format arguments
      );
TBaBoolRC BaLogTraceF(
      TBaLogHdl   hdl,  ///< [in] Handle
      const char* tag,  ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      const char* fmt,  ///< [in] Message format
      ...               ///< [in] Format arguments
      );
TBaBoolRC BaLogWarningF(
      TBaLogHdl   hdl,  ///< [in] Handle
      const char* tag,  ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      const char* fmt,  ///< [in] Message format
      ...               ///< [in] Format arguments
      );
TBaBoolRC BaLogErrorF(
      TBaLogHdl   hdl,  ///< [in] Handle
      const char* tag,  ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      const char* fmt,  ///< [in] Message format
      ...               ///< [in] Format arguments
      );
//@}

/// @name Getter
//@{
/***************************************************************************/
/** Gets the information and options.
 *  The const char pointers of the TBaLogInfo struct must not be changed or
 *  mangled with. They point to internal memory. Also they invalidated when
 *  the logger instance is destroyed. Accessing the freed memory results in
 *  undefined behavior.
 */
void BaLogGetLogInfo(
      TBaLogHdl hdl,  ///< [in] Handle
      TBaLogInfo *pInfo ///< [out] Pointer to preallocated structure
      );
//@}

/***************************************************************************/
/** Forces flush to disk
 */
void BaLogFlush(
      TBaLogHdl hdl ///< [in] Handle
      );

/// @name Syslog logging
//@{
/******************************************************************************/
/** Logs a message into the syslog like @c printf() and adds a @c tag and a
 *  time stamp. The syslog should be used to log critical errors, or when a
 *  normal logger fails. The @c line parameter is for debugging. It is
 *  recommended to use the #BASYSLOG macro in order to use the line information
 *  correctly
 */
void BaLogSysLogF(
      const char *tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      int line,        ///< [in] Line number, useful for debugging
      const char *fmt, ///< [in] Message format
      ...              ///< [in] Format arguments
      );

/******************************************************************************/
/** Logs a message into the syslog and adds a @c tag and a time stamp.
 *  The syslog should be used to log critical errors, or when a
 *  normal logger fails. The @c line parameter is for debugging. It is
 *  recommended to use the #BASYSLOG macro in order to use the line information
 *  correctly
 */
void BaLogSysLog(
      const char *tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      int line,        ///< [in] Line number, useful for debugging
      const char *msg  ///< [in] Message to log
      );
//@}

#ifdef __cplusplus
} // extern c

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

/******************************************************************************/
/** Logger C++ interface
 */
class IBaLog {
public:

   /// @name Getter
   //@{
   /***************************************************************************/
   /** Gets the information and options.
    *  The const char pointers of the TBaLogInfo struct must not be changed or
    *  mangled with. They point to internal memory. Also they invalidated when
    *  the logger instance is destroyed. Accessing the freed memory results in
    *  undefined behavior.
    */
   virtual void GetLogInfo(
         TBaLogInfo *pInfo ///< [out] Pointer to preallocated structure
         ) = 0;
   //@}

   /// @name Logging functions
   //@{
   /***************************************************************************/
   /** Logs a message into the logger and adds a @c tag and a time stamp.
    *  The @c Trace() and family functions have an implied priority
    *  @return true if success, otherwise, false. False if buffer is full or
    *  @c msg is null
    */
   virtual bool Log(
         EBaLogPrio  prio, ///< [in] Message priority
         const char* tag,  ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char* msg   ///< [in] Message to log
         ) = 0;
   virtual bool Trace(
         const char* tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char* msg  ///< [in] Message to log
         ) = 0;
   virtual bool Warning(
         const char* tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char* msg  ///< [in] Message to log
         ) = 0;
   virtual bool Error(
         const char* tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char* msg  ///< [in] Message to log
         ) = 0;
   //@}

   /// @name Logging functions with format
   //@{
   /***************************************************************************/
   /** Logs a message into the logger like @c printf() and adds a @c tag and a
    *  time stamp. These functions have a limit of 65534 characters per message.
    *  The @c TraceF() and family functions have an implied priority
    *  @return true if success, otherwise, false. False if buffer is full or
    *  @c fmt is null
    */
   virtual bool LogF(
         EBaLogPrio  prio, ///< [in] Message priority
         const char* tag,  ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char* fmt,  ///< [in] Message format
         ...               ///< [in] Format arguments
         ) = 0;
   virtual bool TraceF(
         const char* tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char* fmt, ///< [in] Message format
         ...              ///< [in] Format arguments
         ) = 0;
   virtual bool WarningF(
         const char* tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char* fmt, ///< [in] Message format
         ...              ///< [in] Format arguments
         ) = 0;
   virtual bool ErrorF(
         const char* tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char* fmt, ///< [in] Message format
         ...              ///< [in] Format arguments
         ) = 0;
   //@}

   /***************************************************************************/
   /** Forces a flush to disk
    */
   virtual void Flush() = 0;

   // In interfaces and abstract classes, ALWAYS declare a virtual destructor,
   // and implement / inline it
   virtual ~IBaLog() {};
};

/// @name C++ Factory
//@{
/******************************************************************************/
/** Create factory for a logger with defaults
 *  @return Handle if success, otherwise, null
 */
extern "C" IBaLog * IBaLogCreateDef(
      const char *name ///< [in] Name of the logger
      );

/******************************************************************************/
/** Create factory for a logger.
 *  @return Handle if success, otherwise, null
 */
extern "C" IBaLog * IBaLogCreate(
      const TBaLogOptions *pOpts ///< [in] Logger options
      );

/******************************************************************************/
/** Destroy and release resources of logger
 *  @return True if success, otherwise, false
 */
extern "C" TBaBoolRC IBaLogDestroy(
      IBaLog *pHdl, ///< [in] BaLog handle to destroy
      TBaBool saveCfg ///< [in] Flag to specify if the state should be saved in a cfg file
      );
//@}

#endif // __cplusplus
#endif // BALOG_H_
