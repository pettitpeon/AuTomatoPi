/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : OsIpcRegistryTest.cpp
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
#include <OsIpcRegistry.h>
#include "BaGenMacros.h"
#include "BaseApi.h"
#include "CppU.h"
#include "OsIpcRegistryTest.h"

#define LOGFILE "testDef"
#define LOGDIR "/var/log/"
#define SET_FUN(regFun, fun, TYPE) regFun.pFun = (void*) fun; \
      regFun.type = TYPE;

CPPUNIT_TEST_SUITE_REGISTRATION( COsIpcRegistryTest );

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
void COsIpcRegistryTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void COsIpcRegistryTest::tearDown() {
}

/* ****************************************************************************/
/*  ...
 */
void COsIpcRegistryTest::Init() {
   ASS(BaApiInitLoggerDef(LOGFILE));
}

/* ****************************************************************************/
/*  ...
 */
void COsIpcRegistryTest::CppInterfaceFunRegistry() {
   ASS(true);

   // Creation and normal use
   IOsIpcRegistry* pReg = IOsIpcRegistryCreate();
   ASS(pReg);

   TOsIpcRegFun fun;
   TOsIpcFunArg a = {0};
   TOsIpcArg tOut = {0};
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

   ASS(IOsIpcRegistryDestroy(pReg));
}

/* ****************************************************************************/
/*  ...
 */
void COsIpcRegistryTest::CInterfaceFunRegistry() {
   ASS(true);

   // Creation and normal use
   TOsIpcRegistryHdl pReg = OsIpcRegistryCreate();
   ASS(pReg);

   TOsIpcRegFun fun;
   TOsIpcFunArg a = {0};
   TOsIpcArg tOut = {0};
   sInt = 0;

   SET_FUN(fun, funvv, "v:v");
   ASS(OsIpcRegistryRegisterFun(pReg, fun.type, fun));
   ASS(!OsIpcRegistryRegisterFun(0, fun.type, fun));
   ASS(OsIpcRegistryCallFun(pReg, fun.type, a, &tOut));
   ASS(!OsIpcRegistryCallFun(0, fun.type, a, &tOut));
   ASS_EQ(1, sInt);

   SET_FUN(fun, funvi, "v:i");
   ASS(OsIpcRegistryRegisterFun(pReg, fun.type, fun));
   a.a[0].i = 7;
   ASS(OsIpcRegistryCallFun(pReg, fun.type, a, &tOut));
   ASS_EQ(7, sInt);

   SET_FUN(fun, funTT<int32_t>, "i:i");
   ASS(OsIpcRegistryRegisterFun(pReg, fun.type, fun));
   a.a[0].i = 7;
   ASS(OsIpcRegistryCallFun(pReg, fun.type, a, &tOut));
   ASS_EQ(7, tOut.i);

   SET_FUN(fun, funTT<double>, "d:d");
   ASS(OsIpcRegistryRegisterFun(pReg, fun.type, fun));
   a.a[0].d = 7;
   ASS(OsIpcRegistryCallFun(pReg, fun.type, a, &tOut));
   ASS_D_EQ(7.0, tOut.d, 0.0);

   SET_FUN(fun, funTTTTT<int64_t>, "I:IIII");
   ASS(OsIpcRegistryRegisterFun(pReg, fun.type, fun));
   a.a[0].I = 1;
   a.a[1].I = 2;
   a.a[2].I = 3;
   a.a[3].I = 4;
   ASS(OsIpcRegistryCallFun(pReg, fun.type, a, &tOut));
   ASS_EQ(10ll, tOut.I);

   //fundiIfd
   SET_FUN(fun, fundiIfd, "d:iIfd");
   ASS(OsIpcRegistryRegisterFun(pReg, fun.type, fun));
   a.a[0].i = 1;
   a.a[1].I = 2;
   a.a[2].f = 3.3f;
   a.a[3].d = 4.4;
   ASS(OsIpcRegistryCallFun(pReg, fun.type, a, &tOut));
   ASS_D_EQ(10.7, tOut.d, 0.0001);

   ASS(OsIpcRegistryUnregisterFun(pReg, fun.type));
   ASS(!OsIpcRegistryUnregisterFun(0, fun.type));
   ASS(OsIpcRegistryClearFunReg(pReg));
   ASS(!OsIpcRegistryClearFunReg(0));

   ASS(OsIpcRegistryDestroy(pReg));
   ASS(!OsIpcRegistryDestroy(0));
}

/* ****************************************************************************/
/*  ...
 */
void COsIpcRegistryTest::LocalFunRegistry() {
   ASS(true);

   // Creation and normal use
   ASS(OsIpcRegistryLocalInit());

   TOsIpcRegFun fun;
   TOsIpcFunArg a = {0};
   TOsIpcArg tOut = {0};
   sInt = 0;

   SET_FUN(fun, funvv, "v:v");
   ASS(OsIpcRegistryLocalRegisterFun(fun.type, fun));
   ASS(OsIpcRegistryLocalCallFun(fun.type, a, &tOut));
   ASS_EQ(1, sInt);

   SET_FUN(fun, funvi, "v:i");
   ASS(OsIpcRegistryLocalRegisterFun(fun.type, fun));
   a.a[0].i = 7;
   ASS(OsIpcRegistryLocalCallFun(fun.type, a, &tOut));
   ASS_EQ(7, sInt);

   SET_FUN(fun, funTT<int32_t>, "i:i");
   ASS(OsIpcRegistryLocalRegisterFun(fun.type, fun));
   a.a[0].i = 7;
   ASS(OsIpcRegistryLocalCallFun(fun.type, a, &tOut));
   ASS_EQ(7, tOut.i);

   SET_FUN(fun, funTT<double>, "d:d");
   ASS(OsIpcRegistryLocalRegisterFun(fun.type, fun));
   a.a[0].d = 7;
   ASS(OsIpcRegistryLocalCallFun(fun.type, a, &tOut));
   ASS_D_EQ(7.0, tOut.d, 0.0);

   SET_FUN(fun, funTTTTT<int64_t>, "I:IIII");
   ASS(OsIpcRegistryLocalRegisterFun(fun.type, fun));
   a.a[0].I = 1;
   a.a[1].I = 2;
   a.a[2].I = 3;
   a.a[3].I = 4;
   ASS(OsIpcRegistryLocalCallFun(fun.type, a, &tOut));
   ASS_EQ(10ll, tOut.I);

   //fundiIfd
   SET_FUN(fun, fundiIfd, "d:iIfd");
   ASS(OsIpcRegistryLocalRegisterFun(fun.type, fun));
   a.a[0].i = 1;
   a.a[1].I = 2;
   a.a[2].f = 3.3f;
   a.a[3].d = 4.4;
   ASS(OsIpcRegistryLocalCallFun(fun.type, a, &tOut));
   ASS_D_EQ(10.7, tOut.d, 0.0001);

   ASS(OsIpcRegistryLocalUnregisterFun(fun.type));
   ASS(OsIpcRegistryLocalClearFunReg());

   ASS(OsIpcRegistryLocalExit());
}


/* ****************************************************************************/
/*  ...
 */
void COsIpcRegistryTest::CppInterfaceVarRegistry() {
   // Creation and normal use
   IOsIpcRegistry* pReg = IOsIpcRegistryCreate();
   TOsIpcRegVar var;
   TOsIpcRegVarOut varOut;
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

   ASS(IOsIpcRegistryDestroy(pReg));
   ASS(!IOsIpcRegistryDestroy(0));
   strncpy(sStr, "init", 5);
}

/* ****************************************************************************/
/*  ...
 */
void COsIpcRegistryTest::CInterfaceVarRegistry() {
   // Creation and normal use
   TOsIpcRegistryHdl pReg = OsIpcRegistryCreate();
   TOsIpcRegVar var;
   TOsIpcRegVarOut varOut;
   std::string s = "init";
   TBaBool b = eBaBool_false;

   // Set variable structure
   var.pVar = (void*)sStr;
   var.sz = sizeof(strlen(sStr));
   var.wr = eBaBool_true;

   ASS(OsIpcRegistryRegisterVar(pReg, "TestString", &var));

   // Test the return value of the registered var
   ASS(OsIpcRegistryCallVar(pReg, "TestString", &varOut));
   ASS(s == (const char*)varOut.dat.data);

   // Change the var directly and test again
   s = "ini";
   strncpy(sStr, "ini", s.length() + 1);
   ASS(OsIpcRegistryCallVar(pReg, "TestString", &varOut));
   ASS(s == varOut.dat.data);

   // Change through the registered var and check against the direct var
   var.pVar = (void*)"new";
   var.sz = sizeof(strlen("new"));
   ASS(OsIpcRegistrySetVar(pReg, "TestString", &var));
   s = "new";
   ASS(s == sStr);

   // Finally check against the registered var
   var = {0};
   ASS(OsIpcRegistryCallVar(pReg, "TestString", &varOut));
   ASS(s == varOut.dat.data);

   // Test another variable (TBaBool)
   var.pVar = (void*)&b;
   var.sz = sizeof(b);
   var.wr = eBaBool_true;
   ASS(OsIpcRegistryRegisterVar(pReg, "TestBool", &var));
   ASS(OsIpcRegistryCallVar(pReg, "TestBool", &varOut));
   ASS_EQ(b, (TBaBool)varOut.dat.i);
   b = eBaBool_true;
   ASS(OsIpcRegistrySetVar(pReg, "TestBool", &var));
   memcpy(varOut.dat.data, "GARBAGE", sizeof("GARBAGE"));
   ASS(OsIpcRegistryCallVar(pReg, "TestBool", &varOut));
   ASS_EQ(b, (TBaBool)varOut.dat.i);

   // Unregister
   ASS(OsIpcRegistryUnregisterVar(pReg, "TestString"));
   ASS(!OsIpcRegistryCallVar(pReg, "TestString", &varOut));

   // Clear registry
   OsIpcRegistryClearVarReg(pReg);
   ASS(!OsIpcRegistryCallVar(pReg, "TestBool", &varOut));

   ASS(OsIpcRegistryDestroy(pReg));
   ASS(!OsIpcRegistryDestroy(0));
   strncpy(sStr, "init", 5);

}

/* ****************************************************************************/
/*  ...
 */
void COsIpcRegistryTest::LocalVarRegistry() {
   // Creation and normal use
   ASS(OsIpcRegistryLocalInit());
   TOsIpcRegVar var;
   TOsIpcRegVarOut varOut;
   std::string s = "init";
   TBaBool b = eBaBool_false;

   // Set variable structure
   var.pVar = (void*)sStr;
   var.sz = sizeof(strlen(sStr));
   var.wr = eBaBool_true;

   ASS(OsIpcRegistryLocalRegisterVar("TestString", &var));

   // Test the return value of the registered var
   ASS(OsIpcRegistryLocalCallVar("TestString", &varOut));
   ASS(s == (const char*)varOut.dat.data);

   // Change the var directly and test again
   s = "ini";
   strncpy(sStr, "ini", s.length() + 1);
   ASS(OsIpcRegistryLocalCallVar("TestString", &varOut));
   ASS(s == varOut.dat.data);

   // Change through the registered var and check against the direct var
   var.pVar = (void*)"new";
   var.sz = sizeof(strlen("new"));
   ASS(OsIpcRegistryLocalSetVar("TestString", &var));
   s = "new";
   ASS(s == sStr);

   // Finally check against the registered var
   var = {0};
   ASS(OsIpcRegistryLocalCallVar("TestString", &varOut));
   ASS(s == varOut.dat.data);

   // Test another variable (TBaBool)
   var.pVar = (void*)&b;
   var.sz = sizeof(b);
   var.wr = eBaBool_true;
   ASS(OsIpcRegistryLocalRegisterVar("TestBool", &var));
   ASS(OsIpcRegistryLocalCallVar("TestBool", &varOut));
   ASS_EQ(b, (TBaBool)varOut.dat.i);
   b = eBaBool_true;
   ASS(OsIpcRegistryLocalSetVar("TestBool", &var));
   memcpy(varOut.dat.data, "GARBAGE", sizeof("GARBAGE"));
   ASS(OsIpcRegistryLocalCallVar("TestBool", &varOut));
   ASS_EQ(b, (TBaBool)varOut.dat.i);

   // Unregister
   ASS(OsIpcRegistryLocalUnregisterVar("TestString"));
   ASS(!OsIpcRegistryLocalCallVar("TestString", &varOut));

   // Clear registry
   OsIpcRegistryLocalClearVarReg();
   ASS(!OsIpcRegistryLocalCallVar("TestBool", &varOut));

   ASS(OsIpcRegistryLocalExit());
   strncpy(sStr, "init", 5);
}

/* ****************************************************************************/
/*  ...
 */
void COsIpcRegistryTest::Exit() {
   ASS(BaApiExitLogger());
   remove(LOGDIR LOGFILE ".log");
}
