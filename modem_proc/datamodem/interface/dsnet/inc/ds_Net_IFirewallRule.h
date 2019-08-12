#ifndef DS_NET_IFIREWALLRULE_H
#define DS_NET_IFIREWALLRULE_H

/*============================================================================
  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/

#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Errors_Def.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
#include "ds_Net_IIPFilterPriv.h"

namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IFirewallRule = 0x109e2f5;
      struct IFirewallRule : public ::IQI
      {
         
         /**
           * This function retrieves one firewall rule
           * @param filterSpec Output Retrieved firewall specs
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other DS designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetFirewallRule(::ds::Net::IIPFilterPriv** filterSpec) = 0;
      };
   };
};
#endif //DS_NET_IFIREWALLRULE_H
