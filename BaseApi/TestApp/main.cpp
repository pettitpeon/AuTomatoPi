/*------------------------------------------------------------------------------
 *                             (c) 2017 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : main.cpp
 *   Date     : 06.10.2020
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  Example application
 */
/*------------------------------------------------------------------------------
 */

#include "BaseApi.h"
#include "BaLogMacros.h"

#define TAG "Main"
int main() {
    BaApiInitLoggerDef("AuTomatoesLog");
    TRACE_("Main exit");
    BaApiExitLogger();
    return 0;
}

