#ifndef RFNV_TDSCDMA_H
#define RFNV_TDSCDMA_H

/*!
  @file
  rfnv_tdscdma.h

  @brief

  @details
  This header file contains definitions  to interface with RF NV for TDSCDMA.

*/

/*===========================================================================

Copyright (c) 2011, 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/rfnv_tdscdma.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/30/14   jak     Added RFNV_TDSCDMA_<band>_MPR_VAL_V2_I support
11/03/14   jps     Updates for get PA switchpoints API
10/14/14   jps     Add support for GET_PA_SWITCHPOINTS command
07/17/14   ms      Sawless RX support
05/29/14   ych     Updated for 8 PA gain states
08/25/13   jhe     Add DPD scaling NV
07/11/13   jz      Add Antenna Tuner Close Loop NV Support
05/20/13   al      Add NV support for B40_B
04/19/13   kb      Add NV support for EPT feature 
07/11/12   jps     TD-SCDMA Antenna Tuner Feature 
07/09/12   ms	   Diversity support
05/02/12   ycl     Add MPR and threshold.
05/23/11   hy      Split Tx/Rx NV ptrs, fix hdet vs agc size
04/19/11   hy      Initial Revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "nv.h"
#include "rfcommon_nv.h"
#include "rfc_card.h"
#include "rfcommon_core_xpt.h"
#include "rfcommon_core_txlin.h"
/*===========================================================================

                           CONSTANT DEFINITIONS

===========================================================================*/
#define RF_TDSCDMA_PATHS_TABLE_SIZ      2

#define RF_TDSCDMA_MAX_LNA_GAIN_STATES 4

#define RF_TDSCDMA_NUM_TX_LINEARIZERS 4

#define RF_TDSCDMA_TX_LIN_MASTER_SIZ 64

#define RF_TDSCDMA_MAX_CAL_CHAN 16

#define RF_TDSCDMA_TX_TEMP_COMP_SIZ 8

#define RF_TDSCDMA_MPR_SIZ 7

#define RF_TDSCDMA_MPR_V2_SIZ 17

#define RF_TDSCDMA_HDET_VS_AGC_SIZ 16

#define RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE 12

#define RF_TDSCDMA_NV_SCALED_THERM_MIN 0
#define RF_TDSCDMA_NV_SCALED_THERM_MAX 255
#define RF_TDSCDMA_NV_SINGLE_TEMP_BIN_SIZE \
                       (RF_TDSCDMA_NV_SCALED_THERM_MAX/(NV_TEMP_TABLE_SIZ-1))

#define RF_TDSCDMA_NUM_RX_LIN_STATES 2

/*===========================================================================

                       PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef struct
{
  /* Tx linearizer master offset, 64 offsets per PA state */
  int16 tx_lin_master[RFCOM_TXLIN_NUM_LINEARIZERS][RF_TDSCDMA_TX_LIN_MASTER_SIZ];
  uint16 tx_lin_gain_idx[RFCOM_TXLIN_NUM_LINEARIZERS][RF_TDSCDMA_TX_LIN_MASTER_SIZ];

} rf_tdscdma_tx_linearizer_type;

typedef struct { 
  
  uint16                                           rise_delay;
  
  uint16                                           fall_delay;
} rf_tdscdma_agc_pa_on_rise_fall_delay_type;

typedef struct { 
  
  uint16                                           rise_delay;
  
  uint16                                           fall_delay;
}  rf_tdscdma_agc_tx_on_rise_fall_delay_type;

typedef struct { 
  
  uint16                                           up_time;
  
  uint16                                           down_time;
}  rf_tdscdma_pa_gain_up_down_time_type;

typedef struct { 
  
  int16                                            up_comp;
  
  int16                                            down_comp;
}  rf_tdscdma_pa_compensate_up_down_type;

typedef struct { 
  
  int16                                           rise_threshold;
  
 int16                                           fall_threshold;
}  rf_tdscdma_pa_rise_fall_threshold_type;

typedef enum
{
  RFNV_TDSCDMA_B34,
  RFNV_TDSCDMA_B39,
  RFNV_TDSCDMA_B40,
  RFNV_TDSCDMA_B40_B,

  /* add additional mode enum field here */

  RFNV_TDSCDMA_SUPPORTED_BAND_NUM
} rfnv_tdscdma_supported_band_type;

/*! 
  @brief  
  Tx Lin vs Temp vs Freq Pin/Pout NV structure
*/
typedef PACK (struct)
{
  /*! Lin vs Temp vs Freq NV information */
  uint8 reserved;
  int16 lin_vs_temp_vs_freq[RFCOM_TXLIN_NUM_LINEARIZERS]
                           [RF_TDSCDMA_TX_TEMP_COMP_SIZ]
                           [RF_TDSCDMA_MAX_CAL_CHAN];

}rf_tdscdma_nv_tx_lin_vs_temp_vs_freq_type;

/* TDSCDMA NV calibration table for Rx items */
typedef struct
{  
  /* rx calibration channels list */
  int16  rx_cal_chan[RF_TDSCDMA_MAX_CAL_CHAN];
  /* lna threshold in narrow band in acquisation mode*/
  uint8   lna_range_rise_fall_nb_mode_acq[RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE];
  /* lna threshold in wide band in acquisation mode*/
  uint8   lna_range_rise_fall_wb_mode_acq[RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE];
  /* lna threshold in narrow band in acquisation mode*/
  uint8   lna_range_rise_fall_nb_mode_acq_2[RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE];
  /* lna threshold in wide band in acquisation mode*/
  uint8   lna_range_rise_fall_wb_mode_acq_2[RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE];
  /* lna threshold in narrow band in idle mode*/
  uint8   lna_range_rise_fall_nb_mode_idle[RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE];
  /* lna threshold in wide band in idle mode*/
  uint8   lna_range_rise_fall_wb_mode_idle[RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE];
  /* lna threshold in narrow band in inter freq mode*/
  uint8   lna_range_rise_fall_nb_mode_ifreq[RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE];
  /* lna threshold in wide band in inter freq mode*/
  uint8   lna_range_rise_fall_wb_mode_ifreq[RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE];
  /* lna threshold in narrow band in power scan mode*/
  uint8   lna_range_rise_fall_nb_mode_power_scan[RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE];
  /* lna threshold in wide band in power scan mode*/
  uint8   lna_range_rise_fall_wb_mode_power_scan[RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE];
  /* lna threshold in narrow band in tracking mode*/
  uint8   lna_range_rise_fall_nb_mode_tracking[RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE];
  /* lna threshold in wide band in tracking mode*/
  uint8   lna_range_rise_fall_wb_mode_tracking[RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE];
  /* lna threshold in narrow band in tracking mode*/
  uint8   lna_hs_range_rise_fall_nb_mode_tracking[RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE];
  /* lna threshold in wide band in tracking mode*/
  uint8   lna_hs_range_rise_fall_wb_mode_tracking[RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE];

  /* absolute rx gain vs frequency */
  int16    rx_gain_vs_freq[RF_TDSCDMA_MAX_LNA_GAIN_STATES][RF_TDSCDMA_MAX_CAL_CHAN];
  
//#ifdef FEATURE_RF_TDSCDMA_RX_DIVERSITY
  /* absolute rx_div gain vs frequency */
  int16    rx_div_gain_vs_freq[RF_TDSCDMA_MAX_LNA_GAIN_STATES][RF_TDSCDMA_MAX_CAL_CHAN];
  /* rx diverstiy calibration channels list */
  int16    rx_div_cal_chan[RF_TDSCDMA_MAX_CAL_CHAN];
//#endif

 /* primary path LNA phase control in TDSCDMA band */
  uint16  lna_phase_ctrl[RF_TDSCDMA_MAX_LNA_GAIN_STATES];
  /* antenna tuner NVs for RX */
  rfcommon_nv_ant_tuner_type ant_tuner_rx;
  /* NV Related to RX_GAIN_VS_FREQ  */
  rfnv_rx_cal_data_type rx_band_cal_data;
} rfnv_tdscdma_rx_tbl_type;

/* TDSCDMA NV calibration table for Tx items */
typedef struct
{
  /* tx calibration channels list */
  int16  tx_cal_chan[RF_TDSCDMA_MAX_CAL_CHAN];
  /* UE maximum TX power */
  int16    max_tx_power;

  /* tx delays and update agc time */
   rf_tdscdma_agc_pa_on_rise_fall_delay_type  agc_pa_on_rise_fall_delay;
   rf_tdscdma_agc_tx_on_rise_fall_delay_type  agc_tx_on_rise_fall_delay;
   uint16  agc_update_tx_agc_time;
   /* pa range update time */
   rf_tdscdma_pa_gain_up_down_time_type  pa_gain_up_down_time;
   /* tx rotator angles */
   uint16 tx_rot_angle_pa_state[RFCOM_TXLIN_NUM_LINEARIZERS];
   /* PA Compensation UP/Down for PA switch threshold from gain states*/
   rf_tdscdma_pa_compensate_up_down_type   pa_compensate_up_down[RFCOM_TXLIN_NUM_LINEARIZERS - 1];//up/down pair
   /* Tx linearizer versus temperature */
   int8    tx_lin_vs_temp[RFCOM_TXLIN_NUM_LINEARIZERS][RF_TDSCDMA_TX_TEMP_COMP_SIZ];
   /* PA Range Map */
   int8    pa_range_map[RFCOM_TXLIN_NUM_LINEARIZERS];
   /* timer hysterisis for swithcing from high to low gain state*/
   int16   timer_hysterisis;
   /* rise/fall value for PA switch threshold from Linearizer gain states*/
   rf_tdscdma_pa_rise_fall_threshold_type  pa_rise_fall[RFCOM_TXLIN_NUM_LINEARIZERS - 1];//rise/fall pair
   /* offset Tx power limit versus temperature */
   int8    tx_limit_vs_temp[RF_TDSCDMA_TX_TEMP_COMP_SIZ];

   /* pa switch point shift, one for each channel */
   uint16 mpr_pa_switchpoints_shift[RF_TDSCDMA_MPR_SIZ];
   /* MPR power backoff */
   uint16 mpr_backoff_val[RF_TDSCDMA_MPR_SIZ];  
   /* MPR power backoff V2 */
   uint16 mpr_backoff_val_v2[RF_TDSCDMA_MPR_V2_SIZ];     
   /* MPR Analog Power backoff */
   uint16  mpr_analog_backoff[RF_TDSCDMA_MPR_SIZ];
   /* MPR Digital Power backoff */
   int16  mpr_digital_backoff[RF_TDSCDMA_MPR_SIZ];
   
   /* linearizer table of 64 entry for pa range 0 */
   rf_tdscdma_tx_linearizer_type tx_linearizer;
   /* tx power freq compensation per pa range */
   int8  tx_comp_vs_freq[RFCOM_TXLIN_NUM_LINEARIZERS][RF_TDSCDMA_MAX_CAL_CHAN];   
   /* Expected HDET versus AGC */
   uint16  exp_hdet_vs_agc[RF_TDSCDMA_HDET_VS_AGC_SIZ];
   /* tx limiting freq compensation for high PA range */
   int8  tx_limit_vs_freq[RF_TDSCDMA_MAX_CAL_CHAN];
   /* antenna tuner NVs for TX */
   rfcommon_nv_ant_tuner_type ant_tuner_tx;

   /* EPT DPD static params for expansion and processing */
   uint32 ept_dpd_cfg_params[MAX_EPT_NUM_CONFIG_PARAMS];
   /* Lin vs Temp vs Freq P_in Compensation */
   rf_tdscdma_nv_tx_lin_vs_temp_vs_freq_type p_in_comp;
   /* Lin vs Temp vs Freq P_out Compensation */
   rf_tdscdma_nv_tx_lin_vs_temp_vs_freq_type p_out_comp;
   
   /* Tuner CL NV support */
   rfcommon_nv_ant_tuner_cl_ctrl_type ant_tuner_cl;

   /* DPD scaling data */
   xpt_dpd_scaling_data_type dpd_scaling_data;

   /* Holds temp comp static NV data from 
      legacy as well as Pin/Pout Temp vs Freq NV items */
   rfcom_txlin_temp_comp_nv_data_type lin_temp_comp_data;
   
   /* Appending new NV data */
   rfcommon_nv_cmn_pa_static_type pa_static;
} rfnv_tdscdma_tx_tbl_type;

/* pointer to TDSCDMA NV table Rx items */
extern rfnv_tdscdma_rx_tbl_type   *rfnv_tdscdma_rx_tbl_ptr;

/* pointer to TDSCDMA NV table Tx items */
extern rfnv_tdscdma_tx_tbl_type   *rfnv_tdscdma_tx_tbl_ptr;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
rfnv_tdscdma_rx_tbl_type *rfnv_tdscdma_get_rx_tbl_ptr
(
rfnv_tdscdma_supported_band_type band            //!< band to get table ptr for
);

rfnv_tdscdma_tx_tbl_type *rfnv_tdscdma_get_tx_tbl_ptr
(
rfnv_tdscdma_supported_band_type band            //!< band to get table ptr for
);

rfcommon_nv_status_type rfnv_tdscdma_retrieve_items
(
rex_tcb_type *task_ptr,                     //!< pointer to tcb of the calling task
rex_sigs_type task_wait_sig,                //!< signal that calling task waiting for
void (*task_wait_handler)( rex_sigs_type )  //!< function ptr to callback function
);

/* ----------------------------------------------------------------------- */
rfnv_tdscdma_supported_band_type rfnv_tdscdma_convert_band_rfcom_to_rfnv 
(
  rfcom_tdscdma_band_type band
);

/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the NV valid flag to the correct value based on BC config

  @details
  This function is used to set the NV valid flag for each band which is enabled
  in NV_BC_Config to TRUE so that when NV is read part of Reload RF NV. This way
  we can avoid missing any NV for the active bands if this flag has been set to
  FALSE by someone else.

  @retval

*/
void rfnv_tdscdma_reset_valid_flags(void);

/* ----------------------------------------------------------------------- */
void rfnv_tdscdma_load_equidistant_therm_bins_data
(
  rfnv_tdscdma_tx_tbl_type *nv_tbl_ptr
);

/* ----------------------------------------------------------------------- */
void rfnv_tdscdma_load_lin_vs_temp_vs_freq
(
  rfnv_tdscdma_tx_tbl_type *nv_tbl_ptr,
  rfnv_item_type           *rfnv_item_ptr,
  boolean                  is_pin
);

/* ----------------------------------------------------------------------- */
void rfnv_tdscdma_load_lin_vs_temp_tx_char_chan
(
  rfnv_tdscdma_tx_tbl_type *nv_tbl_ptr
);

void rfnv_tdscdma_load_lin_vs_temp_vs_freq_from_pa_static
(
  rfnv_tdscdma_tx_tbl_type* nv_tbl_ptr
);

/* ----------------------------------------------------------------------- */
boolean rfnv_tdscdma_get_new_nv_flag
(
  rfcom_tdscdma_band_type band
);

/* ----------------------------------------------------------------------- */
boolean rfnv_tdscdma_get_pa_swpts(rfcom_tdscdma_band_type band, int16* rise_swpts, int16* fall_swpts, int16* active_pa_states);

#endif /* RFNV_TDSCDMA_H */
