/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : HwPi.cpp
 *   Date     : Oct 11, 2016
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <HwPi.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "BaUtils.hpp"
#include "BaGenMacros.h"


/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define CPUINFO_ "/proc/cpuinfo"

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
typedef struct TBoard_ {
   std::string hardware;
   std::string revision;
   std::string serial;
   bool init;
   TBoard_() : hardware(""), revision(""), serial(""), init(false) {};
} TBoard_;

static TBoard_ sPiBrdInt;
static THwPiBoard sPiBoard = {0};

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
LOCAL EHwPiModel getBoardModel();

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/

// http://elinux.org/RPi_HardwareHistory
TBaBoolRC HwPiGetBoardInfo(THwPiBoard *pBoardInf) {
   std::ifstream iS;
   std::string line;
   std::size_t pos = 0;

   if (!pBoardInf) {
      return eBaBoolRC_Error;
   }

   if (sPiBrdInt.init) {
      *pBoardInf = sPiBoard;
      return eBaBoolRC_Success;
   }

   // Open the CPU info file from proc
   iS.open(CPUINFO_);
   if(iS.fail()) {
      return eBaBoolRC_Error;
   }

   // Get hardware, revision and serial from the cpuinfo file
   while (std::getline(iS, line)) {
      pos = line.find("Hardware\t:");
      if (pos != std::string::npos) {
         sPiBrdInt.hardware = line.substr(pos + 11);
         continue;
      }

      pos = line.find("Revision\t:");
      if (pos != std::string::npos) {
         sPiBrdInt.revision = line.substr(pos + 11);
         continue;
      }

      pos = line.find("Serial\t\t:");
      if (pos != std::string::npos) {
         sPiBrdInt.serial = line.substr(pos + 9);
         continue;
      }
   }

   iS.close();
   sPiBrdInt.init = true;

   sPiBoard.hardware   = sPiBrdInt.hardware.c_str();
   sPiBoard.revision   = sPiBrdInt.revision.c_str();
   sPiBoard.serial     = sPiBrdInt.serial.c_str();
   sPiBoard.boardModel = getBoardModel();

   *pBoardInf = sPiBoard;
   return eBaBoolRC_Success;
}

//
EHwPiModel HwPiGetBoardModel() {
   THwPiBoard bi;
   bi.boardModel = eHwPiModelUnknown;

   HwPiGetBoardInfo(&bi);
   return bi.boardModel;
}

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
// http://elinux.org/RPi_HardwareHistory#Board_Revision_History
// Raspberry Pi Model A: 0007, 0008, 0009
// Raspberry Pi Model B (Rev 1.0, 256Mb): 0002, 0003
// Raspberry Pi Model B (Rev 2.0, 256Mb): 0004, 0005, 0006
// Raspberry Pi Model B (Rev 2.0, 512Mb): 000d, 000e, 000f
// Raspberry Pi Model A+: 0012
// Raspberry Pi Model B+: 0010, 0013
// Raspberry Pi 2 Model B: 1041
// Raspberry Pi 3 Model B: 2082
LOCAL EHwPiModel getBoardModel() {
   std::string rev = sPiBrdInt.revision.length() > 6 ?
         sPiBrdInt.revision.substr(4) : sPiBrdInt.revision;
   rev = "0x" + rev;

   switch (BaToNumber(rev, 0u)) {
      case 0x0002  : return eHwPiModelB;
      case 0x0003  : return eHwPiModelB;
      case 0x0004  : return eHwPiModelB;
      case 0x0005  : return eHwPiModelB;
      case 0x0006  : return eHwPiModelB;
      case 0x0007  : return eHwPiModelA;
      case 0x0008  : return eHwPiModelA;
      case 0x0009  : return eHwPiModelA;
      case 0x000d  : return eHwPiModelB;
      case 0x000e  : return eHwPiModelB;
      case 0x000f  : return eHwPiModelB;
      case 0x0010  : return eHwPiModelBp;
      case 0x0011  : return eHwPiModelUnknown;
      case 0x0012  : return eHwPiModelAp;
      case 0x0013  : return eHwPiModelBp;
      case 0x0014  : return eHwPiModelUnknown;
      case 0x0015  : return eHwPiModelAp;
      case 0xa01040: return eHwPiModel2;
      case 0xa01041: return eHwPiModel2;
      case 0xa21041: return eHwPiModel2;
      case 0xa22042: return eHwPiModel2;
      case 0x900092: return eHwPiModelUnknown;
      case 0x900093: return eHwPiModelUnknown;
      case 0xa02082: return eHwPiModel3;
      case 0xa22082: return eHwPiModel3;
      default: return eHwPiModelUnknown;
   }

}
