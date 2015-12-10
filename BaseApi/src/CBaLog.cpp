/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CBaLog.cpp
 *   Date     : Dec 9, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */

#include <fstream> // std::ofstream
#include <map>
#include <vector>
#include <iostream>
#include <mutex>

#include "CBaLog.h"
#include "BaGenMacros.h"
#include "BaCore.h"


static std::map<std::string, CBaLog*> sLoggers;
static TBaCoreThreadHdl sLogdHdl = 0;
static TBaCoreThreadArg sLogdArg = {0};
static std::mutex mtx;

/* ****************************************************************************/
/*  Pimpl
 */
class CBaLog::Impl {
public:

   // Constructors
   Impl(std::string name) : name(name), log(), count(1), fileSize(0), buf() {
   }


   //
   static bool init() {
      if (sLogdHdl) {
         return true;
      }
      sLogdArg.exitTh = false;

      sLogdHdl = BaCoreCreateThread("BaLogD", logRoutine, 0, eBaCorePrio_High);
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
         for (auto &kv : sLoggers) {
            if (kv.second->mpImpl->buf.size()) {
               for (auto &msg : kv.second->mpImpl->buf) {
                  kv.second->mpImpl->log << msg << std::endl;
                  std::cout << msg << std::endl;
               }
               kv.second->mpImpl->buf.clear();
            }
         }
         BaCoreMSleep(50);
      }
   }

   std::string name;
   std::ofstream log;
   int16_t count;
   uint32_t fileSize;
   std::vector<std::string> buf;

};


CBaLog* CBaLog::Create(std::string name) {
   if (name.empty() || !Impl::init()) {
      return 0;
   }

   name = "/var/log/" + name;
   auto logger = sLoggers.find(name);
   if (logger != sLoggers.end()) {
      logger->second->mpImpl->count++;
      return logger->second;
   }

   CBaLog *p = new CBaLog();
   p->mpImpl = new CBaLog::Impl(name);
   p->mpImpl->log.open(p->mpImpl->name, std::ios_base::binary | std::ios_base::out);
   sLoggers[name] = p;
   return p;
}

bool CBaLog::Delete(CBaLog *pHdl) {
   CBaLog *p = dynamic_cast<CBaLog*>(pHdl);
   if (!p ) {
      return false;
   }

   if (--p->mpImpl->count == 0) {
      // erase from loggers
      sLoggers.erase(p->mpImpl->name);
      delete p->mpImpl;
      delete p;
   }

   if (sLoggers.empty()) {
      Impl::exit();
   }
   return true;
}

bool CBaLog::Log(std::string msg) {
   mpImpl->buf.push_back(msg);
   return true;
}


