/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : OsIpcTest.cpp
 *   Date     : 12.03.2017
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */

#ifdef __linux

#include "OsIpcTest.h"

#include <unistd.h>
#include <iostream>
#include <chrono>
#include <impl/COsIpcRegistry.h>
#include <impl/COsIpcSvr.h>
#include <OsIpc.h>
#include "BaGenMacros.h"
#include "BaseApi.h"
#include "CppU.h"
#include "BaUtils.hpp"
#include "BaLogMacros.h"

CPPUNIT_TEST_SUITE_REGISTRATION( COsIpcTest );

#define LOGFILE "testDef"
#define LOGDIR "/var/log/"
#define SET_FUN(regFun, fun, TYPE) regFun.pFun = (void*) fun; \
      regFun.type = TYPE;
#define TAG "IPCTST"

LOCAL void funvv();
LOCAL void funvi(int32_t i);
LOCAL double fundiIfd(int32_t i, int64_t I, float f, double d)
   { return i + I + f + d; };
template<typename T> LOCAL T funTT(T TT) {return TT; };
template<typename T> LOCAL T funTTTTT(T T1, T T2, T T3, T T4)
   { return T1 + T2 + T3 + T4; };

static int32_t sInt = 0;

static char sStr[] = "init";

/* ****************************************************************************/
/*  ...
 */
void COsIpcTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void COsIpcTest::tearDown() {
}


LOCAL int32_t testRegFun(int32_t i) {
   sInt = i;
   TRACE_("i = %i", sInt);
   return sInt;
}

LOCAL TBaBoolRC testLongSleepFun() {
   BaCoreSleep(1);
   TRACE_("testLongSleepFun Finished");
   return eBaBoolRC_Success;
}

void COsIpcTest::Init() {
   ASS(BaApiInitLoggerDef(LOGFILE));
}

/* ****************************************************************************/
/*  ...
 */
void COsIpcTest::VarRegNiceWeather() {
   ASS(true);

   // Creation and normal use
   COsIpcRegistry* pReg = COsIpcRegistry::Create();
   TOsIpcRegVar var;
   std::string s = "init";

   // Set variable structure
   var.pVar = (void*)sStr;
   var.sz = sizeof(strlen(sStr));
   var.wr = eBaBool_true;

   ASS(pReg->RegisterVar("TestString", var));

   // Test the return value of the registered var
   var = {0};
   ASS(pReg->CallVarInternal("TestString", var));
   ASS(s == (const char*)var.pVar);

   // Change the var directly and test again
   s = "ini";
   strncpy(sStr, "ini", s.length() + 1);
   ASS(pReg->CallVarInternal("TestString", var));
   ASS(s == (const char*)var.pVar);

   // Change through the registered var and check against the direct var
   var.pVar = (void*)"new";
   var.sz = sizeof(strlen("new"));
   ASS(pReg->SetVar("TestString", var));
   s = "new";
   ASS(s == sStr);

   // Finally check against the registered var
   var = {0};
   ASS(pReg->CallVarInternal("TestString", var));
   ASS(s == (const char*)var.pVar);
}

/* ****************************************************************************/
/*  ...
 */
void COsIpcTest::FunRegNiceWeather() {
   ASS(true);
   bool ret = false;

   // Creation and normal use
   COsIpcRegistry* pReg = COsIpcRegistry::Create();
   TOsIpcRegFun fun;
   TOsIpcFunArg a = {0};
   TOsIpcArg tOut = {0};
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

   COsIpcRegistry::Destroy(pReg);
}

/* ****************************************************************************/
/*  ...
 */
void COsIpcTest::FunRegErrors() {
   ASS(true);
   bool ret = false;

   // Creation
   COsIpcRegistry* pReg = COsIpcRegistry::Create();
   TOsIpcRegFun fun;
   TOsIpcFunArg a = {0};
   TOsIpcArg tOut = {0};
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
void COsIpcTest::IPCServer() {
   ASS(true);
   ASS(OsIpcInitSvr());
   int i;

   bool svrRun = false;
   for (i = 0; i < 500; ++i) {
      if (OsIpcSvrRunning()) {
         svrRun = true;
         break;
      }
      BaCoreMSleep(10);
   }
   ASS(svrRun);

   ASS(OsIpcInitClnt());
   ASS(OsIpcInitClnt());

   TOsIpcRegFun fun;
   fun.pFun = (void*) testRegFun;
   fun.type = "i:i";

   ASS(COsIpcRegistry::SInitRegistry());
   ASS(COsIpcRegistry::SRegisterFun("testRegFun", fun));
   std::string name;
   for (int i = 0; i < 5000; ++i) {
      ASS(COsIpcRegistry::SRegisterFun(BaFString("testRegFun_%i", i), fun));
   }

   // testLongSleepFun
   fun.pFun = (void*) testLongSleepFun;
   fun.type = "i:v";
   ASS(COsIpcRegistry::SRegisterFun("testLongSleepFun", fun));


   sInt = 1;
   TOsIpcFunArg a = {0};
   a.a[0].i = 7;
   TOsIpcArg r;
   r.I = 0;

   TBaCoreMonTStampUs us = BaCoreGetMonTStamp();
   ASS(OsIpcCallFun("testRegFun", a, &r));
   us = BaCoreGetMonTStamp() - us;
   std::cout << "Call duration: " << us/1000.0 << "ms" << std::endl;
   ASS_EQ(r.i, sInt);

   // Register a variable
   TOsIpcRegVar var;
   TOsIpcRegVarOut varOut = {0};
   uint32_t target = 11;
   var.pVar = (void *)&target;
   var.sz = sizeof(target);
   var.wr = true;
   ASS(COsIpcRegistry::SRegisterVar("testvar", var));

   // Call a var
   ASS(OsIpcCallVar("testvar", &varOut));
   ASS_EQ(11, varOut.dat.i);

   // Call a bad var
   ASS(!OsIpcCallVar("BAADFOOD", &varOut));
   ASS_EQ((size_t)4, varOut.sz); // not changed

   // todo: Set a var

   // Register a function that will not finish on time
   r.I = 0;
   us = BaCoreGetMonTStamp();
   ASS(!OsIpcCallFun("testLongSleepFun", a, &r));
   us = BaCoreGetMonTStamp() - us;
   std::cout << "Call duration: " << us/1000.0 << "ms: " << r.i << std::endl;

   // Exit
   COsIpcRegistry::SClearFunRegistry();
   ASS(OsIpcExitClnt());
   ASS(OsIpcExitSvr());
}

//
void COsIpcTest::IPCRealFunClientServer() {
   CPPUNIT_ASSERT(true);
   bool rc = false;

   pid_t pid = fork();

   ASS(pid != -1);

   if (pid == 0) {
      // Child is client
      std::cout << "Child - Client" << std::endl;
      BaCoreMSleep(10);
      rc = OsIpcInitClnt();
      std::cout << "Client init: " << rc << std::endl;
      TOsIpcFunArg a = {0};
      a.a[0].i = 7;
      TOsIpcArg r;
      r.I = 0;
      rc = OsIpcCallFun("dummy", a, &r);

      std::cout << "Exit Child" << std::endl;
      exit(0);
   }

   // parent is server
   std::cout << "Parent - Server:" << pid << std::endl;
   TOsIpcRegFun fun;
   fun.pFun = (void*) testRegFun;
   fun.type = "i:i";
   COsIpcRegistry::SRegisterFun("dummy", fun);
   sInt = 0;

   ASS(OsIpcInitSvr());
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

   ASS(OsIpcExitSvr());

   std::cout << "Exit Parent:" << pid << std::endl;
}

//
void COsIpcTest::Exit() {
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


#endif // _linux

