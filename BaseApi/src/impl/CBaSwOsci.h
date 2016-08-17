/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : CBaSwOsci.h
 *   Date     : Aug 15, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/*  @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef CBASWOSCI_H_
#define CBASWOSCI_H_

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <vector>
#include <fstream> // std::ofstream
#include <mutex>
#include <string>
#include "BaBool.h"
#include "BaCore.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
typedef enum EBaSwOsciType {
   eBaSwOsci_undef = -1, ///< 0
   eBaSwOsci_int8 = 0, ///< 0
   eBaSwOsci_uint8,  ///< 1
   eBaSwOsci_int16,  ///< 2
   eBaSwOsci_uint16, ///< 3
   eBaSwOsci_int32,  ///< 5
   eBaSwOsci_uint32, ///< 6
   eBaSwOsci_int64,  ///< 7
   eBaSwOsci_uint64, ///< 8
   eBaSwOsci_float,  ///< 9
   eBaSwOsci_double,  ///< 10
   eBaSwOsci_max = eBaSwOsci_double///< 10
} EBaSwOsciType;

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

class CBaSwOsci {
public:

   static CBaSwOsci* Create(const char *name, const char *path, bool toCnsole);
   static bool Destroy(CBaSwOsci* pHdl);

	virtual bool Register(void* pVar, EBaSwOsciType type, const char *name, const char *desc);
	virtual bool Header();
	virtual bool Sample();

   // Force flush to disk
   virtual void Flush();
private:

   static void thRout(TBaCoreThreadArg *pArg);

   CBaSwOsci(const char *name, const char *path, bool toCnsole) : mSampling(false),
   mToCnsole(toCnsole), mThread(0), mThrArg {0}, mName(name), mPath(path) {};

   // Typical object oriented destructor must be virtual!
   virtual ~CBaSwOsci() {};

   struct TSWOsci;
   bool mSampling;
   bool mToCnsole;
   TBaCoreThreadHdl mThread;
   TBaCoreThreadArg mThrArg;
   std::string mName;
   std::string mPath;
   std::mutex mMtx; // Mutex to avoid simultaneous read and write of the buffer
   std::vector<TSWOsci*> mRegister;
   std::ofstream mLog; // file stream
   std::vector<std::string> mBuf; // Message queue
};


#endif // CBASWOSCI_H_

