#ifndef DS_NET_IMCASTSESSION_H
#define DS_NET_IMCASTSESSION_H

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
      namespace MCastSessionEvent
      {
         
         /**
           * QDS_EV_REGISTRATION_STATE: Register to this event to be notified
           * on changes in the registration state of this MCast Session.
           * Registration state change information contains both a sepcification
           * of the new registration state and an information code (status code
           * that may specify further information such as registration failure
           * reason.
           * @See MCastStateChangedType, MCastRegStateType, MCastInfoCodeType
           */
         const ::ds::Net::EventType QDS_EV_REGISTRATION_STATE = 0x106e60d;
         
         /**
           * QDS_EV_TECHNOLOGY_STATUS: Register to this event to be notified
           * on status changes of this MCast Session. 
           * @See MCastInfoCodeType
           */
         const ::ds::Net::EventType QDS_EV_TECHNOLOGY_STATUS = 0x106e60e;
      };
      const ::AEEIID AEEIID_IMCastSession = 0x106cf95;
      
      /** @interface IMCastSession
        * 
        * ds Net MultiCast interface.
        * Events that can be registered on this interface via OnStateChange (as part of IEventManager interface): 
        * - ds::Net::MCastSessionEvent::REGISTRATION_STATE. Use
        *   GetRegStateAndInfoCode to fetch the state information.
        * - ds::Net::MCastSessionEvent::TECHNOLOGY_STATUS. Use
        *   GetTechnologyStatus to fetch the status information.
        */
      struct IMCastSession : public ::ds::Net::IEventManager
      {
         
         /**
           * This function issues a request for leaving a multicast group.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Leave() = 0;
         
         /**
           * This attribute indicates the MCast session registration state and
           * information code.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetRegStateAndInfoCode(::ds::Net::MCastStateChangedType* value) = 0;
         
         /**
           * This attribute indicates the information code for the status of the
           * MCast technology session.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetTechStatusInfoCode(::ds::Net::MCastInfoCodeType* value) = 0;
      };
   };
};
#endif // FEATUTE_DATA_PS_MCAST
#endif //DS_NET_IMCASTSESSION_H
