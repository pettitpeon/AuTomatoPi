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


// FIXME: determine the tag size

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

/*------------------------------------------------------------------------------
 *  C interface
 */
#ifdef __cplusplus
extern "C" {
#endif

void BaLogInit();


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
         const char* fmt,  ///< [in] Message to log
         ...
         ) = 0;
   virtual bool TraceF(
         const char* tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char* fmt, ///< [in] Message to log
         ...
         ) = 0;
   virtual bool WarningF(
         const char* tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char* fmt, ///< [in] Message to log
         ...
         ) = 0;
   virtual bool ErrorF(
         const char* tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char* fmt, ///< [in] Message to log
         ...
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
      const char *name, ///< [in] Name of the logger
      const char *path, ///< [in] Directory path for saving the logger
      EBaLogPrio  prioFilt, ///< [in] Priority filter
      EBaLogOut   out, ///< [in] Output specifier
      uint32_t    maxFileSizeB, ///< [in] Maximum file size in bytes
      uint16_t    maxNoFiles, ///< [in] Maximum number of files
      uint16_t    maxBufLength ///< [in] Maximum number of messages in the buffer
      );

/******************************************************************************/
/** Destroy and release resources of logger
 *  @return True if success, otherwise, false
 */
extern "C" bool CBaLogDestroy(
      IBaLog *pHdl, ///< [in] BaLog handle to destroy
      bool saveCfg ///< [in] Flag to specify if the state should be saved in a cfg file
      );

#endif // __cplusplus
#endif // BALOG_H_
