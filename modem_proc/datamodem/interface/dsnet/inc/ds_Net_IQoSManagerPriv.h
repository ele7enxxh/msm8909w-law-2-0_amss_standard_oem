#ifndef DS_NET_IQOSMANAGERPRIV_H
#define DS_NET_IQOSMANAGERPRIV_H

/*============================================================================
  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
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
#include "ds_Net_IQoSSecondary.h"
#include "ds_Net_IQoSFlowPriv.h"
#include "ds_Addr_Def.h"
#include "ds_Net_IIPFilterPriv.h"
#include "ds_Net_QoS_DefPriv.h"
#include "ds_Net_IQoSSecondariesInput.h"
#include "ds_Net_IQoSSecondariesOutput.h"
#include "ds_Net_IQoSNetInitiated.h"
#include "ds_Net_IQoSNetInitiatedReq.h"

/*============================================================================
               Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
                            All Rights Reserved.
                    Qualcomm Confidential and Proprietary
============================================================================
*/
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IQoSManagerPriv = 0x10a32b1;
      
      /** @interface IQoSManagerPriv
        * 
        * ds Net QoS Manager Private interface.
        */
      struct IQoSManagerPriv : public ::IQI
      {
         
         /**
           * This function issues a secondary QoS request.
           * @param requestedQoSSpec The requested QoS specification.
           * @param session The secondary QoS session opened for handling the request.
           * @see IQoSSecondary
           * @retval AEE_SUCCESS Request received successfully.
           * @retval ds::EBADOPTS Bad options.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL RequestSecondary(const ::ds::Net::QoSSpecType* requestedQoSSpec, ::ds::Net::IQoSSecondary** session) = 0;
         
         /**
           * This function issues QoS requests for multiple QoS sessions.
           * @param specs The QoS specifications.
           * @param opCode The operation.
           * @param sessions The QoS sessions.
           * @see ds::Net::QoS::BundleOpCodeType
           * @retval AEE_SUCCESS Request received successfully.
           * @retval ds::EBADOPTS Bad options.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL RequestBundle(const ::ds::Net::QoSSpecType* specs, int specsLen, ::ds::Net::QoSRequestOpCodeType opCode, ::ds::Net::IQoSSecondariesOutput** sessions) = 0;
         
         /**
           * The function closes multiple QoS sessions.
           * @param sessions The QoS sessions.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Close(::ds::Net::IQoSSecondariesInput* qosSessions) = 0;
         
         /**
           * The function resumes multiple QoS sessions.
           * @param sessions The QoS sessions.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Resume(::ds::Net::IQoSSecondariesInput* qosSessions) = 0;
         
         /**
           * The function suspends multiple QoS sessions.
           * @param sessions The QoS sessions.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Suspend(::ds::Net::IQoSSecondariesInput* qosSessions) = 0;
         
         /**
          * This function issues a request to register filter specification 
          * for Network Initiated QoS. The filter specification shall be
          * compared with Network Initiated QoS' filter specification once
          * such QoS is established.
          * @param requestedQoSSpec The requested QoS specification.
          * @param session The Network Initiated QoS request session opened for handling the request.
          * @see IQoSNetInitiated
          * @retval AEE_SUCCESS Request received successfully.
          * @retval ds::EBADOPTS Bad options.
          * @retval Other ds designated error codes might be returned.
          * @see ds_Errors_Def.idl.
          */
         virtual ::AEEResult AEEINTERFACE_CDECL RequestNetInitiatedReq(const ::ds::Net::QoSSpecNetInitiatedType* requestedQoSSpec, ::ds::Net::IQoSNetInitiatedReq** session) = 0;
         
         /**
          * This function issues a request to filter match 
          * for Network Initiated QoS. The filter specification shall be
          * compared with Network Initiated QoS' filter specification once
          * such QoS is established.
          * @param filter info
          * @see IQoSNetInitiated
          * @retval AEE_SUCCESS Request received successfully.
          * @retval ds::EBADOPTS Bad options.
          * @retval Other ds designated error codes might be returned.
          * @see ds_Errors_Def.idl.
          */
         virtual ::AEEResult AEEINTERFACE_CDECL RequestFiltrMatch(void * fltr_info_ptr) = 0;
         
         /**
          * This function creates a QoSNetInitiated object.
          * It should be called when client gets a notification that a new Network
          * Initiated QoS has been established.          
          * @param QoSNetInitiatedReq The request for which notification of
          *                           the new QoS arrived.
          * @Param QoSNetInitiated Output The newly created QoSNetInitiated object
          * @see IQoSNetInitiatedReq
          * @retval AEE_SUCCESS Object created successfully.          
          * @retval Other ds designated error codes might be returned.
          * @see ds_Errors_Def.idl.
          */
         virtual ::AEEResult AEEINTERFACE_CDECL CreateQoSNetInitiated(::ds::Net::IQoSNetInitiatedReq* qoSNetInitiatedReq, ::ds::Net::IQoSNetInitiated** qoSNetInitiated) = 0;
      };
   };
};
#endif //DS_NET_IQOSMANAGERPRIV_H
