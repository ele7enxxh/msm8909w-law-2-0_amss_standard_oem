/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            T C P S H E L L

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1995-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcpshell.c_v   1.1   30 Aug 2002 20:59:26   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_tcpshell.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/21/12   ss      Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR and 
                   evaluating ASSERT requirement.
12/23/10   ss      Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
01/11/10   ss      Lint warning fixes.
09/31/06   msr     Checking if rexmit timer is running before starting persist
                   timer in append_to_sndq()
06/13/06   msr     Making sure that timers are non-NULL before manipulating
                   them. Async TCB is not creating all the timers
11/05/05    rt     Fixed coverity errors.
08/12/05   kr      Removed FEATURE DS_PSTATS
07/20/05   rt      Setting the half_close flag as FALSE if close_tcp() during
                   FINWAIT1 state.
12/27/04   ifk     Replaced setting of tracer field with DSM_TOUCH_ITEM
12/02/04   msr     Including pstimer.h
10/25/04   msr     Starting persist timer only if there is data to be sent
                   and if receiver's window size is 0
10/13/04   vp      Removal of byte order conversion on ports in reset_tcp().
09/10/04   vp      Corrected the assignment of sequence number in RESET
                   segment in reset_tcp().
08/16/04   sv      Removed nxt pointer increments when we are sending FIN as
                   tcp_output already updates that.
06/11/04   vp      Removal of inclusion of psglobal.h,netuser.h and internet.h.
                   Included ps_pkt_info.h and ps_socket_defs.h. Replaced use of
                   TCP_PTCL with PS_IPPROTO_TCP. Replaced the use of byte ordering
                   macros with their PS versions.
05/19/04   vp      Assigned tcb_ptr->cwind according to passed mss in open_tcp
04/14/04   vp      Changed the input signature of reset_tcp to include
                   close_reason as an argument and removed a byte ordering bug
                   in reset_tcp
02/04/04   vp      Fixed function reset_tcp() due to change in signature of
                   send_reset()
02/26/04   vp      Changed the name of functions setstate, close_self,
                   send_reset, send_syn and added tcp_in front of thier names.
02/17/04   mct     Replaced references to tcb window field with rcv.wnd
12/12/03   sv      Modified tcp_close to change tcp state from FIN_WAIT2 to
                   TIME_WAIT upon a close call in FIN_WAIT2 state.
10/17/03   sv      Modified open_tcp to return void.
08/18/03   sv      Modified the code to use IPv6 addresses internally.
08/01/03   ss/sv   Restrucutured the code to support IPv6
08/30/02   om      Updates for using routing cache when sending TCP resets.
07/31/02   usb     Added include ps_iface.h
07/31/02   usb     Removed include ps.h
03/05/02   pjb     Finished Accept Listen Code Review Items
02/22/02   pjb     Added accept and listen
11/12/01   dwp     Add "|| FEATURE_DATA_PS" to whole module. Wrap out
                   ps_initiate_upper_layer() if FEATURE_DATA_PS is defined.
10/30/00   snn     Replaced Tcb_ptr with tcb_ptr to follow naming convention.
10/10/00   na/snn  Removed the sndq_tail_ptr and put it in the TCB. This was
                   done to support multiple TCP sockets.
08/25/00   rc      Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK
09/14/98   na      Made changes for improved bidirectional performance.
05/05/98   na      Fixed a pointer problem in append_to_sendq.
04/14/98   ldg     Added counter for payload sent.
04/14/98   na      Changed append_to_sendq to save memory items by copying
                   small ones.
01/27/98   ldg     Removed the DS_SLIM switch.
01/19/98   na      Changes in append to sendq to facilitate escaping at task
                   level. Also, removed ISS2 target dependent code that needed
                   smaller memory items.
06/17/97   fkm     FEATURE_xxx Updates (via Automatic Scripts)
06/12/97   ldg     Reset stats in open_tcp for new FEATURE_DS_PSTATS.
01/23/97   na      Optimized append to sndq for better throughput. Uses small
                   items on the TX stream instead of large ones. Older code
                   under #if T_I2 due to ISS2 memory limitations.
01/20/97   na      Fixed bug for proper port assignment when TCP is opened
                   passively in a LtoM call
11/14/96   jjw     Upgraded (sped up) TCP open scheme
10/17/96   jjw     Minor change to initiate TCP Open after PPP (IPCP) Opens
05/24/96   jjw     Gemini & ISS2 Data Services release.
11/13/95   jjw     Changed min/max macros to upper case
11/09/95   jjw     IS-99 Compliant Release

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* User calls to TCP
 * Copyright 1991 Phil Karn, KA9Q
 * Additional material Copyright 1992 Glenn Hamilton McGregor
 * Additional material Copyright 1992 William Allen Simpson
 */


#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "dsm.h"
#include "ps_pkt_info.h"
#include "ps_iface.h"

#include "ps_tcp.h"
#include "ps_pcb_close_reason_codes.h"
#include "ps_ip.h"
#include "memory.h"
#include "msg.h"
#include "err.h"
#include "ps_socket_defs.h"
#include "pstimer.h"
#include "ds_Utils_DebugMsg.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

static const uint16 Tcp_mss    = DEF_MSS;
static const uint16 Tcp_window = DEF_WND;


/*===========================================================================

FUNCTION OPEN_TCP

DESCRIPTION
  This function will Open a TCP connection using the specified parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

 void open_tcp
(
  struct socket *lsocket,  /* Local socket */
  struct socket *fsocket,  /* Remote socket */
  int           mode,      /* Active/passive/server */
  uint32        window,    /* Receive window (and send buffer) sizes */
  uint16        mss,       /* TCP mss  */
  int           tos,       /* Type of service   */
  int           user,      /* User linkage area */
  struct tcb    *tcb_ptr
)
{
  uint32 high_rcv_window = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( lsocket == NULL )
  {
    DATA_ERR_FATAL("open_tcp: NULL socket passed");
    return;
  }

  tcb_ptr->conn.local.address = lsocket->address;
  tcb_ptr->conn.local.port    = lsocket->port;
  if(fsocket != NULL)
  {
    tcb_ptr->conn.remote.address = fsocket->address;
    tcb_ptr->conn.remote.port    = fsocket->port;
  }
  else
  {
    tcb_ptr->conn.remote.address = ps_in6addr_any;
    tcb_ptr->conn.remote.port    = 0;
  }

  tcb_ptr->user = user;
  if( window != 0)
  {
    tcb_ptr->rcv.wnd = window;
  }
  else if( Tcp_window >= Tcp_mss )
  {
    tcb_ptr->rcv.wnd = Tcp_window;
  }
  else
  {
    tcb_ptr->rcv.wnd = Tcp_window * Tcp_mss;
  }

  /*-------------------------------------------------------------------------
   Enable window scale option by default
  -------------------------------------------------------------------------*/
  tcb_ptr->flags.window_scale_enabled = TRUE;
  high_rcv_window = tcb_ptr->rcv.wnd >> 16;
  if(high_rcv_window > 0)
  {
    do
    {
      tcb_ptr->rcv.scale++;
      high_rcv_window >>= 1;
    } while(high_rcv_window != 0);
  }

  tcb_ptr->snd.wnd = 1;      /* Allow space for sending a SYN */
  ASSERT( tos >= 0 && tos < 256 );
  tcb_ptr->tos                 = (uint8)tos;
  tcb_ptr->mss                 = mss;
  tcb_ptr->timer_overload_flag = FALSE;
  /*-------------------------------------------------------------------------
    Update the congestion window depending upon the passed mss. The reason
    for this is that Async/Fax data calls have different default mss than PS.
  -------------------------------------------------------------------------*/
  tcb_ptr->cwind   = MIN(2 * mss, PS_TCP_DEF_CWIND);

  switch( mode)
  {
    case TCP_SERVER:
      tcb_ptr->flags.clone = 1;
      tcp_setstate( tcb_ptr, TCP_LISTEN);
      break;

    case TCP_PASSIVE:
      tcp_setstate( tcb_ptr, TCP_LISTEN);
      break;

    case TCP_ACTIVE:
      /* Send SYN, go into TCP_SYN_SENT state */
      tcb_ptr->flags.active = 1;
      tcp_send_syn( tcb_ptr);
      tcp_setstate( tcb_ptr, TCP_SYN_SENT);
      tcp_output( tcb_ptr);
      break;
   default:   /* Error Trap,  Path-whatever-logical */
    LOG_MSG_ERROR_0( "Invalid TCP Open");
  }
  return;
}

/*===========================================================================

FUNCTION RESET_TCP

DESCRIPTION
  This function will

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void reset_tcp
(
  register struct tcb *tcb,
  uint8 close_reason
)
{
  struct tcp fakeseg;
  pseudo_header_type ph;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(tcb == NULL)
  {
    return;
  }
  if(tcb->state != TCP_LISTEN)
  {
    /* Compose a fake segment with just enough info to generate the
     * correct RST reply
     */
    memset(&fakeseg,0,sizeof(fakeseg));
    fakeseg.dest      = tcb->conn.local.port;
    fakeseg.source    = tcb->conn.remote.port;
    fakeseg.ack_flag = 1;
    /* Here we try to pick a sequence number with the greatest likelihood
     * of being in his receive window.
     */
    fakeseg.ack = tcb->snd.nxt;

    /* Fill required fields of pseudo_hdr */
    ph.dest = tcb->conn.local.address;
    ph.source = tcb->conn.remote.address;
    ph.length = TCPLEN;
    ph.protocol = (uint8)PS_IPPROTO_TCP;

    /* Interface */

    tcp_send_reset(&fakeseg,&ph,&tcb->ipcb);
  }
  tcp_close_self(tcb,close_reason);
}

/*===========================================================================

FUNCTION CLOSE_TCP

DESCRIPTION
  This function will close the TCP connection for the passed TCB. This really
  means "I have no more data to send". It only closes the  connection in one
  direction, and we can continue to receive data indefinitely.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

int close_tcp
(
  register struct tcb *tcb
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      if(tcb == NULL)
      {
            return -1;
      }
      switch(tcb->state)
      {
      case TCP_CLOSED:
            return 0;      /* Unlikely */
      case TCP_LISTEN:
      case TCP_SYN_SENT:
            tcp_close_self(tcb, PS_PCB_CLOSE_REASON_CODE_NORMAL);
            return 0;
      case TCP_SYN_RECEIVED:
      case TCP_ESTABLISHED:
            tcb->sndcnt++;
            tcp_setstate(tcb,TCP_FINWAIT1);
            tcp_output(tcb);
            return 0;
      case TCP_CLOSE_WAIT:
            tcb->sndcnt++;
            tcp_setstate(tcb,TCP_LAST_ACK);
            tcp_output(tcb);
            return 0;
      case TCP_FINWAIT2:
           tcp_setstate(tcb, TCP_TIME_WAIT);
           return 0;
      case TCP_FINWAIT1:
           tcb->flags.half_close = FALSE;
           /* lint-fall through */
      case TCP_CLOSING:
      case TCP_LAST_ACK:
      case TCP_TIME_WAIT:
            return -1;
      }
      return -1;      /* "Can't happen" */
}
