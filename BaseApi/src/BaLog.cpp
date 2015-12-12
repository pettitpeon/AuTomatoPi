/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaLog.cpp
 *   Date     : Dec 9, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */

#ifdef __linux

#include <syslog.h>
#include "BaLog.h"


#define TAG "BaLog"


void BaLogInit() {
   openlog(TAG, LOG_CONS, LOG_USER);
   syslog(LOG_INFO, "%s", "Init syslog");
   closelog();
}

#endif // __linux
