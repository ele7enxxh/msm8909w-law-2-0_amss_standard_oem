#ifndef DS_NET_IMCASTSESSIONSOUTPUT_H
#define DS_NET_IMCASTSESSIONSOUTPUT_H

#ifdef FEATUTE_DATA_PS_MCAST
/*============================================================================
  Copyright (c) 2012 Qualcomm Technologies Incorporated.
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
      const ::AEEIID AEEIID_IMCastSessionsOutput = 0x109b9a8;
      
      /** @interface IMCastSessionsOutput
        * 
        * ds Net IMCastSessionsOutput interface.
        * This interface represents a bundle of MCast Sessions.
        * The Bundle holds a reference to its MCast Sessions objects.
        * See IMCastManager::JoinBundle.
        */
      struct IMCastSessionsOutput : public ::IQI
      {
         
         /**    
           * This function provide the Nth IMCastSession object of the bundle.
           * The order of the objects corresponds to the order of MCast addresses
           * provided to IMCastManager::JoinBundle.          
           * @param mcastSession The Nth Mcast session object of the bundle.
           * @param index number of Session in the bundle. Zero based.
           * @retval ds::SUCCESS The Nth Mcast session is successfully provided.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           * @See IMCastManager::JoinBundle.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetNth(int index, ::ds::Net::IMCastSession** mcastSession) = 0;
         
         /**
           * Number of elements in the bundle.          
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetnumElements(int* value) = 0;
      };
   };
};
#endif // FEATUTE_DATA_PS_MCAST
#endif //DS_NET_IMCASTSESSIONSOUTPUT_H
