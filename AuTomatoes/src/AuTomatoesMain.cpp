//============================================================================
// Name        : AuTomatoes.cpp
// Author      : Ivan Peon
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "BaseApi.h"
#include "Appl.h"

#include "BaLogMacros.h"
#include "OsProc.h"

#define TAG "Main"


int main() {
	BaApiInitLoggerDef("AuTomatoesLog");
	TRACE_("========================");
	TRACE_("====== AuTomatoes ======");
	TRACE_("========================");

	TOsProcCtrlTaskOpts ctrlOpts = {0};
	ctrlOpts.name = "AuTomatoes";
	ctrlOpts.prio = eBaCorePrio_RT_Normal;
	ctrlOpts.cyleTimeUs = 1000000; // 1s

	// Callbacks
	ctrlOpts.init = ApplInit;
	ctrlOpts.update = ApplUpd;
	ctrlOpts.exit = ApplExit;

	OsProcStartCtrlTask(&ctrlOpts);
	BaCoreSleep(10);

	TRACE_("Main exit");
	BaApiExitLogger();
	return 0;
}


