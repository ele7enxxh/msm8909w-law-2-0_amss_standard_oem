/*!
  @file
  lte_rlcdl_log.h

  @brief
  RLCDL log packet related functionality.

  @author
  axiao

*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcdl_log.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/31/14   mg      CR606601: Extend '0xB097 LTE RLC UL Statistics' log packet to
                   include number of UM RLC PDUs missed 
08/22/12   ax      CR383618: L2 modem heap use reduction phase I  
05/21/12   sb      CR 363363: Compiler and Lint warnings fixes
04/17/12   ru      CR 342859: Added eMBMS logging changes  
12/16/11   ax      feature clean up 
07/23/11   ax      add persistent error count and lte_rlcdl_log_update_error()
06/28/10   ax      added flow control related change in logging
03/18/10   ax      added profile log pkt
10/19/09   ax      log packet enhancement
09/17/09   ax      add reserved field to statistics log pkt.
09/02/09   ax      removed multiple invokation of LTE_ERROR in RLCDl_LOG_ERROR
08/31/09   ax      replace LTE_RLC_ERROR with DL specific LTE_RLCDL_LOG_ERROR
08/10/09   ax      added support for RLC statistics log pkt
08/06/09   ax      moved PDU_BUF_SZ to common location lte_rlc_cfg.h
05/06/09   ax      added array_size to cfg log pkt to sheild log parsers from 
                   potential LTE_MAX_ACTIVE_RB change in value in the future
04/29/09   ax      merged rb cfg subpkt into pdu subpkt
04/22/09   ax      declare all sub struct within log struct PACK
04/17/09   ax      update RLCDL logging according to the ICD
03/08/09   ax      move log id & subpkt log id into lte_rlc_log.h
02/03/09   ax      complete RLCDL packet logging
01/13/09   ax      initial version
==============================================================================*/

#ifndef LTE_RLCDL_LOG_H
#define LTE_RLCDL_LOG_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <genlog.h>
#include <dsm_pool.h>
#include "lte_as.h"
#include "lte_rlc_log.h"
#include "lte_rlcdli.h"
#include "lte_rlcdl_cfg.h"
#include "lte_rlc_cfg.h"
#include "lte_l2_profile.h"
#include "lte_rlcdl_embms.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/  

#define LTE_RLCDL_LOG_ERROR(fmt, a, b, c)                                     \
  MSG_3(MSG_SSID_LTE_RLCDL, MSG_LEGACY_FATAL,  fmt, a, b, c );                \
  lte_rlcdl_log_error_handler()

#ifdef _WIN32
    #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

#define LTE_RLCDL_LOG_AREA_PMCH_LCID 5
/*! @brief mark the received PDU with different category
*/
typedef enum
{
  LTE_RLCDL_LOG_PDU_DATA = 0,   /*!< Recieved RLC data PDU */
  LTE_RLCDL_LOG_PDU_CTRL,       /*!< Recieved RLC control PDU */
  LTE_RLCDL_LOG_PDU_INVALID,    /*!< Recieved RLC PDU deemed invalid */
  LTE_RLCDL_LOG_PDU_OUT_RX_WIN, /*!< Recieved RLC PDU outside receiving window */
  LTE_RLCDL_LOG_PDU_DROPPED_FC, /*!< Recieved RLC PDU dropped due to flow control */
  LTE_RLCDL_LOG_UM_PDU_DISCARDED,  /*!< Recieved UM RLC PDU discarded */
  LTE_RLCDL_LOG_PDU_MAX     /*!< MAX */
} lte_rlcdl_log_pdu_status_e;

/*! @brief s enumeration of errors being tracked
*/
typedef enum
{
  LTE_RLCDL_LOG_ERROR_GENERIC   = 0,   /*!< generic error */
  LTE_RLCDL_LOG_ERROR_HUGE_SDU  = 1,   /*!< detection of huge SDU */
  LTE_RLCDL_LOG_ERROR_MAX              /*!< MAX */
} lte_rlcdl_log_error_e;

/*! @brief DL UM configuration definition
*/
typedef PACK(struct)
{
  uint16  t_reordering; /*!< Indicates the value of timer T_reordering in ms */
  uint8   sn_length;    /*!< Indicates the UM RLC SN field size in bits. */
} lte_rlcdl_log_um_cfg_s;

/*! @brief DL AM configuration definition
*/
typedef PACK(struct)
{
  uint16  t_reordering;      /*!< Indicates the value of timer
                                  T_reordering in ms */
  uint16  t_status_prohibit; /*!< Indicates the value of timer
                                  T_status_prohibit in ms */
} lte_rlcdl_log_am_cfg_s;

/*! @brief union of DL cfg
*/
typedef PACK(union)
{
  lte_rlcdl_log_um_cfg_s  um_cfg; /*!< configuration for UM */
  lte_rlcdl_log_am_cfg_s  am_cfg; /*!< configuration for AM */
} lte_rlcdl_log_cfg_u;

/*! @brief DL RB info definition
*/
typedef PACK(struct)
{
  lte_rlc_log_rb_mode_t  rb_mode;    /*!< TM/UM/AM */
  lte_lc_id_t            lc_id;      /*!< logical channel ID */
  lte_rb_id_t            rb_id;      /*!< radio bearer ID: not unique */
  lte_rb_cfg_idx_t       rb_cfg_idx; /*!< RB configuration index: unique */
  lte_rlc_log_rb_type_t  rb_type;    /*!< SRB/DRB */
  lte_rlcdl_log_cfg_u    cfg;        /*!< configuration of the RB */
} lte_rlcdl_log_rb_info_s;

/*! @brief radio bearer RLC statistics counters
*/
typedef PACK(struct)
{
  uint32  num_data_pdu;       /*!< number of RLC data pdu received */
  uint64  data_pdu_bytes;     /*!< RLC data PDU in bytes received */
  uint32  num_status_rxed;    /*!< number of status rxed and forwarded to UL */
  uint64  status_rxed_bytes;  /*!< number of bytes in status rxed for UL */
  uint32  num_invalid_pdu;    /*!< number of invalid PDUs */
  uint64  invalid_pdu_bytes;  /*!< invalid PDU in bytes */
  uint32  num_retx_pdu;       /*!< number of retx PDUs from receiver perspective
                                   only the transmitter knows the real number */
  uint64  retx_pdu_bytes;     /*!< retx PDU in bytes */
  uint32  num_dup_pdu;        /*!< number of duplicate PDUs received */
  uint64  dup_pdu_bytes;      /*!< bytes that are received as duplicates */
  uint32  num_dropped_pdu;    /*!< number of dropped PDUs, excluding dups */
  uint64  dropped_pdu_bytes;  /*!< dropped PDU in bytes, excluding dups */
  uint32  num_dropped_pdu_fc; /*!< number of dropped PDUs due to flow control */
  uint64  dropped_pdu_bytes_fc;/*!< dropped PDU in bytes due to flow control */
  uint32  num_sdu;            /*!< number of RLC sdu reassembled */
  uint64  sdu_bytes;          /*!< RLC SDUs in bytes reassembled */
  uint32  num_nonseq_sdu;     /*!< number of out of order SDU reassembled in HO */
  uint32  num_status_sent;    /*!< number of status PDU sent */
  uint32  num_comp_nack;      /*!< number of complete PDU NACKs sent in status */
  uint32  num_segm_nack;      /*!< number of segmented PDU NACKs sent */
  uint32  num_reord_exp;      /*!< number of times t_reordering expired */
  uint32  num_reord_start;    /*!< number of times t_reordering started */
  uint32  num_missed_um_pdu;  /*!< number of UM PDUs missing on DL*/
  uint32  reserved;           /*!< reserved */
} lte_rlcdl_log_stat_counter_s;

/*! @brief DL RB statistics definition
*/
typedef PACK(struct)
{
  lte_rb_cfg_idx_t       rb_cfg_idx; /*!< RB configuration index: unique */
  lte_rlc_log_rb_mode_t  rb_mode;    /*!< TM/UM/AM */
  uint32                 num_rst;    /*!< num re-establishment since RB added */
  lte_rlcdl_log_stat_counter_s tot; /*!<total stats */
  lte_rlcdl_log_stat_counter_s curr;/*!<curr stats since last re-establishment*/
} lte_rlcdl_log_rb_stat_s;

/*! @brief RLC DL configuation subpkt
*/
typedef PACK(struct)
{
  lte_rlc_log_cfg_reason_t   cfg_reason;       /*!< configuration reason */
  uint8                      array_size;   /*!< this is only useful for parser
                                         should be set to LTE_MAX_ACTIVE_RB */
  uint8                      num_released_rb;  /*!< number of released RBs */
  lte_rb_cfg_idx_t           released_rb_cfg_idx[LTE_MAX_ACTIVE_RB]; /*!< 
                                      Released RB cfg index */
  uint8                      num_modified_rb;  /*!< number of add/modified RBs*/
  lte_rlc_log_modified_rb_s  modified_rb[LTE_MAX_ACTIVE_RB]; /*!< add/modified 
                                                      RB configuration index */
  uint8                      num_active_rb;  /*!< number of active RBs*/
  lte_rlcdl_log_rb_info_s    active_rb_info[LTE_MAX_ACTIVE_RB]; /*!<rb info */
} lte_rlcdl_log_sub_pkt_cfg_s;

/*! @brief Active mrb array 
*/
typedef PACK(struct)
{
  uint8                  area_id;
  uint8                  pmch_id;
  uint8                  mrb_id;
  uint8                  mrb_cfg_index;
  uint8                  lc_id;
} lte_rlcdl_log_embms_active_mrb;

/*! @brief RLC DL eMBMS configuration subpkt 
*/
typedef PACK(struct)
{
  uint8                  num_released_mrb;
  uint8                  released_mrb_cfg_index[LTE_MAX_ACTIVE_MRB];
  uint8                  num_added_mrb;
  uint8                  added_mrb_cfg_index[LTE_MAX_ACTIVE_MRB];
  uint8                  num_active_mrb;
  lte_rlcdl_log_embms_active_mrb active_mrb[LTE_MAX_ACTIVE_MRB];
} lte_rlcdl_log_embms_sub_pkt_cfg_s;

/*! @brief This struct defines the format of PDU subpacket */
typedef PACK(struct)
{
  lte_rb_cfg_idx_t       rb_cfg_idx; /*!< RB configuration index: unique */
  lte_rlc_log_rb_mode_t  rb_mode;    /*!< action on the RB */
  uint8                  sn_length;  /*!< sequence length */
  uint8                  reserved;   /*!< filler */
  uint16  log_mask;                       /*!< log mask */
  uint16  vr_r;                       /*!< VR(R) */
  uint16  vr_x;                       /*!< VR(X) */
  uint16  vr_ms;                      /*!< VR(MS) */
  uint16  vr_h;                       /*!< VR(H) */
  uint16  num_pdus;                       /*!< number of pdu's */
  uint8   log_buf[LTE_RLC_CFG_DL_LOG_PDU_BUF_SZ]; /*!< buffer for the PDU's */
  uint16  size;      /*!< Size of the payload, this is not sent cross the wire */
} lte_rlcdl_log_sub_pkt_pdu_s;

/*! @brief This struct defines per area+pmch combo RLC eMBMS statistics*/
typedef PACK(struct)
{
  uint8   area_id;
  uint8   pmch_id;
  uint8   lc_id;
  uint32  num_data_pdu;
  uint32  num_data_pdu_bytes;
  uint32  num_invalid_pdu;
  uint32  num_invalid_pdu_bytes;
  uint32  num_sdu;
  uint32  num_sdu_bytes;
  uint32  num_holes;
  uint32  num_holes_discard;
  uint32  num_sdu_discard_bytes;
  uint32  reserved1; 
  uint32  reserved2;
} lte_rlcdl_log_embms_sub_pkt_stats_s;

/*! @brief This struct defines the format of the RLC eMBMS statistics subpkt*/
typedef PACK(struct)
{
  uint8   total_num_errors;
  uint8   total_num_dropped_pdus;
  uint8   total_num_dropped_bytes;
  lte_rlcdl_log_embms_sub_pkt_stats_s area_pmch_combo[LTE_RLCDL_LOG_AREA_PMCH_LCID]; 
} lte_rlcdl_log_embms_stats_s;

/*! @brief This struct defines the format of UNIFIED RLC DL PDU subpacket */
typedef PACK(struct)
{
  uint16  num_pdus;                    /*! number of pdu's */
  uint8   log_buf[LTE_RLC_CFG_DL_LOG_PDU_BUF_SZ]; /*!< buffer for PDU's */
  uint16  size;      /*!< Size of the payload, this is not sent cross the wire */
} lte_rlcdl_log_unified_sub_pkt_pdu_s;

/*! @brief This struct defines the format of statistics subpacket */
typedef PACK(struct)
{
  uint8                  num_rb;     /*!< number of radio bearer stats logged */
  uint32                 rlc_pdcp_q_full_cnt; /*!< number of times the q is full */
  uint32                 num_rlcdl_errors;    /*!< number of rlcdl errors */
  lte_rlcdl_log_rb_stat_s rb_stats[LTE_MAX_ACTIVE_RB]; /*!< RB statistics */
} lte_rlcdl_log_sub_pkt_stats_s;

/*! @brief info buffer and max info
*/
#define LTE_RLCDL_LOG_PROFILE_ARRAY_SZ  (LTE_L2_PROFILE_HISTORY_BUF_SZ + 1)

/*! @brief the last element of the array is for MAX cycle info
*/
#define LTE_RLCDL_LOG_PROFILE_MAX_ARR_IDX     LTE_L2_PROFILE_HISTORY_BUF_SZ

/*! @brief This struct defines the format of datapath profile subpacket */
typedef PACK(struct)
{
  uint32           umid;          /*!< UMID that helps to identify the msg*/
  uint32           cycle_counts;  /*!< cycle counts between start and end */
  lte_sfn_s        sfn;           /*!< timing which can be used to corolation*/
} lte_rlcdl_log_profile_info_s;

/*! @brief This struct defines the format of datapath profile subpacket */
typedef PACK(struct)
{
  lte_rlcdl_log_profile_info_s info[LTE_L2_PROFILE_TASK_MAX_ID]
                                   [LTE_RLCDL_LOG_PROFILE_ARRAY_SZ]; 
  /*!< 2-D array of profile info */
} lte_rlcdl_log_sub_pkt_profile_s;

#ifdef _WIN32
   #pragma pack(pop) // Revert alignment to what it was previously
#endif

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern void lte_rlcdl_log_init(void);

extern void lte_rlcdl_log_deinit(void);

extern void lte_rlcdl_log_update_status(void);

extern void lte_rlcdl_log_remove_rb
(
  const lte_rlcdli_ctrl_blk_s*  ctrl_blk_ptr
);

extern void lte_rlcdl_log_add_rb
(
  lte_rb_cfg_idx_t            rb_cfg_idx,
  const lte_rlcdl_rb_info_s*  rb_info_ptr
);

extern void lte_rlcdl_log_modify_rb
(
  const lte_rlcdli_ctrl_blk_s*   ctrl_blk_ptr,
  const lte_rlcdl_rb_info_s*     rb_info_ptr  
);

extern void lte_rlcdl_log_pdu
(
  lte_rlcdli_ctrl_blk_ptr        ctrl_blk_ptr,
  const dsm_item_type*           data_pdu_ptr,
  uint16                         pkt_len,   
  uint16                         log_size,
  lte_sfn_s                      sfn,
  uint16                         mask,
  lte_rlcdl_log_pdu_status_e     status
);

extern void lte_rlcdl_log_embms_statistics( void );

extern void lte_rlcdl_log_statistics
(
  const lte_rlcdli_ctrl_blk_s*     ctrl_blk_ptr
);

extern void lte_rlcdl_log_statistics_accumulate
(
  lte_rlcdli_ctrl_blk_ptr     ctrl_blk_ptr
);

extern void lte_rlcdl_log_submit_pdu_log_pkt
(
  const lte_rlcdli_ctrl_blk_s*     ctrl_blk_ptr
);

extern void lte_rlcdl_log_submit_cfg_log_pkt
(
  const lte_rlcdl_cfg_req_s*      cfg_msg_ptr,
  lte_rlcdl_cfg_s*                cfg_ptr
);

extern void lte_rlcdl_log_embms_submit_cfg_log_pkt
(
  const lte_rlcdl_cfg_req_s*      cfg_msg_ptr
);

extern void lte_rlcdl_log_embms_submit_statistics_log_pkt( void );

extern void lte_rlcdl_log_submit_statistics_log_pkt( void );

extern uint16 lte_rlcdl_log_get_log_mask( void );

extern void lte_rlcdl_log_error_handler( void );

extern void lte_rlcdl_log_reset_error_counter( void );

extern void lte_rlcdl_log_update_error
(
  lte_rlcdl_log_error_e    err_code
);

extern void lte_rlcdl_log_embms_pdu
(
  lte_rlcdl_embms_cb_s*         cb_ptr,        
  const dsm_item_type*          pdu_dsm_ptr,   
  lte_mac_embms_buf_element_s*  circ_q_elem,   
  uint16                        pdu_size,      
  uint16                        bytes_to_log,  
  uint16                        mask,             
  lte_rlcdl_log_pdu_status_e    status          
);

extern void lte_rlcdl_log_embms_submit_pdu_log_pkt
(
  void
);

#define LTE_RLCDL_LOG_SUBMIT_PROFILE(counter)

#endif /* LTE_RLCDL_LOG_H */
