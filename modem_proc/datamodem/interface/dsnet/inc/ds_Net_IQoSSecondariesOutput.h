#ifndef DS_NET_IQOSSECONDARIESOUTPUT_H
#define DS_NET_IQOSSECONDARIESOUTPUT_H

/*============================================================================
  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Net_Def.h"
#include "ds_Net_QoS_Def.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_IPhysLink.h"
#include "ds_Net_IQoS.h"
#include "ds_Net_IQoSSecondary.h"
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IQoSSecondariesOutput = 0x109b9a7;
      
      /** @interface IQoSSecondariesOutput
        * 
        * ds Net IQoSSecondariesOutput interface.
        * This interface represents a bundle of QoS Secondary Sessions.
        * The Bundle holds a reference to its QoS Secondary objects.
        * See IQoSManager::Request.
        */
      struct IQoSSecondariesOutput : public ::IQI
      {
         
         /**    
           * This function provide the Nth IQoSSecondary object of the bundle.
           * The order of the objects corresponds to the order of QoS Specs
           * provided to IQoSManager::Request.          
           * @param qosSecondary The Nth QoS Secondary object of the bundle.
           * @param index number of Session in the bundle. Zero based.
           * @retval ds::SUCCESS The Nth QoS Secondary is successfully provided.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           * @See IQoSManager::Request.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetNth(int index, ::ds::Net::IQoSSecondary** qosSecondary) = 0;
         
         /**
           * Number of elements in the bundle.          
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetnumElements(int* value) = 0;
      };
   };
};
#endif //DS_NET_IQOSSECONDARIESOUTPUT_H
