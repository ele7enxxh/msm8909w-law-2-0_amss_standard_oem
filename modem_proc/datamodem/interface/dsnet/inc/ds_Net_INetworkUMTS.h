#ifndef DS_NET_INETWORKUMTS_H
#define DS_NET_INETWORKUMTS_H

/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
namespace ds
{
   namespace Net
   {
      typedef boolean UMTSIMCNFlagType;
      const ::AEEIID AEEIID_INetworkUMTS = 0x107fc24;
      
      /** @interface INetworkUMTS
        * 
        * ds Net Network UMTS interface.
        * This interface provides UMTS network operations.
        */
      struct INetworkUMTS : public ::IQI
      {
         
         /**
           * This function Gets the IM CN Flag.
           * @param IMCNFlag Output IM CN flag information.
           * @retval AEE_SUCCESS INetworkUMTS cloned successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetIMCNFlag(::ds::Net::UMTSIMCNFlagType* IMCNFlag) = 0;
      };
   };
};
#endif //DS_NET_INETWORKUMTS_H
