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
#include "BaIpcTest.h"
#include "BaGenMacros.h"
#include "BaIpc.h"
//#include "impl/CBaIpcRegistry.h"
#include "impl/CBaIpcRegistry.h"
#include "impl/CBaIpcSvr.h"
#include "BaseApi.h"
#include "CppU.h"

CPPUNIT_TEST_SUITE_REGISTRATION( CBaIpcTest );


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


double TestRegFun(uint32_t i, float f) {
   return f + i;
}


/* ****************************************************************************/
/*  ...
 */
void CBaIpcTest::FunRegNiceWeather() {
   CPPUNIT_ASSERT(true);
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
}

/* ****************************************************************************/
/*  ...
 */
void CBaIpcTest::FunRegErrors() {
   CPPUNIT_ASSERT(true);
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
}


void CBaIpcTest::IPC() {
   CPPUNIT_ASSERT(true);
   bool rc = eBaBool_false;

   // todo: delete log file?
   CPPUNIT_ASSERT(BaApiInitLoggerDef("testDef"));

   pid_t ret = fork();

   if (ret == -1) {
      // error
      return;
   }

   if (ret == 0) {
      // child is server
      std::cout << "Child" << std::endl;


      TBaIpcRegFun fun;
      fun.pFun = (void*) TestRegFun;
      fun.type = "d:If";

      CBaIpcRegistry::SRegisterFun("dummy", fun);

      BaIpcInitSvr();
      BaCoreSleep(30);
      BaIpcExitSvr();

      std::cout << "Exit Child" << std::endl;
      return;
   }


   // parent is client
   std::cout << "Parent:" << ret << std::endl;
   BaCoreSleep(1);
   rc = BaIpcInitClnt();
   std::cout << rc << std::endl;
   TBaIpcFunArg a;
   a.a[0].i = 77777;
   a.a[0].f = 7e7;
   TBaIpcArg r;
   r.I = 0;

   rc = BaIpcCallFun("dummy", a, &r);

   std::cout << rc << "r:" << r.d << std::endl;
   BaCoreSleep(10);
   std::cout << "Exit Parent:" << ret << std::endl;
}

//
LOCAL void funvv() {
   sInt++;
}

LOCAL void funvi(int32_t i) {
   sInt = i;
}



