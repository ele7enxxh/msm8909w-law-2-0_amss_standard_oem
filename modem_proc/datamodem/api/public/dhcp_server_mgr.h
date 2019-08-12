#ifndef DHCP_SERVER_MGR_H
#define DHCP_SERVER_MGR_H
/*===========================================================================

                           D H C P _ S E R V E R _ M G R . H

=============================================================================*/

/**
  @file dhcp_server_mgr.h
  @brief Contains the public function to interface with the DHCP server for 
   SoftAp functionality only.
*/

/*
  INITIALIZATION AND SEQUENCING REQUIREMENTS

  dhcp_server_mgr_start()
    Serializes the start of a DHCP server through the PS task.

  dhcp_server_mgr_stop()
    Serializes the stop of a DHCP server through the PS task.

  dhcp_server_get_conn_info() 
    Returns the information about the devices which are currently connected.

  dhcp_server_release_conn()
    Updates the lease information associated with the client which is disconnected. 

  dhcp_server_init_params()
    Returns the default DHCP server configuration values.
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
Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
05/05/11    PT     (Tech Pubs) Edited Doxygen markup and comments. 
07/02/10    cp     Moved the declarations and definitions to 
                   dhcp_server_defs.h. Removed ARP cache callback mechanism.
03/18/10    cp     Initial development work done.  Added comments.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ps_iface.h"
#include "dhcp_server_defs.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/* group: dhcp */

/*===========================================================================
FUNCTION DHCP_SERVER_MGR_START()
===========================================================================*/
/** @ingroup dhcp_server_mgr_start
  Serializes the start of a DHCP server through the PS task. This causes the 
  DHCP server to be configured for the client with the provided configuration 
  parameters and the resulting handle to be returned in a callback also 
  specified with user data.

  @datatypes
  #ps_iface_type\n
  #dhcp_server_config_params_s\n
  #dhcp_msg_cback_type

  @vertspace
  @param[in] client        Interface for which the DHCP is serving addresses.
  @param[in] config_vals   DHCP configuration parameters.
  @param[in] done_cb       Callback function that is called when the DHCP has 
                           started. This may be called before this
                           function has returned. The handle is NULL 
                           on failure and non-NULL on success. The userdata 
						   parameter is also passed to this function.
  @param[in] msg_cb        Callback invoked on various DHCP messages being
                           received/transmitted.
  @param[in] userdata      Pointer to the user data passed to the done_cb 
                           parameter.

  @return
  TRUE -- Message successfully sent to the PS task. Expect a callback.\n
  FALSE -- Message not set to the PS task. Do not expect a callback.

  @dependencies
  The client interface must be up.
  
  @sideeffects
  Causes a message to be sent to the PS task to start the DHCP server.
  @newpage
*/
boolean
dhcp_server_mgr_start
( 
  ps_iface_type * client, 
  dhcp_server_config_params_s  *config_vals,
  void (*done_cb)(void * userdata, void * handle),
  dhcp_msg_cback_type msg_cb,
  void * userdata
);

/*===========================================================================
FUNCTION DHCP_SERVER_MGR_STOP()
===========================================================================*/
/** @ingroup dhcp_server_mgr_stop
  Serializes the stop of a DHCP server through the PS task. This causes the 
  specified DHCP server to be freed. The callback is called back with the 
  userdata when this is done. The client handle_ptr is set to NULL before 
  this function returns.

  @param[in] handle_ptr   Handle to free. 
  @param[in] done_cb()    Function to call when done freeing this handle.
                          The function is not called when the callback is NULL.
  @param[in] userdata     Pointer to the user-specifed data to be passed back
                          when the callback is called.

  @return
  TRUE -- Message was successfully sent to the PS task. Expect a callback.\n
  FALSE -- Message was not not set to the PS task. Do not expect a callback.

  @dependencies
  The handle must be allocated by a call to dhcp_server_mgr_start() before this 
  function is called.

  @sideeffects
  A message is sent to the PS task to stop the DHCP server specifed by the 
  handle passed in.
  @newpage
*/
boolean
dhcp_server_mgr_stop
(
  void ** handle_ptr,
  void (*done_cb)(void * userdata, boolean ok),
  void * userdata
);

/*===========================================================================
FUNCTION DHCP_SERVER_GET_CONN_INFO()
===========================================================================*/
/** @ingroup dhcp_server_get_conn_info
  Gets the information about the currently connected devices.

  @datatypes
  #dhcp_server_conn_devices_info_s

  @vertspace
  @param[in] handle                DHCP server handle.
  @param[out] conn_devs            Contains information about the 
                                   connected devices.
  @param[in] num_conn_devs         Contains the required number of connected
                                   devices.

  @return
  @ge 0 -- Least of the actual connected devices and the required number of 
           connected devices.\n
  < 0   -- Operation failed.

  @dependencies
  The handle must be allocated by a call to dhcp_server_mgr_start() before this 
  function is called.\n
  Memory must be allocated for the required number of connected devices before 
  this function is called.

  @sideeffects
  An access violation may occur when insufficient memory is provided for the 
  conn_devs parameter.
  @newpage
*/
int32
dhcp_server_get_conn_info
(
  void *handle,
  dhcp_server_conn_devices_info_s * conn_devs,
  uint32 num_conn_devs
);

/*===========================================================================
FUNCTION DHCP_SERVER_RELEASE_CONN
===========================================================================*/
/** @ingroup dhcp_server_release_conn
  Updates the lease information associated with the client being disconnected 
  by the WLAN. 

  @datatypes
  #dhcp_server_conn_devices_info_s

  @vertspace
  @param[in] handle      DHCP server handle.
  @param[in] dev_info    Contains information about the connected devices.

  @return
  None.

  @dependencies
  The handle must be allocated by a call to the dhcp_server_mgr_start()
  function. 
  @newpage
*/
void
dhcp_server_release_conn
(
  void *handle,
  dhcp_server_conn_devices_info_s *dev_info
);

/*===========================================================================
FUNCTION DHCP_SERVER_INIT_PARAMS
===========================================================================*/
/** @ingroup dhcp_server_init_params
  Gets the default DHCP server configuration values.

  @datatypes
  #dhcp_server_config_params_s

  @vertspace
  @param[out] config_vals   Pointer to the default DHCP configuration values.

  @return
  @ge 0 -- Operation succeeded.\n
  < 0  -- Operation failed.

  @dependencies
  Memory must be allocated for config_vals before this function is called.
  @newpage
*/
int32
dhcp_server_init_params
(
  dhcp_server_config_params_s  *config_vals
);


#endif /* DHCP_SERVER_MGR_H */
