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
   THwPiBoard bi = {0};

#ifdef __arm__
   ASS(HwPiGetBoardInfo(&bi));
   ASS(bi.boardModel != eHwPiModelUnknown);
   ASS(HwPiGetBoardModel() != eHwPiModelUnknown);
   ASS(bi.hardware);
   ASS(bi.revision);
   ASS(bi.serial);

   std::cout << "Hardware: " << bi.hardware << std::endl;
   std::cout << "Revision: " << bi.revision << std::endl;
   std::cout << "Serial: " << bi.serial << std::endl;
#else
   ASS(!HwPiGetBoardInfo(&bi));
   ASS(HwPiGetBoardModel() == eHwPiModelUnknown);
#endif
}
