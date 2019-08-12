#ifndef TCP_CONFIG_H
#define TCP_CONFIG_H

/*===========================================================================

                       P S _ T C P _ C O N F I G . H

DESCRIPTION
   This TCP implementation follows RFC 793 as closely as possible.
   This header file includes type definitions for the TCP control block and
   other data structures. It also includes the declarations for the external
   functions of TCP.

Copyright (c) 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcp.h_v   1.7   20 Nov 2002 10:33:30   mlioy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_tcp_config.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/20/09    pp     New file created: Split from ps_tcp.h.

===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"       /* Customer Specific Features */
#include "target.h"

#include "ds_Sock_SocketDef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define DEF_MSS TCP_MAXSEG_DEF_VAL

/*---------------------------------------------------------------------------
  Initialize default recv window size
---------------------------------------------------------------------------*/
#define DEF_WND SO_RCVBUF_DEF_VAL

/*---------------------------------------------------------------------------
  Initialize default congestion window size
---------------------------------------------------------------------------*/
#ifdef FEATURE_JCDMA_2
  #define PS_TCP_DEF_CWIND (1 * DEF_MSS)
#else
  #define PS_TCP_DEF_CWIND (2 * DEF_MSS)
#endif

/*---------------------------------------------------------------------------
  Initialize initial RTT (ms)
---------------------------------------------------------------------------*/
#ifdef FEATURE_JCDMA_2
  #define DEF_RTT 1000L
#else
  #define DEF_RTT 3000L
#endif

/*---------------------------------------------------------------------------
  Maximum time in ms that TCP can spend in backoffs. Lot of apps don't want to
  wait for 15-20 minutes doing exponential backoff. So limiting it to 2 minutes
---------------------------------------------------------------------------*/
#define TCP_MAX_TOTAL_BACKOFF_TIME 120000

#define MSL2  30                 /* Guess at two maximum-segment lifetimes */
#define MAX_RTO 120000L          /* Maximum Retransmission timeout (ms)    */
#define TCP_MAX_BACKOFFS 10      /* Maximum # of TCP backoff attempts      */
#define MIN_RTO 500L             /* Minimum Retransmission timeout (ms)    */
#define MAX_ATO 500L             /* Maximum Delayed Ack timeout (ms)       */
#define MIN_ATO 100L             /* Minimum Delayed Ack timeout (ms)       */
#define TCP_SYNDATA 1            /* Data sent with Syn                     */
#define SSTHRESH 65535U          /* Initial Slow Start Threshhold          */
#define TCPDUPACKS  3            /* # of conseq. dup acks for fast recovery*/

/* Can be removed?? */
#define DSS_TCP_MAXSEG_LO (536)  /* Minimum MSS value                      */
#define DSS_TCP_MAXSEG_HI 18*1024/* Maximum MSS value                    */

#define TCP_TIME_WAIT_INTERVAL 30000 /* Time wait duration 30 sec          */
#define TCP_MAX_WND_NO_SCALE (64*1024 - 1)
#define TCP_MAX_WINDOW_SCALE (14)

/*---------------------------------------------------------------------------
  Default timeout values for Keepalive timers in milliseconds.
---------------------------------------------------------------------------*/
#define TCP_KEEPALIVE_IDLE_TIME (2*60*60*1000) /* Default keepalive time   */
#define TCP_KEEPALIVE_INTERVAL 75000 /* Time between keepalive probes      */
#define TCP_MAX_KEEPALIVE_PROBES 9   /* Maximum number of keepalive probes */

/*---------------------------------------------------------------------------
  Default timeout values in milliseconds for persist timers.
  RFC 793 (section 3.7) talks about window probes. The following values
  are obtained from "TCP/IP Illustrated, Volume 1, Richard Stevens"
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Base value used to calculate timeout for persist timer. This is used in
  exponential backoff algorithm to get the actual timeout
---------------------------------------------------------------------------*/
#define TCP_PERSIST_BASE_TIMEOUT  1500     /* Base timeout is 1.5s         */
#define TCP_PERSIST_MIN_TIMEOUT   5000     /* Min. timeout is bound to 5s  */
#define TCP_PERSIST_MAX_TIMEOUT   60000    /* Max. timeout is bound to 60s */

/*---------------------------------------------------------------------------
  Round Trip Time calculation parameters.
---------------------------------------------------------------------------*/
#define AGAIN   8                /* Average RTT gain = 1/8                 */
#define LAGAIN  3                /* Log2(AGAIN)                            */
#define DGAIN   4                /* Mean deviation gain = 1/4              */
#define LDGAIN  2                /* log2(DGAIN)                            */

/*---------------------------------------------------------------------------
  TCP segment header -- internal representation
  Note that this structure is NOT the actual header as it appears on the
  network (in particular, the offset field is missing).
  All that knowledge is in the functions ntohtcp() and htontcp() in tcpsubr.c
---------------------------------------------------------------------------*/
#define TCPLEN      20           /* Minimum Header length, bytes           */
#define TCP_MAXOPT  40           /* Largest option field, bytes            */
#define TCP_SACK_OPT_MAX_BLOCKS 4 /* Maximum blocks in SACK option         */

/*---------------------------------------------------------------------------
 Worst case TCP/IP/PPP header.
---------------------------------------------------------------------------*/
#define PS_MAX_HDR (TCPLEN + TCP_MAXOPT + IPLEN + IP_MAXOPT + 8) /* 128 */

#endif  /* TCP_CONFIG_H */
