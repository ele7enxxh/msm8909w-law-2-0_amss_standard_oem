/*============================================================================
@file cpr_stub.c

Stub for RBCPR driver. Will be used on targets that don't support the RBCPR
HW on the MSS rail. Goal is to allow sleep LPRM configuration to stay
independent of the HW module on B-family.

Copyright © 2013 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/src/common/cpr_stub.c#1 $
============================================================================*/
#include "CoreVerify.h"
#include "npa.h"
#include "sleep_lpr.h"

/**
 * <!-- CprInitContinueCB -->
 *
 * @brief Continue initialization after our dependencies have become available.
 *
 * @param context : Unused
 * @param event_type : Unused
 * @param data : Unused
 * @param data_size : Unused
 */
static void CprInitContinueCB( void         * context,
                               unsigned int   event_type,
                               void         * data,
                               unsigned int   data_size )
{
  npa_client_handle npaClientHandle;

  CORE_VERIFY_PTR(npaClientHandle =
                  npa_create_sync_client(SLEEP_LPR_NODE_NAME,
                                         "core/power/sleep/rbcpr",
                                         NPA_CLIENT_REQUIRED));

  sleep_define_lpr_str("rbcpr", npaClientHandle);

  npa_issue_required_request(npaClientHandle, SLEEP_ALL_LPRMS);
}


/*----------------------------------------------------------------------------
 * Public interfaces
 * -------------------------------------------------------------------------*/
/**
 * <!-- CprLprmDisableEnter -->
 *
 * @brief Stub for the LPRM.
 */
void CprLprmDisableEnter(uint64_t wakeupTick)
{
}


/**
 * <!-- CprLprmDisableExit -->
 *
 * @brief Stub for the LPRM. 
 */
void CprLprmDisableExit(void)
{
}


/**
 * <!-- CprInit -->
 *
 * @brief Wait for Sleep to become ready and register an LPRM for CPR.
 */
void CprInit(void)
{
  static boolean initDone = FALSE;

  if (initDone)
  {
    return;
  }
  initDone = TRUE;

  npa_resource_available_cb(SLEEP_LPR_NODE_NAME, CprInitContinueCB, NULL);
}

