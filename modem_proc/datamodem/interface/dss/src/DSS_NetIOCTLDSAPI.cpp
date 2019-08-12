/*======================================================

FILE:  DSS_NetIOCTLDSAPI.cpp

SERVICES:

GENERAL DESCRIPTION:
Implementation of Iface IOCTLs related ds functions.

=====================================================

Copyright (c) 2008-2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetIOCTLDSAPI.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2015-06-24 pg  Added to support qos handle when registration to net initiated qos.
  2013-11-01 rk  Removed feature mobileap.
  2011-11-27 ss  IOCTLs to enable/disable and retrieve ALG status list.
  2011-08-31 hs  added dss_iface_ioctl_get_subnet_mask
  2011-08-30 hs  added dss_iface_ioctl_get_gateway_addr.
  2011-08-24 hs  added dss_iface_ioctl_get_device_name.
  2010-06-30 cp  IOCTL changes for Soft AP DHCP server.
  2010-04-18 en  History added.

===========================================================================*/

#include "datamodem_variation.h"
#include <stringl/stringl.h>
#include "customer.h"
#include "dssocket.h"

#include "DSS_Conversion.h"
#include "DSS_NetApp.h"
#include "DSS_Globals.h"
#include "DSS_Common.h"
#include "DSS_IDSNetworkPrivScope.h"
#include "DSS_IDSNetworkScope.h"
#include "DSS_IDSNetActiveScope.h"
#include "DSS_NetActiveScope.h"
#include "DSS_NetMonitoredScope.h"
#include "DSS_IDSNetworkExtScope.h"
#include "DSS_IDSNetwork1xPrivScope.h"
#include "DSS_IDSNetwork1xScope.h"
#include "DSS_IDSNetworkUMTSScope.h"
#include "DSS_MemoryManagement.h"
#include "ds_Net_INetwork1x.h"
#include "ds_Net_INetwork1xPriv.h"
#include "ds_Net_IQoS1x.h"
#include "ds_Net_INetworkIPv6.h"
#include "ds_Net_INetworkIPv6Priv.h"
#include "ds_Net_INetworkControl.h"
#include "ds_Net_IFirewallManager.h"
#include "DSS_GenScope.h"
#include "DSS_QoSRequestExScope.h"
#include "ps_iface_defs.h"
#include "ps_iface_flow.h"
#include "ds_Net_IMCastManagerBCMCS.h"
#include "ds_Sock_AddrUtils.h"
#include "ds_Net_CreateInstance.h"
#include "ds_Net_INetworkUMTS.h"
#include "ds_Net_IMCastManagerMBMSPriv.h"
#include "ds_Net_IBearerInfo.h"
#include "ds_Net_IBearerTechEx.h"
#include "ds_Addr_Def.h"
#include "ds_Net_IMCastManagerBCMCS.h"
#include "ds_Net_IQoSManagerPriv.h"
#include "ds_Net_IQoSSecondariesOutput.h"
#include "ds_Net_IQoSSecondariesInput.h"
#include "ds_Net_IQoSSecondaryPriv.h"
#include "ds_Net_IQoSDefaultPriv.h"
#include "ds_Net_INetworkExt2.h"
#include "ds_Utils_DebugMsg.h"

#include "ds_Net_IQoSNetInitiatedReq.h"
#include "ps_flowi.h"
#include "dss_errors_def.h"


using namespace ds;
using namespace ds::Net;
using namespace dss::Error;

#ifndef min
#define min(a,b)  ((a) < (b) ? (a):(b))
#endif

#ifndef max
#define max(a,b)  ((a) < (b) ? (b):(a))
#endif

/* Defines the maximal amount of QoS specs allowed per QoS IOCTL request */
#define DSS_IFACE_IOCTL_QOS_REQUEST_EX_MAX_QOS_SPECS 10

/* functions for handling different cases */
static sint15 dss_iface_ioctl_get_ipv4_addr
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_ipv4_addr(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_ipv4_addr_type* pIpv4Addr = reinterpret_cast<dss_iface_ioctl_ipv4_addr_type*>(argval_ptr);
   IDS_ERR_RET_ERRNO(pNetMonitored->GetIPv4Addr(pIpv4Addr));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_ipv6_addr
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_ipv6_addr(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_ipv6_addr_type* pIpv6Addr = reinterpret_cast<dss_iface_ioctl_ipv6_addr_type*>(argval_ptr);
   IDS_ERR_RET_ERRNO(pNetMonitored->GetIPv6Addr(pIpv6Addr));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_ipv4_prim_dns_addr
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_ipv4_prim_dns_addr(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_ipv4_prim_dns_addr_type* pIpv4Addr = reinterpret_cast<dss_iface_ioctl_ipv4_prim_dns_addr_type*>(argval_ptr);
   IDS_ERR_RET_ERRNO(pNetMonitored->GetIPv4PrimDnsAddr(pIpv4Addr));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_ipv6_prim_dns_addr
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_ipv6_prim_dns_addr(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_ipv6_prim_dns_addr_type* pIpv6Addr = reinterpret_cast<dss_iface_ioctl_ipv6_prim_dns_addr_type*>(argval_ptr);
   IDS_ERR_RET_ERRNO(pNetMonitored->GetIPv6PrimDnsAddr(pIpv6Addr));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_ipv4_seco_dns_addr
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_ipv4_seco_dns_addr(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_ipv4_seco_dns_addr_type* pIpv4Addr = reinterpret_cast<dss_iface_ioctl_ipv4_seco_dns_addr_type*>(argval_ptr);
   IDS_ERR_RET_ERRNO(pNetMonitored->GetIPv4SecoDnsAddr(pIpv4Addr));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_ipv6_seco_dns_addr
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_ipv6_seco_dns_addr(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_ipv6_seco_dns_addr_type* pIpv6Addr = reinterpret_cast<dss_iface_ioctl_ipv6_seco_dns_addr_type*>(argval_ptr);
   IDS_ERR_RET_ERRNO(pNetMonitored->GetIPv6SecoDnsAddr(pIpv6Addr));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_all_dns_addrs
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_all_dns_addrs(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_get_all_dns_addrs_type* pDnsAddrs = reinterpret_cast<dss_iface_ioctl_get_all_dns_addrs_type*>(argval_ptr);
   IDS_ERR_RET_ERRNO(pNetMonitored->GetAllDnsAddrs(pDnsAddrs));
   return DSS_SUCCESS;
}


static sint15 dss_iface_ioctl_get_mtu
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_mtu(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_mtu_type* pMtu = reinterpret_cast<dss_iface_ioctl_mtu_type*>(argval_ptr);
   IDS_ERR_RET_ERRNO(pNetMonitored->GetMtu(pMtu));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_ip_addr
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_ip_addr(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_get_ip_addr_type* pIpAddr = reinterpret_cast<dss_iface_ioctl_get_ip_addr_type*>(argval_ptr);
   IDS_ERR_RET_ERRNO(pNetMonitored->GetIPAddress(pIpAddr));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_state
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_state(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_state_type* pIfaceState = reinterpret_cast<dss_iface_ioctl_state_type*>(argval_ptr);
   IDS_ERR_RET_ERRNO(pNetMonitored->GetIfaceState(pIfaceState));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_phys_link_state
(
   void *argval_ptr,
   DSSNetApp*  pNetApp,
   sint15  *dss_errno,
   dss_iface_id_type  iface_id
)
{
   IPhysLink* piPhysLink = NULL;
   AEEResult res = AEE_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_get_phys_link_state(): iface_id:%u, argval_ptr:0x%p",
                            iface_id, argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_phys_link_state_type* pPhyslinkState =
   reinterpret_cast<dss_iface_ioctl_phys_link_state_type*>(argval_ptr);
   PhysLinkStateType ds_state = 0;
   
   res = pNetApp->GetPhysLinkObject(iface_id, &piPhysLink);
   if(AEE_SUCCESS != res){
      if(QDS_ENETDOWN == res){
         *pPhyslinkState = PHYS_LINK_DOWN;
         return DSS_SUCCESS;
      }
      else{
         *dss_errno = DSSConversion::IDS2DSErrorCode(res); 
         return DSS_ERROR;
      }
   }

   DSSGenScope GenScopeObject(piPhysLink, DSSGenScope::IDSIQI_TYPE);

   IDS_ERR_RET_ERRNO(piPhysLink->GetState(&ds_state));


   if (AEE_SUCCESS != DSSConversion::IDS2DSPhyslinkState(ds_state, pPhyslinkState)) {
         LOG_MSG_ERROR_0("dss_iface_ioctl_get_phys_link_state(): "
                         "Can't allocate sipServerDomainNames ");
      }

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_rf_conditions
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_rf_conditions(): argval_ptr: 0x%p",
                            argval_ptr);
   
   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   IDS_ERR_RET_ERRNO(pNetMonitored->GetRFConditions(reinterpret_cast<dss_iface_ioctl_rf_conditions_type*>(argval_ptr)));
   return DSS_SUCCESS;
}

static AEEResult dss_iface_ioctl_reg_event_cb_explicitly_registerable
(
   dss_iface_ioctl_event_enum_type event
)
{
   AEEResult dss_errno = DSS_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_reg_event_cb_explicitly_registerable(): event: %d",
                            (int)(event));

   switch (event) {
   case DSS_IFACE_IOCTL_MIN_EV:
   case DSS_IFACE_IOCTL_REG_EVENT_MIN:
   case DSS_IFACE_IOCTL_DOWN_EV:
   case DSS_IFACE_IOCTL_UP_EV:
   case DSS_IFACE_IOCTL_COMING_UP_EV:
   case DSS_IFACE_IOCTL_GOING_DOWN_EV:
   case DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV:
   case DSS_IFACE_IOCTL_PHYS_LINK_UP_EV:
   case DSS_IFACE_IOCTL_PHYS_LINK_COMING_UP_EV:
   case DSS_IFACE_IOCTL_PHYS_LINK_GOING_DOWN_EV:
   case DSS_IFACE_IOCTL_ADDR_CHANGED_EV:
   case DSS_IFACE_IOCTL_BEARER_TECH_CHANGED_EV:
   case DSS_IFACE_IOCTL_BEARER_TECH_EX_CHANGED_EV:
   case DSS_IFACE_IOCTL_EXTENDED_IP_CONFIG_EV:
   case DSS_IFACE_IOCTL_QOS_AWARE_SYSTEM_EV:
   case DSS_IFACE_IOCTL_QOS_UNAWARE_SYSTEM_EV:
   case DSS_IFACE_IOCTL_RF_CONDITIONS_CHANGED_EV:
   case DSS_IFACE_IOCTL_QOS_ADDED_ON_IFACE_EV:
   case DSS_IFACE_IOCTL_QOS_DELETED_ON_IFACE_EV:
   case DSS_IFACE_IOCTL_OUTAGE_NOTIFICATION_EV:
   case DSS_IFACE_IOCTL_IFACE_AVAILABLE_EV:
   case DSS_IFACE_IOCTL_707_NETWORK_SUPPORTED_QOS_PROFILES_CHANGED_EV:
   case DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA_SUCCESS_EV:
   case DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA_FAILURE_EV:
   case DSS_IFACE_IOCTL_IPV6_EXTERNAL_ADDR_DELETED_EV:
      break;

   case DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV:
   case DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV:
      dss_errno = QDS_EFAULT;
      break;

   case DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SESSION_CHANGED_EV:
      break;

   case DSS_IFACE_IOCTL_PREFIX_UPDATE_EV:
   case DSS_IFACE_IOCTL_PRIMARY_QOS_MODIFY_RESULT_EV:
      break;

   case DSS_IFACE_IOCTL_MT_REQUEST_EV:
      dss_errno = QDS_EFAULT;
      break;

   case DSS_IFACE_IOCTL_NONREG_EVENT_MAX:
      break;

   case DSS_IFACE_IOCTL_QOS_EVENT_MIN:
   case DSS_IFACE_IOCTL_QOS_AVAILABLE_EV: // deprecated
   case DSS_IFACE_IOCTL_QOS_AVAILABLE_MODIFIED_EV:
   case DSS_IFACE_IOCTL_QOS_AVAILABLE_DEACTIVATED_EV:
   case DSS_IFACE_IOCTL_QOS_UNAVAILABLE_EV:
   case DSS_IFACE_IOCTL_QOS_MODIFY_ACCEPTED_EV:
   case DSS_IFACE_IOCTL_QOS_MODIFY_REJECTED_EV:
   case DSS_IFACE_IOCTL_QOS_INFO_CODE_UPDATED_EV:
   case DSS_IFACE_IOCTL_QOS_GROUP_EVENT: // not an event
   case DSS_IFACE_IOCTL_QOS_NET_INITIATED_AVAILABLE_EV:
      dss_errno = QDS_EFAULT;
      break;

#ifdef FEATUTE_DATA_PS_MCAST
   case DSS_IFACE_IOCTL_MCAST_EVENT_MIN:
   case DSS_IFACE_IOCTL_MCAST_REGISTER_SUCCESS_EV:
   case DSS_IFACE_IOCTL_MCAST_REGISTER_FAILURE_EV:
   case DSS_IFACE_IOCTL_MCAST_DEREGISTERED_EV:
   case DSS_IFACE_IOCTL_MCAST_STATUS_EV:
   case DSS_IFACE_IOCTL_MCAST_EVENT_MAX:
#endif // FEATUTE_DATA_PS_MCAST
   case DSS_IFACE_IOCTL_MBMS_CONTEXT_EVENT_MIN:
   case DSS_IFACE_IOCTL_MBMS_CONTEXT_ACT_SUCCESS_EV:
   case DSS_IFACE_IOCTL_MBMS_CONTEXT_ACT_FAILURE_EV:
   case DSS_IFACE_IOCTL_MBMS_CONTEXT_DEACT_SUCCESS_EV:
   case DSS_IFACE_IOCTL_MBMS_CONTEXT_DEACT_FAILURE_EV:
   case DSS_IFACE_IOCTL_MBMS_CONTEXT_EVENT_MAX:
      break;

   case DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_MIN:
   case DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_GENERATED_EV:
   case DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DEPRECATED_EV:
   case DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DELETED_EV:
   case DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_MAX:
      dss_errno = QDS_EFAULT;
      break;

   case DSS_IFACE_IOCTL_EVENT_MAX:
   case DSS_IFACE_IOCTL_EVENT_FORCE_32_BIT:
      break;

   case DSS_IFACE_IOCTL_HANDOFF_INIT_EV:
   case DSS_IFACE_IOCTL_HANDOFF_STATUS_SUCCESS_EV:
   case DSS_IFACE_IOCTL_HANDOFF_STATUS_FAILURE_EV:
      break;

   case DSS_IFACE_IOCTL_EXTENDED_IP_CONFIG_EX_EV:
      break;

   case DSS_IFACE_IOCTL_APN_PARAM_CHANGE_EV:
      break;

   default:
     dss_errno = QDS_EFAULT;

   }

   if (dss_errno != DSS_SUCCESS)
   {
      LOG_MSG_ERROR_2("dss_iface_ioctl_reg_event_cb_explicitly_registerable(): event %d errno %d",
                      (int)(event), dss_errno);
   }

   return dss_errno;
}

static sint15 dss_iface_ioctl_reg_event_cb
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15 *dss_errno,
   dss_iface_id_type ifaceId
)
{
  LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_reg_event_cb(): event: %d",
                           (int)((reinterpret_cast<dss_iface_ioctl_ev_cb_type*>(argval_ptr))->event));

  // check whether it's possible to explicitly register to this event
  IDS_ERR_RET_ERRNO(dss_iface_ioctl_reg_event_cb_explicitly_registerable((reinterpret_cast<dss_iface_ioctl_ev_cb_type*>(argval_ptr))->event));

  IDS_ERR_RET_ERRNO(pNetMonitored->RegEventCB(reinterpret_cast<dss_iface_ioctl_ev_cb_type*>(argval_ptr), ifaceId));
  return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_dereg_event_cb
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_dereg_event_cb(): event: %d",
                            (int)((reinterpret_cast<dss_iface_ioctl_ev_cb_type*>(argval_ptr))->event));
   IDS_ERR_RET_ERRNO(pNetMonitored->DeregEventCB(reinterpret_cast<dss_iface_ioctl_ev_cb_type*>(argval_ptr)));
   return DSS_SUCCESS;
}

 //TODO: the types are different in the IDS and ds APIs - need to ask
static sint15 dss_iface_ioctl_get_hw_addr
(
   void *argval_ptr,
   DSSIDSNetworkScope* IDSNetworkScope,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_hw_addr(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   HWAddressType idsHwAddress;
   memset(&idsHwAddress, 0, sizeof(HWAddressType));

   //TODO: is the len field an input parameter?
   IDS_ERR_RET_ERRNO(IDSNetworkScope->Fetch()->GetHWAddress(&idsHwAddress));
   dss_iface_ioctl_hw_addr_type* pdssHwAddress =
      reinterpret_cast<dss_iface_ioctl_hw_addr_type*>(argval_ptr);
   if (pdssHwAddress->hw_addr_len < idsHwAddress.len) {
      *dss_errno = DS_EINVAL; //TODO: what's the appropriate error?
      return DSS_ERROR;
   }
   pdssHwAddress->hw_addr_len = idsHwAddress.len; //TODO: ok?
   //TODO: what memsmove/memscpy should be used?
   (void)memsmove(pdssHwAddress->hw_addr, idsHwAddress.len,
     idsHwAddress.hwAddr, idsHwAddress.len);
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_iface_name
(
   void *argval_ptr,
   DSSIDSNetworkScope* IDSNetworkScope,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_iface_name(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }
   ds::Net::IfaceNameType idsIface = 0;
   IDS_ERR_RET_ERRNO(IDSNetworkScope->Fetch()->GetIfaceName(&idsIface));
   (void)memsmove(argval_ptr, sizeof(dss_iface_ioctl_iface_name_type),
     &idsIface, sizeof(dss_iface_ioctl_iface_name_type));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_ip_family
(
   void *argval_ptr,
   DSSIDSNetworkScope* IDSNetworkScope,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_ip_family(): argval_ptr: 0x%p",
                            argval_ptr);
   ds::AddrFamilyType ipFamily = 0;

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   IDS_ERR_RET_ERRNO(IDSNetworkScope->Fetch()->GetAddressFamily(&ipFamily));
   IDS_ERR_RET_ERRNO(DSSConversion::IDS2DSEnumAddrFamily(ipFamily, reinterpret_cast<dss_iface_ioctl_ip_family_type*>(argval_ptr)));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_bearer_technology
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_bearer_technology(): argval_ptr: 0x%p",
                            argval_ptr);
   
   if (NULL == argval_ptr)
   {
     if (NULL != dss_errno)
     {
       *dss_errno = DS_EINVAL;
     }
     return DSS_ERROR;
   }

   IDS_ERR_RET_ERRNO(pNetMonitored->GetBearerTech(reinterpret_cast<dss_iface_ioctl_bearer_tech_type*>(argval_ptr)));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_bearer_technology_ex
(
 void *argval_ptr,
 DSSNetMonitored* pNetMonitored,
 sint15  *dss_errno
 )
{
  LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_bearer_technology_ex(): argval_ptr: 0x%p",
                           argval_ptr);

  if (NULL == argval_ptr)
  {
    if (NULL != dss_errno)
    {
      *dss_errno = DS_EINVAL;
    }
    return DSS_ERROR;

  }

  IDS_ERR_RET_ERRNO(pNetMonitored->GetBearerTechEx(reinterpret_cast<dss_iface_ioctl_bearer_tech_ex_type*>(argval_ptr)));
  return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_data_path
(
   void* argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15* dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1(
"dss_iface_ioctl_get_data_path(): argval_ptr: 0x%p", argval_ptr);

   if (NULL == argval_ptr)
   {
      if (NULL != dss_errno)
      {
         *dss_errno = DS_EINVAL;
      }
      return DSS_ERROR;
   }

   IDS_ERR_RET_ERRNO(pNetMonitored->GetDataPathBridgeMode(reinterpret_cast<dss_iface_ioctl_data_path_bridge_mode_type*>(argval_ptr)));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_op_res_pco
(
 void* argval_ptr,
 DSSNetMonitored* pNetMonitored,
 sint15* dss_errno
 )
{
  LOG_MSG_FUNCTION_ENTRY_1(
    "dss_iface_ioctl_get_op_res_pco(): argval_ptr: 0x%p", argval_ptr);

  if (NULL == argval_ptr)
  {
    if (NULL != dss_errno)
    {
      *dss_errno = DS_EINVAL;
    }
    return DSS_ERROR;
  }

  IDS_ERR_RET_ERRNO(pNetMonitored->GetOpResPco(reinterpret_cast<dss_iface_op_reserved_pco_type*>(argval_ptr)));
  return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_3gpp_lte_reestab_calltype
(
   void* argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15* dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1(
      "dss_iface_ioctl_get_3gpp_lte_reestab_calltype(): argval_ptr: 0x%p", argval_ptr);

   if (NULL == argval_ptr)
   {
      if (NULL != dss_errno)
      {
         *dss_errno = DS_EINVAL;
      }
      return DSS_ERROR;
   }

   IDS_ERR_RET_ERRNO(pNetMonitored->_3GPPLTEReestabCalltypeGet(
      reinterpret_cast<dss_iface_ioctl_3gpp_get_lte_reestab_calltype_type*>(argval_ptr)));

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_set_3gpp_lte_reestab_calltype
(
   void* argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15* dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1(
      "dss_iface_ioctl_set_3gpp_lte_reestab_calltype(): argval_ptr: 0x%p", argval_ptr);

   if (NULL == argval_ptr)
   {
      if (NULL != dss_errno)
      {
         *dss_errno = DS_EINVAL;
      }
      return DSS_ERROR;
   }

   IDS_ERR_RET_ERRNO(pNetMonitored->_3GPPLTEReestabCalltypeSet(
      reinterpret_cast<dss_iface_ioctl_3gpp_set_lte_reestab_calltype_type*>(argval_ptr)));

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_set_silent_redial
(
   void* argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15* dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1(
      "dss_iface_ioctl_set_silent_redial(): argval_ptr: 0x%p", argval_ptr);

   IDS_ERR_RET_ERRNO(pNetMonitored->SetSilentRedial(
     reinterpret_cast<dss_iface_ioctl_set_silent_redial_type*>(argval_ptr)));

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_3gpp_force_pdn_disconnect
(
   DSSNetMonitored* pNetMonitored,
   sint15* dss_errno
)
{	   	
   if (NULL == dss_errno)
   {
     return DSS_ERROR;
   }
   IDS_ERR_RET_ERRNO(pNetMonitored->_3GPPFORCEPDNdisconnect());
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_set_keep_alive
(
  void* argval_ptr,
  DSSNetMonitored* pNetMonitored,
  sint15* dss_errno
)
{
  if (NULL == argval_ptr)
   {
      LOG_MSG_ERROR_0("dss_iface_ioctl_set_keep_alive(): argval_ptr is Null");
      if (NULL != dss_errno)
      {
         *dss_errno = DS_EINVAL;
      }
      return DSS_ERROR;
   }
   IDS_ERR_RET_ERRNO(pNetMonitored->Setkeepalive(reinterpret_cast<dss_iface_keep_alive_flag_type *>(argval_ptr)));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_set_data_path_bridge_mode
(
 void* argval_ptr,
 DSSNetMonitored* pNetMonitored,
 sint15* dss_errno
 )
{
  LOG_MSG_FUNCTION_ENTRY_1(
"dss_iface_ioctl_set_data_path_bridge_mode(): argval_ptr: 0x%p", argval_ptr);

  if (NULL == argval_ptr)
  {
    if (NULL != dss_errno)
    {
      *dss_errno = DS_EINVAL;
    }
    return DSS_ERROR;
  }

  IDS_ERR_RET_ERRNO(pNetMonitored->SetDataPathBridgeMode(
    reinterpret_cast<dss_iface_ioctl_data_path_bridge_mode_type*>(argval_ptr)));

  return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_set_pdn_teardown_config
(
 void* argval_ptr,
 DSSNetMonitored* pNetMonitored,
 sint15* dss_errno
 )
{

  if (NULL == argval_ptr)
  {
    if (NULL != dss_errno)
    {
      *dss_errno = DS_EINVAL;
    }
    return DSS_ERROR;
  }

  IDS_ERR_RET_ERRNO(pNetMonitored->SetPDNTeardownConfig(
    reinterpret_cast<dss_iface_ioctl_pdn_teardown_config_type*>(argval_ptr)));
  
  return DSS_SUCCESS;
} /* dss_iface_ioctl_set_pdn_teardown_config */


static sint15 dss_iface_ioctl_get_pdn_teardown_config
(
 void* argval_ptr,
 DSSNetMonitored* pNetMonitored,
 sint15* dss_errno
 )
{
  if (NULL == argval_ptr)
  {
    if (NULL != dss_errno)
    {
      *dss_errno = DS_EINVAL;
    }
    return DSS_ERROR;
  }

  IDS_ERR_RET_ERRNO(pNetMonitored->GetPDNTeardownConfig(
    reinterpret_cast<dss_iface_ioctl_pdn_teardown_config_type*>(argval_ptr)));
  
  return DSS_SUCCESS;
} /* dss_iface_ioctl_get_pdn_teardown_config */

static sint15 dss_iface_ioctl_get_data_bearer_rate
(
   void *argval_ptr,
   DSSIDSNetworkExtScope* IDSNetworkExtScope,
   sint15  *dss_errno
)
{
   IBearerInfo* bearerInfo;
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_data_bearer_rate(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   // The types are identical in the old and new APIs.
   IDS_ERR_RET_ERRNO(IDSNetworkExtScope->Fetch()->GetBearerInfo(&bearerInfo));
   IDS_ERR_RET_ERRNO(bearerInfo->GetRate(reinterpret_cast<BearerTechRateType*>(argval_ptr)));
   DSSCommon::ReleaseIf((IQI**)&bearerInfo);
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_mip_ma_info
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15 *dss_errno
)
{
   dss_iface_ioctl_mip_ma_info_type* pApiMipInfo = NULL;
   ds::IPAddrType dsnetMipInfo;
   DSSIDSNetworkScope IDSNetworkScope;
   ds::Net::INetwork* pIDSNetwork = NULL;
   sint15 nRet = DSS_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_mip_ma_info(): argval_ptr: 0x%p",
                            argval_ptr);
   pApiMipInfo = reinterpret_cast<dss_iface_ioctl_mip_ma_info_type*>(argval_ptr);

   // get dsnet network - ref count +1
   BAIL_ERRNO(pNetActive->GetIDSNetworkObject(&pIDSNetwork));
   BAIL_ERRNO(pIDSNetwork->GetMipInfo(&dsnetMipInfo));
   BAIL_ERRNO(DSSConversion::IDS2DSIpAddr(&dsnetMipInfo, &pApiMipInfo->fa_addr));

bail:
   if(pIDSNetwork != NULL)
   {
      pIDSNetwork->Release();
   }
   return nRet;
}

static sint15 dss_iface_ioctl_is_laptop_call_active
(
   void *argval_ptr,
   DSSIDSNetworkPrivScope* IDSNetworkPrivScope,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_is_laptop_call_active(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   IDS_ERR_RET_ERRNO(IDSNetworkPrivScope->Fetch()->IsLaptopCallActive(reinterpret_cast<boolean*>(argval_ptr)));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_sip_serv_addr
(
   void *argval_ptr,
   DSSIDSNetworkScope* IDSNetworkScope,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_sip_serv_addr(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_sip_serv_addr_info_type* arg =
      reinterpret_cast<dss_iface_ioctl_sip_serv_addr_info_type*>(argval_ptr);
   int nAvailable = 0;
   // TODO: memory allocation ok? need to check ret value? throws exception?

   ds::IPAddrType* sipServerAddresses = NULL;
   if (arg->count > 0) {
      PS_SYSTEM_HEAP_MEM_ALLOC(sipServerAddresses,sizeof(ds::IPAddrType)*(arg->count),
                               ds::IPAddrType*);
      if (NULL == sipServerAddresses) {
         *dss_errno = DS_ENOMEM;
         return DSS_ERROR;
      }
   }

   DSSGenScope GenScopeObject(sipServerAddresses,DSSGenScope::GEN_SCRATCHPAD_ARRAY);

   IDS_ERR_RET_ERRNO(IDSNetworkScope->Fetch()->GetSIPServerAddr(sipServerAddresses,
                                                                    static_cast<int>(arg->count),
                                                                    &nAvailable));
   arg->count = min(arg->count, (uint32)nAvailable);
   IDS_ERR_RET_ERRNO(DSSConversion::IDS2DSArray(sipServerAddresses, arg->addr_array,
      static_cast<int>(arg->count),
      DSSConversion::IDS2DSIpAddr));

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_sip_serv_domain_names
(
   void *argval_ptr,
   DSSIDSNetworkScope* IDSNetworkScope,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_sip_serv_domain_names(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_sip_serv_domain_name_info_type* arg =
      reinterpret_cast<dss_iface_ioctl_sip_serv_domain_name_info_type*>(argval_ptr);
   int nAvailable = 0;

   DomainName* sipServerDomainNames = NULL;
   if (arg->count > 0) {
      PS_SYSTEM_HEAP_MEM_ALLOC(sipServerDomainNames, sizeof(DomainName)*(arg->count),
                               DomainName*);
      if (NULL == sipServerDomainNames) {
         *dss_errno = DS_ENOMEM;
         return DSS_ERROR;
      }
   }

   DSSGenScope GenScopeObject(sipServerDomainNames, DSSGenScope::DS_Network_DomainName_ARRAY_PTR, arg->count);

   IDS_ERR_RET_ERRNO(IDSNetworkScope->Fetch()->GetSIPServerDomainNames(sipServerDomainNames,
                                                          static_cast<int>(arg->count),
                                                          &nAvailable));
   arg->count = min(arg->count, (uint32)nAvailable);
   IDS_ERR_RET_ERRNO(DSSConversion::IDS2DSArray(static_cast<const DomainName*>(sipServerDomainNames),
                                                arg->name_array,
                                                static_cast<int>(arg->count),
                                                DSSConversion::IDS2DSDomainName));

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_domain_name_search_list
(
   void *argval_ptr,
   DSSIDSNetworkScope* IDSNetworkScope,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_domain_name_search_list(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_domain_name_search_list_type* arg =
      reinterpret_cast<dss_iface_ioctl_domain_name_search_list_type*>(argval_ptr);
   int nAvailable = 0;
   // TODO: memory allocation ok? need to check ret value? throws exception?
   DomainName* domainNameSearchList = NULL;
   if (arg->count > 0) {
      PS_SYSTEM_HEAP_MEM_ALLOC(domainNameSearchList, sizeof(DomainName)*(arg->count), 
                               DomainName*);
      if (NULL == domainNameSearchList) {
         *dss_errno = DS_ENOMEM;
         return DSS_ERROR;
      }
   }

   DSSGenScope GenScopeObject(domainNameSearchList, DSSGenScope::DS_Network_DomainName_ARRAY_PTR, arg->count);

   IDS_ERR_RET_ERRNO(IDSNetworkScope->Fetch()->GetDomainNameSearchList(domainNameSearchList,
                                                          static_cast<int>(arg->count),
                                                          &nAvailable));

   arg->count = min((uint32)nAvailable, arg->count);
   IDS_ERR_RET_ERRNO(DSSConversion::IDS2DSArray(static_cast<const DomainName*>(domainNameSearchList),
                                                arg->name_array,
                                                static_cast<int>(arg->count),
                                                DSSConversion::IDS2DSDomainName));

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_refresh_dhcp_config_info
(
   DSSIDSNetworkScope* IDSNetworkScope,
   sint15  *dss_errno
)
{
   INetworkControl *pNetworkControl = NULL;
   AEEResult res = AEE_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_0("dss_iface_ioctl_refresh_dhcp_config_info(): ");

   IDS_ERR_RET_ERRNO(IDSNetworkScope->Fetch()->QueryInterface(AEEIID_INetworkControl, (void**)&pNetworkControl));

   res = pNetworkControl->RefreshDHCPConfigInfo();

   DSSCommon::ReleaseIf((IQI**)&pNetworkControl);

   if (AEE_SUCCESS != res) {
     *dss_errno = DSSConversion::IDS2DSErrorCode(res);
     return DSS_ERROR;
   }

   return DSS_SUCCESS;
}
static sint15 dss_iface_ioctl_go_active
(
   void *argval_ptr,
   DSSNetApp* pNetApp,
   DSSNetMonitored* pNetMonitored,
   sint15 *dss_errno,
   dss_iface_id_type iface_id
)
{
   IPhysLink* piPhysLink = NULL;
   AEEResult res = AEE_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_go_active(): iface_id:%u, argval_ptr:0x%p",
                            iface_id, argval_ptr);

   if (NULL != pNetApp)
   {
      res = pNetApp->GetPhysLinkObject(iface_id, &piPhysLink);
      if (AEE_SUCCESS != res){
         // these lines are for backward compatibility
         // GetPhysLinkObject was modified to return QDS_ENETDOWN
         // instead of AEE_EFAILED
         if(QDS_ENETDOWN == res){
            *dss_errno = DSSConversion::IDS2DSErrorCode(AEE_EFAILED);
            return DSS_ERROR;
         }
         else{
            *dss_errno = DSSConversion::IDS2DSErrorCode(res);
            return DSS_ERROR;
         }
      }
   }
   else {
      if (NULL != pNetMonitored)
      {
         // pNetMonitored should be a temporary DSSNetMonitored, created just for GO_ACTIVE IOCTL
         // This scenario is supported for backward compatibility in a case iface ID is received from dss_get_iface_id_by_policy()
         // However, pNetMonitored may be secondary net monitored.
         // TODO: consider not allowing secondary net monitored or NetMonitored at all.
         res = pNetMonitored->GetPhysLinkObject(iface_id, &piPhysLink);
         if (AEE_SUCCESS != res){
            // these lines are for backward compatibility
            // GetPhysLinkObject was modified to return QDS_ENETDOWN
            // instead of AEE_EFAILED
            if(QDS_ENETDOWN == res){
               *dss_errno = DSSConversion::IDS2DSErrorCode(AEE_EFAILED);
               return DSS_ERROR;
            }
            else{
               *dss_errno = DSSConversion::IDS2DSErrorCode(res);
               return DSS_ERROR;
            }
         }
      }
      else
      {
         LOG_MSG_ERROR_0("dss_iface_ioctl_go_dormant(): "
                         "Both pNetApp and pNetMonitored are NULL");
         *dss_errno = DSSConversion::IDS2DSErrorCode(AEE_EFAILED);
         return DSS_ERROR;
      }
   }

   DSSGenScope GenScopeObject(piPhysLink, DSSGenScope::IDSIQI_TYPE);

   IDS_ERR_RET_ERRNO(piPhysLink->GoActive());
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_go_dormant
(
   void *argval_ptr,
   DSSNetApp* pNetApp,
   DSSNetMonitored* pNetMonitored,
   sint15 *dss_errno,
   dss_iface_id_type iface_id
)
{
   IPhysLink* piPhysLink = NULL;
   DormantReasonType dormantReason = DormantReason::QDS_NONE;
   AEEResult res = AEE_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_go_dormant(): iface_id:%u, argval_ptr:0x%p",
                            iface_id, argval_ptr);

   if (NULL != pNetApp)
   {
      res = pNetApp->GetPhysLinkObject(iface_id, &piPhysLink);
      if (AEE_SUCCESS != res){
         // these lines are for backward compatibility
         // GetPhysLinkObject was modified to return QDS_ENETDOWN
         // instead of AEE_EFAILED
         if(QDS_ENETDOWN == res){
           *dss_errno = DSSConversion::IDS2DSErrorCode(AEE_EFAILED);
           return DSS_ERROR;
         }
         else{
           *dss_errno = DSSConversion::IDS2DSErrorCode(res);
           return DSS_ERROR;
         }
      }
   }
   else {
      if (NULL != pNetMonitored)
      {
         res = pNetMonitored->GetPhysLinkObject(iface_id, &piPhysLink);
         if (AEE_SUCCESS != res){
           // these lines are for backward compatibility
           // GetPhysLinkObject was modified to return QDS_ENETDOWN
           // instead of AEE_EFAILED
           if(QDS_ENETDOWN == res){
              *dss_errno = DSSConversion::IDS2DSErrorCode(AEE_EFAILED);
              return DSS_ERROR;
           }
           else{
              *dss_errno = DSSConversion::IDS2DSErrorCode(res);
              return DSS_ERROR;
           }
        }
     }
     else
     {
        LOG_MSG_ERROR_0("dss_iface_ioctl_go_dormant(): "
                        "Both pNetApp and pNetMonitored are NULL");
        *dss_errno = DSSConversion::IDS2DSErrorCode(AEE_EFAILED);
        return DSS_ERROR;
     }
   }

   DSSGenScope GenScopeObject(piPhysLink, DSSGenScope::IDSIQI_TYPE);

   IDS_ERR_RET_ERRNO(piPhysLink->GoDormant(dormantReason));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_go_null
(
   void *argval_ptr,
   DSSIDSNetworkScope* IDSNetworkScope,
   sint15  *dss_errno
)
{
   INetworkControl *pNetworkControl = NULL;
   AEEResult res = AEE_SUCCESS;
   ReleaseReasonType nArg = ReleaseReason::QDS_NONE;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_go_null(): argval_ptr: 0x%p",
                            argval_ptr);

   dss_iface_ioctl_null_arg_type* arg = reinterpret_cast<dss_iface_ioctl_null_arg_type*>(argval_ptr);
   
   if (NULL != arg){
      memscpy(&nArg, sizeof(ReleaseReasonType),
        arg, sizeof(ReleaseReasonType));
   }

   IDS_ERR_RET_ERRNO(IDSNetworkScope->Fetch()->QueryInterface(AEEIID_INetworkControl, (void**)&pNetworkControl));

   res = pNetworkControl->GoNull(nArg);

   DSSCommon::ReleaseIf((IQI**)&pNetworkControl);

   if (AEE_SUCCESS != res) {
     *dss_errno = DSSConversion::IDS2DSErrorCode(res);
     return DSS_ERROR;
   }

   return DSS_SUCCESS;
}
static sint15 dss_iface_ioctl_ipcp_dns_opt
(
   void *argval_ptr,
   DSSIDSNetworkPrivScope* IDSNetworkPrivScope,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_ipcp_dns_opt(): argval_ptr: 0x%p",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   IDS_ERR_RET_ERRNO(IDSNetworkPrivScope->Fetch()->EnableDNSDuringIPCP(*reinterpret_cast<boolean*>(argval_ptr)));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_tx_queue_level
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno,
   dss_iface_id_type  iface_id
)
{
   IfaceIdType flowID;
   IQoS* pNetQoSDefault = NULL;
   DSSNetQoSSecondary* pDSSNetQoSSecondary = NULL;
   DSSIDSNetActiveScope IDSNetActiveScope;
   IQoSSecondary* pNetQoSSecondary = NULL;
   QoSTXQueueLevelType TXQueueLevel;
   dss_iface_ioctl_tx_queue_level_type* pTxQueueLevel = reinterpret_cast<dss_iface_ioctl_tx_queue_level_type*>(argval_ptr);

   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_get_tx_queue_level(): iface_id:%u, argval_ptr:0x%p",
                            iface_id, argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   // Initialize the TXQueueLevel struct
   TXQueueLevel.currentNewDataCnt = 0;
   TXQueueLevel.totalPendingCnt = 0;
   TXQueueLevel.wmFreeCnt = 0;

   // check if the appropriate QoS to be used is the primary.
    IDS_ERR_RET_ERRNO(IDSNetActiveScope.Init(pNetActive));
   IDS_ERR_RET_ERRNO(IDSNetActiveScope.Fetch()->GetIfaceId(&flowID));

   if (DSSIfaceId::CompareIfaceIds((dss_iface_id_type)flowID,iface_id)) {
      IDS_ERR_RET_ERRNO(pNetActive->GetQoSDefault(&pNetQoSDefault));
      DSSGenScope scopeNetQosManager(pNetQoSDefault,DSSGenScope::IDSIQI_TYPE);

      IDS_ERR_RET_ERRNO(pNetQoSDefault->GetTXQueueLevel(&TXQueueLevel));

      pTxQueueLevel->current_new_data_cnt = TXQueueLevel.currentNewDataCnt;
      pTxQueueLevel->wm_free_cnt = TXQueueLevel.wmFreeCnt;
      pTxQueueLevel->total_pending_cnt = TXQueueLevel.totalPendingCnt;
      return DSS_SUCCESS;
   }


   // Look for the appropriate secondary QoS.
   IDS_ERR_RET_ERRNO(pNetActive->GetDSSNetQoSSecondary(iface_id, &pDSSNetQoSSecondary));

   if (NULL != pDSSNetQoSSecondary){
      IDS_ERR_RET_ERRNO(pDSSNetQoSSecondary->GetNetQoSSecondary(&pNetQoSSecondary));
   }

   DSSGenScope GenScopeObject(pNetQoSSecondary, DSSGenScope::IDSIQI_TYPE);

   if (NULL != pNetQoSSecondary){
      IDS_ERR_RET_ERRNO(pNetQoSSecondary->GetTXQueueLevel(&TXQueueLevel));
   }

   pTxQueueLevel->current_new_data_cnt = TXQueueLevel.currentNewDataCnt;
   pTxQueueLevel->wm_free_cnt = TXQueueLevel.wmFreeCnt;
   pTxQueueLevel->total_pending_cnt = TXQueueLevel.totalPendingCnt;
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_qos_net_initiated_request
(
   void*              argval_ptr,
   DSSNetActive*      pNetActive,
   sint15*            dss_errno, 
   dss_iface_id_type  iface_id
)
{
   IQoSManager* pNetQoSManager = NULL;
   IQoSManagerPriv* pNetQoSManagerPriv = NULL;
   IQoSNetInitiatedReq* pNetQoSInitiatedReq = NULL;
   QoSSpecNetInitiatedType requestedQoSSpec;
   DSSGenScope ScopeRxFilter;
   DSSGenScope ScopeTxFilter;

   LOG_MSG_FUNCTION_ENTRY_4("dss_iface_ioctl_qos_net_initiated_request():" 
                            "iface_id:%u, argval_ptr:0x%p, errno: 0x%x, NetActive:0x%x",
                            iface_id, argval_ptr, dss_errno, pNetActive);

   dss_iface_ioctl_qos_net_initiated_request_type* net_initiated_qos_status_request =
      (dss_iface_ioctl_qos_net_initiated_request_type*)argval_ptr;

   if (NULL == dss_errno) {
      return DSS_ERROR;
   }

   if (NULL == net_initiated_qos_status_request->cback_fn) {
      LOG_MSG_ERROR_0 ("dss_iface_ioctl_qos_net_initiated_request(): "
                       "net_initiated_qos_status_request->cback_fn is NULL");
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   if (NULL == pNetActive) {
      *dss_errno = DS_ENETDOWN;
      return DSS_ERROR;
   }

   memset(&requestedQoSSpec, 0, sizeof(QoSSpecNetInitiatedType));

   // Convert the ds api qos specification to QCM API SpecType.
   IDS_ERR_RET_ERRNO(DSSConversion::DS2IDSQoSNetInitiatedSpec(
      &net_initiated_qos_status_request->net_initiated_qos_spec,  // The ds api qos specification
      &requestedQoSSpec));                                        // The QCM API qos specification

   ScopeRxFilter.SetParams(requestedQoSSpec.rxFilters,
      DSSGenScope::IDSNetIPFilter_ARRAY_PTR, net_initiated_qos_status_request->net_initiated_qos_spec.rx_fltr_template.num_filters);
   ScopeTxFilter.SetParams(requestedQoSSpec.txFilters,
      DSSGenScope::IDSNetIPFilter_ARRAY_PTR, net_initiated_qos_status_request->net_initiated_qos_spec.tx_fltr_template.num_filters);


   IDS_ERR_RET(pNetActive->GetNetQoSManager(&pNetQoSManager));
   DSSGenScope scopeNetQosManager(pNetQoSManager,DSSGenScope::IDSIQI_TYPE);

   IDS_ERR_RET(pNetQoSManager->QueryInterface(AEEIID_IQoSManagerPriv, (void**)&pNetQoSManagerPriv));
   DSSGenScope scopeNetQosManagerPriv(pNetQoSManagerPriv,DSSGenScope::IDSIQI_TYPE);

   // Create the QoSNetInitiated object that shall be used to set the QoS specification on the iface.
   ds::ErrorType nRes2 = pNetQoSManagerPriv->RequestNetInitiatedReq(&requestedQoSSpec, &pNetQoSInitiatedReq);
   DSSGenScope scopeNetInitiatedQos(pNetQoSInitiatedReq,DSSGenScope::IDSIQI_TYPE);

   if ((AEE_SUCCESS != nRes2) && (AEE_EWOULDBLOCK != nRes2)){
      *dss_errno = DSSConversion::IDS2DSErrorCode(nRes2);
      return DSS_ERROR;
   }

   // find a free QoS Net Initiated handle and set in output parameter to application
   IDS_ERR_RET_ERRNO(DSSGlobals::Instance()->GetFreeQoSNetInitiatedReqHandle(&(net_initiated_qos_status_request->handle)));

   LOG_MSG_INFO1_1("dss_iface_ioctl_qos_net_initiated_request(): "
                   "Received qos net initiated request handle 0x%x",
                   net_initiated_qos_status_request->handle);

   boolean real_flag = FALSE;
   net_initiated_qos_status_request->qos_handle = 0;
   if(AEE_SUCCESS == nRes2)
   {
     real_flag = TRUE;
   }
   // Add the newly created pNetQoSInitiatedReq to pNetActive.
   ds::ErrorType nRes3 = pNetActive->AddQoSNetInitiatedReq(
      pNetQoSInitiatedReq,                         // dsnet object pointer
      net_initiated_qos_status_request->handle,    // DSS identifer of the request
      net_initiated_qos_status_request->cback_fn,  // App CB
      net_initiated_qos_status_request->user_data, // App user-data
      &(net_initiated_qos_status_request->qos_handle),
      iface_id,
      real_flag);
   if (AEE_SUCCESS != nRes3){
      DSSGlobals::Instance()->ReleaseQoSNetInitiatedReqHandle(net_initiated_qos_status_request->handle); 
      *dss_errno = DSSConversion::IDS2DSErrorCode(nRes3);
      return DSS_ERROR;
   }

   LOG_MSG_FUNCTION_EXIT_3("dss_iface_ioctl_qos_net_initiated_request(): iface_id:%u, "
                           "qos net initiated request handle 0x%x, qos net initiated request qos_handle 0x%x",
                           iface_id,net_initiated_qos_status_request->handle, net_initiated_qos_status_request->qos_handle);
   if (AEE_EWOULDBLOCK == nRes2) {
     *dss_errno = DSSConversion::IDS2DSErrorCode(nRes2);     
   }
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_qos_net_initiated_release
(
   void *             argval_ptr,
   DSSNetActive*      pNetActive,
   sint15 *           dss_errno,
   dss_iface_id_type  iface_id
)
{
   DSSQoSNetInitiatedReq *pDSSNetQoSNetInitiatedReq = NULL;
   sint15 nRet = DSS_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_qos_net_initiated_release(): iface_id:%u, argval_ptr:0x%p",
                            iface_id, argval_ptr);

   dss_iface_ioctl_qos_net_initiated_release_type* net_initiated_qos_status_release =
      (dss_iface_ioctl_qos_net_initiated_release_type*)argval_ptr;   

   if ( (net_initiated_qos_status_release->handle < 1) || 
        (net_initiated_qos_status_release->handle > NUM_OF_QOS_NET_INITIATED_REQUESTS) ) {
      LOG_MSG_ERROR_1 ("dss_iface_ioctl_qos_net_initiated_release(): "
                       "out of limits handle provided: 0x%x. Ignoring",
                       net_initiated_qos_status_release->handle);
      IDS_ERR_RET_ERRNO(QDS_EFAULT);
   }
   
   BAIL_ERRNO( pNetActive->RemoveDSSNetQoSNetInitiatedReqFromList(
      net_initiated_qos_status_release->handle,
      &pDSSNetQoSNetInitiatedReq) );

   // Delete the address of the pDSSNetQoSNetInitiatedReq object from all matching DSSQoSNetInitiated objects
   BAIL_ERRNO(pNetActive->RemoveDSSNetQoSNetInitiatedAssocReq(pDSSNetQoSNetInitiatedReq));

   BAIL_ERRNO( pDSSNetQoSNetInitiatedReq->Close() );

   // QoS Net Initiated handle is released in DSSNetQoSNetInitiatedReq::Destructor

   nRet = DSS_SUCCESS;

bail:
   // delete the DSSQoSNetInitiatedReq instance.
   PS_MEM_RELEASE(pDSSNetQoSNetInitiatedReq);

   LOG_MSG_FUNCTION_EXIT_3("dss_iface_ioctl_qos_net_initiated_release(): iface_id:%u, "
                           "qos net initiated request handle 0x%x, ret %d",
                           iface_id,
                           net_initiated_qos_status_release->handle,
                           nRet);
   return nRet;
}

static sint15 dss_iface_ioctl_qos_request
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno,
   dss_iface_id_type  iface_id
)
{
   IQoSManager* pNetQoSManager = NULL;
   IQoSManagerPriv* pNetQoSManagerPriv = NULL;
   IQoSSecondary* pNetQoSSecondary = NULL;

   QoSSpecType requestedQoSSpec;
   uint8 flowID;

   DSSGenScope ScopeRxFlows;
   DSSGenScope ScopeTxFlows;
   DSSGenScope ScopeRxFilter;
   DSSGenScope ScopeTxFilter;

   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_qos_request(): iface_id:%u, argval_ptr:0x%p",
                            iface_id, argval_ptr);

   memset(&requestedQoSSpec, 0, sizeof(QoSSpecType));

   dss_iface_ioctl_qos_request_type* qos_request =
      (dss_iface_ioctl_qos_request_type*)argval_ptr;

   // Convert the old qos_request to the new SpecType.
   IDS_ERR_RET_ERRNO(DSSConversion::DS2IDSQoSSpec(qos_request, //The old QoSSpec request type
                                                  &requestedQoSSpec, // The new ds QoSSpec
                                                  FALSE, // We want to create new QoS requst, no modify operation
                                                  NULL)); // Ignore the modify mask.

   ScopeRxFlows.SetParams(requestedQoSSpec.rxFlows, DSSGenScope::IDSNetQoSFlow_ARRAY_PTR, requestedQoSSpec.rxFlowsLen);
   ScopeTxFlows.SetParams(requestedQoSSpec.txFlows, DSSGenScope::IDSNetQoSFlow_ARRAY_PTR, requestedQoSSpec.txFlowsLen);
   ScopeRxFilter.SetParams(requestedQoSSpec.rxFilter, DSSGenScope::IDSNetIPFilter_ARRAY_PTR, qos_request->qos.rx.fltr_template.num_filters);
   ScopeTxFilter.SetParams(requestedQoSSpec.txFilter, DSSGenScope::IDSNetIPFilter_ARRAY_PTR, qos_request->qos.tx.fltr_template.num_filters);

   IDS_ERR_RET(pNetActive->GetNetQoSManager(&pNetQoSManager));
   DSSGenScope scopeNetQosManager(pNetQoSManager,DSSGenScope::IDSIQI_TYPE);

   IDS_ERR_RET(pNetQoSManager->QueryInterface(AEEIID_IQoSManagerPriv, (void**)&pNetQoSManagerPriv));
   DSSGenScope scopeNetQosManagerPriv(pNetQoSManagerPriv,DSSGenScope::IDSIQI_TYPE);


   // Create the NetQoSSecondary object.
   ds::ErrorType nRes2 = pNetQoSManagerPriv->RequestSecondary(&requestedQoSSpec, &pNetQoSSecondary);
   DSSGenScope scopeNetQosSecondary(pNetQoSSecondary,DSSGenScope::IDSIQI_TYPE);

   if (AEE_SUCCESS != nRes2) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(nRes2);
      (void) DSSConversion::IDS2DSQoSSpecErrMask(&requestedQoSSpec,qos_request);
      LOG_MSG_ERROR_2("dss_iface_ioctl_qos_request(): "
                      "RequestSecondary failed, error %d, errno %d",
                      nRes2, dss_errno);
      return DSS_ERROR;
   }

   // find a free flow ID and Add it as the 8 LSB of iface_ID
   flowID = 0;
   IDS_ERR_RET_ERRNO(DSSGlobals::Instance()->GetFreeQoSFlowID(&flowID));

   // We can't be sure that iface id already contains the app id , let's build it again , and add a flow id .
   uint8 ifaceIndex = 0;
   sint15 app_id = 0;
   ifaceIndex = static_cast<uint8>(DSSIfaceId::GetIfaceIndex(iface_id));
   pNetActive->GetNetHandle(&app_id);
   iface_id = DSSIfaceId::BuildIfaceIdWithAppId(ifaceIndex,app_id);

   iface_id |= flowID;

   // update the qos handle for the user
   qos_request->handle = iface_id;

   // Add the new created pNetQoSSecondary to pNetActive.
   nRes2 = pNetActive->AddNetQoSSecondary(pNetQoSSecondary,
      iface_id,
      qos_request->cback_fn,
      qos_request->user_data);
   if (AEE_SUCCESS != nRes2){
      DSSGlobals::Instance()->ReleaseQoSFlowID(flowID); // flowID must be released
      *dss_errno = DSSConversion::IDS2DSErrorCode(nRes2);
      LOG_MSG_ERROR_2("dss_iface_ioctl_qos_request(): "
                      "AddNetQoSSecondary failed, error %d, errno %d",
                      nRes2, dss_errno);
      return DSS_ERROR;
   }

   LOG_MSG_FUNCTION_EXIT_1("dss_iface_ioctl_qos_request(): Received qos handle 0x%x",
                           qos_request->handle);
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_qos_release
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   DSSNetQoSSecondary* pDSSNetQoSSecondary = NULL;
   IQoSSecondary* pIDSNetQoSSecondary = NULL;
   AEEResult res = DSS_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_qos_release(): argval_ptr: 0x%p",
                            argval_ptr);

   dss_iface_ioctl_qos_release_type* qos_release =
      (dss_iface_ioctl_qos_release_type*)argval_ptr;

   LOG_MSG_INFO1_1("dss_iface_ioctl_qos_release(): Processing handle 0x%x",
                   qos_release->handle);

   // Get the DSSNetQoSSecondary Structure according to the provided QoSHandle.
   res = pNetActive->GetDSSNetQoSSecondary(qos_release->handle, &pDSSNetQoSSecondary);
   if (AEE_SUCCESS != res) {
     AEEResult resNIQ = DSS_SUCCESS;
     LOG_MSG_ERROR_1("dss_iface_ioctl_qos_release(): Failed in GetDSSNetQoSSecondary, error %d, "
                     "will try NIQ", res);
      
     // Search in DSSQoSNetInitiated list
      resNIQ = pNetActive->RemoveDSSNetQoSNetInitiatedFromList(qos_release->handle);
      if (AEE_SUCCESS != resNIQ) {
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         LOG_MSG_ERROR_2("dss_iface_ioctl_qos_release(): Failed in "
                         "RemoveDSSNetQoSNetInitiatedFromList, error %d, errno %d",
                         resNIQ, dss_errno);
         return res;
      }
     return DSS_SUCCESS;
   }

   // get the NetQoSSecondary object
   if (NULL != pDSSNetQoSSecondary){
      res = pDSSNetQoSSecondary->GetNetQoSSecondary(&pIDSNetQoSSecondary);
      if (AEE_SUCCESS != res) {
        *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         LOG_MSG_ERROR_2("dss_iface_ioctl_qos_release(): Failed in "
                         "GetNetQoSSecondary, error %d, errno %d",
                         res, dss_errno);
         return res;
      }
   }

   DSSGenScope GenScopeObject(pIDSNetQoSSecondary, DSSGenScope::IDSIQI_TYPE);

   if (NULL != pIDSNetQoSSecondary){
      res = pIDSNetQoSSecondary->Close();
      if (AEE_SUCCESS != res) {
        *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         LOG_MSG_ERROR_2("dss_iface_ioctl_qos_release(): "
                         "Failed in Close, error %d, errno %d",
                         res, dss_errno);
         return res;
      }
   }

   LOG_MSG_FUNCTION_EXIT_1("dss_iface_ioctl_qos_release(): QoS handle 0x%x released successfuly",
                           qos_release->handle);
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_qos_get_granted_flow_spec
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   DSSNetQoSSecondary* pDSSNetQoSSecondary = NULL;
   DSSQoSNetInitiated* pDSSQoSNetInitiated = NULL;
   AEEResult res = AEE_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_qos_get_granted_flow_spec(): argval_ptr: 0x%p",
                            argval_ptr);

   dss_iface_ioctl_qos_get_flow_spec_type* qos_flow_spec =
      (dss_iface_ioctl_qos_get_flow_spec_type*)argval_ptr;

   // Get the DSSNetQoSSecondary or DSSQoSNetInitiated object according to the provided QoSHandle.   
   res = pNetActive->GetDSSNetQoSSecondary(qos_flow_spec->handle, &pDSSNetQoSSecondary);
   if ( (AEE_SUCCESS != res) ||  (NULL == pDSSNetQoSSecondary) ) {                                  
      // No QoSSecondary object corresponding to provided qos_handle was found
      // Search if there is a corresponding QoSNetInitiated object
      res = pNetActive->GetDSSQoSNetInitiated(qos_flow_spec->handle, &pDSSQoSNetInitiated);
      if ( (AEE_SUCCESS != res) ||  (NULL == pDSSQoSNetInitiated) ) {                                  
         // Could not find corresponding QoSSecondary nor QoSNetInitiated objects
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         LOG_MSG_ERROR_2("dss_iface_ioctl_qos_get_granted_flow_spec(): "
                         "QoSSecondary or QoSNetInitiated not found, error %d, errno %d",
                         res, dss_errno);
         return DSS_ERROR;
      }
      IDS_ERR_RET_ERRNO(pDSSQoSNetInitiated->GetGrantedFlowSpec(qos_flow_spec));
      return DSS_SUCCESS;
   }

   // get the NetQoSSecondary object   
   IQoSSecondary* pIDSNetQoSSecondary = NULL;   
   IDS_ERR_RET_ERRNO(pDSSNetQoSSecondary->GetNetQoSSecondary(&pIDSNetQoSSecondary));   

   if(NULL == pIDSNetQoSSecondary){
      *dss_errno = DS_EINVAL;
      LOG_MSG_ERROR_1("dss_iface_ioctl_qos_get_granted_flow_spec(): "
                      "GetNetQoSSecondary failed, errno %d", dss_errno);
      return DSS_ERROR;
   }

   DSSGenScope GenScopeObject(pIDSNetQoSSecondary, DSSGenScope::IDSIQI_TYPE);

   // allow the operation only if the QoSSession is AVAILABLE_MODIFIED
   QoSStateInfoType statusInfo;
   memset(&statusInfo, 0, sizeof(QoSStateInfoType));

   IDS_ERR_RET_ERRNO(pIDSNetQoSSecondary->GetState(&statusInfo));

   if (QoSState::QDS_AVAILABLE_MODIFIED != statusInfo.state) {
      memset (&qos_flow_spec->rx_flow, 0, sizeof(qos_flow_spec->rx_flow));
      memset (&qos_flow_spec->tx_flow, 0, sizeof(qos_flow_spec->tx_flow));
      return DSS_SUCCESS;
   }

   IQoSSecondaryPriv* pIDSNetQoSSecondaryPriv = NULL;
   IDS_ERR_RET_ERRNO(pIDSNetQoSSecondary->QueryInterface(AEEIID_IQoSSecondaryPriv, (void**)&pIDSNetQoSSecondaryPriv));
   DSSGenScope GenScopeSecondaryPriv(pIDSNetQoSSecondaryPriv, DSSGenScope::IDSIQI_TYPE);

   IQoSFlowPriv *txQoSFlow = NULL, *rxQoSFlow = NULL;
   IDS_ERR_RET_ERRNO(pIDSNetQoSSecondaryPriv->GetGrantedFlowSpecPriv(&rxQoSFlow, &txQoSFlow));

   // Convert new QoS types to old QoS Types
   IDS_ERR_RET_ERRNO(DSSConversion::IDS2DSQoSSpecFlow(rxQoSFlow, &(qos_flow_spec->rx_flow)));
   IDS_ERR_RET_ERRNO(DSSConversion::IDS2DSQoSSpecFlow(txQoSFlow, &(qos_flow_spec->tx_flow)));

   DSSCommon::ReleaseIf((IQI**)&(txQoSFlow));
   DSSCommon::ReleaseIf((IQI**)&(rxQoSFlow));
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_qos_get_granted_flow_spec2
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_qos_get_granted_flow_spec2(): argval_ptr: 0x%p",
                            argval_ptr);

   DSSNetQoSSecondary* pDSSNetQoSSecondary = NULL;
   DSSQoSNetInitiated* pDSSQoSNetInitiated = NULL;
   AEEResult res = AEE_SUCCESS;
   dss_iface_ioctl_qos_get_granted_flow_spec2_type* qos_getGrantedFlowSpec2 =
      (dss_iface_ioctl_qos_get_granted_flow_spec2_type*)argval_ptr;

   // Get the DSSNetQoSSecondary or DSSQoSNetInitiated object according to the provided QoSHandle.   
   res = pNetActive->GetDSSNetQoSSecondary(qos_getGrantedFlowSpec2->handle, &pDSSNetQoSSecondary);
   if ( (AEE_SUCCESS != res) ||  (NULL == pDSSNetQoSSecondary) ) {                                  
      // No QoSSecondary object corresponding to provided qos_handle was found
      // Search if there is a corresponding QoSNetInitiated object
      res = pNetActive->GetDSSQoSNetInitiated(qos_getGrantedFlowSpec2->handle, &pDSSQoSNetInitiated);
      if ( (AEE_SUCCESS != res) ||  (NULL == pDSSQoSNetInitiated) ) {                                  
         // Could not find corresponding QoSSecondary nor QoSNetInitiated objects
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         LOG_MSG_ERROR_2("dss_iface_ioctl_qos_get_granted_flow_spec2(): "
                         "QoSSecondary or QoSNetInitiated not found, error %d, errno %d",
                         res, dss_errno);
         return DSS_ERROR;
      }
      IDS_ERR_RET_ERRNO(pDSSQoSNetInitiated->GetGrantedFlowSpec2(qos_getGrantedFlowSpec2));
      return DSS_SUCCESS;
   }

   // get the NetQoSSecondary object   
   IQoSSecondary* pIDSNetQoSSecondary = NULL;   
   IDS_ERR_RET_ERRNO(pDSSNetQoSSecondary->GetNetQoSSecondary(&pIDSNetQoSSecondary)); 
   DSSGenScope GenScopeObject(pIDSNetQoSSecondary, DSSGenScope::IDSIQI_TYPE);
   if (NULL == pIDSNetQoSSecondary){
      LOG_MSG_ERROR_0("dss_iface_ioctl_qos_get_granted_flow_spec2(): "
                      "GetNetQoSSecondary failed.");
      return DSS_ERROR;
   }

   IQoSFlowPriv *txQoSFlow = NULL, *rxQoSFlow = NULL;

   IQoSSecondaryPriv* pIDSNetQoSSecondaryPriv = NULL;
   IDS_ERR_RET_ERRNO(pIDSNetQoSSecondary->QueryInterface(AEEIID_IQoSSecondaryPriv, (void**)&pIDSNetQoSSecondaryPriv));
   DSSGenScope GenScopeSecondaryPriv(pIDSNetQoSSecondaryPriv, DSSGenScope::IDSIQI_TYPE);


   QoSStateInfoType statusInfo;
   memset(&statusInfo, 0, sizeof(QoSStateInfoType));

   // Get the QoS Status
   IDS_ERR_RET_ERRNO(pIDSNetQoSSecondary->GetState(&statusInfo));

   // TODO: move to DSSConversion.cpp
   switch(statusInfo.state)
   {
   case QoSState::QDS_AVAILABLE_MODIFIED:
      qos_getGrantedFlowSpec2->qos_status = QOS_AVAILABLE;
      break;
   case QoSState::QDS_SUSPENDING:
      qos_getGrantedFlowSpec2->qos_status = QOS_SUSPENDING;
      break;
   case QoSState::QDS_UNAVAILABLE:
      qos_getGrantedFlowSpec2->qos_status = QOS_UNAVAILABLE;
      break;
   case QoSState::QDS_INVALID:
      qos_getGrantedFlowSpec2->qos_status = QOS_STATE_INVALID;
      break;
   case QoSState::QDS_ACTIVATING:
      qos_getGrantedFlowSpec2->qos_status = QOS_ACTIVATING;
      break;
   case QoSState::QDS_SUSPENDED:
      qos_getGrantedFlowSpec2->qos_status = QOS_DEACTIVATED;
      break;
   case QoSState::QDS_RELEASING:
      qos_getGrantedFlowSpec2->qos_status = QOS_RELEASING;
      break;
   case QoSState::QDS_CONFIGURING:
      qos_getGrantedFlowSpec2->qos_status = QOS_CONFIGURING;
      break;
   default:
      // TODO: update the errno
      LOG_MSG_ERROR_1("dss_iface_ioctl_qos_get_granted_flow_spec2(): "
                      "Unknown state, state %d", statusInfo.state);
      return DSS_ERROR;
   }

   
   IDS_ERR_RET_ERRNO(pIDSNetQoSSecondaryPriv->GetGrantedFlowSpecPriv(&rxQoSFlow, &txQoSFlow));
   
  // Convert new QoS types to old QoS Types
  IDS_ERR_RET_ERRNO(DSSConversion::IDS2DSQoSSpecFlow(rxQoSFlow, &(qos_getGrantedFlowSpec2->rx_flow)));
  IDS_ERR_RET_ERRNO(DSSConversion::IDS2DSQoSSpecFlow(txQoSFlow, &(qos_getGrantedFlowSpec2->tx_flow)));

  DSSCommon::ReleaseIf((IQI**)&(txQoSFlow));
  DSSCommon::ReleaseIf((IQI**)&(rxQoSFlow));

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_qos_get_granted_filter_spec
(
   void *argval_ptr,
   DSSNetActive*  pNetActive,
   sint15  *dss_errno
)
{
  LOG_MSG_INFO1_1("dss_iface_ioctl_qos_get_granted_filter_spec(): argval_ptr: 0x%p",
                  argval_ptr);

  DSSQoSNetInitiated* pDSSQoSNetInitiated = NULL;
  IQoSNetInitiated*   pIDSNetQoSNetInitiated = NULL;
  int rxFilterLenReq = 0, txFilterLenReq = 0;
  sint15 nRet = DSS_SUCCESS;
  dss_iface_ioctl_qos_get_granted_filter_spec_type* getFilterSpec =
    (dss_iface_ioctl_qos_get_granted_filter_spec_type*)argval_ptr;
  QoSStateInfoType statusInfo;
  IIPFilterPriv *rxFilters[MAX_FLTR_PER_REQ] = {0, }, *txFilters[MAX_FLTR_PER_REQ] = {0, };

  memset(&statusInfo, 0, sizeof(QoSStateInfoType));

  // Get the NetworkFactory
  ds::Net::INetworkFactory *pNetworkFactory = NULL;
  IDS_ERR_RET_ERRNO(DSSGlobals::Instance()->GetNetworkFactory(&pNetworkFactory));
  DSSGenScope scopeNetworkFactory(pNetworkFactory,DSSGenScope::IDSIQI_TYPE);

  // Get the DSSNetQoSSecondary Structure according to the provided QoSHandle.
  IDS_ERR_RET_ERRNO(pNetActive->GetDSSQoSNetInitiated(getFilterSpec->handle, &pDSSQoSNetInitiated));

  // get the NetQoSSecondary object
  if (NULL != pDSSQoSNetInitiated){
    IDS_ERR_RET_ERRNO(pDSSQoSNetInitiated->GetQoSNetInitiated(&pIDSNetQoSNetInitiated));
  }

  DSSGenScope GenScopeObject(pIDSNetQoSNetInitiated,DSSGenScope::IDSIQI_TYPE);

  if (NULL == pIDSNetQoSNetInitiated){
    LOG_MSG_ERROR_0("dss_iface_ioctl_qos_get_granted_filter_spec(): "
                    "GetQoS failed.");
    return DSS_ERROR;
  }

//Query the amount of Granted Filter Spec
  BAIL_ERRNO(pIDSNetQoSNetInitiated->GetGrantedFilterSpec(
    NULL, 0, &rxFilterLenReq, NULL, 0, &txFilterLenReq));

  for(int i = 0; i < rxFilterLenReq; i++){
    BAIL_ERRNO(pNetworkFactory->CreateIPFilterSpec(&rxFilters[i]));
  }

  for(int i = 0; i < txFilterLenReq; i++){
    BAIL_ERRNO(pNetworkFactory->CreateIPFilterSpec(&txFilters[i]));
  }

  BAIL_ERRNO(pIDSNetQoSNetInitiated->GetGrantedFilterSpec(
                                                            rxFilters, 
                                                            rxFilterLenReq,
                                                            &rxFilterLenReq,
                                                            txFilters,
                                                            txFilterLenReq,
                                                            &txFilterLenReq
                                                          ));

  // Convert new QoS types to old QoS Types
  getFilterSpec->rx_filter_spec.num_filters = 
    min(getFilterSpec->rx_filter_spec.num_filters, rxFilterLenReq);
  getFilterSpec->tx_filter_spec.num_filters = 
    min(getFilterSpec->tx_filter_spec.num_filters, txFilterLenReq);

  for(int i = 0; i < getFilterSpec->rx_filter_spec.num_filters; i++){
    BAIL_ERRNO(DSSConversion::IDS2DSIPFilterSpec(rxFilters[i], &(getFilterSpec->rx_filter_spec.list_ptr[i])));
  }
  for(int i = 0; i < getFilterSpec->tx_filter_spec.num_filters; i++){
    BAIL_ERRNO(DSSConversion::IDS2DSIPFilterSpec(txFilters[i], &(getFilterSpec->tx_filter_spec.list_ptr[i])));
  }
  
  // Get the QoS Status
  BAIL_ERRNO(pIDSNetQoSNetInitiated->GetState(&statusInfo));
  
  // TODO: move to DSSConversion.cpp
  switch(statusInfo.state)
  {
  case QoSState::QDS_AVAILABLE_MODIFIED:
    getFilterSpec->qos_status = QOS_AVAILABLE;
    break;
  case QoSState::QDS_SUSPENDING:
    getFilterSpec->qos_status = QOS_SUSPENDING;
    break;
  case QoSState::QDS_UNAVAILABLE:
    getFilterSpec->qos_status = QOS_UNAVAILABLE;
    break;
  case QoSState::QDS_INVALID:
    getFilterSpec->qos_status = QOS_STATE_INVALID;
    break;
  case QoSState::QDS_ACTIVATING:
    getFilterSpec->qos_status = QOS_ACTIVATING;
    break;
  case QoSState::QDS_SUSPENDED:
    getFilterSpec->qos_status = QOS_DEACTIVATED;
    break;
  case QoSState::QDS_RELEASING:
    getFilterSpec->qos_status = QOS_RELEASING;
    break;
  case QoSState::QDS_CONFIGURING:
    getFilterSpec->qos_status = QOS_CONFIGURING;
    break;
  default:
    // TODO: update the errno
    nRet = DSS_ERROR;
    LOG_MSG_ERROR_1("dss_iface_ioctl_qos_get_granted_filter_spec(): "
                    "Unknown state, state %d", statusInfo.state);
    goto bail;
  }
  nRet = DSS_SUCCESS;

bail:

  for(int i = 0; i < MAX_FLTR_PER_REQ; i++){
    DSSCommon::ReleaseIf((IQI**)&rxFilters[i]);
    DSSCommon::ReleaseIf((IQI**)&txFilters[i]);
  }

  return nRet;
}/* dss_iface_ioctl_qos_get_granted_filter_spec() */

static sint15 dss_iface_ioctl_qos_resume
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_qos_resume(): argval_ptr: 0x%p",
                            argval_ptr);

   DSSNetQoSSecondary *pDSSNetQoSSecondary = NULL;
   dss_iface_ioctl_qos_resume_type* qos_resume =
      (dss_iface_ioctl_qos_resume_type*)argval_ptr;

   // Get the DSSNetQoSSecondary Structure according to the provided QoSHandle.
   IDS_ERR_RET_ERRNO(pNetActive->GetDSSNetQoSSecondary(qos_resume->handle, &pDSSNetQoSSecondary));

   IQoSSecondary* pIDSNetQoSSecondary = NULL;

   // get the NetQoSSecondary object
   if (NULL != pDSSNetQoSSecondary){
      IDS_ERR_RET_ERRNO(pDSSNetQoSSecondary->GetNetQoSSecondary(&pIDSNetQoSSecondary));
   }

   DSSGenScope GenScopeObject(pIDSNetQoSSecondary,DSSGenScope::IDSIQI_TYPE);

   // release the QoS Session
   if (NULL != pIDSNetQoSSecondary){
      IDS_ERR_RET_ERRNO(pIDSNetQoSSecondary->Resume());
   }
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_qos_modify
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   DSSNetQoSSecondary *pDSSNetQoSSecondary = NULL;
   dss_iface_ioctl_qos_modify_type* qos_modify =
      (dss_iface_ioctl_qos_modify_type*)argval_ptr;
   sint15 nRet = DSS_SUCCESS;
   ds::ErrorType dsErrno = 0;
   QoSModifyMaskType modifyMask = ds::Net::QoSModifyMask::QDS_NONE;
   DSSGenScope scopeRxFlows;
   DSSGenScope scopeTxFlows;
   DSSGenScope scopeRxFilter;
   DSSGenScope scopeTxFilter;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_qos_modify(): argval_ptr: 0x%p",
                            argval_ptr);
   memset(&modifyMask,0,sizeof(QoSModifyMaskType));

   // Get the DSSNetQoSSecondary Structure according to the provided QoSHandle.
   IDS_ERR_RET_ERRNO(pNetActive->GetDSSNetQoSSecondary(qos_modify->handle, &pDSSNetQoSSecondary));

   IQoSSecondary* pIDSNetQoSSecondary = NULL;
   IQoSSecondaryPriv* pIDSNetQoSSecondaryPriv = NULL;

   // get the NetQoSSecondary object
   if (NULL != pDSSNetQoSSecondary){
      IDS_ERR_RET_ERRNO(pDSSNetQoSSecondary->GetNetQoSSecondary(&pIDSNetQoSSecondary));
   }

   DSSGenScope GenScopeObject(pIDSNetQoSSecondary,
                              DSSGenScope::IDSIQI_TYPE);

   if (NULL != pIDSNetQoSSecondary){
      IDS_ERR_RET_ERRNO(pIDSNetQoSSecondary->QueryInterface(AEEIID_IQoSSecondaryPriv,
                                                           (void**)&pIDSNetQoSSecondaryPriv));
   }

   DSSGenScope GenScopeQoSSecondaryPriv(pIDSNetQoSSecondaryPriv,
                                        DSSGenScope::IDSIQI_TYPE);

   dss_iface_ioctl_qos_request_type qos_req;
   qos_req.qos = qos_modify->qos;

   QoSSpecType requestedQoSSpec;
   memset(&requestedQoSSpec, 0, sizeof(QoSSpecType));

   // convert the old qos_request to the new SpecType.
   BAIL_ERRNO(DSSConversion::DS2IDSQoSSpec(&qos_req, &requestedQoSSpec, TRUE, &modifyMask));

   scopeRxFlows.SetParams(requestedQoSSpec.rxFlows, DSSGenScope::IDSNetQoSFlow_ARRAY_PTR, requestedQoSSpec.rxFlowsLen);
   scopeTxFlows.SetParams(requestedQoSSpec.txFlows, DSSGenScope::IDSNetQoSFlow_ARRAY_PTR, requestedQoSSpec.txFlowsLen);
   scopeRxFilter.SetParams(requestedQoSSpec.rxFilter, DSSGenScope::IDSNetIPFilter_ARRAY_PTR, qos_req.qos.rx.fltr_template.num_filters);
   scopeTxFilter.SetParams(requestedQoSSpec.txFilter, DSSGenScope::IDSNetIPFilter_ARRAY_PTR, qos_req.qos.tx.fltr_template.num_filters);

   if (NULL != pIDSNetQoSSecondaryPriv){
      dsErrno = pIDSNetQoSSecondaryPriv->ModifySecondaryPriv(&requestedQoSSpec,
                                                             modifyMask);
      if (AEE_SUCCESS != dsErrno) {
         *dss_errno = DSSConversion::IDS2DSErrorCode(dsErrno);
         (void) DSSConversion::IDS2DSQoSSpecErrMask(&requestedQoSSpec,&qos_req);
         nRet = DSS_ERROR;
         LOG_MSG_ERROR_2("dss_iface_ioctl_qos_modify(): "
                         "ModifySecondaryPriv failed, error %d, dsErrno %d",
                         nRet, dsErrno);
         goto bail;
      }
   }

bail:
   memscpy(&(qos_modify->qos), sizeof(qos_req.qos),
     &(qos_req.qos), sizeof(qos_req.qos));
   return nRet;
}

static sint15 dss_iface_ioctl_qos_suspend
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_qos_suspend(): argval_ptr: 0x%p",
                            argval_ptr);
   DSSNetQoSSecondary *pDSSNetQoSSecondary = NULL;
   dss_iface_ioctl_qos_suspend_type* qos_suspend =
      (dss_iface_ioctl_qos_suspend_type*)argval_ptr;

   // Get the DSSNetQoSSecondary Structure according to the provided QoSHandle.
   IDS_ERR_RET_ERRNO(pNetActive->GetDSSNetQoSSecondary(qos_suspend->handle, &pDSSNetQoSSecondary));

   IQoSSecondary* pIDSNetQoSSecondary = NULL;

   // get the NetQoSSecondary object
   if (NULL != pDSSNetQoSSecondary){
      IDS_ERR_RET_ERRNO(pDSSNetQoSSecondary->GetNetQoSSecondary(&pIDSNetQoSSecondary));
   }

   DSSGenScope GenScopeObject(pIDSNetQoSSecondary, DSSGenScope::IDSIQI_TYPE);

   // release the QoS Session
   if (NULL != pIDSNetQoSSecondary){
      IDS_ERR_RET_ERRNO(pIDSNetQoSSecondary->Suspend());
   }

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_qos_get_status
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_qos_get_status(): argval_ptr: 0x%p",
                            argval_ptr);
   DSSNetQoSSecondary *pDSSNetQoSSecondary = NULL;
   DSSQoSNetInitiated* pDSSQoSNetInitiated = NULL;
   AEEResult res = AEE_SUCCESS;
   dss_iface_ioctl_qos_get_status_type* pqos_status_type = (dss_iface_ioctl_qos_get_status_type*)argval_ptr;

   // Get the DSSNetQoSSecondary or DSSQoSNetInitiated object according to the provided QoSHandle.   
   res = pNetActive->GetDSSNetQoSSecondary(pqos_status_type->handle, &pDSSNetQoSSecondary);
   if ( (AEE_SUCCESS != res) ||  (NULL == pDSSNetQoSSecondary) ) {                                  
      // No QoSSecondary object corresponding to provided qos_handle was found
      // Search if there is a corresponding QoSNetInitiated object
      res = pNetActive->GetDSSQoSNetInitiated(pqos_status_type->handle, &pDSSQoSNetInitiated);
      if ( (AEE_SUCCESS != res) ||  (NULL == pDSSQoSNetInitiated) ) {                                  
         // Could not find corresponding QoSSecondary nor QoSNetInitiated objects
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         LOG_MSG_ERROR_2("dss_iface_ioctl_qos_get_status(): "
                         "QoSSecondary or QoSNetInitiated not found, error %d, errno %d",
                         res, dss_errno);
         return DSS_ERROR;
      }
      IDS_ERR_RET_ERRNO(pDSSQoSNetInitiated->GetQoSStatus(pqos_status_type));
      return DSS_SUCCESS;
   }

   IQoSSecondary* pIDSNetQoSSecondary = NULL;

   // get the NetQoSSecondary object
   if (NULL != pDSSNetQoSSecondary){
      IDS_ERR_RET_ERRNO(pDSSNetQoSSecondary->GetNetQoSSecondary(&pIDSNetQoSSecondary));
   }

   DSSGenScope GenScopeObject(pIDSNetQoSSecondary, DSSGenScope::IDSIQI_TYPE);

   QoSStateInfoType statusInfo;
   memset(&statusInfo, 0, sizeof(QoSStateInfoType));

   // Get the QoS Status
   if (NULL != pIDSNetQoSSecondary){
      IDS_ERR_RET_ERRNO(pIDSNetQoSSecondary->GetState(&statusInfo));
   }

   // TODO: move to DSSConversion.cpp
   switch(statusInfo.state)
   {
   case QoSState::QDS_AVAILABLE_MODIFIED:
      pqos_status_type->qos_status = QOS_AVAILABLE;
      break;
   case QoSState::QDS_SUSPENDING:
      pqos_status_type->qos_status = QOS_SUSPENDING;
      break;
   case QoSState::QDS_UNAVAILABLE:
      pqos_status_type->qos_status = QOS_UNAVAILABLE;
      break;
   case QoSState::QDS_INVALID:
      pqos_status_type->qos_status = QOS_STATE_INVALID;
      break;
   case QoSState::QDS_ACTIVATING:
      pqos_status_type->qos_status = QOS_ACTIVATING;
      break;
   case QoSState::QDS_SUSPENDED:
      pqos_status_type->qos_status = QOS_DEACTIVATED;
      break;
   case QoSState::QDS_RELEASING:
      pqos_status_type->qos_status = QOS_RELEASING;
      break;
   case QoSState::QDS_CONFIGURING:
      pqos_status_type->qos_status = QOS_CONFIGURING;
      break;
   default:
      // TODO: Update the errno
      LOG_MSG_ERROR_1("dss_iface_ioctl_qos_get_status(): "
                      "unknown state, state %d", statusInfo.state);
      return DSS_ERROR;
   }

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_qos_request_ex
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno,
   dss_iface_id_type  iface_id
)
{
   ds::ErrorType dsErrno;
   uint8 flowID;
   // We can't be sure that iface id already contains the app id , let's build it again , and add a flow id .
   uint8 ifaceIndex = 0;
   sint15 app_id = 0;
   IQoSManager* pNetQoSManager = NULL;
   IQoSManagerPriv* pNetQoSManagerPriv = NULL;
   int qosIndex = 0, qosToReleaseIndex = 0;

   dss_iface_ioctl_qos_request_ex_type* qos_request_ex =
      (dss_iface_ioctl_qos_request_ex_type*)argval_ptr;

   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_qos_request_ex(): iface_id:%u, argval_ptr:0x%p ",
                            iface_id, argval_ptr);

   if(0 == qos_request_ex->handles_ptr){
     LOG_MSG_ERROR_0("dss_iface_ioctl_qos_request_ex : memory for output "
                     " argument handles_ptr is not allocated by application");
     *dss_errno = DS_EFAULT;
     return DSS_ERROR;
   }

   if ((0 == qos_request_ex->num_qos_specs) || 
       (DSS_IFACE_IOCTL_QOS_REQUEST_EX_MAX_QOS_SPECS < qos_request_ex->num_qos_specs)) {
         LOG_MSG_ERROR_1("dss_iface_ioctl_qos_request_ex(): "
                         "Invalid number of provided qos specs - %d",
                         qos_request_ex->num_qos_specs);
      *dss_errno = DS_EINVAL;
      return DSS_ERROR;
   }

   // prepare an array SpecType
   SeqQoSSpecType requestedQoSSpecs ;
   memset(&requestedQoSSpecs,0,sizeof(SeqQoSSpecType));

      PS_SYSTEM_HEAP_MEM_ALLOC(requestedQoSSpecs.data, 
                              (sizeof(QoSSpecType)*(qos_request_ex->num_qos_specs)),
                               QoSSpecType*);
      if (NULL == requestedQoSSpecs.data) {
         *dss_errno = DS_ENOMEM;
         return DSS_ERROR;
      }

   requestedQoSSpecs.dataLen = qos_request_ex->num_qos_specs;

   memset((void*)(requestedQoSSpecs.data), 0, sizeof(QoSSpecType)*(qos_request_ex->num_qos_specs));

   DSSGenScope ScopeQosSpecs(requestedQoSSpecs.data, DSSGenScope::GEN_SCRATCHPAD_ARRAY);

   // convert the old qos_request to the new SpecType.
   IDS_ERR_RET_ERRNO(DSSConversion::DS2IDSQoSSpecBundle(qos_request_ex, requestedQoSSpecs));

   dss_iface_ioctl_qos_request_type tmpReqType;
   DSSQoSRequestExScope QoSRequestExScope(qos_request_ex->num_qos_specs);

   for (int i=0;i<qos_request_ex->num_qos_specs;i++) {
      memscpy(&(tmpReqType.qos), sizeof(tmpReqType.qos),
        &(qos_request_ex->qos_specs_ptr[i]), sizeof(tmpReqType.qos));
      QoSRequestExScope.SetNthQoSSpec(i,
                                      requestedQoSSpecs.data[i].rxFilter,
                                      tmpReqType.qos.rx.fltr_template.num_filters,
                                      requestedQoSSpecs.data[i].txFilter,
                                      tmpReqType.qos.tx.fltr_template.num_filters,
                                      requestedQoSSpecs.data[i].rxFlows,
                                      requestedQoSSpecs.data[i].rxFlowsLen,
                                      requestedQoSSpecs.data[i].txFlows,
                                      requestedQoSSpecs.data[i].txFlowsLen);
   }

   IQoSSecondariesOutput* QoSSecondaryList = NULL;
   QoSRequestOpCodeType opCode = qos_request_ex->opcode == DSS_IFACE_IOCTL_QOS_REQUEST_OP ? QoSRequestOpCode::QDS_REQUEST : QoSRequestOpCode::QDS_CONFIGURE;

   IDS_ERR_RET(pNetActive->GetNetQoSManager(&pNetQoSManager));
   DSSGenScope scopeNetQosManager(pNetQoSManager,DSSGenScope::IDSIQI_TYPE);

   IDS_ERR_RET(pNetQoSManager->QueryInterface(AEEIID_IQoSManagerPriv, (void**)&pNetQoSManagerPriv));
   DSSGenScope scopeNetQosManagerPriv(pNetQoSManagerPriv,DSSGenScope::IDSIQI_TYPE);

   dsErrno = pNetQoSManagerPriv->RequestBundle(requestedQoSSpecs.data,
                                               requestedQoSSpecs.dataLen,
                                               opCode,
                                               &QoSSecondaryList);
   DSSGenScope scopeQoSSecondaryList(QoSSecondaryList,DSSGenScope::IDSIQI_TYPE);

   if (AEE_SUCCESS != dsErrno) {
      IDS_ERR_RET_ERRNO(DSSConversion::IDS2DSQoSSpecBundleErrMask(&requestedQoSSpecs,qos_request_ex));
      *dss_errno = DSSConversion::IDS2DSErrorCode(dsErrno);
      LOG_MSG_ERROR_1("dss_iface_ioctl_qos_request_ex(): "
                      "RequestBundle failed, errno %d", dss_errno);
      return DSS_ERROR;
   }

   // We can't be sure that iface id already contains the app id , let's build it again , and add a flow id .
   ifaceIndex = static_cast<uint8>(DSSIfaceId::GetIfaceIndex(iface_id));
   pNetActive->GetNetHandle(&app_id);
   dss_iface_id_type iface_id_temp = DSSIfaceId::BuildIfaceIdWithAppId(ifaceIndex,app_id);

   for (qosIndex = 0; qosIndex < qos_request_ex->num_qos_specs ; qosIndex++)
   {
      // find a free flow ID and Add it as the 8 LSB of iface_ID
      flowID = 0;
      
      dsErrno = DSSGlobals::Instance()->GetFreeQoSFlowID(&flowID);
      if (AEE_SUCCESS != dsErrno) {
         *dss_errno = DSSConversion::IDS2DSErrorCode(dsErrno);
         LOG_MSG_ERROR_1("dss_iface_ioctl_qos_request_ex(): "
                         "No available QoS flow IDs. Error is %d", *dss_errno);
         goto bail;
      }

      iface_id = iface_id_temp | flowID;

      // update the qos handle for the user
      qos_request_ex->handles_ptr[qosIndex] = iface_id;

      // Add the new created pNetQoSSecondary to pNetApp.
      // In case of failure QoSSecondaryList.data is released via ScopeSecondaryList object
      IQoSSecondary* qosSecondary = NULL;
      dsErrno = QoSSecondaryList->GetNth(qosIndex,
                                         &qosSecondary);
      DSSGenScope scopeQoSSecondary(qosSecondary,DSSGenScope::IDSIQI_TYPE);

      if (AEE_SUCCESS != dsErrno){
         DSSGlobals::Instance()->ReleaseQoSFlowID(flowID); // flowID must be released
         *dss_errno = DSSConversion::IDS2DSErrorCode(dsErrno);
         LOG_MSG_ERROR_1("dss_iface_ioctl_qos_request_ex(): "
                         "Unable to retrieve QoSSecondary object. Error is %d",
                         *dss_errno);
         goto bail;
      }

      dsErrno = pNetActive->AddNetQoSSecondary(qosSecondary,
                                               iface_id,
                                               qos_request_ex->cback_fn,
                                               qos_request_ex->user_data);
      if (AEE_SUCCESS != dsErrno){
         DSSGlobals::Instance()->ReleaseQoSFlowID(flowID); // flowID must be released
         *dss_errno = DSSConversion::IDS2DSErrorCode(dsErrno);
         LOG_MSG_ERROR_1("dss_iface_ioctl_qos_request_ex(): "
                         "Failed in adding NetQoSSecondary object. Error is %d",
                         *dss_errno);
         goto bail;
      }

   }
   

   LOG_MSG_FUNCTION_EXIT_0("dss_iface_ioctl_qos_request_ex(): Received QoS handles successfuly");
   return DSS_SUCCESS;

bail:
   // Releasing all the NetQoSSecondary objects that succeeded
   for (qosToReleaseIndex = 0 ; qosToReleaseIndex < qosIndex ; qosToReleaseIndex++)
   {
     (void)pNetActive->RemoveDSSNetQoSSecondary(qos_request_ex->handles_ptr[qosToReleaseIndex]);
     // Last 8 bits of the iface_id / qos_handle are the flow ID
     (void)DSSGlobals::Instance()->ReleaseQoSFlowID((uint8)(qos_request_ex->handles_ptr[qosToReleaseIndex] & 0xFF));
   }
   return DSS_ERROR;
}

static sint15 dss_iface_ioctl_qos_release_ex
(
      void *argval_ptr,
      DSSNetActive* pNetActive,
      sint15  *dss_errno,
      dss_iface_id_type  iface_id
)
{
   AEEResult res = DSS_SUCCESS;
   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_qos_release_ex(): iface_id:%u, argval_ptr:0x%p ",
                            iface_id, argval_ptr);
   dss_iface_ioctl_qos_release_ex_type* qos_release_ex =
      (dss_iface_ioctl_qos_release_ex_type*)argval_ptr;

   IQoSSecondary* pIDSNetQoSSecondary = NULL;

   // Get the IDSNetQoSManager
   IQoSManager* pNetQoSManager = NULL;
   DSSGenScope scopeNetQosManager;

   res = pNetActive->GetNetQoSManager(&pNetQoSManager);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("dss_iface_ioctl_qos_release_ex(): Failed in GetNetQoSManager, error %d",
                      res);
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      return res;
   }

   scopeNetQosManager.SetParams(pNetQoSManager,DSSGenScope::IDSIQI_TYPE);

   IQoSSecondariesInput* pQoSSecondariesInput = NULL;
   
   res = pNetQoSManager->CreateQoSSecondariesInput(&pQoSSecondariesInput);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("dss_iface_ioctl_qos_release_ex(): Failed in CreateQoSSecondariesInput, error %d",
                      res);
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      return res;
   }
   
   DSSGenScope scopeQosInput(pQoSSecondariesInput,DSSGenScope::IDSIQI_TYPE);

   DSSNetQoSSecondary* pDSSNetQoSSecondary = NULL;

   // prepare the IQoSSecondary sessions array
   for (int i=0 ; i<qos_release_ex->num_handles; i++)
   {
      LOG_MSG_INFO1_1("dss_iface_ioctl_qos_release_ex(): Processing handle 0x%x",
                      qos_release_ex->handles_ptr[i]);
      // Get the DSSNetQoSSecondary Structure according to the provided QoSHandle.
      res = pNetActive->GetDSSNetQoSSecondary(qos_release_ex->handles_ptr[i], &pDSSNetQoSSecondary);
      if (AEE_SUCCESS != res) {
        *dss_errno = DSSConversion::IDS2DSErrorCode(res); 
        LOG_MSG_ERROR_2("dss_iface_ioctl_qos_release_ex(): Failed in GetDSSNetQoSSecondary, "
                        "error %d, errno %d", res, dss_errno);
         return res;
      }

      // get the NetQoSSecondary object
      if (NULL != pDSSNetQoSSecondary){
         res = pDSSNetQoSSecondary->GetNetQoSSecondary(&pIDSNetQoSSecondary);
         if (AEE_SUCCESS != res) {
           *dss_errno = DSSConversion::IDS2DSErrorCode(res); 
           LOG_MSG_ERROR_2("dss_iface_ioctl_qos_release_ex(): Failed in GetNetQoSSecondary, "
                           "error %d, errno %d", res, dss_errno);
            return res;
      }

      pQoSSecondariesInput->Associate(pIDSNetQoSSecondary);
         
         // Releasing the strong reference taken by GetNetQoSSecondary
         DSSCommon::ReleaseIf((IQI**)&pIDSNetQoSSecondary);
      }
   }

   res = pNetQoSManager->Close(pQoSSecondariesInput);
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res); 
      LOG_MSG_ERROR_2("dss_iface_ioctl_qos_release_ex(): Failed in Close, "
                      "error %d, errno %d", res, dss_errno);
      
      return res;
   }

   LOG_MSG_FUNCTION_EXIT_0("dss_iface_ioctl_qos_release_ex(): QoS handles released successfuly");
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_qos_suspend_ex
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno,
   dss_iface_id_type  iface_id
 )
{
   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_qos_suspend_ex(): iface_id:%u, argval_ptr:0x%p ",
                            iface_id, argval_ptr);
   dss_iface_ioctl_qos_suspend_ex_type* qos_suspend_ex =
      (dss_iface_ioctl_qos_suspend_ex_type*)argval_ptr;

   IQoSSecondary* pIDSNetQoSSecondary = NULL;
   // Get the IDSNetQoSManager
   IQoSManager* pNetQoSManager = NULL;
   DSSGenScope scopeNetQosManager;
   IDS_ERR_RET_ERRNO(pNetActive->GetNetQoSManager(&pNetQoSManager));

   scopeNetQosManager.SetParams(pNetQoSManager,DSSGenScope::IDSIQI_TYPE);

   IQoSSecondariesInput* pQoSSecondariesInput = NULL;
   IDS_ERR_RET_ERRNO(pNetQoSManager->CreateQoSSecondariesInput(&pQoSSecondariesInput));
   DSSGenScope scopeQosInput(pQoSSecondariesInput,DSSGenScope::IDSIQI_TYPE);

   DSSNetQoSSecondary* pDSSNetQoSSecondary = NULL;

   // prepare the IQoSSecondary sessions array
   for (int i=0 ; i<qos_suspend_ex->num_handles; i++)
   {
      // Get the DSSNetQoSSecondary Structure according to the provided QoSHandle.
      IDS_ERR_RET_ERRNO(pNetActive->GetDSSNetQoSSecondary(qos_suspend_ex->handles_ptr[i], &pDSSNetQoSSecondary));

      // get the NetQoSSecondary object
      if (NULL != pDSSNetQoSSecondary){
         IDS_ERR_RET_ERRNO(pDSSNetQoSSecondary->GetNetQoSSecondary(&pIDSNetQoSSecondary));
         DSSGenScope scopeQoSSecondary(pIDSNetQoSSecondary, DSSGenScope::IDSIQI_TYPE);
      }

      pQoSSecondariesInput->Associate(pIDSNetQoSSecondary);
   }

   IDS_ERR_RET_ERRNO(pNetQoSManager->Suspend(pQoSSecondariesInput));

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_qos_resume_ex
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno,
   dss_iface_id_type  iface_id
)
{
   DSSIDSNetworkExtScope IDSNetworkExtScope;
   IQoSSecondary* pIDSNetQoSSecondary = NULL;
   AEEResult res = AEE_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_qos_resume_ex(): iface_id:%u, argval_ptr:0x%p ",
                            iface_id, argval_ptr);

   dss_iface_ioctl_qos_suspend_ex_type* qos_resume_ex =
      (dss_iface_ioctl_qos_suspend_ex_type*)argval_ptr;

   res = IDSNetworkExtScope.Init(pNetActive);
   if (AEE_SUCCESS != res) {
     *dss_errno = DSSConversion::IDS2DSErrorCode(res);
     LOG_MSG_ERROR_2("dss_iface_ioctl_qos_resume_ex(): "
                     "Scope init failed, error %d, errno %d", res, dss_errno);
     return DSS_ERROR;
   }

   // Get the IDSNetQoSManager
   IQoSManager* pNetQoSManager = NULL;
   DSSGenScope scopeNetQosManager;
   if(AEE_SUCCESS != pNetActive->GetNetQoSManager(&pNetQoSManager))
   {
     // Create a new NetQoSManager since pNetActive does not have
     // a NetQoSManager instance.
     IDS_ERR_RET_ERRNO(IDSNetworkExtScope.Fetch()->CreateQoSManager(&pNetQoSManager));

     // Add the new created pNetQoSManager to pNetActive.
     IDS_ERR_RET_ERRNO(pNetActive->SetNetQoSManager(pNetQoSManager));
   }

   scopeNetQosManager.SetParams(pNetQoSManager,DSSGenScope::IDSIQI_TYPE);

   IQoSSecondariesInput* pQoSSecondariesInput = NULL;
   IDS_ERR_RET_ERRNO(pNetQoSManager->CreateQoSSecondariesInput(&pQoSSecondariesInput));
   DSSGenScope scopeQosInput(pQoSSecondariesInput,DSSGenScope::IDSIQI_TYPE);

   DSSNetQoSSecondary* pDSSNetQoSSecondary = NULL;

   // prepare the IQoSSecondary sessions array
   for (int i=0 ; i<qos_resume_ex->num_handles; i++)
   {
      // Get the DSSNetQoSSecondary Structure according to the provided QoSHandle.
      IDS_ERR_RET_ERRNO(pNetActive->GetDSSNetQoSSecondary(qos_resume_ex->handles_ptr[i], &pDSSNetQoSSecondary));

      // get the NetQoSSecondary object
      if (NULL != pDSSNetQoSSecondary){
         IDS_ERR_RET_ERRNO(pDSSNetQoSSecondary->GetNetQoSSecondary(&pIDSNetQoSSecondary));
         DSSGenScope scopeQoSSecondary(pIDSNetQoSSecondary, DSSGenScope::IDSIQI_TYPE);
      }

      pQoSSecondariesInput->Associate(pIDSNetQoSSecondary);
   }

   IDS_ERR_RET_ERRNO(pNetQoSManager->Resume(pQoSSecondariesInput));

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_primary_qos_modify

(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno,
   dss_iface_id_type iface_id
)
{
   IQoS* pIDSNetQoSDefault;
   IQoSDefaultPriv* pIDSNetQoSDefaultPriv = NULL;
   sint15 nRet = DSS_SUCCESS;
   ds::ErrorType dsErrno;
   QoSModifyMaskType modifyMask;
   DSSNetApp* pNetApp = NULL;
   sint15 netHandle = DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID;
   AEEResult resInternal = AEE_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_primary_qos_modify(): iface_id:%u, argval_ptr:0x%p ",
                            iface_id, argval_ptr);
   memset(&modifyMask,0,sizeof(QoSModifyMaskType));

   // get the NetQoSSecondary object
   IDS_ERR_RET_ERRNO(pNetActive->GetQoSDefault(&pIDSNetQoSDefault));
   DSSGenScope scopeNetQosDefault(pIDSNetQoSDefault,DSSGenScope::IDSIQI_TYPE);

   IDS_ERR_RET_ERRNO(pIDSNetQoSDefault->QueryInterface(AEEIID_IQoSDefaultPriv, (void**)&pIDSNetQoSDefaultPriv));
   DSSGenScope scopeNetQosDefaultPriv(pIDSNetQoSDefaultPriv,DSSGenScope::IDSIQI_TYPE);

   dss_iface_ioctl_primary_qos_modify_type* qos_modify =
      (dss_iface_ioctl_primary_qos_modify_type*)argval_ptr;

   if (NULL == qos_modify->cback_fn) {
      LOG_MSG_ERROR_0 ("dss_iface_ioctl_primary_qos_modify(): "
                       "NULL cback for qos modify");
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   QoSSpecPrimaryType requestedQoSSpec;
   memset(&requestedQoSSpec, 0, sizeof(QoSSpecPrimaryType));

   // convert the old qos_request to the new SpecType.
   IDS_ERR_RET_ERRNO(DSSConversion::DS2IDSQoSSpecPrimary(qos_modify, &requestedQoSSpec, TRUE, &modifyMask));

   DSSGenScope ScopeRxFlows(requestedQoSSpec.rxFlows, DSSGenScope::IDSNetQoSFlow_ARRAY_PTR, requestedQoSSpec.rxFlowsLen);
   DSSGenScope ScopeTxFlows(requestedQoSSpec.txFlows, DSSGenScope::IDSNetQoSFlow_ARRAY_PTR, requestedQoSSpec.txFlowsLen);

   QoSModifyMaskType mask;
   // convert the old qos mask to the new mask.
   IDS_ERR_RET_ERRNO(DSSConversion::DS2IDSQoSMask(qos_modify->primary_qos_spec.field_mask, &mask));
   
   // get the corresponding NetApp obj
   pNetActive->GetNetHandle(&netHandle);
   resInternal = DSSGlobals::Instance()->GetNetApp(netHandle, &pNetApp);
   if (AEE_SUCCESS != resInternal) {
      LOG_MSG_ERROR_1("dss_iface_ioctl_primary_qos_modify(): "
                      "Failed getting NetApp, netHandle: %d", netHandle);
      ASSERT(0);
   }
   
   // release the QoS Session
   
   // Register to primary QoS status events. (Accepted / rejected). the following command
   // causes the DSS_ to register to ds::NetQoS::Event::MODIFY_RESULT event.
   BAIL_ERRNO(pNetActive->RegEventCBPrimary(DSS_IFACE_IOCTL_QOS_MODIFY_ACCEPTED_EV,
                                            qos_modify->cback_fn,
                                            qos_modify->user_data_ptr,
                                            iface_id));

   dsErrno = pIDSNetQoSDefaultPriv->ModifyDefaultPriv(&requestedQoSSpec, mask);
   if (AEE_SUCCESS != dsErrno) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(dsErrno);
      (void) DSSConversion::IDS2DSQoSSpecPrimaryErrMask(&requestedQoSSpec,qos_modify);
      nRet = DSS_ERROR;
      goto bail;
   }

bail:
   return nRet;
}
static sint15 dss_iface_ioctl_primary_qos_get_granted_flow_spec
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno,
   dss_iface_id_type  iface_id
)
{
   IQoS* pIDSNetQoSDefault;
   IQoSSecondaryPriv* pIDSNetQoSDefaultPriv = NULL;

   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_primary_qos_get_granted_flow_spec(): iface_id:%u, argval_ptr:0x%p ",
                            iface_id, argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   // get the NetQoSDefault object
   IDS_ERR_RET_ERRNO(pNetActive->GetQoSDefault(&pIDSNetQoSDefault));
   DSSGenScope scopeNetQoSDefault(pIDSNetQoSDefault,DSSGenScope::IDSIQI_TYPE);


   IDS_ERR_RET_ERRNO(pIDSNetQoSDefault->QueryInterface(AEEIID_IQoSDefaultPriv, (void**)&pIDSNetQoSDefaultPriv));
   DSSGenScope scopeNetQoSDefaultPriv(pIDSNetQoSDefaultPriv, DSSGenScope::IDSIQI_TYPE);

   dss_iface_ioctl_primary_qos_get_granted_flow_spec_type* pPrimaryQoSGetFlowType =
      (dss_iface_ioctl_primary_qos_get_granted_flow_spec_type*)argval_ptr;

   IQoSFlowPriv *txQoSFlow = NULL, *rxQoSFlow = NULL;

   IDS_ERR_RET_ERRNO(pIDSNetQoSDefaultPriv->GetGrantedFlowSpecPriv(&rxQoSFlow, &txQoSFlow));

   // Convert new QoS types to old QoS Types
   IDS_ERR_RET_ERRNO(DSSConversion::IDS2DSQoSSpecFlow(rxQoSFlow, &(pPrimaryQoSGetFlowType->rx_ip_flow)));
   IDS_ERR_RET_ERRNO(DSSConversion::IDS2DSQoSSpecFlow(txQoSFlow, &(pPrimaryQoSGetFlowType->tx_ip_flow)));

   DSSCommon::ReleaseIf((IQI**)&(txQoSFlow));
   DSSCommon::ReleaseIf((IQI**)&(rxQoSFlow));

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_on_qos_aware_system
(
   void *argval_ptr,
   DSSIDSNetworkExtScope* IDSNetworkExtScope,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_on_qos_aware_system(): argval_ptr:0x%p ",
                            argval_ptr);
      
   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_on_qos_aware_system_type* bIsAwareSystem =
      (dss_iface_ioctl_on_qos_aware_system_type*)argval_ptr;

   boolean qosAwareUnaware;
   // Ask the network if it's QoS aware or unaware
   IDS_ERR_RET_ERRNO(IDSNetworkExtScope->Fetch()->GetQosAware(&qosAwareUnaware));

   *bIsAwareSystem = qosAwareUnaware;

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_network_supported_qos_profiles
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{
   IQoSManager *pNetQoSManager = NULL;
   QoSProfileIdType* profileValues = NULL;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_network_supported_qos_profiles(): argval_ptr:0x%p ",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_get_network_supported_qos_profiles_type* supp_qos_profiles =
      (dss_iface_ioctl_get_network_supported_qos_profiles_type*)argval_ptr;

   // allocate the profileValues array
    PS_SYSTEM_HEAP_MEM_ALLOC(profileValues, 
                             sizeof(QoSProfileIdType)*(QoSProfileIds::MAX_SUPPORTED_PROFILES),
                             QoSProfileIdType*);

   if (NULL == profileValues) {
      *dss_errno = DS_ENOMEM;
      return DSS_ERROR;
   }

   DSSGenScope GenScopeObject(profileValues, DSSGenScope::GEN_SCRATCHPAD_ARRAY);

   int profileIdsLenReq = 0;

   // Get the QoS Profile values
   IDS_ERR_RET_ERRNO(pNetMonitored->GetNetQoSManager(&pNetQoSManager));
   DSSGenScope scopeNetQosManager(pNetQoSManager,DSSGenScope::IDSIQI_TYPE);

   IDS_ERR_RET_ERRNO(pNetQoSManager->GetSupportedProfiles(profileValues,
      QoSProfileIds::MAX_SUPPORTED_PROFILES,
      &profileIdsLenReq));

   // Set the user struct to the values we got from the GetSupportedProfiles call
   supp_qos_profiles->profile_count = (uint8)min(profileIdsLenReq, QoSProfileIds::MAX_SUPPORTED_PROFILES);

   for (int i = 0; i < supp_qos_profiles->profile_count; i++){
      supp_qos_profiles->profile_value[i] = (uint16)profileValues[i];
   }

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_707_get_hdr_1x_handdown_option
(
   void *argval_ptr,
   DSSIDSNetworkScope* IDSNetworkScope,
   sint15  *dss_errno
)
{
   DSSIDSNetwork1xPrivScope IDSNetwork1xPrivScope;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_707_get_hdr_1x_handdown_option(): argval_ptr:0x%p ",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_707_hdr_1x_handdown_option_type* handdownOption =
      (dss_iface_ioctl_707_hdr_1x_handdown_option_type*)argval_ptr;

   IDS_ERR_RET_ERRNO(IDSNetwork1xPrivScope.Init(IDSNetworkScope->Fetch()));

   boolean bHandDownOption;
   IDS_ERR_RET_ERRNO(IDSNetwork1xPrivScope.Fetch()->GetHDR1xHandDownOption(&bHandDownOption));

   *handdownOption = bHandDownOption;

   return DSS_SUCCESS;
}
static sint15 dss_iface_ioctl_707_set_hdr_1x_handdown_option
(
   void *argval_ptr,
   DSSIDSNetworkScope* IDSNetworkScope,
   sint15  *dss_errno
)
{
   DSSIDSNetwork1xPrivScope IDSNetwork1xPrivScope;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_707_set_hdr_1x_handdown_option(): argval_ptr:0x%p ",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_707_hdr_1x_handdown_option_type* handdownOption =
      (dss_iface_ioctl_707_hdr_1x_handdown_option_type*)argval_ptr;

   IDS_ERR_RET_ERRNO(IDSNetwork1xPrivScope.Init(IDSNetworkScope->Fetch()));

   boolean bHandDownOption = *handdownOption;
   IDS_ERR_RET_ERRNO(IDSNetwork1xPrivScope.Fetch()->SetHDR1xHandDownOption(bHandDownOption));

   return DSS_SUCCESS;
}
#ifdef FEATUTE_DATA_PS_MCAST
static sint15 dss_iface_ioctl_mcast_join
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno,
   dss_iface_id_type  iface_id
)
{
   IMCastManager *pNetMCastManager = NULL;
   IMCastManagerPriv* pNetMCastManagerPriv = NULL;
   IMCastSessionPriv* session = NULL;
   ds::AddrFamilyType family =ds::AddrFamily::QDS_AF_UNSPEC;

   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_mcast_join(): iface_id:%u, argval_ptr:0x%p ",
                            iface_id, argval_ptr);

   IDS_ERR_RET_ERRNO(pNetActive->GetNetMCastManager(&pNetMCastManager));
   DSSGenScope scopeNetMCastManager(pNetMCastManager,DSSGenScope::IDSIQI_TYPE);

   IDS_ERR_RET_ERRNO(pNetMCastManager->GetTechObject(AEEIID_IMCastManagerPriv,
                                                     (void**)&pNetMCastManagerPriv));
   DSSGenScope scopeNetMCastManagerPriv(pNetMCastManagerPriv,DSSGenScope::IDSIQI_TYPE);


   dss_iface_ioctl_mcast_join_type* mcast_join = (dss_iface_ioctl_mcast_join_type*)argval_ptr;

   ds::SockAddrStorageType addr;

   IQI* pInfo = NULL;

   IDS_ERR_RET_ERRNO(DSSConversion::DS2IDSMCast(mcast_join, addr, pNetMCastManagerPriv, &pInfo));

   DSSGenScope GenScopeObject(pInfo, DSSGenScope::IDSIQI_TYPE);

   (void)ds::Sock::AddrUtils::GetFamily(addr, &family);

   IDS_ERR_RET_ERRNO(pNetMCastManagerPriv->Join(addr, pInfo, &session));
   DSSGenScope scopeSession(session,DSSGenScope::IDSIQI_TYPE);

   // find a free flow ID and Add it as the 8 LSB of iface_ID
   uint8 flowID = 0;
   IDS_ERR_RET_ERRNO(DSSGlobals::Instance()->GetFreeMCastFlowID(&flowID));
   iface_id |= flowID;

   IDS_ERR_RET_ERRNO(pNetActive->AddDSSMCast(session, iface_id, mcast_join->event_cb, mcast_join->user_data_ptr));

   mcast_join->handle = iface_id;

   return DSS_SUCCESS;
}
static sint15 dss_iface_ioctl_mcast_join_ex
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno,
   dss_iface_id_type  iface_id
)
{
   sint15 nRet = DSS_SUCCESS;
   AEEResult result = AEE_SUCCESS;
   IMCastManager* pNetMCastManager = NULL;
   IMCastManagerPriv* pNetMCastManagerPriv = NULL;

   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_mcast_join_ex(): iface_id:%u, argval_ptr:0x%p ",
                            iface_id, argval_ptr);

   IDS_ERR_RET_ERRNO(pNetActive->GetNetMCastManager(&pNetMCastManager));
   DSSGenScope scopeNetMCastManager(pNetMCastManager,DSSGenScope::IDSIQI_TYPE);

   IDS_ERR_RET_ERRNO(pNetMCastManager->GetTechObject(AEEIID_IMCastManagerPriv,
                                                     (void**)&pNetMCastManagerPriv));
   DSSGenScope scopeNetMCastManagerPriv(pNetMCastManagerPriv,DSSGenScope::IDSIQI_TYPE);

   dss_iface_ioctl_mcast_join_ex_type* mcast_Control_Bundle = (dss_iface_ioctl_mcast_join_ex_type*)argval_ptr;

   ds::SockAddrStorageType addrSeq[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL] = {{0}};
   int addrSeqLen = mcast_Control_Bundle->num_flows;
   IQI* infoSeq[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
   int infoSeqLen = mcast_Control_Bundle->num_flows;
   MCastJoinFlagsType mcastFlags[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL] = {0};
   int mcastFlagsLen = mcast_Control_Bundle->num_flows;

   result = DSSConversion::DS2IDSMCastBundle(mcast_Control_Bundle,
                                             addrSeq,
                                             mcastFlags,
                                             pNetMCastManagerPriv,
                                             infoSeq);
   if (AEE_SUCCESS != result) {
      LOG_MSG_ERROR_0("dss_iface_ioctl_mcast_join_ex(): "
                      "Can't convert Mcast Bundle");
      *dss_errno = DSSConversion::IDS2DSErrorCode(result);
      nRet = DSS_ERROR;
      return nRet;
   }

   infoSeqLen = addrSeqLen;

   IMCastSessionPriv* sessions[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
   int sessionsLen = mcast_Control_Bundle->num_flows;
   int sessionsLenReq;

   for( uint32 i = 0; mcast_Control_Bundle->num_flows > i; i++ ) {
      sessions[i] = NULL ;
   }

   IDS_ERR_RET_ERRNO( pNetMCastManagerPriv->JoinBundle (addrSeq,
                                                        addrSeqLen,
                                                        infoSeq,
                                                        infoSeqLen,
                                                        mcastFlags,
                                                        mcastFlagsLen,
                                                        sessions,   // or &sessions[0]
                                                        sessionsLen,
                                                        &sessionsLenReq));

   uint8 flowID = 0;
   dss_iface_id_type  tmp_iface_id;


   for( uint32 i = 0; mcast_Control_Bundle->num_flows > i; i++ )
   {
      // find a free flow ID and Add it as the 8 LSB of iface_ID
      BAIL_ERRNO(DSSGlobals::Instance()->GetFreeMCastFlowID(&flowID));
      tmp_iface_id = iface_id;
      tmp_iface_id |= flowID;

      //AddDSSMCast adds refCnt on each session, so session shall be released
      BAIL_ERRNO(pNetActive->AddDSSMCast(sessions[i],
                                         tmp_iface_id,
                                         mcast_Control_Bundle->event_cb,
                                         mcast_Control_Bundle->user_data_ptr));

      // update the mcast handle for the user. handle field in mcast_Control_Bundle is an output parameter.
      mcast_Control_Bundle->handle[i] = tmp_iface_id;
   }

/* fall through */

bail :

   for( uint32 i = 0; mcast_Control_Bundle->num_flows > i; i++ ) {
      DSSCommon::ReleaseIf((IQI**)&(sessions[i]));
   }

   for( uint32 i = 0; mcast_Control_Bundle->num_flows > i; i++ ) {
     DSSCommon::ReleaseIf((IQI**)&(infoSeq[i]));
   }

   return nRet;

}
static sint15 dss_iface_ioctl_mcast_leave
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   DSSMCast* pDSSMCast = NULL;
   dss_iface_ioctl_mcast_leave_type* mcast_leave =
      (dss_iface_ioctl_mcast_leave_type*)argval_ptr;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_mcast_leave(): argval_ptr:0x%p ",
                            argval_ptr);

   // Get the DSSMCast class according to the provided MCastHandle.
   IDS_ERR_RET_ERRNO(pNetActive->GetDSSMCast(mcast_leave->handle, &pDSSMCast));

   IMCastSessionPriv* pIDSNetMCastSession = NULL;

   // get the NetQoSSecondary object
   if (NULL != pDSSMCast){
      IDS_ERR_RET_ERRNO(pDSSMCast->GetMCastSession(&pIDSNetMCastSession));
   }

   // Leave the MCast Session.
   if (NULL != pIDSNetMCastSession){
      IDS_ERR_RET_ERRNO(pIDSNetMCastSession->Leave());
   }

   DSSCommon::ReleaseIf((IQI**)&pIDSNetMCastSession);

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_mcast_leave_ex
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   dss_iface_ioctl_mcast_leave_ex_type* mcast_Control_Bundle = (dss_iface_ioctl_mcast_leave_ex_type*)argval_ptr;

   IMCastManager* pIDSNetMCastManager = NULL;
   IMCastManagerPriv * pIDSNetMcastManagerPriv = NULL;

   DSSMCast* pDSSMCast = NULL;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_mcast_leave_ex(): argval_ptr:0x%p ",
                            argval_ptr);

   // Get the DSSMCast class according to the provided MCastHandle.
   IDS_ERR_RET_ERRNO(pNetActive->GetNetMCastManager(&pIDSNetMCastManager));
   DSSGenScope scopeNetMCastManager(pIDSNetMCastManager, DSSGenScope::IDSIQI_TYPE);

   IDS_ERR_RET_ERRNO(pIDSNetMCastManager->GetTechObject(AEEIID_IMCastManagerPriv,
                                                        (void**)&pIDSNetMcastManagerPriv));
   DSSGenScope scopeNetMCastManagerPriv(pIDSNetMcastManagerPriv, DSSGenScope::IDSIQI_TYPE);

   IMCastSessionPriv** pIDSNetMCastSession = NULL;
   if (mcast_Control_Bundle->num_flows > 0) {
      PS_SYSTEM_HEAP_MEM_ALLOC(pIDSNetMCastSession, 
                               sizeof(IMCastSessionPriv*)*(mcast_Control_Bundle->num_flows),
                               IMCastSessionPriv**);
      if (NULL == pIDSNetMCastSession) {
         *dss_errno = DS_ENOMEM;
         return DSS_ERROR;
      }
   }

   int mcastIndex;

   for(mcastIndex = 0; mcast_Control_Bundle->num_flows > mcastIndex; mcastIndex++ ) {
      pIDSNetMCastSession[mcastIndex] = NULL;
   }

   DSSGenScope GenScopeObject(pIDSNetMCastSession, DSSGenScope::IDSNetMCastSession_ARRAY_PTR, mcast_Control_Bundle->num_flows);

   for(mcastIndex = 0; mcast_Control_Bundle->num_flows > mcastIndex; mcastIndex++ ) {
      IDS_ERR_RET_ERRNO(pNetActive->GetDSSMCast(mcast_Control_Bundle->handle[mcastIndex], &pDSSMCast));
      if (NULL != pDSSMCast){
         IDS_ERR_RET_ERRNO(pDSSMCast->GetMCastSession(&pIDSNetMCastSession[mcastIndex]));
      }
   }

   // Leave the MCast Session.
   IDS_ERR_RET_ERRNO(pIDSNetMcastManagerPriv->LeaveBundle (pIDSNetMCastSession,
                                                           mcast_Control_Bundle->num_flows));

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_mcast_register_ex
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_mcast_register_ex(): argval_ptr:0x%p ",
                            argval_ptr);

   dss_iface_ioctl_mcast_register_ex_type* mcast_Control_Bundle =
      (dss_iface_ioctl_mcast_register_ex_type*)argval_ptr;

   DSSMCast* pDSSMCast = NULL;

   IMCastManager* pIDSNetMCastManager = NULL;
   IMCastManagerPriv * pIDSNetMcastManagerPriv = NULL;

   IDS_ERR_RET_ERRNO(pNetActive->GetNetMCastManager(&pIDSNetMCastManager));
   DSSGenScope scopeNetMCastManager(pIDSNetMCastManager,DSSGenScope::IDSIQI_TYPE);

   IDS_ERR_RET_ERRNO(pIDSNetMCastManager->GetTechObject(AEEIID_IMCastManagerPriv,
     (void**)&pIDSNetMcastManagerPriv));
   DSSGenScope scopeNetMCastManagerPriv(pIDSNetMcastManagerPriv, DSSGenScope::IDSIQI_TYPE);

   IMCastSessionPriv** ppIDSNetMCastSession = NULL;
   if (mcast_Control_Bundle->num_flows > 0) {
      PS_SYSTEM_HEAP_MEM_ALLOC(ppIDSNetMCastSession, 
                               sizeof(IMCastSessionPriv*)*(mcast_Control_Bundle->num_flows),
                               IMCastSessionPriv**);
      if (NULL == ppIDSNetMCastSession) {
         *dss_errno = DS_ENOMEM;
         return DSS_ERROR;
      }
   }
   memset(ppIDSNetMCastSession, 0, sizeof(IMCastSessionPriv*)*(mcast_Control_Bundle->num_flows));

   DSSGenScope GenScopeObject(ppIDSNetMCastSession, DSSGenScope::IDSNetMCastSession_ARRAY_PTR, mcast_Control_Bundle->num_flows);

   for( int i = 0; mcast_Control_Bundle->num_flows > i; i++ ) {
      IDS_ERR_RET_ERRNO(pNetActive->GetDSSMCast(mcast_Control_Bundle->handle[i], &pDSSMCast));
      if (NULL != pDSSMCast){
         IDS_ERR_RET_ERRNO(pDSSMCast->GetMCastSession(&(ppIDSNetMCastSession[i])));
      }
   }

   IDS_ERR_RET_ERRNO(pIDSNetMcastManagerPriv->RegisterBundle(ppIDSNetMCastSession, (mcast_Control_Bundle->num_flows)));

   return DSS_SUCCESS;
}
static sint15 dss_iface_ioctl_mbms_mcast_context_activate
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno,
   dss_iface_id_type ifaceId
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_mbms_mcast_context_activate(): argval_ptr:0x%p ",
                            argval_ptr);

   dss_iface_ioctl_mbms_mcast_context_act_type* mbms_act =
      (dss_iface_ioctl_mbms_mcast_context_act_type*)argval_ptr;

   IMCastManager* pNetMCastManager = 0;
   IDS_ERR_RET_ERRNO(pNetActive->GetNetMCastManager(&pNetMCastManager));
   DSSGenScope scopeNetMCastManager(pNetMCastManager,DSSGenScope::IDSIQI_TYPE);


   IMCastManagerMBMSPriv* pIDSMBMSManager = 0;
   IDS_ERR_RET_ERRNO(pNetMCastManager->QueryInterface(AEEIID_IMCastManagerMBMSPriv, (void **)(&pIDSMBMSManager)));

   DSSGenScope GenScopeObject(pIDSMBMSManager, DSSGenScope::IDSIQI_TYPE);

   IMCastMBMSCtrlPriv* pNetMCastMBMSCtrl = 0;
   ds::IPAddrType addr;

   IDS_ERR_RET_ERRNO(DSSConversion::DS2IDSIpAddr(&(mbms_act->ip_addr), &addr));

   IDS_ERR_RET_ERRNO(pIDSMBMSManager->Activate(&addr, mbms_act->profile_id, &pNetMCastMBMSCtrl));
   DSSGenScope scopeNetMCastMBMSCtrl(pNetMCastMBMSCtrl,DSSGenScope::IDSIQI_TYPE);

   mbms_act->handle = (uint32)pNetMCastMBMSCtrl;

   IDS_ERR_RET_ERRNO(pNetActive->AddDSSMCastMBMSCtrl(pNetMCastMBMSCtrl, mbms_act->handle, mbms_act->event_cb, mbms_act->user_data_ptr, ifaceId));

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_mbms_mcast_context_deactivate
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_mbms_mcast_context_deactivate(): argval_ptr:0x%p ",
                            argval_ptr);

   dss_iface_ioctl_mbms_mcast_context_deact_type* mbms_act =
      (dss_iface_ioctl_mbms_mcast_context_deact_type*)argval_ptr;

   IMCastMBMSCtrlPriv* pNetMCastMBMSCtrl = NULL;

   IDS_ERR_RET_ERRNO(pNetActive->GetMCastMBMSCtrl( mbms_act->nethandle, &pNetMCastMBMSCtrl ));
   DSSGenScope scopeNetMCastMBMSCtrl(pNetMCastMBMSCtrl,DSSGenScope::IDSIQI_TYPE);

   if (NULL != pNetMCastMBMSCtrl){
      IDS_ERR_RET_ERRNO(pNetMCastMBMSCtrl->DeActivate());
   }

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_bcmcs_enable_handoff_reg
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   sint15 nRet = DSS_SUCCESS;
   AEEResult res = AEE_SUCCESS;
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_bcmcs_enable_handoff_reg(): argval_ptr:0x%p ",
                            argval_ptr);

   // Get the MCAST Manager
   IMCastManager* pNetMCastManager = NULL;
   IDS_ERR_RET_ERRNO(pNetActive->GetNetMCastManager(&pNetMCastManager));
   DSSGenScope scopeNetMCastManager(pNetMCastManager,DSSGenScope::IDSIQI_TYPE);

   // Use the MCast manager to get the BCMCS Manager
   IMCastManagerBCMCS* pIDSBCMCSManager;
   IDS_ERR_RET_ERRNO(pNetMCastManager->QueryInterface(AEEIID_IMCastManagerBCMCS, (void **)(&pIDSBCMCSManager)));

   dss_iface_ioctl_bcmcs_enable_handoff_reg_type* handoff_reg =
      (dss_iface_ioctl_bcmcs_enable_handoff_reg_type*)argval_ptr;

   ds::SockAddrStorageType addrSeq[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL] = {{0}};

   // Create a sequence of addresses.
   for (uint32 i = 0; i < handoff_reg->num_mcast_addr; i++) {
      res = DSSConversion::DS2IDSSockIpAddr(&(handoff_reg->mcast_addr_info[i].ip_addr), (addrSeq[i]));
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_0("dss_iface_ioctl_bcmcs_enable_handoff_reg(): "
                         "Invalid Address Sequence");
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         nRet = DSS_ERROR;
         return nRet;
      }
   }

   // Register with handoff optimization.
   IDS_ERR_RET_ERRNO(pIDSBCMCSManager->RegisterUsingHandoffOpt(addrSeq, handoff_reg->num_mcast_addr));

   return nRet;
}
#endif // FEATUTE_DATA_PS_MCAST
static sint15 dss_iface_ioctl_mt_reg_cb
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno,
   dss_iface_id_type ifaceId
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_mt_reg_cb(): argval_ptr:0x%p ",
                            argval_ptr);
   dss_iface_ioctl_mt_reg_cb_type* mt_reg_ptr = reinterpret_cast<dss_iface_ioctl_mt_reg_cb_type*>(argval_ptr);

   if (NULL == mt_reg_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }
   // This call causes to register to MTPD event and update the MTPD handle.
   IDS_ERR_RET_ERRNO(pNetMonitored->RegMTPDEventCB(mt_reg_ptr, ifaceId));

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_mt_dereg_cb
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_mt_dereg_cb(): argval_ptr:0x%p ",
                            argval_ptr);

   dss_iface_ioctl_mt_dereg_cb_type* mt_reg_ptr = reinterpret_cast<dss_iface_ioctl_mt_dereg_cb_type*>(argval_ptr);

   if (NULL == mt_reg_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }
   // This call causes to de-register to MTPD event
   IDS_ERR_RET_ERRNO(pNetMonitored->DeRegMTPDEventCB(mt_reg_ptr));

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_dormancy_info_code
(
   void *argval_ptr,
   DSSIDSNetworkScope* IDSNetworkScope,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_dormancy_info_code(): argval_ptr:0x%p ",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   DormancyInfoCodeType IDSDormancyInfoCode;
   dss_iface_ioctl_dormancy_info_code_enum_type dormancyInfoCodeToReturn;
   IDS_ERR_RET_ERRNO(IDSNetworkScope->Fetch()->GetDormancyInfoCode(&IDSDormancyInfoCode));
   AEEResult res = DSSConversion::IDS2DSDormancyInfoCode(IDSDormancyInfoCode, &dormancyInfoCodeToReturn);
   if(AEE_SUCCESS != res){
      return DSSConversion::IDS2DSErrorCode(res);
   }
   *reinterpret_cast<dss_iface_ioctl_dormancy_info_code_enum_type*>(argval_ptr) = dormancyInfoCodeToReturn;
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_qos_get_mode
(
 void *argval_ptr,
 DSSNetMonitored* pDSSNetMonitored,
 sint15  *dss_errno
 )
{
   AEEResult res = AEE_SUCCESS;
   QoSModeType IDSQoSMode;
   DSSIDSNetworkExtScope IDSNetworkExtScope;
   dss_iface_ioctl_qos_mode_type qosModeToReturn;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_qos_get_mode(): argval_ptr:0x%p ",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   IDS_ERR_RET_ERRNO(IDSNetworkExtScope.Init(pDSSNetMonitored));
   IDS_ERR_RET_ERRNO(IDSNetworkExtScope.Fetch()->GetQoSMode(&IDSQoSMode));

   res = DSSConversion::IDS2DSQoSMode(IDSQoSMode, &qosModeToReturn);
   if(AEE_SUCCESS != res){
      return DSSConversion::IDS2DSErrorCode(res);
   }
   *reinterpret_cast<dss_iface_ioctl_qos_mode_type*>(argval_ptr) = qosModeToReturn;
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_umts_get_im_cn_flag
(
   void *argval_ptr,
   DSSIDSNetworkScope* IDSNetworkScope,
   sint15  *dss_errno
)
{
   DSSIDSNetworkUMTSScope IDSNetworkUMTSScope;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_umts_get_im_cn_flag(): argval_ptr:0x%p ",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   IDS_ERR_RET_ERRNO(IDSNetworkUMTSScope.Init(IDSNetworkScope->Fetch()));

   // Get the IM_CN flag
   boolean IM_CNFlag;
   IDS_ERR_RET_ERRNO(IDSNetworkUMTSScope.Fetch()->GetIMCNFlag((UMTSIMCNFlagType*)&IM_CNFlag));

   // Update argval with the received IM CN flag
   *(uint32*)(argval_ptr) = IM_CNFlag;

   return DSS_SUCCESS;
}


static sint15 dss_iface_ioctl_generate_priv_ipv6_addr
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   DSSIDSNetworkScope* IDSNetworkScope,
   sint15  *dss_errno,
   dss_iface_id_type  iface_id
)
{
   INetworkIPv6* pNetworkIpv6 = NULL;
   IIPv6Address* pNetIpv6Address = NULL;
   ::ds::INAddr6Type ip6Addr ;
   sint15 nRet = DSS_SUCCESS;
   AEEResult res;

   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_generate_priv_ipv6_addr(): iface_id:%u, argval_ptr:0x%p ",
                            iface_id, argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   IDS_ERR_RET(IDSNetworkScope->Fetch()->GetTechObject(AEEIID_INetworkIPv6, (void**)&pNetworkIpv6));
   if (NULL == pNetworkIpv6) {
      *dss_errno = DS_EINVAL;
      return DSS_ERROR;
   }

   DSSGenScope scopeIPv6 (pNetworkIpv6,DSSGenScope::IDSIQI_TYPE);

   dss_iface_ioctl_priv_ipv6_addr_type* priv_addr = reinterpret_cast<dss_iface_ioctl_priv_ipv6_addr_type*>(argval_ptr);

   res = pNetworkIpv6->GeneratePrivAddr(priv_addr->iid_params.is_unique,&pNetIpv6Address);
   if (AEE_SUCCESS != res && AEE_EWOULDBLOCK != res) {
      LOG_MSG_ERROR_1("dss_iface_ioctl_generate_priv_ipv6_addr(): "
                      "IPv6 address generation failed: %d", res);
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      return DSS_ERROR;
   }

   // assuming pNetIpv6Address gets some value even in case of WOULDBLOCK
   // TODO verify
   if (pNetIpv6Address == NULL) {
      LOG_MSG_ERROR_0("dss_iface_ioctl_generate_priv_ipv6_addr(): "
                      "IPv6 address generation failed. Expecting valid object despite WOULDBLOCK: %d");
      *dss_errno = DS_EINVAL;
      return DSS_ERROR;
   }

   DSSGenScope scopeIPv6address (pNetIpv6Address,DSSGenScope::IDSIQI_TYPE);

   if (AEE_EWOULDBLOCK != res) {
      IDS_ERR_RET_ERRNO(pNetIpv6Address->GetAddress(ip6Addr));
      IDS_ERR_RET_ERRNO(DSSConversion::IDS2DSIp6Addr(ip6Addr,priv_addr->ip_addr));
   }

   // ipv6address may not exist yet at that point in case of WOULDBLOCK
   IDS_ERR_RET_ERRNO(pNetActive->AddIpv6PrivAddr(pNetIpv6Address,
                                          iface_id,
                                          priv_addr->iid_params.is_unique,
                                          priv_addr->event_cb,
                                          priv_addr->user_data_ptr));

   *dss_errno = DSSConversion::IDS2DSErrorCode(res);
   if (*dss_errno != DSS_SUCCESS) {
      nRet = DSS_ERROR;
   }

   return nRet;
}

static sint15 dss_iface_ioctl_get_all_v6_prefixes
(
 void *argval_ptr,
 DSSIDSNetworkScope* IDSNetworkScope,
 sint15  *dss_errno
)
{
   INetworkIPv6Priv* pNetworkIPv6Priv = NULL;
   IPv6PrivPrefixInfoType* prefixes = NULL;
   sint15 nRet = DSS_SUCCESS;
   int nReq = 0;
   int nMin = 0;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_all_v6_prefixes(): argval_ptr:0x%p ",
                            argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   IDS_ERR_RET_ERRNO(IDSNetworkScope->Fetch()->GetTechObject(AEEIID_INetworkIPv6Priv, (void**)&pNetworkIPv6Priv));
   if (NULL == pNetworkIPv6Priv) {
      *dss_errno = DS_EINVAL;
      return DSS_ERROR;
   }

   DSSGenScope scopeIPV6prefixes(pNetworkIPv6Priv,DSSGenScope::IDSIQI_TYPE);

   dss_iface_ioctl_get_all_v6_prefixes_type* all_v6_prefixes = reinterpret_cast<dss_iface_ioctl_get_all_v6_prefixes_type*>(argval_ptr);

   prefixes = NULL;

   if(all_v6_prefixes->num_prefixes == 0)
   {
      LOG_MSG_ERROR_0("dss_iface_ioctl_get_all_v6_prefixes(): "
                      "No memory alloc'd for prefixes!");
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   PS_SYSTEM_HEAP_MEM_ALLOC(prefixes, sizeof(IPv6PrivPrefixInfoType)*(all_v6_prefixes->num_prefixes),
                            IPv6PrivPrefixInfoType*);
   if (NULL == prefixes) {
      *dss_errno = DS_ENOMEM;
      goto bail;
   }

   BAIL_ERRNO(pNetworkIPv6Priv->GetAllIPv6Prefixes(prefixes,all_v6_prefixes->num_prefixes,&nReq));
   // at this point nReq can be < all_v6_prefixes->num_prefixes
   nMin = min(all_v6_prefixes->num_prefixes, nReq);
   for(int i = 0; i < nMin; i++)
   {
      memscpy(all_v6_prefixes->prefix_info_ptr[i].prefix.ps_s6_addr,
        sizeof(struct ps_in6_addr),
        prefixes[i].prefix,
        sizeof(struct ps_in6_addr));

      BAIL_ERRNO(DSSConversion::IDS2DSIPv6PrefixState(prefixes[i].prefixType,&(all_v6_prefixes->prefix_info_ptr[i].prefix_state)));

      all_v6_prefixes->prefix_info_ptr[i].prefix_len =
        (uint8) prefixes[i].prefixLen;
   }

   // num_prefixes returned to app should be at most the number app required , even though NetworkIPv6Priv may actually return more
   // backward compatibility issues
   all_v6_prefixes->num_prefixes = nMin;

bail:

   PS_SYSTEM_HEAP_MEM_FREE (prefixes);

   return nRet;
}
static sint15 dss_iface_ioctl_enable_firewall
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   IFirewallManager* pNetFirewallManager = NULL;

   //DM when do we need reinterpret cast? Is it required here
   dss_iface_ioctl_enable_firewall_type* pEnableFirewall = (dss_iface_ioctl_enable_firewall_type*)argval_ptr;;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_enable_firewall(): "
                            "Ioctl: Enable Firewall, Allow pkts: %d",
                            pEnableFirewall->is_pkts_allowed);

   IDS_ERR_RET_ERRNO(pNetActive->GetNetFirewallManager(&pNetFirewallManager));
   DSSGenScope scopeNetFirewallManager(pNetFirewallManager,DSSGenScope::IDSIQI_TYPE);

   IDS_ERR_RET_ERRNO(pNetFirewallManager->EnableFirewall(pEnableFirewall->is_pkts_allowed));

   LOG_MSG_FUNCTION_EXIT_0("dss_iface_ioctl_enable_firewall(): "
                           "Success");
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_disable_firewall
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   IFirewallManager* pNetFirewallManager = NULL;
   LOG_MSG_FUNCTION_ENTRY_0("dss_iface_ioctl_disable_firewall(): "
                            "Ioctl: Disable Firewall");

   (void) argval_ptr;

   IDS_ERR_RET_ERRNO(pNetActive->GetNetFirewallManager(&pNetFirewallManager));
   DSSGenScope scopeNetFirewallManager(pNetFirewallManager,DSSGenScope::IDSIQI_TYPE);

   IDS_ERR_RET_ERRNO(pNetFirewallManager->DisableFirewall());

   LOG_MSG_FUNCTION_EXIT_0("dss_iface_ioctl_disable_firewall(): "
                           "Success");
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_add_firewall_rule
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   IFirewallManager*          pIFirewallManager = NULL;
   IFirewallRule*             pIFirewallRule = NULL;
   IIPFilterPriv*             pIIPFilterPriv = NULL;
   INetworkFactory*           pINetworkFactory = NULL;

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_add_firewall_rule(): "
                            "IOCTL: Add firewall rule, argval %p", argval_ptr);

   dss_iface_ioctl_add_firewall_rule_type* add_firewall_rule = (dss_iface_ioctl_add_firewall_rule_type*)argval_ptr;
   if (NULL == add_firewall_rule) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   IDS_ERR_RET_ERRNO(pNetActive->GetNetFirewallManager(&pIFirewallManager));
   DSSGenScope scopeNetFirewallManager(pIFirewallManager,DSSGenScope::IDSIQI_TYPE);

   // Get the NetworkFactory from DSSGlobals
   DSSGlobals::Instance()->GetNetworkFactory(&pINetworkFactory);
   DSSGenScope scopeNetworkFactory(pINetworkFactory,DSSGenScope::IDSIQI_TYPE);

   // Create a new ds::Net::IIPFilter interface
   IDS_ERR_RET_ERRNO (pINetworkFactory->CreateIPFilterSpec(reinterpret_cast<IIPFilterPriv**>(&pIIPFilterPriv)));
   DSSGenScope GenScopeObject(pIIPFilterPriv, DSSGenScope::IDSIQI_TYPE);

   // Pass over the filters and convert them to the new API filters in order
   // to set the filters on the corresponding IDSNetwork object.
   IDS_ERR_RET_ERRNO(DSSConversion::DS2IDSIPFilter(&(add_firewall_rule->fltr_spec), pIIPFilterPriv));

   //Add the firewall rule on the Firewall Manager object
   IDS_ERR_RET_ERRNO(pIFirewallManager->AddFirewallRule(pIIPFilterPriv, &pIFirewallRule));
   DSSGenScope scopeFirewallRule(pIFirewallRule,DSSGenScope::IDSIQI_TYPE);

   //Add the FirewallRule object to the DSS layer along with handle
   IDS_ERR_RET_ERRNO(pNetActive->AddDSSFirewallRule(pIFirewallRule, &add_firewall_rule->handle));

   LOG_MSG_FUNCTION_EXIT_1("dss_iface_ioctl_add_firewall_rule(): "
                           "Success, handle %d", add_firewall_rule->handle);
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_delete_firewall_rule
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   dss_iface_ioctl_delete_firewall_rule_type* delete_firewall_rule =
      (dss_iface_ioctl_delete_firewall_rule_type*)argval_ptr;
   IFirewallRule*         pIFirewallRule = NULL;

   if (NULL == delete_firewall_rule) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_delete_firewall_rule(): "
                            "Ioctl: Delete firewall rule, handle %d",
                            delete_firewall_rule->handle);

   // Get the FirewallRule object from handle passed in.
   IDS_ERR_RET_ERRNO(pNetActive->GetDSSFirewallRule(delete_firewall_rule->handle, &pIFirewallRule));
   DSSGenScope scopeFirewallRule(pIFirewallRule,DSSGenScope::IDSIQI_TYPE);

   // delete firewall rule
   IDS_ERR_RET_ERRNO(pNetActive->DeleteDSSFirewallRule(delete_firewall_rule->handle));

   LOG_MSG_FUNCTION_EXIT_0("dss_iface_ioctl_delete_firewall_rule(): "
                           "Success");
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_firewall_rule
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   IFirewallRule* pIFirewallRule = NULL;
   IIPFilterPriv*     pIIPFilterSpec = NULL;
   dss_iface_ioctl_get_firewall_rule_type* get_firewall_rule =
      (dss_iface_ioctl_get_firewall_rule_type*)argval_ptr;

   if (NULL == get_firewall_rule) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   LOG_MSG_FUNCTION_ENTRY_1("dss_iface_ioctl_get_firewall_rule(): "
                            "Ioctl: Get firewall rule, handle %d",
                            get_firewall_rule->handle);

   // Get the IFirewall rule object corresponding to this handle
   IDS_ERR_RET_ERRNO(pNetActive->GetDSSFirewallRule(get_firewall_rule->handle, &pIFirewallRule));
   DSSGenScope scopeFirewallRule(pIFirewallRule,DSSGenScope::IDSIQI_TYPE);

   // Get the IIPFilterSpec associated with this firewall rule.
   if (NULL == pIFirewallRule) 
   {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   IDS_ERR_RET_ERRNO(pIFirewallRule->GetFirewallRule(&pIIPFilterSpec));
   DSSGenScope scopeFilterSpec(pIIPFilterSpec,DSSGenScope::IDSIQI_TYPE);

   // Convert IIPPFilterSepc interface into DSS format.
   memset (&get_firewall_rule->fltr_spec, 0, sizeof(ip_filter_type));
   IDS_ERR_RET_ERRNO(DSSConversion::IDS2DSIPFilterSpec(pIIPFilterSpec, &get_firewall_rule->fltr_spec));

   LOG_MSG_FUNCTION_EXIT_0("dss_iface_ioctl_get_firewall_rule(): "
                           "Success");
   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_firewall_table
(
   void *argval_ptr,
   DSSNetActive* pNetActive,
   sint15  *dss_errno
)
{
   int                index;
   IFirewallManager*  pIFirewallManager = NULL;
   IIPFilterPriv*     pIIPFilterSpec = NULL;
   AEEResult          res            = AEE_SUCCESS;
   sint15             ret;

   LOG_MSG_FUNCTION_ENTRY_0("dss_iface_ioctl_get_firewall_table(): "
                            "Ioctl: Get firewall table");

   dss_iface_ioctl_get_firewall_table_type* get_firewall_table =
      (dss_iface_ioctl_get_firewall_table_type*)argval_ptr;

   // Create the IFirewallRule objects.
   IFirewallManager::SeqFirewallRulesType FirewallRulesList;
   memset(&FirewallRulesList, 0, sizeof(IFirewallManager::SeqFirewallRulesType));
   
   if (get_firewall_table->num_fltrs > 0) {
      PS_SYSTEM_HEAP_MEM_ALLOC(FirewallRulesList.data,
                               sizeof(IFirewallRule*)*(get_firewall_table->num_fltrs),
                               IFirewallRule**);
      if (NULL ==  FirewallRulesList.data) {
         *dss_errno = DS_ENOMEM;
         return DSS_ERROR;
      }
   }

   FirewallRulesList.dataLen = get_firewall_table->num_fltrs;

   DSSGenScope GenScopeObject(FirewallRulesList.data, DSSGenScope::IDSNetFirewallRule_ARRAY_PTR, get_firewall_table->num_fltrs);

   res = pNetActive->GetNetFirewallManager(&pIFirewallManager);

   DSSGenScope scopeNetFirewallManager(pIFirewallManager,DSSGenScope::IDSIQI_TYPE);

   if(AEE_SUCCESS != res){
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      ret = DSS_ERROR;
      goto bail;
   }

   res = pIFirewallManager->GetFirewallTable(FirewallRulesList.data,
                                             FirewallRulesList.dataLen,
                                             &FirewallRulesList.dataLenReq);

   if(AEE_SUCCESS != res){
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      ret = DSS_ERROR;
      goto bail;
   }

   for (index = 0; index < MIN (FirewallRulesList.dataLen, FirewallRulesList.dataLenReq); index++)
   {
      res = (FirewallRulesList.data[index])->GetFirewallRule(&pIIPFilterSpec);
      if(AEE_SUCCESS != res){
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         ret = DSS_ERROR;
         goto bail;
      }

      res = DSSConversion::IDS2DSIPFilterSpec(pIIPFilterSpec, &(get_firewall_table->fltr_spec_arr[index]));
      if(AEE_SUCCESS != res){
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         ret = DSS_ERROR;
         goto bail;
      }
   }

   get_firewall_table->avail_num_fltrs = FirewallRulesList.dataLenReq;

   LOG_MSG_FUNCTION_EXIT_0("dss_iface_ioctl_get_firewall_table(): "
                           "Success");

   return DSS_SUCCESS;

bail:
   if (get_firewall_table->num_fltrs > 0)
   {
     PS_SYSTEM_HEAP_MEM_FREE (FirewallRulesList.data);
   }

   return ret;
}

static sint15 dss_iface_ioctl_get_iface_stats
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{      
   LOG_MSG_FUNCTION_ENTRY_0("dss_iface_ioctl_get_iface_stats(): "
                            "Ioctl: Get Iface stats");

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_get_iface_stats_type* pStats = reinterpret_cast<dss_iface_ioctl_get_iface_stats_type*>(argval_ptr);
   IDS_ERR_RET_ERRNO(pNetMonitored->GetNetworkStatistics(pStats));
   LOG_MSG_FUNCTION_EXIT_0("dss_iface_ioctl_get_iface_stats(): "
                           "Success");
   return DSS_SUCCESS;
} 

static sint15 dss_iface_ioctl_reset_iface_stats
(
   void *argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15  *dss_errno
)
{      
   LOG_MSG_FUNCTION_ENTRY_0("dss_iface_ioctl_reset_iface_stats(): "
                            "Ioctl: Reset Iface stats");
   IDS_ERR_RET_ERRNO(pNetMonitored->ResetNetworkStatistics());
   LOG_MSG_FUNCTION_EXIT_0("dss_iface_ioctl_reset_iface_stats(): "
                           "Success");
   return DSS_SUCCESS;
} 

static sint15 dss_iface_ioctl_set_fmc_tunnel_params
(
   void       * argval_ptr,
   DSSNetMonitored* pNetMonitored,
   sint15     * dss_errno
)
{
   INetworkExt2 *pNetworkExt2 = 0;
   FMCTunnelParamsType pIDSFMCTunnelParams;
   AEEResult result = AEE_SUCCESS;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   sint15             sRet = DSS_SUCCESS;

   
   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_set_fmc_tunnel_params(): "
                            "NetApp obj : 0x%x argval_ptr : 0x%x",
                            pNetMonitored, argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_uw_fmc_tunnel_params_type * pDSFMCTunnelParams = 
      (dss_iface_ioctl_uw_fmc_tunnel_params_type *)argval_ptr;

   memset(&pIDSFMCTunnelParams, 0, sizeof(FMCTunnelParamsType));

   IDS_ERR_RET_ERRNO(DSSConversion::DS2IDSFMCTunnelParams(pDSFMCTunnelParams, &pIDSFMCTunnelParams));

   IDS_ERR_RET(pNetMonitored->GetIDSNetworkExt2Object(&pNetworkExt2));

   result = pNetworkExt2->SetFMCTunnelParams(&pIDSFMCTunnelParams);
   if(AEE_SUCCESS != result){
     dss_error_code = DSS_ERROR_SET_FMCTUNNEL_PARAMS_FAILED;
     sRet = DSS_ERROR;
   }

   DSSCommon::ReleaseIf((IQI**)&pNetworkExt2);

   LOG_MSG_FUNCTION_EXIT_2("dss_iface_ioctl_set_fmc_tunnel_params(): "
                           "error %d, result %d", dss_error_code, result);

   *dss_errno = DSSConversion::IDS2DSErrorCode(result); 

   return sRet;
}

static sint15 dss_iface_ioctl_reset_fmc_tunnel_params
(
  void       * argval_ptr,
  DSSNetMonitored* pNetMonitored,
  sint15     * dss_errno
)
{
   INetworkExt2 *pNetworkExt2 = 0;
   AEEResult result = AEE_SUCCESS;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   sint15             sRet = DSS_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_reset_fmc_tunnel_params(): NetApp obj : 0x%x argval_ptr : 0x%x",
                            pNetMonitored, argval_ptr);

   // this IOCTL does not use IOCTL argument
   (void) argval_ptr;

   IDS_ERR_RET(pNetMonitored->GetIDSNetworkExt2Object(&pNetworkExt2));

   result = pNetworkExt2->ResetFMCTunnelParams();
   if(AEE_SUCCESS != result){
     dss_error_code = DSS_ERROR_RESET_FMCTUNNEL_PARAMS_FAILED;
     sRet = DSS_ERROR;
   }

   DSSCommon::ReleaseIf((IQI**)&pNetworkExt2);

   LOG_MSG_FUNCTION_EXIT_2("dss_iface_ioctl_reset_fmc_tunnel_params(): "
                           "error %d, result %d", dss_error_code, result);

   *dss_errno = DSSConversion::IDS2DSErrorCode(result); 

   return sRet;
}

static sint15 dss_iface_ioctl_get_device_name
(
   void *argval_ptr,
   DSSNetMonitored*  pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_get_device_name(): NetApp obj : 0x%x argval_ptr : 0x%x",
                            pNetMonitored, argval_ptr);

   if (NULL == argval_ptr) {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   dss_iface_ioctl_device_name_type* pDeviceName = reinterpret_cast<dss_iface_ioctl_device_name_type*>(argval_ptr);
   IDS_ERR_RET_ERRNO(pNetMonitored->GetDeviceName(pDeviceName));

   LOG_MSG_FUNCTION_EXIT_1("dss_iface_ioctl_get_device_name(): SUCCESS, device name = %s",
                           pDeviceName);

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_gateway_addr
(
   void *argval_ptr,
   DSSNetMonitored*  pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_get_gateway_addr(): NetApp obj : 0x%x argval_ptr : 0x%x",
                            pNetMonitored, argval_ptr);
   dss_iface_ioctl_gateway_addr_type* pGatewayAddr = reinterpret_cast<dss_iface_ioctl_gateway_addr_type*>(argval_ptr);
   IDS_ERR_RET_ERRNO(pNetMonitored->GetGatewayAddress(pGatewayAddr));

   LOG_MSG_FUNCTION_EXIT_1("dss_iface_ioctl_get_gateway_addr(): SUCCESS, gateway's addr = 0x%x",
                           pGatewayAddr);

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_get_subnet_mask
(
   void *argval_ptr,
   DSSNetMonitored*  pNetMonitored,
   sint15  *dss_errno
)
{
   LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_get_subnet_mask(): NetApp obj : 0x%x argval_ptr : 0x%x",
                            pNetMonitored, argval_ptr);
   dss_iface_ioctl_subnet_mask_type* pSubnetMask = reinterpret_cast<dss_iface_ioctl_subnet_mask_type*>(argval_ptr);
   IDS_ERR_RET_ERRNO(pNetMonitored->GetSubnetMask(pSubnetMask));

   LOG_MSG_FUNCTION_EXIT_1("dss_iface_ioctl_get_subnet_mask(): SUCCESS, subnet mask = 0x%x",
                           pSubnetMask);

   return DSS_SUCCESS;
}

static sint15 dss_iface_ioctl_filter_match_request
(
	void              *argval_ptr,
	DSSNetActive*      pNetActive,
	sint15	          *dss_errno,
	dss_iface_id_type  iface_id
)
{
  IQoSManager* pNetQoSManager = NULL;
  IQoSManagerPriv* pNetQoSManagerPriv = NULL;
  DSSQoSNetInitiated* pDSSQoSNetInitiated = NULL;
  ds::ErrorType nRes2 = -1;

  LOG_MSG_FUNCTION_ENTRY_2("dss_iface_ioctl_filter_match_request(): iface_id:%u, argval_ptr:0x%p",
                            iface_id, argval_ptr);

   dss_iface_ioctl_filter_match_request_type* fltr_info_ptr =
           (dss_iface_ioctl_filter_match_request_type*)argval_ptr;
   dss_iface_filter_match_info fltr_info;
   memset(&fltr_info, 0, sizeof(dss_iface_filter_match_info));

   if (NULL == dss_errno || NULL == pNetActive || NULL== fltr_info_ptr || 0 == fltr_info_ptr->qos_handle) {
      LOG_MSG_ERROR_0 ("dss_iface_ioctl_filter_match_request(): "
                       "dss_errno or pNetActive or qos_handle is NULL");
      return DSS_ERROR;
   }
   
   ds::ErrorType res = pNetActive->GetDSSQoSNetInitiated(fltr_info_ptr->qos_handle, &pDSSQoSNetInitiated);
   if ( AEE_SUCCESS != res ) {                                  
      // Could not find corresponding QoSSecondary nor QoSNetInitiated objects
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      LOG_MSG_ERROR_2("dss_iface_ioctl_filter_match_request(): "
                      "QoSSecondary or QoSNetInitiated not found, error %d, errno %d",
                      res, dss_errno);
      return DSS_ERROR;
   }

   
   IDS_ERR_RET(pNetActive->GetNetQoSManager(&pNetQoSManager));
   DSSGenScope scopeNetQosManager(pNetQoSManager,DSSGenScope::IDSIQI_TYPE);

   IDS_ERR_RET(pNetQoSManager->QueryInterface(AEEIID_IQoSManagerPriv, (void**)&pNetQoSManagerPriv));
   DSSGenScope scopeNetQosManagerPriv(pNetQoSManagerPriv,DSSGenScope::IDSIQI_TYPE);

   fltr_info.fltr_info_ptr = fltr_info_ptr;
   fltr_info.fltr_info_ptr->list_fitr_ptr->is_match = FALSE;
   if((NULL != pDSSQoSNetInitiated) && (NULL != pNetQoSManagerPriv))
   {
     fltr_info.flow_handle = pDSSQoSNetInitiated->GetFlow();
     nRes2 = pNetQoSManagerPriv->RequestFiltrMatch(&fltr_info);
   }
   return nRes2;
}

//===================================================================
//  FUNCTION:   IfaceIoctl707
//
//  DESCRIPTION:
//
//===================================================================
static int IfaceIoctl707(DSSNetActive* pNetActive,
                         DSSNetMonitored* pNetMonitored,
                         dss_iface_ioctl_type ioctlName,
                         void* pArgVal,
                         sint15* sErrno,
                         dss_iface_id_type iface_id)
{
   DSSIDSNetwork1xScope IDSNetwork1xScope;
   DSSIDSNetwork1xPrivScope IDSNetwork1xPrivScope;
   DSSIDSNetworkScope IDSActiveNetworkScope;
   DSSIDSNetworkScope IDSMonitoredNetworkScope;
   IQoS1x             * piQoS1x = NULL;
   DSSNetQoSSecondary * pDSSNetQoSSecondary = NULL;
   IQoSSecondary      * pIDSNetQoSSecondary = NULL;
   IQoS               * pIDSNetQoSDefault = NULL;
   uint32             flowID = 0;
   AEEResult          res = AEE_SUCCESS;
   sint15             result = DSS_SUCCESS;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   sint15 err_arg = DSS_SUCCESS;

   
   LOG_MSG_FUNCTION_ENTRY_4("IfaceIoctl707(): DSSNetActive: 0x%p, "
                            "DSSNetMonitored: 0x%p, ioctlName: 0x%x, argval_ptr:0x%p",
                            pNetActive, pNetMonitored, ioctlName, pArgVal);

   res = IDSMonitoredNetworkScope.Init(pNetMonitored);
   if (AEE_SUCCESS != res) {
     goto bail;
   }
   
   // Fetch the right interface for the ioctl.
   switch (ioctlName) {
      // These IOCTLs use IDSNetwork1x.
      case DSS_IFACE_IOCTL_707_GET_MDR:
      case DSS_IFACE_IOCTL_707_SET_MDR:
      case DSS_IFACE_IOCTL_707_GET_RLP_ALL_CURR_NAK:
      case DSS_IFACE_IOCTL_707_SET_RLP_ALL_CURR_NAK:
      case DSS_IFACE_IOCTL_707_GET_RLP_ALL_DEF_NAK:
      case DSS_IFACE_IOCTL_707_SET_RLP_ALL_DEF_NAK:
      case DSS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK:
      case DSS_IFACE_IOCTL_707_GET_RLP_QOS_NA_PRI:
      case DSS_IFACE_IOCTL_707_SET_RLP_QOS_NA_PRI:
      case DSS_IFACE_IOCTL_707_SDB_SUPPORT_QUERY:
      case DSS_IFACE_IOCTL_707_GET_SIP_MIP_DATA_CALL_TYPE:
      case DSS_IFACE_IOCTL_707_SET_HYSTERESIS_ACT_TIMER:
      case DSS_IFACE_IOCTL_707_GET_HYSTERESIS_ACT_TIMER:
         res = IDSNetwork1xScope.Init(IDSMonitoredNetworkScope.Fetch());
         if (AEE_SUCCESS != res) {
            dss_error_code = DSS_ERROR_IDSNETWORKSCOPE_INIT_FAILED;
            goto bail;
         }
         break;

      // These IOCTLs use IDSNetwork1xPriv.
      case DSS_IFACE_IOCTL_GET_SESSION_TIMER:
      case DSS_IFACE_IOCTL_SET_SESSION_TIMER:
      case DSS_IFACE_IOCTL_707_GET_DORM_TIMER:
      case DSS_IFACE_IOCTL_707_SET_DORM_TIMER:
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_HPT_MODE:
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INTERIA:
      case DSS_IFACE_IOCTL_707_ENABLE_HOLDDOWN:
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_SLOTTED_MODE:
         res = IDSNetwork1xPrivScope.Init(IDSMonitoredNetworkScope.Fetch());
         if (AEE_SUCCESS != res) {
            dss_error_code = DSS_ERROR_IDSNETWORKPRIVSCOPE_INIT_FAILED;
            goto bail;
         }
         break;

      // These IOCTLs use IQoS1x.
      case DSS_IFACE_IOCTL_707_HDR_GET_RMAC3_INFO:
      case DSS_IFACE_IOCTL_707_GET_TX_STATUS:
      case DSS_IFACE_IOCTL_707_GET_INACTIVITY_TIMER:
      case DSS_IFACE_IOCTL_707_SET_INACTIVITY_TIMER:
         if (NULL == pNetActive) {
            res = QDS_ENETNONET;
            dss_error_code = DSS_ERROR_NULL_NETACTIVE;
            goto bail;
         }

         // handle default flow case
         flowID = PS_IFACE_GET_FLOW_INST_FROM_ID(iface_id);
         if (PS_IFACE_DEFAULT_FLOW_INST == flowID){
            // Get the QoSDefault Structure from NetMonitored.
            res = pNetActive->GetQoSDefault(&pIDSNetQoSDefault);
            if(AEE_SUCCESS != res){
               dss_error_code = DSS_ERROR_GET_QOS_DEFAULT_FAILED;
               goto bail;
            }
            res = pIDSNetQoSDefault->GetTechObject(AEEIID_IQoS1x, (void**)&piQoS1x);
            if (AEE_SUCCESS != res) {
               dss_error_code = DSS_ERROR_GET_TECH_OBJECT_FAILED;
               goto bail;
            }
         }
         // handle secondary flow case
         else{
            // Get the DSSNetQoSSecondary Structure according to the provided QoSHandle.
            sint15 netHandle = DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID;
            DSSNetApp* pNetApp = NULL;
            dss_iface_id_type ifaceId = INVALID_IFACE_ID;
            
            pNetActive->GetNetHandle(&netHandle);
            res = DSSGlobals::Instance()->GetNetApp(netHandle, &pNetApp);
            if(AEE_SUCCESS != res){
               dss_error_code = DSS_ERROR_GET_NETAPP_FAILED;
               err_arg        = netHandle;
               goto bail;
            }

            pNetApp->GetIfaceId(&ifaceId);
            if (ifaceId != iface_id) {
               LOG_MSG_ERROR_2("Cannot GetDSSNetQoSSecondary, NetActive iface_id: %d , NetApp iface_id: %d",
                               iface_id, ifaceId);
            }

            res = pNetActive->GetDSSNetQoSSecondary(flowID, &pDSSNetQoSSecondary);
            if(AEE_SUCCESS != res){
               dss_error_code = DSS_ERROR_GET_DSSNETQOS_SECONDARY_FAILED;
               goto bail;
            }
            // get the NetQoSSecondary object
            if (NULL != pDSSNetQoSSecondary){
               res = pDSSNetQoSSecondary->GetNetQoSSecondary(&pIDSNetQoSSecondary);
               if(AEE_SUCCESS != res){
                  dss_error_code = DSS_ERROR_GET_NETQOS_SECONDARY_FAILED;
                  goto bail;
               }
               res = pIDSNetQoSSecondary->GetTechObject(AEEIID_IQoS1x, (void**)&piQoS1x);
               if (AEE_SUCCESS != res) {
                  dss_error_code = DSS_ERROR_GET_TECH_OBJECT_FAILED;
                  goto bail;
               }
            }
         }
         break;

      default:
         break;// of IQoS1x IOCTLs case
   }


   // Call the right function for the ioctl.
   switch (ioctlName) {
      // TODO: for MDR & DORM_TIMER - the original ioctl expects uint* (not int*)
      case DSS_IFACE_IOCTL_707_GET_MDR:
         res = IDSNetwork1xScope.Fetch()->GetMDR(reinterpret_cast<int*>(pArgVal));
         break;
      case DSS_IFACE_IOCTL_707_SET_MDR:
         if (NULL == pArgVal) {
            res = QDS_EFAULT;
            dss_error_code = DSS_ERROR_NULL_ARGVAL;
            goto bail;
         }
         res = IDSNetwork1xScope.Fetch()->SetMDR(*reinterpret_cast<int*>(pArgVal));
         break;
      case DSS_IFACE_IOCTL_707_GET_DORM_TIMER:
         res = IDSNetwork1xPrivScope.Fetch()->GetDormancyTimer(reinterpret_cast<int*>(pArgVal));
         break;
      case DSS_IFACE_IOCTL_707_SET_DORM_TIMER:
         if (NULL == pArgVal) {
            res = QDS_EFAULT;
            dss_error_code = DSS_ERROR_NULL_ARGVAL;
            goto bail;
         }
         res = IDSNetwork1xPrivScope.Fetch()->SetDormancyTimer(*reinterpret_cast<int*>(pArgVal));
         break;
      case DSS_IFACE_IOCTL_707_GET_RLP_ALL_CURR_NAK:
         res = IDSNetwork1xScope.Fetch()->GetRLPAllCurrentNAK(reinterpret_cast<Network1xRLPOptionType*>(pArgVal));
         break;
      case DSS_IFACE_IOCTL_707_SET_RLP_ALL_CURR_NAK:
         res = IDSNetwork1xScope.Fetch()->SetRLPAllCurrentNAK(reinterpret_cast<Network1xRLPOptionType*>(pArgVal));
         break;
      case DSS_IFACE_IOCTL_707_GET_RLP_ALL_DEF_NAK:
         res = IDSNetwork1xScope.Fetch()->GetRLPDefCurrentNAK(reinterpret_cast<Network1xRLPOptionType*>(pArgVal));
         break;
      case DSS_IFACE_IOCTL_707_SET_RLP_ALL_DEF_NAK:
         res = IDSNetwork1xScope.Fetch()->SetRLPDefCurrentNAK(reinterpret_cast<Network1xRLPOptionType*>(pArgVal));
         break;
      case DSS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK:
         res = IDSNetwork1xScope.Fetch()->GetRLPNegCurrentNAK(reinterpret_cast<Network1xRLPOptionType*>(pArgVal));
         break;
      // TODO: the original ioctl expects byte* (not int*) (MUST be solved!)
      case DSS_IFACE_IOCTL_707_GET_RLP_QOS_NA_PRI:
         res = IDSNetwork1xScope.Fetch()->GetQoSNAPriority(reinterpret_cast<int*>(pArgVal));
         break;
      case DSS_IFACE_IOCTL_707_SET_RLP_QOS_NA_PRI:
         if (NULL == pArgVal) {
            res = QDS_EFAULT;
            dss_error_code = DSS_ERROR_NULL_ARGVAL;
            goto bail;
         }
         res = IDSNetwork1xScope.Fetch()->SetQoSNAPriority(*reinterpret_cast<int*>(pArgVal));
         break;
      case DSS_IFACE_IOCTL_707_SDB_SUPPORT_QUERY:
      {
         dss_iface_ioctl_707_sdb_support_query_type* pssq =
            reinterpret_cast<dss_iface_ioctl_707_sdb_support_query_type*>(pArgVal);
         res = IDSNetwork1xScope.Fetch()->QueryDoSSupport(DSSConversion::DS2IDSSDBFlags(pssq->flags), &pssq->can_do_sdb);
         break;
      }
      case DSS_IFACE_IOCTL_707_ENABLE_HOLDDOWN:
         if (NULL == pArgVal) {
            res = QDS_EFAULT;
            dss_error_code = DSS_ERROR_NULL_ARGVAL;
            goto bail;
         }
         res = IDSNetwork1xPrivScope.Fetch()->EnableHoldDown(*reinterpret_cast<boolean*>(pArgVal));
         break;
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_HPT_MODE:
      {
         dss_iface_ioctl_707_enable_hdr_hpt_mode_type  *hdr_hpt_mode = 
            (dss_iface_ioctl_707_enable_hdr_hpt_mode_type*) pArgVal;

         res = IDSNetwork1xPrivScope.Fetch()->EnableHDRHPTMode
               (
                  hdr_hpt_mode->enable
               );
         break;
      }

      case DSS_IFACE_IOCTL_707_ENABLE_HDR_SLOTTED_MODE:
      {
         dss_iface_ioctl_707_enable_hdr_slotted_mode_type* hdr_slotted_mode_ptr;

         if(pArgVal == NULL) {
            res = QDS_EFAULT;
            dss_error_code = DSS_ERROR_NULL_ARGVAL;
            break;
         }

         hdr_slotted_mode_ptr =(dss_iface_ioctl_707_enable_hdr_slotted_mode_type*) pArgVal;

         LOG_MSG_INFO1_6(
"IfaceIoctl707():dss_net_handle: %d," 
"enable:%d, slotted_mode_opion: %d, "
"get_slotted_mode:%d, event_cb:0x%p, user_data:0x%p ",
hdr_slotted_mode_ptr->dss_nethandle,
hdr_slotted_mode_ptr->enable,
hdr_slotted_mode_ptr->slotted_mode_option,
hdr_slotted_mode_ptr->get_slotted_mode,
hdr_slotted_mode_ptr->event_cb,
hdr_slotted_mode_ptr->user_data);

         /* Check whether intent is to get current slotted mode value */
         if(hdr_slotted_mode_ptr->get_slotted_mode == TRUE) 
         {
            uint32 sci = 0;
            res = IDSNetwork1xPrivScope.Fetch()->GetHDRSlottedModeCycleIndex(&sci);
            if (AEE_SUCCESS != res) 
            {
               break;
            }
            hdr_slotted_mode_ptr->slotted_mode_option = sci;
         }
         else if (hdr_slotted_mode_ptr->enable) 
         {
            ds::Net::Network1xPrivHDRSlottedModeArg arg;

            if (hdr_slotted_mode_ptr->event_cb)
            {
               // Register for SUCCESS event
               res = pNetMonitored->RegEventCB(
                  DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV,
                  hdr_slotted_mode_ptr->event_cb,
                  hdr_slotted_mode_ptr->user_data,
                  iface_id);

               if (res != AEE_SUCCESS) {
                  dss_error_code = DSS_ERROR_REG_EVENTCB_FAILED;
                  break;
               }

               // Register for FAILURE event
               res = pNetMonitored->RegEventCB(
                  DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV,
                  hdr_slotted_mode_ptr->event_cb,
                  hdr_slotted_mode_ptr->user_data,
                  iface_id);

               if (res != AEE_SUCCESS) {
                  dss_error_code = DSS_ERROR_REG_EVENTCB_FAILED;
                  break;
               }
            }
            else
            {
              LOG_MSG_INFO1_0("IfaceIoctl707(): "
                              "hdr_slotted_mode_ptr->event_cb is NULL");
            }

            arg.enable = hdr_slotted_mode_ptr->enable;
            arg.slottedModeOption = hdr_slotted_mode_ptr->slotted_mode_option;
            arg.getSlottedMode = FALSE;
            res = IDSNetwork1xPrivScope.Fetch()->EnableHDRSlottedMode(&arg);
        }
        else
        {
            // De-register SUCCESS event
            res = pNetMonitored->DeregEventCB(DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV);
            if (res != AEE_SUCCESS) {
               dss_error_code = DSS_ERROR_DEREG_EVENTCB_FAILED;
               break;
            }

            // De-register FAILURE event
            res = pNetMonitored->DeregEventCB(DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV);
            if (res != AEE_SUCCESS) {
               dss_error_code = DSS_ERROR_DEREG_EVENTCB_FAILED;
               break;
            }
         }
      }
      break;

      case DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INTERIA:
      {
         dss_iface_ioctl_707_enable_hdr_rev0_rate_inertia_type *rev0_inertia = 
      (dss_iface_ioctl_707_enable_hdr_rev0_rate_inertia_type *)pArgVal; 

      if (rev0_inertia->event_cb)
          {
             res = pNetMonitored->RegEventCB
                   (
                      DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA_SUCCESS_EV,
                      rev0_inertia->event_cb,
                      rev0_inertia->user_data,
                      iface_id
                   );

             res = pNetMonitored->RegEventCB
                   (
                      DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA_FAILURE_EV,
                      rev0_inertia->event_cb,
                      rev0_inertia->user_data,
                      iface_id
                   );
          }

         res = IDSNetwork1xPrivScope.Fetch()->EnableHDRRev0RateInertia(rev0_inertia->enable);
      }
      break;

      // TODO: the new RMAC3 info fields are of type int - originally they were uint16
      case DSS_IFACE_IOCTL_707_HDR_GET_RMAC3_INFO:
         if (NULL == pArgVal) {
            dss_error_code = DSS_ERROR_NULL_ARGVAL;
            res = QDS_EFAULT;
            goto bail;
         }
         else {
            IQoS1x::RMAC3InfoType idsInfo = {0, 0, 0};
            dss_iface_ioctl_707_hdr_rmac3_info_type* pdssInfo =
               reinterpret_cast<dss_iface_ioctl_707_hdr_rmac3_info_type*>(pArgVal);
            // this cannot happen cause in the switch above we create QoS1x for this case
            // (case DSS_IFACE_IOCTL_707_HDR_GET_RMAC3_INFO:), shut up KW
            if (NULL == piQoS1x) {
               DATA_ERR_FATAL("NULL == piQoS1x");
               goto bail;
            }
            else {
               res = piQoS1x->GetRMAC3Info(&idsInfo);
               pdssInfo->ps_headroom_payload_size = (uint16)idsInfo.headroomPayloadSize;
               pdssInfo->bucket_level_payload_size = (uint16)idsInfo.bucketLevelPayloadSize;
               pdssInfo->t2p_inflow_payload_size = (uint16)idsInfo.t2pInflowPayloadSize;
            }
         }
         break;
         
      case DSS_IFACE_IOCTL_707_GET_TX_STATUS:
         if (NULL == pArgVal) {
            dss_error_code = DSS_ERROR_NULL_ARGVAL;
            res = QDS_EFAULT;
            goto bail;
         }

         // this cannot happen cause in the switch above we create QoS1x for this case
         // (case DSS_IFACE_IOCTL_707_GET_TX_STATUS:), shut up KW
         if(NULL == piQoS1x){
            DATA_ERR_FATAL("NULL == piQoS1x");
            goto bail;
         }
         else{
            res = piQoS1x->GetTXStatus(reinterpret_cast<boolean*>(pArgVal));
         }
         break;
      // TODO: for INACTIVITY_TIMER - the original ioctl expects uint32* (not int*)
      case DSS_IFACE_IOCTL_707_GET_INACTIVITY_TIMER:
         if (NULL == pArgVal) {
            dss_error_code = DSS_ERROR_NULL_ARGVAL;
            res = QDS_EFAULT;
            goto bail;
         }

         // this cannot happen cause in the switch above we create QoS1x for this case
         // (case DSS_IFACE_IOCTL_707_GET_INACTIVITY_TIMER:), shut up KW
         if(NULL == piQoS1x){
            DATA_ERR_FATAL("NULL == piQoS1x");
            goto bail;
         }
         else{
            res = piQoS1x->GetInactivityTimer(reinterpret_cast<int*>(pArgVal));
         }
         break;
      case DSS_IFACE_IOCTL_707_SET_INACTIVITY_TIMER:
         if (NULL == pArgVal) {
            dss_error_code = DSS_ERROR_NULL_ARGVAL;
            res = QDS_EFAULT;
            goto bail;
         }

         // this cannot happen cause in the switch above we create QoS1x for this case
         // (case DSS_IFACE_IOCTL_707_SET_INACTIVITY_TIMER:), shut up KW
         if(NULL == piQoS1x){
            DATA_ERR_FATAL("NULL == piQoS1x");
            goto bail;
         }
         else{
            res = piQoS1x->SetInactivityTimer(*reinterpret_cast<int*>(pArgVal));
         }
         break;
      case DSS_IFACE_IOCTL_GET_SESSION_TIMER:
         if (NULL == pArgVal) {
            dss_error_code = DSS_ERROR_NULL_ARGVAL;
            res = QDS_EFAULT;
            goto bail;
         }
         else {
            Network1xPrivSessionTimerType idsTimer = {0, 0, {0}};
            dss_session_timer_type* pdssTimer =
               reinterpret_cast<dss_session_timer_type*>(pArgVal);
            res = DSSConversion::DS2IDSSessionTimerSelect(pdssTimer->timer_select, &(idsTimer.select));
            if (AEE_SUCCESS != res){
               break;
            }
            res = IDSNetwork1xPrivScope.Fetch()->GetSessionTimer(&idsTimer);
            pdssTimer->timer_val = idsTimer.value;
         }
         break;
      case DSS_IFACE_IOCTL_SET_SESSION_TIMER:
         if (NULL == pArgVal) {
            dss_error_code = DSS_ERROR_NULL_ARGVAL;
            res = QDS_EFAULT;
            goto bail;
         }
         else {
            Network1xPrivSessionTimerType idsTimer = {0, 0, {0}};
            dss_session_timer_type* pdssTimer =
               reinterpret_cast<dss_session_timer_type*>(pArgVal);
            res = DSSConversion::DS2IDSSessionTimerSelect(pdssTimer->timer_select,&(idsTimer.select));
            if (AEE_SUCCESS != res){
               dss_error_code = DSS_ERROR_DS2IDSSESSION_TIMER_SELECT_FAILED;
               break;
            }
            idsTimer.value = pdssTimer->timer_val;
            res = IDSNetwork1xPrivScope.Fetch()->SetSessionTimer(&idsTimer);
         }
         break;

      case DSS_IFACE_IOCTL_707_SET_HYSTERESIS_ACT_TIMER:
        if(NULL != pArgVal){
           res = IDSNetwork1xScope.Fetch()->SetHysteresisTimer(*reinterpret_cast<Network1xPrivHysteresisTimerType*>(pArgVal));
        }
        else{
           dss_error_code = DSS_ERROR_NULL_ARGVAL;
           res            = QDS_EBADF;
        }
        break;

      case DSS_IFACE_IOCTL_707_GET_HYSTERESIS_ACT_TIMER:
      {
        if(NULL != pArgVal){
           res = IDSNetwork1xScope.Fetch()->GetHysteresisTimer(reinterpret_cast<Network1xPrivHysteresisTimerType*>(pArgVal));
        }
        else{
           dss_error_code = DSS_ERROR_NULL_ARGVAL;
           res            = QDS_EBADF;
        }
        break;
      }

      case DSS_IFACE_IOCTL_707_GET_SIP_MIP_DATA_CALL_TYPE:
        {
          if(NULL != pArgVal){
            res = IDSNetwork1xScope.Fetch()->GetSIPMIPCallType(reinterpret_cast<Network1xSIPMIPCallType*>(pArgVal));
          }
          else{
            dss_error_code = DSS_ERROR_NULL_ARGVAL;
            res            = QDS_EBADF;
          }
          break;
        }

      default:
         res = QDS_EOPNOTSUPP;
         break;
   }

/* fall through */

bail:

   DSSCommon::ReleaseIf((IQI**)&piQoS1x);

   DSSCommon::ReleaseIf((IQI**)&pIDSNetQoSDefault);

   DSSCommon::ReleaseIf((IQI**)&pIDSNetQoSSecondary);

   if(0 != pDSSNetQoSSecondary){
      delete pDSSNetQoSSecondary;
   }

   if(AEE_SUCCESS != res){
      result = DSS_ERROR;
   }
   else{
      result = DSS_SUCCESS;
   }

   LOG_MSG_FUNCTION_EXIT_3("IfaceIoctl707(): "
                           "error %d, error_arg %d, res %d ",
                            dss_error_code, err_arg, res);

   *sErrno = DSSConversion::IDS2DSErrorCode(res);

   return result;
}

int dss_iface_ioctl_get_all_ifaces(void* argval_ptr, sint15* dss_errno)
{
  dss_iface_ioctl_all_ifaces_type* arg = reinterpret_cast<dss_iface_ioctl_all_ifaces_type*>(argval_ptr);
  IfaceIdType                      aNetTypes[MAX_SYSTEM_IFACES];
  int                              nLen = 0;
  INetworkFactory*                 pIDSNetworkFactory = NULL;
  INetwork*                        pIDSNetwork = NULL;
  INetworkPriv*                    pIDSNetworkPriv = NULL;
  AEEResult                        res = AEE_SUCCESS;
  sint15                           ret = DSS_SUCCESS;
  DSSGenScope                      scopeNetworkFactory;

  if (NULL == argval_ptr) {
    LOG_MSG_ERROR_0("argval_ptr is NULL");
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  // get network factory
  res = DSSGlobals::Instance()->GetNetworkFactory(&pIDSNetworkFactory);
  if (AEE_SUCCESS != res) {
     goto bail;
  }
  scopeNetworkFactory.SetParams(pIDSNetworkFactory, DSSGenScope::IDSIQI_TYPE);

  // create NetworkMonitored object
  res = pIDSNetworkFactory->CreateNetwork(ds::Net::NetworkMode::QDS_MONITORED,
                                          NULL,
                                          &pIDSNetwork);
  if (AEE_SUCCESS != res) {
     goto bail;
  }

  res = pIDSNetwork->QueryInterface(AEEIID_INetworkPriv, (void**)&pIDSNetworkPriv);
  if (AEE_SUCCESS != res) {
     LOG_MSG_ERROR_1("dss_iface_ioctl_internal(): "
                     "QueryInterface(AEEIID_IDSNetIPFilterMgr) failed: %d",
                     res);
     goto bail;
  }

  res = pIDSNetworkPriv->GetAllIfaces(aNetTypes, MAX_SYSTEM_IFACES, &nLen);
  if (AEE_SUCCESS != res) {
     goto bail;
  }

  if (MAX_SYSTEM_IFACES < nLen) {
     LOG_MSG_INFO1_0("dss_iface_ioctl_internal(): "
                     "There are more ifaces than allowed");
  }

  nLen = min(nLen, MAX_SYSTEM_IFACES);
  arg->number_of_ifaces = nLen;
  for (int i = 0; i < nLen; i++) {
     arg->ifaces[i] = (dss_iface_id_type)aNetTypes[i];
  }

bail:
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      ret = DSS_ERROR;
   }

   DSSCommon::ReleaseIf((IQI**)&pIDSNetwork);
   DSSCommon::ReleaseIf((IQI**)&pIDSNetworkPriv);
   return ret;
}

boolean ioctl_is_qos(dss_iface_ioctl_type ioctl_name)
{
  switch (ioctl_name) {
     case DSS_IFACE_IOCTL_QOS_MODIFY:
     case DSS_IFACE_IOCTL_QOS_RELEASE:
     case DSS_IFACE_IOCTL_QOS_SUSPEND:
     case DSS_IFACE_IOCTL_QOS_RESUME:
        //case DSS_IFACE_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC: // this operation can be performed on temporary app
     case DSS_IFACE_IOCTL_QOS_GET_STATUS:
     case DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC:
     case DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2:
     case DSS_IFACE_IOCTL_QOS_RELEASE_EX:
     case DSS_IFACE_IOCTL_QOS_SUSPEND_EX:
     case DSS_IFACE_IOCTL_QOS_RESUME_EX:
     case DSS_IFACE_IOCTL_707_HDR_GET_RMAC3_INFO:
     case DSS_IFACE_IOCTL_707_GET_TX_STATUS:
     case DSS_IFACE_IOCTL_707_GET_INACTIVITY_TIMER:
     case DSS_IFACE_IOCTL_707_SET_INACTIVITY_TIMER:
        return TRUE;

     default:
        return FALSE;
  }
}

// check that ioctl_name value is legal
boolean ioctl_is_legal(dss_iface_ioctl_type ioctl_name)
{
   boolean ret = FALSE;
   if ((DSS_IFACE_IOCTL_BCMCS_BOM_CACHING_SETUP != ioctl_name) &&
      // ioctl_name shouldn't be within reserved values
      !((DSS_IFACE_IOCTL_RESERVED_MIN <= ioctl_name) && (DSS_IFACE_IOCTL_RESERVED_MAX >= ioctl_name)))
   {
        ret = TRUE;
   }

   return ret;
}

/*===========================================================================
FUNCTION DSS_IFACE_IOCTL()

DESCRIPTION
This function determines the ps_iface_ptr associated with the passed in
identifier. It then calls ps_iface_ioctl().

DEPENDENCIES
None.

PARAMETERS

dss_iface_id_type         - Interface ID on which the specified operations
is to be performed

dss_iface_ioctl_type      - The operation name

void*                     - Pointer to operation specific structure

sint15*                   - Error code returned in case of failure (Error
values are those defined in dserrno.h)

DS_EBADF - Returned by dss_iface_ioctl() if the
specified id_ptr is invalid (i.e. id_ptr does
not map to a valid ps_iface_ptr).

DS_EINVAL - Returned by dss_iface_ioctl() when
the specified IOCTL does not belong to the
common set of IOCTLs and there is no IOCTL mode
handler registered for the specified interface.

DS_EOPNOTSUPP - Returned by the lower level
IOCTL mode handler when specified IOCTL is not
supported by the interface. For instance, this
would be returned by interfaces that do not
support a certain "iface specific common IOCTL"
(i.e. these are common IOCTLs, but the
implementation is mode specific, for example,
GO_DORMANT).

DS_EFAULT - This error code is returned if the
specified arguments for the IOCTL are incorrect
or if dss_iface_ioctl() or a mode handler
encounters an error while executing the IOCTL..
For instance, if the 1X interface cannot
"GO_DORMANT" it would return this error.

DS_NOMEMORY - This error code is returned if we
run out of buffers during execution.

RETURN VALUE
0 - on success
-1 - on failure

SIDE EFFECTS
None.

===========================================================================*/
int dss_iface_ioctl
(
  dss_iface_id_type iface_id,
  dss_iface_ioctl_type    ioctl_name,
  void                   *argval_ptr,
  sint15 *dss_errno
)
{
   sint15 netHandle = DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID;
   int calltype = DSS_IFACE_3GPP_LTE_REESTAB_CALLTYPE_NORMAL;
   DSSNetApp* pNetApp = NULL;
   DSSIDSNetworkPrivScope IDSNetworkPrivScope;
   DSSIDSNetworkScope IDSNetworkScope;
   DSSIDSNetworkExtScope IDSNetworkExtScope;
   DSSIDSNetwork1xScope IDSNetwork1xScope;
   DSSNetActiveScope netActiveScope;
   DSSNetMonitoredScope netMonitoredScope;
   bool bAllowSecondary = FALSE;
   bool bAllowToCreateSecondary = TRUE;
   bool bForbidTemporary = FALSE;
   AEEResult res = AEE_SUCCESS;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   int32 err_arg = DSS_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_4("dss_iface_ioctl(): iface_id:0x%x, "
                            "ioctl_name:0x%x, argval_ptr:0x%p, errno:0x%x",
                            iface_id, ioctl_name, argval_ptr, dss_errno);

   // check whether dss_errno is NULL
   if (NULL == dss_errno) {
      return DSS_ERROR;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   // check that ioctl_name value is legal
   if (FALSE == ioctl_is_legal(ioctl_name))
   {
      dss_error_code = DSS_ERROR_ILLEGAL_IOCTL_NAME;
      *dss_errno     = DS_EOPNOTSUPP;
      goto bail;
   }

   // Initialize returned error code
   *dss_errno = DSS_SUCCESS;

   // Handle the ioctls that don't care about the iface id before checking validity.
   if (DSS_IFACE_IOCTL_GET_ALL_IFACES == ioctl_name) {
      return dss_iface_ioctl_get_all_ifaces(argval_ptr, dss_errno);
   }

   // For IOCTLs that get app_id in argval fetch the app_id from the argval
   // Forbid temporary , unless ioctl is not in the list
   bForbidTemporary = TRUE;
   switch (ioctl_name) {
      case DSS_IFACE_IOCTL_REG_EVENT_CB:
      {
        if (NULL == argval_ptr) {
           *dss_errno = DS_EFAULT;
           return DSS_ERROR;
        }

         dss_iface_ioctl_ev_cb_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_ev_cb_type*>(argval_ptr);
         netHandle = pEvRegArgval->app_id;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }
         bAllowSecondary = TRUE;
         break;
      }

      case DSS_IFACE_IOCTL_DEREG_EVENT_CB:
      {
        if (NULL == argval_ptr) {
           *dss_errno = DS_EFAULT;
           return DSS_ERROR;
        }

         dss_iface_ioctl_ev_cb_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_ev_cb_type*>(argval_ptr);
         netHandle = pEvRegArgval->app_id;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }
         bAllowSecondary = TRUE;
         bAllowToCreateSecondary = FALSE;
         break;
      }


      case DSS_IFACE_IOCTL_QOS_REQUEST:
      {
         dss_iface_ioctl_qos_request_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_qos_request_type*>(argval_ptr);
         netHandle = pEvRegArgval->app_id;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_QOS_REQUEST_EX:
      {
         dss_iface_ioctl_qos_request_ex_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_qos_request_ex_type*>(argval_ptr);
         netHandle = pEvRegArgval->app_id;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_PRIMARY_QOS_MODIFY:
      {
         dss_iface_ioctl_primary_qos_modify_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_primary_qos_modify_type*>(argval_ptr);
         netHandle = pEvRegArgval->dss_nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }
#ifdef FEATUTE_DATA_PS_MCAST
      case DSS_IFACE_IOCTL_MCAST_JOIN:
      {
         dss_iface_ioctl_mcast_join_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_mcast_join_type*>(argval_ptr);
         netHandle = pEvRegArgval->app_id;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }


      case DSS_IFACE_IOCTL_MCAST_LEAVE:
      {
         dss_iface_ioctl_mcast_leave_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_mcast_leave_type*>(argval_ptr);
         netHandle = pEvRegArgval->app_id;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_MCAST_JOIN_EX:
      {
         dss_iface_ioctl_mcast_join_ex_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_mcast_join_ex_type*>(argval_ptr);
         netHandle = pEvRegArgval->dss_nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_MCAST_LEAVE_EX:
      {
         dss_iface_ioctl_mcast_leave_ex_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_mcast_leave_ex_type*>(argval_ptr);
         netHandle = pEvRegArgval->dss_nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_MCAST_REGISTER_EX:
      {
         dss_iface_ioctl_mcast_register_ex_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_mcast_register_ex_type*>(argval_ptr);
         netHandle = pEvRegArgval->dss_nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_MBMS_MCAST_CONTEXT_ACTIVATE:
      {
         dss_iface_ioctl_mbms_mcast_context_act_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_mbms_mcast_context_act_type*>(argval_ptr);
         netHandle = pEvRegArgval->nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_MBMS_MCAST_CONTEXT_DEACTIVATE:
      {
         dss_iface_ioctl_mbms_mcast_context_deact_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_mbms_mcast_context_deact_type*>(argval_ptr);
         netHandle = pEvRegArgval->nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }
#endif // FEATUTE_DATA_PS_MCAST
      case DSS_IFACE_IOCTL_MT_REG_CB:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_mt_reg_cb_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_mt_reg_cb_type*>(argval_ptr);
         netHandle = pEvRegArgval->app_id;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_MT_DEREG_CB:
      {

         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_mt_dereg_cb_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_mt_dereg_cb_type*>(argval_ptr);
         netHandle = pEvRegArgval->app_id;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_707_ENABLE_HDR_HPT_MODE:
      {
         dss_iface_ioctl_707_enable_hdr_hpt_mode_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_707_enable_hdr_hpt_mode_type*>(argval_ptr);
         netHandle = pEvRegArgval->dss_nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INTERIA:
      {
         dss_iface_ioctl_707_enable_hdr_rev0_rate_inertia_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_707_enable_hdr_rev0_rate_inertia_type*>(argval_ptr);
         netHandle = pEvRegArgval->dss_nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_707_ENABLE_HDR_SLOTTED_MODE:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_707_enable_hdr_slotted_mode_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_707_enable_hdr_slotted_mode_type*>(argval_ptr);
         netHandle = pEvRegArgval->dss_nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_707_GET_MIP_MA_INFO:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }
         break;
      }

      case DSS_IFACE_IOCTL_ENABLE_FIREWALL:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_enable_firewall_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_enable_firewall_type*>(argval_ptr);
         netHandle = pEvRegArgval->dss_nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }
    
         break;
      }

      case DSS_IFACE_IOCTL_DISABLE_FIREWALL:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_disable_firewall_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_disable_firewall_type*>(argval_ptr);
         netHandle = pEvRegArgval->dss_nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_ADD_FIREWALL_RULE:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_add_firewall_rule_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_add_firewall_rule_type*>(argval_ptr);
         netHandle = pEvRegArgval->dss_nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_DELETE_FIREWALL_RULE:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_delete_firewall_rule_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_delete_firewall_rule_type*>(argval_ptr);
         netHandle = pEvRegArgval->dss_nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_GET_FIREWALL_RULE:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_get_firewall_rule_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_get_firewall_rule_type*>(argval_ptr);
         netHandle = pEvRegArgval->dss_nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno     = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_GET_FIREWALL_TABLE:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_get_firewall_table_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_get_firewall_table_type*>(argval_ptr);
         netHandle = pEvRegArgval->dss_nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno     = DS_EBADAPP;
            goto bail;
         }

         break;
      }

      case DSS_IFACE_IOCTL_ADD_STATIC_NAT_ENTRY:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_add_static_nat_entry_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_add_static_nat_entry_type*>(argval_ptr);
         netHandle = pEvRegArgval->nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno     = DS_EBADAPP;
            goto bail;
         }
  
         break;     
       }

      case DSS_IFACE_IOCTL_DELETE_STATIC_NAT_ENTRY:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_delete_static_nat_entry_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_delete_static_nat_entry_type*>(argval_ptr);
         netHandle = pEvRegArgval->nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno     = DS_EBADAPP;
            goto bail;
         }
  
         break;     
      }


      case DSS_IFACE_IOCTL_GET_STATIC_NAT_ENTRY:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_get_static_nat_entry_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_get_static_nat_entry_type*>(argval_ptr);
         netHandle = pEvRegArgval->nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno     = DS_EBADAPP;
            goto bail;
         }

         break;     
      }

      case DSS_IFACE_IOCTL_GET_DYNAMIC_NAT_ENTRY_TIMEOUT:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_get_dynamic_nat_entry_timeout_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_get_dynamic_nat_entry_timeout_type*>(argval_ptr);
         netHandle = pEvRegArgval->nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno     = DS_EBADAPP;
            goto bail;
         }
  
         break;     
      }

      case DSS_IFACE_IOCTL_SET_DYNAMIC_NAT_ENTRY_TIMEOUT:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_set_dynamic_nat_entry_timeout_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_set_dynamic_nat_entry_timeout_type*>(argval_ptr);
         netHandle = pEvRegArgval->nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno     = DS_EBADAPP;
            goto bail;
         }

         break;     
      }

      case DSS_IFACE_IOCTL_GET_NAT_IPSEC_VPN_PASS_THROUGH:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_nat_ipsec_vpn_pass_through_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_nat_ipsec_vpn_pass_through_type*>(argval_ptr);
         netHandle = pEvRegArgval->nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno     = DS_EBADAPP;
            goto bail;
         }
  
         break;     
      }

      case DSS_IFACE_IOCTL_SET_NAT_IPSEC_VPN_PASS_THROUGH:
      {
         if (NULL == argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         dss_iface_ioctl_nat_ipsec_vpn_pass_through_type* pEvRegArgval =
            reinterpret_cast<dss_iface_ioctl_nat_ipsec_vpn_pass_through_type*>(argval_ptr);
         netHandle = pEvRegArgval->nethandle;
         if (0 > netHandle) {
            dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg        = netHandle;
            *dss_errno     = DS_EBADAPP;
            goto bail;
         }

         break;     
      }

     case DSS_IFACE_IOCTL_GET_NAT_L2TP_VPN_PASS_THROUGH:
     {
       if (NULL == argval_ptr) {
           *dss_errno = DS_EFAULT;
           return DSS_ERROR;
        }

        dss_iface_ioctl_nat_l2tp_vpn_pass_through_type* pEvRegArgval =
           reinterpret_cast<dss_iface_ioctl_nat_l2tp_vpn_pass_through_type*>(argval_ptr);
        netHandle = pEvRegArgval->nethandle;
        if (0 > netHandle) {
           dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg       = netHandle;
           *dss_errno     = DS_EBADAPP;
           goto bail;
        }
  
        break;     
     }
  
     case DSS_IFACE_IOCTL_SET_NAT_L2TP_VPN_PASS_THROUGH:
     {
        if (NULL == argval_ptr) {
           *dss_errno = DS_EFAULT;
           return DSS_ERROR;
        }

        dss_iface_ioctl_nat_l2tp_vpn_pass_through_type* pEvRegArgval =
           reinterpret_cast<dss_iface_ioctl_nat_l2tp_vpn_pass_through_type*>(argval_ptr);
        netHandle = pEvRegArgval->nethandle;
        if (0 > netHandle) {
           dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg       = netHandle;
           *dss_errno     = DS_EBADAPP;
           goto bail;
        }
  
        break;     
     }

     case DSS_IFACE_IOCTL_GET_NAT_PPTP_VPN_PASS_THROUGH:
     {
        if (NULL == argval_ptr) {
           *dss_errno = DS_EFAULT;
           return DSS_ERROR;
        }

        dss_iface_ioctl_nat_pptp_vpn_pass_through_type* pEvRegArgval =
           reinterpret_cast<dss_iface_ioctl_nat_pptp_vpn_pass_through_type*>(argval_ptr);
        netHandle = pEvRegArgval->nethandle;
        if (0 > netHandle) {
           dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg       = netHandle;
           *dss_errno = DS_EBADAPP;
           goto bail;
        }
  
        break;     
     }
  
     case DSS_IFACE_IOCTL_SET_NAT_PPTP_VPN_PASS_THROUGH:
     {
        if (NULL == argval_ptr) {
           *dss_errno = DS_EFAULT;
           return DSS_ERROR;
        }

        dss_iface_ioctl_nat_pptp_vpn_pass_through_type* pEvRegArgval =
           reinterpret_cast<dss_iface_ioctl_nat_pptp_vpn_pass_through_type*>(argval_ptr);
        netHandle = pEvRegArgval->nethandle;
        if (0 > netHandle) {
           dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg       = netHandle;
           *dss_errno = DS_EBADAPP;
           goto bail;
        }
  
        break;     
     }

     case DSS_IFACE_IOCTL_ADD_DMZ:
     {
        if (NULL == argval_ptr) {
           *dss_errno = DS_EFAULT;
           return DSS_ERROR;
        }

        dss_iface_ioctl_dmz_type* pEvRegArgval =
           reinterpret_cast<dss_iface_ioctl_dmz_type*>(argval_ptr);
        netHandle = pEvRegArgval->nethandle;
        if (0 > netHandle) {
           dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg       = netHandle;
           *dss_errno = DS_EBADAPP;
           goto bail;
        }
  
        break;     
      }

     case DSS_IFACE_IOCTL_GET_DMZ:
     {
        if (NULL == argval_ptr) {
           *dss_errno = DS_EFAULT;
           return DSS_ERROR;
        }

        dss_iface_ioctl_dmz_type* pEvRegArgval =
           reinterpret_cast<dss_iface_ioctl_dmz_type*>(argval_ptr);
        netHandle = pEvRegArgval->nethandle;
        if (0 > netHandle) {
           dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg       = netHandle;
           *dss_errno = DS_EBADAPP;
           goto bail;
        }
  
        break;     
      }

     case DSS_IFACE_IOCTL_DELETE_DMZ:
     {
        if (NULL == argval_ptr) {
           *dss_errno = DS_EFAULT;
           return DSS_ERROR;
        }

        dss_iface_ioctl_delete_dmz_type* pEvRegArgval =
           reinterpret_cast<dss_iface_ioctl_delete_dmz_type*>(argval_ptr);
        netHandle = pEvRegArgval->nethandle;
        if (0 > netHandle) {
           dss_error_code = DSS_ERROR_NEGATIVE_NETHANDLE;
            err_arg       = netHandle;
           *dss_errno = DS_EBADAPP;
           goto bail;
        }
  
        break;
      }
     case DSS_IFACE_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE:
     {
        dss_iface_ioctl_3gpp_get_lte_reestab_calltype_type* pEvRegArgval =
           reinterpret_cast<dss_iface_ioctl_3gpp_get_lte_reestab_calltype_type*>(argval_ptr);
        calltype = pEvRegArgval->calltype;
        if (0 > calltype) {
           dss_error_code = DSS_ERROR_NEGATIVE_CALLTYPE;
            err_arg       = calltype;
           *dss_errno = DS_EBADAPP;
           goto bail;
        }
        bAllowToCreateSecondary = FALSE;
        bForbidTemporary = TRUE;
        break;
     }
     case DSS_IFACE_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE:
     {
        dss_iface_ioctl_3gpp_set_lte_reestab_calltype_type* pEvRegArgval =
           reinterpret_cast<dss_iface_ioctl_3gpp_set_lte_reestab_calltype_type*>(argval_ptr);
        calltype = pEvRegArgval->calltype;
        if (0 > calltype) {
           dss_error_code = DSS_ERROR_NEGATIVE_CALLTYPE;
            err_arg       = calltype;
           *dss_errno = DS_EBADAPP;
           goto bail;
        }
        bAllowToCreateSecondary = FALSE;
        bForbidTemporary = TRUE;
        break;
     }
     case DSS_IFACE_IOCTL_SET_SILENT_REDIAL:
     {
        bAllowToCreateSecondary = FALSE;
        bForbidTemporary = TRUE;
        break;
     }
     case DSS_IFACE_IOCTL_3GPP_FORCE_PDN_DISCONNECT:
     {
        bAllowToCreateSecondary = FALSE;
        bForbidTemporary = TRUE;
        break;
     }
     case DSS_IFACE_IOCTL_KEEP_ALIVE:
     {
        if(NULL == argval_ptr) {
           *dss_errno = DS_EFAULT;
           return DSS_ERROR;
        }
        break;
     }
     case DSS_IFACE_IOCTL_FILTER_MATCH_REQUEST:
     {
        dss_iface_ioctl_filter_match_request_type* pEvRegArgval =
           reinterpret_cast<dss_iface_ioctl_filter_match_request_type*>(argval_ptr);
        if (NULL == pEvRegArgval || 80 < pEvRegArgval->num_filters ) {
           LOG_MSG_ERROR_0("more number of filters");
           *dss_errno = DS_EBADAPP;
           return DSS_ERROR;
        }
	 	bAllowToCreateSecondary = FALSE;
        bForbidTemporary = TRUE;
        break;
     }
      default:
         // Ioctl is not in the list , temporary will not be forbidden
         bForbidTemporary = FALSE;
   }

   // TODO (security!): write enhancement to keep a list of iface_ids provided to apps, and verify iface_id specified by apps
   // against that list before applying IOCTLs.
   // TODO: consider adding a flag for secondary creation
   bForbidTemporary = bForbidTemporary || ioctl_is_qos(ioctl_name);
   res = DSSGlobals::Instance()->GetNetObjects(netHandle,
                                               iface_id,
                                               bAllowSecondary,
                                               bAllowToCreateSecondary,
                                               bForbidTemporary,
                                               &pNetApp,
                                               &netActiveScope, 
                                               &netMonitoredScope,
                                               ioctl_name,
                                               argval_ptr);

   if ( (AEE_SUCCESS != res) ||
        (NULL == pNetApp && NULL == netActiveScope.Fetch() && NULL == netMonitoredScope.Fetch()) ) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      dss_error_code = DSS_ERROR_GET_NET_OBJECTS_FAILED;
      err_arg        = res;
      goto bail;
   }

   // Note to the developer: IDSNetworkPrivScope.Fetch can only be used after IDSNetworkPrivScope.Init is called.
   // If IDSNetworkPrivScope.Fetch is needed before this point be sure to move IDSNetworkPrivScope.Init before it.
   res = IDSNetworkPrivScope.Init(netMonitoredScope.Fetch());
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      dss_error_code = DSS_ERROR_IDSNETWORKPRIVSCOPE_INIT_FAILED;
      err_arg        = res;
      goto bail;
   }

   res = IDSNetworkScope.Init(netMonitoredScope.Fetch());
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      dss_error_code = DSS_ERROR_IDSNETWORKSCOPE_INIT_FAILED;
      err_arg        = res;
      goto bail;
   }

   res = IDSNetworkExtScope.Init(netMonitoredScope.Fetch());
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      dss_error_code = DSS_ERROR_IDSNETWORKEXTSCOPE_INIT_FAILED;
      err_arg        = res;
      goto bail;
   }

   switch (ioctl_name) {
      case DSS_IFACE_IOCTL_GET_IPV4_ADDR:
         return dss_iface_ioctl_get_ipv4_addr(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_IPV6_ADDR:
         return dss_iface_ioctl_get_ipv6_addr(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR:
         return dss_iface_ioctl_get_ipv4_prim_dns_addr(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_IPV6_PRIM_DNS_ADDR:
         return dss_iface_ioctl_get_ipv6_prim_dns_addr(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR:
         return dss_iface_ioctl_get_ipv4_seco_dns_addr(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_IPV6_SECO_DNS_ADDR:
         return dss_iface_ioctl_get_ipv6_seco_dns_addr(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_ALL_DNS_ADDRS:
         return dss_iface_ioctl_get_all_dns_addrs(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_MTU:
         return dss_iface_ioctl_get_mtu(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_IP_ADDR:
         return dss_iface_ioctl_get_ip_addr(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_STATE:
         return dss_iface_ioctl_get_state(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_PHYS_LINK_STATE:
         return dss_iface_ioctl_get_phys_link_state(argval_ptr, pNetApp, dss_errno,iface_id);

      case DSS_IFACE_IOCTL_GET_RF_CONDITIONS:
         return dss_iface_ioctl_get_rf_conditions(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_REG_EVENT_CB:
         return dss_iface_ioctl_reg_event_cb(argval_ptr, netMonitoredScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_DEREG_EVENT_CB:
         return dss_iface_ioctl_dereg_event_cb(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_HW_ADDR:
         return dss_iface_ioctl_get_hw_addr(argval_ptr, &IDSNetworkScope, dss_errno);

      case DSS_IFACE_IOCTL_GET_IFACE_NAME:
         return dss_iface_ioctl_get_iface_name(argval_ptr, &IDSNetworkScope, dss_errno);

      case DSS_IFACE_IOCTL_GET_IP_FAMILY:
         return dss_iface_ioctl_get_ip_family(argval_ptr, &IDSNetworkScope, dss_errno);

      case DSS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY:
         return dss_iface_ioctl_get_bearer_technology(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY_EX:
         return dss_iface_ioctl_get_bearer_technology_ex(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_DATA_PATH_BRIDGE_MODE:
        if (NULL == netActiveScope.Fetch()) {
          dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
          *dss_errno = DS_ENETNONET;
          goto bail;
        }
        return dss_iface_ioctl_get_data_path(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_SET_DATA_PATH_BRIDGE_MODE:
        if (NULL == netActiveScope.Fetch()) {
          dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
          *dss_errno = DS_ENETNONET;
          goto bail;
        }
        return dss_iface_ioctl_set_data_path_bridge_mode(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_OP_RES_PCO:
        if (NULL == netActiveScope.Fetch()) {
          dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
          *dss_errno = DS_ENETNONET;
          goto bail;
        }
        return dss_iface_ioctl_get_op_res_pco(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_get_3gpp_lte_reestab_calltype(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_set_3gpp_lte_reestab_calltype(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

     case DSS_IFACE_IOCTL_SET_SILENT_REDIAL:
        if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_set_silent_redial(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_3GPP_FORCE_PDN_DISCONNECT:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
       }
       return dss_iface_ioctl_3gpp_force_pdn_disconnect(netMonitoredScope.Fetch(), dss_errno);

	   case DSS_IFACE_IOCTL_KEEP_ALIVE:
	   	  if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
          }
	      return dss_iface_ioctl_set_keep_alive(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_DATA_BEARER_RATE:
         return dss_iface_ioctl_get_data_bearer_rate(argval_ptr, &IDSNetworkExtScope, dss_errno);

      case DSS_IFACE_IOCTL_707_GET_MIP_MA_INFO:
         return dss_iface_ioctl_get_mip_ma_info(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_IS_LAPTOP_CALL_ACTIVE:
         return dss_iface_ioctl_is_laptop_call_active(argval_ptr, &IDSNetworkPrivScope, dss_errno);

      case DSS_IFACE_IOCTL_GET_SIP_SERV_ADDR:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_get_sip_serv_addr(argval_ptr, &IDSNetworkScope, dss_errno);

      case DSS_IFACE_IOCTL_GET_SIP_SERV_DOMAIN_NAMES:
         return dss_iface_ioctl_get_sip_serv_domain_names(argval_ptr, &IDSNetworkScope, dss_errno);

      case DSS_IFACE_IOCTL_GENERATE_PRIV_IPV6_ADDR:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_generate_priv_ipv6_addr(argval_ptr, netActiveScope.Fetch(), &IDSNetworkScope, dss_errno,iface_id);

      case DSS_IFACE_IOCTL_GET_ALL_V6_PREFIXES:
        return dss_iface_ioctl_get_all_v6_prefixes(argval_ptr, &IDSNetworkScope, dss_errno);

      case DSS_IFACE_IOCTL_GET_DOMAIN_NAME_SEARCH_LIST:
         return dss_iface_ioctl_get_domain_name_search_list(argval_ptr, &IDSNetworkScope, dss_errno);

      case DSS_IFACE_IOCTL_REFRESH_DHCP_CONFIG_INFO:
         return dss_iface_ioctl_refresh_dhcp_config_info(&IDSNetworkScope, dss_errno);

      case DSS_IFACE_IOCTL_GO_ACTIVE:
         return dss_iface_ioctl_go_active(argval_ptr, pNetApp, netMonitoredScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_GO_DORMANT:
         return dss_iface_ioctl_go_dormant(argval_ptr, pNetApp, netMonitoredScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_GO_NULL:
         return dss_iface_ioctl_go_null(argval_ptr, &IDSNetworkScope, dss_errno);

      case DSS_IFACE_IOCTL_IPCP_DNS_OPT:
         return dss_iface_ioctl_ipcp_dns_opt(argval_ptr, &IDSNetworkPrivScope, dss_errno);

      case DSS_IFACE_IOCTL_GET_TX_QUEUE_LEVEL:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno     = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_get_tx_queue_level(argval_ptr, netActiveScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_GET_PDN_TEARDOWN_CONFIG:
        if (NULL == netActiveScope.Fetch()) {
          dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
          *dss_errno = DS_ENETNONET;
          goto bail;
        }
        return dss_iface_ioctl_get_pdn_teardown_config(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_SET_PDN_TEARDOWN_CONFIG:
        if (NULL == netActiveScope.Fetch()) {
          dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
          *dss_errno = DS_ENETNONET;
          goto bail;
        }
        return dss_iface_ioctl_set_pdn_teardown_config(argval_ptr, netMonitoredScope.Fetch(), dss_errno);
        

      // All 707 ioctls fall through to a common handler.
      case DSS_IFACE_IOCTL_707_GET_MDR:
      case DSS_IFACE_IOCTL_707_SET_MDR:
      case DSS_IFACE_IOCTL_707_GET_DORM_TIMER:
      case DSS_IFACE_IOCTL_707_SET_DORM_TIMER:
      case DSS_IFACE_IOCTL_707_GET_RLP_ALL_CURR_NAK:
      case DSS_IFACE_IOCTL_707_SET_RLP_ALL_CURR_NAK:
      case DSS_IFACE_IOCTL_707_GET_RLP_ALL_DEF_NAK:
      case DSS_IFACE_IOCTL_707_SET_RLP_ALL_DEF_NAK:
      case DSS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK:
      case DSS_IFACE_IOCTL_707_GET_RLP_QOS_NA_PRI:
      case DSS_IFACE_IOCTL_707_SET_RLP_QOS_NA_PRI:
      case DSS_IFACE_IOCTL_707_SDB_SUPPORT_QUERY:
      case DSS_IFACE_IOCTL_707_ENABLE_HOLDDOWN:
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_HPT_MODE:
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INTERIA:
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_SLOTTED_MODE:
      case DSS_IFACE_IOCTL_707_HDR_GET_RMAC3_INFO:
      case DSS_IFACE_IOCTL_707_GET_TX_STATUS:
      case DSS_IFACE_IOCTL_GET_SESSION_TIMER:
      case DSS_IFACE_IOCTL_SET_SESSION_TIMER:
      case DSS_IFACE_IOCTL_707_GET_INACTIVITY_TIMER:
      case DSS_IFACE_IOCTL_707_SET_INACTIVITY_TIMER:
      case DSS_IFACE_IOCTL_707_SET_HYSTERESIS_ACT_TIMER:
      case DSS_IFACE_IOCTL_707_GET_HYSTERESIS_ACT_TIMER:
         return IfaceIoctl707(netActiveScope.Fetch(), netMonitoredScope.Fetch(), ioctl_name, argval_ptr, dss_errno, iface_id);
      // Handling of 707IOCTL's we wish to check for netActive
      case DSS_IFACE_IOCTL_707_GET_SIP_MIP_DATA_CALL_TYPE:
        if (NULL == netActiveScope.Fetch()) {
          dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
          *dss_errno     = DS_ENETNONET;
          goto bail;
        }
        return IfaceIoctl707(netActiveScope.Fetch(), netMonitoredScope.Fetch(), ioctl_name, argval_ptr, dss_errno, iface_id);

      case DSS_IFACE_IOCTL_QOS_REQUEST:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno     = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_qos_request(argval_ptr, netActiveScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_QOS_RELEASE:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno     = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_qos_release(argval_ptr, netActiveScope.Fetch(), dss_errno);

      // DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC == DSS_IFACE_IOCTL_QOS_GET_FLOW_SPEC
      case DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno     = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_qos_get_granted_flow_spec(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_qos_get_granted_flow_spec2(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_QOS_GET_GRANTED_FILTER_SPEC:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_qos_get_granted_filter_spec(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_QOS_RESUME:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_qos_resume(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_QOS_MODIFY:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno     = DS_ENETNONET;
            return DSS_ERROR;
         }
         return dss_iface_ioctl_qos_modify(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_QOS_SUSPEND:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_qos_suspend(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_QOS_GET_STATUS:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_qos_get_status(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_QOS_REQUEST_EX:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_qos_request_ex(argval_ptr, netActiveScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_QOS_RELEASE_EX:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_qos_release_ex(argval_ptr, netActiveScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_QOS_SUSPEND_EX:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_qos_suspend_ex(argval_ptr, netActiveScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_QOS_RESUME_EX:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_qos_resume_ex(argval_ptr, netActiveScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_PRIMARY_QOS_MODIFY:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_primary_qos_modify(argval_ptr,netActiveScope.Fetch(),dss_errno,iface_id);

      case DSS_IFACE_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_primary_qos_get_granted_flow_spec(argval_ptr, netActiveScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_ON_QOS_AWARE_SYSTEM:
         return dss_iface_ioctl_on_qos_aware_system(argval_ptr, &IDSNetworkExtScope, dss_errno);

      case DSS_IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES:
         return dss_iface_ioctl_get_network_supported_qos_profiles(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_QOS_NET_INITIATED_REQUEST:
         return dss_iface_ioctl_qos_net_initiated_request(argval_ptr, netActiveScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_QOS_NET_INITIATED_RELEASE:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno     = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_qos_net_initiated_release(argval_ptr, netActiveScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_707_GET_HDR_1X_HANDDOWN_OPTION:
         return dss_iface_ioctl_707_get_hdr_1x_handdown_option(argval_ptr, &IDSNetworkScope, dss_errno);

      case DSS_IFACE_IOCTL_707_SET_HDR_1X_HANDDOWN_OPTION:
         return dss_iface_ioctl_707_set_hdr_1x_handdown_option(argval_ptr, &IDSNetworkScope, dss_errno);
#ifdef FEATUTE_DATA_PS_MCAST
      case DSS_IFACE_IOCTL_MCAST_JOIN:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno     = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_mcast_join(argval_ptr, netActiveScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_MCAST_JOIN_EX:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_mcast_join_ex(argval_ptr, netActiveScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_MCAST_LEAVE:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_EBADF;
            goto bail;
         }
         return dss_iface_ioctl_mcast_leave(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_MCAST_LEAVE_EX:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_mcast_leave_ex(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_MCAST_REGISTER_EX:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_mcast_register_ex(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_MBMS_MCAST_CONTEXT_ACTIVATE:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_mbms_mcast_context_activate(argval_ptr, netActiveScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_MBMS_MCAST_CONTEXT_DEACTIVATE:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_mbms_mcast_context_deactivate(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_BCMCS_ENABLE_HANDOFF_REG:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_bcmcs_enable_handoff_reg(argval_ptr, netActiveScope.Fetch(), dss_errno);
#endif // FEATUTE_DATA_PS_MCAST
      case DSS_IFACE_IOCTL_MT_REG_CB:
         return dss_iface_ioctl_mt_reg_cb(argval_ptr, netMonitoredScope.Fetch(), dss_errno, iface_id);

      case DSS_IFACE_IOCTL_MT_DEREG_CB:
         return dss_iface_ioctl_mt_dereg_cb(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_DORMANCY_INFO_CODE:
         return dss_iface_ioctl_get_dormancy_info_code(argval_ptr, &IDSNetworkScope, dss_errno);

      case DSS_IFACE_IOCTL_QOS_GET_MODE:
         return dss_iface_ioctl_qos_get_mode(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_UMTS_GET_IM_CN_FLAG:
         return dss_iface_ioctl_umts_get_im_cn_flag(argval_ptr, &IDSNetworkScope, dss_errno);

      case DSS_IFACE_IOCTL_ENABLE_FIREWALL:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno     = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_enable_firewall(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_DISABLE_FIREWALL:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_disable_firewall(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_ADD_FIREWALL_RULE:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_add_firewall_rule(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_DELETE_FIREWALL_RULE:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_delete_firewall_rule(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_FIREWALL_RULE:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
         return dss_iface_ioctl_get_firewall_rule(argval_ptr, netActiveScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_FIREWALL_TABLE:
         if (NULL == netActiveScope.Fetch()) {
            dss_error_code = DSS_ERROR_IOCTL_FAILED_NO_ACTIVE_NETWORK;
            *dss_errno = DS_ENETNONET;
            goto bail;
         }
        return dss_iface_ioctl_get_firewall_table(argval_ptr, netActiveScope.Fetch(), dss_errno);
      case DSS_IFACE_IOCTL_GET_IFACE_STATS:
        return dss_iface_ioctl_get_iface_stats(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_RESET_IFACE_STATS:
        return dss_iface_ioctl_reset_iface_stats(argval_ptr, netMonitoredScope.Fetch(), dss_errno);
      case DSS_IFACE_IOCTL_UW_FMC_SET_TUNNEL_PARAMS:
        return dss_iface_ioctl_set_fmc_tunnel_params(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_UW_FMC_RESET_TUNNEL_PARAMS:
        return dss_iface_ioctl_reset_fmc_tunnel_params(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_DEVICE_NAME:
        return dss_iface_ioctl_get_device_name(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_GATEWAY_ADDR:
        return dss_iface_ioctl_get_gateway_addr(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_GET_SUBNET_MASK:
        return dss_iface_ioctl_get_subnet_mask(argval_ptr, netMonitoredScope.Fetch(), dss_errno);

      case DSS_IFACE_IOCTL_FILTER_MATCH_REQUEST:
      {
  	if (NULL == netActiveScope.Fetch()) {
            LOG_MSG_ERROR_0("no active network, IOCTL failed");
            *dss_errno = DS_ENETNONET;
            return DSS_ERROR;
         }
  	return dss_iface_ioctl_filter_match_request(argval_ptr, netActiveScope.Fetch(), dss_errno, iface_id);;
      }
      default:
         LOG_MSG_ERROR_1("dss_iface_ioctl(): "
                         "unsupported ioctl=0x%x", ioctl_name);
         *dss_errno = DS_EOPNOTSUPP;
         return DSS_ERROR;
   }

bail:
  
  LOG_MSG_ERROR_2("dss_iface_ioctl(): failed"
                  "error %d, error_arg %d",
                  dss_error_code, err_arg);
  return DSS_ERROR;
}
