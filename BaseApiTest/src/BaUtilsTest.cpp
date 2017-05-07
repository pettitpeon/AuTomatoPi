/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaUtilsTest.cpp
 *   Date     : 07.05.2017
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
#include "BaUtilsTest.h"
#include "BaGenMacros.h"
#include "BaseApi.h"
#include "CppU.h"
#include "BaUtils.hpp"

#ifdef _WIN32
# define RESPATH CPPU_RESPATH "BaUtilsTest\\"
#else
# define RESPATH CPPU_RESPATH "BaUtilsTest/"
#endif

CPPUNIT_TEST_SUITE_REGISTRATION( CBaUtilsTest );

/* ****************************************************************************/
/*  ...
 */
void CBaUtilsTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaUtilsTest::tearDown() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaUtilsTest::Init() {
   BaFS::MkDir(RESPATH);
}

/* ****************************************************************************/
/*  ...
 */
void CBaUtilsTest::BaFS() {
   ASS(true);

   // MkDir
   ASS_MSG(strerror(errno), !BaFS::MkDir(RESPATH "testDir"));
   ASS(BaFS::Exists(RESPATH "testDir"));
   ASS_MSG(strerror(errno), !rmdir(RESPATH "testDir"));

#ifdef __WIN32
   auto sz = (uint64_t)4200;
#else
   auto sz = (uint64_t)4100;
#endif

   // Create a file
   std::ofstream os(RESPATH "testfile");

   // Exists
   ASS(BaFS::Exists(RESPATH "testfile"));

   // Fill
   for (int i = 0; i < 100; ++i) {
      os << "123456789 123456789 123456789 123456789 \n";
   }
   os.close();

   // Size
   ASS_EQ(BaFS::Size(RESPATH "testfile"), sz);

   // Copy and rename
   BaFS::CpFile(RESPATH "testfile", RESPATH "testfileCp");
   ASS(BaFS::Exists(RESPATH "testfileCp"));
   ASS_EQ(BaFS::Size(RESPATH "testfileCp"), sz);
   ASS_MSG(strerror(errno), !BaFS::Rename(RESPATH "testfileCp", RESPATH "testfileRn"));
   ASS_EQ(BaFS::Size(RESPATH "testfileRn"), sz);

   // Dir size, todo recursive
   ASS_EQ_MSG(strerror(errno), 2*sz, BaFS::DirSize(RESPATH));

   // Remove creations
   ASS_MSG(strerror(errno), !remove(RESPATH "testfile"));
   ASS_MSG(strerror(errno), !remove(RESPATH "testfileRn"));
}

/* ****************************************************************************/
/*  ...
 */
void CBaUtilsTest::Exit() {
   remove(RESPATH);
}
