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

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define C_HDL_ ((IBaIpc*) hdl)
#define TAG "BaIpc"

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
typedef struct TPipeSvr {
   CBaPipePairSvr *pPipes;
   std::mutex mtx;
   TPipeSvr() : pPipes(0) {};
} TPipeSvr;


/*------------------------------------------------------------------------------
    Static vars
 -----------------------------------------------------------------------------*/
static int sRdFifo = -1;
static int sWrFifo = -1;
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

   return eBaBoolRC_Success;
}

//
TBaBoolRC BaIpcInitClnt() {
   if (sRdFifo != -1 && sWrFifo != -1) {
      return eBaBoolRC_Success;
   }

   int fdWr = open(PIPEDIR "BaIpcSvrRd.fifo", O_WRONLY | O_NONBLOCK);
   int fdRd = open(PIPEDIR "BaIpcSvrWr.fifo", O_RDONLY | O_NONBLOCK);

   if (fdWr < 0) {
      ERROR_("%s", strerror(errno));
      return eBaBoolRC_Error;
   }

   if (fdRd < 0) {
      close(fdWr);
      ERROR_("%s", strerror(errno));
      return eBaBoolRC_Error;
   }

   sWrFifo = fdWr;
   sRdFifo = fdRd;
   TBaIpcMsg msg = {0};
   msg.cmd = eBaIpcCmdGetSvrStatus;

   // Check if the server is running
   BaIpcWritePipe(sWrFifo, (const char*)&msg, sizeof(TBaIpcMsg));

   for (int i = 1; i < 50; ++i) {
      BaCoreMSleep(20);
      BaIpcReadPipe(fdRd, (char*)&msg, sizeof(TBaIpcMsg));
      if (msg.cmd == eBaIpcReplySvrRuns) {
         TRACE_("Server is running");
         return eBaBoolRC_Success;
      }
   }

   // todo: Return error or success?
   TRACE_("Server is not running");
   return eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcExitClnt() {
   int rc = 0;
   rc = close(sWrFifo);
   if (rc == -1) {
      WARN_("Pipe(%i) did not close: %s", sWrFifo, strerror(errno));
   }

   rc |= close(sRdFifo);
   if (rc == -1) {
      WARN_("Pipe(%i) did not close: %s", sRdFifo, strerror(errno));
   }

   sRdFifo = -1;
   sWrFifo = -1;

   return !rc ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcCallFun(const char* name, TBaIpcFunArg a, TBaIpcArg *pRet) {
   if (sRdFifo == -1 || sWrFifo == -1) {
      return eBaBoolRC_Error;
   }
   TBaIpcMsg msg = {0};

   msg.cmd = eBaIpcCmdCall;
   TBaIpcFunCall fc;
   fc.name = name;
   fc.a = a;
   memcpy(msg.data.data, &fc, sizeof(fc));

   // send mesg
   if (!BaIpcWritePipe(sWrFifo, (char*) &msg, sizeof(TBaIpcMsg))) {
      return eBaBoolRC_Error;
   }

   // todo: wait for reply??
   for (int i = 0; i < 100; ++i) {
      BaCoreMSleep(10);
      memset(msg.data.data, 0, sizeof(msg.data.data));
      BaIpcReadPipe(sRdFifo, (char*)&msg, sizeof(TBaIpcMsg));
      if (msg.cmd == eBaIpcReplyCmdCall) {
         *pRet = *((TBaIpcArg*)msg.data.data);
         // todo: delete
         TRACE_("Call delay %i ms", i*10);
         return eBaBoolRC_Success;
      }
   }

   return eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcReadPipe(int fd, char* pData, size_t size) {
   if (fd < 0 || !pData) {
      return eBaBoolRC_Error;
   }

   // If not all was read at once, continue reading
   size_t rc = 0;
   size_t offset = 0;
   do {
      rc = read(fd, pData + offset, size - offset);
      offset += rc;
      if (rc < 0 || offset > size) {
         WARN_("Read failed(fd %i): %s", fd, strerror(errno));
         return eBaBoolRC_Error;
      }
   } while (rc != 0);

   return eBaBoolRC_Success;
}

//
TBaBoolRC BaIpcWritePipe(int fd, const char* pData, size_t sz) {
   if (fd < 0 || !pData) {
      return eBaBoolRC_Error;
   }

   size_t bytesWr = 0;
   size_t offset = 0;
   do {
      bytesWr = write(fd, (const void*)(pData + offset), sz - offset);
      offset += bytesWr;
      if (bytesWr < 0) {
         WARN_("IPC write failed: %s", strerror(errno));
         return eBaBoolRC_Error;
      }

      if (offset > sz) {
         ERROR_("IPC write failed");
         return eBaBoolRC_Error;
      }
   } while (bytesWr != 0);

   return eBaBoolRC_Success;
}

