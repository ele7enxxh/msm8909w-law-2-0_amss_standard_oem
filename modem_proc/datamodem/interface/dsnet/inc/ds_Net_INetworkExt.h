#ifndef DS_NET_INETWORKEXT_H
#define DS_NET_INETWORKEXT_H

/*============================================================================
  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated.
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
#include "ds_Net_QoS_Def.h"
#include "ds_Net_IPhysLink.h"
#include "ds_Net_IQoS.h"
#include "ds_Net_IQoSSecondary.h"
#include "ds_Net_IQoSSecondariesInput.h"
#include "ds_Net_IQoSSecondariesOutput.h"
#include "ds_Net_IQoSManager.h"
#include "ds_Addr_Def.h"
#include "ds_Net_MCast_Def.h"
#include "ds_Net_IMCastSession.h"
#include "ds_Net_IMCastManager.h"
#include "ds_Net_IBearerInfo.h"
#include "ds_Net_IBearerTechEx.h"
namespace ds
{
   namespace Net
   {
      namespace NetworkExtEvent
      {
         const ::ds::Net::EventType QDS_EV_QOS_AWARENESS = 0x106e611;
         const ::ds::Net::EventType QDS_EV_BEARER_TECH_CHANGED = 0x106e616;
         const ::ds::Net::EventType QDS_EV_BEARER_TECH_CHANGED_EX = 0x106e627;
         const ::ds::Net::EventType QDS_EV_HANDOFF_INIT = 0x106e624;
         const ::ds::Net::EventType QDS_EV_HANDOFF_SUCCESS = 0x106e625;
         const ::ds::Net::EventType QDS_EV_HANDOFF_FAILURE = 0x106e626;
         const ::ds::Net::EventType QDS_EV_EXTENDED_IP_CONFIG_EX = 0x106e628;
         const ::ds::Net::EventType QDS_EV_APN_PARAM_EV = 0x106e629;
      };
      typedef int QoSModeType;
      typedef struct
      {
        uint16 srat;
        uint16 trat;
      }HandoffinfoType;
      typedef struct
      {
        uint16 srat;
        uint16 trat;
        uint32 failure_reason;
      }HandoffFailureinfoType;
      namespace QoSMode
      {
         const ::ds::Net::QoSModeType UE_ONLY = 0;
         const ::ds::Net::QoSModeType UE_AND_NW = 1;
         const ::ds::Net::QoSModeType NW_ONLY = 2;
      };
      const ::AEEIID AEEIID_INetworkExt = 0x109a8af;
      
      /** @interface INetworkExt
        * 
        * ds NetworkExt interface.
        * This Network Extended interface is an auxiliary interface to INetwork,
        * supporting advanced Network operations, including QoS, MCast, BearerTech and more.
        * All QoS and MCast objects are created via this interface. @See CreateQoSManager and
        * CreateMCastManager.
        *
        * Events that can be registered on this interface via OnStateChange (as part of IEventManager interface): 
        * - ds::Net::NetworkExtEvent::QDS_EV_QOS_AWARENESS. Use IsQoSAware to fetch the QOS aware status information.       
        * - ds::Net::NetworkExtEvent::QDS_EV_BEARER_TECH_CHANGED. Use GetBearerInfo to fetch the status information.
        */
      struct INetworkExt : public ::ds::Net::IEventManager
      {
         
         /**
           * This function returns the bearer technology information.
           * @param bearerTech Object that will hold the bearer information.
           * @see IBearerInfo
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetBearerInfo(::ds::Net::IBearerInfo** bearerTech) = 0;
         
         /**          
           * This function returns the bearer technology information in a different format than
           * the above GetBearerInfo function.
           * @param bearerTechEx Object that will hold the bearer information.
           * @see IBearerTechEx
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetBearerTechEx(::ds::Net::IBearerTechEx** bearerTechEx) = 0;

         /**          
           * This attribute indicates whether or not the current system is
           * CDMA QoS Aware system.
           * TRUE if the mobile is on a QoS Aware system
           * FALSE if the mobile is on a QoS Unaware system.          
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetQosAware(boolean* value) = 0;
         
         /**
           * This function Gets the QoS mode
           * @param qosMode Output The current QosMode
           * @retval ds::SUCCESS QosMode retrieved successfully 
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetQoSMode(::ds::Net::QoSModeType* qosMode) = 0;

         /**
           * This function Gets the Handoff Info
           * @param handoffInfo Output The current HandoffInfo
           * @retval ds::SUCCESS HandoffInfo retrieved successfully 
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetHandoffInfo(::ds::Net::HandoffinfoType* handoffinfo) = 0;
         
         /**    
           * This function Gets the Handoff failure Info
           * @param handofffailureInfo Output The current HandofffailureInfo
           * @retval ds::SUCCESS HandoffFailureInfo retrieved successfully 
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetHandoffFailureInfo(::ds::Net::HandoffFailureinfoType* handofffailureinfo) = 0;
         
         /**    
           * This function creates an instance of IQoSManager. IQoSManager creation
           * is supported only via INetworkExt.    
           * @param newQoSManager Output The newly created IQoSManager instance.
           * @retval ds::SUCCESS IQoSManager created successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL CreateQoSManager(::ds::Net::IQoSManager** newQoSManager) = 0;
#ifdef FEATUTE_DATA_PS_MCAST
         /**    
           * This function creates an instance of IMCastManager. IMCastManager creation
           * is supported only via INetworkExt.    
           * @param newMCastManager Output The newly created IMCastManager instance.
           * @retval ds::SUCCESS IMCastManager created successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL CreateMCastManager(::ds::Net::IMCastManager** newMCastManager) = 0;
#endif // FEATUTE_DATA_PS_MCAST
      };
   };
};
#endif //DS_NET_INETWORKEXT_H
