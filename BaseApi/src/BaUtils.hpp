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

namespace BaPath {

static inline std::string GetDirectory(
      const std::string& path,
      char delimiter = '/'
            ) {
   //
   // Returns everything, including the trailing path separator, except the filename
   // part of the path.
   //
   // "/foo/bar/baz.txt" --> "/foo/bar/"
   return path.substr(0, path.find_last_of(delimiter) + 1);
}

static inline std::string GetFilename(
      const std::string& path,
      char delimiter = '/'
            ) {
   //
   // Returns only the filename part of the path.
   //
   // "/foo/bar/baz.txt" --> "baz.txt"
   return path.substr(path.find_last_of(delimiter) + 1);
}

static inline std::string GetFileExtension(
      const std::string& path,
      char delimiter = '/'
            ) {
   //
   // Returns the file's extension, if any. The period is considered part
   // of the extension.
   //
   // "/foo/bar/baz.txt" --> ".txt"
   // "/foo/bar/baz" --> ""
   std::string filename = GetFilename(path, delimiter);
   std::string::size_type n = filename.find_last_of('.');
   if (n != std::string::npos)
      return filename.substr(n);
   return std::string();
}

static inline std::string ChangeFileExtension(
      const std::string& path,
      const std::string& ext,
      char delimiter = '/'
            ) {
   //
   // Modifies the filename's extension. The period is considered part
   // of the extension.
   //
   // "/foo/bar/baz.txt", ".dat" --> "/foo/bar/baz.dat"
   // "/foo/bar/baz.txt", "" --> "/foo/bar/baz"
   // "/foo/bar/baz", ".txt" --> "/foo/bar/baz.txt"
   //
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
   } catch (...) {
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
