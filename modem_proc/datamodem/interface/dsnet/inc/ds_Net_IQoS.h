#ifndef DS_NET_IQOS_H
#define DS_NET_IQOS_H

/*============================================================================
  Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated.
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
#include "ds_Net_QoS_Def.h"
#include "ds_Net_IPhysLink.h"
namespace ds
{
   namespace Net
   {
      namespace QoSEvent
      {
         const ::ds::Net::EventType QDS_EV_STATE_CHANGED = 0x106dee4;
         const ::ds::Net::EventType QDS_EV_MODIFIED = 0x106dee7;
      };

      typedef int QoSStateType;
      namespace QoSState
      {
         const ::ds::Net::QoSStateType QDS_AVAILABLE_MODIFIED = 1;
         const ::ds::Net::QoSStateType QDS_SUSPENDING = 2;
         const ::ds::Net::QoSStateType QDS_UNAVAILABLE = 3;
         const ::ds::Net::QoSStateType QDS_INVALID = 4;
         const ::ds::Net::QoSStateType QDS_ACTIVATING = 5;
         const ::ds::Net::QoSStateType QDS_SUSPENDED = 6;
         const ::ds::Net::QoSStateType QDS_RELEASING = 7;
         const ::ds::Net::QoSStateType QDS_CONFIGURING = 8;
      };
      struct QoSStateInfoType {
         QoSStateType state;
         QoSInfoCodeType infoCode;
      };
      const ::AEEIID AEEIID_IQoS = 0x106d74f;
      
      /** @interface IQoS
        * 
        * ds Net QoS interface.
        * Events that can be registered on this interface via OnStateChange (as part of IEventManager interface): 
        * - ds::Net::QoSEvent::MODIFY_RESULT. Use GetModifyResult to fetch the result information.
        * - ds::Net::QoSEvent::INFO_CODE_UPDATED. Use GetUpdatedInfoCode to fetch the information.
        */
      struct IQoS : public ::ds::Net::IEventManager
      {
         
         /**
           * This attribute provides the Tx physical link object.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetTXPhysLink(::ds::Net::IPhysLink** value) = 0;
         
         /**
           * This attribute provides the Rx physical link object.          
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetRXPhysLink(::ds::Net::IPhysLink** value) = 0;
         
         /**
           * This attribute can be used to fetch the result of last 
           * IQoSSecondary::Modify operation when a
           * ds::Net::QoS::Event::MODIFY_RESULT
           * event is received.          
           * @param value Attribute value
           * @See ds::Net::QoS::ResultType.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetModifyResult(::ds::Net::QoSResultType* value) = 0;
         
         /**    
           * This attribute can be used to fetch updated infoCode information
           * for the QoS flow when a ds::Net::QoS::Event::INFO_CODE_UPDATED
           * event is received.
           * @param value Attribute value
           * @See ds::Net::QoS::InfoCodeType.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetUpdatedInfoCode(::ds::Net::QoSInfoCodeType* value) = 0;
         
         /**
           * This attribute represents the TX queue level.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetTXQueueLevel(::ds::Net::QoSTXQueueLevelType* value) = 0;
         
         /**
           * This function allows object extensibility.
           * For supported interfaces, objects implementing those interfaces may be 
           * fetched via this function. The supported interfaces are documented in
           * the DS_NET_NetworkFactory.bid file. GetTechObject-supported interface
           * does not imply any similar contract in regard to QueryInterface for the
           * respective interface.
           * Unlike IQI, the availability of a specific interface depends on some 
           * factors, e.g. current network type. Moreover, there is no guaranty 
           * for transitivity or symmetry. 
           * Note: 'interface' parameter will map to iid and techObj.
           * @param iid The interface that should be retrieved.
           * @param techObj On success, will contain the requested interface instance.
           * @retval ds::SUCCESS Interface retrieved successfully.
           * @retval AEE_ECLASSNOTSUPPORT Specified interface unsupported.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetTechObject(AEEIID techObj_iid, void** techObj) = 0;

         /**
           * This attribute can be used to fetch the current state of the QoS
           * session when a ds::Net::QoS::EVENT::STATE_CHANGED event
           * is received.
           * It provides the current state of the QoS Session and additional
           * info code.
           * See ds::Net::QoS::StateType and ds::Net::QoS::InfoCodeType.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetState(::ds::Net::QoSStateInfoType* value) = 0;
      };
   };
};
#endif //DS_NET_IQOS_H
