/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <stdarg.h>

#include "CBaLog.h"
#include "BaUtils.hpp"


/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define C_HDL_ ((IBaLog*) hdl)

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/

//
TBaLogHdl BaLogCreateDef(const char *name) {
   return CBaLog::Create(name);
}

//
void BaLogSetDefOpts(TBaLogOptions *pOpts ) {
   if (pOpts) {
      pOpts->name = "";
      pOpts->path = "";
      pOpts->prioFilt = eBaLogPrio_Trace;
      pOpts->out = eBaLogOut_LogAndConsole;
      pOpts->maxFileSizeB = 1048576; // 1 MiB
      pOpts->maxNoFiles = 3;
      pOpts->maxBufLength = 0;
   }
}

//
TBaLogHdl BaLogCreate(const TBaLogOptions *pOpts) {
   return pOpts ? CBaLog::Create(*pOpts) : 0;
}

//
TBaBoolRC BaLogDestroy(TBaLogHdl hld, TBaBool saveCfg) {
   return CBaLog::Destroy((IBaLog*)hld, saveCfg) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaLogLog(TBaLogHdl hdl, EBaLogPrio prio, const char* tag, const char* msg) {
   if (!hdl) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->Log(prio, tag, msg) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaLogTrace(TBaLogHdl hdl, const char* tag, const char* msg) {
   if (!hdl) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->Trace(tag, msg) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaLogWarning(TBaLogHdl hdl, const char* tag, const char* msg) {
   if (!hdl) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->Warning(tag, msg) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaLogError(TBaLogHdl hdl, const char* tag, const char* msg) {
   if (!hdl) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->Error(tag, msg) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaLogLogF(TBaLogHdl hdl, EBaLogPrio prio, const char* tag, const char* fmt, ...) {
   if (!hdl) {
      return eBaBoolRC_Error;
   }

   va_list arg;
   va_start(arg, fmt);
   TBaBoolRC ret =  C_HDL_->LogF(prio, tag, fmt, arg) ? eBaBoolRC_Success : eBaBoolRC_Error;
   va_end(arg);
   return ret;
}

//
TBaBoolRC BaLogTraceF(TBaLogHdl hdl, const char* tag, const char* fmt, ...) {
   if (!hdl) {
      return eBaBoolRC_Error;
   }

   va_list arg;
   va_start(arg, fmt);
   TBaBoolRC ret =  C_HDL_->TraceF(tag, fmt, arg) ? eBaBoolRC_Success : eBaBoolRC_Error;
   va_end(arg);
   return ret;
}

//
TBaBoolRC BaLogWarningF(TBaLogHdl hdl, const char* tag, const char* fmt, ...) {
   if (!hdl) {
      return eBaBoolRC_Error;
   }

   va_list arg;
   va_start(arg, fmt);
   TBaBoolRC ret =  C_HDL_->WarningF(tag, fmt, arg) ? eBaBoolRC_Success : eBaBoolRC_Error;
   va_end(arg);
   return ret;
}

//
TBaBoolRC BaLogErrorF(TBaLogHdl hdl, const char* tag, const char* fmt, ...) {
   if (!hdl) {
      return eBaBoolRC_Error;
   }

   va_list arg;
   va_start(arg, fmt);
   TBaBoolRC ret =  C_HDL_->ErrorF(tag, fmt, arg) ? eBaBoolRC_Success : eBaBoolRC_Error;
   va_end(arg);
   return ret;
}

//
void BaLogGetLogInfo(TBaLogHdl hdl, TBaLogInfo *pInfo) {
   C_HDL_->GetLogInfo(pInfo);
}

//
void BaLogFlush(TBaLogHdl hdl) {
   C_HDL_->Flush();
}

//
void BaLogSysLogF(const char *tag, int line, const char *fmt, ...) {
   va_list arg;
   va_start(arg, fmt);
   std::string s = BaFString(fmt, arg);
   va_end(arg);
   CBaLog::SysLog(tag, line, s.c_str());
}

//
void BaLogSysLog(const char *tag, int line, const char *msg) {
   CBaLog::SysLog(tag, line, msg);
}


/*------------------------------------------------------------------------------
    C++ Factories
 -----------------------------------------------------------------------------*/

//
IBaLog * IBaLogCreateDef(const char *name) {
   return CBaLog::Create(name);
}

//
IBaLog * IBaLogCreate(const TBaLogOptions *pOpts) {
   return pOpts ? CBaLog::Create(*pOpts) : 0;
}

//
TBaBoolRC IBaLogDestroy(IBaLog *pHdl, TBaBool saveCfg) {

   // TODO: dynamic cast has to be done here instead of in CBaLog?
   return CBaLog::Destroy(pHdl, saveCfg);
}


/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/

