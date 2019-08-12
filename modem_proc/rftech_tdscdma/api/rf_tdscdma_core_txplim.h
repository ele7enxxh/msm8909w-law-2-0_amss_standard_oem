#ifndef RF_TDSCDMA_CORE_TXPLIM_H
#define RF_TDSCDMA_CORE_TXPLIM_H

/*!
  @file
  rf_tdscdma_core_txplim.h

  @brief
  Provides TXPLIM control.

  @details


*/

/*===========================================================================

Copyright (c) 2011, 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/rf_tdscdma_core_txplim.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/12/14   jyu     MDSP/SMEM cleanup
08/05/14   jhe     Separate mtpl for coex 
03/25/13   jyu     Exposed one API for HDET debug 
02/22/13   jyu     Merged from NikeL 
05/27/11   jhe     Initial version

============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "rfcom.h"
#include "rf_tdscdma_mdsp.h"


typedef enum
{
  RF_TDSCDMA_TXPLIM_MODE_ACQ,
  RF_TDSCDMA_TXPLIM_MODE_TRK,
  RF_TDSCDMA_TXPLIM_MODE_INIT,
  RF_TDSCDMA_TXPLIM_MODE_DISABLED,
  RF_TDSCDMA_TXPLIM_MODE_INVALID,
} rf_tdscdma_core_txplim_mode_type;


/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txplim_init(void);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txplim_enable(boolean enable);

/*--------------------------------------------------------------------------*/
int16 rf_tdscdma_core_txplim_get_hdet(void);

/*--------------------------------------------------------------------------*/
uint16 rf_tdscdma_core_txplim_get_hdet_adc(void);

/*--------------------------------------------------------------------------*/
int16 rf_tdscdma_core_txplim_get_hdet_threshold(void);

/*--------------------------------------------------------------------------*/
int16 rf_tdscdma_core_txplim_get_mtpl_nv_item(rfcom_txplim_channel_type chan_type);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txplim_do_mtpl(uint16 hdet_adc, int16 mdsp_txagc);

/*--------------------------------------------------------------------------*/
uint16 rf_tdscdma_core_txplim_convert_hdet_adc_to_txagc(uint16 hdet_adc);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txplim_set_mtpl
(
  int16 limit_in_dbm,
  rfcom_txplim_channel_type chan_type
);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txplim_update_lim_vs_freq
(
  //rf_path_enum_type path,
  rfcom_tdscdma_band_type band,
  uint16 channel,
  uint8 buf_idx
);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txplim_adjust_mtpl_initial(void);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txplim_adjust_mtpl_bho(void);

/*--------------------------------------------------------------------------*/
int16 rf_tdscdma_core_txplim_get_mtpl_backoff(void);

#ifdef FEATURE_RF_TDSCDMA_VBATT_BASED_BACKOFF
/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txplim_do_vbatt_based_backoff(void);
#endif

/*--------------------------------------------------------------------------*/
int16 rf_tdscdma_core_txplim_get_mtpl(void);

/*--------------------------------------------------------------------------*/
int16 rf_tdscdma_core_txplim_get_desired_limit(void);

/*--------------------------------------------------------------------------*/
int16 rf_tdscdma_core_txplim_get_desired_limit_coex(void);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txplim_temp_val_update
(
  int16 tx_lim_vs_temp_val, 
  int16 utran_tx_lim_vs_temp_offset_val,
  uint8 temp_index,
  uint8 temp_rem
);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txplim_reset_to_acq_mode(void);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txplim_reduce_mptl_db_for_therm(uint8 backoff);


/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txplim_interval_update(int16 interval);


/*--------------------------------------------------------------------------*/
uint16 rf_tdscdma_core_txplim_get_hdet_interval(void);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txplim_set_coex_limit(int16 sar_tx_pwr_limit, 
                                           int16 coex_tx_pwr_limit);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_txplim_reset_coex_limit(void);

#endif /* RF_TDSCDMA_CORE_TXPLIM_H */
