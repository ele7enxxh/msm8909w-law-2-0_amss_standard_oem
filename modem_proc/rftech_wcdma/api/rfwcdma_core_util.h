#ifndef RF_WCDMA_CORE_UTIL_H
#define RF_WCDMA_CORE_UTIL_H

/*!
  @file
  rfwcdma_core_util.h

  @brief
  RF WCDMA Core Utility Functions.

  @details

*/

/*===========================================================================

Copyright (c) 2008 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_core_util.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/26/14   zhh    Update XPT Delay structure to support DC-UPA
08/06/14   dw      Disable TxAGC in Tx init when Tx BW change
08/04/14   rmb     Add the core conversion APIs.
02/26/14   adk     Added function prototype rfwcdma_core_util_get_min_max_tx_freqs
06/06/14   ac      nbr support
05/01/14   rmb     Remove old AGC log interface 0x4176 version 1.
04/20/14   bn      WCDMA 8 PA state support and related cleanup
04/11/14   dw      Add Tx ON logging from RFLM
04/09/14   dw      Fix TxAGC logging resolution issue
03/25/14   rmb     Add a new get rxagc API during X2W measurements. 
03/11/14   dw      Fix function name of rfwcdma_core_count_carriers
02/04/14   kai     Add common API to read TxAGC status from RFLM current params
01/02/14   dw      Added macros for timing unit conversion
12/05/13   bn      Add IQ gain logging support
12/04/13   dw      Consolidate all get RxAGC core functions
10/28/13   dw      Add LNA state logging support
09/16/13   dw      Add IS_FTM_CAL_MODE() Macro
08/28/13   vs      Add API to return max carrier ID for given carrier configuration
08/23/13   vs      Deprecating unused API rfm_get_receive_agc_val_rscp
07/14/13   aro     Compiler Warning fix
07/11/13   aro     Compiler Warning fix
06/27/13   ac      Unified RF API to support query of RxAGC for SC and DC case
06/26/13   ac      tx freezeaccum api change for both car
04/05/13   kg      Added API to convert rfcom band type to a number 
02/05/13   kg      Added API's to return rfcom type band mask and max/min
                   freq based on band 
02/13/12   pl      Fix compiler warning
07/28/11   ckl     Remove rfwcdma_core_util_get_cgagc_settling_time()
05/16/11   ad      Added exportable APIs to access internal MDSP rxagc cmd status 
                   and hdet buffered tx gain value
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1  
08/16/10   ac      compiler warning fix
01/10/10   kguo    Merged mdm9k dev branch to main\latest 
10/01/09   ckl     Fixed rfwcdma_core_util_get_tx_agc_val( ) prototype
05/28/09   ckl     Add missing function declarations
03/09/09   ckl     Code cleanup
11/05/08   kc      Added comments in doxygen format.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfcom.h"
#include "rfwcdma_mdsp.h"
#include "rfwcdma_mc.h"
#include "rfnv_wcdma.h"
#include "rfcommon_core.h"
#include "rfm.h"
#include "rfwcdma_mdsp_types.h"
/*---------------------------------------------------------
                   Macro Definition 
---------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Macro to convert Chip x8 unit to USTMR unit (19.2MHz)
    ceil((time_chipx8 * 19.2) / (3.84 * 8)) 
*/
#define WCDMA_CONV_CX8_TO_USTMR(c)  (c * 5 + 7) / 8

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Macro to convert mirco-second unit to USTMR unit (19.2MHz)
    ceil((time_us * 19.2) 
*/
#define WCDMA_CONV_US_TO_USTMR(u)  (u * 96 + 4 )/5

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This Macro queries WCDMA FTM Cal mode
*/
#define IS_FTM_CAL_MODE() (( ftm_get_mode() == FTM_MODE )&&(rfm_get_calibration_state() == TRUE))

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This Macro queries if there is a bandwidth change based on previous and current multi-carrier index
    Initial Enablement from 0 to 1 is not a BW change
*/
#define IS_BW_CHANGE(prev_idx, new_idx) ((prev_idx!=new_idx)&&(prev_idx!=0)&&(new_idx!=0))

#define SIGN(x)              (((x) < 0) ? -1 : 1)
#define RF_FLOOR(x,y)           rfwcdma_core_util_math_floor(x, y)
#define RF_ROUND(x,y)           RF_FLOOR(((2 * (x)) + (y)), (2 * (y)))

/* WCDMA Band mask definitions*/
#define RF_WCDMA_B1_MASK_IDX  0
#define RF_WCDMA_B2_MASK_IDX  1
#define RF_WCDMA_B3_MASK_IDX  2
#define RF_WCDMA_B4_MASK_IDX  3
#define RF_WCDMA_B5_MASK_IDX  4
#define RF_WCDMA_B8_MASK_IDX  5
#define RF_WCDMA_B9_MASK_IDX  6
#define RF_WCDMA_B11_MASK_IDX 7
#define RF_WCDMA_B19_MASK_IDX 8

#define RF_WCDMA_B1_MASK   (uint64)(1<<(uint64)(RF_WCDMA_B1_MASK_IDX))
#define RF_WCDMA_B2_MASK   (uint64)(1<<(uint64)(RF_WCDMA_B2_MASK_IDX))
#define RF_WCDMA_B3_MASK   (uint64)(1<<(uint64)(RF_WCDMA_B3_MASK_IDX))
#define RF_WCDMA_B4_MASK   (uint64)(1<<(uint64)(RF_WCDMA_B4_MASK_IDX))
#define RF_WCDMA_B5_MASK   (uint64)(1<<(uint64)(RF_WCDMA_B5_MASK_IDX))
#define RF_WCDMA_B8_MASK   (uint64)(1<<(uint64)(RF_WCDMA_B8_MASK_IDX))
#define RF_WCDMA_B9_MASK   (uint64)(1<<(uint64)(RF_WCDMA_B9_MASK_IDX))
#define RF_WCDMA_B11_MASK  (uint64)(1<<(uint64)(RF_WCDMA_B11_MASK_IDX))
#define RF_WCDMA_B19_MASK  (uint64)(1<<(uint64)(RF_WCDMA_B19_MASK_IDX))

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to get TxAGC logging
 
*/
typedef struct
{
  uint16       rgi;
  uint16       row_idx;
  uint16       row_idx_smps;
  int16        iq_gain;
  uint8        pa_range;
  int16        smps_bias;
  uint16       icq_current;
  int16        env_scale;
  uint8        xpt_mode;
  uint16       dpd_index;
  int16        pin_comp;
  int16        pout_comp;
  int16        xpt_delay;
}rfwcdma_core_util_txagc_logging_type;

/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_util_is_rxagc_on
(
  rfagc_receive_chain_type rx_chain,
  rfcom_multi_carrier_id_type car_id
);
/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_util_is_rxagc_enabled
(
  rfagc_receive_chain_type rx_chain,
  rfcom_multi_carrier_id_type car_id
);
/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_util_get_rxagc
(
  rfagc_receive_chain_type rx_chain,
  rfcom_rxagc_type rxagc_type,
  rfcom_multi_carrier_id_type car_id,
  rfcom_rxagc_unit_type rxagc_unit,
  int16 *rxagc_val
);
/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_util_get_rxagc_xtow_meas
(
  rfagc_receive_chain_type rx_chain,
  rfcom_rxagc_type rxagc_type,
  rfcom_multi_carrier_id_type car_id,
  rfcom_rxagc_unit_type rxagc_unit,
  rfcom_wcdma_band_type band,
  boolean bypass_rxagcon_flag
);
/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_util_get_rf_capability( rfcom_rf_cap_type feature );

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_util_get_rflm_txagc_dBm( void );

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_util_get_rflm_txagc_dB10( void );

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_util_get_rflm_rgi( void );

/*--------------------------------------------------------------------------*/
uint8 rfwcdma_core_util_get_rflm_pa_range( void );

/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_util_get_rflm_tx_on( void );

/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_util_get_rflm_txagc_logging
(
  rfwcdma_core_util_txagc_logging_type* data
);

/*--------------------------------------------------------------------------*/
rfcom_multi_carrier_id_type rfwcdma_core_util_get_max_carr_id
(
  rfcom_multi_carrier_hspa_id_type multi_carrier_cfg
);
/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_util_get_mdsp_tx_power_limit( void );

/*--------------------------------------------------------------------------*/
uint16 rfwcdma_core_util_get_rf_warmup_time( void );

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_util_get_txagctablestore(rfcom_multi_carrier_id_type car_idx );

/*--------------------------------------------------------------------------*/
void rfwcdma_core_util_get_tx_rotator_angle
(
  rfwcdma_mc_rot_angle_type* tx_rot_angle
);

/*--------------------------------------------------------------------------*/
uint8 rfwcdma_core_util_get_pa_range_from_nv(uint8 pa_index);

/*----------------------------------------------------------------------------*/
uint16 rfwcdma_core_util_get_beta_scaling_comp(uint8 mpr_idx);

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_util_get_ue_max_tx_power
(
  rfcom_txplim_channel_type chan_type
);

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_util_get_min_tx_power_val
(
  rfcom_txplim_channel_type chan_type
);

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_util_get_ue_min_tx_power( void );

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_util_convert_tx_dbm_to_agc_unit( int16 dbmval );

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_util_convert_tx_agc_to_dbm_unit( int16 txagcval );

/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_util_convert_dbm10_to_txagc_unit( int16 dbm10val );

/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_util_is_sleeping( rf_path_enum_type rx_chain );

/*--------------------------------------------------------------------------*/
void rfwcdma_core_update_min_power_from_nv(void);

/*--------------------------------------------------------------------------*/
uint16 rfwcdma_core_get_enc_btf_val(void);

/*--------------------------------------------------------------------------*/
int32 rfwcdma_core_util_math_floor(int32 dividend,int32 divisor);

/*--------------------------------------------------------------------------*/
uint8 rfwcdma_core_util_get_pa_range_map_nv_index(uint8 pa_range);
/*--------------------------------------------------------------------------*/
int16 rfwcdma_core_util_get_hdet_tx_agc_value(void);
/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_util_get_rxagc_cmd_status(void);
/*--------------------------------------------------------------------------*/
rfnv_wcdma_nv_tbl_type *rfwcdma_core_init_nv_table(rfcom_wcdma_band_type band);
/* ----------------------------------------------------------------------- */
boolean rfwcdma_core_util_get_min_max_rx_freqs(rfcom_wcdma_band_type band, uint32* min, uint32* max);
/* ----------------------------------------------------------------------- */
boolean rfwcdma_core_util_get_min_max_tx_freqs(rfcom_wcdma_band_type band, uint32* min, uint32* max);
/*------------------------------------------------------------------*/
uint64 rfwcdma_core_util_get_band_mask(rfcom_wcdma_band_type band);

/*------------------------------------------------------------------*/
boolean rfwcdma_core_util_get_band_num(rfcom_wcdma_band_type band, uint16* num);

/*------------------------------------------------------------------*/
rfcom_multi_carrier_hspa_id_type rfwcdma_core_count_carriers(uint32 multi_carrier_index);

/*------------------------------------------------------------------*/
uint8 rfwcdma_core_util_get_lna_state(rfm_device_enum_type device);
/*------------------------------------------------------------------*/
uint32 rfwcdma_core_util_get_rot_freq_for_nbr
(
  rfm_device_enum_type device,
  rfcom_multi_carrier_id_type car_id
);
/* ----------------------------------------------------------------------- */
uint8 rfwcdma_core_util_device_to_antenna
(
  rfcom_device_enum_type device
);

/* ----------------------------------------------------------------------- */
uint8 rfwcdma_core_util_device_to_lna_id
(
  rfcom_device_enum_type device,
  rfwcdma_mdsp_rx_irat_meas_type meas_scenario
);

/*------------------------------------------------------------------*/
rfcom_multi_carrier_hspa_id_type 
rfwcdma_core_get_multi_carriers_from_carrier_mode
(
rfwcdma_mc_carrier_idx_enum_type carrier_mode
);

#endif /*RF_WCDMA_CORE_UTIL_H*/
