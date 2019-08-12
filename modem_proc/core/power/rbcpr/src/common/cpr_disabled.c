/*============================================================================
@file cpr_disabled.c

CPR driver stub that interfaces with the system when the CPR HW
is not used. The driver will still continue to correct clkrgm's
default voltage with the offset values blown into the efuses.

Copyright © 2013-2015 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/src/common/cpr_disabled.c#1 $
============================================================================*/
#include "DALStdDef.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "DALSys.h"
#include "DALSysTypes.h"
#include "DDIClock.h"
#include "CoreVerify.h"
#include "cpr_internal.h"
#ifdef WINSIM
#include "cpr_offline.h"
#endif /* WINSIM */

/* Contains all the CPR context data. */
extern CprContextType cprContext;


/**
 * <-- CprAcquireSystemResourcesInDisabledMode -->
 *
 * @brief Acquire resources necessary to correct the default clkrgm voltage
 * with the efused offsets when CPR has been DISABLED in the efuses.
 */
inline static void CprAcquireSystemResourcesInDisabledMode( void )
{
  // We need a DAL handle to be able to correct the default voltage used by
  // clkrgm with initVoltOffsetSteps since this is applicable whether or not
  // CPR is enabled.
  CORE_DAL_VERIFY(DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &cprContext.clkHandle));
}


/**
 * <-- CprDisabledModePreSwitchCallback -->
 *
 * @brief Stub - nothing to do here.
 */
static DALResult CprDisabledModePreSwitchCallback( VCSRailType eRail )
{
  return DAL_SUCCESS;
}


/**
 * <-- CprDisabledModeSetVoltageMode -->
 *
 * @brief  Correct the default clkrgm voltage with the efused offsets obtained
 * by characterization in the manufacturing processes.
 * 
 * @param eRail : Voltage rail identifier
 * @param eCorner : Voltage mode VCS is switching to.
 * @param newVoltage : Return value in microVolts
 */
static DALResult CprDisabledModeSetVoltageMode( VCSRailType eRail,
		                                        VCSCornerType newMode,
                                                uint32 *newVoltage )
{
  CORE_VERIFY((uint32)newMode < CPR_MAX_VMODE_COUNT);

  *newVoltage = cprContext.measurements[newMode].currentVoltage;

  ULOG_RT_PRINTF_2(cprContext.ulogHandle,
                   "CprDisabled (Vmode: %u) (Voltage: %u)",
                    newMode,
                    *newVoltage);

  cprContext.debugTimesInEachMode[newMode]++;
  ULOG_RT_STATIC_PRINTF_6(cprContext.ulogStaticHandle,
                          CPR_ULOG_STATIC_SLOT4_MODECOUNT_INFO,
                          "Entrance Count (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d)",
                          cprContext.debugTimesInEachMode[VCS_CORNER_RETENTION], cprContext.debugTimesInEachMode[VCS_CORNER_LOW_MINUS], cprContext.debugTimesInEachMode[VCS_CORNER_LOW],
                          cprContext.debugTimesInEachMode[VCS_CORNER_NOMINAL], cprContext.debugTimesInEachMode[VCS_CORNER_NOMINAL_PLUS],
                          cprContext.debugTimesInEachMode[VCS_CORNER_TURBO]);

  return DAL_SUCCESS;
}


/**
 * <-- CprDisabledModePostClockSwitchCallback -->
 *
 * @brief Callback that is invoked by clkrgm after it has completed switching
 * to a new voltage/reference clock rate/changed the power source for the CPU. 
 * 
 * @param eRail : Voltage rail identifier
 */
static DALResult CprDisabledModePostClockSwitchCallback( VCSRailType eRail )
{
  // Nothing to do here
  return DAL_SUCCESS;
}


/**
 * <!-- CprDisabledModeMeasureCprNow -->
 *
 * @brief Take a CPR measurement now.
 * 
 */
static DALResult CprDisabledModeMeasureCprNow( VCSRailType nRail,
		                                       VCSCornerType eCorner,
		                                       uint32 *nVoltageuV )
{
  return DAL_SUCCESS;
}


/*----------------------------------------------------------------------------
 * Public interfaces
 * -------------------------------------------------------------------------*/
/**
 * <-- CprInitDisabledMode -->
 *
 * @brief Setup the driver to handle voltage requirements in disabled mode.
 *
 * Initialize the driver so that CPR HW is not used. The driver will still
 * continue to correct clkrgm's default voltage with the offset
 * values blown into the efuses.
 */
void CprInitDisabledMode( CprApiType *pCprApi )
{
  // Even though CPR is disabled, we should still apply the 
  // initVoltOffsetSteps for each voltage mode. This could be handled in
  // clkrgm as well, if they read the same fuses, but we'll do it here
  // so that we can localize all the related info in the CPR driver.

  CprAcquireSystemResourcesInDisabledMode();

  // We'll just be correcting the default clkrgm voltage with a fixed
  // offset, but clkrgm doesn't need to know that!
  pCprApi->Disable = CprDisabledModePreSwitchCallback;
  pCprApi->Enable = CprDisabledModePostClockSwitchCallback;
  pCprApi->GetRailVoltageRecommendation = CprDisabledModeSetVoltageMode;
  pCprApi->MeasureNow = CprDisabledModeMeasureCprNow;
}

