/*============================================================================
  @file therm_npa_sensor.c

  Define NPA nodes representing sensors.  Provide polling functionality where
  needed.

  Copyright (c) 2011-2014 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/thermal/src/asic/8952/qdsp6/therm_npa_sensor.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "npa_resource.h"
#include "AdcInputs.h"
#include "DDIAdc.h"
#include "timer.h"
#include "CoreVerify.h"
#include "therm_diag.h"
#include "therm_log.h"
#include "stdarg.h"
#include <stringl/stringl.h>
#include "log.h"
#include "rex.h"
#include "therm_npa_sensor.h"
#include "therm_v.h"

/*=======================================================================

                  STATIC MEMBER / FUNCTION DECLARATIONS / DEFINITIONS

========================================================================*/

/* Use 300 K as default temp */
#define THERM_NPA_DEFAULT_TEMP 300
#define THERM_NPA_MAX_RES_TEMP 528

#define THERM_NPA_SENSOR_MAX 8
#define THERM_NPA_STR_MAX    64

#define THERM_K_TO_C(x) ((int32)(((int32)(x)) - 273))
#define THERM_C_TO_K(x) ((int32)(((int32)(x)) + 273))

#define THERM_NPA_BCL_WARNING          "therm_bcl_warning"
#define THERM_NPA_BCL_DEFAULT_WARNING  0

#define NPA_RESOURCE_AT_INDEX( node_ptr, index ) (&((node_ptr)->resources[(index)]))

typedef enum
{
  THERM_NPA_HKADC_SENSOR = 0,
  THERM_NPA_TSENS_SENSOR,
  THERM_NPA_LOGICAL_SENSOR,
  THERM_NPA_MAX_SENSOR = THERM_NPA_LOGICAL_SENSOR
} therm_npa_sensor_type;

typedef struct
{
  /* Diag log packet subsystem id */
  unsigned int subsystem_id;
  /* Diag log packet sensor id */
  unsigned int sensor_id;
  /* Units of measurement Eg. Degrees C, .001 Degrees C */
  const char  *units_of_measure;
  /* Nuber of units per whole integer value */
  unsigned int unit_per_whole;
  /* ADC input name */
  const char  *adc_input_str;
  /* NPA node name */
  const char  *npa_node_str;
  /* Therm sensor types */
  therm_npa_sensor_type sensor_type;
} therm_sensor_info;

/* Must edit if additional sensors are added */
therm_sensor_info therm_sensor_data[] = {
  {PA_THERM_SS, 0, "Degrees K", 1, ADC_INPUT_PA_THERM, "pa", THERM_NPA_HKADC_SENSOR},
  {PA_THERM_SS, 1, "Degrees K", 1, ADC_INPUT_PA_THERM1, "pa_1", THERM_NPA_HKADC_SENSOR},

  /* Place all logical resources at the end and physical at the beginning due to 
   * the way they are handled */
  {LOGICAL_THERM_SS, 0, "Warning Level", 1, THERM_NPA_BCL_WARNING, "modem_bcl_warning", THERM_NPA_LOGICAL_SENSOR}
};

/* Structure to maintain the internal state of the DCVS NPA members */
typedef struct
{
  npa_resource_state sampling_time;
} therm_npa_type;

static npa_resource_state therm_hk_adc_sensor_update_fcn( npa_resource *resource,
                                                          npa_client_handle client );

static npa_resource_state therm_bcl_max_update_fcn( npa_resource *resource,
                                                    npa_client_handle client );

static npa_resource_state therm_node_driver_fcn( npa_resource *resource,
                                                 npa_client *client,
                                                 npa_resource_state state );

static npa_resource_state therm_sampling_node_driver_fcn( npa_resource *resource,
                                                          npa_client *client,
                                                          npa_resource_state state );


/*=======================================================================

                  GLOBAL DEFINITIONS

========================================================================*/

/* Supplies sensor list for QMI request for thermal sensor device list.
   Must edit if additional thermal sensors are added. */
const char *therm_sensor_list[] =
{
  "pa",
  "pa_1",
  "modem_bcl_warning",
};

const unsigned int therm_sensor_list_count = ARR_SIZE(therm_sensor_list);

/* "/therm/sampling" node and resource */
static npa_resource_definition therm_sampling_resource[] =
{
  {
    "/therm/sampling",                  /* name of resource */
    "Milliseconds",                     /* Units of the resource */
    0xFFFFFFFF,                         /* Maximum value of resource */
    &npa_min_plugin,                    /* Plugin - only handles impulses */
    NPA_RESOURCE_DEFAULT,
    NULL,
    NULL
  }
};

/* Define the dependencies of our sampling resource.  Must edit if additional
   sensors are added */
static npa_node_dependency therm_sampling_deps[] =
{
  { "/therm/sensor/pa",                NPA_NO_CLIENT },
  { "/therm/sensor/pa_1",              NPA_NO_CLIENT },
  { "/therm/sensor/modem_bcl_warning", NPA_NO_CLIENT }
};


static npa_node_definition therm_sampling_node =
{
  "/node/therm/sampling",            /* Node name - info only */
  therm_sampling_node_driver_fcn,    /* Driver function for temperature */
  NPA_NODE_DEFAULT,                  /* No attributes */
  NULL,                              /* No User Data */
  NPA_ARRAY(therm_sampling_deps),    /* No Dependencies */
  NPA_ARRAY(therm_sampling_resource) /* Resources */
};

static npa_resource_state initial_sampling_state[] = {10000};

/* The plugin definition for HKADC /therm/<sensor> resource */
const npa_resource_plugin therm_hk_adc_sensor_plugin =
{
  therm_hk_adc_sensor_update_fcn,
  NPA_CLIENT_IMPULSE,                            /* Supported client types */
  NULL,                                          /* Create client function */
  NULL                                           /* Destroy client function */
};

/* The plugin definition for modem_bcl_warning resource - similar to npa max
 * plugin with added logging */
const npa_resource_plugin therm_sensor_max_plugin = 
{
  therm_bcl_max_update_fcn, 
  NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE,     /* Supported Client types */
  NULL,                                          /* Create client function */
  NULL                                           /* Destroy client function */
};

/* "therm" node and resource
   Must edit if additional sensors are added. 

   Place all actual physical resources together at the beginning and 
   logical/virtual resources at the end.
 */
static npa_resource_definition therm_sensor_resource[] =
{
  /* Physical Resources */
  {
    "/therm/sensor/pa",                        /* name of resource */
    "Degrees K",                               /* Units of the resource */
    THERM_NPA_MAX_RES_TEMP,                    /* Maximum value of resource */
    &therm_hk_adc_sensor_plugin,
    NPA_RESOURCE_DEFAULT,
    NULL,
    NULL
  },
  {
    "/therm/sensor/pa_1",                      /* name of resource */
    "Degrees K",                               /* Units of the resource */
    THERM_NPA_MAX_RES_TEMP,                    /* Maximum value of resource */
    &therm_hk_adc_sensor_plugin,
    NPA_RESOURCE_DEFAULT,
    NULL,
    NULL
  },

  /* Logical Resources */
  {
    "/therm/sensor/modem_bcl_warning",         /* name of resource */
    "Warning Level",                           /* Unit of the resource */
    100,                                       /* Maximum value of resource */
    &therm_sensor_max_plugin,                  /* Plugin - Max Aggregation */
    NPA_RESOURCE_DEFAULT,                      /* Resource attribute */
    (npa_user_data)THERM_NPA_LOGICAL_SENSOR,   /* Resource specific data */
    NULL                                       /* Default query function */
  }
};

/* Must edit if additional sensors are added */
static npa_resource_state initial_sensor_state[] =
{
  /* Initial state for physical resources */
  THERM_NPA_DEFAULT_TEMP,
  THERM_NPA_DEFAULT_TEMP,

  /* Initial state for logical resources */
  THERM_NPA_BCL_DEFAULT_WARNING,
};


npa_node_definition therm_sensor_node =
{
  "/node/therm/sensor",          /* Node name - info only */
  therm_node_driver_fcn,         /* Driver function for temperature */
  NPA_NODE_DEFAULT,              /* No attributes */
  NULL,                          /* No User Data */
  NPA_EMPTY_ARRAY,               /* No Dependencies */
  NPA_ARRAY(therm_sensor_resource) /* Resources */
};

static npa_client_handle sensor_client_handle[ARR_SIZE(therm_sensor_resource)];

/* Struct for allowing sync reads using ADC DAL interface. */
typedef struct
{
  AdcResultType *pAdcResult;
  DALSYSEventHandle hCallbackEvent;
  DALSYS_EVENT_OBJECT(callbackEventObject);
  DALSYSEventHandle hSignalEvent;
  DALSYS_EVENT_OBJECT(signalEventObject);
  DALSYSSyncHandle hSync;
  DALSYS_SYNC_OBJECT(syncObject);
} therm_npa_adc_sync_read_obj_type;

/* Internal struct for managing ADC DAL sensor init of read object */
typedef struct
{
  AdcResultType                   readResult;
  therm_npa_adc_sync_read_obj_type readObject;
  AdcInputPropertiesType          properties;
  boolean                         itemInit;
  boolean                         sensorPresent;
} therm_npa_adc_sync_read_type;

/* ADC DAL device handle */
static DalDeviceHandle *phADCDalDevHandle = NULL;

/* ADC DAL sensor sync read data */
static therm_npa_adc_sync_read_type adc_sync_read_data[ARR_SIZE(therm_sensor_resource)];

static timer_type  sampling_timer;

static npa_resource_state sampling_time;

extern rex_tcb_type thermal_tcb;

/*=======================================================================

                  LOCAL FUNCTION DEFINITIONS

========================================================================*/
/**
  @brief therm_npa_log_reading

  Log sensor reading to QXDM.

  @param  :  res_index: resource index.
             state: Thermal resource state to report over QXDM.

  @return : None
*/
static void therm_npa_log_reading( unsigned int res_index,
                                   int          state )
{
  PACK(void *)            log_ptr = NULL;
  diag_therm_packet       temp_data;
  const char              *debug_log_str = NULL;
  const char              *stats_log_str = NULL;
  npa_resource_definition *resource = NPA_RESOURCE_AT_INDEX( &therm_sensor_node, res_index );

  memset(&temp_data, '\0', sizeof(temp_data));

  temp_data.version = 0x1;
  temp_data.num_samples = 1;

  temp_data.samples[0].subsystem_id = therm_sensor_data[res_index].subsystem_id;
  temp_data.samples[0].sensor_id    = therm_sensor_data[res_index].sensor_id;
  temp_data.samples[0].temp         = state; 

  /* Setting log string as per resource - Make sure to provide same number of
   * format specifiers (%d, %s, ..) */
  if( (npa_user_data)THERM_NPA_LOGICAL_SENSOR == resource->data )
  {
    debug_log_str = "THERM_NPA read ss_id %d %s, sensor_id %d, State %d";
    stats_log_str = "State ( %s:%d )";
  }
  /* Add checks for other type of resources as required */
  else
  {
    debug_log_str = "THERM_NPA read ss_id %d %s, sensor_id %d, TEMP %d C";
    stats_log_str = "Temperature ( %s:%d )";
  }

  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 4,
                   debug_log_str,
                   temp_data.samples[0].subsystem_id,
                   therm_sensor_data[res_index].npa_node_str,
                   temp_data.samples[0].sensor_id,
                   temp_data.samples[0].temp);

  therm_log_printf(THERM_LOG_LEVEL_STATS, 2,
                   stats_log_str,
                   therm_sensor_data[res_index].npa_node_str,
                   temp_data.samples[0].temp);

  /* Only send out a log packet if we have samples to report on. */
  log_ptr = log_alloc(LOG_TEMPERATURE_MONITOR_C, sizeof(temp_data) + sizeof(log_hdr_type));

  if (log_ptr)
  {
    memcpy((uint8 *)log_ptr + sizeof(log_hdr_type), &temp_data, sizeof(temp_data));
    log_commit(log_ptr);
  }
}

/**
  @brief therm_npa_adc_sync_read_complete_cb

  Read complete CB func of type DALSYSCallbackFunc.

  @param  :  pObj: Internal therm NPA adc read object data.
             dwParam: Unused
             pPayload: AdcResultType data.
             nPayloadSize: Unused

  @return : None
*/
static void therm_npa_adc_sync_read_complete_cb(
                                         therm_npa_adc_sync_read_obj_type *pObj,
                                         uint32 dwParam,
                                         void *pPayload,
                                         uint32 nPayloadSize)
{
  AdcResultType *pAdcResult = pPayload;

  DALSYS_memcpy(pObj->pAdcResult, pAdcResult, sizeof(AdcResultType));
  DALSYS_EventCtrl(pObj->hSignalEvent, DALSYS_EVENT_CTRL_TRIGGER);
}

/**
  @brief therm_npa_adc_sync_read_perform

  Peforms sync/blocking read of sensor.

  @param  :  pObj: Internal therm NPA adc read object data.
             pAdcInputProps: Input properties struct of sensor to read.
             pAdcResult: Pointer to address to place read result.

  @return : DALResult
*/
static DALResult therm_npa_adc_sync_read_perform(
                                        therm_npa_adc_sync_read_obj_type *pObj,
                                        AdcInputPropertiesType *pAdcInputProps,
                                        AdcResultType *pAdcResult)
{
  AdcRequestParametersType adcParams;
  DALResult result = DAL_ERROR;

  DALSYS_SyncEnter(pObj->hSync);
  pObj->pAdcResult = pAdcResult;
  adcParams.hEvent = pObj->hCallbackEvent;
  adcParams.nDeviceIdx = pAdcInputProps->nDeviceIdx;
  adcParams.nChannelIdx = pAdcInputProps->nChannelIdx;
  DALSYS_EventCtrl(pObj->hSignalEvent, DALSYS_EVENT_CTRL_RESET);
  result = DalAdc_RequestConversion(phADCDalDevHandle, &adcParams, NULL);
  (void)DALSYS_EventWait(pObj->hSignalEvent);
  DALSYS_SyncLeave(pObj->hSync);

  return result;
}

/**
  @brief therm_npa_adc_sync_read_obj_init

  Init DAL ADC read obj.

  @param  : phDev: DAL handle for ADC device driver.
            pObj: Internal therm NPA adc read object data.

  @return : DALResult
*/
static DALResult therm_npa_adc_sync_read_obj_init(DalDeviceHandle *phDev,
                                          therm_npa_adc_sync_read_obj_type *pObj)
{
  DALResult result;

  DALSYS_memset(pObj, 0, sizeof(therm_npa_adc_sync_read_obj_type));

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT,
                              &pObj->hCallbackEvent,
                              &pObj->callbackEventObject);

  if(result != DAL_SUCCESS)
  {
     return result;
  }

  result = DALSYS_SetupCallbackEvent(
     pObj->hCallbackEvent,
     (DALSYSCallbackFunc)therm_npa_adc_sync_read_complete_cb,
     pObj);

  if(result != DAL_SUCCESS)
  {
     return result;
  }

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT,
     &pObj->hSignalEvent,
     &pObj->signalEventObject);

  if(result != DAL_SUCCESS)
  {
     return result;
  }

  result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                             &pObj->hSync,
                             &pObj->syncObject);
  if(result != DAL_SUCCESS)
  {
     return result;
  }

  return DAL_SUCCESS;
}

/**
  @brief therm_npa_adc_sync_read_init

  Init ADC DAL read objects.

  @param  : res_index: resource index.

  @return : TRUE or FALSE
*/
static boolean therm_npa_adc_sync_read_init(unsigned int res_index)
{
  DALResult result;
  DALBOOL ret_val    = FALSE;
  char   *input_name = NULL;
  uint32  input_name_size = 0;

  do
  {
    input_name = (char*)therm_sensor_data[res_index].adc_input_str;
    input_name_size = strlen(therm_sensor_data[res_index].adc_input_str);

    result = DalAdc_GetAdcInputProperties(phADCDalDevHandle, input_name,
                                          input_name_size,
                                          &adc_sync_read_data[res_index].properties);

    if(result != DAL_SUCCESS)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                       "DalAdc_GetAdcInputProperties FAIL %d", result);
      break;
    }

    result = therm_npa_adc_sync_read_obj_init(phADCDalDevHandle,
                                              &adc_sync_read_data[res_index].readObject);
    if(result != DAL_SUCCESS)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "Init read obj FAIL %d", result);
      break;
    }
    ret_val = TRUE;
  } while (0);

  return ret_val;
}

/**
  @brief therm_npa_adc_sync_read

  This function is used to perform sensor reads.

  @param  : res_index : resource index.

  @return : Sensor reading.
*/
static npa_resource_state therm_npa_adc_sync_read(unsigned int res_index)
{
  static DALBOOL adc_init = FALSE;
  npa_resource_state therm_state = THERM_NPA_DEFAULT_TEMP;
  DALResult result;
  int32 nPhysical = 0;
  npa_resource_definition *resource = NPA_RESOURCE_AT_INDEX( &therm_sensor_node, res_index );

  /* Check if the resource at given index is a logical resource. If so, we read
   * it differently and therefore return. */
  if( (npa_user_data)THERM_NPA_LOGICAL_SENSOR == resource->data )
  {
    return 0;  /* Eventually we can assert */
  }

  if (adc_init == FALSE)
  {
    /* One time init. */
    DALResult result;
    unsigned int idx;

    DALSYS_InitMod(NULL);

    result = DAL_AdcDeviceAttach((DALDEVICEID)DALDEVICEID_ADC,
                                 &phADCDalDevHandle);
    if (result != DAL_SUCCESS)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "DAL_AdcDeviceAttach FAIL %d",
                       result);

      /* Mark all sensors as init'd and not present. */
      for (idx = 0;
           (idx < ARR_SIZE(therm_sensor_data)) && (idx < THERM_NPA_SENSOR_MAX);
           idx++)
      {
        adc_sync_read_data[idx].itemInit = TRUE;
        adc_sync_read_data[idx].sensorPresent = FALSE;;
      }
    }

    adc_init = TRUE;
  }

  if (adc_sync_read_data[res_index].itemInit == FALSE)
  {
    /* Init ADC sync read object data for each signal. */
    adc_sync_read_data[res_index].sensorPresent =
      therm_npa_adc_sync_read_init(res_index);
    adc_sync_read_data[res_index].itemInit = TRUE;
  }

  do
  {
    if(adc_sync_read_data[res_index].sensorPresent == FALSE)
    {
      break;
    }

    result = therm_npa_adc_sync_read_perform(&adc_sync_read_data[res_index].readObject,
                                             &adc_sync_read_data[res_index].properties,
                                             &adc_sync_read_data[res_index].readResult);

    if(result != DAL_SUCCESS)
    {
      break;
    }

    /* Convert to whole units */
    nPhysical = adc_sync_read_data[res_index].readResult.nPhysical /
                ((int)therm_sensor_data[res_index].unit_per_whole);

    /* Convert units to K. */
    therm_state =  (npa_resource_state)THERM_C_TO_K(nPhysical);

  } while (0);
  return therm_state;
}

/**
  @brief therm_hk_adc_sensor_update_fcn

  This function is invoked by the /therm/sensor/<sensor>
  resources to service requests.

  @param  : resource: A dynamic system element that work requests can be made against.
            client: The handle to the clients registered to the resource.

  @return : None.
*/
static npa_resource_state therm_hk_adc_sensor_update_fcn( npa_resource *resource,
                                                          npa_client_handle client )
{
  npa_resource_state temperature = THERM_NPA_DEFAULT_TEMP;

  /* Ensure that the client is not NULL */
  CORE_VERIFY(client);

  switch (client->type)
  {
    case NPA_CLIENT_IMPULSE:
      temperature = therm_npa_adc_sync_read(resource->index);
      if (adc_sync_read_data[resource->index].sensorPresent)
      {
        therm_npa_log_reading(resource->index, THERM_K_TO_C(temperature));
      }
      break;

    default:
      break;
  } /* End of switch */

  return temperature;
}

/**
  @brief therm_bcl_max_update_fcn

  This function will be invoked when an NPA request is issued to 
  /therm/sensor/modem_bcl_warning resource.

  @param : resource: NPA resource against which request was isssued.
           client: The handle to the client which issued this request.

  @return : State of the resource after request is applied.
 */
static npa_resource_state therm_bcl_max_update_fcn( npa_resource *resource,
                                                    npa_client_handle client )
{
  npa_resource_state new_state = npa_max_plugin.update_fcn(resource, client);

  /* Log the state of the resource */
  therm_npa_log_reading( resource->index, new_state );

  return new_state;
}

/**
  @brief therm_node_driver_fcn

  Sensor node driver function.  Nothing to update.

  @param  : resource: A dynamic system element that work requests can be made against.
            client: The handle to the clients registered to the
            resource.
            state: Update function state.

  @return : Resource state.

*/
static npa_resource_state therm_node_driver_fcn ( npa_resource *resource,
                                                  npa_client *client,
                                                  npa_resource_state state )
{
  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    // The driver function will be called with this client *before* it is
    // made publicly available, so you can place any initialization you
    // need here.

    // The value of state passed here is provided by the
    // npa_define_node function.
  }
  else
  {
    // Set the resource based on the value state.
  }

  return state;
}

/**
  @brief therm_sampling_node_driver_fcn

  Sampling node driver function.  Perform init, but no actual
  driver update.

  @param  : resource: A dynamic system element that work requests can be made against.
            client: The handle to the clients registered to the
            resource.
            state: State passed in from update function.

  @return : Resource state.

*/
static npa_resource_state therm_sampling_node_driver_fcn( npa_resource *resource,
                                                          npa_client *client,
                                                          npa_resource_state state )
{
  sampling_time = state;

  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    // The driver function will be called with this client *before* it is
    // made publicly available, so you can place any initialization you
    // need here.

    // The value of state passed here is provided by the
    // npa_define_node function.
    unsigned int i;

    for ( i = 0; i < ARR_SIZE(sensor_client_handle); i++)
    {
      /* Create impulse client for sampling node to trigger a sensor read */
      sensor_client_handle[i] =  npa_create_sync_client( therm_sensor_resource[i].name,
                                                         "Sampling Sensor Handle",
                                                         NPA_CLIENT_IMPULSE);
      CORE_VERIFY(sensor_client_handle[i]);
    }

    timer_def( &sampling_timer, NULL, &thermal_tcb,
               THERM_SAMPLING_LOOP_TIMER_SIG, NULL, 0 );

    /* Start the sensor sampling polling timer. */
    timer_set( &sampling_timer, sampling_time, 0, T_MSEC);

  }
  else
  {
    // Set the resource based on the value state.
  }

  return state;
}

/*=======================================================================

                 PUBLIC FUNCTION DEFINITIONS

========================================================================*/
/**
  @brief therm_sampling_timer_cb

  Used to trigger sensor read update.

  @param  : timerIndex: Unused.

  @return : None.
*/
void therm_sampling_timer_cb(void)
{
  unsigned int i;

  for ( i = 0; i < ARR_SIZE(sensor_client_handle); i++)
  {
    npa_issue_impulse_request(sensor_client_handle[i]);
  }

  /* Restart sampling timer. */
  timer_set( &sampling_timer, sampling_time, 0, T_MSEC);
}

/**
  @brief therm_npa_sensor_init

  Thermal NPA sensor init.

 */
void therm_npa_sensor_init(void)
{
  /* Define nodes */
  npa_define_node( &therm_sensor_node, initial_sensor_state, NULL );

  npa_define_node( &therm_sampling_node, initial_sampling_state, NULL );

  /* Create logical resources. For simplicty sampling will be performed a the rate of
     the smallest sampling resource request from clients.  */
  npa_alias_resource_cb( "/therm/sampling", "/therm/sensor/pa/sampling", NULL, NULL );
  npa_alias_resource_cb( "/therm/sampling", "/therm/sensor/pa_1/sampling", NULL, NULL );
  npa_alias_resource_cb( "/therm/sampling", "/therm/sensor/modem_bcl_warning/sampling", NULL, NULL );
}
