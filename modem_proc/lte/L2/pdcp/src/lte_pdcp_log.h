/*!
  @file
  lte_pdcp_log.h

  @brief
  This header file contains the variou defines, enums, and other data
  structure types which are necessary for logging binary packets via DIAG.

  @author
  bq

*/

/*===========================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcp_log.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/24/15   sb      CR938646: Reduce frequency of F3s without impacting debugging
07/23/15   sb      CR867973: Enhance logging and QSH for UDC
06/30/14   mg      CR 640102: Log packet of Number of packets with different 
                   delays in PDCP WMks for all bearers
11/19/13   sb      CR535830: PDCP/RLC Re-arch and support of REL-11 features
10/18/13   ta      CR 553489: Reorg of log defs
10/23/12   sb      Code changes for case when RRC explicitly signals that RoHC is enabled
12/16/11   ax      feature clean up 
06/15/11   ax      RoHC support
01/17/11   ax      add error counters for specific error conditions on UL
01/07/11   ax      add statistics for non-RB specific errors 
06/30/10   ar      added PDCP integrity log packet support
05/19/10   ar      added PDCP ciphering log packet support
01/15/10   ax      F3 Supressing and filtering
11/09/09   ax      declared structure within log pkt PACK(struct)
08/06/09   bq      move security cfg to top level
05/06/09   ax      added array_size to cfg log pkt to sheild log parsers from
                   potential LTE_MAX_ACTIVE_RB change in value in the future
03/04/09   bq      add PDCP UL logging APIs for RLCUL
02/22/09   bq      Log code cleanup
10/15/08   bq      Initial version
===========================================================================*/

#ifndef LTE_PDCP_LOG_H
#define LTE_PDCP_LOG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <genlog.h>
#include <lte_log_codes.h>
#include "lte_pdcp_log_ext.h"
#include "lte_pdcp_cfg.h"
#include "lte_pdcp_msg.h"
#include "lte_pdcp_ul_if.h"
#include "lte_pdcpi.h"
#include "lte_pdcp_dl_protocol.h"
#include "lte_pdcp_ul_protocol.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define LTE_PDCPUL_LOG_ERROR(err_code, fmt, a, b, c)                                     \
  MSG_3(MSG_SSID_LTE_PDCPUL, MSG_LEGACY_FATAL,  fmt, a, b, c );                \
  lte_pdcp_ul_log_error_handler(err_code)

#define LTE_PDCPDL_LOG_ERROR(err_code, fmt, a, b, c)                           \
  MSG_3(MSG_SSID_LTE_PDCPDL, MSG_LEGACY_FATAL,  fmt, a, b, c );                \
  lte_pdcp_dl_log_error_handler(err_code)

#define LTE_PDCPUL_MSG_0_DATA_PATH(xx_fmt)

#define LTE_PDCPUL_MSG_1_DATA_PATH(xx_fmt, a)

#define LTE_PDCPUL_MSG_2_DATA_PATH(xx_fmt, a, b)

#define LTE_PDCPUL_MSG_3_DATA_PATH(xx_fmt, a, b, c)

#define LTE_PDCPUL_MSG_4_DATA_PATH(xx_fmt, a, b, c, d)

#define LTE_PDCPDL_MSG_0_DATA_PATH(xx_fmt)

#define LTE_PDCPDL_MSG_1_DATA_PATH(xx_fmt, a)

#define LTE_PDCPDL_MSG_2_DATA_PATH(xx_fmt, a, b)

#define LTE_PDCPDL_MSG_3_DATA_PATH(xx_fmt, a, b, c)

#ifdef MSG_BUILD_MASK_MSG_SSID_LTE_PDCPUL
  #undef MSG_BUILD_MASK_MSG_SSID_LTE_PDCPUL
#endif /* MSG_BUILD_MASK_MSG_SSID_LTE_PDCPUL */

/* MASK not included here will be supressed at runtime:
   * MSG_LEGACY_LOW
   * MSG_LEGACY_MED
*/
#define MSG_BUILD_MASK_MSG_SSID_LTE_PDCPUL  \
               (MSG_LEGACY_HIGH         |   \
                MSG_LEGACY_ERROR        |   \
                MSG_LEGACY_FATAL)

/* MASK not included here will be supressed at runtime:
   * MSG_LEGACY_LOW
   * MSG_LEGACY_MED
*/
#ifdef MSG_BUILD_MASK_MSG_SSID_LTE_PDCPDL
  #undef MSG_BUILD_MASK_MSG_SSID_LTE_PDCPDL
#endif /* MSG_BUILD_MASK_MSG_SSID_LTE_PDCPDL */

#define MSG_BUILD_MASK_MSG_SSID_LTE_PDCPDL  \
               (MSG_LEGACY_HIGH         |   \
                MSG_LEGACY_ERROR        |   \
                MSG_LEGACY_FATAL)

/*! @brief Log direction */
typedef enum
{
  LTE_PDCP_LOG_DL,
  LTE_PDCP_LOG_UL
} lte_pdcp_log_direction_e;

/*! @brief Log SubIds of PDCP */
#define LTE_PDCP_LOG_SUB_ID_DL_CFG      0xC0  /*< Subpkt Id for PDCP DL Config
                                                  log packet */
#define LTE_PDCP_LOG_SUB_ID_UL_CFG      0xC1  /*< Subpkt Id for PDCP UL Config
                                                  log packet */
#define LTE_PDCP_LOG_SUB_ID_PDU         0xC2  /*< Subpkt Id for PDCP PDU log
                                                  packet */
#define LTE_PDCP_LOG_SUB_ID_PDU_CIPHER  0xC3  /*< Subpkt Id for PDCP PDU log
                                                  packet with Cipher */
#define LTE_PDCP_LOG_SUB_ID_DL_STATS    0xC4  /*< Subpkt Id for PDCP DL Stats */
#define LTE_PDCP_LOG_SUB_ID_UL_STATS    0xC5  /*< Subpkt Id for PDCP UL Stats */
#define LTE_PDCP_LOG_SUB_ID_DL_PDU_INTEG 0xC6 /*< Subpkt Id for PDCP DL PDU log
                                                   packet with integrity */
#define LTE_PDCP_LOG_SUB_ID_UL_PDU_INTEG 0xC7 /*< Subpkt Id for PDCP UL PDU log
                                                   packet with integrity */
#define LTE_PDCP_LOG_SUB_ID_UL_DELAY_STATS 0xC8  /*< Subpkt Id for PDCP UL 
                                                   Delay Stats */
#define LTE_PDCP_LOG_SUB_ID_MAX            0xC9

/*! @brief
    Frequency of logging for PDCP PDU packet in msec
*/
#define LTE_PDCP_LOG_PDU_FLUSH_TIMER 50

/*! @brief
    Frequency of logging for PDCP PDU packet in msec
*/
#define LTE_PDCP_LOG_STATS_FLUSH_TIMER 500

/*! @brief
    Frequency of logging for PDCP STATS packet based on
    the units of LTE_PDCP_LOG_PDU_FLUSH_TIMER
*/
#define LTE_PDCP_LOG_STATS_FLUSH_COUNT 10

/*! @brief
    Integrity pdu log buffer size in bytes
*/
#define LTE_PDCP_LOG_INTEG_PDU_BUF_SIZE          1024

#ifdef _WIN32
    #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

/*! @brief PDCP DL statistics
*/
typedef PACK(struct)
{
  uint32  num_flow_ctrl_trig;   /*!< number of timer flow control trigger */
  uint32  num_data_pdu;         /*!< number of PDCP data pdu received */
  uint32  data_pdu_bytes;       /*!< PDCP data PDU in bytes received */
  uint32  num_control_pdu;      /*!< number of PDCP control pdu received */
  uint32  control_pdu_bytes;    /*!< PDCP control PDU in bytes received */
  uint32  num_control_pdu_gen;   /*!< number of PDCP control pdu generated */
  uint32  control_pdu_gen_bytes; /*!< PDCP control PDU in bytes generated */
  uint32  num_status_report;  /*!< number of status report received */
  uint32  num_rohc_feedback;  /*!< number of ROHC feedback received */
  uint32  num_rohc_fail;      /*!< nmuber of ROHC decompression failed */
  uint32  num_integrity_fail;  /*!< number of integrity check failed */
  uint32  num_miss_pdu_to_upper_layer; /*!< gap to upper layer */
  uint32  num_miss_pdu_from_low_layer; /*!< gap from low layer */
  uint32  num_dup_pdu;       /*!< number of dup PDUs */
  uint32  dup_pdu_bytes;     /*!< dup PDU in bytes */
  uint32  num_out_of_win_pdu;   /*!< number of discard PDUs */
  uint32  out_of_win_pdu_bytes; /*!< discard PDUs bytes */
  uint32  num_invalid_pdu;   /*!< number of invalid PDUs */
  uint32  invalid_pdu_bytes; /*!< invalid PDU in bytes */
  uint32  reserved;             /*!< reserved */
} lte_pdcpdl_log_stats_set_s;

/*! @brief PDCP UL statistics
*/
typedef PACK(struct)
{
  uint32  num_flow_ctrl_trig;  /*!< number of timer flow disable */
  uint32  num_data_pdu;        /*!< number of PDCP pdu transmitted */
  uint32  data_pdu_bytes;      /*!< PDCP PDU in bytes transmitted */
  uint32  data_bytes_from_ps;  /*!< Total bytes dequeued from PS Wm*/
  uint32  num_control_pdu;     /*!< number of PDCP pdu transmitted */
  uint32  control_pdu_bytes;   /*!< PDCP PDU in bytes transmitted */
  uint32  num_status_report;  /*!< number of status report transmitted */
  uint32  num_rohc_feedback;  /*!< number of ROHC feedback transmitted */
  uint32  num_rohc_fail;      /*!< nmuber of ROHC compression failed */
  uint32  num_discard_sdu;   /*!< number of time based discard PDUs */
  uint32  discard_sdu_bytes; /*!< time based discard PDUs bytes */
  uint32  num_retransmit_sdu;   /*!< number of retransmit PDUs */
  uint32  retransmit_sdu_bytes; /*!< retransmit PDU in bytes */
  uint32  num_piggyback_rohc_feedback_rxed;  /*!< number of piggyback ROHC
                               feedback rxed */  
  uint32  num_rerohc_pdu_drop; /*!< number of RoHC PDUs drop during HO*/
  uint32  rerohc_pdu_drop_bytes; /*!< number of RoHC PDU bytes drop during HO*/
  uint32  num_udc_comp_pkts; /*!<*total num of UDC compressed pkts*/
  uint32  udc_comp_bytes; /*!<*total num of UDC compressed  bytes*/  
  uint32  num_udc_fc_uncomp_pkts; /*!< total num of UDC uncompressed pkts*/
  uint32  udc_fc_uncomp_bytes; /*!<*total num of UDC non-compressed  bytes*/
  uint32  num_udc_fail;  /*!< Number of UDC compression failed*/
  uint16  num_udc_reset; /*!< Num of time UDC RESET is called */
  uint32  num_ue_udc_ctrl_pdus;      /*!< num of UE triggered UDC Ctrl PDUs */
  uint32  num_enb_udc_ctrl_pdus;      /*!< num of eNB triggered UDC Ctrl PDUs */
  uint32  num_udc_dsm_fc_down;         /*!< num of  CMD_DOWN for DSM monitor*/
  uint32  num_udc_cpu_fc_down;         /*!< num of  CMD_DOWN for CPU monitor*/
  uint32  num_udc_dsm_fc_shutdown;        /*!< num of  CMD_SHUT_DOWN for DSM monitor*/
  uint32  num_udc_cpu_fc_freeze;           /*!< num of  CMD_UP for CPU monitor*/
  uint32  num_enb_trigg_udc_reset;  /*!<num of UDC reset triggered by enB during non-HO/non-RLF*/
  uint32  reserved;             /*!< reserved */
} lte_pdcpul_log_stats_set_s;

/*! @brief Structure for PDCP DL RB Stats Log
*/
typedef PACK(struct)
{
  uint8   rb_cfg_idx; /*!< RB cfg index */
  uint8   rb_mode;    /*!< RB mode */
  uint8   pdcp_hdr_len;    /*!< PDCP hdr len */
  uint32  num_rst;     /*!< number of reestablishments since activated */
  lte_pdcpdl_log_stats_set_s tot; /*!< Total stats counter */
  lte_pdcpdl_log_stats_set_s curr; /*!< Current stats counter */

} lte_pdcp_log_dl_rb_stats_s;

/*! @brief Structure for PDCP DL Stats Log sub packet
*/
typedef PACK(struct)
{
  uint8                 num_rb;       /*!< number of RBs */
  uint32                num_error;   /*!< number of PDCP DL errors */
  uint32                num_pdcp_offload_q_full_cnt;
  uint32                num_pkt_drop_pdcp_offload_q_full;

  lte_pdcp_log_dl_rb_stats_s rb_stats[LTE_MAX_ACTIVE_RB]; /*!< RB stats */

} lte_pdcp_log_sub_pkt_dl_stats_s;

/*! @brief Structure for PDCP UL RB Stats Log
*/
typedef PACK(struct)
{
  uint8   rb_cfg_idx; /*!< RB cfg index */
  uint8   rb_mode;    /*!< RB mode */
  uint8   pdcp_hdr_len;    /*!< PDCP hdr len */
  uint32  num_rst;     /*!< number of reestablishments since activated */
  uint16  num_ps_wm_pkt; /*!< number of packets in PS PDCP WM */
  uint32  ps_wm_bytes; /*!< number of bytes in PS PDCP WM*/
  uint32  udc_comp_state;        /*!<*UDC compression state: disable_comp(0), full_comp(1), 
                                                            hdr_only_comp(2), no_comp(3)*/
  lte_pdcpul_log_stats_set_s tot; /*!< Total stats counter */
  lte_pdcpul_log_stats_set_s curr; /*!< Current stats counter */

} lte_pdcp_log_ul_rb_stats_s;

/*! @brief Structure for PDCP UL Stats Log sub packet
*/
typedef PACK(struct)
{
  uint8                 num_rb;       /*!< number of RBs */
  uint32                num_error;   /*!< number of PDCP DL errors */

  lte_pdcp_log_ul_rb_stats_s rb_stats[LTE_MAX_ACTIVE_RB]; /*!< RB stats */

} lte_pdcp_log_sub_pkt_ul_stats_s;

/*! @brief PDCP UL delay statistics
*/
typedef PACK(struct)
{
  uint32  pdcp_delay_pkt_cnt[LTE_PDCP_DELAY_STATS_SIZE]; /*accumulated
   histogram containing the delays of the packets in pdcp ul watermark*/
  uint32  num_discard_sdu;   /*!< number of time based discard PDUs */
  uint32  discard_sdu_bytes; /*!< time based discard PDUs bytes */
  uint32  reserved;             /*!< reserved */
} lte_pdcpul_delay_log_stats_set_s;

/*! @brief Structure for PDCP UL  delay RB Stats Log
*/
typedef PACK(struct)
{
  uint8   rb_cfg_idx; /*!< RB cfg index */
  uint16  discard_timer; /*!< discard timer for this bearer*/
  uint8   reserved; /*!< Padding for pack structure to align the array*/
  lte_pdcpul_delay_log_stats_set_s curr; /*!< Current stats counter */
  lte_pdcpul_delay_log_stats_set_s tot; /*!< Total stats counter */
} lte_pdcp_log_ul_delay_rb_stats_s;

/*! @brief Structure for PDCP UL delay Stats Log sub packet
*/
typedef PACK(struct)
{
  uint8   num_rb;     /*!< number of RBs */
  uint32  num_rst;    /*!< number of reestablishments since activated */
  lte_pdcp_log_ul_delay_rb_stats_s rb_stats[LTE_MAX_ACTIVE_RB]; /*!< RB stats */
} lte_pdcp_log_sub_pkt_ul_delay_stats_s;

/*! @brief Structure for PDCP srb integrity log sub packet */
typedef PACK(struct)
{
  uint8  srb_cipher_key[LTE_PDCP_LOG_KEY_SIZE_IN_BYTES]; /*!< srb cipher key */
  uint8  srb_integ_key[LTE_PDCP_LOG_KEY_SIZE_IN_BYTES];  /*!< srb integ key */
  uint8  srb_cipher_algo;                                /*!< srb cipher algo */
  uint8  srb_integ_algo;                                 /*!< srb integ algo */
  uint16 num_pdus;                                       /*!< num pdus logged */
  uint8  log_buf[LTE_PDCP_LOG_INTEG_PDU_BUF_SIZE];       /*!< pdu log buffer */
  uint32 size;                                           /*!< payload size */
} lte_pdcp_log_sub_pkt_integ_pdu_s;

/*! @brief Structure for PDCPDL srb integrity log pdu */
typedef PACK(struct)
{
  lte_pdcp_log_pdu_rb_info_s        rb_info;       /*!< radio bearer cfg info */
  uint16                            pdu_size;      /*!< total size of the pdu */
  uint16                            logged_bytes;  /*!< num pdu bytes logged */
  lte_sfn_s                         sfn;           /*!< system/sub frame info */
  uint32                            count;         /*!< PDCP count */
  uint32                            received_maci; /*!< maci value received from
                                                        the eNB */
  uint32                            computed_maci; /*!< maci value computed by
                                                        the UE */
}lte_pdcpdl_log_integ_pdu_s;

/*! @brief Structure for PDCPUL srb integrity log pdu */
typedef PACK(struct)
{
  lte_pdcp_log_pdu_rb_info_s        rb_info;       /*!< radio bearer cfg info */
  uint16                            pdu_size;      /*!< total size of the pdu */
  uint16                            logged_bytes;  /*!< num pdu bytes logged */
  lte_sfn_s                         sfn;           /*!< system/sub frame info */
  uint32                            count;         /*!< PDCP count */
  uint32                            computed_maci; /*!< maci value computed by
                                                        the UE */
}lte_pdcpul_log_integ_pdu_s;

#ifdef _WIN32
   #pragma pack(pop) // Revert alignment to what it was previously
#endif

/*! @brief error index
*/
typedef enum
{
  LTE_PDCP_LOG_ERR_GENERIC = 0,           /*!< meta info exhaustion error */
  LTE_PDCP_LOG_ERR_META_EXHAUSTION,       /*!< meta info exhaustion error */
  LTE_PDCP_LOG_ERR_WIN_STALL,             /*!< window stall error*/
  LTE_PDCP_LOG_ERR_RAB_REG_FAILURE,       /*!< RAB registration failure */
  LTE_PDCP_LOG_ERR_RB_ADD_FAILURE,        /*!< radio bearer add failure */
  LTE_PDCP_LOG_ERR_DECOMPRESS_FAILURE,    /*!< decompression failure */
  LTE_PDCP_LOG_ERR_COMPRESS_FAILURE,      /*!< compression failure */
  LTE_PDCP_LOG_ERR_MAX                    /*!< max */
} lte_pdcp_log_error_e;

/*! @brief static non RB specific UL error statistics
*/
typedef struct
{
  uint32  total_num_errors;           /*!< total number of errors in PDCP UL */
  uint32  num_generic_errors;         /*!< num of generic errors */
  uint32  num_meta_exhaustion;        /*!< num of out of meta info errors */
  uint32  num_win_stall;              /*!< num of window stall errors */
  uint32  num_rab_reg_failure;        /*!< num of RAB registration errors */
  uint32  num_rb_add_failure;         /*!< num of RB add errors */
  uint32  num_rohc_compress_errors;   /*!< num of decompression errors */
} lte_pdcp_log_ul_error_s;

/*! @brief static non RB specific UL error statistics
*/
typedef struct
{
  uint32  total_num_errors;           /*!< total number of errors in PDCP UL */
  uint32  num_generic_errors;         /*!< num of generic errors */
  uint32  num_rohc_decompress_errors; /*!< num of decompression errors */
} lte_pdcp_log_dl_error_s;

/*! @brief Structure for PDCP Log
*/
typedef struct
{
  /*! sub pkt for DL PDUs */
  lte_pdcp_log_sub_pkt_pdu_s         dl_pdus[LTE_MAX_ACTIVE_RB];
  /*! whether this DL PDU sub pkt assigned to an active RB */
  boolean                            dl_pdus_assigned[LTE_MAX_ACTIVE_RB];
  /*! mapping from DL RB ID to sub pkt ptr */
  lte_pdcp_log_sub_pkt_pdu_s*        dl_pdus_mapping[LTE_MAX_RB_CFG_IDX];
  /*! sub pkt for DL stats */
  lte_pdcp_log_sub_pkt_dl_stats_s    dl_stats;
  /*! sub pkt for UL stats */
  lte_pdcp_log_sub_pkt_ul_stats_s    ul_stats;
  /*! sub pkt for UL delay stats */
  lte_pdcp_log_sub_pkt_ul_delay_stats_s ul_delay_stats;
  /*! sub pkt for DL ciphered PDUs */
  lte_pdcp_log_sub_pkt_cipher_pdu_s  dl_ciph_pdus;
  /*! sub pkt for DL integrity PDUs */
  lte_pdcp_log_sub_pkt_integ_pdu_s   dl_integ_pdus;
  /*! 2byte dl pdu rb info needed for cipher logging  */
  lte_pdcp_log_pdu_rb_info_s         dl_pdu_rb_info[LTE_MAX_RB_CFG_IDX];
  /*! sub pkt for DL integrity PDUs */
  lte_pdcp_log_sub_pkt_integ_pdu_s   ul_integ_pdus;
  /*! 2byte ul pdu rb info needed for cipher logging  */
  lte_pdcp_log_pdu_rb_info_s         ul_pdu_rb_info[LTE_MAX_RB_CFG_IDX];

  /*! ptr to the last cipher pdu that was logged, needed for updating COUNT and
   * pdu_valid flag */
  lte_pdcp_log_cipher_pdu_s*         last_ciph_pdu_log_ptr;
  /*! ptr to the last srb integrity pdu that was logged, needed for updating
   * COUNT and pdu_valid flag */
  lte_pdcpdl_log_integ_pdu_s*        last_integ_pdu_log_ptr;
  /*! GENLOG */
  genlog_s                           dl_genlog;
  /*! GENLOG */
  genlog_s                           ul_genlog;
  /*! whether it is initialized */
  boolean                            is_init;
  /*! DL log mask*/
  uint32                             dl_log_mask;
  /*! UL log mask*/
  uint32                             ul_log_mask;
  /*! num cipher pdu payload bytes to log */
  uint16                             num_sdu_bytes_to_log;
  /*! statistics of UL errors */
  lte_pdcp_log_ul_error_s            ul_error_stat;

  /*! statistics of DL errors */
  lte_pdcp_log_dl_error_s            dl_error_stat;
} lte_pdcp_log_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_pdcp_log_init

===========================================================================*/
/*!
    @brief
    This function is called to init PDCP log data structure.

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcp_log_init(void);

/*===========================================================================

  FUNCTION:  lte_pdcp_log_get_log_data

===========================================================================*/
/*!
  @brief
  This function returns the global data for PDCP log

  @details
  This function returns the global data for PDCP log

  @return
  lte_pdcp_log_s*
*/
/*=========================================================================*/
extern lte_pdcp_log_s* lte_pdcp_log_get_log_data(void);

/*===========================================================================

  FUNCTION:  lte_pdcpul_log_process_recfg_req

===========================================================================*/
/*!
    @brief
    This function is called to process RB reconfig request for add/modified/delete.

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpul_log_process_recfg_req
(
  lte_pdcp_cfg_act_e  cfg_act,
  lte_rb_cfg_idx_t    rb_cfg_idx,
  lte_pdcpul_info_s* cfg_info_ptr
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_log_process_recfg_req

===========================================================================*/
/*!
    @brief
    This function is called to process RB reconfig request for add/modified/delete.

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpdl_log_process_recfg_req
(
  lte_pdcp_cfg_act_e  cfg_act,
  lte_rb_cfg_idx_t    rb_cfg_idx,
  lte_pdcpdl_info_s* cfg_info_ptr,
  boolean            log_F3
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_log_process_pdu

===========================================================================*/
/*!
    @brief
    This function is called to process PDU.

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpdl_log_process_pdu
(
  lte_rb_cfg_idx_t    rb_cfg_idx,
  dsm_item_type*    dsm_ptr,
  lte_sfn_s         sfn,
  uint32            pkt_len
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_log_process_integ_pdu

===========================================================================*/
/*!
    @brief
    This function is called to log the SRB integrity PDU.

    @return
    none

*/
/*=========================================================================*/
void lte_pdcpdl_log_process_integ_pdu
(
  lte_rb_cfg_idx_t  rb_cfg_idx, /*!< rb config index */
  dsm_item_type*    dsm_ptr,    /*!< pdcp pdu dsm ptr */
  lte_sfn_s         sfn,        /*!< sys_fn/sub_fn info */
  uint32            pkt_len     /*!< length of pdcp pdu */
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_log_process_integ_pdu

===========================================================================*/
/*!
    @brief
    This function is called to log the UL SRB integrity PDU.

    @detail
    UL SRB integrity pdu is logged in PDCP instead of RLC due to the fact that
    PDCP computes the integrity on the ip packets before submitting to RLC WM.
    There can be a potential mis-match between the computed maci and the
    integrity algo/key info that gets logged as the security configuration may
    change in between

    @return
    none

*/
/*=========================================================================*/
void lte_pdcpul_log_process_integ_pdu
(
  lte_rb_cfg_idx_t  rb_cfg_idx, /*!< rb config index */
  dsm_item_type*    dsm_ptr,    /*!< pdcp pdu dsm ptr */
  uint32            mac_i,      /*!< 32 bit integrity value */
  lte_sfn_s         sfn,        /*!< sys_fn/sub_fn info */
  uint32            pkt_len     /*!< pdcp pdu length */
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_log_submit_pdu_log_pkt

===========================================================================*/
/*!
    @brief
    This function submits the PDU log pkt.
    The log buffer is then reset to start as an empty buffer for the same stream.

    @return
    None

*/
/*=========================================================================*/
extern void lte_pdcpdl_log_submit_pdu_log_pkt
(
  lte_rb_cfg_idx_t    rb_cfg_idx,
  log_code_type       log_code
);


/*===========================================================================

  FUNCTION:  lte_pdcpdl_update_log_status

===========================================================================*/
/*!
  @brief
  This functions update the local log status based on log code when DL log flush
  timer is expired

  @details

  @return
*/
/*=========================================================================*/
extern void lte_pdcpdl_update_log_status
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_update_log_status

===========================================================================*/
/*!
  @brief
  This functions update the local log status based on log code when UL log flush
  timer is expired

  @return
  none
*/
/*=========================================================================*/
extern void lte_pdcpul_update_log_status
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_log_submit_cfg_log_pkt

===========================================================================*/
/*!
    @brief
    This function submits the config log pkt.

    @return
    None

*/
/*=========================================================================*/
extern void lte_pdcpdl_log_submit_cfg_log_pkt
(
  uint8   cfg_reason,      /*!< cfg reason */
  uint8   num_released_rb, /*!< number of released RB */
  lte_rb_cfg_idx_t *released_rb_cfg_idx, /*!< released RB cfg index */
  uint8   num_add_modified_rb, /*!< number of add/modified RB */
  lte_pdcp_cfg_act_s *add_modified_rb /*!< cfg action for add/modified RB ID */
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_log_submit_cfg_log_pkt

===========================================================================*/
/*!
    @brief
    This function submits the config log pkt.

    @return
    None

*/
/*=========================================================================*/
extern void lte_pdcpul_log_submit_cfg_log_pkt
(
  uint8   cfg_reason,      /*!< cfg reason */
  uint8   num_released_rb, /*!< number of released RB */
  lte_rb_cfg_idx_t *released_rb_cfg_idx, /*!< released RB cfg index */
  uint8   num_add_modified_rb, /*!< number of add/modified RB */
  lte_pdcp_cfg_act_s *add_modified_rb /*!< cfg action for add/modified RB ID */
);


/*==============================================================================

  FUNCTION:  lte_pdcpdl_log_statistics

==============================================================================*/
/*!
    @brief
    log the statistics for the radio bearer

    @return
    None
*/
/*============================================================================*/
extern void lte_pdcpdl_log_statistics
(
  lte_pdcpdl_cb_s* rb_cb_ptr /*!< pointer to the control block */
);

/*==============================================================================

  FUNCTION:  lte_pdcpdl_log_submit_statistics_log_pkt

==============================================================================*/
/*!
    @brief
    submit the statistics log pkt to diag.

    @return
    None
*/
/*============================================================================*/
void lte_pdcpdl_log_submit_statistics_log_pkt(uint32 num_erros);

/*==============================================================================

  FUNCTION:  lte_pdcpul_log_statistics

==============================================================================*/
/*!
    @brief
    log the statistics for the radio bearer

    @return
    None
*/
/*============================================================================*/
extern void lte_pdcpul_log_statistics
(
  lte_pdcpul_cb_s* rb_cb_ptr /*!< pointer to the control block */
);

/*==============================================================================

  FUNCTION:  lte_pdcpul_log_submit_statistics_log_pkt

==============================================================================*/
/*!
    @brief
    submit the statistics log pkt to diag.

    @return
    None
*/
/*============================================================================*/
void lte_pdcpul_log_submit_statistics_log_pkt(void);

/*==============================================================================
  FUNCTION:  lte_pdcpul_log_delay_statistics

==============================================================================*/
/*!
    @brief
    log the delay statistics for the radio bearer

    @return
    None
*/
/*============================================================================*/
extern void lte_pdcpul_log_delay_statistics
(
  lte_pdcpul_cb_s* rb_cb_ptr /*!< pointer to the control block */
);

/*==============================================================================

  FUNCTION:  lte_pdcpul_log_submit_delay_statistics_log_pkt

==============================================================================*/
/*!
    @brief
    submit the statistics log pkt to diag.

    @return
    None
*/
/*============================================================================*/
void lte_pdcpul_log_submit_delay_statistics_log_pkt(void);


/*===========================================================================

  FUNCTION:  lte_pdcpdl_log_submit_cipher_pdu_log_pkt

===========================================================================*/
/*!
    @brief
    This function submits the CIPHER PDU log pkt.
    The log buffer is then reset to start as an empty buffer for the same stream

    @return
    None

*/
/*=========================================================================*/
void lte_pdcpdl_log_submit_cipher_pdu_log_pkt(void);

/*==============================================================================

  FUNCTION:  lte_pdcpdl_log_set_pdu_security_cfg

==============================================================================*/
/*!
    @brief
    Sets the SRB and DRB security configuration for ciphering and integrity
    log packets

    @return
    None
*/
/*============================================================================*/
void lte_pdcpdl_log_set_pdu_security_cfg(void);

/*==============================================================================

  FUNCTION:  lte_pdcpul_log_set_integ_pdu_security_cfg

==============================================================================*/
/*!
    @brief
    Sets the SRB security configuration for integrity log
    packet

    @return
    None
*/
/*============================================================================*/
void lte_pdcpul_log_set_integ_pdu_security_cfg(void);

/*==============================================================================

  FUNCTION:  lte_pdcpul_log_reset_integ_pdu_security_cfg

==============================================================================*/
/*!
    @brief
    Resets the SRB security configuration integrity log packets

    @return
    None
*/
/*============================================================================*/
void lte_pdcpul_log_reset_integ_pdu_security_cfg(void);

/*==============================================================================

  FUNCTION:  lte_pdcpdl_log_reset_pdu_security_cfg

==============================================================================*/
/*!
    @brief
    Resets the SRB and DRB security configuration for ciphering and integrity
    log packets

    @return
    None
*/
/*============================================================================*/
void lte_pdcpdl_log_reset_pdu_security_cfg(void);

/*==============================================================================

  FUNCTION:  lte_pdcpdl_log_set_ciph_pdu_fields

==============================================================================*/
/*!
    @brief
    Sets the COUNT and pdu_valid flag(if needed) field

    @return
    None
*/
/*============================================================================*/
void lte_pdcpdl_log_set_ciph_pdu_fields
(
  boolean valid_pdu, /*!< whether pdu is valid or not */
  uint32  count      /*!< PDCP COUNT for this pdu */
);

/*==============================================================================

  FUNCTION:  lte_pdcpdl_log_set_integ_pdu_fields

==============================================================================*/
/*!
    @brief
    Sets the COUNT and pdu_valid flag(if needed) field

    @return
    None
*/
/*============================================================================*/
void lte_pdcpdl_log_set_integ_pdu_fields
(
  boolean valid_pdu, /*!< whether pdu is valid or not */
  uint32  count,     /*!< PDCP COUNT for this pdu */
  uint32  received_maci, /*!< maci value received from n/w */
  uint32  computed_maci /*!< maci value computed by UE */
);

/*==============================================================================

  FUNCTION:  lte_pdcpdl_log_reset_last_ciph_integ_pdu_log_ptr

==============================================================================*/
/*!
    @brief
    reset the last cipher and integrity pdu log ptr to NULL.

    @detail
    last pdu log ptr can stay non-null in case of
    an invalid pdu and then if crypo done indication processes the next valid
    srb pdu, it might set the invalid log pdu to valid in the cipher\
    integrity log buffer memory

    @return
    None
*/
/*============================================================================*/
void lte_pdcpdl_log_reset_last_ciph_integ_pdu_log_ptr(void);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_log_submit_integ_pdu_log_pkt

===========================================================================*/
/*!
    @brief
    This function submits the INTEGRITY PDU log pkt.
    The log buffer is then reset to start as an empty buffer for the same stream

    @return
    None

*/
/*=========================================================================*/
void lte_pdcpdl_log_submit_integ_pdu_log_pkt(void);

/*===========================================================================

  FUNCTION:  lte_pdcpul_log_submit_integ_pdu_log_pkt

===========================================================================*/
/*!
    @brief
    This function submits the INTEGRITY PDU log pkt.
    The log buffer is then reset to start as an empty buffer for the same stream

    @return
    None

*/
/*=========================================================================*/
void lte_pdcpul_log_submit_integ_pdu_log_pkt(void);

void lte_pdcp_ul_log_error_handler
( 
  lte_pdcp_log_error_e  err_code /*!< error code  */
);

void lte_pdcp_dl_log_error_handler
( 
  lte_pdcp_log_error_e  err_code /*!< error code  */
);

void lte_pdcp_ul_log_meta_exhaustion
( 
  lte_rb_cfg_idx_t  rb_cfg_idx,     /*!< radio bearer configure index */
  uint32            ps_wm_in_bytes, /*!< ps2pdcp watermark in bytes */
  uint32            ps_wm_num_pkts  /*!< number of pkts in ps2pdcp watermark */
);

void lte_pdcp_ul_log_win_stall
(
  lte_rb_cfg_idx_t  rb_cfg_idx,     /*!< radio bearer configure index */
  uint32            ps_wm_in_bytes, /*!< ps2pdcp watermark in bytes */
  uint32            tx_seq          /*!< current tx_seq */
);

#endif /* LTE_PDCP_LOG_H */
