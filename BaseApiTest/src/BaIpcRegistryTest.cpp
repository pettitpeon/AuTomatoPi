/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaIpcRegistryTest.cpp
 *   Date     : 19.04.2017
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#include <string.h>
#include <iostream>
#include "BaIpcRegistryTest.h"
#include "BaGenMacros.h"
#include "BaseApi.h"
#include "CppU.h"
#include "BaIpcRegistry.h"

#define LOGFILE "testDef"
#define LOGDIR "/var/log/"
#define SET_FUN(regFun, fun, TYPE) regFun.pFun = (void*) fun; \
      regFun.type = TYPE;

CPPUNIT_TEST_SUITE_REGISTRATION( CBaIpcRegistryTest );

static int32_t sInt = 0;
static char sStr[] = "init";

LOCAL void funvv() { sInt++; } ;
LOCAL void funvi(int32_t i) { sInt = i; };
LOCAL double fundiIfd(int32_t i, int64_t I, float f, double d)
   { return i + I + f + d; };
template<typename T> LOCAL T funTT(T TT) {return TT; };
template<typename T> LOCAL T funTTTTT(T T1, T T2, T T3, T T4)
   { return T1 + T2 + T3 + T4; };



/* ****************************************************************************/
/*  ...
 */
void CBaIpcRegistryTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaIpcRegistryTest::tearDown() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaIpcRegistryTest::Init() {
   ASS(BaApiInitLoggerDef(LOGFILE));
}

/* ****************************************************************************/
/*  ...
 */
void CBaIpcRegistryTest::CppInterfaceFunRegistry() {
   ASS(true);

   // Creation and normal use
   IBaIpcRegistry* pReg = IBaIpcRegistryCreate();
   ASS(pReg);

   TBaIpcRegFun fun;
   TBaIpcFunArg a = {0};
   TBaIpcArg tOut = {0};
   sInt = 0;

   SET_FUN(fun, funvv, "v:v");
   ASS(pReg->RegisterFun(fun.type, fun));
   ASS(pReg->CallFun(fun.type, a, &tOut));
   ASS_EQ(1, sInt);

   SET_FUN(fun, funvi, "v:i");
   ASS(pReg->RegisterFun(fun.type, fun));
   a.a[0].i = 7;
   ASS(pReg->CallFun(fun.type, a, &tOut));
   ASS_EQ(7, sInt);

   SET_FUN(fun, funTT<int32_t>, "i:i");
   ASS(pReg->RegisterFun(fun.type, fun));
   a.a[0].i = 7;
   ASS(pReg->CallFun(fun.type, a, &tOut));
   ASS_EQ(7, tOut.i);

   SET_FUN(fun, funTT<double>, "d:d");
   ASS(pReg->RegisterFun(fun.type, fun));
   a.a[0].d = 7;
   ASS(pReg->CallFun(fun.type, a, &tOut));
   ASS_D_EQ(7.0, tOut.d, 0.0);

   SET_FUN(fun, funTTTTT<int64_t>, "I:IIII");
   ASS(pReg->RegisterFun(fun.type, fun));
   a.a[0].I = 1;
   a.a[1].I = 2;
   a.a[2].I = 3;
   a.a[3].I = 4;
   ASS(pReg->CallFun(fun.type, a, &tOut));
   ASS_EQ(10ll, tOut.I);

   //fundiIfd
   SET_FUN(fun, fundiIfd, "d:iIfd");
   ASS(pReg->RegisterFun(fun.type, fun));
   a.a[0].i = 1;
   a.a[1].I = 2;
   a.a[2].f = 3.3f;
   a.a[3].d = 4.4;
   ASS(pReg->CallFun(fun.type, a, &tOut));
   ASS_D_EQ(10.7, tOut.d, 0.0001);

   ASS(pReg->UnregisterFun(fun.type));
   pReg->ClearFunRegistry();

   ASS(IBaIpcRegistryDestroy(pReg));
}

/* ****************************************************************************/
/*  ...
 */
void CBaIpcRegistryTest::CInterfaceFunRegistry() {
   ASS(true);

   // Creation and normal use
   TBaIpcRegistryHdl pReg = BaIpcRegistryCreate();
   ASS(pReg);

   TBaIpcRegFun fun;
   TBaIpcFunArg a = {0};
   TBaIpcArg tOut = {0};
   sInt = 0;

   SET_FUN(fun, funvv, "v:v");
   ASS(BaIpcRegistryRegisterFun(pReg, fun.type, fun));
   ASS(!BaIpcRegistryRegisterFun(0, fun.type, fun));
   ASS(BaIpcRegistryCallFun(pReg, fun.type, a, &tOut));
   ASS(!BaIpcRegistryCallFun(0, fun.type, a, &tOut));
   ASS_EQ(1, sInt);

   SET_FUN(fun, funvi, "v:i");
   ASS(BaIpcRegistryRegisterFun(pReg, fun.type, fun));
   a.a[0].i = 7;
   ASS(BaIpcRegistryCallFun(pReg, fun.type, a, &tOut));
   ASS_EQ(7, sInt);

   SET_FUN(fun, funTT<int32_t>, "i:i");
   ASS(BaIpcRegistryRegisterFun(pReg, fun.type, fun));
   a.a[0].i = 7;
   ASS(BaIpcRegistryCallFun(pReg, fun.type, a, &tOut));
   ASS_EQ(7, tOut.i);

   SET_FUN(fun, funTT<double>, "d:d");
   ASS(BaIpcRegistryRegisterFun(pReg, fun.type, fun));
   a.a[0].d = 7;
   ASS(BaIpcRegistryCallFun(pReg, fun.type, a, &tOut));
   ASS_D_EQ(7.0, tOut.d, 0.0);

   SET_FUN(fun, funTTTTT<int64_t>, "I:IIII");
   ASS(BaIpcRegistryRegisterFun(pReg, fun.type, fun));
   a.a[0].I = 1;
   a.a[1].I = 2;
   a.a[2].I = 3;
   a.a[3].I = 4;
   ASS(BaIpcRegistryCallFun(pReg, fun.type, a, &tOut));
   ASS_EQ(10ll, tOut.I);

   //fundiIfd
   SET_FUN(fun, fundiIfd, "d:iIfd");
   ASS(BaIpcRegistryRegisterFun(pReg, fun.type, fun));
   a.a[0].i = 1;
   a.a[1].I = 2;
   a.a[2].f = 3.3f;
   a.a[3].d = 4.4;
   ASS(BaIpcRegistryCallFun(pReg, fun.type, a, &tOut));
   ASS_D_EQ(10.7, tOut.d, 0.0001);

   ASS(BaIpcRegistryUnregisterFun(pReg, fun.type));
   ASS(!BaIpcRegistryUnregisterFun(0, fun.type));
   ASS(BaIpcRegistryClearFunReg(pReg));
   ASS(!BaIpcRegistryClearFunReg(0));

   ASS(BaIpcRegistryDestroy(pReg));
   ASS(!BaIpcRegistryDestroy(0));
}

/* ****************************************************************************/
/*  ...
 */
void CBaIpcRegistryTest::LocalFunRegistry() {
   ASS(true);

   // Creation and normal use
   ASS(BaIpcRegistryLocalInit());

   TBaIpcRegFun fun;
   TBaIpcFunArg a = {0};
   TBaIpcArg tOut = {0};
   sInt = 0;

   SET_FUN(fun, funvv, "v:v");
   ASS(BaIpcRegistryLocalRegisterFun(fun.type, fun));
   ASS(BaIpcRegistryLocalCallFun(fun.type, a, &tOut));
   ASS_EQ(1, sInt);

   SET_FUN(fun, funvi, "v:i");
   ASS(BaIpcRegistryLocalRegisterFun(fun.type, fun));
   a.a[0].i = 7;
   ASS(BaIpcRegistryLocalCallFun(fun.type, a, &tOut));
   ASS_EQ(7, sInt);

   SET_FUN(fun, funTT<int32_t>, "i:i");
   ASS(BaIpcRegistryLocalRegisterFun(fun.type, fun));
   a.a[0].i = 7;
   ASS(BaIpcRegistryLocalCallFun(fun.type, a, &tOut));
   ASS_EQ(7, tOut.i);

   SET_FUN(fun, funTT<double>, "d:d");
   ASS(BaIpcRegistryLocalRegisterFun(fun.type, fun));
   a.a[0].d = 7;
   ASS(BaIpcRegistryLocalCallFun(fun.type, a, &tOut));
   ASS_D_EQ(7.0, tOut.d, 0.0);

   SET_FUN(fun, funTTTTT<int64_t>, "I:IIII");
   ASS(BaIpcRegistryLocalRegisterFun(fun.type, fun));
   a.a[0].I = 1;
   a.a[1].I = 2;
   a.a[2].I = 3;
   a.a[3].I = 4;
   ASS(BaIpcRegistryLocalCallFun(fun.type, a, &tOut));
   ASS_EQ(10ll, tOut.I);

   //fundiIfd
   SET_FUN(fun, fundiIfd, "d:iIfd");
   ASS(BaIpcRegistryLocalRegisterFun(fun.type, fun));
   a.a[0].i = 1;
   a.a[1].I = 2;
   a.a[2].f = 3.3f;
   a.a[3].d = 4.4;
   ASS(BaIpcRegistryLocalCallFun(fun.type, a, &tOut));
   ASS_D_EQ(10.7, tOut.d, 0.0001);

   ASS(BaIpcRegistryLocalUnregisterFun(fun.type));
   ASS(BaIpcRegistryLocalClearFunReg());

   ASS(BaIpcRegistryLocalExit());
}


/* ****************************************************************************/
/*  ...
 */
void CBaIpcRegistryTest::CppInterfaceVarRegistry() {
   // Creation and normal use
   IBaIpcRegistry* pReg = IBaIpcRegistryCreate();
   TBaIpcRegVar var;
   TBaIpcRegVarOut varOut;
   std::string s = "init";
   TBaBool b = eBaBool_false;

   // Set variable structure
   var.pVar = (void*)sStr;
   var.sz = sizeof(strlen(sStr));
   var.wr = eBaBool_true;

   ASS(pReg->RegisterVar("TestString", var));

   // Test the return value of the registered var
   ASS(pReg->CallVar("TestString", varOut));
   ASS(s == varOut.dat.data);

   // Change the var directly and test again
   s = "ini";
   strncpy(sStr, "ini", s.length() + 1);
   ASS(pReg->CallVar("TestString", varOut));
   ASS(s == varOut.dat.data);

   // Change through the registered var and check against the direct var
   var.pVar = (void*)"new";
   var.sz = sizeof(strlen("new"));
   ASS(pReg->SetVar("TestString", var));
   s = "new";
   ASS(s == sStr);

   // Finally check against the registered var
   var = {0};
   ASS(pReg->CallVar("TestString", varOut));
   ASS(s == varOut.dat.data);

   // Test another variable (TBaBool)
   var.pVar = (void*)&b;
   var.sz = sizeof(b);
   var.wr = eBaBool_true;
   ASS(pReg->RegisterVar("TestBool", var));
   ASS(pReg->CallVar("TestBool", varOut));
   ASS_EQ(b, (TBaBool)varOut.dat.i);
   b = eBaBool_true;
   ASS(pReg->SetVar("TestBool", var));
   memcpy(varOut.dat.data, "GARBAGE", sizeof("GARBAGE"));
   ASS(pReg->CallVar("TestBool", varOut));
   ASS_EQ(b, (TBaBool)varOut.dat.i);

   // Unregister
   ASS(pReg->UnregisterVar("TestString"));
   ASS(!pReg->CallVar("TestString", varOut));

   // Clear registry
   pReg->ClearVarRegistry();
   ASS(!pReg->CallVar("TestBool", varOut));

   ASS(IBaIpcRegistryDestroy(pReg));
   ASS(!IBaIpcRegistryDestroy(0));
   strncpy(sStr, "init", 5);
}

/* ****************************************************************************/
/*  ...
 */
void CBaIpcRegistryTest::CInterfaceVarRegistry() {
   // Creation and normal use
   TBaIpcRegistryHdl pReg = BaIpcRegistryCreate();
   TBaIpcRegVar var;
   TBaIpcRegVarOut varOut;
   std::string s = "init";
   TBaBool b = eBaBool_false;

   // Set variable structure
   var.pVar = (void*)sStr;
   var.sz = sizeof(strlen(sStr));
   var.wr = eBaBool_true;

   ASS(BaIpcRegistryRegisterVar(pReg, "TestString", &var));

   // Test the return value of the registered var
   ASS(BaIpcRegistryCallVar(pReg, "TestString", &varOut));
   ASS(s == (const char*)varOut.dat.data);

   // Change the var directly and test again
   s = "ini";
   strncpy(sStr, "ini", s.length() + 1);
   ASS(BaIpcRegistryCallVar(pReg, "TestString", &varOut));
   ASS(s == varOut.dat.data);

   // Change through the registered var and check against the direct var
   var.pVar = (void*)"new";
   var.sz = sizeof(strlen("new"));
   ASS(BaIpcRegistrySetVar(pReg, "TestString", &var));
   s = "new";
   ASS(s == sStr);

   // Finally check against the registered var
   var = {0};
   ASS(BaIpcRegistryCallVar(pReg, "TestString", &varOut));
   ASS(s == varOut.dat.data);

   // Test another variable (TBaBool)
   var.pVar = (void*)&b;
   var.sz = sizeof(b);
   var.wr = eBaBool_true;
   ASS(BaIpcRegistryRegisterVar(pReg, "TestBool", &var));
   ASS(BaIpcRegistryCallVar(pReg, "TestBool", &varOut));
   ASS_EQ(b, (TBaBool)varOut.dat.i);
   b = eBaBool_true;
   ASS(BaIpcRegistrySetVar(pReg, "TestBool", &var));
   memcpy(varOut.dat.data, "GARBAGE", sizeof("GARBAGE"));
   ASS(BaIpcRegistryCallVar(pReg, "TestBool", &varOut));
   ASS_EQ(b, (TBaBool)varOut.dat.i);

   // Unregister
   ASS(BaIpcRegistryUnregisterVar(pReg, "TestString"));
   ASS(!BaIpcRegistryCallVar(pReg, "TestString", &varOut));

   // Clear registry
   BaIpcRegistryClearVarReg(pReg);
   ASS(!BaIpcRegistryCallVar(pReg, "TestBool", &varOut));

   ASS(BaIpcRegistryDestroy(pReg));
   ASS(!BaIpcRegistryDestroy(0));
   strncpy(sStr, "init", 5);

}

/* ****************************************************************************/
/*  ...
 */
void CBaIpcRegistryTest::LocalVarRegistry() {

}

/* ****************************************************************************/
/*  ...
 */
void CBaIpcRegistryTest::Exit() {
   ASS(BaApiExitLogger());
   remove(LOGDIR LOGFILE ".log");
}
