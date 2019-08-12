/*=============================================================================
  @file sns_rh_rpt_gen.c

  Implementation of Report Generation functions

******************************************************************************
* Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_rh_rpt_gen.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-11-10  pn   Allocates TCM memory for Query item
  2015-10-12  pn   Handles pending report when QMI connection resumes
  2015-09-24  pn   Code cleanup
  2015-09-18  pn   sns_rh_send_report_indications() renamed to sns_rh_send_ready_reports()
  2015-09-10  pn   Enhanced debug support for interpolation
  2015-08-28  pn   Improved handling of QMI connection busy/resume
  2015-08-10  pn   Uses correct message ID for Query when sending saved indication
  2015-06-15  pn   Fixed problem sending saved indications
  2015-05-27  bd   Indication flow control in SMGR
  2015-04-09  pn   Added support for 56-bit timestamp
  2014-10-06  jc   Grab mutex before accessing depot
  2014-09-25  pn   Uses depot selected for report item
  2014-06-16  pn   Removed references to unused circular buffer
  2014-04-29  pn   Refactored to support microImage
  2014-04-23  pn   Initial version

============================================================================*/

#define __SNS_MODULE__ SNS_RH

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_memmgr.h"
#include "sns_rh.h"
#include "sns_rh_main.h"
#include "sns_rh_main_uimg.h"
#include "sns_rh_mr.h"
#include "sns_rh_rpt_gen.h"
#include "sns_rh_sample_proc_uimg.h"
#include "sns_rh_sol.h"
#include "sns_rh_util.h"
#include "sns_smgr_depot.h"
#include "sns_smgr_main.h"
#include "sns_smgr_util.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
//#define SNS_SMGR_QUERY_DEBUG
#ifdef SNS_SMGR_QUERY_DEBUG
#define SNS_SMGR_QUERY_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_QUERY_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_QUERY_DBG3 SNS_SMGR_PRINTF3
#else
#define SNS_SMGR_QUERY_DBG1(level,msg,p1)
#define SNS_SMGR_QUERY_DBG2(level,msg,p1,p2)
#define SNS_SMGR_QUERY_DBG3(level,msg,p1,p2,p3)
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION:   sns_rh_rpt_gen_pack_query_sample

===========================================================================*/
/*!
  @brief Packs the given sample into indication message.

  @param[i]  query_ptr          - pointer to the query spec
  @param[i]  ind_ptr            - pointer to the query indication
  @param[i]  timestamp_ptr      - pointer to the sample timestamp
  @param[io] prev_timestamp_ptr - pointer to the previous timestamp

  @return   None
*/
/*=========================================================================*/
SMGR_STATIC void sns_rh_rpt_gen_pack_query_sample(
  sns_rh_query_s*                       query_ptr,
  sns_smgr_buffering_query_ind_msg_v01* ind_ptr,
  const sns_ddf_time_t*                 timestamp_ptr,
  sns_ddf_time_t*                       prev_timestamp_ptr,
  uint8_t                               quality)
{
  sns_rh_rpt_item_s*                item_ptr       = query_ptr->item_ptr;
  sns_smgr_buffering_sample_s_v01*  ind_sample_ptr =
    &ind_ptr->Samples[ind_ptr->Samples_len];
  /* -------------------------------------------------------------------- */

  ind_sample_ptr->Flags = sns_rh_apply_calibration(ind_sample_ptr->Data,
                                                   item_ptr->ddf_sensor_ptr,
                                                   item_ptr->cal_sel);

  ind_sample_ptr->Quality = quality;

  if ( ind_ptr->Samples_len++ == 0 )
  {
    ind_ptr->FirstSampleTimestamp   = (uint32_t)*timestamp_ptr;
    ind_sample_ptr->TimeStampOffset = 0;
  }
  else
  {
    ind_sample_ptr->TimeStampOffset = *timestamp_ptr - *prev_timestamp_ptr;
  }
  *prev_timestamp_ptr = *timestamp_ptr;
}

/*===========================================================================

  FUNCTION:   sns_rh_rpt_gen_send_empty_query_indication

===========================================================================*/
/*!
  @brief Generates one Query indication message with no samples.

  @param[i] query_ptr

  @return TRUE if indication sent; FALSE otherwise
*/
/*=========================================================================*/
bool sns_rh_rpt_gen_send_empty_query_indication(
  const sns_rh_query_s* query_ptr)
{
  sns_smgr_buffering_query_ind_msg_v01* ind_ptr = sns_rh_get_query_ind_buf();
  /* -------------------------------------------------------------------- */

  SNS_OS_MEMZERO(ind_ptr, sizeof(sns_smgr_buffering_query_ind_msg_v01));
  ind_ptr->QueryId              = query_ptr->query_id;
  ind_ptr->FirstSampleTimestamp = 0;
  ind_ptr->SamplingRate         = 0;
  ind_ptr->Samples_len          = 0;
  SNS_SMGR_PRINTF1(HIGH, "send_empty_query_ind - id=0x%x", ind_ptr->QueryId);
  sns_rh_rpt_gen_send_report_indication(
    query_ptr->item_ptr->parent_report_ptr,
    ind_ptr,
    SNS_SMGR_BUFFERING_QUERY_IND_V01,
    sizeof(sns_smgr_buffering_query_ind_msg_v01),
    false);
  return true;
}

/*===========================================================================

  FUNCTION:   sns_rh_rpt_gen_generate_query_indication

===========================================================================*/
/*!
  @brief Generates one Query indication message.

  @param[i] query_ptr

  @return
    true if indication sent, false otherwise

*/
/*=========================================================================*/
SMGR_STATIC bool sns_rh_rpt_gen_generate_query_indication(
  sns_rh_query_s* query_ptr)
{
  sns_smgr_buffering_query_ind_msg_v01* ind_ptr = sns_rh_get_query_ind_buf();
  sns_rh_interpolator_obj_s t0_t1_interp_obj;
  sns_ddf_time_t            previous_timestamp;
  const sns_smgr_sample_s*  sample_ptr;
  sns_rh_rpt_item_s*        item_ptr = query_ptr->item_ptr;
  /* -------------------------------------------------------------------- */

  if ( item_ptr == NULL )
  {
    return false;
  }

  sample_ptr = sns_smgr_depot_get_sample(&item_ptr->depot_iter, 0);

  SNS_OS_MEMZERO(ind_ptr, sizeof(sns_smgr_buffering_query_ind_msg_v01));
  ind_ptr->QueryId      = query_ptr->query_id;
  ind_ptr->SamplingRate = item_ptr->effective_rate_hz;

  SNS_OS_MEMZERO(&t0_t1_interp_obj, sizeof(sns_rh_interpolator_obj_s));
  t0_t1_interp_obj.item_ptr = item_ptr;
  t0_t1_interp_obj.desired_timestamp = query_ptr->T0;
  item_ptr->interpolator_obj.desired_timestamp = query_ptr->T0;
  item_ptr->interpolator_obj.sample1_ptr = NULL;
  item_ptr->interpolator_obj.sample2_ptr = NULL;
  item_ptr->last_processed_sample_timestamp = 0;

  /* skip invalid samples */
  while ( (sample_ptr != NULL) && SMGR_SAMPLE_STATUS(sample_ptr) != SNS_DDF_SUCCESS )
  {
    sample_ptr = sns_smgr_depot_get_sample(&item_ptr->depot_iter, 1);
  }

  if ( sample_ptr != NULL )
  {
    /* feeds interpolator with first valid sample */
    sns_rh_interpolator_update(&t0_t1_interp_obj, sample_ptr);
  }

  /* skip old samples */
  while ( (sample_ptr != NULL) && 
          TICK1_GTR_TICK2(query_ptr->T0, SMGR_SAMPLE_TIMESTAMP(sample_ptr)) )
  {
    sample_ptr = sns_smgr_depot_get_sample(&item_ptr->depot_iter, 1);
    sns_rh_interpolator_update(&t0_t1_interp_obj, sample_ptr);
  }

  if ( (t0_t1_interp_obj.sample1_ptr != NULL) &&
       (sample_ptr != NULL) && (query_ptr->T0 != SMGR_SAMPLE_TIMESTAMP(sample_ptr)))
  {
    SNS_SMGR_QUERY_DBG3(
      HIGH, "query_ind - s1=%u T0=%u s2=%u",
      SMGR_SAMPLE_TIMESTAMP(t0_t1_interp_obj.sample1_ptr), query_ptr->T0,
      SMGR_SAMPLE_TIMESTAMP(t0_t1_interp_obj.sample2_ptr));
    sns_rh_interpolate(
      &t0_t1_interp_obj, ind_ptr->Samples[ind_ptr->Samples_len].Data);
    sns_rh_rpt_gen_pack_query_sample(
      query_ptr, ind_ptr, &query_ptr->T0, &previous_timestamp,
      SNS_SMGR_ITEM_QUALITY_INTERPOLATED_V01);

    /* compute next desired timestamp in case it's needed*/
    RH_UPDATE_INTERP_TIMESTAMP(&item_ptr->interpolator_obj);
  }

  /* prepares to interpolate for T1*/
  t0_t1_interp_obj.desired_timestamp = query_ptr->T1;
  sns_rh_interpolator_update(&t0_t1_interp_obj, sample_ptr);

  if ( item_ptr->quality != SNS_SMGR_ITEM_QUALITY_CURRENT_SAMPLE_V01 )
  {
    SNS_SMGR_QUERY_DBG3(
      MED, "query_ind - quality=%d interp_intvl=%d desired_ts=%u",
      item_ptr->quality, item_ptr->interpolator_obj.interval_ticks,
      item_ptr->interpolator_obj.desired_timestamp);
  }

  while ( (sample_ptr != NULL) &&
          (ind_ptr->Samples_len < ARR_SIZE(ind_ptr->Samples)) &&
          TICK1_GEQ_TICK2(query_ptr->T1, SMGR_SAMPLE_TIMESTAMP(sample_ptr)) )
  {
    sns_ddf_status_e status;
    sns_ddf_time_t   timestamp;

    status = sns_rh_process_sample(item_ptr, sample_ptr, &timestamp,
                                   ind_ptr->Samples[ind_ptr->Samples_len].Data);
    if ( status == SNS_DDF_SUCCESS )
    {
      sns_rh_rpt_gen_pack_query_sample(
        query_ptr, ind_ptr, &timestamp, &previous_timestamp, item_ptr->quality);

      /* prepare for next sample */
      RH_UPDATE_INTERP_TIMESTAMP(&item_ptr->interpolator_obj);
    }
    else if ( status != SNS_DDF_PENDING )
    {
      SNS_SMGR_QUERY_DBG2(ERROR, "query_ind - ts=%u status=%d", 
                          SMGR_SAMPLE_TIMESTAMP(sample_ptr), status);
    }
    if ( ((item_ptr->quality != SNS_SMGR_ITEM_QUALITY_INTERPOLATED_V01) &&
          (item_ptr->quality != SNS_SMGR_ITEM_QUALITY_INTERPOLATED_FILTERED_V01) &&
          (query_ptr->T1 != SMGR_SAMPLE_TIMESTAMP(sample_ptr))) ||
         (status != SNS_DDF_SUCCESS) )
    {
      sample_ptr = sns_smgr_depot_get_sample(&item_ptr->depot_iter, 1);
      sns_rh_interpolator_update(&t0_t1_interp_obj, sample_ptr);
    }
  }
  if ( (sample_ptr != NULL) &&
       (ind_ptr->Samples_len < ARR_SIZE(ind_ptr->Samples)) &&
       TICK1_GTR_TICK2(SMGR_SAMPLE_TIMESTAMP(sample_ptr), query_ptr->T1) )
  {
    SNS_SMGR_QUERY_DBG3(
      HIGH, "query_ind - s1=%u T1=%u s2=%u",
      SMGR_SAMPLE_TIMESTAMP(t0_t1_interp_obj.sample1_ptr), query_ptr->T1,
      SMGR_SAMPLE_TIMESTAMP(t0_t1_interp_obj.sample2_ptr));
    sns_rh_interpolate(
      &t0_t1_interp_obj, ind_ptr->Samples[ind_ptr->Samples_len].Data);
    sns_rh_rpt_gen_pack_query_sample(
      query_ptr, ind_ptr, &query_ptr->T1, &previous_timestamp,
      SNS_SMGR_ITEM_QUALITY_INTERPOLATED_V01);
  }
  SNS_SMGR_PRINTF2(HIGH, "send_query_ind - ID=0x%x samples=%u",
                   ind_ptr->QueryId, (unsigned)ind_ptr->Samples_len);
  sns_rh_rpt_gen_send_report_indication(
    item_ptr->parent_report_ptr,
    ind_ptr,
    SNS_SMGR_BUFFERING_QUERY_IND_V01,
    sizeof(sns_smgr_buffering_query_ind_msg_v01),
    false);
  item_ptr->parent_report_ptr->ind_count++;
  return TRUE;
}

/*===========================================================================

  FUNCTION:   sns_smgr_send_query_indication

===========================================================================*/
/*!
  @brief Generates one Query indication message.

  @param[i] NONE

  @return
   NONE

*/
/*=========================================================================*/
bool sns_rh_rpt_gen_send_query_indication(sns_rh_rpt_spec_s* rpt_ptr)
{
  bool sent = false;
  uint8_t i;
  /* -------------------------------------------------------------------- */

  for ( i=0; i<rpt_ptr->num_items; i++ )
  {
    sns_rh_query_s* query_ptr = rpt_ptr->item_list[i]->query_ptr;
    if ( query_ptr != NULL )
    {
      sns_ddf_time_t first_ts = 0, last_ts = 0;
      sns_rh_rpt_item_s* item_ptr = rpt_ptr->item_list[i];
      sns_smgr_ddf_sensor_s* ddf_sensor_ptr = item_ptr->ddf_sensor_ptr;
      const sns_smgr_sample_s* sample_ptr;

      sns_os_mutex_pend(ddf_sensor_ptr->depot_mutex_ptr, 0, NULL);

      sample_ptr = sns_smgr_depot_init_iter(item_ptr->depot_ptr,
                                            query_ptr->T0, &item_ptr->depot_iter);
      if (sample_ptr != NULL)
      {
        /* iterate to previous sample so T0 would be included in indication */
        sample_ptr = sns_smgr_depot_get_sample(&item_ptr->depot_iter, -1);
      }
      while ( (sample_ptr != NULL) && 
              (SMGR_SAMPLE_STATUS(sample_ptr) != SNS_DDF_SUCCESS) )
      {
        /* skip invalid samples */
        sample_ptr = sns_smgr_depot_get_sample(&item_ptr->depot_iter, 1);
      }
      sns_os_mutex_post(ddf_sensor_ptr->depot_mutex_ptr);

      SNS_SMGR_QUERY_DBG3(
         MED, "send_query_ind - T0=%u iter_sample_ts=%u status=%d",
         query_ptr->T0, sample_ptr?SMGR_SAMPLE_TIMESTAMP(sample_ptr):0,
         sample_ptr?SMGR_SAMPLE_STATUS(sample_ptr):-1);

      if ( sample_ptr != NULL )
      {
        first_ts = SMGR_SAMPLE_TIMESTAMP(sample_ptr);
      }
      if ( first_ts != 0 )
      {
        last_ts = sns_rh_get_latest_sample_timestamp(item_ptr);
        if ( TICK1_GTR_TICK2(first_ts, query_ptr->T1) )
        {
          SNS_SMGR_PRINTF2(
            LOW, "send_query_ind - no valid samples between %u and %u",
            query_ptr->T0, query_ptr->T1);
          /* the requested time period is in the past; no samples to send */
          sent = sns_rh_rpt_gen_send_empty_query_indication(query_ptr);
        }
        else if ( TICK1_GEQ_TICK2(last_ts, query_ptr->T1) )
        {
          /* some or all of available samples fall within requested time period */
          sent = sns_rh_rpt_gen_generate_query_indication(query_ptr);
        }
        /* else, not time to send indication for this Query */
      }
      else if ( TICK1_GTR_TICK2(sns_ddf_get_timestamp(), query_ptr->T1) )
      {
        SNS_SMGR_PRINTF0(LOW, "send_query_ind - no valid samples available");
        /* the requested time period is in the past; no samples to send */
        sent = sns_rh_rpt_gen_send_empty_query_indication(query_ptr);
      }
      /* else, not time to send indication for this Query */

      if ( sent || sns_rh_mr_conn_busy(rpt_ptr->msg_header.connection_handle) )
      {
        rpt_ptr->item_list[i]->query_ptr = query_ptr->next_query_ptr;
        SNS_OS_ANY_FREE(query_ptr);
        if ( sns_rh_mr_conn_busy(rpt_ptr->msg_header.connection_handle) )
        {
          break;
        }
      }
    }
  }

  return sent;
}

/*===========================================================================

  FUNCTION:   sns_rh_rpt_gen_save_indication

===========================================================================*/
/*!
  @brief Save indication in the rpt_spec_ptr when connection busy

  @param[i]  rpt_spec_ptr - the report spec
  @param[i]  ind_msg_ptr - the indication message to send
  @param[i]  len         - length of the indication body

  @return none
 */
/*=========================================================================*/
void sns_rh_rpt_gen_save_indication(
   sns_rh_rpt_spec_s* rpt_spec_ptr,
   void*              ind_msg_ptr,
   uint16_t           len)
{
  SNS_SMGR_PRINTF4(
    HIGH, "save_ind - rpt_id=%u/0x%x type=%u/0x%x", 
    rpt_spec_ptr->rpt_id, rpt_spec_ptr->msg_header.connection_handle,
    rpt_spec_ptr->msg_header.svc_num, rpt_spec_ptr->msg_header.msg_id);

  if ( rpt_spec_ptr->ind_buffer != NULL )
  {
    SNS_SMGR_PRINTF0(ERROR, "save_ind - previous saved ind still present");
  }
  else if ( (rpt_spec_ptr->ind_buffer = 
             SMGR_ALLOC_STRUCT(sns_rh_report_indication_u)) == NULL )
  {
    SNS_SMGR_PRINTF0(ERROR, "save_ind - OOM, indication dropped");
  }
  else
  {
    SNS_OS_MEMCOPY(rpt_spec_ptr->ind_buffer, ind_msg_ptr, len);
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_rpt_gen_send_saved_ind

===========================================================================*/
/*!
  @brief Sends the given report's saved indication
  @param[i]  rpt_spec_ptr  - the report with saved indication to be sent
  @return  none
 */
/*=========================================================================*/
void sns_rh_rpt_gen_send_saved_ind(sns_rh_rpt_spec_s* rpt_spec_ptr)
{
  if ( (rpt_spec_ptr->ind_buffer != NULL) && 
       !sns_rh_mr_conn_busy(rpt_spec_ptr->msg_header.connection_handle) )
  {
    uint16_t len = 0;
    uint16_t msg_id;

    switch ( rpt_spec_ptr->msg_header.msg_id )
    {
      case SNS_SMGR_BUFFERING_REQ_V01:
        if ( rpt_spec_ptr->q16_rpt_rate != 0 )
        {
          msg_id = SNS_SMGR_BUFFERING_IND_V01;
          len = sizeof(sns_smgr_buffering_ind_msg_v01);
        }
        else
        {
          msg_id = SNS_SMGR_BUFFERING_QUERY_IND_V01;
          len = sizeof(sns_smgr_buffering_query_ind_msg_v01);
        }
        break;
      case SNS_SMGR_EVENT_GATED_BUFFERING_REQ_V02:
        msg_id = SNS_SMGR_EVENT_GATED_BUFFERING_IND_V02;
        len = sizeof(sns_smgr_buffering_ind_msg_v01);
        break;
      case SNS_SMGR_REPORT_REQ_V01:
        msg_id = SNS_SMGR_REPORT_IND_V01;
        len = sizeof(sns_smgr_periodic_report_ind_msg_v01);
        break;
      default:
        SNS_OS_ANY_FREE(rpt_spec_ptr->ind_buffer);
        rpt_spec_ptr->ind_buffer = NULL;
        SNS_SMGR_PRINTF2(
          ERROR, "send_saved_ind - unexpected report type=%u/0x%x", 
          rpt_spec_ptr->msg_header.svc_num, rpt_spec_ptr->msg_header.msg_id);
        break;
    }

    if ( len > 0 )
    {
      bool sent = sns_rh_rpt_gen_send_report_indication(
                    rpt_spec_ptr,
                    rpt_spec_ptr->ind_buffer,
                    msg_id,
                    len,
                    true);
      if ( sent )
      {
        rpt_spec_ptr->ind_buffer = NULL;
        SNS_SMGR_PRINTF2(
          HIGH, "send_saved_ind - rpt_id=%u/0x%x", 
          rpt_spec_ptr->rpt_id, rpt_spec_ptr->msg_header.connection_handle);
      }
      else if ( !sns_rh_mr_conn_busy(rpt_spec_ptr->msg_header.connection_handle) )
      {
        /* failed to send due to reasons other than CONN_BUSY */
        SNS_OS_ANY_FREE(rpt_spec_ptr->ind_buffer);
        rpt_spec_ptr->ind_buffer = NULL;
        SNS_SMGR_PRINTF2(
          ERROR, "send_saved_ind - dropped rpt_id=%u/0x%x", 
          rpt_spec_ptr->rpt_id, rpt_spec_ptr->msg_header.connection_handle);
      }
    }
  }
}


/*===========================================================================

  FUNCTION:   sns_rh_process_conn_resume

===========================================================================*/
/*!
  @brief This function processes resume signal from SMR
  @param  none
  @return none
 */
/*=========================================================================*/
void sns_rh_process_conn_resume(void)
{
  sns_rh_rpt_spec_s* rpt_ptr;
  sns_rh_rpt_spec_s* rpt_ptrs[SNS_SMGR_MAX_REPORT_CNT];
  sns_q_s*           rpt_queue_ptr = sns_rh_get_report_queue();
  uint8_t            num_rpts = 0;

  SMGR_FOR_EACH_Q_ITEM( rpt_queue_ptr, rpt_ptr, rpt_link )
  {
    if ( rpt_ptr->ind_buffer != NULL && 
         !sns_rh_mr_conn_busy(rpt_ptr->msg_header.connection_handle) )
    {
      rpt_ptrs[num_rpts++] = rpt_ptr;
    }
  }

  SNS_SMGR_PRINTF1(MED, "process_conn_resume - #rpts=%u", num_rpts);

  if ( num_rpts > 0 )
  {
    uint8_t i;
    for ( i=0; i<num_rpts; i++ )
    {
      sns_rh_send_report_indication(rpt_ptrs[i]);
      if ( rpt_ptrs[i]->state_extended != RH_RPT_STATE_EXT_NONE )
      {
        sns_rh_sol_handle_pending_rpt(rpt_ptrs[i]);
      }
    }
    sns_rh_update_report_timer();
  }
}


