#ifndef DS_NET_IQOSSECONDARIESINPUT_H
#define DS_NET_IQOSSECONDARIESINPUT_H

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
      const ::AEEIID AEEIID_IQoSSecondariesInput = 0x109b6d0;
      
      /** @interface IQoSSecondariesInput
        * 
        * ds Net IQoSSecondariesInput interface.
        * This interface represents a group of QoS Secondary Sessions.
        * The Group does not hold a reference to the QoS Secondary objects.
        * A QoS Secondary object is disassociated from the group upon its
        * destruction.
        * @See IQoSManager::Close, IQoSManager::Resume, 
        *      IQoSManager::Suspend.
        */
      struct IQoSSecondariesInput : public ::IQI
      {
         
         /**    
           * This function associates a QoS Secondary with the group.
           * @param qosSecondary QoS Secondary to be associated with the group.
           * @retval ds::SUCCESS QoS Secondary successfully associated.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Associate(::ds::Net::IQoSSecondary* qosSecondary) = 0;
         
         /**    
           * This function disassociates a QoS Secondary from the group.
           * @param qosSecondary QoS Secondary to be disassociated from the group.
           * @retval ds::SUCCESS QoS Secondary successfully disassociated.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Disassociate(::ds::Net::IQoSSecondary* qosSecondary) = 0;
      };
   };
};
#endif //DS_NET_IQOSSECONDARIESINPUT_H
