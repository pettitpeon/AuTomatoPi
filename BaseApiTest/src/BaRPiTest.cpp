/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaRPiTest.cpp
 *   Date     : Oct 27, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#include <iostream>
#include "BaRPiTest.h"
#include "BaGenMacros.h"
#include "CppU.h"
#include "BaRPi.h"

CPPUNIT_TEST_SUITE_REGISTRATION( CBaRPiTest );

/* ****************************************************************************/
/*  ...
 */
void CBaRPiTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaRPiTest::tearDown() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaRPiTest::Test() {
   TBaPiBoard bi;

#ifdef __arm__
   ASS(BaPiGetBoardInfo(&bi));
   ASS(bi.boardModel != eBaPiModelUnknown);
   ASS(BaPiGetBoardModel() != eBaPiModelUnknown);
#else
   ASS(!BaPiGetBoardInfo(&bi));
   ASS(BaPiGetBoardModel() == eBaPiModelUnknown);
#endif
}
