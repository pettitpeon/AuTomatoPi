/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaUtils.hpp
 *   Date     : 23/06/2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  General useful functions
 */
/*------------------------------------------------------------------------------
 */


#ifndef BAUTILS_HPP_
#define BAUTILS_HPP_

#if defined(__cplusplus)

///
template<typename T>
static inline T BaToNumber(const char* s, T badInput) {
   std::istringstream i(s);
   T res;
   if (!(i >> res)) {
      return badInput;
   }

   return res;
}

#endif // __cplusplus


#endif /* BAUTILS_HPP_ */
