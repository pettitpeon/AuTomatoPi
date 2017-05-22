/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : HwPi.h
 *   Date     : Oct 11, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  Raspberry Pi model information
 */
/*------------------------------------------------------------------------------
 */
#ifndef HWPI_H_
#define HWPI_H_

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

/// Model
typedef enum EHwPiModel {
   eHwPiModelUnknown = 0,
   eHwPiModelA,
   eHwPiModelAp,
   eHwPiModelB,
   eHwPiModelBp,
   eHwPiModel2,
   eHwPiModel3
} EHwPiModel;

/// Board Descriptor. Do not modify the strings of this structure
typedef struct THwPiBoard {
   const char* hardware; ///< Hardware version, e.g., BCM2709
   const char* revision; ///< Revision version, e.g., a01041
   const char* serial; ///< Serial version, e.g., 00000000a3428455
   EHwPiModel boardModel;
} THwPiBoard;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/** Get the board information from the RPi
 *  @return True if success, otherwise, false
 */
TBaBool HwPiGetBoardInfo(
      THwPiBoard *pBoardInf ///< [out] Board information
      );

/******************************************************************************/
/** Get the board model from the RPi
 *  @return The board model
 */
EHwPiModel HwPiGetBoardModel();
//@}


#ifdef __cplusplus
}
#endif
#endif // HWPI_H_
