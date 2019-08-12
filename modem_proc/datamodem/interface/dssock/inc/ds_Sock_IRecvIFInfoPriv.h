#ifndef DS_SOCK_IRECVIFINFOPRIV_H
#define DS_SOCK_IRECVIFINFOPRIV_H

/*============================================================================
  Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/

#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Sock_IAncDataPriv.h"

namespace ds
{
   namespace Sock
   {
      const ::AEEIID AEEIID_IRecvIFInfoPriv = 0x106e70e;
      
      /** @interface IRecvIFInfoPriv
        * 
        * ds Socket Recv IF Info interface.
        * This interface is used to provide application with the network interface handle
        * on which a datagram was received via a call to IDSSock RecvMsg API.
        * This information is provided if the IP_RECVIF socket option is enabled.
        */
      struct IRecvIFInfoPriv : public ::ds::Sock::IAncDataPriv
      {
         virtual ::AEEResult AEEINTERFACE_CDECL GetRecvIFHandle(unsigned int* value) = 0;
      };
   };
};
#endif //DS_SOCK_IRECVIFINFOPRIV_H
