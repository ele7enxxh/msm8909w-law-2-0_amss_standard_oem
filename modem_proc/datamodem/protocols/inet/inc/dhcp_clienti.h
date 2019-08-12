#ifndef DHCP_CLIENTI_H
#define DHCP_CLIENTI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      D H C P _ C L I E N T I . H

GENERAL DESCRIPTION
  THIS IS A DHCP CLIENT INTERNAL HEADER FILE.  NO SYSTEM OTHER THAN
  DHCP CLIENT SHOULD INCLUDE THIS!  See dhcp_client.h instead.

  This file describes the API's between each of the modules that make
  up the DHCP client.  These modules include the manager, the core, the
  client shim, configuration shim and the persistant storage.

REFERENCES
  Useful information on how this DHCP client works, the following RFCs
  might prove useful.
  - RFC 2131 - Latest definition of DHCP protocol.
  - RFC 2132 - Latest definition of bootp/DHCP options.
  - RFC 3396 - Encoding long options.

OVERVIEW

  Dynamic Host Configuration Protocol is defined by RFC 2131.  The
  options are defined in RFC 2132 and additional documents.  Long
  option handling is expanded apon in RFC 3396.

  There are 2 potential uses for the DHCP client code.  First, for
  configuration of the local networking parameters when on a
  non-cellular network (eg. 802.11).  Second, as a way of obtaining
  parameters for the DHCP server used for the OSif/WWAN project.  Both
  of these goals will need to be kept in mind for the purposes of this
  design.

REQUIREMENTS

  - RFC 2131 compliant client.
  - Client must manage 1 lease per interface.

THOERY OF OPERATION

  The DMSS DHCP server runs as part of the PS task.  Although the DHCP
  client will use the DSS API to communicate with a remote DHCP
  server, it is anticipated that the client will also run as part of
  the PS task.

LAYERING

  The DHCP client will be layered in a Object Oriented way, similar to
  the DHCP server.  The DHCP client will consist of the following
  parts: the manager, the core, the client shim, the configuration
  shim, the timer shim and the persistant storage layer.

  The manager provides external APIs and internal configuration.

  The core provides the DHCP protocol logic, message generation,
  processing and so on.

  The client shim will interface with the DSS API to allow the the
  core to communicate with a remote server.

  The configuration shim will allow the core to interface with the
  local protocol stacks to set/clear the IP address and other
  configured parameters.

  The timer shim interfaces with the timer system to provide a clean
  interface for the core.  This also allows accelerated testing by the
  use of simulated timers.

  The persistant storage layer deals with storage of leases that have
  been allocated to the client.

  a simple diagram of the system:

    External
    Interface
        |
        +---DHCP Mgr ------------+--------------------+--------------+
              |   |              |                    |              |
              | +-DHCP Core----+-|------------------+-|------------+ |
              | |              | |                  | |            | |
          Client Shim  Configuration shim  Persistant storage  Timer shim
               |                |                    |              |
             Client       Network Stack              |            Timer
            Interface     configuration          file system      System


Copyright (c) 2006-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/dhcp_clienti.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/27/15    ds     Fix to handle expansion of DNS names accordingly as both v4 
                   v6 have different encoding techniques.
07/04/14    ss     Added support for generating 
                   IFACE_EXTENDED_IP_CONFIG_EX_EV
07/25/13    cx     Changed prototype of dhcp_client_get_bytes_type due to 
                   replacement of memcpy
08/07/09    kk     Added support for client identifier option (opt type=61).
09/26/08    am     Changed malloc/free names for off target support.
08/25/08    am     Added DHCP ongoing determination.
01/29/08    am     Added Rapid Commit Option handling.
05/02/07    es     Moved set_handle to config layer.
                   Removed ref to dhcp_client_params to fix layering issue.
04/15/07    es     Added configuration event ind function. Removed changes
                   to support previous IOCTL.
02/01/07    hm     Added IOCTL to get fresh DHCP configuration
09/11/06    es     Added SIP option data structs.
08/12/04    clp    File created from design doc.
===========================================================================*/
#include "dhcp_client_common.h"
#include "dhcp_client_timer.h"
#include "dssocket_defs.h"

#include "ps_iface_defs.h"

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
MANAGER LAYER

  The manager layer is a thin layer that will provide the external
  interfaces listed in dhcp_client.h, as well as the logic for gluing
  together the internal components.  Additionally, the manager layer
  will provide mapping functions for internally needed functionality
  such as memory management.

  The manager layer has the following initialization requirements:
    - All modules must be init'd before any are started.
    - The dhcp_clienti_client_init() must be the last module to be
      init'd.
    - The client API is asynchronous, so the module start's will be
      done in a callback from the callback.

  In addition to the above functions, the manager layer will provide
  the following internal functions:
    - malloc/free shim
    - callback for intialization done for the client shim, since the
      startup (open sockets) may be asynchronous
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#define DHCP_CLIENT_MAX_DOMAIN_NAME_BUF_LEN     (256)

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
CORE LAYER

  The core layer takes care of sending messages, maintaining the DHCP
  client state, and dispatching responses.

  The following are the data structures that the API will use.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* The dns structure contains a DNS server IP address (IPV4 only) and a link
 * to the next dns server address (NULL if none).
 */
typedef struct dhcp_client_dns {
  uint32 dns_addr;
  struct dhcp_client_dns * next;
} dhcp_client_dns;

/* The SIP structure contains a SIP server domain name or a SIP server
 * IPV4 address if the server domain name pointer is NULL, and a link
 * to the next SIP server address (NULL if none).
 */
typedef struct dhcp_client_sip_addr {
  uint32 addr;
  struct dhcp_client_sip_addr * next;
} dhcp_client_sip_addr;

/* The SIP structure contains a SIP server domain name or a SIP server
 * IPV4 address if the server domain name pointer is NULL, and a link
 * to the next SIP server address (NULL if none).
 */
typedef struct dhcp_client_sip_domain_name {
  uint8  domain_name[DHCP_CLIENT_MAX_DOMAIN_NAME_BUF_LEN];
  uint32 domain_name_len;
  struct dhcp_client_sip_domain_name * next;
} dhcp_client_sip_domain_name;

/* The provision structure contains the information needed by the
 * configuration layer to provision the interface.
 */
typedef struct {
  uint32 ip_addr;
  uint32 ip_mask;
  uint32 router;
  dhcp_client_dns * dns_servers;
  uint8 * domain_name;
  uint32 domain_name_len;
  uint16 mtu;
  dhcp_client_sip_addr * sip_addrs;
  dhcp_client_sip_domain_name * sip_domain_names;
  /* etc */
} dhcp_client_provision;

/* The lease structure contains the information needed to be stored away in
 * the persistant storage to shortcut the DHCP client state machine at the
 * next renew
 */
typedef struct {
  boolean valid;
  uint32 ip_addr;
  uint32 server_id;
} dhcp_client_lease;

/* This function type is passed into the callback for domain_name_expand
 * in the core_init struct to extract bytes from the relevant option data */
typedef int (*dhcp_client_get_bytes_type)
(
  void   * src,              /* Buffer to get bytes from */
  uint32   offset,           /* Offset in source buffer to get bytes from */
  char   * dst_buf,          /* Destination buffer */
  uint16   dst_buf_size,      /*Destination buffer size*/
  uint32   len               /* Number of bytes to get */
);

/* This function type is the callback to expand domain_names, passed to
 * the core module via the core_init struct */
typedef int (*dhcp_client_domain_name_expand_cb_type)
(
  void                        * src_buf,
  uint32                        offset,
  char                        * dst_buf,
  uint32                        buf_len,
  uint32                      * name_len,
  dhcp_client_get_bytes_type   get_f,
  boolean                     ip_ver_flag 
);

/* The core params struct contains parameters which determine the behavior of the DHCP client core */
typedef struct
{
  /* Default value is DHCP_CLIENT_HWTYPE_ETHERNET */
  dhcp_client_hw_type_enum  hw_type;
  uint32 hw_addr_len;
  uint8  *hw_addr;

  /* The following params determine if the client request these
   * options in the Parameter Request List Option.
   * If the options are not requested, they are subsequently
   * not set in the ps_iface even if the DHCP server responds
   * with the relevant configuration info.
   * Suggested value for these options is TRUE. */
  boolean domain_name_server_option; /* RFC 2132, 3.8 */
  boolean subnet_mask_option;        /* RFC 2132, 3.3 */
  boolean router_option;             /* RFC 2132, 3.5 */
  boolean interface_mtu_option;      /* RFC 2132, 5.1 */
  boolean domain_name_option;        /* RFC 2132, 3.17 */
  boolean sip_server_option;         /* RFC 3361 */
  boolean rapid_commit_option;       /* RFC 4039 */

  /* Number of times to try at various states */
        /* The number of tries to request at startup before starting
         * over with discover. Suggest 2
         */
  uint32 reboot_tries;
        /* The number of tries to send the discover before starting
         * over in the init state.  This is not well defined in the
         * RFC, but allows us to count the number of times through the
         * init state to indicate a failure to get a lease.  Suggest 4
         */
  uint32 selecting_tries;
        /* The number of tries to send the request message in the
         * requesting state.  Suggest 4
         */
  uint32 requesting_tries;

  /* Number of times before "failing" */
        /* This is not specified by RFC, but this is the number of
         * times we end up in the init state without successfully
         * getting a lease before we tell the upper layer that we have
         * failed to get a lease.  The upper layer may have us
         * continue (our default), or to abort.  Suggest 3
         */
  uint32 init_tries;
        /* This is also not specified by RFCm by this is the number of
         * timers we try to send an inform message before we tell the
         * upper layer that we have failed to get provisioning.  We
         * will stop trying on failure, per RFC 2131, section 4.4.3.
         * Suggest 4
         */
  uint32 inform_tries;

  /* Times to wait at the beginning of various states */
        /* The amount of time to wait before starting the init state
         * after a decline. RFC suggests a minimum of 10 (seconds)
         */
  uint32 decline_restart_time;
        /* This sets the maximum random time (between 1 second and
         * this value) to wait before sending the request in the init
         * state.  The RFC suggests 10 (seconds)
         */
  uint32 init_restart_time;
        /* This is the time to allow the ARP layer to verify the
         * address that we got in the discover.  The RFC gives no
         * guidance.  Suggest 2 (seconds)
         */
  uint32 verify_time;

  /* Base times for exponential back off.  These are the times that
   * are doubled on each failure to get the expected result to retry,
   * as specified by RFC 2131, section 4.1
   */
        /* This sets the initial time between requests in the reboot
         * state.  The RFC suggests 4 (seconds).
         */
  uint32 reboot_base_time;
        /* This sets the initial time between discover messages in the
         * select state.  The RFC gives minimal guidance. Suggest 4
         * (seconds)
         */
  uint32 select_base_time;
        /* This sets the initial time between request messages in the
         * requesting state.  Suggest 4 (seconds)
         */
  uint32 request_base_time;
        /* This sets the initial time between inform messages in the
         * inform state.  Suggest 4 (Seconds)
         */
  uint32 inform_base_time;

  /* Address DHCPv4 that client tries to send DHCP messages to
   * when send_broadcast is true. Default is DHCP_CLIENT_BROADCAST_ADDR */
  uint32 server_addr;

  dhcp_client_id_option_type_enum client_id_type;
  uint32 client_id_len;
  uint8 client_id[DHCP_CLIENT_CLIENT_ID_MAX_LEN];
} dhcp_client_core_params;

/* The core init structure contains the information that is provided to the
 * core on init
 */
typedef struct {
  void * (*d_malloc)(uint32 size);
  void (*d_free)(void *);
  dhcp_client_core_params core_params;
  uint32 xid; /* A random number */
  dhcp_client_status_cb_type status_cb;
  void * userdata;
  dhcp_client_domain_name_expand_cb_type domain_name_expand_cb;
} dhcp_client_core_init;

/* The core configuration structure contains the information provided to the
 * core on how to connect to other DHCP client layers.  This information is
 * provided by the start function call.
 */
typedef struct {
  /* Client request send */
  boolean (*client_request_new)(void * handle, void ** msg, uint32 size);
  boolean (*client_request_pushdown_tail)(void * handle, void ** msg,
                                          void * data, uint32 size);
  void (*client_request_send)(void * handle, void ** msg, uint32 ip_addr );
  void (*client_request_free)(void * handle, void ** msg);
  void * client_request_handle;

  /* Configuration Layer */
  void (*configuration_test)(void * handle,
                             dhcp_client_provision * provision);
  ps_iface_ioctl_extended_ip_config_ex_type (*configuration_set)(void * handle,
                            dhcp_client_provision * provision);
  void (*configuration_clear)(void * handle);
  void (*configuration_event_ind)(void * handle,
                                  ps_iface_ioctl_extended_ip_config_ex_type config_change_mask);
  void * configuration_handle;

  /* Persistant storage */
  boolean (*storage_get_lease)(void * handle, dhcp_client_lease * lease);
  void (*storage_set_lease)(void * handle, dhcp_client_lease * lease );
  void * storage_handle;

  /* Timer services */
  void (*timer_start)( void * handle, uint32 delta,
                       dhcp_client_timer_mode mode);
  void (*timer_stop)( void * handle );
  void * timer_handle;
} dhcp_client_core_config;

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  In addition to the data structures above, there will be a mechanism
  to configure various parameters, such as timeouts and number of
  retries at various points in the state machine.

  The functions provided by the core are as follows.  All of these
  functions should only be called by the DHCP client manager.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_NEW

Description:
  This function initializes the core with internal data and returns a handle
  to a core object.

Arguements:
  dhcp_client_core_init * init - the information (see structure above) needed
    to initialize the core.

Return value:
  void * - A handle to the new core object, or NULL on failure.

Dependencies:
  This function must be called to get a handle before any other core functions
  are called.  The handle must be started (see dhcp_clienti_core_start() below)
  before the core is fully functional.
===========================================================================*/
void *
dhcp_clienti_core_new
(
  dhcp_client_core_init * init
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_START

Description:
  This function configures the core with the information on how to interact
  with the other dhcp client modules.

Arguements:
  void * handle - Handle to the core object to start.
  dhcp_client_core_config * config - The configuartion information needed to
    interface with other modules in the DHCP client architecture.

Return value:
  Boolean - TRUE if successful, otherwise FALSE.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new() above.
===========================================================================*/
boolean
dhcp_clienti_core_start
(
  void * handle,
  dhcp_client_core_config * config
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_STOP

Description:
  This function stops a DHCP client core, including timers and frees the
  associated memory.  Additionally, the handle pointer will be NULL'd.  Note:
  This function does not release a lease.  See dhcp_clienti_core_release below.

Arguements:
  void ** handle - The handle to the core to release.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new().
===========================================================================*/
void
dhcp_clienti_core_stop
(
  void ** handle
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_RENEW

Description:
  This function causes a the DHCP client core to start a renew sequence.

Arguements:
  void * handle - handle to DHCP Client core.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new() and started
  by dhcp_clienti_core_start().
===========================================================================*/
void
dhcp_clienti_core_renew
(
  void * handle
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_RELEASE

Description:
  This function causes the DHCP client core to release the lease it has. If
  there is no current lease, nothing will happen.

Arguements:
  void * handle - handle to DHCP Client core.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new() and started
  by dhcp_clienti_core_start().
===========================================================================*/
void
dhcp_clienti_core_release
(
  void * handle
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_REPLY

Description:
  This function is used to send messages from servers to the core for
  processing.  Parsing of the message and event processing will happen before
  this function returns.  The memory associated with the incoming data may be
  freed on return of this function.

Arguements:
  void * handle - handle to DHCP Client core.
  void * msg - a handle to the incoming messsage.
  boolean (*extract) - The extract function to extract data from the message
    handle. Arguements as follows:
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
  The handle must have been allocated by dhcp_clienti_core_new() and started
  by dhcp_clienti_core_start().
===========================================================================*/
void
dhcp_clienti_core_reply
(
  void * handle,
  void * msg,
  boolean (*extract)( void * dest, void * src, uint32 offset, uint32 len)
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_DECLINE

Description:
  This function informs the core that a proposed (or in use) lease has been
  detected to be bad.  This function is called from the configuration layer.

  This function will cause the lease to be declined back to the server, and if
  in use, will cause the network interface to be cleared.

Arguements:
  void * handle - handle to DHCP Client core.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new() and started
  by dhcp_clienti_core_start().
===========================================================================*/
void
dhcp_clienti_core_decline
(
  void * handle
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_TIMER_EXPIRE

Description:
  This function informs the core that the timer has expired.

Arguements:
  void * handle - handle to DHCP Client core.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new() and started
  by dhcp_clienti_core_start().
===========================================================================*/
void
dhcp_clienti_core_timer_expire
(
  void * handle
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_INFORM

Description:
  This function sends an INFORM event to the core.  If the core is in
  an acceptable state, the INFORM event will cause an inform message
  which will configure the stacks without obtaining an IP lease.

Arguements:
  void * handle - handle to DHCP Client core.
  uint32 inform_addr - the IP address we have been provisioned with, in
                       network byte order.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new() and started
  by dhcp_clienti_core_start().
===========================================================================*/
void
dhcp_clienti_core_inform
(
  void * handle,
  uint32 inform_addr
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_IS_DHCP_CORE_IN_PROGRESS

Description:
  This function returns whether the DHCP client CORE is running.

Arguements:
  void * handle - handle to DHCP Client core.

Return value:
  TRUE if client is running, FALSE otherwise.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new() and started
  by dhcp_clienti_core_start().
===========================================================================*/
boolean
dhcp_clienti_core_is_dhcp_core_in_progress
(
  void * handle
);

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  The following types of events can occur:
   EVENT    CAUSE
  init      dhcp_clienti_core_new()
  start     dhcp_clienti_core_start()
  renew     dhcp_clienti_core_renew(), timeout
  release   dhcp_clienti_core_release(), timeout
  reply     dhcp_clienti_core_reply()
  decline   dhcp_clienti_core_decline()
  timer     dhcp_clienti_core_timer_expire()
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
CLIENT LAYER

  The client layer interfaces with the DSS socket layer for sending
  and recieving messages.

  This layer wil use dss_open_netlib() to hold the iface open while
  the DHCP client is "started".  This keeps the iface from forgetting
  the configuration information.

  The following are the data structures that the API will use.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* The init structure contains the information that is supplied by the init
 * call.
 */
typedef struct {
  dss_iface_id_type iface;
  void * (*d_malloc)(uint32 size);
  void (*d_free)(void *);
} dhcp_client_client_init;

/* The config structure contains information that is supplied on start.  The
 * information describes how to interface with the other modules in the DHCP
 * client.
 */
typedef struct {
  /* Send messages */
  void (* reply)
  (
    void * handle,
    void * msg,
    boolean (*extract)( void * dest, void * src, uint32 offset, uint32 len)
  );
  void * reply_handle;
} dhcp_client_client_config;

/*===========================================================================
FUNCTION DHCP_CLIENTI_CLIENT_NEW

Description:
  This function initializes the client layer with internal data and returns a
  handle to a core object.

Arguements:
  dhcp_client_client_init * init - the information (see structure above) needed
    to initialize the client layer.

Return value:
  void * - A handle to the new client object, or NULL on failure.

Dependencies:
  This function must be called to get a handle before any other client
  functions are called.  The handle must be started (see
  dhcp_clienti_client_start() below) before the core is fully functional.  The
  callback must be called from this module before the start is called.
===========================================================================*/
void *
dhcp_clienti_client_new
(
  dhcp_client_client_init * init
);

/*==========================================================================
FUNCTION DHCP_CLIENTI_CLIENT_START

Description:
  This funtion configures the client with the information on how to interact
  with other dhcp client modules.

Arguements:
  void * handle - Handle to the client object to start.
  dhcp_client_client_config * config - The configuartion information needed to
    interface with other modules in the DHCP client architecture.

Return value:
  boolean - TRUE if successful, otherwise FALSE.

Dependencies:
  The handle must have been allocated by dhcp_clienti_client_new() above.
===========================================================================*/
boolean
dhcp_clienti_client_start
(
  void * handle,
  dhcp_client_client_config * config
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CLIENT_STOP

Description:
  This function initiates the stop of the DHCP client layer and frees the
  associated memory.  Additionally, the handle pointer will be NULL'd.

Arguements:
  void ** handle - The handle to the client layer to release.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_client_new() above.
===========================================================================*/
void
dhcp_clienti_client_stop
(
  void ** handle
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CLIENT_REQUEST_NEW

Description:
  This function allocates a new request object.  This object is used to build
  a request to be sent out through the client layer.  Data is added to the
  object using the dhcp_clienti_client_request_pushdown_tail function, and sent
  using the dhcp_clienti_client_request_send function which will also free the
  object.  If the send should be aborted after the object has been allocated,
  then the dhcp_clienti_client_request_free function should be used.

Arguements:
  void * handle - Handle to the client layer object allocated by
    dhcp_clienti_client_new().
  void ** msg - The request object to be allocated.
  uint32 size - The size of the message to be sent.

Return value:
  boolean - TRUE if msg allocated correctly, otherwise FALSE.
  void ** msg - The allocated object.

Dependencies:
  The client layer must have been properly started using
  dhcp_clienti_client_start().
===========================================================================*/
boolean
dhcp_clienti_client_request_new
(
  void * handle,
  void ** msg,
  uint32 size
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CLIENT_REQUEST_PUSHDOWN_TAIL

Description:
  This function adds data to the end of a request object.

Arguements:
  void * handle - The client layer handle.
  void ** msg - The request object to add data to.
  void * data - The data to add
  uint32 size - The amount of data to add.

Return value:
  boolean - TRUE if data added correctly, otherwise FALSE.

Dependencies:
  The client layer must have been properly started using
  dhcp_clienti_client_start().
  The msg must have been allocated using the dhcp_clienti_client_request_new()
  function above.
===========================================================================*/
boolean
dhcp_clienti_client_request_pushdown_tail
(
  void * handle,
  void ** msg,
  void * data,
  uint32 size
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CLIENT_REQUEST_SEND

Description:
  This function sends the data collected in the request object out the
  configured interface.

Arguements:
  void * handle - The client layer handle.
  void ** msg - The request object to send.
  uint32 ip_addr - The IPv4 address to which to send the data.  This may be
    the broadcast address.

Return value:
  None.  The msg handle will be freed by the client layer after the message
  has been sent.

Dependencies:
  The client layer must have been properly started using
  dhcp_clienti_client_start().
  The msg must have been allocated using the dhcp_clienti_client_request_new()
  function above.
===========================================================================*/
void
dhcp_clienti_client_request_send
(
  void * handle,
  void ** msg,
  uint32 ip_addr
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CLIENT_REQUEST_FREE

Description:
  This function frees the allocated request object.  This function should only
  be called if the object is NOT to be sent with the
  dhcp_client_cleint_request_send() function above.

Arguements:
  void * handle - The client layer handle.
  void ** msg - The request object to send.

Return value:
  None. The msg handle will be freed.

Dependencies:
  The client layer must have been properly started using
  dhcp_clienti_client_start().
  The msg must have been allocated using the dhcp_clienti_client_request_new()
  function above.
===========================================================================*/
void
dhcp_clienti_client_request_free
(
  void * handle,
  void ** msg
);

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
CONFIGURATION LAYER

  The configuration layer configures the network interface with the
  appropiate information as the DHCP server provides.

  The following are the data structures that the API will use.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

typedef struct {
  dhcp_client_hw_type_enum htype; /* For duplicate IP detection scheme */

  /* The following params determine if the client request these
   * options in the Parameter Request List Option.
   * If the options are not requested, they are subsequently
   * not set in the ps_iface even if the DHCP server responds
   * with the relevant configuration info.
   * Suggested value for these options is TRUE. */
  boolean domain_name_server_option; /* RFC 2132, 3.8 */
  boolean subnet_mask_option;        /* RFC 2132, 3.3 */
  boolean router_option;             /* RFC 2132, 3.5 */
  boolean interface_mtu_option;      /* RFC 2132, 5.1 */
  boolean domain_name_option;        /* RFC 2132, 3.17 */
  boolean sip_server_option;         /* RFC 3361 */
} dhcp_client_configuration_params;

/* The init structure contains the information that is supplied by the init
 * call.
 */
typedef struct {
  dss_iface_id_type iface;
  dhcp_client_configuration_params
    config_params; /* for parameter request flags and hw_type */
  void * (*d_malloc)(uint32 size);
  void (*d_free)(void *);
  void (*status_cb)( void * userdata, dhcp_client_status_enum status);
  void * userdata;
} dhcp_client_configuration_init;

/* The config structure contains information that is supplied on start.  The
 * information describes how to interface with the other modules in the DHCP
 * client.
 */
typedef struct {
  void (*decline)(void * dhcp_decline_handle);
  void * decline_handle;
} dhcp_client_configuration_config;

/*===========================================================================
FUNCTION DHCP_CLIENTI_CONFIGURATION_NEW

Description:
  This function initializes the configuration layer with internal data and
  returns a handle to the core object.

Arguements:
  dhcp_client_configuration_init * init - The information needed to
    initialize the configuration layer.

Return value:
  void * - A handle to the new configuration object, or NULL on failure.

Dependencies:
  This function must be called to get a handle before any other configuration
  functions are called.  The handle must be started (see
  dhcp_clienti_configuration_start() below) before the configuration layer is
  fully functional.
===========================================================================*/
void *
dhcp_clienti_configuration_new
(
   dhcp_client_configuration_init * init
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CONFIGURATION_START

Description:
  This function configures the configuration layer with the information on how
  to interact with the other client modules.

Arguements:
  void * handle - Handle to the configuration object to start.
  dhcp_client_configuration_config * config - The configuration information
  needed to interface with other modules in the DHCP client architecture.
  void * dhcp_handle - DHCP handle to set in the ps_iface layer.

Return value:
  Boolean - TRUE if success, otherwise FALSE.

Dependencies:
  The handle must be have been allocated by dhcp_clienti_configuration_new()
  above.
===========================================================================*/
boolean
dhcp_clienti_configuration_start
(
  void * handle,
  dhcp_client_configuration_config * config,
  void * dhcp_handle
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CONFIGURATION_STOP

Description:
  This function stops a DHCP client configuration layer object and frees the
  associated memory.  Additionally, the handle pointer will be NULL'd.

Arguements:
  void ** handle - The handle to the configuration layer object to release.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_configuration_new().
===========================================================================*/
void
dhcp_clienti_configuration_stop
(
  void ** handle
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CONFIGURATION_TEST

Description:
  This function configures the ARP (or other) layer with the proposed lease to
  verify that the lease is valid.

  If the ARP (or other) layer finds that the lease is not valid, then it
  should promptly call the registered callback which will call back into this
  layer.  This layer will then inform the core about the bad lease.

  If there is nothing wrong with the lease, then no action need be taken.

Arguements:
  void * handle - Handle to the configuration layer object.
  dhcp_client_provision * provision - The lease information needed to
    configure the interface

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_configuration_new().
  The handle must have been started by dhcp_clienti_configuration_start().
===========================================================================*/
void
dhcp_clienti_configuration_test
(
  void * handle,
  dhcp_client_provision * provision
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CONFIGURATION_SET

Description:
  This function configures the network interface associated with the
  configuration layer handle.

Arguements:
  void * handle - Handle to the configuration layer object.
  dhcp_client_provision * provision - The lease information needed to
    configure the interface

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_configuration_new().
  The handle must have been started by dhcp_clienti_configuration_start().
===========================================================================*/
ps_iface_ioctl_extended_ip_config_ex_type
dhcp_clienti_configuration_set
(
  void * handle,
  dhcp_client_provision * provision
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CONFIGURATION_CLEAR

Description:
  This function unconfigures (or clears) the network interface associated with
  the configuration layer handle.

Arguements:
  void * handle - Handle to the configuration layer object.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_configuration_new().
  The handle must have been started by dhcp_clienti_configuration_start().
===========================================================================*/
void
dhcp_clienti_configuration_clear
(
  void * handle
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_CONFIGURATION_EVENT_IND

Description:
  This function issues an IFACE_EXTENDED_IP_CONFIG event indication
  on the interface associated with the configuration layer handle.

Arguments:
  void    * handle   - Handle to the configuration layer object.
  config_change_mask - Mask indicating updated config items.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_configuration_new().
  The handle must have been started by dhcp_clienti_configuration_start().
===========================================================================*/
void
dhcp_clienti_configuration_event_ind
(
  void                                      *handle,
  ps_iface_ioctl_extended_ip_config_ex_type  config_change_mask
);

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
PERSISTANT STORAGE LAYER

  The persistant storage layer stores data that is needed to jump
  start the boot state of the DHCP clien state machine which saves the
  discover phase.  This data may be stored in the EFS.  If the data is
  not available, the DHCP client state machine must go through the
  discover phase.

  The data will be stored using the interface (as appears in the init
  structure) as the key.

  The following are the data structures that the API will use.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* The init structure contains the information that is supplied by the init
 * call.
 */
typedef struct {
  dss_iface_id_type iface;
  void * (*d_malloc)(uint32 size);
  void (*d_free)(void *);
} dhcp_client_storage_init;

/*===========================================================================
FUNCTION DHCP_CLIENTI_STORAGE_NEW

Description:
  This function allocates a new storage object and initializes it with the
  internal data.  A handle to this object is returned.

Arguements:
  dhcp_client_storage_init * init - The information needed to initialize the
    storage layer.

Return value:
  void * - The handle to the storage layer object.  NULL on failure.

Dependencies:
  This function must be called to get a handle before any other storage layer
  functions are called.  The handle must be started (see
  dhcp_clienti_storage_start() below) before the storage layer is fully
  functional.
===========================================================================*/
void *
dhcp_clienti_storage_new
(
  dhcp_client_storage_init * init
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_STORAGE_START

Description:
  This function configures the storage layer with the information on how to
  interact with other DHCP client modules. [Currently, this module is a leaf
  module and doesn't call into other DHCP client modules.  This function is
  for symmetry and future interactions.]

Arguements:
  void * handle - Handle to the storage layer object to start.
  to interface with other modules in the DHCP client architecture.

Return value:
  Boolean - TRUE if successful, otherwise FALSE.

Dependencies:
===========================================================================*/
boolean
dhcp_clienti_storage_start
(
  void * handle
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_STORAGE_STOP

Description:
  This function stops a DHCP storage layer object and frees the associated
  memory.  Additionally, the handle pointer will be NULL'd.

Arguements:
  void ** handle - The handle to the storage layer object to release.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_storage_new().
===========================================================================*/
void
dhcp_clienti_storage_stop
(
  void ** handle
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_STORAGE_GET_LEASE

Description:
  This function reads the current lease information for the given handle and
  returns it.   Lease information is stored by dss_iface_id_type.

Arguements:
  void * handle - Handle to the storage object.
  dhcp_client_lease * lease - The lease to fill in and return.

Return value:
  boolean - TRUE if lease found, valid and returned, FALSE otherwise.
  dhcp_client_lease * lease - Filled out lease info if found and valid.

Dependencies:
  The storage handle must have been allocated by dhcp_clienti_storage_new() and
  started by dhcp_clienti_storage_start().
  The lease pointer must point to valid memory to be filled in.
===========================================================================*/
boolean
dhcp_clienti_storage_get_lease
(
  void * handle,
  dhcp_client_lease * lease
);

/*===========================================================================
FUNCTION DHCP_CLIENTI_STORAGE_SET_LEASE

Description:
  This function sets the lease for the given handle.  This function can also
  be used to mark a lease invalid by passing the lease pointer as NULL.
  Lease information is stored by dss_iface_id_type.

Arguements:
  void * handle - Handle to the storage object.
  dhcp_client_lease * lease - The lease to store.

Return value:
  None.

Dependencies:
  The storage handle must have been allocated by dhcp_clienti_storage_new() and
  started by dhcp_clienti_storage_start().
===========================================================================*/
void
dhcp_clienti_storage_set_lease
(
  void * handle,
  dhcp_client_lease * lease
);

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
REQUIRED OPTIONS

  The following options are required to configure our network stack:
  - IP Address lease time option (51)
  - Server ID option (54)
  - Domain name server option (6).
  - Subnet mask option (1).
  - Router option (3).
  - MTU (26).

DEVELOPMENT AND TESTING

  Development will proceed similar to the DCHP server.  An ONCRPC server
  running on a unix/linux system will be used to test the core
  functionality of most of the DHCP client.  The ONCRPC server will take
  the place of the DHCP manager layer, although an STA manager layer may also
  be envisioned.

  On target, the STA module will allow testing in situ of a subset of
  the layers.   Of particular interest to test is the core layer, as
  most of the complexity lies there.

STACK QUESTIONS

  Can the network stack send and receive UDP before IP address?
    - In theory, as a system application, this can be done.

  Can the network stack accept unicast packet to the hw address?
    - In theory, this will work.

  We need to have an API to get the "hardware address" for the interface.
    - This will be added.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#endif /* DHCP_CLIENTI_H */
