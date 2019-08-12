#ifndef DS_NET_IPOLICYPRIV_H
#define DS_NET_IPOLICYPRIV_H

/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Net_Def.h"
#include "ds_Utils_IQI.h"
#include "ds_Addr_Def.h"
#include "ds_Net_IPolicy.h"
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IPolicyPriv = 0x1072d07;
      
      /** @interface IPolicyPriv
        * 
        * ds Network Policy Privileged interface.
        * This interface allows for additional, non public, policy attributes.
        */
      struct IPolicyPriv : public ::ds::Net::IPolicy
      {
         
         /**
           * This attribute indicates an iface id for the policy.
           * @param value Attribute value
           * @See INetwork::IfaceId
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetIfaceId(::ds::Net::IfaceIdType* value) = 0;
         
         /**
           * This attribute indicates an iface id for the policy.
           * @param value Attribute value
           * @See INetwork::IfaceId
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetIfaceId(::ds::Net::IfaceIdType value) = 0;
         
         /**
           * This attribute indicates the UMTS Access Point name as part of the
           * network policy.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetUMTSAPNName(char* value, int valueLen, int* valueLenReq) = 0;
         
         /**
           * This attribute indicates the UMTS Access Point name as part of the
           * network policy.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetUMTSAPNName(const char* value) = 0;

         /**
           * This attribute indicates the subscription identifier as part of the
           * network policy.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetSubsId(::ds::Net::SubsInfoType* subs_id) = 0;
         
         /**
           * This attribute indicates the subscription identifier as part of the
           * network policy.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetSubsId(::ds::Net::SubsInfoType subs_id) = 0;
         /**
           * This attribute indicates the apn type as part of the network
           * policy. 
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetApnType(::ds::Net::ApnInfoType* apn_type) = 0;
         
         /**
           * This attribute indicates the apn type as part of the network 
           * policy. 
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetApnType(::ds::Net::ApnInfoType apn_type) = 0;
      };
   };
};
#endif //DS_NET_IPOLICYPRIV_H
