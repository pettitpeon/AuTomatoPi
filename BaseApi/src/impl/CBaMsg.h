/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CBaMsg.h
 *   Date     : Apr 26, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/*  @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef CBAMSG_H_
#define CBAMSG_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "BaMsg.h"
#include "BaLog.h"

#define _L_SETSYSLOGF(tag, fmt, ...) SetSysLogF(tag, __LINE__, fmt, __VA_ARGS__)

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
class CBaMsg : public IBaMsg {
public:

   virtual void SetPrintF(
         const char *fmt,
         ...
         );

   virtual void SetPrint(
         const char *msg
         );

   virtual void SetSysLogF(
         const char *tag,
         int line,
         const char *fmt,
         ...
         );

   virtual void SetSysLog(
         const char *tag,
         int line,
         const char *msg
         );

   virtual void SetLogF(
         IBaLog* pLog,
         EBaLogPrio prio,
         const char *tag,
         const char *fmt,
         ...
         );

   virtual void SetLog(
         IBaLog* pLog,
         EBaLogPrio prio,
         const char *tag,
         const char *msg
         );

   virtual void SetDefLog(
         EBaLogPrio prio, ///< [in] Message priority
         const char *tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char *msg  ///< [in] Message
         );

   virtual void SetDefLogF(
         EBaLogPrio prio, ///< [in] Message priority
         const char *tag, ///< [in] Optional tag of maximum 6 chars + 7th terminating null
         const char *fmt,
         ...
         );

   virtual void Reset() { mSet = false; };
   virtual bool Get() { return mSet; };

   CBaMsg() : mSet(false) {};

   // Typical object oriented destructor must be virtual!
   virtual ~CBaMsg() {};

private:

   CBaMsg(bool set) : mSet(set) {};

   // Make this object non-copyable.
   // This is important because the implementation is in a dynamic pointer and
   // a deep copy of the implementation is explicitly required
   CBaMsg(const CBaMsg&);
   CBaMsg& operator=(const CBaMsg&);

   bool mSet;
};

#endif // __cplusplus
#endif // CBAMSG_H_
