#ifndef DHCP6_CLIENTI_H
#define DHCP6_CLIENTI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      D H C P 6 _ C L I E N T I . H

GENERAL DESCRIPTION
  THIS IS A DHCPv6 CLIENT INTERNAL HEADER FILE.  NO SYSTEM OTHER THAN
  DHCP CLIENT SHOULD INCLUDE THIS!  See dhcp_client.h instead.

  This file describes the API's between each of the modules that make
  up the DHCP client.  These modules include the manager, the core, the
  client shim, configuration shim.


Copyright (c) 2006-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/dhcp6_clienti.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/27/15    ds     Fix to handle expansion of DNS names accordingly as both v4 
                   v6 have different encoding techniques.
07/04/14    ss     Added support for generating 
                   IFACE_EXTENDED_IP_CONFIG_EX_EV
04/09/14    ash    Added Prefix Delegation support.
09/12/13    sd     Cryptographic vulnerability - replacing ran_next APIs.
09/26/08    am     Changed malloc/free names for off target support.
08/25/08    cp     Updated changes in draft-ietf-mip6-hiopt-17.txt from
                   draft-ietf-mip6-hiopt-09.txt.
08/25/08    am     Added DHCP ongoing determination.
05/02/07    es     Made changes to fix layering problem by removing reference
                   to dhcp6_client_params struct.
04/15/07    es     Added configuration event ind function. Removed changes
                   to support previous IOCTL.
02/01/07    hm     Added IOCTL to get fresh DHCP configuration
01/23/07    es     Added MIPv6 support.
06/29/06    es     Initial development work.
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h" 
#include <stringl/stringl.h>
#ifdef FEATURE_DATA_PS_IPV6

#include "dhcp_client_common.h"
#include "dhcp_client_timer.h"
#include "ps_in.h"
#include "dssocket_defs.h"
#include "dhcp6_client.h"
#include "ps_iface_defs.h"

/*==========================================================================
                                  DEFINES
==========================================================================*/

/* RFC 1035 Section 3.1 */
#define DHCP6_CLIENT_MAX_DOMAIN_NAME_BUF_LEN     (256)

/*==========================================================================
                                  STRUCTS & ENUMS
==========================================================================*/

/* Status codes for status code option */
typedef enum {
  DHCP6_CLIENT_SUCCESS_STATUS       = 0,
  DHCP6_CLIENT_UNSPECFAIL_STATUS    = 1,
  DHCP6_CLIENT_NOADDRAVAIL_STATUS   = 2,
  DHCP6_CLIENT_NOBINDING_STATUS     = 3,
  DHCP6_CLIENT_NOTONLINK_STATUS     = 4,
  DHCP6_CLIENT_USEMULTICAST_STATUS  = 5,
  DHCP6_CLIENT_NOPREFIXAVAIL_STATUS = 6,
} dhcp6_client_status_code;

/* List structure for Domain Names */
typedef struct dhcp6_client_domain_name {
  /* null-terminated domain_name */
  char    domain_name[DHCP6_CLIENT_MAX_DOMAIN_NAME_BUF_LEN];
  uint32  domain_name_len;
  struct  dhcp6_client_domain_name * next;
} dhcp6_client_domain_name;

/* List structure for IPv6 addresses */
typedef struct dhcp6_client_addr {
  struct ps_in6_addr         addr;
  struct dhcp6_client_addr * next;
} dhcp6_client_addr;

#ifdef FEATURE_DATA_PS_MIPV6
/* MIPv6 structure holding MIPv6 info, fields zeroed if invalid */
typedef struct dhcp6_client_mip6_bootstrap_config_info_type {
  struct ps_in6_addr mip6_home_agent_addr;
  struct ps_in6_addr mip6_home_addr;
  struct ps_in6_addr mip6_home_link_prefix;
  struct ps_in_addr  mip6_home_agent_ipv4_addr;
  uint32             hl_prefix_len;
  boolean            visited_nw_info;
} dhcp6_client_mip6_bootstrap_config_info;
#endif /* FEATURE_DATA_PS_MIPV6 */

/* The provision structure contains the information needed by the
 * configuration layer to provision the interface. */
typedef struct {
  /* SIP server IPV6 addresses */
  dhcp6_client_addr                        * sip_addr;
  /* SIP server domain names */
  dhcp6_client_domain_name                 * sip_domain_name;
  /* DNS server IPV6 addresses */
  dhcp6_client_addr                        * dns_addr;
  /* DNS domain search list */
  dhcp6_client_domain_name                 * dns_domain_search_list;
#ifdef FEATURE_DATA_PS_MIPV6
  /* MIPv6 bootstrap configuration info */
  dhcp6_client_mip6_bootstrap_config_info  * mip6_bootstrap_info;
#endif /* FEATURE_DATA_PS_MIPV6 */
#ifdef FEATURE_DATA_PS_DHCPV6
  /* Prefix Delegation configuration info */
  struct ps_in6_addr                         delegated_prefix;
  uint8                                      delegated_prefix_len;
  boolean                                    exclude_prefix;
  uint64                                     excluded_prefix;
  uint8                                      excluded_prefix_len;
#endif /* FEATURE_DATA_PS_DHCPV6 */
} dhcp6_client_provision;

/* This structure contains server address, server DUID and flag to indicate 
 * if the server unicast option is seen or not */
typedef struct {
  /* IPv6 address of Server */
  struct ps_in6_addr server_address;
  /* Server DUID */
  uint8  server_duid[DHCP6_CLIENT_MAX_DUID_LENGTH];
  /* Server DUID option seen */
  /* Server DUID length */
  uint16 server_duid_len;
  boolean server_id_option_seen;
  /* Unicast option seen flag */
  boolean  send_unicast;
} dhcp6_client_server;

#ifdef FEATURE_DATA_PS_DHCPV6
/* This structure contains IA_PD options of IA_PD which contains
 * Prefix, Prefix length, Preferred lifetime, Valid lifetime,
 * and status of prefix delegation. */
typedef struct {
  /* IPv6 Delegated prefix */
  struct ps_in6_addr  prefix;
  /* Prefix length of delegated prefix */
  uint8   prefix_length;
  /* Preferred lifetime of delegated prefix */
  uint32  preferred_lifetime;
  /* Valid lifetime of delegated prefix */
  uint32  valid_lifetime;
  /* Excluded prefix from delegated prefix */
  uint64  excluded_prefix;         
  uint8   excluded_prefix_len;  

  /* IA_PREFIX Status code */
  dhcp6_client_status_code                   status;
} dhcp6_client_ia_prefix_option;


/* This structure contains Prefix Delegation information needed by the
 * DHCPv6 State machine & configuration layer to provision the 
 * Delegated prefix on the interface. */
typedef struct {
  /* IAID of IA_PD */
  uint32                                     iaid;
  /* T1 time of IA_PD */
  uint32                                     T1;
  /* T2 time of IA_PD */
  uint32                                     T2;
  /* IA_PD options of IA_PD */
  dhcp6_client_ia_prefix_option              ia_prefix;
  /* IA_PD Status code */
  dhcp6_client_status_code                   status;
} dhcp6_client_ia_pd;
#endif /* FEATURE_DATA_PS_DHCPV6 */

/* This function type is passed into the callback for domain_name_expand
 * in the core_init struct to extract bytes from the relevant option data */
typedef int (*dhcp6_client_get_bytes_type)
(
  void   * src,              /* Buffer to get bytes from */
  uint32   offset,           /* Offset in source buffer to get bytes from */
  char   * dst_buf,          /* Destination buffer */
  uint16    dst_buf_size,     /* Destination buffer size*/  
  uint32   len               /* Number of bytes to get */
);

/* This function type is the callback to expand domain_names, passed to
 * the core module via the core_init struct */
typedef int (*dhcp6_client_domain_name_expand_cb_type)
(
  void                        * src_buf,
  uint32                        offset,
  char                        * dst_buf,
  uint32                        buf_len,
  uint32                      * name_len,
  dhcp6_client_get_bytes_type   get_f,
  boolean                     ip_ver_flag 
);

/*=========================================================================
INIT STRUCTS

  The init structures contain information provided to the modules (core,
client, config) upon initialization.
=========================================================================*/

/* Core parameters controlling DHCPv6 client core behavior */
typedef struct {
  /* client DUID */
  uint8 * client_duid;
  uint16  client_duid_len;

  /* The following two params determine if the client includes these
   * options in sending a Information-Request message. */

  /* If set, the client ID option is included in all messages sent
   * from the client to the server (ie. the Inform message for the
   * stateless client). Section 18.1.5 of RFC 3315 states that
   * "The client SHOULD include a Client Identifier option
   * to identify itself to the server. If the client does not include
   * a Client Identifier option, the server will not be able to return
   * any client-specific options to the client, or the server may
   * choose not to respond to the message at all." */
  boolean client_id_option;

  /* If set, the reconfigure accept option is included in all allowed
   * messages from the client to the server (see Section A RFC 3315).
   * Section 22.20, RFC 3315 states:
   * "A client uses the Reconfigure Accept option to announce to the
   * server whether the client is willing to accept Reconfigure
   * messages. The default behavior, in the absence of this option,
   * means unwillingness to accept Reconfigure messages. */
  boolean reconfigure_accept_option;

#ifdef FEATURE_DATA_PS_DHCPV6
  /* If set, the reconfigure accept option is included in all allowed
   * messages from the client to the server (see Section A RFC 3315).
   * Section 22.20, RFC 3315 states:
   * "A client uses the Reconfigure Accept option to announce to the
   * server whether the client is willing to accept Reconfigure
   * messages. The default behavior, in the absence of this option,
   * means unwillingness to accept Reconfigure messages. */
  dhcp6_client_mode_type mode;
#endif /* FEATURE_DATA_PS_DHCPV6 */

  /* The following five params determine if the client request these
   * options in the Option Request Option.
   * Suggested value for these options is true. 
   * Below option flags are for stateless configuration */
  boolean sip_domain_name_list_option;  /* detailed in RFC 3319 */
  boolean sip_ipv6_list_option;         /* detailed in RFC 3319 */
  boolean dns_servers_option;           /* detailed in RFC 3646 */
  boolean dns_domain_searchlist_option; /* detailed in RFC 3646 */
  boolean info_refresh_time_option;     /* detailed in RFC 4242 */

#ifdef FEATURE_DATA_PS_DHCPV6
  /* Below option flags are for statelful configuration */
  boolean prefix_delegation_option;      /* detailed in RFC 3769, 3633*/
  boolean rapid_commit;                /* detailed in RFC 3315*/
  boolean prefix_exclude;               /* detailed in RFC 6603*/
#endif /* FEATURE_DATA_PS_DHCPV6 */

#ifdef FEATURE_DATA_PS_MIPV6
  /* User should set this to TRUE if it is using mechanism defined in
   * 3GPP2 X.S0011-002-D Section 5.5.2 for obtaining MIPv6 bootstrap info,
   * and is expecting bootstrap info, otherwise bootstrap config info may
   * not be set in ps_iface */
  boolean expect_vendor_opts_bootstrap_info;

  /* detailed in draft-ietf-mip6-bootstrapping-integrated-06.txt
   * Option code is placed in Option Request Option if set to true.
   * default value is FALSE */
  boolean mip6_home_network_id_option;

  /* home network info option values to send to DHCPv6 server,
   * zeroed out by default */
  dhcp6_mip6_hnid_option_vals mip6_home_network_id_vals;
#endif /* FEATURE_DATA_PS_MIPV6 */

  /* This is not specified by RFC - this is the number of times we
   * try to send an information request message before we tell the upper
   * layer that we have failed to get provisioning.
   * Suggest 4. */
  uint32 informationrequest_tries;

  /* The following information-request retransmission suggested values
   * are specified in RFC 3315, Section 5.5, and 14. */

  /* Max desyncronization delay
   * in seconds, RFC suggested value is 1 sec */
  uint32 informationrequest_max_delay;

  /* initial retransmit time
   * in seconds, RFC suggested value is 1 sec */
  uint32 informationrequest_irt;

  /* max retransmit time
   * in seconds, RFC suggested value is 120 sec */
  uint32 informationrequest_max_rt;

#ifdef FEATURE_DATA_PS_DHCPV6
  /* SOLICIT max retry count,
     RFC suggested value is 0 (infinite).
   */
  uint32 solicit_mrc;

  /* SOLICIT Max transmission timeout value
   * in seconds, RFC suggested value is 120 sec */
  uint32 solicit_mrt;

  /* SOLICIT initial retry timeout,
   * RFC suggested value is 1 sec */
  uint32 solicit_irt;

  /* SOLICIT max retransmit duration
   * in seconds, RFC suggested value is 0 (infinite)*/
  uint32 solicit_mrd;

  /* REQUEST max retry count,
     RFC suggested value is 0 (infinite).
   */
  uint32 request_mrc;

  /* REQUEST Max transmission timeout value
   * in seconds, RFC suggested value is 120 sec */
  uint32 request_mrt;

  /* REQUEST initial retry timeout,
   * RFC suggested value is 1 sec */
  uint32 request_irt;

  /* REQUEST max retransmit duration
   * in seconds, RFC suggested value is 0 (infinite)*/
  uint32 request_mrd;

  /* RENEW max retry count,
     RFC suggested value is 0 (infinite).
   */
  uint32 renew_mrc;

  /* RENEW Max transmission timeout value
   * in seconds, RFC suggested value is 120 sec */
  uint32 renew_mrt;

  /* RENEW initial retry timeout,
   * RFC suggested value is 1 sec */
  uint32 renew_irt;

  /* RENEW max retransmit duration
   * in seconds, RFC suggested value is 0 (infinite)*/
  uint32 renew_mrd;

  /* REBIND max retry count,
     RFC suggested value is 0 (infinite).
   */
  uint32 rebind_mrc;

  /* REBIND Max transmission timeout value
   * in seconds, RFC suggested value is 120 sec */
  uint32 rebind_mrt;

  /* REBIND initial retry timeout,
   * RFC suggested value is 1 sec */
  uint32 rebind_irt;

  /* REBIND max retransmit duration
   * in seconds, RFC suggested value is 0 (infinite)*/
  uint32 rebind_mrd;

  /* RELEASE max retry count,
     RFC suggested value is 0 (infinite).
   */
  uint32 release_mrc;

  /* RELEASE Max transmission timeout value
   * in seconds, RFC suggested value is 120 sec */
  uint32 release_mrt;

  /* RELEASE initial retry timeout,
   * RFC suggested value is 1 sec */
  uint32 release_irt;

  /* RELEASE max retransmit duration
   * in seconds, RFC suggested value is 0 (infinite)*/
  uint32 release_mrd;
#endif /* FEATURE_DATA_PS_DHCPV6 */

  /* The following information refresh time suggested values
   * are specified in RFC 4242, Section 3.1 */

  /* default refresh time
   * in seconds, RFC suggested value is 86400 */
  uint32 info_refresh_time_default;

  /* minimum refresh time
   * in seconds, RFC suggested value is 600 */
  uint32 info_refresh_time_minimum;

  /* DHCPv6 server address to send DHCP messages to when
   * send_multicast flag is TRUE, default value is
   * ALL_DHCP_RELAY_AGENTS_AND_SERVERS_ADDR multicast address */
  struct ps_in6_addr server_addr;

} dhcp6_client_core_params;

/* Init struct for the core module */
typedef struct dhcp6_client_core_init {
  void * (*d_malloc)(uint32 size);
  void (*d_free)(void *);
  void (*rand)(void *rand_num, uint16 rand_len); /* for xid generation */
  dhcp6_client_core_params core_params;
  dhcp_client_status_cb_type status_cb;
  void * userdata;      /* acts as a cookie for the status callback */
  dhcp6_client_domain_name_expand_cb_type domain_name_expand_cb;
} dhcp6_client_core_init;

/* Init struct for the client module */
typedef struct dhcp6_client_client_init {
  dss_iface_id_type iface;
  void * (*d_malloc)(uint32 size);
  void (*d_free)(void *);
} dhcp6_client_client_init;

/* Config parameters, e.g. param flags for what to set in ps_iface */
typedef struct
{
  boolean sip_domain_name_list_option;  /* detailed in RFC 3319 */
  boolean sip_ipv6_list_option;         /* detailed in RFC 3319 */
  boolean dns_servers_option;           /* detailed in RFC 3646 */
  boolean dns_domain_searchlist_option; /* detailed in RFC 3646 */
#ifdef FEATURE_DATA_PS_DHCPV6
  boolean prefix_delegation_option;     /* detailed in RFC 3633 */
#endif /* FEATURE_DATA_PS_DHCPV6 */
#ifdef FEATURE_DATA_PS_MIPV6
  /* User sets this to TRUE if it is using mechanism defined in
   * 3GPP2 X.S0011-002-D Section 5.5.2 for obtaining MIPv6 bootstrap info,
   * so the config gets set in ps_iface */
  boolean expect_vendor_opts_bootstrap_info;
  /* detailed in draft-ietf-mip6-bootstrapping-integrated-06.txt */
  boolean mip6_home_network_id_option;
#endif /* FEATURE_DATA_PS_MIPV6 */

} dhcp6_client_configuration_params;

/* Init struct for the config module */
typedef struct dhcp6_client_configuration_init {
  dss_iface_id_type iface;
  dhcp6_client_configuration_params config_params; /* for option request list */
  void * (*d_malloc)(uint32 size);
  void (*d_free)(void *);
  dhcp_client_status_cb_type status_cb;
  void * userdata;      /* acts as a cookie for the status callback */
} dhcp6_client_configuration_init;

/*=========================================================================
CONFIG STRUCTS

  The config structures contain information to the modules (core, client,
config) on how to connect to the other DHCPv6 client layers. The modules
are provided this information by their start function calls.
=========================================================================*/

/* Config structure for the core module */
typedef struct {
  /* Client Layer */
  boolean (*client_request_new)(void * handle, void ** msg, uint32 size);
  boolean (*client_request_append)(void * handle, void ** msg,
                                          void * data, uint32 size);
  void (*client_request_send)(void * handle, void ** msg,
         struct ps_in6_addr ip_addr );
  void (*client_request_free)(void * handle, void ** msg);
  void * client_request_handle;

  /* Configuration Layer */
  void (*configuration_test)(void * handle,
                             dhcp6_client_provision * provision);
  ps_iface_ioctl_extended_ip_config_ex_type (*configuration_set)
    ( void * handle,
      dhcp6_client_provision * provision );
  void (*configuration_clear)(void * handle);
  void (*configuration_event_ind)
           ( void * handle,
             ps_iface_ioctl_extended_ip_config_ex_type config_change_mask);
  void * configuration_handle;

  /* Timer services */
  void (*timer_set)( void * handle, uint32 delta,
                       dhcp_client_timer_mode mode);
  void (*timer_clear)( void * handle );
  void * timer_handle;
} dhcp6_client_core_config;

/* Config structure for the client module */
typedef struct {
  /* Send messages to the core, see DHCP6_CLIENT_CORE_REPLY function
   * header for details */
  void (*reply)
  (
    void * handle,
    void * msg,
    boolean (*extract)( void * dest, void * msg, uint32 offset, uint32 len)
  );
  void * reply_handle;
} dhcp6_client_client_config;

/* Config structure for the configuration module */
typedef struct {
  /* config struct unused now. build throws an error with empty struct */
  dhcp6_client_mode_type client_mode;
} dhcp6_client_configuration_config;

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
CORE LAYER

  The core layer takes care of sending messages, maintaining the DHCP
  client state, and dispatching responses.

  The following are the data structures that the API will use.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CORE_NEW

Description:
  This function initializes the core with internal data and returns a handle
  to a core object.

Arguments:
  dhcp6_client_core_init * init - the information (see structure above)
    needed to initialize the core.

Return value:
  void * - A handle to the new core object, or NULL on failure.

Dependencies:
  This function must be called to get a handle before any other core
  functions are called.  The handle must be started before the core is fully
  functional. (see dhcp6_clienti_core_start() below)
===========================================================================*/
void *
dhcp6_clienti_core_new
(
  dhcp6_client_core_init * init
);

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CORE_START

Description:
  This function configures the core with the information on how to interact
  with the other dhcp client modules.

Arguments:
  void * handle - Handle to the core object to start.
  dhcp6_client_core_config * config - The configuartion information needed to
    interface with other modules in the DHCP client architecture.

Return value:
  Boolean - TRUE if successful, otherwise FALSE.

Dependencies:
  The handle must have been allocated by dhcp6_clienti_core_new() above.
===========================================================================*/
boolean
dhcp6_clienti_core_start
(
  void * handle,
  dhcp6_client_core_config * config
);

#ifdef FEATURE_DATA_PS_DHCPV6
/*===========================================================================
FUNCTION DHCP6_CLIENTI_CORE_STOP

Description:
  This function stops a DHCP client core, including timers and frees the
  associated memory.  Additionally, the handle pointer will be NULL'd.
  Note: This function does not release a lease.

Arguments:
  void ** handle - The handle to the core to release.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp6_clienti_core_new().
===========================================================================*/
void
dhcp6_clienti_core_stop
(
  void ** handle
);

/*=========================================================================
FUNCTION DHCP6_CLIENTI_CORE_ABORT

DESCRIPTION
  This function Abort a DHCP client core, including timers and frees the
  associated memory.  Additionally, the handle pointer will be NULL'd.
  Note: This function does not release a lease.  See
  dhcp6_clienti_core_release below.

DEPENDENCIES
  The handle must have been allocated by dhcp6_clienti_core_new().

PARAMETERS
  void ** handle - The handle to the core to release.

RETURN VALUE
  None.

SIDE EFFECTS
  The handle pointer will be NULLed out.
=========================================================================*/
void
dhcp6_clienti_core_abort
(
  void ** handle
);
#endif /* FEATURE_DATA_PS_DHCPV6 */

/*=========================================================================
FUNCTION DHCP6_CLIENTI_CORE_FREE

DESCRIPTION
  This function frees memory associated with DHCP client core. 
  Additionally, the handle pointer will be NULL'd.

DEPENDENCIES
  The handle must have been allocated by dhcp6_clienti_core_new().

PARAMETERS
  void ** handle - The handle to the core to release.

RETURN VALUE
  None.

SIDE EFFECTS
  The handle pointer will be NULLed out.
=========================================================================*/
void
dhcp6_clienti_core_free
(
  void ** handle
);

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CORE_REPLY

Description:
  This function is used to send messages from servers to the core for
  processing.  Parsing of the message and event processing will happen before
  this function returns.  The memory associated with the incoming data may be
  freed on return of this function.

Arguments:
  void * handle - handle to DHCP Client core.
  void * msg - a handle to the incoming messsage.
  boolean (*extract) - The extract function to extract data from the message
    handle. Arguments as follows:
      - dest - The buffer into which the data is put.
      - src - The message pointer.
      - offset - The offset into the message to start extracting.
      - len - the length of the message to extract.
    Return value will be whether the function succeeds in extracting correct
    amount of data.  If FALSE, the dest will be assumed to not have any new
    data in it.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp6_clienti_core_new() and started
  by dhcp6_clienti_core_start().
===========================================================================*/
void
dhcp6_clienti_core_reply
(
  void * handle,
  void * msg,
  boolean (*extract)( void * dest, void * src, uint32 offset, uint32 len)
);


/*===========================================================================
FUNCTION DHCP6_CLIENTI_CORE_TIMER_EXPIRE

Description:
  This function informs the core that the timer has expired.

Arguments:
  void * handle - handle to DHCP Client core.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp6_clienti_core_new() and started
  by dhcp6_clienti_core_start().
===========================================================================*/
void
dhcp6_clienti_core_timer_expire
(
  void * handle
);

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CORE_INFORM

Description:
  This function sends an INFORM event to the core.  If the core is in
  an acceptable state, the INFORM event will cause an information-request
  message which will configure the stacks without obtaining an IP lease.

Arguments:
  void * handle - handle to DHCP Client core.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp6_clienti_core_new() and started
  by dhcp6_clienti_core_start().
===========================================================================*/
void
dhcp6_clienti_core_inform
(
  void * handle
);

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
CLIENT LAYER

  The client layer interfaces with the DSS socket layer for sending
  and receiving messages.

  The following are the data structures that the API will use.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CLIENT_NEW

Description:
  This function initializes the client layer with internal data and returns a
  handle to a core object.

Arguments:
  dhcp6_client_client_init * init - the information (see structure above)
    needed to initialize the client layer.

Return value:
  void * - A handle to the new client object, or NULL on failure.

Dependencies:
  This function must be called to get a handle before any other client
  functions are called.  The handle must be started (see
  dhcp6_clienti_client_start() below) before the core is fully functional.
  The callback must be called from this module before the start is called.
===========================================================================*/
void *
dhcp6_clienti_client_new
(
  dhcp6_client_client_init * init
);

/*==========================================================================
FUNCTION DHCP6_CLIENTI_CLIENT_START

Description:
  This funtion configures the client with the information on how to interact
  with other dhcp client modules.

Arguments:
  void * handle - Handle to the client object to start.
  dhcp6_client_client_config * config - The configuartion information needed
    to interface with other modules in the DHCP client architecture.

Return value:
  boolean - TRUE if successful, otherwise FALSE.

Dependencies:
  The handle must have been allocated by dhcp6_clienti_client_new() above.
===========================================================================*/
boolean
dhcp6_clienti_client_start
(
  void * handle,
  dhcp6_client_client_config * config
);

/*===========================================================================
FFUNCTION DHCP6_CLIENTI_CLIENT_FREE

DESCRIPTION:
  This function free the memory associated with DHCP client client layer.
  Additionally, the handle pointer will be NULL'd.

Arguments:
  void ** handle - The handle to the client layer to release.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp6_clienti_client_new() above.
===========================================================================*/
void
dhcp6_clienti_client_free
(
  void ** handle
);

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CLIENT_REQUEST_NEW

Description:
  This function allocates a new request object.  This object is used to build
  a request to be sent out through the client layer.  Data is added to the
  object using the dhcp6_clienti_client_request_pushdown_tail function, and
  sent using the dhcp6_clienti_client_request_send function which will also
  free the object.  If the send should be aborted after the object has been
  allocated, then the dhcp6_clienti_client_request_free function should be
  used.

Arguments:
  void * handle - Handle to the client layer object allocated by
    dhcp6_clienti_client_new().
  void ** msg - The request object to be allocated.
  uint32 size - The size of the message to be sent.

Return value:
  boolean - TRUE if msg allocated correctly, otherwise FALSE.
  void ** msg - The allocated object.

Dependencies:
  The client layer must have been properly started using
  dhcp6_clienti_client_start().
===========================================================================*/
boolean
dhcp6_clienti_client_request_new
(
  void * handle,
  void ** msg,
  uint32 size
);

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CLIENT_REQUEST_APPEND

Description:
  This function adds data to the end of a request object.

Arguments:
  void * handle - The client layer handle.
  void ** msg - The request object to add data to.
  void * data - The data to add
  uint32 size - The amount of data to add.

Return value:
  boolean - TRUE if data added correctly, otherwise FALSE.

Dependencies:
  The client layer must have been properly started using
  dhcp6_clienti_client_start().
  The msg must have been allocated using the
  dhcp6_clienti_client_request_new() function above.
===========================================================================*/
boolean
dhcp6_clienti_client_request_append
(
  void * handle,
  void ** msg,
  void * data,
  uint32 size
);

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CLIENT_REQUEST_SEND

Description:
  This function sends the data collected in the request object out the
  configured interface.

Arguments:
  void * handle - The client layer handle.
  void ** msg - The request object to send.
  struct ps_in6_addr ip_addr - The IPv6 address to which to send the data.

Return value:
  None.  The msg handle will be freed by the client layer after the message
  has been sent.

Dependencies:
  The client layer must have been properly started using
  dhcp6_clienti_client_start().
  The msg must have been allocated using the dhcp6_clienti_client_request_new()
  function above.
===========================================================================*/
void
dhcp6_clienti_client_request_send
(
  void * handle,
  void ** msg,
  struct ps_in6_addr ip_addr
);

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CLIENT_REQUEST_FREE

Description:
  This function frees the allocated request object.  This function should
  only be called if the object is NOT to be sent with the
  dhcp6_client_cleint_request_send() function above.

Arguments:
  void * handle - The client layer handle.
  void ** msg - The request object to send.

Return value:
  None. The msg handle will be freed.

Dependencies:
  The client layer must have been properly started using
  dhcp6_clienti_client_start().
  The msg must have been allocated using the dhcp6_clienti_client_request_new()
  function above.
===========================================================================*/
void
dhcp6_clienti_client_request_free
(
  void * handle,
  void ** msg
);

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
CONFIGURATION LAYER

  The configuration layer configures the network interface with the
  appropiate information as the DHCP server provides.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CONFIGURATION_NEW

Description:
  This function initializes the configuration layer with internal data and
  returns a handle to the core object.

Arguments:
  dhcp6_client_configuration_init * init - The information needed to
    initialize the configuration layer.

Return value:
  void * - A handle to the new configuration object, or NULL on failure.

Dependencies:
  This function must be called to get a handle before any other configuration
  functions are called.  The handle must be started (see
  dhcp6_clienti_configuration_start() below) before the configuration layer is
  fully functional.
===========================================================================*/
void *
dhcp6_clienti_configuration_new
(
   dhcp6_client_configuration_init * init
);

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CONFIGURATION_START

Description:
  This function configures the configuration layer with the information on
  how to interact with the other client modules.

Arguments:
  void * handle - Handle to the configuration object to start.
  dhcp6_client_configuration_config * config - The configuration information
  needed to interface with other modules in the DHCP client architecture.
  This struct is unused in the DHCPv6 stateless client, but may be needed
  later for the stateful client.
  void * dhcp6_handle - DHCP client handle to set in the ps_iface.

Return value:
  Boolean - TRUE if success, otherwise FALSE.

Dependencies:
  The handle must be have been allocated by dhcp6_clienti_configuration_new()
  above.
===========================================================================*/
boolean
dhcp6_clienti_configuration_start
(
  void                              * handle,
  dhcp6_client_configuration_config * config,
  void                              * dhcp6_handle
);

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CONFIGURATION_FREE

Description:
  This function frees the memory associated with DHCP client configuration
  layer.  Additionally, the handle pointer will be NULL'd.

Arguments:
  void ** handle - The handle to the configuration layer object to release.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp6_clienti_configuration_new().
===========================================================================*/
void
dhcp6_clienti_configuration_free
(
  void ** handle
);

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CONFIGURATION_TEST

Description:
  This function is legacy API from the DHCPv4 client.

  This function is not implemented in the DHCPv6 stateless client, no testing
  at the configuration is needed as no lease is set.
  A DHCPv6 stateful client, if implemented, would need this functionality.

Arguments:
  void * handle - Handle to the configuration layer object.
  dhcp6_client_provision * provision - The lease information needed to
    configure the interface

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
void
dhcp6_clienti_configuration_test
(
  void * handle,
  dhcp6_client_provision * provision
);

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CONFIGURATION_SET

Description:
  This function configures the network interface associated with the
  configuration layer handle.

Arguments:
  void * handle - Handle to the configuration layer object.
  dhcp6_client_provision * provision - The lease information needed to
    configure the interface

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp6_clienti_configuration_new().
  The handle must have been started by dhcp6_clienti_configuration_start().
===========================================================================*/
ps_iface_ioctl_extended_ip_config_ex_type
dhcp6_clienti_configuration_set
(
  void * handle,
  dhcp6_client_provision * provision
);

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CONFIGURATION_EVENT_IND

Description:
  This function issues an IFACE_EXTENDED_IP_CONFIG event indication
  on the interface associated with the configuration layer handle.

Arguments:
  void    * handle   - Handle to the configuration layer object.
  boolean got_config - whether the DHCP client was successful.
  config_change_mask - Mask indicating updated config items.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp6_clienti_configuration_new().
  The handle must have been started by dhcp6_clienti_configuration_start().
===========================================================================*/
void
dhcp6_clienti_configuration_event_ind
(
  void                                      *handle,
  ps_iface_ioctl_extended_ip_config_ex_type  config_change_mask
);

/*===========================================================================
FUNCTION DHCP6_CLIENTI_CONFIGURATION_CLEAR

Description:
  This function unconfigures (or clears) the network interface associated with
  the configuration layer handle.

Arguments:
  void * handle - Handle to the configuration layer object.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp6_clienti_configuration_new().
  The handle must have been started by dhcp6_clienti_configuration_start().
===========================================================================*/
void
dhcp6_clienti_configuration_clear
(
  void * handle
);

/*=========================================================================
FUNCTION DHCP6_CLIENTI_CORE_IS_DHCP_CORE_IN_PROGRESS

DESCRIPTION
  This function returns whether the DHCP client CORE is operational.

DEPENDENCIES
  The core must be started.

PARAMETERS
  void * handle - handle to DHCP Client core.

RETURN VALUE
  TRUE if client is running, FALSE otherwise.

SIDE EFFECTS
  None.
=========================================================================*/
boolean
dhcp6_clienti_core_is_dhcp_core_in_progress
(
  void * handle
);

#endif /* FEATURE_DATA_PS_IPV6 */

#endif /* DHCP6_CLIENTI_H */
