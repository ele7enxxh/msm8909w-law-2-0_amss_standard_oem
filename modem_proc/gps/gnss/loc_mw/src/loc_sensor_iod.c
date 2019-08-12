/*============================================================================
 @file loc_sensor_client.c

 loc MW sensor client implementation

 GENERAL DESCRIPTION

 This file contains the loc middleware sensor client implementation.

 Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
 All Rights Reserved
 Qualcomm Confidential and Proprietary
 
 Copyright (c) 2015 Qualcomm Atheros, Inc.
 All Rights Reserved. 
 Qualcomm Atheros Confidential and Proprietary.

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_sensor_iod.c#1 $
 $DateTime: 2016/12/13 07:59:45 $
 $Author: mplcsds1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

=============================================================================*/

#include "customer.h"

#if (defined(FEATURE_GEOFENCE_SENSOR_SUPPORT) && (!defined(FEATURE_SMART_OFFLINE_COMPILATION)))
#include "comdef.h"
#include "msg.h"
#include "qw.h"
#include "time_svc.h"
#include "aries_os_api.h"
#include "qmi_cci_target_ext.h"
#include "sns_sam_iod_autocal_v01.h"
#include "qmi_client.h"
#include "loc_log.h"
#include "loc_sensor_iod.h"
#include "gm_api.h"




#define LOC_SENSOR_SVC_IND_EVENT C_OS_FLAG_QMI_CCI_EVENT 
/*No need to handle the above event as we do not use QMI notify init and use
QMI client init directly. But still the event is required for internal QMI purpose which 
QMI team uses to block our thread for flow control. No handling on our side is necessary*/
/*---------------------------------------------------------------------------
   Notifier OS param
---------------------------------------------------------------------------*/
static qmi_client_os_params          loc_geofence_iod_sensor_notifier_os_params;
static qmi_client_type               loc_geofence_iod_server_handle = NULL;
static qmi_idl_service_object_type   loc_geofence_iod_sensor_svc_obj=NULL;

static qmi_client_type               loc_geofence_iod_autocal_server_handle = NULL;
#define SHIM_MSG_LOW( str, a, b, c )                                          \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED, str,  a, b, c )
#define SHIM_MSG_MED( str, a, b, c )                                          \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED, str, a, b, c )
#define SHIM_MSG_HIGH( str, a, b, c )                                         \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH, str, a, b, c )
#define SHIM_MSG_ERROR( str, a, b, c )                                        \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR, str, a, b, c )
#define SHIM_ERR_FATAL( str, a, b, c )                                        \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_FATAL, str, a, b, c )



/** loc_geofence_sensor_iod_handle_async_report
 *  Handle the IOD Async Report from IOD service. This carries async IOD report 
 *  indications
 *  @param [in] p_report_ind
 * */
void loc_geofence_sensor_iod_handle_async_report
(
  sns_sam_iod_report_ind_msg_v01 *p_report_ind
)
{
  loc_geofence_iod_ind iod_indication;

  if(p_report_ind == NULL)
  {
    SHIM_MSG_ERROR("iod Report Indication pointer NULL",0,0,0);
    return;  
  }
  
  SHIM_MSG_HIGH("IOD Report Indication Instace TS %d Output %d",
                 p_report_ind->timestamp, 
                 p_report_ind->report_data.output_class, 0);

  memset((void*)&iod_indication, 0, sizeof(iod_indication)); 
  iod_indication.iod_indication_type = SNS_SAM_IOD_REPORT_IND_V01;
  memscpy((void*)&iod_indication.u.iod_ind, sizeof(iod_indication.u.iod_ind),
  	(void*)p_report_ind, sizeof(*p_report_ind));
  
  loc_mw_log_als_iod_ind(p_report_ind);
  
  gm_inject_iod_indication(&iod_indication);
}

/** loc_geofence_sensor_iod_cb
 *    IOD callback registered when enabling IOD.
 * */
void loc_geofence_sensor_iod_cb
(
 qmi_client_type        user_handle,
 unsigned int           msg_id,
 void                  *buf,
 unsigned int           len,
 void                  *resp_cb_data,
 qmi_client_error_type  transp_err
 )
{
  loc_geofence_iod_ind iod_indication;
  boolean  b_inject = FALSE;

  if(buf == NULL)
  {
    SHIM_MSG_ERROR("NULL Buffer pointer in loc_geofence_sensor_iod_cb",0,0,0);
    return;
  }

  memset((void*)&iod_indication, 0, sizeof(iod_indication));

  /* Print the appropriate message based on the message ID */
  switch (msg_id)
  {
    case SNS_SAM_IOD_ENABLE_RESP_V01:
      SHIM_MSG_HIGH("loc_sensor_iod_cb: iod Enable Response",0,0,0);
      iod_indication.iod_indication_type = SNS_SAM_IOD_ENABLE_RESP_V01;
      memscpy( (void*)&iod_indication.u.iod_enable_resp,
               sizeof(iod_indication.u.iod_enable_resp),
               (void*)buf, sizeof(sns_sam_iod_enable_resp_msg_v01));
      b_inject = TRUE;
      break;

    case SNS_SAM_IOD_DISABLE_RESP_V01:
      SHIM_MSG_HIGH("loc_sensor_iod_cb: iod Disable Response",0,0,0);
      iod_indication.iod_indication_type = SNS_SAM_IOD_DISABLE_RESP_V01;
      memscpy( (void*)&iod_indication.u.iod_disable_resp,
               sizeof(iod_indication.u.iod_disable_resp),
               (void*)buf, sizeof(sns_sam_iod_disable_resp_msg_v01));
      b_inject = TRUE;
      break;

    case SNS_SAM_IOD_UPDATE_RESP_V01:
      SHIM_MSG_HIGH("loc_sensor_iod_cb: iod Config Response",0,0,0);
      iod_indication.iod_indication_type = SNS_SAM_IOD_UPDATE_RESP_V01;
      memscpy( (void*)&iod_indication.u.iod_update_resp,
               sizeof(iod_indication.u.iod_update_resp),
               (void*)buf, sizeof(sns_sam_iod_update_resp_msg_v01));
      b_inject = TRUE;
      break;

    case SNS_SAM_IOD_GET_REPORT_RESP_V01:
      SHIM_MSG_HIGH("loc_sensor_iod_cb: iod GetReport Response",0,0,0);
      iod_indication.iod_indication_type = SNS_SAM_IOD_GET_REPORT_RESP_V01;
      memscpy( (void*)&iod_indication.u.iod_report_resp,
               sizeof(iod_indication.u.iod_report_resp),
               (void*)buf, sizeof(sns_sam_iod_report_ind_msg_v01));
      b_inject = TRUE;
      break;

    default:
      SHIM_MSG_HIGH("loc_sensor_iod_cb: Unknown iod msg %d", msg_id, 0, 0);
      b_inject = FALSE;
      break;
  }

  if(TRUE == b_inject)
  {
    gm_inject_iod_indication(&iod_indication);
  }
  os_MemFree((void**)&buf);
}

/** loc_geofence_iod_handle_indicator
 *    Handles IOD indications from the IOD Service
 * */
static void loc_geofence_iod_handle_indicator
(
 qmi_client_type    user_handle,
 unsigned int       msg_id,
 void              *ind_buf,
 unsigned int       ind_buf_len,
 void              *ind_cb_data
)
{
  qmi_client_error_type          retval;  
  sns_sam_iod_report_ind_msg_v01 iod_report_ind;


  SHIM_MSG_MED("loc_geofence_iod_handle_indicator Msg Id = %d",msg_id, 0,0);

  if(msg_id == SNS_SAM_IOD_REPORT_IND_V01)
  {
    if((ind_buf == NULL) || (ind_buf_len == 0))
    {      
      SHIM_MSG_ERROR("loc_geofence_iod_handle_indicator: Buff NULL\n",0, 0,0);
      return;
    }

    
    retval = qmi_client_message_decode(loc_geofence_iod_server_handle,
                                   QMI_IDL_INDICATION,
                                   SNS_SAM_IOD_REPORT_IND_V01,
                                   (void *)ind_buf,
                                   ind_buf_len,
                                   (void *)&iod_report_ind,
                                   sizeof(iod_report_ind));

    
    if (retval == QMI_NO_ERR)
    {
      loc_geofence_sensor_iod_handle_async_report(&iod_report_ind);
    }
    else
    {
      SHIM_MSG_ERROR("loc_gerofence_sensor_iod_handle_indicator: Message Decode Failed with Error %d\n",retval, 0,0);
    }
  }
}

/** loc_geofence_sensor_iod_autocal_cb
 *    IOD callback registered when enabling IOD Autocal.
 * */
void loc_geofence_sensor_iod_autocal_cb
(
 qmi_client_type        user_handle,
 unsigned int           msg_id,
 void                  *buf,
 unsigned int           len,
 void                  *resp_cb_data,
 qmi_client_error_type  transp_err
 )
{
  sns_sam_iod_autocal_enable_resp_msg_v01 *p_iod_autocal_enable_resp = NULL;

  if(buf == NULL)
  {
    SHIM_MSG_ERROR("NULL Buffer pointer in loc_geofence_sensor_iod_cb",0,0,0);
    return;
  }

  p_iod_autocal_enable_resp = (sns_sam_iod_autocal_enable_resp_msg_v01*) buf;

  /* Print the appropriate message based on the message ID */
  switch (msg_id)
  {
    case SNS_SAM_IOD_AUTOCAL_ENABLE_RESP_V01:
      MSG_4(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED,
            "iod autocal: Enable Resp %d ErrCode %d inst_id_valid %d inst %d",
            p_iod_autocal_enable_resp->resp.sns_result_t,
            p_iod_autocal_enable_resp->resp.sns_err_t,
            p_iod_autocal_enable_resp->instance_id_valid,
            p_iod_autocal_enable_resp->instance_id);
      break;

    case SNS_SAM_IOD_AUTOCAL_CANCEL_RESP_V01:
    case SNS_SAM_IOD_AUTOCAL_VERSION_RESP_V01:
    case SNS_SAM_IOD_AUTOCAL_DISABLE_RESP_V01:
    case SNS_SAM_IOD_AUTOCAL_GET_REPORT_RESP_V01:
    case SNS_SAM_IOD_AUTOCAL_UPDATE_RESP_V01:
    case SNS_SAM_IOD_AUTOCAL_GET_ATTRIBUTES_RESP_V01:
    default:
      SHIM_MSG_ERROR("iod autocal: Unexpected iod msg %d", msg_id, 0, 0);
      break;
  }

  os_MemFree((void**)&buf);
}

/** loc_geofence_iod_autocal_handle_indicator
 *    Handles indications from IOD AutoCal Service.
 * */
static void loc_geofence_iod_autocal_handle_indicator
(
 qmi_client_type    user_handle,
 unsigned int       msg_id,
 void              *ind_buf,
 unsigned int       ind_buf_len,
 void              *ind_cb_data
)
{
  qmi_client_error_type          retval;  
  sns_sam_iod_report_ind_msg_v01 iod_report_ind;


  SHIM_MSG_MED("loc_geofence_iod_autocal_handle_indicator Msg Id = %d", msg_id, 0,0);

  if((ind_buf == NULL) || (ind_buf_len == 0))
  {      
    SHIM_MSG_ERROR("IOD Autocal: Buff NULL\n",0, 0,0);
    return;
  }


  if(msg_id == SNS_SAM_IOD_AUTOCAL_ERROR_IND_V01)
  {
    sns_sam_iod_autocal_error_ind_msg_v01 z_error_ind;

    SHIM_MSG_ERROR("AutoCal ERROR!!", 0, 0, 0);
    retval = qmi_client_message_decode(
                     loc_geofence_iod_autocal_server_handle,
                     QMI_IDL_INDICATION,
                     SNS_SAM_IOD_AUTOCAL_ERROR_IND_V01,
                     (void *)ind_buf,
                     ind_buf_len,
                     (void *)&z_error_ind,
                     sizeof(z_error_ind));
    if (retval != QMI_NO_ERR)
    {
      SHIM_MSG_ERROR("could not decode QMI Ind!! %d", msg_id, 0, 0);
      return;
    }
    SHIM_MSG_MED("AutoCal Error!! %d %d", z_error_ind.error, z_error_ind.instance_id, 0);
  }
  if(msg_id == SNS_SAM_IOD_AUTOCAL_REPORT_IND_V01)
  {
    sns_sam_iod_autocal_report_ind_msg_v01 z_report_ind;

    SHIM_MSG_ERROR("AutoCal Report!!", 0, 0, 0);
    retval = qmi_client_message_decode(
                     loc_geofence_iod_autocal_server_handle,
                     QMI_IDL_INDICATION,
                     SNS_SAM_IOD_AUTOCAL_REPORT_IND_V01,
                     (void *)ind_buf,
                     ind_buf_len,
                     (void *)&z_report_ind,
                     sizeof(z_report_ind));
    if (retval != QMI_NO_ERR)
    {
      SHIM_MSG_ERROR("could not decode QMI Ind!! %d", msg_id, 0, 0);
      return;
    }
    MSG_4(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED, 
          "AutoCal Report!! %d %d %d 0x%x", 
                            z_report_ind.instance_id,
                            z_report_ind.convergence_status,
                            z_report_ind.auto_cal_coeff_valid,
                            (uint32) z_report_ind.auto_cal_coeff);
  }
}

/** loc_geofence_sensor_start_iod_autocal
 *    Handles IOD AutoCal Start request from Geofence module. We start the 
 *    AutoCal only once in a boot cycle. 
 *  @param [in] report_period
 * */
boolean loc_geofence_sensor_start_iod_autocal
(
  uint32  q_utc_time_secs,
  uint32  q_sunrisetime_sec,
  uint32  q_sunsettime_sec
)
{
  static qmi_client_os_params             iod_autocal_notifier_os_params;
  static qmi_idl_service_object_type      iod_autocal_svc_obj=NULL;

  sns_sam_iod_autocal_enable_req_msg_v01  *p_iod_autocal_enable_req_msg  = NULL;
  sns_sam_iod_autocal_enable_resp_msg_v01 *p_iod_autocal_enable_resp_msg = NULL;
  qmi_txn_handle txn;

  qmi_client_error_type         retval;  
  qmi_service_info              info[10];
  /* excuse native type, needed by API */
  unsigned int  num_services = 0, 
                num_entries = 1;

  if(loc_geofence_iod_autocal_server_handle != NULL)
  {
    SHIM_MSG_MED("IOD Autocal already started\n", 0,0,0);
    return TRUE;
  }

  /*-----------------------------------------------------------------------
     Get AutoCal service object
  -----------------------------------------------------------------------*/
  iod_autocal_svc_obj = SNS_SAM_IOD_AUTOCAL_SVC_get_service_object_v01();
  if(!iod_autocal_svc_obj)
  {
    SHIM_MSG_ERROR("Unable to get iod_autocal_svc_obj", 0, 0, 0);
    return FALSE;
  }

  /*------------------------------------------------------------------
     Check if the IOD AutoCal service is up, if not return. Else,
     Get addressing information for accessing the service
  ------------------------------------------------------------------*/  
  retval = qmi_client_get_service_list(iod_autocal_svc_obj,
                                       info,
                                       &num_entries,
                                       &num_services);
                                       
  if(QMI_NO_ERR != retval)
  {
    SHIM_MSG_ERROR("Could not Get Service list ERROR %d", retval, 0, 0);
    return FALSE;
  }

  SHIM_MSG_MED("IOD AutoCal Service UP num_entries = %d and num_services = %d",
               num_entries, num_services, 0);

  memset(&iod_autocal_notifier_os_params, 
          0,
          sizeof(iod_autocal_notifier_os_params));

  iod_autocal_notifier_os_params.tcb = rex_self();
  iod_autocal_notifier_os_params.sig = LOC_SENSOR_SVC_IND_EVENT;     
  
  retval = qmi_client_init(&info[0],
                      iod_autocal_svc_obj,
                      loc_geofence_iod_autocal_handle_indicator,
                      NULL, 
                      &iod_autocal_notifier_os_params,
                      &loc_geofence_iod_autocal_server_handle);

  if(retval != QMI_NO_ERR)
  {
    SHIM_MSG_ERROR("IOD AutoCal Client Init failed err %d", retval, 0, 0);
    return FALSE;
  }

  p_iod_autocal_enable_req_msg = os_MemAlloc(sizeof(*p_iod_autocal_enable_req_msg), 
                                             OS_MEM_SCOPE_PROCESSOR);

  if(p_iod_autocal_enable_req_msg == NULL)
  {    
    SHIM_MSG_ERROR(" Unable to allocate memory for Enable Autocal req",0,0,0);
    return FALSE;
  }
  memset((void*)p_iod_autocal_enable_req_msg, 0, sizeof(*p_iod_autocal_enable_req_msg));


  p_iod_autocal_enable_resp_msg = os_MemAlloc(sizeof(*p_iod_autocal_enable_resp_msg), 
                                              OS_MEM_SCOPE_PROCESSOR);
  if(p_iod_autocal_enable_resp_msg == NULL)
  {    
    SHIM_MSG_ERROR("Unable to allocate memory for Enable Autocal resp",0,0,0);
    os_MemFree((void**)&p_iod_autocal_enable_req_msg);
    return FALSE;
  }
  memset((void*)p_iod_autocal_enable_resp_msg, 0, sizeof(*p_iod_autocal_enable_resp_msg));
  
  p_iod_autocal_enable_req_msg->report_period = 0; /* reporting on new event only */
  /* UTC timestamp in secs from epoch to sync the ADSP clock */
  p_iod_autocal_enable_req_msg->timeofday = q_utc_time_secs;
  
  /*  Sunset and Sunrise Timestamps */
  p_iod_autocal_enable_req_msg->sunset_sunrise_ts.sunset_ts  = q_sunsettime_sec;
  p_iod_autocal_enable_req_msg->sunset_sunrise_ts.sunrise_ts = q_sunrisetime_sec;
  
  /*  indications for this request should not be sent when the processor is in 
   *  suspend state */
  p_iod_autocal_enable_req_msg->notify_suspend_valid = FALSE;
  
  retval = qmi_client_send_msg_async(loc_geofence_iod_autocal_server_handle, 
                                     SNS_SAM_IOD_AUTOCAL_ENABLE_REQ_V01, 
                                     p_iod_autocal_enable_req_msg, 
                                     sizeof(*p_iod_autocal_enable_req_msg),
                                     p_iod_autocal_enable_resp_msg, 
                                     sizeof(*p_iod_autocal_enable_resp_msg), 
                                     loc_geofence_sensor_iod_autocal_cb, (void *)2, &txn);
  
  if (retval != 0){
    SHIM_MSG_ERROR("loc_geofence_sensor_start_iod: send_msg_async error: %d\n",retval, 0,0);    
    os_MemFree((void**)&p_iod_autocal_enable_req_msg);    
    os_MemFree((void**)&p_iod_autocal_enable_resp_msg);
    return FALSE;
  }

  
  os_MemFree((void**)&p_iod_autocal_enable_req_msg);    
  return TRUE;
}

/** loc_geofence_sensor_iod_get_service
 *    Handles IOD Get service request. This function
      will find and get the IOD service handle.
 * */
boolean loc_geofence_sensor_iod_get_service(void)
{
  qmi_client_error_type         retval;  
  qmi_service_info              info[10];
  unsigned int   num_services = 0, 
                 num_entries = 1;

  if(loc_geofence_iod_server_handle != NULL)
  {
    SHIM_MSG_MED("IOD Serice already discovered\n", 0,0,0);
    return TRUE;
  }

  /*-----------------------------------------------------------------------
     Get Sensor service object
  -----------------------------------------------------------------------*/
  loc_geofence_iod_sensor_svc_obj = SNS_SAM_IOD_SVC_get_service_object_v01();
  if(!loc_geofence_iod_sensor_svc_obj)
  {
    SHIM_MSG_ERROR("loc_geofence_iod_sensor_svc_obj(): Unable to get"
                  " Sensor service object",0,0,0);
    return FALSE;
  }

  /*------------------------------------------------------------------
     Check if the IOD service is up, if not return. Else,
     Get addressing information for accessing wds service
  ------------------------------------------------------------------*/  
  retval = qmi_client_get_service_list(loc_geofence_iod_sensor_svc_obj,
                                       info,
                                       &num_entries,
                                       &num_services);
                                       
  if(QMI_NO_ERR != retval)
  {
    SHIM_MSG_ERROR("Could not Get Service list ERROR %d", retval, 0, 0);
    return FALSE;
  }

  SHIM_MSG_MED("loc_geofence_sensor_iod_get_service(): IOD Service UP"
               " num_entries = %d and num_services = %d",
               num_entries, num_services, 0);

  memset(&loc_geofence_iod_sensor_notifier_os_params, 
          0,
          sizeof(loc_geofence_iod_sensor_notifier_os_params));

  loc_geofence_iod_sensor_notifier_os_params.tcb = rex_self();
  loc_geofence_iod_sensor_notifier_os_params.sig = LOC_SENSOR_SVC_IND_EVENT;     
  
  retval = qmi_client_init(&info[0],
                      loc_geofence_iod_sensor_svc_obj,
                      loc_geofence_iod_handle_indicator,
                      NULL, 
                      &loc_geofence_iod_sensor_notifier_os_params,
                      &loc_geofence_iod_server_handle);
  if(retval != QMI_NO_ERR)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_iod_get_service(): Client Init failed "
                  "err %d", retval, 0, 0);     
    return FALSE;
  }

  return TRUE;
}


/** loc_geofence_sensor_start_iod
 *    Handles IOD Start request from Geofence module. 
 *  @param [in] report_period
 * */
boolean loc_geofence_sensor_start_iod
(
  uint32 utc_ts,
  uint32 sunrise_ts,
  uint32 sunset_ts
)
{  
  qmi_client_error_type            retval;  
  sns_sam_iod_enable_req_msg_v01  *p_iod_enable_req_msg = NULL;
  sns_sam_iod_enable_resp_msg_v01 *p_iod_enable_resp_msg = NULL;
  qmi_txn_handle txn;
  uint32         index;

  if(loc_geofence_iod_server_handle == NULL)
  {
    if(loc_geofence_sensor_iod_get_service() == FALSE)
    {      
      SHIM_MSG_ERROR("loc_geofence_sensor_start_iod: Unable to get IOD Sensor Service\n",0, 0,0);
      return FALSE;
    }
  }
  
  p_iod_enable_req_msg = os_MemAlloc(sizeof(*p_iod_enable_req_msg), OS_MEM_SCOPE_PROCESSOR);
  if(p_iod_enable_req_msg == NULL)
  {    
    SHIM_MSG_ERROR("loc_geofence_sensor_start_iod  Unable to allocate memory for Enable req",0,0,0);
    return FALSE;
  }
  memset((void*)p_iod_enable_req_msg, 0, sizeof(*p_iod_enable_req_msg));


  p_iod_enable_resp_msg = os_MemAlloc(sizeof(*p_iod_enable_resp_msg), OS_MEM_SCOPE_PROCESSOR);
  if(p_iod_enable_resp_msg == NULL)
  {    
    SHIM_MSG_ERROR("loc_geofence_sensor_start_iod  Unable to allocate memory for Enable resp",0,0,0);
    os_MemFree((void**)&p_iod_enable_req_msg);
    return FALSE;
  }
  memset((void*)p_iod_enable_resp_msg, 0, sizeof(*p_iod_enable_resp_msg));

  p_iod_enable_req_msg->report_period = 10;
  p_iod_enable_req_msg->timeofday = utc_ts;  
  p_iod_enable_req_msg->sunset_sunrise_ts.sunset_ts = sunset_ts;
  p_iod_enable_req_msg->sunset_sunrise_ts.sunrise_ts = sunrise_ts;  
  p_iod_enable_req_msg->notify_suspend_valid = TRUE;
  p_iod_enable_req_msg->notify_suspend.proc_type = SNS_PROC_MODEM_V01;
  p_iod_enable_req_msg->notify_suspend.send_indications_during_suspend = TRUE;

  #if 0
  p_iod_enable_req_msg->iod_sources_allowed_valid = TRUE;
  p_iod_enable_req_msg->iod_sources_allowed = (IOD_SOURCE_ALS_MASK_V01 | 
                                               IOD_SOURCE_PROXIMITY_MASK_V01);

  if((b_conf_threshold_valid) == TRUE && 
     (u_num_conf_thresholds == IOD_OUTPUT_CLASS_MAX_V01))
  {
    p_iod_enable_req_msg->confidence_threshold_valid = TRUE;
    for(index = 0; index < IOD_OUTPUT_CLASS_MAX_V01; index++)
    {
      p_iod_enable_req_msg->confidence_threshold[index] = f_conf_threshold[index];
    }
  }
  else
  {
    p_iod_enable_req_msg->confidence_threshold_valid = FALSE;
  }
  
  p_iod_enable_req_msg->weather_forecast_valid;
  p_iod_enable_req_msg->weather_forecast;
  p_iod_enable_req_msg->local_timezone_offset_valid;
  p_iod_enable_req_msg->local_timezone_offset;
  #endif
  retval = qmi_client_send_msg_async(loc_geofence_iod_server_handle, 
                                     SNS_SAM_IOD_ENABLE_REQ_V01, 
                                     p_iod_enable_req_msg, 
                                     sizeof(*p_iod_enable_req_msg),
                                     p_iod_enable_resp_msg, 
                                     sizeof(*p_iod_enable_resp_msg), 
                                     loc_geofence_sensor_iod_cb, (void *)2, &txn);
  
  if (retval != 0){
    SHIM_MSG_ERROR("loc_geofence_sensor_start_iod: send_msg_async error: %d\n",retval, 0,0);    
    os_MemFree((void**)&p_iod_enable_req_msg);    
    os_MemFree((void**)&p_iod_enable_resp_msg);
    return FALSE;
  }

  
  os_MemFree((void**)&p_iod_enable_req_msg);    
  return TRUE;
}


/** loc_geofence_sensor_stop_iod
 *    Handles IOD Stop request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_stop_iod
(
  uint8 u_instance_id_valid,
  uint8 u_instance_id
)
{
  qmi_client_error_type             retval;  
  sns_sam_iod_disable_req_msg_v01  *p_iod_disable_req_msg = NULL;
  sns_sam_iod_disable_resp_msg_v01 *p_iod_disable_resp_msg = NULL;
  qmi_txn_handle txn;

  SHIM_MSG_MED("Stop ALS IOD sensor", 0, 0, 0);
  if(loc_geofence_iod_server_handle == NULL)
  {
    SHIM_MSG_HIGH("loc_geofence_sensor_disable_iod: Client pointer NULL\n",0, 0,0);
    /* No client to stop, therefore stop command worked! */
    return TRUE;
  }

  p_iod_disable_req_msg = os_MemAlloc(sizeof(*p_iod_disable_req_msg),
                                      OS_MEM_SCOPE_PROCESSOR);
  if(p_iod_disable_req_msg == NULL)
  {    
    SHIM_MSG_ERROR("loc_geofence_sensor_stop_iod  Unable to allocate memory "
                   "for disable req",0,0,0);
    return FALSE;
  }
  memset((void*)p_iod_disable_req_msg, 0, sizeof(*p_iod_disable_req_msg));

  if(u_instance_id_valid)
  {
    p_iod_disable_req_msg->instance_id = u_instance_id;
  }
  p_iod_disable_resp_msg = os_MemAlloc(sizeof(*p_iod_disable_resp_msg),
                                       OS_MEM_SCOPE_PROCESSOR);
  if(p_iod_disable_resp_msg == NULL)
  {    
    SHIM_MSG_ERROR("loc_geofence_sensor_disable_iod  Unable to allocate memory for Enable resp",0,0,0);
    os_MemFree((void**)&p_iod_disable_req_msg);
    return FALSE;
  }
  memset((void*)p_iod_disable_resp_msg, 0, sizeof(*p_iod_disable_resp_msg));

  retval = qmi_client_send_msg_async(loc_geofence_iod_server_handle, 
                                     SNS_SAM_IOD_DISABLE_REQ_V01, 
                                     p_iod_disable_req_msg, 
                                     sizeof(*p_iod_disable_req_msg),
                                     p_iod_disable_resp_msg, 
                                     sizeof(*p_iod_disable_resp_msg), 
                                     loc_geofence_sensor_iod_cb, (void *)2, &txn);

  
  if (retval != 0){
    SHIM_MSG_MED("loc_geofence_sensor_disable_iod: send_msg_async error: %d\n",retval, 0,0);
    os_MemFree((void**)&p_iod_disable_req_msg);    
    os_MemFree((void**)&p_iod_disable_resp_msg);    
    return FALSE;
  }

  
  os_MemFree((void**)&p_iod_disable_req_msg);    
  return TRUE;
}

/** loc_geofence_configure_iod
 *    Handles IOD Configure request from Geofence module. 
 *  @param [in] p_config_iod
 * */
boolean loc_geofence_configure_iod(sns_sam_iod_update_req_msg_v01 *p_config_iod)
{
  qmi_txn_handle         txn;
  qmi_client_error_type  retval;  
  sns_sam_iod_update_resp_msg_v01 *p_iod_config_resp_msg = NULL;

  if(loc_geofence_iod_server_handle == NULL)
  {
    SHIM_MSG_ERROR("loc_geofence_configure_iod: Client pointer NULL\n",0, 0,0);
    return FALSE;
  }

  if(p_config_iod == NULL)
  {    
    SHIM_MSG_ERROR("loc_geofence_configure_iod  Null config req", 0, 0, 0);
    return FALSE;
  }

  p_iod_config_resp_msg = os_MemAlloc(sizeof(*p_iod_config_resp_msg),
                                      OS_MEM_SCOPE_PROCESSOR);
  if(p_iod_config_resp_msg == NULL)
  {    
    SHIM_MSG_ERROR("loc_geofence_configure_iod No memory for config resp",
                    0, 0, 0);
    return FALSE;
  }
  memset((void*)p_iod_config_resp_msg, 0, sizeof(*p_iod_config_resp_msg));

  retval = qmi_client_send_msg_async(loc_geofence_iod_server_handle, 
                                     SNS_SAM_IOD_UPDATE_REQ_V01, 
                                     p_config_iod, 
                                     sizeof(*p_config_iod),
                                     p_iod_config_resp_msg, 
                                     sizeof(*p_iod_config_resp_msg), 
                                     loc_geofence_sensor_iod_cb, (void *)2, &txn);

  
  if (retval != 0){
    SHIM_MSG_MED("loc_geofence_configure_iod: send_msg_async error: %d\n",retval, 0,0);
    os_MemFree((void**)&p_iod_config_resp_msg);    
    return FALSE;
  }

  return TRUE;
}

#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/


