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
 *  ...
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
/// Syslog macro that includes the line number
#define BASYSLOG(tag, fmt, ...) BaLogSysLog(tag, __LINE__, fmt, __VA_ARGS__)

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

/******************************************************************************/
/** ...
 */
void BaLogSysLog(
      const char *tag,
      int line,
      const char *fmt,
      ...
      );

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
/** Destroy and release resources of logger
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaLogDestroy(
      TBaLogHdl hdl, ///< [in] BaLog handle to destroy
      TBaBool saveCfg ///< [in] Flag to specify if the state should be saved in a cfg file
      );

/// @name Logging functions
//@{
/******************************************************************************/
/** Logs a message into the logger and adds a @c tag and a time stamp.
 *  The @c Trace() and rest functions have an implied priority
 *  @return true if success, otherwise, false
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
 *  The @c TraceF() and rest functions have an implied priority
 *  @return true if success, otherwise, false
 */
TBaBoolRC BaLogLogF(
      TBaLogHdl *pHdl,  ///< [in] Handle
      EBaLogPrio  prio, ///< [in] Message priority
      const char* tag,  ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      const char* fmt,  ///< [in] Message format
      ...               ///> [in] Format arguments
      );

TBaBoolRC BaLogTraceF(
      TBaLogHdl *pHdl,  ///< [in] Handle
      const char* tag,  ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      const char* fmt,  ///< [in] Message format
      ...               ///> [in] Format arguments
      );

TBaBoolRC BaLogWarningF(
      TBaLogHdl *pHdl,  ///< [in] Handle
      const char* tag,  ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      const char* fmt,  ///< [in] Message format
      ...               ///> [in] Format arguments
      );


TBaBoolRC BaLogErrorF(
      TBaLogHdl *pHdl,  ///< [in] Handle
      const char* tag,  ///< [in] Optional tag of maximum 6 chars + 7th terminating null
      const char* fmt,  ///< [in] Message format
      ...               ///> [in] Format arguments
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

   virtual void Flush() = 0;

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
    *  The @c Trace() and rest functions have an implied priority
    *  @return true if success, otherwise, false
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
    *  The @c TraceF() and rest functions have an implied priority
    *  @return true if success, otherwise, false
    */
   virtual bool LogF(
         EBaLogPrio  prio, ///< [in] Message priority
         const char* tag,  ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char* fmt,  ///< [in] Message format
         ...               ///> [in] Format arguments
         ) = 0;
   virtual bool TraceF(
         const char* tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char* fmt,  ///< [in] Message format
         ...               ///> [in] Format arguments
         ) = 0;
   virtual bool WarningF(
         const char* tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char* fmt,  ///< [in] Message format
         ...               ///> [in] Format arguments
         ) = 0;
   virtual bool ErrorF(
         const char* tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char* fmt,  ///< [in] Message format
         ...               ///> [in] Format arguments
         ) = 0;
   //@}

   // In interfaces and abstract classes, ALWAYS declare a virtual destructor,
   // and implement / inline it
   virtual ~IBaLog() {};
};

/******************************************************************************/
/** Create factory for a logger with defaults
 *  @return Handle if success, otherwise, null
 */
extern "C" IBaLog * CBaLogCreateDef(
      const char *name ///< [in] Name of the logger
      );

/******************************************************************************/
/** Create factory for a logger.
 *  @return Handle if success, otherwise, null
 */
extern "C" IBaLog * CBaLogCreate(
      const TBaLogOptions *pOpts ///< [in] Logger options
      );

/******************************************************************************/
/** Destroy and release resources of logger
 *  @return True if success, otherwise, false
 */
extern "C" TBaBoolRC CBaLogDestroy(
      IBaLog *pHdl, ///< [in] BaLog handle to destroy
      TBaBool saveCfg ///< [in] Flag to specify if the state should be saved in a cfg file
      );

#endif // __cplusplus
#endif // BALOG_H_
