/******************************************************************************
  @file:  loc_api.c
  @brief: Location Middleware task

  DESCRIPTION
  This module defines the loc API routines

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Copyright (c) 2013-2014 QUALCOMM Atheros, Inc.
  All Rights Reserved. 
  Copyright (c) 2014 QUALCOMM Atheros, Inc.
  All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. 
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
01/11/16   yh       LocMW refactoring
06/11/15   sjk      LB 2.0 Integration
09/05/14   ssu      GM LOWI Integration
04/15/14   yh       XTRA version check added   
03/10/14   ah       Added support for Set Premium Services Cfg & Get Avail WWAN Pos
05/02/11   pn       Added handler for OC_IOCTL_ERROR_ESTIMATE_CONFIG.
04/07/10   ns       Make sure that invalid client handle is returned if
                    the loc mw task is not up or if initialization
                    fails during loc_open   

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_api.c#1 $
$DateTime: 2016/12/13 07:59:45 $
======================================================================*/

#include "gps_variation.h"
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
#include "gnss_api.h"
#include "loc_geofence.h"
#include "loc_batching.h"
#include "loc_aon.h"

// Global data variable

// zero-initialize the global variable, so this flag loc_mw_task_started would be FALSE at start up,
// even before our lock-less RPC server gets to run. This works only if we live in the same process so data initialization is
// always done by compiler generated code before any C code.
// what we really want is {.loc_mw_task_started = FALSE}, but that would require a C99 compiler, which we may or may not have
// on various platforms.
loc_middleware_data_s_type loc_middleware_data = {0};

/*===========================================================================
FUNCTION loc_initialize_modules

DESCRIPTION
  This function initializes the sub-modules of loc-mw based on the events that 
  need to be generated

PARAMETERS
  event_reg_mask:          Events which the client is interested in receiving.
                           The modules which generate these events will be 
                           initialized.

DEPENDENCIES

RETURN VALUE
  TRUE: If modules are initialized successfully
  FALSE: If modules are not initialized successfully

SIDE EFFECTS
  race condition could happen if more than one LocAPI is calling loc_open.
  loc_middleware_data.crit_sec is mostly used for data protection 
  we need some mutex in safe guarding state machines or procedures, 
  which implies redesigning of LocMW

===========================================================================*/

static boolean loc_initialize_modules (const loc_event_mask_type event_reg_mask)
{
   boolean result = FALSE;

   // poor man's exception handling: do...while with break
   do
   {
       LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);

       // Location sub-module will be initialzied when the first location client
       // registers for the corresponding event
       if ((event_reg_mask & LOC_EVENT_PARSED_POSITION_REPORT) ||
           (event_reg_mask & LOC_EVENT_NMEA_1HZ_REPORT) ||
           (event_reg_mask & LOC_EVENT_NMEA_POSITION_REPORT) ||
           (event_reg_mask & LOC_EVENT_WPS_NEEDED_REQUEST)
          )
       {
          if (loc_middleware_data.pd_module_initialized == FALSE)
          {
             LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
             if(FALSE == loc_pd_init())
             {
                break;
             }
             LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
             loc_middleware_data.pd_module_initialized = TRUE;
          }
    
          if (loc_middleware_data.pa_module_initialized  == FALSE)
          {     
             LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
             if(FALSE == loc_pa_init())
             {
                break;
             }
             LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
             loc_middleware_data.pa_module_initialized = TRUE;
          }
       }
    
       if (event_reg_mask & LOC_EVENT_NMEA_POSITION_REPORT ||
           event_reg_mask & LOC_EVENT_NMEA_1HZ_REPORT )
       {
          if (loc_middleware_data.nmea_module_initialized == FALSE)
          {
             LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
             if(FALSE == loc_pd_init_nmea())
             {
                break;
             }
             LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
             loc_middleware_data.nmea_module_initialized = TRUE;
          }
       }
    
       if (event_reg_mask & LOC_EVENT_LOCATION_SERVER_REQUEST)
       {
          if (loc_middleware_data.conn_module_initialized == FALSE)
          {
             LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
             if(FALSE == loc_conn_init())
             {
                break;
             }
             LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
             loc_middleware_data.conn_module_initialized = TRUE;
          }
       }
    
       if (event_reg_mask & LOC_EVENT_ASSISTANCE_DATA_REQUEST)
       {
           if (loc_middleware_data.xtra_module_initialized == FALSE)
           {
              LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
              if(FALSE == loc_xtra_init())
              {
                break;
              }
              LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
              loc_middleware_data.xtra_module_initialized = TRUE;
           }
       }
    
       if (event_reg_mask & LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST)
       {
          if (loc_middleware_data.ni_module_initialized == FALSE)
          {
             LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
             if(FALSE == loc_ni_init())
             {
               break;
             }
             LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
             loc_middleware_data.ni_module_initialized = TRUE;
          }
       }
    
       if (event_reg_mask & LOC_EVENT_WPS_NEEDED_REQUEST)
       {
          if (loc_middleware_data.wifi_module_initialized == FALSE)
          {
             LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
             loc_middleware_data.wifi_module_initialized = locWifi_Init();
             LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
          }
       }
    
       if (loc_middleware_data.pa_module_initialized  == FALSE)
       {     
          LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
          if(FALSE == loc_pa_init())
          {
            break;
          }
          LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
          loc_middleware_data.pa_module_initialized = TRUE;
       }

       if (loc_middleware_data.geofence_module_initialized == FALSE)
       {
          LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
          if(FALSE == locGeofenceInit())
          {
             break;
          }
          LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
          loc_middleware_data.geofence_module_initialized = TRUE;
       }

       if (loc_middleware_data.aon_module_initialized == FALSE)
       {
          LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
          if(FALSE == locAonInit())
          {
             break;
          }
          LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
          loc_middleware_data.aon_module_initialized = TRUE;
       }
       LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);

       // finally, we're done with initialization
       result = TRUE;
   }
   while(0);

   return result;
}

/*===========================================================================
FUNCTION loc_open2

DESCRIPTION
  This function connects a location client to the location engine.
  If connection is successful, it will return a handle which the location client
  should use for future location operations. If connection fails, the return 
  value will be LOC_CLIENT_HANDLE_INVALID.

  This is different from loc_open2, as it does not require the event-
  registration mask.

PARAMETERS
  event_callback:          a function pointer which will be invoked by the GPS
                           server when an event of interest happens.

DEPENDENCIES

RETURN VALUE
  -1 (LOC_CLIENT_HANDLE_INVALID) if connection fails.
  Otherwise, a number >= 0.

SIDE EFFECTS

===========================================================================*/

loc_client_handle_type loc_open2
(
   loc_event_cb_f_type    *event_callback
)
{
   loc_client_handle_type client_handle = LOC_CLIENT_HANDLE_INVALID;

   // If location middleware task hasn't started return invalid handle
   if(loc_middleware_initialized () != TRUE)
   {
      return LOC_CLIENT_HANDLE_INVALID;
   }

   // try to initialize all modules (except Conn) at client registration
   // this slightly reduces complexity when client shall want to change 
   // event registration later. loc_conn will be initialized later, if
   // the client indicates that it can process LOCATION_SERVER requests.
   if(TRUE == loc_initialize_modules((loc_event_mask_type) ~LOC_EVENT_LOCATION_SERVER_REQUEST))
   {
      client_handle = loc_client_init ((loc_event_mask_type)LOC_EVENT_IOCTL_REPORT, event_callback);
   }
   return client_handle;
}

/*===========================================================================
FUNCTION loc_close

DESCRIPTION
  This function discconect the client from the location engine.

PARAMETERS
  handle:  the handle returned by loc_open

DEPENDENCIES

RETURN VALUE
  the closed client handle

SIDE EFFECTS

===========================================================================*/
int32 loc_close(loc_client_handle_type handle)
{
   int32 ret_val;
   loc_client_info_s_type *client_ptr = NULL;
   
   client_ptr = loc_client_find_handle(handle);

   if (client_ptr == NULL)
   {
      LOC_MSG_ERROR("loc_close, invalid client handle %d", handle,0, 0);
      ret_val = LOC_API_INVALID_HANDLE;
      return ret_val;
   }

   /* first stop the session */
   (void) loc_stop_fix (handle);

   ret_val = loc_client_release (handle);

   LOC_MSG_HIGH ("loc_close handle %d returned %d", handle, ret_val, 0);
   return ret_val;
}

/*===========================================================================
FUNCTION loc_close

DESCRIPTION
  This function discconects the client from the location engine.

PARAMETERS
  handle:  the handle returned by loc_open

DEPENDENCIES

RETURN VALUE
  On success: it returns 0 (LOC_API_SUCCESS).
  On failure, it will return one-zero error code.

SIDE EFFECTS

===========================================================================*/
int32 loc_start_fix
(
  loc_client_handle_type handle
)
{
   int                       ret_val;

   ret_val = loc_client_start_fix (handle);
   LOC_MSG_HIGH ("loc_start_fix, client = %d, ret_val = %d", handle, ret_val, 0);

   return ret_val;
}

/*===========================================================================
FUNCTION loc_stop_fix

DESCRIPTION
  This function stops positioning process for this client. If client is not
  currently requesting fixes, it will return LOC_API_SUCCESS.

PARAMETERS
  handle:  the handle returned by loc_open

DEPENDENCIES

RETURN VALUE
  On success: it returns 0 (LOC_API_SUCCESS).
  On failure, it will return one-zero error code.

SIDE EFFECTS

===========================================================================*/
int32 loc_stop_fix
(
  loc_client_handle_type handle
)
{
   int  ret_val;

   ret_val = loc_client_stop_fix (handle);
   LOC_MSG_HIGH ("loc_stop_fix, client = %d, ret_val = %d", handle, ret_val, 0);
   return ret_val;
}

/*===========================================================================
FUNCTION loc_error_estimate_config

DESCRIPTION
  This helper function configures the error estimate settings.

PARAMETERS
  e_config:  the desired setting

DEPENDENCIES

RETURN VALUE
  LOC_API_SUCCESS            - error estimate settings successfully applied
  LOC_API_INVALID_PARAMETER  - the given settings were not supported.
  LOC_API_GENERAL_FAILURE    - error estimate settings were not applied

SIDE EFFECTS

===========================================================================*/
static int loc_error_estimate_config(loc_error_estimate_config_e_type e_config)
{
   int ret_val = LOC_API_SUCCESS;
   uint32 q_LowErrEstCfg = 0;

   switch (e_config)
   {
      case LOC_ERROR_ESTIMATE_CONFIG_SET:
         q_LowErrEstCfg = 1;
         break;
      case LOC_ERROR_ESTIMATE_CONFIG_CLEAR:
         q_LowErrEstCfg = 0;
         break;
      default:
      case LOC_ERROR_ESTIMATE_MAX:
         ret_val = LOC_API_INVALID_PARAMETER;
   }

   if ((LOC_API_SUCCESS == ret_val) &&
       (FALSE == gnss_SetLowErrEst(q_LowErrEstCfg)))
   {
      ret_val = LOC_API_GENERAL_FAILURE;
   }

   return ret_val;
}

/*===========================================================================
FUNCTION loc_ioctl

DESCRIPTION
  This function is used to control the location engine and retrieve information
  from the engine.

  If loc_ioctl returns LOC_API_SUCCESS, then a callback event through the registered
  callback in loc_open will be fired to notify the final processing result. The event
  will be set to LOC_EVENT_IOCTL_REPORT and the event payload shall be interpreted
  as loc_ioctl_callback_s_type. The type in this payload will indicate the IOCTL
  type. The ioctl_status will indicate whether the loc_ioctl call is successful or not.
  If the loc_ioctl is used to retrieve information from the engine and the ioctl_status
  indicates success, then the requesting payload will be available in the callback.

DEPENDENCIES

RETURN VALUE
  On success: it returns 0 (LOC_API_SUCCESS).
  On failure, it will return one-zero error code.

SIDE EFFECTS

===========================================================================*/
int32 loc_ioctl
(
      loc_client_handle_type           handle,
      loc_ioctl_e_type                 ioctl_type,
      const loc_ioctl_data_u_type*     ioctl_data
)
{
   int ret_val = LOC_API_SUCCESS;

   // parameter validation
   if (loc_client_find_handle (handle) == NULL)
   {
      ret_val = LOC_API_INVALID_HANDLE;
   }

   // dispatching
   if( LOC_API_SUCCESS == ret_val )
   {
      switch (ioctl_type)
      {
      /* All the ioctls to be processed
         in "loc_client" module are handled in
         this block */
      case LOC_IOCTL_GET_API_VERSION:
      case LOC_IOCTL_SET_FIX_CRITERIA:
      case LOC_IOCTL_GET_FIX_CRITERIA:
      case LOC_IOCTL_SET_CUSTOM_PDE_SERVER_ADDR:
      case LOC_IOCTL_GET_CUSTOM_PDE_SERVER_ADDR:
      case LOC_IOCTL_SET_EVENT_REG_MASK:
      case LOC_IOCTL_GET_EVENT_REG_MASK:        
      case LOC_IOCTL_SET_OPERATION_MODE:
      case LOC_IOCTL_GET_OPERATION_MODE:
         ret_val = loc_client_process_ioctl (handle, ioctl_type, ioctl_data);
         break;
      
      /* All the ioctls to be processed
         in "loc_pa" module are handled in
         this block */   
      case LOC_IOCTL_SET_ENGINE_LOCK:
      case LOC_IOCTL_GET_ENGINE_LOCK:
      case LOC_IOCTL_SET_CDMA_PDE_SERVER_ADDR:
      case LOC_IOCTL_GET_CDMA_PDE_SERVER_ADDR:
      case LOC_IOCTL_SET_CDMA_MPC_SERVER_ADDR:
      case LOC_IOCTL_GET_CDMA_MPC_SERVER_ADDR:
      case LOC_IOCTL_SET_UMTS_SLP_SERVER_ADDR:
      case LOC_IOCTL_GET_UMTS_SLP_SERVER_ADDR:
      case LOC_IOCTL_SET_SBAS_CONFIG:
      case LOC_IOCTL_GET_SBAS_CONFIG:
      case LOC_IOCTL_GET_NMEA_TYPES:
      case LOC_IOCTL_SET_NMEA_TYPES:
      case LOC_IOCTL_SET_LBS_APN_PROFILE:
      case LOC_IOCTL_SET_XTRA_APN_PROFILE:        
      case LOC_IOCTL_GET_LBS_APN_PROFILE:
      case LOC_IOCTL_GET_XTRA_APN_PROFILE:
      case LOC_IOCTL_DELETE_ASSIST_DATA:
      case LOC_IOCTL_SET_ON_DEMAND_LPM:
      case LOC_IOCTL_GET_ON_DEMAND_LPM:
      case LOC_IOCTL_ACCESS_EFS_DATA:
      case LOC_IOCTL_SET_DATA_ENABLE:
      case LOC_IOCTL_SET_SUPL_VERSION:
      case LOC_IOCTL_GET_SUPL_VERSION:
      case LOC_IOCTL_SET_XTRA_T_SESSION_CONTROL:
      case LOC_IOCTL_GET_XTRA_T_SESSION_CONTROL:
      case LOC_IOCTL_SET_SUPL_SECURITY:
      case LOC_IOCTL_GET_SUPL_SECURITY:
      case LOC_IOCTL_SET_PE_CONFIG:
      case LOC_IOCTL_GET_PE_CONFIG:
      case LOC_IOCTL_SET_LPP_CONFIG:
      case LOC_IOCTL_GET_LPP_CONFIG:  
      case LOC_IOCTL_SET_ASSISTED_GLO_CONFIG:
      case LOC_IOCTL_GET_ASSISTED_GLO_CONFIG:
      case LOC_IOCTL_SET_EMERGENCY_PROTOCOL:
      case LOC_IOCTL_GET_EMERGENCY_PROTOCOL:
      case LOC_IOCTL_SET_SUPL_HASH_ALGO:
      case LOC_IOCTL_GET_SUPL_HASH_ALGO:
      case LOC_IOCTL_SET_SUPL_TLS_VERSION:
      case LOC_IOCTL_GET_SUPL_TLS_VERSION:
      case LOC_IOCTL_SET_WIFI_SCAN_INJECT_TIMEOUT:
      case LOC_IOCTL_GET_WIFI_SCAN_INJECT_TIMEOUT:
      case LOC_IOCTL_SET_PREMIUM_SERVICES_CONFIG:
      case LOC_IOCTL_SET_XTRA_VERSION_CHECK:
      case LOC_IOCTL_SET_GNSS_CONSTELL_REPORT_CONFIG:
      case LOC_IOCTL_DELETE_GNSS_SERVICE_DATA:
         ret_val = loc_pa_process_ioctl (handle, ioctl_type, ioctl_data);
         break;
      
      /* All the ioctls to be processed
         in "loc_ni" module are handled in
         this block */
      case LOC_IOCTL_INFORM_NI_USER_RESPONSE:
      case LOC_IOCTL_SET_VX_CONFIG:
      case LOC_IOCTL_GET_VX_CONFIG:
         ret_val = loc_ni_process_ioctl (handle, ioctl_type, ioctl_data);
         break;

      /* All the ioctls to be processed
         in "loc_xtra" module are handled in
         this block */
      case LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA:
      case LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_SOURCE:
      case LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_VALIDITY:
      case LOC_IOCTL_SET_PREDICTED_ORBITS_DATA_AUTO_DOWNLOAD:
      case LOC_IOCTL_INJECT_UTC_TIME:
         ret_val = loc_xtra_process_ioctl (handle, ioctl_type, ioctl_data);
         break;

      /* All the ioctls to be processed
         in "loc_pd" module are handled in
         this block */
      case LOC_IOCTL_INJECT_POSITION:
      case LOC_IOCTL_GET_BEST_AVAILABLE_POSITION:
      case LOC_IOCTL_GET_AVAILABLE_WWAN_POSITION:
      case LOC_IOCTL_GET_AVAILABLE_POSITION_FROM_SRC:
      case LOC_IOCTL_INJECT_GSM_CELL_INFO_REQ:
      case LOC_IOCTL_INJECT_NI_MESSAGE_REQ:
      case LOC_IOCTL_WWAN_OOS_NOTIFICATION_REQ:
      {
         ret_val = loc_pd_process_ioctl (handle, ioctl_type, ioctl_data);
         break;
      }

      /* All the ioctls to be processed
         in "loc_conn" module are handled in
         this block */
      case LOC_IOCTL_INFORM_SERVER_MULTI_OPEN_STATUS:
      case LOC_IOCTL_INFORM_SERVER_CLOSE_STATUS:
         ret_val = loc_conn_process_ioctl (handle, ioctl_type, ioctl_data);
         break;

      case LOC_IOCTL_ERROR_ESTIMATE_CONFIG:
         ret_val = loc_error_estimate_config(ioctl_data->error_estimate_config);
         break;

      default:
         ret_val = LOC_API_UNSUPPORTED;
         break;
      }
   }

   LOC_MSG_HIGH ("loc_ioctl handle = 0x%x, type = %d, ret_val = %d", handle, ioctl_type, ret_val);
   return ret_val;
}


