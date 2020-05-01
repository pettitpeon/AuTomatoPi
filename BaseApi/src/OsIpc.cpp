/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : OsIpc.cpp
 *   Date     : Nov 7, 2016
 *------------------------------------------------------------------------------
 */

#ifdef __linux
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
#include <poll.h>

#include "OsIpc.h"
#include "COsIpcSvr.h"
#include "BaLogMacros.h"
#include "COsIpcRegistry.h"
#include "BaGenMacros.h"
#include "BaMsg.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define TAG "OsIpc"
#define WRPIPE COSIPCPIPEDIR COSIPCSERVER_RD // RD svr is WR clnt
#define RDPIPE COSIPCPIPEDIR COSIPCSERVER_WR // WR svr is RD clnt
#define WAITRPLY 100 // 1s


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
static IBaMsg *spClntWrMsg = 0;

static TPipeSvr sPipeSvr;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/

//
TBaBoolRC OsIpcInitSvr() {
   std::lock_guard<std::mutex> lck(sPipeSvr.mtx);
   if (sPipeSvr.pPipes != 0) {
      return eBaBoolRC_Success;
   }

   sPipeSvr.pPipes = CBaPipePairSvr::Create(0, 0);

   return sPipeSvr.pPipes ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcSvrRunning() {
   return sPipeSvr.pPipes ? sPipeSvr.pPipes->SvrRunning() : eBaBool_false;
}

//
TBaBoolRC OsIpcExitSvr() {
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
TBaBoolRC OsIpcInitClnt() {
   if (sClntRdFifo != -1 && sClntWrFifo != -1 && spClntWrMsg) {
      return eBaBoolRC_Success;
   }

   spClntWrMsg = spClntWrMsg ? spClntWrMsg : IBaMsgCreate();
   if (!spClntWrMsg) {
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
   TOsIpcMsg msg = {0};
   msg.cmd = eOsIpcCmdGetSvrStatus;

   // Check if the server is running
   writeClntPipe((const char*)&msg, sizeof(TOsIpcMsg));

   // Read reply
   readClntPipe((char*)&msg, sizeof(TOsIpcMsg));
   if (msg.cmd == eOsIpcReplySvrRuns) {
      TRACE_("Server is running");
      return eBaBoolRC_Success;
   }

   TRACE_("Server is not running");
   return eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcExitClnt() {
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

   IBaMsgDestroy(spClntWrMsg);
   spClntWrMsg = 0;

   return !rc ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcCallFun(const char* name, TOsIpcFunArg a, TOsIpcArg *pRet) {
   if (sClntRdFifo == -1 || sClntWrFifo == -1) {
      return eBaBoolRC_Error;
   }
   TOsIpcMsg msg = {0};
   size_t sz = strlen(name) + 1;
   if (sz > OSIPC_FUNNAMESZ) {
      return eBaBoolRC_Error;
   }

   // Prepare the function call message
   msg.cmd = eOsIpcCmdCall;

   // Create a pointer to the desired structure to ease the copy
   TOsIpcFunCallData *pFc = (TOsIpcFunCallData *)msg.dat.data;
   memcpy(pFc->name, name, sz);
   pFc->a = a;

   // Send message
   if (!writeClntPipe((char*) &msg, sizeof(TOsIpcMsg))) {
      return eBaBoolRC_Error;
   }

   // Erase the message to reuse it
   memset(msg.dat.data, 0, sizeof(msg.dat.data));

   // Read reply
   if (readClntPipe((char*)&msg, sizeof(TOsIpcMsg)) && msg.cmd == eOsIpcReplyCmdCall) {

      // Copy data to return value
      *pRet = *((TOsIpcArg*)msg.dat.data);
      return eBaBoolRC_Success;
   }

   WARN_("CallFun: No reply from server");
   return eBaBoolRC_Error;
}

//
TBaBoolRC OsIpcCallVar(const char* name, TOsIpcRegVarOut *pVar) {
   if (!pVar || sClntRdFifo == -1 || sClntWrFifo == -1) {
      return eBaBoolRC_Error;
   }
   TOsIpcMsg msg = {0};
   size_t sz = strlen(name) + 1;

   if (sz > OSIPC_MSGDATASZ) {
      return eBaBoolRC_Error;
   }

   // Prepare the message for the variable call
   msg.cmd = eOsIpcCmdGetVar;
   memcpy(msg.dat.data, name, sz);

   // Send message
   if (!writeClntPipe((char*) &msg, sizeof(TOsIpcMsg))) {
      return eBaBoolRC_Error;
   }

   // Erase message for reuse
   memset(msg.dat.data, 0, sizeof(msg.dat.data));


   if (readClntPipe((char*)&msg, sizeof(TOsIpcMsg)) && msg.cmd == eOsIpcReplyCmdGetVar) {
      // capture reply
      TOsIpcRegVarOut *p = (TOsIpcRegVarOut *)msg.dat.data;
      if (p->sz > OSIPC_MAXVARSZ) {
         return eBaBoolRC_Error;
      }

      memcpy(pVar, msg.dat.data, sizeof(TOsIpcRegVarOut));
      return eBaBoolRC_Success;
   }

   WARN_("CallVar: No reply from server");
   return eBaBoolRC_Error;
}

//
LOCAL TBaBoolRC readClntPipe(char* pData, size_t size) {
   if (sClntRdFifo < 0 || !pData) {
      return eBaBoolRC_Error;
   }

   pollfd pfds[1] = {{0, 0, 0}};
   pfds[0].fd = sClntRdFifo;
   pfds[0].events |= POLLIN;
   constexpr auto timeoutMs = 100;

   // Poll for read ready (POLLIN)
   if (!poll(pfds, 1, timeoutMs))
   {
      TRACE_("poll timeout (fd %i)", sClntRdFifo);
      return eBaBoolRC_Error;
   }

   // If not all was read at once, continue reading
   size_t rc = 0;
   size_t offset = 0;
   do {
      rc = read(sClntRdFifo, pData + offset, size - offset);
      offset += rc;
      if (rc < 0 || offset > size) {
         TRACE_("Read failed(fd %i): %s", sClntRdFifo, strerror(errno));
         return eBaBoolRC_Error;
      }
   } while (rc != 0);

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

#endif // __linux

