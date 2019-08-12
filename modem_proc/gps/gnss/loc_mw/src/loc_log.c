/*============================================================================
  @file loc_log.c
 
  Functions for SLIM module to communicate with Loc MW task
 
  Copyright (c) 2015 QUALCOMM Atheros, Inc.
   All Rights Reserved.
   Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_log.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "loc_log.h"
#include "sns_sam_iod_v01.h"

void loc_mw_log_als_iod_ind
(
  sns_sam_iod_report_ind_msg_v01 *p_als_iod_qmi_ind
)
{
  loc_mw_log_als_iod_resp *p_loc_mw_log_pkt;
  static uint32 q_count = 0;
  
  if ( p_als_iod_qmi_ind == NULL )
  {
    MSG_ERROR("NULL pointer!", 0, 0, 0);
    return;
  }

  if ( !log_status(LOG_GNSS_LOCMW_ALS_IOD_DATA_C) )
  {
    MSG_LOW("LocMw log not enabled",0,0,0);
    return;
  }
  MSG_HIGH("ALS IOD output OPC %d RU %d CONF %d",
            p_als_iod_qmi_ind->report_data.output_class,
            p_als_iod_qmi_ind->report_data.reason_unknown,
            p_als_iod_qmi_ind->report_data.confidence);

  p_loc_mw_log_pkt = (loc_mw_log_als_iod_resp*)log_alloc( 
                             LOG_GNSS_LOCMW_ALS_IOD_DATA_C,
                             sizeof(*p_loc_mw_log_pkt) );

  if (p_loc_mw_log_pkt == NULL)
  {
    MSG_ERROR("Geofencing Position Log allocation failure",0,0,0);
    return;	  
  }
  
  p_loc_mw_log_pkt->u_pkt_version = 1;
  p_loc_mw_log_pkt->q_als_iod_utc_timestamp = p_als_iod_qmi_ind->timestamp;
  p_loc_mw_log_pkt->u_is_async_resp = 1;
  p_loc_mw_log_pkt->w_als_iod_instance_id = p_als_iod_qmi_ind->instance_id;
  p_loc_mw_log_pkt->u_als_iod_sensor_resp = SNS_RESULT_SUCCESS_V01;
  p_loc_mw_log_pkt->u_als_iod_sensor_output_class = p_als_iod_qmi_ind->report_data.output_class;
  p_loc_mw_log_pkt->u_als_iod_unknown_reason = p_als_iod_qmi_ind->report_data.reason_unknown;
  p_loc_mw_log_pkt->f_als_iod_output_confidence = p_als_iod_qmi_ind->report_data.confidence;
  
  log_commit(p_loc_mw_log_pkt);
}

