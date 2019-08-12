/*!
  @file
  ftm_tdscdma_selftest.c

  @brief
  This module contains definitions and prototypes for TDSCDMA selftest functionality.
*/

/*==============================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/ftm/src/ftm_tdscdma_selftest.c#1 $
  $DateTime: 2016/12/13 07:59:50 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/25/14   jmf     [selftest] Remove IQ sample capture save to EFS
11/24/14   jmf     [selftest] Add support for TDSCDMA VSWR mag/phase measurement
11/10/14   jmf     [selftest] Remove hard-coded FBRx gain switchpoints, get from Static NV
10/22/14   jmf     [selftest] Implement VSWR power ratio measurement
10/16/14   jmf     [selftest] Implement multi-gain stage switch for ACLR improvement
10/14/14   jmf     [selftest] Implement FBRx Noise Floor Cal
09/08/14   jmf     [SelfTest] Implement Tx Power measurement using TxAGC loop
08/20/14   jmf     [SelfTest] Remove switching of Tx waveform
08/14/14   jmf     [SelfTest] ACLR Meas Changes (not verified)
08/14/14   jps     Updates for TDS self test IQ capture
08/05/14   jmf     [SelfTest] Port Dime Defs for VSWR result return
07/26/14   lcl     Fix OFT compilation error
06/30/14   daa     Created.

==============================================================================*/

#include "DALStdDef.h"
#include "ftm_tdscdma_ctl.h"
#include "rfcom.h"
#include "ftm_common_control.h"
#include "rfcommon_time_profile.h"
#include "stringl.h"
#include "math.h"
#include "ftm_msg.h"
#include "rfm.h"
#include "ftm_common_iq_processing.h"
#include "ftm_common_dispatch.h"
#include "rf_test_task_v.h"
#include "ftm_common_selftest.h"
#include "ftm_tdscdma_selftest.h"
#include "rf_tdscdma_core_util.h"
#include "rfcommon_fbrx_types.h"
#include "ftm_common_fbrx.h"

#include "rfcommon_mdsp.h" // For IQ sample save to EFS

ftm_rf_fbrx_meas_tx_params_rsp_pkt_type *ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt;
ftm_rf_tdscdma_fbrx_iq_acquired_samples_data_type *ftm_rf_tdscdma_fbrx_iq_acquired_samples_data;
extern int16 ftm_tdscdma_power_dbm;
extern rfcom_tdscdma_band_type ftm_tdscdma_current_mode;
extern ftm_common_selftest_vswr_v2_results_type ftm_common_selftest_vswr_v2_results;

static boolean ftm_tdscdma_obtain_fbrx_gain_stage(uint8 * gain_stage);
static boolean ftm_tdscdma_obtain_fbrx_gain_swtichpoint_nv_item
(
   rfcom_tdscdma_band_type band,
   rfnv_item_id_enum_type *nv_item
);

// extern coupler_direction_enum_type tdscdma_coupler_direction;


// Commented out for 9x35 bringup
#if 0
/*============================================================================

FUNCTION
ftm_tdscdma_calculate_fbrx_gain

DESCRIPTION
Interpolate FBRX gain v.s. channel based on FBRX gain NV

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void ftm_tdscdma_calculate_fbrx_gain
(
  rfcommon_nv_fbrx_gain_vs_freq_type fbrx_gain_nv_data,
  uint16 channel,
  ftm_common_selftest_fbrx_attn_type gain_stage,
  float *fbrx_gain
)
{
  uint8 ch_size, idx;
  int16 ch_idx, gain_idx;
  double x0, y0, x1, y1, y;

  gain_idx = (int16) gain_stage;

  ch_size = fbrx_gain_nv_data.fbrx_gain_chan_size;

  if (ch_size == 0) {

    FTM_MSG(FTM_ERROR,"ftm_tdscdma_calculate_fbrx_gain: NV has 0 channel data, error" );

    return;
  }

  if (ch_size == 1) {

    /* only has one data point */

    *fbrx_gain = (float) fbrx_gain_nv_data.fbrx_gain[0][gain_idx].gain;

    FTM_MSG_1(FTM_HIGH,"ftm_tdscdma_calculate_fbrx_gain: NV has 1 channel data, FBRX gain = %d",  (int16) *fbrx_gain);

    return;

  } else
  {
    /* Has more than one data point */

    /* Take care of the extreme cases*/

    if (channel<=fbrx_gain_nv_data.fbrx_gain_chan[0])
    {
      *fbrx_gain = (float)fbrx_gain_nv_data.fbrx_gain[0][gain_idx].gain;
      FTM_MSG_3(FTM_HIGH,"ftm_tdscdma_calculate_fbrx_gain: NV has %d channel data, channel %d <channel[0], FBRX gain = %d",
                ch_size, channel, (int16) *fbrx_gain);
      return;
    }

    if (channel>=fbrx_gain_nv_data.fbrx_gain_chan[ch_size-1])
    {
      *fbrx_gain = (float)fbrx_gain_nv_data.fbrx_gain[ch_size-1][gain_idx].gain;
      FTM_MSG_3(FTM_HIGH,"ftm_tdscdma_calculate_fbrx_gain: NV has %d channel data, channel %d >channel[max], FBRX gain = %d",
                ch_size, channel, (int16) *fbrx_gain);
      return;
    }

    /* Determine location of the current channel in the list */

    ch_idx = 0;

    for (idx=0; idx<ch_size; idx++)
    {
      if (channel <= fbrx_gain_nv_data.fbrx_gain_chan[idx])
      {
        ch_idx = idx;
        break;
      }
    }

    FTM_MSG_3(FTM_HIGH,"ftm_tdscdma_calculate_fbrx_gain: channel[%d]<channel = %d <channel[%d]",
              idx-1, channel, idx);

    /* linear intepolation */

    x0 = (double) fbrx_gain_nv_data.fbrx_gain_chan[ch_idx-1];

    x1 = (double) fbrx_gain_nv_data.fbrx_gain_chan[ch_idx];

    y0 = (double) fbrx_gain_nv_data.fbrx_gain[ch_idx-1][gain_idx].gain;

    y1 = (double) fbrx_gain_nv_data.fbrx_gain[ch_idx][gain_idx].gain;

    y = y0 + (y1 - y0)*((double)channel-x0)/(x1-x0);

    *fbrx_gain = (float) y;

    FTM_MSG_1(FTM_HIGH,"ftm_tdscdma_calculate_fbrx_gain: interpolate, FBRX gain = %d",
                (int16) *fbrx_gain);

    //*fbrx_gain = fbrx_gain_nv_data.fbrx_gain[ch_idx][gain_idx].gain;
  }

  return;

}
#endif


// Commented out for 9x35 bringup
#if 0
/*============================================================================

FUNCTION
ftm_tdscdma_obtain_fbrx_gain_nv_item

DESCRIPTION
Determine FBRX gain NV based on LTE band

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/

boolean ftm_tdscdma_obtain_fbrx_gain_nv_item
(
   rfcom_tdscdma_band_type band,
   rfnv_item_id_enum_type *nv_item
)
{
  boolean ret_value = TRUE;
  switch (band)
  {

  case   RFCOM_BAND_TDSCDMA_B34:
    *nv_item = RFNV_TDSCDMA_B34_FBRX_GAIN_FREQ_I;
    break;

  case   RFCOM_BAND_TDSCDMA_B39:
    *nv_item = RFNV_TDSCDMA_B39_FBRX_GAIN_FREQ_I;
    break;

  case   RFCOM_BAND_TDSCDMA_B40:
    *nv_item = RFNV_TDSCDMA_B40_FBRX_GAIN_FREQ_I;
    break;

  default:

    *nv_item = RFNV_TDSCDMA_B34_FBRX_GAIN_FREQ_I;
    ret_value = FALSE;
  }

  return ret_value;
}
#endif


// Commented out for 9x35 bringup
#if 0
/*============================================================================

FUNCTION
ftm_tdscdma_calculate_power

DESCRIPTION
  Process FBRX gain NV, inteporlate gain v.s. freq, and calculate power dBm
  at antenna for a given band/channel RMS power

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/

void ftm_tdscdma_calculate_power
(
   uint32 pwr_rms,
   rfcom_tdscdma_band_type band,
   uint16 channel,
   ftm_common_selftest_fbrx_attn_type gain_stage,
   int16 *pwr_ant
)
{

   rfnv_item_id_enum_type nv_id;
   rfcommon_nv_fbrx_gain_vs_freq_type fbrx_gain_nv_data;
   double nbee_rms_offset = 0.0; // this is characterized offset when cal'ing with NBEE, measuring with IQ RMS
   float fbrx_gain;
   boolean nv_valid;

   if (pwr_ant != NULL) {
   if (rfm_get_calibration_state())
   {
       /* return raw IQ RMS in dB100 for caliration */
       *pwr_ant = (int16)FTM_SELFTEST_ROUND(2000 * log10(pwr_rms));
     FTM_MSG_1(FTM_HIGH, "ftm_tdscdma_calculate_power: calibrating, Raw IQ Rx RMS = %d",
                * pwr_ant);
     return;
   }

   /* determine the correct NV item location */

   nv_valid =  ftm_tdscdma_obtain_fbrx_gain_nv_item(band, &nv_id);

   if (nv_valid)
   {
        FTM_MSG_4(FTM_HIGH, "ftm_tdscdma_calculate_power: NV id =%d, channel = %d, band = %d, 200log10(power_rms)=%d",
                  nv_id, channel, band, (int16)(200*log10(pwr_rms)));

       read from the correct NV and populate the data structure for the current band from the NV
      rfcommon_nv_load_fbrx_gain_from_nv
         (&fbrx_gain_nv_data, nv_id,
          NULL, 0, NULL);


      /* determine the NBEE and RMS Power offset */
      nbee_rms_offset = 0.0;


      /* interpolate the FBRX cal data across freq to get the calculated gain for the current channel*/
      ftm_tdscdma_calculate_fbrx_gain(fbrx_gain_nv_data, channel, gain_stage, &fbrx_gain);

      /* power at ant is in dBm*100 */
      *pwr_ant = (int16)FTM_SELFTEST_ROUND(2000 * log10(pwr_rms) - (fbrx_gain + nbee_rms_offset*100));

      FTM_MSG_1(FTM_HIGH, "ftm_tdscdma_calculate_power: using interpolated FBRX gain NV, calculated pwr_ant = %d",
                 * pwr_ant);
   }
   else
   {

      /* band invalid, no NV stored, use default to bail out */
      *pwr_ant = (int16)FTM_SELFTEST_ROUND((20 * log10(pwr_rms) - nbee_rms_offset)*100);

      FTM_MSG_1(FTM_HIGH, "ftm_tdscdma_calculate_power: no valid NV for the band, using default FBRX gain pwr_ant = %d",
                 * pwr_ant);
   }
}
}
#endif


// Commented out for 9x35 bringup
#if 0
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Calculate power dBm at antenna

  @details

  @return
  power at antenna
*/

int16 ftm_tdscdma_ant_pwr
(
   uint32 pwr_rms
)
{
  rfcom_tdscdma_band_type band;
  uint16 channel;
  ftm_common_selftest_fbrx_attn_type gain_stage;
  int16 ant_pwr;

  band = rf_tdscdma_mc_state.curr_band_tx;

  channel = rf_tdscdma_mc_state.curr_chan_tx;

  gain_stage = ftm_common_get_coupler_wtr_fb_path_state();

  FTM_MSG_3(FTM_HIGH,"ftm_tdscdma_ant_pwr: band = %d, channel = %d, gain_stage = %d",
          band, channel, gain_stage);

  ftm_tdscdma_calculate_power(pwr_rms, band, channel,  gain_stage, &ant_pwr);

  FTM_MSG_1(FTM_HIGH,"ftm_tdscdma_ant_pwr return value: pwr_ant = %d (dBm * 10)",
            ant_pwr);

  return ant_pwr;
}
#endif


// Commented out for 9x35 bringup
#if 0
boolean ftm_tdscdma_obtain_fbrx_gain_swtichpoint_nv_item
(
   rfcom_tdscdma_band_type band,
   rfnv_item_id_enum_type *nv_item
)
{
  boolean ret_value = TRUE;
  switch (band)
  {

  case   RFCOM_BAND_TDSCDMA_B34:
    *nv_item = RFNV_TDSCDMA_B34_FBRX_GAIN_SWITCH_POINT_I;
    break;

  case   RFCOM_BAND_TDSCDMA_B39:
    *nv_item = RFNV_TDSCDMA_B39_FBRX_GAIN_SWITCH_POINT_I;
    break;

  case   RFCOM_BAND_TDSCDMA_B40:
    *nv_item = RFNV_TDSCDMA_B40_FBRX_GAIN_SWITCH_POINT_I;
    break;

  default:

    *nv_item = RFNV_TDSCDMA_B34_FBRX_GAIN_SWITCH_POINT_I;
    ret_value = FALSE;
  }

  return ret_value;
}
#endif


// Commented out for 9x35 bringup
#if 0
void ftm_tdscdma_obtain_fbrx_gain_stage(void)
{

    rfcom_tdscdma_band_type band;
    rfnv_item_id_enum_type nv_id;
    boolean nv_valid;
    int16 switch_pt;
    int32 txagc;
    rfcommon_nv_fbrx_attn_switch_pt_type fbrx_gain_switchpoint_nv_data;
    ftm_common_selftest_fbrx_attn_type fb_attenuation_state;

    band = rf_tdscdma_mc_state.curr_band_tx;

   /* determine the correct NV item location */

   nv_valid =  ftm_tdscdma_obtain_fbrx_gain_swtichpoint_nv_item(band, &nv_id);

   if (nv_valid)
   {

      FTM_MSG_2(FTM_HIGH, "ftm_tdscdma_obtain_fbrx_gain_stage: NV id =%d, band = %d",
                nv_id, band);


      /* read from the correct NV and populate the data structure for the current band from the NV*/

      rfcommon_nv_load_fbrx_attn_switch_pts_from_nv
         (&fbrx_gain_switchpoint_nv_data, nv_id,
          NULL, 0, NULL);

      switch_pt = fbrx_gain_switchpoint_nv_data.switch_pwr_dbm[0];

     FTM_MSG_1(FTM_HIGH, "ftm_tdscdma_obtain_fbrx_gain_stage: obtained from NV: gain switch point = %d",
                 switch_pt);

   }
   else
   {

     switch_pt = 140;

     FTM_MSG_1(FTM_HIGH, "ftm_tdscdma_obtain_fbrx_gain_stage: no valid NV for the band, using default gain switch point = %d",
                 switch_pt);
   }



    /* Obtain the TXAGC from SM variable */
    txagc=*ftm_tdscdma_power_dbm;


  if (txagc<switch_pt) {

    fb_attenuation_state = FTM_COMMON_SELFTEST_FBRX_ATTN_0;
    FTM_MSG_2(FTM_HIGH,
          " ftm_tdscdma_obtain_fbrx_gain_stage: TXAGC = :%d < switch pt = %d; set attenuation to LOW",
          txagc, switch_pt);
  } else
  {
    fb_attenuation_state = FTM_COMMON_SELFTEST_FBRX_ATTN_1;
    FTM_MSG_2(FTM_HIGH,
          " ftm_tdscdma_obtain_fbrx_gain_stage: TXAGC = :%d >= switch pt = %d; set attenuation to HIGH",
          txagc, switch_pt);
  }


  ftm_common_set_coupler_wtr_fb_path_state(fb_attenuation_state);

}
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize parameters and vars for selftest

  @details

  @return
  0 for success, 1 for failure
*/
uint8 ftm_tdscdma_selftest_init
(
  ftm_tdscdma_selftest_meas_params_struct *params,

  // ftm_common_power_per_bin_struct_type *noise_data_to_subtract,
  ftm_rf_tdscdma_payload_tx_params_type **ftm_rf_tdscdma_payload_tx_params,
  ftm_rsp_pkt_type *rsp_pkt,
  uint8 *ftm_selftest_pwr_only
)
{
   // memset((void*)noise_data_to_subtract, 0, sizeof(ftm_common_power_per_bin_struct_type));
   /* Get pointer to static structures */
   *ftm_rf_tdscdma_payload_tx_params = &(ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results);

   params->enable_tx_pwr_meas = (params->tx_measurement_config & FTM_FBRX_MEASURE_TX_POWER)? TRUE: FALSE;
   params->enable_evm_meas    = (params->tx_measurement_config & FTM_FBRX_MEASURE_EVM)? TRUE: FALSE;
   params->enable_aclr_meas   = (params->tx_measurement_config & FTM_FBRX_MEASURE_ACLR)? TRUE: FALSE;
   params->enable_vswr_meas   = (params->tx_measurement_config & FTM_FBRX_MEAS_VSWR)? TRUE: FALSE;
   params->enable_vswr_meas_v2= (params->tx_measurement_config & FTM_FBRX_MEAS_VSWR_V2)? TRUE: FALSE;

  // if (((params->enable_tx_pwr_meas) || (params->enable_vswr_meas)) &&
  //     ((!params->enable_evm_meas) && (!params->enable_aclr_meas)) ) {

  //    *ftm_selftest_pwr_only = TRUE;
  //  }
  //  else
  //  {
  //    *ftm_selftest_pwr_only = FALSE;
  //  }

   // FTM_MSG_1( FTM_HIGH, "TDSCDMA self_test TX or VSWR only =  %d ", *ftm_selftest_pwr_only );

   /* print some debug */
   FTM_MSG_3(FTM_MED, "Inputs to Meas Tx params command -> num_averages : %d : capture_offset : %d : tx_measurement_config : %x",
             params->num_averages,
             params->capture_offset,
             params->tx_measurement_config);

  /***************** Fill up RSP packet with standard data *****************/

  // Fill in the constant fields of the response packet.
  ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->cmd_code = 75;
  ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->subsys_id = FTM;
  ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->subsys_cmd_code = (params->device == RFM_DEVICE_0) ? FTM_TDSCDMA_C : FTM_TDSCDMA_RX_2_C ;
  ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->ftm_rf_cmd = FTM_RF_MEAS_TX_PARAMS;

  // Start with sucess flag and change it if we fail any check conditions.
  ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->ftm_error_code = FTM_FBRX_TEST_SUCCESS;
  ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->num_averages = params->num_averages;
  ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->capture_offset = params->capture_offset;
  ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->tx_measurement_config = params->tx_measurement_config;
  ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->payload_size = FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE;

  // Fill in the constant fields of response packet.Size of the packet will be based on the requested Tx measurement config
  rsp_pkt->cmd = FTM_RSP_DO_NEW_PKT;
  rsp_pkt->pkt_payload = ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt;
  rsp_pkt->delete_payload = FALSE;
  rsp_pkt->pkt_len = FTM_RF_MEAS_TX_PARAMS_HDR_SIZE;


  // if (!(rfm_get_calibration_state())) {

  //    if not in calibration mode, obtain and set overral FB path gain stage
  //   ftm_tdscdma_obtain_fbrx_gain_stage();

  //   /* otherwise use the settings set during cal */
  // }


  if( ftm_get_current_state(params->device) != FTM_STATE_TDSCDMA)
  {
    ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->ftm_error_code = FTM_FBRX_TEST_GENERAL_FAILURE;
    FTM_MSG(FTM_ERROR, "FTM Mode is not TDSCDMA, bailing out");
    return 1;
  }

  return 0;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize sample buffers and fft params

  @details

  @return
  0 for success, 1 for failure
*/
uint8 ftm_tdscdma_selftest_buffer_init
(
  const uint16 fft_size,
  const rfcom_lte_bw_type lte_bw,

  uint32 *sample_buffer_size,
  int32 **sample_buffer,
  uint16 *pcfl_data_size
)
{

  /* Buffer for samples and FFT - 32 bit I/32 bit Q*/
  *sample_buffer_size = (fft_size * 2 * 4);
  *sample_buffer = (int32 *)ftm_malloc(*sample_buffer_size);

  /* If pointer is NULL even after allocating memory, then declare it as failure*/
  if (*sample_buffer == NULL)
  {
     ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->ftm_error_code = FTM_FBRX_TEST_MEM_ALLOCATION_FAILURE;
     FTM_MSG(FTM_ERROR, "Unable to allocate memory for Rx - IQ buffer");
     return 1;
  }

  /* init FFT - required before calling FFT API for ACLR*/
  ftm_common_init_fft_data(FTM_STATE_TDSCDMA, lte_bw);

  *pcfl_data_size = (fft_size * 20) / 8;

  return 0;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Capture iq samples and process with fft

  @details

  @return
  None
*/
void ftm_tdscdma_selftest_capture_and_fft
(
  const ftm_tdscdma_selftest_meas_params_struct params,
  const uint16 sample_capture_size,
  const uint16 pcfl_data_size,
  const uint16 fft_size,
  const uint32 sample_buffer_size,

  int32 *sample_buffer,
  uint32 *evm_value_acc,
  uint64 *rx_rms_mag_acc,
  uint32 * samp_rate
)
{
  uint8 average_count  = 0;
  boolean capture_done = FALSE;
  uint16 evm_value     = 0;
  uint32 rx_rms_mag    = 0;
  int32 *iq_buf_rx_ptr = NULL;
  uint8 fft_step       = 0;
  ftm_common_selftest_fbrxgs_info_type fbrx_gs_info;
  boolean do_rxrms_once = FALSE;

  //uint32 capture_type = (params.enable_tx_pwr_meas) | (params.enable_aclr_meas<<2);
  uint32 capture_type = params.tx_measurement_config;
  
  fbrx_gs_info.override_enable = ftm_tdscdma_obtain_fbrx_gain_stage(&(fbrx_gs_info.selftest_fbrx_gs));
  fbrx_gs_info.com_band.tdscdma_band = rf_tdscdma_mc_state.curr_band_tx;
  fbrx_gs_info.wtr_lte_bw = 0;

  for (average_count = 0; average_count < params.num_averages; average_count++)
  {
    /* Trigger I/Q capture*/
    capture_done = ftm_common_fbrx_iq_capture_processing(
                                       FTM_RF_TECH_TDSCDMA,
                                       1, /* FBRX chain*/
                                       capture_type,
                                       sample_capture_size,
                                       &(ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->iq_buf_rx),
                                       &(ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->iq_buf_tx),
                                       &evm_value,
                                       &rx_rms_mag,
                                       samp_rate,
                                       &fbrx_gs_info);

    if (capture_done == TRUE)
    {
      ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->req_samples = sample_capture_size;
      ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->data_available = TRUE;
      #if 0
      /*dump acquired samples in EFS*/
      rfcommon_iq_efs_save(
      ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->iq_buf_tx,
      ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->iq_buf_rx,
      9216);
      #endif
     }

    if (params.enable_aclr_meas && sample_buffer != NULL)
    {
      iq_buf_rx_ptr = ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->iq_buf_rx;

      for (fft_step = 0; fft_step < sample_capture_size / fft_size; fft_step++)
      {
         /*get FFT_SIZE samples from the Rx buffer*/
         memscpy((void *)sample_buffer, sample_buffer_size, (void *)iq_buf_rx_ptr, pcfl_data_size);

         /* increase the ptr to point to next Rx samples*/
         iq_buf_rx_ptr = iq_buf_rx_ptr + (pcfl_data_size / sizeof(int32));

         /* Parse the data*/
         ftm_common_parse_PCFL20_to_IQ(FTM_STATE_TDSCDMA, sample_buffer, fft_size);

         /* take FFT of this data*/
         ftm_common_get_samples_fft_avg(fft_size, sample_buffer);
      }
    }

    if (params.enable_evm_meas)
    {
      *evm_value_acc = *evm_value_acc + evm_value;
      FTM_MSG_3(FTM_HIGH, "ftm_tdscdma_selftest_capture_and_fft: EVM reading #%d :  %d, accum : %d ", average_count, evm_value, *evm_value_acc);
      *rx_rms_mag_acc = *rx_rms_mag_acc + rx_rms_mag;
      FTM_MSG_3(FTM_HIGH, "ftm_tdscdma_selftest_capture_and_fft: IQ RMS Reading #%d: RxRMS=%d, RxRMSAcc=%d", average_count, rx_rms_mag, *rx_rms_mag_acc);
      do_rxrms_once = TRUE;
    }

    if (params.enable_tx_pwr_meas)
    {
      if (do_rxrms_once == FALSE)
      {
        *rx_rms_mag_acc = *rx_rms_mag_acc + rx_rms_mag;
        FTM_MSG_3(FTM_HIGH, "ftm_tdscdma_selftest_capture_and_fft: IQ RMS Reading #%d: RxRMS=%d, RxRMSAcc=%d", average_count, rx_rms_mag, *rx_rms_mag_acc);
      }
    }
  }
}


// Commented out for 9x35 bringup
#if 0
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reverse coupler direction and re-collect samples, then calculate vswr

  @details

  @return
  None
*/
void ftm_tdscdma_selftest_calculate_vswr
(
  const ftm_tdscdma_selftest_meas_params_struct params,
  const uint16 sample_capture_size,

  uint32 *rx_rms_reverse_mag_avg,
  uint32 *rx_rms_mag_avg,
  uint64 *rx_rms_reverse_mag_acc,
  uint8 *status
)
{
  uint8 average_count       = 0;
  boolean capture_done      = FALSE;
  uint32 rx_rms_reverse_mag = 0;
  uint16 evm_value          = 0;

  //ftm_common_set_coupler_direction(COUPLER_DIRECTION_REVERSE);
  for (average_count = 0; average_count < params.num_averages; average_count++)
  {

    // tdscdma_coupler_direction = COUPLER_DIRECTION_REVERSE;

    /* Trigger I/Q capture*/
    capture_done = ftm_common_fbrx_iq_capture_processing(
                                     FTM_RF_TECH_TDSCDMA,
                                     1,
                                     params.enable_vswr_meas,
                                     sample_capture_size,
                                     &(ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->iq_buf_rx),
                                     &(ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->iq_buf_tx),
                                     &evm_value,
                                     &rx_rms_reverse_mag);
    *rx_rms_reverse_mag_acc = *rx_rms_reverse_mag_acc + rx_rms_reverse_mag;
    FTM_MSG_4(FTM_HIGH, "ftm_tdscdma_selftest_calculate_vswr: Forward Avg %d Reverse reading #%d = %d , Reverse Acc = %d",
                 *rx_rms_mag_avg,
                 average_count,
                 rx_rms_reverse_mag,
                 *rx_rms_reverse_mag_acc);
  }

  ftm_common_set_coupler_direction(COUPLER_DIRECTION_FORWARD);

  // tdscdma_coupler_direction = COUPLER_DIRECTION_FORWARD;

  *rx_rms_reverse_mag_avg = (uint32)(*rx_rms_reverse_mag_acc/params.num_averages);
  FTM_MSG_3(FTM_HIGH, "ftm_tdscdma_selftest_calculate_vswr: Reverse IQ RMS Averaging :  %d / %d = %d",
               *rx_rms_reverse_mag_acc,
               params.num_averages,
               *rx_rms_reverse_mag_avg);

  if ( *rx_rms_mag_avg > *rx_rms_reverse_mag_avg )
  {
    ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.vswr_ratio_dB10 =
                                (200 * log10(*rx_rms_mag_avg)) - (200 * log10(*rx_rms_reverse_mag_avg));

    FTM_MSG_4(FTM_HIGH, "ftm_tdscdma_selftest_calculate_vswr: Reverse IQ RMS Averaging :  %d / %d = %d, VSWR(dB10) = %d",
                 *rx_rms_reverse_mag_acc,
                 params.num_averages,
                 *rx_rms_reverse_mag_avg,
                 ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.vswr_ratio_dB10);
  }
  else
  {
    ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.vswr_ratio_dB10 = 9999;
    *status = FTM_FBRX_TEST_GENERAL_FAILURE;
    FTM_MSG_2(FTM_ERROR, "ftm_tdscdma_selftest_calculate_vswr: Reverse IQ RMS Averaged %d > Forward IQ RMS Averaged %d, returning 9999",
                 *rx_rms_reverse_mag_avg,
                 *rx_rms_mag_avg);

  }
}
#endif


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Calculate ACLR from FFT data

  @details

  @return
  None
*/
void ftm_tdscdma_selftest_calculate_aclr
(
  const uint16 fft_size,
  const rfcom_lte_bw_type lte_bw,

  // ftm_common_power_per_bin_struct_type noise_data_to_subtract,
  int32 *sample_buffer
)
{
  rfnv_tdscdma_tx_tbl_type *nv_tbl = NULL;
  ftm_common_aclr_result_struct_type ftm_common_aclr_result;
  uint8 spec_inv = 1;
  ftm_common_power_per_bin_struct_type noise_pwr_to_subtract;

  memset((void*)&noise_pwr_to_subtract, 0, sizeof(ftm_common_power_per_bin_struct_type));

  nv_tbl = rf_tdscdma_core_util_init_tx_nv_table(ftm_tdscdma_current_mode);

  /* Validate NV table */
  if (nv_tbl == NULL)
  {
    FTM_MSG_1(FTM_HIGH, "ftm_tdscdma_selftest_calculate_aclr: Failed to get valid static NV pointer for band %d, assuming spectral inversion = 1",
                  ftm_tdscdma_current_mode);
  }
  else
  {
    spec_inv = (uint8)(nv_tbl->ept_dpd_cfg_params[DPD_CFG_SPEC_INV]);
  }

  ftm_common_get_aclr(fft_size, FTM_STATE_TDSCDMA, lte_bw, noise_pwr_to_subtract, &ftm_common_aclr_result, spec_inv);

  ftm_common_deinit_fft_data();

  ftm_free(sample_buffer);

  /* Copy ACLR values to FTM data structures*/
  ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.aclr_value[0] = ftm_common_aclr_result.aclr1_neg;
  ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.aclr_value[1] = ftm_common_aclr_result.aclr1_pos;
  ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.aclr_value[2] = ftm_common_aclr_result.aclr2_neg;
  ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.aclr_value[3] = ftm_common_aclr_result.aclr2_pos;
  ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.aclr_value[4] = ftm_common_aclr_result.aclr_eutra_neg;
  ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.aclr_value[5] = ftm_common_aclr_result.aclr_eutra_pos;

  FTM_MSG_6(FTM_HIGH, "aclr_value[0] : %d , aclr_value[1]: %d, aclr_value[2]: %d, aclr_value[3]: %d, aclr_value[4]: %d, aclr_value[5]: %d",
            ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.aclr_value[0],
            ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.aclr_value[1],
            ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.aclr_value[2],
            ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.aclr_value[3],
            ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.aclr_value[4],
            ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.aclr_value[5]
            );

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Populate response packet for selftest api

  @details

  @return
  None
*/
void ftm_tdscdma_selftest_populate_rsp_pkt
(
  const ftm_tdscdma_selftest_meas_params_struct params,
  const ftm_rf_tdscdma_payload_tx_params_type *ftm_rf_tdscdma_payload_tx_params,
  const uint32 *iq_capture_frequency,
  const uint32 *max_samples_per_diag_packet_tx,
  const uint32 *max_samples_per_diag_packet_rx,

  ftm_rsp_pkt_type *rsp_pkt
)
{

  int payload_size_count = 0;

  // Adding payload secion of response packet.
  // Check if Tx Power measurement type is enabled and add Tx Power results to the response packet
  if (params.tx_measurement_config & FTM_FBRX_MEASURE_TX_POWER)
  {
    /*Populate Tx power data*/
    memscpy(&ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_tdscdma_payload_tx_params->tx_power),
            sizeof(ftm_rf_tdscdma_payload_tx_params->tx_power));
    payload_size_count += sizeof(ftm_rf_tdscdma_payload_tx_params->tx_power);
    FTM_MSG_2(FTM_MED, "payload_size: %d : pwr : %d",ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count], ftm_rf_tdscdma_payload_tx_params->tx_power);
  }
  // Check if EVM measurement type is enabled and add EVM results to the response packet
  if (params.tx_measurement_config & FTM_FBRX_MEASURE_EVM)
  {
    /*Populate EVM data*/
    memscpy(&ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_tdscdma_payload_tx_params->evm_value),
            sizeof(ftm_rf_tdscdma_payload_tx_params->evm_value));
    payload_size_count += sizeof(ftm_rf_tdscdma_payload_tx_params->evm_value);
  }
  // Check if ACLR measurement type is enabled and add ACLR results to the response packet
  if (params.tx_measurement_config & FTM_FBRX_MEASURE_ACLR)
  {
    /*Populate ACLR data*/
    memscpy(&ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_tdscdma_payload_tx_params->aclr_value),
            sizeof(ftm_rf_tdscdma_payload_tx_params->aclr_value));
    payload_size_count += sizeof(ftm_rf_tdscdma_payload_tx_params->aclr_value);
  }
  if ((params.tx_measurement_config & FTM_FBRX_SAVE_TX_IQ_SAMPLES)
    || (params.tx_measurement_config & FTM_FBRX_SAVE_RX_IQ_SAMPLES))
  {
    /* Check if Tx IQ Capture measurement type is enabled and add Tx IQ capture results to the response packet*/
    if (params.tx_measurement_config & FTM_FBRX_SAVE_TX_IQ_SAMPLES)
    {
      /*Populate Tx IQ capture data format inforamtion*/
      ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count++] = (uint8) FTM_TDSCDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_TX;
      ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count++] = (uint8) FTM_FBRX_SAMPLE_SIZE_9216;

      /* Todo Check for Tx BW and accordingly choose sampling frequency*/
      memscpy(&ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
              (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
               iq_capture_frequency,
               sizeof(*iq_capture_frequency));

      payload_size_count += sizeof(*iq_capture_frequency);

      memscpy(&ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
              (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
               max_samples_per_diag_packet_tx,
               sizeof(*max_samples_per_diag_packet_tx));

      payload_size_count += sizeof(*max_samples_per_diag_packet_tx);
    }
    // Check if Rx IQ Capture measurement type is enabled and add Rx IQ capture results to the response packet
    if (params.tx_measurement_config & FTM_FBRX_SAVE_RX_IQ_SAMPLES)
    {
      /*Populate Rx IQ capture data format inforamtion*/
      ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count++] = (uint8) FTM_TDSCDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_RX;
      ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count++] = (uint8) FTM_FBRX_SAMPLE_SIZE_9216;

      memscpy(&ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
              (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
               iq_capture_frequency,
               sizeof(*iq_capture_frequency));
      payload_size_count += sizeof(*iq_capture_frequency);

      memscpy(&ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
              (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
               max_samples_per_diag_packet_rx,
               sizeof(*max_samples_per_diag_packet_rx));
      payload_size_count += sizeof(*max_samples_per_diag_packet_rx);
    }
  }
  // Check if VSWR measurement type is enabled and append VSWR result
  if (params.tx_measurement_config & FTM_FBRX_MEAS_VSWR)
  {
    /*Populate VSWR data*/
    memscpy(&ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_tdscdma_payload_tx_params->vswr_ratio_dB10),
            sizeof(ftm_rf_tdscdma_payload_tx_params->vswr_ratio_dB10));
    payload_size_count += sizeof(ftm_rf_tdscdma_payload_tx_params->vswr_ratio_dB10);
  }

  // Check if VSWR v2 measurement type is enabled and append VSWRv2 result
  if (params.tx_measurement_config & FTM_FBRX_MEAS_VSWR_V2)
  {
    /*Populate VSWR Return Loss*/
    memscpy(&ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_tdscdma_payload_tx_params->vswr_v2_return_loss),
            sizeof(ftm_rf_tdscdma_payload_tx_params->vswr_v2_return_loss));
    payload_size_count += sizeof(ftm_rf_tdscdma_payload_tx_params->vswr_v2_return_loss);

    /*Populate VSWR Magnitude*/
    memscpy(&ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_tdscdma_payload_tx_params->vswr_v2_mag),
            sizeof(ftm_rf_tdscdma_payload_tx_params->vswr_v2_mag));
    payload_size_count += sizeof(ftm_rf_tdscdma_payload_tx_params->vswr_v2_mag);

    /*Populate VSWR Phase*/
    memscpy(&ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_tdscdma_payload_tx_params->vswr_v2_phase),
            sizeof(ftm_rf_tdscdma_payload_tx_params->vswr_v2_phase));
    payload_size_count += sizeof(ftm_rf_tdscdma_payload_tx_params->vswr_v2_phase);

    /*Populate VSWR Forward Power*/
    memscpy(&ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_tdscdma_payload_tx_params->vswr_v2_fwd_pwr),
            sizeof(ftm_rf_tdscdma_payload_tx_params->vswr_v2_fwd_pwr));
    payload_size_count += sizeof(ftm_rf_tdscdma_payload_tx_params->vswr_v2_fwd_pwr);

    /*Populate VSWR Reverse Power*/
    memscpy(&ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_tdscdma_payload_tx_params->vswr_v2_rev_pwr),
            sizeof(ftm_rf_tdscdma_payload_tx_params->vswr_v2_rev_pwr));
    payload_size_count += sizeof(ftm_rf_tdscdma_payload_tx_params->vswr_v2_rev_pwr);

  }

  if (payload_size_count > FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE)
  {
    ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->ftm_error_code = FTM_FBRX_TEST_EXCEEDS_PAYLOAD_SIZE_LIMIT;
  } else
  {
    rsp_pkt->pkt_len +=  payload_size_count;
    ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->payload_size = payload_size_count;
  }
}


/*============================================================================

FUNCTION FTM_RF_MEAS_TX_PARAMS

DESCRIPTION
  This API is used to capture and process Tx IQ samples using FBRx and to calculate
  various Tx parameters (i.e. Tx Power, EVM and ACLR)

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

uint8 ftm_tdscdma_selftest
(
  ftm_tdscdma_selftest_meas_params_struct params,

  ftm_rsp_pkt_type *rsp_pkt,
  ftm_rf_fbrx_meas_tx_params_rsp_pkt_type *ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt_input,
  ftm_rf_tdscdma_fbrx_iq_acquired_samples_data_type *ftm_rf_tdscdma_fbrx_iq_acquired_samples_data_input,
  int16 *ftm_tdscdma_power_dbm_input,
  boolean *ftm_selftest_pwr_only
)
{

  uint8 status                          = FTM_FBRX_TEST_SUCCESS;
  uint32 iq_capture_frequency           = FTM_TDSCDMA_FBRX_IQ_SAMPLE_FREQ;
  uint32 max_samples_per_diag_packet_tx = FTM_TDSCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_TX;
  uint32 max_samples_per_diag_packet_rx = FTM_TDSCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_RX;

  rfcom_lte_bw_type lte_bw = RFCOM_BW_LTE_INVALID;      /* This parameter is LTE specific and TDSCDMA related algorithm ignores its value */

  uint32 sample_buffer_size = 0;
  int32 *sample_buffer      = NULL;
  ftm_rf_tdscdma_payload_tx_params_type *ftm_rf_tdscdma_payload_tx_params;

  uint32 evm_value_acc = 0;
  uint16 evm_value_avg;
  uint32 dummy_evm_val = 0;
  uint8 selftest_error = 0;
  uint64 rx_rms_mag_reverse_acc = 0;
  // uint32 rx_rms_reverse_mag_avg;
  uint64 rx_rms_mag_acc  = 0;
  uint32  rx_rms_mag_avg = 0;
  boolean vswr_from_rxrms    = FALSE;
  ftm_tdscdma_selftest_meas_params_struct params_mod_for_vswr_pwr_ratio;
  ftm_tdscdma_selftest_meas_params_struct params_mod_for_vswr_mag_phase;
  boolean revert_fbrx = FALSE;
  rfcommon_fbrx_error_type fbrx_error;
  lm_handle_type tx_handle;
  rfcommon_fbrx_tx_cfg_type fbrx_bw_mode;
  rfcom_band_type_u com_band;

  uint16 pcfl_data_size      = 0;
  uint16 sample_capture_size = 9216;
  uint16 fft_size            = 1024;
  uint32 samp_rate = -1;
  // ftm_common_power_per_bin_struct_type noise_data_to_subtract;

  uint8 fbrx_gs;
  int16 pwr_est_db10;
  boolean pwr_meas_success = FALSE;

  ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt = ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt_input;
  ftm_rf_tdscdma_fbrx_iq_acquired_samples_data = ftm_rf_tdscdma_fbrx_iq_acquired_samples_data_input;

  com_band.tdscdma_band = rf_tdscdma_mc_state.curr_band_tx;
  fbrx_bw_mode = RFCOMMON_FBRX_TDS;
  tx_handle = rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx;

  FTM_MSG(FTM_HIGH, "Entering selftest_init");
  selftest_error = ftm_tdscdma_selftest_init(
                                             &params,
                                             // &noise_data_to_subtract,
                                             &ftm_rf_tdscdma_payload_tx_params,
                                             rsp_pkt,
                                             ftm_selftest_pwr_only);
  FTM_MSG(FTM_HIGH, "Leaving selftest_init");
  if (selftest_error)
  {
    FTM_MSG(FTM_HIGH, "Selftest_init failed");
    status = FTM_FBRX_TEST_GENERAL_FAILURE;
    return 1;
  }

  if (params.enable_tx_pwr_meas)
  {
    pwr_meas_success = ftm_common_fbrx_meas_ant_pwr( &pwr_est_db10, &(ftm_rf_tdscdma_payload_tx_params->tx_power), params.num_averages, &fbrx_gs );
    FTM_MSG_5(FTM_HIGH, "pwr_est_db10: %d, pwr_est_db100: %d, num_avg: %d, status: %d, fbrx_gs: %d", 
              pwr_est_db10,
              ftm_rf_tdscdma_payload_tx_params->tx_power,
              params.num_averages,
              pwr_meas_success,
              fbrx_gs);
    if (!pwr_meas_success)
    {
      status = FTM_FBRX_TEST_GENERAL_FAILURE;
    }
  }

  params_mod_for_vswr_pwr_ratio = params;

  // need to turn off the VSWRv2 flag because that capture will need completely separate treatment
  // from ACLR/EVM/VSWRv1
  params_mod_for_vswr_pwr_ratio.enable_vswr_meas_v2 = FALSE;
  params_mod_for_vswr_pwr_ratio.tx_measurement_config = 
    params_mod_for_vswr_pwr_ratio.tx_measurement_config & ~FTM_FBRX_MEAS_VSWR_V2;

  if (params.enable_vswr_meas)
  {
    vswr_from_rxrms = TRUE;
    // turn off legacy vswr flag to pass into capture function, because
    // we will trigger either EVM or RxMRS type FW capture based on whether
    // EVM meas was also requested
    params_mod_for_vswr_pwr_ratio.enable_vswr_meas = FALSE;
    params_mod_for_vswr_pwr_ratio.tx_measurement_config = 
      (params_mod_for_vswr_pwr_ratio.tx_measurement_config & ~FTM_FBRX_MEAS_VSWR) ;
    // if EVM meas was requested as well, we can just also get RxRMS for FWD direction
    // through FW EVM Capture type
    // otherwise trigger RxRMS / TxPower type because it takes shorter time

    // don't do below part now because FW crashes for FW RxRMS cap type + Self Test flag TRUE
    // instead use the EVM FW cap type now
    #if 0
    if (params.enable_evm_meas == FALSE)
    {
      params_mod_for_vswr_pwr_ratio.enable_tx_pwr_meas = TRUE;
      params_mod_for_vswr_pwr_ratio.tx_measurement_config = 
        params_mod_for_vswr_pwr_ratio.tx_measurement_config | FTM_FBRX_MEASURE_TX_POWER;
    }
    #endif
    if (params.enable_evm_meas == FALSE)
    {
      params_mod_for_vswr_pwr_ratio.enable_evm_meas = TRUE;
      params_mod_for_vswr_pwr_ratio.tx_measurement_config = 
        params_mod_for_vswr_pwr_ratio.tx_measurement_config | FTM_FBRX_MEASURE_EVM;
    }
    FTM_MSG_2(FTM_MED, "vswr_from_rxrms flag: %d, tx_measurement_config: 0x%08x", 
              vswr_from_rxrms,
              params_mod_for_vswr_pwr_ratio.tx_measurement_config);
  }

  // if VSWR is required as Mag/Phase format, need to trigger separate FW VSWR Cap type and need to turn off EVM
  // Cap type flag for separate VSWR split capture
  if (params.enable_vswr_meas_v2)
  {
    params_mod_for_vswr_mag_phase = params;
    params_mod_for_vswr_mag_phase.enable_aclr_meas = FALSE;
    params_mod_for_vswr_mag_phase.enable_evm_meas = FALSE;
    params_mod_for_vswr_mag_phase.enable_tx_pwr_meas = FALSE;
    params_mod_for_vswr_mag_phase.enable_vswr_meas = FALSE;
    params_mod_for_vswr_mag_phase.tx_measurement_config =
      params_mod_for_vswr_mag_phase.tx_measurement_config
                                      & ~( FTM_FBRX_MEASURE_TX_POWER 
                                         | FTM_FBRX_MEASURE_EVM 
                                         | FTM_FBRX_MEASURE_ACLR
                                         | FTM_FBRX_MEAS_VSWR);
    FTM_MSG_1(FTM_MED, "modified tx_measurement_config for vswr_meas_v2: 0x%08x", 
              params_mod_for_vswr_mag_phase.tx_measurement_config);
  }

  if (params.enable_aclr_meas)
  {
    FTM_MSG(FTM_HIGH, "Entering ACLR Setup");
    selftest_error = ftm_tdscdma_selftest_buffer_init(
                                      fft_size,
                                      lte_bw,
                                      &sample_buffer_size,
                                      &sample_buffer,
                                      &pcfl_data_size);
    FTM_MSG(FTM_HIGH, "Leaving ACLR Setup");
    if (selftest_error)
    {
      FTM_MSG( FTM_HIGH, "Self_test failed at aclr_setup" );
      ftm_common_deinit_fft_data();
      if (sample_buffer != NULL)
      {
        ftm_free(sample_buffer);
      }
      status = FTM_FBRX_TEST_GENERAL_FAILURE;
      return 1;
    }
  }

  if (params.enable_aclr_meas || params.enable_evm_meas || params.enable_vswr_meas)
  {
    FTM_MSG(FTM_HIGH, "Entering capture and fft");
    if (revert_fbrx == FALSE)
    {
      rfcommon_fbrx_mc_enable_fbrx(FALSE);
      DALSYS_BusyWait(6000);
      revert_fbrx = TRUE;
    }
    // This delay is introduced to ensure previous TxAGC update ran to completion and freed FBRx
    ftm_tdscdma_selftest_capture_and_fft(
                    params_mod_for_vswr_pwr_ratio,
                    sample_capture_size,
                    pcfl_data_size,
                    fft_size,
                    sample_buffer_size,
                    sample_buffer,
                    &evm_value_acc,
                    &rx_rms_mag_acc,
                    &samp_rate);
  }
  if (params.enable_aclr_meas)
  {
    ftm_common_get_aclr_bins_struct_with_samp_rate(FTM_STATE_TDSCDMA, lte_bw, fft_size, samp_rate);
  }

  FTM_MSG(FTM_HIGH, "Leaving capture and fft");

  #if 0
  if (params.enable_tx_pwr_meas)
  {
    rx_rms_mag_avg = (uint32)(rx_rms_mag_acc/params.num_averages);
    FTM_MSG_3(FTM_HIGH, "ftm_tdscdma_selftest: RMS Averaging :  %d / %d = %d", rx_rms_mag_acc, params.num_averages, rx_rms_mag_avg);
    ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.tx_power  = rx_rms_mag_avg; // ftm_tdscdma_ant_pwr(rx_rms_mag_avg);
  }
  #endif

  if (params.enable_evm_meas)
  {
    evm_value_avg = (uint16)(evm_value_acc/params.num_averages);
    ftm_rf_tdscdma_fbrx_iq_acquired_samples_data->meas_results.evm_value = evm_value_avg;    /* add logic to do avergaing here*/
    FTM_MSG_3(FTM_HIGH, "ftm_tdscdma_selftest: EVM Averaging :  %d / %d = %d", evm_value_acc, params.num_averages, evm_value_avg);
  }

  /* If VSWR measuremnt is enabled change coupler direction and again measure the IQ RMS based power estimate*/
  // if(params.enable_vswr_meas)
  // {
  //   ftm_tdscdma_selftest_calculate_vswr(
  //                    params,
  //                    sample_capture_size,
  //                    &rx_rms_reverse_mag_avg,
  //                    &rx_rms_mag_avg,
  //                    &rx_rms_reverse_mag_acc,
  //                    &status);
  // }

  if (params.enable_aclr_meas)
  {
    FTM_MSG(FTM_HIGH, "Entering calculate ACLR");
    ftm_tdscdma_selftest_calculate_aclr(
                     fft_size,
                     lte_bw,
                     // noise_data_to_subtract,
                     sample_buffer);
    FTM_MSG(FTM_HIGH, "Leaving calculate ACLR");
  }

  if (vswr_from_rxrms)
  {
    // we can safely trigger RxRMS capture for Reverse direction RxRMS
    // as long as FW ensures that the RxRMS values themselves are comparable between
    // the two capture types

    // temporarily use EVM FW cap type  because FW crashes with FW CAP RXRMS with selftest flag set
    params_mod_for_vswr_pwr_ratio.enable_tx_pwr_meas = FALSE;
    params_mod_for_vswr_pwr_ratio.enable_aclr_meas = FALSE;
    params_mod_for_vswr_pwr_ratio.enable_evm_meas = TRUE;
    params_mod_for_vswr_pwr_ratio.tx_measurement_config = 
      (params_mod_for_vswr_pwr_ratio.tx_measurement_config
                                      & ~( FTM_FBRX_MEASURE_TX_POWER
                                         | FTM_FBRX_MEASURE_ACLR)) | FTM_FBRX_MEASURE_EVM ;
    FTM_MSG_1(FTM_MED, "modified tx_measurement_config for reverse RxRMS: 0x%08x", 
              params_mod_for_vswr_pwr_ratio.tx_measurement_config);

    if (revert_fbrx == FALSE)
    {
      rfcommon_fbrx_mc_enable_fbrx(FALSE);
      DALSYS_BusyWait(6000);
      revert_fbrx = TRUE;
    }

    ftm_common_set_coupler_direction(1); //COUPLER_DIRECTION_REVERSE);
    ftm_tdscdma_selftest_capture_and_fft(
                    params_mod_for_vswr_pwr_ratio,
                    sample_capture_size,
                    pcfl_data_size,
                    fft_size,
                    sample_buffer_size,
                    sample_buffer,
                    &dummy_evm_val,
                    &rx_rms_mag_reverse_acc,
                    &samp_rate);
    ftm_common_set_coupler_direction(0); //COUPLER_DIRECTION_FORWARD);
    if (rx_rms_mag_acc > rx_rms_mag_reverse_acc )
    {
      ftm_rf_tdscdma_payload_tx_params->vswr_ratio_dB10 = 
                  (200 * log10(rx_rms_mag_acc)) - (200 * log10(rx_rms_mag_reverse_acc));

      FTM_MSG_3(FTM_HIGH, "ftm_tdscdma_selftest: Forward IQ RMS avg = %d, Reverse IQ RMS avg = %d, VSWR(dB10) = %d", 
                   rx_rms_mag_acc,
                   rx_rms_mag_reverse_acc, 
                   ftm_rf_tdscdma_payload_tx_params->vswr_ratio_dB10);
    }
    else
    {
      ftm_rf_tdscdma_payload_tx_params->vswr_ratio_dB10 = 9999;
      //status = FTM_FBRX_TEST_GENERAL_FAILURE;
      FTM_MSG_2(FTM_ERROR, "ftm_tdscdma_selftest: Reverse IQ RMS Averaged %d > Forward IQ RMS Averaged %d, returning 9999", 
                   rx_rms_mag_reverse_acc, 
                   rx_rms_mag_acc);
    }
    ftm_common_set_coupler_direction (0);
  }


  if (params.enable_vswr_meas_v2)
  {
    // only trigger capture for now, getting measurement will come later 
    // and need FTM return payload extension as well as API updates
    if (revert_fbrx == FALSE)
    {
      rfcommon_fbrx_mc_enable_fbrx(FALSE);
      DALSYS_BusyWait(6000);
      revert_fbrx = TRUE;
    }

    ftm_tdscdma_selftest_capture_and_fft(
                    params_mod_for_vswr_mag_phase,
                    sample_capture_size,
                    pcfl_data_size,
                    fft_size,
                    sample_buffer_size,
                    sample_buffer,
                    &dummy_evm_val,
                    &rx_rms_mag_reverse_acc,
                    &samp_rate);
    // fill up the return packet structure with results
    ftm_rf_tdscdma_payload_tx_params->vswr_v2_mag = ftm_common_selftest_vswr_v2_results.vswr_v2_mag;
    ftm_rf_tdscdma_payload_tx_params->vswr_v2_phase = ftm_common_selftest_vswr_v2_results.vswr_v2_phase;
    ftm_rf_tdscdma_payload_tx_params->vswr_v2_return_loss = ftm_common_selftest_vswr_v2_results.vswr_v2_return_loss;
    ftm_rf_tdscdma_payload_tx_params->vswr_v2_fwd_pwr= ftm_common_selftest_vswr_v2_results.vswr_v2_fwd_pwr;
    ftm_rf_tdscdma_payload_tx_params->vswr_v2_rev_pwr= ftm_common_selftest_vswr_v2_results.vswr_v2_rev_pwr;  
  }


  if (revert_fbrx == TRUE)
  {
    fbrx_error = rfcommon_fbrx_mc_reconfigure_rxlm
                                    ( 
                                      RFM_TDSCDMA_MODE, 
                                      fbrx_bw_mode,
                                      (rflm_handle_tx_t)tx_handle,
                                      com_band
                                    );
    if (fbrx_error != RFCOMMON_FBRX_SUCCESS)
    {
      FTM_MSG_4(FTM_ERROR, "Could not reconfigure FBRx LM with Tech %d FBRx_BW_Mode %d tx_handle %d TDS band %d", 
                RFM_TDSCDMA_MODE,
                fbrx_bw_mode,
                tx_handle,
                com_band.tdscdma_band);
      status = FTM_FBRX_TEST_GENERAL_FAILURE;
    }
    else
    {
      FTM_MSG_4(FTM_HIGH, "Reconfigured FBRx LM with Tech %d FBRx_BW_Mode %d tx_handle %d TDS band %d", 
                RFM_TDSCDMA_MODE,
                fbrx_bw_mode,
                tx_handle,
                com_band.tdscdma_band);
    }
    rfcommon_fbrx_mc_enable_fbrx(TRUE);
  }

  // update error code
  ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt->ftm_error_code = status;

  if (status == FTM_FBRX_TEST_SUCCESS)
  {
    FTM_MSG(FTM_HIGH, "Populating response packet");
    ftm_tdscdma_selftest_populate_rsp_pkt(
      params,
      ftm_rf_tdscdma_payload_tx_params,
      &iq_capture_frequency,
      &max_samples_per_diag_packet_tx,
      &max_samples_per_diag_packet_rx,
      rsp_pkt);
    FTM_MSG(FTM_HIGH, "Leaving populate response packet");
  }

  return 0;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Determine FBRX gain NV based on TDSCDMA band

  @details

  @return 
  None
*/

static boolean ftm_tdscdma_obtain_fbrx_gain_swtichpoint_nv_item
(
   rfcom_tdscdma_band_type band,
   rfnv_item_id_enum_type *nv_item
)
{
  boolean ret_value = TRUE;
  switch (band)
  {

  case   RFCOM_BAND_TDSCDMA_B34:
    *nv_item = RFNV_TDSCDMA_B34_SELFTEST_FBRX_GAIN_SWITCH_POINT_I;
    break;

  case   RFCOM_BAND_TDSCDMA_B39:
    *nv_item = RFNV_TDSCDMA_B39_SELFTEST_FBRX_GAIN_SWITCH_POINT_I;
    break;

  case   RFCOM_BAND_TDSCDMA_B40:
    *nv_item = RFNV_TDSCDMA_B40_SELFTEST_FBRX_GAIN_SWITCH_POINT_I;
    break;

  default:

    *nv_item = RFNV_TDSCDMA_B34_SELFTEST_FBRX_GAIN_SWITCH_POINT_I;
    ret_value = FALSE;

  }

  return ret_value;
}

static boolean ftm_tdscdma_obtain_fbrx_gain_stage(uint8 * gain_stage)
{
  rfcom_tdscdma_band_type band;
  rfnv_item_id_enum_type nv_id;
  boolean nv_valid;
  int8 gain_stage_iter;
  int16 txagc_val;
  boolean ret_val = TRUE;
  rfcommon_nv_selftest_fbrx_gain_switch_pt_type fbrx_gain_switchpoint_nv_data;

  band = rf_tdscdma_mc_state.curr_band_tx;
  txagc_val = ftm_tdscdma_power_dbm;

  /* determine the correct NV item location */
  nv_valid =  ftm_tdscdma_obtain_fbrx_gain_swtichpoint_nv_item(band, &nv_id);

  if (nv_valid)
  {
    FTM_MSG_2(FTM_HIGH, "ftm_tdscdma_obtain_fbrx_gain_stage: NV id =%d, band = %d",
              nv_id, band);

    /* read from the correct NV and populate the data structure for the current band from the NV*/
    rfcommon_nv_load_selftest_fbrx_gain_switch_pts_from_nv
         (&fbrx_gain_switchpoint_nv_data, nv_id,
          NULL, 0, NULL);

    *gain_stage = 0;
    for (gain_stage_iter=fbrx_gain_switchpoint_nv_data.num_switch_pts-2; gain_stage_iter>=0; gain_stage_iter--)
    {
      if (txagc_val >= fbrx_gain_switchpoint_nv_data.switch_pwr_dbm[gain_stage_iter])
      {
        *gain_stage = (uint8)(gain_stage_iter+1);
        break;
      }
    }

  }
  else
  {
    *gain_stage = 2;
    FTM_MSG(FTM_HIGH, "ftm_tdscdma_obtain_fbrx_gain_stage: no valid NV for the band, using default gain stage");
    ret_val = FALSE;
  }

  FTM_MSG_2(FTM_HIGH, "ftm_tdscdma_obtain_fbrx_gain_stage - txagc_val: %d, selected fbrx gain stage: %d", 
                       txagc_val, *gain_stage);

  return ret_val;

}