/*!
  @file
  rf_tdscdma_core_txplim.c

  @brief
  Provides TX Power liming loop algorithm.

  @details
*/

/*===========================================================================

Copyright (c) 2011 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/core/src/rf_tdscdma_core_txplim.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/14/16   nsh     Fix power jump issue
11/13/14   whz     Enable HDET on Jolokia
11/12/14   jyu     MDSP/SMEM cleanup
08/05/14   jhe     Separate mtpl for coex
07/16/14   kb      Convert TxAGC threshold to dB10 for FED compatibility
05/27/14   jhe     update mtpl to rflm
08/27/13   whz     Explicitly clear thermal_backoff_prev_db10 when exiting
08/21/13   whz     Fix MTPL overflow when doing thermal mitigation
07/19/13   jps     Fix FTM RF mode max TX power discrepancy after boot up
04/24/13   qzh     Porting FTM RF code from NIKEL to DIME
03/25/13   jyu     Exposed one API for HDET debug 
02/22/13   jyu     Merged from NikeL
12/07/12   jhe     Interpolate hdet adc to txagc on-the-fly
11/19/12   jps     Do not send HDET config MSG in RF Mode TX
09/07/12   ms      Pass in buf_idx to publish tx pll frequency to wrk/tgt buffer 
08/09/12   ms      Dynamic pll frequency update 
05/02/12   ycl     Max power change to RF TxAGC unit (930)
05/27/11   jhe     Initial version

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfa_variation.h"
#include "rf_tdscdma_core_txplim.h"
#include "rfcommon_nv_mm.h"
#include "rfnv_tdscdma.h"
#include "rex.h"
#include "nv.h"
#include "err.h"
#include "msg.h"
#include "rf_tdscdma_core_txlin.h"
#include "rf_tdscdma_core_util.h"
#include "rf_tdscdma.h"
#include "rf_tdscdma_mc.h"
#include "rf_tdscdma_mdsp.h"
#include "rfdevice_intf_cmd.h"
//#include "rfdevice_tdscdma_intf.h"
#include "math.h"
#include "rfm.h"
#include "ftm.h"
#include "ftm_tdscdma_ctl.h"
#include "rflm_tds_mc.h"


/*===========================================================================

                           CONSTANT DEFINITIONS

===========================================================================*/

/* Typical UTRAN network max power limit value 24dbm */
#define RF_TDSCDMA_TXPLIM_UTRAN_TX_PWR_LIMIT       940

/* min for TX pwr limit software - 15dbm => (15 + 70) * 10 */
#define RF_TDSCDMA_TXPLIM_TX_PWR_LIM_MIN           850

/* max for TX pwr limit software */
#define RF_TDSCDMA_TXPLIM_TX_PWR_LIM_MAX           RF_TDSCDMA_NV_TXAGC_MAX

/* TX Spec Limit for Power Class 3 - 25dbm => (25 + 70) * 10 */
#define RF_TDSCDMA_TXPLIM_TX_SPEC_LIMIT            950

/* 50 => 5dBm */
#define RF_TDSCDMA_TXPLIM_FILT_LIMIT_ERR_MAX       50

/* min TX AGC value */
#define RF_TDSCDMA_TXPLIM_TX_AGC_MIN               RF_TDSCDMA_NV_TXAGC_MIN

/* max Tx AGC value (with offset) */
#define RF_TDSCDMA_TXPLIM_TX_AGC_MAX               RF_TDSCDMA_NV_TXAGC_MAX

/* offset for the 256 element Tx AGC vs HDET table index */
#define RF_TDSCDMA_TXPLIM_TX_AGC_VS_HDET_TBL_OFF   768

/* txagc step size assumed in EXP_HDET_VS_AGC table in units of dB10 */
#define RF_TDSCDMA_TXPLIM_EXP_HDET_VS_AGC_STEP_SIZ 16

/* size of Tx AGC vs HDET interpolated table */
/* 12-bit HDET will require change in the array size */
#define RF_TDSCDMA_TXPLIM_TX_AGC_VS_HDET_TBL_SIZ   4096

/* size of expected HDET vs AGC table, 256 */
#define RF_TDSCDMA_TXPLIM_EXP_HDET_VS_AGC_TBL_SIZ \
            (RF_TDSCDMA_TXPLIM_TX_AGC_VS_HDET_TBL_SIZ / RF_TDSCDMA_HDET_VS_AGC_SIZ)

/* index for top of HDET curve high zone, 15 */
#define RF_TDSCDMA_TXPLIM_EXP_HDET_VS_AGC_MAX_IDX  (RF_TDSCDMA_HDET_VS_AGC_SIZ-1)

/**/
#define RF_TDSCDMA_TXPLIM_EXP_HDET_VS_AGC_SIZ      RF_TDSCDMA_HDET_VS_AGC_SIZ

/* Tx AGC limit is in 1/10 dB */
#define RF_TDSCDMA_TXPLIM_TX_PWR_LIM_CNT_PER_DB    RF_TDSCDMA_TXAGC_RESOLUTION

/* minimum value for TX power limit in dbm */
#define RF_TDSCDMA_TXPLIM_MIN_TX_PWR_LIM           RF_TDSCDMA_MIN_TX_POWER_DBM

/* maximum value for TX power limit in dbm */
#define RF_TDSCDMA_TXPLIM_MAX_TX_PWR_LIM           RF_TDSCDMA_UE_MAX_TX_POWER

/* default MAX TX power limit in SW unit = 930 = 23dbm */
#define RF_TDSCDMA_TXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT   RF_TDSCDMA_UE_MAX_TX_POWER_DB10

/* maximum possible hdet value */
#define RF_TDSCDMA_TXPLIM_MAX_HDET 4095

/* amount to shift to translate between integer and 11Q5 values */
#define RF_TDSCDMA_TXPLIM_Q5 5

#define RF_TDSCDMA_TXPLIM_2Q5 32

#define RF_TDSCDMA_TXPLIM_THERMAL_PMIN_DB 10

/* Macros to convert between integer and Q5 values */
#define RF_TDSCDMA_TXPLIM_INT_TO_Q5(x) ((x) << RF_TDSCDMA_TXPLIM_Q5)

#define RF_TDSCDMA_TXPLIM_Q5_TO_INT(x) ((x) >> RF_TDSCDMA_TXPLIM_Q5)

#define RF_TDSCDMA_TXPLIM_Q5_INTEGER(x) ((x) >= 0) ? ((x) >> RF_TDSCDMA_TXPLIM_Q5) : (~((~x+1) >> RF_TDSCDMA_TXPLIM_Q5) + 1)

#define RF_TDSCDMA_TXPLIM_Q5_FRACTION(x) ((x) >= 0) ? ((x) - RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_Q5_TO_INT(x)))*100/RF_TDSCDMA_TXPLIM_2Q5 \
                                           : ((~x+1) - RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_Q5_TO_INT(~x+1)))*100/RF_TDSCDMA_TXPLIM_2Q5

#define RF_TDSCDMA_TXPLIM_INIT_ACQ_COUNT 100

/*
                              ACQ mode
          |------<------|-----------------
 TRK mode |             ^
----------|------>------| 
     acq2trk_tol    trk2acq_tol
        (1dB)         (2dB)
----------ABS(Limit_Error)-------------->   
*/
/* Limit Error ACQ->TRK tolerance = 1dB */
#define RF_TDSCDMA_TXPLIM_ACQ2TRK_TOLERANCE 10

/* Limit Error tolerance = 2dB */
#define RF_TDSCDMA_TXPLIM_TRK2ACQ_TOLERANCE 20



/*===========================================================================

                           LOCAL VARIABLES DEFINITIONS

===========================================================================*/
/* flag to enable TX power limit */
boolean        rf_tdscdma_core_txplim_enabled = FALSE;

/* flag to indicate first pass of TX power limiting */
boolean        rf_tdscdma_core_txplim_first_time_flag = TRUE;

/* Tx Power limit loop filter gain */
static int8 rf_tdscdma_core_txplim_gain = 4;

/* Received HDET trigger counter */
static uint16 rf_tdscdma_core_txplim_counter = 0;

rf_tdscdma_core_txplim_mode_type rf_tdscdma_core_txplim_mode = RF_TDSCDMA_TXPLIM_MODE_DISABLED;

/* Global variable: HDET reported calibration error in dB10 */
/* HDET value - TxAGC_buffered */
int16  rf_tdscdma_core_txplim_cal_error = 0;

/* flag to enable RF Txplim debug messages */
boolean rf_tdscdma_core_txplim_dbgmsg = TRUE;

static int16 rf_tdscdma_core_txplim_backoff_limit = 0;
static int16 rf_tdscdma_core_txplim_coex_backoff_limit = 0;

static rfcom_txplim_channel_type rf_tdscdma_core_txplim_chan_type = RFCOM_TXPLIM_DCH;

/* This value is the absolute TX power limit value based on temperature. */
/* It is NOT a delta value. */
/* will give it a default value */
LOCAL int16    rf_tdscdma_core_txlim_vs_temp_val = RF_TDSCDMA_TXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT;

/* tx pwr limit frequency comp value */
/* If the freq. comp val is positive then increase the TX power or */
/* shift the TX lin table to the right */
/* If the freq. comp val is negative then decrease the TX power or */
/* shift the TX lin table to the left */
LOCAL int16    rf_tdscdma_core_txlim_vs_freq_val = 0;
LOCAL int16    rf_tdscdma_core_txlim_vs_temp_val_delta = 0;

/* UTRAN TX power limit temperature compensation offset value*/
/* This value is a UTRAN TX power limit temperature compensation offset */
LOCAL int16    rf_tdscdma_core_txlim_utran_vs_temp_offset_val = 0;

/* last raw reading of HDET from ADC */
LOCAL int16    rf_tdscdma_core_txplim_hdet_adc;

/* last reading of HDET in dB10 units */
LOCAL int16    rf_tdscdma_core_txplim_hdet;

/* last reading of TX_AGC value from MDSP */
/* this is 0..1023 min..max */
LOCAL int16    rf_tdscdma_core_txplim_txagc;

/* last software value for Max Tx power limit (MTPL) as an Q5 value */
/* this TxAGC value in 0..1023 min..max units shifted by 5 */
LOCAL uint32  rf_tdscdma_core_txplim_mtpl_q5 = RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT);

LOCAL uint32  rf_tdscdma_core_txplim_mtpl_q5_coex = RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT);

/* Tx power limit commanded by UTRAN */
/* this is 0..1023 min..max */
LOCAL int16  rf_tdscdma_core_txplim_utpl = RF_TDSCDMA_TXPLIM_UTRAN_TX_PWR_LIMIT;

/* TX power limit set by NV_TDSCDMA_TX_LIM_VS_TEMP */
/* this is 0..1023 min..max */
LOCAL int16  rf_tdscdma_core_txplim_ntpl = RF_TDSCDMA_TXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT;

LOCAL int16  rf_tdscdma_core_txplim_desired = RF_TDSCDMA_TXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT;

LOCAL int16  rf_tdscdma_core_txplim_desired_coex = RF_TDSCDMA_TXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT;

/* pointer to expected HDET vs AGC table */
LOCAL uint16    *rf_tdscdma_core_txplim_exp_hdet_vs_agc_ptr;

/* Expected AGC (TX AGC Value - 768) for each HDET value 0..255 */
/* 768 is used to scale down the TX AGC value range of 768..1023 to */
/* a range of 0..255. why ? because TX power limiting is only done */
/* on the top 25% of TX AGC range */
//LOCAL uint8    rf_tdscdma_core_txplim_exp_agc_vs_hdet[RF_TDSCDMA_TXPLIM_TX_AGC_VS_HDET_TBL_SIZ];

/* MPR values */
static uint8 rf_tdscdma_core_txplim_mprVals[RF_TDSCDMA_MPR_SIZ] = 
  {0, 0, 5, 10, 15, 20, 25};

/* TX Gain Shift - this contains the Gain backoff in digital and the Gain comp value in RF 
   they're same value but opposite direction */
static uint8 rf_tdscdma_core_txplim_txGainShift[RF_TDSCDMA_MPR_SIZ] =
  {0, 10, 15, 20, 25, 30, 35};  

static uint8 rf_tdscdma_core_txplim_paThresholdOffset[RF_TDSCDMA_MPR_SIZ] = 
  {0, 10, 15, 20, 25, 30, 35};

/* MPR adjustment values */
static uint8 rf_tdscdma_core_txplim_mprVals_adjustment[RF_TDSCDMA_MPR_SIZ] = 
  {0, 0, 0, 0, 0, 0, 0};

static int16 rf_tdscdma_core_txplim_vbatt_backoff = 0;

static int16 rf_tdscdma_core_txplim_thermal_backoff_db10 = 0;
static int16 rf_tdscdma_core_txplim_thermal_backoff_prev_db10 = 0;

/* in TDS subframes */
static uint16 rf_tdscdma_core_txplim_hdet_interval = 2;

static uint16 rf_tdscdma_core_txplim_coex_limit_db10 = RF_TDSCDMA_NV_TXAGC_MAX;

static uint16 rf_tdscdma_core_txplim_sar_limit_db10 = RF_TDSCDMA_NV_TXAGC_MAX;

/*===========================================================================

                           LOCAL FUNCTIONS DECLARATIONS

===========================================================================*/

LOCAL void rf_tdscdma_core_txplim_update_desired_limit(void);

LOCAL void rf_tdscdma_core_txplim_adjust_mtpl(void);

LOCAL void rf_tdscdma_core_txplim_update_mode(uint16 abs_limit_error);


/*===========================================================================

                           FUNCTIONS DEFINITIONS

===========================================================================*/
extern rf_tdscdma_ftm_burst_tx_params_type ftm_tdscdma_get_rf_mode_tx_params(void);
/*--------------------------------------------------------------------------*/
/*!
  @brief
  Initializes TX power limiting algorithm

  @details
  This function initializes the TX power limiting algorithm parameters
  with parameters data from NV and calculates an expected AGC based on HDET
  table. This function must be called in rf_init after NV items are read.
  For multimode, call this function when changing mode and pass in the
  appropriate parameters based on the NV items for that mode.

*/

void rf_tdscdma_core_txplim_init(void)
{
  uint8 i = 0;

  rf_tdscdma_core_txplim_exp_hdet_vs_agc_ptr  = rfnv_tdscdma_tx_tbl_ptr->exp_hdet_vs_agc;

  /* calculate the 256 element expected AGC versus HDET table */
  /* from the 16+1 element expected HDET versus AGC table */
  //no longer needed, do interpolation on the fly
  //rf_tdscdma_core_txplim_calc_exp_agc_vs_hdet_table();

  for ( i=0; i < RF_TDSCDMA_MPR_SIZ; i++ )
  {
    rf_tdscdma_core_txplim_mprVals[i]     = (uint8)rfnv_tdscdma_tx_tbl_ptr->mpr_backoff_val[i];
    //TODO - to delete. no need to save txGainshift and pa_switching - they're updated to mdsp together with
    //other band-dependent NVs
    rf_tdscdma_core_txplim_txGainShift[i] = (uint8)rfnv_tdscdma_tx_tbl_ptr->mpr_digital_backoff[i];
    rf_tdscdma_core_txplim_paThresholdOffset[i] = (uint8)rfnv_tdscdma_tx_tbl_ptr->mpr_pa_switchpoints_shift[i];
  }

  /* disable TX power limiting initially */
  rf_tdscdma_core_txplim_enable(FALSE);
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
   This function set the Max tx power limit(MTPL) based on a new UTRAN TX power limit value 
   which is specified in dbm unit.

  @details


*/
void rf_tdscdma_core_txplim_set_mtpl( int16 utran_limit_dBm, rfcom_txplim_channel_type chan_type)
{
  uint8 i = 0;
  uint8 mpr = 0;
  int16 old_limit;
  int16 mtpl_diff;
  int16 utran_limit_db10;

  utran_limit_db10 = (utran_limit_dBm-RF_TDSCDMA_MIN_TX_POWER_DBM) * RF_TDSCDMA_TXAGC_RESOLUTION; /*lint !e734 */

  /* Store the old limit */
  old_limit = rf_tdscdma_core_txplim_desired;

  /* UTRAN Tx Power Limit (UTPL) */
  rf_tdscdma_core_txplim_utpl = utran_limit_db10;

  /* Store Channel Type RACH or DCH */
  rf_tdscdma_core_txplim_chan_type = chan_type;

  /* Update Desired Limit upon UTPL change */
  rf_tdscdma_core_txplim_update_desired_limit();
 
  /* Max Tx Power Limit (MTPL) */
  if(rf_tdscdma_core_txplim_first_time_flag)
  {
    rf_tdscdma_core_txplim_mtpl_q5 = RF_SATURATE( RF_TDSCDMA_TXPLIM_INT_TO_Q5(rf_tdscdma_core_txplim_desired),
                                                  RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_TX_AGC_MIN),
                                                  RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_TX_PWR_LIM_MAX)); /*lint !e734 */
    
    /* not the first time anymore */
    rf_tdscdma_core_txplim_first_time_flag = FALSE;
  }
  else
  {
    /* To account for scenairo where an UTRAN update happens during a call (stablized MTPL loop) */ 
    /* don't overwrite current power limit with uncorrected utran limit, just update the differance */
    mtpl_diff = rf_tdscdma_core_txplim_desired - old_limit;
    rf_tdscdma_core_txplim_mtpl_q5 += RF_TDSCDMA_TXPLIM_INT_TO_Q5(mtpl_diff); /*lint !e734 */
    rf_tdscdma_core_txplim_mtpl_q5 = RF_SATURATE( rf_tdscdma_core_txplim_mtpl_q5, 
                                                  RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_TX_AGC_MIN), 
                                                  RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_TX_PWR_LIM_MAX));
  }
  
  /* MDSP writes: MPR (power backoff) beta scaling table */
  for ( i=0; i < RF_TDSCDMA_MPR_SIZ; i++ )
  {
    /* MPR limit check - rf_tdscdma_core_txplim_backoff_limit contains (NV limit - Actual Tx Limit)
       If MPR is less than rf_tdscdma_core_txplim_backoff_limit we don't have to backoff any further and if MPR is 
       greater than rf_tdscdma_core_txplim_backoff_limit the new MPR will be MPR-rf_tdscdma_core_txplim_backoff_limit */
    mpr = (uint8)(rf_tdscdma_core_txplim_mprVals[i] > rf_tdscdma_core_txplim_backoff_limit) ? 
             (rf_tdscdma_core_txplim_mprVals[i] - rf_tdscdma_core_txplim_backoff_limit):0; /*lint !e734 */
    //rf_tdscdma_mdsp_set_mpr_beta_table(i, mpr, rf_tdscdma_core_txplim_txGainShift[i]);    
    //rf_tdscdma_mdsp_set_pa_thresh_offset(i, rf_tdscdma_core_txplim_paThresholdOffset[i]);
    rf_tdscdma_core_txplim_mprVals_adjustment[i] = rf_tdscdma_core_txplim_mprVals[i] - mpr; 
  }

  #if 0 // Comment out for MDSP/SMEM cleanup
  /* If in RXTX state, update the adjusted mpr value to mdsp */
  if ( (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX) &&
       rf_tdscdma_core_txplim_enabled ) 
  {
    rf_tdscdma_core_txplim_mprval_update(FALSE, rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx);

    /* update the mtpl to mdsp smem - this is necessary when hdet loop is not running (eg <15dbm) */
    rf_tdscdma_core_txplim_update_mtpl_to_mdsp(rf_tdscdma_core_txplim_get_mtpl_backoff(), 
                                               FALSE,
                                               rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx);  
  }
  #endif
  
  if(rf_tdscdma_core_txplim_dbgmsg)
  { 
    MSG_7(MSG_SSID_RF, MSG_LVL_HIGH,"RFTXPLIM-UTRAN:%05d,DCH:%d,UTRAN:%03d,NV:%03d,Desired:%03d,MTPL:%03d.%02d",
      rf_tdscdma_core_txplim_counter,
      chan_type,
      (rf_tdscdma_core_txplim_utpl + rf_tdscdma_core_txlim_utran_vs_temp_offset_val),
      rf_tdscdma_core_txplim_ntpl,
      rf_tdscdma_core_txplim_desired,
      /* MTPL Integer part */
      RF_TDSCDMA_TXPLIM_Q5_TO_INT(rf_tdscdma_core_txplim_mtpl_q5),
      /* MTPL Fraction part */
      RF_TDSCDMA_TXPLIM_Q5_FRACTION(rf_tdscdma_core_txplim_mtpl_q5)); /*lint !e502 */
  }
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function update the desired limit based on UTRAN TX power limit value
  and NV TX power limit along with lim_vs_freq.

  @details

*/
LOCAL void rf_tdscdma_core_txplim_update_desired_limit(void)
{
  int16 power_limit;

  /* get the min of sar limit and Utran limit */
  power_limit = MIN(rf_tdscdma_core_txplim_sar_limit_db10, rf_tdscdma_core_txplim_utpl);

  /* NV Tx Power Limit (NTPL) */
  rf_tdscdma_core_txplim_ntpl = rf_tdscdma_core_txplim_get_mtpl_nv_item(rf_tdscdma_core_txplim_chan_type);

  /* update the Lecacy Backoff Limit */
  rf_tdscdma_core_txplim_backoff_limit = (int16) MAX((rf_tdscdma_core_txplim_ntpl - 
                    (power_limit + rf_tdscdma_core_txlim_utran_vs_temp_offset_val)),0);

  /* Legacy Desired Limit - Excludes Coex */
  rf_tdscdma_core_txplim_desired = MIN(rf_tdscdma_core_txplim_ntpl, 
                    (power_limit + rf_tdscdma_core_txlim_utran_vs_temp_offset_val));

  /*Update coex back-off limit*/
  rf_tdscdma_core_txplim_coex_backoff_limit = (int16) MAX((rf_tdscdma_core_txplim_coex_limit_db10 - rf_tdscdma_core_txplim_desired), 0);

  /* Desired Limit for coex*/
  rf_tdscdma_core_txplim_desired_coex = MIN(rf_tdscdma_core_txplim_desired, rf_tdscdma_core_txplim_coex_limit_db10);

  /* Add Freq Comp offset */
  rf_tdscdma_core_txplim_desired += rf_tdscdma_core_txlim_vs_freq_val;
  rf_tdscdma_core_txplim_desired_coex += rf_tdscdma_core_txlim_vs_freq_val;

  /* Add thermal mitigation backoff */
  rf_tdscdma_core_txplim_desired += rf_tdscdma_core_txplim_thermal_backoff_db10;
  rf_tdscdma_core_txplim_desired_coex += rf_tdscdma_core_txplim_thermal_backoff_db10;

  /* Update the desired limit value into RFLM when it changes */

  //Skip below in TA since TA does not use FBRx for TDS now.
  /*
  rflm_tds_set_mtpl((int32)rf_tdscdma_core_txplim_get_desired_limit(),
                    (int32)rf_tdscdma_core_txplim_get_desired_limit_coex());*/

} 


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function enables/disables the TX power limit adjustment algorithm.
  This function should be called when TX is enabled/disabled.

  @param enalbe Enable or disable Tx power limiting

*/
void rf_tdscdma_core_txplim_enable(boolean enable)
{
  rf_tdscdma_core_txplim_first_time_flag = TRUE;
  rf_tdscdma_core_txplim_enabled = enable;

  if(enable)
  {
    rf_tdscdma_core_txplim_mode = RF_TDSCDMA_TXPLIM_MODE_INIT;
    rf_tdscdma_core_txplim_counter = 0;
    rf_tdscdma_core_txplim_gain =  4;    /* Filter Gain */
    if(rf_tdscdma_core_txplim_dbgmsg)
    { 
       MSG_1(MSG_SSID_RF, MSG_LVL_HIGH,"RFTXPLIM-ENABLE:%05d",rf_tdscdma_core_txplim_counter);
    }
  }
  else
  {
    rf_tdscdma_core_txplim_mode = RF_TDSCDMA_TXPLIM_MODE_DISABLED;
    rf_tdscdma_core_txplim_thermal_backoff_db10 = 0;
	rf_tdscdma_core_txplim_thermal_backoff_prev_db10 = 0;
    if(rf_tdscdma_core_txplim_dbgmsg)
    { 
       MSG_1(MSG_SSID_RF, MSG_LVL_HIGH,"RFTXPLIM-DISABLE:%05d",rf_tdscdma_core_txplim_counter);
    }
    
  }
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  Returns Max TX power limit(MTPL) obtained by the limiting loop

  @details

*/
int16 rf_tdscdma_core_txplim_get_mtpl(void)
{
  //return(RF_TDSCDMA_TXPLIM_Q5_TO_INT(rf_tdscdma_core_txplim_mtpl_q5) + RF_TDSCDMA_AGC_MIN_AGC_VAL);
  return(RF_TDSCDMA_TXPLIM_Q5_TO_INT(rf_tdscdma_core_txplim_mtpl_q5));
}


//TODO - will check if separate RACH/DCH processing(each has its own limit) is really needed
/*--------------------------------------------------------------------------*/
/*!
   @brief
   This function return the NV TX power limit in dB10 unit. 
   
   @details
*/
int16 rf_tdscdma_core_txplim_get_mtpl_nv_item(rfcom_txplim_channel_type chan_type)
{
  int16 max_tx_power = RF_TDSCDMA_UE_MAX_TX_POWER_DB10;

  if ( chan_type == RFCOM_TXPLIM_RACH )
  {
    /* NV value 930 means 23dbm */
    max_tx_power = rfnv_tdscdma_tx_tbl_ptr->max_tx_power;
  }
  else if ( chan_type == RFCOM_TXPLIM_DCH )
  {
    max_tx_power = rf_tdscdma_core_txlim_vs_temp_val;
  }
  else
  {
    MSG_ERROR("Invalid control channel specified for max Tx power %d", chan_type,0,0);
  }

  return (max_tx_power);
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  Returns UE max tx limit/Desired Limit (dbm10)

  @details

*/

int16 rf_tdscdma_core_txplim_get_desired_limit(void)
{
  return( rf_tdscdma_core_txplim_desired + RF_TDSCDMA_TXAGC_RESOLUTION*RF_TDSCDMA_MIN_TX_POWER_DBM);
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  Returns UE max coex tx limit/Desired Limit (dbm10)

  @details

*/

int16 rf_tdscdma_core_txplim_get_desired_limit_coex(void)
{
  return( rf_tdscdma_core_txplim_desired_coex + RF_TDSCDMA_TXAGC_RESOLUTION*RF_TDSCDMA_MIN_TX_POWER_DBM);
}


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function reads the latest hdet value (txagc db10 unit) being used by the RF driver.

*/

int16 rf_tdscdma_core_txplim_get_hdet(void)
{
  return ((int16)rf_tdscdma_core_txplim_hdet);
}


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function returns the last raw hdet adc value.

*/

uint16 rf_tdscdma_core_txplim_get_hdet_adc(void)
{
  return ((uint16)rf_tdscdma_core_txplim_hdet_adc);
}


/*--------------------------------------------------------------------------*/
/*!
   @brief

  @details 
  Return HDET threshold value (15 dB) in dB10 TxAGC units.
  
  @param none

*/

int16 rf_tdscdma_core_txplim_get_hdet_threshold(void)
{
  int16 hdet_thres = RF_TDSCDMA_TXPLIM_TX_PWR_LIM_MIN;
  return hdet_thres;
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  Returns MTPL values for different BNCS cases in TX AGC counts.

  @details
  The function updates the passed rf_max_tx_pwr_limit_type structure with
  MTPL (Max Tx Power Limit) values for difference BCS case in Tx AGC counts.
  L1 should call this function on every frame boundary and update the L1-DSP table with MTPL values.

  @param rf_mtpl <Parameter description>

  @pre
  rfm_init is called.
  TDSCDMA mdsp fw image must be loaded.
*/

int16 rf_tdscdma_core_txplim_get_mtpl_backoff(void)
{
  int16 mtpl;

  mtpl = rf_tdscdma_core_txplim_get_mtpl();

  return (mtpl - MAX(0,(rf_tdscdma_core_txplim_vbatt_backoff - rf_tdscdma_core_txplim_backoff_limit)));
}


#ifdef FEATURE_RF_TDSCDMA_VBATT_BASED_BACKOFF
/*--------------------------------------------------------------------------*/
/*!
  @brief
  
  @details
  This function reads the VBatt voltage and compares it with the three 
  'nv_power_backoff_voltages' values.  Depending on the current VBatt value,  
  the rf_backoff_for_bcs variable is assigned to the nv_power_backoff_volt#.

*/
void rf_tdscdma_core_txplim_do_vbatt_based_backoff(void)
{
  uint8 vbatt=0;

  /* Assign the max_pwr_backoff_volt2(mid range) to rf_backoff by default */
  rf_tdscdma_core_txplim_vbatt_backoff = rfnv_tdscdma_tx_tbl_ptr->max_pwr_backoff_volt2;

  if ( (rfnv_tdscdma_tx_tbl_ptr->max_pwr_backoff_voltages[2] - 
        rfnv_tdscdma_tx_tbl_ptr->max_pwr_backoff_voltages[0]) != 0 )
  {
    /* Read the current vBatt from ADC */
    vbatt = (uint8)vbatt_read();

    /* vbatt between voltages[0](min ADC vbatt) & voltages[1] NVs */
    if ( ( vbatt >= rfnv_tdscdma_tx_tbl_ptr->max_pwr_backoff_voltages[0] ) &&
         ( vbatt < rfnv_tdscdma_tx_tbl_ptr->max_pwr_backoff_voltages[1] ) )
    {
      rf_tdscdma_core_txplim_vbatt_backoff = rfnv_tdscdma_tx_tbl_ptr->max_pwr_backoff_volt1;
    }
    /* vbatt between voltages[1] & voltages[2] NVs */
    else if ( ( vbatt >= rfnv_tdscdma_tx_tbl_ptr->max_pwr_backoff_voltages[1] ) &&
              ( vbatt < rfnv_tdscdma_tx_tbl_ptr->max_pwr_backoff_voltages[2] ) )
    {
      rf_tdscdma_core_txplim_vbatt_backoff = rfnv_tdscdma_tx_tbl_ptr->max_pwr_backoff_volt2;
    }
    /* vbatt greater than voltages[2] NVs */
    else if ( vbatt >= rfnv_tdscdma_tx_tbl_ptr->max_pwr_backoff_voltages[2] )
    {
      rf_tdscdma_core_txplim_vbatt_backoff = rfnv_tdscdma_tx_tbl_ptr->max_pwr_backoff_volt3[0];
    } 
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_ERROR," Invalid values in Max Power Backoff Voltages: %d", vbatt); 
    }
  } 
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR," Invalid values in Max Power Backoff Voltages: %d", vbatt); 
  }
}
#endif /* FEATURE_RF_TDSCDMA_VBATT_BASED_BACKOFF */


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function read HDET value from ADC and TX AGC value from MDSP and then
  call the rftxplim_adjust_tx_pwr_limit function to adjust the TX power limit
  based on temperature, frequency, and HDET feedback. This function is called
  by rf_do_tx_pwr_limit every 100ms when in track mode.

  @details
  rftxplim_init must be called first.
  temperature must be read and temperature data must be ready.
  frequency index must be calculated.
  Updates MDSP TX power limit and PA range threshold

*/
void rf_tdscdma_core_txplim_do_mtpl(uint16 hdet_adc, int16 mdsp_txagc)
{

  /* Update Counter */
  if(rf_tdscdma_core_txplim_counter == 0xFFFF)
  {
    /* Avoid Overflow */
    rf_tdscdma_core_txplim_counter = RF_TDSCDMA_TXPLIM_INIT_ACQ_COUNT;
  }
  else
  {
    rf_tdscdma_core_txplim_counter++; /* Increment counter */
  }

  /* Save the TxAGC value reported from mdsp */
  rf_tdscdma_core_txplim_txagc = mdsp_txagc;

  if((hdet_adc == 0xFFFF) || 
     (hdet_adc < rf_tdscdma_core_txplim_exp_hdet_vs_agc_ptr[0]) ||
     (hdet_adc > rf_tdscdma_core_txplim_exp_hdet_vs_agc_ptr[RF_TDSCDMA_TXPLIM_EXP_HDET_VS_AGC_MAX_IDX]) ||
     (rf_tdscdma_core_txplim_txagc > RF_TDSCDMA_TXPLIM_TX_PWR_LIM_MAX) ||
     (rf_tdscdma_core_txplim_txagc < RF_TDSCDMA_TXPLIM_TX_PWR_LIM_MIN))
  {
    /* HDET mode Invalid */
    rf_tdscdma_core_txplim_mode = RF_TDSCDMA_TXPLIM_MODE_INVALID;
  
    /* Invalid HDET reading */
    rf_tdscdma_core_txplim_hdet = 0xFFFF; /*lint !e569 */

    if(rf_tdscdma_core_txplim_dbgmsg)
    {
      MSG_6(MSG_SSID_RF, MSG_LVL_HIGH,
        "RFTXPLIM-INVALID:%05d,TxAGC:%03d,HDET_adc:%d,Desired:%03d,MTPL:%03d.%02d",
        rf_tdscdma_core_txplim_counter,
        rf_tdscdma_core_txplim_txagc,
        hdet_adc,
        rf_tdscdma_core_txplim_desired,
        /* MTPL Integer part */
        RF_TDSCDMA_TXPLIM_Q5_TO_INT(rf_tdscdma_core_txplim_mtpl_q5),
        /* MTPL Fraction part */
        RF_TDSCDMA_TXPLIM_Q5_FRACTION(rf_tdscdma_core_txplim_mtpl_q5)); /*lint !e502 */
    }
  }
  else
  {
    /* HDET AGC value by interpolation */
    rf_tdscdma_core_txplim_hdet = rf_tdscdma_core_txplim_convert_hdet_adc_to_txagc(hdet_adc);

    rf_tdscdma_core_txplim_hdet_adc = hdet_adc;

    if( (rf_tdscdma_core_txplim_enabled) && (rf_tdscdma_core_txplim_desired >= RF_TDSCDMA_TXPLIM_TX_PWR_LIM_MIN) )
    {
      /* Adjust TX power limit */
      rf_tdscdma_core_txplim_adjust_mtpl();
    }
    else
    {
      /* HDET Disabled */
      rf_tdscdma_core_txplim_mode = RF_TDSCDMA_TXPLIM_MODE_DISABLED;
      
      if(rf_tdscdma_core_txplim_dbgmsg)
      {
        MSG_6(MSG_SSID_RF, MSG_LVL_HIGH,
          "RFTXPLIM-DISABLED:%05d,TxAGC:%03d,HDET:%03d,Desired:%03d,MTPL:%03d.%02d",
          rf_tdscdma_core_txplim_counter,
          rf_tdscdma_core_txplim_txagc,
          rf_tdscdma_core_txplim_hdet,
          rf_tdscdma_core_txplim_desired,
          /* MTPL Integer part */
          RF_TDSCDMA_TXPLIM_Q5_TO_INT(rf_tdscdma_core_txplim_mtpl_q5),
          /* MTPL Fraction part */
          RF_TDSCDMA_TXPLIM_Q5_FRACTION(rf_tdscdma_core_txplim_mtpl_q5)); /*lint !e502 */
      }
    }
  }

  rflm_tds_set_mtpl(rf_tdscdma_core_txplim_get_mtpl_backoff()-700,
                    rf_tdscdma_core_txplim_get_mtpl_backoff()-700);

  #if 0 // Comment out for MDSP/SMEM cleanup
  if( rf_tdscdma_core_txplim_enabled )
  {
    /* Update mtpl to mdsp (f1) */
    rf_tdscdma_core_txplim_update_mtpl_to_mdsp(rf_tdscdma_core_txplim_get_mtpl_backoff(), 
                                               FALSE,
                                               rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx);
  }
  #endif

} /* rftxplim_do_tx_pwr_limit */


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Recalculates the TX power limit based on the frequency, temperature, current
  HDET value, and TX AGC value.

*/

LOCAL void rf_tdscdma_core_txplim_adjust_mtpl(void)
{
  int16 target_limit = 0;
  int16 limit_error_q5 = 0;
  int16 filtered_limit_error_q5 = 0;
 
  /* HDET Cal Error */
  rf_tdscdma_core_txplim_cal_error = rf_tdscdma_core_txplim_hdet - rf_tdscdma_core_txplim_txagc;
  
  target_limit = rf_tdscdma_core_txplim_desired - rf_tdscdma_core_txplim_cal_error;
  
  limit_error_q5 = RF_TDSCDMA_TXPLIM_INT_TO_Q5(target_limit) - rf_tdscdma_core_txplim_mtpl_q5; /*lint !e734 */

  rf_tdscdma_core_txplim_update_mode((uint16)abs(limit_error_q5));

  filtered_limit_error_q5 = RF_SATURATE(limit_error_q5/rf_tdscdma_core_txplim_gain,
                                        RF_TDSCDMA_TXPLIM_INT_TO_Q5(-RF_TDSCDMA_TXPLIM_FILT_LIMIT_ERR_MAX),
                                        RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_FILT_LIMIT_ERR_MAX));
  
  rf_tdscdma_core_txplim_mtpl_q5 = RF_SATURATE( rf_tdscdma_core_txplim_mtpl_q5 + filtered_limit_error_q5,                                            
                                                RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_TX_AGC_MIN),
                                                RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_TX_PWR_LIM_MAX));

  if(rf_tdscdma_core_txplim_dbgmsg)
  {
    if(rf_tdscdma_core_txplim_mode == RF_TDSCDMA_TXPLIM_MODE_ACQ)
    {
      MSG_9(MSG_SSID_RF, MSG_LVL_HIGH,
        "RFTXPLIM-ACQ:%05d,TxAGC:%03d,HDET:%03d,FreqComp:%02d,UTRAN:%03d,NV:%03d,Desired:%03d,Target:%03d,DEM:%03d",
        rf_tdscdma_core_txplim_counter,
        rf_tdscdma_core_txplim_txagc,
        rf_tdscdma_core_txplim_hdet,
        rf_tdscdma_core_txlim_vs_freq_val,
        (rf_tdscdma_core_txplim_utpl + rf_tdscdma_core_txlim_utran_vs_temp_offset_val),
        rf_tdscdma_core_txplim_ntpl,
        rf_tdscdma_core_txplim_desired,
        target_limit,
        rf_tdscdma_core_txplim_thermal_backoff_db10);
      MSG_8(MSG_SSID_RF, MSG_LVL_HIGH,
        "RFTXPLIM-ACQ:%05d,Gain:%02d,LimitErr:%02d.%02d,Filtered:%02d.%02d,MTPL:%03d.%02d",
        rf_tdscdma_core_txplim_counter,
        rf_tdscdma_core_txplim_gain,
        RF_TDSCDMA_TXPLIM_Q5_INTEGER(limit_error_q5),RF_TDSCDMA_TXPLIM_Q5_FRACTION(limit_error_q5),
        RF_TDSCDMA_TXPLIM_Q5_INTEGER(filtered_limit_error_q5),RF_TDSCDMA_TXPLIM_Q5_FRACTION(filtered_limit_error_q5),
        RF_TDSCDMA_TXPLIM_Q5_TO_INT(rf_tdscdma_core_txplim_mtpl_q5),
        RF_TDSCDMA_TXPLIM_Q5_FRACTION(rf_tdscdma_core_txplim_mtpl_q5)); /*lint !e502 */
    }
    else if(rf_tdscdma_core_txplim_mode == RF_TDSCDMA_TXPLIM_MODE_TRK)
    {
      MSG_9(MSG_SSID_RF, MSG_LVL_HIGH,
        "RFTXPLIM-TRK:%05d,TxAGC:%03d,HDET:%03d,FreqComp:%02d,UTRAN:%03d,NV:%03d,Desired:%03d,Target:%03d,DEM:%03d",
        rf_tdscdma_core_txplim_counter,
        rf_tdscdma_core_txplim_txagc,
        rf_tdscdma_core_txplim_hdet,
        rf_tdscdma_core_txlim_vs_freq_val,
        (rf_tdscdma_core_txplim_utpl + rf_tdscdma_core_txlim_utran_vs_temp_offset_val),
        rf_tdscdma_core_txplim_ntpl,
        rf_tdscdma_core_txplim_desired,
        target_limit,
        rf_tdscdma_core_txplim_thermal_backoff_db10);
      MSG_8(MSG_SSID_RF, MSG_LVL_HIGH,
        "RFTXPLIM-TRK:%05d,Gain:%02d,LimitErr:%02d.%02d,Filtered:%02d.%02d,MTPL:%03d.%02d",
        rf_tdscdma_core_txplim_counter,
        rf_tdscdma_core_txplim_gain,
        RF_TDSCDMA_TXPLIM_Q5_INTEGER(limit_error_q5),RF_TDSCDMA_TXPLIM_Q5_FRACTION(limit_error_q5),
        RF_TDSCDMA_TXPLIM_Q5_INTEGER(filtered_limit_error_q5),RF_TDSCDMA_TXPLIM_Q5_FRACTION(filtered_limit_error_q5),
        RF_TDSCDMA_TXPLIM_Q5_TO_INT(rf_tdscdma_core_txplim_mtpl_q5),
        RF_TDSCDMA_TXPLIM_Q5_FRACTION(rf_tdscdma_core_txplim_mtpl_q5)); /*lint !e502 */
    }
    
  }
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
   This function adjusts the Max tx power limit(MTPL) based on the freq comp and temp comp.  
   This is used when start Tx, to fill the MTPL init value in the buffer(f1)
   It is called after freq comp and temp comp is done, and and has been reflected in txlim_desired 
    
   It is currently used before power limiting is enabled, to make sure the MTPL is 
   freq/temp compensated.

  @details
*/
void rf_tdscdma_core_txplim_adjust_mtpl_initial(void)
{
  rf_tdscdma_core_txplim_mtpl_q5 = RF_SATURATE( RF_TDSCDMA_TXPLIM_INT_TO_Q5(rf_tdscdma_core_txplim_desired),
                                                RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_TX_AGC_MIN),
                                                RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_TX_PWR_LIM_MAX)); /*lint !e734 */
    
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
   This function adjusts the Max tx power limit(MTPL) based on the freq comp and temp comp.  
   This is used when start BHO, to fill the MTPL init value in the buffer(f3)
   It is called after freq comp and temp comp is done
    
   Probably should just use the desired_limit on f3

  @details
*/
void rf_tdscdma_core_txplim_adjust_mtpl_bho(void)
{
#if 0
  int16 mtpl_diff;

  mtpl_diff = rf_tdscdma_core_txlim_vs_freq_val + rf_tdscdma_core_txlim_vs_temp_val_delta;

  /* update the differance */
  rf_tdscdma_core_txplim_mtpl_q5 += RF_TDSCDMA_TXPLIM_INT_TO_Q5(mtpl_diff); /*lint !e734 */
  rf_tdscdma_core_txplim_mtpl_q5 = RF_SATURATE( rf_tdscdma_core_txplim_mtpl_q5, 
                                                RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_TX_AGC_MIN), 
                                                RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_TX_PWR_LIM_MAX));
#endif

  rf_tdscdma_core_txplim_mtpl_q5 = RF_SATURATE( RF_TDSCDMA_TXPLIM_INT_TO_Q5(rf_tdscdma_core_txplim_desired),
                                                RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_TX_AGC_MIN),
                                                RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_TX_PWR_LIM_MAX)); /*lint !e734 */

}


/*--------------------------------------------------------------------------*/
/*!
   @brief
                                
   @details
                                 ACQ mode
             |------<------|-----------------
    TRK mode |             |
   ----------|------>------| 
        acq2trk_tol    trk2acq_tol
           (1dB)         (2dB)
   ----------ABS(Limit_Error)-------------->   
*/

LOCAL void rf_tdscdma_core_txplim_update_mode(uint16 abs_limit_error)
{

  /* Spec Voilation: Move to ACQ immediately */
  if(rf_tdscdma_core_txplim_hdet > RF_TDSCDMA_TXPLIM_TX_SPEC_LIMIT)
  {
    rf_tdscdma_core_txplim_mode = RF_TDSCDMA_TXPLIM_MODE_ACQ;
    rf_tdscdma_core_txplim_gain =  4;                 /* Filter Gain */
    rf_tdscdma_core_txplim_interval_update(2); /* HDET Timer */  
  }
  /* Initial ACQ Counter */
  else if((rf_tdscdma_core_txplim_counter < RF_TDSCDMA_TXPLIM_INIT_ACQ_COUNT) &&
    ((rf_tdscdma_core_txplim_txagc-1) <= RF_TDSCDMA_TXPLIM_Q5_TO_INT(rf_tdscdma_core_txplim_mtpl_q5)) &&
    ((rf_tdscdma_core_txplim_txagc+1) >= RF_TDSCDMA_TXPLIM_Q5_TO_INT(rf_tdscdma_core_txplim_mtpl_q5)) &&
    (abs_limit_error > RF_TDSCDMA_TXPLIM_INT_TO_Q5(1)) )  /* >0.1 db error */
  {
    rf_tdscdma_core_txplim_mode = RF_TDSCDMA_TXPLIM_MODE_ACQ;
    rf_tdscdma_core_txplim_gain =  4;                 /* Filter Gain */          
    rf_tdscdma_core_txplim_interval_update(2); /* HDET Timer */     
  }
  /* ACQ -> TRK mode transistion */
  else if((rf_tdscdma_core_txplim_mode == RF_TDSCDMA_TXPLIM_MODE_ACQ) && 
    (abs_limit_error < RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_ACQ2TRK_TOLERANCE)) &&
    (rf_tdscdma_core_txplim_counter > 10) )
  {
    rf_tdscdma_core_txplim_mode = RF_TDSCDMA_TXPLIM_MODE_TRK;
    rf_tdscdma_core_txplim_gain = 32;                  /* Filter Gain */          
    rf_tdscdma_core_txplim_interval_update(20); /* HDET Timer */ 
  }
  /* TRK -> ACQ mode transistion */
  else if((rf_tdscdma_core_txplim_mode == RF_TDSCDMA_TXPLIM_MODE_TRK) && 
    (abs_limit_error > RF_TDSCDMA_TXPLIM_INT_TO_Q5(RF_TDSCDMA_TXPLIM_TRK2ACQ_TOLERANCE)))
  {
    rf_tdscdma_core_txplim_mode = RF_TDSCDMA_TXPLIM_MODE_ACQ;
    rf_tdscdma_core_txplim_gain =  4;                 /* Filter Gain */          
    rf_tdscdma_core_txplim_interval_update(2); /* HDET Timer */     
  }
  /* Previous mode is not ACQ or TRK */
  else if(!( rf_tdscdma_core_txplim_mode == RF_TDSCDMA_TXPLIM_MODE_TRK || rf_tdscdma_core_txplim_mode == RF_TDSCDMA_TXPLIM_MODE_ACQ ))
  {
    if(abs_limit_error < RF_TDSCDMA_TXPLIM_INT_TO_Q5(3))   /* <0.3db error */
    {
      rf_tdscdma_core_txplim_mode = RF_TDSCDMA_TXPLIM_MODE_TRK;
      rf_tdscdma_core_txplim_gain = 32;                  /* Filter Gain */          
      rf_tdscdma_core_txplim_interval_update(20); /* HDET Timer */ 
    }
    else
    {
      rf_tdscdma_core_txplim_mode = RF_TDSCDMA_TXPLIM_MODE_ACQ;
      rf_tdscdma_core_txplim_gain =  4;                 /* Filter Gain */          
      rf_tdscdma_core_txplim_interval_update(2); /* HDET Timer */     
    }
  }
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function updates Tx_lim frequency offset data.

  @detail
  This function interpolates frequency compensation items from the channel
  number. And, applies the interpolation results to all addected frequency
  compensation items.

  @param RF primary or secondary path
  @param band Operational TDSCDMA band
  @param channel Operational channel
*/
void rf_tdscdma_core_txplim_update_lim_vs_freq
(
  //rf_path_enum_type path,
  rfcom_tdscdma_band_type band,
  uint16 channel,
  uint8 buf_idx
)
{
  int16 tx_start_bin = 0;   /* starting TX cal channel for bin containing */
  int32 tx_bin_size;        /* number of channels in the bin (not constant) */
  int32 tx_delta_x;         /* scale of channel number in bin */
  int16 delta_y;            /* used to store delta_y between adjacent nv items */
  boolean tx_channel_under = 0; /* flag if lowest TX cal channel > tune value */
  uint32 tx_freq;
  boolean out_of_range= FALSE;
  tx_band_cal_type* tx_cal_data = rf_tdscdma_core_txlin_get_band_data(band);

  if (tx_cal_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,"RFTXPLIM-FreqComp:Null pointer for Tx Cal data!",0);
    return;
  }


  tx_freq = rf_tdscdma_core_util_get_freq_from_uarfcn( channel, band );

  /* only handle TX items for path 0 */
  //if ( path == RF_PATH_0 )
  //{
    /* check your local channel against the 1st (0th) element of the array.
       if it's smaller than the first element then we use the lim_vs_freq
       value of the first element.  If it's not smaller than the first element
       then we start traversing the array to find out which bin it belongs to.*/

    if ( tx_freq <= tx_cal_data->tx_cal_freq[0] )
    {
      tx_start_bin = 0;
      tx_channel_under ++;   /* set flag if we are under the cal table
                                range flatline the value. */
    }

    /* search for frequency bin that contains the channel
       that is being compensated
       or if the bin contains zero we went off the table, use the last value */

    /* supports varying bin size, also used for detecting going past last
       cal point, when < 16 cal channels and remaining channel list stuffed
       with 0's. (negative bin size) */
    else
    {
      out_of_range = FALSE;
      while ( tx_start_bin < (RF_TDSCDMA_MAX_CAL_CHAN-1) )
      {
        /* note: tables with less than 16 elements in them must be
           padded with 0's */

        if ( tx_cal_data->tx_cal_freq[tx_start_bin+1] == 0 )
        {
          out_of_range = TRUE;
          break;
        }
        else if ( tx_freq <=
                  tx_cal_data->tx_cal_freq[tx_start_bin +1] )
        {
          break;            /* exit*/
        }
        else
        {
          tx_start_bin ++;     /* check next bin*/
        }
      }
    }
    /* if you've traversed through the entire array already (tx_start_bin ==15)
       then just set the bin size to 0 so that you will just use the lim_vs_freq
       value of the last element, rather than calculating it */

    if ( tx_start_bin == (RF_TDSCDMA_MAX_CAL_CHAN -1) || out_of_range )
    {
      tx_bin_size = 0;
    }
    else
    {
      tx_bin_size=(int32)(tx_cal_data->tx_cal_freq[tx_start_bin+1]-
                  tx_cal_data->tx_cal_freq[tx_start_bin]);
    }
    /* Load the comp items with the end point values if we are outside the
       cal channel list
       else the second statement when we interpolate.
       also avoid divide by zero error if two bins have the same channel #
       (we divide by bin size) */

    /* These are the TX items */

    /* outside the array case */

    if ( (tx_bin_size <= 0) || (tx_channel_under == 1) )
    {

      /* update tx lim vs freq */
      rf_tdscdma_core_txlim_vs_freq_val =
        (int8)rfnv_tdscdma_tx_tbl_ptr->tx_limit_vs_freq[tx_start_bin];

    }
    else /* we're within our cal chan list, do the interpolation calculation */
    {
      /*  ensures that dx/binsize <1 at bin boundary, when delta_y = 1. */
      /* delta_x is the distance from your local channel to the beginning of the
         bin, -1 */
      tx_delta_x = (int32)((tx_freq - tx_cal_data->tx_cal_freq[tx_start_bin]) - 1);

      /* update LIM_VS_FREQ items */

      /* delta_y is the difference between the lim_vs_freq values
         of the bin edges to the left and right of our tx frequency */

      /*lint -e{661} bound check is done thru tx_bin_size which would be 0 if tx_start_bin were >= RF_TDSCDMA_MAX_CAL_CHAN -1*/
      delta_y = (rfnv_tdscdma_tx_tbl_ptr->
                 tx_limit_vs_freq[tx_start_bin+1]-
                 rfnv_tdscdma_tx_tbl_ptr->tx_limit_vs_freq[tx_start_bin]);

      (delta_y<0)?delta_y--:delta_y++;

      rf_tdscdma_core_txlim_vs_freq_val = (int8)((delta_y*tx_delta_x/tx_bin_size) +
                                          rfnv_tdscdma_tx_tbl_ptr->
                                          tx_limit_vs_freq[tx_start_bin]);


    } /* interpolate case */
  //} /* end PATH 0 - tx items */

  /* update desired limit upon freq change */
  rf_tdscdma_core_txplim_update_desired_limit();

  if(rf_tdscdma_core_txplim_dbgmsg)
  { 
    MSG_3(MSG_SSID_RF, MSG_LVL_HIGH,"RFTXPLIM-FreqComp:%05d,Channel:%d,LimVsFreq:%d",
      rf_tdscdma_core_txplim_counter,
      channel,
      rf_tdscdma_core_txlim_vs_freq_val);
  }
  
  #if 0 // Comment out for MDSP/SMEM cleanup
  rf_tdscdma_mdsp_update_dyn_params((int16)RF_PATH_0,
                                    (int16)RF_TDSCDMA_MDSP_DYN_TX_CHAN_FREQ_UPDATE_ID,
                                    RF_TDSCDMA_MDSP_TX_BUF_TYPE,
                                    (void*)(&tx_freq),
                                    TRUE,
                                    buf_idx); 
  #endif
}


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details

*/
void rf_tdscdma_core_txplim_temp_val_update
(
  int16 tx_lim_vs_temp_val, 
  int16 utran_tx_lim_vs_temp_offset_val,
  uint8 temp_index,
  uint8 temp_rem
)
{

  /* tx_lim_vs_temp_val is a delta value */
  rf_tdscdma_core_txlim_vs_temp_val = rfnv_tdscdma_tx_tbl_ptr->max_tx_power + tx_lim_vs_temp_val; 
  rf_tdscdma_core_txlim_utran_vs_temp_offset_val = utran_tx_lim_vs_temp_offset_val;

  /* update desired limit upon temp update */
  rf_tdscdma_core_txplim_update_desired_limit();

  rf_tdscdma_core_txlim_vs_temp_val_delta = tx_lim_vs_temp_val;

  if(rf_tdscdma_core_txplim_dbgmsg)
  { 
    MSG_5(MSG_SSID_RF, MSG_LVL_HIGH,"RFTXPLIM-TempComp:%05d,TempIndex:%d,TempRem:%d,UtranTempOffset:%d,LimVsTemp:%d",
      rf_tdscdma_core_txplim_counter,
      temp_index,temp_rem,
      rf_tdscdma_core_txlim_utran_vs_temp_offset_val,
      rf_tdscdma_core_txlim_vs_temp_val);
  }
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function will reset the limiting loop to acquisition mode.

  @details
  It is intended to be called after a handover to improve the loop
  convergence time.

*/
void rf_tdscdma_core_txplim_reset_to_acq_mode(void)
{
  rf_tdscdma_core_txplim_counter = 0;
  rf_tdscdma_core_txplim_mode = RF_TDSCDMA_TXPLIM_MODE_ACQ;
  rf_tdscdma_core_txplim_gain =  4;          /* Filter Gain */
  rf_tdscdma_core_txplim_interval_update(2); /* HDET Timer */     
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function will set the thermal mitigation backoff value. This backoff is
  applied in addition to any other current backoffs.

  @details
  The argument backoff is a positive number x, in that passing x will result
  in the power limit being reduced by x dB

*/
void rf_tdscdma_core_txplim_reduce_mptl_db_for_therm(uint8 backoff)
{

  /* Range checking - do not allow backoff to take the limit below RF_TDSCDMA_TXPLIM_THERMAL_PMIN_DB */
  backoff = MIN(backoff, MAX((RF_TDSCDMA_TXPLIM_Q5_TO_INT(rf_tdscdma_core_txplim_mtpl_q5)/10 - 70)
                              - RF_TDSCDMA_TXPLIM_THERMAL_PMIN_DB,0));


  /* keep local variable in standard units, i.e. negative value indicates backoff */
  rf_tdscdma_core_txplim_thermal_backoff_db10 = -1*10*backoff;

  /* Set ACQ mode to converge quickly to new backoff level */
  rf_tdscdma_core_txplim_reset_to_acq_mode();

  /* update desired limit */
  rf_tdscdma_core_txplim_update_desired_limit();

  /* Adjust the current MTPL to account for the case where the hdet
   * loop is not running.
   */
  rf_tdscdma_core_txplim_mtpl_q5  += RF_TDSCDMA_TXPLIM_INT_TO_Q5(rf_tdscdma_core_txplim_thermal_backoff_db10 - 
                                                     rf_tdscdma_core_txplim_thermal_backoff_prev_db10);

  rf_tdscdma_core_txplim_thermal_backoff_prev_db10 = rf_tdscdma_core_txplim_thermal_backoff_db10;

  #if 0 // Comment out for MDSP/SMEM cleanup
  /* update the mtpl to mdsp smem - this is necessary when hdet loop is not running */
  if( rf_tdscdma_core_txplim_enabled )
  {
    /* Update mtpl to mdsp (f1) */
    rf_tdscdma_core_txplim_update_mtpl_to_mdsp(rf_tdscdma_core_txplim_get_mtpl_backoff(), 
                                               FALSE,
                                               rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx);
  }
  #endif
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function will set the coexistence MTPL.

  @details

*/
void rf_tdscdma_core_txplim_set_coex_limit(int16 sar_tx_pwr_limit, int16 coex_tx_pwr_limit)
{
  //int16 min_limit;
  int16 txplim_desired_prev = rf_tdscdma_core_txplim_desired;

  /* Get the min of the 2 limits */
  //min_limit = MIN(sar_tx_pwr_limit,coex_tx_pwr_limit);

  /* Convert SAR limit to dB10 AGC unit */
  rf_tdscdma_core_txplim_sar_limit_db10 = sar_tx_pwr_limit - RF_TDSCDMA_MIN_TX_POWER_DBM*RF_TDSCDMA_TXAGC_RESOLUTION;

  /* Convert coex limit to dB10 AGC unit */
  rf_tdscdma_core_txplim_coex_limit_db10 = coex_tx_pwr_limit - RF_TDSCDMA_MIN_TX_POWER_DBM*RF_TDSCDMA_TXAGC_RESOLUTION;
  
  /* Set ACQ mode to converge quickly to new backoff level */
  rf_tdscdma_core_txplim_reset_to_acq_mode();

  /* update limits */
  rf_tdscdma_core_txplim_update_desired_limit();

  /* Adjust the current MTPL to account for the case where the hdet
   * loop is not running.
   */
  rf_tdscdma_core_txplim_mtpl_q5  += RF_TDSCDMA_TXPLIM_INT_TO_Q5(rf_tdscdma_core_txplim_desired - txplim_desired_prev);

  rf_tdscdma_core_txplim_mtpl_q5_coex  = RF_TDSCDMA_TXPLIM_INT_TO_Q5(rf_tdscdma_core_txplim_desired_coex);

  #if 0 // Comment out for MDSP/SMEM cleanup 
  /* update the mtpl to mdsp smem - this is necessary when hdet loop is not running */
  if( rf_tdscdma_core_txplim_enabled )
  {
    /* Update mtpl to mdsp (f1) */
    rf_tdscdma_core_txplim_update_mtpl_to_mdsp(rf_tdscdma_core_txplim_get_mtpl_backoff(), 
                                               FALSE,
                                               rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx);
  }
  #endif

  MSG_5( MSG_SSID_RF, MSG_LVL_LOW,"rf_tdscdma_core_txplim_set_coex_limit:-UTPL:%03d,Desired:%03d,Previous:%03d,MTPL:%03d.%02d",
    rf_tdscdma_core_txplim_utpl,
    rf_tdscdma_core_txplim_desired,
    txplim_desired_prev,
    /* MTPL Integer part */
    RF_TDSCDMA_TXPLIM_Q5_TO_INT(rf_tdscdma_core_txplim_mtpl_q5),
    /* MTPL Fraction part */
    RF_TDSCDMA_TXPLIM_Q5_FRACTION(rf_tdscdma_core_txplim_mtpl_q5));
 
  MSG_4( MSG_SSID_RF, MSG_LVL_LOW,"rf_tdscdma_core_txplim_set_coex_limit:-CoexL(dB10):%03d,Desired Coex:%03d,Coex MTPL:%03d.%02d",
    rf_tdscdma_core_txplim_coex_limit_db10,
    rf_tdscdma_core_txplim_desired_coex,
    /* Coex MTPL Integer part */
    RF_TDSCDMA_TXPLIM_Q5_TO_INT(rf_tdscdma_core_txplim_mtpl_q5_coex),
    /* Coex MTPL Fraction part */
    RF_TDSCDMA_TXPLIM_Q5_FRACTION(rf_tdscdma_core_txplim_mtpl_q5_coex)); 
    
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function will reset the coexistence limit. 

  @details

*/
void rf_tdscdma_core_txplim_reset_coex_limit(void)
{
  rf_tdscdma_core_txplim_coex_limit_db10 = RF_TDSCDMA_NV_TXAGC_MAX;
}



/*--------------------------------------------------------------------------*/
/*!
  @brief
  This func is called to update the hdet reading trigger interval to mdsp 

  @details

*/
void rf_tdscdma_core_txplim_interval_update(int16 interval)
{
  int16 hdet_threshold;
  rf_tdscdma_ftm_burst_tx_params_type ftm_rf_mode_tx_params;
  /* save the current interval */
  rf_tdscdma_core_txplim_hdet_interval = interval;

  /* Send HDET_CONFIG message to FW */

  /* 15dbm in txagc db10 unit */
  hdet_threshold = RF_TDSCDMA_TXPLIM_TX_PWR_LIM_MIN;
#if 0
  hdet_threshold = 1100;    //Give 40dbm so fw will never trigger
#endif

  /* skip hdet reporting when there is no meas */
  if(IS_FTM_IN_TEST_MODE() && (!rfm_get_calibration_state()))
  {
    ftm_rf_mode_tx_params = ftm_tdscdma_get_rf_mode_tx_params();
    /* Note: hdet_threshold format for this MSGR command is: (hdet_threshold_in_dBm + 70) * 10 */
    rf_tdscdma_mdsp_config_hdet((ftm_rf_mode_tx_params.pdet_thresh + 700), TRUE, ftm_rf_mode_tx_params.pdet_trig_interval);
  }
  else
  {
  rf_tdscdma_mdsp_config_hdet(hdet_threshold, TRUE, interval);
}
}


#if 0 // Comment out for MDSP/SMEM cleanup
/*--------------------------------------------------------------------------*/
/*!
  @brief
  This func is called to update the mtpl value to mdsp shared memory 

  @details

*/
void rf_tdscdma_core_txplim_update_mtpl_to_mdsp(int16 mtpl, 
                                                boolean init_flag,
                                                uint8 buf_idx)
{

  /* Load the mdsp value to MDSP shared memory */
  rf_tdscdma_mdsp_update_dyn_params((int16)RF_TDSCDMA_MDSP_CHAIN_0,
                                    (int16)RF_TDSCDMA_MDSP_DYN_MAX_PWR_UPDATE_ID,
                                    (int16)RF_TDSCDMA_MDSP_TX_BUF_TYPE,
                                    (void*)&mtpl,
                                    init_flag,
                                    buf_idx); 
}
#endif

/*--------------------------------------------------------------------------*/
/*!
  @brief
  This func is called to get the current hdet reading trigger interval 

  @details

*/
uint16 rf_tdscdma_core_txplim_get_hdet_interval(void)
{
  return rf_tdscdma_core_txplim_hdet_interval;
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This func does interpolation to get the txagc value corresponding to
  the hdet adc count 

  @details

*/
uint16 rf_tdscdma_core_txplim_convert_hdet_adc_to_txagc(uint16 hdet_adc)
{
  int16 index = 0;
  uint16 txagc = 0;
  uint16 *exp_hdet_vs_agc_ptr = rfnv_tdscdma_tx_tbl_ptr->exp_hdet_vs_agc;

  /* find the bin of hdet_adc */
  for(index=0; index<RF_TDSCDMA_HDET_VS_AGC_SIZ; index++)
  {
    if(hdet_adc < exp_hdet_vs_agc_ptr[index])
    {
      break;
    }
  }

  /* hdet_adc smaller than smallest value in exp_hdet_vs_agc table, set to lowest value */
  if(index == 0)
  {
    /* the 16 txagc value are 768, 784, ...., 1008 */
    /* txagc=768 */
    txagc = RF_TDSCDMA_TXPLIM_TX_AGC_VS_HDET_TBL_OFF;
  }
  /* hdet_adc larger than largest value in expected_hdet_vs_AGC table, set to highest value */
  else if(index == RF_TDSCDMA_HDET_VS_AGC_SIZ)
  {
    /* txagc=1008 */
    txagc = RF_TDSCDMA_TXPLIM_TX_AGC_VS_HDET_TBL_OFF + (RF_TDSCDMA_HDET_VS_AGC_SIZ-1)*RF_TDSCDMA_TXPLIM_EXP_HDET_VS_AGC_STEP_SIZ;
  }
  /* Linear interpolation with rounding */
  else
  {
    txagc = RF_TDSCDMA_TXPLIM_TX_AGC_VS_HDET_TBL_OFF + (index-1)*RF_TDSCDMA_TXPLIM_EXP_HDET_VS_AGC_STEP_SIZ;
    txagc += ( (hdet_adc - exp_hdet_vs_agc_ptr[index-1])*RF_TDSCDMA_TXPLIM_EXP_HDET_VS_AGC_STEP_SIZ + (exp_hdet_vs_agc_ptr[index] - exp_hdet_vs_agc_ptr[index-1])/2 )
               /(exp_hdet_vs_agc_ptr[index] - exp_hdet_vs_agc_ptr[index-1]);
  }

  return txagc;
}

