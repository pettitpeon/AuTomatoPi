/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CtrlPT1Test.cpp
 *   Date     : Aug 12, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#include <iostream>
#include "CtrlPT1Test.h"
#include "impl/CCtrlPT1.h"
#include "BaGenMacros.h"
#include "CppU.h"

#include <stdio.h>

CPPUNIT_TEST_SUITE_REGISTRATION( CCtrlPT1Test );

/* ****************************************************************************/
/*  ...
 */
void CCtrlPT1Test::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CCtrlPT1Test::tearDown() {
}

/* ****************************************************************************/
/*  ...
 */
void CCtrlPT1Test::Test() {
   ICtrlPT1* pPT1 = ICtrlPT1Create(500, 1, 0);
   ASS(pPT1);
   float out1tau = 0.63212;

   // After 1 tau out = 0.63212055882
   for (int i = 0; i < 500; ++i) {
      pPT1->Update(1);
   }
   ASS_D_EQ(out1tau, pPT1->Update(1), 0.001);

   // Reset and test with variable sample time
   pPT1->Reset(500, 1, 0);

   // todo: Test if pure virtual function can be overloaded
   pPT1->Update2(1.0f, 0.0f);
   ASS_D_EQ(out1tau, pPT1->Update2(1, 500), 0.001);

   ASS(ICtrlPT1Destroy(pPT1));
}
