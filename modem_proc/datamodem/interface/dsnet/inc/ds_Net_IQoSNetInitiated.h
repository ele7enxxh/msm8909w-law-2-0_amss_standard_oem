#ifndef DS_NET_IQOSNETINITIATED_H
#define DS_NET_IQOSNETINITIATED_H
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Net_IQoSFlowPriv.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
#include "ds_Net_IIPFilterPriv.h"
#include "ds_Net_QoS_DefPriv.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_QoS_Def.h"
#include "ds_Net_IPhysLink.h"
#include "ds_Net_IQoS.h"

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
      const ::AEEIID AEEIID_IQoSNetInitiated = 0x1101f32;
      
      /** @interface IQoSNetInitiated
       * 
       * ds QoS Network Initiated interface.
       * Events that can be registered on this interface via OnStateChange(as part of IEventManager interface):
       * ds::Net::QoSEvent::QDS_EV_STATE_CHANGED. Use GetState to fetch the current state of the QoS session.
       */
      struct IQoSNetInitiated : public ::ds::Net::IQoS
      {
         
         /**    
          * This attribute can be used to fetch the current state of the QoS
          * session when a ds::Net::QoSNetInitiated::EVENT::STATE_CHANGED event
          * is received.
          * It provides the current state of the QoS Session and additional
          * info code.
          * See ds::Net::QoS::QoSStateType and ds::Net::QoS::InfoCodeType.
          * @param value Attribute value
          */
         virtual int AEEINTERFACE_CDECL GetState(::ds::Net::QoSStateInfoType* value) = 0;
         
         /**
          * This function returns the current Rx and Tx filter specification.
          * @param rxFilter the current Rx filter specification.
          * @param txFilter the current Tx filter specification.
          * @retval AEE_SUCCESS Request received successfully.
          * @retval ds::EBADOPTS Bad options.
          * @retval Other ds designated error codes might be returned.
          * @see ds_Errors_Def.idl.
          */
         virtual ::AEEResult AEEINTERFACE_CDECL GetGrantedFilterSpec(::ds::Net::IIPFilterPriv** rxFilter, int rxFilterLen, int* rxFilterLenReq, ::ds::Net::IIPFilterPriv** txFilter, int txFilterLen, int* txFilterLenReq) = 0;

         /**
          * This function returns an identifier which identify this object.
          * @retval identifier which identify this object.
          */
         virtual int32 AEEINTERFACE_CDECL GetQoSNetInitiatedId() = 0;

         /**
          * This function returns flow handle.
          * @retval flow handle.
          */
         virtual int32 AEEINTERFACE_CDECL GetFlowPriv() = 0;
      };
   };
};
#endif //DS_NET_IQOSNETINITIATED_H
