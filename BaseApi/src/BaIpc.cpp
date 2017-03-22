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

#include "BaIpc.h"
#include "CBaIpcSvr.h"
#include "BaLogMacros.h"
#include "CBaIpcRegistry.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define C_HDL_ ((IBaIpc*) hdl)
#define DEF_PIPE PIPEDIR
#define TAG "BaIpc"

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Static vars
 -----------------------------------------------------------------------------*/
static int sRdFifo = -1;
static int sWrFifo = -1;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
//
TBaBoolRC BaIpcInitClnt() {
   int fdWr = open(DEF_PIPE "BaIpcSvrRd.fifo", O_WRONLY | O_NONBLOCK);
   int fdRd = open(DEF_PIPE "BaIpcSvrWr.fifo", O_RDONLY | O_NONBLOCK);

   if (fdWr < 0) {
      ERROR_("%s", strerror(errno));
      return eBaBoolRC_Error;
   }

   sWrFifo = fdWr;
   TBaIpcMsg msg = {0};
   msg.cmd = eBaIpcCmdGetPipePair;

   BaIpcWritePipe(sWrFifo, (const char*)&msg, sizeof(TBaIpcMsg));

   for (int i = 0; i < 50; ++i) {
      BaCoreMSleep(20);
      BaIpcReadPipe(fdRd, (char*)&msg, sizeof(TBaIpcMsg));
      if (msg.cmd == eBaIpcReplyPipePair) {

         TBaIpcClntPipes* p = (TBaIpcClntPipes*)&msg.data.data;
         printf("eBaIpcReplyPipePair: r %i=%i,  w %i=%i\n",
               p->fdRd, fdRd, p->fdWr, fdWr);

         if (p->fdWr == sWrFifo && p->fdRd == fdRd) {
            sRdFifo = p->fdRd;
            return eBaBoolRC_Success;
         }
      }
   }
   return eBaBoolRC_Error;
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
   for (int i = 0; i < 10; ++i) {
      BaCoreMSleep(100);
      memset(msg.data.data, 0, sizeof(msg.data.data));
      BaIpcReadPipe(sRdFifo, (char*)&msg, sizeof(TBaIpcMsg));
      if (msg.cmd == eBaIpcReplyCmdCall) {
         *pRet = *((TBaIpcArg*)msg.data.data);
         return eBaBoolRC_Success;
      }
   }

   return eBaBoolRC_Error;
}

//
TBaBoolRC BaIpcCreatePipeReader() {
   if (sRdFifo != -1) {
      return eBaBoolRC_Success;
   }

   int rc = mkfifo(DEF_PIPE, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
   if (rc != 0) {
      printf("BaIpcCreatePipeReader: %s\n", strerror(errno));
      return eBaBoolRC_Error;
   }

   int fd = open(DEF_PIPE, O_RDONLY | O_NONBLOCK);

   if (fd < 0) {
      printf("BaIpcCreatePipeReader: %s\n", strerror(errno));
      return eBaBoolRC_Error;
   }

   sRdFifo = fd;
   return eBaBoolRC_Success;
}

TBaBoolRC BaIpcCreatePipeWriter() {
   if (sWrFifo != -1) {
      return eBaBoolRC_Success;
   }

   if (sRdFifo < 0) {
      return BaIpcCreatePipeReader();
   }

   int fd = open(DEF_PIPE, O_WRONLY | O_NONBLOCK);

   if (fd < 0) {
      printf("%i\n", errno);
      return eBaBoolRC_Error;
   }

   sWrFifo = fd;
   return eBaBoolRC_Success;
}

//
TBaBoolRC BaIpcDestroyPipe() {
   if (sRdFifo < 0) {
      return eBaBoolRC_Error;
   }

   close((int) sRdFifo);
   unlink(DEF_PIPE);

   return eBaBoolRC_Success;
}

TBaBoolRC BaIpcReadPipe(int fd, char* pData, size_t size) {
   if (fd < 0 || !pData) {
      return eBaBoolRC_Error;
   }

   int rc = read(fd, pData, size);
   if (rc < 0) {
      //todo: this can be a spamming message
      printf("BaIpcReadPipe: %s\n", strerror(errno));
      return eBaBoolRC_Error;
   }

   return eBaBoolRC_Success;
}

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
         ERROR_("IPC write failed: %s", strerror(errno));
         return eBaBoolRC_Error;
      }

      if (offset > sz) {
         ERROR_("IPC write failed");
         return eBaBoolRC_Error;
      }
   } while (bytesWr != 0);

   return eBaBoolRC_Success;
}


/*------------------------------------------------------------------------------
    C++ Factories
 -----------------------------------------------------------------------------*/
//
IBaIpc * IBaIpcCreate() {
   return 0;
}

//
TBaBoolRC IBaIpcDestroy(IBaIpc *pHdl) {
   return eBaBoolRC_Success;
}

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/

