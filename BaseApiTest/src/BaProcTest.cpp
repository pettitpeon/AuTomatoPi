/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaProcTest.cpp
 *   Date     : Jun 28, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  TODO:
 */
/*------------------------------------------------------------------------------
 */
#include <iostream>
#include "BaProcTest.h"
#include "BaGenMacros.h"
#include "BaProc.h"
#include "CppU.h"
#include "BaUtils.hpp"

#if __WIN32
# define PIDPATH "C:\\var\\run\\BaseApi\\"
#else
# define PIDPATH "/var/run/BaseApi/"
#endif
#define PIDEXT ".pid"

CPPUNIT_TEST_SUITE_REGISTRATION( CBaProcTest );

/* ****************************************************************************/
/*  ...
 */
void CBaProcTest::setUp() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaProcTest::tearDown() {
}

/* ****************************************************************************/
/*  ...
 */
void CBaProcTest::Init() {
#ifdef __WIN32
   if (!BaFS::Exists("C:\\var")) {
      BaFS::MkDir("C:\\var");
   }

   if (!BaFS::Exists("C:\\var\\run")) {
         BaFS::MkDir("C:\\var\\run");
      }
#endif

}

/* ****************************************************************************/
/*  ...
 */
void CBaProcTest::Test() {

   char buf[BAPROC_SHORTNAMELEN];
   ASS(BaProcGetOwnShortName());
   ASS(BaProcGetOwnFullName());
   std::cout << BaProcGetOwnShortName() << std::endl;
   std::cout << BaProcGetOwnFullName() << std::endl;
   ASS(BaProcWriteCtrlTaskPidFile());
   ASS(BaProcReadCtrlTaskPidFile(buf));
   std::cout << buf << std::endl;
   std::string nameFromPID(buf);
   ASS(nameFromPID == BaProcGetOwnShortName());
   ASS(BaProcDelCtrlTaskPidFile());
   ASS(!BaFS::Exists(PIDPATH "BaseApiCtrlTask"));

}

/* ****************************************************************************/
/*  ...
 */
void CBaProcTest::PIDFiles() {
   std::string path = PIDPATH +
         BaPath::ChangeFileExtension(BaProcGetOwnShortName(), PIDEXT);

   ASS(BaProcWriteOwnPidFile());
   ASS(BaProcReadPidFile(BaProcGetOwnShortName(), eBaBool_true));
   ASS(BaProcReadPidFile(path.c_str(), eBaBool_false));
   ASS(!BaProcReadPidFile("BadApp", eBaBool_true));
   ASS(!BaProcReadPidFile("BadApp", eBaBool_false));

   ASS(BaProcDelPidFile(BaProcGetOwnShortName(), eBaBool_true));
   ASS(BaProcWriteOwnPidFile());
   ASS(BaProcWriteOwnPidFile());
   ASS(BaProcDelPidFile(path.c_str(), eBaBool_false));
   ASS(!BaProcDelPidFile(BaProcGetOwnShortName(), eBaBool_true));
   ASS(!BaProcDelPidFile(path.c_str(), eBaBool_false));
}


