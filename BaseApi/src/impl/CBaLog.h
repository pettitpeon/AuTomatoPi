/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CBaLog.h
 *   Date     : Dec 9, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/*  @file
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
#include <mutex>
#include <vector>
#include <fstream> // std::ofstream

#include "BaLog.h"
#include "CBaMsg.h"

struct TBaCoreThreadArg;

/*------------------------------------------------------------------------------
 *  Type definitions
 */

/*------------------------------------------------------------------------------
 *  C++ Interface
 */
class CBaLog : public IBaLog {
public:

   // Factory customized
   static CBaLog* Create(
         std::string name,
         std::string path = "",
         EBaLogPrio  prioFilt = eBaLogPrio_Trace,
         EBaLogOut   out = eBaLogOut_LogAndConsole,
         uint32_t    maxFileSizeB = 1048576, // 1 MiB
         uint16_t    maxNoFiles   = 3, // max No of extra files
         uint16_t    maxBufLength = 0,
         bool disableThread = false // Disable the internal worker thread to
         // manually flush the loggerS
         );

   static CBaLog* Create(
         const TBaLogOptions &rOpts,
         bool disableThread = false // Disable the internal worker thread to
         // manually flush the loggerS
         );

   // Factory from config
   static CBaLog* CreateFromCfg(
         std::string cfgFile,
         bool disableThread = false // Disable the internal worker thread to
         // manually flush the loggerS
         );

   //
   static bool Destroy (
         IBaLog* hdl,
         bool saveCfg = false
         );

   //
   static void SysLog(
         const char* tag,
         int line,
         const char* msg
         );

   // Info function
   virtual void GetLogInfo(TBaLogInfo *pInfo);

   // Logging functions
   virtual bool Log(EBaLogPrio prio, const char* tag, const char* msg);
   virtual bool Trace(const char* tag, const char* msg);
   virtual bool Warning(const char* tag, const char* msg);
   virtual bool Error(const char* tag, const char* msg);

   virtual bool LogF(EBaLogPrio prio, const char* tag, const char* fmt, ...);
   virtual bool TraceF(const char* tag, const char* fmt, ...);
   virtual bool WarningF(const char* tag, const char* fmt, ...);
   virtual bool ErrorF(const char* tag, const char* fmt, ...);

   //
   virtual void Flush();

   // Not part of the interface
   bool saveCfg();
   bool logV(EBaLogPrio prio, const char* tag, const char* fmt, va_list arg);

private:
   static CBaLog* commonCreate(
         std::string name, std::string path, EBaLogPrio prioFilt, EBaLogOut out,
         int32_t maxFileSizeB, uint16_t maxNoFiles, uint16_t maxBufLength,
         uint16_t fileCnt, int32_t fileSizeB, bool fromCfg, bool disableThread);

   static bool init(bool disableThreadDebug);
   static bool exit();
   static void logRoutine(
         TBaCoreThreadArg *pArg
         );

   static void getCfgPath(std::string &rNamePath);

   bool log(EBaLogPrio prio, const char* tag, const char* msg);

   // Private constructor because a public factory method is used
   CBaLog(std::string name, std::string path, EBaLogPrio prioFilt, EBaLogOut out,
         int32_t maxFileSizeB, uint16_t maxNoFiles, uint16_t maxBufLength,
         uint16_t fileCnt, int32_t fileSizeB) :
      mName(name), mPath(path), mPrioFilt(prioFilt), mOut(out),
      mMaxFileSizeB(maxFileSizeB), mMaxNoFiles(maxNoFiles), mMaxBufLength(maxBufLength),
      mFileCnt(fileCnt), mFileSizeB(fileSizeB), mOpenCnt(1), mFullPath(),
      mTmpPath(), mLog(), mBuf(), mCameFromCfg(false), mMtx(), mMsg() {};

   // Typical object oriented destructor must be virtual!
   virtual ~CBaLog() {};

   // Make this object non-copyable.
   // This is important because the implementation is in a dynamic pointer and
   // a deep copy of the implementation is explicitly required
   CBaLog(const CBaLog&);
   CBaLog& operator=(const CBaLog&);

   // Configuration parameters
   // todo: make them dynamic
   const std::string mName; // Name of the log
   const std::string mPath; // Path to the log
   const EBaLogPrio mPrioFilt; // Priority filter, allows messages equal of higher
   const EBaLogOut mOut; // Output form specifier
   const uint32_t mMaxFileSizeB; // File size limit in bytes
   const uint16_t mMaxNoFiles; // Maximum no. of extra history files
   const uint16_t mMaxBufLength; // Max. no. of messages in the buffer

   // Things to keep track of
   uint16_t mFileCnt; // Actual file count
   uint32_t mFileSizeB; // Actual estimated file size in bytes

   // Internal temporary variables
   uint16_t mOpenCnt; // No. of times the file was opened
   std:: string mFullPath; // Full path, concatenation of path, name and extension
   std:: string mTmpPath; // Temp name of the new file
   std::ofstream mLog; // file stream
   std::vector<std::string> mBuf; // Message queue
   bool mCameFromCfg; // Flag to remember if it was opened from a cfg file
   std::mutex mMtx; // Mutex to avoid simultaneous read and write of the buffer
   char mMillis[4]; // Temp variable to save the milli part of a time-stamp
   char mTag[7]; // Temp variable to manipulate the tag and pad spaces

   CBaMsg mMsg;

};


#endif // CBALOG_H_
