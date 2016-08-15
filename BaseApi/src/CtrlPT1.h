/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CtrlPT1.h
 *   Date     : Aug 11, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef CTRLPT1_H_
#define CTRLPT1_H_

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include "BaBool.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
/// C message handle
typedef void* TCtrlPT1Hdl;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/// @name Factory
//@{
/******************************************************************************/
/** Create factory for ...
 *  @return Handle if success, otherwise, null
 */
TCtrlPT1Hdl CtrlPT1Create();

/******************************************************************************/
/** Destroy and release resources
 *  @return True if success, otherwise, false
 */
TBaBoolRC CtrlPT1Destroy(
      TCtrlPT1Hdl hdl ///< [in] handle to destroy
      );
//@}


#ifdef __cplusplus
} // extern c

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

class ICtrlPT1 {
public:

   /***************************************************************************/
   /** Update the first order delay element with fixed sample time
    *  @return Output of the first order delay PT1: in-->PT1-->out
    */
   virtual float Update(
         float in ///< [in] input value
         ) = 0;

   /***************************************************************************/
   /** Update the first order delay element with variable sample time (deltaTS)
    *  @return Output of the first order delay PT1: in-->PT1-->out
    */
   virtual float Update2(
         float in, ///< [in] input value
         float deltaTS ///< [in] Time delta in s since last call or reset
         ) = 0;

   /***************************************************************************/
   /** Update the first order delay element with variable sample time (deltaTS)
    *  @return @c true if success.
    */
   virtual bool Reset(
         float tConstS, /**< [in] Time constant in s. Has to be < 2 * sampTimeS
         in order to respect the Nyquist sampling theorem */
         float sampTimeS, ///< [in] Sample time in s
         float initVal ///< [in] Initial value
         ) = 0;

   // Typical object oriented destructor must be virtual!
   virtual ~ICtrlPT1() {};
};

/// @name C++ Factory
//@{
/******************************************************************************/
/** Create factory for ...
 *  @return Handle if success, otherwise, null
 */
extern "C" ICtrlPT1 * ICtrlPT1Create(float tConstS, float sampTimeS, float initVal);

/******************************************************************************/
/** Destroy and release resources
 *  @return True if success, otherwise, false
 */
extern "C" TBaBoolRC ICtrlPT1Destroy(
      ICtrlPT1 *pHdl ///< [in] handle to destroy
      );
//@}


#endif // __cplusplus
#endif // CTRLPT1_H_
