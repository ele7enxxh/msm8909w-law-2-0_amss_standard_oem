/*======================================================

FILE:  DSSNetMonitored.cpp

SERVICES:

GENERAL DESCRIPTION:
Implementation of DSSNetMonitored class

=====================================================

Copyright (c) 2008 - 2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetMonitored.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-08-31 hs  Added GetSubnetMask
  2011-08-30 hs  Added GetGatewayAddress
  2011-08-24 hs  Added GetDeviceName.
  2011-07-15 rp  QShrink2 changes: MSG_* macros are removed from inline functions.
  2010-04-18 en  History added.

===========================================================================*/


//===================================================================
//   Includes and Public Data Declarations
//===================================================================

#include "datamodem_variation.h"
#include "DSS_NetMonitored.h"
#include "DSS_Globals.h"
#include "DSS_Conversion.h"
#include "DSS_Common.h"

#include "DSS_NetworkStateHandler.h"
#include "DSS_NetworkIPHandler.h"
#include "DSS_ExtendedIPConfigHandler.h"
#include "DSS_RFConditionsHandler.h"
#include "DSS_BearerTechHandler.h"
#include "DSS_BearerTechExHandler.h"
#include "DSS_OutageHandler.h"
#include "DSS_PhysLinkStateHandler.h"
#include "DSS_QoSAwareUnAwareHandler.h"
#include "DSS_PrimaryQoSModifyHandler.h"
#include "DSS_PrimaryQoSModifyStatusHandler.h"
#include "DSS_MTPDRequestHandler.h"
#include "DSS_SlottedResultHandler.h"
#include "DSS_SlottedSessionChangedHandler.h"
#include "DSS_HDRRev0RateInteriaHandler.h"
#include "DSS_QoSProfileChangedHandler.h"
#include "DSS_IPv6PrefixChangedStateHandler.h"
#include "DSS_FastDormStatusHandler.h"
#include "DSS_NetworkExtIPv6DelAddrHandler.h"
#include "DSS_IDSNetworkPrivScope.h"
#include "DSS_IDSNetworkScope.h"
#include "DSS_MemoryManagement.h"
#include "DSS_PrivIpv6Addr.h"
#include "ds_Net_IIPv6Address.h"
#include "DSS_GenScope.h"
#include "DSS_HandoffInitHandler.h"
#include "DSS_HandoffSuccessHandler.h"
#include "DSS_HandoffFailureHandler.h"
#include "DSS_ExtendedIPConfigExHandler.h"
#include "DSS_ApnParamChangedHandler.h"

#include "ds_Net_INetworkPriv.h"

#include "DSS_MCast.h"

#include "DSS_EventHandlerMCastMBMSCtrl.h"
#include "DSS_NetMCastMBMSCtrl.h"

#include "ds_Addr_Def.h"
#include "ds_Utils_CCritSect.h"
#include "ds_Utils_Atomic.h"
#include "ds_Utils_CreateInstance.h"

using namespace ds::Net;
using namespace ds::Error;
//-------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Constant / Define Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Type Declarations (typedef, struct, enum, etc.)
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Constant Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Forward Declarations
//-------------------------------------------------------------------

//===================================================================
//              Macro Definitions
//===================================================================
#define DSS_DNS_PRIMARY_ADDR_IDX   0

#define DSS_DNS_SECONDARY_ADDR_IDX 1

#ifndef min
#define min(a,b)  ((a) < (b) ? (a):(b))
#endif

//===================================================================
//            DSSNetMonitored Functions Definitions
//===================================================================

//===================================================================
//  FUNCTION:   DSSNetMonitored::DSSNetMonitored
//
//  DESCRIPTION:
//  Constructor of the DSSNetMonitored class.
//===================================================================
DSSNetMonitored::DSSNetMonitored(bool bPolicyBitIsOn):
   refCnt(1), weakRefCnt(1), mNetHandle(-1),
   mpIDSNetworkPriv(NULL), mpIDSNetwork(NULL),
   mpIDSNetworkExt(NULL), mpIDSNetworkExt2(NULL), mpDSSNetQoSDefault(NULL),
   mpDSNetQoSManager(NULL), mNext(NULL), mpNetworkStateDownHandler(NULL),
   mpNetworkStateUpHandler(NULL), mpNetworkStateComingUpHandler(NULL),
   mpNetworkStateGoingDownHandler(NULL), mpNetIPHandler(NULL),
   mpExtendedIPConfigHandler(NULL), mpRFConditionsHandler(NULL),
   mpBearerTechHandler(NULL), mpBearerTechExHandler(NULL), mpOutageHandler(NULL),
   mpPhysLinkDownHandler(NULL), mpPhysLinkUpHandler(NULL),
   mpPhysLinkComingUpHandler(NULL), mpPhysLinkGoingDownHandler(NULL),
   mpFastDormStatusHandler(NULL), mpQosAwareHandler(NULL), mpQosUnawareHandler(NULL),
   mpPrimaryQoSModifyHandler(NULL), mpMTPDRequestHandler(NULL),
   mpSlottedResultSuccessHandler(NULL), mpSlottedResultFailureHandler(NULL),
   mpSlottedSessionChangedHandler(NULL), mpQoSProfileChangedHandler(NULL),
   mpHDRRev0RateInteriaSuccessHandler(NULL), mpHDRRev0RateInteriaFailureHandler(NULL),
   mpDSSIPv6PrefixChangedStateHandler(NULL), mpNetworkExtIPv6DelAddrHandler(0),
   mpHandoffInitHandler(NULL), mpHandoffSuccessHandler(NULL), mpHandoffFailureHandler(NULL),
   mpExtendedIpConfigExHandler(NULL),
   mpApnParamChangedHandler(NULL),
   mPPPIsOpen(FALSE), mpCritSect(NULL), mPolicyBitIsOn(bPolicyBitIsOn),
    mpDSNetPhysLink(0), mpDSNetTechUMTS(0)
{
   // DSSNetMonitored::Destructor makes use of mpCritSect before Releasing it
   // So we ASSERT here in case mpCritSect fails, otherwise
   // DSSNetMonitored::Destructor will access uninitialized mpCritSect
   if (AEE_SUCCESS != DSSGlobals::Instance()->GetCritSect(&mpCritSect)) {
      DATA_ERR_FATAL("Couldn't fetch CritSect object from DSSGlobals");
   }
}

void * DSSNetMonitored::operator new (unsigned int numBytes) throw()
{
   return ps_mem_get_buf( PS_MEM_DSAL_NET_MONITORED_TYPE);
}

void DSSNetMonitored::operator delete (void *  bufPtr) throw()
{
   PS_MEM_FREE(bufPtr);
}

AEEResult DSSNetMonitored::GetIfaceIdForEventInfo(sint15 appId, dss_iface_id_type* pIfaceId)
{
   AEEResult ret = AEE_SUCCESS;
   dss_iface_id_type ifaceId = DSS_IFACE_INVALID_ID;

   if (NULL == mpIDSNetwork) {
      return QDS_EINVAL;
   }

   DSSCritScope cs(*mpCritSect);

   // get iface id
   ret = GetIfaceIdByAppId(appId, &ifaceId);
   if (AEE_SUCCESS != ret) {
      return ret;
   }

   // check if iface id is invalid
   // if iface id is invalid, get stale iface id
   if (DSS_IFACE_INVALID_ID == ifaceId) {
      LOG_MSG_INFO1_0("GetIfaceIdForEventInfo: "
                      "iface_id was cleared, providing saved iface id");

      // iface id is invalid. Get stale iface id.
      return DSSIfaceId::BuildIfaceId(mpIDSNetwork,
         DSSIfaceId::IFACE_ID_TYPE_STALE,
         appId,
         pIfaceId);
   }

   *pIfaceId = ifaceId;

   return AEE_SUCCESS;
}

AEEResult DSSNetMonitored::GetIDSNetworkPrivObject(INetworkPriv** ppIDSNetworkPriv)
{
   DSSCritScope cs(*mpCritSect);
   *ppIDSNetworkPriv = mpIDSNetworkPriv;
   if (NULL == mpIDSNetworkPriv) {
      return QDS_EINVAL;
   }
   (void)mpIDSNetworkPriv->AddRef();
   return AEE_SUCCESS;
}

AEEResult DSSNetMonitored::GetIDSNetworkObject(INetwork** ppIDSNetwork)
{
   DSSCritScope cs(*mpCritSect);
   *ppIDSNetwork = mpIDSNetwork;
   if (NULL == mpIDSNetwork) {
      return QDS_EINVAL;
   }
   (void)mpIDSNetwork->AddRef();
   return AEE_SUCCESS;
}

AEEResult DSSNetMonitored::GetIDSNetworkExtObject(INetworkExt** ppIDSNetworkExt)
{
   DSSCritScope cs(*mpCritSect);
   *ppIDSNetworkExt = mpIDSNetworkExt;
   if (NULL == mpIDSNetworkExt) {
      return QDS_EINVAL;
   }
   (void)mpIDSNetworkExt->AddRef();
   return AEE_SUCCESS;
}

AEEResult DSSNetMonitored::GetIDSNetworkExt2Object(INetworkExt2** ppIDSNetworkExt2)
{
  DSSCritScope cs(*mpCritSect);
  *ppIDSNetworkExt2 = mpIDSNetworkExt2;
  if (NULL == mpIDSNetworkExt2) {
    return QDS_EINVAL;
  }
  (void)mpIDSNetworkExt2->AddRef();
  return AEE_SUCCESS;
}

AEEResult DSSNetMonitored::SetPolicy(ds::Net::IPolicy* pIDSNetPolicy)
{
   if (NULL != mpIDSNetworkPriv)
   {
      (void)mpIDSNetworkPriv->SetPolicy(pIDSNetPolicy);
   }

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::Init
//
//  DESCRIPTION:
//  Init the DSSNetMonitored class.
//===================================================================
AEEResult DSSNetMonitored::Init(INetwork* pIDSNetwork)
{
   // TODO: implement the initialization of the rest of the fields
   INetworkPriv* pIDSNetworkPriv = NULL;
   INetworkExt* pIDSNetworkExt = NULL;
   INetworkExt2* pIDSNetworkExt2 = NULL;
   AEEResult res = AEE_SUCCESS;

   SetIDSNetworkObject(pIDSNetwork);   // DSSNetMonitored takes care to AddRef pIDSNetwork once it is set

   res = pIDSNetwork->QueryInterface(AEEIID_INetworkPriv, (void**)&pIDSNetworkPriv);
   if(AEE_SUCCESS != res){
      LOG_MSG_ERROR_1("DSSNetMonitored::Init(): "
                      "Couldn't QueryInterface for AEEIID_INetworkPriv result = %d",
                      res);
      goto bail;
   }

   SetIDSNetworkPrivObject(pIDSNetworkPriv);   // DSSNetMonitored takes care to AddRef pIDSNetworkPriv once it is set

   res = pIDSNetwork->QueryInterface(AEEIID_INetworkExt, (void**)&pIDSNetworkExt);
   if(AEE_SUCCESS != res){
      LOG_MSG_ERROR_1("DSSNetMonitored::Init(): "
                      "Couldn't QueryInterface for AEEIID_INetworkExt result = %d",
                      res);
      goto bail;
   }

   SetIDSNetworkExtObject(pIDSNetworkExt);   // DSSNetMonitored takes care to AddRef pIDSNetworkExt once it is set

   res = pIDSNetwork->QueryInterface(AEEIID_INetworkExt2, (void**)&pIDSNetworkExt2);
   if(AEE_SUCCESS != res){
      LOG_MSG_ERROR_1("DSSNetMonitored::Init(): "
                      "Couldn't QueryInterface for AEEIID_INetworkExt2 result = %d",
                      res);
     goto bail;
   }

   SetIDSNetworkExt2Object(pIDSNetworkExt2);   // DSSNetMonitored takes care to AddRef pIDSNetworkExt2 once it is set

   res = AEE_SUCCESS;

/* fall through */

bail:

   DSSCommon::ReleaseIf((IQI**)&pIDSNetworkPriv);
   DSSCommon::ReleaseIf((IQI**)&pIDSNetworkExt);
   DSSCommon::ReleaseIf((IQI**)&pIDSNetworkExt2);

   return res;
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::Destructor
//
//  DESCRIPTION:
//  Destructor of the DSSNetMonitored class.
//===================================================================
/*lint -e{1551} */
void DSSNetMonitored::Destructor() throw()
{
   PS_MEM_RELEASE(mpNetworkStateDownHandler);
   PS_MEM_RELEASE(mpNetworkStateUpHandler);
   PS_MEM_RELEASE(mpNetworkStateComingUpHandler);
   PS_MEM_RELEASE(mpNetworkStateGoingDownHandler);
   PS_MEM_RELEASE(mpNetIPHandler);
   PS_MEM_RELEASE(mpExtendedIPConfigHandler);
   PS_MEM_RELEASE(mpRFConditionsHandler);
   PS_MEM_RELEASE(mpBearerTechHandler);
   PS_MEM_RELEASE(mpBearerTechExHandler);
   PS_MEM_RELEASE(mpOutageHandler);
   PS_MEM_RELEASE(mpPhysLinkDownHandler);
   PS_MEM_RELEASE(mpPhysLinkUpHandler);
   PS_MEM_RELEASE(mpPhysLinkComingUpHandler);
   PS_MEM_RELEASE(mpPhysLinkGoingDownHandler);
   PS_MEM_RELEASE(mpFastDormStatusHandler);
   PS_MEM_RELEASE(mpQosAwareHandler);
   PS_MEM_RELEASE(mpQosUnawareHandler);
   PS_MEM_RELEASE(mpPrimaryQoSModifyHandler);
   PS_MEM_RELEASE(mpMTPDRequestHandler);
   PS_MEM_RELEASE(mpSlottedResultSuccessHandler);
   PS_MEM_RELEASE(mpSlottedResultFailureHandler);
   PS_MEM_RELEASE(mpSlottedSessionChangedHandler);
   PS_MEM_RELEASE(mpQoSProfileChangedHandler);
   PS_MEM_RELEASE(mpHDRRev0RateInteriaSuccessHandler);
   PS_MEM_RELEASE(mpHDRRev0RateInteriaFailureHandler);
   PS_MEM_RELEASE(mpDSSIPv6PrefixChangedStateHandler);
   PS_MEM_RELEASE(mpHandoffInitHandler);
   PS_MEM_RELEASE(mpHandoffSuccessHandler);
   PS_MEM_RELEASE(mpHandoffFailureHandler);
   PS_MEM_RELEASE(mpNetworkExtIPv6DelAddrHandler);
   PS_MEM_RELEASE(mpExtendedIpConfigExHandler);
   PS_MEM_RELEASE(mpApnParamChangedHandler);

   DSSCommon::ReleaseIf((IQI**)&mpDSNetQoSManager);
   PS_MEM_DELETE(mpDSSNetQoSDefault);
   DSSCommon::ReleaseIf((IQI**)&mpIDSNetworkPriv);
   DSSCommon::ReleaseIf((IQI**)&mpIDSNetwork);
   DSSCommon::ReleaseIf((IQI**)&mpIDSNetworkExt);
   DSSCommon::ReleaseIf((IQI**)&mpIDSNetworkExt2);
   DSSCommon::ReleaseIf((IQI**)&mpDSNetTechUMTS);
   DSSCommon::ReleaseIf((IQI**)&mpDSNetPhysLink);

   DSSCommon::ReleaseIf((IQI**)&mpCritSect);
}
/*lint –restore */

//===================================================================
//  FUNCTION:   DSSNetMonitored::GetIPv4Addr
//
//  DESCRIPTION:
//  Serves DSS_IFACE_IOCTL_GET_IPV4_ADDR
//===================================================================
AEEResult DSSNetMonitored::GetIPv4Addr(dss_iface_ioctl_ipv4_addr_type* pIpv4Addr)
{
   if (NULL == mpIDSNetwork) {
      return QDS_EINVAL;
   }

   ds::IPAddrType ipAddr;
   IDS_ERR_RET(mpIDSNetwork->GetIPAddr(&ipAddr));

   if (ds::AddrFamily::QDS_AF_INET != ipAddr.family) {
      return QDS_EINVAL;
   }

   return DSSConversion::IDS2DSIpAddr(&ipAddr, pIpv4Addr);
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::GetIPv6Addr
//
//  DESCRIPTION:
//  Serves DSS_IFACE_IOCTL_GET_IPV6_ADDR
//===================================================================
AEEResult DSSNetMonitored::GetIPv6Addr(dss_iface_ioctl_ipv6_addr_type* pIpv6Addr)
{
   if (NULL == mpIDSNetwork) {
      return QDS_EINVAL;
   }

   ds::IPAddrType ipAddr;
   IDS_ERR_RET(mpIDSNetwork->GetIPAddr(&ipAddr));

   if (ds::AddrFamily::QDS_AF_INET6 != ipAddr.family) {
      return QDS_EINVAL;
   }

   return DSSConversion::IDS2DSIpAddr(&ipAddr, pIpv6Addr);
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::GetIPv4PrimDnsAddr
//
//  DESCRIPTION:
//  Serves DSS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR
//===================================================================
AEEResult DSSNetMonitored::GetIPv4PrimDnsAddr(dss_iface_ioctl_ipv4_prim_dns_addr_type* pIpv4Addr)
{
   if (NULL == mpIDSNetwork) {
      return QDS_EINVAL;
   }

   ds::IPAddrType ipAddr[PS_IFACE_NUM_DNS_ADDRS] = {{0,{0}}};
   int lenReq = 0;
   int numOfAddrAvailable = 0;

   IDS_ERR_RET(mpIDSNetwork->GetDNSAddr(ipAddr, PS_IFACE_NUM_DNS_ADDRS, &lenReq));

   numOfAddrAvailable = min(PS_IFACE_NUM_DNS_ADDRS, lenReq);

   if(0 == numOfAddrAvailable){
     return QDS_EFAULT;
   }

   if (ds::AddrFamily::QDS_AF_INET != ipAddr[DSS_DNS_PRIMARY_ADDR_IDX].family) {
      return QDS_EINVAL;
   }

   return DSSConversion::IDS2DSIpAddr(&ipAddr[DSS_DNS_PRIMARY_ADDR_IDX], pIpv4Addr);
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::GetIPv6PrimDnsAddr
//
//  DESCRIPTION:
//  Serves DSS_IFACE_IOCTL_GET_IPV6_PRIM_DNS_ADDR
//===================================================================
AEEResult DSSNetMonitored::GetIPv6PrimDnsAddr(dss_iface_ioctl_ipv6_prim_dns_addr_type* pIpv6Addr)
{
   if (NULL == mpIDSNetwork) {
      return QDS_EINVAL;
   }

   ds::IPAddrType ipAddr[PS_IFACE_NUM_DNS_ADDRS] = {{0,{0}}};
   int lenReq = 0;
   int numOfAddrAvailable = 0;

   IDS_ERR_RET(mpIDSNetwork->GetDNSAddr(ipAddr, PS_IFACE_NUM_DNS_ADDRS, &lenReq));

   numOfAddrAvailable = min(PS_IFACE_NUM_DNS_ADDRS, lenReq);

   if(0 == numOfAddrAvailable){
     return QDS_EFAULT;
   }

   if (ds::AddrFamily::QDS_AF_INET6 != ipAddr[DSS_DNS_PRIMARY_ADDR_IDX].family) {
     return QDS_EINVAL;
   }

   return DSSConversion::IDS2DSIpAddr(&ipAddr[DSS_DNS_PRIMARY_ADDR_IDX], pIpv6Addr);
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::GetIPv4SecoDnsAddr
//
//  DESCRIPTION:
//  Serves DSS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR
//===================================================================
AEEResult DSSNetMonitored::GetIPv4SecoDnsAddr(dss_iface_ioctl_ipv4_seco_dns_addr_type* pIpv4Addr)
{
   if (NULL == mpIDSNetwork) {
      return QDS_EINVAL;
   }

   ds::IPAddrType ipAddr[PS_IFACE_NUM_DNS_ADDRS] =  {{0,{0}}};
   int lenReq = 0;
   int numOfAddrAvailable = 0;

   IDS_ERR_RET(mpIDSNetwork->GetDNSAddr(ipAddr, PS_IFACE_NUM_DNS_ADDRS, &lenReq));

   numOfAddrAvailable = min(PS_IFACE_NUM_DNS_ADDRS, lenReq);

   if(numOfAddrAvailable < PS_IFACE_NUM_DNS_ADDRS){
     return QDS_EFAULT;
   }

   if (ds::AddrFamily::QDS_AF_INET != ipAddr[DSS_DNS_SECONDARY_ADDR_IDX].family) {
      return QDS_EINVAL;
   }

   return DSSConversion::IDS2DSIpAddr(&ipAddr[DSS_DNS_SECONDARY_ADDR_IDX], pIpv4Addr);
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::GetIPv6SecoDnsAddr
//
//  DESCRIPTION:
//  Serves DSS_IFACE_IOCTL_GET_IPV6_SECO_DNS_ADDR
//===================================================================
AEEResult DSSNetMonitored::GetIPv6SecoDnsAddr(dss_iface_ioctl_ipv6_seco_dns_addr_type* pIpv6Addr)
{
   if (NULL == mpIDSNetwork) {
      return QDS_EINVAL;
   }

   ds::IPAddrType ipAddr[PS_IFACE_NUM_DNS_ADDRS] =  {{0,{0}}};
   int lenReq = 0;
   int numOfAddrAvailable = 0;

   IDS_ERR_RET(mpIDSNetwork->GetDNSAddr(ipAddr, PS_IFACE_NUM_DNS_ADDRS, &lenReq));

   numOfAddrAvailable = min(PS_IFACE_NUM_DNS_ADDRS, lenReq);

   if(numOfAddrAvailable < PS_IFACE_NUM_DNS_ADDRS){
     return QDS_EFAULT;
   }

   if (ds::AddrFamily::QDS_AF_INET6 != ipAddr[DSS_DNS_SECONDARY_ADDR_IDX].family) {
     return QDS_EINVAL;
   }

   return DSSConversion::IDS2DSIpAddr(&ipAddr[DSS_DNS_SECONDARY_ADDR_IDX], pIpv6Addr);
}


//===================================================================
//  FUNCTION:   DSSNetMonitored::GetAllDnsAddrs
//
//  DESCRIPTION:
//  Serves DSS_IFACE_IOCTL_GET_ALL_DNS_ADDRS
//===================================================================
AEEResult DSSNetMonitored::GetAllDnsAddrs(dss_iface_ioctl_get_all_dns_addrs_type* pDnsAddrs)
{
   // check that dns_addrs_ptr, provided by the user, is not NULL
   if(NULL == pDnsAddrs->dns_addrs_ptr) {
      return QDS_EFAULT;
   }

   if (NULL == mpIDSNetwork) {
      return QDS_EINVAL;
   }

   ds::IPAddrType ipAddr[PS_IFACE_NUM_DNS_ADDRS] = {{0,{0}}};
   int lenReq = 0;
   int numOfAddrAvailable = 0;
   const uint8 maxNumOfAddressesPossibleByUser = pDnsAddrs->num_dns_addrs;
   pDnsAddrs->num_dns_addrs = 0;

   // get DNS addresses
   IDS_ERR_RET(mpIDSNetwork->GetDNSAddr(ipAddr, PS_IFACE_NUM_DNS_ADDRS, &lenReq));

   numOfAddrAvailable = min(PS_IFACE_NUM_DNS_ADDRS, lenReq);

   if(0 == numOfAddrAvailable) {
      return QDS_EFAULT;
   }

   if (maxNumOfAddressesPossibleByUser > 0) {

      // convert primary DNS address
      IDS_ERR_RET(DSSConversion::IDS2DSIpAddr(&ipAddr[DSS_DNS_PRIMARY_ADDR_IDX], &pDnsAddrs->dns_addrs_ptr[0]));
      pDnsAddrs->num_dns_addrs++;

      if (maxNumOfAddressesPossibleByUser > 1) {

         // convert secondary DNS address if available
         if(numOfAddrAvailable > 1) {
            IDS_ERR_RET(DSSConversion::IDS2DSIpAddr(&ipAddr[DSS_DNS_SECONDARY_ADDR_IDX], &pDnsAddrs->dns_addrs_ptr[1]));
            pDnsAddrs->num_dns_addrs++;
         }
      }
   }

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::GetMtu
//
//  DESCRIPTION:
//  Serves DSS_IFACE_IOCTL_GET_MTU
//===================================================================
AEEResult DSSNetMonitored::GetMtu(dss_iface_ioctl_mtu_type* pMtu)
{
   if (NULL == mpIDSNetwork) {
      return QDS_EINVAL;
   }

   IDS_ERR_RET(mpIDSNetwork->GetNetMTU((int*)pMtu));

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::GetIPAddress
//
//  DESCRIPTION:
//  Serves DSS_IFACE_IOCTL_GET_MTU
//===================================================================
AEEResult DSSNetMonitored::GetIPAddress(dss_iface_ioctl_get_ip_addr_type* pIpAddr)
{
   if (NULL == mpIDSNetwork) {
      LOG_MSG_ERROR_0 ("NULL args");
      return QDS_EINVAL;
   }

   ds::IPAddrType ipAddr;
   IDS_ERR_RET(mpIDSNetwork->GetIPAddr(&ipAddr));

   IDS_ERR_RET(DSSConversion::IDS2DSIpAddr(&ipAddr, pIpAddr));

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::GetDeviceName
//
//  DESCRIPTION:
//  Serves DSS_IFACE_IOCTL_GET_DEVICE_NAME
//===================================================================
AEEResult DSSNetMonitored::GetDeviceName(dss_iface_ioctl_device_name_type* pDeviceName)
{

  if ((NULL == mpIDSNetwork) ||(NULL == pDeviceName)) {
    LOG_MSG_ERROR_0 ("NULL args");
    return QDS_EINVAL;
  }

  IDS_ERR_RET(mpIDSNetwork->GetDeviceName(pDeviceName));
  return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::GetGatewayAddress
//
//  DESCRIPTION:
//  Serves DSS_IFACE_IOCTL_GET_GATEWAY_ADDR
//===================================================================
AEEResult DSSNetMonitored::GetGatewayAddress(dss_iface_ioctl_gateway_addr_type* pGatewayAddr)
{

  if ((NULL == mpIDSNetwork) || (NULL == pGatewayAddr)) {
    LOG_MSG_ERROR_0 ("NULL args");
    return QDS_EINVAL;
  }

  ds::IPAddrType ipAddr;
  memset(&ipAddr,0,sizeof(ds::IPAddrType));
  IDS_ERR_RET(mpIDSNetwork->GetGatewayAddress(&ipAddr));

  IDS_ERR_RET(DSSConversion::IDS2DSIpAddr(&ipAddr, pGatewayAddr));

  return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::GetSubnetMask
//
//  DESCRIPTION:
//  Serves DSS_IFACE_IOCTL_GET_SUBNET_MASK
//===================================================================
AEEResult DSSNetMonitored::GetSubnetMask(dss_iface_ioctl_subnet_mask_type* pSubnetMask)
{

  if ((NULL == mpIDSNetwork) || (NULL == pSubnetMask)){
    LOG_MSG_ERROR_0 ("NULL args");
    return QDS_EINVAL;
  }

  ds::IPAddrType subnetMask;
  memset(&subnetMask,0,sizeof(ds::IPAddrType));
  IDS_ERR_RET(mpIDSNetwork->GetSubnetMask(&subnetMask));

  IDS_ERR_RET(DSSConversion::IDS2DSIpAddr(&subnetMask, pSubnetMask));

  return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::GetPreviousIPAddress
//
//  DESCRIPTION:
//  Used when DSS_IFACE_IOCTL_ADDR_CHANGED_EV is received
//===================================================================
AEEResult DSSNetMonitored::GetPreviousIPAddress(dss_iface_ioctl_get_ip_addr_type* pIpAddr)
{
   if (NULL == mpIDSNetworkPriv) {
      // TODO: log
      return QDS_EINVAL;
   }

   ds::IPAddrType ipAddr;
   IDS_ERR_RET(mpIDSNetworkPriv->GetPreviousIPAddr(&ipAddr));

   IDS_ERR_RET(DSSConversion::IDS2DSIpAddr(&ipAddr, pIpAddr));

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::GetIfaceState
//
//  DESCRIPTION:
//  Serves DSS_IFACE_IOCTL_GET_STATE
//===================================================================
AEEResult DSSNetMonitored::GetIfaceState(dss_iface_ioctl_state_type* pIfaceState)
{
   if (NULL == mpIDSNetwork) {
      return QDS_EINVAL;
   }

   NetworkStateType netState;
   IDS_ERR_RET(mpIDSNetwork->GetState(&netState));

   IDS_ERR_RET(DSSConversion::IDS2DSIfaceState(netState, pIfaceState));

   return AEE_SUCCESS;
}

AEEResult DSSNetMonitored::GetFastDormStatus(dss_iface_ioctl_fast_dormancy_status_event_type* pFastDormStatus)
{
  uint8 iFastDormStatusInfo = 0;

  if (NULL == mpIDSNetwork) {
    // TODO: log
    return QDS_EINVAL;
  }

  IDS_ERR_RET(mpIDSNetwork->GetFastDormStatusInfo(&iFastDormStatusInfo));
  pFastDormStatus->dorm_status =
    (dss_iface_ioctl_fast_dormancy_status_enum_type) iFastDormStatusInfo;
  return AEE_SUCCESS;
}

AEEResult DSSNetMonitored::GetBearerTech(dss_iface_ioctl_bearer_tech_type* pBearerTech)
{
   IBearerInfo* piBearerInfo = 0;

   if (NULL == mpIDSNetworkExt) {
      return QDS_EINVAL;
   }

   IDS_ERR_RET(mpIDSNetworkExt->GetBearerInfo(&piBearerInfo));
   AEEResult res = DSSConversion::IDS2DSBearerInfo(piBearerInfo, pBearerTech);
   DSSCommon::ReleaseIf((IQI**)&piBearerInfo);
   return res;
}

AEEResult DSSNetMonitored::GetBearerTechEx(dss_iface_ioctl_bearer_tech_ex_type* pBearerTechEx)
{
  IBearerTechEx* piBearerTechEx = 0;

  if (NULL == mpIDSNetworkExt) {
    return QDS_EINVAL;
  }

  IDS_ERR_RET(mpIDSNetworkExt->GetBearerTechEx(&piBearerTechEx));
  AEEResult res = DSSConversion::IDS2DSBearerTechEx(piBearerTechEx, pBearerTechEx);
  DSSCommon::ReleaseIf((IQI**)&piBearerTechEx);
  return res;
}

AEEResult DSSNetMonitored::_3GPPLTEReestabCalltypeGet(dss_iface_ioctl_3gpp_get_lte_reestab_calltype_type* p3gppLteReestabGet)
{
  AEEResult res = AEE_SUCCESS;
  if (NULL == mpIDSNetwork) {
    return QDS_EINVAL;
  }

  _3GPPLTEReestabCalltypeStruct _3gpplteReestabCalltype;
  memset(&_3gpplteReestabCalltype, 0, sizeof(_3GPPLTEReestabCalltypeStruct));

  IDS_ERR_RET(mpIDSNetwork->_3GPPLTEReestabCalltypeGet(&_3gpplteReestabCalltype));
  IDS_ERR_RET(DSSConversion::IDS2DS3GPPLTEReestabCalltype(&_3gpplteReestabCalltype, p3gppLteReestabGet));
  LOG_MSG_FUNCTION_EXIT_2("DSSNetMonitored::_3GPPLTEReestabCalltypeGet(): "
                          "NetHandle :0x%x , result :0x%x ", mpIDSNetwork, res);
  return res;
}

AEEResult DSSNetMonitored::_3GPPLTEReestabCalltypeSet(dss_iface_ioctl_3gpp_set_lte_reestab_calltype_type* p3gppLteReestabSet)
{
  AEEResult res = AEE_SUCCESS;
  _3GPPLTEReestabCalltypeStruct _3gpplteReestabCalltype;
  memset(&_3gpplteReestabCalltype, 0, sizeof(_3GPPLTEReestabCalltypeStruct));

  if (NULL == mpIDSNetwork) {
    return QDS_EINVAL;
  }

  IDS_ERR_RET(DSSConversion::DS2IDS3GPPLTEReestabCalltype(p3gppLteReestabSet, &_3gpplteReestabCalltype));
  IDS_ERR_RET(mpIDSNetwork->_3GPPLTEReestabCalltypeSet(&_3gpplteReestabCalltype));
  LOG_MSG_FUNCTION_EXIT_2("DSSNetMonitored::_3GPPLTEReestabCalltypeSet(): "
                          "NetHandle :0x%x , result :0x%x ", mpIDSNetwork, res);

  return res;
}

AEEResult DSSNetMonitored::SetSilentRedial(dss_iface_ioctl_set_silent_redial_type* pSilentRedial)
{
  AEEResult res = AEE_SUCCESS;
  SilentRedialStruct silentRedialtype;

  if (NULL == mpIDSNetwork) {
    return QDS_EINVAL;
  }

  IDS_ERR_RET(DSSConversion::DS2IDSSilentRedial(pSilentRedial, &silentRedialtype));
  IDS_ERR_RET(mpIDSNetwork->SetSilentRedial(&silentRedialtype));
  LOG_MSG_FUNCTION_EXIT_2("DSSNetMonitored::SetSilentRedial(): "
                          "NetHandle :0x%x , result :0x%x ", mpIDSNetwork, res);

  return res;
}

AEEResult DSSNetMonitored::_3GPPFORCEPDNdisconnect()
{
  AEEResult res = AEE_SUCCESS;
 
  if (NULL == mpIDSNetwork) {
    return QDS_EINVAL;
  }

  IDS_ERR_RET(mpIDSNetwork->_3GPPFORCEPDNdisconnect());
  LOG_MSG_FUNCTION_EXIT_2("DSSNetMonitored::_3GPPFORCEPDNdisconnect(): "
                          "NetHandle :0x%x , result :0x%x ", mpIDSNetwork, res);

  return res;
}

AEEResult DSSNetMonitored::Setkeepalive(dss_iface_keep_alive_flag_type* pKeepaliveflag)
{
  AEEResult res = AEE_SUCCESS;
 
  if (NULL == mpIDSNetwork) {
    return QDS_EINVAL;
  }

  IDS_ERR_RET(mpIDSNetwork->Setkeepalive(*pKeepaliveflag));
  LOG_MSG_FUNCTION_EXIT_2("DSSNetMonitored::Setkeepalive(): "
                          "NetHandle :0x%x , result :0x%x ", mpIDSNetwork, res);

  return res;
}

AEEResult DSSNetMonitored::GetDataPathBridgeMode(dss_iface_ioctl_data_path_bridge_mode_type* pGetDataPath)
{
  AEEResult res = AEE_SUCCESS;
  if (NULL == mpIDSNetwork) {
    return QDS_EINVAL;
  }

  DataPathBridgeModeType dataPath;

  IDS_ERR_RET(mpIDSNetwork->GetDataPathBridgeMode(&dataPath));
  IDS_ERR_RET(DSSConversion::IDS2DSDataPath(&dataPath, pGetDataPath));
  return res;
}

AEEResult DSSNetMonitored::SetDataPathBridgeMode(dss_iface_ioctl_data_path_bridge_mode_type* pSetDataPath)
{
  AEEResult res = AEE_SUCCESS;
  DataPathBridgeModeType dataPath;

  if (NULL == mpIDSNetwork) {
    return QDS_EINVAL;
  }

  IDS_ERR_RET(DSSConversion::DS2IDSDataPath(pSetDataPath, &dataPath));
  IDS_ERR_RET(mpIDSNetwork->SetDataPathBridgeMode(&dataPath));
  
  return res;
}

AEEResult DSSNetMonitored::GetOpResPco(dss_iface_op_reserved_pco_type* pGetOpResPco)
{
  AEEResult res = AEE_SUCCESS;
  if (NULL == mpIDSNetwork) {
    return QDS_EINVAL;
  }

  NetworkOpReservedPcoType opReservedPco;

  IDS_ERR_RET(mpIDSNetwork->GetOpResPco(&opReservedPco));
  IDS_ERR_RET(DSSConversion::IDS2DSOpResPco(&opReservedPco, pGetOpResPco));
  return res;
}

AEEResult DSSNetMonitored::GetPreviousBearerTech(dss_iface_ioctl_bearer_tech_type* pBearerTech)
{
   IBearerInfo* piBearerInfo = 0;
   if (NULL == mpIDSNetworkPriv) {
      return QDS_EINVAL;
   }

   IDS_ERR_RET(mpIDSNetworkPriv->GetPreviousBearerInfo(&piBearerInfo));
   AEEResult res = DSSConversion::IDS2DSBearerInfo(piBearerInfo, pBearerTech);
   DSSCommon::ReleaseIf((IQI**)&piBearerInfo);
   return res;
}

AEEResult DSSNetMonitored::SetPDNTeardownConfig
(
  dss_iface_ioctl_pdn_teardown_config_type* pSetPDNTeardownConfig
)
{
  AEEResult res = AEE_SUCCESS;
  PDNTeardownConfigType pdnTeardownConfig;

  if (NULL == mpIDSNetwork) {
    return QDS_EINVAL;
  }

  IDS_ERR_RET(DSSConversion::DS2IDSPDNTeardown(pSetPDNTeardownConfig, &pdnTeardownConfig));
  IDS_ERR_RET(mpIDSNetwork->SetPDNTeardownConfig(&pdnTeardownConfig));
  
  return res;
} /* DSSNetMonitored::SetPDNTeardownConfig */

AEEResult DSSNetMonitored::GetPDNTeardownConfig
(
  dss_iface_ioctl_pdn_teardown_config_type* pGetPDNTeardownConfig
)
{
  AEEResult res = AEE_SUCCESS;
  PDNTeardownConfigType pdnTeardownConfig;

  if (NULL == mpIDSNetwork) {
    return QDS_EINVAL;
  }

  IDS_ERR_RET(mpIDSNetwork->GetPDNTeardownConfig(&pdnTeardownConfig));
  IDS_ERR_RET(DSSConversion::IDS2DSPDNTeardown(&pdnTeardownConfig, pGetPDNTeardownConfig));
 
  return res;
} /* DSSNetMonitored::GetPDNTeardownConfig */

//===================================================================
//  FUNCTION:   DSSNetMonitored::GetPhysLink
//
//  DESCRIPTION:
//  TODO
//===================================================================
AEEResult DSSNetMonitored::GetPhysLink
(
   ds::Net::IPhysLink** pIDSPhysLink
)
{
   AEEResult res = AEE_SUCCESS;
   AEEResult ret = AEE_SUCCESS;
   ds::Net::IPhysLink* pTmpIDSPhysLink = NULL;
   ds::Net::INetwork* pTmpIDSNetwork = NULL;

   mpCritSect->Enter();
   if (0 == mpDSNetPhysLink) {
      if (NULL == mpIDSNetwork) {
         ret = QDS_EINVAL;
         goto bail;
      }

      // Calling DSNET is performed outside critical section locking
      pTmpIDSNetwork = mpIDSNetwork;
      mpCritSect->Leave();
      res = pTmpIDSNetwork->GetTXPhysLink(&pTmpIDSPhysLink);
      if(AEE_SUCCESS !=res ) {
         LOG_MSG_INFO1_0("DSSNetMonitored::GetPhysLink(): "
                         "GetPhysLinkObject - iface down, physLink cannot be created");
         return QDS_ENETDOWN;
      }

      mpCritSect->Enter();
      mpDSNetPhysLink = pTmpIDSPhysLink;
   }

   (void)mpDSNetPhysLink->AddRef();

   *pIDSPhysLink = mpDSNetPhysLink;

bail:
  mpCritSect->Leave();
  return ret;
}

AEEResult DSSNetMonitored::GetPhysLinkObject(uint32 ifaceid, IPhysLink** ppPhyslink)
{
   FlowID flowid                        = 0;
   AEEResult res                        = AEE_SUCCESS;
   DSSIDSNetworkScope IDSNetworkScope;

   flowid = DSSIfaceId::StripDownToFlowID(ifaceid);

   if (0 !=  flowid) { // Flow id is real
     res = QDS_EBADF;
     goto bail;
   }

   res = IDSNetworkScope.Init(this);
   if (AEE_SUCCESS != res) {
      goto bail;
   }

   res = IDSNetworkScope.Fetch()->GetTXPhysLink(ppPhyslink);
   if (AEE_SUCCESS != res) {
      LOG_MSG_INFO1_0("DSSNetMonitored::GetPhysLinkObject(): "
                      "GetPhysLinkObject - physLink cannot be created");
      res = QDS_ENETDOWN;
      goto bail;
   }

bail:
   if (AEE_SUCCESS != res) {
      *ppPhyslink = NULL;
   }

   return res;
}


AEEResult DSSNetMonitored::GetRFConditions(dss_iface_ioctl_rf_conditions_type* pRFConds)
{
   RFConditionType rfCondition;

   IDS_ERR_RET(GetBearerTech(&pRFConds->bearer_tech));
   IDS_ERR_RET(mpIDSNetwork->GetCurrRFCondition(&rfCondition));
   IDS_ERR_RET(DSSConversion::IDS2DSRFCondition(rfCondition, &pRFConds->rf_conditions));
   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:    DSSNetMonitored::RegEventCB
//
//  DESCRIPTION: Serves DSS_IFACE_IOCTL_REG_EVENT_CB
//===================================================================
AEEResult DSSNetMonitored::RegEventCB(dss_iface_ioctl_ev_cb_type* pEvArg,
                                      dss_iface_id_type ifaceId)
{
   return RegEventCB(pEvArg->event,pEvArg->event_cb,
                     pEvArg->user_data_ptr,
                     ifaceId);
}

//===================================================================
//  FUNCTION:    DSSNetMonitored::RegEventCB
//
//  DESCRIPTION: Serves DSS_IFACE_IOCTL_REG_EVENT_CB and other IOCTLs
//===================================================================
AEEResult DSSNetMonitored::RegEventCB(dss_iface_ioctl_event_enum_type event,
                                      dss_iface_ioctl_event_cb event_cb,
                                      void* user_data_ptr,
                                      dss_iface_id_type ifaceId)
{
   DSSEventHandler* ppEventHandler = NULL;
   sint15 netHandle = 0;

   LOG_MSG_FUNCTION_ENTRY_2("DSSNetMonitored::RegEventCB(): "
                            "NetHandle :%d , Event :%d ", mNetHandle, event);

   AEEResult res = GetEventHandler(event, &ppEventHandler, true);
   if (AEE_SUCCESS != res) {
      GetNetHandle(&netHandle);
      LOG_MSG_ERROR_2("DSSNetMonitored::RegEventCB(): "
                      "Error while registering app %d for event %d",
                      netHandle, event);
      return res;
   }

   res = ppEventHandler->Register(
      event,
      event_cb,
      user_data_ptr,
      ifaceId);
   if (AEE_SUCCESS != res) {
      GetNetHandle(&netHandle);
      if (QDS_EINPROGRESS == res) {
         LOG_MSG_ERROR_2("DSSNetMonitored::RegEventCB(): "
                         "App %d already has registered for event %d",
                         netHandle, event);
         res = QDS_EFAULT;
      } else {
        LOG_MSG_ERROR_2("DSSNetMonitored::RegEventCB(): "
                        "Error while registering app %d for event %d",
                        netHandle, event);
      }
      return res;
   }

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:    DSSNetMonitored::DeregEventCB
//
//  DESCRIPTION: Serves DSS_IFACE_IOCTL_DEREG_EVENT_CB
//===================================================================
AEEResult DSSNetMonitored::DeregEventCB(dss_iface_ioctl_ev_cb_type* pEvArg)
{
   return DeregEventCB(pEvArg->event);
}

//===================================================================
//  FUNCTION:    DSSNetMonitored::DeregEventCB
//
//  DESCRIPTION: Serves DSS_IFACE_IOCTL_DEREG_EVENT_CB
//===================================================================
AEEResult DSSNetMonitored::DeregEventCB(dss_iface_ioctl_event_enum_type event)
{
   DSSEventHandler* pEventHandler = NULL;
   sint15 netHandle = -1;
   AEEResult res = AEE_SUCCESS;

   LOG_MSG_FUNCTION_ENTRY_2("DSSNetMonitored::DeregEventCB(): "
                            "NetHandle :%d , Event :%d ", mNetHandle, event);

   res = GetEventHandler(event, &pEventHandler, false);
   if (AEE_SUCCESS != res) {
      GetNetHandle(&netHandle);
      LOG_MSG_ERROR_2("DSSNetMonitored::DeregEventCB(): "
                      "App %d couldn't deregister for event %d",
                      netHandle, event);
      return res;
   }

   // De-Register to the event.
   if (NULL != pEventHandler) {
      res = pEventHandler->DeRegister(event);
      if (AEE_SUCCESS != res) {
         GetNetHandle(&netHandle);
         if (QDS_ENOTCONN == res) {
            LOG_MSG_ERROR_2("DSSNetMonitored::DeregEventCB(): "
                            "App %d already is not registered for event %d",
                            netHandle, event);
            res = QDS_EFAULT;
         } else {
            LOG_MSG_ERROR_2("DSSNetMonitored::DeregEventCB(): "
                            "Error while deregistering app %d for event %d",
                            netHandle, event);
         }
         return res;
      }

      // release event handler object
      ReleaseEventHandler(event);
   }

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:    DSSNetMonitored::RegMTPDEventCB
//
//  DESCRIPTION: Serves DSS_IFACE_IOCTL_MT_REG_CB
//===================================================================
AEEResult DSSNetMonitored::RegMTPDEventCB(dss_iface_ioctl_mt_reg_cb_type* pEvArg,
                                          dss_iface_id_type ifaceId)
{
   // Get the MTPD event handler
   DSSEventHandler* ppEventHandler = NULL;
   sint15 netHandle = 0;

   AEEResult res = GetEventHandler(DSS_IFACE_IOCTL_MT_REQUEST_EV, &ppEventHandler, true);
   if (AEE_SUCCESS != res) {
      GetNetHandle(&netHandle);
      LOG_MSG_ERROR_2("DSSNetMonitored::RegMTPDEventCB(): "
                      "Error while registering app %d for event %d",
                      netHandle, DSS_IFACE_IOCTL_MT_REQUEST_EV);
      return res;
   }

   // The pointer will be the MTPD handle that is returned to the caller.
   pEvArg->handle = (dss_iface_ioctl_mt_handle_type*)ppEventHandler;

   // Register to the event.
   res = ppEventHandler->Register(
      DSS_IFACE_IOCTL_MT_REQUEST_EV,
      pEvArg->event_cb,
      pEvArg->user_data_ptr,
      ifaceId);
   if (AEE_SUCCESS != res) {
      GetNetHandle(&netHandle);
      if (QDS_EINPROGRESS == res) {
         LOG_MSG_ERROR_2("DSSNetMonitored::RegMTPDEventCB(): "
                         "App %d already has registered for event %d",
                         netHandle, DSS_IFACE_IOCTL_MT_REQUEST_EV);
         res = QDS_EFAULT;
      } else {
         LOG_MSG_ERROR_2("DSSNetMonitored::RegMTPDEventCB(): "
                         "Error while registering app %d for event %d",
                         netHandle, DSS_IFACE_IOCTL_MT_REQUEST_EV);

      }
      return res;
   }

   return AEE_SUCCESS;
}

AEEResult DSSNetMonitored::GetNetQoSManager(IQoSManager** ppNetQosManager)
{
   ds::Net::IQoSManager* pDSNetQoSManager = NULL;
   if(NULL == mpDSNetQoSManager) {

      // Create a new NetQoSManager since pNetActive does not have
      // a NetQoSManager instance.
      IDS_ERR_RET(mpIDSNetworkExt->CreateQoSManager(&pDSNetQoSManager));
   }

   DSSCritScope cs(*mpCritSect);
   if(NULL == mpDSNetQoSManager) {
      mpDSNetQoSManager = pDSNetQoSManager;
   }
   else{
      DSSCommon::ReleaseIf((IQI**)&pDSNetQoSManager);
   }
   *ppNetQosManager = mpDSNetQoSManager;
   (void) mpDSNetQoSManager->AddRef();

   return AEE_SUCCESS;
}

AEEResult DSSNetMonitored::SetQoSDefault(IQoS* pNetQoSDefault)
{
   DSSCritScope cs(*mpCritSect);

   if (NULL != mpDSSNetQoSDefault) {
      // do nothing if QosDefault already exists
      return AEE_SUCCESS;
   }
   // AddRef is done inside
   mpDSSNetQoSDefault = PS_MEM_NEW(DSSNetQoSDefault(pNetQoSDefault));
   if (NULL == mpDSSNetQoSDefault) {
     LOG_MSG_ERROR_0("DSSNetMonitored::SetQoSDefault(): "
                     "Couldn't allocate DSSNetQoSDefault");
      return AEE_ENOMEMORY;
   }

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:    DSSNetMonitored::DeRegMTPDEventCB
//
//  DESCRIPTION: DeRegister DSS_IFACE_IOCTL_MT_REG_CB
//===================================================================
AEEResult DSSNetMonitored::DeRegMTPDEventCB(dss_iface_ioctl_mt_dereg_cb_type* pEvArg)
{
   if (NULL == mpMTPDRequestHandler) {
      return DS_EFAULT;
   }

   // Check if this is the same handle that was already allocated.
   if (pEvArg->handle != (dss_iface_ioctl_mt_handle_type)mpMTPDRequestHandler) {
      return DS_EFAULT;
   }

   PS_MEM_RELEASE(mpMTPDRequestHandler);

   return AEE_SUCCESS;
}

template<typename HandlerType>
AEEResult DSSNetMonitored::FetchHandler(HandlerType** pHandler,
                                        DSSEventHandler** ppEventHandler,
                                        bool bInit,
                                        dss_iface_ioctl_event_enum_type event)
{
   if (bInit) {
      if (NULL == *pHandler) {
         *pHandler = HandlerType::CreateInstance();
         if (NULL == *pHandler) {
            return AEE_ENOMEMORY;
         }
         IDS_ERR_RET((*pHandler)->Init(this, event));
      }
   } else {
      // bInit is false , we expect pHandler to have a value here
      if (0 == *pHandler) {
         return QDS_EFAULT;
      }
   }
   *ppEventHandler = *pHandler;
   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:    DSSNetMonitored::GetEventHandler
//
//  DESCRIPTION: Returns the DSSEventHandler for the specified event.
//               If bInit is true, this function will also initialize
//               the handler if it's not initialized.
//               This method's switch case must correspond to the one in
//               ReleaseEventHandler() method.
//===================================================================
AEEResult DSSNetMonitored::GetEventHandler(dss_iface_ioctl_event_enum_type event,
                                           DSSEventHandler** ppEventHandler,
                                           bool bInit)
{
   // This method's switch case must correspond to the one in
   // ReleaseEventHandler() method.
   switch (event) {
      case DSS_IFACE_IOCTL_DOWN_EV:
         return FetchHandler(&mpNetworkStateDownHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_DOWN_EV);
      case DSS_IFACE_IOCTL_UP_EV:
         return FetchHandler(&mpNetworkStateUpHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_UP_EV);
      case DSS_IFACE_IOCTL_COMING_UP_EV:
         return FetchHandler(&mpNetworkStateComingUpHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_COMING_UP_EV);
      case DSS_IFACE_IOCTL_GOING_DOWN_EV:
         return FetchHandler(&mpNetworkStateGoingDownHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_GOING_DOWN_EV);
      case DSS_IFACE_IOCTL_ADDR_CHANGED_EV:
         return FetchHandler(&mpNetIPHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_MIN_EV);
      case DSS_IFACE_IOCTL_EXTENDED_IP_CONFIG_EV:
         return FetchHandler(&mpExtendedIPConfigHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_MIN_EV);
      case DSS_IFACE_IOCTL_RF_CONDITIONS_CHANGED_EV:
         return FetchHandler(&mpRFConditionsHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_MIN_EV);
      case DSS_IFACE_IOCTL_FAST_DORMANCY_STATUS_EV:
        return FetchHandler(&mpFastDormStatusHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_MIN_EV);
      case DSS_IFACE_IOCTL_BEARER_TECH_CHANGED_EV:
         return FetchHandler(&mpBearerTechHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_MIN_EV);
      case DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV:
        return FetchHandler(&mpPhysLinkDownHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV);
      case DSS_IFACE_IOCTL_PHYS_LINK_UP_EV:
         return FetchHandler(&mpPhysLinkUpHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_PHYS_LINK_UP_EV);
      case DSS_IFACE_IOCTL_PHYS_LINK_COMING_UP_EV:
         return FetchHandler(&mpPhysLinkComingUpHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_PHYS_LINK_COMING_UP_EV);
      case DSS_IFACE_IOCTL_PHYS_LINK_GOING_DOWN_EV:
         return FetchHandler(&mpPhysLinkGoingDownHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_PHYS_LINK_GOING_DOWN_EV);
      case DSS_IFACE_IOCTL_QOS_AWARE_SYSTEM_EV:
         return FetchHandler(&mpQosAwareHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_QOS_AWARE_SYSTEM_EV);
      case DSS_IFACE_IOCTL_QOS_UNAWARE_SYSTEM_EV:
         return FetchHandler(&mpQosUnawareHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_QOS_UNAWARE_SYSTEM_EV);
      case DSS_IFACE_IOCTL_QOS_ADDED_ON_IFACE_EV:
         return AEE_EUNSUPPORTED;
      case DSS_IFACE_IOCTL_QOS_DELETED_ON_IFACE_EV:
         return AEE_EUNSUPPORTED;
      case DSS_IFACE_IOCTL_OUTAGE_NOTIFICATION_EV:
         return FetchHandler(&mpOutageHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_MIN_EV);
      case DSS_IFACE_IOCTL_707_NETWORK_SUPPORTED_QOS_PROFILES_CHANGED_EV:
         return FetchHandler(&mpQoSProfileChangedHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_MIN_EV);  // TODO change handler name to 1x something
#ifdef FEATURE_DATA_PS_IPV6
      case DSS_IFACE_IOCTL_PREFIX_UPDATE_EV:
         return FetchHandler(&mpDSSIPv6PrefixChangedStateHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_MIN_EV);
#endif
      case DSS_IFACE_IOCTL_PRIMARY_QOS_MODIFY_RESULT_EV:
         return FetchHandler(&mpPrimaryQoSModifyHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_MIN_EV);
      case DSS_IFACE_IOCTL_MT_REQUEST_EV:
         return FetchHandler(&mpMTPDRequestHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_MIN_EV);
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV:
         return FetchHandler(&mpSlottedResultSuccessHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV);
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV:
         return FetchHandler(&mpSlottedResultFailureHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV);
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SESSION_CHANGED_EV:
         return FetchHandler(&mpSlottedSessionChangedHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_MIN_EV);
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA_SUCCESS_EV:
         return FetchHandler(&mpHDRRev0RateInteriaSuccessHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA_SUCCESS_EV);
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA_FAILURE_EV:
         return FetchHandler(&mpHDRRev0RateInteriaFailureHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA_FAILURE_EV);

      case DSS_IFACE_IOCTL_IPV6_EXTERNAL_ADDR_DELETED_EV:
         return FetchHandler(&mpNetworkExtIPv6DelAddrHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_IPV6_EXTERNAL_ADDR_DELETED_EV);

      case DSS_IFACE_IOCTL_HANDOFF_INIT_EV:
        return FetchHandler(&mpHandoffInitHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_HANDOFF_INIT_EV);
      case DSS_IFACE_IOCTL_HANDOFF_STATUS_SUCCESS_EV:
        return FetchHandler(&mpHandoffSuccessHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_HANDOFF_STATUS_SUCCESS_EV);
      case DSS_IFACE_IOCTL_HANDOFF_STATUS_FAILURE_EV:
         return FetchHandler(&mpHandoffFailureHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_HANDOFF_STATUS_FAILURE_EV);
      case DSS_IFACE_IOCTL_BEARER_TECH_EX_CHANGED_EV:
         return FetchHandler(&mpBearerTechExHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_BEARER_TECH_EX_CHANGED_EV);
      case DSS_IFACE_IOCTL_EXTENDED_IP_CONFIG_EX_EV:
        return FetchHandler(&mpExtendedIpConfigExHandler, ppEventHandler, bInit, DSS_IFACE_IOCTL_EXTENDED_IP_CONFIG_EX_EV);
      case DSS_IFACE_IOCTL_APN_PARAM_CHANGE_EV:
        return FetchHandler(&mpApnParamChangedHandler, ppEventHandler, bInit,DSS_IFACE_IOCTL_APN_PARAM_CHANGE_EV);
      default:
         LOG_MSG_ERROR_1("DSSNetMonitored::GetEventHandler(): "
                         "Unhandled event=%d case", event);
         return QDS_EFAULT;
   }
}

AEEResult DSSNetMonitored::GetTechUMTS
(
   ds::Net::ITechUMTS** pIDSTechUMTS
)
{
   DSSGenScope scopeNetworkFactory;

   if(0 == mpDSNetTechUMTS){
      INetworkFactory*       pINetworkFactory = NULL;
      AEEResult              res = AEE_SUCCESS;

      res = DSSGlobals::Instance()->GetNetworkFactory(&pINetworkFactory);
      if (AEE_SUCCESS != res || 0 == pINetworkFactory) {
         return QDS_EFAULT;
      }
      scopeNetworkFactory.SetParams(pINetworkFactory, DSSGenScope::IDSIQI_TYPE);

      res = pINetworkFactory->CreateTechUMTS(&mpDSNetTechUMTS);
      if(AEE_SUCCESS != res){
         return QDS_EFAULT;
      }
}

   (void)mpDSNetTechUMTS->AddRef();

   *pIDSTechUMTS = mpDSNetTechUMTS;

   return AEE_SUCCESS;
}

AEEResult DSSNetMonitored::GetQoSDefault(IQoS** ppNetQoSDefault)
{
   if(NULL == mpDSSNetQoSDefault) {

      // Create a new DSSNetQosDefault since pNetActive does not have
      // a DSSNetQosDefault instance.
      IQoS*        pNetQoSDefault = NULL;
      IQoSManager* pNetQoSManager = NULL;

      IDS_ERR_RET(GetNetQoSManager(&pNetQoSManager));
      DSSGenScope scopeNetQosManager(pNetQoSManager,DSSGenScope::IDSIQI_TYPE);

      // Get the primary QoS
      IDS_ERR_RET(pNetQoSManager->GetQosDefault(&pNetQoSDefault));
      DSSGenScope scopeNetQosDefault(pNetQoSDefault,DSSGenScope::IDSIQI_TYPE);

      // Set the primary QoS in DSSNetActive (set the mpDSSNetQoSDefault)
      IDS_ERR_RET(SetQoSDefault(pNetQoSDefault));
   }

   return mpDSSNetQoSDefault->GetNetQoSDefault(ppNetQoSDefault);
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::GetNetworkStatistics
//
//  DESCRIPTION:
//  Serves DSS_IFACE_IOCTL_GET_IFACE_STATS
//===================================================================
AEEResult DSSNetMonitored::GetNetworkStatistics(dss_iface_ioctl_get_iface_stats_type* pStats)
{
   if (NULL == mpIDSNetwork) {
      return ds::Error::QDS_EINVAL;
   }

   NetworkStatsType statsType;
   IDS_ERR_RET(mpIDSNetwork->GetNetworkStatistics(&statsType));

   IDS_ERR_RET(DSSConversion::IDS2DSStatistics(&statsType, pStats));

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSNetMonitored::ResetNetworkStatistics
//
//  DESCRIPTION:
//  Serves DSS_IFACE_IOCTL_RESET_IFACE_STATS
//===================================================================
AEEResult DSSNetMonitored::ResetNetworkStatistics(void)
{
   if (NULL == mpIDSNetwork) {
      return ds::Error::QDS_EINVAL;
   }

   IDS_ERR_RET(mpIDSNetwork->ResetNetworkStatistics());

   return AEE_SUCCESS;
}

DSSNetMonitored* DSSNetMonitored::CreateInstance(bool bPolicyBitIsOn)
{
   return new DSSNetMonitored(bPolicyBitIsOn);
}

void DSSNetMonitored::ReleasePhysLink(void)
{
   DSSCommon::ReleaseIf((IQI**)&mpDSNetPhysLink);
}

//===================================================================
//  FUNCTION:    DSSNetMonitored::ReleaseEventHandler
//
//  DESCRIPTION: Releases the DSSEventHandler for the specified event.
//               This method's switch case must correspond to the one in
//               GetEventHandler() method.
//===================================================================
AEEResult DSSNetMonitored::ReleaseEventHandler(dss_iface_ioctl_event_enum_type event)
{
   // This method's switch case must correspond to the one in
   // GetEventHandler() method.
   switch (event) {
      case DSS_IFACE_IOCTL_DOWN_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpNetworkStateDownHandler));
         break;
      case DSS_IFACE_IOCTL_UP_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpNetworkStateUpHandler));
         break;
      case DSS_IFACE_IOCTL_COMING_UP_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpNetworkStateComingUpHandler));
         break;
      case DSS_IFACE_IOCTL_GOING_DOWN_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpNetworkStateGoingDownHandler));
         break;
      case DSS_IFACE_IOCTL_ADDR_CHANGED_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpNetIPHandler));
         break;
      case DSS_IFACE_IOCTL_EXTENDED_IP_CONFIG_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpExtendedIPConfigHandler));
         break;
      case DSS_IFACE_IOCTL_RF_CONDITIONS_CHANGED_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpRFConditionsHandler));
         break;
      case DSS_IFACE_IOCTL_FAST_DORMANCY_STATUS_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpFastDormStatusHandler));
         break;
      case DSS_IFACE_IOCTL_BEARER_TECH_CHANGED_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpBearerTechHandler));
         break;
      case DSS_IFACE_IOCTL_BEARER_TECH_EX_CHANGED_EV:
        DSSCommon::ReleaseIf((IQI**)(&mpBearerTechExHandler));
        break;
      case DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpPhysLinkDownHandler));
         break;
      case DSS_IFACE_IOCTL_PHYS_LINK_UP_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpPhysLinkUpHandler));
         break;
      case DSS_IFACE_IOCTL_PHYS_LINK_COMING_UP_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpPhysLinkComingUpHandler));
         break;
      case DSS_IFACE_IOCTL_PHYS_LINK_GOING_DOWN_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpPhysLinkGoingDownHandler));
         break;
      case DSS_IFACE_IOCTL_QOS_AWARE_SYSTEM_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpQosAwareHandler));
         break;
      case DSS_IFACE_IOCTL_QOS_UNAWARE_SYSTEM_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpQosUnawareHandler));
         break;
      case DSS_IFACE_IOCTL_QOS_ADDED_ON_IFACE_EV:
         return AEE_EUNSUPPORTED;
      case DSS_IFACE_IOCTL_QOS_DELETED_ON_IFACE_EV:
         return AEE_EUNSUPPORTED;
      case DSS_IFACE_IOCTL_OUTAGE_NOTIFICATION_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpOutageHandler));
         break;
      case DSS_IFACE_IOCTL_707_NETWORK_SUPPORTED_QOS_PROFILES_CHANGED_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpQoSProfileChangedHandler));
         break;
#ifdef FEATURE_DATA_PS_IPV6
      case DSS_IFACE_IOCTL_PREFIX_UPDATE_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpDSSIPv6PrefixChangedStateHandler));
         break;
#endif
      case DSS_IFACE_IOCTL_PRIMARY_QOS_MODIFY_RESULT_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpPrimaryQoSModifyHandler));
         break;
      case DSS_IFACE_IOCTL_MT_REQUEST_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpMTPDRequestHandler));
         break;
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpSlottedResultSuccessHandler));
         break;
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpSlottedResultFailureHandler));
         break;
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SESSION_CHANGED_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpSlottedSessionChangedHandler));
         break;
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA_SUCCESS_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpHDRRev0RateInteriaSuccessHandler));
         break;
      case DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA_FAILURE_EV:
         DSSCommon::ReleaseIf((IQI**)(&mpHDRRev0RateInteriaFailureHandler));
         break;
      case DSS_IFACE_IOCTL_HANDOFF_INIT_EV:
        DSSCommon::ReleaseIf((IQI**)(&mpHandoffInitHandler));
        break;
      case DSS_IFACE_IOCTL_HANDOFF_STATUS_FAILURE_EV:
        DSSCommon::ReleaseIf((IQI**)(&mpHandoffFailureHandler));
        break;
      case DSS_IFACE_IOCTL_HANDOFF_STATUS_SUCCESS_EV:
        DSSCommon::ReleaseIf((IQI**)(&mpHandoffSuccessHandler));
        break;
      case DSS_IFACE_IOCTL_EXTENDED_IP_CONFIG_EX_EV:
        DSSCommon::ReleaseIf((IQI**)(&mpExtendedIpConfigExHandler));
      case DSS_IFACE_IOCTL_APN_PARAM_CHANGE_EV:
        DSSCommon::ReleaseIf((IQI**)(&mpApnParamChangedHandler));
        break;
      default:
         LOG_MSG_ERROR_1("DSSNetMonitored::ReleaseEventHandler(): "
                         "Unhandled event=%d case", event);
         return QDS_EFAULT;
   }

   return AEE_SUCCESS;
}

AEEResult DSSNetMonitored::GetExtIPv6DelAddr
(
   dss_iface_ioctl_ipv6_addr_type* pIpv6Addr
)
{
  ::ds::INAddr6Type ip6Addr;

  if (NULL == mpIDSNetwork || 0 == pIpv6Addr) {
    LOG_MSG_ERROR_0 ("DSSNetMonitored::GetExtIPv6DelAddr() : NULL args");
    return QDS_EINVAL;
  }

  IDS_ERR_RET(mpIDSNetwork->GetExtIpv6DelAddr(ip6Addr));

  IDS_ERR_RET(DSSConversion::IDS2DSIp6Addr(ip6Addr, (ip_addr_type *)pIpv6Addr));

  return AEE_SUCCESS;
}

AEEResult DSSNetMonitored::GetExtIPConfigEx
(
 dss_iface_ioctl_extended_ip_config_ex_type* pIpConfigEx
 )
{
  ::ds::Net::NetworkExtendedIpConfigExType extended_ip_config_ex = 0;

  if (NULL == mpIDSNetwork || 0 == pIpConfigEx) {
    LOG_MSG_ERROR_0 ("DSSNetMonitored::GetExtIPConfigEx() : NULL args");
    return QDS_EINVAL;
  }

  IDS_ERR_RET(mpIDSNetwork->GetExtIPConfigEx(&extended_ip_config_ex));
  *pIpConfigEx = extended_ip_config_ex;
//  IDS_ERR_RET(DSSConversion::IDS2DSIp6Addr(ip6Addr, (ip_addr_type *)pIpv6Addr));

  return AEE_SUCCESS;
}

AEEResult DSSNetMonitored::GetApnParamInfo
(
 dss_iface_ioctl_changed_apn_param_info_type* pApnParams
)
{
  ApnParamChangeList apnParamChangeList = {0};

  if (NULL == mpIDSNetwork || 0 == pApnParams) {
    LOG_MSG_ERROR_0 ("DSSNetMonitored::GetChangedApnParamInfo() : NULL args");
    return QDS_EINVAL;
  }

  IDS_ERR_RET(mpIDSNetwork->GetApnParamInfo(&apnParamChangeList));
  IDS_ERR_RET(DSSConversion::IDS2DSApnParamInfo(&apnParamChangeList, pApnParams));
  return AEE_SUCCESS;
}
