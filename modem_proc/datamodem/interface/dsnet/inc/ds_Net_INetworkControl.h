#ifndef DS_NET_INETWORKCONTROL_H
#define DS_NET_INETWORKCONTROL_H

/*============================================================================
  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Net_Def.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_IEventManager.h"
namespace ds
{
   namespace Net
   {
      typedef int ReleaseReasonType;
      namespace ReleaseReason
      {
         const ::ds::Net::ReleaseReasonType QDS_NONE = 1;
         const ::ds::Net::ReleaseReasonType QDS_LOW_BATTERY = 2;
      };
      namespace NetworkControlEvent
      {
         const ::ds::Net::EventType QDS_EV_EXTENDED_IP_CONFIG = 0x1074cd5;
      };
      const ::AEEIID AEEIID_INetworkControl = 0x109a89f;
      
      /** @interface INetworkControl
        * 
        * ds NetworkControl interface.
        * This is an auxiliary interface to INetwork, supporting some advanced 
        * Network control operations.
        *
        * Events that can be registered on this interface via OnStateChange (as part of IEventManager interface): 
        * - ds::Net::NetworkControlEvent::EXTENDED_IP_CONFIG. Use GetDHCPRefreshResult to fetch the result of the DHCP refresh.
        */
      struct INetworkControl : public ::ds::Net::IEventManager
      {
         
         /**
           * This function is used to explicitly put into dormancy the Default (Primary) Physical 
           * Link associated with this Network *regardless* of other entities that are using it 
           * when this function is called.
           * Dormancy on a link implies the releasing of the traffic channel and other air
           * interface channels to free up appropriate radio resources.
           * The link is automatically brought out of dormancy when applicable operations are
           * applied by users of this Network.
           * The state of the Network object should remain OPEN while the Physical
           * link is dormant.
           * This function requires special privilege. 
           * @param dormantReason Reason why attempting to put the network into dormancy.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval AEE_EWOULDBLOCK Request is processed asynchronous. Application may fetch the
           *                         Physical Link object and register for Physical Link STATE_CHANGED
           *                         events in order to identify when the Physical Link is DOWN.          
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GoDormant(::ds::Net::DormantReasonType dormantReason) = 0;
         
         /**
           * This function is used to explicitly bring down the network connection.
           * Note that the applicable Network interface shall be brought down as well
           * *regardless* of other entities that are using it when this function is called.
           * This function is supported only for MONITORED mode Network objects. 
           * This function requires special privilege. 
           * @param nullReason Reason why attempting to tear down the network.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GoNull(::ds::Net::ReleaseReasonType nullReason) = 0;
         
         /**
           * This function used to refresh the DHCP configuration information.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL RefreshDHCPConfigInfo() = 0;
         
         /**
           * This function is used to get the result of a DHCP config refresh.
           * @param res The result of the refresh: true if successful, false otherwise.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetDHCPRefreshResult(boolean* res) = 0;
      };
   };
};
#endif //DS_NET_INETWORKCONTROL_H
