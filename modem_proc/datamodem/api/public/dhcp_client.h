#ifndef DHCP_CLIENT_H
#define DHCP_CLIENT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D H C P _ C L I E N T . H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/**
  @file dhcp_client.h
  @brief Contains the public function to interface with the DHCP client.
   The DHCP client needs to know about media connects and disconnects.
   This will be done with start and stop calls.  Similarly, the DHCP
   client needs to inform the network stack of when the current address
   is not valid any longer.  This will done through the Configuration
   shim layer.  In addition, the DHCP client layer will need
   initialization before starting (for memory pool initialization, etc).
 */

/*
  INITIALIZATION AND SEQUENCING REQUIREMENTS
  - The DHCP client is started by calling dhcp_client_start().
  - The DHCP client is stoped by calling dhcp_client_stop().
*/

/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the dhcp group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2004-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dhcp_client.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
08/07/09    kk     Added support for client identifier option(opt type=61).
05/14/09    pp     Moved Local Heap allocations to Modem Heap.
03/26/09    pp     CMI De-featurization.
12/14/08    pp     Common Modem Interface: Public/Private API split.
08/25/08    am     Added DHCP ongoing determination.
01/29/08    am     Added Rapid Commit Option handling.
04/15/07    es     Removed DHCP IOCTL callback functions, added params_init
                   function to initialize client parameters.  Added unicast
                   address and param request booleans to params struct.
02/01/07    hm     Added IOCTL to get fresh DHCP configuration.
08/12/04    clp    Initial development work done.
===========================================================================*/
/* group: dhcp */

#include "ps_iface.h"
#include "dhcp_client_common.h"


/** @addtogroup datatypes 
  @{
*/

/**
  @brief Parameters that control DHCP client behavior.
*/
typedef struct
{
  uint32 cookie;                     
    /**< Cookie to verify the initialization of the structure. */

  dhcp_client_hw_type_enum  hw_type; 
    /**< Client hardware address type. Default: #DHCP_CLIENT_HWTYPE_ETHERNET. */

  uint32 hw_addr_len;                
    /**< Length of the hardware address. */
  uint8  *hw_addr;                   
    /**< Hardware address. */

  /*
  The following params determine if the client request these
  options in the Parameter Request List Option.
  If the options are not requested, they are subsequently
  not set in the ps_iface even if the DHCP server responds
  with the relevant configuration info.
  Suggested value for these options is TRUE.
  */
  boolean domain_name_server_option; 
    /**< Indicates whether the client has requested the Domain Name Server
         option in the Parameter Request List option. If not requested, this
         option is not set in ps_iface even if the DHCP server responds with
         the relevant configuration information. Suggested value: TRUE. Refer
         to RFC 2132 @xhyperref{S1,[S1]}, Section 3.8. */
  boolean subnet_mask_option;        
    /**< Indicates whether the client has requested the Subnet Mask option in
         the Parameter Request List option. If not requested, this option is
         not set in ps_iface even if the DHCP server responds with the relevant
         configuration information. Suggested value: TRUE. Refer to RFC 2132
         @xhyperref{S1,[S1]}, Section 3.3. */
  boolean router_option;             
    /**< Indicates whether the client has requested the Router option in the
         Parameter Request List option. If not requested, this option is not
         set in ps_iface even if the DHCP server responds with the relevant
         configuration information. Suggested value: TRUE. Refer to RFC 2132
         @xhyperref{S1,[S1]}, Section 3.5. */
  boolean interface_mtu_option;      
    /**< Indicates whether the client has requested the Interface MTU option in
         the Parameter Request List option. If not requested, this option is not
         set in ps_iface even if the DHCP server responds with the relevant
         configuration information. Suggested value: TRUE. Refer to RFC 2132 
         @xhyperref{S1,[S1]}, Section 5.1. */
  boolean domain_name_option;        
    /**< Indicates whether the client has requested the Domain Name option in
         the Parameter Request List option. If not requested, this option is
         not set in ps_iface even if the DHCP server responds with the relevant
         configuration information. Suggested value: TRUE. Refer to RFC 2132 
         @xhyperref{S1,[S1]}, Section 3.17.  */
  boolean sip_server_option;         
    /**< Indicates whether the client has requested the SIP Server option in
         the Parameter Request List option. If not requested, this option is
         not set in ps_iface even if the DHCP server responds with the relevant
         configuration information. Suggested value: TRUE. Refer to RFC 3361
         @xhyperref{S2,[S2]}. */
  boolean rapid_commit_option;       
    /**< Indicates whether the client has requested the Rapid Commit option in
         the Parameter Request List option. If not requested, this option is
         not set in ps_iface even if the DHCP server responds with the relevant
         configuration information. Suggested value: TRUE. Refer to RFC 4039
         @xhyperref{S7,[S7]}. */

  /* Number of times to try at various states */

  uint32 reboot_tries;               
    /**< Number of tries to request at startup before starting over with
         discover. Suggested value: 2. */

  uint32 selecting_tries;            
    /**< Number of tries to send the discover before starting over in the Init 
         state. This value allows a count of the number of times through the 
         Init state to indicate a failure to get a lease. Suggested value: 4. */

  uint32 requesting_tries;           
    /**< Number of tries to send the request message in the Requesting state. 
         Suggested value: 4. */

  uint32 init_tries;                
    /**< Number of times before failing. This is the number of times returned 
         to the Init state without successfully getting a lease, before 
         notifying the upper layer of the failure to get a lease. The upper 
         layer may indicate to continue the tries (default), or to abort the 
         tries. Suggested value: 4. */

  uint32 inform_tries;               
    /**< Number of tries to send an inform message before the upper layer is 
         notified of the failure to get provisioning. A value is not specified  
         by the RFC. Per RFC 2131 @xhyperref{S33,{S33]}, Section 4.4.3, stop 
         trying on failure. Suggested value: 4. */

  uint32 decline_restart_time;       
    /**< Amount of time to wait before starting the Init state after a decline. 
         Suggested value: A minimum of 10 seconds. */

  uint32 init_restart_time;          
    /**< Sets the maximum random time (between 1 second and this value) to wait 
         before sending the request in the Init state. Suggested value: 
         10 seconds. */

  uint32 verify_time;                
    /**< Time to allow the ARP layer to verify the address returned in the 
         discover. Suggested value: 2 seconds. */

  uint32 reboot_base_time;           
    /**< Sets the initial time between requests in the Reboot state. Suggested 
         value: 2 seconds. */

  uint32 select_base_time;           
    /**< Sets the initial time between discover messages in the Select state. 
         Suggested value: 4 seconds. */

  uint32 request_base_time;           
    /**< Sets the initial time between requests in the Requesting state. 
         Suggested value: 4 seconds. */

  uint32 inform_base_time;           
    /**< Sets the initial time between inform messages in the Inform state. 
         Suggested value: 4 seconds. */

  uint32 server_addr;                
    /**< DHCPv4 address that the client tries to send DHCP messages to when
         send_broadcast is TRUE. Default: #DHCP_CLIENT_BROADCAST_ADDR. */

  dhcp_client_id_option_type_enum client_id_type;
   /**< Specifies a DHCP client identifier. Default: 
        #DHCP_CLIENT_ID_OPTTYPE_NONE. This means the client identifier option
        is not used.*/

  uint32 client_id_len;              
    /**< Length of the custom client identifier, if it has been set. For
         #DHCP_CLIENT_ID_OPTTYPE_NONE and #DHCP_CLIENT_ID_OPTTYPE_HWADDR, this 
         value is set to zero. This value is also bound by 
         #DHCP_CLIENT_CLIENT_ID_MIN_LEN and #DHCP_CLIENT_CLIENT_ID_MAX_LEN. */

  uint8 *client_id;                  
    /**< Buffer that the DHCP clients use to pass the custom client identifier 
         value, if necessary. This is the pointer to a static buffer allocated 
         by the client. */
} dhcp_client_params;
/** @} */ /* end_addtogroup datatypes */

/*==========================================================================
FUNCTION DHCP_CLIENT_PARAMS_INIT()
===========================================================================*/
/** @ingroup dhcp_client_params_init
  Initializes a #dhcp_client_params structure with the suggested default 
  values.

  @datatypes
  #dhcp_client_params

  @vertspace
  @param[out] params      Handle to the #dhcp_client_params structure to fill
                          with default values.
  @param[in] hw_addr      Pointer to the hardware address identifying the DHCP
                          client.
  @param[in] hw_addr_len  Length of the hardware address passed in hw_addr.

  @return
  0 -- Initialization was successful.\n
  -1 -- Initialization failed.

  @dependencies
  The params handle must not be NULL, and space must already be allocated
  for the structure.
  @newpage
*/
int
dhcp_client_params_init
(
  dhcp_client_params    * params,
  void                  * hw_addr,
  uint32                  hw_addr_len
);

/*===========================================================================
FUNCTION DHCP_CLIENT_START()
===========================================================================*/
/** @ingroup dhcp_client_start
  Configures the DHCP client for the given interface, and returns a handle to 
  the DHCP client object. This function is serialized through the PS task.

  @datatypes
  #dss_iface_id_type\n
  #dhcp_client_params\n
  #dhcp_client_status_cb_type

  @vertspace
  @param[in] dss_iface_id_type Interface descriptor of the interface to
                               configure.
  @param[in] params            Pointer to the parameters in the 
                               #dhcp6_client_params structure.
  @param[in] status_cb         Indicates whether an IP address is obtained or
                               lost.
  @param[in] userdata          Pointer to the user data passed to the status_cb
                               parameter.

  @return
  A handle to the new DHCP client.\n 
  NULL -- Error.

  @dependencies
  This function must be called before the dhcp_client_stop(), dhcp_client_renew(), 
  or dhcp_client_release() functions.\n
  The dhcp_client_params_init() must be called before calling this function.\n
  When a non-NULL handle is returned, the dhcp_client_stop() function must be
  called because the actual initialization is serialized through the PS task.
  @newpage
*/
void *
dhcp_client_start
(
  dss_iface_id_type iface,
  dhcp_client_params * params,
  dhcp_client_status_cb_type status_cb,
  void * userdata
);

/*===========================================================================
FUNCTION DHCP_CLIENT_RENEW()
===========================================================================*/
/** @ingroup dhcp_client_renew
  Starts the DHCP client. This function configures an instance of the DHCP 
  client for provisioning the PS network stacks for operation. The actual 
  provisioning occurs at some point in the future once the DHCP client has 
  found and negotiated with a server. This function is serialized through the 
  PS task.

  @param[in] dhcp_client_handle   Handle from dhcp_client_start() of the
                                  client to release.

  @return
  none.

  @dependencies
  The dhcp_client_start() function must be called before calling this 
  function.\n
  The interface passed in must be in a state that enables it to receive UDP 
  packets and pass the packets up to the application (DHCP
  client) before an IP address is provisioned based on the hardware address.
  @newpage
*/
void
dhcp_client_renew
(
  void * dhcp_client_handle
);

/*===========================================================================
FUNCTION DHCP_CLIENT_RELEASE()
===========================================================================*/
/** @ingroup dhcp_client_release
  Enables the DHCP client to release a lease. This function is implemented when 
  needed. This function is serialized through the PS task.

  @param[in] dhcp_client_handle   Handle from dhcp_client_start() of the
                                  client to release.

  @return
  None.

  @dependencies
  The handle must be allocated from the dhcp_client_start() function.
  @newpage
*/
void
dhcp_client_release
(
  void * dhcp_client_handle
);

/*===========================================================================
FUNCTION DHCP_CLIENT_INFORM()
===========================================================================*/
/** @ingroup dhcp_client_inform
  Enables the DHCP client to get provisions via an Inform message without 
  getting a lease. This function is serialized through the PS task.

  @param[in] dhcp_client_handle   Handle from dhcp_client_start() of the
                                  client to release.
  @param[in] addr                 IP address in network byte order.

  @return
  None.

  @dependencies
  The handle must have been allocated from the dhcp_client_start()
  function.
  @newpage
*/
void
dhcp_client_inform
(
  void * dhcp_client_handle,
  uint32 addr
);

/*===========================================================================
FUNCTION DHCP_CLIENT_STOP()
===========================================================================*/
/** @ingroup dhcp_client_stop
  Stops a DHCP client. This function causes the DHCP to close the DHCP client 
  handle and free the associated memory. This function is serialized through 
  the PS task.

  @param[in] dhcp_client_handle   Handle from dhcp_client_start() of the
                                  client to close.

  @return
  None.

  @dependencies
  The handle must have been allocated from the dhcp_client_start()
  function.
  @newpage
*/
void
dhcp_client_stop
(
  void ** dhcp_client_handle
);

/*===========================================================================
FUNCTION DHCP_CLIENT_IS_DHCP_IN_PROGRESS
===========================================================================*/
/** @ingroup dhcp_client_is_dhcp_in_progress 
  Indicates whether the DHCP client is currently running.

  @param[in] handle   Handle to the DHCP client core.

  @return
  TRUE  -- Client is running.\n
  FALSE -- Otherwise.

  @dependencies
  The handle must have been allocated from the dhcp_client_start() 
  function.
  @newpage
*/
boolean
dhcp_client_is_dhcp_in_progress
(
  void * dhcp_client_handle
);

#endif /* DHCP_CLIENT_H */
