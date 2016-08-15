/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaSwOsciTest.cpp
 *   Date     : Aug 15, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#include <iostream>
#include "BaSwOsciTest.h"
#include "impl/CBaSwOsci.h"
#include "BaGenMacros.h"
#include "CppU.h"

CPPUNIT_TEST_SUITE_REGISTRATION( CBaSwOsciTest );

/* ****************************************************************************/
/*  ...
 */
void CBaSwOsciTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaSwOsciTest::tearDown() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaSwOsciTest::Test() {
   CBaSwOsci *p = CBaSwOsci::Create();
   ASS(p);
   double d = 3.14e-5;
   uint64_t llu = 999999999999;
   uint16_t usi = 300;
   int8_t c = 56;

   p->Register(&d, eBaSwOsci_double, "d", "double");
   p->Register(&llu, eBaSwOsci_uint64, "llu", "unsigned ll");
   p->Register(&usi, eBaSwOsci_uint16, "usi", "unsigned s");
   p->Register(&c, eBaSwOsci_int8, "c", "signed char");

   p->Header();
   p->Sample();
   d *= 2;
   llu *= 2;
   usi *= 2;
   c *= 2;
   p->Sample();


}

