/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            GRR Downlink Signalling Counter Module

GENERAL DESCRIPTION
  This module contains the code to set up and operate the downlink signalling
  counter.

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_gprs_dsc.c#1 $
$DateTime: 2016/12/13 08:00:21 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
09/09/14    sjw     Revised DSC algorithm for multi-SIM and Dual Receive support
dd/mm/yy    who     Sample text for edit history
===========================================================================*/
#include "geran_variation.h"
#include "customer.h"  /* Contains FEATURE_... defines */
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_gprs_dsc.h"
#include "rr_general.h"
#include "rr_gprs_defs.h"
#include "rr_log.h"
#include "rr_multi_sim.h"
#include "rr_nv.h"
#include "rr_l1_send.h"
#include "rr_pscan.h"
#ifdef FEATURE_IDLE_DRX_SCALING
#include "rr_idle_drx.h"
#endif /* FEATURE_IDLE_DRX_SCALING */

/*===================================================================
            DEFINES
=====================================================================*/
#define RR_DSC_THRESHOLD_POWER_SCAN_VALIDITY 25 /* percentage */

typedef struct
{
  int    gprs_dsc_counter; /* Downlink Signalling Counter */
  int    ms_blk_watermark; /* Multi-SIM Block Watermark   */
  uint32 denial_period_ms; /* Time in milliseconds since last good/bad decode */
} rr_gprs_dsc_data_t;

/*===================================================================
            LOCAL DATA
=====================================================================*/
static rr_gprs_dsc_data_t rr_gprs_dsc_data[NUM_GERAN_DATA_SPACES];

/*===================================================================
                      FUNCTION DECLARATIONS
=====================================================================*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_gprs_dsc_data_t*
 */
static rr_gprs_dsc_data_t *rr_gprs_dsc_get_data_ptr(const gas_id_t gas_id)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_gprs_dsc_data[as_index];
}

/****************************************************************************
 *   Function name:  rr_gprs_get_max_dsc_value
 *   --------------
 *
 *   Description:    Returns upper bound for Downlink Signalling Counter
 *   ------------
 *
 *   Parameters:     whether to use scaled DSC or not, GAS ID
 *   -----------
 *
 *
 *   Returns:        Max DSC value for the current serving cell
 *   --------
 *
 *
 ****************************************************************************/
int rr_gprs_get_max_dsc_value(const boolean use_scaling, const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  int                bs_pa_mfrms;
  byte               extended_dsc = 0;

  if (scell_info_ptr && scell_info_ptr->gsm.valid_data_flag)
  {
    bs_pa_mfrms = (int)(scell_info_ptr->gsm.control_channel_description.BS_PA_MFRMS);

    if (bs_pa_mfrms == 0)
    {
      /* default to smallest value (corresponding to fastest paging cycle) */
      bs_pa_mfrms = BS_PA_MFRMS_MIN;
      MSG_GERAN_HIGH_0_G("DSC: default to BS_PA_MFRMS=2");
    }
  }
  else
  {
    bs_pa_mfrms = BS_PA_MFRMS_MIN;
  }

#ifdef FEATURE_IDLE_DRX_SCALING
  if (use_scaling)
  {
    extended_dsc = rr_idle_drx_get_extended_dsc(gas_id);
  }
#else
  NOTUSED(use_scaling);
#endif /* FEATURE_IDLE_DRX_SCALING */

  if (extended_dsc != 0)
  {
    MSG_GERAN_HIGH_0_G("Using scaled DSC");
    return extended_dsc;
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Using normal DSC");
    return(GPRS_DSC_DIVIDEND / bs_pa_mfrms);
  }
}


/****************************************************************************
 *   Function name:  rr_gprs_return_gprs_dsc_counter_value
 *   --------------
 *
 *   Description: This function returns the dsc counter value
 *
 *   Parameters: An event and a possible message
 *   -----------
 *
 *   Returns:  gprs_dsc_counter value
 *   --------
 *
 *
 ****************************************************************************/
int rr_gprs_return_gprs_dsc_counter_value(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_gprs_dsc_data_t *rr_gprs_dsc_data_ptr = rr_gprs_dsc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_dsc_data_ptr);

  return rr_gprs_dsc_data_ptr->gprs_dsc_counter;
}

#ifdef FEATURE_IDLE_DRX_SCALING
/****************************************************************************
 *   Function name:  rr_gprs_set_gprs_dsc_counter_value
 *   --------------
 *
 *   Description: This function sets the dsc counter value
 *
 *   Parameters: New DSC, GAS ID
 *   -----------
 *
 *   Returns:  None
 *   --------
 *
 *
 ****************************************************************************/
void rr_gprs_set_gprs_dsc_counter_value(const int dsc, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_gprs_dsc_data_t *rr_gprs_dsc_data_ptr = rr_gprs_dsc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_dsc_data_ptr);

  MSG_GERAN_HIGH_2_G("gprs_dsc_counter changed from %d to %d", rr_gprs_dsc_data_ptr->gprs_dsc_counter, dsc);
  rr_gprs_dsc_data_ptr->gprs_dsc_counter = dsc;
  return;
}
#endif /* FEATURE_IDLE_DRX_SCALING */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/****************************************************************************
 *   Function name:  rr_gprs_serving_cell_resync_allowed
 *   --------------
 *
 *   Description: This function checks whether the Multi-SIM serving cell
 *                "resync" mechanism should be allowed to take place.
 *
 *
 *   Parameters: GAS ID
 *   -----------
 *
 *
 *   Returns:  TRUE if a resync is allowed
 *   --------
 *
 *
 ****************************************************************************/
static boolean rr_gprs_serving_cell_resync_allowed(const l2_channel_type_T l2_channel_type, const gas_id_t gas_id)
{
  MSG_GERAN_HIGH_1_G("Allow serving cell resync to occur, channel type %d", (int)l2_channel_type);
  return(TRUE);
}

/****************************************************************************
 *   Function name:  rr_gprs_epd_preference
 *   --------------
 *
 *   Description: This function checks whether Enhanced Page Decoding  pref needs to be set as 
 *                     EPD_ON_PREFERRED/EPD_OFF_PREFERRED for Multi-SIM which is 
 *                     Enumerated value of GRR preference for EPD
 *
 *
 *   Parameters: GAS ID
 *   -----------
 *
 *
 *   Returns: RR_L1_EPD_NO_CHANGE if sys info refresh is allowed else
 *                RR_L1_EPD_ON_PREFERRED  if timer is running, else
 *                RR_L1_EPD_OFF_PREFERRED 
 *   --------
 *
 *
 ****************************************************************************/
static rr_l1_epd_preference_t rr_gprs_epd_preference(const gas_id_t gas_id)
{
  if (rr_ms_sys_info_refresh_blocked(gas_id))
  {
    if (rr_timer_has_expired(RR_EPD_TIMER, gas_id))
    {
      /* timer has expired at some point in the past and not been cleared yet */
      (void)rr_timer_cancel(RR_EPD_TIMER, gas_id);
      return(RR_L1_EPD_OFF_PREFERRED);
    }

    if (FALSE == rr_timer_is_running(RR_EPD_TIMER, gas_id))
    {
      if (rr_timer_start(RR_EPD_TIMER, RR_EPD_TIMER_DURATION, gas_id))
      {
        MSG_GERAN_HIGH_1_G("EPD timer started for %dms", RR_EPD_TIMER_DURATION);
      }
    }
    /* EPD duty cycle timer is now running */
    return(RR_L1_EPD_ON_PREFERRED);
  }

  return(RR_L1_EPD_NO_CHANGE);
}

#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/****************************************************************************
 *   Function name:  rr_gprs_check_dsc_in_idle
 *   --------------
 *
 *   Description: This function maintains the Downlink Signalling Counter by
 *   ------------ handling the reported block quality which may modify it.
 *
 *
 *   Parameters: Block quality enumeration, channel type and GAS ID
 *   -----------
 *
 *
 *   Returns:  EV_NO_EVENT, EV_DSF_RESELECTION, EV_DSF_FOR_DENIALS or RR_EV_START_RESELECTION_POWER_SCAN
 *   --------
 *
 *
 ****************************************************************************/
rr_event_T rr_gprs_check_dsc_in_idle(const gl1_block_quality_t block_quality, const l2_channel_type_T l2_channel_type, const gas_id_t gas_id)
{
  rr_gprs_dsc_data_t     *rr_gprs_dsc_data_ptr;
  rr_event_T             output_event;
  int                    maximum_value;
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  boolean                resync_serving_cell = FALSE;
  rr_l1_epd_preference_t epd_pref = RR_L1_EPD_NO_CHANGE; 
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  /* Obtain a pointer to the module data */
  rr_gprs_dsc_data_ptr = rr_gprs_dsc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_dsc_data_ptr);

  /* Retrieve the maximum value of the counter for the serving cell */
  maximum_value = rr_gprs_get_max_dsc_value(TRUE, gas_id);

  /* Check that the DSC is not greater than the maximum permitted value */
  /* In other words, ensure that the DSC has been initialised correctly */
  if (rr_gprs_dsc_data_ptr->gprs_dsc_counter > maximum_value)
  {
    MSG_GERAN_HIGH_2_G("DSC %d is greater than max %d, reinitialise now", rr_gprs_dsc_data_ptr->gprs_dsc_counter, maximum_value);
    rr_gprs_dsc_data_ptr->gprs_dsc_counter = maximum_value;
    rr_gprs_dsc_data_ptr->ms_blk_watermark = 0;
  }

  /* Now process the incoming block quality value received from Layer 1 */
  switch(block_quality)
  {
    case GL1_BLOCK_QUALITY_GOOD:
    {
      /* increment the DSC by one for each good block */
      rr_gprs_dsc_data_ptr->gprs_dsc_counter = rr_gprs_dsc_data_ptr->gprs_dsc_counter + 1;

      /* cap the DSC at the maximum value for the current cell */
      if (rr_gprs_dsc_data_ptr->gprs_dsc_counter > maximum_value)
      {
        rr_gprs_dsc_data_ptr->gprs_dsc_counter = maximum_value;
      }

      /*decrement the watermark by one for each good block*/
      rr_gprs_dsc_data_ptr->ms_blk_watermark = rr_gprs_dsc_data_ptr->ms_blk_watermark -1;

      if (rr_gprs_dsc_data_ptr->ms_blk_watermark <= 0)
      {
        rr_gprs_dsc_data_ptr->ms_blk_watermark = 0;
      }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
      /* on reception of a good block, reset the multi-SIM counter */
      rr_ms_reset_block_quality(gas_id);
      epd_pref = rr_gprs_epd_preference(gas_id);
      /* reset the denial duration since a good block means access was granted */
      rr_gprs_dsc_data_ptr->denial_period_ms = 0;
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
    }
    break;

    case GL1_BLOCK_QUALITY_BAD:
    {
      /* decrement the DSC by four for each bad block (xCCH failure) */
      rr_gprs_dsc_data_ptr->gprs_dsc_counter = rr_gprs_dsc_data_ptr->gprs_dsc_counter - 4;

      /* floor the value at zero */
      if (rr_gprs_dsc_data_ptr->gprs_dsc_counter < 0)
      {
        rr_gprs_dsc_data_ptr->gprs_dsc_counter = 0;
      }

      /* report the DSC and the denial period */
      MSG_GERAN_HIGH_2_G("DSC-- = %d (denied for %dms)",
                         (int)(rr_gprs_dsc_data_ptr->gprs_dsc_counter),
                         (int)(rr_gprs_dsc_data_ptr->denial_period_ms));

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
      /* check whether the denial period exceeds the value loaded from EFS at power-on */
      if (rr_gprs_dsc_data_ptr->denial_period_ms > rr_nv_get_scell_resync_time_millisecs(RR_GAS_ID_TO_AS_ID))
      {
        /* set flag to generate the appropriate output event below */
        resync_serving_cell = rr_gprs_serving_cell_resync_allowed(l2_channel_type, gas_id);
      }

      /* reset the denial duration since a bad block means access was granted */
      rr_gprs_dsc_data_ptr->denial_period_ms = 0;
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
    }
    break;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
    case GL1_BLOCK_QUALITY_UNKNOWN:
    {
      /* process this missing block indication in the multi-SIM controller */
      if (GL1_BLOCK_QUALITY_BAD == rr_ms_adjust_block_quality(l2_channel_type, gas_id))
      {
        /* Increment the watermark by four for each unknown block conversion event */
        /* If the real DSC falls **below** this watermark, a DSF will be triggered */
        rr_gprs_dsc_data_ptr->ms_blk_watermark = rr_gprs_dsc_data_ptr->ms_blk_watermark + 4;

        /* report the MSIM water mark  and the DSC counter */
        MSG_GERAN_HIGH_2_G("MSIM water mark = %d (DSC counter %d)",                       
                          (int)(rr_gprs_dsc_data_ptr->ms_blk_watermark),
                          (int)(rr_gprs_dsc_data_ptr->gprs_dsc_counter));
      }

      /* increment the denial period by the DRX cycle length i.e. (235ms x BS_PA_MFRMS) */
      rr_gprs_dsc_data_ptr->denial_period_ms += RR_DSC_DRX_DURATION_PER_MULTIFRAME * (GPRS_DSC_DIVIDEND / maximum_value);

      /* avoid triggering DSC threshold indications below */
      /* because the real DSC is not changing as a result */
      maximum_value = 0;
    }
    break;
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

   default:
     MSG_GERAN_ERROR_2_G("Unexpected BQ value %d, channel %d", (int)block_quality, (int)l2_channel_type);
     break;
  }

  /* always log the current value of the DSC */
  rr_log_dsc_counter(rr_gprs_dsc_data_ptr->gprs_dsc_counter, gas_id);

  /* compare the current DSC against 0 as documented in the 3GPP spec */
  if (rr_gprs_dsc_data_ptr->gprs_dsc_counter <= 0)
  {
    /* Declare a Downlink Signalling Failure since DSC has reached zero */
    output_event = EV_DSF_RESELECTION;
    MSG_GERAN_HIGH_0_G("DSF - Force Reselection");
  }
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  else if (resync_serving_cell)
  {
    /* trigger a Reselection Power Scan for the serving cell and the BA list */
    /* to determine whether the serving cell continues to be a viable target */
    output_event = RR_EV_START_RESELECTION_POWER_SCAN;
    MSG_GERAN_HIGH_0_G("Serving cell resync required, xCCH failure detected");
  }
  else if (rr_gprs_dsc_data_ptr->gprs_dsc_counter <= rr_gprs_dsc_data_ptr->ms_blk_watermark)
  { 
    /* Declare a Downlink Signalling Failure since too many blocks have been missed */   
    output_event = EV_DSF_FOR_DENIALS;
    MSG_GERAN_HIGH_2_G("DSF - triggered by DSC %d below watermark %d",
                       rr_gprs_dsc_data_ptr->gprs_dsc_counter,
                       rr_gprs_dsc_data_ptr->ms_blk_watermark);
    
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  else
  {
    output_event = EV_NO_EVENT;
  }

#ifdef FEATURE_IDLE_DRX_SCALING
  if (maximum_value)
  {
    rr_idle_drx_check_dsc(maximum_value, rr_gprs_dsc_data_ptr->gprs_dsc_counter, gas_id);
  }
#endif /* FEATURE_IDLE_DRX_SCALING */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if ((RR_EV_START_RESELECTION_POWER_SCAN == output_event) ||
      (EV_DSF_RESELECTION                 == output_event) ||
      (EV_DSF_FOR_DENIALS                 == output_event))
  {
    /* reset the watermark to zero before acquiring any new cell */
    rr_gprs_dsc_data_ptr->ms_blk_watermark = 0;

    /* and reset the denial duration */
    rr_gprs_dsc_data_ptr->denial_period_ms = 0;

    /* if this event causes a Reselection Power Scan to start, then PIMMS */
    /* will trigger recovery when that RPS primitive is dispatched to GL1 */
  }
#ifndef PERLUTF
  else if (maximum_value)
  {
    int percent_dsc;

    /* calculate the percentage of maximum DSC */
    percent_dsc = (rr_gprs_dsc_data_ptr->gprs_dsc_counter * 100) / maximum_value;
    if (percent_dsc <= RR_DSC_THRESHOLD_POWER_SCAN_VALIDITY)
    {
      /* invalidate any power scan results since this sub is close to a DSF */
      /* and it is not desirable to share or reuse the data from this point */
      /* since it may cause us to return to this same cell, and start again */
      rr_invalidate_power_scan_results(gas_id);
    }

    /* report the current percentage to Layer 1 */
    rr_send_mph_dsc_threshold_ind(percent_dsc, l2_channel_type, epd_pref, gas_id);
  }
#endif /* !PERLUTF */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return(output_event);
}

/****************************************************************************
 *   Function name:  rr_gprs_initialise_dsc
 *   --------------
 *
 *   Description: This function initialises the Downlink Signalling Counter
 *   ------------
 *
 *   Parameters: NONE
 *   -----------
 *
 *
 *   Returns:  NONE
 *   --------
 *
 *
 ****************************************************************************/
void rr_gprs_initialise_dsc(const gas_id_t gas_id)
{
  rr_gprs_dsc_data_t *rr_gprs_dsc_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_dsc_data_ptr = rr_gprs_dsc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_dsc_data_ptr);

  rr_gprs_dsc_data_ptr->gprs_dsc_counter = rr_gprs_get_max_dsc_value(TRUE, gas_id);
  rr_gprs_dsc_data_ptr->ms_blk_watermark = 0;
  rr_gprs_dsc_data_ptr->denial_period_ms = 0;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  /* reset the counters in the DS module */
  rr_ms_reset_block_quality(gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return;
}


/****************************************************************************
 *   Function name:  rr_gprs_is_dsc_below_max_value
 *   --------------
 *
 *   Description: This function can be used to detect if the dsc has fallen
 *   ------------ below its maximum value
 *
 *   Parameters: NONE
 *   -----------
 *
 *
 *   Returns:  TRUE/FALSE
 *   --------
 *
 *
 ****************************************************************************/
boolean rr_gprs_is_dsc_below_max_value(const gas_id_t gas_id)
{
  rr_gprs_dsc_data_t *rr_gprs_dsc_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_dsc_data_ptr = rr_gprs_dsc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_dsc_data_ptr);

  return(rr_gprs_dsc_data_ptr->gprs_dsc_counter < rr_gprs_get_max_dsc_value(TRUE, gas_id));
}

/* EOF */

