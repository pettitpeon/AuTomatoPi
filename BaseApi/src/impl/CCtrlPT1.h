/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CCtrlPT1.h
 *   Date     : Apr 26, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef CCTRLPT1_H_
#define CCTRLPT1_H_

/*------------------------------------------------------------------------------
 *  Includes
 */
#include "CtrlPT1.h"


/*------------------------------------------------------------------------------
 *  Type definitions
 */


/*------------------------------------------------------------------------------
 *  C interface
 */
#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
} // extern c

/*------------------------------------------------------------------------------
 *  C++ Interface
 */
class CCtrlPT1 : public ICtrlPT1 {
public:

   static CCtrlPT1* Create(
         float tConstS, // 1/(2*pi*fc) min() = 2*sampTimeS
         float sampTimeS, //
         float initVal  // (default: 0.0)
   );

   static bool Destroy(
         ICtrlPT1* pHdl
         );

   virtual float Update(float in);
   virtual float Update2(float in, float deltaTS);
   virtual bool Reset(float tConstS, float sampTimeS, float initVal);

   // Typical object oriented destructor must be virtual!
   virtual ~CCtrlPT1() {};

private:

   CCtrlPT1(float tConstS, float sampTimeS, float initVal);

   // Make this object non-copyable.
   // This is important because the implementation is in a dynamic pointer and
   // a deep copy of the implementation is explicitly required
   CCtrlPT1(const CCtrlPT1&);
   CCtrlPT1& operator=(const CCtrlPT1&);

   float mTConstS;
   float mTSampleS;
   float mLastValIn;
   float mLastValOut;
   float mTPt1;
};

#endif // __cplusplus
#endif // CCTRLPT1_H_
