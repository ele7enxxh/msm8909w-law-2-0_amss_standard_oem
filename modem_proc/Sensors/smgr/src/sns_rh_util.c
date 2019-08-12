/*=============================================================================
  @file sns_rh_util.c

  This file implements the utility functions of Request Handler

******************************************************************************
* Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_rh_util.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-09-24  pn   Code cleanup
  2014-07-14  VY   Fixed compiler warnings
  2014-05-06  vy   Refactored to support uImage  
  2014-04-23  pn   Initial version

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_em.h"
#include "sns_osa.h"
#include "sns_memmgr.h"
#include "sns_rh_main.h"
#include "sns_rh_sol.h"
#include "sns_smgr_main.h"
#include "sns_rh_util.h"
#include "sns_log_api.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Function Prototypes
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*===========================================================================

  FUNCTION:   sns_rh_get_max_report_rate_for_sensor

===========================================================================*/
/*!
  @brief Returns the max requested report rate for the given sensor.
 
  @details For Query requests, sampling rate is used as report rate 

  @param None

  @return The highest requested report rate
*/
/*=========================================================================*/
q16_t sns_rh_get_max_report_rate_for_sensor(
  const sns_smgr_sensor_s* sensor_ptr)
{
  uint32_t i;
  q16_t q16_rpt_rate = 0;
  for ( i=0; i<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
  {
    if ( sensor_ptr->ddf_sensor_ptr[i] != NULL )
    {
      q16_rpt_rate = 
        MAX(q16_rpt_rate, sensor_ptr->ddf_sensor_ptr[i]->client_stat.max_report_freq_hz);
    }
  }
  SNS_SMGR_PRINTF3(
    LOW, "max_report_rate - sensor=%u rate=0x%x/%u", 
    SMGR_SENSOR_ID(sensor_ptr), q16_rpt_rate, q16_rpt_rate >> 16);

  return q16_rpt_rate;
}

/*=========================================================================
  FUNCTION:  sns_rh_put_next_response
  =========================================================================*/
void sns_rh_put_next_response(sns_smgr_request_response_s* req_resp_ptr)
{
  sns_q_link(req_resp_ptr, &req_resp_ptr->q_link);
  sns_os_mutex_pend(sns_rh.resp_queue.mutex_ptr, 0, NULL);
  sns_q_put(&sns_rh.resp_queue.protected_q, &req_resp_ptr->q_link);
  (void)sns_os_mutex_post(sns_rh.resp_queue.mutex_ptr);

  sns_rh_signal_me(SNS_RH_RESP_Q_SIG);
}

/*=========================================================================
  FUNCTION:  sns_rh_get_next_response
  =========================================================================*/
sns_smgr_request_response_s* sns_rh_get_next_response(void)
{
  sns_smgr_request_response_s*  req_resp_ptr = NULL;

  sns_os_mutex_pend(sns_rh.resp_queue.mutex_ptr, 0, NULL);
  req_resp_ptr = 
    (sns_smgr_request_response_s*)sns_q_get(&sns_rh.resp_queue.protected_q);
  (void)sns_os_mutex_post(sns_rh.resp_queue.mutex_ptr);

  return req_resp_ptr;
}

/*=========================================================================
  FUNCTION:  sns_rh_put_next_indication
  =========================================================================*/
void sns_rh_put_next_indication(sns_smgr_indication_s* ind_ptr)
{
  sns_q_link(ind_ptr, &ind_ptr->q_link);
  sns_os_mutex_pend(sns_rh.ind_queue.mutex_ptr, 0, NULL);
  sns_q_put(&sns_rh.ind_queue.protected_q, &ind_ptr->q_link);
  (void)sns_os_mutex_post(sns_rh.ind_queue.mutex_ptr);

  sns_rh_signal_me(SNS_RH_IND_Q_SIG);
}

/*=========================================================================
  FUNCTION:  sns_rh_get_next_indication
  =========================================================================*/
sns_smgr_indication_s* sns_rh_get_next_indication(void)
{
  sns_smgr_indication_s* ind_ptr = NULL;

  sns_os_mutex_pend(sns_rh.ind_queue.mutex_ptr, 0, NULL);
  ind_ptr = (sns_smgr_indication_s*) sns_q_get(&sns_rh.ind_queue.protected_q);
  (void)sns_os_mutex_post(sns_rh.ind_queue.mutex_ptr);

  return ind_ptr;
}


