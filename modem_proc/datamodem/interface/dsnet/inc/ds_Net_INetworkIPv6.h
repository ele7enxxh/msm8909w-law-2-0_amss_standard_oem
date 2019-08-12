#ifndef DS_NET_INETWORKIPV6_H
#define DS_NET_INETWORKIPV6_H

/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Addr_Def.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_Def.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_IIPv6Address.h"
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_INetworkIPv6 = 0x106dcc5;
      
      /** @interface INetworkIPv6
        * 
        * ds Network IPv6 interface.
        */
      struct INetworkIPv6 : public ::IQI
      {
         
         /**
           * This function requests to generate a private IPv6 address.
           * This API is part of functionality based on RFC 3041 - 
           * "Privacy Extensions for Stateless Address Autoconfiguration in IPv6".
           * @param unique TRUE if the user wishes to generate a unique address,
           *               FALSE if the user wishes to generate a shared address.
           * @param addr Object that will hold the IPv6 address.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GeneratePrivAddr(boolean unique, ::ds::Net::IIPv6Address** addr) = 0;
                  
         /**
           * This function returns the scope id 
           * @param scopeID Object that will hold the scope id.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetScopeID(unsigned int* scopeID) = 0;
      };
   };
};
#endif //DS_NET_INETWORKIPV6_H
