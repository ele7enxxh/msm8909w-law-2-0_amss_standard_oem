#ifndef DS_NET_IPHYSLINK_H
#define DS_NET_IPHYSLINK_H

/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
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
      namespace PhysLinkEvent
      {
         const ::ds::Net::EventType QDS_EV_STATE_CHANGED = 0;
      };
      typedef unsigned int PhysLinkStateType;
      namespace PhysLinkState
      {
         const ::ds::Net::PhysLinkStateType QDS_DOWN = 0x1;
         const ::ds::Net::PhysLinkStateType QDS_RESUMING = 0x2;
         const ::ds::Net::PhysLinkStateType QDS_UP = 0x4;
         const ::ds::Net::PhysLinkStateType QDS_GOING_DOWN = 0x8;
      };
      const ::AEEIID AEEIID_IPhysLink = 0x106d750;
      
      /** @interface IPhysLink
        * 
        * ds physical link interface.
        */
      struct IPhysLink : public ::ds::Net::IEventManager
      {
         
         /**
           * This function is used to activate a dormant physical link.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GoActive() = 0;
         
         /**
           * This function is used to explicitly put into dormancy the Physical
           * Link *regardless* of other entities that are using it when this
           * function is called.
           * Dormancy on a link implies the releasing of the traffic channel and
           * other air interface channels to free up appropriate radio resources.
           * The link is automatically brought out of dormancy when applicable
           * operations are applied on it.
           * This function requires special privilege. 
           * @param dormantReason Reason why attempting to put the Physical Link
           * into dormancy.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval AEE_EWOULDBLOCK Request is processed asynchronous.
           *                         Application may register for Physical Link
           *                         STATE_CHANGED events in order to identify
           *                         when the Physical Link is DOWN.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GoDormant(::ds::Net::DormantReasonType dormantReason) = 0;
         
         /**
           * This attribute represents the state of the physical link.
           * @param value Attribute value
           * @see ds::Net::PhysLink::StateType
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetState(::ds::Net::PhysLinkStateType* value) = 0;
         
         /**
           * This attribute represents the previous phys link state.          
           * @param value Attribute value
           * @see ds::Net::PhysLink::StateType
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetPreviousState(::ds::Net::PhysLinkStateType* value) = 0;
      };
   };
};
#endif //DS_NET_IPHYSLINK_H
