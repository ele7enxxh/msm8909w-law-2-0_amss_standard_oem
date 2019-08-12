#ifndef DS_NET_CONVERSION_H
#define DS_NET_CONVERSION_H

/*=========================================================================*/
/*!
  @file
  ds_Net_Conversion.h

  @brief
  This file provides misc conversion routines commonly used in the 
  DSNET layer.

  @details
  The conversions defined in this file are related to IP addresses, iface 
  names, iface states, QoS specifications, MCAST specifications etc. All
  the conversion routines are collected under the namespace 
  ds::Net::Conversion.

  Many of the parameters being converted are noted as "PS layer format" and 
  "ds layer format". Basically the PS layer format refers to the lower 
  legacy layers. The ds layer format refers to the new structures defined
  by the DSNET/DSSOCK IDLs.

  Copyright (c) 2009-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_Conversion.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-11-01 rk  Removed feature mobileap.
  2011-11-27 ss  IOCTLs to enable/disable and retrieve ALG status list.
  2008-12-31 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Addr_Def.h"
#include "ds_Net_INetwork.h"
#include "ds_Net_IIPFilterPriv.h"
#include "ds_Net_IQoSFlowPriv.h"
#include "ds_Net_IMCastManager.h"
#include "ds_Net_IMCastManagerBCMCS.h"
#include "ds_Net_IQoSManager.h"
#include "ds_Net_IQoSSecondary.h"
#include "ds_Net_INetworkIPv6Priv.h"
#include "ds_Net_IPFilterSpec.h"
#include "ds_Net_IPhysLink.h"
#include "ds_Net_Platform.h"
#include "ds_Net_EventDefs.h"
#include "ds_Net_IMCastManagerPriv.h"
#include "ds_Net_QoS_DefPriv.h"
#include "ds_Net_Def.h"

namespace ds
{
namespace Net
{
namespace Conversion
{

/*---------------------------------------------------------------------------
  Functions for IP Address conversions.
---------------------------------------------------------------------------*/
/*!
  @brief
  Converts address family from PS to ds. 

  @param[in]  ipFamily - PS layer IP family type.
  
  @see        NetPlatform::PSIPAddrFamilyType
  @see        ds::AddrFamily::DS_AF_ANY
  @see        ds::AddrFamily::QDS_AF_INET
  @see        ds::AddrFamily::QDS_AF_INET6
  @see        ds::AddrFamily::QDS_AF_UNSPEC
  @return     The IP family in ds.
*/
ds::ErrorType PS2DSAddrFamily
(
  NetPlatform::PSIPAddrFamilyType ipFamily, 
  ds::AddrFamilyType*  dsFamily
);

/*!
  @brief
  Converts an IP address from PS to ds.

  @param[in]  pPSIPAddr - PS IP address to be converted.
  @param[out] pDSIPAddr - ds IP address returned.
  
  @see        NetPlatform::PSIPAddrType
  @see        ds::IPAddrType
  @return     QDS_EFAULT - Invalid input.
  @return     AEE_SUCCESS - On success.
*/
ds::ErrorType PS2DSIPAddr
(
  NetPlatform::PSIPAddrType*              pPSIPAddr,
  ds::IPAddrType*                         pDSIPAddr
);

/*!
  @brief
  Converts an IP address from PS to ds.

  @param[in]  pPSIPAddr - PS IP address to be converted.
  @param[out] pDSIPAddr - ds IP address returned.
  
  @see        ip_addr_type
  @see        ds::IPAddrType
  @return     QDS_EFAULT - Invalid input.
  @return     AEE_SUCCESS - On success.
*/
ds::ErrorType PS2DSIPAddr
(
  ip_addr_type*                           pPSIPAddr,
  ds::IPAddrType*                         pDSIPAddr
);

/*!
  @brief
  Converts an IP address from PS to ds.

  @param[in]  pPSIPAddr - PS IP address to be converted.
  @param[out] in6Addr - ds IP address returned.
  
  @see        NetPlatform::PSIPAddrType
  @see        ds::INAddr6Type
  @return     QDS_EFAULT - Invalid input.
  @return     AEE_SUCCESS - On success.
*/
ds::ErrorType PS2DSIPAddr
(
  NetPlatform::PSIPAddrType*              pPSIPAddr,
  ds::INAddr6Type                         in6Addr
);

/*---------------------------------------------------------------------------
  Functions for a QoS structures translations.
---------------------------------------------------------------------------*/
/*!
  @brief
  Converts an IP Filter spec specification from ds to PS format.

  @param[in]  pIDSNetIPFilter - Interface that holds IP filter specification.
  @param[out] pPSFilter - IP Filter specfication in PS format.
  
  @see        Interface IIPFilter
  @see        NetPlatform::PSFilterSpecType
  @return     QDS_EFAULT - Invalid input.
  @return     AEE_SUCCESS - On success.
*/
ds::ErrorType DS2PSIPFilterSpec
(
  IIPFilterPriv*                          pIDSNetIPFilter,
  NetPlatform::PSFilterSpecType*          pPSFilter
);

/*!
  @brief
  Converts QoS Flow spefication from ds to PS format.

  @param[in]  pIDSNetQoSFlowSpec - Interface ptr to QoS flow specification.
  @param[out] pPSFlowSpec - QoS flow specfication in PS format.
  
  @see        Interface IQoSFlowPriv
  @see        NetPlatform::PSFlowSpecType
  @return     QDS_EFAULT - Invalid input.
  @return     AEE_SUCCESS - On success.
*/
ds::ErrorType DS2PSQoSFlowSpec
(
  IQoSFlowPriv*                          pIDSNetQoSFlowSpec,
  NetPlatform::PSFlowSpecType*            pPSFlowSpec
);

/*!
  @brief
  Converts QoS request specification from ds to PS format.

  @details
  A QoS Request specification in ds format contains the following: 
  RX Flows
    Req flow (mandatory), aux flows (optional), min flow (optional) 
  TX Flows
    Req flow (mandatory), aux flows (optional), min flow (optional) 
  RX Filters (optional)
  TX Filters (optional)

  To specify if min flow is present, the boolean values 
  rxMinFlow, txMinFlow, are used. 

  The number of aux flows are calculated based on the total number of flows
  specified. For ex, if min flow is specified and txFlowsLen is 6, then 
  aux flows len is 4. If min flow were not specified, then aux flows len 
  would be 5. The array of flows ordered in this format:
  {req flow, aux flows, min req flow}

  @param[in]  pDSNetQoSSpec - ds Net QOS request spec format.
  @param[out] pPSFlowSpec - QoS flow specfication in PS format.

  @note
  This function allocates temporary memory from system heap for populating
  the PS layer QoS spec (for aux flows array, filter specs array etc). It
  is recommended that ds::Net::Conversion::CleanupPSQoSRequestSpec() be used
  to clean up this memory.

  @see        ds::Net::QoSSpecType
  @see        NetPlatform::PSQoSSpecType
  @see        ds::Net::Conversion::CleanupPSQoSRequestSpec()
  @return     QDS_EFAULT - Invalid input.
  @return     AEE_SUCCESS - On success.
*/
ds::ErrorType DS2PSQoSRequestSpec
(
  const ds::Net::QoSSpecType*             pDSNetQoSSpec, 
  NetPlatform::PSQoSSpecType*             pPSQoSSpec
);

/*!
  @brief
  Converts Network Initiated QoS request specification from ds to PS format.

  @details
  A Network Initiated QoS Request specification in ds format contains the
  following: 
  RX Filters (optional)
  TX Filters (optional)

  @param[in]  pDSNetQoSSpec - ds Net Network Initiated QOS request spec format.
  @param[out] pPSQoSNetInitiatedSpec - Network Initiated QoS specification in
                                       PS format.

  @note
  This function allocates temporary memory from system heap for populating
  the PS layer QoS spec. It is recommended that 
  ds::Net::Conversion::CleanupPSQoSNetInitiatedRequestSpec() be used
  to clean up this memory.

  @see        ds::Net::QoSSpecNetInitiatedType
  @see        NetPlatform::PSQoSNetInitiatedSpecType
  @see        ds::Net::Conversion::CleanupPSQoSNetInitiatedRequestSpec()
  @return     QDS_EFAULT - Invalid input.
  @return     AEE_SUCCESS - On success.
*/
ds::ErrorType DS2PSQoSNetInitiatedRequestSpec
(
  const ds::Net::QoSSpecNetInitiatedType* pDSNetQoSNetInitiatedSpec, 
  NetPlatform::PSQoSNetInitiatedSpecType* pPSQoSNetInitiatedSpec
);

/*!
  @brief  
  This function cleans up the memories allocated by DS2PSQoSRequestSpec().

  @details
  This function cleans up the memories allocated by DS2PSQoSRequestSpec(). 
  In DS2PSQoSRequestSpec() we allocate memory to hold arrays of RX/TX aux 
  flow specs and TX/RX flow specs. This function should be used to clean 
  them. It is highly recommended to use this function to free memory 
  allocated via DS2PSQoSRequestSpec(), instead of client trying to free it 
  by itself.

  @param[in]  pPSQoSSpec - Pointer to PS QoS structure.
  @see        NetPlatform::PSQoSSpecType
  @see        ds::Net::Conversion::DS2PSQoSRequestSpec()
  @return     None.
*/
void CleanupPSQoSRequestSpec
(
  NetPlatform::PSQoSSpecType*             pPSQoSSpec
);        

/*!
  @brief
  This function translates a QoS Spec from PS to ds format.

  @details
  This function translates a QoS Spec from PS to ds format. Primarily this
  function is used for getting updated err masks back into the ds QoS Spec.

  @param[in]   pPSQoSSpec - Pointer to output QoS structure.
  @param[out]  pDSNetQoSSpec - Pointer to input QoS structure.
  @return      None.
*/
void PS2DSQoSSpec
(
  NetPlatform::PSQoSSpecType*             pPSQoSSpec,
  ds::Net::QoSSpecType*                   pDSQoSSpec
);

/*!
  @brief
  This function translates a Network Initiated QoS Spec from PS to ds format.

  @details
  This function translates a Network Initiated QoS Spec from PS to ds format.
  Primarily this function is used for getting updated err masks back into the
  ds QoS Spec.

  @param[in]   pPSQoSSpec - Pointer to output, IDS, QoS structure.
  @param[out]  pDSNetQoSSpec - Pointer to input, PS, QoS structure.
  @return      None.
*/
void PS2DSQoSNetInitiatedSpec
(
 NetPlatform::PSQoSNetInitiatedSpecType*  pPSQoSSpec,
 QoSSpecNetInitiatedType*                 pDSQoSSpec
);

void CleanupPSQoSNetInitiatedSpec
(
 NetPlatform::PSQoSNetInitiatedSpecType*  pPSQoSSpec
);


/*!
  @brief
  This function translates a Filter Spec from PS to DS format.

  @details
  This function translates a Filter Spec from PS to DS format. 

  @param[in]   pPSIPFilterSpec - Pointer to input filter structure.
  @param[out]  pDSNetIPFilterSpec - Pointer to output filter structure.
  @return      None.
*/
void PS2DSIPFilterSpec
(
  NetPlatform::PSFilterSpecType*         pPSIPFilterSpec,
  ds::Net::IPFilterSpec*                 pDSIPFilterSpec
);
/*!
  @brief
  This function translates a QoS Modify spec from ds 2 PS format.

  @details
  For QoS Modify, we have a different signature than ds::Net::QoSSpecType. 
  Hence this function is needed.

  @param[in]   pPSQoSSpec - Pointer to output QoS structure.
  @param[out]  pDSNetQoSSpec - Pointer to input QoS structure.
  @return      AEE_SUCCESS on successful translation.
  @return      EFAULT on invalid input.
*/
ds::ErrorType DS2PSQoSModifySpec
(
  IQoSFlowPriv**                         rxFlows, 
  int                                     rxFlowsLen, 
  IQoSFlowPriv**                         txFlows, 
  int                                     txFlowsLen, 
  ds::Net::QoSModifyMaskType              mask,
  NetPlatform::PrimaryQoSModifySpecType * pPSQoSModifySpec
);

/*!
  @brief
  This function frees the temporary memories allocated by DS2PSQoSModifySpec

  @details
  The function DS2PSQoSModifySpec() allocates temporary memories to hold the
  auxillary flow list arrays from the PS Scratchpad. Use this function to
  free the memories allocated.

  @param[in]   pPSQoSSpec - Pointer to the QoS Modify that needs cleanup.
  @return      None.
*/
void CleanupPSQoSModifySpec
(
  NetPlatform::PrimaryQoSModifySpecType *   pPSQoSSpec
);
#ifdef FEATUTE_DATA_PS_MCAST
/*---------------------------------------------------------------------------
  Functions for a MCast structure translations.
---------------------------------------------------------------------------*/
/*!
  @brief  
  This function constructs the MCastSpec IOCTL argument that is expected 
  by the PS iface layer.

  @param[in]  pSockAddr - MCast Sockaddr to join. 
  @param[out] pPSMCastJoinSpec - Pointer to output PS Join IOCTL type.
  @see        ds::Sock::SockAddrStorageType
  @see        NetPlatform::MCastJoinType
  @return     AEE_SUCCESS on success.
  @return     QDS_EFAULT on invalid arguments.
*/
ds::ErrorType DS2PSMCastJoinSpec
(
  const SockAddrStorageType      pSockAddr, 
  NetPlatform::MCastJoinType*    pPSMCastJoinSpec
);

/*!
  @brief  
  This function constructs the MCAST_JOIN_EX IOCTL argument.

  @param[in]  pSockAddrSeq - Set of socket addrs to join.
  @param[in]  addrSeqLen - Number of socket addresses.
  @param[out] pPSMCastJoinExSpec - Output argument to Join.

  @see        ds::Sock::SockAddrStorageType 
  @see        NetPlatform::MCastJoinExType
  @return     AEE_SUCCESS on success.
  @return     QDS_EFAULT on invalid arguments.
*/
ds::ErrorType DS2PSMCastJoinExSpec
(
  const SockAddrStorageType *     pSockAddrSeq, 
  int                             addrSeqLen, 
  NetPlatform::MCastJoinExType *  pPSMCastJoinExSpec,
  const MCastJoinFlagsType *      regFlags
);

/*!
  @brief  
  This function constructs the MCastAddrInfo argument that is expected 
  by the PS iface layer.

  @param[in]  addrSeq - MCast Sockaddr 
  @param[out] pMCastAddrInfo - Pointer to output PS MCast AddrInfo type.
  @see        ds::Sock::SockAddrStorageType
  @see        NetPlatform::MCastAddrInfoType
  @return     AEE_SUCCESS on success.
  @return     QDS_EFAULT on invalid arguments.
*/
ds::ErrorType DS2PSMCastAddrInfo
(
  const SockAddrStorageType        addrSeq,
  NetPlatform::MCastAddrInfoType*  pMCastAddrInfo 
);

/*---------------------------------------------------------------------------
  Misc conversion routines
---------------------------------------------------------------------------*/
/*!
@brief
Converts an BCMCS update parameter from ds to PS format.

@param[in]  pIBCMCSUpdParam - Interface that holds BCMCS update parameter.
@param[out] pPSNetBCMCSUpdParam - BCMCS update parameter in PS format.

@see        Interface IBCMCSDBSpec
@see        NetPlatform::PSBCMCSDbUpdateType
@return     QDS_EFAULT - Invalid input.
@return     AEE_SUCCESS - On success.
*/
ds::ErrorType DS2PSBCMCSSpec
(
  const BCMCSDBSpecType                  *pIBCMCSUpdParam,
  NetPlatform::PSBCMCSDbUpdateType *pPSNetBCMCSUpdParam
);

/*!
  @brief
  Converts an IPv6 prefix from PS to ds format.

  @param[in]  pPSIPv6Prefix - PS format IPv6 prefix.
  @param[out] pDSIPv6Prefix - ds format IPv6 prefix.
  
  @see        NetPlatform::IPv6PrefixInfoType
  @see        ds::Net::NetworkIPv6Priv::PrefixIpv6InfoType
  @return     None.
*/
#endif // FEATUTE_DATA_PS_MCAST
void PS2DSIPv6Prefix
(
  NetPlatform::IPv6PrefixInfoType *           pPSIPv6Prefix,
  IPv6PrivPrefixInfoType *   pDSIPv6Prefix 
);

/*!
  @brief
  Converts an iface state from PS layer format to ds layer format.

  @param[in]  ifaceState - Iface state in PS layer format.
  
  @see        NetPlatform::IfaceStateEnumType
  @see        ds::NetworkStateType
  @return     The iface state in ds layer format.
  @return     ds::NetworkState::INVALID_STATE on invalid input.
*/
ds::ErrorType PS2DSIfaceState 
(
  NetPlatform::IfaceStateEnumType         ifaceState,
  ds::Net::NetworkStateType*              networkState 
);

/*!
  @brief
  Converts an phys link state from PS layer format to its matching event

  @param[in]  psPhysLinkState - Phys link state in PS layer format.
  @param[out] ps_iface_event_enum_type - Phys link event.
  
  @see        NetPlatform::PSPhysLinkStateType
  @see        ps_iface_defs.h
  @return     Returned phys link event.
*/
ps_iface_event_enum_type PS2PSPhysLinkStateToEvent
(
  NetPlatform::PSPhysLinkStateType   psPhysLinkState
);

/*!
  @brief
  Converts an phys link state from PS layer format to ds layer format.

  @param[in]  psPhysLinkState - Phys link state in PS layer format.
  @param[out] pDSPhysLinkState - Phys link state in ds layer format.
  
  @see        NetPlatform::PSPhysLinkStateType
  @see        ds::Net::PhysLinkStateType
  @return     Returned phys link state.
*/
ds::Net::PhysLinkStateType PS2DSPhysLinkState
(
  NetPlatform::PSPhysLinkStateType   psPhysLinkState
);

/*!
  @brief
  Converts an flow state from PS layer format to ds layer format.

  @param[in]  flowState - PS Flow state in PS layer format.
  
  @see        NetPlatform::PSFlowStateType
  @see        ds::NetQoSSecondary::StatusType
  @return     The iface state in ds layer format.
  @return     ds::Net::QoSState::UNAVAILABLE when the state is 
              down or invalid input
*/
ds::Net::QoSStateType PS2DSFlowState
(
  NetPlatform::PSFlowStateType            flowState
);

/*!
  @brief
  Converts an event name from ps_iface_event_enum_type to event name
  and event group in the ds NET format.

  @param[in]  psEventName - Event name in PS iface event name format.
  @param[in]  pPSEventInfo - PS event info associated with the event. 
  @param[out] pDSEventName - ds event name.
  @param[out] pDSUserHandle - User handle (ex: mcast handle)
  @param[out] pDSEventGroup - ds event group.
  
  @see        ps_iface_event_enum_type
  @see        ps_iface_event_info_u_type
  @see        Different event names in ds Net layer. These are defined
              in various interfaces the events belong to.
  @see        ds::Net::EventGroupType 
  @return     AEE_SUCCESS - On success
  @return     QDS_EFAULT - invalid paramters.
  @return     QDS_EINVAL - Invalid/ignoring PS event.
*/
ds::ErrorType PS2DSEventInfo
(
  int32                                  psEventName,
  NetPlatform::IfaceEventInfoUnionType  *pPSEventInfo,
  int32                                 *pDSEventName,
  int32                                 *pDSUserHandle,
  ds::Net::EventGroupType               *pDSEventGroup
);
#ifdef FEATUTE_DATA_PS_MCAST
/*!
@brief
Converts Mcast info code from BCMCS 1.0 version to BCMCS 2.0 version.

@param[in]  oldCode - MCast info code in BCMCS 1.0 format.
@param[out]  newCode - MCast info code in BCMCS 2.0 format. 

@see        ps_iface_mcast_info_code_enum_type

@return     AEE_SUCCESS - On success
@return     QDS_EFAULT - invalid parameters.
@return     QDS_EINVAL - no mapping from 1.0 to 2.0 exists
*/
ds::ErrorType MCastInfoCodeBCMCS1_0To2_0
(
  ps_iface_mcast_info_code_enum_type   oldCode,
  ps_iface_mcast_info_code_enum_type * newCode
);

/*!
@brief
Checks if provided Mcast info code is related to BCMCS 1.0 version.

@param[in]  infoCode - MCast info code to check.

@see        ps_iface_mcast_info_code_enum_type

@return     TRUE - info code is related to BCMCS 1_0
@return     FALSE - otherwise.
*/
boolean MCastInfoCodeIsBCMCS1_0
(
  ps_iface_mcast_info_code_enum_type   infoCode
);

/*!
@brief
Checks if provided Mcast info code is related to DVBH or MFLO.

@param[in]  infoCode - MCast info code to check.

@see        ps_iface_mcast_info_code_enum_type

@return     TRUE - info code is related to DVBH or MFLO
@return     FALSE - otherwise.
*/
boolean MCastInfoCodeIsDVBHOrMFLO
(
  ps_iface_mcast_info_code_enum_type   infoCode
);
#endif // FEATUTE_DATA_PS_MCAST
/*!
@brief
Converts socket address family from ds to PS. 

@param[in]  dsFamily - DS layer socket address family type.
@param[out] ipFamily - PS layer socket address family type.

@return     AEE_SUCCESS - On success
@return     QDS_EFAULT - invalid parameters.
*/
ds::ErrorType DS2PSSockAddrFamily
(
  ds::AddrFamilyType dsFamily,
  uint16             * ipFamily
);

/*!
@brief
Converts ip_filter_spec_type from ds to PS. 

@param[in]  psFilterSpec   - PS layer ip_filter_spec_type.
@param[out] dsFilters      - DS layer ip_filter_spec_type.
@param[in]  dsFilterLen    - DS layer ip_filter_spec length.
@param[out] dsFilterLenReq - DS layer ip_filter_spec_type length requried.

@return     AEE_SUCCESS - On success
@return     QDS_EFAULT - invalid parameters.
*/
ds::ErrorType PS2DSIPFilterSpecType
(
  ip_filter_spec_type* psFilterSpec,
  IPFilterSpec**       dsFilters,
  int                  dsFilterLen,
  int*                 dsFilterLenReq
);

/*!
@brief
Converts iface group name from dsnet to PS. 

@param[in]  IfaceGroupType   - ds_net layer iface group.
@param[out] iface_group - ps_iface layer iface group

@return     AEE_SUCCESS - On success
@return     QDS_EFAULT - invalid parameters.
*/
ds::ErrorType DS2PSIFaceNameType
(
  IfaceGroupType                     ifaceGroup,
  NetPlatform::PSIFaceNameEnumType * iface_group
);

} /* namespace Conversion */
} /* namespace Net */
} /* namespace ds */

#endif /* DS_NET_CONVERSION_H */

