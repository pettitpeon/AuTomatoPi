/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CppU.h
 *   Date     : 14.10.2015
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  Defines for unit tests
 */
/*------------------------------------------------------------------------------
 */
#ifndef CPPU_H_
#define CPPU_H_

#ifdef _WIN32
# define CPPU_RESPATH "res\\"
#else
# define CPPU_RESPATH "res/"
#endif

// Shorter names
#define ASS             CPPUNIT_ASSERT
#define ASS_MSG         CPPUNIT_ASSERT_MESSAGE
#define ASS_EQ          CPPUNIT_ASSERT_EQUAL

#endif /* CPPU_H_ */
