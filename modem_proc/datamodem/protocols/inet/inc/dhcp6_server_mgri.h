#ifndef DHCP6_SERVER_MGRI_H
#define DHCP6_SERVER_MGRI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      D H C P 6 _ S E R V E R _ M G R I . H

GENERAL DESCRIPTION
  THIS IS A DHCPv6 SERVER INTERNAL HEADER FILE.  NO SYSTEM OTHER THAN
  DHCP6 SERVER SHOULD INCLUDE THIS!  See dhcp6_sl_server_mgr.h instead.

  This file describes the API's between each of the modules that make
  up the DHCP6 SERVER.  These modules include the manager, the core, the
  client shim, configuration shim.


Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/dhcp6_server_mgri.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/09/11    cp     Initial development work.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_DATA_PS_IPV6

#include "dhcp6_sl_server_mgr.h"
#include "dhcp6_common_defs.h"
#include "ps_in.h"
#include "ps_iface.h"
#include "ps_iface_defs.h"
#include "dssocket_defs.h"

/*=========================================================================
INIT STRUCTS

  The init structures contain information provided to the modules (core,
client, config) upon initialization.
=========================================================================*/
/* Init struct for the core module */
typedef struct dhcp6_core_init {
  void * (*d_malloc)(uint32 size);
  void (*d_free)(void *);
} dhcp6_core_init;

/* Init struct for the client module */
typedef struct dhcp6_server_init {
  void * (*d_malloc)(uint32 size);
  void (*d_free)(void *);
  ps_iface_type * iface;
  dss_iface_id_type iface_id;
} dhcp6_server_init;

/*=========================================================================
CONFIG STRUCTS

  The config structures contain information to the modules (core, client,
config) on how to connect to the other DHCPv6 server layers. The modules
are provided this information by their start function calls.
=========================================================================*/

/* Config structure for the core module */
typedef struct {
  /* Server Layer */
  boolean (*server_reply_new)(void * handle, void ** msg, uint32 size);
  boolean (*server_reply_pushdown_tail)(void * handle, void ** msg,
                                          void * data, uint32 size);
  void (*server_reply_send)(void * handle, void ** msg,
         struct ps_in6_addr ip_addr );
  void (*server_reply_free)(void * handle, void ** msg);
  void * server_reply_handle;

  void           (*dns_addr_list)(void *, ip_addr_type *, uint32 *);
  void * dns_addr_handle;

  void           (*dns_domain_name_list)(void *, ps_iface_domain_name_type *, uint32 *);
  void * dns_domain_name_handle;

  void           (*sip_addr_list)(void *, ip_addr_type *, uint32 *);
  void * sip_addr_handle;

  void           (*sip_domain_name_list)(void *, ps_iface_domain_name_type *, uint32 *);
  void * sip_domain_name_handle;

  void      (*server_duid)(void *,uint8 *, uint32 *);
  void * server_duid_handle;

} dhcp6_core_config;

/* Config structure for the client module */
typedef struct {
  /* Send messages to the core, see DHCP6_CORE_INPUT function
   * header for details */
  void (*recv_msg)
  ( 
    void * handle, 
    struct ps_in6_addr ps_sin6_addr,
    void * msg, 
    boolean (*extract)(void * dest, void * src, uint32 offset, uint32 len)
  );
  void * recv_msg_handle;
} dhcp6_server_config;

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
  ps_iface_type - Interface bound to.
  config_vals   - DHCP configuration parameters.
  ------------------------------------------------------------------- */
typedef struct {
  void *(*d_malloc)(uint32 size);
  void (*d_free)(void *);
  ps_iface_type * iface;
  dhcp6_sl_server_duid server_duid;
  dhcp6_sl_server_config_params * config_vals;
} dhcp6_am_init;

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
DHCP6 CORE LAYER

  The core layer takes care of sending messages and dispatching responses.

  The following are the data structures that the API will use.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
FUNCTION DHCP6_CORE_NEW

Description:
  This function initializes the core with internal data and returns a handle
  to a core object.

Arguments:
  dhcp6_core_init * init - the information (see structure above)
    needed to initialize the core.

Return value:
  void * - A handle to the new core object, or NULL on failure.

Dependencies:
  This function must be called to get a handle before any other core
  functions are called.  The handle must be started before the core is fully
  functional. (see dhcp6_core_start() below)
===========================================================================*/
void *
dhcp6_core_new
(
  dhcp6_core_init * init
);

/*===========================================================================
FUNCTION DHCP6_CORE_START

Description:
  This function configures the core with the information on how to interact
  with the other dhcp client modules.

Arguments:
  void * handle - Handle to the core object to start.
  dhcp6_core_config * config - The configuartion information needed to
    interface with other modules in the DHCP client architecture.

Return value:
  Boolean - TRUE if successful, otherwise FALSE.

Dependencies:
  The handle must have been allocated by dhcp6_core_new() above.
===========================================================================*/
boolean
dhcp6_core_start
(
  void * handle,
  dhcp6_core_config * config
);

/*===========================================================================
FUNCTION DHCP6_SERVERI_CORE_STOP

Description:
  This function stops a DHCP server core, including timers and frees the
  associated memory.  Additionally, the handle pointer will be NULL'd.
  Note: This function does not release a lease.

Arguments:
  void ** handle - The handle to the core to release.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp6_core_new().
===========================================================================*/
void
dhcp6_core_stop
(
  void ** handle
);

/*===========================================================================
FUNCTION DHCP6_CORE_INPUT

Description:
  This function is used to send messages from servers to the core for
  processing.  Parsing of the message and event processing will happen before
  this function returns.  The memory associated with the incoming data may be
  freed on return of this function.

Arguments:
  void * handle - handle to DHCP Server core.
  struct ps_in6_addr - IPv6 address from where the packet came in.
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
  The handle must have been allocated by dhcp6_core_new() and started
  by dhcp6_core_start().
===========================================================================*/
void
dhcp6_core_input
(
  void * handle,
  struct ps_in6_addr from_addr,
  void * msg,
  boolean (*extract)( void * dest, void * src, uint32 offset, uint32 len)
);

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
DHCP6 AM LAYER

  The AM layer takes care of passing of the address configuration information
  to the CORE module.

  The following are the data structures that the API will use.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================
FUNCTION DHCP6_SERVER_AM_MP_NEW()

DESCRIPTION
  This function initializes a dhcp6_am handle Address Manager
  This works by keeping track of all existing address manager handles
  that have been allocated.  If we are reusing one, then pass that
  handle back. If not, then set up a new MP address manager that 
  points at the address manager.

DEPENDENCIES
  The handle must be "started" after the other subsystems have been
  created.  The handle should be freed when it is no longer needed.

PARAMETERS
  dhcp6_am_init * - Internal configuration information.  This includes
                   the interface to connect to.

RETURN VALUE
  handle for calling am functions. NULL on failure.

SIDE EFFECTS
  None
===========================================================================*/
void *
dhcp6_server_am_mp_new
(
  dhcp6_am_init * init
);
/*===========================================================================
FUNCTION DHCP6_AM_MP_START()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function registers the functions that interface the DHCP6 Address
  Manager with other DHCP6 systems.

DEPENDENCIES
  The handle must have been previously allocated using
  dhcp6_server_am_mp_new().
  Calls to this function must be serialized with the new function
  through ps task to protect against ref getting out of sync.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.

RETURN VALUE
  TRUE - if successful.
  FALSE - if there are problems.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp6_am_mp_start
(
  void * handle
);
/*===========================================================================
FUNCTION DHCP6_AM_MP_STOP()

DESCRIPTION
  This function unregisters the functions that interface the DHCP6 Address
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
dhcp6_am_mp_stop
(
  void ** handle
);
/*===========================================================================
FUNCTION DHCP6_AM_MP_DNS_ADDR_LIST()

DESCRIPTION
  This function passes through to the underlying backend address manager.
  This function fetches the IPv6 DNS server information (if available) and fills
  out the dhcp6_core_msg_info with these addresses.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  ip_addr_type * - pointer to the IPv6 DNS server addresses.
  uint32 * : as input value it contains the count of the DNS server
          addresses asked for and as a return value it contains the
          actual number of addresses returned.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void
dhcp6_am_mp_dns_addr_list
(
void * handle,
ip_addr_type * dns_addr_list,
uint32 * count
);
/*===========================================================================
FUNCTION DHCP6_AM_MP_SIP_ADDR_LIST()

DESCRIPTION
  This function passes through to the underlying backend address manager.
  This function fetches the IPv6 SIP server information (if available) and fills
  out the dhcp6_core_msg_info with these addresses.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  ip_addr_type * - pointer to the IPv6 SIP server addresses.
  uint32 * : as input value it contains the count of the SIP server
          addresses asked for and as a return value it contains the
          actual number of addresses returned.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void
dhcp6_am_mp_sip_addr_list
(
void * handle,
ip_addr_type * sip_addr_list,
uint32 * count
);
/*===========================================================================
FUNCTION DHCP6_AM_MP_DNS_DOMAIN_NAME_LIST()

DESCRIPTION
  This function passes through to the underlying backend address manager.
  This function fetches the IPv6 DNS server domain name information (if available) and fills
  out the dhcp6_core_msg_info with these names.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  dns_domain_name_list: pointer to an array of DNS domain names.
  uint32 * : as input value it contains the count of the DNS domain
          names asked for and as a return value it contains the
          actual number of domains names returned.


RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void
dhcp6_am_mp_dns_domain_name_list
(
void * handle,
ps_iface_domain_name_type  *dns_domain_name_list,
uint32 * count
);
/*===========================================================================
FUNCTION DHCP6_AM_MP_SIP_DOMAIN_NAME_LIST()

DESCRIPTION
  This function passes through to the underlying backend address manager.
  This function fetches the IPv6 SIP server domain name information (if available) and fills
  out the dhcp6_core_msg_info with these names.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  sip_domain_name_list: pointer to an array of SIP domain names.
  uint32 * : as input value it contains the count of the SIP domain
          names asked for and as a return value it contains the
          actual number of domains names returned.


RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void
dhcp6_am_mp_sip_domain_name_list
(
void * handle,
ps_iface_domain_name_type  *sip_domain_name_list,
uint32 * count
);
/*===========================================================================
FUNCTION DHCP6_AM_MP_SERVER_DUID()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the DUID of the server (us).

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  uint8 * - pointer to the server_duid buffer.
  uint32 * - length of the server_duid buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void
dhcp6_am_mp_server_duid
(
  void * handle,
  uint8 * server_duid,
  uint32 * server_duid_len
);

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Socket Interface

  The server layer interfaces with the DSS socket layer for sending
  and receiving messages.

  The following are the data structures that the API will use.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
FUNCTION DHCP6_SERVER_NEW

Description:
  This function initializes the client layer with internal data and returns a
  handle to a core object.

Arguments:
  dhcp6_server_init * init - the information (see structure above)
    needed to initialize the client layer.

Return value:
  void * - A handle to the new client object, or NULL on failure.

Dependencies:
  This function must be called to get a handle before any other client
  functions are called.  The handle must be started (see
  dhcp6_server_start() below) before the core is fully functional.
  The callback must be called from this module before the start is called.
===========================================================================*/
void *
dhcp6_server_new
(
  dhcp6_server_init * init
);

/*==========================================================================
FUNCTION DHCP6_SERVER_START

Description:
  This funtion configures the server with the information on how to interact
  with other dhcp server modules.

Arguments:
  void * handle - Handle to the client object to start.
  dhcp6_server_config * config - The configuartion information needed
    to interface with other modules in the DHCP client architecture.

Return value:
  boolean - TRUE if successful, otherwise FALSE.

Dependencies:
  The handle must have been allocated by dhcp6_server_new() above.
===========================================================================*/
boolean
dhcp6_server_start
(
  void * handle,
  dhcp6_server_config * config
);

/*===========================================================================
FUNCTION DHCP6_SERVER_STOP

Description:
  This function initiates the stop of the DHCP server layer and frees the
  associated memory.  Additionally, the handle pointer will be NULL'd.

Arguments:
  void ** handle - The handle to the client layer to release.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp6_server_new() above.
===========================================================================*/
void
dhcp6_server_stop
(
  void ** handle
);

/*===========================================================================
FUNCTION DHCP6_SERVER_REPLY_NEW

DESCRIPTION
  This function allocates a new reply object.  This object is used to build
  a reply to be sent out through the client layer.  Data is added to the
  object using the dhcp6_server_reply_pushdown_tail function, and sent
  using the dhcp6_server_reply_send function which will also free the
  object.  If the send should be aborted after the object has been allocated,
  then the dhcp6_server_reply_free function should be used.

PARAMETERS
  void * handle - Handle to the client layer object allocated by
    dhcp6_server_new().
  void ** msg - The reply object to be allocated.
  uint32 size - The size of the message to be sent.

RETURN VALUE
  boolean - TRUE if msg allocated correctly, otherwise FALSE.
  void ** msg - The allocated object.

DEPENDENCIES
  The client layer must have been properly started using
  dhcp6_server_start().
===========================================================================*/
boolean
dhcp6_server_reply_new
(
  void * handle,
  void ** msg,
  uint32 size
);

/*===========================================================================
FUNCTION DHCP6_SERVER_REPLY_APPEND

DESCRIPTION
  This function adds data to the end of a reply object.

PARAMETERS
  void * handle - The client layer handle.
  void ** msg - The reply object to add data to.
  void * data - The data to add
  uint32 size - The amount of data to add.

RETURN VALUE
  boolean - TRUE if data added correctly, otherwise FALSE.

DEPENDENCIES
  The client layer must have been properly started using
  dhcp6_server_start().
  The msg must have been allocated using the dhcp6_server_reply_new()
  function above.
===========================================================================*/
boolean
dhcp6_server_reply_pushdown_tail
(
  void * handle,
  void ** msg,
  void * data,
  uint32 size
);

/*===========================================================================
FUNCTION DHCP6_SERVER_REPLY_SEND

DESCRIPTION
  This function sends the data collected in the reply object out the
  configured interface.

PARAMETERS
  void * handle - The client layer handle.
  void ** msg - The reply object to send.
  struct ps_in6_addr ip_addr - The IPv6 address to which to send the data. This
  may be the broadcast address.

RETURN VALUE
  None.  The msg handle will be freed by the client layer after the message
  has been sent.

DEPENDENCIES
  The client layer must have been properly started using
  dhcp6_server_start().
  The msg must have been allocated using the dhcp6_server_reply_new()
  function above.
===========================================================================*/
void
dhcp6_server_reply_send
(
  void * handle,
  void ** msg,
  struct ps_in6_addr ip_addr
);

/*===========================================================================
FUNCTION DHCP6_SERVER_REPLY_FREE

DESCRIPTION
  This function frees the allocated reply object.  This function should only
  be called if the object is NOT to be sent with the
  dhcp6_server_reply_send() function above.

PARAMETERS
  void * handle - The client layer handle.
  void ** msg - The reply object to send.

RETURN VALUE
  None. The msg handle will be freed.

DEPENDENCIES
  The client layer must have been properly started using
  dhcp6_server_start().
  The msg must have been allocated using the dhcp6_server_reply_new()
  function above.
===========================================================================*/
void
dhcp6_server_reply_free
(
  void * handle,
  void ** msg
);

#endif /* FEATURE_DATA_PS_IPV6 */
#endif /* DHCP6_SERVER_MGRI_H */

