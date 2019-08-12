/*============================================================================
  @file therm_monitor.c

  Simple thermal monitor implementation.

  Copyright (c) 2011,2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/thermal/src/common/therm_monitor.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "rex.h"
#include "task.h"
#include "timer.h"
#include "timetick.h"
#include "time_remote.h"
#include "nv.h"
#include "stdarg.h"
#include "DALStdDef.h"
#include "CoreVerify.h"
#include <stringl/stringl.h>
#include "therm_monitor.h"
#include "npa.h"
#include "npa_resource.h"
#include "therm_diag.h"
#include "therm_log.h"
#include "therm_v.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#define GCF_POLLING_PERIOD           10000
#define GCF_MAX_POLLING_ATTEMPTS     12

static const char *therm_monitor_client_name = "Therm Monitor";

static const char *therm_monitor_sensor_event = "Sensor Change Event";

extern const char *therm_monitor_res_avail[];

extern npa_node_definition therm_mitigate_node;

extern rex_tcb_type thermal_tcb;

extern const unsigned int therm_monitor_res_count;

typedef struct {
  /* GCF mode check polling timer */
  timer_type        polling_timer;

  /* GCF client handle */
  npa_client_handle npa_client_handle[MAX_NUM_THERM_MITIGATION_RES];

  /* GCF mode */
  boolean           disable_tm_algo;

  /* GCF polling attempts */
  uint32            polling_attempts;

  /* GCF polling period in ms */
  uint32            polling_period;

  /* GCF max polling attempts */
  uint32            max_attempts;

} therm_monitor_gcf_type;


/* Access to global variables for this module */
static therm_monitor_type     therm_monitor_data;

static therm_monitor_gcf_type therm_monitor_gcf;


/*===========================================================================

                     INTERNAL FUNCTION DECLARATIONS

===========================================================================*/

static void therm_monitor_event_cb( void         *context,
                                    unsigned int  event_type,
                                    void         *data,
                                    unsigned int  data_size );

static void therm_monitor_res_avail_cb( void         *context,
                                        unsigned int  event_type,
                                        void         *data,
                                        unsigned int  data_size );

/**
  @brief therm_monitor_nv_read

  This function is used to read the value of an NV item.

  @param  : nv_item:Item to read
            item_ptr:Location to put the data read

  @return : TRUE if read succeeded, FALSE otherwise.

*/
static boolean therm_monitor_nv_read
(
  nv_items_enum_type nv_item,
  nv_item_type      *item_ptr
)
{
  boolean           ret_val = TRUE;
  nv_cmd_type       cmd_ptr;

  cmd_ptr.item       = nv_item;
  cmd_ptr.cmd        = NV_READ_F;
  cmd_ptr.data_ptr   = item_ptr;
  cmd_ptr.tcb_ptr    = rex_self();
  cmd_ptr.sigs       = THERM_NV_RD_WR_SIG;
  cmd_ptr.done_q_ptr = NULL;

  nv_cmd( &cmd_ptr );

  /* Block till NV signal is set. */
  (void)rex_wait( THERM_NV_RD_WR_SIG );
  (void)rex_clr_sigs( rex_self(), THERM_NV_RD_WR_SIG );

  if ( cmd_ptr.status != NV_DONE_S )
  {
    /* NV read failure. */
    ret_val = FALSE;;
  }

  return ret_val;
}

/**
  @brief therm_monitor_gcf_mode_test_handler

  This function is used determine whether the device is in GCF test mode.
  The function will only read NV_GPRS_ANITE_GCF_I for
  therm_monitor_gcf.max_attempts attempts. After the
  therm_monitor_gcf.max_attempts attempts, it should be safe to
  assume that we are not in the GCF test mode.  The reason
  behind the count limit is that the time penalty of an NV item
  access is around 1ms, and it is the simplest method to avoid
  using sclk ticks and accounting for rollover.  Will limit
  mitigation to normal state.

  @param  : None

  @return : None

*/
static void therm_monitor_gcf_mode_test_handler( void )
{
  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "GCF test mode, polling attempts %d",
                   therm_monitor_gcf.polling_attempts);
  if (therm_monitor_gcf.polling_attempts < therm_monitor_gcf.max_attempts)
  {
    therm_monitor_nv_read(NV_GPRS_ANITE_GCF_I,
                           (nv_item_type*)&therm_monitor_gcf.disable_tm_algo);
    therm_monitor_gcf.polling_attempts++;

    if (therm_monitor_gcf.disable_tm_algo)
    {
      int i;

      for (i = 0; (i < therm_mitigate_node.resource_count) &&
            (i < MAX_NUM_THERM_MITIGATION_RES); i++)
      {
        npa_issue_scalar_request(therm_monitor_gcf.npa_client_handle[i],
                                 0xffffffff);
      }

      /* Stop reading we are in GCF test mode. */
      therm_monitor_gcf.polling_attempts = therm_monitor_gcf.max_attempts;
      therm_log_printf(THERM_LOG_LEVEL_INFO, 0, "GCF test mode");
    }
    else
    {
      /* Start the GCF polling timer. */
      timer_set( &( therm_monitor_gcf.polling_timer ),
                 therm_monitor_gcf.polling_period , 0, T_MSEC);
    }
  }
}

/**
  @brief therm_monitor_event_cb

  Thermal monitor NPA sensor change event callback.

  @param  : context:Internal index of sensor.
            event_type:Ignored
            data:npa_event_data
            data_size:Ignored

  @return : None.

*/
static void therm_monitor_event_cb( void         *context,
                                    unsigned int  event_type,
                                    void         *data,
                                    unsigned int  data_size )
{
  uint32                          index          = (uint32)context;
  therm_monitor_sensor_info_type *sensor_ptr     = &therm_monitor_data.sensor[index];
  boolean                         state_change   = FALSE;
  npa_event_data                 *event_data_ptr = (npa_event_data*)data;
  int32                           temperature    = THERM_K_TO_C(event_data_ptr->state);
  uint32                          old_state      = sensor_ptr->current_state;


  switch (event_type)
  {
    case NPA_EVENT_THRESHOLD_LO:
      therm_log_printf(THERM_LOG_LEVEL_INFO, 3,
                       "Threshold Low Event \"%s\", Temp %d < thresh clr %d",
                       sensor_ptr->sensor_resource_path, temperature,
                       sensor_ptr->state_data[sensor_ptr->current_state].thresh_clr);
      while (sensor_ptr->current_state > 0)
      {
        --sensor_ptr->current_state;
        if ((sensor_ptr->state_data[sensor_ptr->current_state].thresh_clr <=
             temperature))
        {
          /* Scroll through all threshold ranges in case there was a temperature
             jump. */
          break;
        }
      }
      state_change = TRUE;
      break;
    case NPA_EVENT_THRESHOLD_HI:
      therm_log_printf(THERM_LOG_LEVEL_INFO, 3,
                       "Threshold High Event \"%s\", Temp %d > thresh %d",
                       sensor_ptr->sensor_resource_path, temperature,
                       sensor_ptr->state_data[sensor_ptr->current_state].thresh);
      while (sensor_ptr->current_state < (sensor_ptr->num_of_states - 1))
      {
        ++sensor_ptr->current_state;
        if ((sensor_ptr->state_data[sensor_ptr->current_state].thresh >=
             temperature))
        {
          /* Scroll through all threshold ranges in case there was a temperature
             jump. */
          break;
        }
      }
      state_change = TRUE;
      break;
    default:
      break;
  }

  if (state_change)
  {
    if (THERM_MONITOR_SET_MITIGATE_STATE ==
        sensor_ptr->state_data[sensor_ptr->current_state].action)
    {
      npa_issue_required_request(sensor_ptr->mitigate_state_handle,
                 sensor_ptr->state_data[sensor_ptr->current_state].action_info);
      therm_log_printf(THERM_LOG_LEVEL_INFO, 3, "\"%s\" mitigate req, old state %d, new state %d",
                       sensor_ptr->mitigate_state_resource_path,
                       old_state,
                       sensor_ptr->state_data[sensor_ptr->current_state].action_info);
    }

    npa_issue_required_request(sensor_ptr->sampling_handle,
                    sensor_ptr->state_data[sensor_ptr->current_state].sampling);

    /* Request the sampling period required for that sensor */
    therm_log_printf(THERM_LOG_LEVEL_INFO, 2, "\"%s\" sampling request %d ms",
                     sensor_ptr->sampling_resource_path,
                     sensor_ptr->state_data[sensor_ptr->current_state].sampling);

    npa_set_event_thresholds(sensor_ptr->sensor_event_handle,
      (npa_resource_state) THERM_C_TO_K(sensor_ptr->state_data[sensor_ptr->current_state].thresh_clr),
      (npa_resource_state) THERM_C_TO_K(sensor_ptr->state_data[sensor_ptr->current_state].thresh));

    therm_log_printf(THERM_LOG_LEVEL_INFO, 3, "\"%s\" thresh req, LOW: %dC, HI: %dC",
                     sensor_ptr->sensor_resource_path,
                     sensor_ptr->state_data[sensor_ptr->current_state].thresh_clr,
                     sensor_ptr->state_data[sensor_ptr->current_state].thresh);
  }
}

/**
  @brief therm_monitor_res_avail_cb

  Thermal monitor callback notifying NPA resources available.

  @param  : context:Ignored
            event_type:Ignored
            data:Ignored
            data_size:Ignored

  @return : None.

*/
static void therm_monitor_res_avail_cb( void         *context,
                                        unsigned int  event_type,
                                        void         *data,
                                        unsigned int  data_size )
{
  int i;

  for (i = 0; i < therm_monitor_data.num_valid_sensors; i++)
  {
    /* Create sensor sampling handle */
    therm_monitor_data.sensor[i].sampling_handle =
      npa_create_sync_client(therm_monitor_data.sensor[i].sampling_resource_path,
                             therm_monitor_client_name,
                             NPA_CLIENT_REQUIRED);
    CORE_VERIFY(therm_monitor_data.sensor[i].sampling_handle);

    npa_issue_required_request(therm_monitor_data.sensor[i].sampling_handle,
                               therm_monitor_data.sensor[i].state_data[0].sampling);

    /* Create mitigation client handle */
    therm_monitor_data.sensor[i].mitigate_state_handle =
      npa_create_sync_client(therm_monitor_data.sensor[i].mitigate_state_resource_path,
                             therm_monitor_client_name,
                             NPA_CLIENT_REQUIRED);
    CORE_VERIFY(therm_monitor_data.sensor[i].mitigate_state_handle);

    /* Create thermal events. */
    therm_monitor_data.sensor[i].sensor_event_handle =
       npa_create_event_cb(therm_monitor_data.sensor[i].sensor_resource_path,
                           therm_monitor_sensor_event,
                           NPA_TRIGGER_THRESHOLD_EVENT,
                           therm_monitor_event_cb,
                           (void *) i);
    CORE_VERIFY(therm_monitor_data.sensor[i].sensor_event_handle);

    npa_set_event_thresholds(therm_monitor_data.sensor[i].sensor_event_handle,
      (npa_resource_state) THERM_C_TO_K(therm_monitor_data.sensor[i].state_data[0].thresh_clr),
      (npa_resource_state) THERM_C_TO_K(therm_monitor_data.sensor[i].state_data[0].thresh));
  }
}

/**
  @brief therm_monitor_gcf_res_avail_cb

  Thermal monitor GCF callback notifying NPA resources
  available.

  @param  : context:Ignored
            event_type:Ignored
            data:Ignored
            data_size:Ignored

  @return : None.

*/
static void therm_monitor_gcf_res_avail_cb( void         *context,
                                        unsigned int  event_type,
                                        void         *data,
                                        unsigned int  data_size )
{
  int i;

  for (i = 0; (i < therm_mitigate_node.resource_count) &&
        (i < MAX_NUM_THERM_MITIGATION_RES); i++)
  {
    therm_monitor_gcf.npa_client_handle[i] =
      npa_create_sync_client(therm_mitigate_node.resources[i].name, "GCF MODE",
                             NPA_CLIENT_CUSTOM1);
  }

  timer_def( &( therm_monitor_gcf.polling_timer ), NULL,
             &thermal_tcb, THERM_GCF_SAMPLING_LOOP_TIMER_SIG, NULL, NULL );

  /* Start the GCF polling timer. */
  timer_set( &( therm_monitor_gcf.polling_timer ),
             therm_monitor_gcf.polling_period , 0, T_MSEC);
}

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/**
  @brief therm_monitor_process_sigs

  This function is used to process the signals received by the
  thermal Task.

  @param  : rcvd_sigs:Signals received by thermal task

  @return : None.

*/
void therm_monitor_process_sigs
(
  rex_sigs_type rcvd_sigs
)
{
  if ( rcvd_sigs & THERM_GCF_SAMPLING_LOOP_TIMER_SIG )
  {
    rex_clr_sigs( &thermal_tcb, THERM_GCF_SAMPLING_LOOP_TIMER_SIG );
    therm_monitor_gcf_mode_test_handler();
  }
}


/**
  @brief therm_monitor_init

  This function initializes the variables used the thermal monitor.

  @param  : None.

  @return : None.

*/
void therm_monitor_init( void )
{
  int i, j;

  /* Inititalize the global variable structure */
  memset( &( therm_monitor_data ), 0x0, sizeof( therm_monitor_type ) );

  for (i = 0; i < MAX_NUM_THERM_SENSORS; i++)
  {
    for (j = 0; j < MAX_NUM_THERM_STATES; j++)
    {
      therm_monitor_data.sensor[i].state_data[j].thresh = 528;
      therm_monitor_data.sensor[i].state_data[j].thresh_clr = -273;
      therm_monitor_data.sensor[i].state_data[j].action = THERM_MONITOR_NONE;
      therm_monitor_data.sensor[i].state_data[j].sampling = DEFAULT_SAMPLING_MS;
    }
  }

  therm_monitor_gcf.polling_period = GCF_POLLING_PERIOD;
  therm_monitor_gcf.max_attempts   = GCF_MAX_POLLING_ATTEMPTS;
}


/**
  @brief therm_monitor_after_task_start_init

  This function is used to perform necessary initialization after the thermal
  monitor task starts.

  @param  : None.

  @return : None.

*/
void therm_monitor_after_task_start_init( void )
{
  /* Load config from file */
  if (therm_monitor_load_config(&therm_monitor_data) == MONITOR_SUCCESS)
  {
    /* Create Res Available callback */
    npa_resources_available_cb( therm_monitor_res_count,
                                therm_monitor_res_avail,
                                therm_monitor_res_avail_cb,
                                NULL );
  }

  /* Create Res Available callback */
  npa_resources_available_cb( therm_monitor_res_count,
                              therm_monitor_res_avail,
                              therm_monitor_gcf_res_avail_cb,
                              NULL );
}

