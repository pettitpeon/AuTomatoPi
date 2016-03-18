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
 *  General useful functions\n
 *   - Path functions
 *   - Number parsing
 */
/*------------------------------------------------------------------------------
 */


#ifndef BAUTILS_HPP_
#define BAUTILS_HPP_

#if defined(__cplusplus)
#include <string>
#include <istream>

/******************************************************************************/
/** Namespace to wrap all path functions
 */
namespace BaPath {

/******************************************************************************/
/** Gets everything, including the trailing path separator, except the filename.
 *  - "/foo/bar/baz.txt" --> "/foo/bar/"
 *
 *  @return The directory part of the path (without filename)
 */
static inline std::string GetDirectory(
      std::string path, ///< [in] Full path
#ifdef _WIN32
      char delimiter = '\\'
#else
      char delimiter = '/'
#endif
            ) {
   return path.substr(0, path.find_last_of(delimiter) + 1);
}

/******************************************************************************/
/** Gets only the filename part of the path.
 *  - "/foo/bar/baz.txt" --> "baz.txt"
 *
 *  @return The filename including the extension
 */
static inline std::string GetFilename(
      std::string path, ///< [in] Full path
#ifdef _WIN32
      char delimiter = '\\'
#else
      char delimiter = '/'
#endif
            ) {
   return path.substr(path.find_last_of(delimiter) + 1);
}

/******************************************************************************/
/** Gets the extension of a path. The period is considered part of the
 *  extension.
 *  - "/foo/bar/baz.txt" --> ".txt"
 *  - "/foo/bar/baz" --> ""
 *
 *  @return The extension
 */
static inline std::string GetFileExtension(
      std::string path, ///< [in] Full path
#ifdef _WIN32
      char delimiter = '\\'
#else
      char delimiter = '/'
#endif
            ) {
   std::string filename = GetFilename(path, delimiter);
   std::string::size_type n = filename.find_last_of('.');

   // if n == npos, it was not found. Return ""
   if (n == std::string::npos) {
      return "";
   }

   return filename.substr(n);
}

/******************************************************************************/
/** Changes the extension of a path. The period is considered part of the
 *  extension.
 *  - "/foo/bar/baz.txt", ".dat" --> "/foo/bar/baz.dat"
 *  - "/foo/bar/baz.txt", "" --> "/foo/bar/baz"A
 *  - "/foo/bar/baz", ".txt" --> "/foo/bar/baz.txt"
 *
 *  @return The path with the new extension
 */
static inline std::string ChangeFileExtension(
      std::string path, ///< [in] Full path
      std::string ext,  ///< [in] Extension to replace
#ifdef _WIN32
      char delimiter = '\\'
#else
      char delimiter = '/'
#endif
            ) {
   std::string filename = GetFilename(path, delimiter);
   return GetDirectory(path, delimiter)
         + filename.substr(0, filename.find_last_of('.'))
         + ext;
}

}  // namespace BaPath

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
   } catch (...) {
      try {
         return std::stoul(s, 0, 0);
      } catch (...) {
         if (pError) {
            *pError = true;
         }
         return def;
      }

      if (pError) {
         *pError = true;
      }
      return def;
   }
}

template<>
inline uint32_t BaToNumber<uint32_t>(std::string s, uint32_t def, bool *pError) {
   return BaToNumber(s, (int32_t) def, pError);
}

template<>
inline int64_t BaToNumber<int64_t>(std::string s, int64_t def, bool *pError) {
   try {
      return std::stoll(s, 0, 0);
   } catch (...) {
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
   } catch (...) {
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
   } catch (...) {
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
   } catch (...) {
      if (pError) {
         *pError = true;
      }
      return def;
   }
}

#endif // __cplusplus


#endif /* BAUTILS_HPP_ */
