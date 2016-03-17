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

#include "BaLog.h"

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
         EBaLogPrio  prioFilt = eBaLogPrio_UpsCrash,
         bool        toConsole = true,
         uint32_t    maxFileSizeB = 1048576,
         uint16_t    maxNoFiles   = 3,
         uint16_t    maxBufLength = 0
         );

   // Factory from config
   static CBaLog* CreateFromCfg(
         std::string cfgFile
         );

   //
   static bool Delete (
         CBaLog* hdl,
         bool saveCfg = false
         );

   // Logging functions
   virtual bool Log(EBaLogPrio prio, const char* tag, const char* msg);
   virtual bool Trace(const char* tag, const char* msg);
   virtual bool Warning(const char* tag, const char* msg);
   virtual bool Error(const char* tag, const char* msg);

   virtual bool LogF(EBaLogPrio prio, const char* tag, const char* fmt, ...);
   virtual bool TraceF(const char* tag, const char* fmt, ...);
   virtual bool WarningF(const char* tag, const char* fmt, ...);
   virtual bool ErrorF(const char* tag, const char* fmt, ...);

   // Not part of the interface
   bool saveCfg();

private:
   static CBaLog* commonCreate(
         std::string name, std::string path, EBaLogPrio prioFilt, bool toConsole,
         int32_t maxFileSizeB, uint16_t maxNoFiles, uint16_t maxBufLength,
         uint16_t fileCnt, int32_t fileSizeB, bool fromCfg = false);

   static bool init();
   static bool exit();
   static void logRoutine(
         TBaCoreThreadArg *pArg
         );

   void flush2Disk();
   bool log(EBaLogPrio prio, const char* tag, const char* msg);
   bool logV(EBaLogPrio prio, const char* tag, const char* fmt, va_list arg);

   // Private constructor because a public factory method is used
   CBaLog(std::string name, std::string path, EBaLogPrio prioFilt, bool toConsole,
         int32_t maxFileSizeB, uint16_t maxNoFiles, uint16_t maxBufLength,
         uint16_t fileCnt, int32_t fileSizeB) :
      mName(name), mPath(path), mPrioFilt(prioFilt), mToConsole(toConsole),
      mMaxFileSizeB(maxFileSizeB), mMaxNoFiles(maxNoFiles), mMaxBufLength(maxBufLength),
      mFileCnt(fileCnt), mFileSizeB(fileSizeB), mOpenCnt(1), mFullPath(),
      mTmpPath(), mLog(), mBuf(), mCameFromCfg(false), mMtx() {};

   // Typical object oriented destructor must be virtual!
   virtual ~CBaLog() {};

   // Make this object non-copyable.
   // This is important because the implementation is in a dynamic pointer and
   // a deep copy of the implementation is explicitly required
   CBaLog(const CBaLog&);
   CBaLog& operator=(const CBaLog&);

   // Configuration parameters
   const std::string mName; // name of the log
   std::string       mPath; // Path to the log
   EBaLogPrio        mPrioFilt;
   bool              mToConsole;
   const uint32_t mMaxFileSizeB; // File size limit in bytes
   const uint16_t mMaxNoFiles; // Maximum no. of history files
   const uint16_t mMaxBufLength; // Max. no. of messages in the buffer

   // Things to keep track of
   uint16_t mFileCnt; // Actual file count
   uint32_t mFileSizeB; // Actual estimated file size in bytes

   // Internal temporary variables
   uint16_t mOpenCnt; // No. of times the file was opened
   std:: string mFullPath; // name of the new file // TODO describe it correctly
   std:: string mTmpPath; // name of the new file // TODO describe it correctly
   std::ofstream mLog; // file stream
   std::vector<std::string> mBuf; // Message queue
   bool mCameFromCfg;
   std::mutex mMtx;
   char mMillis[4];
   char mTag[7];

};


#endif // CBALOG_H_
