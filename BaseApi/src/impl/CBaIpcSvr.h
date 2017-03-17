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

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <string>
#ifdef __linux
# include <sys/epoll.h>
#endif
#include "BaBool.h"
#include "BaCore.h"


/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define PIPEDIR "/run/user/0/"

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
typedef enum EBaIpcCmd {
   eBaIpcCmdGetPipePair = 1,
   eBaIpcCmdCall,
   eBaIpcCmdGetVar,
   eBaIpcReplyPipePair,
   eBaIpcReplyCmdCall,
   eBaIpcReplyCmdGetVar,
   eBaIpcCmdMax = eBaIpcReplyCmdGetVar
} EBaIpcCmd;
typedef int32_t TBaIpcCmd;

typedef struct TBaIpcMsg {
   TBaIpcCmd cmd;
   union Data {
      char data[1020];
   } data;
} TBaIpcMsg;

typedef struct TBaIpcClntPipes {
   int fdRd;
   int fdWr;
} TBaIpcClntPipes;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
TBaBoolRC BaIpcInitSvr();
TBaBoolRC BaIpcExitSvr();

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

   virtual size_t Read(
         void* pData,
         size_t size
         );

   virtual bool Write(
         const void* pData,
         size_t size
         );

   // Does not have to be successful
   virtual inline void OpenSvrWr();

   virtual int GetServerFd() { return mType == eTypeRd ? mFdRd : mFdWr; };
   virtual int GetClientFd() { return mType == eTypeRd ? mFdWr : mFdRd; };
   virtual EType GetType () { return mType; };

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

//   virtual size_t Read(
//         void* pData,
//         size_t size
//         );
//
//   virtual size_t Write(
//         const void* pData,
//         size_t size
//         );

//   virtual void GetClientFds(int* pFdRd, int* pFdWr);
   virtual TBaIpcClntPipes GetClientFds();

   CBaPipePairSvr() : mpRd(0), mpWr(0), mFdEp(0), mTh(0), mThArg{0}, mEv{0}, mMsg{0} {};

   // Typical object oriented destructor must be virtual!
   virtual ~CBaPipePairSvr() {};

private:
   static void svrRout(
         TBaCoreThreadArg *pArg
         );

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

};


#endif // CBAIPCSVR_H_
