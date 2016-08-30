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
#include "BaSwOsci.h"
#include "BaBool.h"
#include "BaCore.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

// todo: write the interface
class CBaSwOsci : public IBaSwOsci {
public:

   static CBaSwOsci* Create(const char *name, const char *path, bool toCnsole);
   static bool Destroy(IBaSwOsci* pHdl, uint32_t timeoutMs);

	virtual bool Register(void* pVar, EBaSwOsciType type, const char *name, const char *desc);
	bool Header();
	virtual bool Sample();

   // Force flush to disk
   void Flush();
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

