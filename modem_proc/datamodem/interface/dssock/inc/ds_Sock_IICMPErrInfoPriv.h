#ifndef DS_SOCK_IICMPERRINFOPRIV_H
#define DS_SOCK_IICMPERRINFOPRIV_H

/*============================================================================
  Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/

#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
#include "ds_Sock_Def.h"
#include "ds_Utils_IQI.h"
#include "ds_Sock_IAncDataPriv.h"

namespace ds
{
   namespace Sock
   {
      const ::AEEIID AEEIID_IICMPErrInfoPriv = 0x106c948;
      
      /** @interface IICMPErrInfoPriv
        * 
        * ds Socket ICMP Error Info interface.
        */
      struct IICMPErrInfoPriv : public ::ds::Sock::IAncDataPriv
      {
         
         /**
           * Error information.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetExtendedErr(::ds::Sock::ExtendedErrType* value) = 0;
         
         /**
           * Address of the node that sent this ICMP error.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetAddr(::ds::SockAddrStorageType value) = 0;
      };
   };
};
#endif //DS_SOCK_IICMPERRINFOPRIV_H
