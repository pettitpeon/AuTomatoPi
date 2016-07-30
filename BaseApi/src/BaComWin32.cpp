///*------------------------------------------------------------------------------
// *                             (c) 2015 by Ivan Peon
// *                             All rights reserved
// *------------------------------------------------------------------------------
// *   Module   : BaCom.cpp
// *   Date     : Nov 30, 2015
// *------------------------------------------------------------------------------
// *   Module description:
// */
///*------------------------------------------------------------------------------
// */
//#ifdef __WIN32
//
//#include <fcntl.h>
//
//#include <string>
//#include <fstream>
//#include <iostream>
//#include <vector>
//#include <sstream>
//#include <map>
//
//#include "BaCom.h"
//#include "BaCore.h"
//#include "BaGpio.h"
//#include "BaGenMacros.h"
//#include "BaUtils.hpp"
//
//#define BUS1W   04
//#define TXD0    14
//#define RXD0    15
//#define SER_ALT  0
//#define DEVPATH  "C:\\tmp\\devices\\"
//#define TEMPFAM 28
//
//// Serial descriptor
//typedef struct TSerialDesc {
//   int fd;
//   IBaGpio *pTXD0;
//   IBaGpio *pRXD0;
//
//   TSerialDesc() : fd(0), pTXD0(0), pRXD0(0) {
//      pTXD0 = IBaGpioCreate(TXD0);
//      pRXD0 = IBaGpioCreate(RXD0);
//   }
//   ~TSerialDesc() {
//      IBaGpioDelete(pTXD0);
//      IBaGpioDelete(pRXD0);
//   }
//} TSerialDesc;
//
//// 1W descriptor
//typedef struct T1wDev {
//   std::ifstream* pIs;
//   std::string serNo;
//   std::string actVal;
//   bool valid; // Flag to check that the value is valid
//   bool run; // Flag to check that the value is valid
//   TBaCoreThreadHdl updThread;
//   TBaCoreThreadArg threadArg;
//   uint32_t heartBeat;
//
//   T1wDev() : pIs(0), serNo(""), actVal(""), valid(false), run(false), updThread(0),
//         heartBeat(0) {
//      threadArg = {0};
//      threadArg.pArg = this;
//   }
//} T1wDev;
//
//typedef std::map<uint16_t, std::vector<T1wDev*>> T1WDevs;
//LOCAL inline float readTemp(const char *dvrStr, TBaBool *pError);
//LOCAL inline TBaBool read1wDevice(uint8_t devFam, const char *serNo, std::string &contents);
//LOCAL inline T1wDev* find1wDev(const char *serNo, std::vector<T1wDev*> &rTherms);
//LOCAL void rdAsync1WRout(TBaCoreThreadArg *pArg);
//
//// GPIO for the 1w bus. default GPIO 4, pin 7
//static IBaGpio *sp1w = 0;
//
//// Map of family IDs and devices vector
//static T1WDevs s1WDevs;
//
////
//TBaBoolRC BaCom1WInit() {
//   if (sp1w) {
//      return eBaBoolRC_Success;
//   }
//
//   // This reserves the GPIO in the c++ interface
//   sp1w = IBaGpioCreate(BUS1W);
//   if (!sp1w) {
//      return eBaBoolRC_Error;
//   }
//
//   // This reserves the GPIO in the C interface
//   sp1w->SetAlt(0);
//   BaCom1WGetDevices();
//
//   return eBaBoolRC_Success;
//}
//
////
//TBaBoolRC BaCom1WExit() {
//   if (!sp1w) {
//      return eBaBoolRC_Success;
//   }
//
//   for (auto kv : s1WDevs) {
//      for (auto pDev : kv.second) {
//         if (pDev && pDev->pIs) {
//            if (pDev->updThread) {
//               pDev->threadArg.exitTh = eBaBool_true;
//               BaCoreDestroyThread(pDev->updThread, 10);
//               pDev->updThread = 0;
//            }
//            delete pDev->pIs; // deleting closes the file =)
//            pDev->pIs = 0;
//            delete pDev;
//         }
//      }
//   }
//   s1WDevs.clear();
//
//   TBaBoolRC rc = IBaGpioDelete(sp1w);
//   sp1w = 0;
//   return rc;
//}
//
////
//uint16_t BaCom1WGetDevices(){
//   if (!BaFS::Exists(DEVPATH)) {
//      return eBaBoolRC_Error;
//   }
//
//   // directory entry
//   struct dirent *de = 0;
//   DIR *d = 0;
//   uint16_t famId = 0;
//
//   T1wDev *pDev = new T1wDev;
//
//   d = opendir(DEVPATH);
//   if (!d) {
//      return 0;
//   }
//
//   uint16_t i = 0;
//   for (de = readdir(d); de != 0; de = readdir(d)) {
//
//      // Skip "." and ".."
//      if ((strcmp(de->d_name, "..") == 0) || (strcmp(de->d_name, ".") == 0)) {
//         continue;
//      }
//
//      // Get open devices with a valid family ID in a input file stream
//      // eg. 28-0215c2c4bcff [devFam]-[devID]
//      // The information is under /sys/bus/w1/devices/28-0215c2c4bcff/w1_slave
//      std::stringstream ss;
//      ss << de->d_name;
//      if (ss >> famId) {
//         pDev->serNo = de->d_name;
//         pDev->pIs = new std::ifstream();
//         if (pDev->pIs) {
//            std::string path = DEVPATH + std::string(de->d_name) + "/w1_slave";
//            pDev->pIs->open(path);
//            if(!pDev->pIs->fail()) {
//               // This is a map of family IDs and file streams
//               // push_back() copies the object
//               pDev->updThread = BaCoreCreateThread("name", rdAsync1WRout,
//                     &pDev->threadArg, eBaCorePrio_Low);
//               s1WDevs[famId].push_back(pDev);
//               i++;
//            } else {
//               delete pDev->pIs;
//               pDev->pIs = 0;
//            }
//         }
//      }
//   }
//   return i;
//}
//
////
//int32_t BaCom1WRdAsync(uint8_t famID, const char *serNo, TBaBool *pError) {
//   TBaBool terror;
//   pError = pError ? pError : &terror;
//   T1wDev *pDev = 0;
//   if(!sp1w || s1WDevs.size() == 0) {
//      *pError = eBaBool_true;
//      return 0;
//   }
//
//   auto family = s1WDevs.find(famID);
//   if (family == s1WDevs.end() || family->second.size() == 0) {
//      return eBaBoolRC_Error;
//   }
//
//   for (auto dev : family->second) {
//      if (dev && dev->serNo == serNo) {
//         pDev = dev;
//      }
//   }
//
//   if (pDev) {
//      pDev->run = true;
//      std::cout << "(" << pDev->valid << ", " << pDev->heartBeat << "): " << pDev->actVal << std::endl;
//   }
//   return 0;
//}
//
////
//float BaCom1WGetTemp(const char* serNo, TBaBool *pError) {
//   TBaBool terror;
//   pError = pError ? pError : &terror;
//   std::string contents;
//
//   if (!read1wDevice(TEMPFAM, serNo, contents)) {
//      *pError = eBaBool_true;
//      return -300;
//   }
//
//   return readTemp(contents.c_str(), pError);
//}
//
////
//void *BaCom1WGetValue(uint8_t famID, const char *serNo, TBaCom1wReadFun cb,
//      TBaBool *pError) {
//   TBaBool terror;
//   pError = pError ? pError : &terror;
//
//   if (!cb) {
//      *pError = eBaBoolRC_Error;
//      return 0;
//   }
//   std::string contents;
//
//   if (!read1wDevice(famID, serNo, contents)) {
//      *pError = eBaBoolRC_Error;
//      return 0;
//   }
//
//   return cb(contents.c_str(), contents.size());
//}
//
////
//LOCAL inline float readTemp(const char *dvrStr, TBaBool *pError) {
//   if (!dvrStr) {
//      if (pError) {
//         *pError = eBaBool_true;
//      }
//      return -300;
//   }
//
//   // Extract the values. Temp in milli °C
//   // 96 01 4b 46 7f ff 0c 10 a0 : crc=a0 YES
//   // 96 01 4b 46 7f ff 0c 10 a0 t=25375
//
//   std::string contents(dvrStr);
//   if (contents.find("YES") == std::string::npos) {
//      if (pError) {
//         *pError = eBaBool_true;
//      }
//      return -300;
//   }
//
//   return (BaToNumber(contents.substr(contents.find("t=") + 2), -300, (bool*)pError)/1000.0f);
//}
//
////
//LOCAL inline TBaBool read1wDevice(uint8_t devFam, const char *serNo, std::string &contents) {
//   if (!sp1w) {
//      return eBaBoolRC_Error;
//   }
//   bool runTemp = false;
//   // Find thermometers
//   auto therms = s1WDevs.find(devFam);
//   if (therms == s1WDevs.end() || therms->second.size() == 0) {
//      return eBaBoolRC_Error;
//   }
//
//   // Find the thermometer
//   T1wDev *pDev = find1wDev(serNo, therms->second);
//   if (!pDev) {
//      return eBaBoolRC_Error;
//   }
//
//   runTemp = pDev->run;
//   pDev->run = false;
//   std::ifstream &r1wIn = *pDev->pIs;
//
//   r1wIn.seekg(0, std::ios::end);
//   auto size = r1wIn.tellg();
//   if (size == -1) {
//      return eBaBoolRC_Error;
//   }
//
//   contents.resize(size);
//   r1wIn.seekg(0, std::ios::beg);
//   r1wIn.read(&contents[0], contents.size());
//
//   // Rewind the stream
//   r1wIn.clear();
//   r1wIn.seekg(0, std::ios::beg);
//
//   pDev->run = runTemp;
//   return eBaBoolRC_Success;
//}
//
////
//LOCAL inline T1wDev* find1wDev(const char *serNo, std::vector<T1wDev*> &rTherms) {
//   if (!serNo) {
//      return rTherms[0];
//   }
//
//   // todo: no info about the serno =(
//   for (auto dev : rTherms) {
//      if (dev && dev->serNo == serNo) {
//         return dev;
//      }
//   }
//
//   return 0;
//}
//
////
//LOCAL void rdAsync1WRout(TBaCoreThreadArg *pArg) {
//   if (!pArg || !pArg->pArg) {
//      return;
//   }
//
//   T1wDev *pDev = (T1wDev*)pArg->pArg;
//   std::ifstream &r1wIn =  *pDev->pIs;
//   while (!pArg->exitTh) {
//      if(pDev->run) {
//
//         // Check file size
//         r1wIn.seekg(0, std::ios::end);
//         auto size = r1wIn.tellg();
//
//         if (size != -1) {
//            // resize string to size
//            pDev->actVal.resize(size);
//
//            // Rewind stream
//            r1wIn.seekg(0, std::ios::beg);
//
//            // Read contents to string
//            r1wIn.read(&pDev->actVal[0], pDev->actVal.size());
//            pDev->valid = true;
//            // Rewind the stream
//            r1wIn.clear();
//            r1wIn.seekg(0, std::ios::beg);
//
//         } else {
//            pDev->valid = false;
//         }
//
//      } else {
//         pDev->valid = false;
//      }
//      pDev->heartBeat++;
////      if (pDev->heartBeat % 50) {
////         printf("%ui \n", pDev->heartBeat);
////      }
//      BaCoreMSleep(10);
//   }
//}
//
//#endif // __linux
