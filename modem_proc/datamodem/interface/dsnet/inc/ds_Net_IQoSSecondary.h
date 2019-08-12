#ifndef DS_NET_IQOSSECONDARY_H
#define DS_NET_IQOSSECONDARY_H

/*============================================================================
  Copyright (c) 200 - 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Net_Def.h"
#include "ds_Net_QoS_Def.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_IPhysLink.h"
#include "ds_Net_IQoS.h"
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IQoSSecondary = 0x106cd45;
      
      /** @interface IQoSSecondary
        * 
        * ds QoS Secondary interface.
        * Events that can be registered on this interface via OnStateChange(as part of IEventManager interface):
        * - ds::Net::QoSSecondaryEvent::STATE_CHANGED. Use GetState to fetch the current state of the QoS session.
        */
      struct IQoSSecondary : public ::ds::Net::IQoS
      {
         
         /**
           * This function issues a request for closing a QoS Session. The request is 
           * forwarded to lower layers in order to release the QoS link resources.
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Close() = 0;
         
         /**
           * This function is used in order to activate the QoS link (from dormant mode).
           * @retval ds::SUCCESS The request was successfully issued. This doesn't indicate that the QoS
           *                     Session is already activated.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Resume() = 0;
         
         /**
           * This function is used in order to suspend a granted QoS Session.
           * @retval ds::SUCCESS The request was successfully issued. This doesn't indicate that the QoS
           *                     Session is already suspended.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Suspend() = 0;
      };
   };
};
#endif //DS_NET_IQOSSECONDARY_H
