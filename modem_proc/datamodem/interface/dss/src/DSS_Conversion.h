#ifndef __DSS_CONVERSION_H__
#define __DSS_CONVERSION_H__

/*===================================================

FILE:  DSS_Conversion.h

SERVICES:
   Functions to convert between ds and IDS types.

=====================================================

Copyright (c) 2008-2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_Conversion.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-11-01 rk  Removed feature mobileap.
  2011-11-27 ss  Added IOCTLs to enable/disable and retrieve ALG status list.  
  2010-04-13 en  History added.

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "dssocket.h"
#include "dserrno.h"

#include "ds_Sock_Def.h"
#include "ds_Sock_ISocketExt.h"
#include "ds_Sock_ISocketLocalPriv.h"
#include "ds_Net_QoS_Def.h"
#include "ds_Net_QoS_DefPriv.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_INetwork.h"
#include "ds_Net_INetworkExt.h"
#include "ds_Net_INetworkExt2.h"
#include "ds_Net_INetwork1xPriv.h"
#include "ds_Net_IIPv6Address.h"
#include "dss_iface_ioctl.h"
#include "ds_Addr_Def.h"
#include "ds_Net_IPolicy.h"
#include "ds_Net_IPolicyPriv.h"
#include "ds_Net_IMCastManager.h"
#include "ds_Net_IMCastManagerBCMCS.h"
#include "ds_Net_IMCastManagerExt.h"
#include "ds_Net_IMCastManagerPriv.h"

// All non-public interface conversions should be added here
typedef enum
{
   DSS_IFACE_STA_IFACE = 0x8200  //
} dss_iface_name_enum_type_internal;

namespace DSSConversion
{
   // Translate an AEEResult value into a ds errno value.
   sint15 IDS2DSErrorCode(AEEResult nRes);
   // Translate an AEEResult value into a ds errno value.
   // AEEResult DS2IDSErrorCode(sint15 nRes); Ebrahem
   // Translate a dss_net_policy_info_type into an IDSNetPolicy value.
   AEEResult DS2IDSNetPolicy(const dss_net_policy_info_type* from, ds::Net::IPolicyPriv *to);
   // Translate a dss_net_policy_info_type into an IDSNetPolicy value.
   AEEResult DS2IDSNetPolicy(const dss_net_policy_info_type* from, ds::Net::IPolicy *to);
   // Translate a IDSNetPolicy into a dss_net_policy_info_type value.
   // appid is provided in order to incorporate it into the iface_id in the policy (if applicable).
   // If appid is not available -1 should be specified.
   AEEResult IDS2DSNetPolicy(ds::Net::IPolicy *from, dss_net_policy_info_type *to, sint15 appid);
   // Translate a dss_iface_policy_flags_enum_type into a ds::Policy::FlagType value.
   AEEResult DS2IDSPolicyFlag(dss_iface_policy_flags_enum_type DSFlag, ds::Net::PolicyFlagType* IDSFlag);
   // Translate a ds::Policy::FlagType into a dss_iface_policy_flags_enum_type value.
   AEEResult IDS2DSPolicyFlag(ds::Net::PolicyFlagType IDSPolicyFlag, dss_iface_policy_flags_enum_type* DSFlag);

   // Translate a dss_iface_name_enum_type into a ds::Net::IfaceNameType value
   AEEResult DS2IDSIfaceName(dss_iface_name_enum_type DSIfaceName, ds::Net::IfaceNameType* IDSIfaceName);

   // Translate a dss_iface_name_enum_type_internal into a ds::Net::IfaceNameType value
   // This function does additional conversions of non-public interfaces not covered by DS2IDSIfaceName
   AEEResult DS2IDSIfaceNameInternal(dss_iface_name_enum_type_internal DSIfaceName, ds::Net::IfaceNameType* IDSIfaceName);


   AEEResult DS2IDSIfaceGroup(dss_iface_name_enum_type DSIfaceGroup, ds::Net::IfaceGroupType* IDSIfaceGroup);

   // Translate an int value into a ds::AddrFamilyType value.
   AEEResult DS2IDSAddrFamily(const int DSFamily,ds::AddrFamilyType* AddrFamily );
   // Translate a ds::AddrFamilyType into an int value.
   AEEResult IDS2DSAddrFamily(const ds::AddrFamilyType IDSFamily, int* DSFamily);
   // Translate a ds::AddrFamilyType into an ip_addr_enum_type value.
   AEEResult IDS2DSEnumAddrFamily(ds::AddrFamilyType idsFamily, ip_addr_enum_type* DSFamily);
   // Translate an ip_addr_enum_type into a ds::AddrFamilyType value.
   AEEResult DS2IDSEnumAddrFamily(ip_addr_enum_type dsFamily, ds::AddrFamilyType* idsFamily);
   // Translate an int value into a ds::SocketType value.
   AEEResult DS2IDSSockType(const int DSSockType, ds::Sock::SocketType *IDSSockType);
   // Translate an int value into a ds::ProtocolType value.
   AEEResult DS2IDSProtocol(const int DSProtocol, ds::Sock::ProtocolType *IDSProtocol);
   // Translate an int value into a ds::EventType value.
   AEEResult DS2IDSEventId(const int DSEventId , ds::Sock::SocketEventType* EventID);
   // Translate a dss_net_policy_info_type into a ds::Sock::OptLevelType value.
   AEEResult DS2IDSOptLevel(const dss_sockopt_levels_type DSOptLevel, ds::Sock::OptLevelType* OptLevel);
   // Translate a dss_sockopt_names_type into a ds::Sock::OptNameType value.
   AEEResult DS2IDSOptName(const dss_sockopt_names_type DSOptName, ds::Sock::OptNameType* OptName);
   // Translate a dss_sockopt_names_type into a ds::Sock::OptNameType value.
   AEEResult DS2IDSOptNamePriv(const dss_sockopt_names_type DSOptName, ds::Sock::OptNameTypePriv* OptName);
   // Translate a dss_so_linger_type into a IDSSock::LingerType value.
   AEEResult DS2IDSLinger(const dss_so_linger_type *from, ds::Sock::LingerType *to);
   // Translate a IDSSock::LingerType into a dss_so_linger_type value.
   AEEResult IDS2DSLinger(const ds::Sock::LingerType *from, dss_so_linger_type *to);
   // Translate an IDSNetBearerTech into a dss_iface_ioctl_bearer_tech_type value.
   AEEResult IDS2DSBearerInfo(ds::Net::IBearerInfo* piBearerInfo, dss_iface_ioctl_bearer_tech_type* pdsBearerTech);
   // Translate an IDS2DSBearerTechEx into a dss_iface_ioctl_bearer_tech_ex_type value.
   AEEResult IDS2DSBearerTechEx(ds::Net::IBearerTechEx* piBearerTechEx, dss_iface_ioctl_bearer_tech_ex_type* pdsBearerTechEx);
   // Translate a NetworkOpReservedPcoType into a dss_iface_op_reserved_pco_type.
   AEEResult IDS2DSOpResPco(ds::Net::NetworkOpReservedPcoType* popReservedPco, dss_iface_op_reserved_pco_type* pGetOpResPco);
   //Translate an IDS2DSApnParamInfo into a dss_iface_ioctl_apn_param_change_list_type value.
   AEEResult IDS2DSApnParamInfo(ds::Net::ApnParamChangeList* pApnParamInfo, dss_iface_ioctl_changed_apn_param_info_type* pdsApnParamInfo);
   // Translate a DataPathBridgeModeType into a dss_iface_ioctl_data_path_bridge_mode_type.
   AEEResult IDS2DSDataPath(ds::Net::DataPathBridgeModeType* pIDSDataPath, dss_iface_ioctl_data_path_bridge_mode_type* pGetDataPath);
   // Translate a dss_iface_ioctl_data_path_bridge_mode_type into a DataPathBridgeModeType.
   AEEResult DS2IDSDataPath(dss_iface_ioctl_data_path_bridge_mode_type* pDsDataPath, ds::Net::DataPathBridgeModeType* pIDSDataPath);
   // Translate a _3GPPLTEReestabCalltypeStruct into a dss_iface_ioctl_3gpp_get_lte_reestab_calltype_type
   AEEResult IDS2DS3GPPLTEReestabCalltype(ds::Net::_3GPPLTEReestabCalltypeStruct* p3GPPLTEReestabCalltype,
                                          dss_iface_ioctl_3gpp_get_lte_reestab_calltype_type* pDss3GPPLTEReestabCalltype);
   // Translate a dss_iface_ioctl_3gpp_set_lte_reestab_calltype_type into a _3GPPLTEReestabCalltypeStruct
   AEEResult DS2IDS3GPPLTEReestabCalltype(dss_iface_ioctl_3gpp_set_lte_reestab_calltype_type* pDss3GPPLTEReestabCalltype,
                                          ds::Net::_3GPPLTEReestabCalltypeStruct* p3GPPLTEReestabCalltype);
   // Bharath Translate a dss_iface_ioctl_set_silent_redial_type into a _3GPPSetSilentRedialStruct
   AEEResult DS2IDSSilentRedial(dss_iface_ioctl_set_silent_redial_type* pDssSilentRedial,
                                ds::Net::SilentRedialStruct* pSilentRedial);
   // Translate a ds::IPAddrType into an ip_addr_type value.
   AEEResult IDS2DSIpAddr(const ds::IPAddrType* idsAddr, ip_addr_type* dsAddr);
   // Translate an ip_addr_type into a ds::IPAddrType value.
   AEEResult DS2IDSIpAddr(const ip_addr_type* dsAddr, ds::IPAddrType* idsAddr);
   // Translate a ds::INAddr6Type into an ip_addr_type value.
   AEEResult IDS2DSIp6Addr(const ds::INAddr6Type idsAddr, ip_addr_type* dsAddr);
   // Translate an ip_addr_type into a ds::IPAddr6Type value.
   AEEResult DS2IDSIp6Addr(const ip_addr_type* dsAddr, ds::INAddr6Type idsAddr);
   // Translate ip_addr_type into ds::Sock::SockAddrStorageType
   AEEResult DS2IDSSockIpAddr(const ip_addr_type* dsAddr, ds::SockAddrStorageType idsAddr);
#ifdef FEATUTE_DATA_PS_MCAST
   // Translate a dss_iface_ioctl_mcast_addr_info_type array into a SeqSockAddrStorageType
   AEEResult DS2IDSMCastIpAddrSeq(ds::Net::IMCastManagerExt::SeqSockAddrStorageType* addrSeq,
                                  dss_iface_ioctl_mcast_addr_info_type*  mcast_addr_info_array);
#endif // FEATUTE_DATA_PS_MCAST
   AEEResult DS2IDSQoSNetInitiatedSpec(net_initiated_qos_spec_type* reqType, ds::Net::QoSSpecNetInitiatedType* specType);
   void FreeQoSNetInitiatedMemOnError(ds::Net::QoSSpecNetInitiatedType* specType);
   // Translate a PDNTeardownConfigType into a dss_iface_ioctl_pdn_teardown_config_type.
   AEEResult IDS2DSPDNTeardown(ds::Net::PDNTeardownConfigType* pIDSPDNTeardown, dss_iface_ioctl_pdn_teardown_config_type* pDSPDNTeardownConfig);
   // Translate a dss_iface_ioctl_pdn_teardown_config_type into a PDNTeardownConfigType.
   AEEResult DS2IDSPDNTeardown(dss_iface_ioctl_pdn_teardown_config_type* pDSPDNTeardownConfig, ds::Net::PDNTeardownConfigType* pIDSPDNTeardown);
   // Translate a ds::Network::DomainName into a dss_iface_ioctl_domain_name_type.
   AEEResult IDS2DSDomainName(const ds::Net::DomainName* idsDomainName, dss_iface_ioctl_domain_name_type* dsDomainName);
   // Translate an array of a certain type from IDS to ds.
   template<typename IDSType, typename DSType>
   AEEResult IDS2DSArray(const IDSType* fromArr, DSType* toArr, int nLen,
                         AEEResult (*pfnIDS2DSItem)(const IDSType*, DSType*))
   {
      for (int i = 0; i < nLen; i++) {
         AEEResult res = pfnIDS2DSItem(&fromArr[i], &toArr[i]);
         if (AEE_SUCCESS != res) {
            return res;
         }
      }
      return AEE_SUCCESS;
   }

   // Translate a ds::Network::DownReasonType into a dss_net_down_reason_type value.
   AEEResult IDS2DSNetworkDownReason(const ds::Net::NetDownReasonType from, dss_net_down_reason_type *to);

   // Translate a ds::Network::StateType into a dss_iface_ioctl_state_type value.
   AEEResult IDS2DSIfaceState(const ds::Net::NetworkStateType from, dss_iface_ioctl_state_type *to);

   AEEResult IDS2DSPhyslinkState(const ds::Net::PhysLinkStateType from,
                              dss_iface_ioctl_phys_link_state_type *to);
   AEEResult DS2IDSPhyslinkState(const dss_iface_ioctl_phys_link_state_type from,
                              ds::Net::PhysLinkStateType *to);

   /* QOS spec conversion includes the conversion of an applicable QOS SPEC mask (the mask is needed for
      QOS Modify operations).
      For QoS request we don't need it but the function will still create it.*/
   AEEResult DS2IDSQoSSpec(dss_iface_ioctl_qos_request_type* reqType, 
                           ds::Net::QoSSpecType* specType,
                           boolean isModify,
                           ds::Net::QoSModifyMaskType *modifyMask);

   /* QOS spec conversion includes the conversion of an applicable QOS SPEC mask (the mask is needed for
   QOS Modify operations).
   For QoS request we don't need it but the function will still create it.*/
   AEEResult DS2IDSQoSSpecPrimary(dss_iface_ioctl_primary_qos_modify_type* reqType, 
                                  ds::Net::QoSSpecPrimaryType* specType,
                                  boolean isModify,
                                  ds::Net::QoSModifyMaskType *modifyMask);

   // Translate just the error masks, disregard all the other info
   AEEResult IDS2DSQoSSpecErrMask(ds::Net::QoSSpecType* specType,dss_iface_ioctl_qos_request_type* reqType);
   AEEResult IDS2DSQoSSpecPrimaryErrMask(ds::Net::QoSSpecPrimaryType* specType,dss_iface_ioctl_primary_qos_modify_type* reqType);
   AEEResult IDS2DSQoSSpecFlowsErrMask(ds::Net::IQoSFlowPriv** ppIDSNetQoSFlow,boolean min,int len,ip_flow_spec_type* flow);
   AEEResult IDS2DSQoSSpecFiltersErrMask(ds::Net::IIPFilterPriv** ppIDSNetIPFilter,int len,ip_filter_spec_type* filter);
   AEEResult IDS2DSQoSSpecFlowErrMask(ds::Net::IQoSFlowPriv* pIDSNetQoSFlow,ip_flow_type* flow);
   AEEResult IDS2DSQoSSpecFilterErrMask(ds::Net::IIPFilterPriv* pIDSNetIPFilter,ip_filter_type* filter);

   AEEResult DS2IDSQoSSpecBundle(dss_iface_ioctl_qos_request_ex_type* qos_request_ex, ds::Net::SeqQoSSpecType requestedQoSSpecs);
   AEEResult IDS2DSQoSSpecBundleErrMask(ds::Net::SeqQoSSpecType* requestedQoSSpecs,dss_iface_ioctl_qos_request_ex_type* qos_request_ex);

   AEEResult IDS2DSQoSSpec(const ds::Net::QoSSpecType* specType, dss_iface_ioctl_qos_request_type* reqType);

   AEEResult DS2IDSQoSSpecFlows(ip_flow_spec_type* ipFlowSpecType,
                                ds::Net::IQoSFlowPriv*** flows,
                                int* flowsLen,
                                int* flowsLenReq,
                                boolean* minFlow,
                                qos_spec_field_mask_type field_mask,
                                bool isTxFlow);
   AEEResult DS2IDSQoSSpecFlow(ip_flow_type* ipFlowType, ds::Net::IQoSFlowPriv* QoSFlow);

   AEEResult DS2IDSQoSMask(qos_spec_field_mask_type qosMaskType, ds::Net::QoSModifyMaskType* modifyMask);

   AEEResult IDS2DSQoSMask(qos_spec_field_mask_type* qosMaskType, ds::Net::QoSModifyMaskType modifyMask);

   AEEResult DS2IDSIPFilter(ip_filter_type* ipFilterType, ds::Net::IIPFilterPriv* NetIPFilter);

   AEEResult IDS2DSQoSSpecFlow(ds::Net::IQoSFlowPriv* QoSFlow, ip_flow_type* ipFlowType);

   AEEResult IDS2DSIPFilterSpec(ds::Net::IIPFilterPriv* Filter, ip_filter_type* ipFilterType);
#ifdef FEATUTE_DATA_PS_MCAST
   AEEResult DS2IDSMCast(dss_iface_ioctl_mcast_join_type* mcast_join,
                         ds::SockAddrStorageType addr,
                         ds::Net::IMCastManagerPriv* pNetMCastManagerPriv,
                         IQI** info);

   AEEResult DS2IDSMCastBundle(dss_iface_ioctl_mcast_join_ex_type* mcast_join_ex,
                               ds::SockAddrStorageType* addrSeq,
                               ds::Net::MCastJoinFlagsType * mcastFlags,
                               ds::Net::IMCastManagerPriv* pNetMCastManagerPriv,
                               IQI** info);

   AEEResult IDS2DSMCast(IQI* info,
                         dss_iface_ioctl_mbms_mcast_param_type* mbms_mcast_param_ptr);

   AEEResult IDS2DSMCastJoinEx( const ds::SockAddrStorageType* addrSeq,
                                IQI* info,
                                dss_iface_ioctl_mcast_join_ex_type* mcast_Control_Bundle );

   AEEResult IDS2DSMCastJoin( const ds::SockAddrStorageType addr,
                              IQI* info,
                              dss_iface_ioctl_mcast_join_type* mcast_join );
#endif // FEATUTE_DATA_PS_MCAST

   // Translate a ds::Network::RFConditionType into a ps_iface_rf_conditions_enum_type.
   AEEResult IDS2DSRFCondition(ds::Net::RFConditionType idsCond, ps_iface_rf_conditions_enum_type* pdsCond);

   // Translate a dss_session_timer_select_enum_type into a
   // ds::Network1xPriv::SessionTimer::SelectType.
   AEEResult DS2IDSSessionTimerSelect(dss_session_timer_select_enum_type dsSelect,
                                      ds::Net::Network1xPrivSessionTimerSelectType* idsSelect);

   // Translate an SDB flag mask from the C API into the IDL API.
   int DS2IDSSDBFlags(uint32 dsFlags);

   // Translate a ds::Net::DormancyInfoCodeType into a dss_iface_ioctl_dormancy_info_code_enum_type.
   AEEResult IDS2DSDormancyInfoCode(ds::Net::DormancyInfoCodeType dormInfoCode,
                                    dss_iface_ioctl_dormancy_info_code_enum_type* DSDormInfoCode);

   // Translate a ds::Net::QoSModeType into a dss_iface_ioctl_qos_mode_type.
   AEEResult IDS2DSQoSMode(ds::Net::QoSModeType qosMode,
      dss_iface_ioctl_qos_mode_type* DSQoSMode);

   // Translate a ds::Sock::DoSAckStatusType into a dss_sdb_ack_status_info_type.
   AEEResult IDS2DSSDBAckStatus(ds::Sock::DoSAckStatusType status, dss_sdb_ack_status_enum_type* DSStatus);

   // Translate a ::ds::NetQoS::InfoCodeType into a dss_iface_ioctl_extended_info_code_enum_type.
   AEEResult IDS2DSQoSAwareInfoCode(ds::Net::QoSInfoCodeType infoCode,
                                    dss_iface_ioctl_extended_info_code_enum_type* DSInfoCode);

   // Translate a ::ds::NetQoS::InfoCodeType into a dss_iface_ioctl_extended_info_code_enum_type.
   AEEResult IDS2DSQoSInfoCode(ds::Net::QoSInfoCodeType infoCode,
                               dss_iface_ioctl_extended_info_code_enum_type* DSInfoCode);
   
   // Translate a ::ds::Network1xPriv::HDRRev0RateInertia::FailureCodeType into a dss_iface_ioctl_extended_info_code_enum_type.
   AEEResult IDS2DSInertiaFailureInfoCode(ds::Net::Network1xPrivHDRRev0RateInertiaFailureCodeType infoCode,
                                          dss_iface_ioctl_hdr_rev0_rate_inertia_failure_code_enum_type* DSInfoCode);
#ifdef FEATUTE_DATA_PS_MCAST
   //Translate a dss_iface_ioctl_mcast_info_code_enum_type into a ::ds::NetMCast::InfoCodeType
   AEEResult DS2IDSMCastStatus(dss_iface_ioctl_mcast_info_code_enum_type regStatus,
                               ds::Net::MCastInfoCodeType* infoCode);
#endif // FEATUTE_DATA_PS_MCAST
   AEEResult IDS2DSIPv6PrefixState(ds::Net::IPv6AddrStateType netIPv6PrefState,
                                   ps_iface_ipv6_addr_state_enum_type *prefStatus);

   AEEResult IDS2DSRcvMsgFlag(unsigned int idsRcvMsgFlags, unsigned int *dsRcvMsgFlags);

   void FreeQoSMemOnError(ds::Net::QoSSpecType* specType);

   AEEResult IDS2DSStatistics(const ds::Net::NetworkStatsType* statsType, 
                              dss_iface_ioctl_get_iface_stats_type* stats);

   AEEResult DS2IDSFMCTunnelParams(dss_iface_ioctl_uw_fmc_tunnel_params_type * pDSFMCTunnelParams,
                                   ds::Net::FMCTunnelParamsType * pIDSFMCTunnelParams);

   AEEResult DS2IDSSockAddrType(struct ps_sockaddr *pDSSockddrType, 
                               ds::SockAddrStorageType pIDSSockAddr,
                               uint16 addrLen);

   dss_iface_ioctl_event_enum_type NetState2Event(ds::Net::NetworkStateType netState);

   dss_iface_ioctl_event_info_type NetState2IfaceState(ds::Net::NetworkStateType netState);

   ps_iface_state_enum_type Event2IfaceState(dss_iface_ioctl_event_enum_type eventId);

   int32 Event2DsnetEvent(dss_iface_ioctl_event_enum_type eventId);

   dss_iface_ioctl_event_enum_type PhysLinkState2Event(ds::Net::PhysLinkStateType physState);

   dss_iface_ioctl_phys_link_event_info_type IDS2DSPhysLinkState(ds::Net::PhysLinkStateType physState);

   dss_iface_ioctl_event_enum_type QosAware2Event(boolean qosAware);
}

#endif // __DSS_CONVERSION_H__
