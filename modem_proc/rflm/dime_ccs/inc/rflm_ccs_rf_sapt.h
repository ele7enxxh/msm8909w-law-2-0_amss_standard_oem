#ifndef RFLM_CCS_RF_SAPT_H
#define RFLM_CCS_RF_SAPT_H

/*!
  @file rflm_ccs_sapt.h

  @brief
  This file contains SAPT helper function.
*/

/*============================================================================== 
   
  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved 
   
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
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dime_ccs/inc/rflm_ccs_rf_sapt.h#1 $ 
   
when       who     what, where, why 
--------   ---     ------------------------------------------------------------- 
07/31/13   ra     Initial version
==============================================================================*/
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#define FEATURE_RF_BOLT_PRE_SILICON
#ifndef FEATURE_RF_BOLT_PRE_SILICON

#include "rflm.h"
#include "rflm_ccs_rf_intf.h"
#include "fw_crm.h"
#include "fw_memss_mempool.h"
#include "fw_txlm.h"
#include "fw_rxlm.h"
#include "fw_mcdma.h"

/*===========================================================================

                        Macro and Structure Definition
 
===========================================================================*/
#ifndef FEATURE_RF_BOLT_PRE_SILICON
/*! @brief MCDMA per task structre*/
typedef struct
{
  /*! @brief DMA config for the task */
  fw_mcdma_dma_ch_cfg_t dma_cfg;
  /*! @brief VBUF used for the task */
  fw_mcdma_vbuf_cfg_t vbuf;
  /*! @brief MCDMA channel ID */
  uint16 mcdma_ch_id;
} rflm_ccs_mcdma_task_t;
#endif
/*===========================================================================*/

/*===========================================================================

                                FUNCTIONS

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*! @brief Single task */
extern rflm_ccs_mcdma_task_t rflm_ccs_mcdma_single_xfer;

void rflm_ccs_sapt_capture_init
(
  /*! RF tech ID*/
  fw_crm_rat_e tech,
  /*! Enabled RxFE chain info */
  fw_rxlm_enabled_chain_s rx_enabled_chain,
  /*! TxC chain Id */
  uint32 txc_chain_id,
  /*! RxFE Bridge Configuration regarding to mempool */
  fw_memss_mempool_bridge_cfg_t *rxfe_brdg_mem_cfg,
  /*! Tx Brdige configuratin regarding to mempool */
  fw_memss_mempool_bridge_cfg_t *tx_brdg_mem_cfg
);

void rflm_ccs_sapt_capture_deinit
(
  /*! RF tech ID*/
  fw_crm_rat_e tech,
  /*! Enabled RxFE chain info */
  fw_rxlm_enabled_chain_s rx_enabled_chain,
  /*! TXC chain ID*/
  uint32 txc_chain_id
);

void rflm_ccs_sapt_dynamic_cfg
(
  /*! RF tech ID*/
  fw_crm_rat_e tech,
  /*! rxlm buf ID used for sample capture*/
  uint32 rxlm_buf_id,
  /*! TxC chain Id (0 or 1) for sample capture*/
  uint32 txc_chain_id
);

void rflm_ccs_sapt_capture_ext_mem
(
  /*! RF tech ID*/
  fw_crm_rat_e tech,
  /*! start time in units/precision of the tech Rx stmr.The stmr value must 
    match the tx frame boundary*/
  uint32 start_rx_time,
  /*! Start time in units of TXC_SAMPLE_COUNT.*/
  uint32 start_tx_time,
  /*! Enabled RxFE chain info */
  fw_rxlm_enabled_chain_s rx_enabled_chain,
  /*! TxC chain Id (0 or 1) for sample capture*/
  uint32 txc_chain_id,
  /*! capture length(in samples)of TxC reference and RX signals.*/
  uint32 capture_len,
  /*! receive sample buffer */
  uint32 * rx_samp_ptr,
  /*! transmit sample buffer */
  uint32 * tx_samp_ptr,
  /*! RxFE Bridge Configuration regarding to mempool */
  fw_memss_mempool_bridge_cfg_t *rxfe_brdg_mem_cfg,
  /*! Tx Brdige configuratin regarding to mempool */
  fw_memss_mempool_bridge_cfg_t *tx_brdg_mem_cfg,
  /*! Callback that gets called when copy is done in LP thread */
  void (*callback_fn)( void)
);

void rflm_ccs_sapt_capture
(
  /*! RF tech ID*/
  fw_crm_rat_e tech,
  /*! start time in units/precision of the tech Rx stmr.The stmr value must 
    match the tx frame boundary*/
  uint32 start_rx_time,
  /*! Start time in units of TXC_SAMPLE_COUNT.*/
  uint32 start_tx_time,
  /*! Enabled RxFE chain info */
  fw_rxlm_enabled_chain_s rx_enabled_chain,
  /*! TxC chain Id (0 or 1) for sample capture*/
  uint32 txc_chain_id,
  /*! capture length(in samples)of TxC reference and RX signals.*/
  uint32 capture_len,
  /*! RxFE Bridge Configuration regarding to mempool */
  fw_memss_mempool_bridge_cfg_t *rxfe_brdg_mem_cfg,
  /*! Tx Brdige configuratin regarding to mempool */
  fw_memss_mempool_bridge_cfg_t *tx_brdg_mem_cfg
);
void rflm_ccs_sapt_capture_background_task
(
  /*! RF tech ID*/
  fw_crm_rat_e tech,
  /*! start time in universal stmr.The stmr value must match the tx frame boundary*/
  uint32 start_rx_time,
  /*! Start time in units of TXC_SAMPLE_COUNT.*/
  uint32 start_tx_time,
  /*! Enabled RxFE chain info */
  fw_rxlm_enabled_chain_s rx_enabled_chain,
  /*! TxC chain Id (0 or 1) for sample capture*/
  uint32 txc_chain_id,
  /*! capture length(in samples)of TxC reference and RX signals.*/
  uint32 capture_len,
  /*! RxFE Bridge Configuration */
  fw_memss_mempool_bridge_cfg_t *rxfe_brdg_mem_cfg,
  /*! Tx Brdige configuratin regarding to mempool */
  fw_memss_mempool_bridge_cfg_t *tx_brdg_mem_cfg
);

void rflm_ccs_sapt_capture_main
(
  /*! RF tech ID*/
  fw_crm_rat_e tech,
  /*! start time in universal stmr.The stmr value must match the tx frame boundary*/
  uint32 start_rx_time,
  /*! Start time in units of TXC_SAMPLE_COUNT.*/
  uint32 start_tx_time,
  /*! Enabled RxFE chain info */
  fw_rxlm_enabled_chain_s rx_enabled_chain,
  /*! TxC chain Id (0 or 1) for sample capture*/
  uint32 txc_chain_id,
  /*! capture length(in samples)of TxC reference and RX signals.*/
  uint32 capture_len,
  /*! RxFE Bridge Configuration */
  fw_memss_mempool_bridge_cfg_t *rxfe_brdg_mem_cfg,
  /*! Tx Brdige configuratin regarding to mempool */
  fw_memss_mempool_bridge_cfg_t *tx_brdg_mem_cfg,
  /*! Stucture indicating what buffer to use for capture */
  rflm_ccs_xpt_cal_mode_params_t *cal_p,
  /*! Callback that gets called when copy is done in LP thread */
  void (*callback_fn)( void),
  /*! Boolean to indicate if copy is in done in background (either low priority 
    thread or MCDMA), set to TRUE for background copy */
  boolean bkgd_xfer

);

void rflm_ccs_sapt_capture_start (
  /*! RF tech ID*/
  fw_crm_rat_e tech,
  /*! start time in universal stmr.The stmr value must match the tx frame boundary*/
  uint32 start_rx_time,
  /*! Start time in units of TXC_SAMPLE_COUNT.*/
  uint32 start_tx_time,
  /*! Enabled RxFE chain info */
  fw_rxlm_enabled_chain_s rx_enabled_chain,
  /*! TxC chain Id (0 or 1) for sample capture*/
  uint32 txc_chain_id,
  /*! capture length(in samples)of TxC reference and RX signals.*/
  uint32 capture_len
  );

void rflm_ccs_sapt_capture_wait(
  /*! RF tech ID*/
  fw_crm_rat_e tech,
  /*! start time in universal stmr.The stmr value must match the tx frame boundary*/
  uint32 start_rx_time,
  /*! capture length(in samples)of TxC reference and RX signals.*/
  uint32 capture_len,
  /*! TxC chain Id (0 or 1) for sample capture*/
  uint32 txc_chain_id
);

void rflm_ccs_sapt_capture_tx_rx_samples(
  /*! Structure indicating what buffer to use for capture */
  rflm_ccs_xpt_cal_mode_params_t *cal_p,
  /*! capture length(in samples)of TxC reference and RX signals.*/
  uint32 capture_len,
  /*! RxFE Bridge Configuration */
  fw_memss_mempool_bridge_cfg_t *rxfe_brdg_mem_cfg,
  /*! Tx Brdige configuratin regarding to mempool */
  fw_memss_mempool_bridge_cfg_t *tx_brdg_mem_cfg,
  /*! Callback that gets called when copy is done in LP thread */
  void (*callback_fn)( void),
  /*! Boolean to indicate if copy is in done in background (either low priority 
    thread or MCDMA), set to TRUE for background copy */
  boolean lp_thread_copy);

void rflm_ccs_mcdma_mempool_data
(
  /*! capture length(in virtual samples) to be copied */
  uint32 capture_len,
  /*! RxFE Bridge Configuration */
  fw_memss_mempool_bridge_cfg_t *mem_brdg_cfg,
  /*! Buffer index in rf_common_log_buf[FW_RF_COMMON_NUM_CAPTURES] where 
    samples will be captured to */
  uint32 buf_idx,
  /*! Callback that gets called when copy is done */
  void (*callback_fn)( void)
  );
uint32 rflm_ccs_mcdma_mempool_data_at_offset
(
  /*! capture length(in virtual samples) to be copied */
  uint32 capture_len,
  /*! RxFE Bridge Configuration */
  fw_memss_mempool_bridge_cfg_t *mem_brdg_cfg,
  /*! Buffer index in rf_common_log_buf[FW_RF_COMMON_NUM_CAPTURES] where 
    samples will be captured to */
  uint32 buf_idx,
  /*! Offset within buffer where samples will go to */
  uint32 buf_offset,
  /*! Callback that gets called when copy is done */
  void (*callback_fn)( void)
  );

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_RF_BOLT_PRE_SILICON */

#endif /*RFLM_CCS_RF_SAPT_H*/
