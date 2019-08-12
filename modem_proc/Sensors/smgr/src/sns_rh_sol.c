/*=============================================================================
  @file sns_rh_sol.c

  This file contains functions for managing the Standing Orders List (SOL) of
  the Request Handler task

******************************************************************************
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_rh_sol.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-08-08  kl   a new sample qualtiy flag added RH_RPT_ITEM_FLAGS_INTERPOLATION_B 
  2016-07-15  pn   Uses depot interval as report's min_sampling_interval
  2016-04-07  pn   Resets report state before report gets suspended
  2016-04-04  pn   Always adds new report before deleting the old report being replaced
  2016-04-01  pn   Allows first filtered sample to be generated sooner
  2016-03-11  pn   Uses fifo_interval rather than computing it
  2016-03-08  pn   Changed MD state change request method
  2016-02-11  pn   Tracks processor type over connection
  2016-02-08  gju  Update sns_batchbuff header file name
  2016-02-05  pn   Reports rescheduling is now done by RH task
  2016-01-28  pn   Should not flush reports being deleted
  2016-01-21  gju  Use SNS_ASSERT rather than ASSERT
  2015-12-04  pn   Disassociates report from sensors before removing it from report queue
  2015-11-23  pn   Computes samples per report interval for each report item
  2015-11-10  pn   Allocates TCM memory for Query item
  2015-10-28  bd   Save rate_sum in rpt_spec to specify number of samples for each item
  2015-10-28  pn   Report being replaced with new parameters is treated as a new report
  2015-10-20  pn   Tallies report rates when activating or deactivating a report
  2015-10-19  pn   Changed scheduling of first indication of streaming sensor batching
  2015-10-12  pn   Handles pending report when QMI connection resumes
  2015-10-12  bd   Add batchbuff block tracking
  2015-09-29  pn   Added support for new FLUSH command
  2015-09-24  pn   Code cleanup
  2015-09-18  pn   - (Re)computes min sampling interval after sensor (re)configured
                   - Sets report interval when processing Buffering report requests
                   - Changed how first indication of Buffering reports is scheduled
  2015-09-15  pn   No need to keep extra copy of message header when replacing a report
  2015-09-15  pn   Sampling factor calculation must follow 90%-200% rule
  2015-09-11  pn   Made use of new function sns_rh_send_report_indication()
  2015-09-10  pn   Improved interpolation implementation
  2015-09-03  bd   Separate batchbuff for bimg and uimg 
  2015-08-25  bd   Get num_axes from availabe depot ptr 
  2015-08-24  pn   Added support for concurrent selftest and streaming
  2015-08-24  bd   Use only one depot per sensor 
  2015-08-21  bd   - Do not send sample_ready_sig for buffering report
                   - Code cleanup
  2015-08-11  gju  Use new AckNak RESPONSE_NAK_CANCELLED when replacing a pend req
  2015-07-30  bd   - Initialize batchbuffer q when report is added
                   - Remove batchbuffers when report is deleted
  2015-07-23  pn   GyroT and AccelT must be requested with their Primary counterparts
  2015-06-19  pn   Extra sampling interval no longer added to report interval
  2015-05-29  pn   Clears interpolator when switching depot
  2015-05-27  bd   Indication flow control in SMGR
  2015-05-27  pn   Avoids unnecessarily sending reports out of schedule
  2015-05-11  pn   Delays depot selection for report until after setting ODR
  2015-05-01  pn   Updates more bookkeeping info of pending reports
  2015-04-25  pn   No interpolation when requests specify ACCURATE_TS
  2015-04-21  pn   Removed references to deprecated field new_sample_idx
  2015-04-09  pn   Added support for 56-bit timestamp
  2015-03-20  pn   Schedules first indication immediately if samples are available
  2015-03-05  pn   Updates pending report after its predecessor is flushed
  2015-03-03  pn   - Bumped up upper threshold for sampling rate delta when 
                     interpolation would be necessary
                   - Fixed BW voting when adding new reports
  2015-02-17  pn   Sends appropriate signal to RH when rescheduling reports
  2015-02-05  pn   No need to reschedule report to be sent early when WM is 1
  2015-02-03  pn   - Changed reportable initial sample timestamp
                   - Fixed report schedule adjustment errors
                   - SubHz requests will always use bigImage depot
  2015-01-19  hw   Move flush report function to uImage to support modem sync
  2015-01-13  pn   Fixed Flush command handling
  2014-12-30  pn   - No longer necessary to set event_done_tick
                   - Does not update sensor event when adding/deleting event gated requests
  2014-12-19  pn   Updates sensor events when report is deleted
  2014-12-18  tc   Fixed the minimum calculation when setting the report timer
  2014-12-16  pn   Fixed memory leaks when replacing reports
  2014-12-09  pn   Updates event_done_tick only if sensor is not being configured
  2014-12-05  pn   - Handles report delete the same regardless of how it's initiated
                   - Removed a flawed condition in sns_rh_select_item_depot()
  2014-12-03  pn   - Handles report replacement for Periodic reports
                   - Uses uImage depot whenever possible
  2014-11-25  jtl  Add support for timestamp accurate clients in monitor ind
  2014-11-24  pn   Fixed report interval calculation for reports with multiple 
                   FIFO sensors
  2014-11-18  jc   Removed quantization error in report time interval
  2014-11-14  pn   Sends sensor status indication when updating client count
  2014-11-12  pn   Sets depot_ptr of report item before releasing sensor mutex
  2014-11-11  tc   Allow subHz sampling requests to be periodic-like
  2014-11-06  pn   Correctly selects sample depot
  2014-11-04  pn   Enables sensor events discrimately
  2014-10-28  jms  Rpt sending with effective_rate_hz=0 fix
  2014-10-26  pn   Resets bookkeeping info when using bigImage depot
  2014-10-22  pn   Buffering report scheduling improvements
  2014-10-15  rt   Added 'deleting' state to reports
  2014-10-14  pn   Updated algorithm for making Buffering request periodic-like 
  2014-10-02  pn   - Uses ODR in determining which depot to assign to report item
                   - Synchronizes reports with same report rate
  2014-09-25  pn   - Selects appropriate depot for report item
                   - Keeps track whether big-image depot is in use
  2014-09-23  pn   Added QDSS logging
  2014-09-16  pn   Event gated reports can be deactivated and reactivated
  2014-09-11  pn   Report rate of 0 should not be rejected
  2014-09-11  pn   Protects all shared sensor state variables, not just client DB
  2014-09-08  pn   Maintains external report rate 
  2014-09-04  pn   Simplified report interval calculation
  2014-09-02  rt   Added signal for UPDATE_REPORT_TIMER
  2014-08-08  pn   Replaced SMGR_U_ALLOC_STRUCT with SMGR_ANY_ALLOC_STRUCT
  2014-08-05  pn   FIFO must be flushed when clients send Flush command
  2014-07-31  pn   Obsoleted FEATURE_TEST_DRI
  2014-07-25  pn   Makes sure report is set up completely before flushing it
  2014-07-21  pn   Restructured to support event_gated_buffering
  2014-06-16  pn   Reduced unnecessary updates to report info; removed unused code
  2014-06-16  pn   Removed references to unused circular buffer
  2014-05-28  tc   SubHz Sampling
  2014-05-12  tc   Converted frequencies to fixed point, Q16. This is to
                   accommodate frequencies less than 1. Q16 fixed point is
                   used, instead of floating point, because the Q6 does not
                   have hardware-support for floating point division.
                   Also added support for the overloaded sub-Hz sampling rate
                   fields in the SMGR API.
  2014-05-13  vy   Added interface to PM for stat reporting
  2014-04-29  pn   Refactored to support microImage
  2014-04-24  pn   Initial version

============================================================================*/

#define __SNS_MODULE__ SNS_RH

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <float.h>    // This is included to gain access to the FLT_MAX macro

#include "sns_memmgr.h"
#include "sns_batchbuff.h"
#include "sns_rh_batchbuff.h"
#include "sns_rh_internal_svc.h"
#include "sns_rh_main.h"
#include "sns_rh_main_uimg.h"
#include "sns_rh_mr.h"
#include "sns_rh_rpt_gen.h"
#include "sns_rh_sol.h"
#include "sns_rh_util.h"
#include "sns_smgr_main.h"
#include "sns_smgr_util.h"
#include "sns_smgr_pm_if.h"

#ifdef SNS_QDSP_SIM
#include "sns_qdsp_playback_utils.h"
extern int playback_next_sample_ts;
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* When ratio of max sampling rate and report rate in a buffering request 
   is below this threshold the report is considered periodic-like. */
#define RH_SOL_PERIODIC_LIKE_RATIO_THRESHOLD        1.9f

/*===========================================================================

                   MACROS

===========================================================================*/


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Local Functions
 * -------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION:   sns_rh_cic_init

===========================================================================*/
/*!
  @brief Initializes CIC filter object

  @param[io] cic_obj_ptr: CIC filter state structure
  @param[i] factor: Decimation factor, must be >0, >= 2 for practical uses
  @param[i] freq_q16: frequency of input data
  @param[i] num_axes: Number of axes (example 3 for XYZ)

  @return
    void
*/
/*=========================================================================*/
SMGR_STATIC void sns_rh_cic_init(
  sns_rh_cic_obj_s* cic_obj_ptr,
  uint16_t          factor,
  q16_t             freq_q16,
  uint8_t           num_axes)
{
  sns_ddf_odr_t in_freq_hz = DDF_FIXTOODR_FLOOR_Q16(freq_q16);
  SNS_OS_MEMZERO(cic_obj_ptr, sizeof(sns_rh_cic_obj_s));
  cic_obj_ptr->factor          = factor;
  cic_obj_ptr->input_frequency = freq_q16;
  cic_obj_ptr->axis_cnt        = num_axes;
  cic_obj_ptr->delay_ticks     = 
    sns_em_convert_usec_to_localtick((1000000/in_freq_hz)*(factor-1)/2);
  SNS_SMGR_PRINTF4(
    HIGH, "cic_init - factor=%u freq=0x%x/%u delay=%u",
    factor, freq_q16, in_freq_hz, cic_obj_ptr->delay_ticks);
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_compute_cic_factor

===========================================================================*/
SMGR_STATIC void sns_rh_sol_compute_cic_factor(sns_rh_rpt_item_s* item_ptr)
{
  uint16_t cic_factor;
  q16_t    cic_frequency;
  uint16_t old_count = item_ptr->cic_obj.count;
  /* ---------------------------------------------------------------------- */

  if ( SMGR_BIT_CLEAR_TEST(item_ptr->flags, RH_RPT_ITEM_FLAGS_DECIM_FILTER_B) )
  {
    cic_factor = 1;
    cic_frequency = item_ptr->sampling_rate_hz;
  }
  else
  {
    cic_factor = item_ptr->sampling_factor;
    
    if ( SMGR_BIT_TEST(item_ptr->flags, RH_RPT_ITEM_FLAGS_ACCURATE_TS_B) )
    {
      cic_frequency = item_ptr->ddf_sensor_ptr->depot_data_rate;
    }
    else
    {
      cic_frequency = item_ptr->sampling_rate_hz * cic_factor;
    }
  }

  if ( (cic_factor != item_ptr->cic_obj.factor) ||
       (cic_frequency != item_ptr->cic_obj.input_frequency) )
  {
    sns_rh_cic_init(&item_ptr->cic_obj, cic_factor, cic_frequency,
                    item_ptr->depot_ptr->num_axes);
  }
  SNS_SMGR_PRINTF4(
    LOW, "comp_cic_factor - factor=%d freq=0x%X count=%u/%u",
    cic_factor, cic_frequency, old_count, item_ptr->cic_obj.count);
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_compute_sampling_factor

===========================================================================*/
SMGR_STATIC void sns_rh_sol_compute_sampling_factor(sns_rh_rpt_item_s* item_ptr)
{
  const sns_smgr_ddf_sensor_s* ddf_sensor_ptr = item_ptr->ddf_sensor_ptr;
  q16_t resulting_rate = ddf_sensor_ptr->depot_data_rate;
  /* ---------------------------------------------------------------------- */

  item_ptr->sampling_count  = 0;
  item_ptr->sampling_factor = 1;

  if ( !sns_smgr_ddf_sensor_is_event_sensor(item_ptr->ddf_sensor_ptr) &&
       ddf_sensor_ptr->depot_data_rate != item_ptr->sampling_rate_hz )
  {
    item_ptr->sampling_factor = 
      ddf_sensor_ptr->depot_data_rate / item_ptr->sampling_rate_hz;
    item_ptr->sampling_factor = MAX(item_ptr->sampling_factor, 1);
  }

  if ( item_ptr->sampling_factor > 1 )
  {
    int8_t change = 0;
    resulting_rate = ddf_sensor_ptr->depot_data_rate / item_ptr->sampling_factor;

    if ( resulting_rate < 
         (item_ptr->sampling_rate_hz * (100-SMGR_ODR_THRESHOLD_DELTA_PCT) / 100) )
    {
      item_ptr->sampling_factor--;
      change = -1;
    }
    else if ( resulting_rate > item_ptr->sampling_rate_hz << 1 )
    {
      item_ptr->sampling_factor++;
      change = 1;
    }
    if ( change != 0 )
    {
      resulting_rate = ddf_sensor_ptr->depot_data_rate / item_ptr->sampling_factor;
      SNS_SMGR_PRINTF1(LOW, "sampling_factor - factor adjusted %d", change);
    }
  }

  SNS_SMGR_PRINTF4(
    MED, "sampling_factor - factor=%u rate(have/want/get)=0x%x / 0x%x /0x%x", 
    item_ptr->sampling_factor, ddf_sensor_ptr->depot_data_rate, 
    item_ptr->sampling_rate_hz, resulting_rate);
}

/*===========================================================================

  FUNCTION:   smgr_interpolation_required

===========================================================================*/
/*!
  @Detail Interpolation is done only if
          - sensor is streaming sensor, and
          - accurate timestamp is not requested, and
          - requested rate is not a factor of sampling rate

  @param[i] item_ptr - the report item associated with a sensor

  @return true if interpolation is required
*/
/*=========================================================================*/
SMGR_STATIC bool sns_rh_sol_interpolation_required(
  const sns_rh_rpt_item_s* item_ptr)
{
  bool interpolation_required = false;
  /* ---------------------------------------------------------------------- */

  if ( !sns_smgr_ddf_sensor_is_event_sensor(item_ptr->ddf_sensor_ptr) &&
       ((SMGR_BIT_CLEAR_TEST(item_ptr->flags, RH_RPT_ITEM_FLAGS_ACCURATE_TS_B) &&
       item_ptr->ddf_sensor_ptr->depot_data_rate % item_ptr->sampling_rate_hz) || 
       SMGR_BIT_TEST(item_ptr->flags, RH_RPT_ITEM_FLAGS_INTERPOLATION_B) )) 
       
  {
    interpolation_required = true;
  }

  return interpolation_required;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_determine_sample_quality

===========================================================================*/
SMGR_STATIC void sns_rh_sol_determine_sample_quality(sns_rh_rpt_item_s* item_ptr)
{
  /* determines sample quality to be used in indications */
  if ( sns_rh_sol_interpolation_required(item_ptr) )
  {
    item_ptr->interpolator_obj.item_ptr = item_ptr;
    if ( item_ptr->cic_obj.factor > 1 )
    {
      item_ptr->quality = SNS_SMGR_ITEM_QUALITY_INTERPOLATED_FILTERED_V01;
      item_ptr->interpolator_obj.interval_ticks =
        sns_smgr_get_sched_intval(item_ptr->cic_obj.input_frequency);
    }
    else
    {
      item_ptr->quality = SNS_SMGR_ITEM_QUALITY_INTERPOLATED_V01;
      item_ptr->interpolator_obj.interval_ticks = item_ptr->sampling_interval;
    }

    if ( item_ptr->ts_last_saved != 0 )
    {
      item_ptr->interpolator_obj.desired_timestamp =
        item_ptr->ts_last_saved + item_ptr->sampling_interval;
    }
    else if ( item_ptr->ts_last_sent != 0 )
    {
      item_ptr->interpolator_obj.desired_timestamp =
        item_ptr->ts_last_sent + item_ptr->sampling_interval;
    }

    SNS_SMGR_PRINTF2(
      MED, "init_interp - intvl=%u desired_ts=%u",
      item_ptr->interpolator_obj.interval_ticks, 
      item_ptr->interpolator_obj.desired_timestamp);
  }
  else
  {
    if ( item_ptr->cic_obj.factor > 1 )
    {
      item_ptr->quality = SNS_SMGR_ITEM_QUALITY_FILTERED_V01;
    }
    else
    {
      item_ptr->quality = SNS_SMGR_ITEM_QUALITY_CURRENT_SAMPLE_V01;
    }
    item_ptr->interpolator_obj.interval_ticks    = 0;
    item_ptr->interpolator_obj.desired_timestamp = 0;
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_compute_effective_rate

===========================================================================*/
/*!
  @brief Computes the effective sampling rate of the given report item
  @param [io]  item_ptr - the report item representing a sensor/data type
  @return      none
 */
/*=========================================================================*/
SMGR_STATIC void sns_rh_sol_compute_effective_rate(sns_rh_rpt_item_s* item_ptr)
{
  const sns_smgr_ddf_sensor_s* ddf_sensor_ptr = item_ptr->ddf_sensor_ptr;
  q16_t effective_rate;
  /* ---------------------------------------------------------------------- */

  if ( item_ptr->cic_obj.factor > 1 )
  {
    effective_rate = item_ptr->cic_obj.input_frequency/item_ptr->cic_obj.factor;
  }
  else if ( SMGR_BIT_TEST(item_ptr->flags, RH_RPT_ITEM_FLAGS_ACCURATE_TS_B) )
  {
    effective_rate = ddf_sensor_ptr->depot_data_rate/item_ptr->sampling_factor;
  }
  else
  {
    effective_rate = item_ptr->sampling_rate_hz;
  }
  item_ptr->effective_rate_hz = effective_rate;
  item_ptr->sampling_interval = sns_smgr_get_sched_intval(item_ptr->effective_rate_hz);
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_compute_min_sampling_interval

===========================================================================*/
/*!
  @brief Determines the minimum sampling interval for the given report

  @details Called after ODR configuration is done

  @param[i] rpt_ptr - the report of interest

  @return true of min_sampling_interval is (re)computed
*/
/*=========================================================================*/
SMGR_STATIC bool sns_rh_sol_compute_min_sampling_interval(sns_rh_rpt_spec_s* rpt_ptr)
{
  bool recompute_required = false;
  uint8_t i;
  uint8_t ready_sensors = 0;
  /* ---------------------------------------------------------------------- */

  RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
  {
    sns_smgr_ddf_sensor_s* ddf_sensor_ptr = rpt_ptr->item_list[i]->ddf_sensor_ptr;
    if ( ddf_sensor_ptr->sensor_ptr->sensor_state != SENSOR_STATE_READY ||
         rpt_ptr->item_list[i]->state < RH_RPT_ITEM_STATE_IDLE )
    {
      break;
    }
    ready_sensors++;
    if ( !recompute_required &&
         TICK1_GTR_TICK2(ddf_sensor_ptr->client_stat.client_modifier.last_updated, 
                         rpt_ptr->ts_last_updated) )
    {
      recompute_required = true;
    }
  }

  SNS_SMGR_PRINTF4(
    LOW, "compute_min_intvl - id=%u/0x%x #ready=%u recomp=%u",
    rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle,
    ready_sensors, recompute_required);

  if ( (ready_sensors == rpt_ptr->num_items) && recompute_required )
  {
    rpt_ptr->min_sampling_interval = UINT32_MAX;
    RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
    {
      rpt_ptr->min_sampling_interval =
        MIN(rpt_ptr->min_sampling_interval,
            rpt_ptr->item_list[i]->ddf_sensor_ptr->depot_data_interval);
    }
    SNS_SMGR_PRINTF3(
      MED, "compute_min_intvl - rpt_id=%u/0x%x min_samp_intvl=%u", 
      rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle, 
      rpt_ptr->min_sampling_interval);

    rpt_ptr->ts_sensors_updated = sns_ddf_get_timestamp();
  }
  return ((ready_sensors == rpt_ptr->num_items) && recompute_required);
}

/*===========================================================================

  FUNCTION:   sns_rh_find_same_report_rate

===========================================================================*/
/*!
  @brief  Searches the report queue for a report with the same report rate
          as that of the given report.

  @param [i] new_rpt_ptr - the new report
 
  @return the first report with same report rate as given report, or NULL
 */
/*=========================================================================*/
SMGR_STATIC sns_rh_rpt_spec_s* sns_rh_find_same_report_rate(
  const sns_rh_rpt_spec_s* new_rpt_ptr)
{
  sns_rh_rpt_spec_s* rpt_ptr;
  sns_rh_rpt_spec_s* found_rpt_ptr = NULL;
  sns_q_s*           rpt_queue_ptr = sns_rh_get_report_queue();
  /* -------------------------------------------------------------------- */

  SMGR_FOR_EACH_Q_ITEM( rpt_queue_ptr, rpt_ptr, rpt_link )
  {
    if ( (rpt_ptr != new_rpt_ptr) && 
         (rpt_ptr->proc_type == new_rpt_ptr->proc_type) &&
         (rpt_ptr->state == RH_RPT_STATE_ACTIVE) && (rpt_ptr->rpt_tick != 0) && 
         (rpt_ptr->rpt_interval == new_rpt_ptr->rpt_interval) )
    {
      found_rpt_ptr = rpt_ptr;
      SNS_SMGR_PRINTF4(
        MED, "same_rate_rpt - id=%u/0x%x intvl=%u next=%u",
        rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle,
        rpt_ptr->rpt_interval, rpt_ptr->rpt_tick);
      break;
    }
  }
  return found_rpt_ptr;
}

/*===========================================================================

  FUNCTION:   sns_rh_add_reason_code

===========================================================================*/
/*!
  @brief  Add a pair (item # and reason code) to the list of reasons for failure or
    modification that will be sent in the response message.

  @detail

  @param[i]   item_idx      - 0 for report body, item 1...
  @param[i]   reason_code   - see enumeration
  @param[io]  num_pairs_ptr - number of existing items in reason pairs array
  @param[o]   reason_pairs  - array of reason pairs
  @return  none
 */
/*=========================================================================*/
SMGR_STATIC void sns_rh_sol_add_reason_code(
  uint32_t                   item_idx,
  uint8_t                    reason_code,
  uint32_t*                  num_pairs_ptr,
  sns_smgr_reason_pair_s_v01 reason_pairs[])
{
  SNS_SMGR_PRINTF3(LOW, "add_reason_code - idx=%d reason=%d pairs=%d",
                   (int)item_idx, reason_code, (int)*num_pairs_ptr );
  if ( *num_pairs_ptr < SNS_SMGR_MAX_NUM_REASONS_V01 )
  {
    reason_pairs[*num_pairs_ptr].ItemNum = (uint8_t)item_idx;
    reason_pairs[*num_pairs_ptr].Reason  = reason_code;
    (*num_pairs_ptr)++;
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_flush_report

===========================================================================*/
/*!
  @brief Called when a new report is added or an existing report is removed
         resulting in changes to ODRs of associated sensors.

  @param    none

  @return   number of reports generated
 */
/*=========================================================================*/
SMGR_STATIC bool sns_rh_flush_report(sns_rh_rpt_spec_s *rpt_ptr)
{
  bool sent = false;
  if ( rpt_ptr->state == RH_RPT_STATE_FLUSHING )
  {
    if ( sns_rh_ok_to_send_report(rpt_ptr) )
    {
      SNS_SMGR_PRINTF4(
        MED, "flush_reports - rpt_id=%u/0x%x state=0x%02x #inds=%u",
        rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle, 
        (rpt_ptr->state<<4)|rpt_ptr->state_extended, rpt_ptr->ind_count);

      sent = sns_rh_send_report_indication(rpt_ptr);
      if ( sent )
      {
        SNS_SMGR_PRINTF4(
          MED, "flush_report - rpt_id=%u/0x%x #inds=%u next_tick=%u",
          rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle, 
          rpt_ptr->ind_count, rpt_ptr->rpt_tick);
      }
      sns_rh_rpt_gen_remove_from_ready_list(rpt_ptr);
    }
    rpt_ptr->state = RH_RPT_STATE_ACTIVE;
  }
  return sent;
}

/*===========================================================================

  FUNCTION:   sns_rh_flush_reports

===========================================================================*/
/*!
  @brief Called when a new report is added or an existing report is removed
         resulting in changes to ODRs of associated sensors.

  @param    none

  @return   number of reports generated
 */
/*=========================================================================*/
uint32_t sns_rh_flush_reports(void)
{
  uint32_t           num_rpt_sent = 0;
  sns_rh_rpt_spec_s* rpt_ptr;
  sns_q_s*           rpt_queue_ptr = sns_rh_get_report_queue();
  /* ---------------------------------------------------------------------- */

  SMGR_FOR_EACH_Q_ITEM( rpt_queue_ptr, rpt_ptr, rpt_link )
  {
    if ( sns_rh_flush_report(rpt_ptr) )
    {
      num_rpt_sent++;
    }
  }
  return num_rpt_sent;
}

/*===========================================================================

  FUNCTION:  sns_rh_sol_get_ddf_sensor_list

===========================================================================*/
/*!
  @brief Returns the list of DDF sensors used by the given report

  @param[i] rpt_ptr   - the report of interest
  @param[o] ddf_sensor_ptrs - destination for the list

  @return none
 */
/*=========================================================================*/
SMGR_STATIC void sns_rh_sol_get_ddf_sensor_list(
  const sns_rh_rpt_spec_s* rpt_ptr,
  sns_smgr_ddf_sensor_s*   ddf_sensor_ptrs[SNS_SMGR_MAX_ITEMS_PER_REPORT_V01])
{
  uint32_t i;

  RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
  {
    ddf_sensor_ptrs[i] = rpt_ptr->item_list[i]->ddf_sensor_ptr;
  }
}

/*===========================================================================

  FUNCTION:  sns_rh_sol_update_sensor_events

===========================================================================*/
/*!
  @brief Affects possbible event for the DDF sensors in the given list
 
  @details After a report is deleted its associated sensors might be in
           the right condition to have their events (re)enabled.
           For example, Accel sensors that support Motion Dectect (MD) attribute
           can have MD event (re)enabled if MD is requested and Accel streaming
           is not.
 
  @param[io] ddf_sensor_ptrs - list of DDF sensors on which to operate

  @return none
 */
/*=========================================================================*/
SMGR_STATIC void sns_rh_sol_update_sensor_events(
  sns_smgr_ddf_sensor_s*   ddf_sensor_ptrs[SNS_SMGR_MAX_ITEMS_PER_REPORT_V01])
{
  uint32_t i = 0;
  while ( ddf_sensor_ptrs[i] != NULL )
  {
    sns_rh_update_sensor_event(ddf_sensor_ptrs[i++]);
  }
}

/*===========================================================================

  FUNCTION:  sns_rh_sol_find_report

===========================================================================*/
/*!
  @brief Searches active queue and suspended queue for the same report
         from the same client

  @param[i]  conn_handle - connection handle to match
  @param[i]  report_id   - report ID to match

  @return pointer to the report if found, NULL otherwise
 */
/*=========================================================================*/
SMGR_STATIC sns_rh_rpt_spec_s* sns_rh_sol_find_report(
  const void* conn_handle, 
  uint8_t report_id)
{
  sns_rh_rpt_spec_s* found_rpt_ptr = NULL;
  sns_rh_rpt_spec_s* rpt_ptr;
  sns_q_s*           q_ptr = sns_rh_get_report_queue();
  sns_q_s*           rpt_q_ptr = q_ptr;
  bool               suspended = false;
  /* ---------------------------------------------------------------------- */

  while ( found_rpt_ptr == NULL )
  {
    SMGR_FOR_EACH_Q_ITEM( q_ptr, rpt_ptr, rpt_link )
    {
      if ( (conn_handle == rpt_ptr->msg_header.connection_handle) &&
           (report_id == rpt_ptr->rpt_id) )
      {
        found_rpt_ptr = rpt_ptr;
        if ( q_ptr != rpt_q_ptr )
        {
          suspended = true;
        }
        break;
      }
    }
    if ( found_rpt_ptr == NULL && q_ptr == rpt_q_ptr )
    {
      q_ptr = sns_rh_get_suspended_report_queue();
    }
    else
    {
      break;
    }
  }

  if ( found_rpt_ptr != NULL )
  {
    SNS_SMGR_PRINTF4(
      LOW, "find_report - rpt_id=%u/0x%x suspended=%u #inds=%u",
      report_id, conn_handle, suspended, found_rpt_ptr->ind_count);
  }

  return found_rpt_ptr;
}

/*===========================================================================

  FUNCTION:  sns_rh_sol_find_and_delete_report

===========================================================================*/
/*!
  @brief Searches report queue for the given report and deletes it.

  @param[i]  conn_handle - connection handle to match
  @param[i]  report_id - report ID to match
  @param[i]  flush     - true if samples should be flushed before report deleted

  @return SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 or SNS_SMGR_RESPONSE_NAK_REPORT_ID_V01
 */
/*=========================================================================*/
uint8_t sns_rh_sol_find_and_delete_report(
  const void* conn_handle,
  uint8_t     report_id,
  bool        flush)
{
  uint8_t            ack_nak = SNS_SMGR_RESPONSE_NAK_REPORT_ID_V01;
  sns_rh_rpt_spec_s* rpt_ptr = sns_rh_sol_find_report(conn_handle, report_id);
  sns_q_s*           rpt_queue_ptr = sns_rh_get_report_queue();
  /* ---------------------------------------------------------------------- */

  if ( rpt_ptr != NULL )
  {
    sns_q_s* q_ptr = SMGR_QUE_HEADER_FROM_LINK(&rpt_ptr->rpt_link);
    SNS_SMGR_PRINTF2(MED, "find_and_delete_rpt - state=%d items=%d",
                     rpt_ptr->state, rpt_ptr->num_items);
    if ( q_ptr == rpt_queue_ptr && flush && rpt_ptr->rpt_tick != 0 )
    {
      /* Buffering report may have accumulated samples waiting to be sent */
      rpt_ptr->state = RH_RPT_STATE_FLUSHING;
      sns_rh_flush_report(rpt_ptr);
    }
    sns_rh_sol_delete_report(rpt_ptr);
    ack_nak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  }

  return ack_nak;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_find_query_item

===========================================================================*/
/*!
  @brief Searches for the report item in the Buffering request corresponding
  to the given Query request.

  @param[i] hdr_ptr  - message header
  @param[i] req_ptr  - incoming request message body
  @param[i] item_ptr_ptr - destination for address of the corresponding item, if found

  @return
   SNS_SMGR_RESPONSE_ACK_SUCCESS_V01   - found what we're looking for
   SNS_SMGR_RESPONSE_NAK_REPORT_ID_V01 - if no matching Buffering report found
   SNS_SMGR_RESPONSE_NAK_SENSOR_ID_V01 - if Buffering report does not include the
     requested sensor type
*/
/*=========================================================================*/
SMGR_STATIC uint8_t sns_rh_sol_find_query_item(
  const sns_rh_mr_header_s*                   hdr_ptr,
  const sns_smgr_buffering_query_req_msg_v01* req_ptr,
  sns_rh_rpt_item_s**                         item_ptr_ptr)
{
  uint8_t report_id = (uint8_t)req_ptr->QueryId;
  sns_rh_rpt_spec_s* report_ptr = 
    sns_rh_sol_find_report(hdr_ptr->connection_handle, report_id);
  uint8_t ackNak = SNS_SMGR_RESPONSE_NAK_REPORT_ID_V01;

  /* ---------------------------------------------------------------------- */

  if ( report_ptr != NULL )
  {
    /* found the corresponding Buffering request */
    uint8_t i;
    RH_FOR_EACH_REPORT_ITEM( report_ptr, i )
    {
      if ( (SMGR_SENSOR_ID(report_ptr->item_list[i]->ddf_sensor_ptr->sensor_ptr) ==
            req_ptr->SensorId) &&
           (report_ptr->item_list[i]->ddf_sensor_ptr->data_type == req_ptr->DataType) )
      {
        *item_ptr_ptr = report_ptr->item_list[i];
        ackNak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
        break;
      }
    }
    if ( ackNak != SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
    {
      ackNak = SNS_SMGR_RESPONSE_NAK_SENSOR_ID_V01;
    }
  }
  return ackNak;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_override_response_ack

===========================================================================*/
/*!
  @brief

  @detail

  @param[io]  orig_ack_ptr - pointer to original ack code
  @param[i]   ack_code     -  ack code to be put in response

  @return  none
 */
/*=========================================================================*/
SMGR_STATIC void sns_rh_sol_override_response_ack(
  uint8_t* orig_ack_ptr, 
  uint8_t  ack_code)
{
  SNS_SMGR_PRINTF2(HIGH, "override_resp_ack - new=%d orig=%d", 
                   ack_code, *orig_ack_ptr );
  switch ( ack_code )
  {
    case SNS_SMGR_RESPONSE_ACK_SUCCESS_V01:
      break;
    case SNS_SMGR_RESPONSE_ACK_MODIFIED_V01:
      if ( SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 == *orig_ack_ptr )
      {
        *orig_ack_ptr = ack_code;
      }
      break;
    default:
      if ( (SNS_SMGR_RESPONSE_ACK_SUCCESS_V01  == *orig_ack_ptr) ||
           (SNS_SMGR_RESPONSE_ACK_MODIFIED_V01 == *orig_ack_ptr) )
      {
        *orig_ack_ptr = ack_code;
      }
      break;
  }
}

/*===========================================================================

  FUNCTION:  sns_rh_sol_update_max_frequencies

===========================================================================*/
/*!
  @brief Updates max requested HP and overall frequencies for the given DDF sensor

  @param[i] ddf_sensor_ptr - the DDF sensor

  @return  None
*/
/*=========================================================================*/
SMGR_STATIC void sns_rh_sol_update_max_frequencies(
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  sns_smgr_client_stat_s* client_stat_ptr = &ddf_sensor_ptr->client_stat;
  sns_rh_rpt_item_s* item_ptr = client_stat_ptr->rpt_item_ptr;
  q16_t max_sampling_freq = 0;
  q16_t max_report_freq   = 0;
  uint8_t  flags          = 0;
  /* ---------------------------------------------------------------------- */

  while ( item_ptr != NULL )
  {
    if ( item_ptr->state != RH_RPT_ITEM_STATE_UNLINKING )
    {
      flags |= item_ptr->flags;
      max_sampling_freq = MAX(max_sampling_freq, item_ptr->sampling_rate_hz);
      if ( item_ptr->parent_report_ptr->q16_rpt_rate != 0 )
      {
        max_report_freq = MAX(max_report_freq, item_ptr->parent_report_ptr->q16_rpt_rate);
      }
      else
      {
        max_report_freq = MAX(max_report_freq, item_ptr->sampling_rate_hz);
      }

    }
    item_ptr = item_ptr->next_item_ptr;
  }
  if( SMGR_BIT_TEST(client_stat_ptr->flags, RH_RPT_ITEM_FLAGS_ACCURATE_TS_B) !=
      SMGR_BIT_TEST(flags, RH_RPT_ITEM_FLAGS_ACCURATE_TS_B) )
  {
    /* Presence of a high perf client has changed. Update sensor status */
    sns_rh_sol_update_sensor_status( ddf_sensor_ptr );
  }
  client_stat_ptr->max_report_freq_hz    = max_report_freq;
  client_stat_ptr->max_requested_freq_hz = max_sampling_freq;
  client_stat_ptr->flags                 = flags;

  SNS_SMGR_PRINTF4(
    MED, "update_max_freq - sensor=%u flags=0x%X reporting=0x%X sampling=0x%X",
    SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type),
    client_stat_ptr->flags, client_stat_ptr->max_report_freq_hz,
    client_stat_ptr->max_requested_freq_hz);
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_mark_reports_for_flushing

===========================================================================*/
/*!
  @brief Changes state of affected reports to FLUSHING if ODR for their sensors
         would change.

  @param[i] ddf_sensor_ptr

  @return None
*/
/*=========================================================================*/
SMGR_STATIC void sns_rh_sol_mark_reports_for_flushing(
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  uint32_t new_odr = sns_smgr_choose_odr(ddf_sensor_ptr->sensor_ptr);
  /* ---------------------------------------------------------------------- */

  SNS_SMGR_PRINTF4(
    MED, "mark_4_flushing - sensor=%u curr_odr=%u new_odr=%u depot_rate=0x%x", 
    SMGR_SENSOR_ID(ddf_sensor_ptr->sensor_ptr), ddf_sensor_ptr->current_odr,
    new_odr, ddf_sensor_ptr->depot_data_rate);

  if ( ((new_odr > 0) && (new_odr != ddf_sensor_ptr->current_odr)) ||
       (sns_smgr_compute_depot_data_rate(ddf_sensor_ptr) !=
        ddf_sensor_ptr->depot_data_rate) )
  {
    sns_rh_rpt_item_s* item_ptr = ddf_sensor_ptr->client_stat.rpt_item_ptr;

    SMGR_BIT_CLEAR(ddf_sensor_ptr->flags, SMGR_SENSOR_FLAGS_FLUSH_FIFO_B);
    SMGR_BIT_CLEAR(ddf_sensor_ptr->sensor_ptr->flags, SMGR_SENSOR_FLAGS_FLUSH_FIFO_B);

    while ( item_ptr != NULL )
    {
      sns_rh_rpt_spec_s* rpt_ptr = item_ptr->parent_report_ptr;
      if ( SNS_RH_REPORT_IS_BUFFERING(rpt_ptr) &&
           rpt_ptr->state != RH_RPT_STATE_DELETING &&
           item_ptr->state != RH_RPT_ITEM_STATE_LINKING )
      {
        SNS_SMGR_PRINTF2(
          LOW, "mark_4_flushing - rpt_id=%u/0x%x", 
          rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle);
        sns_rh_prepare_report_for_flushing(rpt_ptr);
      }
      item_ptr = item_ptr->next_item_ptr;
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_update_sensor_when_item_changed

===========================================================================*/
/*!
  @brief Updates bookeeping info when an item is linked to or unlinked from
         the given DDF sensor.

  @param[i] ddf_sensor_ptr
  @param[i] in_item_spec_ptr the item spec in the new report or to bedeleted
  @return None
*/
/*=========================================================================*/
SMGR_STATIC void sns_rh_sol_update_sensor_when_item_changed (
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr,
  sns_rh_rpt_item_s*     in_item_spec_ptr)
{
  sns_smgr_sensor_s* sensor_ptr = ddf_sensor_ptr->sensor_ptr;
  sns_smgr_client_stat_s* cs_ptr = &ddf_sensor_ptr->client_stat;
  int8_t change;
  uint8_t sensor_flags_bit;
  /* ---------------------------------------------------------------------- */

  if ( in_item_spec_ptr->state == RH_RPT_ITEM_STATE_LINKING )
  {
    change           = 1;
    sensor_flags_bit = SMGR_SENSOR_FLAGS_ITEM_ADD_B;
  }
  else
  {
    change           = -1;
    sensor_flags_bit = SMGR_SENSOR_FLAGS_ITEM_DEL_B;
  }

  if ( SNS_RH_REPORT_IS_STREAMING(in_item_spec_ptr->parent_report_ptr) )
  {
    cs_ptr->num_periodic_reports += change;
  }

  cs_ptr->num_rpt_items += change;
  cs_ptr->sensor_status.last_status_ts = sns_ddf_get_timestamp();

  sns_rh_sol_update_max_frequencies(ddf_sensor_ptr);

  SMGR_BIT_SET(sensor_ptr->flags, sensor_flags_bit);

  sns_rh_sol_update_sensor_status(ddf_sensor_ptr);

  sns_profiling_log_qdss(SNS_RH_LINK_SENSOR, 3, 
                         SMGR_SENSOR_ID(ddf_sensor_ptr->sensor_ptr), 
                         change, cs_ptr->num_rpt_items);

  SNS_SMGR_PRINTF4(
    HIGH, "item_changed - sensor=%d flags=0x%x #items=%d item_state=%d",
    SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type), sensor_ptr->flags,
    cs_ptr->num_rpt_items, in_item_spec_ptr->state);

  sns_rh_sol_mark_reports_for_flushing(ddf_sensor_ptr);
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_link_item_to_sensor

===========================================================================*/
/*!
  @brief associate a item spec from the sensor that the item have associated with

  @Detail if this is the first for the data type, invalidate the latest issued tick

  @param[i] in_item_spec_ptr - Point to the report specification structure

  @return
   TRUE the item spec associated successfully to the sensor
   FALSE  the item spec could not associate to the sensor because of some reasons
*/
/*=========================================================================*/
SMGR_STATIC void sns_rh_sol_link_item_to_sensor(
  sns_rh_rpt_item_s* in_item_spec_ptr)
{
  sns_smgr_ddf_sensor_s*  ddf_sensor_ptr;
  sns_rh_rpt_item_s**     item_ptr_ptr;
  uint8_t never_err;
  /* ---------------------------------------------------------------------- */

  ddf_sensor_ptr = in_item_spec_ptr->ddf_sensor_ptr;
  sns_os_mutex_pend(ddf_sensor_ptr->sensor_ptr->mutex_ptr, 0, &never_err);

  item_ptr_ptr = &ddf_sensor_ptr->client_stat.rpt_item_ptr;

  while ( *item_ptr_ptr != NULL )
  {
    item_ptr_ptr = &(*item_ptr_ptr)->next_item_ptr;
  }
  *item_ptr_ptr = in_item_spec_ptr;
  in_item_spec_ptr->state = RH_RPT_ITEM_STATE_LINKING;
  sns_rh_sol_update_sensor_when_item_changed(ddf_sensor_ptr, in_item_spec_ptr);
  if ( in_item_spec_ptr->parent_report_ptr->proc_type != SNS_PROC_SSC_V01 )
  {
    sns_smgr_update_total_sampling_freq(
      (uint16_t)DDF_FIXTOODR_CEIL_Q16(in_item_spec_ptr->sampling_rate_hz), 0);
  }
  if ( in_item_spec_ptr->parent_report_ptr->msg_header.svc_num == SNS_SMGR_SVC_ID_V01 )
  {
    sns_rh_update_sensor_event(ddf_sensor_ptr);
  }
  in_item_spec_ptr->latest_sample_timestamp_ptr = sns_rh_link_lastest_sample_ts(
     in_item_spec_ptr->ddf_sensor_ptr );
  sns_os_mutex_post(ddf_sensor_ptr->sensor_ptr->mutex_ptr);
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_unlink_item_to_sensor

===========================================================================*/
/*!
  @brief de-associate a item spec from the sensor that the item have associated with

  @Detail

  @param[i] ItemSpec_p - Point to the report specification structure

  @return
   TRUE the item spec de-associated successfully
   FAIL could not de-associate because the item spec was not associated with the sensor
*/
/*=========================================================================*/
SMGR_STATIC bool sns_rh_sol_unlink_item_to_sensor(
  sns_rh_rpt_item_s* in_item_spec_ptr)
{
  bool found = false;
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr;
  sns_rh_rpt_item_s** item_ptr_ptr;
  uint8_t never_err;
  /* ---------------------------------------------------------------------- */

  ddf_sensor_ptr = in_item_spec_ptr->ddf_sensor_ptr;
  sns_os_mutex_pend(ddf_sensor_ptr->sensor_ptr->mutex_ptr, 0, &never_err);

  item_ptr_ptr = &ddf_sensor_ptr->client_stat.rpt_item_ptr;
  while ( (*item_ptr_ptr != in_item_spec_ptr) && (*item_ptr_ptr != NULL) )
  {
    item_ptr_ptr = &(*item_ptr_ptr)->next_item_ptr;
  }
  if ( *item_ptr_ptr == in_item_spec_ptr )     /* found the element */
  {
    *item_ptr_ptr  = in_item_spec_ptr->next_item_ptr;
    in_item_spec_ptr->next_item_ptr = NULL;
    in_item_spec_ptr->state = RH_RPT_ITEM_STATE_UNLINKING;

    sns_rh_sol_update_sensor_when_item_changed(ddf_sensor_ptr, in_item_spec_ptr);
    if ( in_item_spec_ptr->parent_report_ptr->proc_type != SNS_PROC_SSC_V01 )
    {
      sns_smgr_update_total_sampling_freq(
        0, (uint16_t)DDF_FIXTOODR_CEIL_Q16(in_item_spec_ptr->sampling_rate_hz));
    }

    found = true;
  }
  sns_os_mutex_post(ddf_sensor_ptr->sensor_ptr->mutex_ptr);
  return found;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_parse_item_decimation

===========================================================================*/
/*!
  @brief Parses Decimation field of report request.

  @Detail

  @param[i] decimation - the Decimation from report request
  @param[i] item_ptr   - the report item to be updated based on decimation value

  @return
   SNS_SMGR_REASON_NULL_V01 - everything's fine
   SNS_SMGR_REASON_DEFAULT_DECIM_V01 - Decimation value requested can not be honored
*/
/*=========================================================================*/
SMGR_STATIC uint8_t sns_rh_sol_parse_item_decimation(
  uint8_t             decimation,
  sns_rh_rpt_item_s*  item_ptr)
{
  uint8_t reason_code  = SNS_SMGR_REASON_NULL_V01;
  sns_ddf_sensor_e ddf_sensor_type;

  /* ---------------------------------------------------------------------- */

  ddf_sensor_type = SMGR_SENSOR_TYPE(item_ptr->ddf_sensor_ptr->sensor_ptr,
                                     item_ptr->ddf_sensor_ptr->data_type);

  if ( (SNS_SMGR_DECIMATION_FILTER_V01  == decimation) ||
       (SNS_SMGR_DECIMATION_AVERAGE_V01 == decimation) )
  {
    if ( !sns_smgr_is_on_change_sensor(ddf_sensor_type) )
    {
      SMGR_BIT_SET(item_ptr->flags, RH_RPT_ITEM_FLAGS_DECIM_FILTER_B);
    }
    else
    {
      SNS_SMGR_PRINTF2(ERROR, "parse_item_decim - ddf_sensor=%d decim=%d",
                       ddf_sensor_type, decimation);
      reason_code = SNS_SMGR_REASON_DEFAULT_DECIM_V01;
    }
  }
  else if ( SNS_SMGR_DECIMATION_RECENT_SAMPLE_V01 != decimation )
  {
    reason_code = SNS_SMGR_REASON_DEFAULT_DECIM_V01;
  }
  return reason_code;
}

/*===========================================================================

  FUNCTION:   sns_rh_update_event_sensor_only_flag

===========================================================================*/
/*!
  @brief Sets the report's event_sensor_only flag.

  @param [io] rpt_ptr - the report of interest

  @return none
*/
/*=========================================================================*/
SMGR_STATIC void sns_rh_update_event_sensor_only_flag(sns_rh_rpt_spec_s* rpt_ptr)
{
  uint8_t i;
  rpt_ptr->event_sensors_only = true;
  RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
  {
    if ( !sns_smgr_ddf_sensor_is_event_sensor(rpt_ptr->item_list[i]->ddf_sensor_ptr) )
    {
      rpt_ptr->event_sensors_only = false;
      break;
    }
  }
  SNS_SMGR_PRINTF3(
    LOW, "event_sensor_only - rpt_id=%u/0x%x only=%u",
    rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle, rpt_ptr->event_sensors_only);
}

/*===========================================================================

  FUNCTION:   sns_rh_parse_periodic_report_item

===========================================================================*/
/*!
  @brief Fill the item spec structure with the input parameter

  @Detail: SNS_SMGR_RESPONSE_ACK_MODIFIED_V01 could override SNS_SMGR_RESPONSE_ACK_SUCCESS_V01
    NAK could overide any ACKs.

  @param ItemDef_p[i] points to the structure of incoming request message
  @param[i] ItemSpec_p - Point to the report specification structure
  @param[i] ItemSpec_p - Point to the report specification structure
  @param[i] cal_sel - calibraion selection
  @resp_ptr[i] Point to instance of response message structure

  @return
   TRUE Successfully filled the item spec
   FALSE Failed to fill the itemp spec because:
   - the sensor state is SENSOR_STATE_FAILED or otherwise not ready
   - Linking the item to the sensor had failed
   - etc
*/
/*=========================================================================*/
SMGR_STATIC bool sns_rh_parse_periodic_report_item(
  uint8_t                                     item_idx,
  const sns_smgr_periodic_report_req_msg_v01* req_ptr,
  sns_rh_rpt_item_s*                          item_ptr,
  uint32_t*                                   num_pairs_ptr,
  sns_smgr_reason_pair_s_v01*                 reason_pairs)
{
  bool success = false;
  const sns_smgr_periodic_report_item_s_v01* in_item_ptr = &req_ptr->Item[item_idx];
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr =
    sns_smgr_find_ddf_sensor(in_item_ptr->SensorId, in_item_ptr->DataType);
  /* ---------------------------------------------------------------------- */

  SNS_SMGR_PRINTF2(LOW, "parse_periodic_item - sensor=%d dtype=%d",
                   in_item_ptr->SensorId, in_item_ptr->DataType);

  if ( ddf_sensor_ptr == NULL )
  {
    /* Unknown sensor ID, delete incoming item */
    sns_rh_sol_add_reason_code(item_idx+1, SNS_SMGR_REASON_UNKNOWN_SENSOR_V01,
                               num_pairs_ptr, reason_pairs);
  }
  else if ( (SENSOR_STATE_FAILED   == ddf_sensor_ptr->sensor_ptr->sensor_state) ||
            (SENSOR_STATE_PRE_INIT == ddf_sensor_ptr->sensor_ptr->sensor_state) )
  {
    /* Device not ready, delete incoming item */
    sns_rh_sol_add_reason_code(item_idx+1, SNS_SMGR_REASON_FAILED_SENSOR_V01,
                               num_pairs_ptr, reason_pairs);
  }
  else if ( (FX_FLTTOFIX_Q16(req_ptr->ReportRate) >
             ddf_sensor_ptr->max_supported_freq_hz) &&
            (req_ptr->ReportRate <= SNS_SMGR_SAMPLING_RATE_INVERSION_POINT_V01) &&
            !sns_smgr_ddf_sensor_is_event_sensor(ddf_sensor_ptr) )
  {
    sns_rh_sol_add_reason_code(item_idx+1, SNS_SMGR_REASON_SAMPLING_RATE_V01,
                               num_pairs_ptr, reason_pairs);
    SNS_SMGR_PRINTF2(ERROR, "parse_periodic_item - req_rate=%d max_freq=0x%X",
                     req_ptr->ReportRate, ddf_sensor_ptr->max_supported_freq_hz);
  }
  else if ( (req_ptr->ReportRate == 0) && (req_ptr->Item_len > 1) &&
            ( FX_FLTTOFIX_Q16(SNS_SMGR_REPORT_RATE_DEFAULT_V01) >
              ddf_sensor_ptr->max_supported_freq_hz ) )
  {
    sns_rh_sol_add_reason_code(item_idx+1, SNS_SMGR_REASON_SAMPLING_RATE_V01,
                               num_pairs_ptr, reason_pairs);
  }
  else
  {
    SNS_OS_MEMZERO(item_ptr, sizeof(sns_rh_rpt_item_s));
    item_ptr->ddf_sensor_ptr          = ddf_sensor_ptr;
    sns_rh_select_item_depot(item_ptr);
    sns_q_init(&item_ptr->ubatch_buffer_queue);
    sns_q_init(&item_ptr->bbatch_buffer_queue);

    SMGR_SAMPLE_STATUS_SET(&item_ptr->resulting_sample, SNS_DDF_PENDING);
    item_ptr->sampling_rate_hz        = sns_smgr_rate_hz_to_q16(req_ptr->ReportRate);
    if ( item_ptr->sampling_rate_hz > item_ptr->ddf_sensor_ptr->max_supported_freq_hz )
    {
      item_ptr->sampling_rate_hz  = ddf_sensor_ptr->max_supported_freq_hz;
      sns_rh_sol_add_reason_code(item_idx+1, SNS_SMGR_REASON_DEFAULT_RATE_V01,
                                 num_pairs_ptr, reason_pairs);
      SNS_SMGR_PRINTF2(
        ERROR, "parse_periodic_item - req_rate=%d max_freq=0x%X",
        req_ptr->ReportRate, ddf_sensor_ptr->max_supported_freq_hz);
    }
    if ( item_ptr->sampling_rate_hz == 0 )
    {
      item_ptr->sampling_rate_hz = sns_smgr_rate_hz_to_q16(SNS_SMGR_REPORT_RATE_DEFAULT_V01);
      if ( item_ptr->sampling_rate_hz > ddf_sensor_ptr->max_supported_freq_hz )
      {
        item_ptr->sampling_rate_hz = ddf_sensor_ptr->max_supported_freq_hz;
      }
    }
    item_ptr->sampling_interval = sns_smgr_get_sched_intval(item_ptr->sampling_rate_hz);

    item_ptr->sensitivity = in_item_ptr->Sensitivity;
    if ( sns_rh_sol_parse_item_decimation(in_item_ptr->Decimation, item_ptr) != 0 )
    {
      sns_rh_sol_add_reason_code(item_idx+1, SNS_SMGR_REASON_DEFAULT_DECIM_V01,
                                 num_pairs_ptr, reason_pairs);
    }

    /* when cal_sel_valid is FALSE, cal_sel[i] shall be 0 */
    item_ptr->cal_sel = req_ptr->cal_sel[item_idx];
    if ( !req_ptr->cal_sel_valid || (item_idx >= req_ptr->cal_sel_len ) )
    {
      item_ptr->cal_sel = SNS_SMGR_CAL_SEL_FULL_CAL_V01;
    }

    if ( req_ptr->SampleQuality_valid &&
         (item_idx < req_ptr->SampleQuality_len) &&
         (req_ptr->SampleQuality[item_idx] &
          SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01) )
    {
      if ( SMGR_SENSOR_IS_SELF_SCHED(ddf_sensor_ptr->sensor_ptr) )
      {
        SMGR_BIT_SET(item_ptr->flags, RH_RPT_ITEM_FLAGS_ACCURATE_TS_B);
      }
      else
      {
        sns_rh_sol_add_reason_code(item_idx+1, SNS_SMGR_REASON_SAMPLE_QUALITY_NORMAL_V01,
                                   num_pairs_ptr, reason_pairs);
      }
    }

    if ( req_ptr->SampleQuality_valid &&
       (item_idx < req_ptr->SampleQuality_len) &&
       (req_ptr->SampleQuality[item_idx] &
        SNS_SMGR_SAMPLE_QUALITY_ACCURATE_FREQUENCY_V01) )
    {
      SMGR_BIT_SET(item_ptr->flags, RH_RPT_ITEM_FLAGS_INTERPOLATION_B);
    }
    SNS_SMGR_PRINTF1(LOW, "parse_periodic_item - flags=0x%x", item_ptr->flags);
    success = true;
  }
  return success;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_parse_buffering_item

===========================================================================*/
/*!
  @brief Fill the item spec with the input

  @Detail: SNS_SMGR_RESPONSE_ACK_MODIFIED_V01 could override SNS_SMGR_RESPONSE_ACK_SUCCESS_V01
    NAK could overide any ACKs.

  @param ItemDef_p[i] points to the structure of incoming request message
  @param[i] ItemSpec_p - Point to the report specification structure
  @param[i] ItemSpec_p - Point to the report specification structure
  @param[i] cal_sel - calibraion selection
  @resp_ptr[i] Point to instance of response message structure

  @return
   TRUE Successfully filled the item spec
   FALSE Failed to fill the itemp spec becasue of:
   - the sensor state is SENSOR_STATE_FAILED
   - Linking the item to the sensor was fail
   - etc
*/
/*=========================================================================*/
bool sns_rh_sol_parse_buffering_item(
  uint8_t                                  item_idx,
  const sns_smgr_buffering_req_item_s_v01* in_item_ptr,
  sns_rh_rpt_item_s*                       item_ptr,
  uint32_t*                                num_pairs_ptr,
  sns_smgr_reason_pair_s_v01*              reason_pairs)
{
  bool success = false;
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr = 
    sns_smgr_find_ddf_sensor(in_item_ptr->SensorId, in_item_ptr->DataType);
  /* ---------------------------------------------------------------------- */

  SNS_SMGR_PRINTF3(LOW, "parse_buffering_item - sensor=%d dtype=%d rate=%d",
                   in_item_ptr->SensorId, in_item_ptr->DataType,
                   in_item_ptr->SamplingRate);

  if ( ddf_sensor_ptr == NULL )
  {
    sns_rh_sol_add_reason_code(
      item_idx+1, SNS_SMGR_REASON_UNKNOWN_SENSOR_V01,
      num_pairs_ptr, reason_pairs);
  }
  else if ( (SENSOR_STATE_FAILED   == ddf_sensor_ptr->sensor_ptr->sensor_state) ||
            (SENSOR_STATE_PRE_INIT == ddf_sensor_ptr->sensor_ptr->sensor_state) )
  {
    /* Unknown sensor ID, delete incoming item */
    sns_rh_sol_add_reason_code(
      item_idx+1, SNS_SMGR_REASON_FAILED_SENSOR_V01,
      num_pairs_ptr, reason_pairs);
    SNS_SMGR_PRINTF2(
      ERROR, "parse_buffering_item - sensor=%d state=%d",
      in_item_ptr->SensorId, ddf_sensor_ptr->sensor_ptr->sensor_state);
  }
  else if ( (in_item_ptr->SamplingRate < 1) ||
            ((FX_FLTTOFIX_Q16(in_item_ptr->SamplingRate) > 
              ddf_sensor_ptr->max_supported_freq_hz) &&
             (in_item_ptr->SamplingRate <= SNS_SMGR_SAMPLING_RATE_INVERSION_POINT_V01)) )
  {
    sns_rh_sol_add_reason_code(
      item_idx+1, SNS_SMGR_REASON_SAMPLING_RATE_V01,
      num_pairs_ptr, reason_pairs);
    SNS_SMGR_PRINTF2(
      ERROR, "parse_buffering_item - req_rate=%d max=0x%X",
      in_item_ptr->SamplingRate, ddf_sensor_ptr->max_supported_freq_hz);
  }
  else
  {
    SNS_OS_MEMZERO(item_ptr, sizeof(sns_rh_rpt_item_s));
    item_ptr->ddf_sensor_ptr    = ddf_sensor_ptr;
    sns_rh_select_item_depot(item_ptr);
    sns_q_init(&item_ptr->ubatch_buffer_queue);
    sns_q_init(&item_ptr->bbatch_buffer_queue);

    item_ptr->sampling_rate_hz  = sns_smgr_rate_hz_to_q16(in_item_ptr->SamplingRate);
    item_ptr->sampling_interval = sns_smgr_get_sched_intval(item_ptr->sampling_rate_hz);

    if ( sns_rh_sol_parse_item_decimation(in_item_ptr->Decimation, item_ptr) != 0 )
    {
      sns_rh_sol_add_reason_code(item_idx+1, SNS_SMGR_REASON_DEFAULT_DECIM_V01,
                                 num_pairs_ptr, reason_pairs);
    }

    /* when cal_sel_valid is FALSE, cal_sel[i] shall be 0 */
    item_ptr->cal_sel = in_item_ptr->Calibration;

    if ( in_item_ptr->SampleQuality & SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01 )
    {
      if ( SMGR_SENSOR_IS_SELF_SCHED(ddf_sensor_ptr->sensor_ptr) )
      {
        SMGR_BIT_SET(item_ptr->flags, RH_RPT_ITEM_FLAGS_ACCURATE_TS_B);
      }
      else
      {
        sns_rh_sol_add_reason_code(
          item_idx+1, SNS_SMGR_REASON_SAMPLE_QUALITY_NORMAL_V01,
          num_pairs_ptr, reason_pairs);
      }
    }

    if ( in_item_ptr->SampleQuality & SNS_SMGR_SAMPLE_QUALITY_ACCURATE_FREQUENCY_V01 )
    {
      SMGR_BIT_SET(item_ptr->flags, RH_RPT_ITEM_FLAGS_INTERPOLATION_B);
    }
    SNS_SMGR_PRINTF1(LOW, "parse_buffering_item - flags=0x%x", item_ptr->flags);
    success = true;
  }
  return success;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_parse_notify_suspend

===========================================================================*/
SMGR_STATIC void sns_rh_sol_parse_notify_suspend(
  uint8_t                               src_module,
  bool                                  suspend_notif_valid,
  const sns_suspend_notification_s_v01* suspend_notif_ptr,
  sns_rh_rpt_spec_s*                    rpt_ptr)
{
  if ( suspend_notif_valid )
  {
    rpt_ptr->proc_type = suspend_notif_ptr->proc_type;
    rpt_ptr->send_ind_during_suspend = 
      suspend_notif_ptr->send_indications_during_suspend;
  }
  else
  {
    if ( src_module < SNS_MODULE_DSPS )
    {
      rpt_ptr->proc_type = SNS_PROC_APPS_V01;
    }
    else if ( src_module < SNS_MODULE_MDM )
    {
      rpt_ptr->proc_type = SNS_PROC_SSC_V01;
    }
    else
    {
      rpt_ptr->proc_type = SNS_PROC_MODEM_V01;
    }
    rpt_ptr->send_ind_during_suspend = false;
  }
  SNS_SMGR_PRINTF2(
    HIGH, "parse_notify_suspend - proc=%u send=%u",
    rpt_ptr->proc_type, rpt_ptr->send_ind_during_suspend);
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_check_unaccompanied_temp_request

===========================================================================*/
/*!
  @brief Checks for Temperature sensor request not accompanied by its Primary sensor

  @param[i]  rpt_ptr  - the report to check

  @return true if Temperature sensor of a FIFO sensor is requested without
          its Primary sensor
*/
/*=========================================================================*/
SMGR_STATIC bool sns_rh_sol_check_unaccompanied_temp_request(
  const sns_rh_rpt_spec_s*  rpt_ptr)
{
  bool orphan_temp = false;
  uint8_t i, j;
  for ( i=0; i<rpt_ptr->num_items && !orphan_temp; i++ )
  {
    sns_rh_rpt_item_s* item_ptr = rpt_ptr->item_list[i];
    if ( SMGR_SENSOR_TYPE(item_ptr->ddf_sensor_ptr->sensor_ptr, 
                          item_ptr->ddf_sensor_ptr->data_type) == SNS_DDF_SENSOR_TEMP &&
         SMGR_SENSOR_FIFO_IS_SUPPORTED(item_ptr->ddf_sensor_ptr->sensor_ptr) )
    {
      orphan_temp = true;
      for ( j=0; j<rpt_ptr->num_items && orphan_temp; j++ )
      {
        if ( (i != j) &&
             (rpt_ptr->item_list[j]->ddf_sensor_ptr->sensor_ptr == 
              rpt_ptr->item_list[i]->ddf_sensor_ptr->sensor_ptr) &&
             (rpt_ptr->item_list[j]->ddf_sensor_ptr->data_type == 
              SNS_SMGR_DATA_TYPE_PRIMARY_V01) )
        {
          orphan_temp = false;
        }
      }
    }
  }
  return orphan_temp;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_parse_periodic_report_request

===========================================================================*/
/*!
  @brief Parses the incoming periodic report request

  @param[i]  hdr_ptr  - message header
  @param[i]  req_ptr  - the incoming report request
  @param[io] resp_ptr - response to the request
  @param[io] rpt_ptr  - report specification

  @return
    SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 - if all goes well
    SNS_SMGR_RESPONSE_NAK_RESOURCES_V01 - out of mem
    SNS_SMGR_RESPONSE_NAK_NO_ITEMS_V01 - no item saved
*/
/*=========================================================================*/
SMGR_STATIC uint8_t sns_rh_sol_parse_periodic_report_request(
  const sns_rh_mr_header_s*              hdr_ptr,
  sns_smgr_periodic_report_req_msg_v01*  req_ptr,
  sns_smgr_periodic_report_resp_msg_v01* resp_ptr,
  sns_rh_rpt_spec_s*                     rpt_ptr)
{
  uint8_t i, ackNak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  /* ---------------------------------------------------------------------- */

  SNS_SMGR_PRINTF1(HIGH, "parse_periodic_report - rate=%u",  req_ptr->ReportRate);

  sns_rh_sol_parse_notify_suspend(
    hdr_ptr->src_module, req_ptr->notify_suspend_valid, 
    &req_ptr->notify_suspend, rpt_ptr);

  rpt_ptr->q16_rpt_rate = 
    sns_smgr_rate_hz_to_q16(req_ptr->ReportRate ? 
                            req_ptr->ReportRate : SNS_SMGR_REPORT_RATE_DEFAULT_V01);

  for (i=0; i<req_ptr->Item_len && ackNak==SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 &&
            rpt_ptr->num_items < ARR_SIZE(rpt_ptr->item_list); i++)
  {
    sns_rh_rpt_item_s* item_ptr = SMGR_ANY_ALLOC_STRUCT(sns_rh_rpt_item_s);
    if ( NULL != item_ptr )
    {
      bool success = sns_rh_parse_periodic_report_item(
        i, req_ptr, item_ptr, &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
      if ( !success )
      {
        SNS_SMGR_PRINTF1(ERROR, "parse_periodic_report - deleting item %u", i);
        SNS_OS_ANY_FREE(item_ptr);
      }
      else
      {
        item_ptr->parent_report_ptr = rpt_ptr;
        rpt_ptr->item_list[rpt_ptr->num_items++] = item_ptr;
      }
    }
    else
    {
      ackNak = SNS_SMGR_RESPONSE_NAK_RESOURCES_V01;
      SNS_SMGR_PRINTF1(ERROR, "parse_periodic_report - no resource for rpt_id=%d",
                       rpt_ptr->rpt_id);
    }
  }

  if ( resp_ptr->ReasonPair_len > 0 )
  {
    sns_rh_sol_override_response_ack(
      &resp_ptr->AckNak, SNS_SMGR_RESPONSE_ACK_MODIFIED_V01);
  }


  if ( sns_rh_sol_check_unaccompanied_temp_request(rpt_ptr) )
  {
    ackNak = SNS_SMGR_RESPONSE_NAK_INTERNAL_ERR_V01;
  }

  if ( (ackNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) || 
       (ackNak == SNS_SMGR_RESPONSE_ACK_MODIFIED_V01) )
  {
    if ( rpt_ptr->num_items > 0 )
    {
      sns_q_link(rpt_ptr, &rpt_ptr->rpt_link);
      rpt_ptr->msg_header = *hdr_ptr;
      rpt_ptr->rpt_id = req_ptr->ReportId;

      if ( req_ptr->ReportRate == 0 )
      {
        req_ptr->ReportRate = SNS_SMGR_REPORT_RATE_DEFAULT_V01;
        sns_rh_sol_add_reason_code(0, SNS_SMGR_REASON_DEFAULT_RATE_V01,
                                   &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
        sns_rh_sol_override_response_ack(&resp_ptr->AckNak,
                                         SNS_SMGR_RESPONSE_ACK_MODIFIED_V01);
      }
      sns_rh_update_event_sensor_only_flag(rpt_ptr);
    }
    else
    {
      ackNak = SNS_SMGR_RESPONSE_NAK_NO_ITEMS_V01;
    }
  }

  SNS_SMGR_PRINTF3(LOW, "parse_periodic_report - rpt_id=%d #items=%d ackNak=%u",
                   rpt_ptr->rpt_id, rpt_ptr->num_items, ackNak);
  return ackNak;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_parse_buffering_report_request

===========================================================================*/
/*!
  @brief Parses the incoming periodic or buffering report request

  @param[i]  req_ptr  - the incoming report request
  @param[io] resp_ptr - response to the request
  @param[io] rpt_ptr  - report specification

  @return
    SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 - if all goes well
    SNS_SMGR_RESPONSE_NAK_RESOURCES_V01 - out of mem
    SNS_SMGR_RESPONSE_NAK_NO_ITEMS_V01 - no item saved
*/
/*=========================================================================*/
SMGR_STATIC uint8_t sns_rh_sol_parse_buffering_report_request(
  const sns_rh_mr_header_s*        hdr_ptr,
  sns_smgr_buffering_req_msg_v01*  req_ptr,
  sns_smgr_buffering_resp_msg_v01* resp_ptr,
  sns_rh_rpt_spec_s*               rpt_ptr)
{
  uint8_t i, ackNak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  /* ---------------------------------------------------------------------- */

  SNS_SMGR_PRINTF1(HIGH, "parse_buffering_report - rate=0x%x",  req_ptr->ReportRate);
  
  sns_rh_sol_parse_notify_suspend(
    hdr_ptr->src_module, req_ptr->notify_suspend_valid, 
    &req_ptr->notify_suspend, rpt_ptr);
  rpt_ptr->q16_rpt_rate = req_ptr->ReportRate;
  rpt_ptr->rpt_interval = sns_smgr_get_sched_intval(rpt_ptr->q16_rpt_rate);

  for (i=0; i<req_ptr->Item_len && ackNak==SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 &&
            rpt_ptr->num_items < ARR_SIZE(rpt_ptr->item_list); i++)
  {
    sns_rh_rpt_item_s* item_ptr = SMGR_ANY_ALLOC_STRUCT(sns_rh_rpt_item_s);
    if ( NULL != item_ptr )
    {
      bool success = sns_rh_sol_parse_buffering_item(
        i, &req_ptr->Item[i], item_ptr, 
        &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
      if ( !success )
      {
        SNS_SMGR_PRINTF1(ERROR, "parse_buffering_report - deleting item %u", i);
        SNS_OS_ANY_FREE(item_ptr);
      }
      else
      {
        item_ptr->parent_report_ptr = rpt_ptr;
        rpt_ptr->item_list[rpt_ptr->num_items++] = item_ptr;
      }
    }
    else
    {
      ackNak = SNS_SMGR_RESPONSE_NAK_RESOURCES_V01;
      SNS_SMGR_PRINTF1(ERROR, "parse_buffering_report - no resource for rpt_id=%d",
                       rpt_ptr->rpt_id);
    }
  }

  if ( resp_ptr->ReasonPair_len > 0 )
  {
    sns_rh_sol_override_response_ack(
      &resp_ptr->AckNak, SNS_SMGR_RESPONSE_ACK_MODIFIED_V01);
  }

  if ( sns_rh_sol_check_unaccompanied_temp_request(rpt_ptr) )
  {
    ackNak = SNS_SMGR_RESPONSE_NAK_INTERNAL_ERR_V01;
  }

  if ( (ackNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) || 
       (ackNak == SNS_SMGR_RESPONSE_ACK_MODIFIED_V01) )
  {
    if ( rpt_ptr->num_items > 0 )
    {
      sns_q_link(rpt_ptr, &rpt_ptr->rpt_link);
      rpt_ptr->msg_header = *hdr_ptr;
      rpt_ptr->rpt_id = req_ptr->ReportId;
      sns_rh_update_event_sensor_only_flag(rpt_ptr);
      sns_rh_update_periodic_like_flag(rpt_ptr);
      SNS_SMGR_PRINTF3(
        LOW, "parse_buffering_report - rpt_rate=0x%X rpt_intvl=%u per_like=%u",
        rpt_ptr->q16_rpt_rate, rpt_ptr->rpt_interval, rpt_ptr->periodic_like);
    }
    else
    {
      ackNak = SNS_SMGR_RESPONSE_NAK_NO_ITEMS_V01;
    }
  }
  SNS_SMGR_PRINTF3(LOW, "parse_buffering_report - rpt_id=%d #items=%d ackNak=%u",
                   rpt_ptr->rpt_id, rpt_ptr->num_items, ackNak);
  return ackNak;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_validate_buffering_rate

===========================================================================*/
/*!
  @brief Validates report rate and sampling rates.

  @param[i] req_ptr - points to the structure of incoming request message

  @return
   SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 - rates in proper ranges
   SNS_SMGR_RESPONSE_NAK_REPORT_RATE_V01 - see the code
*/
/*=========================================================================*/
SMGR_STATIC uint8_t sns_rh_sol_validate_buffering_rate(
  const sns_smgr_buffering_req_msg_v01*  req_ptr)
{
  uint8_t ackNak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  q16_t min_sampling_rate = FX_MAX_Q16;
  q16_t max_sampling_rate = 1; //~0.0000153 Hz
  q16_t item_sampling_rate;
  uint8_t i;

  /* ---------------------------------------------------------------------- */

  for ( i=0; i<req_ptr->Item_len && i<ARR_SIZE(req_ptr->Item); i++ )
  {
    item_sampling_rate = sns_smgr_rate_hz_to_q16(req_ptr->Item[i].SamplingRate);
    min_sampling_rate = MIN(min_sampling_rate, item_sampling_rate);
    max_sampling_rate = MAX(max_sampling_rate, item_sampling_rate);
  }

  if ( req_ptr->ReportRate > max_sampling_rate )
  {
    ackNak = SNS_SMGR_RESPONSE_NAK_REPORT_RATE_V01;
    SNS_SMGR_PRINTF3(
      ERROR, "validate_buffering_rate - rpt_rate=0x%x sampling(min/max)=0x%x/0x%x",
      req_ptr->ReportRate, min_sampling_rate, max_sampling_rate);
  }
  return ackNak;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_compare_reports

===========================================================================*/
/*!
  @brief Checks if the given reports are identical.

  @param[i] existing_rpt_ptr
  @param[i] new_rpt_ptr

  @return   true if the reports are identical
*/
/*=========================================================================*/
SMGR_STATIC bool sns_rh_sol_compare_reports(
  const sns_rh_rpt_spec_s*  existing_rpt_ptr,
  const sns_rh_rpt_spec_s*  new_rpt_ptr)
{
  bool identical = true;
  uint8_t i;
  /* ---------------------------------------------------------------------- */

  if ( existing_rpt_ptr->num_items != new_rpt_ptr->num_items ||
       existing_rpt_ptr->q16_rpt_rate != new_rpt_ptr->q16_rpt_rate )
  {
    identical = false;
  }

  if ( identical )
  {
    RH_FOR_EACH_REPORT_ITEM( existing_rpt_ptr, i )
    {
      sns_rh_rpt_item_s* new_item_ptr = new_rpt_ptr->item_list[i];
      sns_rh_rpt_item_s* old_item_ptr = existing_rpt_ptr->item_list[i];

      if ( (old_item_ptr->ddf_sensor_ptr != new_item_ptr->ddf_sensor_ptr) ||
           (old_item_ptr->sampling_rate_hz != new_item_ptr->sampling_rate_hz) || 
           (old_item_ptr->flags != new_item_ptr->flags) || 
           (old_item_ptr->cal_sel != new_item_ptr->cal_sel))
      {
        identical = false;
        break;
      }
    }
  }

  return identical;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_update_pending_report

===========================================================================*/
/*!
  @brief Updates pending report so it won't send samples already sent

  @param[i] existing_rpt_ptr
  @param[i] pending_rpt_ptr

  @return   none
*/
/*=========================================================================*/
SMGR_STATIC void sns_rh_sol_update_pending_report(
  sns_rh_rpt_spec_s* existing_rpt_ptr,
  sns_rh_rpt_spec_s* pending_rpt_ptr)
{
  uint32_t i, j;
  for ( i=0; i<pending_rpt_ptr->num_items; i++ )
  {
    for ( j=0; j<existing_rpt_ptr->num_items; j++ )
    {
      if ( pending_rpt_ptr->item_list[i]->ddf_sensor_ptr == 
           existing_rpt_ptr->item_list[j]->ddf_sensor_ptr &&
           existing_rpt_ptr->item_list[j]->samples_sent )
      {
        pending_rpt_ptr->item_list[i]->ts_last_sent = 
          existing_rpt_ptr->item_list[j]->ts_last_sent;
        pending_rpt_ptr->item_list[i]->last_processed_sample_timestamp =
          existing_rpt_ptr->item_list[j]->last_processed_sample_timestamp;
        SNS_SMGR_PRINTF3(
          LOW, "update_pending_report - sensor=%u last_sent=%u last_proc=%u",
          SMGR_SENSOR_ID(pending_rpt_ptr->item_list[i]->ddf_sensor_ptr->sensor_ptr),
          pending_rpt_ptr->item_list[i]->ts_last_sent,
          pending_rpt_ptr->item_list[i]->last_processed_sample_timestamp);
        break;
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_schedule_existing_report

===========================================================================*/
/*!
  @brief   Schedules flush, deletes/attaches new reports if those already exist.
 
  @details Report being replaced with identical report is a Flush command.
           Existing report will be put in either FLUSH_PENDING or FLUSHING state
           and its extended state will be DELETE_PENDING.  Once all samples are
           flushed the pending report will be deleted.
 
           Report being replaced with new paramters is a combo of Delete old
           and Add new.  Delete before Add can lead to sensor being turned off
           then immediately turned back on.  To avoid this expensive and unnecessary
           toggle Add will be done before Delete.

  @param [i/o] new_rpt_ptr      - the new report to schedule
  @param [i/o] existing_rpt_ptr - the existing report to replace or schedule
  @param [o]   pending_resp     - the resp to schedule for later

  @return the new report to be activated immediately, or NULL if pending of Flush
*/
/*=========================================================================*/
SMGR_STATIC sns_rh_rpt_spec_s* sns_rh_sol_schedule_existing_report(
  sns_rh_rpt_spec_s*  new_rpt_ptr,
  sns_rh_rpt_spec_s*  existing_rpt_ptr,
  bool*               pending_resp_ptr )
{
  bool identical = sns_rh_sol_compare_reports(existing_rpt_ptr, new_rpt_ptr);
  sns_rh_rpt_spec_s* rpt_pending = existing_rpt_ptr->pending_rpt_ptr;
  sns_q_s* q_ptr = SMGR_QUE_HEADER_FROM_LINK(&existing_rpt_ptr->rpt_link);
  sns_q_s* suspended_q_ptr = sns_rh_get_suspended_report_queue();
  bool existing_rpt_suspended = (q_ptr == suspended_q_ptr);
  /* ---------------------------------------------------------------------- */

  SNS_SMGR_PRINTF4(
    MED, "sched_existing_report - state=0x%04x suspended=%u identical=%u pending_rpt=0x%x",
    existing_rpt_ptr->state << 8 | existing_rpt_ptr->state_extended,
    existing_rpt_suspended, identical, rpt_pending);

  if ( rpt_pending != NULL )
  {
    /* Can't buffer more rpts waiting for activation */
    if ( SNS_RH_REPORT_IS_PERIODIC(rpt_pending) )
    {
      ((sns_smgr_periodic_report_resp_msg_v01*)rpt_pending->resp_ptr)->AckNak =
        SNS_SMGR_RESPONSE_NAK_CANCELLED_V01;
    }
    else
    {
      ((sns_smgr_buffering_resp_msg_v01*) rpt_pending->resp_ptr)->AckNak = 
        SNS_SMGR_RESPONSE_NAK_CANCELLED_V01;
    }
    rpt_pending->resp_ptr->sns_result_t = SNS_RESULT_FAILURE_V01;
    rpt_pending->resp_ptr->sns_err_t    = SENSOR1_EFAILED;
    sns_rh_mr_send_resp(&rpt_pending->msg_header, rpt_pending->resp_ptr);
    SNS_SMGR_PRINTF0(LOW, "sched_existing_report - deleting existing pending report");
    sns_rh_sol_delete_report(rpt_pending);

    existing_rpt_ptr->pending_rpt_ptr = NULL;
    existing_rpt_ptr->state_extended  = RH_RPT_STATE_EXT_NONE;
    existing_rpt_ptr->state           = RH_RPT_STATE_ACTIVE;
  }

  if ( !existing_rpt_suspended )
  {
    existing_rpt_ptr->pending_rpt_ptr = new_rpt_ptr;
    new_rpt_ptr       = NULL;  /* do not activate new rpt here */
    *pending_resp_ptr = true;  /* do not send resp now */

    if ( !identical )
    {
      /* Will activate new first, then delete old */
      existing_rpt_ptr->state_extended = RH_RPT_STATE_EXT_ACTIVATE_PENDING;
    }
    else
    {
      /* This is the legacy Flush command */
      sns_rh_prepare_report_for_flushing(existing_rpt_ptr); 
      existing_rpt_ptr->state_extended = RH_RPT_STATE_EXT_DELETE_PENDING;
      sns_rh_flush_report(existing_rpt_ptr);
    }
  }
  else
  {
    /* Existing report is currently suspended and can be safely replaced  
       in its holding queue. */
    sns_rh_sol_update_pending_report(existing_rpt_ptr, new_rpt_ptr);
    sns_q_delete(&existing_rpt_ptr->rpt_link);
    sns_rh_sol_delete_report(existing_rpt_ptr);

    new_rpt_ptr->holding_q = suspended_q_ptr;
    sns_q_put(new_rpt_ptr->holding_q, &new_rpt_ptr->rpt_link);
    new_rpt_ptr = NULL;
  }

  return new_rpt_ptr;
}

/*===========================================================================

  FUNCTION:  sns_rh_sol_process_flush_command

===========================================================================*/
/*!
  @brief Flushes samples for the given report if found

  @param[i] hdr_ptr - Point to header structure for the incoming request
  @param[i] req_ptr - points to the structure of incoming request message
  @param[io] resp_ptr_ptr - address of response message pointer

  @return
    SNS_SMGR_RESPONSE_ACK_SUCCESS_V01   - request accepted as is
    SNS_SMGR_RESPONSE_ACK_MODIFIED_V01  - request accepted with modification
    any other ACK code                  - request rejected
 */
/*=========================================================================*/
SMGR_STATIC uint8_t sns_rh_sol_process_flush_command(
  const sns_rh_mr_header_s*         hdr_ptr,
  sns_smgr_buffering_req_msg_v01*   req_ptr,
  sns_smgr_buffering_resp_msg_v01** resp_ptr_ptr)
{
  uint8_t            ack_nak = SNS_SMGR_RESPONSE_NAK_REPORT_ID_V01;
  sns_q_s*           rpt_queue_ptr = sns_rh_get_report_queue();
  sns_rh_rpt_spec_s* rpt_ptr = sns_rh_sol_find_report(hdr_ptr->connection_handle,
                                                      req_ptr->ReportId);
  sns_smgr_buffering_resp_msg_v01* resp_ptr = *resp_ptr_ptr;
  /* ---------------------------------------------------------------------- */

  if ( rpt_ptr != NULL )
  {
    sns_q_s* q_ptr = SMGR_QUE_HEADER_FROM_LINK(&rpt_ptr->rpt_link);
    SNS_SMGR_PRINTF4(
      MED, "flush_cmd - rpt_id=%u/0x%x state=0x%02x resp_ptr=0x%x",
      rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle, 
      (rpt_ptr->state<<4)|rpt_ptr->state_extended, resp_ptr);

    if ( q_ptr == rpt_queue_ptr && rpt_ptr->rpt_tick != 0 )
    {
      sns_rh_rpt_spec_s* pending_rpt_ptr = rpt_ptr->pending_rpt_ptr;
      if ( pending_rpt_ptr != NULL )
      {
        ((sns_smgr_buffering_resp_msg_v01*)pending_rpt_ptr->resp_ptr)->AckNak = 
          SNS_SMGR_RESPONSE_NAK_CANCELLED_V01;
        pending_rpt_ptr->resp_ptr->sns_result_t = SNS_RESULT_FAILURE_V01;
        pending_rpt_ptr->resp_ptr->sns_err_t    = SENSOR1_EFAILED;
        sns_rh_mr_send_resp(&pending_rpt_ptr->msg_header, pending_rpt_ptr->resp_ptr);
        SNS_SMGR_PRINTF0(ERROR, "flush_cmd - cancel existing pending cmd");
        sns_rh_sol_delete_report(pending_rpt_ptr);
        rpt_ptr->pending_rpt_ptr = NULL;
      }

      if ( rpt_ptr->state_extended == RH_RPT_STATE_EXT_NONE )
      {
        sns_rh_prepare_report_for_flushing(rpt_ptr); 
      }
      if ( rpt_ptr->state == RH_RPT_STATE_FLUSHING )
      {
        sns_rh_flush_report(rpt_ptr);
      }
      else
      {
        rpt_ptr->pending_rpt_ptr = SMGR_ANY_ALLOC_STRUCT(sns_rh_rpt_spec_s);
        if ( rpt_ptr->pending_rpt_ptr != NULL )
        {
          SNS_OS_MEMZERO(rpt_ptr->pending_rpt_ptr, sizeof(sns_rh_rpt_spec_s));
          rpt_ptr->pending_rpt_ptr->msg_header = *hdr_ptr;
          rpt_ptr->pending_rpt_ptr->resp_ptr = (sns_common_resp_s_v01*) resp_ptr;
          rpt_ptr->pending_rpt_ptr->rpt_id   = rpt_ptr->rpt_id;
          rpt_ptr->state_extended = RH_RPT_STATE_EXT_DELETE_PENDING;
          *resp_ptr_ptr = NULL;
        }
      }
    }
    ack_nak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  }

  if ( ack_nak != SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
  {
    sns_rh_sol_override_response_ack(&resp_ptr->AckNak, ack_nak);
  }

  return ack_nak;
}

/*===========================================================================

  FUNCTION:   sns_rh_tally_report_rates_for_proc

===========================================================================*/
/*!
  @details Tallies up the max and sum report rates for the given processor type

  @param[i]  proc_type - the processor for which to tally up the report rates
  @param[o]  max_rate_ptr - destination of the max report rate
  @param[o]  sum_rate_ptr - destination of the sum of the report rates

  @return  none
 */
/*=========================================================================*/
SMGR_STATIC void sns_rh_tally_report_rates_for_proc(
  sns_proc_type_e_v01 proc_type,
  q16_t* restrict max_rate_ptr,
  q16_t* restrict sum_rate_ptr)
{
  sns_rh_rpt_spec_s* rpt_ptr;
  sns_q_s*           rpt_queue_ptr = sns_rh_get_report_queue();

  *max_rate_ptr = 0;
  *sum_rate_ptr = 0;

  SMGR_FOR_EACH_Q_ITEM( rpt_queue_ptr, rpt_ptr, rpt_link )
  {
    if ( rpt_ptr->proc_type == proc_type )
    {
      q16_t rpt_rate = rpt_ptr->q16_rpt_rate;

      /* Effective report rate for a Query report is its highest requested sampling rate */
      if ( rpt_ptr->q16_rpt_rate == 0 )
      {
        uint8_t i;
        RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
        {
          rpt_rate = MAX(rpt_rate, rpt_ptr->item_list[i]->sampling_rate_hz);
        }
      }

      *max_rate_ptr = MAX(*max_rate_ptr, rpt_rate);
      *sum_rate_ptr += rpt_rate;
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_activate_report

===========================================================================*/
/*!
  @brief Activates a report by updating its requested sensors

  @param[i] rpt_ptr - the report to be activated

  @return None
*/
/*=========================================================================*/
SMGR_STATIC void sns_rh_sol_activate_report(sns_rh_rpt_spec_s* rpt_ptr)
{
  uint8_t  i;
  q16_t max_rpt_rate, sum_rpt_rate;
  sns_q_s* rpt_queue_ptr = sns_rh_get_report_queue();

  sns_q_put(rpt_queue_ptr, &rpt_ptr->rpt_link);

  sns_rh_tally_report_rates_for_proc(rpt_ptr->proc_type, &max_rpt_rate, &sum_rpt_rate);
  sns_smgr_update_eff_rept_rate_est(max_rpt_rate, sum_rpt_rate, rpt_ptr->proc_type);

  SNS_SMGR_PRINTF4(
    HIGH, "activate_report - rpt_id=%u/0x%x type=%u/0x%x", 
    rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle, 
    rpt_ptr->msg_header.svc_num, rpt_ptr->msg_header.msg_id);
  SNS_SMGR_PRINTF4(
    HIGH, "activate_report - addr=0x%x proc=%u state=%u #reports=%u", 
    rpt_ptr, rpt_ptr->proc_type, rpt_ptr->state, sns_q_cnt(rpt_queue_ptr));

  RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
  {
    sns_rh_sol_link_item_to_sensor(rpt_ptr->item_list[i]);
  }

  sns_smgr_update_avgitems();
  sns_smgr_update_bw_reqd();
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_schedule_report

===========================================================================*/
/*!
  @brief Schedules a report.

  @param[i] report_ptr - the new report to schedule

  @return
   bool pending_resp
*/
/*=========================================================================*/
bool sns_rh_sol_schedule_report(
  const sns_rh_mr_header_s* hdr_ptr,
  sns_rh_rpt_spec_s*        report_ptr)
{
  bool pending_resp = false;
  sns_rh_rpt_spec_s* existing_report_ptr =
    sns_rh_sol_find_report(hdr_ptr->connection_handle, report_ptr->rpt_id);
  /* ---------------------------------------------------------------------- */

  if ( existing_report_ptr != NULL )
  {
    report_ptr =
      sns_rh_sol_schedule_existing_report(report_ptr, existing_report_ptr, &pending_resp);
  }
  else
  {
    sns_rh_mr_update_connection(report_ptr->msg_header.connection_handle, 
                                report_ptr->proc_type);
  }

  if ( report_ptr != NULL )
  {
    sns_rh_sol_activate_report(report_ptr);
  }
  else if ( existing_report_ptr != NULL && 
            existing_report_ptr->state == RH_RPT_STATE_ACTIVE &&
            existing_report_ptr->state_extended != RH_RPT_STATE_EXT_NONE )
  {
    sns_rh_sol_handle_pending_rpt(existing_report_ptr);
  }
  sns_smgr_signal_me(SNS_SMGR_SENSOR_STATE_SIG);

  return pending_resp;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_schedule_buffering_report

===========================================================================*/
/*!
  @brief Insert a buffering report to a schedule block.

  @param[i] hdr_ptr - Point to header structure for the incoming request
  @param[i] req_ptr - points to the structure of incoming request message
  @param[io] resp_ptr_ptr - address of response message pointer

  @return
    SNS_SMGR_RESPONSE_ACK_SUCCESS_V01   - request accepted as is
    SNS_SMGR_RESPONSE_ACK_MODIFIED_V01  - request accepted with modification
    any other ACK code                  - request rejected
*/
/*=========================================================================*/
SMGR_STATIC uint8_t sns_rh_sol_schedule_buffering_report(
  const sns_rh_mr_header_s*         hdr_ptr,
  sns_smgr_buffering_req_msg_v01*   req_ptr,
  sns_smgr_buffering_resp_msg_v01** resp_ptr_ptr)
{
  uint8_t            ack_nak       = SNS_SMGR_RESPONSE_NAK_RESOURCES_V01;
  sns_rh_rpt_spec_s* rpt_ptr       = NULL;
  sns_q_s*           rpt_queue_ptr = sns_rh_get_report_queue();
  sns_smgr_buffering_resp_msg_v01* resp_ptr = *resp_ptr_ptr;
  /* -------------------------------------------------------------------- */

  if ( SNS_SMGR_MAX_REPORT_CNT > sns_q_cnt(rpt_queue_ptr) )
  {
    rpt_ptr = SMGR_ANY_ALLOC_STRUCT(sns_rh_rpt_spec_s);
  }
  else
  {
    sns_rh_sol_override_response_ack(&resp_ptr->AckNak,
                                     SNS_SMGR_RESPONSE_NAK_RESOURCES_V01);
    SNS_SMGR_PRINTF0(ERROR, "schedule_buffering_report - max# of reports present");
  }

  if ( NULL == rpt_ptr )
  {
    if ( resp_ptr->AckNak != SNS_SMGR_RESPONSE_NAK_RESOURCES_V01 )
    {
      sns_rh_sol_override_response_ack(&resp_ptr->AckNak,
                                       SNS_SMGR_RESPONSE_NAK_RESOURCES_V01);
      SNS_SMGR_PRINTF1(ERROR, "schedule_buffering_rpt - alloc %u bytes failed",
                       sizeof(sns_rh_rpt_spec_s));
    }
  }
  else
  {
    ack_nak = sns_rh_sol_validate_buffering_rate(req_ptr);
    if ( ack_nak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
    {
      ack_nak = 
        sns_rh_sol_parse_buffering_report_request(hdr_ptr, req_ptr, resp_ptr, rpt_ptr);
    }
    if ( ack_nak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 ||
         ack_nak == SNS_SMGR_RESPONSE_ACK_MODIFIED_V01 )
    {
      /* Store resp for later send */
      rpt_ptr->resp_ptr = (sns_common_resp_s_v01*) resp_ptr;
      if ( sns_rh_sol_schedule_report(hdr_ptr, rpt_ptr) )
      {
        *resp_ptr_ptr = NULL; /* response already handled */
      }
      else
      {
        rpt_ptr->resp_ptr = NULL;
      }
    }
    else
    {
      sns_rh_sol_override_response_ack(&resp_ptr->AckNak, ack_nak);
      sns_rh_sol_delete_report(rpt_ptr);
    }
  }

  return ack_nak;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_delete_report

===========================================================================*/
/*!
  @brief Remove report block and item blocks.

  @param[i] rpt_ptr: the report to be deleted

  @return
   None
*/
/*=========================================================================*/
void sns_rh_sol_delete_report(sns_rh_rpt_spec_s* rpt_ptr)
{
  sns_smgr_ddf_sensor_s* ddf_sensor_ptrs[SNS_SMGR_MAX_ITEMS_PER_REPORT_V01];
  SNS_OS_MEMZERO(ddf_sensor_ptrs, sizeof(ddf_sensor_ptrs));

  rpt_ptr->holding_q = NULL;
  rpt_ptr->state = RH_RPT_STATE_DELETING;

  sns_rh_sol_get_ddf_sensor_list(rpt_ptr, ddf_sensor_ptrs);
  sns_rh_sol_deactivate_report(rpt_ptr);
  sns_rh_sol_update_sensor_events(ddf_sensor_ptrs);
}
/*===========================================================================

  FUNCTION:   sns_rh_sol_delete_batchbuff

===========================================================================*/
/*!
  @brief Delete and free batchbuffers allocated for a report item
 
  @details

  @param[i] item_ptr - the report item from which batchbuffers will be freed

  @return None
*/
/*=========================================================================*/
SMGR_STATIC void sns_rh_sol_delete_batchbuff(sns_rh_rpt_item_s* item_ptr)
{
  SNS_SMGR_PRINTF4(
    HIGH, "delete_batchbuff - rpt_id=%u/0x%x #bbb/#ubb=0x%08x #samples_saved=%u",
    item_ptr->parent_report_ptr->rpt_id,
    item_ptr->parent_report_ptr->msg_header.connection_handle,
    sns_q_cnt(&item_ptr->ubatch_buffer_queue) |
    sns_q_cnt(&item_ptr->bbatch_buffer_queue) << 8, item_ptr->num_saved_samples);
     
  while ( sns_q_cnt(&item_ptr->ubatch_buffer_queue) > 0 )
  {
    sns_batchbuff_free(SNS_BATCHBUFF_CLIENT_SMGR, sns_q_get(&item_ptr->ubatch_buffer_queue));
  }
  while ( sns_q_cnt(&item_ptr->bbatch_buffer_queue) > 0 )
  {
    sns_batchbuff_free(SNS_BATCHBUFF_CLIENT_SMGR, sns_q_get(&item_ptr->bbatch_buffer_queue));
  }
  item_ptr->parent_report_ptr->num_saved_samples -= item_ptr->num_saved_samples;
  item_ptr->num_saved_samples = 0;
  item_ptr->ts_last_saved = 0;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_deactivate_report

===========================================================================*/
/*!
  @brief Deactivates a report
 
  @details
  Disassociates the report from its requested sensors, removes it from
  the report queue, and either puts it in its holding queue or frees its
  resources.

  @param[i] rpt_ptr - the report to be deactivated

  @return None
*/
/*=========================================================================*/
void sns_rh_sol_deactivate_report(sns_rh_rpt_spec_s* rpt_ptr)
{
  uint8_t  i;
  q16_t max_rpt_rate, sum_rpt_rate;
  sns_q_s* q_ptr = SMGR_QUE_HEADER_FROM_LINK(&rpt_ptr->rpt_link);
  /* -------------------------------------------------------------------- */

  SNS_SMGR_PRINTF4(
    HIGH, "deactivate_report - rpt_id=%u/0x%x type=%u/0x%x", 
    rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle, 
    rpt_ptr->msg_header.svc_num, rpt_ptr->msg_header.msg_id);
  SNS_SMGR_PRINTF4(
    HIGH, "deactivate_report - addr=0x%x state=%u #inds=%u queued=%u", 
    rpt_ptr, rpt_ptr->state, rpt_ptr->ind_count, q_ptr ? true : false);

  sns_rh_rpt_gen_remove_from_ready_list(rpt_ptr);

  RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
  {
    sns_rh_rpt_item_s* item_ptr = rpt_ptr->item_list[i];
    uint8_t num_axes = item_ptr->ddf_sensor_ptr->uimg_depot_ptr == NULL?
      item_ptr->ddf_sensor_ptr->bimg_depot_ptr->num_axes:
      item_ptr->ddf_sensor_ptr->uimg_depot_ptr->num_axes;
      
    while ( item_ptr->query_ptr != NULL )
    {
      sns_rh_query_s* query_ptr = item_ptr->query_ptr;
      item_ptr->query_ptr = query_ptr->next_query_ptr;
      sns_rh_rpt_gen_send_empty_query_indication(query_ptr);
      SNS_OS_ANY_FREE(query_ptr);
    }

    if ( NULL != q_ptr )
    {
      sns_rh_sol_unlink_item_to_sensor(item_ptr);
      sns_rh_sol_delete_batchbuff(item_ptr);
      item_ptr->samples_sent = false;     
    }

    if ( rpt_ptr->holding_q == NULL )
    {
      SNS_OS_ANY_FREE(item_ptr);
    }
    else if ( rpt_ptr->holding_q != sns_rh_get_suspended_report_queue() )
    {
      sns_rh_cic_obj_s* cic_obj_ptr = &item_ptr->cic_obj;
      SNS_OS_MEMZERO(&item_ptr->interpolator_obj, sizeof(sns_rh_interpolator_obj_s));
      sns_rh_cic_init(cic_obj_ptr, cic_obj_ptr->factor, cic_obj_ptr->input_frequency,
                      num_axes);
      item_ptr->quality = SNS_SMGR_ITEM_QUALITY_CURRENT_SAMPLE_V01;
      item_ptr->effective_rate_hz = 0;
      item_ptr->last_processed_sample_timestamp = 0;
      item_ptr->ts_last_sent = 0;
    }
  }
  if ( NULL != q_ptr )
  {
    sns_q_delete(&rpt_ptr->rpt_link);
  }
  sns_rh_tally_report_rates_for_proc(rpt_ptr->proc_type, &max_rpt_rate, &sum_rpt_rate);
  sns_smgr_update_eff_rept_rate_est(max_rpt_rate, sum_rpt_rate, rpt_ptr->proc_type);

  sns_smgr_update_avgitems();
  sns_smgr_update_bw_reqd();

  if ( rpt_ptr->ind_buffer != NULL )
  {
    SNS_OS_ANY_FREE(rpt_ptr->ind_buffer);
    rpt_ptr->ind_buffer = NULL;
  }

  if ( rpt_ptr->holding_q != NULL )
  {
    rpt_ptr->state          = RH_RPT_STATE_ACTIVE;
    rpt_ptr->state_extended = RH_RPT_STATE_EXT_NONE;
    rpt_ptr->rpt_tick       = 0;
    rpt_ptr->ind_count      = 0;
    rpt_ptr->num_saved_samples = 0;
    sns_q_put(rpt_ptr->holding_q, &rpt_ptr->rpt_link);
  }
  else
  {
    SNS_OS_ANY_FREE(rpt_ptr);
  }
  if ( NULL != q_ptr )
  {
    sns_rh_update_report_timer();
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_schedule_periodic_report

===========================================================================*/
/*!
  @brief Schedules new Periodic report.

  @param[i]  hdr_ptr      - header for the incoming request
  @param[i]  req_ptr      - incoming request message
  @param[io] resp_ptr_ptr - address of response message pointer

  @return
    SNS_SMGR_RESPONSE_ACK_SUCCESS_V01   - request accepted as is
    SNS_SMGR_RESPONSE_ACK_MODIFIED_V01  - request accepted with modification
    any other ACK code                  - request rejected
*/
/*=========================================================================*/
SMGR_STATIC uint8_t sns_rh_sol_schedule_periodic_report(
  const sns_rh_mr_header_s*               hdr_ptr,
  sns_smgr_periodic_report_req_msg_v01*   req_ptr,
  sns_smgr_periodic_report_resp_msg_v01** resp_ptr_ptr)
{
  uint8_t            ack_nak       = SNS_SMGR_RESPONSE_NAK_RESOURCES_V01;
  sns_rh_rpt_spec_s* rpt_ptr       = NULL;
  sns_q_s*           rpt_queue_ptr = sns_rh_get_report_queue();
  sns_smgr_periodic_report_resp_msg_v01* resp_ptr = *resp_ptr_ptr;
  /* -------------------------------------------------------------------- */

  if ( SNS_SMGR_MAX_REPORT_CNT > sns_q_cnt(rpt_queue_ptr) )
  {
    rpt_ptr = SMGR_ANY_ALLOC_STRUCT(sns_rh_rpt_spec_s);
  }
  else
  {
    sns_rh_sol_override_response_ack(&resp_ptr->AckNak,
                                     SNS_SMGR_RESPONSE_NAK_RESOURCES_V01);
    SNS_SMGR_PRINTF0(ERROR, "schedule_periodic_report - max# of reports present");
  }

  if ( NULL == rpt_ptr )
  {
    if ( resp_ptr->AckNak != SNS_SMGR_RESPONSE_NAK_RESOURCES_V01 )
    {
      sns_rh_sol_override_response_ack(&resp_ptr->AckNak,
                                       SNS_SMGR_RESPONSE_NAK_RESOURCES_V01);
      SNS_SMGR_PRINTF1(ERROR, "schedule_periodic_report - alloc %u bytes failed",
                       sizeof(sns_rh_rpt_spec_s));
    }
  }
  else
  {
    ack_nak =
      sns_rh_sol_parse_periodic_report_request(hdr_ptr, req_ptr, resp_ptr, rpt_ptr);
    if ( ack_nak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
    {
      rpt_ptr->resp_ptr = (sns_common_resp_s_v01*)resp_ptr;
      if ( sns_rh_sol_schedule_report(hdr_ptr, rpt_ptr) )
      {
        *resp_ptr_ptr = NULL; /* response already handled */
      }
      else
      {
        rpt_ptr->resp_ptr = NULL;
      }
    }
    else
    {
      sns_rh_sol_override_response_ack(&resp_ptr->AckNak, ack_nak);
      sns_rh_sol_delete_report(rpt_ptr);
    }
  }
  return ack_nak;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_process_periodic_request

===========================================================================*/
/*!
  @brief Processes SNS_SMGR_REPORT_REQ_V01
 
  @details
    - Add, modify or delete a SOL entry consisting of the report spec structure
      plus subordinate item structures. Subordinate structures have
      dependencies on sensor sampling structures. When possible, connect to or
      disconnect from sensors without causing changes in the established
      sampling pattern. Set flags if sensor plan must be recalculated.
    - Send a response message for the request message providing ACK or NAK.
      Provide reasons for NAK or dropped items. Notify when defaults have been
      applied.

  @param[i] req_ptr  - incoming request message

  @return SNS_ERR_NOMEM or SNS_SUCCESS
*/
/*=========================================================================*/
sns_err_code_e sns_rh_sol_process_periodic_request(
  sns_rh_mr_req_q_item_s* msg_ptr)
{
  uint8_t ack_nak = SNS_SMGR_RESPONSE_NAK_UNK_ACTION_V01;
  sns_smgr_periodic_report_req_msg_v01*  req_ptr = &msg_ptr->request.periodic_report;
  sns_smgr_periodic_report_resp_msg_v01* resp_ptr =
    SMGR_ALLOC_STRUCT(sns_smgr_periodic_report_resp_msg_v01);
  /* ---------------------------------------------------------------------- */

  if ( resp_ptr == NULL )
  {
    return SNS_ERR_NOMEM;
  }

  sns_profiling_log_qdss(SNS_RH_RPT_REQ, 4, 
                         req_ptr->ReportId, msg_ptr->header.connection_handle, 
                         SNS_SMGR_REPORT_REQ_V01, req_ptr->Action );

  if ( req_ptr->SrcModule_valid )
  {
    msg_ptr->header.src_module = req_ptr->SrcModule;
  }

  SNS_SMGR_PRINTF4(
    MED, "periodic_req - rpt_id=%u/0x%x action=%u rate=%u",
    req_ptr->ReportId, msg_ptr->header.connection_handle, 
    req_ptr->Action, req_ptr->ReportRate);

  SNS_SMGR_PRINTF2(
    MED, "periodic_req - src_valid=%u src=%u", 
    req_ptr->SrcModule_valid, req_ptr->SrcModule);

  msg_ptr->header.msg_id   = SNS_SMGR_REPORT_RESP_V01;
  msg_ptr->header.body_len = sizeof(sns_smgr_periodic_report_resp_msg_v01);

  resp_ptr->ReportId = req_ptr->ReportId;
  resp_ptr->ReasonPair_len = 0;
  resp_ptr->AckNak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  resp_ptr->Resp.sns_err_t = SENSOR1_SUCCESS;

  switch( req_ptr->Action )
  {
    case SNS_SMGR_REPORT_ACTION_ADD_V01:
      ack_nak = sns_rh_sol_schedule_periodic_report(&msg_ptr->header, req_ptr, &resp_ptr);
      break;
    case SNS_SMGR_REPORT_ACTION_DELETE_V01:
      ack_nak = sns_rh_sol_find_and_delete_report(
                  msg_ptr->header.connection_handle, req_ptr->ReportId, false);
      sns_rh_sol_override_response_ack(&resp_ptr->AckNak, ack_nak);
      break;
    default:
      sns_rh_sol_override_response_ack(&resp_ptr->AckNak,
                                       SNS_SMGR_RESPONSE_NAK_UNK_ACTION_V01);
      break;
  }

  if ( (ack_nak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) ||
       (ack_nak == SNS_SMGR_RESPONSE_ACK_MODIFIED_V01) )
  {
    sns_rh_flush_reports();
    sns_smgr_signal_me(SNS_SMGR_SENSOR_STATE_SIG);
  }

  if ( resp_ptr != NULL )
  {
    if ( (resp_ptr->AckNak != SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) &&
         (resp_ptr->AckNak != SNS_SMGR_RESPONSE_ACK_MODIFIED_V01) )
    {
       resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
       resp_ptr->Resp.sns_err_t = SENSOR1_EFAILED;
    }

    sns_rh_mr_send_resp(&msg_ptr->header, resp_ptr);
  }

  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_process_buffering_request

===========================================================================*/
/*!
  @brief Processes SNS_SMGR_BUFFERING_REQ_V01
 
  @details
    - Add, modify or delete a SOL entry consisting of the report spec structure
      plus subordinate item structures. Subordinate structures have
      dependencies on sensor sampling structures. When possible, connect to or
      disconnect from sensors without causing changes in the established
      sampling pattern. Set flags if sensor plan must be recalculated.
    - Sends a response message for the request message providing ACK or NAK.
      Provides reasons for NAK or dropped items. Notifies when defaults have been
      applied.

  @param[i] req_ptr  - incoming request message

  @return SNS_ERR_NOMEM or SNS_SUCCESS
*/
/*=========================================================================*/
sns_err_code_e sns_rh_sol_process_buffering_request(
  sns_rh_mr_req_q_item_s*  msg_ptr)
{
  uint8_t ack_nak = SNS_SMGR_RESPONSE_NAK_UNK_ACTION_V01;
  sns_smgr_buffering_req_msg_v01*  req_ptr = &msg_ptr->request.buffering_report;
  sns_smgr_buffering_resp_msg_v01* resp_ptr = 
    SMGR_ALLOC_STRUCT(sns_smgr_buffering_resp_msg_v01);
  /* ---------------------------------------------------------------------- */

  if ( resp_ptr == NULL )
  {
    return SNS_ERR_NOMEM;
  }

  sns_profiling_log_qdss(SNS_RH_RPT_REQ, 4, 
                         req_ptr->ReportId, msg_ptr->header.connection_handle, 
                         SNS_SMGR_BUFFERING_REQ_V01, req_ptr->Action );

  SNS_SMGR_PRINTF4(
    MED, "buffering_req - rpt_id=%u/0x%x action=%d items=%d",
    req_ptr->ReportId, msg_ptr->header.connection_handle, 
    req_ptr->Action, req_ptr->Item_len);

  msg_ptr->header.msg_id   = SNS_SMGR_BUFFERING_RESP_V01;
  msg_ptr->header.body_len = sizeof(sns_smgr_buffering_resp_msg_v01);

  resp_ptr->ReportId          = req_ptr->ReportId;
  resp_ptr->ReportId_valid    = true;
  resp_ptr->ReasonPair_len    = 0;
  resp_ptr->AckNak            = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  resp_ptr->AckNak_valid      = true;
  resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  resp_ptr->Resp.sns_err_t    = SENSOR1_SUCCESS;

  SNS_SMGR_PRINTF3(MED, "buffering_req - src_valid=%d src=%d conn=0x%x",
                   req_ptr->SrcModule_valid, req_ptr->SrcModule,
                   msg_ptr->header.connection_handle);

  if ( req_ptr->SrcModule_valid )
  {
    msg_ptr->header.src_module = req_ptr->SrcModule;
  }

  switch( req_ptr->Action )
  {
    case SNS_SMGR_BUFFERING_ACTION_ADD_V01:
      ack_nak = sns_rh_sol_schedule_buffering_report(&msg_ptr->header, req_ptr, &resp_ptr);
      break;
    case SNS_SMGR_BUFFERING_ACTION_DELETE_V01:
      ack_nak = sns_rh_sol_find_and_delete_report(
                  msg_ptr->header.connection_handle, req_ptr->ReportId, false);
      sns_rh_sol_override_response_ack(&resp_ptr->AckNak, ack_nak);
      break;
    case SNS_SMGR_BUFFERING_ACTION_FLUSH_V01:
      ack_nak =
        sns_rh_sol_process_flush_command(&msg_ptr->header, req_ptr, &resp_ptr);
      break;
    default:
      /* Unknown action code */
      sns_rh_sol_override_response_ack(&resp_ptr->AckNak,
                                       SNS_SMGR_RESPONSE_NAK_UNK_ACTION_V01);
      break;
  }

  if ( ack_nak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 ||
       ack_nak == SNS_SMGR_RESPONSE_ACK_MODIFIED_V01 )
  {
    sns_rh_flush_reports();
    sns_smgr_signal_me(SNS_SMGR_SENSOR_STATE_SIG);
  }

  if ( resp_ptr != NULL )
  {
    if ( (resp_ptr->AckNak != SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) &&
         (resp_ptr->AckNak != SNS_SMGR_RESPONSE_ACK_MODIFIED_V01) )
    {
       resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
       resp_ptr->Resp.sns_err_t = SENSOR1_EFAILED;
    }
    sns_rh_mr_send_resp(&msg_ptr->header, resp_ptr);
  }

  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_process_buffering_query_request

===========================================================================*/
/*! 
  @brief Processes SNS_SMGR_BUFFERING_QUERY_REQ_V01
 
  @details
  Called from the input driver when a queued message has been identified
  as a Buffering Query request.  The same client is expected to have already
  sent Buffering request for the sensor specified in this Query request.

  @param[i] req_ptr  - incoming request message

  @return SNS_ERR_NOMEM or SNS_SUCCESS
*/
/*=========================================================================*/
sns_err_code_e sns_rh_sol_process_buffering_query_request(
  sns_rh_mr_req_q_item_s* msg_ptr)
{
  sns_smgr_buffering_query_req_msg_v01*  req_ptr = &msg_ptr->request.query_report;
  sns_smgr_buffering_query_resp_msg_v01* resp_ptr =
    SMGR_ALLOC_STRUCT(sns_smgr_buffering_query_resp_msg_v01);
  sns_ddf_time_t     time_now = sns_ddf_get_timestamp();
  sns_ddf_time_t     t0, t1;
  sns_rh_rpt_item_s* item_ptr = NULL;
  sns_rh_query_s**   qpp = NULL;
  sns_rh_query_s*    query_ptr = NULL;
  /* ---------------------------------------------------------------------- */

  if ( resp_ptr == NULL )
  {
    return SNS_ERR_NOMEM;
  }

  SNS_SMGR_PRINTF3(
    MED, "query_req - id=0x%x sensor=%d dtype=%d",
    req_ptr->QueryId, req_ptr->SensorId, req_ptr->DataType);
  SNS_SMGR_PRINTF4(
    MED, "query_req - T0=%u T1=%u now=%u now-T1=%d",
    req_ptr->TimePeriod[0], req_ptr->TimePeriod[1], time_now, 
    time_now-req_ptr->TimePeriod[1]);

  resp_ptr->QueryId_valid  = true;
  resp_ptr->QueryId        = req_ptr->QueryId;
  resp_ptr->AckNak_valid   = true;
  resp_ptr->AckNak         = 
    sns_rh_sol_find_query_item(&msg_ptr->header, req_ptr, &item_ptr);

  /* Verify that Query ID is unique and there are fewer than max outstanding queries */
  if ( (resp_ptr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) &&
       (item_ptr != NULL) )
  {
    uint8_t outstanding_queries = 0;
    for ( qpp=&item_ptr->query_ptr; *qpp!=NULL && (query_ptr=*qpp)!=FALSE;
          qpp=&(*qpp)->next_query_ptr )
    {
      if ( query_ptr->query_id == req_ptr->QueryId )
      {
        /* Query IDs must be unique; reject this request */
        resp_ptr->AckNak = SNS_SMGR_RESPONSE_NAK_QUERY_ID_V01;
        break;
      }
      outstanding_queries++;
    }
    if ( (resp_ptr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) &&
         (outstanding_queries >= SMGR_MAX_EIS_QUERY_REQUESTS) )
    {
      resp_ptr->AckNak = SNS_SMGR_REASON_OTHER_FAILURE_V01;
      SNS_SMGR_PRINTF0(ERROR, "query_req - max outstanding queries");
    }
  }

  /* T0 and T1 must get their upper bits from current system time */
  t0 = (req_ptr->TimePeriod[0] + sns_em_system_time_offset) | 
       ((uint64_t)time_now & 0xFFFFFF00000000);
  t1 = (req_ptr->TimePeriod[1] + sns_em_system_time_offset) | 
       ((uint64_t)time_now & 0xFFFFFF00000000);
  SNS_SMGR_PRINTF4(MED, "query_req - adjusted T0=%u/0x%x T1=%u/0x%x", t0, t0, t1, t1);

  /* Sanity check for T0 and T1 */
  if ( (resp_ptr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) && (t0 >= t1) )
  {
    resp_ptr->AckNak = SNS_SMGR_RESPONSE_NAK_TIME_PERIOD_V01;
    SNS_SMGR_PRINTF0(ERROR, "query_req - T1 < T0");
  }

  /* Verify that T0 and T1 meet the requirements */
  if ( (resp_ptr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 ) &&
       (item_ptr != NULL) && (item_ptr->depot_ptr != NULL) )
  {
    uint8_t num_samples = ((t1 - t0)/item_ptr->sampling_interval);
    uint32_t time_for_max_samples =
      item_ptr->sampling_interval * SMGR_MAX_EIS_QUERY_SAMPLES;
    sns_ddf_time_t lower_time_limit = time_now - time_for_max_samples;
    sns_ddf_time_t upper_time_limit = time_now + time_for_max_samples;

    if ( (num_samples > SMGR_MAX_EIS_QUERY_SAMPLES) ||
         (t0 > upper_time_limit) || (t1 < lower_time_limit) )
    {
      resp_ptr->AckNak = SNS_SMGR_RESPONSE_NAK_TIME_PERIOD_V01;
      SNS_SMGR_PRINTF3(
        ERROR, "query_req - samples=%d lower=%u upper=%u",
        num_samples, lower_time_limit, upper_time_limit);
    }
  }

  /* Everything checks out */
  if ( (resp_ptr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) &&
       (qpp != NULL) )
  {
    query_ptr = SMGR_ANY_ALLOC_STRUCT(sns_rh_query_s);
    if ( query_ptr != NULL )
    {
      SNS_OS_MEMZERO(query_ptr, sizeof(sns_rh_query_s));
      query_ptr->item_ptr = item_ptr;
      query_ptr->query_id = req_ptr->QueryId;
      query_ptr->T0       = t0;
      query_ptr->T1       = t1;
      query_ptr->msg_header = msg_ptr->header;
      *qpp = query_ptr;
    }
    else
    {
      resp_ptr->AckNak = SNS_SMGR_RESPONSE_NAK_RESOURCES_V01;
    }
  }

  /* See if data is available for the requested time period */
  if ( (resp_ptr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) &&
       (item_ptr != NULL) )
  {
    const sns_smgr_sample_s* oldest_sample_ptr =
      sns_smgr_depot_get_oldest_sample(item_ptr->depot_ptr);
    const sns_smgr_sample_s* latest_sample_ptr =
      sns_smgr_depot_get_latest_sample(item_ptr->depot_ptr);
    if ( (oldest_sample_ptr != NULL) && (latest_sample_ptr != NULL) )
    {
      SNS_SMGR_PRINTF2(
        LOW, "query_req - oldest=%u latest=%u",
        SMGR_SAMPLE_TIMESTAMP(oldest_sample_ptr), 
        SMGR_SAMPLE_TIMESTAMP(latest_sample_ptr));
    }
    if ( (latest_sample_ptr != NULL) &&
         (t1 < SMGR_SAMPLE_TIMESTAMP(latest_sample_ptr)) )
    {
      /* data available for the time period */
      sns_rh_add_to_ready_list(item_ptr->parent_report_ptr);
    }
    /* else, when T1 is reached, report will be generated */
  }

  if ( resp_ptr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
  {
    resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
    resp_ptr->Resp.sns_err_t    = SENSOR1_SUCCESS;
  }
  else
  {
    resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
    resp_ptr->Resp.sns_err_t    = SENSOR1_EFAILED;
  }
  SNS_SMGR_PRINTF1(HIGH, "query_req - ackNak=%d", resp_ptr->AckNak);

  msg_ptr->header.msg_id   = SNS_SMGR_BUFFERING_QUERY_RESP_V01;
  msg_ptr->header.body_len = sizeof(sns_smgr_buffering_query_resp_msg_v01);
  sns_rh_mr_send_resp(&msg_ptr->header, resp_ptr);

  sns_rh_send_ready_reports();
  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_rh_update_sensor_status

===========================================================================*/
/*!
  @brief Sends Sensor Status indication to all clients interested in the given
         sensor's status
 
  @details Called when some aspects of the given sensor have changed

  @param[i] ddf_sensor_ptr - the DDF sensor
 
  @return  none
 */
/*=========================================================================*/
void sns_rh_sol_update_sensor_status(sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  sns_q_s* ssr_q_ptr = &ddf_sensor_ptr->client_stat.sensor_status.request_queue;
  sns_rh_mr_req_q_item_s* req_q_item_ptr;
  /* ---------------------------------------------------------------------- */

  SMGR_FOR_EACH_Q_ITEM( ssr_q_ptr, req_q_item_ptr, q_link )
  {
    sns_rh_generate_sensor_status_ind(ddf_sensor_ptr, req_q_item_ptr);
  }
}
/*===========================================================================

  FUNCTION:   sns_rh_sol_compute_sample_ratio

===========================================================================*/
/*!
  @brief Computes the sample count ratio of each item in the given report

  @param[io] rpt_ptr   - the report

  @return None
*/
/*=========================================================================*/
void sns_rh_sol_compute_sample_ratio(sns_rh_rpt_spec_s* rpt_ptr)
{
  uint8_t  i;
  uint32_t samples_per_report = 0;
  RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
  {
    sns_rh_rpt_item_s* item_ptr = rpt_ptr->item_list[i];
    item_ptr->samples_per_report = item_ptr->effective_rate_hz / rpt_ptr->q16_rpt_rate;
    samples_per_report += item_ptr->samples_per_report;
  }
  RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
  {
    sns_rh_rpt_item_s* item_ptr = rpt_ptr->item_list[i];
    item_ptr->sample_cnt_ratio = 
      (float)item_ptr->samples_per_report/(float)samples_per_report;
    SNS_SMGR_PRINTF4(
      LOW, "sample_ratio - rpt_id=%u/0x%x spr=%u ratio=0x%x",
      rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle,
      item_ptr->samples_per_report, item_ptr->sample_cnt_ratio);
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_update_items_info

===========================================================================*/
/*!
  @brief After DDF sensor is configured with new ODR its associated report
         items must be updated.

  @param[i] ddf_sensor_ptr - the DDF sensor
  @param[i] new_items_only - whether to update only items in LINKING state

  @return
*/
/*=========================================================================*/
void sns_rh_sol_update_items_info(
  sns_smgr_ddf_sensor_s*  ddf_sensor_ptr,
  bool                    new_items_only)
{
  sns_rh_rpt_item_s*  item_ptr;
  sns_rh_rpt_item_s** item_ptr_ptr;
  /* ---------------------------------------------------------------------- */

  SNS_SMGR_PRINTF3(
    HIGH, "items_info - ddf_sensor=%d new_only=%u depot_intvl=%u",
    SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type), 
    new_items_only, ddf_sensor_ptr->depot_data_interval);

  SMGR_FOR_EACH_ASSOC_ITEM( ddf_sensor_ptr, item_ptr, item_ptr_ptr )
  {
    if ( new_items_only && (item_ptr->state != RH_RPT_ITEM_STATE_LINKING) )
    {
      continue;
    }
    sns_rh_sol_compute_sampling_factor(item_ptr);
    sns_rh_sol_compute_cic_factor(item_ptr);
    sns_rh_sol_determine_sample_quality(item_ptr);
    sns_rh_sol_compute_effective_rate(item_ptr);
    if ( item_ptr->ts_last_sent == 0 )
    {
#ifndef SNS_QDSP_SIM
      item_ptr->ts_last_sent = 
        item_ptr->parent_report_ptr->msg_header.timestamp -
        item_ptr->ddf_sensor_ptr->current_filter_delay -
        (item_ptr->cic_obj.delay_ticks<<1);
#else
      item_ptr->ts_last_sent = 
        playback_next_sample_ts - QDSP_PLAYBACK_SAMPLE_LAST_TICK_DELTA;
#endif
    }

    if ( item_ptr->state < RH_RPT_ITEM_STATE_IDLE )
    {
      item_ptr->state = RH_RPT_ITEM_STATE_IDLE;
      if ( sns_smgr_ddf_sensor_is_event_sensor(ddf_sensor_ptr) &&
           ddf_sensor_ptr->latest_sample_ts != 0 )
      {
        item_ptr->ts_last_sent  = ddf_sensor_ptr->latest_sample_ts - 1;
        if ( item_ptr->parent_report_ptr->num_items == 1 &&
             item_ptr->parent_report_ptr->rpt_interval == 0 )
        {
          /* this is the only requested sensor in the report; send indication now */
          sns_rh_signal_me(SNS_RH_SAMPLE_READY_SIG);
        } /* else, indication sent when other requested sensors provide samples */
      }
    }
    SNS_SMGR_PRINTF4(
      MED, "items_info - rpt_id=%u/0x%x ind_cnt=%u quality=%d",
      item_ptr->parent_report_ptr->rpt_id, 
      item_ptr->parent_report_ptr->msg_header.connection_handle,
      item_ptr->parent_report_ptr->ind_count, item_ptr->quality);
    SNS_SMGR_PRINTF4(
      LOW, "items_info - state=%u rate=0x%x samp_intvl=%u samp_factor=%u",
      item_ptr->state, item_ptr->effective_rate_hz, item_ptr->sampling_interval,
      item_ptr->sampling_factor);
    SNS_SMGR_PRINTF3(
      LOW, "items_info - last(proc/saved/sent)=%u / %u / %u",
      item_ptr->last_processed_sample_timestamp, item_ptr->ts_last_saved,
      item_ptr->ts_last_sent);
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_update_periodic_like_flag

===========================================================================*/
/*!
  @brief Sets periodic_like field based on report rate and sampling rates
  @param [io] rpt_ptr - the report of interest
  @return none
*/
/*=========================================================================*/
void sns_rh_update_periodic_like_flag(sns_rh_rpt_spec_s* rpt_ptr)
{
  rpt_ptr->periodic_like = true;
  if ( rpt_ptr->event_sensors_only )
  {
    rpt_ptr->periodic_like = false;
  }
  else if ( rpt_ptr->q16_rpt_rate != 0 )
  {
    q16_t max_sampling_rate = rpt_ptr->item_list[0]->sampling_rate_hz;
    uint8_t i;
    RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
    {
      max_sampling_rate = MAX(max_sampling_rate,
                              rpt_ptr->item_list[i]->sampling_rate_hz);
    }

    if ( (float)((float)max_sampling_rate/(float)rpt_ptr->q16_rpt_rate) < 
         RH_SOL_PERIODIC_LIKE_RATIO_THRESHOLD )
    {
      rpt_ptr->q16_rpt_rate = max_sampling_rate;
    }

    /* TODO - Consider making sub-hz request-handling smarter, such that sub-hz
       for high-sampling-rate sensors (like gyro), do not incur a higher power
       penalty. The higher power penalty comes from the RH having to check if
       a report is ready to go out after receiving every sample. This could be
       optimized in reports where the report rate is sub-hz while the sampling
       rate is very high (like 50 Hz or higher). */
    if ( rpt_ptr->q16_rpt_rate != max_sampling_rate )
    {
      rpt_ptr->periodic_like = false;
    }
    rpt_ptr->q16_rpt_rate = MIN(rpt_ptr->q16_rpt_rate, max_sampling_rate);
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_smgr_update_assoc_reports

===========================================================================*/
/*!
  @brief Prepares reports associated with the given sensor for rescheduling
 
  @details Executes in SMGR context when the given sensor completes (re)configuration.

  @param   ddf_sensor_ptr
 
  @return  none
 */
/*=========================================================================*/
void sns_rh_sol_smgr_update_assoc_reports(sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  sns_rh_rpt_item_s*  item_ptr;
  sns_rh_rpt_item_s** item_ptr_ptr;
  bool                resched_reports = false;
  /* ---------------------------------------------------------------------- */

  SMGR_FOR_EACH_ASSOC_ITEM( ddf_sensor_ptr, item_ptr, item_ptr_ptr )
  {
    sns_rh_rpt_spec_s* rpt_ptr = item_ptr->parent_report_ptr;

    if ( rpt_ptr->q16_rpt_rate == 0 ||
         rpt_ptr->state == RH_RPT_STATE_DELETING ||
         item_ptr->effective_rate_hz == 0 )
    {
      /* skips scheduling Query reports, and reports being deleted or being added */
      continue;
    }
    if ( SNS_RH_REPORT_IS_PERIODIC(rpt_ptr) || rpt_ptr->periodic_like )
    {
      sns_rh_sol_compute_sample_ratio(rpt_ptr);
    }
    else if ( sns_rh_sol_compute_min_sampling_interval(rpt_ptr) )
    {
    sns_rh_sol_compute_sample_ratio(rpt_ptr);
    resched_reports = true;
    }
  }
  
  if ( resched_reports )
  {
    sns_rh_signal_me(SNS_RH_RESCHED_REPORTS_SIG);
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_reschedule_reports

===========================================================================*/
/*!
  @brief Reschedules reports if necessary
 
  @details Executes in RH task context

  @param   none
 
  @return  none
 */
/*=========================================================================*/
void sns_rh_sol_reschedule_reports(void)
{
  sns_rh_rpt_spec_s* rpt_ptr;
  sns_q_s*           rpt_queue_ptr = sns_rh_get_report_queue();
  /* ---------------------------------------------------------------------- */
  
  SMGR_FOR_EACH_Q_ITEM(rpt_queue_ptr, rpt_ptr, rpt_link)
  {
    if ( TICK1_GTR_TICK2(rpt_ptr->ts_sensors_updated, rpt_ptr->ts_last_updated) &&
         rpt_ptr->rpt_tick == 0 )
    {
      if ( rpt_ptr->event_sensors_only )
      {
        uint8_t i;
         RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
         {
           sns_ddf_time_t latest_sample_ts = 
             sns_rh_get_latest_sample_timestamp(rpt_ptr->item_list[i]);
           if ( latest_sample_ts != 0 )
           {
             /* send existing samples now */
             rpt_ptr->ts_last_rpt_sent = rpt_ptr->msg_header.timestamp;
             rpt_ptr->rpt_tick = latest_sample_ts;
             sns_rh_signal_me(SNS_RH_REPORT_TIMER_SIG);
             break;
           }
         }
      }
      
      if ( SNS_RH_REPORT_IS_BUFFERING(rpt_ptr) && !rpt_ptr->periodic_like && 
           rpt_ptr->rpt_tick == 0 )
      {
        sns_rh_rpt_spec_s* same_rate_rpt_ptr = sns_rh_find_same_report_rate(rpt_ptr);
        if ( same_rate_rpt_ptr != NULL )
        {
          /* synchronizes with another report of the same rate */
          rpt_ptr->ts_last_rpt_sent = same_rate_rpt_ptr->ts_last_rpt_sent;
          rpt_ptr->rpt_tick         = same_rate_rpt_ptr->rpt_tick;
        }
        else
        {
          /* schedule the first report one report interval from now */
          rpt_ptr->ts_last_rpt_sent = rpt_ptr->msg_header.timestamp;
          rpt_ptr->rpt_tick = rpt_ptr->rpt_interval + rpt_ptr->ts_last_rpt_sent;
          sns_rh_signal_me(SNS_RH_UPDATE_REPORT_TIMER_SIG);
        }
      }
      rpt_ptr->ts_last_updated = sns_ddf_get_timestamp();
      SNS_SMGR_PRINTF4(
        HIGH, "resched_rpts - rpt_id=%u/0x%x intvl=%u tick=%u", 
        rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle,
        rpt_ptr->rpt_interval, rpt_ptr->rpt_tick);
    }
    /* else, keeps the established schedule */
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_select_item_depot

===========================================================================*/
/*!
  @brief Selects the sample depot to be used by the given item 
  @param [io] item_ptr - the report item of interest
  @return none
*/
/*=========================================================================*/
void sns_rh_select_item_depot(sns_rh_rpt_item_s* item_ptr)
{
  if ( item_ptr->ddf_sensor_ptr->ddf_sensor_info->uimg_depot_info.depth > 0 )
  {
    item_ptr->depot_ptr = item_ptr->ddf_sensor_ptr->uimg_depot_ptr;
  }
  else if ( item_ptr->ddf_sensor_ptr->ddf_sensor_info->bimg_depot_info.depth > 0 )
  {
    item_ptr->depot_ptr = item_ptr->ddf_sensor_ptr->bimg_depot_ptr;
  }
  else
  {
    SNS_SMGR_PRINTF0(ERROR, "select_item_depot - no depot allocated");
    SNS_ASSERT(false);
  }
}


/*===========================================================================

  FUNCTION:   sns_rh_sol_handle_pending_rpt

===========================================================================*/
/*!
  @brief Delete marked report after fifo and depot flush is done.

  @detail  Report in extended state == RH_RPT_STATE_EXT_FLUSH_AND_DELETE_PENDING
           is deleted in this function.
           Also pending report attached to rpt_ptr will be activated here.

  @param[i/o]  rpt_ptr  - pointer to report
  @return   none
 */
/*=========================================================================*/
void sns_rh_sol_handle_pending_rpt(sns_rh_rpt_spec_s* rpt_ptr)
{
  sns_rh_rpt_spec_s* pending_rpt_ptr = rpt_ptr->pending_rpt_ptr;

  SNS_SMGR_PRINTF4(
    HIGH, "handle_pending_rpt - rpt_id=%u/0x%x ext_state=0x%04x pending_rpt=0x%x", 
    rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle, 
    rpt_ptr->state << 8 | rpt_ptr->state_extended, pending_rpt_ptr);

  if ( rpt_ptr->ind_buffer != NULL )
  {
    /* Flush command faced with QMI connection busy; can't send response yet */
    SNS_SMGR_PRINTF0(HIGH, "handle_pending_rpt - waiting for conn resume");
  }
  else if ( pending_rpt_ptr != NULL )  /* Available new pending rpt */
  {
    if ( rpt_ptr->state_extended == RH_RPT_STATE_EXT_ACTIVATE_PENDING )
    {
      /* updates and activates pending, sends reponse, and deletes existing */
      sns_rh_sol_update_pending_report(rpt_ptr, pending_rpt_ptr);
      sns_rh_sol_activate_report(pending_rpt_ptr);
      sns_rh_mr_send_resp(&pending_rpt_ptr->msg_header, pending_rpt_ptr->resp_ptr);
      pending_rpt_ptr->resp_ptr       = NULL;
      sns_rh_sol_delete_report(rpt_ptr);
      sns_smgr_signal_me(SNS_SMGR_SENSOR_STATE_SIG);
    }
    else
    {
      /* sends reponse and deletes pending */
      sns_rh_mr_send_resp(&pending_rpt_ptr->msg_header, pending_rpt_ptr->resp_ptr);
      sns_rh_sol_delete_report(pending_rpt_ptr);
      rpt_ptr->state_extended  = RH_RPT_STATE_EXT_NONE;
      rpt_ptr->pending_rpt_ptr = NULL;
    }
  }
  else
  {
    SNS_SMGR_PRINTF0(ERROR, "handle_pending_rpt - missing pending report");
    rpt_ptr->state_extended = RH_RPT_STATE_EXT_NONE;
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_suspend_sensor_clients

===========================================================================*/
/*!
  @brief Suspends all activities of given sensor and its sibling sensors

  @param[io]  ddf_sensor_ptr  - the sensor of interest
 
  @return   none
 */
/*=========================================================================*/
void sns_rh_sol_suspend_sensor_clients(sns_smgr_ddf_sensor_s* in_ddf_sensor_ptr)
{
  uint8_t s;
  const sns_ddf_driver_if_s* driver_ptr = 
    in_ddf_sensor_ptr->sensor_ptr->const_ptr->drv_fn_ptr;

  for ( s=0; s<ARR_SIZE(sns_smgr.sensor); s++ )
  {
    sns_smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[s];
    if ( sensor_ptr->const_ptr != NULL &&
         sensor_ptr->const_ptr->drv_fn_ptr == driver_ptr )
    {
      uint8_t d;
      for ( d=0; d<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); d++ )
      {
        sns_smgr_ddf_sensor_s* ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[d];
        if ( ddf_sensor_ptr == NULL )
        {
          continue;
        }

        if ( ddf_sensor_ptr->sensor_ptr->sensor_state == SENSOR_STATE_MD )
        {
          sns_smgr_set_md_state(false);
        }
        else if ( ddf_sensor_ptr->sensor_ptr->sensor_state == SENSOR_STATE_READY ||
                  ddf_sensor_ptr->sensor_ptr->sensor_state == SENSOR_STATE_CONFIGURING )
        {
          sns_smgr_client_stat_s* client_stat_ptr = &ddf_sensor_ptr->client_stat;
          sns_rh_rpt_item_s* item_ptr = client_stat_ptr->rpt_item_ptr;

          while ( (item_ptr = client_stat_ptr->rpt_item_ptr) != NULL )
          {
            if ( item_ptr->parent_report_ptr->holding_q == NULL )
            {
              item_ptr->parent_report_ptr->holding_q = sns_rh_get_suspended_report_queue();
            }
            /* else, it's an event-gated report and should go back to event queue */

            sns_rh_sol_deactivate_report(item_ptr->parent_report_ptr);
          }
        }
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_sol_resume_sensor_clients

===========================================================================*/
/*!
  @brief Resumes all activities of given sensor previously suspended 

  @param[io]  ddf_sensor_ptr  - the sensor of interest
 
  @return   none
 */
/*=========================================================================*/
void sns_rh_sol_resume_sensor_clients(sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  sns_q_s* suspended_report_q_ptr = sns_rh_get_suspended_report_queue();
  sns_rh_rpt_spec_s* rpt_ptr;
  uint8_t num_suspended_reports = sns_q_cnt(suspended_report_q_ptr);

  SNS_SMGR_PRINTF1(HIGH, "resume_sensor_clients - #reports=%u", num_suspended_reports);

  while ( (rpt_ptr = sns_q_get(suspended_report_q_ptr)) != NULL )
  {
    rpt_ptr->holding_q = NULL;
    sns_rh_sol_activate_report(rpt_ptr);
  }
  if ( num_suspended_reports > 0 )
  {
    sns_smgr_signal_me(SNS_SMGR_SENSOR_STATE_SIG);
  }
  else
  {
    sns_rh_update_sensor_event(ddf_sensor_ptr);
  }
}

