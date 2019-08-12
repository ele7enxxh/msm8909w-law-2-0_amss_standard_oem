#ifndef DSRLP_STATS_H
#define DSRLP_STATS_H
/*===========================================================================

                D S R L P  S T A T S     H E A D E R    F I L E

DESCRIPTION
  
  This header file contains all the extern definitions related to RLP
  logging. Any callers of these functions need to include this header file. 
 
EXTERNALIZED FUNCTIONS
 
  dsrlp_clear_rlp_statistics()
    Clears the current values of the RLP statistics on command 
    from the off-target diagnostic tool.
  
  dsrlp_log_rlp_statistics()
    Logs the current values of the RLP statistics to the off-target 
    diagnostic tool.
  
  dsrlp_log_rx_init_frm_buf()
    Inits the log_rx_rlp_frm_buf data structure.

  dsrlp_log_tx_init_frm_buf()
    Inits the dsrlp_log_tx_frm_buf data structure.

  dsrlp_log_rx_init_frame()
    Initializes the next frame packet, as well as the pointer to it,
    in the RLP Rx frames log buffer.

  dsrlp_log_tx_init_frame()
    Initializes the next frame packet, as well as the pointer to it,
    in the RLP Tx frames log buffer.

  dsrlp_log_rx_send_frames()
    Send the accumulated RLP Rx partial log.

  dsrlp_log_tx_send_frames()
    Send the accumulated RLP Tx partial log.

  ds_log_rlp_frames_type *dsrlp_get_log_rx_rlp_frm_buf()
    Accessor method for dsrlp_log_rx_rlp_frm_buf

  ds_log_rlp_frames_type *dsrlp_get_log_tx_rlp_frm_buf()
    Accessor method for dsrlp_log_tx_rlp_frm_buf

  
Copyright (c) 2003 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/dsrlp_stats.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ----------------------------------------------------------
02/04/11     op     Data SU API cleanup
06/17/10     ms     Fix RLP Partial logging Crash issue on Q6.
10/28/09     vs     Removed the inclusion of customer.h
05/11/09     ms     Included Required Diag files.
05/07/04     atp    Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "diagpkt.h"
#include "log.h"
#include "diagcmd.h"
#include "cdma2kdiag.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
 /*---------------------------------------------------------------------------
                        RLP PARTIAL FRAMES LOG TYPE
 ---------------------------------------------------------------------------*/
 /* This is a high volume accumulated log.  Many fields are bit stuffed. */
 
#define DS_LOG_MAX_RLP_PKT 300
 
 typedef enum {
   DS_LOG_RLP_INVALID = -1, /* Invalid Channel              */
   DS_LOG_RLP_FCH     = 0,  /* Fundamental Channel         */
   DS_LOG_RLP_DCCH    = 1,  /* Dedicated Control Channel    */
   DS_LOG_RLP_SCH_0   = 2,  /* Supplemental Channel 0       */
   DS_LOG_RLP_SCH_1   = 3,  /* Supplemental Channel 1       */
   DS_LOG_RLP_SCCH_0  = 4,  /* IS95B Supplemental Channel 0 */
   DS_LOG_RLP_SCCH_1  = 5,  /* IS95B Supplemental Channel 1 */
   DS_LOG_RLP_SCCH_2  = 6,  /* IS95B Supplemental Channel 2 */
   DS_LOG_RLP_SCCH_3  = 7,  /* IS95B Supplemental Channel 3 */
   DS_LOG_RLP_SCCH_4  = 8,  /* IS95B Supplemental Channel 4 */
   DS_LOG_RLP_SCCH_5  = 9,  /* IS95B Supplemental Channel 5 */
   DS_LOG_RLP_SCCH_6  = 10, /* IS95B Supplemental Channel 6 */
   DS_LOG_RLP_SCCH_7  = 11, /* IS95B Supplemental Channel 7 */
   DS_LOG_RLP_PDCH    = 12, /* EVDV Pkt Data Channel        */
 
   DS_LOG_RLP_MAX     = 15  /* Values 13-15 reserved        */
 } ds_log_rlp_phy_channel_enum_type;
 
 /* Channel frame entry for partial RLP frame.
 */
 /* Plans exist to change this format slightly! */
 typedef PACKED struct PACKED_POST{
   PACKED struct PACKED_POST{
     byte phy_chan : 4;       /* ds_log_rlp_phy_channel_enum_type  */
     byte mux_pdu  : 3;       /* Use mux_pdu_type 0-7           */
                              /* 0 == "mux_pdu1"                */
                              /* 1 == "mux_pdu2"                */
     byte rsvd     : 1;
   } mux_chan;
 
   PACKED struct PACKED_POST {
     word phy_chan_rate : 14; /* Physical Channel Rate / 100    */
     word time_delta    : 1;  /* 0 if same frame,
                                 1 if next frame                */
     word rsvd          : 1;
   } time_rate;
 
   word frame_len;            /* Length, in bits, of this frame. */
                              /* 0 for blank, 0xFFFF for erasure.*/
 
   PACKED struct PACKED_POST {
     byte tail :2;            /* Number of tail payload bytes    */
     byte head :6;            /* Number of head payload bytes    */
 
   } payload_len;
 
   byte payload[1];           /* Head and tail payload bytes,
                                 length given above.             */
 }ds_log_rlp_pkt_type;
 
 /* Partial RLP frames log record.
 */
 /* LOG_RLP_RX_FRAMES_C */
 typedef PACKED struct PACKED_POST
 {
   log_hdr_type hdr;
 
   byte         service_ID; /* Service ID                      */
   byte         count;      /* Count of partial RLP packets    */
 
   PACKED union PACKED_POST{
 
     ds_log_rlp_pkt_type pkt[1];
     byte             buf_ref[DS_LOG_MAX_RLP_PKT];
                            /* Accumulated partial RLP packets
                               defined in ds_log_rlp_pkt_type     */
   } var_len_buf;
 
 }ds_log_rlp_frames_type;
 
 typedef ds_log_rlp_frames_type DS_LOG_RLP_RX_FRAMES_C_type;
 typedef ds_log_rlp_frames_type DS_LOG_RLP_TX_FRAMES_C_type;
 
#define DSRLP_SCH_PAYLOAD_LOG_LEN 5
 
#define DSRLP_LOG_RX_FRAMES_OFFSET \
   (dsrlp_log_rx_rlp_frm_buf.hdr.len - FPOS(ds_log_rlp_frames_type, var_len_buf))
 
#define DSRLP_LOG_TX_FRAMES_OFFSET \
   (dsrlp_log_tx_rlp_frm_buf.hdr.len - FPOS(ds_log_rlp_frames_type, var_len_buf))

#define DS_LOG_MAX_RLP_PAYLOAD_LEN 34
 
#define DS_LOG_MAX_RLP_FRM_SIZE \
   (FPOS(ds_log_rlp_pkt_type, payload) + DS_LOG_MAX_RLP_PAYLOAD_LEN)
 
 /* Threshold to determine when the buf is full and the log should be sent. */
#define DS_LOG_RLP_FRM_BUF_THRESHOLD \
   (sizeof(ds_log_rlp_frames_type) - \
     (FPOS(ds_log_rlp_pkt_type, payload) + DS_LOG_MAX_RLP_PAYLOAD_LEN))
 
 /*---------------------------------------------------------------------------
                          RLP STATISTICS LOG TYPE
 ---------------------------------------------------------------------------*/
#define DS_LOG_RLP_MAX_NAK_ROUNDS  3
 
 /*---------------------------------------------------------------------------
   RLP statistics information definition. This structure contains all of the
   RLP statistical information for the currently active RLP session.
 ---------------------------------------------------------------------------*/
 /* LOG_RLP_STAT_C */
 typedef PACKED struct PACKED_POST
 {
   log_hdr_type hdr;
 
   uint8        service_ID; /* Service ID                          */
   qword        reset_time; /* Timestamp for last statistic reset. */
 
   uint32 naks_received ;           /* total # of nak frames received        */
   uint32 re_xmits_not_found;       /* # of Rexmits not found                */
   uint16 largest_contig_erasure;   /* largest block of consecutive erasures */
   uint8  resets;                   /* # of resets                           */
   uint8  aborts;                   /* # of aborts    (nak aborts)           */
   uint32 last_rtt;                 /* round trip time                       */
   uint8  rlp_blob_used;            /* 0-no BLOB; 1-NAK; 2-RTT; 3-NAK+RTT    */
 
   uint32 rx_rexmit_fr_cnt;         /* # of received re-transmitted frames   */
   uint32 rx_idle_fr_cnt;           /* # of received idle frames             */
   uint32 rx_fill_fr_cnt;           /* # of received fill frames             */
   uint32 rx_blank_fr_cnt;          /* # of blank frames received            */
   uint32 rx_null_fr_cnt;           /* # of null frames received             */
   uint32 rx_new_data_fr_cnt;       /* total # of received new data frames   */
   uint32 rx_20ms_fr_cnt;           /* # of received Fundamental data frames */
   uint32 rx_total_bytes;           /* total # of bytes received             */
   uint32 rx_rlp_erasures;          /* # of rlp erasures received            */
   uint32 rx_mux_erasures;          /* # of mux erasures received            */
 
   uint32 tx_rexmit_fr_cnt;         /* # of transmitted re-transmitted frames*/
   uint32 tx_idle_fr_cnt;           /* # of transmitted idle frames          */
   uint32 tx_new_data_fr_cnt;       /* total # of new transmitted data frames*/
   uint32 tx_20ms_fr_cnt;           /* # of txed Fundamental data frames     */
   uint32 tx_total_bytes;           /* total # of bytes transmitted          */
   uint8  tx_naks_cnt;              /* Number of tx_naks[] entries to follow */
   uint32 tx_naks[DS_LOG_RLP_MAX_NAK_ROUNDS];
                                    /* single, double etc NAKs sent array    */
 
 }ds_log_rlp_stats_type;
 
/* END from log_dmss.h */

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSRLP_CLEAR_RLP_STATISTICS

DESCRIPTION
  Clears the current values of the RLP statistics on command from the
  off-target diagnostic tool.

  Input Parameters:
    *req_ptr: Pointer to DIAG request packet
    pkt_len: Length of DIAG request packet

DEPENDENCIES
  Logging mask, dsrlp_log_stats_interval 

RETURN VALUE
  Pointer to DIAG response packet

SIDE EFFECTS
  None
===========================================================================*/
diag_rsp_type * dsrlp_clear_rlp_statistics
(
  DIAG_RLP_STAT_RESET_F_req_type *req_ptr,
  word pkt_len
);

/*===========================================================================

FUNCTION DSRLP_LOG_RLP_STATISTICS

DESCRIPTION
  Logs the current values of the RLP statistics to the off-target diagnostic
  tool.  If we can't allocate space for the log packet on the DIAG ring
  buffer, we'll abort and try again next time.

  Input Parameters:
    sr_id: The Service ID for the particular service. Primary is usually 1
           and secondary is usually 2.
    callTerminated: True if we're logging at call termination, False if
           we're logging periodically during the call.

DEPENDENCIES
  Logging mask, dsrlp_log_stats_interval 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_log_rlp_statistics
( 
  byte sr_id_index, 
  boolean callTerminated 
);

/*===========================================================================

FUNCTION DSRLP_LOG_RX_INIT_FRM_BUF

DESCRIPTION
  Initializes the dsrlp_log_rx_frm_buf structure 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsrlp_log_rx_init_frm_buf (void);

/*===========================================================================

FUNCTION DSRLP_LOG_TX_INIT_FRM_BUF

DESCRIPTION
  Initializes the dsrlp_log_tx_frm_buf structure

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsrlp_log_tx_init_frm_buf (void);

/*===========================================================================

FUNCTION DSRLP_LOG_RX_INIT_FRAME

DESCRIPTION
  This function initializes the next frame packet, as well as the
  pointer to it, in the RLP Rx frames log buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  An RLP Rx partial frames log may be sent.

===========================================================================*/
ds_log_rlp_pkt_type * dsrlp_log_rx_init_frame (void);

/*===========================================================================

FUNCTION DSRLP_LOG_TX_INIT_FRAME

DESCRIPTION
  This function initializes the next frame packet, as well as the
  pointer to it, in the RLP Tx frames log buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  An RLP Tx partial frames log may be sent.

===========================================================================*/
ds_log_rlp_pkt_type * dsrlp_log_tx_init_frame (void);

/*===========================================================================

FUNCTION DSRLP_LOG_RX_SEND_FRAMES

DESCRIPTION
  Terminate and send the accumulated RLP Rx partial frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  An RLP Rx partial frames log may be sent.

===========================================================================*/
void dsrlp_log_rx_send_frames (void);

/*===========================================================================

FUNCTION DSRLP_LOG_TX_SEND_FRAMES

DESCRIPTION
  Terminate and send the accumulated RLP Tx partial frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  An RLP Tx partial frames log may be sent.

===========================================================================*/
void dsrlp_log_tx_send_frames (void);

/*===========================================================================

FUNCTION DSRLP_GET_LOG_RX_RLP_FRM_BUF

DESCRIPTION
  Accessor method for dsrlp_log_rx_rlp_frm_buf

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to dsrlp_log_rx_rlp_frm_buf structure

SIDE EFFECTS
  None

===========================================================================*/
ds_log_rlp_frames_type *dsrlp_get_log_rx_rlp_frm_buf(void);

/*===========================================================================

FUNCTION DSRLP_GET_LOG_TX_RLP_FRM_BUF

DESCRIPTION
  Accessor method for dsrlp_log_tx_rlp_frm_buf

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to dsrlp_log_tx_rlp_frm_buf structure

SIDE EFFECTS
  None

===========================================================================*/
ds_log_rlp_frames_type *dsrlp_get_log_tx_rlp_frm_buf(void);

#endif /* DSRLP_STATS_H */
