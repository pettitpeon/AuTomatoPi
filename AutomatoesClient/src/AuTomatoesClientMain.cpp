/*------------------------------------------------------------------------------
 *                             (c) 2017 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : AuTomatoesMain.cpp
 *   Date     : 22.05.2017
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  Example application
 */
/*------------------------------------------------------------------------------
 */

#include <iostream>
#include "BaseApi.h"
#include "Appl.h"

#include "BaLogMacros.h"
#include "OsProc.h"

#define TAG "Main"


int main() {
	BaApiInitLoggerDef("AuTomatoesClientLog");

	TOsProcCtrlTaskOpts ctrlOpts = {0};
	ctrlOpts.name = "AuTomatoesClient";
	ctrlOpts.prio = eBaCorePrio_RT_Normal;
	ctrlOpts.cyleTimeUs = 2000000; // 2s

	// Callbacks
	ctrlOpts.init = ApplInit;
	ctrlOpts.initArg = &ctrlOpts;
	ctrlOpts.update = ApplUpd;
	ctrlOpts.exit = ApplExit;

	OsProcStartCtrlThread(&ctrlOpts);

	BaCoreSleep(5000);
	OsProcStopCtrlThread();
	TRACE_("Main exit");
	BaApiExitLogger();
	return 0;
}


