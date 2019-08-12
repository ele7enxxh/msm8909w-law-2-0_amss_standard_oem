#ifndef PS_DNSI_RESOLVER_H
#define PS_DNSI_RESOLVER_H

/*===========================================================================

             R E S O L V E R   M O D U L E   H E A D E R   F I L E

DESCRIPTION
  The Data Services DNS Subsystem Resolver module header file. Contains
  declarations of functions, variables, macros, structs and enums
  used by DNS subsystem resolver.

  The following functions are exported to the DNS subsystem.

EXTERNALIZED FUNCTIONS
  ps_dnsi_resolve()
    This function takes the query type, query class and query data, and
    returns query identifier corresponding to the query.

INTIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"


#include "dssdns.h"
#include "ps_dnsi_defs.h"
#include "dss_net_mgr.h"
#include "ps_dnsi_io_mgr.h"
#include "dssinternal.h"
#include "pstimer.h"
#include "dss_dns_config.h"



/*===========================================================================

                               MACRO DECLARATIONS

===========================================================================*/
#define DNS_LOOKUP_PORT            53


/*===========================================================================

                                DATA STRUCTURES

===========================================================================*/

/*--------------------------------------------------------------------------
  Resolver events and errors. Includes events from IO manager, network
  manager and resolver timeout callbacks. The events are only processed
  only if the resolver is in certain states. Otherwise they are ignored.

  The errors are processed in PS_DNSI_RESOLVER_STATE_FAIL.

  ---------------------------------------------------------------------------
  Event               State
  ---------------------------------------------------------------------------
  IO_ERROR_EVENT      IO_WAIT
  IO_READ_EVENT       IO_WAIT
  NET_ERROR_EVENT     NETWORK_WAIT, IO_WAIT
  NET_UP_EVENT        NETWORK_WAIT
  TIMEOUT_EVENT       NETWORK_WAIT, IO_WAIT
  other error events  FAIL state.
  ---------------------------------------------------------------------------
---------------------------------------------------------------------------*/
typedef enum
{
  PS_DNSI_RESOLVER_EVENT_MIN                = 0,
  PS_DNSI_RESOLVER_EVENT_NONE               = PS_DNSI_RESOLVER_EVENT_MIN,
  PS_DNSI_RESOLVER_EVENT_IO_READ            = 1,
  PS_DNSI_RESOLVER_EVENT_NET_UP             = 2,

  /*------------------------------------------------------------------------
    Different types of error events that can occur.
  -------------------------------------------------------------------------*/
  PS_DNSI_RESOLVER_EVENT_ERROR_IO_MGR       = 3,
  PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR      = 4,
  PS_DNSI_RESOLVER_EVENT_ERROR_SESSION_MGR  = 5,
  PS_DNSI_RESOLVER_EVENT_ERROR_CACHE_MGR    = 6,
  PS_DNSI_RESOLVER_EVENT_ERROR_MSG_MGR_FORM_QUERY = 7,
  PS_DNSI_RESOLVER_EVENT_ERROR_MSG_MGR_PARSE_RESP = 8,
  PS_DNSI_RESOLVER_EVENT_ERROR_LOCAL_QUERY_FAILED = 9,
  PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN      = 10,
  PS_DNSI_RESOLVER_EVENT_MAX                = 11,
  PS_DNSI_RESOLVER_EVENT_FORCE_SIGNED_32_BIT = 0x7FFFFFFF

} ps_dnsi_resolver_event_enum_type;

/*--------------------------------------------------------------------------
  Resolver states

  The following are the states the resolver can be in at any point of time.
  MIN and MAX states are invalid states forming boundaries.

  START           - The first state the resolver starts off from.
  NETWORK_WAIT    - State where resolver is waiting for the network to open.
  NETWORK_OPEN    - After the network has been opened.
  IO_WAIT         - Waiting for response from IO manager.
  IO_COMPLETE     - IO operations are successful and complete.
  FAIL            - Failure state
  SUCCESS         - Success state.

  Some states are marked as dummy states because there are no events
  associated with those states.

  State Transition table
-----------------------------------------------------------------------------
  State             Where can we go from here?
-----------------------------------------------------------------------------
  MIN               N/A
-----------------------------------------------------------------------------
  START             1. SUCCESS – If cache find is successful.
  (dummy state)     2. NETWORK_OPEN – If config specifies to use given iface
                    3. NETWORK_OPEN - If call to net mgr returns success.
                    3. NETWORK_WAIT – If call to net mgr blocks.
                    4. FAIL – If net mgr bringup call fails immedately
-----------------------------------------------------------------------------
  NETWORK_WAIT      1. NETWORK_OPEN – If network was successfully opened.
                    2. FAIL – If network doesn't open (on NET_MGR_ERROR).
                    3. FAIL – If resolver’s network timer times out.
-----------------------------------------------------------------------------
  NETWORK_OPEN      1. IO_WAIT – On successful call to IO mgr to send query
  (dummy state)     2. FAIL – If IO manager returns failure immediately.
-----------------------------------------------------------------------------
  IO_WAIT           1. IO_COMPLETE – IO_MGR_READ_EVENT, if msg can be parsed.
                    2. FAIL - If reply message cannot be parsed.
                    3. FAIL – If IO manager returns error(IO_MGR_ERROR_EVENT)
                    4. FAIL – If Network Manager returns(NET_MGR_ERROR_EVENT)
                    5. FAIL – If Resolvers IO timer times out.
-----------------------------------------------------------------------------
  IO_COMPLETE       1. Add the results to cache and goto SUCCESS state.
  (dummy state)
-----------------------------------------------------------------------------
  FAIL              1. Network/General failure – Bringup network again
                    depending upon configuration and previous retries.
                    Go to NETWORK_WAIT or FAIL states.
                    2. IO failure/Timeout – Call IO manager again depending
                    on configuration and previous retries. Can go to SUCCESS
                    or FAIL states.
-----------------------------------------------------------------------------
  SUCCESS           1. Go to FREE state
-----------------------------------------------------------------------------
  FREE              1. In this state, the resolver can be freed or can be
                       reused to perform another query.
-----------------------------------------------------------------------------
  MAX               N/A
-----------------------------------------------------------------------------

---------------------------------------------------------------------------*/
typedef enum
{
  PS_DNSI_RESOLVER_STATE_MIN            = 0,
  PS_DNSI_RESOLVER_STATE_FREE           = PS_DNSI_RESOLVER_STATE_MIN,
  PS_DNSI_RESOLVER_STATE_OPEN           = 1,
  PS_DNSI_RESOLVER_STATE_START          = 2,
  PS_DNSI_RESOLVER_STATE_NETWORK_WAIT   = 3,
  PS_DNSI_RESOLVER_STATE_NETWORK_OPEN   = 4,
  PS_DNSI_RESOLVER_STATE_IO_WAIT        = 5,
  PS_DNSI_RESOLVER_STATE_IO_COMPLETE    = 6,
  PS_DNSI_RESOLVER_STATE_FAIL           = 7,
  PS_DNSI_RESOLVER_STATE_SUCCESS        = 8,
  PS_DNSI_RESOLVER_STATE_ALIAS          = 9,
  PS_DNSI_RESOLVER_STATE_MAX            = 10,
  PS_DNSI_RESOLVER_STATE_FORCE_SIGNED_32_BIT = 0x7FFFFFFF
} ps_dnsi_resolver_state_enum_type;

/*--------------------------------------------------------------------------
  Resolver server type index.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_DNSI_SERVER_TYPE_PRIMARY = 0,
  PS_DNSI_SERVER_TYPE_SECONDARY = 1,
  PS_DNSI_SERVER_TYPE_MAX_IFACE_SERVERS
} ps_dnsi_server_type_enum_type;


typedef enum
{
  PS_DNSI_RES_NOTIFY_SUCCESS,
  PS_DNSI_RES_NOTIFY_FAILURE
} ps_dnsi_resolver_notify_event_enum_type;

/*---------------------------------------------------------------------------
  Typedef for API callback function type
---------------------------------------------------------------------------*/
typedef void (*ps_dnsi_resolver_notify_f_type)
(
  ps_dnsi_resolver_notify_event_enum_type         event,
  ps_dnsi_resolver_handle_type                    res_handle,
  q_type                                        * answer_q,
  void                                          * user_data_ptr,
  int16                                           ps_errno
);

/*--------------------------------------------------------------------------
  Resolver Control block.

  handle            - Handle to identify the resolver control block.
  session_handle    - Session control block handle. Used to session
                      configuration information such as iface_id, routable
                      iface mode, whether to use cache results etc.
  state             - State of the resolver
  event             - Event or error that occurred is stored here.
  server_index      - Primary or secondary DNS server address
  query_type        - Type of query.
  query_class       - Class of query.
  query_data_ptr    - Query data.
  query_id          - Query identifier returned by message manager.
  cback_f_ptr       - API callback function pointer.
  user_data_ptr     - API callback function user data.
  net_mgr_handle    - Network manager handle.
  netlib_handle     - The handle of the netlib used by the resolver for iface
                      events registration.
  net_retries       - Indicates current iteration of number of retries
                      to bring up the network.
  iface_id          - Interface id on which the query should be sent.
  iface_id_for_registration - Interface id used for events registration.
  net_policy        - Network policy to use to send the query.
  io_mgr_handle     - IO manager handle.
  io_retries        - Indicates current iteration of number of retries
                      to the IO manager.
  pstimer           - PS timer for timing network and IO manager timeouts.
  query_msg         - DSM item pointer to the formed query message.
                      Stored in order not to repeat the construction of
                      query message for ever IO retransmission.
  result_q          - Queue of result RRs
---------------------------------------------------------------------------*/
typedef struct ps_dnsi_resolver_cb_s_type
{
  q_link_type                           link;
  ps_dnsi_resolver_handle_type          handle;
  dss_dns_session_mgr_handle_type       session_handle;
  ps_dnsi_resolver_state_enum_type      state;
  ps_dnsi_resolver_event_enum_type      event;
  uint16                                server_index;
  uint32                                query_id[DSS_DNS_DEFAULT_MAX_QUERY_RETRIES+1];
  boolean                               query_id_valid[DSS_DNS_DEFAULT_MAX_QUERY_RETRIES+1];
  uint32                                max_query_index;
  uint16                                max_pending_servers_index;
  struct ps_sockaddr_storage            pending_servers[DSS_DNS_DEFAULT_MAX_QUERY_RETRIES+1];
  ps_dnsi_query_type_enum_type          query_type;
  ps_dnsi_query_class_enum_type         query_class;
  char                                * query_data_ptr;
  ps_dnsi_resolver_notify_f_type        cback_f_ptr;
  void                                * user_data_ptr;
  int32                                 net_mgr_handle;
  int16                                 netlib_handle;
  dss_iface_id_type                     iface_id;
  dss_iface_id_type                     iface_id_for_registration;
  dss_net_policy_info_type              net_policy;
  uint16                                net_retries;
  ps_dnsi_io_mgr_handle_type            io_mgr_handle;
  uint16                                io_retries;
  ps_timer_handle_type                  pstimer;
  dsm_item_type                       * query_msg;
  dsm_item_type                       * dns_response_ptr;
  q_type                                result_q;
} ps_dnsi_resolver_cb_type;




/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION  PS_DNSI_RESOLVER_INIT

DESCRIPTION
  Initializes the Resolver module during powerup.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void ps_dnsi_resolver_init ( void );

/*===========================================================================
FUNCTION  PS_DNSI_RESOLVER_GET_INSTANCE()

DESCRIPTION
  Gets a free resolver instance in the current session if any.
  Otherwise tries to create a new resolver instance and returns its handle.

PARAMETERS
  session_handle  - Session control block handle.
  errno           - Error code in case of error.

RETURN VALUE
  Handle to the resolver control block on success.
  PS_DNSI_RESOLVER_INVALID_HANDLE on error.

  errno values
  ------------
  DS_ENOMEM if the resolver instance cannot be created.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
ps_dnsi_resolver_handle_type ps_dnsi_resolver_get_instance
(
  dss_dns_session_mgr_handle_type   session_handle,
  int16                           * ps_errno
);

/*===========================================================================
FUNCTION  PS_DNSI_RESOLVER_START_QUERY

DESCRIPTION
  Starts a DNS query corresponding through a resolver instance.

PARAMETERS
  res_handle             - Resolver instance identifier.
  query_type             - Type of query
  query_class            - Class of query
  query_data_ptr         - Query data pointer
  user_data_ptr          - Callback function user data.
  errno                  - Error code in case of error.

DEPENDENCIES
  The caller must have created a resolver instance.
  Resolver state must be PS_DNSI_RESOLVER_STATE_FREE.

RETURN VALUE
  Always returns DSS_ERROR
    If errno is DS_EWOULDBLOCK, it indicates that the results would be
      returned through the registered callback.
    Else there is an irrecoverable error and resolver control block
      should be freed using ps_dnsi_resolver_delete_instance().

  errno values
  ------------
  DS_EWOULDBLOCK  - The call would block.
  DS_EFAULT       - Invalid arguments.
  DS_ESYSTEM      - Other system error.

SIDE EFFECTS
  None.
===========================================================================*/
int16 ps_dnsi_resolver_start_query
(
  ps_dnsi_resolver_handle_type      res_handle,
  ps_dnsi_query_type_enum_type      query_type,
  ps_dnsi_query_class_enum_type     query_class,
  char                            * query_data_ptr,
  ps_dnsi_resolver_notify_f_type    cback_f_ptr,
  void                            * user_data_ptr,
  int16                           * ps_errno
);

/*===========================================================================
FUNCTION PS_DNSI_RESOLVER_DELETE_INSTANCE

DESCRIPTION
  Frees the resolver control block. Also closes the network manager and
  IO manager associated with the resolver control block.

PARAMETERS
  res_handle  - Handle to the resolver control block.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Aborts all queries in progress.
===========================================================================*/
void ps_dnsi_resolver_delete_instance
(
  ps_dnsi_resolver_handle_type   res_handle
);

/*===========================================================================
FUNCTION PS_DNSI_RESOLVER_DELETE_ALL_INSTANCES

DESCRIPTION
  Deletes all the resolver instances associated with a certain session.

PARAMETERS
  session_handle    - Session identifier.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Aborts all queries in progress.
===========================================================================*/
void ps_dnsi_resolver_delete_all_instances
(
  dss_dns_session_mgr_handle_type     session_handle
);

#endif /* PS_DNSI_RESOLVER_H */

