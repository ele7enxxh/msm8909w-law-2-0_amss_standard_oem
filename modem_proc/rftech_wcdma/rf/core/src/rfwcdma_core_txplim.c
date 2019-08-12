/*!
  @file
  rfwcdma_core_txplim.c

  @brief
  Provides TX Power liming loop algorithm.

  @details
*/

/*===========================================================================

Copyright (c) 2010 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/core/src/rfwcdma_core_txplim.c#1 $

when          who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/18/16   sk      Initialize the MTPL every Tx Config
05/26/16   sub     RF F3 cleanup
12/17/15   ag      Release meter lock for hdet in SW callback if Tx is disabled before
07/01/15   zhh     take the mtpl_adj_db10 into the mtpl calc
03/12/15   gh      Add timing information MSG for METER read
01/27/15   rmb     Add api to fetch MTPL in dBm10 units. 
03/09/14   ag     Skip Hdet reading in W2G gap
11/17/14   vbh     Update txplim_get_mtpl_nv_item to accept band as a param
09/17/14   gh      Add support for FBRx ILPC based HDET operation
03/09/14   ag     Skip Hdet reading in W2G gap
08/15/14   zhh    populate the new MPR to DM in rfwcdma_core_get_tx_mpr_param()
06/02/14   kr      Fix MTPL overshoot issue
03/20/14   zhh     update new MPR backoff NV
07/01/14   aro     Collision Prevention between TxPlim and TempComp
05/15/14   aro     Deleted unused interfaces
05/15/14   aro     Support for new Power Meter Loop
03/31/14   zhh    Add Tx state check before starting MTPL
02/24/14   rmb     Fix OFT Compilation Error.
02/21/14   rmb     Add global NV pointers for each of band specific static NVs.
08/17/12   ry      Fix Max Tx power is abnormal during Thermal mitigation on WCDMA
05/17/13   kcj     Enable multi-TX SAR limit 
03/20/13   cd      XPT Temp comp support
03/19/13   dw/kai  [1]Added support for HDET based linearizer compensation 
                   [2]Added support to limit allowable HDET mod range 
02/14/13   ac      feature and code cleaupup related to tdet, common api 
02/01/13   ac      added common api for hdet read
12/05/12   tks     Fixed the call to hdet read device interface 
11/15/12   ac      replacing with the api to support buffer format, compiler warning fix
11/15/12   ka      Initial support for DCHSUPA
10/15/12   kai     Changed QFE1510 HDET APIs
09/26/12   kai     Changed name of K sensor comp apis to ant common api
08/02/12   kai     Add QFE1510 HDET and K val comp support for W online mode
09/28/12   ac      device interface to pass band to rfc_wcdma_get_device
03/27/12   dw      Fix compiler warning
03/27/12   dw      Fix max power issue that NV limit was not initalized 
                   properly until temp comp kicks in
03/02/12   kai     Optimize local function rfwcdma_core_txplim_calc_exp_agc_vs_hdet_table() 
03/02/12   kai     Add local function rfwcdma_core_txplim_binary_search()
02/17/12   aa      Initialize SAR limit to max value to prevent undesired MTPL saturation 
02/16/12   swb     Fixed issues with MTPL update in SAR limiting function
02/13/12   swb     RFTXPLIM 2.5 - Faster error convergence
01/25/12   dw      Fix KW error
01/23/11   dw      Update FW MTPL variable after RF do MTPL in FTM non-Cal mode
12/08/11   dw      Fix rfwcdma_core_txplim_reset_to_acq_mode and force MTPL to ACQ mode 
                   for 100 interations after handover
10/31/11   dw      Max power freq comp fix for Cal V3
10/25/11   ac/ka   Integrate pa thresh offset fix  
10/25/11   ac/ka   Add NULL pointer check in rfwcdma_core_txplim_do_vbatt_based_backoff()  
09/08/11   ac      SAR support
05/12/11   dw      Initialize MPR table 
03/09/11   sar     Removed deprecated file clk.h.
02/08/11   ka      Fix thermal mitigation range checking 
02/03/11   ad      Fix gradual MTPL decrement while processing UTPL updates during a call
01/27/11   ka      Added support for thermal mitigation 
01/12/10   ad      Fix overflow in MTPL variable  
10/19/10   vb      All device functions are now accessed thru device interface functions 
10/13/10   dw      add rfwcdma_core_txplim_get_mode()
10/04/10   ka      Added rfwcdma_core_txplim_reset_to_acq_mode() 
08/17/10   ka      Back out hdet change 
08/09/10   dw      Fix crash in populating agc_vs_hdet table
08/04/10   ac      Improve exp_hdet_vs_agc interpolation to reduce overall enable_tx execution time
07/19/10   ka      Removed INTLOCK 
06/22/10   ad      Remove limiting min. MTPL to +15dBm. New min. MTPL is -70dBm.
05/06/10   ka      Enable HDET for MDM9K. 
03/16/10   kguo    Merged from scmm_final_merge branch
03/10/10   ka      Additional temporary hdet disable for MDM9K.
02/12/10   ka      Remove direct mdsp access from core layer.
01/12/10   xw      Changed ABS macro to abs() in the C standard library. 
12/23/09   bn      PA MPR NV feature support
12/10/09   bn      FTM WCDMA ENH log packet
12/05/09   jfc     Lint fix: Suppress warning since lint doesn't detect the bound check
09/22/09   bn      RFTXPLIM 2.0 - New Tx power limiting for UMTS targets
08/13/09   jhe     freq comp change
07/07/09   bn      TxAGC dB10 changes
05/28/09   ckl     Changed function name to rfwcdma_core_txplim_do_tx_pwr_limit( )
03/17/09   ckl     Comment out QDSP6 interface
03/15/09   ckl     Deleted rfnv.h
03/09/09   ckl     Code cleanup
11/06/08   av      Initial version.

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfwcdma_core_txplim.h"
#include "rfcommon_nv_mm.h"
#include "rfnv_wcdma.h"
#include "rex.h"
#include "nv.h"
#include "err.h"
#include "msg.h"
#include "rfwcdma_core_txlin.h"
#include "rfwcdma_core_util.h"
#include "rfumts.h"
#include "rfwcdma_mdsp.h"
#include "rfdevice_wcdma_intf.h"
#include "math.h"
#include "rfm_common_sar.h"
#include "ftm.h"
#include "rfm_internal.h"
#include "rfwcdma_core_temp_comp.h"
#include "rfdevice_hdet_wcdma_intf.h"
#include "rfdevice_hdet_cmn_intf.h"
#include "rfwcdma_mdsp_async.h"
#include "rfwcdma_power_limiting.h"
#include "rfwcdma_data.h"
#include "rf_cmd_interface.h"
#include "rflm_wcdma_msg.h"

/*===========================================================================

                           CONSTANT DEFINITIONS

===========================================================================*/

/* min for TX pwr limit software - 15dbm => (15 + 70) * 10 */
#define RFTXPLIM_TX_PWR_LIM_MIN           850

/* max for TX pwr limit software */
#define RFTXPLIM_TX_PWR_LIM_MAX           RF_NV_TXAGC_MAX-1

/* TX Spec Limit for Power Class 3 - 25dbm => (25 + 70) * 10 */
#define RFTXPLIM_TX_SPEC_LIMIT            950

/* 50 => 5dBm */
#define RFTXPLIM_FILT_LIMIT_ERR_MAX       50
#define RFTXPLIM_TXAGC_ERROR_ACCUM_MAX    50

/* min TX AGC value */
#define RFTXPLIM_TX_AGC_MIN               RF_NV_TXAGC_MIN

/* max Tx AGC value (with offset) */
#define RFTXPLIM_TX_AGC_MAX               RF_NV_TXAGC_MAX

/* offset for the 256 element Tx AGC vs HDET table index */
#define RFTXPLIM_TX_AGC_VS_HDET_TBL_OFF   768

/* size of Tx AGC vs HDET interpolated table */
/* 12-bit HDET will require change in the array size */
#define RFTXPLIM_TX_AGC_VS_HDET_TBL_SIZ   4096

/* size of expected HDET vs AGC table, 256 */
#define RFTXPLIM_EXP_HDET_VS_AGC_TBL_SIZ \
            (RFTXPLIM_TX_AGC_VS_HDET_TBL_SIZ / NV_WCDMA_EXP_HDET_VS_AGC_SIZ)

/* index for top of HDET curve high zone, 15 */
#define RFTXPLIM_EXP_HDET_VS_AGC_MAX_IDX  (NV_WCDMA_EXP_HDET_VS_AGC_SIZ-1)

/**/
#define RFTXPLIM_EXP_HDET_VS_AGC_SIZ      NV_WCDMA_EXP_HDET_VS_AGC_SIZ

/* Tx AGC limit is in 1/10 dB */
#define RFTXPLIM_TX_PWR_LIM_CNT_PER_DB    RF_TXAGC_RESOLUTION

/* minimum value for TX power limit in dbm */
#define RFTXPLIM_MIN_TX_PWR_LIM           RF_MIN_TX_POWER_DBM

/* maximum value for TX power limit in dbm */
#define RFTXPLIM_MAX_TX_PWR_LIM           RF_UE_MAX_TX_POWER

/* default MAX TX power limit in SW unit = 930 = 23dbm */
#define RFTXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT RF_UE_MAX_TX_POWER_DB10

/* maximum possible hdet value */
#define RFTXPLIM_MAX_HDET 4095

/* amount to shift to translate between integer and 11Q5 values */
#define RFTXPLIM_Q5 5

#define RFTXPLIM_2Q5 32

#define RFTXPLIM_THERMAL_PMIN_DB 10

/* Macros to convert between integer and Q5 values */
#define RFTXPLIM_INT_TO_Q5(x) ((x) << RFTXPLIM_Q5)

#define RFTXPLIM_Q5_TO_INT(x) ((x) >> RFTXPLIM_Q5)

#define RFTXPLIM_Q5_INTEGER(x) ((x) >= 0) ? ((x) >> RFTXPLIM_Q5) : (~((~x+1) >> RFTXPLIM_Q5) + 1)

#define RFTXPLIM_Q5_FRACTION(x) ((x) >= 0) ? ((x) - RFTXPLIM_INT_TO_Q5(RFTXPLIM_Q5_TO_INT(x)))*100/RFTXPLIM_2Q5 \
                                           : ((~x+1) - RFTXPLIM_INT_TO_Q5(RFTXPLIM_Q5_TO_INT(~x+1)))*100/RFTXPLIM_2Q5

/* Value must be added to the SAR NV dB10 values to get sw agc units */
#define RFTXPLIM_SAR_NV_SCALE_FACTOR 700

/*===========================================================================

                           LOCAL VARIABLES DEFINITIONS

===========================================================================*/
/* flag to enable TX power limit */
boolean        rfwcdma_core_txplim_enabled = FALSE;

/* flag to indicate first pass of TX power limiting */
boolean        rfwcdma_core_txplim_first_time_flag = TRUE;

/* Tx Power limit loop filter gain */
static int8 rfwcdma_core_txplim_gain = 4;

/* Received HDET trigger counter */
static uint16 rfwcdma_core_txplim_counter = 0;

uint8 rfwcdma_core_txplim_init_acq_counter = 100;

rfwcdma_core_txplim_mode_type rfwcdma_core_txplim_mode = RFTXPLIM_MODE_DISABLED;

/* Global variable: HDET reported calibration error in dB10 */
/* HDET value - TxAGC_buffered */
int16  rfwcdma_core_txplim_cal_error = 0;

/* flag to enable RF Txplim debug messages */
boolean rfwcdma_core_txplim_dbgmsg = TRUE;

static int16 rfwcdma_core_txplim_backoff_limit = 0;

static rfcom_txplim_channel_type rfwcdma_core_txplim_chan_type = RFCOM_TXPLIM_DCH;

/* This value is the absolute TX power limit value based on temperature. */
/* It is NOT a delta value. */
/* will give it a default value */
LOCAL int16    rfwcdma_core_txlim_vs_temp_val = RFTXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT;
/* HDET vs tempreture value*/
LOCAL int16    rfwcdma_core_txlim_hdet_vs_temp = 0;
/* tx pwr limit frequency comp value */
/* HDET vs freq comp value*/
LOCAL int16    rfwcdma_core_hdet_vs_freq_val = 0;
/* Desire MTPL vs freq val */
LOCAL int16    rfwcdma_core_desired_lim_vs_freq_val = 0;

/* UTRAN TX power limit temperature compensation offset value*/
/* This value is a UTRAN TX power limit temperature compensation offset */
LOCAL int16    rfwcdma_core_txlim_utran_vs_temp_offset_val = 0;

/* last raw reading of HDET from ADC */
LOCAL int16    rfwcdma_core_txplim_hdet_adc;

/* last reading of HDET in dB10 units */
LOCAL int16    rfwcdma_core_txplim_hdet;

/* Accumlated Tx AGC error in Q5 format */
LOCAL int32    rfwcdma_core_txplim_tx_err_accum_q5 = 0;

/* Allowable HDET based Tx lin adjust lower and upper limits */
LOCAL int16    rfwcdma_core_txlim_txlin_adjust_limit[2];

/* last reading of TX_AGC value from MDSP */
/* this is 0..1023 while the firmware is -512..+511 min..max */
LOCAL int16    rfwcdma_core_txplim_txagc;

/* last software value for Max Tx power limit (MTPL) as an Q5 value */
/* this TxAGC value in 0..1023 min..max units shifted by 5 */
LOCAL uint32  rfwcdma_core_txplim_mtpl_q5 = RFTXPLIM_INT_TO_Q5(RFTXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT);

/* Tx power limit commanded by UTRAN */
/* this is 0..1023 min..max */
LOCAL int16  rfwcdma_core_txplim_utpl = RFTXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT;

/* TX power limit set by NV_WCDMA_TX_LIM_VS_TEMP */
/* this is 0..1023 min..max */
LOCAL int16  rfwcdma_core_txplim_ntpl = RFTXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT;
LOCAL int16  rfwcdma_core_txplim_lin = RFTXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT;
LOCAL int16  rfwcdma_core_txplim_desired = RFTXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT;
LOCAL int16  rfwcdma_core_txplim_desired_prev = RFTXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT;

/* pointer to expected HDET vs AGC table */
LOCAL uint16    *rfwcdma_core_txplim_exp_hdet_vs_agc_ptr;

/* Expected AGC (TX AGC Value - 768) for each HDET value 0..255 */
/* 768 is used to scale down the TX AGC value range of 768..1023 to */
/* a range of 0..255. why ? because TX power limiting is only done */
/* on the top 25% of TX AGC range */
LOCAL uint8    rfwcdma_core_txplim_exp_agc_vs_hdet[RFTXPLIM_TX_AGC_VS_HDET_TBL_SIZ];

/* Pointer to the the expected agc tables */
LOCAL uint8    *rfwcdma_core_txplim_exp_agc_vs_hdet_ptr = rfwcdma_core_txplim_exp_agc_vs_hdet;

/* MPR (power backoff) based beta scaling table */
/* Default values contains no scaling */
static uint16 rfwcdma_core_txplim_betaBackoff[RF_WCDMA_MAX_NUM_MPR_BINS] = 
  {32767, 29205, 27571, 26029, 24573, 23198, 21900};

/* MPR values applied*/
static uint8 rfwcdma_core_txplim_mprVals[RF_WCDMA_MAX_NUM_MPR_BINS] = 
  {0, 0, 5, 10, 15, 20, 25};

/* TXAGC offset */
static uint8 rfwcdma_core_txplim_txAgcOffset[RF_WCDMA_MAX_NUM_MPR_BINS] =
  {0, 10, 15, 20, 25, 30, 35};  

static uint8 rfwcdma_core_txplim_paThresholdOffset[RF_WCDMA_MAX_NUM_MPR_BINS - 1] = 
  {10,15, 20, 25, 30, 35};
  
static rf_max_tx_pwr_limit_type   rfwcdma_core_txplim_rel5_backoff;

static int16 rfwcdma_core_txplim_thermal_backoff_db10 = 0;
static int16 rfwcdma_core_txplim_thermal_backoff_prev_db10 = 0;
static uint16 rfwcdma_core_txplim_sar_limit_db10 = RFTXPLIM_TX_PWR_LIM_MAX;
static uint16 rfwcdma_core_txplim_multi_tx_sar_limit_db10 = RFTXPLIM_TX_PWR_LIM_MAX;

/*===========================================================================

                           LOCAL FUNCTIONS DECLARATIONS

===========================================================================*/

LOCAL void rfwcdma_core_txplim_set_desired_limit(void);

LOCAL void rfwcdma_core_txplim_adjust_mtpl(void);

LOCAL void rfwcdma_core_txplim_update_mode(uint16 abs_limit_error);


/*===========================================================================

                           FUNCTIONS DEFINITIONS

===========================================================================*/

/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Query if TxPlim is supported or not
 
  @details
  This function checks the plim_enable flag and HDET NV to determine if TxPlim is supoorted or not
 
  @param plim_data
  Pointer to data structure containing PLIM data
 
  @return
  Flag indicating, if TxPlim is supported or not
*/
static boolean
rfwcdma_core_txplim_is_supported
(
  rfwcdma_power_limiting_data_type *plim_data
)
{
  boolean is_supported = TRUE;
  uint8 nv_idx;

  if ( plim_data == NULL )
  {
    RF_MSG(RF_ERROR,"rfwcdma_core_txplim_is_supported: NULL PLIM Data");
    return FALSE;
  } /* if ( plim_data == NULL ) */

  is_supported &= plim_data->enable_plim; /* Enable Flag */  

  for ( nv_idx = 0; nv_idx < RFTXPLIM_EXP_HDET_VS_AGC_MAX_IDX; nv_idx++ ) 
  {
    if ( rfwcdma_core_txplim_exp_hdet_vs_agc_ptr[ nv_idx ] == 0 )
    {
      is_supported &= FALSE; /* Enable Flag */  
      break;
    } /* if ( rfwcdma_core_txplim_exp_hdet_vs_agc_ptr[ nv_idx ] == 0 ) */
  } /* for ( nv_idx = 0; nv_idx < RFTXPLIM_EXP_HDET_VS_AGC_MAX_IDX; nv_idx++ )  */

  RF_MSG_1( RF_MED, "rfwcdma_core_txplim_is_supported - Support Flag %d", is_supported );

  return is_supported;

} /* rfwcdma_core_txplim_is_supported */

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

  @param channel Tx channel

*/

void rfwcdma_core_txplim_init(rfcom_wcdma_band_type band,uint16 channel, uint32 multi_carrier_idx)
{
  rfwcdma_mdsp_set_data_param_type power_meter_config_param = { 0 };
  rfwcdma_mdsp_set_data_param_type set_periodicity_param = { 0 };
  rfwcdma_power_limiting_data_type *plim_data = NULL;

  plim_data = rfwcdma_get_power_limiting_data();

  if ( plim_data == NULL )
  {
    RF_MSG(RF_ERROR,"rfwcdma_core_txplim_init: NULL PLIM Data");
    return;
  }

  /* NV table pointer needs to be intialized */
  if (band < RFCOM_NUM_WCDMA_BANDS)
  {
    if (rfnv_wcdma_tbl_ptr[band] == NULL)
    {
      RF_MSG_1(RF_ERROR,
               "rfwcdma_core_txplim_init: NULL NV tbl ptr for band %d",
               band);
      return;
    }
  }

  else
  {
    RF_MSG_1(RF_ERROR,
               "rfwcdma_core_txplim_init: Not a valid band %d",
               band);
    return;
  }

  rfwcdma_core_txplim_exp_hdet_vs_agc_ptr  = rfnv_wcdma_tbl_ptr[band]->exp_hdet_vs_agc_v2;

  /* Populate MDSP paramters */
  power_meter_config_param.set_data_cmd = RFWCDMA_MDSP_SET_POWER_METER_ENABLE_FLAG;
  power_meter_config_param.rflm_handle = (rflm_dm_handle_id_t)rfwcdma_mc_state.txlm_buf_idx;
  power_meter_config_param.data.gen_b = rfwcdma_core_txplim_is_supported(plim_data); /*Enable Flag*/
  rfwcdma_mdsp_async_set_data(&power_meter_config_param);

  /* Populate MDSP paramters */
  set_periodicity_param.set_data_cmd = RFWCDMA_MDSP_SET_POWER_METER_PERIODICITY;
  set_periodicity_param.rflm_handle = (rflm_dm_handle_id_t)rfwcdma_mc_state.txlm_buf_idx;
  set_periodicity_param.data.gen_dw = 10000; /* HDET Timer = 10ms */  
  rfwcdma_mdsp_async_set_data(&set_periodicity_param);

  rfwcdma_core_txplim_load_mpr_backoff(band, multi_carrier_idx);

  /* Update Lim vs Freq */
  rfwcdma_core_txplim_update_lim_vs_freq(RF_PATH_0,band,channel);

  /* Init Lim vs Temp NV limit */
  rfwcdma_core_temp_txplim_init(band);

  /* disable TX power limiting initially */
  rfwcdma_core_txplim_enable(FALSE);

  /* Reset MTPL to ACQ mode */
  rfwcdma_core_txplim_reset_to_acq_mode();
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
   This function set the Max tx power limit(MTPL) based on a new UTRAN TX power limit value 
   which is specified in dbm unit.

   @details


*/
void rfwcdma_core_txplim_set_mtpl( int16 utran_limit_dBm, rfcom_txplim_channel_type chan_type)
{
  uint8 i = 0;
  rfwcdma_mdsp_set_data_param_type mpr_data = { 0 };
  int16 old_limit;
  int16 mtpl_diff;
  int16 utran_limit_db10;
  utran_limit_db10 = (utran_limit_dBm-RF_MIN_TX_POWER_DBM) * RF_TXAGC_RESOLUTION; /*lint !e734 */

  /* Store the old limit */
  old_limit = rfwcdma_core_txplim_desired;

  /* UTRAN Tx Power Limit (UTPL) */
  rfwcdma_core_txplim_utpl = utran_limit_db10;

  /* Channel Type RACH or DCH */
  rfwcdma_core_txplim_chan_type = chan_type;

  /* Update Desired Limit */
  rfwcdma_core_txplim_set_desired_limit();
 
  /* Max Tx Power Limit (MTPL) */
  if(rfwcdma_core_txplim_first_time_flag)
  {
    rfwcdma_core_txplim_mtpl_q5 = RF_SATURATE( RFTXPLIM_INT_TO_Q5(rfwcdma_core_txplim_desired),
                                            RFTXPLIM_INT_TO_Q5(RFTXPLIM_TX_AGC_MIN),
                                            RFTXPLIM_INT_TO_Q5(RFTXPLIM_TX_PWR_LIM_MAX)); /*lint !e734 */
    
    /* not the first time anymore */
    rfwcdma_core_txplim_first_time_flag = FALSE;

    /* Reset previous desired MTPL */
    rfwcdma_core_txplim_desired_prev = rfwcdma_core_txplim_desired;
  }
  else
  {
    /* To account for scenairo where an UTRAN update happens during a call (stablized MTPL loop) */ 
    /* don't overwrite current power limit with uncorrected utran limit, just update the differance */
    mtpl_diff = rfwcdma_core_txplim_desired - old_limit;
    rfwcdma_core_txplim_mtpl_q5 += RFTXPLIM_INT_TO_Q5(mtpl_diff); /*lint !e734 */
    rfwcdma_core_txplim_mtpl_q5 = RF_SATURATE( rfwcdma_core_txplim_mtpl_q5, 
                                          RFTXPLIM_INT_TO_Q5(RFTXPLIM_TX_AGC_MIN), 
                                          RFTXPLIM_INT_TO_Q5(RFTXPLIM_TX_PWR_LIM_MAX));
  }
  
  /* MDSP writes: MPR (power backoff) beta scaling table */
  for ( i=0; i < RFTXPLIM_MPR_BETA_TABLE_ENTRIES; i++ )
  {
    /* MPR limit check - rfwcdma_core_txplim_backoff_limit contains (NV limit - Actual Tx Limit)
       If MPR is less than rfwcdma_core_txplim_backoff_limit we don't have to backoff any further and if MPR is 
       greater than rfwcdma_core_txplim_backoff_limit the new MPR will be MPR-rfwcdma_core_txplim_backoff_limit */
    mpr_data.data.set_mpr.mpr[i] = (uint8)(rfwcdma_core_txplim_mprVals[i] > rfwcdma_core_txplim_backoff_limit) ? 
                            (rfwcdma_core_txplim_mprVals[i] - rfwcdma_core_txplim_backoff_limit):0; /*lint !e734 */
    rfwcdma_mdsp_set_mpr_beta_table(i, mpr_data.data.set_mpr.mpr[i], rfwcdma_core_txplim_betaBackoff[i], rfwcdma_core_txplim_txAgcOffset[i]);
    
    if(i>1)
    {
      rfwcdma_mdsp_set_pa_thresh_offset(i-1, rfwcdma_core_txplim_paThresholdOffset[i-1]);
    }
  }

  /* Update FED DM with updated MPR bin values */
  mpr_data.set_data_cmd = RFWCDMA_MDSP_SET_MPR;
  mpr_data.rflm_handle = (rflm_dm_handle_id_t)rfwcdma_mc_state.txlm_buf_idx;
  rfwcdma_mdsp_async_set_data(&mpr_data);
  
  if(rfwcdma_core_txplim_dbgmsg)
  { 
    MSG_8(MSG_SSID_RF, MSG_LEGACY_HIGH,"RFTXPLIM-UTRAN:%05d,DCH:%d,UTRAN:%03d,NV:%03d,LIN:%03d,Desired:%03d,MTPL:%03d.%02d",
      rfwcdma_core_txplim_counter,
      chan_type,
      (rfwcdma_core_txplim_utpl + rfwcdma_core_txlim_utran_vs_temp_offset_val) + RFAGC_MIN_AGC_VAL,
      rfwcdma_core_txplim_ntpl + RFAGC_MIN_AGC_VAL,
      rfwcdma_core_txplim_desired + RFAGC_MIN_AGC_VAL,
      rfwcdma_core_txplim_lin + RFAGC_MIN_AGC_VAL,
      /* MTPL Integer part */
      RFTXPLIM_Q5_TO_INT(rfwcdma_core_txplim_mtpl_q5)+RFAGC_MIN_AGC_VAL,
      /* MTPL Fraction part */
      RFTXPLIM_Q5_FRACTION(rfwcdma_core_txplim_mtpl_q5)); /*lint !e502 */
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function update the desired limit based on UTRAN TX power limit value
  and NV TX power limit along with lim_vs_freq.

  @details

*/
LOCAL void rfwcdma_core_txplim_set_desired_limit(void)
{
  const rfcommon_core_txlin_log_data_type* log_data;
  rfcom_wcdma_band_type band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx;
  
  /* NV Tx Power Limit (NTPL) */
  rfwcdma_core_txplim_ntpl = rfwcdma_core_txplim_get_mtpl_nv_item(rfwcdma_core_txplim_chan_type, band);

  if ((!IS_FTM_IN_TEST_MODE()) || (rfm_get_calibration_state() == FALSE))
  {
    /* Get current log data */
    log_data = rfcommon_core_txlin_get_log_data(RFCOM_WCDMA_MODE,rfwcdma_mc_state.primary_rx_device);

    if (NULL == log_data)
    {
      MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "rfwcdma_core_txplim_set_desired_limit: "
                                          "Null pointer access to Log data" );
      return; 
    }
    
    rfwcdma_core_txplim_lin = (log_data->max_calibrated_power) +700;
  }

  /* SAR Limit */
  rfwcdma_core_txplim_ntpl = MIN(rfwcdma_core_txplim_ntpl, 
        (rfwcdma_core_txplim_sar_limit_db10 + rfwcdma_core_txlim_utran_vs_temp_offset_val));

  /* Multi-TX SAR Limit */
  rfwcdma_core_txplim_ntpl = MIN(rfwcdma_core_txplim_ntpl,
         (rfwcdma_core_txplim_multi_tx_sar_limit_db10+rfwcdma_core_txlim_utran_vs_temp_offset_val));

  /* Backoff Limit */
  rfwcdma_core_txplim_backoff_limit = (int16) MAX((rfwcdma_core_txplim_ntpl - 
                (rfwcdma_core_txplim_utpl + rfwcdma_core_txlim_utran_vs_temp_offset_val)),0);

  /* Desired Limit */
  rfwcdma_core_txplim_desired = MIN(rfwcdma_core_txplim_ntpl, 
                    (rfwcdma_core_txplim_utpl + rfwcdma_core_txlim_utran_vs_temp_offset_val));

  rfwcdma_core_txplim_desired = MIN(rfwcdma_core_txplim_desired, rfwcdma_core_txplim_lin);

  /* Add Freq Comp factor */
  rfwcdma_core_txplim_desired += rfwcdma_core_hdet_vs_freq_val;
  
  rfwcdma_core_txplim_desired += rfwcdma_core_desired_lim_vs_freq_val;

  /* Add thermal mitigation backoff */
  rfwcdma_core_txplim_desired += rfwcdma_core_txplim_thermal_backoff_db10;

  MSG_3( MSG_SSID_RF, MSG_LEGACY_LOW,"rfwcdma_core_txplim_set_desired_limit( ): desired:%03d,UTPL:%03d,SAR:%03d",
  rfwcdma_core_txplim_desired + RFAGC_MIN_AGC_VAL, rfwcdma_core_txplim_utpl + RFAGC_MIN_AGC_VAL,rfwcdma_core_txplim_sar_limit_db10); 

}

/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function enables the TX power limit adjustment algorithm.
  This function should be called when TX is enabled.

  @param enalbe Enable or disable Tx power limit

*/
void rfwcdma_core_txplim_enable(boolean enable)
{
  rfwcdma_core_txplim_enabled = enable;

  rfwcdma_core_txplim_first_time_flag = TRUE;
  
  if(enable)
  {
    rfwcdma_core_txplim_mode = RFTXPLIM_MODE_INIT;
    if(rfwcdma_core_txplim_dbgmsg)
    { 
       MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"RFTXPLIM-ENABLE:%05d",rfwcdma_core_txplim_counter);
    }
  }
  else
  {
    rfwcdma_core_txplim_mode = RFTXPLIM_MODE_DISABLED;
    rfwcdma_core_txplim_thermal_backoff_db10 = 0;
    if(rfwcdma_core_txplim_dbgmsg)
    { 
       MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"RFTXPLIM-DISABLE:%05d",rfwcdma_core_txplim_counter);
    }
    
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Returns Max TX power limit(MTPL) obtained by the limiting loop

  @details

*/
int16 rfwcdma_core_txplim_get_mtpl(void)
{
  return(RFTXPLIM_Q5_TO_INT(rfwcdma_core_txplim_mtpl_q5) + RFAGC_MIN_AGC_VAL);
}


/*--------------------------------------------------------------------------*/
/*!
   @brief
   This function return the NV TX power limit in dBm unit. 
   
   @details
*/
int16 rfwcdma_core_txplim_get_mtpl_nv_item(rfcom_txplim_channel_type chan_type, rfcom_wcdma_band_type band)
{
  int16 max_tx_power = RF_UE_MAX_TX_POWER;

  if ( chan_type == RFCOM_TXPLIM_RACH )
  {
    /* Null pointer check */
    if (band < RFCOM_NUM_WCDMA_BANDS)
    {
      if (rfnv_wcdma_tbl_ptr[band] == NULL)
      {
        RF_MSG_1(RF_ERROR,
                 "rfwcdma_core_txplim_get_mtpl_nv_item: NULL NV tbl ptr for band %d",
                 band);
      }

      else
      {
        max_tx_power = (rfnv_wcdma_tbl_ptr[band]->max_tx_power - RF_MIN_TX_POWER_DBM) * RF_TXAGC_RESOLUTION;
      }
    }

    else
    {
       RF_MSG_1(RF_ERROR,
                "rfwcdma_core_txplim_get_mtpl_nv_item: Not a valid band %d",
                band);
    }
  }
  else if ( chan_type == RFCOM_TXPLIM_DCH )
  {
    max_tx_power = rfwcdma_core_txlim_vs_temp_val;
  }
  else
  {
    RF_MSG_1(RF_ERROR,"rfwcdma_core_txplim_get_mtpl_nv_item: Invalid control channel specified for max Tx power %d", chan_type);
  }

  return (max_tx_power);
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  Returns UE max tx limit/Desired Limit

  @details

*/

int16 rfwcdma_core_txplim_get_desired_limit(void)
{
  return(((rfwcdma_core_txplim_desired)/RF_TXAGC_RESOLUTION) + RF_MIN_TX_POWER_DBM);
}


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function reads the latest hdet value being used by the RF driver.

*/

int16 rfwcdma_core_txplim_get_hdet(void)
{
  return ((int16)(rfwcdma_core_txplim_hdet + RFAGC_MIN_AGC_VAL));
}


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function retururns the last raw hdet adc value.

*/

uint16 rfwcdma_core_txplim_get_hdet_adc(void)
{
  return ((uint16)rfwcdma_core_txplim_hdet_adc);
}

/*--------------------------------------------------------------------------*/
/*!
   @brief

  @details 
  Return HDET threshold value (18 dB) in dB10 AGC units.
  
  @param none

*/

int16 rfwcdma_core_txplim_get_hdet_threshold(void)
{
  int16 hdet_thres = RFTXPLIM_TX_PWR_LIM_MIN + RFAGC_MIN_AGC_VAL;
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
  WCDMA mdsp fw image must be loaded.
*/

void rfwcdma_core_txplim_get_mtpl_backoff(rf_max_tx_pwr_limit_type *rf_mtpl)
{
  int16 mtpl;

  mtpl = rfwcdma_core_txplim_get_mtpl();

  rf_mtpl->caseA = mtpl - MAX(0,(rfwcdma_core_txplim_rel5_backoff.caseA - rfwcdma_core_txplim_backoff_limit));
  rf_mtpl->caseB = mtpl - MAX(0,(rfwcdma_core_txplim_rel5_backoff.caseB - rfwcdma_core_txplim_backoff_limit));
  rf_mtpl->caseC = mtpl - MAX(0,(rfwcdma_core_txplim_rel5_backoff.caseC - rfwcdma_core_txplim_backoff_limit));
  rf_mtpl->caseD = mtpl - MAX(0,(rfwcdma_core_txplim_rel5_backoff.caseD - rfwcdma_core_txplim_backoff_limit));
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  
  @details
  This function reads the VBatt voltage and compares it with the three 
  'nv_power_backoff_voltages' values.  Depending on the current VBatt value,  
  the rf_backoff_for_bcs variable is assigned to the nv_power_backoff_volt#.

*/
void rfwcdma_core_txplim_do_vbatt_based_backoff(void)
{

 rfcom_wcdma_band_type band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx;// Use Rx band as Tx band has not been initialised

  /* Null pointer check */
  if (band < RFCOM_NUM_WCDMA_BANDS)
  {
    if( rfnv_wcdma_tbl_ptr[band] != NULL)
    { 
    /*  Assign the max_pwr_backoff_volt1 to rf_backoff_for_bcs by default */
    rfwcdma_core_txplim_rel5_backoff.caseA = rfnv_wcdma_tbl_ptr[band]->max_pwr_backoff_volt1[0];
    rfwcdma_core_txplim_rel5_backoff.caseB = rfnv_wcdma_tbl_ptr[band]->max_pwr_backoff_volt1[1];
    rfwcdma_core_txplim_rel5_backoff.caseC = rfnv_wcdma_tbl_ptr[band]->max_pwr_backoff_volt1[2];
    rfwcdma_core_txplim_rel5_backoff.caseD = rfnv_wcdma_tbl_ptr[band]->max_pwr_backoff_volt1[3];
    }
  }
  /*! @todo bhaskarn enable vbatt based backoff */
}

/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Recalculates the TX power limit based on the frequency, temperature, current
  HDET value, and TX AGC value.

  @dep
  rftxplim must be called first.
  temperature must be read and temperature data must be ready.
  frequency index must be calculated.

*/

/*--------------------------------------------------------------------------*/
/*!
  @brief
  
  @details
*/
LOCAL void rfwcdma_core_txplim_adjust_mtpl(void)
{
  int16 target_limit = 0;
  int16 limit_error_q5 = 0;
  int16 filtered_limit_error_q5 = 0;
  int16 mtpl_adj_db10;

  mtpl_adj_db10 = -rfwcdma_mdsp_async_get_mtpl_adj_db10(rfwcdma_mc_state.txlm_buf_idx);
 
  /* HDET Cal Error */
  rfwcdma_core_txplim_cal_error = rfwcdma_core_txplim_hdet - (rfwcdma_core_txplim_txagc + mtpl_adj_db10);
  
  target_limit = rfwcdma_core_txplim_desired - rfwcdma_core_txplim_cal_error;

  target_limit = MIN(target_limit, rfwcdma_core_txplim_lin);
  
  limit_error_q5 = RFTXPLIM_INT_TO_Q5(target_limit) - rfwcdma_core_txplim_mtpl_q5; /*lint !e734 */

  rfwcdma_core_txplim_update_mode((uint16)abs(limit_error_q5));

  /* Invalid Mode based on abs limit error.  If one erronous HDET trigger detected showing
    more than 5dB of limit error it is ignored */  
  if(rfwcdma_core_txplim_mode == RFTXPLIM_MODE_INVALID)
  {
    MSG_8(MSG_SSID_RF, MSG_LVL_HIGH,
      "RFTXPLIM-INVALID:%05d,TxAGC:%03d,HDET:%03d,abs_limit_err:%02d.%02d,Desired:%03d,MTPL:%03d.%02d",
      rfwcdma_core_txplim_counter,
      rfwcdma_core_txplim_txagc + RFAGC_MIN_AGC_VAL,
      rfwcdma_core_txplim_hdet + RFAGC_MIN_AGC_VAL,
      RFTXPLIM_Q5_TO_INT(abs(limit_error_q5)),
      RFTXPLIM_Q5_FRACTION(abs(limit_error_q5)),
      rfwcdma_core_txplim_desired + RFAGC_MIN_AGC_VAL,
      /* MTPL Integer part */
      RFTXPLIM_Q5_TO_INT(rfwcdma_core_txplim_mtpl_q5)+RFAGC_MIN_AGC_VAL,
      /* MTPL Fraction part */
      RFTXPLIM_Q5_FRACTION(rfwcdma_core_txplim_mtpl_q5)); /*lint !e502 */
      return; /* Skip the Invalid trigger */
  }

  filtered_limit_error_q5 = RF_SATURATE((limit_error_q5/rfwcdma_core_txplim_gain),
                                            RFTXPLIM_INT_TO_Q5(-RFTXPLIM_FILT_LIMIT_ERR_MAX),
                                            RFTXPLIM_INT_TO_Q5(RFTXPLIM_FILT_LIMIT_ERR_MAX));
  
  rfwcdma_core_txplim_mtpl_q5 = RF_SATURATE( (rfwcdma_core_txplim_mtpl_q5 + filtered_limit_error_q5),                                            
                                            RFTXPLIM_INT_TO_Q5(RFTXPLIM_TX_AGC_MIN),
                                            RFTXPLIM_INT_TO_Q5(RFTXPLIM_TX_PWR_LIM_MAX));

  if(rfwcdma_core_txplim_dbgmsg)
  {
    if(rfwcdma_core_txplim_mode == RFTXPLIM_MODE_ACQ)
    {
      MSG_9(MSG_SSID_RF, MSG_LEGACY_HIGH,
        "RFTXPLIM-ACQ:%05d,TxAGC:%03d,HDET:%03d,FreqComp:%02d,UTRAN:%03d,NV:%03d,Desired:%03d,Target:%03d,DEM:%03d",
        rfwcdma_core_txplim_counter,
        rfwcdma_core_txplim_txagc + RFAGC_MIN_AGC_VAL,
        rfwcdma_core_txplim_hdet + RFAGC_MIN_AGC_VAL,
        rfwcdma_core_hdet_vs_freq_val,
        (rfwcdma_core_txplim_utpl + rfwcdma_core_txlim_utran_vs_temp_offset_val) + RFAGC_MIN_AGC_VAL,
        rfwcdma_core_txplim_ntpl + RFAGC_MIN_AGC_VAL,
        rfwcdma_core_txplim_desired + RFAGC_MIN_AGC_VAL,
        target_limit + RFAGC_MIN_AGC_VAL,
        rfwcdma_core_txplim_thermal_backoff_db10);
      MSG_8(MSG_SSID_RF, MSG_LEGACY_HIGH,
        "RFTXPLIM-ACQ:%05d,Gain:%02d,LimitErr:%02d.%02d,Filtered:%02d.%02d,MTPL:%03d.%02d",
        rfwcdma_core_txplim_counter,
        rfwcdma_core_txplim_gain,
        RFTXPLIM_Q5_INTEGER(limit_error_q5),RFTXPLIM_Q5_FRACTION(limit_error_q5),
        RFTXPLIM_Q5_INTEGER(filtered_limit_error_q5),RFTXPLIM_Q5_FRACTION(filtered_limit_error_q5),
        RFTXPLIM_Q5_TO_INT(rfwcdma_core_txplim_mtpl_q5)+RFAGC_MIN_AGC_VAL,
        RFTXPLIM_Q5_FRACTION(rfwcdma_core_txplim_mtpl_q5)); /*lint !e502 */
    }
    else if(rfwcdma_core_txplim_mode == RFTXPLIM_MODE_TRK)
    {
      MSG_9(MSG_SSID_RF, MSG_LEGACY_HIGH,
        "RFTXPLIM-TRK:%05d,TxAGC:%03d,HDET:%03d,FreqComp:%02d,UTRAN:%03d,NV:%03d,Desired:%03d,Target:%03d,DEM:%03d",
        rfwcdma_core_txplim_counter,
        rfwcdma_core_txplim_txagc + RFAGC_MIN_AGC_VAL,
        rfwcdma_core_txplim_hdet + RFAGC_MIN_AGC_VAL,
        rfwcdma_core_hdet_vs_freq_val,
        (rfwcdma_core_txplim_utpl + rfwcdma_core_txlim_utran_vs_temp_offset_val) + RFAGC_MIN_AGC_VAL,
        rfwcdma_core_txplim_ntpl + RFAGC_MIN_AGC_VAL,
        rfwcdma_core_txplim_desired + RFAGC_MIN_AGC_VAL,
        target_limit + RFAGC_MIN_AGC_VAL,
        rfwcdma_core_txplim_thermal_backoff_db10);
      MSG_8(MSG_SSID_RF, MSG_LEGACY_HIGH,
        "RFTXPLIM-TRK:%05d,Gain:%02d,LimitErr:%02d.%02d,Filtered:%02d.%02d,MTPL:%03d.%02d",
        rfwcdma_core_txplim_counter,
        rfwcdma_core_txplim_gain,
        RFTXPLIM_Q5_INTEGER(limit_error_q5),RFTXPLIM_Q5_FRACTION(limit_error_q5),
        RFTXPLIM_Q5_INTEGER(filtered_limit_error_q5),RFTXPLIM_Q5_FRACTION(filtered_limit_error_q5),
        RFTXPLIM_Q5_TO_INT(rfwcdma_core_txplim_mtpl_q5)+RFAGC_MIN_AGC_VAL,
        RFTXPLIM_Q5_FRACTION(rfwcdma_core_txplim_mtpl_q5)); /*lint !e502 */
    }
    
  }
}

/*--------------------------------------------------------------------------*/
/*!
   @brief                             
     RFTXPLIM 2.5 - Faster error tracking
   
   @detail
     Max power limiting mode with finer resolution of filter gain is used 
     for faster error tracking.
   
* ACQ - Acquistion   (10 ms HDET trigger)
* TRK - Tracking     (100ms HDET trigger)
* INV - Invalid Once (Absolute error >= 5dB)

  ------------------------------------
   | Limit  | MODE | HDET  | Filter |
   | Error  |      | Timer |  Gain  |
  ------------------------------------
   |>=5.0dB | INV  | ----    -----
  ------------------------------------
   | >3.0dB | ACQ  | 10ms  |  1/4   |
  ------------------------------------
   | >2.5dB | ACQ  | 10ms  |  1/6   |
  ------------------------------------
   | >2.0dB | ACQ  | 10ms  |  1/7   |
  ------------------------------------
   | >1.5dB | ACQ  | 10ms  |  1/8   |
  ------------------------------------
   | >1.0dB | ACQ  | 10ms  |  1/9   |
  ------------------------------------
   | >0.5dB | ACQ  | 10ms  |  1/10  |
  ------------------------------------
   | >0.3dB | ACQ  | 10ms  |  1/16  |
  ------------------------------------
   | >0.2dB | ACQ  | 10ms  |  1/24  |
  ------------------------------------
   | >0.1dB | ACQ  | 10ms  |  1/32  |
  ====================================
   |  else  | TRK  | 100ms |  1/32  |
  ====================================
*/

LOCAL void rfwcdma_core_txplim_update_mode(uint16 abs_limit_error)
{

  rfwcdma_mdsp_set_data_param_type set_periodicity_param = { 0 };

  /* Populate MDSP paramters */
  set_periodicity_param.set_data_cmd = RFWCDMA_MDSP_SET_POWER_METER_PERIODICITY;
  set_periodicity_param.rflm_handle = (rflm_dm_handle_id_t)rfwcdma_mc_state.txlm_buf_idx;

  /* Spec Voilation: Move to ACQ immediately */
  if(rfwcdma_core_txplim_hdet >= RFTXPLIM_TX_SPEC_LIMIT)
  {
    rfwcdma_core_txplim_mode = RFTXPLIM_MODE_ACQ;
    rfwcdma_core_txplim_gain =  4;                 /* Filter Gain = 1/4 */
    set_periodicity_param.data.gen_dw = 10000; /* HDET Timer = 10ms */  
    rfwcdma_mdsp_async_set_data(&set_periodicity_param);

    return;
  }
  /* >= 5dB limit error */
  else if(abs_limit_error >= RFTXPLIM_INT_TO_Q5(50)) /* 5.0 dB */
  {
    if(rfwcdma_core_txplim_mode != RFTXPLIM_MODE_INVALID)
    {
      /* First HDET trigger with more than 5dB of absoulte error is ignored */
      rfwcdma_core_txplim_mode = RFTXPLIM_MODE_INVALID;
    }
  }
  /* >3dB limit_error */
  else if(abs_limit_error > RFTXPLIM_INT_TO_Q5(30)) /* 3.0 dB */
  {
    rfwcdma_core_txplim_mode = RFTXPLIM_MODE_ACQ;
    rfwcdma_core_txplim_gain = 4;                  /* Filter Gain = 1/4 */
    set_periodicity_param.data.gen_dw = 10000; /* HDET Timer = 10ms */  
    rfwcdma_mdsp_async_set_data(&set_periodicity_param);
    return;
  }
  /* >2.5dB limit_error */
  else if(abs_limit_error > RFTXPLIM_INT_TO_Q5(25)) /* 2.5 dB */
  {
    rfwcdma_core_txplim_mode = RFTXPLIM_MODE_ACQ;
    rfwcdma_core_txplim_gain = 6;                  /* Filter Gain = 1/6 */
    set_periodicity_param.data.gen_dw = 10000; /* HDET Timer = 10ms */  
    rfwcdma_mdsp_async_set_data(&set_periodicity_param);
    return;    
  }
  /* >2dB limit_error */
  else if(abs_limit_error > RFTXPLIM_INT_TO_Q5(20)) /* 2.0 dB */
  {
    rfwcdma_core_txplim_mode = RFTXPLIM_MODE_ACQ;
    rfwcdma_core_txplim_gain = 7;                  /* Filter Gain = 1/7 */
    set_periodicity_param.data.gen_dw = 10000; /* HDET Timer = 10ms */  
    rfwcdma_mdsp_async_set_data(&set_periodicity_param);
    return;    
  }
  /* >1.5dB limit_error */
  else if(abs_limit_error > RFTXPLIM_INT_TO_Q5(15)) /* 1.5 dB */
  {
    rfwcdma_core_txplim_mode = RFTXPLIM_MODE_ACQ;
    rfwcdma_core_txplim_gain = 8;                  /* Filter Gain = 1/8 */
    set_periodicity_param.data.gen_dw = 10000; /* HDET Timer = 10ms */  
    rfwcdma_mdsp_async_set_data(&set_periodicity_param);
    return;    
  }
  /* >1dB limit_error */
  else if(abs_limit_error > RFTXPLIM_INT_TO_Q5(10)) /* 1.0 dB */
  {
    rfwcdma_core_txplim_mode = RFTXPLIM_MODE_ACQ;
    rfwcdma_core_txplim_gain = 9;                  /* Filter Gain = 1/9 */
    set_periodicity_param.data.gen_dw = 10000; /* HDET Timer = 10ms */  
    rfwcdma_mdsp_async_set_data(&set_periodicity_param);
    return;    
  }  
  /* >0.5dB limit_error */
  else if(abs_limit_error > RFTXPLIM_INT_TO_Q5(5)) /* 0.5 dB */
  {
    rfwcdma_core_txplim_mode = RFTXPLIM_MODE_ACQ;
    rfwcdma_core_txplim_gain = 10;                  /* Filter Gain = 1/10 */
    set_periodicity_param.data.gen_dw = 10000; /* HDET Timer = 10ms */  
    rfwcdma_mdsp_async_set_data(&set_periodicity_param);
    return;    
  } 
  /* >0.3dB limit_error */
  else if(abs_limit_error > RFTXPLIM_INT_TO_Q5(3)) /* 0.3 dB */
  {
    rfwcdma_core_txplim_mode = RFTXPLIM_MODE_ACQ;
    rfwcdma_core_txplim_gain = 16;                  /* Filter Gain = 1/16 */
    set_periodicity_param.data.gen_dw = 10000; /* HDET Timer = 10ms */  
    rfwcdma_mdsp_async_set_data(&set_periodicity_param);
    return;    
  } 
  /* >0.2dB limit_error */
  else if(abs_limit_error > RFTXPLIM_INT_TO_Q5(2)) /* 0.2 dB */
  {
    rfwcdma_core_txplim_mode = RFTXPLIM_MODE_ACQ;
    rfwcdma_core_txplim_gain = 24;                  /* Filter Gain = 1/24 */
    set_periodicity_param.data.gen_dw = 10000; /* HDET Timer = 10ms */  
    rfwcdma_mdsp_async_set_data(&set_periodicity_param);
    return;    
  }
  /* >0.1dB limit_error */
  else if(abs_limit_error > RFTXPLIM_INT_TO_Q5(1)) /* 0.1 dB */
  {
    rfwcdma_core_txplim_mode = RFTXPLIM_MODE_ACQ;
    rfwcdma_core_txplim_gain = 32;                  /* Filter Gain = 1/32 */
    set_periodicity_param.data.gen_dw = 10000; /* HDET Timer = 10ms */  
    rfwcdma_mdsp_async_set_data(&set_periodicity_param);
    return;    
  }  
  /* TRK mode */
  else
  {
    rfwcdma_core_txplim_mode = RFTXPLIM_MODE_TRK;
    rfwcdma_core_txplim_gain = 32;                  /* Filter Gain = 1/32 */
    set_periodicity_param.data.gen_dw = 100000; /* HDET Timer = 100ms */  
    rfwcdma_mdsp_async_set_data(&set_periodicity_param);
    return;    
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
  @param band Operational WCDMA band
  @param channel Operational channel
*/
void rfwcdma_core_txplim_update_lim_vs_freq
(
  rf_path_enum_type path,
  rfcom_wcdma_band_type band,
  uint16 channel
)
{
  int16 tx_start_bin = 0;     /* starting TX cal channel for bin containing */
  int32 tx_bin_size;          /* number of channels in the bin (not constant) */
  int32 tx_delta_x;           /* scale of channel number in bin */
  int16 delta_y_1, delta_y_2; /* used to store delta_y between adjacent nv items */
  boolean tx_channel_under = 0; /* flag if lowest TX cal channel > tune value */
  uint32 tx_freq;
  boolean out_of_range= FALSE;
  tx_band_cal_type* tx_cal_data = rfwcdma_core_txlin_get_band_data(band);

  if (tx_cal_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFTXPLIM-FreqComp:Null pointer for Tx Cal data!",0);
    return;
  }

  /* NV table pointer needs to be intialized */
  if (band < RFCOM_NUM_WCDMA_BANDS)
  {
    if (rfnv_wcdma_tbl_ptr[band] == NULL)
    {
      RF_MSG_1(RF_ERROR,
               "rfwcdma_core_txplim_update_lim_vs_freq: NULL NV tbl ptr for band %d",
               band);
      return;
    }
  }

  else
  {
    RF_MSG_1(RF_ERROR,
               "rfwcdma_core_txplim_update_lim_vs_freq: Not a valid band %d",
               band);
    return;
  }


  /*!@ anupamav Removing the channel check if it is the same band. this shouldn't
  happen as we are checking in main ctrl and calling this function only when band
  changes & no need to update the channel/band information*/

  tx_freq = rfwcdma_core_get_freq_from_uarfcn( band, channel, RFWCDMA_CORE_UARFCN_UL );

  /* only handle TX items for path 0 */
  if ( path == RF_PATH_0 )
  {
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
      while ( tx_start_bin < (NV_FREQ_TABLE_SIZ-1) )
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

    if ( tx_start_bin == (NV_FREQ_TABLE_SIZ -1) || out_of_range )
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
      rfwcdma_core_hdet_vs_freq_val =
        (int8)rfnv_wcdma_tbl_ptr[band]->tx_lim_vs_freq[tx_start_bin];
      rfwcdma_core_desired_lim_vs_freq_val = 
        (int8)rfnv_wcdma_tbl_ptr[band]->mtpl_vs_freq[tx_start_bin];
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

      /*lint -e{661} bound check is done thru tx_bin_size which would be 0 if tx_start_bin were >= NV_FREQ_TABLE_SIZ -1*/
      delta_y_1 = (rfnv_wcdma_tbl_ptr[band]->
                 tx_lim_vs_freq[tx_start_bin+1]-
                 rfnv_wcdma_tbl_ptr[band]->tx_lim_vs_freq[tx_start_bin]);

      delta_y_2 = (rfnv_wcdma_tbl_ptr[band]->
                 mtpl_vs_freq[tx_start_bin+1]-
                 rfnv_wcdma_tbl_ptr[band]->mtpl_vs_freq[tx_start_bin]);

      (delta_y_1<0)?delta_y_1--:delta_y_1++;

      (delta_y_2<0)?delta_y_2--:delta_y_2++;

      rfwcdma_core_hdet_vs_freq_val = (int8)((delta_y_1*tx_delta_x/tx_bin_size) +
                                      rfnv_wcdma_tbl_ptr[band]->
                                      tx_lim_vs_freq[tx_start_bin]);
      rfwcdma_core_desired_lim_vs_freq_val = (int8)((delta_y_2*tx_delta_x/tx_bin_size) +
                                      rfnv_wcdma_tbl_ptr[band]->
                                      mtpl_vs_freq[tx_start_bin]);
    } /* interpolate case */
  } /* end PATH 0 - tx items */

  /* update limits */
  rfwcdma_core_txplim_set_desired_limit();

  if(rfwcdma_core_txplim_dbgmsg)
  { 
    MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH,"RFTXPLIM-FreqComp:%05d,Channel:%d,HDET Freq comp:%d, Desired Limit Freq comp:%d",
      rfwcdma_core_txplim_counter,
      channel,
      rfwcdma_core_hdet_vs_freq_val,
      rfwcdma_core_desired_lim_vs_freq_val);
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details

*/
void rfwcdma_core_txplim_temp_val_update
(
  int16 tx_lim_vs_temp_val, 
  int16 utran_tx_lim_vs_temp_offset_val,
  int16 hdet_vs_temp_val,
  int16 hdet_mod_base,
  int16 hdet_mod_range[2]
)
{
  rfwcdma_core_txlim_vs_temp_val = tx_lim_vs_temp_val; 
  rfwcdma_core_txlim_utran_vs_temp_offset_val = utran_tx_lim_vs_temp_offset_val;
  rfwcdma_core_txlim_hdet_vs_temp = hdet_vs_temp_val;
  rfwcdma_core_txlim_txlin_adjust_limit[0] = RF_SATURATE((hdet_mod_base+hdet_mod_range[0]),
                                                       -RFTXPLIM_TXAGC_ERROR_ACCUM_MAX,
                                                       RFTXPLIM_TXAGC_ERROR_ACCUM_MAX);
  rfwcdma_core_txlim_txlin_adjust_limit[1] = RF_SATURATE((hdet_mod_base+hdet_mod_range[1]),
                                                       -RFTXPLIM_TXAGC_ERROR_ACCUM_MAX,
                                                       RFTXPLIM_TXAGC_ERROR_ACCUM_MAX);
  /* update limits */
  rfwcdma_core_txplim_set_desired_limit();

  if(rfwcdma_core_txplim_dbgmsg)
  { 
    MSG_7(MSG_SSID_RF, MSG_LEGACY_HIGH,
          "RFTXPLIM-TempComp:%05d,UtranTempOffset:%d,LimVsTemp:%d, HDETVsTemp:%d, HDETModBase:%d,HDETModRange:%d -- %d",
          rfwcdma_core_txplim_counter,
          rfwcdma_core_txlim_utran_vs_temp_offset_val,
          rfwcdma_core_txlim_vs_temp_val,
          rfwcdma_core_txlim_hdet_vs_temp,
          hdet_mod_base,
          hdet_mod_range[0],hdet_mod_range[1] );
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
void rfwcdma_core_txplim_reset_to_acq_mode(void)
{
  rfwcdma_core_txplim_counter = 0;
  rfwcdma_core_txplim_tx_err_accum_q5 = 0;
  rfwcdma_core_txplim_mode = RFTXPLIM_MODE_ACQ;
  rfwcdma_core_txplim_gain =  4;                 /* Filter Gain */
  rfwcdma_mdsp_set_hdet_interval(150);           /* HDET Timer */  
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
void rfwcdma_core_txplim_reduce_mptl_db_for_therm(uint8 backoff)
{

  /* Range checking - do not allow backoff to take the limit below RFTXPLIM_THERMAL_PMIN_DB */
  backoff = MIN(backoff, MAX((RFTXPLIM_Q5_TO_INT(rfwcdma_core_txplim_mtpl_q5)/10 - 70)
                              - RFTXPLIM_THERMAL_PMIN_DB,0));


  /* keep local variable in standard units, i.e. negative value indicates backoff */
  rfwcdma_core_txplim_thermal_backoff_db10 = -1*10*backoff;

  /* Set ACQ mode to converge quickly to new backoff level */
  rfwcdma_core_txplim_reset_to_acq_mode();

  /* update limits */
  rfwcdma_core_txplim_set_desired_limit();

 /* Adjust the current MTPL read by L1 to account for the case where the hdet
  * loop is not running.
  */
  rfwcdma_core_txplim_mtpl_q5  += RFTXPLIM_INT_TO_Q5(rfwcdma_core_txplim_thermal_backoff_db10 - 
                                                     rfwcdma_core_txplim_thermal_backoff_prev_db10);

  rfwcdma_core_txplim_thermal_backoff_prev_db10 = rfwcdma_core_txplim_thermal_backoff_db10;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function will set the SAR MTPL.

  @details


*/
void rfwcdma_core_txplim_set_sar_limit(rfm_common_sar_state_type sar_index)
{
  rfcom_wcdma_band_type band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx;// Use Rx band as Tx band has not been initialised

  rfwcdma_core_txplim_desired_prev = rfwcdma_core_txplim_desired;

  if(sar_index > RF_SAR_STATE_DEFAULT && sar_index < RF_SAR_STATE_MAX)
  {
    /* retreive desired MTPL in db10 software units*/
    /* NV contains 8 elements as it does not include the default state */
      /* Null pointer check */
      if (band < RFCOM_NUM_WCDMA_BANDS)
      {
        if (rfnv_wcdma_tbl_ptr[band] == NULL)
        {
          RF_MSG_1(RF_ERROR,
                   "rfwcdma_core_txplim_set_sar_limit: NULL NV tbl ptr for band %d",
                   band);
        }
        else
        {
          rfwcdma_core_txplim_sar_limit_db10 = rfnv_wcdma_tbl_ptr[band]->wcdma_db10_sar_back_off_limit[sar_index-1];
        }
      }

      else
      {
         RF_MSG_1(RF_ERROR,
                  "rfwcdma_core_txplim_set_sar_limit: Not a valid band %d",
                  band);
      }
  }
  else
  {
    rfwcdma_core_txplim_sar_limit_db10 = RFTXPLIM_TX_PWR_LIM_MAX - RFTXPLIM_SAR_NV_SCALE_FACTOR;
  }
  /* Convert absoulte dB10 to AGC units */
  rfwcdma_core_txplim_sar_limit_db10 += RFTXPLIM_SAR_NV_SCALE_FACTOR;

  /* Set ACQ mode to converge quickly to new backoff level */
  rfwcdma_core_txplim_reset_to_acq_mode();

  /* update limits */
  rfwcdma_core_txplim_set_desired_limit();

 /* Adjust the current MTPL read by L1 to account for the case where the hdet
  * loop is not running. This is required because the rfwcdma_core_txplim_set_desired_limit() 
  * function will have no effect if the HDET loop is not running. 
  */
  rfwcdma_core_txplim_mtpl_q5  += RFTXPLIM_INT_TO_Q5(rfwcdma_core_txplim_desired - rfwcdma_core_txplim_desired_prev);

  MSG_5( MSG_SSID_RF, MSG_LEGACY_LOW,"UTPL:%03d,Desired:%03d,Previous:%03d,MTPL:%03d.%02d",
    rfwcdma_core_txplim_utpl + RFAGC_MIN_AGC_VAL,
    rfwcdma_core_txplim_desired + RFAGC_MIN_AGC_VAL,
    rfwcdma_core_txplim_desired_prev + RFAGC_MIN_AGC_VAL,
    /* MTPL Integer part */
    RFTXPLIM_Q5_TO_INT(rfwcdma_core_txplim_mtpl_q5)+RFAGC_MIN_AGC_VAL,
    /* MTPL Fraction part */
    RFTXPLIM_Q5_FRACTION(rfwcdma_core_txplim_mtpl_q5)); /*lint !e502 *///auto-gen, to change remove 'QSR_' and first param
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function will the multi-TX SAR limit. 

  @details
  Update the static value for multi-tx sar and then force an update of the
  overall power limit values.
 
  @param device enum - NOT CURRENTLY USED
  @param SAR limit in dBx10
*/
void rfwcdma_core_txplim_set_multi_tx_sar_limit
(
  rfcom_device_enum_type device,
  int16 sar_limit
)
{
  rfwcdma_core_txplim_desired_prev = rfwcdma_core_txplim_desired;

  /* Convert absolute dB10 to AGC units */
  rfwcdma_core_txplim_multi_tx_sar_limit_db10 =
    sar_limit + RFTXPLIM_SAR_NV_SCALE_FACTOR;

  /* Set ACQ mode to converge quickly to new backoff level */
  rfwcdma_core_txplim_reset_to_acq_mode();

  /* update limits */
  rfwcdma_core_txplim_set_desired_limit();

 /* Adjust the current MTPL read by L1 to account for the case where the hdet
  * loop is not running. This is required because the rfwcdma_core_txplim_set_desired_limit() 
  * function will have no effect if the HDET loop is not running. 
  */
  rfwcdma_core_txplim_mtpl_q5  += RFTXPLIM_INT_TO_Q5(rfwcdma_core_txplim_desired - rfwcdma_core_txplim_desired_prev);

  MSG_5( MSG_SSID_RF, MSG_LEGACY_LOW,"UTPL:%03d,Desired:%03d,Previous:%03d,MTPL:%03d.%02d",
    rfwcdma_core_txplim_utpl + RFAGC_MIN_AGC_VAL,
    rfwcdma_core_txplim_desired + RFAGC_MIN_AGC_VAL,
    rfwcdma_core_txplim_desired_prev + RFAGC_MIN_AGC_VAL,
    /* MTPL Integer part */
    RFTXPLIM_Q5_TO_INT(rfwcdma_core_txplim_mtpl_q5)+RFAGC_MIN_AGC_VAL,
    /* MTPL Fraction part */
    RFTXPLIM_Q5_FRACTION(rfwcdma_core_txplim_mtpl_q5)); /*lint !e502 *///auto-gen, to change remove 'QSR_' and first param
}

/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Calculates 4095 expected TX AGC value for each HDET and store them into
  the table pointed by rftxplim_exp_agc_vs_hdet_ptr (rfwcdma_core_txplim_exp_agc_vs_hdet[]).

  The 4095 expected TX AGC value is calculated by interpolating the 16+1 element
  rfnv_exp_hdet_vs_agc[] table.

*/
uint16 
rfwcdma_core_binary_search
(
  uint16 value,
  uint16 low,
  uint16 high
)
{
  uint16 mid;
  uint16 num_return;

  if ((high - low)==1)
  {
    num_return = low; // found
  } 
  else
  {
    mid = RF_FLOOR((low + high),2);
    if (value >= rfwcdma_core_txplim_exp_hdet_vs_agc_ptr[ mid ])
    {
      num_return = rfwcdma_core_binary_search(value, mid, high);
    }
    else
    { 
      num_return = rfwcdma_core_binary_search(value, low, mid);
    }
  }

  return num_return;
}


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Calculates 4095 expected TX AGC value for each HDET and store them into
  the table pointed by rftxplim_exp_agc_vs_hdet_ptr (rfwcdma_core_txplim_exp_agc_vs_hdet[]).

  The 4095 expected TX AGC value is calculated by interpolating the 16+1 element
  rfnv_exp_hdet_vs_agc[] table.

*/
int16 
rfwcdma_core_txplim_hdet_to_txagc
(
  uint16 hdet_idx
)
{
  uint16 zone;        /* agc zone for interpolation */
  uint16 low_zone;    /* agc low  zone for interpolation */
  uint16 high_zone;   /* agc high zone for interpolation */
  uint16 delta;       /* hdet delta or offset into agc zone */
  uint16 offset;      /* TX AGC offset */
  uint16 rise;        /* hdet rise or slope between two agc zone */
  int16 result;      /* temporary holder */
  uint16 i;
  int16 txagc;
  
  /* Find the bottom of the expected HDET curve.  The curve is flat
  ** at the top and bottom, and sloping in the middle.
  **
  ** note: RF Cal makes sure that the points below the curve all
  ** have the same value.
  */
  low_zone = 0;
  
  /* traverse agc zone index 1 to 15 */
  /* to find the end of agc low zone */
  for ( i = 1; i < RFTXPLIM_EXP_HDET_VS_AGC_SIZ; i++ ) 
  {
     /* if current HDET is equal to the first HDET then continue search for */
     /* the end of agc low zone */
     if ( rfwcdma_core_txplim_exp_hdet_vs_agc_ptr[ i ] == rfwcdma_core_txplim_exp_hdet_vs_agc_ptr[ 0 ] )
     {
        low_zone = i;
     }
     else
        /* exit from loop when end of agc low zone is found */
        /* The end of agc low zone is indexed by the previous agc zone index */
        break;
  }
  
  /* Find the top of the expected HDET curve.  The curve is flat
  ** at the top and bottom, and sloping in the middle.
  **
  ** note: RF Cal makes sure that the points above the curve all
  ** have the same value.
  */
  
  /* set start of high zone to the last agc zone index, index 15 */
  high_zone = RFTXPLIM_EXP_HDET_VS_AGC_MAX_IDX;
  
  /* traverse agc zone index 1 to 14 */
  /* to find the start of high agc zone */
  for ( i = 1; i < RFTXPLIM_EXP_HDET_VS_AGC_MAX_IDX; i++ ) 
  {
  
     /* if current HDET is >= than the last HDET then the start of high zone */
     /* if found, stop search */
     if ( rfwcdma_core_txplim_exp_hdet_vs_agc_ptr[ i ] >= 
          rfwcdma_core_txplim_exp_hdet_vs_agc_ptr[ RFTXPLIM_EXP_HDET_VS_AGC_MAX_IDX ] )
     {
        /* The start of agc high zone is indexed by the previous agc zone index */
        high_zone = i - 1;
  
        /* exit from loop when beginning of high zone is found */
        break;
     }
  }
  
  /* to interpolate the TX AGC value from HDET value */
     /* Calculate the agc zone (segment) to use for interpolation */
  
     /* If below the curve */
     if ( hdet_idx <= rfwcdma_core_txplim_exp_hdet_vs_agc_ptr[ 0 ] )
     {
        zone = low_zone;
     }
     /* Else if above the curve */
     else if ( hdet_idx >= rfwcdma_core_txplim_exp_hdet_vs_agc_ptr[ RFTXPLIM_EXP_HDET_VS_AGC_MAX_IDX ] ) 
     {
        zone = high_zone;
     } 
     /* Else, on the curve */
     else
     {
        /* Find where in the middle segment rfnv_exp_hdet_vs_agc[] table the hdet_idx lies */      
        zone = rfwcdma_core_binary_search(hdet_idx, low_zone, high_zone+1);
     }

     RF_MSG_2(RF_HIGH,"idx %d, zone %d", hdet_idx, zone);
  
     /* Calculate the hdet delta from the start this agc zone */
     /* This is the delta between current HDET value and HDET value */
     /* at the start of the agc zone */
     if ( hdet_idx <= rfwcdma_core_txplim_exp_hdet_vs_agc_ptr[ zone ] )
     {
        /* this is very unlikely */
        delta = 0;
     }
     else
     {
        /* calculate the hdet delta */
        delta = hdet_idx - (uint16) rfwcdma_core_txplim_exp_hdet_vs_agc_ptr[ zone ];
     }
  
     /* Calculate the TX AGC offset */
     /* The TX AGC offset is equal to the start of agc zone multiplied  by 16 */
     /* why ? The agc zone 0..15 is equivalent to TX AGC offset 0..240 which */
     /* represents the top 25% of TX AGC range scaled down by 768. */
     /* The top 25% of TX AGC range 768..1023 is scaled down by 768 */
     /* to get 0..255 range */
     offset =  (zone * RFTXPLIM_EXP_HDET_VS_AGC_SIZ);
  
     /* Calculate the hdet rise for this segment */
     /* The delta between this zone and next zone HDET value */
     rise = rfwcdma_core_txplim_exp_hdet_vs_agc_ptr[ zone + 1 ] - 
            rfwcdma_core_txplim_exp_hdet_vs_agc_ptr[ zone ]; /*lint !e734 */
  
     /* Calculate the Tx AGC value from the AGC offset and slope */
     /* If rise is 0, just use the offset, because the slope is 0 */
     
     /* get TX AGC offset */
     result = offset;
  
     /* if hdet rise is not 0 */
     if ( rise != 0 )
    {
        /* calculate the TX AGC slope */
        /* Round up the hdet delta by hdet rise/2 */
        /* TX AGC slope = ( (hdet delta * 256) + hdet rise/2 ) / (hdet rise * 16) */
        /* TX AGC value = TX AGC offset + TX AGC slope */
       /*lint -e{734} */
        result += RF_ROUND( ((delta*RFTXPLIM_EXP_HDET_VS_AGC_TBL_SIZ) + rise/2),  (rise * RFTXPLIM_EXP_HDET_VS_AGC_SIZ) );
    }
  
     /* Saturate the TX AGC value and store it in the */
     /* expected AGC value versus HDET table */
     /* Scale the TX AGC value from 768..1023 range down to 0..255 range */
      txagc = 
        (uint8) RF_SATURATE( result,
                     RFTXPLIM_TX_AGC_MIN,
                     ( RFTXPLIM_TX_AGC_MAX - RFTXPLIM_TX_AGC_VS_HDET_TBL_OFF ) 
                   );

      RF_MSG_2( RF_HIGH, "RFTXPLIM-Interporation: ADC %d => TxAGC %d",
                hdet_idx, txagc );

      return txagc;


} /* rftxplim_calc_exp_agc_vs_hdet_table */


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function read HDET value from ADC and TX AGC value from MDSP and then
  call the rftxplim_adjust_tx_pwr_limit function to adjust the TX power limit
  based on temperature, frequency, and HDET feedback. This function is called
  by rf_do_tx_pwr_limit every 100ms.

  @details
  rftxplim_init must be called first.
  temperature must be read and temperature data must be ready.
  frequency index must be calculated.
  Updates MDSP TX power limit and PA range threshold


*/
void
rfwcdma_core_txplim_do_mtpl_handler
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
)
{
  uint16 hdet_adc = 0;
  int16 final_mtpl = 0;
  rfdevice_wcdma_hdet_read_type hdet_output = {0};
  rfcom_wcdma_band_type rf_band;
  rfwcdma_mdsp_get_data_param_type mdsp_param = {0};
  rfwcdma_mdsp_set_data_param_type set_power_meter_state = { 0 };
  boolean valid_txagc = TRUE;
  boolean hdet_read_en = TRUE;
  boolean irat_script = FALSE;
  rfwcdma_mc_state_enum_type rf_state;
  rflm_wcdma_do_mtpl_cmd* fbrx_data = NULL;

  rflm_wcdma_dm_buf_type *wcdma_dm_data_ptr = NULL;
  boolean buffer_valid = FALSE;
  /* @TODO: These are temp vars needed for HDET calculaion from ILPC data
            Remove this when device support is available
   */
  uint64 abs_sum_i = 0;
  uint64 abs_sum_q = 0;
  uint64 hdet_reading_u64 = 0;
  uint32 fbrx_accum = 0;

  /* Sanity - check if data ptr from MSGR is valid */
  if(cmd_ptr == NULL)
  {
    RF_MSG(RF_ERROR,"rfwcdma_core_txplim_do_mtpl_handler: NULL MSGR data ptr!");
    return;
  }

  /* Get the payload from MSGR cmd_ptr */
  fbrx_data = (rflm_wcdma_do_mtpl_cmd*)((rf_cmd_type*)cmd_ptr)->payload;

  /* Print RFLM timing MSG */
  RF_MSG_6(RF_HIGH,
           "[WCDMA_METER][Timing]: uses_ilpc_data %d, tx_slot_boundary %d, stable_tx start %d, stable_tx stop %d, meter_trigger %d, cb %d",
           fbrx_data->uses_ilpc_data,
           fbrx_data->txagc_boundary_time,
           fbrx_data->stable_tx_start_time,
           fbrx_data->stable_tx_stop_time,
           fbrx_data->trigger_time,
           fbrx_data->cb_stmr_time
          );

  rf_band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx;
  rf_state = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].rf_state;
  irat_script = rfwcdma_mc_state.irat_script;

  if((RFWCDMA_MC_STATE_RXTX != rf_state)||(irat_script== TRUE))
  {
    RF_MSG_4( RF_HIGH, "rfwcdma_core_txplim_do_mtpl: invalid rf_state=%d or irat_script_building %d"
                        " device=%d rf_band=%d", 
                         rf_state,irat_script, RFM_DEVICE_0, rf_band);
    if (rfwcdma_core_txplim_mode == RFTXPLIM_MODE_TRK)
    {
      rfwcdma_mdsp_set_hdet_interval(150);
    }
    hdet_read_en = FALSE;

    if (!fbrx_data->uses_ilpc_data)
    {
      rflm_wcdma_unlock_meter_trigger( (rflm_handle_tx_t)rfwcdma_mc_state.txlm_buf_idx );
    }
  }

  /* Update Counter */
  if(rfwcdma_core_txplim_counter == 0xFFFF)
  {
    /* Avoid Overflow */
    rfwcdma_core_txplim_counter = rfwcdma_core_txplim_init_acq_counter;
  }
  else
  {
    rfwcdma_core_txplim_counter++; /* Increment counter */
  }

  if(TRUE == hdet_read_en)
  {
     /* Get HDET data from device if ILPC is not active */
     if (!fbrx_data->uses_ilpc_data)
     {
        (void)rfdevice_hdet_wcdma_read_hdet( RFM_DEVICE_0,
                                             rf_band,
                                             TRUE,
                                             &hdet_output );

		rflm_wcdma_unlock_meter_trigger( (rflm_handle_tx_t)rfwcdma_mc_state.txlm_buf_idx );
     }
     /* Otherwise compute HDET from ILPC data */
     else
     {
        /* Check if ILPC data is valid */
        hdet_output.hdet_quality = fbrx_data->ilpc_data_valid;

        /* @TODO: Following is algo exerpt from device
                  Remove this when 
         */
        if (hdet_output.hdet_quality)
        {
           /* this part is based on device driver HDET calculations */
           fbrx_accum = fbrx_data->num_samples;
           
           abs_sum_i = (uint64)(((fbrx_data->sum_i)<0) ? -(fbrx_data->sum_i) : (fbrx_data->sum_i));
           abs_sum_q = (uint64)(((fbrx_data->sum_i)<0) ? -(fbrx_data->sum_q) : (fbrx_data->sum_q));

           /* avg_hdet = (sum_i_and_q_squred - (sum_i^2+sum_q^2)/n)/n */
           hdet_reading_u64 = (uint64) ((fbrx_data->sum_i_q_sqr<<8)-((abs_sum_i*abs_sum_i+abs_sum_q*abs_sum_q)/fbrx_accum))/fbrx_accum;

           if ( (hdet_reading_u64 >> 29) > 0)
           {
              /* make the hdet read saturate at max limit */
              hdet_output.hdet_reading = 0xffff;
           }
           else
           {
              /* take 16 bits between bit [11:26] from the division result */
              hdet_output.hdet_reading = (uint16) ((hdet_reading_u64 & 0x1fffe000) >> 13 );
           }
        }

        /* If debug msg is enabled then print dbg */
        if (rfwcdma_core_txplim_dbgmsg)
        {
           RF_MSG_7( RF_HIGH,
                     "[WCDMA_METER][Data]: "
                     "uses_ilpc_data %d, ilpc_data_valid %d, sum_i %d, sum_q %d, sum_i_q_sqr %d, num_samples %d, hdet_adc %d",
                     fbrx_data->uses_ilpc_data,
                     fbrx_data->ilpc_data_valid,
                     fbrx_data->sum_i,
                     fbrx_data->sum_q,
                     fbrx_data->sum_i_q_sqr,
                     fbrx_data->num_samples,
                     hdet_output.hdet_reading
                   );
        }
     } /* end else for calculating HDET from FBRx ILPC */

    hdet_adc = hdet_output.hdet_reading;

    /* Read Buffered TxAGC */
    mdsp_param.get_data_cmd = RFWCDMA_MDSP_GET_METER_BUFFERED_TXAGC;
    mdsp_param.rflm_handle = (rflm_dm_handle_id_t)rfwcdma_mc_state.txlm_buf_idx;
    valid_txagc = rfwcdma_mdsp_async_get_tx_data(&mdsp_param);
    if ( valid_txagc == TRUE )
    {
       rfwcdma_core_txplim_txagc = mdsp_param.data.gen_w - RFAGC_MIN_AGC_VAL;
    }

    /* Indicate that Power Meter is not active anymore */
    set_power_meter_state.set_data_cmd = RFWCDMA_MDSP_SET_POWER_METER_STATE;
    set_power_meter_state.rflm_handle = (rflm_dm_handle_id_t)rfwcdma_mc_state.txlm_buf_idx;
    set_power_meter_state.data.gen_b = FALSE;
    rfwcdma_mdsp_async_set_data(&set_power_meter_state);
  }

  
       
  buffer_valid = rflm_dm_get_handle_tech_data((rflm_dm_handle_id_t)rfwcdma_mc_state.txlm_buf_idx, (void *)&wcdma_dm_data_ptr, RFLM_DM_TECH_DATA_GET_PTR);

  if ((buffer_valid  == TRUE) && (wcdma_dm_data_ptr != NULL))
  {
    wcdma_dm_data_ptr->tx_data.tx_status.power_meter_in_progress = FALSE;
  }

  if((valid_txagc == FALSE) ||
     (hdet_output.hdet_quality == FALSE) || 
     (hdet_adc < rfwcdma_core_txplim_exp_hdet_vs_agc_ptr[0]) ||
     (hdet_adc > rfwcdma_core_txplim_exp_hdet_vs_agc_ptr[RFTXPLIM_EXP_HDET_VS_AGC_MAX_IDX]) ||
     (rfwcdma_core_txplim_txagc > RFTXPLIM_TX_PWR_LIM_MAX) ||
     (rfwcdma_core_txplim_txagc < RFTXPLIM_TX_PWR_LIM_MIN) ||
     (FALSE == hdet_read_en)||
     (0xFFFF == hdet_adc))  //0xFFFF is the invalid value. we need discard it
  {
    /* HDET Invalid */
    rfwcdma_core_txplim_mode = RFTXPLIM_MODE_INVALID;
  
    /* Invalid HDET reading */
    rfwcdma_core_txplim_hdet = 0xFFFF; /*lint !e569 */

    if(rfwcdma_core_txplim_dbgmsg)
    {
      MSG_6(MSG_SSID_RF, MSG_LEGACY_HIGH,
            "RFTXPLIM-INVALID:%05d,TxAGC:%03d,HDET_adc:%d,Desired:%03d,MTPL:%03d.%02d",
            rfwcdma_core_txplim_counter,
            rfwcdma_core_txplim_txagc + RFAGC_MIN_AGC_VAL,
            hdet_adc,
            rfwcdma_core_txplim_desired + RFAGC_MIN_AGC_VAL,
            /* MTPL Integer part */
            RFTXPLIM_Q5_TO_INT(rfwcdma_core_txplim_mtpl_q5)+RFAGC_MIN_AGC_VAL,
            /* MTPL Fraction part */
            RFTXPLIM_Q5_FRACTION(rfwcdma_core_txplim_mtpl_q5)); /*lint !e502 */
    }
  }
  else
  {
    /* HDET AGC */
    rfwcdma_core_txplim_hdet = rfwcdma_core_txplim_hdet_to_txagc(hdet_adc) + 
                                                     RFTXPLIM_TX_AGC_VS_HDET_TBL_OFF;

    rfwcdma_core_txplim_hdet_adc = hdet_adc;

    if( (rfwcdma_core_txplim_enabled)&& rfwcdma_core_txplim_desired >= RFTXPLIM_TX_PWR_LIM_MIN)
    {
      /* Adjust TX power limit */
      rfwcdma_core_txplim_adjust_mtpl();
    }
    else
    {
      /* HDET Disabled */
      rfwcdma_core_txplim_mode = RFTXPLIM_MODE_DISABLED;
      
      if(rfwcdma_core_txplim_dbgmsg)
      {
        MSG_7(MSG_SSID_RF, MSG_LEGACY_HIGH,
          "RFTXPLIM-DISABLED:%05d,TxAGC:%03d,HDET:%03d (ADC %d),Desired:%03d,MTPL:%03d.%02d",
          rfwcdma_core_txplim_counter,
          rfwcdma_core_txplim_txagc + RFAGC_MIN_AGC_VAL,
          rfwcdma_core_txplim_hdet + RFAGC_MIN_AGC_VAL,
          hdet_adc,
          rfwcdma_core_txplim_desired + RFAGC_MIN_AGC_VAL,
          /* MTPL Integer part */
          RFTXPLIM_Q5_TO_INT(rfwcdma_core_txplim_mtpl_q5)+RFAGC_MIN_AGC_VAL,
          /* MTPL Fraction part */
          RFTXPLIM_Q5_FRACTION(rfwcdma_core_txplim_mtpl_q5)); /*lint !e502 */
      }
    }
  }
  /* Convert final MTPL to TxAGC unit */
  final_mtpl = RFTXPLIM_Q5_TO_INT(rfwcdma_core_txplim_mtpl_q5) + RFAGC_MIN_AGC_VAL;

  /* Update FW interface only in FTM non-Cal mode. In online or NS mode it will be
  updated by L1 with value returned by RF */
  if (IS_FTM_IN_TEST_MODE()&&(rfm_get_calibration_state() != TRUE))
  {
    rfwcdma_mdsp_update_mtpl(final_mtpl);
  }
  return;
} /* rfwcdma_core_txplim_do_mtpl_handler */

/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Override the power limiting mode of operation
 
  @details
  This function overrides the PLIM loop to enable or disable mode
 
  @param device
  RFM Device on which PLIM is to be overriden
 
  @param plim_enable
  Flag indicating, if the PLIM is to be enabled
 
  @return
  Status of plim override
*/
boolean
rfwcdma_plim_override_mode
(
  rfm_device_enum_type device,
  boolean plim_enable
)
{
  rfwcdma_power_limiting_data_type *plim_data = NULL;

  plim_data = rfwcdma_get_power_limiting_data();

  if ( plim_data == NULL )
  {
    RF_MSG(RF_ERROR,"rfwcdma_plim_override_mode: NULL PLIM Data");
    return FALSE;
  }

  plim_data->enable_plim = plim_enable;

  RF_MSG_2( RF_HIGH,"rfwcdma_plim_override_mode: Device %d - PLIM overriden to %d", 
            device, plim_enable);

   return TRUE;
  
} /* rfwcdma_plim_override_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
 load MPR backoff value
 
  @details
  This function is used to load MPR backoff value
 
  @param device
  RFM Device on which HDET is to be enabled
 
  @param multi_carrier_idx
  bit mask for tx multi carriers
 
  @return
  none
*/
void 
rfwcdma_core_txplim_load_mpr_backoff
(
  rfcom_wcdma_band_type band,
  uint32 multi_carrier_idx
)
{
  uint8 i;
  int8 mpr_table_entries;

  /*If combination value exist, use new value, otherwisw use old value*/
  if((rfnv_wcdma_tbl_ptr[band]->mpr_backoff.mpr_table_entries > 0) && 
      (rfnv_wcdma_tbl_ptr[band]->mpr_backoff.mpr_table_entries <= RF_WCDMA_MAX_NUM_MPR_BINS))
  {
    mpr_table_entries = rfnv_wcdma_tbl_ptr[band]->mpr_backoff.mpr_table_entries;
    RF_MSG_2( RF_LOW, "rfwcdma_core_txplim_load_mpr_backoff: multi_carrier_idx=%d "
                         "mpr_table_entries=%d", multi_carrier_idx, mpr_table_entries);

    for ( i=0; i < mpr_table_entries; i++ )
    {
      if(RFWCDMA_MC_DC == multi_carrier_idx)
      {
        rfwcdma_core_txplim_mprVals[i]     = 
                 (uint8)rfnv_wcdma_tbl_ptr[band]->mpr_backoff.dc_backoff.tx_mpr_backoff[i];
        rfwcdma_core_txplim_betaBackoff[i] = 
                 rfnv_wcdma_tbl_ptr[band]->mpr_backoff.dc_backoff.tx_beta_scaling_comp[i];
        rfwcdma_core_txplim_txAgcOffset[i] = 
                 (uint8)rfnv_wcdma_tbl_ptr[band]->mpr_backoff.dc_backoff.tx_agc_offset[i];
      }
      else
      {
        rfwcdma_core_txplim_mprVals[i]     = 
                 (uint8)rfnv_wcdma_tbl_ptr[band]->mpr_backoff.sc_backoff.tx_mpr_backoff[i];
        rfwcdma_core_txplim_betaBackoff[i] = 
                 rfnv_wcdma_tbl_ptr[band]->mpr_backoff.sc_backoff.tx_beta_scaling_comp[i];
        rfwcdma_core_txplim_txAgcOffset[i] = 
                 (uint8)rfnv_wcdma_tbl_ptr[band]->mpr_backoff.sc_backoff.tx_agc_offset[i];
      }

      /* Initialize MPR table here. For FTM, we need beta scaling filled out even wihtout MTPL */
      rfwcdma_mdsp_set_mpr_beta_table(i, rfwcdma_core_txplim_mprVals[i], rfwcdma_core_txplim_betaBackoff[i], rfwcdma_core_txplim_txAgcOffset[i]);

      if(i< mpr_table_entries-1)
      {
        if(RFWCDMA_MC_DC == multi_carrier_idx)
        {
          rfwcdma_core_txplim_paThresholdOffset[i] = 
                 (uint8)rfnv_wcdma_tbl_ptr[band]->mpr_backoff.dc_backoff.pa_mpr_backoff[i];
        }
        else
        {
          rfwcdma_core_txplim_paThresholdOffset[i] = 
                  (uint8)rfnv_wcdma_tbl_ptr[band]->mpr_backoff.sc_backoff.pa_mpr_backoff[i];
        }
        rfwcdma_mdsp_set_pa_thresh_offset(i, rfwcdma_core_txplim_paThresholdOffset[i]);
      }
    }
  }
  else
  {
    mpr_table_entries = RFTXPLIM_MPR_BETA_TABLE_ENTRIES;

    for ( i=0; i < mpr_table_entries; i++ )
    {
      rfwcdma_core_txplim_mprVals[i]     = (uint8)rfnv_wcdma_tbl_ptr[band]->wcdma_rel6_tx_mpr_backoff[i];
      rfwcdma_core_txplim_betaBackoff[i] = rfnv_wcdma_tbl_ptr[band]->wcdma_rel6_tx_beta_scaling_comp[i];
      rfwcdma_core_txplim_txAgcOffset[i] = (uint8)rfnv_wcdma_tbl_ptr[band]->wcdma_rel6_tx_agc_offset[i];

      /* Initialize MPR table here. For FTM, we need beta scaling filled out even wihtout MTPL */
      rfwcdma_mdsp_set_mpr_beta_table(i, rfwcdma_core_txplim_mprVals[i], rfwcdma_core_txplim_betaBackoff[i], rfwcdma_core_txplim_txAgcOffset[i]);

      if(i< mpr_table_entries-1)
      {
        rfwcdma_core_txplim_paThresholdOffset[i] = (uint8)rfcommon_nv_mm_tbl.wcdma_rel7_pa_mpr_backoff[i];
        rfwcdma_mdsp_set_pa_thresh_offset(i, rfwcdma_core_txplim_paThresholdOffset[i]);
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
/*!
  @brief
  Returns UE max tx limit/Desired Limit in dBm10 units

  @details
  Returns MTPL in dBm * 10 or dBm10 units.

*/

int16 rfwcdma_core_txplim_get_desired_limit_dbm10(void)
{
  return( rfwcdma_core_txplim_desired + (RF_MIN_TX_POWER_DBM * 10) );
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  reset the mtpl with mtp_adj_db10.

  @details
  reset the mtpl with mtp_adj_db10. mtpl_adj_db10 is the value we passed to FW to 
  adjust the mtpl
 
  @param 
  none

  @return
  none
*/
void rfwcdma_core_txplim_reset_mtpl_during_temp_comp(void)
{
  int16 mtpl_adj_db10;
  int16 final_mtpl;

  /* Convert final MTPL to TxAGC unit */
  final_mtpl = RFTXPLIM_Q5_TO_INT(rfwcdma_core_txplim_mtpl_q5) + RFAGC_MIN_AGC_VAL;

  mtpl_adj_db10 = rfwcdma_mdsp_async_get_mtpl_adj_db10(rfwcdma_mc_state.txlm_buf_idx);
  final_mtpl += mtpl_adj_db10;

  /* Update FW interface only in FTM non-Cal mode. In online or NS mode it will be
  updated by L1 with value returned by RF */
  if (IS_FTM_IN_TEST_MODE()&&(rfm_get_calibration_state() != TRUE))
  {
    rfwcdma_mdsp_update_mtpl(final_mtpl);
  }
}

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
void rfwcdma_core_txplim_reset_mtpl()
{
  uint16 init_mtpl  =  0;
  init_mtpl = RFTXPLIM_MAX_TX_PWR_LIM_IN_SW_UNIT + RFAGC_MIN_AGC_VAL;
  if (IS_FTM_IN_TEST_MODE()&&(rfm_get_calibration_state() != TRUE))
  {
    rfwcdma_mdsp_update_mtpl(init_mtpl);
  }
}
