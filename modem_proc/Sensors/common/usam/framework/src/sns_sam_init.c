/*============================================================================
  @file sns_sam_init.c

  @brief
  Initialization code for the SAM Framework.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include "sns_sam_common_v01.h"
#include "sns_smgr_internal_api_v02.h"
#include "sns_reg_api_v02.h"
#include "sns_init.h"
#include "sns_em.h"
#include "sns_debug_str.h"
#include "sns_sam.h"
#include "sns_sam_algo_api.h"
#include "sns_sam_init.h"
#include "sns_sam_client.h"
#include "sns_sam_service.h"
#include "sns_sam_req.h"
#include "sns_sam_reg.h"
#include "sns_sam_ind.h"
#include "sns_sam_dep.h"
#include "sns_sam_cb.h"
#include "sns_sam_pm.h"
#include "sns_sam_dl.h"
#include "sns_sam_io.h"
#include "sns_sam_prov.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/

/* How long to wait for the registry service before giving up on trying to
 * create a client with the registry (in milliseconds). */
#define SNS_SAM_INIT_REG_CLI_TIMEOUT_MS 10000

/* How long SAM is prepared to batch algorithm output for in seconds */
#define SNS_SAM_MAX_BATCHING_TIME 10

/*============================================================================
  External Data
  ===========================================================================*/

/* Static list of available algorithms */
extern const sns_sam_algo_register samAlgoRegisterFuncs[];
/* Number of algorithms listed in samAlgoRegisterFuncs */
extern const uint32_t samAlgoRegisterFuncsSize;

/*============================================================================
  Static Data
  ===========================================================================*/

/* Timeout timer for registry requests during SAM initialization */
static sns_em_timer_obj_t registryTimerObj;

/* List of services that are available, but no client connection has been made
 * with SMR for them yet */
static sns_q_s samClientInitQ;
static OS_EVENT *samClientInitQMutex;

/*============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
* Allocate memory for an existing sensor algorithm object in TCM, Copy memory
* from DDR to TCM. Free DDR memory if alloc was successful in TCM
*
* @param[io] algo algorithm pointer in DDR heap that has to be moved to TCM
* @param[i] size Size of object to be moved
*
* @return SAM_ENONE
*         SAM_ENOMEM Not enough memory to allocate sensor_algo_object. ddr_ptr is
*                    not freed.
*/
STATIC sns_sam_err
sns_sam_move_ddr_algo_to_tcm(sns_sam_sensor_algo **algo, uint32_t size)
{
  sns_sam_sensor_algo *tcm_ptr = SNS_OS_U_MALLOC(SNS_SAM_DBG_MOD, size);

  if(NULL == tcm_ptr)
  {
    return SAM_ENOMEM;
  }
  SNS_OS_MEMCOPY( tcm_ptr, *algo, size);

  tcm_ptr->sensor.sensorReq.sensor = (sns_sam_sensor*)tcm_ptr;
  sns_q_link( tcm_ptr, (sns_q_link_s*)tcm_ptr );

  SNS_OS_FREE( *algo );
  *algo = tcm_ptr;

  return SAM_ENONE;
}

/**
 * Create sns_sam_sensor_algo objects for all local SAM algorithm, and adds to
 * global database.
 *
 * @return SAM_ENONE
 *         SAM_ENOMEM Not enough memory to allocate sensor_algo objects
 */
STATIC sns_sam_err
sns_sam_record_local_algos( void )
{
  sns_sam_err err = SAM_ENONE;
  uint32_t i;
  bool alreadyLoaded;

  SNS_PRINTF_STRING_LOW_0( samModule, "Record local algorithms" );

  // PEND: The following should not be a blocking call
  sns_sam_dl_record_algos();

  for( i = 0; i < samAlgoRegisterFuncsSize; i++ )
  {
    sns_sam_sensor_algo *algo = NULL;
    err = sns_sam_init_algo( &algo );

    if( SAM_ENONE == err )
    {
      samAlgoRegisterFuncs[ i ](
          &algo->algoAPI,
          &algo->algoMsgAPI,
          &algo->sensor.serviceObj,
          algo->sensor.sensorUID );

      algo->sensor.sensorReq.sensorUID = algo->sensor.sensorUID;
      alreadyLoaded =
        ( NULL != sns_sam_lookup_sensor_from_suid( algo->sensor.sensorUID ) );
      if( !alreadyLoaded )
      {
        if( SNS_SAM_ALGOINST_UIMAGE == sns_sam_determine_uimage( algo ) )
        {
          SNS_PRINTF_STRING_LOW_1( samModule, "Moving algorithm %x to TCM",
              *algo->sensor.sensorUID );
          sns_sam_move_ddr_algo_to_tcm( &algo, sizeof(sns_sam_sensor_algo) );
        }

        SNS_PRINTF_STRING_LOW_1( samModule, "Record local algorithm: %x",
          *algo->sensor.sensorUID );

        sns_q_put( &sensorQ, (sns_q_link_s*)algo );
      }
      else
      {
        SNS_PRINTF_STRING_LOW_2( samModule,
            "Static Algo#%d Not Loaded. SUID:0x%x Already Exits",
            i, *algo->sensor.sensorUID );
        sns_sam_deinit_algo( algo );
      }
    }
  }

  return err;
}

/**
 * Populate the fields of a local SAM algorithm.  Will involve multiple calls
 * to the algorithm API.  Must be called after all local algorithms are recorded
 * to algorithm list.
 *
 * @param[io] algo Algorithm to populate
 *
 * @return SAM_ENONE
 *         SAM_ENOMEM Not enough memory to allocate all necessary objects
 *         SAM_ESTATE Unable to initialize; algorithm will not be available
 */
STATIC sns_sam_err
sns_sam_init_local_algo( sns_sam_sensor_algo *algo )
{
  sns_sam_err err;
  uint32_t count;
  sns_sam_reg_group registryGroups[SNS_SAM_MAX_REG_GRP];
  struct sns_sam_algo_dep_sensor depSensors[SNS_SAM_MAX_DEPENDENT_SENSOR];
  uint32_t i;
  bool hasDependency = false;
  sns_sam_algoinst_uimage_mode uImageMode;
  int32_t errQMI;

  SNS_PRINTF_STRING_LOW_1( samModule, "sns_sam_init_local_algo: %x",
    *algo->sensor.sensorUID );

  count = SNS_SAM_MAX_REG_GRP;
  err = algo->algoMsgAPI->sns_sam_algo_reg_grps( &count, registryGroups );
  if( SAM_EMAX == err )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
      "Unable to init algo reg groups %x", *algo->sensor.sensorUID );
    return SAM_ESTATE;
  }
  hasDependency |= ( 0 != count );

  for( i = 0; i < count; i++ )
  {
    algo->registryGroups[ i ] = registryGroups[ i ];
  }

  count = SNS_SAM_MAX_DEPENDENT_SENSOR;
  err = algo->algoMsgAPI->sns_sam_algo_dep_sensors( &count, depSensors );
  if( SAM_EMAX == err )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
      "Unable to init algo dep sensors %x", *algo->sensor.sensorUID );
    return SAM_ESTATE;
  }
  hasDependency |= ( 0 != count );

  for( i = 0; i < count; i++ )
  {
    sns_sam_sensor *sensor =
      sns_sam_lookup_sensor_from_suid( &depSensors[ i ].sensorUID );
    if( NULL == sensor )
    {
      err = sns_sam_init_sensor( &sensor );
      if( SAM_ENOMEM == err )
      {
        SNS_PRINTF_STRING_ERROR_1( samModule,
          "Not enough memory to init sensor %x", *algo->sensor.sensorUID );
        return SAM_ENOMEM;
      }
      else
      {
        *sensor->sensorUID = depSensors[ i ].sensorUID;
        sensor->serviceObj = depSensors[ i ].serviceObj;
        sensor->sensorReq.sensorUID = sensor->sensorUID;
      }
    }
    algo->dependencies[ i ] = sensor;
  }

  algo->bufferSizes.structSize = sizeof(algo->bufferSizes);
  algo->algoMsgAPI->sns_sam_algo_mem_const_req( &algo->bufferSizes );

  // Round all i/o buffer sizes to the nearest 4-byte size
  algo->bufferSizes.outputDataSize += (0 == algo->bufferSizes.outputDataSize % 4)
    ? 0 : 4 - (algo->bufferSizes.outputDataSize % 4);
  algo->bufferSizes.inputDataSize += (0 == algo->bufferSizes.inputDataSize % 4)
    ? 0 : 4 - (algo->bufferSizes.inputDataSize % 4);

  if( algo->bufferSizes.outputDataSize + sizeof(sns_sam_timestamp) > sns_sam_io_size() ||
      algo->bufferSizes.inputDataSize + sizeof(sns_sam_timestamp) > sns_sam_io_size() )
  {
    SNS_PRINTF_STRING_ERROR_3( samModule,
      "Input/Output maximum size exceeded; input: %i; output: %i; max: %i",
      algo->bufferSizes.inputDataSize, algo->bufferSizes.outputDataSize,
      sns_sam_io_size() );
    return SAM_ESTATE;
  }

  uImageMode = sns_sam_determine_uimage( algo );
  if( SNS_SAM_ALGOINST_UIMAGE == uImageMode )
  {
    algo->persistData.buf = (intptr_t)SNS_OS_U_MALLOC( SNS_SAM_DBG_MOD,
      algo->bufferSizes.persistDataSize );
  }
  else
  {
    algo->persistData.buf = (intptr_t)SNS_OS_MALLOC( SNS_SAM_DBG_MOD,
      algo->bufferSizes.persistDataSize );
  }
  SNS_ASSERT( (intptr_t)NULL != algo->persistData.buf );

  SNS_OS_MEMZERO( (void*)algo->persistData.buf, algo->bufferSizes.persistDataSize );

  errQMI = qmi_idl_get_message_c_struct_len( algo->sensor.serviceObj, QMI_IDL_INDICATION,
    SNS_SAM_ALGO_REPORT_IND, &algo->qmiIndSize );
  if( QMI_IDL_LIB_NO_ERR != errQMI )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
      "QMI length lookup error: %i", errQMI );
    return SAM_ESTATE;
  }

  if( NULL != algo->algoMsgAPI->sns_sam_algo_gen_batch_ind )
  {
    // PEND: An algorithm may have this function non-NULL, but still not support batching
    errQMI = qmi_idl_get_message_c_struct_len( algo->sensor.serviceObj, QMI_IDL_INDICATION,
      SNS_SAM_ALGO_BATCH_IND, &algo->qmiBatchIndSize );
    if( QMI_IDL_LIB_NO_ERR != errQMI )
    {
      SNS_PRINTF_STRING_ERROR_1( samModule,
        "QMI length lookup error: %i", errQMI );
      return SAM_ESTATE;
    }
  }
  else
  {
    algo->qmiBatchIndSize = 0;
  }

  if( !hasDependency )
  {
    sns_sam_init_dep_met( algo );
  }

  return SAM_ENONE;
}

/**
 * Checks if an algorithm is presently registered with SMR
 *
 * @param[i] algorithm Local SAM algorithm to check
 *
 * @return true if registered, false otherwise
 */
STATIC bool
sns_sam_algo_is_reg( sns_sam_sensor_algo const *algorithm )
{
  return algorithm->sensor.isAvailable;
}

/**
 * Timer callback function for registry request timeout.
 *
 * @param[i] unused
 */
STATIC void sns_sam_init_timer_cb( void *unused )
{
  UNREFERENCED_PARAMETER(unused);
  uint8_t errOS;
  sns_os_sigs_post( sns_sam_sig_event, SNS_SAM_REG_INIT_TIMER_SIG,
      OS_FLAG_SET, &errOS );
  SNS_ASSERT( OS_ERR_NONE == errOS );
}

/**
 * Initializes the registry service connection, and sends the registry group
 * requests for all known local algorithms.
 *
 * @return SAM_ENONE
 *         SAM_EFAILED Unable to send registry read request(s)
 *         SAM_ENOT_AVAILABLE Unable to set registry timer; but request(s) sent
 */
STATIC sns_sam_err
sns_sam_init_reg_client( void )
{
  sns_q_link_s *qCurr;
  sns_sam_err errSAM = SAM_EFAILED;
  sns_err_code_e errEM;
  uint32_t tickOffset;

  for( qCurr = sns_q_check( &sensorQ ); NULL != qCurr;
       qCurr = sns_q_next( &sensorQ, qCurr ) )
  {
    sns_sam_sensor_algo *algo = (sns_sam_sensor_algo*)qCurr;
    uint32_t i;

    if( !algo->sensor.isLocal )
    {
      break;
    }

    for( i = 0; i < SNS_SAM_MAX_REG_GRP && -1 != algo->registryGroups[ i ]; i++ )
    {
      sns_sam_reg_data regData = {
        .groupID = algo->registryGroups[ i ], .groupDataLen = 0 };
      errSAM = sns_sam_reg_req( &regData );

      if( SAM_ENONE != errSAM )
      {
        SNS_PRINTF_STRING_ERROR_1( samModule,
          "Error sending regiatry request %i", errSAM );
        errSAM = SAM_EFAILED;
      }
    }
  }

  if( SAM_ENONE == errSAM )
  {
    errEM = sns_em_create_timer_obj( &sns_sam_init_timer_cb,
        NULL, SNS_EM_TIMER_TYPE_ONESHOT, &registryTimerObj );
    if( SNS_SUCCESS == errEM )
    {
      tickOffset = sns_em_convert_usec_to_localtick(
        SNS_SAM_INIT_REG_TIMEOUT_US );
      errEM = sns_em_register_timer( registryTimerObj, tickOffset );

      if( SNS_SUCCESS != errEM )
      {
        SNS_PRINTF_STRING_ERROR_1( samModule,
          "Error registering timer %i", errEM );
        errSAM = SAM_ENOT_AVAILABLE;
      }
      else
      {
        errSAM = SAM_ENONE;
      }
    }
    else
    {
      SNS_PRINTF_STRING_ERROR_1( samModule,
        "Error creating timer %i", errEM );
      errSAM = SAM_ENOT_AVAILABLE;
    }
  }

  return errSAM;
}

/**
 * Initializes the client connection to a service SAM wishes to use.
 *
 * PEND: Can we remove the need for this function?
 *
 * @param[i] initData Information about the service that has become available
 */
STATIC void
sns_sam_init_client( struct sns_sam_client_init_data *initData )
{
  sns_sam_err err;
  int32_t errQMI = QMI_IDL_LIB_NO_ERR;
  uint32_t serviceID, serviceIDREG;

  errQMI |= qmi_idl_get_service_id( initData->serviceObj, &serviceID );
  errQMI |= qmi_idl_get_service_id(
    SNS_REG2_SVC_get_service_object_v02(), &serviceIDREG );

  if( QMI_IDL_LIB_NO_ERR != errQMI )
  {
    SNS_PRINTF_STRING_ERROR_0( samModule, "Error getting Service ID" );
    return ;
  }

  if( serviceIDREG == serviceID )
  {
    if( initData->timeoutExpired )
    {
      sns_sam_init_timer_cb( NULL );
    }
    else
    {
      err = sns_sam_client_init( &sensorReqREG,
          SNS_SAM_INIT_REG_CLI_TIMEOUT_MS, false );
      if( SAM_ENONE != err )
      {
        SNS_PRINTF_STRING_ERROR_1( samModule,
          "Error initializing REG client connection %i", err );
        sns_sam_handle_reg_timeout();
      }
      else
      {
        sns_sam_init_reg_client();
      }
    }
  }
  else
  {
    sns_sam_prov_init_client( initData );
  }
}

/*============================================================================
  Function Definitions
  ===========================================================================*/

/**
 * Generates gated sensors that are a combination of a streaming sensor and an
 * event that an algorithm has previously requested. An algorithm can request
 * for such a sensor by providing SAM with the bitwise XOR of the event sensor's
 * SUID and the streaming sensor's SUID. The  resulting sensor will inherit the
 * attributes of the streaming sensor, and the SMR client handle, QMI service object,
 * location and processor details from the event sensors. Messages sent to this sensor
 * will be directed to the parent event sensor.
 *
 * @param[i] parentSensor  An event sensor or a streaming sensor that is needed for the
 *                         gated sensor.
 *
 * PEND: Find a way to move this functionality into the SMGR/SMGRI Providers
*/
void
sns_sam_gen_gated_sensors( sns_sam_sensor const *parentSensor )
{
  sns_q_link_s *qCurr;

  SNS_PRINTF_STRING_HIGH_1( samModule, "Check for gated Sensor: %x",
      *parentSensor->sensorUID );

  for( qCurr = sns_q_check( &sensorQ ); NULL != qCurr;
       qCurr = sns_q_next( &sensorQ, qCurr ) )
  {
    uint8_t i;
    sns_sam_sensor const *gatingSensor = (sns_sam_sensor*)qCurr;
    sns_sam_sensor_uid suid = *parentSensor->sensorUID ^ *gatingSensor->sensorUID;
    sns_sam_sensor *gatedSensor = sns_sam_lookup_sensor_from_suid( &suid );

    if( NULL != gatedSensor )
    {
      sns_sam_sensor const *streamSensor = NULL, *eventSensor = NULL;

      if( SNS_SAM_SENSOR_STREAM == parentSensor->gatingType &&
          SNS_SAM_SENSOR_EVENT == gatingSensor->gatingType )
      {
        streamSensor = parentSensor;
        eventSensor = gatingSensor;
      }
      else if( SNS_SAM_SENSOR_EVENT == parentSensor->gatingType &&
               SNS_SAM_SENSOR_STREAM == gatingSensor->gatingType )
      {
        streamSensor = gatingSensor;
        eventSensor = parentSensor;
      }

      if( NULL != streamSensor )
      {
        SNS_PRINTF_STRING_HIGH_3(samModule,
            "An algorithm wants gated Sensor: %x with parents: %x and %x",
            *gatedSensor->sensorUID, *streamSensor->sensorUID,
            *eventSensor->sensorUID);

        for( i = 0; i < SAM_ALGO_ATTR_CNT ; i++ )
        {
          gatedSensor->attributes[i].attribute =
            streamSensor->attributes[i].attribute;
          gatedSensor->attributes[i].attrValue =
            streamSensor->attributes[i].attrValue;
          gatedSensor->attributes[i].supported =
            streamSensor->attributes[i].supported;
        }

        gatedSensor->sensorLocation = eventSensor->sensorLocation;
        gatedSensor->sensorReq.clientHndl = eventSensor->sensorReq.clientHndl;
        gatedSensor->serviceObj = eventSensor->serviceObj;
        gatedSensor->gatingType = SNS_SAM_SENSOR_GATED_STREAM;
        *gatedSensor->sensorUID = suid;
        gatedSensor->isAvailable = true;
        gatedSensor->provAPI = eventSensor->provAPI;
        gatedSensor->sensorReq.provAPI = gatedSensor->provAPI;
        sns_sam_dep_available( gatedSensor );
      }
    }
  }
}

sns_sam_err
sns_sam_init_algo( sns_sam_sensor_algo **algo )
{
  sns_sam_sensor_algo *algoOut;
  uint32_t i;

  // PEND: Use sns_sam_init_sensor internally

  algoOut = SNS_OS_MALLOC( SNS_SAM_DBG_MOD, sizeof(sns_sam_sensor_algo) );
  if( NULL == algoOut )
  {
    return SAM_ENOMEM;
  }

  algoOut->algoAPI = NULL;
  algoOut->algoMsgAPI = NULL;
  for( i = 0; i < SNS_SAM_MAX_REG_GRP; i++ )
  {
    algoOut->registryGroups[ i ]= -1;
  }
  for( i = 0; i < SNS_SAM_MAX_DEPENDENT_SENSOR; i++ )
  {
    algoOut->dependencies[ i ] = NULL;
  }
  SNS_OS_MEMZERO( &algoOut->bufferSizes, sizeof(sns_sam_algo_const_buf_size) );

  algoOut->persistData.bufSize = 0;
  algoOut->persistData.buf = (intptr_t)NULL;
  algoOut->serviceProvider = NULL;

  algoOut->sensor.sensorUID = (sns_sam_sensor_uid *) SNS_OS_U_MALLOC( SNS_SAM_DBG_MOD,
    sizeof(sns_sam_sensor_uid));
  algoOut->sensor.sensorLocation = localProcessor;
  for( i = 0; i < SAM_ALGO_ATTR_CNT; i++ )
  {
    algoOut->sensor.attributes[ i ].attribute = i;
    algoOut->sensor.attributes[ i ].supported = false;
  }

  algoOut->sensor.serviceObj = NULL;
  algoOut->sensor.sensorReq.clientHndl = NULL;
  algoOut->sensor.sensorReq.sensor = (sns_sam_sensor*)algoOut;
  algoOut->sensor.isAvailable = false;
  algoOut->sensor.isLocal = true;
  algoOut->sensor.dlInfo = NULL;

  sns_q_link( algoOut, (sns_q_link_s*)algoOut );

  *algo = algoOut;

  return SAM_ENONE;
}

void
sns_sam_deinit_algo(sns_sam_sensor_algo *algo)
{
  if( NULL != algo )
  {
    if( NULL != algo->sensor.dlInfo )
    {
      SNS_OS_FREE(algo->sensor.dlInfo);
    }
    SNS_OS_U_FREE(algo->sensor.sensorUID);
    SNS_OS_FREE(algo);
  }
}

void
sns_sam_handle_reg_timeout( void )
{
  sns_q_link_s *qCurr;
  uint32_t i;
  sns_sam_reg_data temp;

  temp.groupDataLen = 0;
  temp.groupData = NULL;

  SNS_PRINTF_STRING_LOW_0( samModule, "sns_sam_handle_reg_timeout" );

  for( qCurr = sns_q_check( &sensorQ ); NULL != qCurr;
       qCurr = sns_q_next( &sensorQ, qCurr ) )
  {
    sns_sam_sensor_algo *algo = (sns_sam_sensor_algo*)qCurr;

    if( !algo->sensor.isLocal || sns_sam_algo_is_reg( algo ) )
    {
      continue;
    }

    for( i = 0;
         i < SNS_SAM_MAX_REG_GRP && -1 != algo->registryGroups[ i ]; i++ )
    {
      temp.groupID = algo->registryGroups[ i ];
      SNS_PRINTF_STRING_LOW_1( samModule, "Alerting algorithm %x",
        *algo->sensor.sensorUID );
      algo->algoMsgAPI->sns_sam_algo_dep_registry_met( algo,
          sns_sam_init_dep_met, &algo->persistData, &temp );
    }
  }

  if( NULL != registryTimerObj )
  {
    sns_em_delete_timer_obj( registryTimerObj );
    registryTimerObj = NULL;
  }
}

void
sns_sam_init_dep_met( sns_sam_sensor_algo const *algo )
{
  bool registered;
  sns_sam_err err;

  SNS_PRINTF_STRING_MEDIUM_1( samModule,
    "sns_sam_init_dep_met %x", *algo->sensor.sensorUID );

  registered = sns_sam_algo_is_reg( algo );
  if( !registered )
  {
    algo->algoMsgAPI->sns_sam_algo_get_attr( &algo->persistData,
      ((sns_sam_sensor_algo*)algo)->sensor.attributes );

    if( algo->sensor.attributes [SAM_ALGO_ATTR_MAX_REPORTS_PER_IND].supported &&
        1 < algo->sensor.attributes [SAM_ALGO_ATTR_MAX_REPORTS_PER_IND].attrValue &&
        NULL != algo->algoMsgAPI->sns_sam_algo_gen_batch_ind )
    {
      ((sns_sam_sensor_algo*)algo)->sensor.attributes [SAM_ALGO_ATTR_MAX_BATCH].supported = true;

      ((sns_sam_sensor_algo*)algo)->sensor.attributes [SAM_ALGO_ATTR_MAX_BATCH].attrValue =
        sns_sam_io_cnt() * sns_sam_io_size() /
        ( algo->bufferSizes.outputDataSize + sizeof(sns_sam_timestamp) );
    }

    if( algo->sensor.attributes[SAM_ALGO_ATTR_REPORT_MODE].supported)
    {
       /* Limit to only supported report modes.
        *
        * Supported: Periodic, async, sync
        * Not supported: One shot
        */
      ((sns_sam_sensor_algo*)algo)->sensor.attributes[SAM_ALGO_ATTR_REPORT_MODE].attrValue &=
        (SNS_SAM_PERIODIC_REPORT_V01 | SNS_SAM_ASYNC_REPORT_V01 | SNS_SAM_SYNC_REPORT_V01);
    }

    err = sns_sam_service_reg( (sns_sam_sensor_algo*)algo );
    if( SAM_ENONE != err )
    {
      SNS_PRINTF_STRING_ERROR_2( samModule,
        "Error registering service %x, error %i",
        *algo->sensor.sensorUID, err );
    }
  }
}

sns_sam_err
sns_sam_init_algos( void )
{
  sns_sam_err err = SAM_ENONE;
  sns_q_link_s *qCurr;

  err = sns_sam_record_local_algos();
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_ERROR_0( samModule, "Not enough memory to init algos" );
    return SAM_EFAILED;
  }

  for( qCurr = sns_q_check( &sensorQ ); NULL != qCurr;
       qCurr = sns_q_next( &sensorQ, qCurr ) )
  {
    sns_sam_sensor_algo *algo = (sns_sam_sensor_algo*)qCurr;
    uint32_t i;

    if( !algo->sensor.isLocal )
    {
      continue;
    }

    err = sns_sam_init_local_algo( algo );
    if( SAM_ENONE != err )
    {
      SNS_PRINTF_STRING_ERROR_2( samModule, "Unable to init algo %x (%i)",
        *algo->sensor.sensorUID, err );
      // PEND: Need to free all associated memory, and remove the algorithm from the queue
    }
    else
    {
      for( i = 0; i < SNS_SAM_MAX_DEPENDENT_SENSOR; i++ )
      {
        if( NULL != algo->dependencies[ i ] &&
            NULL != algo->dependencies[ i ]->serviceObj &&
            !sns_sam_prov_managed( algo->dependencies[ i ]->serviceObj ) )
        {
          err = sns_sam_client_check( algo->dependencies[ i ]->serviceObj,
              SMR_CLIENT_INSTANCE_ANY, SNS_SAM_INIT_CLIENT_TIMEOUT_US );

          if( SAM_ENONE != err )
          {
            SNS_PRINTF_STRING_ERROR_1( samModule,
              "Error in client_check %i", err );
          }
        }
      }
    }
  }

  err = sns_sam_client_check( SNS_REG2_SVC_get_service_object_v02(),
      SMR_CLIENT_INSTANCE_ANY, SNS_SAM_INIT_CLIENT_TIMEOUT_US );
  if( SAM_ENONE != err )
  {
    return SAM_EFAILED;
  }

  return SAM_ENONE;
}

void
sns_sam_handle_client_init_cb( sns_sam_client_init_data *initData )
{
  uint8_t errOS;

  sns_os_mutex_pend( samClientInitQMutex, 0, &errOS );
  sns_q_put( &samClientInitQ, (sns_q_link_s*)initData );
  sns_os_mutex_post( samClientInitQMutex );

  sns_os_sigs_post( sns_sam_sig_event,
    SNS_SAM_CLIENT_INIT_SIG, OS_FLAG_SET, &errOS );
}

void
sns_sam_init_clients( void )
{
  struct sns_sam_client_init_data *initData = NULL;
  uint8_t errOS;

  SNS_PRINTF_STRING_LOW_0( samModule, "sns_sam_init_clients" );

  do
  {
    sns_os_mutex_pend( samClientInitQMutex, 0, &errOS );
    initData = sns_q_get( &samClientInitQ );
    sns_os_mutex_post( samClientInitQMutex );

    if( NULL != initData )
    {
      sns_sam_init_client( initData );
      SNS_OS_ANY_FREE( initData );
    }
  } while( NULL != initData );
}

sns_sam_err
sns_sam_init_sensor( sns_sam_sensor **sensor )
{
  uint32_t i;

  sns_sam_sensor *sensorOut = SNS_OS_MALLOC( SNS_SAM_DBG_MOD,
    sizeof(sns_sam_sensor));
  SNS_ASSERT(NULL != sensorOut); // Assert if we run out of mem during initialization

  sensorOut->sensorUID = (sns_sam_sensor_uid *)
    SNS_OS_U_MALLOC( SNS_SAM_DBG_MOD, sizeof(sns_sam_sensor_uid));
  SNS_ASSERT(NULL != sensorOut->sensorUID);
  *sensorOut->sensorUID = 0;
  sensorOut->sensorLocation = localProcessor; // As good of a default as any
  sensorOut->isAvailable = false;
  sensorOut->isLocal = false;
  sensorOut->sensorReq.clientHndl = NULL;
  sensorOut->sensorReq.sensor = sensorOut;
  sensorOut->provAPI = NULL;

  for( i = 0; i < SAM_ALGO_ATTR_CNT; i++ )
  {
    sensorOut->attributes[ i ].attribute = i;
    sensorOut->attributes[ i ].supported = false;
  }

  *sensor = sensorOut;

  sns_q_link( sensorOut, (sns_q_link_s*)sensorOut );
  sns_q_put( &sensorQ, (sns_q_link_s*)sensorOut );

  return SAM_ENONE;
}

sns_sam_err
sns_sam_init_fw( void )
{
  sns_sam_err err;
  const uint8_t priority0 = 0;
  uint8_t errOS = 0;

  SNS_PRINTF_STRING_LOW_0( samModule, "Initializing SAM Framework" );

  samClientInitQMutex = sns_os_mutex_create( priority0, &errOS );
  if( 0 != errOS )
  {
    SNS_PRINTF_STRING_FATAL_1( samModule, "Cannot create mutex %i", errOS );
    return SAM_EFAILED;
  }

  sns_q_init( &samClientInitQ );
  sns_q_init( &sensorQ );
  sns_q_init( &algoInstQ );

  // PEND: Combine init function calls here and in sns_sam_main
  // PEND: Turn init list into an array
  err = sns_sam_cb_init_fw();
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_FATAL_1( samModule, "Error initializing cb FW %i", err );
    return SAM_EFAILED;
  }

  err = sns_sam_ind_init_fw();
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_FATAL_1( samModule, "Error initializing ind FW %i", err );
    return SAM_EFAILED;
  }

  err = sns_sam_reg_init_fw();
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_FATAL_1( samModule, "Error initializing registry FW %i", err );
    return SAM_EFAILED;
  }

  err = sns_sam_client_init_fw();
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_FATAL_1( samModule, "Error initializing client FW %i", err );
    return SAM_EFAILED;
  }

  err = sns_sam_service_init_fw();
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_FATAL_1( samModule, "Error initializing service FW %i", err );
    return SAM_EFAILED;
  }

  err = sns_sam_dep_init_fw();
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_FATAL_1( samModule, "Error initializing dep FW %i", err );
    return SAM_EFAILED;
  }

  err = sns_sam_io_init_fw();
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_FATAL_1( samModule, "Error initializing IO FW %i", err );
    return SAM_EFAILED;
  }

  err = sns_sam_prov_init_fw();
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_FATAL_1( samModule, "Error initializing Prov FW %i", err );
    return SAM_EFAILED;
  }

  return SAM_ENONE;
}
