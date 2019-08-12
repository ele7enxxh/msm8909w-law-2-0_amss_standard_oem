#ifndef DHCPI_AM_PS_IFACE_H
#define DHCPI_AM_PS_IFACE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         D H C P I _ A M _ P S _ I F A C E . H

GENERAL DESCRIPTION
  DMSS Dynamic Host Configuration Protocol PS Interface Address
  Manager.  This header file has the public functions of the ps iface
  address manager.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Initializations for single IP address, PS address manager:
  - dhcp_am_ps_single_new must be called first to initialize a handle
    with the internal configuration.  
  - dhcp_am_ps_single_start must be called second to configure the
    handle with external interdependancies. 

  Cleanup for any mp address manager:
  - dhcp_am_ps_single_stop must be called to free the handle when the
    session is finished.  This will clean up the lower layer AM. 

  All external functions of this file must be called from the same
  context. 

Copyright (c) 2004-2014 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dhcpi_am_ps_iface.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/03/14    mp     Added the support for SIP server option.
10/12/11    am     Added get_ip_cfg to fetch IP from stored lease.
08/29/11    ag     Added the support for NBNS option.
05/03/10    cp     Soft AP changes.
02/16/10    cp     Changes to propagate the UM iface mtu value to TE.
12/17/08    pp     CMI updates.
04/13/04    clp    Initial development work done.  Added comments.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "ps_in.h"
#include "dhcpi.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_NEW()

DESCRIPTION
  This function initializes a dhcp_am handle.

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
dhcp_am_ps_single_new
( 
  dhcp_am_init * init
);

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_START()

DESCRIPTION
  This function registers the functions that interface the DHCP Address
  Manager with other DHCP systems.  This function also calculates the default
  route ip address and netmask that should be provided to clients, and this is
  provided back to the client interface via the provided function in the
  dhcp_am_config structure. 

DEPENDENCIES
  Handle must have been allocated by dhcp_am_ps_single_new.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  dhcp_am_config * - pointer to functions used to connect to other
                     systems.

RETURN VALUE
  boolean - Success/TRUE or failure/FALSE.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_ps_single_start
( 
  void * handle, dhcp_am_config * config
);

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_STOP()

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
dhcp_am_ps_single_stop
( 
  void ** handle 
);

/*===========================================================================
LOCAL FUNCTION DHCP_AM_PS_SINGLE_GET_LEASE()

DESCRIPTION
  This function allocates a lease based on the information that is
  passed in and the state of the lease.  Calls local function
  _alloc_lease() if lease needs to be allocated.

DEPENDENCIES
  dhcp_am_ps_single_start must have been called with the handle.

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
dhcp_am_ps_single_get_lease
( 
  void * handle,
  dhcp_core_msg_info * core
);

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_FREE_LEASE()

DESCRIPTION
  This function frees a previously allocated lease.  This function can be
  called either on a release from a client, or is a previously allocated lease
  is not actually sent out due to other errors.

DEPENDENCIES
  dhcp_am_ps_single_start must have been called with the handle.

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
dhcp_am_ps_single_free_lease
(
  void * handle, 
  dhcp_core_msg_info * core
);

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_ERR_LEASE()

DESCRIPTION
  This function informs the DHCP Address Manager that a lease that was
  previously given out had a problem.  This function is called on a
  DHCPDECLINE message from the client.

DEPENDENCIES
  dhcp_am_ps_single_start must have been called with the handle.

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
dhcp_am_ps_single_err_lease
(
  void * handle, 
  dhcp_core_msg_info * core
);

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_DNS_FIRST()

DESCRIPTION
  This function fetches the DNS server information (if available) and
  fills out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  dhcp_am_ps_single_start must have been called with the handle.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  void ** - This is a cookie that will be returned to get to the next
            DNS address.
  uint32 * - The first dns address.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - no more addresses.
  void ** - cookie to get to next address.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_ps_single_dns_first
( 
  void * handle,
  void ** cookie,
  uint32 * ip_addr
);

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_DNS_NEXT()

DESCRIPTION
  This function fetches the DNS server information (if available) and fills
  out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  dhcp_am_ps_single_start must have been called with the handle.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  void ** - This is a cookie that will be returned to get to the next
            DNS address.
  uint32 * - The next dns address.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - no more addresses.
  void ** - cookie to get to next address.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_ps_single_dns_next
( 
  void * handle,
  void ** cookie,
  uint32 * ip_addr
);


/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_NBNS_FIRST()

DESCRIPTION
  This function fetches the NBNS server information (if available) and
  fills out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  dhcp_am_ps_single_start must have been called with the handle.

PARAMETERS
  handle  - Handle to the glue needed to get at the connection
            details.
  cookie  - This is a cookie that will be returned to get to the next
            NBNS address.
  ip_addr - The first NBNS address.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - no more addresses.
  cookie  - cookie to get to next address.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_ps_single_nbns_first
( 
  void * handle,
  void ** cookie,
  uint32 * ip_addr
);


/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_NBNS_NEXT()

DESCRIPTION
  This function fetches the NBNS server information (if available) and
  fills out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  dhcp_am_ps_single_start must have been called with the handle.

PARAMETERS
  handle  - Handle to the glue needed to get at the connection
            details.
  cookie  - This is a cookie that will be returned to get to the next
            NBNS address.
  ip_addr - The next NBNS address.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - no more addresses.
  cookie  - cookie to get to next address.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_ps_single_nbns_next
( 
  void * handle,
  void ** cookie,
  uint32 * ip_addr
);


/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_ADDR_INFO_SERVER_ID()

DESCRIPTION
  This function fetches the IP address of the server (us).

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.

RETURN VALUE
  uint32 - The IPV4 address of the server.

SIDE EFFECTS
  None
===========================================================================*/
uint32
dhcp_am_ps_single_addr_info_server_id
( 
  void * handle
);

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_MTU()

DESCRIPTION
  This function fetches the MTU of the server (us).

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.

RETURN VALUE
  uint16 - The MTU of the server.

SIDE EFFECTS
  None
===========================================================================*/
uint32
dhcp_am_ps_single_mtu
( 
  void * handle
);

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_ADDR_INFO_NETMASK()

DESCRIPTION
  This function fetches the netmask of the subnet the server is on.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.

RETURN VALUE
  uint32 - The IPV4 netmask of the subnet we are on.

SIDE EFFECTS
  None
===========================================================================*/
uint32
dhcp_am_ps_single_addr_info_netmask
( 
  void * handle
);


/*===========================================================================
FUNCTION DHCP_AM_PS_IFACE_GET_IP_CONFIG()

DESCRIPTION
  This function fetches the IP config for this AM.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  3*uint32 * - IP params

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dhcp_am_ps_iface_get_ip_config
(
  void   * handle,
  uint32 * client_ip,
  uint32 * server_id,
  uint32 * netmask
);

/*===========================================================================
FUNCTION DHCP_AM_PS_IFACE_SIP_ADDR_LIST()

DESCRIPTION
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
dhcp_am_ps_iface_sip_addr_list
(
  void * handle,
  ip_addr_type * sip_addr_list,
  uint32 * count
);


/*===========================================================================
FUNCTION DHCP_AM_PS_IFACE_SIP_DOMAIN_NAME_LIST()

DESCRIPTION
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
dhcp_am_ps_iface_sip_domain_name_list
(
  void * handle,
  ps_iface_domain_name_type  *sip_domain_name_list,
  uint32 * count
);


#endif /* DHCPI_AM_PS_IFACE_H */
