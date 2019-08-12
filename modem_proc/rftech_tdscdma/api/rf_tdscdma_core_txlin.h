#ifndef RF_TDSCDMA_CORE_TXLIN_H
#define RF_TDSCDMA_CORE_TXLIN_H
/*!
   @file
   rf_tdscdma_core_txlin.h

   @brief
   Provides TDSCDMA TX Linearizer Functionality

   @details
    

*/

/*===========================================================================

Copyright (c) 2011 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/rf_tdscdma_core_txlin.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/29/14   ych     Updated for 8 PA gain states
12/15/13   jps     Fixes for RFLM based TXAGC override
11/19/13   jps     Support for RFLM-based Tx AGC override (TD-SCDMA) 
10/30/13   jps     Updates and fixes for common TXAGC hookup
02/22/13   jyu     Merged from NikeL
12/04/12   ms      Remove unwanted APT functions
06/13/12   jhe     Multi-lin NV update 
05/27/11   jhe     Initial version

============================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "rfcom.h"
#include "nv_items.h"
#include "rfnv_tdscdma.h"
#include "rf_tdscdma_mdsp.h"
#include "rfcommon_core_txlin.h"


typedef struct
{
  //int8 tx_comp_vs_freq_val[RF_TDSCDMA_NUM_TX_LINEARIZERS];
  //int8 tx_secondary_comp_vs_freq_val[RF_TDSCDMA_NUM_TX_LINEARIZERS];
  int16 tx_lin_vs_temp_val[RFCOM_TXLIN_NUM_LINEARIZERS];
  int16 tx_lin_comp[RFCOM_TXLIN_NUM_LINEARIZERS];
  int16 pin_vs_temp_vs_freq[RFCOM_TXLIN_NUM_LINEARIZERS];
  int16 pout_vs_temp_vs_freq[RFCOM_TXLIN_NUM_LINEARIZERS];
}rf_tdscdma_core_txlin_data_type;

typedef struct
{
  tx_band_cal_type* txlin_data;
  rfnv_item_id_enum_type txlin_nv;
}rf_tdscdma_core_tx_multi_lin_data_type;

typedef struct
{
  /* Linearizer table shadow for the current Tx frequency */
  tx_linearizer_table_type current_lin_tbl[RFCOM_TXLIN_NUM_LINEARIZERS];
  /* Current linearizer table after temp compensation */
  tx_linearizer_table_type current_lin_tbl_comp[RFCOM_TXLIN_NUM_LINEARIZERS];
  /* XPT table shadow for the current Tx frequency -- TODO store of xpt doesn't seem to be necessary */
  //xpt_data_type current_xpt_tbl[RFCOM_TXLIN_NUM_LINEARIZERS];
  /* EPT first sweep linearizer table shadow for the current Tx frequency */
  //tx_linearizer_table_type first_sweep_lin_tbl;
  /* Multi Linearizer revision*/
  tx_multi_lin_rev_type multi_lin_rev;
  /* Number of valid PA states */
  uint8 num_valid_pa_state;
  /* Aux data flag */
  tx_linearizer_aux_data_type current_aux_data[RFCOM_TXLIN_NUM_LINEARIZERS];
  /* EPT PA state */
  uint8 ept_pa_state;
  /* Linearizer table size */
  uint8 txlin_valid_entries[RFCOM_TXLIN_NUM_LINEARIZERS];
  /* Current Temp Comp Algorithm */
  rfcom_txlin_temp_comp_algo_type temp_comp_algo;
  /* Current Pin Comp value */
  int16 txlin_pin_comp[RFCOM_TXLIN_NUM_LINEARIZERS];
  /* Current Pout Comp value */
  int16 txlin_pout_comp[RFCOM_TXLIN_NUM_LINEARIZERS];
}rf_tdscdma_core_txlin_record_type;



/*===========================================================================

                     FUNCTION DECLARATIONS
===========================================================================*/

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txlin_calc_total_txlin_comp(void);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txlin_init(void);

/*--------------------------------------------------------------------------*/
tx_band_cal_type *rf_tdscdma_core_txlin_get_band_data(rfcom_tdscdma_band_type band);

/*--------------------------------------------------------------------------*/
boolean rf_tdscdma_core_txlin_update( rfcom_tdscdma_band_type band,
                                      uint16 rf_chan_tx,
                                      uint8 buf_idx,
                                      lm_handle_type txlm_buffer_idx );

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txlin_comp_update(uint16 tx_chan,
                                       boolean init_flag,
                                       uint8 buf_idx);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txlin_temp_val_update(int16 tx_lin_vs_temp_val[],
                                           int16 pin_offset[],
                                           int16 pout_offset[]
                                           );

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txlin_calc_freq_comp(rfcom_tdscdma_band_type band, uint16 channel);

/*--------------------------------------------------------------------------*/
rf_tdscdma_core_txlin_record_type* rf_tdscdma_core_get_current_txlin_data(void);

/*--------------------------------------------------------------------------*/
boolean rf_tdscdma_core_txlin_get_tx_device_info(rfm_device_enum_type device,
                                            rfcom_tdscdma_band_type band,
                                            rfcommon_core_txlin_agc_device_info *device_info);

/*--------------------------------------------------------------------------*/
boolean rf_tdscdma_core_txlin_config_txagc(rfm_device_enum_type tx_device,
                                           rfcom_tdscdma_band_type band,
                                           uint32 txlm_buffer_index
                                           );

/*--------------------------------------------------------------------------*/
boolean rf_tdscdma_core_txlin_temp_comp_update(int16 pin_comp_val[],
                                               int16 pout_comp_val[]);

/*--------------------------------------------------------------------------*/
boolean rf_tdscdma_core_txlin_autopin_update
(
  int16 pin_comp_val,
  uint8 pa_index
);

#endif /* RF_TDSCDMA_CORE_TXLIN_H */
