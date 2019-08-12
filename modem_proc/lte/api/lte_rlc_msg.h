/*!
  @file
  lte_rlc_msg.h

  @brief
  This file contains all External message IDs interfaces exported by RLC layer.


*/

/*===========================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_rlc_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/15   mg      CR904992: EMBMS DPD tradeoff G and 1x paging
01/22/15   mg      CR785048: Embms DSDS DPD feature - Phase 3
11/23/14   sb      CR745901: Turn AM to UM mode and do best effort deliver
                   when new Flow contrl level is triggered.
12/18/14   mg      CR 768623: Embms DSDS DPD feature - Phase 1
04/11/14   mg      CR647594: QSH Phase 1
06/12/14   ru      CR665154: SRLTE: Fast retransmission of UL RLC packets lost 
                   due to HARQ failures in Tuneaway
01/31/14   mg      CR606601: Extend '0xB097 LTE RLC UL Statistics' log packet to
                   include number of UM RLC PDUs missed 
12/12/13   mg      CR583687: Support eMBMS data path with new DS-DPM design
03/05/12   ax      clean up comments
08/23/11   ax      add eps_id as part of RLC statistics
07/18/11   ax      support RLC statistics query 
09/17/10   wshaw   add watchdog support for LTE tasks
06/27/10   ax      added UMID defs for LTE_RLCDL_FC_REQ
04/28/10   ar      added UMID defs for RLCUL and RLCDL START and STOP
                   request and confirm messages
03/26/09   ar      added UMID defs for LTE_RLCUL_FLUSH_RLCUL_LOG_BUF_IND and
                   LTE_RLCUL_FLUSH_PDCPUL_LOG_BUF_IND
12/04/08   ar      Added UMID definition for LTE_RLCUL_MAX_RETX_IND
11/07/08   ax      replace void* with actual msg type to help AST tools
11/07/08   ar      Changed the name of LTE_RLCDL_STATUS_IND to
                   LTE_RLCDL_CTRL_PDU_IND
10/12/08   ar      Added UMID for RLCUL periodic timer expiry
08/01/2008 ax      initial version
===========================================================================*/

#ifndef LTE_RLC_MSG_H
#define LTE_RLC_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "dsm_queue.h"
#include <msgr_lte.h>
#include <lte_app.h>
#include "lte.h"
#include "lte_as.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief RLCDL statistics per radio bearer
*/
typedef struct
{
  lte_rb_rlc_mode_e  rb_mode;    /*!< TM/UM/AM */
  lte_lc_id_t        lc_id;      /*!< logical channel ID */
  lte_rb_id_t        rb_id;      /*!< radio bearer ID: not unique */
  lte_rb_cfg_idx_t   rb_cfg_idx; /*!< RB configuration index: unique */
  lte_rb_type_e      rb_type;    /*!< SRB/DRB */
  lte_eps_id_t       eps_id;     /*!< EPS bearer ID, PDCP param */
  uint32  num_data_pdu;       /*!< number of RLC data pdu received */
  uint64  data_pdu_bytes;     /*!< RLC data PDU in bytes received */
  uint32  num_sdu;            /*!< number of RLC sdu reassembled */
  uint64  sdu_bytes;          /*!< RLC SDUs in bytes reassembled */
  uint32  num_comp_nack;      /*!< number of complete PDU NACKs sent in status */
  uint32  num_segm_nack;      /*!< number of segmented PDU NACKs sent */
} lte_rlcdl_rb_stats_s;

/*! @brief RLCDL statistics 
*/
typedef struct
{
  uint32                  num_active_rb;          /*!< number of active RB's */
  lte_rlcdl_rb_stats_s    rb_stats[LTE_MAX_ACTIVE_RB]; /*!< stats per RB, only 
            the first num_active_rb elements in the array are valid */
} lte_rlcdl_stats_s;

/*! @brief statistics request message for RLCDL
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;          /*!< common message router header */
  lte_rlcdl_stats_s*     stats_ptr;        /*!< pointer to the statistics */
} lte_rlcdl_stats_req_s;

/*! @brief RLCUL statistics per radio bearer
*/
typedef struct
{
  lte_rb_cfg_idx_t   rb_cfg_idx; /*!< RB configuration index: unique */
  uint32 num_new_data_pdus_txed;      /*!< Total number of new RLC PDUs txed */
  uint32 new_data_pdu_bytes_txed;     /*!< Total new data pdu bytes transmitted
                                           including header */
  uint32 num_sdus_txed;               /*!< Total number of new RLC SDUs txed */
  uint32 sdu_bytes_txed;              /*!< Total number of new RLC SDUs txed in
                                           bytes */
  uint32 num_pdus_re_txed;            /*!< Total number of re-txed RLC PDUs */
  uint32 pdu_bytes_re_txed;           /*!< Total pdu bytes re-transmitted */
} lte_rlcul_rb_stats_s;

/*! @brief RLCUL statistics 
*/
typedef struct
{
  uint32                  num_active_rb;          /*!< number of active RB's */
  lte_rlcul_rb_stats_s    rb_stats[LTE_MAX_ACTIVE_RB]; /*!< stats per RB only 
            the first num_active_rb elements in the array are valid */
} lte_rlcul_stats_s;

/*! @brief statistics request message for RLCUL
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;          /*!< common message router header */
  lte_rlcul_stats_s*     stats_ptr;        /*!< pointer to the statistics */
} lte_rlcul_stats_req_s;


/*! @brief EMBMS watermark registration request message for RLCDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;          /*!< common message router header */
  /* Message payload */
  uint8                               id;           /*!< Watermark id*/
  dsm_watermark_type*                 embms_wm_ptr; /*!< EMBMS WM ptr provided by DS */
} lte_rlcdl_embms_wm_register_req_msg_s;

/*! @brief EMBMS watermark deregistration request message for RLCDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;          /*!< common message router header */
  /* Message payload */
  uint8                               id;           /*!< Watermark id*/
} lte_rlcdl_embms_wm_deregister_req_msg_s;

/*! @brief EMBMS watermark registration confirmation message for RLCDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;          /*!< common message router header */
  /* Message payload */
  uint8                               id;           /*!< Watermark id*/
  lte_errno_e                         cnf_status;   /*!< confimation status message */
} lte_rlcdl_embms_wm_register_cnf_msg_s;

/*! @brief EMBMS watermark deregistration confirmation message for RLCDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;          /*!< common message router header */
  /* Message payload */
  uint8                               id;           /*!< Watermark id*/
  lte_errno_e                         cnf_status;   /*!< confimation status message */
} lte_rlcdl_embms_wm_deregister_cnf_msg_s;

/*! @brief QSH analysis process request message for RLCDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;          /*!< common message router header */
  /*! whether to run analysis, reset stats ... */
  uint32 action;   
  /*! category mask. Use enum qsh_cat_e values to define mask e.g. 
      QSH_CAT_CFG|QSH_CAT_PERF (since the values are bit mask simple
      "or" will work) */
  uint32 category_mask;  
  /*! whether the registered CB can run in sync/async mode.
      When called with async_allowed = false the clients MUST RUN 
      the registered CB in the same task context which invoked their call back  
      (e.g. error fatal). When called with async_allowed = true
      the clients have an option to run their registered CB in async mode. */
  boolean async_allowed; 
} lte_rlcdl_qsh_req_msg_s;

/*! @brief QSH analysis process request message for RLCUL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;          /*!< common message router header */
  /*! whether to run analysis, reset stats ... */
  uint32 action;   
  /*! category mask. Use enum qsh_cat_e values to define mask e.g. 
      QSH_CAT_CFG|QSH_CAT_PERF (since the values are bit mask simple
      "or" will work) */
  uint32 category_mask;  
  /*! whether the registered CB can run in sync/async mode.
      When called with async_allowed = false the clients MUST RUN 
      the registered CB in the same task context which invoked their call back  
      (e.g. error fatal). When called with async_allowed = true
      the clients have an option to run their registered CB in async mode. */
  boolean async_allowed; 
} lte_rlcul_qsh_req_msg_s;





/*=============================================================================

                   EXTERNAL ENUMERATION TYPES

=============================================================================*/

/*! @brief UMID for all REQUEST accepted by RLCUL
*/
enum
{
  MSGR_DEFINE_UMID(LTE, RLCUL, REQ, CFG, 0x0, lte_rlcul_cfg_req_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, REQ, START, 0x1, lte_rlc_start_req_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, REQ, STOP, 0x2, lte_rlc_stop_req_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, REQ, STATS, 0x3, lte_rlcul_stats_req_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, REQ, QSH, 0x4, lte_rlcul_qsh_req_msg_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, REQ, ACK_TX_STS, 0x5, lte_rlcul_ack_tx_sts_req_s),
  LTE_RLCUL_MAX_REQ,
  LTE_RLCUL_LAST_REQ = LTE_RLCUL_MAX_REQ - 1
} ;

/*! @brief UMID for all CONFIRMATION sent by RLCUL
*/
enum
{
  MSGR_DEFINE_UMID(LTE, RLCUL, CNF, CFG, 0x0, lte_rlc_cfg_cnf_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, CNF, START, 0x1, lte_rlc_start_cnf_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, CNF, STOP, 0x2, lte_rlc_stop_cnf_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, CNF, STATS, 0x3, msgr_hdr_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, CNF, ACK_TX_STS, 0x4, lte_rlcul_ack_tx_sts_cnf_s),
  LTE_RLCUL_MAX_CNF,
  LTE_RLCUL_LAST_CNF = LTE_RLCUL_MAX_CNF - 1
} ;

/*! @brief UMID for all INDICATIONS sent by RLCUL
*/
enum
{
  MSGR_DEFINE_UMID(LTE, RLCUL, IND, ACK, 0x0,
                   lte_rlcul_ack_ind_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, IND, FREE_PDU, 0x1,
                   lte_rlcul_free_pdu_ind_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, IND, RE_EST_STATUS, 0x2,
                   lte_rlcul_re_est_status_ind_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, IND, MAX_RETX, 0x3,
                   lte_rlcul_max_retx_ind_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, IND, FLUSH_RLCUL_LOG_BUF, 0x4,
                   lte_rlcul_flush_rlcul_log_buf_ind_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, IND, FLUSH_PDCPUL_LOG_BUF, 0x5,
                   lte_rlcul_flush_pdcpul_log_buf_ind_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, IND, STOP_STAT_PROHIBIT, 0x6,
                   lte_rlcul_am_stop_stat_prohibit_ind_s),
  LTE_RLCUL_MAX_IND,
  LTE_RLCUL_LAST_IND = LTE_RLCUL_MAX_IND - 1
} ;

/*! @brief UMID for RLCUL periodic timer expiry
*/
enum
{
  MSGR_DEFINE_UMID(LTE, RLCUL, TMRI, PERIODIC_TIMER_EXP, 0x0, msgr_hdr_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, TMRI, WDOG_TIMER_EXPIRED, 0x1, msgr_hdr_s),
  LTE_RLCUL_MAX_TMRI,
  LTE_RLCUL_LAST_TMRI = LTE_RLCUL_MAX_TMRI - 1
} ;

/*! @brief UMID for all REQUEST accepted by RLCDL
*/
enum
{
  MSGR_DEFINE_UMID(LTE, RLCDL, REQ, CFG, 0x0, lte_rlcdl_cfg_req_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, REQ, START, 0x1, lte_rlc_start_req_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, REQ, STOP, 0x2, lte_rlc_stop_req_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, REQ, FC, 0x3, cfm_fc_cmd_msg_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, REQ, STATS, 0x4, lte_rlcdl_stats_req_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, REQ, EMBMS_REG, 0x5, lte_rlcdl_embms_wm_register_req_msg_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, REQ, EMBMS_DEREG, 0x6, lte_rlcdl_embms_wm_deregister_req_msg_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, REQ, QSH, 0x7, lte_rlcdl_qsh_req_msg_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, REQ, FC_LEVEL1, 0x8, cfm_fc_cmd_msg_s),
  LTE_RLCDL_MAX_REQ,
  LTE_RLCDL_LAST_REQ = LTE_RLCDL_MAX_REQ - 1
} ;

/*! @brief UMID for all CONFIRMATION sent by RLCDL
*/
enum
{
  MSGR_DEFINE_UMID(LTE, RLCDL, CNF, CFG, 0x0, lte_rlc_cfg_cnf_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, CNF, START, 0x1, lte_rlc_start_cnf_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, CNF, STOP, 0x2, lte_rlc_stop_cnf_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, CNF, STATS, 0x3, msgr_hdr_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, CNF, EMBMS_REG, 0x4, lte_rlcdl_embms_wm_register_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, CNF, EMBMS_DEREG, 0x5, lte_rlcdl_embms_wm_deregister_cnf_msg_s),
  LTE_RLCDL_MAX_CNF,
  LTE_RLCDL_LAST_CNF = LTE_RLCDL_MAX_CNF - 1
} ;

/*! @brief UMID for all indications sent by RLCDL
*/
enum
{
  MSGR_DEFINE_UMID(LTE, RLCDL, IND, DATA, 0x0, msgr_hdr_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, IND, CTRL_PDU, 0x1, lte_rlcdl_ctrl_pdu_ind_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, IND, MCCH_PDU, 0x2, lte_rlcdl_mcch_pdu_ind_s),
  LTE_RLCDL_MAX_IND,
  LTE_RLCDL_LAST_IND = LTE_RLCDL_MAX_IND - 1
} ;

/*! @brief UMID for RLCDL timer
*/
enum
{
  MSGR_DEFINE_UMID(LTE, RLCDL, TMRI, EXPIRE, 0x0, msgr_hdr_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, TMRI, WDOG_TIMER_EXPIRED, 0x1, msgr_hdr_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, TMRI, EMBMS_SEG_DUR_TIMER_EXPIRED, 0x2, msgr_hdr_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, TMRI, EMBMS_BURST_END_TIMER_EXPIRED, 0x3, msgr_hdr_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, TMRI, EMBMS_ONEX_PAGE_DECODE_TIMER_EXPIRED, 0x4, msgr_hdr_s),
  LTE_RLCDL_MAX_TMRI,
  LTE_RLCDL_LAST_TMRI = LTE_RLCDL_MAX_TMRI - 1
} ;

/*! @brief Supervisory messages that RLC DL receives or throws
*/

enum
{
  MSGR_DEFINE_UMID(LTE, RLCDL, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(LTE, RLCDL, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY,
                   none),
  MSGR_DEFINE_UMID(LTE, RLCDL, SPR, THREAD_KILL, LTE_APP_ID_THREAD_KILL,
                   none)
};

/*! @brief Supervisory messages that RLC UL receives or throws
*/
enum
{
  MSGR_DEFINE_UMID(LTE, RLCUL, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(LTE, RLCUL, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY,
                   none),
  MSGR_DEFINE_UMID(LTE, RLCUL, SPR, THREAD_KILL, LTE_APP_ID_THREAD_KILL,
                   none)
};
#endif /* LTE_RLC_MSG_H */
