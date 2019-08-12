#ifndef DS_NET_IMCASTMBMSCTRLPRIV_H
#define DS_NET_IMCASTMBMSCTRLPRIV_H

#ifdef FEATUTE_DATA_PS_MCAST
/*============================================================================
  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
#include "ds_Net_DownReasons_Def.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_IPhysLink.h"
#include "ds_Net_INetwork.h"
namespace ds
{
   namespace Net
   {
      namespace MBMSEvent
      {
         const ::ds::Net::EventType QDS_EV_ACTIVATE_STATE = 0x106e618;
      };
      typedef int MBMSStateType;
      namespace MBMSState
      {
         const ::ds::Net::MBMSStateType QDS_ACTIVATED = 0;
         const ::ds::Net::MBMSStateType QDS_DEACTIVATED = 1;
      };
      const ::AEEIID AEEIID_IMCastMBMSCtrlPriv = 0x107a97c;
      
      /** @interface IMCastMBMSCtrlPriv
        * 
        * ds Net MBMS Control interface.
        * Events that can be registered on this interface via OnStateChange (as part of IEventManager interface): 
        * - ds::Net::MBMSEvent::ACTIVATE_STATE. Use GetState to get activate state information.
        */
      struct IMCastMBMSCtrlPriv : public ::ds::Net::IEventManager
      {
         
         /**
           * This function is used to deactivate the MBMS context over the air.
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL DeActivate() = 0;
         
         /**
           * This function is used to retrieve the state of the MBMS context.
           * @param state structure that will hold the MBMS context state.
           * @see ds::Net::MBMS::StateType.
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetState(::ds::Net::MBMSStateType* state) = 0;
      };
   };
};
#endif // FEATUTE_DATA_PS_MCAST
#endif //DS_NET_IMCASTMBMSCTRLPRIV_H
