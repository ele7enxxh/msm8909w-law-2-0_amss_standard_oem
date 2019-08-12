#ifndef RFWCDMA_CORE_TXPLIM_H
#define RFWCDMA_CORE_TXPLIM_H

/*!
  @file
  rfwcdma_core_txplim.h

  @brief
  Provides TXPLIM control.

  @details


*/

/*===========================================================================

Copyright (c) 2013 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_core_txplim.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/18/16   sk      Initialize the MTPL every Tx Config
07/01/15   zhh     reset the mtpl with the LUT revert value during temp comp
01/27/15   rmb     Add api to fetch MTPL in dBm10 units.
11/17/14   vbh     Update txplim_get_mtpl_nv_item to accept band as a param
08/15/14   zhh    populate the new MPR to DM in rfwcdma_core_get_tx_mpr_param()
05/15/14   aro     Added mtpl handler function
05/15/14   aro     Deprecated old command to perform MTPL update
05/17/13   kcj     Enable multi-TX SAR limit 
03/19/13   dw/kai  [1]Added support for HDET based linearizer compensation 
                   [2]Added support to limit allowable HDET mod range 
11/15/12   ka      Initial support for DCHSUPA
09/28/12   ac      device interface to pass band to rfc_wcdma_get_device
03/27/12   dw      Fix max power issue that NV limit was not initalized 
                   properly until temp comp kicks in
09/08/11   ac      SAR support
01/27/11   ka/dw   Merge support for thermal mitigation  
10/13/10   dw      add rfwcdma_core_txplim_get_mode()
10/04/10   ka      Added rfwcdma_core_txplim_reset_to_acq_mode() 
08/17/10   ka      Back out hdet change  
08/04/10   ac      Improve exp_hdet_vs_agc interpolation to reduce overall enable_tx execution time
01/12/10   xw      Fixed compiler warnings 
09/22/09   bn      RFTXPLIM 2.0 - New Tx Power limiting for UMTS targets
08/13/09   jhe     freq comp change
07/07/09   bn      TxAGC dB10 changes
05/28/09   ckl     Add missing function declaration
03/09/09   clk     Code cleanup
11/06/08   av     Initial version.

============================================================================*/

#include "comdef.h"
#include "rfcom.h"
#include "rfwcdma_mc.h"
#include "rfm_common_sar.h"
#include "rf_dispatch.h"

/* Number of entries in MPR based beta scaling table */
#define RFTXPLIM_MPR_BETA_TABLE_ENTRIES 7

typedef enum
{
  RFTXPLIM_MODE_ACQ,
  RFTXPLIM_MODE_TRK,
  RFTXPLIM_MODE_INIT,
  RFTXPLIM_MODE_DISABLED,
  RFTXPLIM_MODE_INVALID,
} rfwcdma_core_txplim_mode_type;


/*--------------------------------------------------------------------------*/
void rfwcdma_core_txplim_init(rfcom_wcdma_band_type band,uint16 channel,uint32 multi_carrier_idx);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_txplim_enable(boolean enable);

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_txplim_get_hdet(void);

/*--------------------------------------------------------------------------*/
uint16 rfwcdma_core_txplim_get_hdet_adc(void);

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_txplim_get_hdet_threshold(void);

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_txplim_get_mtpl_nv_item(rfcom_txplim_channel_type chan_type, rfcom_wcdma_band_type band);

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_txplim_get_mtpl(void);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_txplim_set_mtpl
(
  int16 limit_in_dbm,
  rfcom_txplim_channel_type chan_type
);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_txplim_update_lim_vs_freq
(
  rf_path_enum_type path,
  rfcom_wcdma_band_type band,
  uint16 channel
);

/*--------------------------------------------------------------------------*/
uint16 rfwcdma_core_txplim_read_hdet_being_used(void);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_txplim_get_mtpl_backoff(rf_max_tx_pwr_limit_type *rf_mtpl);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_txplim_do_vbatt_based_backoff(void);

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_txplim_get_mtpl(void);

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_txplim_get_desired_limit(void);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_txplim_temp_val_update
(
  int16 tx_lim_vs_temp_val, 
  int16 utran_tx_lim_vs_temp_offset_val,
  int16 hdet_vs_temp_val,
  int16 hdet_mod_base,
  int16 hdet_mod_range[2]
);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_txplim_reset_to_acq_mode(void);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_txplim_reduce_mptl_db_for_therm(uint8 backoff);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_txplim_set_sar_limit(rfm_common_sar_state_type sar_index);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_txplim_set_multi_tx_sar_limit
(
  rfcom_device_enum_type device,
  int16 sar_limit
);

/*--------------------------------------------------------------------------*/
void
rfwcdma_core_txplim_do_mtpl_handler
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*--------------------------------------------------------------------------*/
boolean
rfwcdma_plim_override_mode
(
  rfm_device_enum_type device,
  boolean plim_enable
);

/*----------------------------------------------------------------------------*/
void rfwcdma_core_txplim_load_mpr_backoff
(
  rfcom_wcdma_band_type band,
  uint32 multi_carrier_idx
);

/*----------------------------------------------------------------------------*/
int16 rfwcdma_core_txplim_get_desired_limit_dbm10(void);

void rfwcdma_core_txplim_reset_mtpl_during_temp_comp(void);

/*--------------------------------------------------------------------------*/
/*!
  @brief
  reset the mtpl as part of tx config in FTM Non-Cal mode

  @details
  reset the mtpl as part of tx config in FTM Non-Cal mode
 
  @param 
  none

  @return
  none
*/
void rfwcdma_core_txplim_reset_mtpl(void);

#endif /* RFWCDMA_CORE_TXPLIM_H */
