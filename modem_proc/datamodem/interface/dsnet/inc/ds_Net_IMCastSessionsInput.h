#ifndef DS_NET_IMCASTSESSIONSINPUT_H
#define DS_NET_IMCASTSESSIONSINPUT_H

#ifdef FEATUTE_DATA_PS_MCAST
/*============================================================================
  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
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
#include "ds_Addr_Def.h"
#include "ds_Net_MCast_Def.h"
#include "ds_Net_IMCastSession.h"
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IMCastSessionsInput = 0x109b66a;
      
      /** @interface IMCastSessionsInput
        * 
        * ds Net IMCastSessionsInput interface.
        * This interface represents a group of MCast Sessions.
        * The Group does not hold a reference to the MCast Sessions objects.
        * A MCast Session object is disassociated from the group upon its
        * destruction.
        * @See IMCastManager::CreateMCastSessionsInput.
        * @See IMCastManager::JoinBundle.
        * @See IMCastManager::LeaveBundle.*
        * @See IMCastManager::RegisterBundle.
        */
      struct IMCastSessionsInput : public ::IQI
      {
         
         /**    
           * This function associates a mcastSession with the group.
           * @param mcastSession MCast Session to be associated with the group.
           * @retval ds::SUCCESS Mcast Session successfully associated.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.          
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Associate(::ds::Net::IMCastSession* mcastSession) = 0;
         
         /**    
           * This function disassociates a mcastSession from the group.
           * @param mcastSession MCast Session to be disassociated from the group.
           * @retval ds::SUCCESS Mcast Session successfully disassociated.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.          
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Disassociate(::ds::Net::IMCastSession* mcastSession) = 0;
      };
   };
};
#endif // FEATUTE_DATA_PS_MCAST
#endif //DS_NET_IMCASTSESSIONSINPUT_H
