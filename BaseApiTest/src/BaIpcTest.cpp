/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaIpcTest.cpp
 *   Date     : 12.03.2017
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#include <unistd.h>
#include <iostream>
#include <chrono>
#include "BaIpcTest.h"
#include "BaGenMacros.h"
#include "BaIpc.h"
#include "impl/CBaIpcRegistry.h"
#include "impl/CBaIpcSvr.h"
#include "BaseApi.h"
#include "CppU.h"
#include "BaUtils.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION( CBaIpcTest );

#define LOGFILE "testDef"
#define LOGDIR "/var/log/"

LOCAL void funvv();
LOCAL void funvi(int32_t i);
LOCAL double fundiIfd(int32_t i, int64_t I, float f, double d)
   { return i + I + f + d; };
template<typename T> LOCAL T funTT(T TT) {return TT; };
template<typename T> LOCAL T funTTTTT(T T1, T T2, T T3, T T4)
   { return T1 + T2 + T3 + T4; };

static int32_t sInt = 0;

/* ****************************************************************************/
/*  ...
 */
void CBaIpcTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaIpcTest::tearDown() {
}

#define SET_FUN(regFun, fun, TYPE) regFun.pFun = (void*) fun; \
      regFun.type = TYPE;


LOCAL int32_t testRegFun(int32_t i) {
   sInt = i;
   return sInt;
}

void CBaIpcTest::Init() {
   ASS(BaApiInitLoggerDef(LOGFILE));
}

/* ****************************************************************************/
/*  ...
 */
void CBaIpcTest::FunRegNiceWeather() {
   ASS(true);
   bool ret = false;

   // Creation and normal use
   CBaIpcRegistry* pReg = CBaIpcRegistry::Create();
   TBaIpcRegFun fun;
   TBaIpcFunArg a = {0};
   TBaIpcArg tOut = {0};
   sInt = 0;

   SET_FUN(fun, funvv, "v:v");
   ret = pReg->RegisterFun(fun.type, fun);
   ASS(ret);
   ret = pReg->CallFun(fun.type, a, &tOut);
   ASS(ret);
   ASS_EQ(1, sInt);

   SET_FUN(fun, funvi, "v:i");
   ret = pReg->RegisterFun(fun.type, fun);
   ASS(ret);
   a.a[0].i = 7;
   ret = pReg->CallFun(fun.type, a, &tOut);
   ASS(ret);
   ASS_EQ(7, sInt);

   SET_FUN(fun, funTT<int32_t>, "i:i");
   ret = pReg->RegisterFun(fun.type, fun);
   ASS(ret);
   a.a[0].i = 7;
   ret = pReg->CallFun(fun.type, a, &tOut);
   ASS(ret);
   ASS_EQ(7, tOut.i);

   SET_FUN(fun, funTT<double>, "d:d");
   ret = pReg->RegisterFun(fun.type, fun);
   ASS(ret);
   a.a[0].d = 7;
   ret = pReg->CallFun(fun.type, a, &tOut);
   ASS(ret);
   ASS_D_EQ(7.0, tOut.d, 0.0);

   SET_FUN(fun, funTTTTT<int64_t>, "I:IIII");
   ret = pReg->RegisterFun(fun.type, fun);
   ASS(ret);
   a.a[0].I = 1;
   a.a[1].I = 2;
   a.a[2].I = 3;
   a.a[3].I = 4;
   ret = pReg->CallFun(fun.type, a, &tOut);
   ASS(ret);
   ASS_EQ(10ll, tOut.I);

   //fundiIfd
   SET_FUN(fun, fundiIfd, "d:iIfd");
   ret = pReg->RegisterFun(fun.type, fun);
   ASS(ret);
   a.a[0].i = 1;
   a.a[1].I = 2;
   a.a[2].f = 3.3f;
   a.a[3].d = 4.4;
   ret = pReg->CallFun(fun.type, a, &tOut);
   ASS(ret);
   ASS_D_EQ(10.7, tOut.d, 0.0001);

   ASS(pReg->UnregisterFun(fun.type));
   pReg->ClearFunRegistry();
}

/* ****************************************************************************/
/*  ...
 */
void CBaIpcTest::FunRegErrors() {
   ASS(true);
   bool ret = false;

   // Creation
   CBaIpcRegistry* pReg = CBaIpcRegistry::Create();
   TBaIpcRegFun fun;
   TBaIpcFunArg a = {0};
   TBaIpcArg tOut = {0};
   sInt = 0;

   SET_FUN(fun, funvv, "q:v");
   ret = pReg->RegisterFun(fun.type, fun);
   ASS(ret);
   ret = pReg->CallFun(fun.type, a, &tOut);
   ASS(!ret);

   SET_FUN(fun, funvv, "qv");
   ret = pReg->RegisterFun(fun.type, fun);
   ASS(!ret);
   ret = pReg->CallFun(fun.type, a, &tOut);
   ASS(!ret);

   SET_FUN(fun, funvv, "I:iiiii");
   ret = pReg->RegisterFun(fun.type, fun);
   ASS(!ret);
   ret = pReg->CallFun(fun.type, a, &tOut);
   ASS(!ret);

   SET_FUN(fun, 0, "");
   ret = pReg->RegisterFun(fun.type, fun);
   ASS(!ret);
   ret = pReg->CallFun(fun.type, a, &tOut);
   ASS(!ret);

}

//
void CBaIpcTest::IPCServer() {
   ASS(true);
   ASS(BaIpcInitSvr());
   int i;
   for (i = 0; i < 500 && !BaIpcSvrRunning(); ++i) {
      BaCoreMSleep(10);
   }

   ASS(BaIpcInitClnt());
   ASS(BaIpcInitClnt());

   TBaIpcRegFun fun;
   fun.pFun = (void*) testRegFun;
   fun.type = "i:i";

   ASS(CBaIpcRegistry::SRegisterFun("testRegFun", fun));
   std::string name;
   for (int i = 0; i < 5000; ++i) {
      ASS(CBaIpcRegistry::SRegisterFun(BaFString("testRegFun_%i", i), fun));
   }

   sInt = 1;
   TBaIpcFunArg a = {0};
   a.a[0].i = 7;
   TBaIpcArg r;
   r.I = 0;

   TBaCoreMonTStampUs us = BaCoreGetMonTStamp();
   ASS(BaIpcCallFun("testRegFun", a, &r));
   us = BaCoreGetMonTStamp() - us;
   std::cout << "Call duration: " << us/1000.0 << "ms" << std::endl;
   ASS_EQ(r.i, sInt);

   CBaIpcRegistry::SClearFunRegistry();
   ASS(BaIpcExitClnt());
   ASS(BaIpcExitSvr());
}

//
void CBaIpcTest::IPCRealClientServer() {
   CPPUNIT_ASSERT(true);
   bool rc = false;

   pid_t pid = fork();

   ASS(pid != -1);

   if (pid == 0) {
      // Child is client
      std::cout << "Child - Client" << std::endl;
      BaCoreMSleep(10);
      rc = BaIpcInitClnt();
      std::cout << "Client init: " << rc << std::endl;
      TBaIpcFunArg a = {0};
      a.a[0].i = 7;
      TBaIpcArg r;
      r.I = 0;
      rc = BaIpcCallFun("dummy", a, &r);

      std::cout << "Exit Child" << std::endl;
      exit(0);
   }

   // parent is server
   std::cout << "Parent - Server:" << pid << std::endl;
   TBaIpcRegFun fun;
   fun.pFun = (void*) testRegFun;
   fun.type = "i:i";
   CBaIpcRegistry::SRegisterFun("dummy", fun);
   sInt = 0;

   ASS(BaIpcInitSvr());
   rc = false;
   int i = 0;
   for (i = 0; i < 20; ++i) {
      BaCoreMSleep(50);
      if (sInt == 7) {
         rc = true;
         break;
      }
   }
   ASS(rc);

   ASS(BaIpcExitSvr());

   std::cout << "Exit Parent:" << pid << std::endl;
}

//
void CBaIpcTest::Exit() {
   ASS(BaApiExitLogger());
   remove(LOGDIR LOGFILE ".log");
}

//
LOCAL void funvv() {
   sInt++;
}

//
LOCAL void funvi(int32_t i) {
   sInt = i;
}



