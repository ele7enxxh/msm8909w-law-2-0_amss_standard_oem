/*==============================================================================
  FILE:         rpm_lpr.c

  OVERVIEW:     This file provides the LPR definition for the RPM  
                low-power modes.

  DEPENDENCIES: None

                Copyright (c) 2011-2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/lpr/rpm_lpr.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include <stdbool.h>
#include "HALcomdef.h"
#include "sleepi.h"
#include "sleep_lpr.h"
#include "rpm.h"
#include "rpmclient.h"
#include "kvp.h"
#include "CoreVerify.h"
#include "smd.h"
#include "vmpm_utils.h"
#include "spm.h"
#include "sleep_log.h"
#include "sleep_target.h"

/*==============================================================================
                              INTERNAL VARIABLES
 =============================================================================*/
static uint32 g_nasResult;
static uint32 g_ssResult;

/*==============================================================================
                              INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * RPMSyncLPR_enter
 *
 * @brief Enter function for RPM synchronization low power mode.
 */
void RPMSyncLPR_enter( uint64 wakeup_tick )
{
  /* Indicate we are doing an RPM assisted mode */
  sleepTarget_setRPMAssistedMode(TRUE);

  spm_set_rpm_bypass(0, SPM_RPM_PERFORM_HANDSHAKE);

  /* Tell RPM the wakeup time */
  vmpm_SetWakeupTimetick(wakeup_tick);
  g_ssResult = rpm_force_sync(RPM_SLEEP_SET);

  sleepLog_QDSSPrintf( SLEEP_LOG_LEVEL_INFO, SLEEP_SET_SEND_NUM_ARGS,
                       SLEEP_SET_SEND_STR, SLEEP_SET_SEND,
                       ULOG64_DATA(wakeup_tick) );

  g_nasResult = rpm_force_sync(RPM_NEXT_ACTIVE_SET);

  if(g_ssResult || g_nasResult)
  {
    CORE_VERIFY(smd_disable_intr(SMEM_RPM) == SMD_STATUS_SUCCESS);
  }

  return;
}

/**
 * RPMSyncLPR_exit
 *
 * @brief Exit function for RPM synchronization low power mode.
 */
void RPMSyncLPR_exit( void )
{
  /* Ensure churn_queue uses g_nasResult if valid, else g_ssResult */
  rpm_churn_queue(g_nasResult?g_nasResult:g_ssResult);

  if(g_ssResult || g_nasResult)
  {
    CORE_VERIFY(smd_enable_intr(SMEM_RPM) == SMD_STATUS_SUCCESS);
  }

  /* Change SPM to bypass RPM handshake */
  spm_set_rpm_bypass(0, SPM_RPM_BYPASS_HANDSHAKE);
 
  /* Unset RPM assisted mode */
  sleepTarget_setRPMAssistedMode(FALSE);

  return;
}

