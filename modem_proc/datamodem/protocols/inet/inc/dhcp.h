#ifndef DHCP_H
#define DHCP_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           D H C P . H

GENERAL DESCRIPTION
  DMSS Dynamic Host Configuration Protocol public header file.
  This header file contains the public function to interface with the
  DHCP server.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  - The DCHP server is started by calling dhcp_start().  The DHCP
    server handle will be returned in the callback that is registered. 
  - The DHCP server is stoped by calling dhcp_stop(). 
  - The DHCP server must be restarted if the IP address from the PSDN
    changes. 
  - The DHCP server is restarted by calling dhcp_stop(), then
    dhcp_start().

Copyright (c) 2004-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/dhcp.h#1 $ 

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/12/11    am     dhcp_calc_gw_addr would now fetch IP from stored lease.
06/02/10    cp     IOCTL changes for Soft AP DHCP server.
05/03/10    cp     DHCP Soft AP changes.
10/15/07    ks     Exporting function to calculate gateway address and mask.
11/27/06    jd     Added DHCP message callbacks
04/15/04    clp    Initial development work done.  Added comments.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ps_iface.h"
#include "dhcp_server_mgr.h"

/*===========================================================================

                      DEFINITIONS AND DECLARATIONS

===========================================================================*/

/* -------------------------------------------------------------------
  Callback used to update the arp cache. 

  iface_ptr     - PS_IFACE for which an entry is been updated.
  set_or_clear  - boolean indicating whether to set or clear the arp cache.
  ipaddr        - IPv4 address for which the arp cache needs to be updated.
  hw_addr       - Hardware address for which the arp cache needs to be 
                  updated.
  ------------------------------------------------------------------- */
typedef void (* dhcp_arp_cache_updt_type) (ps_iface_type* iface_ptr,
                                           boolean        set_or_clear,
                                           uint32         ip_addr,
                                           byte*          hw_addr);

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION DHCP_START()

DESCRIPTION
  This function serializes the start of a DHCP server through the PS
  task.  This will cause the DHCP server to be configured for the
  client and AM interfaces specified and the resulting handle to be
  returned in a callback also specified with user data.

DEPENDENCIES
  The am interface must be up and configured. 

PARAMETERS
  ps_iface_type * client - The interface for which the DHCP is serving 
                           addresses.
  ps_iface_type * am - The interface to which the Client interface
                       will be routed.
  done_cb - callback function that will be called when the DHCP has started.
            This may be called before this function has returned. 
            The handle will be NULL on failure. Non-null on success. 
            The userdata will also be passed to this function.
  msg_cb - callback invoked when a DHCP messaeg (INFORM, ACK or DECLINE) is 
           received.  Userdata (below) is passed back, along with received
           message type and the current yiaddr (client IP address) value in 
           the dhcp instance.
  void * userdata - This is user data that will be passed to done_cb.

RETURN VALUE
  boolean - TRUE if message successfully sent to PS task: Expect a
            callback.  FALSE if message not set to PS task: Do not
            expect a callback.

SIDE EFFECTS
  Causes a message to be sent to the PS task to start a DHCP server
  configured to the specified client and AM interfaces. 
===========================================================================*/
boolean
dhcp_start
( 
  ps_iface_type * client, 
  ps_iface_type * am,
  void (*done_cb)(void * userdata, void * handle),
  dhcp_msg_cback_type msg_cb,
  dhcp_arp_cache_updt_type arp_cache_update_cb,
  void * userdata
);

/*===========================================================================
FUNCTION DHCP_STOP()

DESCRIPTION
  This function serializes the stop of a DHCP server through the PS
  task.  This will cause the DHCP server specified to be freed.  The
  callback specifed will be called back with the userdata when this is
  done. The clients handle_ptr will be set to NULL before this
  function returns.

DEPENDENCIES
  The handle must have being allocated by a call to dhcp_start.

PARAMETERS
  void ** handle_ptr - The handle to free. 
  done_cb() - The function to call when done freeing this handle.
              Function will not be called if NULL.
  void * userdata - User specifed data to be passed back when the
                    callback is called.

RETURN VALUE
  boolean - TRUE if message successfully sent to PS task: Expect a
            callback.  FALSE if message not set to PS task: Do not
            expect a callback.

SIDE EFFECTS
  Causes a message to be sent to the PS task to stop the DHCP server
  specifed by the handle passed in.
===========================================================================*/
boolean
dhcp_stop
( 
  void ** handle_ptr,
  void (*done_cb)(void * userdata, boolean ok),
  void * userdata
);

/*===========================================================================
FUNCTION DHCP_INPUT_DSM()

DESCRIPTION
  This function causes the message to be parsed, and the response to
  be sent in the way configured by dhcp_*_start().

DEPENDENCIES
  Handle must be valid. 
  This function must be called from PS context.

PARAMETERS
  void * handle_ptr - Handle of the DHCP session this message is for.
  dsm_item_type **  - dsm item message to parse.

RETURN VALUE
  0 for success, -1 for failure

SIDE EFFECTS
  None
===========================================================================*/
int32
dhcp_input_dsm
( 
  void * handle_ptr,
  dsm_item_type ** item
);

/*===========================================================================
FUNCTION DHCP_CALC_GATEWAY_INFO()

DESCRIPTION
  This function calculates the netmask and server/router IP address
  from the client IP address. This function tries to figure out the
  smallest netmask without handing out the broadcast address.

DEPENDENCIES
  Client IP address must be valid
  Currently only supports IPV4 addrs.  Of course, with IPV6, this
  whole thing is obsolete.

PARAMETERS
  client_ip: Client IP address
  server_id: Gateway IP address (OUT)
  netmask:   Subnet mask (OUT) 

RETURN VALUE
  Server_id and netmask  

SIDE EFFECTS
  None
===========================================================================*/
void dhcp_calc_gateway_info
(
  void  *  handle,
  uint32*  client_ip,
  uint32*  server_id,
  uint32*  netmask
);

/*===========================================================================
FUNCTION DHCP_IPFLTR_INIT()

DESCRIPTION
  This function installs DHCP and DHCP6 global filters 

DEPENDENCIES
  None

PARAMETERS
  None 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dhcp_server_ipfltr_init
(
  void
);

/*===========================================================================
  FUNCTION DHCP_IPFLTR_DEINIT()

  DESCRIPTION
    This function deregisters dhcp filters on the Um and Rm.

  RETURN VALUE
    0 success
   -1 failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dhcp_server_ipfltr_deinit
(
  void
);

/*===========================================================================
FUNCTION DHCP6_IPFLTR_INIT()

DESCRIPTION
  This function installs DHCP6 global filters 

DEPENDENCIES
  None

PARAMETERS
  None 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dhcp6_server_ipfltr_init
(
  void
);

/*===========================================================================
  FUNCTION DHCP6_IPFLTR_DEINIT()

  DESCRIPTION
    This function deregisters dhcp6 filters on the Um and Rm.

  RETURN VALUE
    0 success
   -1 failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dhcp6_server_ipfltr_deinit
(
  void
);

#endif /* DHCP_H */
