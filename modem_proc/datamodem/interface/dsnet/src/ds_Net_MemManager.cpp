/*===========================================================================
  FILE: DSNetMemoryManager.cpp

  This file defines functions for translation of Class id to pool id macros.
  It also provides default initializations of PS mem pool objects for all
  the different DSNet objects.

  TODO: Write detailed explanation.

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_MemManager.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-11-01 rk  Removed feature mobileap.
  2009-02-27 hm  Added buffer IPv6 address objects.
  2008-06-30 hm  Created module.

===========================================================================*/
/*---------------------------------------------------------------------------
  Include Files
---------------------------------------------------------------------------*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_MemConfig.h"
#include "ds_Net_MemManager.h"
#include "ps_mem.h"
#include "dss_config.h"

#include "ds_Net_EventManager.h"
#include "ds_Net_MCastSession.h"
#include "ds_Net_NetworkActive.h"
#include "ds_Net_NetworkMonitored.h"
#include "ds_Net_Network1X.h"
#include "ds_Net_NetworkFactory.h"
#include "ds_Net_NetworkIPv6.h"
#include "ds_Net_MCastMBMSCtrl.h"
#include "ds_Net_NetworkUMTS.h"
#include "ds_Net_Policy.h"
#include "ds_Net_PhysLink.h"
#include "ds_Net_QoS1X.h"
#include "ds_Net_QoSDefault.h"
#include "ds_Net_QoSSecondary.h"
#include "ds_Net_NetworkIPv6Address.h"
#include "ds_Net_TechUMTSFactory.h"
#include "ds_Net_BearerTech.h" 
#include "ds_Net_BearerTechEx.h"
#include "ds_Net_MCastManager.h"
#include "ds_Net_QoSManager.h"
#include "ds_Net_QoSFlowSpec.h"
#include "ds_Net_IPFilterSpec.h"
#include "ds_Net_IPFilterReg.h"
#include "ds_Net_MBMSSpec.h"
#include "ds_Net_MTPDReg.h"
#include "ds_Net_FirewallManager.h"
#include "ds_Net_FirewallRule.h"
#include "ds_Net_MCastManagerPriv.h"
#include "ds_Net_MCastSessionPriv.h"
#include "ds_Net_MCastSessionsInput.h"
#include "ds_Net_MCastSessionsOutput.h"
#include "ds_Net_PSQoSSpecWrapper.h"
#include "ds_Net_QoSSecondariesInput.h"
#include "ds_Net_QoSSecondariesOutput.h"
#include "ds_Net_QoSNetInitiated.h"
#include "ds_Net_QoSNetInitiatedReq.h"

namespace ds
{
namespace Net
{

/*---------------------------------------------------------------------------
  Macros for sizes of objects of these classes.
---------------------------------------------------------------------------*/
#define EVENT_MANAGER_SIZE       ((sizeof(EventManager) + 3) & ~3)
#define MCAST_SESSION_SIZE       ((sizeof(MCastSession) + 3) & ~3)
#define MCAST_SESSION_PRIV_SIZE  ((sizeof(MCastSessionPriv) + 3) & ~3)
#define MCAST_SESSIONS_INPUT_SIZE   ((sizeof(MCastSessionsInput) + 3) & ~3)
#define MCAST_SESSIONS_OUTPUT_SIZE  ((sizeof(MCastSessionsOutput) + 3) & ~3)
#define NETWORK_1X_SIZE          ((sizeof(Network1X) + 3) & ~3)
#define NETWORK_FACTORY_SIZE     ((sizeof(NetworkFactory) + 3) & ~3)
#define NETWORK_IPV6_SIZE        ((sizeof(NetworkIPv6) + 3) & ~3)
#define NETWORK_MBMS_SIZE        ((sizeof(MCastMBMSCtrl) + 3) & ~3)
#define NETWORK_UMTS_SIZE        ((sizeof(NetworkUMTS) + 3) & ~3)
#define NETWORK_ACTIVE_SIZE      ((sizeof(NetworkActive) + 3) & ~3)
#define NETWORK_MONITORED_SIZE   ((sizeof(NetworkMonitored) + 3) & ~3)
#define POLICY_SIZE              ((sizeof(Policy) + 3) & ~3)
#define PHYS_LINK_SIZE           ((sizeof(PhysLink) + 3) & ~3)
#define QOS_1X_SIZE              ((sizeof(QoS1X) + 3) & ~3)
#define QOS_DEFAULT_SIZE         ((sizeof(QoSDefault) + 3) & ~3)
#define QOS_SECONDARY_SIZE       ((sizeof(QoSSecondary) + 3) & ~3)
#define QOS_NET_INITIATED_REQ_SIZE ((sizeof(QoSNetInitiatedReq) + 3) & ~3)
#define QOS_NET_INITIATED_SIZE   ((sizeof(QoSNetInitiated) + 3) & ~3)
#define IPV6_ADDRESS_SIZE        ((sizeof(NetworkIPv6Address) + 3) & ~3)
#define TECH_UMTS_FACTORY_SIZE   ((sizeof(TechUMTSFactory) + 3) & ~3)
#define BEARER_TECH_SIZE         ((sizeof(BearerTech) + 3) & ~3)
#define BEARER_TECH_EX_SIZE      ((sizeof(BearerTechEx) + 3) & ~3)
#define MCAST_MANAGER_SIZE       ((sizeof(MCastManager) + 3) & ~3)
#define MCAST_MANAGER_PRIV_SIZE  ((sizeof(MCastManagerPriv) + 3) & ~3)
#define QOS_MANAGER_SIZE         ((sizeof(QoSManager) + 3) & ~3)
#define QOS_FLOW_SPEC_SIZE       ((sizeof(QoSFlowSpec) + 3) & ~3)
#define PS_QOS_SPEC_WRAPPER_SIZE ((sizeof(PSQoSSpecWrapper) + 3) & ~3)
#define IP_FILTER_SPEC_SIZE      ((sizeof(IPFilterSpec) + 3) & ~3)
#define IP_FILTER_REG_SIZE       ((sizeof(IPFilterReg) + 3) & ~3)
#define MBMS_JOIN_INFO_SIZE      ((sizeof(MBMSSpec) + 3) & ~3)
#define MTPD_REG_SIZE            ((sizeof(MTPDReg) + 3) & ~3)
#define FIREWALL_MANAGER_SIZE    ((sizeof(FirewallManager) + 3) & ~3)
#define FIREWALL_RULE_SIZE       ((sizeof(FirewallRule) + 3) & ~3)
#define NET_NAT_SESSION_SIZE     ((sizeof(NatSession) + 3) & ~3)
#define NET_NAT_MANAGER_SIZE     ((sizeof(NatManager) + 3) & ~3)
#define QOS_SECONDARIES_INPUT_SIZE   ((sizeof(QoSSecondariesInput) + 3) & ~3)
#define QOS_SECONDARIES_OUTPUT_SIZE   ((sizeof(QoSSecondariesOutput) + 3) & ~3)
        

/*---------------------------------------------------------------------------
  Macros for number of buffers needed, high and low watermarks.
  These are valid for both high end and low end targets.
---------------------------------------------------------------------------*/
#define EVENT_MANAGER_NUM_BUFS   (1)
#define MCAST_SESSION_NUM_BUFS   (MAX_MCAST_OBJS)
#define MCAST_SESSION_PRIV_NUM_BUFS  (MAX_MCAST_OBJS)
#define MCAST_SESSIONS_INPUT_NUM_BUFS  (MAX_MCAST_OBJS)
#define MCAST_SESSIONS_OUTPUT_NUM_BUFS  (MAX_MCAST_OBJS)
#define NETWORK_1X_NUM_BUFS      (MAX_NETWORK_1X_OBJS)
#define NETWORK_FACTORY_NUM_BUFS (1)
#define NETWORK_IPV6_NUM_BUFS    (MAX_NETWORK_ACTIVE_OBJS)
#define NETWORK_MBMS_NUM_BUFS    (MAX_NETWORK_MBMS_OBJS)
#define NETWORK_UMTS_NUM_BUFS    (MAX_NETWORK_UMTS_OBJS)
#define NETWORK_ACTIVE_NUM_BUFS (MAX_NETWORK_ACTIVE_OBJS)
#define NETWORK_MONITORED_NUM_BUFS (MAX_NETWORK_MONITORED_OBJS)
#define PHYS_LINK_NUM_BUFS       (MAX_PHYS_LINK_OBJS)
#define POLICY_NUM_BUFS          (MAX_POLICY_OBJS)
#define QOS_1X_NUM_BUFS          (MAX_QOS_1X_OBJS)
#define QOS_DEFAULT_NUM_BUFS     (MAX_QOS_DEFAULT_OBJS)
#define QOS_SECONDARY_NUM_BUFS   (MAX_QOS_SECONDARY_OBJS)
#define QOS_NET_INITIATED_REQ_NUM_BUFS   (MAX_QOS_SECONDARY_OBJS)
#define QOS_NET_INITIATED_NUM_BUFS   (MAX_QOS_SECONDARY_OBJS)
#define IPV6_ADDRESS_NUM_BUFS    (20)
#define TECH_UMTS_FACTORY_NUM_BUFS (MAX_NETWORK_UMTS_OBJS)
#define BEARER_TECH_NUM_BUFS     (20)
#define BEARER_TECH_EX_NUM_BUFS     (BEARER_TECH_NUM_BUFS)
#define MCAST_MANAGER_NUM_BUFS   (MAX_NETWORK_ACTIVE_OBJS)
#define MCAST_MANAGER_PRIV_NUM_BUFS  (MAX_NETWORK_ACTIVE_OBJS)
#define QOS_MANAGER_NUM_BUFS     (MAX_NETWORK_ACTIVE_OBJS)
#define QOS_FLOW_SPEC_NUM_BUFS   (16 * MAX_QOS_SECONDARY_OBJS)
#define PS_QOS_SPEC_WRAPPER_NUM_BUFS   (16 * MAX_QOS_SECONDARY_OBJS)
#define IP_FILTER_SPEC_NUM_BUFS  (16 * MAX_QOS_SECONDARY_OBJS)
#define IP_FILTER_REG_NUM_BUFS   (2*MAX_NETWORK_OBJS)
#define MBMS_JOIN_INFO_NUM_BUFS  (10)
#define MTPD_REG_NUM_BUFS        (10)
#define BCMCS_UPD_PARAM_NUM_BUFS (10)
#define FIREWALL_MANAGER_NUM_BUFS (MAX_NETWORK_OBJS)
#define FIREWALL_RULE_NUM_BUFS    (15)
#define NET_NAT_SESSION_NUM_BUFS  (MAX_NETWORK_OBJS)
#define NET_NAT_MANAGER_NUM_BUFS  (3)
#define QOS_SECONDARIES_INPUT_NUM_BUFS (MAX_QOS_SECONDARY_OBJS)
#define QOS_SECONDARIES_OUTPUT_NUM_BUFS (MAX_QOS_SECONDARY_OBJS)

#define EVENT_MANAGER_HIGH_WM    (1)
#define MCAST_SESSION_HIGH_WM    (MCAST_SESSION_NUM_BUFS - 1)
#define MCAST_SESSION_PRIV_HIGH_WM    (MCAST_SESSION_PRIV_NUM_BUFS - 1)
#define MCAST_SESSIONS_INPUT_HIGH_WM    (MCAST_SESSIONS_INPUT_NUM_BUFS - 1)
#define MCAST_SESSIONS_OUTPUT_HIGH_WM    (MCAST_SESSIONS_OUTPUT_NUM_BUFS - 1)
#define NETWORK_1X_HIGH_WM       (NETWORK_1X_NUM_BUFS - 1)
#define NETWORK_FACTORY_HIGH_WM  (1)
#define NETWORK_IPV6_HIGH_WM     (NETWORK_IPV6_NUM_BUFS - 1)
#define NETWORK_MBMS_HIGH_WM     (NETWORK_MBMS_NUM_BUFS - 1)
#define NETWORK_UMTS_HIGH_WM     (NETWORK_UMTS_NUM_BUFS - 1)
#define NETWORK_ACTIVE_HIGH_WM    (NETWORK_ACTIVE_NUM_BUFS - 1)
#define NETWORK_MONITORED_HIGH_WM (NETWORK_MONITORED_NUM_BUFS - 1)
#define PHYS_LINK_HIGH_WM        (PHYS_LINK_NUM_BUFS - 1)
#define POLICY_HIGH_WM           (POLICY_NUM_BUFS - 1)
#define QOS_1X_HIGH_WM           (QOS_1X_NUM_BUFS - 1)
#define QOS_DEFAULT_HIGH_WM      (QOS_DEFAULT_NUM_BUFS - 2)
#define QOS_SECONDARY_HIGH_WM    (MAX_QOS_SECONDARY_OBJS - 1)
#define QOS_NET_INITIATED_REQ_HIGH_WM   (MAX_QOS_SECONDARY_OBJS - 1)
#define QOS_NET_INITIATED_HIGH_WM   (MAX_QOS_SECONDARY_OBJS - 1)
#define IPV6_ADDRESS_HIGH_WM     (IPV6_ADDRESS_NUM_BUFS - 1)
#define BCMCS_UPD_PARAM_HIGH_WM  (BCMCS_UPD_PARAM_NUM_BUFS - 1)
#define TECH_UMTS_FACTORY_HIGH_WM (NETWORK_UMTS_NUM_BUFS - 1)
#define BEARER_TECH_HIGH_WM      (15)
#define BEARER_TECH_EX_HIGH_WM (BEARER_TECH_HIGH_WM)
#define MCAST_MANAGER_HIGH_WM    (MCAST_MANAGER_NUM_BUFS - 3)
#define MCAST_MANAGER_PRIV_HIGH_WM    (MCAST_MANAGER_PRIV_NUM_BUFS - 3)
#define QOS_MANAGER_HIGH_WM      (QOS_MANAGER_NUM_BUFS - 3)
#define QOS_FLOW_SPEC_HIGH_WM    (MAX_QOS_SECONDARY_OBJS - 5)
#define PS_QOS_SPEC_WRAPPER_HIGH_WM    (MAX_QOS_SECONDARY_OBJS - 5)
#define IP_FILTER_SPEC_HIGH_WM   (MAX_QOS_SECONDARY_OBJS - 5)
#define IP_FILTER_REG_HIGH_WM    (IP_FILTER_REG_NUM_BUFS - 2)
#define MBMS_JOIN_INFO_HIGH_WM   (8)
#define MTPD_REG_HIGH_WM         (8)
#define FIREWALL_MANAGER_HIGH_WM (FIREWALL_MANAGER_NUM_BUFS - 3)
#define FIREWALL_RULE_HIGH_WM    (13)
#define NET_NAT_SESSION_HIGH_WM  (NET_NAT_SESSION_NUM_BUFS - 3)
#define NET_NAT_MANAGER_HIGH_WM  (3)
#define QOS_SECONDARIES_INPUT_HIGH_WM (MAX_QOS_SECONDARY_OBJS - 5)
#define QOS_SECONDARIES_OUTPUT_HIGH_WM (MAX_QOS_SECONDARY_OBJS - 5)

#define EVENT_MANAGER_LOW_WM     (0)
#define MCAST_SESSION_LOW_WM     (1)
#define MCAST_SESSION_PRIV_LOW_WM  (1)
#define MCAST_SESSIONS_INPUT_LOW_WM  (1)
#define MCAST_SESSIONS_OUTPUT_LOW_WM  (1)
#define NETWORK_1X_LOW_WM        (1)
#define NETWORK_FACTORY_LOW_WM   (0)
#define NETWORK_IPV6_LOW_WM      (1)
#define BCMCS_UPD_PARAM_LOW_WM  (1)
#define NETWORK_MBMS_LOW_WM      (0)
#define NETWORK_UMTS_LOW_WM      (1)
#define NETWORK_ACTIVE_LOW_WM    (0)
#define NETWORK_MONITORED_LOW_WM    (1)
#define PHYS_LINK_LOW_WM         (1)
#define POLICY_LOW_WM            (1)
#define QOS_1X_LOW_WM            (1)
#define QOS_DEFAULT_LOW_WM       (1)
#define QOS_SECONDARY_LOW_WM     (1)
#define QOS_NET_INITIATED_REQ_LOW_WM (1)
#define QOS_NET_INITIATED_LOW_WM (1)
#define IPV6_ADDRESS_LOW_WM      (1)
#define TECH_UMTS_FACTORY_LOW_WM (1)
#define BEARER_TECH_LOW_WM       (5)
#define BEARER_TECH_EX_LOW_WM    (BEARER_TECH_LOW_WM)
#define MCAST_MANAGER_LOW_WM     (1)
#define MCAST_MANAGER_PRIV_LOW_WM     (1)
#define QOS_MANAGER_LOW_WM       (1)
#define QOS_FLOW_SPEC_LOW_WM     (3)
#define PS_QOS_SPEC_WRAPPER_LOW_WM     (3)
#define IP_FILTER_SPEC_LOW_WM    (3)
#define IP_FILTER_REG_LOW_WM     (2)
#define MBMS_JOIN_INFO_LOW_WM    (3)
#define MTPD_REG_LOW_WM          (3)
#define FIREWALL_MANAGER_LOW_WM  (1)
#define FIREWALL_RULE_LOW_WM     (3)
#define NET_NAT_SESSION_LOW_WM   (1)
#define NET_NAT_MANAGER_LOW_WM   (0)
#define QOS_SECONDARIES_INPUT_LOW_WM (3)
#define QOS_SECONDARIES_OUTPUT_LOW_WM (3)

/*---------------------------------------------------------------------------
  Allocate memory to hold different ds Net objects along with ps_mem header.
---------------------------------------------------------------------------*/
static int event_manager_buf[PS_MEM_GET_TOT_SIZE(EVENT_MANAGER_NUM_BUFS, 
                                                 EVENT_MANAGER_SIZE)];
#ifdef FEATUTE_DATA_PS_MCAST
static int mcast_session_buf [PS_MEM_GET_TOT_SIZE_OPT(MCAST_SESSION_NUM_BUFS,
                                                      MCAST_SESSION_SIZE)];

static int mcast_session_priv_buf [PS_MEM_GET_TOT_SIZE_OPT(MCAST_SESSION_PRIV_NUM_BUFS,
                                                           MCAST_SESSION_PRIV_SIZE)];

static int mcast_sessions_input_buf [PS_MEM_GET_TOT_SIZE_OPT(MCAST_SESSIONS_INPUT_NUM_BUFS,
                                                           MCAST_SESSIONS_INPUT_SIZE)];

static int mcast_sessions_output_buf [PS_MEM_GET_TOT_SIZE_OPT(MCAST_SESSIONS_OUTPUT_NUM_BUFS,
                                                           MCAST_SESSIONS_OUTPUT_SIZE)];
#endif // FEATUTE_DATA_PS_MCAST

static int network_1x_buf [PS_MEM_GET_TOT_SIZE_OPT(NETWORK_1X_NUM_BUFS,
                                                   NETWORK_1X_SIZE)];

static int network_factory_buf [PS_MEM_GET_TOT_SIZE(NETWORK_FACTORY_NUM_BUFS,
                                                    NETWORK_FACTORY_SIZE)];

static int network_ipv6_buf [PS_MEM_GET_TOT_SIZE_OPT(NETWORK_IPV6_NUM_BUFS,
                                                     NETWORK_IPV6_SIZE)];

#ifdef FEATUTE_DATA_PS_MCAST
static int network_mbms_buf [PS_MEM_GET_TOT_SIZE_OPT(NETWORK_MBMS_NUM_BUFS,
                                                     NETWORK_MBMS_SIZE)];
#endif //FEATUTE_DATA_PS_MCAST
static int network_umts_buf [PS_MEM_GET_TOT_SIZE_OPT(NETWORK_UMTS_NUM_BUFS,
                                                     NETWORK_UMTS_SIZE)];

static int network_active_buf [PS_MEM_GET_TOT_SIZE_OPT(NETWORK_ACTIVE_NUM_BUFS,
                                                NETWORK_ACTIVE_SIZE)];

static int network_monitored_buf [PS_MEM_GET_TOT_SIZE_OPT(NETWORK_MONITORED_NUM_BUFS,
                                                NETWORK_MONITORED_SIZE)];

static int phys_link_buf [PS_MEM_GET_TOT_SIZE_OPT(PHYS_LINK_NUM_BUFS,
                                                  PHYS_LINK_SIZE)];

static int policy_buf [PS_MEM_GET_TOT_SIZE_OPT(POLICY_NUM_BUFS,
                                               POLICY_SIZE)];

static int qos_1x_buf [PS_MEM_GET_TOT_SIZE_OPT(QOS_1X_NUM_BUFS,
                                               QOS_1X_SIZE)];

static int qos_default_buf [PS_MEM_GET_TOT_SIZE_OPT(QOS_DEFAULT_NUM_BUFS,
                                                    QOS_DEFAULT_SIZE)];

static int qos_secondary_buf [PS_MEM_GET_TOT_SIZE_OPT(QOS_SECONDARY_NUM_BUFS,
                                                      QOS_SECONDARY_SIZE)];

static int qos_net_initiated_req_buf [PS_MEM_GET_TOT_SIZE_OPT(QOS_NET_INITIATED_REQ_NUM_BUFS,
                                                          QOS_NET_INITIATED_REQ_SIZE)];

static int qos_net_initiated_buf [PS_MEM_GET_TOT_SIZE_OPT(QOS_NET_INITIATED_NUM_BUFS,
                                                          QOS_NET_INITIATED_SIZE)];

static int network_ipv6_address_buf [PS_MEM_GET_TOT_SIZE_OPT(IPV6_ADDRESS_NUM_BUFS,
                                                             IPV6_ADDRESS_SIZE)];

static int tech_umts_factory_buf [PS_MEM_GET_TOT_SIZE_OPT(TECH_UMTS_FACTORY_NUM_BUFS,
                                                          TECH_UMTS_FACTORY_SIZE)];

static int bearer_tech_buf [PS_MEM_GET_TOT_SIZE_OPT(BEARER_TECH_NUM_BUFS,
                                                    BEARER_TECH_SIZE)];

static int bearer_tech_ex_buf [PS_MEM_GET_TOT_SIZE_OPT(BEARER_TECH_EX_NUM_BUFS,
                                                    BEARER_TECH_EX_SIZE)];
#ifdef FEATUTE_DATA_PS_MCAST
static int mcast_manager_buf [PS_MEM_GET_TOT_SIZE_OPT(MCAST_MANAGER_NUM_BUFS,
                                                      MCAST_MANAGER_SIZE)];

static int mcast_manager_priv_buf [PS_MEM_GET_TOT_SIZE_OPT(MCAST_MANAGER_PRIV_NUM_BUFS,
                                                           MCAST_MANAGER_PRIV_SIZE)];
#endif // FEATUTE_DATA_PS_MCAST
static int qos_manager_buf [PS_MEM_GET_TOT_SIZE_OPT(QOS_MANAGER_NUM_BUFS,
                                                    QOS_MANAGER_SIZE)];

static int qos_flow_spec_buf [PS_MEM_GET_TOT_SIZE_OPT(QOS_FLOW_SPEC_NUM_BUFS,
                                                      QOS_FLOW_SPEC_SIZE)];

static int ps_qos_spec_wrapper_buf [PS_MEM_GET_TOT_SIZE_OPT(PS_QOS_SPEC_WRAPPER_NUM_BUFS,
                                                      PS_QOS_SPEC_WRAPPER_SIZE)];

static int ip_filter_spec_buf [PS_MEM_GET_TOT_SIZE_OPT(IP_FILTER_SPEC_NUM_BUFS,
                                                       IP_FILTER_SPEC_SIZE)];

static int ip_filter_reg_buf [PS_MEM_GET_TOT_SIZE_OPT(IP_FILTER_REG_NUM_BUFS,
                                                      IP_FILTER_REG_SIZE)];
#ifdef FEATUTE_DATA_PS_MCAST
static int mbms_join_info_buf [PS_MEM_GET_TOT_SIZE_OPT(MBMS_JOIN_INFO_NUM_BUFS,
                                                       MBMS_JOIN_INFO_SIZE)];
#endif // FEATUTE_DATA_PS_MCAST
static int mtpd_reg_buf [PS_MEM_GET_TOT_SIZE_OPT(MTPD_REG_NUM_BUFS,
                                                 MTPD_REG_SIZE)];

static int firewall_manager_buf [PS_MEM_GET_TOT_SIZE_OPT(FIREWALL_MANAGER_NUM_BUFS,
                                                         FIREWALL_MANAGER_SIZE)];

static int firewall_rule_buf [PS_MEM_GET_TOT_SIZE_OPT(FIREWALL_RULE_NUM_BUFS,
                                                      FIREWALL_RULE_SIZE)];

static int qos_secondaries_input_buf [PS_MEM_GET_TOT_SIZE_OPT(QOS_SECONDARIES_INPUT_NUM_BUFS,
                                                               QOS_SECONDARIES_INPUT_SIZE)];

static int qos_secondaries_output_buf [PS_MEM_GET_TOT_SIZE_OPT(QOS_SECONDARIES_OUTPUT_NUM_BUFS,
                                                              QOS_SECONDARIES_OUTPUT_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*---------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to actual object array.
---------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type   * event_manager_buf_hdr_ptr[EVENT_MANAGER_NUM_BUFS];
static EventManager          * event_manager_buf_ptr[EVENT_MANAGER_NUM_BUFS];
#ifdef FEATUTE_DATA_PS_MCAST
static ps_mem_buf_hdr_type   * mcast_session_buf_hdr_ptr[MCAST_SESSION_NUM_BUFS];
static MCastSession          * mcast_session_buf_ptr[MCAST_SESSION_NUM_BUFS];

static ps_mem_buf_hdr_type   * mcast_sessions_input_buf_hdr_ptr[MCAST_SESSIONS_INPUT_NUM_BUFS];
static MCastSessionsInput    * mcast_sessions_input_buf_ptr[MCAST_SESSIONS_INPUT_NUM_BUFS];

static ps_mem_buf_hdr_type   * mcast_sessions_output_buf_hdr_ptr[MCAST_SESSIONS_OUTPUT_NUM_BUFS];
static MCastSessionsOutput   * mcast_sessions_output_buf_ptr[MCAST_SESSIONS_OUTPUT_NUM_BUFS];

static ps_mem_buf_hdr_type   * mcast_session_priv_buf_hdr_ptr[MCAST_SESSION_PRIV_NUM_BUFS];
static MCastSessionPriv      * mcast_session_priv_buf_ptr[MCAST_SESSION_PRIV_NUM_BUFS];
#endif // FEATUTE_DATA_PS_MCAST

static ps_mem_buf_hdr_type   * network_1x_buf_hdr_ptr[NETWORK_1X_NUM_BUFS];
static Network1X             * network_1x_buf_ptr[NETWORK_1X_NUM_BUFS];

static ps_mem_buf_hdr_type   * network_factory_buf_hdr_ptr[NETWORK_FACTORY_NUM_BUFS];
static NetworkFactory        * network_factory_buf_ptr[NETWORK_FACTORY_NUM_BUFS];

static ps_mem_buf_hdr_type   * network_ipv6_buf_hdr_ptr[NETWORK_IPV6_NUM_BUFS];
static NetworkIPv6           * network_ipv6_buf_ptr[NETWORK_IPV6_NUM_BUFS];
#ifdef FEATUTE_DATA_PS_MCAST
static ps_mem_buf_hdr_type   * network_mbms_buf_hdr_ptr[NETWORK_MBMS_NUM_BUFS];
static MCastMBMSCtrl         * network_mbms_buf_ptr[NETWORK_MBMS_NUM_BUFS];
#endif // FEATUTE_DATA_PS_MCAST
static ps_mem_buf_hdr_type   * network_umts_buf_hdr_ptr[NETWORK_UMTS_NUM_BUFS];
static NetworkUMTS           * network_umts_buf_ptr[NETWORK_UMTS_NUM_BUFS];

static ps_mem_buf_hdr_type   * network_active_buf_hdr_ptr[NETWORK_ACTIVE_NUM_BUFS];
static NetworkActive         * network_active_buf_ptr[NETWORK_ACTIVE_NUM_BUFS];

static ps_mem_buf_hdr_type   * network_monitored_buf_hdr_ptr[NETWORK_MONITORED_NUM_BUFS];
static NetworkMonitored      * network_monitored_buf_ptr[NETWORK_MONITORED_NUM_BUFS];

static ps_mem_buf_hdr_type   * policy_buf_hdr_ptr[POLICY_NUM_BUFS];
static Policy                * policy_buf_ptr[POLICY_NUM_BUFS];

static ps_mem_buf_hdr_type   * phys_link_buf_hdr_ptr[PHYS_LINK_NUM_BUFS];
static PhysLink              * phys_link_buf_ptr[PHYS_LINK_NUM_BUFS];

static ps_mem_buf_hdr_type   * qos_1x_buf_hdr_ptr[QOS_1X_NUM_BUFS];
static QoS1X                 * qos_1x_buf_ptr[QOS_1X_NUM_BUFS];

static ps_mem_buf_hdr_type   * qos_default_buf_hdr_ptr[QOS_DEFAULT_NUM_BUFS];
static QoSDefault            * qos_default_buf_ptr[QOS_DEFAULT_NUM_BUFS];

static ps_mem_buf_hdr_type   * qos_secondary_buf_hdr_ptr[QOS_SECONDARY_NUM_BUFS];
static QoSSecondary          * qos_secondary_buf_ptr[QOS_SECONDARY_NUM_BUFS];

static ps_mem_buf_hdr_type   * qos_net_initiated_req_buf_hdr_ptr[QOS_NET_INITIATED_REQ_NUM_BUFS];
static QoSNetInitiatedReq    * qos_net_initiated_req_buf_ptr[QOS_NET_INITIATED_REQ_NUM_BUFS];

static ps_mem_buf_hdr_type   * qos_net_initiated_buf_hdr_ptr[QOS_NET_INITIATED_NUM_BUFS];
static QoSNetInitiated       * qos_net_initiated_buf_ptr[QOS_NET_INITIATED_NUM_BUFS];

static ps_mem_buf_hdr_type   * firewall_rule_buf_hdr_ptr[FIREWALL_RULE_NUM_BUFS];
static FirewallRule          * firewall_rule_buf_ptr[FIREWALL_RULE_NUM_BUFS];

static ps_mem_buf_hdr_type   * network_ipv6_address_buf_hdr_ptr[IPV6_ADDRESS_NUM_BUFS];
static NetworkIPv6Address    * network_ipv6_address_buf_ptr[IPV6_ADDRESS_NUM_BUFS];

static ps_mem_buf_hdr_type   * tech_umts_factory_buf_hdr_ptr[TECH_UMTS_FACTORY_NUM_BUFS];
static TechUMTSFactory       * tech_umts_factory_buf_ptr[TECH_UMTS_FACTORY_NUM_BUFS];

static ps_mem_buf_hdr_type   * bearer_tech_buf_hdr_ptr[BEARER_TECH_NUM_BUFS];
static BearerTech            * bearer_tech_buf_ptr[BEARER_TECH_NUM_BUFS];

static ps_mem_buf_hdr_type   * bearer_tech_ex_buf_hdr_ptr[BEARER_TECH_EX_NUM_BUFS];
static BearerTechEx          * bearer_tech_ex_buf_ptr[BEARER_TECH_EX_NUM_BUFS];
#ifdef FEATUTE_DATA_PS_MCAST
static ps_mem_buf_hdr_type   * mcast_manager_buf_hdr_ptr[MCAST_MANAGER_NUM_BUFS];
static MCastManager          * mcast_manager_buf_ptr[MCAST_MANAGER_NUM_BUFS];

static ps_mem_buf_hdr_type   * mcast_manager_priv_buf_hdr_ptr[MCAST_MANAGER_PRIV_NUM_BUFS];
static MCastManagerPriv      * mcast_manager_priv_buf_ptr[MCAST_MANAGER_PRIV_NUM_BUFS];
#endif // FEATUTE_DATA_PS_MCAST
static ps_mem_buf_hdr_type   * qos_manager_buf_hdr_ptr[QOS_MANAGER_NUM_BUFS];
static QoSManager            * qos_manager_buf_ptr[QOS_MANAGER_NUM_BUFS];

static ps_mem_buf_hdr_type   * qos_flow_spec_buf_hdr_ptr[QOS_FLOW_SPEC_NUM_BUFS];
static QoSFlowSpec           * qos_flow_spec_buf_ptr[QOS_FLOW_SPEC_NUM_BUFS];

static ps_mem_buf_hdr_type   * ps_qos_spec_wrapper_buf_hdr_ptr[PS_QOS_SPEC_WRAPPER_NUM_BUFS];
static PSQoSSpecWrapper      * ps_qos_spec_wrapper_buf_ptr[PS_QOS_SPEC_WRAPPER_NUM_BUFS];

static ps_mem_buf_hdr_type   * firewall_manager_buf_hdr_ptr[FIREWALL_MANAGER_NUM_BUFS];
static FirewallManager       * firewall_manager_buf_ptr[FIREWALL_MANAGER_NUM_BUFS];

static ps_mem_buf_hdr_type   * ip_filter_spec_buf_hdr_ptr[IP_FILTER_SPEC_NUM_BUFS];
static IPFilterSpec          * ip_filter_spec_buf_ptr[IP_FILTER_SPEC_NUM_BUFS];

static ps_mem_buf_hdr_type   * ip_filter_reg_buf_hdr_ptr[IP_FILTER_REG_NUM_BUFS];
static IPFilterReg           * ip_filter_reg_buf_ptr[IP_FILTER_REG_NUM_BUFS];

static ps_mem_buf_hdr_type   * mbms_join_info_buf_hdr_ptr[MBMS_JOIN_INFO_NUM_BUFS];
static MBMSSpec              * mbms_join_info_buf_ptr[MBMS_JOIN_INFO_NUM_BUFS];

static ps_mem_buf_hdr_type   * mtpd_reg_buf_hdr_ptr[MTPD_REG_NUM_BUFS];
static MTPDReg               * mtpd_reg_buf_ptr[MTPD_REG_NUM_BUFS];

static ps_mem_buf_hdr_type   * net_nat_session_buf_hdr_ptr[NET_NAT_SESSION_NUM_BUFS];
static NatSession            * net_nat_session_buf_ptr[NET_NAT_SESSION_NUM_BUFS];

static ps_mem_buf_hdr_type   * net_nat_manager_buf_hdr_ptr[NET_NAT_MANAGER_NUM_BUFS];
static NatManager            * net_nat_manager_buf_ptr[NET_NAT_MANAGER_NUM_BUFS];

static ps_mem_buf_hdr_type   * qos_secondaries_input_buf_hdr_ptr[QOS_SECONDARIES_INPUT_NUM_BUFS];
static QoSSecondariesInput   * qos_secondaries_input_buf_ptr[QOS_SECONDARIES_INPUT_NUM_BUFS];

static ps_mem_buf_hdr_type   * qos_secondaries_output_buf_hdr_ptr[QOS_SECONDARIES_OUTPUT_NUM_BUFS];
static QoSSecondariesOutput  * qos_secondaries_output_buf_ptr[QOS_SECONDARIES_OUTPUT_NUM_BUFS];

#endif /* FEATURE_DATA_PS_MEM_DEBUG */


void MemoryManager::MemPoolInit
(
  void
)
{
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO1_0 ("MemoryManager::MemPoolInit(): "
                   "Initing mem pools for DSNet objects");

  /*-------------------------------------------------------------------------
    Initialize DSNet mem pools
  -------------------------------------------------------------------------*/
  if (ps_mem_pool_init(PS_MEM_DS_NET_EVENT_MANAGER,
                       event_manager_buf,
                       EVENT_MANAGER_SIZE,
                       EVENT_MANAGER_NUM_BUFS,
                       EVENT_MANAGER_HIGH_WM,
                       EVENT_MANAGER_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) event_manager_buf_hdr_ptr,
                       (int *) event_manager_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }

#ifdef FEATUTE_DATA_PS_MCAST
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_MCAST_SESSION,
                           mcast_session_buf,
                           MCAST_SESSION_SIZE,
                           MCAST_SESSION_NUM_BUFS,
                           MCAST_SESSION_HIGH_WM,
                           MCAST_SESSION_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) mcast_session_buf_hdr_ptr,
                           (int *) mcast_session_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT( PS_MEM_DS_NET_MCAST_SESSION_PRIV,
                            mcast_session_priv_buf,
                            MCAST_SESSION_PRIV_SIZE,
                            MCAST_SESSION_PRIV_NUM_BUFS,
                            MCAST_SESSION_PRIV_HIGH_WM,
                            MCAST_SESSION_PRIV_LOW_WM,
                            NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                            (int *) mcast_session_priv_buf_hdr_ptr,
                            (int *) mcast_session_priv_buf_ptr
#else
                            NULL,
                            NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT( PS_MEM_DS_NET_MCAST_SESSIONS_INPUT,
                            mcast_sessions_input_buf,
                            MCAST_SESSIONS_INPUT_SIZE,
                            MCAST_SESSIONS_INPUT_NUM_BUFS,
                            MCAST_SESSIONS_INPUT_HIGH_WM,
                            MCAST_SESSIONS_INPUT_LOW_WM,
                            NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                            (int *) mcast_sessions_input_buf_hdr_ptr,
                            (int *) mcast_sessions_input_buf_ptr
#else
                            NULL,
                            NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT( PS_MEM_DS_NET_MCAST_SESSIONS_OUTPUT,
                            mcast_sessions_output_buf,
                            MCAST_SESSIONS_OUTPUT_SIZE,
                            MCAST_SESSIONS_OUTPUT_NUM_BUFS,
                            MCAST_SESSIONS_OUTPUT_HIGH_WM,
                            MCAST_SESSIONS_OUTPUT_LOW_WM,
                            NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                            (int *) mcast_sessions_output_buf_hdr_ptr,
                            (int *) mcast_sessions_output_buf_ptr
#else
                            NULL,
                            NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }
#endif // FEATUTE_DATA_PS_MCAST

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_NETWORK_1X,
                           network_1x_buf,
                           NETWORK_1X_SIZE,
                           NETWORK_1X_NUM_BUFS,
                           NETWORK_1X_HIGH_WM,
                           NETWORK_1X_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) network_1x_buf_hdr_ptr,
                           (int *) network_1x_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (ps_mem_pool_init(PS_MEM_DS_NET_NETWORK_FACTORY,
                       network_factory_buf,
                       NETWORK_FACTORY_SIZE,
                       NETWORK_FACTORY_NUM_BUFS,
                       NETWORK_FACTORY_HIGH_WM,
                       NETWORK_FACTORY_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) network_factory_buf_hdr_ptr,
                       (int *) network_factory_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_NETWORK_IPV6,
                           network_ipv6_buf,
                           NETWORK_IPV6_SIZE,
                           NETWORK_IPV6_NUM_BUFS,
                           NETWORK_IPV6_HIGH_WM,
                           NETWORK_IPV6_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) network_ipv6_buf_hdr_ptr,
                           (int *) network_ipv6_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }
#ifdef FEATUTE_DATA_PS_MCAST
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_NETWORK_MBMS,
                           network_mbms_buf,
                           NETWORK_MBMS_SIZE,
                           NETWORK_MBMS_NUM_BUFS,
                           NETWORK_MBMS_HIGH_WM,
                           NETWORK_MBMS_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) network_mbms_buf_hdr_ptr,
                           (int *) network_mbms_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }
#endif //FEATUTE_DATA_PS_MCAST
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_NETWORK_UMTS,
                           network_umts_buf,
                           NETWORK_UMTS_SIZE,
                           NETWORK_UMTS_NUM_BUFS,
                           NETWORK_UMTS_HIGH_WM,
                           NETWORK_UMTS_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) network_umts_buf_hdr_ptr,
                           (int *) network_umts_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_NETWORK_ACTIVE,
                           network_active_buf,
                           NETWORK_ACTIVE_SIZE,
                           NETWORK_ACTIVE_NUM_BUFS,
                           NETWORK_ACTIVE_HIGH_WM,
                           NETWORK_ACTIVE_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                          (int *) network_active_buf_hdr_ptr,
                          (int *) network_active_buf_ptr
#else
                          NULL,
                          NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                         ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_NETWORK_MONITORED,
                           network_monitored_buf,
                           NETWORK_MONITORED_SIZE,
                           NETWORK_MONITORED_NUM_BUFS,
                           NETWORK_MONITORED_HIGH_WM,
                           NETWORK_MONITORED_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                          (int *) network_monitored_buf_hdr_ptr,
                          (int *) network_monitored_buf_ptr
#else
                          NULL,
                          NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                         ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_POLICY,
                           policy_buf,
                           POLICY_SIZE,
                           POLICY_NUM_BUFS,
                           POLICY_HIGH_WM,
                           POLICY_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) policy_buf_hdr_ptr,
                           (int *) policy_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }


  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_PHYS_LINK,
                           phys_link_buf,
                           PHYS_LINK_SIZE,
                           PHYS_LINK_NUM_BUFS,
                           PHYS_LINK_HIGH_WM,
                           PHYS_LINK_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) phys_link_buf_hdr_ptr,
                           (int *) phys_link_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_QOS_1X,
                           qos_1x_buf,
                           QOS_1X_SIZE,
                           QOS_1X_NUM_BUFS,
                           QOS_1X_HIGH_WM,
                           QOS_1X_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) qos_1x_buf_hdr_ptr,
                           (int *) qos_1x_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_QOS_DEFAULT,
                           qos_default_buf,
                           QOS_DEFAULT_SIZE,
                           QOS_DEFAULT_NUM_BUFS,
                           QOS_DEFAULT_HIGH_WM,
                           QOS_DEFAULT_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) qos_default_buf_hdr_ptr,
                           (int *) qos_default_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }
  
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_QOS_SECONDARY,
                           qos_secondary_buf,
                           QOS_SECONDARY_SIZE,
                           QOS_SECONDARY_NUM_BUFS,
                           QOS_SECONDARY_HIGH_WM,
                           QOS_SECONDARY_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) qos_secondary_buf_hdr_ptr,
                           (int *) qos_secondary_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_QOS_NET_INITIATED_REQ,
     qos_net_initiated_req_buf,
     QOS_NET_INITIATED_REQ_SIZE,
     QOS_NET_INITIATED_REQ_NUM_BUFS,
     QOS_NET_INITIATED_REQ_HIGH_WM,
     QOS_NET_INITIATED_REQ_LOW_WM,
     NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
     (int *) qos_net_initiated_req_buf_hdr_ptr,
     (int *) qos_net_initiated_req_buf_ptr
#else
     NULL,
     NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
     ) == -1)
  {
     goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_QOS_NET_INITIATED,
     qos_net_initiated_buf,
     QOS_NET_INITIATED_SIZE,
     QOS_NET_INITIATED_NUM_BUFS,
     QOS_NET_INITIATED_HIGH_WM,
     QOS_NET_INITIATED_LOW_WM,
     NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
     (int *) qos_net_initiated_buf_hdr_ptr,
     (int *) qos_net_initiated_buf_ptr
#else
     NULL,
     NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
     ) == -1)
  {
     goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_NETWORK_IPV6_ADDRESS,
    network_ipv6_address_buf,
    IPV6_ADDRESS_SIZE,
    IPV6_ADDRESS_NUM_BUFS,
    IPV6_ADDRESS_HIGH_WM,
    IPV6_ADDRESS_LOW_WM,
    NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
    (int *) network_ipv6_address_buf_hdr_ptr,
    (int *) network_ipv6_address_buf_ptr
#else
    NULL,
    NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
    ) == -1)
  {
    goto bail;
  }
  
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_TECH_UMTS_FACTORY,
                       tech_umts_factory_buf,
                       TECH_UMTS_FACTORY_SIZE,
                       TECH_UMTS_FACTORY_NUM_BUFS,
                       TECH_UMTS_FACTORY_HIGH_WM,
                       TECH_UMTS_FACTORY_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) tech_umts_factory_buf_hdr_ptr,
                       (int *) tech_umts_factory_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_BEARER_TECH,
                       bearer_tech_buf,
                       BEARER_TECH_SIZE,
                       BEARER_TECH_NUM_BUFS,
                       BEARER_TECH_HIGH_WM,
                       BEARER_TECH_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) bearer_tech_buf_hdr_ptr,
                       (int *) bearer_tech_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_BEARER_TECH_EX,
    bearer_tech_ex_buf,
    BEARER_TECH_EX_SIZE,
    BEARER_TECH_EX_NUM_BUFS,
    BEARER_TECH_EX_HIGH_WM,
    BEARER_TECH_EX_LOW_WM,
    NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
    (int *) bearer_tech_ex_buf_hdr_ptr,
    (int *) bearer_tech_ex_buf_ptr
#else
    NULL,
    NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
    ) == -1)
  {
    goto bail;
  }

#ifdef FEATUTE_DATA_PS_MCAST
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_MCAST_MANAGER,
                       mcast_manager_buf,
                       MCAST_MANAGER_SIZE,
                       MCAST_MANAGER_NUM_BUFS,
                       MCAST_MANAGER_HIGH_WM,
                       MCAST_MANAGER_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) mcast_manager_buf_hdr_ptr,
                       (int *) mcast_manager_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT( PS_MEM_DS_NET_MCAST_MANAGER_PRIV,
                            mcast_manager_priv_buf,
                            MCAST_MANAGER_PRIV_SIZE,
                            MCAST_MANAGER_PRIV_NUM_BUFS,
                            MCAST_MANAGER_PRIV_HIGH_WM,
                            MCAST_MANAGER_PRIV_LOW_WM,
                            NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                            (int *) mcast_manager_priv_buf_hdr_ptr,
                            (int *) mcast_manager_priv_buf_ptr
#else
                            NULL,
                            NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }
#endif // FEATUTE_DATA_PS_MCAST
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_QOS_MANAGER,
                       qos_manager_buf,
                       QOS_MANAGER_SIZE,
                       QOS_MANAGER_NUM_BUFS,
                       QOS_MANAGER_HIGH_WM,
                       QOS_MANAGER_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) qos_manager_buf_hdr_ptr,
                       (int *) qos_manager_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_QOS_FLOW_SPEC,
                       qos_flow_spec_buf,
                       QOS_FLOW_SPEC_SIZE,
                       QOS_FLOW_SPEC_NUM_BUFS,
                       QOS_FLOW_SPEC_HIGH_WM,
                       QOS_FLOW_SPEC_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) qos_flow_spec_buf_hdr_ptr,
                       (int *) qos_flow_spec_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_PS_QOS_SPEC_WRAPPER,
                       ps_qos_spec_wrapper_buf,
                       PS_QOS_SPEC_WRAPPER_SIZE,
                       PS_QOS_SPEC_WRAPPER_NUM_BUFS,
                       PS_QOS_SPEC_WRAPPER_HIGH_WM,
                       PS_QOS_SPEC_WRAPPER_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) ps_qos_spec_wrapper_buf_hdr_ptr,
                       (int *) ps_qos_spec_wrapper_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_IP_FILTER_SPEC,
                       ip_filter_spec_buf,
                       IP_FILTER_SPEC_SIZE,
                       IP_FILTER_SPEC_NUM_BUFS,
                       IP_FILTER_SPEC_HIGH_WM,
                       IP_FILTER_SPEC_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) ip_filter_spec_buf_hdr_ptr,
                       (int *) ip_filter_spec_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_IP_FILTER_REG,
                       ip_filter_reg_buf,
                       IP_FILTER_REG_SIZE,
                       IP_FILTER_REG_NUM_BUFS,
                       IP_FILTER_REG_HIGH_WM,
                       IP_FILTER_REG_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) ip_filter_reg_buf_hdr_ptr,
                       (int *) ip_filter_reg_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }

#ifdef FEATUTE_DATA_PS_MCAST
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_MBMS_JOIN_INFO,
                       mbms_join_info_buf,
                       MBMS_JOIN_INFO_SIZE,
                       MBMS_JOIN_INFO_NUM_BUFS,
                       MBMS_JOIN_INFO_HIGH_WM,
                       MBMS_JOIN_INFO_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) mbms_join_info_buf_hdr_ptr,
                       (int *) mbms_join_info_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }
#endif //FEATUTE_DATA_PS_MCAST

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_MTPD_REG,
                       mtpd_reg_buf,
                       MTPD_REG_SIZE,
                       MTPD_REG_NUM_BUFS,
                       MTPD_REG_HIGH_WM,
                       MTPD_REG_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) mtpd_reg_buf_hdr_ptr,
                       (int *) mtpd_reg_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_FIREWALL_RULE,
                           firewall_rule_buf,
                           FIREWALL_RULE_SIZE,
                           FIREWALL_RULE_NUM_BUFS,
                           FIREWALL_RULE_HIGH_WM,
                           FIREWALL_RULE_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) firewall_rule_buf_hdr_ptr,
                           (int *) firewall_rule_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_FIREWALL_MANAGER,
                           firewall_manager_buf,
                           FIREWALL_MANAGER_SIZE,
                           FIREWALL_MANAGER_NUM_BUFS,
                           FIREWALL_MANAGER_HIGH_WM,
                           FIREWALL_MANAGER_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) firewall_manager_buf_hdr_ptr,
                           (int *) firewall_manager_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_QOS_SECONDARIES_INPUT,
                           qos_secondaries_input_buf,
                           QOS_SECONDARIES_INPUT_SIZE,
                           QOS_SECONDARIES_INPUT_NUM_BUFS,
                           QOS_SECONDARIES_INPUT_HIGH_WM,
                           QOS_SECONDARIES_INPUT_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) qos_secondaries_input_buf_hdr_ptr,
                           (int *) qos_secondaries_input_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_NET_QOS_SECONDARIES_OUTPUT,
                           qos_secondaries_output_buf,
                           QOS_SECONDARIES_OUTPUT_SIZE,
                           QOS_SECONDARIES_OUTPUT_NUM_BUFS,
                           QOS_SECONDARIES_OUTPUT_HIGH_WM,
                           QOS_SECONDARIES_OUTPUT_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) qos_secondaries_output_buf_hdr_ptr,
                           (int *) qos_secondaries_output_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }

  return;

bail:
  DATA_ERR_FATAL("MemoryManager::MemPoolInit(): Can't init DSNET pools");
  return;

} /* MemPoolInit() */

} /* Namespace Net */
} /* Namespace ds */
