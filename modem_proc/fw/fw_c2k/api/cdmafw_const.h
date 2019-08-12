/*!
  @file cdmafw_const.h

  @brief External definitions for the CDMA1x FW module

*/

/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_c2k/api/cdmafw_const.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/
#ifndef CDMAFW_CONST_H
#define CDMAFW_CONST_H

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! \brief # Number of RGIs EPT is supported in 1x.*/
#define CDMAFW_EPT_MAX_NUM_RGI      CFW_EPT_MAX_NUM_RGI 

/*! \brief # of SSBI writes for AVGA */
#define CDMAFW_NUM_AVGA_SSBI_WRITES 4

/*! \brief # of Antennas */
#define CDMAFW_NUM_ANTENNAS 2

/*! \brief # of notch filters supported in RXF HW */
#define CDMAFW_NUM_RF_NOTCH_FILTERS 2

/*! \brief ACK Buffer log size */
#define CDMAFW_NUM_ACK_LOG_BUFFERS          2
#define CDMAFW_ADV1X_ACK_PCG_LOG_SIZE       12
#define CDMAFW_ACK_FRAME_LOG_SIZE           CDMAFW_ADV1X_ACK_PCG_LOG_SIZE*8

/*! \brief 1xAdvanced FCH results  */
#define  CDMAFW_RDA_RESULT__ERASURE         0
#define  CDMAFW_RDA_RESULT__EIGHTH_RATE     1
#define  CDMAFW_RDA_RESULT__QUARTER_RATE    2
#define  CDMAFW_RDA_RESULT__HALF_RATE       3
#define  CDMAFW_RDA_RESULT__FULL_RATE       4

/*! \brief # of PCGs per FRAME - for RxAGCLog */
#define  CDMAFW_NUM_PCG_PER_FRAME           16
#define  CDMAFW_NUM_SYMBOLS_PER_FRAME       (16*24)

#define C2K_CDMA_SHARED_MEMORY_BASE FW_SMEM_CDMA_ADDR
#define C2K_CDMA_SHARED_MEMORY_SIZE FW_SMEM_CDMA_SIZE

/*! \brief Number of rate hypotheses tested in Rate Determination Algorithm
 * (RDA) */
#define CDMAFW_NUM_RATE_HYPOTHESES   4

/*! \brief Number of decode attempts on a packet.*/
#define CDMAFW_NUM_DECODE_ATTEMPTS   15

/*! @brief Size of the buffer used for sending QXDM packet data */
#define CDMAFW_QXDM_DIAG_DATA_BUF_SIZE_BYTES  64

/*! @brief 1x SRAM I/Q Capture Mempool configuration.
 * 
 * @deprecated These fields are DEPRECATED!! in Jolokia and are present for compilation
 * compatibility ONLY. Alternatives will be provided in SMEM (TBD). DO NOT USE the
 * following definitions.
 * 
 */
/*! @brief Mempool line size */
#define CDMAFW_MEMPL_BITS_PER_BANK        128
#define CDMAFW_MEMPL_BANKS                8
#define CDMAFW_MEMPL_LINE_SIZE            (CDMAFW_MEMPL_BANKS*CDMAFW_MEMPL_BITS_PER_BANK)
/*! @brief Defines Mempool bridge to be used for I/Q capture */
#define CDMAFW_SAMPRAM_MEMPL_BRDG_ID      1
/*! @brief Bank Indices */
#define CDMAFW_SAMPRAM_MIN_BANK           0
#define CDMAFW_SAMPRAM_MAX_BANK           7
#define CDMAFW_SAMPRAM_INIT_BANK          0
/*! @brief QSIZE in bits (I + Q = 8 + 8 = 16 bits). */
#define CDMAFW_SAMPRAM_QSIZE              16
/*! @brief QSIZE register entry for mempool bridge. */
#define CDMAFW_SAMPRAM_QSIZE_REG          0x5
/*! @brief VBUF_LEN in bits
 * 2 * 2048 * 2 (cx2) * (8+8) = 131072 bits for a single antenna.
 */
#define CDMAFW_SAMPRAM_VBUF_LEN_BITS      (2*2048*2*CDMAFW_SAMPRAM_QSIZE)
/*! @brief VBUF_LEN in QSIZE units = 8192. */
#define CDMAFW_SAMPRAM_VBUF_LEN           (CDMAFW_SAMPRAM_VBUF_LEN_BITS/CDMAFW_SAMPRAM_QSIZE)
/*! @brief # of mempool lines (per antenna) = VBUF_LEN (bits) / (128*8) = 64 lines. */
#define CDMAFW_SAMPRAM_NUM_LINES          (CDMAFW_SAMPRAM_VBUF_LEN_BITS/CDMAFW_MEMPL_LINE_SIZE)
/*! @brief CDMA line start offset for antenna0 */
#define CDMAFW_SAMPRAM_START_LINE_OFS_A0  60
/*! @brief CDMA line start offset for antenna1 */
#define CDMAFW_SAMPRAM_START_LINE_OFS_A1  (CDMAFW_SAMPRAM_START_LINE_OFS_A0 + CDMAFW_SAMPRAM_NUM_LINES)

/** 
 * \addtogroup clog
 * @{
 */

/*!< CDMA FW Diag Logging definitions 
 */

/*!< Available CDMA FW Diag Log IDs */
#define CDMAFW_LOG_DIAG_PKT_START_ID            0x1600
#define CDMAFW_LOG_DIAG_PKT_END_ID              0x1632
#define CDMAFW_LOG_NUM_DIAG_PKT_IDS             (CDMAFW_LOG_DIAG_PKT_END_ID - \
                                                 CDMAFW_LOG_DIAG_PKT_START_ID + 1)
/*!< FING Common Config, Assign MSG Log */                 
#define CDMAFW_FL_FINGER_CFG_ASSIGN_MSG_LOG_PKT_OFS   0x0
#define CDMAFW_FL_FINGER_CFG_ASSIGN_MSG_LOG_PKT_ID    (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                       CDMAFW_FL_FINGER_CFG_ASSIGN_MSG_LOG_PKT_OFS)
#define CDMAFW_FL_FINGER_CFG_ASSIGN_MSG_LOG_PKT_VER   0

/*!< Basic Combiner/Finger Info Log */
#define CDMAFW_FL_BASIC_FING_COMB_LOG_PKT_OFS   0x1
#define CDMAFW_FL_BASIC_FING_COMB_LOG_PKT_ID    (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_FL_BASIC_FING_COMB_LOG_PKT_OFS)
#define CDMAFW_FL_BASIC_FING_COMB_LOG_PKT_VER   1

/*!< Basic Rx Timing Info Log */
#define CDMAFW_FL_RX_TIMING_LOG_PKT_OFS         0x2
#define CDMAFW_FL_RX_TIMING_LOG_PKT_ID          (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_FL_RX_TIMING_LOG_PKT_OFS)
#define CDMAFW_FL_RX_TIMING_LOG_PKT_VER         0

/*!< Finger SM Log */
#define CDMAFW_FL_FING_SM_LOG_PKT_OFS           0x3
#define CDMAFW_FL_FING_SM_LOG_PKT_ID            (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_FL_FING_SM_LOG_PKT_OFS)
#define CDMAFW_FL_FING_SM_LOG_PKT_VER           0

/*!< Rx Sched Log */
#define CDMAFW_FL_RX_SCHED_LOG_PKT_OFS          0x4
#define CDMAFW_FL_RX_SCHED_LOG_PKT_ID           (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_FL_RX_SCHED_LOG_PKT_OFS)
#define CDMAFW_FL_RX_SCHED_LOG_PKT_VER          0

/*!< Finger, Combiner & Global DMEM Log */
#define CDMAFW_FL_FING_COMB_DMEM_LOG_PKT_OFS    0x5
#define CDMAFW_FL_FING_COMB_DMEM_LOG_PKT_ID     (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_FL_FING_COMB_DMEM_LOG_PKT_OFS)
#define CDMAFW_FL_FING_COMB_DMEM_LOG_PKT_VER    0

/*!< Basic AFC Info Log */
#define CDMAFW_FL_BASIC_AFC_LOG_PKT_OFS         0x30
#define CDMAFW_FL_BASIC_AFC_LOG_PKT_ID          (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_FL_BASIC_AFC_LOG_PKT_OFS)
#define CDMAFW_FL_BASIC_AFC_LOG_PKT_VER         1

/*!< VSRC-STMR Sync Log */
#define CDMAFW_FL_VSRC_STMR_LOG_PKT_OFS         0x31
#define CDMAFW_FL_VSRC_STMR_LOG_PKT_ID          (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_FL_VSRC_STMR_LOG_PKT_OFS)
#define CDMAFW_FL_VSRC_STMR_LOG_PKT_VER         0

/*!< Reverse Link Half-PCG Info Log */
#define CDMAFW_RL_HALF_PCG_LOG_PKT_OFS          0x18
#define CDMAFW_RL_HALF_PCG_LOG_PKT_ID           (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_RL_HALF_PCG_LOG_PKT_OFS)
#define CDMAFW_RL_HALF_PCG_LOG_PKT_VER          3

/*!< Reverse Link TxAGC Log */
#define CDMAFW_RL_PER_UPDATE_LOG_PKT_OFS        0x19
#define CDMAFW_RL_PER_UPDATE_LOG_PKT_ID         (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_RL_PER_UPDATE_LOG_PKT_OFS)
#define CDMAFW_RL_PER_UPDATE_LOG_PKT_VER        3

/*!< DSDA TX Blanking Info Log */
#define CDMAFW_RL_DSDA_TX_BLANK_LOG_PKT_OFS     0x1B
#define CDMAFW_RL_DSDA_TX_BLANK_LOG_PKT_ID      (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_RL_DSDA_TX_BLANK_LOG_PKT_OFS)
#define CDMAFW_RL_DSDA_TX_BLANK_LOG_PKT_VER     1

/*!< Reverse Link Frame Config Params Log */
#define CDMAFW_RL_FRAME_CONFIG_LOG_PKT_OFS      0x1C
#define CDMAFW_RL_FRAME_CONFIG_LOG_PKT_ID       (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_RL_FRAME_CONFIG_LOG_PKT_OFS)
#define CDMAFW_RL_FRAME_CONFIG_LOG_PKT_VER      0

/*!< Mempool Dump Log Packet */
#define CDMAFW_MEMPOOL_DUMP_LOG_PKT_OFS         0x20
#define CDMAFW_MEMPOOL_DUMP_LOG_PKT_ID          (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                  CDMAFW_MEMPOOL_DUMP_LOG_PKT_OFS)
#define CDMAFW_MEMPOOL_DUMP_LOG_PKT_VER         0
/*!< WAV uK Log */
#define CDMAFW_RL_WAV_SYNC_LOG_PKT_OFS          0x1D
#define CDMAFW_RL_WAV_SYNC_LOG_PKT_ID           (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_RL_WAV_SYNC_LOG_PKT_OFS)
#define CDMAFW_RL_WAV_SYNC_LOG_PKT_VER          0

/*!< Basic RxAGC 1/6 PCG Log */
#define CDMAFW_RXAGC_FOUR_SYM_LOG_PKT_OFS       0x8
#define CDMAFW_RXAGC_FOUR_SYM_LOG_PKT_ID        (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_RXAGC_FOUR_SYM_LOG_PKT_OFS)
#define CDMAFW_RXAGC_FOUR_SYM_LOG_PKT_VER       0

/*!< RxAGC 1/12 PCG Log */
#define CDMAFW_RXAGC_TWO_SYM_LOG_PKT_OFS       0x9
#define CDMAFW_RXAGC_TWO_SYM_LOG_PKT_ID        (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_RXAGC_TWO_SYM_LOG_PKT_OFS)
#define CDMAFW_RXAGC_TWO_SYM_LOG_PKT_VER       0

/*!< RPC Log */
#define CDMAFW_RPC_LOG_PKT_OFS                 0x10
#define CDMAFW_RPC_LOG_PKT_ID                  (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_RPC_LOG_PKT_OFS)
#define CDMAFW_RPC_LOG_PKT_VER                 0

#define CDMAFW_FL_ACK_LOG_PKT_OFS              0x11
#define CDMAFW_FL_ACK_LOG_PKT_ID               (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_FL_ACK_LOG_PKT_OFS)
#define CDMAFW_FL_ACK_LOG_PKT_VER              0

/*!<1x->X QTA Log */
#define CDMAFW_1X2X_QTA_LOG_PKT_OFS            0x13
#define CDMAFW_1X2X_QTA_LOG_PKT_ID             (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_1X2X_QTA_LOG_PKT_OFS)
#define CDMAFW_1X2X_QTA_LOG_PKT_VER            0
/*!< DSDA RX Blanking Info Log */
#define CDMAFW_FL_DSDX_RX_BLANK_LOG_PKT_OFS     0x15
#define CDMAFW_FL_DSDX_RX_BLANK_LOG_PKT_ID      (CDMAFW_LOG_DIAG_PKT_START_ID + \
                                                 CDMAFW_FL_DSDX_RX_BLANK_LOG_PKT_OFS)
#define CDMAFW_FL_DSDX_RX_BLANK_LOG_PKT_VER     0

/**@}**/

#endif

