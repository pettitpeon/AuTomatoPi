/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaIpc.cpp
 *   Date     : Nov 7, 2016
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <mutex>

#include "BaIpc.h"
#include "CBaIpcSvr.h"
#include "BaLogMacros.h"
#include "CBaIpcRegistry.h"
#include "BaGenMacros.h"
#include "BaMsg.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define C_HDL_ ((IBaIpc*) hdl)
#define TAG "BaIpc"
#define WRPIPE CBAIPCPIPEDIR CBAIPCSERVER_RD // RD svr is WR clnt
#define RDPIPE CBAIPCPIPEDIR CBAIPCSERVER_WR // WR svr is RD clnt


/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
typedef struct TPipeSvr {
   CBaPipePairSvr *pPipes;
   std::mutex mtx;
   TPipeSvr() : pPipes(0) {};
} TPipeSvr;

//
LOCAL TBaBoolRC readClntPipe(char* pData, size_t sz);
LOCAL TBaBoolRC writeClntPipe(const char* pData, size_t sz);


/*------------------------------------------------------------------------------
    Static vars
 -----------------------------------------------------------------------------*/
static int sClntRdFifo = -1;
static int sClntWrFifo = -1;
static IBaMsg *spClntRdMsg = 0;
static IBaMsg *spClntWrMsg = 0;

static TPipeSvr sPipeSvr;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/

//
TBaBoolRC BaIpcInitSvr() {
   std::lock_guard<std::mutex> lck(sPipeSvr.mtx);
   if (sPipeSvr.pPipes != 0) {
      return eBaBoolRC_Success;
   }

   sPipeSvr.pPipes = CBaPipePairSvr::Create(0, 0);

   return sPipeSvr.pPipes ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcSvrRunning() {
   return sPipeSvr.pPipes ? sPipeSvr.pPipes->SvrRunning() : eBaBool_false;
}

//
TBaBoolRC BaIpcExitSvr() {
   std::lock_guard<std::mutex> lck(sPipeSvr.mtx);
   if (sPipeSvr.pPipes == 0) {
      return eBaBoolRC_Success;
   }

   auto p = sPipeSvr.pPipes;
   sPipeSvr.pPipes = 0;
   return CBaPipePairSvr::Destroy(p) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

/* ****************************************************************************
 *  CLIENT side
 ******************************************************************************/
TBaBoolRC BaIpcInitClnt() {
   if (sClntRdFifo != -1 && sClntWrFifo != -1 && spClntRdMsg && spClntWrMsg) {
      return eBaBoolRC_Success;
   }

   spClntRdMsg = spClntRdMsg ? spClntRdMsg : IBaMsgCreate();
   spClntWrMsg = spClntWrMsg ? spClntWrMsg : IBaMsgCreate();
   if (!spClntRdMsg || !spClntWrMsg) {
      return eBaBoolRC_Error;
   }

   int fdWr = open(WRPIPE, O_WRONLY | O_NONBLOCK);
   int fdRd = open(RDPIPE, O_RDONLY | O_NONBLOCK);

   if (fdWr < 0) {
      ERROR_("%s", strerror(errno));
      return eBaBoolRC_Error;
   }

   if (fdRd < 0) {
      close(fdWr);
      ERROR_("%s", strerror(errno));
      return eBaBoolRC_Error;
   }

   sClntWrFifo = fdWr;
   sClntRdFifo = fdRd;
   TBaIpcMsg msg = {0};
   msg.cmd = eBaIpcCmdGetSvrStatus;

   // Check if the server is running
   writeClntPipe((const char*)&msg, sizeof(TBaIpcMsg));

   for (int i = 1; i < 50; ++i) {
      BaCoreMSleep(20);
      readClntPipe((char*)&msg, sizeof(TBaIpcMsg));
      if (msg.cmd == eBaIpcReplySvrRuns) {
         TRACE_("Server is running");
         return eBaBoolRC_Success;
      }
   }

   TRACE_("Server is not running");
   return eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcExitClnt() {
   int rc = 0;
   rc = close(sClntWrFifo);
   if (rc == -1) {
      WARN_("Pipe(%i) did not close: %s", sClntWrFifo, strerror(errno));
   }

   rc |= close(sClntRdFifo);
   if (rc == -1) {
      WARN_("Pipe(%i) did not close: %s", sClntRdFifo, strerror(errno));
   }

   sClntRdFifo = -1;
   sClntWrFifo = -1;

   IBaMsgDestroy(spClntRdMsg);
   IBaMsgDestroy(spClntWrMsg);
   spClntRdMsg = 0;
   spClntWrMsg = 0;

   return !rc ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcCallFun(const char* name, TBaIpcFunArg a, TBaIpcArg *pRet) {
   if (sClntRdFifo == -1 || sClntWrFifo == -1) {
      return eBaBoolRC_Error;
   }
   TBaIpcMsg msg = {0};

   msg.cmd = eBaIpcCmdCall;
   TBaIpcFunCall fc;
   fc.name = name;
   fc.a = a;
   memcpy(msg.data.data, &fc, sizeof(fc));

   // send mesg
   if (!writeClntPipe((char*) &msg, sizeof(TBaIpcMsg))) {
      return eBaBoolRC_Error;
   }

   memset(msg.data.data, 0, sizeof(msg.data.data));

   // todo: wait for reply??
   for (int i = 0; i < 100; ++i) {
      if (readClntPipe((char*)&msg, sizeof(TBaIpcMsg)) && msg.cmd == eBaIpcReplyCmdCall) {
         *pRet = *((TBaIpcArg*)msg.data.data);
         // todo: delete
         TRACE_("Call delay %i ms", i*10);
         return eBaBoolRC_Success;
      }

      BaCoreMSleep(10);
   }

   return eBaBoolRC_Error;
}

//
LOCAL TBaBoolRC readClntPipe(char* pData, size_t size) {
   if (sClntRdFifo < 0 || !pData) {
      return eBaBoolRC_Error;
   }

   // If not all was read at once, continue reading
   size_t rc = 0;
   size_t offset = 0;
   do {
      rc = read(sClntRdFifo, pData + offset, size - offset);
      offset += rc;
      if (rc < 0 || offset > size) {
         spClntRdMsg->SetDefLogF(eBaLogPrio_Warning, TAG,
               "Read failed(fd %i): %s", sClntRdFifo, strerror(errno));
         return eBaBoolRC_Error;
      }
   } while (rc != 0);

   spClntRdMsg->Reset();
   return eBaBoolRC_Success;
}

//
LOCAL TBaBoolRC writeClntPipe(const char* pData, size_t sz) {
   if (sClntWrFifo < 0 || !pData) {
      return eBaBoolRC_Error;
   }

   size_t bytesWr = 0;
   size_t offset = 0;
   do {
      bytesWr = write(sClntWrFifo, (const void*)(pData + offset), sz - offset);
      offset += bytesWr;
      if (bytesWr < 0) {
         spClntWrMsg->SetDefLogF(eBaLogPrio_Warning, TAG,
               "IPC write failed: %s", strerror(errno));
         return eBaBoolRC_Error;
      }

      if (offset > sz) {
         spClntWrMsg->SetDefLog(eBaLogPrio_Warning, TAG,
               "IPC write failed");
         return eBaBoolRC_Error;
      }
   } while (bytesWr != 0);

   spClntWrMsg->Reset();
   return eBaBoolRC_Success;
}

