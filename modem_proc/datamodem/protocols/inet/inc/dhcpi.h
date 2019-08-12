#ifndef DHCPI_H_
#define DHCPI_H_
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                D H C P I . H

GENERAL DESCRIPTION
  THIS IS A DHCP INTERNAL HEADER FILE.  NO SYSTEM OTHER THAN DHCP
  SHOULD INCLUDE THIS!  See dhcp.h instead.

  This file describes the API's between each of the modules that make
  up the DHCP server.  These modules include the manager, the core, the
  server shim and the address manager.

REFERENCES
  Useful information on how this DHCP Server works, the following RFCs
  might prove useful.
  - RFC 2131 - Latest definition of DHCP protocol.
  - RFC 2132 - Latest definition of bootp/DHCP options.
  - RFC 3396 - Encoding long options.

OVERVIEW

  DHCP is an extention on the original bootp protocol.  The core
  message is identical to the bootp message (although for
  clarification, some of the field names have been changed).  This
  allows for backward compatability, although in our case, this in not
  much of an issue.  DHCP and bootp are protocols that run over UDP.

  This document details the interfaces and interactions with other
  subsystems such as the client interface, the routed interface, DNS,
  etc. 

  This document will not go into detail of the internal implementation
  of the DHCP server parser itself.  A list of the initially supported
  DHCP options is appended.

ORGININIZATION

  The DHCP server consists of several layers.  These layers perform
  different tasks, and connect to each other through well defined
  APIs.  The idea is that one or more of the layers can be replaced
  easily if more complicated behaviour is needed, without replacing
  the other layers.  Also, there may be more than one version of some
  layers, for example for test.

  The layers are the following:
   - DHCP mgr: This handles gluing together various bits of the DHCP
     system together and handles starting and stopping of the DHCP
     server for a particular Client/DHCP Address Manager interface
     pair.
   - DHCP core: This is the message parser and option handlers. 
   - Client shim: This interfaces with the client interface for
     message handling. 
   - DHCP address manager: This handles the policies on allocation of
     addresses and interfaces to the remote interface.   This will
     also interface with other subsystems such as policies on getting
     DNS server addresses. 

  a simple diagram of the system:

    Interface 
    Controller
        |
        +---DHCP Mgr -----------------+
              |   |                   |
              | +-DHCP Core------+    |
              | |                |    |
          Server Shim -------- DHCP Address Manager
               |                    |
             Server             Other Systems 
             Interface

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Each module (except dhcp_mgr) has a _new() and a _start().  All of
  the _new()'s must be called before any of the _start()'s.
  The address manager must be started before the dhcp_server_start
  is called, since the server start calls into the address manager
  to get parameters. 

  All external functions of this file must be called from the PS task
  context. 

Copyright (c) 2004-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/dhcpi.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/03/14    mp     Added the support for SIP server option.
11/01/13    rk     Removed feature mobileap.
10/12/11    am     Added server iface in AM init struct.
08/29/11    ag     Added the support for NBNS option.
06/02/10    cp     IOCTL changes for Soft AP DHCP server.
05/03/10    cp     DHCP Soft AP changes.
02/16/10    cp     Changes to propagate the UM iface mtu value to TE.
10/22/08    am     Changes to ARP IP cache update if dst IP addr is bcast.
09/26/08    am     Changed malloc/free names for off target support.
11/27/06    jd     Added DHCP message callbacks
04/13/04    clp    Initial development work done.  Added comments.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "ps_in.h"
#include "ps_iface.h"
#include "dhcp.h"
#include "dssocket.h"

/*===========================================================================

                      DEFINITIONS AND DECLARATIONS 

===========================================================================*/

/*===========================================================================
                                CORE
===========================================================================*/

/* -------------------------------------------------------------------
   Enum for the operation of message.
  ------------------------------------------------------------------- */
typedef enum {
  DHCP_BOOTP_OP_BOOTREQUEST = 1,
  DHCP_BOOTP_OP_BOOTREPLY = 2
} dhcp_bootp_op_enum;

/* -------------------------------------------------------------------
   Sizes and Offsets of some important bootp message fields, in bytes
  ------------------------------------------------------------------- */
#define DHCP_BOOTP_SIZE (240)
#define DHCP_CHADDR_LEN (16)
#define DHCP_SNAME_LEN (64)
#define DHCP_FILE_LEN (128)

#define DHCP_BOOTP_OFFSET (0)
#define DHCP_SNAME_OFFSET (44)
#define DHCP_FILE_OFFSET (108)
#define DHCP_OPTION_OFFSET (240)

/* -------------------------------------------------------------------
   This structure contains infomation parsed from the current message,
   and information that has been collected for the reply.  This is
   used to exchange information with the DHCP Address Manager layer. 
  ------------------------------------------------------------------- */
typedef struct {
  /* bootp message parameters themselves */
  dhcp_bootp_op_enum op; /* message op code */
  uint32 htype;          /* hardware address type */
  uint32 hlen;           /* hardware address length */
  uint32 hops;           /* number of hops (bootp relays) */
  uint32 xid;            /* random transaction id */
  uint16 secs;           /* number of seconds client attemping */
  uint16 flags;          /* flags (broadcast/unicast) */
  uint32 ciaddr;         /* client IP address */
  uint32 yiaddr;         /* your (assigned) IP address */
  uint32 siaddr;         /* next server IP adderss in bootstrap process */
  uint32 giaddr;         /* relay agent IP address */
  uint8  chaddr[DHCP_CHADDR_LEN]; /* Client hardware address */
  uint8  sname[DHCP_SNAME_LEN];   /* server host name (optional) */
  uint8  file[DHCP_FILE_LEN];     /* boot file name */
  uint32 magic;                   /* 99.130.83.99 dec == 63.82.53.63 hex */

  /* Server ID from client */
  uint32 server_id;

  /* Subnet mask */
  uint32 netmask;

  /* DHCP message type */
  dhcp_msg_type dhcp_msg_request;
  dhcp_msg_type dhcp_msg_reply;

  /* hostname option info */
  uint8 * hostname;
  uint32  hostnamelen;

  /* Client ID */
  uint8 * client_id;
  uint32  client_id_len;

  /* Domain name for client */
  uint8 * domainname;
  uint32  domainnamelen;

  /* requested IP addr */
  uint32 requested_ip;

  /* ip lease times in seconds relative to now, see RFC2131, 4.4.5 */
  uint32 ip_lease; 
  uint32 renew_time; 
  uint32 rebind_time;

} dhcp_core_msg_info;

/* -------------------------------------------------------------------
  This structure is used to initialize internal DHCP core stuff.
  d_malloc - Function that can be used for getting memory. See DHCP
             Manager.
  d_free - Function that can be used for freeing memory. See DHCP
           Manager.
  ------------------------------------------------------------------- */
typedef struct {
  void * (*d_malloc)(uint32 size);
  void (*d_free)(void *);
} dhcp_core_init;

/* -------------------------------------------------------------------
  This structure is used to pass info into the DHCP Core for
  configuring.  It contains pointers to each of the functions that
  the DHCP Core needs to either get information for the reply, or how
  to send the reply. 

  The functions to be filled out in this structure are as follows:
  server_reply_new - allocates (as nessecary) msg for a message of 
                     size. See server shim.
  server_reply_pushdown_tail - adds data of size to end of msg.
                               Increments internal end as needed.  See
                               server shim.
  server_reply_send - Send msg to ip.  This frees msg. See server shim.
  server_reply_free - Free msg in case of previous error. See server
                      shim. 
  server_reply_handle - The handle to use when calling the server reply
                        functions.

  lease_get - This is the DHCP Address Manager function that
              allocates DHCP leases. See DHCP Address Manager. 
  lease_free - This is the DHCP Address Manager function that free
               DHCP leases. See DHCP Address Manager.
  lease_err - This is the DHCP Address Manager function that handles
              botched DHCP leases. See DHCP Address Manager.
  lease_handle - The handle to use when calling the lease functions.

  dns_first - Get the first DNS address.  Return TRUE if address
              returned, FALSE if address unavailable. See DHCP
              Address Manager
  dns_next - Get the next DNS address.  Return TRUE if address
             returned, FALSE if no more addresses to return. See
             DHCP Address Mananger
  dns_handle - The handle to use when calling the DNS functions.
  interface_mtu - Get the mtu value. Return TRUE if value is 
                  returned, FALSE otherwise.
  interface_mtu_handle - The handle to use when calling the interface_mtu
                         function.
  sip_addr_list - Get the SIP addresses.
  sip_addr_handle - The handle to use when calling the sip_addr_list
                         function.
  sip_domain_name_list - Get the SIP Domain names.
  sip_domain_name_handle - The handle to use when calling the
                           sip_domain_name_list function.
  ------------------------------------------------------------------- */
typedef struct {
  boolean (*server_reply_new)(void * handle, void ** msg, uint32 size);
  boolean (*server_reply_pushdown_tail)(void * handle, void ** msg,
                                        void * data, uint32 size);
  void (*server_reply_send)(void * handle, void ** msg, uint32
                            ip_addr, uint8 * hwaddr, uint32 hwaddrlen,
                            uint32 arp_ip_addr );
  void (*server_reply_free)(void * handle, void ** msg);
  void * server_reply_handle;

  boolean (*lease_get)(void * handle, dhcp_core_msg_info * core);
  boolean (*lease_free)(void * handle, dhcp_core_msg_info * core);
  boolean (*lease_err)(void * handle, dhcp_core_msg_info * core);
  void * lease_handle;

  uint32 (*addr_info_server_id)(void * handle);
  uint32 (*addr_info_netmask)(void * handle);
  void * addr_info_handle;

  boolean (*dns_first)(void * handle, void ** cookie, 
                       uint32 * ip_addr);
  boolean (*dns_next)(void * handle, void ** cookie, uint32 * ip_addr);
  void * dns_handle;

  boolean (*nbns_first)(void * handle, void ** cookie, 
                        uint32 * ip_addr);
  boolean (*nbns_next)(void * handle, void ** cookie, uint32 * ip_addr);
  void * nbns_handle;
  
  uint32 (*interface_mtu)(void * handle);
  void * interface_mtu_handle;
  
  void (*sip_addr_list)(void *handle, ip_addr_type *sip_addr_list,
                        uint32 *count);
  void * sip_addr_handle;

  void (*sip_domain_name_list)(void *handle,
                               ps_iface_domain_name_type *sip_domain_name_list,
                               uint32 *count);
  void * sip_domain_name_handle;

  dhcp_msg_cback_type msg_cback;
  void *              msg_cback_user_data;
} dhcp_core_config;

/*===========================================================================
                              SERVER
===========================================================================*/

/* -------------------------------------------------------------------
  This structure contains configuration information for initialization
  of the server shim.
  d_malloc - Function that can be used for getting memory. See DHCP Manager.
  d_free - Function that can be used for freeing memory. See DHCP Manager.
  iface - The inferface we are talking with.
  ------------------------------------------------------------------- */
typedef struct {
  void * (*d_malloc)(uint32 size);
  void (*d_free)(void *);
  ps_iface_type * iface;
  dss_iface_id_type iface_id;
} dhcp_server_init;

/* -------------------------------------------------------------------
  This structure contains configuration information for the server shim
  to interface with other systems.

  recv_msg - This is the function to which the messages are passed for
             parsing.  See DHCP Core.
  recv_msg_handle - The handle to use when calling recv_msg
  addr_info_server_id - This is the function which supplies the
                        server IPV4 address.
  addr_info_handle - The handle to use when calling addr_info_server_id.
  dhcp_mgr_handle - The dhcp_mgr handle used for setting and clearing
                    PS filters.
  ------------------------------------------------------------------- */
typedef struct {
  void (*recv_msg)
  ( 
    void * handle, 
    void * msg, 
    boolean (*extract)(void * dest, void * src, uint32 offset, uint32 len)
  );
  void * recv_msg_handle;
  uint32 (*addr_info_server_id)(void * handle);
  void * addr_info_handle;
  void * dhcp_mgr_handle;
  dhcp_arp_cache_updt_type arp_cache_update_cb;
} dhcp_server_config;

/*===========================================================================
                              ADDRESS MANAGER
===========================================================================*/

/* -------------------------------------------------------------------
  This structure contains configuration information for initialization
  of the Address Manager.

  d_malloc      - Function that can be used for getting memory.
                  See DHCP Manager.
  d_free        - Function that can be used for freeing memory.
                  See DHCP Manager.
  iface         - Um iface from which IP cfg is retrieved.
  gw_iface      - Rm iface on which server is started.
  config_vals   - DHCP configuration parameters.
  ------------------------------------------------------------------- */
typedef struct {
  void *(*d_malloc)(uint32 size);
  void (*d_free)(void *);
  ps_iface_type * iface;
  ps_iface_type * gw_iface;
} dhcp_am_init;

/* -------------------------------------------------------------------
  This structure contains configuration information for the Address
  Manager to interface with other systems.

  addr_free - A function to call when a lease is freed.
  ------------------------------------------------------------------- */
typedef struct {
  void (*addr_free)(void * handle, uint32 ip_addr, 
                         uint8 * msg, uint32 len );
  void * addr_free_handle;
} dhcp_am_config;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
                                CORE
===========================================================================*/
/*===========================================================================
FUNCTION DHCP_CORE_NEW()

DESCRIPTION
  This function allocates and initializes a handle and returns it.

DEPENDENCIES
  Good parameters.

PARAMETERS
  dhcp_core_init * : Pointer to a structure of functions used to connect to
                       other systems.

RETURN VALUE
  void * - handle for dhcp_core functions.

SIDE EFFECTS
  Handle will be allocated.
===========================================================================*/
void *
dhcp_core_new
(
  dhcp_core_init * init
);

/*===========================================================================
FUNCTION DHCP_CORE_START()

DESCRIPTION
  This function registers the functions used for this interface handle and
  does any per connection initialization needed for the DHCP Core system.

DEPENDENCIES
  The handle must be a valid handle allocated by dhcp_core_new.

PARAMETERS
  dhcp_handle_type * - Handle to the glue needed to get at the connection
                       details.
  dhcp_core_config * - Pointer to a structure of functions used to connect to
                       other systems.

RETURN VALUE
  Boolean - TRUE if ok, FALSE if failure.

SIDE EFFECTS
  Handle is finised initializing. 
===========================================================================*/
boolean
dhcp_core_start
( 
  void * handle,
  dhcp_core_config * config
);

/*===========================================================================
FUNCTION DHCP_CORE_STOP()

DESCRIPTION
  This function unregisters the functions used for this interface
  handle and does any per connection cleanup needed for the DHCP Core
  system.

DEPENDENCIES

PARAMETERS
  dhcp_handle_type * - Handle to the glue needed to get at the connection
                       details.

RETURN VALUE
  None

SIDE EFFECTS
  The handle passed in is freed.
===========================================================================*/
void 
dhcp_core_stop
(
  void ** handle
);

/*===========================================================================
FUNCTION DHCP_CORE_INPUT()

DESCRIPTION
  This function processes the message passed in, and sends a reply if
  needed.  This is the core of this system.

DEPENDENCIES
  Handle must be valid.

PARAMETERS
  dhcp_handle_type * - Handle to the glue needed to get at the
                       connection details.
  char * msg - Handle to message to process.
  extract() - Function to call to extract message.  The parameters are:
               * dest - buffer to fill into.
               * src - object to get data from.
               * offset - offset from start of message in src object.
               * len - length of data to extract from offset.
              This function return TRUE if successful, else FALSE.

RETURN VALUE
  None.

SIDE EFFECTS
  Message is processed.  
  Reply may (or may not) be sent.
  State of leases may change.
  State in handle may change.
===========================================================================*/
void 
dhcp_core_input
(
  void * handle,
  void * msg, 
  boolean (*extract)(void * dest, void * src, uint32 offset, uint32 len)
);

/*===========================================================================
                              SERVER
===========================================================================*/
/*===========================================================================
FUNCTION DHCP_SERVER_NEW()

DESCRIPTION
  This function allocates and intializes the server shim.

DEPENDENCIES
  Info in init must be valid.

PARAMETERS
  dhcp_server_init * - pointer to functions used to init the server system.

RETURN VALUE
  Handle for server functions.

SIDE EFFECTS
  Allocates a server shim handle. 
===========================================================================*/
void * 
dhcp_server_new
( 
  dhcp_server_init * init
);

/*===========================================================================
FUNCTION DHCP_SERVER_START()

DESCRIPTION
  This function registers the functions that interface the server shim
  with the DHCP Core.  This function also configures server to
  receieve DHCP messages.  This function registers the filter with the
  PS layer to receive DHCP messages.

DEPENDENCIES
  Handle must have been allocated by dhcp_server_new() above.
  Config must contain valid information.

PARAMETERS
  dhcp_handle_type * - Handle to the glue needed to get at the
                       connection details.
  dhcp_server_config * - pointer to functions used to connect to other
                         systems.

RETURN VALUE
  boolean - TRUE if successful, FALSE if not.

SIDE EFFECTS
  Registers the filter with the PS to receive DHCP messages.  
===========================================================================*/
boolean
dhcp_server_start
( 
  void               * handle, 
  dhcp_server_config * config
);

/*===========================================================================
FUNCTION DHCP_SERVER_STOP()

DESCRIPTION
  This function unregisters the functions that interface the server shim.
  This function also cleans up any connections with the PS including clearing
  any filters that are in place.

DEPENDENCIES
  Handle must be valid.

PARAMETERS
  dhcp_handle_type * - Handle to the glue needed to get at the connection
                       details.

RETURN VALUE
  None.

SIDE EFFECTS
  Unregisters filters associated with receiving messages on the server
  interface associated with handle.
  Frees handle.
===========================================================================*/
void 
dhcp_server_stop
( 
  void ** handle
);

/*===========================================================================
FUNCTION DHCP_SERVER_REPLY_NEW()

DESCRIPTION
  This function "allocates" as needed a message for the reply.
  In this particular case, we are using dsm, and we don't need to
  prealloc anything.

DEPENDENCIES
  Handle must have been started. 

PARAMETERS
  dhcp_handle_type * - Handle to the glue needed to get at the connection
                       details.
  void ** msg - The pointer to the message to allocate.
  uint32 len - The length of the message.

RETURN VALUE
  boolean - TRUE if successful, FALSE if not.

SIDE EFFECTS
  Updates the server handle.
===========================================================================*/
boolean
dhcp_server_reply_new
(
  void * handle,
  void ** msg,
  uint32 len
);

/*===========================================================================
FUNCTION DHCP_SERVER_REPLY_PUSHDDOWN_TAIL()

DESCRIPTION
  This fucntion appends data of len to msg.

DEPENDENCIES
  Handle must have been started. 

PARAMETERS
  dhcp_handle_type * - Handle to the glue needed to get at the connection
                       details.
  void ** msg - The pointer to the message to allocate.
  void * data - data to append.
  uint32 len - The length to append.

RETURN VALUE
  boolean - TRUE if successful, FALSE if not.

SIDE EFFECTS
  Adds data to msg.  Updates length of current message in handle.
===========================================================================*/
boolean
dhcp_server_reply_pushdown_tail
(
  void * handle,
  void ** msg,
  void * data,
  uint32 len
);

/*===========================================================================
FUNCTION DHCP_SERVER_REPLY_SEND()

DESCRIPTION
  This function takes a reply and sends it to the server interface.

DEPENDENCIES
  IP address must be an IPV4 address.
  The handle must be valid.

PARAMETERS
  dhcp_handle_type * - Handle to the glue needed to get at the connection
                       details.
  void ** msg - The pointer to the message to process.
  uint32 - ip address to send reply to.
  uint8 * - hw address (for arp).
  uint32 - length of hw address.
  uint32 - address to be updated in ARP cache if dest addr is bcast.

RETURN VALUE
  None.

SIDE EFFECTS
  Injects the message into the middle of the UDP stack.
===========================================================================*/
void 
dhcp_server_reply_send
( 
  void * handle, 
  void ** msg, 
  uint32 addr,
  uint8 * hwaddr, 
  uint32 len,
  uint32 arp_ip_addr
);

/*===========================================================================
FUNCTION DHCP_SERVER_REPLY_FREE()

DESCRIPTION
  Frees msg in the case that something went wrong.

DEPENDENCIES
  None.

PARAMETERS
  dhcp_handle_type * - Handle to the glue needed to get at the connection
                       details.
  void ** msg - The pointer to the message to free.

RETURN VALUE
  None.

SIDE EFFECTS
  Frees message.
===========================================================================*/
void
dhcp_server_reply_free
(
  void * handle,
  void ** msg
);

/*===========================================================================
FUNCTION DHCP_SERVER_ADDR_FREE()

DESCRIPTION
  This function cleans up when an address is freed by the address
  manager.  This consists of cleaning the ARP cache for the freed
  lease.

DEPENDENCIES
  The IP address must be an IPV4 address.
  The handle must be valid. 

PARAMETERS
  dhcp_handle_type * - Handle to the glue needed to get at the connection
                       details.
  uint32 - ip address to associate with the hardware address.
  char * hwaddr - hardware address.
  uint32 len - The length of the hardware address.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates (removes the IP address from) the ARP cache.
===========================================================================*/
void 
dhcp_server_addr_free
( 
  void * handle, 
  uint32 ip_addr,
  uint8 * hwaddr, 
  uint32 len
);

/*===========================================================================
FUNCTION DHCP_SERVER_SET_PARAMETERS()

DESCRIPTION
  This function interfaces between the DHCP Address manager and the
  server interface for informing the server interface of the operating
  parameters for this connection.

DEPENDENCIES
  dhcp_server_start() must be called first. (So that the server iface is known).

PARAMETERS
  dhcp_handle_type * - Handle to the glue needed to get at the connection
                       details.
  uint32 route_ip - The address that the server interface should answer
                          to.
  uint32 netmask - The subnet mask for the server interface.

RETURN VALUE
  None.

SIDE EFFECTS
  The PS interface for this handle is informed of the parameters
  passed in. 
===========================================================================*/
void 
dhcp_server_set_parameters
( 
  void * handle, 
  uint32 ip_ptr,
  uint32 netmask 
);

/*===========================================================================
FUNCTION DHCP_SERVER_INPUT()

DESCRIPTION
  This function causes the message to be parsed, and the response to
  be sent in the way configured by dhcp_*_start().

DEPENDENCIES
  Handle must be valid. 

PARAMETERS
  void * handle - The handle of the DHCP session the message came in on.
  void * msg - The msg to parse and respond to.
  extract() - The function to extract the message with.

RETURN VALUE
  None.

SIDE EFFECTS
  Message send to dhcp_core for parsing.
===========================================================================*/
void
dhcp_server_input
( 
  void * handle,
  void * msg,
  boolean (*extract)(void * dest, void * src, uint32 offset, uint32 len)
);

/*===========================================================================
                              ADDRESS MANAGER
===========================================================================*/

/*===========================================================================
FUNCTION DHCP_AM_MP_PS_SINGLE_NEW()

DESCRIPTION
  This function allocates and initializes a dhcp_am handle with
  multiplexing support for the PS single IP address Address Manager.
  This works by keeping track of all existing PS single address
  manager handles that have been allocated.  If we are reusing one,
  then pass that handle back.  If not, then set up a new MP address
  manager that points at the PS single address manager. 

DEPENDENCIES
  The handle must be "started" after the other subsystems have been
  created.  The handle should be freed when it is no longer needed. 

PARAMETERS
  dhcp_am_init * - Internal configuration information.  This includes
                   the interface to connect to. 

RETURN VALUE
  handle for calling am functions. NULL on failure.

SIDE EFFECTS
  None
===========================================================================*/
void *
dhcp_am_mp_ps_single_new
( 
  dhcp_am_init * init
);

/*===========================================================================
FUNCTION DHCP_AM_MP_START()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function registers the functions that interface the DHCP Address
  Manager with other DHCP systems.  This function also calculates the default
  route ip address and netmask that should be provided to clients, and this is
  provided back to the client interface via the provided function in the
  dhcp_am_config structure. 

DEPENDENCIES
  The handle must have been previously allocated using
  dhcp_am_mp_ps_single_new()/dhcp_server_am_mp_new().

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  dhcp_am_config * - pointer to functions used to connect to other
                           systems.

RETURN VALUE
  TRUE - if successful. 
  FALSE - if there are problems.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_mp_start
( 
  void * handle,
  dhcp_am_config * config
);

/*===========================================================================
FUNCTION DHCP_AM_MP_STOP()

DESCRIPTION
  This function unregisters the functions that interface the DHCP Address
  Manager. 

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void
dhcp_am_mp_stop
( 
  void ** handle
);

/*===========================================================================
FUNCTION DHCP_AM_MP_GET_LEASE()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fills out the following IP lease information in the
  dhcp_core_msg_info structure, if a lease can be filled out:
    - Assigned IP.
    - Server IP.
    - Router IP.
    - Subnet Mask.
    - Lease time.
    - Renew time (T1).
    - Rebind time (T2).
  This function is called in the event of a DISCOVER, REQUEST or INFORM
  message.
  If a NAK is desired, set reply_type to DHCP_DHCPNAK and return FALSE.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  dhcp_core_msg_info * - This provides information about the client for
                         calculating the values to return.  Also used to return
                         the results. 

RETURN VALUE
  uint32 - non-zero if lease filled out ok.  zero if there is a problem. 
  dhcp_core_msg_info * - Results filled out in this structure.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_mp_get_lease
( 
  void * handle,
  dhcp_core_msg_info * core
);

/*===========================================================================
FUNCTION DHCP_AM_MP_FREE_LEASE()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function frees a previously allocated lease.  This function can be
  called either on a release from a client, or is a previously allocated lease
  is not actually sent out due to other errors.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  dhcp_core_msg_info * - This provides information about the client for
                         calculating the lease to free. 

RETURN VALUE
  boolean - TRUE if SUCCESS.
          - FALSE if FAILS.

SIDE EFFECTS
  None
===========================================================================*/
boolean 
dhcp_am_mp_free_lease
(
  void * handle,
  dhcp_core_msg_info * core
);

/*===========================================================================
FUNCTION DHCP_AM_MP_ERR_LEASE()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function informs the DHCP Address Manager that a lease that was
  previously given out had a problem.  This function is called on a
  DHCPDECLINE message from the client.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  dhcp_core_msg_info * - This provides information about the client for
                         calculating the lease. 

RETURN VALUE
  boolean - TRUE if SUCCESS.
          - FALSE if FAILS.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_mp_err_lease
( 
  void * handle,
  dhcp_core_msg_info * core
);

/*===========================================================================
FUNCTION DHCP_AM_MP_DNS_FIRST()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the DNS server information (if available) and fills
  out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  void ** - This is user data that will be returned to get to the next
            DNS address.
  ip_add_type * - The first dns address.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - no more addresses.
  void ** - user data to get to next address.


SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_mp_dns_first
( 
  void * handle,
  void ** user_data,
  uint32 * ip_addr
);

/*===========================================================================
FUNCTION DHCP_AM_MP_DNS_NEXT()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the DNS server information (if available) and fills
  out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  void ** - This is user data that will be returned to get to the next
            DNS address.
  ip_add_type * - The next dns address.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - no more addresses.
  void ** - user data to get to next address.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_mp_dns_next
( 
  void * handle, 
  void ** user_data,
  uint32 * ip_addr
);


/*===========================================================================
FUNCTION DHCP_AM_MP_NBNS_FIRST()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the NBNS server information (if available) and fills
  out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  handle    - Handle to the glue needed to get at the connection
              details.
  user_data - This is user data that will be returned to get to the next
              NBNS address.
  ip_addr   - The first nbns address.

RETURN VALUE
  boolean   - TRUE is address is valid. FALSE - no more addresses.
  user_data - user data to get to next address.


SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_mp_nbns_first
( 
  void * handle,
  void ** user_data,
  uint32 * ip_addr
);

/*===========================================================================
FUNCTION DHCP_AM_MP_NBNS_NEXT()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the NBNS server information (if available) and fills
  out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  handle    - Handle to the glue needed to get at the connection
              details.
  user_data - This is user data that will be returned to get to the next
              NBNS address.
  ip_addr   - The next nbns address.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - no more addresses.
  void ** - user data to get to next address.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_mp_nbns_next
( 
  void * handle, 
  void ** user_data,
  uint32 * ip_addr
);


/*===========================================================================
FUNCTION DHCP_AM_MP_ADDR_INFO_SERVER_ID()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the IP address of the server (us).

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.

RETURN VALUE
  uint32 - The IP address of the server.

SIDE EFFECTS
  None
===========================================================================*/
uint32
dhcp_am_mp_addr_info_server_id
( 
  void * handle 
);

/*===========================================================================
FUNCTION DHCP_AM_MP_ADDR_INFO_NETMASK()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the netmask of the server (us).

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.

RETURN VALUE
  uint32 - The netmask of the subnet we are on.

SIDE EFFECTS
  None
===========================================================================*/
uint32
dhcp_am_mp_addr_info_netmask
( 
  void * handle 
);

/*===========================================================================
FUNCTION DHCP_AM_MP_MTU()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the MTU of the server (us).

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
           details.

RETURN VALUE
  uint32 - The MTU of the server.

SIDE EFFECTS
  None
===========================================================================*/
uint32
dhcp_am_mp_mtu
( 
  void * handle 
);

/*===========================================================================
FUNCTION DHCP_AM_MP_GET_IP_CONFIG()

DESCRIPTION
  This function returns the IP config for the address
    manager associated with the DHCP handle.

  This is invoked only in single client DHCP case.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
           details.
  3*uint32* - IP params

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dhcp_am_mp_get_ip_config
(
  void   * handle,
  uint32 * client_ip,
  uint32 * server_id,
  uint32 * netmask
);

/*===========================================================================
FUNCTION DHCP_AM_MP_SIP_ADDR_LIST()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the SIP server information (if available) for this AM.
DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  ip_addr_type * - pointer to the SIP server addresses.
  uint32 * : as input value it contains the count of the SIP server
          addresses asked for and as a return value it contains the
          actual number of addresses returned.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void
dhcp_am_mp_sip_addr_list
(
  void * handle,
  ip_addr_type * sip_addr_list,
  uint32 * count
);


/*===========================================================================
FUNCTION DHCP_AM_MP_SIP_DOMAIN_NAME_LIST()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the SIP server domain name information for this AM.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  dns_domain_name_list: pointer to an array of SIP domain names.
  uint32 * : as input value it contains the count of the SIP domain
          names asked for and as a return value it contains the
          actual number of domains names returned.


RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void
dhcp_am_mp_sip_domain_name_list
(
  void * handle,
  ps_iface_domain_name_type  *sip_domain_name_list,
  uint32 * count
);
#endif /* DHCPI_H_ */
