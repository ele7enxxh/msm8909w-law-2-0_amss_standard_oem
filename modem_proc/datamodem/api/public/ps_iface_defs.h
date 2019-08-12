#ifndef PS_IFACE_DEFS_H
#define PS_IFACE_DEFS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ I F A C E _ D E F S . H

DESCRIPTION
  Header containing PS_IFACE names that need to be used in other contexts,
  but that don't required the entire ps_iface header.

Copyright (c) 2002-2016 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iface_defs.h_v   1.4   07 Feb 2003 20:12:48   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_iface_defs.h#1 $ : //source/qcom/qct/modem/api/datacommon/main/latest/ps_iface_defs.h#41 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
12/20/14    pg     Added TRAT/SRAT info in Handoff events.
11/04/14    sd     Added new call end reason for subs_id mismatch.
07/09/14    niv    Added new PS_NET_DOWN Reason for NV Refresh in progress
06/30/14    ss     New event added: IFACE_EXTENDED_IP_CONFIG_EX_EV
06/04/14    cx     Add support for 32 wlan clients
03/06/14    cx     Set Minimum IPv4 MTU to 68 as per RFC791
11/20/13    ash    Increase MAX_IPV6_LO_IFACES to 19
11/11/13    rk     Removed feature mobileap.
01/18/13    tw     Move alternate PDN below IP fltr client max
12/07/12    tw     Multi PDN over 1 RMNET 
10/09/12    sj     Increase MAX_SYSTEM_IFACES for 8 PDN support
08/06/12    wc     Increase MAX_SYSTEM_IFACES by 1 to support 1 eMBMS iface
04/02/12    jee    New call end reasons to propagate correct call end reason 
03/21/12    op     Added new CDMA rat masks PS_IFACE_CDMA_EVDO_EHRPD and
                   PS_IFACE_CDMA_FMC
03/13/12    ss     Added RAB_FAILURE and ESM local cause codes.
03/08/12    su     Changes for VToLTE feature
02/10/12    rt     Incremented MAX IID and added support for SOFTAPv6 iface
01/25/12    rr     Added support for profile apn bearer
01/04/12    hs     Added support for eMBMS
11/17/11    sa     Added support for V6 LO_MODE_B_IFACE.
11/13/11    msh    Added call end reasons for pdn throttling
10/03/11    msh    Added call end reason for Ipv6 HDR
10/03/2011  su     Added new enum Pre Register Handoff
08/31/11    ss     Added new cause code to map SM internal cause for PDP
                   activation reject.
08/28/11    aa     Added new event IFACE_NET_INITIATED_QOS_AVAILABLE_EV
08/08/11    sa     Added new rat mask PS_IFACE_3GPP_64_QAM.
08/03/11    mct    Added new event for reporting fast dormancy status to apps.
07/15/11    msr    Defined minimum MTU values for v4 and v6 ifaces
06/06/2011  sid    Added iface teardown reason enum to support
                   PPP partial context feature.
05/09/10    asn    Added support for new event for active Ifaces
04/27/11    sy     Added PAP, CHAP failures to PPP call end reason.
12/10/10    ls     Updated ps_iface_net_down_reason_type enum to include
                   some call cmd err reason
10/03/10    az     Additional net down cause code for LTE and eHRPD
09/13/10    ss     Adding support for new hs_status_ind: HSDPA+ and DC_HSDPA+
08/07/10    at     IPV6_ENABLED_DEFAULT value set to TRUE.
07/21/10    gc     Changed some conflicting error codes
07/15/10    ts     Added support for linger and dormancy.
06/30/10    rt     NAT IFACE addition.
05/26/10    gc     Pass PPP failure code to PS
05/14/10    guru   Added defenitions for supporting IP session continuity
05/12/10    jy     New data type ps_iface_bearer_ip_type for supporting Dual-IP
                   bearer EPC handoff.
04/21/10    sa     Added LO_MODE_B_IFACE.
03/15/10    hs     Increased MAX_SYSTEM_IFACES by 2 to support 4 PDNs on LTE
02/17/10    hs     Added definitions to support bearer tech IOCTL on LTE
08/31/09   mga     Merged from eHRPD branch
04/28/09    dm     Added new MIP net down reason error codes
03/26/09    pp     CMI De-featurization.
12/14/08    pp     Common Modem Interface: Public/Private API split.
07/18/08    dm     Fixed compiler warnings
07/01/08    pp     Added SLIP_IFACE, UICC_IFACE.
05/22/08    am     Added boolean force_dereg_cbacks to
                   ps_iface_mcast_event_info_type.
05/15/08    dm     Added support to pass the dormancy info code (reject cause)
                   in case of RAB reestablishment reject to applications
04/03/08    am     BCMCS 2.0 support.
05/21/07    msr/sv Added support for IFACE linger
04/17/07    es     Added support for EXTENDED_IP_CONFIG_EV.
03/27/07    msr    Added API for RF conditions and HDR-1x handdown
01/23/07    msr    EMPA enhancements
12/15/06    mct    Added additional event/timer for privacy extensions.
12/04/06    asn    ENH to propagate SM cause-codes to App
11/16/06    msr    Added FLOW_INFO_CODE_UPDATED_EV
11/02/06    mct    Added enum for total supported IPv6 addresses. Added
                   missing WLAN iface to total number of supported ifaces.
11/1/06     rt     Merged EIDLE_SCI feature changes.
09/18/06    hm     Moved macro PS_IFACE_IS_ID_A_GROUP to ps_iface.h
09/12/06    msr    Removed redundant state field in event_info structure
08/14/06    sv     Merged slotted/Mode attribute ioctl.
08/03/06    sv     Added support for header compression filtering.
07/31/06    msr    Added support for PRIMARY_QOS_MODIFY
06/06/06    ss     Incremented max ifaces to account for DVB-H IPv6 iface
05/18/06    msr    Added STA_IFACE
04/30/06    rt     Added PS_IFACE_MAX_DOMAIN_NAME_SIZE for SIP server
                   domain names.
04/18/06    mct    Added enums for loopbacl optimization and supporting
                   multiple IPv6 iids in RFC 3041.
04/14/06    ss     Added DVBH iface name and DVBH info codes
03/28/06    mpa    Added new BCMCS flow status.
02/13/06    mct    Updated/added some BCMCS info codes.
01/25/06    mct    Renamed some QOS info codes.
12/07/05    vas    Added more failure reasons in rate inertia failure enum
12/05/05    msr    Changed IP fltr client IDs
12/05/05    msr    Split QOS client in to INPUT and OUTPUT clients
11/14/05    rt     Added new cause code enums in
                   ps_iface_net_down_reason_type.
11/08/05    msr    Fixed merge issues related to HDR rate inertia changes
10/18/05    msr    Removed support for FLOW_ACTIVATE_MODIFIED_EV and
                   PHYS_LINK_UP_MODIFIED_EV and changed ipfltr_info structure
10/12/05    mct    Added support for iface flow added/deleted events.
09/13/05    sv     Added support for new HDR rev0 rate inertia ioctl and HDR
                   HPT mode ioctl.
09/01/05    rt     Moved ps_iface_data_bearer_rate definition to
                   ps_iface_defs.h
09/01/05    rt     Added ps_iface_ioctl_bearer_tech_changed_type for
                   TECH_CHANGED_EV
08/26/05    mpa    Added new BCMCS flow statuses.
08/15/05    mct    Added support for QOS_AWARE/UNAWARE_EV and added
                   QOS_CONFIGURING.
08/14/05    rt     Added IFACE_BEARER_TECH_CHANGED_EV.
08/03/05    msr    Added FLOW_MODIFY_ACCEPTED_EV and FLOW_MODIFY_REJECTED_EV
07/25/05    rt     Added ps_iface_net_down_reason_type and modified
                   structure ps_iface_event_info_u_type.
05/03/05    msr    Moved all IP FLTR related definitions to ps_iface_ipfltr.h
04/18/05    mct    Added Multicast support.
04/17/05    msr    Added stuff (states, events etc) related to new ps_flow.
04/16/05    ks     Added new events and states for new PHYS_LINK_NULL state.
03/15/05    tmr    Adding DUN_IFACE to iface definitions
01/10/05    sv     Merged IPSEC changes.
01/10/05    msr    Increased MAX_SYSTEM_IFACES to 15 to include Ethernet.
01/08/05    vp     Removed the extra defines for DEFAULT_V6_INDEX and
                   MAX_IPV6_PREFIXES.
12/23/04    lyr    Added WLAN_IFACE to ps_iface_name_enum_type
11/19/04    msr    Added IFACE_STATE_INVALID, PHYS_LINK_STATE_INVALID states.
11/16/04    ks     Removed IFACE_PHYS_LINK_UP_MODIFIED_EV,
                   IFACE_PHYS_LINK_FLOW_ENABLED_EV and
                   IFACE_PHYS_LINK_FLOW_DISABLED_EV.
10/25/04    msr    Increased MAX_SYSTEM_IFACES to include IP ANY and
                   IPV6 loopback interfaces.
10/27/04    jd     Added rx and tx packet counters to ps_iface structure
08/13/04    mct    Added support for the MT event.
08/12/04    sv     Added support to distinguish IFACE_FLOW_ENABLED event
                   from PHYS_LINK_FLOW_ENABLED event.
08/11/04    mvl    Added default values for IPv6 enabled and failover mode.
08/06/04    mct    Added failover enums so they are accessible by PS task.
07/30/04    mvl    Changed representation of states to be a bit mask as that
                   will simplify cheking state against multiple states.
07/29/04    msr    Added IFACE_ADDR_FAMILY_CHANGED_EV.
07/12/04    mvl    Added some support for IPv6 - including new indication.
06/11/04    vp     Typedef'ed ps_iface_addr_family_type as ip_addr_enum_type
05/17/04    aku    Removed featurization of BCMCS iface with the objevtive of
                   not having any features in the header file.
04/28/04    aku    Added support for the BCMCS iface
04/27/04    usb    ps_iface_ipfltr_id_type definition, incremented max
                   iface count by 1 for 1x v6 iface.
04/19/04    vp     Added ps_iface_ipv6_iid_enum_type
03/18/04    ak     Added enums for phys link asking for dormant info.
02/18/04    usb    Added client id enum for IP filtering clients, updated
                   event info to include client id.
02/04/04    ak     Added in events for phys link and transient states.
12/26/03    usb    Added new ps_iface state and event for CONFIGURING.
10/14/03    mct    Added event enums for coming_up, going_down, and phys_link
                   equivalents.
08/28/03    ss     Changed ps_iface_id_type to uint32. Added macro to
                   determine if an id corresponds to a group
08/15/03    aku    Changes to support phys link separation from ps_iface
07/22/03    usb    New data type, enums and constant definitions, bumped up
                   MAX_SYSTEM_IFACES value.
05/27/03    om     Added IPSEC_IFACE name.
05/13/03    usb    Incremented MAX_SYSTEM_IFACES for near future needs,
                   changed iface name values to bit mask.
05/05/03    aku    Moved MAX_SYSTEM_IFACES defintion from ps_iface.h
02/07/03    usb    Added event IFACE_IPFLTR_UPDATED_EV.
01/31/03    usb    Added LO_IFACE to the iface name enum
12/24/02    aku    Moved ps_iface_event_enum_type to this file from
                   ps_iface.h
11/15/02    mvl    Added physical link statistics.
09/17/02    mvl    created file.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "dserrno.h"
#include "ps_in.h"
#include "ps_iface_embms.h"
#include "ds_sys.h"
#include "ps_ipfltr_defs.h"

/*===========================================================================

                              TYPE DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  MAX_SYSTEM_IFACES - The maximum number of interfaces in the system.
  4 - CDMA SN IFACE (1 x v4 pkt + 1 x v6 pkt + 1 x async + 1 x any)
  1 - CDMA AN IFACE
  3 - UMTS IFACE (3 Primary PDP contexts)
  1 - WLAN IFACE
  3 - SIO IFACE (1 x CDMA, 1 x UMTS, 1 x Ethernet)
  2 - LO IFACE  (1 x v4 pkt + 1 x v6 pkt)
 16 - IPSEC IFACE
  1 - CDMA MCAST IFACE
  1 - MediaFLO IFACE
  1 - DUN (dial-up networking) IFACE
  2 - DVB-H IFACE (1 IPv4 + 1 IPv6)
  1 - STA IFACE
  1 - MBMS IFACE
  1 - eMBMS IFACE
  1 - UW_FMC IFACE
  2 - LO_MODE_B_IFACE
  1 - MODEM_LINK_LOCAL_IFACE
  --------------------------------------------------------------------------
  The following section covers the IPv4 and IPv6 loopback routing tables.
  --------------------------------------------------------------------------
  When a new interface is added the appropriate loopback #define listed below
  which matches the interface's IP address family should be updated.

  The value of MAX_IPV4_LO_IFACES and MAX_IPV6_LO_IFACES should always be:
  the total number of loopback ifaces for that IP version + 1;

  Logical interfaces ONLY supported if they obtain their own unique IP address
  and do not inherit IP information from an underlying iface. IPSEC ifaces
  are not supported. Do not add these ifaces to the loopback section or
  increment the loopback defines.

  MAX_IPV4_LO_IFACES - The maximum number of ifaces supported for IPv4 loopback.
  1 - CDMA
  3 - UMTS
  1 - WLAN
  3 - SIO
  1 - LO
  1 - DUN
  -------
  10 - Total

  MAX_IPV6_LO_IFACES - The maximum number of ifaces supported for IPv6 loopback.
  8 - CDMA
  8 - UMTS
  1 - LO
  1 - MODEM_LINK_LOCAL_IFACE
  1 - IWLAN_3GPP_IFACE or IWLAN_3GPP2_IFACE  
  -------
  19 - Total

---------------------------------------------------------------------------*/
#define MAX_SYSTEM_IFACES  119

/*---------------------------------------------------------------------------
Maximum no of event queues per iface for iface events, flow events and 
phys link events. So total number of event queues per iface would be 
2 queues * 3 (iface, physlink and flow) + 1 (global iface queue) = 7

Iface Event Queue 1 : IFACE_FLOW_ENABLED_EV, IFACE_FLOW_DISABLED_EV
Iface Queue 1 : Rest of the iface events

Phys Link Event Queue 1 : PHYS_LINK_FLOW_ENABLED_EV, PHYS_LINK_FLOW_DISABLED_EV
Phys Link Event Queue 2 : Rest of the phys link events

Flow Event Queue 1 : FLOW_TX_ENABLED_EV, FLOW_TX_DISABLED_EV
Flow Event Queue 2 : Rest of the flow events

---------------------------------------------------------------------------*/
#define MAX_EVENT_QUEUES 2

#define MAX_IPV4_LO_IFACES 11
#define MAX_IPV6_LO_IFACES 19
/*---------------------------------------------------------------------------
  MAX_PHYS_LINKS_PER_IFACE - maximum number of phys links allowed for each
  iface.
---------------------------------------------------------------------------*/
#define MAX_PHYS_LINKS_PER_IFACE  5

/*---------------------------------------------------------------------------
  PS_IFACE_MAX_DOMAIN_NAME_SIZE - Maximum Domain name size.
---------------------------------------------------------------------------*/
#define PS_IFACE_MAX_DOMAIN_NAME_SIZE    256

/*---------------------------------------------------------------------------
  PS_IFACE_MAX_SIP_SERVER_DOMAIN_NAMES - Max supported Sip domain names.
---------------------------------------------------------------------------*/
#define PS_IFACE_MAX_SIP_SERVER_DOMAIN_NAMES    5

/*---------------------------------------------------------------------------
  PS_IFACE_MAX_FIREWALL_RULES - Max supported Firewall rules.
---------------------------------------------------------------------------*/
#define PS_IFACE_MAX_FIREWALL_RULES    15

/*---------------------------------------------------------------------------
PS_IFACE_MAX_SIP_SERVER_ADDRESSES - Max supported Sip addresses.
---------------------------------------------------------------------------*/
#define PS_IFACE_MAX_SIP_SERVER_ADDRESSES       6

/*---------------------------------------------------------------------------
PS_IFACE_MAX_SEARCH_LIST_DOMAIN_NAMES -
                                 Max supported domain name search list size
---------------------------------------------------------------------------*/
#define PS_IFACE_MAX_SEARCH_LIST_DOMAIN_NAMES    6

/*---------------------------------------------------------------------------
PS_IFACE_NUM_DNS_ADDRS -
Currently DS support a maximum of 2 DNS addresses per iface -
Primary and Secondary.
This define shall be irrelevant and its usage altered when the handset
supports more DNS addresses.
---------------------------------------------------------------------------*/
#define PS_IFACE_NUM_DNS_ADDRS               2

/*---------------------------------------------------------------------------
  MAX_IPV6_PREFIXES - the maximum number of IPv6 prefixes we can support
---------------------------------------------------------------------------*/
#define MAX_IPV6_PREFIXES 1
#define DEFAULT_V6_INDEX  0

/*---------------------------------------------------------------------------
  MAX_IPV6_IIDS - the maximum number of IPv6 iids we support per interface,
                  including internal addresses and those from ext. devices
---------------------------------------------------------------------------*/
#define MAX_IPV6_IIDS 70

/*---------------------------------------------------------------------------
  DEFAULT_UNUSED_IPV6_PRIV_ADDR_TIMEOUT - default (2 min, or 120,000 ms)
    The maximum time to wait for an application to bind to the privacy addr
    before deleteing it.
---------------------------------------------------------------------------*/
#define DEFAULT_UNUSED_IPV6_PRIV_ADDR_TIMEOUT 120000

/*---------------------------------------------------------------------------
  MIN_IPV6_VALID_LIFETIME - The minimum time (7200 sec) that the IPv6 valid
    lifetime can be set to.
---------------------------------------------------------------------------*/
#define MIN_IPV6_VALID_LIFETIME 7200

/*---------------------------------------------------------------------------
  MAX_IPV6_ADDRS - the total number of IPv6 addresses supported per interface
---------------------------------------------------------------------------*/
#define MAX_IPV6_ADDRS (MAX_IPV6_PREFIXES * MAX_IPV6_IIDS)

/*---------------------------------------------------------------------------
  Maximum number of filters allowed in each QOS request. UMTS can fit atmost
  8 filters in one message. By restricting this to 8, UMTS need not worry
  about whether network rejected previous message.
---------------------------------------------------------------------------*/
#define MAX_FLTR_PER_REQ  16
#define MAX_ADDITIONAL_FLOWS_PER_REQ 6            /* 8 total: min/max/other
                                                     8 for rx and 8 for tx */

/*---------------------------------------------------------------------------
  PS_IFACE_MAX_MCAST_FLOWS - maximum number of mcast flow requests in a
  MCAST_JOIN_EX / MCAST_LEAVE_EX / MCAST_REGISTER_EX ioctl calls.
---------------------------------------------------------------------------*/
#define PS_IFACE_MAX_MCAST_FLOWS (25)

/*---------------------------------------------------------------------------
  PS_IFACE_MAX_STATIC_NAT_ENTRIES - maximum number of static NAT entry
  get requests in a PS_IFACE_IOCTL_GET_STATIC_NAT_ENTRY
---------------------------------------------------------------------------*/
#define PS_IFACE_MAX_STATIC_NAT_ENTRIES (10)

/*---------------------------------------------------------------------------
  Define minimum MTU values for V4 and V6 ifaces. An iface can never have a
  value lower than these values
---------------------------------------------------------------------------*/
#define PS_IFACE_MIN_V4_MTU   68
#define PS_IFACE_MIN_V6_MTU  1280

/*---------------------------------------------------------------------------
  Default filter id and precedence when qos request is initiated
---------------------------------------------------------------------------*/
#define PS_IFACE_IPFLTR_DEFAULT_ID 256
#define PS_IFACE_IPFLTR_DEFAULT_PRCD 256

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_ID_TYPE
  Opaque handle which uniquely identifies ps_iface
  First 8 bits are index into global_iface_ptr_array,
  Next 16 bits are reserved for future use, currently are set to FFFF
  Lowest 8 bits are phys link instance

  +----------------+----------------+----------------+----------------+
  |    8 bits      |    8 bits      |    8 bits      |    8 bits      |
  +----------------+----------------+----------------+----------------+
  |   Iface Index  |    Reserved for future use      | phys link inst |
  +----------------+----------------+----------------+----------------+
---------------------------------------------------------------------------*/
typedef uint32 ps_iface_id_type;

/*---------------------------------------------------------------------------
PS_IFACE_INVALID_ID - Id which does not identify any valid iface
---------------------------------------------------------------------------*/
#define PS_IFACE_INVALID_ID  0

#define PS_IFACE_PROC_ID_LOCAL 0x0
#define PS_IFACE_PROC_ID_TE_DUN 0x1
#define PS_IFACE_PROC_ID_ANY   0x7FFFFFFE
#define PS_IFACE_PROC_ID_NONE  0x7FFFFFFF


#define PS_IFACE_PROC_ID_TE_DUN   0x1
#define PS_IFACE_APP_ID_TE_DUN    0x0000000100000000ULL

#define PS_IFACE_PROC_ID_APPS     0x2
#define PS_IFACE_APP_ID_APPS      0x0000000200000000ULL

#define PS_IFACE_PROC_ID_TE_RMNET 0x3
#define PS_IFACE_APP_ID_TE_RMNET  0x0000000300000000ULL

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_MT_HANDLE_TYPE
---------------------------------------------------------------------------*/
typedef uint32 ps_iface_mt_handle_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_MCAST_HANDLE_TYPE
---------------------------------------------------------------------------*/
typedef uint32 ps_iface_mcast_handle_type;

/**
  @brief    Event handle
 
  @details  Type declaration for event handle
   
  First 16 bits are index into global_iface_ptr_array,
  Next 16 holds cookie
  
  +----------------+----------------+----------------+----------------+
  |    8 bits      |    8 bits      |    8 bits      |    8 bits      |
  +----------------+----------------+----------------+----------------+
  |              Iface Index        |          cookie value           |
  +----------------+----------------+----------------+----------------+ 
  
*/
typedef uint32 ps_iface_event_handle_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_NAME_ENUM_TYPE

DESCRIPTION
  List of interface names and groups.

  IMPORTANT NOTE: increment MAX_SYSTEM_IFACES (see below) appropriately
  if a new iface name is added to this list.
---------------------------------------------------------------------------*/
typedef enum
{
  /* Group Names - these are bit masks (uses bottom 15 bits), msb is 0 */
  ANY_DEFAULT_GROUP  = 0x0001,
  WWAN_GROUP         = 0x0002,
  RM_GROUP           = 0x0004,
  BCAST_MCAST_GROUP  = 0x0008,
  IFACE_3GPP_GROUP   = 0x0010,
  IFACE_3GPP2_GROUP  = 0x0020,
  IFACE_EPC_GROUP    = 0x0040,
  ANY_IFACE_GROUP    = 0x7FFF,

  IFACE_MASK         = 0x8000,
  CDMA_SN_IFACE      = 0x8001,
  CDMA_AN_IFACE      = 0x8002,
  UMTS_IFACE         = 0x8004,
  SIO_IFACE          = 0x8008,
  CDMA_BCAST_IFACE   = 0x8010,
  WLAN_IFACE         = 0x8020,
  IWLAN_S2B_IFACE    = 0x8021,
  DUN_IFACE          = 0x8040,
  FLO_IFACE          = 0x8080,
  DVBH_IFACE         = 0x8100,
  STA_IFACE          = 0x8200,
  IPSEC_IFACE        = 0x8400,
  LO_MODE_B_IFACE    = 0x8401,
  LO_IFACE           = 0x8800,
  MBMS_IFACE         = 0x8801,
  IWLAN_3GPP_IFACE   = 0x8802,
  IWLAN_3GPP2_IFACE  = 0x8804,
  MIP6_IFACE         = 0x8808,
  SLIP_IFACE         = 0x8810,
  UICC_IFACE         = 0x8820,
  UW_FMC_IFACE       = 0x8840,
  EPC_IFACE          = 0x8880,
  CLAT_IFACE         = 0x8881,
  EMBMS_IFACE        = 0x8882,  /**< eMBMS interface for both V4 and V6
                                     traffic */
  MODEM_LINK_LOCAL_IFACE  = 0x8888
} ps_iface_name_enum_type;


/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_STATE_ENUM_TYPE

DESCRIPTION
  Enum for states of an interface.  This is a bit mask to ease comparisons
  NOT so that the interface can be in multiple states!  i.e. assignments
  should always be straight assignements.
---------------------------------------------------------------------------*/
typedef enum
{
  IFACE_STATE_INVALID = 0x00,
  IFACE_DISABLED      = 0x01,
  IFACE_DOWN          = 0x02,
  IFACE_COMING_UP     = 0x04,
  IFACE_CONFIGURING   = 0x08,   /* address configuration in progress       */
  IFACE_ROUTEABLE     = 0x10,   /* packet can be routed to this interface  */
  IFACE_UP            = 0x20,   /* data can originate over this interface  */
  IFACE_GOING_DOWN    = 0x40,
  IFACE_LINGERING     = 0x80
} ps_iface_state_enum_type;


/**
  Identifies handoff classes of an interface.  This is a bit
  mask to ease comparisons NOT so that the interface can be in
  multiple classes! i.e. assignments should always be straight
  assignements.
*/
typedef enum
{
  PS_IFACE_HANDOFF_CLASS_NONE        = 0x00, /**< No Handoff default value */
  PS_IFACE_HANDOFF_CLASS_EPC         = 0x01, /**< Active Handoff */
  PS_IFACE_HANDOFF_CLASS_EPC_PREREG  = 0x02  /**< Pre Register Handoff, not active */
} ps_iface_handoff_class_enum_type;


/*---------------------------------------------------------------------------
TYPEDEF FLOW_STATE_TYPE

DESCRIPTION
  Flow state enum - what is the flow doing
---------------------------------------------------------------------------*/
typedef enum
{
  FLOW_STATE_INVALID = 0x00,
  FLOW_NULL          = 0x01,
  FLOW_ACTIVATING    = 0x02,
  FLOW_ACTIVATED     = 0x04,
  FLOW_SUSPENDING    = 0x08,
  FLOW_SUSPENDED     = 0x10,
  FLOW_RESUMING      = 0x20,
  FLOW_GOING_NULL    = 0x40,
  FLOW_CONFIGURING   = 0x80
} ps_flow_state_enum_type;


/*---------------------------------------------------------------------------
TYPEDEF PHYS_LINK_STATE_TYPE

DESCRIPTION
  Physical link state enum - what is the physical link doing
---------------------------------------------------------------------------*/
typedef enum
{
  PHYS_LINK_STATE_INVALID = 0x00,
  PHYS_LINK_DOWN          = 0x01,
  PHYS_LINK_COMING_UP     = 0x02,
  PHYS_LINK_UP            = 0x04,
  PHYS_LINK_GOING_DOWN    = 0x08,
  PHYS_LINK_RESUMING      = 0x10,
  PHYS_LINK_GOING_NULL    = 0x20,
  PHYS_LINK_NULL          = 0x40
} phys_link_state_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_IPV6_ADDR_STATE_ENUM_TYPE

DESCRIPTION
  State of the IPv6 address.
---------------------------------------------------------------------------*/
typedef enum
{
  // No address has been created. An invalid entry.
  IPV6_ADDR_STATE_INVALID    = 0,
  // This address is pending DAD verification
  IPV6_ADDR_STATE_TENTATIVE  = 1,
  // This address has been verified (DAD/local) and is ready to be allocated
  IPV6_ADDR_STATE_UNASSIGNED = 2,
  // This address has been assigned.
  IPV6_ADDR_STATE_VALID      = 3,
  // This address has been deprecated
  IPV6_ADDR_STATE_DEPRECATED = 4
} ps_iface_ipv6_addr_state_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_IPV6_ADDR_TYPE_ENUM_TYPE

DESCRIPTION
  The type of the IPv6 address. Public/Private, etc.
---------------------------------------------------------------------------*/
typedef enum
{
  // This is either a PUBLIC address or unused
  IPV6_ADDR_TYPE_INVALID          = 0,
  // This address is a public address
  IPV6_ADDR_TYPE_PUBLIC           = 1,
  // This address is an in use private shareable address
  IPV6_ADDR_TYPE_PRIV_SHARED      = 2,
  // This address is an in use private unique address
  IPV6_ADDR_TYPE_PRIV_UNIQUE      = 3,
  // This address is in use by an external device
  IPV6_ADDR_TYPE_EXTERNAL         = 4
} ps_iface_ipv6_addr_type_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_EVENT_ENUM_TYPE

DESCRIPTION
  List of all the possible events on an interface that modules can register
  callbacks for.
---------------------------------------------------------------------------*/
typedef enum
{
  IFACE_MIN_EV                   =  0,
  IFACE_PHYS_LINK_DOWN_EV        =  0,
  IFACE_PHYS_LINK_COMING_UP_EV   =  1,
  IFACE_PHYS_LINK_UP_EV          =  2,
  IFACE_PHYS_LINK_GOING_DOWN_EV  =  3,
  IFACE_PHYS_LINK_GONE_EV        =  4,
  IFACE_PHYS_LINK_MAX_EV,

  /* phys_link aliases for the existing events       */
  PHYS_LINK_DOWN_EV          = IFACE_PHYS_LINK_DOWN_EV,
  PHYS_LINK_COMING_UP_EV     = IFACE_PHYS_LINK_COMING_UP_EV,
  PHYS_LINK_UP_EV            = IFACE_PHYS_LINK_UP_EV,
  PHYS_LINK_GOING_DOWN_EV    = IFACE_PHYS_LINK_GOING_DOWN_EV,
  PHYS_LINK_GONE_EV          = IFACE_PHYS_LINK_GONE_EV,
  PHYS_LINK_FLOW_ENABLED_EV  = 5,
  PHYS_LINK_FLOW_DISABLED_EV = 6,
  PHYS_LINK_RESUMING_EV      = 7,
  PHYS_LINK_GOING_NULL_EV    = 8,
  PHYS_LINK_707_DOS_ACK_EV   = 9,

  PHYS_LINK_MAX_EV,
  IFACE_UP_EV                     = 10,
  IFACE_GOING_DOWN_EV             = 11,
  IFACE_ENABLED_EV                = 12,
  IFACE_DISABLED_EV               = 13,
  IFACE_DOWN_EV                   = 14,
  IFACE_COMING_UP_EV              = 15,
  IFACE_CONFIGURING_EV            = 16,
  IFACE_FLOW_ENABLED_EV           = 17,
  IFACE_FLOW_DISABLED_EV          = 18,
  IFACE_ROUTEABLE_EV              = 19,
  IFACE_ADDR_CHANGED_EV           = 20,
  IFACE_MTU_CHANGED_EV            = 21,
  IFACE_DELETE_EV                 = 22,
  IFACE_IPFLTR_UPDATED_EV         = 23,
  IFACE_PRI_PHYS_LINK_CHANGED_EV  = 24,
  IFACE_PREFIX_UPDATE_EV          = 25, /* IPv6 specific event */
  IFACE_VALID_RA_EV               = 26, /* IPv6 specific event */
  IFACE_ADDR_FAMILY_CHANGED_EV    = 27,
  IFACE_MT_REQUEST_EV             = 28,
  IFACE_MCAST_REGISTER_SUCCESS_EV = 29,
  IFACE_MCAST_REGISTER_FAILURE_EV = 30,
  IFACE_MCAST_DEREGISTERED_EV     = 31,
  IFACE_BEARER_TECH_CHANGED_EV    = 32,
  IFACE_QOS_AWARE_SYSTEM_EV       = 33,
  IFACE_QOS_UNAWARE_SYSTEM_EV     = 34,
  IFACE_FLOW_ADDED_EV             = 35,
  IFACE_FLOW_DELETED_EV           = 36,

  IFACE_ENABLE_HDR_REV0_RATE_INERTIA_SUCCESS_EV = 37,
  IFACE_ENABLE_HDR_REV0_RATE_INERTIA_FAILURE_EV = 38,

  IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV         = 39,
  IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV         = 40,
  IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SESSION_CHANGED_EV = 41,

  IFACE_IPV6_PRIV_ADDR_GENERATED_EV                   = 42,
  IFACE_IPV6_PRIV_ADDR_DEPRECATED_EV                  = 43,
  IFACE_IPV6_PRIV_ADDR_EXPIRED_EV                     = 44,
  IFACE_IPV6_PRIV_ADDR_DELETED_EV                     = 45,
  IFACE_OUTAGE_NOTIFICATION_EV                        = 46,
  IFACE_707_NETWORK_SUPPORTED_QOS_PROFILES_CHANGED_EV = 47,
  IFACE_RF_CONDITIONS_CHANGED_EV                      = 48,
  IFACE_EXTENDED_IP_CONFIG_EV                         = 49,
  IFACE_LINGERING_EV                                  = 50,

  IFACE_MBMS_CONTEXT_ACT_SUCCESS_EV                   = 51,
  IFACE_MBMS_CONTEXT_ACT_FAILURE_EV                   = 52,
  IFACE_MBMS_CONTEXT_DEACT_SUCCESS_EV                 = 53,
  IFACE_MBMS_CONTEXT_DEACT_FAILURE_EV                 = 54,

  IFACE_AUTHENTICATING_EV                             = 55,
  IFACE_APP_PREEMPTED_EV                              = 56,
  IFACE_MCAST_STATUS_EV                               = 57,
  IFACE_ACTIVE_OUT_OF_USE_EV                          = 58,
  IFACE_FAST_DORMANCY_STATUS_EV                       = 59,
  IFACE_NET_INITIATED_QOS_AVAILABLE_EV                = 60,
  IFACE_ACTIVE_IN_USE_EV                              = 61,

  IFACE_EMBMS_TMGI_ACTIVATED_EV                       = 62,
  /**< Posted when a TMGI is succesfully activated */
  IFACE_EMBMS_TMGI_DEACTIVATED_EV                     = 63,
  /**< Posted when a TMGI is deactivated */
  IFACE_EMBMS_ACTIVE_TMGI_LIST_EV                     = 64,
  /**< Event to convey the currently active TMGI list */
  IFACE_EMBMS_AVAILABLE_TMGI_LIST_EV                  = 65,
  /**< Event to convey the currently available TMGI list */
  IFACE_EMBMS_WARNING_TMGI_LIST_EV                    = 66,
  /**< Event to convey the TMGI list which have been running in to decoding
       errors on the traffic channel and which may get deactivated if not
       recovered shortly */

  IFACE_IPV6_EXTERNAL_ADDR_DELETED_EV                 = 67,
  /**< Event to convey external IPv6 address is deleted. */
  IFACE_EMBMS_TMGI_ACTIVATE_DEACTIVATE_RESPONSE_EV    = 68,
  /**< Posted as response to activate/deactivate TMGI request */
  IFACE_REVERSE_IP_TRANSPORT_CONFIG_EV                = 69,
  /**< Posted when reverse IP transport is to be set up */

  IFACE_BEARER_TECH_EX_CHANGED_EV                     = 70,
  IFACE_HANDOFF_INIT_EV                               = 71,
  /**< Posted when handoff process is triggered by Modehandler */
  IFACE_HANDOFF_STATUS_SUCCESS_EV                     = 72,
  /**< Posted when handoff process is completed ie RATs have been swapped 
       and handoff is successful
  */
  
  IFACE_HANDOFF_STATUS_FAILURE_EV                    = 73,
  /**< Posted when handoff operation is not successful
  */
 
  IFACE_EMBMS_AVAILABLE_SAI_LIST_EV                   = 75,
  IFACE_POWERSAVE_FILTERING_MODE_CHANGE_EV            = 76,
  /**< Posted when power save filter mode of interface is changed
  */
  /**< Posted when IP configuration items have changed.
       This is an extension to the extended ip config event which only 
       denotes whether or not config is fetched.
       Also indicates what all config got updated.
  */
  IFACE_EXTENDED_IP_CONFIG_EX_EV                      = 77,
  IFACE_REVERSE_IP_TRANSPORT_IPFLTR_CHANGED_EV        = 78,
  /**< Posted when filters on the reverse IP transport interface are added or 
       deleted
  */
  IFACE_MODEM_PCO_INFO_UPDATE_EV                      = 79,
  /**< Posted by mode handler when modem reserved pco(protocol configuration 
       options) information is saved on the iface. PCO has APN, PCO container 
       ID and PCO message string and used by the network for authentication
       purpose.
  */
  IFACE_EMBMS_CONTENT_DESC_CONTROL_EV                 = 80,
  /**< Posted by mode handler to indicate eMBMS content description settings
  */
   IFACE_APN_PARAM_CHANGED_EV                         = 81,
  /**< Posted by mode handler when network updates the APN parameter 
       information. This information is saved on the iface.
  */
  IFACE_ALL_EV                                        = 82,
  IFACE_EVENT_MAX                                     = IFACE_ALL_EV + 1,

  /*-------------------------------------------------------------------------
    To make sure that libraries are not messed up when features are undefined,
    assigning an explicit value to FLOW_MIN_EV. Otherwise library will have
    one value for flow event and regular code will have another value
  -------------------------------------------------------------------------*/
  FLOW_MIN_EV                     = IFACE_EVENT_MAX,
  FLOW_NULL_EV                    = FLOW_MIN_EV,
  FLOW_ACTIVATING_EV              = FLOW_MIN_EV + 1,
  FLOW_ACTIVATED_EV               = FLOW_MIN_EV + 2,
  FLOW_SUSPENDING_EV              = FLOW_MIN_EV + 3,
  FLOW_SUSPENDED_EV               = FLOW_MIN_EV + 4,
  FLOW_RESUMING_EV                = FLOW_MIN_EV + 5,
  FLOW_GOING_NULL_EV              = FLOW_MIN_EV + 6,
  FLOW_CONFIGURING_EV             = FLOW_MIN_EV + 7,
  FLOW_TX_ENABLED_EV              = FLOW_MIN_EV + 8,
  FLOW_TX_DISABLED_EV             = FLOW_MIN_EV + 9,
  FLOW_RX_FLTR_UPDATED_EV         = FLOW_MIN_EV + 10,
  FLOW_MODIFY_ACCEPTED_EV         = FLOW_MIN_EV + 11,
  FLOW_MODIFY_REJECTED_EV         = FLOW_MIN_EV + 12,
  FLOW_PRIMARY_MODIFY_RESULT_EV   = FLOW_MIN_EV + 13,
  FLOW_INFO_CODE_UPDATED_EV       = FLOW_MIN_EV + 14,
  /*---------------------------------------------------------------------------
    Event represents that auxiliary information associated with
    Rx or TX filter spec associated with a flow has been changed by the network
  ---------------------------------------------------------------------------*/
  FLOW_FLTR_AUX_INFO_UPDATED_EV   = FLOW_MIN_EV + 15,
  FLOW_MAX_EV

} ps_iface_event_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_EXTENDED_INFO_CODE_ENUM_TYPE

DESCRIPTION
  Extended reason codes to pass additional information with the
  iface/flow/phys_link events.  If extended information is not available,
  the default value PS_EIC_NOT_SPECIFIED should be used.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_EIC_NOT_SPECIFIED = 0,

  PS_EIC_QOS_INTERNAL_MIN                     = 1,
  PS_EIC_QOS_INVALID_PARAMS                   = PS_EIC_QOS_INTERNAL_MIN + 2,
  PS_EIC_QOS_INTERNAL_INVALID_PARAMS          = PS_EIC_QOS_INVALID_PARAMS,
  PS_EIC_QOS_INTERNAL_CALL_ENDED              = PS_EIC_QOS_INTERNAL_MIN + 3,
  PS_EIC_QOS_INTERNAL_ERROR                   = PS_EIC_QOS_INTERNAL_MIN + 4,
  PS_EIC_QOS_INSUFFICIENT_LOCAL_RESOURCES     = PS_EIC_QOS_INTERNAL_MIN + 5,
  PS_EIC_QOS_TIMED_OUT_OPERATION              = PS_EIC_QOS_INTERNAL_MIN + 6,
  PS_EIC_QOS_INTERNAL_UNKNOWN_CAUSE_CODE      = PS_EIC_QOS_INTERNAL_MIN + 7,
  PS_EIC_QOS_INTERNAL_MODIFY_IN_PROGRESS      = PS_EIC_QOS_INTERNAL_MIN + 8,
  PS_EIC_QOS_MODEM_RESTART                    = PS_EIC_QOS_INTERNAL_MIN + 9,
  PS_EIC_QOS_INTERNAL_MAX,

  PS_EIC_QOS_NETWORK_MIN                      = 128,
  PS_EIC_QOS_NOT_SUPPORTED                    = PS_EIC_QOS_NETWORK_MIN + 1,
  PS_EIC_QOS_NOT_AVAILABLE                    = PS_EIC_QOS_NETWORK_MIN + 2,
  PS_EIC_QOS_NOT_GUARANTEED                   = PS_EIC_QOS_NETWORK_MIN + 3,
  PS_EIC_QOS_INSUFFICIENT_NET_RESOURCES       = PS_EIC_QOS_NETWORK_MIN + 4,
  PS_EIC_QOS_AWARE_SYSTEM                     = PS_EIC_QOS_NETWORK_MIN + 5,
  PS_EIC_QOS_UNAWARE_SYSTEM                   = PS_EIC_QOS_NETWORK_MIN + 6,
  PS_EIC_QOS_REJECTED_OPERATION               = PS_EIC_QOS_NETWORK_MIN + 7,
  PS_EIC_QOS_WILL_GRANT_WHEN_QOS_RESUMED      = PS_EIC_QOS_NETWORK_MIN + 8,
  PS_EIC_QOS_NETWORK_CALL_ENDED               = PS_EIC_QOS_NETWORK_MIN + 9,
  PS_EIC_QOS_NETWORK_SVC_NOT_AVAILABLE        = PS_EIC_QOS_NETWORK_MIN + 10,
  PS_EIC_QOS_NETWORK_L2_LINK_RELEASED         = PS_EIC_QOS_NETWORK_MIN + 11,
  PS_EIC_QOS_NETWORK_L2_LINK_REESTAB_REJ      = PS_EIC_QOS_NETWORK_MIN + 12,
  PS_EIC_QOS_NETWORK_L2_LINK_REESTAB_IND      = PS_EIC_QOS_NETWORK_MIN + 13,
  PS_EIC_QOS_NETWORK_UNKNOWN_CAUSE_CODE       = PS_EIC_QOS_NETWORK_MIN + 14,
  PS_EIC_QOS_NETWORK_DISJOINT_PROFILE_SET_SUGGESTED
                                              = PS_EIC_QOS_NETWORK_MIN + 15,
  PS_EIC_QOS_NETWORK_NULL_PROFILE_SUGGESTED   = PS_EIC_QOS_NETWORK_MIN + 16,
  PS_EIC_QOS_NETWORK_UE_NOT_AUTHORIZED        = PS_EIC_QOS_NETWORK_MIN + 17,
  PS_EIC_QOS_NETWORK_MAX,

  PS_EIC_NETWORK_MIN                          = 200,
  PS_EIC_NETWORK_NOT_SPECIFIED                = PS_EIC_NETWORK_MIN + 1,
  PS_EIC_NETWORK_BUSY                         = PS_EIC_NETWORK_MIN + 2,
  PS_EIC_NETWORK_RAB_FAILURE                  = PS_EIC_NETWORK_MIN + 3,
  PS_EIC_CLIENT_END                           = PS_EIC_NETWORK_MIN + 4,
  PS_EIC_NO_SRV                               = PS_EIC_NETWORK_MIN + 5,
  PS_EIC_CDMA_LOCK                            = PS_EIC_NETWORK_MIN + 6,
  PS_EIC_FADE                                 = PS_EIC_NETWORK_MIN + 7,
  PS_EIC_INTERCEPT                            = PS_EIC_NETWORK_MIN + 8,
  PS_EIC_REORDER                              = PS_EIC_NETWORK_MIN + 9,
  PS_EIC_REL_NORMAL                           = PS_EIC_NETWORK_MIN + 10,
  PS_EIC_REL_SO_REJ                           = PS_EIC_NETWORK_MIN + 11,
  PS_EIC_INCOM_CALL                           = PS_EIC_NETWORK_MIN + 12,
  PS_EIC_ALERT_STOP                           = PS_EIC_NETWORK_MIN + 13,
  PS_EIC_ACTIVATION                           = PS_EIC_NETWORK_MIN + 14,
  PS_EIC_MC_ABORT                             = PS_EIC_NETWORK_MIN + 15,
  PS_EIC_MAX_ACCESS_PROBE                     = PS_EIC_NETWORK_MIN + 16,
  PS_EIC_PSIST_NG                             = PS_EIC_NETWORK_MIN + 17,
  PS_EIC_UIM_NOT_PRESENT                      = PS_EIC_NETWORK_MIN + 18,
  PS_EIC_ACC_IN_PROG                          = PS_EIC_NETWORK_MIN + 19,
  PS_EIC_ACC_FAIL                             = PS_EIC_NETWORK_MIN + 20,
  PS_EIC_RETRY_ORDER                          = PS_EIC_NETWORK_MIN + 21,
  PS_EIC_CCS_NOT_SUPPORTED_BY_BS              = PS_EIC_NETWORK_MIN + 22,
  PS_EIC_NO_RESPONSE_FROM_BS                  = PS_EIC_NETWORK_MIN + 23,
  PS_EIC_REJECTED_BY_BS                       = PS_EIC_NETWORK_MIN + 24,
  PS_EIC_INCOMPATIBLE                         = PS_EIC_NETWORK_MIN + 25,
  PS_EIC_ACCESS_BLOCK                         = PS_EIC_NETWORK_MIN + 26,
  PS_EIC_ALREADY_IN_TC                        = PS_EIC_NETWORK_MIN + 27,
  PS_EIC_USER_CALL_ORIG_DURING_GPS            = PS_EIC_NETWORK_MIN + 28,
  PS_EIC_USER_CALL_ORIG_DURING_SMS            = PS_EIC_NETWORK_MIN + 29,
  PS_EIC_USER_CALL_ORIG_DURING_VOICE_CALL     = PS_EIC_NETWORK_MIN + 30,
  PS_EIC_REDIR_OR_HANDOFF                     = PS_EIC_NETWORK_MIN + 31,
  PS_EIC_ACCESS_BLOCK_ALL                     = PS_EIC_NETWORK_MIN + 32,
  PS_EIC_IS707B_MAX_ACC                       = PS_EIC_NETWORK_MIN + 33,
  PS_EIC_NO_CDMA_SRV                          = PS_EIC_NETWORK_MIN + 34,
  PS_EIC_CD_GEN_OR_BUSY                       = PS_EIC_NETWORK_MIN + 35,
  PS_EIC_CD_BILL_OR_AUTH                      = PS_EIC_NETWORK_MIN + 36,
  PS_EIC_CHG_HDR                              = PS_EIC_NETWORK_MIN + 37,
  PS_EIC_EXIT_HDR                             = PS_EIC_NETWORK_MIN + 38,
  PS_EIC_HDR_NO_SESSION                       = PS_EIC_NETWORK_MIN + 39,
  PS_EIC_COLLOC_ACQ_FAIL                      = PS_EIC_NETWORK_MIN + 40,
  PS_EIC_HDR_ORIG_DURING_GPS_FIX              = PS_EIC_NETWORK_MIN + 41,
  PS_EIC_HDR_CS_TIMEOUT                       = PS_EIC_NETWORK_MIN + 42,
  PS_EIC_HDR_RELEASED_BY_CM                   = PS_EIC_NETWORK_MIN + 43,
  PS_EIC_OTASP_COMMIT_IN_PROG                 = PS_EIC_NETWORK_MIN + 44,
  PS_EIC_NO_HYBR_HDR_SRV                      = PS_EIC_NETWORK_MIN + 45,
  PS_EIC_HDR_NO_LOCK_GRANTED                  = PS_EIC_NETWORK_MIN + 46,
  PS_EIC_HOLD_OTHER_IN_PROG                   = PS_EIC_NETWORK_MIN + 47,
  PS_EIC_HDR_FADE                             = PS_EIC_NETWORK_MIN + 48,
  PS_EIC_HDR_ACC_FAIL                         = PS_EIC_NETWORK_MIN + 49,
  PS_EIC_THERMAL_EMERGENCY                    = PS_EIC_NETWORK_MIN + 50,
  PS_EIC_CALL_ORIG_THROTTLED                  = PS_EIC_NETWORK_MIN + 51,
  PS_EIC_OFFLINE                              = PS_EIC_NETWORK_MIN + 52,
  PS_EIC_EMERGENCY_MODE                       = PS_EIC_NETWORK_MIN + 53,
  PS_EIC_PHONE_IN_USE                         = PS_EIC_NETWORK_MIN + 54,
  PS_EIC_INVALID_MODE                         = PS_EIC_NETWORK_MIN + 55,
  PS_EIC_INVALID_SIM_STATE                    = PS_EIC_NETWORK_MIN + 56,
  PS_EIC_NO_COLLOC_HDR                        = PS_EIC_NETWORK_MIN + 57,
  PS_EIC_CALL_CONTROL_REJECTED                = PS_EIC_NETWORK_MIN + 58,
  PS_EIC_NETWORK_MAX

} ps_extended_info_code_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_NET_DOWN_REASON_TYPE

DESCRIPTION
  Enum type to determine the network going down reason.
  The 26 onwards values are taken from the 3GPP document and mapped
  corresponding to the values defined in the doc.
  3GPP TS 24.008 version 3.5.0 Release 1999
  The values from 500 onwards do NOT map to the 3GPP spec.
  The net down reasons have the following format:

  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |          Type                 |             Reason            |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  The upper two bytes represent the type of error.
  The type is 0 for all legacy net down reason such as CDMA, GSM, HDR, EAP
  and IPSEC
  The type is 1 for all MIP error codes.
  The type is 6 for all 3GPP error codes.
  The type is 7 for all PPP error codes.
  The type is 8 for all eHRPD error codes.
  The type is 9 for all IPv6 error codes
  The lower two bytes represent the actual net down reason.
---------------------------------------------------------------------------*/
#define TYPE_REASON_VAL(type, reason) ((0xFFFF0000 & (type << 16)) | (0x0000FFFF & reason))
typedef enum
{
  PS_NET_DOWN_REASON_NOT_SPECIFIED               = 0,
  /**< The reason is posted by the UE when a more appropriate reason is not 
       known.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry */
  PS_NET_DOWN_REASON_OPERATOR_DETERMINED_BARRING = TYPE_REASON_VAL(6,8),
  /**< The reason code is posted by the MME to indicate operator has barred 
       the UE. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Temporary if PDN throttling applies. Permanent
       otherwise. 
       Recovery Mechanism:
       If PDN throttling is enabled, DSS clients can obtain information about
       PDN throttling via invoking a DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO.
       QMI clients can use QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is
       throttled, the client shall wait for the corresponding time period
       before retrying the call. If the PDN is not throttled, the client
       may retry immediately.
       If PDN throttling does not apply, contact network provider */
  PS_NET_DOWN_REASON_NAS_SIGNALLING_ERROR = TYPE_REASON_VAL(6, 14),
  /**< The reason code is posted by the NAS to inform higher layers 
       that deactivation is triggered because of NW.
       To differentiate this external cause with internal 
       cause PS_NET_DOWN_REASON_PPD_UNKNOWN_REASON code added _EXT_ to it
       Failure Type: Temporary
       Recovery Mechanism: Client may retry   */  
	   
  PS_NET_DOWN_REASON_LLC_SNDCP_FAILURE           = TYPE_REASON_VAL(6,25),
  /**< This reason is posted to indicate that the network cannot provide the 
       requested service and PDP context is deactivated because of LLC or
       SNDCP failure
       Failure Type: Temporary
       Recovery Mechanism: Client may retry */
  PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES   = TYPE_REASON_VAL(6,26),
  /**< This reason is posted to indicate that the network cannot provide the 
       requested service due to insufficient resources.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry */
  PS_NET_DOWN_REASON_UNKNOWN_APN              = TYPE_REASON_VAL(6,27),
  /**< This reason is posted to specify that the APN was required and not 
       specified or APN could not be resolved. In LTE mode of operation,
       this is a PDN throttling cause code, meaning the UE may endup
       throttling further requests to the same APN.
       Failure Type: Permanent.
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If PDN throttling does not apply, client may retry with a different APN
       name or profile. On further failure with the same cause code, check with
       network provider. */
  PS_NET_DOWN_REASON_UNKNOWN_PDP              = TYPE_REASON_VAL(6,28),
  /**< The reason is posted by the network to indicate that the PDN type 
       was not recognized.
       Failure Type: Permanent
       Recovery Mechanism: Client may retry with a different IP type */
  PS_NET_DOWN_REASON_AUTH_FAILED              = TYPE_REASON_VAL(6,29),
  /**< The reason is posted when authentication fails. In LTE mode of operation, 
       this is a PDN throttling cause code, meaning the UE may endup throttling
       further requests to the same APN.
       Failure Type: Permanent
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately. Client shall retry
       with a different set of authentication parameters/algorithm */
  PS_NET_DOWN_REASON_GGSN_REJECT              = TYPE_REASON_VAL(6,30),
  /**< The reason is posted when the request was rejected by Serving GW or 
       PDN GW. In LTE mode of operation, this is a PDN throttling cause code, 
       meaning the UE may endup throttling further requests to the same APN.
       Failure Type: Temporary 
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.*/
  PS_NET_DOWN_REASON_ACTIVATION_REJECT        = TYPE_REASON_VAL(6,31),
  /**< The reason is posted when the request is rejected by the network 
       due to unspecified reasons. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately. */
  PS_NET_DOWN_REASON_OPTION_NOT_SUPPORTED     = TYPE_REASON_VAL(6,32),
  /**< The reason is posted when UE requested a service not supported by the 
       PLMN. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Temporary. 
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       Client may also retry after change in PLMN */
  PS_NET_DOWN_REASON_OPTION_UNSUBSCRIBED      = TYPE_REASON_VAL(6,33),
  /**< The reason is posted when UE requested a service option for which it 
       has no subscription. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN. 
       Failure Type: Temporary if PDN throttling applies. Permananet otherwise. 
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If PDN throttling does not apply, check device configuration or check with network
       provider */
  PS_NET_DOWN_REASON_OPTION_TEMP_OOO          = TYPE_REASON_VAL(6,34),
  /**< This reason is posted when the network is temporarily out of resources 
       to service the request. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.*/
  PS_NET_DOWN_REASON_NSAPI_ALREADY_USED       = TYPE_REASON_VAL(6,35),
  /**< The reason is posted to indicate that PTI (Procedure Transaction Identifier) 
       used in the request is already active via another UE requested procedure.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry after some time*/
  PS_NET_DOWN_REASON_REGULAR_DEACTIVATION     = TYPE_REASON_VAL(6,36),
  /**< This reason is posted by the network to indiate a regualr release of 
       bearer resources.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry */
  PS_NET_DOWN_REASON_QOS_NOT_ACCEPTED         = TYPE_REASON_VAL(6,37),
  /**< This reason is posted by the network to indicate that the QoS 
       requested by the UE could not be accepted.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry with different QoS */
  PS_NET_DOWN_REASON_NETWORK_FAILURE          = TYPE_REASON_VAL(6,38),
  /**< This reason is posted when an error occurs in the network. 
       In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.*/
  PS_NET_DOWN_REASON_UMTS_REATTACH_REQ        = TYPE_REASON_VAL(6,39),
  /**< This reason is posted by the network to request for bearer 
       reactivation. This code may be posted during network congestion.
       Failure Type: Temporary
       Recovery Mechanism: Client must retry after some time */
  PS_NET_DOWN_REASON_FEATURE_NOT_SUPPORTED    = TYPE_REASON_VAL(6,40),
  /**< This reason is posted to indicate that the Feature is not 
       supported by the network
       Failure Type: Permanent 
       Recovery Mechanism: Client shall retry after making sure the feature is
       supported by network*/
  PS_NET_DOWN_REASON_TFT_SEMANTIC_ERROR       = TYPE_REASON_VAL(6,41),
  /**< The reason is posted by the network to indicate semantic error(s) 
       in specifying TFT operation included in the request.
       Failure Type: Permanent
       Recovery Mechanism: Client may retry with different TFT */
  PS_NET_DOWN_REASON_TFT_SYNTAX_ERROR         = TYPE_REASON_VAL(6,42),
  /**< The reason is posted by the network to indicate syntactic error(s) 
       in specifying TFT operation included in the request.
       Failure Type: Permanent 
       Recovery Mechanism: Client may retry with different TFT */
  PS_NET_DOWN_REASON_UNKNOWN_PDP_CONTEXT      = TYPE_REASON_VAL(6,43),
  /**< The reason is posted when the bearer identity (or linked bearer
       identity) in the request is invalid (or inactive).
       Failure Type: Permanent
       Recovery Mechanism: Client may retry. */
  PS_NET_DOWN_REASON_FILTER_SEMANTIC_ERROR    = TYPE_REASON_VAL(6,44),
  /**< The reason is posted by the network to indicate semantic error(s) 
       in specifying packet filter(s) associated with a TFT.
       Failure Type: Permanent
       Recovery Mechanism: Client may retry with different TFT */
  PS_NET_DOWN_REASON_FILTER_SYNTAX_ERROR      = TYPE_REASON_VAL(6,45),
  /**< The reason is posted by the network to indicate syntactic error(s) 
       in specifying packet filter(s) associated with a TFT.
       Failure Type: Permanent
       Recovery Mechanism: Client may retry with different TFT */
  PS_NET_DOWN_REASON_PDP_WITHOUT_ACTIVE_TFT   = TYPE_REASON_VAL(6,46),
  /**< The reason is posted by the network when UW requested more than 
       one PDP connection without TFT.
       Failure Type: Permanent
       Recovery Mechanism: Client may retry with a TFT */
  PS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED       = TYPE_REASON_VAL(6,50),
  /**< The reason is posted when the network supports IPV4 PDP type only. 
       IPV6 is not allowed. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Permanent
       Recovery Mechanism: 
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately. PDP type during
       retry must be set to IPV4. */
  PS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED       = TYPE_REASON_VAL(6,51),
  /**< The reason is posted when the network supports IPV6 PDP type only. 
       IPV4 is not allowed. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Permanent
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately. PDP type during
       retry must be set to IPV6 */
  PS_NET_DOWN_REASON_SINGLE_ADDR_BEARER_ONLY  = TYPE_REASON_VAL(6,52),
  /**< The reason is posted when the network supports single address 
       bearers only, meaning dual IP bearers are not supported.      
       Failure Type: Permanent
       Recovery Mechanism: Client may retry the request specifying
       single IP bearer. */
  PS_NET_DOWN_REASON_ESM_INFO_NOT_RECEIVED    = TYPE_REASON_VAL(6,53),
  /**< The reason is posted by the network to indicate that the PDN 
       connection request was rejected because ESM information was
       not received.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry */
  PS_NET_DOWN_REASON_PDN_CONN_DOES_NOT_EXIST  = TYPE_REASON_VAL(6,54),
  /**< The reason is posted by the network during handover from a non-3GPP 
       network to indicate that the MME does not have any information
       regarding the requested PDN connection.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry */
  PS_NET_DOWN_REASON_MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED = TYPE_REASON_VAL(6,55),
  /**< The reason is posted by the network to indicate that the UE is already
       connected to the requested APN via another PDN/PDN connection. 
       Failure Type: Permanent
       Recovery Mechanism: NV item 67248 allows UE to be configured to send
       multiple PDN connection requests to the same APN. Check the NV to ensure
       that it is in sync with the network capabilities. */
  PS_NET_DOWN_REASON_COLLISION_WITH_NW_INIT_REQ = TYPE_REASON_VAL(6,56),
  /**< The reason is posted by the network to indicate that the network has 
       already initiated the activation, modification or deactivation of bearer
       resources which was requested by the UE. 
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.  */
  PS_NET_DOWN_REASON_UNSUPPORTED_QCI_VALUE = TYPE_REASON_VAL(6,59),
  /**< The reason is posted by the network if the QCI indicated in the UE 
       request cannot be supported.
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately with a different
       QCI value */ 
  PS_NET_DOWN_REASON_BEARER_HANDLING_NOT_SUPPORTED  = TYPE_REASON_VAL(6,60),
  /**< The reason is posted by the network to indicate that the procedure 
       requested by the UE was rejected because the bearer handling is not
       supported. 
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.  */ 
  PS_NET_DOWN_REASON_MAX_ACTIVE_PDP_CONTEXT_REACHED = TYPE_REASON_VAL(6,65),
  /**< The reason is posted by the network to indicate that maximum number 
       of active PDP contexts per UE has been reached. 
       Failure Type: Temporary
       Recovery Mechanism: Client may retry after it brings down one of the active PDNs */
  PS_NET_DOWN_REASON_UNSUPPORTED_APN_IN_CURRENT_PLMN = TYPE_REASON_VAL(6,66),
  /**< The reason is posted by the network is to indicate that the APN is 
       not supported in the current RAT and PLMN.
       Failure Type: Permanent
       Recovery Mechanism: client may retry after a change in either PLMN or RAT. */
  PS_NET_DOWN_REASON_INVALID_TRANSACTION_ID       = TYPE_REASON_VAL(6,81),
  /**< The reason is posted by the network to indicate that the PTI used in 
       the request is unassigned or reserved.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry */
  PS_NET_DOWN_REASON_MESSAGE_INCORRECT_SEMANTIC   = TYPE_REASON_VAL(6,95),
  /**< The reason is posted by the network to indicate receipt of an 
       invalid message. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Permanent.
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If throttling does not apply, client may not retry. Check with eureka
       for resolution. */
  PS_NET_DOWN_REASON_INVALID_MANDATORY_INFO       = TYPE_REASON_VAL(6,96),
  /**< The reason is posted by the network to indicate receipt of a message 
       with semantic error in a mandatory information element. In LTE mode
       of operation, this is a PDN throttling cause code, meaning the UE may
       endup throttling further requests to the same APN.
       Failure Type: Temporary if PDN throttling applies. Permanent otherwise.
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If PDN throttling does not apply, client may not retry. Check with eureka
       for resolution */
  PS_NET_DOWN_REASON_MESSAGE_TYPE_UNSUPPORTED     = TYPE_REASON_VAL(6,97),
  /**< The reason is posted by the network to indicate receipt of a message 
       that is either undefined or defined but not implemented by the
       equipment sending this ESM cause. In LTE mode
       of operation, this is a PDN throttling cause code, meaning the UE may
       endup throttling further requests to the same APN.
       Failure Type: Temporary/Permanent
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If throttling does not apply, client may not retry. Check with network
       provider. */
  PS_NET_DOWN_REASON_MSG_TYPE_NONCOMPATIBLE_STATE = TYPE_REASON_VAL(6,98),
  /**< The reason is posted by the network to indicate receipt of a message 
       type that cannot be handled in the current network protocol state.
       In LTE mode of operation, this is a PDN throttling cause code, meaning
       the UE may endup throttling further requests to the same APN.
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.*/
  PS_NET_DOWN_REASON_UNKNOWN_INFO_ELEMENT         = TYPE_REASON_VAL(6,99),
  /**< The reason is posted by the network to indicate receipt of a message 
       that includes and information element that is either not defined or
       defined but not implemented by the equipment sending the ESM cause.
       In LTE mode of operation, this is a PDN throttling cause code, meaning
       the UE may endup throttling further requests to the same APN.
       Failure Type: Temporary/Permanent
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If PDN throttling does not apply, client may retry after a change in PLMN.  */
  PS_NET_DOWN_REASON_CONDITIONAL_IE_ERROR         = TYPE_REASON_VAL(6,100),
  /**< The reason is posted by the netowrk to indicate receipt of a message 
       that includes a syntactically incorrect information element. This
       message is ignored by the network.
       In LTE mode of operation, this is a PDN throttling cause code, meaning
       the UE may endup throttling further requests to the same APN.
       Failure Type: Temporary/Permanent
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If PDN throttling does not apply, client may not retry. Contact eureka for
       resolution */
  PS_NET_DOWN_REASON_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE = TYPE_REASON_VAL(6,101),
  /**< The reason is posted by the network to indicate receipt of a message 
       that cannot be handled in the current network protocol state.
       In LTE mode of operation, this is a PDN throttling cause code, meaning
       the UE may endup throttling further requests to the same APN.
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.*/
  PS_NET_DOWN_REASON_PROTOCOL_ERROR           = TYPE_REASON_VAL(6,111),
  /**< The reason is posted by the network to indicate a protocol error 
       when no other error applies. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Temporary if PDN throttling applies. Permanent otherwise.
       Recovery Mechanism:                                               .
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If PDN throttling does not apply, client may not retry. Check with network
       provider */
  PS_NET_DOWN_REASON_APN_TYPE_CONFLICT       = TYPE_REASON_VAL(6,112),
  /**< The reason is posted by the network to indicate that the EPS bearer 
       has an APN restriction value that cannot be used in conjunction with
       existing EPS bearers. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Temporary if PDN throttling applies. Permanent otherwise.
       Recovery Mechanism:
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If PDN throttling does not apply, client may not retry. Check device
       configuration */
  PS_NET_DOWN_REASON_INVALID_PCSCF_ADDR      = TYPE_REASON_VAL(6,113),
  /**< The reason is posted by the UE when it tears down a PDN for not 
       receiving a P-CSCF address that was mandatory.
       Failure Type: Permanent
       Recovery Mechanism: Client may retry with a different profile */

   PS_NET_DOWN_REASON_INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN = TYPE_REASON_VAL(6,114),
  /**< 
      This reason is posted by the UE when the data call is brought down
      due to insufficient resources to bring up another prioritized data call. 
      In LTE mode of operation this reason will be posted while bringing down an
      ongoing PDN connection, when the device runs out of bearer resources to bring
      up a priortized PDN connection
            
      Failure Type       : Permanent Failure
      Recovery Mechanism : Client may retry after the higher priority 
                           data call is brought down
  */
  PS_NET_DOWN_REASON_EMM_ACCESS_BARRED       = TYPE_REASON_VAL(6,115),
  /**< The reason is returned when there is RRC failure of NAS signalling 
       which is results in connection establishment rejected by the network
       Failure Type: Temporary 
       Recovery Mechanism: Client may retry after RRC has been established */
  
  PS_NET_DOWN_REASON_EMERGENCY_IFACE_ONLY       = TYPE_REASON_VAL(6,116),
  /**< This reason is posted to indicate that iface can support Emergency
       iface Only
       Failure Type: Temporary
       Recovery Mechanism: UE can re-try after some time */
  
  PS_NET_DOWN_REASON_IFACE_MISMATCH             = TYPE_REASON_VAL(6,117),
  /**< This reason is posted to indicate iface mismatch between requested
       iface and received iface
       Failure Type:Temporary 
       Recovery Mechanism: UE can use re-try using correct iface */
  
  PS_NET_DOWN_REASON_COMPANION_IFACE_IN_USE     = TYPE_REASON_VAL(6,118),
  /**< This reason is posted to indicate that iface can support Emergency
       iface Only
       Failure Type: Temporary
       Recovery Mechanism: UE can use another profile which matches IP type */
  
  PS_NET_DOWN_REASON_IP_ADDRESS_MISMATCH         = TYPE_REASON_VAL(6,119),
  /**< This reason is posted to indicate IP address mismatch between 
       Profile and PDN Context
       Failure Type:  Temporary
       Recovery Mechanism: UE can retry after some time */

  PS_NET_DOWN_REASON_IFACE_AND_POL_FAMILY_MISMATCH = TYPE_REASON_VAL(6,120),
  /**< This reason is posted to indicate that IFace and Policy IP type
       do not match
       Failure Type:  Temporary
       Recovery Mechanism: UE can retry after some time with another profile */
  PS_NET_DOWN_REASON_EMM_ACCESS_BARRED_INFINITE_RETRY       = TYPE_REASON_VAL(6,121),
  /**< The reason is returned when there is RRC failure of NAS signalling 
       which is results in connection establishment rejected by the network with
       an access barring timer value set to 20 ms
       Failure Type: Temporary 
       Recovery Mechanism: Client may retry after RRC has been established */

  PS_NET_DOWN_REASON_AUTH_FAILURE_ON_EMERGENCY_CALL  = TYPE_REASON_VAL(6,122),
  /**< This reason is returned when active data calls are brought down
       because authentication failure occured on emergency call.
       Data calls can be brought up after Emergency call has ended.
       Failure Type       : Temporary 
       Recovery Mechanism : UE can retry after Emergency call has ended*/
  
PS_NET_DOWN_REASON_INVALID_DNS_ADDR      = TYPE_REASON_VAL(6,123),
  /**< The reason is posted by the UE when it tears down a PDN for not 
       receiving a DNS address that was mandatory.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry with a different profile */

  PS_NET_DOWN_REASON_INVALID_PCSCF_DNS_ADDR      = TYPE_REASON_VAL(6,124),
  /**< The reason is posted by the UE when it tears down a PDN for not 
       receiving either a PCSCF or a DNS address one of them being mandatory.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry with the same profile */

  PS_NET_DOWN_REASON_TEST_LOOPBACK_MODE_A_OR_B_ENABLED = TYPE_REASON_VAL(6,125),
  /**< The reason is posted by the UE when when network indicates UE is loopback 
       test mode A or B.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry with the same profile once network
       deactivates loopback mode*/
  PS_NET_DOWN_REASON_EMM_ACCESS_BARRED_EAB       = TYPE_REASON_VAL(6,126),
  /**< The reason is returned when there is RRC failure of NAS signalling 
       which is results in connection establishment rejected by the network
       Failure Type: Temporary 
       Recovery Mechanism: Client may retry after RRC has been established */
  /* RESERVED  = TYPE_REASON_VAL(2,200) */

  PS_NET_DOWN_REASON_INTERNAL_MIN                 = TYPE_REASON_VAL(2,200),
  /**< Reserved for bound checks. Unused by clients */
  PS_NET_DOWN_REASON_INTERNAL_ERROR               = TYPE_REASON_VAL(2,201),
    /**< 
        This error code is returned when data call is brought down due  
        to some unspecified internal error
        Failure Type       : Permanent Failure
        Recovery Mechanism : Unknown
    */
  PS_NET_DOWN_REASON_INTERNAL_CALL_ENDED          = TYPE_REASON_VAL(2,202),
    /**< 
        This error code is returned when call was ended by internal clients  
        Failure Type       : Temporary Failure
        Recovery Mechanism : Client may retry after some time
    */
  PS_NET_DOWN_REASON_INTERNAL_UNKNOWN_CAUSE_CODE  = TYPE_REASON_VAL(2,203),
  /**< 
        This error code is returned when reason for the call end is unknown.  
        Failure Type       : Permanent Failure
        Recovery Mechanism : Unknown
    */
  PS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE           = TYPE_REASON_VAL(2,204),
    /**< 
        This error code is returned when data call is brought down due  
	to some unspecified unknown error
        Failure Type       : Permanent Failure
        Recovery Mechanism : Unknown
    */
  PS_NET_DOWN_REASON_CLOSE_IN_PROGRESS            = TYPE_REASON_VAL(2,205),
  /**< 
        This error code is returned when a data call teardown is in progress
        Failure Type       : Temporary Failure
        Recovery Mechanism : Client may retry once call end notification is
        returned when tear down is complete.
    */
  PS_NET_DOWN_REASON_NW_INITIATED_TERMINATION     = TYPE_REASON_VAL(2,206),
  /**< 
        This error code is returned when data call is brought down by 
		    the network
        Failure Type       : Permanent Failure
        Recovery Mechanism : Unknown
    */
  PS_NET_DOWN_REASON_APP_PREEMPTED                = TYPE_REASON_VAL(2,207),
  /**< 
        This error code is returned when another application preempts
        the data call.
        Failure Type       : Temporary Failure
        Recovery Mechanism : Client can retry when other application
        stops using the data call.
    */
  PS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_DISALLOWED = TYPE_REASON_VAL(2,208),
    /**< 
        This error code is returned when V4 PDN is in throttled state 
        due to network providing only V6 address e.g during the previous  
        VSNCP bringup (subs_limited_to_v6). The time for which the IPv4  
        PDN is throttled is determined by the IPv4 throttling timers  
        maintained in the profile
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after the V6 
                             interface is brought down and the V4 
                             throttling timer expires
    */
  PS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_THROTTLED  = TYPE_REASON_VAL(2,209),
    /**< 
        This error code is returned when V4 PDN is in throttled state 
        due to previous VSNCP bringup failure(s). The time for which the  
        IPv4 PDN is throttled is determined by the IPv4 throttling 
        timers maintained in the profile
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after V4 throttling 
                             timer expires
    */
  PS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_DISALLOWED = TYPE_REASON_VAL(2,210),
    /**< 
        This error code is returned when V6 PDN is in throttled state 
        due to network providing only V4 address during the previous  
        VSNCP bringup (subs_limited_to_v4). The time for which the IPv6  
        PDN is throttled is determined by the IPv6 throttling timers  
        maintained in the profile
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after the V4 
                             interface is brought down and the V6 
                             throttling timer expires
    */
  PS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_THROTTLED  = TYPE_REASON_VAL(2,211),
    /**< 
        This error code is returned when V6 PDN is in throttled state 
        due to previous VSNCP bringup failure(s). The time for which the  
        IPv6 PDN is throttled is determined by the IPv6 throttling  
        timers maintained in the profile
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after V6 throttling 
                             timer expires 
    */
  PS_NET_DOWN_REASON_MODEM_RESTART                = TYPE_REASON_VAL(2,212),
  /**< 
        This error code is returned when data call is brought 
		down due to modem restart
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after modem restart 
						     is complete.
    */
  PS_NET_DOWN_REASON_PDP_PPP_NOT_SUPPORTED        = TYPE_REASON_VAL(2,213),
    /**< 
        This error code is returned when PDP PPP calls are not supported        
        Failure Type       : Temporary Failure
        Recovery Mechanism : Clients may need to change Profile PDP type and retry
    */
  PS_NET_DOWN_REASON_UNPREFERRED_RAT              = TYPE_REASON_VAL(2,214),
    /**< 
        This error code is returned when data call is brought down since  
        the RAT on which the data call is attempted/connected is no  
        longer the preferred RAT
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination on the new preferred  
                             RAT reported by Data System Determination 
                             module
    */
  PS_NET_DOWN_REASON_PHYS_LINK_CLOSE_IN_PROGRESS  = TYPE_REASON_VAL(2,215),
    /**< 
        This error code is returned when data call bring up is rejected  
        because physical link is in the process of cleanup
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_APN_PENDING_HANDOVER  = TYPE_REASON_VAL(2,216),
     /**<
        This error code is returned when interface bring up is attempted for an APN 
        that is yet to be handed over to target RAT
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
     */      
  PS_NET_DOWN_REASON_PROFILE_BEARER_INCOMPATIBLE  = TYPE_REASON_VAL(2,217),
     /**< 
        This error code is returned when APN bearer type in the profile does not match Preferred
        network mode
        Failure Type       : : Temporary Failure
        Recovery Mechanism : Clients may need to change APN bearer type and retry
     */
     /* Error codes for calls ended by mode handler/EPC */
  PS_NET_DOWN_REASON_INTERNAL_MMGSDI_CARD_EVT         = TYPE_REASON_VAL(2,218),
    /**< 
        This error code is returned when data call is brought down  
        because card got refreshed/removed
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time. In 
                             case of card removal, expectation is that 
                             card has been reinserted into device
    */
  PS_NET_DOWN_REASON_INTERNAL_LPM_OR_PWR_DOWN         = TYPE_REASON_VAL(2,219),
    /**< 
        This error code is returned when data call is brought down  
        because device is going into lower power mode or powering down
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after device comes  
                             out of lower power mode or powers up
    */
  PS_NET_DOWN_REASON_INTERNAL_APN_DISABLED            = TYPE_REASON_VAL(2,220),
  /**< Alias to PS_NET_DOWN_REASON_APN_DISABLED. Do not publish in DSS/QMI */
  PS_NET_DOWN_REASON_APN_DISABLED                     
                                    = PS_NET_DOWN_REASON_INTERNAL_APN_DISABLED,
    /**< 
        This error code is returned when APN is disabled in card / 
        profile
        Failure Type       : Permanent Failure 
        Recovery Mechanism : Retry call origination after enabling APN  
                             in card/profile
    */
  PS_NET_DOWN_REASON_INTERNAL_MPIT_EXPIRED            = TYPE_REASON_VAL(2,221),
    /**< 
        This error code is returned when data call is brought down  
        because maximum PPP inactivity timer expired
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_INTERNAL_IPV6_ADDR_TRANSFER_FAILED  = TYPE_REASON_VAL(2,222),
    /**<
        This error code is returned when the transfer of secondary
        IPv6 address from srat to trat is failed during handoff.
        Failure Type       : Temporary failure
        Recovery Mechanism : Retry call origination immediately on
	    					the current system.
    */
  PS_NET_DOWN_REASON_INTERNAL_TRAT_SWAP_FAILED        = TYPE_REASON_VAL(2,223),
    /**<
        This error code is returned when the swap rat (trat <-> srat)
        fails after successful bringup of trat during handoff.
        Failure Type       :  Temporary Failure
        Recovery Mechanism :  Retry call origination immediately on
	    					  the current system.
    */

  PS_NET_DOWN_REASON_INTERNAL_EHRPD_TO_HRPD_FALLBACK  = TYPE_REASON_VAL(2,224),
    /**< 
        This error code is returned when data call is brought down  
        because device falls back from eHRPD to HRPD (not because of  
        OOS on eHRPD but due to operator/spec driven eHRPD to HRPD  
        fallback requirements)
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after Data System  
                             Determination module reportes preferred 
                             system as HRPD
    */
  PS_NET_DOWN_REASON_MANDATORY_APN_DISABLED           = TYPE_REASON_VAL(2,225),
    /**< 
        This error code is returned when any mandatory APN is disabled, 
        and MinApnList Disallow call config item is set to TRUE in 
        device
        Failure Type       : Permanent Failure 
        Recovery Mechanism : Retry call origination after enabling 
                             Mandatory APN(s)
    */
  PS_NET_DOWN_REASON_MIP_CONFIG_FAILURE               = TYPE_REASON_VAL(2,226),
    /**< 
        This error code is returned when UE is in MIP Only config (QCMIP 
        =2) but MIP config fails on call bring up due to incorrect  
        provisioning
        Failure Type       : Permanent Failure 
        Recovery Mechanism : Retry call origination after correctly 
                             provisioning the device with MIP 
                             information
    */
  PS_NET_DOWN_REASON_INTERNAL_PDN_INACTIVITY_TIMER_EXPIRED  = TYPE_REASON_VAL(2,227),
     /**<
        This error code is returned when pdn inactivity timer expired due to no data transmission 
        in a configurable duration of time. 

        Failure Type : Temporary Failure

        Recovery Mechanism : Retry call origination immediately since it is brought 
        down due to pdn inactivity timer expiration.

     */ 

  PS_NET_DOWN_REASON_MAX_V4_CONNECTIONS               = TYPE_REASON_VAL(2,228),
    /**< 
        This error code is returned when an IPv4 data call bring up is rejected
        since UE already maintains the allotted maximum number of IPv4 data
        connections
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after disconnecting existing
                             IPv4 call(s)
    */

  PS_NET_DOWN_REASON_MAX_V6_CONNECTIONS             = TYPE_REASON_VAL(2,229),
     /**<
      This error code is returned when an IPv6 data call bring up is rejected
      since UE already maintains the allotted maximum number of IPv6 data
      connections
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination after disconnecting existing
                           IPv6 call(s)
  */

  PS_NET_DOWN_REASON_APN_MISMATCH                   = TYPE_REASON_VAL(2,230),
  /**< 
      This error code is returned when a new PDN bring up is rejected
      during interface selection since UE already allotted the available
      interfaces for other PDNs
        Failure Type : Temporary Failure
      Recovery Mechanism : Retry call origination after disconnecting existing
                           PDN(s)
  */

  PS_NET_DOWN_REASON_IP_VERSION_MISMATCH            = TYPE_REASON_VAL(2,231),
  /**< 
      This error code is returned when a new call bring up is rejected
      since the existing data call IP type doesn't match the requested IP
      type
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination after disconnecting existing
                           call
  */

  PS_NET_DOWN_REASON_DUN_CALL_DISALLOWED            = TYPE_REASON_VAL(2,232),
  /**< 
      This error code is returned when a dial up networking (DUN) call bring
      up is rejected since UE is in eHRPD RAT
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry DUN call origination after UE transitions to
                           1X/HRPD
  */

  PS_NET_DOWN_REASON_INVALID_PROFILE                = TYPE_REASON_VAL(2,233),
  /**< 
      This error code is returned when a call bring up is requested with an
      invalid profile
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination using correct profile
  */

  PS_NET_DOWN_REASON_INTERNAL_EPC_NONEPC_TRANSITION = TYPE_REASON_VAL(2,234),
  /**< 
      This error code is returned when a data call is rejected/brought down
      since UE is transition between EPC and NONEPC RAT
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination after UE settles down on the
                           new RAT
  */

  PS_NET_DOWN_REASON_INVALID_PROFILE_ID             = TYPE_REASON_VAL(2,235),
  /**< 
      This error code is returned when a call bring up is requested with an
      invalid profile id
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination using correct profile id
     */ 

  PS_NET_DOWN_REASON_INTERNAL_CALL_ALREADY_PRESENT  = TYPE_REASON_VAL(2,236),
  /**<
      This error code is returned when a data call with the same policy is already
      connected or in the process of bringup on another RmNet instance. */

  PS_NET_DOWN_REASON_IFACE_IN_USE                   = TYPE_REASON_VAL(2,237),
  /**< 
      This error code is returned the current Iface is being in use 
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination after disconnecting existing
                           call
  */

  PS_NET_DOWN_REASON_IP_PDP_MISMATCH                = TYPE_REASON_VAL(2,238),
  /**< 
      This error code is returned when PPP call is attempted on PDP profile 
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination using correct profile id
  */

PS_NET_DOWN_REASON_APN_DISALLOWED_ON_ROAMING      = TYPE_REASON_VAL(2,239),
  /**< 
      This reason is returned if PDN connection to the APN is disallowed on
      roaming network.
   
      DS will generate DS_SYS_EVENT_3GPP_ROAMING_DISALLOWED_INFO
      to inform clients of the APNs that need to deregister
      and disconnect (if the PDN is UP) while UE is on Roaming network.
   
      Clients need to deregister/disconnect if APN name corresponding
      to the client is listed in this SYS event.
   
      Clients can register again if APN name corresponding to the
      client is not listed in this SYS event.   

      Failure Type       : Temporary Failure
      Recovery Mechanism : Retry using an APN name that is
                           not blocked or
                           retry when UE moves back to home network.
                                                                       */
  PS_NET_DOWN_REASON_APN_PRM_CHG                    = TYPE_REASON_VAL(2,240),
  /**< 
      When APN-related parameters are changed, the PDN associated with
      the parameters has to be brought down and re-established with the
      changed parameters. This error code is returned on a failure to
      re establish the PDN with the changed parameters
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Application can retry immediately to establish
                           a connection.
 
  */

  PS_NET_DOWN_REASON_IFACE_IN_USE_CFG_MATCH         = TYPE_REASON_VAL(2,241),
  /**< 
      This error code is returned when the iface is already in use with a 
      matching configuration. 
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination after disconnecting existing
                           call
  */                                                 

  PS_NET_DOWN_REASON_NULL_APN_DISALLOWED          = TYPE_REASON_VAL(2,242),
  /**< 
      The error code is returned when a PDN is attempted to be brought up
      with NULL APN and when NULL APN is not supported
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Application can retry immediately with a non NULL apn
                           to establish a connection.
  */

   PS_NET_DOWN_REASON_THERMAL_MITIGATION          = TYPE_REASON_VAL(2,243),
   /**< 
       The error code is returned when thermal level increases and causes
       calls to be torn down since normal mode of operation is not allowed.
       Failure Type       : Temporary Failure 
       Recovery Mechanism : Application can retry after thermal level resumes
                            to normal
   */

  PS_NET_DOWN_REASON_SUBS_ID_MISMATCH            = TYPE_REASON_VAL(2,244),
  /**< 
      This error code is returned when a new call bring up is rejected
      since the subs_id in the profile doesn't match the subs_id in
      ACL policy info
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination using correct subs_id
  */

  PS_NET_DOWN_REASON_DATA_SETTINGS_DISABLED            = TYPE_REASON_VAL(2,245),
  /**< 
      The error code is returned when PDN Connection to a given APN is
      disallowed because Data is disabled from the Device
      User Interface(UI) Settings.
   
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Application can retry with the same APN
                           if
                           a) Data Connection is re-enabled from the UI
                           AND
                           b) Data Roaming Connection is enabled from the UI
                              OR UE is on Home network
  */

  PS_NET_DOWN_REASON_DATA_ROAMING_SETTINGS_DISABLED    = TYPE_REASON_VAL(2,246),
  /**< 
      The error code is returned when PDN Connection to a given APN is
      disallowed because Data Roaming is disabled from the Device
      User Interface(UI) Settings and the UE is on Roaming Network.
   
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Application can retry with the same APN
                           if
                           a) Data Connection is enabled on the UI
                           AND
                           b) Data Roaming Connection is re-enabled from the UI
                              OR UE moves back to Home network
  */

    PS_NET_DOWN_REASON_APN_FORMAT_INVALID    = TYPE_REASON_VAL(2,247),
  /**< 
      The error code is returned when APN specified in policy does not follow
      the format specificated in 3gpp spec
   
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Application can retry with the another APN
                          
  */

  PS_NET_DOWN_REASON_VALIDATION_FAILURE      = TYPE_REASON_VAL(2,248),
  /**< 
         This error code is returned when data call is brought down due to some
         internal validation failure
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period 
  */

   PS_NET_DOWN_REASON_PROFILES_NOT_COMPATIBLE             = TYPE_REASON_VAL(2,251),
   /**< 
        This error code is returned when handoff failed 
        because 3gpp and 3gpp2 profile is not compatible.
        Failure Type       : Permanent Failure
        Recovery Mechanism : Client can retry a call using compatible 3GPP and 
                             3GPP2 profiles with the same APN and compatible IP types.
    */
   PS_NET_DOWN_REASON_NULL_RESOLVED_APN_NO_MATCH           = TYPE_REASON_VAL(2,252),
    /**< 
        This error code is returned when handoff failed because
        of NULL APN handoff, TRAT resolved APN is not same as SRAT APN
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with the network provider for NULL APN resolution mechanism.
    */

    PS_NET_DOWN_REASON_INVALID_APN_NAME      = TYPE_REASON_VAL(2,253),
    /**< The reason is posted by the UE when it tears down a PDN being brought 
         up with an APN that is part of forbidden APN Name list.
         Failure Type: Temporary
         Recovery Mechanism: Client may retry with the different APN/Profile */
    
  PS_NET_DOWN_REASON_INTERNAL_MAX,

/* To map CDMA specific call-end reasons from CM */
  PS_NET_DOWN_REASON_CDMA_LOCK                 = TYPE_REASON_VAL(3,500),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM due to  
        device in CDMA locked state
        Failure Type       : Permanent Failure
        Recovery Mechanism : Power cycle the device and retry call  
                             origination
    */
  PS_NET_DOWN_REASON_INTERCEPT                 = TYPE_REASON_VAL(3,501),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since it  
        received an intercept order from the base station
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_REORDER                   = TYPE_REASON_VAL(3,502),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM due to  
        receiving a reorder from base station
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_REL_SO_REJ                = TYPE_REASON_VAL(3,503),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM due to  
        receiving a release from base station with reason: SO Reject
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_INCOM_CALL                = TYPE_REASON_VAL(3,504),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since it  
        received an incoming call from base station
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_ALERT_STOP                = TYPE_REASON_VAL(3,505),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM due to RL/FL  
        fade (or) receiving call release from base stations
        code Failure Type  : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_ACTIVATION                = TYPE_REASON_VAL(3,506),
    /**< 
        This error code is returned when data call is brought down 
        because traffic channel got rejected/released by CM due to 
        channel acquisition failures. This indicates that device has 
        failed acquiring all the channels in the PRL
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_MAX_ACCESS_PROBE          = TYPE_REASON_VAL(3,507),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM due maximum  
        access probes transmitted
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_CCS_NOT_SUPPORTED_BY_BS   = TYPE_REASON_VAL(3,508),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since  
        concurrent service is not supported by base station
        Failure Type       : Permanent Failure 
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_NO_RESPONSE_FROM_BS       = TYPE_REASON_VAL(3,509),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since there  
        is no response received from base station
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_REJECTED_BY_BS            = TYPE_REASON_VAL(3,510),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM due to base  
        station rejecting the call
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_INCOMPATIBLE              = TYPE_REASON_VAL(3,511),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since  
        concurrent services requested were not compatible
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_ALREADY_IN_TC             = TYPE_REASON_VAL(3,512),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since traffic  
        channel is already up for voice calls
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_GPS = TYPE_REASON_VAL(3,513),
    /**< 
        This error code is not used. Do not publish in DSS/QMI
        Failure Type       : N/A
        Recovery Mechanism : N/A
    */
  PS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_SMS = TYPE_REASON_VAL(3,514),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected since SMS is  
        ongoing
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after SMS is  
                             complete
    */
  PS_NET_DOWN_REASON_NO_CDMA_SRV               = TYPE_REASON_VAL(3,515),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since device  
        does not have CDMA service
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_MC_ABORT                  = TYPE_REASON_VAL(3,516),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since MC  
        aborted the origination/conversation
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_PSIST_NG                  = TYPE_REASON_VAL(3,517),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM due to  
        persistence test failure
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_UIM_NOT_PRESENT           = TYPE_REASON_VAL(3,518),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM due to RUIM  
        not present
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after inserting UIM  
                             card
    */
  PS_NET_DOWN_REASON_RETRY_ORDER               = TYPE_REASON_VAL(3,519),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM due to  
        receiving a retry order from base station
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_ACCESS_BLOCK              = TYPE_REASON_VAL(3,520),
    /**<
        This error code is returned when data call is brought down because 
        traffic channel rejected/released due to Access blocked by 
        base station
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_ACCESS_BLOCK_ALL          = TYPE_REASON_VAL(3,521),
    /**<
        This error code is returned when data call is brought down because
        traffic channel rejected due to Access blocked by the 
        base station for all mobile devicess
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_IS707B_MAX_ACC            = TYPE_REASON_VAL(3,522),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM due maximum  
        access probes for IS-707B call
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_THERMAL_EMERGENCY         = TYPE_REASON_VAL(3,523),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM to put  
        device in thermal emergency
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after device cools  
                             down
    */
  PS_NET_DOWN_REASON_CALL_ORIG_THROTTLED       = TYPE_REASON_VAL(3,524),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since call  
        origination is throttled by DCTM module
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after CM unthrottles 
                             call origination
    */
  PS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_VOICE_CALL = TYPE_REASON_VAL(3,525),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got released by CM in favor of voice  
        call or SMS when concurrent voice and data are not supported
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after SMS/Voice call 
                             is complete
    */

/* To map GSM/WCDMA specific call-end reasons from CM */
  PS_NET_DOWN_REASON_CONF_FAILED               = TYPE_REASON_VAL(3,1000),
  /**<   This reason is returned when data call is brought down 
         because data call origination request failed by CM
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after some time  */
  PS_NET_DOWN_REASON_INCOM_REJ                 = TYPE_REASON_VAL(3,1001),
  /**<   This reason is returned when data call is brought down because 
         the other clients reject the incoming call
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after some time */
  PS_NET_DOWN_REASON_NO_GW_SRV                 = TYPE_REASON_VAL(3,1002),
  /**<   This reason is returned when the phone has no service 
         avaliable on G/W
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call after service becomes available */
  PS_NET_DOWN_REASON_NO_GPRS_CONTEXT           = TYPE_REASON_VAL(3,1003),
  /**<   This reason is returned when GPRS context is not available
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after some time   */
  PS_NET_DOWN_REASON_ILLEGAL_MS                = TYPE_REASON_VAL(3,1004),
  /**<   This reason is returned when the network refuses service to 
         the MS either because an identity of the MS is not acceptable to
         the network or because the MS does not pass the authentication check
         Failure Type       : Permanent Failure
         Recovery Mechanism : None  */
  PS_NET_DOWN_REASON_ILLEGAL_ME                = TYPE_REASON_VAL(3,1005),
  /**<   This reason is returned because ME could not authenticated 
         and the ME used is not acceptable to the network 
         Failure Type       : Permanent Failure
         Recovery Mechanism : None */
  PS_NET_DOWN_REASON_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED 
                                                     = TYPE_REASON_VAL(3,1006),
  /**<   This reason is returned to the MS when it is not allowed 
         to operate either GPRS or non-GPRS services
         Failure Type       : Permanent Failure
         Recovery Mechanism : None  */
  PS_NET_DOWN_REASON_GPRS_SERVICES_NOT_ALLOWED                       = TYPE_REASON_VAL(3,1007),
  /**<   This reason is returned to the MS when it is not allowed to 
         operate GPRS services
         Failure Type       : Permanent Failure
         Recovery Mechanism : None  */
  PS_NET_DOWN_REASON_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK
                                                     = TYPE_REASON_VAL(3,1008),
  /**<   This reason is returned because no matching identity/context could 
         be found in the network
         Failure Type       : Temporary failure
         Recovery Mechanism : Retry after a back-off time period  */
  PS_NET_DOWN_REASON_IMPLICITLY_DETACHED                             = TYPE_REASON_VAL(3,1009),
  /**<   This reason is returned to the MS either after the Mobile
         reachable timer has expired, or if the GMM context data related to
         the subscription dose not exist in the SGSN 
         Failure Type       : Temporary failure
         Recovery Mechanism : Retry after a back-off time period  */
  PS_NET_DOWN_REASON_PLMN_NOT_ALLOWED                                = TYPE_REASON_VAL(3,1010),
  /**<   This reason is returned when UE requests GPRS service, 
         or if the network initiates a detach request in a PLMN which does
         not offer roaming for GPRS services to that MS
         Failure Type       : Permanent Failure
         Recovery Mechanism : None  */
  PS_NET_DOWN_REASON_LA_NOT_ALLOWED                                  = TYPE_REASON_VAL(3,1011),
  /**<   This reason is returned because to the MS if it requests service, 
         or if the network initiates a detach request, in a location area
         where the HPLMN determines that the MS, by subscription, is not
         allowed to operate
         Failure Type       : Permanent Failure
         Recovery Mechanism : None */
  PS_NET_DOWN_REASON_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN          
                                                     = TYPE_REASON_VAL(3,1012),
  /**<   This reason is returned when UE request GPRS service or network 
         initiates a detach request in a PLMN which does not offer roaming
         for GPRS services
         Failure Type       : Permanent Failure
         Recovery Mechanism : None  */
  PS_NET_DOWN_REASON_PDP_DUPLICATE                                   = TYPE_REASON_VAL(3,1013),
  /**<   This reason is returned because PDP context already exists , 
         PDP context is rejected
         Failure Type       : Temporary Failure 
         Recovery Mechanism : Retry call origination with different APN  */
  PS_NET_DOWN_REASON_UE_RAT_CHANGE                                   = TYPE_REASON_VAL(3,1014),
  /**<   This reason is returned because there is RAT change on UE 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  PS_NET_DOWN_REASON_CONGESTION                                      = TYPE_REASON_VAL(3,1015),
  /**<   This reason is returned because network cannot serve a 
         request from the MS because of congestion 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_NO_PDP_CONTEXT_ACTIVATED                        = TYPE_REASON_VAL(3,1016),
  /**<   This reason is returned when MS requests an establishment of the 
         radio access bearers for all active PDP contexts by sending a
         SERVICE REQUEST message indicating "data" to the network, but
         the SGSN does not have any active PDP context
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time   */
  PS_NET_DOWN_REASON_ACCESS_CLASS_DSAC_REJECTION                     = TYPE_REASON_VAL(3,1017),
  /**<   This reason is returned when there is Access Class blocking restrictions 
         for the current camped cell
         Failure Type       : Temporary Failure
         Recovery Mechanism :  UE can try after some time */
  PS_NET_DOWN_REASON_PDP_ACTIVATE_MAX_RETRY_FAILED                   = TYPE_REASON_VAL(3,1018),
  /**<   This reason is returned when SM attempts PDP activation 
         for MAX times(4 attempts)
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_RAB_FAILURE                                     = TYPE_REASON_VAL(3,1019),
  /**<   This reason is returned when RAB failure occurs
         Failure Type       : Temporary Failure
         Recovery Mechanism :  UE can try after some time  */
  PS_NET_DOWN_REASON_EPS_SERVICES_NOT_ALLOWED                        = TYPE_REASON_VAL(3,1020),
  /**    Not Used */
  PS_NET_DOWN_REASON_TRACKING_AREA_NOT_ALLOWED                       = TYPE_REASON_VAL(3,1021),
  /**    Not Used */
  PS_NET_DOWN_REASON_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA       
                                                     = TYPE_REASON_VAL(3,1022),
  /**    Not Used */
  PS_NET_DOWN_REASON_NO_SUITABLE_CELLS_IN_TRACKING_AREA
                                                     = TYPE_REASON_VAL(3,1023),
  /**    Not Used */
  PS_NET_DOWN_REASON_NOT_AUTHORIZED_FOR_THIS_CSG                     = TYPE_REASON_VAL(3,1024),
  /**    Not Used */
  PS_NET_DOWN_REASON_ESM_UNKNOWN_EPS_BEARER_CONTEXT                  = TYPE_REASON_VAL(3,1025),
  /**<   This reason is returned when Invalid EPS bearer identity 
         in the request
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  PS_NET_DOWN_REASON_DRB_RELEASED_AT_RRC                             = TYPE_REASON_VAL(3,1026),
 /**<    This reason is returned when DRBs are released for many reason. 
         a) The DRB is released by RRC for internal reasons
         b) There is a mismatch scenario where UE has more DRBs than the N/W.
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_NAS_SIG_CONN_RELEASED                           = TYPE_REASON_VAL(3,1027),
  /**<   This reason is returned to indicate connection released
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_EMM_DETACHED                                    = TYPE_REASON_VAL(3,1028),
  /**<   This reason is returned when UE is detached
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_EMM_ATTACH_FAILED                               = TYPE_REASON_VAL(3,1029),
  /**<   This reason is returned when attach procedure is 
         rejected by the network
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_EMM_ATTACH_STARTED                              = TYPE_REASON_VAL(3,1030),
  /**<   This reason is returned for LTE E911 when attach procedure 
         is started for EMC purposes
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LTE_NAS_SERVICE_REQ_FAILED                      = TYPE_REASON_VAL(3,1031),
  /**<   This reason is returned indicates service request procedure failure
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_ESM_ACTIVE_DEDICATED_BEARER_REACTIVATED_BY_NW      
                                                     = TYPE_REASON_VAL(3,1032),
  /**<   This reason is returned when ACT DEDICATION BEARER is requested using 
         same DEFAULT BEARER ID
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_ESM_LOWER_LAYER_FAILURE                         = TYPE_REASON_VAL(3,1033),
  /**<   This reason is returned for collision scenarios for UE and network 
         initiated procedures.
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_ESM_SYNC_UP_WITH_NW                             = TYPE_REASON_VAL(3,1034),
  /**<   This reason is returned when bearer needs to be deactivated 
         to get in sync with network
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER 
                                                     = TYPE_REASON_VAL(3,1035),
  /**<   This reason is returned when ACT DEDICATION BEARER is requested 
         for an existing DEFAULT BEARER 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time   */
  PS_NET_DOWN_REASON_ESM_BAD_OTA_MESSAGE                               = TYPE_REASON_VAL(3,1036),
  /**<   This reason is returned to abort ongoing procedure in DS if 
         bad OTA message is received from network
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time   */
  PS_NET_DOWN_REASON_ESM_DS_REJECTED_THE_CALL                          = TYPE_REASON_VAL(3,1037),
  /**<   This reason is returned when DS rejects the call
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_ESM_CONTEXT_TRANSFERED_DUE_TO_IRAT
                                                     = TYPE_REASON_VAL(3,1038),
  /**<   This reason is returned when PDN is disconnected by DS due to IRAT 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_DS_EXPLICIT_DEACT                                 = TYPE_REASON_VAL(3,1039),
  /**<   This reason is returned when the dedicated bearer 
         will be deactivated regardless NW response
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_ESM_LOCAL_CAUSE_NONE                              = TYPE_REASON_VAL(3,1040),
  /**<   This reason is returned when no specific local cause is mentioned. 
         Usually come with a valid OTA cause
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE            
                                                     = TYPE_REASON_VAL(3,1041),
  /**<   This reason is returned to indicate that throttling is not 
         needed for this service request failure
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  PS_NET_DOWN_REASON_ACL_FAILURE                                       = TYPE_REASON_VAL(3,1042),
  /**<   This reason is returned when ACL check failure at lower layer
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LTE_NAS_SERVICE_REQ_FAILED_DS_DISALLOW            
                                                     = TYPE_REASON_VAL(3,1043),
  /**<   This reason is returned when service not allowed on the requested
         PLMN.
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  PS_NET_DOWN_REASON_EMM_T3417_EXPIRED                                 = TYPE_REASON_VAL(3,1044),
  /**<   This reason is returned when T3417 expires for service 
         request procedure 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  PS_NET_DOWN_REASON_EMM_T3417_EXT_EXPIRED                             = TYPE_REASON_VAL(3,1045),
  /**<   This reason is returned when ESR fails as T3417 EXT timer expires 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  PS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_TXN                      = TYPE_REASON_VAL(3,1046),
  /**<   This reason is returned when Transmission failure of uplink data 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_HO                       = TYPE_REASON_VAL(3,1047),
  /**<   This reason this is returned when uplink data failed to be delivered 
         due to Handover
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_CONN_REL
                                                     = TYPE_REASON_VAL(3,1048),
  /**<   This reason is returned when uplink data failed to be delivered 
         due to connection release
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time   */
  PS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_RLF                      = TYPE_REASON_VAL(3,1049),
  /**<   This reason is returned when uplink data failed to be 
         delivered due to Radio Link Failure
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN           
                                                     = TYPE_REASON_VAL(3,1050),
  /**<   This reason is returned where RRC is not in connected but NAS 
         sends uplink data request
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE                             = TYPE_REASON_VAL(3,1051),
  /**<   This reason is returned when connection failure at access stratum
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time   */
  PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_ABORTED                     = TYPE_REASON_VAL(3,1052),
  /**<   This reason is returned when connection establishment is aborted 
         due to other procedure
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_ACCESS_BARRED          //     
                                                     = TYPE_REASON_VAL(3,1053),
  /**<   This reason is returned when connection establishment failed 
         due to Lower layer RRC connection Failure
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time   */
  PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CELL_RESEL                  
                                                     = TYPE_REASON_VAL(3,1054),
  /**<   This reason is returned when connection establishment failed 
         due to cell reselection at access stratum
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time   */
  PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE              
                                                     = TYPE_REASON_VAL(3,1055),
  /**<   This reason is returned when connection establishment 
         failed due to configuration failure at RRC
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED               
                                                     = TYPE_REASON_VAL(3,1056),
  /**<   This reason is returned when connection could not be 
         establishment in the time limit 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_LINK_FAILURE
                                                     = TYPE_REASON_VAL(3,1057),
  /**<   This reason is returned when connection establishment 
         failed due to link failure at RRC 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_NOT_CAMPED
                                                     = TYPE_REASON_VAL(3,1058),
  /**<   This reason is returned when connection establishment failed 
         as RRC is not camped on any cell
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_SI_FAILURE
                                                     = TYPE_REASON_VAL(3,1059),
  /**<   This reason is returned when connection establishment 
         failed due to SI failure at RRC
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CONN_REJECT
                                                     = TYPE_REASON_VAL(3,1060),
  /**<   This reason is returned when connection establishment failed 
         due to NW rejecting UE connection request
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LRRC_CONN_REL_NORMAL                              = TYPE_REASON_VAL(3,1061),
  /**<   This reason is returned for normal connection release
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LRRC_CONN_REL_RLF                                 = TYPE_REASON_VAL(3,1062),
  /**<   This reason is returned when connection release failed due 
         to RLF conditions       
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  PS_NET_DOWN_REASON_LRRC_CONN_REL_CRE_FAILURE                         = TYPE_REASON_VAL(3,1063),
  /**<   This reason is returned when connection re-establishment failure 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LRRC_CONN_REL_OOS_DURING_CRE                      = TYPE_REASON_VAL(3,1064),
  /**<   This reason is returned when UE going OOS during CR, so the 
         connection is getting released
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LRRC_CONN_REL_ABORTED                             = TYPE_REASON_VAL(3,1065),
  /**<   This reason is returned when connection released by RRC due 
         to the abort request
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_LRRC_CONN_REL_SIB_READ_ERROR                      = TYPE_REASON_VAL(3,1066),
  /**<   This reason is returned when connection release due to SIB read 
         error
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  PS_NET_DOWN_REASON_DETACH_WITH_REATTACH_LTE_NW_DETACH               
                                                     = TYPE_REASON_VAL(3,1067),
  /**<   This reason is returned when NW initiated detach with reattach
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH            
                                                     = TYPE_REASON_VAL(3,1068),
  /**<   This reason is returned when NW initiated detach without reattach
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  PS_NET_DOWN_REASON_ESM_PROC_TIME_OUT                                 = TYPE_REASON_VAL(3,1069),
  /**<   This reason is returned when ESM procedure max attempt 
         timeout failure
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */

  PS_NET_DOWN_REASON_INVALID_CONNECTION_ID                              = TYPE_REASON_VAL(3,1070),
    /**< 
        This error code is returned when No PDP exists with the 
        given Connection Id while doing Modification/Deactivation 
        OR Activation for already Active PDP
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_INVALID_NSAPI                                     = TYPE_REASON_VAL(3,1071),
    /**< 
        This error code is returned when NSAPIs exceeds max while 
        PDP activation. Invalid Modify/Deactivation Request by CM 
        for ConId. While activating Secondary when PDP already active 
        with same connection Id, Reject the ACTIVATE req
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_INVALID_PRI_NSAPI                                 = TYPE_REASON_VAL(3,1072),
    /**< 
        This error code is returned when Primary context for NSAPI 
        Doesn’t exist, Reject the SEC ACTIVATE req con_id
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_INVALID_FIELD                                     = TYPE_REASON_VAL(3,1073),
    /**< 
        This error code is returned when Unable to Encode 
        OTA message for MT PDP or Deactivate PDP
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_RAB_SETUP_FAILURE                                 = TYPE_REASON_VAL(3,1074),
    /**< 
        This error code is returned when When RAB is not established
        by lower layers during Activation/Modification/Deactivation
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_PDP_ESTABLISH_MAX_TIMEOUT                         = TYPE_REASON_VAL(3,1075),
    /**< 
        This error code is returned when 
        On Expiry of PDP establish timer with MAX (5) retries
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_PDP_MODIFY_MAX_TIMEOUT                            = TYPE_REASON_VAL(3,1076),
    /**< 
        This error code is returned On Expiry 
        of PDP Modify timer with MAX (4) retries
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_PDP_INACTIVE_MAX_TIMEOUT                          = TYPE_REASON_VAL(3,1077),
    /**< 
        This error code is returned when on Expiry of PDP Deactivate 
        timer with MAX (4) reties
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_PDP_LOWERLAYER_ERROR                              = TYPE_REASON_VAL(3,1078),
    /**< 
        This error code is returned PDP activation failed 
        because of RRC_ABORT or Forbidden PLMN
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_PPD_UNKNOWN_REASON                                = TYPE_REASON_VAL(3,1079),
    /**< 
        This error code is returned when Local Deactivation 
        SM_NATIONAL_ROAMING_NOT_ALLOWED: Roaming not allowed
        SM_NO_SUITABLE_CELLS_IN_LA: No Suitable Cells In Location Area
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_PDP_MODIFY_COLLISION                              = TYPE_REASON_VAL(3,1080),
    /**< 
        This error code is returned when MO PDP Modify collision when 
        MT PDP is already in progress
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_PDP_MBMS_REQUEST_COLLISION                        = TYPE_REASON_VAL(3,1081),
    /**< 
        This error code is returned when PDP_MBMS_REQUEST received when 
        PDP activation is already PDP_ACTIVE_PENDING on same Conn Id
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_MBMS_DUPLICATE                                    = TYPE_REASON_VAL(3,1082),
    /**< 
        This error code is returned when MBMS activation is already pending 
        and PDP_MBMS_REQUEST is triggered
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_SM_PS_DETACHED                                    = TYPE_REASON_VAL(3,1083),
    /**< 
        This error code is returned when 
        Internal cause for call end due to PS DETACH
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_SM_NO_RADIO_AVAILABLE                             = TYPE_REASON_VAL(3,1084),
    /**< 
        This error code is returned when Internal cause to indicate that 
        Radio resource is not available
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_SM_ABORT_SERVICE_NOT_AVAILABLE                    = TYPE_REASON_VAL(3,1085),
    /**< 
        This error code is returned when Internal cause to indicate abort 
        because service is not available
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */

  PS_NET_DOWN_REASON_MESSAGE_EXCEED_MAX_L2_LIMIT                       = TYPE_REASON_VAL(3,1086),
  /**<   This reason is returned when MAX size of L3 message is exceeded
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  PS_NET_DOWN_REASON_SM_NAS_SRV_REQ_FAILURE                            = TYPE_REASON_VAL(3,1087),
  /**<   
         This reason is returned when NAS/Lower layers service request 
         is rejected by the network
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after some time 
  */

  PS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_REQ_ERROR                    = TYPE_REASON_VAL(3,1088),
  /**<   This reason is returned when there is an RRC Connection 
         Establishment Failure due to error in Request message.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_TAI_CHANGE                   = TYPE_REASON_VAL(3,1089),
  /**<   This reason is returned when there is an RRC Connection 
         Establishment Failure due to change in Tracking Area ID.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE               = TYPE_REASON_VAL(3,1090),
  /**<   This reason is returned when there is an RRC Connection 
         Establishment Failure because RF was unavailable.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_RRC_CONN_REL_ABORTED_IRAT_SUCCESS                 = TYPE_REASON_VAL(3,1091),
  /**<   This reason is returned after connection is aborted 
         before deactivating LTE stack due to a successful
         L->X IRAT (e.g. after IRAT handovers).
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period or
                              after WCDMA/GSM/TDSCDMA System Status
                              Indication is received */

  PS_NET_DOWN_REASON_RRC_CONN_REL_RLF_SEC_NOT_ACTIVE                   = TYPE_REASON_VAL(3,1092),
  /**<   If UE hits an LTE Radio Link Failure(RLF) 
         before security is established, 
         the connection needs to be released and UE needs to move
         back to idle. 
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_RRC_CONN_REL_IRAT_TO_LTE_ABORTED                  = TYPE_REASON_VAL(3,1093),
  /**<   Connection aborted by NAS right after a IRAT to LTE
         IRAT handover.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_CCO_SUCCESS       = TYPE_REASON_VAL(3,1094),
  /**<   This reason is returned after connection is aborted before
         deactivating LTE stack after a successful L->G IRAT Cell
         Change Order (CCO) procedure.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period or
                              after GSM/EDGE System Status Indication
                              is received by the client */

  PS_NET_DOWN_REASON_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_CCO_ABORTED       = TYPE_REASON_VAL(3,1095),
  /**<   This reason is returned after connection is aborted in 
         the middle of a L->G IRAT Cell Change Order (CCO)
         procedure.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period or
                              after GSM/EDGE System Status Indication
                              is received by the client */

  PS_NET_DOWN_REASON_IMSI_UNKNOWN_IN_HSS                               = TYPE_REASON_VAL(3,1096),
  /**<   This reason is returned when the IMSI present in UE 
         is unknown in HSS.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_IMEI_NOT_ACCEPTED                                 = TYPE_REASON_VAL(3,1097),
  /**<   This reason is returned when the IMEI of the UE is 
         not accepted by the network.
         Failure Type       : Permanent Failure
         Recovery Mechanism : None */

  PS_NET_DOWN_REASON_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED     = TYPE_REASON_VAL(3,1098),
  /**<   This reason is returned when Evolved Packet System(EPS) 
         and non-Evolved Packet System(non-EPS) Services are
         not allowed by the network.
         Failure Type       : Permanent Failure
         Recovery Mechanism : None */

  PS_NET_DOWN_REASON_EPS_SERVICES_NOT_ALLOWED_IN_PLMN                  = TYPE_REASON_VAL(3,1099),
  /**<   This reason is returned when Evolved Packet System(EPS) 
         Services are not allowed in the PLMN.
         Failure Type       : Permanent Failure
         Recovery Mechanism : None */

  PS_NET_DOWN_REASON_MSC_TEMPORARILY_NOT_REACHABLE                     = TYPE_REASON_VAL(3,1100),
  /**<   This reason is returned when the Mobile Switching Center 
         (MSC) is temporarily not reachable.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_CS_DOMAIN_NOT_AVAILABLE                           = TYPE_REASON_VAL(3,1101),
  /**<   This reason is returned if CS Domain is not available.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_ESM_FAILURE                                       = TYPE_REASON_VAL(3,1102),
  /**<   This reason is returned if there is an ESM level failure.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_MAC_FAILURE                                       = TYPE_REASON_VAL(3,1103),
  /**<   This reason is returned if there is a MAC level failure.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_SYNCH_FAILURE                                     = TYPE_REASON_VAL(3,1104),
  /**<   This reason is returned if there is a Synchronization failure.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_UE_SECURITY_CAPABILITIES_MISMATCH                 = TYPE_REASON_VAL(3,1105),
  /**<   This reason is returned if there is a UE Security 
         capabilities mismatch.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_SECURITY_MODE_REJ_UNSPECIFIED                     = TYPE_REASON_VAL(3,1106),
  /**<   This reason is returned if there is an unspecified 
         security mode reject.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_NON_EPS_AUTH_UNACCEPTABLE                         = TYPE_REASON_VAL(3,1107),
  /**<   This reason is returned if there is unacceptable
         non-EPS authentication. 
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_CS_FALLBACK_CALL_EST_NOT_ALLOWED                  = TYPE_REASON_VAL(3,1108),
  /**<   This reason is returned if CS Fallback call establishment 
         is not allowed.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_NO_EPS_BEARER_CONTEXT_ACTIVATED                   = TYPE_REASON_VAL(3,1109),
  /**<   This reason is returned if no Evolved packet System (EPS) 
         Bearer Context was activated.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_EMM_INVALID_STATE                                 = TYPE_REASON_VAL(3,1110),
  /**<   This reason is returned if call ended due to Invalid 
         EMM state.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

   PS_NET_DOWN_REASON_NAS_LAYER_FAILURE                                 = TYPE_REASON_VAL(3,1111),
  /**<   This reason is returned if there was a 
         Non-Access Spectrum(NAS) layer failure.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  PS_NET_DOWN_REASON_DETACH_EPS_SERVICES_NOT_ALLOWED = TYPE_REASON_VAL(3,1118),
  /**< 
         This error code is returned when Network initiates a detach on LTE with 
         cause code 7 saying data plan has been replinished or has expired

         Failure Type       : Temporary Failure
         Recovery Mechanism : Client may retry on a new APN
  */

  PS_NET_DOWN_REASON_SM_INTERNAL_PDP_DEACTIVATION = TYPE_REASON_VAL(3,1119),
  /**< 
         This error code is returned when UMTS Interface is brought down
         due to handover from UMTS to iWLAN.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period 
  */

/* To map HDR specific call-end reasons from CM */
  PS_NET_DOWN_REASON_CD_GEN_OR_BUSY            = TYPE_REASON_VAL(3,1500),
    /**< 
        This error code is returned when data call is brought down
        because traffic channel got rejected/released by CM due to the  
        reception of a Connection Deny message with a deny code of  
        general or network busy
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_CD_BILL_OR_AUTH           = TYPE_REASON_VAL(3,1501),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM due to the  
        reception of a Connection Deny message with a deny code of  
        billing failure or authentication failure
        Failure Type       : Permanent Failure 
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_CHG_HDR                   = TYPE_REASON_VAL(3,1502),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since there 
        is a change to HDR system due to redirection or PRL not  
        preferred
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_EXIT_HDR                  = TYPE_REASON_VAL(3,1503),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since device  
        exited HDR due to redirection or PRL not preferred
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_HDR_NO_SESSION            = TYPE_REASON_VAL(3,1504),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since device  
        does not have a HDR session
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after device opens  
                             a HDR session
    */
  PS_NET_DOWN_REASON_HDR_ORIG_DURING_GPS_FIX   = TYPE_REASON_VAL(3,1505),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since it is  
        ending an HDR call origination in favor of a GPS fix
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_HDR_CS_TIMEOUT            = TYPE_REASON_VAL(3,1506),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since  
        connection setup on HDR system timed out
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_HDR_RELEASED_BY_CM        = TYPE_REASON_VAL(3,1507),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM when it  
        wants to release a HDR call so a 1X call can continue
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after 1X call is  
                             brought down
    */
  PS_NET_DOWN_REASON_COLLOC_ACQ_FAIL           = TYPE_REASON_VAL(3,1508),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM when device  
        failed to acquire co-located HDR for origination
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination over 1X
    */
  PS_NET_DOWN_REASON_OTASP_COMMIT_IN_PROG      = TYPE_REASON_VAL(3,1509),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since an  
        OTASP commit is in progress
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_NO_HYBR_HDR_SRV           = TYPE_REASON_VAL(3,1510),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since  
        device has no Hybrid HDR service
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_HDR_NO_LOCK_GRANTED       = TYPE_REASON_VAL(3,1511),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since HDR  
        module could not get the RF lock
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_HOLD_OTHER_IN_PROG        = TYPE_REASON_VAL(3,1512),
    /**< 
        This error code is returned when data call is brought down by CM  
        because DBM or SMS is in progress
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_HDR_FADE                  = TYPE_REASON_VAL(3,1513),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since HDR  
        module released the call due to fade
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_HDR_ACC_FAIL              = TYPE_REASON_VAL(3,1514),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM due to HDR  
        system Access Failure
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */

/* To map technology-agnostic call-end reasons from CM */
  PS_NET_DOWN_REASON_CLIENT_END               = TYPE_REASON_VAL(3,2000),
    /**<
        This error code is returned when client ends the data call
        Failure Type       : Not Applicable
        Recovery Mechanism : Not Applicable
    */  
  PS_NET_DOWN_REASON_NO_SRV                   = TYPE_REASON_VAL(3,2001),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since device  
        has no service
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_FADE                     = TYPE_REASON_VAL(3,2002),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since device  
        lost the system due to fade
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_REL_NORMAL               = TYPE_REASON_VAL(3,2003),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM due to  
        receiving a release from base station with no reason
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_ACC_IN_PROG              = TYPE_REASON_VAL(3,2004),
    /**<
      This error code is returned when data call is brought down because 
      traffic channel got rejected/released by CM due to Access 
      attempt already in progress
      Failure Type       : Temporary Failure
      Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_ACC_FAIL                 = TYPE_REASON_VAL(3,2005),
    /**<
      This error code is returned when data call is brought down because 
      traffic channel got rejected/released by CM due to Access Failure
      Failure Type       : Temporary Failure
      Recovery Mechanism : Retry call origination after some time
    */

  PS_NET_DOWN_REASON_REDIR_OR_HANDOFF         = TYPE_REASON_VAL(3,2006),
    /**< 
        This error code is returned when data call is brought down  
        because device is in the process of redirecting/handing off to  
        a different target system
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after device settles 
                             down on the new system
    */

/* To map some call cmd err reasons from CM */
  PS_NET_DOWN_REASON_OFFLINE                  = TYPE_REASON_VAL(3,2500),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since device  
        went offline
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after device goes  
                             online
    */
  PS_NET_DOWN_REASON_EMERGENCY_MODE           = TYPE_REASON_VAL(3,2501),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since device  
        is operating in Emergency mode
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after device comes  
                             out of emergency mode
    */
  PS_NET_DOWN_REASON_PHONE_IN_USE             = TYPE_REASON_VAL(3,2502),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since device  
        is in use (e.g voice call)
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_INVALID_MODE             = TYPE_REASON_VAL(3,2503),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since the  
        device's operational mode is different from the mode requested  
        in the traffic channel bringup
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after correcting the  
        device's mode preference
    */
  PS_NET_DOWN_REASON_INVALID_SIM_STATE        = TYPE_REASON_VAL(3,2504),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since the  
        SIM was marked by network as invalid for circuit and/or packet  
        service domain
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/check SIM  
                             validity
    */
  PS_NET_DOWN_REASON_NO_COLLOC_HDR            = TYPE_REASON_VAL(3,2505),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since there  
        is no colocated HDR
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_CALL_CONTROL_REJECTED    = TYPE_REASON_VAL(3,2506),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since Call  
        control module rejected the request
        Failure Type       : Permanent Failure 
        Recovery Mechanism : Check UIM configuration
    */ 
   
  /* EAP error codes. Start from 5000. These EAP error codes are not 
     reported by Mode Handler currently. Do not publish in DSS/QMI */
  PS_NET_DOWN_REASON_EAP_MIN = TYPE_REASON_VAL(4,5000), /* Do not use. Used for bounds check. */
  PS_NET_DOWN_REASON_EAP_CLIENT_ERR_UNABLE_TO_PROCESS,
  PS_NET_DOWN_REASON_EAP_CLIENT_ERR_UNSUPPORTED_VERS,
  PS_NET_DOWN_REASON_EAP_CLIENT_ERR_INSUFFICIENT_CHALLANGES,
  PS_NET_DOWN_REASON_EAP_CLIENT_ERR_RAND_NOT_FRESH,
  PS_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_AFTER_AUTH, /*EAP 0*/
  PS_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_BEFORE_AUTH, /*EAP 16384*/
  PS_NET_DOWN_REASON_EAP_NOTIFICATION_TEMP_DENIED_ACCESS, /*EAP 1026*/
  PS_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NOT_SUBSCRIBED, /*EAP 1031*/
  PS_NET_DOWN_REASON_EAP_SUCCESS, /*EAP 32768*/
  PS_NET_DOWN_REASON_EAP_NOTIFICATION_REALM_UNAVAILABLE,
  PS_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NAME_UNAVAILABLE,
  PS_NET_DOWN_REASON_EAP_NOTIFICATION_CALL_BARRED,
  PS_NET_DOWN_REASON_EAP_MAX, /* Do not use. Used for bounds check. */

  /* IPSEC Error Codes. Start from 5100*/
  PS_NET_DOWN_REASON_IPSEC_MIN = TYPE_REASON_VAL(5,5100), /* Do not use. Used for bounds check. */
  PS_NET_DOWN_REASON_IPSEC_GW_UNREACHABLE,
  PS_NET_DOWN_REASON_IPSEC_AUTH_FAILED,
  PS_NET_DOWN_REASON_IPSEC_CERT_INVALID,
  PS_NET_DOWN_REASON_IPSEC_INTERNAL_ERROR,
  PS_NET_DOWN_REASON_IPSEC_MAX, /* Do not use. Used for bounds check. */

  /* MIP Error codes. */
  PS_NET_DOWN_REASON_MIP_FA_ERR_REASON_UNSPECIFIED              = TYPE_REASON_VAL(1,64),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        with unspecified reason
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_ADMINISTRATIVELY_PROHIBITED     = TYPE_REASON_VAL(1,65),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent administratively prohibited  
        MIP registration
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_INSUFFICIENT_RESOURCES          = TYPE_REASON_VAL(1,66),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        due to insufficient resources
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_MOBILE_NODE_AUTHENTICATION_FAILURE
                                                                = TYPE_REASON_VAL(1,67),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        because MN-AAA authenticator was wrong
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after correctly  
                             provisioning the device with MIP 
                             information
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_HA_AUTHENTICATION_FAILURE       = TYPE_REASON_VAL(1,68),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        because of home agent authentication failure
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/Retry call  
                             origination after correctly provisioning 
                             the device with MIP information
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_REQUESTED_LIFETIME_TOO_LONG     = TYPE_REASON_VAL(1,69),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        because  requested lifetime is too long
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after sometime.  
                             Expectation is FA might propose an 
                             acceptable lifetime in AAM during next MIP 
                             call bringup
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_MALFORMED_REQUEST               = TYPE_REASON_VAL(1,70),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        due to malformed request
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/Retry call  
                             origination after correctly provisioning 
                             the device with MIP information
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_MALFORMED_REPLY                 = TYPE_REASON_VAL(1,71),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        due to malformed reply
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/Retry call  
                             origination after correctly provisioning 
                             the device with MIP information
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_ENCAPSULATION_UNAVAILABLE       = TYPE_REASON_VAL(1,72),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        because requested encapsulation is unavailable
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/Retry call  
                             origination after correctly provisioning 
                             the device with MIP information
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_VJHC_UNAVAILABLE                = TYPE_REASON_VAL(1,73),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        because VJ Header Compression is unavailable
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/Retry call  
                             origination after correctly provisioning 
                             the device with MIP information
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_UNKNOWN_CHALLENGE               = TYPE_REASON_VAL(1,104),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        due to unknown challenge
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after sometime.  
                             Expectation is the next MIP call bringup 
                             will get a correct challenge in AAM/RRP
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_MISSING_CHALLENGE               = TYPE_REASON_VAL(1,105),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration d 
        ue to missing challenge
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/Retry call  
                             origination after correctly provisioning 
                             the device with MIP information
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_STALE_CHALLENGE                 = TYPE_REASON_VAL(1,106),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        due to stale challenge
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after sometime.  
                             Expectation is the next MIP call bringup 
                             will get a correct challenge in AAM/RRP
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_REVERSE_TUNNEL_UNAVAILABLE      = TYPE_REASON_VAL(1,74),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        because reverse tunnel is unavailable
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/Retry call  
                             origination after correctly provisioning 
                             the device with MIP information
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_REVERSE_TUNNEL_IS_MANDATORY_AND_T_BIT_NOT_SET
                                                                = TYPE_REASON_VAL(1,75),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        because reverse tunnel is mandatory but not requested by device
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/Retry call  
                             origination after correctly provisioning 
                             the device with MIP information
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_DELIVERY_STYLE_NOT_SUPPORTED    = TYPE_REASON_VAL(1,79),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        because delivery style is not supported
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/Retry call  
                             origination after correctly provisioning 
                             the device with MIP information
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_MISSING_NAI                     = TYPE_REASON_VAL(1,97),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        due to missing NAI
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after correctly  
                             provisioning the device with MIP 
                             informations
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_MISSING_HA                      = TYPE_REASON_VAL(1,98),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        due to missing Home Agent
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/Retry call  
                             origination after correctly provisioning 
                             the device with MIP information
    */
  PS_NET_DOWN_REASON_MIP_FA_ERR_MISSING_HOME_ADDR               = TYPE_REASON_VAL(1,99),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since foreign agent rejected MIP registration  
        due to missing Home Address
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/Retry call  
                             origination after correctly provisioning 
                             the device with MIP information
    */
  PS_NET_DOWN_REASON_MIP_HA_ERR_REASON_UNSPECIFIED              = TYPE_REASON_VAL(1,128),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since home agent rejected MIP registration with  
        unspecified reason
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_MIP_HA_ERR_ADMINISTRATIVELY_PROHIBITED     = TYPE_REASON_VAL(1,129),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since home agent administratively prohibited  
        MIP registration
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_MIP_HA_ERR_INSUFFICIENT_RESOURCES          = TYPE_REASON_VAL(1,130),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since home agent rejected MIP registration due  
        to insufficient resources
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_MIP_HA_ERR_MOBILE_NODE_AUTHENTICATION_FAILURE
                                                                = TYPE_REASON_VAL(1,131),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since home agent fails authentication because  
        MN-HA authenticator was wrong
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after correctly  
                             provisioning the device with MIP 
                             information
    */
  PS_NET_DOWN_REASON_MIP_HA_ERR_FA_AUTHENTICATION_FAILURE       = TYPE_REASON_VAL(1,132),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since home agent rejected MIP registration due  
        to FA authentication failure
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/Retry call  
                             origination after correctly provisioning 
                             the device with MIP information
    */
  PS_NET_DOWN_REASON_MIP_HA_ERR_REGISTRATION_ID_MISMATCH        = TYPE_REASON_VAL(1,133),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since home agent rejected MIP registration due  
        to registration id mismatch
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_MIP_HA_ERR_MALFORMED_REQUEST               = TYPE_REASON_VAL(1,134),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since home agent rejected MIP registration due  
        to malformed request
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after correctly  
                             provisioning the device with MIP 
                             information
    */
  PS_NET_DOWN_REASON_MIP_HA_ERR_UNKNOWN_HA_ADDR                 = TYPE_REASON_VAL(1,136),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since home agent rejected MIP registration due  
        to unknown Home Agent address. This code is returned by a home  
        agent when the mobile node is performing dynamic home agent  
        address resolution as described in RFC 3220 (IP Mobility Support  
        for IPV4) Sections 3.6.1.1 and 3.6.1.2
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_MIP_HA_ERR_REVERSE_TUNNEL_UNAVAILABLE      = TYPE_REASON_VAL(1,137),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since home agent rejected MIP registration  
        because reverse tunnel is unavailable
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/Retry call  
                             origination after correctly provisioning 
                             the device with MIP information
    */
  PS_NET_DOWN_REASON_MIP_HA_ERR_REVERSE_TUNNEL_IS_MANDATORY_AND_T_BIT_NOT_SET
                                                                = TYPE_REASON_VAL(1,138),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since home agent rejected MIP registration  
        because reverse tunnel is mandatory but not requested by device
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/Retry call  
                             origination after correctly provisioning 
                             the device with MIP information
    */
  PS_NET_DOWN_REASON_MIP_HA_ERR_ENCAPSULATION_UNAVAILABLE       = TYPE_REASON_VAL(1,139),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase since home agent rejected MIP registration due  
        to encapsulation unavailable
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after correctly  
                             provisioning the device with MIP 
                             information
    */
  PS_NET_DOWN_REASON_MIP_ERR_REASON_UNKNOWN                     = TYPE_REASON_VAL(1,65535),
    /**< 
        This error code is returned when the data call bringup fails in  
        MIP setup phase with unknown reason
        Failure Type       : Permanent Failure
        Recovery Mechanism : Unknown
    */

  /*PPP error codes */
  PS_NET_DOWN_REASON_PPP_ERR_TIMEOUT                            = TYPE_REASON_VAL(7,1),
    /**< 
        This error code is returned when the data call bringup fails in  
        PPP setup due to timeout (For e.g: LCP Conf Ack not received  
        from network)
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_PPP_ERR_AUTH_FAILURE                       = TYPE_REASON_VAL(7,2),
    /**< 
        This error code is returned when the data call bringup fails in  
        PPP setup due to authentication failure
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after correctly  
                             provisioning the device with authentication 
                             credentials
    */
  PS_NET_DOWN_REASON_PPP_ERR_OPTION_MISMATCH                    = TYPE_REASON_VAL(7,3),
    /**< 
        This error code is returned when the data call bringup fails in  
        PPP setup due option mismatch (e.g: Auth is required, but not  
        negotiated with network during LCP phase)
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after comparing the  
                             PPP configurations in device and network
    */
  PS_NET_DOWN_REASON_PPP_ERR_PAP_FAILURE                        = TYPE_REASON_VAL(7,31),
    /**< 
        This error code is returned when the data call bringup fails in  
        PPP setup due to PAP failure
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after correctly  
                             provisioning the device with PAP 
                             credentials
    */
  PS_NET_DOWN_REASON_PPP_ERR_CHAP_FAILURE                       = TYPE_REASON_VAL(7,32),
    /**< 
        This error code is returned when the data call bringup fails in  
        PPP setup due to CHAP failure
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after correctly  
                             provisioning the device with CHAP 
                             credentials
    */
  PS_NET_DOWN_REASON_PPP_ERR_CLOSE_IN_PROGRESS                  = TYPE_REASON_VAL(7,33),
    /**< 
        This error code is returned when the data call bringup fails in  
        PPP setup since PPP is in the process of cleaning the previous 
        PPP session
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after sometime
    */
   PS_NET_DOWN_REASON_NV_REFRESH_IN_PROGRESS                  =   TYPE_REASON_VAL(7,34),
    /**< 
        This error code is returned when the data call bringup fails in  
        bring up phase while nv refresh is in progress.
        
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after sometime
    */

  PS_NET_DOWN_REASON_PPP_ERR_REASON_UNKNOWN                     = TYPE_REASON_VAL(7,65535),
    /**< 
        This error code is unused. Do not publish in DSS/QMI
        Failure Type       : Not Applicable
        Recovery Mechanism : Not Applicable
    */

  /* eHRPD error codes */
  PS_NET_DOWN_REASON_EHRPD_ERR_SUBS_LIMITED_TO_V4               = TYPE_REASON_VAL(8,1),
	  /**<
        This error code is returned when the V6 interface bring up fails  
        because network provided only V4 address for the upcoming PDN 
        Failure Type       : Permanent Failure
        Recovery Mechanism : Client can reattempt V6 call bringup after  
                             the V4 interface is also brought down. 
                             However there is no guarantee that network  
                             will provide a V6 address
    */
	
  PS_NET_DOWN_REASON_EHRPD_ERR_SUBS_LIMITED_TO_V6               = TYPE_REASON_VAL(8,2),
	  /**<
        This error code is returned when the V4 interface bring up fails  
        because network provided only V6 address for the upcoming PDN 
        Failure Type       : Permanent Failure
        Recovery Mechanism : Client can reattempt V4 call bringup after  
                             the V6 interface is also brought down. 
                             However there is no guarantee that network  
                             will provide a V4 address
    */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_TIMEOUT                    = TYPE_REASON_VAL(8,4),
    /**< 
        This error code is returned when the data call bringup fails in  
        VSNCP phase due to VSNCP timeout error
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_FAILURE                    = TYPE_REASON_VAL(8,5),
   /**< Alias to PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_CONFIG_FAILURE. Do not publish in DSS/QMI */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_CONFIG_FAILURE             
                                          = PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_FAILURE,
    /**< 
        This error code is returned when VSNCP configuration failed 
        during call bringup
        Failure Type       : Permanent Failure 
        Recovery Mechanism : Retry call origination after correctly 
                             provisioning the device with VSNCP 
                             information
    */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_GEN_ERROR           = TYPE_REASON_VAL(8,6),
    /**< 
        This error code is returned when the data call bringup fails in  
        VSNCP phase due to general error
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_UNAUTH_APN          = TYPE_REASON_VAL(8,7),
    /**< 
        This error code is returned when the data call bringup fails in  
        VSNCP phase since network rejected VSNCP config request with  
        reason requested APN is unauthorized
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check network provider for authorized list  
                             of APNs
    */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_LIMIT_EXCEED    = TYPE_REASON_VAL(8,8),
    /**< 
        This error code is returned when the data call bringup fails in  
        VSNCP phase since network rejected VSNCP config request with  
        reason PDN limit exceeded
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check network for any stale PDN connections 
                             maintained for the device (or) check the 
                             max number of PDN connections allowed by  
                             the device/network (or) power cycle device
    */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_NO_PDN_GW           = TYPE_REASON_VAL(8,9),
    /**< 
        This error code is returned when the data call bringup fails in  
        VSNCP phase since network rejected VSNCP config request with  
        reason no PDN gateway
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_GW_UNREACH      = TYPE_REASON_VAL(8,10),
    /**< 
        This error code is returned when the data call bringup fails in  
        VSNCP phase since network rejected VSNCP config request with  
        reason PDN gateway unreachable
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_GW_REJ          = TYPE_REASON_VAL(8,11),
    /**< 
        This error code is returned when the data call bringup fails in  
        VSNCP phase since network rejected VSNCP config request with  
        reason PDN gateway reject
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_INSUFF_PARAM        = TYPE_REASON_VAL(8,12),
    /**< 
        This error code is returned when the data call bringup fails in  
        VSNCP phase since network rejected VSNCP config request with  
        reason insufficient parameter
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after correctly  
                             provisioning the device with VSNCP 
                             information
    */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_RESOURCE_UNAVAIL    = TYPE_REASON_VAL(8,13),
    /**< 
        This error code is returned when the data call bringup fails in  
        VSNCP phase since network rejected VSNCP config request with  
        reason resource unavailable
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_ADMIN_PROHIBIT      = TYPE_REASON_VAL(8,14),
    /**< 
        This error code is returned when the data call bringup fails in  
        SNCP phase since network rejected VSNCP config request with  
        reason admin prohibited
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_ID_IN_USE       = TYPE_REASON_VAL(8,15),
    /**< 
        This error code is returned when the data call bringup fails in  
        VSNCP phase because network rejected with reason PDN ID IN USE  
        (or) All existing PDNs are brought down with this end reason  
        because one of the PDN bring up got rejected by network with  
        reason PDN ID IN USE
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after the entire  
                             PPP session is brought down in the device
    */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_SUBSCR_LIMITATION   = TYPE_REASON_VAL(8,16),
    /**< 
        This error code is returned when the data call bringup fails in  
        VSNCP phase since network rejected VSNCP config request with  
        reason subscriber limitation
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_EXISTS_FOR_THIS_APN = TYPE_REASON_VAL(8,17),
    /**< 
        This error code is returned when the data call bringup fails in  
        VSNCP phase since network rejected VSNCP config request with  
        reason PDN exists for this APN
        Failure Type       : Permanent Failure
        Recovery Mechanism : This is a mismatch case where device  
                             doesn't have the PDN context for the APN 
                             but network is still maintaining the PDN 
                             context. Check with network provider
    */
  PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED     = TYPE_REASON_VAL(8,19),
  /**< 
      This error code is returned when the data call bringup fails in  
      VSNCP phase since network rejected VSNCP config request with  
      reconnect to this PDN not allowed, or, an active data call is
      terminated by NW with reason reconnect to this PDN not allowed.
      Upon receiving this error code from NW, modem will infinitely
      throttle the PDN until the next power cycle.
      Failure Type       : Permanent Failure
      Recovery Mechanism : power cycle
  */

  PS_NET_DOWN_REASON_EHRPD_ERR_MAX,

  /* IPv6 error codes */
  PS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE                = TYPE_REASON_VAL(9,1),
    /**< 
        This error code is returned when V6 data call is brought down  
        because device failed to get the prefix from network
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after the V6  
                             throttling timer expires. Throttling timer 
                             is maintained in the profile
    */
  PS_NET_DOWN_REASON_IPV6_ERR_HRPD_IPV6_DISABLED                = TYPE_REASON_VAL(9,2),
    /**< 
        This error code is returned when V6 data call bring up is  
        rejected because IPV6 is disabled in 1X/HRPD mode
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry V6 call origination after enabling  
                             V6 on HRPD configuration (NV# 65677)
    */
  PS_NET_DOWN_REASON_IPV6_DISABLED                              = TYPE_REASON_VAL(9,3),
   /**< 
   This error code is returned when IPv6 data call bring up is rejected
   because NV1896 (IPV6 enable) is disabled
   Failure Type : Permanent Failure
   Recovery Mechanism : Retry call origination after enabling NV1896
   */ 
  PS_NET_DOWN_REASON_IPV6_ERR_MAX,

  /* Tunnel mode error codes. Applicable only to EPC. Do not publish in DSS/QMI */
  PS_NET_DOWN_REASON_TUN_CONN_DENIED                            = TYPE_REASON_VAL(10,1),
    /**< 
        This error code is returned when preregistration fails because  
        virtual connection got rejected/released by HDR due to  
        connection deny
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration after some time
    */
  PS_NET_DOWN_REASON_TUN_CONN_CR_ROR_FAILED                     = TYPE_REASON_VAL(10,2),
    /**< 
        This error code is returned when preregistration fails because  
        virtual connection got rejected/released by HDR due to  
        connection request/reservation in request message failure
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration after some time
    */
  PS_NET_DOWN_REASON_TUN_CONN_SAP_L2_FAILED                     = TYPE_REASON_VAL(10,3),
    /**< 
        This error code is returned when preregistration fails because  
        virtual connection got rejected/released by HDR due to Signaling  
        Adaptation Protocol L2 ACK failure
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration after some time
    */
  PS_NET_DOWN_REASON_TUN_LCP_THROTTLED                          = TYPE_REASON_VAL(10,4),
    /**< 
        This error code is returned when preregistration request is  
        rejected because of previous preregistration failures in PPP
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration once S101 module  
                             notifies tunnel call is allowed 
                             after LCP throttling is cleared
    */
  PS_NET_DOWN_REASON_TUN_VIRTUAL_CONN_THROTTLED                 = TYPE_REASON_VAL(10,5),
    /**< 
        This error code is returned when preregistration request is  
        rejected because virtual connection is throttled due to previous  
        virtual connection bring up failures
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration once S101 module  
                             notifies tunnel call is allowed after 
                             virtual connection is unthrottled
    */
  PS_NET_DOWN_REASON_TUN_NON_EHPRD_MODE                         = TYPE_REASON_VAL(10,6),
    /**< 
        This error code is returned when preregistration request is  
        rejected because device does not have a tunnel mode eHRPD  
        session
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration once S101 module  
                             notifies tunnel call is allowed after 
                             device opens a tunnel mode eHRPD session
    */
  PS_NET_DOWN_REASON_TUN_NON_TUNNEL_MODE                        = TYPE_REASON_VAL(10,7),
    /**< 
        This error code is returned when preregistration request is  
        rejected because device is not operating in tunnel mode
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration once S101 module  
                             notifies tunnel call is allowed after 
                             device moves in to tunnel call allowed mode
    */
  PS_NET_DOWN_REASON_TUN_PREREG_DISALLOWED                      = TYPE_REASON_VAL(10,8),
    /**< 
        This error code is returned when preregistration request is  
        rejected because preregistration is disallowed
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration once S101 module  
                             notifies tunnel call is allowed after 
                             preregistration is allowed by HDR
    */
  PS_NET_DOWN_REASON_TUN_CONN_SAP_L3_FAILED                     = TYPE_REASON_VAL(10,9),
    /**< 
        This error code is returned when preregistration fails because  
        virtual connection got rejected/released by HDR due to Signaling  
        Adaptation Protocol L3 response failure
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration after some time
    */
  PS_NET_DOWN_REASON_TUN_CONN_SAP_TIMEOUT                       = TYPE_REASON_VAL(10,10),
    /**< 
        This error code is returned when preregistration fails because  
        virtual connection got rejected/released by HDR due to SAP  
        timeout
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration after some time
    */
  PS_NET_DOWN_REASON_TUN_CONN_SAP_NO_SESSION                    = TYPE_REASON_VAL(10,11),
    /**< 
        This error code is returned when preregistration fails because  
        virtual connection got rejected/released by HDR due to u 
        navailability of HDR session
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration after some time
    */
  PS_NET_DOWN_REASON_TUN_CONN_SAP_PREREG_NOT_ALLOW              = TYPE_REASON_VAL(10,12),
    /**< 
        This error code is returned when preregistration fails because  
        virtual connection got rejected/released by HDR due to  
        preregistration disallowed
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration after preregistration  
                             is allowed
    */
  PS_NET_DOWN_REASON_TUN_CONN_SAP_ACTIVE_HO                     = TYPE_REASON_VAL(10,13),
    /**< 
        This error code is returned when preregistration fails because  
        virtual connection got rejected/released by HDR due to Active  
        Handoff in progess
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration after some time
    */
  PS_NET_DOWN_REASON_TUN_CONN_SAP_NO_SRV                        = TYPE_REASON_VAL(10,14),
    /**< 
        This error code is returned when preregistration fails because  
        virtual connection got rejected/released by HDR due to LTE in  
        out of service
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration after device acquires 
                             LTE service
    */
  PS_NET_DOWN_REASON_TUN_NO_LTE_SRV                             = TYPE_REASON_VAL(10,15),
    /**< 
        This error code is returned when preregistration request is  
        rejected because device is not in LTE service
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration once S101 module  
                             notifies tunnel call is allowed after 
                             device acquires LTE service
    */
  PS_NET_DOWN_REASON_TUN_HDR_SESSION_CLOSED                     = TYPE_REASON_VAL(10,16),
    /**< 
        This error code is returned when eHRPD preregistration fails  
        because tunnel HDR session is in closed state
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry preregistration once S101 module  
                             notifies tunnel call is allowed when the 
                             tunnel HDR session opens

    */

  PS_NET_DOWN_REASON_IWLAN_S2B_PDN_CONN_REJ                     = TYPE_REASON_VAL(11,1),
  /**<   This reason is returned when connection establishment failed
         due to NW rejecting UE connection request      
         Failure Type       : Permanent Failure
         Recovery Mechanism : UE can attempt to re-establish this PDN
                              only when connected to a different ePDG or
                              one or more existing PDNs to the given APN
                              are released */

  PS_NET_DOWN_REASON_IWLAN_S2B_PDN_CONN_REJ_WITH_IP_ADDR_INFO     = TYPE_REASON_VAL(11,2),
  /**<   This reason is returned when connection establishment failed
         during a handoff scenario due to NW rejecting UE connection request      
         Failure Type       : Permanent Failure
         Recovery Mechanism : UE can attempt to re-establish this PDN
                              only when connected to a different ePDG */

  PS_NET_DOWN_REASON_IWLAN_S2B_MAX_CONN_REACHED                   = TYPE_REASON_VAL(11,3),
  /**<   This reason is returned when connection establishment failed
         due to maximum number of connections reached at the NW      
         Failure Type       : Permanent Failure
         Recovery Mechanism : UE can attempt to re-establish this PDN
                              only when connected to a different ePDG or
                              one or more existing PDNs are released  */

  PS_NET_DOWN_REASON_IWLAN_S2B_IKEV2_CONFIG_FAILURE               = TYPE_REASON_VAL(11,4),
  /**<   This reason is returned when there is failure during IKEv2
         configuration      
         Failure Type       : Permanent Failure
         Recovery Mechanism : Client should modify the IKEv2 config parameters
                              and retry. This can be modified in the iWLAN S2B
                              config file */
 
  PS_NET_DOWN_REASON_IWLAN_S2B_IKEV2_AUTH_FAILURE                 = TYPE_REASON_VAL(11,5),
  /**<   This reason is returned when data call bringup fails in IKEv2 setup
         due to authentication failure
         configuration      
         Failure Type       : Permanent Failure
         Recovery Mechanism : Retry call origination after correctly  
                              provisioning the device with authentication
                              credentials. This can be done in the card. */
 
  PS_NET_DOWN_REASON_IWLAN_S2B_IKEV2_MSG_TIMEOUT                  = TYPE_REASON_VAL(11,6),
  /**<   This reason is returned when data call bringup fails in  IKEv2 setup
         due to a timeout
         configuration      
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after some time */

  PS_NET_DOWN_REASON_IWLAN_S2B_DNS_RESOLUTION_CONFIG_FAILURE     = TYPE_REASON_VAL(11,12),    
  /**<   This reason is returned when data call bringup fails in retrieving the 
         address from DNS due to the config failure
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after some time */ 

  PS_NET_DOWN_REASON_IWLAN_S2B_DNS_RESOLUTION_NAME_FAILURE       = TYPE_REASON_VAL(11,13), 
  /**<   This reason is returned when data call bringup fails in retrieving the 
         address from DNS
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination with correct FQDN */

  PS_NET_DOWN_REASON_IWLAN_S2B_DNS_RESOLUTION_TIMEOUT            = TYPE_REASON_VAL(11,14), 
  /**<   This reason is returned when data call bringup fails in DNS sevrer 
         not responding 
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after some time */

  PS_NET_DOWN_REASON_IWLAN_S2B_BLOCKING_FAILURE                   = TYPE_REASON_VAL(11,15),
  /**<   This reason is returned when data call bringup fails because there are multiple 
         PDNs that are trying to come up on IWLAN on one epdg addr and we have already received one
         or more ikev2 failures.
         configuration      
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after some time */

 PS_NET_DOWN_REASON_IWLAN_S2B_DNS_RESOLUTION_FAILURE          = TYPE_REASON_VAL(11,18), 
  /**<   This reason is returned when data call bringup fails in DNS 
         resolution due to some internal failures 
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after some time */

  PS_NET_DOWN_REASON_IWLAN_S2B_NETWORK_FAILURE                   = TYPE_REASON_VAL(11,7),
  /**<   This reason is returned when data call bringup fails 
         due to network side failure
         
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after correctly  
                              provisioning the device with authentication
                              credentials. This can be done in the card. */

  PS_NET_DOWN_REASON_IWLAN_S2B_ROAMING_NOT_ALLOWED               = TYPE_REASON_VAL(11,8),
  /**<   This reason is returned when data call bringup fails 
         due to roaming not allowed
         
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after UE moves back to 
                              home network */

  PS_NET_DOWN_REASON_IWLAN_S2B_RAT_DISALLOWED                    = TYPE_REASON_VAL(11,9),   
  /**<   This reason is returned when data call bringup fails 
         due to network not allowing IWLAN RAT
     
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after UE moves to a 
                              different RAT */

  PS_NET_DOWN_REASON_IWLAN_S2B_NO_SUBSCRIPTION                   = TYPE_REASON_VAL(11,10),
   /**<  The reason is posted when UE requested a service option for which it 
         has no subscription.  

         Failure Type       : Temporary Failure
         Recovery Mechanism : If the PDN is throttled, the client shall
                              wait for the corresponding time period before 
                              retrying the call. If the PDN is not throttled, 
                              the client may retry immediately.*/

  PS_NET_DOWN_REASON_IWLAN_S2B_NETWORK_TOO_BUSY                  = TYPE_REASON_VAL(11,11),
  /**<   The reason is posted when the network is busy to accept new 
         connections.  
  
        Failure Type       : Temporary Failure
        Recovery Mechanism : If the PDN is throttled, the client shall
                             wait for the corresponding time period before 
                             retrying the call. If the PDN is not throttled, 
                             the client may retry immediately.*/

 PS_NET_DOWN_REASON_IWLAN_S2B_IKEV2_PROTOCOL_FAILURE          = TYPE_REASON_VAL(11,19), 
  /**<   This reason is returned when data call bringup fails due to IPSEC failure
         Failure Type       : Permenant Failure
         Recovery Mechanism : Retry call origination after some time */         

  PS_NET_DOWN_REASON_IWLAN_S2B_NW_INIT_CONN_DOWN         = TYPE_REASON_VAL(11,20), 
   /**<   This reason is returned when data call bringup fails due to network initiated down
          Failure Type       : Temporary Failure
          Recovery Mechanism : Retry call origination after some time */         

  PS_NET_DOWN_REASON_IWLAN_S2B_IKEV2_DPD_FAILURE          = TYPE_REASON_VAL(11,21), 
   /**<   This reason is returned when data call bringup fails due to DPD failure
          Failure Type       : Temporary Failure
          Recovery Mechanism : Retry call origination after some time */         

  PS_NET_DOWN_REASON_IWLAN_S2B_IKE_REKEY_FAILURE               = TYPE_REASON_VAL(11,22),
  /**<   This reason is returned when there is failure during IKE rekeying
         configuration      
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after some time */       

  PS_NET_DOWN_REASON_IWLAN_S2B_IPSEC_REKEY_FAILURE               = TYPE_REASON_VAL(11,23),
  /**<   This reason is returned when there is failure during IPSec rekeying
         configuration      
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after some time */       

  /* Handoff error codes */
  PS_NET_DOWN_REASON_HANDOFF_PREF_SYS_BACK_TO_SRAT                = TYPE_REASON_VAL(12,1),
   /**<
      This error code is returned when handoff failed because 
      system preference change back to SRAT in the middle of handoff
      Failure Type : Temporary Failure
      Recovery Mechanism : Call may still be up on source RAT and data transfer will continue on source RAT. 
   */

  PS_NET_DOWN_REASON_MAX             /* DO NOT USE. Used for bounds check */
} ps_iface_net_down_reason_type;


typedef enum
{
  PS_IFACE_MCAST_IC_NOT_SPECIFIED             = 0,

  /* BCMCS info codes #'s 1-1000 */
  PS_IFACE_MCAST_BCMCS_FLOW_STATUS_CANCELLED = 100,

  PS_IFACE_MCAST_BCMCS_FLOW_STATUS_UNABLE_TO_MONITOR = 300,
  PS_IFACE_MCAST_BCMCS_FLOW_STATUS_REQUESTED = 301,
  PS_IFACE_MCAST_BCMCS_FLOW_STATUS_TIMEOUT = 302,
  PS_IFACE_MCAST_BCMCS_FLOW_STATUS_LOST = 303,
  PS_IFACE_MCAST_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE = 304,

  PS_IFACE_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_AVAILABLE = 400,
  PS_IFACE_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_TRANSMITTED = 401,
  PS_IFACE_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_INVALID_AUTH_SIG = 402,

  PS_IFACE_MCAST_BCMCS_FLOW_STATUS_UNAVAILABLE = 500,
  PS_IFACE_MCAST_BCMCS_FLOW_STATUS_NO_MAPPING = 501,
  PS_IFACE_MCAST_BCMCS_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP = 502,

  PS_IFACE_MCAST_BCMCS_MAX_FLOWS_REACHED = 503,
  PS_IFACE_MCAST_BCMCS_MAX_DEPRECATED_INFO_CODE = 504, /* Backward compatibility */

  /* BCMCS 2.0: Info codes (range 600 onwards) */
  /* Some of the above info codes are re-written as 2p0 info codes */
  PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_CANCELLED = 600,
  PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_UNABLE_TO_MONITOR = 601,
  PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_TIMEOUT = 602,
  PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_LOST = 603,
  PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_SYS_UNAVAILABLE = 604,
  PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_UNAVAILABLE = 605,

  PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_AN_REJECT_NOT_AVAILABLE = 606,
  PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_AN_REJECT_NOT_TRANSMITTED = 607,
  PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_AN_REJECT_INVALID_AUTH_SIG = 608,

  PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_NO_MAPPING = 609,
  PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP = 610,
  PS_IFACE_MCAST_BCMCS2p0_FLOW_STATUS_REQUESTED = 611,
  PS_IFACE_MCAST_BCMCS2p0_MAX_FLOWS_REACHED = 612,

  /* BCMCS 2.0 New info codes */
  PS_IFACE_MCAST_BCMCS_JOIN_REQ_IN_PROGRESS = 613,
  PS_IFACE_MCAST_BCMCS_FLOW_REQUEST_SENT = 614,
  PS_IFACE_MCAST_BCMCS_FLOW_STATUS_MAX_MONITORED_FLOWS = 615,
  PS_IFACE_MCAST_BCMCS_FLOW_STATUS_MONITORED = 616,
  PS_IFACE_MCAST_BCMCS_REGISTRATION_SUCCESS = 617,
  PS_IFACE_MCAST_BCMCS_REGISTRATION_NOT_ALLOWED = 618,
  PS_IFACE_MCAST_BCMCS_REGISTRATION_FAILED = 619,
  PS_IFACE_MCAST_BCMCS_FLOW_DEREGISTERED = 620,

  /* MediaFLO info codes #'s 1051-1100*/
  PS_IFACE_MCAST_FLO_IP_OR_PORT_NOT_SUPPORTED = 1051,
  PS_IFACE_MCAST_FLO_NO_AUTHORIZATION         = 1052,
  PS_IFACE_MCAST_FLO_NO_SYSTEM_COVERAGE       = 1053,
  PS_IFACE_MCAST_FLO_MAX_FLOW_REACHED         = 1054,

/* DVB-H info codes, #1101-1150 */
  PS_IFACE_MCAST_DVBH_IP_OR_PORT_NOT_FOUND    = 1101,
  PS_IFACE_MCAST_DVBH_SYSTEM_UNAVAILABLE      = 1102,
  PS_IFACE_MCAST_DVBH_BAD_REQUEST             = 1103,
  PS_IFACE_MCAST_DVBH_REQUEST_CONFLICT        = 1104,
  PS_IFACE_MCAST_DVBH_DUP_REQUEST             = 1105,
  PS_IFACE_MCAST_DVBH_MAX_FLOWS_REACHED       = 1106,

/* MBMS  info codes, #1151-1200 */
  PS_IFACE_MCAST_MBMS_SYSTEM_UNAVAILABLE = 1151

  /* Any additional info code */
} ps_iface_mcast_info_code_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_HDR_MODE_FAILURE_REASON_CODE_ENUM_TYPE

DESCRIPTION
  Enum type for HDR mode failure
---------------------------------------------------------------------------*/
typedef enum
{
  PS_HDR_REV0_RATE_INERTIA_REQUEST_REJECTED = 0, /* Request to change rate
                                                    params rejected */
  PS_HDR_REV0_RATE_INERTIA_REQUEST_FAILED_TX,    /* Failed transmission */
  PS_HDR_REV0_RATE_INERTIA_NOT_SUPPORTED,        /* RMAC subtype RMAC1 */
  PS_HDR_REV0_RATE_INERTIA_NO_NET,               /* Not idle or connected */
  PS_HDR_REV0_RATE_INERTIA_FAILURE_REASON_MAX    /* DO NOT USE. Used for
                                                    bounds check */
} ps_hdr_rev0_rate_inertia_failure_code_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_HDR_MODE_FAILURE_REASON_CODE_ENUM_TYPE

DESCRIPTION
  Enum type for HDR mode failure
---------------------------------------------------------------------------*/
typedef enum
{
  PS_HDR_SLOTTED_MODE_REQUEST_REJECTED = 0, /* Request to change slot
                                               cycle index rejected        */
  PS_HDR_SLOTTED_MODE_REQUEST_FAILED_TX,    /* Failed transmission         */
  PS_HDR_SLOTTED_MODE_NOT_SUPPORTED,        /* Slotted mode not supported  */
  PS_HDR_SLOTTED_MODE_NO_NET,               /* Not idle or connected       */
  PS_HDR_SLOTTED_MODE_FAILURE_REASON_MAX    /* DO NOT USE. Used for
                                               bounds check                */
} ps_hdr_slotted_mode_failure_code_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_ADDR_FAMILY_TYPE

DESCRIPTION
  The type that is used to define the IP type that an interface supports.
---------------------------------------------------------------------------*/
typedef ip_addr_enum_type ps_iface_addr_family_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_BEARER_TECHNOLOGY_TYPE

DESCRIPTION
  The type that is used to define the bearer technology type.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IFACE_NETWORK_MIN = 0,
  PS_IFACE_NETWORK_CDMA,
  PS_IFACE_NETWORK_UMTS,
  PS_IFACE_NETWORK_3GPP = PS_IFACE_NETWORK_UMTS,         /* Alias for UMTS */
  PS_IFACE_NETWORK_WLAN,
  PS_IFACE_NETWORK_MAX
} ps_iface_network_type;

/*---------------------------------------------------------------------------
  APN type enum
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IFACE_APN_TYPE_UNSPECIFIED  = 0,
  PS_IFACE_APN_TYPE_IMS          = 1
}ps_iface_apn_enum_type;

typedef struct
{
  /*-------------------------------------------------------------------------
    rat_mask specifies the type of technology
  -------------------------------------------------------------------------*/
  uint32  rat_mask;
  #define PS_IFACE_CDMA_1X               0x01
  #define PS_IFACE_CDMA_EVDO_REV0        0x02
  #define PS_IFACE_CDMA_EVDO_REVA        0x04
  #define PS_IFACE_CDMA_EVDO_REVB        0x08
  #define PS_IFACE_CDMA_EVDO_EHRPD       0x10
  #define PS_IFACE_CDMA_FMC              0x20
  #define PS_IFACE_CDMA_NULL_BEARER      0x8000

  /*-------------------------------------------------------------------------
    so_mask specifies the service option or type of application
  -------------------------------------------------------------------------*/
  uint32  so_mask;

  /*-------------------------------------------------------------------------
    Following are 1x specific so_masks
  -------------------------------------------------------------------------*/
  #define PS_IFACE_CDMA_1X_IS95                0x01
  #define PS_IFACE_CDMA_1X_IS2000              0x02
  #define PS_IFACE_CDMA_1X_IS2000_REL_A        0x04

  /*-------------------------------------------------------------------------
    Following are specific so_masks for Packet Applications on HDR

    The following table gives which of the following so_masks are supported
    by which technology
          +-------------------+----------------------------------+
          | Rev0 supports     |    PS_IFACE_CDMA_EVDO_DPA        |
          +-------------------+----------------------------------+
          | RevA supports     |    PS_IFACE_CDMA_EVDO_DPA        |
          |                   |    PS_IFACE_CDMA_EVDO_MFPA       |
          |                   |    PS_IFACE_CDMA_EVDO_EMPA       |
          |                   |    PS_IFACE_CDMA_EVDO_EMPA_EHRPD |
          +-------------------+----------------------------------+
          | RevB supports     |    PS_IFACE_CDMA_EVDO_DPA        |
          |                   |    PS_IFACE_CDMA_EVDO_MFPA       |
          |                   |    PS_IFACE_CDMA_EVDO_EMPA       |
          |                   |    PS_IFACE_CDMA_EVDO_EMPA_EHRPD |
          |                   |    PS_IFACE_CDMA_EVDO_MMPA       |
          |                   |    PS_IFACE_CDMA_EVDO_MMPA_EHRPD |
          +-------------------+----------------------------------+
  -------------------------------------------------------------------------*/
  #define PS_IFACE_CDMA_EVDO_DPA          0x01
  #define PS_IFACE_CDMA_EVDO_MFPA         0x02
  #define PS_IFACE_CDMA_EVDO_EMPA         0x04
  #define PS_IFACE_CDMA_EVDO_EMPA_EHRPD   0x08
  #define PS_IFACE_CDMA_EVDO_MMPA         0x10
  #define PS_IFACE_CDMA_EVDO_MMPA_EHRPD   0x20

  /*-------------------------------------------------------------------------
    DEPRECATE: As packet Applications are not dependent on Rev0/RevA/RevB
               Use the ones defined.
    Following are DoRA specific so_masks
  -------------------------------------------------------------------------*/
  #define PS_IFACE_CDMA_EVDO_REVA_DPA          PS_IFACE_CDMA_EVDO_DPA
  #define PS_IFACE_CDMA_EVDO_REVA_MFPA         PS_IFACE_CDMA_EVDO_MFPA
  #define PS_IFACE_CDMA_EVDO_REVA_EMPA         PS_IFACE_CDMA_EVDO_EMPA
  #define PS_IFACE_CDMA_EVDO_REVA_EMPA_EHRPD   PS_IFACE_CDMA_EVDO_EMPA_EHRPD

} ps_iface_network_cdma_type;

typedef struct
{
  uint32  rat_mask;
  /*-------------------------------------------------------------------------
    ATTENTION: Following definitions are DEPRECATED. Add any new deifnitions
    to the "PS_IFACE_3GPP" section below
  -------------------------------------------------------------------------*/

  #define PS_IFACE_UMTS_WCDMA             0x01
  #define PS_IFACE_UMTS_GPRS              0x02
  #define PS_IFACE_UMTS_HSDPA             0x04
  #define PS_IFACE_UMTS_HSUPA             0x08
  #define PS_IFACE_UMTS_EDGE              0x10

  /*-------------------------------------------------------------------------
    Aliases to all the above deprecated definitions.
  -------------------------------------------------------------------------*/
  #define PS_IFACE_3GPP_WCDMA             PS_IFACE_UMTS_WCDMA
  #define PS_IFACE_3GPP_GPRS              PS_IFACE_UMTS_GPRS
  #define PS_IFACE_3GPP_HSDPA             PS_IFACE_UMTS_HSDPA
  #define PS_IFACE_3GPP_HSUPA             PS_IFACE_UMTS_HSUPA
  #define PS_IFACE_3GPP_EDGE              PS_IFACE_UMTS_EDGE
  #define PS_IFACE_3GPP_LTE               0x20
  #define PS_IFACE_3GPP_HSDPAPLUS         0x40 /* HSDPA+ hs_status */
  #define PS_IFACE_3GPP_DC_HSDPAPLUS      0x80 /* Dual Carrier HSDPA+
                                                         hs_status */
  #define PS_IFACE_3GPP_64_QAM            0x100
  #define PS_IFACE_3GPP_TDSCDMA           0x200
  #define PS_IFACE_3GPP_NULL_BEARER       0x8000
} ps_iface_network_umts_type;

typedef struct
{
  ps_iface_network_type  current_network;
  union
  {
    ps_iface_network_cdma_type      cdma_type;
    ps_iface_network_umts_type      umts_type;
  }data;
} ps_iface_bearer_technology_type;

/*---------------------------------------------------------------------------
  Serving mode change information.
---------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_bearer_technology_type      old_bearer_tech;
  ps_iface_bearer_technology_type      new_bearer_tech;
} ps_iface_ioctl_bearer_tech_changed_type;

/*---------------------------------------------------------------------------
  Extended IP config event data
---------------------------------------------------------------------------*/
typedef boolean ps_iface_ioctl_extended_ip_config_type;

/**
  @brief IP Configuration change mask values.
         These values are set and propagated along with the following event:
         IFACE_EXTENDED_IP_CONFIG_EX_EV
         The various mask values indicate which IP Config parmeter has been 
         updated.
*/
/**< No change in extended ip config */
#define PS_IFACE_EXT_IP_CFG_MASK_NONE             (0x00)

/**< This mask values is set when DNS server IP address changes as compared 
     to previously configured value */
#define PS_IFACE_EXT_IP_CFG_MASK_DNS_ADDR         (0x10)

/**< This mask value is set when Gateway address changes compared to 
     previously configured value */
#define PS_IFACE_EXT_IP_CFG_MASK_GATEWAY          (0x0200)

/**< This mask value is set when PCSCF server's IP address changes compared
     to previously configured value */
#define PS_IFACE_EXT_IP_CFG_MASK_PCSCF_ADDR       (0x0800)

/**< This mask value is set when PCSCF server's URL changes compared to 
     previously configured value */
#define PS_IFACE_EXT_IP_CFG_MASK_PCSCF_NAME       (0x1000)

/**< This mask value is set when IP MTU value changes compared to 
     previously configured value */
#define PS_IFACE_EXT_IP_CFG_MASK_MTU              (0x2000)

/**< This mask value is set when DNS server URL changes compared to 
     previously configured value */
#define PS_IFACE_EXT_IP_CFG_MASK_DNS_URL_LIST     (0x4000)

/**< This mask value is set when Operator reserved PCO flag changes compared
     to previously configured value */
#define PS_IFACE_EXT_IP_CFG_MASK_OP_RES_PCO       (0x40000)

/**
  @brief Extended IP config EX event data : IFACE_EXTENDED_IP_CONFIG_EX_EV
         Mask value indicating what all config items have changed.
         This is an extension to the extended ip config event which only 
         denotes whether or not config is fetched.
*/
typedef uint32 ps_iface_ioctl_extended_ip_config_ex_type;

/*---------------------------------------------------------------------------
  The bearer data rate information. A value of -1 would indicate
  an unknown value.
  max Tx/Rx - As defined by the technology.
  avg Tx/Rx - Rate at which data was transferred over the last sampling
              period (currently 1 second)
  current Tx/Rx - Estimate of the rate that can be supported at this time.
---------------------------------------------------------------------------*/

typedef struct
{
  int32 max_tx_bearer_data_rate;      /*Max Tx bearer data rate*/
  int32 max_rx_bearer_data_rate;      /*Max Rx bearer data rate*/
  int32 avg_tx_bearer_data_rate;      /*Average Tx bearer data rate*/
  int32 avg_rx_bearer_data_rate;      /*Average Rx bearer data rate*/
  int32 current_tx_bearer_data_rate;  /*Current Tx bearer data rate*/
  int32 current_rx_bearer_data_rate;  /*Current Rx bearer data rate*/
} ps_iface_data_bearer_rate;

/*---------------------------------------------------------------------------
  Outage notification information
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  time_to_outage;      /* milliseconds in which outage will start  */
  uint32  duration;            /* milliseconds for which outage will last  */
} ps_iface_outage_notification_event_info_type;

/*---------------------------------------------------------------------------
  Enum to indicate the DOS ack status code. The pkt mgr receives the status
  from CM and maps it to this enum. Changes to CM/Pkt mgr may necessitate
  modifying this enum definition.
---------------------------------------------------------------------------*/
typedef enum ps_phys_link_dos_ack_status_enum_type
{
  PS_PHYS_LINK_DOS_ACK_NONE = -1,           /* completed successfully??SURE?? */
  PS_PHYS_LINK_DOS_ACK_OK = 0,                   /* completed successfully */
  PS_PHYS_LINK_DOS_ACK_HOLD_ORIG_RETRY_TIMEOUT = 1,  /* hold orig retry
                                                        timeout            */
  PS_PHYS_LINK_DOS_ACK_HOLD_ORIG = 2,   /* cannot process because hold orig
                                           is true                         */
  PS_PHYS_LINK_DOS_ACK_NO_SRV = 3,              /* no service              */
  PS_PHYS_LINK_DOS_ACK_ABORT = 4,               /* abort                   */
  PS_PHYS_LINK_DOS_ACK_NOT_ALLOWED_IN_AMPS = 5, /* cannot send in analog mode*/
  PS_PHYS_LINK_DOS_ACK_NOT_ALLOWED_IN_HDR = 6,  /* cannot send in HDR call */
  PS_PHYS_LINK_DOS_ACK_L2_ACK_FAILURE = 7,      /* failure receiving L2 ack */
  PS_PHYS_LINK_DOS_ACK_OUT_OF_RESOURCES = 8,    /* e.g., out of memory buffer*/
  PS_PHYS_LINK_DOS_ACK_ACCESS_TOO_LARGE = 9,    /* msg too large to be sent
                                                   over acc                */
  PS_PHYS_LINK_DOS_ACK_DTC_TOO_LARGE = 10,      /* msg too large to be sent
                                                   over DTC                */
  PS_PHYS_LINK_DOS_ACK_OTHER = 11,              /* any status response other
                                                   than above              */
  PS_PHYS_LINK_DOS_ACK_ACCT_BLOCK = 12,         /* Access blocked based on
                                                   service option          */
  PS_PHYS_LINK_DOS_ACK_L3_ACK_FAILURE = 13,    /* failure receiving L3 ack */
  PS_PHYS_LINK_DOS_ACK_FORCE_32_BIT = 0x7FFFFFFF
} ps_phys_link_dos_ack_status_enum_type;

/*---------------------------------------------------------------------------
  The structure containing information about the sdb ack status. This info
  is returned to the socket application in the sdb ack callback if the sdb
  ack callback socket option is set.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                                 overflow;
  ps_phys_link_dos_ack_status_enum_type  status;
} ps_phys_link_dos_ack_status_info_type;

/*---------------------------------------------------------------------------
  This structure defines a type to notify of RF condition in a cell
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IFACE_RF_CONDITIONS_MIN       = 0,
  PS_IFACE_RF_CONDITIONS_INVALID   = PS_IFACE_RF_CONDITIONS_MIN,
  PS_IFACE_RF_CONDITIONS_BAD       = 1,
  PS_IFACE_RF_CONDITIONS_GOOD      = 2,
  PS_IFACE_RF_CONDITIONS_DONT_CARE = 3,
  PS_IFACE_RF_CONDITIONS_MAX
} ps_iface_rf_conditions_enum_type;

typedef struct
{
  ps_iface_bearer_technology_type   bearer_tech;
  ps_iface_rf_conditions_enum_type  rf_conditions;
} ps_iface_rf_conditions_info_type;

/*---------------------------------------------------------------------------
  Domain name type. Used for SIP/DNS server domain names.
---------------------------------------------------------------------------*/
typedef struct
{
  char domain_name[PS_IFACE_MAX_DOMAIN_NAME_SIZE];
} ps_iface_domain_name_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_PREFIX_UPDATE_ENUM_TYPE

DESCRIPTION
  Contains the enums describing the action for the accompanying prefix.
---------------------------------------------------------------------------*/

typedef enum
{
  PREFIX_ADDED      = 0, /* Added a prefix          */
  PREFIX_REMOVED    = 1, /* Removed a prefix        */
  PREFIX_DEPRECATED = 2, /* State of prefix changed */
  PREFIX_UPDATED    = 3  /* Lifetimes of prefix have been updated */
} ps_iface_prefix_update_enum_type;

/*===========================================================================
                            FORWARD DECLARATIONS
===========================================================================*/
typedef struct ps_iface_s      ps_iface_type;
typedef struct ps_flow_s       ps_flow_type;
typedef struct ps_phys_link_s  ps_phys_link_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_IPFLTR_CLIENT_ID_ENUM_TYPE

DESCRIPTION
  Client Ids for the users of filtering library.
  NOTE : INPUT filter clients must have a client ID between 0 and
         IP_FLTR_CLIENT_INPUT_MAX
---------------------------------------------------------------------------*/
typedef enum
{
  IP_FLTR_CLIENT_SOCKETS      = 0,  /* Incoming pkt filtering for sockets  */
  IP_FLTR_CLIENT_QOS_INPUT    = 1,  /* Rx Pkt classification fltrs for QOS */
  IP_FLTR_CLIENT_IPSEC_INPUT  = 2,  /* IPSEC filters for input processing  */
  IP_FLTR_CLIENT_FIREWALL_INPUT  = 3,  /* Firewall filters for input
                                                                processing */
  IP_FLTR_CLIENT_POWERSAVE ,           /* Power save filters for input     */
  IP_FLTR_CLIENT_INPUT_MAX,
  IP_FLTR_CLIENT_QOS_OUTPUT,        /* Tx Pkt classification fltrs for QOS */
  IP_FLTR_CLIENT_IPSEC_OUTPUT,      /* IPSEC filters for output processing */
  IP_FLTR_CLIENT_HEADER_COMP,       /* Header compression protocol         */
  IP_FLTR_CLIENT_MBIM,              /* MBIM filters */
  IP_FLTR_CLIENT_CLAT,              /* CLAT filters */
  IP_FLTR_CLIENT_MAX
} ps_iface_ipfltr_client_id_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_DORM_ACTION_ENUM_TYPE

DESCRIPTION
  List of all the possible dormancy actions that phys link may want to do
  and may ask iface if it's okay to do that action.
---------------------------------------------------------------------------*/
typedef enum
{
  IFACE_DORM_ACTION_MIN = 0,      /* DO NOT USE.  Used for bounds checking */
  IFACE_DORM_ACTION_OK_TO_ORIG,   /* is it okay to originate from dormancy */
  IFACE_DORM_ACTION_OK_TO_GO_DORM,/* is it ok to go dormant                */
  IFACE_DORM_ACTION_MAX           /* DO NOT USE.  Used for bounds checking */
} ps_iface_dorm_action_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_DORM_REASON_ENUM_TYPE

DESCRIPTION
  Contains reasons phys link wants to originate from dormancy.
---------------------------------------------------------------------------*/
typedef enum
{
  IFACE_DORM_REASON_MIN = 0,      /* DO NOT USE.  Used for bounds checking */
  IFACE_DORM_REASON_DATA,         /* has data to send                      */
  IFACE_DORM_REASON_PZ_S_NID,     /* PZID/SID/NID changed                  */
  IFACE_DORM_REASON_NETWORK,      /* network changed                       */
  IFACE_DORM_REASON_MAX           /* DO NOT USE.  Used for bounds checking */
} ps_iface_dorm_reason_enum_type;

typedef enum
{
  PS_FAST_DORMANCY_STATUS_SUCCESS = 0,
  PS_FAST_DORMANCY_STATUS_FAILURE
} ps_iface_fast_dormancy_status_enum_type;

typedef enum
{
  PS_FAST_DORMANCY_STATUS_FAIL_NONE = 0,
  PS_FAST_DORMANCY_STATUS_FAIL_RRC,
  PS_FAST_DORMANCY_STATUS_FAIL_DATA_ACTIVITY,
  PS_FAST_DORMANCY_STATUS_FAIL_OTHER_PDN_UP,
  PS_FAST_DORMANCY_STATUS_FAIL_INVALID_PDN_STATE,
  PS_FAST_DORMANCY_STATUS_FAIL_MAX
} ps_iface_fast_dormancy_failure_reason_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_FAST_DORMANCY_STATUS_TYPE

DESCRIPTION
  Contains status of the request and failure reason.
---------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_fast_dormancy_status_enum_type  dorm_status;
  ps_iface_fast_dormancy_failure_reason_enum_type failure_reason;
} ps_iface_fast_dormancy_status_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_DATA_PATH_TYPE

DESCRIPTION
  Contains data path types
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IFACE_IOCTL_DATA_PATH_BRIDGE_MODE_HW, /* Hardware Data Path */
  PS_IFACE_IOCTL_DATA_PATH_BRIDGE_MODE_SW  /* Software Data Path */
} ps_iface_data_path_bridge_mode_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_PREFIX_UPDATE_TYPE

DESCRIPTION
  This contains all of the prefix information related to the update.

MEMBERS:
  prefix: the prefix that changed.
  n_prefixes: the number of prefixes
  kind: added, changed, state change (i.e. preferred -> deprecated)
---------------------------------------------------------------------------*/
typedef struct
{
  struct ps_in6_addr prefix;
  ps_iface_prefix_update_enum_type kind;
  uint8 prefix_len;
} ps_iface_prefix_update_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_PREFIX_INFO_TYPE

DESCRIPTION
  This contains all of the prefix information.

MEMBERS:
  prefix:       the prefix that changed
  prefix_state: the state of the prefix
  prefix_len:   the length of the prefix
---------------------------------------------------------------------------*/
typedef struct
{
  struct ps_in6_addr                 prefix;
  ps_iface_ipv6_addr_state_enum_type prefix_state;
  uint8                              prefix_len;
} ps_iface_prefix_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_PRIV_IPV6_ADDR_INFO_TYPE

DESCRIPTION
  This contains the private IPv6 address and type.

MEMBERS:
  ip_addr: the IPv6 privacy address.
  is_unique: whether the address is private shared or private unique
---------------------------------------------------------------------------*/
typedef struct
{
  ps_ip_addr_type   ip_addr;
  boolean           is_unique;
} ps_iface_priv_ipv6_addr_info_type;

typedef struct
{
  ps_iface_mcast_handle_type             handle;
  ps_iface_mcast_info_code_enum_type     info_code;
  boolean                                force_dereg_cbacks;
} ps_iface_mcast_event_info_type;

/**
  @brief Handle to network initiated qos request.
*/
typedef int32 ps_iface_net_initiated_qos_req_handle_type;

/**
  @brief This contains the flow handle and net initiated qos request handle.
*/
typedef struct
{
   ps_iface_net_initiated_qos_req_handle_type   handle;
   /**< Handle to net initiated qos request */
   ps_flow_type                               * flow_ptr;
   /**< Flow, whose filters satisfy the net initiated qos request */
} ps_iface_net_initiated_qos_available_info_type;

/*
  Event info populated by tech mode handlers when IFACE DOWN is posted 
  on the iface.
*/
typedef struct
{
  ps_iface_net_down_reason_type    down_reason;
  ds_sys_system_status_info_type   bearer_tech;
} ps_iface_down_mh_event_info_type;

/*
  Event info associated with IFACE_OUT_OF_USE_EV/IFACE_DOWN_EV.
*/
typedef struct
{
  ps_iface_state_enum_type       state;
  ps_iface_net_down_reason_type  netdown_reason;
  uint32                         proc_id;
  ps_iface_addr_family_type      addr_family;
  ds_sys_system_status_info_type bearer_tech;
} ps_iface_down_event_info_type;

/*
  Event info associated with IFACE_UP_EV/IFACE_IN_USE_EV
*/
typedef struct
{
  ps_iface_state_enum_type       state;
  uint32                         proc_id;
  boolean                        is_iface_in_use;
} ps_iface_up_event_info_type;

/* 
   Argument to be passed with
   PS_IFACE_IOCTL_SET_DATA_PATH/PS_IFACE_IOCTL_GET_DATA_PATH
*/
typedef struct
{
  ps_iface_data_path_bridge_mode_enum_type        data_path;
} ps_iface_ioctl_data_path_bridge_mode_type;

/**
  @brief Contains status of IPFLTRS.
*/
typedef enum
{
  PS_IFACE_IPFLTRS_ADDED,   /* Filters have been added */
  PS_IFACE_IPFLTRS_DELETED  /* Filters have been deleted */
} ps_iface_ipfltrs_status_enum_type;

/**
  @brief Argument to be passed with 
         PS_IFACE_IOCTL_REVERSE_IP_TRANSPORT_GET_IPFLTRS.
*/
typedef struct
{
  uint8                   list_ptr_size;  /* Size in num filters for memory allocated for list_ptr */       
  ip_filter_spec_type     filters;
} ps_iface_ipfltr_list_type;

/**
  @brief Argument to be passed with 
         IFACE_REVERSE_IP_TRANSPORT_IPFLTR_CHANGED_EV and
         PS_IFACE_IOCTL_IPFLTR_CHANGED.
*/
typedef struct
{
  ps_iface_ipfltrs_status_enum_type   status;
  ip_filter_spec_type                 filters;
} ps_iface_ipfltr_change_type;

/**
  @brief APN PARAM CHANGES MASK VALUE 
         The mask indicates the parameters that have changed.
*/
/**< No change in any params */ 
#define PS_IFACE_APN_PARAM_NONE                 (0x00000000)

/**< This mask indicates apn_name change */
#define PS_IFACE_APN_PARAM_APN_NAME             (0x00000001)

/**< This mask indicates pdp_type changed */
#define PS_IFACE_APN_PARAM_PDP_TYPE             (0x00000002)


typedef struct
{
  char apn_name[DS_SYS_MAX_APN_LEN];
  /**< Container to store APN name*/

  uint32 pdp_type;
  /**< Container to store PDN's IP Type */

} ps_iface_apn_param_value_type; 

/**
  @brief Argument to be passed with IFACE_APN_PARAM_CHANGED_EV and 
*/
typedef struct
{
  uint32 apn_mask;
  /**< Apn Mask to specify which parameter changed*/
  ps_iface_apn_param_value_type old_apn_param_values;
   /**< Old Apn Parameter Information*/
  ps_iface_apn_param_value_type new_apn_param_values;
  /**< New Apn Parameter Information */
} ps_iface_changed_apn_param_info_type;

typedef struct
{
  ds_sys_rat_ex_enum_type  srat;
  ds_sys_rat_ex_enum_type  trat;
} ps_iface_handoff_event_info_type;

/*For failure event need to include the failure reason to know what is the exact reason for handoff failure 
   and also based on this clients can decide whether to retry or not*/
typedef struct
{
  ds_sys_rat_ex_enum_type        srat;
  ds_sys_rat_ex_enum_type        trat;
  ps_iface_net_down_reason_type  ho_failure_reason;
} ps_iface_handoff_failure_event_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_EVENT_INFO_U_TYPE

DESCRIPTION
  Data structure that is used to pass event specific information into the
  callback functions that are registered for the events.

MEMBERS
  state: This will be set when the interface state change(*) callback is
    called and will contain the previous state the interface was in.
    (*) This applies to the IFACE_DOWN_EV and IFACE_UP_EV events.
  link_state: This will be set when the physical link state change(*)
    callback is called and will contain the previous state the link was in.
    (*) this applies to the IFACE_PHYS_LINK_DOWN_EV and IFACE_PHYS_LINK_UP_EV
  flow_mask: This will be set when the flow state(*) is changed, that is,
    flow is enabled, or disabled.  It will contain the previous flow_mask.
    (*) This applies to the IFACE_FLOW_ENABLED_EV and IFACE_FLOW_DISABLED_EV
  ip_addr: This will be set when the IP address of the iface changes.
    Will be set to the previous IP address.
    (*) This applies to IFACE_ADDR_CHANGED_EV.
  ipfltr_info: This will be set when the number of filters installed in the
    iface for a particular client changes. "cnt" will be set to the previous
    number of filters in the iface for this "client".
    (*) This applies to IFACE_IPFLTR_UPDATED_EV.
  prefix_info: the information related to the prefix update.  Applies to
    IFACE_PREFIX_UPDATE_EV.
  phys_link_event_info: same as link_state but also contains extended
    information code to describing the cause of the state change.
  pri_phys_link_ptr: indicates previous primary phys link, applies to
  IFACE_PRI_PHYS_LINK_CHANGED_EV.
 addr_family_info: This will be set when address family of iface changes.
  powersave_filtering_info: Indicates whether powersave filtering mode is
    enabled/disabled.
    (*) This applies to IFACE_ADDR_FAMILY_CHANGED_EV.
 iface_down_info:  This will be set to indicate the motivation for the
    PS_iface going down during the down indication.
bearer_tech_changed_info: This would be set when the bearer technology
  changes.
  (*) This applies to IFACE_BEARER_TECH_CHANGED_EV.
  iface_net_initiated_qos_available_info: event info for
                                        IFACE_NET_INITIATED_QOS_AVAILABLE_EV
   ps_iface_changed_apn_param_info_type: Indicates the old and new APN Parameter
   values

---------------------------------------------------------------------------*/
typedef union
{
  struct
  {
    ps_iface_ipfltr_client_id_enum_type  client;
    int                                  curr_fltr_cnt;
  } ipfltr_info;

  struct
  {
    boolean                              powersave_mode;
  } powersave_filtering_info;

  struct
  {
    ps_flow_type  * flow_ptr;
    uint8           prev_cnt;
    uint8           new_cnt;
  } rx_fltr_info;

  struct
  {
    ps_flow_state_enum_type          state;
    ps_extended_info_code_enum_type  info_code;
  } flow_event_info;

  struct
  {
    phys_link_state_type             state;
    ps_extended_info_code_enum_type  info_code;
  } phys_link_event_info;

  struct
  {
    boolean  is_modify_succeeded;
  } primary_qos_modify_result_info;

  struct
  {
    ps_phys_link_type                * pri_phys_link_ptr;
    ps_extended_info_code_enum_type    info_code;
  } pri_changed_info;

  struct
  {
    ps_iface_addr_family_type  old_addr_family;
    ps_iface_addr_family_type  new_addr_family;
  } addr_family_info;

  ps_iface_ioctl_bearer_tech_changed_type  bearer_tech_changed_info;
  ds_sys_system_status_info_type           bearer_tech_ex_changed_info;

  struct
  {
    int32                                  handle;
    ps_phys_link_dos_ack_status_info_type  status_info;
  } dos_ack_info;

  /*---------------------------------------------------------------------------
    Event info associated with IFACE_FLOW_ADDED_EV/IFACE_FLOW_DELETED_EV.
    Indicates the ps flow which got added/deleted
  ---------------------------------------------------------------------------*/
  ps_flow_type                          * flow_ptr;
  ps_iface_priv_ipv6_addr_info_type       priv_ipv6_addr;
  ps_iface_prefix_update_type             prefix_info;
  ps_ip_addr_type                         ip_addr;
  uint32                                  flow_mask;
  ps_iface_mt_handle_type                 mt_handle;
  ps_iface_mcast_event_info_type          mcast_info;
  ps_iface_state_enum_type                state;
  ps_extended_info_code_enum_type         qos_aware_info_code;
  ps_iface_rf_conditions_info_type        rf_conditions_change_info;

  ps_iface_outage_notification_event_info_type  outage_notification_info;
  ps_hdr_rev0_rate_inertia_failure_code_enum_type
                                    hdr_rev0_rate_inertia_failure_code;
  ps_iface_ioctl_extended_ip_config_type  extended_ip_config_succeeded;

  /**
  @brief Extended IP config EX event data : IFACE_EXTENDED_IP_CONFIG_EX_EV
         Mask value indicating what all config items have changed.
  */
  ps_iface_ioctl_extended_ip_config_ex_type  extended_ip_config_change_mask;

  /*---------------------------------------------------------------------------
    Event info associated with IFACE_OUT_OF_USE_EV/IFACE_DOWN_EV.
  ---------------------------------------------------------------------------*/
  ps_iface_down_event_info_type               iface_down_info;  

  /*---------------------------------------------------------------------------
    Event info associated with IFACE_UP_EV/IFACE_IN_USE_EV.
  ---------------------------------------------------------------------------*/
  ps_iface_up_event_info_type                 iface_up_info;

  struct
  {
    ps_hdr_slotted_mode_failure_code_enum_type  hdr_slotted_mode_failure_code;
    uint8                                       sm_current_sci;
  } slotted_mode_info;

  ps_iface_fast_dormancy_status_type fast_dorm_status;

  ps_iface_handoff_event_info_type      handoff_event_info;
  
  ps_iface_handoff_failure_event_info_type handoff_failure_event_info;
  
  ps_iface_net_initiated_qos_available_info_type
                                      iface_net_initiated_qos_available_info;

  uint16                      ra_lifetime;
  uint8                       n_filters;

  ps_iface_embms_tmgi_activation_info_type      embms_tmgi_act_ind_info;
  /**< Event info for event: IFACE_EMBMS_TMGI_ACTIVATED_EV */
  
  ps_iface_embms_tmgi_deactivation_info_type    embms_tmgi_deact_ind_info;
  /**< Event info for event: IFACE_EMBMS_TMGI_DEACTIVATED_EV */

    ps_iface_embms_tmgi_activation_deactivation_info_type      
                                            embms_tmgi_act_deact_ind_info;

  ps_iface_embms_tmgi_list_info_type            embms_tmgi_list_ind_info;
  /**< Event info for events: IFACE_EMBMS_ACTIVE_TMGI_LIST_EV,
                              IFACE_EMBMS_AVAILABLE_TMGI_LIST_EV &
                              IFACE_EMBMS_WARNING_TMGI_LIST_EV */

  ps_iface_embms_sai_info_type embms_sai_list_ind_info;
 /**< Event info for events: IFACE_EMBMS_AVAILABLE_SAI_LIST_EV*/

  ps_iface_ipfltr_change_type   fltr_change_info;  
  /**< Event info associated with IFACE_REVERSE_IP_TRANSPORT_IPFLTR_CHANGED_EV*/         

  ps_iface_embms_content_desc_update_type  embms_content_desc_info;     

  ps_iface_changed_apn_param_info_type  apn_param_change_info_type;
  /**< Event info associated with IFACE_APN_PARAM_CHANGED_EV*/

} ps_iface_event_info_u_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_STATS_TYPE
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
    IP packets received/transmitted
  -------------------------------------------------------------------------*/
  uint32 rx_pkts;
  uint32 tx_pkts;

  /*-------------------------------------------------------------------------
    physical link statistics - bytes RX/TX by physical layer (e.g. RLP/RLC)
  -------------------------------------------------------------------------*/
  struct
  {
    uint32 rx_bytes;
    uint32 tx_bytes;
  } phys_link;

} ps_iface_stats_type;

/*---------------------------------------------------------------------------
TYPEDEF IPV6_IID_ENUM_TYPE

DESCRIPTION
  Enum type to determine whether the interface ID for an IPv6 interface is
  a random or user supplied.
---------------------------------------------------------------------------*/
typedef enum
{
  IPV6_RANDOM_IID = 0,
  IPV6_USER_IID   = 1
} ps_iface_ipv6_iid_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_IP_VER_FAILOVER_E_TYPE

DESCRIPTION
  Mapping of the NV item for failover behavior.
---------------------------------------------------------------------------*/
typedef enum
{
  IPV4_ONLY                             = 0,
  IPV6_ONLY                             = 1,
  IPV6_PREFERRED                        = 2,
  IPV4_PREFERRED                        = 3,
  IPV6_DESIRED                          = 4,
  IPV4_DESIRED                          = 5
} ps_iface_ip_ver_failover_e_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_BEARER_IP_TYPE

DESCRIPTION
  This enum defines the supported bearer on a given ps_iface
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IFACE_BEARER_IP_V4_ONLY = 0,
  PS_IFACE_BEARER_IP_V6_ONLY,
  PS_IFACE_BEARER_IP_SINGLE_BEARER,
  PS_IFACE_BEARER_IP_DUAL_BEARER
} ps_iface_bearer_ip_type;

/*--------------------------------------------------------------------------
 PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK - if call_end_reason is set
 PS_CLIENT_DATA_INFO_LOCAL_ABORT_MASK  - if local_abort is set
 PS_CLIENT_DATA_INFO_INVALID_MASK   - mask is not set  
---------------------------------------------------------------------------*/
#define PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK        0x01
#define PS_CLIENT_DATA_INFO_LOCAL_ABORT_MASK            0x02
#define PS_CLIENT_DATA_INFO_INVALID_MASK                0X00   
                                                  
/*-------------------------------------------------------------------------
  This structure is used to pass as client_data_ptr type in
  ps_iface_tear_down_cmd()
-------------------------------------------------------------------------*/
typedef struct
{

  /*-------------------------------------------------------------------------
   Validity mask is used to define which of the variables (call_end_reason, 
   local_abort) in the following structure are set. 
   PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK - if call_end_reason is set
   PS_CLIENT_DATA_INFO_LOCAL_ABORT_MASK  - if local_abort is set
   PS_CLIENT_DATA_INFO_INVALID_MASK   - mask is not set   
  -------------------------------------------------------------------------*/
  uint32 validity_mask;

  struct
{
  /*-------------------------------------------------------------------------
      If the call should be local abort.
    -------------------------------------------------------------------------*/
    boolean                            local_abort;
    /*-------------------------------------------------------------------------
      Iface teardown reason. It is used to propagate ps net down reason.
  -------------------------------------------------------------------------*/
    ps_iface_net_down_reason_type      call_end_reason;

  }data_info;

} ps_iface_tear_down_client_data_info_type;

/*---------------------------------------------------------------------------
  Defines of the default values for IPV6 enabled and the Failover
  configuration NV items should they not be set.
---------------------------------------------------------------------------*/
#define IPV6_ENABLED_DEFAULT TRUE
#define IP_VER_FAILOVER_DEFAULT IPV6_DESIRED

/* Def from ds_flow_control.h
   - defined to avoid API level churn from ModemData
     (See DS_FLOW_IS_ENABLED)
 */
#define ALL_FLOWS_ENABLED     0x00000000  /* All flows enabled flag  */

/**
  @brief Structure that stores the IP config details of a given iface
*/
typedef struct
{
  boolean         is_ip_cfg_valid; 
  /**< Flag to indicate validity of this structure contents */
  ps_ip_addr_type ip_addr;
  /**< V4/V6 Ip Address */
  uint32          v4_gateway_addr;
  /**< V4 Gateway Address */
  uint64          v6_gateway_iid;
  /**< V6 gateway IID */
  uint32          prefix_valid_life_time;
  /**< V6 Prefix Valid Life time value */
  uint32          prefix_pref_life_time;
  /**< V6 Prefix Preferred Life time value */
  uint16          router_life_time;
  /**< V6 Router Life time value */
  ip_addr_type    dns_addrs[PS_IFACE_NUM_DNS_ADDRS];
  /**< V4/V6 DNS Addresses */
  ip_addr_type    sip_serv_addr_array[PS_IFACE_MAX_SIP_SERVER_ADDRESSES];   
  /**< V4/V6 Sip Server Addresses */
  uint32          valid_sip_serv_addr_count;
  /**< Valid Sip Server addresses in the sip_serv_addr_array */
} ps_iface_ip_config_type;

/**
  @brief Structure that stores the IP config details for a PDN
*/

typedef struct
{
  ps_iface_ip_config_type iface_ipv4_config;
  /**< Container to store PDN's IPV4 related info */
  ps_iface_ip_config_type iface_ipv6_config;
  /**< Container to store PDN's IPV6 related info */
} ps_iface_pdn_ip_config_type;

#endif /* PS_IFACE_DEFS_H */
