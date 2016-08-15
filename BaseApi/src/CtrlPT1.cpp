/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CtrlPT1.cpp
 *   Date     : Aug 11, 2016
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include "CtrlPT1.h"
#include "CCtrlPT1.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define C_HDL_ ((ICtrlPT1*) hdl)

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
//
TCtrlPT1Hdl CtrlPT1Create(float tConstS, float sampTimeS, float initVal) {
   return (void*) CCtrlPT1::Create(tConstS, sampTimeS, initVal);
}

//
TBaBoolRC CtrlPT1Destroy(TCtrlPT1Hdl hdl) {
   return CCtrlPT1::Destroy(C_HDL_) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
float CtrlPT1Update(TCtrlPT1Hdl hdl, float in) {
   if (!hdl) {
      return 0;
   }

   return C_HDL_->Update(in);
}

//
float CtrlPT1UpdateVarSampT(TCtrlPT1Hdl hdl, float in, float deltaTS) {
   if (!hdl) {
      return 0;
   }

   return C_HDL_->UpdateVarSampT(in, deltaTS);
}

TBaBoolRC CtrlPT1Reset(TCtrlPT1Hdl hdl, float tConstS, float sampTimeS, float initVal) {
   if (!hdl) {
      return eBaBoolRC_Error;
   }

   return C_HDL_->Reset(tConstS, sampTimeS, initVal) ? eBaBoolRC_Success : eBaBoolRC_Error;
}


/*------------------------------------------------------------------------------
    C++ Factories
 -----------------------------------------------------------------------------*/
//
ICtrlPT1 * ICtrlPT1Create(float tConstS, float sampTimeS, float initVal) {
   return CCtrlPT1::Create(tConstS, sampTimeS, initVal);
}

//
TBaBoolRC ICtrlPT1Destroy(ICtrlPT1 *pHdl) {
   return CCtrlPT1::Destroy(pHdl) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
