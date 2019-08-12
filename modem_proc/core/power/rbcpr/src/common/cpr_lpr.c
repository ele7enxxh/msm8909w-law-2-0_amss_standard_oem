/*============================================================================
@file cpr_lpr.c

Manages CPR in low power modes such as power collapse, where the CPR HW needs
to be disabled. 
  
Copyright © 2013-2015 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/src/common/cpr_lpr.c#1 $
============================================================================*/
#include "CoreVerify.h"
#include "cpr_internal.h"
#include "npa_resource.h"
#include "sleep_lpr.h"
#ifdef WINSIM
#include "cpr_offline.h"
#endif /* WINSIM */


// Contains all the CPR context data.
extern CprContextType cprContext;


/**
 * <!-- CprSetLowPowerMode -->
 * 
 * @brief Calls the registered low power state handler for CPR. 
 * 
 * The handler is required to disable the HW block during power 
 * collapse. 
 * 
 * @param enteringLowPowerMode : TRUE if entering power collapse, else FALSE
 */
static void CprSetLowPowerMode(unsigned long int enteringLowPowerMode)
{
  
  if (!(cprContext.enableFlags_ClosedLoopEnabled) || (cprContext.enableFlags_DisabledByFuse))
  {
    return;
  }

  // No need to use the mutex here - sleep code will guarantee that
  // interrupts are disabled and we should not expect clkrgm to be changing
  // corners either (other than going to retention, which is transparent to us).
  if (cprContext.lpFunc != NULL)
  {
    cprContext.lpFunc(enteringLowPowerMode);
  }

  ULogFront_RealTimePrintf(
    cprContext.ulogHandle,
    1,
    "CPR (PwrCollapse: %d)",
    enteringLowPowerMode);
}


/*----------------------------------------------------------------------------
 * Public interfaces
 * -------------------------------------------------------------------------*/
/**
 * <!-- CprLprmDisableEnter -->
 * 
 * @brief Entering power collapse mode. Disable the CPR HW.
 * 
 * @param wakeupTick : Unused
 */
void CprLprmDisableEnter(uint64_t wakeupTick)
{
  CprSetLowPowerMode(TRUE);
}


/**
 * <!-- CprLprmDisableExit -->
 * 
 * @brief Leaving power collapse mode. Re-enable the CPR HW.
 */
void CprLprmDisableExit( void )
{
  CprSetLowPowerMode(FALSE);
}


/**
 * <!-- CprInitLowPowerMgr -->
 * 
 * @brief Register an LPRM with Sleep so that we can disable CPR during 
 * power collapse. 
 */
void CprInitLowPowerMgr( void )
{
  npa_client_handle npaClientHandle;

  CORE_VERIFY_PTR(npaClientHandle =
                  npa_create_sync_client(SLEEP_LPR_NODE_NAME,
                                         "mss_rbcpr",
                                         NPA_CLIENT_REQUIRED));

  sleep_define_lpr_str("rbcpr", npaClientHandle);

  npa_issue_required_request(npaClientHandle, SLEEP_ALL_LPRMS);
}

