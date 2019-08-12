#ifndef DSS_IFACE_IOCTL_H
#define DSS_IFACE_IOCTL_H

/*===========================================================================

                          D S S _ I F A C E _ I O C T L . H

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

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dss_iface_ioctl.h_v   1.14   25 Feb 2003 14:24:10   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dss_iface_ioctl.h#1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/24/15    pg     Added to support qos handle when registration to net initiated qos.
12/20/14    pg     Added TRAT/SRAT info in Handoff events.
08/05/12    yl     Added new event, which occurs when external IPv6 address 
                   is deleted and carries the deleted IPv6 address.
11/27/11    ss     Added IOCTLs to enable/disable and retrieve ALG status
                   list.
08/31/11    hs     Added ioctl DSS_IFACE_IOCTL_GET_SUBNET_MASK.
08/30/11    hs     Added ioctl DSS_IFACE_IOCTL_GET_GATEWAY_ADDR. 
08/28/11    aa     Added new ioctls for Network Initiated QoS.
08/24/11    hs     Added ioctl DSS_IFACE_IOCTL_GET_DEVICE_NAME.
08/03/11    mct    Added new event for reporting fast dormancy status to apps.
04/27/11    sy     Added ioctl DSS_IFACE_IOCTL_707_GET_SIP_MIP_DATA_CALL_TYPE.
04/27/11    sy     Added additional failure codes to HDR slotted failure enum.
01/25/11    dm     Changed the name to ps_iface_ioctl_iface_stats_type.
12/16/10    dm     Added IOCTL to get/set L2TP/PPTP VPN Passthrough.
11/16/10    dm     Added IOCTL to get NAT public IP address
10/25/10    rt     Added GET_IFACE_STATS and RESET_IFACE_STATS IOCTL's.
10/11/10    dm     Added DMZ IOCTLs.
08/31/10    dm     Updated enable/disable firewall signature.
07/30/10    dm     Update firewall IOCTL signature.
07/30/10    bq     Added NAT_IPSEC_VPN_PASSTHROUGH.
07/02/10    cp     Added DHCP ARP cache UPDATE & CLEAR IOCTLS.
06/02/10    cp     IOCTL changes for Soft AP DHCP server.
05/23/10    bq     NAT related configuration
08/31/09   mga     Merged from eHRPD branch
03/26/09    pp     CMI De-featurization.
12/28/08    pp     Common Modem Interface: Public/Private API split.
03/11/08    am     Changed DSS_INVALID_HANDLE_TYPE def.
10/24/08    am     Fixed compiler warnings for off-target.
09/02/08    dm     Added Network initiated QoS support
07/07/08    dm     Removed DSS_IFACE_MAX_MCAST_FLOWS constant
05/22/08    am     Added boolean force_dereg_cbacks to
                   dss_iface_ioctl_mcast_event_info_type.
05/15/08    dm     Added support to pass the dormancy info code (reject cause)
                   in case of RAB reestablishment reject to applications
04/29/08    pp     IS707b: Added GET/SET Hysteresis Activation Timer IOCTLs.
04/18/08    dm     BCMCS 2.0 support. Added two new ioctl's:
                   DSS_IFACE_IOCTL_BCMCS_BOM_CACHING_SETUP and
                   DSS_IFACE_IOCTL_BCMCS_ENABLE_HANDOFF_REG
04/03/08    am     BCMCS 2.0 support.
01/30/08    dm     Removed Feature FEATURE_JCDMA_2
07/31/07    sv     Added support for MBMS context act/deact ioctls.
04/17/07    es     Added REFRESH_DHCP_CONFIG_INFO ioctl, removed
                   GET_DHCP_CONFIG_INFO IOCTL.
03/27/07    msr    Added API for RF conditions and HDR-1x handdown
02/21/07    rt     Removed the DHCP refresh time option.
02/05/07    sv     Added support for IM_CN flag.
02/01/07    hm     Added IOCTL to get fresh DHCP configuration
01/23/07    msr    EMPA enhancements
11/14/06    rsj    Added changes for JCDMA Session Timer. Created two new
                   IOCTL's: DSS_IFACE_IOCTL_SET_SESSION_TIMER and
                   DSS_IFACE_IOCTL_GET_SESSION_TIMER.
10/23/06    rsj    Added changes for JCDMA 2 Low Battery Event processing.
12/15/06    mct    Export IPv6 priv addr deleted event.
12/04/06    asn    ENH to propagate SM cause-codes to App
11/16/06    msr    Added DSS_IFACE_IOCTL_QOS_INFO_CODE_UPDATED_EV
11/04/06    hm     Added IOCTL for domain name search list option for DHCPv6
11/02/06    mct    Added support for IPv6 privacy extensions and exporting
                   prefix update event.
11/01/06    rt     Merged EIDLE_SCI feature changes.
08/14/06    sv     Merged slotted/Mode attribute ioctl.
07/31/06    msr    Added support for PRIMARY_QOS_MODIFY
06/01/06    mct    Added support for retrieving all DNS addresses in one call.
05/01/06    rt     Renamed DSS_IFACE_IOCTL_GET_TX_STATUS to
                   DSS_IFACE_IOCTL_707_GET_TX_STATUS
05/01/06    msr    Added support for DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2
04/30/06    rt     Added code for SIP server and domain names. Also added
                   ioctls for HDR RMAC3 info and Tx Status.
04/14/06    ss     Added DVB-H specific info codes
03/28/06    mpa    Added new BCMCS flow status.
02/13/06    mct    Added a new QOS ioctl and new BCMCS/QOS info codes.
01/25/06    mct    Renamed some qos info codes.
12/01/05    mct    Added support for ON_QOS_AWARE_SYSTEM ioctl.
10/12/05    mct    Updated some IOCTL naming. Added support for
                   QOS_ADDED/DELETED on iface events.
09/29/05    msr    Added DSS_IFACE_IOCTL_IS_LAPTOP_CALL_ACTIVE
09/15/05    mct    Featurized BCMCS code.
09/13/05    sv     Added support for new HDR rev0 rate inertia and HDR HPT mode
                   ioctls.
09/01/05    mct    Added support for BCMCS_DB_UPDATE.
09/01/05    rt     Type defined dss_iface_ioctl_bearer_tech_changed_type as
                   ps_iface_ioctl_bearer_tech_changed_type
08/26/05    mpa    Added new BCMCS flow statuses
08/09/05    mct    Added support for QOS bundle APIs, QOS_CONFIGURE, and
                   QOS_AWARE/QOS_UNAWARE events. Fixed ps qos info_code bug.
08/15/05    msr    Aliased DSS_IFACE_IOCTL_GET_FLOW_SPEC to
                   DSS_IFACE_IOCTL_GET_GRANTED_FLOW_SPEC
08/14/05    rt     Added DSS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY and
                   DSS_IFACE_IOCTL_GET_DATA_BEARER_TECHNOLOGY ioctls.
08/10/05    msr    Added DSS_IFACE_IOCTL_QOS_MODIFY_ACCEPTED_EV,
                   DSS_IFACE_IOCTL_QOS_MODIFY_REJECTED_EV events
07/28/05    rt     Added new ioctl's DSS_IFACE_IOCTL_GET_IFACE_NAME and
                   DSS_IFACE_IOCTL_GET_IP_FAMILY.
07/25/05    ks     Moved definition of DSS_IFACE_IOCTL_GET_TX_QUEUE_LEVEL
                   below to indicate it is a Reserved IOCTL.
06/10/05    mct    Updated events and info codes for Multicast interfaces.
04/18/05    mct    Added support for Multicast IOCTLs MCAST_JOIN and
                   MCAST_LEAVE.
04/18/05    ks     Added new ioctl, DSS_IFACE_IOCTL_GET_TX_QUEUE_LEVEL.
04/17/05    msr    Added PS_FLOW_IOCTL_QOS_RESUME and
                   PS_FLOW_IOCTL_QOS_SUSPEND and moved QOS_RELEASE and
                   QOS_MODIFY to flow IOCTLs.
04/16/05    ks     Added support for new ioctl, DSS_IFACE_GET_HW_ADDR
11/02/04    vp     Renamed PS_IFACE_IOCTL_GET_PHYS_LINK_STATE to
                   PS_PHYS_LINK_IOCTL_GET_STATE.
10/07/04    mct    Added support for Mobile Terminated PDP. Added two IOCTLs
                   for registering and deregistering.
10/06/04    sv     changed GO_ACTIVE and GO_DORMANT ioctls to phys_link ioctls.
09/23/04    jd     Added definitions for IP filter APIs
09/17/04    mct    Fixed issue where the event_q_array was not being init'd
                   to include the ADDR_CHANGED_EV.
07/11/04    aku    Added suport for the QOS feature.
07/14/04    mct    Added new IOCTL support for getting V6 primary/secondary
                   DNS addresses and retrieving the v6 and generic ip addrs.
07/12/04    gr     Added IOCTLs to Get and Set the 1x RLP Fill Rate
06/20/04    mct    Updated dependency between ps_bcmcs.h and ip_addr_type.
06/11/04    vp     Added declaration of ip_addr_type
04/28/04    aku    Added support for BCMCS related IOCTLs.
03/30/04    vp     Merged changes from June04Dev_05.00.00 PS branch.
02/18/04    usb    IP filtering library interface change.
11/11/03    sv     Added ioctl for disabling DNS negotiation during ipcp
                   configuration.
10/28/03    sv     Added version macro for new transient state events.
10/24/03    sv     Added event call back support for iface and phys link
                   transient states.
10/09/03    jd     Added IP snooping reg/dereg IOCTL enums and parameter
                   typedefs
08/28/03    ss     Moved dss_iface_get_id() and associated decls. to
                   dssocket_defs.h. Modified to use dss_iface_id_type.
07/07/03    atp    Renamed SDB flags for SDB support query ioctl, as part of
                   SDB enhancements.
05/05/03    aku    Added support for DSS_IFACE_IOCTL_GET_ALL_IFACES and added
                   the loopback interface to list of interfaces.
04/30/03    aku    Moved dss_get_iface_id() prototype from dssocket.h
03/05/03    aku    Removed feature definitions from header file and added
                   void typdef for all IOCTLs that do not need to pass in an
                   arg.
02/25/03    atp    Added ioctl for enabling/disabling holddown timer for 707.
02/12/03    aku    Updated comment.
02/12/03    atp    Made GO_ACTIVE, GO_DORMANT, GO_NULL ioctls common, not
                   707-specific.
02/12/03    atp    Introduced new definition for 707 RLP NAK policy to avoid
                   include of dsrlp.h.
02/12/03    aku    Changed errnos to use ones in defined in dserrno.h
02/07/03    usb    Added ioctls for reg and dereg inbound IP filters
02/06/03    atp    Added SDB support query ioctl for 707.
02/06/03    atp    Moved functionality of dsiface (w.r.t 707 - RLP NAK policy
                   dorm timer, MDR, QOS) to ioctls. dsiface to be deprecated.
01/31/03    atp    Added GoNull ioctl for 707.
01/17/03    atp    Added support for 707 ioctls.
01/17/03    usb    Defined all enums to be 32 bit wide, max value of enums
                   changed from 0xFFFFFFFF to 0x7FFFFFFF, removed
                   DSS_IFACE_IOCTL_ALL - all interfaces currently not
                   supported for ioctls.
12/24/02    aku    Added support for reg/dereg event callbacks and added
                   support for specifying interface using name and instance.
11/23/02    aku    Removed support for sockfd as identifier and iface stats.
11/17/02    aku    added interface statistics IOCTL
11/10/02    aku    created file
===========================================================================*/

#include "comdef.h"
#include "dserrno.h"
#include "dssocket_defs.h"
#include "ps_iface.h"
#include "ps_iface_defs.h"
#include "ps_qos_defs.h"
#include "ps_in.h"
#include "ps_iface_ioctl.h"
#include "ps_flow_ioctl.h"
#include "ps_phys_link_ioctl.h"
#include "ps_ipfltr_defs.h"
#include "ds_sys.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      REGIONAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  Definition of various interface control operations. Please do NOT change
  the values of the existing enumerated types when adding new ones (to ensure
  that object code released with previous versions of the code do not break).
  Each of the following control operations have associated tyepdef (with
  corresponding names) that are passed in or returned for get/set operations.

  Also ensure that symetric operations are grouped.
---------------------------------------------------------------------------*/
typedef enum
{
  /*-------------------------------------------------------------------------

                              COMMON IOCTLS

    The following operations are common to all interfaces.
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_MIN                     = PS_IFACE_IOCTL_MIN,
  /* Get IPV4 addr of iface */
  DSS_IFACE_IOCTL_GET_IPV4_ADDR           = PS_IFACE_IOCTL_GET_IPV4_ADDR,
  /* Get IPV6 addr of iface */
  DSS_IFACE_IOCTL_GET_IPV6_ADDR           = PS_IFACE_IOCTL_GET_IPV6_ADDR,
  /* Get Primary DNS (IPV4) addr of iface */
  DSS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR  =
                                       PS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR,
  /* Get Primary DNS (IPV6) addr of iface */
  DSS_IFACE_IOCTL_GET_IPV6_PRIM_DNS_ADDR  =
                                       PS_IFACE_IOCTL_GET_IPV6_PRIM_DNS_ADDR,
  /* Get Secondary DNS (IPV4) addr of iface */
  DSS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR  =
                                       PS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR,
  /* Get Secondary DNS (IPV6) addr of iface */
  DSS_IFACE_IOCTL_GET_IPV6_SECO_DNS_ADDR  =
                                       PS_IFACE_IOCTL_GET_IPV6_SECO_DNS_ADDR,
  /* Get ALL DNS addrs addr of an iface */
  DSS_IFACE_IOCTL_GET_ALL_DNS_ADDRS       = PS_IFACE_IOCTL_GET_ALL_DNS_ADDRS,
  /* Get MTU of iface */
  DSS_IFACE_IOCTL_GET_MTU                 = PS_IFACE_IOCTL_GET_MTU,
  /* Get IPADDR of iface */
  DSS_IFACE_IOCTL_GET_IP_ADDR             = PS_IFACE_IOCTL_GET_IP_ADDR,
  /* Get state of iface */
  DSS_IFACE_IOCTL_GET_STATE               = PS_IFACE_IOCTL_GET_STATE,
  /* Get physical link state */
  DSS_IFACE_IOCTL_GET_PHYS_LINK_STATE     = PS_PHYS_LINK_IOCTL_GET_STATE,
  /* Register callback for events. Note that app can have only one callback
     per interface for each event */
  DSS_IFACE_IOCTL_REG_EVENT_CB            = PS_IFACE_IOCTL_REG_EVENT_CB,
  /* Deregister event callback */
  DSS_IFACE_IOCTL_DEREG_EVENT_CB          = PS_IFACE_IOCTL_DEREG_EVENT_CB,
  /* Get all enabled ifaces. Note for that this IOCTL, a NULL id_ptr has to
     be passed as a parameter */
  DSS_IFACE_IOCTL_GET_ALL_IFACES          = PS_IFACE_IOCTL_GET_ALL_IFACES,
  /* Request for a specific QOS                     */
  DSS_IFACE_IOCTL_QOS_REQUEST             = PS_IFACE_IOCTL_QOS_REQUEST,
  /* Requests for multiple QOS instances */
  DSS_IFACE_IOCTL_QOS_REQUEST_EX          = PS_IFACE_IOCTL_QOS_REQUEST_EX,
  /* Get the HW Address of the iface */
  DSS_IFACE_IOCTL_GET_HW_ADDR             = PS_IFACE_IOCTL_GET_HW_ADDR,
  /* Get the iface name */
  DSS_IFACE_IOCTL_GET_IFACE_NAME          = PS_IFACE_IOCTL_GET_IFACE_NAME,
  /* Get the iface IP family */
  DSS_IFACE_IOCTL_GET_IP_FAMILY           = PS_IFACE_IOCTL_GET_IP_FAMILY,
  /* Get the RF condition */
  DSS_IFACE_IOCTL_GET_RF_CONDITIONS       = PS_IFACE_IOCTL_GET_RF_CONDITIONS,
  /* Modify an existing QOS */
  DSS_IFACE_IOCTL_QOS_MODIFY              = PS_FLOW_IOCTL_QOS_MODIFY,
  /* Release QOS */
  DSS_IFACE_IOCTL_QOS_RELEASE             = PS_FLOW_IOCTL_QOS_RELEASE,
  /* Get QOS flow spec granted by the network */
  DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC  =
                                     PS_FLOW_IOCTL_QOS_GET_GRANTED_FLOW_SPEC,
  /* Aliasing to preserve backward compatibility */
  DSS_IFACE_IOCTL_QOS_GET_FLOW_SPEC       =
                                   DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC,
  /* Extended IOCTL to get QOS flow spec granted by the network */
  DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2       =
                                   PS_FLOW_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2,
  /* Get QOS staus */
  DSS_IFACE_IOCTL_QOS_GET_STATUS          = PS_FLOW_IOCTL_QOS_GET_STATUS,
  /* Modify existing QOS on primary */
  DSS_IFACE_IOCTL_PRIMARY_QOS_MODIFY      = PS_FLOW_IOCTL_PRIMARY_QOS_MODIFY,
  /* Get QOS flow spec granted by the network on primary */
  DSS_IFACE_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC =
                              PS_FLOW_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC,
  /** Get QOS filter spec */
  DSS_IFACE_IOCTL_QOS_GET_GRANTED_FILTER_SPEC =
                              PS_FLOW_IOCTL_QOS_GET_GRANTED_FILTER_SPEC,
  /* Get the SIP Server addresses */
  DSS_IFACE_IOCTL_GET_SIP_SERV_ADDR       = PS_IFACE_IOCTL_GET_SIP_SERV_ADDR,
  /* Get the SIP Server domain names */
  DSS_IFACE_IOCTL_GET_SIP_SERV_DOMAIN_NAMES =
                                    PS_IFACE_IOCTL_GET_SIP_SERV_DOMAIN_NAMES,
  DSS_IFACE_IOCTL_GENERATE_PRIV_IPV6_ADDR =
                                    PS_IFACE_IOCTL_GENERATE_PRIV_IPV6_ADDR,
  /* Get the domain names search list */
  DSS_IFACE_IOCTL_GET_DOMAIN_NAME_SEARCH_LIST =
                                    PS_IFACE_IOCTL_GET_DOMAIN_NAME_SEARCH_LIST,
  /* Refresh the DHCP configuration info */
  DSS_IFACE_IOCTL_REFRESH_DHCP_CONFIG_INFO =
                                    PS_IFACE_IOCTL_REFRESH_DHCP_CONFIG_INFO,
  DSS_IFACE_IOCTL_GET_ALL_V6_PREFIXES =
                                    PS_IFACE_IOCTL_GET_ALL_V6_PREFIXES,
  /* Get dormancy info code when primary phys link is down*/
  DSS_IFACE_IOCTL_GET_DORMANCY_INFO_CODE  =
                                    PS_PHYS_LINK_IOCTL_GET_DORMANCY_INFO_CODE,
  /* Get the QoS control mode */
  DSS_IFACE_IOCTL_QOS_GET_MODE = PS_IFACE_IOCTL_QOS_GET_MODE,

  /* Enable the firewall */
  DSS_IFACE_IOCTL_ENABLE_FIREWALL = PS_IFACE_IOCTL_ENABLE_FIREWALL,

  /* Disable the firewall */
  DSS_IFACE_IOCTL_DISABLE_FIREWALL = PS_IFACE_IOCTL_DISABLE_FIREWALL,

  /* Add firewall rule*/
  DSS_IFACE_IOCTL_ADD_FIREWALL_RULE = PS_IFACE_IOCTL_ADD_FIREWALL_RULE,

  /* Delete firewall rule*/
  DSS_IFACE_IOCTL_DELETE_FIREWALL_RULE = PS_IFACE_IOCTL_DELETE_FIREWALL_RULE,

  /* Get firewall rule*/
  DSS_IFACE_IOCTL_GET_FIREWALL_RULE = PS_IFACE_IOCTL_GET_FIREWALL_RULE,

  /* Get firewall table*/
  DSS_IFACE_IOCTL_GET_FIREWALL_TABLE = PS_IFACE_IOCTL_GET_FIREWALL_TABLE,

  /* ARP Cache update ioctl for DHCP server unicast messages */
  DSS_IFACE_IOCTL_DHCP_ARP_CACHE_UPDATE   =
                                PS_IFACE_IOCTL_DHCP_ARP_CACHE_UPDATE,

  /* ARP Cache clear ioctl for DHCP server unicast messages */
  DSS_IFACE_IOCTL_DHCP_ARP_CACHE_CLEAR   =
                                PS_IFACE_IOCTL_DHCP_ARP_CACHE_CLEAR,

  /* Get the devices information which are connected to DHCP server
     at any given point of time */
  DSS_IFACE_IOCTL_DHCP_SERVER_GET_DEVICE_INFO   =
                                PS_IFACE_IOCTL_DHCP_SERVER_GET_DEVICE_INFO,

  /* Get the iface statistics */
  DSS_IFACE_IOCTL_GET_IFACE_STATS = PS_IFACE_IOCTL_GET_IFACE_STATS,
  /* Reset the iface statistics */
  DSS_IFACE_IOCTL_RESET_IFACE_STATS = PS_IFACE_IOCTL_RESET_IFACE_STATS,

  /** Request to monitor Network Initiated QoS (per specific qos filters) */
  DSS_IFACE_IOCTL_QOS_NET_INITIATED_REQUEST =
                                    PS_IFACE_IOCTL_QOS_NET_INITIATED_REQUEST,

  /**
    Request to stop monitoring Network Initiated QoS
    (per specific previous request)
  */
  DSS_IFACE_IOCTL_QOS_NET_INITIATED_RELEASE =
                                    PS_IFACE_IOCTL_QOS_NET_INITIATED_RELEASE,

  /* Set data path preference */
  DSS_IFACE_IOCTL_SET_DATA_PATH_BRIDGE_MODE = PS_IFACE_IOCTL_SET_DATA_PATH_BRIDGE_MODE,
  /* Get data path */
  DSS_IFACE_IOCTL_GET_DATA_PATH_BRIDGE_MODE = PS_IFACE_IOCTL_GET_DATA_PATH_BRIDGE_MODE,

  /* Get OP_RES_PCO */
  DSS_IFACE_IOCTL_GET_OP_RES_PCO = PS_IFACE_IOCTL_GET_OP_RES_PCO,

  /* Keep alive */
  DSS_IFACE_IOCTL_KEEP_ALIVE = PS_IFACE_IOCTL_KEEP_ALIVE,

  /*-------------------------------------------------------------------------
    The following operations are common, but implementation is specific to
    a particular iface. Some may not even be supported by a particular iface.
  -------------------------------------------------------------------------*/
  /* Send multiple QOS releases in one message */
  DSS_IFACE_IOCTL_QOS_RELEASE_EX          = PS_FLOW_IOCTL_QOS_RELEASE_EX,
    /* Send multiple QOS suspends in one message */
  DSS_IFACE_IOCTL_QOS_SUSPEND_EX          = PS_FLOW_IOCTL_QOS_SUSPEND_EX,
  /* Send multiple QOS resumes in one message */
  DSS_IFACE_IOCTL_QOS_RESUME_EX          = PS_FLOW_IOCTL_QOS_RESUME_EX,
  /* Bring flow out of dormancy */
  DSS_IFACE_IOCTL_QOS_RESUME              = PS_FLOW_IOCTL_QOS_RESUME,
  /* Suspend a flow */
  DSS_IFACE_IOCTL_QOS_SUSPEND             = PS_FLOW_IOCTL_QOS_SUSPEND,
  /* Go active from dormant */
  DSS_IFACE_IOCTL_GO_ACTIVE               = PS_PHYS_LINK_IOCTL_GO_ACTIVE,
  /* Go dormant */
  DSS_IFACE_IOCTL_GO_DORMANT              = PS_PHYS_LINK_IOCTL_GO_DORMANT,
  /* Go NULL  */
  DSS_IFACE_IOCTL_GO_NULL                 = PS_IFACE_IOCTL_GO_NULL,
  /* IPCP dns option         */
  DSS_IFACE_IOCTL_IPCP_DNS_OPT            = PS_IFACE_IOCTL_IPCP_DNS_OPT,
  /* Register for a Mobile Terminated event callback */
  DSS_IFACE_IOCTL_MT_REG_CB               = PS_IFACE_IOCTL_MT_REG_CB,
  /* Deregister for a Mobile Terminated event callback */
  DSS_IFACE_IOCTL_MT_DEREG_CB             = PS_IFACE_IOCTL_MT_DEREG_CB,
  /* Multicast join and leave IOCTLs for registering and deregistering
     with a multicast IP and port. */
  DSS_IFACE_IOCTL_MCAST_JOIN              = PS_IFACE_IOCTL_MCAST_JOIN,
  /* Leave a Multicast group */
  DSS_IFACE_IOCTL_MCAST_LEAVE             = PS_IFACE_IOCTL_MCAST_LEAVE,
  /* Query to see if mobile is on a QOS aware system */
  DSS_IFACE_IOCTL_ON_QOS_AWARE_SYSTEM     = PS_IFACE_IOCTL_ON_QOS_AWARE_SYSTEM,
  /* Get the Mode of the iface */
  DSS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY   =
                                       PS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY,
  /**
   Get the bearer technology, this supersedes the DSS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY
  */
  DSS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY_EX =
                                     PS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY_EX,
  /* GET THE DATA BEARER RATE */
  DSS_IFACE_IOCTL_GET_DATA_BEARER_RATE    =
                                       PS_IFACE_IOCTL_GET_DATA_BEARER_RATE,
  /* Return TRUE if iface is in a laptop call */
  DSS_IFACE_IOCTL_IS_LAPTOP_CALL_ACTIVE  =
                                       PS_IFACE_IOCTL_IS_LAPTOP_CALL_ACTIVE,
  /** Get HLOS specific device name corresponding to the iface */
  DSS_IFACE_IOCTL_GET_DEVICE_NAME         = PS_IFACE_IOCTL_GET_DEVICE_NAME,
  /** Get gateway ip address. */
  DSS_IFACE_IOCTL_GET_GATEWAY_ADDR       = PS_IFACE_IOCTL_GET_GATEWAY_ADDR,
  /** Get IPv4 subnet mask. */
  DSS_IFACE_IOCTL_GET_SUBNET_MASK        = PS_IFACE_IOCTL_GET_SUBNET_MASK,
  /* BCMCS 2.0 support */
  /* Join (register/configure) multiple Multicast flows. */
  DSS_IFACE_IOCTL_MCAST_JOIN_EX           = PS_IFACE_IOCTL_MCAST_JOIN_EX,
  /* Leave multiple Multicast groups */
  DSS_IFACE_IOCTL_MCAST_LEAVE_EX          = PS_IFACE_IOCTL_MCAST_LEAVE_EX,
  /* Trigger multiple Multicast flow Registration */
  DSS_IFACE_IOCTL_MCAST_REGISTER_EX       = PS_IFACE_IOCTL_MCAST_REGISTER_EX,
  /* Set the PDN tear down configuration */
  DSS_IFACE_IOCTL_SET_PDN_TEARDOWN_CONFIG = PS_IFACE_IOCTL_SET_PDN_TEARDOWN_CONFIG,
  /* Get the PDN tear down configuration */
  DSS_IFACE_IOCTL_GET_PDN_TEARDOWN_CONFIG = PS_IFACE_IOCTL_GET_PDN_TEARDOWN_CONFIG,
  /* Trigger to perform filter match */
  DSS_IFACE_IOCTL_FILTER_MATCH_REQUEST       = PS_IFACE_IOCTL_FILTER_MATCH_REQUEST,

  /*-------------------------------------------------------------------------

   NAT related IOCTL.

  -------------------------------------------------------------------------*/

  /* Get Static NAT Entry    */
  DSS_IFACE_IOCTL_GET_STATIC_NAT_ENTRY    = PS_IFACE_IOCTL_GET_STATIC_NAT_ENTRY,

  /* Add Static NAT Entry    */
  DSS_IFACE_IOCTL_ADD_STATIC_NAT_ENTRY    = PS_IFACE_IOCTL_ADD_STATIC_NAT_ENTRY,

  /* Get Dynamic NAT Entry Timeout */
  DSS_IFACE_IOCTL_GET_DYNAMIC_NAT_ENTRY_TIMEOUT = PS_IFACE_IOCTL_GET_DYNAMIC_NAT_ENTRY_TIMEOUT,

  /* Set Dynamic NAT Entry Timeout */
  DSS_IFACE_IOCTL_SET_DYNAMIC_NAT_ENTRY_TIMEOUT = PS_IFACE_IOCTL_SET_DYNAMIC_NAT_ENTRY_TIMEOUT,

  /* Delete Static NAT Entry */
  DSS_IFACE_IOCTL_DELETE_STATIC_NAT_ENTRY = PS_IFACE_IOCTL_DELETE_STATIC_NAT_ENTRY,

  /* Get NAT IPSEC VPN Pass Through */
  DSS_IFACE_IOCTL_GET_NAT_IPSEC_VPN_PASS_THROUGH = PS_IFACE_IOCTL_GET_NAT_IPSEC_VPN_PASS_THROUGH,

  /* Set NAT IPSEC VPN Pass Through */
  DSS_IFACE_IOCTL_SET_NAT_IPSEC_VPN_PASS_THROUGH = PS_IFACE_IOCTL_SET_NAT_IPSEC_VPN_PASS_THROUGH,

  /* Add DeMilitarized Zone */
  DSS_IFACE_IOCTL_ADD_DMZ = PS_IFACE_IOCTL_ADD_DMZ,

  /* Get DeMilitarized Zone */
  DSS_IFACE_IOCTL_GET_DMZ = PS_IFACE_IOCTL_GET_DMZ,

  /* Delete DeMilitarized Zone */
  DSS_IFACE_IOCTL_DELETE_DMZ = PS_IFACE_IOCTL_DELETE_DMZ,

  /* Get NAT Public IP address */
  DSS_IFACE_IOCTL_GET_NAT_PUBLIC_IP_ADDR    = PS_IFACE_IOCTL_GET_NAT_PUBLIC_IP_ADDR,

  /* Get NAT L2TP VPN Pass Through */
  DSS_IFACE_IOCTL_GET_NAT_L2TP_VPN_PASS_THROUGH = PS_IFACE_IOCTL_GET_NAT_L2TP_VPN_PASS_THROUGH,

  /* Set NAT L2TP VPN Pass Through */
  DSS_IFACE_IOCTL_SET_NAT_L2TP_VPN_PASS_THROUGH = PS_IFACE_IOCTL_SET_NAT_L2TP_VPN_PASS_THROUGH,

  /* Get NAT PPTP VPN Pass Through */
  DSS_IFACE_IOCTL_GET_NAT_PPTP_VPN_PASS_THROUGH = PS_IFACE_IOCTL_GET_NAT_PPTP_VPN_PASS_THROUGH,

  /* Set NAT PPTP VPN Pass Through */
  DSS_IFACE_IOCTL_SET_NAT_PPTP_VPN_PASS_THROUGH = PS_IFACE_IOCTL_SET_NAT_PPTP_VPN_PASS_THROUGH,

  /* Enable an ALG client */
  DSS_IFACE_IOCTL_ENABLE_ALG = PS_IFACE_IOCTL_ENABLE_ALG,

  /* Disable an ALG client */
  DSS_IFACE_IOCTL_DISABLE_ALG = PS_IFACE_IOCTL_DISABLE_ALG,

  /* Get ALG status list */
  DSS_IFACE_IOCTL_GET_ALG_STATUS = PS_IFACE_IOCTL_GET_ALG_STATUS,

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
  /* 1x QCMDR value */
  DSS_IFACE_IOCTL_707_GET_MDR             = PS_IFACE_IOCTL_707_GET_MDR,
  /* 1x QCMDR value */
  DSS_IFACE_IOCTL_707_SET_MDR             = PS_IFACE_IOCTL_707_SET_MDR,
  /* 1x dorm timer value */
  DSS_IFACE_IOCTL_707_GET_DORM_TIMER      =
                                           PS_IFACE_IOCTL_707_GET_DORM_TIMER,
  /* 1x dorm timer value */
  DSS_IFACE_IOCTL_707_SET_DORM_TIMER      =
                                           PS_IFACE_IOCTL_707_SET_DORM_TIMER,
  /* 1x RLP curr NAK policy */
  DSS_IFACE_IOCTL_707_GET_RLP_ALL_CURR_NAK=
                                     PS_IFACE_IOCTL_707_GET_RLP_ALL_CURR_NAK,
  /* 1x RLP curr NAK policy */
  DSS_IFACE_IOCTL_707_SET_RLP_ALL_CURR_NAK=
                                     PS_IFACE_IOCTL_707_SET_RLP_ALL_CURR_NAK,
  /* 1x RLP  default NAK */
  DSS_IFACE_IOCTL_707_GET_RLP_ALL_DEF_NAK =
                                      PS_IFACE_IOCTL_707_GET_RLP_ALL_DEF_NAK,
  /* 1x RLP  default NAK */
  DSS_IFACE_IOCTL_707_SET_RLP_ALL_DEF_NAK =
                                      PS_IFACE_IOCTL_707_SET_RLP_ALL_DEF_NAK,
  /* 1x RLP  negotiated NAK */
  DSS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK =
                                      PS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK,
  /* 1x QOS nonassured prio */
  DSS_IFACE_IOCTL_707_GET_RLP_QOS_NA_PRI  =
                                       PS_IFACE_IOCTL_707_GET_RLP_QOS_NA_PRI,
  /* 1x QOS nonassured prio */
  DSS_IFACE_IOCTL_707_SET_RLP_QOS_NA_PRI  =
                                       PS_IFACE_IOCTL_707_SET_RLP_QOS_NA_PRI,
  /* 1x SDB possible query */
  DSS_IFACE_IOCTL_707_SDB_SUPPORT_QUERY   =
                                        PS_IFACE_IOCTL_707_SDB_SUPPORT_QUERY,
  /* 1x enable/disable holddown timer */
  DSS_IFACE_IOCTL_707_ENABLE_HOLDDOWN     =
                                          PS_IFACE_IOCTL_707_ENABLE_HOLDDOWN,

  /* Enable the HDR High Priority traffic mode. This mode will enable SHDR
     (maye also DDTM) */
  DSS_IFACE_IOCTL_707_ENABLE_HDR_HPT_MODE =
                              PS_IFACE_IOCTL_707_ENABLE_HDR_HPT_MODE,

  /* Fetch whether handdown from HDR to 1x is enabled */
  DSS_IFACE_IOCTL_707_GET_HDR_1X_HANDDOWN_OPTION =
                              PS_IFACE_IOCTL_707_GET_HDR_1X_HANDDOWN_OPTION,

  /* Enable/disable handdown from HDR to 1x */
  DSS_IFACE_IOCTL_707_SET_HDR_1X_HANDDOWN_OPTION =
                              PS_IFACE_IOCTL_707_SET_HDR_1X_HANDDOWN_OPTION,

  /* Set the mode for operating VT on a Rev 0 HDR system. This will end up
     sending a GAUP message to the AN for changing the RTCMAC rate transition
     parameters (may also change the DPA RLP parameters */
  DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA =
                          PS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA,
  DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INTERIA =
                          DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA,

/* Retrieve all supported QOS profiles */
  DSS_IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES =
                           PS_IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES,
  /* Get HDR RMAC3 Information */
  DSS_IFACE_IOCTL_707_HDR_GET_RMAC3_INFO  = PS_FLOW_IOCTL_HDR_GET_RMAC3_INFO,
  /* Get the Transmission Status */
  DSS_IFACE_IOCTL_707_GET_TX_STATUS       = PS_FLOW_IOCTL_707_GET_TX_STATUS,
  /* IOCTLs to get/set inactivity timer for a ps_flow */
  DSS_IFACE_IOCTL_707_GET_INACTIVITY_TIMER =
                                     PS_FLOW_IOCTL_707_GET_INACTIVITY_TIMER,
  DSS_IFACE_IOCTL_707_SET_INACTIVITY_TIMER =
                                     PS_FLOW_IOCTL_707_SET_INACTIVITY_TIMER,

/* Enable the slot cycle index mode. This mode will allow for the slot cycle
   index value to be set by the application */

  DSS_IFACE_IOCTL_707_ENABLE_HDR_SLOTTED_MODE =
                               PS_IFACE_IOCTL_707_ENABLE_HDR_SLOTTED_MODE,

  DSS_IFACE_IOCTL_SET_SESSION_TIMER        = PS_IFACE_IOCTL_SET_SESSION_TIMER, /* Set Session Timer value */
  DSS_IFACE_IOCTL_GET_SESSION_TIMER        = PS_IFACE_IOCTL_GET_SESSION_TIMER , /* Get Sesion Timer value */

  /* IOCTLs to get/set Hysteresis Activation Timer */
  DSS_IFACE_IOCTL_707_GET_HYSTERESIS_ACT_TIMER =
                                PS_IFACE_IOCTL_707_GET_HYSTERESIS_ACT_TIMER,
  DSS_IFACE_IOCTL_707_SET_HYSTERESIS_ACT_TIMER =
                                PS_IFACE_IOCTL_707_SET_HYSTERESIS_ACT_TIMER,

  /* Get SIP, MIP data call type */
  DSS_IFACE_IOCTL_707_GET_SIP_MIP_DATA_CALL_TYPE =
                                PS_IFACE_IOCTL_707_GET_SIP_MIP_DATA_CALL_TYPE,

  /* Get MIP info */
  DSS_IFACE_IOCTL_707_GET_MIP_MA_INFO = PS_IFACE_IOCTL_707_GET_MIP_MA_INFO,

  /*-------------------------------------------------------------------------

                              UMTS IOCTLS

    The following operations are specific to UMTS interface.Start from 300.
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_UMTS_GET_IM_CN_FLAG      = PS_IFACE_IOCTL_UMTS_GET_IM_CN_FLAG,
  DSS_IFACE_IOCTL_MBMS_MCAST_CONTEXT_ACTIVATE =
                                    PS_IFACE_IOCTL_MBMS_MCAST_CONTEXT_ACTIVATE,
  DSS_IFACE_IOCTL_MBMS_MCAST_CONTEXT_DEACTIVATE =
                                  PS_IFACE_IOCTL_MBMS_MCAST_CONTEXT_DEACTIVATE,
  DSS_IFACE_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE = 
                                  PS_IFACE_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE, 
  DSS_IFACE_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE = 
                                  PS_IFACE_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE, 

  DSS_IFACE_IOCTL_SET_SILENT_REDIAL  = PS_IFACE_IOCTL_SET_SILENT_REDIAL,
  DSS_IFACE_IOCTL_3GPP_FORCE_PDN_DISCONNECT = 
                                  PS_IFACE_IOCTL_3GPP_FORCE_PDN_DISCONNECT,


  /*-------------------------------------------------------------------------

                              BCMCS IOCTLS

    The following operations are specific to UMTS interface.Start from 400.
  -------------------------------------------------------------------------*/
  /* Update the BCMCS database table */
  DSS_IFACE_IOCTL_BCMCS_DB_UPDATE         = PS_IFACE_IOCTL_BCMCS_DB_UPDATE,

  /* BCMCS BOM caching */
  DSS_IFACE_IOCTL_BCMCS_BOM_CACHING_SETUP  =
                                     PS_IFACE_IOCTL_BCMCS_BOM_CACHING_SETUP,
  /* BCMCS handoff optimization for registration */
  DSS_IFACE_IOCTL_BCMCS_ENABLE_HANDOFF_REG  =
                                     PS_IFACE_IOCTL_BCMCS_ENABLE_HANDOFF_REG,

  /*-------------------------------------------------------------------------

                              UW FMC IOCTLS

    The following operations are specific to UW FMC interface
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_UW_FMC_SET_TUNNEL_PARAMS  =
                                     PS_IFACE_IOCTL_UW_FMC_SET_TUNNEL_PARAMS,
  DSS_IFACE_IOCTL_UW_FMC_RESET_TUNNEL_PARAMS  =
                                     PS_IFACE_IOCTL_UW_FMC_RESET_TUNNEL_PARAMS,

  /*-------------------------------------------------------------------------
                             RESERVED IOCTLs
    The following IOCTLs are reserved and should NOT be used by external
    applications.
  -------------------------------------------------------------------------*/

  /* Get TX RLP queue info - Reserved IOCTL*/
  DSS_IFACE_IOCTL_GET_TX_QUEUE_LEVEL      = PS_FLOW_IOCTL_GET_TX_QUEUE_LEVEL,

  /* Min reserved value */
  DSS_IFACE_IOCTL_RESERVED_MIN            = PS_IFACE_IOCTL_RESERVED_MIN,
  /* Max reserved value */
  DSS_IFACE_IOCTL_RESERVED_MAX            = PS_IFACE_IOCTL_RESERVED_MAX,

  /*-------------------------------------------------------------------------

                              MORE COMMON STUFF

  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_MAX,
  DSS_IFACE_IOCTL_NAME_FORCE_32_BIT       = 0x7FFFFFFF /* Force 32bit enum */
} dss_iface_ioctl_type;

/*---------------------------------------------------------------------------

                                COMMON DATA STRUCTURES

              The following data structs are for common ioctls.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Definition of iface events.
---------------------------------------------------------------------------*/
#define DSS_IFACE_IOCTL_DOWN_EV_VERS (1)
#define DSS_IFACE_IOCTL_UP_EV_VERS  (1)
#define DSS_IFACE_IOCTL_COMING_UP_EV_VERS (1)
#define DSS_IFACE_IOCTL_GOING_DOWN_EV_VERS (1)
#define DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV_VERS (1)
#define DSS_IFACE_IOCTL_PHYS_LINK_UP_EV_VERS (1)
#define DSS_IFACE_IOCTL_PHYS_LINK_COMING_UP_EV_VERS (1)
#define DSS_IFACE_IOCTL_PHYS_LINK_GOING_DOWN_EV_VERS (1)
#define DSS_IFACE_IOCTL_ADDR_CHANGED_EV_VERS (1)

typedef enum
{
  DSS_IFACE_IOCTL_MIN_EV,
  DSS_IFACE_IOCTL_REG_EVENT_MIN,
  DSS_IFACE_IOCTL_DOWN_EV,
  DSS_IFACE_IOCTL_UP_EV,
  DSS_IFACE_IOCTL_COMING_UP_EV,
  DSS_IFACE_IOCTL_GOING_DOWN_EV,
  DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV,
  DSS_IFACE_IOCTL_PHYS_LINK_UP_EV,
  DSS_IFACE_IOCTL_PHYS_LINK_COMING_UP_EV,
  DSS_IFACE_IOCTL_PHYS_LINK_GOING_DOWN_EV,
  DSS_IFACE_IOCTL_ADDR_CHANGED_EV,
  DSS_IFACE_IOCTL_BEARER_TECH_CHANGED_EV,
  DSS_IFACE_IOCTL_EXTENDED_IP_CONFIG_EV,
  DSS_IFACE_IOCTL_QOS_AWARE_SYSTEM_EV,
  DSS_IFACE_IOCTL_QOS_UNAWARE_SYSTEM_EV,
  DSS_IFACE_IOCTL_RF_CONDITIONS_CHANGED_EV,
  DSS_IFACE_IOCTL_QOS_ADDED_ON_IFACE_EV,
  DSS_IFACE_IOCTL_QOS_DELETED_ON_IFACE_EV,
  DSS_IFACE_IOCTL_OUTAGE_NOTIFICATION_EV,
  DSS_IFACE_IOCTL_IFACE_AVAILABLE_EV,
  DSS_IFACE_IOCTL_FAST_DORMANCY_STATUS_EV,
  DSS_IFACE_IOCTL_707_NETWORK_SUPPORTED_QOS_PROFILES_CHANGED_EV,
  DSS_IFACE_IOCTL_HANDOFF_INIT_EV,
  DSS_IFACE_IOCTL_HANDOFF_STATUS_SUCCESS_EV,
  DSS_IFACE_IOCTL_HANDOFF_STATUS_FAILURE_EV,

  /*-------------------------------------------------------------------------
    Following events are required for knowing the outcome of trying to set
    various parameters required for setting RATE inertia on HDR.
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA_SUCCESS_EV,
  DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA_FAILURE_EV,

  /*-------------------------------------------------------------------------
    Following events are required for knowing the outcome of trying to set
    various parameters required for setting the slot cycle index on HDR.
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV,
  DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV,
  DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SESSION_CHANGED_EV,

  /*-------------------------------------------------------------------------
    This event is used to notify apps about IPV6 prefix updates.
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_PREFIX_UPDATE_EV,
  /**<
      This event is used to notify apps about bearer technology change.
  */
  DSS_IFACE_IOCTL_BEARER_TECH_EX_CHANGED_EV,

  /*-------------------------------------------------------------------------
    The following events are QOS related and are explicitly registered by
    apps
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_EXPLICIT_QOS_REG_EVENT_MIN,
  DSS_IFACE_IOCTL_PRIMARY_QOS_MODIFY_RESULT_EV =
                                  DSS_IFACE_IOCTL_EXPLICIT_QOS_REG_EVENT_MIN,
  DSS_IFACE_IOCTL_EXPLICIT_QOS_REG_EVENT_MAX,

  DSS_IFACE_IOCTL_REG_EVENT_MAX = DSS_IFACE_IOCTL_EXPLICIT_QOS_REG_EVENT_MAX,

  /*-------------------------------------------------------------------------
    The following events cannot be explicitly registered for.
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_NONREG_EVENT_MIN       =  DSS_IFACE_IOCTL_REG_EVENT_MAX,
  DSS_IFACE_IOCTL_MT_REQUEST_EV          =  DSS_IFACE_IOCTL_NONREG_EVENT_MIN,
  DSS_IFACE_IOCTL_NONREG_EVENT_MAX,

  /*-------------------------------------------------------------------------
    The following events are QOS related and are not allowed for explicit
    registration
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_QOS_EVENT_MIN,
  DSS_IFACE_IOCTL_QOS_AVAILABLE_EV,
  DSS_IFACE_IOCTL_QOS_AVAILABLE_MODIFIED_EV,
  DSS_IFACE_IOCTL_QOS_AVAILABLE_DEACTIVATED_EV,
  DSS_IFACE_IOCTL_QOS_UNAVAILABLE_EV,
  DSS_IFACE_IOCTL_QOS_MODIFY_ACCEPTED_EV,
  DSS_IFACE_IOCTL_QOS_MODIFY_REJECTED_EV,
  DSS_IFACE_IOCTL_QOS_INFO_CODE_UPDATED_EV,
  DSS_IFACE_IOCTL_QOS_EVENT_MAX,
  DSS_IFACE_IOCTL_QOS_GROUP_EVENT = DSS_IFACE_IOCTL_QOS_EVENT_MAX,
  DSS_IFACE_IOCTL_QOS_NET_INITIATED_AVAILABLE_EV,

  DSS_IFACE_IOCTL_MCAST_EVENT_MIN,
  DSS_IFACE_IOCTL_MCAST_REGISTER_SUCCESS_EV,
  DSS_IFACE_IOCTL_MCAST_REGISTER_FAILURE_EV,
  DSS_IFACE_IOCTL_MCAST_DEREGISTERED_EV,
  DSS_IFACE_IOCTL_MCAST_STATUS_EV,
  DSS_IFACE_IOCTL_MCAST_EVENT_MAX,


/*-------------------------------------------------------------------------
    The following events are MBMS  related and are not allowed for explicit
    registration
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_MBMS_CONTEXT_EVENT_MIN,
  DSS_IFACE_IOCTL_MBMS_CONTEXT_ACT_SUCCESS_EV,
  DSS_IFACE_IOCTL_MBMS_CONTEXT_ACT_FAILURE_EV,
  DSS_IFACE_IOCTL_MBMS_CONTEXT_DEACT_SUCCESS_EV,
  DSS_IFACE_IOCTL_MBMS_CONTEXT_DEACT_FAILURE_EV,
  DSS_IFACE_IOCTL_MBMS_CONTEXT_EVENT_MAX,


  /*-------------------------------------------------------------------------
    These events are used to notify apps about IPV6 privacy addresses.
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_MIN,
  DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_GENERATED_EV,
  DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DEPRECATED_EV,
  DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DELETED_EV,
  DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_MAX,

  /*-------------------------------------------------------------------------
    These events are used to notify apps about IPV6 external addresses.
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_IPV6_EXTERNAL_ADDR_DELETED_EV,

  /*-------------------------------------------------------------------------
    This event is used to notify the application of configuration changes.
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_EXTENDED_IP_CONFIG_EX_EV,

  /*-------------------------------------------------------------------------
    This event is used to notify the application of APN Param changed event.
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_APN_PARAM_CHANGE_EV,

  /*-------------------------------------------------------------------------
    Define events for other features, not allowed for explicit registration
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_EVENT_MAX,
  DSS_IFACE_IOCTL_EVENT_FORCE_32_BIT = 0x7FFFFFFF /* Force 32bit enum type */
} dss_iface_ioctl_event_enum_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_IPV4_ADDR parameter type
  - Iface IPV4 address
---------------------------------------------------------------------------*/
typedef ip_addr_type dss_iface_ioctl_ipv4_addr_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_IPV6_ADDR parameter type
  - Iface IPV6 address
---------------------------------------------------------------------------*/
typedef ip_addr_type dss_iface_ioctl_ipv6_addr_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR parameter type
  - Iface IPV4 Primary DNS address
---------------------------------------------------------------------------*/
typedef ip_addr_type dss_iface_ioctl_ipv4_prim_dns_addr_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_IPV6_PRIM_DNS_ADDR parameter type
  - Iface IPV4 Primary DNS address
---------------------------------------------------------------------------*/
typedef ip_addr_type dss_iface_ioctl_ipv6_prim_dns_addr_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR parameter type
  - Iface IPV4 Primary DNS address
---------------------------------------------------------------------------*/
typedef ip_addr_type dss_iface_ioctl_ipv4_seco_dns_addr_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_IPV6_SECO_DNS_ADDR parameter type
  - Iface IPV4 Primary DNS address
---------------------------------------------------------------------------*/
typedef ip_addr_type dss_iface_ioctl_ipv6_seco_dns_addr_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_ALL_DNS_ADDRS parameter type
  - Retrieve both primary and secondary DNS addresses of an iface
---------------------------------------------------------------------------*/
typedef struct
{
  /* This is an input/output param that specifies the # of addresses
     requested by the app/as well as the acutal # of addresses returned.   */
  uint8         num_dns_addrs;

  /* OUTPUT param */
  ip_addr_type *dns_addrs_ptr;  /* Ptr to client's allocated ip_addr array */
} dss_iface_ioctl_get_all_dns_addrs_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_MTU parameter type
  - Iface MTU
---------------------------------------------------------------------------*/
typedef uint32 dss_iface_ioctl_mtu_type;

/*---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

typedef ps_iface_ioctl_extended_ip_config_ex_type dss_iface_ioctl_extended_ip_config_ex_type;

/*---------------------------------------------------------------------------
 DSS_IFACE_IOCTL_APN_PARAM_CHANGE_EV parameter type
  -Retrieve the data specifying the changed apn parameters.
---------------------------------------------------------------------------*/
typedef ps_iface_changed_apn_param_info_type
  dss_iface_ioctl_changed_apn_param_info_type;


/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_IP_ADDR parameter type
  - Iface IP addr
---------------------------------------------------------------------------*/
typedef ip_addr_type dss_iface_ioctl_get_ip_addr_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_STATE parameter type
  - Iface State
---------------------------------------------------------------------------*/
typedef ps_iface_state_enum_type dss_iface_ioctl_state_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_PHYS_LINK_STATE parameter type
  - Phys link State
---------------------------------------------------------------------------*/
typedef phys_link_state_type dss_iface_ioctl_phys_link_state_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_REG_EVENT_CB/DSS_IFACE_IOCTL_DEREG_EVENT_CB parameter type
  - registering/deregistering for event callbacks
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_DOWN_EV and DSS_IFACE_IOCTL_UP_EV parameter type
  - ps iface state
---------------------------------------------------------------------------*/
typedef ps_iface_state_enum_type dss_iface_ioctl_event_info_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV and DSS_IFACE_IOCTL_PHYS_LINK_UP_EV
  parameter type
  - physical link state
---------------------------------------------------------------------------*/
typedef phys_link_state_type dss_iface_ioctl_phys_link_event_info_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_ADDR_CHANGED_EV parameter type
  - IP address
---------------------------------------------------------------------------*/
typedef ip_addr_type dss_iface_ioctl_addr_change_event_info_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_IPCP_DNS_OPT parameter type
---------------------------------------------------------------------------*/
typedef boolean dss_iface_ioctl_ipcp_dns_opt_type;

/*---------------------------------------------------------------------------
  Mobile Terminated handle type.
---------------------------------------------------------------------------*/
typedef ps_iface_mt_handle_type dss_iface_ioctl_mt_handle_type;

/*---------------------------------------------------------------------------
  Mulicast handle type
---------------------------------------------------------------------------*/
typedef ps_iface_mcast_handle_type dss_iface_ioctl_mcast_handle_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY parameter type
  - Provides the current service mode.
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_bearer_tech_type dss_iface_ioctl_bearer_tech_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY_EX and 
   DSS_IFACE_IOCTL_BEARER_TECH_EX_CHANGED_EV parameter type
  - Provides the current service mode.
---------------------------------------------------------------------------*/
typedef ds_sys_system_status_info_type dss_iface_ioctl_bearer_tech_ex_type;

/*---------------------------------------------------------------------------
  Argument to be passed with 
  PS_IFACE_IOCTL_GET_OP_RES_PCO
---------------------------------------------------------------------------*/
typedef ps_iface_op_reserved_pco_type
  dss_iface_op_reserved_pco_type;

/*---------------------------------------------------------------------------
  Argument to be passed with 
  PS_IFACE_IOCTL_SET_DATA_PATH/PS_IFACE_IOCTL_GET_DATA_PATH
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_data_path_bridge_mode_type
  dss_iface_ioctl_data_path_bridge_mode_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_KEEP_ALIVE
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_keep_alive_type dss_iface_ioctl_keep_alive_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_GET_MIP_INFO
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_mip_ma_info_type dss_iface_ioctl_mip_ma_info_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_ON_QOS_AWARE_SYSTEM parameter type
  - Identifies whether the mobile is currently on a QOS aware system.
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_on_qos_aware_system_type
  dss_iface_ioctl_on_qos_aware_system_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_BCMCS_DB_UPDATE parameter type
  - Updates the BCMCS table with the multicast ip, port, and flow id.
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_bcmcs_db_update_type
  dss_iface_ioctl_bcmcs_db_update_type;

/*---------------------------------------------------------------------------
  Serving mode change information.
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_bearer_tech_changed_type
  dss_iface_ioctl_bearer_tech_changed_type;

/*---------------------------------------------------------------------------
  Extended IP config information.
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_extended_ip_config_type
  dss_iface_ioctl_extended_ip_config_type;

/*---------------------------------------------------------------------------
  Data type used for getting DSS_IFACE_IOCTL_GET_RF_CONDITIONS
---------------------------------------------------------------------------*/
typedef ps_iface_rf_conditions_info_type dss_iface_ioctl_rf_conditions_type;

/*---------------------------------------------------------------------------
  Data type used to add static nat entry.
  DSS_IFACE_IOCTL_ADD_STATIC_NAT_ENTRY
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_add_static_nat_entry_type
  dss_iface_ioctl_add_static_nat_entry_type;

/*---------------------------------------------------------------------------
  Data type used to get static nat entry.
  DSS_IFACE_IOCTL_GET_STATIC_NAT_ENTRY
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_get_static_nat_entry_type
  dss_iface_ioctl_get_static_nat_entry_type;

/*---------------------------------------------------------------------------
  Data type used to delete static nat entry.
  DSS_IFACE_IOCTL_DELETE_STATIC_NAT_ENTRY
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_delete_static_nat_entry_type
  dss_iface_ioctl_delete_static_nat_entry_type;

/*---------------------------------------------------------------------------
  Data type used to get dynamic nat entry timeout.
  DSS_IFACE_IOCTL_GET_DYNAMIC_NAT_ENTRY_TIMEOUT
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_get_dynamic_nat_entry_timeout_type
  dss_iface_ioctl_get_dynamic_nat_entry_timeout_type;

/*---------------------------------------------------------------------------
  Data type used to set dynamic nat entry timeout.
  DSS_IFACE_IOCTL_SET_DYNAMIC_NAT_ENTRY_TIMEOUT
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_set_dynamic_nat_entry_timeout_type
  dss_iface_ioctl_set_dynamic_nat_entry_timeout_type;

/*---------------------------------------------------------------------------
  Data type used to get/set NAT IPSEC VPN PassThrough.
  DSS_IFACE_IOCTL_GET_NAT_IPSEC_VPN_PASS_THROUGH
  DSS_IFACE_IOCTL_SET_NAT_IPSEC_VPN_PASS_THROUGH
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_nat_ipsec_vpn_pass_through_type
  dss_iface_ioctl_nat_ipsec_vpn_pass_through_type;

/*---------------------------------------------------------------------------
  Data type used to get/set NAT L2TP VPN PassThrough.
  DSS_IFACE_IOCTL_GET_NAT_L2TP_VPN_PASS_THROUGH
  DSS_IFACE_IOCTL_SET_NAT_L2TP_VPN_PASS_THROUGH
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_nat_l2tp_vpn_pass_through_type
  dss_iface_ioctl_nat_l2tp_vpn_pass_through_type;

/*---------------------------------------------------------------------------
  Data type used to get/set NAT PPTP VPN PassThrough.
  DSS_IFACE_IOCTL_GET_NAT_PPTP_VPN_PASS_THROUGH
  DSS_IFACE_IOCTL_SET_NAT_PPTP_VPN_PASS_THROUGH
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_nat_pptp_vpn_pass_through_type
  dss_iface_ioctl_nat_pptp_vpn_pass_through_type;

/*---------------------------------------------------------------------------
  Data type used to get NAT public IP address
  DSS_IFACE_IOCTL_GET_NAT_PUBLIC_IP_ADDR
---------------------------------------------------------------------------*/
typedef ip_addr_type dss_iface_ioctl_nat_public_ip_addr_type;

/*---------------------------------------------------------------------------
  Data type used to add/get a DeMitlitarized zone.
  DSS_IFACE_IOCTL_ADD_DMZ
  DSS_IFACE_IOCTL_GET_DMZ
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_dmz_type dss_iface_ioctl_dmz_type;

/*---------------------------------------------------------------------------
  Data type used to delete a DeMitlitarized zone.
  DSS_IFACE_IOCTL_DELETE_DMZ
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_delete_dmz_type dss_iface_ioctl_delete_dmz_type;

/*---------------------------------------------------------------------------
  Argument to be passed with 
  PS_IFACE_IOCTL_SET_PDN_TEARDOWN_CONFIG/PS_IFACE_IOCTL_GET_PDN_TEARDOWN_CONFIG
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_pdn_teardown_config_type
  dss_iface_ioctl_pdn_teardown_config_type;

/*---------------------------------------------------------------------------
  Data type used to enable or disable ALG clients:
  DSS_IFACE_IOCTL_ENABLE_ALG
  DSS_IFACE_IOCTL_DISABLE_ALG
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_alg_client_e
  dss_iface_ioctl_alg_client_e;
typedef ps_iface_ioctl_enable_alg_type
  dss_iface_ioctl_enable_alg_type;
typedef ps_iface_ioctl_disable_alg_type
  dss_iface_ioctl_disable_alg_type;

/*---------------------------------------------------------------------------
  Data type used to retrieve ALG status list. 
  DSS_IFACE_IOCTL_GET_ALG_STATUS
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_get_alg_status_type
  dss_iface_ioctl_get_alg_status_type;

/*---------------------------------------------------------------------------
  Iface HW Address Type for DSS_IFACE_IOCTL_GET_HW_ADDR
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_hw_addr_type dss_iface_ioctl_hw_addr_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_SIP_SERV_ADDRS paramameter type
    List of SIP Server Addresses.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32            count;
  ip_addr_type     *addr_array;
} dss_iface_ioctl_sip_serv_addr_info_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_SIP_SERV_DOMAIN_NAMES and
  DSS_IFACE_IOCTL_GET_DOMAIN_NAME_SEARCH_LIST paramameter type
    List of SIP Server or DNS Domain Names.
---------------------------------------------------------------------------*/

#define DSS_IFACE_MAX_DOMAIN_NAME_SIZE  PS_IFACE_MAX_DOMAIN_NAME_SIZE

typedef ps_iface_domain_name_type   dss_iface_ioctl_domain_name_type;

/**
  @brief Argument to be passed with DSS_IFACE_IOCTL_GET_DEVICE_NAME.
*/
typedef ps_iface_ioctl_device_name_type dss_iface_ioctl_device_name_type;

/**
  @brief Argument to be passed with DSS_IFACE_IOCTL_GET_GATEWAY_ADDR.
*/
typedef ps_iface_ioctl_gateway_addr_type dss_iface_ioctl_gateway_addr_type;

/**
  @brief Argument to be passed with DSS_IFACE_IOCTL_GET_SUBNET_MASK.
*/
typedef ps_iface_ioctl_subnet_mask_type dss_iface_ioctl_subnet_mask_type;

typedef struct
{
  uint32                             count;
  dss_iface_ioctl_domain_name_type  *name_array;
} dss_iface_ioctl_sip_serv_domain_name_info_type;

typedef struct
{
  uint32                              count;
  dss_iface_ioctl_domain_name_type   *name_array;
} dss_iface_ioctl_domain_name_search_list_type;

/*---------------------------------------------------------------------------
  Get all supported qos profiles.
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_get_network_supported_qos_profiles_type
  dss_iface_ioctl_get_network_supported_qos_profiles_type;

typedef enum
{
  DSS_IFACE_IOCTL_EIC_NOT_SPECIFIED      = PS_EIC_NOT_SPECIFIED,

  DSS_IFACE_IOCTL_EIC_QOS_INTERNAL_MIN   = PS_EIC_QOS_INTERNAL_MIN,
  DSS_IFACE_IOCTL_EIC_QOS_INVALID_PARAMS = PS_EIC_QOS_INTERNAL_INVALID_PARAMS,
  DSS_IFACE_IOCTL_EIC_QOS_INTERNAL_INVALID_PARAMS =
    PS_EIC_QOS_INTERNAL_INVALID_PARAMS,
  DSS_IFACE_IOCTL_EIC_QOS_INTERNAL_CALL_ENDED = PS_EIC_QOS_INTERNAL_CALL_ENDED,
  DSS_IFACE_IOCTL_EIC_QOS_INTERNAL_ERROR      = PS_EIC_QOS_INTERNAL_ERROR,
  DSS_IFACE_IOCTL_EIC_QOS_INSUFFICIENT_LOCAL_RESOURCES =
    PS_EIC_QOS_INSUFFICIENT_LOCAL_RESOURCES,
  DSS_IFACE_IOCTL_EIC_QOS_TIMED_OUT_OPERATION =
    PS_EIC_QOS_TIMED_OUT_OPERATION,
  DSS_IFACE_IOCTL_EIC_QOS_INTERNAL_UNKNOWN_CAUSE_CODE =
    PS_EIC_QOS_INTERNAL_UNKNOWN_CAUSE_CODE,
  DSS_IFACE_IOCTL_EIC_QOS_INTERNAL_MODIFY_IN_PROGRESS =
    PS_EIC_QOS_INTERNAL_MODIFY_IN_PROGRESS,
  DSS_IFACE_IOCTL_EIC_QOS_INTERNAL_MAX = PS_EIC_QOS_INTERNAL_MAX,

  DSS_IFACE_IOCTL_EIC_QOS_NETWORK_MIN    = PS_EIC_QOS_NETWORK_MIN,
  DSS_IFACE_IOCTL_EIC_QOS_NOT_SUPPORTED  = PS_EIC_QOS_NOT_SUPPORTED,
  DSS_IFACE_IOCTL_EIC_QOS_NOT_AVAILABLE  = PS_EIC_QOS_NOT_AVAILABLE,
  DSS_IFACE_IOCTL_EIC_QOS_NOT_GUARANTEED = PS_EIC_QOS_NOT_GUARANTEED,
  DSS_IFACE_IOCTL_EIC_QOS_INSUFFICIENT_NET_RESOURCES =
                               PS_EIC_QOS_INSUFFICIENT_NET_RESOURCES,
  DSS_IFACE_IOCTL_EIC_QOS_AWARE_SYSTEM   = PS_EIC_QOS_AWARE_SYSTEM,
  DSS_IFACE_IOCTL_EIC_QOS_UNAWARE_SYSTEM = PS_EIC_QOS_UNAWARE_SYSTEM,
  DSS_IFACE_IOCTL_EIC_QOS_REJECTED_OPERATION =
                                       PS_EIC_QOS_REJECTED_OPERATION,
  DSS_IFACE_IOCTL_EIC_QOS_WILL_GRANT_WHEN_QOS_RESUMED =
                              PS_EIC_QOS_WILL_GRANT_WHEN_QOS_RESUMED,
  DSS_IFACE_IOCTL_EIC_QOS_NETWORK_CALL_ENDED = PS_EIC_QOS_NETWORK_CALL_ENDED,
  DSS_IFACE_IOCTL_EIC_QOS_NETWORK_SVC_NOT_AVAILABLE =
    PS_EIC_QOS_NETWORK_SVC_NOT_AVAILABLE,
  DSS_IFACE_IOCTL_EIC_QOS_NETWORK_L2_LINK_RELEASED =
    PS_EIC_QOS_NETWORK_L2_LINK_RELEASED,
  DSS_IFACE_IOCTL_EIC_QOS_NETWORK_L2_LINK_REESTAB_REJ =
    PS_EIC_QOS_NETWORK_L2_LINK_REESTAB_REJ,
  DSS_IFACE_IOCTL_EIC_QOS_NETWORK_L2_LINK_REESTAB_IND =
    PS_EIC_QOS_NETWORK_L2_LINK_REESTAB_IND,
  DSS_IFACE_IOCTL_EIC_QOS_NETWORK_UNKNOWN_CAUSE_CODE =
    PS_EIC_QOS_NETWORK_UNKNOWN_CAUSE_CODE,
  DSS_IFACE_IOCTL_EIC_QOS_NETWORK_DISJOINT_PROFILE_SET_SUGGESTED =
    PS_EIC_QOS_NETWORK_DISJOINT_PROFILE_SET_SUGGESTED,
  DSS_IFACE_IOCTL_EIC_QOS_NETWORK_NULL_PROFILE_SUGGESTED =
    PS_EIC_QOS_NETWORK_NULL_PROFILE_SUGGESTED,
  DSS_IFACE_IOCTL_EIC_QOS_NETWORK_UE_NOT_AUTHORIZED =
    PS_EIC_QOS_NETWORK_UE_NOT_AUTHORIZED,
  DSS_IFACE_IOCTL_EIC_QOS_NETWORK_MAX = PS_EIC_QOS_NETWORK_MAX

} dss_iface_ioctl_extended_info_code_enum_type;

typedef enum
{
  DSS_IFACE_IOCTL_EIC_NETWORK_MIN           = PS_EIC_NETWORK_MIN,
  DSS_IFACE_IOCTL_EIC_NETWORK_NOT_SPECIFIED = PS_EIC_NETWORK_NOT_SPECIFIED,
  DSS_IFACE_IOCTL_EIC_NETWORK_BUSY          = PS_EIC_NETWORK_BUSY,
  DSS_IFACE_IOCTL_EIC_CLIENT_END            = PS_EIC_CLIENT_END,
  DSS_IFACE_IOCTL_EIC_NO_SRV                = PS_EIC_NO_SRV,
  DSS_IFACE_IOCTL_EIC_CDMA_LOCK             = PS_EIC_CDMA_LOCK,
  DSS_IFACE_IOCTL_EIC_FADE                  = PS_EIC_FADE,
  DSS_IFACE_IOCTL_EIC_INTERCEPT             = PS_EIC_INTERCEPT,
  DSS_IFACE_IOCTL_EIC_REORDER               = PS_EIC_REORDER,
  DSS_IFACE_IOCTL_EIC_REL_NORMAL            = PS_EIC_REL_NORMAL,
  DSS_IFACE_IOCTL_EIC_REL_SO_REJ            = PS_EIC_REL_SO_REJ,
  DSS_IFACE_IOCTL_EIC_INCOM_CALL            = PS_EIC_INCOM_CALL,
  DSS_IFACE_IOCTL_EIC_ALERT_STOP            = PS_EIC_ALERT_STOP,
  DSS_IFACE_IOCTL_EIC_ACTIVATION            = PS_EIC_ACTIVATION,
  DSS_IFACE_IOCTL_EIC_MC_ABORT              = PS_EIC_MC_ABORT,
  DSS_IFACE_IOCTL_EIC_MAX_ACCESS_PROBE      = PS_EIC_MAX_ACCESS_PROBE,
  DSS_IFACE_IOCTL_EIC_PSIST_NG              = PS_EIC_PSIST_NG,
  DSS_IFACE_IOCTL_EIC_UIM_NOT_PRESENT       = PS_EIC_UIM_NOT_PRESENT,
  DSS_IFACE_IOCTL_EIC_ACC_IN_PROG           = PS_EIC_ACC_IN_PROG,
  DSS_IFACE_IOCTL_EIC_ACC_FAIL              = PS_EIC_ACC_FAIL,
  DSS_IFACE_IOCTL_EIC_RETRY_ORDER           = PS_EIC_RETRY_ORDER,
  DSS_IFACE_IOCTL_EIC_CCS_NOT_SUPPORTED_BY_BS = 
    PS_EIC_CCS_NOT_SUPPORTED_BY_BS,
  DSS_IFACE_IOCTL_EIC_NO_RESPONSE_FROM_BS   = PS_EIC_NO_RESPONSE_FROM_BS,
  DSS_IFACE_IOCTL_EIC_REJECTED_BY_BS        = PS_EIC_REJECTED_BY_BS,
  DSS_IFACE_IOCTL_EIC_INCOMPATIBLE          = PS_EIC_INCOMPATIBLE,
  DSS_IFACE_IOCTL_EIC_ACCESS_BLOCK          = PS_EIC_ACCESS_BLOCK,
  DSS_IFACE_IOCTL_EIC_ALREADY_IN_TC         = PS_EIC_ALREADY_IN_TC,
  DSS_IFACE_IOCTL_EIC_USER_CALL_ORIG_DURING_GPS = 
    PS_EIC_USER_CALL_ORIG_DURING_GPS,
  DSS_IFACE_IOCTL_EIC_USER_CALL_ORIG_DURING_SMS = 
    PS_EIC_USER_CALL_ORIG_DURING_SMS,
  DSS_IFACE_IOCTL_EIC_USER_CALL_ORIG_DURING_VOICE_CALL = 
    PS_EIC_USER_CALL_ORIG_DURING_VOICE_CALL,
  DSS_IFACE_IOCTL_EIC_REDIR_OR_HANDOFF      = PS_EIC_REDIR_OR_HANDOFF,
  DSS_IFACE_IOCTL_EIC_ACCESS_BLOCK_ALL      = PS_EIC_ACCESS_BLOCK_ALL,
  DSS_IFACE_IOCTL_EIC_IS707B_MAX_ACC        = PS_EIC_IS707B_MAX_ACC,
  DSS_IFACE_IOCTL_EIC_NO_CDMA_SRV           = PS_EIC_NO_CDMA_SRV,
  DSS_IFACE_IOCTL_EIC_CD_GEN_OR_BUSY        = PS_EIC_CD_GEN_OR_BUSY,
  DSS_IFACE_IOCTL_EIC_CD_BILL_OR_AUTH       = PS_EIC_CD_BILL_OR_AUTH,
  DSS_IFACE_IOCTL_EIC_CHG_HDR = PS_EIC_CHG_HDR,
  DSS_IFACE_IOCTL_EIC_EXIT_HDR = PS_EIC_EXIT_HDR,
  DSS_IFACE_IOCTL_EIC_HDR_NO_SESSION = PS_EIC_HDR_NO_SESSION,
  DSS_IFACE_IOCTL_EIC_COLLOC_ACQ_FAIL = PS_EIC_COLLOC_ACQ_FAIL,
  DSS_IFACE_IOCTL_EIC_HDR_ORIG_DURING_GPS_FIX = PS_EIC_HDR_ORIG_DURING_GPS_FIX,
  DSS_IFACE_IOCTL_EIC_HDR_CS_TIMEOUT = PS_EIC_HDR_CS_TIMEOUT,
  DSS_IFACE_IOCTL_EIC_HDR_RELEASED_BY_CM = PS_EIC_HDR_RELEASED_BY_CM,
  DSS_IFACE_IOCTL_EIC_OTASP_COMMIT_IN_PROG = PS_EIC_OTASP_COMMIT_IN_PROG,
  DSS_IFACE_IOCTL_EIC_NO_HYBR_HDR_SRV = PS_EIC_NO_HYBR_HDR_SRV,
  DSS_IFACE_IOCTL_EIC_HDR_NO_LOCK_GRANTED = PS_EIC_HDR_NO_LOCK_GRANTED,
  DSS_IFACE_IOCTL_EIC_HOLD_OTHER_IN_PROG = PS_EIC_HOLD_OTHER_IN_PROG,
  DSS_IFACE_IOCTL_EIC_HDR_FADE = PS_EIC_HDR_FADE,
  DSS_IFACE_IOCTL_EIC_HDR_ACC_FAIL = PS_EIC_HDR_ACC_FAIL,
  DSS_IFACE_IOCTL_EIC_THERMAL_EMERGENCY = PS_EIC_THERMAL_EMERGENCY,
  DSS_IFACE_IOCTL_EIC_CALL_ORIG_THROTTLED = PS_EIC_CALL_ORIG_THROTTLED,
  DSS_IFACE_IOCTL_EIC_OFFLINE = PS_EIC_OFFLINE,
  DSS_IFACE_IOCTL_EIC_EMERGENCY_MODE = PS_EIC_EMERGENCY_MODE,
  DSS_IFACE_IOCTL_EIC_PHONE_IN_USE = PS_EIC_PHONE_IN_USE,
  DSS_IFACE_IOCTL_EIC_INVALID_MODE = PS_EIC_INVALID_MODE,
  DSS_IFACE_IOCTL_EIC_INVALID_SIM_STATE = PS_EIC_INVALID_SIM_STATE,
  DSS_IFACE_IOCTL_EIC_NO_COLLOC_HDR = PS_EIC_NO_COLLOC_HDR,
  DSS_IFACE_IOCTL_EIC_CALL_CONTROL_REJECTED = PS_EIC_CALL_CONTROL_REJECTED,
  DSS_IFACE_IOCTL_EIC_NETWORK_MAX           = PS_EIC_NETWORK_MAX
} dss_iface_ioctl_dormancy_info_code_enum_type;

typedef enum
{
  DSS_IFACE_IOCTL_QOS_REQUEST_OP   = PS_IFACE_IOCTL_QOS_REQUEST_OP,
  DSS_IFACE_IOCTL_QOS_CONFIGURE_OP = PS_IFACE_IOCTL_QOS_CONFIGURE_OP
} dss_iface_ioctl_qos_request_ex_opcode_enum_type;

typedef enum
{
  DSS_IFACE_IOCTL_MCAST_IC_NOT_SPECIFIED =
    PS_IFACE_MCAST_IC_NOT_SPECIFIED,

  /* BCMCS info codes - start*/

  DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_CANCELLED =
    PS_IFACE_MCAST_BCMCS_FLOW_STATUS_CANCELLED,

  DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_UNABLE_TO_MONITOR =
    PS_IFACE_MCAST_BCMCS_FLOW_STATUS_UNABLE_TO_MONITOR,

  DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_REQUESTED =
    PS_IFACE_MCAST_BCMCS_FLOW_STATUS_REQUESTED,

  DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_TIMEOUT =
    PS_IFACE_MCAST_BCMCS_FLOW_STATUS_TIMEOUT,

  DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_LOST =
    PS_IFACE_MCAST_BCMCS_FLOW_STATUS_LOST,

  DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE =
    PS_IFACE_MCAST_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE,

  DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_AVAILABLE =
    PS_IFACE_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_AVAILABLE,

  DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_TRANSMITTED =
    PS_IFACE_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_TRANSMITTED,

  DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_INVALID_AUTH_SIG =
    PS_IFACE_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_INVALID_AUTH_SIG,

  DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_UNAVAILABLE =
    PS_IFACE_MCAST_BCMCS_FLOW_STATUS_UNAVAILABLE,

  DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_NO_MAPPING =
    PS_IFACE_MCAST_BCMCS_FLOW_STATUS_NO_MAPPING,

  DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP =
    PS_IFACE_MCAST_BCMCS_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP,

  DSS_IFACE_IOCTL_MCAST_BCMCS_MAX_FLOWS_REACHED =
    PS_IFACE_MCAST_BCMCS_MAX_FLOWS_REACHED,

  DSS_IFACE_IOCTL_MCAST_BCMCS_MAX_DEPRECATED_INFO_CODE =
    PS_IFACE_MCAST_BCMCS_MAX_DEPRECATED_INFO_CODE, /* For backward compatibility */

  /* -----------------------------------------------------------------------------
   BCMCS 2.0: Info codes:
   All BCMCS info codes less than
   DSS_IFACE_IOCTL_MCAST_BCMCS_MAX_DEPRECATED_INFO_CODE are tagged as
   "deprecated" and re-written as new info codes for BCMCS 2.0 support below, except
   DSS_IFACE_IOCTL_MCAST_IC_NOT_SPECIFIED.
  ------------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_CANCELLED =
    PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_CANCELLED,

  DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_UNABLE_TO_MONITOR =
    PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_UNABLE_TO_MONITOR,

  DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_TIMEOUT =
    PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_TIMEOUT,

  DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_LOST =
    PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_LOST,

  DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_SYS_UNAVAILABLE =
    PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_SYS_UNAVAILABLE,

  DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_AN_REJECT_NOT_AVAILABLE =
    PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_AN_REJECT_NOT_AVAILABLE,

  DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_AN_REJECT_NOT_TRANSMITTED =
    PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_AN_REJECT_NOT_TRANSMITTED,

  DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_AN_REJECT_INVALID_AUTH_SIG =
    PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_AN_REJECT_INVALID_AUTH_SIG,

  DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_UNAVAILABLE =
    PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_UNAVAILABLE,

  DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_NO_MAPPING =
    PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_NO_MAPPING,

  DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP =
    PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP,

  DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_REQUESTED =
    PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_REQUESTED,

  DSS_IFACE_IOCTL_MCAST_BCMCS2p0_MAX_FLOWS_REACHED =
    PS_IFACE_MCAST_BCMCS2p0_MAX_FLOWS_REACHED,

  /* New Info codes: BCMCS 2.0 */
  DSS_IFACE_IOCTL_MCAST_BCMCS_JOIN_REQ_IN_PROGRESS =
    PS_IFACE_MCAST_BCMCS_JOIN_REQ_IN_PROGRESS,
    /* JOIN request successfully received */

  DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_REQUEST_SENT =
    PS_IFACE_MCAST_BCMCS_FLOW_REQUEST_SENT,
    /* Success from DS (FlowRequest command successfully sent to DO-CP) */

  DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_MAX_MONITORED_FLOWS =
    PS_IFACE_MCAST_BCMCS_FLOW_STATUS_MAX_MONITORED_FLOWS,
    /* We've reached the max number of flows that can be MONITORED */

  DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_MONITORED =
      PS_IFACE_MCAST_BCMCS_FLOW_STATUS_MONITORED,
      /* Success - Flow is monitored (DESIRED_MONITORED) -
         Flow is active and included in broadcast overhead msg */

  DSS_IFACE_IOCTL_MCAST_BCMCS_REGISTRATION_SUCCESS =
    PS_IFACE_MCAST_BCMCS_REGISTRATION_SUCCESS,
    /* Registration Request was successful */

  DSS_IFACE_IOCTL_MCAST_BCMCS_REGISTRATION_NOT_ALLOWED =
    PS_IFACE_MCAST_BCMCS_REGISTRATION_NOT_ALLOWED,
    /* Registration is not allowed at this time,
       Please try again later */

  DSS_IFACE_IOCTL_MCAST_BCMCS_REGISTRATION_FAILED =
    PS_IFACE_MCAST_BCMCS_REGISTRATION_FAILED,
    /* Lower layers attempted to send the registration but
       the message did not go through */

  DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_DEREGISTERED =
    PS_IFACE_MCAST_BCMCS_FLOW_DEREGISTERED,
    /* Flow deregistered */
  /* BCMCS info codes - end */


  /* MediaFLO info codes */
  DSS_IFACE_IOCTL_MCAST_FLO_IP_OR_PORT_NOT_SUPPORTED =
    PS_IFACE_MCAST_FLO_IP_OR_PORT_NOT_SUPPORTED,
  DSS_IFACE_IOCTL_MCAST_FLO_NO_AUTHORIZATION =
    PS_IFACE_MCAST_FLO_NO_AUTHORIZATION,
  DSS_IFACE_IOCTL_MCAST_FLO_NO_SYSTEM_COVERAGE =
    PS_IFACE_MCAST_FLO_NO_SYSTEM_COVERAGE,
  DSS_IFACE_IOCTL_MCAST_FLO_MAX_FLOW_REACHED =
    PS_IFACE_MCAST_FLO_MAX_FLOW_REACHED,

  /* DVB-H info codes */
  DSS_IFACE_IOCTL_MCAST_DVBH_IP_OR_PORT_NOT_FOUND =
    PS_IFACE_MCAST_DVBH_IP_OR_PORT_NOT_FOUND,
  DSS_IFACE_IOCTL_MCAST_DVBH_SYSTEM_UNAVAILABLE =
    PS_IFACE_MCAST_DVBH_SYSTEM_UNAVAILABLE,
  DSS_IFACE_IOCTL_MCAST_DVBH_BAD_REQUEST =
    PS_IFACE_MCAST_DVBH_BAD_REQUEST,
  DSS_IFACE_IOCTL_MCAST_DVBH_REQUEST_CONFLICT =
    PS_IFACE_MCAST_DVBH_REQUEST_CONFLICT,
  DSS_IFACE_IOCTL_MCAST_DVBH_DUP_REQUEST =
    PS_IFACE_MCAST_DVBH_DUP_REQUEST,
  DSS_IFACE_IOCTL_MCAST_DVBH_MAX_FLOWS_REACHED =
  PS_IFACE_MCAST_DVBH_MAX_FLOWS_REACHED,

  /* MBMS info code */
  DSS_IFACE_IOCTL_MCAST_MBMS_SYSTEM_UNAVAILABLE =
  PS_IFACE_MCAST_MBMS_SYSTEM_UNAVAILABLE

  /* Any additional info code */
} dss_iface_ioctl_mcast_info_code_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_HDR_MODE_FAILURE_REASON_CODE_ENUM_TYPE

DESCRIPTION
  Enum type for HDR mode failure
---------------------------------------------------------------------------*/
typedef enum
{

  DSS_IFACE_IOCTL_HDR_REV0_RATE_INERTIA_REQUEST_REJECTED =
    PS_HDR_REV0_RATE_INERTIA_REQUEST_REJECTED,
  DSS_IFACE_IOCTL_HDR_REV0_RATE_INERTIA_REQUEST_FAILED_TX =
    PS_HDR_REV0_RATE_INERTIA_REQUEST_FAILED_TX,
  DSS_IFACE_IOCTL_HDR_REV0_RATE_INERTIA_NOT_SUPPORTED =
    PS_HDR_REV0_RATE_INERTIA_NOT_SUPPORTED,
  DSS_IFACE_IOCTL_HDR_REV0_RATE_INERTIA_NO_NET =
    PS_HDR_REV0_RATE_INERTIA_NO_NET,
  DSS_IFACE_IOCTL_HDR_REV0_RATE_INERTIA_FAILURE_REASON_MAX =
    PS_HDR_REV0_RATE_INERTIA_FAILURE_REASON_MAX

} dss_iface_ioctl_hdr_rev0_rate_inertia_failure_code_enum_type;

/*---------------------------------------------------------------------------
  TYPEDEF PS_IFACE_HDR_MODE_FAILURE_REASON_CODE_ENUM_TYPE

  DESCRIPTION
  Enum type for HDR mode failure
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_IFACE_IOCTL_HDR_SLOTTED_MODE_REQUEST_REJECTED =
    PS_HDR_SLOTTED_MODE_REQUEST_REJECTED,
  DSS_IFACE_IOCTL_HDR_SLOTTED_MODE_REQUEST_FAILED_TX =
    PS_HDR_SLOTTED_MODE_REQUEST_FAILED_TX,
  DSS_IFACE_IOCTL_HDR_SLOTTED_MODE_NOT_SUPPORTED =
    PS_HDR_SLOTTED_MODE_NOT_SUPPORTED,
  DSS_IFACE_IOCTL_HDR_SLOTTED_MODE_NO_NET =
    PS_HDR_SLOTTED_MODE_NO_NET,
  DSS_IFACE_IOCTL_HDR_SLOTTED_MODE_FAILURE_REASON_MAX =
    PS_HDR_SLOTTED_MODE_FAILURE_REASON_MAX
} dss_iface_ioctl_hdr_slotted_mode_failure_code_enum_type;

typedef enum
{
  DSS_IFACE_IOCTL_PREFIX_ADDED      = PREFIX_ADDED,
  DSS_IFACE_IOCTL_PREFIX_REMOVED    = PREFIX_REMOVED,
  DSS_IFACE_IOCTL_PREFIX_DEPRECATED = PREFIX_DEPRECATED,
  DSS_IFACE_IOCTL_PREFIX_UPDATED    = PREFIX_UPDATED
} dss_iface_ioctl_prefix_update_enum_type;

typedef struct
{
  struct ps_in6_addr prefix;
  dss_iface_ioctl_prefix_update_enum_type kind;
  uint8 prefix_len;
} dss_iface_ioctl_prefix_update_info_type;

typedef struct
{
  ip_addr_type ip_addr;
  boolean      is_unique;
} dss_iface_ioctl_priv_addr_info_type;

typedef struct
{
  dss_iface_ioctl_mcast_handle_type             handle;
  dss_iface_ioctl_mcast_info_code_enum_type     info_code;
  /* control for clearing event_buf_ptr in dssocki_invoke_mcast_cbacks() */
  boolean                                       force_dereg_cbacks;
} dss_iface_ioctl_mcast_event_info_type;

typedef struct
{
  dss_qos_handle_type                           handle;
  dss_iface_ioctl_extended_info_code_enum_type  info_code;
} dss_iface_ioctl_qos_event_info_type;

/**
  @brief Handle to network initiated qos request.
*/
typedef ps_iface_net_initiated_qos_req_handle_type
  dss_qos_net_initiated_req_handle_type;

/**
  @brief Event info for DSS_IFACE_IOCTL_QOS_NET_INITIATED_AVAILABLE_EV
*/
typedef struct
{
  dss_qos_handle_type                    qos_handle;
  /**< Handle to QOS instance */
  dss_qos_net_initiated_req_handle_type  handle;
  /**< Handle to Network Initiated QOS Request */
} dss_iface_ioctl_qos_net_initiated_available_event_info_type;

typedef enum
{
  DSS_FAST_DORMANCY_STATUS_SUCCESS       = 1,
  DSS_FAST_DORMANCY_STATUS_FAILURE_RETRY = 2
} dss_iface_ioctl_fast_dormancy_status_enum_type;

typedef struct
{
  dss_iface_ioctl_fast_dormancy_status_enum_type  dorm_status;
} dss_iface_ioctl_fast_dormancy_status_event_type;

typedef struct
{
  boolean  is_modify_succeeded;
} dss_iface_ioctl_primary_qos_modify_result_type;

typedef ps_iface_outage_notification_event_info_type
  dss_iface_ioctl_outage_notification_event_info_type;

typedef struct
{
   dss_iface_ioctl_hdr_slotted_mode_failure_code_enum_type
                                               hdr_slotted_mode_failure_code;
   uint8                                       sm_current_sci;
} dss_iface_ioctl_slotted_mode_info_type;

#define   DSS_MAX_DHCP_SIP_IP_LIST       3
#define   DSS_MAX_DHCP_SIP_DOMAIN_LIST   3
#define   DSS_MAX_DHCP_DNS_IP_LIST       3
#define   DSS_MAX_DHCP_DNS_DOMAIN_LIST   3

typedef struct
{
  uint32  options_mask;
#define    DSS_DHCP_SIP_SERVER_ADDRS_OPT        ( 0x01 )
#define    DSS_DHCP_SIP_SERVER_DOMAIN_LIST_OPT  ( 0x02 )
#define    DSS_DHCP_DNS_SERVER_ADDRS_OPT        ( 0x04 )
#define    DSS_DHCP_DNS_SERVER_DOMAIN_LIST_OPT  ( 0x08 )
} dss_iface_ioctl_dhcp_info_options_mask;

typedef struct
{
  dss_iface_ioctl_dhcp_info_options_mask            dhcp_options;

  dss_iface_ioctl_sip_serv_addr_info_type           sip_serv_addrs;
  dss_iface_ioctl_sip_serv_domain_name_info_type    sip_domain_list;
  dss_iface_ioctl_get_all_dns_addrs_type            dns_addrs;
  dss_iface_ioctl_domain_name_search_list_type      dns_domain_list;
} dss_iface_ioctl_dhcp_config_info_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_DHCP_ARP_CACHE_UPDATE parameter type
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_dhcp_arp_cache_update_type
  dss_iface_ioctl_dhcp_arp_cache_update_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_DHCP_ARP_CACHE_CLEAR parameter type
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_dhcp_arp_cache_clear_type
  dss_iface_ioctl_dhcp_arp_cache_clear_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_DHCP_SERVER_GET_DEVICE_INFO parameter type
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_dhcp_get_device_info_type
  dss_iface_ioctl_dhcp_get_device_info_type;
  
/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_IPV6_EXTERNAL_ADDR_DELETED_EV parameter type
---------------------------------------------------------------------------*/
typedef ip_addr_type
  dss_iface_ioctl_ipv6_external_addr_deleted_event_info_type;

typedef enum
{
  /* 3GPP RAT Values */
  DSS_IFACE_IOCTL_SYS_RAT_EX_3GPP_WCDMA    = DS_SYS_RAT_EX_3GPP_WCDMA,
  DSS_IFACE_IOCTL_SYS_RAT_EX_3GPP_GERAN    = DS_SYS_RAT_EX_3GPP_GERAN,
  DSS_IFACE_IOCTL_SYS_RAT_EX_3GPP_LTE      = DS_SYS_RAT_EX_3GPP_LTE,
  DSS_IFACE_IOCTL_SYS_RAT_EX_3GPP_TDSCCDMA = DS_SYS_RAT_EX_3GPP_TDSCDMA,
  DSS_IFACE_IOCTL_SYS_RAT_EX_3GPP_WLAN     = DS_SYS_RAT_EX_3GPP_WLAN,

  /* 3GPP2 RAT Values */
  DSS_IFACE_IOCTL_SYS_RAT_EX_3GPP2_1X      = DS_SYS_RAT_EX_3GPP2_1X,
  DSS_IFACE_IOCTL_SYS_RAT_EX_3GPP2_HRPD    = DS_SYS_RAT_EX_3GPP2_HRPD,
  DSS_IFACE_IOCTL_SYS_RAT_EX_3GPP2_EHRPD   = DS_SYS_RAT_EX_3GPP2_EHRPD,
  DSS_IFACE_IOCTL_SYS_RAT_EX_3GPP2_WLAN    = DS_SYS_RAT_EX_3GPP2_WLAN, 

  /* WLAN RAT Values */
  DSS_IFACE_IOCTL_SYS_RAT_EX_WLAN          = DS_SYS_RAT_EX_WLAN
}dss_iface_ioctl_handoff_enum_type;

typedef ps_iface_net_down_reason_type dss_iface_ioctl_handoff_failure_reason_type;

typedef struct
{
  dss_iface_ioctl_handoff_enum_type  srat;
  dss_iface_ioctl_handoff_enum_type  trat;
  dss_iface_ioctl_handoff_failure_reason_type  failure_reason;
} dss_iface_ioctl_handoff_info_type;


typedef union
{
  dss_iface_ioctl_event_info_type              iface_state_info;
  dss_iface_ioctl_phys_link_event_info_type    phys_link_state_info;
  dss_iface_ioctl_addr_change_event_info_type  addr_change_info;
  dss_iface_ioctl_qos_event_info_type          qos_info;
  dss_iface_ioctl_mt_handle_type               mt_handle;
  dss_iface_ioctl_mcast_event_info_type        mcast_info;
  dss_iface_ioctl_bearer_tech_changed_type     tech_change_info;
  dss_iface_ioctl_bearer_tech_ex_type          bearer_tech_ex_info;
  dss_iface_ioctl_extended_ip_config_type      extended_ip_config_info;
  dss_iface_ioctl_extended_info_code_enum_type qos_aware_info_code;
  dss_iface_ioctl_rf_conditions_type           rf_conditions_change_info;
  dss_iface_ioctl_primary_qos_modify_result_type
                                            primary_qos_modify_result_info;
  dss_iface_ioctl_outage_notification_event_info_type outage_notification_info;
  dss_iface_ioctl_prefix_update_info_type      prefix_info;
  dss_iface_ioctl_priv_addr_info_type          priv_ipv6_addr;

  dss_iface_ioctl_hdr_rev0_rate_inertia_failure_code_enum_type
                                            hdr_rev0_rate_inertia_failure_code;
  dss_iface_ioctl_slotted_mode_info_type       slotted_mode_info;
  dss_iface_ioctl_fast_dormancy_status_event_type fast_dorm_status;
  dss_iface_ioctl_qos_net_initiated_available_event_info_type
                                              qos_net_initiated_available_info;
  dss_iface_ioctl_ipv6_external_addr_deleted_event_info_type
                                               ip6_ext_addr_deleted_info; 
  dss_iface_ioctl_extended_ip_config_ex_type   extended_ip_config_ex;

  dss_iface_ioctl_handoff_info_type            handoff_info;
  dss_iface_ioctl_changed_apn_param_info_type  changed_apn_param_info;
} dss_iface_ioctl_event_info_union_type;

/*---------------------------------------------------------------------------
  Data type used to return all the v6 prefixes of an interface.
  (DSS_IFACE_IOCTL_GET_ALL_V6_PREFIXES)
---------------------------------------------------------------------------*/
typedef ps_iface_ipv6_addr_state_enum_type
  dss_iface_ioctl_ipv6_addr_state_enum_type;

typedef struct
{
  struct ps_in6_addr                         prefix;
  dss_iface_ioctl_ipv6_addr_state_enum_type  prefix_state;
  uint8                                      prefix_len;
} dss_iface_ioctl_prefix_info_type;

typedef struct
{
  /* OUTPUT param */
  dss_iface_ioctl_prefix_info_type *prefix_info_ptr; /* Ptr to client prefix array */

  /* This is an input/output param that specifies the # of addresses
   for which space was allocated by the application and the acutal # of
   addresses  returned.                                                    */
   uint8         num_prefixes;

} dss_iface_ioctl_get_all_v6_prefixes_type;

/*---------------------------------------------------------------------------
  Typedef for event callback function
  (DSS_IFACE_IOCTL_REG_EVENT_CB/ DSS_IFACE_IOCTL_DEREG_EVENT_CB)
---------------------------------------------------------------------------*/
typedef void (*dss_iface_ioctl_event_cb)
(
  dss_iface_ioctl_event_enum_type          event,
  dss_iface_ioctl_event_info_union_type    event_info,
  void                                     *user_data,
  sint15                                   dss_nethandle,
  dss_iface_id_type                        iface_id
);

/*---------------------------------------------------------------------------
  Data type used to return if iface is in laptop call
  (DSS_IFACE_IOCTL_IS_LAPTOP_CALL_ACTIVE)
---------------------------------------------------------------------------*/
typedef boolean  dss_iface_ioctl_is_laptop_call_active_type;

/*---------------------------------------------------------------------------
  Data type used for specifying the QOS parameters when app requests for QOS
  (DSS_IFACE_IOCTL_QOS_REQUEST)
---------------------------------------------------------------------------*/
typedef struct
{
  sint15                    app_id;     /* Client's App Id                 */
#define dss_nethandle       app_id
  qos_spec_type             qos;        /* QOS specification               */
  dss_iface_ioctl_event_cb  cback_fn;   /* QOS event notification callback */
  void                     *user_data;  /* User data passed to callback    */

  /* OUTPUT Parameter */
  dss_qos_handle_type       handle;     /* Handle to QOS instance, Use
                                           for all subsequent IOCTLs on
                                           this QOS instance               */
} dss_iface_ioctl_qos_request_type;

/*---------------------------------------------------------------------------
  Data type used for specifying the QOS parameters when app requests QOS or
  to configure QOS
  (DSS_IFACE_IOCTL_QOS_REQUEST_EX)
---------------------------------------------------------------------------*/
typedef struct
{
  sint15                     app_id;                  /* Client's App Id   */
#define dss_nethandle       app_id
  dss_iface_ioctl_qos_request_ex_opcode_enum_type opcode;
                                                      /* Request type      */
  uint8                      num_qos_specs;           /* Num QOS requests  */
  uint8                      qos_control_flags;       /* Extra QOS options -
                                                      not used currently   */
  qos_spec_type            * qos_specs_ptr;          /* QOS specifications */
  dss_iface_ioctl_event_cb   cback_fn;  /* QOS event notification callback */
  void                     * user_data; /* User data passed to callback    */

  /* OUTPUT Parameter */
  dss_qos_handle_type      * handles_ptr; /* Handles to the QOS instances,
                                             use for all subsequent IOCTLs
                                             on these QOS instances        */
} dss_iface_ioctl_qos_request_ex_type;

/*---------------------------------------------------------------------------
  Data type used for suspending/resuming and releasing multiple QOS instances
  (DSS_IFACE_IOCTL_QOS_RELEASE_EX)
  (DSS_IFACE_IOCTL_QOS_SUSPEND_EX)
  (DSS_IFACE_IOCTL_QOS_RESUME_EX)
---------------------------------------------------------------------------*/
typedef struct dss_iface_ioctl_qos_release_s
{
  uint8                  num_handles;               /* Num flows passed    */
  dss_qos_handle_type  * handles_ptr;
} dss_iface_ioctl_qos_release_ex_type;

typedef struct dss_iface_ioctl_qos_release_s
  dss_iface_ioctl_qos_suspend_ex_type;
typedef struct dss_iface_ioctl_qos_release_s
  dss_iface_ioctl_qos_resume_ex_type;

/*---------------------------------------------------------------------------
  Data type used for modifying the QOS parameters
  (DSS_IFACE_IOCTL_QOS_MODIFY)
---------------------------------------------------------------------------*/
typedef struct
{
  dss_qos_handle_type                   handle;  /* Handle to QOS instance */
  qos_spec_type                         qos;     /* QOS specification      */
} dss_iface_ioctl_qos_modify_type;

/*---------------------------------------------------------------------------
  Data type used for releasing the QOS parameters
  (DSS_IFACE_IOCTL_QOS_RELEASE)
---------------------------------------------------------------------------*/
typedef struct
{
  dss_qos_handle_type                   handle;  /* Handle to QOS instance */
} dss_iface_ioctl_qos_release_type;

/*---------------------------------------------------------------------------
  Data type used for suspending the QOS parameters
  (DSS_IFACE_IOCTL_QOS_SUSPEND)
---------------------------------------------------------------------------*/
typedef struct
{
  dss_qos_handle_type                   handle;  /* Handle to QOS instance */
} dss_iface_ioctl_qos_suspend_type;

/*---------------------------------------------------------------------------
  Data type used for resuming the QOS parameters
  (DSS_IFACE_IOCTL_QOS_RESUME)
---------------------------------------------------------------------------*/
typedef struct
{
  dss_qos_handle_type                   handle;  /* Handle to QOS instance */
} dss_iface_ioctl_qos_resume_type;

/*---------------------------------------------------------------------------
  Data type used for retrieving the granted QOS flow spec on secondary
  (DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC)
---------------------------------------------------------------------------*/
typedef struct
{
  dss_qos_handle_type                   handle;  /* Handle to QOS instance */

  /* OUTPUT Parameters */
  ip_flow_type      rx_flow;                     /* Rx Flow spec           */
  ip_flow_type      tx_flow;                     /* Tx Flow spec           */
} dss_iface_ioctl_qos_get_flow_spec_type;

/*---------------------------------------------------------------------------
  Data type used for modifying QoS on primary
  (DSS_IFACE_IOCTL_PRIMARY_QOS_MODIFY)
---------------------------------------------------------------------------*/
typedef struct
{
  int16                      dss_nethandle;     /* Client’s net handle      */
  primary_qos_spec_type      primary_qos_spec; /* Newly Requested QoS      */
  dss_iface_ioctl_event_cb   cback_fn;         /* Event notification cback */
  void                     * user_data_ptr;    /* User data passed in cback*/
} dss_iface_ioctl_primary_qos_modify_type;

/*---------------------------------------------------------------------------
  Data type used for retrieving the granted QOS flow spec on primary
  (DSS_IFACE_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC)
---------------------------------------------------------------------------*/
typedef ps_flow_ioctl_primary_qos_get_granted_flow_spec_type
  dss_iface_ioctl_primary_qos_get_granted_flow_spec_type;

typedef enum
{
  QOS_STATE_INVALID = 0x00,
  QOS_UNAVAILABLE   = 0x01,
  QOS_ACTIVATING    = 0x02,
  QOS_AVAILABLE     = 0x04,
  QOS_SUSPENDING    = 0x08,
  QOS_DEACTIVATED   = 0x10,
  QOS_RELEASING     = 0x20,
  QOS_CONFIGURING   = 0x40
} dss_iface_ioctl_qos_status_type;

/*---------------------------------------------------------------------------
  Data type used for getting QOS status
  (DSS_IFACE_IOCTL_QOS_GET_STATUS)
---------------------------------------------------------------------------*/
typedef struct
{
  dss_qos_handle_type              handle;       /* Handle to QOS instance */

  /* OUTPUT param */
  dss_iface_ioctl_qos_status_type  qos_status;   /* status of QOS instance */
} dss_iface_ioctl_qos_get_status_type;

/*---------------------------------------------------------------------------
  Data type used for retrieving the QOS flow spec
  (DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2)
---------------------------------------------------------------------------*/
typedef struct
{
  dss_qos_handle_type              handle;       /* Handle to QOS instance */

  /* OUTPUT Parameters */
  ip_flow_type                     rx_flow;      /* Rx Flow spec           */
  ip_flow_type                     tx_flow;      /* Tx Flow spec           */
  dss_iface_ioctl_qos_status_type  qos_status;   /* status of QOS instance */
} dss_iface_ioctl_qos_get_granted_flow_spec2_type;

/**
  @brief Data type used for retrieving the QOS filter spec

  @see DSS_IFACE_IOCTL_QOS_GET_GRANTED_FILTER_SPEC
*/
typedef struct
{
  dss_qos_handle_type             handle;
  /**< Handle to QOS instance */
  ip_filter_spec_type             rx_filter_spec;
  /**< [out] Rx Filter spec. App should allocate MAX_FLTR_PER_REQ
             for ip_filter_spec_type list_ptr and specify 
             num_filters accordingly.                                      */
  ip_filter_spec_type             tx_filter_spec;
  /**< [out] Tx Filter spec. App should allocate MAX_FLTR_PER_REQ
             for ip_filter_spec_type list_ptr and specify 
             num_filters accordingly.                                      */
  dss_iface_ioctl_qos_status_type qos_status;
  /**< [out] status of QOS instance */
} dss_iface_ioctl_qos_get_granted_filter_spec_type;

/*---------------------------------------------------------------------------
  Data type used for retrieving the QoS control mode
  (DSS_IFACE_IOCTL_QOS_GET_MODE)
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_qos_mode_type dss_iface_ioctl_qos_mode_type;

/**
  @brief Data type for specifying the parameters for
         DSS_IFACE_IOCTL_QOS_NET_INITIATED_REQUEST

  @see DSS_IFACE_IOCTL_QOS_NET_INITIATED_REQUEST
*/
typedef struct
{
  int16                                        dss_nethandle;
  /**< Client's net handle */
  net_initiated_qos_spec_type                  net_initiated_qos_spec;
  /**< QOS specification */
  dss_iface_ioctl_event_cb                     cback_fn;
  /**< Event callback to be called on availability of matching
       Network Initiated QoS */
  void                                       * user_data;
  /**< User data passed to callback */
  dss_qos_net_initiated_req_handle_type        handle;
  /**< [out] Handle to this request. it can be used in subsequent
             operations on this request */
  dss_qos_handle_type                          qos_handle;
  /**< [out] QoS handle to this request. it can be used in subsequent
             operations on this request */
} dss_iface_ioctl_qos_net_initiated_request_type;

/**
  @brief Data type for specifying the parameters for
         DSS_IFACE_IOCTL_QOS_NET_INITIATED_RELEASE

  @see DSS_IFACE_IOCTL_QOS_NET_INITIATED_RELEASE
*/
typedef struct
{
  dss_qos_net_initiated_req_handle_type  handle;
  /**< Handle, which application had obtained obtained from
       DSS_IFACE_IOCTL_QOS_NET_INITIATED_REQUEST */
} dss_iface_ioctl_qos_net_initiated_release_type;

/*---------------------------------------------------------------------------
  Data type used for getting current and max iface Tx queue size
---------------------------------------------------------------------------*/
typedef struct ps_flow_ioctl_tx_queue_level_s
  dss_iface_ioctl_tx_queue_level_type;

/*---------------------------------------------------------------------------
  Register callback through dss_iface_ioctl(DSS_IFACE_IOCTL_MT_REG_CB)
---------------------------------------------------------------------------*/
typedef struct
{
  sint15                                app_id;
#define dss_nethandle       app_id
  dss_iface_ioctl_event_cb              event_cb;
  void                                  *user_data_ptr;
  dss_iface_ioctl_mt_handle_type        *handle;
} dss_iface_ioctl_mt_reg_cb_type;

/*---------------------------------------------------------------------------
  De-register callback through dss_iface_ioctl(DSS_IFACE_IOCTL_MT_DEREG_CB)
---------------------------------------------------------------------------*/
typedef struct ps_iface_ioctl_mt_dereg_cb_s dss_iface_ioctl_mt_dereg_cb_type;

/*---------------------------------------------------------------------------
  Join a Multicast group address and port
---------------------------------------------------------------------------*/
typedef struct
{
  ip_addr_type               ip_addr;          /* Multicast IP address     */
  uint16                     port;             /* Multicast Port           */
  void                       *mcast_param_ptr; /* Multicast input params   */

  dss_iface_ioctl_event_cb   event_cb;         /* Callback for mcast ev    */
  void                       *user_data_ptr;   /* App specific data,
                                                  passed in cback          */
  sint15                     app_id;           /* dss_nethandle            */
#define dss_nethandle       app_id

  /* Output Paramters */
  uint32                     handle;           /* Multicast handle         */
} dss_iface_ioctl_mcast_join_type;

/*---------------------------------------------------------------------------
  Leave a Multicast group using handle assigned by the mode handler
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                          handle;
  sint15                          app_id;
#define dss_nethandle       app_id
} dss_iface_ioctl_mcast_leave_type;

/* BCMCS 2.0 Support */
/*---------------------------------------------------------------------------
  Maximum number of flows requested in a MCAST_JOIN_EX ioctl call
----------------------------------------------------------------------------*/
#define DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL PS_IFACE_MAX_MCAST_FLOWS

/*---------------------------------------------------------------------------
  Multicast request flags for register/configure request per flow
----------------------------------------------------------------------------*/
typedef enum
{
  /* Flow registration is turned on */
  DSS_IFACE_IOCTL_MCAST_REG_SETUP_ALLOWED =
                                 PS_IFACE_IOCTL_MCAST_REG_SETUP_ALLOWED,
  /* Flow registration is turned off */
  DSS_IFACE_IOCTL_MCAST_REG_SETUP_NOT_ALLOWED =
                                 PS_IFACE_IOCTL_MCAST_REG_SETUP_NOT_ALLOWED
} dss_iface_ioctl_mcast_join_ex_req_flags_enum_type;

/*---------------------------------------------------------------------------
  Join a number of Multicast group addresses and ports
  IOCTL: DSS_IFACE_IOCTL_MCAST_JOIN_EX

  Fields are:
   For each flow:
    ip_addr                 :     Multicast IP address
    port                    :     Multicast port
    mcast_param_ptr         :     Multicase input parameters
    mcast_request_flags     :     Configure/Register request
    handle                  :     Multicast handle
   num_flows                :     Number of flow join requests
   user_data_ptr            :     App specific data, passed back in cback
   event_cb                 :     Callback for mcast ev
   dss_nethandle            :     DSS Net handle (app_id)
---------------------------------------------------------------------------*/
typedef struct
{
  ip_addr_type               ip_addr[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
  uint16                     port[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
  void                       *mcast_param_ptr[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
  uint32                     num_flows;
  void                       *user_data_ptr;
  int16                      dss_nethandle;
  dss_iface_ioctl_mcast_join_ex_req_flags_enum_type
                             mcast_request_flags[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
  dss_iface_ioctl_event_cb   event_cb;
  uint32                     handle[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
}dss_iface_ioctl_mcast_join_ex_type;

/*---------------------------------------------------------------------------
  Leave a number of Multicast groups using handles assigned by the mode handler
  IOCTL: DSS_IFACE_IOCTL_MCAST_LEAVE_EX

  Fields are:
    num_flows               :     Number of flow leave requests
    handle                  :     Multicast handle for each flow request
    dss_nethandle:          :     DSS Net handle (app_id)
----------------------------------------------------------------------------*/
typedef struct
{
  uint8                      num_flows;
  uint32                     handle[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
  int16                      dss_nethandle;
} dss_iface_ioctl_mcast_leave_ex_type;

/*---------------------------------------------------------------------------
  Trigger registration of multiple multicast flows using assigned handles
  IOCTL: DSS_IFACE_IOCTL_MCAST_REGISTER_EX

  Fields are:
    num_flows                :    Number of flow register requests
    handle                   :    Multicast handle for each flow request
    dss_nethandle:           :    DSS Net handle (app_id)
---------------------------------------------------------------------------*/
typedef struct
{
  uint8                      num_flows;
  uint32                     handle[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
  int16                      dss_nethandle;
} dss_iface_ioctl_mcast_register_ex_type;

/*---------------------------------------------------------------------------
  Structure to store Mulicast IP address and port
    ip_addr                 :     Multicast IP address
    port                    :     Multicast port
---------------------------------------------------------------------------*/
typedef struct
{
  ip_addr_type               ip_addr;
  uint16                     port;
}dss_iface_ioctl_mcast_addr_info_type;

/*---------------------------------------------------------------------------
  Enable handoff optimizations while registration of multiple multicast flows
  using assigned handles
  DSS_IFACE_IOCTL_BCMCS_ENABLE_HANDOFF_REG

  Fields are:
    mcast_addr_info          :    Multicast IP address and port
    num_mcast_addr           :    Number of Multicast addresses requested
    dss_nethandle            :    DSS Net handle (app_id)
---------------------------------------------------------------------------*/
typedef struct
{
  dss_iface_ioctl_mcast_addr_info_type  mcast_addr_info[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
  uint8                                 num_mcast_addr;
} dss_iface_ioctl_bcmcs_enable_handoff_reg_type;

typedef ps_iface_ioctl_bcmcs_bom_caching_setup_type
  dss_iface_ioctl_bcmcs_bom_caching_setup_type;

typedef  ps_iface_ioctl_mbms_mcast_param_type dss_iface_ioctl_mbms_mcast_param_type;


/*---------------------------------------------------------------------------
  Typedef for struct used with DSS_IFACE_IOCTL_REG_EVENT_CB and
  DSS_IFACE_IOCTL_DEREG_EVENT_CB. Note that the application must specify a
  valid APP_ID when registering/deregistering for event callbacks.
---------------------------------------------------------------------------*/
typedef struct
{
  dss_iface_ioctl_event_cb        event_cb;
  dss_iface_ioctl_event_enum_type event;
  void                            *user_data_ptr;
  sint15                          app_id;
#define dss_nethandle       app_id
} dss_iface_ioctl_ev_cb_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_ALL_IFACES parameter type
  -  (Note for this IOCTL, a NULL id_ptr has to be passed as a parameter).
---------------------------------------------------------------------------*/
typedef struct
{
  unsigned int      number_of_ifaces;
  dss_iface_id_type ifaces[MAX_SYSTEM_IFACES];
} dss_iface_ioctl_all_ifaces_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_IP_FAMILY parameter type
  - Iface IP family
---------------------------------------------------------------------------*/
typedef ps_iface_addr_family_type  dss_iface_ioctl_ip_family_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_IFACE_NAME parameter type
  - Iface name
---------------------------------------------------------------------------*/
typedef ps_iface_name_enum_type    dss_iface_ioctl_iface_name_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_DATA_BEARER_RATE parameter type to get the current
  mode of the iface.
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_data_bearer_rate dss_iface_ioctl_data_bearer_rate;

/*---------------------------------------------------------------------------
TYPEDEF DSS_IFACE_IOCTL_IPV6_IID_PARAMS_TYPE

DESCRIPTION
  Parameters for generating specific types of privacy addresses.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean is_unique;              /* Request a unique address or shareable */
} dss_iface_ioctl_ipv6_iid_params_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GENERATE_PRIV_IPV6_ADDR parameter type
---------------------------------------------------------------------------*/
typedef struct
{
  sint15                               dss_nethandle;
  dss_iface_ioctl_ipv6_iid_params_type iid_params;
  dss_iface_ioctl_event_cb             event_cb;         /* event callback */
  void                                *user_data_ptr;/* User data to cback */

  /* OUTPUT PARAMETER */
  ip_addr_type                         *ip_addr;
} dss_iface_ioctl_priv_ipv6_addr_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GO_ACTIVE,
  DSS_IFACE_IOCTL_GO_DORMANT,
  DSS_IFACE_IOCTL_GO_NULL parameter types
  - (Note that apps do not have to define an arg
  of this type. They can just pass in Null).
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_RELEASE_REASON_MIN = 1,

  DSS_RELEASE_REASON_NONE = DSS_RELEASE_REASON_MIN,
  DSS_RELEASE_REASON_LOW_BATTERY = 2,

  DSS_RELEASE_REASON_MAX = 2
} dss_release_reason_enum_type;

typedef struct
{
  dss_release_reason_enum_type release_reason_value;
} dss_iface_ioctl_null_arg_type;

/*---------------------------------------------------------------------------
  Enumerated Type used for selecting Session Timer Value to be modified.
---------------------------------------------------------------------------*/

typedef enum
{
  DSS_SESSION_TIMER_DO=1,
  DSS_SESSION_TIMER_1X=2,
  DSS_SESSION_TIMER_1X_AND_DO
} dss_session_timer_select_enum_type;
/*---------------------------------------------------------------------------
  Structure passed into get/set session timer IOCTL's to interact with
  Session timeout values.
---------------------------------------------------------------------------*/
typedef struct
{
  dss_session_timer_select_enum_type timer_select;
  int16 timer_val;
} dss_session_timer_type;

/*---------------------------------------------------------------------------
Enumerated Type used for data path IOCTL/Events
---------------------------------------------------------------------------*/
typedef ps_iface_data_path_bridge_mode_enum_type dss_iface_data_path_enum_type;

/*---------------------------------------------------------------------------
                                707 DATA STRUCTURES

    The following data structs are specific to 707 (1x) interface.
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Data type used for 707 QCMDR value
  (DSS_IFACE_IOCTL_707_GET/SET_MDR)
---------------------------------------------------------------------------*/
typedef unsigned int dss_iface_ioctl_707_mdr_type;

/*---------------------------------------------------------------------------
  Data type used for 707 dorm timer value
  (DSS_IFACE_IOCTL_707_GET/SET_DORM_TIMER)
---------------------------------------------------------------------------*/
typedef unsigned int dss_iface_ioctl_707_dorm_timer_type;

/*---------------------------------------------------------------------------
  Data type used for 707 inactivity timer value
  (DSS_IFACE_IOCTL_707_GET/SET_INACTIVITY_TIMER)
---------------------------------------------------------------------------*/
typedef ps_flow_ioctl_707_inactivity_timer_type
  dss_iface_ioctl_707_inactivity_timer_type;

/*---------------------------------------------------------------------------
  Data type used for 707 RLP3 NAK policies (current, default, negotiated)
  (DSS_IFACE_IOCTL_707_GET/SET_RLP_ALL_CURR_NAK)
  (DSS_IFACE_IOCTL_707_GET/SET_RLP_ALL_DEF_NAK)
  (DSS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK)
  IOCTL_707_MAX_NAK_ROUNDS should be same as DSRLP_MAX_NAK_ROUNDS. Duplicate
  definition to avoid exposing dsrlp.h  by including here.
---------------------------------------------------------------------------*/
#define IOCTL_707_MAX_NAK_ROUNDS 3

typedef struct
{
  byte rscb_index;
  byte nak_rounds_fwd;
  byte naks_per_round_fwd[IOCTL_707_MAX_NAK_ROUNDS];
  byte nak_rounds_rev;
  byte naks_per_round_rev[IOCTL_707_MAX_NAK_ROUNDS];
} dss_iface_ioctl_707_rlp_opt_type;

/*---------------------------------------------------------------------------
  Data type used for 707 QOS non-assured priority (DSS_IFACE_IOCTL_GET_STATE)
  (DSS_IFACE_IOCTL_707_GET_RLP_QOS_NA_PRI)
---------------------------------------------------------------------------*/
typedef byte dss_iface_ioctl_707_qos_na_pri_type;

/*--------------------------------------------------------------------------
  Data type used for 707 SDB support query.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 flags;       /* Same flag as used in socket write to specify SDB
                         (Eg: MSG_EXPEDITE, MSG_FAST_EXPEDITE, etc)        */
  boolean can_do_sdb; /* On return from ioctl, this boolean field will say
                         whether SDB supported or not                      */
} dss_iface_ioctl_707_sdb_support_query_type;

/*---------------------------------------------------------------------------
   DSS_IFACE_IOCTL_707_ENABLE_HDR_HPT_MODE parameter type
---------------------------------------------------------------------------*/
typedef struct
{
  sint15  dss_nethandle;
  boolean enable;
} dss_iface_ioctl_707_enable_hdr_hpt_mode_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_ENABLE_HDR_REV0_RATE_INERTIA parameter type
---------------------------------------------------------------------------*/
typedef struct
{
  sint15 dss_nethandle;
  boolean enable;
  dss_iface_ioctl_event_cb  event_cb;   /* event notification callback     */
  void                     *user_data;  /* User data passed to callback    */
} dss_iface_ioctl_707_enable_hdr_rev0_rate_inertia_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_707_[GS]ET_HDR_1X_HANDDOWN_OPTION parameter type
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_hdr_1x_handdown_option_type
  dss_iface_ioctl_707_hdr_1x_handdown_option_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_ENABLE_HDR_SLOT_CYCLE_INDEX parameter type
---------------------------------------------------------------------------*/
typedef struct
{
  sint15 dss_nethandle;
  boolean enable;
  uint32 slotted_mode_option;           /* slot cycle value */
  uint32 get_slotted_mode;              /* indicate intent to get SM value */
  dss_iface_ioctl_event_cb  event_cb;   /* event notification callback     */
  void                     *user_data;  /* User data passed to callback    */
} dss_iface_ioctl_707_enable_hdr_slotted_mode_type;

/*---------------------------------------------------------------------------
  Data type used for 707 holddown timer (DSS_IFACE_IOCTL_707_ENABLE_HOLDDOWN)
  Specify true for enabling timer, false for disabling.
---------------------------------------------------------------------------*/
typedef boolean dss_iface_ioctl_enable_holddown_type;

/*---------------------------------------------------------------------------
  Data type used for getting DSS_IFACE_IOCTL_707_HDR_GET_RMAC3_INFO.
---------------------------------------------------------------------------*/
typedef ps_flow_ioctl_707_hdr_rmac3_info_type
  dss_iface_ioctl_707_hdr_rmac3_info_type;

/*---------------------------------------------------------------------------
  Data type used for getting Transmission Status Information
---------------------------------------------------------------------------*/
typedef ps_flow_ioctl_707_tx_status_type dss_iface_ioctl_707_tx_status_type;

/*---------------------------------------------------------------------------
  Data type used for 707 Hysteresis Activation timer value:
  Get: if value is -1 timer is inactive, rest of the values App can
       consider timer is active and the value represents the time remaining!
  Set: Valid range: 0 to MAX(30s).
---------------------------------------------------------------------------*/
typedef int dss_iface_ioctl_707_hat_type;

/*---------------------------------------------------------------------------
  Data type used for DSS_IFACE_IOCTL_707_GET_SIP_MIP_DATA_CALL_TYPE
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_sip_mip_data_call_enum_type
  dss_iface_ioctl_sip_mip_data_call_enum_type;

/*---------------------------------------------------------------------------
  Data type used for DSS_IFACE_IOCTL_UW_FMC_SET_TUNNEL_PARAMS
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_uw_fmc_tunnel_params_type
  dss_iface_ioctl_uw_fmc_tunnel_params_type;

/*---------------------------------------------------------------------------
  Data type used for DSS_IFACE_IOCTL_ENABLE_FIREWALL
---------------------------------------------------------------------------*/
typedef struct
{
  int16                dss_nethandle;   /* Clients app ID */
  boolean              is_pkts_allowed; /* Pkts allowed through firewall */
} dss_iface_ioctl_enable_firewall_type;

/*---------------------------------------------------------------------------
  Data type used for DSS_IFACE_IOCTL_DISABLE_FIREWALL
---------------------------------------------------------------------------*/
typedef struct
{
  int16                      dss_nethandle;  /* Clients app ID */
} dss_iface_ioctl_disable_firewall_type;

/*---------------------------------------------------------------------------
  Data type used for adding firewall rule DSS_IFACE_IOCTL_ADD_FIREWALL_RULE
---------------------------------------------------------------------------*/
typedef struct
{
  int16                      dss_nethandle; /* Clients app ID */
  ip_filter_type             fltr_spec;     /* Filter specifications */
  uint32                     handle;        /* Handle to the firewall rule */
} dss_iface_ioctl_add_firewall_rule_type;

/*---------------------------------------------------------------------------
  Data type used for deleting firewall rule DSS_IFACE_IOCTL_DELETE_FIREWALL_RULE
---------------------------------------------------------------------------*/
typedef struct
{
  int16                      dss_nethandle;   /* Clients app ID */
  uint32                     handle;          /* Handle to the firewall rule */
} dss_iface_ioctl_delete_firewall_rule_type;

/*---------------------------------------------------------------------------
  Data type used to retrieve firewall rule DSS_IFACE_IOCTL_GET_FIREWALL_RULE
---------------------------------------------------------------------------*/
typedef struct
{
  int16                      dss_nethandle;   /* Clients app ID */
  uint32                     handle;          /* Handle to the firewall rule */
  ip_filter_type             fltr_spec;       /* Filter specifications */
} dss_iface_ioctl_get_firewall_rule_type;

/*---------------------------------------------------------------------------
  Data type used to retrieve firewall rule DSS_IFACE_IOCTL_GET_FIREWALL_TABLE
---------------------------------------------------------------------------*/
typedef struct
{
  int16                  dss_nethandle;       /* Clients app ID */
  uint8                  num_fltrs;           /* Number of requested filters */
  ip_filter_type         *fltr_spec_arr;      /* Filter specifications */
  uint8                  avail_num_fltrs;     /* Available no of filters */
} dss_iface_ioctl_get_firewall_table_type;

/*---------------------------------------------------------------------------
  Data type used for DSS_IFACE_IOCTL_GET_IFACE_STATS
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_iface_stats_type
  dss_iface_ioctl_get_iface_stats_type;

/*---------------------------------------------------------------------------

                                UMTS DATA STRUCTURES

    The following data structs are specific to UMTS interface.
 ---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  MBMS context activate/deactivate ioctls
---------------------------------------------------------------------------*/
typedef struct
{
  ip_addr_type               ip_addr;          /* Multicast IP address     */
  uint32                      profile_id;      /* UMTS unicast profile ID */

  dss_iface_ioctl_event_cb   event_cb;         /* Callback for mcast ev    */
  void                       *user_data_ptr;   /* App specific data,
                                                  passed in cback          */
  sint15                     nethandle;        /* dss_nethandle            */

  /* Output Paramters */
  uint32                     handle;           /* MBMS handle              */

} dss_iface_ioctl_mbms_mcast_context_act_type;

typedef struct
{
  sint15                      nethandle;
  uint32                      handle;         /* MBMS handle              */
} dss_iface_ioctl_mbms_mcast_context_deact_type;

/*---------------------------------------------------------------------------
  Argument to be passed with the ioctls
  DSS_IFACE_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE
  DSS_IFACE_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE
  These IOCTLs are valid only when iface is up.
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_IFACE_3GPP_LTE_REESTAB_CALLTYPE_NORMAL,
  DSS_IFACE_3GPP_LTE_REESTAB_CALLTYPE_VOLTE,
} dss_iface_ioctl_3gpp_lte_reestab_calltype_enum_type;

/*---------------------------------------------------------------------------
  Argument for DSS_IFACE_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE
---------------------------------------------------------------------------*/
typedef struct
{
  dss_iface_ioctl_3gpp_lte_reestab_calltype_enum_type calltype;
} dss_iface_ioctl_3gpp_get_lte_reestab_calltype_type;

/*---------------------------------------------------------------------------
  Argument for DSS_IFACE_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE
---------------------------------------------------------------------------*/
typedef struct
{
  dss_iface_ioctl_3gpp_lte_reestab_calltype_enum_type calltype;
} dss_iface_ioctl_3gpp_set_lte_reestab_calltype_type;

/*---------------------------------------------------------------------------
  Argument for DSS_IFACE_IOCTL_SET_SILENT_REDIAL
---------------------------------------------------------------------------*/
typedef struct
{
  boolean set_silent_redial;
} dss_iface_ioctl_set_silent_redial_type;

/*---------------------------------------------------------------------------
  Argument for DSS_IFACE_IOCTL_KEEP_ALIVE
---------------------------------------------------------------------------*/
typedef boolean dss_iface_keep_alive_flag_type;

/*---------------------------------------------------------------------------
  Argument for DSS_IFACE_IOCTL_FILTER_MATCH_REQUEST
---------------------------------------------------------------------------*/

typedef ps_ip_filter_match_type dss_ip_filter_match_type;

typedef ps_iface_ioctl_filter_match_request_type dss_iface_ioctl_filter_match_request_type;

typedef ps_iface_filter_match_info dss_iface_filter_match_info;

typedef ps_fltr_direction_enum_type dss_fltr_direction_enum_type;

#ifdef __cplusplus
extern "C" {
#endif
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION DSS_IFACE_IOCTL()

DESCRIPTION
  This function dstermines the ps_iface_ptr associated with the passed in
  identifier (App ID). It then calls ps_iface_ioctl().

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
  dss_iface_id_type        iface_id,
  dss_iface_ioctl_type     ioctl_name,
  void                     *argval_ptr,
  sint15                   *dss_errno
);

#ifdef __cplusplus
}
#endif

#endif /* DSS_IFACE_IOCTL_H */
