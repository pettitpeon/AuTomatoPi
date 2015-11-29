/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaBool.h
 *   Date     : 23/06/2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *   General purpose pure C boolean type
 */
/*------------------------------------------------------------------------------
 */

#ifndef BABOOL_H_
#define BABOOL_H_

#include <stdint.h>

/// General purpose pure C boolean type
typedef int8_t  TBaBool;
typedef enum EBaBool {
   eBaBool_false = 0,
   eBaBool_true  = 1,
} EBaBool;

/// General purpose pure C boolean return code type
typedef TBaBool  TBaBoolRC;
typedef enum EBaBoolRC {
   eBaBoolRC_Error   = eBaBool_false,
   eBaBoolRC_Success = eBaBool_true,
} EBaBoolRC;

#endif /* BABOOL_H_ */
