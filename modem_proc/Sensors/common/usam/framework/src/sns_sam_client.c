/*============================================================================
  @file sns_sam_client.c

  @brief
  The SAM Framework both acts as a QMI service and a QMI client.  This file
  contains the implementation of all client duties.

  Once a dependent service becomes available, SAM will initialize a connection,
  and maintain that connection throughout its life.

  @note Response messages are processed first, then request messages, and
        finally indications

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include "qmi_idl_lib.h"
#include "sns_smgr_api_v01.h"
#include "sns_smgr_internal_api_v02.h"
#include "sns_reg_api_v02.h"
#include "sns_init.h"
#include "sns_usmr.h"
#include "sns_debug_str.h"
#include "sns_sam.h"
#include "sns_sam_algo_api.h"
#include "sns_sam_init.h"
#include "sns_sam_req.h"
#include "sns_sam_ind.h"
#include "sns_sam_pm.h"
#include "sns_profiling.h"
#include "sns_sam_io.h"
#include "sns_sam_dep.h"
#include "sns_sam_client.h"
#include "sns_sam_service.h"
#include "sns_sam_prov.h"
#include "sns_sam_reg.h"
#include "sns_sam_error.h"
#ifdef FAST_PLAYBACK
#include "fast_playback_phase1.h"
#endif /* FAST_PLAYBACK */

/*============================================================================
  Type Declarations
  ===========================================================================*/

struct sns_sam_client_resp_cb_data
{
  sns_sam_sensor_req *sensorReq;
  sns_sam_resp *respMsg;
};
typedef struct sns_sam_client_resp_cb_data sns_sam_client_resp_cb_data;

struct sns_sam_release_event
{
  /* Data fields necessary to add this object to a SAM list */
  sns_q_link_s qLink;

  sns_sam_sensor_req *sensorReq;
};
typedef struct sns_sam_release_event sns_sam_release_event;

/**
 * Limit the maximum number of messages in samClntMsgQ. If an incoming
 * indication message would cause the queue to exceed this size, "drop"
 * that new message.  (Response-type messages are never dropped).
 */
#ifdef FAST_PLAYBACK
#define SNS_SAM_CLNT_MSG_Q_MAX 1000
#else
#define SNS_SAM_CLNT_MSG_Q_MAX 75
#endif 

/*============================================================================
  Static Data
  ===========================================================================*/

/* Contains all unprocessed incoming indication and response messages */
sns_q_s samClntMsgQ SNS_SAM_UIMAGE_DATA;
OS_EVENT *samClntMsgQMutex SNS_SAM_UIMAGE_DATA;

/* Queue of sns_sam_release_event objects associated with client release callbacks */
STATIC sns_q_s samReleaseQ;
STATIC OS_EVENT *samReleaseQMutex;

/*============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * @brief
 * This callback function is called by the SMR infrastructure when a response
 * is received after a request is sent using smr_client_send_req()
 *
 * @param[i] clientHndl Handle used by the infrastructure to identify different clients.
 * @param[i] msgID Message ID
 * @param[i] respStruct Pointer to the response
 * @param[i] respStructLen Length of the response
 * @param[i] respCBData User-data
 * @param[i] transpErr Error code
 */
STATIC void
sns_sam_client_resp_cb( smr_client_hndl clientHndl, unsigned int msgID,
    void *respStruct, unsigned int respStructLen, void *respCBData,
    smr_err transpErr )
{
  UNREFERENCED_PARAMETER(clientHndl);
  UNREFERENCED_PARAMETER(respStruct);
  uint8_t errOS;
  sns_sam_client_resp_cb_data *cbData = respCBData;

  if( SMR_NO_ERR == transpErr )
  {
    sns_sam_resp *respMsg = cbData->respMsg;

    SNS_PRINTF_STRING_LOW_2( samModule,
      "Received response message %i from %x", msgID,
      NULL != cbData->sensorReq->sensor->sensorUID ? *cbData->sensorReq->sensor->sensorUID : 0 );

    respMsg->msg.msg.bufSize = respStructLen;
    respMsg->msg.msgID = msgID;
    respMsg->msg.sensorUID = cbData->sensorReq->sensor->sensorUID;
    respMsg->sensorReq = cbData->sensorReq;
    respMsg->isResp = true;

    sns_q_link( respMsg, (sns_q_link_s*)respMsg );

    sns_os_mutex_pend( samClntMsgQMutex, 0, &errOS );
    sns_q_put( &samClntMsgQ, (sns_q_link_s*)respMsg );
    sns_os_mutex_post( samClntMsgQMutex );

    sns_os_sigs_post( sns_sam_sig_event, SNS_SAM_CLNT_MSG_SIG, OS_FLAG_SET, &errOS );
  }
  else
  {
    sns_sam_resp *respMsg = cbData->respMsg;

    SNS_PRINTF_STRING_ERROR_3( samModule,
        "SMR Transport error %i for sensor UID %x, msg ID: %i",
        transpErr, *cbData->sensorReq->sensor->sensorUID, msgID );

    if(NULL != respMsg)
    {
      SNS_OS_ANY_FREE((void *)(respMsg->msg.msg.buf));
      SNS_OS_ANY_FREE(respMsg);
    }
  }

  SNS_OS_ANY_FREE( (void*)respCBData );
}

/**
 * See smr_client_ind_cb.
 */
SNS_SAM_UIMAGE_CODE STATIC void
sns_sam_client_ind_cb( smr_client_hndl clientHndl, unsigned int msgID,
  void *indBuf, unsigned int indBufLen, void *indCBData )
{
  UNREFERENCED_PARAMETER(clientHndl);
  uint8_t errOS;
  bool adjustMips = false;
  sns_sam_sensor_req *sensorReq = indCBData;

  if(samClntMsgQ.cnt < SNS_SAM_CLNT_MSG_Q_MAX )
  {
    sns_sam_ind *indMsg = SNS_OS_ANY_MALLOC( SNS_SAM_DBG_MOD, sizeof(sns_sam_ind) );
    SNS_ASSERT(NULL != indMsg);

    indMsg->msg.msg.buf = (intptr_t)SNS_OS_ANY_MALLOC( SNS_SAM_DBG_MOD, indBufLen );
    SNS_ASSERT(NULL != (void*)indMsg->msg.msg.buf);
    SNS_OS_MEMSCPY( (void*)indMsg->msg.msg.buf, indBufLen, indBuf, indBufLen );
    indMsg->msg.msg.bufSize = indBufLen;

    indMsg->msg.msgID = msgID;
    indMsg->msg.sensorUID = sensorReq->sensorUID;
    indMsg->sensorReq = sensorReq;
    indMsg->isResp = false;
    sns_q_link( indMsg, (sns_q_link_s*)indMsg );

    sns_os_mutex_pend( samClntMsgQMutex, 0, &errOS );
    sns_q_put( &samClntMsgQ, (sns_q_link_s*)indMsg );
    adjustMips = (samClntMsgQ.cnt > SNS_SAM_Q_UPPER_THRESH)? true : false;
    sns_os_mutex_post( samClntMsgQMutex );

    if(adjustMips)
    {
      sns_sam_uimage_exit();
      sns_sam_pm_vote_mips( SNS_SAM_MIPS_TURBO );
    }

    sns_os_sigs_post( sns_sam_sig_event,
      SNS_SAM_CLNT_MSG_SIG, OS_FLAG_SET, &errOS );
  }
  else
  {

    SNS_UMSG_LOW_3( samModule, "Rejecting Indication from SUID %x, Msg ID: %d, ClntMsgQ.cnt: %d",
      (uint32_t)(*sensorReq->sensorUID), msgID, (uint32_t)samClntMsgQ.cnt);
    sns_profiling_log_qdss( SNS_SAM_DROPPED_IND_MSG, 2,
        (uint32_t)(*sensorReq->sensorUID), (uint32_t)samClntMsgQ.cnt );
  }
}

/**
 * @brief
 * This callback function is called by the SMR infrastructure when the service
 * terminates or deregisters
 *
 * @param[i] clientHndl Used by the infrastructure to identify unique clients
 * @param[i] error Error code
 * @param[i] errCBData User-data
 */
STATIC void
sns_sam_client_error_cb( smr_client_hndl clientHndl, smr_err error,
    void *errCBData )
{
  UNREFERENCED_PARAMETER(clientHndl);

  sns_sam_client_error *clientErrCB = SNS_OS_MALLOC( SNS_SAM_DBG_MOD,
    sizeof(sns_sam_client_error) );
  SNS_ASSERT(NULL != clientErrCB);

  clientErrCB->error = SAM_ESMR;
  clientErrCB->errCBData = errCBData;
  clientErrCB->errorSource = SNS_SAM_ERR_SMR;

  SNS_PRINTF_STRING_ERROR_1( samModule, "sns_sam_client_error_cb error: %d", error );

  sns_sam_report_error(clientErrCB);
}

/**
 * @brief
 * This callback function is called by the SMR infrastructure when a service
 * becomes available, or if the client-specified timeout passes.
 *
 * @param[i] serviceObj QMI service object
 * @param[i] instanceID Instance ID of the service found
 * @param[i] timeoutExpired Whether the timeout expired
 */
STATIC void
sns_sam_client_init_cb( qmi_idl_service_object_type serviceObj,
    qmi_service_instance instanceID, bool timeoutExpired )
{
  int32_t errQMI = QMI_IDL_LIB_NO_ERR;
  uint32_t serviceID = 0;

  sns_sam_client_init_data *initData = SNS_OS_MALLOC( SNS_SAM_DBG_MOD,
    sizeof(sns_sam_client_init_data) );
  SNS_ASSERT(NULL != initData);

  initData->serviceObj = serviceObj;
  initData->instanceID = instanceID;
  initData->timeoutExpired = timeoutExpired;

  errQMI = qmi_idl_get_service_id( serviceObj, &serviceID );
  if( QMI_IDL_LIB_NO_ERR != errQMI )
  {
    SNS_PRINTF_STRING_MEDIUM_1( samModule, "Service ID check failed %i",
        serviceID );
  }

  SNS_PRINTF_STRING_LOW_1( samModule, "sns_sam_client_init_cb %i", serviceID );

  sns_q_link( initData, (sns_q_link_s*)initData );
  sns_sam_handle_client_init_cb( initData );
}

/**
 * Client release callback handler.  Add release event to queue and signal
 * for asynchronous processing.
 *
 * @param[i] data Sensor Request object to delete
 */
STATIC void
sns_sam_client_release_cb( void *data )
{
  uint8_t errOS;
  sns_sam_release_event *releaseData = SNS_OS_MALLOC( SNS_SAM_DBG_MOD,
    sizeof(sns_sam_release_event) );
  SNS_ASSERT(NULL != releaseData);

  releaseData->sensorReq = (sns_sam_sensor_req*)data;

  SNS_PRINTF_STRING_LOW_0( samModule, "sns_sam_client_release_cb" );

  sns_q_link( releaseData, (sns_q_link_s*)releaseData );

  sns_os_mutex_pend( samReleaseQMutex, 0, &errOS );
  sns_q_put( &samReleaseQ, (sns_q_link_s*)releaseData );
  sns_os_mutex_post( samReleaseQMutex );

  sns_os_sigs_post( sns_sam_sig_event, SNS_SAM_CLIENT_RELEASE_SIG,
      OS_FLAG_SET, &errOS );
}

/**
 * Drop all response and indication messages associated with the sensor req.
 * Free all memory associated with the sensor request object.
 *
 * @param[i] sensorReq Sensor Request object to delete
 */
STATIC void
sns_sam_handle_release(sns_sam_sensor_req *sensorReq)
{
  sns_q_link_s *qCurr;
  uint8_t errOS;

  SNS_PRINTF_STRING_MEDIUM_1( samModule,
    "Released sensor request %x", sensorReq );

  sns_os_mutex_pend( samClntMsgQMutex, 0, &errOS );
  qCurr = sns_q_check( &samClntMsgQ );
  while( NULL != qCurr )
  {
    struct sns_sam_msg_int *message = (struct sns_sam_msg_int*)qCurr;
    qCurr = sns_q_next( &samClntMsgQ, qCurr );

    if( sensorReq == message->sensorReq )
    {
      sns_q_delete( &message->qLink );
      SNS_OS_ANY_FREE( (void*)message->msg.msg.buf );
      SNS_OS_ANY_FREE( (void*)message );
    }
  }
  sns_os_mutex_post( samClntMsgQMutex );

  if( NULL != sensorReq->enableReq )
  {
    SNS_OS_ANY_FREE( (void*)sensorReq->enableReq->msg.buf );
    SNS_OS_ANY_FREE( (void*)sensorReq->enableReq );
  }

  // Don't free if sns_sam_sensor::sensorReq
  if( &sensorReq->sensor->sensorReq != sensorReq )
  {
    SNS_OS_ANY_FREE( sensorReq );
  }
}

/**
 * Handle an incoming response message to the SAM framework.  The request
 * message associated with respMsg will be freed unless it is associated
 * with an active sensor request.
 *
 * @param[i] respMsg Message to be processed
 */

STATIC void
sns_sam_handle_resp( sns_sam_resp *respMsg )
{
  sns_sam_sensor const *sensor = respMsg->sensorReq->sensor;
  SNS_PRINTF_STRING_LOW_2( samModule,
    "sns_sam_handle_resp %i from SUID %x", respMsg->msg.msgID,
    NULL != respMsg->msg.sensorUID ? *respMsg->msg.sensorUID : 0 );

  if( SNS_REG2_SVC_get_service_object_v02() == sensor->serviceObj )
  {
    sns_sam_reg_handle_resp( respMsg );
  }
  else if( NULL != sensor->provAPI->sns_sam_prov_handle_resp )
  {
    provCB.sensorReq = respMsg->sensorReq;
    sensor->provAPI->sns_sam_prov_handle_resp( &provCB, &respMsg->msg );
  }

  SNS_OS_ANY_FREE( (void*)respMsg->msg.msg.buf );
  SNS_OS_ANY_FREE( (void*)respMsg );
}

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_sam_err
sns_sam_client_init_fw( void )
{
  const uint8_t priority0 = 0;
  uint8_t errOS = 0;

  samClntMsgQMutex = sns_os_mutex_create_uimg( priority0, &errOS );
  if( 0 != errOS )
  {
    SNS_PRINTF_STRING_FATAL_1( samModule, "Cannot create mutex %i", errOS );
    return SAM_EFAILED;
  }

  samReleaseQMutex = sns_os_mutex_create( priority0, &errOS );
  if( 0 != errOS )
  {
    SNS_PRINTF_STRING_FATAL_1( samModule, "Cannot create mutex %i", errOS );
    return SAM_EFAILED;
  }

  sns_q_init( &samClntMsgQ );
  sns_q_init( &samReleaseQ );

  return SAM_ENONE;
}

sns_sam_err
sns_sam_client_init( sns_sam_sensor_req *sensorReq, uint32_t timeout,
    bool uImage )
{
  sns_sam_err rv = SAM_ENONE;
  void *indCBData = sensorReq;
  void *errCBData = sensorReq;

  if( NULL == sensorReq->clientHndl )
  {
    smr_err err = smr_client_init( sensorReq->sensor->serviceObj, SMR_CLIENT_INSTANCE_ANY,
        sns_sam_client_ind_cb, indCBData, timeout, sns_sam_client_error_cb,
        errCBData, &sensorReq->clientHndl, uImage );

    if( SMR_NO_ERR != err )
    {
      SNS_PRINTF_STRING_ERROR_1( samModule,
        "smr_client_init error %i", err );
      rv = SAM_ESMR;
    }
  }
  else
  {
    rv = SAM_ENOT_AVAILABLE;
  }

  return rv;
}

sns_sam_err
sns_sam_client_check( qmi_idl_service_object_type serviceObj,
    qmi_service_instance instanceID, uint32_t timeout )
{
  UNREFERENCED_PARAMETER(serviceObj);
  UNREFERENCED_PARAMETER(instanceID);
  UNREFERENCED_PARAMETER(timeout);
  smr_err err = SMR_NO_ERR;
  int32_t errQMI = QMI_IDL_LIB_NO_ERR;
  uint32_t serviceID = 0;

  errQMI = qmi_idl_get_service_id( serviceObj, &serviceID );
  if( QMI_IDL_LIB_NO_ERR != errQMI )
  {
    SNS_PRINTF_STRING_MEDIUM_1( samModule, "Service ID check failed %i",
        serviceID );
  }

  SNS_PRINTF_STRING_LOW_1( samModule, "Checking for service %i", serviceID );

  err = smr_client_check( serviceObj, instanceID, timeout,
      sns_sam_client_init_cb );

  return ( SMR_NO_ERR == err ) ? SAM_ENONE : SAM_ESMR;
}

sns_sam_err
sns_sam_client_send( sns_sam_sensor_req const *sensorReq,
  struct sns_sam_msg const *reqMsg )
{
  sns_sam_err rv = SAM_ENONE;
  uint32_t respMsgSize = 0;
  int32_t errQMI;
  smr_txn_handle txnHandle = NULL;
  smr_err errSMR;
  sns_sam_client_resp_cb_data *respCBData = SNS_OS_MALLOC( SNS_SAM_DBG_MOD,
    sizeof(sns_sam_client_resp_cb_data) );
  SNS_ASSERT(NULL != respCBData);

  respCBData->sensorReq = (sns_sam_sensor_req*)sensorReq;

  SNS_PRINTF_STRING_LOW_2( samModule, "Sending request %i to %x", reqMsg->msgID,
    NULL != sensorReq->sensor->sensorUID ? *sensorReq->sensor->sensorUID : 0 );

  errQMI = qmi_idl_get_message_c_struct_len( sensorReq->sensor->serviceObj,
      QMI_IDL_RESPONSE, reqMsg->msgID, &respMsgSize );
  if( QMI_IDL_LIB_NO_ERR == errQMI )
  {
    sns_sam_resp *respMsg = SNS_OS_MALLOC( SNS_SAM_DBG_MOD, sizeof(sns_sam_resp) );
    SNS_ASSERT(NULL != respMsg);
    respCBData->respMsg = respMsg;

    respMsg->msg.msg.buf = (intptr_t)SNS_OS_MALLOC( SNS_SAM_DBG_MOD, respMsgSize );
    SNS_ASSERT(NULL != (void*)respMsg->msg.msg.buf);
    SNS_OS_MEMZERO( (void*)respMsg->msg.msg.buf, respMsgSize );
    respMsg->msg.msg.bufSize = respMsgSize;

    errSMR = smr_client_send_req( sensorReq->clientHndl, reqMsg->msgID,
      (void*)reqMsg->msg.buf, reqMsg->msg.bufSize, (void*)respMsg->msg.msg.buf,
      respMsgSize, sns_sam_client_resp_cb, respCBData, &txnHandle );

    if( SMR_NO_ERR != errSMR )
    {
      SNS_PRINTF_STRING_ERROR_1( samModule,
        "smr_client_send_req error %i", errSMR );
      rv = SAM_ESMR;
      SNS_OS_ANY_FREE( (void*)respMsg->msg.msg.buf );
      SNS_OS_ANY_FREE( respMsg );
      SNS_OS_ANY_FREE( respCBData );
    }
  }
  else
  {
    SNS_OS_ANY_FREE( respCBData );
    rv = SAM_ETYPE;
  }

  return rv;
}

SNS_SAM_UIMAGE_CODE void
sns_sam_process_clnt_msg_q( void )
{
  sns_q_link_s *qCurr;
  uint8_t errOS;
  struct sns_sam_msg_int *message;

  sns_sam_pm_adjust_mips( );
  do
  {
    sns_os_mutex_pend( samLowMemQMutex, 0, &errOS );
    if (samLowMemQ.cnt > 0)
    {
      sns_os_sigs_post( sns_sam_sig_event, SNS_SAM_CLNT_MSG_SIG, OS_FLAG_SET, &errOS );
      sns_os_mutex_post( samLowMemQMutex );
      return;
    }
    sns_os_mutex_post( samLowMemQMutex );

    sns_os_mutex_pend( samClntMsgQMutex, 0, &errOS );
    qCurr = sns_q_get( &samClntMsgQ );
    message = (struct sns_sam_msg_int*)qCurr;
    sns_os_mutex_post( samClntMsgQMutex );

    if( NULL != qCurr )
    {
      if( message->isResp )
      {
        sns_sam_uimage_exit();
        sns_sam_handle_resp( message );
      }
      else
      {
        sns_sam_handle_ind( message );
      }
    }
  } while( NULL != qCurr );
#ifdef FAST_PLAYBACK
  sns_os_sigs_post( playback_resume_sig_event, (OS_FLAGS) FPB_SENSOR_DATA_RESUME,
      		    OS_FLAG_SET, &errOS );
#endif /* FAST_PLAYBACK */

}

void
sns_sam_process_release_q( void )
{
  uint8_t errOS;
  sns_sam_release_event *releaseEvent;

  do
  {
    sns_os_mutex_pend( samReleaseQMutex, 0, &errOS );
    releaseEvent = (sns_sam_release_event*)sns_q_get( &samReleaseQ );
    sns_os_mutex_post( samReleaseQMutex );

    if( NULL != releaseEvent )
    {
      sns_sam_handle_release( releaseEvent->sensorReq );
      SNS_OS_FREE( releaseEvent );
    }
  } while( NULL != releaseEvent );
}

void
sns_sam_client_release( sns_sam_sensor_req *sensorReq )
{
  SNS_PRINTF_STRING_MEDIUM_1( samModule,
    "Releasing sensor request %x", sensorReq );

  smr_client_release( sensorReq->clientHndl,
    sns_sam_client_release_cb, sensorReq );
}

uint32_t
sns_sam_max_message_size( sns_sam_sensor_uid const *sensorUID )
{
  uint32_t maxLen = 1500;
  sns_sam_sensor *sensor;

  sensor = sns_sam_lookup_sensor_from_suid( sensorUID );
  if( NULL != sensor )
  {
    qmi_idl_get_max_service_len( sensor->serviceObj, &maxLen );
  }

  return maxLen;
}
