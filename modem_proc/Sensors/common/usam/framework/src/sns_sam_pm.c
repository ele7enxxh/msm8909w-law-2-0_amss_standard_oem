/*============================================================================
  @file sns_sam_pm.c

  @brief
  Functions that will be used by the SAM Framework related to the power management.

  @note
  File line length should generally be limited to <= 80 columns.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  INCLUDE FILES
  ===========================================================================*/

#include "sns_sam_pm.h"
#include "sns_sam.h"
#include "sns_common.h"
#include "sns_pm.h"
#include "sns_sam_ind.h"
#include "sns_sam_io.h"
#include "sns_debug_str.h"
#include "sns_profiling.h"
#include "sns_sam_dep.h"

/*=============================================================================
  MACROS
  ===========================================================================*/

#define SNS_SAM_UIMAGE_WAIT_TIME_US 5000   // 500ms of wait time before entering uImage

#ifdef SNS_USES_ISLAND
# define SNS_SAM_PM_VOTE_MODE(handle, mode)       sns_pm_vote_img_mode(handle, mode)
#else
# define SNS_SAM_PM_VOTE_MODE(handle, mode)       SNS_PM_SUCCESS
#endif /*USES_ISLAND*/

#define SNS_SAM_PM_VOTE_MIPS(handle, mips)       sns_pm_vote_mips(handle, mips)
#define SNS_SAM_PM_VOTE_BW(handle, bw, core)     sns_pm_vote_bw(handle, bw, core)
#define SNS_SAM_PM_INIT(handle, suspendCb, name, client_id) sns_pm_client_init(handle, suspendCb, name, client_id)

/*=============================================================================
  TYPE DEFINITIONS
  ===========================================================================*/

/**
*  SAM's power management information
*/
typedef struct
{

  /** Mutex to protect samImageMode*/
  OS_EVENT                  *samStatusMutexPtr;
  /** The current MIPS*/
  sns_sam_internal_mips_type current_mips;
  /** Handle required by PM*/
  sns_pm_handle_t            pmHandle;
  /** uImage mode is allowed; true = enabled*/
  bool uimage_enabled;
  /** SAM is in uImage */
  bool uImageMode;

} sns_sam_pm_state;

/*=============================================================================
  External Objects
  ===========================================================================*/

/* Contains all unprocessed incoming indication messages */
extern sns_q_s samClntMsgQ;
extern OS_EVENT *samClntMsgQMutex;

/* Client Requests, whose Batch Timers have fired (and need to be handled) */
extern sns_q_s samBatchQ;
extern OS_EVENT *samBatchQMutex;

/* Contains all unprocessed incoming request messages */
extern sns_q_s samReqMsgQ;
extern OS_EVENT *samReqMsgQMutex;

/*=============================================================================
  Static Variables
  ===========================================================================*/

/* Whether the AP is currently in suspend */
static bool apInSuspend SNS_SAM_UIMAGE_DATA = false;
static OS_EVENT *apInSuspendMutex SNS_SAM_UIMAGE_DATA;

/** The internal state maintained by SAM related to power management */
static sns_sam_pm_state sns_sam_pm SNS_SAM_UIMAGE_DATA;

/*=============================================================================
  Static Functions
  ===========================================================================*/

/**
* Callback function for Apps suspend/wakeup
*/
STATIC SNS_SAM_UIMAGE_CODE
void sns_sam_pm_suspend_cb( bool inSuspend )
{
  uint8_t os_err;
  sns_os_mutex_pend( apInSuspendMutex, 0, &os_err );
  SNS_ASSERT(OS_ERR_NONE == os_err);
  apInSuspend = inSuspend;
  os_err = sns_os_mutex_post( apInSuspendMutex );
  SNS_ASSERT(OS_ERR_NONE == os_err);

  sns_os_sigs_post( sns_sam_sig_event, SNS_SAM_PM_SIG, OS_FLAG_SET, &os_err );
  SNS_ASSERT( OS_ERR_NONE == os_err );
}


STATIC SNS_SAM_UIMAGE_CODE uint32_t
sns_sam_q_count( sns_q_s *samQ, OS_EVENT *samQMutex )
{
  uint32_t samQCnt;
  uint8_t errOS;

  sns_os_mutex_pend( samQMutex, 0, &errOS );
  samQCnt = sns_q_cnt(samQ);
  sns_os_mutex_post( samQMutex );

  return samQCnt;
}

/*=============================================================================
  Public Functions
  ===========================================================================*/

void
sns_sam_handle_pm_event( void )
{
  sns_q_link_s *qCurrAI;

  if( !sns_sam_pm_ap_suspend() )
  {
    sns_sam_resume_client_timers();
    sns_sam_flush_missed_event_output();
    sns_sam_flush_batched_ap_output();
  }
  else
  {
    if( 0 != sns_q_cnt( &algoInstQ ) )
    {
      sns_sam_drop_unneeded_output();
      sns_sam_pause_client_timers();
    }
  }
  
  // Always recompute min batch period after PM state change
  for( qCurrAI = sns_q_check( &algoInstQ ); NULL != qCurrAI;
       qCurrAI = sns_q_next( &algoInstQ, qCurrAI ) )
  {
    sns_sam_algo_inst *algoInst = (sns_sam_algo_inst*)qCurrAI;
    sns_sam_inform_report_change(algoInst);
  }
  
}

SNS_SAM_UIMAGE_CODE void
sns_sam_uimage_vote_enter()
{
  uint8_t os_err;

  sns_os_mutex_pend(sns_sam_pm.samStatusMutexPtr, 0, &os_err);
  SNS_ASSERT(OS_ERR_NONE == os_err);
  sns_sam_pm.uImageMode = true;

  if( 0 == algoInstQ.cnt )
  {
    SNS_SAM_PM_VOTE_MODE( sns_sam_pm.pmHandle, SNS_IMG_MODE_BIG );
    sns_sam_pm_vote_mips(SNS_SAM_MIPS_IDLE);
    SNS_SAM_PM_VOTE_MODE( sns_sam_pm.pmHandle, SNS_IMG_MODE_NOCLIENT );
  }
  else
  {
    SNS_SAM_PM_VOTE_MODE( sns_sam_pm.pmHandle, SNS_IMG_MODE_MICRO );
    sns_profiling_log_qdss( SNS_SAM_UIMAGE_ENTER, 0 );
  }

  os_err = sns_os_mutex_post(sns_sam_pm.samStatusMutexPtr);
  SNS_ASSERT(OS_ERR_NONE == os_err);
}


SNS_SAM_UIMAGE_CODE void
sns_sam_uimage_exit()
{
  uint8_t os_err;

  sns_os_mutex_pend(sns_sam_pm.samStatusMutexPtr, 0, &os_err);
  SNS_ASSERT(OS_ERR_NONE == os_err);

  sns_sam_pm.uImageMode = false;

  SNS_SAM_PM_VOTE_MODE( sns_sam_pm.pmHandle, SNS_IMG_MODE_BIG);
  sns_profiling_log_qdss( SNS_SAM_UIMAGE_EXIT, 2, 1 );

  os_err = sns_os_mutex_post(sns_sam_pm.samStatusMutexPtr);
  SNS_ASSERT(OS_ERR_NONE == os_err);
}

void
sns_sam_pm_vote_mips( sns_sam_internal_mips_type newMips )
{
  if(sns_sam_pm.current_mips != newMips)
  {
    uint8_t os_err;
    sns_pm_err_code_e pm_err;
    pm_err = SNS_SAM_PM_VOTE_MIPS( sns_sam_pm.pmHandle, sns_sam_mips_table[newMips] );
    if( SNS_PM_SUCCESS != pm_err )
    {
      SNS_PRINTF_STRING_ERROR_1(samModule, "Error voting for PM MIPS %i", pm_err);
    }
    sns_os_mutex_pend(sns_sam_pm.samStatusMutexPtr, 0, &os_err);
    SNS_ASSERT(OS_ERR_NONE == os_err);

    sns_sam_pm.current_mips = newMips;

    os_err = sns_os_mutex_post(sns_sam_pm.samStatusMutexPtr);
    SNS_ASSERT(OS_ERR_NONE == os_err);
  }
}

SNS_SAM_UIMAGE_CODE void
sns_sam_pm_adjust_mips( void )
{
  uint32_t samReqQCnt, samBatchQCnt, samClntQCnt;

  // Get SAM queue counts
  samReqQCnt = sns_sam_q_count(&samReqMsgQ, samReqMsgQMutex);
  samBatchQCnt = sns_sam_q_count(&samBatchQ, samBatchQMutex);
  samClntQCnt = sns_sam_q_count(&samClntMsgQ, samClntMsgQMutex);

  // If any of the SAM queues have > threshold items, vote for higher MIPS
  if( ((samReqQCnt > SNS_SAM_Q_UPPER_THRESH) ||
      (samBatchQCnt > SNS_SAM_Q_UPPER_THRESH) || (samClntQCnt > SNS_SAM_Q_UPPER_THRESH) ) &&
       SNS_SAM_MIPS_TURBO != sns_sam_pm.current_mips )
  {
    sns_sam_uimage_exit();
    sns_sam_pm_vote_mips( SNS_SAM_MIPS_TURBO );
  }
  // If all of the SAM queues have < threshold items, and we are not already
  // in IDLE, vote for normal MIPS
  else if( (samReqQCnt < SNS_SAM_Q_LOWER_THRESH) &&
      (samBatchQCnt < SNS_SAM_Q_LOWER_THRESH) && (samClntQCnt < SNS_SAM_Q_LOWER_THRESH)&&
      ((SNS_SAM_MIPS_IDLE != sns_sam_pm.current_mips) &&
      (SNS_SAM_MIPS_NORMAL != sns_sam_pm.current_mips)))
  {
    sns_sam_uimage_exit();
    sns_sam_pm_vote_mips( SNS_SAM_MIPS_NORMAL );
  }
}

sns_sam_err
sns_sam_pm_init()
{
  uint8_t os_err;
  sns_pm_err_code_e pm_err;

  sns_sam_pm.uImageMode = false;
  sns_sam_pm.current_mips = SNS_SAM_MIPS_NORMAL;

  apInSuspendMutex = sns_os_mutex_create_uimg(0, &os_err);
  SNS_ASSERT(OS_ERR_NONE == os_err);

  sns_sam_pm.samStatusMutexPtr = sns_os_mutex_create_uimg(0, &os_err);
  if( OS_ERR_NONE != os_err )
  {
    sns_sam_pm.uimage_enabled = false;
    return SAM_ENONE;
  }

  pm_err = SNS_SAM_PM_INIT( &sns_sam_pm.pmHandle, (suspend_cb_t)&sns_sam_pm_suspend_cb, "SAM", SNS_PM_CLIENT_ID_SAM);
  if( SNS_PM_SUCCESS != pm_err )
  {
    sns_sam_pm.uimage_enabled = false;
    return SAM_EFAILED;
  }

  pm_err = SNS_SAM_PM_VOTE_MIPS(sns_sam_pm.pmHandle, sns_sam_mips_table[sns_sam_pm.current_mips]);
  pm_err = SNS_SAM_PM_VOTE_MODE(sns_sam_pm.pmHandle, SNS_IMG_MODE_BIG);

  sns_sam_pm.uimage_enabled = true;
  return SAM_ENONE;
}

SNS_SAM_UIMAGE_CODE bool
sns_sam_pm_ap_suspend()
{
  bool retVal;
  uint8_t os_err;

  sns_os_mutex_pend(apInSuspendMutex, 0, &os_err);
  SNS_ASSERT(OS_ERR_NONE == os_err);
  retVal = apInSuspend;
  os_err = sns_os_mutex_post(apInSuspendMutex);
  SNS_ASSERT(OS_ERR_NONE == os_err);

  return retVal;
}

SNS_SAM_UIMAGE_CODE void
sns_sam_pm_ap_wakeup_broadcast(void)
{
#ifndef FAST_PLAYBACK
  sns_pm_ap_wakeup_broadcast();
#endif
}
