#ifndef DS_NET_IQOSMANAGER_H
#define DS_NET_IQOSMANAGER_H

/*============================================================================
  Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated.
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
#include "ds_Net_IQoSSecondariesInput.h"
#include "ds_Net_IQoSSecondariesOutput.h"
namespace ds
{
   namespace Net
   {
      namespace QoSMgrEvent
      {
         const ::ds::Net::EventType QDS_EV_PROFILES_CHANGED = 0x106e615;
      };
      const ::AEEIID AEEIID_IQoSManager = 0x106cd44;
      
      /** @interface IQoSManager
        * 
        * ds Net QoS Manager interface.
        * Events that can be registered on this interface via OnStateChange (as part of IEventManager interface): 
        * - ds::Net::QoSMgrEvent::PROFILES_CHANGED. Use GetSupportedProfiles to fetch the current set of supported QoS Profiles. 
        */
      struct IQoSManager : public ::ds::Net::IEventManager
      {
         struct _SeqQoSProfileIdType__seq_long {
            ::ds::Net::QoSProfileIdType* data;
            int dataLen;
            int dataLenReq;
         };
         typedef _SeqQoSProfileIdType__seq_long SeqQoSProfileIdType;
         
         /**
           * This attribute returns an object that represents the default QoS link.          
           * @param value Attribute value
           * @see IQoS
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetQosDefault(::ds::Net::IQoS** value) = 0;
         
         /**
           * The function closes one or more QoS sessions.
           * @param qosSessions IQoSSecondary objects to Close.
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Close(::ds::Net::IQoSSecondariesInput* qosSessions) = 0;
         
         /**
           * The function resumes one or more QoS sessions.
           * @param qosSessions IQoSSecondary objects to Resume.
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Resume(::ds::Net::IQoSSecondariesInput* qosSessions) = 0;
         
         /**
           * The function suspends one or more QoS sessions.
           * @param qosSessions IQoSSecondary objects to Suspend.
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Suspend(::ds::Net::IQoSSecondariesInput* qosSessions) = 0;
         
         /**
           * This attribute returns the QoS Profiles that are currently supported
           * by the network.           
           * @param value Attribute value
           * @see SeqQoSProfileIdType
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetSupportedProfiles(::ds::Net::QoSProfileIdType* value, int valueLen, int* valueLenReq) = 0;
         
         /**    
           * This function creates an instance of IQoSSecondariesInput.
           * IQoSSecondariesInput creation is supported only via IQoSManager. 
           * @param new newQoSSecondariesInput Output The newly created 
           *                                          IQoSSecondariesInput instance.
           * @retval ds::SUCCESS IQoSSecondariesInput created successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL CreateQoSSecondariesInput(::ds::Net::IQoSSecondariesInput** newQoSSecondariesInput) = 0;
      };
   };
};
#endif //DS_NET_IQOSMANAGER_H
