/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <fstream> // std::ofstream
#include <map>
#include <vector>
#include <iostream>
#include <mutex>
#include <ctime>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "BaUtils.hpp"

#include "CBaLog.h"
#include "../BaGenMacros.h"
#include "BaCore.h"
#include "BaIniParse.h"

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
#define FASTSIZE  81
#define CHRONOHRC std::chrono::high_resolution_clock
#define CHRONO    std::chrono
#define TAG       "BaLog"

/*------------------------------------------------------------------------------
    Static variables
 -----------------------------------------------------------------------------*/
static std::map<std::string, CBaLog*> sLoggers;
static TBaCoreThreadHdl sLogdHdl = 0;
static TBaCoreThreadArg sLogdArg = {0};
static std::mutex sMtx;
static char sFasMsg[FASTSIZE];

/*------------------------------------------------------------------------------
    Local Declarations
 -----------------------------------------------------------------------------*/
namespace tmp_4_9_2 {
LOCAL tm localtime(const std::time_t& rTime);
LOCAL std::string put_time(const std::tm* pDateTime, const char* cTimeFormat);
}

//
void CBaLog::logRoutine(TBaCoreThreadArg *pArg) {
   while (!sLogdArg.exitTh) {
      { // RAII Scope
         std::lock_guard<std::mutex> lck(sMtx);

         // iterate loggers
         for (auto &kv : sLoggers) {
            kv.second->flush2Disk();
         }
      }

      BaCoreMSleep(50);
   }
}

//
bool CBaLog::init() {
   if (sLogdHdl) {
      return true;
   }
   sLogdArg.exitTh = false;

   sLogdHdl = BaCoreCreateThread("BaLogD", logRoutine, &sLogdArg, eBaCorePrio_High);
   return sLogdHdl;
}

//
bool CBaLog::exit() {
   if (!sLogdHdl) {
      return true;
   }

   BaCoreDestroyThread(sLogdHdl, 100);
   sLogdHdl = 0;
   return true;
}

//
CBaLog* CBaLog::commonCreate(std::string name, int32_t maxFileSizeB,
      uint16_t maxNoFiles, uint16_t maxBufLength, uint16_t fileCnt,
      int32_t fileSizeB, bool fromCfg)  {

   if (name.empty() || !init()) {
      // todo Log error
      return 0;
   }

   // Check if already exists
   auto logger = sLoggers.find(name);
   if (logger != sLoggers.end()) {
      logger->second->mOpenCnt++;
      return logger->second;
   }

   // //////////////// Create //////////////
   CBaLog *p = new CBaLog(name, maxFileSizeB, maxNoFiles, maxBufLength, fileCnt,
         fileSizeB);
   if (!p) {
      // todo Log error
      return 0;
   }
   // //////////////// Create //////////////

   p->mPath = LOGDIR + name + LOGEXT;
   p->mCameFromCfg = fromCfg;
   p->mLog.open(p->mPath, std::ios_base::binary | std::ios_base::out | std::ios_base::app);
   sLoggers[name] = p;
   return p;
}

//
CBaLog* CBaLog::Create(std::string name, uint32_t maxFileSizeB,
      uint16_t maxNoFiles, uint16_t maxBufLength) {
   std::lock_guard<std::mutex> lck(sMtx);

   return commonCreate(name, maxFileSizeB, maxNoFiles, maxBufLength, 1, 1, 0);
}

//
CBaLog* CBaLog::CreateFromCfg(std::string cfgFile) {

   IBaIniParser *pIni = CBaIniParserCreate(cfgFile.c_str());
   if (!pIni) {
      // todo: log?
      return 0;
   }

   std::string name = pIni->GetString(TAG ":mName","");
   std::string path = pIni->GetString(TAG ":mPath","");
   uint32_t maxFileSizeB = (uint32_t) pIni->GetInt(TAG ":mMaxFileSizeB", -1);
   int32_t  maxNoFiles = pIni->GetInt(TAG ":mMaxNoFiles", -1);
   uint32_t maxBufLength = (uint32_t) pIni->GetInt(TAG ":mMaxBufLength", -1);
   uint32_t fileSizeB = (uint32_t) pIni->GetInt(TAG ":mFileSizeB", -1);
   uint16_t fileCnt = (uint16_t) pIni->GetInt(TAG ":mFileCnt", -1);

   if (name == "" || path == "" || maxFileSizeB == (uint32_t) -1 ||
       maxNoFiles == -1 || maxBufLength == (uint32_t) -1 ||
       fileCnt == (uint16_t) -1) {
      // todo: log?
      return 0;
   }

   std::lock_guard<std::mutex> lck(sMtx);

   return commonCreate(name, maxFileSizeB, maxNoFiles, maxBufLength,
         fileCnt, fileSizeB, true);
}

//
bool CBaLog::Delete(CBaLog *pHdl, bool saveCfg) {
   std::lock_guard<std::mutex> lck(sMtx);

   CBaLog *p = dynamic_cast<CBaLog*>(pHdl);
   if (!p ) {
      return false;
   }

   if (--p->mOpenCnt == 0) {
      // Erase from loggers
      sLoggers.erase(p->mName);
      p->flush2Disk();
      p->mLog.close();

      // Save state
      if (saveCfg) {
         p->saveCfg();
      }

      delete p;
   }

   if (sLoggers.empty()) {
      exit();
   }

   return true;
}

//
bool CBaLog::Log(const char* msg) {

   // todo try to replace this with a member lock so that msgs can be buffered
   // while another log is flushing
   std::lock_guard<std::mutex> lck(sMtx);
   auto nowT = CHRONOHRC::now();

   std::time_t tt = CHRONOHRC::to_time_t(nowT);
   struct tm timem = tmp_4_9_2::localtime(tt);
   auto ms = CHRONO::duration_cast<CHRONO::milliseconds>(nowT.time_since_epoch());

   // Static is allowed because of the mutex
   static char milliS[4] = {0};
   snprintf(milliS, 4, "%03d", (int) (ms.count() % 1000));

   std::string entry = tmp_4_9_2::put_time(&timem, "%y/%m/%d %H:%M:%S") +
         "." + milliS + "| " + msg;

   // Write to buffer if it is not full
   if (!mMaxBufLength || mBuf.size() < mMaxBufLength) {
      mBuf.push_back(entry);
   }
   // todo: else? log error?

   std::cout << entry << std::endl;
   return true;
}

//
void CBaLog::Logf(const char* fmt, ...) {
   va_list arg;
   va_start(arg, fmt);
   uint16_t size = snprintf(0, 0, fmt, arg);

   // If the string is short, I do not have to create a variable on the stack.
   // I have a static one already available
   // todo: Check the reentrancy problem!!! I think the fast msg is not worth it
   if (size >= FASTSIZE) {
      char msg[size + 1];
      vsnprintf(msg, size, fmt, arg);
      Log(msg);
   } else {
      vsnprintf(sFasMsg, FASTSIZE, fmt, arg);
      Log(sFasMsg);
   }

}

//
inline void CBaLog::flush2Disk() {
   // Iterate messages in buffer
   for (auto &msg : mBuf) {

      // Check file size. If it is the first line written to the file,
      // write it to disk anyways.
      mFileSizeB += msg.size();
      if (msg.size() != mFileSizeB && mFileSizeB > mMaxFileSizeB) {
         // Open new file
         if (++mFileCnt > mMaxNoFiles) {
            mFileCnt = 1;
            // Rewrite file
         }
         mLog.close();
         if (mLog.fail()) {
            // todo: error
         }

         // Rename file
         mTmpPath = BaPath::ChangeFileExtension(mPath,
               "_" + std::to_string(mFileCnt) + ".log");

         std::cout << mPath << " to: " << mTmpPath << std::endl;

         // todo: windows does not let rewriting the file!! fix it to make it portable
#ifdef __WIN32
         remove(mTmpPath.c_str());
#endif
         if (rename(mPath.c_str(), mTmpPath.c_str()) == -1) {
            errno;
            std::cout << errno << std::endl;
            // todo: error
         }

         // todo: check open error!
         mLog.open(mPath, std::ios_base::binary | std::ios_base::out);
      }

      // /////////// Log to disc ///////////////////////
      mLog << msg << std::endl;
      std::cout << msg << std::endl;
      // ///////////////////////////////////////////////

   }
   mBuf.clear();
}

//
bool CBaLog::saveCfg() {

   // Create file-less
   IBaIniParser *pIni = CBaIniParserCreate(0);

   // Set tag for section
   pIni->Set(TAG, "");

   // Set the rest of the values
   pIni->Set(TAG ":mName", mName.c_str());
   pIni->Set(TAG ":mPath", mPath.c_str());

   std::string s = std::to_string(mMaxFileSizeB);
   pIni->Set(TAG ":mMaxFileSizeB", s.c_str());

   s = std::to_string(mMaxNoFiles);
   pIni->Set(TAG ":mMaxNoFiles", s.c_str());

   s = std::to_string(mFileCnt);
   pIni->Set(TAG ":mFileCnt", s.c_str());

   s = std::to_string(mFileSizeB);
   pIni->Set(TAG ":mFileSizeB", s.c_str());

   s = std::to_string(mMaxBufLength);
   pIni->Set(TAG ":mMaxBufLength" ,s.c_str());

   struct stat info;

   // Check if dir exists
   if (stat(CFGDIR, &info) != 0) {
      // Create directory
      if (mkdir(CFGDIR) != 0) {
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



// ////////////////////////////////////////////////////
// put_time is not implemented yet in 4.9.2 thus the tmp NS
namespace tmp_4_9_2 {
LOCAL tm localtime(const std::time_t& rTime) {
   std::tm tm_snapshot;
#ifdef __WIN32
   localtime_s(&tm_snapshot, &rTime);
#else
   localtime_r(&rTime, &tm_snapshot); // POSIX
#endif
   return tm_snapshot;
}


// To simplify things the return value is just a string. I.e. by design!
LOCAL std::string put_time(const std::tm* pDateTime, const char* cTimeFormat) {
   const size_t size = 1024;
   char buffer[size];

   if (!std::strftime(buffer, size, cTimeFormat, pDateTime)) {
      return cTimeFormat;
   }

   return buffer;
}
} // NS tmp_4_9_2

// ////////////////////////////////////////////////////


