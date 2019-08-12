/*!
   @file rftxplim_mm_report_tx_power.c

   @brief
   This file contains structure and function prototype for acquring TX power
   status from remote RTR

   @details

*/

/*===========================================================================
Copyright (c) 2010 - 2013  by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/sv/common/src/rftxplim_mm_report_tx_power.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/10/16   rs      Compiler warning fix for C2K disabled builds
09/23/14   pk      compiler warning fix
12/11/13   sty     deleted set-but-unused var
12/10/13   sbo     Added missing timer_def2 for rf_tx_measurement_report_isr
03/19/13   br      Remove un-used F3 messages.
01/18/13   fh        Triton compile error fixed
01/17/13   sty     skip updating max power limit in rfsv_compute_sar_tx_pwr()
                   -- this will come from MCS
01/21/13   bsh     Fixed compiler warnings due to moving some APIs to rflte_ext_mc.h 
12/07/12   gvn     Added IM3 backoff for LTE B2/B25 and 1X B15 combination 
                   (Merge from Nikel CR399105)
11/29/12   gvn     Added rflte_core_get_num_rb_from_bandwidth()
11/29/12   av      Using proper featurization and removing featurization that is not needed
06/28/12   sbm     Bug fix: type cast to int32 in IM3 calculation.
06/21/12   zhw     Off-target test ASSERT fix. 
06/07/12   pl      Update to LTE MC state machine with CA support
06/04/12   vvr     Fix for Off Target compile issues  
05/16/12   php     Add IM3 backoff for LTE B2/1x BC1
05/24/12   ndb     Implementation of IM2 backoff for LTE B4 and 1xBC0
05/23/12   ndb     Implementation of IM3 backoff for GPS desense
05/15/12   sbm     added RFCOM_BAND_LTE_B2 in  rfsv_get_lte_sar_table() 
05/03/12   php     Removed invalid global variable assignment
02/21/12   sar     Removed unused/obsolete code.
01/10/12   whc     Corrected naming convention of F3's
11/29/11   whc     Improved debug messages for SV LTE SAR Backoff
11/29/11   sty     Fixed compile errors
11/17/11   Saul    SV Limiting. Added LTE B25 support. New SVDO Chans Combo
11/14/11   Saul    SV Limiting. Added support for 1x-HDR Half-Duplex and 
                   DSI backoffs.
11/11/11   Saul    SV Limiting. LTE backoff support.
11/10/11   Saul    SV Limiting. L1 interface update.
11/10/11   Saul    SV Limiting. KW fix.
11/10/11   Saul    SV Limiting. Initial support.
08/11/11    ap     Get SAR State API no longer accepts parameter  
06/16/11   sar     Included internal 1x interface.
05/04/11   pl      SV-LTE SAR non-RPC implementation
05/04/11   pl      remove use of rfm_mode
12/13/10   kma     Added message sequence number for RPC communication
12/06/10   kma     Fixed CMI compliance bug
10/14/10   kma     Reverted one of the previous compilation warning fixes
10/14/10   kma     Fixed compilation warnings
09/23/10   kma     Fixed bug in F3 message
09/13/10   kma     Fixed the boot up race condition on Fusion
07/23/10   kma     Fixed agc to dbm conversion resolution bug
07/13/10   kma     Fixed TX power resolution bug
07/09/10   kma     Acquire total filtered 1x TX power from FW
07/09/10   kma     Used ONCRPC PROXY task to call RPC API
07/02/10   kma     Used REX task to call RPC API
07/01/10   kma     Added RPC remote API call
06/28/10   kma     Added API to convert agc to dbm for 1X TX power
                   Moved RPC API to a a separate file
06/08/10   kma     Initial version
============================================================================*/

#include "rfcommon_core_sar.h"
#include "rftxplim_mm_report_tx_power.h"
#ifdef FEATURE_LTE
#include "lte_rrc_ext_msg.h"
#include "rflte_core_txpl.h"
#include "rflte_mc.h"
#include "rflte_core_util.h"
#include "rflte_ext_mc.h"
#endif
#include "rf_1x_mc.h"
#include "rf_hdr_mc.h"
#include "rfm_internal.h"
#include "rfcommon_msg.h"
#include "rf_cdma_utils_freq.h"
#include "rfgnss_mc.h"
#include "rf_cdma_utils.h"


#define RF_CDMA_MDSP_TX_TOTAL_GAIN_OFFSET      1024
#define RF_CDMA_MDSP_TX_TOTAL_GAIN_MASK        0x3ff
#define RF_TX_AGC_VAL_OFFSET 512
#define TX_AGC_VAL_MASK 0x3ff
#define TX_AGC_VAL_CNTS_PER_DB_85_DB 12
#define TX_AGC_VAL_CNTS_PER_DB_102_DB 10

boolean hdr_normal_power_limit_active = TRUE;
boolean lte_normal_power_limit_active = TRUE;

rfsv_data_tech_tx_status_type rfsv_data_tech_tx_status =
{
  RFM_INVALID_DEVICE,
  FALSE,
  RFM_PARKED_MODE,
  RF_MAX_BAND,
  0,
  { 0 },
};

rfsv_data_tech_tx_status_type rfsv_voice_tech_tx_status =
{
  RFM_INVALID_DEVICE,
  FALSE,
  RFM_PARKED_MODE,
  RF_MAX_BAND,
  0,
  { 0 },
};

#ifdef FEATURE_LTE
static boolean old_lte_normal_back_off_status = TRUE;
#endif /*FEATURE_LTE*/

timer_type rf_tx_measurement_report_isr;   /* Call back struct for remote power control */

int16 rfsv_search_lte_pwr_limits_in_lut
(
   int16 cdma_tx_power_dbm,
   int16 **tbl_ptr,
   rf_card_band_type band,
   int16 bias_input,
   int16 bias_output
   );

void rfsv_get_lte_sar_table
(
   int32 cdma_curr_band,
   rfcom_lte_band_type lte_curr_band,
   int16 **sar_table_ptr,
   int16 **dsi_table_ptr
   );

void rfsv_get_dsi_sar_bias
(
   int16 tbl_ptr[MAX_NUM_OF_DSI_STATE][2],
   int16 *input_bias,
   int16 *output_bias
   );

boolean rfsv_check_lte_1x_im3_ingps_band(uint32 low_freq, uint32 high_freq, uint32 lowband_bw, uint32 highband_bw);


boolean rfsv_check_lte_1x_im3_in_lte_rx_band(uint32 fc_1xtx, uint32 fc_ltetx, uint32 fc_lterx, uint32 bw_1x, uint32 bw_lte);

boolean rfsv_check_lte_1x_im3_in_1x_rx_band(uint32 fc_1xtx, uint32 fc_1xrx, uint32 fc_ltetx, uint32 bw_1x, uint32 bw_lte);

boolean rfsv_check_lte_1x_im2_in_1x_rx_band(uint32 fc_ltetx, uint32 lte_bw, uint32 fc_cdmatx, uint32 fc_cdmarx);


/*===========================================================================

FUNCTION rfsv_get_dsi_sar_bias

DESCRIPTION
  This API gets the input/output bias for DSI
  SAR algorith,

DEPENDENCIES
  None

RETURN VALUE
  Input/output for DSI SAR

SIDE EFFECTS
  None

===========================================================================*/
void rfsv_get_dsi_sar_bias(int16 tbl_ptr[MAX_NUM_OF_DSI_STATE][2],
                           int16 *input_bias,
                           int16 *output_bias)
{
  rfm_common_sar_state_type sar_type;

  sar_type = rfcommon_core_sar_get_state();

  if ((input_bias == NULL) || (output_bias == NULL))
  {
    RF_MSG_3(RF_ERROR, "RF SV Limit : NULL POINTER!   DSI state = %d; &input_bias = %x; &output_bias = %x ",
             sar_type, input_bias, output_bias);
    return;
  }

  if (tbl_ptr == NULL)
  {
    *input_bias = 0;
    *output_bias = 0;
    return;
  }

  switch (sar_type)
  {
  case RF_SAR_STATE_DEFAULT:
    /*Zero Bias is applied in default state*/
    *input_bias = 0;
    *output_bias = 0;
    break;

  case RF_SAR_STATE_1:
    *input_bias = tbl_ptr[0][0];
    *output_bias = tbl_ptr[0][1];
    break;

  case RF_SAR_STATE_2:
    *input_bias = tbl_ptr[1][0];
    *output_bias = tbl_ptr[1][1];
    break;

  case RF_SAR_STATE_3:
    *input_bias = tbl_ptr[2][0];
    *output_bias = tbl_ptr[2][1];
    break;

  case RF_SAR_STATE_4:
    *input_bias = tbl_ptr[3][0];
    *output_bias = tbl_ptr[3][1];
    break;

  case RF_SAR_STATE_5:
    *input_bias = tbl_ptr[4][0];
    *output_bias = tbl_ptr[4][1];
    break;

  case RF_SAR_STATE_6:
    *input_bias = tbl_ptr[5][0];
    *output_bias = tbl_ptr[5][1];
    break;

  case RF_SAR_STATE_7:
    *input_bias = tbl_ptr[6][0];
    *output_bias = tbl_ptr[6][1];
    break;

  case RF_SAR_STATE_8:
    *input_bias = tbl_ptr[7][0];
    *output_bias = tbl_ptr[7][1];
    break;

  case RF_SAR_STATE_MAX:
  default:
    *input_bias = 0;
    *output_bias = 0;
    break;
  }
  RF_MSG_3(RF_MED, "RF SV Limit : DSI state = %d; input_bias = %d; output_bias = %d ",
           sar_type, *input_bias, *output_bias);
}

/*===========================================================================

FUNCTION rfsv_get_lte_sar_table

DESCRIPTION
  This function returns the 1X/LTE SAR and DSI offset table based on 1x Band and LTE band.

DEPENDENCIES
  None

RETURN VALUE
  SAR table and corresponding DSI table

SIDE EFFECTS
  None

===========================================================================*/
void rfsv_get_lte_sar_table
(
   int32 cdma_curr_band,
   rfcom_lte_band_type lte_curr_band,
   int16 **sar_table_ptr,
   int16 **dsi_table_ptr
   )
{

  switch (lte_curr_band)
  {
  case RFCOM_BAND_LTE_B2:
    switch (cdma_curr_band)
    {
    case RF_BC0_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc2_sar[0][0];
      dsi_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc2_dsi_sar[0][0];
      break;
    case RF_BC1_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc2_sar[0][0];
      dsi_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc2_dsi_sar[0][0];
      break;
    case RF_BC15_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc2_sar[0][0];
      dsi_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc2_dsi_sar[0][0];
    default:
      break;
    }
    break;

  case RFCOM_BAND_LTE_B4:
    switch (cdma_curr_band)
    {
    case RF_BC0_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc4_sar[0][0];
      dsi_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc4_dsi_sar[0][0];
      break;
    case RF_BC1_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc4_sar[0][0];
      dsi_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc4_dsi_sar[0][0];
      break;
    case RF_BC6_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc4_sar[0][0];
      break;
    case RF_BC15_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc4_sar[0][0];
      dsi_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc4_dsi_sar[0][0];
    default:
      break;
    }
    break;

  case RFCOM_BAND_LTE_B5:
    switch (cdma_curr_band)
    {
    case RF_BC0_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc5_sar[0][0];
      break;
    case RF_BC1_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc5_sar[0][0];
      break;
    case RF_BC4_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc4_lte_bc5_sar[0][0];
      dsi_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc4_lte_bc5_dsi_sar[0][0];
      break;
    case RF_BC15_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc5_sar[0][0];
      break;
    default:
      break;
    }
    break;

  case RFCOM_BAND_LTE_B12:
    switch (cdma_curr_band)
    {
    case RF_BC0_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc12_sar[0][0];
      dsi_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc12_dsi_sar[0][0];
      break;
    case RF_BC1_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc12_sar[0][0];
      dsi_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc12_dsi_sar[0][0];
      break;
    case RF_BC15_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc12_sar[0][0];
      dsi_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc12_dsi_sar[0][0];
      break;
    default:
      break;
    }
    break;

  case RFCOM_BAND_LTE_B13:
    switch (cdma_curr_band)
    {
    case RF_BC0_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_sar[0][0];
      dsi_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_dsi_sar[0][0];
      break;
    case RF_BC1_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc13_sar[0][0];
      dsi_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc13_dsi_sar[0][0];
      break;
    case RF_BC6_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc13_sar[0][0];
    default:
      break;
    }
    break;

  case RFCOM_BAND_LTE_B17:
    switch (cdma_curr_band)
    {
    case RF_BC0_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc17_sar[0][0];
      break;
    case RF_BC1_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc17_sar[0][0];
      break;
    case RF_BC6_BAND:
      sar_table_ptr = (int16 **)&rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc17_sar[0][0];
    default:
      break;
    }
    break;
  default:
    break;
  }
}

/*===========================================================================

FUNCTION RFSV_SEARCH_PWR_LIMITS_IN_LUT

DESCRIPTION
  This function SEARCHES the max power limits based 
  input cdma Tx power in dBm


DEPENDENCIES
  None

RETURN VALUE
  New power limit in dBm based on SAR algorithm

SIDE EFFECTS
  None

===========================================================================*/
int16 rfsv_search_lte_pwr_limits_in_lut(int16 cdma_tx_power_dbm,
                                        int16 **tbl_ptr,
                                        rf_card_band_type band,
                                        int16 bias_input,
                                        int16 bias_output)
{
  int i = 0;

  if (tbl_ptr == NULL)
  {
    return DEFAULT_FAIL_SAFE_TX_POWER_LIM;
  }

  if (cdma_tx_power_dbm < (tbl_ptr[0][0] + bias_input))
  {
    /*There is no need to search and return default max power*/
    return LTE_P_MAX;
  }
  else
  {
    while (i < MAX_NUM_OF_ROWS && (cdma_tx_power_dbm >= (tbl_ptr[i][0] + bias_input)))
    {
      i++;
    }
  }
  return (tbl_ptr[i - 1][1] + bias_output);
}

/*===========================================================================

FUNCTION RFSV_COMPUTE_SAR_PWR_LIMIT                           INTERNAL FUNCTION

DESCRIPTION
  This function SEARCHES the max power limits based 
  on SAR algorithm. This API is designed specifically
  for targets with remote RF driver.


DEPENDENCIES
  None

RETURN VALUE
  New power limit based on SAR algorithm

SIDE EFFECTS
  None

===========================================================================*/
int16 rfsv_compute_sar_pwr_limit(int16 cdma_tx_power_dbm10,
                                 int16 tbl_ptr[MAX_NUM_OF_ROWS][2],
                                 int16 bias_input,
                                 int16 bias_output)
{

  int i = 0;
  int16 retval = 0;

  if (cdma_tx_power_dbm10 < (tbl_ptr[0][0] + bias_input))
  {
    /*There is no need to search and return default max power*/
    return HDR_P_MAX;
  }
  else
  {
    while ((i < MAX_NUM_OF_ROWS) &&
           (cdma_tx_power_dbm10 >= (tbl_ptr[i][0] + bias_input)))
    {
      i++;
    }
  }

  if (i > 0)
  {
    retval = (tbl_ptr[i - 1][1] + bias_output);
  }
  else
  {
    retval = tbl_ptr[0][1] + bias_output;
  }

  /* Debug Messages */
  RF_MSG_5(RF_MED, "rfsv_compute_sar_pwr_limit : cdma_tx_pwr_dbm10 = %d, bias_input = %d, bias_output = %d, i = %d, returning %d", cdma_tx_power_dbm10, bias_input, bias_output, i, retval);
  RF_MSG_5(RF_MED, "tbl_ptr[x][0] (Voice) =  %d, %d, %d, %d, %d", tbl_ptr[0][0], tbl_ptr[1][0], tbl_ptr[2][0], tbl_ptr[3][0], tbl_ptr[4][0]);
  RF_MSG_5(RF_MED, "tbl_ptr[x][1] (Data)  =  %d, %d, %d, %d, %d", tbl_ptr[0][1], tbl_ptr[1][1], tbl_ptr[2][1], tbl_ptr[3][1], tbl_ptr[4][1]);

  return retval;
}

/*===========================================================================

FUNCTION RFSV_GET_MIN_LTE_TX_POWER                            INTERNAL FUNCTION

DESCRIPTION
  This function returns the min LTE TX power calculated
  from SAR algorithm


DEPENDENCIES
  None

RETURN VALUE
  Min TX power

SIDE EFFECTS
  None

===========================================================================*/
int16 rfsv_get_min_lte_tx_power(lte_power_limit power)
{
  int16 current_lte_tx_limit = 0;

  current_lte_tx_limit = MIN(power.sar_limit, MIN(power.im3_limit, power.im2_limit));

  return current_lte_tx_limit;
}

/*===========================================================================

FUNCTION RFSV_SET_LTE_SAR_PWR_LIMITS_STATUS                 INTERNAL FUNCTION

DESCRIPTION
  This function sets flag to TRUE if regular pwr limit is
  used and FALSE if SAR/IM3 pwr limit is applied. This 
  API is designed specifically for targets with remote 
  RF driver.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfsv_set_lte_sar_pwr_limits_status(boolean normal_back_off)
{
  lte_normal_power_limit_active = normal_back_off;
}

/*===========================================================================

FUNCTION RFSV_GET_LTE_SAR_PWR_LIMITS_STATUS                 INTERNAL FUNCTION

DESCRIPTION
  This function returns TRUE if regular pwr limit is
  used and FALSE if SAR/IM3 pwr limit is applied. This 
  API is designed specifically for targets with remote 
  RF driver.


DEPENDENCIES
  None

RETURN VALUE
  Status of power limit

SIDE EFFECTS
  None

===========================================================================*/
boolean rfsv_get_lte_sar_pwr_limits_status(void)
{
  return lte_normal_power_limit_active;
}

/*===========================================================================

FUNCTION rfsv_compute_sar_tx_pwr 

DESCRIPTION
  This API will be called within 1X ISR to compute SAR Max Tx power backoff 
  based on the current 1X TX status and 1X tx power level.


DEPENDENCIES
  SAR POWER Limit NVs have been loaded

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfsv_compute_sar_tx_pwr(void)
{

  int16 bias_input = 0;
  int16 bias_output = 0;
  uint32 cdma1x_tx_freq = 0;
#ifdef FEATURE_LTE
  uint32 cdma1x_rx_freq = 0;
#endif/*FEATURE_LTE*/
int16 voice_tech_tx_lvl_dbm10 = 0;

  /* Obtain current Tx power from voice tech */
#ifdef FEATURE_CDMA1X
  voice_tech_tx_lvl_dbm10 =
     rf_1x_mc_get_total_tx_filtered_power(rfsv_voice_tech_tx_status.device);
#endif/*FEATURE_CDMA1X*/
  uint32 cdma1x_rf_chan = rfsv_voice_tech_tx_status.band_chan_array[0];

  /* Based on the data-tech's mode... */
  switch (rfsv_data_tech_tx_status.rf_mode)
  {

    /* data tech : HDR */
#ifdef FEATURE_HDR
  case RFM_1XEVDO_MODE:
    {
      uint32 hdr_tx_freq = 0;
      uint32 hdr_rf_chan = rfsv_data_tech_tx_status.band_chan_array[0];
      /* Provide a fail-safe TX power limit for HDR in event of error */
      old_hdr_power_limit.sar_limit = DEFAULT_FAIL_SAFE_TX_POWER_LIM;
      old_hdr_power_limit.half_duplex_desense_limit = HDR_P_MAX;

      /* HDR bands */
      switch (rfsv_data_tech_tx_status.rf_band)
      {

        /* hdr band : RF_BC0_BAND */
      case RF_BC0_BAND:
        {
          /* cdma bands */
          switch (rfsv_voice_tech_tx_status.rf_band)
          {

            /* cdma band : RF_BC0_BAND */
          case RF_BC0_BAND:
            {
              boolean part1a, part1b, part1, part2a, part2b, part2;
              /* Account for Half Duplex Desense only if the following channel
              combination is met */
              cdma1x_tx_freq = rf_cdma_get_tx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band),
                                                           cdma1x_rf_chan);
              hdr_tx_freq = rf_cdma_get_tx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_data_tech_tx_status.rf_band),
                                                        hdr_rf_chan);

              /* 1x Tx channel frequency is below DO Tx channel */
              part1a = (((cdma1x_rf_chan >= 1) && (cdma1x_rf_chan <= 799)) &&
                        ((hdr_rf_chan >= 1) && (hdr_rf_chan <= 799)) &&
                        ((hdr_rf_chan + 41) >= (cdma1x_rf_chan + 750)) &&
                        ((hdr_rf_chan - 41) <= (cdma1x_rf_chan + 750)));

              part1b = (((cdma1x_rf_chan >= 991) && (cdma1x_rf_chan <= 1023)) &&
                        ((hdr_rf_chan >= 1) && (hdr_rf_chan <= 799)) &&
                        ((hdr_rf_chan + 41) >= (cdma1x_rf_chan + 750 - 1023)) &&
                        ((hdr_rf_chan - 41) <= (cdma1x_rf_chan + 750 - 1023)));

              /* Combine parts 1a and 1b */
              part1 = ((cdma1x_tx_freq < hdr_tx_freq) && (part1a || part1b));

              /* 1x Tx channel frequency is above DO Tx channel */
              part2a = (((hdr_rf_chan >= 1) && (hdr_rf_chan <= 799)) &&
                        ((cdma1x_rf_chan >= 1) && (cdma1x_rf_chan <= 799)) &&
                        ((cdma1x_rf_chan + 41) >= (hdr_rf_chan + 750)) &&
                        ((cdma1x_rf_chan - 41) <= (hdr_rf_chan + 750)));

              part2b = (((hdr_rf_chan >= 991) && (hdr_rf_chan <= 1023)) &&
                        ((cdma1x_rf_chan >= 1) && (cdma1x_rf_chan <= 799)) &&
                        ((cdma1x_rf_chan + 41) >= (hdr_rf_chan + 750 - 1023)) &&
                        ((cdma1x_rf_chan - 41) <= (hdr_rf_chan + 750 - 1023)));

              /* Combine parts 2a and 2b */
              part2 = ((cdma1x_tx_freq > hdr_tx_freq) && (part2a || part2b));

              /* Decide if half duplex backoff should be applied. */
              if (part1 || part2)
              {
                /* Apply Half Duples Backoff */
                old_hdr_power_limit.half_duplex_desense_limit =
                   rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                              rfnv_cdma1x_sv_tbl.hdr_bc0_half_duplex,
                                              bias_input,
                                              bias_output);
              }

              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc0_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc0_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC0_BAND */
            break;

            /* cdma band : RF_BC1_BAND */
          case RF_BC1_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc0_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc0_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC1_BAND */
            break;

            /* cdma band : RF_BC4_BAND */
          case RF_BC4_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc0_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc0_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC4_BAND */
            break;

            /* cdma band : RF_BC6_BAND */
          case RF_BC6_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc0_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc0_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC6_BAND */
            break;

            /* cdma band : RF_BC10_BAND */
          case RF_BC10_BAND:
            {
              if (((30 * hdr_rf_chan + 20230) >= (25 * cdma1x_rf_chan + 22500)) &&
                  ((30 * hdr_rf_chan + 17770) <= (25 * cdma1x_rf_chan + 22500))
                  )
              {
                /*Reset Bias for half-duplex de-sense computation*/
                old_hdr_power_limit.half_duplex_desense_limit =
                   rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                              rfnv_cdma1x_sv_tbl.hdr_bc0_half_duplex,
                                              bias_input,
                                              bias_output);
              }
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc0_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc0_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC10_BAND */
            break;

            /* cdma band : RF_BC15_BAND */
          case RF_BC15_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc0_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc0_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC15_BAND */
            break;

          default:
            RF_MSG_3(RF_ERROR, "RF SV Limit : Support for CDMA band %d and HDR band %d "
                     "combo needs to be added. Using %d dBm10 for default limit.",
                     rfsv_voice_tech_tx_status.rf_band, rfsv_data_tech_tx_status.rf_band,
                     old_hdr_power_limit.sar_limit);
            break;

          } /* cdma bands */
        } /* hdr band : RF_BC0_BAND: */
        break;


        /* hdr band : RF_BC1_BAND */
      case RF_BC1_BAND:
        {
          /* cdma bands */
          switch (rfsv_voice_tech_tx_status.rf_band)
          {

            /* cdma band : RF_BC0_BAND */
          case RF_BC0_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc1_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc1_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC0_BAND */
            break;

            /* cdma band : RF_BC1_BAND */
          case RF_BC1_BAND:
            {
              boolean part1, part2;
              /* 1x chan below HDR chan: */
              part1 = (cdma1x_rf_chan < hdr_rf_chan) &&
                 ((10 * hdr_rf_chan + 246) >= (8000 + 10 * cdma1x_rf_chan)) &&
                 ((10 * hdr_rf_chan - 246) <= (8000 + 10 * cdma1x_rf_chan));

              /* 1x chan above HDR chan: */
              part2 = (cdma1x_rf_chan > hdr_rf_chan) &&
                 ((10 * cdma1x_rf_chan + 246) >= (8000 + 10 * hdr_rf_chan)) &&
                 ((10 * cdma1x_rf_chan - 246) <= (8000 + 10 * hdr_rf_chan));

              /* Determine if half-duplex backoff is to be applied */
              if (part1 || part2)
              {
                /*Reset Bias for half-duplex de-sense computation*/
                old_hdr_power_limit.half_duplex_desense_limit =
                   rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                              rfnv_cdma1x_sv_tbl.hdr_bc1_half_duplex,
                                              bias_input,
                                              bias_output);
              }
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc1_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc1_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC1_BAND */
            break;

            /* cdma band : RF_BC4_BAND */
          case RF_BC4_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc1_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc1_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC4_BAND */
            break;

            /* cdma band : RF_BC6_BAND */
          case RF_BC6_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc1_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc1_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC6_BAND */
            break;

            /* cdma band : RF_BC10_BAND */
          case RF_BC10_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc1_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc1_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC10_BAND */
            break;

            /* cdma band : RF_BC15_BAND */
          case RF_BC15_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc1_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc1_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC15_BAND */
            break;

          default:
            RF_MSG_3(RF_ERROR, "RF SV Limit : Support for CDMA band %d and HDR band %d "
                     "combo needs to be added. Using %d dBm10 for default limit.",
                     rfsv_voice_tech_tx_status.rf_band, rfsv_data_tech_tx_status.rf_band,
                     old_hdr_power_limit.sar_limit);
            break;

          } /* cdma bands */
        } /* hdr band : RF_BC1_BAND: */
        break;


        /* hdr band : RF_BC4_BAND */
      case RF_BC4_BAND:
        {
          /* cdma bands */
          switch (rfsv_voice_tech_tx_status.rf_band)
          {

            /* cdma band : RF_BC0_BAND */
          case RF_BC0_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc4_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc4_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC0_BAND */
            break;

            /* cdma band : RF_BC1_BAND */
          case RF_BC1_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc4_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc4_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC1_BAND */
            break;

            /* cdma band : RF_BC4_BAND */
          case RF_BC4_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc4_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc4_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC4_BAND */
            break;

            /* cdma band : RF_BC6_BAND */
          case RF_BC6_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc4_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc4_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC6_BAND */
            break;

            /* cdma band : RF_BC10_BAND */
          case RF_BC10_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc4_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc4_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC10_BAND */
            break;

            /* cdma band : RF_BC15_BAND */
          case RF_BC15_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc4_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc4_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC15_BAND */
            break;

          default:
            RF_MSG_3(RF_ERROR, "RF SV Limit : Support for CDMA band %d and HDR band %d "
                     "combo needs to be added. Using %d dBm10 for default limit.",
                     rfsv_voice_tech_tx_status.rf_band, rfsv_data_tech_tx_status.rf_band,
                     old_hdr_power_limit.sar_limit);
            break;

          } /* cdma bands */
        } /* hdr band : RF_BC4_BAND: */
        break;


        /* hdr band : RF_BC6_BAND */
      case RF_BC6_BAND:
        {
          /* cdma bands */
          switch (rfsv_voice_tech_tx_status.rf_band)
          {

            /* cdma band : RF_BC0_BAND */
          case RF_BC0_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc6_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc6_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC0_BAND */
            break;

            /* cdma band : RF_BC1_BAND */
          case RF_BC1_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc6_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc6_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC1_BAND */
            break;

            /* cdma band : RF_BC4_BAND */
          case RF_BC4_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc6_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc6_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC4_BAND */
            break;

            /* cdma band : RF_BC6_BAND */
          case RF_BC6_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc6_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc6_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC6_BAND */
            break;

            /* cdma band : RF_BC10_BAND */
          case RF_BC10_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc6_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc6_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC10_BAND */
            break;

            /* cdma band : RF_BC15_BAND */
          case RF_BC15_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc6_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc6_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC15_BAND */
            break;

          default:
            RF_MSG_3(RF_ERROR, "RF SV Limit : Support for CDMA band %d and HDR band %d "
                     "combo needs to be added. Using %d dBm10 for default limit.",
                     rfsv_voice_tech_tx_status.rf_band, rfsv_data_tech_tx_status.rf_band,
                     old_hdr_power_limit.sar_limit);
            break;

          } /* cdma bands */
        } /* hdr band : RF_BC6_BAND: */
        break;


        /* hdr band : RF_BC10_BAND */
      case RF_BC10_BAND:
        {
          /* cdma bands */
          switch (rfsv_voice_tech_tx_status.rf_band)
          {

            /* cdma band : RF_BC0_BAND */
          case RF_BC0_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc10_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc10_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC0_BAND */
            break;

            /* cdma band : RF_BC1_BAND */
          case RF_BC1_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc10_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc10_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC1_BAND */
            break;

            /* cdma band : RF_BC4_BAND */
          case RF_BC4_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc10_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc10_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC4_BAND */
            break;

            /* cdma band : RF_BC6_BAND */
          case RF_BC6_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc10_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc10_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC6_BAND */
            break;

            /* cdma band : RF_BC10_BAND */
          case RF_BC10_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc10_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc10_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC10_BAND */
            break;

            /* cdma band : RF_BC15_BAND */
          case RF_BC15_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc10_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc10_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC15_BAND */
            break;

          default:
            RF_MSG_3(RF_ERROR, "RF SV Limit : Support for CDMA band %d and HDR band %d "
                     "combo needs to be added. Using %d dBm10 for default limit.",
                     rfsv_voice_tech_tx_status.rf_band, rfsv_data_tech_tx_status.rf_band,
                     old_hdr_power_limit.sar_limit);
            break;

          } /* cdma bands */
        } /* hdr band : RF_BC10_BAND: */
        break;


        /* hdr band : RF_BC15_BAND */
      case RF_BC15_BAND:
        {
          /* cdma bands */
          switch (rfsv_voice_tech_tx_status.rf_band)
          {

            /* cdma band : RF_BC0_BAND */
          case RF_BC0_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc15_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc15_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC0_BAND */
            break;

            /* cdma band : RF_BC1_BAND */
          case RF_BC1_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc15_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc15_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC1_BAND */
            break;

            /* cdma band : RF_BC4_BAND */
          case RF_BC4_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc15_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc15_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC4_BAND */
            break;

            /* cdma band : RF_BC6_BAND */
          case RF_BC6_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc15_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc15_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC6_BAND */
            break;

            /* cdma band : RF_BC10_BAND */
          case RF_BC10_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc15_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc15_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC10_BAND */
            break;

            /* cdma band : RF_BC15_BAND */
          case RF_BC15_BAND:
            {
              rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc15_dsi_sar,
                                    &bias_input, &bias_output);
              old_hdr_power_limit.sar_limit =
                 rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                            rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc15_sar,
                                            bias_input, bias_output);
            } /* cdma band : RF_BC15_BAND */
            break;

          default:
            RF_MSG_3(RF_ERROR, "RF SV Limit : Support for CDMA band %d and HDR band %d "
                     "combo needs to be added. Using %d dBm10 for default limit.",
                     rfsv_voice_tech_tx_status.rf_band, rfsv_data_tech_tx_status.rf_band,
                     old_hdr_power_limit.sar_limit);
            break;

          } /* cdma bands */
        } /* hdr band : RF_BC15_BAND: */
        break;


      default:
        RF_MSG_2(RF_ERROR, "RF SV Limit : Support for HDR band %d needs to be added. "
                 "Using %d dBm10 for default limit.",
                 rfsv_data_tech_tx_status.rf_band, old_hdr_power_limit.sar_limit);
        break;

      } /* HDR bands */

    } /* data tech : HDR */
    break;
#endif /* FEATURE_HDR */
#ifdef FEATURE_LTE
    /* If the data tech mode is LTE */
  case RFM_LTE_MODE:
    {
      boolean gps_status = FALSE;
      uint32 cur_lte_tx_freq;
      uint32 cur_lte_tx_num_rb;
      uint16 cur_lte_bw;
      uint32 cur_lte_tx_chan;
      uint32 cur_lte_rx_freq;
      int16 current_limit = 0;
      int16 gps_desense_im3_limit = LTE_P_MAX;
      int16 lte_desense_im3_limit = LTE_P_MAX;
      int16 cdma1x_desense_im3_limit = LTE_P_MAX;
      lte_rrc_im3_backoff_applied_req_s sar_back_off_applied;
      errno_enum_type msg_send_status;
      old_lte_power_limit.im3_limit = LTE_P_MAX;
      old_lte_power_limit.sar_limit = LTE_P_MAX;
      old_lte_power_limit.im2_limit = LTE_P_MAX;



/* Get the GPS status */

      gps_status = rfgnss_mc_get_state();

      /* LTE Bands */

      switch (rflte_mc_state.rf_path_state[0].curr_band_tx)
      {
      case RFCOM_BAND_LTE_B3:
        switch ((rf_card_band_type)rfsv_voice_tech_tx_status.rf_band)
        {
        case RF_BC1_BAND:
        case RF_BC14_BAND:
          {
            cur_lte_tx_freq = rflte_core_get_tx_freq_from_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_tx, RFCOM_BAND_LTE_B3);
            cdma1x_tx_freq = rf_cdma_get_tx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band), cdma1x_rf_chan);

            old_lte_power_limit.im3_limit = LTE_P_MAX;

            cur_lte_bw = rflte_core_get_bw_in_khz(rflte_mc_state.rf_path_state[0].bw_tx);


            /* Decide the back off has to be applied based on GPS on and IM3 Frequency Range*/
            if (gps_status && rfsv_check_lte_1x_im3_ingps_band(cur_lte_tx_freq, cdma1x_tx_freq, cur_lte_bw, 1250)) //(rfgnss_core_gnss_onoff_status()) &&
            {
              old_lte_power_limit.im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                         rfnv_cdma1x_sv_tbl.sv_aws_pcs_gnss_im3_desense,
                                                                         bias_input,
                                                                         bias_output);
            }
            else
            {
              RF_MSG_1(RF_MED, "GPSdesense:No Tx backoff for GPS desense:GPS status : %d", 0);
            }
          }
          break;
        default:
          RF_MSG_1(RF_ERROR,
                   "RF SV Limit : Invalid Band for remote Max power control, remote band = %d",
                   rfsv_voice_tech_tx_status.rf_band);
          old_lte_power_limit.im3_limit = LTE_P_MAX;
        }
        break;

      case RFCOM_BAND_LTE_B9:
        switch ((rf_card_band_type)rfsv_voice_tech_tx_status.rf_band)
        {
        case RF_BC1_BAND:
        case RF_BC14_BAND:
          {
            cur_lte_tx_freq = rflte_core_get_tx_freq_from_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_tx, RFCOM_BAND_LTE_B9);
            cdma1x_tx_freq = rf_cdma_get_tx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band), cdma1x_rf_chan);

            old_lte_power_limit.im3_limit = LTE_P_MAX;

            cur_lte_bw = rflte_core_get_bw_in_khz(rflte_mc_state.rf_path_state[0].bw_tx);

            /* Decide the back off has to be applied based on GPS on and IM3 Frequency Range*/
            if (gps_status && rfsv_check_lte_1x_im3_ingps_band(cur_lte_tx_freq, cdma1x_tx_freq, cur_lte_bw, 1250)) //(rfgnss_core_gnss_onoff_status()) &&
            {
              old_lte_power_limit.im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                         rfnv_cdma1x_sv_tbl.sv_aws_pcs_gnss_im3_desense,
                                                                         bias_input,
                                                                         bias_output);
            }
            else
            {
              RF_MSG_1(RF_MED, "GPSdesense:No Tx backoff for GPS desense:GPS status : %d", 0);
            }
          }
          break;
        default:
          RF_MSG_1(RF_ERROR,
                   "RF SV Limit : Invalid Band for remote Max power control, remote band = %d",
                   rfsv_voice_tech_tx_status.rf_band);
          old_lte_power_limit.im3_limit = LTE_P_MAX;
        }
        break;

      case RFCOM_BAND_LTE_B10:
        switch ((rf_card_band_type)rfsv_voice_tech_tx_status.rf_band)
        {
        case RF_BC1_BAND:
        case RF_BC14_BAND:
          {
            cur_lte_tx_freq = rflte_core_get_tx_freq_from_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_tx, RFCOM_BAND_LTE_B10);
            cdma1x_tx_freq = rf_cdma_get_tx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band), cdma1x_rf_chan);

            old_lte_power_limit.im3_limit = LTE_P_MAX;

            cur_lte_bw = rflte_core_get_bw_in_khz(rflte_mc_state.rf_path_state[0].bw_tx);

            /* Decide the back off has to be applied based on GPS on and IM3 Frequency Range*/
            if (gps_status && rfsv_check_lte_1x_im3_ingps_band(cur_lte_tx_freq, cdma1x_tx_freq, cur_lte_bw, 1250)) //(rfgnss_core_gnss_onoff_status()) &&
            {
              old_lte_power_limit.im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                         rfnv_cdma1x_sv_tbl.sv_aws_pcs_gnss_im3_desense,
                                                                         bias_input,
                                                                         bias_output);
            }
            else
            {
              RF_MSG_1(RF_MED, "GPSdesense:No Tx backoff for GPS desense:GPS status : %d", 0);
            }
          }
          break;
        default:
          RF_MSG_1(RF_ERROR,
                   "RF SV Limit : Invalid Band for remote Max power control, remote band = %d",
                   rfsv_voice_tech_tx_status.rf_band);
          old_lte_power_limit.im3_limit = LTE_P_MAX;
        }
        break;

      case RFCOM_BAND_LTE_B13:
        switch ((rf_card_band_type)rfsv_voice_tech_tx_status.rf_band)
        {
        case RF_BC0_BAND:
          {
            /* Determine if im3 backoff is to be applied */
            if (((cdma1x_rf_chan >= 1) && (cdma1x_rf_chan <= 279)) ||
                ((cdma1x_rf_chan >= 991) && (cdma1x_rf_chan <= 1023))
                )
            {
              old_lte_power_limit.im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                         rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_im3,
                                                                         bias_input,
                                                                         bias_output);
            }
            else
            {
              /*Set IM3 limit to a very high value*/
              old_lte_power_limit.im3_limit = LTE_P_MAX;
            }

            rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_dsi_sar, &bias_input, &bias_output);
            old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                       rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_sar,
                                                                       bias_input,
                                                                       bias_output);
          }
          break;

        case RF_BC1_BAND:
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc13_dsi_sar, &bias_input, &bias_output);
          old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                     rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc13_sar,
                                                                     bias_input,
                                                                     bias_output);
          break;

        case RF_BC6_BAND:
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc13_dsi_sar, &bias_input, &bias_output);
          old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                     rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc13_sar,
                                                                     bias_input,
                                                                     bias_output);
          break;

        default:
          RF_MSG_1(RF_ERROR,
                   "RF SV Limit : Invalid Band for remote Max power control, remote band = %d",
                   rfsv_voice_tech_tx_status.rf_band);
          /*Provide a fail-safe TX power limit in event of error*/
          old_lte_power_limit.sar_limit = DEFAULT_FAIL_SAFE_TX_POWER_LIM;
        }
        break;

      case RFCOM_BAND_LTE_B2:
        switch ((rf_card_band_type)rfsv_voice_tech_tx_status.rf_band)
        {
        case RF_BC0_BAND:
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc2_dsi_sar, &bias_input, &bias_output);
          old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                     rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc2_sar,
                                                                     bias_input,
                                                                     bias_output);
          break;

        case RF_BC1_BAND:
          {
            boolean part1a, part1b, part1, part2a, part2b, part2;
            /*Reset Bias for IM3 computation*/
            cur_lte_tx_freq = rflte_core_get_tx_freq_from_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_tx, RFCOM_BAND_LTE_B2);
            cdma1x_tx_freq = rf_cdma_get_tx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band), cdma1x_rf_chan);

            /* convert to uint32 to prevent overflow and simplify comparisons */
            cur_lte_tx_num_rb = (18 * (uint32)rflte_core_get_num_rb_from_bandwidth(rflte_mc_state.rf_path_state[0].bw_tx)) + 123;
            cur_lte_tx_chan = 10 * ((uint32)rflte_mc_state.rf_path_state[0].curr_chan_tx - 18600);

            /* LTE uplink frequency is above 1x Tx frequency */
            part1a = ((int32)(2 * cur_lte_tx_chan + cur_lte_tx_num_rb - 8000) >= (int32)(10 * cdma1x_rf_chan));
            part1b = ((int32)(2 * cur_lte_tx_chan - cur_lte_tx_num_rb - 8000) <= (int32)(10 * cdma1x_rf_chan));
            part1 = ((cur_lte_tx_freq > cdma1x_tx_freq) && part1a && part1b);

            /* LTE uplink frequency is below 1x Tx frequency */
            part2a = ((int32)(5 * 10 * cdma1x_rf_chan + 5 * cur_lte_tx_num_rb - 40000) >= (int32)(10 * cur_lte_tx_chan));
            part2b = ((int32)(5 * 10 * cdma1x_rf_chan - 5 * cur_lte_tx_num_rb - 40000) <= (int32)(10 * cur_lte_tx_chan));
            part2 = ((cur_lte_tx_freq < cdma1x_tx_freq) && part2a && part2b);

            /* Determine if IM3 backoff is to be applied */
            if (part1 || part2)
            {
              old_lte_power_limit.im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                         rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b2_im3,
                                                                         bias_input,
                                                                         bias_output);
            }
            else
            {
              /*Set IM3 limit to a very high value*/
              old_lte_power_limit.im3_limit = LTE_P_MAX;
            }
            rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc2_dsi_sar, &bias_input, &bias_output);
            old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                       rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc2_sar,
                                                                       bias_input,
                                                                       bias_output);
          }
          break;

        case RF_BC8_BAND:
          {
            RF_MSG_2(RF_MED, "GPSdesense:Back-off Initiated For LTE Band %d 1x Band %d ", rflte_mc_state.rf_path_state[0].curr_band_tx, rfsv_voice_tech_tx_status.rf_band);

            cur_lte_tx_freq = rflte_core_get_tx_freq_from_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_tx, RFCOM_BAND_LTE_B2);
            cdma1x_tx_freq = rf_cdma_get_tx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band), cdma1x_rf_chan);

            old_lte_power_limit.im3_limit = LTE_P_MAX;

            cur_lte_bw = rflte_core_get_bw_in_khz(rflte_mc_state.rf_path_state[0].bw_tx);

            /* Decide the back off has to be applied based on GPS on and IM3 Frequency Range*/
            if (gps_status && rfsv_check_lte_1x_im3_ingps_band(cdma1x_tx_freq, cur_lte_tx_freq, 1250, cur_lte_bw)) //(rfgnss_core_gnss_onoff_status()) &&
            {
              old_lte_power_limit.im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                         rfnv_cdma1x_sv_tbl.sv_aws_pcs_gnss_im3_desense,
                                                                         bias_input,
                                                                         bias_output);
            }
            else
            {
              RF_MSG_1(RF_MED, "GPSdesense:No Tx backoff for GPS desense:GPS status : %d", 0);
            }
          }
          break;
        case RF_BC10_BAND:
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc10_lte_bc2_dsi_sar, &bias_input, &bias_output);
          old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                     rfnv_cdma1x_sv_tbl.cdma1x_bc10_lte_bc2_sar,
                                                                     bias_input,
                                                                     bias_output);
          break;

        case RF_BC15_BAND:
          {
            cur_lte_tx_freq = rflte_core_get_tx_freq_from_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_tx, RFCOM_BAND_LTE_B2);
            cur_lte_rx_freq = rflte_core_get_rx_freq_from_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_rx, RFCOM_BAND_LTE_B2);
            cdma1x_tx_freq = rf_cdma_get_tx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band), cdma1x_rf_chan);

            old_lte_power_limit.im3_limit = LTE_P_MAX;

            cur_lte_bw = rflte_core_get_bw_in_khz(rflte_mc_state.rf_path_state[0].bw_tx);

            /* Decide the back off has to be applied based on GPS on and IM3 Frequency Range*/
            if (gps_status && rfsv_check_lte_1x_im3_ingps_band(cdma1x_tx_freq, cur_lte_tx_freq, 1250, cur_lte_bw)) //(rfgnss_core_gnss_onoff_status()) &&
            {
              RF_MSG_2(RF_MED, "GPSdesense:Back-off Initiated For LTE Band %d 1x Band %d ", rflte_mc_state.rf_path_state[RF_PATH_0].curr_band_tx, rfsv_voice_tech_tx_status.rf_band);
              gps_desense_im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                 rfnv_cdma1x_sv_tbl.sv_aws_pcs_gnss_im3_desense,
                                                                 bias_input,
                                                                 bias_output);
            }
            else
            {
              RF_MSG_1(RF_MED, "GPSdesense:No Tx backoff for GPS desense:GPS status : %d", 0);
            }
            /* Decide the back off has to be applied based on LTE bandwidth, IM3 frequency range and LTE rx frequency range*/
            if ((cur_lte_bw >= 15000) && rfsv_check_lte_1x_im3_in_lte_rx_band(cdma1x_tx_freq, cur_lte_tx_freq, cur_lte_rx_freq, 1250, cur_lte_bw))
            {
              RF_MSG_2(RF_MED, "LTE_rx_desense:Back-off Initiated For LTE Band %d 1x Band %d ", rflte_mc_state.rf_path_state[RF_PATH_0].curr_band_tx, rfsv_voice_tech_tx_status.rf_band);
              lte_desense_im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                 rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_b2_b25_lte_im3_desense,
                                                                 bias_input,
                                                                 bias_output);
            }
            else
            {
              RF_MSG_1(RF_MED, "LTE_rx_desense:No Tx backoff has been applied for cdma1x_bc15_lte_b2_b25_lte_im3_desense", 0);
            }

            old_lte_power_limit.im3_limit = MIN(gps_desense_im3_limit, lte_desense_im3_limit);

            rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc2_dsi_sar, &bias_input, &bias_output);
            old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                       rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc2_sar,
                                                                       bias_input,
                                                                       bias_output);
          }
          break;

        default:
          RF_MSG_1(RF_ERROR,
                   "RF SV Limit : Invalid Band for remote Max power control, remote band = %d",
                   rfsv_voice_tech_tx_status.rf_band);
          /*Provide a fail-safe TX power limit in event of error*/
          old_lte_power_limit.sar_limit = DEFAULT_FAIL_SAFE_TX_POWER_LIM;
          old_lte_power_limit.im3_limit = LTE_P_MAX;
        }
        break;

      case RFCOM_BAND_LTE_B4:
        switch ((rf_card_band_type)rfsv_voice_tech_tx_status.rf_band)
        {
        case RF_BC0_BAND:
          {

            old_lte_power_limit.im2_limit = LTE_P_MAX;

            RF_MSG_2(RF_MED, "IM2backoff", rflte_mc_state.rf_path_state[0].curr_band_tx, rfsv_voice_tech_tx_status.rf_band);

            cur_lte_tx_freq = rflte_core_get_tx_freq_from_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_tx, RFCOM_BAND_LTE_B4);
            cdma1x_tx_freq = rf_cdma_get_tx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band), cdma1x_rf_chan);
            cdma1x_rx_freq = rf_cdma_get_rx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band), cdma1x_rf_chan);


            /* Currently we assumed that we are using the full bw  */
            cur_lte_bw = rflte_core_get_bw_in_khz(rflte_mc_state.rf_path_state[0].bw_tx);

            if (rfsv_check_lte_1x_im2_in_1x_rx_band(cur_lte_tx_freq, cur_lte_bw, cdma1x_tx_freq, cdma1x_rx_freq)) //(rfgnss_core_gnss_onoff_status())&&
            {
              old_lte_power_limit.im2_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                         rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_b4_im2,
                                                                         bias_input,
                                                                         bias_output);

            }
            else
            {
              RF_MSG_1(RF_MED, "IM2backoff:No Backoff has been applied %d", 0);
            }

            old_lte_power_limit.im3_limit = LTE_P_MAX;
            rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc4_dsi_sar, &bias_input, &bias_output);
            old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                       rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc4_sar,
                                                                       bias_input,
                                                                       bias_output);
          }


          break;

        case RF_BC1_BAND:
          {
            RF_MSG_2(RF_MED, "GPSdesense:Back-off Initiated For LTE Band %d 1x Band %d ", rflte_mc_state.rf_path_state[0].curr_band_tx, rfsv_voice_tech_tx_status.rf_band);

            cur_lte_tx_freq = rflte_core_get_tx_freq_from_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_tx, RFCOM_BAND_LTE_B4);
            cdma1x_tx_freq = rf_cdma_get_tx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band), cdma1x_rf_chan);

            old_lte_power_limit.im3_limit = LTE_P_MAX;

            /* Currently we assumed that we are using the full bw  */
            cur_lte_bw = rflte_core_get_bw_in_khz(rflte_mc_state.rf_path_state[0].bw_tx);

            /* Decide the back off has to be applied based on GPS on and IM3 Frequency Range*/
            if (gps_status && rfsv_check_lte_1x_im3_ingps_band(cur_lte_tx_freq, cdma1x_tx_freq, cur_lte_bw, 1250)) //(rfgnss_core_gnss_onoff_status())&&
            {
              old_lte_power_limit.im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                         rfnv_cdma1x_sv_tbl.sv_aws_pcs_gnss_im3_desense,
                                                                         bias_input,
                                                                         bias_output);

            }
            else
            {
              RF_MSG_1(RF_MED, "GPSdesense:No Tx backoff for GPS desense:GPS status : %d", 0);
            }

            rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc4_dsi_sar, &bias_input, &bias_output);
            old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                       rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc4_sar,
                                                                       bias_input,
                                                                       bias_output);

          }
          break;

        case RF_BC14_BAND:
          {
            RF_MSG_2(RF_MED, "GPSdesense:Back-off Initiated For LTE Band %d 1x Band %d ", rflte_mc_state.rf_path_state[0].curr_band_tx, rfsv_voice_tech_tx_status.rf_band);

            cur_lte_tx_freq = rflte_core_get_tx_freq_from_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_tx, RFCOM_BAND_LTE_B4);
            cdma1x_tx_freq = rf_cdma_get_tx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band), cdma1x_rf_chan);

            old_lte_power_limit.im3_limit = LTE_P_MAX;

            /* Currently we assumed that we are using the full bw  */
            cur_lte_bw = rflte_core_get_bw_in_khz(rflte_mc_state.rf_path_state[0].bw_tx);

            /* Decide the back off has to be applied based on GPS on and IM3 Frequency Range*/
            if (gps_status && rfsv_check_lte_1x_im3_ingps_band(cur_lte_tx_freq, cdma1x_tx_freq, cur_lte_bw, 1250)) //(rfgnss_core_gnss_onoff_status())&&
            {
              old_lte_power_limit.im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                         rfnv_cdma1x_sv_tbl.sv_aws_pcs_gnss_im3_desense,
                                                                         bias_input,
                                                                         bias_output);

            }
            else
            {
              RF_MSG_1(RF_MED, "GPSdesense:No Tx backoff for GPS desense:GPS status : %d", 0);
            }
          }
          break;

        case RF_BC6_BAND:
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc4_dsi_sar, &bias_input, &bias_output);
          old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                     rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc4_sar,
                                                                     bias_input,
                                                                     bias_output);
          break;

        case RF_BC15_BAND:
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc4_dsi_sar, &bias_input, &bias_output);
          old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                     rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc4_sar,
                                                                     bias_input,
                                                                     bias_output);
          break;

        default:
          RF_MSG_1(RF_ERROR,
                   "RF SV Limit : Invalid Band for remote Max power control, remote band = %d",
                   rfsv_voice_tech_tx_status.rf_band);
          /*Provide a fail-safe TX power limit in event of error*/
          old_lte_power_limit.sar_limit = DEFAULT_FAIL_SAFE_TX_POWER_LIM;
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          old_lte_power_limit.im2_limit = LTE_P_MAX;
        }
        break;

      case RFCOM_BAND_LTE_B5:
        switch ((rf_card_band_type)rfsv_voice_tech_tx_status.rf_band)
        {
        case RF_BC4_BAND:
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc4_lte_bc5_dsi_sar, &bias_input, &bias_output);
          old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                     rfnv_cdma1x_sv_tbl.cdma1x_bc4_lte_bc5_sar,
                                                                     bias_input,
                                                                     bias_output);
          break;

        default:
          RF_MSG_1(RF_ERROR, "RF SV Limit : Invalid Band for remote Max power control, remote band = %d",
                   rfsv_voice_tech_tx_status.rf_band);
          /*Provide a fail-safe TX power limit in event of error*/
          old_lte_power_limit.sar_limit = DEFAULT_FAIL_SAFE_TX_POWER_LIM;
        }
        break;

      case RFCOM_BAND_LTE_B12:
        switch ((rf_card_band_type)rfsv_voice_tech_tx_status.rf_band)
        {
        case RF_BC0_BAND:
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc12_dsi_sar, &bias_input, &bias_output);
          old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                     rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc12_sar,
                                                                     bias_input,
                                                                     bias_output);
          break;

        case RF_BC1_BAND:
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc12_dsi_sar, &bias_input, &bias_output);
          old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                     rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc12_sar,
                                                                     bias_input,
                                                                     bias_output);
          break;

        case RF_BC15_BAND:
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc12_dsi_sar, &bias_input, &bias_output);
          old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                     rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc12_sar,
                                                                     bias_input,
                                                                     bias_output);
          break;

        default:
          RF_MSG_1(RF_ERROR, "RF SV Limit : Invalid Band for remote Max power control, remote band = %d",
                   rfsv_voice_tech_tx_status.rf_band);
          /*Provide a fail-safe TX power limit in event of error*/
          old_lte_power_limit.sar_limit = DEFAULT_FAIL_SAFE_TX_POWER_LIM;
        }
        break;

      case RFCOM_BAND_LTE_B17:
        switch ((rf_card_band_type)rfsv_voice_tech_tx_status.rf_band)
        {
        case RF_BC0_BAND:
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc17_dsi_sar, &bias_input, &bias_output);
          old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                     rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc17_sar,
                                                                     bias_input,
                                                                     bias_output);
          break;

        case RF_BC1_BAND:
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc17_dsi_sar, &bias_input, &bias_output);
          old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                     rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc17_sar,
                                                                     bias_input,
                                                                     bias_output);
          break;

        case RF_BC6_BAND:
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc17_dsi_sar, &bias_input, &bias_output);
          old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                     rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc17_sar,
                                                                     bias_input,
                                                                     bias_output);
          break;

        default:
          RF_MSG_1(RF_ERROR, "RF SV Limit : Invalid Band for remote Max power control, remote band = %d",
                   rfsv_voice_tech_tx_status.rf_band);
          /*Provide a fail-safe TX power limit in event of error*/
          old_lte_power_limit.sar_limit = DEFAULT_FAIL_SAFE_TX_POWER_LIM;
        }
        break;

      case RFCOM_BAND_LTE_B1:
        switch ((rf_card_band_type)rfsv_voice_tech_tx_status.rf_band)
        {
        case RF_BC8_BAND :
        case RF_BC15_BAND:
          {
            RF_MSG_2(RF_MED, "GPSdesense:Back-off Initiated For LTE Band %d 1x Band %d ", rflte_mc_state.rf_path_state[0].curr_band_tx, rfsv_voice_tech_tx_status.rf_band);

            cur_lte_tx_freq = rflte_core_get_tx_freq_from_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_tx, RFCOM_BAND_LTE_B1);
            cdma1x_tx_freq = rf_cdma_get_tx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band), cdma1x_rf_chan);

            old_lte_power_limit.im3_limit = LTE_P_MAX;

            cur_lte_bw = rflte_core_get_bw_in_khz(rflte_mc_state.rf_path_state[0].bw_tx);

            /* Decide the back off has to be applied based on GPS on and IM3 Frequency Range*/
            if (gps_status && rfsv_check_lte_1x_im3_ingps_band(cdma1x_tx_freq, cur_lte_tx_freq, 1250, cur_lte_bw)) //(rfgnss_core_gnss_onoff_status()) &&
            {
              old_lte_power_limit.im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                         rfnv_cdma1x_sv_tbl.sv_aws_pcs_gnss_im3_desense,
                                                                         bias_input,
                                                                         bias_output);
            }
            else
            {
              RF_MSG_1(RF_MED, "GPSdesense:No Tx backoff for GPS desense:GPS status : %d", 0);
            }
          }
          break;
        default:
          RF_MSG_1(RF_ERROR, "RF SV Limit : Invalid Band for remote Max power control, remote band = %d",
                   rfsv_voice_tech_tx_status.rf_band);
          old_lte_power_limit.im3_limit = LTE_P_MAX;
        }
        break;

      case RFCOM_BAND_LTE_B25:
        switch ((rf_card_band_type)rfsv_voice_tech_tx_status.rf_band)
        {
        case RF_BC8_BAND:
          {
            RF_MSG_2(RF_MED, "GPSdesense:Back-off Initiated For LTE Band %d 1x Band %d ", rflte_mc_state.rf_path_state[0].curr_band_tx, rfsv_voice_tech_tx_status.rf_band);

            cur_lte_tx_freq = rflte_core_get_tx_freq_from_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_tx, RFCOM_BAND_LTE_B25);
            cdma1x_tx_freq = rf_cdma_get_tx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band), cdma1x_rf_chan);

            old_lte_power_limit.im3_limit = LTE_P_MAX;

            cur_lte_bw = rflte_core_get_bw_in_khz(rflte_mc_state.rf_path_state[0].bw_tx);

            /* Decide the back off has to be applied based on GPS on and IM3 Frequency Range*/
            if (gps_status &&  rfsv_check_lte_1x_im3_ingps_band(cdma1x_tx_freq, cur_lte_tx_freq, 1250, cur_lte_bw)) //(rfgnss_core_gnss_onoff_status()) &&
            {
              old_lte_power_limit.im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                         rfnv_cdma1x_sv_tbl.sv_aws_pcs_gnss_im3_desense,
                                                                         bias_input,
                                                                         bias_output);
            }
            else
            {
              RF_MSG_1(RF_MED, "GPSdesense:No Tx backoff for GPS desense:GPS status : %d", 0);
            }
          }
          break;
        case RF_BC15_BAND:
          {
            RF_MSG_2(RF_MED, "GPSdesense:Back-off Initiated For LTE Band %d 1x Band %d ", rflte_mc_state.rf_path_state[RF_PATH_0].curr_band_tx, rfsv_voice_tech_tx_status.rf_band);

            cur_lte_tx_freq = rflte_core_get_tx_freq_from_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_tx, RFCOM_BAND_LTE_B25);
            cur_lte_rx_freq = rflte_core_get_rx_freq_from_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_rx, RFCOM_BAND_LTE_B25);
            cdma1x_tx_freq = rf_cdma_get_tx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band), cdma1x_rf_chan);
            cdma1x_rx_freq = rf_cdma_get_rx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band), cdma1x_rf_chan);

            old_lte_power_limit.im3_limit = LTE_P_MAX;

            cur_lte_bw = rflte_core_get_bw_in_khz(rflte_mc_state.rf_path_state[RF_PATH_0].bw_tx);

            /* Decide the back off has to be applied based on GPS on and IM3 Frequency Range*/
            if (gps_status &&  rfsv_check_lte_1x_im3_ingps_band(cdma1x_tx_freq, cur_lte_tx_freq, 1250, cur_lte_bw)) //(rfgnss_core_gnss_onoff_status()) &&
            {
              gps_desense_im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                 rfnv_cdma1x_sv_tbl.sv_aws_pcs_gnss_im3_desense,
                                                                 bias_input,
                                                                 bias_output);
            }
            else
            {
              RF_MSG_1(RF_MED, "GPSdesense:No Tx backoff for GPS desense:GPS status : %d", 0);
            }
            /* Decide the back off has to be applied based on LTE bandwidth, IM3 frequency range and LTE rx frequency range*/
            if ((cur_lte_bw >= 15000) && rfsv_check_lte_1x_im3_in_lte_rx_band(cdma1x_tx_freq, cur_lte_tx_freq, cur_lte_rx_freq, 1250, cur_lte_bw))
            {
              RF_MSG_2(RF_MED, "LTE_rx_desense:Back-off Initiated For LTE Band %d 1x Band %d ", rflte_mc_state.rf_path_state[RF_PATH_0].curr_band_tx, rfsv_voice_tech_tx_status.rf_band);
              lte_desense_im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                 rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_b2_b25_lte_im3_desense,
                                                                 bias_input,
                                                                 bias_output);
            }
            else
            {
              RF_MSG_1(RF_MED, "LTE_rx_desense:No Tx backoff has been applied for cdma1x_bc15_lte_b25_1x_im3_desense", 0);
            }

            /* Decide the back off has to be applied based on LTE bandwidth, IM3 frequency range and CDMA 1x rx frequency range*/
            if ((cur_lte_bw >= 15000) && rfsv_check_lte_1x_im3_in_1x_rx_band(cdma1x_tx_freq, cdma1x_rx_freq, cur_lte_tx_freq, 1250, cur_lte_bw))
            {
              RF_MSG_2(RF_MED, "CDMA1x_rx_desense:Back-off Initiated For LTE Band %d 1x Band %d ", rflte_mc_state.rf_path_state[RF_PATH_0].curr_band_tx, rfsv_voice_tech_tx_status.rf_band);
              cdma1x_desense_im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                    rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_b25_1x_im3_desense,
                                                                    bias_input,
                                                                    bias_output);
            }
            else
            {
              RF_MSG_1(RF_MED, "CDMA1x_rx_desense:No Tx backoff has been applied for cdma1x_bc15_lte_b2_b25_lte_im3_desense", 0);
            }

            old_lte_power_limit.im3_limit = MIN(gps_desense_im3_limit, MIN(lte_desense_im3_limit, cdma1x_desense_im3_limit));
          }
          break;

        case RF_BC0_BAND:
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_b25_dsi_sar, &bias_input, &bias_output);
          old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                     rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_b25_sar,
                                                                     bias_input,
                                                                     bias_output);
          break;
        case RF_BC1_BAND:
          {
            boolean part1a, part1b, part1, part2a, part2b, part2;
            /*Reset Bias for IM3 computation*/
            cur_lte_tx_freq = rflte_core_get_tx_freq_from_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_tx, RFCOM_BAND_LTE_B25);
            cdma1x_tx_freq = rf_cdma_get_tx_carrier_freq(rf_cdma_band_to_rfm_card_band(rfsv_voice_tech_tx_status.rf_band), cdma1x_rf_chan);

            /* convert to uint32 to prevent overflow and simplify comparisons */
            cur_lte_tx_num_rb = (18 * (uint32)rflte_core_get_num_rb_from_bandwidth(rflte_mc_state.rf_path_state[0].bw_tx)) + 123;
            cur_lte_tx_chan = 10 * ((uint32)rflte_mc_state.rf_path_state[0].curr_chan_tx - 26040);

            /* LTE uplink frequency is above 1x Tx frequency */
            part1a = ((int32)(2 * cur_lte_tx_chan + cur_lte_tx_num_rb - 8000) >= (int32)(10 * cdma1x_rf_chan));
            part1b = ((int32)(2 * cur_lte_tx_chan - cur_lte_tx_num_rb - 8000) <= (int32)(10 * cdma1x_rf_chan));
            part1 = ((cur_lte_tx_freq > cdma1x_tx_freq) && part1a && part1b);

            /* LTE uplink frequency is below 1x Tx frequency */
            part2a = ((int32)(5 * 10 * cdma1x_rf_chan + 5 * cur_lte_tx_num_rb - 40000) >= (int32)(10 * cur_lte_tx_chan));
            part2b = ((int32)(5 * 10 * cdma1x_rf_chan - 5 * cur_lte_tx_num_rb - 40000) <= (int32)(10 * cur_lte_tx_chan));
            part2 = ((cur_lte_tx_freq < cdma1x_tx_freq) && part2a && part2b);

            /* Determine if IM3 backoff is to be applied */
            if (part1 || part2)
            {
              old_lte_power_limit.im3_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                         rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b25_im3,
                                                                         bias_input,
                                                                         bias_output);
            }
            else
            {
              /*Set IM3 limit to a very high value*/
              old_lte_power_limit.im3_limit = LTE_P_MAX;
            }
            rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b25_dsi_sar, &bias_input, &bias_output);
            old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                       rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b25_sar,
                                                                       bias_input,
                                                                       bias_output);
          }
          break;
        case RF_BC10_BAND:
          old_lte_power_limit.im3_limit = LTE_P_MAX;
          rfsv_get_dsi_sar_bias(rfnv_cdma1x_sv_tbl.cdma1x_bc10_lte_b25_dsi_sar, &bias_input, &bias_output);
          old_lte_power_limit.sar_limit = rfsv_compute_sar_pwr_limit(voice_tech_tx_lvl_dbm10,
                                                                     rfnv_cdma1x_sv_tbl.cdma1x_bc10_lte_b25_sar,
                                                                     bias_input,
                                                                     bias_output);
          break;

        default:
          RF_MSG_1(RF_ERROR, "RF SV Limit : Invalid Band for remote Max power control, remote band = %d",
                   rfsv_voice_tech_tx_status.rf_band);
          /*Provide a fail-safe TX power limit in event of error*/
          old_lte_power_limit.sar_limit = DEFAULT_FAIL_SAFE_TX_POWER_LIM;
        }
        break;


      default:
        RF_MSG_1(RF_ERROR,
                 "RF SV Limit : Invalid Band for remote Max power control, slave LTE band = %d",
                 rflte_mc_state.rf_path_state[RF_PATH_0].curr_band_tx);
        /*Provide a fail-safe TX power limit in event of error*/
        old_lte_power_limit.sar_limit = DEFAULT_FAIL_SAFE_TX_POWER_LIM;
      } /* LTE Bands */


      RF_MSG_1(RF_HIGH, "RF SV Limit : 1X power reported from remote processor is (%d)dbm*10", voice_tech_tx_lvl_dbm10);

      /*Search for the new power limit*/
      current_limit =  rfsv_get_min_lte_tx_power(old_lte_power_limit);

      /*Update the internal flag*/

      if (current_limit < rflte_core_txpl_get_pcmax())
      {
        rfsv_set_lte_sar_pwr_limits_status(FALSE);
      }
      else
      {
        rfsv_set_lte_sar_pwr_limits_status(TRUE);
      }

      /*Notify upper layer about the transition only when there is a change of back off status*/
      if (old_lte_normal_back_off_status != rfsv_get_lte_sar_pwr_limits_status())
      {
        /*Update internal variable*/
        old_lte_normal_back_off_status = rfsv_get_lte_sar_pwr_limits_status();
        sar_back_off_applied.on_off = !old_lte_normal_back_off_status;

        /*initialize RRC LTE msg packet*/
        msgr_init_hdr(&sar_back_off_applied.msg_hdr, MSGR_LTE_RRC, LTE_RRC_IM3_BACKOFF_APPLIED_REQ);

        msg_send_status = msgr_send(&sar_back_off_applied.msg_hdr, sizeof(sar_back_off_applied));

        if (msg_send_status != E_SUCCESS)
        {
          RF_MSG(RF_ERROR, "RF SV Limit : Failed to send LTE SAR back off update to RRC");
        }
        RF_MSG_1(RF_MED,
                 "RF SV Limit : Notify LTE RRC that new SAR back off status is %d",
                 (!old_lte_normal_back_off_status));
      }

      /*Set new limit in FW*/
      //rflte_core_txpl_set_mtpl_for_sar(current_limit);

    } /* data tech: LTE */
#endif /*FEATURE_LTE*/
    break;

  default:
    /* Provide a fail-safe TX power limit in event of error */
    old_hdr_power_limit.sar_limit = DEFAULT_FAIL_SAFE_TX_POWER_LIM;
    RF_MSG_2(RF_ERROR, "RF SV Limit : Support for RF mode %d needs to be added. "
             "Using %d dBm10 for default limit.",
             rfsv_data_tech_tx_status.rf_mode, old_hdr_power_limit.sar_limit);
    break;

  } /* switch( data-tech's mode ) */

} /* rfsv_compute_sar_tx_pwr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface to bring the voice-tech's status variable to init state.

*/
void rfsv_init_voice_tx_status(void)
{
  int index = 0;

  rfsv_voice_tech_tx_status.device = RFM_INVALID_DEVICE;
  rfsv_voice_tech_tx_status.tx_on = FALSE;
  rfsv_voice_tech_tx_status.rf_mode = RFM_PARKED_MODE;
  rfsv_voice_tech_tx_status.rf_band = RF_MAX_BAND;
  rfsv_voice_tech_tx_status.num_band_chan = 0;
  for (index = 0; index < MAX_NUM_CHANNELS_SUPPORTED; index++)
  {
    rfsv_voice_tech_tx_status.band_chan_array[index] = 0;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface to bring the data-tech's status variable to init state.

*/
void rfsv_init_data_tx_status(void)
{
  int index = 0;

  rfsv_data_tech_tx_status.device = RFM_INVALID_DEVICE;
  rfsv_data_tech_tx_status.tx_on = FALSE;
  rfsv_data_tech_tx_status.rf_mode = RFM_PARKED_MODE;
  rfsv_data_tech_tx_status.rf_band = RF_MAX_BAND;
  rfsv_data_tech_tx_status.num_band_chan = 0;
  for (index = 0; index < MAX_NUM_CHANNELS_SUPPORTED; index++)
  {
    rfsv_data_tech_tx_status.band_chan_array[index] = 0;
  }
}

/*===========================================================================

FUNCTION RFSV_20_MS_ISR                                      INTERNAL FUNCTION

DESCRIPTION
   This 20ms interrupt is designed to update the remote 
   processor with current 1x TX power. There is no HDET
   read involved here. TX AGC is obtained from FW and the
   most recent filtered HDET error is applied. Information
   is transmitted via RPC call.  Used in CDMA mode only.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfsv_20_ms_isr(void)
{
  /* If both voice and data are transmitting */
  if (rfsv_voice_tech_tx_status.tx_on && rfsv_data_tech_tx_status.tx_on)
  {
    /* If both voice and HDR are on, SV module contributes max power limit to HDR */
    hdr_normal_power_limit_active = FALSE;

    /* Execute SAR PWR limit algorithm*/
    rfsv_compute_sar_tx_pwr();
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface used by voice-tech to start the SV limiting ISR.

  @param voice_tech_tx_status
  A structure containing voice-tech's Tx information.

*/
void rfsv_start_voice_timer(rfsv_data_tech_tx_status_type voice_tech_tx_status)
{
  int index = 0;

  /* No need to start timer during cal */
  if (rfm_get_calibration_state() == TRUE)
  {
    return;
  }

  rfsv_init_voice_tx_status();

  rfsv_voice_tech_tx_status.tx_on = voice_tech_tx_status.tx_on;
  rfsv_voice_tech_tx_status.device = voice_tech_tx_status.device;
  rfsv_voice_tech_tx_status.rf_mode = voice_tech_tx_status.rf_mode;
  rfsv_voice_tech_tx_status.rf_band = voice_tech_tx_status.rf_band;
  rfsv_voice_tech_tx_status.num_band_chan = voice_tech_tx_status.num_band_chan;
  for (index = 0; index < MAX_NUM_CHANNELS_SUPPORTED; index++)
  {
    rfsv_voice_tech_tx_status.band_chan_array[index] =
       voice_tech_tx_status.band_chan_array[index];
  }

#ifndef T_WINNT
  
  timer_def2(&rf_tx_measurement_report_isr,NULL);

  timer_reg(&rf_tx_measurement_report_isr,
            (timer_t2_cb_type)rfsv_20_ms_isr,
            (timer_cb_data_type)NULL,
            RF_TX_MEASUREMENT_REPORT_REPEAT_TIME,
            RF_TX_MEASUREMENT_REPORT_REPEAT_TIME);
#endif /* T_WINNT */
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface used by voice-tech to stop the SV limiting ISR.

*/
void rfsv_stop_voice_timer(void)
{
  (void)timer_clr(&rf_tx_measurement_report_isr, T_NONE);

  /* Re-store data tech's max power to highest */
  if (rfsv_data_tech_tx_status.tx_on)
  {
    /* switch : modes*/
    switch (rfsv_data_tech_tx_status.rf_mode)
    {
#ifdef FEATURE_HDR
    case RFM_1XEVDO_MODE:
      /* Re-store HDR SV Power Limit to highest level */
      old_hdr_power_limit.sar_limit = HDR_P_MAX;
      old_hdr_power_limit.half_duplex_desense_limit = HDR_P_MAX;
      rf_hdr_mc_tx_update_sv_limit(HDR_P_MAX);
      break;
#endif /* FEATURE_HDR */
#ifdef FEATURE_LTE
      /* mode LTE */
    case RFM_LTE_MODE:
      {
        lte_rrc_im3_backoff_applied_req_s sar_back_off_applied;

        /* Re-store LTE Vars */
        old_lte_power_limit.im3_limit = LTE_P_MAX;
        old_lte_power_limit.sar_limit = LTE_P_MAX;

        /* Restore the original DSI SAR limit for LTE*/
        (void)rflte_mc_tx_update_sar_backoff(RFCOM_TRANSCEIVER_0);
        /*Update tracking variable*/
        rfsv_set_lte_sar_pwr_limits_status(TRUE);

        if (old_lte_normal_back_off_status != rfsv_get_lte_sar_pwr_limits_status())
        {
          errno_enum_type msg_send_status;

          old_lte_normal_back_off_status = rfsv_get_lte_sar_pwr_limits_status();
          /*notify upper layer that SVLTE mode is stopping and no more back off*/
          sar_back_off_applied.on_off = !old_lte_normal_back_off_status;

          /*initialize RRC LTE msg packet*/
          msgr_init_hdr(&sar_back_off_applied.msg_hdr, MSGR_LTE_RRC, LTE_RRC_IM3_BACKOFF_APPLIED_REQ);

          msg_send_status = msgr_send(&sar_back_off_applied.msg_hdr, sizeof(sar_back_off_applied));

          if (msg_send_status != E_SUCCESS)
          {
            RF_MSG(RF_ERROR, "RF SV Limit : Failed to send LTE SAR back off update to RRC");
          }
          RF_MSG(RF_HIGH, "RF SV Limit : Notify LTE RRC that SAR back off is not applied");
        }
      } /* mode LTE */
#endif /*FEATURE_LTE*/
      break;

    default:
      RF_MSG_1(RF_ERROR, "RF SV Limit : Need to add support data tech mode %d",
               rfsv_data_tech_tx_status.rf_mode);

    } /* switch : modes*/

  }

  /* Clear local vars structure */
  rfsv_init_voice_tx_status();

  /* If voice tech is off, HDR max power is not limited by SV module */
  hdr_normal_power_limit_active = TRUE;
}

/*===========================================================================

FUNCTION rfsv_send_tx_update_notification                              

DESCRIPTION
  This API will send TX profile update to the
  1x side. If message is the same as the last one,
  message will be ignored. A copy of the message 
  will also be stored in the buffer. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfsv_send_tx_update_notification(boolean                   tx_on,
                                      rfm_mode_enum_type        rf_mode,
                                      rf_card_band_type         rf_band,
                                      uint32                    num_band_chan,
                                      uint32                    band_chan_array[MAX_NUM_CHANNELS_SUPPORTED],
                                      boolean                   packet_check_override)
{
  uint8 index = 0;

  if (tx_on == TRUE)
  {
    rfsv_data_tech_tx_status.tx_on = tx_on;
    rfsv_data_tech_tx_status.rf_mode = rf_mode;
    rfsv_data_tech_tx_status.rf_band = rf_band;
    rfsv_data_tech_tx_status.num_band_chan = num_band_chan;
    for (index = 0; index < MAX_NUM_CHANNELS_SUPPORTED; index++)
    {
      rfsv_data_tech_tx_status.band_chan_array[0] = band_chan_array[0];
    }
  }
  else
  {
    rfsv_init_data_tx_status();
    hdr_normal_power_limit_active = TRUE;
    old_hdr_power_limit.sar_limit = HDR_P_MAX;
    old_hdr_power_limit.half_duplex_desense_limit = HDR_P_MAX;
  }

} /* rfsv_send_tx_update_notification */



/*===========================================================================

FUNCTION rfsv_check_lte_1x_im3_ingps_band                              

DESCRIPTION
        This API will calculate the IM3 Frequency Range  and check if it falls on 
the GPS or GNSS  band.

DEPENDENCIES 
  None

RETURN VALUE TRUE or FALSE 
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean rfsv_check_lte_1x_im3_ingps_band(uint32 low_freq, uint32 high_freq, uint32 lowband_bw, uint32 highband_bw)
{
  uint32 fc_im3;
  uint32 f1_im3;
  uint32 f2_im3;
  uint32 bw_im3;
  uint32 gps_lo = 1569000;
  uint32 gps_hi = 1585000;
  uint32 glonass_lo = 1595000;
  uint32 glonass_hi = 1611000;
  boolean return_status = FALSE;


  fc_im3 = 2 * low_freq - high_freq;  // Need to check with whether it is a negative sign or positive sign . As far we understood that it should be negative //

  bw_im3 = 2 * lowband_bw + highband_bw;

  f1_im3 = fc_im3 - bw_im3 / 2;
  f2_im3 = fc_im3 + bw_im3 / 2;

  RF_MSG_4(RF_MED, "GPSdesense: low_freq:%d,high_freq:%d,lowbandbw:%d,highbandbw:%d ", low_freq, high_freq, lowband_bw, highband_bw);

  /* Check im3 Frequency range falls in [1569,1585 - GPS range ] or [1595,1611 -GNSS range ].Some of the cases may be redudant which can be removed  */


  if (((gps_lo <= f1_im3) && (f1_im3 <= gps_hi)) ||
      ((gps_lo <= f2_im3) && (f2_im3 <= gps_hi)) ||
      ((glonass_lo <= f1_im3) && (f1_im3 <= glonass_hi)) ||
      ((glonass_lo <= f2_im3) && (f2_im3 <= glonass_hi)) ||
      ((f1_im3 <= gps_lo) && (f2_im3 >= glonass_hi)) ||
      ((f1_im3 <= gps_lo) && (f2_im3 >= gps_hi)) ||
      ((f1_im3 <= glonass_lo) && (f2_im3 >= glonass_hi)))
  {
    return_status = TRUE;
  }

  return return_status;
}

/*===========================================================================
FUNCTION rfsv_check_lte_1x_im3_in_lte_rx_band                              

DESCRIPTION
        This API will calculate the IM3 Frequency Range  and check if it falls on 
the LTE Rx band.

DEPENDENCIES 
  None

RETURN VALUE TRUE or FALSE 
  None

SIDE EFFECTS
  None
  
===========================================================================*/

boolean rfsv_check_lte_1x_im3_in_lte_rx_band(uint32 fc_1xtx,uint32 fc_ltetx,uint32 fc_lterx,uint32 bw_1x,uint32 bw_lte)
{
  uint32 fc_im3;
  uint32 fl_im3;
  uint32 fh_im3;
  uint32 bw_im3;
  uint32 fl_lterx;
  uint32 fh_lterx;
  boolean return_status = FALSE;

//Calcualte IM3 Frequency and BW 

  fc_im3 = 2*fc_ltetx - fc_1xtx;  
  bw_im3 = 2*bw_lte + bw_1x; 
 
// Calculate IM3 Frequency range

  fl_im3 = fc_im3 - bw_im3/2;
  fh_im3 = fc_im3 + bw_im3/2;

// Calculate LTE DL Frequency range

  fl_lterx = fc_lterx - bw_lte/2;
  fh_lterx = fc_lterx + bw_lte/2;

  RF_MSG_4( RF_MED, "LTE_rx_desense: fc_ltetx %d,fc_cdmatx %d,f1_im3 %d,f2_im3 %d ",fc_ltetx,fc_1xtx,fl_im3,fh_im3);


  if (fl_im3 <= fh_lterx && fh_im3 >= fl_lterx)
  {
    return_status = TRUE;
  }
  return return_status;
}

/*===========================================================================

FUNCTION rfsv_check_lte_1x_im3_in_1x_rx_band                              

DESCRIPTION
        This API will calculate the IM3 Frequency Range  and check if it falls on 
the 1x Rx band.

DEPENDENCIES 
  None

RETURN VALUE TRUE or FALSE 
  None

SIDE EFFECTS
  None
  
===========================================================================*/

boolean rfsv_check_lte_1x_im3_in_1x_rx_band(uint32 fc_1xtx,uint32 fc_1xrx,uint32 fc_ltetx,uint32 bw_1x,uint32 bw_lte)
{
  uint32 fc_im3;
  uint32 fl_im3;
  uint32 fh_im3;
  uint32 bw_im3;
  uint32 fl_1xrx;
  uint32 fh_1xrx;
  boolean return_status = FALSE;

//Calcualte IM3 Frequency and BW 

  fc_im3 = 2*fc_ltetx - fc_1xtx;  
  bw_im3 = 2*bw_lte + bw_1x; 
 
// Calculate IM3 Frequency range

  fl_im3 = fc_im3 - bw_im3/2;
  fh_im3 = fc_im3 + bw_im3/2;

// Calculate 1x DL Frequency range

  fl_1xrx = fc_1xrx - bw_1x/2;
  fh_1xrx = fc_1xrx + bw_1x/2;

  RF_MSG_4( RF_MED, "CDMA_1x_rx_desense: fc_ltetx %d,fc_cdmatx %d,f1_im3 %d,f2_im3 %d ",fc_ltetx,fc_1xtx,fl_im3,fh_im3);


  if (fl_im3 <= fh_1xrx && fh_im3 >= fl_1xrx)
  {
    return_status = TRUE;
  }
  return return_status;
}

/*===========================================================================

FUNCTION rfsv_check_lte_1x_im2_in_1x_rx_band                              

DESCRIPTION
        This API will calculate the IM2 Frequency Range  and check if it falls on 
the 1x RX channel

DEPENDENCIES 
  None

RETURN VALUE TRUE or FALSE 
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean rfsv_check_lte_1x_im2_in_1x_rx_band(uint32 fc_ltetx, uint32 lte_bw, uint32 fc_cdmatx, uint32 fc_cdmarx)
{
  uint32 fc_im2;
  uint32 f1_im2;
  uint32 f2_im2;
  uint32 bw_im2;
  boolean return_status = FALSE;
  uint32 fc_cdmarx_lo = fc_cdmarx - 750;
  uint32 fc_cdmarx_ho = fc_cdmarx + 750;


//Calcualte IM2 Frequency and BW

  fc_im2 = fc_ltetx - fc_cdmatx;
  bw_im2 = lte_bw + 1250;


  f1_im2 = fc_im2 - bw_im2 / 2;
  f2_im2 = fc_im2 + bw_im2 / 2;

  RF_MSG_4(RF_MED, "Im2 backoff:fc_ltetx %d,fc_cdmatx %d,f1_im3 %d,f2_im3 %d ", fc_ltetx, fc_cdmatx, f1_im2, f2_im2);


  if (fc_cdmarx_lo <= f2_im2 && f1_im2 <= fc_cdmarx_ho)
  {
    return_status = TRUE;
  }

  return return_status;
}

