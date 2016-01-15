/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaIniParse.h
 *   Date     : Dec 9, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  Ini/config files parser based on iniparser from N. Devillard
 *  TODO:
 *  Check if case insensitive is good
 */
/*------------------------------------------------------------------------------
 */

#ifndef _BAINIPARSE_H_
#define _BAINIPARSE_H_

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/

#include <stdio.h>
#ifdef __cplusplus
# include <string>
#endif

#include "BaBool.h"

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/// C INI parser handle
typedef void* TBaIniParseHdl;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/** Create factory for an INI parser
 *  @return Handle if success, otherwise, null
 */
TBaIniParseHdl BaIniParseCreate(
      const char *file ///< [in] Path to file
      );

/******************************************************************************/
/** Destroy and release resources of INI parser
 *  @return True if success, otherwise, false
 */
TBaBoolRC BaIniParseDestroy(
      TBaIniParseHdl hdl ///< [in] INI parser handle to destroy
      );

/******************************************************************************/
/** Gets string value associated with a key or default if not found
 *  @return the value or def. The returned string must not be edited or freed
 *  and invalidates if the instance handle is destroyed
 */
const char* BaIniParseGetString(
      TBaIniParseHdl hdl, ///< [in] Handle
      const char *key,    ///< [in] Key as "section:key"
      const char *def     ///< [in] Default value to return if not found
      );

/******************************************************************************/
/** Gets integer value associated with a key or default if not found or parse
 *  failed. Supported values for integers include the usual C notation so
 *  decimal, octal (starting with 0) and hexadecimal (starting with 0x) are
 *  supported. Examples:
 *   - "42"   ->  42
 *   - "042"  ->  34 (octal -> decimal)
 *   - "0x42" ->  66 (hexa  -> decimal)
 *
 *  @return the value or def
 */
int BaIniParseGetInt(
      TBaIniParseHdl hdl, ///< [in] Handle
      const char *key,    ///< [in] Key as "section:key"
      int def             ///< [in] Default value to return if not found
      );

/******************************************************************************/
/** Gets double value associated with a key or default if not found or parse
 *  failed. Supported values for double include the usual C notation including
 *  the scientific notation. Examples:
 *   - "3.14"   ->  3.14
 *   - "3.0e-3" ->  0.003
 *
 *  @return the value or def
 */
double BaIniParseGetDouble(
      TBaIniParseHdl hdl, ///< [in] Handle
      const char *key,    ///< [in] Key as "section:key"
      double def          ///< [in] Default value to return if not found
      );

/******************************************************************************/
/** Gets bool value associated with a key or default if not found or parse
 *  failed.
 *  A true is found if one of the following is matched:
 *   - A string starting with 'y'
 *   - A string starting with 'Y'
 *   - A string starting with 't'
 *   - A string starting with 'T'
 *   - A string starting with '1'
 *  A false is found if one of the following is matched:
 *   - A string starting with 'n'
 *   - A string starting with 'N'
 *   - A string starting with 'f'
 *   - A string starting with 'F'
 *   - A string starting with '0'
 *  @return the value or def
 */
TBaBool BaIniParseGetBool(
      TBaIniParseHdl hdl, ///< [in] Handle
      const char *key,    ///< [in] Key as "section:key"
      TBaBool def         ///< [in] Default value to return if not found
      );

/******************************************************************************/
/** Add or overwrite a key-value pair
 *  @return Error or success
 */
TBaBoolRC BaIniParseSet(
      TBaIniParseHdl hdl, ///< [in] Handle
      const char *key,    ///< [in] Key as "section:key"
      const char *val     ///< [in] Value
      );

/******************************************************************************/
/** Reset or erase a key-value pair
 *  @return true if the element was erased
 */
TBaBoolRC BaIniParseReset(
      TBaIniParseHdl hdl, ///< [in] Handle
      const char *key    ///< [in] Key as "section:key"
      );

/******************************************************************************/
/** Gets if the entry exists
 *  @return True if entry exists
 */
TBaBool BaIniParseExists(
      TBaIniParseHdl hdl, ///< [in] Handle
      const char *key     ///< [in] Key as "section:key"
      );

/******************************************************************************/
/** Dumps the entries in loadable INI format
 */
void BaIniParseDumpIni(
      TBaIniParseHdl hdl, ///< [in] Handle
      FILE * f            ///< [in] Opened output file (can also be stdout)
      );

/******************************************************************************/
/** Dump all entries in a particular section in loadable INI format
 *  @return true if section exists
 */
TBaBool BaIniParseDumpIniSec(
      TBaIniParseHdl hdl, ///< [in] Handle
      const char *sec,    ///< [in] Section name
      FILE *f             ///< [in] Opened output file (can also be stdout)
      );

/******************************************************************************/
/** Dumps all section-less entries
 *  @return true if section exists
 */
TBaBool BaIniParseDumpIniSecLess(
      TBaIniParseHdl hdl, ///< [in] Handle
      FILE *f ///< [in] Opened output file (can also be stdout)
      );

/******************************************************************************/
/** Dumps all the raw entries in non-loadable format
 */
void BaIniParseDump(
      TBaIniParseHdl hdl, ///< [in] Handle
      FILE * f            ///< [in] Opened output file (can also be stdout)
      );

#ifdef __cplusplus
}

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

/******************************************************************************/
/** INI parser C++ interface
 */
class IBaIniParser {
public:

   /***************************************************************************/
   /** Gets string value associated with a key or default if not found
    *  @return the value or def
    */
   virtual std::string GetString(
         const char *key, ///< [in] Key as "section:key"
         const char *def  ///< [in] Default value to return if not found
         ) = 0;

   /***************************************************************************/
   /** Gets integer value associated with a key or default if not found or parse
    *  failed. Supported values for integers include the usual C notation so
    *  decimal, octal (starting with 0) and hexadecimal (starting with 0x) are
    *  supported. Examples:
    *   - "42"   ->  42
    *   - "042"  ->  34 (octal -> decimal)
    *   - "0x42" ->  66 (hexa  -> decimal)
    *
    *  @return the value or def
    */
   virtual int GetInt(
         const char *key, ///< [in] Key as "section:key"
         int def          ///< [in] Default value to return if not found
         ) = 0;

   /***************************************************************************/
   /** Gets double value associated with a key or default if not found or parse
    *  failed. Supported values for double include the usual C notation including
    *  the scientific notation. Examples:
    *   - "3.14"   ->  3.14
    *   - "3.0e-3" ->  0.003
    *
    *  @return the value or def
    */
   virtual double GetDouble(
         const char *key, ///< [in] Key as "section:key"
         double def       ///< [in] Default value to return if not found
         ) = 0;

   /***************************************************************************/
   /** Gets bool value associated with a key or default if not found or parse
    *  failed.
    *  A true is found if one of the following is matched:
    *   - A string starting with 'y'
    *   - A string starting with 'Y'
    *   - A string starting with 't'
    *   - A string starting with 'T'
    *   - A string starting with '1'
    *  A false is found if one of the following is matched:
    *   - A string starting with 'n'
    *   - A string starting with 'N'
    *   - A string starting with 'f'
    *   - A string starting with 'F'
    *   - A string starting with '0'
    *  @return the value or def
    */
   virtual bool GetBool(
         const char *key, ///< [in] Key as "section:key"
         bool def         ///< [in] Default value to return if not found
         ) = 0;

   /***************************************************************************/
   /** Add or overwrite a key-value pair
    *  @return Error or success
    */
   virtual bool Set(
         const char *key, ///< [in] Key as "section:key"
         const char *val  ///< [in] Value
         ) = 0;

   /***************************************************************************/
   /** Reset or erase a key-value pair
    *  @return true if the element was erased
    */
   virtual bool Reset(
         const char *key ///< [in] Key as "section:key"
         ) = 0;

   /***************************************************************************/
   /** Gets if the entry exists
    *  @return True if entry exists
    */
   virtual bool Exists(
         const char *key ///< [in] Key as "section:key"
         ) = 0;

   /***************************************************************************/
   /** Dumps the entries in loadable INI format
    */
   virtual void DumpIni(
         FILE * f ///< [in] Opened output file (can also be stdout)
         ) = 0;

   /***************************************************************************/
   /** Dump all entries in a particular section in loadable INI format
    *  @return true if section exists
    */
   virtual bool DumpIniSec(
         const char *sec, ///< [in] Section name
         FILE *f          ///< [in] Opened output file (can also be stdout)
         ) = 0;

   /***************************************************************************/
   /** Dumps all section-less entries
    *  @return true if section exists
    */
   virtual bool DumpIniSecLess(
         FILE *f ///< [in] Opened output file (can also be stdout)
         ) = 0;

   /***************************************************************************/
   /** Dumps all the raw entries in non-loadable format
    */
   virtual void Dump(
         FILE * f ///< [in] Opened output file (can also be stdout)
         ) = 0;


   // In interfaces and abstract classes, ALWAYS declare a virtual destructor,
   // and implement / inline it
   virtual ~IBaIniParser() {};
};

/******************************************************************************/
/** Create factory for an INI parser
 *  @return Handle if success, otherwise, null
 */
extern "C" IBaIniParser * CBaIniParserCreate(
      const char *file ///< [in] Path to file
      );

/******************************************************************************/
/** Destroy and release resources of INI parser
 *  @return True if success, otherwise, false
 */
extern "C" bool CBaIniParserDestroy(
      IBaIniParser *pHdl ///< [in] INI parser handle to destroy
      );

#endif // __cplusplus

#endif // _BAINIPARSE_H_
