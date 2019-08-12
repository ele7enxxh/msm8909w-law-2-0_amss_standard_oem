/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               D S S _ P S . C

GENERAL DESCRIPTION
   This task has the cmd processing for commands sent by the sockets layer
   to PS to be processed.

   These commands include commands to WRITE/CONNECT/CLOSE.

   Copyright (c) 2002-2014 Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/



/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dss_ps.c_v   1.13   24 Feb 2003 18:32:12   akuzhiyi  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dss_ps.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/30/14    vl     Moved IPv6 related NV item to ps_ip6_sm.c file.
11/01/13    rk     Removed feature mobileap.
01/30/13    ds     Replacing LOG_MSG_FATAL_ERROR macro with LOG_MSG_ERROR.
12/14/12    fn     Remove default NV item writes when NV read fails 
11/05/12    fn     Optimizing NV read for NV IPV6 ENABLED item  
05/21/12    am     Invoking NAT callbacks for DSM small item events.
12/23/10    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
10/01/10    sa     Added appropriate log messages before ASSERT(0).
04/06/10    am     Making sure queues are zeroed out before init.
05/22/09    kk     Q6 compiler warning fixes.
03/13/08    pp     Metainfo optimizations.
06/26/07    rs     Added code to register command handler for IGMP/MLD
                   Multicasting.
04/29/07    ss     Added support for ICMPv6 sockets (PS_IPPROTO_ICMP6).
04/11/07    msr    Posting a cmd to PS in dsm_mem_event_ctrl
01/29/07    hm     Moved Q initialations to dssps_powerup_init()
11/21/06    rt     Added IGMP/MLD support for MBMS.
11/06/06    sv     Added PS command to process socket events.
11/02/06    mct    Added event queues for IPv6 privacy extensions.
10/13/06    msr    Handling PS_SOCKET_CMD_FLOW_ENABLED_IND
07/31/06    rt     Route Scope changes.
04/12/06    msr    Removed a F3 msg from data path
03/27/06    rt     Added a new case PS_SOCKET_CMD_WINDOW_REOPEN.
02/22/06    msr    Using single critical section
02/07/06    rt     L4 TASKLOCK changes.
12/19/05    msr    Removed PS_SOCKET_FLOW_ENABLED_CMD
12/03/05    rt     Added sockfd check to insure that the socket is not
                   freed while the command was in the queue.
11/29/05    sv     Added support for RSVP
08/15/05    ks     Changed PS_SOCKET_FLOW_ENABLED_SIG to CMD.
05/26/05    rt     Initialized tcp_count in powerup_init.
06/20/05    mct    Added the Multicast event queue.
04/18/05    vp     Change in signature of bind and listen functions.
04/17/05    msr    Setting command handler for PS_FLOW_EVENT_CMD_TYPE.
09/30/04    ks     Registering low wm callback for PS_MEM_META_INFO_TYPE in
                   dssps_powerup_init().
08/30/04    ks/msr Initialized SCB/ACB/TCB/UCB/ICMB queues in powerup_init
                   and removed dsstcp_init_tcb_array() call from init.
08/23/04    sv     Split the function dssps_init into powerup_init and init.
07/11/04    aku    Added suport for the QOS feature.
08/11/04    mvl    Changed the way dssps_init() initializes the default Any
                   IP family policy.
08/06/04    sv     Updated dssps_init() to read the IPV6 fail over config
                   and set the failover policy in sock cfg control block.
07/30/04    sv     Initialize socket configuration control block.
07/22/04    msr    Renamed the function ps_mem_reg_low_wm_cb to
                   ps_mem_reg_mem_avail_cb.
06/04/04    sv     Updated dssps_init to register for low watermark call back
                   of ps memory items.
04/30/04    sv     Fixed potential memory leak in process_socket_cmd.
03/30/04    vp     Merged changes from June04Dev_05.00.00 PS branch.
12/12/03    sv     Consolidated PS commands for iface events into one PS cmd.
10/24/03    sv     Added command processing functions for new ps commands
                   to support transient state call backs.
10/17/03    sv     Modified protocol specific functions to take protocl
                   control block as an argument.
10/01/03    sv     Added ps cmd code to handle PS bind request.
08/28/03    ss     Removed some no more relevant comments
08/25/03    aku    ps_mem_free() takes pointer-to-pointer as arg.
08/12/03    sv     Fixed the bug in handling socket option arguments.
08/08/03    sv     Restructured the sockets code.
07/18/03    ss     Get the sockfd from the socket command and validate that
                   the scb_ptr points to the correct socket.
02/24/02    aku    Access ps_iface_ptr from scb instead of acb.
02/13/03    aku    Added support to notify sockets when DSM becomes available
                   and also fixed bug wherein sockets could not be closed
                   once dss_pppclose() is called.
02/13/03    ss     Added socket shutdown command processing
01/24/03    usb    Removed unused variable to fix compiler warning
12/24/02    aku    Register for PS_SOCKET_PHYS_LINK_UP_EVENT_CMD and
                   PS_SOCKET_PHYS_LINK_DOWN_EVENT_CMD command handlers with
                   PS.
12/22/02    aku    IFACE_UP and IFACE_DOWN events have independent cmd
                   handlers.
10/08/02    om     Fixed preservation of app-field across send_q.
10/06/02    aku    Added support for processing iface callbacks in PS task
                   context.
10/06/02    aku    Added support for processing iface callbacks in PS task
10/02/02    mvl    fixed meta info memory leak.
09/18/02    aku    Added check for NULL ps_iface_ptr prior to calling the
                   abort function.
09/11/02    aku    Added support for aborting protocol stack if interface is
                   configured to do so.
07/16/02    aku    Include dss_ps.h
07/14/02    na/aku Created.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "amssassert.h"
#include "msg.h"
#include "ps_svc.h"
#include "queue.h"
#include "dsm.h"
#include "dss_ps.h"
#include "ps_mem.h"
#include "ps_pkt_meta_info.h"
#include "ps_rt_meta_info.h"
#include "ds_flow_control.h"
#include "ps_socket_cmd.h"
#include "ps_socket_defs.h"
#include "ps_socket_event_defs.h"
#include "ps_utils.h"

#include "ps_mcasti.h"

#include "dsstcp.h"
#include "dssudp.h"
#include "dssicmp.h"
#include "ps_tcp.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"
#include "dcc_task_defs.h"
#include "dcc_task_svc.h"

/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
q_type ps_socket_send_q;

/*---------------------------------------------------------------------------
  Socket configuration control block
---------------------------------------------------------------------------*/
struct dss_socket_config_s sock_config_cb;

/*---------------------------------------------------------------------------
  protocol specific function pointer table - contains function pointers
  to "common" socket functions, that are implemented differently, depending
  on the protocol being used.
---------------------------------------------------------------------------*/
typedef struct
{
  int protocol;                                             /* Socket type */

  /* Protocol specific connect function */
  void (*connect)
  (
    int32                     pcb_handle,
    int32                     sockfd,
    struct ps_sockaddr_in6  * dest_ptr
  );

  /* Protocol specific listen function */
  void (*listen)
  (
    int32  pcb_handle,
    int32  sockfd,
    int32  backlog,
    int32  rcv_window
  );

  /* Protocol specific write function */
  void (*write)
  (
     int32                     pcb_handle,
     int32                     sockfd,
     int32                     length,
     struct ps_sockaddr_in6  * ta_ptr,
     ps_pkt_meta_info_type   * pkt_mi_ptr
  );

  /* Protocol specific shutdown function */
  void (*shutdown)
  (
    int32  pcb_handle,
    int32  sockfd,
    int32  shutdown_dir
  );

  /* Protocol specific close function */
  void (*close)
  (
    int32    pcb_handle,
    int32    sockfd,
    boolean  silent_close_flag
  );

  /* Protocol specific bind function */
  void (*bind)
  (
    int32                     pcb_handle,
    int32                     sockfd,
    struct ps_sockaddr_in6  * local_addr_ptr
  );

  /* Protocol specific setopt function */
  void (*setopt)
  (
    int32  pcb_handle,
    int32  sockfd,
    int32  level,
    int32  name,
    int32  val,
    int32  len
  );

  /* Protocol specific function to flow enable on socket's
     routing cache/flow cache */
  void (*flow_enabled_ind)
  (
    int32  pcb_handle,
    int32  sockfd
  );

  void (*set_route_meta_info)
  (
    int32                   pcb_handle,
    int32                   sockfd,
    ps_rt_meta_info_type  * rt_mi_ptr
  );

  void (*set_route_scope)
  (
    int32                  pcb_handle,
    int32                  sockfd,
    ps_route_scope_type  * route_scope_ptr
  );

  void (*set_linger_reset)
  (
    int32    pcb_handle,
    int32    sockfd,
    boolean  is_linger_on,
    int32    time_in_sec
  );

} dss_ps_fcn_ptr_table_type;

/*---------------------------------------------------------------------------
  Socket function pointer table.  The functions correspond to the
  protocol specific function calls.
  follows:

  The final entry in the table is DSS_ERROR.
---------------------------------------------------------------------------*/
static dss_ps_fcn_ptr_table_type dss_ps_fcn_ptr_table[] =
{
  {
  /*-------------------------------------------------------------------------
    TCP functions - found in dsstcp.c
  -------------------------------------------------------------------------*/
    (int)PS_IPPROTO_TCP,
    dsstcp_connect,
    dsstcp_listen,
    dsstcp_write,
    dsstcp_shutdown,
    dsstcp_close,
    dsstcp_bind,
    dsstcp_setopt,
    dsstcp_flow_enabled_ind,
    dsstcp_set_route_meta_info,
    dsstcp_set_route_scope,
    dsstcp_set_linger_reset
  },
  {
  /*-------------------------------------------------------------------------
    UDP functions - found in dssudp.c
  -------------------------------------------------------------------------*/
    (int)PS_IPPROTO_UDP,
    dssudp_connect,
    NULL,
    dssudp_write,
    dssudp_shutdown,
    dssudp_close,
    dssudp_bind,
    dssudp_setopt,
    NULL,
    dssudp_set_route_meta_info,
    dssudp_set_route_scope,
    NULL
  },
  {
  /*-------------------------------------------------------------------------
    ICMP functions - found in dssicmp.c
  -------------------------------------------------------------------------*/
    (int)PS_IPPROTO_ICMP,
    NULL,
    NULL,
    dssicmp_write,
    NULL,
    dssicmp_close,
    NULL,
    dssicmp_setopt,
    NULL,
    dssicmp_set_route_meta_info,
    NULL,
    NULL
  },
  {
  /*-------------------------------------------------------------------------
    End of table
  -------------------------------------------------------------------------*/
    DSS_ERROR,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  }
};

socket_platform_event_cback_f_ptr_type  socket_platform_event_cback_f_ptr;

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =

                    INTERNAL FUNCTION DEFINTIONS

= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION DSSOCKI_GET_FCNPTR_TABLE

DESCRIPTION
  Returns the protocol specific function pointer table.

DEPENDENCIES
  None.

RETURN VALUE
  function pointer table

SIDE EFFECTS
  Function pointer table is assinged.
===========================================================================*/
static dss_ps_fcn_ptr_table_type * dss_ps_get_fcnptr_table
(
 int protocol                                        /* transport protocol */
)
{
  dss_ps_fcn_ptr_table_type *  sp = NULL;   /* function pointer table item */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Look for entry in protocol table - break when socket protocol type
    matches the protocol type of the function pointer table.
  -------------------------------------------------------------------------*/
  for (sp = dss_ps_fcn_ptr_table; sp->protocol != DSS_ERROR; sp++)
  {
    if (protocol == sp->protocol)
    {
      break;
    }
  }

  return sp;

} /* dss_ps_get_fcnptr_table() */


/*===========================================================================
FUNCTION DSSPS_PROCESS_SOCKET_CMD()

DESCRIPTION
  This function process the protocol packets that need to be sent by a socket
  Each item of the psi_socket_send_q is a protocol PDU and contains the
  pointer to the socket control block. This function removes the pointer to
  the socket control block from the enqueued item and calls the corresponding
  send routine for the socket.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: When ps_socket_send_q is empty, clear signal from set signal mask
  FALSE: Don't clear signal yet - wait for send_q to be emptied out.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean dssps_process_socket_cmd
(
  ps_sig_enum_type    sig,                 /* Signal to be processed       */
  void             *  user_data_ptr        /* Parameter carrying user data */
)
{
  ps_socket_cmd_type         * ps_cmd_ptr;                   /* PS command */
  dss_ps_fcn_ptr_table_type  * sp;          /* function pointer table item */
  (void) sig;
  (void) user_data_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the item from the ps_socket_send_q.
    If queue is empty then then ask for the signal to be removed.
    Note, we don't empty the queue in a while loop. Reason being that we want
    to process one write cmd for socket and then give a chance for a packet
    to be read thus balancing the flows.
  -------------------------------------------------------------------------*/
  ps_cmd_ptr = (ps_socket_cmd_type *) q_get( &ps_socket_send_q);
  if (ps_cmd_ptr == NULL)
  {
    return TRUE;
  }

  if (ps_cmd_ptr->protocol != (int32)PS_IPPROTO_TCP &&
      ps_cmd_ptr->protocol != (int32)PS_IPPROTO_UDP &&
      ps_cmd_ptr->protocol != (int32)PS_IPPROTO_ICMP)
  {
    LOG_MSG_INFO1_1("Unknown protocol %d", ps_cmd_ptr->protocol);
    PS_MEM_FREE(ps_cmd_ptr);
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    get protocol specific function pointer table
  -------------------------------------------------------------------------*/
  sp = dss_ps_get_fcnptr_table(ps_cmd_ptr->protocol);
  ASSERT(sp != NULL);

  /*-------------------------------------------------------------------------
    Call protocol specific functions depending on the command.
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO3_3("Command: %d, Sockfd: %d, Protocol %d",
                ps_cmd_ptr->kind, ps_cmd_ptr->sockfd, ps_cmd_ptr->protocol);
  switch (ps_cmd_ptr->kind)
  {
    case PS_SOCKET_CMD_PAYLOAD:
      if (sp->write != NULL)
      {
        sp->write( ps_cmd_ptr->pcb_handle,
                   ps_cmd_ptr->sockfd,
                   ps_cmd_ptr->user_data.write_data.length,
                   &( ps_cmd_ptr->user_data.write_data.dest_addr),
                   ps_cmd_ptr->user_data.write_data.pkt_meta_info_ptr);
      }
      else
      {
        LOG_MSG_INFO1_2("NULL WRITE func, sockfd %d, protocol %d",
                        ps_cmd_ptr->sockfd, ps_cmd_ptr->protocol);
      }
      break;

    case PS_SOCKET_CMD_CONNECT:
      if (sp->connect != NULL)
      {
        sp->connect( ps_cmd_ptr->pcb_handle,
                     ps_cmd_ptr->sockfd,
                     &( ps_cmd_ptr->user_data.dest_addr));
      }
      else
      {
        LOG_MSG_INFO1_2("NULL CONNECT func, sockfd %d, protocol %d",
                        ps_cmd_ptr->sockfd, ps_cmd_ptr->protocol);
      }
      break;

    case PS_SOCKET_CMD_CLOSE:
      if (sp->close != NULL)
      {
        sp->close( ps_cmd_ptr->pcb_handle,
                   ps_cmd_ptr->sockfd,
                   ps_cmd_ptr->user_data.silent_close_flag);
      }
      else
      {
        LOG_MSG_INFO1_2("NULL CLOSE func, sockfd %d, protocol %d",
                        ps_cmd_ptr->sockfd, ps_cmd_ptr->protocol);
      }
      break;

    case PS_SOCKET_CMD_SHUTDOWN:
      if (sp->shutdown != NULL)
      {
        sp->shutdown( ps_cmd_ptr->pcb_handle,
                      ps_cmd_ptr->sockfd,
                      ps_cmd_ptr->user_data.shutdown_dir);
      }
      else
      {
        LOG_MSG_INFO1_2("NULL SHUTDOWN func, sockfd %d, protocol %d",
                        ps_cmd_ptr->sockfd, ps_cmd_ptr->protocol);
      }
      break;

    case PS_SOCKET_CMD_LISTEN:
      if (sp->listen != NULL)
      {
        sp->listen( ps_cmd_ptr->pcb_handle,
                    ps_cmd_ptr->sockfd,
                    ps_cmd_ptr->user_data.listen_data.backlog,
                    ps_cmd_ptr->user_data.listen_data.rcv_buf);
      }
      else
      {
        LOG_MSG_INFO1_2("NULL LISTEN func, sockfd %d, protocol %d",
                        ps_cmd_ptr->sockfd, ps_cmd_ptr->protocol);
      }
      break;

    case PS_SOCKET_CMD_SETOPT:
      if (sp->setopt != NULL)
      {
        sp->setopt( ps_cmd_ptr->pcb_handle,
                    ps_cmd_ptr->sockfd,
                    ps_cmd_ptr->user_data.sock_opt_data.level,
                    ps_cmd_ptr->user_data.sock_opt_data.name,
                    ps_cmd_ptr->user_data.sock_opt_data.val,
                    ps_cmd_ptr->user_data.sock_opt_data.len);
      }
      else
      {
        LOG_MSG_INFO1_2("NULL SETOPT func, sockfd %d, protocol %d",
                        ps_cmd_ptr->sockfd, ps_cmd_ptr->protocol);
      }

      break;

    case PS_SOCKET_CMD_BIND:
      if (sp->bind != NULL)
      {
        sp->bind( ps_cmd_ptr->pcb_handle,
                  ps_cmd_ptr->sockfd,
                  &( ps_cmd_ptr->user_data.local_addr));
      }
      else
      {
        LOG_MSG_INFO1_2("NULL BIND func, sockfd %d, protocol %d",
                        ps_cmd_ptr->sockfd, ps_cmd_ptr->protocol);
      }
      break;

    case PS_SOCKET_CMD_FLOW_ENABLED_IND:
      if (sp->flow_enabled_ind != NULL)
      {
        sp->flow_enabled_ind( ps_cmd_ptr->pcb_handle, ps_cmd_ptr->sockfd);
      }
      else
      {
        LOG_MSG_INFO1_2("NULL FLOW_ENABLED_IND func, sockfd %d, protocol %d",
                        ps_cmd_ptr->sockfd, ps_cmd_ptr->protocol);
      }
      break;

    case PS_SOCKET_CMD_SET_ROUTE_META_INFO:
      if (sp->set_route_meta_info != NULL)
      {
        sp->set_route_meta_info( ps_cmd_ptr->pcb_handle,
                                 ps_cmd_ptr->sockfd,
                                 ps_cmd_ptr->user_data.rt_meta_info_ptr);
      }
      else
      {
        PS_RT_META_INFO_FREE( &( ps_cmd_ptr->user_data.rt_meta_info_ptr));
        LOG_MSG_INFO1_2("NULL SET_ROUTE_META_INFO func, sockfd %d, protocol %d",
                        ps_cmd_ptr->sockfd, ps_cmd_ptr->protocol);
      }

      break;

    case PS_SOCKET_CMD_SET_ROUTE_SCOPE:
    {
      if (sp->set_route_scope != NULL)
      {
        sp->set_route_scope( ps_cmd_ptr->pcb_handle,
                             ps_cmd_ptr->sockfd,
                             &( ps_cmd_ptr->user_data.route_scope));
      }
      else
      {
        LOG_MSG_INFO1_2("NULL SET_ROUTE_SCOPE func, sockfd %d, protocol %d",
                        ps_cmd_ptr->sockfd, ps_cmd_ptr->protocol);
      }

      break;
    }

    case PS_SOCKET_CMD_LINGER_RESET:
    {
      if (sp->set_linger_reset != NULL)
      {
        sp->set_linger_reset( ps_cmd_ptr->pcb_handle,
                              ps_cmd_ptr->sockfd,
                              ps_cmd_ptr->user_data.linger_reset_data.is_linger_on,
                              ps_cmd_ptr->user_data.linger_reset_data.time_in_sec);
      }
      else
      {
        LOG_MSG_INFO1_2("NULL SET_LINGER_RESET func, sockfd %d, protocol %d",
                        ps_cmd_ptr->sockfd, ps_cmd_ptr->protocol);
      }

      break;
    }

    default:
      LOG_MSG_ERROR_1("unknown cmd %d", ps_cmd_ptr->kind);
      ASSERT( 0);
  }

  PS_MEM_FREE(ps_cmd_ptr);
  return FALSE;

} /* dssps_process_socket_cmd() */


/*===========================================================================
FUNCTION DSS_MEM_EVENT_CTRL()

DESCRIPTION
  This function notify sockets apps when DSM memory is available.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void dss_mem_event_ctrl
(
  dsm_mempool_id_enum_type pool_id,                      /* memory pool ID */
  dsm_mem_level_enum_type event,                   /* type of memory event */
  dsm_mem_op_enum_type op
)
{
  static uint32 dsm_mem_mask=0;
  (void) op;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(pool_id == DSM_DS_SMALL_ITEM_POOL)
  {
    switch (event)
    {
    case DSM_MEM_LEVEL_MANY:
      /*-----------------------------------------------------------------
        Notify sockets applications if the mobile has previously run out
        of both large and small items.
      -----------------------------------------------------------------*/
      LOG_MSG_INFO3_0(" SMALL ITEM MANY ITEMS MARK Reached");
      if(dsm_mem_mask == (DS_FLOW_DSM_SMALL_Q_MASK |
                          DS_FLOW_DSM_LARGE_Q_MASK))
      {
        LOG_MSG_INFO2_1("Small items available, mask=0x%x", dsm_mem_mask);
        ps_send_cmd(PS_DSM_MEM_EVENT_CTRL_CMD,
                    (void *) PS_SOCKET_DSM_ITEM_AVAILABLE_EV);
      }

      dsm_mem_mask &= ~(DS_FLOW_DSM_SMALL_Q_MASK);
      break;

    case DSM_MEM_LEVEL_FEW:
      /*-----------------------------------------------------------------
        Update the static mask.
      -----------------------------------------------------------------*/
      LOG_MSG_INFO3_0(" SMALL ITEM FEW ITEMS MARK Reached");
      dsm_mem_mask |= DS_FLOW_DSM_SMALL_Q_MASK;

      if(dsm_mem_mask == (DS_FLOW_DSM_SMALL_Q_MASK |
                          DS_FLOW_DSM_LARGE_Q_MASK))
      {
        ps_send_cmd(PS_DSM_MEM_EVENT_CTRL_CMD,
                    (void *) PS_SOCKET_DSM_ITEM_NOT_AVAILABLE_EV);
      }

      break;

    default:
      LOG_MSG_INFO3_0("Do nothing");
    }
  }
  else if(pool_id == DSM_DS_LARGE_ITEM_POOL)
  {
    switch (event)
    {
    case DSM_MEM_LEVEL_MANY:
      /*-----------------------------------------------------------------
        Notify sockets applications if the mobile has previously run out
        of both large and small items.
      -----------------------------------------------------------------*/
      LOG_MSG_INFO3_0(" LARGE ITEM MANY ITEMS MARK Reached");
      if(dsm_mem_mask == (DS_FLOW_DSM_SMALL_Q_MASK |
                          DS_FLOW_DSM_LARGE_Q_MASK))
      {
        LOG_MSG_INFO2_1("Large items available, mask=0x%x", dsm_mem_mask);
        ps_send_cmd(PS_DSM_MEM_EVENT_CTRL_CMD,
                    (void *) PS_SOCKET_DSM_ITEM_AVAILABLE_EV);
      }
      dsm_mem_mask &= ~(DS_FLOW_DSM_LARGE_Q_MASK);
      break;

    case DSM_MEM_LEVEL_FEW:
      /*-----------------------------------------------------------------
        Update the static mask.
      -----------------------------------------------------------------*/
      LOG_MSG_INFO3_0(" LARGE ITEM FEW ITEMS MARK Reached");
      dsm_mem_mask |= DS_FLOW_DSM_LARGE_Q_MASK;

      if(dsm_mem_mask == (DS_FLOW_DSM_SMALL_Q_MASK |
                          DS_FLOW_DSM_LARGE_Q_MASK))
      {
        ps_send_cmd(PS_DSM_MEM_EVENT_CTRL_CMD,
                    (void *) PS_SOCKET_DSM_ITEM_NOT_AVAILABLE_EV);
      }

      break;

    default:
      LOG_MSG_INFO3_0("Do nothing");
    }
  }
} /* dss_mem_event_ctrl() */


/*===========================================================================
FUNCTION DSSPS_PROCESS_DSM_MEM_EVENT_CTRL_CMD()

DESCRIPTION
  This function processes the DSM memory related events posted by DSM layer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssps_process_dsm_mem_event_ctrl_cmd
(
  ps_cmd_enum_type    cmd_name,
  void              * user_info_ptr
)
{
  ps_socket_event_info_type  socket_event_info;
  (void) cmd_name;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  socket_event_info.scb_ptr = NULL;
  socket_platform_event_cback_f_ptr(
           (ps_socket_event_enum_type)(uint32)user_info_ptr,
                 &socket_event_info);

} /* dssps_process_dsm_mem_event_ctrl_cmd() */


/*===========================================================================

                    EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSPS_POWERUP_INIT()

DESCRIPTION
  Powerup initialization of the sockets stuff in the PS task.

DEPENDENCIES
  Called at powerup from the PS context

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssps_powerup_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the Socket command queue (from the app tasks to PS.)
  -------------------------------------------------------------------------*/
  memset(&ps_socket_send_q, 0, sizeof(q_type));
  (void) q_init( &ps_socket_send_q);

  /*-------------------------------------------------------------------------
    Register sockets command handler for all ps_iface events.
  -------------------------------------------------------------------------*/
 /*-------------------------------------------------------------------------
    Register command handler for IGMP/MLD Multicasting.
  -------------------------------------------------------------------------*/
  (void)
    ps_set_cmd_handler( PS_MCAST_SEND_REPORT_CMD, ps_mcasti_send_report_msg);
  (void) ps_set_cmd_handler( PS_MCAST_SEND_LEAVE_CMD, ps_mcasti_send_leave_msg);
  (void) ps_set_cmd_handler( PS_MCAST_JOIN_CMD, ps_mcasti_join);
  (void) ps_set_cmd_handler( PS_MCAST_LEAVE_CMD, ps_mcasti_leave);

  /*-------------------------------------------------------------------------
    Register handlers and enable signals used by Sockets.
  -------------------------------------------------------------------------*/
  (void)
  ps_set_sig_handler( PS_SOCKET_RX_Q_SIGNAL, dssps_process_socket_cmd, NULL);
  ps_enable_sig( PS_SOCKET_RX_Q_SIGNAL);

  /*-------------------------------------------------------------------------
    Register for DSM memory events (both SMALL and LARGE items)
  -------------------------------------------------------------------------*/
  dsm_reg_mem_event_cb ( DSM_DS_SMALL_ITEM_POOL,
                         DSM_MEM_LEVEL_MANY,
                         DSM_MEM_OP_FREE,
                         dss_mem_event_ctrl);

  dsm_reg_mem_event_cb ( DSM_DS_SMALL_ITEM_POOL,
                         DSM_MEM_LEVEL_FEW,
                         DSM_MEM_OP_NEW,
                         dss_mem_event_ctrl);

  dsm_reg_mem_event_cb ( DSM_DS_LARGE_ITEM_POOL,
                         DSM_MEM_LEVEL_MANY,
                         DSM_MEM_OP_FREE,
                         dss_mem_event_ctrl);

  dsm_reg_mem_event_cb ( DSM_DS_LARGE_ITEM_POOL,
                         DSM_MEM_LEVEL_FEW,
                         DSM_MEM_OP_NEW,
                         dss_mem_event_ctrl);

  /*-------------------------------------------------------------------------
    Register command handler for all DSM memory events
  -------------------------------------------------------------------------*/
  (void) ps_set_cmd_handler( PS_DSM_MEM_EVENT_CTRL_CMD,
                             dssps_process_dsm_mem_event_ctrl_cmd);


  /*-------------------------------------------------------------------------
    Processing related to the meaning of app requests for Any IP family: by
    default assume that these requests map to IPv4 only.
  -------------------------------------------------------------------------*/
  memset(&sock_config_cb, 0, sizeof(struct dss_socket_config_s));
  sock_config_cb.any_network_policy = IFACE_IPV4_ADDR_FAMILY;

  /*-------------------------------------------------------------------------
    Initialize various control block queues
  -------------------------------------------------------------------------*/
  (void) q_init(&sock_config_cb.tcp_cb_q);
  (void) q_init(&sock_config_cb.udp_cb_q);
  (void) q_init(&sock_config_cb.icmp_cb_q);
  (void) q_init(&sock_config_cb.rsvp_cb_q);
} /* dssps_powerup_init() */


/*===========================================================================
FUNCTION DSSPS_INIT()

DESCRIPTION
   Initialization of the sockets stuff in the PS task.

DEPENDENCIES
  Called at initialization from the PS context

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssps_init(void)
{
  nv_stat_enum_type nv_status;
  nv_item_type     *ps_nv_item_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  ps_nv_item_ptr = (nv_item_type *)
    ps_system_heap_mem_alloc(sizeof(nv_item_type));
  if( ps_nv_item_ptr == NULL )
  {
    DATA_ERR_FATAL("Mem alloc from system heap failed.");
  }

 /*-------------------------------------------------------------------------
   * Retrieve the value of tcp_graceful_dormant_close flag from the NV
  -------------------------------------------------------------------------*/
  nv_status = ps_get_nv_item( NV_TCP_GRACEFUL_DORMANT_CLOSE_I,
                              ps_nv_item_ptr );
  if( nv_status != NV_DONE_S )
  {
    LOG_MSG_INFO1_0("TCP graceful dormant close flag unavailable, using default");
    /*-----------------------------------------------------------------------
     * set the global variable with default value
     ----------------------------------------------------------------------*/
    sock_config_cb.tcp_graceful_dormant_close = FALSE;
  }
  else
  {
     sock_config_cb.tcp_graceful_dormant_close =
       ps_nv_item_ptr->tcp_graceful_dormant_close;
  }

  /*-------------------------------------------------------------------------
    Retrieve the value of tcp_keepalive_idle_time from the NV
  -------------------------------------------------------------------------*/
  nv_status = ps_get_nv_item(NV_TCP_KEEPALIVE_IDLE_TIME_I, ps_nv_item_ptr);
  if( nv_status != NV_DONE_S )
  {
    LOG_MSG_INFO1_0("TCP keepalive idle time unavailable, using default");
    /*-----------------------------------------------------------------------
      set the global variable with default value in milliseconds
     ----------------------------------------------------------------------*/
    sock_config_cb.tcp_keepalive_idle_time = TCP_KEEPALIVE_IDLE_TIME;
  }
  else
  {
    /*-----------------------------------------------------------------------
     The NV keepalive time is supplied in minutes. Convert it to milliseconds
    -----------------------------------------------------------------------*/
     sock_config_cb.tcp_keepalive_idle_time =
       ((ps_nv_item_ptr->tcp_keepalive_idle_time) * 60 * 1000);
  }

  PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
  return;
} /* dssps_init() */


void dssps_reg_event_cback
(
  socket_platform_event_cback_f_ptr_type  event_cback_f_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  socket_platform_event_cback_f_ptr = event_cback_f_ptr;
} /* dss_ps_reg_event_cback() */

/*===========================================================================
FUNCTION dssps_set_network_policy_setting

DESCRIPTION
  Sets the Network Policy setting in Sockets config.

DEPENDENCIES

RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void dssps_set_network_policy_setting
(
  ip_addr_enum_type nw_policy
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sock_config_cb.any_network_policy = nw_policy;  
} /* dssps_set_network_policy_setting() */