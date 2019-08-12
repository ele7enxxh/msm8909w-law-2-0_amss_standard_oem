#ifndef DS_NET_INETWORKEXT2_H
#define DS_NET_INETWORKEXT2_H

/*============================================================================
  Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated.
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
#include "ds_Net_IIPFilterPriv.h"
#include "ds_Errors_Def.h"
#include "ds_Net_IFirewallRule.h"
#include "ds_Net_IFirewallManager.h"
namespace ds
{
   namespace Net
   {
      struct FMCTunnelParamsType {
         int streamId;
         ::ds::SockAddrStorageType tunnelEndPointAddr;
         unsigned short addrLen;
         boolean IsNatPresent;
         AEEINTERFACE_PADMEMBERS(__pad, 1)
      };
      const ::AEEIID AEEIID_INetworkExt2 = 0x10b37b1;
      struct INetworkExt2 : public ::IQI
      {
         
         /**
           * This function creates an instance of IFirewallManager. IFirewallManager creation
           * is supported only via INetwork.    
           * @param newFirewallManagerObj Output The newly created IFirewallManager instance.
           * @retval DS::SUCCESS IFirewallManager created successfully.
           * @retval Other DS designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL CreateNetFirewallManager(::ds::Net::IFirewallManager** newFirewallManagerObj) = 0;
         /**
           * This function sets the FMC tunnel parameters for this network.
           * @param tunnelParams Structure representing FMC tunnel parameters
           * @see FMCTunnelParamsType.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetFMCTunnelParams(const ::ds::Net::FMCTunnelParamsType* tunnelParams) = 0;
         
         /**
           * This function resets the FMC tunnel parameters for this network.
           * @param none 
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL ResetFMCTunnelParams() = 0;
      };
   };
};
#endif //DS_NET_INETWORKEXT2_H
