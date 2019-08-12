/*===========================================================================

                             D S S _ P I N G . C

DESCRIPTION
 The Data Services Ping API File. Contains API functions for pinging a
 remote destination.

EXTERNALIZED FUNCTIONS
  dss_ping_init_options()
    Initializes the ping options' structure to default values.
  dss_ping_start()
    Ping the remote destination with the user specified ping options.
  dss_ping_stop()
    Stop pinging. Can be used to stop an ongoing ping session.

Copyright (c) 2007-2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/dss_ping.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
24/11/08    am     High/Medium lint fixes.
10/24/08    am     Fixed compiler warnings for off-target.
04/29/07   ss      Added ping6 support/fixed issues found in testing
04/03/07   ss/msr  Fixed issues found in testing
01/22/07    ss     Created.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "msg.h"
#include "dss_ping.h"
#include "dssdns.h"
#include "dss_ping_config.h"
#include "dss_ping_comm_mgr.h"
#include "ds_Utils_DebugMsg.h"


/*===========================================================================

                          DATA DECLARATIONS

===========================================================================*/



/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSS_PING_INIT_OPTIONS

DESCRIPTION
  This function initializes the ping options (e.g, number of times to ping
  the destination) to default values.

DEPENDENCIES
  None.

RETURN VALUE
  On success, return DSS_SUCCESS. On error, return DSS_ERROR.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_ping_init_options
(
  dss_ping_config_type  *ping_configs
)
{
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("dss_ping_init_options(): "
    "caller 0x%p", caller_ptr);

  if( ping_configs == NULL )
  {
    LOG_MSG_INFO1_0("dss_ping_init_options(): "
                    "NULL pointer");
    return DSS_ERROR;
  }

  /* Initialize to default values */
  ping_configs->num_pings           = DSS_PING_DEFAULT_TIMES_TO_PING;
  ping_configs->num_data_bytes      = DSS_PING_DEFAULT_PING_DATA_BYTES;
  ping_configs->ttl                 = DSS_PING_DEFAULT_TTL;
  ping_configs->ping_interval_time  = DSS_PING_DEFAULT_PING_RETRANSMIT_TIME;
  ping_configs->ping_response_time_out  =
    DSS_PING_DEFAULT_PING_RESPONSE_TIME_OUT;
  ping_configs->cookie              = DSS_PING_COOKIE;

  return DSS_SUCCESS;
} /* dss_ping_init_options() */


/*===========================================================================
FUNCTION DSS_PING_START()

DESCRIPTION
 This function is used to ping the specified IP address. It is non-blocking.
 The user is notified of the ping results through the user-specified
 callback function.

DEPENDENCIES
  The ping options are specified through the dss_ping_config_type structure.
  So, before invoking the dss_ping() API, the user MUST initialize the
  dss_ping_config_type structure by calling dss_ping_init_options().
  Once the structure has been initialized with default ping options, the user
  can overwrite these default options with her own specific options.

RETURN VALUE
  On success, return a valid ping handle. On error, return DSS_ERROR and place
  the error condition in errno.


SIDE EFFECTS
  None.
===========================================================================*/
dss_ping_handle dss_ping_start
(
  dss_net_policy_info_type                *net_policy_ptr,
  char                                    *dest_addr_ptr,
  dss_ping_ip_addr_enum_type              dest_addr_type,
  dss_ping_config_type                    *app_ping_options,
  dss_ping_callback_fn_type               app_callback_fn,
  dss_ping_sess_summary_callback_fn_type  app_sess_summary_callback_fn,
  void                                    *app_user_data,
  int16                                   *dss_errno
)
{
  dss_ping_handle                         ping_handle;
  int16                                   addr_family;
  int32                                   ret_val;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("dss_ping_start(): "
    "caller 0x%p", caller_ptr);

  LOG_MSG_INFO2_4("dss_ping_start(): Ping req recvd params,"
                  " dest addr  0x%x dss_errno 0x%x "
                   "DestAddrType %d ping options 0x%x",
                  dest_addr_ptr, dss_errno, dest_addr_type, app_ping_options);

  if( dss_errno == NULL )
  {
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  if( dest_addr_ptr == NULL)
  {
    *dss_errno = DS_EADDRREQ;
    return(DSS_ERROR);
  }

  if( dest_addr_type != DSS_PING_IPV4_ADDR && dest_addr_type != DSS_PING_IPV6_ADDR )
  {
    *dss_errno = DS_EFAULT;
    return(DSS_ERROR);
  }

  if( app_ping_options == NULL )
  {
    *dss_errno = DS_EFAULT;
    return(DSS_ERROR);
  }

  LOG_MSG_INFO2_4("dss_ping_start(): Ping req recvd params,"
                  " App CB func 0x%x App Sess Summ CB func 0x%x "
                   "cookie %d Num of bytes %d",
                  app_callback_fn, app_sess_summary_callback_fn,
                  app_ping_options->cookie, app_ping_options->num_data_bytes);
  
  if( app_callback_fn == NULL || app_sess_summary_callback_fn == NULL )
  {
    *dss_errno = DS_EFAULT;
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Verify that ping options structure is initialized, and that all
    ping options are within valid range.
  -------------------------------------------------------------------------*/
  if( app_ping_options->cookie != DSS_PING_COOKIE )
  {
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if( app_ping_options->num_data_bytes < DSS_PING_MIN_PING_DATA_BYTES ||
      app_ping_options->num_data_bytes > DSS_PING_MAX_PING_DATA_BYTES )
  {
    *dss_errno = DS_EMSGSIZE;
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Create a ping session
  -------------------------------------------------------------------------*/
  if( (ping_handle =
       dss_ping_comm_mgr_create_ping_session( net_policy_ptr,
                                              app_ping_options,
                                              app_callback_fn,
                                              app_sess_summary_callback_fn,
                                              app_user_data,
                                              dss_errno ) ) ==
       DSS_PING_INVALID_HANDLE )
  {
    LOG_MSG_ERROR_0("dss_ping_start(): "
                    "Unable to create ping session");
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Perform DNS lookup 
  -------------------------------------------------------------------------*/
  addr_family = 
    (dest_addr_type == DSS_PING_IPV4_ADDR) ? DSS_AF_INET : DSS_AF_INET6;
  
  ret_val = dss_ping_comm_mgr_perform_dns_lookup (ping_handle, 
                                                  dest_addr_ptr, 
                                                  addr_family, 
                                                  dss_errno);
  if (DSS_SUCCESS != ret_val)
  {
    LOG_MSG_ERROR_1 ("dss_ping_start(): "
                     "DNS lookup failed err %d", *dss_errno);
    (void) dss_ping_comm_mgr_abort_ping (ping_handle, 
                                         NULL,
                                         DSS_SUCCESS);
    return DSS_ERROR;
  }

  return ping_handle;

} /* dss_ping_start() */


/*===========================================================================
FUNCTION DSS_PING_STOP()

DESCRIPTION
 Stop pinging and close the ping session.

RETURN VALUE
  DSS_SUCCESS: if a correct ping_handle was specified.
  DSS_ERROR: if an incorrect ping_handle was specified.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_ping_stop
(
  dss_ping_handle  ping_handle
)
{
  dss_ping_session_close_reason_type  reason;
  int16                               dss_errno;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("dss_ping_stop(): "
    "caller 0x%p", caller_ptr);

  LOG_MSG_INFO1_1("dss_ping_stop(): "
                  "Ping sess user abort handle 0x%x", ping_handle);

  if( ping_handle == DSS_PING_INVALID_HANDLE )
  {
    return DSS_ERROR;
  }

  reason = DSS_PING_USER_REQ;
  dss_errno  = DSS_SUCCESS;
  return dss_ping_comm_mgr_abort_ping(ping_handle, &reason, dss_errno);
} /* dss_ping_stop() */


