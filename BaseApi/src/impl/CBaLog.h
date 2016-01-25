/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CBaLog.h
 *   Date     : Dec 9, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef CBALOG_H_
#define CBALOG_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include <string>

struct TBaCoreThreadArg;

/*------------------------------------------------------------------------------
 *  Type definitions
 */

/*------------------------------------------------------------------------------
 *  C++ Interface
 */
class CBaLog {
public:
   // Factory
   static CBaLog* Create(
         std::string name
         );

   static bool Delete (
         CBaLog* hdl
         );

   // Hardware PWM setup functions
   virtual bool Log(const char* msg);
   virtual void Logf(const char* fmt, ...);

private:
   static bool init();
   static bool exit();
   static void logRoutine(
         TBaCoreThreadArg *pArg
         );

   void flush2Disk();


   // Private constructor because a public factory method is used
   CBaLog(std::string name) : mName(name), mTmpName(), mLog(), mOpenCnt(1), mFileSizeB(0), mBuf(),
         mMaxFileSizeB(45), mMaxNoFiles(2), mFileCnt(0) {};

   // Typical object oriented destructor must be virtual!
   virtual ~CBaLog() {};

   // Make this object non-copyable.
   // This is important because the implementation is in a dynamic pointer and
   // a deep copy of the implementation is explicitly required
   CBaLog(const CBaLog&);
   CBaLog& operator=(const CBaLog&);

   std::string mName; // name of the log
   std:: string mTmpName; // name of the new file // TODO describe it correctly
   std::ofstream mLog; // file stream
   int16_t mOpenCnt; // No. of times the file was opened
   uint32_t mFileSizeB; // Actual estimated file size in bytes
   std::vector<std::string> mBuf; // Message queue
   uint32_t mMaxFileSizeB; // File size limit in bytes
   uint16_t mMaxNoFiles; // Maximum no. of history files
   uint16_t mFileCnt; // Actual file count

};


#endif // CBALOG_H_
