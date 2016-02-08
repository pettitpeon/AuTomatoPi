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
   // Factory with defaults
//   static CBaLog* Create(
//         std::string name
//         );

   // Factory customized
   static CBaLog* Create(
         std::string name,
         uint32_t maxFileSizeB = 1048576,
         uint16_t maxNoFiles   = 3,
         uint16_t maxBufLength = 0
         );

   // Factory from config
   static CBaLog* CreateFromCfg(
         std::string cfgFile
         );

   //
   static bool Delete (
         CBaLog* hdl,
         bool saveCfg = true
         );

   // Hardware PWM setup functions
   virtual bool Log(const char* msg);
   virtual void Logf(const char* fmt, ...);

   // Not part of the interface
   bool saveCfg();

private:
   static bool init();
   static bool exit();
   static void logRoutine(
         TBaCoreThreadArg *pArg
         );

   void flush2Disk();



   // Private constructor because a public factory method is used
   CBaLog(std::string name, uint32_t maxFileSizeB, uint16_t maxNoFiles,
         uint16_t maxBufLength, uint16_t fileCnt, uint16_t openCnt,
         uint16_t fileSizeB) :
      mName(name), mMaxFileSizeB(maxFileSizeB), mMaxNoFiles(maxNoFiles),
      mMaxBufLength(maxBufLength),mFileCnt(fileCnt), mOpenCnt(openCnt),
      mFileSizeB(fileSizeB), mTmpName(),mLog(), mBuf() {};

   // Typical object oriented destructor must be virtual!
   virtual ~CBaLog() {};

   // Make this object non-copyable.
   // This is important because the implementation is in a dynamic pointer and
   // a deep copy of the implementation is explicitly required
   CBaLog(const CBaLog&);
   CBaLog& operator=(const CBaLog&);

   // Configuration parameters
   const std::string mName; // name of the log
   const uint32_t mMaxFileSizeB; // File size limit in bytes
   const uint16_t mMaxNoFiles; // Maximum no. of history files
   const uint16_t mMaxBufLength; // Max. no. of messages in the buffer

   // Things to keep track of
   uint16_t mFileCnt; // Actual file count
   uint16_t mOpenCnt; // No. of times the file was opened
   uint32_t mFileSizeB; // Actual estimated file size in bytes

   // Internal temporary variables
   std:: string mTmpName; // name of the new file // TODO describe it correctly
   std::ofstream mLog; // file stream
   std::vector<std::string> mBuf; // Message queue


};


#endif // CBALOG_H_
