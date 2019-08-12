#ifndef DHCP6_SERVER_AMI_H
#define DHCP6_SERVER_AMI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         D H C P 6 _ S E R V E R _ A M I _ . H

GENERAL DESCRIPTION
  AMSS IPv6 Dynamic Host Configuration Protocol Address
  Manager.  This header file has the public functions of the dhcp6
  address manager.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Initializations for Address manager:
  - dhcp6_server_am_new must be called first to initialize a handle
    with the internal configuration.  
  - dhcp6_server_am_start must be called second to configure the
    handle with external interdependancies. 

  Cleanup for any mp address manager:
  - dhcp6_server_am_stop must be called to free the handle when the
    session is finished.  This will clean up the lower layer AM. 

  All external functions of this file must be called from the same
  context. 

Copyright (c) 2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/dhcp6_server_ami.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/09/11    cp    Initial development work done.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_DATA_PS_IPV6
#include "ps_in.h"
#include "dhcp6_server_mgri.h"


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION DHCP6_SERVER_AM_NEW()

DESCRIPTION
  This function initializes a dhcp6_server_am handle. 

DEPENDENCIES
  The handle must be "started" after the other subsystems have been created.
  The handle should be freed when it is no longer needed. 

PARAMETERS
  dhcp6_am_init * - Internal configuration information.This includes 
                   DHCP6 configuration parameters like DNS, SIP addresses etc.,
RETURN VALUE
  handle for calling am functions. NULL on failure.

SIDE EFFECTS
None
===========================================================================*/
void *
dhcp6_server_am_new
(
  dhcp6_am_init * init
);

/*===========================================================================
FUNCTION DHCP6_SERVER_AM_START()

DESCRIPTION
  This function registers the functions that interface the DHCP6 Address
  Manager with other DHCP6 systems.

DEPENDENCIES
  Handle must have been allocated by dhcp6_server_am_new

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.

RETURN VALUE
  boolean - Success/TRUE or failure/FALSE.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp6_server_am_start
(
  void * handle
);

/*===========================================================================
FUNCTION DHCP6_SERVER_AM_STOP()

DESCRIPTION
  This function unregisters the functions that interface the DHCP6
  Address  Manager.

DEPENDENCIES
  Handle must have been previously allocated and 
  dhcp6_server_am_start must have been called.


PARAMETERS
  void ** - Handle to the glue needed to get at the connection details.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void
dhcp6_server_am_stop
(
  void ** handle
);

/*===========================================================================
FUNCTION DHCP6_SERVER_AM_DNS_ADDR_LIST()

DESCRIPTION
  This function fetches the IPv6 DNS server information (if available) and fills
  out the dhcp6_core_msg_info with these addresses.

DEPENDENCIES
  Handle must have been previously allocated and started.
  dhcp6_server_am_start must have been called.


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
dhcp6_server_am_dns_addr_list
(
void * handle,
ip_addr_type * dns_addr_list,
uint32 * count
);

/*===========================================================================
FUNCTION DHCP6_SERVER_AM_SIP_ADDR_LIST()

DESCRIPTION
  This function fetches the IPv6 SIP server information (if available) and fills
  out the dhcp6_core_msg_info with these addresses.

DEPENDENCIES
  Handle must have been previously allocated and started.
  dhcp6_server_am_start must have been called.


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
dhcp6_server_am_sip_addr_list
(
void * handle,
ip_addr_type * sip_addr_list,
uint32 * count
);

/*===========================================================================
FUNCTION DHCP6_SERVER_AM_DNS_DOMAIN_NAME_LIST()

DESCRIPTION
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
dhcp6_server_am_dns_domain_name_list
(
void * handle,
ps_iface_domain_name_type  *dns_domain_name_list,
uint32 * count
);

/*===========================================================================
FUNCTION DHCP6_SERVER_AM_SIP_DOMAIN_NAME_LIST()

DESCRIPTION
  This function fetches the IPv6 SIP server domain name information (if available) and fills
  out the dhcp6_core_msg_info with these names.

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
dhcp6_server_am_sip_domain_name_list
(
void * handle,
ps_iface_domain_name_type  *sip_domain_name_list,
uint32 * count
);

/*===========================================================================
FUNCTION DHCP6_SERVER_AM_SERVER_DUID()

DESCRIPTION
  This function fetches the DUID of the server (us).

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  uint8 * - pointer to the server_duid buffer.
  uint32 - length of the server_duid buffer.

RETURN VALUE
  None.
SIDE EFFECTS
  None
===========================================================================*/
void
dhcp6_server_am_server_duid
(
  void * handle,
  uint8 * server_duid,
  uint32 * server_duid_len
);
#endif /* FEATURE_DATA_PS_IPV6 */
#endif /* DHCP6_SERVER_AMI_H */

