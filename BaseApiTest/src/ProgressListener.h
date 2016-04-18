/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : ProgressListener.h
 *   Date     : 21.11.2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef CPPUNIT_PROGRESSLISTENER_H
#define CPPUNIT_PROGRESSLISTENER_H

#include <cppunit/TestListener.h>
CPPUNIT_NS_BEGIN
/******************************************************************************/
/** CppUnit-Test Listener to log progress in real time
*/
class CPPUNIT_API ProgressListener : public TestListener {
public:
   ProgressListener();
   virtual ~ProgressListener();

   void startTest(Test *test);
   void addFailure(const TestFailure &failure );
   void endTest(Test *test);
protected:

   // Prevents the use of the copy operator and constructor
   ProgressListener(const ProgressListener &copy);
   void operator =(const ProgressListener &copy);

   bool mLastTestFailed;

};
CPPUNIT_NS_END
#endif  // CPPUNIT_PROGRESSLISTENER_H
