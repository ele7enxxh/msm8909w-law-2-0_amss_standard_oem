#ifndef DS_NET_IMCASTMANAGERMBMSPRIV_H
#define DS_NET_IMCASTMANAGERMBMSPRIV_H

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
#include "ds_Addr_Def.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_Def.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_MCast_Def.h"
#include "ds_Net_IMCastSession.h"
#include "ds_Net_DownReasons_Def.h"
#include "ds_Net_IPhysLink.h"
#include "ds_Net_INetwork.h"
#include "ds_Net_IMCastMBMSCtrlPriv.h"
namespace ds
{
   namespace Net
   {
      typedef int MCastInfoCodeTypePriv;
      namespace MCastInfoCode
      {
         
         /**
           * MCastInfoCodeTypePriv is part of the enumeration in MCastInfoCodeType.
           * MBMS related info code is moved here until MBMS feature is released
           * publicly.
           */
         const ::ds::Net::MCastInfoCodeTypePriv MBMS_SYSTEM_UNAVAILABLE = 1151;
      };
      const ::AEEIID AEEIID_IMCastManagerMBMSPriv = 0x106df81;
      
      /** @interface IMCastManagerMBMSPriv
        * 
        * ds Net MCast Manager MBMS interface.
        * This Object is received from IMCastManagerPriv via Query Interface.
        */
      struct IMCastManagerMBMSPriv : public ::IQI
      {
         
         /**
           * This function is used to setup the MBMS context over the air.
           * @param addr Multicast address to be used.
           * @param pdpNumber PDP number to be used.
           * @see ds::IPAddrType.
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Activate(const ::ds::IPAddrType* addr, int pdpNumber, ::ds::Net::IMCastMBMSCtrlPriv** ppMCastMBMSCtrlPriv) = 0;
      };
   };
};
#endif // FEATUTE_DATA_PS_MCAST
#endif //DS_NET_IMCASTMANAGERMBMSPRIV_H
