/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     I N E T _ S T U B S . C

GENERAL DESCRIPTION

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/inet_stubs.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/31/09    pp     Initial release
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ps_tcp_hdr.h"

#ifndef FEATURE_DATA_PS_IPV6

#include "msg.h"
#include "dsm.h"
#include "dssocket_defs.h"
#include "dssocket.h"
#include "dhcp_client.h"
#include "dhcp6_client.h"
#include "ps_ip6_hdr.h"
#include "ps_ip6_sm.h"
#include "ds_Utils_DebugMsg.h"

#define INET_STUBS_EXT_PERROR_NO_FEAT(api, feat)                         \
  LOG_MSG_ERROR_0(#api": "#feat" feature(s) turned off."                       \
           "Returning error.")

#define DHCP_PERROR(api)                                                 \
  INET_STUBS_EXT_PERROR_NO_FEAT(api, DATA_PS_DHCP)

#define DHCPV6_PERROR(api)                                               \
  INET_STUBS_EXT_PERROR_NO_FEAT(api, DATA_PS_IPV6+DATA_PS_DHCPV6)

#define IPV6_HDR_PERROR(api)                                             \
  INET_STUBS_EXT_PERROR_NO_FEAT(api, DATA_PS_IPV6)

#define IPV6_SM_PERROR(api)                                             \
  INET_STUBS_EXT_PERROR_NO_FEAT(api, DATA_PS_IPV6)

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/




/*===========================================================================
FUNCTION DHCP6_CLIENT_INIT()

Description:
  This function will initialize the memory pool for the DHCPv6 client.

Arguments:
  None.

Return value:
  None.

Dependencies:
  This function must only be called once.  This function must be
  called before the DHCP6 client is used. 
===========================================================================*/
void
dhcp6_client_init
(
  void
)
{
  DHCPV6_PERROR(dhcp6_client_init);
  return;
} /* dhcp6_client_init */

/*==========================================================================
FUNCTION DHCP6_CLIENT_DUID_LLT_INIT()

Description:
  This function will initialize a buffer with a DUID-LLT given the proper 
  field values. 
  
Arguments: 
  duid_buf    - A pointer to a buffer with sufficient size to store the 
                duid of the client interface.
  duid_len    - Size of buffer provided. If size is insufficient, 
                function returns -1. See RFC 3315 for required size.
  hw_type     - An enum indicating the hardware type.
  time        - A 32-bit time value in host byte order.
  ll_addr     - A variable length link-layer address.
  ll_addr_len - Length of ll_addr passed in.
  
Return value:
  Returns -1 if failed, valid size of DUID in bytes if ok.

===========================================================================*/
int
dhcp6_client_duid_llt_init
(
  void                      * duid_buf,
  uint32                      duid_buf_len,
  dhcp_client_hw_type_enum    hw_type,
  uint32                      time,
  void                      * ll_addr,
  uint32                      ll_addr_len
)
{
  DHCPV6_PERROR(dhcp6_client_duid_llt_init);
  return -1;
} /* dhcp6_client_duid_llt_init */

/*==========================================================================
FUNCTION DHCP6_CLIENT_DUID_EN_INIT()

Description:
  This function will initialize a buffer with a DUID-EN given the proper 
  field values. 
  
Arguments: 
  duid_buf       - A pointer to a buffer with sufficient size to store the 
                   duid of the client interface.
  duid_len       - Size of buffer provided. If size is insufficient, 
                   function returns -1. See RFC 3315 for required size.
  enterprise_num - A 32-bit vendor enterprise number in host byte order.
  identifier     - A variable length vendor identifier.
  identifier_len - Length of vendor identifier field.
  
Return value:
  Returns -1 if failed, valid size of DUID in bytes if ok.

===========================================================================*/
int
dhcp6_client_duid_en_init
(
  void                      * duid_buf,
  uint32                      duid_buf_len,
  uint32                      enterprise_num,
  void                      * identifier,
  uint32                      identifier_len
)
{
  DHCPV6_PERROR(dhcp6_client_duid_en_init);
  return -1;
} /* dhcp6_client_duid_en_init */

/*==========================================================================
FUNCTION DHCP6_CLIENT_DUID_LL_INIT()

Description:
  This function will initialize a buffer with a DUID-LL given the proper 
  field values. 
  
Arguments: 
  duid_buf    - A pointer to a buffer with sufficient size to store the 
                duid of the client interface.
  duid_len    - Size of buffer provided. If size is insufficient, 
                function returns -1. See RFC 3315 for required size.
  hw_type     - An enum indicating the hardware type.
  ll_addr     - A variable length link-layer address.
  ll_addr_len - Length of ll_addr passed in.
  
Return value:
  Returns -1 if failed, valid size of buffer of DUID in bytes if ok.

===========================================================================*/
int
dhcp6_client_duid_ll_init
(
  void                      * duid_buf,
  uint32                      duid_buf_len,
  dhcp_client_hw_type_enum    hw_type,
  void                      * ll_addr,
  uint32                      ll_addr_len
)
{
  DHCPV6_PERROR(dhcp6_client_duid_ll_init);
  return -1;
} /* dhcp6_client_duid_ll_init */

/*==========================================================================
FUNCTION DHCP6_CLIENT_PARAMS_INIT()

Description:
  This function will initialize a dhcp6_client_params struct with
  suggested default values. 

Arguments: 
  params          - The handle to the params struct to fill with default
                    values.
  client_duid     - The Client DUID identifying the DHCPv6 client.
  client_duid_len - The length of the DUID passed in 'client_duid'.
  
Return value:
  Returns -1 if failed, 0 if OK.

Dependencies:
  The 'params' handle should not be NULL and space already allocated
  for the struct.

Notes:
  There are no validity checks performed on client duid and duid_len.
  If client duid is passed here, option for sending client_id is set.
===========================================================================*/
int
dhcp6_client_params_init
(
  dhcp6_client_params    * params,
  void                   * client_duid,
  uint32                   client_duid_len
)
{
  DHCPV6_PERROR(dhcp6_client_params_init);
  return -1;
} /* dhcp6_client_params_init */

/*===========================================================================
FUNCTION DHCP6_CLIENT_START()

Description:
  This function will configure the DHCP6 Client for the given
  interface, and returns a handle to the DHCP6 client object. 

  This function will be serialized through the PS task. 

Arguments:
  dss_iface_id_type - The interface descriptor of the interface to configure.
  dhcp6_client_params params - The parameters that are to be used.
  dhcp_client_status_cb_type status_cb
   - This function will be called to indicate the obtaining or loss of an IP
     address.  See below for details on the enum.
  void * userdata - A cookie to be used when the status_cb() is called.

Return value:
  void * - A handle to the new DHCP6 client. NULL on error.

Dependencies:
  This must be called before stop, renew or release.
  The DUID in the params struct must be properly formatted according
  to RFC 3315.  See dhcp6_client_duid_*_init() utility functions to format 
  a DUID buffer.
  The dhcp6_client_init() must have been called before this function.

Notes:
  Validity checks on client duid and duid_len are done here. If client duid
  is passed in params, duid_len is checked to be within specific limits.
===========================================================================*/
void * 
dhcp6_client_start
( 
  dss_iface_id_type iface,
  dhcp6_client_params * params,
  dhcp_client_status_cb_type status_cb,
  void * userdata
)
{
  DHCPV6_PERROR(dhcp6_client_start);
  return NULL;
} /* dhcp6_client_start */

/*===========================================================================
FUNCTION DHCP6_CLIENT_RENEW()

Description:
  This function starts the DHCP6 client.  In particular, this function
  will configure an instance of the DHCP6 client for provisioning the
  PS network stacks for operation.
  Because our DHCPv6 stateless client does not deal with leases, this 
  function is not implemented and will cause an ASSERT to occur.

Arguments:
  void * dhcp6_client_handle - The handle from dhcp6_client_start().
    
Return value:
  None.

Dependencies:
  None.
===========================================================================*/
void
dhcp6_client_renew
(
  void * dhcp6_client_handle
)
{
  DHCPV6_PERROR(dhcp6_client_renew);
  return;
} /* dhcp6_client_renew */

/*===========================================================================
FUNCTION DHCP6_CLIENT_RELEASE()

Description:
  This function will cause the DHCP6 client to release a lease. 
  Because our DHCPv6 stateless client does not deal with leases, this 
  function is not implemented and will cause an ASSERT to occur.

Arguments:
  void * dhcp6_client_handle - The handle from dhcp6_client_start() of the
    client to release.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
void 
dhcp6_client_release
(
  void * dhcp6_client_handle 
)
{
  DHCPV6_PERROR(dhcp6_client_release);
  return;
} /* dhcp6_client_release */

/*===========================================================================
FUNCTION DHCP6_CLIENT_INFORM()

Description:
  This function will cause the DHCP6 client to try to get provisions
  via an INFORMATIONREQUEST message without getting a lease.

  This function will be serialized through the PS task. 

Arguments:
  void * dhcp6_client_handle - The handle from dhcp6_client_start() of the
    client to release.

Return value:
  None.

Dependencies:
  The handle must have been allocated from a dhcp6_client_start()
    function. 
===========================================================================*/
void 
dhcp6_client_inform
(
  void * dhcp6_client_handle
)
{
  DHCPV6_PERROR(dhcp6_client_inform);
  return;
} /* dhcp6_client_inform */

/*===========================================================================
FUNCTION DHCP6_CLIENT_STOP()

Description:
  This function will stop a DHCP6 client.  This will close the DHCP6 client 
  handle and free the associated memory. 

  This function will be serialized through the PS task. 

Arguments:
  dhcp6_client_handle - The handle from dhcp6_client_start() of the
    client to close.

Return value:
  None.

Dependencies:
  The handle must have been allocated from a dhcp6_client_start()
    function. 
===========================================================================*/
void 
dhcp6_client_stop
(
  void ** dhcp6_client_handle 
)
{
  DHCPV6_PERROR(dhcp6_client_stop);
  return;
} /* dhcp6_client_stop */

/*=========================================================================
FUNCTION DHCP6_CLIENT_IS_DHCP_IN_PROGRESS

Description:
  This function returns whether DHCP6 client is currently running.

Arguements:
  void * dhcp6_client_handle - handle to DHCP Client core.

Return value:
  TRUE if client is running, FALSE otherwise.

Dependencies:
  The handle must have been allocated by dhcp6_client_start().
=========================================================================*/
boolean
dhcp6_client_is_dhcp_in_progress
(
  void * dhcp6_client_handle
)
{
  DHCPV6_PERROR(dhcp6_client_is_dhcp_in_progress);
  return FALSE;
} /* dhcp6_client_is_dhcp_core_in_progress */




/*===========================================================================

FUNCTION     PS_IP6_HDR_CREATE()

DESCRIPTION  This function is used to create outgoing IP6 headers for 
             outgoing IP6 packets. This function creates the associated IP6 
             header using the parameters specified by the caller.
             
DEPENDENCIES None

RETURN VALUE TRUE on success
             FALSE on failure
                         
SIDE EFFECTS None
===========================================================================*/
boolean ps_ip6_hdr_create
(
  dsm_item_type   **data_payload,
  ip6_hdr_type    *hdr
)
{
  IPV6_HDR_PERROR(ps_ip6_hdr_create);
  return FALSE;
} /* ps_ip6_hdr_create() */


/*===========================================================================
FUNCTION IP6_SM_INIT()

DESCRIPTION
  This function performs NV related initializations of IP6 state machine.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  This function is called after signalling PS task start.

SIDE EFFECTS
  None
===========================================================================*/
void ip6_sm_init
(
  void
)
{
  IPV6_SM_PERROR(ip6_sm_init);
  return;
}

/*===========================================================================
FUNCTION IP6_SM_CREATE()

DESCRIPTION
  This function creates an instance of the IPv6 state machine using memory
  passed in by the caller.  It will initialize the associated ps_iface to
  refer to it so that events (such as adding a prefix or lifetime expiry) can
  be posted to the SM.

PARAMETERS
  instance: pointer to the memory to be used to allocate the state machine.
  iface_ptr:  pointer to the associated PS Iface instance
  init_sol_delay: initial delay for solicitations (ms)
  sol_interval:   interval between solicitations in Soliciting state (ms)
  re_sol_interval: interval between solicitations in RE-Soliciting state (ms)

RETURN VALUE
  0 on succuss
 -1 on failure

DEPENDENCIES
  ip6_sm_init() needs to have been called first.

SIDE EFFECTS
  will set the ip6_sm_ptr value in the associated PS Iface pointer to it.
===========================================================================*/
int ip6_sm_create
(
  ip6_sm_type         *instance,
  ps_iface_type       *iface_ptr,
  ip6_sm_config_type  *sm_config
)
{
  IPV6_SM_PERROR(ip6_sm_create);
  return -1;
}

/*===========================================================================
FUNCTION IP6_SM_POST_EVENT()

DESCRIPTION
  This function is used to post an event to the state machine passed in.
  NOTE: if the event is posted outside PS context a command will be sent to
  PS to process that event, otherwise this will happen in line.

PARAMETERS
  instance: the instance to which the event is being posted.
  event: the event being posted
  event_data_ptr: the pointer points the user data when post a event

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  ip6_sm_create() needs to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
int ip6_sm_post_event
(
  ip6_sm_type      *instance,
  ip6_sm_event_type event,
  void             *event_data_ptr
)
{
  IPV6_SM_PERROR(ip6_sm_post_event);
  return -1;
}

/*===========================================================================
FUNCTION IP6_SM_IND_CBACK_REG()

DESCRIPTION
  This function is used to register for indications from the SM.

PARAMETERS
  instance: the instance of the SM to register with.
  cback: the callback being registered.
  user_data: data that is passed back in the callback.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  ip6_sm_create() needs to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
int ip6_sm_ind_cback_reg
(
  ip6_sm_type       *instance,
  ip6_sm_cback_type  cback,
  void              *user_data
)
{
  IPV6_SM_PERROR(ip6_sm_ind_cback_reg);
  return -1;
}
#endif /* IPV6*/
