/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            T C P T I M E R

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

tcp_timeout_cb()
  Gets called when TCP rexmit timer goes off.

tcp_delayed_cb()
  Gets called when TCP delayed ack timer goes off.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1995-2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcptimer.c_v   1.0   08 Aug 2002 11:19:54   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_tcptimer.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/14   vl      Handling of dss_connect when flow is disabled
09/21/12   ss      Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR and 
                   evaluating ASSERT requirement.
06/15/2010 vm      Adding connect timeout feature
07/31/09   am      Notifying stored ICMP soft errors in case of max timeout.
06/10/08   am      Changed ssthresh calc in case of retransmissions.
03/20/07   msr     Resetting TCP connection if 120s have been elapsed in
                   backoffs instead of waiting for MAX_BACKOFFS
02/05/07   rt      Removed EIFEL code.
10/15/05   rt      Added code for new tcp option DSS_TCP_EIFEL.
0815/05    kr      Added support for stats update and removed FEATURE
                   DS_PSTATS
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
12/28/04   vp      Set cwind to loss window (1 mss) on retranmission timeout.
11/03/04   vp      Change the numb_bytes_in_transit to 0 and high_seq_lost
                   to snd.nxt upon window collapse in retransmission.
10/04/04   sv      Modified SACK processing to store list of SACK blocks
                   instead of unack list.
08/16/04   sv      Added support for processing incoming SACK option.
06/11/04   vp      Removed the inclusion of psglobal.h, netuser.h & internet.h
05/19/04   vp      Fixed the assignment to tcb->cwind in tcp_timeout_cb()
                   for Async/Fax
02/26/04   vp      Changed the name of function close_self to tcp_close_self.
01/28/04   sv      Changed timeout_cb to update the cwind to DEF_CWND when
                   the congestion window is reset.
01/27/04   vp      Added call to t_upcall when cwind changes in timeout_cb
01/14/04   vp      replaced call to reset_tcp with call to close_self with
                   TIMEOUT after max allowable backoffs.
07/31/01   usb     Removed include psi.h
11/12/01   dwp     Add "|| FEATURE_DATA_PS" to whole module.
11/11/00   snn     Renamed the callback functions with _cb postfix.
                   Removed errno from the timer functions because they are no
                    longer used.
10/10/00   na/snn  Added support for multiple TCP sockets.
08/17/99   smp     Added include files msg.h & err.h.
06/17/98   ldg     Update backoff history at time of backoff.
01/27/98   ldg     Removed the DS_SLIM switch.
09/24/97   na      Changed TCP backoff from exponential to linear. Put a limit
                   of 6 attempts at backoff.
06/25/97   jgr     Added ifdef FEATURE_DS over whole file
05/24/96   jjw     Gemini & ISS2 Data Services release.
11/13/95   jjw     Changed min/max macros to upper case
11/09/95   jjw     IS-99 Compliant Release

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* TCP timeout routines
 * Copyright 1991 Phil Karn, KA9Q
 * Additional material Copyright 1992-1995 William Allen Simpson
 */
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "dsm.h"

#include "ps_tcp.h"
#include "ps_pcb_close_reason_codes.h"

#include "msg.h"
#include "err.h"
#include "pstimer.h"
#include "amssassert.h"
#include "ps_mem.h"
#include "ps_stat_tcp.h"
#include "ds_Utils_DebugMsg.h"


/*===========================================================================

                       EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION TCP_CLEAR_SACK_DATA()

DESCRIPTION
  This function will expunge all SACK information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Delete all SACK information.
===========================================================================*/

void tcp_clear_sack_data
(
  register struct tcb *tcb                 /* pointer to TCP control block */
)
{
  struct tcp_sequence_block_list * temp_block_ptr;
  struct tcp_sequence_block_list * curr_block_ptr;


  curr_block_ptr = tcb->sack_blocks_head_ptr;
  while(curr_block_ptr != NULL)
  {
    temp_block_ptr = curr_block_ptr;
    curr_block_ptr = curr_block_ptr->next_ptr;
    PS_MEM_FREE(temp_block_ptr);
  }

  tcb->sack_blocks_head_ptr = tcb->sack_blocks_tail_ptr = NULL;
}

/*===========================================================================

FUNCTION TCP_TIMEOUT_CB

DESCRIPTION
  This function will perform Re-transmission processing for TCP. This function
  is invoked upon expiration of the TCP Re-xmit timer. This timer expires
  when a TCP segment that was TX'd has not been ACKnowledged.

  This function is called in the context of the PS task.

  The passed parameter in_tcb_ptr is the pointer to the TCP control block
  for the TCP connection whose close timer went off. This should not be NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tcp_timeout_cb
(
  void  *in_tcb_ptr                        /* pointer to TCP control block */
)
{
  struct tcb *tcb_ptr = NULL;                                /* Ptr to tcb */
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check to make sure the parameters to this function are valid.
    If ASSERTs are turned on, this condition should be caught.
    If ASSERTs are turned off, we exit from this function so that invalid
    pointers will not be referenced or modified.
  -------------------------------------------------------------------------*/
  if( in_tcb_ptr == NULL)
  {
    ASSERT( 0);
    return;
  }

  tcb_ptr = (struct tcb *) in_tcb_ptr;

  /*-------------------------------------------------------------------------
    Cancel timer.
  -------------------------------------------------------------------------*/
  timer_ret_val = ps_timer_cancel( tcb_ptr->timer);
  if( PS_TIMER_SUCCESS != timer_ret_val )
  {
    LOG_MSG_ERROR_1("Error canceling timer 0x%x", tcb_ptr->timer);
    ASSERT(0);
  }

  /*-------------------------------------------------------------------------
    Check if timer overload flag is set.
    If set, cancel overload flag and invoke tcp_oputput() 
  -------------------------------------------------------------------------*/
   if(tcb_ptr->timer_overload_flag)
   {
     LOG_MSG_INFO1_0("Overloaded re-tx timer expired");
     tcb_ptr->timer_overload_flag = FALSE;
     tcp_output(tcb_ptr);
     return;
   }

  /*-------------------------------------------------------------------------
    If the TCP connection is in TIME_WAIT state, then close the TCP
    connection - 2MSL timer has expired.
    Otherwise setup for retransmission - retransmission timer has expired.
  -------------------------------------------------------------------------*/
  switch(tcb_ptr->state)
  {
    case TCP_TIME_WAIT:
      tcp_close_self(tcb_ptr, PS_PCB_CLOSE_REASON_CODE_NORMAL);
      break;

    default:
      LOG_MSG_INFO2_0( "TCP Re-xmit");
      TCP_INC_STATS(rexmit_timeouts, 1);
      TCP_INC_INSTANCE_STATS(tcb_ptr,rexmit_timeouts, 1);
      tcb_ptr->timeouts++;
      tcb_ptr->backoff++;
      tcb_ptr->total_backoff_time += tcb_ptr->tcp_rexmit_timer_val;

      /*---------------------------------------------------------------------
        Check if we have exceeded the max allowable backoff tries or if we
        have spent max allowable time in backoffs
      ---------------------------------------------------------------------*/
      if ( tcb_ptr->total_backoff_time >= tcb_ptr->max_backoff_time ||
           tcb_ptr->backoff >= TCP_MAX_BACKOFFS)
      {
        LOG_MSG_ERROR_0("TCP closed because of max backoffs");
        TCP_INC_STATS(conn_timeout_aborts, 1);

       /*--------------------------------------------------------------------
         Notify any socket soft errors
       --------------------------------------------------------------------*/
        tcp_notify((uint16)tcb_ptr->soft_error, &(tcb_ptr->conn));

       /*--------------------------------------------------------------------
         Reset the connection with close reason to be TIMEOUT
       --------------------------------------------------------------------*/
        reset_tcp(tcb_ptr, PS_PCB_CLOSE_REASON_CODE_TIMEOUT);
      }
      else
      {
        tcb_ptr->flags.retran = 1;           /* Indicate > 1  transmission */
        tcb_ptr->flags.force = 1;

        /*------------------------------------------------------------------
         Set the dupack counter to 0 as here RTO would take care of the
         retransmit.
        ------------------------------------------------------------------*/
        tcb_ptr->dupacks = 0;

        /* Reduce slowstart threshold to half current window */

        tcb_ptr->ssthresh = tcb_ptr->numb_bytes_in_transit / 2;
        tcb_ptr->ssthresh = MAX(tcb_ptr->ssthresh,tcb_ptr->mss);

        /*-------------------------------------------------------------------
          Shrink congestion window to loss window (one mss). This depends
          on current mss as Async/Fax calls have separate default mss.
        -------------------------------------------------------------------*/

        tcb_ptr->cwind = tcb_ptr->mss;

        /*-------------------------------------------------------------------
          Clear the SACK data
        -------------------------------------------------------------------*/
        tcp_clear_sack_data(tcb_ptr);
        tcb_ptr->high_seq_rexmit = tcb_ptr->snd.una;
        tcb_ptr->high_seq_lost = tcb_ptr->snd.nxt;
        tcb_ptr->numb_bytes_in_transit = 0;
        tcp_output(tcb_ptr);
      }
  }  /* switch on tcp state */

}  /* tcp_timeout_cb() */

/*===========================================================================

FUNCTION TCP_DELAYED_CB

DESCRIPTION
  This function performs the Delayed-ACK processing for TCP. This function
  is invoked to perform an ACK to the other side when there was no data to
  TX that could have an ACK piggybacked onto.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tcp_delayed_cb
(
  void  *in_tcb_ptr                        /* pointer to TCP control block */
)
{
  struct tcb *tcb_ptr = NULL;                                /* Ptr to tcb */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check to make sure the parameters to this function are valid.
    If ASSERTs are turned on, this condition should be caught.
    If ASSERTs are turned off, we exit from this function so that invalid
    pointers will not be referenced or modified.
  -------------------------------------------------------------------------*/
  if( in_tcb_ptr == NULL)
  {
    ASSERT( 0);
    return;
  }

  tcb_ptr = (struct tcb *) in_tcb_ptr;

  /*-------------------------------------------------------------------------
    Since the timer has gone off, send a delayed ACK.
  -------------------------------------------------------------------------*/
  tcb_ptr->flags.force = TRUE;
  tcp_output(tcb_ptr);

} /* tcp_delayed_cb() */
