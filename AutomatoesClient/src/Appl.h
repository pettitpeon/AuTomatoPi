/*------------------------------------------------------------------------------
 *                             (c) 2017 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : Appl.h
 *   Date     : 22.05.2017
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  AuTomatoes application module
 */
/*------------------------------------------------------------------------------
 */
#ifndef APPL_H_
#define APPL_H_

#include "BaBool.h"

TBaBoolRC ApplInit(void *pArg);

void ApplUpd(void *pArg);

TBaBoolRC ApplExit(void *pArg);

#endif // APPL_H_
