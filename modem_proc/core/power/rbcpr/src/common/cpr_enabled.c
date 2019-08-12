/*============================================================================
@file cpr_enabled.c

CPR driver implementation. Will manage the CPR HW if the user and efuse
config allow the use of CPR.

Copyright © 2013, 2015 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/src/common/cpr_enabled.c#1 $
============================================================================*/
#include "DALStdDef.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "DALSys.h"
#include "DALSysTypes.h"
#include "CoreVerify.h"
#include "DDIClock.h"
#include "DDIInterruptController.h"
#include "HAL_cpr.h"
#include "HALhwio.h"
#include "mss_cpr_hwio.h"
#include "cpr_internal.h"
#include "ClockDefs.h"
#include "npa.h"
#include "DDIVCS.h"
#include "ULogFront.h"
#include "VCSDefs.h" /* Enums for the power modes */
#ifdef WINSIM
#include "cpr_offline.h"
#endif /* WINSIM */

/* Contains all the CPR context data. */
extern CprContextType cprContext;


/**
 * <!-- CprEnable -->
 *
 * @brief Enable/disable CPR measurements and associated interrupts.
 * 
 * @param enable: TRUE to enable; FALSE otherwise.
 */
static void CprEnable( boolean enable )
{
  //ULOG_RT_PRINTF_1(cprContext.ulogHandle, "CprEnable %x", enable);
  HalCprConfigInterrupts(enable, cprContext.upIntEnabled, cprContext.downIntEnabled);
  HalCprEnable(enable);
}


//void DumpMssCprLogRegistersToLog( ULogHandle h )
//{
//  ULogFront_RealTimePrintf(h, 8,"(MSS_RBCPR_LOG 0 to 6: (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x)",
//                                 HWIO_IN(MSS_RBCPR_LOG_0), HWIO_IN(MSS_RBCPR_LOG_1), HWIO_IN(MSS_RBCPR_LOG_2), HWIO_IN(MSS_RBCPR_LOG_3),
//                                 HWIO_IN(MSS_RBCPR_LOG_4), HWIO_IN(MSS_RBCPR_LOG_5), HWIO_IN(MSS_RBCPR_LOG_6),HWIO_IN(MSS_RBCPR_LOG_7));
//}

// void CprDumpMssCprRegistersToLog( ULogHandle h )
// {
  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_VERSION: 0x%x", HWIO_IN(MSS_RBCPR_VERSION));

  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_SENSOR_MASK0: 0x%x", HWIO_IN(MSS_RBCPR_SENSOR_MASK0));
  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_SENSOR_MASK1: 0x%x", HWIO_IN(MSS_RBCPR_SENSOR_MASK1));
  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_SENSOR_MASK2: 0x%x", HWIO_IN(MSS_RBCPR_SENSOR_MASK2));
  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_SENSOR_MASK3: 0x%x", HWIO_IN(MSS_RBCPR_SENSOR_MASK3));
  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_SENSOR_BYPASS0: 0x%x", HWIO_IN(MSS_RBCPR_SENSOR_BYPASS0));
  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_SENSOR_BYPASS1: 0x%x", HWIO_IN(MSS_RBCPR_SENSOR_BYPASS1));
  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_SENSOR_BYPASS2: 0x%x", HWIO_IN(MSS_RBCPR_SENSOR_BYPASS2));
  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_SENSOR_BYPASS3: 0x%x", HWIO_IN(MSS_RBCPR_SENSOR_BYPASS3));

  // ULogFront_RealTimePrintf(h, 1,"MSS_RBIF_PMIC_ADDR: 0x%x", HWIO_IN(MSS_RBIF_PMIC_ADDR));

  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_TIMER_INTERVAL: 0x%x", HWIO_IN(MSS_RBCPR_TIMER_INTERVAL));

  // ULogFront_RealTimePrintf(h, 1,"MSS_RBIF_LIMIT: 0x%x", HWIO_IN(MSS_RBIF_LIMIT));

  // ULogFront_RealTimePrintf(h, 1,"MSS_RBIF_TIMER_ADJUST: 0x%x", HWIO_IN(MSS_RBIF_TIMER_ADJUST));

  // ULogFront_RealTimePrintf(h, 3,"MSS_RBCPR_GCNT_TARGET0: 0x%x gcnt:%d  target:%d", HWIO_IN(MSS_RBCPR_GCNT_TARGET0),(HWIO_IN(MSS_RBCPR_GCNT_TARGET0)&0x3ff000)>>12,(HWIO_IN(MSS_RBCPR_GCNT_TARGET0)&0xfff));
  // ULogFront_RealTimePrintf(h, 3,"MSS_RBCPR_GCNT_TARGET1: 0x%x gcnt:%d  target:%d", HWIO_IN(MSS_RBCPR_GCNT_TARGET1),(HWIO_IN(MSS_RBCPR_GCNT_TARGET1)&0x3ff000)>>12,(HWIO_IN(MSS_RBCPR_GCNT_TARGET1)&0xfff));
  // ULogFront_RealTimePrintf(h, 3,"MSS_RBCPR_GCNT_TARGET2: 0x%x gcnt:%d  target:%d", HWIO_IN(MSS_RBCPR_GCNT_TARGET2),(HWIO_IN(MSS_RBCPR_GCNT_TARGET2)&0x3ff000)>>12,(HWIO_IN(MSS_RBCPR_GCNT_TARGET2)&0xfff));
  // ULogFront_RealTimePrintf(h, 3,"MSS_RBCPR_GCNT_TARGET3: 0x%x gcnt:%d  target:%d", HWIO_IN(MSS_RBCPR_GCNT_TARGET3),(HWIO_IN(MSS_RBCPR_GCNT_TARGET3)&0x3ff000)>>12,(HWIO_IN(MSS_RBCPR_GCNT_TARGET3)&0xfff));
  // ULogFront_RealTimePrintf(h, 3,"MSS_RBCPR_GCNT_TARGET4: 0x%x gcnt:%d  target:%d", HWIO_IN(MSS_RBCPR_GCNT_TARGET4),(HWIO_IN(MSS_RBCPR_GCNT_TARGET4)&0x3ff000)>>12,(HWIO_IN(MSS_RBCPR_GCNT_TARGET4)&0xfff));
  // ULogFront_RealTimePrintf(h, 3,"MSS_RBCPR_GCNT_TARGET5: 0x%x gcnt:%d  target:%d", HWIO_IN(MSS_RBCPR_GCNT_TARGET5),(HWIO_IN(MSS_RBCPR_GCNT_TARGET5)&0x3ff000)>>12,(HWIO_IN(MSS_RBCPR_GCNT_TARGET5)&0xfff));
  // ULogFront_RealTimePrintf(h, 3,"MSS_RBCPR_GCNT_TARGET6: 0x%x gcnt:%d  target:%d", HWIO_IN(MSS_RBCPR_GCNT_TARGET6),(HWIO_IN(MSS_RBCPR_GCNT_TARGET6)&0x3ff000)>>12,(HWIO_IN(MSS_RBCPR_GCNT_TARGET6)&0xfff));
  // ULogFront_RealTimePrintf(h, 3,"MSS_RBCPR_GCNT_TARGET7: 0x%x gcnt:%d  target:%d", HWIO_IN(MSS_RBCPR_GCNT_TARGET7),(HWIO_IN(MSS_RBCPR_GCNT_TARGET7)&0x3ff000)>>12,(HWIO_IN(MSS_RBCPR_GCNT_TARGET7)&0xfff));


  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_STEP_QUOT: 0x%x", HWIO_IN(MSS_RBCPR_STEP_QUOT));
  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_CTL: 0x%x", HWIO_IN(MSS_RBCPR_CTL));
  // ULogFront_RealTimePrintf(h, 1,"MSS_RBIF_SW_VLEVEL: 0x%x", HWIO_IN(MSS_RBIF_SW_VLEVEL));
  /* MMSS_RBIF_CONT_ACK_CMD is write only */
  /* MMSS_RBIF_CONT_NACK_CMD is write only */

  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_RESULT_0: 0x%x", HWIO_IN(MSS_RBCPR_RESULT_0));
  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_RESULT_1: 0x%x", HWIO_IN(MSS_RBCPR_RESULT_1));

  // ULogFront_RealTimePrintf(h, 1,"MSS_RBIF_IRQ_EN_0: 0x%x", HWIO_IN(MSS_RBIF_IRQ_EN_0));
  // ULogFront_RealTimePrintf(h, 1,"MSS_RBIF_IRQ_EN_1: 0x%x", HWIO_IN(MSS_RBIF_IRQ_EN_1));
  // ULogFront_RealTimePrintf(h, 1,"MSS_RBIF_IRQ_EN_2: 0x%x", HWIO_IN(MSS_RBIF_IRQ_EN_2));
  /* MMSS_RBIF_IRQ_CLEAR is write only */

  // ULogFront_RealTimePrintf(h, 1,"MSS_RBIF_IRQ_STATUS: 0x%x", HWIO_IN(MSS_RBIF_IRQ_STATUS));

  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_QUOT_AVG: 0x%x", HWIO_IN(MSS_RBCPR_QUOT_AVG));
  // ULogFront_RealTimePrintf(h, 3,"MSS_RBCPR_DEBUG 0 to 2 : (0x%x) (0x%x) (0x%x)", HWIO_IN(MSS_RBCPR_DEBUG0),HWIO_IN(MSS_RBCPR_DEBUG1),HWIO_IN(MSS_RBCPR_DEBUG2));
  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_BIST_MINMAX: 0x%x", HWIO_IN(MSS_RBCPR_BIST_MINMAX));
  // ULogFront_RealTimePrintf(h, 1,"MSS_RBCPR_BIST_RESULT: 0x%x", HWIO_IN(MSS_RBCPR_BIST_RESULT));
  // ULogFront_RealTimePrintf(h, 4,"MSS_RBCPR_BIST_FAIL_MAP 0 to 3: (0x%x) (0x%x) (0x%x) (0x%x) ", HWIO_IN(MSS_RBCPR_BIST_FAIL_MAP0), HWIO_IN(MSS_RBCPR_BIST_FAIL_MAP1), HWIO_IN(MSS_RBCPR_BIST_FAIL_MAP2), HWIO_IN(MSS_RBCPR_BIST_FAIL_MAP3));
  // ULogFront_RealTimePrintf(h, 8,"(MSS_RBCPR_LOG 0 to 6: (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x)",
                                 // HWIO_IN(MSS_RBCPR_LOG_0), HWIO_IN(MSS_RBCPR_LOG_1), HWIO_IN(MSS_RBCPR_LOG_2), HWIO_IN(MSS_RBCPR_LOG_3),
                                 // HWIO_IN(MSS_RBCPR_LOG_4), HWIO_IN(MSS_RBCPR_LOG_5), HWIO_IN(MSS_RBCPR_LOG_6),HWIO_IN(MSS_RBCPR_LOG_7));
// }

/**
 * <!-- CprSwitchVoltage -->
 *
 * @brief Tell the clock driver to switch to the new voltage at the specified
 * voltage mode.
 * 
 * @param newVoltage: voltage in microVolts
 * 
 * @return boolean: TRUE if successful; FALSE otherwise.
 */
static boolean CprSwitchVoltage( uint32 newVoltage )
{
  DALResult retval;	
  //ULOG_RT_PRINTF_1(cprContext.ulogHandle, "CprSwitchVoltage newVoltage %d", newVoltage);

  retval = DalVCS_SetRailVoltage(cprContext.vcsHandle, VCS_RAIL_MSS, newVoltage);

  if (retval == DAL_SUCCESS)
  {
    cprContext.measurements[cprContext.currentVmode].currentVoltage = newVoltage;
	
    if (cprContext.measurements[cprContext.currentVmode].currentVoltage < cprContext.debugMinVoltageInEachMode[cprContext.currentVmode])
    {
      cprContext.debugMinVoltageInEachMode[cprContext.currentVmode] = cprContext.measurements[cprContext.currentVmode].currentVoltage;

      ULOG_RT_STATIC_PRINTF_6(cprContext.ulogStaticHandle, 
                              CPR_ULOG_STATIC_SLOT3_MIN_VOLTAGE_INFO,
                              "Min Voltages (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d)", 
                              cprContext.debugMinVoltageInEachMode[VCS_CORNER_RETENTION], cprContext.debugMinVoltageInEachMode[VCS_CORNER_LOW_MINUS], cprContext.debugMinVoltageInEachMode[VCS_CORNER_LOW],
                              cprContext.debugMinVoltageInEachMode[VCS_CORNER_NOMINAL], cprContext.debugMinVoltageInEachMode[VCS_CORNER_NOMINAL_PLUS],
                              cprContext.debugMinVoltageInEachMode[VCS_CORNER_TURBO]);

    }
	
	
    ULOG_RT_STATIC_PRINTF_6(cprContext.ulogStaticHandle, 
                            CPR_ULOG_STATIC_SLOT2_CURR_VOLTAGE_INFO,
                            "Current Voltages (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d)", 
                            cprContext.measurements[VCS_CORNER_RETENTION].currentVoltage, cprContext.measurements[VCS_CORNER_LOW_MINUS].currentVoltage, cprContext.measurements[VCS_CORNER_LOW].currentVoltage,
                            cprContext.measurements[VCS_CORNER_NOMINAL].currentVoltage, cprContext.measurements[VCS_CORNER_NOMINAL_PLUS].currentVoltage,
                            cprContext.measurements[VCS_CORNER_TURBO].currentVoltage);

    // Disable the sensors on the CPU if the LDO is being used to power the CPU
    // at a lower voltage - CPR HW needs all the sensors to be at the
    // same voltage.
    HalCprBypassCpuBlock(cprContext.cpuNotUsingMssRail);

    return TRUE;
  }
  
  ULOG_RT_PRINTF_2(cprContext.ulogHandle, "SetRailVoltage %d in mode %d returned FALSE, the voltage was not changed.  This should indicate a mode change was in progress.", newVoltage, cprContext.currentVmode);
  return FALSE;
}


/**
 * <!-- CprPreSwitchConfig -->
 *
 * @brief Prepare the HW for a voltage/reference clock switch by disabling & 
 * clearing interrupts. 
 */
static void CprPreSwitchConfig( void )
{
  //ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CprPreSwitchConfig");
  cprContext.switchingVmode = TRUE;

  // Disable the HW in preparation for the switch.
  CprEnable(FALSE);

  // Clear any pending interrupts
  HalCprClearAllInterrupts();
}


/**
 * <!-- CprEnabledModePreSwitchCallback -->
 *
 * @brief Callback which will be invoked by clkrgm prior to switching to a 
 * different voltage mode. 
 *
 * The CPR driver uses this callback to disable the CPR block in preparation for
 * the mode switching. clkrgm is required to invoke CprEnabledModeSetVoltageMode() 
 * after it finishes switching to the newMode.
 */
static DALResult CprEnabledModePreSwitchCallback( VCSRailType eRail )
{
  //ULOG_RT_PRINTF_1(cprContext.ulogHandle, "CprEnabledModePreSwitchCallback eRail %d", eRail);  
  CPR_MUTEX_ACQUIRE();

  CprPreSwitchConfig();

  ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CPR (SwitchingMode: 1) (Enabled: 0)");

  CPR_MUTEX_RELEASE();

  return DAL_SUCCESS;
}


/**
 * <!-- CprEnabledModeSetVoltageMode -->
 *
 * @brief Callback that is invoked by clkrgm whenever it switches to another 
 * voltage mode to query CPR for the recommended voltage to use for that mode.
 *
 * Clkrgm should follow this call with a call to 
 * CprEnabledModePostSwitchCallback().
 * 
 * @param eRail : Voltage rail identifier
 * @param newMode : New voltage mode
 * @param newVoltage : Return value in microVolts
 */
static DALResult CprEnabledModeSetVoltageMode( VCSRailType eRail,
                                               VCSCornerType newMode,
                                               uint32 * newVoltage)
{
  //ULOG_RT_PRINTF_2(cprContext.ulogHandle, "CprEnabledModeSetVoltageMode (eRail %d) (newMode %d)", eRail, newMode);  
  CORE_VERIFY((uint32)newMode < CPR_MAX_VMODE_COUNT);

  CPR_MUTEX_ACQUIRE();

  // Use the last recommendation from the HW.
  *newVoltage = cprContext.measurements[newMode].currentVoltage;

  if (!cprContext.switchingVmode)
  {
    if ((cprContext.currentVmode < CPR_MAX_VMODE_COUNT) &&
        (newMode != cprContext.currentVmode))
    {
      ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CPR (ERROR: SetVoltage called before PreSwitch)");

      // Disable the HW as a safety measure
      CprPreSwitchConfig();
    }
  }
  cprContext.currentVmode = newMode;

  cprContext.debugTimesInEachMode[newMode]++;
  ULOG_RT_STATIC_PRINTF_6(cprContext.ulogStaticHandle, 
                          CPR_ULOG_STATIC_SLOT4_MODECOUNT_INFO,
                          "Entrance Count (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d)", 
                          cprContext.debugTimesInEachMode[VCS_CORNER_RETENTION], cprContext.debugTimesInEachMode[VCS_CORNER_LOW_MINUS], cprContext.debugTimesInEachMode[VCS_CORNER_LOW],
                          cprContext.debugTimesInEachMode[VCS_CORNER_NOMINAL], cprContext.debugTimesInEachMode[VCS_CORNER_NOMINAL_PLUS],
                          cprContext.debugTimesInEachMode[VCS_CORNER_TURBO]);

 // Cap at the ceiling and disable/enable up ints
  if (*newVoltage >= cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[newMode]->voltageCeiling)
  {
    *newVoltage = cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[newMode]->voltageCeiling;
    cprContext.upIntEnabled = FALSE;
  } 
  else  
  {
    cprContext.upIntEnabled = TRUE;
  }

  // Cap at the floor and disable/enable down ints
  if (*newVoltage <= cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[newMode]->voltageFloor)
  {
    *newVoltage = cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[newMode]->voltageFloor;
    cprContext.downIntEnabled = FALSE;
  } else {
    cprContext.downIntEnabled = TRUE;
  }

  cprContext.measurements[newMode].currentVoltage = *newVoltage;

  ULOG_RT_PRINTF_2(cprContext.ulogHandle, "CPR (Vmode: %u) (Voltage: %u)", newMode, *newVoltage);

  CPR_MUTEX_RELEASE();

  return DAL_SUCCESS;
}


/**
 * <!-- CprEnableDownRecommendation -->
 *
 * @brief Change the cprContext downIntEnabled flag and call the
 * Hal configuration function.
 *
 * @param enable : turn on or off the down enable. 
 */
void CprEnableDownRecommendation( boolean enable ){
  //ULOG_RT_PRINTF_1(cprContext.ulogHandle, "CprEnableDownRecommendation (enable %d)", enable);  
  cprContext.downIntEnabled  = enable;
  HalCprEnableDownRecommendation(enable);
}


/**
 * <!-- CprEnableUpRecommendation -->
 *
 * @brief Change the cprContext upIntEnabled flag and call the
 * Hal configuration function.
 *
 * @param enable : turn on or off the up enable. 
 */
void CprEnableUpRecommendation( boolean enable ){
  //ULOG_RT_PRINTF_1(cprContext.ulogHandle, "CprEnableUpRecommendation (enable %d)", enable);  
  cprContext.upIntEnabled  = enable;
  HalCprEnableUpRecommendation(enable);
}


/**
 * <!-- CprEnabledModePostSwitchCallback -->
 *
 * @brief Callback that is invoked by clkrgm after it has completed switching
 * to a new voltage/reference clock rate/changed the power source for the CPU. 
 * 
 * @param eRail : Voltage rail identifier
 */
static DALResult CprEnabledModePostSwitchCallback( VCSRailType eRail )
{
  uint32 currRailVoltage = cprContext.measurements[cprContext.currentVmode].currentVoltage;
  //Hard coding the 9x35 frequency to the correct value.
  uint32 refClkRateKhz = 19200;   

  //ULOG_RT_PRINTF_1(cprContext.ulogHandle, "CprEnabledModePostSwitchCallback (eRail %d)", eRail);  

  cprContext.currentRefClkRate = refClkRateKhz;


  if (!(cprContext.enableFlags_ClosedLoopEnabled) || (cprContext.enableFlags_DisabledByFuse))
  {
    return DAL_SUCCESS;
  }

  CORE_VERIFY( CPR_IS_VALID_REFERENCE_CLOCK_RATE(refClkRateKhz) );

  CPR_MUTEX_ACQUIRE();

  // At init, currentVmode is set to CPR_VMODE_INVALID 
  if (cprContext.currentVmode >= CPR_MAX_VMODE_COUNT)
  {
    ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CPR (ERROR: Post called before setting initial Voltage mode)");
    DALSYS_SyncLeave(cprContext.mutexHandle);
    return DAL_ERROR_INTERNAL;
  }

  if (!cprContext.switchingVmode)
  {
    if(cprContext.clearedAfterFirstUse)
    {
      /* The first time PostSwitch is called the CPR driver is enabled.
         For future transitions we will always expect PreSwitch to be called before PostSwitch. */
      cprContext.clearedAfterFirstUse = FALSE; 
      ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CPR PostSwitch enabling CPR for 1st use.");
    } else {
      ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CPR (ERROR: Post called before PreSwitch)");
    }
    // Try to recover as best as we can 
    CprPreSwitchConfig();
  }

  /* The first time PostSwitch is called the CPR driver is enabled.
     For future transitions we will always expect PreSwitch to be called before PostSwitch. */
  cprContext.clearedAfterFirstUse = FALSE; 

  /* Reconfigure the CPR HW with the new settings */
  HalCprSetupVoltageMode(cprContext.currentVmode, refClkRateKhz, cprContext.targetCfg->delayBeforeNextMeas);

  HalCprBypassCpuBlock(cprContext.cpuNotUsingMssRail);

  if (currRailVoltage <= cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[cprContext.currentVmode]->voltageFloor)
  {
    /* Disable the down interrupts if we're already at the floor voltage. */
    CprEnableDownRecommendation(FALSE);
  } 
  else
  {
    CprEnableDownRecommendation(TRUE);
  }

  if (currRailVoltage >= cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[cprContext.currentVmode]->voltageCeiling)
  {
    /* Disable the up interrupts if we're already at the ceiling voltage. */
    CprEnableUpRecommendation(FALSE);
  } 
  else
  {
     CprEnableUpRecommendation(TRUE);
  }

  if (currRailVoltage != cprContext.measurements[cprContext.currentVmode].currentVoltage)
  {
    ULOG_RT_PRINTF_3(cprContext.ulogHandle,
                     "ERROR (RailVolt: %lu) (Vmode: %lu) (Voltage: %lu)",
                     currRailVoltage, cprContext.currentVmode,
                     cprContext.measurements[cprContext.currentVmode].currentVoltage);

    cprContext.measurements[cprContext.currentVmode].currentVoltage = currRailVoltage;

    // Log the voltage so the USee tool can plot the voltage accurately.
    ULOG_RT_PRINTF_1(cprContext.ulogHandle, "CPR (Voltage: %lu)", currRailVoltage);
  }

  ULOG_RT_PRINTF_2(cprContext.ulogHandle, "CPR (SwitchingMode: 0) (Enabled: 1) (RefClkRate: %lu) (CPUnotUsingRail: %lu)", refClkRateKhz, cprContext.cpuNotUsingMssRail);

  cprContext.switchingVmode = FALSE;
  CprEnable(TRUE);

  CPR_MUTEX_RELEASE();

  return DAL_SUCCESS;
}


/**
 * <!-- CprGetOffsetRecommendation -->
 *
 * @brief Get the new offset recommended by the HW.
 * 
 * @return int32: Offset in microVolts. Recommendations are multiples of the step size. 
 */
static int32 CprGetOffsetRecommendation(void)
{
  //ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CprGetOffsetRecommendation");  
  
  // Poll until the HW is not busy - this shouldn't really be necessary since
  // the interrupt already implies that a result is ready. 
  while(HalCprIsBusy());

  return (HalCprGetOffsetRecommendation() * cprContext.targetCfg->voltageStepSize);
}


/**
 * <!-- CprIsr -->
 *
 * @brief ISR for CPR interrupts. Makes corrections to the voltage at the current
 * operating voltage mode based on the HW's recommendation.
 *
 * The ack nack decision is simple.  If the voltage is moving (by our limited movement 
 * of 1 pmic step) ack, otherwise nack. 
 * 
 * @param unused
 */
static void CprIsr( uint32 unused )
{
  boolean           ack = TRUE;
  int32             offset;
  uint32            currVmode;
  uint32            newVoltage;
  CprVmodeBoundsAndOffsetCfgType * currVmodeBoundsAndOffsetCfgPtr;

  //ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CprIsr");  
  
  if (!(cprContext.enableFlags_ClosedLoopEnabled) || (cprContext.enableFlags_DisabledByFuse))
  {
    return;
  }

  CPR_MUTEX_ACQUIRE();

  if (cprContext.switchingVmode ||
      (cprContext.currentVmode >= CPR_MAX_VMODE_COUNT) ||
      (!cprContext.currentRefClkRate))
  {
    // This may happen if the interrupt fired just as we were executing the PreSwitch callback.
    HalCprClearAllInterrupts();
    HalCprStartNextMeasurement(FALSE);
    DALSYS_SyncLeave(cprContext.mutexHandle);
    ULOG_RT_PRINTF_3(cprContext.ulogHandle,"ISR during inactive mode or invalid state.  Ignoring it (%d %d %d)",
	                 cprContext.switchingVmode, cprContext.currentVmode, cprContext.currentRefClkRate);
    return;
  }

  HalCprLogInterruptStatus();

  currVmode = cprContext.currentVmode;
  currVmodeBoundsAndOffsetCfgPtr = cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[currVmode];

  // Get the offset in mV.  Results are always multiples of pmic steps. 
  offset = CprGetOffsetRecommendation();

  if (offset < (int32)(-cprContext.targetCfg->voltageStepSize))
  {
    // Limit the downward changes to lowering one step at a time. 
    offset = (-cprContext.targetCfg->voltageStepSize);
  } 
  else if (offset > (int32)(cprContext.targetCfg->voltageStepSize))
  {
    // Limit the upward changes to increasing one step at a time. 
    offset = cprContext.targetCfg->voltageStepSize;
  }

  // Calculate what the new voltage would be with the change. 
  newVoltage = cprContext.measurements[currVmode].currentVoltage + offset;

  if (newVoltage == currVmodeBoundsAndOffsetCfgPtr->voltageFloor)    				// --- Arriving at the floor
  {
    // Disable down interrupts if we're at the floor voltage.
    CprEnableDownRecommendation(FALSE);

    // Allow up interrupts in case we previously disabled them
    CprEnableUpRecommendation(TRUE);

    ULOG_RT_PRINTF_2(cprContext.ulogHandle, "Arrived at %d floor for mode %d", newVoltage, currVmode);
  }
  else if (newVoltage == currVmodeBoundsAndOffsetCfgPtr->voltageCeiling)     // --- Arriving at the ceiling
  {
    // Disable up interrupts if we're at the ceiling.
    CprEnableUpRecommendation(FALSE);

    // Allow down interrupts in case we previously disabled them
    CprEnableDownRecommendation(TRUE);

    ULOG_RT_PRINTF_2(cprContext.ulogHandle, "Arrived at %d ceiling for mode %d", newVoltage, currVmode);
  }
  else if (newVoltage < currVmodeBoundsAndOffsetCfgPtr->voltageFloor)        // --- Below the floor
  {
    
    CprEnableDownRecommendation(FALSE);

    // Allow up interrupts in case we previously disabled them
    CprEnableUpRecommendation(TRUE);

    ULOG_RT_PRINTF_3(cprContext.ulogHandle, "Calculated %d voltage for mode %d.  Voltage will be limited to %d",
                     newVoltage, currVmode, currVmodeBoundsAndOffsetCfgPtr->voltageFloor);

    // Limit ourselves to the floor
    newVoltage = currVmodeBoundsAndOffsetCfgPtr->voltageFloor;

    offset = newVoltage - cprContext.measurements[currVmode].currentVoltage;
  }
  else if (newVoltage > currVmodeBoundsAndOffsetCfgPtr->voltageCeiling)           // --- Above the ceiling
  {

    ULOG_RT_PRINTF_3(cprContext.ulogHandle, "Calculated %d voltage for mode %d.  Voltage will be limited to %d",
                     newVoltage, currVmode, currVmodeBoundsAndOffsetCfgPtr->voltageCeiling);


    // Cap off the voltage at the ceiling
    newVoltage = currVmodeBoundsAndOffsetCfgPtr->voltageCeiling;

    // Disable up interrupts
    CprEnableUpRecommendation(FALSE);
    // Allow down interrupts in case we previously disabled them
    CprEnableDownRecommendation(TRUE);

  }
  else                                                 // --- Between the ceiling and floor.
  {
    // Allow UP/DOWN interrupts in case we previously disabled them
    CprEnableDownRecommendation(TRUE);
    CprEnableUpRecommendation(TRUE);
  }


  // If there's ever a case where the offset was 0 and the newVoltage is the same as the currentVoltage we should nack.
  if (newVoltage == cprContext.measurements[currVmode].currentVoltage){
    ULOG_RT_PRINTF_2(cprContext.ulogHandle,
                     "CPR No voltage change (CurMode: %d) (Voltage: %u)",
                     currVmode, cprContext.measurements[currVmode].currentVoltage);
    ack = FALSE;
  } else {
    //we are requesting a voltage move so we intend to ack.
    ack = TRUE;
  }


  // Always inform clkrgm so that it can tell us if the CPU sensors need to be
  // disabled (due to LDO usage).
  if (CprSwitchVoltage(newVoltage) == FALSE)
  {
    //if the voltage change was not done we should not ack. 
    ack = FALSE;
  }

  ULOG_RT_PRINTF_4(cprContext.ulogHandle,
                   "CPR (CurMode: %d) (ISR: 0) (Voltage: %u) (Offset: %d) (Ack: %d)",
                   currVmode, cprContext.measurements[currVmode].currentVoltage,
                   offset, ack);

  HalCprClearAllInterrupts();
  HalCprStartNextMeasurement(ack);

  CPR_MUTEX_RELEASE();
}


/**
 * <!-- CprEnabledModeAcquireSystemResources -->
 *
 * @brief Acquire clocks, mutex etc. for the CPR driver.
 *
 * CPR Hardware initialization should NOT be done here - that should be in
 * HalCprInit().
 */
inline static void CprEnabledModeAcquireSystemResources( void )
{
  ClockIdType clockId;
  DalDeviceHandle *interruptHandle = NULL;

  //ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CprEnabledModeAcquireSystemResources");  
#ifndef WINSIM
  // Attach to VCS
  CORE_DAL_VERIFY( DAL_StringDeviceAttach("VCS", &cprContext.vcsHandle));
  CORE_DAL_VERIFY( DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &cprContext.clkHandle));

  // Attach to Clock Driver
  CORE_DAL_VERIFY( DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &cprContext.clkHandle));

  // Enable the rbcpr bus clock.
  DalClock_GetClockId(cprContext.clkHandle, "clk_bus_rbcpr", &clockId);
  DalClock_EnableClock(cprContext.clkHandle, clockId);

  // Enable the rbcpr reference clock.
  DalClock_GetClockId(cprContext.clkHandle, "clk_rbcpr_ref", &clockId);
  DalClock_EnableClock(cprContext.clkHandle, clockId);

  // Take the clock out of reset state.
  DalClock_ResetClock(cprContext.clkHandle, clockId, CLOCK_RESET_DEASSERT);
#endif
  // Create a mutex
  CORE_DAL_VERIFY( DALSYS_SyncCreate( DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT,
                                      &cprContext.mutexHandle,
                                      &cprContext.syncObj));
#ifndef WINSIM
  // Get an interrupt controller handle
  CORE_DAL_VERIFY(DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &interruptHandle));

  // Register the RBCPR interrupt.
  CORE_DAL_VERIFY(DalInterruptController_RegisterISR(interruptHandle, 
                                           cprContext.targetCfg->irq,
                                           (DALIRQ)CprIsr, 
                                           (DALISRCtx)0, 
                                           DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER));
#endif
}


/**
 * <!-- CprGoToLowPowerMode -->
 *
 * @brief Enable/disable CPR HW when entering/exiting low power modes.
 * 
 * @param inLowPowerMode: TRUE if entering low power mode; FALSE otherwise.
 */
void CprGoToLowPowerMode( boolean inLowPowerMode )
{
  ClockIdType clockId;

  //ULOG_RT_PRINTF_3(cprContext.ulogHandle, "CprGoToLowPowerMode (inLowPowerMode %d) (IRQ_STATUS %x) (firstUse %d)", inLowPowerMode, HWIO_IN(MSS_RBIF_IRQ_STATUS),cprContext.clearedAfterFirstUse); 
  
  if (!inLowPowerMode)
  {
    if (cprContext.clkHandle != NULL) {
	  // turn on the clocks after waking up
      DalClock_GetClockId(cprContext.clkHandle, "clk_bus_rbcpr", &clockId);
      DalClock_EnableClock(cprContext.clkHandle, clockId);

      DalClock_GetClockId(cprContext.clkHandle, "clk_rbcpr_ref", &clockId);
      DalClock_EnableClock(cprContext.clkHandle, clockId);
    }
  }
  
  // If the CPR driver hasn't begun being used don't change the enable state.
  if (cprContext.clearedAfterFirstUse==FALSE){
    //disable or re-enable CPR
    CprEnable(!inLowPowerMode);
  }
  
  if (inLowPowerMode)
  {
    if (cprContext.clkHandle != NULL)
    {  
	  // turn off the clocks before going to sleep
      DalClock_GetClockId(cprContext.clkHandle, "clk_bus_rbcpr", &clockId);
      DalClock_DisableClock(cprContext.clkHandle, clockId);
	  
      DalClock_GetClockId(cprContext.clkHandle, "clk_rbcpr_ref", &clockId);
      DalClock_DisableClock(cprContext.clkHandle, clockId);
    }
  }
}


/**
 * <!-- CprEnabledModeMeasureCprNow -->
 *
 * @brief Take a CPR measurement and return the recommended voltage.
 * 
 * @param eRail : Which rail is being used.
 * @param eCorner : SVS, NOM, TURBO etc
 * @param nVoltageuV : MSS rail voltage in microVolts
 *
 * @return The new recommended voltage in the nVoltageuV variable.
 */
static DALResult CprEnabledModeMeasureCprNow( VCSRailType eRail, VCSCornerType eCorner, uint32 *nVoltageuV )
{
  //ULOG_RT_PRINTF_2(cprContext.ulogHandle, "CprEnabledModeMeasureCprNow (eRail : %d) (eCorner : %d)", eRail, eCorner);  
  //TODO: actual implementation to be added later. 
  //return currRailVoltage;
  return DAL_SUCCESS;
}



/*----------------------------------------------------------------------------
 * Public interfaces
 * -------------------------------------------------------------------------*/
/**
 * <!-- CprInitEnabledMode -->
 *
 * @brief Initialize the driver so that CPR HW can be used to manager the voltages.
 */
void CprInitEnabledMode( CprApiType *pCprApi )
{
  int i;

  //ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CprInitEnabledMode");    

  for(i=0;i<CPR_MAX_VMODE_COUNT;i++){
    cprContext.debugTimesInEachMode[i] = 0;
    cprContext.debugUpIsrInEachMode[i] = 0;
    cprContext.debugDownIsrInEachMode[i] = 0;
  }
  CprEnabledModeAcquireSystemResources();

  //begin with ints enabled. 
  cprContext.downIntEnabled = TRUE;
  cprContext.upIntEnabled = TRUE;

  HalCprInit(cprContext.halTargetCfg, cprContext.ulogHandle, cprContext.ulogStaticHandle);

  // Set this "clearedAfterFirstUse" flag so that the first time the postSwitch
  // function is used to turn on CPR on we don't mind that preswitch hasn't been
  // called yet it and log an error message. 
  cprContext.clearedAfterFirstUse = TRUE;  

  ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CPR Ready - Registering CPR Switch Mode Callbacks");
  
  // Tell the clock driver that we're ready to operate.
  pCprApi->Disable = CprEnabledModePreSwitchCallback;
  pCprApi->Enable = CprEnabledModePostSwitchCallback;
  pCprApi->GetRailVoltageRecommendation = CprEnabledModeSetVoltageMode;
  pCprApi->MeasureNow = CprEnabledModeMeasureCprNow;

  cprContext.lpFunc = CprGoToLowPowerMode;
}

