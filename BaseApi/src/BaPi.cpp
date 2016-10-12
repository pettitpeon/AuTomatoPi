/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaPi.cpp
 *   Date     : Oct 11, 2016
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "BaPi.h"
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
static TBaPiBoard sPiBoard = {0};

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
LOCAL EBaPiModel getBoardModel();

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/

// http://elinux.org/RPi_HardwareHistory
TBaBoolRC BaPiGetBoardInfo(TBaPiBoard *pBoardInf) {
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
      puts(line.c_str());

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
EBaPiModel BaPiGetBoardModel() {
   TBaPiBoard bi;
   bi.boardModel = eBaPiModelUnknown;

   BaPiGetBoardInfo(&bi);
   return bi.boardModel;
}

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/
//
LOCAL EBaPiModel getBoardModel() {
   std::string rev = sPiBrdInt.revision.length() > 6 ?
         sPiBrdInt.revision.substr(4) : sPiBrdInt.revision;
   rev = "0x" + rev;

   switch (BaToNumber(rev, 0u)) {
      case 0x0002  : return eBaPiModelB;
      case 0x0003  : return eBaPiModelB;
      case 0x0004  : return eBaPiModelB;
      case 0x0005  : return eBaPiModelB;
      case 0x0006  : return eBaPiModelB;
      case 0x0007  : return eBaPiModelA;
      case 0x0008  : return eBaPiModelA;
      case 0x0009  : return eBaPiModelA;
      case 0x000d  : return eBaPiModelB;
      case 0x000e  : return eBaPiModelB;
      case 0x000f  : return eBaPiModelB;
      case 0x0010  : return eBaPiModelBp;
      case 0x0011  : return eBaPiModelUnknown;
      case 0x0012  : return eBaPiModelAp;
      case 0x0013  : return eBaPiModelBp;
      case 0x0014  : return eBaPiModelUnknown;
      case 0x0015  : return eBaPiModelAp;
      case 0xa01040: return eBaPiModel2;
      case 0xa01041: return eBaPiModel2;
      case 0xa21041: return eBaPiModel2;
      case 0xa22042: return eBaPiModel2;
      case 0x900092: return eBaPiModelUnknown;
      case 0x900093: return eBaPiModelUnknown;
      case 0xa02082: return eBaPiModel3;
      case 0xa22082: return eBaPiModel3;
      default: return eBaPiModelUnknown;
   }

}
