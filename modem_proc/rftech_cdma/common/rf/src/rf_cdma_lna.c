/*!
  @file
  rf_cdma_lna.c

  @details
  This file defines interfaces pertaining to LNA configuration.
 
  @addtogroup RF_CDMA_COMMON_LNA
  @{ 
*/

/*==============================================================================

  Copyright (c) 2010 - 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_lna.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/12/13   spa     Query MSM layer for sigma delta max level
12/08/11   bmg     Updated OLTA set point for new DVGA offset with RxLM 1.6.5
08/03/11   sty     Added API to check for rise fall values -- 
                   rf_cdma_check_rise_fall_values()
07/28/11   cd      Provided fixed default rise/fall values when in calibration 
                   state
07/21/11   cd      - Added OLTA switchpoint support
                   - Added APIs to query timer hysteresis information
06/17/11   cd      Provided get APIs to retrieve switchpoint rise/fall data
01/31/11   aro     Deleted unused function
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
12/29/10   aro     Added function to query LNA config
12/23/10   aro     Doxygen update
12/14/10   aro     Removed common LNA override function
12/13/10   aro     Added function to set LNA State
12/09/10   aro     Initial Release.

==============================================================================*/

#include "comdef.h"
#include "rf_cdma_lna.h"
#include "rf_cdma_nv.h"
#include "rfcommon_msg.h"
#include "rfm_internal.h"
#include "rf_cdma_msm.h"

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Fixed Rise Switchpoint Values for default programming
 
  @details
  These default values represent fixed rise switchpoints in dBm10 and
  must be used when required to program FW with a default switchpoint
  configuration. The switchpoints must be in increasing monotonic order.
  Currently, used in calibration state, when NV data might be invalid
*/
int16 rf_cdma_lna_rise_swpts_fixed_default[RFM_LNA_GAIN_STEP_NUM] =
{
  /*
    G0->G1: -88.2 dBm
    G1->G2: -80.6 dBm
    G2->G3: -39.8 dBm
    G3->G4: -12.7 dBm
  */
  -882, -806, -398, -127
};

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Fixed Fall Switchpoint Values for default programming
 
  @details
  These default values represent fixed fall switchpoints in dBm10 and
  must be used when required to program FW with a default switchpoint
  configuration. The switchpoints must be in increasing monotonic order.
  Currently, used in calibration state, when NV data might be invalid
*/
int16 rf_cdma_lna_fall_swpts_fixed_default[RFM_LNA_GAIN_STEP_NUM] =
{
  /*
    G1->G0: -90.6 dBm
    G2->G1: -81.8 dBm
    G3->G2: -43.8 dBm
    G4->G3: -12.7 dBm
  */
  -906, -818, -438, -127
};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to provide the LNA rise switchpoint value for the requested
  Power mode/linearity, and LNA gain step, from the generic switchpoint
  structure provided
 
  In calibration state, this API returns a fixed default value for the rise
  switchpoint in dBm10 as calibration data to calculate OLTA switchpoints
  may not be available
 
  @param lna_info
  The LNA Info data structure based on the current mode of operation, from
  which switchpoint info must be extracted.

  @param power_mode
  The power mode for which rise switchpoints are requested.

  @param gain_step
  The gain step for which rise switchpoints are requested
 
  @param gain_offset_val
  The calibrated LNA Gain Offset value in 1/10th dB for the gain state
  corresponding to the gain step. E.g. G0 corresponds to step G0->G1
 
  @return
  Signed 16-bit rise switchpoint value in dBm10.
*/
int16
rf_cdma_lna_get_rise_swpt
(
  const rf_cdma_lna_info_type* lna_info,
  rfm_cdma_power_mode_type power_mode,
  rfm_lna_gain_step_type gain_step,
  int16 gain_offset_val
)
{
  const rf_cdma_lna_sp_info_type* sp_info_ptr;
  int16 rise_swpt_value;

  /* If in calibration state, return a fixed value to ensure
  monotonicity of switchpoints programmed to FW */
  if ( rfm_get_calibration_state() == TRUE )
  {
    rise_swpt_value = 
      rf_cdma_lna_rise_swpts_fixed_default[gain_step];

    RF_MSG_2( RF_LOW, "rf_cdma_lna_get_rise_swpt: "
                      "In cal state, programming fixed default. Gain state %d, r swpt %d",
              gain_step, rise_swpt_value );

    return rise_swpt_value;
  }

  if ( lna_info == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_lna_get_rise_swpt: "
                      "LNA NV structure isn't allocated." );
    return rise_swpt_value = 0;
  }

  /* Check whether switchpoints are active for requested
  power mode */
  if ( lna_info->lin_info[power_mode].lin_mode_active ==
       RF_CDMA_LNA_SWITCH_PT_INACTIVE )
  {
    RF_MSG_1( RF_MED, "rf_cdma_lna_get_rise_swpt: "
                      "Switchpoint inactive for Power Mode %d",
              power_mode );
    return rise_swpt_value = 0;
  }

  /* Obtain switchpoint data structure for the requested linearity 
  and gain step */
  sp_info_ptr = &lna_info->lin_info[power_mode].sp_info[gain_step];
  if ( sp_info_ptr->sp_mode == RF_CDMA_LNA_SWITCH_PT_MODE_FIXED )
  {
    rise_swpt_value = sp_info_ptr->sp_data.fixed.rise;
  } /* if ( sp_info_ptr->sp_mode == RF_CDMA_LNA_SWITCH_PT_MODE_FIXED ) */
  else if ( sp_info_ptr->sp_mode == RF_CDMA_LNA_SWITCH_PT_MODE_OLTA )
  {
    /* Query for the modem specific sigma delta max level value */
    int16 sigma_delta_max_lvl =  rf_cdma_msm_get_sigma_delta_max_level();
    /* Compute the rise switchpoint in dBm10 using setpoint val,
    ADC backoff val and the calibrated gain offset for band chan */
    rise_swpt_value = sigma_delta_max_lvl - 
                      sp_info_ptr->sp_data.olta.backoff - 
                      gain_offset_val;
  } /* else if ( sp_info_ptr->sp_mode == RF_CDMA_LNA_SWITCH_PT_MODE_OLTA ) */
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_lna_get_rise_swpt: Inconsistent switchpoint type" );
    rise_swpt_value = 0;
  }
  
  return rise_swpt_value;
} /* rf_cdma_lna_get_rise_swpt */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to provide the LNA fall switchpoint value for the requested
  Power mode/linearity, and LNA gain step, from the generic switchpoint
  structure provided
 
  In calibration state, this API returns a fixed default value for the fall
  switchpoint in dBm10 as calibration data to calculate OLTA switchpoints
  may not be available
 
  @param lna_info
  The LNA Info data structure based on the current mode of operation, from
  which switchpoint info must be extracted.

  @param power_mode
  The power mode for which fall switchpoints are requested.

  @param gain_step
  The gain step for which fall switchpoints are requested
 
  @param gain_offset_val
  The calibrated LNA Gain Offset value in 1/10th dB for the gain state
  corresponding to the gain step. E.g. G0 corresponds to step G1->G0

  @return
  Signed 16-bit rise switchpoint value in dBm10.
*/
int16
rf_cdma_lna_get_fall_swpt
(
  const rf_cdma_lna_info_type* lna_info,
  rfm_cdma_power_mode_type power_mode,
  rfm_lna_gain_step_type gain_step,
  int16 gain_offset_val
)
{
  const rf_cdma_lna_sp_info_type* sp_info_ptr;
  int16 fall_swpt_value;

  /* If in calibration state, return a fixed value to ensure
  monotonicity of switchpoints programmed to FW */
  if ( rfm_get_calibration_state() == TRUE )
  {
    fall_swpt_value = 
      rf_cdma_lna_fall_swpts_fixed_default[gain_step];

    RF_MSG_2( RF_LOW, "rf_cdma_lna_get_fall_swpt: "
                      "In cal state, programming fixed default. Gain state %d, f swpt %d",
              gain_step, fall_swpt_value );

    return fall_swpt_value;
  }

  if ( lna_info == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_lna_get_fall_swpt: "
                      "LNA NV structure isn't allocated." );
    return fall_swpt_value = 0;
  }

  /* Check whether switchpoints are active for requested
  power mode */
  if ( lna_info->lin_info[power_mode].lin_mode_active ==
       RF_CDMA_LNA_SWITCH_PT_INACTIVE )
  {
    RF_MSG_1( RF_MED, "rf_cdma_lna_get_fall_swpt: "
                      "Switchpoint inactive for Power Mode %d",
              power_mode );
    return fall_swpt_value = 0;
  }

  /* Obtain switchpoint data structure for the requested linearity 
  and gain step */
  sp_info_ptr = &lna_info->lin_info[power_mode].sp_info[gain_step];
  if ( sp_info_ptr->sp_mode == RF_CDMA_LNA_SWITCH_PT_MODE_FIXED )
  {
    fall_swpt_value = sp_info_ptr->sp_data.fixed.fall;
  } /* if ( sp_info_ptr->sp_mode == RF_CDMA_LNA_SWITCH_PT_MODE_FIXED ) */
  else if ( sp_info_ptr->sp_mode == RF_CDMA_LNA_SWITCH_PT_MODE_OLTA )
  {
    /* Query for the modem specific sigma delta max level value */
    int16 sigma_delta_max_lvl =  rf_cdma_msm_get_sigma_delta_max_level();
    /* Compute the fall switchpoint in dBm10 using setpoint val,
    ADC backoff val and the calibrated gain offset for band chan.
    Fall switchpoint value uses hysteresis val over the rise
    switchpoint value */
    fall_swpt_value = sigma_delta_max_lvl - 
                      sp_info_ptr->sp_data.olta.backoff - 
                      gain_offset_val -
                      sp_info_ptr->sp_data.olta.hysteresis;

    RF_MSG_4( RF_LOW,"rf_cdma_lna_get_fall_swpt: sigma delta: %d |olta backoff:" 
     " %d | gain offest_value: %d | hyst: %d",sigma_delta_max_lvl,
            sp_info_ptr->sp_data.olta.backoff, gain_offset_val, 
            sp_info_ptr->sp_data.olta.hysteresis);
    
  } /* else if ( sp_info_ptr->sp_mode == RF_CDMA_LNA_SWITCH_PT_MODE_OLTA ) */
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_lna_get_fall_swpt: Inconsistent switchpoint mode" );
    fall_swpt_value = 0;
  }
  
  return fall_swpt_value;
} /* rf_cdma_lna_get_fall_swpt */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to provide the LNA rise timer hysteresis value for the requested
  Power mode/linearity, and LNA gain step, from the generic switchpoint
  structure provided

  @param lna_info
  The LNA Info data structure based on the current mode of operation, from
  which timer hysteresis info must be extracted.

  @param power_mode
  The power mode for which rise timer hysteresis is requested.

  @param gain_step
  The gain step for which rise timer hysteresis is requested
 
  @return
  Signed 16-bit post-rise timer hysteresis val in microseconds.
*/
int16
rf_cdma_lna_get_post_rise_hold
(
  const rf_cdma_lna_info_type* lna_info,
  rfm_cdma_power_mode_type power_mode,
  rfm_lna_gain_step_type gain_step
)
{
  const rf_cdma_lna_sp_info_type* sp_info_ptr;
  int16 post_rise_hold_val;

  if ( lna_info == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_lna_get_post_rise_hold: "
                      "LNA NV structure isn't allocated." );
    return post_rise_hold_val = 0;
  }

  /* Check whether switchpoints are active for requested
  power mode */
  if ( lna_info->lin_info[power_mode].lin_mode_active ==
       RF_CDMA_LNA_SWITCH_PT_INACTIVE )
  {
    RF_MSG_1( RF_MED, "rf_cdma_lna_get_post_rise_hold: "
                      "Switchpoint inactive for Power Mode %d",
              power_mode );
    return post_rise_hold_val = 0;
  }

  /* Obtain switchpoint data structure for the requested linearity 
  and gain step */
  sp_info_ptr = &lna_info->lin_info[power_mode].sp_info[gain_step];
  post_rise_hold_val = sp_info_ptr->post_rise_hold;
  
  return post_rise_hold_val;
} /* rf_cdma_lna_get_post_rise_hold */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to provide the LNA fall timer hysteresis value for the requested
  Power mode/linearity, and LNA gain step, from the generic switchpoint
  structure provided

  @param lna_info
  The LNA Info data structure based on the current mode of operation, from
  which timer hysteresis info must be extracted.

  @param power_mode
  The power mode for which fall timer hysteresis is requested.

  @param gain_step
  The gain step for which fall timer hysteresis is requested
 
  @return
  Signed 16-bit post-fall timer hysteresis val in microseconds.
*/
int16
rf_cdma_lna_get_post_fall_hold
(
  const rf_cdma_lna_info_type* lna_info,
  rfm_cdma_power_mode_type power_mode,
  rfm_lna_gain_step_type gain_step
)
{
  const rf_cdma_lna_sp_info_type* sp_info_ptr;
  int16 post_fall_hold_val;

  if ( lna_info == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_lna_get_post_fall_hold: "
                      "LNA NV structure isn't allocated." );
    return post_fall_hold_val = 0;
  }

  /* Check whether switchpoints are active for requested
  power mode */
  if ( lna_info->lin_info[power_mode].lin_mode_active ==
       RF_CDMA_LNA_SWITCH_PT_INACTIVE )
  {
    RF_MSG_1( RF_MED, "rf_cdma_lna_get_post_fall_hold: "
                      "Switchpoint inactive for Power Mode %d",
              power_mode );
    return post_fall_hold_val = 0;
  }

  /* Obtain switchpoint data structure for the requested linearity 
  and gain step */
  sp_info_ptr = &lna_info->lin_info[power_mode].sp_info[gain_step];
  post_fall_hold_val = sp_info_ptr->post_fall_hold;
  
  return post_fall_hold_val;
} /* rf_cdma_lna_get_post_fall_hold */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Checks if LNA rise and fall values satify the conditions required by MDSP
 
  @details
  FW requires that the following condition be satisfied:
  fall_1 <= rise_1 < fall_2 <= rise_2 < fall_3 <= rise_3 < fall_4 <= rise_4
 
  @param rise_values
  The LNA rise switch-points for each gain state
 
  @param fall_values
  The LNA fall switch-points for each gain state  
    
  @return
  TRUE on success and FALSE on failure
*/
boolean 
rf_cdma_check_rise_fall_values
(
  int16 rise_vals[RFM_LNA_GAIN_STEP_NUM],
  int16 fall_vals[RFM_LNA_GAIN_STEP_NUM]
)
{

  if 
  (  
    ( fall_vals[RFM_LNA_GAIN_STEP_0_TO_1] <= rise_vals[RFM_LNA_GAIN_STEP_0_TO_1] ) &&
    ( rise_vals[RFM_LNA_GAIN_STEP_0_TO_1] <  fall_vals[RFM_LNA_GAIN_STEP_1_TO_2] ) &&
    ( fall_vals[RFM_LNA_GAIN_STEP_1_TO_2] <= rise_vals[RFM_LNA_GAIN_STEP_1_TO_2] ) &&
    ( rise_vals[RFM_LNA_GAIN_STEP_1_TO_2] <  fall_vals[RFM_LNA_GAIN_STEP_2_TO_3] ) &&
    ( fall_vals[RFM_LNA_GAIN_STEP_2_TO_3] <= rise_vals[RFM_LNA_GAIN_STEP_2_TO_3]   ) &&
    ( rise_vals[RFM_LNA_GAIN_STEP_2_TO_3] <  fall_vals[RFM_LNA_GAIN_STEP_3_TO_4] ) &&
    ( fall_vals[RFM_LNA_GAIN_STEP_3_TO_4] <= rise_vals[RFM_LNA_GAIN_STEP_3_TO_4]   ) 
  )
  {
    return TRUE;
  }
  else
  {
    RF_MSG_8( RF_ERROR, "rf_cdma_check_rise_fall_values: Checks failed "
                        "[fall rise]:"
                        " LNA state 0 [%d %d] | LNA state 1 [%d %d]"
                        " LNA state 2 [%d %d] | LNA state 3 [%d %d]",
              fall_vals[RFM_LNA_GAIN_STEP_0_TO_1],
              rise_vals[RFM_LNA_GAIN_STEP_0_TO_1],
              fall_vals[RFM_LNA_GAIN_STEP_1_TO_2],
              rise_vals[RFM_LNA_GAIN_STEP_1_TO_2],
              fall_vals[RFM_LNA_GAIN_STEP_2_TO_3],
              rise_vals[RFM_LNA_GAIN_STEP_2_TO_3],
              fall_vals[RFM_LNA_GAIN_STEP_3_TO_4],
              rise_vals[RFM_LNA_GAIN_STEP_3_TO_4]);

    return FALSE;
  }

} /* rf_cdma_check_rise_fall_values */

/*! @} */

