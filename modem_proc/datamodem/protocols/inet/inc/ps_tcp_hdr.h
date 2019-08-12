#ifndef TCP_HDR_H
#define TCP_HDR_H

/*===========================================================================

                        P S _ T C P _ H D R . H

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
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_tcp_hdr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/17/09    pp     Modified TCP header to use UINT32_T(unsigned int) to fix
                   compile warning.
02/20/99    pp     Created file: Split from ps_tcp.h
===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"       /* Customer Specific Features */
#include "target.h"


#include "dsm.h"
#include "ps_ip.h"
#include "ps_pkt_info.h"
#include "ps_socki_defs.h"
#include "ps_in.h"
#include "pstimer.h"
#include "ps_tcp_config.h"

/*===========================================================================

                      EXTERNAL MACRO DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Macros to compare TCP sequence numbers.
---------------------------------------------------------------------------*/
#define tcp_seq_ge(x,y) ((sint31)((uint32)(x)-(uint32)(y)) >= 0)

#define tcp_seq_gt(x,y) ((sint31)((uint32)(x)-(uint32)(y)) > 0)

#define tcp_seq_lt(x,y) ((sint31)((uint32)(x)-(uint32)(y)) < 0)

/*---------------------------------------------------------------------------
  Generate Initial Sequence Number Macro. Increment clock at 4MB/sec.
---------------------------------------------------------------------------*/
#define tcp_geniss()  ((uint32)msclock() << 12)

/*---------------------------------------------------------------------------
  Codes for the tcp_open call
---------------------------------------------------------------------------*/
typedef enum
{
  TCP_PASSIVE = 0,
  TCP_ACTIVE  = 1,
  TCP_SERVER  = 2                       /* Passive, clone on opening */
} tcp_open_mode_enum_type;

/*---------------------------------------------------------------------------
  TCP sequence block
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 start_seqno;
  uint32 end_seqno;
}tcp_sequence_block;

struct tcp_sequence_block_list
{
  tcp_sequence_block  seq_block;
  struct tcp_sequence_block_list * next_ptr;
  struct tcp_sequence_block_list * prev_ptr;
};

struct tcp
{
  uint16 source;                 /* Source port                            */
  uint16 dest;                   /* Destination port                       */
  uint32 seq;                    /* Sequence number                        */
  uint32 ack;                    /* Acknowledgment number                  */
  /*-------------------------------------------------------------------------
   ANSI C: Bit wise fields should be defined with "int" type ONLY. 
   AMSS "uint32" uses unsigned long and cannot be used to define Bit fields.
   Current "unit32" definition in comdef.h violates ANSI C rule, hence 
   created new MACRO to define unsigned int
  -------------------------------------------------------------------------*/
#define UINT32_T unsigned int 
  UINT32_T reserved1:4;          /* Reserved bits                          */
  UINT32_T hlen:4;               /* TCP header length in 4 byte units      */
  UINT32_T fin:1;                /* FIN bit - finish the TCP connection    */
  UINT32_T syn:1;                /* SYN bit - sync up the TCP connection   */
  UINT32_T rst:1;                /* RESET the TCP connection               */
  UINT32_T psh:1;                /* PUSH bit                               */
  UINT32_T ack_flag:1;           /* This segment contains acknowledgement  */
  UINT32_T urg:1;                /* Urgent Bit                             */
  UINT32_T congest:1;            /* Echoed IP congestion experienced bit   */
  UINT32_T reserved2:1;          /* Part of the resrved bits above         */
  UINT32_T wnd:16;               /* Receiver flow control window           */
  uint16 checksum;               /* Checksum                               */
  uint16 up;                     /* Urgent pointer                         */
  uint8 options[TCP_MAXOPT];     /* Options field                          */
  uint8  optlen;                 /* Length of options field, bytes         */
  uint16 mss;                    /* Optional max seg size                  */
  boolean sack_permitted;        /* Optional SACK permitted option         */
  boolean window_scale_enable;   /* Window scale option is enabled         */
  boolean ts_present;            /* Timestamp option is enabled			   */
  uint8  window_scale;           /* Optional window scale options          */
  uint32 tsval;                  /* Time stamp value field                 */
  uint32 tsecr;                  /* Time stamp echo reply field            */
  tcp_sequence_block sack_blocks[TCP_SACK_OPT_MAX_BLOCKS]; /* sack blocks  */
  uint8  sack_block_cnt;         /* Number of SACK blocks                  */
};

/*---------------------------------------------------------------------------
  TCP option numbers.
---------------------------------------------------------------------------*/
typedef enum
{
  TCP_EOL_KIND = 0,
  TCP_NOOP_KIND = 1,
  TCP_MSS_KIND = 2,
  TCP_WINDOW_SCALE_KIND = 3,
  TCP_SACK_PERMITTED_KIND = 4,
  TCP_SACK_KIND = 5,
  TCP_TIME_STAMP_KIND = 8
} tcp_options_kind_enum_type;

/*---------------------------------------------------------------------------
  TCP option length
---------------------------------------------------------------------------*/
#define TCP_MSS_LENGTH  4
#define TCP_SACK_PERMITTED_LENGTH 2
#define TCP_TIME_STAMP_LENGTH 10
#define TCP_UNIT_SACK_OPT_SIZE 8
#define TCP_WINDOW_SCALE_OPTION_LENGTH 3

/*---------------------------------------------------------------------------
  These numbers match those defined in the MIB for TCP connection state
---------------------------------------------------------------------------*/
typedef enum
{
  TCP_CLOSED,
  TCP_LISTEN,
  TCP_SYN_SENT,
  TCP_SYN_RECEIVED,
  TCP_ESTABLISHED,
  TCP_FINWAIT1,
  TCP_FINWAIT2,
  TCP_CLOSE_WAIT,
  TCP_LAST_ACK,
  TCP_CLOSING,
  TCP_TIME_WAIT
}tcp_state_enum_type ;

/*---------------------------------------------------------------------------
  Send sequence variables
---------------------------------------------------------------------------*/
struct tcb_snd_seq_s
{
  uint32       una;    /* First unacknowledged sequence number             */
  uint32       nxt;    /* Next sequence num to be sent for the first time  */
  uint32       ptr;    /* Working transmission pointer                     */
  uint32       wl1;    /* Sequence number used for last window update      */
  uint32       wl2;    /* Ack number used for last window update           */
  uint32       wnd;    /* Other end's offered receive window               */
  uint16       up;     /* Send urgent pointer                              */
  uint8        scale;  /* Other end's window scale                         */
};

/*---------------------------------------------------------------------------
   Receive sequence variables
---------------------------------------------------------------------------*/
struct tcb_rcv_seq_s
{
  uint32       nxt;              /* Incoming sequence number expected next */
  uint32       wnd;              /* Our offered receive window             */
  uint16       up;               /* Receive urgent pointer                 */
  uint8        scale;            /* Our window scale                       */
};

/*---------------------------------------------------------------------------
   TCP control flags
---------------------------------------------------------------------------*/
struct tcb_flags_s
{
  unsigned int force:1;          /* Force transmit a segment               */
  unsigned int clone:1;          /* Server-type TCB,cloned on incoming SYN */
  unsigned int retran:1;         /* A retransmission has occurred          */
  unsigned int active:1;         /* TCB created with an active open        */
  unsigned int synack:1;         /* Our SYN has been acked                 */
  unsigned int rtt_run:1;        /* We're timing a segment                 */
  unsigned int congest:1;        /* Copy of last IP congest bit received   */
  unsigned int half_close:1;     /* tcp half close flag                    */
  unsigned int sack_permitted:1; /* tcp sack permitted ?                   */
  unsigned int ts_permitted:1;   /* TCP time stamp option negotiated?      */
  unsigned int fin_recvd:1;      /* Out of order fin received              */
  unsigned int window_scale_enabled:1; /* Window scale option is enabled   */
};

/*---------------------------------------------------------------------------
   TCP options
---------------------------------------------------------------------------*/
typedef struct tcp_options_s
{
  unsigned int tcp_nodelay:1;
  unsigned int tcp_keepalive:1;
  unsigned int tcp_delayed_ack:1;
  unsigned int tcp_sack_permitted:1;
  unsigned int tcp_ts_permitted:1;
}tcp_options_type;

/*---------------------------------------------------------------------------
  TCP resequnce header.
---------------------------------------------------------------------------*/

typedef struct
{
  uint32 length;
  uint32 seqno;
}tcp_reseq_hdr;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/* In tcphdr.c: */
/*===========================================================================

FUNCTION TCP_HDR_CREATE()

DESCRIPTION
  This function will Convert TCP header in host format into mbuf ready for
  transmission, link in data (if any). If ph != NULL, compute checksum,
  otherwise take checksum from tcph->checksum

PARAMETERS
  dsm_item_type**    - DSM item with the outgoing packet.
  struct tcp*        - TCP header.
  Pseudo_header_type - Pseudo header for checksum calculations.

DEPENDENCIES
  Source and destination port in passed TCP header should be in network
  order.
  The pseudo header length should only have the data size in it as TCP
  header size is not known before call to this function.

RETURN VALUE
  boolean - TRUE on success and FALSE on failure.

SIDE EFFECTS
  None
===========================================================================*/

boolean tcp_hdr_create
(
  dsm_item_type      **bpp,
  struct tcp           *tcph,
  pseudo_header_type *ph
);

/*===========================================================================

FUNCTION TCP_HDR_PARSE()

DESCRIPTION
  This function will update the appropriate fields in passed TCP header
  after extracting information from dsm item.

PARAMETERS
  dsm_item_type * - DSM item containing the packet.
  uint16 *        - Offset where the TCP header is in the DSM item.
  struct tcp *    - TCP header to be filled.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE on success and FALSE on failure.

SIDE EFFECTS
  Sets the value of offset to after the TCP header in dsm item.
===========================================================================*/
boolean tcp_hdr_parse
(
  dsm_item_type *bpp,
  uint16        *offset,
  struct tcp    *tcph
);

/* In tcpin.c: */

void tcp_send_syn
(
  struct tcb *tcb
);


void tcp_input
(
  ip_pkt_info_type*        ip_pkt_info_ptr,   /* IP Filter Info            */
  struct dsm_item_s        *tcp_seg_ptr,      /* Data field, if any        */
  uint16                   offset             /* Offset of the TCP packet  */
);

/* In tcpsubr.c: */

void tcp_close_self
(
  struct tcb *tcb,
  int        reason
);


struct tcb *create_tcb
(
  struct connection *conn
);

int tcp_seq_within
(
  uint32 x,
  uint32 low,
  uint32 high
);



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
  Persist_backoff field of TCB is incremented
===========================================================================*/
void tcp_start_persist_timer
(
  struct tcb  * tcb_ptr
);


/* In tcpshell.c */

sint15 send_tcp
(
  struct tcb        *tcb,
  struct dsm_item_s *bp
);

extern  void open_tcp
(
  struct socket *lsocket,  /* Local socket */
  struct socket *fsocket,  /* Remote socket */
  int           mode,      /* Active/passive/server */
  uint32        window,    /* Receive window (and send buffer) sizes */
  uint16        mss,       /* TCP mss  */
  int           tos,       /* Type of service   */
  int           user,      /* User linkage area */
  struct tcb    *Tcb_ptr
);



void tcp_setstate(struct tcb *tcb, tcp_state_enum_type newstate);

/* In tcpout.c: */

void tcp_output
(
  struct tcb *tcb
);

/* In tcptimer.c: */

void tcp_delayed_cb
(
  void *
);

void tcp_timeout_cb
(
  void *
);


/* In tcpuser.c: */

int close_tcp
(
  struct tcb *tcb
);

void reset_tcp
(
  struct tcb *tcb,
  uint8 close_reason
);



/*===========================================================================

FUNCTION TCP_SEND_RESET

DESCRIPTION
  This function will Send an acceptable reset (RST) response for this
  segment. The RST reply is composed in place on the input segment.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tcp_send_reset
(
  register struct tcp      *seg,              /* Offending TCP header      */
  pseudo_header_type       *pseudo_hdr,	      /* tcp pseudo hdr            */
  ip_cb_type               *ip_cb             /* IP control block          */
);


/*===========================================================================

FUNCTION TCP_CLEANUP_RESEQ

DESCRIPTION
  This function cleans up the resequencing queue for the given TCP connection

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
);
/*===========================================================================

FUNCTION TCP_CLEAR_SACK_DATA()

DESCRIPTION
  This function will expunge all SACK information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tcp_clear_sack_data
(
  register struct tcb *tcb                 /* pointer to TCP control block */
);

/*===========================================================================

FUNCTION TCP_QUENCH()

DESCRIPTION
  This function flow controls the TCP connection identified by the passed
  conn structure. Sets the congestion window to half.

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
);

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
);



/*===========================================================================
FUNCTION DSSTCP_LOOKUP_CONN()

DESCRIPTION
  Given a connection structure, looks up in the tcb to see if this is a
  valid connection.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to TCB if valid connection.  If not, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
struct tcb *dsstcp_lookup_conn
(
  struct connection *conn                    /* connection to be looked up */
);

/*===========================================================================
FUNCTION DSSTCP_LOOKUP_SERV()

DESCRIPTION
  Given a connection structure, looks up in the tcb to see if this is a
  valid server waiting for new connections.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to TCB if valid connection.  If not, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
struct tcb *dsstcp_lookup_serv
(
  struct connection *conn                    /* connection to be looked up */
);

#endif  /* TCP_HDR_H */
