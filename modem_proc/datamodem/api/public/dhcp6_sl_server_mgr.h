#ifndef DHCP6_SL_SERVER_MGR_H
#define DHCP6_SL_SERVER_MGR_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D H C P 6 _ S L _ S E R V E R _ M G R . H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/**
  @file dhcp6_sl_server_mgr.h
  @brief Contains the public function to interface with the DHCPv6 Stateless Server.
*/
/*
  INITIALIZATION AND SEQUENCING REQUIREMENTS
  - The DHCPv6 Stateless Server is started by calling dhcp6_sl_server_start().  
  - The DHCPv6 Stateless Server is stoped by calling dhcp6_sl_server_stop(). 
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
Copyright (c) 2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE
                           
$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dhcp6_sl_server_mgr.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/28/11    cp     Initial Revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ps_iface.h"
#include "dhcp6_common_defs.h"

/*===========================================================================

                      DEFINITIONS AND DECLARATIONS

===========================================================================*/
/* group: dhcp */

/** @addtogroup constants 
  @{
*/
/**
  Mask to indicate DNS addresses to be updated.
*/
#define DHCP6_CONFIG_UPDATE_DNS_ADDRS 0x1

/**
  Mask to indicate DNS domain names to be updated.
*/
#define DHCP6_CONFIG_UPDATE_DNS_DOMAIN_NAMES 0x2

/**
  Mask to indicate SIP addresses to be updated.
*/
#define DHCP6_CONFIG_UPDATE_SIP_ADDRS 0x4

/**
  Mask to indicate SIP domain names to be updated.
*/
#define DHCP6_CONFIG_UPDATE_SIP_DOMAIN_NAMES 0x8
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes 
  @{
*/
/**
  @brief Server DUID value.
*/
typedef struct
{
  uint8  server_duid[DHCP6_MAX_DUID_LENGTH]; /**< Server DUID. */
  uint32  server_duid_len; /**< Server DUID Length. */
}dhcp6_sl_server_duid;

/**
  @brief Configurable Parameters that control DHCPv6 Stateless Server behavior.
*/
typedef struct 
{
  ip_addr_type  * dns_serv_addr_list;
  /**< DNS server addresses. */
  uint32            dns_addr_count;
  /**< Number of DNS addresses present in the list. */
  ps_iface_domain_name_type  * domain_name_search_list;
  /**< DNS servers domain name list. */
  uint32            dns_domain_name_count;
  /**< Number of DNS Domain names present in the list. */
  ip_addr_type  * sip_serv_addr_list;
  /**< SIP server addresses. */
  uint32            sip_addr_count;
  /**< Number of SIP addresses present in the list. */
  ps_iface_domain_name_type  * sip_serv_domain_name_list;
  /**< SIP servers domain name list. */
  uint32            sip_domain_name_count;
  /**< Number of SIP Domain names present in the list. */
} dhcp6_sl_server_config_params;
/** @} */ /* end_addtogroup datatypes */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DHCP6_SL_SERVER_MGR_START()
===========================================================================*/
/** @ingroup dhcp6_sl_server_mgr_start
  Serializes the start of a DHCP6 server through the PS task. This causes the 
  DHCP6 server to be configured for the client with the provided configuration 
  parameters and the resulting handle to be returned in a callback also 
  specified with user data.

  @datatypes
  #ps_iface_type\n
  #dhcp6_sl_server_duid\n
  #dhcp6_sl_server_config_params

  @vertspace
  @param[in] client_iface    Interface for which the DHCP6 is serving configuration params.
  @param[in] gw_iface       The interface from which the configuration params are obtained.
  @params[in] server_duid  Server DUID.
  @param[in] config_vals    DHCP6 configuration parameters. if confg_vals is NULL, then
                            configuration parameters are taken from the gw_iface.
  @param[in] done_cb        Callback function that is called when the DHCP6 server has 
                            started. This may be called before this
                            function has returned. The handle is NULL 
                            on failure. Non-null on success. The userdata 
                            parameter is also passed to this function.

  @param[in] userdata      User data that will be passed to done_cb.

  @return
  TRUE -- Message successfully sent to the PS task. Expect a callback.\n
  FALSE -- Message not set to the PS task. Do not expect a callback.

  @dependencies
  The client interface must be up.
  
  @sideeffects
  Causes a message to be sent to the PS task to start the DHCP6 server.
  @newpage
*/
boolean
dhcp6_sl_server_mgr_start
( 
  ps_iface_type * client_iface,
  ps_iface_type * gw_iface,
  dhcp6_sl_server_duid *server_duid,
  dhcp6_sl_server_config_params * config_vals,
  void (*done_cb)(void * userdata, void * handle),
  void * userdata
);

/*===========================================================================
FUNCTION DHCP6_SL_SERVER_MGR_STOP()
===========================================================================*/
/** @ingroup dhcp6_sl_server_mgr_stop
  Serializes the stop of a DHCP6 server through the PS task. This causes the 
  specified DHCP6 server to be freed. The callback is called back with the 
  userdata when this is done. The client handle_ptr is set to NULL before 
  this function returns.

  @param[in] handle_ptr   Handle to free. 
  @param[in] done_cb()    Function to call when done freeing this handle.
                          The function is not called when the callback is NULL.
  @param[in] userdata     Pointer to the User-specifed data to be passed back
                          when the callback is called.

  @return
  TRUE -- Message was successfully sent to the PS task. Expect a callback.\n
  FALSE -- Message was not not set to the PS task. Do not expect a callback.

  @dependencies
  The handle must be allocated by a call to dhcp6_sl_server_mgr_start() before this 
  function is called.

  @sideeffects
  A message is sent to the PS task to stop the DHCP6 server specifed by the 
  handle passed in.
  @newpage
*/
boolean
dhcp6_sl_server_mgr_stop
(
  void ** handle_ptr,
  void (*done_cb)(void * userdata, boolean ok),
  void * userdata
);

/*===========================================================================
FUNCTION DHCP6_SL_SERVER_MGR_UPDATE_PARAMS()
===========================================================================*/
/** @ingroup dhcp6_sl_server_mgr_update_params
  Serializes the updating of config params of a DHCP6 server through the PS task. 
  This causes the configuration params to be updated at the DHCP6 server. 
  The callback is called back with the userdata when this is done. 

  @datatypes
  #dhcp6_sl_server_config_params

  @vertspace
  @param[in] handle_ptr   Handle to the dhcpv6 server for which the config 
                          params need to be updated.
  @param[in] update_mask  Mask indicating the config params to be updated.
  @param[in] config_vals  DHCP6 configuration parameters to be
                          updated at the server.
  @param[in] done_cb()    Function to call when done updating the params.
                          The function is not called when the callback is NULL.
  @param[in] userdata     Pointer to the User-specifed data to be passed back
                          when the callback is called.

  @return
  TRUE -- Message was successfully sent to the PS task. Expect a callback.\n
  FALSE -- Message was not not set to the PS task. Do not expect a callback.

  @dependencies
  The handle must be allocated by a call to dhcp6_sl_server_mgr_start() before this 
  function is called.

  @sideeffects
  A message is sent to the PS task to update the params specifed by the 
  handle passed in.
  @newpage
*/
boolean
dhcp6_sl_server_mgr_update_params
(
  void * handle_ptr,
  uint32 update_mask,
  dhcp6_sl_server_config_params * new_config_vals,
  void (*done_cb)(void * userdata, boolean ok),
  void * userdata
);
#endif /* DHCP6_SL_SERVER_MGR_H */
