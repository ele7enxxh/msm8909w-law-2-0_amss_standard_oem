#ifndef SNS_SSM_TIME_C
#define SNS_SSM_TIME_C
/*=============================================================================
  @file sns_ssm_time

  Sensors Service Manager's Time Service

  This file contains the functions required by the Time Service

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/ssm/framework/src/sns_ssm_time.c#1 $  */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2015-08-12  gju  Remove smr_util
  2014-01-30  dk   Initial version

=============================================================================*/


/*-----------------------------------------------------------------------------
* Include Files
* ---------------------------------------------------------------------------*/

#include "stdbool.h"
#include "sns_common.h"
#include "sns_common_v01.h"
#include "sns_debug_api.h"
#include "sns_em.h"
#include "sns_debug_str.h"
#include "sns_ssm_priv.h"
#include "sns_time_api_v02.h"
#include "sns_osa.h"


/*-----------------------------------------------------------------------------
* Global variables
* ---------------------------------------------------------------------------*/

static uint32_t             sns_ssm_time_rollover_cnt;
static uint32_t             sns_ssm_time_last_timestamp;
static sns_em_timer_obj_t   sns_ssm_time_priodic_timer;
static OS_EVENT             *sns_ssm_time_service_mutex_ptr;


#define SSM_PERIODIC_TIMER_TIMEOUT 0xFFF00000

/*-----------------------------------------------------------------------------
* sns_ssm_time_update_last_timestamp
* ---------------------------------------------------------------------------*/
sns_err_code_e sns_ssm_time_update_last_timestamp(uint32_t *timestamp, uint32_t *rollover)
{
   uint32_t current_timestamp;
   uint8_t os_err;
   sns_os_mutex_pend(sns_ssm_time_service_mutex_ptr, 0, &os_err );
   if( os_err != OS_ERR_NONE )
   {
      return SNS_ERR_FAILED;
   }
   current_timestamp = sns_em_get_timestamp();
   if (current_timestamp < sns_ssm_time_last_timestamp)
   {
      ++sns_ssm_time_rollover_cnt;
   }
   sns_ssm_time_last_timestamp = current_timestamp;
   *timestamp = current_timestamp;
   *rollover = sns_ssm_time_rollover_cnt;
   os_err = sns_os_mutex_post(sns_ssm_time_service_mutex_ptr );
   if( os_err != OS_ERR_NONE )
   {
      return SNS_ERR_FAILED;
   }
   return SNS_SUCCESS;
}

/*-----------------------------------------------------------------------------
* sns_ssm_time_periodic_cb
* ---------------------------------------------------------------------------*/
void sns_ssm_time_periodic_cb(void * pargs)
{
   uint32_t current_timestamp;
   uint32_t rollover_cnt;

   SNS_SSM_PRINTF0(HIGH, "SSM: Time service timer callback");
   sns_ssm_time_update_last_timestamp(&current_timestamp, &rollover_cnt);
}


/*-----------------------------------------------------------------------------
* sns_ssm_time_init
* ---------------------------------------------------------------------------*/
sns_err_code_e sns_ssm_time_init(void)
{
   uint8_t err;
   sns_ssm_time_rollover_cnt = 0;
   sns_ssm_time_last_timestamp = sns_em_get_timestamp();

   // TODO: Move mutex creation to SSM initialization
   sns_ssm_time_service_mutex_ptr = sns_os_mutex_create(SNS_SSM_QUE_MUTEX,
                                                        &err);
   if(err != OS_ERR_NONE)
   {
      return SNS_ERR_FAILED;
   }
   err = sns_em_create_timer_obj(sns_ssm_time_periodic_cb,
                                 NULL, 
                                 SNS_EM_TIMER_TYPE_PERIODIC,
                                 &sns_ssm_time_priodic_timer);
   if(err != OS_ERR_NONE)
   {
      return SNS_ERR_FAILED;
   }

   return sns_em_register_timer(sns_ssm_time_priodic_timer,
                                SSM_PERIODIC_TIMER_TIMEOUT);

}

/*-----------------------------------------------------------------------------
* sns_ssm_time_proc_req
* ---------------------------------------------------------------------------*/
sns_err_code_e sns_ssm_time_proc_req(qmi_req_handle req_handle)
{
   sns_time_timestamp_resp_msg_v02 resp_msg;
   sns_err_code_e sns_err;
   uint32_t current_rollover;
   uint32_t current_timestamp;

   sns_err = sns_ssm_time_update_last_timestamp(&current_timestamp, &current_rollover);

   if( sns_err != SNS_SUCCESS )
   {
      return SNS_ERR_FAILED;
   }

   resp_msg.resp.sns_result_t  =   SENSOR1_SUCCESS;
   resp_msg.error_code_valid = false;
   resp_msg.timestamp_apps_valid = false;
   resp_msg.dsps_rollover_cnt_valid = true;
   resp_msg.dsps_rollover_cnt = current_rollover;
   resp_msg.timestamp_dsps_valid = true;
   resp_msg.timestamp_dsps = current_timestamp;
   SNS_SSM_PRINTF0(HIGH, "SSM: Responding to Time Service request");

   qmi_csi_send_resp(req_handle,
                     SNS_TIME_TIMESTAMP_RESP_V02,
                     &resp_msg,
                     sizeof(sns_time_timestamp_resp_msg_v02));

   return SNS_SUCCESS;
}

#endif /* #ifndef SNS_SSM_TIME_C */
