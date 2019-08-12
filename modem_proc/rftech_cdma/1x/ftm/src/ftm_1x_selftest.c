/*!
  @file
  ftm_1x_selftest.c

  @brief
  This module contains 1x selftest code for FTM.

  @addtogroup RF_FTM_1X_SELFTEST
  @{
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/ftm/src/ftm_1x_selftest.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/25/14   jmf     [selftest] Remove IQ sample capture save to EFS
11/10/14   jmf     [selftest] Remove hard-coded FBRx gain switchpoints, get from Static NV
11/07/14   dbz     [selftest] Add support for VSWR V2 capture and processing
10/22/14   jmf     [selftest] Implement VSWR power ratio measurement
10/16/14   jmf     [selftest] Implement multi-gain stage switch for ACLR improvement
10/08/14   dbz     [selftest] Add VSWR coupler control
09/08/14   jmf     [SelfTest] Implement Tx Power measurement using TxAGC loop
08/20/14   jmf     Fix Warnings
08/20/14   jmf     Fix KW
08/18/14   jmf     Fix Bug caught by KW
08/18/14   jmf     [SelfTest] ACLR meas changes
08/05/14   jmf     [SelfTest] Port Dime Defs for VSWR result return
07/23/14   pk      Compiler Error fix on DPM.2.0 1x compiled out build
07/25/14   sty     off-target fix
07/24/14   dbz     Fix KW errors
07/23/14   dbz     Added support for selftest VSWR split and EVM capture
07/08/14   daa     Initial Revision

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "ftm_1x_selftest.h"
#include "rfcommon_time_profile.h"
#include "rfm_1x.h"
#include "rfm.h"
#include "ftm_cdma_data.h"
#include "ftm_common_control.h"
#include "rf_test_task_v.h"
#include "ftm_common_iq_processing.h"
#include "ftm_msg.h"
#include <stringl/stringl.h>
#include "msg.h"
#include "DALStdDef.h"
#include "rfcommon_time_profile.h"
#include "math.h"
#include "rfnv_items.h"
#include "sys.h"
#include "rf_cdma_utils_freq.h"
#include "ftm_1x_control.h"
#include "ftm_common_selftest.h"
#include "rfcommon_mdsp.h"
#include "ftm_common_fbrx.h"
#include "rf_cdma_data.h"

#define FBRX_AGC_MEAS_WAIT_TIME_US 2000

extern int16 ftm_1x_tx_power_dbm;

extern boolean selftest_lowband;

extern ftm_common_selftest_vswr_v2_results_type ftm_common_selftest_vswr_v2_results;

static boolean ftm_1x_obtain_fbrx_gain_stage(uint8 * gain_stage);
static boolean ftm_1x_obtain_fbrx_gain_swtichpoint_nv_item
(
   rfm_cdma_band_class_type band,
   rfnv_item_id_enum_type *nv_item
);

// Commented out for 9x35 bringup
#if 0
static boolean ftm_1x_fbrx_avg_rx_agc_val
(
   rfm_device_enum_type device,
   rfm_device_enum_type device_fb,
   uint16 num_averages,
   int16 *avg_rxagcval,
   ftm_common_selftest_fbrx_attn_type *fbrx_gain_state
);
/* function get averaged raw rx agc value */
#endif




// Commented out for 9x35 bringup
#if 0
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Determine FBRX gain NV based on LTE band

  @details

  @return
  None
*/

boolean ftm_1x_obtain_fbrx_gain_nv_item
(
   sys_band_class_e_type band,
   rfnv_item_id_enum_type *nv_item
)
{
  boolean ret_value = TRUE;
  switch (band)
  {

  case   SYS_BAND_BC0:
    *nv_item = RFNV_CDMA_BC0_FBRX_GAIN_FREQ_I;
    break;

  case   SYS_BAND_BC1:
    *nv_item = RFNV_CDMA_BC1_FBRX_GAIN_FREQ_I;
    break;

  case   SYS_BAND_BC6:
    *nv_item = RFNV_CDMA_BC6_FBRX_GAIN_FREQ_I;
    break;

  case   SYS_BAND_BC10:
    *nv_item = RFNV_CDMA_BC10_FBRX_GAIN_FREQ_I;
    break;

  case   SYS_BAND_BC15:
    *nv_item = RFNV_CDMA_BC15_FBRX_GAIN_FREQ_I;
    break;

  default:

    *nv_item = RFNV_CDMA_BC0_FBRX_GAIN_FREQ_I;
    ret_value = FALSE;
  }

  return ret_value;
}
#endif


// Commented out for 9x35 bringup
#if 0
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interpolate FBRX gain v.s. channel based on FBRX gain NV

  @details

  @return
  None
*/
void ftm_1x_calculate_fbrx_gain
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
  uint32 channel_freq;
  sys_channel_type band_chan;
  sys_band_class_e_type band;

  band_chan = rfm_1x_get_band_chan(RFM_DEVICE_0);

  band = band_chan.band;

  gain_idx = (int16) gain_stage;

  ch_size = fbrx_gain_nv_data.fbrx_gain_chan_size;

  channel_freq = rf_cdma_get_tx_carrier_freq(band, channel);


  if (ch_size == 0) {

    FTM_MSG(FTM_ERROR,"ftm_1x_calculate_fbrx_gain: NV has 0 channel data, error" );

    return;
  }

  if (ch_size == 1) {

    /* only has one data point */

    *fbrx_gain = (float) fbrx_gain_nv_data.fbrx_gain[0][gain_idx].gain;

    FTM_MSG_1(FTM_HIGH,"ftm_1x_calculate_fbrx_gain: NV has 1 channel data, FBRX gain = %d",  (int16) *fbrx_gain);

    return;

  } else
  {
    /* Has more than one data point */

 /* Take care of the extreme cases*/

    if (channel_freq<=rf_cdma_get_tx_carrier_freq(band, fbrx_gain_nv_data.fbrx_gain_chan[0]))
    {
      *fbrx_gain = (float)fbrx_gain_nv_data.fbrx_gain[0][gain_idx].gain;
      FTM_MSG_3(FTM_HIGH,"ftm_1x_calculate_fbrx_gain: NV has %d channel data, channel %d <channel[0], FBRX gain = %d",
                ch_size, channel, (int16) *fbrx_gain);
      return;
    }

    if (channel_freq>=rf_cdma_get_tx_carrier_freq(band, fbrx_gain_nv_data.fbrx_gain_chan[ch_size-1]))
    {
      *fbrx_gain = (float)fbrx_gain_nv_data.fbrx_gain[ch_size-1][gain_idx].gain;
      FTM_MSG_3(FTM_HIGH,"ftm_1x_calculate_fbrx_gain: NV has %d channel data, channel %d >channel[max], FBRX gain = %d",
                ch_size, channel, (int16) *fbrx_gain);
      return;
    }

    /* Determine location of the current channel in the list */

    ch_idx = 0;

    for (idx=0; idx<ch_size; idx++)
    {
      if (channel_freq <= rf_cdma_get_tx_carrier_freq(band,fbrx_gain_nv_data.fbrx_gain_chan[idx]))
      {
        ch_idx = idx;
        break;
      }
    }

    FTM_MSG_3(FTM_HIGH,"ftm_1x_calculate_fbrx_gain: channel[%d]<channel = %d <channel[%d]",
              idx-1, channel, idx);

    /* linear intepolation */

    x0 = (double) rf_cdma_get_tx_carrier_freq(band,fbrx_gain_nv_data.fbrx_gain_chan[ch_idx-1]);

    x1 = (double) rf_cdma_get_tx_carrier_freq(band,fbrx_gain_nv_data.fbrx_gain_chan[ch_idx]);

    y0 = (double) fbrx_gain_nv_data.fbrx_gain[ch_idx-1][gain_idx].gain;

    y1 = (double) fbrx_gain_nv_data.fbrx_gain[ch_idx][gain_idx].gain;

    y = y0 + (y1 - y0)*((double)channel_freq-x0)/(x1-x0);

    *fbrx_gain = (float) y;

    FTM_MSG_1(FTM_HIGH,"ftm_1x_calculate_fbrx_gain: interpolate, FBRX gain = %d",
                (int16) *fbrx_gain);

  }

  return;

}
#endif


// Commented out for 9x35 bringup
#if 0
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process FBRX gain NV, inteporlate gain v.s. freq, and calculate power dBm
  at antenna for a given band/channel RMS power (Rx AGC based)

  @details

  @return
  None
*/
void ftm_1x_calculate_power_agc
(
   int16 rx_agc,
   sys_band_class_e_type band,
   uint16 channel,
   ftm_common_selftest_fbrx_attn_type gain_stage,
   int16 *pwr_ant
)
{
   rfnv_item_id_enum_type nv_id;
   rfcommon_nv_fbrx_gain_vs_freq_type fbrx_gain_nv_data;
   float fbrx_gain;
   boolean nv_valid;

   /* determine the correct NV item location */
   nv_valid =  ftm_1x_obtain_fbrx_gain_nv_item(band, &nv_id);

   if (nv_valid)
   {

      FTM_MSG_4(FTM_HIGH, "ftm_1x_calculate_power_agc: NV id =%d, channel = %d, band = %d, rx_agc=%d",
                nv_id, channel, band, rx_agc);

      /* read from the correct NV and populate the data structure for the current band from the NV*/
      rfcommon_nv_load_fbrx_gain_from_nv
         (&fbrx_gain_nv_data, nv_id,
          NULL, 0, NULL);

      /* interpolate the FBRX cal data across freq to get the calculated gain for the current channel*/
      ftm_1x_calculate_fbrx_gain(fbrx_gain_nv_data, channel, gain_stage, &fbrx_gain);

      /* power at ant is in dBm*10 */
      *pwr_ant = (rx_agc - (int16)fbrx_gain );

      FTM_MSG_1(FTM_HIGH, "ftm_1x_calculate_power_agc: using interpolated FBRX gain NV, calculated pwr_ant = %d",
                 * pwr_ant);
   }
   else
   {
      /* band invalid, no NV stored, use default to bail out */
      *pwr_ant = rx_agc;

      FTM_MSG_1(FTM_HIGH, "ftm_1x_calculate_power_agc: no valid NV for the band, using default FBRX gain pwr_ant = %d",
                 * pwr_ant);
   }
}
#endif

// Commented out for 9x35 bringup
#if 0
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Calculate power dBm at antenna (Rx AGC based)

  @details

  @return
  power at antenna
*/

int16 ftm_1x_ant_pwr_agc
(
   rfm_device_enum_type device,
   ftm_common_selftest_fbrx_attn_type fbrx_gain_state,
   int16 rx_agc
)
{
  sys_band_class_e_type band;
  uint16 channel;
  sys_channel_type band_chan;
  int16 ant_pwr;

  band_chan = rfm_1x_get_band_chan(device);

  band = band_chan.band;

  channel = (uint16) band_chan.chan_num;

  FTM_MSG_3(FTM_HIGH,"ftm_1x_ant_pwr_agc: band = %d, channel = %d, gain_stage = %d",
          band, channel, fbrx_gain_state);

  ftm_1x_calculate_power_agc(rx_agc, band, channel,  fbrx_gain_state, &ant_pwr);

  FTM_MSG_1(FTM_HIGH,"ftm_1x_ant_pwr_agc return value: pwr_ant = %d (dBm * 10)",
            ant_pwr);

  return ant_pwr;
}
#endif


// Commented out for 9x35 bringup
#if 0
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process FBRX gain NV, inteporlate gain v.s. freq, and calculate power dBm
  at antenna for a given band/channel RMS power

  @details

  @return
  None
*/
void ftm_1x_calculate_power
(
   uint32 pwr_rms,
   sys_band_class_e_type band,
   uint16 channel,
   ftm_common_selftest_fbrx_attn_type gain_stage,
   int16 *pwr_ant
)
{

   rfnv_item_id_enum_type nv_id;
   rfcommon_nv_fbrx_gain_vs_freq_type fbrx_gain_nv_data;
   double nbee_rms_offset = 91.4;
   float fbrx_gain;
   boolean nv_valid;

   if (pwr_ant != NULL) {

     if (rfm_get_calibration_state())
     {
       /* return raw IQ RMS in dB100 for caliration */
       *pwr_ant = (int16)FTM_SELFTEST_ROUND(2000 * log10(pwr_rms));
       FTM_MSG_1(FTM_HIGH, "ftm_1x_calculate_power: calibrating, Raw IQ Rx RMS = %d",
                  * pwr_ant);
       return;
     }

     /* determine the correct NV item location */

     nv_valid =  ftm_1x_obtain_fbrx_gain_nv_item(band, &nv_id);

     if (nv_valid)
     {

        FTM_MSG_4(FTM_HIGH, "ftm_1x_calculate_power: NV id =%d, channel = %d, band = %d, 200log10(power_rms)=%d",
                  nv_id, channel, band, (int16)(200*log10(pwr_rms)));


        /* read from the correct NV and populate the data structure for the current band from the NV*/

        rfcommon_nv_load_fbrx_gain_from_nv
           (&fbrx_gain_nv_data, nv_id,
            NULL, 0, NULL);


        /* determine the NBEE and RMS Power offset */

        /* This offset is applicable when using Rx AGC to do FBRx Cal
           And Self Test Measurement with IQ Cap RMS */
        //nbee_rms_offset = 91.4;
        /* Make it 0 when FBRx Cal is done with IQ Cap RMS and
           Self Test Measurement is done with IQ Cap RMS */
        nbee_rms_offset = 0.0;

        /* interpolate the FBRX cal data across freq to get the calculated gain for the current channel*/

        ftm_1x_calculate_fbrx_gain(fbrx_gain_nv_data, channel, gain_stage, &fbrx_gain);

        /* power at ant is in dBm*100 */
        *pwr_ant = (int16)FTM_SELFTEST_ROUND(2000 * log10(pwr_rms) - (fbrx_gain + nbee_rms_offset*100));

        FTM_MSG_1(FTM_HIGH, "ftm_1x_calculate_power: using interpolated FBRX gain NV, calculated pwr_ant = %d",
                   * pwr_ant);
     }
     else
     {

        /* band invalid, no NV stored, use default to bail out */
        *pwr_ant = (int16)FTM_SELFTEST_ROUND((20 * log10(pwr_rms) - nbee_rms_offset)*100);

        FTM_MSG_1(FTM_HIGH, "ftm_1x_calculate_power: no valid NV for the band, using default FBRX gain pwr_ant = %d",
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

int16 ftm_1x_ant_pwr
(
   uint32 pwr_rms
)
{
  sys_band_class_e_type band;
  uint16 channel;
  sys_channel_type band_chan;
  ftm_common_selftest_fbrx_attn_type gain_stage;
  int16 ant_pwr;

  band_chan = rfm_1x_get_band_chan(RFM_DEVICE_0);

  band = band_chan.band;

  channel = (uint16) band_chan.chan_num;


  gain_stage = ftm_common_get_coupler_wtr_fb_path_state();

  FTM_MSG_3(FTM_HIGH,"ftm_1x_ant_pwr: band = %d, channel = %d, gain_stage = %d",
          band, channel, gain_stage);

  ftm_1x_calculate_power(pwr_rms, band, channel,  gain_stage, &ant_pwr);

  FTM_MSG_1(FTM_HIGH,"ftm_1x_ant_pwr return value: pwr_ant = %d (dBm * 10)",
            ant_pwr);

  return ant_pwr;
}
#endif

// Commented out for 9x35 bringup
#if 0
static boolean ftm_1x_fbrx_config_trig_rx_agc(rfm_device_enum_type device_fb ,
                                       rfm_lna_gain_state_type fb_lna_state,
                                       rfm_cdma_power_mode_type pwr_mode)
{
  boolean ret_val = TRUE;
  /* Set LNA Gain State */
  ret_val &= rfm_1x_ftm_set_rx_gain_state( device_fb, TRUE, fb_lna_state );
  if (ret_val == FALSE)
  {
    FTM_MSG_2(FTM_ERROR,"ftm_1x_fbrx_config_trig_rx_agc: failed to set fb rx lna state for dev %d lna_state %d", device_fb, fb_lna_state);
  }
  /* Set LNA gain to 0 */
  ret_val &= rfm_1x_ftm_set_rx_gain( device_fb, fb_lna_state, 0 ) ;
  if (ret_val == FALSE)
  {
    FTM_MSG_2(FTM_ERROR,"ftm_1x_fbrx_config_trig_rx_agc: failed to set fb rx lna gain for dev %d lna_state %d to 0", device_fb, fb_lna_state);
  }
  /* Override power mode to 0 */
  ret_val &= rfm_cdma_ftm_power_mode_override( device_fb, pwr_mode );
  if (ret_val == FALSE)
  {
    FTM_MSG_2(FTM_ERROR,"ftm_1x_fbrx_config_trig_rx_agc: failed to override pwr mode for dev %d pwr_mod %d", device_fb, pwr_mode);
  }
  return ret_val;
}
#endif

// Commented out for 9x35 bringup
#if 0
static boolean ftm_1x_fbrx_read_rx_agc(rfm_device_enum_type device_fb, int16 *rxagcval)
{
  boolean ret_val = TRUE;
  rfm_cdma_ftm_rx_agc_data_type rx_agc_data;
  ret_val &= rfm_1x_ftm_get_rx_agc_data( device_fb, &rx_agc_data,
                           RFM_CDMA_AGC_FORMAT__RAW );
  if (rxagcval == NULL)
  {
    FTM_MSG(FTM_ERROR, "ftm_1x_fbrx_read_rx_agc: NULL pointer passed in for output");
    return FALSE;
  }

  *rxagcval = (rx_agc_data.rx_agc[RFM_CDMA_CARRIER_0])/64;
  return ret_val;
}
#endif

// Commented out for 9x35 bringup
#if 0
static boolean ftm_1x_fbrx_avg_rx_agc_val
(
   rfm_device_enum_type device,
   rfm_device_enum_type device_fb,
   uint16 num_averages,
   int16 *avg_rxagcval,
   ftm_common_selftest_fbrx_attn_type *fbrx_gain_state
)
{
   boolean ret_val = TRUE;
   uint16 lp_var;
   int16 rxagcval = 0xFFFF;
   int32 rxagc_accum = 0;
   ftm_cdma_data_status_type dev_state;  /* Var to hold current device state */
   rfm_lna_gain_state_type fb_lna_state = RFM_LNA_GAIN_STATE_0;
   rfm_cdma_power_mode_type pwr_mode = RFM_CDMA_POWER_MODE_NORMAL;
   rfc_cmn_properties_type *rfc_cmn_properties_ptr = NULL;

   if (avg_rxagcval == NULL || fbrx_gain_state == NULL)
   {
     FTM_MSG(FTM_ERROR, "ftm_1x_fbrx_avg_rx_agc_val: NULL pointer for output avg_rxagcval or fbrx_gain_state");
     return FALSE;
   }

   (void)ftm_cdma_data_get_device_status( device, &dev_state );

   ret_val &= ftm_1x_fbrx_set_secondary_diversity(device, device_fb, TRUE);

   rfc_get_cmn_device_properties(&rfc_cmn_properties_ptr);
   if (rfc_cmn_properties_ptr == NULL)
   {
     FTM_MSG( FTM_ERROR, "rfc_cmn_properties pointer is NULL" );
     *fbrx_gain_state = FTM_COMMON_SELFTEST_FBRX_GAIN_1; /* Choose High attn state by default*/
   }
   else
   {
     rfm_fb_attenuation_state_type fb_attenuation_state =
       rf_cdma_convert_fb_attenuation_state( rfc_cmn_properties_ptr->rfc_attenuation_state );

   ret_val &= rfm_cdma_ftm_config_fb_transceiver
                                              (
                                               device,
                                               device_fb,
                                               RFM_1X_MODE,
                                               dev_state.curr_band,
                                                 dev_state.curr_chan,
                                                 fb_attenuation_state );

   if (ret_val == FALSE)
   {
     FTM_MSG_2(FTM_ERROR,"ftm_1x_fbrx_rx_agc_val: rfm_cdma_ftm_config_fb_transceiver failed for dev %d dev_fb %d", device, device_fb);
   }

     if (rfc_cmn_properties_ptr->rfc_attenuation_state > RFC_ATTENUATION_STATE_LOW)
     {
       *fbrx_gain_state = FTM_COMMON_SELFTEST_FBRX_GAIN_1; /* Mid/High attn */
     }
     else
     {
       *fbrx_gain_state = FTM_COMMON_SELFTEST_FBRX_GAIN_0; /* Low attn */
     }
   }

   ret_val &= ftm_1x_fbrx_config_trig_rx_agc(device_fb, fb_lna_state, pwr_mode );

   for (lp_var = 0; lp_var < num_averages; lp_var++)
   {
     DALSYS_BusyWait(FBRX_AGC_MEAS_WAIT_TIME_US);

     ret_val &= ftm_1x_fbrx_read_rx_agc(device_fb, &rxagcval);
     FTM_MSG_2(FTM_HIGH, "ftm_1x_fbrx_avg_rx_agc_val: reading #%d = %d", lp_var, rxagcval);

     if (rxagcval > 32767)
     {
       rxagcval = rxagcval - 65536;
     }
     rxagc_accum += rxagcval;

     if (lp_var == (num_averages-1))
     {
       /* Average Out the Accumulated AGC Read value */
       if( rxagc_accum > 0 ) /* For proper rounding */
       {
         *avg_rxagcval =
              (int16)((rxagc_accum + num_averages/2) /
              num_averages);
       } /* if( agc_acc_val > 0 ) */
       else
       {
         *avg_rxagcval =
              (int16)((rxagc_accum - num_averages/2) /
              num_averages );
       } /* if ! ( agc_acc_val > 0 ) */
     }
   }
   ret_val &= rfm_cdma_ftm_deconfig_fb_transceiver( device,
                                                    device_fb,
                                                    RFM_1X_MODE );
   if (ret_val == FALSE)
   {
     FTM_MSG_2(FTM_ERROR,"ftm_1x_fbrx_rx_agc_val: rfm_cdma_ftm_deconfig_fb_transceiver failed for dev %d dev_fb %d", device, device_fb);
   }

   ret_val &= ftm_1x_fbrx_set_secondary_diversity(device, device_fb, FALSE);

   return ret_val;
}
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize flags and other parameters for self test

  @details

  @return
  1 for error, 0 else
*/
uint8 ftm_1x_selftest_init
(
  ftm_1x_selftest_meas_params_struct *params,
  ftm_rf_cdma_fbrx_iq_acquired_samples_data_type *ftm_rf_fbrx_iq_acquired_samples_data_ptr,
  ftm_rf_cdma_payload_tx_params_type **ftm_rf_cdma_payload_tx_params,
  ftm_rf_fbrx_meas_tx_params_rsp_pkt_type *ftm_rf_fbrx_meas_tx_params_rsp_pkt,
  uint8 *ftm_selftest_pwr_only,
  ftm_rsp_pkt_type *rsp_pkt_ptr
)
{

  int payload_size_count = 0;

  // if (!(rfm_get_calibration_state())) {

  //    if not in calibration mode, obtain and set overral FB path gain stage
  //   ftm_1x_obtain_fbrx_gain_stage();

  //   /* otherwise use the settings set during cal */
  // }

  *ftm_rf_cdma_payload_tx_params = &(ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results);

  // flags->enable_tx_pwr_meas = (params->tx_measurement_config & FTM_FBRX_MEASURE_TX_POWER)? TRUE: FALSE;
  // flags->enable_evm_meas    = (params->tx_measurement_config & FTM_FBRX_MEASURE_EVM)? TRUE: FALSE;
  // flags->enable_aclr_meas   = (params->tx_measurement_config & FTM_FBRX_MEASURE_ACLR)? TRUE: FALSE;
  // flags->enable_vswr_meas   = (params->tx_measurement_config & FTM_FBRX_MEASURE_VSWR)? TRUE: FALSE;
  params->enable_tx_pwr_meas = (params->tx_measurement_config & FTM_FBRX_MEASURE_TX_POWER)? TRUE: FALSE;
  params->enable_aclr_meas   = (params->tx_measurement_config & FTM_FBRX_MEASURE_ACLR)? TRUE: FALSE;
  params->enable_evm_meas    = (params->tx_measurement_config & FTM_FBRX_MEASURE_EVM)? TRUE: FALSE;
  params->enable_vswr_meas = (params->tx_measurement_config & FTM_FBRX_MEAS_VSWR)? TRUE: FALSE;
  params->enable_vswr_meas_v2 = (params->tx_measurement_config & FTM_FBRX_MEAS_VSWR_V2)? TRUE: FALSE;

  // if (((flags->enable_tx_pwr_meas) || (flags->enable_vswr_meas)) &&
  //    ((!flags->enable_evm_meas) && (!flags->enable_aclr_meas)) ) {

  //   *ftm_selftest_pwr_only = TRUE;
  // }
  // else
  // {
  //   *ftm_selftest_pwr_only = FALSE;
  // }

  // FTM_MSG_1( FTM_HIGH, "1x self_test TX or VSWR only =  %d ", *ftm_selftest_pwr_only );


  /* print some debug */
  FTM_MSG_3(FTM_MED, "Inputs to Meas Tx params command -> num_averages : %d : capture_offset : %d : tx_measurement_config : %x",
            params->num_averages,
            params->capture_offset,
            params->tx_measurement_config);


  /***************** Fill up RSP packet with standard data *****************/
  // Fill in the constant fields of response packet. Sample buffer will be filled after sanity check.
  ftm_rf_fbrx_meas_tx_params_rsp_pkt->cmd_code = 75;
  ftm_rf_fbrx_meas_tx_params_rsp_pkt->subsys_id = FTM;
  ftm_rf_fbrx_meas_tx_params_rsp_pkt->subsys_cmd_code = (params->device == RFM_DEVICE_0) ? FTM_1X_C0_C : FTM_1X_C2_C;
  ftm_rf_fbrx_meas_tx_params_rsp_pkt->ftm_rf_cmd = FTM_RF_MEAS_TX_PARAMS;

  // Set status to success for now and update later
  ftm_rf_fbrx_meas_tx_params_rsp_pkt->ftm_error_code = FTM_FBRX_TEST_SUCCESS;
  ftm_rf_fbrx_meas_tx_params_rsp_pkt->num_averages = params->num_averages;
  ftm_rf_fbrx_meas_tx_params_rsp_pkt->capture_offset = params->capture_offset;
  ftm_rf_fbrx_meas_tx_params_rsp_pkt->tx_measurement_config = params->tx_measurement_config;
  ftm_rf_fbrx_meas_tx_params_rsp_pkt->payload_size = payload_size_count;

  // Fill in the constant fields of response packet.Size of the packet will be based on the requested sample size.
  rsp_pkt_ptr->cmd = FTM_RSP_DO_NEW_PKT;
  rsp_pkt_ptr->pkt_payload = (void *)(ftm_rf_fbrx_meas_tx_params_rsp_pkt);
  rsp_pkt_ptr->delete_payload = FALSE;
  rsp_pkt_ptr->pkt_len = FTM_RF_MEAS_TX_PARAMS_HDR_SIZE;


  /***********************************************************************/
  // Add code to enable FBRx path, measure IQ samples and process the sample
  // to compute Tx measurements.
  // Also update ftm_error_code as necessary.
  /***********************************************************************/

  if( ftm_get_current_state(params->device) != FTM_STATE_1X)
  {
    ftm_rf_fbrx_meas_tx_params_rsp_pkt->ftm_error_code = FTM_FBRX_TEST_GENERAL_FAILURE;
    FTM_MSG_1(FTM_HIGH, "FTM Mode is %d", ftm_get_current_state(params->device));
    FTM_MSG(FTM_ERROR, "FTM Mode is not 1x, bailing out");
      return 1;
  }

  /* Start with data_available to be FALSE and change it to TRUE when data is available */
  ftm_rf_fbrx_iq_acquired_samples_data_ptr->data_available = FALSE;

  return 0;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize sample buffer

  @details

  @return
  None
*/
uint8 ftm_1x_selftest_buffer_setup
(
  const uint16 fft_size,
  const rfcom_lte_bw_type lte_bw,

  uint32 *sample_buffer_size,
  int32 **sample_buffer,
  uint16 *pcfl_data_size,
  ftm_rf_fbrx_meas_tx_params_rsp_pkt_type *ftm_rf_fbrx_meas_tx_params_rsp_pkt
)
{
  /* Buffer for samples and FFT - 32 bit I/32 bit Q*/
  *sample_buffer_size = (fft_size * 2 * 4);
  *sample_buffer = (int32 *)ftm_malloc(*sample_buffer_size);

  /* If pointer is NULL even after allocating memory, then declare it as failure*/
  if (*sample_buffer == NULL)
  {
     ftm_rf_fbrx_meas_tx_params_rsp_pkt->ftm_error_code = FTM_FBRX_TEST_MEM_ALLOCATION_FAILURE;
     FTM_MSG(FTM_ERROR, "Unable to allocate memory for Rx - IQ buffer");
     return 1;
  }

  /* init FFT - required before calling FFT API for ACLR*/
  ftm_common_init_fft_data(FTM_STATE_1X, lte_bw);

  *pcfl_data_size = (fft_size * 20) / 8;

  return 0;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Capture IQ points and run FFT over samples

  @details

  @return
  None
*/
void ftm_1x_selftest_capture_and_fft
(
  const ftm_1x_selftest_meas_params_struct params,
  const uint16 sample_capture_size,
  const uint16 pcfl_data_size,
  const uint16 fft_size,
  const uint32 sample_buffer_size,
  ftm_rf_cdma_fbrx_iq_acquired_samples_data_type *ftm_rf_fbrx_iq_acquired_samples_data_ptr,
  int32 *sample_buffer,
  uint32 *evm_value_acc,
  uint64 *rx_rms_mag_acc
)
{
  uint8 average_count = 0;
  boolean capture_done = FALSE;
  uint16 evm_value = 0;
  uint32 rx_rms_mag = 0;
  int32 *iq_buf_rx_ptr = NULL;
  uint8 fft_step = 0;
  uint32 samp_rate = -1;
  ftm_common_selftest_fbrxgs_info_type fbrx_gs_info;
  const rf_cdma_data_status_type *dev_status;
  uint32 capture_type = params.tx_measurement_config;
  boolean do_rxrms_once = FALSE;

  dev_status = rf_cdma_get_device_status ( RFM_DEVICE_0 );
  if ( dev_status != NULL )
  {
    fbrx_gs_info.override_enable = ftm_1x_obtain_fbrx_gain_stage(&(fbrx_gs_info.selftest_fbrx_gs));
    fbrx_gs_info.com_band.cdma_band = dev_status->curr_band;
    fbrx_gs_info.wtr_lte_bw = 0;
  }
  else
  {
    fbrx_gs_info.override_enable = FALSE;
  }

  // ftm_common_set_coupler_direction (COUPLER_DIRECTION_FORWARD);

  for (average_count = 0; average_count < params.num_averages; average_count++)
  {
     /* Trigger I/Q capture*/
     FTM_MSG(FTM_HIGH, "Selftest capture start");
     capture_done = ftm_common_fbrx_iq_capture_processing(
                                      FTM_RF_TECH_CDMA,
                                      1,
                                      capture_type,
                                      sample_capture_size,
                                      &(ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_rx),
                                      &(ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_tx),
                                      &evm_value,
                                      &rx_rms_mag,
                                      &samp_rate,
                                      &fbrx_gs_info);
     FTM_MSG(FTM_HIGH, "Selftest capture end");
     if (capture_done == TRUE)
     {
        FTM_MSG(FTM_HIGH, "req_samples");
        ftm_rf_fbrx_iq_acquired_samples_data_ptr->req_samples = sample_capture_size;
        FTM_MSG(FTM_HIGH, "data_available");
        ftm_rf_fbrx_iq_acquired_samples_data_ptr->data_available = TRUE;

        FTM_MSG_2( FTM_HIGH, "iq_buf_rx = %d, iq_buf_tx = %d",
                   ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_rx,
                   ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_tx);
        #if 0
        FTM_MSG(FTM_HIGH, "efs save");
        rfcommon_iq_efs_save(
        ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_tx,
        ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_rx,
        9216);
        #endif
     }

     FTM_MSG(FTM_HIGH, "Selftest fft start");
     if (params.enable_aclr_meas  && sample_buffer != NULL)
     {
       iq_buf_rx_ptr = ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_rx;

       for (fft_step = 0; fft_step < sample_capture_size / fft_size; fft_step++)
       {
         /*get FFT_SIZE samples from the Rx buffer*/
         memscpy((void *)sample_buffer, sample_buffer_size, (void *)iq_buf_rx_ptr, pcfl_data_size);

         /* increase the ptr to point to next Rx samples*/
         iq_buf_rx_ptr = iq_buf_rx_ptr + (pcfl_data_size / sizeof(int32));

         /* Parse the data*/
         ftm_common_parse_PCFL20_to_IQ(FTM_STATE_1X, sample_buffer, fft_size);

         /* take FFT of this data*/
         ftm_common_get_samples_fft_avg(fft_size, sample_buffer);
       }
     }
     FTM_MSG(FTM_HIGH, "Selftest fft end");

     if (params.enable_evm_meas)
     {
        // ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.evm_value = evm_value;     /* add logic to do avergaing here*/
        *evm_value_acc = *evm_value_acc + evm_value;
        FTM_MSG_3(FTM_HIGH, "ftm_1x_fbrx_meas_tx_params: EVM reading #%d :  %d, accum : %d ", average_count, evm_value, *evm_value_acc);
        // ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.tx_power  = rx_rms_mag;    /* FIXME: 1X specific? */
        *rx_rms_mag_acc = *rx_rms_mag_acc + rx_rms_mag;
        FTM_MSG_3(FTM_HIGH, "ftm_1x_fbrx_meas_tx_params: IQ RMS Reading #%d: RxRMS=%d, RxRMSAcc=%d", average_count, rx_rms_mag, *rx_rms_mag_acc);
        do_rxrms_once = TRUE;
     }

     if (params.enable_tx_pwr_meas)
     {
       if (do_rxrms_once == FALSE)
       {
         // ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.tx_power  = rx_rms_mag;    /* FIXME: 1X specific? */
         *rx_rms_mag_acc = *rx_rms_mag_acc + rx_rms_mag;
         FTM_MSG_3(FTM_HIGH, "ftm_1x_fbrx_meas_tx_params: IQ RMS Reading #%d: RxRMS=%d, RxRMSAcc=%d", average_count, rx_rms_mag, *rx_rms_mag_acc);
       }
    }
  }
}

// Commented out for 9x35 bringup
#if 0
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Calculate average rxagc

  @details

  @return
  None
*/
void ftm_1x_selftest_rxagc_avg
(
  ftm_1x_selftest_meas_params_struct params,
  rfcom_device_enum_type device_fb,
  int16 *rx_agc,
  boolean *rxagc_rd_return,
  ftm_common_selftest_fbrx_attn_type *fb_gain_state,
  ftm_rf_cdma_fbrx_iq_acquired_samples_data_type *ftm_rf_fbrx_iq_acquired_samples_data_ptr
)
{
  *rxagc_rd_return = ftm_1x_fbrx_avg_rx_agc_val(params.device, device_fb, params.num_averages, rx_agc, fb_gain_state );
  if (*rxagc_rd_return == TRUE)
  {
    FTM_MSG_2(FTM_HIGH, "ftm_1x_fbrx_meas_tx_params: device %d raw_rxagc_count: %d", device_fb, *rx_agc);
    ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.tx_power  = ftm_1x_ant_pwr_agc(device, *fb_gain_state, *rx_agc);
  }
  else
  {
    FTM_MSG_1(FTM_ERROR, "ftm_1x_fbrx_meas_tx_params: device %d raw_rxagc_count read FAIL", device_fb);
  }
}
#endif

// Commented out for 9x35 bringup
#if 0
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Calculate VSWR

  @details

  @return
  None
*/
void ftm_1x_selftest_calculate_vswr
(
  const ftm_1x_selftest_meas_params_struct params,
  const uint16 sample_capture_size,
  ftm_rf_cdma_fbrx_iq_acquired_samples_data_type *ftm_rf_fbrx_iq_acquired_samples_data_ptr,
  uint32 *rx_rms_mag_avg,
  uint64 *rx_rms_reverse_mag_acc,
  uint32 *rx_rms_reverse_mag_avg,
  uint8 *status
)
{
  boolean capture_done = FALSE;
  uint16 average_count = 0;
  uint16 evm_value = 0;
  uint32 rx_rms_reverse_mag = 0;

  ftm_common_set_coupler_direction(COUPLER_DIRECTION_REVERSE);
  for (average_count = 0; average_count < num_averages; average_count++)
  {

    /* Trigger I/Q capture*/
    capture_done = ftm_common_fbrx_iq_capture_processing(
                                    FTM_RF_TECH_CDMA,
                                    1,
                                    params.enable_vswr_meas,
                                    sample_capture_size,
                                    &(ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_rx),
                                    &(ftm_rf_fbrx_iq_acquired_samples_data_ptr->iq_buf_tx),
                                    &evm_value,
                                    &rx_rms_reverse_mag);
    *rx_rms_reverse_mag_acc = *rx_rms_reverse_mag_acc + rx_rms_reverse_mag;
    FTM_MSG_4(FTM_HIGH, "ftm_1x_fbrx_meas_tx_params: Forward Avg %d Reverse reading #%d = %d , Reverse Acc = %d",
                 *rx_rms_mag_avg,
                 average_count,
                 rx_rms_reverse_mag,
                 *rx_rms_reverse_mag_acc);
  }

  ftm_common_set_coupler_direction(COUPLER_DIRECTION_FORWARD);

  *rx_rms_reverse_mag_avg = (uint32)(*rx_rms_reverse_mag_acc / num_averages);
  FTM_MSG_3(FTM_HIGH, "ftm_1x_fbrx_meas_tx_params: Reverse IQ RMS Averaging :  %d / %d = %d",
               *rx_rms_reverse_mag_acc,
               num_averages,
               *rx_rms_reverse_mag_avg);

  if ( *rx_rms_mag_avg > *rx_rms_reverse_mag_avg )
  {
    ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.vswr_ratio_dB10 =
                 (200 * log10(*rx_rms_mag_avg)) - (200 * log10(*rx_rms_reverse_mag_avg));

    FTM_MSG_4(FTM_HIGH, "ftm_1x_fbrx_meas_tx_params: Reverse IQ RMS Averaging :  %d / %d = %d, VSWR(dB10) = %d",
                 *rx_rms_reverse_mag_acc,
                 num_averages,
                 *rx_rms_reverse_mag_avg,
                 ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.vswr_ratio_dB10);
  }
  else
  {
    ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.vswr_ratio_dB10 = 9999;
    *status = FTM_FBRX_TEST_GENERAL_FAILURE;
    FTM_MSG_2(FTM_ERROR, "ftm_1x_fbrx_meas_tx_params: Reverse IQ RMS Averaged %d > Forward IQ RMS Averaged %d, returning 9999",
                 *rx_rms_reverse_mag_avg,
                 *rx_rms_mag_avg);
  }

}
#endif


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Calculate ACLR

  @details

  @return
  None
*/
void ftm_1x_selftest_calculate_aclr
(
  const ftm_1x_selftest_meas_params_struct params,
  const rfm_cdma_band_class_type curr_1x_band,
  const uint16 fft_size,
  ftm_rf_cdma_fbrx_iq_acquired_samples_data_type *ftm_rf_fbrx_iq_acquired_samples_data_ptr,
  int32 *sample_buffer
)
{

  ftm_common_1x_aclr_result_struct_type ftm_common_1x_aclr_result;

  FTM_MSG_2(FTM_MED, "ftm_1x_fbrx_meas_tx_params: ftm_cdma_data_get_band_class for device %d returns %d", params.device, curr_1x_band);

  ftm_common_get_1x_aclr(fft_size, curr_1x_band, &ftm_common_1x_aclr_result);

  ftm_common_deinit_fft_data();

  ftm_free(sample_buffer);

  /* Copy ACLR values to FTM data structures*/
  #if 0
  ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.aclr_value[0] = ftm_common_1x_aclr_result.adj_neg;
  ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.aclr_value[1] = ftm_common_1x_aclr_result.adj_pos;
  ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.aclr_value[2] = ftm_common_1x_aclr_result.alt1_neg;
  ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.aclr_value[3] = ftm_common_1x_aclr_result.alt1_pos;
  ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.aclr_value[4] = ftm_common_1x_aclr_result.alt2_neg;
  ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.aclr_value[5] = ftm_common_1x_aclr_result.alt2_pos;
  #endif

  // Flip ACLR values because of Spectral Inversion in C2k, TODO: read NV and then decide to do this or not
  ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.aclr_value[0] = ftm_common_1x_aclr_result.adj_pos;
  ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.aclr_value[1] = ftm_common_1x_aclr_result.adj_neg;
  ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.aclr_value[2] = ftm_common_1x_aclr_result.alt1_pos;
  ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.aclr_value[3] = ftm_common_1x_aclr_result.alt1_neg;
  ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.aclr_value[4] = ftm_common_1x_aclr_result.alt2_pos;
  ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.aclr_value[5] = ftm_common_1x_aclr_result.alt2_neg;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Fill the response packet going back to caller function

  @details

  @return
  None
*/
void ftm_1x_selftest_populate_rsp_pkt
(
  const ftm_1x_selftest_meas_params_struct params,
  const ftm_rf_cdma_payload_tx_params_type *ftm_rf_cdma_payload_tx_params,
  const uint32 *iq_capture_frequency,
  const uint32 *max_samples_per_diag_packet_tx,
  const uint32 *max_samples_per_diag_packet_rx,

  ftm_rsp_pkt_type *rsp_pkt_ptr,
  ftm_rf_fbrx_meas_tx_params_rsp_pkt_type *ftm_rf_fbrx_meas_tx_params_rsp_pkt
)
{
  int payload_size_count = 0;

  // Adding payload secion of response packet.
  // Check if Tx Power measurement type is enabled and add Tx Power results to the response packet
  if (params.tx_measurement_config & FTM_FBRX_MEASURE_TX_POWER)
  {
     /*Populate Tx power data*/
     memscpy(&ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_cdma_payload_tx_params->tx_power),
            sizeof(ftm_rf_cdma_payload_tx_params->tx_power));
     payload_size_count += sizeof(ftm_rf_cdma_payload_tx_params->tx_power);
  }
  // Check if EVM measurement type is enabled and add EVM results to the response packet
  if (params.tx_measurement_config & FTM_FBRX_MEASURE_EVM)
  {
     /*Populate EVM data*/
     memscpy(&ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_cdma_payload_tx_params->evm_value),
            sizeof(ftm_rf_cdma_payload_tx_params->evm_value));
     payload_size_count += sizeof(ftm_rf_cdma_payload_tx_params->evm_value);
  }
  // Check if ACLR measurement type is enabled and add ACLR results to the response packet
  if (params.tx_measurement_config & FTM_FBRX_MEASURE_ACLR)
  {
     /*Populate ACLR data*/
     memscpy(&ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_cdma_payload_tx_params->aclr_value),
            sizeof(ftm_rf_cdma_payload_tx_params->aclr_value));
     payload_size_count += sizeof(ftm_rf_cdma_payload_tx_params->aclr_value);
  }
  if ((params.tx_measurement_config & FTM_FBRX_SAVE_TX_IQ_SAMPLES)
      || (params.tx_measurement_config & FTM_FBRX_SAVE_RX_IQ_SAMPLES))
  {

     /* Check if Tx IQ Capture measurement type is enabled and add Tx IQ capture results to the response packet*/
     if (params.tx_measurement_config & FTM_FBRX_SAVE_TX_IQ_SAMPLES)
     {
        /*Populate Tx IQ capture data format inforamtion*/
        ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count++] = (uint8)FTM_CDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_TX;
        ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count++] = (uint8)FTM_FBRX_SAMPLE_SIZE_9216;

        /* Todo Check for Tx BW and accordingly choose sampling frequency*/
        memscpy(&ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
               (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
               iq_capture_frequency,
               sizeof(iq_capture_frequency));
        payload_size_count += sizeof(iq_capture_frequency);

        memscpy(&ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
               (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
               max_samples_per_diag_packet_tx,
               sizeof(max_samples_per_diag_packet_tx));
        payload_size_count += sizeof(max_samples_per_diag_packet_tx);
     }
     // Check if Rx IQ Capture measurement type is enabled and add Rx IQ capture results to the response packet
     if (params.tx_measurement_config & FTM_FBRX_SAVE_RX_IQ_SAMPLES)
     {
        /*Populate Rx IQ capture data format inforamtion*/
        ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count++] = (uint8)FTM_CDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_RX;
        ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count++] = (uint8)FTM_FBRX_SAMPLE_SIZE_9216;

        memscpy(&ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
               (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
               &iq_capture_frequency,
               sizeof(iq_capture_frequency));
        payload_size_count += sizeof(iq_capture_frequency);

        memscpy(&ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
               (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
               max_samples_per_diag_packet_rx,
               sizeof(max_samples_per_diag_packet_rx));
        payload_size_count += sizeof(max_samples_per_diag_packet_rx);
     }
  }
  // Check if VSWR measurement type is enabled and append VSWR result
  if (params.tx_measurement_config & FTM_FBRX_MEAS_VSWR)
  {
     /*Populate ACLR data*/
     memscpy(&ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_cdma_payload_tx_params->vswr_ratio_dB10),
            sizeof(ftm_rf_cdma_payload_tx_params->vswr_ratio_dB10));
     payload_size_count += sizeof(ftm_rf_cdma_payload_tx_params->vswr_ratio_dB10);
  }

  if (params.tx_measurement_config & FTM_FBRX_MEAS_VSWR_V2)
  {
        /*Populate VSWR RL*/

     memscpy(&ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_cdma_payload_tx_params->vswr_v2_return_loss),
            sizeof(ftm_rf_cdma_payload_tx_params->vswr_v2_return_loss));

     payload_size_count += sizeof(ftm_rf_cdma_payload_tx_params->vswr_v2_return_loss);



        /*Populate VSWR magnitude*/

     memscpy(&ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_cdma_payload_tx_params->vswr_v2_mag),
            sizeof(ftm_rf_cdma_payload_tx_params->vswr_v2_mag));

     payload_size_count += sizeof(ftm_rf_cdma_payload_tx_params->vswr_v2_mag);


        /*Populate VSWR phase*/

     memscpy(&ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_cdma_payload_tx_params->vswr_v2_phase),
            sizeof(ftm_rf_cdma_payload_tx_params->vswr_v2_phase));

     payload_size_count += sizeof(ftm_rf_cdma_payload_tx_params->vswr_v2_phase);


        /*Populate VSWR fwd power */
     memscpy(&ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_cdma_payload_tx_params->vswr_v2_fwd_pwr),
            sizeof(ftm_rf_cdma_payload_tx_params->vswr_v2_fwd_pwr));

     payload_size_count += sizeof(ftm_rf_cdma_payload_tx_params->vswr_v2_fwd_pwr);


        /*Populate VSWR rev power */
     memscpy(&ftm_rf_fbrx_meas_tx_params_rsp_pkt->result[payload_size_count],
            (FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE - payload_size_count),
            &(ftm_rf_cdma_payload_tx_params->vswr_v2_rev_pwr),
            sizeof(ftm_rf_cdma_payload_tx_params->vswr_v2_rev_pwr));

     payload_size_count += sizeof(ftm_rf_cdma_payload_tx_params->vswr_v2_rev_pwr);
  
  }

  if (payload_size_count > FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE)
  {
     ftm_rf_fbrx_meas_tx_params_rsp_pkt->ftm_error_code = FTM_FBRX_TEST_EXCEEDS_PAYLOAD_SIZE_LIMIT;
  } else
  {
     rsp_pkt_ptr->pkt_len +=  payload_size_count;
     ftm_rf_fbrx_meas_tx_params_rsp_pkt->payload_size = payload_size_count;
  }

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Handler for selftest functionality

  @details

  @return
  None
*/
uint8 ftm_1x_selftest
(
  ftm_1x_selftest_meas_params_struct params,
  ftm_rsp_pkt_type *rsp_pkt_ptr,
  ftm_rf_cdma_fbrx_iq_acquired_samples_data_type *ftm_rf_fbrx_iq_acquired_samples_data_ptr,
  ftm_rf_fbrx_meas_tx_params_rsp_pkt_type *ftm_rf_fbrx_meas_tx_params_rsp_pkt,
  uint8 *ftm_selftest_pwr_only,
  boolean use_iq_rms_for_pwr_meas
)
{
   uint8 status = FTM_FBRX_TEST_SUCCESS;

   uint32 iq_capture_frequency = FTM_CDMA_FBRX_IQ_SAMPLE_FREQ;
   uint32 max_samples_per_diag_packet_tx = FTM_CDMA_FBRX_IQ_DIAG_MAX_SAMPLES_TX;
   uint32 max_samples_per_diag_packet_rx = FTM_CDMA_FBRX_IQ_DIAG_MAX_SAMPLES_RX;
   ftm_1x_selftest_meas_params_struct params_mod_for_vswr_pwr_ratio;
   ftm_1x_selftest_meas_params_struct params_mod_for_vswr_mag_phase;

   rfcom_lte_bw_type lte_bw = RFCOM_BW_LTE_INVALID;      /* This parameter is LTE specific and will be ignored in 1x case */

   uint32 sample_buffer_size = 0;
   int32 *sample_buffer = NULL;
   uint8 fbrx_gs;
   boolean revert_fbrx = FALSE;
   rfcommon_fbrx_error_type fbrx_error;
   lm_handle_type tx_handle;
   rfcommon_fbrx_tx_cfg_type fbrx_bw_mode;
   rfcom_band_type_u com_band;
   boolean vswr_from_rxrms = FALSE;

   //ftm_common_aclr_result_struct_type ftm_common_aclr_result;
   ftm_rf_cdma_payload_tx_params_type *ftm_rf_cdma_payload_tx_params;
   // rfcom_device_enum_type device_fb = RFM_DEVICE_1;
   // ftm_common_selftest_fbrx_attn_type fb_gain_state;

   uint32 evm_value_acc = 0;
   uint16 evm_value_avg;
   uint32 dummy_evm_val = 0;

   uint64 rx_rms_mag_acc=0;
   uint32  rx_rms_mag_avg = 0;
   uint64 rx_rms_mag_reverse_acc = 0;
   // uint32 rx_rms_reverse_mag_avg;

   uint16 pcfl_data_size = 0;
   uint16 sample_capture_size = 9216;
   uint16 fft_size = 1024;
   // boolean rxagc_rd_return = TRUE;
   // int16 rx_agc = 0xFFFF;

   rfm_cdma_band_class_type curr_1x_band;
   const rf_cdma_data_status_type *dev_status;
   // sys_channel_type band_chan;
   // uint16 curr_1x_chan;

   uint8 selftest_error = 0;
   uint8 aclr_setup_error;

   int16 pwr_est_db10;
   boolean pwr_meas_success = FALSE;

   FTM_MSG(FTM_HIGH, "Entered ftm_1x_selftest");
   selftest_error = ftm_1x_selftest_init(
                                &params,
                                ftm_rf_fbrx_iq_acquired_samples_data_ptr,
                                &ftm_rf_cdma_payload_tx_params,
                                ftm_rf_fbrx_meas_tx_params_rsp_pkt,
                                ftm_selftest_pwr_only,
                                rsp_pkt_ptr);
   if (selftest_error)
   {
      FTM_MSG(FTM_HIGH, "ftm_1x_selftest_init failed, returning");
      status = FTM_FBRX_TEST_GENERAL_FAILURE;
      return 1;
   }



   curr_1x_band = ftm_cdma_data_get_band_class(params.device);
   // band_chan = rfm_1x_get_band_chan(params.device);
   // curr_1x_chan = band_chan->chan_num;
   com_band.cdma_band = curr_1x_band;
   fbrx_bw_mode = RFCOMMON_FBRX_1X;
   dev_status = rf_cdma_get_device_status ( RFM_DEVICE_0 );
   if ( dev_status != NULL )
   {
     tx_handle = dev_status->txlm_handle;
   }
   else
   {
     FTM_MSG(FTM_ERROR, "Invalid device ptr!");
     return 1;
   }



  switch (curr_1x_band)
  {
  case RFM_CDMA_BC0:
  case RFM_CDMA_BC10:
    selftest_lowband = TRUE;
    FTM_MSG(FTM_MED, "Selftest: Low bands");
    break;
  default:
    FTM_MSG(FTM_MED, "Selftest: High/Mid bands");
    selftest_lowband = FALSE;
    break;
  }
   ftm_common_set_coupler_gain(0);

   ftm_common_set_coupler_direction(0);


   if (params.enable_tx_pwr_meas)
   {
     pwr_meas_success = ftm_common_fbrx_meas_ant_pwr( &pwr_est_db10, &(ftm_rf_cdma_payload_tx_params->tx_power), params.num_averages, &fbrx_gs );
     FTM_MSG_5(FTM_HIGH, "pwr_est_db10: %d, pwr_est_db100: %d, num_avg: %d, status: %d, fbrx_gs: %d", 
               pwr_est_db10,
               ftm_rf_cdma_payload_tx_params->tx_power,
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

  ftm_common_set_coupler_gain(0);
   if (params.enable_aclr_meas)
   {
     FTM_MSG(FTM_ERROR, "Buffer setting up");
      aclr_setup_error = ftm_1x_selftest_buffer_setup(
                                           fft_size,
                                           lte_bw,
                                           &sample_buffer_size,
                                           &sample_buffer,
                                           &pcfl_data_size,
                                           ftm_rf_fbrx_meas_tx_params_rsp_pkt);
      if (aclr_setup_error)
      {
         FTM_MSG(FTM_HIGH, "ftm_1x_selftest_buffer_setup failed, returning");
         // TODO: Update response packet to indicate error?
         ftm_common_deinit_fft_data();
         if (sample_buffer!=NULL)
         {
           ftm_free(sample_buffer);
         }
         status = FTM_FBRX_TEST_GENERAL_FAILURE;
         return 1;
      }
   }

   if (params.enable_aclr_meas || params.enable_evm_meas || params.enable_vswr_meas)
   {
     if (revert_fbrx == FALSE)
     {
       rfcommon_fbrx_mc_enable_fbrx(FALSE);
       DALSYS_BusyWait(6000);  
       revert_fbrx = TRUE; 
     }
     // This delay is introduced to ensure previous TxAGC update ran to completion and freed FBRx
     ftm_1x_selftest_capture_and_fft(
               params_mod_for_vswr_pwr_ratio,
               sample_capture_size,
               pcfl_data_size,
               fft_size,
               sample_buffer_size,
               ftm_rf_fbrx_iq_acquired_samples_data_ptr,
               sample_buffer,
               &evm_value_acc,
               &rx_rms_mag_acc);

   // /* get average rx agc measurement */
   // if ((params.enable_tx_pwr_meas|params.enable_vswr_meas) && !use_iq_rms_for_pwr_meas)
   // {
   //    ftm_1x_selftest_rxagc_avg(
   //                params,
   //                device_fb,
   //                &rx_agc,
   //                &rxagc_rd_return,
   //                &fb_gain_state,
   //                ftm_rf_fbrx_iq_acquired_samples_data_ptr);
   // }
   }

   #if 0
   if (params.enable_tx_pwr_meas)
   {
     rx_rms_mag_avg = (uint32)(rx_rms_mag_acc/params.num_averages);
     FTM_MSG_3(FTM_HIGH, "ftm_1x_fbrx_meas_tx_params: RMS Averaging :  %d / %d = %d", rx_rms_mag_acc, params.num_averages, rx_rms_mag_avg);
     // ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.tx_power  = ftm_1x_ant_pwr(rx_rms_mag_avg);
     ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.tx_power = rx_rms_mag_avg;
   }
   #endif

   if (params.enable_evm_meas)
   {
     evm_value_avg = (uint16)(evm_value_acc/params.num_averages);
     ftm_rf_fbrx_iq_acquired_samples_data_ptr->meas_results.evm_value = evm_value_avg;
     FTM_MSG_3(FTM_HIGH, "ftm_1x_fbrx_meas_tx_params: EVM Averaging :  %d / %d = %d", evm_value_acc, params.num_averages, evm_value_avg);
   }

   // /* If VSWR measuremnt is enabled change coupler direction and again measure the IQ RMS based power estimate*/
   // if(params.enable_vswr_meas)
   // {
   //    ftm_1x_selftest_calculate_vswr(
   //                params,
   //                sample_capture_size,
   //                ftm_rf_fbrx_iq_acquired_samples_data_ptr,
   //                &rx_rms_mag_avg,
   //                &rx_rms_reverse_mag_acc,
   //                &rx_rms_reverse_mag_avg,
   //                &status);
   // }

   if (params.enable_aclr_meas)
   {
      FTM_MSG(FTM_HIGH, "Entering ftm_1x_selftest_calculate_aclr");
      ftm_1x_selftest_calculate_aclr(
                  params,
                  curr_1x_band,
                  fft_size,
                  ftm_rf_fbrx_iq_acquired_samples_data_ptr,
                  sample_buffer);
      FTM_MSG(FTM_HIGH, "Left ftm_1x_selftest_calculate_aclr");
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
     ftm_1x_selftest_capture_and_fft(
               params_mod_for_vswr_pwr_ratio,
               sample_capture_size,
               pcfl_data_size,
               fft_size,
               sample_buffer_size,
               ftm_rf_fbrx_iq_acquired_samples_data_ptr,
               sample_buffer,
               &dummy_evm_val,
               &rx_rms_mag_reverse_acc);
     ftm_common_set_coupler_direction(0); //COUPLER_DIRECTION_FORWARD);
     if (rx_rms_mag_acc > rx_rms_mag_reverse_acc )
     {
       ftm_rf_cdma_payload_tx_params->vswr_ratio_dB10 = 
                   (200 * log10(rx_rms_mag_acc)) - (200 * log10(rx_rms_mag_reverse_acc));

       FTM_MSG_3(FTM_HIGH, "ftm_1x_selftest: Forward IQ RMS avg = %d, Reverse IQ RMS avg = %d, VSWR(dB10) = %d", 
                    rx_rms_mag_acc,
                    rx_rms_mag_reverse_acc, 
                    ftm_rf_cdma_payload_tx_params->vswr_ratio_dB10);
     }
     else
     {
       ftm_rf_cdma_payload_tx_params->vswr_ratio_dB10 = 9999;
       status = FTM_FBRX_TEST_GENERAL_FAILURE;
       FTM_MSG_2(FTM_ERROR, "ftm_1x_selftest: Reverse IQ RMS Averaged %d > Forward IQ RMS Averaged %d, returning 9999", 
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

     ftm_1x_selftest_capture_and_fft(
               params_mod_for_vswr_mag_phase,
               sample_capture_size,
               pcfl_data_size,
               fft_size,
               sample_buffer_size,
               ftm_rf_fbrx_iq_acquired_samples_data_ptr,
               sample_buffer,
               &dummy_evm_val,
               &rx_rms_mag_reverse_acc);

    // fill up the return packet structure with results
    
    ftm_rf_cdma_payload_tx_params->vswr_v2_mag = ftm_common_selftest_vswr_v2_results.vswr_v2_mag;
    ftm_rf_cdma_payload_tx_params->vswr_v2_phase = ftm_common_selftest_vswr_v2_results.vswr_v2_phase;
    ftm_rf_cdma_payload_tx_params->vswr_v2_return_loss = ftm_common_selftest_vswr_v2_results.vswr_v2_return_loss;
    ftm_rf_cdma_payload_tx_params->vswr_v2_fwd_pwr= ftm_common_selftest_vswr_v2_results.vswr_v2_fwd_pwr;
    ftm_rf_cdma_payload_tx_params->vswr_v2_rev_pwr= ftm_common_selftest_vswr_v2_results.vswr_v2_rev_pwr;  
  }

   if (revert_fbrx == TRUE)
   {
     fbrx_error = rfcommon_fbrx_mc_reconfigure_rxlm
                                     ( 
                                       RFM_1X_MODE, 
                                       fbrx_bw_mode,
                                       (rflm_handle_tx_t)tx_handle,
                                       com_band
                                     );
     if (fbrx_error != RFCOMMON_FBRX_SUCCESS)
     {
       FTM_MSG_4(FTM_ERROR, "Could not reconfigure FBRx LM with Tech %d FBRx_BW_Mode %d tx_handle %d 1x band %d", 
                 RFM_1X_MODE,
                 fbrx_bw_mode,
                 tx_handle,
                 com_band.cdma_band);
       status = FTM_FBRX_TEST_GENERAL_FAILURE;
     }
     else
     {
       FTM_MSG_4(FTM_HIGH, "Reconfigured FBRx LM with Tech %d FBRx_BW_Mode %d tx_handle %d 1x band %d", 
                 RFM_1X_MODE,
                 fbrx_bw_mode,
                 tx_handle,
                 com_band.cdma_band);
     }
     rfcommon_fbrx_mc_enable_fbrx(TRUE);
   }

   // update error code
   ftm_rf_fbrx_meas_tx_params_rsp_pkt->ftm_error_code = status;

   if (status == FTM_FBRX_TEST_SUCCESS)
   {
      FTM_MSG(FTM_HIGH, "Entering ftm_1x_selftest_populate_rsp_pkt");
      ftm_1x_selftest_populate_rsp_pkt(
                  params,
                  ftm_rf_cdma_payload_tx_params,
                  &iq_capture_frequency,
                  &max_samples_per_diag_packet_tx,
                  &max_samples_per_diag_packet_rx,
                  rsp_pkt_ptr,
                  ftm_rf_fbrx_meas_tx_params_rsp_pkt);
      FTM_MSG(FTM_HIGH, "Leaving ftm_1x_selftest_populate_rsp_pkt");
   }

     ftm_common_set_coupler_direction (0);

   //    /* Set the coupler to forward direction */

   // ftm_common_set_coupler_direction (COUPLER_DIRECTION_REVERSE);

      /* Set the coupler to forward direction */

   // ftm_common_set_coupler_direction (COUPLER_DIRECTION_FORWARD);

  return 0;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Determine FBRX gain NV based on CDMA band

  @details

  @return 
  None
*/

static boolean ftm_1x_obtain_fbrx_gain_swtichpoint_nv_item
(
   rfm_cdma_band_class_type band,
   rfnv_item_id_enum_type *nv_item
)
{
  boolean ret_value = TRUE;
  switch (band)
  {

  case   RFM_CDMA_BC0:
    *nv_item = RFNV_CDMA_BC0_SELFTEST_FBRX_GAIN_SWITCH_POINT_I ;
    break;

  case   RFM_CDMA_BC1:
    *nv_item = RFNV_CDMA_BC1_SELFTEST_FBRX_GAIN_SWITCH_POINT_I ;
    break;

  case   RFM_CDMA_BC4:
    *nv_item = RFNV_CDMA_BC4_SELFTEST_FBRX_GAIN_SWITCH_POINT_I ;
    break;

  case   RFM_CDMA_BC5:
    *nv_item = RFNV_CDMA_BC5_SELFTEST_FBRX_GAIN_SWITCH_POINT_I ;
    break;

  case   RFM_CDMA_BC6:
    *nv_item = RFNV_CDMA_BC6_SELFTEST_FBRX_GAIN_SWITCH_POINT_I ;
    break;

  case   RFM_CDMA_BC10:
    *nv_item = RFNV_CDMA_BC10_SELFTEST_FBRX_GAIN_SWITCH_POINT_I ;
    break;

  case   RFM_CDMA_BC14:
    *nv_item = RFNV_CDMA_BC14_SELFTEST_FBRX_GAIN_SWITCH_POINT_I ;
    break;

  case   RFM_CDMA_BC15:
    *nv_item = RFNV_CDMA_BC15_SELFTEST_FBRX_GAIN_SWITCH_POINT_I ;
    break;

  default:
    *nv_item = RFNV_CDMA_BC0_SELFTEST_FBRX_GAIN_SWITCH_POINT_I ;
    ret_value = FALSE;
  }

  return ret_value;
}

static boolean ftm_1x_obtain_fbrx_gain_stage(uint8 * gain_stage)
{
  rfm_cdma_band_class_type band;
  rfnv_item_id_enum_type nv_id;
  boolean nv_valid;
  int8 gain_stage_iter;
  int16 txagc_val;
  boolean ret_val = TRUE;
  rfcommon_nv_selftest_fbrx_gain_switch_pt_type fbrx_gain_switchpoint_nv_data;
  const rf_cdma_data_status_type *dev_status;

  dev_status = rf_cdma_get_device_status ( RFM_DEVICE_0 );
  if ( dev_status != NULL )
  {
    band = dev_status->curr_band;
  }
  else
  {
    return FALSE;
  }

  txagc_val = ftm_1x_tx_power_dbm;

  /* determine the correct NV item location */
  nv_valid =  ftm_1x_obtain_fbrx_gain_swtichpoint_nv_item(band, &nv_id);

  if (nv_valid)
  {
    FTM_MSG_2(FTM_HIGH, "ftm_1x_obtain_fbrx_gain_stage: NV id =%d, band = %d",
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
    FTM_MSG(FTM_HIGH, "ftm_1x_obtain_fbrx_gain_stage: no valid NV for the band, using default gain stage");
    ret_val = FALSE;
  }

  FTM_MSG_2(FTM_HIGH, "txagc_val: %d, selected fbrx gain stage: %d", txagc_val, *gain_stage);

  return ret_val;

}

#endif