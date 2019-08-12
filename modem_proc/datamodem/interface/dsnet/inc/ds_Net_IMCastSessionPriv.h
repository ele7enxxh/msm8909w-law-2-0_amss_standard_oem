#ifndef DS_NET_IMCASTSESSIONPRIV_H
#define DS_NET_IMCASTSESSIONPRIV_H

#ifdef FEATUTE_DATA_PS_MCAST
/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_Def.h"
#include "ds_Net_IEventManager.h"
#include "ds_Addr_Def.h"
#include "ds_Net_MCast_Def.h"
namespace ds
{
   namespace Net
   {
      namespace MCastSessionPrivEvent
      {
         
         /**
           * QDS_EV_REGISTRATION_STATE_PRIV_BCMCS1_0: 
           * DSS shall register to this event to be notified on MCast events with
           * BCMCS1.0 ("legacy") info code. DSS shall register to this event
           * regardless if the MCastSession object was obtained via call to
           * IMCastManagerPriv::Join or IMCastManagerPriv::JoinBundle.
           * Once the signal of this event registration is set, DSS shall fetch
           * both the registration state and info code together in a single call
           * to GetRegStateAndBCMCS1_0InfoCode.
           */
         const ::ds::Net::EventType QDS_EV_REGISTRATION_STATE_PRIV_BCMCS1_0 = 0x10a4640;
         
         /**
           * QDS_EV_REGISTRATION_STATE_PRIV_BCMCS2_0: 
           * DSS shall register to this event to be notified on MCast events with
           * BCMCS2.0 ("new") info code. DSS shall register to this event
           * regardless if the MCastSession object was obtained via call to
           * IMCastManagerPriv::Join or IMCastManagerPriv::JoinBundle.
           * Once the signal of this event registration is set, DSS shall fetch
           * both the registration state and info code together in a single call
           * to GetRegStateAndBCMCS2_0InfoCode.
           */
         const ::ds::Net::EventType QDS_EV_REGISTRATION_STATE_PRIV_BCMCS2_0 = 0x10a463f;
         
         /**
           * QDS_EV_TECHNOLOGY_STATUS_PRIV: 
           * DSS shall register to this event to be notified on MCast events with
           * BCMCS2.0 ("new") info code. DSS shall register to this event
           * regardless if the MCastSession object was obtained via call to
           * IMCastManagerPriv::Join or IMCastManagerPriv::JoinBundle.
           * Once the signal of this event registration is set, DSS shall fetch
           * both the registration state and info code together in a single call
           * to GetRegStateAndBCMCS2_0InfoCode.
           */
         const ::ds::Net::EventType QDS_EV_TECHNOLOGY_STATUS_PRIV = 0x10a4641;
      };
      const ::AEEIID AEEIID_IMCastSessionPriv = 0x10a38d4;
      
      /** @interface IMCastSessionPriv
        * 
        * ds Net MultiCast interface.
        * Events that can be registered on this interface via OnStateChange (as part of IEventManager interface): 
        * - ds::Net::MCastSessionEvent::REGISTRATION_STATE. Use GetRegistrationState to fetch the state information.
        * - ds::Net::MCastSessionEvent::TECHNOLOGY_STATUS. Use GetTechnologyStatus to fetch the status information.
        */
      struct IMCastSessionPriv : public ::ds::Net::IEventManager
      {
         
         /**
           * This function issues a request for leaving a multicast group.
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Leave() = 0;
         
         /**
           * This attribute indicates the MCast session registration state and
           * information code. DSS fetch this information once 
           * QDS_EV_REGISTRATION_STATE_PRIV_BCMCS1_0 events occurs.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetRegStateAndInfoCodeBCMCS1_0(::ds::Net::MCastStateChangedType* value) = 0;
         
         /**
           * This attribute indicates the MCast session registration state and
           * information code. DSS fetch this information once 
           * QDS_EV_REGISTRATION_STATE_PRIV_BCMCS2_0 events occurs.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetRegStateAndInfoCodeBCMCS2_0(::ds::Net::MCastStateChangedType* value) = 0;
         
         /**
           * This attribute indicates the information code for the status of the
           * MCast technology session.
           * DSS fetch this information onceQDS_EV_TECHNOLOGY_STATUS_PRIV event
           * occurs.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetTechStatusInfoCode(::ds::Net::MCastInfoCodeType* value) = 0;
      };
   };
};
#endif // FEATUTE_DATA_PS_MCAST
#endif //DS_NET_IMCASTSESSIONPRIV_H
