/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  I N T E R N A L   F I L E

DESCRIPTION

 The Data Services sockets internal file. This contains all the functions
 used by the network library internally.

EXTERNALIZED FUNCTIONS

Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssocki.c_v   1.25   28 Feb 2003 18:29:08   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/ps_socket_cmd.cpp#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "ds_Utils_DebugMsg.h"
#include "ds_Sock_Def.h"

#include "ps_socket_cmd.h"
#include "ps_mem.h"
#include "ps_svc.h"
#include "dss_ps.h"
#include "ps_socki_defs.h"
#include "queue.h"


/*===========================================================================

                        LOCAL DATA DEFINITIONS

===========================================================================*/
static ps_socket_cmd_out_of_mem_cback_f_ptr_type  
  ps_socket_cmdi_out_of_mem_cback_f_ptr;


extern "C"
{
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
ps_socket_cmd_type * ps_socket_cmd_alloc
(
  ps_socket_cmd_kind_type  kind
)
{
  ps_socket_cmd_type *  ps_cmd_buf_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure that there are enough buffers in the ps_mem pool. Else, give
    preference to control path commands over data path.
  -------------------------------------------------------------------------*/
  if (PS_SOCKET_CMD_HIGH_PRIORITY <= kind &&
      ps_mem_less_than_few_free_items( PS_MEM_PS_SOCKET_CMD_TYPE))
  {
    LOG_MSG_INFO1_0("ps_socket_cmd_alloc(): "
                    "Low on PS_SOCKET_CMD buf");
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Allocate ps cmd buffer and update the kind field
  -------------------------------------------------------------------------*/
  ps_cmd_buf_ptr =
    (ps_socket_cmd_type *) ps_mem_get_buf( PS_MEM_PS_SOCKET_CMD_TYPE);

  if (0 == ps_cmd_buf_ptr)
  {
    goto bail;
  }

  ps_cmd_buf_ptr->kind = kind;
  return ps_cmd_buf_ptr;

bail:
  /*-------------------------------------------------------------------------
    Notify the client that we are out of socket cmd buffers
  -------------------------------------------------------------------------*/
  if (0 != ps_socket_cmdi_out_of_mem_cback_f_ptr)
  {
    ps_socket_cmdi_out_of_mem_cback_f_ptr();
  }

  return 0;
} /* ps_socket_cmd_alloc() */


void ps_socket_cmd_send
(
  ps_socket_cmd_type **   ps_cmd_buf_ptr_ptr,
  int32                   platform_sock_handle,
  int32                   pcb_handle,
  int32                   protocol
)
{
  ps_socket_cmd_type *   ps_cmd_buf_ptr = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ps_cmd_buf_ptr_ptr || 0 == *ps_cmd_buf_ptr_ptr)
  {
    DATA_ERR_FATAL("ps_socket_cmd_send(): NULL buf");
    return;
  }

  ps_cmd_buf_ptr = *ps_cmd_buf_ptr_ptr;

  if (ps_cmd_buf_ptr->kind <= PS_SOCKET_CMD_MIN_VAL ||
      ps_cmd_buf_ptr->kind >= PS_SOCKET_CMD_MAX_VAL)
  {
    LOG_MSG_ERROR_1("ps_socket_cmd_send(): "
                    "Invalid kind %d", ps_cmd_buf_ptr->kind);
    ASSERT( 0);
    ps_socket_cmd_free( ps_cmd_buf_ptr_ptr);
    return;
  }

  ps_cmd_buf_ptr->sockfd     = platform_sock_handle;
  ps_cmd_buf_ptr->pcb_handle = pcb_handle;

  if (ds::Sock::Protocol::QDS_TCP == protocol)
  {
    ps_cmd_buf_ptr->protocol = PS_IPPROTO_TCP;
  }
  else if (ds::Sock::Protocol::QDS_UDP == protocol)
  {
    ps_cmd_buf_ptr->protocol = PS_IPPROTO_UDP;
  }
  else
  {
    ps_cmd_buf_ptr->protocol = PS_IPPROTO_ICMP;
  }

  /*-------------------------------------------------------------------------
    DO NOT log this msg after the cmd is posted. If PS has higher priority
    than App, PS will get immedetiately proces the cmd and free it. Logging
    contents of a freed buffer is NOT a good idea
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO1_2("ps_socket_cmd_send(): "
                  "Posting cmd %d sock %d",
                  ps_cmd_buf_ptr->kind, platform_sock_handle);

  q_put( &ps_socket_send_q, &( ps_cmd_buf_ptr->link));
  PS_SET_SIGNAL( PS_SOCKET_RX_Q_SIGNAL);

  *ps_cmd_buf_ptr_ptr = 0;
  return;

} /* ps_socket_cmd_send() */


void ps_socket_cmd_free
(
  ps_socket_cmd_type **   ps_cmd_buf_ptr_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ps_cmd_buf_ptr_ptr || 0 == *ps_cmd_buf_ptr_ptr)
  {
    DATA_ERR_FATAL("ps_socket_cmd_free(): NULL buf");
    return;
  }

  PS_MEM_FREE(*ps_cmd_buf_ptr_ptr);
  return;

} /* ps_socket_cmd_free() */


void ps_socket_cmd_reg_out_of_mem_cback
(
  ps_socket_cmd_out_of_mem_cback_f_ptr_type  out_of_mem_cback_f_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == out_of_mem_cback_f_ptr)
  {
    DATA_ERR_FATAL("ps_socket_cmd_reg_out_of_mem_cback(): NULL f_ptr");
    return;
  }

  ps_socket_cmdi_out_of_mem_cback_f_ptr = out_of_mem_cback_f_ptr;
  LOG_MSG_INFO1_1("ps_socket_cmd_reg_out_of_mem_cback(): "
                  "Reg cback 0x%x", out_of_mem_cback_f_ptr);

} /* ps_socket_cmd_reg_out_of_mem_cb() */

} /* extern "C" */

