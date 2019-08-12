/*======================================================

FILE:  DSS_NetDSAPI.cpp

SERVICES:

GENERAL DESCRIPTION:
Implementation of Network related ds functions.

=====================================================

Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetDSAPI.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "dssocket.h"
#include "dssinternal.h"

#include "DSS_Common.h"

#include "DSS_NetApp.h"
#include "DSS_Globals.h"
#include "DSS_Conversion.h"
#include "DSS_IDSNetworkPrivScope.h"
#include "DSS_IDSNetworkScope.h"
#include "DSS_IDSNetActiveScope.h"
#include "DSS_NetActiveScope.h"
#include "DSS_NetMonitoredScope.h"
#include "DSS_IDSNetPolicyPrivScope.h"
#include "DSS_IDSNetPolicyScope.h"
#include "DSS_MemoryManagement.h"

#include "ds_Net_INetworkPriv.h"
#include "ds_Addr_Def.h"

#include "ds_Net_CNetworkFactory.h"

#include "ds_Net_IIPFilterManagerPriv.h"
#include "ps_policy_mgr.h"
#include "ds_Net_CreateInstance.h"
#include "dss_errors_def.h"


#if ( defined(FEATURE_DATACOMMON_PACKAGE_SINGLE_PROC) || defined(FEATURE_DATACOMMON_2H09_2_SINGLE_PROC_MDM) )
extern "C" {
#include "ps_policyi_mgr.h"
}
#endif

#define DSS_NETPOLICY_COOKIE (0x12343210L)

using namespace ds::Net;
using namespace dss::Error;

int dss_reg_ip_filter_common
(
  sint15                     sockfd,
  dss_iface_type             iface,
  ipfltr_type_enum_type      filter_type,
  uint8                      num_filters,
  void                       *filters,
  uint32                     *filterHandle,
  sint15                     *dss_errno
)
{
  ip_filter_type* ipfltr_ptr;
  int loop_var = 0;
  sint15 ret = 0;
  IIPFilterRegPriv* filterReg = NULL;
  IIPFilterManagerPriv *pDSNetIPFilterMgr = NULL;
  INetwork* pIDSNetwork = NULL;
  INetworkPriv* pIDSNetworkPriv = NULL;
  INetworkFactory* pIDSNetworkFactory = NULL;
  DSSIDSNetPolicyPrivScope IDSNetPolicyPrivScope;
  AEEResult res = AEE_SUCCESS;
  dss_iface_id_type tmpIfaceId = INVALID_IFACE_ID;
  DSSGenScope scopeNetworkFactory;
  DSSGenScope scopeNetworkFactoryPriv;
  ds::Net::IfaceGroupType ifaceGroup; // used in group filters
  IIPFilterPriv** ppFilters = NULL;
  dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

  LOG_MSG_FUNCTION_ENTRY_2("dss_reg_ip_filter_common(): socket %d, filter_type %d",
    sockfd, filter_type);

  *dss_errno = DSS_SUCCESS;

  if (IPFLTR_MAX_TYPE <= filter_type)
  {
    *dss_errno = DS_EFAULT;
    LOG_MSG_ERROR_1("dss_reg_ip_filter_common(): Bad arg - filter_type %d",
      filter_type);
    return DSS_ERROR;
  }

  // Allocate the memory for the filters pointers.
  PS_SYSTEM_HEAP_MEM_ALLOC(ppFilters, sizeof(IIPFilterPriv*)*num_filters, IIPFilterPriv**);
  if (NULL == ppFilters) {
    *dss_errno = DS_ENOMEM;
    return DSS_ERROR;
  }
  memset(ppFilters, 0, sizeof(IIPFilterPriv*) * num_filters);

  // Pass over all the filters and convert them to the new API filters in order
  // to set the filters on the corresponding IDSNetwork object.

  // Get the NetworkFactoryPriv from DSSGlobals
  res = DSSGlobals::Instance()->GetNetworkFactory(&pIDSNetworkFactory);
  if (AEE_SUCCESS != res) {
    dss_error_code = DSS_ERROR_DSNET_NETWORK_FACTORY_CREATION_FAILED;
    *dss_errno = DSSConversion::IDS2DSErrorCode(res);
    ret = DSS_ERROR;
    goto bail;
  }
  scopeNetworkFactoryPriv.SetParams(pIDSNetworkFactory, DSSGenScope::IDSIQI_TYPE);

  for (loop_var = 0; loop_var < num_filters; loop_var++)
  {
    ipfltr_ptr = (((ip_filter_type*)filters) + loop_var);

    // Create IDSNetFilter object.
    res = pIDSNetworkFactory->CreateIPFilterSpec(&ppFilters[loop_var]);
    if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_CREATE_IPFILTERSPEC_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      ret = DSS_ERROR;
      goto bail;
    }

    res = DSSConversion::DS2IDSIPFilter(ipfltr_ptr, ppFilters[loop_var]);
    if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_DS2IDSIPFILTER_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      ret = DSS_ERROR;
      goto bail;
    }
  }

  // Create IDSNetPolicyPriv with the provided iface_id
  res = IDSNetPolicyPrivScope.Init();
  if (AEE_SUCCESS != res) {
    dss_error_code = DSS_ERROR_NET_POLICY_CREATION_FAILED;
    *dss_errno = DSSConversion::IDS2DSErrorCode(res);
    ret = DSS_ERROR;
    goto bail;
  }

  if (DSS_IFACE_ID == iface.kind ) {
    tmpIfaceId = DSSIfaceId::DSSIfaceId2DSNetIfaceId(iface.info.id);
    res = IDSNetPolicyPrivScope.Fetch()->SetIfaceId(static_cast<ds::Net::IfaceIdType>(tmpIfaceId));
    if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_SET_IFACEID_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      ret = DSS_ERROR;
      goto bail;
    }

    res = IDSNetPolicyPrivScope.Fetch()->SetRoutable(TRUE);
    if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_SET_ROUTABLE_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      ret = DSS_ERROR;
      goto bail;
    }
  res = pIDSNetworkFactory->CreateNetwork(ds::Net::NetworkMode::QDS_MONITORED,
    IDSNetPolicyPrivScope.Fetch(),
    &pIDSNetwork);
  if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_CREATE_NETWORKPRIV_FAILED;
    *dss_errno = DSSConversion::IDS2DSErrorCode(res);
    ret = DSS_ERROR;
    goto bail;
  }

    res = pIDSNetwork->QueryInterface(AEEIID_INetworkPriv, (void**)&pIDSNetworkPriv);
    if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_QUERY_INTERFACE_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      ret = DSS_ERROR;
      goto bail;
    }

    //Perform LookupInterface() to set the iface id in the network object.
    res = pIDSNetworkPriv->LookupInterface();
    if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_LOOKUP_INTERFACE_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      ret = DSS_ERROR;
      goto bail;
    }

    res = pIDSNetwork->QueryInterface(AEEIID_IIPFilterManagerPriv, (void**)&pDSNetIPFilterMgr);
    if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_QUERY_INTERFACE_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      ret = DSS_ERROR;
      goto bail;
    }

    res = pDSNetIPFilterMgr->RegisterFilters((int)sockfd, ppFilters, num_filters, &filterReg, DSS_IFACE_ANY_DEFAULT);
    if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_REGISTER_FILTERS_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      ret = DSS_ERROR;
      goto bail;
    }

    // Need to save the filter in the DSSGlobals in order to know when to release it.
    ret = DSSGlobals::Instance()->AddFilterRegObjectToList(filterReg, iface.info.id, sockfd);
  }
  else {
    res = pIDSNetworkFactory->CreateNetwork(ds::Net::NetworkMode::QDS_MONITORED,
      IDSNetPolicyPrivScope.Fetch(),
      &pIDSNetwork);
    if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_CREATE_NETWORKPRIV_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      ret = DSS_ERROR;
      goto bail;
    }

    res = pIDSNetwork->QueryInterface(AEEIID_IIPFilterManagerPriv, (void**)&pDSNetIPFilterMgr);
    if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_QUERY_INTERFACE_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      ret = DSS_ERROR;
      goto bail;
    }

    res = DSSConversion::DS2IDSIfaceGroup(iface.info.name, &ifaceGroup);
    if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_DS2IDSIFACEGROUP_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      ret = DSS_ERROR;
      goto bail;
    }

    res = pDSNetIPFilterMgr->RegisterFilters((int)sockfd, ppFilters, num_filters, &filterReg, ifaceGroup);
  if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_REGISTER_FILTERS_FAILED;
    *dss_errno = DSSConversion::IDS2DSErrorCode(res);
    ret = DSS_ERROR;
    goto bail;
  }

    // Need to save the filter in the DSSGlobals in order to know when to release it.
    ret = DSSGlobals::Instance()->AddFilterRegObjectToList(filterReg, DSS_IFACE_INVALID_ID, sockfd);

  }

  *filterHandle = (uint32)filterReg;

bail:
  // Release the objects.
  for (loop_var = 0; loop_var < num_filters; loop_var++)
  {
    if (ppFilters[loop_var]) {
      DSSCommon::ReleaseIf((IQI**)&ppFilters[loop_var]);
    }
  }

  PS_SYSTEM_HEAP_MEM_FREE (ppFilters);

  DSSCommon::ReleaseIf((IQI**)&pIDSNetwork);
  DSSCommon::ReleaseIf((IQI**)&pIDSNetworkPriv);
  DSSCommon::ReleaseIf((IQI**)&pDSNetIPFilterMgr);
  DSSCommon::ReleaseIf((IQI**)&filterReg);

  LOG_MSG_FUNCTION_EXIT_5( "dss_reg_ip_filter_common(): "
    "ret %d, dss_errno %d, socket %d, error %d, res %d",
    ret, *dss_errno, sockfd, dss_error_code, res);
  return ret;
}

int dss_reg_ip_filter_ex
(
  sint15                     sockfd,
  dss_iface_type             iface,
  ipfltr_type_enum_type      filter_type,
  uint8                      num_filters,
  void                       *filters,
  uint32                     *filterHandle,
  sint15                     *dss_errno
)
{
  sint15 ret = 0;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

  LOG_MSG_FUNCTION_ENTRY_4("dss_reg_ip_filter_ex(): "
    "socket %d, iface type %d, filter_type %d, caller 0x%p",
    sockfd, iface.kind, filter_type, caller_ptr);

  if (NULL == dss_errno) {
    LOG_MSG_ERROR_0("dss_reg_ip_filter_ex(): dss_errno is NULL");
    return DSS_ERROR;
  }

  DSS_DS_API_CALLED_RET_ERRNO();

  if (NULL == filters || 0 == num_filters)
  {
    *dss_errno = DS_EFAULT;
    LOG_MSG_ERROR_2("dss_reg_ip_filter_ex(): Bad arg - filters 0x%p, num_filters %d",
      filters, num_filters);
    return DSS_ERROR;
  }

  if (IPFLTR_MAX_TYPE <= filter_type)
  {
    *dss_errno = DS_EFAULT;
    LOG_MSG_ERROR_1("dss_reg_ip_filter_ex(): Bad arg - filter_type %d",
      filter_type);
    return DSS_ERROR;
    }

  if ((DSS_IFACE_ID == iface.kind) &&
      (DSS_IFACE_INVALID_ID == iface.info.id)) {
    *dss_errno = DS_EBADF;
    LOG_MSG_ERROR_1("dss_reg_ip_filter_ex(): Bad iface_id 0x%x", iface.info.id);
    return DSS_ERROR;
  }

  if ((DSS_IFACE_NAME == iface.kind) &&
      (iface.info.name < DSS_IFACE_ANY_DEFAULT || iface.info.name > DSS_IFACE_ANY )){
    *dss_errno = DS_EFAULT;
    LOG_MSG_ERROR_1("dss_reg_ip_filter_ex(): incorrect iface group 0x%x",
      iface.info.name);
    return DSS_ERROR;
  }

  ret = dss_reg_ip_filter_common(sockfd, iface, filter_type, num_filters, filters, filterHandle, dss_errno);

  LOG_MSG_FUNCTION_EXIT_3( "dss_reg_ip_filter_ex(): "
    "ret %d, dss_errno %d, socket %d",
    ret, *dss_errno, sockfd);
  return ret;
}

int dss_dereg_ip_filter_by_handle
(
  uint32                    filter_handle,
  int16                   * dss_errno
)
{
   sint15 ret = 0;
   AEEResult res = AEE_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_2("dss_dereg_ip_filter_by_handle(): handle 0x%x errno 0x%x", filter_handle, dss_errno);

   if (NULL == dss_errno) {
     return DSS_ERROR;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->RemoveFilterRegObjectFromListByHandle(filter_handle);
   if (AEE_SUCCESS != res) {
     LOG_MSG_ERROR_2("dss_dereg_ip_filter_by_handle(): "
       "Failed to remove filter Reg object from list, filter_handle 0x%x, res %d",
       filter_handle, res);
   }

   LOG_MSG_FUNCTION_EXIT_2( "dss_dereg_ip_filter_by_handle(): "
      "ret %d, dss_errno %d", ret, *dss_errno);
   return ret;
}

sint15 dss_get_app_net_policy
(
  sint15 appid,                                          /* Application id */
  dss_net_policy_info_type * policy_info_ptr,     /* policy info structure */
  sint15 * dss_errno                                       /* error number */
)
{
   sint15 sRet = DSS_SUCCESS;

   DSSNetApp *pNetApp = NULL;
   DSSIDSNetPolicyScope IDSNetPolicyScope;

   LOG_MSG_FUNCTION_ENTRY_3("dss_get_app_net_policy(): app_id %d, policy_ptr 0x%p errno 0x%x",
                            appid, policy_info_ptr, dss_errno);

   if (NULL == dss_errno) {
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   IDS_ERR_RET_ERRNO(DSSGlobals::Instance()->GetNetApp(appid, &pNetApp));
   IDS_ERR_RET_ERRNO(IDSNetPolicyScope.Init(pNetApp));
   IDS_ERR_RET_ERRNO(DSSConversion::IDS2DSNetPolicy(IDSNetPolicyScope.Fetch(), policy_info_ptr, appid));
   IDS_ERR_RET_ERRNO(pNetApp->GetLegacyPolicy(policy_info_ptr));
   sRet = DSS_SUCCESS;

bail:
   LOG_MSG_FUNCTION_EXIT_1( "dss_get_app_net_policy(): "
                            "ret 0x%x", sRet);
   return sRet;
}

void dss_init_net_policy_info
(
  dss_net_policy_info_type * policy_info_ptr       /* policy info structure */
)
{
   if(policy_info_ptr != NULL)
   {
      policy_info_ptr->iface.kind = DSS_IFACE_NAME;
      policy_info_ptr->iface.info.name = DSS_IFACE_ANY_DEFAULT;
      policy_info_ptr->policy_flag = DSS_IFACE_POLICY_ANY;
      policy_info_ptr->ipsec_disabled = FALSE;
      policy_info_ptr->is_routeable = FALSE;
      policy_info_ptr->family = DSS_AF_INET;

#if (defined (FEATURE_DS_MOBILE_IP) && defined (FEATURE_DATA_PS_MIP_CCOA))
      policy_info_ptr->sip_iface_reqd = FALSE;
#endif

      policy_info_ptr->app_identifier = PS_POLICY_MGR_APP_DONT_CARE;
      policy_info_ptr->umts.pdp_profile_num = 0;
      policy_info_ptr->umts.im_cn_flag = FALSE;
      policy_info_ptr->umts.apn.length = 0;
      policy_info_ptr->umts.apn.name = NULL;
      policy_info_ptr->cdma.data_session_profile_id = 0;
      policy_info_ptr->dss_netpolicy_private.cookie = DSS_NETPOLICY_COOKIE;
      policy_info_ptr->subs_id = 0;
      policy_info_ptr->apn_type = DSS_APN_TYPE_UNSPECIFIED;
      LOG_MSG_INFO1_1("dss_init_net_policy_info(): "
                      "Set policy info ptr to default policy,  policy_info_ptr=0x%p",
                      policy_info_ptr);
   }
   else
   {
      LOG_MSG_ERROR_0("dss_init_net_policy_info(): "
                      "policy_info_ptr is NULL");
   }

} /* dss_init_net_policy_info() */

sint15 dss_netstatus
(
  sint15 app_id,                                         /* application ID */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSNetApp *pNetApp = NULL;
   DSSNetActiveScope netActiveScope;
   NetworkStateType netState = NetworkState::QDS_CLOSED;
   AEEResult res = AEE_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_2("dss_netstatus(): app_id %d errno 0x%x", app_id, dss_errno);

   // dss_netstatus() returns DSS_ERROR + relevant errno.
   // This is not always an error.

   if (NULL == dss_errno) {
      return AEE_EFAILED;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   IDS_ERR_RET_ERRNO(DSSGlobals::Instance()->GetNetApp(app_id, &pNetApp));
   
   res = netActiveScope.Init(pNetApp);
   if (AEE_SUCCESS != res) {
      *dss_errno = DS_ENETNONET;
      goto bail;
   }

   IDS_ERR_RET_ERRNO(netActiveScope.Fetch()->GetNetworkState(&netState));

   switch (netState) {
      case NetworkState::QDS_OPEN_IN_PROGRESS:
         *dss_errno = DS_ENETINPROGRESS;
         break;

      case NetworkState::QDS_OPEN:
         *dss_errno = DS_ENETISCONN;
         break;

      case NetworkState::QDS_CLOSE_IN_PROGRESS:
         *dss_errno = DS_ENETCLOSEINPROGRESS;
         break;

      case NetworkState::QDS_CLOSED:
      case NetworkState::QDS_LINGERING:
         *dss_errno = DS_ENETNONET;
         break;

      default:
         *dss_errno = DS_ENETDOWN; // TODO: revisit this errno
         goto bail;
   }

bail:
   res = DSS_ERROR; // in order to inform app to read relevant errno
   LOG_MSG_FUNCTION_EXIT_2("dss_netstatus(): "
                           "ret %d, dss_errno %d", res, *dss_errno);
   return res;
}

sint15 dss_pppclose
(
  sint15 app_id,                                         /* application id */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSNetApp *pNetApp = NULL;
   boolean networkIsUp = FALSE;
   sint15 ret = DSS_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_2("dss_pppclose(): app_id %d errno 0x%x", app_id, dss_errno);

   if (NULL == dss_errno) {
      ret = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   *dss_errno = DSS_SUCCESS;

   IDS_ERR_RET_ERRNO(DSSGlobals::Instance()->GetNetApp(app_id, &pNetApp));
   pNetApp->ResetLastIfaceStateSentToApp();
   pNetApp->GetNetworkIsUp(&networkIsUp);
   if (!networkIsUp) {
      // If (NetworkState != UP || NetworkState != COMING_UP || NetworkState != GOING_DOWN) we can just return DSS_SUCCESS
      goto bail;
   }
   IDS_ERR_RET_ERRNO(pNetApp->StopNetActive());

   // Synchronized pppclose: If Stop returns AEE_SUCCESS, it means that the network is already
   // down and we don't need to wait for event

bail:
   LOG_MSG_FUNCTION_EXIT_1( "dss_pppclose(): "
                            "ret %d", ret);
   return ret;
}

sint15 dss_pppopen
(
   sint15 app_id,                                         /* application id */
   sint15 *dss_errno                               /* error condition value */
)
{
   DSSNetApp *pNetApp = NULL;
   DSSIDSNetworkPrivScope IDSNetworkPrivScope;
   AEEResult res = AEE_SUCCESS;
   sint15 ret = DSS_SUCCESS;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

   LOG_MSG_FUNCTION_ENTRY_2("dss_pppopen(): "
     "app_id %d, caller 0x%p", app_id, caller_ptr);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      ret = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetNetApp(app_id, &pNetApp);
   if ((AEE_SUCCESS != res) || (NULL == pNetApp)) {
      *dss_errno = DS_EBADAPP;
      dss_error_code = DSS_ERROR_GET_NETAPP_FAILED;
      ret = DSS_ERROR;
      goto bail;
   }

   IDS_ERR_RET_ERRNO(pNetApp->PPPOpen());

   ret = DSS_SUCCESS;
bail:
   LOG_MSG_FUNCTION_EXIT_4( "dss_pppopen(): "
       "ret %d, error %d, res %d pNetApp = 0x%p", ret, dss_error_code, res, pNetApp);
   return ret;
}

sint15 dss_set_app_net_policy
(
   sint15 appid,                                          /* Application id */
   dss_net_policy_info_type * policy_info_ptr,     /* policy info structure */
   sint15 * dss_errno                                       /* error number */
)
{
   DSSNetApp *pNetApp = NULL;
   DSSNetMonitoredScope netMonitoredScope;
   DSSIDSNetPolicyPrivScope IDSNetPolicyPrivScope;
   boolean networkIsUp = FALSE;

   LOG_MSG_FUNCTION_ENTRY_3("dss_set_app_net_policy(): app_id %d, poicy_ptr 0x%p dss_errno 0x%x",
                            appid, policy_info_ptr, dss_errno);

   if (NULL == dss_errno) {
      return DSS_ERROR;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   IDS_ERR_RET_ERRNO(DSSGlobals::Instance()->GetNetApp(appid, &pNetApp));

   // Verify that the network hasn't already brought up yet.
   // Policy cannot be changed if the app has
   // already brought up the network (because in such case the Network object is already
   // bound to an iface and policy change would not take effect.
   pNetApp->GetNetworkIsUp(&networkIsUp);
   if (TRUE == networkIsUp) {
      LOG_MSG_ERROR_0("dss_set_app_net_policy(): "
                      "Cannot set policy cause network was already brought up.");
      *dss_errno = DS_EINVAL;
      return DSS_ERROR;
   }
   
   IDS_ERR_RET_ERRNO(netMonitoredScope.Init(pNetApp));

   //Verify that the call is not registered for a Mobile Terminated event.
   boolean bMTPDIsReg;
   IDS_ERR_RET_ERRNO(netMonitoredScope.Fetch()->GetMTPDIsRegistered(&bMTPDIsReg));

   if(TRUE == bMTPDIsReg)
   {
      LOG_MSG_ERROR_0("dss_set_app_net_policy(): "
                      "Cannot set policy while registered for MTPD call.");
      *dss_errno = DS_EOPNOTSUPP;
      return DSS_ERROR;
   }

   IDS_ERR_RET_ERRNO(IDSNetPolicyPrivScope.Init());
   IDS_ERR_RET_ERRNO(DSSConversion::DS2IDSNetPolicy(policy_info_ptr, IDSNetPolicyPrivScope.Fetch()));
   IDS_ERR_RET_ERRNO(pNetApp->SetPolicy(IDSNetPolicyPrivScope.Fetch()));
   IDS_ERR_RET_ERRNO(pNetApp->SetLegacyPolicy(policy_info_ptr));

   return DSS_SUCCESS;
}

sint15
dss_last_netdownreason
(
   sint15                      appid,                /* Application id      */
   dss_net_down_reason_type  * reason,               /* network down reason */
   sint15                    * dss_errno             /* error number        */
 )
{
   DSSNetApp *pNetApp = NULL;
   DSSNetActiveScope netActiveScope;
   DSSIDSNetworkScope IDSNetworkScope;
   NetDownReasonType netdownreason;
   AEEResult res = AEE_SUCCESS;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_2("dss_last_netdownreason(): "
     "app_id %d, caller 0x%p",
     appid, caller_ptr);

   if (NULL == dss_errno) {
      LOG_MSG_ERROR_0("dss_last_netdownreason(): "
                      "dss_errno is NULL");
      return DSS_ERROR;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   IDS_ERR_RET_ERRNO(DSSGlobals::Instance()->GetNetApp(appid, &pNetApp));

   // first try to get the reason from ds::Net::NetworkActive object
   res = netActiveScope.Init(pNetApp);
   if (AEE_SUCCESS == res) {
      res = IDSNetworkScope.Init(netActiveScope.Fetch());
      if (AEE_SUCCESS == res) {
         IDS_ERR_RET_ERRNO(IDSNetworkScope.Fetch()->GetLastNetDownReason(&netdownreason));
         IDS_ERR_RET_ERRNO(DSSConversion::IDS2DSNetworkDownReason(netdownreason, reason));
         return DSS_SUCCESS;
      }
   }
   else {
      LOG_MSG_INFO2_2("dss_last_netdownreason(): "
                      "Failed to fetch NetActive from NetApp object. res=%d pNetApp=0x%p",
                      res, pNetApp);
   }
  
   // if ds::Net::NetworkActive object is NULL, use the DSS cached value
   if (QDS_EINVAL == res) {
      pNetApp->GetNetDownReason(reason);
   }
   else {
      // there is an error in this call
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      LOG_MSG_ERROR_1("dss_last_netdownreason(): "
                      "dss_errno %d", *dss_errno);
      return DSS_ERROR;
   }

   return DSS_SUCCESS;
}

dss_iface_id_type dss_get_iface_id(sint15  appId)
{
   DSSNetApp *pNetApp = NULL;
   DSSNetMonitoredScope netMonitoredScope;
   DSSIDSNetworkScope IDSNetworkScope;
   DSSIDSNetworkPrivScope IDSNetworkPrivScope;
   dss_iface_id_type ifaceId = DSS_IFACE_INVALID_ID;
   AEEResult res = AEE_SUCCESS;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_2("dss_get_iface_id(): "
     "appId %d, caller 0x%p", appId, caller_ptr);

   DSS_DS_API_CALLED_RET_PARAM(DSS_IFACE_INVALID_ID);

   res = DSSGlobals::Instance()->GetNetApp(appId, &pNetApp);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_GET_NETAPP_FAILED;
      goto bail;
   }

   // prepare primeryNetMonitored object pointer
   res = netMonitoredScope.Init(pNetApp);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_NETMONITORED_INIT_FAILED;
      goto bail;
   }

   // setting primeryNetMonitored object
   // and prepare ds::Net::INetworkPriv*
   // in order to perform LookupInterface()
   res = IDSNetworkPrivScope.Init(netMonitoredScope.Fetch());
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_IDSNETWORKPRIVSCOPE_INIT_FAILED;
      goto bail;
   }

   // perform LookupInterface()
   IDSNetworkPrivScope.Fetch()->LookupInterface();
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_LOOKUP_INTERFACE_FAILED;
      goto bail;
   }

   // prepare ds::Net::INetwork*
   res = IDSNetworkScope.Init(netMonitoredScope.Fetch());
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_IDSNETWORKSCOPE_INIT_FAILED;
      goto bail;
   }

   res = DSSIfaceId::BuildIfaceId(IDSNetworkScope.Fetch(), DSSIfaceId::IFACE_ID_TYPE_CURRENT, appId, &ifaceId);
   if (AEE_SUCCESS != res) {
      ifaceId = DSS_IFACE_INVALID_ID;
      dss_error_code = DSS_ERROR_BUILDIFACEID_FAILED;
      goto bail;
   }

bail:

   LOG_MSG_FUNCTION_EXIT_3( "dss_get_iface_id(): "
                            "ifaceId 0x%x, error %d, res %d",
                            ifaceId, dss_error_code, res);
   return ifaceId;
}

// TODO: make changes to dss_get_iface_id_by_policy (don't use IDSNetwork)
dss_iface_id_type
dss_get_iface_id_by_policy
(
   dss_net_policy_info_type  net_policy_info,        /* Network policy info */
   sint15                  * dss_errno             /* error condition value */
)
{
   INetworkFactory* pIDSNetworkFactory = NULL;
   INetwork* pIDSNetwork = NULL;
   INetworkPriv* pIDSNetworkPriv = NULL;
   DSSIDSNetPolicyPrivScope IDSNetPolicyPrivScope;
   dss_iface_id_type iface_id = DSS_IFACE_INVALID_ID;
   AEEResult res = AEE_SUCCESS;
   DSSGenScope scopeNetworkFactory;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   
   LOG_MSG_FUNCTION_ENTRY_2("dss_get_iface_id_by_policy(): net_policy_info %p dss_errno 0x%x",
                            &net_policy_info, dss_errno);

   if (NULL == dss_errno) {
     return DSS_ERROR;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   iface_id = DSS_IFACE_INVALID_ID;

   res = IDSNetPolicyPrivScope.Init();
   if (AEE_SUCCESS != res) {
     *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      dss_error_code = DSS_ERROR_NET_POLICY_CREATION_FAILED;
      goto bail;
   }

   // Convert from ds to IDS network policy.
   res = DSSConversion::DS2IDSNetPolicy(&net_policy_info,
                                        IDSNetPolicyPrivScope.Fetch());
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      dss_error_code = DSS_ERROR_DS2IDSNETPOLICY_FAILED;
      goto bail;
   }

   res = DSSGlobals::Instance()->GetNetworkFactory(&pIDSNetworkFactory);
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      dss_error_code = DSS_ERROR_GET_NETWORKFACTORY_FAILED;
      goto bail;
   }
   scopeNetworkFactory.SetParams(pIDSNetworkFactory, DSSGenScope::IDSIQI_TYPE);

   res = pIDSNetworkFactory->CreateNetwork(ds::Net::NetworkMode::QDS_MONITORED,
                                           IDSNetPolicyPrivScope.Fetch(),
                                           &pIDSNetwork);

   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      dss_error_code = DSS_ERROR_CREATE_NETWORK_FAILED;
      goto bail;
   }

   res = pIDSNetwork->QueryInterface(AEEIID_INetworkPriv, (void**)&pIDSNetworkPriv);
   if(AEE_SUCCESS != res) {
     dss_error_code = DSS_ERROR_QUERY_INTERFACE_FAILED;
     goto bail;
   }

   res = pIDSNetworkPriv->LookupInterface();
   if (AEE_SUCCESS != res && AEE_EWOULDBLOCK != res)
   {
      goto bail;
   }

   res = DSSIfaceId::BuildIfaceId(pIDSNetwork, DSSIfaceId::IFACE_ID_TYPE_CURRENT, -1, &iface_id);
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      dss_error_code = DSS_ERROR_BUILDIFACEID_FAILED;
      iface_id = DSS_IFACE_INVALID_ID;
      goto bail;
   }

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSNetwork);
   DSSCommon::ReleaseIf((IQI**)&pIDSNetworkPriv);
   LOG_MSG_FUNCTION_EXIT_4( "dss_get_iface_id_by_policy(): "
                            "return iface_id 0x%x, dss_errno %d, error %d, res %d",
                            iface_id, *dss_errno, dss_error_code, res);
   return iface_id;
}

dss_iface_id_type dss_get_iface_id_by_qos_handle
(
   dss_qos_handle_type  handle     // Handle to QOS instance
)
{
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_2("dss_get_iface_id_by_qos_handle(): "
                            "qos_handle %d, caller 0x%p",
                            handle, caller_ptr);

   DSS_DS_API_CALLED_RET_PARAM(DSS_IFACE_INVALID_ID);

   return handle & 0xFFFFFF00;
}

// TODO Support of this API depends on QMI additions. Complete support here when support in lower layers is provided.
int32 dss_get_app_profile_id (uint32 app_type)
{
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_2("dss_get_app_profile_id(): "
     "app_type %d, caller 0x%p",
     app_type, caller_ptr);

   DSS_DS_API_CALLED_RET();

   // Currently this API supported by single proc targets only
#ifdef FEATURE_DATACOMMON_PACKAGE_SINGLE_PROC
   return ps_policy_mgr_get_profile((int64)app_type);
#else
   LOG_MSG_ERROR_0("dss_get_app_profile_id is not supported");
   return (DSS_ERROR);
#endif
}

/*===========================================================================
FUNCTION DSS_GET_SCOPE_ID_BY_IFACE_ID()

DESCRIPTION
  This function allows to retrieve a route_scope from the iface_id.
  Currently, for applications the notion of scope id is basically same as
  iface id as we do not support sitelocal addresses. However, applications
  need not know that scopeid and ifaceid are same as the interpretation can
  change in future when sitelocal multicast is supported.

DEPENDENCIES
  None.

PARAMETERS
  uint32  - Iface id.
  sint15* - Errno.

RETURN VALUE
  On success - Scope Id
  On failure - 0

  dss_errno Values
  ----------------
  DS_EINVAL      Invalid iface id.

SIDE EFFECTS
  None
===========================================================================*/
dss_scope_id_type dss_get_scope_id_by_iface_id
(
  dss_iface_id_type   iface_id,
  sint15 *dss_errno
)
{
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_2("dss_get_scope_id_by_iface_id(): "
                            "iface_id 0x%x, caller 0x%p",
                            iface_id, caller_ptr);

   dss_iface_id_type iface_id_internal = DSS_IFACE_INVALID_ID;

   DSS_DS_API_CALLED_RET_ERRNO_PARAM(DSS_IFACE_INVALID_ID);

   iface_id_internal = DSSIfaceId::StripAppIdFromIfaceId(iface_id);

   return iface_id_internal;

}

int dss_reg_ip_filter
(
  sint15                     sockfd,
  dss_iface_id_type          iface_id,
  ipfltr_type_enum_type      filter_type,
  uint8                      num_filters,
  void                       *filters,
  sint15                     *dss_errno
)
{
   sint15 ret = 0;

   LOG_MSG_FUNCTION_ENTRY_4("dss_reg_ip_filter(): socket %d, iface_id 0x%x, filter_type %d dss_errno 0x%x",
     sockfd, iface_id, filter_type, dss_errno);

   if (NULL == dss_errno) {
      return DSS_ERROR;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   if (NULL == filters || 0 == num_filters)
   {
      *dss_errno = DS_EFAULT;
      LOG_MSG_ERROR_2("dss_reg_ip_filter(): Bad arg - filters 0x%p, num_filters %d",
                      filters, num_filters);
     return DSS_ERROR;
   }

   if (IPFLTR_MAX_TYPE <= filter_type)
   {
      *dss_errno = DS_EFAULT;
      LOG_MSG_ERROR_1("dss_reg_ip_filter(): Bad arg - filter_type %d",
                      filter_type);
      return DSS_ERROR;
}

   if (DSS_IFACE_INVALID_ID == iface_id)
{
     *dss_errno = DS_EBADF;
     LOG_MSG_ERROR_1("dss_reg_ip_filter(): Bad iface_id 0x%x", iface_id);
    return DSS_ERROR;
  }

   uint32 filterHandle = 0;
   dss_iface_type iface;

   iface.kind = DSS_IFACE_ID;
   iface.info.id = iface_id;

   ret = dss_reg_ip_filter_common(sockfd, iface, filter_type, num_filters, filters, &filterHandle, dss_errno);

  LOG_MSG_FUNCTION_EXIT_3( "dss_reg_ip_filter(): "
    "ret %d, dss_errno %d, socket %d",
    ret, *dss_errno, sockfd);
  return ret;
}

int dss_dereg_ip_filter
(
  sint15             sockfd,
  dss_iface_id_type  iface_id,
  sint15            *dss_errno
)
{
  sint15 sRet = DSS_SUCCESS;
  AEEResult res = AEE_SUCCESS;
  dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

  LOG_MSG_FUNCTION_ENTRY_3("dss_dereg_ip_filter(): "
                           "socket %d, iface_id 0x%x, caller 0x%p",
                           sockfd, iface_id, caller_ptr);

   if (NULL == dss_errno) {
     dss_error_code = DSS_ERROR_NULL_ERRNO;
     sRet = DSS_ERROR;
     goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->RemoveFilterRegObjectFromList(iface_id, sockfd);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_REMOVE_FILTER_REG_OBJECT_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet =  DSS_ERROR;
   }

bail:
   LOG_MSG_FUNCTION_EXIT_5( "dss_dereg_ip_filter(): "
                            "ret %d, dss_errno %d, socket %d, error %d, res %d",
                            sRet, dss_errno, sockfd, dss_error_code, res);
   return sRet;
}

int dss_reg_ip_filter_by_group
(
  sint15                      sockfd,
  dss_iface_name_enum_type   iface_group,
  ipfltr_type_enum_type      filter_type,
  uint8                      num_filters,
  void                       *filters,
  sint15                     *dss_errno
)
{
   sint15 ret = 0;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_4("dss_reg_ip_filter_by_group(): "
                            "socket %d iface_group 0x%x num_filters %d, caller 0x%p",
                            sockfd, iface_group, num_filters, caller_ptr);

   if (NULL == dss_errno) {
      LOG_MSG_ERROR_0("dss_reg_ip_filter_by_group(): dss_errno is NULL");
     return DSS_ERROR;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   *dss_errno = DSS_SUCCESS;

   if (NULL == filters || 0 == num_filters)
   {
      *dss_errno = DS_EFAULT;
      LOG_MSG_ERROR_2("dss_reg_ip_filter_by_group(): Bad arg - filters 0x%p, num_filters %d",
                      filters, num_filters);
      return DSS_ERROR;
}

   if (IPFLTR_MAX_TYPE <= filter_type)
{
      *dss_errno = DS_EFAULT;
      LOG_MSG_ERROR_1("dss_reg_ip_filter_by_group(): Bad arg - filter_type %d",
                      filter_type);
      return DSS_ERROR;
   }

   if (iface_group < DSS_IFACE_ANY_DEFAULT || iface_group > DSS_IFACE_ANY )
   {
     *dss_errno = DS_EFAULT;
     LOG_MSG_ERROR_1("dss_reg_ip_filter_by_group(): incorrect iface group 0x%x",
       iface_group);
     return DSS_ERROR;
   }

   uint32 filterHandle = 0;

   dss_iface_type iface;

   iface.kind = DSS_IFACE_NAME;
   iface.info.name = iface_group;

   ret = dss_reg_ip_filter_common(sockfd, iface, filter_type, num_filters, filters, &filterHandle, dss_errno);

   LOG_MSG_FUNCTION_EXIT_3( "dss_reg_ip_filter_by_group(): "
                            "ret %d, dss_errno %d, socket %d",
                            ret, *dss_errno, sockfd);
   return ret;

}

int dss_dereg_ip_filter_by_group
(
  sint15                     sockfd, 
  sint15                  * dss_errno
)
{
  sint15 sRet = DSS_SUCCESS;
  AEEResult res = AEE_SUCCESS;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
  dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

  LOG_MSG_FUNCTION_ENTRY_2("dss_dereg_ip_filter_by_group(): "
                           "socket %d, caller 0x%p",
                           sockfd, caller_ptr);

   if (NULL == dss_errno) {
     dss_error_code = DSS_ERROR_NULL_ERRNO;
     sRet = DSS_ERROR;
     goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->RemoveFilterRegObjectFromList(sockfd);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_REMOVE_FILTER_REG_OBJECT_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet =  DSS_ERROR;
   }

bail:
   LOG_MSG_FUNCTION_EXIT_5( "dss_dereg_ip_filter_by_group(): "
                            "ret %d, dss_errno %d, socket %d, error %d, res %d", 
                            sRet, dss_errno, sockfd, dss_error_code, res);
   return sRet;
}
