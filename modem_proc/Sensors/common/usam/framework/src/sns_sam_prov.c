/*============================================================================
  @file sns_sam_prov.c

  @brief
  This here is the only file that may be changes while adding new SAM
  Sensor Providers into the SAM Framework.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include "sns_sam_prov_api.h"
#include "sns_sam_prov.h"
#include "sns_sam_client.h"
#include "sns_sam.h"
#include "sns_sam_dep.h"
#include "sns_sam_init.h"
#include "sns_sam_error.h"

/*============================================================================
  Provider Registration Function Declarations
  ===========================================================================*/

sns_sam_err sns_sam_prov_loc_register(
      sns_sam_prov_api **provAPI );

sns_sam_err sns_sam_prov_sam_register(
      sns_sam_prov_api **provAPI );

sns_sam_err sns_sam_prov_smgr_register(
      sns_sam_prov_api **provAPI );

sns_sam_err sns_sam_prov_smgri_register(
      sns_sam_prov_api **provAPI );

/*============================================================================
  Type Definitions
  ===========================================================================*/

struct sns_sam_prov
{
  /* The registration function for this Provider */
  sns_sam_prov_register reg;
  /* Service object associated with this provider */
  qmi_idl_service_object_type serviceObj;
  /* Sensor Req object allocated for initilization */
  sns_sam_sensor sensor;
};
typedef struct sns_sam_prov sns_sam_prov;

/*============================================================================
  Global Data
  ===========================================================================*/

/* Array of all known Providers */
STATIC sns_sam_prov samProviders[] =
{
  { sns_sam_prov_sam_register, NULL },  // Note: SAM must be the first entry in this list
  { sns_sam_prov_loc_register, NULL },
  { sns_sam_prov_smgr_register, NULL },
  { sns_sam_prov_smgri_register, NULL },
};

const uint32_t samProvidersSize =
  sizeof(samProviders) / sizeof(samProviders[0]);

/* Callback functions available for all Providers.  This object
 * is shared by all, and the value of sensorReq will be reset before each
 * Provider API function call passing it as an argument. */
sns_sam_prov_callback provCB SNS_SAM_UIMAGE_DATA;

/*============================================================================
  Static Functions
  ===========================================================================*/

/* See sns_sam_prov_cb_send_req in sns_sam_prov_api.h. */
STATIC sns_sam_err
sns_sam_prov_cb_send_req( sns_sam_sensor_req const *sensorReq,
    sns_sam_enable_req const *enableReqMsg )
{
  return sns_sam_client_send( sensorReq, enableReqMsg );
}

/* See sns_sam_prov_add_sensor in sns_sam_prov_api.h. */
STATIC sns_sam_sensor*
sns_sam_prov_get_sensor( sns_sam_sensor_uid sensorUID )
{
  sns_sam_sensor *sensor = sns_sam_lookup_sensor_from_suid( &sensorUID );
  sns_sam_err err;

  SNS_PRINTF_STRING_MEDIUM_1( samModule, "Adding new sensor %x", sensorUID );

  if( NULL == sensor )
  {
    err = sns_sam_init_sensor( &sensor );
    if( SAM_ENONE != err )
    {
      return NULL;
    }
    else
    {
      return sensor;
    }
  }
  else
  {
    SNS_PRINTF_STRING_MEDIUM_1( samModule, "Added sensor already exists %x",
        sensorUID );
    return sensor;
  }
}

/* See sns_sam_prov_sensor_available in sns_sam_prov_api.h. */
STATIC sns_sam_err
sns_sam_prov_sensor_available( sns_sam_sensor const *sensor )
{
  sns_sam_dep_available( sensor );
  return SAM_ENONE;
}

/* See sns_sam_prov_remove_sensor in sns_sam_prov_api.h. */
STATIC SNS_SAM_UIMAGE_CODE sns_sam_err
sns_sam_prov_remove_sensor( sns_sam_sensor_uid sensorUID )
{
  // PEND: To be implemented later
  // Remove active algorithm instances
  // Remove sensor from sensorQ and unregister from SMR
  // Call smr_client_init for the applicable service object

  return SAM_ENONE;
}

/* See sns_sam_prov_stop_stream in sns_sam_prov_api.h. */
STATIC SNS_SAM_UIMAGE_CODE sns_sam_err
sns_sam_prov_stop_stream( sns_sam_sensor_req const *sensorReq )
{
  SNS_PRINTF_STRING_ERROR_1( samModule, "sns_sam_prov_stop_stream %x",
      *sensorReq->sensor->sensorUID );
  sns_sam_uimage_exit();
  sns_sam_remove_algo_instances( (sns_sam_sensor_req*)sensorReq, SENSOR1_EFAILED );

  return SAM_ENONE;
}

/* See sns_sam_prov_get_data_complete in sns_sam_prov_api.h. */
STATIC void
sns_sam_prov_get_data_complete( sns_sam_sensor_req const *sensorReq )
{
  sns_sam_handle_flush_response( sensorReq );
}

/* See sns_sam_prov_uimage_exit in sns_sam_prov_api.h. */
STATIC SNS_SAM_UIMAGE_CODE void
sns_sam_prov_uimage_exit()
{
  sns_sam_uimage_exit();
}

/**
 * A new SAM service has become available.  Start the initialization
 * process for the associated sensor object, if available.
 *
 * @param[i] serviceObj QMI Service Object for the newly available service
 */
STATIC void
sns_sam_prov_init_sam_client( qmi_idl_service_object_type serviceObj )
{
  sns_q_link_s *qCurr;
  uint32_t serviceID;

  qmi_idl_get_service_id( serviceObj, &serviceID );
  for( qCurr = sns_q_check( &sensorQ ); NULL != qCurr;
       qCurr = sns_q_next( &sensorQ, qCurr ) )
  {
    int32_t errQMI;
    uint32_t tempID;
    sns_sam_sensor *sensor = (sns_sam_sensor*)qCurr;

    if( NULL != sensor->sensorReq.clientHndl )
    {
      // Skip if this sensor has already been initialized with QMI
      continue;
    }

    errQMI = qmi_idl_get_service_id( sensor->serviceObj, &tempID );
    if( QMI_IDL_LIB_NO_ERR == errQMI && serviceID == tempID )
    {
      if( SAM_ENONE != sns_sam_client_init( &sensor->sensorReq, 0, false ) )
      {
        SNS_PRINTF_STRING_ERROR_1( samModule,
          "Error initializing client connection to %i", tempID );
      }
      else
      {
        sensor->provAPI = samProviders[ 0 ].sensor.provAPI;
        sensor->sensorReq.provAPI = sensor->provAPI;

        provCB.sensorReq = &sensor->sensorReq;
        samProviders[ 0 ].sensor.provAPI->sns_sam_prov_sensor_list( &provCB );
      }
    }
  }
}

/*============================================================================
  Public Functions
  ===========================================================================*/

void
sns_sam_prov_init_client( sns_sam_client_init_data *initData )
{
  uint32_t i;
  bool found = false;
  uint32_t serviceID, tempID;

  qmi_idl_get_service_id( initData->serviceObj, &serviceID );
  if( initData->timeoutExpired )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
        "Timeout expired for %i", serviceID );
  }
  else
  {
    for( i = 0; i < samProvidersSize; i++ )
    {
      qmi_idl_get_service_id( samProviders[ i ].sensor.serviceObj, &tempID );
      if( serviceID == tempID )
      {
        found = true;
        if( NULL != samProviders[ i ].sensor.provAPI->sns_sam_prov_sensor_list )
        {
          sns_sam_err err =
            sns_sam_client_init( &samProviders[ i ].sensor.sensorReq, 0, false );
          if( SAM_ENONE != err )
          {
            SNS_PRINTF_STRING_ERROR_2( samModule,
              "Error %i initializing PROV connection %i", err, serviceID );
            // PEND: Do we have any cleanup to perform here?
          }
          else
          {
            provCB.sensorReq = &samProviders[ i ].sensor.sensorReq;
            samProviders[ i ].sensor.provAPI->sns_sam_prov_sensor_list( &provCB );
          }
        }
      }
    }

    if( !found )
    {
      sns_sam_prov_init_sam_client( initData->serviceObj );
    }
  }
}

bool
sns_sam_prov_managed( qmi_idl_service_object_type serviceObj )
{
  uint32_t i;

  for( i = 0; i < samProvidersSize; i++ )
  {
    if( samProviders[ i ].serviceObj == serviceObj )
    {
      return true;
    }
  }
  return false;
}

sns_sam_err
sns_sam_prov_init_fw( void )
{
  uint32_t i;
  sns_sam_err err;

  provCB.structSize = sizeof(sns_sam_prov_callback);
  provCB.sensorReq = NULL;

  provCB.sns_sam_prov_cb_send_req = &sns_sam_prov_cb_send_req;
  provCB.sns_sam_prov_get_sensor = &sns_sam_prov_get_sensor;
  provCB.sns_sam_prov_remove_sensor = &sns_sam_prov_remove_sensor;
  provCB.sns_sam_prov_stop_stream = &sns_sam_prov_stop_stream;
  provCB.sns_sam_prov_sensor_available = &sns_sam_prov_sensor_available;
  provCB.sns_sam_prov_get_data_complete = &sns_sam_prov_get_data_complete;
  provCB.sns_sam_prov_uimage_exit = &sns_sam_prov_uimage_exit;

  for( i = 0; i < samProvidersSize; i++ )
  {
    err = samProviders[ i ].reg( &samProviders[ i ].sensor.provAPI );

    if( SAM_ENONE == err )
    {
      provCB.sensorReq = NULL;
      samProviders[ i ].serviceObj =
        samProviders[ i ].sensor.provAPI->sns_sam_prov_service( &provCB );

      samProviders[ i ].sensor.serviceObj = samProviders[ i ].serviceObj;
      samProviders[ i ].sensor.sensorUID = NULL;
      samProviders[ i ].sensor.sensorReq.sensor = &samProviders[ i ].sensor;
      samProviders[ i ].sensor.sensorReq.sensorUID = samProviders[ i ].sensor.sensorUID;
      samProviders[ i ].sensor.sensorReq.provAPI = samProviders[ i ].sensor.provAPI;

      if( NULL != samProviders[ i ].serviceObj )
      {
        err = sns_sam_client_check( samProviders[ i ].serviceObj,
            SMR_CLIENT_INSTANCE_ANY, SNS_SAM_INIT_CLIENT_TIMEOUT_US );
        if( SAM_ENONE != err )
        {
          SNS_PRINTF_STRING_ERROR_1( samModule, "SAM client check failed %i", err );
          return SAM_EFAILED;
        }
      }
    }
    else
    {
      SNS_PRINTF_STRING_ERROR_1( samModule, "Error registering provider %i", err );
    }
  }

  return SAM_ENONE;
}
