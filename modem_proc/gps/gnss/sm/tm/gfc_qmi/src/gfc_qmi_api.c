/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GFC_QMI module which is the interface between QMI and GM

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/gfc_qmi/src/gfc_qmi_api.c#1 $
$DateTime: 2016/12/13 07:59:45 $

=============================================================================

EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/20/15   ssu     Adding default geofence confidence to HIGH
04/02/15    ssu   Batch Dwell notification for geofence.
03/24/12   ss       Supporting operation mode settting to effect Geofencing operation mode
02/29/12   ssu     Support for purge geofence.
02/23/12   ssu     Adding NULL parameter validation and support geofence origin in
                   querying geofence ids
11/15/11   ssu     Initial creation of file .

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "gfc_qmi.h"
#include "gfc_qmi_api.h"
#include "gm_api.h"
#include "gfc_qmi_internal.h"
#include "time.h"

/* Minimum time interval between geofence breaches in seconds */
#define GFC_QMI_DEFAULT_RELARM_INTERVAL_TIME_S 0

gfc_qmi_operation_status_e_type gfc_qmi_add_geofence(
  const gfc_qmi_add_geofence_req_s_type *p_add_request,
  gfc_qmi_operation_result_e_type *p_return_code
)
{
  sm_gm_add_request_type z_gm_add_request;
  boolean b_ret_val;
  uint8 u_error_code;

  if(
      (NULL == p_add_request) ||
      (NULL == p_return_code)
    )
  {
    MSG_HIGH("GFC_QMI::NULL input parameter for gfc_qmi_add_geofence ", 0, 0, 0);
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }

  memset(&z_gm_add_request, 0, sizeof(sm_gm_add_request_type));
  /* Geofence information to add a geofence.  */
  /* Set the default values for the parameters */
  z_gm_add_request.geofence.start_time = 0;
  z_gm_add_request.geofence.stop_time = 0xFFFFFFFF;
  z_gm_add_request.geofence.min_interval_time = GFC_QMI_DEFAULT_RELARM_INTERVAL_TIME_S;
  z_gm_add_request.geofence.location_estimate = FALSE;
  z_gm_add_request.geofence.responsiveness = GEOFENCE_RESPONSIVENESS_MEDIUM;
  z_gm_add_request.geofence.confidence = GEOFENCE_CONFIDENCE_HIGH;
  z_gm_add_request.geofence.q_dwell_time_secs = 0;

  b_ret_val = gfc_qmi_internal_val_and_fill_req(GFC_QMI_OP_TYPE_ADD_GF, (void*)p_add_request, (void*)&z_gm_add_request, p_return_code);
  if(b_ret_val == FALSE)
  {
    MSG_HIGH("GFC_QMI::Add parameter validation failed error_code %d", *p_return_code, 0,0);
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }

  if(gm_add_geofence(&z_gm_add_request, &u_error_code) == FAILED)
  {
    MSG_HIGH("GFC_QMI::Not able to add Geofence, error_code %d", u_error_code, 0,0);
    *p_return_code = u_error_code;
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }
  else
  {
    MSG_HIGH("GFC_QMI::Succesfully sent request to add Geofence, GM TID:: %d",z_gm_add_request.q_transaction_id,0,0);
    *p_return_code = GFC_QMI_ERROR_NONE;
    return GFC_QMI_OPERATION_STATUS_PENDING;
  }
}


gfc_qmi_operation_status_e_type gfc_qmi_delete_geofence(
  gfc_qmi_delete_geofence_req_s_type *p_del_request,
  gfc_qmi_operation_result_e_type *p_return_code
)
{
  uint8 u_error_code;
  sm_gm_delete_request_type z_gm_del_request;

  if(
      (NULL == p_del_request) ||
      (NULL == p_return_code)
    )
  {
    MSG_HIGH("GFC_QMI::NULL input parameter for gfc_qmi_delete_geofence ", 0, 0, 0);
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }

  memset(&z_gm_del_request, 0, sizeof(sm_gm_delete_request_type));
  /* Geofence information to delete a geofence */
  z_gm_del_request.client_handle = gfc_qmi_internal_get_client_handle();
  (void)gfc_qmi_internal_val_and_fill_req(GFC_QMI_OP_TYPE_DELETE_GF, (void*)p_del_request, (void*)&z_gm_del_request, p_return_code);  /* Removed set-but-not-used variable */

  if ( gm_delete_geofence(&z_gm_del_request, &u_error_code) ==
         FAILED )
  {
    *p_return_code = u_error_code;
    MSG_ERROR( "GFC_QMI::Failure of geofence delete: ID %d, error=%u",
               z_gm_del_request.geofence_id, u_error_code, 0);
    return GFC_QMI_OPERATION_STATUS_FAILED;

  }
  else
  {
    MSG_MED("GFC_QMI::Succesfully sent request for delete Geofence %d GM TID %d", z_gm_del_request.geofence_id, z_gm_del_request.q_transaction_id, 0);
    *p_return_code = GFC_QMI_ERROR_NONE;
    return GFC_QMI_OPERATION_STATUS_PENDING;

  }
}

gfc_qmi_operation_status_e_type gfc_qmi_purge_geofence(
  gfc_qmi_operation_result_e_type *p_return_code
)
{
  uint8 u_error_code;
  sm_gm_purge_request_type z_gm_purge_request;

  if( NULL == p_return_code )
  {
    MSG_HIGH("GFC_QMI::NULL input parameter for gfc_qmi_purge_geofence ", 0, 0, 0);
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }

  memset(&z_gm_purge_request, 0, sizeof(sm_gm_purge_request_type));
  /* Geofence information to delete a geofence */
  z_gm_purge_request.client_handle = gfc_qmi_internal_get_client_handle();
  (void)gfc_qmi_internal_val_and_fill_req(GFC_QMI_OP_TYPE_PURGE_GF, NULL, (void*)&z_gm_purge_request, p_return_code);  /* Removed set-but-not-used variable */

  if ( gm_purge_geofence(&z_gm_purge_request, &u_error_code) ==
         FAILED )
  {
    *p_return_code = u_error_code;
    MSG_ERROR( "GFC_QMI::Failure of geofence purge: error=%u",
               0, u_error_code, 0);
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }
  else
  {
    MSG_MED("GFC_QMI::Succesfully sent request for purge Geofence ", 0, 0, 0);
    *p_return_code = GFC_QMI_ERROR_NONE;
    return GFC_QMI_OPERATION_STATUS_PENDING;
  }
}

gfc_qmi_operation_status_e_type gfc_qmi_edit_geofence(
  const gfc_qmi_edit_geofence_req_s_type *p_edit_request,
  gfc_qmi_operation_result_e_type *p_return_code
)
{
  uint8 u_error_code;
  sm_gm_edit_request_type z_gm_edit_request;
  boolean b_ret_val;

  if(
      (NULL == p_edit_request) ||
      (NULL == p_return_code)
    )
  {
    MSG_HIGH("GFC_QMI::NULL input parameter for gfc_qmi_edit_geofence ", 0, 0, 0);
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }

  memset(&z_gm_edit_request, 0, sizeof(sm_gm_edit_request_type));
  // Obtain the client handle to GM
  z_gm_edit_request.client_handle = gfc_qmi_internal_get_client_handle();

  b_ret_val = gfc_qmi_internal_val_and_fill_req(GFC_QMI_OP_TYPE_EDIT_GF, (void*)p_edit_request, (void*)&z_gm_edit_request, p_return_code);
  if(b_ret_val == FALSE)
  {
    MSG_HIGH("GFC_QMI::Edit parameter validation failed for geofence %d error_code %d", z_gm_edit_request.geofence_id, *p_return_code, 0);
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }

  if(gm_edit_geofence(&z_gm_edit_request, &u_error_code) == FAILED)
  {
    MSG_HIGH("GFC_QMI::Not able to edit Geofence %d, error_code %d", z_gm_edit_request.geofence_id, u_error_code, 0);
    *p_return_code = u_error_code;
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }
  else
  {
    MSG_HIGH("GFC_QMI::Succesfully sent request to edit Geofence %d",z_gm_edit_request.geofence_id,0,0);
    *p_return_code = GFC_QMI_ERROR_NONE;
    return GFC_QMI_OPERATION_STATUS_PENDING;
  }

}

gfc_qmi_operation_status_e_type gfc_qmi_query_geofence(
  const gfc_qmi_query_geofence_req_s_type *p_query_gf_request,
  gfc_qmi_operation_result_e_type *p_return_code
)
{
  uint8 u_error_code;
  sm_gm_query_geofence_status_request_type z_gm_query_request;
  boolean b_ret_val;

  if(
      (NULL == p_query_gf_request) ||
      (NULL == p_return_code)
    )
  {
    MSG_HIGH("GFC_QMI::NULL input parameter for gfc_qmi_query_geofence ", 0, 0, 0);
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }

  memset(&z_gm_query_request, 0, sizeof(sm_gm_query_geofence_status_request_type));
  // Obtain the client handle to GM
  z_gm_query_request.client_handle = gfc_qmi_internal_get_client_handle();

  b_ret_val = gfc_qmi_internal_val_and_fill_req(GFC_QMI_OP_TYPE_QUERY_GF, (void*)p_query_gf_request, (void*)&z_gm_query_request, p_return_code);
  if(b_ret_val == FALSE)
  {
    MSG_HIGH("GFC_QMI::Query geofence validation failed for geofence %d error_code %d", z_gm_query_request.geofence_id, *p_return_code, 0);
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }
  /* Query this geofence from the GM */
  if(gm_query_geofence_status(&z_gm_query_request, &u_error_code) == FAILED)
  {
    MSG_HIGH("GFC_QMI::Not able to query Geofence %d, error_code %d", z_gm_query_request.geofence_id, u_error_code, 0);
    *p_return_code = u_error_code;
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }
  else
  {
    MSG_HIGH("GFC_QMI::Succesfully sent request to query Geofence %d TransactionID %d",z_gm_query_request.geofence_id,p_query_gf_request->transaction_id,0);
    *p_return_code = GFC_QMI_ERROR_NONE;
    return GFC_QMI_OPERATION_STATUS_PENDING;
  }

}

gfc_qmi_operation_status_e_type gfc_qmi_query_geofence_ids(
  const gfc_qmi_query_geofence_ids_req_s_type *p_query_gf_ids_request,
  gfc_qmi_operation_result_e_type *p_return_code
)
{
  uint8 u_error_code;
  sm_gm_geofences_handles_request_type z_gm_get_geofences_handles_request;
  boolean b_ret_val;

  if(
      (NULL == p_query_gf_ids_request) ||
      (NULL == p_return_code)
    )
  {
    MSG_HIGH("GFC_QMI::NULL input parameter for gfc_qmi_query_geofence_ids ", 0, 0, 0);
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }
  memset(&z_gm_get_geofences_handles_request, 0, sizeof(sm_gm_geofences_handles_request_type));
  // Obtain the client handle to GM
  z_gm_get_geofences_handles_request.client_handle = gfc_qmi_internal_get_client_handle();

  b_ret_val = gfc_qmi_internal_val_and_fill_req(GFC_QMI_OP_TYPE_QUERY_GF_IDS, (void*)p_query_gf_ids_request, (void*)&z_gm_get_geofences_handles_request, p_return_code);
  if(b_ret_val == FALSE)
  {
    MSG_HIGH("GFC_QMI::Query geofence id validation failed with error_code %d", *p_return_code, 0, 0);
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }

  /* Query this geofence from the GM */
  if(gm_get_geofences_handles(&z_gm_get_geofences_handles_request, &u_error_code) == FAILED)
  {
    MSG_HIGH("GFC_QMI::Not able to query Geofence IDs, error_code %d", u_error_code, 0, 0);
    *p_return_code = u_error_code;
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }
  else
  {
    MSG_HIGH("GFC_QMI::Succesfully sent request to query ALL Geofence IDs, TID %d ",
             p_query_gf_ids_request->transaction_id,
             0,
             0);
    *p_return_code = GFC_QMI_ERROR_NONE;
    return GFC_QMI_OPERATION_STATUS_PENDING;
  }

}

gfc_qmi_operation_status_e_type gfc_qmi_set_geofence_engine_config(
  const gfc_qmi_set_geofence_engine_config_req_s_type *p_set_gf_engine_config_request,
  gfc_qmi_operation_result_e_type *p_return_code
)
{
  sm_gm_set_geofence_engine_config_request_type z_gm_set_geofence_engine_config_request = {0};
  sm_gm_set_geofence_client_config_request_type z_gm_set_client_config_request = {0};
  
  boolean b_ret_val;
  uint8 u_error_code;
  if(
      (NULL == p_set_gf_engine_config_request) ||
      (NULL == p_return_code)
    )
  {
    MSG_HIGH("GFC_QMI::NULL input parameter for gfc_qmi_set_geofence_engine_config ", 0, 0, 0);
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }
  // Obtain the client handle to GM
  z_gm_set_geofence_engine_config_request.client_handle = gfc_qmi_internal_get_client_handle();
  b_ret_val = gfc_qmi_internal_val_and_fill_req(
    GFC_QMI_OP_TYPE_SET_GF_ENGINE_CONFIG,
    (void*)p_set_gf_engine_config_request,
    (void*)&z_gm_set_geofence_engine_config_request,
    p_return_code);
  
  if(b_ret_val == FALSE)
  {
    MSG_MED("GFC_QMI::Parameter validation failed for set geofence engine config error_code %d", *p_return_code, 0, 0);
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }

  if(z_gm_set_geofence_engine_config_request.q_mask != 0)
  {
    if(gm_set_geofence_engine_config(&z_gm_set_geofence_engine_config_request, &u_error_code) == FAILED)
    {
      MSG_MED("GFC_QMI::Set geofence engine config request failed error_code %d", u_error_code, 0, 0);
      *p_return_code = u_error_code;
      return GFC_QMI_OPERATION_STATUS_FAILED;
    }
  }

  // Obtain the client handle to GM
  z_gm_set_client_config_request.config_request.client_handle = gfc_qmi_internal_get_client_handle();

  b_ret_val = gfc_qmi_internal_val_and_fill_req(
    GFC_QMI_OP_TYPE_SET_GF_CLIENT_CONFIG,
    (void*)p_set_gf_engine_config_request,
    (void*)&z_gm_set_client_config_request,
    p_return_code);
  if(b_ret_val == FALSE)
  {
    MSG_MED("GFC_QMI::Parameter validation failed for set geofence engine config error_code %d", *p_return_code, 0, 0);
    return GFC_QMI_OPERATION_STATUS_FAILED;
  }

  if(z_gm_set_client_config_request.config_request.q_mask != 0)
  {
    if(gm_set_geofence_client_config(&z_gm_set_client_config_request, &u_error_code) == FAILED)
    {
      MSG_MED("GFC_QMI::Set geofence client config request failed error_code %d", u_error_code, 0, 0);
      *p_return_code = u_error_code;
      return GFC_QMI_OPERATION_STATUS_FAILED;
    }
  }
  
  MSG_MED("GFC_QMI::Succesfully sent request to set geofence config",0,0,0);
  *p_return_code = GFC_QMI_ERROR_NONE;
  return GFC_QMI_OPERATION_STATUS_PENDING;
}


/*===========================================================================
gfc_qmi_handle_batch_breach_req

Description:
   This function is used by the QMI to set the batch breach reporting from GM

Parameters:
  b_enable: Enable/disable batch breach reporting from GM

Return value:
  None

=============================================================================*/
void gfc_qmi_handle_batch_breach_req(boolean b_enable)
{
  gfc_qmi_set_geofence_engine_config_req_s_type z_set_gf_engine_config_request;
  gfc_qmi_operation_result_e_type e_return_code;

  memset(&z_set_gf_engine_config_request, 0, sizeof(gfc_qmi_set_geofence_engine_config_req_s_type));

  MSG_MED("GFC_QMI: Setting batch breach enable %d to GM",
            b_enable,0,0);
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_ENABLE_BATCH_BREACH_MASK;
  z_set_gf_engine_config_request.b_enable_batch_breach_report = b_enable;
  /* Hard code the transaction ID as of now */
  z_set_gf_engine_config_request.transaction_id = NON_QMI_TRXN_ID;

  gfc_qmi_set_geofence_engine_config(&z_set_gf_engine_config_request, &e_return_code);
}

/*===========================================================================
gfc_qmi_handle_inject_time_zone

Description:
   This function is used by the QMI to inject the TimeZone information into GM

Parameters:   
   qq_utc_secs : UTC Time
   qq_raw_offset_secs : Raw offset from UTC time
   qq_dlt_save_secs   : Daylight saving time adjustment

Return value: 
  None

=============================================================================*/
void gfc_qmi_handle_inject_time_zone
(
  uint64 qq_utc_secs,
  uint64 qq_raw_offset_secs,
  uint64 qq_dlt_save_secs
)
{
  sm_gm_time_zone_info z_gm_time_info;

  z_gm_time_info.q_up_time_stamp    = time_get_uptime_secs();
  z_gm_time_info.qq_time_utc_secs   = qq_utc_secs;
  z_gm_time_info.qq_raw_offset_secs = qq_raw_offset_secs;
  z_gm_time_info.qq_dst_offset_secs = qq_dlt_save_secs;
  MSG_MED("Rcvd TZ injection uptime %d utc %d offset %d", 
           z_gm_time_info.q_up_time_stamp, 
           z_gm_time_info.qq_time_utc_secs, 
           z_gm_time_info.qq_raw_offset_secs);

  gm_inject_time_zone_req(z_gm_time_info);
}

/*===========================================================================
gfc_qmi_handle_operation_mode_req

Description:
   This function is used by the QMI to set the operation mode

Parameters:
  op_mode: Operation Mode

Return value:
  None

=============================================================================*/
void gfc_qmi_handle_operation_mode_req(pdsm_pd_session_operation_e_type op_mode)
{
  gfc_qmi_set_geofence_engine_config_req_s_type z_set_gf_engine_config_request;
  gfc_qmi_operation_result_e_type e_return_code;

  memset(&z_set_gf_engine_config_request, 0, sizeof(gfc_qmi_set_geofence_engine_config_req_s_type));

  MSG_MED("GFC_QMI: Setting operation mode %d to GM",
            op_mode,0,0);
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_GM_OPERATION_MODE_CONFIG_MASK;
  z_set_gf_engine_config_request.q_gm_pos_operation_mode = op_mode;
  /* Hard code the transaction ID as of now */
  z_set_gf_engine_config_request.transaction_id = NON_QMI_TRXN_ID;

  gfc_qmi_set_geofence_engine_config(&z_set_gf_engine_config_request, &e_return_code);

}



boolean gfc_qmi_register(
  const gfc_qmi_cb_rsp_s_type* p_geofence_func_cbs
)
{
  if(NULL == p_geofence_func_cbs)
  {
    return FALSE;
  }
  else
  {
    gfc_qmi_internal_register_cbs(p_geofence_func_cbs);
    return TRUE;
  }
}

