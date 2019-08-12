#ifndef __PS_CLAT_SM_H_
#define __PS_CLAT_SM_H_
/*===========================================================================

                           P S _ C L A T _ S M . H
                   
DESCRIPTION
  The header file for CLAT State Machine.

EXTERNALIZED FUNCTIONS

Copyright (c) 2014-2015 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_clat_sm.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

--------    ---    ----------------------------------------------------------
12/16/15    kc     Add changes to event handling in XLAT.
09/29/15    ds     Added net down reason when clat is down
02/04/15    ssh    Introduced failover v6 iface in passthrough mode bringup
02/03/15    skc    Added changes for CLAT QTF test 
10/10/14    ssh    IP Translation Engine 464XLAT   
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)
#ifdef FEATURE_DATA_PS_464XLAT
#include "ps_iface.h"
#include "ps_acl.h"
#include "ps_in.h"
#include "dssdns.h"
#include "ps_dns_proxy_ext.h"
#include "ps_clat_iface_hdlr.h"

/*===========================================================================

                                CONSTANTS

===========================================================================*/
/*---------------------------------------------------------------------------
  Maximum length for the Provider Translator (CLAT peer) FQDN 
---------------------------------------------------------------------------*/
#define FQDN_MAX_LEN 128

/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF PS_CLAT_SM_STATE_TYPE

DESCRIPTION
  Enum of CLAT SM states
---------------------------------------------------------------------------*/
#define CLAT_SM_MAX_STATE 6 
typedef enum
{
  CLAT_SM_DOWN        = 0,
  CLAT_SM_STARTING    = 1,
  CLAT_SM_PASSTHROUGH = 2,
  CLAT_SM_CONFIGURING = 3,
  CLAT_SM_TRANSLATE   = 4,
  CLAT_SM_GOING_DOWN  = 5
} ps_clat_sm_state_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_CLAT_SM_EVENT_TYPE

DESCRIPTION
  Enum of SM events
---------------------------------------------------------------------------*/
#define CLAT_SM_MAX_EV 12
typedef enum
{
  CLAT_SM_START_EV                     = 0,
  CLAT_SM_STOP_EV                      = 1,
  CLAT_SM_IPV4_IFACE_UP_EV             = 2,
  CLAT_SM_IPV4_IFACE_DOWN_EV           = 3,
  CLAT_SM_IPV6_IFACE_UP_EV             = 4,
  CLAT_SM_IPV6_IFACE_DOWN_EV           = 5,
  CLAT_SM_IPV6_IFACE_PREFIX_UPDATE_EV  = 6,
  CLAT_SM_PLAT_AVAILABLE_EV            = 7,
  CLAT_SM_PLAT_UNAVAILABLE_EV          = 8,
  CLAT_SM_DNS_PROXY_ENABLED_EV         = 9,
  CLAT_SM_DNS_PROXY_DISABLED_EV        = 10,
  CLAT_SM_DPM_CONTEXT_REGISTERED_EV    = 11
} ps_clat_sm_event_type;

/*---------------------------------------------------------------------------
TYPEDEF PLAT_PROVISIONING_MODE_ENUM_TYPE

DESCRIPTION
  Enum of PLAT provisioning mode
---------------------------------------------------------------------------*/
typedef enum
{
  LOCALLY_PROVISIONED = 0,
  DISCOVER_VIA_DNS64  = 1
} plat_provisioning_mode_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF ICMP_TX_ERR_MODE_ENUM_TYPE

DESCRIPTION
  Enum of CLAT ICMP error mode
---------------------------------------------------------------------------*/
typedef enum
{
  DISABLED      = 0,
  RATE_LIMITED  = 1,
  ENABLED       = 2
} icmp_tx_err_mode_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_CLAT_SM_DNS_CB_INFO_TYPE

DESCRIPTION
  Structure to hold DNS callback information
---------------------------------------------------------------------------*/
typedef struct ps_clat_sm_dns_cb_info_type_s
{
  dss_dns_session_mgr_handle_type session_handle;
  dss_dns_query_handle_type       query_handle;
  dss_dns_api_type_enum_type      api_type;
  uint16                          num_records;
} ps_clat_sm_dns_cb_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_CLAT_SM_DNS_INFO_TYPE

DESCRIPTION
  Structure to hold DNS information
---------------------------------------------------------------------------*/
typedef struct ps_clat_sm_dns_info_type_s
{
  dss_dns_session_mgr_handle_type dss_dns_session_handle;
  dss_dns_query_handle_type       dss_dns_query_handle;
  ps_clat_sm_dns_cb_info_type     dns_cb_info;
} ps_clat_sm_dns_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_CLAT_SM_CB_TYPE

DESCRIPTION
  Control block for CLAT state machine
---------------------------------------------------------------------------*/
struct ps_clat_sm_cb_s;
typedef struct ps_clat_sm_cb_s ps_clat_sm_type;
struct ps_clat_sm_cb_s 
{
  ps_clat_sm_state_type     state;               /* Current SM state       */
  acl_type                  rt_acl;              /* Routing ACL            */
  acl_type                  inbound_acl;         /* Inbound ACL            */
  ps_iface_type             logical_iface;       /* CLAT logical iface     */
  ps_clat_iface_addr_type   v4_addrs;            /* V4 addrs for this PDN  */
  plat_provisioning_mode_enum_type plat_mode;    /* PLAT provisioning mode */
  struct ps_in6_addr        plat_addr;           /* PLAT address           */
  ipv6_prefix_len_enum_type plat_addr_prefix_len;/* PLAT addr prefix len   */
  char                      plat_fqdn[FQDN_MAX_LEN]; /* PLAT FQDN          */
  struct ps_in6_addr        own_ip6_addr;        /* Own IPv6 address       */
  acl_policy_info_type      req_policy_info;     /* Requested policy info  */
  struct
  {
    ps_iface_type * iface_ptr;                   /* assoc. iface (epc|phy) */
    void          * iface_up_cbuf;               /* UP ev buf              */
    void          * iface_down_cbuf;             /* DOWN ev buf            */
    void          * iface_prefix_update_cbuf;    /* pfx update ev buf      */
    void          * iface_routeable_cbuf;       /* ROUTEABLE ev buf        */
  } assoc_iface;
  
  struct
  {
    ps_iface_type * iface_ptr;                   /* assoc. iface (epc|phy) */
    void          * iface_up_cbuf;               /* UP ev buf              */
    void          * iface_down_cbuf;             /* DOWN ev buf            */
    void          * iface_routeable_cbuf;       /* ROUTEABLE ev buf        */
  } failover_v6_iface;
  
  ps_clat_sm_dns_info_type dns_info;             /* PLAT DNS info          */
  dns_proxy_handle_type    dns_proxy_handle;     /* DNS Proxy handle       */
  boolean                  is_dpm_ctxt_set;      /* DPM context set?       */
  ps_iface_net_down_reason_type net_down_reason; /* Iface down reason      */ 
  ps_clat_sm_type        * this_sm_ptr;          /* self pointer           */
};

/*---------------------------------------------------------------------------
TYPEDEF PLAT_CONFIG_INFO_TYPE

DESCRIPTION
  This structure stores the PLAT config information.
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  plat_provisioning_mode_enum_type mode;
  char                             prefix[PS_IN6_ADDRSTRLEN];
  ipv6_prefix_len_enum_type        prefix_len;
  char                             fqdn[FQDN_MAX_LEN];
} plat_config_info_type;

/*---------------------------------------------------------------------------
TYPEDEF CLAT_CONFIG_INFO_TYPE

DESCRIPTION
  This structure stores the CLAT config information.
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint8                       version;
  boolean                     enabled;                     /* CLAT ON/OFF? */
  plat_config_info_type       plat_config_info;            /* PLAT info    */
  boolean                     ul_ignore_v4_tos;            /* UL v4 TOS    */ 
  uint8                       dl_v4_tos_val;               /* DL v4 TOS    */
  boolean                     ul_no_df_insert_v6_frag_hdr; /* frag hdr ?   */
  boolean                     ul_udp_process_zero_cksum;   /* udp 0 cksum? */
  icmp_tx_err_mode_enum_type  icmpv4_err_mode;             /* v4 err tx?   */
  icmp_tx_err_mode_enum_type  icmpv6_err_mode;             /* v6 err tx?   */          
} clat_config_info_type;

/*===========================================================================

                            MACROS

===========================================================================*/
#define CLAT_INST_INITIALIZED(inst_ptr)                                    \
  ((inst_ptr != NULL) && (inst_ptr->this_sm_ptr == inst_ptr))

/*===========================================================================

                            EXTERNAL FUNCTIONS

===========================================================================*/
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
FUNCTION  PS_CLAT_UL_IGNORE_V4_TOS()

DESCRIPTION
  Indicates if IPv4 TOS bit should be ignored on UL and IPv6 traffic class
  should be set to 0 in the IPv6 translated packet.

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_clat_ul_ignore_v4_tos
(
  void
);

/*===========================================================================
FUNCTION  PS_CLAT_DL_V4_TOS_VAL()

DESCRIPTION
  Indicates what value to use for the IPv4 TOS bit on DL IPv4 translated 
  packets. 0 indicates that traffic class value from the incoming IPv6 
  packet should be used as-is.

DEPENDENCIES
  None

RETURN VALUE
  0: Set IPv4 TOS to the traffic class value from IPv6 packet
  other values: Set IPv4 TOS to return value

SIDE EFFECTS
  None
===========================================================================*/
uint8 ps_clat_dl_v4_tos_val
(
  void
);

/*===========================================================================
FUNCTION  PS_CLAT_UL_NO_DF_INSERT_V6_FRAG_HDR()

DESCRIPTION
  Indicates if an IPv6 fragment header should be inserted in the IPv6 
  translated packet if the incoming IPv4 packet has DF bit unset.

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_clat_ul_no_df_insert_v6_frag_hdr
(
  void
);

/*===========================================================================
FUNCTION  PS_CLAT_UL_UDP_PROCESS_ZERO_CKSUM()

DESCRIPTION
  Indicates if UL IPv4 UDP packets with zero checksum should be processed
  (translated to IPv6 and checksum updated) or dropped aftr logging an 
  error.

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_clat_ul_udp_process_zero_cksum
(
  void
);

/*===========================================================================
FUNCTION  PS_CLAT_ICMPV4_ERR_MODE()

DESCRIPTION
  Returns the error mode type configured for ICMPv4 error packet generation

DEPENDENCIES
  None

RETURN VALUE
  icmp_tx_err_mode_enum_type

SIDE EFFECTS
  None
===========================================================================*/
icmp_tx_err_mode_enum_type ps_clat_icmpv4_err_mode
(
  void
);

/*===========================================================================
FUNCTION  PS_CLAT_ICMPV6_ERR_MODE()

DESCRIPTION
  Returns the error mode type configured for ICMPv6 error packet generation

DEPENDENCIES
  None

RETURN VALUE
  icmp_tx_err_mode_enum_type

SIDE EFFECTS
  None
===========================================================================*/
icmp_tx_err_mode_enum_type ps_clat_icmpv6_err_mode
(
  void
);

/*===========================================================================
FUNCTION  PS_CLAT_POWERUP()

DESCRIPTION
  Performs CLAT powerup initialization

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_clat_powerup
(
  void
);

/*===========================================================================
FUNCTION  PS_CLAT_POWERDOWN

DESCRIPTION
  Frees the memory allocated by CLAT via ps_clat_powerup()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_clat_powerdown
(
  void
);

/*===========================================================================
FUNCTION  PS_CLAT_SM_POST_EVENT

DESCRIPTION
  This functions posts events to the CLAT state machine (which runs in DCC
  task)

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_clat_sm_post_event
(
  ps_clat_sm_type       *instance,
  ps_clat_sm_event_type event,
  void                  *event_data
);

/*===========================================================================
FUNCTION  PS_CLAT_SM_HANDLE_ASSOC_IFACE_EV_CBACK

DESCRIPTION
  This function handles assoc iface event

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_clat_sm_handle_assoc_iface_ev_cback
(
  ps_iface_type              * iface_ptr,
  ps_iface_event_enum_type     event,
  ps_iface_event_info_u_type   event_info,
  void                       * user_data
);
#endif /* FEATURE_DATA_PS_464XLAT */
#endif /* FEATURE_DATA_PS && FEATURE_DATA_PS_IPV6*/
#endif /* __PS_CLAT_SM_H_ */
