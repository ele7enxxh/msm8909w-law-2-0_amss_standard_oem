/*!
  @file
  rf_tdscdma_core_tuner_temp_comp.c

  @brief
  Provides TDSCDMA Temperature Compensation functionality

*/

/*===========================================================================

Copyright (c) 2011, 2012 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/core/src/rf_tdscdma_core_tuner_temp_comp.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/30/13   ms     Initial version.

============================================================================*/

#include "rfa_variation.h"
#include "rf_tdscdma_core_tuner_temp_comp.h"
#include "rfcommon_nv_mm.h"
#include "msg.h"
#include "rf_tdscdma_mdsp.h"
#include "rf_tdscdma_core_txplim.h"
#include "rf_tdscdma_core_txlin.h"
#include "rfnv_tdscdma.h"
#include "pm.h"
#include "rf_tdscdma_mc.h"
#include "time_svc.h" 
#include "time_tod.h"


#define RF_TDSCDMA_CORE_THERM_MIN 0
#define RF_TDSCDMA_CORE_THERM_MAX 255
#define RF_TDSCDMA_CORE_TEMP_BIN_SIZ (RF_TDSCDMA_CORE_THERM_MAX / (NV_TEMP_TABLE_SIZ-1))

/*Absolute value macro*/
#define RF_TDSCDMA_CORE_TEMP_COMP_ABS(x)               (((x) < 0) ? -(x) : (x))

static rf_tdscdma_core_tuner_temp_comp_value_type rf_tdscdma_core_tuner_temp_comp_value =
{
  FALSE,
  /* The init value is roughly the raw reading at room temp */
  1234,
  110,
  0,
  0
};


#if 0
/* Local functions */
LOCAL void rf_tdscdma_core_temp_based_rfdevice_voltage_scaling(int16 raw_therm_value);
LOCAL uint8 rf_tdscdma_core_temp_comp_scale_therm(uint16 therm_raw_value);
LOCAL int16 rf_tdscdma_core_temp_comp_calc_comp_val(int8 temp_table[], 
                                                    uint8 temp_index, 
                                                    uint8 temp_rem);

#endif



/* Callback pointer to the Temp compensation calculator */
static timer_type rf_tdscdma_core_tuner_temp_comp_clk_cb;

static boolean rf_tdscdma_core_tuner_temp_comp_started = FALSE;


/*  TDSCDMA Tx power compensation w.r.t temperature callback intervals */
static const uint32 RF_TDSCDMA_CORE_TUNER_TEMP_COMP_REPEAT_INT = 10000;/* milliseconds*/ 
static const uint32 RF_TDSCDMA_CORE_TUNER_TEMP_COMP_START_INT = 1000; /* milliseconds */

static void rf_tdscdma_core_tuner_temp_comp_timer(int4 t_unused);



/*--------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the temp comp clock callback.
 
  @details

*/
void rf_tdscdma_core_tuner_temp_comp_init(void)
{
  /* define the temp comp call back */
  timer_def2( &rf_tdscdma_core_tuner_temp_comp_clk_cb, NULL );
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Starts the timer 

*/
void rf_tdscdma_core_tuner_temp_comp_start(void)
{
  /* times set such that the calculation happens 3 secs later so that it
  ** doesn't start running as soon as TX power is enabled because we don't
  ** know if it is a real call
  */
  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH," rf_tdscdma_core_tuner_temp_comp_start Called", 0);

  if ( !rf_tdscdma_core_tuner_temp_comp_started )
  {
    timer_reg( &rf_tdscdma_core_tuner_temp_comp_clk_cb,
               (timer_t2_cb_type)rf_tdscdma_core_tuner_temp_comp_timer,
               (timer_cb_data_type)NULL,
               RF_TDSCDMA_CORE_TUNER_TEMP_COMP_START_INT,
               RF_TDSCDMA_CORE_TUNER_TEMP_COMP_REPEAT_INT);
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH,"timer trigger");
    rf_tdscdma_core_tuner_temp_comp_started = TRUE;
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stops the timer
   
*/
void rf_tdscdma_core_tuner_temp_comp_stop(void)
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH," rf_tdscdma_core_tuner_temp_comp_stop Called", 0);

  if ( rf_tdscdma_core_tuner_temp_comp_started )
  {
    (void)timer_clr(&rf_tdscdma_core_tuner_temp_comp_clk_cb, T_NONE);
   // rf_tdscdma_core_do_temp_comp_calc = FALSE;
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH,"tuner temp comp timer stopped");
    rf_tdscdma_core_tuner_temp_comp_started = FALSE;
  } 
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,
          " Trying to stop TDSCDMA tuner Temp comp. calculator even though it is not started!!!",0);
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is the call back function and is called evey 10seconds. 

*/
void rf_tdscdma_core_tuner_temp_comp_timer(int4 t_unused)
{
  
  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,
        "rf_tdscdma_core_tuner_temp_comp_timer() has been obsoleted",0);

}


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Clear or set temperature compensation enable flag. This function shall be called
  in enter mode or wherever temp comp needs to be enabled

  @param flag Enable or Disable Temperature compensation

*/
void rf_tdscdma_core_tuner_temp_comp_enable(boolean flag)
{
  rf_tdscdma_core_tuner_temp_comp_value.temp_comp_enable_flag = flag;
}


#if 0

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
    /* Update internal temp comp values as well as update to shared memory */
    rf_tdscdma_core_temp_comp_update(therm_raw_value, 
                                     FALSE,
                                     rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx);
  }
  /* if temp comp not enabled, just save raw reading etc */
  else
  {
    (void)rf_tdscdma_core_temp_comp_scale_therm(therm_raw_value);
  }
}


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function gets the raw thermistor read from FW and calls
  a function to interpolate a new temperature compensation value
  for each temperature compensation NV table.

*/
void rf_tdscdma_core_temp_comp_update(uint16 therm_raw_value, 
                                      boolean init_flag,
                                      uint8 buf_idx)
{
  uint8 i;
  uint8 temperature;
  uint8 temp_index;
  uint8 temp_rem;
  int16 tx_lin_vs_temp_val[RF_TDSCDMA_NUM_TX_LINEARIZERS];
  int16 tx_lim_vs_temp_val = 0;
  int16 utran_tx_lim_vs_temp_offset_val = 0;


  //if (rf_tdscdma_core_temp_comp_value.temp_comp_enable_flag == TRUE)
  //{
    /* scale thermistor */
    temperature = rf_tdscdma_core_temp_comp_scale_therm(therm_raw_value);

    /* calculate temperature compensation table segment index */
    temp_index = temperature / RF_TDSCDMA_CORE_TEMP_BIN_SIZ;

    /* calculate segment remainder */
    temp_rem   = temperature % RF_TDSCDMA_CORE_TEMP_BIN_SIZ;

    rf_tdscdma_core_temp_comp_value.last_temp_comp_index = (uint8) temp_index;
    rf_tdscdma_core_temp_comp_value.last_temp_comp_rem   = (uint8) temp_rem;


    for ( i=0; i < RF_TDSCDMA_NUM_TX_LINEARIZERS; i++ )
    {
      tx_lin_vs_temp_val[i] = rf_tdscdma_core_temp_comp_calc_comp_val( rfnv_tdscdma_tx_tbl_ptr->tx_lin_vs_temp[i],
                                                                       temp_index,
                                                                       temp_rem );
    }

    rf_tdscdma_core_txlin_temp_val_update(tx_lin_vs_temp_val);
    /* perform temp comp on the linearizer table, write to shared memory */
    rf_tdscdma_core_txlin_comp_update(init_flag, buf_idx);

    /* tx_lim_vs_temp is a delta value */
    tx_lim_vs_temp_val = rf_tdscdma_core_temp_comp_calc_comp_val( rfnv_tdscdma_tx_tbl_ptr->tx_limit_vs_temp,
                                                                  temp_index,
                                                                  temp_rem );

    /* there is no utran_tx_lim_vs_temp_offset NV. apply the same delta tx_lim_vs_temp_val to utran limit */
    utran_tx_lim_vs_temp_offset_val = tx_lim_vs_temp_val;

    rf_tdscdma_core_txplim_temp_val_update(tx_lim_vs_temp_val, utran_tx_lim_vs_temp_offset_val,
                                           temp_index,
                                           temp_rem);


    // SMPS pdm vs temp update here if APT is required
    
    //TODO - to see if this temp-based voltage scaling is needed on WTR
    rf_tdscdma_core_temp_based_rfdevice_voltage_scaling(rf_tdscdma_core_temp_comp_value.last_raw_temp_value);

  //}
#ifdef FEATURE_RF_TDSCDMA_VBATT_BASED_BACKOFF
  /* Do VBatt based Max Power Backoff */
  rf_tdscdma_core_txplim_do_vbatt_based_backoff();
#endif 
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function validates and scales the temperature value.

*/
uint8 rf_tdscdma_core_temp_comp_scale_therm(uint16 therm_raw_value)
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

  /* save for logging etc */
  rf_tdscdma_core_temp_comp_value.last_raw_temp_value = therm_raw_value;

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

  rf_tdscdma_core_temp_comp_value.last_scaled_temp_value = (uint8)therm_scaled_value;
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

#endif

/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the thermistor related data for logging or other purposes.

*/
rf_tdscdma_core_tuner_temp_comp_value_type rf_tdscdma_core_tuner_temp_comp_get_therm_data(void)
{
  return(rf_tdscdma_core_tuner_temp_comp_value);
}

#if 0
/*--------------------------------------------------------------------------*/
/*!
  @brief

*/
void rf_tdscdma_core_temp_based_rfdevice_voltage_scaling(int16 raw_therm_value)
{
#ifdef RF_HAS_ENHANCED_DYNAMIC_VOLTAGE_SCALING

  pm_vreg_level_type curr_level, new_level;

  (void)rfc_tdscdma_command_dispatch(RFM_DEVICE_0, (int32)RFC_GET_RF1_VOLTAGE, (void*)(&curr_level));

  /* if therm = enh_therm_min, then RTR is >= 85C */
  /* if curr level is not 1300, then it must be 1.238 since those are the only 2 settings for S2 on MDM9k*/
  if ( (raw_therm_value <= rfcommon_nv_mm_tbl.rf_enh_therm_min) &&  (curr_level != 1300))
  //if ( (raw_therm_value < rfcommon_nv_mm_tbl.rf_enh_therm_min) &&  (curr_level != 1300))
  {
    new_level = 1300;
    (void)rfc_tdscdma_command_dispatch(RFM_DEVICE_0, (int32)RFC_SET_RF1_VOLTAGE, (void*)(&new_level));
    MSG(MSG_SSID_RF, MSG_LVL_HIGH,"RF temp comp setting Voltage to 1.3V");
  }
  else if ((raw_therm_value > rfcommon_nv_mm_tbl.rf_enh_therm_min) &&  (curr_level == 1300))
  {
    new_level = 1238;
    (void)rfc_tdscdma_command_dispatch(RFM_DEVICE_0, (int32)RFC_SET_RF1_VOLTAGE, (void*)(&new_level));
    MSG(MSG_SSID_RF, MSG_LVL_HIGH,"RF temp comp setting Voltage to 1.2375V");
  }
#endif
}

#endif
