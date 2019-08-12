/*!
  @file
  rf_tdscdma_core_temp_comp.c

  @brief
  Provides TDSCDMA Temperature Compensation functionality

*/

/*===========================================================================

Copyright (c) 2011 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/core/src/rf_tdscdma_core_temp_comp.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/08/15   whz     Update AutoPin initial value only for retune
09/17/15   kma     Added auto pin support for Pin cal
09/15/15   whz     Save tempcomp Pin for AutoPin
06/04/15   whz     Added AutoPin support
03/17/15   whz     Fix TempComp on Jolokia
10/14/14   jz      Remove temp comp in rf_tdscdma_core_temp_comp_start()
09/17/14   kb      Clean up FEATURE_DIMEPM_MODEM from mainline
07/21/14   JJ      Use therm bins from each tech to get temp comp value
07/17/14   jhe     fbrx temp comp fix
07/02/14   kr/kai  Non-0dB Pin SW implementation to support -ve pin offsets values
07/02/14   ms      Disable therm read for DPM20 (temporary workaround for RSCP drop issue)
06/16/14   vs/kai  Non-0dB Pin SW implementation
05/27/14   jhe     Add txlim temp comp
05/04/14   jhe     KW fix
05/04/14   jhe     fbrx temp comp
03/20/14   jhe     temp comp update
03/10/14   jyu     Enabled temp read
08/21/13   jhe     Update to support DPD
08/02/12   nsh     interface change from "PATH" to "DEVICE"
04/09/12   ms      Temp comp timer. 
05/27/11   jhe     Initial version.

============================================================================*/

#include "rfa_variation.h"
#include "rf_tdscdma_core_temp_comp.h"
#include "rfcommon_nv_mm.h"
#include "msg.h"
#include "rf_tdscdma_mdsp.h"
#include "rf_tdscdma_core_txplim.h"
#include "rf_tdscdma_core_txlin.h"
#include "rf_tdscdma_core_util.h"
#include "rf_tdscdma_core_xpt.h"
#include "rfc_card.h"
#include "rfnv_tdscdma.h"
#include "pm.h"
#include "rf_tdscdma_mc.h"
#include "time_svc.h" 
#include "time_tod.h"
#include "rfm_internal.h"
#include "rf_tdscdma_core_txctl.h"
#include "rfc_card_tdscdma.h"
#include "rfdevice_trx_tdscdma_rx.h"
#include "rfcommon_fbrx_api.h"
#include "rfcommon_autopin_api.h"
#include "rflm_api_autopin_hw.h"
#include "stringl.h"

#define RF_TDSCDMA_CORE_THERM_MIN 0
#define RF_TDSCDMA_CORE_THERM_MAX 255
#define RF_TDSCDMA_CORE_TEMP_BIN_SIZ (RF_TDSCDMA_CORE_THERM_MAX / (NV_TEMP_TABLE_SIZ-1))

/*Absolute value macro*/
#define RF_TDSCDMA_CORE_TEMP_COMP_ABS(x)               (((x) < 0) ? -(x) : (x))

static rf_tdscdma_core_temp_comp_value_type rf_tdscdma_core_temp_comp_value =
{
  FALSE,
  /* The init value is roughly the raw reading at room temp for wtr */
  1234,
  110,
  0,
  0
};


/* Local functions */
uint8 rf_tdscdma_core_temp_comp_scale_therm(uint16 therm_raw_value, boolean save);
LOCAL int16 rf_tdscdma_core_temp_comp_calc_comp_val(int8 temp_table[], 
                                                    uint8 temp_index, 
                                                    uint8 temp_rem);




/* Callback pointer to the Temp compensation calculator */
static timer_type rf_tdscdma_core_temp_comp_clk_cb;

static boolean rf_tdscdma_core_temp_comp_started = FALSE;


/*  TDSCDMA Tx power compensation w.r.t temperature callback intervals */
static const uint32 RF_TDSCDMA_CORE_TEMP_COMP_REPEAT_INT = 10000;/* milliseconds*/ 
static const uint32 RF_TDSCDMA_CORE_TEMP_COMP_START_INT = 1000; /* milliseconds */
static void rf_tdscdma_core_temp_comp_vs_freq_get_comp(
                                      uint16 tx_chan,
                                      uint8 scaled_temperature_read,
                                      int16 pin_comp_offset[],
                                      int16 pout_comp_offset[] );

extern int16 rf_tdscdma_core_autopin_pin_comp_init[RFCOM_TXLIN_NUM_LINEARIZERS];

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the temp comp clock callback.
 
  @details

*/
void rf_tdscdma_core_temp_comp_init(void)
{
  /* define the temp comp call back */
  timer_def2( &rf_tdscdma_core_temp_comp_clk_cb, NULL );
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Starts the timer 

*/
void rf_tdscdma_core_temp_comp_start(void)
{
  /* times set such that the calculation happens 3 secs later so that it
  ** doesn't start running as soon as TX power is enabled because we don't
  ** know if it is a real call
  */
#if 0
  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH," rf_tdscdma_core_temp_comp_start Called", 0);

  if ( !rf_tdscdma_core_temp_comp_started )
  {
    timer_reg( &rf_tdscdma_core_temp_comp_clk_cb,
               (timer_t2_cb_type)rf_tdscdma_core_temp_comp_timer,
               (timer_cb_data_type)NULL,
               RF_TDSCDMA_CORE_TEMP_COMP_START_INT,
               RF_TDSCDMA_CORE_TEMP_COMP_REPEAT_INT);
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH,"timer trigger");
    rf_tdscdma_core_temp_comp_started = TRUE;
  }
#endif
  if ( !rf_tdscdma_core_temp_comp_started )
  {
    rf_tdscdma_mdsp_config_therm(TRUE, 2000);  //2000 subframes = 10s
    rf_tdscdma_core_temp_comp_started = TRUE;
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stops the timer
   
*/
void rf_tdscdma_core_temp_comp_stop(void)
{
#if 0
  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH," rf_tdscdma_core_temp_comp_stop Called", 0);

  if ( rf_tdscdma_core_temp_comp_started )
  {
    (void)timer_clr(&rf_tdscdma_core_temp_comp_clk_cb, T_NONE);
   // rf_tdscdma_core_do_temp_comp_calc = FALSE;
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH,"temp comp timer stopped");
    rf_tdscdma_core_temp_comp_started = FALSE;
  } 
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,
          " Trying to stop TDSCDMA Temp comp. calculator even though it is not started!!!",0);
  }
#endif

  if ( rf_tdscdma_core_temp_comp_started )
  {
    rf_tdscdma_core_temp_comp_started = FALSE;
    rf_tdscdma_mdsp_config_therm(FALSE, 2000);  //2000 subframes = 10s
  } 
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is the call back function and is called evey 10seconds. 

*/
/*lint -e{715} t_unused is unused - hence the name */
void rf_tdscdma_core_temp_comp_timer(int4 t_unused)
{
  /* Use SW read for temp read */
 
 boolean read_result;
 rfdevice_rxtx_common_class* cmn_device_ptr = NULL;
 uint16 therm_read = 0xFFFF;

 if (rf_tdscdma_mc_state.curr_pri_device != RFM_INVALID_DEVICE)
 {
   rfdevice_trx_tdscdma_tx** device_ptr = NULL;
   uint8 device_idx = 0;
      
   device_ptr = (rfdevice_trx_tdscdma_tx**) rfc_tdscdma_get_rf_device_obj(rf_tdscdma_mc_state.curr_pri_device, 
                                                                          RFC_CONFIG_TX, 
                                                                          RFCOM_BAND_TDSCDMA_B34, 
                                                                          RFDEVICE_TRANSCEIVER);

   if (device_ptr != NULL)
   {
      while (device_ptr[device_idx] != NULL) 
      {
        cmn_device_ptr = rfdevice_tdscdma_tx_get_common_device_pointer(device_ptr[device_idx]);

        device_idx++;
      }
   }
   else
   {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
           "rfc_tdscdma_get_rf_device_obj failure. Null pointer detected. band: %d", 
           RFCOM_BAND_TDSCDMA_B34);
   }

   if (cmn_device_ptr != NULL)
   {
     read_result = rfdevice_cmn_read_therm(cmn_device_ptr, &therm_read);

     if (read_result)
     {
       MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "Therm_read: %d, device: %d", therm_read, rf_tdscdma_mc_state.curr_pri_device);
       rf_tdscdma_mc_proc_therm_reading(0,            //uint16 num_ssbi
                                        NULL,         //uint8 raw_reading[]
                                        TRUE,         //boolean skip_calc
                                        therm_read);
     }
     else
     {
       MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed on getting therm_read for device: %d", rf_tdscdma_mc_state.curr_pri_device);
     }
   }
   else
   {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Can't get cmn_device_ptr for therm_read. device: %d", rf_tdscdma_mc_state.curr_pri_device);
   }   
 }
 
}


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Clear or set temperature compensation enable flag. This function shall be called
  in enter mode or wherever temp comp needs to be enabled

  @param flag Enable or Disable Temperature compensation

*/
void rf_tdscdma_core_temp_comp_enable(boolean flag)
{
  rf_tdscdma_core_temp_comp_value.temp_comp_enable_flag = flag;
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  return if temp comp is enabled

  @param none

*/
boolean rf_tdscdma_core_temp_comp_is_enabled(void)
{
  return rf_tdscdma_core_temp_comp_value.temp_comp_enable_flag;
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function processes the raw therm value from mdsp therm rsp msg.

  @details

*/
void rf_tdscdma_core_temp_comp_proc_therm_reading(uint16 therm_raw_value)
{

  if (rf_tdscdma_core_temp_comp_value.temp_comp_enable_flag == TRUE)
  {
    /* Update txlin according to current therm reading */
    rf_tdscdma_core_temp_comp_update_txlin(therm_raw_value,
                                           rf_tdscdma_mc_state.curr_chan_tx);
    /* Update txlim according to current therm reading */
    rf_tdscdma_core_temp_comp_update_txlim(therm_raw_value);
  }
  /* if temp comp not enabled, just save raw reading etc */
  else
  {
    (void)rf_tdscdma_core_temp_comp_scale_therm(therm_raw_value, TRUE);
  }
}


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function uses the raw therm read, gets pin and pout, and calls
  txlin_temp_comp_update to update temp comp values to the linearizer.

*/
void rf_tdscdma_core_temp_comp_update_txlin(uint16 therm_raw_value,
                                            uint16 tx_chan)
{
  uint8 temperature;
  int16 pin_comp_offset[RFCOM_TXLIN_NUM_LINEARIZERS];
  int16 pout_comp_offset[RFCOM_TXLIN_NUM_LINEARIZERS];
  int16 txlin_vs_temp[RF_TDSCDMA_NUM_TX_LINEARIZERS];
  rfcommon_fbrx_error_type fbrx_temp_comp_result;
  uint16 *tx_therm_bins = NULL;
  int16 i;

  /* scale thermistor */
  temperature = rf_tdscdma_core_temp_comp_scale_therm(therm_raw_value, TRUE);

  /* compute pin and pout from vs_temp_vs_freq NVs */
  rf_tdscdma_core_temp_comp_vs_freq_get_comp( tx_chan,
                                              temperature,
                                              pin_comp_offset,
                                              pout_comp_offset );

  /* calc txlin vs temp from NV */
  rf_tdscdma_core_temp_comp_calc_txlin_vs_temp(txlin_vs_temp);

  RF_MSG_4( RF_MED, "txlin_vs_temp values:%d,%d,%d,%d", 
            txlin_vs_temp[0],txlin_vs_temp[1],txlin_vs_temp[2],txlin_vs_temp[3]);

  /* add txlin_vs_temp to pout, to provide another knob for adjusting pout 
     note: txlin_vs_temp NV only has 4 pa states */
  for (i=0; i<RF_TDSCDMA_NUM_TX_LINEARIZERS; i++)
  {
    pout_comp_offset[i] += txlin_vs_temp[i];
  }
  
  /* apply the pin/pout comp values to the linearizer */
  if (!rfcommon_autopin_is_enabled(RFM_TDSCDMA_MODE) || !rflm_autopin_sanity_check(rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx))
  {
    rf_tdscdma_core_txlin_temp_comp_update(pin_comp_offset, pout_comp_offset);
  }

  /* apply temp comp for FBRx */
  /* check the flag once more in case mc_disable_tx happens during temp comp */
  if( rf_tdscdma_core_temp_comp_is_enabled() )
  {
    /* Query therm bins from tx temp comp NV */
    tx_therm_bins = rfnv_tdscdma_tx_tbl_ptr->lin_temp_comp_data.tx_therm_bins;
    /* Update fbrx temp comp with queried therm bins */
    if (tx_therm_bins != NULL)
    {  
      fbrx_temp_comp_result = rfcommon_fbrx_mc_update_temp_comp(rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx,
                                                                temperature,
                                                                tx_therm_bins);
      if (fbrx_temp_comp_result != RFCOMMON_FBRX_SUCCESS)
      {
        MSG_1( MSG_SSID_RF, MSG_LVL_ERROR,"FBRx Temp Comp failed", 0);
      }
    }
    else
    {
      MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,"FBRx Temp Comp failed: Therm bins is NULL!");
    }
  }
}


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details

*/
void rf_tdscdma_core_temp_comp_update_txlim(uint16 therm_raw_value)
{
  uint8 temperature;
  uint8 temp_index;
  uint8 temp_rem;
  int16 tx_lim_vs_temp_val = 0;
  int16 utran_tx_lim_vs_temp_offset_val = 0;

  /* scale thermistor */
  temperature = rf_tdscdma_core_temp_comp_scale_therm(therm_raw_value, TRUE);

  /* calculate temperature compensation table segment index */
  temp_index = temperature / RF_TDSCDMA_CORE_TEMP_BIN_SIZ;

  /* calculate segment remainder */
  temp_rem   = temperature % RF_TDSCDMA_CORE_TEMP_BIN_SIZ;

  rf_tdscdma_core_temp_comp_value.last_temp_comp_index = (uint8) temp_index;
  rf_tdscdma_core_temp_comp_value.last_temp_comp_rem   = (uint8) temp_rem;

  
  /* tx_lim_vs_temp is a delta value */
  tx_lim_vs_temp_val = rf_tdscdma_core_temp_comp_calc_comp_val( rfnv_tdscdma_tx_tbl_ptr->tx_limit_vs_temp,
                                                                temp_index,
                                                                temp_rem );

  /* there is no utran_tx_lim_vs_temp_offset NV. apply the same delta tx_lim_vs_temp_val to utran limit */
  utran_tx_lim_vs_temp_offset_val = tx_lim_vs_temp_val;

  rf_tdscdma_core_txplim_temp_val_update(tx_lim_vs_temp_val, utran_tx_lim_vs_temp_offset_val,
                                         temp_index,
                                         temp_rem);


}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function validates and scales the temperature value.
 
  @param save_flag  whether to save the raw value and the scaled value to the
  static data structure 
 
*/
uint8 rf_tdscdma_core_temp_comp_scale_therm(uint16 therm_raw_value, boolean save)
{
  uint16 therm_scaled_value = 0;
  uint16 therm_raw_range;

  /* sanity check */
  if ( therm_raw_value == 0xFFFF ) 
  {
    /* wrong raw reading, return last scaled value */
    return(rf_tdscdma_core_temp_comp_value.last_scaled_temp_value);
  }
  else if (therm_raw_value > rfcommon_nv_mm_tbl.rf_enh_therm_max)
  {
    therm_raw_value = (uint16)rfcommon_nv_mm_tbl.rf_enh_therm_max;
  }
  else if (therm_raw_value < rfcommon_nv_mm_tbl.rf_enh_therm_min)
  {
    therm_raw_value = (uint16)rfcommon_nv_mm_tbl.rf_enh_therm_min;
  }

  if (save)
  {
    /* save for logging etc */
    rf_tdscdma_core_temp_comp_value.last_raw_temp_value = therm_raw_value;
  }

  /* scale to 0-255 range */
  therm_raw_range = (uint16)(rfcommon_nv_mm_tbl.rf_enh_therm_max - rfcommon_nv_mm_tbl.rf_enh_therm_min);
  if(therm_raw_range != 0)
  {
    therm_scaled_value = (uint16)(((therm_raw_value - rfcommon_nv_mm_tbl.rf_enh_therm_min) * RF_TDSCDMA_CORE_THERM_MAX) / therm_raw_range);
    
    /* if it exceeds the max then return the max */
    therm_scaled_value = MIN(therm_scaled_value, RF_TDSCDMA_CORE_THERM_MAX);
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,"Raw Therm Range is NULL!", 0);
  }

  if (save)
  {
    rf_tdscdma_core_temp_comp_value.last_scaled_temp_value = (uint8)therm_scaled_value;
  }

  MSG_2(MSG_SSID_RF, MSG_LVL_HIGH, "TDSCDMA read therm, Raw:%d, Scaled:%d", therm_raw_value, therm_scaled_value);

  return (uint8)therm_scaled_value;
}


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function calculates temperature compensation value from temp_table[]
  by linear interpolation.

  @param temp_table[] Temperature compensation table
  @param temp_index Temperature table index
  @param temp_rem Remainder value

  @return int16 Interpolated temperature compensation value

*/
int16 rf_tdscdma_core_temp_comp_calc_comp_val( int8 temp_table[], 
                                               uint8 temp_index, 
                                               uint8 temp_rem )
{
  int16 delta;

  /* If the index is not the last entry, find the delta between
     table entries, scale it by the remainder divided by the
     temperature bin size, to get the delta between the Ith entry
     and the correct value.
  */
  if ( temp_index < (NV_TEMP_TABLE_SIZ-1) )
  {
    /* Calculate the difference between table entries */
    delta  = temp_table[ temp_index + 1 ] - temp_table[ temp_index ];

    /* Scale the difference by the remainder and divide
       it by the bin size, to get the correct difference
       between the Ith entry and the correct value.
       The result is rounded to reduce error to 1/2 LSB.
    */
    delta *= temp_rem;
    delta += RF_TDSCDMA_CORE_TEMP_BIN_SIZ/2;
    delta /= RF_TDSCDMA_CORE_TEMP_BIN_SIZ;

    /* return the interpolated value */
    return( temp_table[ temp_index ] + delta );
  }
  else
  {
    /* return max value */
    return( temp_table[ temp_index ] );
  }
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the thermistor related data for logging or other purposes.

*/
rf_tdscdma_core_temp_comp_value_type rf_tdscdma_core_temp_comp_get_therm_data(void)
{
  return(rf_tdscdma_core_temp_comp_value);
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  Obtain the Pin and Pout compensation values from the common linearizer
  compensation interface
 
  @details
  This API provides functionality to compute the current Pin and Pout
  temperature vs. frequency compensation offsets, based on data from
  static NV items LIN_VS_TEMP_VS_FREQ_PIN/POUT_I. For any error scenarios
  and for invalid PA states, the offsets are set to zero.

  @param scaled_temperature_read
  This is the current therm read, scaled to 0-255
 
  @param pin_comp_offset
  Array of int16 Pin values, populated as output from this function
 
  @param pout_comp_offset
  Array of int16 Pout values, populated as output from this function
 
*/
static void rf_tdscdma_core_temp_comp_vs_freq_get_comp(
                                      uint16 tx_chan,
                                      uint8 scaled_temperature_read,
                                      int16 pin_comp_offset[],
                                      int16 pout_comp_offset[] )
{
  /* Multi-lin NV data retrieval */
  tx_band_cal_type *tx_multilin_nv_cal_ptr = NULL;

  tx_lin_vs_temp_vs_freq_comp_param_type temp_vs_freq_params;
  uint8 pa_state;
  uint32 ref_lin_freq;
  //uint16 therm_bins_data[NV_TEMP_TABLE_SIZ];
  /* Pointer to the Txlin data */
  rf_tdscdma_core_txlin_record_type *curr_txlin_data;

  /* Skip temp comp offset computation in cal state and in Rx only state */
  if ( (IS_FTM_IN_TEST_MODE()&&(rfm_get_calibration_state() == TRUE)) )
  {
    for (pa_state = 0; pa_state < RFCOM_TXLIN_NUM_LINEARIZERS; pa_state++)
    {
      /* Set to 0 */
      pin_comp_offset[pa_state] = 0;
      pout_comp_offset[pa_state] = 0;
    }
    return;  
  }

  /* Initialize the therm bins to use equally spaced bins */
  //for (therm_bin = 0; therm_bin < NV_TEMP_TABLE_SIZ; therm_bin++)
  //{
  //  therm_bins_data[therm_bin] = therm_bin*RF_TDSCDMA_CORE_TEMP_BIN_SIZ;
  //}

  /* Get current Tx linearizer data */
  curr_txlin_data = rf_tdscdma_core_get_current_txlin_data();

  /* For the current band get the pointer to complete linearizer band cal data */
  tx_multilin_nv_cal_ptr = rf_tdscdma_core_txlin_get_band_data( 
                                     rf_tdscdma_core_util_get_band_from_uarfcn(tx_chan) );

  /* Null pointer check */
  if (curr_txlin_data == NULL || tx_multilin_nv_cal_ptr == NULL)
  {
    MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, 
           "rf_tdscdma_core_temp_comp_vs_freq_get_comp:"
           "Invalid TDSCDMA Tx Lin data", 0);
    for (pa_state = 0; pa_state < RFCOM_TXLIN_NUM_LINEARIZERS; pa_state++)
    {
      /* Set to 0 */
      pin_comp_offset[pa_state] = 0;
      pout_comp_offset[pa_state] = 0;
    }
    return;
  }

  /* Initialize input data for the function to compute offset */
  temp_vs_freq_params.freq_params.tx_freq = rf_tdscdma_core_util_get_freq_from_chan(tx_chan); 
  temp_vs_freq_params.freq_params.waveform_type = TX_LINEARIZER_VOICE;

  temp_vs_freq_params.temp_params.temp_read = (uint16)scaled_temperature_read;
  /* scale raw to 255 range, don't save the raw */
  temp_vs_freq_params.temp_params.ref_lin_temp_adc = 
        rf_tdscdma_core_temp_comp_scale_therm(tx_multilin_nv_cal_ptr->ref_lin_temp_adc, FALSE);

  temp_vs_freq_params.tx_multilin_nv_cal_ptr = 
                                    (tx_band_cal_type*)tx_multilin_nv_cal_ptr;

  for ( pa_state = 0; 
        pa_state < curr_txlin_data->num_valid_pa_state; 
        pa_state++ )
  {
    /* Provide current PA state */
    temp_vs_freq_params.pa_state = pa_state;
    /* Find ref linearizer frequency per PA state */
    rfcommon_core_txlin_get_ref_linearizer_frequency( 
                             (tx_band_cal_type*)tx_multilin_nv_cal_ptr, 
                             temp_vs_freq_params.freq_params.tx_freq,
                             pa_state,
                             TX_LINEARIZER_VOICE,
                             &ref_lin_freq );
    temp_vs_freq_params.freq_params.ref_lin_tx_freq = ref_lin_freq;
    temp_vs_freq_params.ref_xpt_pin_offset = 0;

    if( curr_txlin_data->current_lin_tbl[pa_state].dpd_type == TX_LINEARIZER_ET_DPD_DATA )
    {
     temp_vs_freq_params.ref_xpt_pin_offset = 
       (int32)rf_tdscdma_core_xpt_get_cfg_param( rf_tdscdma_core_util_get_band_from_uarfcn(tx_chan), XPT_CFG_CMN_REF_ET_PIN_OFFSET );
    }
    else if( curr_txlin_data->current_lin_tbl[pa_state].dpd_type == TX_LINEARIZER_EPT_DPD_V2_DATA )
    {
     temp_vs_freq_params.ref_xpt_pin_offset = 
       (int32)rf_tdscdma_core_xpt_get_cfg_param( rf_tdscdma_core_util_get_band_from_uarfcn(tx_chan), XPT_CFG_CMN_REF_EPT_PIN_OFFSET );
    }

	temp_vs_freq_params.auto_pin_enable = rfcommon_autopin_is_enabled(RFM_TDSCDMA_MODE) && rflm_autopin_sanity_check(rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx);

    /* Get compensation values */
    rfcommon_core_txlin_get_temp_compensation( 
       RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V3,   //hardcode, only handle V3 for now
       &pin_comp_offset[pa_state],
       &pout_comp_offset[pa_state],
       &temp_vs_freq_params,
       &rfnv_tdscdma_tx_tbl_ptr->lin_temp_comp_data );
  }
  /* Set comp values for the invalid PA states to 0 */
  for ( ; pa_state < RFCOM_TXLIN_NUM_LINEARIZERS; pa_state++ )
  {
    pout_comp_offset[pa_state] = 0;
    pin_comp_offset[pa_state] = 0;
    /*enable only for debug purpose*/
    /*
    MSG_1( MSG_SSID_RF, MSG_LEGACY_MED,
           "rf_tdscdma_core_temp_comp_vs_freq_get_comp: Invalid PA state %d, "
           "setting Pin and Pout offsets to zero", 
           pa_state );
     */
  }
  
}/* rf_tdscdma_core_temp_comp_vs_freq_get_comp */


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function uses the current temperature to get pin/pout temp comp offset 
*/
void rf_tdscdma_core_temp_comp_get_pin_pout(uint16 tx_chan,
                                            int16 pin_comp_val[],
                                            int16 pout_comp_val[])
{
  uint8 temperature;

  /* scale the raw value */
  /* when the api first time is called, it's possible that scaled therm doesn't exist */
  temperature = rf_tdscdma_core_temp_comp_scale_therm(rf_tdscdma_core_temp_comp_value.last_raw_temp_value, TRUE);

  /* Obtain the lin Pin and Pout vs temp vs freq comp values */
  rf_tdscdma_core_temp_comp_vs_freq_get_comp(tx_chan,
                                             temperature,
                                             pin_comp_val,
                                             pout_comp_val);

  /* Save a copy of Pin value for AutoPin */
  memscpy(rf_tdscdma_core_autopin_pin_comp_init, sizeof(int16)*RFCOM_TXLIN_NUM_LINEARIZERS, pin_comp_val, sizeof(int16)*RFCOM_TXLIN_NUM_LINEARIZERS);
}


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details

*/
void rf_tdscdma_core_temp_comp_calc_txlin_vs_temp(int16 txlin_vs_temp[])
{
  uint8 temperature;
  uint8 temp_index;
  uint8 temp_rem;
  int16 i;

  /* scale the raw value */
  temperature = rf_tdscdma_core_temp_comp_scale_therm(rf_tdscdma_core_temp_comp_value.last_raw_temp_value, FALSE);

  /* calculate temperature compensation table segment index */
  temp_index = temperature / RF_TDSCDMA_CORE_TEMP_BIN_SIZ;

  /* calculate segment remainder */
  temp_rem = temperature % RF_TDSCDMA_CORE_TEMP_BIN_SIZ;

  /* txlin_vs_temp NV has 4 pa states */
  for ( i=0; i < RF_TDSCDMA_NUM_TX_LINEARIZERS; i++ )
  {
    txlin_vs_temp[i] = rf_tdscdma_core_temp_comp_calc_comp_val( rfnv_tdscdma_tx_tbl_ptr->tx_lin_vs_temp[i],
                                                                temp_index,
                                                                temp_rem );
  }
}
