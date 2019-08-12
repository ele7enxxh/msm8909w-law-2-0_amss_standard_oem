#ifndef RFLM_TXAGC_HAL_H
#define RFLM_TXAGC_HAL_H
/*
  @file
  rflm_txagc_hal.h

  @brief
  This file contains RFLM  txagc hal definitions, enumerations and prototypes that
  are used in rflm scope.

  @addtogroup RFLM_TXAGC_HAL_H
  @{
*/

/*=============================================================================

                R F L M TXAGC HAL   H E A D E R   F I L E 
       
DESCRIPTION
  This file contains declarations and definitions to support RF frent end driver txagc module.

Copyright (c) 2013-2014  by Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/11/14   hdz     Decoupling rfmeas_event_infor_type from Tx module
08/14/14   hdz     Added rflm_txagc_cmn_save_path_delay_params
08/04/14   cd      Allow digital back-off more than 12.7 dB
07/23/14   hdz     Added et_delay
05/19/14   hdz     Added support fo resend issue_seq
05/07/14   ka      Common tx agc / tx lin fix for fbrx gain state setting
04/04/14   hdz     Added rflm_txagc_set_task_id()
02/20/14   hdz     Added rflm_txagc_w_commit_fixup
02/06/14   hdz     TxAGC timeline optimization
12/20/13   cri     Enable ET Delay
12/04/13   hdz     Added rflm_txagc_get_tx_chain_idx() and 
                   rflm_txagc_get_lmem_addr()
12/03/13   cd      Added API to obtain IQ gain adjustment
12/3/13    hdz     Added RFLM_TXAGC_CCS_TRIG_OFS_TO_USMTR_OFS
11/13/13   hdz     Change h_id to txagc_template
10/24/13   hdz     Added iq_gain_fw and env_scale_fw
10/20/13   hdz     Add rflm_txagc_modem_api.h
10/17/13   hdz     Added rflm_txagc_cmn_ccs_fixup_modem_override()
10/01/13   hdz     Move rflm_txagc_prepare_seq() to rflm_txagc_api.h
09/18/13   hdz     Removed un-used API
09/13/13   hdz     Changed to use DM interface buffer for txagc sequence
08/29/13   hdz     Added rflm_txagc_create_generic_dma_seq()
08/27/13   hdz     Added support for new fixup table type
08/15/13   hdz     Changed interface
08/12/13   hdz     Added rflm_txagc_create_fixup_tbl()
08/05/13   hdz     Fixed fixup API for RFFE_EXT
07/31/13   hdz     Added rflm_txagc_fixup_get_tbl()
07/31/13   hdz     Added fixup tbl support
07/28/13   hdz     Initial creation

===========================================================================*/
#include "rflm_txagc.h"
#include "rflm_txagc_lookup_opt.h"
#include "rflm_api_c2k.h"
#include "rflm_ccs_rf_intf.h"
#include "rflm_dm_api.h"
#include "rflm_txagc_modem_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ccs trigger time number of bits*/
#define RFLM_TXAGC_CCS_COUNT_BITS                                             22
/* max number of txagc issue seq timing fixup, which should be equal or less
   than max number of task in issue_seq                                       */
#define RFLM_TXAGC_MAX_NUM_TIMING_FIXUP                                       16

/* max number of dummy trigger writes, 2 for QPEOTs, 2 for WTR, 1 for FBRx*/
#define RFLM_TXAGC_MAX_NUM_DUMMY_TRIGGER                                       5

/* size of resend txagc table in words, used by teches to allocate resend table in CCS
  header: 1 (PDM addr)
  static table data: 5  (dummy trigger addr writes)
  dynamic table data: 16
  static table + dynamc table index: 6
  total = 28
  Reserved 4 words
*/
#define RFLM_TXAGC_SIZE_OF_RESEND_TABLE_WORDS                                 35

/* number entries of delta table, used to define Q6 resend table
  Current usage:  
  static table data: 5
  dynamic table data: 16   
  total = 21
 */
#define RFLM_CCS_RF_MAX_ISSUE_SEQ_DELTA_TABLE_ENTRY                           30
/* 
  @brief
  Convert from ccs trigger time 22-bits into ustmr bits    
 
  @detail
  If input is less than 2^(22-1) -1, then output = input;
  otherwise, output is wraparound at ustmr number of bits.
  e.g.,  input = 10; output = 10;
         input = 0x3ffff5; output = 0xfffff5;
*/
#define RFLM_TXAGC_CCS_TRIG_OFS_TO_USMTR_OFS(x)      \
  ((x) > BITMASK(RFLM_TXAGC_CCS_COUNT_BITS-1)?    \
   ( BITMASK(USTMR_COUNT_BITS) -  BITMASK(RFLM_TXAGC_CCS_COUNT_BITS) + (x) ):\
   (x) )

/*!  @brief   uint8 addr and data pair struct */
typedef struct
{
  /*! @brief data */  
  uint8 data;

  /*! @brief addr */    
  uint8 addr;
  
}rflm_txagc_hal_u8_data_pair_t;

/*!  @brief   fixup data type */
typedef union
{
  /*! @brief 8-bits addr */  
  uint8 addr_8;

  /*! @brief 16-bits addr */  
  uint16 addr_16;
}rflm_txagc_hal_fixup_addr_t;

/*!  @brief  rflm txagc hal data type  */
typedef union
{
  /*! @brief data uint32 */  
  uint32 data_uint32;

  /*! @brief data for uint8 arrays */  
  uint8  data_uint8[4];
  
}rflm_txagc_hal_u32_u8_t;

/*!  @brief  rflm txagc fixup config data type  */
typedef struct
{
  /*!  txlm_handle_id   */
  uint32 handle_id;

  /*! RFLM_TXAGC_EVENT_ID  */
  uint32 txagc_event_id;

  /*! RFLM_TXAGC_DMA_SEQUENCE_ID    */
  uint32 txagc_dma_seq_id;

  /*! flag to do fbrx or not    */
  boolean do_fbrx;

  /*! pointer to txagc memory */
  rflm_txagc_event_template_t *txagc_template;

  /*! txagc ccs action time*/
  rflm_ustmr_t action_time;

  /*!iq back off in fw format*/
  uint32 iq_gain_fw;

  /*!evn_backoff in fw format*/
  uint32 env_scale_fw;
  
  /*! address of ET Delay data type */
  uint32 et_delay;
  
  /*! RFLM_TxAGC settting params   */
  rflm_txagc_script_input_t *script_setting;

  /*! lmem inpput params   */
  rflm_txagc_modem_lmem_cfg_t *lmem_cfg;
  
}rflm_txagc_fixup_cfg_t;

/*! 
  @brief CCS RF issue_seq delta table used in issue_seq update task.
*/
typedef struct
{
	/*!< header section */
   uint32 seq_ptr;                /*!< pointer to the template seq ptr */
   uint32 num_of_entry;           /*!< the number of entries for this table */
   /*!< Data section */
   uint32 table_entry [RFLM_CCS_RF_MAX_ISSUE_SEQ_DELTA_TABLE_ENTRY];  /*!< data to update issue_seq */
   /*!< Offset section */
   /*!< The entry_offset[i] is used to store address offset of table_entry[i] from the beginning of seq_ptr. 
	    Its unit is the number of uint32.seq_ptr header is 4 uint32, so the smallest can only start from 4 (source field for the 1st DMAC). */
   uint8 entry_offset [RFLM_CCS_RF_MAX_ISSUE_SEQ_DELTA_TABLE_ENTRY];   
   uint32 num_of_static_entry;   /*!< the number of static entries for this table */
}rflm_txagc_ccs_delta_table_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief Iref start/stop time structure
*/
typedef struct
{
  /*! iref start time in ustmr */
  uint32 dac_iref_update_start_time;

  /*! iref stop time in ustmr */
  uint32 dac_iref_update_stop_time;

}rflm_txagc_iref_timing_t;

/*---------------------------------------------------------------------------*/
typedef void 
(*rflm_txagc_fixup_rffe_ssbi_fp)
(
  uint32 *des_ptr,
  rflm_txagc_hal_u32_u8_t *src_ptr,
  const rflm_txagc_fixup_data_t *fixup_ptr,
  uint32 num_trans,
  uint8 *data_ptr
);

/*---------------------------------------------------------------------------*/
typedef void
(*rflm_txagc_fixup_single_rffe_ssbi_fp)
(
  uint32 *des_ptr,
  rflm_txagc_hal_u32_u8_t *src_ptr,
  const rflm_txagc_fixup_data_t *fixup_ptr,
  uint8 *data_ptr
);

/*---------------------------------------------------------------------------*/
typedef void
(*rflm_txagc_fixup_rffe_ext_fp)
(
  uint32 *des_ptr,
  rflm_txagc_hal_u32_u8_t *src_ptr,
  const rflm_txagc_fixup_data_t *fixup_ptr,
  uint32 num_trans, 
  rflm_ccs_rf_transaction_rffe_ext_rw_t *rffe_ext_data_ptr,
  uint8 *data_ptr
);

/*---------------------------------------------------------------------------*/
typedef void 
(*rflm_txagc_fixup_single_rffe_ext_fp)
(
  uint32 *des_ptr,
  rflm_txagc_hal_u32_u8_t *src_ptr,
  const rflm_txagc_fixup_data_t *fixup_ptr,
  rflm_ccs_rf_transaction_rffe_ext_rw_t *rffe_ext_data_ptr,
  uint8 *data_ptr
);

/*---------------------------------------------------------------------------*/
typedef void
(*rflm_txagc_fixup_intf_fp)
(
  uint32 *des_ptr,
  rflm_txagc_hal_u32_u8_t *src_ptr,
  const rflm_txagc_fixup_data_t *fixup_ptr,
  uint32 num_trans,
  rflm_ccs_rf_transaction_rffe_ext_rw_t *rffe_ext_data_ptr,
  uint8 *data_ptr,
  rflm_txagc_task_type_t task_type
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_fixup_rffe_ssbi
(
  uint32 *des_ptr,
  rflm_txagc_hal_u32_u8_t *src_ptr,
  const rflm_txagc_fixup_data_t *fixup_ptr,
  uint32 num_trans,
  uint8 *data_ptr
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_fixup_rffe_ext
(
  uint32 *des_ptr,
  rflm_txagc_hal_u32_u8_t *src_ptr,
  const rflm_txagc_fixup_data_t *fixup_ptr,
  uint32 num_trans, 
  rflm_ccs_rf_transaction_rffe_ext_rw_t *rffe_ext_data_ptr,
  uint8 *data_ptr
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_fixup_grfc
(
  uint32 *des_ptr,
  const rflm_txagc_fixup_data_t *fixup_ptr,
  uint32 grfc_mask
);

/*---------------------------------------------------------------------------*/
rflm_txagc_fixup_data_t*
rflm_txagc_fixup_get_tbl
(
  rflm_txagc_event_template_t *txagc_template,
  rflm_txagc_fixup_tbl_t tbl_type
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_commit_fixup
(
  uint32 *des_ptr,
  const rflm_txagc_fixup_data_t *fixup_ptr,
  const rflm_txagc_fixup_cfg_t *txagc_fixup_cfg_ptr,
  boolean is_resend_issue_seq
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_create_fixup_tbl
(
  uint16 *dma_seq_ptr,
  rflm_txagc_event_t *txagc_seq_ptr,
  rflm_txagc_fixup_data_t *fixup_ptr,
  rflm_txagc_fixup_tbl_t fixup_tbl_t
);

/*---------------------------------------------------------------------------*/
rflm_ccs_rf_seq_err_e
rflm_txagc_create_generic_dma_seq
(
  uint32 handle_id,
  const rflm_ccs_rf_task_t *tasks, 
  uint32 num_tasks,
  uint32 * const generic_dma_seq  
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_fixup_addr_for_dma_seq
(
  uint8  *dma_seq,
  uint16 *fixup_tbl,
  uint32 handle_id,
  uint32 txagc_event_id,
  uint8  seq_t  
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_conv_event_to_issue_seq
(
  rflm_txagc_event_infor_type *event_task_info,
  rflm_ccs_rf_task_t  *issue_seq_task_buff
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_cmn_ccs_fixup_modem_override
(
  rflm_txagc_fixup_cfg_t *txagc_fixup_cfg_ptr,
  uint32 *des_ptr,
  rflm_drif_out_cfg_t* drif_out_cfg
);

/*---------------------------------------------------------------------------*/
boolean
rflm_txagc_cmn_ccs_fixup
(
  rflm_txagc_event_template_t *txagc_template,
  rflm_txagc_fixup_cfg_t *txagc_fixup_cfg_ptr,
  uint32 *des_ptr
);

/*---------------------------------------------------------------------------*/
boolean
rflm_txagc_cmn_resend_ccs_fixup
(
  rflm_txagc_event_template_t *txagc_template,
  rflm_txagc_fixup_cfg_t *txagc_fixup_cfg_ptr
);

/*---------------------------------------------------------------------------*/
uint32
rflm_txagc_cal_backoff_from_dbm10
(
  uint32 default_backoff_log10, // in log10(x) unit
  uint32 target_backoff_dbm10  // in dBm10
);

/*---------------------------------------------------------------------------*/
void 
rflm_txagc_callback
(
  struct rflm_ccs_rf_tq_handle_s *tq, 
  uint32 task_idx, 
  ccs_rf_task_status_t status, 
  void *arg
);

/*---------------------------------------------------------------------------*/
uint32
rflm_txagc_compute_iq_gain_adj_val
(
  uint32 nominal_iq_gain_val,
  int16 dBx10_backoff
);

/*---------------------------------------------------------------------------*/
uint32
rflm_txagc_get_tx_chain_idx
(
  uint32 h_id
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_get_lmem_addr
(
  uint32 h_id,
  uint32 *group_a_addr,
  uint32 *group_b_addr
);

/*---------------------------------------------------------------------------*/
uint32*
rflm_txagc_get_dma_seq_ptr
(
   uint8 seq_id,
   uint8 tech_id,
   rflm_txagc_static_t* txagc_static
);

/*---------------------------------------------------------------------------*/
void 
rflm_txagc_fbrx_wtr_data_prep 
(
  int32 tx_pwr_dbm10,                                 
  rflm_txagc_fixup_cfg_t *txagc_fixup_cfg_ptr,  
  uint8 *fbrx_gain_state
);

/*---------------------------------------------------------------------------*/
rflm_txagc_ccs_delta_table_t*
rflm_txagc_get_resend_tcm_ptr
(
  uint32 handle_id
);

/*---------------------------------------------------------------------------*/
int16
rflm_txagc_cmn_calc_iref_timing
(
  rflm_txagc_event_template_t *txagc_template,
  rflm_txagc_script_input_t *script_setting,
  uint32 ccs_action_time,
  rflm_txagc_iref_timing_t *iref_timing
);

#ifdef __cplusplus
}
#endif

/*!   @} */

#endif
