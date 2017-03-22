/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CBaMsg.cpp
 *   Date     : Apr 26, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */

#include <stdarg.h>
#include <stdio.h>

#include "CBaMsg.h"
#include "BaUtils.hpp"
#include "BaseApi.h"

//
void CBaMsg::SetPrintF(const char *fmt, ...) {
   if(!mSet) {
      va_list arg;
      va_start(arg, fmt);
      vprintf(fmt, arg);
      va_end(arg);
      mSet = true;
   }
}

//
void CBaMsg::SetPrint(const char *msg) {
   if(!mSet) {
      fputs(msg, stdout);
      mSet = true;
   }
}

//
void CBaMsg::SetSysLogF(const char *tag, int line, const char *fmt, ...) {
   if(!mSet) {
      va_list arg;
      va_start(arg, fmt);
      std::string s = BaFString(fmt, arg);
      va_end(arg);
      BaLogSysLog(tag, line, s.c_str());
      mSet = true;
   }
}

void CBaMsg::SetSysLog(const char *tag, int line, const char *msg) {
   if(!mSet) {
      BaLogSysLog(tag, line, msg);
      mSet = true;
   }
}

//
void CBaMsg::SetLogF(IBaLog* pLog, EBaLogPrio prio, const char *tag, const char *fmt, ...) {
   if(pLog && !mSet) {
      va_list arg;
      va_start(arg, fmt);
      std::string s = BaFString(fmt, arg);
      va_end(arg);
      pLog->Log(prio, tag, s.c_str());
      mSet = true;
   }
}

//
void CBaMsg::SetLog(IBaLog* pLog, EBaLogPrio prio, const char *tag, const char *msg) {
   if(pLog && !mSet) {
      pLog->Log(prio, tag, msg);
      mSet = true;
   }
}

//
void CBaMsg::SetDefLog(EBaLogPrio prio, const char *tag, const char *msg) {
   if(!mSet) {
      mSet = BaApiLogF(prio, tag, msg);
   }
}

//
void CBaMsg::SetDefLogF(EBaLogPrio prio, const char *tag, const char *fmt, ...) {
   if(!mSet) {
      va_list arg;
      va_start(arg, fmt);
      std::string s = BaFString(fmt, arg);
      va_end(arg);
      mSet = BaApiLogF(prio, tag, s.c_str());
   }
}
