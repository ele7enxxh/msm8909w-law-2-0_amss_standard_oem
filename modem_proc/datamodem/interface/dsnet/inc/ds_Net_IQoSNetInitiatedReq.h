#ifndef DS_NET_IQOSNETINITIATEDREQ_H
#define DS_NET_IQOSNETINITIATEDREQ_H
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Net_Def.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_QoS_DefPriv.h"

/*============================================================================
               Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
                            All Rights Reserved.
                    Qualcomm Confidential and Proprietary
============================================================================
*/
namespace ds
{
   namespace Net
   {
      namespace QoSNetInitiatedReqEvent
      {
         const ::ds::Net::EventType QDS_EV_STATE_CHANGED = 0x1101f31;
      };
      const ::AEEIID AEEIID_IQoSNetInitiatedReq = 0x1101f33;
      
      /** @interface IQoSNetInitiatedReq
       * 
       * ds QoS Network Initiated Request interface.
       * Events that can be registered on this interface via OnStateChange(as part of IEventManager interface):
       * ds::Net::QoSNetInitiatedReqEvent::QDS_EV_STATE_CHANGED. Use GetLastMatchingFlowHandle to fetch matching flow handle.
       */
      struct IQoSNetInitiatedReq : public ::ds::Net::IEventManager
      {
         
         /**
          * This function issues a request to stop monitoring for network initiated QoS
          * @retval ds::SUCCESS Request received successfully.
          * @retval Other ds designated error codes might be returned.
          * @see ds_Errors_Def.idl.
          */
         virtual ::AEEResult AEEINTERFACE_CDECL Close() = 0;
         
         /**    
          * This attribute can be used to fetch the handle of last matching
          * flow that was notified to applications
          * 
          * @param value Attribute value
          */
         virtual int AEEINTERFACE_CDECL GetLastMatchingFlowHandle(::int32* value) = 0;

         /**    
         * This function indicates if a matching flow is panding for proccessing
         * 
         * 
         */
         virtual boolean AEEINTERFACE_CDECL IsPendingMatchingFlow() = 0;

         /**
          * This function checks if there is an iface that fulfils QoS
		  * requirements.
          * @retval ds::SUCCESS Request received successfully.
          * @retval Other ds designated error codes might be returned.
          * @see ds_Errors_Def.idl.
          */
         virtual ::AEEResult AEEINTERFACE_CDECL RealTimeCheck(const ::ds::Net::QoSSpecNetInitiatedType* pQoSNetInitiatedSpec) = 0;
      };
   };
};
#endif //DS_NET_IQOSNETINITIATEDREQ_H
