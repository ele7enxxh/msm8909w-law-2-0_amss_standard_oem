#ifndef NET_PLATFORM_LIB_H
#define NET_PLATFORM_LIB_H

/*===========================================================================
  @file NetPlatform.h

  This file defines the platform layer for the network library.

  TODO: Detailed explanation about the class here.

  Copyright (c) 2008-2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/inc/ds_Net_Platform.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-02-04 svj New DIAG event to generate data cal status information
  2011-11-27 ss  IOCTLs to enable/disable and retrieve ALG status list.
  2011-08-31 hs  Added IFACE_IOCTL_GET_SUBNET_MASK.
  2011-08-30 hs  Added IFACE_IOCTL_GET_GATEWAY_ADDR.
  2011-08-24 hs  Added IFACE_IOCTL_GET_DEVICE_NAME.
  2010-06-30 cp  IOCTL changes for Soft AP DHCP server.
  2008-03-10 hm  Created module.

===========================================================================*/
/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_IPolicy.h"
#include "ds_Net_INetwork.h"
#include "ds_Net_INetworkPriv.h"
#include "ds_Utils_StdErr.h"
#include "ds_Errors_Def.h"
#include "dss_iface_ioctl.h"
#include "ps_in.h"
#include "ps_acl.h"
#include "ps_iface_defs.h"
#include "ps_routei.h"
#include "ps_in.h"
#include "ps_route_scope.h"
#include "ps_rt_meta_info.h"
#include "ps_iface_ipfltr.h"
#include "ps_iface_defs.h"
#include "ps_iface_ioctl.h"
#include "ps_flow_ioctl.h"
#include "ps_qos_defs.h"
#include "ps_phys_link_ioctl.h"
#include "ps_ifacei.h"
#include "ps_qos_net_initiated_req.h"

/*===========================================================================

                            PUBLIC DATA DECLARATIONS

===========================================================================*/

namespace NetPlatform
{

  /*-------------------------------------------------------------------------
    IOCTL names enum declarations.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
                                    IFACE IOCTLS
  -------------------------------------------------------------------------*/
  typedef enum
  {
    IFACE_IOCTL_MIN,
    /*-----------------------------------------------------------------------
                                   COMMON IOCTLS
    -----------------------------------------------------------------------*/
    IFACE_IOCTL_GET_ALL_IFACES
      = PS_IFACE_IOCTL_GET_ALL_IFACES,
    IFACE_IOCTL_GET_IP_ADDR
      = PS_IFACE_IOCTL_GET_IP_ADDR,
    IFACE_IOCTL_GET_ALL_DNS_ADDRS
      = PS_IFACE_IOCTL_GET_ALL_DNS_ADDRS,
    IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR
      = PS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR,
    IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR
      = PS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR,
    IFACE_IOCTL_GET_IPV6_PRIM_DNS_ADDR
      = PS_IFACE_IOCTL_GET_IPV6_PRIM_DNS_ADDR,
    IFACE_IOCTL_GET_IPV6_SECO_DNS_ADDR
      = PS_IFACE_IOCTL_GET_IPV6_SECO_DNS_ADDR,
    IFACE_IOCTL_GET_MTU
      = PS_IFACE_IOCTL_GET_MTU,
    IFACE_IOCTL_GET_STATE
      = PS_IFACE_IOCTL_GET_STATE,
    IFACE_IOCTL_GET_HW_ADDR
      = PS_IFACE_IOCTL_GET_HW_ADDR,
    IFACE_IOCTL_GET_IFACE_NAME
      = PS_IFACE_IOCTL_GET_IFACE_NAME,
    IFACE_IOCTL_GET_IP_FAMILY
      = PS_IFACE_IOCTL_GET_IP_FAMILY,
    IFACE_IOCTL_GET_BEARER_TECHNOLOGY
      = PS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY,
    IFACE_IOCTL_GET_BEARER_TECHNOLOGY_EX
      = PS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY_EX,
    IFACE_IOCTL_GET_DATA_BEARER_RATE
      = PS_IFACE_IOCTL_GET_DATA_BEARER_RATE,
      IFACE_IOCTL_GET_OP_RES_PCO
      = PS_IFACE_IOCTL_GET_OP_RES_PCO,
    IFACE_IOCTL_GET_DATA_PATH
      = PS_IFACE_IOCTL_GET_DATA_PATH_BRIDGE_MODE,
    IFACE_IOCTL_SET_DATA_PATH_BRIDGE_MODE
      = PS_IFACE_IOCTL_SET_DATA_PATH_BRIDGE_MODE,
    IFACE_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE
      = PS_IFACE_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE,
    IFACE_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE
      = PS_IFACE_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE,
    IFACE_IOCTL_GET_CHANGED_APN_PARAM_INFO
      = PS_IFACE_IOCTL_GET_CHANGED_APN_PARAM_INFO,
    IFACE_IOCTL_SET_SILENT_REDIAL
      = PS_IFACE_IOCTL_SET_SILENT_REDIAL,
    IFACE_IOCTL_3GPP_FORCE_PDN_DISCONNECT
      = PS_IFACE_IOCTL_3GPP_FORCE_PDN_DISCONNECT,
    IFACE_IOCTL_IS_LAPTOP_CALL_ACTIVE
      = PS_IFACE_IOCTL_IS_LAPTOP_CALL_ACTIVE,
    IFACE_IOCTL_GET_RF_CONDITIONS
      = PS_IFACE_IOCTL_GET_RF_CONDITIONS,
    IFACE_IOCTL_IPCP_DNS_OPT 
      = PS_IFACE_IOCTL_IPCP_DNS_OPT,
    IFACE_IOCTL_ON_QOS_AWARE_SYSTEM
      = PS_IFACE_IOCTL_ON_QOS_AWARE_SYSTEM,
    IFACE_IOCTL_GO_NULL
      = PS_IFACE_IOCTL_GO_NULL,
    IFACE_IOCTL_ENABLE_FIREWALL
      = PS_IFACE_IOCTL_ENABLE_FIREWALL,
    IFACE_IOCTL_DISABLE_FIREWALL
      = PS_IFACE_IOCTL_DISABLE_FIREWALL,
    IFACE_IOCTL_ADD_FIREWALL_RULE
      = PS_IFACE_IOCTL_ADD_FIREWALL_RULE,
    IFACE_IOCTL_DELETE_FIREWALL_RULE
      = PS_IFACE_IOCTL_DELETE_FIREWALL_RULE,
    IFACE_IOCTL_GET_FIREWALL_RULE
      = PS_IFACE_IOCTL_GET_FIREWALL_RULE,
    IFACE_IOCTL_GET_FIREWALL_TABLE
      = PS_IFACE_IOCTL_GET_FIREWALL_TABLE,
    IFACE_IOCTL_GET_DEVICE_NAME
      = PS_IFACE_IOCTL_GET_DEVICE_NAME,
    IFACE_IOCTL_GET_GATEWAY_ADDR
      = PS_IFACE_IOCTL_GET_GATEWAY_ADDR,
    IFACE_IOCTL_GET_SUBNET_MASK
      = PS_IFACE_IOCTL_GET_SUBNET_MASK,
    IFACE_IOCTL_KEEP_ALIVE
      = PS_IFACE_IOCTL_KEEP_ALIVE,
    IFACE_IOCTL_SET_PDN_TEARDOWN_CONFIG
      = PS_IFACE_IOCTL_SET_PDN_TEARDOWN_CONFIG,      
    IFACE_IOCTL_GET_PDN_TEARDOWN_CONFIG
      = PS_IFACE_IOCTL_GET_PDN_TEARDOWN_CONFIG,        
    IFACE_IOCTL_FILTER_MATCH_REQUEST
      = PS_IFACE_IOCTL_FILTER_MATCH_REQUEST,  

    /*-----------------------------------------------------------------------
                               COMMON ENHANCED IOCTLS
    -----------------------------------------------------------------------*/
    IFACE_IOCTL_GET_SIP_SERV_ADDR
      = PS_IFACE_IOCTL_GET_SIP_SERV_ADDR,
    IFACE_IOCTL_GET_SIP_SERV_DOMAIN_NAMES
      = PS_IFACE_IOCTL_GET_SIP_SERV_DOMAIN_NAMES,
    IFACE_IOCTL_GET_DOMAIN_NAME_SEARCH_LIST
      = PS_IFACE_IOCTL_GET_DOMAIN_NAME_SEARCH_LIST,
    IFACE_IOCTL_REFRESH_DHCP_CONFIG_INFO
      = PS_IFACE_IOCTL_REFRESH_DHCP_CONFIG_INFO,

    /*-----------------------------------------------------------------------
                                     QOS IOCTLS
    -----------------------------------------------------------------------*/
    IFACE_IOCTL_QOS_REQUEST
      = PS_IFACE_IOCTL_QOS_REQUEST,
    IFACE_IOCTL_QOS_REQUEST_EX
      = PS_IFACE_IOCTL_QOS_REQUEST_EX,
    IFACE_IOCTL_QOS_NET_INITIATED_REQUEST
      = PS_IFACE_IOCTL_QOS_NET_INITIATED_REQUEST,
    IFACE_IOCTL_QOS_NET_INITIATED_RELEASE
      = PS_IFACE_IOCTL_QOS_NET_INITIATED_RELEASE,
    IFACE_IOCTL_QOS_NET_INITIATED_REAL_TIME_CHECK
      = PS_IFACE_IOCTL_QOS_NET_INITIATED_REAL_TIME_CHECK,

    /*-----------------------------------------------------------------------
                                    IPv6 IOCTLS
    -----------------------------------------------------------------------*/
    IFACE_IOCTL_GENERATE_PRIV_IPV6_ADDR
      = PS_IFACE_IOCTL_GENERATE_PRIV_IPV6_ADDR,
    IFACE_IOCTL_GET_ALL_V6_PREFIXES
      = PS_IFACE_IOCTL_GET_ALL_V6_PREFIXES,

    /*-----------------------------------------------------------------------
                                    MCAST IOCTLS
    -----------------------------------------------------------------------*/
    IFACE_IOCTL_MCAST_JOIN
      = PS_IFACE_IOCTL_MCAST_JOIN,
    IFACE_IOCTL_MCAST_LEAVE
      = PS_IFACE_IOCTL_MCAST_LEAVE,
    IFACE_IOCTL_MCAST_JOIN_EX
      = PS_IFACE_IOCTL_MCAST_JOIN_EX,
    IFACE_IOCTL_MCAST_LEAVE_EX
      = PS_IFACE_IOCTL_MCAST_LEAVE_EX,
    //TODO:
    IFACE_IOCTL_MCAST_REGISTER_EX,

    /*-----------------------------------------------------------------------
                                  707 IOCTLS
    -----------------------------------------------------------------------*/
    IFACE_IOCTL_707_GET_MDR
      = PS_IFACE_IOCTL_707_GET_MDR,
    IFACE_IOCTL_707_SET_MDR
      = PS_IFACE_IOCTL_707_SET_MDR,
    IFACE_IOCTL_707_GET_DORM_TIMER
      = PS_IFACE_IOCTL_707_GET_DORM_TIMER,
    IFACE_IOCTL_707_SET_DORM_TIMER
      = PS_IFACE_IOCTL_707_SET_DORM_TIMER,
    IFACE_IOCTL_707_GET_RLP_ALL_CURR_NAK
      = PS_IFACE_IOCTL_707_GET_RLP_ALL_CURR_NAK,
    IFACE_IOCTL_707_SET_RLP_ALL_CURR_NAK
      = PS_IFACE_IOCTL_707_SET_RLP_ALL_CURR_NAK,
    IFACE_IOCTL_707_GET_RLP_ALL_DEF_NAK
      = PS_IFACE_IOCTL_707_GET_RLP_ALL_DEF_NAK,
    IFACE_IOCTL_707_SET_RLP_ALL_DEF_NAK
      = PS_IFACE_IOCTL_707_SET_RLP_ALL_DEF_NAK,
    IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK
      = PS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK,
    IFACE_IOCTL_707_GET_RLP_QOS_NA_PRI
      = PS_IFACE_IOCTL_707_GET_RLP_QOS_NA_PRI,
    IFACE_IOCTL_707_SET_RLP_QOS_NA_PRI
      = PS_IFACE_IOCTL_707_SET_RLP_QOS_NA_PRI,
    IFACE_IOCTL_707_SDB_SUPPORT_QUERY
      = PS_IFACE_IOCTL_707_SDB_SUPPORT_QUERY,
    IFACE_IOCTL_707_ENABLE_HOLDDOWN
      = PS_IFACE_IOCTL_707_ENABLE_HOLDDOWN,
    IFACE_IOCTL_707_ENABLE_HDR_HPT_MODE
      = PS_IFACE_IOCTL_707_ENABLE_HDR_HPT_MODE,
    IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA
      = PS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA,
    IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES
      = PS_IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES,
    IFACE_IOCTL_707_ENABLE_HDR_SLOTTED_MODE
      = PS_IFACE_IOCTL_707_ENABLE_HDR_SLOTTED_MODE,
    IFACE_IOCTL_707_GET_SESSION_TIMER
      = PS_IFACE_IOCTL_GET_SESSION_TIMER,
    IFACE_IOCTL_707_SET_SESSION_TIMER
      = PS_IFACE_IOCTL_SET_SESSION_TIMER,
    IFACE_IOCTL_707_GET_HDR_1X_HANDDOWN_OPTION
      = PS_IFACE_IOCTL_707_GET_HDR_1X_HANDDOWN_OPTION,
    IFACE_IOCTL_707_SET_HDR_1X_HANDDOWN_OPTION
      = PS_IFACE_IOCTL_707_SET_HDR_1X_HANDDOWN_OPTION,
    IFACE_IOCTL_707_GET_RLP_FILL_RATE
      = PS_IFACE_IOCTL_707_GET_RLP_FILL_RATE,
    IFACE_IOCTL_707_SET_RLP_FILL_RATE
      = PS_IFACE_IOCTL_707_SET_RLP_FILL_RATE,
    IFACE_IOCTL_707_GET_HYSTERESIS_ACT_TIMER
      = PS_IFACE_IOCTL_707_GET_HYSTERESIS_ACT_TIMER,
    IFACE_IOCTL_707_SET_HYSTERESIS_ACT_TIMER
      = PS_IFACE_IOCTL_707_SET_HYSTERESIS_ACT_TIMER,
    IFACE_IOCTL_707_GET_MIP_MA_INFO
      = PS_IFACE_IOCTL_707_GET_MIP_MA_INFO,
    IFACE_IOCTL_707_GET_SIP_MIP_DATA_CALL_TYPE
      = PS_IFACE_IOCTL_707_GET_SIP_MIP_DATA_CALL_TYPE,

    /*-----------------------------------------------------------------------
                                    UMTS IOCTLS
    -----------------------------------------------------------------------*/
    IFACE_IOCTL_UMTS_GET_IM_CN_FLAG
      = PS_IFACE_IOCTL_UMTS_GET_IM_CN_FLAG,

    /*-----------------------------------------------------------------------
                                    MBMS IOCTLS
    -----------------------------------------------------------------------*/
    IFACE_IOCTL_MBMS_MCAST_CONTEXT_ACTIVATE
      = PS_IFACE_IOCTL_MBMS_MCAST_CONTEXT_ACTIVATE,
    IFACE_IOCTL_MBMS_MCAST_CONTEXT_DEACTIVATE
      = PS_IFACE_IOCTL_MBMS_MCAST_CONTEXT_DEACTIVATE,


    /*-----------------------------------------------------------------------
                                    BCMCS IOCTLS
    -----------------------------------------------------------------------*/
    IFACE_IOCTL_BCMCS_DB_UPDATE
      = PS_IFACE_IOCTL_BCMCS_DB_UPDATE,
    IFACE_IOCTL_BCMCS_REGISTER_USING_HANDOFF
      = PS_IFACE_IOCTL_BCMCS_ENABLE_HANDOFF_REG,

    /*-------------------------------------------------------------------------
                                    UW FMC IOCTLS
    -------------------------------------------------------------------------*/
    IFACE_IOCTL_UW_FMC_SET_TUNNEL_PARAMS 
      = PS_IFACE_IOCTL_UW_FMC_SET_TUNNEL_PARAMS,
    IFACE_IOCTL_UW_FMC_RESET_TUNNEL_PARAMS 
      = PS_IFACE_IOCTL_UW_FMC_RESET_TUNNEL_PARAMS,


    /*-----------------------------------------------------------------------
                                    NAT IOCTLS
    -----------------------------------------------------------------------*/

    IFACE_IOCTL_GET_STATIC_NAT_ENTRY
      = PS_IFACE_IOCTL_GET_STATIC_NAT_ENTRY,

    IFACE_IOCTL_ADD_STATIC_NAT_ENTRY
      = PS_IFACE_IOCTL_ADD_STATIC_NAT_ENTRY,

    IFACE_IOCTL_GET_DYNAMIC_NAT_ENTRY_TIMEOUT
      = PS_IFACE_IOCTL_GET_DYNAMIC_NAT_ENTRY_TIMEOUT,

    IFACE_IOCTL_SET_DYNAMIC_NAT_ENTRY_TIMEOUT
      = PS_IFACE_IOCTL_SET_DYNAMIC_NAT_ENTRY_TIMEOUT,

    IFACE_IOCTL_DELETE_STATIC_NAT_ENTRY
      = PS_IFACE_IOCTL_DELETE_STATIC_NAT_ENTRY,

    IFACE_IOCTL_GET_NAT_IPSEC_VPN_PASS_THROUGH
      = PS_IFACE_IOCTL_GET_NAT_IPSEC_VPN_PASS_THROUGH,

    IFACE_IOCTL_SET_NAT_IPSEC_VPN_PASS_THROUGH
      = PS_IFACE_IOCTL_SET_NAT_IPSEC_VPN_PASS_THROUGH,

    IFACE_IOCTL_GET_NAT_L2TP_VPN_PASS_THROUGH
      = PS_IFACE_IOCTL_GET_NAT_L2TP_VPN_PASS_THROUGH,

    IFACE_IOCTL_SET_NAT_L2TP_VPN_PASS_THROUGH
      = PS_IFACE_IOCTL_SET_NAT_L2TP_VPN_PASS_THROUGH,

    IFACE_IOCTL_GET_NAT_PPTP_VPN_PASS_THROUGH
      = PS_IFACE_IOCTL_GET_NAT_PPTP_VPN_PASS_THROUGH,

    IFACE_IOCTL_SET_NAT_PPTP_VPN_PASS_THROUGH
      = PS_IFACE_IOCTL_SET_NAT_PPTP_VPN_PASS_THROUGH,

    IFACE_IOCTL_ADD_DMZ
      = PS_IFACE_IOCTL_ADD_DMZ,

    IFACE_IOCTL_GET_DMZ
      = PS_IFACE_IOCTL_GET_DMZ,

    IFACE_IOCTL_DELETE_DMZ
      = PS_IFACE_IOCTL_DELETE_DMZ,

    IFACE_IOCTL_GET_NAT_PUBLIC_IP_ADDR
      = PS_IFACE_IOCTL_GET_NAT_PUBLIC_IP_ADDR,

    IFACE_IOCTL_ENABLE_ALG
      = PS_IFACE_IOCTL_ENABLE_ALG,

    IFACE_IOCTL_DISABLE_ALG
      = PS_IFACE_IOCTL_DISABLE_ALG,

    IFACE_IOCTL_GET_ALG_STATUS
      = PS_IFACE_IOCTL_GET_ALG_STATUS,

    /*-----------------------------------------------------------------------
                                    MTPD IOCTLS
    -----------------------------------------------------------------------*/
    IFACE_IOCTL_MT_REG_CB
      = PS_IFACE_IOCTL_MT_REG_CB,
    IFACE_IOCTL_MT_DEREG_CB
      = PS_IFACE_IOCTL_MT_DEREG_CB,

    /* ARP Cache update for DHCP server unicast messages */
    IFACE_IOCTL_DHCP_ARP_CACHE_UPDATE
      = PS_IFACE_IOCTL_DHCP_ARP_CACHE_UPDATE,
    /* ARP Cache clear for DHCP server unicast messages */
    IFACE_IOCTL_DHCP_ARP_CACHE_CLEAR
      = PS_IFACE_IOCTL_DHCP_ARP_CACHE_CLEAR,
    /* Get the devices information which are connected to DHCP
       at any given point of time */
    IFACE_IOCTL_DHCP_SERVER_GET_DEVICE_INFO
      = PS_IFACE_IOCTL_DHCP_SERVER_GET_DEVICE_INFO,


    /*-----------------------------------------------------------------------
                                    TODO IOCTLS
    -----------------------------------------------------------------------*/
    //TODO : These IOCTLs need to be added to PS iface.
    //These IOCTLs are temporarily numbered so that they don't
    //clash with others.
    IFACE_IOCTL_GET_OUTAGE_NOTIFICATION_INFO                  = 0x7FFF0001,
    IFACE_IOCTL_MCAST_GET_TECHNOLOGY_STATUS                   = 0x7FFF0002,
    IFACE_IOCTL_MCAST_GET_REGISTRATION_STATUS                 = 0x7FFF0003,
    IFACE_IOCTL_MBMS_MCAST_CONTEXT_GET_STATUS                 = 0x7FFF0004,

    /*-----------------------------------------------------------------------
      These IOCTLs are needed to support event infos.
    -----------------------------------------------------------------------*/
    IFACE_IOCTL_GET_PREVIOUS_STATE                            = 0x7FFF0005,
    IFACE_IOCTL_GET_PREVIOUS_IP_ADDR                          = 0x7FFF0006,
    IFACE_IOCTL_GET_PREVIOUS_BEARER_TECHNOLOGY                = 0x7FFF0007,
    IFACE_IOCTL_GET_QOS_AWARE_INFO_CODE                       = 0x7FFF0008,
    IFACE_IOCTL_GET_HDR_REV0_RATE_INERTIA_RESULT_INFO_CODE    = 0x7FFF0009,

    IFACE_IOCTL_MAX,
    IFACE_IOCTL_NAME_FORCE_32_BIT
      = PS_IFACE_IOCTL_NAME_FORCE_32_BIT

  } IfaceIoctlEnumType;


  /*-------------------------------------------------------------------------
                                    FLOW IOCTLS
  -------------------------------------------------------------------------*/
  typedef enum
  {
    FLOW_IOCTL_MIN
      = 0,
    FLOW_IOCTL_QOS_MODIFY
      = PS_FLOW_IOCTL_QOS_MODIFY,
    FLOW_IOCTL_QOS_RELEASE
      = PS_FLOW_IOCTL_QOS_RELEASE,
    FLOW_IOCTL_QOS_GET_GRANTED_FLOW_SPEC
      = PS_FLOW_IOCTL_QOS_GET_GRANTED_FLOW_SPEC,
    FLOW_IOCTL_QOS_GET_STATUS
      = PS_FLOW_IOCTL_QOS_GET_STATUS,
    FLOW_IOCTL_QOS_RESUME
      = PS_FLOW_IOCTL_QOS_RESUME,
    FLOW_IOCTL_QOS_SUSPEND
      = PS_FLOW_IOCTL_QOS_SUSPEND,
    FLOW_IOCTL_QOS_RELEASE_EX
      = PS_FLOW_IOCTL_QOS_RELEASE_EX,
    FLOW_IOCTL_QOS_SUSPEND_EX
      = PS_FLOW_IOCTL_QOS_SUSPEND_EX,
    FLOW_IOCTL_QOS_RESUME_EX
      = PS_FLOW_IOCTL_QOS_RESUME_EX,
    FLOW_IOCTL_GET_TX_QUEUE_LEVEL
      = PS_FLOW_IOCTL_GET_TX_QUEUE_LEVEL,
    FLOW_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2
      = PS_FLOW_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2,
    FLOW_IOCTL_QOS_GET_GRANTED_FILTER_SPEC
      = PS_FLOW_IOCTL_QOS_GET_GRANTED_FILTER_SPEC,
    FLOW_IOCTL_PRIMARY_QOS_MODIFY
      = PS_FLOW_IOCTL_PRIMARY_QOS_MODIFY,
    FLOW_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC
      = PS_FLOW_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC,
    FLOW_IOCTL_HDR_GET_RMAC3_INFO
      = PS_FLOW_IOCTL_HDR_GET_RMAC3_INFO,
    FLOW_IOCTL_707_GET_TX_STATUS
      = PS_FLOW_IOCTL_707_GET_TX_STATUS,
    FLOW_IOCTL_707_GET_INACTIVITY_TIMER
      = PS_FLOW_IOCTL_707_GET_INACTIVITY_TIMER,
    FLOW_IOCTL_707_SET_INACTIVITY_TIMER
      = PS_FLOW_IOCTL_707_SET_INACTIVITY_TIMER,
    FLOW_IOCTL_GET_MODIFY_RESULT
      = PS_FLOW_IOCTL_GET_MODIFY_RESULT,
    FLOW_IOCTL_GET_FLOW_UPDATED_INFO_CODE
      = PS_FLOW_IOCTL_GET_FLOW_UPDATED_INFO_CODE,
    FLOW_IOCTL_GET_PRIMARY_QOS_MODIFY_RESULT
      = PS_FLOW_IOCTL_GET_PRIMARY_QOS_MODIFY_RESULT,
    FLOW_IOCTL_MAX
  } FlowIoctlEnumType;


  /*-------------------------------------------------------------------------
                                  PHYS LINK IOCTLS
  -------------------------------------------------------------------------*/
  typedef enum
  {
    PHYS_LINK_IOCTL_MIN,
    PHYS_LINK_IOCTL_GO_ACTIVE
      = PS_PHYS_LINK_IOCTL_GO_ACTIVE,
    PHYS_LINK_IOCTL_GO_DORMANT
      = PS_PHYS_LINK_IOCTL_GO_DORMANT,
    PHYS_LINK_IOCTL_GET_STATE
      = PS_PHYS_LINK_IOCTL_GET_STATE,
    PHYS_LINK_IOCTL_GET_DORMANCY_INFO_CODE                    
      = PS_PHYS_LINK_IOCTL_GET_DORMANCY_INFO_CODE,
    /* This IOCTL is handled in the network platform layer itself */
    //These IOCTLs are temporarily numbered so that they dont clash.
    PHYS_LINK_IOCTL_GET_PREVIOUS_STATE                        = 0x6FFF0001,
    PHYS_LINK_IOCTL_MAX
  } PhysLinkIoctlEnumType;


  /*-------------------------------------------------------------------------
                                  IOCTL arguments
  -------------------------------------------------------------------------*/
  typedef dss_iface_ioctl_state_type                            IfaceStateEnumType;
  typedef dss_iface_ioctl_bearer_tech_type                      BearerTechType;
  typedef dss_iface_ioctl_bearer_tech_ex_type                   BearerTechExType;
  typedef dss_iface_ioctl_data_path_bridge_mode_type            DataPathBridgeModeInfoType;
  typedef dss_iface_ioctl_data_bearer_rate                      DataBearerType;
  typedef dss_iface_ioctl_hw_addr_type                          HWAddrType;
  typedef dss_iface_ioctl_iface_name_type                       IfaceNameType;
  typedef dss_iface_ioctl_all_ifaces_type                       PSAllIfacesType;
  typedef dss_iface_ioctl_get_all_dns_addrs_type                AllDNSAddrsType;
  typedef dss_iface_ioctl_domain_name_type                      DomainNameType;
  typedef dss_iface_ioctl_domain_name_search_list_type          DomainNameSearchListType;
  typedef dss_iface_ioctl_phys_link_state_type                  PSPhysLinkStateType;
  typedef dss_iface_ioctl_sip_serv_addr_info_type               SipServerAddrInfoType;
  typedef dss_iface_ioctl_sip_serv_domain_name_info_type        SipServerDomainNameInfoType;
  typedef dss_iface_ioctl_rf_conditions_type                    RFConditionsType;
  typedef dss_iface_ioctl_enable_firewall_type                  EnableFirewallType;
  typedef dss_iface_ioctl_add_firewall_rule_type                AddFirewallRuleType;
  typedef dss_iface_ioctl_delete_firewall_rule_type             DeleteFirewallRuleType;
  typedef dss_iface_ioctl_get_firewall_rule_type                GetFirewallRuleType;
  typedef ps_iface_ioctl_get_firewall_table_type                GetFirewallTableType;
  typedef ps_iface_ioctl_add_static_nat_entry_type              AddStaticNatEntryType;
  typedef ps_iface_ioctl_delete_static_nat_entry_type           DeleteStaticNatEntryType;
  typedef ps_iface_ioctl_get_static_nat_entry_type              GetStaticNatEntryType;
  typedef ps_iface_ioctl_get_dynamic_nat_entry_timeout_type     GetDynamicNatEntryTimeoutType;
  typedef ps_iface_ioctl_set_dynamic_nat_entry_timeout_type     SetDynamicNatEntryTimeoutType;
  typedef dss_iface_ioctl_707_sdb_support_query_type            SDBSupportQueryType;
  typedef ps_iface_ioctl_nat_ipsec_vpn_pass_through_type        VPNPassThroughType;
  typedef ps_iface_ioctl_nat_l2tp_vpn_pass_through_type         L2TPVPNPassThroughType;
  typedef ps_iface_ioctl_nat_pptp_vpn_pass_through_type         PPTPVPNPassThroughType;
  typedef ps_iface_ioctl_dmz_type                               DMZType;
  typedef ps_iface_ioctl_alg_client_e                           AlgClientType;
  typedef ps_iface_ioctl_enable_alg_type                        EnableAlgType;
  typedef ps_iface_ioctl_disable_alg_type                       DisableAlgType;
  typedef ps_iface_ioctl_get_alg_status_type                    GetAlgStatusType;

  //TODO: Need to store this in the platform layer, since this is got throug events.
  typedef dss_iface_ioctl_outage_notification_event_info_type   OutageNotificationInfoType;
  //TODO: Store DHCP refresh results in the platform layer.

  typedef dss_iface_ioctl_primary_qos_get_granted_flow_spec_type   QoSGetGrantedFlowSpecType;
  typedef ps_flow_ioctl_qos_get_granted_filter_spec_type      QoSGetGrantedFilterSpecType;
  typedef ps_iface_ioctl_qos_request_type                       QoSRequestType;
  typedef ps_iface_ioctl_qos_request_ex_type                    QoSRequestExType;
  typedef ps_iface_ioctl_qos_net_initiated_request_type         QoSNetInitiatedRequestType;
  typedef ps_iface_ioctl_qos_net_initiated_release_type         QoSNetInitiatedReqReleaseType;
  typedef ps_iface_ioctl_qos_net_initiated_real_time_check_type QoSNetInitiatedReqRealTimeCheckType;
  typedef ps_flow_ioctl_primary_qos_modify_type                 PrimaryQoSModifyType;
  typedef primary_qos_spec_type                                 PrimaryQoSModifySpecType;
  typedef ps_flow_ioctl_qos_modify_type                         QoSModifyType;
  typedef ps_flow_ioctl_tx_queue_level_type                     TxQueueLevelType;
  typedef ps_flow_ioctl_qos_suspend_type                        QoSSuspendType;
  typedef ps_flow_ioctl_qos_suspend_ex_type                     QoSSuspendExType;
  typedef ps_flow_ioctl_qos_resume_type                         QoSResumeType;
  typedef ps_flow_ioctl_qos_resume_ex_type                      QoSResumeExType;
  typedef ps_flow_ioctl_qos_release_type                        QoSReleaseType;
  typedef ps_flow_ioctl_qos_release_ex_type                     QoSReleaseExType;
  typedef ps_flow_state_enum_type                               PSFlowStateType;
#ifdef FEATUTE_DATA_PS_MCAST
  typedef ps_iface_ioctl_mcast_join_type                        MCastJoinType;
  typedef ps_iface_ioctl_mcast_join_ex_type                     MCastJoinExType;
  typedef ps_iface_ioctl_mcast_leave_type                       MCastLeaveType;
  typedef ps_iface_ioctl_mcast_leave_ex_type                    MCastLeaveExType;
  typedef ps_iface_ioctl_mcast_register_ex_type                 MCastRegisterExType;
  typedef ps_iface_ioctl_mcast_addr_info_type                   MCastAddrInfoType;
  typedef ps_iface_ioctl_bcmcs_enable_handoff_reg_type          MCastRegisterUsingHandoffOptType;

  typedef ps_iface_ioctl_mbms_mcast_context_act_type            MBMSContextActType;
  typedef ps_iface_ioctl_mbms_mcast_context_deact_type          MBMSContextDeactType;
#endif // FEATUTE_DATA_PS_MCAST
  typedef ps_iface_ioctl_mt_reg_cb_type                         MTRegCBType;
  typedef ps_iface_ioctl_mt_dereg_cb_type                       MTDeRegCBType;

  typedef ps_iface_event_info_u_type                            IfaceEventInfoUnionType;
  typedef dss_iface_ioctl_qos_status_type                       QoSStatusType;
  typedef ps_iface_net_down_reason_type                         NetworkDownReasonType;

  typedef ps_iface_priv_ipv6_addr_info_type                     IPv6PrivAddrEventInfoType;
  typedef dss_iface_ioctl_priv_ipv6_addr_type                   IPv6PrivAddrIoctlArgType;
  typedef dss_iface_ioctl_get_all_v6_prefixes_type              IPv6GetAllPrefixesType;
  typedef dss_iface_ioctl_prefix_info_type                      IPv6PrefixInfoType;
  typedef ps_iface_prefix_update_type                           IPv6PrefixUpdatedEventInfoType;
  typedef ps_iface_ioctl_extended_ip_config_ex_type             ExtendedIpConfigExType;
  typedef ps_iface_handoff_event_info_type                      HandoffEventInfoType;
  typedef ps_iface_changed_apn_param_info_type                  ApnParamInfoType;
  typedef ps_iface_handoff_failure_event_info_type              HandoffFailureEventInfoType;
  typedef struct
  { 
    phys_link_state_type               state;
    ps_extended_info_code_enum_type    info_code;
  }PhysLinkEventInfo;

  typedef ip_addr_enum_type PSIPAddrFamilyType;
  typedef struct ps_in_addr PSINAddrType;
  typedef struct ps_in6_addr PSIN6AddrType;
  typedef ps_ip_addr_type PSIPAddrType;

  /*-------------------------------------------------------------------------
    Typedefs for iface/flow/physlink event names
  -------------------------------------------------------------------------*/
  typedef ps_iface_event_enum_type IfaceEventNameEnumType;
  typedef ps_iface_event_enum_type FlowEventNameEnumType;
  typedef ps_iface_event_enum_type PhysLinkEventNameEnumType;

  /*-------------------------------------------------------------------------
    Typedefs for IP Filter and Flow spec types
  -------------------------------------------------------------------------*/
  typedef ip_filter_type                  PSFilterSpecType;
  typedef ip_flow_type                    PSFlowSpecType;
  typedef qos_spec_type                   PSQoSSpecType;
  typedef ps_iface_ipfltr_add_param_type  PSIfaceIPFilterAddParamType;
  typedef net_initiated_qos_spec_type
                                          PSQoSNetInitiatedSpecType;

  /*-------------------------------------------------------------------------
  Typedef for BCMCS update parameter type
  -------------------------------------------------------------------------*/
  typedef ps_iface_ioctl_bcmcs_db_update_type PSBCMCSDbUpdateType;

/*-------------------------------------------------------------------------
  Typedef for Slotted mode enum type
  -------------------------------------------------------------------------*/
  typedef ps_hdr_slotted_mode_failure_code_enum_type SlottedModeFailureCodeType;

/*-------------------------------------------------------------------------
  Typedef for HDR Rev0 Rate mode enum type
  -------------------------------------------------------------------------*/  
  typedef ps_hdr_rev0_rate_inertia_failure_code_enum_type Rev0RateInertiaFailureCodeType;
  
  /*-------------------------------------------------------------------------
  Typedef for NAT static Entry type
  -------------------------------------------------------------------------*/
  typedef ps_iface_ioctl_static_nat_entry_type                  PSStaticNatEntryType;

  /*-------------------------------------------------------------------------
  Typedef for DMZ
  -------------------------------------------------------------------------*/
  typedef ps_iface_ioctl_dmz_type                  PSDMZEntryType;

  /*-------------------------------------------------------------------------
  Typedef for Enable ALG
  -------------------------------------------------------------------------*/
  typedef ps_iface_ioctl_enable_alg_type   PSEnableAlgType;

  /*-------------------------------------------------------------------------
  Typedef for Disable ALG
  -------------------------------------------------------------------------*/
  typedef ps_iface_ioctl_disable_alg_type   PSDisableAlgType;

  /*-------------------------------------------------------------------------
  Typedef for Get ALG Status list
  -------------------------------------------------------------------------*/
  typedef ps_iface_ioctl_get_alg_status_type   PSGetAlgStatusType;

  /*-------------------------------------------------------------------------
    Typedef for FMC tunnel parameters type
  -------------------------------------------------------------------------*/
  typedef ps_iface_ioctl_uw_fmc_tunnel_params_type PSFMCTunnelParamsType;

/*-------------------------------------------------------------------------
    Typedef for iface name enum type
  -------------------------------------------------------------------------*/
  typedef ps_iface_name_enum_type PSIFaceNameEnumType;

  /*-------------------------------------------------------------------------
                         Callback function declarations
  -------------------------------------------------------------------------*/
  typedef void (*IfaceEventCbackType)
  (
    int32                          ifaceHandle,
    int32                          eventName,
    IfaceEventInfoUnionType        eventInfo,
    void                          *clientDataPtr
  );

  typedef void (*FlowEventCbackType)
  (
    int32                          flowHandle,
    int32                          eventName,
    IfaceEventInfoUnionType        eventInfo,
    void                          *clientDataPtr
  );

  typedef void (*PhysLinkEventCbackType)
  (
    int32                          physLinkHandle,
    int32                          eventName,
    IfaceEventInfoUnionType        eventInfo,
    void                          *clientDataPtr
  );

  typedef void (*QoSNetInitiatedReqEventCbackType)
  (
    int32                          qosNetInitiatedReqHandle,    
    int32                          eventName,
    IfaceEventInfoUnionType        eventInfo,
    void                          *clientDataPtr
  );

  /*-------------------------------------------------------------------------
                          ACL Definitions.
  -------------------------------------------------------------------------*/
  typedef acl_policy_info_type ACLPolicyInfoType;

  /*-------------------------------------------------------------------------
                          Public function declarations
  -------------------------------------------------------------------------*/
  void Init
  (
    void
  );

  int32 RegIfaceCbackFcn
  (
    IfaceEventCbackType            cbackFcnPtr,
    void *                         userDataPtr
  );

  int32 RegFlowCbackFcn
  (
    FlowEventCbackType             cbackFcnPtr,
    void *                         userDataPtr
  );

  int32 RegPhysLinkCbackFcn
  (
    PhysLinkEventCbackType         cbackFcnPtr,
    void *                         userDataPtr
  );

  int32 IfaceIoctlNonNullArg
  (
    int32                          ifaceHandle,
    IfaceIoctlEnumType             ioctlName,
    void                          *argValPtr
  ) throw();

  int32 IfaceIoctl
  (
    int32                          ifaceHandle,
    IfaceIoctlEnumType             ioctlName,
    void                          *argValPtr
  ) throw();

  int32 IfaceIoctlByIfaceId
  (
    int32                          ifaceId,
    IfaceIoctlEnumType             ioctlName,
    void                          *argValPtr
  );

  int32 FlowIoctl
  (
    int32                          flowHandle,
    FlowIoctlEnumType              ioctlName,
    void                          *argValPtr
  );

  int32 FlowIoctlNonNullArg
  (
    int32                          flowHandle,
    FlowIoctlEnumType              ioctlName,
    void                          *argValPtr
  );

  int32 PhysLinkIoctl
  (
    int32                          ifaceHandle,
    PhysLinkIoctlEnumType          ioctlName,
    void                          *argValPtr
  );

  int32 IfaceRegAllEvents
  (
    void*                          userData
  );

  int32 FlowRegAllEvents
  (
    void*                          userData
  );

  int32 PhysLinkRegAllEvents
  (
    void*                          userData
  );


  int32 IfaceBringUpCmd
  (
    int32                          ifaceHandle,
    void                          *clientDataPtr
  );

  int32 IfaceTearDownCmd
  (
    int32                          ifaceHandle,
    void                          *clientDataPtr
  );

  int32 IfaceTearDownCmdEx
  (
    int32                          ifaceHandle,
    ds::Net::IPolicy              *pIDSNetPolicy,
    void                          *clientDataPtr
  );

  int32 IfaceBringUpByPolicy
  (
    ds::Net::IPolicy              *pIDSNetPolicy,
    int32                         *pIfaceHandle,
    int32                         *pAppPriority,
    ps_iface_net_down_reason_type * net_down_reason
  );

  int32 IfaceLookUpByPolicy
  (
    ds::Net::IPolicy              *pIDSNetPolicy,
    int32                         *pIfaceHandle
  );

  int32 PhysLinkUpCmd
  (
    int32                          physLinkHandle,
    void                          *clientDataPtr
  );

  int32 IfaceGetAddr
  (
    int32                          ifaceHandle,
    const ds::INAddr6Type         *v6RemoteIPAddrPtr,
    ds::INAddr6Type               *ifaceIPAddr
  );

  boolean IsDefaultIPv6AddrStateValid
  (
    int32                           psIfaceHandle
  );

  int32 IPv6PrivAddrIncRefCntEx
  (
    const ds::INAddr6Type          *ipAddrPtr,
    boolean               includeExternalAddr,
    ds::ErrorType                  *dsErrnoPtr
  );

  int32 IPv6PrivAddrIncRefCnt
  (
    const ds::INAddr6Type          *ipAddrPtr,
    ds::ErrorType                  *dsErrnoPtr
  );

  int32 IPv6PrivAddrDecRefCnt
  (
    const ds::INAddr6Type          *ipAddrPtr,
    ds::ErrorType                  *dsErrnoPtr
  ) throw();

  int32 RouteDataPathLookup
  (
    ip_pkt_info_type               *ipPktPtr,
    bool                            isSystemSocket,
    ds::Net::IPolicy               *policyObjPtr,
    boolean                         isAddrBasedRoutingOnly,
    int32                          *ifaceHandlePtr
  );

  uint32 IPFltrExecute
  (
    int32                                ifaceHandle,
    ps_iface_ipfltr_client_id_enum_type  ipFltrClientID,
    ip_pkt_info_type                    *ipPktInfoPtr
  );

  int32 GetDefaultPSFlowFromRtMetaInfo
  (
    ps_rt_meta_info_type                *rtMetaInfoPtr
  );

  int32 GetPSFlowFromRtMetaInfo
  (
    ps_rt_meta_info_type                *rtMetaInfoPtr
  );

  int32 GetPhysLinkFromRtMetaInfo
  (
    ps_rt_meta_info_type                *rtMetaInfoPtr
  );

  int MapErrorCode
  (
    int16 psErrno
  ) throw();

  uint8 PSGetIfaceIndex
  (
    int32 ifaceHandle
  );

  uint8 PSGetFlowInstance
  (
    int32 ifaceHandle
  );

  int32 PSGetPhysLink
  (
    int32  ifaceHandle,
    int32 *physLinkhandle
  );

  int32 PSGetDefaultFlow
  (
    int32  argIfaceHandle,
    int32* defaultFlowPtr
  );

  int32 PSGetPhysLinkFromFlow
  (
    int32   flowHandle,
    int32*  physLinkHandle
  );

  int32 PSGetPhysLinkFromIface
  (
    int32  ifaceHandle,
    int32* physLinkHandle
  );

  int32 PSGetIfaceId
  (
    int32  ifaceHandle,
    int32* ifaceId
  );

  int32 PSIfaceIPFilterAdd
  (
    int32                                   ifaceHandle,
    ps_iface_name_enum_type                 ifaceGroup,
    ps_iface_ipfltr_client_id_enum_type     clientID,
    const PSIfaceIPFilterAddParamType     * fltrParams,
    int32                                 * fltrHandle
  );

  int32 PSIfaceIPFilterDelete
  (
    int32                                  ifaceHandle,
    ps_iface_ipfltr_client_id_enum_type    clientId,
    int32                                  fltrHandle
  ) throw();

  bool IsPSIfaceTxEnabled
  (
    int32  psIfaceHandle
  );

  bool IsPSIfaceCommonTxDoSEnabled
  (
    int32  psIfaceHandle,
    int32  psFlowHandle
  );

  bool IsPSIfaceCommonTxEnabled
  (
    int32  psIfaceHandle,
    int32  psFlowHandle
  );

  int32 GetQoSStatus
  (
    int32             flowHandle,
    QoSStatusType *   pQoSStatus
  );

  void GetLastNetworkDownReason
  (
    int32                   ifaceHandle,
    NetworkDownReasonType * pNetDownReason
  );

  IfaceStateEnumType GetIfaceState
  (
    int32 ifaceHandle
  );

  PSFlowStateType GetPSFlowState
  (
    int32  psFlowHandle
  );

  ps_iface_type *PSIfaceGetHandle
  (
    int32  ifaceId
  );

  bool IsPSFlowDelaySensitive
  (
    int32  psFlowHandle
  );

  bool IsPSFlowDefault
  (
    int32  psFlowHandle
  );

  ds::ErrorType GetRouteScopeByPolicy
  (
    ds::Net::IPolicy *         netPolicyPtr,
    ps_route_scope_type *  routeScopePtr
  );

  ds::ErrorType GetRouteScopeByIfaceId
  (
    int32                  ifaceId,
    bool                   addV6LoopbackIface,
    ps_route_scope_type *  routeScopePtr
  );

  ds::ErrorType GetRouteScopeByIfaceHandle
  (
    int32                  ifaceHandle,
    bool                   addV6LoopbackIface,
    ps_route_scope_type *  routeScopePtr
  );

  ds::ErrorType AddLoopbackToRouteScope
  (
    bool                   addV6LoopbackIface,
    ps_route_scope_type *  routeScopePtr
  );

  ds::ErrorType DeleteIfaceHandleFromRouteScope
  (
    ps_route_scope_type *  routeScopePtr,
    int32                  ifaceHandle
  );

  boolean RouteScopeIntersect
  (
    ps_route_scope_type *  pRouteScope1,
    ps_route_scope_type *  pRouteScope2
  );

  int32 GenerateAclPolicy
  (
    ds::Net::IPolicy          *netPolicyPtr,
    ACLPolicyInfoType     *aclPolicyPtr
  );

  int32 GetIfaceStats
  (
    int32                     ifaceHandle,
    ds::Net::NetworkStatsType * stats
  );

  int32 ResetIfaceStats
  (
    int32                    ifaceHandle
  );

  void* EnableNat
  (
    ds::Net::IPolicy   *pIPolicy,
    ds::ErrorType      *errnoPtr
  );

  ds::ErrorType DisableNat
  (
    void*     natHandle
  );

  ds::ErrorType EnableRoamingAutoconnect
  (
    void
  );

  ds::ErrorType DisableRoamingAutoconnect
  (
    void
  );

} /* namespace NetPlatform */

#endif /* DSNETPLATFORM_H */
