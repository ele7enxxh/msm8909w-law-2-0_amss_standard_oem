/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            P S _ T C P I N . C

GENERAL DESCRIPTION
  Process incoming TCP segments. Page number references are to ARPA RFC-793,
  the TCP specification.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

 * Copyright 1991-1993 Phil Karn, KA9Q
 * Additional material Copyright 1992-1995 William Allen Simpson

Copyright (c) 1995-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcpin.c_v   1.9   20 Feb 2003 10:25:36   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_tcpin.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/09/15   ds      Start persist timer only if required
01/20/15   ss      Add IP family check before picking up TCP server socket
07/03/14   ss      Update routing cache in meta info for an incoming SYN 
                   packet
11/05/13   fn      Fix to use ipcb for TCP rst in case of netpolicy mismatch
07/08/13   vp/fn   Fix IP sec encapsulation for TCP resets
03/15/13   vp/ssh  Use ip_calc_path_mtu for MTU calculations
12/03/12   ds      Fix to print the socket fd in F3 message when there is 
                   route scope mismatch.
09/21/12   ss      Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR and 
                   evaluating ASSERT requirement.
01/17/12   mct     Fix to ensure incoming IPSec TCP connections respond with 
                   appropriate encryption
01/12/11   cp      Lint warning fixes.
07/27/10   ps      Added code changes to fix the broken Async/Fax support.
07/20/10   sy      Removed critical section.
03/25/10   am      Updated the dsm item avail check for dup items.
03/11/10   am      Mod DSM item avail check for incoming segment's pool in
                   TCP reseq.
02/02/10   am      Process seg window update in FINWAIT1/CLOSING/LAST_ACK too
01/11/10   ss      Lint warning fixes.
09/24/09   ss      KW warning fixes
05/22/09   kk      Q6 compiler warning fixes.
23/04/09   am      Check SACK block SN range before processing it.
10/31/08   mct     MSS should be dynamically calculated based on iface type.
10/22/08   dm      Modified TCP and IP4 headers to fix compiler warnings
06/10/08   am      Force retrans when 3 dupacks are received. In case of
                   retrans, ssthresh is based on numb_bytes_in_transit.
11/08/07    hm     Fixed a bug in TCP RTT computation
03/20/07   msr     Resetting TCP connection if 120s have been elapsed in
                   backoffs instead of waiting for MAX_BACKOFFS
02/05/07    rt     Removed EIFEL code.
12/05/06   msr     Passing sockfd in ps_event_info_type
11/22/06   sv      IPSEC transport mode support.
10/19/06   hm      Fixed a SACK retransmission bug for not retransmitting
                   data already SACKed.
10/13/06   hm      Cleaned up the code, added comments for RTT algorithm
10/12/06   hm      Fixed code for negotiating timestamp when tsval sent in
                   the SYN is 0.
07/31/06   rt      Route Scope changes in tcp_input().
06/13/06   msr     Making sure that timers are non-NULL before manipulating
                   them. Async TCB is not creating all the timers
02/22/06   rt      Using single critical section
02/21/06   msr     Checking if snd.nxt == snd.una instead of using
                   num_bytes_in_transit in order to start persist timer
02/07/06   rt      L4 TASKLOCK changes.
01/16/06   vp/rt   Allowing rcv.nxt when rcv.wnd is 0.
12/03/05   rt      Added code for new tcp option DSS_TCP_EIFEL.
11/07/05   msr     Reverted my earlier change of transmitting the byte sent
                   in window probe. It was already handled as part of persist
                   timer implementation
10/17/05   ks      Removed compiler warning and fixed NULL ptr reference.
08/31/05   sv      Update bytes in transit correctly when we get duplicate
                   SACK information.
09/12/05   msr     Transmitting the byte sent in window probe again if it is
                   not acked in window update
08/15/05   mct     Changed pkt_info to look at psuedo_hdr as v6 is not avail.
08/15/05   vp      Check for if its a sys_sock if iface is in configuring
                   state.
08/08/05   kr      Removed Feature DS_PSTATS and added support for updating
                   new stats infrastructure
07/14/05   sv      Send duplicate ack when we receive out of order FIN.
05/12/05   rt      Fixed lint errors.
04/25/05   vp      Send reset when dropping packet due to failed netpolicy.
04/18/05   vp      Changes for enforcing netpolicy on input.
03/31/05   rt      Fixed bug occuring due to non setting of the previous
                   pointer in doubly link list of sack block.
02/28/05   sv      Removed references to old IPSEC code.
02/22/05   ssh    Replaced INTLOCK()s with TASKLOCK()s
02/16/05   msr     Not caching RTT, anymore.
01/17/05   ks      memsetting IP control block in tcp_input.
12/27/04   ifk     Replaced setting of tracer field with DSM_TOUCH_ITEM
12/20/04   msr     Fixed tcp_input() to use numb_bytes_in_transit instead of
                   snd_ptr. Fixed update() to resolve integer underflow of
                   numb_bytes_in_transit when fast retransmit is triggered.
                   Starting persist timer only once in tcp_input(). Removed
                   setting of force flag for fast retransmits.
11/15/04   ks      Removed LOG_MSG_FATAL_ERROR in get_reseq() for the condition when
                   the queue is empty.
11/15/04   vp      Fixed the processing of out of order FIN.
11/15/04   vp      Fixed a bug so that TCP responds to segments out of window
                   with an ACK.
10/25/04   sv      Fixed subset match case in process_sack to check greater
                   than or equal to.
10/25/04   msr     Added support for persist timer.
10/13/04   vp      Replaced ntohtcp(), htontcp() with tcp_hdr_parse/create().
                   Removal of byte order conversions for ports. Related changes.
10/04/04   sv      Modified SACK processing to store list of SACK blocks
                   instead of unack list
08/30/04   sv      Moved graceful dormant close flag and keep alive idle time
                   constants to socket config control block.
08/19/04   sv      Added support for processing incoming SACK option.
08/19/04   vp      Added F3 messages when we receive a RESET tcp packet.
07/30/04   vp      Changes so that TCP uses MTU set in ps_iface.
06/11/04   vp      Removal of inclusion of psglobal.h, internet.h,netuser.h and
                   iface.h. Included ps_pkt_info.h and ps_socket_defs.h.
                   Replaced TCP_PTCL with PS_IPPROTO_TCP. Replaced call to byte
                   ordering macros with their PS versions. Renamed macro PREC
                   to IP_PRECEDENCE.
05/19/04   vp      Fixed the assignment to tcb->cwind in update() and proc_syn
                   for Async/Fax
05/17/04   sv      Fixed bug in get_reseq to update the sack block correctly.
05/17/04   sv      Modified tcp_input to ack the FIN received in TIME_WAIT
                   state.
04/30/04   sv      Fixed compiler warnings.
04/26/04   sv      Added TCP timestamp option.
04/15/04   sv      Added TCP SACK option.
04/14/04   vp      Changes for KEEPALIVE and DELAYED ACK as socket options.
03/16/04   mct     Renamed acl_ipfilter_info_type to ip_pkt_info_type.
03/02/04   rc      Modified call to ip_send() according to new signature.
02/10/04   rc      Changed parameters passed to ip_send() according to
                   new signature.
02/04/04   vp      Changed the signature of send_reset function to include
                   pseudo_hdr and ip_cb structure as input paramters.
02/04/04   vp      Changed the signature of tcp_input function to take
                   ip_pkt_info_type structure, dsm item and offset.
11/11/03   aku/rc  Passed additional parameter 'offset' in call to cksum().
03/01/04   sv      Fixed congestion window update bug in update function.
02/26/04   vp      Changed the names of functions send_syn, close_self,
                   rtt_add, rtt_get, seq_within, setstate, send_reset and
                   prepended tcp_ in front of their names.
02/26/04   vp      Changed the names of macros seq_lt, seq_gt, seq_ge and
                   geniss and prepended tcp_ before their names.

01/28/04   sv      Changed update function to reset the cwind to DEF_CWND when
                   we receive MAX_DUP_ACKS.
01/27/04   vp      Added call to TCP_INFO_CHANGE_UPCALL when cwind changes
                   when MAX_DUP_ACKS are received in update()
11/11/03   sv      Modified call to t_upcall to pass number of bytes acked
                   instead of tcb sndcnt.
11/03/03   mct     Updated calls to ntohtcp() to reflect new boolean value.
10/01/03   sv      Renamed FEATURE_DS_TCP_NO_DELAY to
                   FEATURE_PS_TCP_NO_DELAYED_ACK
09/09/03   sv      Changed tcp_input processing to enqueue any data/fin
                   sent along with a syn packet.
08/18/03   sv      Modified the code to use IPv6 addresses internally.
08/01/03   ss/sv   Restructured the code to support IPv6
07/22/03   sv      Modified the code to use new DSM macros for getting the
                   DSM declarations.
06/27/03   sv      Removed the code that uses limit field in tcb to limit
                   the sndq size. It is now handled by sndbuf socket option.
06/04/03   om      Fixed MSS setting to adjust for IPSec ESP overhead.
06/02/03   ss      Renamed TCP_NO_DELAY to FEATURE_DS_TCP_NO_DELAY.
02/20/03   om      Removed debug message.
02/12/03   om      Setting routing cache in TCB.
11/13/02   rc      Wrapped code in !FEATURE_GSM_GPRS
09/25/02   om      Set the routing cache for LISTEN & unbound sockets.
09/20/02   na      Fix: In tcp_input(), for sockets calls when a ACK is recvd
                   tcp_output is called.
08/30/02   om      Updates to use receive iface for sending a TCP reset.
08/01/02   om      Removed dependecy on if_cb for interface's MTU.
07/31/02   usb     Added include ps_iputil.h
07/31/02   usb     Removed include files ps.h and psi.h, renamed psmisc.h
                   to ps_utils.h
07/15/02   aku     Replaced ps_in_rawdata_mode() with
                   ps707_async_in_rawdata_mode() and ps_process_tcp_send_q()
                   with ps707_async_process_tcp_send_q()
07/10/02   ss      Made changes to incorporate PS timer changes
05/02/02   mvl     File cleanup and changed calls to ip_send() to reflect new
                   prototype
05/29/02   vr      fixed tcpin to do differential processing only if both
                   small and large pools hit transport few
04/17/02   rc      Wrapped code in !FEATURE_DATA_WCDMA_PS
03/05/02    pjb    Finished Accept Listen Code Review Items
02/22/02    pjb    Added accept and listen
02/11/02   ss      Introduced processing of ack and discarding data of
                   incoming segments when running low on memory.
                   tcp_cleanup_reseq() now cleans up whole reseq queue.
12/21/01   dwp     Add or !FEATURE_DATA_PS to some DSM tracer settings. This
                   is temporary until 5200 syncs up to latest DSM.
12/18/01   rsl     Updated tracer field.
11/12/01   dwp     Add "|| FEATURE_DATA_PS" to whole module. Wrap out
                   ps_process_tcp_send_queue() if FEATURE_DATA_PS is defined.
11/05/01   vr      Add call to reset if packet recvd for invalid connection
10/09/01   ss      Modified update() to cancel the timer before
                   retransmitting due to triple duplicate acks. Also, when
                   some data is acked, ps_timer_cancel() is now called
                   only when there is data in flight.
08/08/01   aku     Removed call to reset on reception of packet for invalid
                   connection
07/23/01   pjb     Changed call to dsm_offset_new_buffer to reserve space
                   for header. Save the reserved bit from the TCP header.
06/14/01   snn     Fixed the bug in tcp_input while reading the resequence
                   queue. It was doing an extra network to host (get32)
                   conversion on the sequence number.
01/19/01   na      Fixed bug in add_reseq where a extra network to host(get32)
                   was being done. Fixed some F3 msgs.
11/11/00   snn     Removed errno from the timer functions because they are
                   no longer used.
10/10/00   na/snn  Removed tcp_rexmit_timer_val as a global variable and
                   added to the TCB. This was done because each TCP socket
                   can have its own timer value.
08/25/00   rc      Changed dsm_pushdown() to take take pool id as fourth
                   parameter.Made changes to handle dsm_pushdown()
                   returning FALSE.
                   Changed dsm_new_buffer() calls to take pool id as input
                   parameter. Added Macros that check if we are running
                   out of items.
                   Handled the case when htontcp() returns NULL.
                   Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK
06/07/00   rc      Removed ovld of link ptr in dsm items.
02/02/00   ak      Merged branch and tip.
12/23/99   na      Changed queue functions that use dsm items, to use the
                   ovld of link ptr in dsm items.
12/06/99   hcg     In tcp_cleanup_reseq(), changed function call from
                   dsm_q_last_get() to q_last_get() as the function is now
                   provided as part of queue services.
11/16/99   rc      Fixed mis-aligned access problem in add_reseq() and
                   tcp_input().
11/04/99   rc      Added code to make sure that if we get a FIN, regardless of
                   RST, we return DS_EEOF when finished reading from the pipe,
                   rather than returning DS_ECONNRESET.
04/25/99   hcg     Added INTLOCK()/INTFREE() around updates for TCB rcvq.  Fixes
                   task synchronization problem for sockets interface.  Also
                   added packet trimming to ensure length of packet is consistent.
04/09/99   hcg     Added FEATURE_Q_NO_SELF_QPTR to support queue services memory
                   optimization.
03/31/99   hcg     Changed LOG_MSG_INFO2 to LOG_MSG_INFO3 for duplicate ACKs received to
                   reduce number of DM messages during ASYNC calls.
02/25/99   na/hcg  Added support for DSSock sockets interface under
                   FEATURE_DS_SOCKETS.  Added code for tcp_input() routine.
                   Also, added code to free dsm items, which could lead to
                   memory leaks.
12/14/98   ldg     Removed dangling comma from tcp_cleanup_reseq() parameter
12/05/98   na      Modified tcp_cleanup_reseq to clean one item at a time.
10/30/98   na      Added tcp_cleanup_reseq to clean up the resequence queue.
10/15/98   na      Changes for improved bidirectional performance - call
                   ps_process_tcp_send_queue instead of tcp_output().
07/26/98   jjn     Use more efficient DSM.
06/17/98   ldg     Stop waiting until backoffs=0 to copy backoffs to history.
05/06/98   na      Locked interrupts around changes in the sndcnt variable.
04/14/98   ldg     Added counters for payload received, and backoffs.
04/14/98   na      Added support for buffer based flow control.
01/26/98   ldg     Removed the DS_SLIM switch.
09/19/97   na      Fixed TCP problem where we were getting TCP segment which
                   passes the checksum, but the actual length does not match
                   the length in the IP header.
09/09/97   ldg     (na) Send ACK before passing segment to higher layer.
07/31/97   ldg     Added counter for segments received out of order.
06/17/97   fkm     FEATURE_xxx Updates (via Automatic Scripts)
11/14/96   jjw     Changes to support Unwired Planet (UDP operation)
10/08/96   jjw     Disabled Delayed ACK
07/08/96   jjw     Limited MSS setting to the DEF_MSS.
05/24/96   jjw     Gemini & ISS2 Data Services release.
11/13/95   jjw     Changed min/max macros to upper case
11/09/95   jjw     IS-99 Compliant Release

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "ps_utils.h"
#include "dsm.h"

#include "ps_tcp.h"
#include "ps_icmp.h"
#include "ps_tcp_async.h"

#include "ps_ip.h"
#include "ps_iputil.h"
#include "amssassert.h"
#include "memory.h"
#include "msg.h"
#include "err.h"
#include "ps_pkt_info.h"
#include "ps_socket_defs.h"
#include "ps_mem.h"
#include "ps_route.h"
#include "ps_route_scope.h"
#include "ps_iface.h"
#include "ps_ifacei.h"
#include "dsstcpi.h"
#include "dssocket.h" /* for DSS_SO_SYS_SOCK */
#include "ps_pcb_close_reason_codes.h"

#include "pstimer.h"
#include "ps_stat_tcp.h"
#include "PS_Sock_IO_IReader.h"
#include "PS_Sock_IO_IWriter.h"

#ifdef FEATURE_DATA_IS707
#include "ps707_async.h"
#endif /* FEATURE_DATA_IS707 */

#include "ds_Utils_DebugMsg.h"
#include "dss_config.h"

#include "ps_iface_addr_v6.h"
#include "ps_in.h"

#include "ds_Addr_Def.h"

#define  TCP_LOW_THRESHOLD   4


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION IN_WINDOW()

DESCRIPTION
  This function will Determine if the given sequence number is in our
  receiver window.

DEPENDENCIES
  Must not be used when window is closed!

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static int in_window
(
  struct tcb *tcb,
  uint32     seq
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  return tcp_seq_within(seq,tcb->rcv.nxt,(uint32)(tcb->rcv.nxt+tcb->rcv.wnd-1));
} /* in_window() */

/*===========================================================================

FUNCTION TCP_SEND_SYN()

DESCRIPTION
  This function will Generate an initial sequence number and put a SYN on the
  send queue

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tcp_send_syn
(
  register struct tcb *tcb
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  tcb->iss             = tcp_geniss();
  tcb->high_seq_lost   = tcb->iss;
  tcb->high_seq_rexmit = tcb->iss;
  tcb->rttseq          = tcb->snd.wl2 = tcb->snd.una = tcb->iss;
  tcb->snd.ptr         = tcb->snd.nxt = tcb->rttseq;
  tcb->sndcnt++;
  tcb->flags.force = 1;
} /* tcp_send_syn() */

/*===========================================================================

FUNCTION TCP_UPDATE_SACK_OPTION()

DESCRIPTION
  This function updates the SACK option array containing the SACK options to
  be sent in the next segment with the currently received out of order segment.

DEPENDENCIES
   None.

RETURN VALUE
  None

SIDE EFFECTS
  Update the SACK option array with the currently received out of order
  segment.
===========================================================================*/
static void tcp_update_sack_option
(
  struct tcb * tcb_ptr,
  uint32       seqno,
  uint32       length
)
{
  tcp_reseq_hdr curr_sack_block;               /* sack block to be written */
  tcp_reseq_hdr next_sack_block = {0,0};                /* next sack block */
  uint8         write_index;                        /* current write index */
  uint8         read_index;                          /* current read index */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  write_index = 1;
  curr_sack_block =  tcb_ptr->sack_option[0];
  tcb_ptr->sack_option[0].length =  length;
  tcb_ptr->sack_option[0].seqno = seqno;

  for(read_index =0; read_index < tcb_ptr->sack_option_count &&
        write_index < TCP_SACK_OPT_MAX_BLOCKS; read_index++)
  {
    if(read_index + 1 < tcb_ptr->sack_option_count)
    {
      next_sack_block = tcb_ptr->sack_option[read_index + 1];
    }

    if(tcp_seq_gt(curr_sack_block.seqno, seqno+length) ||
        tcp_seq_lt(curr_sack_block.seqno, seqno))
    {
      tcb_ptr->sack_option[write_index++] = curr_sack_block;
    }

    curr_sack_block = next_sack_block;
  }

  tcb_ptr->sack_option_count = write_index;

}

/*===========================================================================

FUNCTION TCP_DELETE_SACK_BLOCK()

DESCRIPTION
 This function deletes the passed SACK block entry if it is present in the
 SACK block array. SACK block array maintains 4 recent SACK blocks to be
 sent along with the next segment.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Delets an entry from the SACK block array.
===========================================================================*/
static void tcp_delete_sack_block
(
  struct tcb * tcb_ptr,
  uint32       seqno,
  uint32       length
)
{
  boolean sack_block_found = FALSE;
  uint8  count;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for(count = 0; count <  TCP_SACK_OPT_MAX_BLOCKS ; count++)
  {
    if(tcb_ptr->sack_option[count].length == length &&
       tcb_ptr->sack_option[count].seqno == seqno)
    {
      sack_block_found = TRUE;
      break;
    }
  }

  for(;count <  TCP_SACK_OPT_MAX_BLOCKS-1 ; count++)
  {
    tcb_ptr->sack_option[count].length = tcb_ptr->sack_option[count+1].length;
    tcb_ptr->sack_option[count].seqno = tcb_ptr->sack_option[count+1].seqno;
  }

  if(sack_block_found)
  {
    tcb_ptr->sack_option[TCP_SACK_OPT_MAX_BLOCKS-1].length = 0;
    tcb_ptr->sack_option[TCP_SACK_OPT_MAX_BLOCKS-1].seqno = 0;
    tcb_ptr->sack_option_count --;
  }
}
/*===========================================================================

FUNCTION TCP_CLEANUP_RESEQ()

DESCRIPTION
  This function cleans up all the elements from the TCP resequencing
  queue for the given TCP connection

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tcp_cleanup_reseq
(
  register struct tcb *tcb        /* ptr to TCP Task control block         */
)
{
   dsm_item_type* item_ptr;                          /* temporary item ptr */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Cleanup TCP reseq. queue.
   ------------------------------------------------------------------------*/
  while (( item_ptr = (dsm_item_type *)q_last_get( &(tcb->reseq))) != NULL)
  {
    dsm_free_packet( &item_ptr);
  }

} /* tcp_cleanup_reseq() */

/*===========================================================================

FUNCTION GET_RESEQ()

DESCRIPTION
  This function will Fetch the first entry off the resequencing queue. This
  includes getting the segment data, loading the TCP Segment data structure
  with accompanying values, and loading the Segment data length variable.

DEPENDENCIES
1)The format of the data on the Re-sequencing queue must contain the TCP
  segment data structure values and the segment data length stored in front
  of the actual data, respectively.
2)There must be a Re-seq data item on the Re-sequencing queue.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void get_reseq
(
  register struct tcb *tcb,       /* ptr to TCP Task control block         */
  tcp_reseq_hdr * reseq_hdr,      /* ptr to reseq hdr                      */
  struct dsm_item_s   **data_ptr  /* Addr of ptr for Retrieval of seg data */
)
{
  struct dsm_item_s * reseq_item_ptr;
  uint32 seqno;
  uint32 length;
  uint32 dupcnt;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  reseq_item_ptr = (dsm_item_type *)q_check( &tcb->reseq);
  *data_ptr = NULL;

  if(reseq_item_ptr == NULL)
  {
    return;
  }

  (void)dsm_extract(reseq_item_ptr, 0, &length, sizeof(uint32));
  (void)dsm_extract(reseq_item_ptr, 4, &seqno, sizeof(uint32));

  if(tcp_seq_gt(seqno, tcb->rcv.nxt))
  {
    return;
  }
  do
  {
    /*-----------------------------------------------------------------------
      Set the Type of service then pullup the TCP Segment data  values into
      the passed data structure then do the same with the segment data length.
      -----------------------------------------------------------------------*/
    if((*data_ptr = (dsm_item_type *)q_get(&tcb->reseq)) == NULL)
    {
      break;
    }

    (void)dsm_pullup( data_ptr, &(length), sizeof(uint32));
    (void)dsm_pullup( data_ptr, &(seqno), sizeof(uint32));

    dupcnt = MIN(length, tcb->rcv.nxt - seqno);
    (void)dsm_pullup( data_ptr, NULL, (uint16)dupcnt);


    if(tcb->flags.sack_permitted)
    {
      tcp_delete_sack_block(tcb,seqno, length);
    }
    seqno +=dupcnt;
    length -= dupcnt;
  }while(seqno < tcb->rcv.nxt);

  if(seqno == tcb->rcv.nxt)
  {
    reseq_hdr->seqno = seqno;
    reseq_hdr->length = length;
  }

  return;
} /* get_reseq() */

/*===========================================================================

FUNCTION ADD_RESEQ()

DESCRIPTION
  This function will add the passed TCP Segment onto the TCP Re-Sequencing
  queue. Along with the segment the segment length and the TCP head info
  is saved away.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The data item on the Re-sequencing queue will store the data item payload
  length and the TCP Segment data structure values.
===========================================================================*/

static void add_reseq
(
  struct tcb        *tcb,                 /* TCP control block for segment */
  struct tcp        *seg,                 /* TCP Seg data struct ptr       */
  struct dsm_item_s *seg_data_ptr,        /* ptr to TCP segment data       */
  uint32             seg_length           /* length of TCP segment data    */
)
{
  dsm_item_type *next_item_ptr;           /* for appending dsm items       */
  dsm_item_type *curr_item_ptr;           /* for looking thru Re-seq list  */
  dsm_item_type *new_item_ptr;            /* new node to insert in the list*/
  uint32         reseq_seqno;
  uint32         reseq_length;
  uint32         seqno;
  uint32         length;
  uint32         dupcnt;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*----------------------------------------------------------------------
     When a fin is received, we store its seq_no+length as fin_seqno.
     This allows us to recognize the valid segments even though they
     are received after fin.
   ---------------------------------------------------------------------*/
  if(tcb->flags.fin_recvd &&
     tcp_seq_gt(seg->seq + seg_length , tcb->fin_seqno + 1))
  {
    LOG_MSG_INFO1_2("Receiving packets after FIN: seqno %d, fin-seqno %d",
                    seg->seq +seg_length, tcb->fin_seqno);
    dsm_free_packet(&seg_data_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Place complete 'package' onto the Re-sequencing queue in ascending order
    using the Sequence number. Start by finding correct spot in the list.
  -------------------------------------------------------------------------*/
  curr_item_ptr = (dsm_item_type *)q_check( &tcb->reseq);
  new_item_ptr = seg_data_ptr;
  seqno = seg->seq;
  length = seg_length;

  while( curr_item_ptr != NULL)
  {
    (void)dsm_extract(curr_item_ptr, 0, &reseq_length, sizeof(uint32));
    (void)dsm_extract(curr_item_ptr, 4, &reseq_seqno, sizeof(uint32));

    /*-----------------------------------------------------------------------
      If Seq# of received segment is less than the Seq# of current segment on
      the resequencing queue, then break from the while loop. This will
      ensure that the received segment gets inserted before current segment
      on the resequencing queue.
    -----------------------------------------------------------------------*/
    if( tcp_seq_lt(seqno, reseq_seqno) )
    {
      break;
    }

    if(tcp_seq_ge(reseq_seqno + reseq_length,seqno))
    {

        next_item_ptr =
          (dsm_item_type *)q_next( &tcb->reseq, &curr_item_ptr->link);

#ifdef FEATURE_Q_NO_SELF_QPTR
        q_delete(&tcb->reseq, &(curr_item_ptr->link));
#else
        q_delete(&(curr_item_ptr->link));
#endif

        dupcnt = MIN(length, (reseq_seqno+ reseq_length - seqno));

        (void)dsm_pullup(&new_item_ptr,
                    NULL,
                    (uint16)dupcnt);

        (void)dsm_pullup(&curr_item_ptr,
                   NULL,
                   sizeof(uint32) + sizeof(uint32));

        dsm_append(&curr_item_ptr, &new_item_ptr);
        new_item_ptr = curr_item_ptr;

        length += (reseq_length - dupcnt);
        seqno = reseq_seqno;

        curr_item_ptr = next_item_ptr;
        break;
    }

    curr_item_ptr =
      (dsm_item_type *)q_next( &tcb->reseq, &curr_item_ptr->link);
  } /* while */


  while(curr_item_ptr != NULL)
  {
    (void)dsm_extract(curr_item_ptr, 0, &reseq_length, sizeof(uint32));
    (void)dsm_extract(curr_item_ptr, 4, &reseq_seqno, sizeof(uint32));

    if( tcp_seq_lt(seqno + length , reseq_seqno))
    {
      break;
    }

    next_item_ptr =
      (dsm_item_type *)q_next( &tcb->reseq, &curr_item_ptr->link);
#ifdef FEATURE_Q_NO_SELF_QPTR
    q_delete(&tcb->reseq, &(curr_item_ptr->link));
#else
    q_delete(&(curr_item_ptr->link));
#endif

    dupcnt = MIN(reseq_length, (seqno+ length - reseq_seqno));
    (void)dsm_pullup(&curr_item_ptr,
               NULL,
               (uint16)dupcnt+ sizeof(uint32) + sizeof(uint32));

    length += reseq_length - dupcnt;
    dsm_append(&new_item_ptr, &curr_item_ptr);
    curr_item_ptr = next_item_ptr;
  }


  /*-------------------------------------------------------------------------
    Push Data length and  seqno  onto data item for use during later
    Re-sequencing.
  -------------------------------------------------------------------------*/

  if (dsm_pushdown( &new_item_ptr, &seqno, sizeof(uint32),
                    DSM_DS_SMALL_ITEM_POOL) <
      sizeof(uint32))
  {
    LOG_MSG_ERROR_0("Out of Memory!");
    dsm_free_packet(&new_item_ptr);
    return;
  }

  if (dsm_pushdown( &new_item_ptr, &length, sizeof(uint32),
                    DSM_DS_SMALL_ITEM_POOL) <
      sizeof(uint32))
  {
    LOG_MSG_ERROR_0("Out of Memory!");
    dsm_free_packet(&new_item_ptr);
    return;
  }

  DSM_TOUCH_ITEM(new_item_ptr);

  if(curr_item_ptr == NULL)
  {
    LOG_MSG_INFO1_2( "PUT to Reseq: %lx, nxt=%lx", seg->seq, tcb->rcv.nxt);
    q_put( &tcb->reseq, &new_item_ptr->link);
  }
  else
  {
#ifdef FEATURE_Q_NO_SELF_QPTR
    q_insert( &tcb->reseq,
              &new_item_ptr->link,
              &curr_item_ptr->link);
#else
    q_insert(  &new_item_ptr->link,
               &curr_item_ptr->link);
#endif
    LOG_MSG_INFO1_2( "INSERT to Reseq: %lx, nxt=%lx", seg->seq, tcb->rcv.nxt);
  }

  if(tcb->flags.sack_permitted)
  {
    tcp_update_sack_option(tcb, seqno, length);
  }
} /* add_reseq() */

/*===========================================================================

FUNCTION TCP_SEND_RESET()

DESCRIPTION
  This function will Send an acceptable reset (RST) response for this segment
  The RST reply is composed in place on the input segment

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tcp_send_reset
(
  register struct tcp      *seg,              /* Offending TCP header      */
  pseudo_header_type       *pseudo_hdr,       /* tcp pseudo hdr            */
  ip_cb_type               *ip_cb             /* IP control block          */
)
{
  struct dsm_item_s    *header_ptr = NULL;  /* ptr to item with TCP header */
  uint16 tmp;                               /* temp source port holder     */
  int16  ps_errno;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(seg->rst)
  {
    return; /* Never send an RST in response to an RST */
  }

  /*-------------------------------------------------------------------------
    Change the pseudo header length for the reset packet to 0 as we send no
    data. TCP hdrlen will be added by tcp_hdr_create.
  -------------------------------------------------------------------------*/
  pseudo_hdr->length = 0;

  /*-------------------------------------------------------------------------
    Swap port numbers
  -------------------------------------------------------------------------*/
  tmp         = seg->source;
  seg->source = seg->dest;
  seg->dest   = tmp;

  if(seg->ack_flag)
  {
    /*-----------------------------------------------------------------------
      This reset is being sent to clear a half-open connection.  Set the
      sequence number of the RST to the incoming ACK so it will be
      acceptable.
    -----------------------------------------------------------------------*/
    seg->ack_flag = 0;
    seg->seq       = seg->ack;
    seg->ack       = 0;
  }
  else
  {
    /*-----------------------------------------------------------------------
      We're rejecting a connect request (SYN) from TCP_LISTEN state so we
      have to "acknowledge" their SYN.
    -----------------------------------------------------------------------*/
    seg->ack_flag = 1;
    seg->ack       = seg->seq;
    seg->seq       = 0;
    if(seg->syn)
    {
      seg->ack++;
    }
  }
  /*-------------------------------------------------------------------------
    Set remaining parts of packet
  -------------------------------------------------------------------------*/
  seg->urg = 0;
  seg->psh = 0;
  seg->rst = 1;
  seg->syn = 0;
  seg->fin = 0;
  seg->wnd       = 0;
  seg->up        = 0;
  seg->mss       = 0;
  seg->tsval          = 0;
  seg->sack_permitted = 0;
  seg->sack_block_cnt = 0;
  seg->optlen         = 0;
  if(tcp_hdr_create( &header_ptr, seg, pseudo_hdr) == FALSE)
  {
    if(header_ptr != NULL)
    {
      dsm_free_packet(&header_ptr);
      return;
    }
  }
  /*---------------------------------------------------------------------------
    Check to see if we ran out of items.
  ---------------------------------------------------------------------------*/
  if (header_ptr != NULL)
  {
    /*-----------------------------------------------------------------------
      Ship it out (note swap of addresses)
    -----------------------------------------------------------------------*/
    (void)ip_send(ip_cb,
                  pseudo_hdr->source,
                  (uint8)PS_IPPROTO_TCP,
                  header_ptr,
                  pseudo_hdr->length,
                  NULL,
                  &ps_errno);

    TCP_INC_STATS(rst_tx, 1) ;
    TCP_INC_STATS(segs_tx, 1) ;
  }
} /* tcp_send_reset() */

/*===========================================================================

FUNCTION TRIM()

DESCRIPTION
  This function will Trim the passed segment to fit the TCP window.

DEPENDENCIES
  None

RETURN VALUE
  0 if the trim yielded payload data bytes, else -1

SIDE EFFECTS
  None
===========================================================================*/

static sint15 trim
(
  register struct tcb *tcb,          /* ptr to TCP control block           */
  register struct tcp *seg,          /* ptr to TCP segment info            */
  struct dsm_item_s   **data_ptr,    /* address of ptr to TCP segment data */
  uint16              *length        /* ptr to length of TCP segment       */
)
{
  sint31 dupcnt;                   /* holds front end duplicate byte count */
  sint31 excess;                   /* holds excess from end of segment     */
  uint16 len;                      /* Segment length including flags       */
  uint8  accept = 0;               /* Accept/ No accept flag               */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  len = *length;
  if(seg->syn)
  {
    len++;
  }
  if(seg->fin)
  {
    len++;
  }
  /* Acceptability tests */
  if(tcb->rcv.wnd == 0)
  {
    /* Only in-order, zero-length segments are acceptable when
     * our window is closed.
     */
    if(seg->seq == tcb->rcv.nxt && len == 0)
    {
      return 0;  /* Acceptable, no trimming needed */
    }
  }
  else
  {
    /* Some part of the segment must be in the window */
    if( in_window( tcb, seg->seq))
    {
      accept++;  /* Beginning is */
    }
    else if( len != 0)
    {
      if( in_window( tcb, (uint32)(seg->seq+len-1)) ||     /* End is */
          tcp_seq_within( tcb->rcv.nxt,seg->seq, (uint32)(seg->seq+len-1)))
      { /* Straddles */
        accept++;
      }
    }
  }
  if(!accept)
  {
    LOG_MSG_INFO2_1( "Seg: %lx not in window", seg->seq);
    TCP_INC_STATS(dup_bytes_rx, len);
    TCP_INC_STATS(dup_segs_rx, 1);
    TCP_INC_INSTANCE_STATS(tcb,dup_bytes_rx, len);
    TCP_INC_INSTANCE_STATS(tcb,dup_segs_rx, 1);
    dsm_free_packet(data_ptr);
    return -1;
  }
  if((dupcnt = (sint31)(tcb->rcv.nxt - seg->seq)) > 0)
  {
    /* Trim off SYN if present */
    if(seg->syn)
    {
      /* SYN is before first data byte */
      seg->syn = 0;
      seg->seq++;
      dupcnt--;
    }
    if(dupcnt > 0)
    {
      TCP_INC_STATS(dup_bytes_rx, (uint32)dupcnt);
      TCP_INC_STATS(dup_segs_rx, 1);
      TCP_INC_INSTANCE_STATS(tcb,dup_bytes_rx, len);
      TCP_INC_INSTANCE_STATS(tcb,dup_segs_rx, 1);
      LOG_MSG_INFO2_1( "Trimming %d bytes from Seg front", dupcnt);
      (void)dsm_pullup(data_ptr,NULL,(uint16)dupcnt);
      seg->seq += dupcnt;
      *length  -= (uint16)dupcnt;
    }
  }
  if((excess = (sint31)(seg->seq + *length - (tcb->rcv.nxt+tcb->rcv.wnd))) > 0)
  {
    /* Trim right edge */
    LOG_MSG_INFO2_1( "Trimming %d bytes from Seg end", excess);
    *length       -= (uint16)excess;
    dsm_trim_packet(data_ptr,*length);
    seg->fin = 0;  /* FIN follows last data byte */
  }
  return 0;
} /* trim() */

/*===========================================================================

FUNCTION PROC_SYN()

DESCRIPTION
  This function will Process an incoming SYN


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void proc_syn
(
  register struct tcb *tcb,                /* pointer to TCP control block */
  uint8               tos,                 /* Type of service variable     */
  struct   tcp        *seg                 /* pointer to TCP header info   */
)
{
  uint16         mtu;           /*  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  tcb->flags.force = 1;  /* Always send a response */

  /* Note: It's not specified in RFC 793, but SND.WL1 and
   * SND.WND are initialized here since it's possible for the
   * window update routine in update() to fail depending on the
   * IRS if they are left unitialized.
   */
  /* Check incoming precedence and increase if higher */
  if(IP_PRECEDENCE(tos) > IP_PRECEDENCE(tcb->tos))
  {
    tcb->tos = tos;
  }
  tcb->rcv.nxt = seg->seq + 1;  /* p 68 */
  tcb->snd.wl1 = tcb->irs = seg->seq;
  tcb->snd.wnd = seg->wnd;
  if(seg->mss != 0)
  {
    /*----------------------------------------------------------
      Set the Max segment size (MSS) to no greater than DEF_MSS,
      as the memory pool large items are fitted to DEF_MSS
    ----------------------------------------------------------*/
    tcb->mss = MIN(seg->mss, tcb->mss);
  }

  if(seg->sack_permitted && tcb->options.tcp_sack_permitted)
  {
    tcb->flags.sack_permitted = TRUE;
  }

  if(seg->ts_present && tcb->options.tcp_ts_permitted)
  {
    tcb->flags.ts_permitted = TRUE;
    tcb->tsecr = seg->tsval;
  }

  if(seg->window_scale_enable)
  {
    tcb->snd.scale = seg->window_scale;
  }
  else
  {
    tcb->flags.window_scale_enabled = FALSE;
    tcb->rcv.scale = 0;
    tcb->rcv.wnd = MIN(tcb->rcv.wnd, TCP_MAX_WND_NO_SCALE);
  }

  /* Check the MTU of the interface we'll use to reach this guy
   * and lower the MSS so that unnecessary fragmentation won't occur
   */
  if (ps_iface_addr_family_is_v4(tcb->ipcb.routing_cache))
  {
    mtu = ps_ip_calc_path_mtu( IFACE_IPV4_ADDR_FAMILY, 
                               tcb->ipcb.routing_cache ) - IPLEN - TCPLEN;
  }
  else
  {
    mtu = ps_ip_calc_path_mtu( IFACE_IPV6_ADDR_FAMILY, 
                               tcb->ipcb.routing_cache ) - IP6LEN - TCPLEN;
  }

  if(tcb->flags.sack_permitted || tcb->flags.ts_permitted)
  {
    mtu -= TCP_MAXOPT;
  }
  tcb->mss = MIN(mtu,tcb->mss);
  tcb->cwind = MIN(2 * tcb->mss, PS_TCP_DEF_CWIND);
} /* proc_syn() */

/*===========================================================================

FUNCTION tcp_delete_sack_block_from_list()

DESCRIPTION
  This function will mark the first segment in the unacknowledged blocks
  list as lost if it hasn't been marked lost already.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Marks the first segment as lost.
===========================================================================*/
static void tcp_delete_sack_block_from_list
(
 struct tcp_sequence_block_list * curr_block_ptr,
 register struct tcb *tcb

)
{
  if(curr_block_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Function tcp_delete_sack_block_from_list: curr_block_ptr is NULL");
    return;
  }

  if(curr_block_ptr == tcb->sack_blocks_head_ptr)
  {
    tcb->sack_blocks_head_ptr = tcb->sack_blocks_head_ptr->next_ptr;
    if(tcb->sack_blocks_head_ptr != NULL)
    {
      tcb->sack_blocks_head_ptr->prev_ptr = NULL;
    }
  }
  else
  {
    curr_block_ptr->prev_ptr->next_ptr = curr_block_ptr->next_ptr;
    if(curr_block_ptr->next_ptr != NULL)
    {
      curr_block_ptr->next_ptr->prev_ptr = curr_block_ptr->prev_ptr;
    }
  }

  if(curr_block_ptr == tcb->sack_blocks_tail_ptr)
  {
    tcb->sack_blocks_tail_ptr = curr_block_ptr->prev_ptr;
  }

  PS_MEM_FREE(curr_block_ptr);
}
/*===========================================================================

FUNCTION tcp_insert_sack_block_into_list()

DESCRIPTION
  This function will mark the first segment in the unacknowledged blocks
  list as lost if it hasn't been marked lost already.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Marks the first segment as lost.
===========================================================================*/
static void tcp_insert_sack_block_into_list
(
 struct tcp_sequence_block_list * curr_block_ptr,
 tcp_sequence_block  * curr_sack_block,
 register struct tcb *tcb

)
{
  struct tcp_sequence_block_list * temp_block_ptr;

  temp_block_ptr =  (struct tcp_sequence_block_list *)
    ps_mem_get_buf(PS_MEM_TCP_SACK_BLOCKS_TYPE);

  if(temp_block_ptr == NULL)
  {
    LOG_MSG_ERROR_0( "Couldn't allocate a SACK block" );
    return;
  }

  temp_block_ptr->seq_block.start_seqno = curr_sack_block->start_seqno;
  temp_block_ptr->seq_block.end_seqno = curr_sack_block->end_seqno;

  if(tcp_seq_within(tcb->high_seq_rexmit,
                    curr_sack_block->start_seqno,
                    curr_sack_block->end_seqno))
  {
    tcb->high_seq_rexmit = curr_sack_block->end_seqno;
  }

  if(tcp_seq_within(tcb->high_seq_lost,
                    curr_sack_block->start_seqno,
                    curr_sack_block->end_seqno))
  {
    tcb->high_seq_lost = curr_sack_block->end_seqno;
  }

  /* Insert at the head */
  if(curr_block_ptr == NULL)
  {
    temp_block_ptr->prev_ptr = NULL;
    temp_block_ptr->next_ptr = tcb->sack_blocks_head_ptr;

    if(tcb->sack_blocks_head_ptr != NULL)
    {
      tcb->sack_blocks_head_ptr->prev_ptr = temp_block_ptr;
    }

    tcb->sack_blocks_head_ptr = temp_block_ptr;

    if(tcb->sack_blocks_tail_ptr == NULL)
    {
      tcb->sack_blocks_tail_ptr = temp_block_ptr;
    }
    return;
  }

  temp_block_ptr->next_ptr = curr_block_ptr->next_ptr;
  temp_block_ptr->prev_ptr = curr_block_ptr;
  curr_block_ptr->next_ptr = temp_block_ptr;

  if(temp_block_ptr->next_ptr != NULL)
  {
    temp_block_ptr->next_ptr->prev_ptr = temp_block_ptr;
  }
  if(tcb->sack_blocks_tail_ptr == curr_block_ptr)
  {
    tcb->sack_blocks_tail_ptr = temp_block_ptr;
  }
}
/*===========================================================================

FUNCTION TCP_PROCESS_SACK()

DESCRIPTION
  This function will update the sacked blocks list with the current
  cumulative and selective acknowledgement information.

  Doubly linked list of SACK blocks.


   ---------         ---------      ---------------         ---------
   |       |-------> |       |----->|              |------->|       |
   |  1    |<------- |  2    |<---- |      3       |<------ |   4   |
   ---------         ---------      ---------------         ---------
      ^                                   ^                    ^
      |                                   |                    |
    Head_ptr                           curr_block_ptr        Tail_ptr

    ack processing
  ----------------------->
                                                          sack processing
                                              <--------------------------


Case #                                  Sack Block            Action

     1                                  ------
                                        |    |                skip SACK
                                        ------                block

     2                            ---------------------
                                  |                   |       Merge
                                  |                   |
                                  ---------------------


                                  ------------
     3                            |          |                Merge
                                  |          |
                                  -----------

                                           ------------
                                           |          |       Merge
     4                                     |          |
                                           -----------


                   ------------
     5             |          |                               Advance
                   |          |                               curr_block_ptr
                   -----------



                                                        ------  Insert
     6                                                  |    |  SACK block
                                                        ------

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Update the SACK blocks based on SACK and cumulative ack information.
===========================================================================*/
static void tcp_process_sack
(
  register struct tcb *tcb,                /* pointer to TCP control block */
  register struct tcp *seg                 /* pointer to TCP header info   */
)
{
  struct tcp_sequence_block_list * curr_block_ptr;
  struct tcp_sequence_block_list * temp_block_ptr;
  tcp_sequence_block  * curr_sack_block;
  tcp_sequence_block    temp_sack_block;
  int  i,j,count ;
  uint32 sacked_bytes = 0;
  uint32 sacked_blocks = 0;
  uint32 lost_bytes = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(tcp_seq_lt(tcb->high_seq_rexmit, seg->ack))
  {
    if(tcp_seq_gt(seg->ack, tcb->high_seq_lost))
    {
      lost_bytes = tcb->high_seq_lost - tcb->high_seq_rexmit;
    }
    else
    {
      lost_bytes = seg->ack - tcb->high_seq_rexmit;
    }
  }
  /*-------------------------------------------------------------------------
    Process cumulative ack. Delete sack blocks that are completely acked
  -------------------------------------------------------------------------*/
  while(tcb->sack_blocks_head_ptr != NULL &&
  tcp_seq_ge(seg->ack,tcb->sack_blocks_head_ptr->seq_block.end_seqno))
  {
    curr_block_ptr = tcb->sack_blocks_head_ptr;

    /*---------------------------------------------------------------------
      Blocks between high_seq_lost and high_seq_rexmit are counted as
      lost bytes.
    ---------------------------------------------------------------------*/
    if(!(tcp_seq_ge(curr_block_ptr->seq_block.start_seqno,
                    tcb->high_seq_rexmit) &&
         tcp_seq_ge(tcb->high_seq_lost, curr_block_ptr->seq_block.end_seqno)))
    {
      sacked_bytes += (curr_block_ptr->seq_block.end_seqno -
                       curr_block_ptr->seq_block.start_seqno);
    }

    tcp_delete_sack_block_from_list(curr_block_ptr, tcb);
  } /* end of while */

  /*-----------------------------------------------------------------------
    shrink the starting seqno if the block is not completely acknowledged
    This shouldn't happen.
  -----------------------------------------------------------------------*/
  if(tcb->sack_blocks_head_ptr != NULL &&
     tcb->sack_blocks_head_ptr->seq_block.start_seqno < seg->ack)
  {
    if(!(tcp_seq_ge(tcb->sack_blocks_head_ptr->seq_block.start_seqno,
                    tcb->high_seq_rexmit) &&
         tcp_seq_ge(tcb->high_seq_lost,
                    tcb->sack_blocks_head_ptr->seq_block.end_seqno)))
    {
      sacked_bytes +=
        (seg->ack - tcb->sack_blocks_head_ptr->seq_block.start_seqno);
    }
    tcb->sack_blocks_head_ptr->seq_block.start_seqno = seg->ack;
  }


  /*-------------------------------------------------------------------------
    Sacked bytes and lost bytes are already decremented from number of
    bytes in transit. So we will increment the bytes in transmit with this
   and then subtract the whole acked bytes to get the correct bytes in transit.
   The acked bytes subtraction is done in UPDATE function.
  -------------------------------------------------------------------------*/
  tcb->numb_bytes_in_transit += (sacked_bytes + lost_bytes);

  if(seg->sack_block_cnt == 0)
  {
    return;
  }

  sacked_bytes = 0;
  lost_bytes = 0;
  /*-------------------------------------------------------------------------
    Sort the SACK block array. ( Bubble sort)
  -------------------------------------------------------------------------*/
  for (i=0; i < seg->sack_block_cnt - 1; i++)
  {
    for (j=0; (j< seg->sack_block_cnt - 1 - i) &&
              (j+1 < TCP_SACK_OPT_MAX_BLOCKS); j++)
      if (seg->sack_blocks[j+1].start_seqno > seg->sack_blocks[j].start_seqno)
      {
        temp_sack_block = seg->sack_blocks[j];
        seg->sack_blocks[j] = seg->sack_blocks[j+1];
        seg->sack_blocks[j+1] = temp_sack_block;
      }
  }

  /*-------------------------------------------------------------------------
    Process SACK
  -------------------------------------------------------------------------*/
  curr_block_ptr = tcb->sack_blocks_tail_ptr;
  for(count= 0; count < seg->sack_block_cnt; count++)
  {
    curr_sack_block = seg->sack_blocks + count;
    /*-------------------------------------------------------------------------
      Ignore the SACK block if peer's SACK sequence numbers dont fall within
      our send window.
    -------------------------------------------------------------------------*/
    if (!(tcp_seq_within(seg->sack_blocks[i].start_seqno,
                         tcb->snd.una,
                         tcb->snd.nxt) &&
          tcp_seq_within(seg->sack_blocks[i].end_seqno,
                         tcb->snd.una,
                         tcb->snd.nxt))
        )
    {
      LOG_MSG_INFO1_3("SACK block out-of-range, start=%u, end=%u, count=%d",
               seg->sack_blocks[i].start_seqno,
               seg->sack_blocks[i].end_seqno, count);
      continue;
    }

    while(curr_block_ptr != NULL)
    {

      /* case 6 */
      if(tcp_seq_gt(curr_sack_block->start_seqno,
                    curr_block_ptr->seq_block.end_seqno))
      {
        tcp_insert_sack_block_into_list(curr_block_ptr, curr_sack_block, tcb);
        if(curr_block_ptr->next_ptr != NULL)
        {
          curr_block_ptr = curr_block_ptr->next_ptr;
        }
        break;
      }

      /* case 1  subset or exact match*/
      if( tcp_seq_ge( curr_block_ptr->seq_block.end_seqno,
                      curr_sack_block->end_seqno) &&
          tcp_seq_ge( curr_sack_block->start_seqno,
                      curr_block_ptr->seq_block.start_seqno))
      {
        break;
      }

      /* case 2,3,4  overlap case*/
      if( tcp_seq_within( curr_block_ptr->seq_block.end_seqno,
        curr_sack_block->start_seqno,
        curr_sack_block->end_seqno) ||
        tcp_seq_within( curr_block_ptr->seq_block.start_seqno,
        curr_sack_block->start_seqno,
        curr_sack_block->end_seqno))

      {
        curr_sack_block->start_seqno =
          (tcp_seq_lt(curr_block_ptr->seq_block.start_seqno,
                curr_sack_block->start_seqno))
        ? curr_block_ptr->seq_block.start_seqno : curr_sack_block->start_seqno;

        curr_sack_block->end_seqno =
          (tcp_seq_gt(curr_block_ptr->seq_block.end_seqno,
                curr_sack_block->end_seqno))
          ? curr_block_ptr->seq_block.end_seqno : curr_sack_block->end_seqno;

        temp_block_ptr = curr_block_ptr;
        curr_block_ptr = curr_block_ptr->prev_ptr;
        tcp_delete_sack_block_from_list(temp_block_ptr, tcb);
        continue;
      }
      if(!(tcp_seq_ge(curr_block_ptr->seq_block.start_seqno,
                      tcb->high_seq_rexmit) &&
          tcp_seq_ge(tcb->high_seq_lost, curr_block_ptr->seq_block.end_seqno)))
      {
        sacked_bytes += curr_block_ptr->seq_block.end_seqno -
          curr_block_ptr->seq_block.start_seqno;
      }
      sacked_blocks ++;

      if((sacked_bytes >= (uint32)(TCPDUPACKS*tcb->mss) || sacked_blocks >= TCPDUPACKS)&&
        tcp_seq_lt( tcb->high_seq_lost, curr_block_ptr->seq_block.start_seqno))
      {
        tcb->high_seq_lost = curr_block_ptr->seq_block.start_seqno;
      }

      curr_block_ptr = curr_block_ptr->prev_ptr;
    }  /* end of while */

    if(curr_block_ptr == NULL &&
       tcp_seq_gt(curr_sack_block->start_seqno, seg->ack))
    {

      tcp_insert_sack_block_into_list(NULL, curr_sack_block, tcb);

      if(!(tcp_seq_ge(curr_sack_block->start_seqno,
                      tcb->high_seq_rexmit) &&
          tcp_seq_ge(tcb->high_seq_lost, curr_sack_block->end_seqno)))
      {
        sacked_bytes += curr_sack_block->end_seqno -
          curr_sack_block->start_seqno;
      }
      sacked_blocks ++;

      if((sacked_bytes >= (uint32)(TCPDUPACKS*tcb->mss) || sacked_blocks >= TCPDUPACKS)&&
        tcp_seq_lt( tcb->high_seq_lost, curr_sack_block->start_seqno))
      {
        tcb->high_seq_lost = curr_sack_block->start_seqno;
      }
    }
  } /* end of for */

 /* Calculate the sacked bytes in the remaining blocks.*/
  while(curr_block_ptr != NULL)
  {
    /* sacked bytes in between high seq rexmit and high_seq_lost are
       counted as lost bytes for updating Bytes in transit*/
    if(!(tcp_seq_ge(curr_block_ptr->seq_block.start_seqno,
                    tcb->high_seq_rexmit) &&
         tcp_seq_ge(tcb->high_seq_lost, curr_block_ptr->seq_block.end_seqno)))
    {
      sacked_bytes += curr_block_ptr->seq_block.end_seqno -
        curr_block_ptr->seq_block.start_seqno;
    }

    sacked_blocks ++;

    if((sacked_bytes >= (uint32)(TCPDUPACKS*tcb->mss) || sacked_blocks >= TCPDUPACKS)&&
       tcp_seq_lt( tcb->high_seq_lost, curr_block_ptr->seq_block.start_seqno))
    {
      tcb->high_seq_lost = curr_block_ptr->seq_block.start_seqno;
    }

    curr_block_ptr = curr_block_ptr->prev_ptr;
  }

  if(tcp_seq_gt(tcb->high_seq_lost, seg->ack))
  {
    if(tcp_seq_gt(seg->ack, tcb->high_seq_rexmit))
    {
      lost_bytes = tcb->high_seq_lost - seg->ack;
    }
    else
    {
      lost_bytes = tcb->high_seq_lost - tcb->high_seq_rexmit;
    }
  }

  tcb->numb_bytes_in_transit = (tcb->snd.nxt - tcb->snd.una) -
    (sacked_bytes + lost_bytes);
}
/*===========================================================================

FUNCTION UPDATE()

DESCRIPTION
  This function will process an incoming acknowledgement and window indication.
  See RFC 793 (TCP) page 72.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void update
(
  register struct tcb *tcb,                /* pointer to TCP control block */
  register struct tcp *seg,                /* pointer to TCP header info   */
  uint16              length               /* length of data for segment   */
)
{
  uint32 acked  = 0L;
  int    winupd = FALSE;
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
  uint32 rtt;                              /* Latest measured rtt value */
  uint32 abs_err;                          /* abs(rtt - srtt)           */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(tcp_seq_gt( seg->ack, tcb->snd.nxt))
  {
    LOG_MSG_INFO1_0( "ACKing Bogus Segment");
    tcb->flags.force = 1;  /* Acks something not yet sent */
    return;
  }

  /* Decide if we need to do a window update.
   * This is always checked whenever a legal ACK is received,
   * even if it doesn't actually acknowledge anything,
   * because it might be a spontaneous window reopening.
   */
  if( tcp_seq_gt( seg->seq,tcb->snd.wl1) || (( seg->seq == tcb->snd.wl1) &&
      tcp_seq_ge( seg->ack,tcb->snd.wl2)))
  {
    if(seg->wnd > tcb->snd.wnd)
    {
      winupd = 1;  /* Don't count as duplicate ack */

      /* If the window had been closed, crank back the send
       * pointer so we'll immediately resume transmission.
       * Otherwise we'd have to wait until the next probe.
       */
      if(tcb->snd.wnd == 0)
      {
        LOG_MSG_INFO2_0( "Send Window Zero");
        tcb->snd.ptr = tcb->snd.una;
      }
    }
    /* Remember for next time */
    tcb->snd.wnd = seg->wnd << tcb->snd.scale;
    tcb->snd.wl1 = seg->seq;
    tcb->snd.wl2 = seg->ack;
  }
  /* See if anything new is being acknowledged */
  if(tcp_seq_lt(seg->ack,tcb->snd.una))
  {
    LOG_MSG_INFO2_2("Old ACK Rx'd: %lx, una: %lx", seg->ack, tcb->snd.una);
    return; /* Old ack, ignore */
  }

  /*-------------------------------------------------------------------------
    Don't trigger fast retransmit if ACK to window probe is received acking
    nothing new.
  -------------------------------------------------------------------------*/
  if (seg->ack == tcb->snd.una && 0 < tcb->persist_backoff)
  {
    LOG_MSG_INFO2_0("Not triggering fast retransmit");
    return;
  }

  /* Process TCP selective acks*/
  if(tcb->flags.sack_permitted)
  {
    tcp_process_sack(tcb,seg);
  }

  if(seg->ack == tcb->snd.una)
  {
    /* Ack current, but doesn't ack anything */
    if( tcb->sndcnt == 0 || winupd || length != 0 || seg->syn || seg->fin)
    {
       tcb->backoff = 0;
       tcb->total_backoff_time = 0;

       LOG_MSG_INFO3_0("Duplicate ACK Rx'd (Good Reason)");
      /* Either we have nothing in the pipe, this segment
       * was sent to update the window, or it carries
       * data/syn/fin. In any of these cases we
       * wouldn't necessarily expect an ACK.
       */
      return;
    }

    LOG_MSG_INFO2_0("Duplicate ACK Rx'd (apparently BOGUS)");
    TCP_INC_STATS(dup_acks_rx, 1);
    TCP_INC_INSTANCE_STATS(tcb,dup_acks_rx, 1);

    /* Van Jacobson "fast recovery" code */
    if(++tcb->dupacks == TCPDUPACKS)
    {
      /* Knock the threshold down just as though
       * this were a timeout, since we've had
       * network congestion.
       */
      tcb->ssthresh = tcb->numb_bytes_in_transit / 2;
      tcb->ssthresh = MAX(tcb->ssthresh,tcb->mss);


      LOG_MSG_INFO2_0( "Retransmitting due to triple dupacks");
      if(!tcb->flags.sack_permitted && tcp_seq_lt(tcb->high_seq_lost,
                    (tcb->snd.una + MIN(tcb->sndcnt, tcb->mss))))
      {
        /*----------------------------------------------------------------------
          If Sack is permitted, then we should only send till high_seq_rexmit.
          If not then we should send upto one MSS bytes of data for retransmission.
          Need to look into this further...
        ---------------------------------------------------------------------*/
        tcb->high_seq_lost = tcb->snd.una + MIN(tcb->sndcnt, tcb->mss);

        if (MIN(tcb->sndcnt, tcb->mss) > tcb->numb_bytes_in_transit)
        {
          tcb->numb_bytes_in_transit = 0;
        }
        else
        {
          tcb->numb_bytes_in_transit -= MIN(tcb->sndcnt, tcb->mss);
        }
      }
      tcb->recovery_point = tcb->snd.nxt;
      tcb->flags.retran = 1;

      if(tcb->flags.sack_permitted)
      {
        tcb->cwind = tcb->ssthresh;
      }
      else
      {
        tcb->flags.force = 1;
        tcb->cwind = tcb->ssthresh + TCPDUPACKS*tcb->mss;
      }
    }
    else if(tcb->dupacks > TCPDUPACKS)
    {
      /* Continue to inflate the congestion window
       * until the acks finally get "unstuck".
       */
      if(!tcb->flags.sack_permitted)
      {
        tcb->cwind += tcb->mss;
      }
    }
    return;
  }

  if(tcb->dupacks >= TCPDUPACKS && tcb->cwind > tcb->ssthresh)
  {
    /* The acks have finally gotten "unstuck". So now we
     * can "deflate" the congestion window, i.e. take it
     * back down to where it would be after slow start
     * finishes.
     */
    tcb->cwind = tcb->ssthresh;
  }
  /* We're here, so the ACK must have actually acked something */

  LOG_MSG_INFO3_1( "Received ACK: %d bytes", seg->ack-tcb->snd.una);
  tcb->dupacks = 0;
  acked        = seg->ack - tcb->snd.una;

  /*-------------------------------------------------------------------------
    Check length to see whether the ack is piggybacked
  -------------------------------------------------------------------------*/
  if (length > 0 )
  {
    TCP_INC_STATS(piggybacked_acks_rx, 1) ;
    TCP_INC_INSTANCE_STATS(tcb,piggybacked_acks_rx, 1) ;
  }
  /* Expand congestion window if not already at limit and if
   * this packet wasn't retransmitted and is beyond recovery point
   * for SACK based loss recovery.
   */
  if(tcb->cwind < tcb->snd.wnd && !tcb->flags.retran &&
     !(tcb->flags.sack_permitted &&  seg->ack < tcb->recovery_point))
  {
    if(tcb->cwind < tcb->ssthresh)
    {
      /* Still doing slow start/CUTE, expand by amount acked */
      tcb->cwind += MIN(acked,tcb->mss);
    }
    else
    {
      /* Steady-state test of extra path capacity */
      tcb->cwind += (tcb->mss * tcb->mss) / tcb->cwind;
    }
    /* Don't expand beyond the offered window */
    if(tcb->cwind > tcb->snd.wnd)
    {
      tcb->cwind = tcb->snd.wnd;
    }
  }
  tcb->cwind = MAX(tcb->cwind,tcb->mss);

  /*----------------------------------------------------------------------
     Round trip time estimation Algorithm (RFC 2988)

     1. Calculating RTO
       a. Initial RTO <- 3s
       b. For first RTT mearurement rtt
            srtt <- rtt
            mdev <- rtt/2
       c. For each subsequent RTT measurement rtt
            mdev <- (1 - beta) * mdev + beta * |srtt - rtt |
            srtt <- (1 - alpha)* srtt + alpha * rtt
       d. RTO <- srtt + K * mdev, where K=4
       For part (b) rtt measurement can be made by using timestamp
       options as well.

     2. Managing rexmit timer
       a. After a packet send, if timer is not running, start it
          with the current RTO.
       b. When all outstanding packs are acked, stop rexmit timer.
       c. When ACK is received, start timer with newly calculated RTO.

     3. What to do when rexmit timer exprires
       a. Retransmit first unacked segment
       b. Back off RTO: RTO <- RTO*2
       c. Restart the rexmit timer with newly calculated RTO
  ----------------------------------------------------------------------*/
  if( tcb->flags.ts_permitted)
  {
    /* When timestamp option is enabled, use it to calculate rtt */
    rtt = (uint32)msclock() - seg->tsecr;

    if (TCP_SYN_SENT == tcb->state || TCP_SYN_RECEIVED == tcb->state)
    {
      tcb->srtt      = rtt;
      tcb->mdev      = rtt/2;
    }
    else
    {
      abs_err = (rtt > tcb->srtt) ? rtt - tcb->srtt : tcb->srtt - rtt;
      tcb->mdev = ((DGAIN-1)*tcb->mdev + abs_err + (DGAIN/2)) >> LDGAIN;
      tcb->srtt = ((AGAIN-1)*tcb->srtt + rtt + (AGAIN/2)) >> LAGAIN;
      LOG_MSG_INFO2_3("RTT Trace: rtt = %u, tcb->mdev = %u, tcb->srtt = %u",
                rtt, tcb->mdev, tcb->srtt);
    }
  }
  else
  {
    /*
     * rtt_run is set when we are timing a segment.
     * That segment' sequence number is stored in rttseq
     */
    if(tcb->flags.rtt_run && tcp_seq_ge(seg->ack,tcb->rttseq))
    {
      /* A timed sequence number has been acked */
      tcb->flags.rtt_run = 0;
      if(!(tcb->flags.retran))
      {
        /*--------------------------------------------------------
          This packet was sent only once and now it's been acked,
          so process the round trip time. rtt_time stores the
          absolute clock values when the segment was sent.
        --------------------------------------------------------*/
        rtt = (uint32)msclock() - tcb->rtt_time;

        if (TCP_SYN_SENT == tcb->state || TCP_SYN_RECEIVED == tcb->state)
        {
          tcb->srtt      = rtt;
          tcb->mdev      = rtt/2;
        }
        else
        {
          abs_err  = (rtt > tcb->srtt) ? rtt - tcb->srtt : tcb->srtt - rtt;

          /* Run SRTT and MDEV integrators, with rounding */

          tcb->srtt = ((AGAIN-1)*tcb->srtt + rtt + (AGAIN/2)) >> LAGAIN;
          tcb->mdev = ((DGAIN-1)*tcb->mdev + abs_err + (DGAIN/2)) >> LDGAIN;
          LOG_MSG_INFO2_3("RTT Trace: rtt = %u, tcb->mdev = %u, tcb->srtt = %u",
                    rtt, tcb->mdev, tcb->srtt);
        }

        /* Reset the backoff level */
        tcb->backoff = 0;
        tcb->total_backoff_time = 0;
      }
    }
  }

  tcb->sndcnt  -= acked;  /* Update virtual byte count on snd queue */
  tcb->snd.una  = seg->ack;
  tcb->high_seq_lost = MAX(tcb->snd.una, tcb->high_seq_lost);
  tcb->high_seq_rexmit = MAX(tcb->snd.una, tcb->high_seq_rexmit);
  if(acked > tcb->numb_bytes_in_transit)
  {
    tcb->numb_bytes_in_transit = 0;
  }
  else
  {
    tcb->numb_bytes_in_transit -= acked;
  }

  /* If we're waiting for an ack of our SYN, note it and adjust count */
  if(!(tcb->flags.synack))
  {
    tcb->flags.synack = 1;
    acked--;  /* One less byte to pull from real snd queue */
  }
  /* Remove acknowledged bytes from the send queue and update the
   * unacknowledged pointer. If a FIN is being acked,
   * pullup won't be able to remove it from the queue, but that
   * causes no harm.
   */
  if(acked > 0)
  {
    FreeBytes(tcb->reader, acked);
  }

  /* If there are outstanding segments, start the retransmission
   * timer. Otherwise, there is no data in flight and we cancel
   * the timer.
   */

  if(tcb->snd.una != tcb->snd.nxt)
  {
    /* There are segments in flight. Start the retransmission timer.*/
    timer_ret_val = ps_timer_start( tcb->timer,
                                    tcb->tcp_rexmit_timer_val
                                  );
    if( PS_TIMER_SUCCESS != timer_ret_val )
    {
      LOG_MSG_ERROR_1("Cant start timer 0x%x", tcb->timer);
      ASSERT(0);
    }
  }
  else
  {
    timer_ret_val = ps_timer_cancel( tcb->timer);
    if( PS_TIMER_SUCCESS != timer_ret_val )
    {
      LOG_MSG_ERROR_1("Cant cancel timer 0x%x", tcb->timer);
      ASSERT(0);
    }
  }


  /* If retransmissions have been occurring, make sure the
   * send pointer doesn't repeat ancient history
   */
  if(tcp_seq_lt(tcb->snd.ptr,tcb->snd.una))
  {
   tcb->snd.ptr = tcb->snd.una;
  }

  /* Clear the retransmission flag since the oldest
   * unacknowledged segment (the only one that is ever retransmitted)
   * has now been acked.
   */
  tcb->flags.retran = 0;

} /* update() */

/*===========================================================================

FUNCTION TCP_PROCESS_FIN()

DESCRIPTION
  This function processes the incoming FIN.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void tcp_process_fin
(
  struct tcb * tcb
)
{
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */

 /* process FIN bit (p 75) */
  tcb->flags.force = 1;  /* Always respond with an ACK */

  switch(tcb->state)
  {
    case TCP_SYN_RECEIVED:
    case TCP_ESTABLISHED:
      tcb->rcv.nxt++;
      tcp_setstate(tcb,TCP_CLOSE_WAIT);
      break;

    case TCP_FINWAIT1:
      tcb->rcv.nxt++;
      if(tcb->sndcnt == 0)
      {
        /* Our FIN has been acked; bypass TCP_CLOSING state */
        tcp_setstate(tcb,TCP_TIME_WAIT);
        tcb->tcp_rexmit_timer_val = MSL2*1000L;

        timer_ret_val = ps_timer_start( tcb->timer,
                                        tcb->tcp_rexmit_timer_val
                                        );
        if( PS_TIMER_SUCCESS != timer_ret_val )
        {
          LOG_MSG_ERROR_1("tcp_process_fin: Cant start timer 0x%x"
		                       , tcb->timer);
          ASSERT(0);
        }
      }
      else
      {
        tcp_setstate(tcb,TCP_CLOSING);
      }
      break;

    case TCP_FINWAIT2:
      tcb->rcv.nxt++;
      tcp_setstate(tcb,TCP_TIME_WAIT);
      tcb->tcp_rexmit_timer_val = MSL2*1000L;
      timer_ret_val = ps_timer_start( tcb->timer,
                                      tcb->tcp_rexmit_timer_val
                                      );
      if( PS_TIMER_SUCCESS != timer_ret_val )
      {
        LOG_MSG_ERROR_1("tcp_process_fin: Cant start timer 0x%x"
		                         , tcb->timer);
        ASSERT(0);
      }
      break;

    case TCP_CLOSE_WAIT:
    case TCP_CLOSING:
    case TCP_LAST_ACK:
      break;    /* Ignore */

    case TCP_TIME_WAIT:  /* p 76 */
      timer_ret_val = ps_timer_start( tcb->timer,
                                      tcb->tcp_rexmit_timer_val
                                      );
      if( PS_TIMER_SUCCESS != timer_ret_val )
      {
        LOG_MSG_ERROR_1("tcp_process_fin: Cant start timer 0x%x"
				        , tcb->timer);
        ASSERT(0);
      }
      break;
    case TCP_CLOSED:
    case TCP_LISTEN:
    case TCP_SYN_SENT:
      break;
  } /* switch( state) */

}


/*===========================================================================

FUNCTION TCP_ENQUEUE_DATA()

DESCRIPTION
  This function enqueues the received data either onto the tcp receive queue or
  onto the re-sequencing queue.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean tcp_enqueue_data
(
  struct tcb * tcb_ptr,
  dsm_item_type * tcp_seg_ptr,
  struct tcp * seg,
  uint16 length
)
{
  tcp_reseq_hdr   reseq_hdr;                   /* TCP reseq hdr            */
  dsm_item_type        * reseq_data_ptr;       /* data from reseq          */
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
  int16              ps_errno;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Trim segment to fit receive window. */
  if( trim( tcb_ptr, seg, &tcp_seg_ptr, &length) == -1)
  {
    /* Segment is unacceptable */
    LOG_MSG_INFO1_2( "Toss Seg, seq: %lx, nxt: %lx",
                     seg->seq, tcb_ptr->rcv.nxt);
    /* In SYN_RECEIVED state, answer a retransmitted SYN
     * with a retransmitted SYN/ACK.
     */
    if(tcb_ptr->state == TCP_SYN_RECEIVED)
    {
      tcb_ptr->snd.ptr = tcb_ptr->snd.una;
    }
    TCP_INC_STATS(dup_acks_tx, 1);
    TCP_INC_INSTANCE_STATS(tcb_ptr,dup_acks_tx, 1);
    tcb_ptr->flags.force = 1;
    tcp_output(tcb_ptr);
    return FALSE;
  }

  if(seg->fin)
  {
    tcb_ptr->flags.fin_recvd = TRUE;
    tcb_ptr->fin_seqno = seg->seq+length;
    tcb_ptr->flags.force = 1;
  }

  /* If segment isn't the next one expected, and there's data
   * or flags associated with it, put it on the resequencing
   * queue, ACK it and return.
   */

  if( seg->seq != tcb_ptr->rcv.nxt && length != 0)
  {
    TCP_INC_STATS(out_of_order_segs_rx, 1);
    TCP_INC_STATS(out_of_order_bytes_rx, length);
    TCP_INC_INSTANCE_STATS(tcb_ptr,out_of_order_segs_rx, 1);
    TCP_INC_INSTANCE_STATS(tcb_ptr,out_of_order_bytes_rx, length);
    /*-----------------------------------------------------------------------
      If we are running out of items, then stop queueing new TCP segments to
      the resequencing queue.
    -----------------------------------------------------------------------*/
#define PS_TRANSPORT_CHECK_DSM_AVAIL(item)               \
  ((DSM_ITEM_POOL((item)) == DSM_DUP_ITEM_POOL)?         \
    DSM_LESS_THAN_TRANSPORT_LAYER_FEW_FREE_ITEMS(        \
		    DSM_ITEM_POOL((item)->dup_ptr)):     \
    DSM_LESS_THAN_TRANSPORT_LAYER_FEW_FREE_ITEMS(        \
		    DSM_ITEM_POOL((item))))

    if(PS_TRANSPORT_CHECK_DSM_AVAIL(tcp_seg_ptr))
    {
      LOG_MSG_INFO1_3("Dropping pkt for sock%d, no memory in 0x%p item pool,"
                      " seq 0x%x",
                      tcb_ptr->sockfd, DSM_ITEM_POOL(tcp_seg_ptr), seg->seq);
      dsm_free_packet( &tcp_seg_ptr);
    }
    else
    {
      add_reseq(tcb_ptr,seg, tcp_seg_ptr,length);
    }
    /*-----------------------------------------------------------------------
      tcp_output sends a duplicate ack in this case
    -----------------------------------------------------------------------*/
    TCP_INC_STATS(dup_acks_tx, 1) ;
    TCP_INC_INSTANCE_STATS(tcb_ptr,dup_acks_tx, 1) ;
    tcb_ptr->flags.force = 1;
    tcp_output(tcb_ptr);
    return FALSE;
  }

  if(length > 0)
  {
    /* Place on receive queue */

    DSM_TOUCH_ITEM(tcp_seg_ptr);
    
    (void) Write(tcb_ptr->writer, &tcp_seg_ptr, length, NULL, 0, &ps_errno);
    tcb_ptr->rcv.nxt += length;    

    TCP_INC_STATS(bytes_rx, length);
    TCP_INC_INSTANCE_STATS(tcb_ptr,bytes_rx, length);

    LOG_MSG_INFO3_2("rcv.nxt: %lx, rcv.wnd: %lx",
                    tcb_ptr->rcv.nxt, tcb_ptr->rcv.wnd);

    memset(&reseq_hdr, 0, sizeof(reseq_hdr));

    /* process out of order segments */
    get_reseq( tcb_ptr, &reseq_hdr, &reseq_data_ptr);
    if( reseq_data_ptr != NULL)
    {      
      (void) Write(tcb_ptr->writer,
                   &reseq_data_ptr,
                   reseq_hdr.length,
                   NULL,
                   0,
                   &ps_errno);
      tcb_ptr->rcv.nxt += reseq_hdr.length;      
    }

    /* Decide whether to delay Ack.
     * If previous Ack has been delayed,
     * Ack now (cuts number of Acks in half).
     * Otherwise, wait for half send RTT,
     * which shouldn't hurt deviation too badly.
     * Hopefully, the user upcall will result
     * in more data to send.
     */
    if(!tcb_ptr->options.tcp_delayed_ack)
    {
      tcb_ptr->flags.force = TRUE;
    }
    else
    {
      if ( ps_timer_is_running(tcb_ptr->acker) )
      {
        LOG_MSG_INFO2_0( "TCP Delayed Ack Timer Running");
        tcb_ptr->flags.force = TRUE;
      }
      else
      {
        uint32 ato = tcb_ptr->srtt / 2 + tcb_ptr->mdev;
        ato        = MAX( ato, MIN_ATO );
        ato        = MIN( ato, MAX_ATO );
        LOG_MSG_INFO3_0("Starting delayed ack timer");
        timer_ret_val = ps_timer_start( tcb_ptr->acker,ato);

        if( PS_TIMER_SUCCESS != timer_ret_val )
        {
          LOG_MSG_ERROR_1("Cant start timer 0x%x", tcb_ptr->acker);
          ASSERT(0);
        }
      }
    }
  } /* length > 0 */

  /* process the FIN bit */
  if(tcb_ptr->flags.fin_recvd && tcb_ptr->rcv.nxt == tcb_ptr->fin_seqno)
  {
    tcp_process_fin(tcb_ptr);
  }

  return TRUE;
}


/*===========================================================================
FUNCTION DSSTCP_LOOKUP_CONN()

DESCRIPTION
  Given a connection structure, looks up in the tcb to see if this is a
  valid connection.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to TCB if valid connection.
  If not, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
struct tcb *dsstcp_lookup_conn
(
  struct connection *conn                    /* connection to be looked up */
)
{
  struct tcb *tcb_ptr;                       /* ptr to TCP control block   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Look up the connection and look at IP address and port numbers for
    both source and destination.
  -------------------------------------------------------------------------*/
  tcb_ptr = (struct tcb *) q_check(&sock_config_cb.tcp_cb_q);
  while (NULL != tcb_ptr)
    {
    ASSERT(SOCKAVAIL != tcb_ptr->sockfd);

      if (conn->remote.port      == tcb_ptr->conn.remote.port
         && conn->local.port     == tcb_ptr->conn.local.port
         && PS_IN6_ARE_ADDR_EQUAL(&conn->remote.address,
                               &tcb_ptr->conn.remote.address)
         && PS_IN6_ARE_ADDR_EQUAL(&conn->local.address,
                               &tcb_ptr->conn.local.address))
      {
      ASSERT(TCP_CLOSED != tcb_ptr->state);
        return( tcb_ptr);
      }

    tcb_ptr
      = (struct tcb *) q_next(&sock_config_cb.tcp_cb_q,
                              (q_link_type *) tcb_ptr);
  } /* while */

  /*-------------------------------------------------------------------------
      Connection not found.
  -------------------------------------------------------------------------*/
  return( NULL);

} /* dsstcp_lookup_conn() */


/*===========================================================================
FUNCTION DSSTCP_LOOKUP_SERV_AF()

DESCRIPTION
  Given a connection structure, looks up in the tcb to see if this is a
  valid listener.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to TCB if valid connection.
  If not, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
struct tcb *dsstcp_lookup_serv_af
(
  struct connection *conn,                   /* connection to be looked up */
  uint16             family                  /* IP Address Family */
)
{
  struct tcb *listen_tcb_ptr;                      /* ptr to TCP control block   */
  struct tcb *tcb_ptr = NULL;                /* ptr to TCP control block   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Look up the connection and look at IP address and port numbers for
    both source and destination.
  -------------------------------------------------------------------------*/
  listen_tcb_ptr = (struct tcb *) q_check(&sock_config_cb.tcp_cb_q);
  while (NULL != listen_tcb_ptr)
  {
    /*-----------------------------------------------------------------------
      TCB's state can be TCP_CLOSED when spurious SYN packets are received,
      say after bind() but before listen(). So TCB's state MUST NOT be
      asserted against TCP_CLOSED.
    -----------------------------------------------------------------------*/
    if (TCP_CLOSED != listen_tcb_ptr->state)
    {
      ASSERT(SOCKAVAIL != listen_tcb_ptr->sockfd);

      /*---------------------------------------------------------------------
        Look up if there is any connected TCP that matches the 4 tuple
      ---------------------------------------------------------------------*/
      if (conn->remote.port == listen_tcb_ptr->conn.remote.port &&
          conn->local.port == listen_tcb_ptr->conn.local.port &&
          PS_IN6_ARE_ADDR_EQUAL(&conn->remote.address,
                             &listen_tcb_ptr->conn.remote.address) &&
          PS_IN6_ARE_ADDR_EQUAL(&conn->local.address,
                             &listen_tcb_ptr->conn.local.address))
      {
        return( listen_tcb_ptr);
      }

      LOG_MSG_INFO1_4("dsstcp_lookup_serv_af() Matching family %d vs %d, local port %d vs %d", 
                      family, listen_tcb_ptr->family, conn->local.port, listen_tcb_ptr->conn.local.port);

      /*---------------------------------------------------------------------
        Look up if they are an listening TCB's on the specified port/Ipaddr
      ---------------------------------------------------------------------*/
      if (tcb_ptr == NULL &&
          (family == ds_AddrFamily_QDS_AF_UNSPEC || listen_tcb_ptr->family == family) &&
          listen_tcb_ptr->conn.remote.port == 0 &&
          conn->local.port == listen_tcb_ptr->conn.local.port &&
          PS_IN6_ARE_ADDR_EQUAL(&listen_tcb_ptr->conn.remote.address,
                             &ps_in6addr_any) &&
          ( PS_IN6_ARE_ADDR_EQUAL(&listen_tcb_ptr->conn.local.address,
                               &ps_in6addr_any) ||
            PS_IN6_ARE_ADDR_EQUAL(&conn->local.address,
                               &listen_tcb_ptr->conn.local.address)) )
      {
        LOG_MSG_INFO1_0("dsstcp_lookup_serv() Found listening TCB");
        tcb_ptr = listen_tcb_ptr;
      }
    }

    listen_tcb_ptr = (struct tcb *) q_next(&sock_config_cb.tcp_cb_q,
                                           (q_link_type *) listen_tcb_ptr);
  } /* while */

  /*-------------------------------------------------------------------------
    Connection not found.
  -------------------------------------------------------------------------*/
  return tcb_ptr;

} /* dsstcp_lookup_serv() */

/*===========================================================================
FUNCTION DSSTCP_LOOKUP_SERV()

DESCRIPTION
  This is to maintain compatibility for clients who don't specify the 
  address family.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to TCB if valid connection.
  If not, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
struct tcb *dsstcp_lookup_serv
(
  struct connection *conn                   /* connection to be looked up */
)
{
  return dsstcp_lookup_serv_af(conn, ds_AddrFamily_QDS_AF_UNSPEC);
}


/*===========================================================================

FUNCTION TCP_INPUT()

DESCRIPTION
  This function performs TCP input processing. This function is called from
  IP with the IP header in machine byte order, along with a mbuf chain
  pointing to the TCP header.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tcp_input
(
  ip_pkt_info_type*        ip_pkt_info_ptr,   /* IP Packet Info            */
  struct dsm_item_s        *tcp_seg_ptr,      /* Data field, if any        */
  uint16                   offset             /* Offset of the TCP packet  */
)
{
  register struct tcb  *tcb = NULL;      /* TCP Protocol control block     */
  struct   tcp         seg;              /* Local copy of segment header   */
  uint16               hdrlen = 0;       /* Length of TCP header           */
  uint16               length;           /* length of data portion of seg. */
  struct connection    conn;             /* temp connection variable       */
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
  ip_cb_type           ip_cb;         /* IP control block                  */
  uint32               iface_index;
  errno_enum_type      retval;
  uint32               current_connections;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(tcp_seg_ptr == NULL)
  {
    return;
  }
  TCP_INC_STATS(segs_rx,1);
  if(ip_pkt_info_ptr->is_brdcst)
  {
    /* Any TCP packet arriving as a broadcast is
     * to be completely IGNORED!!
     */
    TCP_INC_STATS(dropped_segs_rx,1);
    dsm_free_packet(&tcp_seg_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
   * Calculate the checksum.
   *-----------------------------------------------------------------------*/
  length = ip_pkt_info_ptr->pseudo_hdr.length;


  if(cksum(&ip_pkt_info_ptr->pseudo_hdr,tcp_seg_ptr,length,offset) != 0)
  {
    /* Checksum failed, ignore segment completely */
    (void)tcp_hdr_parse(tcp_seg_ptr,&offset,&seg);
    TCP_INC_STATS(bad_chksum,1);
    TCP_INC_STATS(dropped_segs_rx,1);
    LOG_MSG_INFO1_1( "TCP Checksum Err, Seq: %lx", seg.seq);
    dsm_free_packet(&tcp_seg_ptr);
    return;
  }

  /*------------------------------------------------------------------------
   * Remove the ip header from the data_ptr.
   *-----------------------------------------------------------------------*/
  (void)dsm_pullup(&tcp_seg_ptr, NULL, offset);

  if(length > dsm_length_packet( tcp_seg_ptr))
  /* Sanity check to match IP header length to actual payload length */
  {
    LOG_MSG_ERROR_0("Actual length of segment doesnt match  header len");
     /* Senders TCP is sending bogus segments */
    TCP_INC_STATS(bad_hdr_len,1);
    TCP_INC_STATS(dropped_segs_rx,1);
    dsm_free_packet(&tcp_seg_ptr);
    return;
  }
  else if ( length < dsm_length_packet(tcp_seg_ptr))
  {
    dsm_trim_packet( &tcp_seg_ptr, length);
  }

  /*-------------------------------------------------------------------------
    Parse the TCP header.
  -------------------------------------------------------------------------*/
  if(tcp_hdr_parse(tcp_seg_ptr,&hdrlen,&seg) == FALSE)
  {
    /* TCP header is too small */
    TCP_INC_STATS(too_short,1);
    TCP_INC_STATS(dropped_segs_rx,1);
    LOG_MSG_INFO1_0( "Bad TCP Header, Ignoring Segment");
    dsm_free_packet(&tcp_seg_ptr);
    return;
  }
  /*-------------------------------------------------------------------------
    Remove the TCP header from the DSM item.
  -------------------------------------------------------------------------*/
  (void)dsm_pullup(&tcp_seg_ptr, NULL, hdrlen);
  length -= hdrlen;

  LOG_MSG_INFO3_2("TCP In: seq = %x, ack = %x", seg.seq, seg.ack);

  conn.local.address  = ip_pkt_info_ptr->pseudo_hdr.dest;
  conn.local.port     = seg.dest;
  conn.remote.address = ip_pkt_info_ptr->pseudo_hdr.source;
  conn.remote.port    = seg.source;

  memset(&ip_cb, 0, sizeof(ip_cb));
  ip_cb.routing_cache = (ps_iface_type*)ip_pkt_info_ptr->if_ptr;
  ip_cb.qos           = ip_pkt_info_ptr->ip_hdr.v4.tos;

  /*-------------------------------------------------------------------------
    Lookup the TCP control block for this received TCP packet.
    During async/fax calls, a special tcb is used. In sockets calls, the tcb
    from dsstcp.c is used.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707_ASYNC
  if( ps707_async_in_rawdata_mode() == TRUE)
  {
    tcb = (struct tcb *)ps_tcp_async_get_tcb_handle();
  }
  else
#endif /* FEATURE_DATA_IS707_ASYNC */
  {
    if(!seg.syn || seg.ack_flag || seg.rst)
    {
      tcb = dsstcp_lookup_conn( &conn );
    }
    else
    {
      retval = ps_stat_get_tcp(PS_STAT_TCP_CRNT_OPEN_CONNS,
                               NULL,
                               (void*)&current_connections,
                               sizeof(current_connections));
      if (retval != E_SUCCESS)
      {
        LOG_MSG_ERROR_0("Could not get current TCP connections");
      }
      else if(current_connections <= (DSS_MAX_TCBS - TCP_LOW_THRESHOLD))
      {
        if( ip_pkt_info_ptr->ip_vsn == IP_V4 )
        {
          tcb = dsstcp_lookup_serv_af( &conn, ds_AddrFamily_QDS_AF_INET );
        }
        else if( ip_pkt_info_ptr->ip_vsn == IP_V6 )
        {
          tcb = dsstcp_lookup_serv_af( &conn, ds_AddrFamily_QDS_AF_INET6 );
        }
        else
        {
          LOG_MSG_ERROR_1( "Invalid IP version in ip_pkt_info %d", 
                           ip_pkt_info_ptr->ip_vsn );
          dsm_free_packet(&tcp_seg_ptr);
          return;
        }

        /* Print connection parameters */
        LOG_MSG_INFO1_2( "tcp_input() looked up server: local port %d remote port %d",
                         ps_ntohs(conn.local.port), ps_ntohs(conn.remote.port) );
        IPV6_ADDR_MSG(conn.local.address.ps_s6_addr64);
        IPV6_ADDR_MSG(conn.remote.address.ps_s6_addr64);
      }
      else
      {
        LOG_MSG_ERROR_1("Current TCP connection: %d exceeds limit. Reject",
                        current_connections);
      }
    }
  }

  /*-------------------------------------------------------------------------
    If a valid TCP control block could not be found for this packet, send a
    reset to the peer and return.
  -------------------------------------------------------------------------*/
  if( tcb == NULL)
  {
    TCP_INC_STATS(dropped_segs_noconn_rx,1);
    TCP_INC_STATS(dropped_segs_rx,1);
    dsm_free_packet(&tcp_seg_ptr);
    LOG_MSG_INFO1_0("Received segment for invalid connection");
    tcp_send_reset(&seg,&ip_pkt_info_ptr->pseudo_hdr,&ip_cb);
    return;
  }
  TCP_INC_INSTANCE_STATS(tcb,segs_rx,1);

  /*-------------------------------------------------------------------------
    Enforce netpolicy on input packet. If not loopback. Use routing scope
    from incoming ip pkt info and not tcb->ipcb to do loopback
  -------------------------------------------------------------------------*/
  iface_index =
    PS_IFACE_GET_IFACE_INDEX((ps_iface_type *)ip_pkt_info_ptr->if_ptr);

  if( iface_index == PS_IFACE_INVALID_IFACE_INDEX ||
      !ps_route_scope_is_iface_idx_set( &tcb->ipcb.route_scope, iface_index ))
  {
    TCP_INC_STATS(dropped_segs_rx,1);
    LOG_MSG_INFO1_2("Rx interface 0x%x violates netpolicy, sockfd %d",
                    ip_pkt_info_ptr->if_ptr, tcb->sockfd);
    dsm_free_packet( &tcp_seg_ptr);
    tcp_send_reset(&seg,&ip_pkt_info_ptr->pseudo_hdr,&ip_cb);
    return;
  }

  /* Do unsynchronized-state processing (p. 65-68) */

  switch(tcb->state)
  {
  case TCP_CLOSED:
    TCP_INC_STATS(dropped_segs_noconn_rx,1);
    TCP_INC_STATS(dropped_segs_rx,1);
    dsm_free_packet(&tcp_seg_ptr);

    /*-----------------------------------------------------------------------
      Verify routing cache in TCB before attempting to send rst using TCB
    -----------------------------------------------------------------------*/
    if( !PS_IFACE_IS_VALID(tcb->ipcb.routing_cache) )
    {
      tcp_send_reset(&seg,&ip_pkt_info_ptr->pseudo_hdr,&ip_cb);
    }
    else
    {
      tcp_send_reset(&seg,&ip_pkt_info_ptr->pseudo_hdr,&tcb->ipcb);
    }
    return;

  case TCP_LISTEN:
    if(seg.rst)
    {
      TCP_INC_STATS(rst_rx,1);
      TCP_INC_STATS(dropped_segs_rx,1);
      LOG_MSG_INFO1_0("Received RESET message on a listening socket");
      dsm_free_packet(&tcp_seg_ptr);
      return;
    }
    if(seg.ack_flag)
    {
      TCP_INC_STATS(dropped_segs_rx,1);
      dsm_free_packet(&tcp_seg_ptr);
      tcp_send_reset(&seg,&ip_pkt_info_ptr->pseudo_hdr,&tcb->ipcb);
      return;
    }
    if(seg.syn)
    {
      /* Put all the socket info into the TCB */
      tcb->conn  = conn;

      /* Set the routing cache to the interface we received this on */
      /* unless we bound to a specific interface.                   */
      ps_ip_update_routing_cache_in_ipcb
      (
        &(tcb->ipcb),
        (ps_iface_type*)ip_pkt_info_ptr->if_ptr
      );
      #ifdef FEATURE_SEC_IPSEC
      ps_ip_update_ipsec_info_in_ipcb(&(tcb->ipcb), ip_pkt_info_ptr);
      #endif /* FEATURE_SEC_IPSEC */

      /* Shrink the Route Scope to the single interface the SYN came on */
      (void)ps_route_scope_by_iface_handle(ip_pkt_info_ptr->if_ptr,
                                           &tcb->ipcb.route_scope);

      /* (Security check is bypassed) */
      /* page 66 */
      proc_syn(tcb,(uint8)ip_pkt_info_ptr->ip_hdr.v4.tos,&seg);
      tcp_send_syn(tcb);
      tcp_setstate(tcb,TCP_SYN_RECEIVED);
      tcp_output(tcb);
    }
    else
    {
      /* If this segment doesn't carry a SYN, reject it */
      TCP_INC_STATS(dropped_segs_rx,1);
      LOG_MSG_INFO2_0( "Missing SYN, Listen State");
      tcp_send_reset(&seg,&ip_pkt_info_ptr->pseudo_hdr,&tcb->ipcb);
    }

    dsm_free_packet(&tcp_seg_ptr);
    return;

  case TCP_SYN_SENT:
    if(seg.ack_flag)
    {
      if(!tcp_seq_within(seg.ack,tcb->iss+1,tcb->snd.nxt))
      {
        TCP_INC_STATS(dropped_segs_rx,1);
        dsm_free_packet(&tcp_seg_ptr);
        tcp_send_reset(&seg,&ip_pkt_info_ptr->pseudo_hdr,&tcb->ipcb);
        return;
      }
    }
    if(seg.rst)
    {  /* p 67 */
      TCP_INC_STATS(rst_rx,1);
      LOG_MSG_INFO1_0("Received RESET message from peer");
      if(seg.ack_flag)
      {
        /* The ack must be acceptable since we just checked it.
         * This is how the remote side refuses connect requests.
         */
        tcp_close_self(tcb, PS_PCB_CLOSE_REASON_CODE_RESET);
      }
      dsm_free_packet(&tcp_seg_ptr);
      return;
    }
    /* (Security check skipped here) */
    if(seg.syn)
    {
      proc_syn(tcb,(uint8)ip_pkt_info_ptr->ip_hdr.v4.tos,&seg);
      if(seg.ack_flag)
      {
        /* Our SYN has been acked, otherwise the ACK
         * wouldn't have been valid.
         */
        update(tcb,&seg,length);
        tcp_setstate(tcb,TCP_ESTABLISHED);
      } else
      {
        tcp_setstate(tcb,TCP_SYN_RECEIVED);
      }

      if(length != 0 || seg.fin)
      {
        break;    /* Continue processing if there's more */
      }
      tcp_output(tcb);
    } else
    {
      TCP_INC_STATS(dropped_segs_rx,1);
      dsm_free_packet(&tcp_seg_ptr);  /* Ignore if neither SYN or RST is set */
    }
    dsm_free_packet(&tcp_seg_ptr);
    return;
  case TCP_SYN_RECEIVED:
  case TCP_ESTABLISHED:
  case TCP_FINWAIT1:
  case TCP_FINWAIT2:
  case TCP_CLOSE_WAIT:
  case TCP_LAST_ACK:
  case TCP_CLOSING:
  case TCP_TIME_WAIT:
    break;
  }/* Un-synchronized state processing */

  /* Check Sequnce Number*/
  if( tcp_seq_lt(seg.seq + length , tcb->rcv.nxt)  ||
      (tcp_seq_gt(seg.seq, tcb->rcv.nxt+tcb->rcv.wnd - 1) &&
      tcb->rcv.wnd != 0) )
  {
    LOG_MSG_INFO1_1("Seqno out of receive window. Dropping packet: seqno %d",
                    seg.seq );
    TCP_INC_STATS(dropped_segs_rx,1);
    dsm_free_packet(&tcp_seg_ptr);
    if(!seg.rst)
    {
      /* Send ACK for rcv.nxt */
      TCP_INC_STATS(dup_acks_tx, 1);
      TCP_INC_INSTANCE_STATS(tcb,dup_acks_tx, 1);
      tcb->flags.force = 1;
      tcp_output(tcb);
    }
    return;
  }

  /* Check for  RST bit */
  if(seg.rst)
  {
    LOG_MSG_INFO1_0("Received RESET message from peer");
    TCP_INC_STATS(rst_rx, 1);

    if (tcb->state == TCP_LAST_ACK)
    {
      tcp_close_self(tcb, PS_PCB_CLOSE_REASON_CODE_NORMAL);
    }
    else
    {
      tcp_close_self(tcb, PS_PCB_CLOSE_REASON_CODE_RESET);
    }

    dsm_free_packet(&tcp_seg_ptr);
    return;
  }

  /* (Security check skipped here) p. 71 */

  /* Check for erroneous extra SYN */
  if(seg.syn)
  {
    TCP_INC_STATS(dropped_segs_rx,1);
    dsm_free_packet(&tcp_seg_ptr);
    tcp_send_reset(&seg,&ip_pkt_info_ptr->pseudo_hdr,&tcb->ipcb);
    return;
  }

  /* Check ack field p. 72 */
  if(!seg.ack_flag)
  {
    /* All segments after synchronization must have ACK */
    TCP_INC_STATS(dropped_segs_rx,1);
    LOG_MSG_INFO1_1( "Missing ACK, Discard Seg: %lx", seg.seq);
    dsm_free_packet(&tcp_seg_ptr);
    return;
  }

  /* update the time stamp */
  if( tcb->flags.ts_permitted)
  {
    if(tcp_seq_lt(seg.seq -1 , tcb->last_ack_sent)  &&
       tcp_seq_gt(seg.seq + length, tcb->last_ack_sent))
    {
      tcb->tsecr = seg.tsval;
    }
  }

  /* Process ACK */
  switch(tcb->state)
  {
    case TCP_SYN_RECEIVED:
      if(tcp_seq_within(seg.ack,tcb->snd.una+1,tcb->snd.nxt))
      {
        update(tcb,&seg,length);
        tcp_setstate(tcb,TCP_ESTABLISHED);
      }
      else
      {
        TCP_INC_STATS(dropped_segs_rx,1);
        dsm_free_packet(&tcp_seg_ptr);
        tcp_send_reset(&seg,&ip_pkt_info_ptr->pseudo_hdr,&tcb->ipcb);
        return;
      }
      break;

    case TCP_ESTABLISHED:
    case TCP_CLOSE_WAIT:
      /*---------------------------------------------------------------------
        Reset the keepalive timer if the option is set and it is running.

        IS707 Async handler creates a TCB directly instead of opening a TCP
        socket. It may not have created half_open_keepalive timer
      ---------------------------------------------------------------------*/
      if (tcb->options.tcp_keepalive &&
          tcb->half_open_keepalive_timer != PS_TIMER_INVALID_HANDLE)
      {
        if(ps_timer_is_running(tcb->half_open_keepalive_timer) == TRUE)
        {
          timer_ret_val = ps_timer_cancel(tcb->half_open_keepalive_timer);
          if( PS_TIMER_SUCCESS != timer_ret_val )
          {
            LOG_MSG_ERROR_1("Cant cancel timer 0x%x",
                            tcb->half_open_keepalive_timer);
            ASSERT(0);
          }
        }
        timer_ret_val = ps_timer_start(tcb->half_open_keepalive_timer,
                                       sock_config_cb.tcp_keepalive_idle_time
                                       );
        if( PS_TIMER_SUCCESS != timer_ret_val )
        {
          LOG_MSG_ERROR_1("Cant start timer 0x%x",
                          tcb->half_open_keepalive_timer);
          ASSERT(0);
        }
      }

    /* intended fallthrough for FINWAIT1/CLOSING/LAST_ACK */
    case TCP_FINWAIT1:  /* p. 73 */
    case TCP_CLOSING:
    case TCP_LAST_ACK:

      update(tcb,&seg,length);

      /*---------------------------------------------------------------------
        NOTE: The reason this code is not put in update() is because, persist
        timer should be started only when TCP is in ESTABLISHED or CLOSE_WAIT
        states. Window update checks in update() doesn't check TCP state.

        IS707 Async handler creates a TCB directly instead of opening a TCP
        socket. It may not have created persist_timer

        Consider segment window for persist timer logic only if it has 
        been used in update()
      ---------------------------------------------------------------------*/
      if ( tcb->persist_timer != PS_TIMER_INVALID_HANDLE &&
           seg.wnd == tcb->snd.wnd )
      {
        if (0 == seg.wnd)
        {
          /*-----------------------------------------------------------------
            Start persist timer only if the peer's window size is 0 and there
            is more data to be sent. If data is in transit, rely on
            retransmission timer to get new window size and don't start
            persist timer. Start persist timer here only for the first time.
            Callback function will start it subsequently. persist_backoff
            value need to be checked since multiple packets with send window
            size of 0 could be received after persist timer is started for the
            first time and before the persist timer is expired.
          -----------------------------------------------------------------*/
          if (tcb->snd.nxt == tcb->snd.una && 0 != tcb->sndcnt &&
              0 == tcb->persist_backoff)
          {
            tcp_start_persist_timer(tcb);
          }
        }
        /*-------------------------------------------------------------------
          Peer's window > 0. So if persist_backoff > 0, stop persist timer.
        -------------------------------------------------------------------*/
        else if ( 0 < tcb->persist_backoff &&
                  ps_timer_is_running(tcb->persist_timer))
        {
          timer_ret_val = ps_timer_cancel(tcb->persist_timer);
          if( PS_TIMER_SUCCESS != timer_ret_val )
          {
            LOG_MSG_ERROR_1("Cant cancel timer 0x%x", tcb->persist_timer);
            ASSERT(0);
          }

          tcb->persist_backoff = 0;

          /*-----------------------------------------------------------------
            Since window probes are not considered as actual data
            transmission, reset these variables so that the byte transmitted
            in window probe is transmitted again if it is not acked. No need
            to check if that byte is acked as snd.nxt would have been set to
            snd.una and numb_bytes_in_transit to 0 anyway, when ack is
            received.
          -----------------------------------------------------------------*/
          tcb->snd.nxt               = tcb->snd.una;
          tcb->numb_bytes_in_transit = 0;
        }
      }

      if(tcb->sndcnt == 0)
      {
        switch (tcb->state)
        {
          case TCP_FINWAIT1:
            /* Our FIN is acknowledged */
            tcp_setstate(tcb,TCP_FINWAIT2);
            break;

          case TCP_CLOSING:
            /* Our FIN is acknowledged */
            tcp_setstate(tcb,TCP_TIME_WAIT);
            tcb->tcp_rexmit_timer_val = MSL2*1000L;
            timer_ret_val = ps_timer_start( tcb->timer,
                                            tcb->tcp_rexmit_timer_val
                                          );
            if( PS_TIMER_SUCCESS != timer_ret_val )
            {
              LOG_MSG_ERROR_1("Cant start timer 0x%x", tcb->timer);
              ASSERT(0);
            }
            break;

          case TCP_LAST_ACK:
            /* Our FIN is acknowledged, close connection */
            tcp_close_self(tcb, PS_PCB_CLOSE_REASON_CODE_NORMAL);
            LOG_MSG_INFO1_0("LAST_ACK ack recvd - Exiting");
            dsm_free_packet(&tcp_seg_ptr);
            return;

          default:
            /* Do nothing */
            break;
        } /* end switch tcp_state */
      } /* if sndcnt == 0*/
      break;

    case TCP_FINWAIT2:
      update(tcb,&seg,length);
      break;

    case TCP_TIME_WAIT:
      timer_ret_val = ps_timer_start( tcb->timer,
                                      tcb->tcp_rexmit_timer_val
                                    );
      if( PS_TIMER_SUCCESS != timer_ret_val )
      {
        LOG_MSG_ERROR_1("Cant start timer 0x%x", tcb->timer);
        ASSERT(0);
      }
      break;
    case TCP_SYN_SENT:
    case TCP_LISTEN:
    case TCP_CLOSED:
      break;
  } /* switch( state)  ack processing*/

  /* (URGent bit processing skipped here) */

  /* Process the segment text, if any, beginning at rcv.nxt (p. 74) */
  switch(tcb->state)
  {
    case TCP_SYN_RECEIVED:
    case TCP_ESTABLISHED:
    case TCP_FINWAIT1:
    case TCP_FINWAIT2:
      (void) tcp_enqueue_data(tcb, tcp_seg_ptr, &seg, length);
      break;
    case TCP_TIME_WAIT:
      /*---------------------------------------------------------------------
        If we are in TIME_WAIT state and haven't received a FIN from peer
        then ack the data or fin sent by the peer.
      ---------------------------------------------------------------------*/
      if(!tcb->flags.fin_recvd)
      {
        if(length != 0 || seg.fin)
        {
          if(tcb->rcv.nxt == seg.seq)
          {
            tcb->rcv.nxt += length;
            if(seg.fin)
            {
              tcb->rcv.nxt ++;
              tcb->flags.fin_recvd = TRUE;
              tcb->fin_seqno = seg.seq +length;
            }
          }
          tcb->flags.force = TRUE;
        }
      }
      dsm_free_packet(&tcp_seg_ptr);
      break;
    default:
      /* Ignore segment text */
      dsm_free_packet(&tcp_seg_ptr);
  } /* switch( state) */

#ifdef FEATURE_DATA_IS707_ASYNC
  if( ps707_async_in_rawdata_mode() == TRUE)
  {
    ps707_async_process_tcp_send_q(TRUE); /* Send ACK and any new data */
  }
  else
#endif /* FEATURE_DATA_IS707_ASYNC */
  {
    tcp_output( tcb);
  }

} /* tcp_input() */

