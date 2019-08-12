/*=============================================================================
  @file sns_profiling.c

  This file contains sensor use case timeline profiling code.

*******************************************************************************
* Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved
* Qualcomm Technologies Confidential and Proprietary.
********************************************************************************/


/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-09-17   ps   Remove AVS component dependency
  2014-06-05   vy   Refactored for uImage
  2013-07-26   vh   Eliminated compiler warnings
  2013-06-25   hw   Add latency measurement support
  2012-12-31   hw   Initial version
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdio.h>
#include <qurt.h>
#include "qurt_timer.h"

#include "sns_profiling.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_osa.h"
#include "sns_em.h"
//#include "sns_ddf_common.h"
//#include "uTimetick.h"
#include "sns_smgr_ddf_if.h"

volatile qurt_mutex_t sns_profiling_mutex;
volatile sns_pwr_prof_struct sns_pwr_profiler[SNS_PWR_PROFILING_BUF_SIZE] __attribute__ ((aligned (256)));
unsigned int sns_pwr_prof_loc_count, sns_pwr_wraparound_count;            //count the size of current buffer

/* Initiate the buffer */
static void sns_profiling_struct_init( sns_pwr_prof_struct *profiler )
{
  unsigned int j;

  qurt_mutex_init((qurt_mutex_t *) &sns_profiling_mutex);
  qurt_mutex_lock((qurt_mutex_t *) &sns_profiling_mutex);
  sns_pwr_prof_loc_count=0;
  sns_pwr_wraparound_count=0;

  for ( j=0; j<SNS_PWR_PROFILING_BUF_SIZE; j++ )
  {
    profiler[j].code_loc = (uint64_t) 0xDEADDEAD;
    profiler[j].timestamp = (uint64_t)0xDEADDEAD;
  }
  qurt_mutex_unlock((qurt_mutex_t *) &sns_profiling_mutex);
}


void sns_profiling_init(void)
{
  sns_profiling_struct_init((sns_pwr_prof_struct *)sns_pwr_profiler);
}


/* Get the current location and timestamp */
void sns_profiling_log_timestamp( uint64_t curr_loc_tag )
{
  if( sns_ddf_smgr_is_in_uimage() )
    return;
  
  qurt_mutex_lock((qurt_mutex_t *) &sns_profiling_mutex);
  if ( sns_pwr_prof_loc_count < ( SNS_PWR_PROFILING_BUF_SIZE-2 ) )
  {
    sns_pwr_prof_loc_count++;
  }
  else
  {
    sns_pwr_wraparound_count++;
    sns_pwr_prof_loc_count=20;
  }
  sns_pwr_profiler[sns_pwr_prof_loc_count].code_loc = (uint64_t)curr_loc_tag;
  sns_pwr_profiler[sns_pwr_prof_loc_count].timestamp = (uint64_t)(uTimetick_Get());
  qurt_mutex_unlock((qurt_mutex_t *) &sns_profiling_mutex);
}


void sns_profiling_log_latency_poll(
        sns_profiling_latency_s     latency,
        int32_t                     sensor_id)
{
  sns_log_latency_sampling_poll_s  *log_ptr;
  sns_err_code_e err = SNS_SUCCESS;

  if( sns_ddf_smgr_is_in_uimage() )
    return;

  err = sns_logpkt_malloc(SNS_LOG_LATENCY_SAMPLING_POLL,
                          sizeof(sns_log_latency_sampling_poll_s),
                          (void**)&log_ptr);

  if ( (err == SNS_SUCCESS) && (log_ptr != NULL) )
  {
    log_ptr->version = SNS_LOG_LATENCY_SAMPLING_POLL_VERSION;
    log_ptr->timestamp = (uint64_t)sns_em_get_timestamp();
    log_ptr->sensor_id = sensor_id;
    log_ptr->data_timestamp = (uint64_t)latency.polling_get_data_end_ts;
    log_ptr->sampling_latency = (int64_t)(latency.polling_get_data_end_ts - latency.polling_get_data_start_ts);
    log_ptr->polling_latency = (int64_t)(latency.polling_get_data_end_ts - latency.polling_cb_ts);

    err = sns_logpkt_commit(SNS_LOG_LATENCY_SAMPLING_POLL, log_ptr);
    if ( err != 0 )
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "log packet commit with err: %d", err);
    }
  }
  else
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Latency log packet malloc err: %d", err);
  }
}


void sns_profiling_log_latency_dri(
        sns_profiling_latency_s    latency,
        int32_t                    sensor_id)
{
  sns_log_latency_sampling_dri_s  *log_ptr;
  sns_err_code_e err = SNS_SUCCESS;

  if( sns_ddf_smgr_is_in_uimage() )
    return;
    
  err = sns_logpkt_malloc(SNS_LOG_LATENCY_SAMPLING_DRI,
                          sizeof(sns_log_latency_sampling_dri_s),
                          (void**)&log_ptr);

  if ( (err == SNS_SUCCESS) && (log_ptr != NULL) )
  {
    log_ptr->version = SNS_LOG_LATENCY_SAMPLING_DRI_VERSION;
    log_ptr->timestamp = (uint64_t)sns_em_get_timestamp();
    log_ptr->sensor_id = sensor_id;
    log_ptr->data_timestamp = (uint64_t)latency.dri_notify_irq_ts;
    log_ptr->dri_get_data_latency = (int64_t)(latency.dri_get_data_end_ts - latency.dri_get_data_start_ts);
    log_ptr->sampling_latency = (int64_t)(latency.dri_get_data_end_ts - latency.dri_notify_irq_ts);

    err = sns_logpkt_commit(SNS_LOG_LATENCY_SAMPLING_DRI, log_ptr);
    if ( err != 0 )
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "log packet commit with err: %d", err);
    }
  }
  else
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Latency log packet malloc err: %d", err);
  }
}


