#ifndef HDRRLPLOG_H
#define HDRRLPLOG_H

/*===========================================================================

          M U L T I - F L O W   R A D I O   L I N K   P R O T O C O L 
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Multiflow Radio Link Protocol.

Copyright (c) 2004 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrrlplog.h#1 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/11   vpk     Move hdr log packet definitions to hdrdata
08/16/11   vpk     Add memory log packet support
02/08/11   ls      Global variable cleanup
01/31/11   sa      Made changes for Software decoupling.
09/24/07   da      Created module.
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
#include "hdrrlpi.h"
#include "dsrlpi.h"
#include "hdrrlp.h"
#include "log.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
#if defined(FEATURE_HDRRLP_LOGGING)

#define LOG_HDR_BASE2_C LOG_HDR_RESERVED_CODES_BASE_2_C
#define LOG_HDR_BASE3_C LOG_HDR_RESERVED_CODES_BASE_3_C

#define LOG_HDR_RLP_STATS_C           19 + LOG_HDR_BASE_C

#define LOG_HDR_RLP_PARTIAL_RX_C      37 + LOG_HDR_BASE_C
#define LOG_HDR_RLP_PARTIAL_TX_C      38 + LOG_HDR_BASE_C



/* MRLP related log packets */
#define LOG_HDR_MRLP_REV_STATS_C           0 + LOG_HDR_BASE3_C
#define LOG_HDR_MRLP_FWD_STATS_C           1 + LOG_HDR_BASE3_C
#define LOG_HDR_MRLP_RX_PKTS_C             2 + LOG_HDR_BASE3_C
#define LOG_HDR_MRLP_TX_PKTS_C             3 + LOG_HDR_BASE3_C




/* Enhanced MRLP log packets (652-655) */
#define LOG_HDR_ENHANCED_MRLP_FWD_STATS_C 23 + LOG_HDR_BASE3_C
#define LOG_HDR_ENHANCED_MRLP_REV_STATS_C 24 + LOG_HDR_BASE3_C
#define LOG_HDR_ENHANCED_MRLP_RX_PKTS_C   25 + LOG_HDR_BASE3_C
#define LOG_HDR_ENHANCED_MRLP_TX_PKTS_C   26 + LOG_HDR_BASE3_C


/* Multilink MRLP log packets */
#define LOG_HDR_MULTILINK_MRLP_FWD_STATS_C     48 + LOG_HDR_BASE3_C
#define LOG_HDR_MULTILINK_MRLP_REV_STATS_C     49 + LOG_HDR_BASE3_C
#define LOG_HDR_MULTILINK_MRLP_RX_PKTS_C       50 + LOG_HDR_BASE3_C
#define LOG_HDR_MULTILINK_MRLP_TX_PKTS_C       51 + LOG_HDR_BASE3_C
#define LOG_HDR_MULTILINK_MRLP_RX_PROCESSING_C 52 + LOG_HDR_BASE3_C

/*---------------------------------------------------------------------------
  Rex timer definitions  
---------------------------------------------------------------------------*/

#define HDRRLPLOG_LOG_TX_TIMER_MASK    0x01
#define HDRRLPLOG_LOG_RX_TIMER_MASK    0x02
#define HDRRLPLOG_LOG_STATS_TIMER_MASK 0x04

typedef void (hdrrlplog_notify_callback)(void);


/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/
/*===========================================================================
FUNCTION      GET_HDRRLPLOG_NUM_BYTES_TO_LOG_PTR

DESCRIPTION   Accessor function for hdrrlplog_num_bytes_to_log
  
DEPENDENCIES  None

RETURN VALUE  Address of hdrrlplog_num_bytes_to_log

SIDE EFFECTS  None
===========================================================================*/
uint16* get_hdrrlplog_num_bytes_to_log_ptr 
(
  void
);

/*===========================================================================

FUNCTION HDRRLPLOG_FWD_LOG_STATISTICS

DESCRIPTION
  Logs the current values of the HDR MRLP FWD statistics to the off-target 
  diagnostic tool. If we can't allocate space for the log packet on the DIAG
  ring buffer, we'll abort and try again next time.

  Input Parameters:
    rscb_index - Index of the RLP for which statistics are to be logged

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrlplog_fwd_log_statistics
( 
  uint8 rscb_index,
  byte  route
);
/*===========================================================================

FUNCTION HDRRLPLOG_ENHANCED_FWD_LOG_STATISTICS

DESCRIPTION
  Logs the current values of the HDR ENHANCED MRLP FWD statistics to the off-target 
  diagnostic tool. If we can't allocate space for the log packet on the DIAG
  ring buffer, we'll abort and try again next time.

  Input Parameters:
    rscb_index - Index of the RLP for which statistics are to be logged

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrlplog_enhanced_fwd_log_statistics
( 
  uint8 rscb_index,
  byte  route
);

/*===========================================================================

FUNCTION HDRRLPLOG_MULTILINK_FWD_LOG_STATISTICS

DESCRIPTION
  Logs the current values of the HDR MULTILINK MRLP FWD statistics to the off-target 
  diagnostic tool. If we can't allocate space for the log packet on the DIAG
  ring buffer, we'll abort and try again next time.

  Input Parameters:
    rscb_index - Index of the RLP for which statistics are to be logged

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrlplog_multilink_fwd_log_statistics
( 
  uint8 rscb_index,
  byte  route
);

/*===========================================================================

FUNCTION HDRRLPLOG_MULTILINK_FWD_LOG_RX_PROCESSING

DESCRIPTION
  Logs the LOG_HDR_MULTILINK_MRLP_RX_PROCESSING_C log type to the off-target 
  diagnostic tool.

  Input Parameters:
    rscb_index - Index of the RLP for which statistics are to be logged
    route      - Route index

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrlplog_multilink_fwd_log_rx_processing
( 
  dsrlpi_rlp_ctl_blk_type     *rrcb_ptr       /* RLP-specific ctl block   */
);
/*===========================================================================

FUNCTION HDRRLPLOG_REV_LOG_STATISTICS

DESCRIPTION
  Logs the current values of the HDR MRLP REV statistics to the off-target 
  diagnostic tool. If we can't allocate space for the log packet on the DIAG
  ring buffer, we'll abort and try again next time.

  Input Parameters:
    rscb_index - Index of the RLP for which statistics are to be logged

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrlplog_rev_log_statistics
( 
  uint8 rscb_index,
  byte  route
);
/*===========================================================================

FUNCTION HDRRLPLOG_ENHANCED_REV_LOG_STATISTICS

DESCRIPTION
  Logs the current values of the HDR ENHANCED MRLP REV statistics to the off-target 
  diagnostic tool. If we can't allocate space for the log packet on the DIAG
  ring buffer, we'll abort and try again next time.

  Input Parameters:
    rscb_index - Index of the RLP for which statistics are to be logged

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrlplog_enhanced_rev_log_statistics
( 
  uint8 rscb_index,
  byte  route
);

/*===========================================================================

FUNCTION HDRRLPLOG_MULTILINK_REV_LOG_STATISTICS

DESCRIPTION
  Logs the current values of the HDR MULTILINK MRLP REV statistics to the off-target 
  diagnostic tool. If we can't allocate space for the log packet on the DIAG
  ring buffer, we'll abort and try again next time.

  Input Parameters:
    rscb_index - Index of the RLP for which statistics are to be logged

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrlplog_multilink_rev_log_statistics
( 
  uint8 rscb_index,
  byte  route
);

/*===========================================================================
FUNCTION      HDRRLPLOG_LOG_RX_RLP_FRAMES()

DESCRIPTION   This function LOGS THE PARTIAL RX RLP FRAMES

DEPENDENCIES  None.

RETURN VALUE  Pointer to frame log entry.

SIDE EFFECTS  A MRLP partial frames log may be sent.
===========================================================================*/
void hdrrlplog_log_rx_rlp_frames
(
  dsm_item_type *mrlp_pkt_ptr, 
  dsrlpi_sess_ctl_blk_type *rscb_ptr
);


/*===========================================================================

FUNCTION     HDRRLPLOG_ENHANCED_LOG_RX_RLP_FRAMES()

DESCRIPTION  This function LOGS THE EMPA PARTIAL RX RLP FRAMES

DEPENDENCIES None.

RETURN VALUE Pointer to frame log entry.

SIDE EFFECTS An EMPA RLP partial frames log may be sent.

===========================================================================*/
void hdrrlplog_enhanced_log_rx_rlp_frames
(
  dsm_item_type *mrlp_pkt_ptr,
  dsrlpi_sess_ctl_blk_type *rscb_ptr,
  uint8  route
);

/*===========================================================================

FUNCTION     HDRLOG_MULTILINK_LOG_RX_RLP_FRAMES()

DESCRIPTION  This function LOGS THE MMPA PARTIAL RX RLP FRAMES

DEPENDENCIES None.

RETURN VALUE Pointer to frame log entry.

SIDE EFFECTS A MRLP partial frames log may be sent.

===========================================================================*/
void hdrrlplog_multilink_log_rx_rlp_frames
(
  dsm_item_type *mrlp_pkt_ptr,
  dsrlpi_sess_ctl_blk_type *rscb_ptr,
  uint8  route
);

/*===========================================================================
FUNCTION      HDRRLPLOG_LOG_TX_RLP_FRAMES()

DESCRIPTION   This function LOGS THE PARTIAL TX RLP FRAMES

DEPENDENCIES  None.

RETURN VALUE  Pointer to frame log entry.

SIDE EFFECTS  A MRLP partial frames log may be sent.
===========================================================================*/
void hdrrlplog_log_tx_rlp_frames
(
  dsm_item_type            *mrlp_pkt_ptr,
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr
);

/*===========================================================================
FUNCTION     HDRRLPLOG_ENHANCED_LOG_TX_RLP_FRAMES()

DESCRIPTION  This function LOGS THE EMPA PARTIAL TX RLP FRAMES

DEPENDENCIES None.

RETURN VALUE Pointer to frame log entry.

SIDE EFFECTS An EMPA RLP partial frames log may be sent.
===========================================================================*/
void hdrrlplog_enhanced_log_tx_rlp_frames
(
  dsm_item_type                  *mrlp_pkt_ptr,   
  dsrlpi_rlp_ctl_blk_type        *rrcb_ptr,
  uint8  route
);

/*===========================================================================
FUNCTION     HDRLOG_MULTILINK_LOG_TX_RLP_FRAMES()

DESCRIPTION  This function LOGS THE MMPA PARTIAL TX RLP FRAMES

DEPENDENCIES None.

RETURN VALUE Pointer to frame log entry.

SIDE EFFECTS A MRLP partial frames log may be sent.
===========================================================================*/
void hdrrlplog_multilink_log_tx_rlp_frames
(
  hdrrlp_tx_packet_info_type* tx_packet_info_ptr
);

/*===========================================================================

FUNCTION     HDRLOG_MULTILINK_RX_FRAME_LOG_INIT

DESCRIPTION  This function initializes the next frame packet, as well as 
             the pointer to it, in the MMPA RLP frames log buffer.

DEPENDENCIES None.

RETURN VALUE Pointer to frame log entry.

SIDE EFFECTS An MMPA RLP partial frames log may be sent.

===========================================================================*/
log_multilink_hdrrlplog_rx_frame_type *hdrrlplog_multilink_rx_frame_log_init
(  
  int buffer_id,
  uint16 pkt_len,
  dsrlpi_sess_ctl_blk_type *rscb_ptr
);
/*===========================================================================

FUNCTION     HDRLOG_MULTILINK_TX_FRAME_LOG_INIT

DESCRIPTION  This function initializes the next frame packet, as well as 
             the pointer to it, in the MMPA RLP frames log buffer.

DEPENDENCIES None.

RETURN VALUE Pointer to frame log entry.

SIDE EFFECTS An MMPA RLP partial frames log may be sent.

===========================================================================*/
log_multilink_hdrrlplog_tx_frame_type *hdrrlplog_multilink_tx_frame_log_init
(  
  int buffer_id,
  uint16 pkt_len,
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr
);
/*===========================================================================
FUNCTION     HDRRLPLOG_LOG_SUBMIT_RLP_RX

DESCRIPTION  Terminate and send the accumulated HDR 
             MRLP partial Received log frames for one flow
             
DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS A MRLP partial frames log may be sent.
===========================================================================*/
void hdrrlplog_log_submit_rlp_rx
(
  int buffer_id 
);
/*===========================================================================
FUNCTION     HDRRLPLOG_ENHANCED_LOG_SUBMIT_RLP_RX

DESCRIPTION  Terminate and send the accumulated HDR 
             EMPA RLP partial Received log frames for one flow
             
DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS An EMPA RLP partial frames log may be sent.
===========================================================================*/
void hdrrlplog_enhanced_log_submit_rlp_rx
(
  int buffer_id 
);
/*===========================================================================
FUNCTION      HDRRLPLOG_LOG_SUBMIT_RLP_TX

DESCRIPTION   Terminate and send the accumulated HDR 
              MRLP partial TX log frames for one flow
              
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  A MRLP partial frames log may be sent.
===========================================================================*/
void hdrrlplog_log_submit_rlp_tx
(
  int buffer_id 
);
/*===========================================================================
FUNCTION     HDRRLPLOG_ENHANCED_LOG_SUBMIT_RLP_TX

DESCRIPTION  Terminate and send the accumulated HDR 
             EMPA RLP partial TX log frames for one flow
             
DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS An EMPA RLP partial frames log may be sent.
===========================================================================*/
void hdrrlplog_enhanced_log_submit_rlp_tx
(
  int buffer_id 
);

/*===========================================================================

FUNCTION      HDRRLPLOG_RX_FRAME_LOG_INIT

DESCRIPTION   This function initializes the next frame packet, as well as 
              the pointer to it, in the RLP frames log buffer.

DEPENDENCIES  None.

RETURN VALUE  Pointer to frame log entry.

SIDE EFFECTS  A MRLP partial frames log may be sent.

===========================================================================*/
log_hdrrlplog_rx_pkt_type *hdrrlplog_rx_frame_log_init
(
  int     buffer_id,
  uint16  pkt_len,
  dsrlpi_sess_ctl_blk_type *rscb_ptr
);

/*===========================================================================

FUNCTION     HDRRLPLOG_ENHANCED_RX_FRAME_LOG_INIT

DESCRIPTION  This function initializes the next frame packet, as well as 
             the pointer to it, in the EMPA RLP frames log buffer.

DEPENDENCIES None.

RETURN VALUE Pointer to frame log entry.

SIDE EFFECTS An EMPA RLP partial frames log may be sent.

===========================================================================*/
log_enhanced_hdrrlplog_rx_frame_type *hdrrlplog_enhanced_rx_frame_log_init
(  
  int buffer_id,
  uint16 pkt_len,
  dsrlpi_sess_ctl_blk_type *rscb_ptr
);
/*===========================================================================
FUNCTION      HDRRLPLOG_TX_FRAME_LOG_INIT

DESCRIPTION   This function initializes the next frame packet, as well as 
              the pointer to it, in the RLP frames log buffer.

DEPENDENCIES  None.

RETURN VALUE  Pointer to frame log entry.

SIDE EFFECTS  A MRLP partial frames log may be sent.
===========================================================================*/
log_hdrrlplog_tx_pkt_type *hdrrlplog_tx_frame_log_init
(
  int     buffer_id,
  uint16  pkt_len,
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr
);

/*===========================================================================

FUNCTION     HDRRLPLOG_ENHANCED_TX_FRAME_LOG_INIT

DESCRIPTION  This function initializes the next frame packet, as well as 
             the pointer to it, in the EMPA RLP frames log buffer.

DEPENDENCIES None.

RETURN VALUE Pointer to frame log entry.

SIDE EFFECTS An EMPA RLP partial frames log may be sent.

===========================================================================*/
log_enhanced_hdrrlplog_tx_frame_type *hdrrlplog_enhanced_tx_frame_log_init
(  
  int buffer_id,
  uint16 pkt_len,
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr
);


/*===========================================================================
FUNCTION     HDRLOG_MULTILINK_LOG_SUBMIT_RLP_RX

DESCRIPTION  Terminate and send the accumulated HDR MMPA RLP partial 
             Received log frames for one flow

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS An MMPA RLP partial frames log may be sent.
===========================================================================*/
void hdrrlplog_multilink_log_submit_rlp_rx
(
  int buffer_id 
);
/*===========================================================================
FUNCTION     HDRLOG_MULTILINK_LOG_SUBMIT_RLP_TX

DESCRIPTION  Terminate and send the accumulated HDR MMPA RLP partial 
             TX log frames for one flow

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS An MMPA RLP partial frames log may be sent.
===========================================================================*/
void hdrrlplog_multilink_log_submit_rlp_tx
(
  int buffer_id 
);
/*===========================================================================

FUNCTION      HDRRLPLOG_GET_FREE_BUFFER

DESCRIPTION   Finds a free buffer and returns the id. If the buffer is full, 
              returns invalid
  
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
int hdrrlplog_get_free_buffer
(
  uint8 pool_id
);
/*===========================================================================

FUNCTION      HDRRLPLOG_ENHANCED_GET_FREE_BUFFER

DESCRIPTION   Finds a free buffer and returns the id. If the buffer is full, 
              returns invalid, for EMPA rlp logging 
  
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
int hdrrlplog_enhanced_get_free_buffer
(
  uint8 pool_id
);
/*===========================================================================

FUNCTION      HDRLOG_MULTILINK_GET_FREE_BUFFER

DESCRIPTION   Finds a free buffer and returns the id. If the buffer is full, 
              returns invalid, for MMPA rlp logging 
  
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
int hdrrlplog_multilink_get_free_buffer
(
  uint8 pool_id
);
/*===========================================================================
FUNCTION      HDRRLPLOG_GET_BUFFER_ID()

DESCRIPTION   This function returns the buffer id for a given rlp_flow of 
              known buffer pool (TX or RX pool)

DEPENDENCIES  None.

RETURN VALUE  Buffer id if valid, else invalid

SIDE EFFECTS  None
===========================================================================*/
int hdrrlplog_get_buffer_id
(
  uint8   rlp_flow,
  int     buffer_pool_id
);
/*===========================================================================
FUNCTION      HDRRLPLOG_ENHANCED_GET_BUFFER_ID()

DESCRIPTION   This function returns the buffer id for a given EMPA rlp_flow of 
              known buffer pool (TX or RX pool) and route (A or B).

DEPENDENCIES  None.

RETURN VALUE  Buffer id if valid, else invalid

SIDE EFFECTS  None
===========================================================================*/
int hdrrlplog_enhanced_get_buffer_id
(
  uint8   rlp_flow,
  uint8   route,
  int     buffer_pool_id
);

/*===========================================================================
FUNCTION      HDRLOG_MULTILINK_GET_BUFFER_ID()

DESCRIPTION   This function returns the buffer id for a given rlp_flow of 
              known buffer pool (TX or RX pool), for a certain route.

DEPENDENCIES  None.

RETURN VALUE  Buffer id if valid, else invalid

SIDE EFFECTS  None
===========================================================================*/
int hdrrlplog_multilink_get_buffer_id
(
  uint8   rlp_flow,
  uint8   route,
  int     buffer_pool_id
);

/*===========================================================================
FUNCTION      HDRRLPLOG_CLEANUP_MRLP_LOG

DESCRIPTION   This function resets the RLP flow for logging. Submit if there
              is any log pending on buffer and frees the buffer

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrrlplog_cleanup_mrlp_log
(
  uint8               rlp_flow,
  dsrlp_ver_enum_type ver
);

/*===========================================================================
FUNCTION      HDRRLPLOG_ENHANCED_CLEANUP_MRLP_LOG

DESCRIPTION   This function resets the EMPA RLP flow for logging. Submit if there
              is any log pending on buffer and frees the buffer

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrrlplog_enhanced_cleanup_mrlp_log
(
  uint8               rlp_flow,
  dsrlp_ver_enum_type ver
);

/*===========================================================================
FUNCTION      HDRLOG_MULTILINK_CLEANUP_MRLP_LOG

DESCRIPTION   This function resets the EMPA RLP flow for logging. Submit if there
              is any log pending on buffer

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrrlplog_multilink_cleanup_mrlp_log
(
  uint8 rlp_flow, 
  dsrlp_ver_enum_type ver
);

/*===========================================================================
FUNCTION      HDRRLPLOG_LOG_INIT

DESCRIPTION   This function initializes a rlp flow

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrrlplog_log_init
(
  uint8                rlp_flow,
  dsrlp_ver_enum_type  ver
);

/*===========================================================================
FUNCTION      HDRRLPLOG_ENHANCED_LOG_INIT

DESCRIPTION   This function initializes an EMPA rlp flow

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrrlplog_enhanced_log_init
(
  uint8                rlp_flow,
  dsrlp_ver_enum_type  ver
);

/*===========================================================================
FUNCTION      HDRLOG_MULTILINK_LOG_INIT

DESCRIPTION   This function initializes the log buffer for an MMPA rlp flow

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrrlplog_multilink_log_init
(
  uint8 rlp_flow,
  dsrlp_ver_enum_type ver
);
/*===========================================================================
FUNCTION      HDRRLPLOG_LOG_TX_TIMER_CB

DESCRIPTION   This callback function called whenever the log rlp tx timer 
              expires.The function logs the partial MRLP and  and restarts 
              the timer.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
/*lint -e715 */
void hdrrlplog_log_tx_timer_cb
(
  uint32 dummy
);
/*===========================================================================
FUNCTION      HDRRLPLOG_LOG_RX_TIMER_CB

DESCRIPTION   This callback function called whenever the rx timer 
              expires.The function logs the statistics and restarts the timer.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
/*lint -e715 */
void hdrrlplog_log_rx_timer_cb
(
  uint32 dummy
);

/*===========================================================================
FUNCTION      HDRRLPLOG_LOG_STARTUP

DESCRIPTION   This function called at powerup time

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrrlplog_log_startup(void );

/*===========================================================================
FUNCTION      HDRRLP_LOG_STATS_TIMER_EXP_SIG_HDLR

DESCRIPTION   This function called to processing HDRRX_RLP_LOGGING_SIG for 
              statistics logging. 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrrlp_log_stats_timer_exp_sig_hdlr(void);

/*===========================================================================
FUNCTION      HDRRLP_LOG_TX_TIMER_EXP_SIG_HDLR

DESCRIPTION   This function called to processing HDRRX_RLP_LOGGING_SIG for 
              partial TX logging. 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrrlp_log_tx_timer_exp_sig_hdlr(void);

/*===========================================================================
FUNCTION      HDRRLP_LOG_RX_TIMER_EXP_SIG_HDLR

DESCRIPTION   This function called to processing HDRRX_RLP_LOGGING_SIG for 
              partial RX logging. 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrrlp_log_rx_timer_exp_sig_hdlr(void);

/*===========================================================================
FUNCTION      HDRRLPLOG_START_LOG_PKT

DESCRIPTION   This function called to start packet logging for statistics/TX/RX
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrrlplog_start_log_pkt(void);

/*===========================================================================
FUNCTION      HDRRLPLOG_STOP_LOG_PKT

DESCRIPTION   This function called to stop packet logging for statistics/TX/RX
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrrlplog_stop_log_pkt(void);

/*===========================================================================
FUNCTION      HDRRLPLOG_NOTIFY_ONLOG

DESCRIPTION   This function notifies the callback for every time log is written
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrrlplog_notify_onlog
(
  hdrrlplog_notify_callback *callback_ptr
);

/*===========================================================================
FUNCTION      HDRRLPLOG_SUBMIT_ALL_LOGS

DESCRIPTION   Utility function that flushes all logs collected till now 
              to Diag
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrrlplog_submit_all_logs
(
  void
);

/*===========================================================================
FUNCTION      HDRRLPLOG_GET_HSTR_COUNT()

DESCRIPTION   This function is used to get the HSTR count for EMPA and MMPA. 
              If operating in Optimized HO tunnel mode, there is no L1 to
              get the HSTR count and hence will default to 
              HDRRLPLOG_HSTR_CNT_IN_TUNNEL. While operating in direct mode, 
              it'll get the count from L1

DEPENDENCIES  None.

RETURN VALUE  Current HSTR count if in direct mode, 
              HDRRLPLOG_HSTR_CNT_IN_TUNNEL otherwise

SIDE EFFECTS  If not logged properly, there might be a log packet that 
              contains frames collected in both tunnel and direct mode
              which will contain incorrect time stamps. So ensure
              that the log is submitted when moving from tunnel to direct
              mode so we start with a new log packet when logging in direct
              mode
===========================================================================*/
int16 hdrrlplog_get_hstr_count
(
  void
);

#endif /* FEATURE_HDRRLP_LOGGING */
#endif /* FEATURE_HDR_QOS */
#endif /* HDRMRLPI_H */

