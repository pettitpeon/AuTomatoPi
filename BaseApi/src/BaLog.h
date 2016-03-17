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


   // Logging functions

   /***************************************************************************/
   /** Logs a message into the logger and adds a time stamp
    *  @return true if success, otherwise, false
    */
   virtual bool Log(
         EBaLogPrio  prio,
         const char* tag,
         const char* msg
         ) = 0;
   virtual bool Trace(
         const char* tag,
         const char* msg
         ) = 0;
   virtual bool Warning(
         const char* tag,
         const char* msg
         ) = 0;
   virtual bool Error(
         const char* tag,
         const char* msg
         ) = 0;

   virtual bool LogF(
         EBaLogPrio  prio,
         const char* tag,
         const char* fmt,
         ...
         ) = 0;
   virtual bool TraceF(
         const char* tag,
         const char* fmt,
         ...
         ) = 0;
   virtual bool WarningF(
         const char* tag,
         const char* fmt,
         ...
         ) = 0;
   virtual bool ErrorF(
         const char* tag,
         const char* fmt,
         ...
         ) = 0;

   // In interfaces and abstract classes, ALWAYS declare a virtual destructor,
   // and implement / inline it
   virtual ~IBaLog() {};
};

/******************************************************************************/
/** Create factory for an logger.
 *  @return Handle if success, otherwise, null
 */
extern "C" IBaLog * CBaLogCreate(
      const char *file ///< [in] Path to file
      );

/******************************************************************************/
/** Destroy and release resources of logger
 *  @return True if success, otherwise, false
 */
extern "C" bool CBaLogDestroy(
      IBaLog *pHdl ///< [in] INI parser handle to destroy
      );

#endif // __cplusplus
#endif // BALOG_H_
