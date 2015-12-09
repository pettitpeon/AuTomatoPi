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

#include <string>
#include <fstream> // std::ofstream
#include <map>

#include "CBaLog.h"
#include "BaGenMacros.h"


static std::map<std::string, CBaLog*> sLoggers;

/* ****************************************************************************/
/*  Pimpl
 */
class CBaLog::Impl {
public:

   // Constructors
   Impl(std::string name) : name(name), log(), count(1) {
   }

   std::string name;
   std::ofstream log;
   int16_t count;

};


CBaLog* CBaLog::Create(std::string name) {
   if (name.empty()) {
      return 0;
   }
   name = "/var/log/" + name;
   auto logger = sLoggers.find(name);
   if (logger != sLoggers.end()) {
      logger->second->mpImpl->count++;
      return logger->second;
   }

   CBaLog *p = new CBaLog();
   sLoggers[name] = p;
   p->mpImpl = new CBaLog::Impl(name);

   p->mpImpl->log.open(p->mpImpl->name, std::ios_base::binary | std::ios_base::out);
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

   return true;
}
