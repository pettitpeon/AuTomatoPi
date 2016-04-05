/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <stdarg.h>

#include "CBaLog.h"


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


/*------------------------------------------------------------------------------
    C++ Factories
 -----------------------------------------------------------------------------*/

//
IBaLog * CBaLogCreateDef(const char *name) {
   return CBaLog::Create(name);
}

//
IBaLog * CBaLogCreate(const TBaLogOptions *pOpts) {
   return pOpts ? CBaLog::Create(*pOpts) : 0;
}

//
TBaBoolRC CBaLogDestroy(IBaLog *pHdl, TBaBool saveCfg) {
   return CBaLog::Destroy(pHdl, saveCfg);
}


/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/

