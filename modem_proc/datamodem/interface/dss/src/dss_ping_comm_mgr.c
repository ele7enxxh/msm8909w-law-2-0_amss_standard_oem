/*===========================================================================

                  D S S _ P I N G _ C O M M _ M G R . C

DESCRIPTION
 This file contains communication functions related to ping -
 creating and closing sockets, sending pings and reading the responses.


EXTERNALIZED FUNCTIONS

Copyright (c) 2006-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/dss_ping_comm_mgr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/18/15   skc     Fixed KW issues
08/02/12    kr     KW fixes
11/21/11    pgm    Added dss ping critical section.
01/28/10    ss     Critical sections added to avoid simultaneous access to
                   ping abort by PS and UI tasks.
01/15/10    ss     Allowing multiple ping sessions.
                   Preventing ping abort when send operation blocks.
                   Bug fix regarding invalid response timeout timer restart
                   value.
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
05/14/09    pp     Moved Local Heap allocations to Modem Heap.
03/26/09    am     Handling net_down_evt from net_mgr synch in net_event_cb.
24/11/08    am     High/Medium lint fixes.
10/24/08    am     Fixed compiler warnings for off-target.
07/26/07    ss     Removed ping6 bug caused by use of new DSS net mgr module.
                   Added checks in dssi_ping_comm_mgr_read_ping_response().
                   Modified dss_ping_comm_mgr_update_ping_dest_addr() so that
                   IPv6 address presentation format is correct.
05/08/07    hm     Updated to use new DSS net manager module.
04/29/07   ss      Added ping6 support/fixed issues found in testing
04/03/07   ss/msr  Fixed issues found in testing
02/28/07    sv     Moved max ping session declaration to socket defs.
01/22/07    ss     Created.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include <stringl/stringl.h>
#include "target.h"
#include "customer.h"
#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "list.h"
#include "dssocket_defs.h"
#include "dssocket.h"
#include "dss_config.h"
#include "dssdns.h"
#include "dss_net_mgr.h"
#include "dss_ping.h"
#include "dss_ping_config.h"
#include "dss_ping_comm_mgr.h"
#include "dssinternal.h"
#include "ps_system_heap.h"
#include "ps_utils.h"
#include "dssicmp_api.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_iface_addr_v6.h"
#include "ps_iface.h"
#include "ps_icmp.h"
#include "ps_icmp6.h"


/*===========================================================================

                          DATA DECLARATIONS

===========================================================================*/
#define   DSS_PING_COMM_MGR_DATA_BYTES_REQ_FOR_RTT_CALC   4
#define   ICMP_ECHO_MSG_SEQ_HDR_SIZE                      2
#define   ICMP_ECHO_MSG_ID_HDR_SIZE                       2
#define   ICMP_ECHO_MSG_TOTAL_HDR_SIZE                    8
#define   ICMP_ECHO_MSG_ID_HDR_OFFSET                     4
#define   ICMP_ECHO_MSG_SEQ_HDR_OFFSET                    6
#define   ICMP_ECHO_MSG_PAYLOAD_OFFSET                    8
#define   DSS_PING_COMM_MGR_MAX_RTT                       0xFFFFFFFFU
#define   MAX_SEQUENCE_NUMBER                             0xFFFFU
#define   PING_SESS_MSG_ID_MASK                           0XFF00U

typedef enum
{
  ICMP_V4_ECHO_REQ_MSG_TYPE = 8,
  ICMP_V4_ECHO_RSP_MSG_TYPE = 0,
  ICMP_V6_ECHO_REQ_MSG_TYPE = 128,
  ICMP_V6_ECHO_RSP_MSG_TYPE = 0
} dssi_ping_comm_mgr_icmp_msg_type_val;

typedef enum
{
  ICMP_V4_ECHO_REQ_MSG_CODE = 0,
  ICMP_V6_ECHO_REQ_MSG_CODE = 0
} dssi_ping_comm_mgr_icmp_msg_code_val;

/*---------------------------------------------------------------------------
  This structure holds information about an ongoing ping session. Once a
  session is up and pings are being sent out, the following fields are
  used to track the ping requests/responses:

  ping_ack_info_bit_field:
    64 bit field. Used to keep track of unacked pings. If a bit is set, it
    means the ping response for the corresponding ping has not been received.
    This field is set/unset in ping send/recv respectively. It is right-shifted
    when a ping-reponse timeout occurs so that older information is discarded.

  net_mgr_handle
    Holds the handle for ongoing network session corresponding to the ping
    session.

  sockfd
    Socket over which the ICMP echo messages are being sent and received for
    this ping session.

  icmp_echo_msg_id
    ID field of the ICMP packet header. Uniquely identifies the ICMP packet
    corresponding to the ping session. The remote side must replicate this id
    in its responses. Used to match the responses received against the
    ongoing ping sessions.

  icmp_echo_req_seq_num:
    This refers to the sequence number for the latest ping that has been
    sent out. It is incremented everytime a ping is sent out.

  icmp_echo_req_base_seq_num:
    This sequence always refers to the 0th bit position of the
    "ping_ack_info_bit_field". So if the "ping_ack_info_bit_field" is right
    shifted by n-bits, the "icmp_echo_req_base_seq_num" field needs to be
    incremented by n. It is incremented only in the ping-response-timeout
    callback function.

  icmp_echo_rsp_timeout_seq_num
    Used to identify the echo request sequence number corresponding to which
    the next response timeout is supposed to occur.

  icmp_ver
    Identifies if this ping session is for ICMP v4 or v6

  retransmit_timer_handle
    Handle for the retransmit timer which is restarted after every ping sent.
    Retransmit timer's interval is set to the ping interval time specified by
    the user.

  response_timeout_timer_handle
    Handle for the response timeout timer. This timer's initial value is set
    to the response timeout value specified by the user. Henceforth its
    timeout value is computed in the response timeout handler routine.

  user_opts
    Holds the options specified by the user for this ping session.

  ping_stats
    Holds the statistics for individual pings. Used during callbacks to the
    app to notify about the result of each ping.

  ping_session_stats
    Holds the statistics for the entire ping session. It is used during the
    final callback to the app.

  sock_in6
    Holds the IPv6 destination address to ping.

  dest_ip_addr
    Holds the IPv4 destination address to ping.

  app_callback_fn
    The callback function that is used to notify the app about result of each
    ping.

  app_ping_summary_callback_fn
    The callback function called once the ping session is closed. Notifies
    the user about the result of the entire ping session.

  app_user_data
    User data to be returned with each callback.

  net_policy
    Network policy specified by the user for network selection. If the user
    does not specify any net policy, its initialized with the default values.

  app_net_policy_specified
    Flag to indicate if the user has specified the net policy or not.

  is_free
    Flag to indicate if this ping session is free or not. Used to validate an
    ongoing session (which must not be free). Ping sessions are allocated
    from a static pool and can be reused once the ping session is aborted
    setting is_free to TRUE.

  dns_session_handle
    Holds the dns session handle created for each ping session. Used to
    verify the session handle returned in the dns callback.

  dns_query_handle
    Holds the dns query handle created for each ping session. Used to verify
    the query handle returned in the dns callback.
---------------------------------------------------------------------------*/
typedef struct
{
  uint64                                   ping_ack_info_bit_field;
  int32                                    net_mgr_handle;
  int16                                    sockfd;
  int                                      icmp_echo_msg_id;
  uint16                                   icmp_echo_req_seq_num;
  uint16                                   icmp_echo_req_base_seq_num;
  uint16                                   icmp_echo_rsp_timeout_seq_num;
  dssicmp_icmp_ver_type                    icmp_ver;
  ps_timer_handle_type                     retransmit_timer_handle;
  ps_timer_handle_type                     response_timeout_timer_handle;
  dss_ping_config_type                     user_opts;
  dss_ping_stats_type                      ping_stats;
  dss_ping_session_stats_type              ping_session_stats;
  struct ps_sockaddr_in6                   sock_in6;
  struct ps_sockaddr                       dest_ip_addr;
  dss_ping_callback_fn_type                app_callback_fn;
  dss_ping_sess_summary_callback_fn_type   app_ping_summary_callback_fn;
  void                                    *app_user_data;
  dss_net_policy_info_type                 net_policy;
  boolean                                  app_net_policy_specified;
  boolean                                  is_free;
  dss_dns_session_mgr_handle_type          dns_session_handle;
  dss_dns_query_handle_type                dns_query_handle;
  uint8                                    *ping_read_buf;
  int                                      icmp_index;

} dssi_ping_comm_mgr_ping_sess_type;


/*---------------------------------------------------------------------------
  Data structure to hold information about a ping session, and whether the
  session is currently free.
---------------------------------------------------------------------------*/
typedef struct
{
  dssi_ping_comm_mgr_ping_sess_type  ping_session;
  boolean                           is_free;
} dssi_ping_comm_mgr_ping_sess_pool_type;


/*---------------------------------------------------------------------------
  Data structure to pass network callback information to PS task
---------------------------------------------------------------------------*/
typedef struct
{
  int32                              net_mgr_handle;
  dss_iface_id_type                  iface_id;
  dss_net_mgr_net_event_enum_type    net_mgr_event;
  int                                ping_sess_msg_id;
} dssi_ping_comm_mgr_net_cb_data;

/*---------------------------------------------------------------------------
  Data structure to pass socket callback information to PS task
---------------------------------------------------------------------------*/
typedef struct
{
  int16   sockfd;
  uint32  event_mask;
  int     ping_sess_msg_id;
} dssi_ping_comm_mgr_sock_cb_data;

/*---------------------------------------------------------------------------
  Data structure to pass response timeout callback information to PS task
---------------------------------------------------------------------------*/
typedef struct
{
  uint64  bit_field;
  int     ping_sess_msg_id;
} dssi_ping_comm_mgr_rsp_timeout_cb_data;

/*---------------------------------------------------------------------------
  The pool of ping sessions
---------------------------------------------------------------------------*/
dssi_ping_comm_mgr_ping_sess_pool_type
dssi_ping_comm_mgr_ping_sess_pool[DSS_PING_MAX_PING_SESSIONS];

/*---------------------------------------------------------------------------
  dss ping critical section
---------------------------------------------------------------------------*/

extern ps_crit_sect_type dss_ping_crit_section;
/*===========================================================================

                             INTERNAL FORWARD DECLARATIONS

===========================================================================*/
static void dss_ping_dns_resolv_cb_fn
(
  dss_dns_session_mgr_handle_type   session_handle,
  dss_dns_query_handle_type         query_handle,
  dss_dns_api_type_enum_type        api_type,
  uint16                            num_records,
  void                            * user_data_ptr,
  int16                             dss_errno
);

static inline boolean dssi_ping_comm_mgr_all_pings_sent
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr
);

static inline boolean dssi_ping_comm_mgr_unacked_pings_remaining
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr
);

static void dssi_ping_comm_mgr_reset_ping_sess
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr
);

static boolean dssi_ping_comm_mgr_is_sess_valid
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr
);

static int16 dssi_ping_comm_mgr_close_conn
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr,
  int16                              *errno_ptr
);

static void dssi_ping_comm_mgr_read_ping_response
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr
);

static void dssi_ping_comm_mgr_sock_event_cb_fn
(
  int16   dss_nethandle,
  int16   sockfd,
  uint32  event_mask,
  void    *sock_cb_user_data
);

static void dssi_ping_comm_mgr_net_handle_enetnonet
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr
);

static int16 dssi_ping_comm_mgr_net_handle_enetisconn
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr,
  int16                             *errno_ptr
);

static void dssi_ping_comm_mgr_net_event_cb_fn
(
  int32                              net_mgr_handle,
  dss_iface_id_type                  iface_id,
  dss_net_mgr_net_event_enum_type    net_mgr_event,
  void                             * net_cb_user_data
);

static void dssi_ping_comm_mgr_ping_retransmit_timer_cb_fn
(
  void *cb_param
);

static void dssi_ping_comm_mgr_ping_response_timeout_cb_fn
(
  void *cb_param
);

static dssi_ping_comm_mgr_ping_sess_type* dssi_ping_comm_mgr_alloc_ping_session
(
  void
);

static void dssi_ping_comm_mgr_free_ping_session
(
  dssi_ping_comm_mgr_ping_sess_type   *ping_sess_ptr
);

static int16 dssi_ping_comm_mgr_bring_up_net_iface
(
  dssi_ping_comm_mgr_ping_sess_type *ping_sess_ptr,
  int16                             *errno_ptr
);

static void dssi_ping_comm_mgr_send_icmp_req
(
  dssi_ping_comm_mgr_ping_sess_type *ping_sess_ptr
);

/*===========================================================================

                  PING COMM MANAGER MEMORY SUBSYSTEM

===========================================================================*/
/*===========================================================================
FUNCTION  DSSI_PING_COMM_MGR_MEMALLOC

DESCRIPTION
  The function is passed the amount of memory required.  If it finds a
  chunck of memory of suitable size to service the request it returns that
  otherwise it returns a NULL.

  This function may be called from tasks other then PS and therefore must
  be thread safe.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to memory block if successful.
  NULL if could not get memory.

SIDE EFFECTS
  May allocate a large DSM item.  The DSM item is not freed until all memory
  allocated from it is freed.
===========================================================================*/
static void* dssi_ping_comm_mgr_memalloc
(
  uint32 size                          /* Size of memory to be allocated   */
)
{
  uint32  total_size;
  void * buf = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify parameter
  -------------------------------------------------------------------------*/
  if( 0 == size || 0xfffffff0U < size )
  {
    LOG_MSG_ERROR_1("Invalid size %d in dssi_ping_comm_mgr_memalloc()", size );
    return NULL;
  }

  total_size = (sizeof(int32) - 1 + size) & (~(sizeof(int32) - 1));

  /* Get from Modem heap */
  PS_SYSTEM_HEAP_MEM_ALLOC( buf, total_size, void* );
  return buf;
} /* dssi_ping_comm_mgr_memalloc() */


/*===========================================================================
FUNCTION  DSSI_PING_COMM_MGR_MEMFREE

DESCRIPTION
  Free memory allocated by dssi_ping_comm_mgr_memalloc() and sets the passed
  memory pointer to NULL.

DEPENDENCIES
  The memory chunk passed to be freed must have been allocated by
  dssi_ping_comm_mgr_memalloc().

RETURN VALUE
  None.

SIDE EFFECTS
  May free the memory chunk/DSM item which contains the chunk to be freed.
  Sets the passed in pointer to memory to NULL.
===========================================================================*/
static void dssi_ping_comm_mgr_memfree
(
  void **pmem                     /* Memory to free                        */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Verify parameter and return if NULL pointer passed
  ------------------------------------------------------------------------*/
  if( NULL == pmem )
  {
    DATA_ERR_FATAL("dssi_ping_comm_mgr_memfree(): Null argument passed");
    return;
  }

  if( NULL == *pmem )
  {
    return;
  }

  PS_SYSTEM_HEAP_MEM_FREE(*pmem);

  return;
} /* dssi_ping_comm_mgr_memfree() */



/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_ALL_PINGS_SENT

DESCRIPTION
  This function checks whether the requested number of pings have been
  sent out.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all pings have been sent.
  FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
inline boolean dssi_ping_comm_mgr_all_pings_sent
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ping_sess_ptr->user_opts.num_pings == DSS_PING_INFINITE_NUM_PINGS ||
      (ping_sess_ptr->ping_session_stats.num_pkts_sent <
       ping_sess_ptr->user_opts.num_pings) )
  {
    return FALSE;
  }
  return TRUE;
}/* dssi_ping_comm_mgr_all_pings_sent() */

/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_GET_PING_SESS_PTR

DESCRIPTION
  This function returns the pointer to ping session given the session_msg_id
  used for this session.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
inline dssi_ping_comm_mgr_ping_sess_type* dss_ping_comm_mgr_get_ping_sess_ptr
(
  int ping_sess_msg_id
)
{
  uint8 ping_sess_block_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ping_sess_block_index = ping_sess_msg_id & ~PING_SESS_MSG_ID_MASK;
  if(ping_sess_block_index >= DSS_PING_MAX_PING_SESSIONS)
  {
    LOG_MSG_INFO1_1("dss_ping_comm_mgr_get_ping_sess_ptr(): "
                    "Invalid ping session index %d", ping_sess_block_index);
    return NULL;
  }

  if(dssi_ping_comm_mgr_ping_sess_pool[ping_sess_block_index].
     ping_session.icmp_index == ping_sess_msg_id)
  {
    return &dssi_ping_comm_mgr_ping_sess_pool[ping_sess_block_index].
      ping_session;
  }
  else
  {
    LOG_MSG_INFO1_2("dss_ping_comm_mgr_get_ping_sess_ptr(): "
                    "Random parts of message Ids do not match %d, %d",
                    ping_sess_msg_id,
                    dssi_ping_comm_mgr_ping_sess_pool[ping_sess_block_index]. ping_session.icmp_index);
    return NULL;
  }

}

/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_UNACKED_PINGS_REMAINING

DESCRIPTION
  This function checks whether there is any ping whose response has not
  arrived yet.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if an unacked ping is present.
  FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
inline boolean dssi_ping_comm_mgr_unacked_pings_remaining
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Ping response is outstanding if any bit is set */
  if(ping_sess_ptr->ping_ack_info_bit_field == 0)
  {
    return FALSE;
  }
  return TRUE;
} /* dssi_ping_comm_mgr_unacked_pings_remaining() */


/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_RESET_PING_SESS

DESCRIPTION
  This function resets the ping session to default values.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dssi_ping_comm_mgr_reset_ping_sess
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr
)
{
  if(ping_sess_ptr == NULL)
  {
    LOG_MSG_ERROR_0("dssi_ping_comm_mgr_reset_ping_sess(): "
                    "Invalid ping_sess_ptr");
    return;
  }

  memset(ping_sess_ptr, 0, sizeof(dssi_ping_comm_mgr_ping_sess_type));

  ping_sess_ptr->app_net_policy_specified = FALSE;
  ping_sess_ptr->ping_read_buf  = NULL;
  ping_sess_ptr->net_mgr_handle = DSS_ERROR;
  ping_sess_ptr->is_free = TRUE;

} /* dssi_ping_comm_mgr_reset_ping_sess() */



/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_IS_SESS_VALID

DESCRIPTION
  A valid ping session should belong to the ping session pool, and
  should be currently in use.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  : if the passed pointer really belongs to the ping session pool, and
          is in use.
  FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssi_ping_comm_mgr_is_sess_valid
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr
)
{
  boolean   is_valid;
  uint8     i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ping_sess_ptr == NULL)
  {
    LOG_MSG_ERROR_0("dssi_ping_comm_mgr_is_sess_valid(): "
                    "Invalid ping_sess_ptr");
    return FALSE;
  }

  for(i = 0, is_valid = FALSE; i < DSS_PING_MAX_PING_SESSIONS; ++i)
  {
    if( ping_sess_ptr == &(dssi_ping_comm_mgr_ping_sess_pool[i].ping_session))
    {
      /* A valid session will not be free */
      is_valid = (dssi_ping_comm_mgr_ping_sess_pool[i].is_free)? FALSE : TRUE;
      break;
    }
  }
  return is_valid;
} /* dssi_ping_comm_mgr_is_sess_valid() */

/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_CLOSE_CONN

DESCRIPTION
  This function closes the connection which was created to send the
  ICMP ECHO_REQUEST messages.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on success. DSS_ERROR on error, and error condition placed
  in errno.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dssi_ping_comm_mgr_close_conn
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr,
  int16                              *errno_ptr
)
{
  int16   ret   =   DSS_SUCCESS;
  sint15  dss_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("dssi_ping_comm_mgr_close_conn(): "
                  "Closing ping conn session ptr 0x%x  errno ptr 0x%x",
                  ping_sess_ptr, errno_ptr);

  if( ping_sess_ptr == NULL || errno_ptr == NULL )
  {
    return DSS_ERROR;
  }

  /* Close socket, if created. Then tear down net interface */
  if( ping_sess_ptr->sockfd != 0 )
  {
    /* Remove the filters added for ICMP ECHO RESPONSE */
    dss_dereg_ip_filter_by_group(ping_sess_ptr->sockfd,&dss_errno);
    
    ret = dss_close(ping_sess_ptr->sockfd, errno_ptr);
    if( DSS_ERROR == ret )
    {
      LOG_MSG_ERROR_2("dssi_ping_comm_mgr_close_conn(): "
                      "Error %d closing socket %d",
                      *errno_ptr, ping_sess_ptr->sockfd);
    }
    ping_sess_ptr->sockfd = 0;
  }

  if(ping_sess_ptr->net_mgr_handle != DSS_ERROR)
  {
    ret = dss_net_mgr_tear_down_net_iface(ping_sess_ptr->net_mgr_handle, errno_ptr);
    if( DSS_ERROR == ret )
    {
      LOG_MSG_ERROR_1("dssi_ping_comm_mgr_close_conn(): "
                      "Error %d bringing down network", *errno_ptr);
      ASSERT(0);
    }
    else
    {
      ping_sess_ptr->net_mgr_handle = DSS_ERROR;
    }
  }

  return ret;
} /* dssi_ping_comm_mgr_close_conn() */


/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_READ_PING_RESPONSE

DESCRIPTION
  This function reads the ICMP packet on the receive queue.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dssi_ping_comm_mgr_read_ping_response
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr
)
{
  uint32  echo_req_send_timestamp, echo_reply_recv_timestamp;
  uint32  rtt  = 0;
  int16   dss_errno;
  uint16  addrlen;
  uint8   *inbuf = NULL;
  uint16  icmp_echo_rsp_id, icmp_echo_rsp_seq_num;
  uint64  bit_pos;
  int16   bytes_read;
  dss_ping_session_close_reason_type  reason;
  struct ps_sockaddr_in6 dst_sock_addr6;
  struct ps_sockaddr_in dst_sock_addr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ping_sess_ptr == NULL ||
      ping_sess_ptr->ping_read_buf == NULL )
  {
    LOG_MSG_ERROR_0("dssi_ping_comm_mgr_read_ping_response(): "
                    "Invalid ping_sess_ptr");
    return;
  }
  
  /*-----------------------------------------------------------------------
    Read the ICMP packet, note the time when the packet was received.
  -----------------------------------------------------------------------*/
  addrlen = (ping_sess_ptr->icmp_ver == DSSICMP_ICMP_V4)?
    sizeof(struct ps_sockaddr_in) : sizeof(struct ps_sockaddr_in6);
  
  inbuf = (uint8 *) ping_sess_ptr->ping_read_buf;
  dss_errno = DSS_SUCCESS;

  while (dss_errno == DSS_SUCCESS)
  {
    if(ping_sess_ptr->icmp_ver == DSSICMP_ICMP_V4)
    {
      bytes_read = dss_recvfrom( ping_sess_ptr->sockfd,
                    inbuf,
                    (uint16)ping_sess_ptr->user_opts.num_data_bytes + ICMP_ECHO_MSG_TOTAL_HDR_SIZE,
                    0,
                    (struct ps_sockaddr*)&(dst_sock_addr),
                    &addrlen,
                    &dss_errno);

      if(dss_errno == DS_EWOULDBLOCK)
      {
        LOG_MSG_INFO2_0("dssi_ping_comm_mgr_read_ping_response(): "
                        "Finished reading all data");
        break;
      }

      if(memcmp(&(((struct ps_sockaddr_in*)(&ping_sess_ptr->dest_ip_addr))->ps_sin_addr),
                &dst_sock_addr.ps_sin_addr,
                sizeof(struct ps_in_addr)) != 0)
      {
        LOG_MSG_INFO1_0("dssi_ping_comm_mgr_read_ping_response(): "
                        "Received packet from invalid V4 host");
				IPV4_ADDR_MSG((((struct ps_sockaddr_in*)(&ping_sess_ptr->dest_ip_addr))->ps_sin_addr).ps_s_addr);
				IPV4_ADDR_MSG(dst_sock_addr.ps_sin_addr.ps_s_addr);
        dss_errno = DSS_SUCCESS;
        continue;
      }

    }
    else
    {
      bytes_read = dss_recvfrom( ping_sess_ptr->sockfd,
                    inbuf,
                    (uint16)ping_sess_ptr->user_opts.num_data_bytes + ICMP_ECHO_MSG_TOTAL_HDR_SIZE,
                    0,
                    (struct ps_sockaddr*)&(dst_sock_addr6),
                    &addrlen,
                    &dss_errno);

      if(dss_errno == DS_EWOULDBLOCK)
      {
        LOG_MSG_INFO2_0("dssi_ping_comm_mgr_read_ping_response(): "
                        "Finished reading all data");
        break;
      }

      if(memcmp(&(((struct ps_sockaddr_in6*)(&ping_sess_ptr->sock_in6))->ps_sin6_addr),
                 &dst_sock_addr6.ps_sin6_addr,
                 sizeof(struct ps_in6_addr)) != 0)
      {
        LOG_MSG_INFO1_0("dssi_ping_comm_mgr_read_ping_response(): "
                        "Received packet from invalid V6 host");
        IPV6_ADDR_MSG((((struct ps_sockaddr_in6*)(&ping_sess_ptr->sock_in6))->ps_sin6_addr).ps_s6_addr64);
				IPV6_ADDR_MSG(dst_sock_addr6.ps_sin6_addr.ps_s6_addr64);
        dss_errno = DSS_SUCCESS;
        continue;
      }

    }

    if(bytes_read==DSS_ERROR)
    {
      LOG_MSG_ERROR_1("dssi_ping_comm_mgr_read_ping_response(): "
                      "ICMP socket read error=%d", dss_errno);
      break;
    }


  /*-----------------------------------------------------------------------
    Make sure the packet can be processed -
    1. Header should be available.
    2. ICMP_ECHO_REQ Id should match.
    3. RTT should not exceed ping response timeout value.
  -----------------------------------------------------------------------*/

  if(bytes_read < ICMP_ECHO_MSG_TOTAL_HDR_SIZE)
  {
    LOG_MSG_ERROR_0("dssi_ping_comm_mgr_read_ping_response(): "
                    "ICMP hdr truncated, discarding pkt");
    continue;
  }

  echo_reply_recv_timestamp = msclock();

  memscpy( &icmp_echo_rsp_id,
    sizeof(uint16),
    inbuf + ICMP_ECHO_MSG_ID_HDR_OFFSET,
    sizeof(uint16) );
  icmp_echo_rsp_id = ps_ntohs(icmp_echo_rsp_id);

  memscpy( &icmp_echo_rsp_seq_num,
    sizeof(uint16),
    inbuf + ICMP_ECHO_MSG_SEQ_HDR_OFFSET,
    sizeof(uint16) );
  icmp_echo_rsp_seq_num = ps_ntohs(icmp_echo_rsp_seq_num);

    LOG_MSG_INFO1_3("dssi_ping_comm_mgr_read_ping_response(): "
                    "ICMP_ECHO_RSP id =%d, seq_num=%d expected id %d",
                    icmp_echo_rsp_id, icmp_echo_rsp_seq_num, ping_sess_ptr->icmp_echo_msg_id);

    if(icmp_echo_rsp_id != (uint16)ping_sess_ptr->icmp_echo_msg_id)
    {
      continue;
    }

    if(icmp_echo_rsp_seq_num < ping_sess_ptr->icmp_echo_req_base_seq_num ||
        icmp_echo_rsp_seq_num > ping_sess_ptr->icmp_echo_req_seq_num)
    {
      LOG_MSG_INFO1_3("dssi_ping_comm_mgr_read_ping_response(): "
                      "Invalid sequence number: found %d, maximum range %d -> %d",
                      icmp_echo_rsp_seq_num,
                      ping_sess_ptr->icmp_echo_req_base_seq_num,
                      ping_sess_ptr->icmp_echo_req_seq_num);
      continue;
    }

    /* Get the ping's "send" timestamp. The send time was saved in the first
     * 4 bytes of ICMP ECHO message.
     */
    if(bytes_read >= (int16)ICMP_ECHO_MSG_TOTAL_HDR_SIZE + sizeof(uint32))
    {
      memscpy( &echo_req_send_timestamp,
        sizeof(uint32),
        inbuf + ICMP_ECHO_MSG_PAYLOAD_OFFSET,
        sizeof(uint32) );

      echo_req_send_timestamp = ps_ntohl(echo_req_send_timestamp);
      rtt = echo_reply_recv_timestamp - echo_req_send_timestamp;

      if( rtt >= ping_sess_ptr->user_opts.ping_response_time_out)
      {
        LOG_MSG_INFO1_0("dssi_ping_comm_mgr_read_ping_response(): "
                        "RTT exceeded, ignoring rsp");
        continue;
      }
    }
    else
    {
      LOG_MSG_INFO1_0("dssi_ping_comm_mgr_read_ping_response(): "
                      "Pkt truncated. Cannot calculate RTT");

      /* RTT info unavailable, so rely on ping_window to see if the pkt is
       * antique.
       */
      if( ping_sess_ptr->icmp_echo_req_base_seq_num > icmp_echo_rsp_seq_num )
      {
        LOG_MSG_INFO1_0("dssi_ping_comm_mgr_read_ping_response(): "
                        "Ignoring old rsp");
        continue;
      }
    }

    /*-----------------------------------------------------------------------
      Clear the bit corresponding to the ping for which the response has
      been recieved.
    -----------------------------------------------------------------------*/
    bit_pos = icmp_echo_rsp_seq_num - ping_sess_ptr->icmp_echo_req_base_seq_num;
    if(bit_pos >= 64)
    {
      LOG_MSG_INFO1_3("dssi_ping_comm_mgr_read_ping_response(): "
                      "num_pings on net=%d, >=64 rsp=%d, req_base=%d",
                      bit_pos,
                      icmp_echo_rsp_seq_num,
                      ping_sess_ptr->icmp_echo_req_base_seq_num);

      reason = DSS_PING_ERROR;
      dss_errno = DS_ENOMEM; /*TODO: Set to internal error indication */
      (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, &reason, dss_errno);
      return;
    }
    ping_sess_ptr->ping_ack_info_bit_field &= ~( (uint64)0x1 << bit_pos );

    /*-----------------------------------------------------------------------
      Update ping statistics: pkts_received, RTT (min, max, avg)
    -----------------------------------------------------------------------*/
    ++ping_sess_ptr->ping_session_stats.num_pkts_recvd;

    if( rtt > ping_sess_ptr->ping_session_stats.max_rtt )
    {
      ping_sess_ptr->ping_session_stats.max_rtt = rtt;
    }

    if( rtt < ping_sess_ptr->ping_session_stats.min_rtt )
    {
      ping_sess_ptr->ping_session_stats.min_rtt = rtt;
    }

    ping_sess_ptr->ping_session_stats.avg_rtt =
      ( ping_sess_ptr->ping_session_stats.avg_rtt *
        (ping_sess_ptr->ping_session_stats.num_pkts_recvd - 1)) + rtt;

    ping_sess_ptr->ping_session_stats.avg_rtt =
      ping_sess_ptr->ping_session_stats.avg_rtt /
      ping_sess_ptr->ping_session_stats.num_pkts_recvd;

    /*-----------------------------------------------------------------------
      Notify app
    -----------------------------------------------------------------------*/
    LOG_MSG_INFO1_0("dssi_ping_comm_mgr_read_ping_response(): "
                    "Notifying app");

    ping_sess_ptr->ping_stats.icmp_seq_num = icmp_echo_rsp_seq_num;
    ping_sess_ptr->ping_stats.rtt = rtt;
    ping_sess_ptr->app_callback_fn( (dss_ping_handle)ping_sess_ptr,
                                    &(ping_sess_ptr->ping_stats),
                                    ping_sess_ptr->app_user_data,
                                    dss_errno );
  }
    
  if( dss_async_select( ping_sess_ptr->sockfd,
                        DS_READ_EVENT,
                        &dss_errno) == DSS_ERROR )
  {
    LOG_MSG_ERROR_1("dssi_ping_comm_mgr_read_ping_response(): "
                    "DS_READ_EVENT select error %d", dss_errno);
    reason = DSS_PING_ERROR;
    (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, &reason, dss_errno);
    return;
  }

} /* dssi_ping_comm_mgr_read_ping_response() */

/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_SOCK_EVENT_CB_FN

DESCRIPTION
  This callback function is used by PS layer to notify about socket events.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dssi_ping_comm_mgr_sock_event_cb_fn
(
  int16   dss_nethandle,
  int16   sockfd,
  uint32  event_mask,
  void    *sock_cb_user_data
)
{
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr;
  dssi_ping_comm_mgr_sock_cb_data    *pdata;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((ping_sess_ptr =
      dss_ping_comm_mgr_get_ping_sess_ptr((int)sock_cb_user_data))
     == NULL)
  {
    LOG_MSG_ERROR_0("dssi_ping_comm_mgr_sock_event_cb_fn(): "
                    "Invalid user data in sock_event_cb");
    return;
  }

  LOG_MSG_INFO2_3("dssi_ping_comm_mgr_sock_event_cb_fn(): "
                  "ICMP_sock_ev %d Ping session Sock FD %d, sock fd %d",
                  event_mask, ping_sess_ptr->sockfd, sockfd);

  if( ping_sess_ptr->sockfd != sockfd )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Allocate a command buffer and fill it with sockets callback information.
    In case a command buffer can't be allocated pass a NULL to PS context.
  -------------------------------------------------------------------------*/
  pdata = dssi_ping_comm_mgr_memalloc( sizeof(dssi_ping_comm_mgr_sock_cb_data) );

  if( NULL != pdata )
  {
    pdata->sockfd     = sockfd;
    pdata->event_mask = event_mask;
    pdata->ping_sess_msg_id = ping_sess_ptr->icmp_index;
  }

  /*-------------------------------------------------------------------------
    Queue a command to PS task.
  -------------------------------------------------------------------------*/
  ps_send_cmd( PS_DSS_PING_SOCK_CB_CMD, (void *)pdata );
  return;

} /* dssi_ping_comm_mgr_sock_event_cb_fn() */


/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_HANDLE_ENETNONET

DESCRIPTION
  This function handles the event of no network being present.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void dssi_ping_comm_mgr_net_handle_enetnonet
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr
)
{
  dss_ping_session_close_reason_type  reason;
  int16                               dss_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ping_sess_ptr == NULL )
  {
    LOG_MSG_INFO1_0("dssi_ping_comm_mgr_net_handle_enetnonet(): "
                    "NULL ping sess ptr");
    return;
  }

  reason = DSS_PING_ERROR;
  dss_errno = DS_ENETNONET;
  (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, &reason, dss_errno);
  return;
} /* dssi_ping_comm_mgr_net_handle_enetnonet() */

/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_HANDLE_ENETISCONN

DESCRIPTION
  This function handles the event of the underlying network being connected.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on success. DSS_ERROR on error, and error condition placed
  in errno.

SIDE EFFECTS

===========================================================================*/
int16 dssi_ping_comm_mgr_net_handle_enetisconn
(
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr,
  int16                              *errno_ptr
)
{
  dss_sock_cb_fcn   sock_cb_fn  =   dssi_ping_comm_mgr_sock_event_cb_fn;
  int               sock_cb_user_data;
  uint32            sock_opt_len;
  boolean           ret  = TRUE;
  ip_filter_type    filter;
  uint16            icmp_msg_id = 0;
  uint16            ret_val;
  sint15            dss_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("dssi_ping_comm_mgr_net_handle_enetisconn(): "
                  "Net conn up, errno ptr 0x%x session ptr 0x%x",
                  errno_ptr, ping_sess_ptr);

  if( errno_ptr == NULL )
  {
    return DSS_ERROR;
  }

  if( ping_sess_ptr == NULL )
  {
    *errno_ptr = DS_EFAULT;
    return DSS_ERROR;
  }

  sock_cb_user_data = ping_sess_ptr->icmp_index;

  if(ping_sess_ptr->icmp_ver == DSSICMP_ICMP_V4)
  {
    ping_sess_ptr->sockfd = dssicmp_socket( ping_sess_ptr->icmp_ver,
                                            (uint8)ICMP_V4_ECHO_REQ_MSG_TYPE,
                                            (uint8)ICMP_V4_ECHO_REQ_MSG_CODE,
                                            sock_cb_fn,
                                            (void*)sock_cb_user_data,
                                            &ping_sess_ptr->net_policy,
                                            errno_ptr);
  }
  else
  {
    ping_sess_ptr->sockfd = dssicmp_socket( ping_sess_ptr->icmp_ver,
                                            (uint8)ICMP_V6_ECHO_REQ_MSG_TYPE,
                                            (uint8)ICMP_V6_ECHO_REQ_MSG_CODE,
                                            sock_cb_fn,
                                            (void*)sock_cb_user_data,
                                            &ping_sess_ptr->net_policy,
                                            errno_ptr);
  }
  if( ping_sess_ptr->sockfd == DSS_ERROR )
  {
    LOG_MSG_ERROR_1("dssi_ping_comm_mgr_net_handle_enetisconn(): "
                    "sock create error %d", *errno_ptr);

    if(dss_net_mgr_tear_down_net_iface(ping_sess_ptr->net_mgr_handle,
                                       errno_ptr) == DSS_ERROR)
    {
      LOG_MSG_ERROR_1("dssi_ping_comm_mgr_net_handle_enetisconn(): "
                      "iface close error %d", *errno_ptr);
    }
    ret = FALSE;
  }

  /* Set the "Identifier" header field of the ICMP ECHO REQUEST message */
  sock_opt_len = sizeof(int); /*ICMP_ECHO_MSG_ID_HDR_SIZE*/
  if( ret && dss_setsockopt( ping_sess_ptr->sockfd,
                      (int) DSS_IPPROTO_ICMP,
                      (int) DSS_ICMP_ECHO_ID,
                      &(ping_sess_ptr->icmp_echo_msg_id),
                      &sock_opt_len,
                      errno_ptr) == DSS_ERROR )
  {
    LOG_MSG_ERROR_2("dssi_ping_comm_mgr_net_handle_enetisconn(): "
                    "ECHO_ID dss_setsockopt() error %d, %d",
                    *errno_ptr, ping_sess_ptr->icmp_echo_msg_id);

    ret = FALSE;
  }

  /* User may have requested different TTL values for the ping packets */
  if( ret && ping_sess_ptr->user_opts.ttl != DSS_PING_DEFAULT_TTL )
  {
    sock_opt_len = sizeof(int);
    if( dss_setsockopt( ping_sess_ptr->sockfd,
                        (int) DSS_IPPROTO_IP,
                        (int) DSS_IP_TTL,
                        &(ping_sess_ptr->user_opts.ttl),
                        &sock_opt_len,
                        errno_ptr) == DSS_ERROR )
    {
      LOG_MSG_ERROR_1("dssi_ping_comm_mgr_net_handle_enetisconn(): "
                      "dss_setsockopt() error %d", *errno_ptr);
      ret = FALSE;
    }
  }

  if ( TRUE == ret )
  {
    /* Add filter for ICMP ECHO RESPONSE message with identifier zero */
    memset(&filter, 0, sizeof(filter));

    if(ping_sess_ptr->icmp_ver == DSSICMP_ICMP_V4)
    {
      filter.ip_vsn = IP_V4;
      filter.ip_hdr.v4.field_mask = IPFLTR_MASK_IP4_NEXT_HDR_PROT;
 
      filter.ip_hdr.v4.err_mask = 0;
      filter.ip_hdr.v4.next_hdr_prot = PS_IPPROTO_ICMP;
      filter.next_prot_hdr.icmp.type = ICMP_ECHO_REPLY;
    }
    else
    {
      filter.ip_vsn = IP_V6;
      filter.ip_hdr.v6.field_mask = IPFLTR_MASK_IP6_NEXT_HDR_PROT;

      filter.ip_hdr.v6.err_mask = 0;
      filter.ip_hdr.v6.next_hdr_prot = PS_IPPROTO_ICMP6;
      filter.next_prot_hdr.icmp.type = ICMP6_ECHO_RSP;
    }
 
    filter.next_prot_hdr.icmp.field_mask = IPFLTR_MASK_ICMP_ALL;
    filter.next_prot_hdr.icmp.code       = ICMP_ECHO_REPLY;
    filter.next_prot_hdr.icmp.id         = icmp_msg_id;

    ret_val = dss_reg_ip_filter_by_group(ping_sess_ptr->sockfd,
                                         DSS_IFACE_EPC_ANY,
                                         IPFLTR_DEFAULT_TYPE ,
                                         1,
                                         &filter,
                                         &dss_errno);

    if (DSS_SUCCESS != ret_val)
    {
      ret = FALSE;
    }
  }
  /* Register for socket read and write events */
  if( ret && dss_async_select( ping_sess_ptr->sockfd,
                               DS_WRITE_EVENT,
                               errno_ptr ) == DSS_ERROR )
  {
    LOG_MSG_ERROR_1("dssi_ping_comm_mgr_net_handle_enetisconn(): "
                    "DS_WRITE_EVENT select error %d", *errno_ptr);
    ret = FALSE;
  }

  if( ret == FALSE)
  {
    LOG_MSG_ERROR_1("dssi_ping_comm_mgr_net_handle_enetisconn(): "
                    "sock errors, ping_handle %d",
                    (dss_ping_handle)ping_sess_ptr);
    return DSS_ERROR;
  }

  return DSS_SUCCESS;
} /* dssi_ping_comm_mgr_net_handle_enetisconn() */


/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_ICMP_NET_EVENT_CB_FN

DESCRIPTION
  This callback function is used by PS layer to get notified about
  network events.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dssi_ping_comm_mgr_net_event_cb_fn
(
  int32                              net_mgr_handle,
  dss_iface_id_type                  iface_id,
  dss_net_mgr_net_event_enum_type    net_mgr_event,
  void                             * net_cb_user_data
)
{
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr;
  dssi_ping_comm_mgr_net_cb_data     *pdata;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((ping_sess_ptr =
      dss_ping_comm_mgr_get_ping_sess_ptr((int)net_cb_user_data))
     == NULL)
  {
    LOG_MSG_ERROR_0("dssi_ping_comm_mgr_net_event_cb_fn(): "
                    "Invalid user data in net_event_cb");
    return;
  }

  if (net_mgr_event == DSS_NET_MGR_NET_EVENT_DOWN)
  {
    /*-----------------------------------------------------------------------
      Net mgr requires the NET_DOWN event to be handled synchronously here.
    -----------------------------------------------------------------------*/
    LOG_MSG_INFO2_2("dssi_ping_comm_mgr_net_event_cb_fn(): "
                    "Handling NET_EVENT_DOWN for ping session 0x%x.net mgr handle %d",
                    ping_sess_ptr, ping_sess_ptr->net_mgr_handle );
    
    if (ping_sess_ptr->net_mgr_handle == DSS_ERROR)
    {
      return;
    }
    dssi_ping_comm_mgr_net_handle_enetnonet(ping_sess_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Else handle NET_UP event asynch.
  -------------------------------------------------------------------------*/

  pdata = dssi_ping_comm_mgr_memalloc( sizeof(dssi_ping_comm_mgr_net_cb_data) );

  if( NULL != pdata )
  {
    pdata->net_mgr_handle     = net_mgr_handle;
    pdata->iface_id           = iface_id;
    pdata->net_mgr_event      = net_mgr_event;
    pdata->ping_sess_msg_id   = ping_sess_ptr->icmp_index;
  }
  else
  {
    LOG_MSG_ERROR_0("dssi_ping_comm_mgr_net_event_cb_fn(): "
                    "Out of memory" );
  }

  /*-------------------------------------------------------------------------
    Queue a command to PS task.
  -------------------------------------------------------------------------*/
  ps_send_cmd( PS_DSS_PING_NET_CB_CMD, (void *)pdata );

} /* dssi_ping_comm_mgr_net_event_cb_fn() */

/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_PING_RETRANSMIT_TIMEOUT_CB_FN

DESCRIPTION
  This function is invoked when the ping retransmit timer fires.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dssi_ping_comm_mgr_ping_retransmit_timer_cb_fn
(
  void *cb_param
)
{
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((ping_sess_ptr =
      dss_ping_comm_mgr_get_ping_sess_ptr((int)cb_param))
     == NULL)
  {
    LOG_MSG_INFO1_0("dssi_ping_comm_mgr_ping_retransmit_timer_cb_fn(): "
                    "Invalid user data in ping_retransmit_timer_cb");
    return;
  }
  /* Restarting the retransmit timer taken care of by the
   * dssi_ping_comm_mgr_send_icmp_req()
   */
  dss_ping_comm_mgr_send_ping((dss_ping_handle)ping_sess_ptr);

} /* dssi_ping_comm_mgr_ping_retransmit_timer_cb_fn() */


/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_PING_RESPONSE_TIMEOUT_CB_FN

DESCRIPTION
  This function is invoked when the ping response-timeout timer fires.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dssi_ping_comm_mgr_ping_response_timeout_cb_fn
(
  void *cb_param
)
{
  dssi_ping_comm_mgr_ping_sess_type        *ping_sess_ptr;
  dssi_ping_comm_mgr_rsp_timeout_cb_data   *pdata;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("dssi_ping_comm_mgr_ping_response_timeout_cb_fn(): "
                  "Ping rsp timeout cb");
  if((ping_sess_ptr =
      dss_ping_comm_mgr_get_ping_sess_ptr((int)cb_param))
     == NULL)
  {
    LOG_MSG_INFO1_0("dssi_ping_comm_mgr_ping_response_timeout_cb_fn(): "
                    "Invalid user data in ping_response_timeout");
    return;
  }
  pdata = dssi_ping_comm_mgr_memalloc(sizeof(dssi_ping_comm_mgr_rsp_timeout_cb_data));

  if( NULL != pdata )
  {
    pdata->bit_field = ping_sess_ptr->ping_ack_info_bit_field;
    pdata->ping_sess_msg_id  = ping_sess_ptr->icmp_index;
  }
  else
  {
    LOG_MSG_ERROR_0("dssi_ping_comm_mgr_ping_response_timeout_cb_fn(): "
                    "Out of memory" );
  }

  /*-------------------------------------------------------------------------
    Queue a command to PS task.
  -------------------------------------------------------------------------*/
  ps_send_cmd( PS_DSS_PING_RSP_TIMEOUT_CB_CMD, (void *)pdata );

} /* dssi_ping_comm_mgr_ping_response_timeout_cb_fn() */


/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_ALLOC_PING_SESSION

DESCRIPTION
  This function allocates a new ping session.

DEPENDENCIES
  None.

RETURN VALUE
  NULL if the max limit is reached and no more sessions can be created.
  A valid pointer on success.

SIDE EFFECTS
  None.
===========================================================================*/
dssi_ping_comm_mgr_ping_sess_type* dssi_ping_comm_mgr_alloc_ping_session
(
  void
)
{
  uint8                             i;
  dssi_ping_comm_mgr_ping_sess_type   *new_ping_session   = NULL;
  uint16                               icmp_echo_msg_id = 0;
  uint16                               icmp_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(i = 0; i < DSS_PING_MAX_PING_SESSIONS; ++i)
  {
    if( dssi_ping_comm_mgr_ping_sess_pool[i].is_free )
    {
      new_ping_session = &(dssi_ping_comm_mgr_ping_sess_pool[i].ping_session);
      dssi_ping_comm_mgr_ping_sess_pool[i].is_free = FALSE;
      new_ping_session->is_free = FALSE;

      /*----------------------------------------------------------------------- 
         The icmp_echo_msg_id was also used as an index in 
         dssi_ping_comm_mgr_ping_sess_pool. So as a part of FR 22677(CR 728235)
         new field icmp_index was introduced to act as an index and 
         icmp_echo_msg_id will be 0 for all the ping request packets sent out 
         from DSS PING module.
      -----------------------------------------------------------------------*/

      new_ping_session->icmp_echo_msg_id = icmp_echo_msg_id;
      icmp_index = ps_utils_generate_16bit_rand_num_by_range( 0, 0X7FU);
      new_ping_session->icmp_index = (icmp_index << 8) | i;
      LOG_MSG_INFO2_2("dssi_ping_comm_mgr_alloc_ping_session(): "
                      "Setting messageId for ping session = %d, index =%d",
                      new_ping_session->icmp_echo_msg_id, i);
      break;
    }
  }
  return new_ping_session;
} /* dssi_ping_comm_mgr_alloc_ping_session() */

/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_FREE_PING_SESSION

DESCRIPTION
  This function frees up the ping session, and returns it to the list of
  available sessions.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dssi_ping_comm_mgr_free_ping_session
(
  dssi_ping_comm_mgr_ping_sess_type   *ping_sess_ptr
)
{
  uint8     i;
  boolean   was_session_freed   =   FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ping_sess_ptr == NULL )
  {
    LOG_MSG_ERROR_0("dssi_ping_comm_mgr_free_ping_session(): "
                    "Invalid ping_sess_ptr");
    return;
  }

  for(i = 0; i < DSS_PING_MAX_PING_SESSIONS; ++i)
  {
    if( dssi_ping_comm_mgr_ping_sess_pool[i].is_free == FALSE &&
        ping_sess_ptr == &(dssi_ping_comm_mgr_ping_sess_pool[i].ping_session) )
    {
                    
      if( NULL != ping_sess_ptr->ping_read_buf )
      {
        dssi_ping_comm_mgr_memfree((void **)&ping_sess_ptr->ping_read_buf);
      }    
      dssi_ping_comm_mgr_ping_sess_pool[i].is_free = TRUE;
      dssi_ping_comm_mgr_reset_ping_sess(ping_sess_ptr);
      was_session_freed = TRUE;
      break;
    }
  }

  if( was_session_freed == FALSE )
  {
    LOG_MSG_ERROR_0("dssi_ping_comm_mgr_free_ping_session(): "
                    "no match");
  }
} /* dssi_ping_comm_mgr_free_ping_session() */


/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_BRING_UP_NET_IFACE

DESCRIPTION
  This function brings up the net interface over which ICMP ECHO_REQUEST
  messages will be sent.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS: Indicates network iface is up.
  DSS_ERROR: If errno is set to DS_EWOULDBLOCK, this indicates that network
             interface is being brought up. Otherwise, it indicates an error.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dssi_ping_comm_mgr_bring_up_net_iface
(
  dssi_ping_comm_mgr_ping_sess_type *ping_sess_ptr,
  int16                             *errno_ptr
)
{
  dss_iface_id_type         iface_id;
  int                       ping_sess_msg_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("dssi_ping_comm_mgr_bring_up_net_iface(): "
                  "Bringing up net iface errno ptr 0x%x, ping session ptr 0x%x ",
                  errno_ptr, ping_sess_ptr);

  if( errno_ptr == NULL )
  {
    return DSS_ERROR;
  }
  *errno_ptr = DSS_SUCCESS;

  if( ping_sess_ptr == NULL )
  {
    *errno_ptr = DS_EFAULT;
    return DSS_ERROR;
  }

  /*-----------------------------------------------------------------
    Bring up the interface requested by user, or else the default
    interface.
  -----------------------------------------------------------------*/
  if(ping_sess_ptr->app_net_policy_specified == FALSE)
  {
    /* User did not specify any interface, initialize net_policy */
    dss_init_net_policy_info(&ping_sess_ptr->net_policy);

    if(ping_sess_ptr->icmp_ver == DSSICMP_ICMP_V6)
    {
      ping_sess_ptr->net_policy.family = DSS_AF_INET6;
      ping_sess_ptr->net_policy.umts.pdp_profile_num = 2;
    }
  }

  ping_sess_msg_id = ping_sess_ptr->icmp_index;
  ping_sess_ptr->net_mgr_handle =
    dss_net_mgr_bring_up_net_iface( &ping_sess_ptr->net_policy,
                                    &iface_id,
                                    dssi_ping_comm_mgr_net_event_cb_fn,
                                    (void *)ping_sess_msg_id,
                                    errno_ptr);



  if( ping_sess_ptr->net_mgr_handle != DSS_ERROR && *errno_ptr == DSS_SUCCESS)
  {
    LOG_MSG_INFO1_0("dssi_ping_comm_mgr_bring_up_net_iface(): "
                    "iface already up");
    return DSS_SUCCESS;
  }

  if( ping_sess_ptr->net_mgr_handle == DSS_ERROR)
  {
    LOG_MSG_ERROR_1("dssi_ping_comm_mgr_bring_up_net_iface(): "
                    "iface open error %d", *errno_ptr);
  }

  return DSS_ERROR;
} /* dssi_ping_comm_mgr_bring_up_net_iface() */


/*===========================================================================
FUNCTION DSSI_PING_COMM_MGR_SEND_ICMP_REQ

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
void dssi_ping_comm_mgr_send_icmp_req
(
  dssi_ping_comm_mgr_ping_sess_type *ping_sess_ptr
)
{
  uint8   outbuf[DSS_PING_MAX_PING_DATA_BYTES];
  uint8   ping_data_byte_offset     =   0;
  uint32  ping_send_timestamp;
  int16   dss_errno;
  int16   ret;
  uint16  addrlen;
  dss_ping_session_close_reason_type  reason;
  uint64  bit_pos;
  int16   sendto_ret;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ping_sess_ptr == NULL )
  {
    LOG_MSG_ERROR_0("dssi_ping_comm_mgr_send_icmp_req(): "
                    "Invalid ping_sess_ptr");
    return;
  }

  /* Socket may not have been created so far. Bring up the net iface. */
  if( ping_sess_ptr->sockfd == 0 )
  {
    ret = dssi_ping_comm_mgr_bring_up_net_iface(ping_sess_ptr, &dss_errno);

    if( ret == DSS_SUCCESS)
    {
      LOG_MSG_INFO1_0("dssi_ping_comm_mgr_send_icmp_req(): "
                      "Iface up");
    }
    else if( ret == DSS_ERROR && dss_errno == DS_EWOULDBLOCK)
    {
      LOG_MSG_INFO1_0("dssi_ping_comm_mgr_send_icmp_req(): "
                      "Iface bringup underway");
    }
    else
    {
      /* connection could not be established */
      reason = DSS_PING_ERROR;
      LOG_MSG_ERROR_1("dssi_ping_comm_mgr_send_icmp_req(): "
                      "Iface bringup error %d", dss_errno);
      (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, &reason, dss_errno);
    }
    return;
  }

  /* Proceed only if there are pings to sent */
  if(dssi_ping_comm_mgr_all_pings_sent(ping_sess_ptr))
  {
    if(!dssi_ping_comm_mgr_unacked_pings_remaining(ping_sess_ptr))
    {
      LOG_MSG_INFO1_0("dssi_ping_comm_mgr_send_icmp_req(): "
                      "Ping sess complete");
      reason = DSS_PING_SESSION_COMPLETE;
      dss_errno = DSS_SUCCESS;
      (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, &reason, dss_errno);
    }
    return;
  }

  if( dss_async_select( ping_sess_ptr->sockfd,
                        DS_READ_EVENT,
                        &dss_errno) == DSS_ERROR )
  {
    LOG_MSG_ERROR_1("dssi_ping_comm_mgr_send_icmp_req(): "
                    "DS_READ_EVENT select error %d", dss_errno);
    reason = DSS_PING_ERROR;
    (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, &reason, dss_errno);
    return;
  }


  /* Prepare the data byte portion of the ICMP ECHO_REQUEST packet and
   * send it out.
   *
   * If more than 8 bytes are being sent, then put timestamp information
   * in first 8 bytes. This timestamp is used to note the time when the
   * request was sent out.
   * Since all ECHO_REPLY messages return the data portion unchanged,
   * we can use the timestamp information to calculate the round trip time
   * (RTT) for each ping.
   */
  if( ping_sess_ptr->user_opts.num_data_bytes >=
      DSS_PING_COMM_MGR_DATA_BYTES_REQ_FOR_RTT_CALC )
  {
    ping_send_timestamp = msclock();
    ping_send_timestamp = ps_htonl(ping_send_timestamp);
    memscpy(outbuf, sizeof(ping_send_timestamp),
      &ping_send_timestamp, sizeof(ping_send_timestamp));
    ping_data_byte_offset = sizeof(ping_send_timestamp);
  }

  /* The data byte portion can be filled with anything */
  memset( outbuf + ping_data_byte_offset,
          'a',
          ping_sess_ptr->user_opts.num_data_bytes - ping_data_byte_offset);

  addrlen = (ping_sess_ptr->icmp_ver == DSSICMP_ICMP_V4)?
    sizeof(struct ps_sockaddr_in) :
    sizeof(struct ps_sockaddr_in6);

  if(ping_sess_ptr->icmp_ver == DSSICMP_ICMP_V4)
  {
    sendto_ret = dss_sendto( ping_sess_ptr->sockfd,
                  outbuf,
                  (uint16)ping_sess_ptr->user_opts.num_data_bytes,
                  FALSE,
                  &(ping_sess_ptr->dest_ip_addr),
                  addrlen,
                  &dss_errno);
  }
  else
  {
    sendto_ret = dss_sendto( ping_sess_ptr->sockfd,
                  outbuf,
                  (uint16)ping_sess_ptr->user_opts.num_data_bytes,
                  FALSE,
                  (struct ps_sockaddr*)&(ping_sess_ptr->sock_in6),
                  addrlen,
                  &dss_errno);
  }

  /*-----------------------------------------------------------------------
    Verify if the send operation was successful.
    If the send operation blocks, we must not abort ping. Instead we must
    register for WRITE event again and try to send the remaining pings.
  -----------------------------------------------------------------------*/
  if(sendto_ret==DSS_ERROR)
  {
    if( dss_errno == DS_EWOULDBLOCK )
    {
      LOG_MSG_ERROR_1("dssi_ping_comm_mgr_send_icmp_req(): "
                      "sendto is blocked, error %d", dss_errno);
      /* Register for socket write event */
      if( dss_async_select( ping_sess_ptr->sockfd,
                            DS_WRITE_EVENT,
                            &dss_errno ) == DSS_ERROR )
      {
        reason = DSS_PING_ERROR;
        LOG_MSG_ERROR_1("dssi_ping_comm_mgr_send_icmp_req(): "
                        "DS_WRITE_EVENT select error %d", dss_errno);
        (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr,
                                           &reason, dss_errno);
      }
      return;
    }
    else
    {
      LOG_MSG_ERROR_1("dssi_ping_comm_mgr_send_icmp_req(): "
                      "sendto error %d, aborting ping", dss_errno);
      reason = DSS_PING_ERROR;
      (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr,
                                         &reason, dss_errno);
      return;
    }
  }

  LOG_MSG_INFO1_1("dssi_ping_comm_mgr_send_icmp_req(): "
                  "Ping sent, seq_num=%d",
                  ping_sess_ptr->icmp_echo_req_seq_num);

  /*-----------------------------------------------------------------------
    Set the bit corresponding to this ping to indicate that a ping response
    is awaited
  -----------------------------------------------------------------------*/
  bit_pos = ping_sess_ptr->icmp_echo_req_seq_num - ping_sess_ptr->icmp_echo_req_base_seq_num;
  if(bit_pos >=64)
  {
    LOG_MSG_INFO1_1("dssi_ping_comm_mgr_send_icmp_req(): "
                    "num_pings on net=%d, >=64", bit_pos);
    reason = DSS_PING_ERROR;
    dss_errno = DS_ENOMEM; /*TODO: Set to internal error indication */
    (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, &reason, dss_errno);
    return;
  }
  ping_sess_ptr->ping_ack_info_bit_field |= ( (uint64)0x1 << bit_pos );

  /* Update ping statistics */
  ++ping_sess_ptr->ping_session_stats.num_pkts_sent;
  ++ping_sess_ptr->icmp_echo_req_seq_num; /* Keep track of next seq num */

  /* Get response timeout timer going if this is the first ping that is sent out.
   * Thereafter, the timeout handler will take care of restarting the timer
   * with appropriate timeout values
   */
  if(ping_sess_ptr->ping_session_stats.num_pkts_sent == 1)
  {
    if( ps_timer_start( ping_sess_ptr->response_timeout_timer_handle,
                        ping_sess_ptr->user_opts.ping_response_time_out) == PS_TIMER_FAILURE )
    {
      LOG_MSG_ERROR_0("dssi_ping_comm_mgr_send_icmp_req(): "
                      "rsp timeout timer fail");
      reason = DSS_PING_ERROR;
      dss_errno = DS_ENOMEM;
      (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, &reason, dss_errno);
      return;
    }
  }

  /* Retransmit timer started after every ping */
  if( ps_timer_start( ping_sess_ptr->retransmit_timer_handle,
                      ping_sess_ptr->user_opts.ping_interval_time) == PS_TIMER_FAILURE )
  {
    LOG_MSG_ERROR_0("dssi_ping_comm_mgr_send_icmp_req(): "
                    "rsp timeout timer fail");
    reason = DSS_PING_ERROR;
    dss_errno = DS_ENOMEM;
    (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, &reason, dss_errno);
  }

} /* dssi_ping_comm_mgr_send_icmp_req() */


/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS

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
)
{
  uint8  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(i = 0; i < DSS_PING_MAX_PING_SESSIONS; ++i)
  {
    dssi_ping_comm_mgr_ping_sess_pool[i].is_free = TRUE;

    dssi_ping_comm_mgr_reset_ping_sess(&(dssi_ping_comm_mgr_ping_sess_pool[i].ping_session));
  }

  (void) ps_set_cmd_handler( PS_DSS_PING_SEND_PING_CMD,
                             dss_ping_comm_mgr_send_ping_cmd_handler);

  (void) ps_set_cmd_handler( PS_DSS_PING_NET_CB_CMD,
                             dss_ping_comm_mgr_net_cb_cmd_handler);

  (void) ps_set_cmd_handler( PS_DSS_PING_SOCK_CB_CMD,
                             dss_ping_comm_mgr_sock_cb_cmd_handler);

  (void) ps_set_cmd_handler( PS_DSS_PING_RSP_TIMEOUT_CB_CMD,
                             dss_ping_comm_mgr_ping_rsp_timeout_cb_cmd_handler);

} /* dss_ping_comm_mgr_init_ping_engine() */

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
  ps_cmd_enum_type cmd,
  void *user_data_ptr
)
{
  dssi_ping_comm_mgr_ping_sess_type    *ping_sess_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((ping_sess_ptr =
      dss_ping_comm_mgr_get_ping_sess_ptr((int)user_data_ptr))
     == NULL)
  {
    LOG_MSG_INFO1_0("dss_ping_comm_mgr_send_ping_cmd_handler(): "
                    "Invalid user data in ping_send_handler");
    return;
  }

  if( cmd != PS_DSS_PING_SEND_PING_CMD )
  {
    LOG_MSG_ERROR_0("dss_ping_comm_mgr_send_ping_cmd_handler(): "
                    "Non-ping command");
    return;
  }

  dssi_ping_comm_mgr_send_icmp_req(ping_sess_ptr);
}

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
  void              *netdata
)
{
  dssi_ping_comm_mgr_ping_sess_type  * ping_sess_ptr = NULL;
  dssi_ping_comm_mgr_net_cb_data     * pdata;
  int16                                dss_errno;
  dss_ping_session_close_reason_type   reason;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("dss_ping_comm_mgr_net_cb_cmd_handler:command %d, netdata 0x%x",
                  cmd, netdata);

  if( NULL == netdata )
  {
    DATA_ERR_FATAL("dss_ping_comm_mgr_net_cb_cmd_handler(): NULL user options pointer");
    return;
  }

  if (PS_DSS_PING_NET_CB_CMD != cmd) {
    LOG_MSG_ERROR_1("dss_ping_comm_mgr_net_cb_cmd_handler: wrong command %d",
                    cmd);
    ASSERT(0);
  }

  pdata = (dssi_ping_comm_mgr_net_cb_data *) netdata;

  ping_sess_ptr = dss_ping_comm_mgr_get_ping_sess_ptr (pdata->ping_sess_msg_id);
  if(ping_sess_ptr == NULL)
  {
    LOG_MSG_INFO1_0("dss_ping_comm_mgr_net_cb_cmd_handler(): "
                    "Net event ignoring"  );
    return;
  }

  if(ping_sess_ptr->is_free)
  {
    LOG_MSG_INFO1_1("dss_ping_comm_mgr_net_cb_cmd_handler(): "
                    "Net event %d for freed-up ping sess, ignoring",
                    pdata->net_mgr_event);
    return;
  }

  if (ping_sess_ptr->net_mgr_handle != pdata->net_mgr_handle)
  {
    DATA_ERR_FATAL("dss_ping_comm_mgr_net_cb_cmd_handler(): Inconsistent net mgr handles");
    return;
  }

  switch( pdata->net_mgr_event )
  {
    case DSS_NET_MGR_NET_EVENT_UP:
      /*---------------------------------------------------------------------
        Handle the network up event.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO1_1("dss_ping_comm_mgr_net_cb_cmd_handler(): "
                      "NET_EVENT_UP for ping session 0x%p", ping_sess_ptr);

      if (ping_sess_ptr->sockfd == 0)
      {
        if( dssi_ping_comm_mgr_net_handle_enetisconn( ping_sess_ptr, &dss_errno ) == DSS_ERROR )
        {
          LOG_MSG_INFO1_1("dss_ping_comm_mgr_net_cb_cmd_handler(): "
                          "conn setup failed, reason %d", dss_errno);
          (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, &reason, dss_errno);
        }
      }
      break;

    case DSS_NET_MGR_NET_EVENT_DOWN:
      /*---------------------------------------------------------------------
        Handle the network down event.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO1_1("dss_ping_comm_mgr_net_cb_cmd_handler(): "
                      "NET_EVENT_DOWN for ping session 0x%p", ping_sess_ptr);
      dssi_ping_comm_mgr_net_handle_enetnonet(ping_sess_ptr);
      break;

    default:
      /*---------------------------------------------------------------------
        Unexpected network event.
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_1("dss_ping_comm_mgr_net_cb_cmd_handler(): "
                      "Unhandled net manager event %d", pdata->net_mgr_event);
      break;
  } /* switch */

  dssi_ping_comm_mgr_memfree( &netdata );

} /* dss_ping_comm_mgr_net_cb_cmd_handler() */



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
  void              *sockdata
)
{
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr  = NULL;
  dssi_ping_comm_mgr_sock_cb_data   *pdata          = sockdata;
  dss_ping_session_close_reason_type  reason;
  int16                               dss_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("dss_ping_comm_mgr_sock_cb_cmd_handler: command %d, pdata 0x%x",
                  cmd, pdata);

  if( NULL == pdata )
  {
    DATA_ERR_FATAL("dss_ping_comm_mgr_sock_cb_cmd_handler(): NULL user options pointer");
    return;
  }

  if (PS_DSS_PING_SOCK_CB_CMD != cmd) {
     LOG_MSG_ERROR_1("dss_ping_comm_mgr_sock_cb_cmd_handler: wrong command %d",
                     cmd);
     ASSERT(0);
     return;
  }

  PS_ENTER_CRIT_SECTION(&dss_ping_crit_section);

  if((ping_sess_ptr =
      dss_ping_comm_mgr_get_ping_sess_ptr(pdata->ping_sess_msg_id))
     == NULL)
  {
    LOG_MSG_INFO1_0("dss_ping_comm_mgr_sock_cb_cmd_handler(): "
                    "Invalid user data in sock_cb_cmd");
		PS_LEAVE_CRIT_SECTION(&dss_ping_crit_section);
    return;
  }

  LOG_MSG_INFO1_1("dss_ping_comm_mgr_sock_cb_cmd_handler(): "
                  "sock DS Socket event mask %d", pdata->event_mask);
  

  switch(pdata->event_mask)
  {
    case DS_READ_EVENT:
      dssi_ping_comm_mgr_read_ping_response(ping_sess_ptr);
      break;

    case DS_WRITE_EVENT:
      dss_ping_comm_mgr_send_ping((dss_ping_handle)ping_sess_ptr);
      break;

    case DS_CLOSE_EVENT:
      reason = DSS_PING_ERROR;
      dss_errno = DS_ENETCLOSEINPROGRESS;
      (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, &reason, dss_errno);
      break;

    default:
      break;
  }

	PS_LEAVE_CRIT_SECTION(&dss_ping_crit_section);

  dssi_ping_comm_mgr_memfree( &sockdata );

} /* dss_ping_comm_mgr_sock_cb_cmd_handler() */


/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_PING_RSP_TIMEOUT_CB_CMD_HANDLER

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
)
{
  dssi_ping_comm_mgr_ping_sess_type        *ping_sess_ptr  = NULL;
  dssi_ping_comm_mgr_rsp_timeout_cb_data   *pdata          =
    (dssi_ping_comm_mgr_rsp_timeout_cb_data *)timeout_data;
  dss_ping_session_close_reason_type        reason;
  uint64                                    mask;
  int16                                     dss_errno;
  uint16                                    clock_ticks;
  uint32                                    new_time_out;
  boolean                                   unacked_ping_found;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("dss_ping_comm_mgr_ping_rsp_timeout_cb_cmd_handler: command %d pdata 0x%x",
                  cmd, pdata);
  if( NULL == pdata )
  {
    DATA_ERR_FATAL("dss_ping_comm_mgr_ping_rsp_timeout_cb_cmd_handler(): NULL user options pointer");
    return;
  }

  if ( PS_DSS_PING_RSP_TIMEOUT_CB_CMD != cmd )
  {
    LOG_MSG_ERROR_1("dss_ping_comm_mgr_ping_rsp_timeout_cb_cmd_handler: wrong command %d",
                    cmd);
    ASSERT(0);
  }

  /*------------------------------------------------------------------------
    This critical section ensures that the processing of this response
    timeout callback in PS context is mutually exclusive from ping abort
    processing in UI context.
  ------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&dss_ping_crit_section);

  ping_sess_ptr = dss_ping_comm_mgr_get_ping_sess_ptr(pdata->ping_sess_msg_id);
  if( ping_sess_ptr == NULL )
  {
    LOG_MSG_INFO1_0("dss_ping_comm_mgr_ping_rsp_timeout_cb_cmd_handler(): "
                    "Invalid ping_session_msg_id value");
    PS_BRANCH_LEAVE_CRIT_SECTION(&dss_ping_crit_section);
    return;
  }
  if(ping_sess_ptr->icmp_echo_rsp_timeout_seq_num <
     ping_sess_ptr->icmp_echo_req_base_seq_num)
  {
    /* Here we set the new timeout value. The next timeout should occur for
     * the oldest unacked ping. If this timeout is for the n'th ping and x
     * pings have already been acked in the meantime, then we don't need to
     * timeout for these x intermediate pings. Hence the new timeout value is
     * set to ping interval times the difference between the oldest unacked
     * ping and the currently timed out ping.
     */
    new_time_out = (uint16)ping_sess_ptr->user_opts.ping_interval_time *
                           (ping_sess_ptr->icmp_echo_req_base_seq_num -
                            ping_sess_ptr->icmp_echo_rsp_timeout_seq_num);
    if( ps_timer_start( ping_sess_ptr->response_timeout_timer_handle,
                        new_time_out) == PS_TIMER_FAILURE )
    {
      LOG_MSG_ERROR_0("dss_ping_comm_mgr_ping_rsp_timeout_cb_cmd_handler(): "
                      "rsp timeout timer fail");
      reason = DSS_PING_ERROR;
      dss_errno = DS_ENOMEM;
      (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr,
                                         &reason, dss_errno);
    }
    ping_sess_ptr->icmp_echo_rsp_timeout_seq_num =
      ping_sess_ptr->icmp_echo_req_base_seq_num;

    dssi_ping_comm_mgr_memfree( &timeout_data );
    PS_BRANCH_LEAVE_CRIT_SECTION(&dss_ping_crit_section);
    return;
  }

  /* Check if the response was received for the corresponding packet */
  mask = 0x1;
  if(mask & pdata->bit_field)
  {
    ++ping_sess_ptr->ping_session_stats.num_pkts_lost;

    ping_sess_ptr->ping_stats.icmp_seq_num = ping_sess_ptr->icmp_echo_req_base_seq_num;
    ping_sess_ptr->ping_stats.rtt = 0;
    ping_sess_ptr->app_callback_fn( (dss_ping_handle)ping_sess_ptr,
                                    &(ping_sess_ptr->ping_stats),
                                    ping_sess_ptr->app_user_data,
                                    DS_ETIMEDOUT );

    LOG_MSG_INFO1_0("dss_ping_comm_mgr_ping_rsp_timeout_cb_cmd_handler(): "
                    "Ping timeout occured");
  }

  /* Now find the new timeout value - find the next unacked packet, and
  * calculate the time remaining for its response.
  */
  mask <<= 1;
  clock_ticks = 1;
  unacked_ping_found = FALSE;

  while(mask <= pdata->bit_field)
  {
    if(mask & pdata->bit_field)
    {
      unacked_ping_found = TRUE;
      break;
    }
    mask <<= 1;
    ++clock_ticks;
  }

  /* Slide the window */
  ping_sess_ptr->icmp_echo_req_base_seq_num += clock_ticks;
  ping_sess_ptr->ping_ack_info_bit_field >>= clock_ticks;

  if( dssi_ping_comm_mgr_all_pings_sent(ping_sess_ptr) &&
      !dssi_ping_comm_mgr_unacked_pings_remaining(ping_sess_ptr) )
  {
    LOG_MSG_INFO1_0("dss_ping_comm_mgr_ping_rsp_timeout_cb_cmd_handler(): "
                    "Ping sess complete");
    reason = DSS_PING_SESSION_COMPLETE;
    dss_errno = DSS_SUCCESS;
    (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, &reason, dss_errno);
  }
  else
  {
    if(!unacked_ping_found)
    {
      /* This is possible if all ping responses were received promptly
       * (causing "ping_ack_info_bit_field" to have all 0 bits),
       * but there is still some pings that need to go out.
       * Set new timeout as if a ping has just been sent out.
       * Also, increment base sequence number, otherwise only
       * (increasingly) higher order bits will keep getting set in
       * "ping_ack_info_bit_field".
       */
      ping_sess_ptr->icmp_echo_req_base_seq_num = ping_sess_ptr->icmp_echo_req_seq_num;
    }

    new_time_out = ping_sess_ptr->user_opts.ping_interval_time *
                   (ping_sess_ptr->icmp_echo_req_base_seq_num -
                    ping_sess_ptr->icmp_echo_rsp_timeout_seq_num);
    if( ps_timer_start( ping_sess_ptr->response_timeout_timer_handle,
                        new_time_out) == PS_TIMER_FAILURE )
    {
      LOG_MSG_ERROR_0("dss_ping_comm_mgr_ping_rsp_timeout_cb_cmd_handler(): "
                      "rsp timeout timer fail");
      reason = DSS_PING_ERROR;
      dss_errno = DS_ENOMEM;
      (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, &reason, dss_errno);
    }
    else
    {
      /* Increment response timeout sequence number corresponding to the next
         ping to be sent. */
      ping_sess_ptr->icmp_echo_rsp_timeout_seq_num =
        ping_sess_ptr->icmp_echo_req_base_seq_num;
    }
  }

  dssi_ping_comm_mgr_memfree( &timeout_data );
  PS_LEAVE_CRIT_SECTION(&dss_ping_crit_section);
} /* dss_ping_comm_mgr_ping_rsp_timeout_cb_cmd_handler() */

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
  int16                                   *errno_ptr
)
{
  dssi_ping_comm_mgr_ping_sess_type *ping_sess_ptr;
  int                                ping_sess_msg_id;
  int                                ping_read_buf_len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_4("dss_ping_comm_mgr_create_ping_session(): "
                  "Input Params errno ptr 0x%x user ping options 0x%x "
                  "app callback fn 0x%x app ping summary callback 0x%x ",
                  errno_ptr, user_ping_options, app_callback_fn,
                  app_ping_summary_callback_fn);


  if( errno_ptr == NULL )
  {
    return DSS_PING_INVALID_HANDLE;
  }
  *errno_ptr = DSS_SUCCESS;

  if( user_ping_options == NULL )
  {
    *errno_ptr = DS_EFAULT;
    return DSS_PING_INVALID_HANDLE;
  }

  if( app_callback_fn == NULL || app_ping_summary_callback_fn == NULL )
  {
    *errno_ptr = DS_EFAULT;
    return DSS_PING_INVALID_HANDLE;
  }

  /*-----------------------------------------------------------------------
    Allocate memory for ping session, create timers, and note down
    all ping options requested by user.
  -----------------------------------------------------------------------*/
  if( (ping_sess_ptr = dssi_ping_comm_mgr_alloc_ping_session()) == NULL)
  {
    LOG_MSG_ERROR_0("dss_ping_comm_mgr_create_ping_session(): "
                    "Error creating ping session");
    *errno_ptr = DS_ENOMEM;
    return DSS_PING_INVALID_HANDLE;
  }

  ping_sess_msg_id = ping_sess_ptr->icmp_index;
  ping_sess_ptr->retransmit_timer_handle =
    ps_timer_alloc(dssi_ping_comm_mgr_ping_retransmit_timer_cb_fn,
                   (void*)ping_sess_msg_id);

  if( ping_sess_ptr->retransmit_timer_handle == PS_TIMER_INVALID_HANDLE )
  {
    LOG_MSG_ERROR_0("dss_ping_comm_mgr_create_ping_session(): "
                    "timer create error");
    *errno_ptr = DS_ENOMEM;
    (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, NULL, *errno_ptr);
    return DSS_PING_INVALID_HANDLE;
  }

  ping_sess_ptr->response_timeout_timer_handle =
    ps_timer_alloc(dssi_ping_comm_mgr_ping_response_timeout_cb_fn,
                   (void*)ping_sess_msg_id);

  if( ping_sess_ptr->response_timeout_timer_handle == PS_TIMER_INVALID_HANDLE )
  {
    LOG_MSG_ERROR_0("dss_ping_comm_mgr_create_ping_session(): "
                    "timer create error");
    *errno_ptr = DS_ENOMEM;
    (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, NULL, *errno_ptr);
    return DSS_PING_INVALID_HANDLE;
  }
   
   ping_read_buf_len = user_ping_options->num_data_bytes + ICMP_ECHO_MSG_TOTAL_HDR_SIZE;
   ping_sess_ptr->ping_read_buf = dssi_ping_comm_mgr_memalloc(ping_read_buf_len);
        
  if( NULL == ping_sess_ptr->ping_read_buf )
  {
    LOG_MSG_ERROR_0("dss_ping_comm_mgr_create_ping_session(): "
                    "Error in allocating ping read buffer");
    *errno_ptr = DS_ENOMEM;
    (void)dss_ping_comm_mgr_abort_ping((dss_ping_handle)ping_sess_ptr, NULL, *errno_ptr);
    return DSS_PING_INVALID_HANDLE;
  }
  
  /* Copy all the ping options requested by the user. Copying data fields
   * is safer than copying the pointer to app's dss_ping_config_type structure
   * since we don't know how the user is handling this memory.
   */
  ping_sess_ptr->user_opts = *user_ping_options;

  ping_sess_ptr->app_callback_fn = app_callback_fn;
  ping_sess_ptr->app_ping_summary_callback_fn = app_ping_summary_callback_fn;

  ping_sess_ptr->app_user_data = app_user_data;

  if(net_policy_ptr != NULL)
  {
    ping_sess_ptr->net_policy = *net_policy_ptr;
    ping_sess_ptr->app_net_policy_specified = TRUE;
  }


  /* Init other values to default ones */
  ping_sess_ptr->sockfd = 0;
  memset(&(ping_sess_ptr->dest_ip_addr), 0, sizeof(struct ps_sockaddr));
  memset(&(ping_sess_ptr->sock_in6), 0, sizeof(struct ps_sockaddr_in6));
  memset( &(ping_sess_ptr->ping_stats),
          0,
          sizeof(ping_sess_ptr->ping_stats)  );

  memset( &(ping_sess_ptr->ping_session_stats),
          0,
          sizeof(ping_sess_ptr->ping_session_stats)  );
 
  memset( ping_sess_ptr->ping_read_buf, 0x0 , ping_read_buf_len );

  ping_sess_ptr->icmp_echo_req_seq_num = 0;
  ping_sess_ptr->icmp_echo_req_base_seq_num = 0;
  ping_sess_ptr->icmp_echo_rsp_timeout_seq_num = 0;

  ping_sess_ptr->ping_session_stats.min_rtt = DSS_PING_COMM_MGR_MAX_RTT;
  
  return (dss_ping_handle) ping_sess_ptr;
} /* dss_ping_comm_mgr_create_ping_session() */

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
)
{
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr;
  int16                               local_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    This critical section ensures that the processing of this ping abort in
    UI context is mutually exclusive from ping response timeout callback
    processing in PS context.
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&dss_ping_crit_section);

  LOG_MSG_INFO1_2("dss_ping_comm_mgr_abort_ping(): "
                  "Ping abort, handle %d, reason %s",
                  ping_handle,
                  (reason_ptr == NULL)? "unspecified" : "specfied" );

  if( (ping_sess_ptr = (dssi_ping_comm_mgr_ping_sess_type*)ping_handle) == NULL ||
      !dssi_ping_comm_mgr_is_sess_valid(ping_sess_ptr))
  {
    LOG_MSG_INFO1_0("dss_ping_comm_mgr_abort_ping(): "
                    "Invalid handle");
    PS_BRANCH_LEAVE_CRIT_SECTION(&dss_ping_crit_section);
    return DSS_ERROR;
  }

  /* Cancel any outstanding DNS sessions */
  if (0 != ping_sess_ptr->dns_session_handle)
  {
    (void) dss_dns_delete_session (ping_sess_ptr->dns_session_handle,
                                   &local_errno);
    ping_sess_ptr->dns_session_handle = 0;
    ping_sess_ptr->dns_query_handle   = 0;
  }

  /* Cancel and free all timers first */
  (void)ps_timer_cancel(ping_sess_ptr->retransmit_timer_handle);
  (void)ps_timer_cancel(ping_sess_ptr->response_timeout_timer_handle);

  if( ping_sess_ptr->retransmit_timer_handle != PS_TIMER_INVALID_HANDLE )
  {
    (void)ps_timer_free(ping_sess_ptr->retransmit_timer_handle);
  }

  if( ping_sess_ptr->response_timeout_timer_handle != PS_TIMER_INVALID_HANDLE )
  {
   (void)ps_timer_free(ping_sess_ptr->response_timeout_timer_handle);
  }

  /* Notify the user if required */
  if(reason_ptr)
  {
    LOG_MSG_INFO1_2("dss_ping_comm_mgr_abort_ping(): "
                    "Abort reason %d, dss_errno %d", *reason_ptr, dss_errno);

    /* If no response is ever received, this will be still be set to max possible
     * value. Set it to zero so it doesn't show up as -1.
     */
    if(ping_sess_ptr->ping_session_stats.min_rtt == DSS_PING_COMM_MGR_MAX_RTT)
    {
      ping_sess_ptr->ping_session_stats.min_rtt = 0;
    }

    ping_sess_ptr->app_ping_summary_callback_fn( (dss_ping_handle)ping_sess_ptr,
                                                 &(ping_sess_ptr->ping_session_stats),
                                                 ping_sess_ptr->app_user_data,
                                                 *reason_ptr,
                                                 dss_errno);
  }

  (void)dssi_ping_comm_mgr_close_conn(ping_sess_ptr, &dss_errno);
  dssi_ping_comm_mgr_free_ping_session(ping_sess_ptr);

  PS_LEAVE_CRIT_SECTION(&dss_ping_crit_section);
  return DSS_SUCCESS;
} /* dss_ping_comm_mgr_abort_ping() */


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
)
{
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr;
  int                                 ping_sess_msg_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (ping_sess_ptr = (dssi_ping_comm_mgr_ping_sess_type*)ping_handle) == NULL ||
      !dssi_ping_comm_mgr_is_sess_valid(ping_sess_ptr) )
  {
    LOG_MSG_INFO1_0("dss_ping_comm_mgr_send_ping(): "
                    "Invalid ping_handle");
    return;
  }

  ping_sess_msg_id = ping_sess_ptr->icmp_index;
  ps_send_cmd(PS_DSS_PING_SEND_PING_CMD, (void*)ping_sess_msg_id);

} /* dss_ping_comm_mgr_send_ping() */

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
  dss_ping_handle                     ping_handle,
  struct dss_dns_addrinfo            *addrinfo_ptr
)
{
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr;
  struct ps_sockaddr_in              *sock_ptr;
  struct ps_sockaddr_in6             *sock_in6_ptr;
  int16                               dss_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((ping_sess_ptr = (dssi_ping_comm_mgr_ping_sess_type*)ping_handle) == NULL ||
      !dssi_ping_comm_mgr_is_sess_valid(ping_sess_ptr))
  {
    LOG_MSG_INFO1_0("dss_ping_comm_mgr_update_ping_dest_addr(): "
                    "Invalid ping_handle");
    return;
  }

  if (NULL == addrinfo_ptr)
  {
    LOG_MSG_ERROR_0 ("dss_ping_comm_mgr_update_ping_dest_addr(): "
                     "NULL DNS addrinfo");
    return;
  }

  LOG_MSG_INFO1_1("dss_ping_comm_mgr_update_ping_dest_addr(): "
                  "updating address family %d ", 
                  addrinfo_ptr->ai_sockaddr.ps_ss_family);
  
  if (addrinfo_ptr->ai_sockaddr.ps_ss_family == DSS_AF_INET)
  {

    ping_sess_ptr->icmp_ver = DSSICMP_ICMP_V4;

    sock_ptr = (struct ps_sockaddr_in *) &(ping_sess_ptr->dest_ip_addr);
    memscpy (sock_ptr,
      sizeof (struct ps_sockaddr_in),
      &(addrinfo_ptr->ai_sockaddr), /* Copy the first address */
      sizeof (struct ps_sockaddr_in));

    /* Save the resolved destination IPv4 address */
    (void) dss_inet_ntoa (sock_ptr->ps_sin_addr,
                          (uint8*) ping_sess_ptr->ping_stats.resolved_ip_addr,
                          DSS_PING_RESOLVED_IP_ADDR_BUF_SIZE);
  }
  else if (addrinfo_ptr->ai_sockaddr.ps_ss_family == DSS_AF_INET6)
  {
    ping_sess_ptr->icmp_ver = DSSICMP_ICMP_V6;
    sock_in6_ptr = &(ping_sess_ptr->sock_in6);

    memscpy( sock_in6_ptr,
      sizeof (struct ps_sockaddr_in6),
      &(addrinfo_ptr->ai_sockaddr), /* Copy the first address */
      sizeof (struct ps_sockaddr_in6));

    /* Save the resolved destination IPv6 address */
    (void) dss_inet_ntop((char*)&(sock_in6_ptr->ps_sin6_addr),
                         DSS_AF_INET6,
                         (void*)ping_sess_ptr->ping_stats.resolved_ip_addr,
                         DSS_PING_RESOLVED_IP_ADDR_BUF_SIZE,
                         &dss_errno);
  }
  else
  {
    LOG_MSG_ERROR_1 ("dss_ping_comm_mgr_update_ping_dest_addr(): "
                     "Invalid addr family %d",
                     addrinfo_ptr->ai_sockaddr.ps_ss_family);
    return;
  }

  LOG_MSG_INFO1_1("dss_ping_comm_mgr_update_ping_dest_addr(): "
                  "addr resolved to %s",
                  ping_sess_ptr->ping_stats.resolved_ip_addr);

} /* dss_ping_comm_mgr_update_ping_dest_addr() */

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
)
{
  int                                 ping_sess_msg_id;
  struct dss_dns_addrinfo             hints;
  dssi_ping_comm_mgr_ping_sess_type  *ping_sess_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ping_sess_ptr = (dssi_ping_comm_mgr_ping_sess_type *) ping_handle;
  if (NULL == ping_sess_ptr)
  {
    LOG_MSG_ERROR_1 ("dss_ping_comm_mgr_perform_dns_lookup(): "
                     "Inv Ping session 0x%p", ping_sess_ptr);
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Create DNS session.
  -------------------------------------------------------------------------*/

  ping_sess_msg_id = ping_sess_ptr->icmp_index;
  ping_sess_ptr->dns_session_handle =
    dss_dns_create_session (dss_ping_dns_resolv_cb_fn,
                            (void *) ping_sess_msg_id,
                            dss_errno);
  if (DSS_DNS_SESSION_MGR_INVALID_HANDLE == ping_sess_ptr->dns_session_handle)
  {
    LOG_MSG_ERROR_1 ("dss_ping_comm_mgr_perform_dns_lookup(): "
                     "Err %d creating DNS session", *dss_errno);
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Set DNS config parameters before starting the query.
    Check if user has specified any network policy. If no network policy has
    been specified, default policy would be used by the DNS subsystem.
  -------------------------------------------------------------------------*/
  if(ping_sess_ptr->app_net_policy_specified == TRUE)
  {
    if( dss_dns_set_config_params( ping_sess_ptr->dns_session_handle,
                                   DSS_DNS_CONFIG_PARAMS_NET_POLICY,
                                   (void *)(&ping_sess_ptr->net_policy),
                                   sizeof(dss_net_policy_info_type),
                                   dss_errno) == DSS_ERROR )
    {
      LOG_MSG_ERROR_1("dss_ping_comm_mgr_perform_dns_lookup(): "
                      "Cannot set network policy, error %d", *dss_errno );
      return DSS_ERROR;
    }
  }

  /*-------------------------------------------------------------------------
    Create hints structure corresponding to address family.
  -------------------------------------------------------------------------*/
  memset (&hints, 0, sizeof(hints));
  hints.ai_family = addr_family;

  /*-------------------------------------------------------------------------
    Start addrinfo query.
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&dss_ping_crit_section);
  ping_sess_ptr->dns_query_handle =
    dss_dns_get_addrinfo (ping_sess_ptr->dns_session_handle,
                          dest_addr_ptr,
                          NULL,
                          &hints,
                          dss_errno);
  if (DSS_DNS_QUERY_INVALID_HANDLE == ping_sess_ptr->dns_query_handle)
  {
    LOG_MSG_ERROR_1 ("dss_ping_comm_mgr_perform_dns_lookup(): "
                     "Err %d in get_addrinfo", *dss_errno);
    PS_BRANCH_LEAVE_CRIT_SECTION(&dss_ping_crit_section);
    return DSS_ERROR;
  }
  PS_LEAVE_CRIT_SECTION(&dss_ping_crit_section);
  return DSS_SUCCESS;

}

/*===========================================================================
FUNCTION DSS_PING_RESOLV_CB_FN

DESCRIPTION
  This callback function is used by the DNS subsystem to notify about the
  resolved address. If the DNS subsystem has cached info about the query,
  this function will not be required. But if other DNS servers are being
  queried, this function will be put to use.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void dss_ping_dns_resolv_cb_fn
(
  dss_dns_session_mgr_handle_type     session_handle,
  dss_dns_query_handle_type           query_handle,
  dss_dns_api_type_enum_type          api_type,
  uint16                              num_records,
  void                              * user_data_ptr,
  int16                               dss_errno
)
{
  dss_ping_session_close_reason_type  reason;
  dssi_ping_comm_mgr_ping_sess_type * ping_sess_ptr;
  dss_dns_addrinfo                    dns_result;
  int16                               local_errno;
  int32                               ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&dss_ping_crit_section);
  if((ping_sess_ptr =
      dss_ping_comm_mgr_get_ping_sess_ptr((int)user_data_ptr))
     == NULL)
  {
    LOG_MSG_INFO1_0("dss_ping_dns_resolv_cb_fn(): "
                    "Invalid user data in ping_response_timeout");
    PS_BRANCH_LEAVE_CRIT_SECTION(&dss_ping_crit_section);
    return;
  }

  /*-----------------------------------------------------------------------
    Verify DNS callback is for correct Ping session.
  -----------------------------------------------------------------------*/
  if (ping_sess_ptr->dns_session_handle != session_handle ||
      ping_sess_ptr->dns_query_handle   != query_handle   ||
      DSS_DNS_API_ADDRINFO              != api_type)
  {
    LOG_MSG_INFO1_5 ("dss_ping_dns_resolv_cb_fn(): "
                     "Inv. DNS response for ping sess 0x%x "
                     "DNS session handle: %d, expected %d "
                     "DNS query handle: %d, expected %d",
                     ping_sess_ptr, session_handle, ping_sess_ptr->dns_session_handle,
                     query_handle, ping_sess_ptr->dns_query_handle);
    PS_BRANCH_LEAVE_CRIT_SECTION(&dss_ping_crit_section);
    return;
  }
  PS_LEAVE_CRIT_SECTION(&dss_ping_crit_section);

  /*-----------------------------------------------------------------------
    If resolution failed, notify the app and abort.
  -----------------------------------------------------------------------*/
  if (0 == num_records)
  {
    LOG_MSG_ERROR_1 ("dss_ping_dns_resolv_cb_fn(): "
                     "DNS resolv error %d", dss_errno);
    reason = DSS_PING_DNS_ERROR;
    goto bail;
  }

  /*-----------------------------------------------------------------------
    Get the resolved address.
  -----------------------------------------------------------------------*/
  memset ((void *) &dns_result, 0, sizeof(dns_result));
  ret_val = dss_dns_read_addrinfo (session_handle,
                                   query_handle,
                                   &dns_result,
                                   1,               /* Read first record */
                                   &local_errno);
  if (DSS_SUCCESS != ret_val)
  {
    LOG_MSG_ERROR_1 ("dss_ping_dns_resolv_cb_fn(): "
                     "DNS resolv read error %d", local_errno);
    reason = DSS_PING_DNS_ERROR;
    goto bail;
  }

  /*-----------------------------------------------------------------------
    DNS work is done. Clean up the DNS session.
  -----------------------------------------------------------------------*/
  (void) dss_dns_delete_session (session_handle, &local_errno);
  ping_sess_ptr->dns_session_handle = 0;
  ping_sess_ptr->dns_query_handle   = 0;

  /*-----------------------------------------------------------------------
    Update the Ping destination address and ping the same.
  -----------------------------------------------------------------------*/
  dss_ping_comm_mgr_update_ping_dest_addr ((dss_ping_handle)ping_sess_ptr, &dns_result);
  dss_ping_comm_mgr_send_ping((dss_ping_handle)ping_sess_ptr);
  return;

bail:
  (void) dss_ping_comm_mgr_abort_ping ((dss_ping_handle)ping_sess_ptr, &reason, dss_errno);
  return;

} /* dss_ping_dns_resolv_cb_fn() */



