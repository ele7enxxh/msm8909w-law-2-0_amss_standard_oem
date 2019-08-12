/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               T C P O U T . C

GENERAL DESCRIPTION
  TCP output segment processing

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright 1991 Phil Karn, KA9Q
  Additional material Copyright 1992-1995 William Allen Simpson

Copyright (c) 1995-2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcpout.c_v   1.6   25 Feb 2003 18:00:16   ssinghai  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_tcpout.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/25/14    vl     Handling of dss_connect when flow is disabled
03/02/13    pkp    Remove dog reporting from tcp processing.
09/21/12    ss     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR and 
                   evaluating ASSERT requirement.
02/24/11    cp     Fixed compiler warnings.
02/15/11    ss     Adding check to handle scenario where TCP SYN is received
                   with window size 0.
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free
                   Floating environment.
09/27/10    ss     DYNAMIC DOG support for DATA tasks.
01/11/10    ss     Lint warning fixes.
05/22/09    kk     Q6 compiler warning fixes.
10/22/08    dm     Modified TCP and IP4 headers to fix compiler warnings
09/12/08    pp     Metainfo optimizations.
08/21/08    am     RTO = RTO*2^n, not only SRTT*2^n.
06/10/08    am     RTO backoff calc even if timer is running.
03/20/07   msr     Resetting TCP connection if 120s have been elapsed in
                   backoffs instead of waiting for MAX_BACKOFFS
02/07/07    scb    Fixed Klocworks High errors
02/05/07    rt     Removed EIFEL code.
10/19/06    hm     Fixed a SACK retransmission bug for not retransmitting
                   data already SACKed.
10/13/06    hm     Cleaned up the code, simplified RTT computation algorithm
10/12/06    hm     Fixed code for negotiating timestamp when tsval sent in
                   the SYN is 0.
04/21/06    msr    Not calculating RTT again if retransmission timer is running
12/19/05    msr    Not transmitting data if any of iface, ps_flow, or
                   phys link is flow controlled
12/03/05    rt     Added code for new tcp option DSS_TCP_EIFEL.
08/15/05    kr     Added support for stats update ; Removed existing stats
                   update code
08/31/05    sv     Update high_seq_lost correctly when we rexmit the packet.
05/12/05    rt     Fixed lint errors.
02/28/05    sv     Removed references to old IPSEC code.
01/08/05    vp     Fixed an issue with tcp_next_timestamp().
12/27/04    ifk    Replaced setting of tracer field with DSM_TOUCH_ITEM
12/10/04    vp     Fixed return when tcp_hdr_create() returns false.
12/07/04    vp     tcp_next_timestamp() for monotonically increasing
                   timestamps.
11/30/04    msr    Sending window probes only when force flag is set.
10/25/04    sv     Fixed bug in update_rexmit to update high_rexmit when
                   high_rexmit exactly matches the start seq of sack.
10/25/04    msr    Added support for persist timer.
10/13/04    vp     Replaced ntohtcp(), htontcp() with tcp_hdr_parse/create().
                   Replaced the option construction in tcp_hdr_parse() from
                   tcp_output. Related changes.
10/04/04    sv     Modified SACK processing to store list of SACK blocks
                   instead of unack list.
09/07/04    sv     Fixed bug in mark_segment_sent where we are not updating the
                   unack list correctly if all the segs in list are rexmited.
08/16/04    sv     Added support for processing incoming SACK.
06/11/04    vp     Removed inclusion of psglobal.h, netuser.h and ineternet.h.
                   Included ps_pkt_info.h and ps_socket_defs.h. Replaced call
                   to byte ordering macros to their PS versions. Replaced
                   TCP_PTCL with PS_IPPROTO_TCP.
04/17/04    sv     Fixed bug in RTO back-off computation.
04/30/04    vp     Set ipcb ip_sec_reqd to true if ipsec_hdr_size > 0
04/26/03    sv     Added TCP timestamp option.
04/15/03    sv     Added TCP SACK option.
04/14/04    vp     Changes for DELAYED ACK as socket options.
03/02/04   rc      Modified call to ip_send() according to new signature.
02/10/04   rc      Changed parameters passed to ip_send() according to
                   new signature.
03/08/04    sv     Fixed re-tranmission timeout value computation in
                   tcp_output.
02/26/04    vp     Changed the name of function close self and macro seq_gt
                   and added tcp_ in front of their names.
01/20/04    sv     Modified code to advertize mss from TCB instead
                   of DEF_MSS for all the connections.
12/02/03    aku    Added missing return from tcp_ouput if we ran out of
                   memory.
10/01/03    sv     Renamed FEATURE_DS_TCP_NO_DELAY to
                   FEATURE_PS_TCP_NO_DELAYED_ACK
08/26/03    sv     Fixed a bug in calculating the segement size during
                   forced retransmission.
08/18/03    sv     Modified the code to use IPv6 addresses internally.
08/01/03    ss/sv  Restructured the code to support IPv6
06/06/03    om     Use ID field in ip_send() to indicate ESP processing.
06/03/03    ss     Added TCP_CLOSING state check for retransmitting FIN for
                   simulatenous close scenario.
06/02/03    ss     Renamed TCP_NO_DELAY to FEATURE_DS_TCP_NO_DELAY.
04/23/03    ss     Modified code to reflect new socket options handling
02/24/03    ss     Modified tcp_output() to form DSM item chain to support
                   MSS larger than a single DSM item size.
11/07/02    mvl    Added support for variable window size.
09/25/02    om     Use TCB's or SCB's routing cache for ip_send().
09/17/02    atp    tcp_output() to use tcb routing cache for non-socket build.
08/30/02    om     Updates to use socket's routing cache.
07/31/02    usb    Removed include files ps.h and psi.h, renamed psmisc.h
                   to ps_utils.h
07/10/02    ss     Cancel the delayed ack timer when a segment is transmitted
05/02/02    mvl    File cleanup and changed calls to ip_send() to reflect new
                   prototype
12/21/01    dwp    Add or !FEATURE_DATA_PS to some DSM tracer settings. This
                   is temporary until 5200 syncs up to latest DSM.
12/18/01    rsl    Updated tracer field.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
10/09/01    ss     Removed the code to cancel the retransmission timer
                   in tcp_output(). This fixes the bug when the timer is
                   cancelled but not started even though there is
                   unacknowledged data.
07/23/01    pjb    Changed call to dsm_offset_new_buffer to reserve space
                   for header.   Always set the push bit on TCP segments
                   with data.
02/14/01    js     Modified calls to ip_send() to support socket option
                   DSS_IP_TTL.
11/11/00    snn    Removed errno from the timer functions because they are
                   no longer used.
11/02/00    snn    Modified the back off to be Exponential backoff.
10/10/00    na     Added support for Multiple TCP socket support.
08/25/00    rc     Handled the case when htontcp() returns NULL.
                   Changed dsm_new_buffer() calls to take pool id as input
                   parameter. So, if pool size is to be passed in as
                   parameter, a new macro DSM_DS_POOL_SIZE is used to
                   return pool id based on pool size.
08/17/99    smp    Added include files msg.h & err.h.
06/06/98    na     Added a dog_report() in the main loop of tcp_output().
01/26/98    ldg    Removed the DS_SLIM switch.
09/24/97    na     Changed tcp backoff to linear from exponential. Put a limit
                   of max 6 backoffs.
                   Put code to bring down call if Tcb gets hosed.
06/25/97    jgr    Added ifdef FEATURE_DS over whole file
11/14/96    jjw    Changes to support Unwired Planet (UDP operation)
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/13/95    jjw    Changed min/max macros to upper case
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "ps_utils.h"
#include "psi.h"
#include "dsm.h"
#include "ps_tcp.h"

#include "ps_ip.h"
#include "msg.h"
#include "err.h"
#include "pstimer.h"
#include "amssassert.h"
#include "ps_pkt_info.h"
#include "ps_socket_defs.h"
#include "ps_mem.h"
#include "ps_stat_tcp.h"
#include "ps_iface.h"
#include "ps_ifacei.h"
#include "ps_pcb_close_reason_codes.h"
#include "PS_Sock_IO_IReader.h"
#include "PS_Sock_IO_IWriter.h"
#include "ds_Utils_DebugMsg.h"



/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*===========================================================================
FUNCTION TCP_UPDATE_HIGH_REXMIT()

DESCRIPTION
  This function updates the high_rexmit value to the next possible re-xmit.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates scoreboard
===========================================================================*/
static void  tcp_update_high_rexmit
(
  struct tcb *tcb,
  uint32  seqno,
  uint32  length
)
{
  struct tcp_sequence_block_list* curr_block_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tcb->high_seq_rexmit = MIN(seqno+length, tcb->high_seq_lost) ;

  curr_block_ptr = tcb->sack_blocks_head_ptr;

  while(curr_block_ptr != NULL &&
	tcp_seq_ge( tcb->high_seq_rexmit,curr_block_ptr->seq_block.start_seqno)
	)
  {
    if(tcp_seq_lt(tcb->high_seq_rexmit, curr_block_ptr->seq_block.end_seqno))
    {
      tcb->high_seq_rexmit = curr_block_ptr->seq_block.end_seqno;
      break;
    }
    curr_block_ptr = curr_block_ptr->next_ptr;
  }
  tcb->high_seq_lost = MAX(tcb->high_seq_lost , tcb->high_seq_rexmit);
}
/*===========================================================================
FUNCTION TCP_NEXT_SEGMENT()

DESCRIPTION
  This function returns the starting seqno of the next segment to be sent. If
  there are any segments that are considered lost, then re-xmit the lost
  segment else send the next segment in the sndq.

DEPENDENCIES
  None

RETURN VALUE
  Starting seqno of the next segment to be sent.

SIDE EFFECTS
  None
===========================================================================*/
static uint32  tcp_next_segment
(
  struct tcb *tcb
)
{
  if( tcb->high_seq_rexmit < tcb->high_seq_lost )
  {
    return tcb->high_seq_rexmit;
  }
  return tcb->snd.nxt;
}

/*===========================================================================
FUNCTION TCP_TIME_WITHIN

DESCRIPTION
  This function will perform timestamp comparisons.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if x is between low and high inclusive, otherwise FALSE.

SIDE EFFECTS
  None
===========================================================================*/
static int tcp_time_within
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
    if(!(low >= x && x >= high))
    {
      return 1;
    }
  }
  return 0;
} /* tcp_time_within() */

/*===========================================================================
FUNCTION TCP_NEXT_TIMESTAMP()

DESCRIPTION
  This function tries to give monotonically increasing timestamps. If the
  clock retracts back then it returns the timstamp returned last + 1.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - Timestamp in milliseconds.

SIDE EFFECTS
  None
===========================================================================*/
static uint32 tcp_next_timestamp(struct tcb *tcb)
{
  uint32 new_time = msclock();

  if(tcp_time_within(new_time,tcb->last_time,(tcb->last_time + 0x7fffffff)) ||
     (tcb->last_time == 0))
  {
    tcb->last_time = new_time;
  }

  return(tcb->last_time);
}/* tcp_next_timestamp() */

/*===========================================================================
FUNCTION TCP_OUTPUT()

DESCRIPTION
  This function will send a TCP segment on the specified connection. One gets
  sent only if there is data to be sent or if "force" is non zero.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tcp_output
(
  register struct tcb *tcb
)
{
  pseudo_header_type  ph;            /* Pseudo-header for checksum calcs   */
  struct tcp          seg;           /* Local working copy of header       */
  dsm_item_type      *pkt_head_ptr;  /* Head pointer for header+data buffer*/
  ps_flow_type       *ps_flow_ptr;
  ps_iface_type      *ps_iface_ptr;
  uint32              usable;        /* Usable window                      */
  uint32              sent;          /* Sequence count (incl SYN/FIN) already
                                        in the pipe but not yet acked      */
  uint32              rto;           /* Retransmit timeout setting         */
  uint32              ssize;         /* Size of current segment being sent,
                                        including SYN and FIN flags        */
  uint32              dsize;         /* Size of segment less SYN and FIN   */
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
  uint16              count;          /* iterative index for loop          */
  int16               ps_errno;
  int32               extracted;
  uint32              offset;
  int32               sndcnt = 0;
  uint32              rcvwnd;
  int16               tcp_syndata_flag;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( tcb == NULL || tcb->state == TCP_LISTEN || tcb->state == TCP_CLOSED)
  {
    return;
  }
  
  /* If the re-tx timer is currently overloaded, cancel it.
     Since the purpose of overloading is to ensure that tcp_output() gets 
     invoked. */
  if( tcb->timer_overload_flag && ps_timer_is_running(tcb->timer) )
  {
    /* This will be the case when the IFACE is enabled while overloaded 
       re-tx timer is running */
    LOG_MSG_INFO1_0("Cancelling overloaded re-tx timer");
    timer_ret_val = ps_timer_cancel( tcb->timer);
    if( PS_TIMER_SUCCESS != timer_ret_val )
    {
      LOG_MSG_ERROR_1("Error canceling timer 0x%x", tcb->timer);
    }

    /* re-tx timer is no more overloaded, clear the flag */
    tcb->timer_overload_flag = FALSE;
  }

  for(;;)
  {
    if ((tcb->ipcb.meta_info_ptr != NULL) &&
        (PS_TX_META_GET_RT_META_INFO_PTR(tcb->ipcb.meta_info_ptr) != NULL))
    {
      ps_iface_ptr =
        (ps_iface_type *) PS_TX_META_GET_ROUTING_CACHE( tcb->ipcb.meta_info_ptr);
      ps_flow_ptr =
        (ps_flow_type *) PS_TX_META_GET_FILTER_RESULT(tcb->ipcb.meta_info_ptr,
                                                   IP_FLTR_CLIENT_QOS_OUTPUT);
      if (ps_flow_ptr != NULL)
      {
        if (!PS_IFACE_COMMON_IS_FLOW_ENABLED(ps_iface_ptr, ps_flow_ptr))
        {
          if(NULL != ps_iface_ptr){
            MSG_4(MSG_SSID_DS,
                  MSG_LEGACY_MED,
                  "Not transmitting data as one of iface 0x%x:%d, flow 0x%x, or "
                  "phys link 0x%x are flow disabled",
                  ps_iface_ptr->name,
                  ps_iface_ptr->instance,
                  ps_flow_ptr,
                  PS_FLOW_GET_PHYS_LINK(ps_flow_ptr));

            /* If flow disabled before connection is established, we need
               additional logic to ensure TCP state machine is not stuck 
               forever. Since no TCP timers have yet been started.
               Overloading the re-tx timer to close the socket in case the 
               flow never gets enabled.
               If re-tx timer is already running(flow disabled after sending
               out first SYN) then no overloading logic required. */
            if( tcb->state == TCP_SYN_SENT && !ps_timer_is_running(tcb->timer) )
            {
              /* Overloaded re-tx timer expired. 
                 Flow didn't get enabled. */
              if(tcb->timer_overload_flag)
              {
                /* This will happen if the re-tx timer(MAX_RTO) has expired */
                LOG_MSG_INFO1_0("Overloaded re-tx timer expired, flow not "
                                "enabled. Closing socket");
                tcb->timer_overload_flag = FALSE;
                tcp_setstate(tcb,TCP_CLOSED);
              }
              else
              {
                /* Start the re-tx timer and set the overload flag */
                LOG_MSG_INFO1_0("Overloaded re-tx timer started");
                ps_timer_start(tcb->timer,MAX_RTO);
                tcb->timer_overload_flag = TRUE;
              }
            }
          }
          else
          {
            LOG_MSG_ERROR_0("tcp_output(): Interface pointer is NULL");
          }
          return;
        }
      }
    }

    tcb->snd.ptr = tcp_next_segment(tcb);
    /*-----------------------------------------------------------------------
      Compute data already in flight
    -----------------------------------------------------------------------*/
    sent = tcb->snd.ptr - tcb->snd.una;

    /*-----------------------------------------------------------------------
      Compute usable send window as minimum of offered and congestion
      windows, minus data already in flight.  Be careful that the window
      hasn't shrunk -- these are unsigned vars.

      usable is always 0 when window probes are to be sent as snd.wnd is 0.
      Window probes are sent only when persist timer is expired. So if
      persist timer is running and a zero window advertisement is received,
      data is not sent.
    -----------------------------------------------------------------------*/
    usable = MIN(tcb->snd.wnd,tcb->cwind);
    if(usable > tcb->numb_bytes_in_transit)
    {
      usable -= tcb->numb_bytes_in_transit; /* Most common case */
    }
    /*-----------------------------------------------------------------------
      Send window probes only when TCP's force flag is set. Else packets
      could be sent because of fast retransmit.
    -----------------------------------------------------------------------*/
    else if(usable == 0 && tcb->numb_bytes_in_transit == 0 &&
            0 < tcb->persist_backoff && tcb->flags.force)
    {
      usable = 1;  /* Closed window probe */
    }
    /*-----------------------------------------------------------------------
      We hit this block if we receive a SYN with window size set to 0.
      We set usable to 1 to send a reply to such a SYN.
    -----------------------------------------------------------------------*/
    else if( tcb->state == TCP_SYN_RECEIVED )
    {
      usable = 1;
    }
    else
    {
      usable = 0;  /* Window closed or shrunken */
    }

    /*-----------------------------------------------------------------------
      Compute size of segment we *could* send. This is the smallest of the
      usable window, the mss, or the amount we have on hand. (I don't like
      optimistic windows). If SACK option is permitted, make sure that we
      only transmit until high_seq_lost.
    -----------------------------------------------------------------------*/
    ssize = MIN((tcb->sndcnt - sent), usable);
    ssize = MIN(ssize, tcb->mss);

    if ( tcb->flags.sack_permitted && tcb->snd.ptr != tcb->snd.nxt
         && tcp_seq_gt(tcb->snd.ptr + ssize, tcb->high_seq_lost))
    {
      LOG_MSG_INFO2_3("SACK retran: snd.ptr=%lu high_seq_lost=%lu, ssize=%lu",
                tcb->snd.ptr, tcb->high_seq_lost, ssize );
      ssize = MIN( tcb->mss, tcb->high_seq_lost - tcb->snd.ptr);
    }

    /*-----------------------------------------------------------------------
      Now we decide if we actually want to send it.  Apply John Nagle's
      "single outstanding segment" rule.  If data is already in the pipeline,
      don't send more unless it is MSS-sized, the very last packet, or we're
      being forced to transmit anyway (e.g., to ack incoming data).
    -----------------------------------------------------------------------*/
    if( !tcb->flags.force &&
        !tcb->options.tcp_nodelay &&
        tcb->numb_bytes_in_transit != 0 &&
        ssize < tcb->mss &&
        !(tcb->state == TCP_FINWAIT1 && ssize == tcb->sndcnt-sent) )
    {
      ssize = 0;
    }

    /*-----------------------------------------------------------------------
      Unless the tcp syndata option is on, inhibit data until our SYN has
      been acked. This ought to be OK, but some old TCPs have problems with
      data piggybacked on SYNs.
    -----------------------------------------------------------------------*/
    tcp_syndata_flag = TCP_SYNDATA;
    /*lint -save -e774 */
    if(!tcb->flags.synack && !tcp_syndata_flag)
    /*lint -restore */
    {
      if(tcb->snd.ptr == tcb->iss)
      {
        ssize = MIN(1, ssize);       /* Send only SYN */
      }
      else
      {
        ssize = 0;  /* Don't send anything */
      }
    }

    if(tcb->flags.force && tcb->snd.ptr != tcb->snd.nxt)
    {

      /*---------------------------------------------------------------------
        When forced during a retransmission, we must send just an empty ack
        with the current sequence number to ensure it's accepted
      ---------------------------------------------------------------------*/
      if(tcb->flags.retran)
      {
        if ( tcb->flags.sack_permitted &&
             tcp_seq_gt(tcb->snd.ptr + ssize, tcb->high_seq_lost))
        {
          LOG_MSG_INFO2_3("SACK retran: snd.ptr=%lu high_seq_lost=%lu, ssize=%lu",
                    tcb->snd.ptr, tcb->high_seq_lost, ssize);
          ssize = MIN( tcb->high_seq_lost - tcb->snd.ptr,
                       MIN(tcb->sndcnt - sent, tcb->mss) );
        }
        else
        {
          ssize = MIN((tcb->sndcnt - sent), tcb->mss);
        }
      }
      else
      {
        if(tcb->snd.ptr == tcb->iss)
        {
          ssize = MIN(1, ssize);       /* Send only SYN */
        }
        else
        {
          ssize = 0;  /* Don't send anything */
        }
      }
    }

    if(ssize == 0 && !tcb->flags.force)
    {
      break;    /* No need to send anything */
    }
    tcb->flags.force = FALSE; /* Only one forced segment! */

    memset(&seg,0,sizeof(struct tcp));
    seg.source = tcb->conn.local.port;
    seg.dest   = tcb->conn.remote.port;

    /*-----------------------------------------------------------------------
      Set the flags according to the state we're in. It is assumed that if
      this segment is associated with a state transition, then the state
      change will already have been made. This allows this routine to be
      called from a retransmission timeout with force=1.
    -----------------------------------------------------------------------*/
    seg.urg       = 0;
    seg.rst       = 0;
    seg.ack_flag  = 1; /* Every state except TCP_SYN_SENT */
    seg.syn       = 0; /* syn/fin/psh set later if needed */
    seg.fin       = 0;
    seg.psh       = 0;
    seg.reserved2 = 0;
    seg.congest   = tcb->flags.congest;

  /*-----------------------------------------------------------------------
    TCP timestamp option when it is already negotiated with the peer.
   -----------------------------------------------------------------------*/
    if(tcb->flags.ts_permitted)
    {
      seg.ts_present = TRUE;
      seg.tsval = tcp_next_timestamp(tcb);
      seg.tsecr = tcb->tsecr;
    }
   /*-----------------------------------------------------------------------
        TCP SACK option
   -----------------------------------------------------------------------*/
    if(tcb->flags.sack_permitted && tcb->sack_option_count > 0)
    {
      seg.sack_block_cnt = MIN(tcb->sack_option_count,TCP_SACK_OPT_MAX_BLOCKS);

      for(count = 0; count < seg.sack_block_cnt ; count++)
      {
        seg.sack_blocks[count].start_seqno = tcb->sack_option[count].seqno;
        seg.sack_blocks[count].end_seqno = tcb->sack_option[count].seqno +
                                           tcb->sack_option[count].length;
      }
    }

    if(tcb->state == TCP_SYN_SENT)
    {
      seg.ack_flag = 0; /* Haven't seen anything yet */
    }

    dsize = ssize;

    if(!tcb->flags.synack && tcb->snd.ptr == tcb->iss)
    {

      /*---------------------------------------------------------------------
        Send SYN
      ---------------------------------------------------------------------*/
      seg.syn = 1;
      dsize--;            /* SYN isn't really in snd queue */

      /*---------------------------------------------------------------------
        TCP MSS option
      ---------------------------------------------------------------------*/
      seg.mss = (uint16)tcb->mss;

      /*---------------------------------------------------------------------
        TCP timestamp option of the SYN packet if it is enabled as option.
      ---------------------------------------------------------------------*/
      if(tcb->options.tcp_ts_permitted && seg.ack_flag == 0)
      {
        seg.tsval = tcp_next_timestamp(tcb);
        seg.tsecr = tcb->tsecr;
        seg.ts_present = TRUE;
      }

      if(tcb->options.tcp_sack_permitted)
      {
        seg.sack_permitted = TRUE;
      }

      if(tcb->flags.window_scale_enabled)
      {
        seg.window_scale_enable = TRUE;
        seg.window_scale = tcb->rcv.scale;
      }

    }

    if (tcb->writer != NULL)
    {
      sndcnt = GetSndCnt(tcb->writer);
    }

    if(sndcnt < (int32) tcb->rcv.wnd)
    {
      rcvwnd = tcb->rcv.wnd - sndcnt;
    }
    else
    {
      rcvwnd = 0;
    }
    /*-----------------------------------------------------------------------
      When sending an empty ack, use the latest sequence number to ensure
      acceptance in case we've been retransmitting
    -----------------------------------------------------------------------*/
    if(ssize == 0)
    {
      seg.seq = tcb->snd.nxt;
    }
    else
    {
      seg.seq = tcb->snd.ptr;
    }
    seg.ack = tcb->rcv.nxt;
    seg.wnd = (rcvwnd) >> tcb->rcv.scale;
    seg.up = 0;

    if( seg.ack != tcb->last_ack_sent && dsize != 0)
    {
      TCP_INC_STATS(piggybacked_acks_tx, 1);
      TCP_INC_INSTANCE_STATS(tcb,piggybacked_acks_tx, 1);
    }
    tcb->last_ack_sent = seg.ack;
    /*-----------------------------------------------------------------------
      Now try to extract some data from the send queue. Since SYN and FIN
      occupy sequence space and are reflected in sndcnt but don't actually
      sit in the send queue, extract will return one less than dsize if a FIN
      needs to be sent.
    -----------------------------------------------------------------------*/
    if(dsize != 0)
    {
      pkt_head_ptr = NULL;

      offset = sent;
      /*---------------------------------------------------------------------
        SYN doesn't actually take up space on the sndq, so take it out of the
        sent count
      ---------------------------------------------------------------------*/
      if(!tcb->flags.synack && sent != 0)
      {
        offset--;
      }

      extracted = Peek(tcb->reader, &pkt_head_ptr, dsize, offset, NULL,NULL,
                       &ps_errno);

      if(extracted != dsize)
      {
        /*-------------------------------------------------------------------
          Send a FIN. Without the check for states, this was the source of
          many bugs.
        -------------------------------------------------------------------*/
        if (( extracted == (dsize - 1)) &&
            ( TCP_FINWAIT1 == tcb->state ||
              TCP_LAST_ACK == tcb->state ||
              TCP_CLOSING == tcb->state ))
        {
          seg.fin = 1;
          dsize--;
        }
        else
        {
          /*-----------------------------------------------------------------
            Could happen if sendq is hosed, so just abort and clean up
          -----------------------------------------------------------------*/
          LOG_MSG_ERROR_3(
            "tcp sndq is corrupted, extracted %d dsize %d state %d",
            extracted, dsize, tcb->state);
          /*-----------------------------------------------------------------
            No point in sending a reset, because tcb might be corrupted
            too, so just close our TCP and then close PPP.
          -----------------------------------------------------------------*/
          tcp_close_self( tcb, PS_PCB_CLOSE_REASON_CODE_RESET);
          return;
        }
      }
    }
    else
    {
      pkt_head_ptr = NULL;
    }

    /*-----------------------------------------------------------------------
      If the entire send queue will now be in the pipe, set the push flag

      Note: Previous comment applies to following statement in the original
      TCP spec.  This statement applies when the commented section of the
      condition is included.  All modern TCP implementations set the push
      flag on all segments with data.
    -----------------------------------------------------------------------*/
    if(dsize != 0) /* ( && sent + ssize == tcb->sndcnt) */
    {
      seg.psh = 1;
    }

    /*-----------------------------------------------------------------------
      If this transmission includes previously transmitted data, snd.nxt will
      already be past snd.ptr. In this case, compute the amount of
      retransmitted data and keep score
    -----------------------------------------------------------------------*/
    if(tcb->snd.ptr < tcb->snd.nxt)
    {
      TCP_INC_STATS(segs_rexmit, 1);
      TCP_INC_STATS(bytes_rexmit, MIN(tcb->snd.nxt - tcb->snd.ptr, ssize));
      TCP_INC_INSTANCE_STATS(tcb,segs_rexmit, 1);
      TCP_INC_INSTANCE_STATS(tcb,
                             bytes_rexmit,
                             MIN(tcb->snd.nxt - tcb->snd.ptr, ssize));
    }
    tcb->snd.ptr += ssize;

    /*-----------------------------------------------------------------------
      If this is the first transmission of a range of sequence numbers,
      record it so we'll accept acknowledgments for it later
    -----------------------------------------------------------------------*/
    if(tcp_seq_gt(tcb->snd.ptr,tcb->snd.nxt))
    {
      tcb->snd.nxt = tcb->snd.ptr;
    }
    /*-----------------------------------------------------------------------
      Fill in fields of pseudo IP header
    -----------------------------------------------------------------------*/
    ph.source   = tcb->conn.local.address;
    ph.dest     = tcb->conn.remote.address;
    ph.protocol = (uint8)PS_IPPROTO_TCP;
    ph.length   = (uint16)dsize;

    /*-----------------------------------------------------------------------
      Generate TCP header, compute checksum, and link in data. NOTE: if
      'pkt_head_ptr' is NULL it will be reassigned in tcp_hdr_create().
    -----------------------------------------------------------------------*/
    if(tcp_hdr_create(&pkt_head_ptr, &seg,&ph) == FALSE)
    {
      if (pkt_head_ptr != NULL)
      {
        LOG_MSG_ERROR_0("Error creating tcp packet");
        dsm_free_packet(&pkt_head_ptr);
      }
      return;
    }

    /*-----------------------------------------------------------------------
      If we're sending some data or flags, start retransmission and round
      trip timers if they aren't already running.

      Note: Retransmisson timers are not started when window probes are
      sent. Whenever persist_backoff > 0, we would be here only when window
      probes are sent.
    -----------------------------------------------------------------------*/
    if (ssize != 0 && tcb->persist_backoff == 0)
    {
      /*---------------------------------------------------------------------
        Backoff function -- the subject of much research
        Using exponential backoff.
        Since n is uint32 type, check if n is greater than 31 and limit to
        31.  Multiply the rto with 2^n. This makes it exponential backoff.
      ---------------------------------------------------------------------*/
      uint32 n  = (tcb->backoff > TCP_MAX_BACKOFFS) ?
                    TCP_MAX_BACKOFFS : tcb->backoff;
      rto       = (tcb->srtt + (tcb->mdev << 2)) << n;

      /*---------------------------------------------------------------------
        Set round trip timer.
      ---------------------------------------------------------------------*/
      tcb->tcp_rexmit_timer_val = MAX(MIN_RTO, MIN(MAX_RTO, rto));
      tcb->tcp_rexmit_timer_val =
        MIN(tcb->tcp_rexmit_timer_val,
            tcb->max_backoff_time - tcb->total_backoff_time);
      LOG_MSG_INFO2_2("RTT Trace: tcb->tcp_rexmit_timer_val = %u, rto = %u",
                        tcb->tcp_rexmit_timer_val, rto);

      if (ps_timer_is_running(tcb->timer) == FALSE)
      {
        timer_ret_val =
          ps_timer_start( tcb->timer, tcb->tcp_rexmit_timer_val);
        if( PS_TIMER_SUCCESS != timer_ret_val )
        {
          LOG_MSG_ERROR_1("Error starting timer 0x%x", tcb->timer);
          ASSERT(0);
        }
      }

      /*---------------------------------------------------------------------
        If round trip timer isn't running, start it. The variable rtt_run
        keeps track if we are timing a segment or not. rttseq is the
        sequence number being timed.
      ---------------------------------------------------------------------*/
      if(!tcb->flags.rtt_run)
      {
        tcb->flags.rtt_run = TRUE;
        tcb->rtt_time      = (uint32)msclock();
        tcb->rttseq        = tcb->snd.ptr;
      }
    }

    DSM_TOUCH_ITEM(pkt_head_ptr);
    LOG_MSG_INFO3_2("TCP Out: seq = %x, ack = %x", seg.seq, seg.ack);

    if ( tcb->options.tcp_delayed_ack && ps_timer_is_running(tcb->acker))
    {
      LOG_MSG_INFO2_0("Cancelling TCP Ack Timer");
      timer_ret_val = ps_timer_cancel(tcb->acker);
      if( PS_TIMER_SUCCESS != timer_ret_val )
      {
        LOG_MSG_ERROR_1("Error canceling timer 0x%x", tcb->timer);
        ASSERT(0);
      }
    }

    tcb->ipcb.qos       = 0;
    tcb->ipcb.df        = 0;

    /*-----------------------------------------------------------------------
      Window probes are not considered as actual data transmission.
      So, update SACK blocks only if this is not a window probe and only
      window probes are sent when persist_backoff > 0
    -----------------------------------------------------------------------*/
    if (0 == tcb->persist_backoff)
    {
      if(ssize > 0 && tcp_seq_lt(seg.seq, tcb->high_seq_lost))
      {
        tcp_update_high_rexmit(tcb, seg.seq, ssize);
      }
    }
    else
    {
      TCP_INC_STATS(window_probe_tx, 1);
      TCP_INC_INSTANCE_STATS(tcb,window_probe_tx, 1);
    }
    tcb->numb_bytes_in_transit += ssize;

    TCP_INC_STATS(bytes_tx, dsize) ;
    TCP_INC_STATS(segs_tx, 1) ;
    TCP_INC_INSTANCE_STATS(tcb,bytes_tx, dsize) ;
    TCP_INC_INSTANCE_STATS(tcb,segs_tx, 1) ;
    /*-----------------------------------------------------------------------
      Call ip_send with customized or default TTL value depending on whether
      the scb_ptr is NULL or not
    -----------------------------------------------------------------------*/
    (void)ip_send(  &tcb->ipcb,
              tcb->conn.remote.address,
              (uint8)PS_IPPROTO_TCP,
              pkt_head_ptr,
              ph.length,
              NULL,
              &ps_errno);

    /*-----------------------------------------------------------------------
       Dont need to report DOG, it's costly do for each packet. This was 
	   needed in early use-cases (where modem was fully involved in big tput
	   run). Skipping for now, will assess as the need may be. Refer Dog HB.
    -----------------------------------------------------------------------*/
  } /* for(ever) */

} /* tcp_output() */
