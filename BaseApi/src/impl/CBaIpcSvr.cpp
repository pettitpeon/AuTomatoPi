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
#ifdef __linux

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
#define MAXPIPES 256
#define MAXEVENTS 64
#define EPOLLTIMEOUTMS 50
#define PIPESVRNAME "BaIpcFifoSvr"
#define FOREVER 500
/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local variables
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/

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
   return Create(eTypeRd, CBAIPCPIPEDIR CBAIPCSERVER_RD, true);
}

//
CBaPipe* CBaPipe::CreateSvrWr() {
   return Create(eTypeWr, CBAIPCPIPEDIR CBAIPCSERVER_WR, true);
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

   if (!p->pIPCHandlerMsg || !p->pPollMsg) {
      delete p;
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
void CBaPipePairSvr::svrRout(TBaCoreThreadArg *pArg) {

   CBaPipePairSvr* p = (CBaPipePairSvr*)pArg->pArg;
   struct epoll_event *pEvents;
   pEvents = (epoll_event*)calloc(MAXEVENTS, sizeof(pEvents));

   p->mSvrRunning = eBaBool_true;
   TRACE_("IPC sever started successfully");
   while (!p->mThArg.exitTh) {

      // wait for something to do...
      int nfds = epoll_wait(p->mFdEp, pEvents, MAXEVENTS, EPOLLTIMEOUTMS);
      if (nfds < 0) {
         p->pPollMsg->SetDefLogF(eBaLogPrio_Error, TAG,
               "Error in epoll_wait: %s", strerror(errno));
         TRACE_("Error in epoll_wait: %s", strerror(errno));
         continue;
      }

      // Trace here if debugging needed
//      if (nfds > 0) {
//         TRACE_("polled(%i)", nfds);
//      }

      // for each ready socket
      for(int i = 0; i < nfds; i++) {
         if (!(pEvents[i].events & EPOLLIN)) {
            // An error has occurred on this fd, or the socket is not
            // ready for reading (why were we notified then?)
            p->pPollMsg->SetDefLogF(eBaLogPrio_Error, TAG,
                  "Error in epoll fd (%i): %s", pEvents[i].data.fd, strerror(errno));
            continue;
         }

         // Do something with the fd (pipe)
         p->handleIpcMsg(pEvents[i].data.fd);

      }

      BaCoreMSleep(50);
      p->pPollMsg->Reset();
   }

   TRACE_("IPC sever exited successfully");
   p->mSvrRunning = eBaBool_false;
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

   // Read the actual message from the pipe
   // If not all was read at once, continue reading
   size_t rc = 0;
   uint32_t offset = 0;
   do {
      rc = mpRd->Read(((char*) &mMsg) + offset, sz - offset);
      offset += rc;
      if (rc < 0 || offset > sz) {
         pIPCHandlerMsg->SetDefLogF(eBaLogPrio_Error, TAG,
               "Unrecognizable IPC message(%i)", fdRd);
         return false;
      }
   } while (rc != 0);

   // Check if there is a Wr pipe to reply
   if (mpWr->GetServerFd() < 0) {
      mpWr->OpenSvrWr();
   }

   // Handle the command
   switch (mMsg.cmd) {

   // Server status
   case eBaIpcCmdGetSvrStatus:
      mMsg.cmd = eBaIpcReplySvrRuns;
      break;

   // Function call
   case eBaIpcCmdCall: {
      mMsg.cmd = eBaIpcReplyCmdCall;
      TBaIpcFunCall *pFc = (TBaIpcFunCall*) mMsg.dat.data;
      TBaIpcArg ret = {0};
      CBaIpcRegistry::SCallFun(pFc->name, pFc->a, &ret);
      memcpy(mMsg.dat.data, &ret, sizeof(ret));
   }
      break;

   // Variable request
   case eBaIpcCmdGetVar:
      mMsg.cmd = eBaIpcReplyCmdGetVar;
      memset(mMsg.dat.data, 0, sizeof(mMsg.dat.data));

      // dummy answer
      strcpy(mMsg.dat.data, "GetVar");

      break;

   // Error
   default:
      mMsg.cmd = eBaIpcCmdError;
      rc = false;
      pIPCHandlerMsg->SetDefLogF(eBaLogPrio_Error, TAG,
            "Unrecognizable IPC message(%i)", fdRd);
   }

   // Write the answer on the pipe
   pIPCHandlerMsg->Reset();
   rc |= mpWr->Write((char*)&mMsg, sizeof(TBaIpcMsg));
   return rc;
}

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/

#endif
