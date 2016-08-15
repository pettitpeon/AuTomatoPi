/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CCTrlPT1.cpp
 *   Date     : Apr 26, 2016
 *------------------------------------------------------------------------------
 */


#include <stdarg.h>
#include <stdio.h>
#include <math.h>

#include "CCtrlPT1.h"
#include "BaUtils.hpp"

//
CCtrlPT1::CCtrlPT1(float tConstS, float sampTimeS, float initVal) :
      mTConstS(tConstS), mTSampleS(sampTimeS), mLastValIn(initVal),
      mLastValOut(initVal), mTPt1(exp(-sampTimeS/tConstS)) {};
//
CCtrlPT1* CCtrlPT1::Create(float tConstS, float sampTimeS, float initVal) {
   if (sampTimeS <= 0 || tConstS <= 0 || tConstS <= 2 * sampTimeS) {
      return 0;
   }

   return new CCtrlPT1(tConstS, sampTimeS, initVal);
}

//
bool CCtrlPT1::Destroy(ICtrlPT1* pHdl) {
   CCtrlPT1 *p = dynamic_cast<CCtrlPT1*>(pHdl);
   if (!p ) {
      return false;
   }

   delete p;
   return true;
}

//
float CCtrlPT1::Update(float in) {
   mLastValOut *= mTPt1;
   mLastValOut += (1.0f - mTPt1) * mLastValIn;
   mLastValIn   = in;
   return mLastValOut;
}

float CCtrlPT1::UpdateVarSampT(float in, float deltaTS) {
   float tmpTPt1 = exp(-deltaTS/mTConstS);
   mLastValOut *= tmpTPt1;
   mLastValOut += (1.0f - tmpTPt1) * mLastValIn;
   mLastValIn   = in;
   return mLastValOut;
}

//
bool CCtrlPT1::Reset(float tConstS, float sampTimeS, float initVal) {
   if (sampTimeS <= 0 || tConstS <= 0 || tConstS <= 2 * sampTimeS) {
      return false;
   }
   mTConstS    = tConstS;
   mTSampleS   = sampTimeS;
   mLastValIn  = initVal;
   mLastValOut = initVal;
   return true;
}



