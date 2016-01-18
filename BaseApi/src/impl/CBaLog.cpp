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

#include "CBaLog.h"
#include "../BaGenMacros.h"
#include "BaCore.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#ifdef __linux
# define LOGDIR "/var/log/"
#else
# define LOGDIR "C:\\log\\"
#endif

#define LOGEXT ".log"
#define FASTSIZE 81
#define CHRONOHRC std::chrono::high_resolution_clock
#define CHRONO    std::chrono

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

void CBaLog::logRoutine(TBaCoreThreadArg *pArg) {
   while (!sLogdArg.exitTh) {
      { // RAII Scope
         std::lock_guard<std::mutex> lck(sMtx);
         CBaLog *p = 0;

         // iterate loggers
         for (auto &kv : sLoggers) {
            p = kv.second;

            // Iterate messages in buffer
            for (auto &msg : p->mBuf) {

               // Check file size
               p->mFileSizeB += msg.size();
               if (p->mFileSizeB > p->mMaxFileSizeB) {
                  // Open new file
                  if (p->mFileCnt > ++p->mMaxNoFiles) {
                     p->mFileCnt = 1;
                     // Rewrite file
                  }
                  p->mLog.close();
                  p->mName += std::to_string(p->mFileCnt);

                  // todo: check open error!
                  p->mLog.open(p->mName.c_str(), std::ios_base::binary | std::ios_base::out);

               }

               p->mLog << msg << std::endl;
            }
            kv.second->mBuf.clear();
         }
      }

      BaCoreMSleep(50);
   }
}

bool CBaLog::init() {
   if (sLogdHdl) {
      return true;
   }
   sLogdArg.exitTh = false;

   sLogdHdl = BaCoreCreateThread("BaLogD", logRoutine, &sLogdArg, eBaCorePrio_High);
   return sLogdHdl;
}

bool CBaLog::exit() {
   if (!sLogdHdl) {
      return true;
   }

   BaCoreDestroyThread(sLogdHdl, 100);
   sLogdHdl = 0;
   return true;
}

CBaLog* CBaLog::Create(std::string name) {
   std::lock_guard<std::mutex> lck(sMtx);

   if (name.empty() || !init()) {
      return 0;
   }

   // TODO: name has to contain the fileCount
   name = LOGDIR + name + LOGEXT;
   auto logger = sLoggers.find(name);
   if (logger != sLoggers.end()) {
      logger->second->mOpenCnt++;
      return logger->second;
   }

   CBaLog *p = new CBaLog(name);
   p->mLog.open(p->mName, std::ios_base::binary | std::ios_base::out | std::ios_base::app);
   sLoggers[name] = p;
   return p;
}

bool CBaLog::Delete(CBaLog *pHdl) {
   std::lock_guard<std::mutex> lck(sMtx);

   CBaLog *p = dynamic_cast<CBaLog*>(pHdl);
   if (!p ) {
      return false;
   }

   if (--p->mOpenCnt == 0) {
      // erase from loggers
      sLoggers.erase(p->mName);
      p->flush2Disk();
      delete p;
   }

   if (sLoggers.empty()) {
      exit();
   }

   return true;
}


//
bool CBaLog::Log(const char* msg) {
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

   mBuf.push_back(entry);
   std::cout << entry << std::endl;
   return true;
}

//
void CBaLog::Logf(const char* fmt, ...) {
   va_list arg;
   va_start(arg, fmt);
   uint16_t size = snprintf(0, 0, fmt, arg);
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
   for (auto &msg : mBuf) {
      mLog << msg << std::endl;
      std::cout << msg << std::endl;
   }
}


// ////////////////////////////////////////////////////

// put_time is not implemented yet in 4.9.2 thus the tmp NS
namespace tmp_4_9_2 {
//typedef std::chrono::time_point<std::chrono::system_clock>  system_time_point;

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
}

// ////////////////////////////////////////////////////


