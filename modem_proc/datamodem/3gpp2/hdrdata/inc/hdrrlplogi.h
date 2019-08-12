#ifndef HDRRLPLOGI_H
#define HDRRLPLOGI_H

/*===========================================================================

          R A D I O   L I N K   P R O T O C O L LOG
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Multiflow Radio Link Protocol.

Copyright (c) 2004 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrrlplogi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/11/11   vpk    Created file.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_HDR_QOS
#include "errno.h"
#include "hdrmrlp.h"


/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
#if defined(FEATURE_HDRRLP_LOGGING)

/* TODO: remove this once HDR team removes log definitions*/
#ifndef LOG_HDR_RLP_STATS_C

/*---------------------------------------------------------------------------
                          LOG_HDR_RLP_STATS_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_RLP_STATS_C)
  uint8   service_id;             /* Service ID. Identifies RLP Service */
  qword   reset_time;             /* Timestamp for last statistic reset */
  uint32  naks_received;          /* Total # of nak frames received     */
  uint32  re_xmits_not_found;     /* Number of Retransmits not found    */
  uint64  an_nak_bytes_requested; /* Total # of bytes requested by AN NAKs */
  uint64  rx_duplicate_bytes;     /* # of received duplicate frames */
  uint64  rx_rexmit_bytes;        /* # of received re-transmitted frames */
  uint64  rx_new_data_bytes;      /* Total # of received new data frames */
  uint64  rx_total_bytes;         /* Total # of bytes received */
  uint32  tx_naks_cnt;            /* Number of NAKs transmitted */
  uint32  at_nak_bytes_requested; /* Total # of bytes requested by AT NAKs */
  uint64  tx_rexmit_bytes;        /* # of transmitted re-transmitted bytes */
  uint64  tx_new_data_bytes;      /* Total # of new transmitted data bytes */
  uint64  tx_total_bytes;         /* Total # of bytes transmitted */
  uint32  aborts;                 /* NAK timeouts */
  uint32  resets;                 /* Total resets, initiated by either AN,AT */
  uint32  at_reset_request_count; /* # of resets requested by AT */
  uint32  an_reset_ack_count;     /* # of Acks received from AN */
  uint32  an_reset_request_count; /* # times reset was requested by AN */
LOG_RECORD_END


/*---------------------------------------------------------------------------
                          LOG_HDR_MRLP_FWD_STATS_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_MRLP_FWD_STATS_C)
  uint8   version;                /* Version number */
  uint8   rlp_flow;               /* called RLP ID Identifies RLP Service */
  qword   reset_time;             /* Timestamp for last statistic reset */
  uint32  re_xmits_not_found;     /* Number of Retransmits not found    */
  uint64  at_nak_bytes_requested; /* Total # of bytes requested by AT NAKs */
  uint64  rx_rexmit_bytes;        /* # of received re-transmitted frames */
  uint64  rx_new_data_bytes;      /* Total # of received new data frames */
  uint64  rx_total_bytes;         /* Total # of bytes received */
  uint32  aborts;                 /* called NAK timeouts */
  uint32  resets;                 /* called resetcount - Total resets, initiated by either AN,AT */
  uint32  at_reset_request_count; /* # of resets requested by AT */
  uint32  an_reset_ack_count;     /* # of Acks received from AN */
  uint32  an_reset_request_count; /* # times reset was requested by AN */
LOG_RECORD_END


/*---------------------------------------------------------------------------
                          LOG_HDR_ENHANCED_MRLP_FWD_STATS_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_ENHANCED_MRLP_FWD_STATS_C)
  uint8   version;                   /* Version number */
  uint8   link_flow;                 /* Link Flow ID */
  uint8   route;                     /* Route A or Route B */

#ifdef FEATURE_HDR_QOS
  uint8   flow_protocol;             /* Flow protocol */
  uint8   route_protocol;            /* Route protocol */
#endif /* FEATURE_HDR_QOS */

  uint8   packet_stream;             /* PDU type that link flow provides to the flow protocol & route protocol */
  uint16  segment_sequencing;        /* Data unit type for the link flow */
  qword   reset_time;                /* Timestamp for last statistic reset */
  uint64  at_nak_bytes_requested;    /* # of bytes requested by AT NAKs */
  uint64  at_nak_records_requested;  /* # of NAK records AT sent out */
  uint64  rx_rexmit_bytes;           /* # of bytes of re-transmitted data received */
  uint64  rx_rexmit_frames;          /* # of RLP frames of re-transmitted data received */
  uint64  rx_new_data_bytes;         /* # of bytes of new data received */
  uint64  rx_new_data_frames;        /* # of RLP frames of new data received */
  uint64  rx_total_bytes;            /* Total # of bytes received */
  uint64  rx_total_frames;           /* Total # of RLP frames received */
  uint64  rx_first_data_unit;        /* # of RLP frames received with first_data_unit flag on */
  uint64  rx_last_data_unit;         /* # of RLP frames received with last_data_unit flag on */
  uint32  aborts;                    /* called NAK timeouts */
  uint32  resets;                    /* called resetcount - Total resets, initiated by either AN,AT */
  uint32  at_reset_request_count;    /* # of resets requested by AT */
  uint32  an_reset_ack_count;        /* # of reset Acks received from AN */
  uint32  an_reset_request_count;    /* # times reset was requested by AN */
LOG_RECORD_END


/*---------------------------------------------------------------------------
                          LOG_HDR_MRLP_REV_STATS_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_MRLP_REV_STATS_C)
  uint8   version;                /* Version number */
  uint8   rlp_flow;               /* Identifies RLP Service */
  qword   reset_time;             /* Timestamp for last statistic reset */
  uint32  re_xmits_not_found;     /* Number of Retransmits not found    */
  uint64  an_nak_bytes_requested; /* Total # of bytes requested by AN NAKs */
  uint64  tx_rexmit_bytes;        /* # of transmitted re-transmitted bytes */
  uint64  tx_new_data_bytes;      /* Total # of new transmitted data bytes */
  uint64  tx_total_bytes;         /* Total # of bytes transmitted */
  uint32  resets;                 /* called resetcount - Total resets, initiated by either AN,AT */
  uint32  at_reset_request_count; /* # of resets requested by AT */
  uint32  an_reset_ack_count;     /* # of Acks received from AN */
  uint32  an_reset_request_count; /* # times reset was requested by AN */
LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_ENHANCED_MRLP_REV_STATS_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_ENHANCED_MRLP_REV_STATS_C)
  uint8   version;                  /* Version number */
  uint8   link_flow;                /* Link flow ID */
  uint8   route;                    /* Route A or Route B */
  uint8   flow_protocol;            /* Flow protocol */
  uint8   route_protocol;           /* Route protocol */
  uint8   packet_stream;            /* PDU type that link flow provides to the flow protocol & route protocol */
  uint16  segment_sequencing;       /* Data unit type for the link flow */
  qword   reset_time;               /* Timestamp for last statistic reset */
  uint32  re_xmits_not_found;       /* Number of Retransmits not found    */
  uint64  an_nak_bytes_requested;   /* # of bytes requested by AN NAKs */
  uint64  an_nak_records_requested; /* # of NAK records AN requested */
  uint64  tx_rexmit_bytes;          /* # of bytes re-transmitted by AT */
  uint64  tx_rexmit_frames;         /* # of RLP frames re-transmitted by AT */
  uint64  tx_new_data_bytes;        /* # of bytes of new data transmitted by AT */
  uint64  tx_new_data_frames;       /* # of RLP frames of new data transmitted by AT */
  uint64  tx_total_bytes;           /* Total # of bytes transmitted */
  uint64  tx_total_frames;          /* Total # of RLP frames transmitted */
  uint64  tx_first_data_unit;       /* # of RLP frames transmitted with first_data_unit flag on */
  uint64  tx_last_data_unit;        /* # of RLP frames transmitted with last_data_unit flag on */
  uint32  resets;                   /* called resetcount - Total resets, initiated by either AN,AT */
  uint32  at_reset_request_count;   /* # of resets requested by AT */
  uint32  an_reset_ack_count;       /* # of Acks received from AN */
  uint32  an_reset_request_count;   /* # times reset was requested by AN */
LOG_RECORD_END


#if defined(FEATURE_HDRRLP_LOGGING)
/*---------------------------------------------------------------------------
                    HDR   RLP PARTIAL FRAMES LOG TYPE
---------------------------------------------------------------------------*/
/* This is a high volume accumulated log.  Many fields are bit stuffed.

Physical Channel rate decoding info

0x0    =     Undefined
0x1    =     9.6
0x2    =    19.2
0x3    =    38.4
0x4    =    76.8
0x5    =   153.6
0x6    =   307.2 Short
0x7    =   307.2 Long
0x8    =   614.4 Short
0x9    =   614.4 Long
0xa    =   921.6
0xb    =   1.2288 Short
0xc    =   1.2288 Long
0xd    =   1.843  Short
0xe    =   1.843  Long
0xf    =   2.456

-----------------------------------------------------------------------------*/

/* worst case rx packet can be 4608 ( 512 * 9 ), and the tx packet size
is 900 ( 100 * 9 ) here 9 = payload ( 6 bytes ) + info ( 3 bytes )   */


#define LOG_MAX_HDRRLP_RX_PKT 4608
#define LOG_MAX_HDRRLP_TX_PKT 900

typedef PACK(struct)
{
  PACK(struct)
  {
    uint16 phy_chan_rate : 4;       /* Physical Channel rate          */
    uint16 channel       : 1;       /* 0 = Traffic, 1 = Contro        */
    uint16 pkt_len       :10;       /* Entire RLP Packet Len          */

    uint16 rsvd          : 1;       /* Reserved Bit                   */
  } pkt_info;

  PACK(union) {
    uint8  frame_count      ;      /* Increments of 26.67 msec       */
    uint8  STR              ;   /* Slot Timer ref, Inc of 1.667 msec */
  } time_ref;

  byte hdrrlp_payload[1];                    /* Actual Payload bytes */

} log_hdrrlp_pkt_type;

/* Partial RLP frames log record.*/

LOG_RECORD_DEFINE(LOG_HDR_RLP_PARTIAL_RX_C)

  PACK(struct)
  {
    uint8         service_ID:2;          /* Service ID                 */
    uint8         header_len:6;          /* Number of RLP bytes logged */
  } pkt_block_info;

  uint8         count;            /* Count of partial RLP packets    */


  PACK(union) {

    log_hdrrlp_pkt_type pkt[1];
    byte                logbuf[LOG_MAX_HDRRLP_RX_PKT];
                                      /* Accumulated partial RLP packets
                                         defined in log_rlp_pkt_type     */
  } hdrrlp_log_buf;

LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_HDR_RLP_PARTIAL_TX_C)

  PACK(struct)
  {
    uint8         service_ID:2;         /* Service ID                 */
    uint8         header_len:6;         /* Number of RLP bytes logged */
  }pkt_block_info;

  uint8         count;           /* Count of partial RLP packets    */


  PACK(union) {

    log_hdrrlp_pkt_type pkt[1];
    byte                logbuf[LOG_MAX_HDRRLP_TX_PKT];
                                    /* Accumulated partial RLP packets
                                       defined in log_rlp_pkt_type     */
  } hdrrlp_log_buf;

LOG_RECORD_END

typedef LOG_HDR_RLP_PARTIAL_RX_C_type log_hdrrlp_frames_rx_type;
typedef LOG_HDR_RLP_PARTIAL_TX_C_type log_hdrrlp_frames_tx_type;

#endif /* FEATURE_HDRRLP_LOGGING  */


/* ----------------MRLP Logging support---------------------------- */
#define LOG_MAX_HDRMRLP_RX_PKT 640

/*
 * The reason for defining the LOG_MAX_HDRMRLP_TX_PKT is as follows:
 * The max UL RLP packet size is 636 bytes. In the tx partial log packet,
 * we have a union of a structure (header info) and some logging buffer.
 * The header size varies for different packet apps. Initially this macro
 * was set to 640 when Rev A was implemented. This accounted for 636 bytes
 * for RLP packet and 4 bytes of header. Later the MPA Partial TX packet
 * header was changed to 5 bytes. Also, for EMPA and MMPA, the sum total
 * of full RLP packet size and the header size is more than 640. This results
 * in memory scribble when we do full packet logging on the tx path
 * In orderto avoid maintaining different macros for log packet of each packet app,
 * we are increasing it to 650 byes. This mandates that the header for the
 * Tx Partial Log packet for any packet app cannot grow beyond 14 bytes.
 */
#define LOG_MAX_HDRMRLP_TX_PKT 650

#define LOG_MAX_HDRMRLP_MULTILINK_RX_PKT 1024
/* In Rev A, the RLP size   = 636 Bytes
        Packet Info         =   4 Bytes
        --------------------    ------
        Total               =  640 Bytes

If the request is full logging and packet size is 636 bytes (as per Rev A),
then one RLP packet will be logged and shipped to diag.

*/

/*==========  MRLP RX =================================== */
/*-------------------------------------------------------------------------
                    HDR MRLP RX PARTIAL FRAMES LOG TYPE
---------------------------------------------------------------------------
Physical Channel rate decoding info

0x0    =   Undefined
0x1    =   38.4         Kbps
0x2    =   76.8         Kbps
0x3    =   153.6        Kbps
0x4    =   307.2 Short  Kbps
0x5    =   307.2 Long   Kbps
0x6    =   614.4 Short  Kbps
0x7    =   614.4 Long   Kbps
0x8    =   921.6        Kbps
0x9    =   1.2288 Short Mbps
0xa    =   1.2288 Long  Mbps
0xb    =   1.843        Mbps
0xc    =   2.456        Mbps
0xd    =   1.536        Mbps
0xe    =   3.072        Mbps
-----------------------------------------------------------------------------*/
typedef PACK(struct)
{
  PACK(struct)
  {
    uint32  phy_chan_rate : 4;   /* Physical Channel rate             */
    uint32  channel       : 1;   /* 0 = Traffic, 1 = Contro           */
    uint32  pkt_len       :13;   /* Entire RLP Packet Len             */
    uint32  rlp_id_len    : 6;   /* Length of the RLP ID              */
  } pkt_info;

  uint16   time_ref;             /* Slot Timer ref,Inc of 1.667 msec */
  byte    hdrrlplog_payload[1];   /* Actual Payload bytes             */
} log_hdrrlplog_rx_pkt_type;

/* Partial MRLP frames log record.*/
LOG_RECORD_DEFINE(LOG_HDR_MRLP_RX_PKTS_C)
  PACK(struct)
  {
    uint8    rlp_flow_id  :5;     /* RLP FLOW ID                      */
    uint8    rsvd         :2;     /* Reserved Bits, internal tagging  */
    uint8    seq_len      :1;     /* if 1 then 22, else 14            */
    uint8    pkt_length     ;     /* Header length                    */
  } pkt_block_info;

  uint8    NumPkts;             /* Number of partial RLP packets    */

  PACK(union)
  {
    log_hdrrlplog_rx_pkt_type pkt[1];
    byte   logbuf[LOG_MAX_HDRMRLP_RX_PKT];
                               /* Accumulated partial RLP packets  */
  } hdrrlplog_log_buf;

LOG_RECORD_END


/*---------------------------------------------------------------------------
                          LOG_HDR_ENHANCED_MRLP_RX_PKTS_C
---------------------------------------------------------------------------*/
typedef PACK(struct)
{
  uint16  frame_len;        /* Entire RLP Frame Length          */
  uint16  time_ref;         /* Slot Timer ref,Inc of 1.667 msec */
  byte    frame_payload[1]; /* Actual RLP Payload bytes         */
} log_enhanced_hdrrlplog_rx_frame_type;

/* Enhanced Partial MRLP frames log record.*/
LOG_RECORD_DEFINE(LOG_HDR_ENHANCED_MRLP_RX_PKTS_C)
  PACK(struct)
  {
    uint16    link_flow_id        :5;   /* link FLOW ID                     */
    uint16    rsvd                :6;   /* Reserved Bits, internal tagging  */
    uint16    seq_len             :2;   /* Sequence length 22,  14 or 6     */
    uint16    route               :1;   /* Route A or B */
    uint16    packet_stream       :1;   /* PDU type that link flow provides to the flow protocol & route protocol */
    uint16    segment_sequencing  :1;   /* Data unit type for the link flow */
    uint8    partial_frame_length;      /* Length of the partial RLP frame, 0 is the full length */
    uint8    num_frames;                /* Number of partial RLP frames in this log record */
  } log_buffer_info;

  PACK(union)
  {
    log_enhanced_hdrrlplog_rx_frame_type  partial_rlp_frame[1];  /* Accumulated partial RLP frames */
    byte                  logbuf[LOG_MAX_HDRMRLP_RX_PKT];  /* space reserved for RLP frames*/
  } enhanced_hdrrlplog_log_buf;
LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_MULTILINK_MRLP_RX_PKTS_C
---------------------------------------------------------------------------*/
typedef PACK(struct)
{
  uint16  frame_len;        /* Entire RLP Frame Length          */
  uint16  time_ref;         /* Slot Timer ref,Inc of 1.667 msec */
  uint8   schgrp_id;        /* Scheduler Group on which packet was recieved*/
  uint8   link_id;          /* Link on which packet was recieved */
  byte    frame_payload[1]; /* Actual RLP Payload bytes         */
}log_multilink_hdrrlplog_rx_frame_type;

/* Multilink Multiflow Partial MRLP frames log record.*/
LOG_RECORD_DEFINE(LOG_HDR_MULTILINK_MRLP_RX_PKTS_C)
  PACK(struct)
  {
    uint8    link_flow_id        :5;   /* link FLOW ID                     */
    uint8    route               :1;   /* Route A or B */
    uint8    packet_stream       :1;   /* PDU type that link flow provides to the flow protocol & route protocol */
    uint8    segment_sequencing  :1;   /* Data unit type for the link flow */

    uint16    sar_seq_len        :5;   /* SAR Sequence Length */
    uint16    sar_seq_len_short  :5;   /* SAR Sequence Length Short */
    uint16    buffer_alloc       :1;   /* Buffer Allocated (Housekeeping only*/
    uint16    rsvd               :5;   /* Reserved */

    uint16     partial_frame_length;   /* Length of the partial RLP frame, 0 is the full length else is NV item value*/
    uint8     num_frames;              /* Number of partial RLP frames in this log record */
  } log_buffer_info;

  PACK(union)
  {
    log_multilink_hdrrlplog_rx_frame_type   partial_rlp_frame[1];     /* Accumulated partial RLP frames */
    byte                  logbuf[LOG_MAX_HDRMRLP_MULTILINK_RX_PKT]; /* space reserved for RLP frames*/
  }multilink_hdrrlplog_log_buf;
LOG_RECORD_END



/*==========  MRLP TX =================================== */
/*---------------------------------------------------------------------------
                    HDR MRLP TX PARTIAL FRAMES LOG TYPE
---------------------------------------------------------------------------

Physical Channel rate decoding info

0x0    =   Undefined
Others TBD
-----------------------------------------------------------------------------*/
typedef PACK(struct)
{
  PACK(struct)
  {
    uint32  phy_chan_rate       : 6;   /* Physical Channel rate             */
    uint32  channel             : 1;   /* 0 = Traffic, 1 = Contro           */
    uint32  pkt_len             :14;   /* Entire RLP Packet Len             */
    uint32 rlp_id_len_lsb_3bits : 3;   /* Least Significant 3 bits of
                                          the RLP ID Length                 */
  } pkt_info;

  uint16   time_ref;              /* Increments of 26.67 msec          */
  byte    hdrrlplog_payload[1];    /* Actual Payload bytes              */

} log_hdrrlplog_tx_pkt_type;

/* Partial MRLP tx frames log record.*/
LOG_RECORD_DEFINE(LOG_HDR_MRLP_TX_PKTS_C)
  PACK(struct)
  {
  uint8    rlp_flow_id    :5;     /* RLP FLOW ID                      */
  uint8    rsvd           :1;     /* Reserved bits, internal tagging  */
  uint8    seq_len        :1;     /* if 1 then 22, else 14            */
  uint8    msb_rlp_id_len :1;     /* Most Signigicant Bit of the 4bits
                                                     of RLP id length */
  uint8    pkt_length       ;     /* Header length                    */
  }pkt_block_info;

  uint8    NumPkts;             /* Number of partial RLP packets    */

  PACK(union)
  {
    log_hdrrlplog_tx_pkt_type pkt[1];
    byte   logbuf[LOG_MAX_HDRMRLP_TX_PKT];
                        /* Accumulated partial RLP packets          */
  } hdrrlplog_log_buf;
LOG_RECORD_END


/*---------------------------------------------------------------------------
                          LOG_HDR_ENHANCED_MRLP_TX_PKTS_C
---------------------------------------------------------------------------*/
typedef PACK(struct)
{
  uint16  frame_len;        /* Entire RLP Frame Length          */
  uint16  time_ref;         /* Slot Timer ref,Inc of 1.667 msec */
  byte    frame_payload[1]; /* Actual RLP Payload bytes         */
} log_enhanced_hdrrlplog_tx_frame_type;

/* Enhanced Partial MRLP frames log record.*/
LOG_RECORD_DEFINE(LOG_HDR_ENHANCED_MRLP_TX_PKTS_C)
  PACK(struct)
  {
    uint16    link_flow_id      :5;  /* link FLOW ID                     */
    uint16    rsvd              :6;  /* Reserved Bits, internal tagging  */
    uint16    seq_len           :2;  /* Sequence length 22,  14 or 6     */
    uint16    route             :1;  /* Route A or B */
    uint16   packet_stream      :1;  /* PDU type that link flow provides to the flow protocol & route protocol */
    uint16   segment_sequencing :1;  /* Data unit type for the link flow */
    uint8    partial_frame_length;   /* Length of the partial RLP frame, 0 is the full length */
    uint8    num_frames;             /* Number of partial RLP frames in this log record */
  }log_buffer_info;

  PACK(union)
  {
    log_enhanced_hdrrlplog_tx_frame_type  partial_rlp_frame[1];     /* Accumulated partial RLP frames */
    byte                  logbuf[LOG_MAX_HDRMRLP_TX_PKT]; /* space reserved for RLP frames*/
  } enhanced_hdrrlplog_log_buf;
LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_MULTILINK_MRLP_TX_PKTS_C
---------------------------------------------------------------------------*/
typedef PACK(struct)
{
  uint16  frame_len;        /* Entire RLP Frame Length          */
  uint16  time_ref;         /* Slot Timer ref,Inc of 1.667 msec */
  uint8   carrier_id;       /* Carrier Id*/
  uint8   tx_reason;        /* Reason for Tx( Retx/First time etc)*/
  byte    frame_payload[1]; /* Actual RLP Payload bytes         */
} log_multilink_hdrrlplog_tx_frame_type;

/* Multilink Multiflow Partial MRLP frames log record.*/
LOG_RECORD_DEFINE(LOG_HDR_MULTILINK_MRLP_TX_PKTS_C)
  PACK(struct)
  {
    uint16    link_flow_id        :5;  /* link FLOW ID                     */
    uint16    rsvd                :6;  /* Reserved Bits, internal tagging  */
    uint16    seq_len             :2;  /* Sequence length 22,  14 or 6     */
    uint16    route               :1;  /* Route A or B */
    uint16    packet_stream       :1;  /* PDU type that link flow provides to the flow protocol & route protocol */
    uint16    segment_sequencing  :1;  /* Data unit type for the link flow */
    uint16    partial_frame_length;    /* Length of the partial RLP frame, 0 is the full length */
    uint8     num_frames;              /* Number of partial RLP frames in this log record */
  }log_buffer_info;

  PACK(union)
  {
    log_multilink_hdrrlplog_tx_frame_type   partial_rlp_frame[1];     /* Accumulated partial RLP frames */
    byte                  logbuf[LOG_MAX_HDRMRLP_TX_PKT]; /* space reserved for RLP frames*/
  } multilink_hdrrlplog_log_buf;
LOG_RECORD_END

typedef LOG_HDR_MRLP_RX_PKTS_C_type log_multiflow_hdrrlplog_frames_rx_type;
typedef LOG_HDR_MRLP_TX_PKTS_C_type log_multiflow_hdrrlplog_frames_tx_type;
typedef LOG_HDR_ENHANCED_MRLP_RX_PKTS_C_type log_enhanced_hdrrlplog_frames_rx_type;
typedef LOG_HDR_ENHANCED_MRLP_TX_PKTS_C_type log_enhanced_hdrrlplog_frames_tx_type;
typedef LOG_HDR_MULTILINK_MRLP_RX_PKTS_C_type log_multilink_hdrrlplog_frames_rx_type;
typedef LOG_HDR_MULTILINK_MRLP_TX_PKTS_C_type log_multilink_hdrrlplog_frames_tx_type;


/*---------------------------------------------------------------------------
                          LOG_HDR_MULTILINK_MRLP_FWD_STATS_C
---------------------------------------------------------------------------*/

#define LOG_HDR_MAX_QN_INSTANCES   18

typedef PACK(struct)
{
  uint8   sched_group;          /* Scheduler group id of this QN instance */
  uint8   num_carriers;         /* # of carriers in the QN instance */
  uint64  qn_id;                /* Unique QN id */
  uint64  qn_create_time;       /* Creation time of this QN instance */
  uint64  qn_delete_time;       /* Deletion time of this QN instance */
  uint64  rx_new_data_bytes;    /* # of new bytes recvd */
  uint64  rx_new_data_frames;   /* # of new frames recvd */
  uint32  quick_nak_ind_cnt;    /* # of QuickNak indication generated */

} log_multilink_mrlp_qn_stats_type;


LOG_RECORD_DEFINE(LOG_HDR_MULTILINK_MRLP_FWD_STATS_C)
  uint8   version;                    /* Version number */
  uint16  link_flow          :5;      /* Link Flow ID */
  uint16  route              :1;      /* Route A or Route B */
  uint16  flow_protocol      :4;      /* Flow protocol */
  uint16  route_protocol     :4;      /* Route protocol */
  uint16  packet_stream      :1;      /* PDU type that link flow provides to the flow protocol & route protocol */
  uint16  segment_sequencing :1;      /* Data unit type for the link flow */
  uint8   seq_len;                    /* SAR sequence length */
  uint8   seq_len_short;              /* SAR sequence length short */
  qword   reset_time;                 /* Timestamp for last statistic reset */
  uint64  at_quick_nak_ind_cnt;       /* # of quicknak indications generated */
  uint64  at_quick_nak_cnt;           /* # of Quick NAK records AT sent out */
  uint64  at_delayed_nak_cnt;         /* # of Delayed NAK records AT sent out */
  uint64  at_delayed_nak_data_unit_cnt; /* # of bytes requested by AT Delayed NAKs */
  uint64  rx_rexmit_bytes;            /* # of bytes of re-transmitted data received */
  uint64  rx_rexmit_frames;           /* # of RLP frames of re-transmitted data received */
  uint64  rx_new_data_bytes;          /* # of bytes of new data received */
  uint64  rx_new_data_frames;         /* # of RLP frames of new data received */
  uint64  rx_first_data_unit;         /* # of RLP frames received with first_data_unit flag on */
  uint64  rx_last_data_unit;          /* # of RLP frames received with last_data_unit flag on */
  uint64  rx_dup_data_bytes;          /* # of duplicate bytes received */
  uint64  rx_dup_data_frames;         /* # of duplicate frames received */
  uint32  aborts;                     /* called NAK timeouts */
  uint64  nak_abort_data_units_approx; /* approx because we dont need to find what we are exactly missing from V(n) to lastseq */
  uint32  resets;                    /* called resetcount - Total resets, initiated by either AN,AT */
  uint32  at_reset_request_count;    /* # of resets requested by AT */
  uint32  an_reset_ack_count;        /* # of reset Acks received from AN */
  uint32  an_reset_request_count;    /* # times reset was requested by AN */
  qword   last_rlp_reset_time;       /* last time this rlp instance was reset */
  uint32  rx_reseq_q_size;           /* maximum size of reseq_q in the last logging interval */
  uint32  rx_nak_abort_q_size;       /* maximum size of nak_abort_q in the last logging interval */
  uint32  rx_delayed_nak_q_size;     /* maximum size of delayed_nak_q in the last logging interval */
  uint8   num_qn_instances;          /* # of qn_info included in this log record */
  log_multilink_mrlp_qn_stats_type qn_info[ LOG_HDR_MAX_QN_INSTANCES ];

LOG_RECORD_END


/*---------------------------------------------------------------------------
                          LOG_HDR_MULTILINK_MRLP_RX_PROCESSING_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_MULTILINK_MRLP_RX_PROCESSING_C)
  uint8   version;                /* Version number */
  uint8   event_type  :2;         /* 0:Pkt reception 1:RLP reset 2:Abort 3:Flush message */
  uint8   link_flow   :5;         /* Link Flow ID */
  uint8   route       :1;         /* Route A or Route B */
  uint32  vn_before;              /* V(n) before processing the event */
  uint32  vn_after;               /* V(n) after processing the event */
  uint32  vr_before;              /* V(r) before processing the event */
  uint32  vr_after;               /* V(r) after processing the event */
  uint8   link_id;                /* Link ID on which pkt was recvd */
  uint8   scheduler_group_id :6;  /* Scheduler group id on which pkt was recvd */
  uint8   qn_seq_included    :1;  /* QN seq included in packet */
  uint8   rsvd               :1;  /* reserved */
  uint16  num_data_units;         /* # of data units in packet */
  uint32  sar_seq_num;            /* Interpreted SAR Seq number */
  uint16  qn_seq_num;             /* If qn_seq_included, QN seq number */
  uint32  sar_seq_short_num;      /* If qn_seq_included, SAR seq num short */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_MULTILINK_MRLP_REV_STATS_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_MULTILINK_MRLP_REV_STATS_C)

  uint8   version;                /* Version number */
  uint16  link_flow     :5;       /* Link Flow ID */
  uint16  route         :1;       /* Route Number */
  uint16  flow_type     :1;       /* Whether link flow is packet based, packet_stream */
  uint16  sequence_type :1;       /* Whether link flow is doing segment based
                                     sequencing */
  uint16  flow_protocol :4;       /* Flow Protocol */
  uint16  route_protocol:4;       /* Route Protocol */
  uint8   sequence_length;        /* Sequence Length */
  uint64  tx_new_data_bytes;      /* Number of new data bytes sent */
  uint64  tx_new_data_units;      /* Number of new data units sent */
  uint64  tx_fdu_frames;          /* Number of first data unit frames sent */
  uint64  tx_ldu_frames;          /* Number of last data unilw frames sent */
  uint64  num_marq_instances;     /* Number of PhysicalLayerNak indications
                                     received by RLP */
  uint64  num_new_data_units_marqed;/* Number of new data units indicated by
                                       PhysicalLayerNak indications */
  uint64  num_retx_data_units_marqed;/* Number of retx data units indicated by
                                       PhysicalLayerNak indications */
  uint64  num_ooo_instances;      /* Number of occurences of out of order
                                     transmissions */
  uint64  num_bytes_tx_ooo;       /* Number of bytes transmitted out of order*/
  uint64  num_spd_instances;      /* Number of higher layer packets dropped due
                                     to TransmitAbortTimer */
  uint64  num_spd_bytes;          /* Number of bytes dropped due to
                                     TransmitAbortTimer */
  uint64  num_nak_records_rcvd;   /* Number of nak records received */
  uint64  num_nak_data_units_requested; /* Number of nak data units requested */
  uint64  num_retx_data_units;    /* Number of data units retransmitted */
  uint64  num_retx_bytes;         /* Number of bytes retransmitted */
  uint64  num_retx_not_found;     /* Number of instances retransmit data units
                                     were not found */
  qword  last_rlp_reset_time;    /* Last time the RLP was reset by of AN or AT */
  qword  last_stats_reset_time;  /* Last time the stats were reset  */
  uint32  at_reset_request_count; /* Number of times AT requested for reset */
  uint32  an_reset_request_count; /* Number of times AN requested for reset */
  uint16  max_tx_q_size;          /* Max Transmit queue size within logging interval */
  uint16  max_retx_q_size;        /* Max Retransmit queue size within logging interval*/
  uint16  max_new_data_handles_used; /* Max number of new data handles used per slot
                                        within logging interval */
  uint16  max_retx_data_handles_used;/* Max number of retx data handles used per slot
                                        within logging interval */

LOG_RECORD_END


#endif /* LOG_HDR_RLP_STATS_C */

#endif /* FEATURE_HDRRLP_LOGGING */
#endif /* FEATURE_HDR_QOS */
#endif /* HDRRLPLOGI_H */
