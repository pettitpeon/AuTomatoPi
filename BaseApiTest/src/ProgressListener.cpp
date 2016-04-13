
#include <string>
#include <string.h>
#include <iostream>
#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>
#include <cppunit/Exception.h>
#include <cppunit/SourceLine.h>
#include "ProgressListener.h"

#define FILE_NAME_(f) (strrchr(f, '/') ? strrchr(f, '/') + 1 : f)
#define STARTSTR "|vvvvvvvvvvvvvvvvvvvv  Begin vvvvvvvvvvvvvvvvvvvv|\n"
#define ENDSTR   "|^^^^^^^^^^^^^^^^^^^^   End  ^^^^^^^^^^^^^^^^^^^^|\n"


CPPUNIT_NS_BEGIN

//
ProgressListener::ProgressListener() : mLastTestFailed( false ) {
}

//
ProgressListener::~ProgressListener() {
}


void ProgressListener::startTest(Test *test) {
  std::cout << std::endl << STARTSTR "<" << test->getName() << ">\n";
  std::cout.flush();

  mLastTestFailed = false;
}

void ProgressListener::addFailure(const TestFailure &failure) {
   if (failure.isError()) {
      if (failure.thrownException()) {
         std::cout <<"\nERROR: " << failure.thrownException()->what() << "\n" ENDSTR;
      } else {
         std::cout << "\nERROR" << ENDSTR;
      }
   } else {
   std::cout << "\nASSERTION: " << FILE_NAME_(failure.sourceLine().fileName().c_str())
             << ":" << failure.sourceLine().lineNumber() << "\n" ENDSTR;
   }

   mLastTestFailed  = true;
}

void ProgressListener::endTest(Test *test) {
   if ( !mLastTestFailed ) {
     std::cout << "\nOK\n" << ENDSTR;
   }
}

CPPUNIT_NS_END
