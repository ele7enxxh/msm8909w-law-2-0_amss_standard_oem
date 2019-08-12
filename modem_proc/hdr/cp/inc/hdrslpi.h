#ifndef HDRSLPI_H
#define HDRSLPI_H
/*===========================================================================

            H D R   S I G N A L I N G   L A Y E R   P R O T O C O L
                   I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This file contains data definitions and functions for SLP's internal use.

Copyright (c) 2000-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrslpi.h_v   1.20   18 Apr 2003 11:28:20   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrslpi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/19/12   cnx     Fixed compiler errors.
01/03/12   sju     Added support to enable verbose debug f3 at runtime. 
11/08/07   sju     Added support to allow AHO before the first access attempt
                   when there is no DO session
12/06/06   sju     Make SLP reassembly buffers standard compliant.
07/11/06   dna     Fix logging of msgs > 255 bytes.
11/28/05   yll     Fixed a fragmentation problem for messages over 255 bytes.
06/22/05   pba     Re-defined Connection request and DOS message - msg ID, to 
                   avoid including these protocols internal header files in SLP
11/29/04   grl     Added connected state stat logging support.
04/17/03   mpa     Fixed handling of Reset message
01/15/03   mpa     Cleaned up hdrsrch dependencies.
07/23/02   om      Fixed handling of Reset message.
09/25/01   om      Removed FEATURE_HDR_HAI_X4
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/14/01   om      Added protocol and msg ID fields in SDU struct
08/01/01   dna     Added logging of ack sequence number valid bit of SLP 
                   header as part of the signaling message log packet
02/22/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#ifndef COMDEF_H
  #error Please include <comdef.h> first
#endif
#ifndef DSM_H
  #error Please include <dsm.h> first
#endif
#ifndef QW_H
  #error Please include <qw.h> first
#endif
#ifndef HDRHAI_H
  #error Please include <hdrhai.h> first
#endif
#ifndef HDRSRCH_TYPES_H
  #error Please include <hdrsrchtypes.h> first
#endif
#ifndef HDRERRNO_H
  #error Please include <hdrerrno.h> first
#endif

#ifdef TARGET_OS_SOLARIS
  #include <stdio.h>
#endif

#include "hdrlogi.h"

/* <EJECT> */ 
/*===========================================================================

              DEFINITIONS AND CONSTANTS FROM HAI Vol. III

===========================================================================*/
/*** SLP header fields ***/
/* NOTE: The first two bits contain the Stream Layer Header.         */
/*       This header is _not_ taken out since proper byte alignment  */
/*       of the SLP header fields requires the offset of 2 bits.     */
  #define HDRSLP_SDU_SEQNO_INVALID    0xff

  /* Header fields for the SLP-F Protocol */
  #define HDRSLP_SDU_FRAGMENT( ptr )  ( *((uint8*) ptr) & 0x02 )
  #define HDRSLP_SDU_FIRST( ptr )     ( *((uint8*) ptr) & 0x01 )
  #define HDRSLP_SDU_LAST( ptr )      ( *((uint8*) (ptr+1)) & 0x80 )
  #define HDRSLP_SDU_FRG_SEQNO( ptr ) ( (*((uint8*) (ptr+1)) & 0x7e) >> 1 )

  /* Header fields for the SLP-D protocol */
  #define HDRSLP_SDU_FULL_HDR( ptr )  ( HDRSLP_SDU_FRAGMENT( ptr ) ? \
            *((uint8*) (ptr+1)) & 0x01 :                             \
            *((uint8*) ptr) & 0x01 )

  #define HDRSLP_SDU_ACK_VALID( ptr ) ( HDRSLP_SDU_FRAGMENT( ptr ) ? \
            *((uint8*) (ptr+2)) & 0x80 :                             \
            *((uint8*) (ptr+1)) & 0x80 ) 
  #define HDRSLP_SDU_ACK_SEQNO( ptr ) ( HDRSLP_SDU_FULL_HDR( ptr ) ? \
            ( HDRSLP_SDU_ACK_VALID( ptr ) ?                          \
              ( HDRSLP_SDU_FRAGMENT( ptr ) ?                         \
                ((*((uint8*) (ptr+2)) & 0x70) >> 4) :                \
                ((*((uint8*) (ptr+1)) & 0x70) >> 4) )                \
              : HDRSLP_SDU_SEQNO_INVALID )                           \
            : HDRSLP_SDU_SEQNO_INVALID )

  #define HDRSLP_SDU_SEQ_VALID( ptr ) ( HDRSLP_SDU_FULL_HDR( ptr ) ? \
            ( HDRSLP_SDU_FRAGMENT( ptr ) ?                           \
              *((uint8*) (ptr+2)) & 0x08 :                           \
              *((uint8*) (ptr+1)) & 0x08 )                           \
            : FALSE )
  #define HDRSLP_SDU_SEQNO( ptr ) ( HDRSLP_SDU_FULL_HDR( ptr ) ?     \
            ( HDRSLP_SDU_SEQ_VALID( ptr ) ?                          \
              ( HDRSLP_SDU_FRAGMENT( ptr ) ?                         \
                *((uint8*) (ptr+2)) & 0x07 :                         \
                *((uint8*) (ptr+1)) & 0x07 )                         \
              : HDRSLP_SDU_SEQNO_INVALID )                           \
            : HDRSLP_SDU_SEQNO_INVALID )

  #define HDRSLP_SDU_HDR_SIZE( ptr )                                  \
               ( HDRSLP_SDU_FRAGMENT( ptr ) ?                         \
                  ( HDRSLP_SDU_FULL_HDR( ptr ) ?                      \
                    3 : 2 )                                           \
                  : ( HDRSLP_SDU_FULL_HDR( ptr ) ? 2 : 1 )            \
               )

  /* Some more meaningful names ;-) */
  #define HDRSLP_SDU_RELIABLE( ptr )   HDRSLP_SDU_SEQ_VALID( ptr )

/*** SLP rexmit timeout and counts ***/
#define HDRSLP_REXMIT_TIME    15      /* in units of 26.666 ms        */

/* Clarification proposed in QC-IS-856 CHANGE REQUEST-29 says 2 retries */
#define HDRSLP_REXMIT_COUNT   2       /* up to 2 rexmits per rel. SDU */

#define HDRSLP_REXMIT_BUFFER_MAX_LEVEL  4

/*** SLP messages ***/
#define HDRSLP_RESET_MSG      0x00
#define HDRSLP_RESET_ACK_MSG  0x01
/* Pick an arbitrary value for logging when SLP sends a header only message */
#define HDRSLP_HEADER_ONLY_MSG  0x0A

/* Message Sequence initialization for Reset message */
#define HDRSLP_INITIALIZE_MSG_SEQ 0

/* As these values are never going to change, re-defined both msg id's here 
 * and avoided including these protocols internal header files in SLP */
#define HDRIDLE_CONN_REQ_ID 0x01

/* Which statistics that are being tracked to clear for logging purposes. */
typedef enum 
{
  HDRSLP_ALL_STATS,
    /* Clear all of the statistics tracked in this log packet. */

  HDRSLP_LAST_CALL_STATS,
    /* Clear only the statistics tracked related to the last call. */

} hdrslp_reset_stats_type;

/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

typedef struct
{
  dsm_item_type*           msg_ptr;   /* dsm_item chain containing sig. msg */
  uint16                   length;    /* Length of sig. msg w/o SLP header  */
  hdrhai_channel_enum_type channel;   /* Dst. channel (HDRHAI_AC|HDRHAI_RTC)*/
  uint8                    prot_id;   /* Protocol ID (for debugging only)   */
  uint8                    prot_msg;  /* Message ID (for debugging only)    */
#ifndef HDRSLP_NO_LOGGING
  boolean                  rel;       /* dbg flag: reliable SDU ?           */ 
  boolean                  frg;       /* dbg flag: fragmented SDU ?         */
  boolean                  fst;       /* dbg flag: first fragment ?         */
  boolean                  lst;       /* dbg var: last fragment ?           */
  uint8                    fno;       /* dbg var: fragment sequence number  */
  uint8                    ano;       /* dbg var: Ack seq. number           */
  boolean                  ano_valid; /* dbg var: Ack seq. number is valid  */
  uint8                    sno;       /* dbg var: seq. number for this SDU  */
#endif
} hdrslp_sdu_struct_type;

typedef struct
{
  /* Regular Statistics */
  uint32 rx_byte_cnt;            /* Total rcvd bytes                         */
  uint32 rx_msg_cnt;             /* Total rcvd msgs                          */
  uint32 rx_msg_ack_cnt;         /* Total rcvd acks                          */
  uint32 rx_rel_msg_cnt;         /* Total rcvd reliable delivery msgs        */
  uint32 rx_bef_msg_cnt;         /* Total rcvd best-effort msgs              */
  uint32 rx_fragment_msg_cnt;    /* Total rcvd fragmented msgs               */
  uint32 rx_min_msg_size;        /* Smallest rcvd msg size since last reset  */
  uint32 rx_max_msg_size;        /* Largest rcvd msg size since last reset   */

  uint32 tx_byte_cnt;            /* Total sent bytes                         */
  uint32 tx_msg_cnt;             /* Total sent msgs                          */
  uint32 tx_msg_ack_cnt;         /* Total sent acks                          */
  uint32 tx_rel_msg_cnt;         /* Total sent reliable delivery msgs        */
  uint32 tx_bef_msg_cnt;         /* Total sent best-effort msgs              */
  uint32 tx_traffic_msg_cnt;     /* Total sent msgs on traffic channel       */
  uint32 tx_access_msg_cnt;      /* Total sent msgs on access channel        */
  uint32 tx_rexmit_msg_cnt;      /* Total sent retransmitted msgs            */
  uint32 tx_fragment_msg_cnt;    /* Total sent fragmented msgs               */
  uint32 tx_min_msg_size;        /* Smallest sent msg size since last reset  */
  uint32 tx_max_msg_size;        /* Largest sent msg size since last reset   */
  
  /* Failure statistics: */
  uint32 rx_frg_missing_cnt;     /* Reassembly failure due to missing fragment
                                    (non-fragmented SDU rcvd during reassembly,
                                    first-bit-set rcvd during reassembly)   */
  uint32 rx_out_of_order_cnt;    /* Reassembly failure due to non-match tag */
  uint32 rx_fcs_failure_cnt;     /* FCS check failed on rcvd SDU            */
  uint32 rx_duplicate_cnt;       /* Duplicate SDU seq number rcvd           */
  
  uint32 tx_seq_space_full_cnt;  /* Xmit on hold due to > 4 outstanding msg */

  /* General statistics: */
  qword  last_reset;             /* CDMA time of last SLP reset             */
} hdrslp_stat_struct_type;

/*** Reset msg sequence number ***/
extern uint8               hdrslp_reset_seqno;
extern boolean             hdrslp_in_reset;

/* If there is no DO session, AHO is allowed before the AT sends out the first 
 * access probe. 
 * Example of use case: The AT acquires a DO network without session. 
 *                      It allows the AT first handoff(e.g. using LRCH) to a sector 
 *                      with better signal then opens DO Session. 
 */
extern boolean             hdrslp_access_attempted_since_entering_idle;

/*** SLP stats structure ***/
extern hdrslp_stat_struct_type   hdrslp_stats;

extern uint8               hdrslp_verbose_debug; 

/* <EJECT> */ 
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRSLP_PROCESS_IND

DESCRIPTION
  This function processes any indications given to SLP.

DEPENDENCIES
  None
 
PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_process_ind
(
  hdrind_ind_name_enum_type  ind_name,
  void*                   ind_data_ptr 
);

/*===========================================================================

FUNCTION HDRSLP_PROCESS_MSG

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to SLP.

DEPENDENCIES
  None
  
PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_process_msg
(
  dsm_item_type* msg_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
);

/*===========================================================================

FUNCTION HDRSLP_RESET

DESCRIPTION
  This function performs an SLP reset (flushing buffers and seq. #'s)

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  Rexmit and reassembly buffers get flushed, all sequence variables
  are reinitialized.
===========================================================================*/

void hdrslp_reset( void );

/*===========================================================================

FUNCTION HDRSLP_SEND_RESET_ACK

DESCRIPTION
  This function sends a ResetAck message with the given sequence number.

DEPENDENCIES
  None
  
PARAMETERS
  seqno - Reset sequence number for this transaction

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_send_reset_ack
(
  uint8 seqno
);

/*===========================================================================

FUNCTION HDRSLP_FLUSH_REASSEMBLY_BUF

DESCRIPTION
  This function flushs reassembly buffer with the given buffer index.

DEPENDENCIES
  None
  
PARAMETERS
  Index - Buffer index. [0] for unicast fragments and [1] for brdcst

RETURN VALUE
  None
  
SIDE EFFECTS
  reassembly buffer gets flushed
===========================================================================*/

void hdrslp_flush_reassembly_buf
(
  uint8 index
);

/*===========================================================================

FUNCTION HDRSLP_RESET_STATS

DESCRIPTION
  This function resets all of SLP's stat counters to zero

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_reset_stats( void );

/*===========================================================================

FUNCTION HDRSLP_LOG_MSG

DESCRIPTION
  This function logs a signaling message, either receive or transmit.

DEPENDENCIES
  None
  
PARAMETERS
  link_id - Link ID of the link message received/transmitted on
  sdu_length - Including HMP header but NOT SLP header
  sdu_offset - (AKA SLP header size) Offset of first byte of SDU (protocol ID)
  is_broadcast - If the message is a received broadcast message
  channel - channel message was received/transmitted on
  reliable - If the transmission is reliable or best effort
  fragmented - If this SDU is fragmented
  seq_no - For reliable delivery
  ack_seq_no - For reliable delivery
  ack_seq_valid - Whether the ack seq no given is a valid seq no
  msg_ptr - DSM item whose data INCLUDES the SLP header

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

extern void hdrslp_log_msg
(
  hdrsrch_link_id_type link_id,
  uint16         sdu_length,
  uint8          sdu_offset,
  boolean        is_broadcast,
  hdrhai_channel_enum_type channel,
  boolean        scc_msg,
  boolean        reliable,
  boolean        fragmented,
  uint8          seq_no,
  uint8          ack_seq_no,
  boolean        ack_seq_no_valid,
  dsm_item_type *msg_ptr
);

/* <EJECT> */   
/*=========================================================================

FUNCTION HDRSLP_LOG_CONN_STATE_STATS

DESCRIPTION  
  This function logs the connected state stats. It gets a 
  log record from the diag log buffer, fills it with the latest 
  connected state info and commits the log record to
  the diag for delivery.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

=========================================================================*/
void hdrslp_log_conn_state_stats( void );

/* <EJECT> */   
/*=========================================================================

FUNCTION HDRSLP_LOG_INC_CONN_STATE_STATS_COUNTER

DESCRIPTION
  This function increments the appropriate connected
  state counter by the given amount.

DEPENDENCIES
  None
  
PARAMETERS
  counter - The counter to increment.
  inc_count - How much to increment the counter by.

RETURN VALUE
  None
  
SIDE EFFECTS
  None

=========================================================================*/
void hdrslp_log_inc_conn_state_stats_counter
( 
  hdrslp_log_conn_state_stats_count_type counter,
  uint16                                 inc_count
);

/* <EJECT> */   
/*=========================================================================

FUNCTION HDRSLP_LOG_RESET_CONN_STATE_STATS

DESCRIPTION 
  This function resets the SLP connected state stats as
  specified by the stats reset parameter.

DEPENDENCIES
  None
  
PARAMETERS
  stats_reset - Which statistics to clear... all or just the ones
                related to the last call.

RETURN VALUE
  None
  
SIDE EFFECTS
  None

=========================================================================*/
void hdrslp_log_reset_conn_state_stats
( 
  hdrslp_reset_stats_type stats_reset
);

#endif /* HDRSLPI_H */
