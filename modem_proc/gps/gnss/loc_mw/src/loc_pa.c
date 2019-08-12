/******************************************************************************
  @file:  loc_pa.c
  @brief: Location Middleware PA module

  DESCRIPTION
This module configures/retrieves the NV setting of location
  engine on the modem, and processes EFS data.

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Copyright (c) 2013-2014 QUALCOMM Atheros, Inc.
  All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
 ******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ------------------------------------------------------- 
10/06/15   yh       QZSS
08/28/15   ah       Added missing break when handling GET_SUPL_SECURITY
05/18/15   jv       Added deletion bits for SBAS eph & alm.
08/08/14   jv       Galileo Support added 
04/15/14   yh       XTRA version check added 
04/28/14   ah       Added handling of Set Premium Services indication status
03/14/14   ss       Handling EFS command response in loc_pa_event_cb_ext
03/10/14   ah       Added support for Set Premium Services Cfg
02/20/14   ah       Fixed issue with Get NMEA Type indication
01/24/14   ah       Added GLO support for Get/Set NMEA Type Requests in Loc MW
03/29/10   ns       Changed Loc MW to be an OEM type client
10/31/09   dx       EFS data injection
01/01/09   wc/etc   Initial version and development

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_pa.c#1 $
$DateTime: 2016/12/13 07:59:45 $
======================================================================*/

#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "msg.h"
#include "queue.h"

#include "aries_os_api.h"
#include "pdapi.h"
#include "pdsm_atl.h"
#include "loc_api_2.h"

#include "loc_task.h"
#include "loc_client.h"
#include "loc_pd.h"
#include "loc_conn.h"
#include "loc_xtra.h"
#include "loc_pa.h"
#include "loc_ni.h"
#include "loc_wifi.h"
#include "loc_api_internal.h"

#include "location_service_v02.h"

#define PA_MS_PER_SEC                 (1000)
#define PA_OPER_DEFAULT_TIMEOUT       (4 * PA_MS_PER_SEC )  

static void loc_pa_event_cb
(
      void                       *user_data,   /* user data pointer */
      pdsm_pa_event_type          pa_event,    /* Indicate PD event */
      const pdsm_pa_info_s_type  *pa_info_ptr  /* PARAM pa_info_ptr POINTER */
);

static void loc_pa_event_cb_ext
(
  /* user data pointer */
  void *user_data_ptr,  
  /* Indicate PDSM param type, PDSM_PA_xxx enum */
  pdsm_pa_e_type  pa_type, 
  /* A pointer to a PD state information struct */   
  const pdsm_pa_info_s_type    *pa_info_ptr, 
  /* Optional field to indicate success/failure status of PA operation */     
  pdsm_pa_cmd_err_e_type        status    
);

static void loc_pa_cmd_cb
(
      void                      *data_block_ptr,
      pdsm_pa_cmd_e_type        pa_cmd,
      pdsm_pa_cmd_err_e_type    pa_cmd_err
);


static int32 loc_pa_xlate_cmd_err (pdsm_pa_cmd_err_e_type pa_cmd_err);

static int loc_pa_set_engine_lock (loc_client_handle_type  client_handle, const loc_lock_e_type* loc_type_ptr);

static int loc_pa_set_server_addr
(
      loc_client_handle_type         client_handle,
      loc_ioctl_e_type        ioctl_type,
      const loc_server_info_s_type  * server_addr_ptr
);


static int loc_pa_set_sbas_config
(
      loc_client_handle_type  client_handle,
      boolean          enable_sbas
);

static int loc_pa_set_nmea_types
(
      loc_client_handle_type         client_handle,
      const loc_nmea_sentence_type* nmea_types_ptr
);

static int loc_pa_set_on_demand_lpm
(
      loc_client_handle_type  client_handle,
      boolean                 mode
);


static int loc_pa_set_apn_profiles
(
  loc_client_handle_type  client_handle,
  loc_ioctl_e_type        profile_type,
  const loc_apn_profiles_type *apn_profile_p
);


static int loc_pa_delete_aiding_data
(
      loc_client_handle_type client_handle,
      const loc_assist_data_delete_s_type* aiding_data_deletion_info_ptr
);

static void loc_pa_reset_module (void);

static int loc_pa_xlate_lock_from_pdapi_to_locapi
(
      const pdsm_gps_lock_e_type*   pdapi_lock_ptr,
      loc_lock_e_type*              locapi_lock_ptr
);

static int loc_pa_xlate_lock_from_locapi_to_pdapi
(
      const loc_lock_e_type*  locapi_lock_ptr,
      pdsm_gps_lock_e_type*   pdapi_lock_ptr
);

static int loc_pa_xlate_sbas_status_from_pdapi_to_locapi
(
      const pdsm_pa_sbas_status_e_type*  pdapi_sbas_mode_ptr,
      boolean*                           locapi_sbas_mode_ptr
);

static int loc_pa_xlate_nmea_types_from_pdapi_to_locapi
(
      const pdsm_pa_nmea_type*  pdapi_nmea_types_ptr,
      loc_nmea_sentence_type*   locapi_nmea_types_ptr
);

static int loc_pa_xlate_nmea_types_from_locapi_to_pdapi
(
      const loc_nmea_sentence_type* locapi_nmea_types_ptr,
      pdsm_pa_nmea_type*            pdapi_nmea_types_ptr
);

static int loc_pd_xlate_aiding_data_mask_from_locapi_to_pdapi
(
      pdsm_delete_parms_type                  *pdapi_aiding_data_mask_ptr,
      const loc_assist_data_delete_s_type   *locapi_aiding_data_mask_ptr
);

static int loc_pa_access_efs_data
(
      loc_client_handle_type     client_handle,
      const loc_efs_data_s_type *efs_data
);

static int loc_pa_set_user_control
(
      loc_client_handle_type client_handle,
      boolean                data_enable
);

static int loc_pa_set_supl_version
(
      loc_client_handle_type client_handle,
      const uint32           supl_version
);

static int loc_pa_set_xtra_t_session_ctrl
(
      loc_client_handle_type         client_handle,
      const uint8 control 
);

static int loc_pa_set_supl_security 
(
  loc_client_handle_type  client_handle, 
  boolean supl_security
);

static int loc_pa_set_pe_config 
(
  loc_client_handle_type  client_handle, 
  const qmiLocSetPositionEngineConfigParametersReqMsgT_v02 *pz_SetPeCfg
);

static int loc_pa_set_lpp_config
(
  loc_client_handle_type  client_handle,
  const qmiLocLppConfigMaskT_v02 * const lpp_config_ptr
);

static int loc_pa_set_assisted_glo_config
(
  loc_client_handle_type  client_handle,
  qmiLocAssistedGlonassProtocolMaskT_v02 assisted_glo_config
);

static int loc_pa_set_emergency_protocol
(
  loc_client_handle_type  client_handle,
  qmiLocEmergencyProtocolEnumT_v02 emergency_protocol
);

static int loc_pa_get_param
(
  loc_client_handle_type  client_handle,
  pdsm_pa_e_type          pa_cmd   
);

static int loc_pa_set_supl_hash_algo
(
  loc_client_handle_type  client_handle,
  qmiLocSuplHashAlgoEnumT_v02 supl_hash_algo
);

static int loc_pa_set_supl_tls_version
(
  loc_client_handle_type  client_handle,
  qmiLocSuplTlsVersionEnumT_v02 supl_tls_version
);

static int loc_pa_set_premium_svcs_cfg
(
  loc_client_handle_type                        client_handle,
  const qmiLocSetPremiumServicesCfgReqMsgT_v02* premium_svc_cfg
);

/** loc_pa_set_wifi_scan_inject_timeout
 *  Handle the request to set the wifi scan info injection
 *  timeout
 *  @param[in] client_handle: Loc API client handle
 *  @param [in] wifi_scan_inject_timeout: timeout duration
 *   */
static int loc_pa_set_wifi_scan_inject_timeout
(
  loc_client_handle_type  client_handle,
  uint8 wifi_scan_inject_timeout
);

static int loc_pa_set_xtra_version_check
(
  loc_client_handle_type  client_handle,
  qmiLocXtraVersionCheckEnumT_v02  xtra_version
);

static int loc_pa_set_gnss_constell_report_config
(
  loc_client_handle_type  client_handle,
  const qmiLocSetGNSSConstRepConfigReqMsgT_v02* gnss_config_ptr
);

static int loc_pa_xlate_gnss_config_from_locapi_to_pdapi
(
  const qmiLocSetGNSSConstRepConfigReqMsgT_v02*  locapi_gnss_config_ptr,
  pdsm_LocSetGNSSConfigReqStruct*   pdapi_gnss_config_ptr
);

static int loc_pa_delete_gnss_service_data
(
  loc_client_handle_type  l_ClientHandle,
  const qmiLocDeleteGNSSServiceDataReqMsgT_v02* pz_DeleteGnssServiceData
);

static int loc_pa_xlate_delete_gnss_service_data
(
  const qmiLocDeleteGNSSServiceDataReqMsgT_v02  *pz_DeleteGnssServiceData,
  pdsm_delete_parms_type                        *pz_PdapiDeleteDataMask
);

/**
 * Table to map the assisted glonass config masks between 
 * QMI_LOC and PDAPI 
 */
static struct 
{
  qmiLocAssistedGlonassProtocolMaskT_v02 qmiLocAssistedGlonassMask;
  uint32 pdapiAssistedGlonassMask;
}loc_pa_assisted_glo_map[] =
{
  {QMI_LOC_ASSISTED_GLONASS_PROTOCOL_MASK_RRC_CP_V02,
   PDSM_PA_ASST_GLO_PROTOCOL_RRC_CP },
  {QMI_LOC_ASSISTED_GLONASS_PROTOCOL_MASK_RRLP_UP_V02,
   PDSM_PA_ASST_GLO_PROTOCOL_RRLP_UP },
  {QMI_LOC_ASSISTED_GLONASS_PROTOCOL_MASK_LPP_UP_V02,
    PDSM_PA_ASST_GLO_PROTOCOL_LPP_UP },
  {QMI_LOC_ASSISTED_GLONASS_PROTOCOL_MASK_LPP_CP_V02,
    PDSM_PA_ASST_GLO_PROTOCOL_LPP_CP }
};


/* -----------------------------------------------------------------------*//**
@brief
  A helper utility to ensure the validity of a boolean. Booleans are usually
  represented in uint8 where "0x00" is FALSE and "0x01" is TRUE. However 
  over the wire it is possible that an 8-bit unsigned integer whose absolute 
  value does not represent a boolean is sent in. This function is provided to 
  check if the 8-bit unsigned integer is a valid-boolean representation or not
  
@param[in]   u_Value           The value which needs to be validated

@retval      TRUE              If value is a boolean
@retval      FALSE             If value is not a boolean

@warning 
  This check is done for incoming messages.
*//* ------------------------------------------------------------------------*/
static GNSS_INLINE boolean loc_pa_is_valid_boolean(uint8  u_Value)
{
  /* Assumption: TRUE is "1" and FALSE is "0", ie the valid values
     are less than "2" */
  return ( u_Value < 2 );
}

// Initialize location pa module
boolean loc_pa_init ()
{
   pdsm_client_status_e_type  pdsm_ret_val;
   boolean                    ret_val;
   loc_pa_module_data_s_type* pa_module_data_ptr;

   pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
   pa_module_data_ptr->mo_method = MO_UP;
   do
   {
      pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
      pa_module_data_ptr->pa_process_timer = 
                  os_TimerCreate( LOC_MIDDLEWARE_TIMER_ID_PA, THREAD_ID_LOC_MIDDLEWARE );
   
      if (loc_middleware_data.pdapi_client_id == -1)
      {
         loc_middleware_data.pdapi_client_id = pdsm_client_init(PDSM_CLIENT_TYPE_PDA);
      }
   
      if (loc_middleware_data.pdapi_client_id == -1)
      {
         LOC_MSG_ERROR ( "pdsm_client_init failed", 0, 0, 0);
         ret_val = FALSE;
         break;
      }
   
      pdsm_ret_val = pdsm_client_pa_reg(loc_middleware_data.pdapi_client_id,
                                        NULL,
                                        loc_pa_event_cb,
                                        PDSM_CLIENT_EVENT_REG,
                                        PDSM_PA_EVENT_MASK,
                                        NULL);   
      LOC_MSG_HIGH ("loc_pa_init, pa reg = %d", pdsm_ret_val, 0, 0);
   
      if (pdsm_ret_val != PDSM_CLIENT_OK)
      {
         ret_val = FALSE;
         break;
      }

      /* register with the pdsm pa extension*/

      pdsm_ret_val = pdsm_client_pa_reg_ex(loc_middleware_data.pdapi_client_id,
                                           NULL,
                                           loc_pa_event_cb_ext,
                                           PDSM_CLIENT_EVENT_REG,
                                           NULL);

      if (pdsm_ret_val != PDSM_CLIENT_OK)
      {
         LOC_MSG_ERROR ("loc_pa_init: pa reg ex failed Error = %d", 
                        pdsm_ret_val, 0, 0);
         ret_val = FALSE;
         break;
      }
   
      pdsm_ret_val = pdsm_client_act(loc_middleware_data.pdapi_client_id);
      if (pdsm_ret_val != PDSM_CLIENT_OK)
      {
         ret_val = FALSE;
         break;
      }
      else
      {
         ret_val = TRUE;
      }

      // Issue a command to get mo method
      loc_pa_get_param(LOC_CLIENT_HANDLE_INVALID,
                       PDSM_PA_MO_METHOD);

   } while (0);

   return ret_val;
}

// Process loc_ioctl that belongs to pa module
int loc_pa_process_ioctl
(
      loc_client_handle_type       client_handle,
      loc_ioctl_e_type             ioctl_type,
      const loc_ioctl_data_u_type* ioctl_data
)
{
   int                         ret_val = LOC_API_GENERAL_FAILURE;
   loc_pa_module_data_s_type  *pa_module_data_ptr;

   pa_module_data_ptr = &(loc_middleware_data.pa_module_data);

   LOC_MSG_HIGH ("loc_pa_process_ioctl, client_handle = %d, ioctl_type = %d", client_handle, ioctl_type, 0);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
   // Allow only one pa cmd being processed at any time
   if (pa_module_data_ptr->pa_cmd_is_in_progress == TRUE)
   {
      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);

      LOC_MSG_ERROR ("PA cmd is already in progress, reject the incoming request", 0, 0, 0);
      return LOC_API_ENGINE_BUSY;
   }
   else
   {
      pa_module_data_ptr->pa_cmd_is_in_progress = TRUE;
      pa_module_data_ptr->pa_client_handle      = client_handle;
      pa_module_data_ptr->pa_iotcl_type         = ioctl_type;

      if(NULL != ioctl_data)
      {
         /* Store the operation and the info required  */
         if ( LOC_IOCTL_ACCESS_EFS_DATA == ioctl_type )
         {
            pa_module_data_ptr->pa_ioctl_req_info.efs_operation = ioctl_data->efs_data.operation;
         }
         else if ( LOC_IOCTL_SET_PE_CONFIG == ioctl_type )
         {
            pa_module_data_ptr->pa_ioctl_req_info.setPeCfg = ioctl_data->z_SetPeConfigReq;
         }
         else if ( LOC_IOCTL_GET_PE_CONFIG == ioctl_type )
         {
            pa_module_data_ptr->pa_ioctl_req_info.getPeCfg = ioctl_data->z_GetPeConfigReq;
         }
      }

      os_TimerStart( pa_module_data_ptr->pa_process_timer, PA_OPER_DEFAULT_TIMEOUT,0);
      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
   }

   switch (ioctl_type)
   {
   case LOC_IOCTL_SET_ENGINE_LOCK:
      if(NULL != ioctl_data)
      {
          ret_val = loc_pa_set_engine_lock (client_handle, &(ioctl_data->engine_lock));
      }
      else
      {
          ret_val = LOC_API_INVALID_PARAMETER;
      }
      break;

   case LOC_IOCTL_GET_ENGINE_LOCK:
      ret_val = loc_pa_get_param (client_handle,
                                  PDSM_PA_GPS_LOCK);
      break;

   case LOC_IOCTL_SET_CDMA_PDE_SERVER_ADDR:
   case LOC_IOCTL_SET_CDMA_MPC_SERVER_ADDR:
   case LOC_IOCTL_SET_UMTS_SLP_SERVER_ADDR:
      if(NULL != ioctl_data)
      {
          ret_val = loc_pa_set_server_addr (client_handle, ioctl_type, &(ioctl_data->server_addr));
      }
      else
      {
          ret_val = LOC_API_INVALID_PARAMETER;
      }
      break;

   case LOC_IOCTL_GET_CDMA_PDE_SERVER_ADDR:
      ret_val = loc_pa_get_param (client_handle,
                                  PDSM_PA_1X_PDE_NETWORK_ADRS);
      break;
   case LOC_IOCTL_GET_CDMA_MPC_SERVER_ADDR:
      ret_val = loc_pa_get_param (client_handle,
                                  PDSM_PA_1X_MPC_NETWORK_ADRS);
      break;

   case LOC_IOCTL_GET_UMTS_SLP_SERVER_ADDR:
      ret_val = loc_pa_get_param (client_handle,
                                  PDSM_PA_UMTS_PDE_NETWORK_ADRS);
      break;

   case LOC_IOCTL_SET_SBAS_CONFIG:
      if(NULL != ioctl_data)
      {
          ret_val = loc_pa_set_sbas_config (client_handle, ioctl_data->sbas_mode);
      }
      else
      {
          ret_val = LOC_API_INVALID_PARAMETER;
      }
      break;

   case LOC_IOCTL_GET_SBAS_CONFIG:
      ret_val = loc_pa_get_param (client_handle,
                                  PDSM_PA_SBAS_STATUS);
      break;

   case LOC_IOCTL_SET_NMEA_TYPES:
      if(NULL != ioctl_data)
      {
          ret_val = loc_pa_set_nmea_types (client_handle, &(ioctl_data->nmea_types));
      }
      else
      {
          ret_val = LOC_API_INVALID_PARAMETER;
      }
      break;

   case LOC_IOCTL_GET_NMEA_TYPES:
      ret_val = loc_pa_get_param (client_handle,
                                  PDSM_PA_NMEA_SENTENCE_TYPE);
      break;

   case LOC_IOCTL_DELETE_ASSIST_DATA:
      if(NULL != ioctl_data)
      {
          ret_val = loc_pa_delete_aiding_data (client_handle, &(ioctl_data->assist_data_delete));
      }
      else
      {
          ret_val = LOC_API_INVALID_PARAMETER;
      }
      break;

   case LOC_IOCTL_SET_ON_DEMAND_LPM:
      if(NULL != ioctl_data)
      {
          ret_val = loc_pa_set_on_demand_lpm (client_handle, ioctl_data->on_demand_lpm);
      }
      else
      {
          ret_val = LOC_API_INVALID_PARAMETER;
      }
      break;

   case LOC_IOCTL_GET_ON_DEMAND_LPM:
      ret_val = loc_pa_get_param (client_handle,
                                  PDSM_PA_ON_DEMAND_LOW_POWER_MODE);
      break;

   case LOC_IOCTL_ACCESS_EFS_DATA:
      if(NULL != ioctl_data)
      {
          ret_val = loc_pa_access_efs_data(client_handle, &ioctl_data->efs_data);
      }
      else
      {
          ret_val = LOC_API_INVALID_PARAMETER;
      }
      break;
   case LOC_IOCTL_SET_DATA_ENABLE:    
      if(NULL != ioctl_data)
      {
          ret_val = loc_pa_set_user_control(client_handle, ioctl_data->data_enable);
      }
      else
      {
          ret_val = LOC_API_INVALID_PARAMETER;
      }
    break;

   case LOC_IOCTL_SET_LBS_APN_PROFILE:
   case LOC_IOCTL_SET_XTRA_APN_PROFILE:
      if(NULL != ioctl_data)
      {
          ret_val = loc_pa_set_apn_profiles(client_handle, ioctl_type, ioctl_data->apn_profiles);
      }
      else
      {
          ret_val = LOC_API_INVALID_PARAMETER;
      }
      break;

   case LOC_IOCTL_GET_LBS_APN_PROFILE:
     ret_val = loc_pa_get_param (client_handle,
                                 PDSM_PA_LBS_APN_PROFILE);
     break;
   case LOC_IOCTL_GET_XTRA_APN_PROFILE:
      ret_val = loc_pa_get_param (client_handle,
                                  PDSM_PA_XTRA_APN_PROFILE);
      break;

   case LOC_IOCTL_SET_SUPL_VERSION:
      if(NULL != ioctl_data)
      {
          ret_val = loc_pa_set_supl_version(client_handle, ioctl_data->supl_version);
      }
      else
      {
          ret_val = LOC_API_INVALID_PARAMETER;
      }
      break;

   case LOC_IOCTL_GET_SUPL_VERSION:
      ret_val = loc_pa_get_param (client_handle,
                                  PDSM_PA_SUPL_VERSION);
      break;

   case LOC_IOCTL_SET_XTRA_T_SESSION_CONTROL:
      if(NULL != ioctl_data)
      {
          ret_val = loc_pa_set_xtra_t_session_ctrl(client_handle, ioctl_data->xtra_t_session_control);
      }
      else
      {
          ret_val = LOC_API_INVALID_PARAMETER;
      }
      break;

   case LOC_IOCTL_GET_XTRA_T_SESSION_CONTROL:
      ret_val = loc_pa_get_param(client_handle,
                                 PDSM_PA_XTRAT_USER_SESSION_CONTROL);
      break;

  case LOC_IOCTL_SET_SUPL_SECURITY:
    if(NULL != ioctl_data)
    {
        ret_val = loc_pa_set_supl_security(client_handle, ioctl_data->supl_security);
    }
    else
    {
        ret_val = LOC_API_INVALID_PARAMETER;
    }
    break;
    
  case LOC_IOCTL_GET_SUPL_SECURITY:
    ret_val = loc_pa_get_param(client_handle,
                               PDSM_PA_SUPL_SECURITY);
    break;

  case LOC_IOCTL_SET_PE_CONFIG:
    if(NULL != ioctl_data)
    {
        ret_val = loc_pa_set_pe_config( client_handle, 
                                        &(ioctl_data->z_SetPeConfigReq) );
    }
    else
    {
        ret_val = LOC_API_INVALID_PARAMETER;
    }
    break;
    
  case LOC_IOCTL_GET_PE_CONFIG:
    if(NULL != ioctl_data)
    {
        ret_val = loc_pa_get_param(client_handle, 
                                   PDSM_PA_PE_CONFIG_PARAM );
    }
    else
    {
        ret_val = LOC_API_INVALID_PARAMETER;
    }
    break;

  case LOC_IOCTL_SET_LPP_CONFIG:
    if(NULL != ioctl_data)
    {
        ret_val = loc_pa_set_lpp_config(client_handle, &(ioctl_data->lpp_config));
    }
    else
    {
        ret_val = LOC_API_INVALID_PARAMETER;
    }
    break;

  case LOC_IOCTL_GET_LPP_CONFIG:
    ret_val = loc_pa_get_param(client_handle,
                               PDSM_PA_LPP_CONFIG_INFO);
    break;

  case LOC_IOCTL_SET_ASSISTED_GLO_CONFIG:
    if(NULL != ioctl_data)
    {
        ret_val = loc_pa_set_assisted_glo_config(
          client_handle, ioctl_data->assisted_glo_config);
    }
    else
    {
        ret_val = LOC_API_INVALID_PARAMETER;
    }
    break;

  case LOC_IOCTL_GET_ASSISTED_GLO_CONFIG:
    ret_val = loc_pa_get_param(client_handle,
                               PDSM_PA_ASST_GLONASS_POS_PROTOCOL);
    break;

  case LOC_IOCTL_SET_EMERGENCY_PROTOCOL:
    if(NULL != ioctl_data)
    {
        ret_val = loc_pa_set_emergency_protocol(
          client_handle, ioctl_data->emergency_protocol);
    }
    else
    {
        ret_val = LOC_API_INVALID_PARAMETER;
    }
    break;

  case LOC_IOCTL_GET_EMERGENCY_PROTOCOL:
    ret_val = loc_pa_get_param(client_handle,
                               PDSM_PA_GNSS_EMERGENCY_PROTOCOL_CONFIG);
    break;


  case LOC_IOCTL_SET_SUPL_HASH_ALGO:
    if(NULL != ioctl_data)
    {
      ret_val = loc_pa_set_supl_hash_algo(
        client_handle, ioctl_data->supl_hash_algo); 
    }
    else
    {
      ret_val = LOC_API_INVALID_PARAMETER;
    }
    break;

  case LOC_IOCTL_GET_SUPL_HASH_ALGO:
    ret_val = loc_pa_get_param(client_handle,
                               PDSM_PA_SUPL_HASH_ALGORITHM); 
    break;

  case LOC_IOCTL_SET_SUPL_TLS_VERSION:
    if(NULL != ioctl_data)
    {
      ret_val = loc_pa_set_supl_tls_version(
        client_handle, ioctl_data->supl_tls_version); 
    }
    else
    {
      ret_val = LOC_API_INVALID_PARAMETER;
    }
    break;

  case LOC_IOCTL_GET_SUPL_TLS_VERSION:
    ret_val = loc_pa_get_param(client_handle,
                               PDSM_PA_SUPL_TLS_VERSION); 
    break;

   case LOC_IOCTL_SET_WIFI_SCAN_INJECT_TIMEOUT:
    if(NULL != ioctl_data)
    {
        ret_val = loc_pa_set_wifi_scan_inject_timeout(client_handle,
                                                      ioctl_data->wifi_scan_inject_timeout);
    }
    else
    {
        ret_val = LOC_API_INVALID_PARAMETER;
    }
    break;

  case LOC_IOCTL_GET_WIFI_SCAN_INJECT_TIMEOUT:
    ret_val = loc_pa_get_param(client_handle,
                               PDSM_PA_WIFI_SCAN_TIMEOUT_SEC);
    break;

  case LOC_IOCTL_SET_PREMIUM_SERVICES_CONFIG:
    if (NULL != ioctl_data)
    {
      ret_val = loc_pa_set_premium_svcs_cfg(client_handle, &(ioctl_data->z_SetPremSvcsCfgReq));
    }
    else
    {
      ret_val = LOC_API_INVALID_PARAMETER;
    }
    break;

  case LOC_IOCTL_SET_XTRA_VERSION_CHECK:
    if (NULL != ioctl_data)
    {
      ret_val = loc_pa_set_xtra_version_check(client_handle,
                                              ioctl_data->z_XtraVersion);
    }
    else
    {
      ret_val = LOC_API_INVALID_PARAMETER;
    }
    break;

 case LOC_IOCTL_SET_GNSS_CONSTELL_REPORT_CONFIG:
  	  LOC_MSG_MED("In loc_pa_process_ioctl handling LOC_IOCTL_SET_GNSS_CONSTELL_REPORT_CONFIG",0,0,0);
      if(NULL != ioctl_data)
      {
          ret_val = loc_pa_set_gnss_constell_report_config (client_handle, &(ioctl_data->z_GNSS_config));
      }
      else
      {
          ret_val = LOC_API_INVALID_PARAMETER;
      }
      break;

  case LOC_IOCTL_DELETE_GNSS_SERVICE_DATA:
    if(NULL != ioctl_data)
    {
        ret_val = loc_pa_delete_gnss_service_data(client_handle, &(ioctl_data->z_DelGNSSSvcDataReq));
    }
    else
    {
        ret_val = LOC_API_INVALID_PARAMETER;
    }
    break;

   default:
      ret_val = LOC_API_UNSUPPORTED;
      break;
   }

   if (ret_val != LOC_API_SUCCESS)
   {
      LOC_MSG_ERROR("loc_pa_process_ioctl : error %d while processing ioctl :%d",
                    ret_val, ioctl_type, 0);
      loc_pa_reset_module ();
   }

   return ret_val;
}

// Location PA event cb
static void loc_pa_event_cb
(
      void                       *user_data,   /* user data pointer */
      pdsm_pa_event_type          pa_event,    /* Indicate PA event */
      const pdsm_pa_info_s_type  *pa_info_ptr  /* PARAM pa_info_ptr POINTER */
)
{
   int                            ret_val = LOC_API_GENERAL_FAILURE;
   loc_pa_module_data_s_type*     pa_module_data_ptr = NULL;
   loc_ioctl_callback_data_u_type ioctl_callback_data = {0};
   boolean                        notify_client = FALSE;

   if( NULL == pa_info_ptr)
   {
      LOC_MSG_ERROR ("loc_pa_event_cb: NULL pa_info_ptr", 0, 0, 0);
      return;
   }

   pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
   LOC_MSG_HIGH ("loc_pa_event_cb, user_data=%d, pa_cmd=%d, pa_event=0x%x\n",
                  user_data, pa_module_data_ptr->pa_cmd, pa_event);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
   if (pa_module_data_ptr->pa_cmd_is_in_progress == FALSE)
   {
      notify_client = FALSE;
      ret_val = LOC_API_GENERAL_FAILURE;
      if (pa_event == PDSM_PA_EVENT_MO_METHOD)
      {
         pa_module_data_ptr->mo_method = pa_info_ptr->pa_info.mo_method;
         LOC_MSG_MED ("loc_pa_event_cb: mo method is = %d\n", pa_event, 0, 0);
      }
      LOC_MSG_ERROR ("loc_pa_event_cb: event received when nobody is waiting for the event = %d\n", pa_event, 0, 0);
   }
   else
   {
      // For setting GPS parameters (nv items), return IOCTL status here
      if (pa_module_data_ptr->pa_cmd_type == PDSM_PA_CMD_SET_PARAM)
      {
        
        ret_val = LOC_API_SUCCESS;

        /* Fill up information required by clients on some commands */
        if((PDSM_PA_EVENT_EFS_DATA == pa_event) && (PDSM_PA_EFS_DATA == pa_module_data_ptr->pa_cmd))
        {
           // note: this is just an event marking the completion of the command. 
           //we do not get the Ok/FAILURE result from PDAPI
           ioctl_callback_data.z_AccessEfsData.efs_operation = pa_module_data_ptr->pa_ioctl_req_info.efs_operation;
           ret_val = LOC_API_SUCCESS;
        }
        else if((PDSM_PA_EVENT_PE_CONFIG_PARAM == pa_event) && (PDSM_PA_PE_CONFIG_PARAM == pa_module_data_ptr->pa_cmd))
        {
          /* If a bit mask is set in the PDAPI structure, it means that the 
             parameter was successfully set */
          if ( pa_module_data_ptr->pa_ioctl_req_info.setPeCfg.injectedPositionControl_valid)
          {
            ioctl_callback_data.z_SetPeConfigInd.failedPositionEngineConfigParamMask |=
                               ( pa_info_ptr->pa_info.pe_config_info.peConfigMask & 
                                 PDSM_PA_PE_CONFIG_PARAM_MASK_USE_INJECTED_POSITION )?
                                 0:QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_INJECTED_POSITION_CONTROL_V02;
          }
          if ( pa_module_data_ptr->pa_ioctl_req_info.setPeCfg.filterSvUsage_valid )
          {
            ioctl_callback_data.z_SetPeConfigInd.failedPositionEngineConfigParamMask |=
                               ( pa_info_ptr->pa_info.pe_config_info.peConfigMask & 
                                 PDSM_PA_PE_CONFIG_PARAM_MASK_FILTER_SV_USED )?
                                 0:QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_FILTER_SV_USAGE_V02;
          }
          if ( pa_module_data_ptr->pa_ioctl_req_info.setPeCfg.storeAssistData_valid )
          {
            ioctl_callback_data.z_SetPeConfigInd.failedPositionEngineConfigParamMask |=
                               ( pa_info_ptr->pa_info.pe_config_info.peConfigMask & 
                                 PDSM_PA_PE_CONFIG_PARAM_MASK_STORE_ASSIST_DATA )?
                                 0:QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_STORE_ASSIST_DATA_V02;
          }
          /* If there is atleast one mask that failed, then the set did not 
             succeed */
          if ( 0 != ioctl_callback_data.z_SetPeConfigInd.failedPositionEngineConfigParamMask )
          {
            ioctl_callback_data.z_SetPeConfigInd.failedPositionEngineConfigParamMask_valid = TRUE;
            ret_val = LOC_API_GENERAL_FAILURE;
          }
          else
          {
            ret_val = LOC_API_SUCCESS;
          }
        }
        
        loc_client_queue_ioctl_callback (pa_module_data_ptr->pa_client_handle,
                                         pa_module_data_ptr->pa_iotcl_type,
                                         ret_val,
                                         &ioctl_callback_data);
        notify_client = TRUE;
      }
      // For retrieving GPS parameter (nv items), return the needed information
      else
      {
         memset (&ioctl_callback_data, 0, sizeof(ioctl_callback_data));
         
         // Note: ret_val need to be set to LOC_API_SUCCESS for each of the valid cases
         if ((PDSM_PA_EVENT_GPS_LOCK == pa_event) && (PDSM_PA_GPS_LOCK == pa_module_data_ptr->pa_cmd))
         {
            ret_val = loc_pa_xlate_lock_from_pdapi_to_locapi ( &(pa_info_ptr->pa_info.gps_lock), &(ioctl_callback_data.engine_lock ));
            notify_client = TRUE;
         }
         else if (((PDSM_PA_EVENT_1X_PDE_NETWORK_ADDRESS == pa_event)   && (PDSM_PA_1X_PDE_NETWORK_ADRS == pa_module_data_ptr->pa_cmd)) ||
                  ((PDSM_PA_EVENT_1X_MPC_NETWORK_ADDRESS == pa_event)   && (PDSM_PA_1X_MPC_NETWORK_ADRS == pa_module_data_ptr->pa_cmd)) ||
                  ((PDSM_PA_EVENT_UMTS_PDE_NETWORK_ADDRESS == pa_event) && (PDSM_PA_UMTS_PDE_NETWORK_ADRS == pa_module_data_ptr->pa_cmd)))
         {
            ret_val = loc_pa_xlate_server_addr_from_pdapi_to_locapi (&(pa_info_ptr->pa_info.server_address), &(ioctl_callback_data.server_addr));
            notify_client = TRUE;
         }
         else if ((PDSM_PA_EVENT_SBAS_STATUS == pa_event) && (PDSM_PA_SBAS_STATUS == pa_module_data_ptr->pa_cmd))
         {
            ret_val = loc_pa_xlate_sbas_status_from_pdapi_to_locapi (&(pa_info_ptr->pa_info.sbas_status), &(ioctl_callback_data.sbas_mode));
            notify_client = TRUE;
         }
         else if ((PDSM_PA_EVENT_NMEA_SENTENCE_TYPE == pa_event) && (PDSM_PA_NMEA_SENTENCE_TYPE == pa_module_data_ptr->pa_cmd))
         {
            ret_val = loc_pa_xlate_nmea_types_from_pdapi_to_locapi (&(pa_info_ptr->pa_info.nmea_sentence_type), &(ioctl_callback_data.nmea_types));
            notify_client = TRUE;
         }
         else if ((PDSM_PA_EVENT_ON_DEMAND_LPM == pa_event) && (PDSM_PA_ON_DEMAND_LOW_POWER_MODE == pa_module_data_ptr->pa_cmd))
         {
            ioctl_callback_data.on_demand_lpm = pa_info_ptr->pa_info.on_demand_lpm;
            notify_client = TRUE;
            ret_val = LOC_API_SUCCESS;
         }
         else if (((PDSM_PA_EVENT_LBS_APN_PROFILE_SELECT == pa_event)   && (PDSM_PA_LBS_APN_PROFILE == pa_module_data_ptr->pa_cmd)) ||
                  ((PDSM_PA_EVENT_XTRA_APN_PROFILE_SELECT == pa_event)   && (PDSM_PA_XTRA_APN_PROFILE == pa_module_data_ptr->pa_cmd)) )
         {
            memscpy((void*)&ioctl_callback_data.apn_profiles[0], sizeof(ioctl_callback_data.apn_profiles),
				    (void*)&pa_info_ptr->pa_info.apn_profiles[0], sizeof(pa_info_ptr->pa_info.apn_profiles));
            notify_client = TRUE;
            ret_val = LOC_API_SUCCESS;
         }         
         else if ((PDSM_PA_EVENT_SUPL_VERSION == pa_event) && (PDSM_PA_SUPL_VERSION == pa_module_data_ptr->pa_cmd))
         {
            ioctl_callback_data.supl_version = pa_info_ptr->pa_info.supl_version;
            notify_client = TRUE;
            ret_val = LOC_API_SUCCESS;
         }
         else if((PDSM_PA_EVENT_EFS_DATA == pa_event) && (PDSM_PA_EFS_DATA == pa_module_data_ptr->pa_cmd))
         {
            // note: this is just an event marking the completion of the command. we do not get the Ok/FAILURE result from PDAPI
            notify_client = TRUE;
            ioctl_callback_data.z_AccessEfsData.efs_operation = pa_module_data_ptr->pa_ioctl_req_info.efs_operation;
            ret_val = LOC_API_SUCCESS;
         }
         else if ((PDSM_PA_EVENT_XTRAT_SESSION_CONTROL == pa_event) && (PDSM_PA_XTRAT_USER_SESSION_CONTROL == pa_module_data_ptr->pa_cmd))
         {
            ioctl_callback_data.xtra_t_session_control = pa_info_ptr->pa_info.session_control;
            notify_client = TRUE;
            ret_val = LOC_API_SUCCESS;
         }
         else if ( ( PDSM_PA_EVENT_SUPL_SECURITY == pa_event ) && ( PDSM_PA_SUPL_SECURITY == pa_module_data_ptr->pa_cmd ) )
         {
           ioctl_callback_data.supl_security = pa_info_ptr->pa_info.supl_security;
           notify_client = TRUE;
           ret_val = LOC_API_SUCCESS;
         }
         else if((PDSM_PA_EVENT_PE_CONFIG_PARAM == pa_event) && (PDSM_PA_PE_CONFIG_PARAM == pa_module_data_ptr->pa_cmd))
         {
           qmiLocPositionEngineConfigParamMaskT_v02 recvdMask = 0;
           if ( pa_info_ptr->pa_info.pe_config_info.peConfigMask & 
                                 PDSM_PA_PE_CONFIG_PARAM_MASK_USE_INJECTED_POSITION ) 
           {
            ioctl_callback_data.z_GetPeConfigInd.injectedPositionControl_valid= TRUE;
            ioctl_callback_data.z_GetPeConfigInd.injectedPositionControl=
                    pa_info_ptr->pa_info.pe_config_info.useInjectedPosition;
            recvdMask |= QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_INJECTED_POSITION_CONTROL_V02;
           }

          if ( pa_info_ptr->pa_info.pe_config_info.peConfigMask & 
                                PDSM_PA_PE_CONFIG_PARAM_MASK_FILTER_SV_USED ) 
          {
           ioctl_callback_data.z_GetPeConfigInd.filterSvUsage_valid= TRUE;
           ioctl_callback_data.z_GetPeConfigInd.filterSvUsage =
                   pa_info_ptr->pa_info.pe_config_info.filterSvUsed;
            recvdMask |= QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_FILTER_SV_USAGE_V02;
          }

         
          if ( pa_info_ptr->pa_info.pe_config_info.peConfigMask & 
                                PDSM_PA_PE_CONFIG_PARAM_MASK_STORE_ASSIST_DATA ) 
          {
           ioctl_callback_data.z_GetPeConfigInd.storeAssistData_valid = TRUE;
           ioctl_callback_data.z_GetPeConfigInd.storeAssistData =
                   pa_info_ptr->pa_info.pe_config_info.storeAssistData;
           recvdMask |= QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_STORE_ASSIST_DATA_V02;
          }

          /* If the info recvd is not equal to what was requested, notify a failure
             else notify a success to the client */
          if ( recvdMask != 
                 pa_module_data_ptr->pa_ioctl_req_info.getPeCfg.getPositionEngineConfigParamMask )
          {
            ret_val = LOC_API_GENERAL_FAILURE;            
          }
          else
          {
            ret_val = LOC_API_SUCCESS;
          }
          
          notify_client = TRUE;

         }
         
         /** handle Get LPP config PA event */
         else if((PDSM_PA_EVENT_LPP_CONFIG_TYPE == pa_event) && 
                (PDSM_PA_LPP_CONFIG_INFO == pa_module_data_ptr->pa_cmd))
         {
           if( pa_info_ptr->pa_info.lpp_config_info & 
               QMI_LOC_LPP_CONFIG_ENABLE_USER_PLANE_V02)
           {
             ioctl_callback_data.lpp_config |= PDSM_PA_LPP_IN_LTE_UP_ENABLE;
           }
           if( pa_info_ptr->pa_info.lpp_config_info & 
               QMI_LOC_LPP_CONFIG_ENABLE_CONTROL_PLANE_V02)
           {
             ioctl_callback_data.lpp_config |= PDSM_PA_LPP_IN_LTE_CP_ENABLE;
           }
           ret_val = LOC_API_SUCCESS;
           notify_client = TRUE;
         }

         else
         {
            notify_client = FALSE;
            ret_val = LOC_API_GENERAL_FAILURE;
            LOC_MSG_ERROR ("loc_pa_event_cb: error, unrecognizable event = %d, for cmd = %d\n", pa_event, pa_module_data_ptr->pa_cmd, 0);
         }

         if (notify_client == TRUE)
         {
            loc_client_queue_ioctl_callback (pa_module_data_ptr->pa_client_handle,
                                             pa_module_data_ptr->pa_iotcl_type,
                                             ret_val,
                                             &ioctl_callback_data);
         }
      }
   }

   // Client has been notified of the pa processing status, reset the pa module
   if ( notify_client == TRUE )
   {
      loc_pa_reset_module ();
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
}

/**
 * @brief converts PDAPI Assisted Glonass Mask to QMI_LOC format 
 * @param [in] :  pdapiAssistedGlonassMask 
 * @retval : QMI LOC Assisted Glonass Mask 
 */

static qmiLocAssistedGlonassProtocolMaskT_v02 loc_pa_conv_assisted_glo_to_qmi(
  uint32 pdapiAssistedGlonassMask )
{
  int32 i;
  qmiLocAssistedGlonassProtocolMaskT_v02 qmiLocAssistedGlonassMask = 0;
  /** Not checkign parameter here as this funtiona is called by
   *  inernal modules with valid pointers */
  for( i=0;
       i<(sizeof(loc_pa_assisted_glo_map)/sizeof(loc_pa_assisted_glo_map[0]));
       i++ )
  {
    if(pdapiAssistedGlonassMask & loc_pa_assisted_glo_map[i].pdapiAssistedGlonassMask )
    {
      qmiLocAssistedGlonassMask |= loc_pa_assisted_glo_map[i].qmiLocAssistedGlonassMask;
    }
  }

  return qmiLocAssistedGlonassMask;
  
}

static void loc_pa_event_cb_ext
(
  /* user data pointer */
  void *user_data_ptr,  
  /* Indicate PDSM param type, PDSM_PA_xxx enum */
  pdsm_pa_e_type  pa_type, 
  /* A pointer to a PA state information struct */   
  const pdsm_pa_info_s_type    *pa_info_ptr, 
  /* Optional field to indicate success/failure status of PA operation */     
  pdsm_pa_cmd_err_e_type        status    
)
{
  loc_pa_module_data_s_type*     pa_module_data_ptr;

  loc_ioctl_callback_data_u_type* ioctl_callback_data_ptr = NULL;
  
  int32 ioctl_status = LOC_API_GENERAL_FAILURE;

  /* determines if the IOCTL report should be generated */
  boolean notify_client = FALSE;

  pa_module_data_ptr = &(loc_middleware_data.pa_module_data);

  LOC_MSG_MED ("loc_pa_event_cb_ext:user_data_ptr (client handle)= %d, pa_type =%d status = %d", 
                user_data_ptr, pa_type, status);
  
  /* incurring technical debt : there are no state checks made after
     the IOCTL report IPC is sent, so the state machine checks are made
     in the pdsm callbacks */

  LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
  
  do
  {
    /* check  for errors*/
    /* Note: There is no way to detect GET_PARAM and SET_PARAM mismatch,
             also unsure if client id in pa_info is same as pdapi client id */
     
    if ( (FALSE == pa_module_data_ptr->pa_cmd_is_in_progress ) ||
         (pa_module_data_ptr->pa_cmd != pa_type)||
         ( NULL == pa_info_ptr) )
    {
      LOC_MSG_ERROR ("loc_pa_event_cb_ext: Error pa_cmd_in_progress = %u, "
                     "pa_info_ptr = 0x%x, pa_type is as expected = %u", 
                     pa_module_data_ptr->pa_cmd_is_in_progress, 
                     pa_info_ptr, 
                     (pa_module_data_ptr->pa_cmd == pa_type));
      break;
    }  
    
    // ioctl callback data pointer can be quite large so assigning on heap
    ioctl_callback_data_ptr = loc_calloc(sizeof(*ioctl_callback_data_ptr));

    if ( NULL == ioctl_callback_data_ptr)
    {
      LOC_MSG_ERROR ("loc_pa_event_cb_ext: could not allocate memory for ioctl callback"
                     "for pa type = %u",  pa_type, 0,0);
      break;
    }


    switch (pa_type)
    {
    case PDSM_PA_ASST_GLONASS_POS_PROTOCOL:
      {
        if(PDSM_PA_CMD_GET_PARAM == pa_module_data_ptr->pa_cmd_type)
        {
          /* get the assisted Glo config*/
          ioctl_callback_data_ptr->assisted_glo_config = 
            loc_pa_conv_assisted_glo_to_qmi(pa_info_ptr->pa_info.asst_glo_protocol);
          
          LOC_MSG_MED("loc_pa_event_cb_ext: assisted glo config pdsm =%u, loc = %u ",
                       pa_info_ptr->pa_info.asst_glo_protocol,
                       ioctl_callback_data_ptr->assisted_glo_config,0);
          ioctl_status = LOC_API_SUCCESS;
          notify_client = TRUE;
        }
  
        else if(PDSM_PA_CMD_SET_PARAM == pa_module_data_ptr->pa_cmd_type)
        {
          ioctl_status = LOC_API_SUCCESS;
          notify_client = TRUE;
        } 
        else
        {
          /* should never happen */
          LOC_MSG_ERROR ("loc_pa_event_cb_ext: unexpected pa_cmd_type "
                           "set %d", pa_module_data_ptr->pa_cmd_type, 0,0);
        }
      }
      break;

    case PDSM_PA_GNSS_EMERGENCY_PROTOCOL_CONFIG:
      {
        /*setting notify_client to true by default, in case
        of a translation error it will be set to FALSE */
        notify_client = TRUE;

        if(PDSM_PA_CMD_GET_PARAM == pa_module_data_ptr->pa_cmd_type)
        {
          switch (pa_info_ptr->pa_info.gnss_emergency_protocol_select)
          {
          case PDSM_PA_EMERGENCY_PROTOCOL_CONFIG_WCMDA_CP:
            ioctl_callback_data_ptr->emergency_protocol = 
              eQMI_LOC_EMERGENCY_PROTOCOL_WCDMA_CP_V02;
            break;

          case PDSM_PA_EMERGENCY_PROTOCOL_CONFIG_WCMDA_UP:
            ioctl_callback_data_ptr->emergency_protocol = 
              eQMI_LOC_EMERGENCY_PROTOCOL_WCDMA_UP_V02;
            break;

          default:
            LOC_MSG_ERROR ("loc_pa_event_cb_ext: unknown pdsm emergency protocol %d",
                           pa_info_ptr->pa_info.gnss_emergency_protocol_select,0,0);
            notify_client = FALSE;
            break;
          }
 
          ioctl_status = LOC_API_SUCCESS;
          
        }
  
        else if(PDSM_PA_CMD_SET_PARAM == pa_module_data_ptr->pa_cmd_type)
        {
          ioctl_status = LOC_API_SUCCESS;
          notify_client = TRUE;
        } 
        else
        {
          /* should never happen */
          LOC_MSG_ERROR ("loc_pa_event_cb_ext: unexpected pa_cmd_type "
                           "set %d", pa_module_data_ptr->pa_cmd_type, 0,0);
        }
      }
      break;

    case PDSM_PA_SUPL_HASH_ALGORITHM:
      {
        if(PDSM_PA_CMD_GET_PARAM == pa_module_data_ptr->pa_cmd_type)
        {
          switch (pa_info_ptr->pa_info.supl_hash_algorithm)
          {
          case PDSM_PA_SUPL_HASH_ALGO_SHA1:
            {
              ioctl_callback_data_ptr->supl_hash_algo = eQMI_LOC_SUPL_HASH_ALGO_SHA1_V02;
              ioctl_status = LOC_API_SUCCESS;
              notify_client = TRUE;
              break;
            }

          case PDSM_PA_SUPL_HASH_ALGO_SHA256:
            {
              ioctl_callback_data_ptr->supl_hash_algo = eQMI_LOC_SUPL_HASH_ALGO_SHA256_V02;
              ioctl_status = LOC_API_SUCCESS;
              notify_client = TRUE;
              break;
            }

          default:
            {
              // do not generate an indication since HASH algo is unknown
              LOC_MSG_ERROR("loc_pa_event_cb_ext: unknown hash algo %d, dropping callback",
                            pa_info_ptr->pa_info.supl_hash_algorithm, 0, 0);
              break;

            }
          }
          
          LOC_MSG_MED("loc_pa_event_cb_ext: hash algo pdsm =%u, loc = %u ",
                       pa_info_ptr->pa_info.supl_hash_algorithm,
                       ioctl_callback_data_ptr->supl_hash_algo,0);
          
        }
  
        else if(PDSM_PA_CMD_SET_PARAM == pa_module_data_ptr->pa_cmd_type)
        {
          ioctl_status = LOC_API_SUCCESS;
          notify_client = TRUE;
        } 
        else
        {
          /* should never happen */
          LOC_MSG_ERROR ("loc_pa_event_cb_ext: unexpected pa_cmd_type "
                           "set %d", pa_module_data_ptr->pa_cmd_type, 0,0);
        }
      }
      break;

    case PDSM_PA_SUPL_TLS_VERSION:
      {
        if(PDSM_PA_CMD_GET_PARAM == pa_module_data_ptr->pa_cmd_type)
        {
          switch (pa_info_ptr->pa_info.supl_tls_version)
          {
          case PDSM_PA_SUPL_TLS_VERSION_1_0:
            {
              ioctl_callback_data_ptr->supl_tls_version = eQMI_LOC_SUPL_TLS_VERSION_1_0_V02;
              ioctl_status = LOC_API_SUCCESS;
              notify_client = TRUE;
              break;
            }

          case PDSM_PA_SUPL_TLS_VERSION_1_1:
            {
              ioctl_callback_data_ptr->supl_tls_version = eQMI_LOC_SUPL_TLS_VERSION_1_1_V02;
              ioctl_status = LOC_API_SUCCESS;
              notify_client = TRUE;
              break;
            }

          default:
            {
              // do not generate an indication since HASH algo is unknown
              LOC_MSG_ERROR("loc_pa_event_cb_ext: unknown TLS version %d, dropping callback",
                            pa_info_ptr->pa_info.supl_tls_version, 0, 0);
              break;

            }
          }
          
          LOC_MSG_MED("loc_pa_event_cb_ext: TLS version pdsm =%u, loc = %u ",
                       pa_info_ptr->pa_info.supl_tls_version,
                       ioctl_callback_data_ptr->supl_tls_version,0);
          
        }
  
        else if(PDSM_PA_CMD_SET_PARAM == pa_module_data_ptr->pa_cmd_type)
        {
          ioctl_status = LOC_API_SUCCESS;
          notify_client = TRUE;
        } 
        else
        {
          /* should never happen */
          LOC_MSG_ERROR ("loc_pa_event_cb_ext: unexpected pa_cmd_type "
                           "set %d", pa_module_data_ptr->pa_cmd_type, 0,0);
        }
      }
      break;

    case PDSM_PA_WIFI_SCAN_TIMEOUT_SEC:
      {
        if(PDSM_PA_CMD_GET_PARAM == pa_module_data_ptr->pa_cmd_type)
        {
          /* get the wifi scan inject timeout */
          ioctl_callback_data_ptr->wifi_scan_inject_timeout = pa_info_ptr->pa_info.wifi_wait_timeout_sec;
          
          LOC_MSG_MED("loc_pa_event_cb_ext: wifi scan inject timeout -GET- pdsm =%u, loc = %u ",
                       pa_info_ptr->pa_info.wifi_wait_timeout_sec,
                       ioctl_callback_data_ptr->wifi_scan_inject_timeout,0);

          // always success? failure is notified only by OS-Timer expiry?
          ioctl_status = LOC_API_SUCCESS;
          notify_client = TRUE;
        }
  
        else if(PDSM_PA_CMD_SET_PARAM == pa_module_data_ptr->pa_cmd_type)
        {
          // always success? failure is notified only by OS-Timer expiry?
          ioctl_status = LOC_API_SUCCESS;
          notify_client = TRUE;
        } 
        else
        {
          /* should never happen */
          LOC_MSG_ERROR ("loc_pa_event_cb_ext: unexpected pa_cmd_type "
                           "set %d", pa_module_data_ptr->pa_cmd_type, 0,0);
        }
      }
      break;

    case PDSM_PA_PREMIUM_SERVICES_CONFIG:
    {
      if (PDSM_PA_CMD_SET_PARAM == pa_module_data_ptr->pa_cmd_type)
      {
        if (PDSM_PA_CMD_ERR_NOERR == status)
        {
          ioctl_status = LOC_API_SUCCESS;
        }
        else
        {
          ioctl_status = LOC_API_GENERAL_FAILURE;
        }
        notify_client = TRUE;
      } 

      else
      {
        /* should never happen */
        LOC_MSG_ERROR ("loc_pa_event_cb_ext: unexpected pa_cmd_type "
                       "set %d", pa_module_data_ptr->pa_cmd_type, 0, 0);
      }
    }
    break;

    case PDSM_PA_EFS_DATA:
      {
        if(PDSM_PA_CMD_GET_PARAM == pa_module_data_ptr->pa_cmd_type)
        {
          ioctl_callback_data_ptr->z_AccessEfsData.efs_operation = pa_module_data_ptr->pa_ioctl_req_info.efs_operation;
          if(status == PDSM_PA_CMD_ERR_NOERR)
            ioctl_status = LOC_API_SUCCESS;
          else
            ioctl_status = LOC_API_GENERAL_FAILURE;
          notify_client = TRUE;
          
        }
  
        else if(PDSM_PA_CMD_SET_PARAM == pa_module_data_ptr->pa_cmd_type)
        {

          ioctl_callback_data_ptr->z_AccessEfsData.efs_operation = pa_module_data_ptr->pa_ioctl_req_info.efs_operation;
          if(status == PDSM_PA_CMD_ERR_NOERR)
            ioctl_status = LOC_API_SUCCESS;
          else
            ioctl_status = LOC_API_GENERAL_FAILURE;
          notify_client = TRUE;
        } 
        else
        {
          /* should never happen */
          LOC_MSG_ERROR ("loc_pa_event_cb_ext: unexpected pa_cmd_type "
                           "set %d", pa_module_data_ptr->pa_cmd_type, 0,0);
        }
      }
      break;
	 case PDSM_GNSS_CONSTELL_REPORT_CONFIG:
		 if(PDSM_PA_CMD_SET_PARAM == pa_module_data_ptr->pa_cmd_type)
		 {
		 
		   if(status == PDSM_PA_CMD_ERR_NOERR)
			 ioctl_status = LOC_API_SUCCESS;
		   else
			 ioctl_status = LOC_API_GENERAL_FAILURE;
		   notify_client = TRUE;
		 } 
	break;	 	

    case PDSM_PA_XTRA_VERSION_CHECK:
    {
      if (PDSM_PA_CMD_SET_PARAM == pa_module_data_ptr->pa_cmd_type)
      {
        LOC_MSG_MED ("loc_pa_event_cb_ext: PDSM_PA_XTRA_VERSION_CHECK status=%d ", status, 0, 0);
        /* Status is always PDSM_PA_CMD_ERR_NOERR, sent by pdsm_client_pa_event_ntfy,
           but still do status check */
        if (PDSM_PA_CMD_ERR_NOERR == status) 
        {
          ioctl_status = LOC_API_SUCCESS;
        }
        else
        {
          ioctl_status = LOC_API_GENERAL_FAILURE;
        }
        notify_client = TRUE;
      } 
      else
      {
        /* should never happen */
        LOC_MSG_ERROR ("loc_pa_event_cb_ext: unexpected pa_cmd_type "
                       "set %d", pa_module_data_ptr->pa_cmd_type, 0, 0);
      }
    }
    break;

    default:
      {
        LOC_MSG_ERROR("loc_pa_event_cb_ext: unknown pa type %d", pa_type, 0,0);
      }
      break;

    } // end switch(pa_type)

    
  }while(0);

    /* queue the IOCTL report */
    if(TRUE == notify_client)
    {
      loc_client_queue_ioctl_callback (pa_module_data_ptr->pa_client_handle,
                                       pa_module_data_ptr->pa_iotcl_type,
                                       ioctl_status,
                                       ioctl_callback_data_ptr);
    }
    
  // free up the callback data pointer
  if ( NULL != ioctl_callback_data_ptr)
  {
    loc_free(ioctl_callback_data_ptr);
  }

  LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);

  /* Client has been notified of the pa processing status, reset the pa module*/

  if ( TRUE == notify_client )
  {
    loc_pa_reset_module ();
  }

  return;
}/* loc_pa_event_cb_ext */
    
// PA cmd callback
static void loc_pa_cmd_cb
(
      void                      *data_block_ptr,
      pdsm_pa_cmd_e_type        pa_cmd_type,
      pdsm_pa_cmd_err_e_type    pa_cmd_err
)
{
   loc_pa_module_data_s_type* pa_module_data_ptr;
   int32                      ioctl_status;
   loc_ioctl_callback_data_u_type ioctl_callback_data;

   LOC_MSG_HIGH ("loc_pa_cmd_cb, client_handle=%d, pa_md=%d, cmd_error=%d\n",
                  data_block_ptr, pa_cmd_type, pa_cmd_err);

   pa_module_data_ptr = &(loc_middleware_data.pa_module_data);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
   if ((pa_module_data_ptr->pa_cmd_is_in_progress == FALSE)  ||
       (pa_module_data_ptr->pa_cmd_type != pa_cmd_type))
   {
      LOC_MSG_ERROR ("loc_pa_cmd_cb, pd_cmd not in progress or cmd mismatch, in process = %d, received cmd=%d, issued command =%d\n",
                     pa_module_data_ptr->pa_cmd_is_in_progress, pa_cmd_type, pa_module_data_ptr->pa_cmd_type);
   }
   else
   {
     memset ( &ioctl_callback_data, 0, sizeof(ioctl_callback_data) );

     if ( ( PDSM_PA_CMD_SET_PARAM == pa_module_data_ptr->pa_cmd_type ) && 
          ( PDSM_PA_PE_CONFIG_PARAM == pa_module_data_ptr->pa_cmd ) )
     {
       /* Command Callback failed, all the parameters failed */
       ioctl_callback_data.z_SetPeConfigInd.failedPositionEngineConfigParamMask |=
                            ( pa_module_data_ptr->pa_ioctl_req_info.setPeCfg.injectedPositionControl_valid)?
                            QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_INJECTED_POSITION_CONTROL_V02:0;
       ioctl_callback_data.z_SetPeConfigInd.failedPositionEngineConfigParamMask |=
                          ( pa_module_data_ptr->pa_ioctl_req_info.setPeCfg.filterSvUsage_valid)?
                          QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_FILTER_SV_USAGE_V02:0;
       ioctl_callback_data.z_SetPeConfigInd.failedPositionEngineConfigParamMask |=
                          ( pa_module_data_ptr->pa_ioctl_req_info.setPeCfg.storeAssistData_valid )?
                          QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_STORE_ASSIST_DATA_V02:0;
       ioctl_callback_data.z_SetPeConfigInd.failedPositionEngineConfigParamMask_valid =
         (0 != ioctl_callback_data.z_SetPeConfigInd.failedPositionEngineConfigParamMask)?
         TRUE : FALSE ;
     } 
     else if ( ( PDSM_PA_CMD_GET_PARAM == pa_module_data_ptr->pa_cmd_type ) && 
            ( PDSM_PA_PE_CONFIG_PARAM == pa_module_data_ptr->pa_cmd ) )
     {
       /* Command Callback failed, all the parameters failed, mark all as false */
       ioctl_callback_data.z_GetPeConfigInd.injectedPositionControl_valid= FALSE;
       ioctl_callback_data.z_GetPeConfigInd.filterSvUsage_valid= FALSE;
       ioctl_callback_data.z_GetPeConfigInd.storeAssistData_valid= FALSE;
     }      
     else if ( ( PDSM_PA_CMD_SET_PARAM == pa_module_data_ptr->pa_cmd_type ) && 
               ( PDSM_PA_EFS_DATA == pa_module_data_ptr->pa_cmd ) )
     {
       /* Command Callback failed, all the parameters failed, mark all as false */
       ioctl_callback_data.z_AccessEfsData.efs_operation = pa_module_data_ptr->pa_ioctl_req_info.efs_operation;
     }

     // Deliver the callback if pa cmd fails
     // For Data Enable/Disable, there is no event callback, so deliver for it as well
     if ((pa_cmd_err != PDSM_PA_CMD_ERR_NOERR) ||
         (pa_module_data_ptr->pa_cmd == PDSM_PA_DATA_ENABLE))
      {
         ioctl_status = loc_pa_xlate_cmd_err (pa_cmd_err);
         (void) loc_client_queue_ioctl_callback ((loc_client_handle_type) data_block_ptr,
                                                 pa_module_data_ptr->pa_iotcl_type,
                                                 ioctl_status,
                                                 &ioctl_callback_data );
         loc_pa_reset_module ();
      }
      else
      {
         LOC_MSG_HIGH ("loc_pa_cmd_cb, need to wait for PA event\n", 0, 0, 0);
      }
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
}


// Translate EFS data block from Loc API to PDAPI
// Returns TRUE for success
static boolean loc_pa_xlate_efs_data_from_locapi_to_pdapi
(
      pdsm_efs_data_s_type      *pdsm_efs_data,
      const loc_efs_data_s_type *loc_efs_data
)
{
   uint8 pdsm_op;
   int filename_len;

   // Operation
   switch (loc_efs_data->operation)
   {
   case LOC_FS_CREATE_WRITE_FILE:
      pdsm_op = PDSM_FS_CREATE_WRITE_FILE;
      break;
   case LOC_FS_APPEND_FILE:
      pdsm_op = PDSM_FS_APPEND_FILE;
      break;
   case LOC_FS_DELETE_FILE:
      pdsm_op = PDSM_FS_DELETE_FILE;
      break;
   case LOC_FS_READ_FILE:
      pdsm_op = PDSM_FS_READ_FILE;
      break;
   case LOC_FS_PURGE_DIR:
      pdsm_op = PDSM_FS_PURGE_DIR;
      break;
   default:
      LOC_MSG_ERROR("loc_pa_xlate_efs_data_from_locapi_to_pdapi, unknown operation: %d\n",
            loc_efs_data->operation, 0, 0);
      return FALSE;
   }
   pdsm_efs_data->efs_data_file_operation = pdsm_op;

   // Filename
   if (loc_efs_data->filename == NULL)
   {
      LOC_MSG_ERROR("loc_pa_xlate_efs_data_from_locapi_to_pdapi, filename is NULL", 0,0,0);
      return FALSE;
   }
   filename_len = strlen(loc_efs_data->filename);
   if (filename_len > PDSM_PD_FS_MAX_PATH_LEN_BYTES - 1)
   {
      LOC_MSG_ERROR("loc_pa_xlate_efs_data_from_locapi_to_pdapi, filename too long, len=%d, max=%d",
            filename_len, PDSM_PD_FS_MAX_PATH_LEN_BYTES-1, 0);
      return FALSE;
   }
   else {
      pdsm_efs_data->efs_data_filename_len = filename_len + 1;
      pdsm_efs_data->efs_data_filename = (char*) loc_efs_data->filename;
   }

   // Check injection length for the file
   if (loc_efs_data->part_len > PDSM_PD_EFS_MAX_FILE_LEN_BYTES)
   {
      LOC_MSG_ERROR("loc_pa_xlate_efs_data_from_locapi_to_pdapi, data size too large: %d > %d\n",
            loc_efs_data->part_len, PDSM_PD_EFS_MAX_FILE_LEN_BYTES, 0);
      return FALSE;
   }

   // Other fields
   pdsm_efs_data->efs_data_len = loc_efs_data->part_len;
   pdsm_efs_data->efs_data_part_number = loc_efs_data->part;
   pdsm_efs_data->efs_data_ptr = loc_efs_data->data_ptr;
   pdsm_efs_data->efs_data_total_parts = loc_efs_data->total_parts;

   pdsm_efs_data->efs_data_reserved = loc_efs_data->reserved;

   return TRUE;
}

// Function to process EFS data injection, using LOC API return codes
static int loc_pa_access_efs_data
(
      loc_client_handle_type     client_handle,
      const loc_efs_data_s_type *efs_data
)
{
   int                        ret_val = LOC_API_SUCCESS;
   loc_pa_module_data_s_type* pa_module_data_ptr = &loc_middleware_data.pa_module_data;
   pdsm_pa_info_type          pa_info;
   boolean                    status = FALSE;

   if (!loc_pa_xlate_efs_data_from_locapi_to_pdapi(&pa_info.efs_data, efs_data))
   {
      ret_val = LOC_API_INVALID_PARAMETER;
   }
   else {
      pa_module_data_ptr->pa_cmd = PDSM_PA_EFS_DATA;
      pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
      status = pdsm_set_parameters(loc_pa_cmd_cb,
            (void *) client_handle,
            pa_module_data_ptr->pa_cmd,
            &pa_info,
            loc_middleware_data.pdapi_client_id);

      if (!status)
      {
         ret_val = LOC_API_GENERAL_FAILURE;
      }
   }

   LOC_MSG_HIGH("loc_pa_access_efs_data, ret_val = %d \n", ret_val, 0, 0);

   return ret_val;
}


// Function to process loc_ioctl with LOC_IOCTL_SET_DATA_ENABLE
static int loc_pa_set_user_control
(
      loc_client_handle_type client_handle,
      boolean                data_enable
)
{
  int                         ret_val = LOC_API_GENERAL_FAILURE;
  loc_pa_module_data_s_type*  pa_module_data_ptr;
  pdsm_pa_info_type           pa_info;
  boolean                     status = FALSE;
  
  pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
  pa_module_data_ptr->pa_cmd = PDSM_PA_DATA_ENABLE;
  pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
  pa_info.data_enable= data_enable;
  status = pdsm_set_parameters (loc_pa_cmd_cb,
                               (void *) client_handle,
                               pa_module_data_ptr->pa_cmd,
                               &pa_info,
                               loc_middleware_data.pdapi_client_id);

  if (status == TRUE )
  {
    ret_val = LOC_API_SUCCESS;
  }
  else
  {
    ret_val = LOC_API_GENERAL_FAILURE;
  }
  
  LOC_MSG_HIGH("loc_pa_set_user_control, ret_val = %d \n", ret_val, 0, 0);
  return ret_val;
}

// Function to process loc_ioctl with LOC_IOCTL_SET_SUPL_VERSION
static int loc_pa_set_supl_version
(
      loc_client_handle_type  client_handle,
      const uint32            supl_version 
)
{
   int                        ret_val = LOC_API_GENERAL_FAILURE;
   loc_pa_module_data_s_type* pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
   pdsm_pa_info_type          pa_info;
   boolean                    status = FALSE;

   pa_info.supl_version = supl_version;
   pa_module_data_ptr->pa_cmd = PDSM_PA_SUPL_VERSION;
   pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;

   status = pdsm_set_parameters (loc_pa_cmd_cb,
                                 (void *) client_handle,
                                 pa_module_data_ptr->pa_cmd,
                                 &pa_info,
                                 loc_middleware_data.pdapi_client_id);

   if (status == TRUE )
   {
      ret_val = LOC_API_SUCCESS;
   }
   else
   {
      ret_val = LOC_API_GENERAL_FAILURE;
   }

   LOC_MSG_HIGH ( "loc_pa_set_supl_version, ret_val = %d \n", ret_val, 0, 0);

   return ret_val;
}

// Function to process loc_ioctl with LOC_IOCTL_SET_ENGINE_LOCK.
static int loc_pa_set_engine_lock
(
      loc_client_handle_type  client_handle,
       const loc_lock_e_type* engine_lock_ptr
)
{
   int                        ret_val = LOC_API_GENERAL_FAILURE;
   loc_pa_module_data_s_type* pa_module_data_ptr;
   pdsm_pa_info_type          pa_info;
   boolean                    status = FALSE;

   memset( &pa_info, 0, sizeof(pa_info) );

   ret_val = loc_pa_xlate_lock_from_locapi_to_pdapi (engine_lock_ptr, &(pa_info.gps_lock));
   if (ret_val == LOC_API_SUCCESS)
   {
      pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
      pa_module_data_ptr->pa_cmd = PDSM_PA_GPS_LOCK;
      pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
      status = pdsm_set_parameters (loc_pa_cmd_cb,
                                    (void *) client_handle,
                                    pa_module_data_ptr->pa_cmd,
                                    &pa_info,
                                    loc_middleware_data.pdapi_client_id);

      if (status == TRUE )
      {
         ret_val = LOC_API_SUCCESS;
      }
      else
      {
         ret_val = LOC_API_GENERAL_FAILURE;
      }
   }

   LOC_MSG_HIGH ("loc_pa_set_engine_lock, ret_val = %d \n",
                 ret_val, 0, 0);

   return ret_val;
}


// Function to process loc_ioctl with LOC_IOCTL_SET_GNSS_CONSTELL_REPORT_CONFIG
static int loc_pa_set_gnss_constell_report_config
(
      loc_client_handle_type  client_handle,
       const qmiLocSetGNSSConstRepConfigReqMsgT_v02* gnss_config_ptr
)
{
   int                        ret_val = LOC_API_GENERAL_FAILURE;
   loc_pa_module_data_s_type* pa_module_data_ptr;
   pdsm_pa_info_type          pa_info;
   boolean                    status = FALSE;

   memset( &pa_info, 0, sizeof(pa_info) );

   ret_val = loc_pa_xlate_gnss_config_from_locapi_to_pdapi (gnss_config_ptr, &(pa_info.z_set_gnss_config));
   if (ret_val == LOC_API_SUCCESS)
   {
      pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
      pa_module_data_ptr->pa_cmd = PDSM_GNSS_CONSTELL_REPORT_CONFIG;
      pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
      status = pdsm_set_parameters (loc_pa_cmd_cb,
                                    (void *) client_handle,
                                    pa_module_data_ptr->pa_cmd,
                                    &pa_info,
                                    loc_middleware_data.pdapi_client_id);

      if (status == TRUE )
      {
         ret_val = LOC_API_SUCCESS;
      }
      else
      {
         ret_val = LOC_API_GENERAL_FAILURE;
      }
   }

   LOC_MSG_HIGH ("loc_pa_set_GNSS Config, ret_val = %d \n",
                 ret_val, 0, 0);

   return ret_val;
}




// Function to process loc_ioctl with:
// LOC_IOCTL_SET_CDMA_PDE_SERVER_ADDR,
// LOC_IOCTL_SET_CDMA_MPC_SERVER_ADDR,
// LOC_IOCTL_SET_UMTS_SLP_SERVER_ADDR
static int loc_pa_set_server_addr
(
      loc_client_handle_type         client_handle,
      loc_ioctl_e_type        ioctl_type,
      const loc_server_info_s_type* server_addr_ptr
)
{

   int                        ret_val = LOC_API_GENERAL_FAILURE;
   loc_pa_module_data_s_type* pa_module_data_ptr;
   boolean                    status = FALSE;
   pdsm_pa_info_type          pa_info;

   pa_module_data_ptr = &(loc_middleware_data.pa_module_data);

   ret_val = loc_pa_xlate_server_addr_from_locapi_to_pdapi (server_addr_ptr, &pa_info.server_address);

   if (ret_val == LOC_API_SUCCESS)
   {
      if (ioctl_type == LOC_IOCTL_SET_CDMA_PDE_SERVER_ADDR)
      {
         pa_module_data_ptr->pa_cmd = PDSM_PA_1X_PDE_NETWORK_ADRS;
      }
      else if (ioctl_type == LOC_IOCTL_SET_UMTS_SLP_SERVER_ADDR )
      {
         pa_module_data_ptr->pa_cmd = PDSM_PA_UMTS_PDE_NETWORK_ADRS;
      }
      else // LOC_IOCTL_SET_CDMA_MPC_SERVER_ADDR
      {
         pa_module_data_ptr->pa_cmd = PDSM_PA_1X_MPC_NETWORK_ADRS;
      }

      pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
      status = pdsm_set_parameters (loc_pa_cmd_cb,
                                    (void *) client_handle,
                                    pa_module_data_ptr->pa_cmd,
                                    &pa_info,
                                    loc_middleware_data.pdapi_client_id);

      if (status == TRUE )
      {
         ret_val = LOC_API_SUCCESS;
      }
      else
      {
         ret_val = LOC_API_GENERAL_FAILURE;
      }
   }

   LOC_MSG_HIGH ("loc_pa_set_server_addr, ret_val = %d \n",
                 ret_val, 0, 0);

   return ret_val;
}

// Function to process loc_ioctl with LOC_IOCTL_SET_SBAS_CONFIG
static int loc_pa_set_sbas_config
(
      loc_client_handle_type  client_handle,
      boolean          enable_sbas
)
{
   int                         ret_val = LOC_API_GENERAL_FAILURE;
   loc_pa_module_data_s_type*  pa_module_data_ptr;
   pdsm_pa_info_type           pa_info;
   boolean                     status = FALSE;

   pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
   pa_module_data_ptr->pa_cmd = PDSM_PA_SBAS_USER_PREFERENCE;
   pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
   pa_info.sbas_user_preference = enable_sbas;
   status = pdsm_set_parameters (loc_pa_cmd_cb,
                                 (void *) client_handle,
                                 pa_module_data_ptr->pa_cmd,
                                 &pa_info,
                                 loc_middleware_data.pdapi_client_id);

   if (status == TRUE )
   {
      ret_val = LOC_API_SUCCESS;
   }
   else
   {
      ret_val = LOC_API_GENERAL_FAILURE;
   }

   LOC_MSG_HIGH ("loc_pa_set_sbas_config, ret_val = %d \n",
                 ret_val, 0, 0);

   return ret_val;
}

// Function to process loc_ioctl with LOC_IOCTL_SET_NMEA_TYPES
static int loc_pa_set_nmea_types
(
      loc_client_handle_type               client_handle,
      const loc_nmea_sentence_type* nmea_types_ptr
)
{
   int                         ret_val = LOC_API_GENERAL_FAILURE;
   loc_pa_module_data_s_type*  pa_module_data_ptr;
   pdsm_pa_info_type           pa_info;
   boolean                     status = FALSE;

   ret_val = loc_pa_xlate_nmea_types_from_locapi_to_pdapi (nmea_types_ptr, &(pa_info.nmea_sentence_type));
   if (ret_val == LOC_API_SUCCESS)
   {
      pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
      pa_module_data_ptr->pa_cmd = PDSM_PA_NMEA_SENTENCE_TYPE;
      pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
      status = pdsm_set_parameters (loc_pa_cmd_cb,
                                    (void *) client_handle,
                                    pa_module_data_ptr->pa_cmd,
                                    &pa_info,
                                    loc_middleware_data.pdapi_client_id);

      if (status == TRUE )
      {
         ret_val = LOC_API_SUCCESS;
      }
      else
      {
         ret_val = LOC_API_GENERAL_FAILURE;
      }
   }
   return ret_val;
}

// Function to process loc_ioctl with LOC_IOCTL_DELETE_ASSIST_DATA
static int loc_pa_delete_aiding_data
(
      loc_client_handle_type                client_handle,
      const loc_assist_data_delete_s_type*  aiding_data_deletion_info_ptr
)
{
   int                         ret_val = LOC_API_GENERAL_FAILURE;
   loc_pa_module_data_s_type*  pa_module_data_ptr;
   pdsm_pa_info_type           pa_info;
   boolean                     status = FALSE;

   ret_val = loc_pd_xlate_aiding_data_mask_from_locapi_to_pdapi (&(pa_info.delete_params), aiding_data_deletion_info_ptr);

   if (ret_val == LOC_API_SUCCESS)
   {
     pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
     pa_module_data_ptr->pa_cmd = PDSM_PA_DELETE_PARAMS;
     pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;

     status = pdsm_set_parameters (loc_pa_cmd_cb,
                                   (void *) client_handle,
                                   pa_module_data_ptr->pa_cmd,
                                   &pa_info,
                                   loc_middleware_data.pdapi_client_id);

     if (status == TRUE )
     {
        ret_val = LOC_API_SUCCESS;
     }
     else
     {
        ret_val = LOC_API_GENERAL_FAILURE;
     }
   }

   LOC_MSG_HIGH ("loc_pa_delete_aiding_data, ret_val = %d \n",
                 ret_val, 0, 0);

   return ret_val;
}

// Function to process loc_ioctl with LOC_IOCTL_DELETE_GNSS_SERVICE_DATA,
static int loc_pa_delete_gnss_service_data
(
  loc_client_handle_type  l_ClientHandle,
  const qmiLocDeleteGNSSServiceDataReqMsgT_v02* pz_DeleteGnssServiceData
)
{
   int                         l_Ret = LOC_API_GENERAL_FAILURE;
   loc_pa_module_data_s_type*  pz_PaModuleData = NULL;
   pdsm_pa_info_type           z_PaInfo = {0};
   boolean                     v_Status = FALSE;

   l_Ret = loc_pa_xlate_delete_gnss_service_data (pz_DeleteGnssServiceData, &(z_PaInfo.delete_params));

   if (l_Ret == LOC_API_SUCCESS)
   {
      pz_PaModuleData = &(loc_middleware_data.pa_module_data);
      pz_PaModuleData->pa_cmd = PDSM_PA_DELETE_PARAMS_EX1;
      pz_PaModuleData->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
      v_Status = pdsm_set_parameters (loc_pa_cmd_cb,
                                   (void *) l_ClientHandle,
                                   pz_PaModuleData->pa_cmd,
                                   &z_PaInfo,
                                   loc_middleware_data.pdapi_client_id);

      if (v_Status == TRUE )
      {
         l_Ret = LOC_API_SUCCESS;
      }
      else
      {
         l_Ret = LOC_API_GENERAL_FAILURE;
      }
   }

   LOC_MSG_HIGH ("loc_pa_delete_gnss_service_data, l_Ret = %d \n",
                 l_Ret, 0, 0);

   return l_Ret;
}

// Set NV item for on demand low power mode
static int loc_pa_set_on_demand_lpm
(
      loc_client_handle_type  client_handle,
      boolean                 mode
)
{
   int                         ret_val = LOC_API_GENERAL_FAILURE;
   loc_pa_module_data_s_type*  pa_module_data_ptr;
   boolean                     status = FALSE;
   pdsm_pa_info_type           pa_info;

   pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
   pa_module_data_ptr->pa_cmd = PDSM_PA_ON_DEMAND_LOW_POWER_MODE;
   pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
   pa_info.on_demand_lpm = mode;
   status = pdsm_set_parameters (loc_pa_cmd_cb,
                                 (void *) client_handle,
                                 pa_module_data_ptr->pa_cmd,
                                 &pa_info,
                                 loc_middleware_data.pdapi_client_id);

   if (status == TRUE )
   {
      ret_val = LOC_API_SUCCESS;
   }
   else
   {
      ret_val = LOC_API_GENERAL_FAILURE;
   }

   LOC_MSG_HIGH ("loc_pa_set_on_demand_lpm, ret_val = %d \n",
                 ret_val, 0, 0);

   return ret_val;
}

// Function to process loc_ioctl with LOC_IOCTL_SET_LBS(XTRA)_APN_PROFILE.
static int loc_pa_set_apn_profiles
(
  loc_client_handle_type  client_handle,
  loc_ioctl_e_type        profile_type,
  const loc_apn_profiles_type *apn_profile_p
)
{
  int                        ret_val = LOC_API_GENERAL_FAILURE;
  loc_pa_module_data_s_type*  pa_module_data_ptr;
  pdsm_pa_info_type           pa_info;
  boolean                     status = FALSE;  
  
  if(profile_type == LOC_IOCTL_SET_LBS_APN_PROFILE ||
     profile_type == LOC_IOCTL_SET_XTRA_APN_PROFILE )
  {
    pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
    if(profile_type == LOC_IOCTL_SET_LBS_APN_PROFILE)
    {
      pa_module_data_ptr->pa_cmd = PDSM_PA_LBS_APN_PROFILE;
    }
    else
    {
      pa_module_data_ptr->pa_cmd = PDSM_PA_XTRA_APN_PROFILE;
    }
    pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
    memscpy((void *)&pa_info.apn_profiles[0], sizeof(pa_info.apn_profiles), (void*)apn_profile_p,sizeof(pa_info.apn_profiles));
    status = pdsm_set_parameters (loc_pa_cmd_cb,
                                 (void *) client_handle,
                                 pa_module_data_ptr->pa_cmd,
                                 &pa_info,
                                 loc_middleware_data.pdapi_client_id);
  
    if (status == TRUE )
    {
      ret_val = LOC_API_SUCCESS;
    }
    else
    {
      ret_val = LOC_API_GENERAL_FAILURE;
    }
  }
  return ret_val;
}


// Translate engine lock from PD API to LOC API
static int loc_pa_xlate_lock_from_pdapi_to_locapi
(
      const pdsm_gps_lock_e_type*  pdapi_lock_ptr,
      loc_lock_e_type*             locapi_lock_ptr
)
{
   int ret_val = LOC_API_SUCCESS;

   if (*pdapi_lock_ptr == PDSM_GPS_LOCK_NONE)
   {
      *locapi_lock_ptr = LOC_LOCK_NONE;
   }
   else if (*pdapi_lock_ptr == PDSM_GPS_LOCK_ALL)
   {
      *locapi_lock_ptr = LOC_LOCK_ALL;
   }
   else if (*pdapi_lock_ptr == PDSM_GPS_LOCK_MI)
   {
      *locapi_lock_ptr = LOC_LOCK_MI;
   }
   else if (*pdapi_lock_ptr == PDSM_GPS_LOCK_MT)
   {
      *locapi_lock_ptr = LOC_LOCK_MT;
   }
   else
   {
      ret_val = LOC_API_INVALID_PARAMETER;
   }

   LOC_MSG_HIGH ("loc_pa_xlate_lock_from_pdapi_to_locapi, pdapi lock = %d, locapi lock = %d \n",
                 *pdapi_lock_ptr, *locapi_lock_ptr, 0);

   return ret_val;
}

// Translate engine lock from LOC API to PD API
static int loc_pa_xlate_lock_from_locapi_to_pdapi
(
      const loc_lock_e_type*  locapi_lock_ptr,
      pdsm_gps_lock_e_type*   pdapi_lock_ptr
)
{
   int ret_val = LOC_API_SUCCESS;

   if (*locapi_lock_ptr == LOC_LOCK_NONE)
   {
      *pdapi_lock_ptr = PDSM_GPS_LOCK_NONE;
   }
   else if (*locapi_lock_ptr == LOC_LOCK_ALL )
   {
      *pdapi_lock_ptr = PDSM_GPS_LOCK_ALL;
   }
   else if (*locapi_lock_ptr == LOC_LOCK_MI)
   {
      *pdapi_lock_ptr = PDSM_GPS_LOCK_MI;
   }
   else if (*locapi_lock_ptr == LOC_LOCK_MT)
   {
      *pdapi_lock_ptr = PDSM_GPS_LOCK_MT;
   }
   else
   {
      ret_val =  LOC_API_INVALID_PARAMETER;
   }

   LOC_MSG_HIGH ("loc_pa_xlate_lock_from_locapi_to_pdapi, locapi lock = %d, pdapi lock = %d \n",
                 *locapi_lock_ptr, *pdapi_lock_ptr, 0);

   return ret_val;
}


// Translate gnss config information from LOC API to PD API
static int loc_pa_xlate_gnss_config_from_locapi_to_pdapi
(
      const qmiLocSetGNSSConstRepConfigReqMsgT_v02*  locapi_gnss_config_ptr,
      pdsm_LocSetGNSSConfigReqStruct*   pdapi_gnss_config_ptr
)
{
   int ret_val = LOC_API_SUCCESS;

  LOC_MSG_MED ("loc_pa_xlate_gnss_config_from_locapi_to_pdapi, Incoming measConfig = %d, svPolyConfig = %d \n",
                 locapi_gnss_config_ptr->measReportConfig, locapi_gnss_config_ptr->svPolyReportConfig, 0);

  pdapi_gnss_config_ptr->meas_report_config_valid = locapi_gnss_config_ptr->measReportConfig_valid;
  pdapi_gnss_config_ptr->e_meas_report_config = locapi_gnss_config_ptr->measReportConfig;
  pdapi_gnss_config_ptr->sv_poly_report_config_valid = locapi_gnss_config_ptr->svPolyReportConfig_valid;
  pdapi_gnss_config_ptr->e_sv_poly_report_config = locapi_gnss_config_ptr->svPolyReportConfig;  

   return ret_val;
}



// Translate server address from LOC API to PD API, function being shared with pa and client module
int loc_pa_xlate_server_addr_from_pdapi_to_locapi
(
      const pdsm_server_address_s_type*  pdapi_server_addr_ptr,
      loc_server_info_s_type*            locapi_server_addr_ptr
)
{
   int ret_val = LOC_API_SUCCESS;
   int url_length;

   LOC_MSG_HIGH ("loc_pa_xlate_server_addr_from_pdapi_to_locapi, addr type = %d, \n",
                 pdapi_server_addr_ptr->server_addr_type, 0, 0);

   if (pdapi_server_addr_ptr->server_addr_type == PDSM_SERVER_TYPE_IPV4_ADDRESS )
   {
      locapi_server_addr_ptr->addr_type = LOC_SERVER_ADDR_IPV4;
      locapi_server_addr_ptr->addr_info.ipv4.addr = pdapi_server_addr_ptr->server_adrs.server_adrs_v4.server_adrs;
      locapi_server_addr_ptr->addr_info.ipv4.port = pdapi_server_addr_ptr->server_adrs.server_adrs_v4.port_id;
   }
   else if ( pdapi_server_addr_ptr->server_addr_type == PDSM_SERVER_TYPE_URL_ADDRESS)
   {
      locapi_server_addr_ptr->addr_type = LOC_SERVER_ADDR_URL;

      // Reserve the last byte in the array for null-terminating the string

      url_length = memscpy (&(locapi_server_addr_ptr->addr_info.url.addr),
	  	      sizeof (locapi_server_addr_ptr->addr_info.url.addr) - 1,
              &(pdapi_server_addr_ptr->server_adrs.server_adrs_url.url),
              pdapi_server_addr_ptr->server_adrs.server_adrs_url.url_length);

      locapi_server_addr_ptr->addr_info.url.addr[url_length] = '\0';

      locapi_server_addr_ptr->addr_info.url.length = url_length;
   }
   else
   {
      ret_val = LOC_API_INVALID_PARAMETER;
   }

   return ret_val;
}

// Translate server address from LOC API to PD API, this function is being used by loc_client
// as well to translate the custom location server address
int loc_pa_xlate_server_addr_from_locapi_to_pdapi
(
      const loc_server_info_s_type* locapi_server_addr_ptr,
      pdsm_server_address_s_type*   pdapi_server_addr_ptr
)
{
   int ret_val = LOC_API_SUCCESS;

   LOC_MSG_HIGH ("loc_pa_xlate_server_addr_from_locapi_to_pdapi, addr type = %d, \n",
                 locapi_server_addr_ptr->addr_type, 0, 0);

   if (locapi_server_addr_ptr->addr_type == LOC_SERVER_ADDR_IPV4 )
   {
      pdapi_server_addr_ptr->server_addr_type = PDSM_SERVER_TYPE_IPV4_ADDRESS;
      pdapi_server_addr_ptr->server_adrs.server_adrs_v4.server_adrs = locapi_server_addr_ptr->addr_info.ipv4.addr;
      pdapi_server_addr_ptr->server_adrs.server_adrs_v4.port_id = locapi_server_addr_ptr->addr_info.ipv4.port;
   }
   else if (locapi_server_addr_ptr->addr_type == LOC_SERVER_ADDR_URL )
   {
      pdapi_server_addr_ptr->server_addr_type = PDSM_SERVER_TYPE_URL_ADDRESS;

      pdapi_server_addr_ptr->server_adrs.server_adrs_url.url_length = 
	  	  memscpy (&(pdapi_server_addr_ptr->server_adrs.server_adrs_url.url),
	  	      sizeof (pdapi_server_addr_ptr->server_adrs.server_adrs_url.url),
              &(locapi_server_addr_ptr->addr_info.url.addr ),
                      locapi_server_addr_ptr->addr_info.url.length);
   }
   else
   {
      ret_val = LOC_API_INVALID_PARAMETER;
   }

  return ret_val;
}

// Translate SBAS status from PDAPI to LOC API
static int loc_pa_xlate_sbas_status_from_pdapi_to_locapi
(
      const pdsm_pa_sbas_status_e_type*  pdapi_sbas_mode_ptr,
      boolean*                           locapi_sbas_mode_ptr
)
{
   int ret_val = LOC_API_SUCCESS;

   if (*pdapi_sbas_mode_ptr == PDSM_PA_SBAS_STATUS_ENABLED)
   {
      *locapi_sbas_mode_ptr = TRUE;
   }
   else
   {
      *locapi_sbas_mode_ptr = FALSE;
   }

   LOC_MSG_HIGH ("loc_pa_xlate_sbas_status_from_pdapi_to_locapi, pdapi mode = %d locapi mode = %d, \n",
                 *pdapi_sbas_mode_ptr, *locapi_sbas_mode_ptr, 0);

   return ret_val;
}

// Translate NMEA sentence types from PD API to LOC API
static int loc_pa_xlate_nmea_types_from_pdapi_to_locapi
(
      const pdsm_pa_nmea_type* pdapi_nmea_types_ptr,
      loc_nmea_sentence_type*  locapi_nmea_types_ptr
)
{
   int ret_val = LOC_API_SUCCESS;

   *locapi_nmea_types_ptr = 0;

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_GGA) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_GGA;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_RMC) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_RMC;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_GSV) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_GSV;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_GSA) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_GSA;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_VTG) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_VTG;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_PQXFI) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_PQXFI;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_GLGSV) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_GLGSV;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_GNGSA) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_GNGSA;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_GNGNS) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_GNGNS;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_GARMC) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_GARMC;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_GAGSV) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_GAGSV;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_GAGSA) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_GAGSA;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_GAVTG) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_GAVTG;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_PSTIS) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_PSTIS;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_GAGGA) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_GAGGA;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_PQGSA) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_PQGSA;
   }

   if ( 0 != (*pdapi_nmea_types_ptr & PDAPI_NMEA_PQGSV) )
   {
      *locapi_nmea_types_ptr |= LOC_NMEA_MASK_PQGSV;
   }

   LOC_MSG_HIGH ("loc_pa_xlate_nmea_types_from_pdapi_to_locapi,  pdapi_nmea_types = %d locapi_nmea_types = %d, \n",
                 *pdapi_nmea_types_ptr, *locapi_nmea_types_ptr, 0);

   return ret_val;
}

// Translate NMEA sentence types from LOC API to PD API
static int loc_pa_xlate_nmea_types_from_locapi_to_pdapi
(
      const loc_nmea_sentence_type* locapi_nmea_types_ptr,
      pdsm_pa_nmea_type*            pdapi_nmea_types_ptr
)
{
   int ret_val = LOC_API_SUCCESS;

   *pdapi_nmea_types_ptr = 0;
   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_GGA) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_GGA;
   }

   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_RMC) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_RMC;
   }

   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_GSV) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_GSV;
   }

   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_GSA)  )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_GSA;
   }

   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_VTG) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_VTG;
   }

   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_PQXFI) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_PQXFI;
   }

   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_GLGSV) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_GLGSV;
   }

   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_GNGSA) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_GNGSA;
   }

   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_GNGNS) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_GNGNS;
   }

   if ( 0 != (*locapi_nmea_types_ptr &  LOC_NMEA_MASK_GARMC) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_GARMC;
   }

   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_GAGSV) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_GAGSV;
   }

   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_GAGSA) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_GAGSA;
   }

   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_GAVTG) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_GAVTG;
   }

   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_PSTIS) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_PSTIS;
   }

   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_GAGGA) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_GAGGA;
   }

   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_PQGSA) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_PQGSA;
   }

   if ( 0 != (*locapi_nmea_types_ptr & LOC_NMEA_MASK_PQGSV) )
   {
      *pdapi_nmea_types_ptr |= PDAPI_NMEA_PQGSV;
   }

   LOC_MSG_HIGH ("loc_pa_xlate_nmea_types_from_locapi_to_pdapi, locapi_nmea_types = %d, pdapi_nmea_types = %d \n",
                 *locapi_nmea_types_ptr, *pdapi_nmea_types_ptr, 0);

   return ret_val;
}

// Translate aiding data deletion mask from LOC API to PD API
static int loc_pd_xlate_aiding_data_mask_from_locapi_to_pdapi
(
   pdsm_delete_parms_type                  *pdapi_aiding_data_mask_ptr,
   const loc_assist_data_delete_s_type   *locapi_aiding_data_mask_ptr
)
{
   int ret_val = LOC_API_SUCCESS;

   if(NULL == pdapi_aiding_data_mask_ptr || NULL == locapi_aiding_data_mask_ptr)
   {
      LOC_MSG_ERROR("loc_pd_xlate_aiding_data_mask_from_locapi_to_pdapi, 0x%x, 0x%x",
                    pdapi_aiding_data_mask_ptr,locapi_aiding_data_mask_ptr,0);
      return LOC_API_INVALID_PARAMETER;
   }

   memset (pdapi_aiding_data_mask_ptr, 0, sizeof (pdsm_delete_parms_type));

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_EPH) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_ALM) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_POS) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_POS;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_TIME) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_TIME;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_IONO) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_IONO;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_UTC) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_UTC;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_HEALTH) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_HEALTH;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_SVDIR) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_SVSTEER) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_SADATA) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SADATA;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_RTI) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_RTI;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_CELLDB_INFO) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_CELLDB_INFO;
      /* LocQmiShim Converts to PDAPI */
      pdapi_aiding_data_mask_ptr->delete_celldb_mask = locapi_aiding_data_mask_ptr->cell_db;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_ALM_CORR_DELETE) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_CORR;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_FREQ_BIAS_EST_DELETE) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_FREQ_BIAS_EST;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_EPH_GLO) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH_GLO;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_ALM_GLO) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_GLO;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_SVDIR_GLO) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR_GLO;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_SVSTEER_GLO) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER_GLO;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_ALM_CORR_GLO_DELETE) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_CORR_GLO;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_TIME_GPS) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_TIME_GPS;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_TIME_GLO) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_TIME_GLO;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_SVDIR_SBAS) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR_SBAS;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_SVSTEER_SBAS) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER_SBAS;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_CLOCK_INFO) )
   {
   
     pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_CLOCK_INFO;
     /* LocQmiShim Converts to PDAPI */
      pdapi_aiding_data_mask_ptr->delete_clock_info_mask = locapi_aiding_data_mask_ptr->clock_info;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_SV_NO_EXIST) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SV_NO_EXIST;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_TIME_BDS) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_TIME_BDS;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_EPH_BDS) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH_BDS;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_ALM_BDS) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_BDS;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_SVDIR_BDS) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR_BDS;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_SVSTEER_BDS) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER_BDS;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_ALM_CORR_BDS_DELETE) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_CORR_BDS;
   }

    if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_GPS) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GPS;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_GLO) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GLO;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_BDS) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_GNSS_SV_BLACKLIST_BDS;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_GAL) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GAL;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_TIME_GAL) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_TIME_GAL;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_EPH_GAL) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH_GAL;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_ALM_GAL) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_GAL;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_SVDIR_GAL) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR_GAL;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_SVSTEER_GAL) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER_GAL;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_ALM_CORR_GAL_DELETE) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_CORR_GAL;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_EPH_SBAS) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH_SBAS;
   }

   if ( 0 != (locapi_aiding_data_mask_ptr->type & LOC_ASSIST_DATA_ALM_SBAS) )
   {
      pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_SBAS;
   }


   LOC_MSG_HIGH ("xlate delete mask: loc aiding mask = 0x%llx, pdapi aiding mask = 0x%llx",
                  locapi_aiding_data_mask_ptr->type, 
                  pdapi_aiding_data_mask_ptr->pdsm_delete_parms_flags, 
                  0);
   LOC_MSG_HIGH ("xlate delete mask: cell db mask = 0x%x, clock info mask = 0x%x",
                  pdapi_aiding_data_mask_ptr->delete_celldb_mask, 
                  pdapi_aiding_data_mask_ptr->delete_clock_info_mask, 
                  0);
   

   return ret_val;
}

loc_pa_mask_map_s_type z_QmiPdsmClockInfoMaskMap[LOC_PA_CLOCK_INFO_MASK_LEN] =
{
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_TIME_EST_V02,
      PDSM_PA_DELETE_CLOCK_INFO_TIME_EST },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_FREQ_EST_V02,
      PDSM_PA_DELETE_CLOCK_INFO_FREQ_EST },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_WEEK_NUMBER_V02,
      PDSM_PA_DELETE_CLOCK_INFO_WEEK_NUMBER },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_RTC_TIME_V02, 
      PDSM_PA_DELETE_CLOCK_INFO_RTC_TIME },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_TIME_TRANSFER_V02,
      PDSM_PA_DELETE_CLOCK_INFO_TIME_TRANSFER },     
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GPSTIME_EST_V02,
      PDSM_PA_DELETE_CLOCK_INFO_GPSTIME_EST },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GLOTIME_EST_V02,
      PDSM_PA_DELETE_CLOCK_INFO_GLOTIME_EST },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GLODAY_NUMBER_V02,
      PDSM_PA_DELETE_CLOCK_INFO_GLODAY_NUMBER },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GLO4YEAR_NUMBER_V02,
      PDSM_PA_DELETE_CLOCK_INFO_GLO4YEAR_NUMBER },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY_V02,
      PDSM_PA_DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_DISABLE_TT_V02,
      PDSM_PA_DELETE_CLOCK_INFO_DISABLE_TT },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GG_LEAPSEC_V02,
      PDSM_PA_DELETE_CLOCK_INFO_GG_LEAPSEC },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GG_GGTB_V02,
      PDSM_PA_DELETE_CLOCK_INFO_GG_GGTB },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_BDSTIME_EST_V02,
      PDSM_PA_DELETE_CLOCK_INFO_BDSTIME_EST },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GB_GBTB_V02,
      PDSM_PA_DELETE_CLOCK_INFO_GB_GBTB },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_BG_BGTB_V02,
      PDSM_PA_DELETE_CLOCK_INFO_BG_BGTB },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_BDSWEEK_NUMBER_V02,
      PDSM_PA_DELETE_CLOCK_INFO_BDSWEEK_NUMBER },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_BDS_RF_GRP_DELAY_V02,
      PDSM_PA_DELETE_CLOCK_INFO_BDS_RF_GRP_DELAY },
  /*  { QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTIME_EST_V02,
      PDSM_PA_DELETE_CLOCK_INFO_ }, PDSM mask not defined yet */
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTOGPS_TB_V02,
      PDSM_PA_DELETE_CLOCK_INFO_GPS_GAL_TB },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTOGLO_TB_V02,
      PDSM_PA_DELETE_CLOCK_INFO_GLO_GAL_TB },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTOBDS_TB_V02,
      PDSM_PA_DELETE_CLOCK_INFO_GAL_BDS_TB },
  /*  { QMI_LOC_MASK_DELETE_CLOCK_INFO_GALWEEK_NUMBER_V02,
      PDSM_PA_DELETE_CLOCK_INFO_ }, PDSM mask not defined yet 
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GAL_RF_GRP_DELAY_V02,
      PDSM_PA_DELETE_CLOCK_INFO_ }, PDSM mask not defined yet */
};


loc_pa_mask_map_s_type z_QmiPdsmCellDBMaskMap[LOC_PA_CELL_DB_MASK_LEN] =
{
    { QMI_LOC_MASK_DELETE_CELLDB_POS_V02,
      PDSM_PA_DELETE_CELLDB_MASK_POS_INFO },
    { QMI_LOC_MASK_DELETE_CELLDB_LATEST_GPS_POS_V02,
      PDSM_PA_DELETE_CELLDB_MASK_LATEST_GPS_POS_INFO },
    { QMI_LOC_MASK_DELETE_CELLDB_OTA_POS_V02,
      PDSM_PA_DELETE_CELLDB_MASK_OTA_POS_INFO },
    { QMI_LOC_MASK_DELETE_CELLDB_EXT_REF_POS_V02, 
      PDSM_PA_DELETE_CELLDB_MASK_EXT_REF_POS_INFO },
    { QMI_LOC_MASK_DELETE_CELLDB_TIMETAG_V02,
      PDSM_PA_DELETE_CELLDB_MASK_TIMETAG_INFO },
    { QMI_LOC_MASK_DELETE_CELLDB_CELLID_V02,
      PDSM_PA_DELETE_CELLDB_MASK_CELLID_INFO },
    { QMI_LOC_MASK_DELETE_CELLDB_CACHED_CELLID_V02,
      PDSM_PA_DELETE_CELLDB_MASK_CACHED_CELLID_INFO },
    { QMI_LOC_MASK_DELETE_CELLDB_LAST_SRV_CELL_V02,
      PDSM_PA_DELETE_CELLDB_MASK_LAST_SRV_CELL_INFO },
    { QMI_LOC_MASK_DELETE_CELLDB_CUR_SRV_CELL_V02,
      PDSM_PA_DELETE_CELLDB_MASK_CUR_SRV_CELL_INFO },
    { QMI_LOC_MASK_DELETE_CELLDB_NEIGHBOR_INFO_V02,
      PDSM_PA_DELETE_CELLDB_MASK_NEIGHBOR_INFO_INFO },
};

// A general helper function to translate QMI mask to PDSM mask
static uint32 loc_pa_xlate_qmi_pdsm_mask(
   loc_pa_mask_map_s_type *pz_QmiPdsmMaskMap,
   uint32                  q_Mask,
   uint8                   u_MaskMapSize)
{
   int    l_Cntr;
   uint32 q_PdapiMask = 0;

   if(NULL == pz_QmiPdsmMaskMap)
   {
      LOC_MSG_ERROR("loc_pa_xlate_qmi_pdsm_mask, NULL pointer",0,0,0);
      return q_PdapiMask;
   }

   for (  l_Cntr = 0; l_Cntr < u_MaskMapSize; l_Cntr++ )
   {
      if(0 != (pz_QmiPdsmMaskMap[l_Cntr].q_qmi_mask & q_Mask))
      {
         q_PdapiMask |= pz_QmiPdsmMaskMap[l_Cntr].q_pdapi_mask;
      }
   }

   return q_PdapiMask;
}

// Translate GNSS service data deleteCommonDataMask
static void loc_pa_xlate_delete_common_mask(
  qmiLocDeleteCommonDataMaskT_v02 q_Mask,
  pdsm_delete_parms_type*   const pz_Param)
{
   if(NULL == pz_Param)
   {
      LOC_MSG_ERROR("loc_pa_xlate_delete_common_mask, NULL pointer",0,0,0);
      return;
   }

   if(0 != (q_Mask & QMI_LOC_DELETE_COMMON_MASK_POS_V02))
   {
      pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_POS;
   }
   if(0 != (q_Mask & QMI_LOC_DELETE_COMMON_MASK_TIME_V02))
   {
      pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_TIME;
   }
   if(0 != (q_Mask & QMI_LOC_DELETE_COMMON_MASK_UTC_V02))
   {
      pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_UTC;
   }
   if(0 != (q_Mask & QMI_LOC_DELETE_COMMON_MASK_RTI_V02))
   {
      pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_RTI;
   }
   if(0 != (q_Mask & QMI_LOC_DELETE_COMMON_MASK_FREQ_BIAS_EST_V02))
   {
      pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_FREQ_BIAS_EST;
   }
}

// Translate GNSS service data system & deleteSatelliteDataMask
static void loc_pa_xlate_delete_satellite_data(
  const qmiLocDeleteSatelliteDataStructT_v02 *pz_DeleteSatelliteData,
  pdsm_delete_parms_type* pz_Param)
{
   if(NULL == pz_Param || NULL == pz_DeleteSatelliteData)
   {
      LOC_MSG_ERROR("loc_pa_xlate_delete_satellite_data, 0x%x, 0x%x",
                     pz_Param, pz_DeleteSatelliteData, 0);
      return;
   }

   if(0 != (pz_DeleteSatelliteData->system & QMI_LOC_SYSTEM_GPS_V02))
   {
      LOC_MSG_MED("loc_pa_xlate_delete_satellite_data, GPS, deleteSatelliteDataMask=0x%x",
                     pz_DeleteSatelliteData->deleteSatelliteDataMask, 0, 0);
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_EPHEMERIS_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH;
         pz_Param->eph_sv_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALMANAC_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM;
         pz_Param->alm_sv_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVHEALTH_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_HEALTH;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVDIR_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVSTEER_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALM_CORR_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_CORR;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_BLACKLIST_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GPS;
         pz_Param->gps_gnss_sv_blacklist_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SA_DATA_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SADATA;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SV_NO_EXIST_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SV_NO_EXIST;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_IONO_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_IONO;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_TIME_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_TIME_GPS;
      }
   }
   if(0 != (pz_DeleteSatelliteData->system & QMI_LOC_SYSTEM_GLO_V02))
   {
      LOC_MSG_MED("loc_pa_xlate_delete_satellite_data, GLO, deleteSatelliteDataMask=0x%x",
                     pz_DeleteSatelliteData->deleteSatelliteDataMask, 0, 0);
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_EPHEMERIS_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH_GLO;
         pz_Param->glo_eph_sv_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALMANAC_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_GLO;
         pz_Param->glo_alm_sv_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVHEALTH_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_HEALTH_GLO;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVDIR_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR_GLO;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVSTEER_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER_GLO;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALM_CORR_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_CORR_GLO;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_BLACKLIST_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GLO;
         pz_Param->glo_gnss_sv_blacklist_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_TIME_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_TIME_GLO;
      }
   }
   if(0 != (pz_DeleteSatelliteData->system & QMI_LOC_SYSTEM_BDS_V02))
   {
      LOC_MSG_MED("loc_pa_xlate_delete_satellite_data, BDS, deleteSatelliteDataMask=0x%x",
                     pz_DeleteSatelliteData->deleteSatelliteDataMask, 0, 0);
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_EPHEMERIS_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH_BDS;
         pz_Param->bds_eph_sv_mask = CGPS_MAX_UINT64;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALMANAC_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_BDS;
         pz_Param->bds_alm_sv_mask = CGPS_MAX_UINT64;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVHEALTH_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_HEALTH_BDS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVDIR_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR_BDS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVSTEER_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER_BDS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALM_CORR_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_CORR_BDS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_BLACKLIST_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_GNSS_SV_BLACKLIST_BDS;
         pz_Param->bds_gnss_sv_blacklist_mask = CGPS_MAX_UINT64;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_TIME_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_TIME_BDS;
      }
   }
   if(0 != (pz_DeleteSatelliteData->system & QMI_LOC_SYSTEM_GAL_V02))
   {
      LOC_MSG_MED("loc_pa_xlate_delete_satellite_data, GAL, deleteSatelliteDataMask=0x%x",
                     pz_DeleteSatelliteData->deleteSatelliteDataMask, 0, 0);
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_EPHEMERIS_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH_GAL;
         pz_Param->gal_eph_sv_mask = CGPS_MAX_UINT64;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALMANAC_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_GAL;
         pz_Param->gal_alm_sv_mask = CGPS_MAX_UINT64;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVDIR_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR_GAL;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVSTEER_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER_GAL;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALM_CORR_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_CORR_GAL;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_BLACKLIST_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GAL;
         pz_Param->gal_gnss_sv_blacklist_mask = CGPS_MAX_UINT64;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_TIME_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_TIME_GAL;
      }
   }
   if(0 != (pz_DeleteSatelliteData->system & QMI_LOC_SYSTEM_QZSS_V02))
   {
      LOC_MSG_MED("loc_pa_xlate_delete_satellite_data, QZSS, deleteSatelliteDataMask=0x%x",
                     pz_DeleteSatelliteData->deleteSatelliteDataMask, 0, 0);
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_EPHEMERIS_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH_QZSS;
         pz_Param->qzss_eph_sv_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALMANAC_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_QZSS;
         pz_Param->qzss_alm_sv_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVHEALTH_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_HEALTH_QZSS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVDIR_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR_QZSS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVSTEER_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER_QZSS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALM_CORR_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_CORR_QZSS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_BLACKLIST_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_GNSS_SV_BLACKLIST_QZSS;
         pz_Param->qzss_gnss_sv_blacklist_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_IONO_V02))
      {
         pz_Param->pdsm_delete_parms_flags |= PDSM_PA_DELETE_IONO_QZSS;
      }
   }
}

//GNSS delete service data handler function
static int loc_pa_xlate_delete_gnss_service_data
(
   const qmiLocDeleteGNSSServiceDataReqMsgT_v02  *pz_DeleteGnssServiceData,
   pdsm_delete_parms_type                        *pz_PdapiDeleteDataMask
)
{
   int l_Ret = LOC_API_SUCCESS;

   if(NULL == pz_DeleteGnssServiceData || NULL == pz_PdapiDeleteDataMask)
   {
      LOC_MSG_ERROR("loc_pa_xlate_delete_gnss_service_data, 0x%x, 0x%x",
                    pz_DeleteGnssServiceData,pz_PdapiDeleteDataMask,0);
      return LOC_API_INVALID_PARAMETER;
   }

   memset(pz_PdapiDeleteDataMask, 0, sizeof(*pz_PdapiDeleteDataMask));

   if( FALSE == loc_pa_is_valid_boolean(pz_DeleteGnssServiceData->deleteAllFlag))
   {
      LOC_MSG_ERROR("loc_pa_xlate_delete_gnss_service_data, invalid deleteAllFlag %d",
                     pz_DeleteGnssServiceData->deleteAllFlag,0,0);
      return LOC_API_INVALID_PARAMETER;
   }

   if(TRUE == pz_DeleteGnssServiceData->deleteAllFlag )
   {
      qmiLocDeleteSatelliteDataStructT_v02  z_DeleteSatellitData = {0};
      z_DeleteSatellitData.system = CGPS_MAX_UINT32;
      z_DeleteSatellitData.deleteSatelliteDataMask = CGPS_MAX_UINT32;

      // clock Info
      pz_PdapiDeleteDataMask->pdsm_delete_parms_flags |= PDSM_PA_DELETE_CLOCK_INFO;
      pz_PdapiDeleteDataMask->delete_clock_info_mask = LOC_CLOCK_INFO_DELETE_ALL;

      // cell db
      pz_PdapiDeleteDataMask->pdsm_delete_parms_flags |= PDSM_PA_DELETE_CELLDB_INFO;
      pz_PdapiDeleteDataMask->delete_celldb_mask = LOC_CELLDB_DELETE_ALL;

      // common data
      loc_pa_xlate_delete_common_mask(CGPS_MAX_UINT32,
                                      pz_PdapiDeleteDataMask);

      // satellite data
      loc_pa_xlate_delete_satellite_data(&z_DeleteSatellitData,
                                         pz_PdapiDeleteDataMask);

      // miscellaneous SBAS data
      pz_PdapiDeleteDataMask->sbas_eph_sv_mask = CGPS_MAX_UINT32;
      pz_PdapiDeleteDataMask->sbas_alm_sv_mask = CGPS_MAX_UINT32;
   }
   else
   {
       // TLV=0x10 - clock Info
      if(TRUE == pz_DeleteGnssServiceData->deleteClockInfoMask_valid)
      {
         pz_PdapiDeleteDataMask->pdsm_delete_parms_flags |= PDSM_PA_DELETE_CLOCK_INFO;
         pz_PdapiDeleteDataMask->delete_clock_info_mask = 
         loc_pa_xlate_qmi_pdsm_mask(z_QmiPdsmClockInfoMaskMap,
                                    pz_DeleteGnssServiceData->deleteClockInfoMask,
                                    sizeof(z_QmiPdsmClockInfoMaskMap)/sizeof(z_QmiPdsmClockInfoMaskMap[0]));
      }

      // TLV=0x11 - cell db
      if(TRUE == pz_DeleteGnssServiceData->deleteCellDbDataMask_valid)
      {
         pz_PdapiDeleteDataMask->pdsm_delete_parms_flags |= PDSM_PA_DELETE_CELLDB_INFO;
         pz_PdapiDeleteDataMask->delete_celldb_mask = 
         loc_pa_xlate_qmi_pdsm_mask(z_QmiPdsmCellDBMaskMap,
                                    pz_DeleteGnssServiceData->deleteCellDbDataMask,
                                    sizeof(z_QmiPdsmCellDBMaskMap)/sizeof(z_QmiPdsmCellDBMaskMap[0]));
      }

      // TLV=0x12 - common data
      if(TRUE == pz_DeleteGnssServiceData->deleteCommonDataMask_valid)
      {
         loc_pa_xlate_delete_common_mask(pz_DeleteGnssServiceData->deleteCommonDataMask,
                                         pz_PdapiDeleteDataMask);
      }

      // TLV=0x13 - satellite data
      if(TRUE == pz_DeleteGnssServiceData->deleteSatelliteData_valid)
      {
         loc_pa_xlate_delete_satellite_data(&pz_DeleteGnssServiceData->deleteSatelliteData,
                                             pz_PdapiDeleteDataMask);
      }
   }

   return l_Ret;
}

// Translate pa cmd error to the ioctl_status to be notified to client.
static int32 loc_pa_xlate_cmd_err (pdsm_pa_cmd_err_e_type pa_cmd_err)
{
   int32 ret_val = LOC_API_GENERAL_FAILURE;

   switch (pa_cmd_err)
   {
   case PDSM_PA_CMD_ERR_NOERR:
      ret_val = LOC_API_SUCCESS;
      break;

   case PDSM_PA_CMD_ERR_OFFLINE_S:
      ret_val = LOC_API_PHONE_OFFLINE;
      break;

   case PDSM_PA_CMD_ERR_PA_ACTIVE_S:
      ret_val = LOC_API_ENGINE_BUSY;
      break;

   case PDSM_PA_CMD_ERR_NOT_ALLOWED:
      ret_val = LOC_API_ENGINE_BUSY;
      break;

   default:
      ret_val = LOC_API_GENERAL_FAILURE;
      break;
   }

   LOC_MSG_HIGH ("loc_pa_xlate_cmd_err, pa_cmd_err = %d, ret_val = %d \n", pa_cmd_err, ret_val, 0);

   return ret_val;
}

// When the timer fires, it means location engine has not received the pa callback
// In this case, notify client of the failed status and reset the state
void loc_pa_process_timer_cb ( void )
{
   loc_pa_module_data_s_type* pa_module_data_ptr = &(loc_middleware_data.pa_module_data);

   LOC_MSG_HIGH ("loc_pa_process_timer_cb fired \n", 0, 0, 0);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
   if (pa_module_data_ptr->pa_cmd_is_in_progress == TRUE)
   {
      // Notify client about the timeout
      loc_client_queue_ioctl_callback (pa_module_data_ptr->pa_client_handle,
                                       pa_module_data_ptr->pa_iotcl_type,
                                       LOC_API_TIMEOUT,
                                       NULL);
      // Reset the pa module
      pa_module_data_ptr->pa_cmd_is_in_progress = FALSE;
      pa_module_data_ptr->pa_client_handle      = LOC_CLIENT_HANDLE_INVALID;
   }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
}

// Reset pa module. This function is called when this in progress pa cmd is done.
// This function will be called when: pa cmd error fails, or pa event arrives
static void loc_pa_reset_module (void)
{
   loc_pa_module_data_s_type* pa_module_data_ptr = &(loc_middleware_data.pa_module_data);

   LOC_MSG_HIGH ("loc_pa_reset_module called \n", 0, 0, 0);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);

   // Clear the timer
   os_TimerStop(pa_module_data_ptr->pa_process_timer);

   // Reset the pa module
   pa_module_data_ptr->pa_cmd_is_in_progress = FALSE;
   pa_module_data_ptr->pa_client_handle      = LOC_CLIENT_HANDLE_INVALID;

   /* Reset the Union that carries the information regarding the incoming  
      request */
   memset ( &(pa_module_data_ptr->pa_ioctl_req_info),
            0,
            sizeof(pa_module_data_ptr->pa_ioctl_req_info));
   
   

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
}

// Function to process loc_ioctl with LOC_IOCTL_SET_XTRA_T_SESSION_CONTROL.
static int loc_pa_set_xtra_t_session_ctrl
(
      loc_client_handle_type         client_handle,
      const uint8 control 
)
{
   int                        ret_val = LOC_API_GENERAL_FAILURE;
   loc_pa_module_data_s_type* pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
   pdsm_pa_info_type          pa_info;
   boolean                    status = FALSE;

   pa_info.session_control = control;
   pa_module_data_ptr->pa_cmd = PDSM_PA_XTRAT_USER_SESSION_CONTROL;
   pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;

   status = pdsm_set_parameters (loc_pa_cmd_cb,
                                 (void *) client_handle,
                                 pa_module_data_ptr->pa_cmd,
                                 &pa_info,
                                 loc_middleware_data.pdapi_client_id);

   if (status == TRUE )
   {
      ret_val = LOC_API_SUCCESS;
   }
   else
   {
      ret_val = LOC_API_GENERAL_FAILURE;
   }

   LOC_MSG_HIGH ( "loc_pa_set_xtra_t_session_ctrl, ret_val = %d \n",
                 ret_val, 0, 0);

   return ret_val;
}

// Function to process loc_ioctl with LOC_IOCTL_SET_SUPL_SECURITY.
static int loc_pa_set_supl_security
(
  loc_client_handle_type  client_handle,
  boolean supl_security
)
{
   int                        ret_val = LOC_API_GENERAL_FAILURE;
   loc_pa_module_data_s_type* pa_module_data_ptr;
   pdsm_pa_info_type          pa_info;
   boolean                    status = FALSE;

  /* No translation required, as the exposed api over Loc-IOCTL is
    TRUE: SUPL, FALSE: UPL */
  pa_info.supl_security = supl_security;
  
  pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
  pa_module_data_ptr->pa_cmd = PDSM_PA_SUPL_SECURITY;
  pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
  /* Use the new callback to handle QMI-Loc IOCTls */
  status = pdsm_set_parameters (loc_pa_cmd_cb,
                                (void *) client_handle,
                                pa_module_data_ptr->pa_cmd,
                                &pa_info,
                                loc_middleware_data.pdapi_client_id);

  if (status == TRUE )
  {
     ret_val = LOC_API_SUCCESS;
  }
  else
  {
     ret_val = LOC_API_GENERAL_FAILURE;
  }

   LOC_MSG_HIGH ("loc_pa_set_supl_security, ret_val = %d \n",
                 ret_val, 0, 0);

   return ret_val;
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to handle the IOCTL to Set Position Engine Config
  
@param[in]   pz_ClientHandle               Loc Client handle
@param[in]   pz_SetPeCfg                 The information received from client

@retval    LOC_API_SUCCESS        Converted successfully
@retval    <Anything Else>        DId not convert successfully
*//* ------------------------------------------------------------------------*/

static int loc_pa_set_pe_config 
(
  loc_client_handle_type  pz_ClientHandle, 
  const qmiLocSetPositionEngineConfigParametersReqMsgT_v02 *pz_SetPeCfg
)
{
   int                        ret_val = LOC_API_GENERAL_FAILURE;
   loc_pa_module_data_s_type* pa_module_data_ptr;
   pdsm_pa_info_type          pa_info;
   boolean                    status = FALSE;
	 pdsm_pa_pe_config_s_type   *pe_Cfg = &(pa_info.pe_config_info);
   boolean                   b_invalid_params = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                 
  pe_Cfg->peConfigMask = 0;

  if ( ( TRUE == pz_SetPeCfg->injectedPositionControl_valid) &&
       ( TRUE == loc_pa_is_valid_boolean(pz_SetPeCfg->injectedPositionControl) ) )
  {
      pe_Cfg->peConfigMask |=  
                      PDSM_PA_PE_CONFIG_PARAM_MASK_USE_INJECTED_POSITION;
      pe_Cfg->useInjectedPosition = pz_SetPeCfg->injectedPositionControl;
  }
  else
  {
    b_invalid_params = TRUE;
  }

  if ( ( TRUE == pz_SetPeCfg->filterSvUsage_valid) &&
       ( TRUE == loc_pa_is_valid_boolean(pz_SetPeCfg->filterSvUsage) ) )
  {
      pe_Cfg->peConfigMask |=  
                      PDSM_PA_PE_CONFIG_PARAM_MASK_FILTER_SV_USED;
      pe_Cfg->filterSvUsed = pz_SetPeCfg->filterSvUsage;

  }
  else
  {
    b_invalid_params = TRUE;
  }

  if ( ( TRUE == pz_SetPeCfg->storeAssistData_valid )&&
       ( TRUE == loc_pa_is_valid_boolean(pz_SetPeCfg->storeAssistData ) ) )
  {
      pe_Cfg->peConfigMask |=  
                      PDSM_PA_PE_CONFIG_PARAM_MASK_STORE_ASSIST_DATA;
      pe_Cfg->storeAssistData = pz_SetPeCfg->storeAssistData;
  }
  else
  {
    b_invalid_params = TRUE;
  }

  /* If atleast one of the parameters is invalid, reject the entire message */
  if ( FALSE == b_invalid_params )
  {
    pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
    pa_module_data_ptr->pa_cmd = PDSM_PA_PE_CONFIG_PARAM;
    pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
    /* Use the new callback to handle QMI-Loc IOCTls */
    status = pdsm_set_parameters (loc_pa_cmd_cb,
                                  (void *) pz_ClientHandle,
                                  pa_module_data_ptr->pa_cmd,
                                  &pa_info,
                                  loc_middleware_data.pdapi_client_id);

    if (status == TRUE )
    {
       ret_val = LOC_API_SUCCESS;
    }
    else
    {
       ret_val = LOC_API_GENERAL_FAILURE;
    }
  }
  else
  {
    ret_val = LOC_API_INVALID_PARAMETER;
  }

   LOC_MSG_HIGH ("loc_pa_set_pe_config, ret_val = %d \n",
                 ret_val, 0, 0);

   return ret_val;
}

/** loc_pa_set_lpp_config
 *  Handle the request to set the LPP configuration
 *  @param[in] client_handle: Loc API client handle
 *  @param [in] lpp_config_ptr: LPP configuration 
 *   */

static int loc_pa_set_lpp_config
(
  loc_client_handle_type  client_handle,
  const qmiLocLppConfigMaskT_v02 * const lpp_config_ptr
)
{
  int                        ret_val = LOC_API_GENERAL_FAILURE;

  if(NULL != lpp_config_ptr)
  {
    loc_pa_module_data_s_type* pa_module_data_ptr;
    pdsm_pa_info_type          pa_info;
    boolean                    status = FALSE;
    pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
    pa_module_data_ptr->pa_cmd =  PDSM_PA_LPP_CONFIG_INFO;
    pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
    
    memset(&pa_info, 0, sizeof(pa_info));

    if( *lpp_config_ptr &  QMI_LOC_LPP_CONFIG_ENABLE_USER_PLANE_V02 )
    {
      pa_info.lpp_config_info |= PDSM_PA_LPP_IN_LTE_UP_ENABLE;
    }
    if(*lpp_config_ptr &  QMI_LOC_LPP_CONFIG_ENABLE_CONTROL_PLANE_V02)
    {
       pa_info.lpp_config_info |= PDSM_PA_LPP_IN_LTE_CP_ENABLE;
    }

    /* Use the new callback to handle QMI-Loc IOCTls */
    status = pdsm_set_parameters (loc_pa_cmd_cb,
                                (void *) &client_handle,
                                pa_module_data_ptr->pa_cmd,
                                &pa_info,
                                loc_middleware_data.pdapi_client_id);

    if (TRUE == status )
    {
      ret_val = LOC_API_SUCCESS;
    }
   
  }

  LOC_MSG_HIGH ("loc_pa_set_lpp_config, ret_val = %d \n",
                  ret_val, 0, 0);

  return ret_val;
}

/**
 * @brief converts QMI LOC Assisted Glonass Mask to PDAPI format
 * @param [in] :  qmiLocAssistedGlonassMask 
 * @retval : PDAPI Assisted Glonass Mask 
 */

static uint32  loc_pa_conv_assisted_glo_from_qmi(
  qmiLocAssistedGlonassProtocolMaskT_v02 qmiLocAssistedGlonassMask )
{
  int32 i;
  uint32 pdapiAssistedGlonassMask = 0;
  /** Not checkign parameter here as this funtiona is called by
   *  inernal modules with valid pointers */
  for( i=0;
       i<(sizeof(loc_pa_assisted_glo_map)/sizeof(loc_pa_assisted_glo_map[0]));
       i++ )
  {
    if(qmiLocAssistedGlonassMask & loc_pa_assisted_glo_map[i].qmiLocAssistedGlonassMask )
    {
      pdapiAssistedGlonassMask |= loc_pa_assisted_glo_map[i].pdapiAssistedGlonassMask;
    }
  }

  return pdapiAssistedGlonassMask;
  
}

/** loc_pa_set_assisted_glo_config
 *  Handle the request to set the assisted glonass configuration
 *  @param[in] client_handle: Loc API client handle
 *  @param [in] assisted_glo_config: assisted glonass
 *         configuration mask
 *   */

static int loc_pa_set_assisted_glo_config
(
  loc_client_handle_type  client_handle,
  qmiLocAssistedGlonassProtocolMaskT_v02 assisted_glo_config
)
{
  int                        ret_val = LOC_API_GENERAL_FAILURE;
  loc_pa_module_data_s_type* pa_module_data_ptr;
  pdsm_pa_info_type          pa_info;
  boolean                    status = FALSE;

  pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
  pa_module_data_ptr->pa_cmd =  PDSM_PA_ASST_GLONASS_POS_PROTOCOL;
  pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
    
  memset(&pa_info, 0, sizeof(pa_info));

  pa_info.asst_glo_protocol = 
     loc_pa_conv_assisted_glo_from_qmi(assisted_glo_config);
  
  /* Use the new callback to handle QMI-Loc IOCTls */
  status = pdsm_set_parameters (loc_pa_cmd_cb,
                                (void *) &client_handle,
                                pa_module_data_ptr->pa_cmd,
                                &pa_info,
                                loc_middleware_data.pdapi_client_id);
  
  if (TRUE == status )
  {
    ret_val = LOC_API_SUCCESS;
  }
   
  LOC_MSG_MED ("loc_pa_set_assisted_glo_config, ret_val = %d "
               "assisted glo config pdsm =%u, loc = %u ",
                ret_val,
                pa_info.asst_glo_protocol,
                assisted_glo_config);

  return ret_val;
}

/** loc_pa_set_emergency_protocol
 *  Handle the request to set the emergency protocol
 *  @param[in] client_handle: Loc API client handle
 *  @param [in] assisted_glo_config: assisted glonass
 *         configuration mask
 *   */

static int loc_pa_set_emergency_protocol
(
  loc_client_handle_type  client_handle,
  qmiLocEmergencyProtocolEnumT_v02 emergency_protocol
)
{
  int                        ret_val = LOC_API_GENERAL_FAILURE;
  loc_pa_module_data_s_type* pa_module_data_ptr;
  pdsm_pa_info_type          pa_info;
  boolean                    status = FALSE;

  pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
  pa_module_data_ptr->pa_cmd =  PDSM_PA_GNSS_EMERGENCY_PROTOCOL_CONFIG;
  pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
    
  memset(&pa_info, 0, sizeof(pa_info));

  switch (emergency_protocol)
  {
  case eQMI_LOC_EMERGENCY_PROTOCOL_WCDMA_CP_V02:
    pa_info.gnss_emergency_protocol_select = 
      PDSM_PA_EMERGENCY_PROTOCOL_CONFIG_WCMDA_CP;
    break;

  case eQMI_LOC_EMERGENCY_PROTOCOL_WCDMA_UP_V02:
    pa_info.gnss_emergency_protocol_select = 
      PDSM_PA_EMERGENCY_PROTOCOL_CONFIG_WCMDA_UP;
    break;

  default:
    LOC_MSG_ERROR("loc_pa_set_emergency_protocol: invalid protocol %d",
                  emergency_protocol, 0,0);
    return LOC_API_INVALID_PARAMETER;
 }
  
 /* Use the new callback to handle QMI-Loc IOCTls */
  status = pdsm_set_parameters (loc_pa_cmd_cb,
                                (void *) &client_handle,
                                pa_module_data_ptr->pa_cmd,
                                &pa_info,
                                loc_middleware_data.pdapi_client_id);
  
  if (TRUE == status )
  {
    ret_val = LOC_API_SUCCESS;
  }
   
  LOC_MSG_MED ("loc_pa_set_emergency_protocol, ret_val = %d "
               "emergency protocol: pdsm =%u, loc = %d ",
                ret_val,
                pa_info.gnss_emergency_protocol_select,
                emergency_protocol);

  return ret_val;
}

/** loc_pa_set_supl_hash_algo
 *  Handle the request to set the supl hash algorithm
 *  @param[in] client_handle: Loc API client handle
 *  @param [in] supl_hash_algo: supl hash algorithm
 *   */

static int loc_pa_set_supl_hash_algo
(
  loc_client_handle_type  client_handle,
  qmiLocSuplHashAlgoEnumT_v02 supl_hash_algo
)
{
  int                        ret_val = LOC_API_GENERAL_FAILURE;
  loc_pa_module_data_s_type* pa_module_data_ptr;
  pdsm_pa_info_type          pa_info;
  boolean                    status = FALSE;

  pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
  pa_module_data_ptr->pa_cmd =  PDSM_PA_SUPL_HASH_ALGORITHM;
  pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
    
  memset(&pa_info, 0, sizeof(pa_info));

  switch (supl_hash_algo)
  {
  case eQMI_LOC_SUPL_HASH_ALGO_SHA1_V02:
   { 
     pa_info.supl_hash_algorithm = PDSM_PA_SUPL_HASH_ALGO_SHA1;
     break;
   }

  case eQMI_LOC_SUPL_HASH_ALGO_SHA256_V02:
   { 
     pa_info.supl_hash_algorithm = PDSM_PA_SUPL_HASH_ALGO_SHA256;
     break;
   }

  default:
    {
      LOC_MSG_ERROR("loc_pa_set_supl_hash_algo: invalid hash value passed %d",
                    supl_hash_algo, 0, 0);
      return LOC_API_INVALID_PARAMETER;

    }

  }
  
  /* Use the new callback to handle QMI-Loc IOCTls */
  status = pdsm_set_parameters (loc_pa_cmd_cb,
                                (void *) &client_handle,
                                pa_module_data_ptr->pa_cmd,
                                &pa_info,
                                loc_middleware_data.pdapi_client_id);
  
  if (TRUE == status )
  {
    ret_val = LOC_API_SUCCESS;
  }
   
  LOC_MSG_MED ("loc_pa_set_supl_hash_algo, ret_val = %d "
               "hash algo pdsm =%u, loc = %u ",
                ret_val,
                pa_info.supl_hash_algorithm,
                supl_hash_algo);

  return ret_val;
}


/** loc_pa_set_supl_tls_version
 *  Handle the request to set the supl TLS version
 *  @param[in] client_handle: Loc API client handle
 *  @param [in] supl_tls_version: supl TLS version
 *   */

static int loc_pa_set_supl_tls_version
(
  loc_client_handle_type  client_handle,
  qmiLocSuplTlsVersionEnumT_v02 supl_tls_version
)
{
  int                        ret_val = LOC_API_GENERAL_FAILURE;
  loc_pa_module_data_s_type* pa_module_data_ptr;
  pdsm_pa_info_type          pa_info;
  boolean                    status = FALSE;

  pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
  pa_module_data_ptr->pa_cmd =  PDSM_PA_SUPL_TLS_VERSION;
  pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
    
  memset(&pa_info, 0, sizeof(pa_info));

  switch (supl_tls_version)
  {
  case eQMI_LOC_SUPL_TLS_VERSION_1_0_V02:
   { 
     pa_info.supl_tls_version = PDSM_PA_SUPL_TLS_VERSION_1_0;
     break;
   }

  case eQMI_LOC_SUPL_TLS_VERSION_1_1_V02:
   { 
     pa_info.supl_tls_version = PDSM_PA_SUPL_TLS_VERSION_1_1;
     break;
   }

  default:
    {
      LOC_MSG_ERROR("loc_pa_set_supl_tls_version: invalid tls version passed %d",
                    supl_tls_version, 0, 0);
      return LOC_API_INVALID_PARAMETER;

    }

  }
  
  /* Use the new callback to handle QMI-Loc IOCTls */
  status = pdsm_set_parameters (loc_pa_cmd_cb,
                                (void *) &client_handle,
                                pa_module_data_ptr->pa_cmd,
                                &pa_info,
                                loc_middleware_data.pdapi_client_id);
  
  if (TRUE == status )
  {
    ret_val = LOC_API_SUCCESS;
  }
   
  LOC_MSG_MED ("loc_pa_set_supl_tls_version, ret_val = %d "
               "TLS version pdsm =%u, loc = %u ",
                ret_val,
                pa_info.supl_tls_version,
                supl_tls_version);

  return ret_val;
}


/** loc_pa_set_wifi_scan_inject_timeout
 *  Handle the request to set the wifi scan info injection
 *  timeout
 *  @param[in] client_handle: Loc API client handle
 *  @param [in] wifi_scan_inject_timeout: timeout duration
 *   */
static int loc_pa_set_wifi_scan_inject_timeout
(
  loc_client_handle_type  client_handle,
  uint8 wifi_scan_inject_timeout
)
{
  int                        ret_val = LOC_API_GENERAL_FAILURE;
  loc_pa_module_data_s_type* pa_module_data_ptr;
  pdsm_pa_info_type          pa_info;
  boolean                    status = FALSE;

  pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
  pa_module_data_ptr->pa_cmd =  PDSM_PA_WIFI_SCAN_TIMEOUT_SEC;
  pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
    
  memset(&pa_info, 0, sizeof(pa_info));

  pa_info.wifi_wait_timeout_sec = wifi_scan_inject_timeout; /* uint8 time in seconds */
  
  /* Use the new callback to handle QMI-Loc IOCTls */
  status = pdsm_set_parameters (loc_pa_cmd_cb,
                                (void *) &client_handle,
                                pa_module_data_ptr->pa_cmd,
                                &pa_info,
                                loc_middleware_data.pdapi_client_id);
  
  if (TRUE == status )
  {
    ret_val = LOC_API_SUCCESS;
  }
   
  LOC_MSG_MED ("loc_pa_set_wifi_scan_inject_timeout, ret_val = %d "
               "wifi_scan_inject_timeout pdsm = %u, loc = %u",
                ret_val,
                pa_info.wifi_wait_timeout_sec,
                wifi_scan_inject_timeout);

  return ret_val;
}

/** loc_pa_get_param
 *  Handle the request to get pdsm parameters
 *  @param[in] client_handle: Loc API client handle
 *  @param [in] pa_cmd: pdsm pa parameter to get
 *   */

static int loc_pa_get_param
(
  loc_client_handle_type  client_handle,
  pdsm_pa_e_type          pa_cmd   
)
{
   int                        ret_val = LOC_API_GENERAL_FAILURE;
   loc_pa_module_data_s_type* pa_module_data_ptr;
   boolean                    status = FALSE;

   pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
   pa_module_data_ptr->pa_cmd =  pa_cmd;
   pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_GET_PARAM;
   status = pdsm_get_parameters (loc_pa_cmd_cb,
                                 (void *) client_handle,
                                 pa_module_data_ptr->pa_cmd,
                                 loc_middleware_data.pdapi_client_id);

   if (TRUE == status )
   {
      ret_val = LOC_API_SUCCESS;
   }
   else
   {
      ret_val = LOC_API_GENERAL_FAILURE;
   }

   LOC_MSG_MED ("loc_pa_get_param, param = %d ret_val = %d \n",
                 pa_cmd, ret_val, 0);

   return ret_val;
}

/** loc_pa_set_premium_svcs_cfg
 *  Handle the request to set the Premium Services Configuration
 *  @param[in] client_handle  : Loc API client handle
 *  @param[in] premium_svc_cfg: Premium Service Type & Configuration
 *   */

static int loc_pa_set_premium_svcs_cfg
(
  loc_client_handle_type                        client_handle,
  const qmiLocSetPremiumServicesCfgReqMsgT_v02* premium_svc_cfg
)
{
  int                        ret_val = LOC_API_GENERAL_FAILURE;
  loc_pa_module_data_s_type* pa_module_data_ptr;
  pdsm_pa_info_type          pa_info;
  boolean                    status = FALSE;

  pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
  pa_module_data_ptr->pa_cmd =  PDSM_PA_PREMIUM_SERVICES_CONFIG;
  pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;
    
  memset(&pa_info, 0, sizeof(pa_info));

  switch (premium_svc_cfg->premiumServiceType)
  {
    case eQMI_LOC_PREMIUM_SERVICE_GTP_CELL_V02:
    {
      pa_info.premium_svc_cfg.service = PDSM_PA_PREMIUM_SVC_GTP_CELL;
      break;
    }

    case eQMI_LOC_PREMIUM_SERVICE_SAP_V02:
    {
      pa_info.premium_svc_cfg.service = PDSM_PA_PREMIUM_SVC_SAP;
      break;
    }

    case eQMI_LOC_PREMIUM_SERVICE_GTP_ENH_CELL_V02:
    {
      pa_info.premium_svc_cfg.service = PDSM_PA_PREMIUM_SVC_GTP_ENH_CELL;
      break;
    }

    default:
    {
      LOC_MSG_ERROR("loc_pa_set_premium_svcs_cfg: invalid service type passed %d",
                     premium_svc_cfg->premiumServiceType, 0, 0);
      return LOC_API_INVALID_PARAMETER;
    }
  }

  switch (premium_svc_cfg->premiumServiceCfg)
  {
    case eQMI_LOC_PREMIUM_SERVICE_DISABLED_V02:
    {
      pa_info.premium_svc_cfg.config = PDSM_PA_PREMIUM_SVC_CFG_DISABLED;
      break;
    }

    case eQMI_LOC_PREMIUM_SERVICE_ENABLED_BASIC_V02:
    {
      pa_info.premium_svc_cfg.config = PDSM_PA_PREMIUM_SVC_CFG_ENABLED_BASIC;
      break;
    }

    case eQMI_LOC_PREMIUM_SERVICE_ENABLED_PREMIUM_V02:
    {
      pa_info.premium_svc_cfg.config = PDSM_PA_PREMIUM_SVC_CFG_ENABLED_PREMIUM;
      break;
    }

    default:
    {
      LOC_MSG_ERROR("loc_pa_set_premium_svcs_cfg: invalid service config type passed %d",
                     premium_svc_cfg->premiumServiceCfg, 0, 0);
      return LOC_API_INVALID_PARAMETER;
    }
  }

  /* Use the new callback to handle QMI-Loc IOCTls */
  status = pdsm_set_parameters (loc_pa_cmd_cb,
                                (void *) client_handle,
                                pa_module_data_ptr->pa_cmd,
                                &pa_info,
                                loc_middleware_data.pdapi_client_id);

  if (TRUE == status)
  {
    ret_val = LOC_API_SUCCESS;
  }
   
  LOC_MSG_MED ("loc_pa_set_premium_svcs_cfg, ret_val=%d, Service=%d, Config=%d",
                ret_val,
                pa_info.premium_svc_cfg.service,
                pa_info.premium_svc_cfg.config);

  return ret_val;
}

/** loc_pa_set_xtra_version_check
 *  Handle the request to set XTRA version check enum
 *  @param[in] client_handle: Loc API client handle
 *  @param[in] qmiLocXtraVersionCheckEnumT_v02: XTRA version
 *      check configuration
 *   */
static int loc_pa_set_xtra_version_check
(
  loc_client_handle_type  client_handle,
  qmiLocXtraVersionCheckEnumT_v02  xtra_version
)
{
  int                        ret_val = LOC_API_GENERAL_FAILURE;
  loc_pa_module_data_s_type* pa_module_data_ptr = &(loc_middleware_data.pa_module_data);
  pdsm_pa_info_type          pa_info;
  boolean                    status = FALSE;

  if (client_handle < 0 || pa_module_data_ptr == NULL) 
  {
    LOC_MSG_ERROR( "loc_pa_set_xtra_version_check, client_handle = %d, pa_module_data_ptr = 0x%x", \
                    client_handle, pa_module_data_ptr, 0);
    return LOC_API_INVALID_PARAMETER;
  }

  memset((void *)&pa_info, 0, sizeof(pa_info));

  switch (xtra_version)
  { 
    case eQMI_LOC_XTRA_VERSION_CHECK_DISABLE_V02: 
     pa_info.xtra_version_check = PDSM_PA_XTRA_VERSION_CHECK_DISABLED;
     break;

    case eQMI_LOC_XTRA_VERSION_CHECK_AUTO_V02: 
     pa_info.xtra_version_check = PDSM_PA_XTRA_VERSION_CHECK_AUTO;
     break;

    case eQMI_LOC_XTRA_VERSION_CHECK_XTRA2_V02: 
     pa_info.xtra_version_check = PDSM_PA_XTRA_VERSION_CHECK_XTRA2;
     break;

    case eQMI_LOC_XTRA_VERSION_CHECK_XTRA3_V02: 
     pa_info.xtra_version_check = PDSM_PA_XTRA_VERSION_CHECK_XTRA3;
     break;

    default:
    {
      LOC_MSG_ERROR("loc_pa_set_xtra_version_check: invalid xtra_version %d",
                      xtra_version, 0, 0);
      return LOC_API_INVALID_PARAMETER;
    }
  }
  pa_module_data_ptr->pa_cmd = PDSM_PA_XTRA_VERSION_CHECK;
  pa_module_data_ptr->pa_cmd_type = PDSM_PA_CMD_SET_PARAM;

  status = pdsm_set_parameters (loc_pa_cmd_cb,
                                (void *) client_handle,
                                pa_module_data_ptr->pa_cmd,
                                &pa_info,
                                loc_middleware_data.pdapi_client_id);

  if (status == TRUE )
  {
    ret_val = LOC_API_SUCCESS;
  }

  LOC_MSG_MED ("loc_pa_set_xtra_version_check, xtra_version = %d ret_val = %d",
                xtra_version, ret_val, 0);

  return ret_val;

}

