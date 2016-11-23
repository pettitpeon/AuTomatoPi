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
#include <mutex>

#include "BaUtils.hpp"
#include "BaLogMacros.h"
#include "BaGenMacros.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define TAG     "CIPC"
#define PIPENAME "BaIpc"
#define PIPEEXT ".fifo"
#define MAXPIPES 256
#define MAXEVENTS 64
#define EPOLLTIMEOUTMS 50
#define PIPESVRNAME "fifoSvr"
#define FOREVER 500
/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
typedef struct TPipeSvr {
   CBaPipePair *pPipes;
   TBaCoreThreadArg thArg;
   TBaCoreThreadHdl th;
   std::mutex mtx;
   TPipeSvr() : pPipes(0), thArg{0}, th(0) {};
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

   sPipeSvr.pPipes = CBaPipePair::Create(0, 0);

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

//
//LOCAL void pipeSvrRout(TBaCoreThreadArg *pArg) {
//
//   TPipeSvr* p = (TPipeSvr*)pArg->pArg;
//   struct epoll_event *pEvents;
//   pEvents = (epoll_event*)calloc(MAXEVENTS, sizeof(pEvents));
//
//   int fd = p->pPipes->mpRd->GetServerFd();
//   while (!p->thArg.exitTh) {
//
//      // wait for something to do...
//      int nfds = epoll_wait(fd, pEvents, MAXEVENTS, EPOLLTIMEOUTMS);
//
//      if (nfds < 0) {
//         ERROR_("Error in epoll_wait");
//      }
//
//      // for each ready socket
//      for(int i = 0; i < nfds; i++) {
//         if (!(pEvents[i].events & EPOLLIN)) {
//            // An error has occurred on this fd, or the socket is not
//            // ready for reading (why were we notified then?)
//            close (pEvents[i].data.fd);
//            ERROR_("Error in epoll fd (%i)", pEvents[i].data.fd);
//            continue;
//         }
//         // do something with the fd (pipe)
//         pEvents[i].data.fd;
//
//      }
//
//      BaCoreMSleep(1);
//   }
//
//   free(pEvents);
//}


/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/
CBaPipe* CBaPipe::Create(EType type) {

   CBaPipe* p = new CBaPipe();
   if (!p) {
      return 0;
   }

   for (uint32_t i = 1; i < MAXPIPES; ++i) {
      p->mName = PIPEDIR PIPENAME + BaFString("%i", i) + PIPEEXT;


      int rc = -1;

      // server reads
      if (type == eTypeRd) {
         rc = mkfifo(p->mName.c_str(), S_IWUSR | S_IRUSR | S_IWGRP | S_IWOTH);

      // server writes
      } else {
         rc = mkfifo(p->mName.c_str(), S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
      }


      // Success!
      if (rc == 0) {
         break;
      }

      if (rc < 0 && errno != EEXIST) {
         WARN_("Make fifo failed: %s\n", strerror(errno));
         return 0;
      }

   }

   if (type == eTypeRd) {
      p->mFdRd = open(p->mName.c_str(), O_RDWR | O_NONBLOCK);
      if (p->mFdRd < 0) {
         unlink(p->mName.c_str());
         WARN_("Open reader pipe failed: %s\n", strerror(errno));
         return 0;
      }

   } else {
      p->mFdWr = open(p->mName.c_str(), O_WRONLY | O_NONBLOCK);
      if (p->mFdWr < 0) {
         WARN_("Open writer pipe failed: %s\n", strerror(errno));
         unlink(p->mName.c_str());
         return 0;
      }
   }

   p->mType = type;

   return p;
}

//
bool CBaPipe::Destroy(CBaPipe* pHdl) {
   if (!pHdl || pHdl->mFdRd < 0 || pHdl->mFdWr < 0 || pHdl->mName.length() == 0) {
      return false;
   }

   close(pHdl->mFdRd);
   close(pHdl->mFdWr);
   unlink(pHdl->mName.c_str());

   return true;
}//

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
   if (mType == eTypeRd) {
      return false;
   }

   if (write(mFdWr, pData, size) < 0) {
      WARN_("Write pipe failed(fd %i): %s\n", mFdWr, strerror(errno));
      return false;
   }

   return true;
}

//
CBaPipePair* CBaPipePair::Create(const char *name, TBaCoreThreadFun rout) {
   if (!rout) {
      rout = CBaPipePair::svrRout;
   }
   if (!name) {
      name = PIPESVRNAME;
   }


   CBaPipePair* p = new CBaPipePair();
   if (!p) {
      return 0;
   }

   p->mpRd = CBaPipe::Create(CBaPipe::eTypeRd);
   if (!p->mpRd) {
      return 0;
   }

//   p->mpWr = CBaPipe::Create(CBaPipe::eTypeWr);
//   if (!p->mpWr) {
//      CBaPipe::Destroy(p->mpRd);
//      p->mpRd = 0;
//      return 0;
//   }

   p->mFdEp = epoll_create1(0);
   if (p->mFdEp < 0) {
      CBaPipe::Destroy(p->mpRd);
      p->mpRd = 0;
      CBaPipe::Destroy(p->mpWr);
      p->mpWr = 0;
      WARN_("epoll_create failed: %s\n", strerror(errno));
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
      return 0;
   }

   p->mThArg.pArg = p;
   p->mTh = BaCoreCreateThread(name, rout, &p->mThArg, eBaCorePrio_High);
   return p;
}

//
bool CBaPipePair::Destroy(CBaPipePair *pHdl) {
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
TBaIpcClntPipes CBaPipePair::GetClientFds() {
   TBaIpcClntPipes pipes;

   pipes.fdRd = mpWr->GetClientFd();
   pipes.fdWr = mpRd->GetClientFd();

   return pipes;
}

//
void CBaPipePair::svrRout(TBaCoreThreadArg *pArg) {

   CBaPipePair* p = (CBaPipePair*)pArg->pArg;
   struct epoll_event *pEvents;
   pEvents = (epoll_event*)calloc(MAXEVENTS, sizeof(pEvents));

   TRACE_("IPC sever started successfully");
   while (!p->mThArg.exitTh) {

      // wait for something to do...
      int nfds = epoll_wait(p->mFdEp, pEvents, MAXEVENTS, EPOLLTIMEOUTMS);
      if (nfds < 0) {
         ERROR_("Error in epoll_wait: %s", strerror(errno));
      }
      TRACE_("polled(%i)", nfds);

      // for each ready socket
      for(int i = 0; i < nfds; i++) {
         if (!(pEvents[i].events & EPOLLIN)) {
            // An error has occurred on this fd, or the socket is not
            // ready for reading (why were we notified then?)
            ERROR_("Error in epoll fd (%i): %s", pEvents[i].data.fd, strerror(errno));
//            close (pEvents[i].data.fd);
            continue;
         }
         // do something with the fd (pipe)
         p->handleIpcMsg(pEvents[i].data.fd);

      }

      BaCoreMSleep(50);
   }

   free(pEvents);
}

//
bool CBaPipePair::handleIpcMsg(int fdRd) {

   if (fdRd == -1) {
      // error?
      TRACE_("invalid fd(%i)", fdRd);
      return false;
   }

   TBaIpcMsg *pMsg = 0;
   size_t sz = sizeof(TBaIpcMsg);
   char *pRawMsg = (char *)calloc(1, sz);


   // If not all was read at once, continue reading
   int rc = 0;
   uint32_t offset = 0;
   do {
      rc = mpRd->Read(pRawMsg + offset, sz - offset);
      offset += rc;
      if (rc < 0 || offset > sz) {
         free(pRawMsg);
         return false;
      }
   } while (rc != 0);

   pMsg = (TBaIpcMsg *)pRawMsg;

   if (!mpWr) {
      mpWr = CBaPipe::Create(CBaPipe::eTypeWr);
      if (!mpWr) {
         return false;
      }
   }

   switch (pMsg->cmd) {
      case eBaIpcCmdGetPipePair: {
         TBaIpcClntPipes pipes = GetClientFds();
         pMsg->cmd = eBaIpcReplyPipePair;
         memcpy(pMsg->data.data, &pipes, sizeof(TBaIpcClntPipes));
         rc = mpWr->Write(pMsg, sizeof(TBaIpcMsg));
         TRACE_("Answered(%i)", rc);

         break;
      }
      case eBaIpcCmdCall:

         break;
      case eBaIpcCmdGetVar:

         break;
      default:
         ("IPC msg failed");
         free(pRawMsg);
         return eBaBoolRC_Error;
   }

   free(pRawMsg);
   return true;
}

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
