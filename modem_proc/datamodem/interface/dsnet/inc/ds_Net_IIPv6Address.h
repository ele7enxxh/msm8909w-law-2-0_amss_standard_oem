#ifndef DS_NET_IIPV6ADDRESS_H
#define DS_NET_IIPV6ADDRESS_H

/*============================================================================
  Copyright (c) 20088 - 2012 Qualcomm Technologies Incorporated.
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
namespace ds
{
   namespace Net
   {
      namespace IPv6AddrEvent
      {
         const ::ds::Net::EventType QDS_EV_STATE_CHANGED = 0x106e60a;
      };
      typedef int IPv6AddrStateType;
      namespace IPv6AddrState
      {
         const ::ds::Net::IPv6AddrStateType PRIV_ADDR_DELETED = 0;
         const ::ds::Net::IPv6AddrStateType PRIV_ADDR_WAITING = 1;
         const ::ds::Net::IPv6AddrStateType PRIV_ADDR_AVAILABLE = 3;
         const ::ds::Net::IPv6AddrStateType PRIV_ADDR_DEPRECATED = 4;
      };
      const ::AEEIID AEEIID_IIPv6Address = 0x106df4c;
      
      /** @interface IIPv6Address
        * 
        * IPv6 address interface. 
        * For private IPv6 address get an instance of this interface via
        * INetworkIPv6::GeneratePrivAddr.
        * 
        * Events that can be registered on this interface via OnStateChange (as part of IEventManager interface): 
        * - ds::Net::IPv6AddrEvent::STATE_CHANGED. Use GetState API to fetch information regarding the new state.
        */
      struct IIPv6Address : public ::ds::Net::IEventManager
      {
         
         /**
           * This attribute contains the IPv6 address encapsulated by this interface.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetAddress(::ds::INAddr6Type value) = 0;
         
         /**
           * This function is used to get the state of the IPv6 address.
           * @param state Holds the state of the address.
           * @see ds::StateType.
           * @retval AEE_SUCCESS Registration succeeded.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetState(::ds::Net::IPv6AddrStateType* state) = 0;
      };
   };
};
#endif //DS_NET_IIPV6ADDRESS_H
