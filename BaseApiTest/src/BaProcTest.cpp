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

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "BaProcTest.h"
#include "BaGenMacros.h"
#include "BaProc.h"
#include "CppU.h"
#include "BaUtils.hpp"

#if __WIN32
# define PIDPATH "C:\\var\\run\\BaseApi\\"
# define BINNAME "BaseApiTest.exe"
#else
# define PIDPATH "/var/run/BaseApi/"
# define BINNAME "BaseApiTest"
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
void CBaProcTest::OwnNames() {

   ASS(BaProcGetOwnShortName());
   ASS(BaProcGetOwnFullName());
   std::string nameS = BaProcGetOwnShortName();
   std::string nameF = BaProcGetOwnFullName();
   ASS_MSG(nameS, nameS == BINNAME);
   ASS_MSG(nameF, nameF == BINNAME);
}

/* ****************************************************************************/
/*  ...
 */
void CBaProcTest::CtrlTaskPID() {

   char buf[BAPROC_SHORTNAMELEN];

   // No CtrlTask PID at the beginning
   ASS(!BaProcReadCtrlTaskPidFile(buf));
   ASS(!BaProcReadCtrlTaskPidFile(0));

   // Write task PID
   ASS(BaProcWriteCtrlTaskPidFile());

   // Ctrl Task PID available
   ASS(BaProcReadCtrlTaskPidFile(buf));
   ASS(BaProcReadCtrlTaskPidFile(0));

   std::cout << buf << std::endl;
   ASS(std::string(buf) == BaProcGetOwnShortName());

   ASS(BaFS::Exists(PIDPATH "BaseApiCtrlTask.pid"));
   ASS(BaProcDelCtrlTaskPidFile());
   ASS(!BaFS::Exists(PIDPATH "BaseApiCtrlTask.pid"));
   ASS(!BaProcDelCtrlTaskPidFile());
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
   ASS(!BaProcReadPidFile(0, eBaBool_true));

   // Test external PID files!
   ASS(!BaProcReadPidFile(PIDPATH "BaseApiTest.pid", eBaBool_false));
   ASS(BaProcWriteOwnPidFile());
   ASS(BaProcReadPidFile(PIDPATH "BaseApiTest.pid", eBaBool_false));
   ASS(BaProcDelPidFile(PIDPATH "BaseApiTest.pid", eBaBool_false));
   ASS(!BaProcDelPidFile(PIDPATH "BaseApiTest.pid", eBaBool_false));
   ASS(!BaProcReadPidFile(0, eBaBool_false));
}

/* ****************************************************************************/
/*  ...
 */
void CBaProcTest::NameFromPID() {
   std::string binName = BINNAME;
   char buf[BAPROC_FULLNAMELEN];
   const char *name = BaProcGetPIDName(getpid(), buf);
   ASS(buf == name);
   name = BaProcGetPIDName(getpid(), buf);
   ASS(buf == name);

#ifndef __WIN32
   name = BaProcGetPIDName(getpid(), buf);
   ASS_MSG(name, name == binName);
   name = BaProcGetPIDName(getpid(), 0);
   ASS_MSG(name, name == binName);

   // Test that freeing the mallocated string does not crash
   free((void*) name);
   ASS(!BaProcGetPIDName(0, 0));
   ASS(!BaProcGetPIDName(0, buf));
#endif
}

/* ****************************************************************************/
/*  ...
 */
void CBaProcTest::Prio() {
   ASS(BaProcSetOwnPrio(eBaCorePrio_RT_Highest));
   // if out of range
   ASS(!BaProcSetOwnPrio((EBaCorePrio) (eBaCorePrio_Minimum - 1)));
   ASS(!BaProcSetOwnPrio((EBaCorePrio) (eBaCorePrio_RT_Highest + 1)));

#ifndef __WIN32
   ASS(BaProcSetOwnPrio(eBaCorePrio_RT_Highest));
   ASS_EQ(eBaCorePrio_RT_Highest, BaProcGetOwnPrio());
   ASS(BaProcSetOwnPrio(eBaCorePrio_Normal));
   ASS_EQ(eBaCorePrio_Normal, BaProcGetOwnPrio());
#endif

}
