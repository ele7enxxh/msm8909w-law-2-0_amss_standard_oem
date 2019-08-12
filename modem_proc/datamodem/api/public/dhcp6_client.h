#ifndef DHCP6_CLIENT_H
#define DHCP6_CLIENT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D H C P 6 _ C L I E N T . H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/**
  @file dhcp6_client.h
  @brief Contains the public function to interface with the DHCP client.
*/
/*
  INITIALIZATION AND SEQUENCING REQUIREMENTS
  - The DHCP6 client is started by calling dhcp6_client_start().  
  - The DHCP6 client is stoped by calling dhcp6_client_stop(). 
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
Copyright (c) 2006-2014 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE
                           
$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dhcp6_client.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/09/14    ash    Added Prefix Delegation support.
11/28/11    cp     Moved the DUID changes to dhcp6_common_defs.h.
08/28/11    SCD    (Tech Pubs) Applied new Doxygen grouping markup.
08/11/11    cp     Added Doxygen markup and comments.
04/03/11    sd     (Tech Pubs) Edited/added Doxygen markup and comments.
05/14/09    pp     Moved Local Heap allocations to Modem Heap.
12/14/08    pp     Common Modem Interface: Public/Private API split.
08/25/08    am     Added DHCP ongoing determination.
03/26/07    es     Added support for unicast addr. Removed IOCTL changes,
                   including dhcp6i_client_status_cb_ex callback.
01/23/07    es     Changes made for MIPv6 support, eg. passing generic DUID 
                   instead of hw_addr, home_network_id_option.
                   Added dhcp6_client_duid_init functions.
02/23/07    rt     Changed dhcp6i_client_status_cb_ex() name to internal fn.
02/01/07    hm     Added IOCTL to get fresh DHCP configuration
06/29/06    es     Initial development work.
===========================================================================*/
/* group: dhcp */

#include "ps_iface.h"
#include "ps_in.h"
#include "dhcp_client_common.h"

/** @addtogroup datatypes 
  @{
*/

/**
  @brief DHCPv6 client mode enum.
*/
typedef enum {
  DHCP6_CLIENT_MODE_STATELESS = 0,
  /**< Stateless Client mode. */

  DHCP6_CLIENT_MODE_STATEFUL  = 1
  /**< Statelful Client mode. */
} dhcp6_client_mode_type;


/**
  @brief Parameters that control DHCPv6 client behavior.
*/
typedef struct 
{
  /** @cond
  */
  uint32 cookie;
  /**< Cookie to verify initialization of the structure. This is a private 
       internal value. */
  /** @endcond */
 
  uint8 * client_duid;
  /**< Client DUID. */
  uint16  client_duid_len;
  /**< Client DUID length. */

  /* The following two params determine if the client includes these 
     options in sending a Information-Request message. */
  
  boolean client_id_option;
  /**< If set, the client ID option is included in all messages sent 
       from the client to the server (i.e., the inform message for the 
       stateless client). Section 18.1.5 of RFC 3315 @xhyperref{S3,[S3]} 
       states: 

       @par
       "The client SHOULD include a Client Identifier option
       to identify itself to the server. If the client does not include
       a Client Identifier option, the server will not be able to return
       any client-specific options to the client, or the server may
       choose not to respond to the message at all." */
  
  boolean reconfigure_accept_option;
  /**< If set, the reconfigure accept option is included in all allowed 
       messages from the client to the server (see Section A RFC 3315 @xhyperref{S3,[S3]}).
       Section 22.20, RFC 3315 @xhyperref{S3,[S3]} states:

       @par
       "A client uses the Reconfigure Accept option to announce to the
       server whether the client is willing to accept Reconfigure
       messages. The default behavior, in the absence of this option,
       means unwillingness to accept Reconfigure messages. */

#ifdef FEATURE_DATA_PS_DHCPV6
  /**< This specifies what mode to operate client on. Stateless vs Stateful */
  dhcp6_client_mode_type mode;
#endif /* FEATURE_DATA_PS_DHCPV6 */

  /* The following params determine if the client request these 
   * options in the Option Request Option.
   * Suggested value for these options is true. */
  boolean sip_domain_name_list_option;
  /**< Domain names of the SIP outbound proxy servers for the client to use. 
       Refer to RFC 3319 @xhyperref{S4,[S4]}. */
  boolean sip_ipv6_list_option;
  /**< SIP outbound IPv6 proxy servers available to the client. 
       Refer to RFC 3319 @xhyperref{S4,[S4]}. */
  boolean dns_servers_option;
  /**< Provides a list of one or more IPv6 addresses of DNS recursive 
       name servers to which a client's DNS resolver may send DNS queries. 
       Refer to RFC 3646 @xhyperref{S5,[S5]}.  */
  boolean dns_domain_searchlist_option;
  /**< Domain search list the client is to use when resolving hostnames 
       with DNS. Refer to RFC 3646 @xhyperref{S5,[S5]}. */
  boolean info_refresh_time_option;
  /**< Specifies an upper bound for how long a client is to wait 
       before refreshing information retrieved from the DHCPv6 server. 
       Refer to RFC 4242 @xhyperref{S8,[S8]}. */
  boolean expect_vendor_opts_bootstrap_info;
  /**< Set to TRUE when using the mechanism defined in
       3GPP2 X.S0011-002-D Section 5.5.2 @xhyperref{S16,[S16]} for obtaining 
       MIPv6 bootstrap information, and when expecting bootstrap information, 
       otherwise bootstrap configuration information may not be set in 
       ps_iface. */

  boolean mip6_home_network_id_option;
  /**< Option code is placed in the Option Request option when set to TRUE.
       Refer to @xhyperref{S16,[S16]}. Default: FALSE. */

#ifdef FEATURE_DATA_PS_DHCPV6
  boolean prefix_delegation_option;
  /**< Prefix Delegation is placed in Option Request option when set to TRUE. 
       Default : FALSE for stateless mode and TRUE for stateful mode */

  boolean rapid_commit;
  /**< Rapid commit procedure is used when this is set to TRUE. 
       Default : FALSE for stateless mode and TRUE for stateful mode */

  boolean prefix_exclude;
  /**< Uses prefix exclude procedure when this is set to TRUE. 
       Default : FALSE for stateless mode and TRUE for stateful mode */
#endif /* FEATURE_DATA_PS_DHCPV6 */

  dhcp6_mip6_hnid_option_vals mip6_home_network_id_vals; 
  /**< Home network information option values to send to the DHCPv6 server,
       zeroed out by default. */

  uint32 informationrequest_tries;
  /**< This is not specified by RFC. This is the number of attempts 
       to send an information request message before notifying the upper
       layer that provisioning has not been obtained.  
       Suggested value: 4. */
  
  /* The following information-request retransmission suggested values
   * are specified in RFC 3315, Sections 5.5 and 14. */
  
  uint32 informationrequest_max_delay;
  /**< Maximum desynchronization delay in seconds. RFC suggested value: 1. 
       Refer to RFC 3315, Sections 5.5 and 14 @xhyperref{S3,[S3]}. */

  uint32 informationrequest_irt;
  /**< Initial retransmit time in seconds. RFC suggested value: 1. Refer to 
       RFC 3315, Sections 5.5 and 14 @xhyperref{S3,[S3]}. */      

  uint32 informationrequest_max_rt; 
  /**< Maximum retransmit time in seconds. RFC suggested value: 120. Refer to 
       RFC 3315, Sections 5.5 and 14 @xhyperref{S3,[S3]}. */  

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
   * are specified in RFC 4242, Section 3.1.  */
  
  uint32 info_refresh_time_default;
  /**< Default refresh time in seconds. RFC suggested value: 86400. Refer to 
       RFC 4242, Section 3.1 @xhyperref{S8,[S8]}. */
  
  uint32 info_refresh_time_minimum;
  /**< Minimum refresh time in seconds. RFC suggested value: 600. Refer to 
       RFC 4242, Section 3.1 @xhyperref{S8,[S8]}. */

  /** DHCPv6 server address to send DHCP messages to when the send_multicast 
    flag is TRUE. Default: #ALL_DHCP_RELAY_AGENTS_AND_SERVERS_ADDR multicast 
    address. */ 
  struct ps_in6_addr server_addr;
  

  /* Full DHCPv6 client will require additional parameters 
   * eg. lease related params, solicit/renew/request/etc
   * retransmission parameters, that will go here. */

} dhcp6_client_params;
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup macros_dhcp
  @{
*/
/*==========================================================================
FUNCTION DHCP6_CLIENT_DUID_LLT_INIT()
===========================================================================*/
/** @ingroup dhcp6_client_duid_llt_init
  Initializes a buffer with a DUID LLT.

  @datatypes
  #dhcp_client_hw_type_enum

  @vertspace
  @param[out] duid_buf   Pointer to a buffer with sufficient size to store
                         the DUID of the client interface.
  @param[in] duid_len    Size of the buffer. If the size is insufficient, 
                         the function returns -1.
  @param[in] hw_type     Hardware type.
  @param[in] time        32-bit time value.
  @param[in] ll_addr     Pointer to a variable length link-layer address.
  @param[in] ll_addr_len Length of ll_addr passed in.

  @return
  Valid size of the DUID in bytes.\n
  -1 -- Initialization failed.

  @dependencies
  None.
  @newpage
*/

#define dhcp6_client_duid_llt_init dhcp6_duid_llt_init

/*==========================================================================
FUNCTION DHCP6_CLIENT_DUID_EN_INIT()
===========================================================================*/
/** @ingroup dhcp6_client_duid_en_init
  Initializes a buffer with a DUID EN. 

  @param[out] duid_buf      Pointer to a buffer with sufficient size to store
                            the DUID of the client interface.
  @param[in] duid_len       Size of the buffer. If the size is insufficient, 
                            the function returns -1.
  @param[in] enterprise_num 32-bit vendor enterprise number.
  @param[in] identifier     Pointer to a variable length link-layer address.
  @param[in] identifier_len Length of the identifier passed in.

  @return
  Valid size of the DUID in bytes.\n
  -1 -- Initialization failed.

  @dependencies
  None.
  @newpage
*/

#define dhcp6_client_duid_en_init dhcp6_duid_en_init


/*==========================================================================
FUNCTION DHCP6_CLIENT_DUID_LL_INIT()
===========================================================================*/
/** @ingroup dhcp6_client_duid_ll_init
  Initializes a buffer with a DUID LL. 

  @param[out] duid_buf   Pointer to a buffer with sufficient size to store
                         the DUID of the client interface.
  @param[in] duid_len    Size of the buffer. If the size is insufficient,
                         the function returns -1.
  @param[in] hw_type     Hardware type.
  @param[in] ll_addr     Variable length link-layer address.
  @param[in] ll_addr_len Length of ll_addr passed in.
  
  @return
  Valid size of the DUID in bytes.\n
  -1 -- Initialization failed.

  @dependencies
  None.
  @newpage
*/

#define dhcp6_client_duid_ll_init dhcp6_duid_ll_init
/** @} */ /* end_addtogroup macros_dhcp */

/*==========================================================================
FUNCTION DHCP6_CLIENT_PARAMS_INIT()
===========================================================================*/
/** @ingroup dhcp6_client_params_init
  Initializes a dhcp6_client_params structure. 

  @datatypes
  #dhcp6_client_params

  @vertspace
  @param[out] params     Handle to the params struct to fill with
                         default values.
  @param[in] duid        Pointer to a buffer containing the DUID of the
                         client interface.
  @param[in] duid_len    Length of the DUID passed in.

  @return
  0 -- Initialization was successful.\n
  -1 -- Initialization failed.

  @dependencies
  The params handle must not be NULL, and space must already be allocated
  for the structure.
  @newpage
*/
int 
dhcp6_client_params_init
(
  dhcp6_client_params    * params, 
  dhcp6_client_mode_type   mode,
  void * duid,
  uint16 duid_len
);

/*===========================================================================
FUNCTION DHCP6_CLIENT_START()
===========================================================================*/
/** @ingroup dhcp6_client_start
  Configures the DHCPv6 client for the given interface, and returns a handle to 
  the DHCPv6 client object. This function is serialized through the PS task. 

  @datatypes
  #dss_iface_id_type\n
  #dhcp6_client_params\n
  #dhcp_client_status_cb_type

  @vertspace
  @param[in] dss_iface_id_type          Interface descriptor of the 
                                        interface to be configured.
  @param[in] params                     Pointer to the parameters in the 
                                        #dhcp6_client_params structure.
  @param[in] status_cb                  Indicates the callback status (i.e.,
                                        configuration data being provisioned).
  @param[in] userdata                   Pointer to the user data passed to the 
                                        status_cb parameter.
      
  @return
  A handle to the new DHCP client.\n 
  NULL -- Error.

  @dependencies
  This function must be called before the dhcp6_client_stop(), dhcp6_client_renew() 
  or dhcp6_client_release() function.\n
  Before this function is called the dhcp6_client_params_init() must have been called to 
  initialize the params structure.
  @par
  When a non-NULL handle is returned by this function, the dhcp6_client_stop() 
  function must be called because the actual initialization is serialized 
  through the PS task. 
  @newpage
*/
void * 
dhcp6_client_start
( 
  dss_iface_id_type iface,
  dhcp6_client_params * params,
  dhcp_client_status_cb_type status_cb,
  void * userdata
);

/** @cond
*/
/*===========================================================================
FUNCTION DHCP6_CLIENT_RENEW() ** NOT IMPLEMENTED **
===========================================================================*/
/** 
  DHCPv4 client legacy function used to configure an 
  instance of the DHCP client for provisioning the PS network stacks for 
  operation.

  This function is not implemented for the stateless DHCPv6 client and
  generates an error message. 

  @param[in] dhcp6_client_handle   Handle from dhcp6_client_start() of the
                                   client to release.

  @return
  None.

  @dependencies
  None.
  @newpage
*/
void
dhcp6_client_renew
(
  void * dhcp6_client_handle
);

/*===========================================================================
FUNCTION DHCP6_CLIENT_RELEASE() ** NOT IMPLEMENTED **
===========================================================================*/
/**
  DHCPv4 client legacy function used to cause the DHCP client to release a 
  lease. 

  This function is not implemented for the stateless DHCPv6 client and 
  generates an error message. 

  @param[in] dhcp6_client_handle   Handle from dhcp6_client_start() of the
                                   client to release.

  @return
  None.

  @dependencies
  None.
  @newpage
*/
void 
dhcp6_client_release
(
  void * dhcp6_client_handle 
);
/** @endcond */

/*===========================================================================
FUNCTION DHCP6_CLIENT_INFORM()
===========================================================================*/
/** @ingroup dhcp6_client_inform
  Enables the DHCPv6 client to get provisions via an INFORMATIONREQUEST 
  message without getting a lease. This function is serialized through the PS 
  task. 

  @param[in] dhcp6_client_handle   Handle from dhcp6_client_start() of the
                                   client to release.
  
  @return
  None.

  @dependencies
  The handle must be allocated from a dhcp6_client_start() function before 
  calling this function. 
  @newpage
*/
void 
dhcp6_client_inform
(
  void * dhcp6_client_handle  
);

#ifdef FEATURE_DATA_PS_DHCPV6
/*===========================================================================
FUNCTION DHCP6_CLIENT_STOP()
===========================================================================*/
/** @ingroup dhcp6_client_stop
  Stops a DHCP client. This function causes the DHCP to close the DHCP client 
  handle and free the associated memory. This function is serialized through 
  the PS task. 

  @param[in] dhcp6_client_handle   Handle from dhcp6_client_start() of the
                                   client to close.

  @return
  None.

  @dependencies
  The handle must be allocated from a dhcp6_client_start() function before 
  calling this function. 
  @newpage
*/
void 
dhcp6_client_stop
(
  void * dhcp6_client_handle 
);

/*===========================================================================
FUNCTION DHCP6_CLIENT_ABORT()
===========================================================================*/
/** @ingroup dhcp6_client_abort
  Aborts a DHCP client. This function causes the DHCP to abort the DHCPv6
  client handle and free the associated memory. This function is serialized
  through the PS task. 

  @param[in] dhcp6_client_handle   Handle from dhcp6_client_start() of the
                                   client to close.

  @return
  None.

  @dependencies
  The handle must be allocated from a dhcp6_client_start() function before 
  calling this function. 
  @newpage
*/
void 
dhcp6_client_abort
(
  void * dhcp6_client_handle 
);
#endif /* FEATURE_DATA_PS_DHCPV6 */

/*===========================================================================
FUNCTION DHCP6_CLIENT_FREE()
===========================================================================*/
/** @ingroup dhcp6_client_free
  This function frees the memory associated with DHCPv6 Client.
  This function is serialized through the PS task. 

  @param[in] dhcp6_client_handle   Handle from dhcp6_client_start() of the
                                   client to close.

  @return
  None.

  @dependencies
  The handle must be allocated from a dhcp6_client_start() function before 
  calling this function. 
  @newpage
*/
void 
dhcp6_client_free
(
  void ** dhcp6_client_handle 
);

/*=========================================================================
FUNCTION DHCP6_CLIENT_IS_DHCP_IN_PROGRESS
=========================================================================*/
/** @ingroup dhcp6_client_is_dhcp_in_progress
  Indicates whether the DHCPv6 client is currently running.

  @param[in] dhcp6_client_handle   Handle to the DHCP client core.

  @return
  TRUE -- Client is running.\n
  FALSE -- Otherwise.

  @dependencies
  The handle must be allocated by the dhcp6_client_start() function before 
  calling this function.
  @newpage
*/
boolean
dhcp6_client_is_dhcp_in_progress
(
  void * dhcp6_client_handle
);


#endif /* DHCP6_CLIENT_H */
