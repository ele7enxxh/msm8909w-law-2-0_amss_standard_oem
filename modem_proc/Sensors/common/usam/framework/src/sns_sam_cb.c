/*============================================================================
  @file sns_sam_cb.c

  @brief
  Implements the Algorithm callback functions.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <string.h>
#include "sns_log_api.h"
#include "sns_sam_ped_v01.h"
#include "sns_init.h"
#include "sns_usmr.h"
#include "sns_em.h"
#include "sns_sam.h"
#include "sns_sam_algo_api.h"
#include "sns_sam_req.h"
#include "sns_sam_reg.h"
#include "sns_sam_ind.h"
#include "sns_sam_cb.h"
#include "sns_sam_client.h"
#include "sns_sam_pm.h"
#include "sns_sam_dep.h"
#include "sns_sam_io.h"

#if defined(SNS_DSPS_BUILD)|defined(FAST_PLAYBACK)

typedef struct
    {
      char const *filename;
      uint16_t lineNum;
      uint8_t numParamsValid;
      intptr_t param1;
      intptr_t param2;
      intptr_t param3;
    } debug_params_s;
	
#endif

#ifdef FAST_PLAYBACK
    /**
     * stub out
     */
    STATIC void sns_debug_printf_string( sns_debug_module_id_e moduleID,
      uint8_t priority, const char *debugStr, const debug_params_s *formatParams )
    {

    //  printf("SAM algo_log callback: %s, parameter count: %d, parameters: %d %d %d\n", debugStr, formatParams->numParamsValid, formatParams->param1, formatParams->param2, formatParams->param3);
      return;
    }
#endif

#if !defined(SNS_DSPS_BUILD)
#include "sns_debug_str.h"
#define SAM_ROLLOVER_THRESH UINT64_MAX

SNS_SAM_UIMAGE_CODE STATIC bool sns_sam_island_status() { return false; }
#else
#include "qurt_island.h"
#include "sns_profiling.h"
#include "msg.h"
#define BUFSZ 1024

/* SAM_ROLLOVER_THRESH defines the minimum difference between timestamps to be present for
    a timestamp rollover to have occoured between the two timestamps. */
#define SAM_ROLLOVER_THRESH (UINT32_MAX - (DSPS_SLEEP_CLK * 60 * 5))


SNS_SAM_UIMAGE_CODE STATIC bool sns_sam_island_status( void )
{
  return qurt_island_get_status();
}

/**
 * Prints out the debug string.
 *
 * @param[i] moduleID Module identifier
 * @param[i] priority Priority of the message string
 * @param[i] debugStr Debug string
 * @param[i] formatParams Format parameter structure
 */
STATIC void sns_debug_printf_string( sns_debug_module_id_e moduleID,
  uint8_t priority, const char *debugStr, const debug_params_s *formatParams )
{
  UNREFERENCED_PARAMETER(moduleID);
  UNREFERENCED_PARAMETER(priority);

  MSG_SPRINTF_FMT_VAR_3( MSG_SSID_SNS, DBG_MED_PRIO, debugStr,
      formatParams->param1, formatParams->param2, formatParams->param3 );
}
#endif

/*============================================================================
  Type Declarations
  ===========================================================================*/

typedef void (*sns_sam_timer_cb)(void *);

/**
 * If an timer event occurs due to an algorithm timer, the event is stored in
 * this object, and processed asychronously later, within the SAM main thread.
 */
struct sns_sam_algo_timer_event
{
  /* Data fields necessary to add this object to a SAM list */
  sns_q_link_s qLink;
  /* The algorithm instance whose timer has fired */
  struct sns_sam_algo_inst *algoInst;
};
typedef struct sns_sam_algo_timer_event sns_sam_algo_timer_event;

/*============================================================================
  Global Data
  ===========================================================================*/

/* Callback functions available for all algorithm instances.  This object
 * is shared by all, and the value of algoInst will be reset before each
 * algorithm API function call passing it as an argument. */
sns_sam_algo_callback cbFuncs SNS_SAM_UIMAGE_DATA;

/*============================================================================
  Static Data
  ===========================================================================*/

/* Contains all unprocessed algorithm timer callback events.  Each item
 * has item has type sns_sam_algo_timer_event. */
static sns_q_s algoTimerQ;
static OS_EVENT *algoTimerQMutex = NULL;

/*============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Send a write request to the Sensors Registry.  Processed asynchronously.
 *
 * @param[i] algoHndl Opaque handle of this algorithm instance for SAM.
 * @param[io] regData Registry data to write
 *
 * @return
 *  SAM_ENONE
 *  SAM_ETYPE Invalid Registry ID or wrong data length
 *  SAM_EFAILED Error occurred sending the request; no response expected
 */
SNS_SAM_UIMAGE_CODE STATIC sns_sam_err
sns_sam_algo_reg_req( struct sns_sam_algo_handle const *algoHndl,
    sns_sam_reg_data *regData )
{
  UNREFERENCED_PARAMETER(algoHndl);
  sns_sam_err rv = SAM_EFAILED;

  sns_sam_uimage_exit();
  // Do not allow registry read requests
  if( 0 != regData->groupDataLen )
  {
    rv = sns_sam_reg_req( regData );
  }
  return rv;
}

/**
 * Log a message string generated by the algorithm.  The SAM Framework may
 * do nothing with the message, or log it to one of several buffers.
 *
 * @param[i] algoHndl Opaque handle of this algorithm instance for SAM.
 * @param[i] sensor This algorithm instance
 * @param[i] debug_str Debug string
 * @param[i] fmt_params_ptr Parameters to the debug string
 *
 * @return
 *  SAM_ENONE
 *  SAM_EFAILED Error occurred during log processing
 *  SAM_ETYPE The string was improperly formatted
 *  SAM_ENOT_AVAILABLE Logging is not available at this time
 */
SNS_SAM_UIMAGE_CODE STATIC sns_sam_err
sns_sam_algo_log( struct sns_sam_algo_handle const *algoHndl,
    char const *debugStr, sns_sam_algo_debug_params const *formatParams )
{
  UNREFERENCED_PARAMETER(algoHndl);

  if(sns_sam_island_status())
  {
    return SAM_ENOT_AVAILABLE;
  }

  sns_debug_printf_string( samModule, SNS_MSG_MEDIUM, debugStr,
    (debug_params_s*)formatParams );

  return SAM_ENONE;
}

/**
 * Indicates to the SAM Framework that the list or qualities of the
 * algorithm's dependent sensors has changes.  The framework should
 * then call sns_sam_algo_gen_req() for the sensor.
 *
 * @param[i] algoHndl Opaque handle of this algorithm instance for SAM.
 * @param[i] sensorUID Dependent sensor that has been changed/added/removed
 *
 * @return
 *  SAM_ENONE
 *  SAM_ETYPE Invalid or unknown sensor ID
 *  SAM_EFAILED An error occured; the sensor change was not recorded
 */
SNS_SAM_UIMAGE_CODE STATIC sns_sam_err
sns_sam_algo_sensor_change( struct sns_sam_algo_handle const *algoHndl,
    sns_sam_sensor_uid const *sensorUID )
{
  sns_sam_err err;
  sns_sam_algo_inst *algoInst = ((sam_client_req*)algoHndl)->algoInstance;

  sns_sam_uimage_exit();
  err = sns_sam_handle_sensor_change( algoInst, sensorUID );
  return err;
}

/**
 * Allocates memory for a single input data, adds that item to the algorithm
 * instance input queue, and copies the supplies contents onto the buffer.
 *
 * @note1hang inputData must have the length as supplied in
 *            sns_sam_algo_api::sns_sam_algo_mem_req().
 *
 * @param[in] algoHndl Opaque handle of this algorithm instance for SAM.
 * @param[in] sensorUID Sensor ID of the new input object
 * @param[in] timestamp Timestamp associated with this data
 * @param[in] inputData Data to be added to the input queue.
 *
 * @return
 * SAM_ENONE -- Operation was successful. \n
 * SAM_EFAILED -- No more memory is presently available on the system. \n
 * SAM_EMAX -- Algorithm is not allowed any more input allocations.
 */
SNS_SAM_UIMAGE_CODE STATIC sns_sam_err
sns_sam_algo_add_input( struct sns_sam_algo_handle const *algoHndl,
    sns_sam_sensor_uid const *sensorUID, sns_sam_algo_input const *input )
{
  sns_sam_algo_inst *algoInst = ((sam_client_req*)algoHndl)->algoInstance;

  sns_profiling_log_qdss( SNS_SAM_ALGO_CB_ADD_INPUT_ENTER, 2,
      (uint32_t)*algoInst->algorithm->sensor.sensorUID, (uint32_t)algoInst );

  sns_sam_input_push( algoInst, sensorUID, input );

  sns_profiling_log_qdss( SNS_SAM_ALGO_CB_ADD_INPUT_EXIT, 2,
      (uint32_t)*algoInst->algorithm->sensor.sensorUID, (uint32_t)algoInst );

  return SAM_ENONE;
}

/**
 * Allocates memory for a single output data, adds that item to the algorithm
 * instance output queue, and copies the supplies contents onto the buffer.
 *
 * @note outputData Must have the length as supplied in sns_sam_algo_mem_req.
 *
 * @param[i] algoHndl Opaque handle of this algorithm instance for SAM.
 * @param[i] sensor This algorithm instance
 * @param[i] outputData Data to be added to the output queue
 *
 * @return
 *  SAM_ENONE
 *  SAM_EFAILED No more memory is presently available on the system
 *  SAM_EMAX The algorithm is not allowed any more output allocations
 */
SNS_SAM_UIMAGE_CODE STATIC sns_sam_err
sns_sam_algo_add_output( struct sns_sam_algo_handle const *algoHndl,
  sns_sam_algo_output const *outputData )
{
  sns_sam_err err;
  sns_sam_algo_inst *algoInst = ((sam_client_req*)algoHndl)->algoInstance;

  sns_profiling_log_qdss( SNS_SAM_ALGO_CB_ADD_OUTPUT_ENTER, 2,
      (uint32_t)*algoInst->algorithm->sensor.sensorUID, (uint32_t)algoInst );

  err = sns_sam_process_output( algoInst, outputData );
  if( SAM_ENONE != err )
  {
    SNS_UMSG_ERROR_2( samModule,
      "Error processing output for %x, error %i",
      *algoInst->algorithm->sensor.sensorUID, err );
  }

  sns_profiling_log_qdss( SNS_SAM_ALGO_CB_ADD_OUTPUT_EXIT, 2,
      (uint32_t)*algoInst->algorithm->sensor.sensorUID, (uint32_t)algoInst );

  return SAM_ENONE;
}

/**
 * Submits a log packet to DIAG.  Packet will not be saved by framework, and
 * hence is safe on the stack.
 *
 * @param[i] algoHndl Opaque handle of this algorithm instance for SAM.
 * @param[i] logPktID Log Packet ID
 * @param[i] logPktSize sizeof the log packet
 * @param[i] logPkt Packet packet to submit
 *
 * @return
 *  SAM_ENONE
 *  SAM_EFAILED Unable to sucessfully submit the log packet
 *  SAM_ENOT_AVAILABLE Logging is not available at this time
 */
SNS_SAM_UIMAGE_CODE STATIC sns_sam_err
sns_sam_algo_log_submit( struct sns_sam_algo_handle const *algoHndl,
  uint32_t logPktID, uint32_t logPktSize, PACK(void*) logPkt )
{
  bool result;
  sns_sam_algo_inst *algoInst = ((sam_client_req*)algoHndl)->algoInstance;

  sns_profiling_log_qdss( SNS_SAM_ALGO_CB_LOG_SUBMIT_ENTER, 3,
      (uint32_t)*algoInst->algorithm->sensor.sensorUID,
      (uint32_t)algoInst, logPktID );

  result = ( SNS_SUCCESS == sns_logpkt_submit( logPktID, logPktSize, logPkt ) );

  sns_profiling_log_qdss( SNS_SAM_ALGO_CB_LOG_SUBMIT_EXIT, 3,
      (uint32_t)*algoInst->algorithm->sensor.sensorUID, (uint32_t)algoInst, logPktID );
  return result ? SAM_ENONE : SAM_EFAILED;
}

/**
 * Indicate to the SAM Framework that the algorithm would prefer that the
 * associated sensor stream be batched.
 *
 * @param[i] algoHndl Opaque handle of this algorithm instance for SAM.
 * @param[i] sensorUID Dependent sensor that was already enabled
 * @param[i] batchPeriod Max time (in seconds Q16) between
 *       batched indications
 * @param[i] flushFull True: Wake client to flush full buffer;
 *                     False: Replace oldest if buffer full & client in suspend
 *
 * @return
 *  SAM_ENONE
 *  SAM_ETYPE No active stream is found with this sensor; not a SAM sensor
 */
SNS_SAM_UIMAGE_CODE STATIC sns_sam_err
sns_sam_algo_req_batch( struct sns_sam_algo_handle const *algoHndl,
  sns_sam_sensor_uid const *sensorUID, uint32_t batchPeriod, bool flushFull )
{
  UNREFERENCED_PARAMETER(flushFull);
  sns_sam_err err;
  sns_sam_algo_inst *algoInst = ((sam_client_req*)algoHndl)->algoInstance;

  if ( NULL == sensorUID ||
       NULL == sns_sam_lookup_sensor_from_suid( sensorUID ) )
  {
    return SAM_ETYPE;
  }

  SNS_PRINTF_STRING_LOW_2( samModule,
        "Algorithm %x requested batched %x",
        *algoInst->algorithm->sensor.sensorUID, *sensorUID );

  sns_sam_uimage_exit();
  err = sns_sam_set_batch_period( (sns_sam_algo_inst*)algoInst, sensorUID,
    batchPeriod );

  return SAM_ETYPE == err ? SAM_ETYPE : SAM_ENONE;
}

/**
 * Callback function to be registered with the EM service.  Will create a
 * timer event object on the queue, and signal the SAM main thread for
 * processing.
 *
 * @note This algorithm instance may not be valid, if the algorithm instance
 *       is removed within this function.
 *
 * @param[i] algoHndl Opaque handle of this algorithm instance for SAM.
 */
STATIC void
sns_sam_timer_cb_algo( struct sns_sam_algo_handle const *algoHndl )
{
  uint8_t errOS;
  sns_sam_algo_inst *algoInst = ((sam_client_req*)algoHndl)->algoInstance;
  sns_sam_algo_timer_event *timerEvent = SNS_OS_MALLOC( SNS_SAM_DBG_MOD,
    sizeof(sns_sam_algo_timer_event) );
  SNS_ASSERT(NULL != timerEvent);

  timerEvent->algoInst = algoInst;

  SNS_PRINTF_STRING_LOW_0( samModule, "sns_sam_timer_cb_algo" );

  sns_q_link( timerEvent, (sns_q_link_s*)timerEvent );

  sns_os_mutex_pend( algoTimerQMutex, 0, &errOS );
  sns_q_put( &algoTimerQ, (sns_q_link_s*)timerEvent );
  sns_os_mutex_post( algoTimerQMutex );

  sns_os_sigs_post( sns_sam_sig_event,
    SNS_SAM_ALGO_TIMER_SIG, OS_FLAG_SET, &errOS );
}

/**
 * Register a timer callback.  Only one timer is active per algorithm
 * instance; subsequent requsts update or disable the existing timer.
 *
 * @note If a timer is disabled by the algorithm, it is guaranteed to not
 *       receive the timeout event.
 *
 * @param[i] algoHndl Opaque handle of this algorithm instance for SAM.
 * @param[i] timeout Nanoseconds until the algorithm will be alerted
 *                   0 to disable the current active timer.
 * @param[i] timerCB Callback function to register for this timer event.
 *
 * @return
 *  SAM_ENONE
 *  SAM_EFAILED Unable to successfully register the timer
 */
SNS_SAM_UIMAGE_CODE STATIC sns_sam_err
sns_sam_algo_timer_reg( struct sns_sam_algo_handle const *algoHndl,
  uint64_t timeout, sns_sam_algo_timer_cb timerCB )
{
  sns_sam_err rv = SAM_ENONE;
  sns_sam_algo_inst *algoInst = ((sam_client_req*)algoHndl)->algoInstance;
  sns_sam_uimage_exit();

  // Converting from nanoseconds to microseconds
  timeout = timeout / 1000;

  if( 0 == timeout )
  {
    if( NULL != algoInst->algoTimer )
    {
      uint8_t errOS;
      sns_em_delete_timer_obj( algoInst->algoTimer );
      sns_os_mutex_pend( algoTimerQMutex, 0, &errOS );
      algoInst->algoTimer = NULL;
      algoInst->algoTimerCB = NULL;
      sns_os_mutex_post( algoTimerQMutex );
      sns_sam_clear_algo_timer( algoInst );
    }
  }
  else
  {
    uint64_t timeoutTick = sns_em_convert_usec_to_localtick64( timeout );
    sns_err_code_e err;

    if( NULL == algoInst->algoTimer )
    {
      err = sns_em_create_timer_obj( (sns_sam_timer_cb)&sns_sam_timer_cb_algo,
        (void *)algoHndl, SNS_EM_TIMER_TYPE_ONESHOT, &algoInst->algoTimer );
      if( SNS_SUCCESS != err )
      {
        SNS_PRINTF_STRING_ERROR_1( samModule,
          "Error creating Algo timer %i", err );
        rv = SAM_EFAILED;
      }
    }

    if( SAM_ENONE == rv )
    {
      algoInst->algoTimerCB = timerCB;
      err = sns_em_register_timer64( algoInst->algoTimer, timeoutTick );

      if( SNS_SUCCESS != err )
      {
        SNS_PRINTF_STRING_ERROR_1( samModule,
            "Error registering Algo timer %i", err );
        rv = SAM_EFAILED;
      }
    }
  }

  return rv;
}

/**
 * Log QDSS events for algorithm update per input
 *
 * @param[i] algoHndl Opaque handle of this algorithm instance for SAM.
 * @param[i] qdssEventId QDSS SW event
 * @param[i] inputType Specify the input type for algo depending on inputs from multiple sensors
 *                      For algo depending on only one kind of sensor input, use 0.
 *
 * @return
 *  SAM_ENONE Operation was successful.
 */
SNS_SAM_UIMAGE_CODE STATIC sns_sam_err
sns_sam_algo_log_qdss( struct sns_sam_algo_handle const *algoHndl,
  sns_tracer_event qdssEventId, uint32_t inputType )
{
  sns_sam_algo_inst *algoInst = ((sam_client_req*)algoHndl)->algoInstance;

  sns_profiling_log_qdss( qdssEventId, 3,
    (uint32_t)*algoInst->algorithm->sensor.sensorUID, (uint32_t)algoInst, inputType );
  return SAM_ENONE;
}

/**
 * Obtain the number of input samples remaining for the given sensor type.
 *
 * @note Within a single call to sns_sam_algo_update(), this value will only
 * decrease as a result of calling sns_sam_algo_get_input().
 *
 * @note For an algorithm operating in periodic mode, the # of available
 * input samples may be lower than the value returned from this function.
 *
 * @param[i] algoHndl Opaque handle of this algorithm instance for SAM.
 * @parma[i] sensorUID Sensor type to query; NULL for all input
 *
 * @return Number of input samples remaining
 */
SNS_SAM_UIMAGE_CODE STATIC uint32_t
sns_sam_algo_input_cnt( struct sns_sam_algo_handle const *algoHndl,
    sns_sam_sensor_uid const *sensorUID )
{
  sns_sam_algo_inst *algoInst = ((sam_client_req*)algoHndl)->algoInstance;
  return sns_sam_input_cnt( algoInst, sensorUID );
}

/**
 * Find and remove the oldest input object from the input queue.
 *
 * @param[i] algoHndl Opaque handle of this algorithm instance for SAM.
 * @parma[i] sensorUID Restrict samples from this input type only (or NULL)
 *
 * @return Acquired input data; NULL if none available
 */
SNS_SAM_UIMAGE_CODE STATIC sns_sam_algo_input
sns_sam_algo_get_input( struct sns_sam_algo_handle const *algoHndl,
    sns_sam_sensor_uid const *sensorUID )
{
  sam_client_req *clientReq = (sam_client_req*)algoHndl;
  sns_sam_timestamp maxTS = SNS_SAM_TIMESTAMP_MAX;

  if( SNS_SAM_REPORT_MODE_PERIODIC == clientReq->clientAttr.reportMode )
  {
    maxTS = clientReq->nextReportTS;
  }

  return sns_sam_input_pop( clientReq->algoInstance, sensorUID, maxTS );
}

/**
 * Find and remove the oldest output object from the output queue.
 *
 * @param[i] algoHndl Opaque handle of this algorithm instance for SAM.
 *
 * @return Acquired output data; NULL if none available
 */
SNS_SAM_UIMAGE_CODE STATIC sns_sam_algo_output
sns_sam_algo_get_output( struct sns_sam_algo_handle const *algoHndl )
{
  sam_client_req *clientReq = (sam_client_req*)algoHndl;

  return sns_sam_output_pop( clientReq );
}

/**
 * Send an UPDATE_REQ to a dependent algorithm.
 *
 * @param[i] algoHndl Opaque handle of this algorithm instance for SAM.
 * @param[i] sensorUID The intended recipient of the update request message
 * @param[i] updateReq The update request message to be sent
 *
 * @return
 *  SAM_ENONE Operation was successful.
 *  SAM_ETYPE No active stream is found with this sensor.
 *  SAM_EFAILED Unable to send request.
 */
SNS_SAM_UIMAGE_CODE STATIC sns_sam_err
sns_sam_algo_send_update( struct sns_sam_algo_handle const *algoHndl,
    sns_sam_sensor_uid const *sensorUID, sns_sam_update_req const *updateReq )
{
  sns_sam_err err;
  sns_sam_sensor_req *sensorReq = NULL;
  sns_sam_algo_inst *algoInst = ((sam_client_req*)algoHndl)->algoInstance;
  struct sns_sam_msg reqMsg;
  sns_sam_ped_reset_req_msg_v01 *updateMsg
    = (sns_sam_ped_reset_req_msg_v01*)updateReq->msg.buf;

  sns_sam_uimage_exit();

  sensorReq = sns_sam_find_sensor_req( algoInst, sensorUID );
  if( NULL == sensorReq )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
        "Sensor request not found for %x", *sensorUID );
    return SAM_ETYPE;
  }

  SNS_PRINTF_STRING_LOW_2( samModule,
        "Algorithm %x sending update request to %x",
        *algoInst->algorithm->sensor.sensorUID, *sensorUID );

  updateMsg->instance_id = sensorReq->instanceID;

  reqMsg.msg = updateReq->msg;
  reqMsg.msgID = SNS_SAM_ALGO_UPDATE_REQ;
  reqMsg.sensorUID = sensorReq->sensor->sensorUID;

  err = sns_sam_client_send( sensorReq, &reqMsg );
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
      "Error sending update request %i", err );
    return SAM_EFAILED;
  }

  return SAM_ENONE;
}

/*============================================================================
  Function Definitions
  ===========================================================================*/

void
sns_sam_clear_algo_timer( sns_sam_algo_inst const *algoInst )
{
  uint8_t errOS;
  sns_q_link_s *qCurr;

  sns_os_mutex_pend( algoTimerQMutex, 0, &errOS );
  for( qCurr = sns_q_check( &algoTimerQ ); NULL != qCurr;
       qCurr = sns_q_next( &algoTimerQ, qCurr ) )
  {
    sns_sam_algo_timer_event *timerEvent =
      (sns_sam_algo_timer_event*)qCurr;
    // Confirm that the algorithm instance has not been removed
    if( algoInst == timerEvent->algoInst )
    {
       sns_q_delete( &timerEvent->qLink );
       SNS_OS_ANY_FREE( timerEvent );
       break;
    }
  }
  sns_os_mutex_post( algoTimerQMutex );
}

void
sns_sam_handle_algo_timer()
{
  struct sns_sam_algo_timer_event *timerEvent = NULL;
  uint8_t errOS;

  sns_os_mutex_pend( algoTimerQMutex, 0, &errOS );
  while( NULL != ( timerEvent = sns_q_get( &algoTimerQ ) ) )
  {
    sns_sam_algo_inst *algoInst = timerEvent->algoInst;
    sns_sam_algo_timer_cb timerCB = NULL;
    sns_q_link_s *qCurr;

    for( qCurr = sns_q_check( &algoInstQ ); NULL != qCurr;
         qCurr = sns_q_next( &algoInstQ, qCurr ) )
    {
      sns_sam_algo_inst const *tempAI = (sns_sam_algo_inst*)qCurr;
      // Confirm that the algorithm instance has not been removed
      if( tempAI == algoInst )
      {
        timerCB = algoInst->algoTimerCB;
        break;
      }
    }

    if( NULL != timerCB )
    {
      struct sns_sam_algo_handle const *algoHndl =
        (struct sns_sam_algo_handle*) sns_q_check( &algoInst->clientRequests );

      sns_os_mutex_post( algoTimerQMutex );
      cbFuncs.algoHndl = algoHndl;
      timerCB( &cbFuncs, &algoInst->algoStateData );
      sns_os_mutex_pend( algoTimerQMutex, 0, &errOS );
    }
    SNS_OS_ANY_FREE( timerEvent );
  }
  sns_os_mutex_post( algoTimerQMutex );
}

sns_sam_err
sns_sam_cb_init_fw( void )
{
  uint8_t errOS = 0;
  cbFuncs.structSize = sizeof(sns_sam_algo_callback);
  cbFuncs.algoHndl = NULL;

  cbFuncs.sns_sam_algo_reg_req = &sns_sam_algo_reg_req;
  cbFuncs.sns_sam_algo_log = &sns_sam_algo_log;
  cbFuncs.sns_sam_algo_sensor_change = &sns_sam_algo_sensor_change;
  cbFuncs.sns_sam_algo_add_input = &sns_sam_algo_add_input;
  cbFuncs.sns_sam_algo_add_output = &sns_sam_algo_add_output;
  cbFuncs.sns_sam_algo_log_submit = &sns_sam_algo_log_submit;
  cbFuncs.sns_sam_algo_req_batch = &sns_sam_algo_req_batch;
  cbFuncs.sns_sam_algo_timer_reg = &sns_sam_algo_timer_reg;
  cbFuncs.sns_sam_algo_log_qdss = &sns_sam_algo_log_qdss;
  cbFuncs.sns_sam_algo_input_cnt = &sns_sam_algo_input_cnt;
  cbFuncs.sns_sam_algo_get_input = &sns_sam_algo_get_input;
  cbFuncs.sns_sam_algo_get_output = &sns_sam_algo_get_output;
  cbFuncs.sns_sam_algo_send_update = &sns_sam_algo_send_update;

  algoTimerQMutex = sns_os_mutex_create( 0, &errOS );
  if( 0 != errOS )
  {
    SNS_PRINTF_STRING_FATAL_1( samModule, "Cannot create mutex %i", errOS );
    return SAM_EFAILED;
  }
  sns_q_init( &algoTimerQ );

  return SAM_ENONE;
}
