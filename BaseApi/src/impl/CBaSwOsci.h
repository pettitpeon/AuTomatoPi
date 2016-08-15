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
#include "BaBool.h"

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

   static CBaSwOsci* Create(
   );

   /***************************************************************************/
   /** ...
    *  @return ...
    */
	virtual bool Register(void* pVar, EBaSwOsciType type, const char *name, const char *desc);
	virtual bool Header();
	virtual bool Sample();

private:
   CBaSwOsci() {};
   // Typical object oriented destructor must be virtual!
   virtual ~CBaSwOsci() {};

   struct TSWOsci;
   std::vector<TSWOsci*> mRegister;

};


#endif // CBASWOSCI_H_

