/*============================================================================
  @file sns_sam_req.c

  @brief
  Handles processing of all incoming request messages to the SAM Framework.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include "sns_sam_ped_v01.h"
#include "sns_smgr_api_v01.h"
#include "sns_em.h"
#include "sns_debug_str.h"
#include "sns_sam.h"
#include "sns_sam_algo_api.h"
#include "sns_sam_client.h"
#include "sns_sam_service.h"
#include "sns_sam_cb.h"
#include "sns_sam_req.h"
#include "sns_sam_ind.h"
#include "sns_sam_pm.h"
#include "sns_sam_dep.h"
#include "sns_profiling.h"
#include "sns_sam_io.h"
#include "sns_sam_error.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/
/* Maximum message ID supported by any SAM service */
#define SNS_SAM_MAX_MSG_ID SNS_SAM_ALGO_GET_ATTRIB_REQ

#define SNS_SAM_DC_ON_PERCENT_MAX UINT8_MAX

/*============================================================================
  External Functions
  ===========================================================================*/
extern bool sns_sam_is_algo_uimage_capable(const sns_sam_sensor_algo *algo);

/*============================================================================
  Static Function Definitions
  ===========================================================================*/


/**
 * Mark the specified client request object as valid.
 *
 * @param[i] clientReq Client request to validate
 *
**/
STATIC void sns_sam_mark_client_request_valid( sam_client_req *clientReq )
{
  clientReq->magicNumber =
    SAM_CLIENT_REQ_MAGIC_KEY ^ (intptr_t)clientReq->algoInstance ^ clientReq->extInstanceID ^
    (intptr_t)clientReq->algorithm ^ (intptr_t)clientReq->qLink.q_ptr;
}

/**
 * Mark the specified client request object as invalid.
 *
 * @param[i] clientReq Client request to validate
 *
**/
STATIC void sns_sam_mark_client_request_invalid( sam_client_req *clientReq )
{
  clientReq->magicNumber = 0xFFFFFFFF;
}


/**
 * Mark all matching clients as available, aka "not busy".  Follows
 * q_compare_func_t template.
 *
 * @param[i] itemPtr The current client request to inspect
 * @param[i] compareObj SMR service handle
 *
 * @return 0
 */
STATIC int
sns_sam_mark_client( void *itemPtr, void *compareObject )
{
  sam_client_req *clientReq = (sam_client_req*)itemPtr;
  smr_qmi_client_handle serviceHndl = (smr_qmi_client_handle)compareObject;

  if( clientReq->serviceHndl == serviceHndl )
  {
    clientReq->clientBusy = false;
  }

  return 0;
}

/**
 * Lookup a client request.
 *
 * @param[i] serviceHndl The SMR service handle for this external client
 * @param[i] instanceID The unique ID given to this particular client request
 *
 * @return The client request or NULL if not found
 */
STATIC sam_client_req*
sns_sam_client_req_lookup( smr_qmi_client_handle serviceHndl, uint32_t instanceID )
{
  sns_q_link_s *qCurrAI, *qCurrCR;

  for( qCurrAI = sns_q_check( &algoInstQ ); NULL != qCurrAI;
       qCurrAI = sns_q_next( &algoInstQ, qCurrAI ) )
  {
    sns_sam_algo_inst *algoInst = (sns_sam_algo_inst*)qCurrAI;

    for( qCurrCR = sns_q_check( &algoInst->clientRequests ); NULL != qCurrCR;
         qCurrCR = sns_q_next( &algoInst->clientRequests, qCurrCR ) )
    {
      sam_client_req *clientReq = (sam_client_req*)qCurrCR;

      if( serviceHndl == clientReq->serviceHndl &&
          instanceID == clientReq->extInstanceID )
      {
        return clientReq;
      }
    }
  }

  return NULL;
}

/**
 * Allocate all memory associated with an algorithm instance object.
 *
 * @param[i] algo Algorithm of which we are creating an instance
 * @param[i] bufferSizes Algorithm state buffer size
 *
 * @return Allocated algorithm instance object
 */
STATIC sns_sam_algo_inst*
sns_sam_algo_inst_alloc( sns_sam_sensor_algo const *algo,
    sns_sam_algo_buf_size const *bufferSizes )
{
  sns_sam_algo_inst* algoInst = NULL;
  sns_sam_algoinst_uimage_mode imageMode;

  imageMode = sns_sam_determine_uimage( algo );
  if( SNS_SAM_ALGOINST_BIGIMAGE == imageMode )
  {
    algoInst = SNS_OS_MALLOC( SNS_SAM_DBG_MOD, sizeof(sns_sam_algo_inst) );
    SNS_ASSERT(NULL != algoInst);
    algoInst->imageMode = SNS_SAM_ALGOINST_BIGIMAGE;
  }
  else
  {
    algoInst = SNS_OS_U_MALLOC( SNS_SAM_DBG_MOD, sizeof(sns_sam_algo_inst) );
    if( NULL != algoInst )
    {
      algoInst->imageMode = SNS_SAM_ALGOINST_UIMAGE;
    }
    else
    {
      // There's not enough memory in uheap. Seeking memory in bigImage heap
      algoInst = SNS_OS_MALLOC( SNS_SAM_DBG_MOD, sizeof(sns_sam_algo_inst) );
      SNS_ASSERT(NULL != algoInst);
      algoInst->imageMode = SNS_SAM_ALGOINST_BIGIMAGE;
    }
  }

  SNS_OS_MEMSCPY( &algoInst->bufferSizes, sizeof(sns_sam_algo_buf_size),
      bufferSizes, sizeof(sns_sam_algo_buf_size) );
  algoInst->algoStateData.bufSize = algoInst->bufferSizes.algoStateSize;

  if( SNS_SAM_ALGOINST_UIMAGE == algoInst->imageMode )
  {
    algoInst->algoStateData.buf = (intptr_t)SNS_OS_U_MALLOC(SNS_SAM_DBG_MOD,
        algoInst->bufferSizes.algoStateSize);

    if( NULL == (void*)algoInst->algoStateData.buf )
    {
      SNS_PRINTF_STRING_LOW_0( samModule,
          "Algorithm instance state allocated in DDR due to insufficient TCM" );
      algoInst->imageMode = SNS_SAM_ALGOINST_BIGIMAGE;
      SNS_OS_U_FREE( (void*)algoInst->algoStateData.buf );
    }
    else
    {
      SNS_PRINTF_STRING_LOW_0( samModule,
          "Algorithm instance state allocated in TCM" );
    }
  }

  if( NULL == (void*)algoInst->algoStateData.buf )
  {
    SNS_PRINTF_STRING_LOW_0( samModule,
        "Algorithm instance state allocated in DDR" );
    algoInst->algoStateData.buf = (intptr_t)SNS_OS_MALLOC(SNS_SAM_DBG_MOD,
        algoInst->bufferSizes.algoStateSize);
  }

  SNS_ASSERT(NULL != (void*)algoInst->algoStateData.buf);
  SNS_OS_MEMZERO((void *)algoInst->algoStateData.buf,
      algoInst->algoStateData.bufSize);

  return algoInst;
}

/**
 * Create and start a new algorithm instance.
 *
 * @param[i] algo The associated algorithm
 * @param[i] clientReq Client request to be added to this algorithm instance
 * @param[i] configData Configuration data already generated by the algorithm
 * @param[o] algoInst The created algorithm instance object
 *
 * @return SAM_ENONE
 *         SAM_ENOMEM Not enough memory to allocate algo inst object
 *         SAM_ESTATE Algorithm cannot be created with configData parameters
 *         SAM_EFAILED Unable to initialize algorithm instance or dependencies
 */
STATIC sns_sam_err
sns_sam_create_algo_inst( sns_sam_sensor_algo *algo, sam_client_req *clientReq,
    sns_sam_algo_config const *configData, sns_sam_algo_inst **algoInstOut )
{
  sns_sam_err rv = SAM_ENONE;
  sns_sam_err err;
  sns_sam_algo_inst *algoInst = NULL;
  sns_sam_algo_buf_size bufferSizes;

  SNS_PRINTF_STRING_LOW_1( samModule, "Create new algo inst for %x",
    *algo->sensor.sensorUID );

  bufferSizes.structSize = sizeof(sns_sam_algo_buf_size);
  err = algo->algoAPI->sns_sam_algo_mem_req(
    configData, &bufferSizes );
  if( SAM_EFAILED == err )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
          "Unable to determine buffer sizes for %x",
          *algo->sensor.sensorUID );
    rv = SAM_ESTATE;
  }
  else
  {
    algoInst = sns_sam_algo_inst_alloc( algo, &bufferSizes );
    SNS_ASSERT(NULL != algoInst);

    sns_q_link( algoInst, (sns_q_link_s*)algoInst );
    sns_q_init( &algoInst->clientRequests );
    sns_q_init( (sns_q_s *)&algoInst->algoInputQ );
    algoInst->algorithm = algo;
    algoInst->algoTimer = NULL;

    clientReq->algoInstance = algoInst;
    sns_q_put( &algoInst->clientRequests, &clientReq->qLink );

    cbFuncs.algoHndl = (struct sns_sam_algo_handle*)clientReq;
    err = algo->algoAPI->sns_sam_algo_reset( configData, &cbFuncs,
        &((sns_sam_sensor_algo*)algo)->persistData, &algoInst->algoStateData );

    if( SAM_ENONE != err )
    {
      SNS_PRINTF_STRING_ERROR_2( samModule,
          "Unable to start algorithm instance %x, error %i",
          *algo->sensor.sensorUID, err );
      rv = SAM_ESTATE;
    }
    else
    {
      rv = sns_sam_start_dependencies( algo, algoInst );
    }
  }

  // If there were errors, clean up the algo instance
  if( SAM_ENONE != rv )
  {
    if( NULL != algoInst )
    {
      SNS_OS_ANY_FREE( (void*)algoInst->algoStateData.buf );
      SNS_OS_ANY_FREE( (void*)algoInst );
      algoInst = NULL;
    }
  }

  *algoInstOut = algoInst;
  return rv;
}

/**
 * Removes an algorithm instance, and does all additional necessary cleanup.
 * Should only be called after no client requests remain.
 *
 * @param[i] algoInst Algorithm instance to be removed
 */
STATIC void
sns_sam_remove_algo_inst( sns_sam_algo_inst *algoInst )
{
  SNS_PRINTF_STRING_LOW_1( samModule, "Deleting algo inst %x",
    *algoInst->algorithm->sensor.sensorUID );

  sns_sam_remove_from_error_queue(algoInst);

  sns_q_delete( &algoInst->qLink );

  sns_sam_remove_all_ai_ref( algoInst );
  sns_sam_remove_unused_sensor_reqs();

  sns_sam_input_clear( algoInst );

  if( NULL != algoInst->algoTimer )
  {
    sns_em_delete_timer_obj( algoInst->algoTimer );
  }

  SNS_OS_FREE( (void*)algoInst->algoStateData.buf );
  SNS_OS_MEMZERO((void*)algoInst, sizeof(sns_sam_algo_inst));
  SNS_OS_FREE( (void*)algoInst );
}

/**
 * Generates and returns an unique instance ID.  Must be unique
 * per client-algo connection.  (We'll approximate by making it unique
 * per algorithm).
 *
 * @note Due to inconsistencies between algorithm response messages, we must
 *       not use '0' as a valid external instance ID.
 *
 * @param[i] algo Associated algorithm
 *
 * @return Available instance ID
 */
static uint8_t
sns_sam_create_inst_id( sns_sam_sensor_algo const *algo )
{
  sns_q_link_s *qCurrAI, *qCurrCR;
  static uint8_t proposedID = 0;

  if( 0 == proposedID )
  {
    proposedID++;
  }

  qCurrAI = sns_q_check( &algoInstQ );
  while( NULL != qCurrAI )
  {
    sns_sam_algo_inst *algoInst = (sns_sam_algo_inst*)qCurrAI;

    if( algo == algoInst->algorithm )
    {
      for( qCurrCR = sns_q_check( &algoInst->clientRequests ); NULL != qCurrCR;
           qCurrCR = sns_q_next( &algoInst->clientRequests, qCurrCR ) )
      {
        sam_client_req *clientReq = (sam_client_req*)qCurrCR;

        if( proposedID == clientReq->extInstanceID )
        {
          ++proposedID;
          qCurrAI = sns_q_check( &algoInstQ );
          break;
        }
      }

      if( NULL == qCurrCR )
      {
        qCurrAI = sns_q_next( &algoInstQ, qCurrAI );
      }
    }
    else
    {
      qCurrAI = sns_q_next( &algoInstQ, qCurrAI );
    }
  }

  return proposedID++;
}


/**
 * Create a client request object from the SAM enable request message.
 * Inialize all applicable fields, but do not add to global queue.
 *
 * @param[i] reqMsg The SAM enable request message
 * @param[i] algo Corresponding algorithm from reqMsg
 * @param[o] clientReqOut Generated client request object
 *
 * @return SAM_ENONE
 *         SAM_ENOMEM Not enough memory to allocate client request object
 */
STATIC sns_sam_err
sns_sam_create_client_req( sns_sam_req_msg const *reqMsg,
    sns_sam_sensor_algo const *algo, sam_client_req **clientReqOut )
{
  sam_client_req *clientReq;

  // PEND: Should this be a U_MALLOC?
  clientReq = SNS_OS_ANY_MALLOC( SNS_SAM_DBG_MOD, sizeof(sam_client_req) );
  SNS_ASSERT(NULL != clientReq);
  *clientReqOut = clientReq;

  clientReq->extInstanceID = sns_sam_create_inst_id( algo );
  clientReq->serviceHndl = reqMsg->serviceHndl;
  clientReq->clientBusy = false;
  clientReq->missedEvent = false;
  clientReq->algorithm = algo;
  clientReq->batchPeriod = 0;
  clientReq->nextReportTS = 0;
  clientReq->outputSampleCnt = 0;
  clientReq->lastIndType = SAM_BATCH_ONLY_IND_V01;
  clientReq->clientTimer = NULL;
  sns_q_init( &clientReq->outputDataQ );
  sns_q_init( &clientReq->flushReqQ );

  SNS_PRINTF_STRING_LOW_3( samModule, "Setting report ID for request for %x to %i (%i)",
   *reqMsg->msg.sensorUID, clientReq->extInstanceID, reqMsg->serviceHndl );

  sns_q_link( clientReq, &clientReq->qLink );

  return SAM_ENONE;
}

/**
 * Form the response message.
 *
 * @param[i] instanceID Resulting instance ID
 * @param[i] error Sensor1 error, or SUCCESS
 * @param[io] respMsg
 */
STATIC void
sns_sam_create_enable_resp( uint8_t instanceID, sensor1_error_e error,
    struct sns_sam_msg *respMsg )
{
  sns_sam_ped_enable_resp_msg_v01 *enableResp;
  enableResp = (sns_sam_ped_enable_resp_msg_v01*)respMsg->msg.buf;

  enableResp->resp.sns_result_t = ( SENSOR1_SUCCESS == error )
    ? SNS_RESULT_SUCCESS_V01 : SNS_RESULT_FAILURE_V01;
  enableResp->resp.sns_err_t = error;

  enableResp->instance_id_valid = instanceID;
  enableResp->instance_id = instanceID;
}

/**
 * Try and find a matching algorithm instance for a new client request.
 *
 * @param[i] configData Configuration of the new request
 * @param[i] algo Algorithm that the client is requesting
 *
 * @return Acceptable algorithm instance to use, or NULL
 */
STATIC sns_sam_algo_inst*
sns_sam_match_algo_inst( sns_sam_algo_config const *configData,
    sns_sam_sensor_algo const *algo )
{
  sns_q_link_s *qCurr;
  sns_sam_algo_inst *algoInst = NULL;

  for( qCurr = sns_q_check( &algoInstQ ); NULL != qCurr;
        qCurr = sns_q_next( &algoInstQ, qCurr ) )
  {
    algoInst = (sns_sam_algo_inst*)qCurr;

    if( algoInst->algorithm == algo &&
        algo->algoAPI->sns_sam_algo_check_config(
          &algoInst->algoStateData, configData ))
    {
      return algoInst;
    }
  }

  return NULL;
}


/**
 * Validate the reporting mode in the client attributes
 *
 * @param[i] supportedMode Reporting modes supported by this algorithm
 * @param[i] clientMode Reporting mode to be used with this client
 *
 * @return true if clientMode is a valid reporting mode
 *         false otherwise
 */
STATIC bool
sns_sam_validate_reporting_mode( uint32_t supportedModes,
  sns_sam_report_mode clientMode)
{
  switch(clientMode)
  {
    case SNS_SAM_REPORT_MODE_PERIODIC:
      return (supportedModes & SNS_SAM_PERIODIC_REPORT_V01)? true : false;
    case SNS_SAM_REPORT_MODE_SYNC:
     return (supportedModes & SNS_SAM_SYNC_REPORT_V01)? true : false;
    case SNS_SAM_REPORT_MODE_ASYNC:
     return (supportedModes & SNS_SAM_ASYNC_REPORT_V01)? true : false;
    default:
      return false;
  }
}


/**
 * Process an enable request from some client.  Generates client request
 * and algorithm instances as necessary, and adds them to the global queues.
 *
 * @param[i] reqMsg Incoming request message
 * @param[o] Response message to be generated
 *
 * @return SAM_ENONE
 *         SAM_ENOMEM Not enough memory to allocate necessary objects
 *         SAM_ESTATE Algorithm will not initialize
 *         SAM_EFAILED Error creating new algorithm instance
 */
STATIC sns_sam_err
sns_sam_handle_enable_req( sns_sam_req_msg const *reqMsg,
    struct sns_sam_msg *respMsg )
{
  sns_sam_err rv = SAM_EFAILED, err;
  sns_sam_sensor_algo *algo = NULL;
  sns_sam_algo_inst *algoInst = NULL;
  sns_sam_algo_config configData;
  sam_client_req *clientReq;

  algo = sns_sam_lookup_algo( reqMsg->msg.sensorUID );

  if( NULL != algo )
  {
    sns_profiling_log_qdss( SNS_SAM_ALGO_ENABLE_ENTER, 1, *algo->sensor.sensorUID );
    err = sns_sam_create_client_req( reqMsg, algo, &clientReq );
    if( SAM_ENOMEM != err )
    {
      configData.buf = (intptr_t)SNS_OS_MALLOC( SNS_SAM_DBG_MOD,
      algo->bufferSizes.configDataSize );
      SNS_ASSERT(NULL != (void*)configData.buf);

      err = algo->algoMsgAPI->sns_sam_algo_process_enable_req(
          (sns_sam_enable_req*)&reqMsg->msg, &algo->persistData,
          &clientReq->clientAttr, &configData );
      if( SAM_ENONE != err || !sns_sam_validate_reporting_mode(
        algo->sensor.attributes[SAM_ALGO_ATTR_REPORT_MODE].attrValue,
        clientReq->clientAttr.reportMode ))
      {
        SNS_PRINTF_STRING_FATAL_2( samModule,
          "Unable to process enable req for %x, error %i",
          *reqMsg->msg.sensorUID, err );
        SNS_OS_ANY_FREE( clientReq );
        rv = SAM_ESTATE;
      }
      else
      {
        if( SNS_SAM_REPORT_MODE_PERIODIC == clientReq->clientAttr.reportMode )
        {
          clientReq->nextReportTS = sns_em_get_timestamp();
          clientReq->nextReportTS +=
            sns_em_convert_sec_in_q16_to_localtick( clientReq->clientAttr.reportPeriod );
        }

        // PEND: Check sample/report rate to be in valid range (per attributes)
        algoInst = sns_sam_match_algo_inst( &configData, algo );

        if( NULL == algoInst )
        {
          err = sns_sam_create_algo_inst( algo, clientReq, &configData, &algoInst );
          if( SAM_ENONE != err )
          {
            SNS_PRINTF_STRING_FATAL_2( samModule,
              "Error creating algo inst %x, error %i",
              *reqMsg->msg.sensorUID, err );
            SNS_OS_ANY_FREE( clientReq );
            rv = err;
          }
          else
          {
            void *head = sns_q_check( &algoInstQ );
            SNS_PRINTF_STRING_LOW_1( samModule,
              "Add new algo instance to queue %x",
              *algoInst->algorithm->sensor.sensorUID );

            if( SNS_SAM_ALGOINST_UIMAGE == algoInst->imageMode || NULL == head )
            {
              sns_q_put( &algoInstQ, &algoInst->qLink );
            }
            else
            {
              sns_q_insert( &algoInst->qLink, head );
            }
          }
        }
        else
        {
          sns_q_put( &algoInst->clientRequests, &clientReq->qLink );
          clientReq->algoInstance = algoInst;
        }

        if( NULL != algoInst )
        {
          sns_sam_register_client_timer( clientReq );
          sns_sam_inform_report_change( clientReq->algoInstance );
          rv = SAM_ENONE;

          sns_profiling_log_qdss( SNS_SAM_ALGO_ENABLE_EXIT, 1, *algo->sensor.sensorUID );
          if( algoInst->clientRequests.cnt > 1 &&
              SNS_SAM_REPORT_MODE_PERIODIC != clientReq->clientAttr.reportMode )
          {
            sns_sam_handle_report( clientReq );
          }
          sns_sam_mark_client_request_valid(clientReq);
        }
      }
      SNS_OS_ANY_FREE( (void*)configData.buf );
    }
    else
    {
      SNS_PRINTF_STRING_FATAL_1( samModule,
        "Unable to create client request %x", *reqMsg->msg.sensorUID );
      rv = SAM_ENOMEM;
    }
  }
  else
  {
    SNS_PRINTF_STRING_FATAL_1( samModule,
      "Unable to find SAM algorithm %x", *reqMsg->msg.sensorUID );
  }

  if( SAM_ENONE != rv )
  {
    sensor1_error_e errCode =
      ( SAM_ENOMEM == rv ) ? SENSOR1_ENOMEM :
      ( SAM_ESTATE == rv ) ? SENSOR1_EBAD_PARAM :
      ( SAM_EFAILED == rv ) ? SENSOR1_ENOTALLOWED : SENSOR1_EFAILED;

    sns_sam_create_enable_resp( 0, errCode, respMsg );
  }
  else
  {
    sns_sam_create_enable_resp( clientReq->extInstanceID,
      SENSOR1_SUCCESS, respMsg );
  }

  return rv;
}

/**
 * Process a disable request from some client.
 *
 * @param[i] reqMsg Incoming request message
 * @param[o] respMsg Response message to be generated
 *
 * @return SAM_ENONE
 */
STATIC sns_sam_err
sns_sam_handle_disable_req( sns_sam_req_msg const *reqMsg,
    struct sns_sam_msg *respMsg )
{
  sam_client_req *clientReq;
  sns_sam_ped_disable_resp_msg_v01 *disableResp =
    (sns_sam_ped_disable_resp_msg_v01*)respMsg->msg.buf;
  sns_sam_ped_disable_req_msg_v01 const *disableReq =
    (sns_sam_ped_disable_req_msg_v01*)reqMsg->msg.msg.buf;

  SNS_PRINTF_STRING_LOW_1( samModule,
    "Disable client request %i", disableReq->instance_id );

  clientReq = sns_sam_client_req_lookup(
    reqMsg->serviceHndl, disableReq->instance_id );

  if( NULL != clientReq )
  {
    sns_sam_remove_client_req( clientReq, NULL );
  }

  disableResp->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  disableResp->instance_id_valid = disableReq->instance_id;
  disableResp->instance_id = disableReq->instance_id;

  return SAM_ENONE;
}

/**
 * Process a get report request from some client.
 *
 * @param[i] reqMsg Incoming request message
 * @param[o] Response message to be generated
 *
 * @return SAM_ENONE
 *         SAM_ETYPE Cannot find requested client request
 *         SAM_ENOMEM Not enough memory to generate report
 *         SAM_ESTATE Report generation is not available
 *         SAM_EFAILED Unable to generate the report
 */
STATIC sns_sam_err
sns_sam_handle_get_report_req( sns_sam_req_msg const *reqMsg,
    struct sns_sam_msg *respMsg )
{
  sns_sam_err err;
  sns_sam_err rv = SAM_ENONE;
  sam_client_req *clientReq = NULL;
  sns_sam_ped_get_report_req_msg_v01 const *reportReq =
    (sns_sam_ped_get_report_req_msg_v01*)reqMsg->msg.msg.buf;
  sns_sam_ped_get_report_resp_msg_v01 *reportResp =
    (sns_sam_ped_get_report_resp_msg_v01*)respMsg->msg.buf;

  clientReq = sns_sam_client_req_lookup(
      reqMsg->serviceHndl, reportReq->instance_id );
  if( NULL != clientReq )
  {
    sns_sam_algo_inst *algoInst = clientReq->algoInstance;
    sns_sam_algo_output outputData;

    err = sns_sam_generate_output( algoInst, &outputData );
    if( SAM_ENONE == err )
    {
      err = algoInst->algorithm->algoMsgAPI->sns_sam_algo_gen_report(
        &outputData, respMsg );

      reportResp->instance_id_valid = reportReq->instance_id;
      reportResp->instance_id = reportReq->instance_id;
      if( SAM_ENONE != err )
      {
        SNS_PRINTF_STRING_FATAL_1( samModule,
          "Error generating report %x", *reqMsg->msg.sensorUID );
        rv = SAM_EFAILED;
      }

      SNS_OS_ANY_FREE( (void*)outputData.data );
    }
    else
    {
      SNS_PRINTF_STRING_FATAL_1( samModule,
        "Error generating output %x", *reqMsg->msg.sensorUID );
      rv = SAM_ESTATE;
    }
  }
  else
  {
    rv = SAM_ETYPE;
  }

  return rv;
}

/**
 * Process a get attribute request from some client.
 *
 * @param[i] reqMsg Incoming request message
 * @param[o] Response message to be generated
 *
 * @return SAM_ENONE
 *         SAM_ETYPE Cannot find requested algorithm
 */
STATIC sns_sam_err
sns_sam_handle_attr_req( sns_sam_req_msg const *reqMsg,
    struct sns_sam_msg *respMsg )
{
  sns_sam_sensor_algo *algo = NULL;
  sns_sam_get_algo_attrib_resp_msg_v01 *reportResp =
    (sns_sam_get_algo_attrib_resp_msg_v01*)respMsg->msg.buf;

  algo = sns_sam_lookup_algo( reqMsg->msg.sensorUID );

  if( NULL != algo )
  {
    reportResp->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;

    reportResp->proc_type = localProcessor;
    reportResp->max_batch_size =
      algo->sensor.attributes[ SAM_ALGO_ATTR_MAX_BATCH ].attrValue;
    reportResp->supported_reporting_modes =
      algo->sensor.attributes[ SAM_ALGO_ATTR_REPORT_MODE ].attrValue;
    reportResp->algorithm_revision =
      algo->sensor.attributes[ SAM_ALGO_ATTR_REVISION ].attrValue;
    reportResp->min_report_rate =
      algo->sensor.attributes[ SAM_ALGO_ATTR_MIN_REPORT ].supported ?
      algo->sensor.attributes[ SAM_ALGO_ATTR_MIN_REPORT ].attrValue : 0;
    reportResp->max_report_rate =
      algo->sensor.attributes[ SAM_ALGO_ATTR_MAX_REPORT ].supported ?
      algo->sensor.attributes[ SAM_ALGO_ATTR_MAX_REPORT ].attrValue : 0;
    reportResp->min_sample_rate =
      algo->sensor.attributes[ SAM_ALGO_ATTR_MIN_SAMPLE ].supported ?
      algo->sensor.attributes[ SAM_ALGO_ATTR_MIN_SAMPLE ].attrValue : 0;
    reportResp->max_sample_rate =
      algo->sensor.attributes[ SAM_ALGO_ATTR_MAX_SAMPLE ].supported ?
      algo->sensor.attributes[ SAM_ALGO_ATTR_MAX_SAMPLE ].attrValue : 0;
    reportResp->power = algo->sensor.attributes[ SAM_ALGO_ATTR_POWER ].attrValue;
    
	reportResp->reserved_batch_size_valid = true;
    if( algo->sensor.attributes[ SAM_ALGO_ATTR_RESERVED_FIFO_SIZE ].supported &&
        0 != algo->sensor.attributes[ SAM_ALGO_ATTR_RESERVED_FIFO_SIZE ].attrValue )
    {
      reportResp->reserved_batch_size =
        algo->sensor.attributes[ SAM_ALGO_ATTR_RESERVED_FIFO_SIZE ].attrValue;
    } else if( reportResp->max_batch_size > 150 )
    {
       reportResp->reserved_batch_size = SNS_SAM_DEFAULT_RESERVED_FIFO_SIZE;
    }
	else
    {
        reportResp->reserved_batch_size = reportResp->max_batch_size;
    }

    reportResp->sensorUID_valid = true;
    reportResp->sensorUID = *algo->sensor.sensorUID;
  }
  else
  {
    return SAM_ETYPE;
  }

  return SAM_ENONE;
}

/**
 * Process a batch request from some client.
 *
 * @param[i] reqMsg Incoming request message
 * @param[o] Response message to be generated
 *
 * @return SAM_ENONE
 *         SAM_ETYPE Cannot find associated client request
 *         SAM_ENOT_AVAILABLE No response message available
 */
STATIC sns_sam_err
sns_sam_handle_batch_req( sns_sam_req_msg const *reqMsg,
    struct sns_sam_msg *respMsg )
{
  sns_sam_err rv = SAM_ENONE;
  sns_sam_ped_batch_req_msg_v01 const *batchReq =
    (sns_sam_ped_batch_req_msg_v01*)reqMsg->msg.msg.buf;
  sns_sam_ped_batch_resp_msg_v01 *batchResp =
    (sns_sam_ped_batch_resp_msg_v01*)respMsg->msg.buf;
  sns_sam_sensor *sensor;

  sensor = sns_sam_lookup_sensor_from_suid( reqMsg->msg.sensorUID );
  SNS_ASSERT(NULL != sensor);

  batchResp->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  batchResp->max_batch_size_valid = true;
  batchResp->max_batch_size = sensor->attributes[ SAM_ALGO_ATTR_MAX_BATCH ].attrValue;
  batchResp->timestamp_valid = true;
  batchResp->timestamp = sns_em_get_timestamp();

  batchResp->instance_id_valid = true;
  batchResp->instance_id = batchReq->instance_id;

  if( !batchReq->req_type_valid ||
      SNS_BATCH_GET_MAX_FIFO_SIZE_V01 != batchReq->req_type )
  {
    sam_client_req *clientReq = sns_sam_client_req_lookup(
        reqMsg->serviceHndl, batchReq->instance_id );

    if( NULL != clientReq )
    {
      q16_t batchPeriod = (q16_t)batchReq->batch_period;
      bool delayedResp = false;

      SNS_PRINTF_STRING_LOW_3( samModule,
          "Received batch request (IID:%d, batch period: %d --> %i ms",
          batchReq->instance_id,
          batchPeriod,
          FX_FIXTOFLT_Q16_SP(batchPeriod) * 1000 );

      if( ( 0 != clientReq->batchPeriod || 0 != clientReq->clientAttr.reportPeriod )
          && !clientReq->clientBusy )
      {
        delayedResp = sns_sam_initiate_flush( clientReq, reqMsg->reqHndl );

        if( SNS_PROC_APPS_V01 == clientReq->clientAttr.notifySuspend.proc_type )
        {
          sns_sam_flush_batched_ap_output();
        }
      }

      if( delayedResp )
      {
        sns_sam_flush_req *flushReq =
          (sns_sam_flush_req*)sns_q_last_check( &clientReq->flushReqQ );
        flushReq->batchPeriod = batchPeriod;

        rv = SAM_ENOT_AVAILABLE;
      }
      else if( clientReq->batchPeriod != batchPeriod )
      {
        clientReq->batchPeriod = batchPeriod;
        sns_sam_register_client_timer( clientReq );
        sns_sam_inform_report_change( clientReq->algoInstance );
      }
    }
    else
    {
      rv = SAM_ETYPE;
      SNS_PRINTF_STRING_ERROR_3( samModule, "No instance %i found for %x (%i)",
          batchReq->instance_id, *reqMsg->msg.sensorUID, reqMsg->serviceHndl );
    }
  }

  return rv;
}

/**
 * Process an update request from some client.
 *
 * @param[i] reqMsg Incoming request message
 * @param[o] Response message to be generated
 *
 * @return SAM_ENONE
 *         SAM_EFAILED No response message is available
 *         SAM_ETYPE Cannot find associated client request
 */
STATIC sns_sam_err
sns_sam_handle_update_req( sns_sam_req_msg const *reqMsg,
    struct sns_sam_msg *respMsg )
{
  sam_client_req *clientReq = NULL;
  sns_sam_err err;
  sns_sam_ped_reset_req_msg_v01 const *updateReq =
    (sns_sam_ped_reset_req_msg_v01*)reqMsg->msg.msg.buf;
  sns_sam_ped_update_batch_period_resp_msg_v01 *updateResp =
    (sns_sam_ped_update_batch_period_resp_msg_v01*)respMsg->msg.buf;

  clientReq = sns_sam_client_req_lookup(
      reqMsg->serviceHndl, updateReq->instance_id );

  if( NULL != clientReq )
  {
    cbFuncs.algoHndl = (struct sns_sam_algo_handle*)clientReq;
    err = clientReq->algorithm->algoMsgAPI->sns_sam_algo_process_update_req(
      &cbFuncs, &reqMsg->msg, &clientReq->algorithm->persistData,
      &clientReq->algoInstance->algoStateData, respMsg );

    updateResp->instance_id_valid = true;
    updateResp->instance_id = clientReq->extInstanceID;

    if( SAM_ENONE != err )
    {
      SNS_PRINTF_STRING_ERROR_2( samModule,
        "Error processing update request for %x, error: %i",
        *clientReq->algorithm->sensor.sensorUID, err );
      err = SAM_EFAILED;
    }
  }
  else
  {
    return SAM_ETYPE;
  }

  return SAM_ENONE;
}

/**
 * Process a cancel request, and generate an appropriate response message.
 *
 * @param[i] reqMsg Incoming request message
 * @param[o] Response message to be generated
 *
 * @return SAM_ENONE
 */
STATIC sns_sam_err
sns_sam_handle_cancel_req( sns_sam_req_msg const *reqMsg,
    struct sns_sam_msg *respMsg )
{
  sns_common_cancel_resp_msg_v01 *cancelResp =
    (sns_common_cancel_resp_msg_v01*)respMsg->msg.buf;

  SNS_PRINTF_STRING_LOW_1( samModule,
    "Removing clients from %i", reqMsg->serviceHndl );

  sns_sam_remove_all_client_req( reqMsg->serviceHndl );

  cancelResp->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;

  return SAM_ENONE;
}

/**
 * Generate a version response message.
 *
 * @param[i] reqMsg Incoming request message
 * @param[o] Response message to be generated
 *
 * @return SAM_ENONE
 *         SAM_ETYPE Cannot find associated algorithm
 */
STATIC sns_sam_err
sns_sam_handle_version_req( sns_sam_req_msg const *reqMsg,
    struct sns_sam_msg *respMsg )
{
  UNREFERENCED_PARAMETER(reqMsg);
  sns_sam_sensor *sensor;
  sns_common_version_resp_msg_v01 *versionResp =
    (sns_common_version_resp_msg_v01*)respMsg->msg.buf;

  sensor = sns_sam_lookup_sensor_from_suid( reqMsg->msg.sensorUID );

  if( NULL != sensor )
  {
    versionResp->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
    qmi_idl_get_idl_minor_version( sensor->serviceObj,
        &versionResp->interface_version_number );
    versionResp->max_message_id = SNS_SAM_MAX_MSG_ID;
  }
  else
  {
    return SAM_ETYPE;
  }

  return SAM_ENONE;
}

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

void
sns_sam_remove_client_req( sam_client_req *clientReq, void *unused )
{
  UNREFERENCED_PARAMETER(unused);

  sns_q_delete( &clientReq->qLink );
  SNS_PRINTF_STRING_LOW_1( samModule, "Removing client request %i",
    clientReq->extInstanceID );

  if( NULL != clientReq->clientTimer )
  {
    if(sns_em_is_utimer(clientReq->clientTimer))
    {
      sns_em_delete_utimer_obj( clientReq->clientTimer );
    }
    else
    {
      sns_em_delete_timer_obj( clientReq->clientTimer );
    }

    clientReq->clientTimer = NULL;
  }

  sns_sam_send_batch_resp( clientReq );
  sns_sam_output_clear( clientReq );

  if( 0 == sns_q_cnt( &clientReq->algoInstance->clientRequests ) )
  {
    sns_sam_remove_algo_inst( clientReq->algoInstance );
  }
  else
  {
    sns_sam_inform_report_change( clientReq->algoInstance );
  }

  sns_sam_mark_client_request_invalid(clientReq);
  SNS_OS_ANY_FREE( (void*)clientReq );
}

void
sns_sam_remove_all_client_req( smr_qmi_client_handle serviceHndl )
{
  sns_q_link_s *qCurrAI, *qCurrCR;

  for( qCurrAI = sns_q_check( &algoInstQ ); NULL != qCurrAI; )
  {
    sns_sam_algo_inst *algoInst = (sns_sam_algo_inst*)qCurrAI;
    qCurrAI = sns_q_next( &algoInstQ, qCurrAI );

    for( qCurrCR = sns_q_check( &algoInst->clientRequests ); NULL != qCurrCR; )
    {
      sam_client_req *clientReq = (sam_client_req*)qCurrCR;
      qCurrCR = sns_q_next( &algoInst->clientRequests, qCurrCR );

      if( clientReq->serviceHndl == serviceHndl )
      {
        sns_sam_remove_client_req( clientReq, NULL );
      }
    }
  }
}

void
sns_sam_handle_req( sns_sam_req_msg const *reqMsg )
{
  sns_sam_err err = SAM_ENOT_SUPPORTED;
  sensor1_error_e sensor1Err = SENSOR1_EFAILED;

  SNS_PRINTF_STRING_LOW_1( samModule, "sns_sam_handle_req msgID %i",
    reqMsg->msg.msgID );

  struct sns_sam_msg *respMsg = (struct sns_sam_msg *)&reqMsg->respMsg;

  if( SNS_SAM_ALGO_CANCEL_REQ == reqMsg->msg.msgID )
  {
    err = sns_sam_handle_cancel_req( reqMsg, respMsg );
  }
  else if( SNS_SAM_ALGO_VERSION_REQ == reqMsg->msg.msgID )
  {
    err = sns_sam_handle_version_req( reqMsg, respMsg );
  }
  else if( SNS_SAM_ALGO_ENABLE_REQ == reqMsg->msg.msgID )
  {
    err = sns_sam_handle_enable_req( reqMsg, respMsg );
  }
  else if( SNS_SAM_ALGO_DISABLE_REQ == reqMsg->msg.msgID )
  {
    err = sns_sam_handle_disable_req( reqMsg, respMsg );
  }
  else if( SNS_SAM_ALGO_GET_REPORT_REQ == reqMsg->msg.msgID )
  {
    err = sns_sam_handle_get_report_req( reqMsg, respMsg );
  }
  else if( SNS_SAM_ALGO_UPDATE_REQ == reqMsg->msg.msgID )
  {
    err = sns_sam_handle_update_req( reqMsg, respMsg );
  }
  else if( SNS_SAM_ALGO_BATCH_REQ == reqMsg->msg.msgID )
  {
    err = sns_sam_handle_batch_req( reqMsg, respMsg );
  }
  else if( SNS_SAM_ALGO_GET_ATTRIB_REQ == reqMsg->msg.msgID )
  {
    err = sns_sam_handle_attr_req( reqMsg, respMsg );
  }
  else
  {
    SNS_PRINTF_STRING_FATAL_2( samModule,
        "Received unknown message ID %i for %x",
        reqMsg->msg.msgID, *reqMsg->msg.sensorUID );
  }

  switch(err)
  {
    case SAM_ENONE:
      sensor1Err = SENSOR1_SUCCESS;
      break;
    case SAM_ETYPE:
      sensor1Err = SENSOR1_ENOTALLOWED;
      break;
    case SAM_ENOMEM:
      sensor1Err = SENSOR1_ENOMEM;
      break;
    default:
      sensor1Err = SENSOR1_EFAILED;
      break;
  }

  if( SAM_ENOT_AVAILABLE != err )
  {
    if( SAM_ENONE != err )
    {
      // PEND: Convert SAM error into Sensor1 error
      // Respond with a generic error
      sns_common_version_resp_msg_v01 *versionResp
        = (sns_common_version_resp_msg_v01*)respMsg->msg.buf;
      versionResp->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      versionResp->resp.sns_err_t = sensor1Err;

      SNS_PRINTF_STRING_ERROR_3( samModule,
          "Error processing message %i for %x: %i",
          reqMsg->msg.msgID, *reqMsg->msg.sensorUID, err );
    }

    err = sns_sam_service_send_resp( reqMsg, respMsg );
    if( SAM_ENONE != err )
    {
      SNS_PRINTF_STRING_ERROR_2( samModule,
          "Error sending response message",
          respMsg->sensorUID, err );
    }
  }
}

void
sns_sam_send_batch_resp( sam_client_req *clientReq )
{
  sns_sam_err err;
  q16_t batchPeriod = clientReq->batchPeriod;
  sns_q_link_s *qCurr = sns_q_get( &clientReq->flushReqQ );
  sns_sam_req_msg reqMsg;
  struct sns_sam_msg respMsg;
  sns_sam_ped_batch_resp_msg_v01 batchResp;
  sns_sam_sensor *sensor = &clientReq->algoInstance->algorithm->sensor;

  respMsg.msg.buf = (intptr_t)&batchResp;
  respMsg.msg.bufSize = sizeof(sns_sam_ped_batch_resp_msg_v01);
  respMsg.msgID = SNS_SAM_ALGO_BATCH_RESP;
  respMsg.sensorUID = sensor->sensorUID;

  batchResp.resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  batchResp.resp.sns_err_t = 0;
  batchResp.max_batch_size_valid = true;
  batchResp.max_batch_size = sensor->attributes[ SAM_ALGO_ATTR_MAX_BATCH ].attrValue;
  batchResp.timestamp_valid = true;
  batchResp.timestamp = sns_em_get_timestamp();
  batchResp.instance_id_valid = true;

  while( NULL != qCurr )
  {
    sns_sam_flush_req *flushReq = (sns_sam_flush_req*)qCurr;
    qCurr = sns_q_get( &clientReq->flushReqQ );

    batchPeriod = flushReq->batchPeriod;

    if( NULL != flushReq->reqHndl )
    {
      reqMsg.reqHndl = flushReq->reqHndl;
      batchResp.instance_id = clientReq->extInstanceID;

      err = sns_sam_service_send_resp( &reqMsg, &respMsg );
      if( SAM_ENONE != err )
      {
        SNS_PRINTF_STRING_ERROR_2( samModule,
            "Error sending response message %i, %x",
            err, *respMsg.sensorUID );
      }
    }
    SNS_OS_ANY_FREE( flushReq );
  }

  // Don't bother going through all this trouble, if there aren't any
  // clients left for this algorithm instance (i.e. they were just removed).
  if( clientReq->batchPeriod != batchPeriod &&
      0 != sns_q_cnt( &clientReq->algoInstance->clientRequests ) )
  {
    clientReq->batchPeriod = batchPeriod;
    sns_sam_register_client_timer( clientReq );
    sns_sam_inform_report_change( clientReq->algoInstance );
  }
}

void
sns_sam_mark_client_avail( smr_service_hndl serviceHndl )
{
  sns_q_link_s *qCurr;

  for( qCurr = sns_q_check( &algoInstQ ); NULL != qCurr;
       qCurr = sns_q_next( &algoInstQ, qCurr ) )
  {
    sns_sam_algo_inst *algoInst = (sns_sam_algo_inst*)qCurr;

    sns_q_linear_search( &algoInst->clientRequests,
      &sns_sam_mark_client, serviceHndl );
  }
}

SNS_SAM_UIMAGE_CODE sns_sam_err
sns_sam_generate_output( sns_sam_algo_inst *algoInst, sns_sam_algo_output *outputDataIn )
{
  sns_sam_err err;
  bool algoInstPrevUimageState = SNS_SAM_ALGOINST_IN_UIMAGE(algoInst);
  sns_sam_algo_output outputData;
  outputDataIn->data = (intptr_t)NULL;

  if( !algoInstPrevUimageState )
  {
    sns_sam_uimage_exit();
  }

  if( NULL == algoInst->algorithm->algoAPI->sns_sam_algo_generate )
  {
    err = SAM_EFAILED;
  }
  else
  {
    outputData.data = (intptr_t)SNS_OS_ANY_MALLOC(SNS_SAM_DBG_MOD, sizeof(sns_sam_algo_output) +
      algoInst->algorithm->bufferSizes.outputDataSize);

    if( NULL == outputData.data )
    {
      err = SAM_ENOMEM;
    }
    else
    {
      cbFuncs.algoHndl = (struct sns_sam_algo_handle*)
        sns_q_check( &algoInst->clientRequests );
      err = algoInst->algorithm->algoAPI->sns_sam_algo_generate(
          &algoInst->algoStateData, &cbFuncs, &outputData );

      if( err == SAM_ENONE )
      {
        *outputDataIn = outputData;
      }
      else
      {
        SNS_OS_ANY_FREE( (void*)outputData.data );
        outputData.data = (intptr_t)NULL;
      }
    }
  }

  return err;
}
