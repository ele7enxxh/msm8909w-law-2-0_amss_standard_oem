#ifndef PS_IFACE_IOCTL_H
#define PS_IFACE_IOCTL_H

/*===========================================================================

                          P S _ I F A C E _ I O C T L. H

DESCRIPTION

  The Protocol Services interface file. This contains variables and
  definitions used by the various modules to access interface definitions.

Copyright (c) 2002-2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iface_ioctl.h_v   1.1   12 Feb 2003 12:08:04   akuzhiyi  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_iface_ioctl.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/01/12    ss     Added TFTP ALG entry for enable/disable ioctl.
08/15/12    mp     Added new IOCTLs for Get/Set NAT type.
02/22/12    ss     NAT IOCTLs for Enable/Disable STA mode.
02/01/12    am     Added new IOCTL for getting firewall setting.
01/04/12    hs     Added support for eMBMS
11/27/11    ss     Added IOCTLs for enable/disable and retrieve ALG status.
10/28/11    am     Added new IOCTLs for Softap functionality.
10/03/11    ash    Added new ioctl PS_IFACE_GET_IPV6_REMAINING_ROUTER_LIFETIME
09/14/11    ea     Added PS_IFACE_IOCTL_QOS_NET_INITIATED_REAL_TIME_CHECK
08/31/11    hs     Added ioctl PS_IFACE_IOCTL_GET_SUBNET_MASK.
08/30/11    hs     Added ioctl PS_IFACE_IOCTL_GET_GATEWAY_ADDR.
08/30/11    aa     Added ioctl for Network Initiated QoS
08/24/11    hs     Added ps_iface_ioctl_device_name_type and
                   PS_IFACE_IOCTL_GET_DEVICE_NAME
08/09/11    bd     Added ioctl to retrieve pdp context id
06/23/11    bd     Added ps_iface_ioctl_enable_hdr_slotted_mode ioctl struct
04/27/11    sy     Added Get Session Params ioctl.
04/27/11    sy     Added call type none to ps_iface_ioctl_data_call_enum_type.
04/27/11    sy     Added alias PS_IFACE_IOCTL_707_GET_SIP_MIP_DATA_CALL_TYPE.
04/27/11    sy     Added structure for PS_IFACE_IOCTL_707_ENABLE_HDR_SLOTTED_MODE.
02/08/11    vm     Change deprecated sockaddr* to ps_sockaddr*
01/25/11    dm     Changed the name to ps_iface_ioctl_iface_stats_type.
12/16/10    dm     Added IOCTL to get/set L2TP/PPTP VPN Passthrough.
11/17/10    jy     New IOCTL: PS_IFACE_IOCTL_RECONFIG
11/16/10    dm     Added IOCTL to get NAT public IP address
10/25/10    rt     Added GET_IFACE_STATS and RESET_IFACE_STATS IOCTL's.
10/11/10    dm     Added DMZ IOCTLs.
08/31/10    dm     Updated NAT IOCTL API signature.
07/30/10    bq     Added NAT_IPSEC_VPN_PASSTHROUGH.
07/02/10    cp     DHCP ARP cache IOCTLs update. DHCP header inclusion update.
06/29/10    cp     Changed the ps_iface_ioctl_dhcp_arp_cache_update_type
                   structure to avoid page fault.
06/29/10    cp     IOCTL changes for Soft AP DHCP server.
06/29/10    bq     Added NAT related Configuration
05/27/10    hs     Added new IOCTLs for configuring A2
05/12/2010  jy     New enum PS_IFACE_IOCTL_GET_EPC_HANDOFF_CTXT
04/01/08    dm     Modifying QoS conrol mode enum values
12/14/08    pp     Common Modem Interface: Public/Private API split.
09/02/08    dm     Added Network initiated QoS support
05/22/08    dm     BCMCS 2.0 Phase 3 support.
04/29/08    pp     IS707b: Added GET/SET Hysteresis Activation Timer IOCTLs.
04/18/08    dm     BCMCS 2.0 support. Added two new ioctl's:
                   PS_IFACE_IOCTL_BCMCS_BOM_CACHING_SETUP and
                   PS_IFACE_IOCTL_BCMCS_ENABLE_HANDOFF_REG and
04/10/08    ssh    Added ioctl for bootstrap configuration
04/03/08    am     BCMCS 2.0 Support.
01/30/08    dm     Removed Feature FEATURE_JCDMA_2
07/31/07    sv     Added support for MBMS context act/deact ioctls.
04/17/07    es     Added REFRESH_DHCP_CONFIG_INFO ioctl, removed
                   GET_DHCP_CONFIG_INFO IOCTL.
03/27/07    msr    Added API for RF conditions and HDR-1x handdown
02/05/07    sv     Added support for IM_CN flag.
02/01/07    hm     Added IOCTL to get fresh DHCP configuration
11/14/06    rsj    Added changes for JCDMA Session Timer. Created two new
                   IOCTL's: PS_IFACE_IOCTL_SET_SESSION_TIMER and
                   PS_IFACE_IOCTL_GET_SESSION_TIMER.
11/04/06    hm     Added ioctl support for domain name search list for DHCPv6
11/02/06    mct    Added support for RFC3041: IPv6 Privacy Extensions.
08/14/06    sv     Merged slotted/Mode attribute ioctl.
08/04/06    kvd    Added meta-comments to ip_address struct for 7500 BCMCS.
                   The meta-comments wont affect other targets, so its a no-op.
07/10/06    mct    Changed the bundle request opcode enum to a mask.
06/01/06    mct    Added support for getting all DNS addresses on an interface.
04/30/06    rt     Code for SIP server address and domain names.
02/13/05    mct    Added support for GET_NETWORK_SUPPORTED_QOS_PROFILES ioctl.
12/01/05    mct    Added support for the ON_QOS_AWARE_SYSTEM ioctl.
11/07/05    mct    Changed request_ex ptr to ptr ptr.
10/12/05    mct    Made some naming changes to the IOCTL.
10/04/05    msr    Changed qos in ps_iface_ioctl_qos_request_type to a pointer
09/29/05    msr    Added PS_IFACE_IOCTL_IS_LAPTOP_CALL_ACTIVE
09/15/05    mct    Featurized BCMCS code.
09/13/05    sv     Added support for new HDR rev0 rate inertia and HDR HPT mode
                   ioctls.
09/01/05    mct    Added support for BCMCS_DB_UPDATE.
09/01/05    rt     Moved ps_iface_data_bearer_rate definition to
                   ps_iface_defs.h
08/15/05    mct    Added support for QOS_CONFIGURE and QOS_REQUEST_EX. Also
                   added suspend_ex, resume_ex, and release_ex.
08/14/05    rt     Added new ioctl's PS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY
                   and PS_IFACE_IOCTL_GET_DATA_BEARER_RATE.
07/28/05    rt     Added new ioctl's PS_IFACE_IOCTL_GET_IFACE_NAME and
                   PS_IFACE_IOCTL_GET_IP_FAMILY.
05/03/05    msr    Moved flow and phys link IOCTLs to respective files
04/18/05    mct    Added PS_IFACE_IOCTL_MCAST_JOIN and
                   PS_IFACE_IOCTL_MCAST_LEAVE.
04/18/05    ks     Added new ioctl, PS_FLOW_IOCTL_GET_TX_QUEUE_LEVEL.
04/17/05    msr    Added PS_FLOW_IOCTL_QOS_RESUME and
                   PS_FLOW_IOCTL_QOS_SUSPEND and moved QOS_RELEASE,
                   QOS_MODIFY, and QOS_GET_FLOW_SPEC to flow IOCTLs.
04/16/05    ks     Added support for new ioctl, PS_IFACE_IOCTL_GET_HW_ADDR
11/02/04    mct    Pass only the policy to make MT more generic.
11/02/04    vp     Renamed and renumbered PS_IFACE_IOCTL_GET_PHYS_LINK_STATE
                   to PS_PHYS_LINK_IOCTL_GET_STATE.
10/22/04    mct    Added MT reg/dereg types.
10/06/04    sv     changed GO_ACTIVE and GO_DORMANT ioctls to phys_link ioctls.
02/12/03    aku    Changed errnos to use ones in defined in dserrno.h
11/10/02    aku     created file
===========================================================================*/

#include "comdef.h"
#include "ps_iface_defs.h"
#include "ps_in.h"
#include "dsbcmcs_defs.h"
#include "ps_qos_defs.h"
#include "ps_iface_ipfltr.h"
#include "ps_ppp_ext.h"
#include "dhcp_server_defs.h"
#include "dsumtspdpreg.h"
#include "ps_iface_embms.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      REGIONAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                          IOCTL Definitions

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*---------------------------------------------------------------------------
  Definition of various interface control operations. Please do NOT change
  the values of the existing enumerated types when adding new ones (to ensure
  that object code released with previous versions of the code do not break).
  Each of the following control operations have associated tyepdef (with
  corresponding names) that are passed in or returned for get/set operations.

  Note:
    GET operations are assigned even numbers
    SET operations are assigned odd numbers.

    Also ensure that symetric operations are grouped.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IFACE_IOCTL_MIN                      = 0,

  /*-------------------------------------------------------------------------

                              COMMON IOCTLS

    The following operations are common to all interfaces.
  -------------------------------------------------------------------------*/
  PS_IFACE_IOCTL_GET_IPV4_ADDR            = 0, /* Get IPV4 addr of iface   */
  PS_IFACE_IOCTL_GET_IPV6_ADDR            = 2, /* Get IPV6 addr of iface   */
  PS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR   = 4, /* Get Primary DNS (IPV4) addr
                                                                  of iface */
  PS_IFACE_IOCTL_GET_IPV6_PRIM_DNS_ADDR   = 6, /* Get Primary DNS (IPV6) addr
                                                                  of iface */
  PS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR   = 8, /* Get Secondary DNS (IPV4)
                                                             addr of iface */
  PS_IFACE_IOCTL_GET_IPV6_SECO_DNS_ADDR   = 10, /* Get Secondary DNS (IPV6)
                                                             addr of iface */
  PS_IFACE_IOCTL_GET_ALL_DNS_ADDRS        = 12, /* Retrieve all DNS
                                                   addresses of the iface  */
  PS_IFACE_IOCTL_GET_MTU                  = 14, /* Get MTU of iface        */
  PS_IFACE_IOCTL_GET_IP_ADDR              = 16, /* Get IPADDR of iface     */
  PS_IFACE_IOCTL_GET_STATE                = 18,/* Get state of iface       */
  PS_IFACE_IOCTL_REG_EVENT_CB             = 20,/* Register callback for
                                                  events. Note that app can
                                                  have only one callback per
                                                  interface for each event */
  PS_IFACE_IOCTL_DEREG_EVENT_CB           = 21,/* Deregister event callback*/
  PS_IFACE_IOCTL_GET_ALL_IFACES           = 22,/* Get all enabled ifaces.
                                                  Note for that this IOCTL,
                                                  a NULL id_ptr has to be
                                                  passed as a parameter    */
  PS_IFACE_IOCTL_QOS_REQUEST              = 23, /* Request for a specific
                                                   QOS                     */
  PS_IFACE_IOCTL_QOS_REQUEST_EX           = 25, /* Request multiple QOS
                                                   instances               */
  PS_IFACE_IOCTL_GET_HW_ADDR              = 26, /* Request for HW address
                                                   of the iface */
  PS_IFACE_IOCTL_GET_IFACE_NAME           = 28, /*Get the iface name       */
  PS_IFACE_IOCTL_GET_IP_FAMILY            = 30, /*Get IP family of iface   */

  PS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY    = 32, /*Get the bearer technology*/
  PS_IFACE_IOCTL_GET_DATA_BEARER_RATE     = 34, /*Get DATA Bearer rate.    */
  PS_IFACE_IOCTL_IS_LAPTOP_CALL_ACTIVE    = 35, /* Check if iface is in
                                                   laptop call             */
  PS_IFACE_IOCTL_GET_SIP_SERV_ADDR        = 36, /*Get SIP Server Addresses.*/
  PS_IFACE_IOCTL_GET_SIP_SERV_DOMAIN_NAMES= 38, /*Get SIP Server Domain
                                                  Names.*/
  PS_IFACE_IOCTL_GENERATE_PRIV_IPV6_ADDR  = 40, /*Generate a new private
                                                  IPv6 address             */
  PS_IFACE_IOCTL_GET_DOMAIN_NAME_SEARCH_LIST=42, /*Get the domain name list*/
  PS_IFACE_IOCTL_REFRESH_DHCP_CONFIG_INFO = 43, /* Refresh DHCP config info*/
  PS_IFACE_IOCTL_GET_RF_CONDITIONS        = 44, /* Get the RF conditions   */
  PS_IFACE_IOCTL_QOS_GET_MODE             = 46, /* Get QoS control mode    */
  PS_IFACE_IOCTL_ENABLE_FIREWALL          = 47,
  PS_IFACE_IOCTL_ADD_FIREWALL_RULE        = 48,
  PS_IFACE_IOCTL_DELETE_FIREWALL_RULE     = 49,
  PS_IFACE_IOCTL_GET_FIREWALL_RULE        = 50,
  PS_IFACE_IOCTL_GET_FIREWALL_TABLE       = 51,
  PS_IFACE_IOCTL_DISABLE_FIREWALL         = 52,

  PS_IFACE_IOCTL_DHCP_ARP_CACHE_UPDATE    = 53,   /* Update the ARP cache  */
  PS_IFACE_IOCTL_DHCP_ARP_CACHE_CLEAR     = 54,   /* Clear the ARP cache   */
  PS_IFACE_IOCTL_DHCP_SERVER_GET_DEVICE_INFO = 55,/* Get DHCP connected
                                                     devices info          */
  PS_IFACE_IOCTL_GET_IFACE_STATS          = 56,  /* Get the iface stats    */
  PS_IFACE_IOCTL_RESET_IFACE_STATS        = 57,  /* Reset the iface stats  */

  /** Request to monitor Network Initiated QoS (per specific qos filters)  */
  PS_IFACE_IOCTL_QOS_NET_INITIATED_REQUEST = 58,
  /** Request to stop monitoring Network Initiated QoS
      (per specific previous request)                                      */
  PS_IFACE_IOCTL_QOS_NET_INITIATED_RELEASE = 59,
  /** Request to check if there is an already existing network initiated flow
      that meets the provided QoS specification.                           */
  PS_IFACE_IOCTL_QOS_NET_INITIATED_REAL_TIME_CHECK = 60,
  PS_IFACE_IOCTL_GET_FIREWALL_SETTINGS     = 61,
  PS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY_EX    = 62, /*Get the bearer technology*/
  PS_IFACE_IOCTL_SET_DATA_PATH_BRIDGE_MODE   = 63, /* Set data path bridge mode*/
  PS_IFACE_IOCTL_GET_DATA_PATH_BRIDGE_MODE   = 64, /* Get data path */
  PS_IFACE_IOCTL_GET_OP_RES_PCO              = 65, /* Get operator reserved pco */
  PS_IFACE_IOCTL_GET_CHANGED_APN_PARAM_INFO  = 66, /* Get Changed APN parameter info*/
  PS_IFACE_IOCTL_SET_SILENT_REDIAL           = 67, /* Do Silent redial by MH*/

  PS_IFACE_IOCTL_SET_PDN_TEARDOWN_CONFIG = 68, /* Set the PDN tear down
                                                  configuration */
  PS_IFACE_IOCTL_GET_PDN_TEARDOWN_CONFIG = 69, /* Get the PDN tear down 
                                                  configuration*/  

  /*-------------------------------------------------------------------------
    The following operations are common, but implementation is specific to
    a particular iface. Some may not even be supported by a particular iface.
  -------------------------------------------------------------------------*/
  PS_IFACE_IOCTL_GO_NULL                  = 105,/* Go NULL                 */
  PS_IFACE_IOCTL_IPCP_DNS_OPT             = 107,/* IPCP dns option         */
  PS_IFACE_IOCTL_MT_REG_CB                = 109,/* Mobile Terminated reg   */
  PS_IFACE_IOCTL_MT_DEREG_CB              = 111,/* Mobile Terminated dereg */
  PS_IFACE_IOCTL_MCAST_JOIN               = 113,/* Multicast join          */
  PS_IFACE_IOCTL_MCAST_LEAVE              = 115,/* Multicast leave         */
  PS_IFACE_IOCTL_ON_QOS_AWARE_SYSTEM      = 116,/* On QOS aware system     */
  PS_IFACE_IOCTL_GET_ALL_V6_PREFIXES      = 118,/* Get all v6 prefixes     */
  PS_IFACE_IOCTL_MCAST_JOIN_EX            = 119,/* Multiple Multicast join */
  PS_IFACE_IOCTL_MCAST_LEAVE_EX           = 120,/* Multiple Multicast leave */
  PS_IFACE_IOCTL_MCAST_REGISTER_EX        = 121,/* Multiple Multicast Reg   */
  PS_IFACE_IOCTL_GET_HANDOFF_CTXT         = 122,/* Ctxt from EPC aware sys's*/
  PS_IFACE_IOCTL_GET_EPC_HANDOFF_CTXT     = PS_IFACE_IOCTL_GET_HANDOFF_CTXT, 
  PS_IFACE_IOCTL_RECONFIG                 = 123,/* Re-config iface          */
  PS_IFACE_IOCTL_GET_DEVICE_NAME          = 124,
  /**< Get HLOS specific device name corresponding to the iface.            */
  PS_IFACE_IOCTL_GET_GATEWAY_ADDR         = 125,/**< Get gateway ip address.*/
  PS_IFACE_IOCTL_GET_SUBNET_MASK          = 126,/**< Get IPv4 subnet mask.  */
  PS_IFACE_IOCTL_GET_IPV6_REMAINING_ROUTER_LIFETIME = 127,
                                               /* Get IPv6 Router lifetime
                                                 of public address on iface */
  PS_IFACE_IOCTL_SET_HANDOFF_CTXT         = 128,
  /**< To update Handoff Ctxt on a given iface */

  PS_IFACE_IOCTL_IS_IPSEC_SA_SHARED       = 129,/**< Query whether IPSEC
                                                     SA is shared between
                                                     v4 and v6 ifaces */
  PS_IFACE_IOCTL_GET_IPSEC_SA             = 130,/**< Query the IPSEC SA params   */
  PS_IFACE_IOCTL_PROCESS_HO_INIT          = 131,
  PS_IFACE_IOCTL_IPFLTR_CHANGED           = 132, 
  /**< IPFLTRs are added or deleted */
  PS_IFACE_IOCTL_KEEP_ALIVE                    = 137, /* Keep alive ioctl */
  PS_IFACE_IOCTL_FILTER_MATCH_REQUEST          = 138, /* Filter match ioctl */


  /*-------------------------------------------------------------------------
    IOCTLs which are interface specific should be enumerated below this.
    Note that the enuerated values from 0 - 199 are reserved for the common
    IOCTLs. All interface specific IOCTLs should use values greater than 199.
    It is recommended that each interface have a certain range of values
    reserved. This would ensure that all the interface specific operations
    are grouped together. For instance, CDMA2000 could use value b/w 200 and
    299, WCDMA 300 to 399 etc.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------

                                707 IOCTLS

    The following operations are specific to 707 (1x) interface. Start 200.
  -------------------------------------------------------------------------*/
  PS_IFACE_IOCTL_707_GET_MDR              = 200,/* 1x QCMDR value          */
  PS_IFACE_IOCTL_707_SET_MDR              = 201,/* 1x QCMDR value          */
  PS_IFACE_IOCTL_707_GET_DORM_TIMER       = 202,/* 1x dorm timer value     */
  PS_IFACE_IOCTL_707_SET_DORM_TIMER       = 203,/* 1x dorm timer value     */
  PS_IFACE_IOCTL_707_GET_RLP_ALL_CURR_NAK = 204,/* 1x RLP curr NAK policy  */
  PS_IFACE_IOCTL_707_SET_RLP_ALL_CURR_NAK = 205,/* 1x RLP curr NAK policy  */
  PS_IFACE_IOCTL_707_GET_RLP_ALL_DEF_NAK  = 206,/* 1x RLP  default NAK     */
  PS_IFACE_IOCTL_707_SET_RLP_ALL_DEF_NAK  = 207,/* 1x RLP  default NAK     */
  PS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK  = 208,/* 1x RLP  negotiated NAK  */
  PS_IFACE_IOCTL_707_GET_RLP_QOS_NA_PRI   = 210,/* 1x QOS nonassured prio  */
  PS_IFACE_IOCTL_707_SET_RLP_QOS_NA_PRI   = 211,/* 1x QOS nonassured prio  */
  PS_IFACE_IOCTL_707_SDB_SUPPORT_QUERY    = 212,/* 1x SDB possible query   */
  PS_IFACE_IOCTL_707_ENABLE_HOLDDOWN      = 215,/* 1x enable/disable
                                                   holddown timer          */
  PS_IFACE_IOCTL_707_ENABLE_HDR_HPT_MODE  = 216, /* Set HDR Hybrid mode
                                                   specific/normal */
  PS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA = 217, /* Set HDR REV0 rate
                                                            inertia params  */
  PS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INTERIA =   /* Alias fixing typo */
    PS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA,
  PS_IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES = 218, /* Retrieve all
                                                    supported QOS profiles */

  PS_IFACE_IOCTL_707_ENABLE_HDR_SLOTTED_MODE = 219, /* Set slot cycle
                                                       index params        */

  PS_IFACE_IOCTL_GET_SESSION_TIMER        = 220, /* Get Session Timer value */
  PS_IFACE_IOCTL_SET_SESSION_TIMER        = 221, /* Set Session Timer value */

  PS_IFACE_IOCTL_707_GET_HDR_1X_HANDDOWN_OPTION = 222, /* Is HDR-1x handdown
                                                          enabled          */
  PS_IFACE_IOCTL_707_SET_HDR_1X_HANDDOWN_OPTION = 223, /* Enable HDR-1x
                                                          handdown         */

  PS_IFACE_IOCTL_SET_MIP6_BOOTSTRAP_PARAMS      = 224, /* Set MIPv6
                                                          bootstrap info   */

  /* Get Hysteresis Activation timer value */
  PS_IFACE_IOCTL_707_GET_HYSTERESIS_ACT_TIMER = 225,
  /* Set Hysteresis Activation timer value */
  PS_IFACE_IOCTL_707_SET_HYSTERESIS_ACT_TIMER = 226,

  /*-------------------------------------------------------------------------
    Get current data call type (SIP, MIP).
    Deprecated ioctl name, please use below alias
    PS_IFACE_IOCTL_707_GET_SIP_MIP_DATA_CALL_TYPE
  -------------------------------------------------------------------------*/
  PS_IFACE_IOCTL_707_GET_DATA_CALL_TYPE = 227,
  PS_IFACE_IOCTL_707_GET_SIP_MIP_DATA_CALL_TYPE =
    PS_IFACE_IOCTL_707_GET_DATA_CALL_TYPE,

  /* Get data packet count (bytes tx/rx) */
  PS_IFACE_IOCTL_707_GET_DATA_COUNT = 228,
  /* Set HDR AN User ID */
  PS_IFACE_IOCTL_707_SET_HDR_AN_USER_ID = 229,

  PS_IFACE_IOCTL_707_GET_MIP_MA_INFO = 230, /* Get FA address */

  /*-------------------------------------------------------------------------

                              UMTS IOCTLS

    The following operations are specific to UMTS interface.Start from 300.
  -------------------------------------------------------------------------*/
  PS_IFACE_IOCTL_UMTS_GET_IM_CN_FLAG      =301, /* Get IM_CN flag          */
  PS_IFACE_IOCTL_MBMS_MCAST_CONTEXT_ACTIVATE = 302, /* MBMS context activate*/
  PS_IFACE_IOCTL_MBMS_MCAST_CONTEXT_DEACTIVATE = 303,
                                                 /*MBMS DEACTIVATE CONTEXT */
  PS_IFACE_IOCTL_UMTS_GET_SESSION_PARAMS = 304, /* Get session params */
  PS_IFACE_IOCTL_UMTS_GET_PDP_CONTEXT_ID = 305, /* Get context id */
  PS_IFACE_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE = 306, /* Get calltype */
  PS_IFACE_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE = 307, /* Set calltype */
  PS_IFACE_IOCTL_3GPP_FORCE_PDN_DISCONNECT = 308, /* Force PDN disconnect */

  /*-------------------------------------------------------------------------

                              BCMCS IOCTLS

    The following operations are specific to BCMCS interface.Start from 400.
  -------------------------------------------------------------------------*/
  PS_IFACE_IOCTL_BCMCS_DB_UPDATE          = 401,/* Update BCMCS db table   */
  PS_IFACE_IOCTL_BCMCS_ENABLE_HANDOFF_REG = 402,/* BCMCS handoff optimization
                                                   for regiatration        */
  PS_IFACE_IOCTL_BCMCS_BOM_CACHING_SETUP  = 403,/* BCMCS enable/disable BOM
                                                   caching                 */

  /*-------------------------------------------------------------------------

                              UW FMC IOCTLS

    The following operations are specific to UW FMC interface. Start from 500
  -------------------------------------------------------------------------*/
  PS_IFACE_IOCTL_UW_FMC_SET_TUNNEL_PARAMS   = 501,
  PS_IFACE_IOCTL_UW_FMC_RESET_TUNNEL_PARAMS = 502,

  /*-------------------------------------------------------------------------

                              NAT related IOCTLs.

  -------------------------------------------------------------------------*/

  PS_IFACE_IOCTL_GET_STATIC_NAT_ENTRY     = 550,/* Get Static NAT Entry    */
  PS_IFACE_IOCTL_ADD_STATIC_NAT_ENTRY     = 551,/* Add Static NAT Entry    */
  PS_IFACE_IOCTL_GET_DYNAMIC_NAT_ENTRY_TIMEOUT = 552, /* Get Dynamic NAT
                                                             Entry Timeout */
  PS_IFACE_IOCTL_SET_DYNAMIC_NAT_ENTRY_TIMEOUT = 553, /* Set Dynamic NAT
                                                             Entry Timeout */
  PS_IFACE_IOCTL_DELETE_STATIC_NAT_ENTRY  = 555,/* Delete Static NAT Entry */
  PS_IFACE_IOCTL_GET_NAT_IPSEC_VPN_PASS_THROUGH = 556, /* Get NAT IPSEC VPN
                                                               PassThrough */
  PS_IFACE_IOCTL_SET_NAT_IPSEC_VPN_PASS_THROUGH = 557, /* Set NAT IPSEC VPN
                                                               PassThrough */
  PS_IFACE_IOCTL_ADD_DMZ = 558, /* Add DMZ */
  PS_IFACE_IOCTL_GET_DMZ = 559, /* Get DMZ */
  PS_IFACE_IOCTL_DELETE_DMZ = 560, /* Delete DMZ */
  PS_IFACE_IOCTL_GET_NAT_PUBLIC_IP_ADDR = 561, /* Get NAT public IP addr   */
  PS_IFACE_IOCTL_GET_NAT_L2TP_VPN_PASS_THROUGH = 562, /* Get NAT L2TP VPN
                                                               PassThrough */
  PS_IFACE_IOCTL_SET_NAT_L2TP_VPN_PASS_THROUGH = 563, /* Set NAT L2TP VPN
                                                               PassThrough */
  PS_IFACE_IOCTL_GET_NAT_PPTP_VPN_PASS_THROUGH = 564, /* Get NAT PPTP VPN
                                                               PassThrough */
  PS_IFACE_IOCTL_SET_NAT_PPTP_VPN_PASS_THROUGH = 565, /* Set NAT PPTP VPN
                                                               PassThrough */
  PS_IFACE_IOCTL_GET_DHCP_ADDR_CONFIG = 566,   /* Get DHCP config from NAT
                                                  */
  PS_IFACE_IOCTL_ENABLE_ALG = 567,                 /* Enable an ALG client */
  PS_IFACE_IOCTL_DISABLE_ALG = 568,               /* Disable an ALG client */
  PS_IFACE_IOCTL_GET_ALG_STATUS = 569,       /* Retrieve the status(enabled/
                                                    disabled) for all ALGs */
  PS_IFACE_IOCTL_ENABLE_STA_MODE = 570,          /* Enable STA mode at NAT */
  PS_IFACE_IOCTL_DISABLE_STA_MODE = 571,        /* Disable STA mode at NAT */
  PS_IFACE_IOCTL_SET_NAT_TYPE = 572,            /* Set NAT type */
  PS_IFACE_IOCTL_GET_NAT_TYPE = 573,            /* Get NAT type */

  /*-------------------------------------------------------------------------

                          RM Specific IOCTLs

    These ioctls are applicable to both RmSm and RmNet interfaces
  -------------------------------------------------------------------------*/
  PS_IFACE_IOCTL_RM_GET_SIO_PORT = 601,     /* return associated port ID   */
  PS_IFACE_IOCTL_RM_GET_STREAM_ID = 602,     /* return associated Stream Id*/

  /*-------------------------------------------------------------------------

                           RMNET Specific IOCTLs

  -------------------------------------------------------------------------*/
  PS_IFACE_IOCTL_RMNET_GET_LINK_PROTO_TYPE = 702,   /*return link protocol */
  PS_IFACE_IOCTL_RMNET_GET_IPV4_DEST_MAC_ADDR = 703, /* return destination
                                                     mac addr for ipv4 call*/
  PS_IFACE_IOCTL_RMNET_GET_IPV6_DEST_MAC_ADDR = 704, /* return destination
                                                     mac addr for ipv6 call*/
  PS_IFACE_IOCTL_RMNET_IS_TETHERED_CALL = 705, /* NAT uses this to determine
                                                 if RmNet is in laptop call*/
  PS_IFACE_IOCTL_RMNET_SET_SOFTAP_MODE = 706, /* Set RmNet instance in SoftAP
                                                                       mode*/
  PS_IFACE_IOCTL_RMNET_GET_WATERMARKS = 707, /* return watermarks */

  /*-------------------------------------------------------------------------

                            RMSM Specific IOCTLs

  -------------------------------------------------------------------------*/
  PS_IFACE_IOCTL_RMSM_PPP_BUILD_HEADER = 801, /* return ppp header */
  PS_IFACE_IOCTL_RMSM_GET_VJ_RESULT = 802, /* return vj negotiation result*/

  /*-------------------------------------------------------------------------
                            EMBMS Specific IOCTLs
  -------------------------------------------------------------------------*/
  PS_IFACE_IOCTL_EMBMS_ACTIVATE_TMGI = 901,     /**< Activate TMGI request */
  PS_IFACE_IOCTL_EMBMS_DEACTIVATE_TMGI = 902,   /**< Deactivate TMGI req   */
  PS_IFACE_IOCTL_EMBMS_GET_ACTIVE_TMGI_LIST = 903,
                                                /**< Active TMGI list query*/
  PS_IFACE_IOCTL_EMBMS_GET_AVAIL_TMGI_LIST = 904,
                                                /**< Avail TMGI list query */
  PS_IFACE_IOCTL_EMBMS_ACT_DEACT_TMGI = 905,    /**< Activate/deactivate 
                                                              TMGI request */
  PS_IFACE_IOCTL_EMBMS_GET_SAI_LIST = 906 ,   /**< SAI list query */
  PS_IFACE_IOCTL_EMBMS_GENERATE_EVENTS  = 907,          /**< Send eMBMS events */
  PS_IFACE_IOCTL_EMBMS_CONTENT_DESC_UPDATE = 908, /**< Content desc update*/

  /*-------------------------------------------------------------------------
                            WLAN/EPDG Specific IOCTLs
  -------------------------------------------------------------------------*/
  PS_IFACE_IOCTL_REVERSE_IP_TRANSPORT_CONFIG_STATUS  = 1000,     
  /**< Application Processor has finished configuring Reverse IP Transport */
  PS_IFACE_IOCTL_INITIATE_ESP_REKEY  = 1001,     
  /**< Application Processor wants to initiate ESP rekey */
  PS_IFACE_IOCTL_REVERSE_IP_TRANSPORT_GET_IPFLTRS    = 1002, 
  /**< Query the existing IPFLTRs */
  PS_IFACE_IOCTL_WLAN_DELETE_MODEM_PORT_RANGE_FILTER    = 1003, 
  /**< Delete modem port range filters */  

  /*------------------------------------------------------------------------
                             RESERVED IOCTLs
    The following IOCTLs are reserved and should NOT be used by external
    applications.
  -------------------------------------------------------------------------*/
  PS_IFACE_IOCTL_RESERVED_MIN             = 1100,/* Min reserved value     */

  /*-------------------------------------------------------------------------
                             707 RESERVED IOCTLs
  -------------------------------------------------------------------------*/
  PS_IFACE_IOCTL_707_GET_RLP_FILL_RATE    = 1200,/* 1x RLP fill rate - get  */
  PS_IFACE_IOCTL_707_SET_RLP_FILL_RATE    = 1201,/* 1x RLP fill rate - set  */

  PS_IFACE_IOCTL_RESERVED_MAX             = 1499, /* Max reserved value    */

  /*-------------------------------------------------------------------------

                              MORE COMMON STUFF

  -------------------------------------------------------------------------*/
  PS_IFACE_IOCTL_MAX,
  PS_IFACE_IOCTL_NAME_FORCE_32_BIT        = 0x7FFFFFFF /* Force 32bit enum */
} ps_iface_ioctl_type;

/*---------------------------------------------------------------------------
  The QOS_REQUEST_EX opcode type is a mask to allow for future features.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IFACE_IOCTL_QOS_REQUEST_OP    = 0x00000001,
  PS_IFACE_IOCTL_QOS_CONFIGURE_OP  = 0x00000002
} ps_iface_ioctl_qos_request_ex_opcode_enum_type;

/*---------------------------------------------------------------------------
  QoS control mode
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IFACE_IOCTL_QOS_MODE_UE_ONLY      = 0,
  PS_IFACE_IOCTL_QOS_MODE_UE_AND_NW    = 1,
  PS_IFACE_IOCTL_QOS_MODE_NW_ONLY      = 2
} ps_iface_ioctl_qos_mode_enum_type;

/*---------------------------------------------------------------------------
  The MCAST_JOIN_EX request flags.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IFACE_IOCTL_MCAST_REG_SETUP_ALLOWED = 0x00000000,
  PS_IFACE_IOCTL_MCAST_REG_SETUP_NOT_ALLOWED = 0x00000001
} ps_iface_ioctl_mcast_join_ex_req_flags_enum_type;

/*---------------------------------------------------------------------------
 Enable/Disable BCMCS strict BOM caching
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IFACE_IOCTL_BCMCS_ENABLE_STRICT_BOM_CACHING  = 0,
  PS_IFACE_IOCTL_BCMCS_DISABLE_STRICT_BOM_CACHING = 1
} ps_iface_ioctl_bcmcs_bom_caching_setup_enum_type;

/*---------------------------------------------------------------------------
  Enum to be used with PS_IFACE_IOCTL_707_GET_DATA_CALL_TYPE
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IFACE_IOCTL_DATA_CALL_TYPE_NONE   = 0x00000000,
  PS_IFACE_IOCTL_DATA_CALL_TYPE_SIP    = 0x00000001,
  PS_IFACE_IOCTL_DATA_CALL_TYPE_MIP    = 0x00000002
} ps_iface_ioctl_data_call_enum_type;

/*---------------------------------------------------------------------------
  Enum to be used with PS_IFACE_IOCTL_707_GET_SIP_MIP_DATA_CALL_TYPE
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_data_call_enum_type
  ps_iface_ioctl_sip_mip_data_call_enum_type;

/*---------------------------------------------------------------------------
  Data type to be used with PS_IFACE_IOCTL_707_ENABLE_HDR_SLOTTED_MODE
---------------------------------------------------------------------------*/
typedef struct
{
  boolean    enable;
  uint32     slotted_mode_option;           /* slot cycle value */
  uint32     get_slotted_mode;              /* indicate intent to get SM value */
} ps_iface_ioctl_enable_hdr_slotted_mode_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES
---------------------------------------------------------------------------*/
#define MAX_NETWORK_SUPPORTED_PROFILE_COUNT 255
typedef struct
{
  uint8  profile_count;
  uint16 profile_value[MAX_NETWORK_SUPPORTED_PROFILE_COUNT];
} ps_iface_ioctl_get_network_supported_qos_profiles_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_BCMCS_DB_UPDATE

  Fields are
    overwrite    : Overwrite an entry if current one is found.

    Please see the BCMCS documentation for the remaining descriptions
    of the flow specific parameters.
---------------------------------------------------------------------------*/
typedef struct
{
  dsbcmcs_zone_type zone;
  ip_addr_type      multicast_ip;
  uint32            program_id;
  uint8             program_id_len;
  uint8             flow_discrim_len;
  uint8             flow_discrim;
  uint16            port;
  dsbcmcs_framing_enum_type  framing;
  dsbcmcs_protocol_enum_type protocol;
  uint8   crc_len;
  uint8   flow_format;
  uint32  flow_id;
  uint8   flow_id_len;
  boolean overwrite;
  uint8   flow_priority;
} ps_iface_ioctl_bcmcs_db_update_type;

/*---------------------------------------------------------------------------
  Iface HW Address Type for PS_IFACE_IOCTL_GET_HW_ADDR
---------------------------------------------------------------------------*/
typedef struct
{
  uint8    hw_addr_len;          /* size of the buffer provided */
  uint8 *  hw_addr;              /* ptr to buffer to store HW addr */
} ps_iface_ioctl_hw_addr_type;


/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_BCMCS_BOM_CACHING_SETUP
  Fields are:
    bom_caching_setup        :    BOM caching setup
    bom_cache_timeout        :    BOM cache period timeout
---------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_ioctl_bcmcs_bom_caching_setup_enum_type  bom_caching_setup;
  uint8                                             bom_cache_timeout;
} ps_iface_ioctl_bcmcs_bom_caching_setup_type;

typedef enum

{
  PS_IFACE_IOCTL_MBMS_INVALID_SERVICE_TYPE= -1,
    /* INVALID SERVICE TYPE */

  PS_IFACE_IOCTL_MBMS_STREAMING_SERVICE_TYPE,
    /* Streaming Service Type */

  PS_IFACE_IOCTL_MBMS_DOWNLOAD_SERVICE_TYPE,
  /* Download Service Type */

  PS_IFACE_IOCTL_MBMS_MAX_SERVICE_TYPE
    /*Maximum Service Type */

}ps_iface_ioctl_mbms_service_type;



typedef enum
{
  PS_IFACE_IOCTL_MBMS_INVALID_SERVICE_METHOD= -1,
    /* INVALID SERVICE METHOD */

  PS_IFACE_IOCTL_MBMS_BROADCAST_SERVICE_METHOD,
    /* Broadcast Service Method */

  PS_IFACE_IOCTL_MBMS_MULTICAST_SERVICE_METHOD,
    /* Multicast Service Method */

  PS_IFACE_IOCTL_MBMS_MAX_SERVICE_METHOD
    /*Maximum Service Method */
}ps_iface_ioctl_mbms_service_method;


typedef struct
{
   uint64                            tmgi;
   uint64                            session_start_time;
   uint64                            session_end_time;
   uint16                                     priority;
   ps_iface_ioctl_mbms_service_method         service_method;
   ps_iface_ioctl_mbms_service_type           service_type;
   boolean                                    selected_service;
   boolean                                    service_security;
}ps_iface_ioctl_mbms_mcast_param_type;


/*---------------------------------------------------------------------------
  Argument to be passed with MBMS context activation/deactivation
---------------------------------------------------------------------------*/
typedef struct
{

  ip_addr_type               ip_addr;          /* Multicast IP address     */
  uint32                      profile_id;      /* UMTS unicast profile ID */
  sint15                     nethandle;        /* dss_nethandle            */

  /* Output Paramters */
  uint32                     handle;      /* MBMS handle              */
} ps_iface_ioctl_mbms_mcast_context_act_type;

typedef struct
{
  sint15                      nethandle;
  uint32                      handle;      /* MBMS handle              */
} ps_iface_ioctl_mbms_mcast_context_deact_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_UMTS_GET_SESSION_PARAMS
---------------------------------------------------------------------------*/
typedef struct{
  /* profile number for the profile used in the call */
  uint32                       profile_number;
  /* Profile name in ascii string format */
  byte                         profile_name[DS_UMTS_MAX_PROFILE_NAME_LEN+1];
  /* Authentication info */
  ds_umts_pdp_auth_type        auth;
  /* APN string  */
  byte                         apn[DS_UMTS_MAX_APN_STRING_LEN+1];
  /* PDP type (IP/PPP)  */
  ds_umts_pdp_type_enum_type   pdp_type;
  /* Network alloc GPRS QoS params */
  ds_umts_gprs_qos_params_type gprs_qos;
  /* Network alloc UMTS QoS params */
  ds_umts_umts_qos_params_type umts_qos;
  /* Network alloc LTE QoS params */
  ds_3gpp_lte_qos_params_type  lte_qos;
  boolean                      request_pcscf_address_flag;
  ds_3gpp_lte_qos_params_ex_type  lte_qos_ex;
} ps_iface_ioctl_3gpp_session_params_info_type;

/*---------------------------------------------------------------------------
  Argument to be passed with the ioctls
  PS_IFACE_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE
  PS_IFACE_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE
  The IOCTLs are valid only when iface is up.
---------------------------------------------------------------------------*/

typedef enum
{
  PS_IFACE_3GPP_LTE_REESTAB_CALLTYPE_NORMAL,
  PS_IFACE_3GPP_LTE_REESTAB_CALLTYPE_VOLTE,
  PS_IFACE_3GPP_LTE_REESTAB_CALLTYPE_MAX
} ps_iface_ioctl_3gpp_lte_reestab_calltype_enum_type;

/*---------------------------------------------------------------------------
  Argument for PS_IFACE_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE
---------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_ioctl_3gpp_lte_reestab_calltype_enum_type calltype;
} ps_iface_ioctl_3gpp_get_lte_reestab_calltype_type;

/*---------------------------------------------------------------------------
  Argument for PS_IFACE_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE
---------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_ioctl_3gpp_lte_reestab_calltype_enum_type calltype;
} ps_iface_ioctl_3gpp_set_lte_reestab_calltype_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_QOS_GET_MODE
  Fields are:
    qos_mode        :    QOS control mode
---------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_ioctl_qos_mode_enum_type     qos_mode;
} ps_iface_ioctl_qos_mode_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_ON_QOS_AWARE_SYSTEM
  to get the current bearer technology.
---------------------------------------------------------------------------*/
typedef boolean ps_iface_ioctl_on_qos_aware_system_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY
  to get the current bearer technology.
---------------------------------------------------------------------------*/

typedef ps_iface_bearer_technology_type  ps_iface_ioctl_bearer_tech_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_GET_DATA_BEARER_RATE
  to get the bearer data rate information.
---------------------------------------------------------------------------*/
typedef ps_iface_data_bearer_rate ps_iface_ioctl_data_bearer_rate;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_707_[GS]ET_HDR_1X_HANDDOWN_OPTION
---------------------------------------------------------------------------*/
typedef boolean ps_iface_ioctl_hdr_1x_handdown_option_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_MT_DEREG_CB
---------------------------------------------------------------------------*/
typedef struct ps_iface_ioctl_mt_dereg_cb_s
{
  sint15                   app_id;           /* Appid for application      */
  ps_iface_mt_handle_type  handle;           /* MT handle                  */
} ps_iface_ioctl_mt_dereg_cb_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_QOS_REQUEST_EX

  Fields are
     opcode            : Operation to be performed on the QOS specifications.
     num_qos_specs     : The number of qos specs passed.
    *qos_specs_ptr     : Ptr to QOS specifications
     qos_control_flags : Flags specifiying an additional qos options.
                         Not currently supported.
    *flows_ptr         : OUT param - Ptr to flows providing the QOS
     subset_id         : Subset ID associated with filters in qos
---------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_ioctl_qos_request_ex_opcode_enum_type   opcode;
  uint8                                            num_qos_specs;
  qos_spec_type                                  * qos_specs_ptr;
  uint8                                            qos_control_flags;
  ps_flow_type                                  ** flows_ptr;
  ps_iface_ipfltr_subset_id_type                   subset_id;
} ps_iface_ioctl_qos_request_ex_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_MT_REG_CB
---------------------------------------------------------------------------*/
typedef struct
{
  void*                    acl_pol_ptr;
  /* Out param */
  ps_iface_mt_handle_type  *handle;          /* MT handle for the app      */
} ps_iface_ioctl_mt_reg_cb_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_MCAST_JOIN
---------------------------------------------------------------------------*/
typedef struct
{
  void                        *mcast_param_ptr; /* Multicast specific data */
  ip_addr_type                ip_addr;          /* Multicast addr to join  */
  uint16                      port;             /* Multicast port to join  */
  sint15                      nethandle;        /* Client handle           */
  /* Out param */
  ps_iface_mcast_handle_type  handle;           /* Multicast handle        */
} ps_iface_ioctl_mcast_join_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_MCAST_LEAVE
---------------------------------------------------------------------------*/
typedef struct
{
  sint15                            nethandle; /* Client handle            */
  ps_iface_mcast_handle_type        handle;  /* Multicast handle           */
} ps_iface_ioctl_mcast_leave_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_MCAST_JOIN_EX

  Fields are:
   For each flow::
     mcast_param_ptr    :       Multicast specific data
     ip_addr            :       Multicast address to join
     port               :       Multicast port to join
     handle             :       Multicase handle
     mcast_request_flags:       Configure / Register request
   num_flows            :       Number of requested flows
---------------------------------------------------------------------------*/
typedef struct
{
  void                        *mcast_param_ptr[PS_IFACE_MAX_MCAST_FLOWS];
  ip_addr_type                ip_addr[PS_IFACE_MAX_MCAST_FLOWS];
  uint16                      port[PS_IFACE_MAX_MCAST_FLOWS];
  ps_iface_mcast_handle_type  handle[PS_IFACE_MAX_MCAST_FLOWS];
  uint32                      num_flows;
  ps_iface_ioctl_mcast_join_ex_req_flags_enum_type
                              mcast_request_flags[PS_IFACE_MAX_MCAST_FLOWS];
} ps_iface_ioctl_mcast_join_ex_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_MCAST_LEAVE_EX

  Fields are:
   handle               :       Multicast handle for each flow
   num_flows            :       Number of requested flows
---------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_mcast_handle_type  handle[PS_IFACE_MAX_MCAST_FLOWS];
  uint8                       num_flows;
} ps_iface_ioctl_mcast_leave_ex_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_MCAST_REGISTER_EX

  Fields are:
   handle               :       Multicast handle for each flow
   num_flows            :       Number of requested flows
---------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_mcast_handle_type  handle[PS_IFACE_MAX_MCAST_FLOWS];
  uint8                       num_flows;
} ps_iface_ioctl_mcast_register_ex_type;

/*---------------------------------------------------------------------------
  Structure to store Mulicast IP address and port
    ip_addr                 :     Multicast IP address
    port                    :     Multicast port
---------------------------------------------------------------------------*/
typedef struct
{
  ip_addr_type               ip_addr;
  uint16                     port;
} ps_iface_ioctl_mcast_addr_info_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_BCMCS_ENABLE_HANDOFF_REG
  Fields are:
   handle               :       Multicast handle for each flow
   num_flows            :       Number of multicast handles
---------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_ioctl_mcast_addr_info_type mcast_addr_info[PS_IFACE_MAX_MCAST_FLOWS];
  uint8                               num_mcast_addr;
} ps_iface_ioctl_bcmcs_enable_handoff_reg_type;


/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_QOS_REQUEST

  Fields are
    qos_ptr   : Ptr to QOS specification
    flow_ptr  : OUT param - Flow providing the QOS
    subset_id : Subset ID associated with filters in qos
---------------------------------------------------------------------------*/
typedef struct
{
  qos_spec_type                   * qos_ptr;
  ps_flow_type                    * flow_ptr;
  ps_iface_ipfltr_subset_id_type    subset_id;
} ps_iface_ioctl_qos_request_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_GET_FIREWALL_TABLE

  Fields are
    ip_filter_type  : Fltr spec
    num_fltrs       : Number of filters to be retrived
    avail_num_fltrs : (OUT PARAM) Actual number of filters present
    handle          : (OUT PARAM) Handles to the filters
---------------------------------------------------------------------------*/
typedef struct
{
  ip_filter_type                   * fltr_spec_arr;
  uint32                           * handle_arr;
  uint8                              num_fltrs;
  uint8                              avail_num_fltrs;
} ps_iface_ioctl_get_firewall_table_type;

/**
  @brief Data type for specifying the parameters for
  PS_IFACE_IOCTL_GET_FIREWALL_SETTINGS

  @see PS_IFACE_IOCTL_GET_FIREWALL_SETTINGS
*/
typedef struct
{
  boolean   firewall_enabled;
  /**< Indicates if firewall is enabled */
  boolean   pkts_allowed;
  /**< Indicates if packets are allowed to go through firewall */
} ps_iface_ioctl_firewall_settings_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_UW_FMC_SET_TUNNEL_PARAMS
---------------------------------------------------------------------------*/
typedef struct
{
  int32                stream_id;
  struct ps_sockaddr * tunnel_end_point_info_ptr;
  uint16               addr_len;
  boolean              is_nat_present;
} ps_iface_ioctl_uw_fmc_tunnel_params_type;

/*---------------------------------------------------------------------------
  NAT Static Entry
---------------------------------------------------------------------------*/
typedef struct ps_iface_ioctl_static_nat_entry
{
  /* Client handle */
  int16                         nethandle;
  /* Private IP address in network order */
  struct ps_in_addr             private_ip_addr;
  /* Public Port in network order */
  uint16                        global_port;
  /* Private Port in network order */
  uint16                        private_port;
  /* Protocol TCP/UDP/ICMP */
  uint8                         protocol;
} ps_iface_ioctl_static_nat_entry_type;


/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_ADD_STATIC_NAT_ENTRY
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_static_nat_entry_type
  ps_iface_ioctl_add_static_nat_entry_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_DELETE_STATIC_NAT_ENTRY
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_static_nat_entry_type
  ps_iface_ioctl_delete_static_nat_entry_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_GET_STATIC_NAT_ENTRY
  Fields are:
   nethandle            :       Client handle
   entries              :       Static NAT entries
   num_entries          :       Number of return static NAT entries
   total_entries        :       Total Number of static NAT entries
---------------------------------------------------------------------------*/
typedef struct
{
  int16                                     nethandle;
  ps_iface_ioctl_static_nat_entry_type *    entries_arr;
  uint8                                     num_entries;
  uint8                                     total_entries;
} ps_iface_ioctl_get_static_nat_entry_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_GET_DYNAMIC_NAT_ENTRY_TIMEOUT
  Fields are:
   nethandle            :       Client handle
   timeout              :       Dynamic NAT entry timeout in sec
---------------------------------------------------------------------------*/
typedef struct
{
  int16                        nethandle;
  uint16                       timeout;
} ps_iface_ioctl_get_dynamic_nat_entry_timeout_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_SET_DYNAMIC_NAT_ENTRY_TIMEOUT
  Fields are:
   timeout              :       Dynamic NAT entry timeout in sec
   nethandle            :       Client handle
---------------------------------------------------------------------------*/
typedef struct
{
  int16                        nethandle;
  uint16                       timeout;
} ps_iface_ioctl_set_dynamic_nat_entry_timeout_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_SET_NAT_IPSEC_VPN_PASS_THROUGH
                             PS_IFACE_IOCTL_GET_NAT_IPSEC_VPN_PASS_THROUGH
   nethandle            :     Client handle
   is_vpn_passthrough   :     VPN pass through enabled/disabled
---------------------------------------------------------------------------*/
typedef struct
{
  int16                        nethandle;
  boolean                      is_vpn_passthrough;
} ps_iface_ioctl_nat_ipsec_vpn_pass_through_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_SET_NAT_L2TP_VPN_PASS_THROUGH
                             PS_IFACE_IOCTL_GET_NAT_L2TP_VPN_PASS_THROUGH
   nethandle                 :     Client handle
   is_l2tp_vpn_passthrough   :     L2TP VPN pass through enabled/disabled
---------------------------------------------------------------------------*/
typedef struct
{
  int16                        nethandle;
  boolean                      is_l2tp_vpn_passthrough;
} ps_iface_ioctl_nat_l2tp_vpn_pass_through_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_SET_NAT_PPTP_VPN_PASS_THROUGH
                             PS_IFACE_IOCTL_GET_NAT_PPTP_VPN_PASS_THROUGH
   nethandle                 :     Client handle
   is_pptp_vpn_passthrough   :     PPTP VPN pass through enabled/disabled
---------------------------------------------------------------------------*/
typedef struct
{
  int16                        nethandle;
  boolean                      is_pptp_vpn_passthrough;
} ps_iface_ioctl_nat_pptp_vpn_pass_through_type;

/*---------------------------------------------------------------------------
  Argument to be passed with
   PS_IFACE_IOCTL_ADD_DMZ
   PS_IFACE_IOCTL_GET_DMZ
   nethandle         :   Client handle
   dmz_ip_addr       :   DMZ private IP addr
---------------------------------------------------------------------------*/
typedef struct
{
  int16                         nethandle;
  ip_addr_type                  dmz_ip_addr;
} ps_iface_ioctl_dmz_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_DELETE_DMZ
   nethandle         :   Client handle
---------------------------------------------------------------------------*/
typedef struct
{
  int16                         nethandle;
} ps_iface_ioctl_delete_dmz_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_GET_DHCP_ADDRESS_CONFIG
   subnet_mask       :   IPv4 subnet mask
   client_ip_addr    :   IPv4 address to be assigned to DHCP client
   gw_ip_addr        :   IPv4 address for the gateway/server
   gw_iface_ptr      :   Rm iface on which server was started
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                  subnet_mask;
  uint32                  client_ip_addr;
  uint32                  gw_ip_addr;
  ps_iface_type          *gw_iface_ptr;
} ps_iface_ioctl_dhcp_addr_config_type;

/*---------------------------------------------------------------------------
  Argument to be passed along with
   PS_IFACE_IOCTL_ENABLE_ALG
   PS_IFACE_IOCTL_DISABLE_ALG
   client_name : ALG client which is to be enabled or disabled.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_ALG_CLIENT_NONE, /* Used for cumulative status check - if None of
                         the ALG clients are enabled. */
  PS_FTP_ALG_CLIENT,
  PS_PPTP_ALG_CLIENT,
  PS_RTSP_ALG_CLIENT,
  PS_SIP_ALG_CLIENT,
  PS_TFTP_ALG_CLIENT,
  PS_ALG_ALL_CLIENTS, /* Used for cumulative enable/disable or status
                         check. */
  PS_ALG_CLIENT_MAX
} ps_iface_ioctl_alg_client_e;
typedef struct
{
  ps_iface_ioctl_alg_client_e  client_name;
} ps_iface_ioctl_enable_alg_type;
typedef struct
{
  ps_iface_ioctl_alg_client_e  client_name;
} ps_iface_ioctl_disable_alg_type;
/*---------------------------------------------------------------------------
  Argument to be passed along with
   PS_IFACE_IOCTL_GET_ALG_STATUS
   is_enabled : Array indicating whether each ALG is enabled or disabled.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean is_enabled[PS_ALG_CLIENT_MAX];
}ps_iface_ioctl_get_alg_status_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_707_GET_DATA_COUNT
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  rxed_bytes;
  uint32  txed_bytes;
} ps_iface_ioctl_data_count_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_707_SET_HDR_AN_USER_ID
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 user_id[PPP_MAX_USER_ID_LEN];
  uint8 user_id_len;
} ps_iface_ioctl_hdr_an_user_id_type;

/*---------------------------------------------------------------------------
  PS_IFACE_IOCTL_DHCP_ARP_CACHE structure
---------------------------------------------------------------------------*/
#define PS_IFACE_DHCP_ARP_CACHE_MAX_HW_ADDR_LEN 16
typedef struct ps_iface_ioctl_dhcp_arp_cache_s
{
  uint32         ip_addr;
  byte           hw_addr[PS_IFACE_DHCP_ARP_CACHE_MAX_HW_ADDR_LEN];
  int32          hw_addr_len;
} ps_iface_ioctl_dhcp_arp_cache_type;


/*---------------------------------------------------------------------------
  PS_IFACE_IOCTL_ENABLE_HDR_SLOTTED_MODE structure
---------------------------------------------------------------------------*/
typedef struct
{
  boolean enable;
  uint32 slotted_mode_option;           /* slot cycle value */
  uint32 get_slotted_mode;              /* indicate intent to get SM value */
} ps_iface_ioctl_enable_hdr_slotted_mode;

/*---------------------------------------------------------------------------
  PS_IFACE_IOCTL_DHCP_ARP_CACHE_UPDATE parameter type
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_dhcp_arp_cache_type
  ps_iface_ioctl_dhcp_arp_cache_update_type;

/*---------------------------------------------------------------------------
  PS_IFACE_IOCTL_DHCP_ARP_CACHE_CLEAR parameter type
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_dhcp_arp_cache_type
  ps_iface_ioctl_dhcp_arp_cache_clear_type;

/*---------------------------------------------------------------------------
  PS_IFACE_IOCTL_DHCP_SERVER_GET_DEVICE_INFO parameter type
---------------------------------------------------------------------------*/
typedef struct ps_iface_ioctl_dhcp_get_device_info_s
{
  dhcp_server_conn_devices_info_s *device_info;
  int32                           num_devices;
  int32                           num_devices_returned;
} ps_iface_ioctl_dhcp_get_device_info_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_RM_GET_SIO_PORT
---------------------------------------------------------------------------*/
typedef int32 ps_iface_ioctl_sio_port_id_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_RMNET_GET_LINK_PROTO_TYPE
---------------------------------------------------------------------------*/
typedef int32 ps_iface_ioctl_link_proto_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_RMNET_GET_IPV4_DEST_MAC_ADDR &
                             PS_IFACE_IOCTL_RMNET_GET_IPV6_DEST_MAC_ADDR
---------------------------------------------------------------------------*/
typedef uint8 * ps_iface_ioctl_mac_addr_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_RMNET_IS_TETHERED_CALL
---------------------------------------------------------------------------*/
typedef boolean ps_iface_ioctl_rmnet_is_tethered_call_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_RMNET_SET_SOFTAP_MODE
---------------------------------------------------------------------------*/
typedef boolean ps_iface_ioctl_rmnet_softap_mode_type;

/*---------------------------------------------------------------------------
  PS_IFACE_IOCTL_SET_PDN_TEARDOWN_CONFIG/
  PS_IFACE_IOCTL_GET_PDN_TEARDOWN_CONFIG structure
---------------------------------------------------------------------------*/
typedef struct
{
  boolean       local_abort;
} ps_iface_ioctl_pdn_teardown_config_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_RMNET_GET_WATERMARKS
---------------------------------------------------------------------------*/
typedef struct ps_iface_ioctl_rmnet_watermarks_s
{
  void *                    rx_wmk;
  void *                    tx_wmk;
} ps_iface_ioctl_rmnet_watermarks_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_UMTS_GET_PDP_CONTEXT_ID
---------------------------------------------------------------------------*/
typedef uint32 ps_iface_ioctl_pdp_context_id_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_RMSM_PPP_BUILD_HEADER
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 *                    header_ptr;
  uint8 *                    header_len_ptr;
} ps_iface_ioctl_ppp_header_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_RMSM_GET_VJ_RESULT
---------------------------------------------------------------------------*/
typedef boolean ps_iface_ioctl_vj_result_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_GET_IFACE_STATS
---------------------------------------------------------------------------*/
typedef struct
{
  uint64  bytes_rx;
  uint64  bytes_tx;
  uint32  pkts_rx;
  uint32  mcast_pkts_rx;
  uint32  pkts_dropped_rx;
  uint32  pkts_tx;
  uint32  mcast_pkts_tx;
  uint32  pkts_dropped_tx;
  uint16  state;
} ps_iface_ioctl_iface_stats_type;

/**
  @brief
  Argument to be passed with PS_IFACE_IOCTL_GET_DEVICE_NAME.

  @details
  This type is used to return device name which is a string of at most 256
  characters long.
*/
#define PS_IFACE_IOCTL_DEVICE_NAME_MAX_SIZE 256
typedef byte ps_iface_ioctl_device_name_type[PS_IFACE_IOCTL_DEVICE_NAME_MAX_SIZE];

/**
  @brief
  Argument to be passed with PS_IFACE_IOCTL_GET_GATEWAY_ADDR.

  @details
  This type is used to return the gateway's IP address.
*/
typedef ip_addr_type ps_iface_ioctl_gateway_addr_type;

/**
  @brief
  Argument to be passed with PS_IFACE_IOCTL_GET_SUBNET_MASK.

  @details
  This type is used to return the IPv4 subnet mask.
*/
typedef ip_addr_type ps_iface_ioctl_subnet_mask_type;

/**
  @brief Data type for specifying the parameters for
         PS_IFACE_IOCTL_QOS_NET_INITIATED_REQUEST

  @see PS_IFACE_IOCTL_QOS_NET_INITIATED_REQUEST
*/
typedef struct
{
  net_initiated_qos_spec_type                 net_initiated_qos_spec;
  /**< QOS specification */
  ps_iface_net_initiated_qos_req_handle_type  handle;
  /**< [out] Handle to this request. It can be used in subsequent operations
             on this request */
} ps_iface_ioctl_qos_net_initiated_request_type;

/**
  @brief Data type for specifying the parameters for
         PS_IFACE_IOCTL_QOS_NET_INITIATED_RELEASE

  @see PS_IFACE_IOCTL_QOS_NET_INITIATED_RELEASE
*/
typedef struct
{
  ps_iface_net_initiated_qos_req_handle_type  handle;
  /**< Handle, which application had obtained from
       PS_IFACE_IOCTL_QOS_NET_INITIATED_REQUEST */
} ps_iface_ioctl_qos_net_initiated_release_type;

/**
  @brief List node for matching_flow_list.

  @see ps_iface_ioctl_qos_net_initiated_real_time_check_type
*/
typedef struct
{
  list_link_type                               link;
  /**< Ptr to next node in a list */
  ps_flow_type *                               flow_ptr;
  /**< Flow whose filters satisfy the net initiated qos request */

} ps_iface_ioctl_matching_flow_type;

/**
@brief Data type for specifying the parameters for
PS_IFACE_IOCTL_QOS_NET_INITIATED_REAL_TIME_CHECK

@see PS_IFACE_IOCTL_QOS_NET_INITIATED_REAL_TIME_CHECK
*/
typedef struct
{
  net_initiated_qos_spec_type                 net_initiated_qos_spec;
  /**< QOS specification */
  list_type                                   matching_flow_list;
  /**< [out] List of ps_iface_ioctl_matching_flow_type *. */

} ps_iface_ioctl_qos_net_initiated_real_time_check_type;


/**
  @brief
  Argument to be passed with PS_IFACE_IOCTL_GET_IPV6_REMAINING_ROUTER_LIFETIME.

  @details
  This type is used to return the IPv6 address's remaining
  router life time.
*/
typedef uint16 ps_iface_ioctl_v6_router_lifetime_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_REVERSE_IP_TRANSPORT_CONFIG_STATUS
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IFACE_IOCTL_REVERSE_IP_TRANSPORT_DISCONNECTED,
    /* Disconnected */

  PS_IFACE_IOCTL_REVERSE_IP_TRANSPORT_CONNECTED
    /* Connected */
} ps_iface_ioctl_reverse_ip_transport_conn_enum_type;

typedef struct
{
  boolean                                     status;
  ps_iface_ioctl_reverse_ip_transport_conn_enum_type conn;
} ps_iface_ioctl_reverse_ip_transport_config_status_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_IS_IPSEC_SA_SHARED
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                     is_shared;
} ps_iface_ioctl_is_ipsec_sa_shared_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_GET_IPSEC_SA
---------------------------------------------------------------------------*/
typedef struct
{
  void                * ipsec_sa;
} ps_iface_ioctl_ipsec_sa_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_707_GET_MIP_MA_INFO
---------------------------------------------------------------------------*/
typedef struct
{
  ip_addr_type   fa_addr; /**< MIP FA address */
} ps_iface_ioctl_mip_ma_info_type;

typedef enum
{
  IFACE_DATA_PATH_HARDWARE, /* Hardware Data Path */
  IFACE_DATA_PATH_SOFTWARE  /* Software Data Path */
} ps_iface_ioctl_data_path_enum_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_SET_DATA_PATH/
  PS_IFACE_IOCTL_GET_DATA_PATH
---------------------------------------------------------------------------*/
typedef struct
{
  uint32        data_path;
} ps_iface_ioctl_data_path_info_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_KEEP_ALIVE
---------------------------------------------------------------------------*/
typedef boolean ps_iface_ioctl_keep_alive_type;


/*---------------------------------------------------------------------------
  Argument to be passed with  PS_IFACE_IOCTL_FILTER_MATCH_REQUEST
---------------------------------------------------------------------------*/

typedef enum
{
  PS_FLTR_DIRECTION_UPLINK,
  PS_FLTR_DIRECTION_DOWNLINK
}ps_fltr_direction_enum_type;

typedef struct
{
  boolean                                is_match;         /**< match to n/w filter. output param. */
  ip_filter_type                         filtr;            /**< filter. */
  ps_fltr_direction_enum_type            direction;        /** Direction Uplink(0) or Downlink(1) */
} ps_ip_filter_match_type;


typedef struct
{
  uint8                       num_filters;      /**< Number of filters in the list. */
  ps_ip_filter_match_type    *list_fitr_ptr;   /**< Pointer to the list of filters. */
  uint32                      qos_handle;      /**< QoS handle */
} ps_iface_ioctl_filter_match_request_type;

typedef struct
{
  ps_iface_ioctl_filter_match_request_type *fltr_info_ptr; /**< filter info */
  int32 flow_handle; /** Flow pointer */
} ps_iface_filter_match_info;


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                           EXTERNALIZED FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_IFACE_IOCTL()

DESCRIPTION
  This function performs various operations on the given ps interface.
  Typically, these operations are to get or set a value.

DEPENDENCIES
  None.

PARAMETERS

  ps_iface_ptr              - ps_iface on which the specified operations
                              is to be performed

  ioctl_name                - The operation name

  argval_ptr                - Pointer to operation specific structure

  ps_errno                  - Error code returned in case of failure (Error
                              values are those defined in dserrno.h)

                              DS_EINVAL - Returned when the specified IOCTL
                              does not belong to the common set of IOCTLs
                              and there is no IOCTL mode handler registered
                              for the specified interface.

                              DS_EOPNOTSUPP - Returned by the lower level
                              IOCTL mode handler when specified IOCTL is not
                              supported by the interface. For instance, this
                              would be returned by interfaces that do not
                              support a certain "iface specific common IOCTL"
                              (i.e. these are common IOCTLs, but the
                              implementation is mode specific, for example,
                              GO_DORMANT).

                              DS_EFAULT - This error code is returned if the
                              specified arguments for the IOCTL are correct
                              but an error is encountered while executing
                              the IOCTL. For instance, if the 1X interface
                              cannot "GO_DORMANT" it would return this error.

                              DS_NOMEMORY - This error code is returned if we
                              run out of buffers during execution.

RETURN VALUE
  0 - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
int ps_iface_ioctl
(
  ps_iface_type            *ps_iface_ptr,
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
);

#ifdef __cplusplus
}
#endif

#endif /* PS_IFACE_IOCTL_H */
