/*============================================================================
@file HalCpr.c

Hardware Abstraction Layer for RBCPR HW.

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/src/hal/v1/HAL_cpr.c#1 $
============================================================================*/
#include "HALhwio.h"
#include "HAL_cpr.h"
#include "HALcpr_hwio.h"
#include "cpr_internal.h"


#define CPR_CONCAT(sym, num) sym##num
#define CPR_HWIO_INM(reg, field) (HWIO_INM(reg, HWIO_FMSK(reg, field))>>HWIO_SHFT(reg, field))
#define CPR_HWIO_OUTM(reg, field, val) (HWIO_OUTM(reg, HWIO_FMSK(reg, field), val<<HWIO_SHFT(reg, field)))


/** CPR version specific HW config that is independent of which target we're on. */
extern HalCprConfigType halCprConfiguration;

/** Target specific info */
static HalCprTargetConfigType * halCprTgtCfg = NULL;

/** Pointer to Ulog handle */
static ULogHandle               halUlogHandle = NULL;


/**
 * <!-- HalCprInit --> 
 * 
 * @brief Initialalize CPR masks, bypass, thresholds, mode, limit, timer, 
 * and enable the up interrupt.
 *
 * @param tgtCfg : Pointer to the configuration table..
 * @param ulogHandle : The ULog handle used to log VDD_MSS CPR activities. 
 */
void HalCprInit( HalCprTargetConfigType * tgtCfg,
                 ULogHandle               ulogHandle )
{
  static boolean initDone = FALSE;
  uint32 index;

  if (initDone)
  {
    return;
  }
  initDone = TRUE;

  halCprTgtCfg = tgtCfg;
  halUlogHandle = ulogHandle;

  // Configure Sensor MASK and BYPASS settings.
  for (index = 0; index < halCprConfiguration.numSensors/32; index++)
  {
    uint32 *reg;

    reg = (uint32 *)(HWIO_MSS_RBCPR_SENSOR_MASK0_ADDR + (index*4));
    *reg = halCprTgtCfg->staticCfg.sensorMask[index];

    reg = (uint32 *)(HWIO_MSS_RBCPR_SENSOR_BYPASS0_ADDR + (index*4));
    *reg = halCprTgtCfg->staticCfg.sensorBypass[index];
  }

  HalCprSetErrStepThresholds(halCprTgtCfg->staticCfg.CTL__DN_THRESHOLD,
                             halCprTgtCfg->staticCfg.CTL__UP_THRESHOLD);

  CPR_HWIO_OUTM(MSS_RBIF_SW_VLEVEL, SW_VLEVEL,
                halCprTgtCfg->staticCfg.SW_VLEVEL__SW_VLEVEL);

  // Sensors to count in staggered mode -
  // may need to make this a target config item
  CPR_HWIO_OUTM(MSS_RBCPR_CTL, COUNT_MODE, 0x1);

  // Auto-generate the ACK pulse if no voltage update is needed
  CPR_HWIO_OUTM(MSS_RBCPR_CTL, SW_AUTO_CONT_ACK_EN, 0x1);

  // We're always going to use the SW mode.
  CPR_HWIO_OUTM(MSS_RBCPR_CTL, HW_TO_PMIC_EN, 0x0);

  // Set ceiling to MAX
  CPR_HWIO_OUTM(MSS_RBIF_LIMIT, CEILING,
                ((HWIO_MSS_RBIF_LIMIT_CEILING_BMSK) >>
                 (HWIO_MSS_RBIF_LIMIT_CEILING_SHFT)));

  // Set floor to lowest allowed value
  CPR_HWIO_OUTM(MSS_RBIF_LIMIT, FLOOR, 0);

  CPR_HWIO_OUTM(MSS_RBIF_TIMER_ADJUST, CONSECUTIVE_DN,
                halCprTgtCfg->staticCfg.TIMER_ADJUST__CONSECUTIVE_DN);

  CPR_HWIO_OUTM(MSS_RBCPR_CTL, TIMER_EN, 1);

  HalCprEnableInterrupt(HAL_CPR_INTERRUPT_UP, TRUE);
}


/**
 * <!-- HalCprSetErrStepThresholds -->
 * 
 * @brief Set threshold for allowing the HW to trigger voltage-correction interrupts.
 * 
 * @param dnThresh : Number of error steps (PMIC voltage steps) by which the
 *                   voltage can drop before an interrupt is raised.
 * @param upThresh : Number of error steps (PMIC voltage steps) by which the
 *                   voltage must be increased.
 */
void HalCprSetErrStepThresholds( uint32 dnThresh,
                                 uint32 upThresh )
{
  CPR_HWIO_OUTM(MSS_RBCPR_CTL, DN_THRESHOLD, dnThresh);
  CPR_HWIO_OUTM(MSS_RBCPR_CTL, UP_THRESHOLD, upThresh);
}


/**
 * <!-- HalCprConfigInterrupts -->
 *
 * @brief Enable/disable CPR interrupts.
 * 
 * @param enable : If TRUE, will enable all interrupts selected in
 * HalCprConfiguration.interrupts; will disabled all interrupts if FALSE.
 */
void HalCprConfigInterrupts( boolean enable )
{
  uint32 bitmask = 0;
  
  // Based on the HAL config, generate the bitmask to set.
  if (enable)
  {
    HalCprInterruptEnumType index;
    for (index = 0; index < HAL_CPR_INTERRUPT_COUNT; index++)
    {
      bitmask |= enable << halCprConfiguration.interrupts[index];
    }
  }
  
  // Set the mask to allow those interrupts
  HWIO_OUT(MSS_RBIF_IRQ_EN_0, bitmask);

  // We're never going to use these interrupt lines.
  HWIO_OUT(MSS_RBIF_IRQ_EN_1, 0);
  HWIO_OUT(MSS_RBIF_IRQ_EN_2, 0);
}


/**
 * <!-- HalCprEnable -->
 *
 * @brief Enable/disable the auto-loop timer HW used for starting CPR measurements.
 * 
 * @param enable : TRUE to enable; FALSE otherwise.
 */
void HalCprEnable( boolean enable )
{
  CPR_HWIO_OUTM(MSS_RBCPR_CTL, LOOP_EN, enable);
}


/**
 * <!-- HalCprConfigOscillator -->
 *
 * @brief Write the target and gcnt values to a MSS_RBCPR_GCNT_TARGET register.
 *
 * From the RBCPR SWI document: 
 * There are eight different delay chains, or oscillators, inside each sensor.
 * Each delay chain has a programmable gcnt (gate count) and target. The gcnt
 * is the number of refclk cycles that the delay chain is allowed to oscillate.
 * The target is the expected number of oscillation cycles for that gcnt 
 * period, for typical silicon under typical conditions.
 *
 * @param oscIndex : Which of the 8 RBCPR_GCNT_TARGET registers to write to.
 * @param gcnt :  gcnt calculated from the reference clock
 * @param target : target from the configuration table
 */
void HalCprConfigOscillator( uint32 oscIndex,
                             uint32 gcnt,
                             uint32 target )
{
  uint32 val; 
  uint32 *reg_address;
  
  /* For the shift amount we use TARGET0, GCNT0 register */
  val = (gcnt << HWIO_SHFT(MSS_RBCPR_GCNT_TARGET0, GCNT0)) | target;

  /* Find the offset of the register from the first register */
  reg_address = (uint32 *)(HWIO_MSS_RBCPR_GCNT_TARGET0_ADDR + (oscIndex*4));
  
  /* Now write the value */
  *reg_address = val;
}


/**
 * <!-- HalCprSetupVoltageMode -->
 *
 * @brief Setup the CPR oscillators to operate the specified voltage mode.
 * 
 * @param newMode : voltage mode to switch to.
 * @param refClkRate : Reference clock rate in kHz
 * @param delay : Number of millisecs to delay the next measurement
 */
void HalCprSetupVoltageMode( CprVModeEnumType newMode,
                             uint32           refClkRate,
                             uint32           delay )
{
  // We will use ALL the oscillators and the HW will base its results on the
  // slowest oscillator.

  HalCprOscillatorCfgType * oscCfg = &halCprTgtCfg->vmodeCfg[newMode].oscCfg[0];
  int oscIndex;
  uint32 gcnt = refClkRate / 1000;

  for (oscIndex = 0; oscIndex < CPR_OSCILLATOR_COUNT; oscIndex++)
  {
    HalCprConfigOscillator(oscIndex, gcnt, oscCfg[oscIndex].target);
  }

  HWIO_OUT(MSS_RBCPR_TIMER_INTERVAL, (refClkRate * delay));

  CPR_HWIO_OUTM(MSS_RBCPR_STEP_QUOT, STEP_QUOT,
                halCprTgtCfg->vmodeCfg[newMode].stepQuotient);

  CPR_HWIO_OUTM(MSS_RBCPR_STEP_QUOT, IDLE_CLOCKS,
                halCprTgtCfg->vmodeCfg[newMode].idleClocks);

  // Disable auto NACK of the down interrupt
  CPR_HWIO_OUTM(MSS_RBCPR_CTL, SW_AUTO_CONT_NACK_DN_EN, 0);
}


/**
 * <!-- HalCprBypassCpuBlock -->
 *
 * @brief Bypass the sensors on the CPU block. This is necessary when the Q6 is
 * being powered at different voltage than VDD MSS.
 * 
 * @param bypass : TRUE to bypass the CPU sensors; FALSE to no bypass.
 */
void HalCprBypassCpuBlock( boolean bypass )
{
  uint32 index;

  // Configure Sensor MASK and BYPASS settings.
  for (index = 0; index < halCprConfiguration.numSensors/32; index++)
  {
    uint32 * reg_address;
    uint32   val;

    // Find the offset of the register from the first register
    reg_address = (uint32 *)(HWIO_MSS_RBCPR_SENSOR_BYPASS0_ADDR + (index*4));

    val = *reg_address;

    if (bypass)
    {
      val |= halCprTgtCfg->staticCfg.sensorsOnCpuBlock[index];
    }
    else
    {
      val &= ~(halCprTgtCfg->staticCfg.sensorsOnCpuBlock[index]);
    }
    *reg_address = val;
  }
}


/**
 * <!-- HalCprIsBusy -->
 *
 * @brief Read the BUSY bit of the RBCPR_RESULT_0 register. 
 */
boolean HalCprIsBusy( void )
{
  return (boolean) (CPR_HWIO_INM(MSS_RBCPR_RESULT_0, BUSY));
}


/**
 * <!-- HalCprGetOffsetRecommendation -->
 *
 * @brief Calculate the recommended steps and log the recommendation info.
 */
int32 HalCprGetOffsetRecommendation( void )
{
  uint32 error      = CPR_HWIO_INM(MSS_RBCPR_RESULT_0, ERROR);
  uint32 errorSteps = CPR_HWIO_INM(MSS_RBCPR_RESULT_0, ERROR_STEPS);
  uint32 stepUp     = CPR_HWIO_INM(MSS_RBCPR_RESULT_0, STEP_UP);
  uint32 stepDown   = CPR_HWIO_INM(MSS_RBCPR_RESULT_0, STEP_DN);

  /* Now compute the offset in number of PMIC voltage steps */
  int32  offsetSteps = (stepUp + (stepDown * (-1))) * errorSteps;

  ULogFront_RealTimePrintf(
    halUlogHandle,
    5,
    "HW_REC (Err: %u) (ErrSteps: %u) (StepUp: %u) (StepDn: %u) (Offset: %d)",
    error, errorSteps, stepUp, stepDown, offsetSteps);

  return offsetSteps;
}


/**
 * <!-- HalCprEnableInterrupt -->
 *
 * @brief Enable/disable the specified interrupt.
 * 
 * @param interrupt : Interrupt to change
 * @param enable : TRUE to enable; FALSE otherwise.
 */
void HalCprEnableInterrupt( HalCprInterruptEnumType interrupt,
                            boolean                 enable )
{
  uint32 val = HWIO_IN(MSS_RBIF_IRQ_EN_0);

  if (enable)
  {
    val |= (1 << interrupt);
  }
  else
  {
    val &= ~(1 << interrupt);
  }
  HWIO_OUT(MSS_RBIF_IRQ_EN_0, val);
}


/**
 * <!-- HalCprClearAllInterrupts -->
 *
 * @brief Clear the interrupt status of all interrupts.
 */
void HalCprClearAllInterrupts(void)
{
  HWIO_OUT(MSS_RBIF_IRQ_CLEAR, HWIO_MSS_RBIF_IRQ_CLEAR_RMSK);
}


/**
 * <!-- HalCprEnableDownRecommendation -->
 *
 * Enables/disable the down recommendation from the HW. Disables 
 * the Down interrupt and also enable the auto nack for any down 
 * recommendations.
 * 
 * @param enable : Enabled if TRUE; FALSE otherwise
 */
void HalCprEnableDownRecommendation(boolean enable)
{
  HalCprEnableInterrupt(HAL_CPR_INTERRUPT_DOWN, enable);

  // When down recommendation is not desired, we need to turn on the AUTO
  // NACK for any down recommendations - otherwise the HW will stuck in this
  // state and will not make any further recommendations since no ACK/NACK was
  // received.
  CPR_HWIO_OUTM(MSS_RBCPR_CTL, SW_AUTO_CONT_NACK_DN_EN, !enable);
}


/**
 * <!-- HalCprStartNextMeasurement -->
 *
 * @brief Provide feedback to HW on whether the recommendation was accepted/rejected
 * and allow the next measurement to start.
 * 
 * @param recomAccepted : TRUE if accepted; FALSE otherwise.
 */
void HalCprStartNextMeasurement(boolean recomAccepted)
{
  if (recomAccepted)
  {
    /* Any value will send a pulse to the block */
    HWIO_OUT(MSS_RBIF_CONT_ACK_CMD, 0x1);
  }
  else
  {
    /* Any value will send a pulse to the block */
    HWIO_OUT(MSS_RBIF_CONT_NACK_CMD, 0x1);
  }
}


/**
 * <!-- HalCprLogInterruptStatus -->
 *
 * @brief Log which interrupts are enabled and the interrupt status.
 */
void HalCprLogInterruptStatus(void)
{
  ULogFront_RealTimePrintf(
    halUlogHandle,
    2,
    "CPR (ISR: 1) (IntsEnabled: 0x%x) (IntsStatus: 0x%x)",
    HWIO_IN(MSS_RBIF_IRQ_EN_0),
    HWIO_IN(MSS_RBIF_IRQ_STATUS));
}

