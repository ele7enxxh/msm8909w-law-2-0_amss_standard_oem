#ifndef DS_NET_INETWORKIPV6PRIV_H
#define DS_NET_INETWORKIPV6PRIV_H

/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Addr_Def.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_Def.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_IIPv6Address.h"
namespace ds
{
   namespace Net
   {
      namespace IPv6PrivEvent
      {
         const ::ds::Net::EventType QDS_EV_PREFIX_CHANGED = 0x1089294;
      };
      struct IPv6PrivPrefixInfoType {
         ::ds::INAddr6Type prefix;
         IPv6AddrStateType prefixType;
         unsigned short prefixLen;
         AEEINTERFACE_PADMEMBERS(__pad, 2)
      };
      struct _SeqIPv6PrivPrefixInfoType__seq_IPv6PrivPrefixInfoType_Net_ds {
         IPv6PrivPrefixInfoType* data;
         int dataLen;
         int dataLenReq;
      };
      typedef _SeqIPv6PrivPrefixInfoType__seq_IPv6PrivPrefixInfoType_Net_ds SeqIPv6PrivPrefixInfoType;
      const ::AEEIID AEEIID_INetworkIPv6Priv = 0x1089273;
      
      /** @interface INetworkIPv6Priv
        * 
        * ds Net Network Privileged interface.
        * This interface is intended for internal usage for backward compatibility purposes.
        * INetworkIPv6Priv address interface.
        * Events that can be registered on this interface via OnStateChange (as part of IEventManager interface): 
        * - ds::Net::IPv6PrivEvent::QDS_EV_PREFIX_CHANGED. 
        */
      struct INetworkIPv6Priv : public ::ds::Net::IEventManager
      {
         
         /**
           * This function used to get all available prefixes
           * @param allPrefixes List , prefixes with DELETED state should not be included
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetAllIPv6Prefixes(::ds::Net::IPv6PrivPrefixInfoType* allPrefixes, int allPrefixesLen, int* allPrefixesLenReq) = 0;
      };
   };
};
#endif //DS_NET_INETWORKIPV6PRIV_H
