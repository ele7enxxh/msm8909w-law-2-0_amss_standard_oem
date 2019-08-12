#ifndef TCP_NEW_H
#define TCP_NEW_H

/*===========================================================================

                               P S _ T C P . H

DESCRIPTION
   This TCP implementation follows RFC 793 as closely as possible.
   This header file includes type definitions for the TCP control block and
   other data structures. It also includes the declarations for the external
   functions of TCP.

Copyright (c) 1995-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcp.h_v   1.7   20 Nov 2002 10:33:30   mlioy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_tcp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/20/15    ss     Add IP family check before picking up TCP server socket
07/26/10    ps     Added code changes to fix the broken Async/Fax support.
06/15/10    vm     Adding connect timeout feature
10/22/08    dm     Modified TCP and IP4 headers to fix compiler warnings
02/05/07    rt     Removed EIFEL code.
10/30/06    hm     Updated the value of DEF_RTT if FEATURE_JCDMA_2 is defined
10/13/06    hm     Cleaned up the code, removed several unused variables
                   and function declarations
10/12/06    hm     Fixed code for negotiating timestamp when tsval sent in
                   the SYN is 0.
12/03/05    rt     Added code for new tcp option DSS_TCP_EIFEL.
08/15/05    kr     Removed feature DS_PSTATS and added new stats structure
08/01/05    vp     Increased DEF_RTT to 3000 ms.
06/13/05    vp     Declarations for tcp_quench() tcp_notify().
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
02/28/05    sv     Removed references to old IPSEC code.
01/08/05    vp     Added last_time (last timestamp sent for the conn) to tcb.
10/25/04    msr    Added support for persist timer.
10/13/04    vp     Restructuring of struct tcp. Declaration of
                   tcp_hdr_parse/create(). Related cleanup.
08/16/04    ks/msr Added new field, link, to tcb structure.
07/14/04    sv     Added support for processing incoming SACK option.
07/14/04    vp     Changed tcp_state_enum_type to start with 0 instead of 1.
06/11/04    vp     Removed inclusion of internet.h, netuser.h and psglobal.h
                   Included files ps_pkt_info.h, ps_socket_defs.h,ps_in.h and
                   ps_stats.h Constant defines for codes for open_tcp call.
                   Removed stats info and placed it in ps_stats.h
04/30/04    sv     Added new struct declartion fro tcp options.
04/26/04    sv     Added TCP timestamp option.
04/15/04    sv     Added TCP SACK option.
04/23/04    sv     Changed DEF_MSS to 1220 for IPV6 as we don't support
                   path MTU discovery.
04/14/04    vp     Defines and changes for keepalive as a socket option.
04/14/04    vp     Changed the input signature of reset_tcp function to involve
                   close reason as argument.
03/16/04    mct    Renamed acl_ipfilter_info_type to ip_pkt_info_type.
02/04/04    vp     Changed the signature of send_reset function to include
                   pseudo_hdr and ip_cb structure as input paramters.
02/04/04    vp     Changed the signature of tcp_input function to take
                   acl_ipfilter_info_type structure, dsm item and offset.
03/18/04    vp     Addition of rcvbuf in tcb and cant_recv_more in tcp_flags.
02/26/04    vp     Changed the names of functions send_syn, close_self,
                   rtt_add, rtt_get, seq_within, setstate, send_reset and
                   prepended tcp_ in front of their names.
02/26/04    vp     Changed the names of macros seq_lt, seq_gt, seq_ge and
                   geniss and prepended tcp_ before their names.
02/17/04    mct    Removed window field from tcb

01/29/04    mct    Added TCP_TIME_WAIT_INTERVAL for TCP's TIME_WAIT_STATE
01/28/04    sv     Added PS_TCP_DEF_CWIND declaration.
01/14/04    aku    Changed default rcv window size to 45k for HDR targets.
12/02/03    sv     Removed TCP_LIMIT declaration as it is no longer used.
11/12/03    sv     Added tcp half close flag to tcb flags.
11/03/03    mct    Updated ntohtcp() header with new boolean value.
10/17/03    sv     Added sockfd field to TCB. Modified open_tcp to take
                   MSS as an argument.
10/17/03    sv     Increased default MSS to 1460 and default RCV WINDOW
                   to 16k for both 1x and UMTS targets.
10/01/03    sv     Removed MAX_WND declaration as it is no longer used.
08/18/03    sv     Modified the code to use IPv6 addresses internally.
08/01/03    ss/sv  Common code changes to support IPv6
06/27/03    sv     Added range values for TCP_MAX_SEG socket option.
06/27/03    sv     Removed limit field from tcb. Limt on sndq is handled
                   by sndbuf socket option.
06/03/03    ss     Featurized TCP_LIMIT definition for GSM_GPRS and WCDMA_PS
06/02/03    ss     Removed TCP_NO_DELAY. It now now defined as
                   FEATURE_DS_TCP_NO_DELAY in custdata.h.
05/28/03    ss     Increased DEF_WND and TCP_LIMIT. Removed SNDQ_LIMIT.
11/20/02    mvl    fixed feeaturization error.
11/13/02    rc     Changed Rx window size for GPRS.
11/05/02    mvl    Added defines for different MSS and window sizes.
10/03/02    rc     Changed Rx window size for UMTS.
09/17/02    atp    Added routing_cache to tcb definition to consider case of
                   async calls in non-socket builds.
08/30/02    om     Updates for using routing cache when sending TCP resets.
07/10/02    ss     Moved TCP_NO_DELAY flag to this file
02/22/02    pjb    Added accept and listen
02/11/02    ss     Added macro TCP_SNDQ_LIMIT.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
07/23/01    pjb    Save the reserved bit from the TCP header.   Calculate
                   worst case header size.
11/27/00    na     Cleanup. Rearranged struct tcb to save memory.
11/10/00    snn    Rearranged the elements in the structure tcb. They
                   are arranged according to the types. The structure
                   definitions were moved out of the structure.
                   Renamed dword to uint32 and word to uint16
                   Changed tcp_state to tcp_state_enum_type
                   Renamed callback functions with _cb post-fix.

10/10/00    na     Modified the headers for tcp_delayed and tcp_timeout.
09/05/00    snn    Modified the Default window to 4 times MSS to allow
                   higher throughput in forward direction.
08/25/00    rc     Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK
11/16/99    rc     Removed an unused field in tcb struct.
08/11/99   ak/rc   Added tcp_ip_upcall fcn ptr to tcb struct, to support
                   ppp renegotiating a new IP address when in sockets mode.
02/25/98   na/hcg  Added support for sockets.  Added socket control block to
                   TCB structure.  Changed default MSL value for sockets.
12/14/98    ldg    Removed dangling comma from tcp_cleanup_reseq() parameter
10/27/98    ldg    Moved #include psglobal.h to inside FEATURE_DS.
                   Added #include iface.h.
                   ARM porting: ROM to ROM const.  enum to typedef enum.
10/29/98    na     Added prototype for tcp_cleanup_reseq.
04/21/98    na     Pre code review cleanup.
04/14/98    ldg    Changed method of counting backoffs.
                   Added counters for payload sent and received.
                   Removed apparatus for dumping rcvcnt and sndcnt stats.
04/14/98    na     (ldg) made changes for reporting TCP backoff statistics.
12/15/97    ldg    Moved TCP out-of-order segment count from UI stats to
                   general stats.
11/06/97    ldg    Added prototype for send_reset().
10/16/97    na     Changed TCP_MAX_BACKOFFS from 6 to 10.
09/24/97    na     Added TCP_MAX_BACKOFFS to limit the number of attempts
                   at retransmissions.
07/31/97    ldg    Added accessor function for UI to see some TCP vars.
06/16/97    fkm    FEATURE_xxx Updates (via Automatic Scripts)
06/12/97    ldg    Added new FEATURE_DS_PSTATS
11/14/96    jjw    Unwired Planet Upgrade (ie added UDP)
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"       /* Customer Specific Features */
#include "target.h"

#include "queue.h"
#include "rex.h"

#include "dsm.h"
#include "ps_ip.h"
#include "ps_pkt_info.h"
#include "ps_socki_defs.h"
#include "ps_in.h"
#include "pstimer.h"
#include "ps_tcp_config.h"
#include "ps_tcp_hdr.h"

struct ps_event_info_s;

#define TCB_NEW
/*---------------------------------------------------------------------------
  TCP connection control block.
  Note, this has been arranged so that the fields are packed.
---------------------------------------------------------------------------*/
struct tcb
{
  q_link_type    link;           /* Link to next TCB                      */

  ip_cb_type     ipcb;

  uint32         iss;            /* Initial send sequence number           */
  uint32         cwind;          /* Congestion window                      */
  uint32         ssthresh;       /* Slow-start threshold                   */
  uint32         rtt_time;       /* Stored clock values for RTT            */
  uint32         total_backoff_time; /* Cumulative time spent in backoffs  */
  uint32         irs;            /* Initial receive sequence number        */
  uint16         mss;            /* Maximum segment size                   */
  uint32         rttseq;         /* Sequence number being timed            */
  uint32         srtt;           /* Smoothed round trip time, milliseconds */
  uint32         mdev;           /* Mean deviation, milliseconds           */
  uint32         quench;         /* Count of incoming ICMP source quenches */
  uint32         unreach;        /* Count of incoming ICMP unreachables    */
  uint32         timeouts;       /* Count of retransmission timeouts       */
  uint32         tcp_rexmit_timer_val; /* Rexmit timer value               */
  uint32         fin_seqno;      /* Required for out of order fin          */
  tcp_reseq_hdr  sack_option[TCP_SACK_OPT_MAX_BLOCKS]; /*  SACK blocks     */
  uint8          sack_option_count; /* Number of sack options              */
  uint32         tsecr;          /* Timestamp echo reply                   */
  uint32         last_time;      /* last timestamp sent                    */
  uint32         last_ack_sent;  /* Last acknowledgement sent              */
  uint32         rcvbuf;        /* Maximum number of bytes allowed in rcvq */
  struct tcp_sequence_block_list  * sack_blocks_head_ptr;
                                             /* sorted list of SACK blocks */
  struct tcp_sequence_block_list  * sack_blocks_tail_ptr;
                                             /* sorted list of SACK blocks */
  uint32         high_seq_rexmit ;
  uint32         high_seq_lost;
  uint32         numb_bytes_in_transit; /* Number of bytes in transit      */
  uint32         recovery_point;  /* seq number to end loss recovery phase */

  void *         scb_ptr;        /* Ptr to socket ctl block                */
  int32          sockfd;         /* socket file descriptor                 */

  /*-------------------------------------------------------------------------
    Call when connection state changes
  -------------------------------------------------------------------------*/
  void (*s_upcall)(struct tcb *tcb,
                   tcp_state_enum_type old_state,
                   tcp_state_enum_type new_state);


  q_type         reseq;          /* Out-of-order segment queue             */
  ps_timer_handle_type timer;    /* Timer ID for TCP timeout timer         */
  ps_timer_handle_type acker;    /* Timer ID for TCP Delayed ACK timer     */
                                 /* Timer ID for TCP close timer           */
  ps_timer_handle_type close_timer;
                                 /* Timer ID for half open and keepalive   */
  ps_timer_handle_type half_open_keepalive_timer;
                                 /* Timer ID for persist timer             */
  ps_timer_handle_type persist_timer;
                                 /* Max backoff time value */
  uint32         max_backoff_time;
  struct         connection conn;/* TCP 4-tuple; IP addr. & port numbers   */
  int user;                      /* User params                            */
  struct         tcb_snd_seq_s snd;/* Transmit data structures             */
  uint32 sndcnt;                 /* # of unack. seq# on sndq- incl. SYN/FIN*/
  struct         tcb_rcv_seq_s rcv;/* Receive data structures              */
  tcp_state_enum_type state;     /* State of TCP                           */
  struct         tcb_flags_s flags;/* TCP flags                            */

  byte dupacks;                  /* Count of duplicate (do-nothing) ACKs   */
  byte backoff;                  /* Number of backoffs                     */
  byte persist_backoff;          /* Number of backoffs for persist timer   */
  uint8 reason;                  /* Reason for closing                     */

  /*-------------------------------------------------------------------------
    If reason == NETWORK, the ICMP type and code values are stored here
  -------------------------------------------------------------------------*/
  uint32 soft_error;             /* To store ICMP soft errors              */
  uint8 type;                    /* ICMP type value                        */
  uint8 code;                    /* ICMP code value                        */
  uint8 tos;                     /* Type of service (for IP)               */
  tcp_options_type options;      /* TCP user options                       */

  void * reader;                 /* TCP SND queue handle                   */
  void * writer;                 /* TCP Receive queue handle               */
  boolean  is_linger_on;
  int32    time_in_sec;
  ps_timer_handle_type linger_timer;
  uint32 timer_overload_flag;    /* Flag to check if re-tx timer is overloaded
                                    This will be overloaded when flow is 
                                    disabled while trying to send the SYN 
                                    packet */

  uint16 family; /* Address Family (DSS_AF_INET or DSS_AF_INET6) */

  struct tcb_stats
  {
    uint64  bytes_tx;
    uint64  bytes_rx;
    uint32  segs_tx;
    uint32  segs_rx;
    uint64  bytes_rexmit;
    uint32  segs_rexmit;
    uint32  dup_bytes_rx;
    uint32  dup_segs_rx;
    uint32  window_probe_tx;
    uint32  out_of_order_segs_rx;
    uint32  out_of_order_bytes_rx;
    uint32  rexmit_timeouts;
    uint32  piggybacked_acks_tx;
    uint32  piggybacked_acks_rx;
    uint32  dup_acks_rx;
    uint32  dup_acks_tx;
    uint32  queried;
  } tcp_i_stats;

};

#endif  /* TCP_NEW_H */
