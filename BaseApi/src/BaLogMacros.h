/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaLogMacros.h
 *   Date     : May 20, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  This header defines macros for logging more easily. They expect that TAG
 *  is previously defined
 */
/*------------------------------------------------------------------------------
 */
#ifndef BALOGMACROS_H_
#define BALOGMACROS_H_
#include "BaseApi.h"

/// @name Log Macros
//@{
/// Log with the given priority analog to @c printf() family. This assumes
/// that TAG is defined. see @c BaApiLogF()
#define TRACE_(fmt, ...) BaApiLogF(eBaLogPrio_Trace, TAG, fmt, ##__VA_ARGS__)
#define WARN_(fmt, ...) BaApiLogF(eBaLogPrio_Warning, TAG, fmt, ##__VA_ARGS__)
#define ERROR_(fmt, ...) BaApiLogF(eBaLogPrio_Error, TAG, fmt, ##__VA_ARGS__)
//@}

#endif // BALOGMACROS_H_
