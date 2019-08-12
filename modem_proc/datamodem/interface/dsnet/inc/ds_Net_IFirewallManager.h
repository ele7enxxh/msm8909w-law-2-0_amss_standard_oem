#ifndef DS_NET_IFIREWALLMANAGER_H
#define DS_NET_IFIREWALLMANAGER_H

/*============================================================================
  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/

#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
#include "ds_Net_IIPFilterPriv.h"
#include "ds_Errors_Def.h"
#include "ds_Net_IFirewallRule.h"

namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IFirewallManager = 0x109e2f6;
      
      /** @interface IFirewallManager
        * 
        * DS network Firewall manager interface.
        * This interface manages the firewall rules defined on INetwork object.
        * Application creates INetwork object and call IQI with IIPFirewallMgr
        * This is privileged - internal interface
        */
      struct IFirewallManager : public ::IQI
      {
         struct _IFirewallListType__seq_IIPFilterPriv_Net_ds {
            ::ds::Net::IIPFilterPriv** data;
            int dataLen;
            int dataLenReq;
         };
         typedef _IFirewallListType__seq_IIPFilterPriv_Net_ds IFirewallListType;
         struct _SeqFirewallRulesType__seq_IFirewallRule_Net_ds {
            ::ds::Net::IFirewallRule** data;
            int dataLen;
            int dataLenReq;
         };
         typedef _SeqFirewallRulesType__seq_IFirewallRule_Net_ds SeqFirewallRulesType;
         
         /**
           * This function adds firewall rules on specific DS Network object.
           * @param filter Firewall rule to be defined on the DS Network object.
           * @param firewallRule Output Returned Firewall rule interface.
           * @retval ds::SUCCESS Registration succeeded.
           * @retval ds::EBADOPTS Bad options.
           * @retval Other DS designated error codes might be returned.
           * @see ds_Errors.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL AddFirewallRule(::ds::Net::IIPFilterPriv* filter, ::ds::Net::IFirewallRule** firewallRule) = 0;
         
         /**
           * This function used to enable firewall on an interface
           * @param isallowed Specify if data is to be passed/dropped.
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other DS designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL EnableFirewall(boolean isAllowed) = 0;
         
         /**
           * This function used to disable firewall on an interface
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other DS designated error codes might be returned.
           * @see ds_Errors.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL DisableFirewall() = 0;
         
         /**
           * This function retrieves the firewall rules:
           * @param firewallRules Output Retrieved firewall spec
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other DS designated error codes might be returned.
           * @see ds_Errors.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetFirewallTable(::ds::Net::IFirewallRule** firewallRules, int firewallRulesLen, int* firewallRulesLenReq) = 0;
      };
   };
};
#endif //DS_NET_IFIREWALLMANAGER_H
