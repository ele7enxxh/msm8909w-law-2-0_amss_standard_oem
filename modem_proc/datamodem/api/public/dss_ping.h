#ifndef DSS_PING_H
#define DSS_PING_H
/*===========================================================================

         D A T A   S E R V I C E S   P I N G   H E A D E R  F I L E

DESCRIPTION

 This header file contains shared variables declarations for functions
 related to the ping API.


Copyright (c) 2006-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dss_ping.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/22/11    kr     Changes all memeber types to uint32
06/26/09    am     Fixed alignment problems for ttl in ping_config.
11/24/08    am     High/Medium lint fixes.
10/24/08    am     Fixed compiler warnings for off-target.
01/22/07    ss     Created.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "dss_netpolicy.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define   DSS_PING_RESOLVED_IP_ADDR_BUF_SIZE  64

#define   DSS_PING_INVALID_HANDLE 0

typedef   int32  dss_ping_handle;

typedef enum
{
  DSS_PING_IPV4_ADDR = 1,
  DSS_PING_IPV6_ADDR = 2
} dss_ping_ip_addr_enum_type;

/*---------------------------------------------------------------------------
  Indicates the reason for closure of the ping session
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_PING_USER_REQ         = 1, /* Session closed at user's request      */
  DSS_PING_SESSION_COMPLETE = 2, /* Session closed normally               */
  DSS_PING_ERROR            = 3, /* Session closed because of some error  */
  DSS_PING_DNS_ERROR        = 4  /* DNS error */
} dss_ping_session_close_reason_type;

/*---------------------------------------------------------------------------
  This structure is used to indicate the ping options.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  num_data_bytes;          /* Data byte size of the ping packet    */
  uint32  ping_interval_time;      /* Interval between each ping, ms       */
  uint32  ping_response_time_out;  /* Wait time for ping response, ms      */
  uint32  cookie;                  /* Internal field                       */
  uint32  num_pings;               /* Number of times to ping              */
  uint32  ttl;                     /* Time To Live for the ping packet     */
} dss_ping_config_type;

/*---------------------------------------------------------------------------
  This structure is used to indicate the ping statistics. It is used
  during callbacks to the app to notify about the result of each ping.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  rtt;  /* Round Trip Time (RTT) for the ping, millisecs  */
  uint16  icmp_seq_num;
  char   resolved_ip_addr[DSS_PING_RESOLVED_IP_ADDR_BUF_SIZE];
} dss_ping_stats_type;

/*---------------------------------------------------------------------------
  This structure is used to indicate the ping statistics for the entire ping
  session. It is used during the final callback to the app.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  min_rtt;  /* Minimum RTT so far, in millisecs           */
  uint32  max_rtt;  /* Maximum RTT so far, in millisecs           */
  uint32  avg_rtt;  /* Average RTT so far, in millisecs           */
  uint32  num_pkts_sent;  /* Number of pings sent so far          */
  uint32  num_pkts_recvd; /* Number of responses recieved so far  */
  uint32  num_pkts_lost;  /* Number of responses not received     */
} dss_ping_session_stats_type;


/*---------------------------------------------------------------------------
  The callback function that is used to notify the app about result of each
  ping. If a timeout occured, this indicates a timeout error. Otherwise, if
  a response was received, it indicates the statistics for the ping (ICMP
  seq number, RTT ect).
---------------------------------------------------------------------------*/
typedef void (*dss_ping_callback_fn_type)
(
  dss_ping_handle       ping_handle,  /* Ping handle                       */
  dss_ping_stats_type   *ping_stats,  /* Statistics for this ping response */
  void                  *user_data,   /* User data                         */
  int16                 dss_errno     /* DS_ETIMEDOUT or DSS_SUCCESS       */
);

/*---------------------------------------------------------------------------
  The callback function is always called once the ping session is closed.
  The session could be closed due to: some internal error (e.g. network down),
  user's request, or normal user-uninterrupted closure (all ping responses
  were received/timed out).
---------------------------------------------------------------------------*/
typedef void (*dss_ping_sess_summary_callback_fn_type)
(
  dss_ping_handle                     ping_handle, /* Ping handle          */
  dss_ping_session_stats_type         *ping_session_stats, /* Ping summary */
  void                                *user_data,  /* User data            */
  dss_ping_session_close_reason_type  reason,      /* Why sess ended       */
  int16                               dss_errno    /* Error number         */
);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

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
);



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
  the error condition in dss_errno.


SIDE EFFECTS
  None.
===========================================================================*/
dss_ping_handle dss_ping_start
(
  dss_net_policy_info_type                *net_policy,
  char                                    *dest_addr,
  dss_ping_ip_addr_enum_type              dest_addr_type,
  dss_ping_config_type                    *app_ping_options,
  dss_ping_callback_fn_type               app_callback_fn,
  dss_ping_sess_summary_callback_fn_type  app_sess_summary_callback_fn,
  void                                    *app_user_data,
  int16                                   *dss_errno
);

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
);
#endif /* DSS_PING_H */
