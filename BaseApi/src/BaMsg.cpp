/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaMsg.cpp
 *   Date     : Apr 26, 2016
 *------------------------------------------------------------------------------
 */

// C includes
#include <stdarg.h>

// Local includes
#include "BaMsg.h"
#include "CBaMsg.h"
#include "BaUtils.hpp"

/*------------------------------------------------------------------------------
 *  Defines
 */
#define C_HDL_ ((IBaMsg*) hdl)

/*------------------------------------------------------------------------------
 *  C Interface
 */
//
TBaMsgHdl BaMsgCreate() {
   return (TBaMsgHdl) new CBaMsg();
}

//
TBaBoolRC BaMsgDestroy(TBaMsgHdl hdl) {
   return IBaMsgDestroy(C_HDL_);
}

//
void BaMsgSetPrintF(TBaMsgHdl hdl, const char *fmt, ...) {
   va_list arg;
   va_start(arg, fmt);
   std::string s = BaFString(fmt, arg);
   va_end(arg);
   C_HDL_->SetPrint(s.c_str());
}

//
void BaMsgSetPrint(TBaMsgHdl hdl, const char *msg) {
   C_HDL_->SetPrint(msg);
}

//
void BaMsgSetSysLogF(TBaMsgHdl hdl, const char *tag, int line, const char *fmt, ...) {
   va_list arg;
   va_start(arg, fmt);
   std::string s = BaFString(fmt, arg);
   va_end(arg);
   C_HDL_->SetSysLog(tag, line, s.c_str());
}

//
void BaMsgSetSysLog(TBaMsgHdl hdl, const char *tag, int line, const char *msg) {
   C_HDL_->SetSysLog(tag, line, msg);
}

//
void BaMsgSetLogF(TBaMsgHdl hdl, TBaLogHdl log, EBaLogPrio prio,
      const char *tag, const char *fmt, ...) {
   va_list arg;
   va_start(arg, fmt);
   std::string s = BaFString(fmt, arg);
   va_end(arg);
   C_HDL_->SetLog((IBaLog*)log, prio, tag, s.c_str());
}

//
void BaMsgSetLog(TBaMsgHdl hdl, TBaLogHdl log, EBaLogPrio prio,
      const char *tag, const char *msg) {
   C_HDL_->SetLog((IBaLog*)log, prio, tag, msg);
}

//
void BaMsgReset(TBaMsgHdl hdl ) {
   C_HDL_->Reset();
}

//
TBaBool BaMsgGet(TBaMsgHdl hdl) {
   return C_HDL_->Get();
}

/*------------------------------------------------------------------------------
 *  C++ Interface
 */
//
IBaMsg * IBaMsgCreate() {
   return (IBaMsg*) new CBaMsg();
}

//
TBaBoolRC IBaMsgDestroy(
      IBaMsg *pHdl
      ) {
   CBaMsg *p = dynamic_cast<CBaMsg*>(pHdl);
   if (!p ) {
      return eBaBoolRC_Error;
   }

   delete p;
   return eBaBoolRC_Success;
}

