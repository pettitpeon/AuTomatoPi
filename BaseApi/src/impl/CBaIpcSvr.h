/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CBaIpc.h
 *   Date     : Nov 14, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef CBAIPCSVR_H_
#define CBAIPCSVR_H_
#ifdef __linux
/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <string>
#include <sys/epoll.h>
#include "BaBool.h"
#include "BaCore.h"
#include "BaMsg.h"
#include "BaIpc.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define CBAIPCPIPEDIR "/run/user/0/"
#define CBAIPCSERVER_RD "BaIpcSvrRd.fifo"
#define CBAIPCSERVER_WR "BaIpcSvrWr.fifo"
//#define CBAIPCMSGSZ     1020

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/// IPC protocol requests and replies
typedef enum EBaIpcCmd {
   eBaIpcCmdError = 0, ///< Error
   eBaIpcCmdGetSvrStatus, /**< Sever status request. It is used when client is
                               initialized */
   eBaIpcCmdCall, ///< Function call request
   eBaIpcCmdGetVar, ///< Variable request
   eBaIpcCmdSetVar, ///< Variable set request
   eBaIpcReplySvrRuns, ///< Server is running reply
   eBaIpcReplyCmdCall, ///< Function call reply
   eBaIpcReplyCmdGetVar, ///< Variable request reply
   eBaIpcReplyCmdSetVar, ///< Variable request reply
   eBaIpcCmdMax = eBaIpcReplyCmdSetVar
} EBaIpcCmd;

typedef int32_t TBaIpcCmd;

typedef struct TBaIpcMsg {
   TBaIpcCmd cmd;
   union Data {
      char data[BAIPC_MSGDATASZ];
   } dat;
} TBaIpcMsg;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/
class CBaPipe {
public:

   enum EType {
      eTypeRd = 0, // The server reads
      eTypeWr      // The server writes
   };

   static CBaPipe* Create(
         EType type,
         std::string name,
         bool overwrite
         );

   static CBaPipe* CreateSvrRd();
   static CBaPipe* CreateSvrWr();

   static bool Destroy(
         CBaPipe* pHdl
         );

   size_t Read(
         void* pData,
         size_t size
         );

   bool Write(
         const void* pData,
         size_t size
         );

   // Does not have to be successful
   inline void OpenSvrWr();

   int GetServerFd() { return mType == eTypeRd ? mFdRd : mFdWr; };
   int GetClientFd() { return mType == eTypeRd ? mFdWr : mFdRd; };
   EType GetType () { return mType; };

   // Typical object oriented destructor must be virtual!
   CBaPipe() : mFdRd(-1), mFdWr(-1), mName(""), mType(eTypeRd) {};

   virtual ~CBaPipe() {};

private:
   int mFdRd;
   int mFdWr;
   std::string mName;
   EType mType;
};

class CBaPipePairSvr {
public:

   static CBaPipePairSvr* Create(
         const char *name,
         TBaCoreThreadFun rout
         );

   static bool Destroy(
         CBaPipePairSvr *pHdl
         );

   TBaBool SvrRunning() { return mSvrRunning; };

   CBaPipePairSvr() : mpRd(0), mpWr(0), mFdEp(0), mTh(0), mThArg{0}, mEv{0},
         mMsg{0}, mSvrRunning(eBaBool_false),
         pIPCHandlerMsg(IBaMsgCreate()), pPollMsg(IBaMsgCreate()) {};

   // Typical object oriented destructor must be virtual!
   virtual ~CBaPipePairSvr() {
      IBaMsgDestroy(pIPCHandlerMsg);
      IBaMsgDestroy(pPollMsg);
   };

private:
   static void svrRout(TBaCoreThreadArg *pArg);

   bool handleIpcMsg(
         int fdRds
         );

   CBaPipe* mpRd; // server reads here
   CBaPipe* mpWr; // server writes here
   int mFdEp;  // epoll fd

   TBaCoreThreadHdl mTh;
   TBaCoreThreadArg mThArg;
   struct epoll_event mEv; // epoll event
   TBaIpcMsg mMsg; // Msg used for reading and writing

   volatile TBaBool mSvrRunning;

   IBaMsg *pIPCHandlerMsg;
   IBaMsg *pPollMsg;

};

#endif // __linux
#endif // CBAIPCSVR_H_
