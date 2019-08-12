/*!
  @file
  rflm_txagc_lookup_opt.h

  @brief
  Header file for rflm_txagc_lookup_opt functions

  @detail
  description of functions in this file
 
*/

/*==============================================================================

  Copyright (c) 2013-2014 by Qualcomm Technologies, Inc. (QTI). All Rights Reserved.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/txagc/inc/rflm_txagc_lookup_opt.h#1 $

when         who     what, where, why
----------   ---     -----------------------------------------------------------
04/03/14     ljl     TxAGC lookup search with non-monotonic smps bias rgi2pwr tbl
03/23/14     hdz     Added static input to interface
10/31/13     llin    modify api for TxAGC lookup APT mode integration
09/30/13     llin    update and cleanup
09/29/13     llin    Initial creation
==============================================================================*/

#ifndef RFLM_TXAGC_LOOKUP_OPT_H
#define RFLM_TXAGC_LOOKUP_OPT_H


/*==============================================================================

  Include Files

==============================================================================*/
#include "rflm_txagc_api.h"
#include "rflm_txagc.h"


/*==============================================================================

  Preprocessor Definitions and Constants

==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#define RFLM_TXAGC_RGI2PWR_SIZE     64
#define RFLM_TXAGC_PWR2RGI_SIZE    180
#define RFLM_TXAGC_PWR_DELTA         5
#define RFLM_TXAGC_PWR_MAP_FINE_SEARCH_STEP 4


/*==============================================================================

  Type Declarations

==============================================================================*/
/*------------------------------------------------------------------------------
RF FED txagc data types /function prototypes used in Tech FED
------------------------------------------------------------------------------*/
typedef struct
{
  // rgi2pwr tables for bias case
  int16 pwr[RFLM_TXAGC_RGI2PWR_SIZE];
}rflm_txagc_rgi2pwr_t;

typedef union {
  uint32 w;
  struct {
	int16  txpwr_dbm10_smpsbias_with_pout;
	uint16 index_to_smps_bias_tbl;
  };
}rflm_txagc_smpsbias_sort_t;

typedef union
{
  uint32 rgi_w[RFLM_TXAGC_PWR2RGI_SIZE/4];
  uint8  rgi[RFLM_TXAGC_PWR2RGI_SIZE];
}rflm_txagc_pwr2rgi_t;

typedef struct {
  rflm_txagc_smpsbias_sort_t w[RFLM_TXAGC_TX_LIN_EPT_SIZE];
}rflm_txagc_lin_sort_dynamic_improve_t;

typedef struct
{
    rflm_txagc_pwr2rgi_t pwr2rgi_const[RFLM_TXAGC_NUM_PA_SP];
    rflm_txagc_pwr2rgi_t pwr2rgi_smps[RFLM_TXAGC_NUM_PA_SP];
}rflm_txagc_pwr2rgi_lut_t;

typedef struct ALIGN(8)
{
  /*! @brief smps biased power table */
  int16 txpwr_dbm10_smpsbias[RFLM_TXAGC_TX_LIN_APT_SIZE];
}rflm_txagc_lookup_tcm_lut_per_pa_t;

typedef struct ALIGN(8)
{
  /*! @brief smps biased power table */
  rflm_txagc_lookup_tcm_lut_per_pa_t dbm10_smpsbias_dynamics[RFLM_WCDMA_NUM_PA_STATE];
}rflm_txagc_lookup_tcm_lut_t;

/*==============================================================================

  Function Declarations and Documentation

==============================================================================*/
/*------------------------------------------------------------------------------
RF FED common TXAGC external APIs prototyes
------------------------------------------------------------------------------*/
void
rflm_txagc_rgi2pwr_to_pwr2rgi
(
  rflm_txagc_xpt_mode_t txagc_mode,
  rflm_txagc_pa_state_data_t pa_state,
  rflm_txagc_dynamic_t *txagc_dynamic,
  rflm_txagc_pwr2rgi_t *p_pwr2rgi_lut
);

void
rflm_txagc_rgi2pwr_to_pwr2rgi_full
(
  rflm_txagc_xpt_mode_t txagc_mode,
  rflm_txagc_pa_state_data_t pa_state,
  rflm_txagc_static_t *txagc_static,
  rflm_txagc_pwr2rgi_t *p_pwr2rgi_lut
);

void
rflm_txagc_lookup_apt_opt
(
  rflm_txagc_agc_input_t *txagc_input,
  rflm_txagc_script_input_t *txagc_output,
  rflm_txagc_dynamic_t *txagc_dynamic,
  rflm_txagc_static_t *txagc_static,
  rflm_txagc_pwr2rgi_lut_t *enh_feature_buf
);

uint8 rflm_txagc_binary_search_opt
(
  int16 pwr_dbm10[],
  int16 search,
  uint8 n
);

void
rflm_txagc_sort_smpsbias_with_pout
(
  rflm_txagc_xpt_mode_t txagc_mode,
  rflm_txagc_agc_input_t *txagc_input,
  rflm_txagc_static_t *txagc_static,
  rflm_txagc_dynamic_t *txagc_dynamic,
  rflm_txagc_lin_sort_dynamic_improve_t *txagc_lin_sort_dynamic
);

void
rflm_txagc_lookup_ept_opt
(
  rflm_txagc_agc_input_t *txagc_input,
  rflm_txagc_script_input_t *txagc_output,
  rflm_txagc_dynamic_t *txagc_dynamic,
  rflm_txagc_static_t *txagc_static,
  rflm_txagc_lin_sort_dynamic_improve_t *txagc_lin_sort_dynamic,
  rflm_txagc_pwr2rgi_t *p_pwr2rgi_lut_full
);

void
rflm_txagc_lookup_et_opt
(
  rflm_txagc_agc_input_t *txagc_input,
  rflm_txagc_script_input_t *txagc_output,
  rflm_txagc_dynamic_t *txagc_dynamic,
  rflm_txagc_static_t *txagc_static,
  rflm_txagc_lin_sort_dynamic_improve_t *txagc_lin_sort_dynamic,
  rflm_txagc_pwr2rgi_t *p_pwr2rgi_lut_full
);

uint8 rflm_txagc_search_vector_not_monotonic_apt_opt
(
  int16    pwr_dbm10[],
  int16    value,
  uint8    max_rgi_idx
);

void
rflm_txagc_lookup_apt_opt1
(
  rflm_txagc_agc_input_t *txagc_input,
  rflm_txagc_script_input_t *txagc_output,
  rflm_txagc_dynamic_t *txagc_dynamic,
  rflm_txagc_static_t *txagc_static,
  rflm_txagc_pwr2rgi_lut_t *enh_feature_buf,
  int16 *tx_lin_smps
);

uint8 rflm_txagc_search_vector_not_monotonic_ept_opt
(
  int16    pwr_dbm10[],
  int16    pout_adj[],
  int16    value,
  uint8    max_rgi_idx
);

void
rflm_txagc_lookup_ept_opt1
(
  rflm_txagc_agc_input_t *txagc_input,
  rflm_txagc_script_input_t *txagc_output,
  rflm_txagc_dynamic_t *txagc_dynamic,
  rflm_txagc_static_t *txagc_static,
  rflm_txagc_pwr2rgi_t *p_pwr2rgi_lut_full,
  int16 *tx_lin_smps
);

uint8 rflm_txagc_search_vector_not_monotonic_et_opt
(
  int16    pwr_dbm10[],
  int16    pout_adj[],
  int16    value,
  uint8    max_rgi_idx
);

void
rflm_txagc_lookup_et_opt1
(
  rflm_txagc_agc_input_t *txagc_input,
  rflm_txagc_script_input_t *txagc_output,
  rflm_txagc_dynamic_t *txagc_dynamic,
  rflm_txagc_static_t *txagc_static,
  rflm_txagc_pwr2rgi_t *p_pwr2rgi_lut_full,
  int16 *tx_lin_smps
);

void
rflm_txagc_lookup_apt_ref
(
  rflm_txagc_agc_input_t *txagc_input,
  rflm_txagc_script_input_t *txagc_output,
  rflm_txagc_dynamic_t *txagc_dynamic,
  rflm_txagc_static_t *txagc_static
);

void
rflm_txagc_lookup_ept_et_opt
(
  rflm_txagc_agc_input_t *txagc_input,
  rflm_txagc_script_input_t *txagc_output,
  rflm_txagc_dynamic_t *txagc_dynamic,
  rflm_txagc_static_t *txagc_static,
  rflm_txagc_pwr2rgi_t *p_pwr2rgi_lut_full,
  int16 *tx_lin_smps
);
#ifdef __cplusplus
}
#endif


#endif /* TEMPLATE_H */

