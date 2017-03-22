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

CPPUNIT_TEST_SUITE_REGISTRATION( CBaIpcTest );

LOCAL int tFunInt(uint32_t i, float f);
LOCAL float tFunflt(uint32_t i, float f);
LOCAL double tFundbl(uint32_t i, float f);

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


/* ****************************************************************************/
/*  ...
 */
void CBaIpcTest::FunRegistry() {
   CPPUNIT_ASSERT(true);
   bool ret = false;
   CBaIpcRegistry* pReg = CBaIpcRegistry::Create();
   TBaIpcRegFun fun;
   TBaIpcFunArg a;
   a.a[0].I = INT64_MAX;
   a.a[1].f = 7.77e7;
   TBaIpcArg tOut = {0};

   a.a[0].i = 7777777;

   ret = pReg->CallFun("dummy", a, &tOut);
   std::cout << tOut.d << "\n" << (double)(a.a[1].f + a.a[0].u) << std::endl;

   //fun.pFun = reinterpret_cast<void*>(testFunInt);
   SET_FUN(fun, tFunInt, "I:If");
//   fun.pFun = (void*) testFunInt;
//   fun.type = "I:If";
   SET_FUN(fun, tFunInt, "I:If");
   ret = pReg->RegisterFun(fun.type, fun);


   ret = pReg->CallFun(fun.type, a, &tOut);
   std::cout << tOut.i << "\n" << (int)((uint32_t)a.a[0].i + a.a[1].f) << std::endl;

   SET_FUN(fun, tFunflt, "f:If");
   ret = pReg->RegisterFun(fun.type, fun);
   ret = pReg->CallFun(fun.type, a, &tOut);
   std::cout << tOut.f << "\n" << (float)((uint32_t)a.a[0].i + a.a[1].f) << std::endl;

   SET_FUN(fun, tFundbl, "d:If");
   ret = pReg->RegisterFun(fun.type, fun);
   ret = pReg->CallFun(fun.type, a, &tOut);
   std::cout << tOut.d << "\n" << (double)((uint32_t)a.a[0].i + a.a[1].f) << std::endl;

   ret = false;

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
LOCAL int tFunInt(uint32_t i, float f) {
   int ret = i + f;
   return ret;
}

//
LOCAL float tFunflt(uint32_t i, float f) {
   float ret = i + f;
   return ret;
}

//
LOCAL double tFundbl(uint32_t i, float f) {
   double ret = i + f;
   return ret;
}
