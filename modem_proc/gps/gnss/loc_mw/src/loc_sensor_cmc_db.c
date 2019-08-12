/*============================================================================
 @file loc_geofence_sensor_cmc_db.c

 loc MW sensor client implementation

 GENERAL DESCRIPTION

 This file contains the loc middleware sensor client implementation.

 Copyright (c) 2011-2013 Qualcomm Technologies Incorporated.
 All Rights Reserved
 Qualcomm Confidential and Proprietary

 Copyright (c) 2014 Qualcomm Atheros, Inc.
 All Rights Reserved. 
 Qualcomm Atheros Confidential and Proprietary.
 
 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_sensor_cmc_db.c#1 $
 $DateTime: 2016/12/13 07:59:45 $
 $Author: mplcsds1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 3/31/14     ss     Supporting floats for speed
 11/29/12   ss      Initial version

=============================================================================*/
#include "customer.h"
  
#if (defined(FEATURE_GEOFENCE_SENSOR_SUPPORT) && (!defined(FEATURE_SMART_OFFLINE_COMPILATION)))
#include "msg.h"
#include "aries_os_api.h"
#include "loc_sensor_cmc_db.h"

#include "qmi_cci_target_ext.h"
#include "qmi_client.h"

#include "gm_api.h"




#define LOC_SENSOR_SVC_IND_EVENT C_OS_FLAG_QMI_CCI_EVENT
/*No need to handle the above event as we do not use QMI notify init and use
QMI client init directly. But still the event is required for internal QMI purpose which 
QMI team uses to block our thread for flow control. No handling on our side is necessary*/


/*---------------------------------------------------------------------------
   Notifier OS param
---------------------------------------------------------------------------*/
static qmi_client_os_params          loc_geofence_sensor_cmc_db_notifier_os_params;
static qmi_client_type loc_geofence_sensor_cmc_db_server_handle = NULL;
static qmi_idl_service_object_type loc_geofence_sensor_cmc_db_sensor_svc_obj=NULL;

static uint8_t loc_geofence_sensor_cmc_db_instance_id;
static boolean loc_geofence_sensor_cmc_db_instance_id_valid = FALSE;


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


/** loc_geofence_sensor_cmc_db_handle_enable_resp
 *  Handle the CMC Enable Resp from ADSP. This
 *  should be invoked when CMC Enable request was 
     sent to ADSP previosuly.
 *  @param [in] enable_resp_p
 * */
void loc_geofence_sensor_cmc_db_handle_enable_resp(sns_sam_distance_bound_enable_resp_msg_v01 *enable_resp_p)
{
  loc_geofence_cmc_db_ind cmc_db_indicaiton;

  if(enable_resp_p == NULL)
  {
  SHIM_MSG_ERROR("CMC DB Enable Response pointer NULL",0,0,0);
  return;
  }
  else if(enable_resp_p->resp.sns_result_t == 0)
  {
  SHIM_MSG_HIGH("CMC DB Enable Success Instace Id %d in Response",enable_resp_p->instance_id,0,0);  
  loc_geofence_sensor_cmc_db_instance_id = enable_resp_p->instance_id;
  loc_geofence_sensor_cmc_db_instance_id_valid = TRUE;

  }
  else
  {
  SHIM_MSG_ERROR("CMC DB Enable Failure %d in Response",enable_resp_p->resp.sns_err_t,0,0);
  }

  memset((void*)&cmc_db_indicaiton, 0, sizeof(cmc_db_indicaiton)); 
  cmc_db_indicaiton.cmc_db_indication_type = SNS_SAM_DISTANCE_BOUND_ENABLE_RESP_V01;
  memscpy((void*)&cmc_db_indicaiton.u.cmc_db_start_resp_ind, sizeof(cmc_db_indicaiton.u.cmc_db_start_resp_ind),
  	(void*)enable_resp_p, sizeof(*enable_resp_p));

  SHIM_MSG_HIGH("Size of CMC DB = %d", sizeof(cmc_db_indicaiton),0,0);

  gm_inject_cmc_db_indication(&cmc_db_indicaiton);
}

/** loc_geofence_sensor_cmc_db_handle_disable_resp
 *  Handle the CMC Disable Resp from ADSP. This
 *  should be invoked when CMC Disable request was 
     sent to ADSP previosuly.
 *  @param [in] disable_resp_p
 * */
void loc_geofence_sensor_cmc_db_handle_disable_resp(sns_sam_distance_bound_disable_resp_msg_v01 *disable_resp_p)
{  
  loc_geofence_cmc_db_ind cmc_db_indicaiton;

  if(disable_resp_p == NULL)
  {
  SHIM_MSG_ERROR("CMC DB Disable Response pointer NULL",0,0,0);
  return;
  }
  else if(disable_resp_p->resp.sns_result_t == 0)
  {
   SHIM_MSG_HIGH("CMC DB Disable Success Instace Id %d in Response",disable_resp_p->instance_id,0,0);
  }
  else
  {
  SHIM_MSG_ERROR("CMC DB Disable Failure %d in Response",disable_resp_p->resp.sns_err_t,0,0);
  } 
  memset((void*)&cmc_db_indicaiton, 0, sizeof(cmc_db_indicaiton)); 
  cmc_db_indicaiton.cmc_db_indication_type = SNS_SAM_DISTANCE_BOUND_DISABLE_RESP_V01;
  memscpy((void*)&cmc_db_indicaiton.u.cmc_db_stop_resp_ind, sizeof(cmc_db_indicaiton.u.cmc_db_stop_resp_ind),
  	(void*)disable_resp_p, sizeof(*disable_resp_p));

  gm_inject_cmc_db_indication(&cmc_db_indicaiton);
}



/** loc_geofence_sensor_cmc_db_handle_async_report
 *  Handle the CMC Async Report from ADSP. This carries
     ADSP report indications
 *  @param [in] report_ind_p
 * */
void loc_geofence_sensor_cmc_db_handle_async_report(sns_sam_distance_bound_report_ind_msg_v01 *report_ind_p)
{  
  loc_geofence_cmc_db_ind cmc_db_indicaiton;

  if(report_ind_p == NULL)
  {
    SHIM_MSG_ERROR("CMC DB Report Indication pointer NULL",0,0,0);
    return;  
  }
  
  SHIM_MSG_HIGH("CMC DB Report Indication Instace Id %d ",report_ind_p->instance_id,0,0);
  SHIM_MSG_HIGH("CMC DB Report Indication Timestamp %d ",report_ind_p->timestamp,0,0);

  SHIM_MSG_HIGH("CMC DB Report State %d ",report_ind_p->report_data.distance_bound_breach_event,0,0);

  memset((void*)&cmc_db_indicaiton, 0, sizeof(cmc_db_indicaiton)); 
  cmc_db_indicaiton.cmc_db_indication_type = SNS_SAM_DISTANCE_BOUND_REPORT_IND_V01;
  memscpy((void*)&cmc_db_indicaiton.u.cmc_db_report_ind, sizeof(cmc_db_indicaiton.u.cmc_db_report_ind),
  	(void*)report_ind_p, sizeof(*report_ind_p));

  SHIM_MSG_HIGH("Size of CMC DB = %d", sizeof(cmc_db_indicaiton),0,0);

  gm_inject_cmc_db_indication(&cmc_db_indicaiton);
  
}

/** loc_geofence_sensor_cmc_db_handle_err_ind
 *    Handle the CMC Error Indication from ADSP.
 *  @param [in] err_ind_p
 * */
void loc_geofence_sensor_cmc_db_handle_err_ind(sns_sam_distance_bound_error_ind_msg_v01 *err_ind_p)
{
  loc_geofence_cmc_db_ind cmc_db_indicaiton;

  if(err_ind_p == NULL)
  {
    SHIM_MSG_ERROR("CMC DB Error Indication pointer NULL",0,0,0);
    return;  
  }

  SHIM_MSG_HIGH("CMC DB Error Indication Instace Id %d ",err_ind_p->instance_id,0,0);
  SHIM_MSG_HIGH("CMC DB Error Indication Error %d ",err_ind_p->error,0,0);
  
  memset((void*)&cmc_db_indicaiton, 0, sizeof(cmc_db_indicaiton)); 
  cmc_db_indicaiton.cmc_db_indication_type = SNS_SAM_DISTANCE_BOUND_ERROR_IND_V01;
  memscpy((void*)&cmc_db_indicaiton.u.cmc_db_err_ind, sizeof(cmc_db_indicaiton.u.cmc_db_err_ind),
  	(void*)err_ind_p, sizeof(*err_ind_p));

  gm_inject_cmc_db_indication(&cmc_db_indicaiton);

}

/** loc_geofence_sensor_cmc_db_handle_set_bound_resp
 *    Handle the CMC Set Bound Resp from ADSP.
 *  @param [in] set_bound_resp
 * */
void loc_geofence_sensor_cmc_db_handle_set_bound_resp(sns_sam_distance_bound_set_bound_resp_msg_v01 *set_bound_resp)
{
  loc_geofence_cmc_db_ind cmc_db_indicaiton;

  if(set_bound_resp == NULL)
  {
    SHIM_MSG_ERROR("CMC DB Set Bound Response pointer NULL",0,0,0);
    return;  
  }

  SHIM_MSG_HIGH("CMC DB Set Bound Response  Instance Id %d ",set_bound_resp->instance_id,0,0);
  
  memset((void*)&cmc_db_indicaiton, 0, sizeof(cmc_db_indicaiton)); 
  cmc_db_indicaiton.cmc_db_indication_type = SNS_SAM_DISTANCE_BOUND_SET_BOUND_RESP_V01;
  memscpy((void*)&cmc_db_indicaiton.u.cmc_db_set_bound_resp_ind, sizeof(cmc_db_indicaiton.u.cmc_db_set_bound_resp_ind),
  	(void*)set_bound_resp, sizeof(*set_bound_resp));

  gm_inject_cmc_db_indication(&cmc_db_indicaiton);
}

/** loc_geofence_sensor_cmc_db_handle_get_report_resp
 *    Handle the CMC Get Report Resp from ADSP.
 *  @param [in] set_bound_resp
 * */
void loc_geofence_sensor_cmc_db_handle_get_report_resp(sns_sam_distance_bound_get_report_resp_msg_v01 *get_report_resp)
{
  loc_geofence_cmc_db_ind cmc_db_indicaiton;

  if(get_report_resp == NULL)
  {
    SHIM_MSG_ERROR("CMC DB Get Report Response pointer NULL",0,0,0);
    return;  
  }

  SHIM_MSG_HIGH("CMC DB Get Report Response Instance Id %d ",get_report_resp->instance_id,0,0);
  
  memset((void*)&cmc_db_indicaiton, 0, sizeof(cmc_db_indicaiton)); 
  cmc_db_indicaiton.cmc_db_indication_type = SNS_SAM_DISTANCE_BOUND_GET_REPORT_RESP_V01;
  memscpy((void*)&cmc_db_indicaiton.u.cmc_db_get_report_resp_ind, sizeof(cmc_db_indicaiton.u.cmc_db_get_report_resp_ind),
  	(void*)get_report_resp, sizeof(*get_report_resp));

  gm_inject_cmc_db_indication(&cmc_db_indicaiton);
}

/** loc_geofence_sensor_cmc_db_cb
 *    CMC callback registered when enabling CMC.
 * */
void loc_geofence_sensor_cmc_db_cb
(
 qmi_client_type                user_handle,
 unsigned int                  msg_id,
 void                           *buf,
 unsigned int                   len,
 void                           *resp_cb_data,
 qmi_client_error_type          transp_err
 )
{
  if(buf == NULL)
  {  
    SHIM_MSG_ERROR("NULL Buffer pointer in loc_geofence_sensor_cmc_db_cb",0,0,0);
    return;
  }
  /* Print the appropriate message based on the message ID */
  switch (msg_id)
  {
    case SNS_SAM_DISTANCE_BOUND_ENABLE_RESP_V01:
      SHIM_MSG_HIGH("loc_geofence_sensor_cmc_db_cb: CMC DB Enable Response\n",0,0,0);
      loc_geofence_sensor_cmc_db_handle_enable_resp((sns_sam_distance_bound_enable_resp_msg_v01*)buf);
      break;
    case SNS_SAM_DISTANCE_BOUND_DISABLE_RESP_V01:
      SHIM_MSG_HIGH("loc_geofence_sensor_cmc_db_cb: CMC DB Disable Response\n",0,0,0);
      loc_geofence_sensor_cmc_db_handle_disable_resp((sns_sam_distance_bound_disable_resp_msg_v01*)buf);        
      break;      
    case SNS_SAM_DISTANCE_BOUND_ERROR_IND_V01:
      SHIM_MSG_HIGH("loc_geofence_sensor_cmc_db_cb: CMC DB Error Indication\n",0,0,0);
      loc_geofence_sensor_cmc_db_handle_err_ind((sns_sam_distance_bound_error_ind_msg_v01*)buf);
      break;  
    case SNS_SAM_DISTANCE_BOUND_SET_BOUND_RESP_V01:
      SHIM_MSG_HIGH("loc_geofence_sensor_cmc_db_cb: CMC DB Set Bound Response\n",0,0,0);
      loc_geofence_sensor_cmc_db_handle_set_bound_resp((sns_sam_distance_bound_set_bound_resp_msg_v01*)buf);      
      break;
      case SNS_SAM_DISTANCE_BOUND_GET_REPORT_RESP_V01:
        SHIM_MSG_HIGH("loc_geofence_sensor_cmc_db_cb: CMC DB Get Report Response\n",0,0,0);
        loc_geofence_sensor_cmc_db_handle_get_report_resp((sns_sam_distance_bound_get_report_resp_msg_v01*)buf);      
        break;      
    default:
      break;
  }  
  os_MemFree((void**)&buf);
}

/** loc_geofence_sensor_cmc_db_set_bound
 *    Handles CMC Set Bound request from Geofence module 
 *  @param [in] distance_to_check
 * */
boolean loc_geofence_sensor_cmc_db_set_bound(float distance_to_check)
{  
  qmi_client_error_type         retval;  
  sns_sam_distance_bound_set_bound_req_msg_v01 *cmc_db_set_bound_req_msg = NULL;
  sns_sam_distance_bound_set_bound_resp_msg_v01 *cmc_db_set_bound_resp_msg = NULL;
  qmi_txn_handle txn;

  if( (loc_geofence_sensor_cmc_db_server_handle == NULL) || (loc_geofence_sensor_cmc_db_instance_id_valid == FALSE)) 
  {    
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_set_bound server handle or instance_id invalid",0,0,0);
    return FALSE;
  }

  cmc_db_set_bound_req_msg = os_MemAlloc(sizeof(sns_sam_distance_bound_set_bound_req_msg_v01),OS_MEM_SCOPE_PROCESSOR);
  if(cmc_db_set_bound_req_msg == NULL)
  {    
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_set_bound server Unable to allocate memory for set bound req",0,0,0);
    return FALSE;
  }
  memset((void*)cmc_db_set_bound_req_msg, 0, sizeof(sns_sam_distance_bound_set_bound_req_msg_v01));


  cmc_db_set_bound_resp_msg = os_MemAlloc(sizeof(sns_sam_distance_bound_set_bound_resp_msg_v01),OS_MEM_SCOPE_PROCESSOR);
  if(cmc_db_set_bound_resp_msg == NULL)
  {    
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_set_bound server Unable to allocate memory for set bound resp",0,0,0);
    os_MemFree((void**)&cmc_db_set_bound_req_msg);
    return FALSE;
  }
  memset((void*)cmc_db_set_bound_resp_msg, 0, sizeof(sns_sam_distance_bound_set_bound_resp_msg_v01));
  

  cmc_db_set_bound_req_msg->instance_id= loc_geofence_sensor_cmc_db_instance_id;
  cmc_db_set_bound_req_msg->distance_bound = distance_to_check;
  cmc_db_set_bound_req_msg->session_key = 0;

  retval = qmi_client_send_msg_async(loc_geofence_sensor_cmc_db_server_handle, 
                                     SNS_SAM_DISTANCE_BOUND_SET_BOUND_REQ_V01, 
                                     cmc_db_set_bound_req_msg, 
                                     sizeof(sns_sam_distance_bound_set_bound_req_msg_v01),
                                     cmc_db_set_bound_resp_msg, 
                                     sizeof(sns_sam_distance_bound_set_bound_resp_msg_v01), 
                                     loc_geofence_sensor_cmc_db_cb, (void *)2, &txn);

  SHIM_MSG_MED("loc_geofence_sensor_cmc_db_set_bound: qmi_client_send_msg_async returned %d\n", retval,0,0);
  
  if (retval != 0){
    SHIM_MSG_MED("loc_geofence_sensor_cmc_db_set_bound: send_msg_async error: %d\n",retval, 0,0);    
    os_MemFree((void**)&cmc_db_set_bound_req_msg);    
    os_MemFree((void**)&cmc_db_set_bound_resp_msg);
    return FALSE;
  }

  
  os_MemFree((void**)&cmc_db_set_bound_req_msg);    
  return TRUE;
}

/** loc_geofence_sensor_cmc_db_get_report
 *    Handles CMC Get Report request from Geofence module 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_cmc_db_get_report(void)
{  
  qmi_client_error_type         retval;  
  sns_sam_distance_bound_get_report_req_msg_v01 *cmc_db_get_report_req_msg = NULL;
  sns_sam_distance_bound_get_report_resp_msg_v01 *cmc_db_get_report_resp_msg = NULL;
  qmi_txn_handle txn;

  if( (loc_geofence_sensor_cmc_db_server_handle == NULL) || (loc_geofence_sensor_cmc_db_instance_id_valid == FALSE)) 
  {    
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_get_report server handle or instance_id invalid",0,0,0);
    return FALSE;
  }

  cmc_db_get_report_req_msg = os_MemAlloc(sizeof(sns_sam_distance_bound_get_report_req_msg_v01),OS_MEM_SCOPE_PROCESSOR);
  if(cmc_db_get_report_req_msg == NULL)
  {    
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_get_report server Unable to allocate memory for get report req",0,0,0);
    return FALSE;
  }
  memset((void*)cmc_db_get_report_req_msg, 0, sizeof(sns_sam_distance_bound_get_report_req_msg_v01));


  cmc_db_get_report_resp_msg = os_MemAlloc(sizeof(sns_sam_distance_bound_get_report_resp_msg_v01),OS_MEM_SCOPE_PROCESSOR);
  if(cmc_db_get_report_resp_msg == NULL)
  {    
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_get_report server Unable to allocate memory for get report resp",0,0,0);
    os_MemFree((void**)&cmc_db_get_report_req_msg);
    return FALSE;
  }
  memset((void*)cmc_db_get_report_resp_msg, 0, sizeof(sns_sam_distance_bound_get_report_resp_msg_v01));
  

  cmc_db_get_report_req_msg->instance_id= loc_geofence_sensor_cmc_db_instance_id;

  retval = qmi_client_send_msg_async(loc_geofence_sensor_cmc_db_server_handle, 
                                     SNS_SAM_DISTANCE_BOUND_GET_REPORT_REQ_V01, 
                                     cmc_db_get_report_req_msg, 
                                     sizeof(sns_sam_distance_bound_get_report_req_msg_v01),
                                     cmc_db_get_report_resp_msg, 
                                     sizeof(sns_sam_distance_bound_get_report_resp_msg_v01), 
                                     loc_geofence_sensor_cmc_db_cb, (void *)2, &txn);

  SHIM_MSG_MED("loc_geofence_sensor_cmc_db_get_report: qmi_client_send_msg_async returned %d\n", retval,0,0);
  
  if (retval != 0){
    SHIM_MSG_MED("loc_geofence_sensor_cmc_db_get_report: send_msg_async error: %d\n",retval, 0,0);    
    os_MemFree((void**)&cmc_db_get_report_req_msg);    
    os_MemFree((void**)&cmc_db_get_report_resp_msg);
    return FALSE;
  }

  
  os_MemFree((void**)&cmc_db_get_report_req_msg);    
  return TRUE;
}

/** loc_geofence_sensor_cmc_db_handle_indicator
 *    Handles CMC indications from ADSP
 * */
static void loc_geofence_sensor_cmc_db_handle_indicator
(
 qmi_client_type                user_handle,
 unsigned int                  msg_id,
 void                          *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
  qmi_client_error_type         retval;  
  sns_sam_distance_bound_report_ind_msg_v01 cmc_db_report_ind;

  SHIM_MSG_MED("loc_geofence_sensor_cmc_db_handle_indicator Msg Id = %d",msg_id, 0,0);

  if(msg_id == SNS_SAM_DISTANCE_BOUND_REPORT_IND_V01)
  {
    if((ind_buf == NULL) || (ind_buf_len == 0))
    {      
      SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_handle_indicator: Buff NULL\n",0, 0,0);
      return;
    }

    
    retval = qmi_client_message_decode(loc_geofence_sensor_cmc_db_server_handle,
                                   QMI_IDL_INDICATION,
                                   SNS_SAM_DISTANCE_BOUND_REPORT_IND_V01,
                                   (void *)ind_buf,
                                   ind_buf_len,
                                   (void *)&cmc_db_report_ind,
                                   sizeof(sns_sam_distance_bound_report_ind_msg_v01));

    
    if (retval == QMI_NO_ERR)
    {
      loc_geofence_sensor_cmc_db_handle_async_report((sns_sam_distance_bound_report_ind_msg_v01*)&cmc_db_report_ind);
    }
    else
    {
      SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_handle_indicator: Message Decode Failed with Error %d\n",retval, 0,0);
    }
  }
}

/** loc_geofence_sensor_cmc_db_get_service
 *    Handles CMC Get service request. This funciton
      will find and get the CMC service handle.
 * */
boolean loc_geofence_sensor_cmc_db_get_service(void)
{
  qmi_client_error_type         retval;  
  qmi_service_info              info[10];
  unsigned int num_services = 0, num_entries = 1;    

  /*-----------------------------------------------------------------------
     Get Sensor service object
  -----------------------------------------------------------------------*/
  if(loc_geofence_sensor_cmc_db_server_handle != NULL)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_get_service(): Unable to get"
                  " Sensor service object",0,0,0);
    return FALSE;
  }

  /*-----------------------------------------------------------------------
     Get Sensor service object
  -----------------------------------------------------------------------*/
  loc_geofence_sensor_cmc_db_sensor_svc_obj = SNS_SAM_DISTANCE_BOUND_SVC_get_service_object_v01();
  if(!loc_geofence_sensor_cmc_db_sensor_svc_obj)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_handle_event(): Unable to get"
                  " Sensor service object",0,0,0);
    return FALSE;
  }

  /*------------------------------------------------------------------
     Check if the Sensor  service is up, if not return. Else,
     Get addressing information for accessing wds service
  ------------------------------------------------------------------*/  
  retval = qmi_client_get_service_list(loc_geofence_sensor_cmc_db_sensor_svc_obj,
                                       info,
                                       &num_entries,
                                       &num_services);
                                       
  if(QMI_NO_ERR != retval)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_handle_event(): Get "
                  "Service list err %d", retval, 0, 0);
    return FALSE;
  }

  SHIM_MSG_MED("loc_geofence_sensor_cmc_db_handle_event(): Sensor Service UP  num_entries = %d and num_services = %d",num_entries,num_services,0);

  memset(&loc_geofence_sensor_cmc_db_notifier_os_params,0,sizeof(qmi_cci_os_signal_type));
  loc_geofence_sensor_cmc_db_notifier_os_params.tcb = rex_self();
  loc_geofence_sensor_cmc_db_notifier_os_params.sig = LOC_SENSOR_SVC_IND_EVENT;     
  
  retval = qmi_client_init(&info[0],
                       loc_geofence_sensor_cmc_db_sensor_svc_obj,
                       loc_geofence_sensor_cmc_db_handle_indicator,
                       NULL, 
                       &loc_geofence_sensor_cmc_db_notifier_os_params,
                       &loc_geofence_sensor_cmc_db_server_handle);
  if(retval != QMI_NO_ERR)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_handle_event(): Client Init failed "
                  "err %d", retval, 0, 0);     
     return FALSE;
  }

  return TRUE;
}

/** loc_geofence_sensor_start_cmc_db
 *    Handles CMC Start request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_start_cmc_db(loc_geofence_cmc_db_state_speed_bound *p_speed_bounds)
{  
  qmi_client_error_type         retval;  
  sns_sam_distance_bound_enable_req_msg_v01 *cmc_db_enable_req_msg = NULL;
  sns_sam_distance_bound_enable_resp_msg_v01 *cmc_db_enable_resp_msg = NULL;
  qmi_txn_handle txn;

  if(loc_geofence_sensor_cmc_db_server_handle == NULL)
  {
    if(loc_geofence_sensor_cmc_db_get_service() == FALSE)
    {      
      SHIM_MSG_ERROR("loc_geofence_sensor_start_amd: Unable to get AMD Sensor Service\n",0, 0,0);
      return FALSE;
    }
  }
  cmc_db_enable_req_msg = os_MemAlloc(sizeof(sns_sam_distance_bound_enable_req_msg_v01),OS_MEM_SCOPE_PROCESSOR);
  if(cmc_db_enable_req_msg == NULL)
  {    
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_set_bound server Unable to allocate memory for set bound req",0,0,0);
    return FALSE;
  }
  memset((void*)cmc_db_enable_req_msg, 0, sizeof(sns_sam_distance_bound_enable_req_msg_v01));


  cmc_db_enable_resp_msg = os_MemAlloc(sizeof(sns_sam_distance_bound_enable_resp_msg_v01),OS_MEM_SCOPE_PROCESSOR);
  if(cmc_db_enable_resp_msg == NULL)
  {    
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_set_bound server Unable to allocate memory for set bound resp",0,0,0);
    os_MemFree((void**)&cmc_db_enable_req_msg);
    return FALSE;
  }
  memset((void*)cmc_db_enable_resp_msg, 0, sizeof(sns_sam_distance_bound_enable_resp_msg_v01));

  cmc_db_enable_req_msg->motion_state_speed_bounds_valid = TRUE;
  cmc_db_enable_req_msg->motion_state_speed_bounds_len = 8;
  cmc_db_enable_req_msg->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_UNKNOWN_V01] = p_speed_bounds->cmc_db_unknwon_speed_bound;
  cmc_db_enable_req_msg->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_STATIONARY_V01] = p_speed_bounds->cmc_db_stationary_speed_bound;
  cmc_db_enable_req_msg->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_INMOTION_V01] = p_speed_bounds->cmc_db_in_motion_speed_bound;
  cmc_db_enable_req_msg->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_FIDDLE_V01] = p_speed_bounds->cmc_db_fiddle_speed_bound;
  cmc_db_enable_req_msg->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_PEDESTRIAN_V01] = p_speed_bounds->cmc_db_pedestrian_speed_bound;
  cmc_db_enable_req_msg->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_VEHICLE_V01] = p_speed_bounds->cmc_db_vehicle_speed_bound;
  cmc_db_enable_req_msg->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_WALK_V01] = p_speed_bounds->cmc_db_walk_speed_bound;
  cmc_db_enable_req_msg->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_RUN_V01] = p_speed_bounds->cmc_db_run_speed_bound;

  cmc_db_enable_req_msg->notify_suspend_valid = 1;
  cmc_db_enable_req_msg->notify_suspend.proc_type = SNS_PROC_MODEM_V01;  
  cmc_db_enable_req_msg->notify_suspend.send_indications_during_suspend = TRUE;

  
  retval = qmi_client_send_msg_async(loc_geofence_sensor_cmc_db_server_handle, 
                                     SNS_SAM_DISTANCE_BOUND_ENABLE_REQ_V01, 
                                     cmc_db_enable_req_msg, 
                                     sizeof(sns_sam_distance_bound_enable_req_msg_v01),
                                     cmc_db_enable_resp_msg, 
                                     sizeof(sns_sam_distance_bound_enable_resp_msg_v01), 
                                     loc_geofence_sensor_cmc_db_cb, (void *)2, &txn);
  
  if (retval != 0){
    SHIM_MSG_MED("loc_geofence_sensor_enable_cmc_db: send_msg_async error: %d\n",retval, 0,0);    
    os_MemFree((void**)&cmc_db_enable_req_msg);    
    os_MemFree((void**)&cmc_db_enable_resp_msg);
    return FALSE;
  }

  
  os_MemFree((void**)&cmc_db_enable_req_msg);    
  return TRUE;
}

/** loc_geofence_sensor_stop_cmc_db
 *    Handles CMC Stop request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_stop_cmc_db(void)
{
  qmi_client_error_type         retval;  
  sns_sam_distance_bound_disable_req_msg_v01 *cmc_db_disable_req_msg = NULL;
  sns_sam_distance_bound_disable_resp_msg_v01 *cmc_db_disable_resp_msg = NULL;
  qmi_txn_handle txn;

  if(loc_geofence_sensor_cmc_db_server_handle == NULL)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_enable_cmc_db: Client pointer NULL\n",0, 0,0);
    return FALSE;
  }
  cmc_db_disable_req_msg = os_MemAlloc(sizeof(sns_sam_distance_bound_disable_req_msg_v01),OS_MEM_SCOPE_PROCESSOR);
  if(cmc_db_disable_req_msg == NULL)
  {    
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_set_bound server Unable to allocate memory for set bound req",0,0,0);
    return FALSE;
  }
  memset((void*)cmc_db_disable_req_msg, 0, sizeof(sns_sam_distance_bound_disable_req_msg_v01));


  cmc_db_disable_resp_msg = os_MemAlloc(sizeof(sns_sam_distance_bound_disable_resp_msg_v01),OS_MEM_SCOPE_PROCESSOR);
  if(cmc_db_disable_resp_msg == NULL)
  {    
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_set_bound server Unable to allocate memory for set bound resp",0,0,0);
    os_MemFree((void**)&cmc_db_disable_req_msg);
    return FALSE;
  }
  memset((void*)cmc_db_disable_resp_msg, 0, sizeof(sns_sam_distance_bound_disable_resp_msg_v01));

  cmc_db_disable_req_msg->instance_id = loc_geofence_sensor_cmc_db_instance_id;
  loc_geofence_sensor_cmc_db_instance_id_valid = FALSE;
  loc_geofence_sensor_cmc_db_instance_id = 0;
  
  retval = qmi_client_send_msg_async(loc_geofence_sensor_cmc_db_server_handle, 
                                     SNS_SAM_DISTANCE_BOUND_DISABLE_REQ_V01, 
                                     cmc_db_disable_req_msg, 
                                     sizeof(sns_sam_distance_bound_disable_req_msg_v01),
                                     cmc_db_disable_resp_msg, 
                                     sizeof(sns_sam_distance_bound_disable_resp_msg_v01), 
                                     loc_geofence_sensor_cmc_db_cb, (void *)2, &txn);
  
  if (retval != 0){
    SHIM_MSG_MED("loc_geofence_sensor_disable_cmc_db: send_msg_async error: %d\n",retval, 0,0);    
    os_MemFree((void**)&cmc_db_disable_req_msg);    
    os_MemFree((void**)&cmc_db_disable_resp_msg);
    return FALSE;
  }

  
  os_MemFree((void**)&cmc_db_disable_req_msg);    
  return TRUE;
}

#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/
