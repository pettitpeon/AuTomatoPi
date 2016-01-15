/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CBaLog.cpp
 *   Date     : Dec 9, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/*  @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
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

#ifdef __linux
# define LOGDIR "/var/log/"
#else
# define LOGDIR "C:\\log\\"
#endif

#define LOGEXT ".log"
#define FASTSIZE 81
#define CHRONOHRC std::chrono::high_resolution_clock
#define CHRONO    std::chrono

static std::map<std::string, CBaLog*> sLoggers;
static TBaCoreThreadHdl sLogdHdl = 0;
static TBaCoreThreadArg sLogdArg = {0};
static std::mutex sMtx;
static char sFasMsg[FASTSIZE];

namespace tmp_4_9_2 {
LOCAL tm localtime(const std::time_t& time);
LOCAL std::string put_time(const std::tm* date_time, const char* c_time_format);

}

/* ****************************************************************************/
/*  Pimpl
 */
class CBaLog::Impl {
public:

   // Constructors
   Impl(std::string name) : name(name), log(), openCnt(1), fileSizeB(0), buf(),
   maxFileSizeB(1024), maxNoFiles(2), fileCnt(0) {
   }


   //
   static bool init() {
      if (sLogdHdl) {
         return true;
      }
      sLogdArg.exitTh = false;

      sLogdHdl = BaCoreCreateThread("BaLogD", logRoutine, &sLogdArg, eBaCorePrio_High);
      return sLogdHdl;
   }

   //
   static bool exit() {
      if (!sLogdHdl) {
         return true;
      }

      BaCoreDestroyThread(sLogdHdl, 100);
      sLogdHdl = 0;
      return true;
   }

   //
   static void logRoutine(TBaCoreThreadArg *pArg) {
      while (!sLogdArg.exitTh) {
         {
            std::lock_guard<std::mutex> lck(sMtx);
            Impl *p = 0;

            // iterate loggers
            for (auto &kv : sLoggers) {
               p = kv.second->mpImpl;

               // Iterate messages in buffer
               for (auto &msg : p->buf) {

                  // Check file size
                  p->fileSizeB += msg.size();
                  if (p->fileSizeB > p->maxFileSizeB) {
                     // Open new file
                     if (p->fileCnt > ++p->maxNoFiles) {
                        p->fileCnt = 1;
                        // Rewrite file
                     }
                     p->log.close();
                     p->name += std::to_string(p->fileCnt);

                     // todo: check open error!
                     p->log.open(p->name.c_str(), std::ios_base::binary | std::ios_base::out);

                  }

                  p->log << msg << std::endl;
               }
               kv.second->mpImpl->buf.clear();
            }
         }

         BaCoreMSleep(50);
      }
   }

   void flush2Disk() {
      for (auto &msg : buf) {
         log << msg << std::endl;
         std::cout << msg << std::endl;
      }
   }

   std::string name;
   std::ofstream log;
   int16_t openCnt;
   uint32_t fileSizeB;
   std::vector<std::string> buf;
   uint32_t maxFileSizeB;
   uint16_t maxNoFiles;
   uint16_t fileCnt;

};


CBaLog* CBaLog::Create(std::string name) {
   std::lock_guard<std::mutex> lck(sMtx);

   if (name.empty() || !Impl::init()) {
      return 0;
   }

   // TODO: name has to contain the fileCount
   name = LOGDIR + name + LOGEXT;
   auto logger = sLoggers.find(name);
   if (logger != sLoggers.end()) {
      logger->second->mpImpl->openCnt++;
      return logger->second;
   }

   CBaLog *p = new CBaLog();
   p->mpImpl = new CBaLog::Impl(name);
   p->mpImpl->log.open(p->mpImpl->name, std::ios_base::binary | std::ios_base::out | std::ios_base::app);
   sLoggers[name] = p;
   return p;
}

bool CBaLog::Delete(CBaLog *pHdl) {
   std::lock_guard<std::mutex> lck(sMtx);

   CBaLog *p = dynamic_cast<CBaLog*>(pHdl);
   if (!p ) {
      return false;
   }

   if (--p->mpImpl->openCnt == 0) {
      // erase from loggers
      sLoggers.erase(p->mpImpl->name);
      p->mpImpl->flush2Disk();
      delete p->mpImpl;
      delete p;
   }

   if (sLoggers.empty()) {
      Impl::exit();
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


   std::string timeStamp = tmp_4_9_2::put_time(&timem, "%y/%m/%d %H:%M:%S");

   mpImpl->buf.push_back(msg);
   std::cout << timeStamp << "." << ms.count() % 1000 << "| " << msg << std::endl;
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


// ////////////////////////////////////////////////////
namespace tmp_4_9_2 {
typedef std::chrono::time_point<std::chrono::system_clock>  system_time_point;

LOCAL tm localtime(const std::time_t& time) {
  std::tm tm_snapshot;
#ifdef __WIN32
  localtime_s(&tm_snapshot, &time);
#else
  localtime_r(&time, &tm_snapshot); // POSIX
#endif
  return tm_snapshot;
}


// To simplify things the return value is just a string. I.e. by design!
LOCAL std::string put_time(const std::tm* date_time, const char* c_time_format) {
  const size_t size = 1024;
  char buffer[size];

  if (!std::strftime(buffer, size, c_time_format, date_time)) {
    return c_time_format;
  }

  return buffer;
}
}

// ////////////////////////////////////////////////////


