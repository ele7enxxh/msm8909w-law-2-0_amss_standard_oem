#ifndef RFLM_CDMA_FW_INTF_H
#define RFLM_CDMA_FW_INTF_H

/*!
   @file
   rflm_cdma_fw_intf.h

   @brief
   File to expose MSM specific APIs to RFLM layer
 
   @details

*/

/*==============================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies, Inc. (QTI). All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/api/cdma/rflm_cdma_fw_intf.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/19/15   vr      auto-pin check-in
12/02/15   vr      made seperate buffers for Therm/Hdet events
12/17/14   vr      moved the TX AGC start time 
11/20/14   sbo     Increase the size of static tune script ccs memory
                   to hold Prx tune script.
07/28/14   dr      Fix wrong return type in get WB reading API
05/27/14   JJ      Added two CCS script buffer for LDO on/off event
05/21/14   spa     Move CCS memory #defs to HAl layer (memory is modem specific)
04/11/14   spa     Register write support for WBPwr acuumulation
01/15/14   spa     Added API to freeze/unfreeze WBDC loop 
11/06/13   spa     Initial version

=============================================================================*/

#include "comdef.h" 
#include "lm_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! CDMA CCS script buffer sizes 
    Note: the sizes are in WORDs
*/
/*Increase the size of static tune script ccs memory to hold Prx tune script*/
/*The current profiling shows at least 220 words is needed to hold Prx tune script.*/
/*Allocate to 300 to leave additional margin.*/

#define RFLM_CDMA_TUNE_STATIC_BUF_SIZE                                       300
#define RFLM_CDMA_TX_DTX_RF_ON_BUF_SIZE                                       16
#define RFLM_CDMA_HDET_BUF_SIZE                                               12
/* Double LNA update buffer size, use either the first half buffer or the second 
half as ccs mem addr in adjacent lna update events, to avoid ccs mem overwrite */
#define RFLM_CDMA_RX_LNA_BUF_SIZE                                             64
#define RFLM_CDMA_DLNA_RX_ON_BUF_SIZE                                         28
#define RFLM_CDMA_DLNA_RX_OFF_BUF_SIZE                                        28
#define RFLM_CDMA_TXAGC_BUF_SIZE                                             320
#define RFLM_CDMA_AOL_CL_BUF_SIZE      RFLM_CMN_TUNER_SCRIPT_DATA_BUF_SIZE_WORDS 
#define RFLM_CDMA_ASDIV_BUF_SIZE                                              50
#define RFLM_CDMA_ENABLE_FBRX_BUF_SIZE                                         2
#define RFLM_CDMA_DISABLE_FBRX_BUF_SIZE                                        2
#define RFLM_CDMA_ENABLE_HKADC_BUF_SIZE                                       100
#define RFLM_CDMA_TOTAL_BUF_SIZE     (RFLM_CDMA_TUNE_STATIC_BUF_SIZE+ \
                                      RFLM_CDMA_TX_DTX_RF_ON_BUF_SIZE + \
                                      RFLM_CDMA_HDET_BUF_SIZE + \
                                      RFLM_CDMA_RX_LNA_BUF_SIZE + \
                                      RFLM_CDMA_DLNA_RX_ON_BUF_SIZE + \
                                      RFLM_CDMA_DLNA_RX_OFF_BUF_SIZE + \
                                      RFLM_CDMA_TXAGC_BUF_SIZE +\
                                      RFLM_CDMA_AOL_CL_BUF_SIZE + \
                                      RFLM_CDMA_ASDIV_BUF_SIZE + \
                                      RFLM_CDMA_ENABLE_FBRX_BUF_SIZE +\
                                      RFLM_CDMA_DISABLE_FBRX_BUF_SIZE +\
                                      RFLM_CDMA_ENABLE_HKADC_BUF_SIZE)

/* Those two variables are half of RFLM_CDMA_RX_LNA_BUF_SIZE and 
RFLM_CDMA_TXAGC_BUF_SIZE, they are used as offset when querying ccs memory 
for lna update and txagc update scripts. */
#define RFLM_CDMA_SINGLE_RX_LNA_BUF_SIZE             RFLM_CDMA_RX_LNA_BUF_SIZE/2
#define RFLM_CDMA_SINGLE_TXAGC_BUF_SIZE               RFLM_CDMA_TXAGC_BUF_SIZE/2

/*
  The TX_on/off event needs at most 8 words:
  TX_ON has the following:
    (a) WTR on -> 1 write and 1 trigger
    (b) PA on ->  about 3 writes and 1 trigger
  Assuming all writes are RFFE extended (worst case), it would need:
      2 words for WTR (one word for 1 RFFE ext) and 
      4 words for PA

  ** TX_ON would need 6 words **

  TX_OFF has the following:
    (a) WTR off -> 1 write  
    (b) PA off ->  1 write

  ** TX_ON would need 2 words **
  
*/
#define RFLM_CDMA_SINGLE_TX_ON_OFF_BUF_SIZE    RFLM_CDMA_TX_DTX_RF_ON_BUF_SIZE/2

/*! @brief Time taken for Tx stop script in microseconds */
#define RFLM_CDMA_TX_STOP_ESTIMATE_TIME_US                                    50

/*! @brief Time taken for Tx start script in microseconds */
#define RFLM_CDMA_TX_START_ESTIMATE_TIME_US                                  750

/*! make sure that total allocation  for one taskQ pair is less than what is 
allocated, the #def for this is present in rflm_ccs_task_defs.h, defined as 
CCS_RF_TASKQ_PAIRS_DATA_BUF_NUM_WORDS */ 
COMPILE_ASSERT( RFLM_CDMA_TOTAL_BUF_SIZE <= 
                                         CCS_RF_TASKQ_PAIRS_DATA_BUF_NUM_WORDS);
/*----------------------------------------------------------------------------*/
boolean
rf_cdma_msm_get_wb_nb_indices
( 
  lm_handle_type rxlm_handle,
  uint8 *wb_idx,
  uint8 *nb_indices 
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_msm_wbdc_accum_freeze_unfreeze
( 
  uint8 wb_idx,
  boolean start_imm 
);

/*----------------------------------------------------------------------------*/
void 
rf_cdma_msm_config_wb_power_estimator
(
  uint32 wb_idx,
  boolean always_armed,
  boolean start,
  boolean immediate,
  uint32 start_action_sample,
  uint32 stop_action_sample
);

/*----------------------------------------------------------------------------*/
uint32 
rf_cdma_msm_get_wb_accum_reading
(
  uint32 wb_idx
);

/*----------------------------------------------------------------------------*/
void 
rf_cdma_msm_clear_wb_accum
(
  uint32 wb_idx
);

/*----------------------------------------------------------------------------*/
void 
rf_cdma_msm_clear_wb_accum
(
  uint32 wb_idx
);

/*----------------------------------------------------------------------------*/
void 
rf_cdma_msm_config_wb_pwr_estimate_mask
(
  uint32 wb_idx
);

#ifdef __cplusplus
}
#endif


#endif /* RFLM_CDMA_FW_INTF_H */


