/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             T C P S U B R

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1995-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcpsubr.c_v   1.0   08 Aug 2002 11:19:52   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_tcpsubr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/09/15   ds      Start persist timer only if required.
07/27/10   ps      Added code changes to fix the broken Async/Fax support.
05/22/09   kk      Q6 compiler warning fixes.
08/12/08   pp      create_tcb: Update srtt w/ default RTT from NV instead of
                   hard-coded DEF_RTT.
03/20/07   msr     Resetting TCP connection if 120s have been elapsed in
                   backoffs instead of waiting for MAX_BACKOFFS
12/05/06   msr     Passing sockfd in ps_event_info_type
10/16/06   msr     Fixed bad ASSERT in tcp_start_persist_timer() (CR 103765)
10/13/06   hm      Not storing min_rto and max_rto in tcb anymore
10/27/05   vp      Set Async Tcb route scope to all iface mask in create_tcb
08/08/05   kr      Removed deature DS_PSTATS and added support for stats
                   update
06/13/05   vp      Functions tcp_quench() tcp_notify().
03/21/05   ssh     Changes pursuant to the new typedef ps_timer_handle_type
02/16/05   msr     Not caching RTT, anymore.
12/20/04   msr     Fixed tcp_start_persist_timer() to use
                   numb_bytes_in_transit instead of snd_ptr.
11/30/04   msr     Fixed integer overflow in tcp_start_persist_timer().
10/25/04   msr     Added tcp_start_persist_timer()
06/11/04    vp     Removed the inclusion of psglobal.h,netuser.h and internet.h.
                   Included ps_stats.h and ps_socket_defs.h. Removed the use
                   of MAXINT32.
02/26/04    vp     Changed the names of functions send_syn, close_self,
                   rtt_add, rtt_get, seq_within, setstate, send_reset and
                   prepended tcp_ in front of their names.
02/26/04    vp     Changed the names of functions seq_lt, seq_gt and seq_ge
                   and prepended tcp_ before their names.
11/11/03    sv     Modified call to t_upcall to pass number of bytes acked
                   instead of receive window.
08/18/03    sv     Modified the code to use IPv6 addresses internally.
07/31/02   usb     Removed include psi.h
03/05/02    pjb    Finished Accept Listen Code Review Items
02/22/02    pjb    Added accept and listen
02/12/02   dwp     Cleanup header inclusion(s) for non DS builds.
12/06/01   ss      Corrected the wrong order of memset() parameters
11/12/01   dwp     Add "|| FEATURE_DATA_PS" to whole module. Wrap out old DS
                   dependencies if FEATURE_DATA is defined.
11/11/00   snn     Removed errno from the timer functions because they are
                   no longer used.
                   Renamed tcp_state to tcp_state_enum_type
10/10/00   na      Added support for multiple TCP sockets.
10/27/98   ldg     ARM porting: ROM to ROM const.
04/14/98   ldg     Added counters for payload sent/received, and backoffs.
                   Removed rcvcnt and sndcnt from stats dump.
01/26/98   ldg     Removed the DS_SLIM switch.
12/15/97   ldg     Moved TCP out-of-order segment count from UI stats to
                   general stats, and added to legend for AT stats dump.
                   Also, out-of-order segment count was counting TCP active
                   opens rather than out-of-order segments.  Fixed this.
11/06/97   ldg     Removed externs.
09/26/97   ldg     Removed writes to Tcb from tcp_reset_stats.
08/06/97   na      Fixed bug caused by missing parens. around LOG_MSG_FATAL_ERROR.
07/31/97   ldg     Added accessor function for UI to see some TCP vars.
06/25/97   jgr     Added ifdef FEATURE_DS over whole file
06/12/97   ldg     Added new FEATURE_DS_PSTATS
01/23/97   na      Included support for UI function that returns last status
05/24/96   jjw     Gemini & ISS2 Data Services release.
11/09/95   jjw     IS-99 Compliant Release

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* Low level TCP routines:
 *  control block management
 *  sequence number logical operations
 *  state transitions
 *  RTT cacheing
 *  garbage collection
 *
 * Copyright 1991 Phil Karn, KA9Q
 * Additional material Copyright 1992-1995 William Allen Simpson
 */
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include <memory.h>
#include "dsm.h"
#include "ps_tcp.h"
#include "ps_pcb_close_reason_codes.h"
#include "msg.h"
#include "err.h"
#include "pstimer.h"
#include "amssassert.h"
#include "ps_socket_defs.h"
#include "ps_stat_tcp.h"
#include "ps_stat_logging.h"
#include "ps_socket_event_defs.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*===========================================================================

FUNCTION TCP_CLOSE_SELF

DESCRIPTION
  This function will close the passed tcb.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tcp_close_self
(
  register struct tcb *tcb,
  int                 reason
)
{
  struct dsm_item_s *rp;
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(tcb == NULL)
  {
    return;
  }

  TCP_INC_STATS(conn_aborts, 1) ;
  timer_ret_val = ps_timer_cancel( tcb->timer);
  if( PS_TIMER_SUCCESS != timer_ret_val )
  {
    LOG_MSG_ERROR_1("tcp_close_self: Error canceling timer 0x%x", tcb->timer);
    ASSERT(0);
  }

  timer_ret_val = ps_timer_cancel( tcb->acker);
  if( PS_TIMER_SUCCESS != timer_ret_val )
  {
    LOG_MSG_ERROR_1("tcp_close_self: Error canceling timer 0x%x", tcb->acker);
    ASSERT(0);
  }

  ASSERT( reason < 256 && reason >= 0 );
  tcb->reason = (uint8)reason;

  /* Dump reassembly queue; nothing more can arrive */

  while( (rp = (struct dsm_item_s *)q_get(&tcb->reseq)) != NULL)
  {
    dsm_free_packet(&rp);
  }
  tcp_setstate(tcb,TCP_CLOSED);
}

/*===========================================================================

FUNCTION TCP_SEQ_WITHIN

DESCRIPTION
  This function will perform Sequence number comparisons.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if x is between low and high inclusive, otherwise FALSE.

SIDE EFFECTS
  None
===========================================================================*/

int tcp_seq_within
(
  register uint32 x,
  register uint32 low,
  register uint32 high
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(low <= high)
  {
   if(low <= x && x <= high)
   {
      return 1;
    }
  }
  else
  {
    if(low >= x && x >= high)
    {
      return 1;
    }
  }
  return 0;
}


/*===========================================================================

FUNCTION TCP_SETSTATE

DESCRIPTION
  This function will set the TCP state to the passed state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tcp_setstate
(
  register struct      tcb *tcb,
  tcp_state_enum_type  newstate
)
{
  tcp_state_enum_type oldstate = tcb->state;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*-------------------------------------------------------------------------
  Setting state for the UI last call status function
 ------------------------------------------------------------------------*/

  tcb->state = newstate;

  if(tcb->s_upcall)
  {
    (*tcb->s_upcall)(tcb,oldstate,newstate);
  }

  switch( newstate)
  {
    case TCP_SYN_RECEIVED:
    case TCP_ESTABLISHED:
      /*---------------------------------------------------------------------
        Inform diagnostic tool that the instance exists
      ---------------------------------------------------------------------*/
      ps_stat_logging_inst_update_desc(PS_STAT_MODULE_INST_TCP,
                                       tcb->sockfd,
                                       PS_STAT_INST_CREATED);
      break;

    default:
      break;
  }
}

/*===========================================================================
FUNCTION    TCP_START_PERSIST_TIMER

DESCRIPTION
  Start TCP persist timer. An exponential backoff algorithm is used to
  calculate the timeout value. Base value is TCP_PERSIST_BASE_TIMEOUT and
  the timeout value is bounded between TCP_PERSIST_MIN_TIMEOUT and
  TCP_PERSIST_MAX_TIMEOUT. Number of window probes sent so far is kept track
  in persist_backoff field of TCB and is incremented each time a probe is
  sent.

PARAMETERS
  tcb_ptr : TCB for which persist timer need to be started

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  persist_backoff field of TCB is incremented
===========================================================================*/
void tcp_start_persist_timer
(
  struct tcb  * tcb_ptr
)
{
  int64                persist_timeout_val;
  ps_timer_error_type  timer_ret_val;  /* Return value from timer fn. calls */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!(tcb_ptr != NULL && FALSE == ps_timer_is_running(tcb_ptr->timer) &&
         FALSE == ps_timer_is_running(tcb_ptr->persist_timer)))
  {
    LOG_MSG_ERROR_0("Re-Tx or Persist Timer running !!");
    return;
  }

  /*-------------------------------------------------------------------------
    When persist_backoff is 0, window probe is yet to be sent for the first
    time. So numb_bytes_in_transit is 0. When persist_backoff > 0, window
    probe is typically sent. So there will be one unacknowledged byte in
    transit. Some times window probe is not sent if iface subsystem is flow
    controlled in which case numb_bytes_in_transit is 0
  -------------------------------------------------------------------------*/
  if(!(0 == tcb_ptr->numb_bytes_in_transit ||
         1 == tcb_ptr->numb_bytes_in_transit))
  {
    LOG_MSG_ERROR_1("Number of bytes in transit: %d", tcb_ptr->numb_bytes_in_transit);
    return;
  }

  /*-------------------------------------------------------------------------
    Calculate the timeout value for persist timer and bound it
    between TCP_PERSIST_MIN_TIMEOUT and TCP_PERSIST_MAX_TIMEOUT.
  -------------------------------------------------------------------------*/
  persist_timeout_val =
    TCP_PERSIST_BASE_TIMEOUT * ((int64)0x1 << tcb_ptr->persist_backoff);

  if (TCP_PERSIST_MAX_TIMEOUT < persist_timeout_val)
  {
    /*-----------------------------------------------------------------------
      Not incrementing backoff deliberately as it doesn't serve any purpose
      towards calculating timeout value once TCP_PERSIST_MAX_TIMEOUT is
      reached
    -----------------------------------------------------------------------*/
    persist_timeout_val = TCP_PERSIST_MAX_TIMEOUT;
  }
  else
  {
    if (TCP_PERSIST_MIN_TIMEOUT > persist_timeout_val)
    {
      persist_timeout_val = TCP_PERSIST_MIN_TIMEOUT;
    }

    tcb_ptr->persist_backoff++;
  }

  LOG_MSG_INFO2_1("Starting persist timer with a timeout value of %ds",
                    persist_timeout_val / 1000);

  /*-------------------------------------------------------------------------
    Start the persist timer
  -------------------------------------------------------------------------*/
  timer_ret_val =
    ps_timer_start(tcb_ptr->persist_timer, persist_timeout_val);
  if( PS_TIMER_SUCCESS != timer_ret_val )
  {
    LOG_MSG_ERROR_1("Error canceling timer 0x%x", tcb_ptr->persist_timer);
    ASSERT(0);
  }

} /* tcp_start_persist_timer() */

/*===========================================================================

FUNCTION TCP_QUENCH()

DESCRIPTION
  This function takes care of ICMP source quench messages for TCP.It flow
  controls the TCP connection identified by the passed conn structure.
  Sets the congestion window to half.

DEPENDENCIES
  None.

PARAMETERS
  struct connection * - Pointer to the connection structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tcp_quench
(
  struct connection *conn
)
{
  struct tcb *tcb = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tcb = dsstcp_lookup_serv( conn );

  if(tcb != NULL && tcb->cwind >= (uint32)(2 * tcb->mss))
  {
    /*-----------------------------------------------------------------------
      Reduce the current congestion window to half.
    -----------------------------------------------------------------------*/
    tcb->cwind >>= 1;
  }
}/* tcp_quench() */

/*===========================================================================

FUNCTION TCP_NOTIFY()

DESCRIPTION
  This function notifies the passed ICMP error to the TCP application
  corresponding to the passed conn structure.

DEPENDENCIES
  None.

PARAMETERS
  uint16              - ICMP error to report.
  struct connection * - Pointer to the connection structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tcp_notify
(
  uint16            ps_errno,
  struct connection *conn
)
{
  struct tcb *tcb = NULL;
  ps_socket_event_info_type info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tcb = dsstcp_lookup_serv( conn );

  if(tcb != NULL)
  {

    if(tcb->state == TCP_ESTABLISHED)
    {
      /*---------------------------------------------------------------------
        Ignore the error.
      ---------------------------------------------------------------------*/
      return;
    }
    else if((ps_errno == DS_ENOPROTOOPT ||
             ps_errno == DS_ECONNREFUSED ||
             ps_errno == DS_EMSGSIZE) &&
            tcb->state < TCP_ESTABLISHED)
    {
      /*---------------------------------------------------------------------
        Notify the error immediately to the app as these are hard errors.
        and abort the connection (RFC 1122).
        Note that EMSGSIZE should be removed from this list once path MTU
        discovery is implemented.
      ---------------------------------------------------------------------*/
      info.scb_ptr         = tcb->scb_ptr;
      info.data.icmp_error = ps_errno;

      socket_platform_event_cback_f_ptr(PS_SOCKET_ICMP_ERROR_EV, &info);
      tcp_close_self(tcb, PS_PCB_CLOSE_REASON_CODE_RESET);
    }
    else if( tcb->state < TCP_ESTABLISHED &&
             (tcb->backoff >= TCP_MAX_BACKOFFS ||
                tcb->total_backoff_time >= tcb->max_backoff_time) &&
             tcb->soft_error)
    {
      /*---------------------------------------------------------------------
        Notify the error to the application.
      ---------------------------------------------------------------------*/
      info.scb_ptr         = tcb->scb_ptr;
      info.data.icmp_error = ps_errno;

      socket_platform_event_cback_f_ptr(PS_SOCKET_ICMP_ERROR_EV, &info);
    }
    else
    {
      tcb->soft_error = (uint32)ps_errno;
    }
  }
}/* tcp_notify() */

