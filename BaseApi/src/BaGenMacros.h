/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaGenMacros.h
 *   Date     : 23/06/2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  General Macros or generic templated functions. Do not include this header in
 *  another header file, only in implementation files, .c or .cpp.
 */
/*------------------------------------------------------------------------------
 */


#ifndef GENMACROS_H_
#define GENMACROS_H_

#ifdef _NDEBUG
# include "dbg/BaDbgMacros.h"
#endif

/// Used to signalize a local function in the implementation file of a C-API
#define LOCAL static

#if defined(__cplusplus)
/// Generic swap function
#ifndef SWAP
 template<class T>
 inline void SWAP(T &x, T &y) {T dum=x; x=y; y=dum;}
#endif

#else
/// Swap for primitive C integer types. Floats and doubles are excluded
#ifndef SWAP
 #define SWAP(X, Y) do { X ^= Y; Y ^= X; X ^= Y; } while (0)
#endif

#endif // __cplusplus

/// @name Min and max limit macros
//@{
/// Min and max macros for C primitive types
#ifndef MIN
 #define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#endif
#ifndef MAX
 #define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#endif
#ifndef MINMAX
 #define MINMAX(VAR, MI, MA) (MIN(MAX(VAR, MI), MA))
#endif
 //@}

/// Returns a value with the same value as x and the same sign as y
#ifndef SIGN
 #define SIGN(X, Y) (Y < 0 ? (X < 0 ? X : -X) : (X < 0 ? -X : X))
#endif

/// Generic square function
#ifndef SQR
 #define SQR(X) ((X)*(X))
#endif

/// Swap for typed variables
#ifndef SWAPT
 #define SWAPT(T, X, Y) do {T z; z = X; X=Y; Y=z;} while (0)
#endif

/// Float equality with a delta. Requires math.h
#ifndef FLT_EQ
 #define FLT_EQ(FLT1, FLT2, DELTA) (fabs((FLT1) - (FLT2)) < (DELTA))
#endif

/// Roundto the nearest half
#ifndef ROUND_HALF
 #define ROUND_HALF(X) (X < 0) ? \
      ((float(int32_t((X*2)-0.5)))/2.0) : ((float(int32_t((X*2)+0.5)))/2.0)
#endif

/// Returns if min <= val <= max
#ifndef INRANGE
 #define INRANGE(VAL, MIN, MAX) ((MIN) <= (VAL) && (VAL) <= (MAX))
#endif

/// Array size macro
#ifndef ARRAYSIZE
 #define ARRAYSIZE(ARR) (sizeof(ARR) / sizeof(ARR[0]))
#endif

/// Set all fields of an array ARR of size N to VAL
#ifndef SETARR
 #define SETARR(ARR, N, VAL) do { unsigned int i_, n_; for (n_ = (N), i_ = 0; \
    n_ > 0; --n_, ++i_) (ARR)[i_] = (VAL); } while(0)
#endif

/// Set all fields of an array ARR of size N to 0
#ifndef ZEROARR
 #define ZEROARR(ARR, N) SETARR(ARR, N, 0)
#endif

/// Calculates the file name without the path. Eg: MyFile.cpp. Requires string.h
#ifndef FILE_NAME_
 #define FILE_NAME_ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

/// Times a function or code part and prints the time in seconds to console.
/// Requires <chrono>
#ifndef TIME_FUN_
 #define TIME_FUN_(fun) \
    auto start = std::chrono::steady_clock::now(); \
    fun; \
    std::chrono::duration<double> dur = (std::chrono::steady_clock::now() - start); \
    std::cout << #fun << ": " << dur.count() << " s" << std::endl;
#endif

#endif /* GENMACROS_H_ */
