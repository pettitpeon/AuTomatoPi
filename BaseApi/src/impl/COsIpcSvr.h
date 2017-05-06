/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : COsIpc.h
 *   Date     : Nov 14, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef COSIPCSVR_H_
#define COSIPCSVR_H_

#ifdef __linux
/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <string>
#include <sys/epoll.h>
#include "BaBool.h"
#include "BaCore.h"
#include "BaMsg.h"
#include <OsIpc.h>

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define COSIPCPIPEDIR "/run/user/0/"
#define COSIPCSERVER_RD "OsIpcSvrRd.fifo"
#define COSIPCSERVER_WR "OsIpcSvrWr.fifo"

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/// IPC protocol requests and replies
typedef enum EOsIpcCmd {
   eOsIpcCmdError = 0, ///< Error
   eOsIpcCmdGetSvrStatus, /**< Sever status request. It is used when client is
                               initialized */
   eOsIpcCmdCall, ///< Function call request
   eOsIpcCmdGetVar, ///< Variable request
   eOsIpcCmdSetVar, ///< Variable set request
   eOsIpcReplySvrRuns, ///< Server is running reply
   eOsIpcReplyCmdCall, ///< Function call reply
   eOsIpcReplyCmdGetVar, ///< Variable request reply
   eOsIpcReplyCmdSetVar, ///< Variable request reply
   eOsIpcCmdMax = eOsIpcReplyCmdSetVar
} EOsIpcCmd;

typedef int32_t TOsIpcCmd;

typedef struct TOsIpcMsg {
   TOsIpcCmd cmd;
   union Data {
      char data[OSIPC_MSGDATASZ];
   } dat;
} TOsIpcMsg;

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
   TOsIpcMsg mMsg; // Msg used for reading and writing

   volatile TBaBool mSvrRunning;

   IBaMsg *pIPCHandlerMsg;
   IBaMsg *pPollMsg;

};

#endif // __linux
#endif // COSIPCSVR_H_
