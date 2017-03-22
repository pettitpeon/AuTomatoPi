/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CBaIpc.cpp
 *   Date     : Nov 14, 2016
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/

#include <CBaIpcSvr.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <mutex>

#include "BaUtils.hpp"
#include "BaLogMacros.h"
#include "BaGenMacros.h"
#include "CBaIpcRegistry.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define TAG     "CIPC"
#define PIPENAME "BaIpc"
#define PIPEEXT ".fifo"
#define SERVER_RD   PIPENAME "SvrRd"
#define SERVER_WR   PIPENAME "SvrWr"
#define MAXPIPES 256
#define MAXEVENTS 64
#define EPOLLTIMEOUTMS 50
#define PIPESVRNAME "BaIpcFifoSvr"
#define FOREVER 500
/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
typedef struct TPipeSvr {
   CBaPipePairSvr *pPipes;
//   TBaCoreThreadArg thArg;
//   TBaCoreThreadHdl th;
   std::mutex mtx;
   TPipeSvr() : pPipes(0)/*, thArg{0}, th(0)*/ {};
} TPipeSvr;

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
//LOCAL void pipeSvrRout(TBaCoreThreadArg *pArg);

/*------------------------------------------------------------------------------
    Local variables
 -----------------------------------------------------------------------------*/

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
TBaBoolRC BaIpcExitSvr() {
   std::lock_guard<std::mutex> lck(sPipeSvr.mtx);
   if (sPipeSvr.pPipes == 0) {
      return eBaBoolRC_Success;
   }

   return eBaBoolRC_Success;
}

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

CBaPipe* CBaPipe::Create(EType type, std::string path, bool overwrite) {
   CBaPipe myPipe;

   myPipe.mName = path;
   myPipe.mType = type;


   if (overwrite) {
      unlink(path.c_str());
   }

   int rc = -1;

   // server reads
   if (myPipe.mType == eTypeRd) {
      rc = mkfifo(myPipe.mName.c_str(), S_IWUSR | S_IRUSR | S_IWGRP | S_IWOTH);

      // server writes
   } else {
      rc = mkfifo(myPipe.mName.c_str(), S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
   }

   //
   if (rc < 0 && errno != EEXIST) {
      WARN_("Make fifo failed: %s\n", strerror(errno));
      return 0;
   }

   // If it was a reading pipe, open it non blocking
   if (myPipe.mType == eTypeRd) {
      myPipe.mFdRd = open(myPipe.mName.c_str(), O_RDWR | O_NONBLOCK);
      if (myPipe.mFdRd < 0) {
         unlink(myPipe.mName.c_str());
         WARN_("Open reader pipe failed: %s\n", strerror(errno));
         return 0;
      }

   }

   // Writing pipes have to wait for the reading side. Try but not obligate
   else {
      myPipe.OpenSvrWr();
   }

   // Allocate new pipe
   return new CBaPipe(myPipe);
}


//
CBaPipe* CBaPipe::CreateSvrRd() {
   return Create(eTypeRd, PIPEDIR SERVER_RD PIPEEXT, true);
}

//
CBaPipe* CBaPipe::CreateSvrWr() {
   return Create(eTypeWr, PIPEDIR SERVER_WR PIPEEXT, true);
}

//
bool CBaPipe::Destroy(CBaPipe* pHdl) {
   // todo: all this checking makes sense or better to delete and unlink blind?
   if (!pHdl || pHdl->mFdRd < 0 || pHdl->mFdWr < 0 || pHdl->mName.length() == 0) {
      return false;
   }

   close(pHdl->mFdRd);
   close(pHdl->mFdWr);
   unlink(pHdl->mName.c_str());

   delete pHdl;
   return true;
}

//
size_t CBaPipe::Read(void* pData, size_t size) {
   size_t bytesRd = read(mFdRd, pData, size);
   if (bytesRd < 0) {
      WARN_("Read pipe failed(fd %i): %s\n", mFdRd, strerror(errno));
   }

   return bytesRd;
}

//
bool CBaPipe::Write(const void* pData, size_t size) {
   if (mType == eTypeRd || mFdWr < 0) {
      return false;
   }

   if (write(mFdWr, pData, size) < 0) {
      WARN_("Write pipe failed(fd %i): %s\n", mFdWr, strerror(errno));
      return false;
   }

   return true;
}

//
inline void CBaPipe::OpenSvrWr() {
   mFdWr = open(mName.c_str(), O_RDWR | O_NONBLOCK);
}

//
CBaPipePairSvr* CBaPipePairSvr::Create(const char *name, TBaCoreThreadFun rout) {

   // If no name or routine, set defaults
   if (!rout) {
      rout = CBaPipePairSvr::svrRout;
   }
   if (!name) {
      name = PIPESVRNAME;
   }

   CBaPipePairSvr* p = new CBaPipePairSvr();
   if (!p) {
      return 0;
   }

   p->mpRd = CBaPipe::CreateSvrRd();
   if (!p->mpRd) {
      delete p;
      return 0;
   }

   p->mpWr = CBaPipe::CreateSvrWr();
   if (!p->mpWr) {
      CBaPipe::Destroy(p->mpRd);
      p->mpRd = 0;
      delete p;
      return 0;
   }

   // epoll to get notified
   p->mFdEp = epoll_create1(0);
   if (p->mFdEp < 0) {
      CBaPipe::Destroy(p->mpRd);
      p->mpRd = 0;
      CBaPipe::Destroy(p->mpWr);
      p->mpWr = 0;
      WARN_("epoll_create failed: %s\n", strerror(errno));
      delete p;
      return 0;
   }

   p->mEv.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
   p->mEv.data.fd = p->mpRd->GetServerFd();
   if (epoll_ctl(p->mFdEp, EPOLL_CTL_ADD, p->mEv.data.fd, &p->mEv) < 0) {
      CBaPipe::Destroy(p->mpRd);
      p->mpRd = 0;
      CBaPipe::Destroy(p->mpWr);
      p->mpWr = 0;
      close(p->mFdEp);
      WARN_("epoll_ctl failed: %s\n", strerror(errno));
      delete p;
      return 0;
   }

   p->mThArg.pArg = p;
   p->mTh = BaCoreCreateThread(name, rout, &p->mThArg, eBaCorePrio_High);
   return p;
}

//
bool CBaPipePairSvr::Destroy(CBaPipePairSvr *pHdl) {
   if (!pHdl) {
      return false;
   }

   bool rc = true;
   close(pHdl->mFdEp);
   rc &= CBaPipe::Destroy(pHdl->mpRd);
   pHdl->mpRd = 0;
   rc &= CBaPipe::Destroy(pHdl->mpWr);
   pHdl->mpWr = 0;

   rc = BaCoreDestroyThread(pHdl->mTh, FOREVER);
   if (rc) {
      delete pHdl;
      pHdl = 0;
   } else {
      ERROR_("Pipe pair thread did not return");
   }

   return rc;
}

//
TBaIpcClntPipes CBaPipePairSvr::GetClientFds() {
   TBaIpcClntPipes pipes;

   pipes.fdRd = mpWr->GetClientFd();
   pipes.fdWr = mpRd->GetClientFd();

   return pipes;
}

//
void CBaPipePairSvr::svrRout(TBaCoreThreadArg *pArg) {

   CBaPipePairSvr* p = (CBaPipePairSvr*)pArg->pArg;
   struct epoll_event *pEvents;
   pEvents = (epoll_event*)calloc(MAXEVENTS, sizeof(pEvents));

   TRACE_("IPC sever started successfully");
   while (!p->mThArg.exitTh) {

      // wait for something to do...
      int nfds = epoll_wait(p->mFdEp, pEvents, MAXEVENTS, EPOLLTIMEOUTMS);
      if (nfds < 0) {
         ERROR_("Error in epoll_wait: %s", strerror(errno));
      }

      // todo: delete
      if (nfds > 0) {
         TRACE_("polled(%i)", nfds);
      }

      // for each ready socket
      for(int i = 0; i < nfds; i++) {
         if (!(pEvents[i].events & EPOLLIN)) {
            // An error has occurred on this fd, or the socket is not
            // ready for reading (why were we notified then?)
            // TODO: msg with state?
            ERROR_("Error in epoll fd (%i): %s", pEvents[i].data.fd, strerror(errno));
            continue;
         }

         // Do something with the fd (pipe)
         p->handleIpcMsg(pEvents[i].data.fd);

      }

      BaCoreMSleep(50);
   }

   free(pEvents);
}

//
bool CBaPipePairSvr::handleIpcMsg(int fdRd) {

   if (fdRd == -1) {
      // error?
      TRACE_("invalid fd(%i)", fdRd);
      return false;
   }

   size_t sz = sizeof(TBaIpcMsg);
   mMsg = {0};

   // If not all was read at once, continue reading
   int rc = 0;
   uint32_t offset = 0;
   do {
      rc = mpRd->Read(((char*) &mMsg) + offset, sz - offset);
      offset += rc;
      if (rc < 0 || offset > sz) {
         return false;
      }
   } while (rc != 0);

   // Check if there is a Wr fd to reply
   if (mpWr->GetServerFd() < 0) {
      mpWr->OpenSvrWr();
   }

   switch (mMsg.cmd) {
      case eBaIpcCmdGetPipePair: {
         mMsg.cmd = eBaIpcReplyPipePair;

         // dummy answer
         TBaIpcClntPipes pipes;
         pipes.fdWr = mpRd->GetServerFd();
         pipes.fdRd = mpWr->GetServerFd();
         memcpy(mMsg.data.data, &pipes, sizeof(pipes));

         break;
      }
      case eBaIpcCmdCall: {
         mMsg.cmd = eBaIpcReplyCmdCall;
//         mMsg.data.data; // This is the name of the function to be called
         TBaIpcFunCall *pFc = (TBaIpcFunCall*) mMsg.data.data;

         TBaIpcArg ret = {0};
         CBaIpcRegistry::SCallFun(pFc->name, pFc->a, &ret);
         memcpy(mMsg.data.data, &ret, sizeof(ret));


         // dummy answer
//         strcpy(mMsg.data.data, "CallFun");
      }
         break;
      case eBaIpcCmdGetVar:
         mMsg.cmd = eBaIpcReplyCmdGetVar;
//         mMsg.data.data; // This is the name of the variable to be called
         memset(mMsg.data.data, 0, sizeof(mMsg.data.data));

         // dummy answer
         strcpy(mMsg.data.data, "GetVar");

         break;
      default:
         ERROR_("IPC msg failed");
         return eBaBoolRC_Error;
   }

   //mMsg.cmd = eBaIpcReplyPipePair;
   rc = mpWr->Write((char*)&mMsg, sizeof(TBaIpcMsg));
   TRACE_("Answered(%i, %i)", rc, mMsg.cmd); // todelete

   return rc;
}

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
