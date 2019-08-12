/*===========================================================================

                          P S _ S T A T _ T C P . C

GENERAL DESCRIPTION
  This program specifically deals with statistics at the tcp layer of the
  data servcies stack

EXTERNALIZED FUNCTIONS
  ps_get_tcp_stats
    This function supplies the required tcp stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_stat_tcp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/27/10   rp      RAM optimization changes.
12/23/10   ss      Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
12/23/09   cp      Lint Fixes
04/11/07   scb     Fixed High LINT errors
02/27/07   scb     Added changes to fix RCVT compilation warning
08/15/05   kr      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "IxErrno.h"
#include "err.h"

#include "ps_stat_tcp.h"
#include "ps_stat_common.h"
#include "ps_stat_logging.h"
#include "ps_crit_sect.h"
#include "ps_tcp.h"
#include "ds_Utils_DebugMsg.h"

#include <stddef.h>
#include <string.h>


/*===========================================================================

                             LOCAL DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
 Statistics declaration
---------------------------------------------------------------------------*/
ps_stat_tcp_g_s_type tcp_stats  =
{
  0,         /*  crnt_open_conns       */
  0,         /*  bad_hdr_len           */
  0,         /*  bad_chksum            */
  0,         /*  too_short             */
  0,         /*  conn_aborts           */
  0,         /*  conn_timeout_aborts   */
  0,         /*  conn_keepalive_aborts */
  0,         /*  rst_tx                */
  0,         /*  rst_rx                */
  0,         /*  dropped_segs_noconn_rx*/
  0,         /*  dropped_segs_rx       */
  0,         /*  bytes_tx              */
  0,         /*  bytes_rx              */
  0,         /*  segs_tx               */
  0,         /*  segs_rx               */
  0,         /*  bytes_rexmit          */
  0,         /*  segs_rexmit           */
  0,         /*  dup_bytes_rx          */
  0,         /*  dup_segs_rx           */
  0,         /*  window_probe_tx       */
  0,         /*  out_of_order_segs_rx  */
  0,         /*  out_of_order_bytes_rx */
  0,         /*  rexmit_timeouts       */
  0,         /*  piggybacked_acks_tx   */
  0,         /*  piggybacked_acks_rx   */
  0,         /*  dup_acks_rx           */
  0          /*  dup_acks_tx           */
};

/*===========================================================================
MACRO TCP_STAT_OFFSET_SIZE(INDEX,FIELD)

DESCRIPTION
  This macro fills the offset and size fields of the ps_stat_tcp_g_table.
  INDEX is used only for readability, not for usage

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define TCP_STAT_OFFSET_SIZE(INDEX,FIELD)                               \
{                                                                       \
  FPOS_STAT(ps_stat_tcp_g_s_type,FIELD),                                \
  FSIZ(ps_stat_tcp_g_s_type,FIELD)                                      \
}

/*===========================================================================
MACRO TCP_INSTANCE_STAT_OFFSET_SIZE(INDEX,FIELD,SIZE)

DESCRIPTION
  This macro fills the offset and size fields of the ps_stat_tcp_i_table.
  INDEX is used only for readability, not for usage

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define TCP_INSTANCE_STAT_OFFSET_SIZE(INDEX,FIELD,SIZE)                 \
{                                                                       \
  FPOS_STAT(struct tcb,FIELD),                                          \
  sizeof(SIZE)                                                          \
}

/*---------------------------------------------------------------------------
  Global statistics table:
  Now, we declare a table that will be used to get the necessary statistic
  based on the enumeration value passed. This is done to avoid the
  switch-case structure that would have been necessary to return the
  appropriate member of the tcp_stats structure.
---------------------------------------------------------------------------*/
static const ps_stat_access_table_s_type ps_stat_tcp_g_table[PS_STAT_TCP_ALL + 1] = 
{
  TCP_STAT_OFFSET_SIZE(0,crnt_open_conns		),
  TCP_STAT_OFFSET_SIZE(1,bad_hdr_len			),
  TCP_STAT_OFFSET_SIZE(2,bad_chksum 			),
  TCP_STAT_OFFSET_SIZE(3,too_short				),
  TCP_STAT_OFFSET_SIZE(4,conn_aborts			),
  TCP_STAT_OFFSET_SIZE(5,conn_timeout_aborts	),
  TCP_STAT_OFFSET_SIZE(6,conn_keepalive_aborts	),
  TCP_STAT_OFFSET_SIZE(7,rst_tx 				),
  TCP_STAT_OFFSET_SIZE(8,rst_rx 				),
  TCP_STAT_OFFSET_SIZE(9,dropped_segs_noconn_rx ),
  TCP_STAT_OFFSET_SIZE(10,dropped_segs_rx		),
  TCP_STAT_OFFSET_SIZE(11,bytes_tx				),
  TCP_STAT_OFFSET_SIZE(12,bytes_rx				),
  TCP_STAT_OFFSET_SIZE(13,segs_tx				),
  TCP_STAT_OFFSET_SIZE(14,segs_rx				),
  TCP_STAT_OFFSET_SIZE(15,bytes_rexmit			),
  TCP_STAT_OFFSET_SIZE(16,segs_rexmit			),
  TCP_STAT_OFFSET_SIZE(17,dup_bytes_rx			),
  TCP_STAT_OFFSET_SIZE(18,dup_segs_rx			),
  TCP_STAT_OFFSET_SIZE(19,window_probe_tx		),
  TCP_STAT_OFFSET_SIZE(20,out_of_order_segs_rx	),
  TCP_STAT_OFFSET_SIZE(21,out_of_order_bytes_rx ),
  TCP_STAT_OFFSET_SIZE(22,rexmit_timeouts		),
  TCP_STAT_OFFSET_SIZE(23,piggybacked_acks_tx	),
  TCP_STAT_OFFSET_SIZE(24,piggybacked_acks_rx	),
  TCP_STAT_OFFSET_SIZE(25,dup_acks_rx			),
  TCP_STAT_OFFSET_SIZE(26,dup_acks_tx			),
  /*---------------------------------------------------------------------------
  Indices 27(PS_STAT_TCP_STATE) to 31(PS_STAT_TCP_REMOTE_PORT) are unused.
  Fill with zeros
  - Refer ps_stat_tcp_enum_type
---------------------------------------------------------------------------*/
  STAT_FILL_NULL(27),
  STAT_FILL_NULL(28),
  STAT_FILL_NULL(29),
  STAT_FILL_NULL(30),
  STAT_FILL_NULL(31),
  {0, sizeof(ps_stat_tcp_g_s_type)} /* Boundary for ps_stat_tcp_g_table */
};

/*---------------------------------------------------------------------------
  Access table for Instance statistics
---------------------------------------------------------------------------*/
static const ps_stat_access_table_s_type ps_stat_tcp_i_table[PS_STAT_TCP_ALL + 1] = 
{
  /*---------------------------------------------------------------------------
  Indices from 0(PS_STAT_TCP_CRNT_OPEN_CONNS) to 
  10(PS_STAT_TCP_DROPPED_SEGS_RX) and 32(PS_STAT_TCP_ALL)are unused.
  Fill with zeros
  - Refer ps_stat_tcp_enum_type
  ---------------------------------------------------------------------------*/
  STAT_FILL_NULL(0),
  STAT_FILL_NULL(1),
  STAT_FILL_NULL(2),
  STAT_FILL_NULL(3),
  STAT_FILL_NULL(4),
  STAT_FILL_NULL(5),
  STAT_FILL_NULL(6),
  STAT_FILL_NULL(7),
  STAT_FILL_NULL(8),
  STAT_FILL_NULL(9),
  STAT_FILL_NULL(10),
  TCP_INSTANCE_STAT_OFFSET_SIZE(11,tcp_i_stats.bytes_tx,              uint64),
  TCP_INSTANCE_STAT_OFFSET_SIZE(12,tcp_i_stats.bytes_rx,              uint64),
  TCP_INSTANCE_STAT_OFFSET_SIZE(13,tcp_i_stats.segs_tx,               uint32),
  TCP_INSTANCE_STAT_OFFSET_SIZE(14,tcp_i_stats.segs_rx, 			        uint32),
  TCP_INSTANCE_STAT_OFFSET_SIZE(15,tcp_i_stats.bytes_rexmit,          uint64),
  TCP_INSTANCE_STAT_OFFSET_SIZE(16,tcp_i_stats.segs_rexmit,           uint32),
  TCP_INSTANCE_STAT_OFFSET_SIZE(17,tcp_i_stats.dup_bytes_rx,		      uint64),
  TCP_INSTANCE_STAT_OFFSET_SIZE(18,tcp_i_stats.dup_segs_rx, 		      uint32),
  TCP_INSTANCE_STAT_OFFSET_SIZE(19,tcp_i_stats.window_probe_tx, 	    uint32),
  TCP_INSTANCE_STAT_OFFSET_SIZE(20,tcp_i_stats.out_of_order_segs_rx,  uint32),
  TCP_INSTANCE_STAT_OFFSET_SIZE(21,tcp_i_stats.out_of_order_bytes_rx, uint64),
  TCP_INSTANCE_STAT_OFFSET_SIZE(22,tcp_i_stats.rexmit_timeouts,       uint32),
  TCP_INSTANCE_STAT_OFFSET_SIZE(23,tcp_i_stats.piggybacked_acks_tx,   uint32),
  TCP_INSTANCE_STAT_OFFSET_SIZE(24,tcp_i_stats.piggybacked_acks_tx,   uint32),
  TCP_INSTANCE_STAT_OFFSET_SIZE(25,tcp_i_stats.dup_acks_rx, 		      uint32),
  TCP_INSTANCE_STAT_OFFSET_SIZE(26,tcp_i_stats.dup_acks_tx, 		      uint32),
  TCP_INSTANCE_STAT_OFFSET_SIZE(27,state,							                uint16),
  TCP_INSTANCE_STAT_OFFSET_SIZE(28,conn.local.address,    struct ps_in6_addr),
  TCP_INSTANCE_STAT_OFFSET_SIZE(29,conn.local.port, 				          uint16),
  TCP_INSTANCE_STAT_OFFSET_SIZE(30,conn.remote.address, 	struct ps_in6_addr),
  TCP_INSTANCE_STAT_OFFSET_SIZE(31,conn.remote.port,				          uint16),
  STAT_FILL_NULL(32)
};

/*===========================================================================

                             INTERNAL FORWARD DECLARATIONS

===========================================================================*/
static errno_enum_type ps_stat_get_tcp_instance_all
(
  ps_stat_tcp_enum_type  stat,
  struct tcb             *tcb_ptr,
  void                   *return_value,
  uint16                 ret_len
);

static errno_enum_type ps_stat_get_tcp_control_block
(
  int              handle,
  struct tcb    ** ctrl_blk_ptr
);


static void ps_stat_get_tcp_desc_log_pkt
(
  int32               handle
);

static void ps_stat_get_tcp_inst_stat_log_pkt
(
  int32                            handle
);

static void ps_stat_get_tcp_global_stat_log_pkt
(
  int32                            handle
);
/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_STAT_GET_TCP_INSTANCE_ALL()

DESCRIPTION
  This function supplies all the TCP statistics for the instance handle
  supplied. This is needed because of the need to extract and format data
  available in the tcp control block

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  tcb_ptr         - IN - pointer that points to the TCP control block
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_tcp_instance_all
(
  ps_stat_tcp_enum_type  stat,
  struct tcb             *tcb_ptr,
  void                   *return_value,
  uint16                 ret_len
)
{
   ps_stat_tcp_i_s_type *tcp_i_stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    validate 'stat'; only PS_STAT_TCP_ALL should be passed to this function
  -------------------------------------------------------------------------*/
  if (stat != PS_STAT_TCP_ALL || (NULL == tcb_ptr))
  {
    DATA_ERR_FATAL("ps_stat_get_tcp_instance_all :Null tcb_ptr or Invalid stat");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    return_value validation : should not be null
  -------------------------------------------------------------------------*/

  if (return_value == NULL)
  {
    LOG_MSG_ERROR_0("return_value should not be NULL ");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    check the sizeof the stat requested. If the size is zero, the stat is
    invalid in the current context. return appropriate error
  -------------------------------------------------------------------------*/

  if (ret_len < sizeof(ps_stat_tcp_i_s_type))
  {
    LOG_MSG_ERROR_0(" Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }

  tcp_i_stats = (ps_stat_tcp_i_s_type *) return_value ;

  /*-------------------------------------------------------------------------
    Copy the stats from tcp control block
  -------------------------------------------------------------------------*/
  tcp_i_stats->bytes_tx              = tcb_ptr->tcp_i_stats.bytes_tx ;
  tcp_i_stats->bytes_rx              = tcb_ptr->tcp_i_stats.bytes_rx;
  tcp_i_stats->segs_tx               = tcb_ptr->tcp_i_stats.segs_tx ;
  tcp_i_stats->segs_rx               = tcb_ptr->tcp_i_stats.segs_rx;
  tcp_i_stats->bytes_rexmit          = tcb_ptr->tcp_i_stats.bytes_rexmit ;
  tcp_i_stats->segs_rexmit           = tcb_ptr->tcp_i_stats.segs_rexmit ;
  tcp_i_stats->dup_bytes_rx          = tcb_ptr->tcp_i_stats.dup_bytes_rx ;
  tcp_i_stats->dup_segs_rx           = tcb_ptr->tcp_i_stats.dup_segs_rx ;
  tcp_i_stats->window_probe_tx       = tcb_ptr->tcp_i_stats.window_probe_tx;
  tcp_i_stats->out_of_order_bytes_rx =
                                  tcb_ptr->tcp_i_stats.out_of_order_bytes_rx;
  tcp_i_stats->out_of_order_segs_rx  =
                                  tcb_ptr->tcp_i_stats.out_of_order_segs_rx ;
  tcp_i_stats->rexmit_timeouts       = tcb_ptr->tcp_i_stats.rexmit_timeouts ;
  tcp_i_stats->piggybacked_acks_tx   =
                                  tcb_ptr->tcp_i_stats.piggybacked_acks_tx ;
  tcp_i_stats->piggybacked_acks_rx   =
                                  tcb_ptr->tcp_i_stats.piggybacked_acks_rx;
  tcp_i_stats->dup_acks_tx           = tcb_ptr->tcp_i_stats.dup_acks_tx ;
  tcp_i_stats->dup_acks_rx           = tcb_ptr->tcp_i_stats.dup_acks_rx;
  tcp_i_stats->state                 = (uint16)(tcb_ptr->state);
  tcp_i_stats->local_ip              = tcb_ptr->conn.local.address;
  tcp_i_stats->remote_ip             = tcb_ptr->conn.remote.address;
  tcp_i_stats->local_port            = tcb_ptr->conn.local.port;
  tcp_i_stats->remote_port           = tcb_ptr->conn.remote.port;

  /*-------------------------------------------------------------------------
    Set the queried bit
  -------------------------------------------------------------------------*/
  tcb_ptr->tcp_i_stats.queried = PS_STAT_QUERIED;

  return E_SUCCESS ;
} /* ps_stat_get_tcp_instance_all */


/*===========================================================================
FUNCTION PS_STAT_GET_TCP_CONTROL_BLOCK()

DESCRIPTION
  This function converts a QXDM handle into a TCP control block.  The
  handle-to-ctrl-blk conversion is done in ps_stat_tcp because the common
  stats module does not know the structure of the TCP ctrl blk.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle          - IN - sockfd of the instance ctrl blk of interest
  ctrl_blk_ptr    - OUT- control block ptr.  void** so that it can be
                         passed right into ps_stat_tcp_get/reset

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_tcp_control_block
(
  int              handle,
  struct tcb    ** ctrl_blk_ptr
)
{

  return E_NOT_SUPPORTED;
} /* ps_stat_get_tcp_control_block */

/*===========================================================================
FUNCTION PS_STAT_GET_TCP_DESC()

DESCRIPTION
  This function fills the description of one TCP instance into a log packet.

PARAMETERS
  handle          : Specifies which instance to retrieve the description from
  *pkt_ptr        : Pointer to the log packet that the description will be 
                    copied to

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

/*===========================================================================
FUNCTION PS_STAT_GET_TCP_DESC_LOG_PKT()

DESCRIPTION
  This function fills the description of the specified instance handle into
  a log packet.  If the handle matches the handle for all descriptions, it
  copies all of the active instance descriptions into the log packet.  If the
  handle does not specify all instances but specifies a valid active instance,
  the description associated with that instance is copied into the log packet.
  If the handle is invalid, nothing is done.

PARAMETERS
  handle          : Specifies which instance to retrieve the description from

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_get_tcp_desc_log_pkt
(
  int32               handle
)
{
  return;


} /* ps_stat_get_tcp_desc_log_pkt */


/*===========================================================================
FUNCTION PS_STAT_GET_TCP_INST_STAT_LOG_PKT()

DESCRIPTION
  This function fills the statistics of the specified instance handle into
  a log packet.  If the handle matches the handle for all instances, it
  sends one log packet for each instance that is active on the phone.  If the
  handle does not specify all instances but specifies a valid active instance,
  the statistics associated with that instance is copied into the log packet.
  If the handle is invalid, nothing is done.

PARAMETERS
  handle          : Specifies which instance to retrieve the stats from

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_get_tcp_inst_stat_log_pkt
(
  int32                            handle
)
{
  uint8                          * log_pkt_ptr          = NULL;
  struct tcb                     * tcb_ptr              = NULL;
  ps_stat_inst_tcp_log_pkt_type  * inst_tcp_log_pkt_ptr = NULL;
  errno_enum_type                  ret;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_STAT_INST_DESC_ALL == handle)
  {

#if 0
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

//    tcb_ptr = (struct tcb *) q_check( &(sock_config_cb.tcp_cb_q) );

    /*-----------------------------------------------------------------------
      Loop through all the sockets
    -----------------------------------------------------------------------*/
    while ( tcb_ptr != NULL )
    {
      ASSERT(SOCKAVAIL != tcb_ptr->sockfd);

      ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_tcp_log_pkt_type),
                                  LOG_PS_STAT_TCP_INST_C,
                                  &log_pkt_ptr);

      if (NULL == log_pkt_ptr)
      {
        LOG_MSG_ERROR_0("Log packet allocated for TCP failed");
        return;
      }

      inst_tcp_log_pkt_ptr = (ps_stat_inst_tcp_log_pkt_type *) log_pkt_ptr;

      inst_tcp_log_pkt_ptr->handle  = tcb_ptr->sockfd;

      (void) ps_stat_get_tcp(PS_STAT_TCP_ALL,
                             (void *) tcb_ptr,
                             &(inst_tcp_log_pkt_ptr->inst_tcp_stats),
                             sizeof(ps_stat_tcp_i_s_type));

      ps_stat_logging_commit_log_pkt
      (
        log_pkt_ptr,
        sizeof(ps_stat_inst_tcp_log_pkt_type)
      );
      
      /*tcb_ptr =
          (struct tcb *) q_next(&(sock_config_cb.tcp_cb_q), &(tcb_ptr->link));*/
    } /* while iterating through socket list*/

    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
#endif /* if 0*/
  } /* if fill all instances */
  else
  {
    ret = ps_stat_get_tcp_control_block(handle,
                                       &tcb_ptr);

    if (E_SUCCESS != ret)
    {
      LOG_MSG_ERROR_1("Bad handle %d", handle);
      return;
    }

    ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_tcp_log_pkt_type),
                                LOG_PS_STAT_TCP_INST_C,
                                &log_pkt_ptr);

    if (NULL == log_pkt_ptr)
    {
      LOG_MSG_ERROR_0("Log packet allocated for TCP failed");
      return;
    }

    inst_tcp_log_pkt_ptr = (ps_stat_inst_tcp_log_pkt_type *) log_pkt_ptr;

    inst_tcp_log_pkt_ptr->handle  = handle;

    (void) ps_stat_get_tcp(PS_STAT_TCP_ALL,
                           (void *) tcb_ptr,
                           &(inst_tcp_log_pkt_ptr->inst_tcp_stats),
                           sizeof(ps_stat_tcp_i_s_type));

    ps_stat_logging_commit_log_pkt
    (
      log_pkt_ptr,
      sizeof(ps_stat_inst_tcp_log_pkt_type)
    );
  } /* else only fill one instance */
} /* ps_stat_get_tcp_inst_stat_log_pkt */


/*===========================================================================
FUNCTION PS_STAT_GET_TCP_GLOBAL_LOG_PKT()

DESCRIPTION
  This function fills the global TCP statistics into a log packet.

PARAMETERS
  handle          : Specifies which instance to retrieve the stats from

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_get_tcp_global_stat_log_pkt
(
  int32                            handle
)
{
  uint8                               * log_pkt_ptr            = NULL;
  ps_stat_global_tcp_log_pkt_type     * global_tcp_log_pkt_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_stat_logging_get_log_pkt(sizeof(ps_stat_global_tcp_log_pkt_type),
                              LOG_PS_STAT_GLOBAL_TCP_C,
                              &log_pkt_ptr);

  if (NULL == log_pkt_ptr)
  {
    LOG_MSG_ERROR_0("Log packet allocated for TCP failed");
    return;
  }

  global_tcp_log_pkt_ptr = (ps_stat_global_tcp_log_pkt_type *) log_pkt_ptr;

  (void) ps_stat_get_tcp(PS_STAT_TCP_ALL,
                         NULL,
                         &(global_tcp_log_pkt_ptr->global_tcp_stats),
                         sizeof(ps_stat_tcp_g_s_type));

  ps_stat_logging_commit_log_pkt
  (
    log_pkt_ptr,
    sizeof(ps_stat_global_tcp_log_pkt_type)
  );

  return;  
} /* ps_stat_get_tcp_global_stat_log_pkt */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_STAT_RESET_TCP()

DESCRIPTION
  This function resets the instance statistics associated with the specified
  instance.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  tcb_ptr : Pointer to the instance

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_tcp
(
  int32                  handle
)
{
  struct tcb           * tcp_cb_ptr;
  errno_enum_type        ret;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ret = ps_stat_get_tcp_control_block(handle, &tcp_cb_ptr);

  if (E_SUCCESS != ret)
  {
    LOG_MSG_ERROR_1("Bad handle: %d", handle);
    return;
  }

  if (PS_STAT_QUERIED == tcp_cb_ptr->tcp_i_stats.queried)
  {
    ps_stat_get_tcp_inst_stat_log_pkt(handle);
  }

  memset(&(tcp_cb_ptr->tcp_i_stats), 0, sizeof(tcp_cb_ptr->tcp_i_stats));

  return;
} /* ps_stat_reset_tcp */


/*===========================================================================
FUNCTION PS_STAT_INIT_TCP()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_tcp
(
  void
)
{
  ps_stat_logging_inst_reg_fill_desc_cback(PS_STAT_MODULE_INST_TCP,
                                           ps_stat_get_tcp_desc_log_pkt);

  ps_stat_logging_inst_reg_get_stat_cback(PS_STAT_MODULE_INST_TCP,
                                          ps_stat_get_tcp_inst_stat_log_pkt);

  ps_stat_logging_inst_reg_get_stat_cback(PS_STAT_MODULE_GLOBAL_TCP,
                                          ps_stat_get_tcp_global_stat_log_pkt);
  
  ps_stat_logging_inst_reg_reset_stat_cback(PS_STAT_MODULE_INST_TCP,
                                            ps_stat_reset_tcp);

} /* ps_stat_init_tcp */


/*===========================================================================
FUNCTION PS_STAT_GET_TCP()

DESCRIPTION
  This function supplies the required tcp layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to the tcp control block.If
  the instance pointer is NULL, it signifies that the required statistic is
  global in scope. The third argument points to the memory location where
  the results will be copied.The result ( statistic) will be copied to this
  memory address. The fourth argument passes the length of memory allocated
  for the return value and should be greater than or equal to the memory size
  of the statistic requested.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  instance_ptr    - IN - pointer that has the value of socket descriptor
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_tcp
(
  ps_stat_tcp_enum_type  stat,
  void                   *instance_ptr,
  void                   *return_value,
  uint16                 ret_len
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check Instance pointer; If it is NULL, call ps_satat_get_common with
    global access table
  -------------------------------------------------------------------------*/

  if ( instance_ptr == NULL)
  {
    return ps_stat_get_common((uint32)stat,
                              ps_stat_tcp_g_table,
                              (uint16)PS_STAT_TCP_ALL,
                              &tcp_stats,
                              return_value,
                              ret_len
                             );
  }
  else
  {
    if (stat == PS_STAT_TCP_ALL)
    {
      return ps_stat_get_tcp_instance_all(stat,
                                          (struct tcb *)instance_ptr,
                                          return_value,
                                          ret_len
                                         );
    }
    else
    {
      return ps_stat_get_common((uint32)stat,
                                ps_stat_tcp_i_table,
                                (uint16)PS_STAT_TCP_ALL,
                                instance_ptr,
                                return_value,
                                ret_len
                               );
    }
  }
} /* ps_stat_get_tcp */

