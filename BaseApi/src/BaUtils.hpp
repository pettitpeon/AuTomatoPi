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
#include <string>
#include <istream>

/******************************************************************************/
/** Converts string to number
 *  @return Parsed number on success, otherwise def
 */
template<typename T>
static inline T BaToNumber(
      std::string s,   ///< [in] string
      T def,           ///< [in] default to return if conversion fails
      bool *pError = 0 ///< [out] Optional error flag
      ) {
   std::istringstream i(s);
   T res;
   if (!(i >> res)) {
      if (pError) {
         *pError = true;
      }
      return def;
   }

   return res;
}

/*------------------------------------------------------------------------------
    Specializations
 -----------------------------------------------------------------------------*/
template<>
inline int32_t BaToNumber<int32_t>(std::string s, int32_t def, bool *pError) {
   try {
      return std::stoi(s, 0, 0);
   }
   catch (...) {
      if (pError) {
         *pError = true;
      }
      return def;
   }
}

template<>
inline uint32_t BaToNumber<uint32_t>(std::string s, uint32_t def, bool *pError) {
   try {
      return std::stoul(s, 0, 0);
   }
   catch (...) {
      if (pError) {
         *pError = true;
      }
      return def;
   }
}

template<>
inline int64_t BaToNumber<int64_t>(std::string s, int64_t def, bool *pError) {
   try {
      return std::stoll(s, 0, 0);
   }
   catch (...) {
      if (pError) {
         *pError = true;
      }
      return def;
   }
}

template<>
inline uint64_t BaToNumber<uint64_t>(std::string s, uint64_t def, bool *pError) {
   try {
      return std::stoll(s, 0, 0);
   }
   catch (...) {
      if (pError) {
         *pError = true;
      }
      return def;
   }
}

template<>
inline float BaToNumber<float>(std::string s, float def, bool *pError) {
   try {
      return std::stof(s, 0);
   }
   catch (...) {
      if (pError) {
         *pError = true;
      }
      return def;
   }
}

template<>
inline double BaToNumber<double>(std::string s, double def, bool *pError) {
   try {
      return std::stof(s, 0);
   }
   catch (...) {
      if (pError) {
         *pError = true;
      }
      return def;
   }
}

#endif // __cplusplus


#endif /* BAUTILS_HPP_ */
