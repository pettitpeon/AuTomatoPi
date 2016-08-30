/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CBalog.cpp
 *   Date     : Dec 9, 2015
 *------------------------------------------------------------------------------
 */


/*------------------------------------------------------------------------------
    C Includes
 -----------------------------------------------------------------------------*/
#include <stdarg.h>
#include <time.h>
//#include <sys/types.h>
#include <errno.h>
#ifdef __linux
# include <syslog.h>
#endif


/*------------------------------------------------------------------------------
    C++ Includes
 -----------------------------------------------------------------------------*/
#include <map>
#include <iostream>
#include <ctime>

/*------------------------------------------------------------------------------
    Local Includes
 -----------------------------------------------------------------------------*/
#include "BaUtils.hpp"
#include "CBaLog.h"
#include "../BaGenMacros.h"
#include "BaCore.h"
#include "BaIniParse.h"
#include "dbg/BaDbgMacros.h"
#include <BaTmpTime.hpp>

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#ifdef __linux
# define LOGDIR "/var/log/"
# define CFGDIR "/var/log/config/"
#else
# define LOGDIR "C:\\log\\"
# define CFGDIR "C:\\log\\config\\"
#endif

#define LOGEXT    ".log"
#define CFGEXT    ".cfg"
#define CHRONOHRC_ std::chrono::high_resolution_clock
#define CHRONO    std::chrono
#define TAG       "BaLog"
#define TAGSZ     7
#define FULLPATH(PATH, NAME)  PATH + NAME + LOGEXT
#define STRNOTFOUND    std::string::npos     // Return value for string not found
#define SYSLOG_        _L_SETSYSLOGF
#define FOREVER        200 // 200ms

/*------------------------------------------------------------------------------
    Static variables
 -----------------------------------------------------------------------------*/
static std::map<std::string, CBaLog*> sLoggers;
static TBaCoreThreadHdl sLogdHdl = 0;
static TBaCoreThreadArg sLogdArg = {0};
static std::mutex sMtx;
static std::string sPrioTochar[eBaLogPrio_UpsCrash + 1] = {"T", "W", "E", "C"};

/*------------------------------------------------------------------------------
    Local Declarations
 -----------------------------------------------------------------------------*/

LOCAL void reTag(const char* tagIn, char* tagOut);

/*------------------------------------------------------------------------------
    Implementation
 -----------------------------------------------------------------------------*/

//
void CBaLog::logRoutine(TBaCoreThreadArg *pArg) {
   while (!sLogdArg.exitTh) {
      { // RAII Scope
         std::lock_guard<std::mutex> lck(sMtx);

         // iterate loggers
         for (auto &kv : sLoggers) {
            kv.second->Flush();
         }
      }

      BaCoreMSleep(50);
   }
}

//
void CBaLog::getCfgPath(std::string &rNamePath) {

   // If only name (without extension), add the default path and extension
   if (rNamePath.find('.', 0) == STRNOTFOUND) {
      rNamePath = CFGDIR + rNamePath + CFGEXT;
   }
}

//
bool CBaLog::init(bool disableThread) {
   if (sLogdHdl) {
      return true;
   }

   // Check if dir exists
   if (!BaFS::Exists(LOGDIR)) {
      // Create directory
      if (BaFS::MkDir(LOGDIR) != 0) {
         BASYSLOG(TAG, "Could not create dir: %s", LOGDIR);
         return false;
      }
   }

   sLogdArg.exitTh = false;
   if (disableThread) {
      sLogdHdl = BaCoreCreateThread("BaLogD", logRoutine, &sLogdArg, eBaCorePrio_Highest);
   } else {
      sLogdHdl = (void*)-1;
   }
   return sLogdHdl;
}

//
bool CBaLog::exit() {
   if (!sLogdHdl) {
      return true;
   }

   bool rc = true;

   if (sLogdHdl != (void*)-1) {
      rc = BaCoreDestroyThread(sLogdHdl, FOREVER);
   }
   sLogdHdl = 0;
   return rc;
}

//
CBaLog* CBaLog::commonCreate(std::string name, std::string path, EBaLogPrio prioFilt,
      EBaLogOut out, int32_t maxFileSizeB, uint16_t maxNoFiles, uint16_t maxBufLength,
      uint16_t fileCnt, int32_t fileSizeB, bool fromCfg, bool disableThread)  {

   if (name.empty() || !init(disableThread)) {
      return 0;
   }

   // Check if already exists
   auto logger = sLoggers.find(name);
   if (logger != sLoggers.end()) {
      logger->second->mOpenCnt++;
      return logger->second;
   }

   // Set default
   if (path == "") {
      path = LOGDIR;
   }

   // Limit the priorities
   prioFilt = MINMAX(prioFilt, eBaLogPrio_Trace, eBaLogPrio_UpsCrash);

   // //////////////// Create //////////////
   // If windows, remove the trailing '\' because it generates problems with the
   // when the path is saved with the IniParser
#ifdef __WIN32
   if (path.back() == '\\') {
      path.resize(path.length() - 1);
   }
#endif

   CBaLog *p = new CBaLog(name, path, prioFilt, out, maxFileSizeB, maxNoFiles, maxBufLength, fileCnt,
         fileSizeB);
   if (!p) {
      SysLog(TAG, __LINE__, "Could not create logger");
      return 0;
   }
   // //////////////// Create //////////////

   p->mFullPath = BaPath::Concatenate(p->mPath, p->mName) + ".log";
   std::ios_base::openmode om = std::ios_base::binary | std::ios_base::out;
   if (fromCfg) {
      om |= std::ios_base::app;
   }

   // //////////////// Open ////////////////
   p->mLog.open(p->mFullPath, om);
   if (p->mLog.fail()) {
      BASYSLOG(TAG, "Cannot open log file: %s", p->mFullPath.c_str());
      delete p;
      return 0;
   }
   // //////////////// Open ////////////////

   sLoggers[p->mName] = p;
   return p;
}

//
CBaLog* CBaLog::Create(std::string name, std::string path, EBaLogPrio  prioFilt,
      EBaLogOut out, uint32_t maxFileSizeB, uint16_t maxNoFiles,
      uint16_t maxBufLength, bool disableThread) {
   std::lock_guard<std::mutex> lck(sMtx);

   return commonCreate(name, path, prioFilt, out, maxFileSizeB, maxNoFiles,
         maxBufLength, 0, 0, false, disableThread);
}

//
CBaLog* CBaLog::Create(const TBaLogOptions &rOpts, bool disableThread) {
   std::lock_guard<std::mutex> lck(sMtx);

   return commonCreate(rOpts.name, rOpts.path, rOpts.prioFilt, rOpts.out,
         rOpts.maxFileSizeB, rOpts.maxNoFiles, rOpts.maxBufLength, 0, 0, false,
         disableThread);
}

//
CBaLog* CBaLog::CreateFromCfg(std::string cfgFile, bool disableThread) {

   // Get the real path
   getCfgPath(cfgFile);

   IBaIniParser *pIni = IBaIniParserCreate(cfgFile.c_str());
   if (!pIni) {
      // todo: log? Ini parser should log
      return 0;
   }

   std::string name = pIni->GetString(TAG ":mName", "");
   std::string path = pIni->GetString(TAG ":mPath", "");
   EBaLogPrio prioFilt = (EBaLogPrio) pIni->GetInt(TAG ":mPrioFilt", eBaLogPrio_UpsCrash);
   EBaLogOut out = (EBaLogOut) pIni->GetInt(TAG ":mOut", eBaLogOut_LogAndConsole);
   uint32_t maxFileSizeB = (uint32_t) pIni->GetInt(TAG ":mMaxFileSizeB", -1);
   int32_t  maxNoFiles = pIni->GetInt(TAG ":mMaxNoFiles", -1);
   uint32_t maxBufLength = (uint32_t) pIni->GetInt(TAG ":mMaxBufLength", -1);
   uint32_t fileSizeB = (uint32_t) pIni->GetInt(TAG ":mFileSizeB", -1);
   uint16_t fileCnt = (uint16_t) pIni->GetInt(TAG ":mFileCnt", -1);

   if (name == "" || path == "" || maxFileSizeB == (uint32_t) -1 ||
       maxNoFiles == -1 || maxBufLength == (uint32_t) -1 ||
       fileCnt == (uint16_t) -1) {
      BASYSLOG(TAG, "Invalid config file: %s", cfgFile.c_str());
      return 0;
   }

   std::lock_guard<std::mutex> lck(sMtx);

   return commonCreate(name, path, prioFilt, out, maxFileSizeB, maxNoFiles, maxBufLength,
         fileCnt, fileSizeB, true, disableThread);
}

//
bool CBaLog::Destroy(IBaLog *pHdl, bool saveCfg) {
   std::lock_guard<std::mutex> lck(sMtx);

   CBaLog *p = dynamic_cast<CBaLog*>(pHdl);
   if (!p ) {
      return false;
   }

   if (--p->mOpenCnt == 0) {
      // Erase from loggers
      sLoggers.erase(p->mName);
      p->Flush();
      p->mLog.close();

      // Save state
      if (saveCfg) {
         p->saveCfg();
      }

      delete p;
   }

   if (sLoggers.empty()) {
      return exit();
   }

   return true;
}

//
void inline CBaLog::SysLog(const char *tag, int line, const char *msg) {

   char tagOut[TAGSZ];
   reTag(tag, tagOut);
#ifdef __linux
   syslog(LOG_ERR, "%s(%d): %s", tagOut, line, msg);
   closelog();
#else
   printf("%s(%d): %s\n", tagOut, line, msg);
#endif
}

//
void CBaLog::GetLogInfo(TBaLogInfo *pInfo) {
   if (!pInfo) {
      return;
   }

   pInfo->name = mName.c_str();
   pInfo->fullPath = mFullPath.c_str();
   pInfo->prioFilt = mPrioFilt;
   pInfo->out = mOut;
   pInfo->maxFileSizeB = mMaxFileSizeB;
   pInfo->maxNoFiles = mMaxNoFiles;
   pInfo->maxBufLength = mMaxBufLength;
   pInfo->fileSizeB = mFileSizeB;
}

//
bool inline CBaLog::Log(EBaLogPrio prio, const char* tag, const char* msg) {
   // This uses a member mutex so that msgs can be buffered
   // while another log is flushing
   std::lock_guard<std::mutex> lck(mMtx);
   return log(prio, tag, msg);
}

//
bool inline CBaLog::Trace(const char* tag, const char* msg){
   std::lock_guard<std::mutex> lck(mMtx);
   return log(eBaLogPrio_Trace, tag, msg);
}

//
bool inline CBaLog::Warning(const char* tag, const char* msg){
   std::lock_guard<std::mutex> lck(mMtx);
   return log(eBaLogPrio_Warning, tag, msg);
}

//
bool inline CBaLog::Error(const char* tag, const char* msg){
   std::lock_guard<std::mutex> lck(mMtx);
   return log(eBaLogPrio_Error, tag, msg);
}

//
bool inline CBaLog::LogF(EBaLogPrio prio, const char* tag, const char* fmt, ...) {
   va_list arg;
   va_start(arg, fmt);
   bool ret = LogV(prio, tag, fmt, arg);
   va_end(arg);
   return ret;
}

//
bool inline CBaLog::TraceF(const char* tag, const char* fmt, ...) {
   va_list arg;
   va_start(arg, fmt);
   bool ret = LogV(eBaLogPrio_Trace, tag, fmt, arg);
   va_end(arg);
   return ret;
}

//
bool inline CBaLog::WarningF(const char* tag, const char* fmt, ...) {
   va_list arg;
   va_start(arg, fmt);
   bool ret = LogV(eBaLogPrio_Warning, tag, fmt, arg);
   va_end(arg);
   return ret;
}

//
bool inline CBaLog::ErrorF(const char* tag, const char* fmt, ...) {
   va_list arg;
   va_start(arg, fmt);
   bool ret = LogV(eBaLogPrio_Error, tag, fmt, arg);
   va_end(arg);
   return ret;
}

//
inline void CBaLog::Flush() {


   // Avoid messing around with the buffer while it is being printed
   std::lock_guard<std::mutex> lck(mMtx);


   // Iterate messages in buffer
   for (auto &msg : mBuf) {

      // Check file size. If it is the first line, write it to disk anyways.
      // Every line has an extra LF character at the end.
      mFileSizeB += msg.size() + 1;
      if (msg.size() + 1 != mFileSizeB && mFileSizeB > mMaxFileSizeB) {

         // Open new file
         if (++mFileCnt > mMaxNoFiles) {
            mFileCnt = 1;
            // Rewrite file
         }

         // Close the output stream
         mLog.close();
         if (mLog.fail()) {
            mLogCloseFailed.SYSLOG_(TAG, "Cannot close log: %s", mFullPath.c_str());
         } else {
            mLogCloseFailed.Reset();
         }

         // Rename file
         if (mMaxNoFiles > 0) {
            mTmpPath = BaPath::ChangeFileExtension(mFullPath,
                  "_" + std::to_string(mFileCnt) + ".log");

            // this is only for debugging
//            std::cout << mFullPath << " to: " << mTmpPath << std::endl;

            if (BaFS::Rename(mFullPath.c_str(), mTmpPath.c_str()) == -1) {
               std::cout << errno << std::endl;
               mRenameFailed.SYSLOG_(TAG, "Cannot rename log: %s", mFullPath.c_str());
            } else {
               mRenameFailed.Reset();
            }

         }

         mFileSizeB = msg.size() + 1;
         mLog.open(mFullPath, std::ios_base::binary | std::ios_base::out);
         if (mLog.fail()) {
            mLogOpenFailed.SYSLOG_(TAG, "Cannot open log: %s", mFullPath.c_str());
         } else {
            mLogOpenFailed.Reset();
         }
      }

      // /////////// Log to disc ///////////////////////
      // No std::endl to avoid flushing too much
      mLog << msg << "\n";
//      std::cout << msg << std::endl; // For testing and debugging
      // ///////////////////////////////////////////////

   }

   mLog.flush();
   mBuf.clear();
}

//
bool CBaLog::saveCfg() {

   // Create file-less
   IBaIniParser *pIni = IBaIniParserCreate(0);

   // Set tag for section
   pIni->Set(TAG, "");

   // Set the rest of the values
   pIni->Set(TAG ":mName", mName.c_str());
   pIni->Set(TAG ":mPath", mPath.c_str());

   std::string s = std::to_string(mPrioFilt);
   pIni->Set(TAG ":mPrioFilt", s.c_str());

   s = std::to_string(mOut);
   pIni->Set(TAG ":mOut", s.c_str());

   s = std::to_string(mMaxFileSizeB);
   pIni->Set(TAG ":mMaxFileSizeB", s.c_str());

   s = std::to_string(mMaxNoFiles);
   pIni->Set(TAG ":mMaxNoFiles", s.c_str());

   s = std::to_string(mFileCnt);
   pIni->Set(TAG ":mFileCnt", s.c_str());

   s = std::to_string(mFileSizeB);
   pIni->Set(TAG ":mFileSizeB", s.c_str());

   s = std::to_string(mMaxBufLength);
   pIni->Set(TAG ":mMaxBufLength" ,s.c_str());


   // Check if dir exists
   struct stat info;
   if (stat(CFGDIR, &info) != 0) {
      // Create directory
      if (BaFS::MkDir(CFGDIR) != 0) {
         // not ok
         return false;
      }
   }

   // Generate path
   s = CFGDIR + mName + ".cfg";

   // Open file for saving cfg
   FILE *pF = fopen(s.c_str(), "w+");
   if (!pF) {
      return false;
   }

   pIni->DumpIni(pF);
   return fclose(pF) == 0;
}

//
bool CBaLog::log(EBaLogPrio prio, const char* tag, const char* msg) {
   if (!msg) {
      return false;
   }

   // Generate time stamp with priority
   // Get the prio
   prio = MINMAX(prio, eBaLogPrio_Trace, eBaLogPrio_UpsCrash);


   // Priority filter. Only log things equal or higher prio than the filter
   if (prio < mPrioFilt) {
      return true;
   }

   // Get the tag
   reTag(tag, mTag);

   // Put everything together
   BaCoreGetTStamp(&mTs);
   std::string entry(BaCoreTStampToStr(&mTs, mStrTStamp));
   entry.append( "|" + sPrioTochar[prio] + "|" + mTag + "| " + msg);


   // ////////////////// Write to buffer if it is not full /////////////////////
   if (mOut & eBaLogOut_Log) {
      if (!mMaxBufLength || mBuf.size() < mMaxBufLength) {
         mBuf.push_back(entry);
      } else {
         // todo: else? log error?
         return false;
      }
   }
   // //////////////////////////////////////////////////////////////////////////


   // Write to console if desired
   if (mOut & eBaLogOut_Console) {
      std::cout << entry << std::endl;
   }

   return true;
}

//
bool inline CBaLog::LogV(EBaLogPrio prio, const char* tag, const char* fmt, va_list arg) {
   if (!fmt) {
      return false;
   }

   // Get the required size + 1 for the ending null
   uint16_t size = vsnprintf(0, 0, fmt, arg) + 1;

   // Not using the FString function to be as quick as possible
   char msg[size];
   vsnprintf(msg, size, fmt, arg);
   std::lock_guard<std::mutex> lck(mMtx);
   return log(prio, tag, msg);
}

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
// Condition the tag to be TAGSZ and pad with spaces
LOCAL void inline reTag(const char* tagIn, char* tagOut) {
   uint8_t cnt = snprintf(tagOut, TAGSZ, "%s", tagIn ? tagIn : "");
   tagOut[TAGSZ - 1] = 0;
   if (cnt < TAGSZ - 1) {
      SETARR(tagOut + cnt, TAGSZ - 1 - cnt, ' ');
   }

}
