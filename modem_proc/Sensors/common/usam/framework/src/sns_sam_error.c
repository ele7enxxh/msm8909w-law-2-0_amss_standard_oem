/*============================================================================
  @file sns_sam_error.c

  @brief
  Handles processing of all errors in the SAM Framework.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include "sns_sam_error.h"
#include "sns_sam.h"
#include "sns_sam_client.h"
#include "sns_sam_dep.h"
#include "sns_sam_req.h"
#include "sns_sam_ped_v01.h"
#include "sns_sam_service.h"

/*============================================================================
  Static Data
  ===========================================================================*/

/* Queue of sns_sam_client_error objects associated with error callbacks */
STATIC sns_q_s samClientErrCBQ;
STATIC OS_EVENT *samClientErrCBQMutex;

/*============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Form and send a SAM error indication message to a particular client.
 *
 * @param[i] sensor1Err The Sensor1 API error code
 * @param[i] clientReq SAM client which deserves the error message
 *
 * @return SAM_ENONE
 *         SAM_ENOMEM Not enough memory to allocate indication
 *         SAM_EFAILED Unable to send message to client
 */
STATIC sns_sam_err
sns_sam_send_error_ind( sensor1_error_e sensor1Err, sam_client_req const *clientReq )
{
  sns_sam_err err;
  sns_sam_ind_msg indMsg;
  sns_sam_ped_error_ind_msg_v01 errorInd;
  int32_t errQMI;

  errQMI = qmi_idl_get_message_c_struct_len(
    clientReq->algorithm->sensor.serviceObj, QMI_IDL_INDICATION,
    SNS_SAM_ALGO_ERROR_IND, &indMsg.msg.bufSize  );
  if( QMI_IDL_LIB_NO_ERR == errQMI )
  {
    indMsg.msg.buf = (intptr_t)&errorInd;
    SNS_OS_MEMZERO( (void*)indMsg.msg.buf, indMsg.msg.bufSize );
    indMsg.msgID = SNS_SAM_ALGO_ERROR_IND;
    indMsg.sensorUID = clientReq->algorithm->sensor.sensorUID;

    errorInd.instance_id = clientReq->extInstanceID;
    errorInd.error = sensor1Err;

    SNS_PRINTF_STRING_HIGH_2( samModule,
      "Sending error ind of size %d with ptr %x",
      indMsg.msg.bufSize, indMsg.msg.buf );

    err = sns_sam_service_send_ind( &indMsg, clientReq );
    if( SAM_ENONE != err )
    {
      SNS_PRINTF_STRING_ERROR_2( samModule,
        "Error sending error ind for %x, error: %i",
        *clientReq->algorithm->sensor.sensorUID, err );
      return SAM_EFAILED;
    }
  }
  else
  {
    SNS_PRINTF_STRING_ERROR_2( samModule,
      "Error getting struct size for %x, error: %i",
      *clientReq->algorithm->sensor.sensorUID, errQMI );
    return SAM_EFAILED;
  }

  return SAM_ENONE;
}

/**
 * Disable and unregister the algorithm. Disable and unregister all algorithms
 * that are dependent on this sensor.
 *
 * @param[i] suid     Sensor UID for the algorithm to be disabled and unregisterd
 */
STATIC void
sns_sam_disable_sensor_algo(sns_sam_sensor_uid *suid)
{
  sns_q_link_s *qCurr;
  for( qCurr = sns_q_check( &sensorQ ); NULL != qCurr;
      qCurr = sns_q_next( &sensorQ, qCurr ) )
  {
    uint32_t i;
    sns_sam_sensor_algo *algo = (sns_sam_sensor_algo*)qCurr;
    if( !algo->sensor.isLocal )
    {
      continue;
    }
    for( i = 0; i < SNS_SAM_MAX_DEPENDENT_SENSOR; i++ )
    {
      if( NULL != algo->dependencies[ i ] &&
          *suid == *algo->dependencies[ i ]->sensorUID )
      {
        if( NULL != algo->serviceProvider )
        {
          SNS_PRINTF_STRING_MEDIUM_2( samModule,
              "Unregistering algo %x, due to dependency %x",
              *algo->sensor.sensorUID, *suid );
          sns_sam_service_unreg( algo );
        }
        algo->sensor.isAvailable = false;
      }
    }
  }
}

/**
 * Handle an error from SMR. This will remove all algorithm instances that depend
 * upon the sensor request, disable the algorithm and release the SMR connection.
 *
 * @param[i] sensorReq     Sensor request associated with the SMR error
 *
 */
STATIC void
sns_sam_handle_smr_error( sns_sam_sensor_req *sensorReq)
{
  sns_sam_err err;
  struct sns_sam_sensor *sensor = sensorReq->sensor;
  sns_sam_remove_algo_instances( sensorReq, SENSOR1_EFAILED );

  // SAM uses special connections during initialization to acquire the list
  // of available sensors (e.g. from SMGR).  These connections are not
  // associated with a single SUID, and should be ignored here.
  if( NULL != sensor->sensorUID )
  {
    sns_sam_disable_sensor_algo(sensor->sensorUID);
  }
  else
  {
    sns_sam_client_release( sensorReq );
  }

  sensor->isAvailable = false;
  sensor->sensorReq.clientHndl = NULL;

  err = sns_sam_client_check( sensor->serviceObj, SMR_CLIENT_INSTANCE_ANY, 0 );
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
      "Unable to register for SMR service check: %x", *sensor->sensorUID );
  }
}

/**
 * Handle an internal error. Send an error indication to all clients of the
 * algorithm instance. Remove all client requests and delete the algorithm instance.
 *
 * @param[i] algoInst     Algorithm instance that generated the error.
 */
STATIC void
sns_sam_handle_internal_error( sns_sam_algo_inst *algoInst)
{
  sns_q_link_s *qCurr;
  sam_client_req *clientReq;
  sns_sam_err err;

  // Do nothing if the algorithm has already been removed
  if( NULL == algoInst ||
      algoInst != algoInst->qLink.self_ptr ||
      &algoInstQ != (sns_q_s *)algoInst->qLink.q_ptr )
  {
    return;
  }

  qCurr = sns_q_check( &algoInst->clientRequests );
  while( NULL != qCurr )
  {
    clientReq = (sam_client_req*)qCurr;
    qCurr = sns_q_next( &algoInst->clientRequests, qCurr );

    err = sns_sam_send_error_ind( SENSOR1_EFAILED, clientReq );
    if( SAM_ENONE != err )
    {
      SNS_PRINTF_STRING_ERROR_2( samModule,
        "Unable to send error ind for %x, error: %i",
        *algoInst->algorithm->sensor.sensorUID, err );
    }

    sns_sam_remove_client_req( clientReq, NULL );
  }

}

/**
 * Handle an error.
 *
 * @param[i] clientErr     Error to be handled.
 */
STATIC void
sns_sam_handle_error( sns_sam_client_error *clientErr )
{
  if( SNS_SAM_ERR_INTERNAL == clientErr->errorSource)
  {
    sns_sam_algo_inst *algoInst = (sns_sam_algo_inst*)clientErr->errCBData;
    sns_sam_handle_internal_error(algoInst);
  }
  else if ( SNS_SAM_ERR_SMR == clientErr->errorSource)
  {
    sns_sam_sensor_req *sensorReq =  (sns_sam_sensor_req*)clientErr->errCBData;
    sns_sam_handle_smr_error(sensorReq);
  }
}

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

void
sns_sam_remove_from_error_queue ( sns_sam_algo_inst * algoInst )
{
  uint8_t errOS;
  sns_q_link_s *qCurr;

  sns_os_mutex_pend( samClientErrCBQMutex, 0, &errOS );
  qCurr = sns_q_check( &samClientErrCBQ );

  while( NULL != qCurr )
  {
    sns_sam_client_error *error = (sns_sam_client_error *)qCurr;
    qCurr = sns_q_next(&samClientErrCBQ, qCurr);
    if( SNS_SAM_ERR_INTERNAL == error->errorSource &&
      algoInst == (sns_sam_algo_inst *)error->errCBData)
    {
      SNS_PRINTF_STRING_HIGH_1( samModule,
          "Removing algorithm %x from error queue",
          *algoInst->algorithm->sensor.sensorUID );
      sns_q_delete((sns_q_link_s *)error);
      SNS_OS_FREE(error);
    }
  }

  sns_os_mutex_post( samClientErrCBQMutex );
}

void
sns_sam_remove_algo_instances( sns_sam_sensor_req const *sensorReq,
  uint8_t errCode )
{
  sns_sam_err err = SAM_ENONE;
  sns_q_link_s *qCurr;
  sam_client_req *clientReq;
  uint32_t i;

  for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
  {
    sns_sam_algo_inst *algoInst = sensorReq->algoInst[ i ];
    if( NULL == algoInst )
    {
      continue;
    }

    SNS_PRINTF_STRING_HIGH_1( samModule,
          "Disabling SAM algorithm %x",
          *algoInst->algorithm->sensor.sensorUID );

    qCurr = sns_q_check( &algoInst->clientRequests );
    while( NULL != qCurr )
    {
      clientReq = (sam_client_req*)qCurr;
      qCurr = sns_q_next( &algoInst->clientRequests, qCurr );

      err = sns_sam_send_error_ind( errCode, clientReq );
      if( SAM_ENONE != err )
      {
        SNS_PRINTF_STRING_ERROR_2( samModule,
          "Unable to send error ind for %x, error: %i",
          *algoInst->algorithm->sensor.sensorUID, err );
      }

      sns_sam_remove_client_req( clientReq, NULL );
    }
  }
}

sns_sam_err
sns_sam_init_error_fw()
{
  uint8_t errOS;
  const uint8_t priority0 = 0;

  samClientErrCBQMutex = sns_os_mutex_create( priority0, &errOS );
  if( 0 != errOS )
  {
    SNS_PRINTF_STRING_FATAL_1( samModule, "Cannot create mutex %i", errOS );
    return SAM_EFAILED;
  }
  sns_q_init( &samClientErrCBQ );
  return SAM_ENONE;
}

void
sns_sam_report_error( sns_sam_client_error *clientErr )
{
  uint8_t errOS;
  if(NULL != clientErr )
  {
    sns_q_link( clientErr, (sns_q_link_s*)clientErr );

    sns_os_mutex_pend( samClientErrCBQMutex, 0, &errOS );
    sns_q_put( &samClientErrCBQ, (sns_q_link_s*)clientErr );
    sns_os_mutex_post( samClientErrCBQMutex );

    sns_os_sigs_post( sns_sam_sig_event, SNS_SAM_CLIENT_ERROR_SIG, OS_FLAG_SET, &errOS );
  }
}

void
sns_sam_process_error_q( void )
{
  struct sns_sam_client_error *clientErr = NULL;
  uint8_t errOS;

  sns_os_mutex_pend( samClientErrCBQMutex, 0, &errOS );
  while( NULL != ( clientErr = sns_q_get( &samClientErrCBQ ) ) )
  {
    sns_sam_handle_error(clientErr);
    SNS_OS_FREE(clientErr);
  }
  sns_os_mutex_post( samClientErrCBQMutex );
}
