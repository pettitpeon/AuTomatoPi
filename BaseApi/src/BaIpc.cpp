/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaIpc.cpp
 *   Date     : Nov 7, 2016
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "BaIpc.h"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define C_HDL_ ((IBaIpc*) hdl)
#define DEF_PIPE "/tmp/BaIpc.fifo"


/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Static vars
 -----------------------------------------------------------------------------*/
static int sRdFifo = -1;
static int sWrFifo = -1;

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
//
TBaBoolRC BaIpcCreatePipeReader() {
   if (sRdFifo != -1) {
      return eBaBoolRC_Success;
   }

   int rc = mkfifo(DEF_PIPE, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
   if (rc != 0) {
      printf("%s\n", strerror(errno));
      return eBaBoolRC_Error;
   }

   int fd = open(DEF_PIPE, O_RDONLY | O_NONBLOCK);

   if (fd < 0) {
      printf("%s\n", strerror(errno));
      return eBaBoolRC_Error;
   }

   sRdFifo = fd;
   return eBaBoolRC_Success;
}

TBaBoolRC BaIpcCreatePipeWriter() {
   if (sWrFifo != -1) {
      return eBaBoolRC_Success;
   }

   if (sRdFifo < 0) {
      return BaIpcCreatePipeReader();
   }

   int fd = open(DEF_PIPE, O_WRONLY | O_NONBLOCK);

   if (fd < 0) {
      printf("%i\n", errno);
      return eBaBoolRC_Error;
   }

   sWrFifo = fd;
   return eBaBoolRC_Success;
}

//
TBaBoolRC BaIpcDestroyPipe() {
   if (sRdFifo < 0) {
      return eBaBoolRC_Error;
   }

   close((int) sRdFifo);
   unlink(DEF_PIPE);

   return eBaBoolRC_Success;
}

TBaBoolRC BaIpcReadPipe(void* pData, size_t size) {
   if (sRdFifo < 0 || !pData) {
      return eBaBoolRC_Error;
   }

   int rc = read(sRdFifo, pData, size);
   if (rc < 0) {
      printf("%s\n", strerror(errno));
      return eBaBoolRC_Error;
   }

   return eBaBoolRC_Success;
}

TBaBoolRC BaIpcWritePipe(const void* pData, size_t size) {
   if (sWrFifo < 0 || !pData) {
      return eBaBoolRC_Error;
   }

   int rc = write(sWrFifo, pData, size);
   if (rc < 0) {
      printf("%s\n", strerror(errno));
      return eBaBoolRC_Error;
   }

   return eBaBoolRC_Success;
}


/*------------------------------------------------------------------------------
    C++ Factories
 -----------------------------------------------------------------------------*/
//
IBaIpc * IBaIpcCreate() {
   return 0;
}

//
TBaBoolRC IBaIpcDestroy(IBaIpc *pHdl) {
   return eBaBoolRC_Success;
}

/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/

