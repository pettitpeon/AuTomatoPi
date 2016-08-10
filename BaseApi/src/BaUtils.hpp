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
#include <sys/stat.h> // mkdir()
#include <stdio.h> // rename()
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <string>
#include <istream>
#include <fstream>
#include <algorithm>
#include <iterator>

#include "BaBool.h"


/******************************************************************************/
/** Namespace to wrap all file system functions
 */
namespace BaFS {

/// @name Base API File System
//@{
/******************************************************************************/
/** Portable mkdir(), creates a directory
 *  @return Normal mkdir() return value
 */
static inline int MkDir(
      std::string dir, ///< [in] Path of directory to be created
      int per = 0660 ///< [in] Permissions with default read/write for owner and group
      ) {
#ifdef _WIN32
   return mkdir(dir.c_str());
#else
   return mkdir(dir.c_str(), per);
#endif
}

/******************************************************************************/
/** Portable file copy. Always rewrites
 *  @return true if success
 */
static inline TBaBoolRC CpFile(
      std::string src, ///< [in] Path of source file
      std::string dst  ///< [in] Path of destination
      ) {
   std::filebuf fSrc;
   std::ofstream fDst(dst);
   if (fDst.fail() || !fSrc.open(src, std::ios::in | std::ios::binary)) {
      return eBaBoolRC_Error;
   }

   std::istreambuf_iterator<char> begSrc(&fSrc);
   std::istreambuf_iterator<char> endSrc;
   std::ostreambuf_iterator<char> begDst(fDst);
   std::copy(begSrc, endSrc, begDst);

   return fDst.fail() ? eBaBoolRC_Error : eBaBoolRC_Success;
}

/******************************************************************************/
/** Portable rename(). Standardizes the function by ALWAYS overwriting if the
 *  other file exists.
 *  @return Normal rename() return value
 */
static inline int Rename(
      std::string path, ///< [in] Path of file or directory
      std::string newPath ///< [in] New file or directory path
      ) {
#ifdef _WIN32
   remove(newPath.c_str());
#endif
   return rename(path.c_str(), newPath.c_str());
}

/******************************************************************************/
/** Gets the file size in bytes
 *  @return file size in bytes
 */
static inline uint32_t Size(
      std::string path ///< [in] Path of file
      ) {
   struct stat desc;
   if (stat(path.c_str(), &desc) == 0) {
      return desc.st_size;
   }

   return 0;
}

/******************************************************************************/
/** Gets the directory size in bytes recursively with a maximum depth
 *  @return directory size in bytes
 */
static inline uint64_t DirSize(
      std::string path, ///< [in] Path of directory
      uint32_t maxDepth = (uint32_t)-1 ///< [in] Optional maximum depth
      ) {

   // When maximum depth is reached, stop
   if (maxDepth == 0) {
      return 0;
   }

   DIR *d;
   struct dirent *de; // directory entry
   uint64_t total = 0;
   struct stat info;
#ifdef _WIN32
      char del = '\\';
#else
      char del = '/';
#endif

   d = opendir(path.c_str());
   if (!d) {
      return 0;
   }

   std::string file;
   for (de = readdir(d); de != 0; de = readdir(d)) {

      // Skip "." and ".."
      if ((strcmp(de->d_name, "..") == 0) || (strcmp(de->d_name, ".") == 0)) {
         continue;
      }

      // Get the file info
      file = path + de->d_name;
      if(stat( file.c_str(), &info ) == 0) {

         // Check if is directory and recurse
         if( info.st_mode & S_IFDIR ) {
            total += DirSize(file + del, --maxDepth);
         } else {
            total += info.st_size;
         }
      }
   }

   closedir(d);
   return total;
}

/******************************************************************************/
/** Tests if a file or directory exists
 *  @return true if exists, otherwise, false
 */
static inline bool Exists(
      std::string path ///< [in] Path of file or directory
      ) {
   struct stat desc;
   return stat(path.c_str(), &desc) == 0;
}
//@}

} // BaFS


/******************************************************************************/
/** Namespace to wrap all path functions
 */
namespace BaPath {

/// @name Base API Path
//@{
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
 *  - "/foo/bar/baz.txt", "" --> "/foo/bar/baz"
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

/******************************************************************************/
/** Concatenates @c path1 with @c path2 and adds or removes a delimiter when
 *  required. E.g.:
 *  - "/foo/bar",   "baz.txt" --> "/foo/bar/baz.dat"
 *  - "/foo/bar/", "/baz.txt" --> "/foo/bar/baz.dat"
 *
 *  @return The path with the new extension
 */
static inline std::string Concatenate(
      std::string path1, ///< [in] Path part 1
      std::string path2, ///< [in] Path part 2
#ifdef _WIN32
      char delimiter = '\\'
#else
      char delimiter = '/'
#endif
            ) {

   if (path1.back() == delimiter) {
      if (path2[0] == delimiter) {
         path1.resize(path1.length() - 1);
         return path1 + path2;
      }
      return path1 + path2;
   }
   return path1 + delimiter + path2;
}
//@}
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

/******************************************************************************/
/** Creates a string formated C-style with a maximum length of 65534 chars
 *  @return formated string
 */
static inline std::string BaFString(
      const char *fmt, ///< [in] format string
      ...              ///< [in] variable arguments
      ) {

   // Init arguments
   va_list arg;
   va_start(arg, fmt);

   //
   std::string res = BaFString(fmt, arg);

   // Release resources
   va_end(arg);

   // Return generated string
   return res;
}

/******************************************************************************/
/** Creates a string formated C-style with a maximum length of 65534 chars
 *  @return formated string
 */
static inline std::string BaFString(
      const char *fmt, ///< [in] format string
      va_list arg      ///< [in] variable arguments
      ) {

   // Check size
   uint16_t size = vsnprintf(0, 0, fmt, arg) + 1;

   // buffer
   char msg[size];

   // fill out the buffer
   vsnprintf(msg, size, fmt, arg);

   // Return generated string
   return std::string(msg);
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
