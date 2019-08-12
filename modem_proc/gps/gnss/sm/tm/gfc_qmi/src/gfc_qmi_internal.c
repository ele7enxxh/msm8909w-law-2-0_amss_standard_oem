/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GFC_QMI module which is the interface between QMI and GM

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.
  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/gfc_qmi/src/gfc_qmi_internal.c#1 $
$DateTime: 2016/12/13 07:59:45 $

=============================================================================

EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
1/18/16    ss       Supporting back Max Pos Unc in GF config and fixing leak in txIds
10/26/15   ss       Supporting Geofence Client Config indication
10/20/15   ssu     Adding default geofence confidence to HIGH
06/09/15   ssu     Adding NV for GM filtering of WLAN freq.  
05/06/15   sjk    Adding a default configuration for Max GFs required by GFC_QMI
04/02/15    ssu   Batch Dwell notification for geofence.
02/26/15   ssu      Adding new NV items for GM LOWI integration.
01/26/15    ssu   Adding geofence custom responsiveness.
11/12/14   sj     Make changes for GF Config items.
08/25/14   jv    16-bit SVID Support
05/07/14   ssu   SMART and KW compilation errors fixed.
04/03/14   ssu   Additional Fix details parameters for Geofence Breach reports
03/24/12   ss       Supporting operation mode settting to effect Geofencing operation mode
03/10/14   ssu    Addressing a few KW errors .
02/21/14   ssu     Adding NV for configuring motion sensing distance in case of challenging GPS env.
02/20/14   ssu     Adding a NV for modifying the medium responsiveness backoff value and gps challenging env backoff config.
02/21/14   ssu     Adding GM CPI request rate at bootup and resolving a bug in validation in purge resp handler.
02/10/14   ssu     Adding a NV for modifying the rate at which GM requests CPI injection.
02/10/14   ssu    Updating GFC request queue after receiving purge response from GM.
02/06/14   ss    Supporting Medium Confidence responsivenes value config
01/31/14    ss    Supporting Max acceptable GF PUNC confguration from QMI
1/29/14    ssu     Populate the option pointer while requesting for pos fixes through GFC.
01/24/14   ssu     Setting the height uncertainty flag in GFC breach report when included.
02/29/12   ssu     Adding geofence purge request API.
02/08/12   ssu     Accepting transaction ID value of 0.
02/06/12   ssu     Adding error checks for cases where the QMI does not register
                   with GFC_QMI
11/15/11   ssu     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "gfc_qmi_api.h"
#include "pdapi.h"
#include "gfc_qmi.h"
#include "gm_api.h"
#include "tm_prtl_iface.h"
#include "tm_pdapi_iface.h"
#include "tm_common.h"
#include "gfc_qmi_internal.h"
#include "tm_data.h"
#include "tm_pdapi_client.h"
#include "math.h"


/* The maximum number of concurrent geofence requests */
#define GFC_QMI_GM_MAX_GF_DEFAULT (200)

#define MAX_CONCUR_GEOFENCE_NUM 32
#define GFC_QMI_INVALID_TRXN_ID 0xFFFFFFFF
/* Use this transaction ID when QMI indication notification not desired */

/* offset (in meters) that should be subtracted from altitude sent
   in the GM position report to calculate the altitude w.r.t
   WGS-84 ellipsoid  */
#define GFC_QMI_ALTITUDE_WRT_WGS84_ELLIPSOID_OFFSET (500)

/* Horizontal unc confidence pct */
#define GFC_QMI_HOR_UNC_CONF_PCT 39

typedef struct
{
  /* Function pointers registered for callbacks from GFC_QMI */
  gfc_qmi_add_geofence_rsp_f_type       *p_add_rsp_func;
  gfc_qmi_delete_geofence_rsp_f_type    *p_delete_rsp_func;
  gfc_qmi_purge_geofence_rsp_f_type     *p_purge_rsp_func;
  gfc_qmi_edit_geofence_rsp_f_type      *p_edit_rsp_func;
  gfc_qmi_query_geofence_rsp_f_type     *p_query_rsp_func;
  gfc_qmi_breach_geofence_rsp_f_type    *p_breach_rsp_func;
  gfc_qmi_general_notify_rsp_f_type     *p_general_notify_rsp_func;
  gfc_qmi_query_geofence_ids_rsp_f_type *p_geofence_ids_rsp_func;
  gfc_qmi_set_geofence_engine_config_rsp_f_type *p_set_gf_eng_conf_rsp_func;
  gfc_qmi_batch_breach_geofence_rsp_f_type      *p_batch_breach_rsp_func;
  gfc_qmi_proximity_geofence_rsp_f_type         *p_proximity_rsp_func;
  gfc_qmi_batch_dwell_geofence_rsp_f_type *p_batch_dwell_rsp_func;
  gfc_qmi_geofence_inject_tz_f_type   *tz_inject_func;
}gfc_qmi_cb_s_type;

typedef struct
{
  uint32 q_qmi_trxn_id;
  uint32 q_gm_trxn_id;
  boolean b_used;
}gfc_qmi_trxn_id_s_type;

typedef struct
{
  /* Identifier for the GFC_QMI client for GM*/
  sm_gm_client_type z_client;

  /*Identifier for the CPI client for GM*/
  sm_gm_client_type z_cpi_client;
  /* Handle to the GM core for GFC_QMI*/
  sm_gm_client_handle u_gm_handle;

  /* Handle to the GM core for GFC_QMI*/
  sm_gm_client_handle u_gm_cpi_handle;

  /* The callback functions registered by QMI
  for Geofencing operations. */
  gfc_qmi_cb_s_type z_gfc_qmi_cb_table;

  /* The table which maintains the mapping for the transaction Ids from QMI
     with the transaction id for GM */
  gfc_qmi_trxn_id_s_type trxn_id_table[MAX_CONCUR_GEOFENCE_NUM];

  /* The currently used GM transaction ID */
  uint32 q_curr_gm_trxn_id;

  /* A structure that contains pointer to API functions exposed by TmCore to
     handle its registerd clients for callbacks */
  tm_prtl_cb_s_type z_prtlIfaceTbl;
  /* Structure to obtain the position fix */
  tm_pdapi_get_pos_param_s_type z_get_pos_param;

  /* Optional parameters for TM core position fix. */
  pdsm_pd_option_s_type z_get_pos_options;        /* PD option type */
  pdsm_pd_qos_type z_get_pos_qos;                 /* QOS for this pos fix request */

  /* QOS parameters for TM core position fix. */

  /* Flag to indicate whether QMI has registered with GFC_QMI */
  boolean b_qmi_registered;

  /*Flag to indicate whether CPI was requested*/
  boolean b_cpi_requested;

  /*Timer to wait for cpi position request*/
  os_TimerMsgType* p_cpi_wait_timer;

}gfc_qmi_internal_info_s_type;

static gfc_qmi_internal_info_s_type z_gfc_qmi_info;

boolean gfc_qmi_internal_register_cbs(
  const gfc_qmi_cb_rsp_s_type* p_geofence_func_cbs
)
{
  if(z_gfc_qmi_info.b_qmi_registered == FALSE)
  {
    /* Initialise the function pointers to point to the callbacks supplied */
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_add_rsp_func = p_geofence_func_cbs->add_rsp_func;
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_delete_rsp_func = p_geofence_func_cbs->delete_rsp_func;
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_purge_rsp_func = p_geofence_func_cbs->purge_rsp_func;
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_edit_rsp_func = p_geofence_func_cbs->edit_rsp_func;
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_query_rsp_func = p_geofence_func_cbs->query_gf_rsp_func;
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_breach_rsp_func = p_geofence_func_cbs->breach_rsp_func;
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_general_notify_rsp_func = p_geofence_func_cbs->general_notify_rsp_func;
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_geofence_ids_rsp_func = p_geofence_func_cbs->geofence_ids_rsp_func;
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_set_gf_eng_conf_rsp_func = p_geofence_func_cbs->set_gf_eng_conf_rsp_func;
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_batch_breach_rsp_func = p_geofence_func_cbs->batch_breach_rsp_func;
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_proximity_rsp_func = p_geofence_func_cbs->proximity_rsp_func;
  	z_gfc_qmi_info.z_gfc_qmi_cb_table.p_batch_dwell_rsp_func = p_geofence_func_cbs->batch_dwell_rsp_func;
    z_gfc_qmi_info.z_gfc_qmi_cb_table.tz_inject_func = p_geofence_func_cbs->tz_inject_func;
    
    z_gfc_qmi_info.b_qmi_registered = TRUE;
  }
  return TRUE;
}



/*
 ******************************************************************************
 *
 * gfc_qmi_internal_getnext_gm_trxn_id
 *
 * Function description:
 *  Get next transaction id to send request to Geofence module. A total of
 *  MAX_CONCUR_GEOFENCE_NUM requests are supported at the same time.
 *  The valid GM transaction ID is returned from this function.
 *
 * Parameters:
 *    qmi_trxn_id: The QMI transaction ID which is passed in from QMI .
 *    This transaction ID is mapped with the GM transaction ID by the GFC_QMI.
 * Return value:
 *    The GM transaction ID mapped to the passed in QMI transaction ID .
 *
 ******************************************************************************
*/
static uint32 gfc_qmi_internal_getnext_gm_trxn_id(uint32 qmi_trxn_id)
{
  uint32 i, gm_base_trxn_id;
  /* Note that the base transaction ID starts from TM_GFC_QMI_GM_TRANSACTION_ID_BASE + 2 .
      The transaction ID TM_GFC_QMI_GM_TRANSACTION_ID_BASE is reserved for unsolicited messages from GM to GFC_QMI.
      The transaction ID TM_GFC_QMI_GM_TRANSACTION_ID_BASE+1 is reserved for non qmi txn id PASSED FROM gfc AT BOOT UP. .
  */
  if(qmi_trxn_id == NON_QMI_TRXN_ID)
  {
    gm_base_trxn_id = TM_GFC_QMI_GM_TRANSACTION_ID_BASE + 1;
  }
  else
  {
    gm_base_trxn_id = TM_GFC_QMI_GM_TRANSACTION_ID_BASE + 2;
  }

  if ( (z_gfc_qmi_info.q_curr_gm_trxn_id < gm_base_trxn_id) ||
       ( z_gfc_qmi_info.q_curr_gm_trxn_id >=
         ( TM_GFC_QMI_GM_TRANSACTION_ID_BASE + TM_UMTS_UP_GM_TRANSACTION_ID_RANGE )
       )
     )
  {
    z_gfc_qmi_info.q_curr_gm_trxn_id = gm_base_trxn_id;
  }
  else
  {
    z_gfc_qmi_info.q_curr_gm_trxn_id++;
  }
  /* Store the transaction Id mappings of GM and QMI in the table */
  for (i = 0 ; i < MAX_CONCUR_GEOFENCE_NUM; i++)
  {
    // Search for an unused entry
    if(z_gfc_qmi_info.trxn_id_table[i].b_used == FALSE)
    {
      z_gfc_qmi_info.trxn_id_table[i].q_gm_trxn_id = z_gfc_qmi_info.q_curr_gm_trxn_id;
      z_gfc_qmi_info.trxn_id_table[i].q_qmi_trxn_id = qmi_trxn_id;
      z_gfc_qmi_info.trxn_id_table[i].b_used = TRUE;
      MSG_MED("gfc_qmi_internal_getnext_gm_trxn_id :: i %d GM TID %d , QMI TID %d",
              i,
              z_gfc_qmi_info.trxn_id_table[i].q_gm_trxn_id,
              z_gfc_qmi_info.trxn_id_table[i].q_qmi_trxn_id);
      return z_gfc_qmi_info.trxn_id_table[i].q_gm_trxn_id;
    }
  }
  MSG_MED("gfc_qmi_internal_getnext_gm_trxn_id :: Entry not found return %d ",
          GFC_QMI_INVALID_TRXN_ID,0,0);
  return GFC_QMI_INVALID_TRXN_ID;
}



/*
 ******************************************************************************
 *
 * gfc_qmi_internal_get_qmi_trxn_id
 *
 * Function description:
 *  Get the QMI transaction ID corresponding to the mapped GM transaction ID.
 *
 * Parameters:
 *    gm_trxn_id: The transaction ID passed in from the GM core as a result of its response
 *    to GFC_QMI
 *    This transaction ID is mapped with the QMI transaction ID by the GFC_QMI.
 * Return value:
 *    The QMI transaction ID corresponding to the passed in GM transaction ID .
 *
 ******************************************************************************
*/
static uint32 gfc_qmi_internal_get_qmi_trxn_id(uint32 gm_trxn_id)
{
  uint32 i;

  /* Search for the corresponding QMI transaction ID from GM transaction ID */
  for (i = 0 ; i < MAX_CONCUR_GEOFENCE_NUM; i++)
  {
    if(z_gfc_qmi_info.trxn_id_table[i].q_gm_trxn_id == gm_trxn_id)
    {
      /* Indicate that this entry in the table is no longer used and available for reuse */
      z_gfc_qmi_info.trxn_id_table[i].b_used = FALSE;
      MSG_MED("gfc_qmi_internal_get_qmi_trxn_id i %d GM TID :: %d , QMI TID :: %d",
              i,
              z_gfc_qmi_info.trxn_id_table[i].q_gm_trxn_id,
              z_gfc_qmi_info.trxn_id_table[i].q_qmi_trxn_id);
      return z_gfc_qmi_info.trxn_id_table[i].q_qmi_trxn_id;
    }
  }
  MSG_MED("gfc_qmi_internal_get_qmi_trxn_id No entry found return %d",GFC_QMI_INVALID_TRXN_ID,0,0);
  return GFC_QMI_INVALID_TRXN_ID;
}


boolean gfc_qmi_internal_conv_set_gf_engine_config_req_to_gm(gfc_qmi_set_geofence_engine_config_req_s_type* p_inp_request, 
                                                             sm_gm_set_geofence_engine_config_request_type* p_gm_request)
{
  geofence_engine_config_s_type *gm_config = NULL;
  char u_string[200];

  tm_core_get_gm_engine_config(&gm_config);

  /* Get the transaction ID for this edit operation */
  if((p_gm_request->q_transaction_id = gfc_qmi_internal_getnext_gm_trxn_id(p_inp_request->transaction_id))
        == GFC_QMI_INVALID_TRXN_ID)
  {
    /* The mandatory parametrs are not specified , report error. */
    MSG_ERROR("gfc_qmi_internal_conv_set_gf_engine_config_req_to_gm Invalid transaction ID \n",
             0,0,0);
    return FALSE;
  }

  MSG_MED("Translate GF Engine Config Req QMI-->GM. Conf Mask 0x%x",
           p_inp_request->t_gfc_qmi_gm_engine_conf_mask, 0, 0);

  /* Convert optional parameters */
  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_GNSS_UNAVAIL_TIMEOUT_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONFIG_GNSS_UNAVAIL_TIMEOUT_MASK;
    p_gm_request->q_gnss_unavailable_indication_timeout = p_inp_request->gnss_unavailable_indication_timeout;
  }

  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_ENABLE_SENSORS_FOR_MOTION_DETECTION_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONFIG_ENABLE_SENSOR_MOTION_DETECTION_MASK;
    p_gm_request->q_enable_sensor_motion_detection = p_inp_request->q_gm_enable_sensor_motion_detection;
    MSG_MED("Enable sensors %d",p_gm_request->q_enable_sensor_motion_detection, 0, 0);
  }
  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_ENABLE_WIFI_FOR_MOTION_DETECTION_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONFIG_ENABLE_WIFI_MOTION_DETECTION_MASK;
    p_gm_request->q_enable_wifi_motion_detection = p_inp_request->q_gm_enable_wifi_motion_detection;
    MSG_MED("Enable wifi %d",p_gm_request->q_enable_wifi_motion_detection, 0, 0);
  }
  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_ENABLE_WWAN_FOR_MOTION_DETECTION_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONFIG_ENABLE_WWAN_MOTION_DETECTION_MASK;
    p_gm_request->q_enable_wwan_motion_detection = p_inp_request->q_gm_enable_wwan_motion_detection;
    MSG_MED("Enable wwan %d",p_gm_request->q_enable_wwan_motion_detection, 0, 0);
  }

  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_ENABLE_CPI_USAGE_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONFIG_ENABLE_POSITION_INJECTION_MASK;
    p_gm_request->q_enable_position_injection = p_inp_request->q_gm_enable_position_injection_usage;
    MSG_MED("Enable CPI Usage ox%x",p_gm_request->q_enable_position_injection, 0, 0);
  }

  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_ENABLE_LPW_USAGE_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONFIG_ENABLE_LPW_MASK;
    p_gm_request->q_enable_lpw = p_inp_request->q_gm_enable_lpw_usage;
    MSG_MED("Enable LPW Usage ox%x",p_gm_request->q_enable_lpw, 0, 0);    
  }


  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_ENABLE_BATCH_BREACH_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONFIG_ENABLE_BATCH_BREACH_REPORT_MASK;
    p_gm_request->b_enable_batch_breach_report = p_inp_request->b_enable_batch_breach_report;
  }

  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_GNSS_MAX_POS_UNC_ACCEPTED_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONFIG_GNSS_MAX_POS_UNC_ACCEPTED_MASK;
    p_gm_request->q_gnss_max_pos_unc_accepted = p_inp_request->q_gm_gnss_max_pos_unc_accepted;
    MSG_MED("Max Punc Accepted %d",p_gm_request->q_gnss_max_pos_unc_accepted, 0, 0);    
  }

  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_USE_NW_ASST_FIXES_MASK )
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONFIG_USE_NW_ASST_FIXES_MASK;
    p_gm_request->b_use_nw_asst_fixes = p_inp_request->b_use_nw_asst_fixes;
  }

  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_HIGH_RESPONSIVENESS_CONFIG_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONFIG_HIGH_RESPONSIVENESS_CONFIG_MASK;
    p_gm_request->q_high_responsiveness_config = p_inp_request->q_gm_high_responsiveness_config;
  }


  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_MED_RESP_BACKOFF_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONFIG_MED_RESPONSIVENESS_VALUE_CONFIG_MASK;
    p_gm_request->q_medium_resp_value_secs = p_inp_request->q_gm_med_resp_backoff;
    MSG_MED("Set MedRespBackoff %d secs", p_gm_request->q_medium_resp_value_secs, 0, 0);
  }

  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_MS_SPEED_MASK)
  {
    p_gm_request->q_mask |= SM_GM_MOTION_STATE_SPEED_MASK;
    p_gm_request->q_gm_motion_state_speeds = p_inp_request->q_gm_ms_speed_config;
    MSG_MED("Set Speed",0, 0, 0);
  }

  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_GM_OPERATION_MODE_CONFIG_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONFIG_POS_OPERATION_MODE_MASK;
    p_gm_request->q_gm_pos_operation_mode = p_inp_request->q_gm_pos_operation_mode;
  }

  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_COMP_LOGGING_RATE_CONFIG_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONFIG_COMP_LOGGING_RATE_MASK;
    p_gm_request->q_gm_comp_logging_rate = p_inp_request->q_gm_comp_logging_rate;
  }
  
  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_LOWI_REGISTER_ENABLE)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONF_LOWI_REGISTER_ENABLE_MASK;
    p_gm_request->b_lowi_reg_enable = p_inp_request->b_lowi_reg_enable;
  }
  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_LOWI_SUBSCRIPTION_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONF_LOWI_SUBSCRIPTION_MASK;
    p_gm_request->q_lowi_subs_mask = p_inp_request->q_lowi_subs_mask;
  }
  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_LOWI_REG_TIMER_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONF_LOWI_REG_TIMER_MASK;
    p_gm_request->q_lowi_enable_reg_timer_mask = p_inp_request->q_lowi_enable_reg_timer_mask;
  }
  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_LOWI_SHORT_TERM_TIMER_RETRY_CNT)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONF_LOWI_SHORT_TERM_TIMER_RETRY_CNT_MASK;
    p_gm_request->q_lowi_short_term_timer_max_retry_cnt = p_inp_request->q_lowi_short_term_timer_max_retry_cnt;
  }
  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_IOD_CONFIG_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONFIG_IOD_SWITCH_MASK;
    p_gm_request->q_sm_gm_iod_config_switch_mask = p_inp_request->q_sm_gm_iod_config_switch_mask;
  }

  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_FILTER_WLAN_FREQ_MASK)
  {
    p_gm_request->q_mask |= SM_GM_ENGINE_CONFIG_FILTER_WLAN_FREQ_MASK;
    p_gm_request->q_filter_wlan_freq_mask = p_inp_request->q_sm_gm_filter_wlan_freq_mask;
  }

  if(0 == p_gm_request->q_mask)
  {
    /*Release the TxID*/
    (void)gfc_qmi_internal_get_qmi_trxn_id(p_gm_request->q_transaction_id);
  }

  MSG_MED("GFC_QMI:: Set config mask GFC:: 0x%x GM :: 0x%x , txn id :: 0x%x\n",
          p_inp_request->t_gfc_qmi_gm_engine_conf_mask,
          p_gm_request->q_mask,
          p_gm_request->q_transaction_id);

  return TRUE;

}

boolean gfc_qmi_internal_conv_set_gf_client_config_req_to_gm(gfc_qmi_set_geofence_engine_config_req_s_type* p_inp_request, 
                                                             sm_gm_set_geofence_client_config_request_type* p_gm_request)
{
  char u_string[200];

  if( (NULL == p_inp_request) || (NULL == p_gm_request))
  {
    MSG_ERROR("Null Pointer",0,0,0);
    return FALSE;
  }

  /* Get the transaction ID for this edit operation */
  if((p_gm_request->q_transaction_id = gfc_qmi_internal_getnext_gm_trxn_id(p_inp_request->transaction_id))
        == GFC_QMI_INVALID_TRXN_ID)
  {
    /* The mandatory parametrs are not specified , report error. */
    MSG_ERROR("gfc_qmi_internal_conv_set_gf_engine_config_req_to_gm Invalid transaction ID \n",
             0,0,0);
    return FALSE;
  }

  MSG_MED("Translate GF Client Config Req QMI-->GM. Conf Mask 0x%x",
           p_inp_request->t_gfc_qmi_gm_engine_conf_mask, 0, 0);
  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_GNSS_POS_QOS_SESS_TIMEOUT_MASK)
  {
    p_gm_request->config_request.q_mask |= SM_GM_CLIENT_CONFIG_GNSS_POS_QOS_SESS_TIMEOUT_MASK;
    p_gm_request->config_request.q_gnss_pos_qos_sess_timeout = p_inp_request->gnss_pos_qos_sess_timeout;
    MSG_MED("Set Gnss Pos QosSessTimeout %d",p_gm_request->config_request.q_gnss_pos_qos_sess_timeout, 0, 0);
  }

  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_MAX_GF_MASK)
  {
      p_gm_request->config_request.q_mask |= SM_GM_CLIENT_CONFIG_MAX_GEOFENCES_MASK;
      p_gm_request->config_request.q_max_geofences = p_inp_request->q_max_gf;
  }
  
  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_CPI_REQUEST_RATE_CONFIG_MASK)
  {
    p_gm_request->config_request.q_mask |= SM_GM_CLIENT_CONFIG_CPI_REQUEST_RATE_MASK;
    p_gm_request->config_request.q_cpi_request_rate = p_inp_request->q_gm_cpi_request_rate;
    MSG_MED("Set CPI Rate %d", p_gm_request->config_request.q_cpi_request_rate, 0, 0);
  }
  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_CHAL_GPS_ENV_BO_MIN_MASK)
  {
    p_gm_request->config_request.q_mask |= SM_GM_CLIENT_CHAL_GPS_ENV_BO_MIN_MASK;
    p_gm_request->config_request.q_chal_gps_env_bo_min = p_inp_request->q_gm_chal_gps_env_bo_min;
  }

  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_CHAL_GPS_ENV_BO_MAX_MASK)
  {
    p_gm_request->config_request.q_mask |= SM_GM_CLIENT_CHAL_GPS_ENV_BO_MAX_MASK;
    p_gm_request->config_request.q_chal_gps_env_bo_max = p_inp_request->q_gm_chal_gps_env_bo_max;
  }

  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_CHAL_GPS_ENV_MS_DIST_GPS_MASK)
  {
    p_gm_request->config_request.q_mask |= SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_GPS_MASK;
    p_gm_request->config_request.q_chal_gps_env_ms_dist_gps = p_inp_request->q_gm_chal_gps_env_ms_dist_gps;
  }

  if(p_inp_request->t_gfc_qmi_gm_engine_conf_mask & GFC_QMI_GM_ENGINE_CONF_CHAL_GPS_ENV_MS_DIST_CPI_MASK)
  {
    p_gm_request->config_request.q_mask |= SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_CPI_MASK;
    p_gm_request->config_request.q_chal_gps_env_ms_dist_cpi = p_inp_request->q_gm_chal_gps_env_ms_dist_cpi;
  }
  
  if(0 == p_gm_request->config_request.q_mask)
  {
    /*Release the TxID*/
    (void)gfc_qmi_internal_get_qmi_trxn_id(p_gm_request->q_transaction_id);
  }
  MSG_MED("GFC_QMI:: Set Client config mask GFC:: 0x%x GM :: 0x%x , txn id :: 0x%x\n",
          p_inp_request->t_gfc_qmi_gm_engine_conf_mask,
          p_gm_request->config_request.q_mask,
          p_gm_request->q_transaction_id);

  return TRUE;

}


boolean gfc_qmi_internal_val_and_fill_req(
  gfc_qmi_op_type e_op_type,
  void *p_inp_request,
  void *p_gm_request,
  gfc_qmi_operation_result_e_type *p_return_code
)
{
  if((p_inp_request == NULL) ||
     (p_gm_request == NULL)||
     (p_return_code == NULL)
     ) 
  {
    MSG_MED("Invalid arguments in gfc_qmi_internal_val_and_fill_req", 0, 0, 0);
    return FALSE;
  }
  switch(e_op_type)
  {
    case GFC_QMI_OP_TYPE_ADD_GF:
      {
        gfc_qmi_add_geofence_req_s_type *p_gfc_qmi_add_req;
        sm_gm_add_request_type *p_gm_add_req;
        double d_lat_local = 0;
        double d_long_local = 0;

        p_gfc_qmi_add_req = (gfc_qmi_add_geofence_req_s_type*)p_inp_request;
        p_gm_add_req = (sm_gm_add_request_type*)p_gm_request;
        /* Check for mandatory parameters */
        if(
            (p_gfc_qmi_add_req->breach_event_req_mask == 0) ||
            (p_gfc_qmi_add_req->valid_opt_param_req_mask == 0)
          )
        {
          /* The mandatory parametrs are not specified , report error. */
          *p_return_code = GFC_QMI_ERROR_MANDATORY_PARAMETER_UNSPECIFIED;
          return FALSE;
        }
        if(p_gfc_qmi_add_req->valid_opt_param_req_mask & GFC_QMI_GEOFENCE_VALID_OPT_PARAM_REQ_START_TIME_MASK)
        {
          /* A valid start time is specified as an input. */
          p_gm_add_req->geofence.start_time = p_gfc_qmi_add_req->start_time_s;
        }
        if(p_gfc_qmi_add_req->valid_opt_param_req_mask & GFC_QMI_GEOFENCE_VALID_OPT_PARAM_REQ_DURATION_MASK)
        {
          /* A valid duration is specified as an input. */
          p_gm_add_req->geofence.stop_time = p_gfc_qmi_add_req->duration_s;
        }
        if(p_gfc_qmi_add_req->valid_opt_param_req_mask & GFC_QMI_GEOFENCE_VALID_OPT_PARAM_REQ_RESPONSIVENESS_MASK)
        {
          /* A valid responsiveness value is specified as an input. */
          switch(p_gfc_qmi_add_req->responsiveness)
          {
            case GFC_QMI_RESPONSIVENESS_LOW:
              p_gm_add_req->geofence.responsiveness = GEOFENCE_RESPONSIVENESS_LOW;
              break;
            case GFC_QMI_RESPONSIVENESS_MEDIUM:
              p_gm_add_req->geofence.responsiveness = GEOFENCE_RESPONSIVENESS_MEDIUM;
              break;
            case GFC_QMI_RESPONSIVENESS_HIGH:
              p_gm_add_req->geofence.responsiveness = GEOFENCE_RESPONSIVENESS_HIGH;
              break;
            case GFC_QMI_RESPONSIVENESS_ULTRA_HIGH:
              p_gm_add_req->geofence.responsiveness = GEOFENCE_RESPONSIVENESS_ULTRA_HIGH;
              break;
            case GFC_QMI_RESPONSIVENESS_CUSTOM:
              p_gm_add_req->geofence.responsiveness = GEOFENCE_RESPONSIVENESS_CUSTOM;
              p_gm_add_req->geofence.q_custom_responsiveness_secs = p_gfc_qmi_add_req->q_custom_responsiveness_value;
              MSG_MED("Set custom responsiveness to %d secs", p_gm_add_req->geofence.q_custom_responsiveness_secs, 0, 0);
              break;
            default:
              p_gm_add_req->geofence.responsiveness = GEOFENCE_RESPONSIVENESS_MEDIUM;
              break;
          }
        }

        if(p_gfc_qmi_add_req->valid_opt_param_req_mask & GFC_QMI_GEOFENCE_VALID_OPT_PARAM_REQ_CONFIDENCE_MASK)
        {
          /* A valid confidence value is specified as an input. */
          switch(p_gfc_qmi_add_req->confidence)
          {
            case GFC_QMI_CONFIDENCE_LOW:
              p_gm_add_req->geofence.confidence = GEOFENCE_CONFIDENCE_LOW;
              break;
            case GFC_QMI_CONFIDENCE_MEDIUM:
              p_gm_add_req->geofence.confidence = GEOFENCE_CONFIDENCE_MEDIUM;
              break;
            case GFC_QMI_CONFIDENCE_HIGH:
              p_gm_add_req->geofence.confidence = GEOFENCE_CONFIDENCE_HIGH;
              break;
            case GFC_QMI_CONFIDENCE_MAX:
              p_gm_add_req->geofence.confidence = GEOFENCE_CONFIDENCE_HIGH;
              break;
            default:
              p_gm_add_req->geofence.confidence = GEOFENCE_CONFIDENCE_HIGH;
              break;
          }
        } 
        else
        {
          p_gm_add_req->geofence.confidence = GEOFENCE_CONFIDENCE_HIGH;        
        }

        /* Copy the dwell type in the request */
        if(p_gfc_qmi_add_req->q_dwell_type_mask&GFC_QMI_GEOFENCE_DWELL_TYPE_INSIDE_MASK) 
        {
          p_gm_add_req->geofence.q_dwell_type_mask |= GEOFENCE_DWELL_TYPE_IN;  
          MSG_MED("Dwell type of inside requested", 0, 0, 0);
        }
        if(p_gfc_qmi_add_req->q_dwell_type_mask&GFC_QMI_GEOFENCE_DWELL_TYPE_OUTSIDE_MASK) 
        {
          p_gm_add_req->geofence.q_dwell_type_mask |= GEOFENCE_DWELL_TYPE_OUT;  
          MSG_MED("Dwell type of outside requested", 0, 0, 0);
        }
        if(p_gfc_qmi_add_req->q_dwell_type_mask&GFC_QMI_GEOFENCE_DWELL_TYPE_NONE_MASK) 
        {
          p_gm_add_req->geofence.q_dwell_type_mask |= GEOFENCE_DWELL_TYPE_NONE;  
        }
        /* Copy the dwell time into the request . */
        p_gm_add_req->geofence.q_dwell_time_secs = p_gfc_qmi_add_req->q_dwell_time_s;

        /*Have dynamic responsiveness enabled by default*/
        p_gm_add_req->geofence.dynamic_responsiveness = TRUE;

        /*Have context usage enabled by default*/
        p_gm_add_req->geofence.context_usage = TRUE;

        /*Enable Hystersis*/
        p_gm_add_req->geofence.hysteresis_required = TRUE;

        /* Initialise the request parameters for the geofence. */
        p_gm_add_req->client_handle = z_gfc_qmi_info.u_gm_handle;

        /* Map the transaction ID passed in by the QMI with the transaction ID of the GM  */
        p_gm_add_req->q_transaction_id = gfc_qmi_internal_getnext_gm_trxn_id(p_gfc_qmi_add_req->transaction_id);

        if(p_gm_add_req->q_transaction_id == GFC_QMI_INVALID_TRXN_ID)
        {
          /* Failed to get the GM transaction ID . Report error  */
          *p_return_code = GFC_QMI_ERROR_GENERAL;
          return FALSE;
        }

        if(p_gfc_qmi_add_req->opt_feature_req_mask & GFC_QMI_GEOFENCE_OPT_FEATURE_REQ_POS_REQ_ON_BREACH_MASK)
        {
          p_gm_add_req->geofence.location_estimate = TRUE;
        }
        // ToDo: No flag from QMI for this . We hardcode it for now.
        p_gm_add_req->geofence.target_area_provided = TRUE;
        /* The target area details. */
        p_gm_add_req->geofence.target_area.target_area_type = p_gfc_qmi_add_req->target_area.target_shape_type;

        // Factor to convert degrees ( referenced to WGS-84 ) into radians = ((1<<25)/180)
        d_lat_local = p_gfc_qmi_add_req->target_area.gfc_qmi_geofence_target_area_req_u.circ_target_area.latitude_d;
        d_long_local = p_gfc_qmi_add_req->target_area.gfc_qmi_geofence_target_area_req_u.circ_target_area.longitude_d;
        p_gm_add_req->geofence.target_area.u.circ_target_area.latitude = (sint31)((d_lat_local*(1<<25))/180);
        p_gm_add_req->geofence.target_area.u.circ_target_area.longitude = (sint31)((d_long_local*(1<<25))/180);
        p_gm_add_req->geofence.target_area.u.circ_target_area.radius = p_gfc_qmi_add_req->target_area.gfc_qmi_geofence_target_area_req_u.circ_target_area.radius_m;

        // ToDo:: Set the breach event type for the combination of events.
        if(p_gfc_qmi_add_req->breach_event_req_mask & GFC_QMI_BREACH_GEOFENCE_EVENT_REQ_ENTERING_MASK)
        {           // Request Entering breach type event.
          p_gm_add_req->geofence.event_type^=GEOFENCE_EVENT_ENTERING_AREA;
        }
        if(p_gfc_qmi_add_req->breach_event_req_mask & GFC_QMI_BREACH_GEOFENCE_EVENT_REQ_LEAVING_MASK)
        {
          // Request Leaving breach type event.
          p_gm_add_req->geofence.event_type^=GEOFENCE_EVENT_LEAVING_AREA;
        }

        // ToDo:: Hardcoding as of now.
        p_gm_add_req->geofence.num_of_area_ids = 0;
        p_gm_add_req->geofence.area_id_list = NULL;
        p_gm_add_req->geofence.max_num_reports = 0xFFFFFFFF;

        // ToDo:: Add check for invalid geofence and transaction ID
        // ToDo:: Not using this field for now
        p_gm_add_req->geofence.client_blob = NULL;
        p_gm_add_req->geofence.client_blob_size = 0;
      }
      break;
    case GFC_QMI_OP_TYPE_DELETE_GF:
      {
        gfc_qmi_delete_geofence_req_s_type *p_gfc_qmi_del_req;
        sm_gm_delete_request_type *p_gm_del_req;
        p_gfc_qmi_del_req = (gfc_qmi_delete_geofence_req_s_type*)p_inp_request;
        p_gm_del_req = (sm_gm_delete_request_type*)p_gm_request;
        /* Get the transaction ID for this query operation */
        if((p_gm_del_req->q_transaction_id = gfc_qmi_internal_getnext_gm_trxn_id(p_gfc_qmi_del_req->transaction_id))
            == GFC_QMI_INVALID_TRXN_ID)
        {
          /* Error fetching the transaction ID */
          *p_return_code = GFC_QMI_ERROR_GENERAL;
          return FALSE;
        }
        /* Geofence to be deleted */
        p_gm_del_req->geofence_id = p_gfc_qmi_del_req->geofence_id;
        return TRUE;
      }
      break;
    case GFC_QMI_OP_TYPE_PURGE_GF:
      {
        sm_gm_purge_request_type *p_gm_purge_req;
        p_gm_purge_req = (sm_gm_purge_request_type*)p_gm_request;
        /* Get the transaction ID for the purge GF operation. There is no transaction ID passed in by QMI
           for the purge operation. Pass it as 0xFF */
        if((p_gm_purge_req->q_transaction_id = gfc_qmi_internal_getnext_gm_trxn_id(0xFF))
            == GFC_QMI_INVALID_TRXN_ID)
        {
          /* The mandatory parametrs are not specified , report error. */
          *p_return_code = GFC_QMI_ERROR_GENERAL;
          return FALSE;
        }
      }
      break;
    case GFC_QMI_OP_TYPE_EDIT_GF:
      {
        gfc_qmi_edit_geofence_req_s_type *p_gfc_qmi_edit_req;
        sm_gm_edit_request_type *p_gm_edit_req;
        p_gfc_qmi_edit_req = (gfc_qmi_edit_geofence_req_s_type*)p_inp_request;
        p_gm_edit_req = (sm_gm_edit_request_type*)p_gm_request;

        /* Get the transaction ID for this edit operation */
        if((p_gm_edit_req->q_transaction_id = gfc_qmi_internal_getnext_gm_trxn_id(p_gfc_qmi_edit_req->transaction_id))
            == GFC_QMI_INVALID_TRXN_ID)
        {
          /* The mandatory parametrs are not specified , report error. */
          *p_return_code = GFC_QMI_ERROR_GENERAL;
          return FALSE;
        }
        /* The geofence ID for this operation */
        p_gm_edit_req->geofence_id = p_gfc_qmi_edit_req->geofence_id;
        /* Check for the parameters to be edited */
        if(p_gfc_qmi_edit_req->edit_param_mask & GFC_QMI_EDIT_GEOFENCE_PARAM_START_TIME_MASK)
        {
          /* The start time of the geofence needs to be edited */
          p_gm_edit_req->geofence_edit_mask^=SM_GM_GEOFENCE_EDIT_START_TIME;
          p_gm_edit_req->start_time = p_gfc_qmi_edit_req->start_time_s;
        }
        if(p_gfc_qmi_edit_req->edit_param_mask & GFC_QMI_EDIT_GEOFENCE_PARAM_DURATION_MASK)
        {
          /* The duration of the geofence needs to be edited */
          p_gm_edit_req->geofence_edit_mask^=SM_GM_GEOFENCE_EDIT_STOP_TIME;
          p_gm_edit_req->stop_time = p_gfc_qmi_edit_req->duration_s;
        }

        if(p_gfc_qmi_edit_req->edit_param_mask & GFC_QMI_EDIT_GEOFENCE_PARAM_RESPONSIVENESS_MASK)
        {
          p_gm_edit_req->geofence_edit_mask^=SM_GM_GEOFENCE_EDIT_RESPONSIVENESS;

          /* A valid responsiveness value is specified as an input. */
          switch(p_gfc_qmi_edit_req->responsiveness)
          {
            case GFC_QMI_RESPONSIVENESS_LOW:
              p_gm_edit_req->responsiveness = GEOFENCE_RESPONSIVENESS_LOW;
              break;
            case GFC_QMI_RESPONSIVENESS_MEDIUM:
              p_gm_edit_req->responsiveness = GEOFENCE_RESPONSIVENESS_MEDIUM;
              break;
            case GFC_QMI_RESPONSIVENESS_HIGH:
              p_gm_edit_req->responsiveness = GEOFENCE_RESPONSIVENESS_HIGH;
              break;
            case GFC_QMI_RESPONSIVENESS_ULTRA_HIGH:
              p_gm_edit_req->responsiveness = GEOFENCE_RESPONSIVENESS_ULTRA_HIGH;
              break;
            default:
              p_gm_edit_req->responsiveness = GEOFENCE_RESPONSIVENESS_MEDIUM;
              break;
          }
        }
        if(p_gfc_qmi_edit_req->edit_param_mask & GFC_QMI_EDIT_GEOFENCE_PARAM_CONFIDENCE_MASK)
        {
          p_gm_edit_req->geofence_edit_mask^=SM_GM_GEOFENCE_EDIT_CONFIDENCE;

          /* A valid confidence value is specified as an input. */
          switch(p_gfc_qmi_edit_req->confidence)
          {
            case GFC_QMI_CONFIDENCE_LOW:
              p_gm_edit_req->confidence = GEOFENCE_CONFIDENCE_LOW;
              break;
            case GFC_QMI_CONFIDENCE_MEDIUM:
              p_gm_edit_req->confidence = GEOFENCE_CONFIDENCE_MEDIUM;
              break;
            case GFC_QMI_CONFIDENCE_HIGH:
              p_gm_edit_req->confidence = GEOFENCE_CONFIDENCE_HIGH;
              break;
            case GFC_QMI_CONFIDENCE_MAX:
              p_gm_edit_req->confidence = GEOFENCE_CONFIDENCE_HIGH;
              break;
            default:
              p_gm_edit_req->confidence = GEOFENCE_CONFIDENCE_HIGH;
              break;
          }
        }

        if(p_gfc_qmi_edit_req->edit_param_mask & GFC_QMI_EDIT_GEOFENCE_PARAM_BREACH_EVENT_TYPE_MASK)
        {
          /* The breach event type of the geofence needs to be edited */
          p_gm_edit_req->geofence_edit_mask^=SM_GM_GEOFENCE_EDIT_EVENT_TYPE ;

          /* The actual breach events */
          if(p_gfc_qmi_edit_req->breach_event_req_mask & GFC_QMI_BREACH_GEOFENCE_EVENT_REQ_ENTERING_MASK)
          {
            p_gm_edit_req->event_type^=GEOFENCE_EVENT_ENTERING_AREA;
          }
          if(p_gfc_qmi_edit_req->breach_event_req_mask & GFC_QMI_BREACH_GEOFENCE_EVENT_REQ_LEAVING_MASK)
          {
            p_gm_edit_req->event_type^=GEOFENCE_EVENT_LEAVING_AREA;
          }
        }
      }
      break;
    case GFC_QMI_OP_TYPE_QUERY_GF:
      {
        gfc_qmi_query_geofence_req_s_type *p_gfc_qmi_query_gf_req;
        sm_gm_query_geofence_status_request_type *p_gm_query_gf_req;
        p_gfc_qmi_query_gf_req = (gfc_qmi_query_geofence_req_s_type*)p_inp_request;
        p_gm_query_gf_req = (sm_gm_query_geofence_status_request_type*)p_gm_request;

        /* Get the transaction ID for this query operation */
        if((p_gm_query_gf_req->q_transaction_id = gfc_qmi_internal_getnext_gm_trxn_id(p_gfc_qmi_query_gf_req->transaction_id))
            == GFC_QMI_INVALID_TRXN_ID)
        {
          /* Error fetching the transaction ID */
          *p_return_code = GFC_QMI_ERROR_GENERAL;
          return FALSE;
        }
        /* Geofence to be queried */
        p_gm_query_gf_req->geofence_id = p_gfc_qmi_query_gf_req->geofence_id;
      }
      break;
    case GFC_QMI_OP_TYPE_QUERY_GF_IDS:
      {
        gfc_qmi_query_geofence_ids_req_s_type *p_gfc_qmi_query_gf_ids_req;
        sm_gm_geofences_handles_request_type *p_gm_query_gf_ids_req;
        p_gfc_qmi_query_gf_ids_req = (gfc_qmi_query_geofence_ids_req_s_type*)p_inp_request;
        p_gm_query_gf_ids_req = (sm_gm_geofences_handles_request_type *)p_gm_request;
        /* Get the transaction ID for this query operation */
        if((p_gm_query_gf_ids_req->q_transaction_id = gfc_qmi_internal_getnext_gm_trxn_id(p_gfc_qmi_query_gf_ids_req->transaction_id))
            == GFC_QMI_INVALID_TRXN_ID)
        {
          /* Error fetching the transaction ID */
          *p_return_code = GFC_QMI_ERROR_GENERAL;
          return FALSE;
        }
        /* Assign the geofence origin to the GM request */
        switch(p_gfc_qmi_query_gf_ids_req->origin)
        {
          case GFC_QMI_GEOFENCE_ORIGIN_ALL:
            p_gm_query_gf_ids_req->geofence_origin = GEOFENCE_TYPE_ANY;
            break;
          case GFC_QMI_GEOFENCE_ORIGIN_MO:
            p_gm_query_gf_ids_req->geofence_origin = GEOFENCE_TYPE_MO;
            break;
          case GFC_QMI_GEOFENCE_ORIGIN_NI:
            p_gm_query_gf_ids_req->geofence_origin = GEOFENCE_TYPE_NI;
            break;
          default:
            p_gm_query_gf_ids_req->geofence_origin = GEOFENCE_TYPE_ANY;
            break;
        }
      }
      break;
    case GFC_QMI_OP_TYPE_SET_GF_ENGINE_CONFIG:
      {

        if(FALSE == gfc_qmi_internal_conv_set_gf_engine_config_req_to_gm((gfc_qmi_set_geofence_engine_config_req_s_type *)p_inp_request,
                                                                        (sm_gm_set_geofence_engine_config_request_type *)p_gm_request))
        {
          *p_return_code = GFC_QMI_ERROR_GENERAL;
          return FALSE;
        }
      }
      break;
    case GFC_QMI_OP_TYPE_SET_GF_CLIENT_CONFIG:
      {
    
        if(FALSE == gfc_qmi_internal_conv_set_gf_client_config_req_to_gm((gfc_qmi_set_geofence_engine_config_req_s_type *)p_inp_request,
                                                                        (sm_gm_set_geofence_client_config_request_type *)p_gm_request))
        {
          *p_return_code = GFC_QMI_ERROR_GENERAL;
          return FALSE;
        }
      }
      break;      
    default:
      {
	    *p_return_code = GFC_QMI_ERROR_GENERAL;
        return FALSE;
      }
  }// end of switch

  return TRUE;
}



/*===========================================================================

FUNCTION gfc_qmi_internal_map_gm_error_code_to_gfc

DESCRIPTION
  This function maps the return error codes of GM to error codes of QMI .
  The error codes may arise from any operation ( add, delete, purge etc. )

PARAMETERS
  gm_error_code: The error code from the GM .
  p_gfc_qmi_error_code: The error code which is to be converted to.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void gfc_qmi_internal_map_gm_error_code_to_gfc(uint8 gm_ret_code, gfc_qmi_operation_result_e_type* p_gfc_ret_code)
{
  // Map the error code returned from GM.
  switch(gm_ret_code)
  {
    case GEOFENCE_ERROR_INVALID_CLIENT_HANDLE:
      *p_gfc_ret_code = GFC_QMI_ERROR_INVALID_CLIENT_HANDLE;
      break;
    case GEOFENCE_ERROR_IPC_SEND_FAIL:
      *p_gfc_ret_code = GFC_QMI_ERROR_COMMUNICATION_FAILURE;
      break;
    case GEOFENCE_ERROR_MAX_GEOFENCE_PROGRAMED:
      *p_gfc_ret_code = GFC_QMI_ERROR_MAX_GEOFENCE_PROGRAMED;
      break;
    case GEOFENCE_ERROR_GEOFENCE_NOT_FOUND:
      *p_gfc_ret_code = GFC_QMI_ERROR_GEOFENCE_NOT_FOUND;
      break;
    case GEOFENCE_ERROR_GEOFENCE_WRONG_CLIENT:
      *p_gfc_ret_code = GFC_QMI_ERROR_GEOFENCE_WRONG_CLIENT;
      break;
    case GEOFENCE_ERROR_INCORRECT_PARAMS:
      *p_gfc_ret_code = GFC_QMI_ERROR_INCORRECT_PARAMS;
      break;
    case GEOFENCE_ERROR_NONE:
      *p_gfc_ret_code = GFC_QMI_ERROR_NONE;
      break;
    case GEOFENCE_ERROR_MEMORY_ALLOC_FAIL:
      *p_gfc_ret_code = GFC_QMI_ERROR_INSUFFICIENT_MEMORY;
      break;
    default:
      *p_gfc_ret_code = GFC_QMI_ERROR_GENERAL;
      break;
  }
  MSG_MED("gfc_qmi_internal_map_gm_error_code_to_gfc GM error code:: %d , gfc :: : %d", gm_ret_code, *p_gfc_ret_code, 0);
}

/*===========================================================================

FUNCTION gfc_qmi_internal_add_gf_rsp_handler

DESCRIPTION
  This function notifies the QMI of the result of geofence add operation

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_add_gf_rsp_handler(sm_gm_client_notification_type* p_gm_notify_data)
{
  gfc_qmi_add_geofence_rsp_s_type z_add_rsp;
  z_add_rsp.geofence_id = p_gm_notify_data->notification.geofence_id;

  // Map the error code returned from GM.
  gfc_qmi_internal_map_gm_error_code_to_gfc(p_gm_notify_data->notification.return_code,
                                            &z_add_rsp.return_code);
  /* Retrieve the QMI transaction ID */
  z_add_rsp.transaction_id = gfc_qmi_internal_get_qmi_trxn_id(p_gm_notify_data->q_transaction_id);

  // Fetch the origin of the geofence from GM notification.
  switch(p_gm_notify_data->notification.geofence_type)
  {
    case GEOFENCE_TYPE_MO:
      z_add_rsp.origin = GFC_QMI_GEOFENCE_ORIGIN_MO;
      break;
    case GEOFENCE_TYPE_NI:
      z_add_rsp.origin = GFC_QMI_GEOFENCE_ORIGIN_NI;
      break;
    default:
      z_add_rsp.origin = GFC_QMI_GEOFENCE_ORIGIN_MAX;
      break;
  }


  if(z_add_rsp.return_code == GFC_QMI_ERROR_NONE)
  {
    MSG_HIGH("GFC_QMI::Geofence Add event SUCCESS :: TID=%d GID=%d Origin=%d \n",
             z_add_rsp.transaction_id,
             z_add_rsp.geofence_id,
             z_add_rsp.origin);
  }
  else
  {
    MSG_HIGH("GFC_QMI::Geofence add event FAILURE :: TID=%d Error_code=%d \n",
             z_add_rsp.transaction_id,
             z_add_rsp.return_code,
             0);
  }


  // Notify the QMI
  if (z_gfc_qmi_info.z_gfc_qmi_cb_table.p_add_rsp_func != NULL)
  {
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_add_rsp_func(&z_add_rsp);
  }

}

/*===========================================================================

FUNCTION gfc_qmi_internal_delete_gf_rsp_handler

DESCRIPTION
  This function notifies the QMI of the result of the geofence delete operation

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_delete_gf_rsp_handler(sm_gm_client_notification_type* p_gm_notify_data)
{
  gfc_qmi_delete_geofence_rsp_s_type z_del_rsp;
  z_del_rsp.geofence_id = p_gm_notify_data->notification.geofence_id;

  // Map the error code returned from GM.
  gfc_qmi_internal_map_gm_error_code_to_gfc(p_gm_notify_data->notification.return_code,
                                            &z_del_rsp.return_code);

  /* Retrieve the QMI transaction ID */
  z_del_rsp.transaction_id = gfc_qmi_internal_get_qmi_trxn_id(p_gm_notify_data->q_transaction_id);

  MSG_HIGH("GFC_QMI::Geofence delete rsp :: TID from GM %d\n", p_gm_notify_data->q_transaction_id, 0, 0);

  // Fetch the deletion reason from the GM notification.
  switch(p_gm_notify_data->notification.return_code)
  {
    case GEOFENCE_DELETE_ON_CLIENT_REQUEST:
      /* Geofence deleted on client request */
      z_del_rsp.del_reason = GFC_QMI_GEOFENCE_DEL_REASON_MO_CLIENT_REQ;
      z_del_rsp.return_code = GFC_QMI_ERROR_NONE;
      break;
    case GEOFENCE_DELETE_ON_STOP_TIME :
      /* Geofence deleted because its duration is up */
      z_del_rsp.del_reason = GFC_QMI_GEOFENCE_DEL_REASON_DURATION;
      z_del_rsp.return_code = GFC_QMI_ERROR_NONE;
      break;
    case GEOFENCE_ERROR_GEOFENCE_NOT_FOUND :
      /* Geofence not found to be deleted. Update the delete reason. The return code
         is already updated . */
      z_del_rsp.del_reason = GFC_QMI_GEOFENCE_DEL_REASON_MO_CLIENT_REQ;
      break;
    default:
      /* We do not know why the geofence was deleted */
      z_del_rsp.del_reason = GFC_QMI_GEOFENCE_DEL_REASON_UNKNOWN;
      z_del_rsp.return_code = GFC_QMI_ERROR_GENERAL;
      break;
  }
  // Fetch the origin of the geofence from GM notification.
  switch(p_gm_notify_data->notification.geofence_type)
  {
    case GEOFENCE_TYPE_MO:
      z_del_rsp.origin = GFC_QMI_GEOFENCE_ORIGIN_MO;
      break;
    case GEOFENCE_TYPE_NI:
      z_del_rsp.origin = GFC_QMI_GEOFENCE_ORIGIN_NI;
      break;
    default:
      break;
  }

  /* Check whether the deletion happened correctly */
  if(z_del_rsp.return_code == GFC_QMI_ERROR_NONE)
  {
    MSG_HIGH("GFC_QMI::Geofence delete SUCCESS :: GID=%d Reason=%d TID=%d\n", z_del_rsp.geofence_id, z_del_rsp.del_reason, z_del_rsp.transaction_id);
  }
  else
  {
    MSG_HIGH("GFC_QMI::Geofence delete FAILURE ::  GID=%d Error_code=%d \n",
             z_del_rsp.geofence_id,
             z_del_rsp.return_code,
             0);
  }
  if(z_gfc_qmi_info.z_gfc_qmi_cb_table.p_delete_rsp_func != NULL)
  {
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_delete_rsp_func(&z_del_rsp);
  }
}


/*===========================================================================

FUNCTION gfc_qmi_internal_purge_gf_rsp_handler

DESCRIPTION
  This function handles the purge geofence response from GM .

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_purge_gf_rsp_handler(sm_gm_client_notification_type* p_gm_notify_data)
{
  //gfc_qmi_operation_result_e_type return_code;  /* Removed set-but-not-used variable */
  MSG_HIGH("GFC_QMI::Purge geofence response code %d received from GM \n",
             p_gm_notify_data->notification.return_code,
             0,
             0);

  /* Retrieve the QMI transaction ID */
  (void)gfc_qmi_internal_get_qmi_trxn_id(p_gm_notify_data->q_transaction_id);

  MSG_HIGH("GFC_QMI::Geofence purge rsp :: TID from GM %d\n", p_gm_notify_data->q_transaction_id, 0, 0);
  if(z_gfc_qmi_info.z_gfc_qmi_cb_table.p_purge_rsp_func != NULL)
  {
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_purge_rsp_func(p_gm_notify_data->notification.return_code);
  }
}

/*===========================================================================

FUNCTION gfc_qmi_internal_edit_gf_rsp_handler

DESCRIPTION
  This function notifies the QMI of the result of geofence edit operation

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_edit_gf_rsp_handler(sm_gm_client_notification_type* p_gm_notify_data)
{
  gfc_qmi_edit_geofence_rsp_s_type z_edit_rsp;
  z_edit_rsp.geofence_id = p_gm_notify_data->notification.geofence_id;

  // Map the error code returned from GM.
  gfc_qmi_internal_map_gm_error_code_to_gfc(p_gm_notify_data->notification.return_code,
                                            &z_edit_rsp.return_code);

  /* Retrieve the QMI transaction ID */
  z_edit_rsp.transaction_id = gfc_qmi_internal_get_qmi_trxn_id(p_gm_notify_data->q_transaction_id);

  // suds84. ToDo:: the parameters succesfully edited ?

  if(z_edit_rsp.return_code == GFC_QMI_ERROR_NONE)
  {
    MSG_HIGH("GFC_QMI::Geofence Edit SUCCESS :: TID=%d GID=%d \n",
             z_edit_rsp.transaction_id,
             z_edit_rsp.geofence_id,
             0);
  }
  else
  {
    MSG_HIGH("GFC_QMI::Geofence Edit FAILURE :: TID=%d Error_code=%d \n",
             z_edit_rsp.transaction_id,
             z_edit_rsp.return_code,
             0);
  }
  // Notify the QMI
  if(z_gfc_qmi_info.z_gfc_qmi_cb_table.p_edit_rsp_func != NULL)
  {
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_edit_rsp_func(&z_edit_rsp);
  }

}

/*===========================================================================

FUNCTION gfc_qmi_internal_query_gf_rsp_handler

DESCRIPTION
  This function notifies the QMI of the result of the geofence query operation.

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_query_gf_rsp_handler(sm_gm_geofence_status_notification_type* p_gm_notify_data)
{
  gfc_qmi_query_geofence_rsp_s_type z_query_rsp;
  double d_lat_local = 0;
  double d_long_local = 0;

  if(p_gm_notify_data == NULL) 
  {
    MSG_MED("Null pointer passed in to the function.", 0, 0, 0);
    return;
  }
  memset(&z_query_rsp, 0, sizeof(z_query_rsp));

  /* Retrieve the QMI transaction ID */
  z_query_rsp.transaction_id = gfc_qmi_internal_get_qmi_trxn_id(p_gm_notify_data->q_transaction_id);
  z_query_rsp.geofence_id = p_gm_notify_data->geofence_id;
  // Map the error code returned from GM.
  gfc_qmi_internal_map_gm_error_code_to_gfc(p_gm_notify_data->return_code,
                                            &z_query_rsp.return_code);

  /* The type of geofence  */
  switch(p_gm_notify_data->geofence_type)
  {
    case GEOFENCE_TYPE_MO:
      z_query_rsp.geofence_origin = GFC_QMI_GEOFENCE_ORIGIN_MO;
      break;
    case GEOFENCE_TYPE_NI:
      z_query_rsp.geofence_origin = GFC_QMI_GEOFENCE_ORIGIN_NI;
      break;
    default :
      break;
  }
  /* The position status of the handset with respect to the geofence  */
  switch(p_gm_notify_data->geofence_pos_status)
  {
    case GEOFENCE_POSITION_STATUS_UNKNOWN:
      z_query_rsp.pos_status = GFC_QMI_POS_UNKNOWN;
      break;
    case GEOFENCE_POSITION_STATUS_INSIDE:
      z_query_rsp.pos_status = GFC_QMI_POS_INSIDE;
      break;
    case GEOFENCE_POSITION_STATUS_OUTSIDE:
      z_query_rsp.pos_status = GFC_QMI_POS_OUTSIDE;
      break;
  }
  z_query_rsp.target_area.target_shape_type = (gfc_qmi_geofence_target_shape_req_e_type)p_gm_notify_data->target_area.target_area_type;

  d_lat_local = (double)p_gm_notify_data->target_area.u.circ_target_area.latitude;
  d_long_local = (double)p_gm_notify_data->target_area.u.circ_target_area.longitude;
  // Convert the latitude and longitude values from radians into degrees before sending to QMI
  z_query_rsp.target_area.gfc_qmi_geofence_target_area_req_u.circ_target_area.latitude_d = ((double)d_lat_local*(double)180)/(double)(1<<25);
  z_query_rsp.target_area.gfc_qmi_geofence_target_area_req_u.circ_target_area.longitude_d = ((double)d_long_local*(double)180)/(double)(1<<25);
  z_query_rsp.target_area.gfc_qmi_geofence_target_area_req_u.circ_target_area.radius_m = (double)p_gm_notify_data->target_area.u.circ_target_area.radius;

  /* Obtain the responsiveness value for this geofence. */
  switch(p_gm_notify_data->responsiveness)
  {
    case GEOFENCE_RESPONSIVENESS_LOW:
      z_query_rsp.responsiveness = GFC_QMI_RESPONSIVENESS_LOW;
      break;
    case GEOFENCE_RESPONSIVENESS_MEDIUM:
      z_query_rsp.responsiveness = GFC_QMI_RESPONSIVENESS_MEDIUM;
      break;
    case GEOFENCE_RESPONSIVENESS_HIGH:
      z_query_rsp.responsiveness = GFC_QMI_RESPONSIVENESS_HIGH;
      break;
    case GEOFENCE_RESPONSIVENESS_ULTRA_HIGH:
      z_query_rsp.responsiveness = GFC_QMI_RESPONSIVENESS_ULTRA_HIGH;
      break;
    case GEOFENCE_RESPONSIVENESS_CUSTOM:
      z_query_rsp.responsiveness = GFC_QMI_RESPONSIVENESS_CUSTOM;
      break;
    default:
      z_query_rsp.responsiveness = GFC_QMI_RESPONSIVENESS_MEDIUM;
      break;
  }

  /* Obtain the confidence value for this geofence. */
  switch(p_gm_notify_data->confidence)
  {
    case GEOFENCE_CONFIDENCE_LOW:
      z_query_rsp.confidence = GFC_QMI_CONFIDENCE_LOW;
      break;
    case GEOFENCE_CONFIDENCE_MEDIUM:
      z_query_rsp.confidence = GFC_QMI_CONFIDENCE_MEDIUM;
      break;
    case GEOFENCE_CONFIDENCE_HIGH:
      z_query_rsp.confidence = GFC_QMI_CONFIDENCE_HIGH;
      break;
    default:
      z_query_rsp.confidence = GFC_QMI_CONFIDENCE_HIGH;
      break;
  }


  if(z_query_rsp.return_code == GFC_QMI_ERROR_NONE)
  {
    MSG_HIGH("GFC_QMI::Query Geofence SUCCESS :: TID=%d GID=%d POS_STATUS=%d \n",
             z_query_rsp.transaction_id,
             z_query_rsp.geofence_id,
             z_query_rsp.pos_status);
  }
  else
  {
    MSG_HIGH("GFC_QMI::Query Geofence FAILURE :: TID=%d Error_code=%d \n",
             z_query_rsp.transaction_id,
             z_query_rsp.return_code,
             0);
  }
  if(z_gfc_qmi_info.z_gfc_qmi_cb_table.p_query_rsp_func != NULL)
  {
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_query_rsp_func(&z_query_rsp);
  }
}


/*===========================================================================

FUNCTION gfc_qmi_internal_query_gf_ids_rsp_handler

DESCRIPTION
  This function notifies the QMI of the result of getting ALL geofence IDs operation.

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_query_gf_ids_rsp_handler(sm_gm_geofence_handels_notification_type* p_gm_notify_data)
{
  gfc_qmi_query_geofence_ids_rsp_s_type z_query_gf_ids_rsp;
  z_query_gf_ids_rsp.num_ids = p_gm_notify_data->num_of_geofence_handles;
  /* Retrieve the QMI transaction ID */
  z_query_gf_ids_rsp.transaction_id =  gfc_qmi_internal_get_qmi_trxn_id(p_gm_notify_data->q_transaction_id);
  // suds84. ToDo:: Match the return code from GM
  // z_query_gf_ids_rsp.return_code = p_gm_notify_data->return_code;
  z_query_gf_ids_rsp.return_code = GFC_QMI_ERROR_NONE;
  /* Point to the data given by GM */
  z_query_gf_ids_rsp.geofence_ids = (uint32*)p_gm_notify_data->geofence_handles;

  if(z_query_gf_ids_rsp.return_code == GFC_QMI_ERROR_NONE)
  {
    MSG_HIGH("GFC_QMI::Query Geofence SUCCESS :: TID=%d NUM_IDS=%d \n",
             z_query_gf_ids_rsp.transaction_id,
             z_query_gf_ids_rsp.num_ids,
             0);
  }
  else
  {
    MSG_HIGH("GFC_QMI::Query Geofence FAILURE :: TID=%d Error_code=%d \n",
             z_query_gf_ids_rsp.transaction_id,
             z_query_gf_ids_rsp.return_code,
             0);
  }

  if(z_gfc_qmi_info.z_gfc_qmi_cb_table.p_geofence_ids_rsp_func != NULL)
  {
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_geofence_ids_rsp_func(&z_query_gf_ids_rsp);
  }

}

/*===========================================================================

FUNCTION gfc_qmi_construct_utc_time_from_lr

DESCRIPTION
  This function constructs UTC time form LR

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static boolean gfc_qmi_construct_utc_time_from_lr(tm_lr_resp_type *pos_fix_p,
                                                  pdsm_utc_time_s_type *utc_time_p)
{
  qword  milliseconds;
  me_Clock z_Clock;
  tm_CalendarTimeT z_UtcTime;
  uint8 q_LeapSeconds = 0;
  uint32 q_UtcFixTime;


  if(pos_fix_p == NULL || utc_time_p == NULL)
  {
    MSG_ERROR("Null pointers passed to gfc_qmi_construct_utc_time_from_lr",0,0,0);
    return FALSE;
  }

  /* Ext pos data*/
  gps_total_msec(pos_fix_p->timestamp, milliseconds );

  gps_week_ms( milliseconds, &(z_Clock.z_Time.w_GpsWeek),
               &(z_Clock.z_Time.q_GpsMsec) );
  /* The above gps time conversion routines may cause some rounding errors
   * that cause the millisecond value not to match what had originally been
   * sent in the PE structure.  The following code checks the value and makes
   * an adjustment if needed
   */
  if (z_Clock.z_Time.q_GpsMsec % SEC_MSECS != pos_fix_p->timestamp_msec)
  {
      uint32 q_TempMS = z_Clock.z_Time.q_GpsMsec / SEC_MSECS;
      q_TempMS *= SEC_MSECS;
      z_Clock.z_Time.q_GpsMsec = q_TempMS + pos_fix_p->timestamp_msec;
  }
  /* Check for week rollover */
  gps_MsecWeekLimit((int32 *) &z_Clock.z_Time.q_GpsMsec, &z_Clock.z_Time.w_GpsWeek );

  /* Compute UTC time of fix, adjusting for the GPS leap seconds. */
  q_LeapSeconds = cgps_GetNvUtcOffset(); /*Hardcode this for now, call a PE provided API when available*/

  if (z_Clock.z_Time.q_GpsMsec/SEC_MSECS < q_LeapSeconds)
  {
    q_UtcFixTime = z_Clock.z_Time.q_GpsMsec + (WEEK_SECS - q_LeapSeconds) * SEC_MSECS;
  }
  else
  {
    q_UtcFixTime = z_Clock.z_Time.q_GpsMsec - q_LeapSeconds * SEC_MSECS;
  }

  tm_util_construct_gps_time(z_Clock.z_Time.w_GpsWeek,
                             z_Clock.z_Time.q_GpsMsec,
                             q_UtcFixTime,
                             &z_UtcTime);
  tm_util_time_to_pdsm_time(&z_UtcTime, utc_time_p);

  return TRUE;
}
/*===========================================================================

FUNCTION gfc_qmi_internal_breach_gf_rsp_handler

DESCRIPTION
  This function notifies the QMI when the geofence breach occurs.

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_breach_gf_rsp_handler(sm_gm_client_breach_notify_type *p_gm_notify_data)
{
  gfc_qmi_breach_geofence_rsp_s_type z_breach_rsp;
  tm_lr_resp_type breach_pos_fix;
  float f_vel_east, f_vel_north, f_tmp;

  z_breach_rsp.geofence_id = p_gm_notify_data->geofence_id;

  if(p_gm_notify_data->fix_included == TRUE)
  {
    z_breach_rsp.pos_rep_mask = GFC_QMI_POS_REPORT_ON_BREACH_TRUE;
  }
  else
  {
    z_breach_rsp.pos_rep_mask = GFC_QMI_POS_REPORT_ON_BREACH_FALSE;
  }
  /* Fill in the breach type. */
  switch(p_gm_notify_data->breach_type)
  {
    case GEOFENCE_EVENT_ENTERING_AREA:
      z_breach_rsp.breach_type = GFC_QMI_BREACH_GEOFENCE_TYPE_ENTERING;
      break;
    case GEOFENCE_EVENT_LEAVING_AREA:
      z_breach_rsp.breach_type = GFC_QMI_BREACH_GEOFENCE_TYPE_LEAVING;
      break;
    default:
      z_breach_rsp.breach_type = GFC_QMI_BREACH_GEOFENCE_TYPE_UNKNOWN;
      break;
  }
  /* Fill in the geofence origin type. */
  switch(p_gm_notify_data->geofence_type)
  {
    case GEOFENCE_TYPE_MO:
      z_breach_rsp.origin = GFC_QMI_GEOFENCE_ORIGIN_MO;
      break;
    case GEOFENCE_TYPE_NI:
      z_breach_rsp.origin = GFC_QMI_GEOFENCE_ORIGIN_NI;
      break;
    default:
      break;
  }

  z_breach_rsp.confidence_rep_mask = GFC_QMI_CONFIDENCE_REPORT_ON_BREACH_TRUE;
  switch(p_gm_notify_data->breach_confidence)
  {
    case GEOFENCE_CONFIDENCE_HIGH:
      z_breach_rsp.confidence = GFC_QMI_CONFIDENCE_HIGH;
      break;
    case GEOFENCE_CONFIDENCE_MEDIUM:
      z_breach_rsp.confidence = GFC_QMI_CONFIDENCE_MEDIUM;
      break;
    case GEOFENCE_CONFIDENCE_LOW:
      z_breach_rsp.confidence = GFC_QMI_CONFIDENCE_LOW;
      break;
    default:
      break;
  }


  tm_util_convert_lr(&p_gm_notify_data->pos_fix_rpt, &breach_pos_fix);

  /* populate the position fix information for the breach */
  // Convert the heading angle
  z_breach_rsp.pos_fix.heading = (uint16) tm_util_nearest_long( (double)breach_pos_fix.heading / (double)HEADING_SCALE );
  z_breach_rsp.pos_fix.heading_valid = breach_pos_fix.heading_valid;
  // Convert to altitude With Respect to Ellipsoid
  z_breach_rsp.pos_fix.height = (float)(breach_pos_fix.height - GFC_QMI_ALTITUDE_WRT_WGS84_ELLIPSOID_OFFSET);
  z_breach_rsp.pos_fix.height_incl = breach_pos_fix.height_incl;

  z_breach_rsp.pos_fix.loc_uncrtnty_a = z_breach_rsp.pos_fix.loc_uncrtnty_p = z_breach_rsp.pos_fix.loc_uncrtnty_v = 0;

  if(breach_pos_fix.loc_uncrtnty_a < MAX_POS_UNC_IND)
  {
    z_breach_rsp.pos_fix.loc_uncrtnty_a = f_PosUncArr[breach_pos_fix.loc_uncrtnty_a];
  }
  if(breach_pos_fix.loc_uncrtnty_p < MAX_POS_UNC_IND)
  {
    z_breach_rsp.pos_fix.loc_uncrtnty_p = f_PosUncArr[breach_pos_fix.loc_uncrtnty_p];
  }
  if(breach_pos_fix.loc_uncrtnty_v < MAX_POS_UNC_IND)
  {
    z_breach_rsp.pos_fix.loc_uncrtnty_v = f_PosUncArr[breach_pos_fix.loc_uncrtnty_v];
    z_breach_rsp.pos_fix.height_uncrtnty_incl = TRUE;
  }
  else
  {
    z_breach_rsp.pos_fix.height_uncrtnty_incl = FALSE;
  }
  z_breach_rsp.pos_fix.loc_uncrtnty_ang = breach_pos_fix.loc_uncrtnty_ang*5.625F;

  // Convert the latitude and longitude values from the GM engine in radians to degrees before sending it to QMI_LOC
  z_breach_rsp.pos_fix.longitude = ((double)breach_pos_fix.longitude*(double)180)/(double)(1<<25);
  z_breach_rsp.pos_fix.latitude = ((double)breach_pos_fix.latitude*(double)180)/(double)(1<<25);

  /* Compute the velocity component of the fix */
  if (p_gm_notify_data->pos_fix_rpt.z_NavPos.z_PosFlags.b_IsVelValid)
  {
    z_breach_rsp.pos_fix.velocity_incl = 1;
    /* Compute the horizontal velocity component */
    /* Compute horizontal velocity from components and scale
     * to units of m/s.
    */
    f_vel_east = p_gm_notify_data->pos_fix_rpt.z_NavPos.f_VelEnu[0];
    f_vel_north = p_gm_notify_data->pos_fix_rpt.z_NavPos.f_VelEnu[1];
    f_tmp = sqrt( f_vel_east * f_vel_east + f_vel_north * f_vel_north );
    if (f_tmp < TM_CORE_MIN_VELOCTY)
    {
      z_breach_rsp.pos_fix.velocity_hor = 0;
    }
    else
    {
      z_breach_rsp.pos_fix.velocity_hor = f_tmp;
    }
    /* Compute the vertical velocity component */
    /* Compute vertical velocity from components and scale
     * to units of m/s.
    */
    /* If fix_type = 1, set the vertical velocity in units of m/s. */
    if ( breach_pos_fix.fix_type )
    {
      z_breach_rsp.pos_fix.velocity_ver = p_gm_notify_data->pos_fix_rpt.z_NavPos.f_VelEnu[2];
    }
  }// end of if (p_gm_notify_data->pos_fix_rpt.z_NavPos.z_PosFlags.b_IsVelValid)
  else
  {
    z_breach_rsp.pos_fix.velocity_incl = 0;
  }

  if(gfc_qmi_construct_utc_time_from_lr(&(breach_pos_fix), &(z_breach_rsp.pos_fix.utc_time)) == FALSE)
  {
    MSG_ERROR("Error in constructing UTC time in breach report",0,0,0);
  }

  MSG_HIGH("UTC Date: %d",z_breach_rsp.pos_fix.utc_time.date,0,0 );
  MSG_HIGH("UTC Hour: %d Min: %d Sec: %d",
            z_breach_rsp.pos_fix.utc_time.hour,
            z_breach_rsp.pos_fix.utc_time.minute,
            z_breach_rsp.pos_fix.utc_time.sec);


  if(z_gfc_qmi_info.z_gfc_qmi_cb_table.p_breach_rsp_func != NULL)
  {
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_breach_rsp_func(&z_breach_rsp);
  }
  // Since this is a QMI based geofence, the breach receipt is self acked by GM .
  // So no need to send a breach ack separately .

}

/*===========================================================================

FUNCTION gfc_qmi_internal_proximity_gf_rsp_handler

DESCRIPTION
  This function notifies the QMI when the geofence proximity occurs.

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_proximity_gf_rsp_handler(sm_gm_client_proximity_notify_type *p_gm_notify_data)
{
  gfc_qmi_proximity_geofence_rsp_s_type z_prox_rsp = {0};

  if(p_gm_notify_data == NULL)
  {
    MSG_ERROR("Null pointer in proximity indication",0,0,0);
    return;
  }

  /* Fill in the proximity type. */
  switch(p_gm_notify_data->proximity_type)
  {
    case GM_PROXIMITY_IND_IN_PROXIMITY:
      z_prox_rsp.proximity_type= GFC_QMI_PROXIMITY_IND_TYPE_IN;
      break;
    case GM_PROXIMITY_IND_OUT_PROXIMITY:
      z_prox_rsp.proximity_type= GFC_QMI_PROXIMITY_IND_TYPE_OUT;
      break;
    default:
      z_prox_rsp.proximity_type= GFC_QMI_PROXIMITY_IND_TYPE_MAX;
      break;
  }


  z_prox_rsp.geofence_id = p_gm_notify_data->geofence_id;
  z_prox_rsp.context_id_valid = p_gm_notify_data->context_id_valid;
  z_prox_rsp.context_id = p_gm_notify_data->context_id;

  if(z_gfc_qmi_info.z_gfc_qmi_cb_table.p_proximity_rsp_func != NULL)
  {
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_proximity_rsp_func(&z_prox_rsp);
  }
}


/*===========================================================================

FUNCTION gfc_qmi_internal_start_cpi_fix

DESCRIPTION
  This function is used to perform a CPI fix

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_start_cpi_fix(void)
{
  tm_pdapi_pd_event_s_type pd_event_info;
  pdsm_ext_status_info_s_type   pd_ext_status_info_pos_inj;

  /*Request CPI fix only if TM_QWIP is not running Wifi fixes*/
  if(tm_core_is_wiper_running() == FALSE)
  {
    pd_event_info.e_pd_event = PDSM_PD_EVENT_WPS_NEEDED;
    pd_event_info.pd_event_data.pd_info_type.client_id = pdsm_ni_client_id_get();
    pd_event_info.pd_event_data.pd_info_type.pd_info.qwip_data.request_type = PDSM_PD_QWIP_STOP_PERIODIC_FIXES;
    pd_event_info.pd_event_data.pd_info_type.pd_info.qwip_data.tbf_ms = 0;

    tm_pdapi_pd_event_callback(&pd_event_info, TM_PDAPI_SESS_TYPE_NONE);

    pd_event_info.e_pd_event = PDSM_PD_EVENT_WPS_NEEDED;
    pd_event_info.pd_event_data.pd_info_type.client_id = pdsm_ni_client_id_get();
    pd_event_info.pd_event_data.pd_info_type.pd_info.qwip_data.request_type = PDSM_PD_QWIP_START_PERIODIC_HI_FREQ_FIXES;
    pd_event_info.pd_event_data.pd_info_type.pd_info.qwip_data.tbf_ms = 1000;

    MSG_HIGH("REQUEST POSITION INJECTION for GM",0,0,0);
    tm_pdapi_pd_event_callback(&pd_event_info, TM_PDAPI_SESS_TYPE_NONE);

    /*Request ODCPI V1 Single Shot fix also*/
    memset(&pd_ext_status_info_pos_inj, 0, sizeof(pd_ext_status_info_pos_inj));
    pd_ext_status_info_pos_inj.ext_status_type = PDSM_EXT_STATUS_POS_INJECT_REQUEST;
    pd_ext_status_info_pos_inj.client_id =  pdsm_ni_client_id_get();

    tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_POS_INJECT_REQUEST,
       &pd_ext_status_info_pos_inj);

    z_gfc_qmi_info.b_cpi_requested = TRUE;
  }
  else
  {
    MSG_HIGH("Wiper Running - GFC QMI FIX START",0,0,0);
  }
}

/*===========================================================================

FUNCTION gfc_qmi_internal_stop_cpi_fix

DESCRIPTION
  This function is used to abort a CPI fix

PARAMETERS
 void

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_stop_cpi_fix(void)
{
  /*Request CPI stop only if TM_QWIP is not running Wifi fixes*/
  if(tm_core_is_wiper_running() == FALSE)
  {
    if(z_gfc_qmi_info.b_cpi_requested)
    {

      tm_pdapi_pd_event_s_type pd_event_info;

      pd_event_info.e_pd_event = PDSM_PD_EVENT_WPS_NEEDED;
      pd_event_info.pd_event_data.pd_info_type.client_id = pdsm_ni_client_id_get();
      pd_event_info.pd_event_data.pd_info_type.pd_info.qwip_data.request_type = PDSM_PD_QWIP_STOP_PERIODIC_FIXES;
      pd_event_info.pd_event_data.pd_info_type.pd_info.qwip_data.tbf_ms = 0;

      MSG_HIGH("Stopping POSITION INJECTION for GM",0,0,0);
      tm_pdapi_pd_event_callback(&pd_event_info, TM_PDAPI_SESS_TYPE_NONE);

    }
  }
  else
  {
    MSG_HIGH("Wiper Running - GFC QMI FIX STOP",0,0,0);
  }

  z_gfc_qmi_info.b_cpi_requested = FALSE;
}



/*===========================================================================

FUNCTION gfc_qmi_internal_conv_gm_pos_fix_to_gfc

DESCRIPTION
  This function converts the position info from GM to GFC QMI format

PARAMETERS
  p_gm_pos_info : The GM pos info passed in.
  p_gfc_pos_info: The GFC QMI pos info which is passed to HLOS.
 
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_conv_gm_pos_fix_to_gfc(sm_GnssFixRptStructType *p_gm_pos_info, qfc_qmi_pos_data_rsp_s_type *p_gfc_pos_info)
{
  tm_lr_resp_type z_gm_conv_pos_fix = {0};
  uint32 i = 0, q_SvListIdx = 0;
  float f_vel_east = 0, f_vel_north = 0, f_tmp = 0;
  if((NULL == p_gm_pos_info) ||
     (NULL == p_gfc_pos_info))
  {
    MSG_ERROR("NULL pointer passed in. ", 0, 0, 0);
    return ;
  }


  tm_util_convert_lr(p_gm_pos_info, &z_gm_conv_pos_fix);


  /* populate the position fix information for the dwell */
  // Convert the heading angle
  p_gfc_pos_info->heading = (uint16) tm_util_nearest_long((double)z_gm_conv_pos_fix.heading/(double)HEADING_SCALE );
  p_gfc_pos_info->heading_valid = z_gm_conv_pos_fix.heading_valid;
  // Convert to altitude With Respect to Ellipsoid
  p_gfc_pos_info->height = (float)(z_gm_conv_pos_fix.height - GFC_QMI_ALTITUDE_WRT_WGS84_ELLIPSOID_OFFSET);
  p_gfc_pos_info->height_incl = z_gm_conv_pos_fix.height_incl;

  p_gfc_pos_info->loc_uncrtnty_a = p_gfc_pos_info->loc_uncrtnty_p = p_gfc_pos_info->loc_uncrtnty_v = 0;

  if(z_gm_conv_pos_fix.loc_uncrtnty_a < MAX_POS_UNC_IND)
  {
    p_gfc_pos_info->loc_uncrtnty_a = f_PosUncArr[z_gm_conv_pos_fix.loc_uncrtnty_a];
  }
  if(z_gm_conv_pos_fix.loc_uncrtnty_p < MAX_POS_UNC_IND)
  {
    p_gfc_pos_info->loc_uncrtnty_p = f_PosUncArr[z_gm_conv_pos_fix.loc_uncrtnty_p];
  }
  if(z_gm_conv_pos_fix.loc_uncrtnty_v < MAX_POS_UNC_IND)
  {
    p_gfc_pos_info->loc_uncrtnty_v = f_PosUncArr[z_gm_conv_pos_fix.loc_uncrtnty_v];
    p_gfc_pos_info->height_uncrtnty_incl = TRUE;
  }
  else
  {
    p_gfc_pos_info->height_uncrtnty_incl = FALSE;
  }

  p_gfc_pos_info->loc_uncrtnty_ang = z_gm_conv_pos_fix.loc_uncrtnty_ang*5.625F;

  // Convert the latitude and longitude values from the GM engine in radians to degrees before sending it to QMI_LOC
  p_gfc_pos_info->longitude = ((double)z_gm_conv_pos_fix.longitude*(double)180)/(double)(1<<25);
  p_gfc_pos_info->latitude = ((double)z_gm_conv_pos_fix.latitude*(double)180)/(double)(1<<25);


  /* Compute the velocity component of the fix */
  if (p_gm_pos_info->z_NavPos.z_PosFlags.b_IsVelValid)
  {
    p_gfc_pos_info->velocity_incl = 1;
    /* Compute the horizontal velocity component */
    /* Compute horizontal velocity from components and scale
     * to units of m/s.
    */
    f_vel_east = p_gm_pos_info->z_NavPos.f_VelEnu[0];
    f_vel_north = p_gm_pos_info->z_NavPos.f_VelEnu[1];

    f_tmp = sqrt( f_vel_east * f_vel_east + f_vel_north * f_vel_north );

    if (f_tmp < TM_CORE_MIN_VELOCTY)
    {
      p_gfc_pos_info->velocity_hor = 0;
    }
    else
    {
      p_gfc_pos_info->velocity_hor = f_tmp;
    }

    /* Compute the vertical velocity component */
    /* Compute vertical velocity from components and scale
     * to units of m/s.
    */
    /* If fix_type = 1, set the vertical velocity in units of m/s. */
    if ( z_gm_conv_pos_fix.fix_type )
    {
      p_gfc_pos_info->velocity_ver = p_gm_pos_info->z_NavPos.f_VelEnu[2];
    }
  }// end of if (p_gm_pos_info->z_NavPos.z_PosFlags.b_IsVelValid)
  else
  {
    p_gfc_pos_info->velocity_incl = 0;
  }

  if(gfc_qmi_construct_utc_time_from_lr(&(z_gm_conv_pos_fix), &(p_gfc_pos_info->utc_time)) == FALSE)
  {
    MSG_ERROR("Error in constructing UTC time in batch dwell report",0,0,0);
  }

  /* Convert the incoming Heading uncertainty (rad) to degrees . The incoming Heading unc is not in IS801 format .
     So converting to radians using the formula Pi rad = 180 degrees. */
  p_gfc_pos_info->headingUnc = ((float)p_gm_pos_info->z_NavPos.f_GnssHeadingUncRad*(float)180)/(float)(3.14159265359);

  /* Vertical uncertainty in meters */
  p_gfc_pos_info->loc_uncrtnty_v = p_gm_pos_info->z_NavPos.f_PuncVertMeters;

  /* 3-D Speed uncertainty in m/s */
  p_gfc_pos_info->speedUnc = p_gm_pos_info->z_NavPos.f_Vunc3dMps;

  /* Horizontal uncertainty confidence in percentage */
  p_gfc_pos_info->horConfidence = GFC_QMI_HOR_UNC_CONF_PCT;

  /* Horizontal uncertainty confidence in percentage */
  p_gfc_pos_info->vertConfidence = GFC_QMI_HOR_UNC_CONF_PCT;

  /* Position dilution of precision. */
  p_gfc_pos_info->DOP.PDOP = p_gm_pos_info->z_NavPos.f_PDOP;

  /* Horizontal dilution of precision. */
  p_gfc_pos_info->DOP.HDOP = p_gm_pos_info->z_NavPos.f_HDOP;

  /* Vertical dilution of precision. */
  p_gfc_pos_info->DOP.VDOP = p_gm_pos_info->z_NavPos.f_VDOP;


  /** convert the sv used info to gnssSvUsedList */
  /* Populate the GPS satellite info */
  for (i = 0; (i < PDSM_N_ACTIVE_GPS_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++)
  {
    /* Check whether GPS SVs were used in calculating position */
    if(p_gm_pos_info->z_NavSvInfo.z_GpsSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
    {
      p_gfc_pos_info->gnssSvUsedList[q_SvListIdx++] =
        p_gm_pos_info->z_NavSvInfo.z_GpsSvInfo[i].w_SvId;
    }
  }

  /* Populate the GLO satellite info */
  for (i = 0; (i < PDSM_N_ACTIVE_GLO_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++)
  {
    /* Check whether GLO SVs were used in calculating position */
    if(p_gm_pos_info->z_NavSvInfo.z_GloSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
    {
      p_gfc_pos_info->gnssSvUsedList[q_SvListIdx++] =
        p_gm_pos_info->z_NavSvInfo.z_GloSvInfo[i].w_SvId;
    }
  }

  /* Populate the BDS satellite info */
  for (i = 0; (i < PDSM_N_ACTIVE_BDS_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++)
  {
    /* Check whether GLO SVs were used in calculating position */
    if(p_gm_pos_info->z_NavSvInfo.z_BdsSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
    {
      p_gfc_pos_info->gnssSvUsedList[q_SvListIdx++] =
        p_gm_pos_info->z_NavSvInfo.z_BdsSvInfo[i].w_SvId;
    }
  }

  /** set the valid flag only if the list is non-empty */
  if(q_SvListIdx > 0 )
  {
    p_gfc_pos_info->gnssSvUsedList_len = q_SvListIdx ;
  }

}
/*===========================================================================

FUNCTION gfc_qmi_internal_batch_breach_gf_rsp_handler

DESCRIPTION
  This function notifies the QMI when the geofence batch breach breach occurs.

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_batch_breach_gf_rsp_handler(sm_gm_client_batch_breach_notify_type *p_gm_notify_data)
{
  gfc_qmi_batch_breach_geofence_rsp_s_type *pz_breach_rsp;
  tm_lr_resp_type breach_pos_fix;
  uint32 q_ipc_num_disc_gid, q_ipc_num_cont_gid;
  uint32 i = 0, q_SvListIdx = 0;
  float f_vel_east, f_vel_north, f_tmp;
  
  q_ipc_num_disc_gid = p_gm_notify_data->q_discrete_list_len;
  q_ipc_num_cont_gid = p_gm_notify_data->q_cont_list_len;
  pz_breach_rsp = (gfc_qmi_batch_breach_geofence_rsp_s_type*)os_MemAlloc(sizeof(gfc_qmi_batch_breach_geofence_rsp_s_type)+
	                                                                     q_ipc_num_disc_gid*sizeof(uint32)+
                                                                         q_ipc_num_cont_gid*sizeof(sm_gm_cont_gid_type),
                                                                         OS_MEM_SCOPE_TASK);

  if(pz_breach_rsp == NULL)
  {
    /* Could not allocate a new node for the split return */
    MSG_ERROR("Mem alloc failure in gfc_qmi_internal_batch_breach_gf_rsp_handler ", 0, 0, 0);
    return ;
  }

  memset(pz_breach_rsp, 0, sizeof(gfc_qmi_batch_breach_geofence_rsp_s_type));

  if(p_gm_notify_data->b_fix_included == TRUE)
  {
    pz_breach_rsp->pos_rep_mask = GFC_QMI_POS_REPORT_ON_BREACH_TRUE;
  }
  else
  {
    pz_breach_rsp->pos_rep_mask = GFC_QMI_POS_REPORT_ON_BREACH_FALSE;
  }
  /* Fill in the breach type. */
  switch(p_gm_notify_data->q_breach_type)
  {
    case GEOFENCE_EVENT_ENTERING_AREA:
      pz_breach_rsp->breach_type = GFC_QMI_BREACH_GEOFENCE_TYPE_ENTERING;
      break;
    case GEOFENCE_EVENT_LEAVING_AREA:
      pz_breach_rsp->breach_type = GFC_QMI_BREACH_GEOFENCE_TYPE_LEAVING;
      break;
    default:
      pz_breach_rsp->breach_type = GFC_QMI_BREACH_GEOFENCE_TYPE_UNKNOWN;
      break;
  }
  /* Fill in the geofence origin type. */
  switch(p_gm_notify_data->geofence_type)
  {
    case GEOFENCE_TYPE_MO:
      pz_breach_rsp->origin = GFC_QMI_GEOFENCE_ORIGIN_MO;
      break;
    case GEOFENCE_TYPE_NI:
      pz_breach_rsp->origin = GFC_QMI_GEOFENCE_ORIGIN_NI;
      break;
    default:
      break;
  }

  pz_breach_rsp->confidence_rep_mask = GFC_QMI_CONFIDENCE_REPORT_ON_BREACH_TRUE;
  switch(p_gm_notify_data->u_confidence_type)
  {
    case GEOFENCE_CONFIDENCE_HIGH:
      pz_breach_rsp->confidence = GFC_QMI_CONFIDENCE_HIGH;
      break;
    case GEOFENCE_CONFIDENCE_MEDIUM:
      pz_breach_rsp->confidence = GFC_QMI_CONFIDENCE_MEDIUM;
      break;
    case GEOFENCE_CONFIDENCE_LOW:
      pz_breach_rsp->confidence = GFC_QMI_CONFIDENCE_LOW;
      break;
    default:
      break;
  }


  tm_util_convert_lr(&p_gm_notify_data->z_pos_fix_rpt, &breach_pos_fix);
  
  // Convert the heading angle
  pz_breach_rsp->pos_fix.heading = (uint16) tm_util_nearest_long((double)breach_pos_fix.heading/(double)HEADING_SCALE );
  pz_breach_rsp->pos_fix.heading_valid = breach_pos_fix.heading_valid;
  // Convert to altitude With Respect to Ellipsoid
  pz_breach_rsp->pos_fix.height = (float)(breach_pos_fix.height - GFC_QMI_ALTITUDE_WRT_WGS84_ELLIPSOID_OFFSET);
  pz_breach_rsp->pos_fix.height_incl = breach_pos_fix.height_incl;

  pz_breach_rsp->pos_fix.loc_uncrtnty_a = pz_breach_rsp->pos_fix.loc_uncrtnty_p = pz_breach_rsp->pos_fix.loc_uncrtnty_v = 0;

  if(breach_pos_fix.loc_uncrtnty_a < MAX_POS_UNC_IND)
  {
    pz_breach_rsp->pos_fix.loc_uncrtnty_a = f_PosUncArr[breach_pos_fix.loc_uncrtnty_a];
  }
  if(breach_pos_fix.loc_uncrtnty_p < MAX_POS_UNC_IND)
  {
    pz_breach_rsp->pos_fix.loc_uncrtnty_p = f_PosUncArr[breach_pos_fix.loc_uncrtnty_p];
  }
  if(breach_pos_fix.loc_uncrtnty_v < MAX_POS_UNC_IND)
  {
    pz_breach_rsp->pos_fix.loc_uncrtnty_v = f_PosUncArr[breach_pos_fix.loc_uncrtnty_v];
    pz_breach_rsp->pos_fix.height_uncrtnty_incl = TRUE;
  }
  else
  {
    pz_breach_rsp->pos_fix.height_uncrtnty_incl = FALSE;
  }

  pz_breach_rsp->pos_fix.loc_uncrtnty_ang = breach_pos_fix.loc_uncrtnty_ang*5.625F;

  // Convert the latitude and longitude values from the GM engine in radians to degrees before sending it to QMI_LOC
  pz_breach_rsp->pos_fix.longitude = ((double)breach_pos_fix.longitude*(double)180)/(double)(1<<25);
  pz_breach_rsp->pos_fix.latitude = ((double)breach_pos_fix.latitude*(double)180)/(double)(1<<25);


  /* Compute the velocity component of the fix */
  if (p_gm_notify_data->z_pos_fix_rpt.z_NavPos.z_PosFlags.b_IsVelValid)
  {
    pz_breach_rsp->pos_fix.velocity_incl = 1;
    /* Compute the horizontal velocity component */
    /* Compute horizontal velocity from components and scale
     * to units of m/s.
    */
    f_vel_east = p_gm_notify_data->z_pos_fix_rpt.z_NavPos.f_VelEnu[0];
    f_vel_north = p_gm_notify_data->z_pos_fix_rpt.z_NavPos.f_VelEnu[1];

    f_tmp = sqrt( f_vel_east * f_vel_east + f_vel_north * f_vel_north );

    if (f_tmp < TM_CORE_MIN_VELOCTY)
    {
      pz_breach_rsp->pos_fix.velocity_hor = 0;
    }
    else
    {
      pz_breach_rsp->pos_fix.velocity_hor = f_tmp;
    }

    /* Compute the vertical velocity component */
    /* Compute vertical velocity from components and scale
     * to units of m/s.
    */
    /* If fix_type = 1, set the vertical velocity in units of m/s. */
    if ( breach_pos_fix.fix_type )
    {
      pz_breach_rsp->pos_fix.velocity_ver = p_gm_notify_data->z_pos_fix_rpt.z_NavPos.f_VelEnu[2];
    }
  }// end of if (p_gm_notify_data->z_pos_fix_rpt.z_NavPos.z_PosFlags.b_IsVelValid)
  else
  {
    pz_breach_rsp->pos_fix.velocity_incl = 0;
  }

  if(gfc_qmi_construct_utc_time_from_lr(&(breach_pos_fix), &(pz_breach_rsp->pos_fix.utc_time)) == FALSE)
  {
    MSG_ERROR("Error in constructing UTC time in batch breach report",0,0,0);
  }

  /* Convert the incoming Heading uncertainty (rad) to degrees . The incoming Heading unc is not in IS801 format .
     So converting to radians using the formula Pi rad = 180 degrees. */
  pz_breach_rsp->pos_fix.headingUnc = ((float)p_gm_notify_data->z_pos_fix_rpt.z_NavPos.f_GnssHeadingUncRad*(float)180)/(float)(3.14159265359);

  /* Vertical uncertainty in meters */
  pz_breach_rsp->pos_fix.loc_uncrtnty_v = p_gm_notify_data->z_pos_fix_rpt.z_NavPos.f_PuncVertMeters;

  /* 3-D Speed uncertainty in m/s */
  pz_breach_rsp->pos_fix.speedUnc = p_gm_notify_data->z_pos_fix_rpt.z_NavPos.f_Vunc3dMps;

  /* Horizontal uncertainty confidence in percentage */
  pz_breach_rsp->pos_fix.horConfidence = GFC_QMI_HOR_UNC_CONF_PCT;

  /* Horizontal uncertainty confidence in percentage */
  pz_breach_rsp->pos_fix.vertConfidence = GFC_QMI_HOR_UNC_CONF_PCT;

  /* Position dilution of precision. */
  pz_breach_rsp->pos_fix.DOP.PDOP = p_gm_notify_data->z_pos_fix_rpt.z_NavPos.f_PDOP;

  /* Horizontal dilution of precision. */
  pz_breach_rsp->pos_fix.DOP.HDOP = p_gm_notify_data->z_pos_fix_rpt.z_NavPos.f_HDOP;

  /* Vertical dilution of precision. */
  pz_breach_rsp->pos_fix.DOP.VDOP = p_gm_notify_data->z_pos_fix_rpt.z_NavPos.f_VDOP;


  /** convert the sv used info to gnssSvUsedList */
  /* Populate the GPS satellite info */
  for (i = 0; (i < PDSM_N_ACTIVE_GPS_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++)
  {
    /* Check whether GPS SVs were used in calculating position */
    if(p_gm_notify_data->z_pos_fix_rpt.z_NavSvInfo.z_GpsSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
    {
      pz_breach_rsp->pos_fix.gnssSvUsedList[q_SvListIdx++] =
        p_gm_notify_data->z_pos_fix_rpt.z_NavSvInfo.z_GpsSvInfo[i].w_SvId;
    }
  }

  /* Populate the GLO satellite info */
  for (i = 0; (i < PDSM_N_ACTIVE_GLO_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++)
  {
    /* Check whether GLO SVs were used in calculating position */
    if(p_gm_notify_data->z_pos_fix_rpt.z_NavSvInfo.z_GloSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
    {
      pz_breach_rsp->pos_fix.gnssSvUsedList[q_SvListIdx++] =
        p_gm_notify_data->z_pos_fix_rpt.z_NavSvInfo.z_GloSvInfo[i].w_SvId;
    }
  }

  /* Populate the BDS satellite info */
  for (i = 0; (i < PDSM_N_ACTIVE_BDS_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++)
  {
    /* Check whether GLO SVs were used in calculating position */
    if(p_gm_notify_data->z_pos_fix_rpt.z_NavSvInfo.z_BdsSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
    {
      pz_breach_rsp->pos_fix.gnssSvUsedList[q_SvListIdx++] =
        p_gm_notify_data->z_pos_fix_rpt.z_NavSvInfo.z_BdsSvInfo[i].w_SvId;
    }
  }

 /*TBD: GAL */

  /** set the valid flag only if the list is non-empty */
  if(q_SvListIdx > 0 )
  {
    pz_breach_rsp->pos_fix.gnssSvUsedList_len = q_SvListIdx ;
  }

  if( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
  {
    static char u_string[100];
    (void) snprintf(u_string, sizeof(u_string), "HeadingUnc %f ,PUNC_V %f ,speed_unc %f, PDOP %f, VDOP %f, Num_Sv %d",
                                          pz_breach_rsp->pos_fix.headingUnc,
                                          pz_breach_rsp->pos_fix.loc_uncrtnty_v,
                                          pz_breach_rsp->pos_fix.speedUnc,
                                          pz_breach_rsp->pos_fix.DOP.PDOP,
                                          pz_breach_rsp->pos_fix.DOP.VDOP,
                                          (int)pz_breach_rsp->pos_fix.gnssSvUsedList_len);
    MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", u_string);
  }

  MSG_MED("Batch breach report . Disc list num %d Cont list num %d, ",
          p_gm_notify_data->q_discrete_list_len,
          p_gm_notify_data->q_cont_list_len, 0);

  /* Copy the discrete GID elements from the batch breach report */
  memscpy(&pz_breach_rsp->data[0],
          sizeof(uint32) * p_gm_notify_data->q_discrete_list_len,
          &p_gm_notify_data->data[0],
          sizeof(uint32) * p_gm_notify_data->q_discrete_list_len);

  /* Copy the continuous GID elements from the batch breach report */
  memscpy(&pz_breach_rsp->data[p_gm_notify_data->q_discrete_list_len],
          sizeof(sm_gm_cont_gid_type) * p_gm_notify_data->q_cont_list_len,
          &p_gm_notify_data->data[p_gm_notify_data->q_discrete_list_len],
          sizeof(sm_gm_cont_gid_type) * p_gm_notify_data->q_cont_list_len);

  pz_breach_rsp->q_discrete_list_len = p_gm_notify_data->q_discrete_list_len;
  pz_breach_rsp->q_cont_list_len = p_gm_notify_data->q_cont_list_len;

  if(z_gfc_qmi_info.z_gfc_qmi_cb_table.p_batch_breach_rsp_func != NULL)
  {
    // Note that we could send the breach to the QMI.
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_batch_breach_rsp_func(pz_breach_rsp);
  }
  // Since this is a QMI based geofence, the breach receipt is self acked by GM .
  // So no need to send a breach ack separately for all the geofences received by GFC_QMI.

  /* Deallocate the memory used for the operation */
  os_MemFree((void**)&pz_breach_rsp);
}


/*===========================================================================

FUNCTION gfc_qmi_internal_tz_req_handler

DESCRIPTION
  This function is used to handle TimeZone request from GeoFence engine.

PARAMETERS

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_tz_req_handler(void)
{
  if(NULL != z_gfc_qmi_info.z_gfc_qmi_cb_table.tz_inject_func)
  {
    z_gfc_qmi_info.z_gfc_qmi_cb_table.tz_inject_func();
  }
  else
  {
    MSG_ERROR("NULL TZ Inject Req Handler!!", 0, 0, 0);
  }
}

/*===========================================================================

FUNCTION gfc_qmi_internal_batch_dwell_gf_rsp_handler

DESCRIPTION
  This function notifies the QMI when the geofence batch breach breach occurs.

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_batch_dwell_gf_rsp_handler(sm_gm_client_batch_dwell_notify_type *p_gm_notify_data)
{
  gfc_qmi_batch_dwell_geofence_rsp_s_type *pz_dwell_rsp;
  uint32 q_ipc_num_disc_gid, q_ipc_num_cont_gid;
  uint32 i = 0, q_SvListIdx = 0;
  float f_vel_east, f_vel_north, f_tmp;
  static char u_string[100];

  q_ipc_num_disc_gid = p_gm_notify_data->q_discrete_list_len;
  q_ipc_num_cont_gid = p_gm_notify_data->q_cont_list_len;
  pz_dwell_rsp = (gfc_qmi_batch_dwell_geofence_rsp_s_type*)os_MemAlloc(sizeof(gfc_qmi_batch_dwell_geofence_rsp_s_type)+
	                                                                     q_ipc_num_disc_gid*sizeof(uint32)+
                                                                         q_ipc_num_cont_gid*sizeof(sm_gm_cont_gid_type),
                                                                         OS_MEM_SCOPE_TASK);

  if(pz_dwell_rsp == NULL)
  {
    /* Could not allocate a new node for the split return */
    MSG_ERROR("Mem alloc failure in gfc_qmi_internal_batch_dwell_gf_rsp_handler ", 0, 0, 0);
    return ;
  }

  memset(pz_dwell_rsp, 0, sizeof(gfc_qmi_batch_dwell_geofence_rsp_s_type));

  if(p_gm_notify_data->b_fix_included == TRUE)
  {
    pz_dwell_rsp->pos_rep_mask = GFC_QMI_POS_REPORT_ON_BREACH_TRUE;
  }
  else
  {
    pz_dwell_rsp->pos_rep_mask = GFC_QMI_POS_REPORT_ON_BREACH_FALSE;
  }
  /* Fill in the breach type. */
  switch(p_gm_notify_data->e_dwell_event_type)
  {
    case GEOFENCE_DWELL_TYPE_IN:
	  pz_dwell_rsp->dwell_type = GFC_QMI_GEOFENCE_DWELL_TYPE_INSIDE_MASK;
      break;
    case GEOFENCE_DWELL_TYPE_OUT:
      pz_dwell_rsp->dwell_type = GFC_QMI_GEOFENCE_DWELL_TYPE_OUTSIDE_MASK;
      break;
    default:
      pz_dwell_rsp->dwell_type = GFC_QMI_GEOFENCE_DWELL_TYPE_NONE_MASK;
      break;
  }
  /* Fill in the geofence origin type. */
  switch(p_gm_notify_data->geofence_type)
  {
    case GEOFENCE_TYPE_MO:
      pz_dwell_rsp->origin = GFC_QMI_GEOFENCE_ORIGIN_MO;
      break;
    case GEOFENCE_TYPE_NI:
      pz_dwell_rsp->origin = GFC_QMI_GEOFENCE_ORIGIN_NI;
      break;
    default:
      break;
  }
  
  /* Convert the pos info from GM type to GFC QMI type */
  gfc_qmi_internal_conv_gm_pos_fix_to_gfc(&p_gm_notify_data->z_pos_fix_rpt, &pz_dwell_rsp->pos_fix);

  MSG_MED("Batch dwell report . Disc list num %d Cont list num %d, ",
          p_gm_notify_data->q_discrete_list_len,
          p_gm_notify_data->q_cont_list_len, 0);

  /* Copy the discrete GID elements from the batch dwell report */
  memscpy(&pz_dwell_rsp->data[0],
          sizeof(uint32) * p_gm_notify_data->q_discrete_list_len,
          &p_gm_notify_data->data[0],
          sizeof(uint32) * p_gm_notify_data->q_discrete_list_len);

  /* Copy the continuous GID elements from the batch dwell report */
  memscpy(&pz_dwell_rsp->data[p_gm_notify_data->q_discrete_list_len],
          sizeof(sm_gm_cont_gid_type) * p_gm_notify_data->q_cont_list_len,
          &p_gm_notify_data->data[p_gm_notify_data->q_discrete_list_len],
          sizeof(sm_gm_cont_gid_type) * p_gm_notify_data->q_cont_list_len);

  pz_dwell_rsp->q_discrete_list_len = p_gm_notify_data->q_discrete_list_len;
  pz_dwell_rsp->q_cont_list_len = p_gm_notify_data->q_cont_list_len;

  if(z_gfc_qmi_info.z_gfc_qmi_cb_table.p_batch_dwell_rsp_func != NULL)
  {
    // Note that we could send the dwell to the QMI.
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_batch_dwell_rsp_func(pz_dwell_rsp);
  }

  /* Deallocate the memory used for the operation */
  os_MemFree((void**)&pz_dwell_rsp);
}


/*===========================================================================

FUNCTION gfc_qmi_internal_pos_request_handler

DESCRIPTION
  This function is used to perform the standalone positon request of the GFC_QMI

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_pos_request_handler(void *p_gm_notify_data)
{
  sm_gm_client_pos_req_type* pos_request = (sm_gm_client_pos_req_type*)p_gm_notify_data;

  if(pos_request == NULL)
  {
    MSG_ERROR("Null Pos Request pointer",0,0,0);
    return;
  }

  if(pos_request->pos_fix_src == GEOFENCE_POS_FIX_SRC_GNSS)
  {
    pdsm_pd_session_operation_e_type pd_operation_mode = ((sm_gm_client_pos_req_type*)p_gm_notify_data)->pos_operation_mode;

    /* Start a position fix request from the TM core. */
    z_gfc_qmi_info.z_get_pos_param.client_type = PDSM_CLIENT_TYPE_NI;
    z_gfc_qmi_info.z_get_pos_param.client_id = pdsmclient_get_client_id_map(PDSM_CLIENT_TYPE_NI);
    z_gfc_qmi_info.z_get_pos_param.mt_initiator = MT_UP;
    z_gfc_qmi_info.z_get_pos_param.internal_session = TRUE;

    /* The QOS needed for this position fix. */
    /* suds84. fill this in from the GM core info */
    z_gfc_qmi_info.z_get_pos_options.fix_rate.num_fixes = ((sm_gm_client_pos_req_type*)p_gm_notify_data)->num_fixes;
    z_gfc_qmi_info.z_get_pos_options.fix_rate.time_between_fixes_ms = ((sm_gm_client_pos_req_type*)p_gm_notify_data)->time_between_fixes * 1000;
    z_gfc_qmi_info.z_get_pos_qos.gps_session_timeout = ((sm_gm_client_pos_req_type*)p_gm_notify_data)->session_timeout;
    z_gfc_qmi_info.z_get_pos_qos.accuracy_threshold = ((sm_gm_client_pos_req_type*)p_gm_notify_data)->horizontal_accuracy;
    z_gfc_qmi_info.z_get_pos_options.session = PDSM_PD_SESS_TYPE_NEW;
    z_gfc_qmi_info.z_get_pos_options.operation_mode = pd_operation_mode;
    z_gfc_qmi_info.z_get_pos_param.qos_ptr = &z_gfc_qmi_info.z_get_pos_qos;
    z_gfc_qmi_info.z_get_pos_param.option_ptr = &z_gfc_qmi_info.z_get_pos_options;

    /* Start the position fix with the TM Core. Note that the GM would be notified when the fix becomes available . */
    if(tm_core_handle_get_pos(&z_gfc_qmi_info.z_get_pos_param) == FALSE)
    {
      /*Send notification to GM that pos request failed*/
      gm_position_fix_request_fail(pos_request->pos_fix_src);
    }
  }
  else if(pos_request->pos_fix_src == GEOFENCE_POS_FIX_SRC_CPI)
  {
    gfc_qmi_internal_start_cpi_fix();
  }
  else
  {
    MSG_ERROR("GFC_QMI can not handle fix request for src = %d",pos_request->pos_fix_src,0,0);
  }
}


/*===========================================================================

FUNCTION gfc_qmi_internal_pos_abort_handler

DESCRIPTION
  This function is used to perform position abort

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_pos_abort_handler(void *p_gm_notify_data)
{
  sm_gm_client_abort_pos_req_type* pos_abort_request = (sm_gm_client_abort_pos_req_type*)p_gm_notify_data;


  if(pos_abort_request == NULL)
  {
    MSG_ERROR("Null Pos Abort pointer",0,0,0);
    return;
  }

  if(pos_abort_request->pos_fix_src == GEOFENCE_POS_FIX_SRC_CPI)
  {
    gfc_qmi_internal_stop_cpi_fix();
  }
  else if(pos_abort_request->pos_fix_src == GEOFENCE_POS_FIX_SRC_GNSS)
  {
    if(tm_core_is_internal_session_on())
    {
      tm_core_abort_recover(TRUE, TM_STOP_REASON_COMPLETED);
    }
  }

}
/*===========================================================================

FUNCTION gfc_qmi_internal_unsol_event_handler

DESCRIPTION
  This function handles the unsolicited events from GM .

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_unsol_event_handler(sm_gm_client_unsol_event_notify_type* p_gm_notify_data)
{
  gfc_qmi_general_notify_rsp_e_type e_event_type = {0};

  switch(p_gm_notify_data->unsol_event)
  {
    case GEOFENCE_UNSOL_EVENT_GPS_LOST:
      e_event_type = GFC_QMI_GENERAL_NOTIFY_GPS_OFF;
      break;
    case GEOFENCE_UNSOL_EVENT_GPS_AVAIL:
      e_event_type = GFC_QMI_GENERAL_NOTIFY_GPS_ON;
      break;
    case GEOFENCE_UNSOL_EVENT_OUT_OF_SERVICE:
      e_event_type = GFC_QMI_GENERAL_NOTIFY_OOS;
      break;
    case GEOFENCE_UNSOL_EVENT_TIME_INVALID:
      e_event_type = GFC_QMI_GENERAL_NOTIFY_TIMESTAMP_INVALID;
      break;
  default:
      MSG_MED("GFC_QMI::Unhandled Unsolicited event %d received from GM \n",
             p_gm_notify_data->unsol_event,
             0,
             0);
  }
  MSG_HIGH("GFC_QMI::Unsolicited event %d received from GM \n",
             e_event_type,
             0,
             0);
  if(z_gfc_qmi_info.z_gfc_qmi_cb_table.p_general_notify_rsp_func != NULL)
  {
    z_gfc_qmi_info.z_gfc_qmi_cb_table.p_general_notify_rsp_func(e_event_type);
  }

}


/*===========================================================================

FUNCTION gfc_qmi_internal_set_gf_engine_config_rsp_handler

DESCRIPTION
  This function notifies the QMI of the result of set geofence engine config request

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_set_gf_engine_config_rsp_handler(sm_gm_client_notification_type* p_gm_notify_data)
{
  gfc_qmi_set_geofence_engine_config_rsp_s_type z_rsp;
  memset((void*)&z_rsp, 0, sizeof(z_rsp));
  // suds84. ToDo::Match the return code from GM.
  z_rsp.return_code = p_gm_notify_data->notification.return_code;
  /* Retrieve the QMI transaction ID */
  z_rsp.transaction_id = gfc_qmi_internal_get_qmi_trxn_id(p_gm_notify_data->q_transaction_id);

  if(z_rsp.return_code == GFC_QMI_ERROR_NONE)
  {
    MSG_HIGH("GFC_QMI::Geofence Set GF engine config SUCCESS :: TID=0x%x \n",
             z_rsp.transaction_id,0,0
             );
  }
  else
  {
    MSG_HIGH("GFC_QMI::Geofence Set GF engine config FAILURE :: TID=0x%x \n",
             z_rsp.transaction_id,
             0,
             0);
  }
  /* Check if the set engine parameters have failed */
  if(p_gm_notify_data->notification.return_code == GEOFENCE_ERROR_INCORRECT_PARAMS)
  {
    z_rsp.return_code = GFC_QMI_ERROR_INVALID_PARAMETER;
  }
  /* Check whether it is a non-qmi transaction */
  if(z_rsp.transaction_id == NON_QMI_TRXN_ID)
  {
    MSG_HIGH("GFC_QMI::Non-QMI txn 0x%x dropping the GM notification\n",
             z_rsp.transaction_id,0,0);
  }
  else
  {
    // Notify the QMI
    if (z_gfc_qmi_info.z_gfc_qmi_cb_table.p_set_gf_eng_conf_rsp_func != NULL)
    {
      z_gfc_qmi_info.z_gfc_qmi_cb_table.p_set_gf_eng_conf_rsp_func(&z_rsp);
    }
  }

}

/*===========================================================================

FUNCTION gfc_qmi_internal_gm_event_handler

DESCRIPTION
  This function handles the GM events passed from TM-Core.

PARAMETERS
  q_msg_type: The type of geofence message.
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
boolean gfc_qmi_internal_gm_event_handler
(
  uint32    q_msg_type,
  void      *p_msg_data
)
{
  boolean ret_val = TRUE;

  if(z_gfc_qmi_info.b_qmi_registered == FALSE)
  {
    /* The QMI is not registered with GFC_QMI. So drop the notifications which
       are sent to the QMI */
    return ret_val;

  }
  MSG_MED("GFC_QMI: gm_event_handler event type : %d", q_msg_type, 0, 0);
  switch(q_msg_type)
  {
    case GEOFENCE_ADD_NOTIFY:
      /*GM notification for Add request*/
      gfc_qmi_internal_add_gf_rsp_handler((sm_gm_client_notification_type*)p_msg_data);
      break;

    case GEOFENCE_EDIT_NOTIFY:
      /*GM notification for Edit request*/
      gfc_qmi_internal_edit_gf_rsp_handler((sm_gm_client_notification_type*)p_msg_data);
      break;

    case GEOFENCE_DELETE_NOTIFY:
      /*GM notification for Delete request*/
      gfc_qmi_internal_delete_gf_rsp_handler(p_msg_data);
      break;

    case GEOFENCE_PURGE_NOTIFY:
      /*GM notification for Delete request*/
      gfc_qmi_internal_purge_gf_rsp_handler(p_msg_data);
      break;

    case GEOFENCE_BREACH_NOTIFY:
      /*GM notification for Geofence Breach*/
      gfc_qmi_internal_breach_gf_rsp_handler((sm_gm_client_breach_notify_type*)p_msg_data);
      break;

    case GEOFENCE_PROXIMITY_NOTIFY:
      /*GM notification for Geofence Proximity*/
      gfc_qmi_internal_proximity_gf_rsp_handler((sm_gm_client_proximity_notify_type*)p_msg_data);
      break;

    case GEOFENCE_POS_REQUEST_NOTIFY:
      /*GM notification for starting pos fix*/
      gfc_qmi_internal_pos_request_handler(p_msg_data);
      break;

    case GEOFENCE_POS_ABORT_NOTIFY:
      /*GM notification for starting pos fix*/
      gfc_qmi_internal_pos_abort_handler(p_msg_data);
      break;

    case GEOFENCE_HANDELS_NOTIFY:
      /*GM notification for obtaining ALL geofence ids */
      gfc_qmi_internal_query_gf_ids_rsp_handler((sm_gm_geofence_handels_notification_type*)p_msg_data);
      break;

    case GEOFENCE_STATUS_NOTIFY:
      /*GM notification for querying a particular geofence. */
      gfc_qmi_internal_query_gf_rsp_handler((sm_gm_geofence_status_notification_type*)p_msg_data);
      break;

    case GEOFENCE_UNSOL_EVENT_NOTIFY:
      /* GM notification for unsolicited events */
      gfc_qmi_internal_unsol_event_handler((sm_gm_client_unsol_event_notify_type*)p_msg_data);
      break;

    case GEOFENCE_SET_ENGINE_CONFIG_NOTIFY:
    case GEOFENCE_SET_CLIENT_CONFIG_NOTIFY:
      /* GM notifications for set engine config request */
      gfc_qmi_internal_set_gf_engine_config_rsp_handler((sm_gm_client_notification_type*)p_msg_data);
      break;

    case GEOFENCE_BATCH_BREACH_NOTIFY:
      /* GM batch breach notifications */
      gfc_qmi_internal_batch_breach_gf_rsp_handler((sm_gm_client_batch_breach_notify_type*)p_msg_data);
      break;

    case GEOFENCE_TZ_REQ_NOTIFY:
      /* GM TimeZone request */
      gfc_qmi_internal_tz_req_handler();
      break;
      
	case GEOFENCE_BATCH_DWELL_NOTIFY:
      /* GM batch breach notifications */
      gfc_qmi_internal_batch_dwell_gf_rsp_handler((sm_gm_client_batch_dwell_notify_type*)p_msg_data);
      break;

    default:
      MSG_MED("GFC_QMI: gm_event_handler unknown event type : %d", q_msg_type, 0, 0);
      break;

  }//end switch
  return ret_val;
}


sm_gm_client_handle gfc_qmi_internal_get_client_handle(void)
{
  return(z_gfc_qmi_info.u_gm_handle);
}

/*
******************************************************************************
* gfc_qmi_internal_gm_init
*
* Function description:
*
*   This function initialises the GFC_QMI module . This sets up the internal
*   states of the GFC_QMI and registers with GM core.
*
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/

static void gfc_qmi_internal_gm_init(void)
{
  uint8 error_code;  
  sm_gm_set_client_config_type gfClientConfig = {0};
  
  // ToDo:: ID for GFC_QMI
  z_gfc_qmi_info.z_client.client_id = GEOFENCE_CLIENT_QMI;
  z_gfc_qmi_info.z_client.q_base_msg_id = TM_CORE_GM_MSG_ID_BASE;
  // ToDo:: Need to change this transaction ID since GFC_QMI is not a protocol.
  z_gfc_qmi_info.z_client.q_base_transaction_id = TM_GFC_QMI_GM_TRANSACTION_ID_BASE;
  z_gfc_qmi_info.z_client.q_client_thread_id = THREAD_ID_SM_TM;
  memset(&z_gfc_qmi_info.trxn_id_table, 0, sizeof(z_gfc_qmi_info.trxn_id_table));
  /* The currently used GM transaction ID */
  z_gfc_qmi_info.z_client.client_cap.client_capabilites = 
    GEOFENCE_CLIENT_QMI_NOTIFICATION_CAPABLE|GEOFENCE_CLIENT_POS_FIX_CAPABLE|GEOFENCE_CLIENT_GEOFENCE_SERVICE_REQ;
  z_gfc_qmi_info.z_client.client_cap.pos_fix_capability.pos_fix_method_cap = GEOFENCE_CLIENT_POS_FIX_STANDALONE_CAPABLE;
  z_gfc_qmi_info.z_client.client_cap.pos_fix_capability.pos_fix_accuracy_cap = GEOFENCE_CLIENT_POS_FIX_ACCURACY_HIGH;
  z_gfc_qmi_info.z_client.client_cap.pos_fix_capability.pos_fix_accuracy = 2; /*Minimum accuracy can be as low as 2 meters*/
  z_gfc_qmi_info.z_client.client_cap.pos_fix_capability.pos_fix_power_cap = GEOFENCE_CLIENT_POS_FIX_POWER_PROFILE_2;
  /*AP is not woken up during standalone fix*/
  z_gfc_qmi_info.z_client.client_cap.pos_fix_capability.high_power_proc_wakeup_cap = GEOFENCE_CLIENT_HIGH_POWER_PROC_WAKEUP_PROFILE_0;  
  /*Nw is not used during standalone fix*/  
  z_gfc_qmi_info.z_client.client_cap.pos_fix_capability.nw_usage_cap = GEOFENCE_CLIENT_NW_USAGE_PROFILE_0;
  
  z_gfc_qmi_info.z_client.client_cap.pos_fix_capability.is_imp_location_notification_required = FALSE;
  z_gfc_qmi_info.z_client.client_cap.pos_fix_capability.pos_fix_wait_secs = 60; /*Geofence core shall wait for 60 seconds for the fix*/
  z_gfc_qmi_info.z_client.client_cap.pos_fix_capability.is_prior_fix_request_indication_required = FALSE; /*Prior fix request indication required*/
  z_gfc_qmi_info.z_client.client_cap.pos_fix_capability.prior_fix_request_indication_secs = 0; /*Prior fix request indication required 2mins beforehand*/

  gfClientConfig.q_mask = (SM_GM_CLIENT_CONFIG_MAX_GEOFENCES_MASK|
                           SM_GM_CLIENT_CONFIG_GNSS_POS_QOS_SESS_TIMEOUT_MASK |
                           SM_GM_CLIENT_CONFIG_CPI_REQUEST_RATE_MASK |
                           SM_GM_CLIENT_CHAL_GPS_ENV_BO_MIN_MASK |
                           SM_GM_CLIENT_CHAL_GPS_ENV_BO_MAX_MASK |
                           SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_GPS_MASK |
                           SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_CPI_MASK |
                           SM_GM_CLIENT_CONFIG_GNSS_UNAVAIL_AGGR_EXIT_MASK);
  
  gfClientConfig.q_max_geofences = GFC_QMI_GM_MAX_GF_DEFAULT;
  gfClientConfig.q_gnss_pos_qos_sess_timeout = GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESS_TIMEOUT_DEFAULT;
  gfClientConfig.q_cpi_request_rate = GNSS_NV_EFS_SM_GM_CPI_REQUEST_RATE_DEFAULT;
  gfClientConfig.q_chal_gps_env_bo_min = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_MIN_DEFAULT;
  gfClientConfig.q_chal_gps_env_bo_max = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_MAX_DEFAULT;
  gfClientConfig.q_chal_gps_env_ms_dist_gps = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_GPS_DEFAULT;
  gfClientConfig.q_chal_gps_env_ms_dist_cpi = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_CPI_DEFAULT;   
  gfClientConfig.gnss_unavail_aggresive_exit = FALSE;
  

  if(gm_client_register(&z_gfc_qmi_info.z_client,&z_gfc_qmi_info.u_gm_handle,&gfClientConfig,&error_code) == FAILED)
  {
    MSG_HIGH("Not able to add gfc_qmi as Geofence client to GM, error_code %d", error_code,0,0 );
  }
  gfc_qmi_internal_set_geofence_engine_config_at_bootup();

  /*Also register as Coarse Position Injection Client*/

  z_gfc_qmi_info.z_cpi_client.client_id = GEOFENCE_CLIENT_CPI;
  z_gfc_qmi_info.z_cpi_client.q_base_msg_id = TM_CORE_GM_MSG_ID_BASE;
  // ToDo:: Need to change this transaction ID since GFC_QMI is not a protocol.
  z_gfc_qmi_info.z_cpi_client.q_base_transaction_id = TM_GFC_QMI_GM_TRANSACTION_ID_BASE;
  z_gfc_qmi_info.z_cpi_client.q_client_thread_id = THREAD_ID_SM_TM;
  /* The currently used GM transaction ID */
  z_gfc_qmi_info.z_cpi_client.client_cap.client_capabilites = GEOFENCE_CLIENT_POS_FIX_CAPABLE;
  z_gfc_qmi_info.z_cpi_client.client_cap.pos_fix_capability.pos_fix_method_cap = GEOFENCE_CLIENT_POS_FIX_CPI_CAPABLE;
  z_gfc_qmi_info.z_cpi_client.client_cap.pos_fix_capability.pos_fix_accuracy_cap = GEOFENCE_CLIENT_POS_FIX_ACCURACY_MEDIUM;
  z_gfc_qmi_info.z_cpi_client.client_cap.pos_fix_capability.pos_fix_accuracy = 50; /*Minimum accuracy can be as low as 50 meters*/
  z_gfc_qmi_info.z_cpi_client.client_cap.pos_fix_capability.pos_fix_power_cap = GEOFENCE_CLIENT_POS_FIX_POWER_PROFILE_4;
  /*CPI always wakes up AP*/
  z_gfc_qmi_info.z_cpi_client.client_cap.pos_fix_capability.high_power_proc_wakeup_cap = GEOFENCE_CLIENT_HIGH_POWER_PROC_WAKEUP_PROFILE_2;
  /*CPI may use Nw if assistance data is required*/
  z_gfc_qmi_info.z_cpi_client.client_cap.pos_fix_capability.nw_usage_cap = GEOFENCE_CLIENT_NW_USAGE_PROFILE_1;
  
  z_gfc_qmi_info.z_cpi_client.client_cap.pos_fix_capability.is_imp_location_notification_required = FALSE;
  z_gfc_qmi_info.z_cpi_client.client_cap.pos_fix_capability.pos_fix_wait_secs = 5; /*Geofence core shall wait for 5 seconds for the fix*/
  z_gfc_qmi_info.z_cpi_client.client_cap.pos_fix_capability.is_prior_fix_request_indication_required = FALSE; /*Prior fix request indication required*/
  z_gfc_qmi_info.z_cpi_client.client_cap.pos_fix_capability.prior_fix_request_indication_secs = 0; /*Prior fix request indication  not required*/

  if(gm_client_register(&z_gfc_qmi_info.z_cpi_client,&z_gfc_qmi_info.u_gm_cpi_handle,NULL,&error_code) == FAILED)
  {
    MSG_HIGH("Not able to add gfc_qmi as CPI Geofence client to GM, error_code %d", error_code,0,0 );
  }
}



/*===========================================================================

FUNCTION gfc_qmi_internal_tm_prtl_cb_init

DESCRIPTION
  Initializes function callback table for TM-Core.
  The GM core sends the geofence events to the TM core.
  The TM-Core uses these callbacks to send those events to GFC_QMI .

DEPENDENCIES
  None.

RETURN VALUE
  None.
SIDE EFFECTS
  None.

===========================================================================*/
void gfc_qmi_internal_tm_prtl_cb_init(void)
{
  /* Register the event handler for GM core related events for GFC_QMI */
  z_gfc_qmi_info.z_prtlIfaceTbl.gm_event_handle_fp  = gfc_qmi_internal_gm_event_handler;
}

/*
******************************************************************************
* tm_gfc_qmi_prtl_init
*
* Function description:
*
*   This function initialises the GFC_QMI module . This sets up the internal
*   states of the GFC_QMI and registers with GM core.
*
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/

void tm_gfc_qmi_prtl_init(void)
{
  memset(&z_gfc_qmi_info, 0, sizeof(gfc_qmi_internal_info_s_type));

  MSG_MED("tm_gfc_qmi_prtl_init", 0, 0, 0);

  /* Initialize the function callbacks of GFC_QMI with TM Core. */
  gfc_qmi_internal_tm_prtl_cb_init();

  (void)tm_prtl_reg(TM_PRTL_TYPE_GFC_QMI, &z_gfc_qmi_info.z_prtlIfaceTbl);

  /* Init with the GM core. */
  gfc_qmi_internal_gm_init();

}

/*
******************************************************************************
* gfc_qmi_internal_set_engine_config_at_bootup
*
* Function description:
*
*   This function is used to set the geofence engine configuration
*   at boot up time , by reading the required configuration items from NV.
*
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/
void gfc_qmi_internal_set_geofence_engine_config_at_bootup(void)
{
  gfc_qmi_set_geofence_engine_config_req_s_type z_set_gf_engine_config_request;
  gfc_qmi_operation_result_e_type e_return_code;
  geofence_engine_config_s_type *gm_config = NULL;

  tm_core_get_gm_engine_config(&gm_config);

  if(gm_config == NULL)
  {
    MSG_ERROR("NULL GM Config pointer",0,0,0);
    return;
  }


  memset(&z_set_gf_engine_config_request, 0, sizeof(z_set_gf_engine_config_request));
  /* Setthe GNSS unavailable timeout parameter. This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_GNSS_UNAVAIL_TIMEOUT_MASK;

  z_set_gf_engine_config_request.gnss_unavailable_indication_timeout = gm_config->sm_gm_gnss_unavail_ind_timeout;
  MSG_MED("GFC_QMI: Setting GNSS unavail ind timeout as %d from NV",z_set_gf_engine_config_request.gnss_unavailable_indication_timeout,0,0);

  /* Set Sesnor usage for motion detection parameter. This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_ENABLE_SENSORS_FOR_MOTION_DETECTION_MASK;

  if(gm_config->sm_gm_motion_detection_sources & GM_MOTION_DETECTOR_SENSORS)
  {
    z_set_gf_engine_config_request.q_gm_enable_sensor_motion_detection = TRUE;
  }
  else
  {
    z_set_gf_engine_config_request.q_gm_enable_sensor_motion_detection = FALSE;
  }

  MSG_MED("GFC_QMI: Setting GM Enable Sensors as %d from NV",
    z_set_gf_engine_config_request.q_gm_enable_sensor_motion_detection,0,0);

  /* Set Wifi usage for motion detection parameter. This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_ENABLE_WIFI_FOR_MOTION_DETECTION_MASK;

  if(gm_config->sm_gm_motion_detection_sources & GM_MOTION_DETECTOR_WIFI)
  {
    z_set_gf_engine_config_request.q_gm_enable_wifi_motion_detection = TRUE;
  }
  else
  {
    z_set_gf_engine_config_request.q_gm_enable_wifi_motion_detection = FALSE;
  }

  MSG_MED("GFC_QMI: Setting GM Enable Wifi as %d from NV",
    z_set_gf_engine_config_request.q_gm_enable_wifi_motion_detection,0,0);

  /* Set WWAN usage for motion detection parameter. This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_ENABLE_WWAN_FOR_MOTION_DETECTION_MASK;

  if(gm_config->sm_gm_motion_detection_sources & GM_MOTION_DETECTOR_WWAN)
  {
    z_set_gf_engine_config_request.q_gm_enable_wwan_motion_detection = TRUE;
  }
  else
  {
    z_set_gf_engine_config_request.q_gm_enable_wwan_motion_detection = FALSE;
  }

  MSG_MED("GFC_QMI: Setting GM Enable WWAN as %d from NV",
    z_set_gf_engine_config_request.q_gm_enable_wwan_motion_detection,0,0);

  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_GNSS_POS_QOS_SESS_TIMEOUT_MASK;

  z_set_gf_engine_config_request.gnss_pos_qos_sess_timeout = gm_config->sm_gm_gnss_pos_qos_sess_timeout;
  MSG_MED("GFC_QMI: Setting GNSS Pos Qos Sess timeout as %d from NV",
    z_set_gf_engine_config_request.gnss_pos_qos_sess_timeout,0,0);

  /* Set the CPI usage parameterr. This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_ENABLE_CPI_USAGE_MASK;

  if(gm_config->sm_gm_position_sources & GEOFENCE_POS_SOURCE_NV_CPI_MASK)
  {
    z_set_gf_engine_config_request.q_gm_enable_position_injection_usage = TRUE;
  }
  else
  {
    z_set_gf_engine_config_request.q_gm_enable_position_injection_usage = FALSE;
  }

  MSG_MED("GFC_QMI: Setting GM Enable CPI as %d from NV",
    z_set_gf_engine_config_request.q_gm_enable_position_injection_usage,0,0);

  /* Set the LPW usage parameterr. This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_ENABLE_LPW_USAGE_MASK;

  if(gm_config->sm_gm_position_sources & GEOFENCE_POS_SOURCE_NV_LPW_MASK)
  {
    z_set_gf_engine_config_request.q_gm_enable_lpw_usage = TRUE;
  }
  else
  {
    z_set_gf_engine_config_request.q_gm_enable_lpw_usage = FALSE;
  }

  MSG_MED("GFC_QMI: Setting GM Enable LPW as %d from NV",
    z_set_gf_engine_config_request.q_gm_enable_lpw_usage,0,0);

  /* Set the value for max pos unc accepted by GM  */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_GNSS_MAX_POS_UNC_ACCEPTED_MASK;
  z_set_gf_engine_config_request.q_gm_gnss_max_pos_unc_accepted = gm_config->sm_gm_gnss_max_pos_unc_accepted;
  /* Set the NV value for using the NW asst fixes by GM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_USE_NW_ASST_FIXES_MASK;
  z_set_gf_engine_config_request.b_use_nw_asst_fixes = gm_config->sm_gm_use_nw_asst_fixes;
  MSG_MED("GFC_QMI: Setting GM use NW asst fixes %d from NV",
    z_set_gf_engine_config_request.b_use_nw_asst_fixes,0,0);


  /* Set the High Repsonsiveness Config parameterr. This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_HIGH_RESPONSIVENESS_CONFIG_MASK;
  z_set_gf_engine_config_request.q_gm_high_responsiveness_config = gm_config->sm_gm_high_responsiveness_config;

  MSG_MED("GFC_QMI: Setting GM High Resp Config as %d from NV",
    z_set_gf_engine_config_request.q_gm_high_responsiveness_config,0,0);

  /* Set the NV value for changing the CPI request rate by GM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_CPI_REQUEST_RATE_CONFIG_MASK;
  z_set_gf_engine_config_request.q_gm_cpi_request_rate = gm_config->sm_gm_cpi_request_rate;
  MSG_MED("GFC_QMI: Setting GM CPI request rate %d from NV",
    z_set_gf_engine_config_request.q_gm_cpi_request_rate,0,0);

  /* Set the Medium resp. backoff parameter. This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_MED_RESP_BACKOFF_MASK;
  z_set_gf_engine_config_request.q_gm_med_resp_backoff = gm_config->sm_gm_med_resp_backoff;

  /* Set the Minimum backoff when in Challenging GPS Env parameter. This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_CHAL_GPS_ENV_BO_MIN_MASK;
  z_set_gf_engine_config_request.q_gm_chal_gps_env_bo_min = gm_config->sm_gm_chal_gps_env_bo_config.q_backoff_min;

  /* Set maximum backoff when in Challenging GPS Env parameter. This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_CHAL_GPS_ENV_BO_MAX_MASK;
  z_set_gf_engine_config_request.q_gm_chal_gps_env_bo_max = gm_config->sm_gm_chal_gps_env_bo_config.q_backoff_max;

  /* Set the motion sensing distance for GPS in case of challenging GPS env. This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_CHAL_GPS_ENV_MS_DIST_GPS_MASK;
  z_set_gf_engine_config_request.q_gm_chal_gps_env_ms_dist_gps = gm_config->sm_gm_chal_gps_env_ms_dist_config.q_dist_used_for_gps;

  /* Set the motion sensing distance for GPS in case of challenging GPS env. This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_CHAL_GPS_ENV_MS_DIST_CPI_MASK;
  z_set_gf_engine_config_request.q_gm_chal_gps_env_ms_dist_cpi = gm_config->sm_gm_chal_gps_env_ms_dist_config.q_dist_used_for_cpi;

  /* Set MotionState speeds. This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_MS_SPEED_MASK;
  z_set_gf_engine_config_request.q_gm_ms_speed_config.b_walking_valid = TRUE;
  z_set_gf_engine_config_request.q_gm_ms_speed_config.gm_ms_walk_speed =
    gm_config->sm_gm_motion_state_speed_config.gm_ms_walk_speed;
  z_set_gf_engine_config_request.q_gm_ms_speed_config.b_running_valid = TRUE;
  z_set_gf_engine_config_request.q_gm_ms_speed_config.gm_ms_run_speed =
    gm_config->sm_gm_motion_state_speed_config.gm_ms_run_speed;

  /* Set the NV value for changing the CPI request rate by GM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_GM_OPERATION_MODE_CONFIG_MASK;
  z_set_gf_engine_config_request.q_gm_pos_operation_mode = PDSM_SESSION_OPERATION_MSBASED;

  /* Set the IOD config switch mask */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_IOD_CONFIG_MASK;
  z_set_gf_engine_config_request.q_sm_gm_iod_config_switch_mask = gm_config->q_sm_gm_iod_config_switch_mask;

  /* Set Comprehensive logging rate*/
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_COMP_LOGGING_RATE_CONFIG_MASK;
  z_set_gf_engine_config_request.q_gm_comp_logging_rate = gm_config->q_gm_comp_logging_rate;
  
  /* Enable/Disable GM LOWI registration . This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_LOWI_REGISTER_ENABLE;
  z_set_gf_engine_config_request.b_lowi_reg_enable = gm_config->sm_gm_lowi_reg_enable;

  /* Subscribe to different services on LOWI. This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_LOWI_SUBSCRIPTION_MASK;
  z_set_gf_engine_config_request.q_lowi_subs_mask = gm_config->sm_gm_lowi_sub_mask;

  /* Enable different registration timers for GM . This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_LOWI_REG_TIMER_MASK;
  z_set_gf_engine_config_request.q_lowi_enable_reg_timer_mask = gm_config->sm_gm_lowi_reg_config.q_enable_reg_timer_mask;

  /* The maximum retry count of short term registration timer. This parameter is read from the EFS NV by TM */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_LOWI_SHORT_TERM_TIMER_RETRY_CNT;
  z_set_gf_engine_config_request.q_lowi_short_term_timer_max_retry_cnt = gm_config->sm_gm_lowi_reg_config.q_short_term_timer_max_retry_cnt;

  MSG_MED("Enable GM LOWI reg::%d LOWI Subs mask ::0x%x ", 
          z_set_gf_engine_config_request.b_lowi_reg_enable, 
          z_set_gf_engine_config_request.q_lowi_subs_mask, 
          0);

  /* Set up the default maximum number of geofences for GFC QMI */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_MAX_GF_MASK;
  z_set_gf_engine_config_request.q_max_gf = GFC_QMI_GM_MAX_GF_DEFAULT;
  MSG_MED("Enable LOWI reg mask::0x%x Short term timer retry::%d ", 
          z_set_gf_engine_config_request.q_lowi_enable_reg_timer_mask, 
          z_set_gf_engine_config_request.q_lowi_short_term_timer_max_retry_cnt, 
          0);

    /* Set the GM filter WLAN frequencies mask */
  z_set_gf_engine_config_request.t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_FILTER_WLAN_FREQ_MASK;
  z_set_gf_engine_config_request.q_sm_gm_filter_wlan_freq_mask = gm_config->q_sm_gm_filter_wlan_freq_mask;

  /* Hard code the transaction ID as of now */
  z_set_gf_engine_config_request.transaction_id = NON_QMI_TRXN_ID;
  gfc_qmi_set_geofence_engine_config(&z_set_gf_engine_config_request, &e_return_code);
}

/*=============================================================================

  FUNCTION      CoarsePosToNavSolution

  DESCRIPTION   Use the contents of a external coarse position structure to
                populate a nav solution structure.

  DEPENDENCIES  None

  PARAMETERS    pz_NavSolution: Nav solution structure to be populated
                pz_CoarsePosInfo: Input coarse position structure

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
static boolean gfc_qmi_convert_coarsePos_to_fixRpt( sm_GnssFixRptStructType* p_fixRpt,
                                   const pdsm_pd_external_position_info_type* pz_CoarsePosInfo )
{
  float                        f_scale_up_to_99pct_conf;

  if( (p_fixRpt == NULL) ||
      (pz_CoarsePosInfo == NULL)
    )
  {
    MSG_ERROR("Null pointers passed to gfc_qmi_convert_coarsePos_to_fixRpt",0,0,0);
    return FALSE;
  }

  /* Update validity flags */
  p_fixRpt->z_NavPos.z_PosFlags.b_IsExternal = TRUE;
  p_fixRpt->z_NavPos.z_PosFlags.b_IsCoarsePos = TRUE;

  /* Set WIFI source flag */
  if (pz_CoarsePosInfo->source == PDSM_PD_EXTERN_COARSE_POS_SOURCE_WIFI)
  {
    p_fixRpt->z_NavPos.z_PosFlags.b_IsWifiGenerated = TRUE;
  }

  p_fixRpt->z_NavPos.z_PosFlags.b_IsVelValid = FALSE;
 /* Velocity valid is set to false */
  MSG_LOW("GFC_QMI:CPI injection velocity valid = %d", p_fixRpt->z_NavPos.z_PosFlags.b_IsVelValid, 0, 0);

  /* Heading valid is set to false */
  p_fixRpt->z_NavPos.z_PosFlags.b_IsHeadingValid = FALSE;

  /* Convert valid, coarse pos time stamp to GPS week and GPS msec */
  if ( pz_CoarsePosInfo->opt_field_mask & PDSM_PD_EXTERN_COARSE_POS_GPS_TIME_VALID )
  {
    U32 q_CoarsePosGpsTotalSec;
    if ( pz_CoarsePosInfo->time_info_type == PDSM_PD_EXTERN_COARSE_POS_TIME_GPS )
    {
      q_CoarsePosGpsTotalSec = pz_CoarsePosInfo->timestamp_sec;
      p_fixRpt->z_NavPos.w_GpsWeek =
        (U16)(q_CoarsePosGpsTotalSec / (U32)WEEK_SECS);
      p_fixRpt->z_NavPos.q_GpsTimeMs =
        (q_CoarsePosGpsTotalSec % (U32)WEEK_SECS) * 1000;
	   p_fixRpt->z_NavPos.q_UtcTimeMs =
		((pz_CoarsePosInfo->timestamp_sec - (U32)cgps_GetNvUtcOffset()) % (U32)WEEK_SECS) * 1000;

	  MSG_HIGH("GFC_QMI:CPI injection extern time type= GPS GPS Week = %u, GPS Ms = %u",
		p_fixRpt->z_NavPos.w_GpsWeek, p_fixRpt->z_NavPos.q_GpsTimeMs,
		p_fixRpt->z_NavPos.q_UtcTimeMs);
    }
    else if ( pz_CoarsePosInfo->time_info_type == PDSM_PD_EXTERN_COARSE_POS_TIME_UTC )
    {
      q_CoarsePosGpsTotalSec = pz_CoarsePosInfo->timestamp_sec
                              + (U32)cgps_GetNvUtcOffset();
      p_fixRpt->z_NavPos.w_GpsWeek =
        (U16)(q_CoarsePosGpsTotalSec / (U32)WEEK_SECS);
      p_fixRpt->z_NavPos.q_GpsTimeMs =
        (q_CoarsePosGpsTotalSec % (U32)WEEK_SECS) * 1000;
	  p_fixRpt->z_NavPos.q_UtcTimeMs =
		(pz_CoarsePosInfo->timestamp_sec % (U32)WEEK_SECS) * 1000;

	  MSG_HIGH("GFC_QMI:CPI injection extern time type= UTC GPS Week = %u, GPS Ms = %u, UTC ms = %u",
		p_fixRpt->z_NavPos.w_GpsWeek, p_fixRpt->z_NavPos.q_GpsTimeMs,
		p_fixRpt->z_NavPos.q_UtcTimeMs);
    }
    else
    {
      p_fixRpt->z_NavPos.w_GpsWeek = C_GPS_WEEK_UNKNOWN;
      p_fixRpt->z_NavPos.q_GpsTimeMs = 0;
    }
  }
  else
  {
    MSG_HIGH("ExternCoarsePosPut: No valid gps time info",0,0,0);
    p_fixRpt->z_NavPos.w_GpsWeek = C_GPS_WEEK_UNKNOWN;
    p_fixRpt->z_NavPos.q_GpsTimeMs = 0;
  }


  if ( pz_CoarsePosInfo->opt_field_mask & PDSM_PD_EXTERN_COARSE_POS_LAT_LONG_VALID )
  {
    float  horizontal_unc = 0;
    uint8  horizontal_conf = 0;

	/* mark the position as valid */
	p_fixRpt->z_NavPos.z_PosFlags.b_IsValid = TRUE;

	/* mark the position as CPI */
	p_fixRpt->z_NavPos.z_SmPosFlags.b_IsCoarsePos = TRUE;

	/* Convert to radians from degrees */
    p_fixRpt->z_NavPos.d_PosLla[0] = (DBL)pz_CoarsePosInfo->latitude * DEG2RAD;

    /* Convert to radians from degrees */
    p_fixRpt->z_NavPos.d_PosLla[1] = (DBL)pz_CoarsePosInfo->longitude * DEG2RAD;

    p_fixRpt->z_NavPos.d_PosLla[2] = pz_CoarsePosInfo->altitude;

    /*Check from where to pick up Horizontal Unc and Horizontal Confidence*/
    if( pz_CoarsePosInfo->opt_field_mask & PDSM_PD_EXTERN_COARSE_POS_RAW_CIRCULAR_UNC_VALID)
    {
      MSG_HIGH("Taking Hor Unc and Confidence from Raw field",0,0,0);
      horizontal_unc = pz_CoarsePosInfo->loc_unc_hor_raw;
      if( pz_CoarsePosInfo->opt_field_mask & PDSM_PD_EXTERN_COARSE_POS_RAW_HORIZ_CONF_VALID)
      {
        horizontal_conf = pz_CoarsePosInfo->confidence_hor_raw;
      }
      else
      {
        horizontal_conf = 50;
      }
    }
    else
    {
      MSG_HIGH("Not Taking Hor Unc and Confidence from Raw field",0,0,0);
      horizontal_unc = pz_CoarsePosInfo->loc_unc_hor;
      if( pz_CoarsePosInfo->opt_field_mask & PDSM_PD_EXTERN_COARSE_POS_HORIZ_CONF_VALID)
      {
        horizontal_conf = pz_CoarsePosInfo->confidence_hor;
      }
      else
      {
        horizontal_conf = 50;
      }
    }


    f_scale_up_to_99pct_conf = tm_util_get_cep_scale_ratio_from_pct( (float)horizontal_conf* 0.01f, 0.99f);

    /* Update the error ellipse for horizontal position uncertainty and ensure
      the result is within bounds */
    p_fixRpt->z_NavPos.f_ErrorEllipse[0] = 0.0;
    p_fixRpt->z_NavPos.f_ErrorEllipse[1] = (FLT)( (float)horizontal_unc * (float)f_scale_up_to_99pct_conf )
      * (FLT)(1./3.03); /*CEP 99% -> Error Ellipse 39%*/

    p_fixRpt->z_NavPos.f_ErrorEllipse[2] = p_fixRpt->z_NavPos.f_ErrorEllipse[1];

    /* Set the latitude and longitude position uncertaintes as the error ellipse
      directly. The semi-major axis corresponds to longitude b/c alpha is 0. */
    p_fixRpt->z_NavPos.f_PuncLatLonMeters[0] = p_fixRpt->z_NavPos.f_ErrorEllipse[2];
    p_fixRpt->z_NavPos.f_PuncLatLonMeters[1] = p_fixRpt->z_NavPos.f_ErrorEllipse[1];

    /* Horizontal reliability */
    if ( pz_CoarsePosInfo->e_HoriRelIndicator == PDSM_POSITION_RELIABILITY_NOT_SET )
    {
      p_fixRpt->z_NavPos.e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_LOW;
    }
    else
    {
      p_fixRpt->z_NavPos.e_HoriRelIndicator = pz_CoarsePosInfo->e_HoriRelIndicator;
    }

    /* Update vertical position uncertainties */
    if ( (pz_CoarsePosInfo->opt_field_mask & PDSM_PD_EXTERN_COARSE_POS_ALT_VALID)
      && (pz_CoarsePosInfo->e_VertRelIndicator != PDSM_POSITION_RELIABILITY_VERY_LOW) )
    {
      /* If HAE, accept uncertainty directly */
      if ( pz_CoarsePosInfo->alt_def == PDSM_PD_EXTERN_COARSE_POS_ALT_HAE )
      {
        p_fixRpt->z_NavPos.f_PuncVertMeters = (FLT)pz_CoarsePosInfo->loc_unc_vert;
      }
      else
      {
        /* On receipt of an altitude in MSL from coarse position, increase uncertainty
          to compensate difference between MSL height and ellipsoidal height */
        /* Otherwise assume MSL and increase uncertainty. Optimally, the
          altitude reference could be adjusted to HAE by accounting for geoid
          in PE. This would avoid the unnecessary uncertainty increase. */
        p_fixRpt->z_NavPos.f_PuncVertMeters =
          (FLT)pz_CoarsePosInfo->loc_unc_vert + (FLT)100.0;
      }
      /* Vertical reliability */
      if ( pz_CoarsePosInfo->e_VertRelIndicator == PDSM_POSITION_RELIABILITY_NOT_SET )
      {
        p_fixRpt->z_NavPos.e_VertRelIndicator = PDSM_POSITION_RELIABILITY_LOW;
      }
      else
      {
        p_fixRpt->z_NavPos.e_VertRelIndicator = pz_CoarsePosInfo->e_VertRelIndicator;
      }
    }

    /* Compute the 3D RSS position uncertainty in terms of the components and
      ensure the result is within bounds */
    p_fixRpt->z_NavPos.f_Punc3dMeters =
      (FLT)sqrt( p_fixRpt->z_NavPos.f_ErrorEllipse[1] * p_fixRpt->z_NavPos.f_ErrorEllipse[1]
      + p_fixRpt->z_NavPos.f_ErrorEllipse[2] * p_fixRpt->z_NavPos.f_ErrorEllipse[2]
      + p_fixRpt->z_NavPos.f_PuncVertMeters * p_fixRpt->z_NavPos.f_PuncVertMeters );

    /* Set the total RSS velocity uncertainty to maximum */
    p_fixRpt->z_NavPos.f_Vunc3dMps = (FLT)30.0;
  }
  else if ( (pz_CoarsePosInfo->opt_field_mask & PDSM_PD_EXTERN_COARSE_POS_ALT_VALID)
    && (pz_CoarsePosInfo->e_VertRelIndicator != PDSM_POSITION_RELIABILITY_VERY_LOW) )
  {
    /* Process the altitude information, ensuring ECEF and LLA positions are
      consistent */
    p_fixRpt->z_NavPos.d_PosLla[2] = pz_CoarsePosInfo->altitude;

    /* If HAE, accept uncertainty directly */
    if ( pz_CoarsePosInfo->alt_def == PDSM_PD_EXTERN_COARSE_POS_ALT_HAE )
    {
      p_fixRpt->z_NavPos.f_PuncVertMeters = (FLT)pz_CoarsePosInfo->loc_unc_vert;
    }
    else
    {
      /* Otherwise assume MSL and increase uncertainty. Optimally, the
        altitude reference could be adjusted to HAE by accounting for geoid
        in PE. This would avoid the unnecessary uncertainty increase. */
      p_fixRpt->z_NavPos.f_PuncVertMeters =
        (FLT)pz_CoarsePosInfo->loc_unc_vert + (FLT)100.0;
    }

    /* Vertical reliability */
    if ( pz_CoarsePosInfo->e_VertRelIndicator == PDSM_POSITION_RELIABILITY_NOT_SET )
    {
      p_fixRpt->z_NavPos.e_VertRelIndicator = PDSM_POSITION_RELIABILITY_LOW;
    }
    else
    {
      p_fixRpt->z_NavPos.e_VertRelIndicator = pz_CoarsePosInfo->e_VertRelIndicator;
    }
  }

  return TRUE;
}


/*
******************************************************************************
* tm_gfc_qmi_inject_ext_pos
*
* Function description:
*
*   This function handles Ext position injection
*
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/
void tm_gfc_qmi_inject_ext_pos(pdsm_pd_external_position_info_type* ext_pos_ptr)
{
  sm_GnssFixRptStructType gnss_fix_rpt;
  sm_GnssFixRptStructType* p_fix_rpt = &gnss_fix_rpt;
  boolean u_sanity_ok = TRUE;
  sm_gm_pos_fix_info fix_info;
  float  horizontal_unc = 0;
  uint8  horizontal_conf = 0;

  memset(&gnss_fix_rpt, 0, sizeof(gnss_fix_rpt));
  if(ext_pos_ptr == NULL)
  {
    MSG_ERROR("Null Ext Pos Ptr",0,0,0);
    return;
  }

  /*Check from where to pick up Horizontal Unc and Horizontal Confidence*/
  if( ext_pos_ptr->opt_field_mask & PDSM_PD_EXTERN_COARSE_POS_RAW_CIRCULAR_UNC_VALID)
  {
    horizontal_unc = ext_pos_ptr->loc_unc_hor_raw;
    if( ext_pos_ptr->opt_field_mask & PDSM_PD_EXTERN_COARSE_POS_RAW_HORIZ_CONF_VALID)
    {
      horizontal_conf = ext_pos_ptr->confidence_hor_raw;
    }
    else
    {
      horizontal_conf = 50;
    }
  }
  else
  {
    horizontal_unc = ext_pos_ptr->loc_unc_hor;
    if( ext_pos_ptr->opt_field_mask & PDSM_PD_EXTERN_COARSE_POS_HORIZ_CONF_VALID)
    {
      horizontal_conf = ext_pos_ptr->confidence_hor;
    }
    else
    {
      horizontal_conf = 50;
    }
  }

  if ( !( ext_pos_ptr->opt_field_mask &
        PDSM_PD_EXTERN_COARSE_POS_LAT_LONG_VALID ) &&
       !( ext_pos_ptr->opt_field_mask &
        PDSM_PD_EXTERN_COARSE_POS_ALT_VALID )
     )
  {
    MSG_HIGH("No hor. or vert. pos", 0, 0, 0);
    u_sanity_ok = FALSE; /* no hor. or vert. pos. info., reject */
  }
  /* If the coarse position is very low reliablity, the input remains invalid */
  else if ( (ext_pos_ptr->opt_field_mask & PDSM_PD_EXTERN_COARSE_POS_LAT_LONG_VALID)
            && (ext_pos_ptr->e_HoriRelIndicator == PDSM_POSITION_RELIABILITY_VERY_LOW) )
  {
    MSG_HIGH("Very Low Reliability", 0, 0, 0);
    u_sanity_ok = FALSE; /* no hor. or vert. pos. info., reject */
  }
  else if ( ( ext_pos_ptr->opt_field_mask &
        PDSM_PD_EXTERN_COARSE_POS_LAT_LONG_VALID ) &&
      ( ( horizontal_unc == 0 ) ||
        ( horizontal_conf == 0 ) ||
        ( horizontal_conf > 100 )
      )
     )
  {
    u_sanity_ok = FALSE; /* hor. unc = 0 unreasonable, reject */
  }
  else if ( ( ext_pos_ptr->opt_field_mask &
        PDSM_PD_EXTERN_COARSE_POS_ALT_VALID ) &&
       ( ( ext_pos_ptr->loc_unc_vert == 0 ) ||
         ( ext_pos_ptr->confidence_vert == 0 ) ||
         ( ext_pos_ptr->confidence_vert > 100 )
       )
     )
  {
    u_sanity_ok = FALSE; /* vert. unc = 0 unreasonable, reject */
  }

  if(u_sanity_ok == FALSE)
  {
    MSG_ERROR("Sanity failed for Ext Pos",0,0,0);
    return;
  }

  if(gfc_qmi_convert_coarsePos_to_fixRpt(p_fix_rpt, ext_pos_ptr) == FALSE)
  {
    MSG_ERROR("Error converting Ext pos to Fix Rpt",0,0,0);
    return;
  }

  fix_info.fix_type = GEOFENCE_POS_FIX_TYPE_FINAL; /*Dont care*/
  fix_info.fix_src = GEOFENCE_POS_FIX_SRC_CPI;
  fix_info.pos_fix = *p_fix_rpt;

  gm_position_update(&fix_info);

  gfc_qmi_internal_stop_cpi_fix();

}

