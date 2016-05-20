/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaTmpTime.h
 *   Date     : May 20, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BATMPTIME_HPP_
#define BATMPTIME_HPP_

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <time.h>
#include <string>
#include <chrono>


// put_time is not implemented yet in 4.9.2 thus the tmp NS
namespace tmp_4_9_2 {
tm inline localtime(const std::time_t& rTime) {
   std::tm tm_snapshot;
#ifdef __WIN32
   localtime_s(&tm_snapshot, &rTime);
#else
   localtime_r(&rTime, &tm_snapshot); // POSIX
#endif
   return tm_snapshot;
}


// To simplify things the return value is just a string. I.e. by design!
std::string inline put_time(const std::tm* pDateTime, const char* cTimeFormat) {
   const size_t size = 1024;
   char buffer[size];

   if (!std::strftime(buffer, size, cTimeFormat, pDateTime)) {
      return cTimeFormat;
   }

   return buffer;
}
} // NS tmp_4_9_2
/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/

#endif // BATMPTIME_HPP_
