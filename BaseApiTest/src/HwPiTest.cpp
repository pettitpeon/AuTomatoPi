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
#include "BaGenMacros.h"
#include "CppU.h"
#include "HwPi.h"
#include "HwPiTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( CHwPiTest );

/* ****************************************************************************/
/*  ...
 */
void CHwPiTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CHwPiTest::tearDown() {
}

/* ****************************************************************************/
/*  ...
 */
void CHwPiTest::Test() {
   THwPiBoard bi;

#ifdef __arm__
   ASS(HwPiGetBoardInfo(&bi));
   ASS(bi.boardModel != eHwPiModelUnknown);
   ASS(HwPiGetBoardModel() != eHwPiModelUnknown);
#else
   ASS(!HwPiGetBoardInfo(&bi));
   ASS(HwPiGetBoardModel() == eHwPiModelUnknown);
#endif
}
