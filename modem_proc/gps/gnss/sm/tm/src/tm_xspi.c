/*===========================================================================

                  TM - XSPI Module

DESCRIPTION
  This file contains implementations for TM XSPI module.
  Responsible for handling XSPI related functionality.

  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_xspi.c#1 $
$Author: mplcsds1 $
$Created:$
$Modified:$
===========================================================================*/

#include "tm_xspi.h"
#include "mgp_api.h"
#include "tm_pdapi_client.h"

#include "nv_items.h"
#include "gps_common.h"
#include "tm_nv.h"
/*
   static methods
*/

/**
@brief retrieves module config data
*/
static tm_xspi_module_data_type* tm_xspi_get_module_data(void);

/*
@brief handles injection timer expiry
*/
static void tm_xspi_handle_injection_timer_expiry(tm_xspi_module_data_type *p_module_data);

/*
@brief handles stream timer expiry
*/
static void tm_xspi_handle_stream_timer_expiry(tm_xspi_module_data_type *p_module_data);

/**
@brief starts the stream timer
@param p_module_data pointer to xspi configuration data
@return
*/
static void tm_xspi_start_stream_timer(tm_xspi_module_data_type *p_module_data);
/**
@brief stops the stream timer
@param p_module_data pointer to xspi configuration data
@return
*/
static void tm_xspi_stop_stream_timer(tm_xspi_module_data_type *p_module_data);


/**
@brief stops the injection timer
@param p_module_data pointer to xspi configuration data
@return
*/
static void tm_xspi_stop_injection_timer(tm_xspi_module_data_type *p_module_data);

/**
@brief starts the injection timer
@param p_module_data pointer to xspi configuration data
@return
*/
static void tm_xspi_start_injection_timer(tm_xspi_module_data_type *p_module_data);

/*
   implementations
*/

/**
@breif handles injection of xspi data received via pdapi
@param p_cmd pointer to data received from pdapi
@return void
*/
void tm_xspi_handle_status_injection(pdsm_xspi_cmd_s_type *p_cmd)
{
   tm_xspi_module_data_type *p_module_data = NULL;
   mgp_ReportSensorDataParm *p_sensor_data = NULL;

   p_module_data = tm_xspi_get_module_data();
   if(p_cmd)
   {
      pdsm_pd_cmd_err_e_type cmd_status = PDSM_PD_CMD_ERR_NOERR;

      if(tm_core_is_reset_location_service_in_progress())
      {        
        MSG_ERROR("TM_XSPI: Reset Location Service in progress",0,0,0);
        cmd_status = PDSM_PD_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S;
      }      
      else if(p_module_data && p_module_data->b_Enabled)
      {

         /* stop timers */
         tm_xspi_stop_injection_timer(p_module_data);
         tm_xspi_stop_stream_timer(p_module_data);

         p_sensor_data = &(p_module_data->z_sensorData);
         /* clear any previous cached data */
         memset(&(p_module_data->z_sensorData), 0, sizeof(p_module_data->z_sensorData));

         /* indicate to MGP that we're sending data for xspi */
         p_sensor_data->e_SensorDataType = C_MGP_EXTERNAL_DATA_MOTION;

         /* cap confidence unless unknown*/
         if(MGP_EXTERNAL_DATA_MOTION_CONFIDENCE_UNKNOWN != p_cmd->xspi_data.u_confidence)
         {
            p_sensor_data->u_Confidence =
               MIN(TM_XSPI_MAX_DATA_MOTION_CONFIDENCE, p_cmd->xspi_data.u_confidence);
         }
         else
         {
            /* allow unknown confidence to pass through */
            p_sensor_data->u_Confidence = p_cmd->xspi_data.u_confidence;
         }

         switch(p_cmd->xspi_data.q_state)
         {
         case PDSM_PD_EXTERNAL_SPI_STATE_UNKNOWN:
            p_sensor_data->q_Value = (uint32) C_MGP_EXTERNAL_DATA_MOTION_UNKNOWN;
            break;
         case PDSM_PD_EXTERNAL_SPI_STATE_STATIONARY:
            p_sensor_data->q_Value = (uint32) C_MGP_EXTERNAL_DATA_MOTION_STATIONARY;
            break;
         case PDSM_PD_EXTERNAL_SPI_STATE_NON_STATIONARY:
            p_sensor_data->q_Value = (uint32) C_MGP_EXTERNAL_DATA_MOTION_NON_STATIONARY;
            break;

         default:
            MSG_HIGH("unknown data motion state [%d]", p_cmd->xspi_data.q_state, 0, 0);
            p_sensor_data->q_Value = (uint32) C_MGP_EXTERNAL_DATA_MOTION_UNKNOWN;
            break;

         }

         /* update time-stamp on the xspi data */
         p_sensor_data->t_TimeTickMsec = cgps_TimeTickGetMsec();

         /* start injection timer */
         tm_xspi_start_injection_timer(p_module_data);

         /* start streaming timer */
         tm_xspi_start_stream_timer(p_module_data);

         /* pass on data to MGP if it has requested XSPI injection */
         if(p_module_data->b_TimerInjectionEnabled)
         {
            mgp_ReportSensorData(p_sensor_data);
         }
      }
      else if(!p_module_data)
      {
         MSG_ERROR("module data is NULL!", 0, 0, 0);
         cmd_status = PDSM_PD_CMD_ERR_INVALID_PARAM; // should not happen
      }
      else //!b_enabled
      {
         MSG_HIGH("module is not enabled", 0, 0, 0);
         cmd_status = PDSM_PD_CMD_ERR_FEATURE_NOT_ENABLED;
      }

      /* issue callback with appropriate status */
      if(p_cmd->cmd_cb_func)
      {
         p_cmd->cmd_cb_func(p_cmd->client_data_ptr, PDSM_PD_CMD_INJECT_EXTERNAL_SPI, cmd_status);
      }
   }
   return;
}

/* module data accessor function */
/* if needed to change to dynamic allocation for module data,
all changes will be limited to this function only */
static tm_xspi_module_data_type* tm_xspi_get_module_data(void)
{
   /* enforce access policy only via this function by defining
   data as static to this function*/
   static tm_xspi_module_data_type mz_tm_xspi_config;

   /* have we initialized data yet ? */
   static boolean b_dataInitialized = 0;

   if(!b_dataInitialized)
   {
      b_dataInitialized = tm_xspi_init_module_data(&mz_tm_xspi_config);
   }

   return &mz_tm_xspi_config;
}

/**
@brief initializes given module data struct
@param p_config_data pointer to xsp module data to be initialized
@return FALSE on failure (e..g if p_config_data == NULL), TRUE otherwise
*/
boolean tm_xspi_init_module_data(tm_xspi_module_data_type* p_config_data)
{
   boolean b_retval = FALSE;
   if(p_config_data)
   {
      memset(p_config_data, 0, sizeof(*p_config_data));
      /* add other initializations here */

      /* creating timers */
      p_config_data->p_timerInjection =
         os_TimerCreateInCgpsTimerGroup((uint32)TM_XSPI_TIMER_INJECTION, (uint32)THREAD_ID_SM_TM);

      p_config_data->p_timerStream =
         os_TimerCreateInCgpsTimerGroup((uint32)TM_XSPI_TIMER_STREAM, (uint32)THREAD_ID_SM_TM);

      /* set sane defaults on user motion data */
      p_config_data->z_sensorData.e_SensorDataType = C_MGP_EXTERNAL_DATA_MOTION;
      p_config_data->z_sensorData.q_Value = (uint32) C_MGP_EXTERNAL_DATA_MOTION_UNKNOWN;
      p_config_data->z_sensorData.u_Confidence = (uint8) MGP_EXTERNAL_DATA_MOTION_CONFIDENCE_UNKNOWN;

      b_retval = TRUE;
   }
   return b_retval;
}

/**
@brief enables the xspi feature. When enabled PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION
       is raised when MGP requests external SPI data
@param
@return
*/
void tm_xspi_enable(void)
{
   tm_xspi_module_data_type* p_module_data = tm_xspi_get_module_data();
   if(p_module_data)
   {
      /* enable only if enabled in NV */
      p_module_data->b_Enabled = p_module_data->b_xspiNvEnabled;
   }

   MSG_MED("XSPI state = %d", (p_module_data ? p_module_data->b_Enabled : 0), 0, 0);
}

/**
@brief disables the xspi feature. When disabled PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION
       is NOT raised even if MGP requests external SPI data
@param
@return
*/
void tm_xspi_disable(void)
{
   tm_xspi_module_data_type* p_module_data = tm_xspi_get_module_data();
   if(p_module_data)
   {
      p_module_data->b_Enabled = FALSE;

      tm_xspi_stop_stream_timer(p_module_data);
      tm_xspi_stop_injection_timer(p_module_data);
      /* add other cleanups here */
   }
}

/**
@brief translates injection requests from MGP to PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION events
@param p_reqData pointer to request data denote requested stream mode (on / off)
@return
*/
void tm_xspi_handle_injection_request(tm_xspi_request_type* p_reqData)
{
   tm_pdapi_pd_event_s_type   event_info;
   tm_xspi_module_data_type   *p_module_data = NULL;

   /* Init */
   memset(&event_info, 0, sizeof(event_info));

   p_module_data = tm_xspi_get_module_data();

   if(p_reqData && p_module_data && p_module_data->b_Enabled)
   {
      event_info.e_pd_event = PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION;

      /* We don't know which client requested this, so we set it to -1.
       tm_pdapi_pd_event_callback() substitutes this with the cached client id*/
      event_info.pd_event_data.pd_info_type.client_id = -1;

      switch(p_reqData->q_stream_status)
      {
      case TM_XSPI_STREAM_STATUS_ENABLE:
         MSG_MED("TM_XSPI_STREAM_STATUS_ENABLE", 0, 0, 0);
         event_info.pd_event_data.pd_info_type.pd_info.xspi_data.q_stream_status =
            (uint32) PDSM_PD_EXTERNAL_SPI_STREAM_STATUS_ENABLE;

         /* set "unknown" data motion */
         p_module_data->z_sensorData.e_SensorDataType = C_MGP_EXTERNAL_DATA_MOTION;
         p_module_data->z_sensorData.q_Value = (uint32) C_MGP_EXTERNAL_DATA_MOTION_UNKNOWN;
         p_module_data->z_sensorData.u_Confidence = (uint8) MGP_EXTERNAL_DATA_MOTION_CONFIDENCE_UNKNOWN;

         /* allow timer based injections */
         p_module_data->b_TimerInjectionEnabled = TRUE;

         /* start waiting for injection */
         tm_xspi_start_injection_timer(p_module_data);

         /* start stream with "unknown" status */
         tm_xspi_start_stream_timer(p_module_data);
         break;

      case TM_XSPI_STREAM_STATUS_DISABLE: /* intentional cascade */
      default:
         MSG_MED("TM_XSPI_STREAM_STATUS_DISABLE", 0, 0, 0);
         event_info.pd_event_data.pd_info_type.pd_info.xspi_data.q_stream_status =
            (uint32) PDSM_PD_EXTERNAL_SPI_STREAM_STATUS_DISABLE;

         /* dis-allow timer based injections */
         p_module_data->b_TimerInjectionEnabled = FALSE;

         /* reset cached data to unknown */
         p_module_data->z_sensorData.e_SensorDataType = C_MGP_EXTERNAL_DATA_MOTION;
         p_module_data->z_sensorData.q_Value = (uint32) C_MGP_EXTERNAL_DATA_MOTION_UNKNOWN;
         p_module_data->z_sensorData.u_Confidence = (uint8) MGP_EXTERNAL_DATA_MOTION_CONFIDENCE_UNKNOWN;

         /* stop all active timers */
         tm_xspi_stop_injection_timer(p_module_data);
         tm_xspi_stop_stream_timer(p_module_data);
         break;
      }

      /* raise event to request an injection */
      MSG_MED("TM_XSPI: raise injection request event", 0, 0, 0);
      tm_pdapi_pd_event_callback(&event_info, TM_PDAPI_SESS_TYPE_NONE);

   }
   return;
}

/**
@brief handles expiration of various timers
@param timer_data pointer to expired timer information
@return
*/
void tm_xspi_handle_timer_expiry(const os_TimerExpiryType *p_timerData)
{
  tm_xspi_module_data_type   *p_module_data = NULL;

  p_module_data = tm_xspi_get_module_data();
  if(p_timerData && p_module_data)
  {
    switch(p_timerData->q_TimerId)
    {
      case TM_XSPI_TIMER_INJECTION:
        tm_xspi_handle_injection_timer_expiry(p_module_data);
      break;

      case TM_XSPI_TIMER_STREAM:
        tm_xspi_handle_stream_timer_expiry(p_module_data);
      break;

      default:
        MSG_ERROR("TM_XSPI: Uknown timer expired", 0, 0, 0);
      break;
    }
  }
  else
  {
    MSG_ERROR("Can not process null timer msg", 0, 0, 0);
  }
} /* end tm_xspi_handle_timer_expiry() */

/*
@brief handles injection timer expiry.
       The injection timer is started whenever XSPI module
       receives a request for external motion data from MGP.
       (See tm_xspi_handle_injection_request()). If data is
       injected before this timer expires, it is reset. Else,
       motion state is changed to unknown within MGP. Timer is
       killed when injection is terminated.
@param p_module_data
         pointer to data used by xspi module. Must NOT
         be NULL
@return
*/
void tm_xspi_handle_injection_timer_expiry(tm_xspi_module_data_type *p_module_data)
{
   if(p_module_data)
   {
      MSG_MED("TM_XSPI: tm_xspi_handle_injection_timer_expiry()", 0, 0, 0);

      /* stop stream timer */
      tm_xspi_stop_stream_timer(p_module_data);

      /* set "unknown" data motion */
      p_module_data->z_sensorData.e_SensorDataType = C_MGP_EXTERNAL_DATA_MOTION;
      p_module_data->z_sensorData.q_Value = (uint32) C_MGP_EXTERNAL_DATA_MOTION_UNKNOWN;
      p_module_data->z_sensorData.u_Confidence = (uint8) TM_XSPI_MAX_DATA_MOTION_CONFIDENCE;
      p_module_data->z_sensorData.t_TimeTickMsec = cgps_TimeTickGetMsec();

      /* restart stream timer only if feature is enabled */
      if(p_module_data->b_Enabled && p_module_data->b_TimerInjectionEnabled)
      {
         /* inject cached data */
         MSG_MED("TM_XSPI: injecting unknown status to MGP", 0, 0, 0);
         mgp_ReportSensorData(&(p_module_data->z_sensorData));
         tm_xspi_start_stream_timer(p_module_data);
      }

   }
}

/*
@brief handles stream timer expiry.
       MGP expects a regular stream of external stationary position
       information. However, clients may inject this only on state change.
       The stream timer is used to fake a regular stream to MGP. This
       timer is started when XSPI is first injected and remains active
       till new data is injected or till XSPI injection is terminated.

@param p_module_data
         pointer to data used by xspi module. Must NOT
         be NULL
@return
*/
void tm_xspi_handle_stream_timer_expiry(tm_xspi_module_data_type *p_module_data)
{
   MSG_MED("TM_XSPI: tm_xspi_handle_stream_timer_expiry()", 0, 0, 0);
   if(p_module_data && p_module_data->b_Enabled && p_module_data->b_TimerInjectionEnabled)
   {
      MSG_MED("TM_XSPI: injecting cached data with confidence = %d, mode = %d",
              p_module_data->z_sensorData.u_Confidence,
              p_module_data->z_sensorData.q_Value,
              0);

      /* update cached data */
      p_module_data->z_sensorData.t_TimeTickMsec = cgps_TimeTickGetMsec();

      /* inject cached data */
      mgp_ReportSensorData(&(p_module_data->z_sensorData));

      /* let's restart the timer. timer will be killed when
      injection requests are terminated. see tm_xspi_handle_injection_request() */
      tm_xspi_start_stream_timer(p_module_data);
   }
}


/**
@brief initialize xspi module data and read appropriate
       variables from NV RAM
*/
void tm_xspi_init_and_read_nv(void)
{
   nv_item_type       nv_write_buffer;
   nv_item_type       nv_read_buffer;
   tm_xspi_module_data_type   *p_module_data = NULL;
   static volatile boolean b_DebugNvWriteFlag = 0; /* temp debug helper. delete when checking in */

   p_module_data = tm_xspi_get_module_data();

   if(!p_module_data)
   {
      MSG_ERROR("no memory to initialize module ", 0, 0, 0);
   } /* end else(p_module_data)*/
   else
   {
      if(!tm_xspi_init_module_data(p_module_data))
      {
         MSG_HIGH("unable to initialize module data", 0, 0, 0);
      }

      /* read enablement NV */
      if(!sm_nv_read(NV_CGPS_SENSORS_ENABLE_I, (void *) &nv_read_buffer))
      {
         /* failed to read NV item, so assign defaults and write back to NV */
         p_module_data->b_xspiNvEnabled = FALSE;
      }
      else
      {
         p_module_data->b_xspiNvEnabled =
            ((nv_read_buffer.cgps_sensors_enable & TM_XSPI_NV_ENABLED_FLAG) != 0);
      }

      /* disable by default. To enable call tm_xspi_enable() */
      p_module_data->b_Enabled = FALSE;

      if(!sm_nv_read(NV_GNSS_XSPI_INJECTION_TIMEOUT_I, &nv_read_buffer))
      {
         /* failed to read NV item, so assign defaults and write back to NV */
         p_module_data->q_InjectionWaitPeriod = TM_XSPI_DEFAULT_INJECTION_TIMEOUT;

         /* set default values to NV */
         nv_write_buffer.gnss_xspi_injection_timeout = (uint8) p_module_data->q_InjectionWaitPeriod;
         if(b_DebugNvWriteFlag)
         {
            (void)tm_core_nv_write(NV_GNSS_XSPI_INJECTION_TIMEOUT_I,(void*)&nv_write_buffer );
         }
      }
      else
      {
         p_module_data->q_InjectionWaitPeriod =
            TM_XSPI_CONVERT_SECONDS_TO_MSECS(nv_read_buffer.gnss_xspi_injection_timeout);
      }

      /* failed to read NV item, so assign defaults and write back to NV */
      p_module_data->q_StreamPeriod = TM_XSPI_DEFAULT_STREAM_TIMEOUT;

   }
} /* end tm_xspi_init_and_read_nv() */

/**
@brief stops the stream timer
@param p_module_data pointer to xspi configuration data
@return
*/
void tm_xspi_stop_stream_timer(tm_xspi_module_data_type *p_module_data)
{
   if(p_module_data && p_module_data->p_timerStream)
   {
      if(!os_TimerStop(p_module_data->p_timerStream))
      {
         MSG_ERROR("unable to stop stream timer", 0, 0, 0);
      }
   }
} /* end tm_xspi_stop_stream_timer() */

/**
@brief starts the stream timer
@param p_module_data pointer to xspi configuration data
@return
*/
void tm_xspi_start_stream_timer(tm_xspi_module_data_type *p_module_data)
{
   /* start stream timer only if non-zero stream period */
   if(p_module_data && p_module_data->p_timerStream && p_module_data->q_StreamPeriod)
   {
      if(!os_TimerStart(p_module_data->p_timerStream, p_module_data->q_StreamPeriod, 0))
      {
         MSG_ERROR("unable to start stream timer", 0, 0, 0);
      }
   }
} /* end tm_xspi_start_stream_timer() */

/**
@brief starts the injection timer
@param p_module_data pointer to xspi configuration data
@return
*/
void tm_xspi_start_injection_timer(tm_xspi_module_data_type *p_module_data)
{
   /* start injection timer only if non-zero injection wait period is specified */
   if(p_module_data && p_module_data->p_timerInjection && p_module_data->q_InjectionWaitPeriod)
   {
      if(!os_TimerStart(p_module_data->p_timerInjection, p_module_data->q_InjectionWaitPeriod, 0))
      {
         MSG_ERROR("unable to start injection timer", 0, 0, 0);
      }
   }
} /* end tm_xspi_start_injection_timer() */

/**
@brief stops the injection timer
@param p_module_data pointer to xspi configuration data
@return
*/
void tm_xspi_stop_injection_timer(tm_xspi_module_data_type *p_module_data)
{
   if(p_module_data && p_module_data->p_timerInjection)
   {
      if(!os_TimerStop(p_module_data->p_timerInjection))
      {
         MSG_ERROR("unable to stop injection timer", 0, 0, 0);
      }
   }
} /* end tm_xspi_stop_injection_timer() */

