/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaPi.h
 *   Date     : Oct 11, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef BARPI_H_
#define BARPI_H_

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
typedef enum EBaPiModel {
   eBaPiModelUnknown = 0,
   eBaPiModelA,
   eBaPiModelAp,
   eBaPiModelB,
   eBaPiModelBp,
   eBaPiModel2,
   eBaPiModel3
} EBaPiModel;

/// Board Descriptor
typedef struct TBaPiBoard {
   const char* hardware;
   const char* revision;
   const char* serial;
   EBaPiModel boardModel;
} TBaPiBoard;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


/// @name Factory

/******************************************************************************/
/** Get the board information from the RPi
 *  @return True if success, otherwise, false
 */
TBaBool BaPiGetBoardInfo(TBaPiBoard *pBoardInf);

/******************************************************************************/
/** Get the board model from the RPi
 *  @return The board model
 */
EBaPiModel BaPiGetBoardModel();

//@}



#ifdef __cplusplus
}
#endif
#endif // BARPI_H_