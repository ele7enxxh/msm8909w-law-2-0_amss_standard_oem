#ifndef DSS_PING_COMM_MGR_H
#define DSS_PING_COMM_MGR_H
/*===========================================================================

         P I N G   C O M M U N I C A T I O N   M A N A G E R   
                         H E A D E R   F I L E
                   
DESCRIPTION

 This header file manages communication functions related to ping - 
 creating and closing sockets, sending pings and reading the responses.

Copyright (c) 2006-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/inc/dss_ping_comm_mgr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/22/07    ss     Created.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ps_iface_defs.h"
#include "ps_svc.h"
#include "pstimer.h"
#include "dssocket_defs.h"
#include "list.h"
#include "dss_ping.h"
#include "dssdns.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_INIT_PING_ENGINE

DESCRIPTION
  This function initializes the ping session info structures, and sets
  the command handler for handling outgoing pings.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void dss_ping_comm_mgr_init_ping_engine
(
  void
);

/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_SEND_PING_CMD_HANDLER

DESCRIPTION
  This function handles the PS_DSS_PING_SEND_PING_CMD command.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/

void dss_ping_comm_mgr_send_ping_cmd_handler
(
  ps_cmd_enum_type  cmd,
  void              *user_data_ptr
);

/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_NET_CB_CMD_HANDLER

DESCRIPTION
  This function handles the PS_DSS_PING_NET_CB_CMD command.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void dss_ping_comm_mgr_net_cb_cmd_handler
(
  ps_cmd_enum_type  cmd,
  void              *user_data_ptr
);

/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_SOCK_CB_CMD_HANDLER

DESCRIPTION
  This function handles the PS_DSS_PING_SOCK_CB_CMD command.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void dss_ping_comm_mgr_sock_cb_cmd_handler
(
  ps_cmd_enum_type  cmd,
  void              *user_data_ptr
);

/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_RSP_TIMEOUT_CB_CMD_HANDLER

DESCRIPTION
  This function handles the PS_DSS_PING_RSP_TIMEOUT_CB_CMD command.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void dss_ping_comm_mgr_ping_rsp_timeout_cb_cmd_handler
(
  ps_cmd_enum_type  cmd,
  void              *timeout_data
);


/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_CREATE_PING_SESSION

DESCRIPTION
  This function creates a ping session. Ping retransmit and ping
  response timeout handles are created. No sockets are created here.

DEPENDENCIES
  None.

RETURN VALUE
  On success, a valid handle is returned.
  On error, NULL is returned and the error condition value is placed in 
  *errno_ptr. 
  
  errno_ptr Values
  ----------------
  DS_EFAULT           bad memory address
  DS_ENOMEM           out of memory

SIDE EFFECTS
  None.
===========================================================================*/
dss_ping_handle dss_ping_comm_mgr_create_ping_session
(
  dss_net_policy_info_type                *net_policy_ptr,
  dss_ping_config_type                    *user_ping_options,
  dss_ping_callback_fn_type               app_callback_fn,
  dss_ping_sess_summary_callback_fn_type  app_ping_summary_callback_fn,
  void                                    *app_user_data,
  int16                                   *dss_errno
);

/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_ABORT_PING

DESCRIPTION
  This function ends the ping session. If a non-NULL reason_ptr and non-NULL
  errno_ptr are specified, it also calls the application's summary callback 
  function before bringing down the ping session.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS if session was closed successfully.
  DSS_ERROR if an invalid handle was specified.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_ping_comm_mgr_abort_ping
(
  dss_ping_handle                     ping_handle,
  dss_ping_session_close_reason_type  *reason_ptr,
  int16                               dss_errno
);

/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_SEND_PING

DESCRIPTION
  This function sends out an ICMP ECHO_REQUEST packet. It then starts a 
  retransmit timer. When this timer expires, the next ping will be sent.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void dss_ping_comm_mgr_send_ping
(
  dss_ping_handle   ping_handle
);


/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_UPDATE_PING_DEST_ADDR

DESCRIPTION
  This function updates the destination ping address for the specified 
  ping session (indicated through ping_handle) with the information contained 
  in the dss_dns_addrinfo structure.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void dss_ping_comm_mgr_update_ping_dest_addr
(
  dss_ping_handle     ping_handle,
  struct dss_dns_addrinfo  *addrinfo_ptr
);

/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_PERFORM_DNS_LOOKUP

DESCRIPTION
  This function performs the DNS lookup.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
int32 dss_ping_comm_mgr_perform_dns_lookup
(
  dss_ping_handle             ping_handle,
  char *                      dest_addr_ptr,
  int16                       addr_family,
  int16 *                     dss_errno
);


#endif /* DSS_PING_COMM_MGR_H */
