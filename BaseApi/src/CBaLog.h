/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CBaLog.h
 *   Date     : Dec 9, 2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef CBALOG_H_
#define CBALOG_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include <string>

/*------------------------------------------------------------------------------
 *  Type definitions
 */

/*------------------------------------------------------------------------------
 *  C++ Interface
 */
class CBaLog {
public:
   // Factory
   static CBaLog* Create(std::string name);
   static bool Delete (
         CBaLog* hdl
         );

   // Hardware PWM setup functions
   virtual bool Log(std::string msg);

private:
   // Private constructor because a public factory method is used
   CBaLog() : mpImpl(0) {};

   // Typical object oriented destructor must be virtual!
   virtual ~CBaLog() {};

   // Make this object non-copyable.
   // This is important because the implementation is in a dynamic pointer and
   // a deep copy of the implementation is explicitly required
   CBaLog(const CBaLog&);
   CBaLog& operator=(const CBaLog&);

   // IF THIS IS AN EXTERNAL INTERFACE, NO MEMBER VARIABLES!
   // Pimpl idiom (Pointer to Implementation) http://c2.com/cgi/wiki?PimplIdiom
   class Impl;
   Impl *mpImpl;

};


#endif // CBALOG_H_
