/*============================================================================
  @file sns_sam.c

  @brief
  Implementation of the Sensors Algorithm Manager Framework.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include "qurt_timer.h"
#include "sns_init.h"
#include "sns_common.h"
#include "sns_debug_str.h"
#include "sns_sam_common_v01.h"
#include "sns_em.h"
#include "sns_osa.h"
#include "sns_sam.h"
#include "sns_sam_algo_api.h"
#include "sns_sam_req.h"
#include "sns_sam_reg.h"
#include "sns_sam_init.h"
#include "sns_sam_client.h"
#include "sns_sam_service.h"
#include "sns_sam_ind.h"
#include "sns_sam_cb.h"
#include "sns_sam_pm.h"
#include "sns_sam_dl.h"
#include "sns_sam_io.h"
#include "sns_profiling.h"
#include "sns_sam_algo_gpio.h"
#include "sns_sam_error.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/

/* Thresholds used for determining when a clock rollover likely occurred. */
#define SNS_SAM_TS_ROLLOVER_HIGH_THRESH SNS_SAM_TIMESTAMP_MAX * .9
#define SNS_SAM_TS_ROLLOVER_LOW_THRESH SNS_SAM_TIMESTAMP_MAX * .1

/*============================================================================
  Type Definitions
  ===========================================================================*/

/** A signal event handled by the SAM Framework */
struct sns_sam_signal
{
  void (*processFunc)( void );  // Function pointer to process signal
  uint32_t signal; // Signal mask
  bool bigImage;// Whether SAM must exit uImage to call processFunc
};
typedef struct sns_sam_signal sns_sam_signal;

/*============================================================================
  Forward Declarations
  ===========================================================================*/

STATIC void sns_sam_handle_lowmem( void );

/*============================================================================
  Static Data
  ===========================================================================*/

/* SAM task stack */
STATIC OS_STK sns_sam_task_stk[SNS_SAM_MODULE_STK_SIZE] SNS_SAM_UIMAGE_DATA;

/** All of the signals handled by the SAM Framework */
STATIC sns_sam_signal samSignals[] SNS_SAM_UIMAGE_DATA =
{
  {sns_sam_handle_lowmem, SNS_SAM_LOWMEM_CB_SIG, false},
  {sns_sam_handle_pm_event, SNS_SAM_PM_SIG, true},
  {sns_sam_handle_algo_timer, SNS_SAM_ALGO_TIMER_SIG, true},
  {sns_sam_process_svc_msg_q, SNS_SAM_REQ_MSG_SIG, true},
  {sns_sam_process_clnt_msg_q, SNS_SAM_CLNT_MSG_SIG, false},
  {sns_sam_handle_client_timer, SNS_SAM_CLIENT_TIMER_SIG, false},
  {sns_sam_handle_reg_timeout, SNS_SAM_REG_INIT_TIMER_SIG, true},
  {sns_sam_init_clients, SNS_SAM_CLIENT_INIT_SIG, true},
  {sns_sam_process_error_q, SNS_SAM_CLIENT_ERROR_SIG, true},
  {sns_sam_process_resume_q, SNS_SAM_SVC_RESUME_SIG, true},
  {sns_sam_process_disc_q, SNS_SAM_SVC_DISC_SIG, true},
  {sns_sam_process_release_q, SNS_SAM_CLIENT_RELEASE_SIG, true},
};
STATIC const uint32_t signalsLength SNS_SAM_UIMAGE_RODATA =
  sizeof(samSignals) / sizeof(samSignals[0]);

/*============================================================================
  Global Data -  Comments in sns_sam.h
  ===========================================================================*/

OS_FLAG_GRP *sns_sam_sig_event SNS_SAM_UIMAGE_DATA;
sns_proc_type_e_v01 localProcessor SNS_SAM_UIMAGE_DATA = SNS_SAM_PROCESSOR_TYPE;
sns_debug_module_id_e samModule SNS_SAM_UIMAGE_DATA = SNS_SAM_DBG_MOD;
sns_q_s algoInstQ SNS_SAM_UIMAGE_DATA;
sns_q_s sensorQ SNS_SAM_UIMAGE_DATA;

/*============================================================================
  Static Function Definitions
  ===========================================================================*/
/**
 * Perform the specified low memory action.
 *
 * @param[i] lowmemData Lowe memory action
 */
SNS_SAM_UIMAGE_CODE STATIC void
sns_sam_perform_lowmem( sns_sam_io_lowmem_data const *lowmemData )
{

  bool exitUimage =
    SNS_SAM_IO_FLUSH_AP == lowmemData->action ||
    SNS_SAM_IO_DROP_DATA == lowmemData->action ||
    SNS_SAM_IO_FLUSH_DDR == lowmemData->action;

  if( exitUimage )
  {
    sns_sam_uimage_exit();
  }

  if( SNS_SAM_IO_FLUSH_AP == lowmemData->action )
  {
    sns_sam_flush_batched_ap_output();
    sns_sam_drop_unneeded_output();
  }
  else if( SNS_SAM_IO_DROP_DATA == lowmemData->action )
  {
    sns_sam_drop_unneeded_output();
  }
  else
  {
    sns_q_link_s *qCurrAI, *qCurrCR, *qCurrIn;

    for( qCurrAI = sns_q_last_check_probe( &algoInstQ ); NULL != qCurrAI;
         qCurrAI = sns_q_prev( &algoInstQ, qCurrAI ) )
    {
      sns_sam_algo_inst *algoInst = (sns_sam_algo_inst*)qCurrAI;

      if( SNS_SAM_IO_SEND_BATCH == lowmemData->action &&
          !SNS_SAM_ALGOINST_IN_UIMAGE(algoInst) )
      {
        // If we get the TCM low memory cb, we can ignore bigImage algos
        break;
      }

      for( qCurrIn = sns_q_check(&algoInst->algoInputQ); NULL != qCurrIn;
           qCurrIn = sns_q_next(&algoInst->algoInputQ, qCurrIn ))
      {
        sns_sam_algo_input_q * input_q = (sns_sam_algo_input_q *) qCurrIn;
        SNS_PRINTF_STRING_LOW_3(samModule, "Algorithm %x has Sensor %x with Input Bin Count: %d",
          (uint32_t)*algoInst->algorithm->sensor.sensorUID, (uint32_t) *input_q->sensorUID,
          input_q->inputDataQ.cnt);
      }
      for( qCurrCR = sns_q_check( &algoInst->clientRequests ); NULL != qCurrCR;
           qCurrCR = sns_q_next( &algoInst->clientRequests, qCurrCR ) )
      {
        sam_client_req *clientReq = (sam_client_req*)qCurrCR;
        SNS_PRINTF_STRING_LOW_3(samModule, "Algorithm %x has Client %x with Output Bin Count: %d",
          (uint32_t)*algoInst->algorithm->sensor.sensorUID, clientReq->extInstanceID,
          clientReq->outputDataQ.cnt);

        if( SNS_SAM_IO_FLUSH_DDR == lowmemData->action )
        {
          sns_sam_move_output( clientReq );
        }
        else if( SNS_SAM_IO_SEND_BATCH == lowmemData->action &&
                  SNS_SAM_BATCH_ISLAND_V01 == clientReq->batchPeriod )
        {
          sns_sam_send_all_batch_ind( clientReq, SAM_BATCH_ONLY_IND_V01 );
        }
      }
    }
  }
}

/**
 * Handle the low memory callback from the batchbuff module.
 *
 * See sns_batchbuff_threshold_cb for further information.
 */
STATIC SNS_SAM_UIMAGE_CODE void
sns_sam_handle_lowmem( void )
{
  sns_q_link_s *qCurr;
  uint8_t errOS;
  sns_sam_io_lowmem_data *lowmemData = NULL;

  do
  {
    sns_os_mutex_pend( samLowMemQMutex, 0, &errOS );
    qCurr = sns_q_get( &samLowMemQ );
    lowmemData = (sns_sam_io_lowmem_data*)qCurr;
    sns_os_mutex_post( samLowMemQMutex );

    if( NULL != lowmemData )
    {
      if( 0 != sns_q_cnt( &algoInstQ ) )
      {
        sns_sam_perform_lowmem( lowmemData );
      }
      SNS_OS_ANY_FREE( lowmemData );
    }
  } while( NULL != lowmemData );
}

/**
 * The USAM Framework thread.
 *
 * @param[i] argPtr Not used
 */
STATIC SNS_SAM_UIMAGE_CODE void
sns_usam_task( void *argPtr )
{
  UNREFERENCED_PARAMETER(argPtr);
  uint8_t errOS;
  OS_FLAGS sigFlags;
  OS_FLAGS sam_sig_mask = SNS_SAM_SIGNAL_MASK;
  sns_sam_err errSAM;

  //qurt_timer_sleep( 15000000 );

  errSAM = sns_sam_pm_init();
  SNS_ASSERT( SAM_ENONE == errSAM );
  errSAM = sns_sam_init_fw();
  SNS_ASSERT( SAM_ENONE == errSAM );
  errSAM = sns_sam_init_dl();
  SNS_ASSERT( SAM_ENONE == errSAM );
  errSAM = sns_sam_init_algos();
  SNS_ASSERT( SAM_ENONE == errSAM );
  errSAM = sns_sam_init_error_fw();
  SNS_ASSERT( SAM_ENONE == errSAM );

  sns_sam_init_gpio( &cbFuncs );
  sns_init_done();

  for( ;; )
  {
    uint32_t i;
    sns_profiling_log_qdss(SNS_SAM_EXIT, 0);

    sns_sam_uimage_vote_enter();

    sigFlags = sns_os_sigs_pend( sns_sam_sig_event, sam_sig_mask,
      OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &errOS );
    sns_profiling_log_qdss( SNS_SAM_ENTER, 1, sigFlags );
    SNS_ASSERT( OS_ERR_NONE == errOS );

    for( i = 0; i < signalsLength; i++ )
    {
      if( sigFlags & samSignals[ i ].signal )
      {
        if( samSignals[ i ].bigImage )
        {
          sns_sam_uimage_exit();
        }
        samSignals[ i ].processFunc();
      }
    }

    //Reset the SAM MIPS vote based on Queue count after processing all signals
    sns_sam_pm_adjust_mips( );
  }
}

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_sam_sensor*
sns_sam_lookup_sensor_from_suid( sns_sam_sensor_uid const *sensorUID )
{
  sns_q_link_s *qCurr;

  for( qCurr = sns_q_check( &sensorQ ); NULL != qCurr;
       qCurr = sns_q_next( &sensorQ, qCurr ) )
  {
    sns_sam_sensor *sensor = (sns_sam_sensor*)qCurr;

    if( *sensorUID == *sensor->sensorUID )
    {
      return sensor;
    }
  }

  return NULL;
}

sns_sam_sensor_algo*
sns_sam_lookup_algo( sns_sam_sensor_uid const *sensorUID )
{
  sns_q_link_s *qCurr;
  sns_sam_sensor_algo *algo = NULL;

  for( qCurr = sns_q_check( &sensorQ ); NULL != qCurr;
       qCurr = sns_q_next( &sensorQ, qCurr ) )
  {
    algo = (sns_sam_sensor_algo*)qCurr;

    if( !algo->sensor.isLocal )
    {
      continue;
    }
    else if( *sensorUID == *algo->sensor.sensorUID )
    {
      return algo;
    }
  }

  return NULL;
}

sns_sam_algoinst_uimage_mode
sns_sam_determine_uimage( sns_sam_sensor_algo const *algo )
{
  bool uImage = false;

  if( NULL == algo->sensor.dlInfo )
  {
    if( SNS_OS_IS_ISLAND_FUNC(algo->algoAPI->sns_sam_algo_update) )
    {
      uImage = true;
    }
  }
  else
  {
    uImage = algo->sensor.dlInfo->uImageAlgo;
  }

  return uImage ? SNS_SAM_ALGOINST_UIMAGE : SNS_SAM_ALGOINST_BIGIMAGE;
}

SNS_SAM_UIMAGE_CODE int
sns_sam_compare_ts( sns_sam_timestamp const ts1, sns_sam_timestamp const ts2 )
{
  if( ts1 == ts2 )
  {
    return 0;
  }
  else if( (ts1 > ts2 &&
            ( ts2 > SNS_SAM_TS_ROLLOVER_LOW_THRESH || ts1 < SNS_SAM_TS_ROLLOVER_HIGH_THRESH ))
           || ( ts2 > SNS_SAM_TS_ROLLOVER_HIGH_THRESH && ts1 < SNS_SAM_TS_ROLLOVER_LOW_THRESH ) )
  {
    return 1;
  }
  else
  {
    return -1;
  }
}

/**
 * SAM Framework initialization.  Called by the Sensors module initializer.
 *
 * @return SNS_SUCCESS
 *         SNS_ERR_FAILED Failure to create signal event or SAM task
 */
sns_err_code_e
sns_usam_init( void )
{
  uint8_t errOS;
  uint32_t i;

  SNS_PRINTF_STRING_LOW_0( samModule, "sns_usam_init" );

  sns_sam_sig_event = sns_os_sigs_create(
    (OS_FLAGS)SNS_SAM_SVC_RESUME_SIG, &errOS );
  SNS_ASSERT( NULL != sns_sam_sig_event );

  for( i = 0; i < signalsLength; i++ )
  {
    sns_os_sigs_add( sns_sam_sig_event, samSignals[ i ].signal );
  }

  errOS = sns_os_task_create_ext( sns_usam_task, NULL,
      &sns_sam_task_stk[ SNS_SAM_MODULE_STK_SIZE - 1 ], SNS_SAM_MODULE_PRIORITY,
      SNS_SAM_MODULE_PRIORITY, &sns_sam_task_stk[0], SNS_SAM_MODULE_STK_SIZE,
      (void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_ISLAND,
      (uint8_t*)("SNS_USAM") );

  SNS_ASSERT( OS_ERR_NONE == errOS );
  return SNS_SUCCESS;
}
