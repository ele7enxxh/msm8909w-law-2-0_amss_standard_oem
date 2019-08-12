/*
===========================================================================

FILE:         HALmpmintPlatform.c

DESCRIPTION:  
  This is the platform hardware abstraction layer implementation for the
  MPM interrupt controller block.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/source/MPM_V4/HALmpmPlatform.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
08/06/09   ajf     Created new platform based on MPM_V2 platform.

===========================================================================
           Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
               QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "BSPmpm.h"
#include "cortex-m3.h"
#include "../common/HALmpmInternal.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "HALmpm.h"
#include "HALhwio.h"
#include "mpm2_hwio.h"
#include "qtimer_hwio.h"
#include "rpm_hwio.h"
#include "swevent.h"
#include "tcsr_hwio.h"
#include "rob.h"
#include "QDSSLite.h"
#include "gpio_debug.h"
#include "sleep_lpr.h"

#define INT_STATUS_REGISTER_COUNT 2

#define ARR_SIZE(x) (sizeof(x) / sizeof (x[0]))

#ifndef QDSS_QDSS_BASE
// this is added for 8952 as the base address is not auto generated in msmhwiobase.h
// once that is corrected, this can be removed
#define QDSS_QDSS_BASE                               0x60800000
#endif
/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

/*
 * Consolidated data
 */

static uint32_t wakeup_ints[INT_STATUS_REGISTER_COUNT];
static bool HAL_mpm_freeze_ios_enabled = TRUE;
#ifdef TSENSE_SAMPLING_1SEC
static uint32 tsens_measure_period = 0;
#endif

HAL_mpm_PlatformType mpmPlatform = 
{
  // Placeholder for BSP config data.
  NULL,

  // Placeholders for some custom functions (unused).
  NULL,
  NULL,
  NULL
};

static uint32_t rpm_mpm_registers[50] __attribute__((used));

// SPMI Delay table from MPM2 HPG
// 4bit   # XO cycles     Delay in usec
// 0000   0               0
// 0001   192             10
// 0010   480             25
// 0011   960             50
// 0100   1920            100
// 0101   2880            150
// 0110   3840            200
// 0111   4800            250
// 1000   5760            300
// 1001   6720            350
// 1010   7680            400
// 1011   9600            500
// 1100   11520           600
// 1101   13440           700
// 1110   16320           850
// 1111   19200           1000
static uint16 delayTable[16]      = {0, 10, 25, 50, 100, 150, 200, 250, 300, 350, 400, 500, 600, 700, 850, 1000};
static uint32 sleep_bail_before_halt_counter = 0;

/* ===========================================================================  
**  HAL_mpm_SetCXOWarmup
**
** ======================================================================== */
void HAL_mpm_SetCXOWarmup (uint16_t warmupSclks)
{
  HWIO_OUTF(MPM2_MPM_CXO_POWER_RAMPUP_TIME, CXO_WARMUP_TIME, warmupSclks);
}

/* ===========================================================================  
**  HAL_mpm_CfgFreezeIOs
**
** ======================================================================== */
void HAL_mpm_CfgFreezeIOs(bool enable)
{
  HAL_mpm_freeze_ios_enabled = enable;
}

/* ===========================================================================  
**  HAL_mpm_CfgVoltageRailSleep
**
** ======================================================================== */
void HAL_mpm_CfgVoltageRailSleep(uint32_t offMxMicrovolts, uint32_t onMxMicrovolts, 
                                 uint32_t offCxMicrovolts, uint32_t onCxMicrovolts)
{
  uint32_t mask, val = 0;
 
  HWIO_OUT(MPM2_MPM_SPMI_CDIV_CNTRL, 0x28); //program SMPI clock div value


  // First, the part of the register we need to set.
  //mask  = HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_CORE_RESET);
  mask  = 0;
      
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, VDDCX_MIN_EN); //enable CX minimization
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, VDDMX_MIN_EN); //enable MX minimization
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, SMEM_EN); //enable split rail mem
  if(!(HAL_mpm_freeze_ios_enabled && mpmPlatform.mpmCfg->mpmCfg.ioCfg.freezeIOs))
  {
    mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_FREEZE);
    mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_FREEZE_IO_M);
  }
  if(!mpmPlatform.mpmCfg->mpmCfg.ioCfg.clampIOs)
  {
    mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_CLAMPS);
    mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_CLAMP_MEM);
  }

  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_HW_RESTORED);
  val = mask;

  // Always add these to the mask--that way they are cleared if the BSP
  // checks above indicate that they shouldn't be set.
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_FREEZE);
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_CLAMPS);
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_FREEZE_IO_M);
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_CLAMP_MEM);

  HWIO_OUTM(MPM2_MPM_LOW_POWER_CFG, mask, val);

  /* program SPMI commands */
  HAL_mpm_CfgVoltageCommands(offMxMicrovolts, onMxMicrovolts, offCxMicrovolts, onCxMicrovolts);
}

/* ===========================================================================  
**  HAL_mpm_CfgXOSleep
**
** ======================================================================== */
void HAL_mpm_CfgCXOSleep (void)
{

  uint32_t mask = 0;
  uint32_t val = 0;

  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, ENABLE_CXOPAD_GATING);
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, CXO_SD_EN);
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_FREEZE_IO_M);
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_FREEZE);
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_CLAMP_MEM);
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_CLAMPS);
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_CORE_RESET);

  val = mask;

  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_TRST_N);

  HWIO_OUTM(MPM2_MPM_LOW_POWER_CFG, mask, val);

  HWIO_OUT(MPM2_MPM_INT_WKUP_CLK_SEL_1, 0x0);
  HWIO_OUT(MPM2_MPM_INT_WKUP_CLK_SEL_2, 0x0);
}

/* ===========================================================================  
**  HAL_mpm_ClearSleepModes
**
** ======================================================================== */
void HAL_mpm_ClearSleepModes (void)
{
  uint32_t mask = 0;

  // Issue HARDWARE RESTORED command to allow unclamp / unfreeze.
  HWIO_OUT(MPM2_MPM_HARDWARE_RESTORED, 1);

  // Build up a mask of everything we want to clear (omitting EBI1 registers  
  //   as thats only triggered by DDR collapse/restore)
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, SMEM_EN); 
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_FREEZE_IO_M); 
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_CLAMP_MEM); 
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_HW_RESTORED); 
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, ENABLE_CXOPAD_GATING); 
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_TRST_N); 
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_CORE_RESET); 
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_CLAMPS); 
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DISABLE_FREEZE); 
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, DEBUG_ON); 
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, VDDMX_MIN_EN); 
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, VDDMX_PC_EN); 
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, VDDCX_MIN_EN); 
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, VDDCX_PC_EN); 
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, PXO_SD_EN); 
  mask |= HWIO_FMSK(MPM2_MPM_LOW_POWER_CFG, CXO_SD_EN); 

  // Disable all power saving bits.
  HWIO_OUTM( MPM2_MPM_LOW_POWER_CFG, mask, 0 );

  // Additionally, make sure that next time the MPM is used we don't
  // auto-reactivate CXO unless we meant to.
  HWIO_OUT(MPM2_MPM_INT_WKUP_CLK_SEL_1, 0);
  HWIO_OUT(MPM2_MPM_INT_WKUP_CLK_SEL_2, 0);
}

/* ===========================================================================  
**  HAL_mpm_EnableCXO
**
** ======================================================================== */
void HAL_mpm_EnableCXO (void)
{
  uint32_t mask;
  mask  = HWIO_FMSK(MPM2_MPM_CXO_CTRL, CXO_DIS);
  mask |= HWIO_FMSK(MPM2_MPM_CXO_CTRL, CXOPAD_DIS);
  HWIO_OUTM(MPM2_MPM_CXO_CTRL, mask, 0);
}

/* ===========================================================================  
**  HAL_mpm_DebugThroughPC
**
** ======================================================================== */
void HAL_mpm_DebugThroughPC( bool32 enable )
{
  uint32_t bmsk = HWIO_FMSK( MPM2_MPM_LOW_POWER_CFG, DEBUG_ON ) | HWIO_FMSK( MPM2_MPM_LOW_POWER_CFG, DISABLE_TRST_N );
  uint32_t val = 0;

  if ( enable ) 
  {
    val = bmsk;
  }

  HWIO_OUTM( MPM2_MPM_LOW_POWER_CFG, bmsk, val );
}


/* ===========================================================================  
**  HAL_mpm_ConfigDebugBus
**
** ======================================================================== */
void HAL_mpm_ConfigDebugBus( bool32 enable )
{
  uint32_t bmsk = HWIO_FMSK( MPM2_MPM_DEBUG_BUS_EN, DEBUG_EN );
  uint32_t val = 0;

  (mpmPlatform.mpmCfg)->mpmCfg.dbgBusEnable = enable;

  if ( enable ) 
  {
    /* Both bits cannot be enabled at the same time, so set it 
     * up to write a 0x0 to the other bit. */
    bmsk |= HWIO_FMSK( MPM2_MPM_DEBUG_BUS_EN, DEBUG_EN_1 );
    val = HWIO_FMSK( MPM2_MPM_DEBUG_BUS_EN, DEBUG_EN );
  }

  HWIO_OUTM( MPM2_MPM_DEBUG_BUS_EN, bmsk, val );
}

/* ===========================================================================  
**  HAL_mpm_ConfigDebugBus_1
**
** ======================================================================== */
void HAL_mpm_ConfigDebugBus_1( bool32 enable )
{
  uint32_t bmsk = HWIO_FMSK( MPM2_MPM_DEBUG_BUS_EN, DEBUG_EN_1 );
  uint32_t val = 0;

  (mpmPlatform.mpmCfg)->mpmCfg.dbgBusEnable_1 = enable;

  if ( enable ) 
  {
    /* Both bits cannot be enabled at the same time, so set it 
     * up to write a 0x0 to the other bit. */
    bmsk |= HWIO_FMSK( MPM2_MPM_DEBUG_BUS_EN, DEBUG_EN );
    val = HWIO_FMSK( MPM2_MPM_DEBUG_BUS_EN, DEBUG_EN_1 );
  }

  HWIO_OUTM( MPM2_MPM_DEBUG_BUS_EN, bmsk, val );
}

/* ===========================================================================  
**  HAL_mpm_PlatformInitEarly
**
** ======================================================================== */
void HAL_mpm_PlatformInitEarly (HAL_mpm_PlatformType *pmPlatform)
{
  uint32 tcsr_ctrl_state;
  //The following 32bit register is split into 16 bit fields but the fields were swapped
  //workaround to intialize the register correctly.
  HWIO_OUT(MPM2_MPM_CNTR_INCR_DATA, 0x24a0000);

  //enable the MPM counter
  HWIO_OUTF(MPM2_MPM_CONTROL_CNTCR, EN, 1);

  //enable access to the CNTP_* and CNTPCT registers
  HWIO_OUTI(MPM2_QTMR_AC_CNTACR_n, 0, 33);
  HWIO_OUTF(MPM2_QTMR_AC_CNTSR, NSN, 1);

  //enable bus and proc clock halt on wfi
  // RJ: temporarily disable BUS_CLK_HALT so that Apps can read sleep stats
  // HWIO_OUTF(RPM_WFI_CONFIG, BUS_CLK_HALT, 1);
  HWIO_OUTF(RPM_WFI_CONFIG, PROC_CLK_HALT, 1);

  //enable internal LDO001
  tcsr_ctrl_state = HWIO_INF(TCSR_LDO_UPDATE_STATE_CTRL, LDO_UPDATE_STATE);
  HWIO_OUTF(TCSR_LDO_UPDATE_STATE_CTRL, LDO_UPDATE_STATE, 1);
  HWIO_OUTF(TCSR_LDO_BGC_CONFIG, LDO_BGC, 1);
  
#ifndef MPM_INTERNAL_VREF
  // use external vref
  HWIO_OUTF(TCSR_LDO_VREF_CTRL, LDO_VREF_SEL_OVR, 1);
  HWIO_OUTF(TCSR_LDO_VREF_CTRL, LDO_VREF_SEL_SW, 0);
#endif //MPM_INTERNAL_VREF

  HWIO_OUTF(TCSR_LDO_UPDATE_STATE_CTRL, LDO_UPDATE_STATE, tcsr_ctrl_state);

  wakeup_ints[0] = 0;
  wakeup_ints[1] = 0;

} /* END HAL_mpm_PlatformInitEarly */

uint64_t mpm_wakeup_time = 0;

/* ===========================================================================
**  HAL_mpm_SetWakeUpTime
**
** ======================================================================== */
void HAL_mpm_SetWakeUpTime( uint64_t sclkDuration )
{
  uint64_t wakeup_time, temp_time;
  uint32_t lo, hi;

  /* Figure out what time we need to wake up at. */
  HAL_mpm_GetCurrentSclkCount(&wakeup_time);
  temp_time = wakeup_time;
  wakeup_time += sclkDuration;

  //check for wraparound
  if(temp_time > wakeup_time)
    wakeup_time = 0xFFFFFFFFFFFFFF;

  mpm_wakeup_time = wakeup_time;
  lo = (uint32)wakeup_time;
  hi = wakeup_time >> 32;

  SWEVENT(SLEEP_MPM_WAKEUP_TIME, lo, hi);
  
  HWIO_OUT(MPM2_QTMR_V1_CNTP_CVAL_LO, lo);
  HWIO_OUT(MPM2_QTMR_V1_CNTP_CVAL_HI, hi);

  // Make sure the timer is ticking.
  HWIO_OUTF(MPM2_QTMR_V1_CNTP_CTL, EN, 1);
}

/* ===========================================================================
**  HAL_mpm_CfgEbi1SwCtl
**
** ======================================================================== */
void HAL_mpm_CfgEbi1SwCtl(bool enable)
{

  if (enable && mpmPlatform.mpmCfg->mpmCfg.ioCfg.swEbi1CtlEnable)
  {
    HWIO_OUTF(MPM2_MPM_LOW_POWER_CFG, SW_EBI1_CTL_ENABLE, 1);
    HWIO_OUTF(MPM2_MPM_LOW_POWER_CFG, SW_EBI1_CTL_VALUE, 1);
  }
  else if ((!enable) && mpmPlatform.mpmCfg->mpmCfg.ioCfg.swEbi1CtlEnable)
  {
    HWIO_OUTF (MPM2_MPM_LOW_POWER_CFG, SW_EBI1_CTL_ENABLE, 1);
    HWIO_OUTF (MPM2_MPM_LOW_POWER_CFG, SW_EBI1_CTL_VALUE, 0);
  }
}

/* ===========================================================================
**  HAL_mpm_CfgWbootEbiIOCntl
**
** ======================================================================== */
void HAL_mpm_CfgWbootEbiIOCntl( bool32 wbootFreezeEBI1, bool32 wbootFreezeEBI2 )
{
  uint32_t bmsk = HWIO_FMSK(MPM2_MPM_WARM_BOOT_CFG, ENABLE_EBI1) |
                HWIO_FMSK(MPM2_MPM_WARM_BOOT_CFG, ENABLE_EBI2);

  uint32_t val  = 0;

  if (!wbootFreezeEBI1) 
  {
    val |= HWIO_FMSK(MPM2_MPM_WARM_BOOT_CFG, ENABLE_EBI1);
  }

  if (!wbootFreezeEBI2) 
  {
    val |= HWIO_FMSK(MPM2_MPM_WARM_BOOT_CFG, ENABLE_EBI2);
  }

  HWIO_OUTM(MPM2_MPM_WARM_BOOT_CFG, bmsk, val);
}


/* ===========================================================================
**  HAL_mpm_PenDebounceCtl
**
** ======================================================================== */
void HAL_mpm_PenDebounceCtl( bool32 enable )
{
  uint32_t val = enable ? 1 : 0;

  HWIO_OUTF(MPM2_MPM_PEN_DEBOUNCE_CTL, ENABLE, val);
}

/* ===========================================================================
**  HAL_mpm_ConfigPenDebounceDelay
**
** ======================================================================== */
void HAL_mpm_ConfigPenDebounceDelay( HAL_mpm_PenDebounceDelayType delay )
{
  uint32_t val = delay;

  HWIO_OUTF(MPM2_MPM_PEN_DEBOUNCE_CTL, TIME, val);
}

/* ===========================================================================
**  HAL_mpm_GetCurrentSclkCount
**
** ======================================================================== */
void HAL_mpm_GetCurrentSclkCount( uint64_t *pTickCount)
{
  /* Time Tick count */
  //uint32_t pTickCountHigh;
  //uint32_t pTickCountLow;

  uint32_t                   ticklo, tickhi, tickhi_count;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Reading the counter value once may not return an accurate value if the
     counter is in the processing of counting to the next value, and several
     bits are changing.  Instead, the counter is repeatedly read until a
     consistant value is read. */


  

  do
  {
    tickhi = HWIO_IN(MPM2_QTMR_V1_CNTPCT_HI);
    __schedule_barrier();
 
    ticklo = HWIO_IN(MPM2_QTMR_V1_CNTPCT_LO);
    __schedule_barrier();

    tickhi_count = HWIO_IN(MPM2_QTMR_V1_CNTPCT_HI);
    __schedule_barrier();

  } while (tickhi_count != tickhi);

  /* The counter has returned the same value twice in a row, and hence must
     be stable. */

  //pTickCountHigh = tickhi_count;
  //pTickCountLow = ticklow;
 
  //convert to a uint64
  //*pTickCount = pTickCountHigh;
  //*pTickCount = (*pTickCount << 32);
  //*pTickCount |= pTickCountLow;

  *pTickCount = (((uint64_t)tickhi) << 32) | ticklo;
}

uint32_t HAL_mpm_ReadFSMState(void)
{
  uint32_t old_state, state = HWIO_INF(MPM2_MPM_LOW_POWER_STATUS, FSM_STATE);

  do
  {
    old_state = state;
    state     = HWIO_INF(MPM2_MPM_LOW_POWER_STATUS, FSM_STATE);
  } while(state != old_state);

  return state;
}

__asm static void halt(void)
{
    ; Lock interrupts via primask instead of basepri during sleep, or else we never wake up.
    mrs r0, basepri
    cpsid i
    mov r1, #0
    msr basepri, r1

    ; Do the halt, using ISB to clear the M3 store buffer
    dsb
    isb
    wfi

    ; Revert interrupt lock to basepri-based.
    msr basepri, r0
    cpsie i
    bx r14
}

void HAL_mpm_GetWakeupInts(uint32 mask_index, uint32 *status)
{
  if (mask_index >= INT_STATUS_REGISTER_COUNT)
    return;

  *status = wakeup_ints[mask_index];
}

extern boolean Clock_IsQDSSOn(void);
extern bool sleep_is_any_interrupt_pending(void);

void HAL_mpm_SWDone(uint64_t sleep_duration)
{
  static const    uint32_t MPM_STATE_IDLE = 0;
  static const    uint32_t MPM_STATE_UNFREEZE = 5;
  static const    uint32_t MPM_STATE_BOOT = 9;
  uint32_t                 sleep_val;
  uint32_t                 qdss_was_locked = 0;
  uint32_t                 en1, en2;

  uint32_t                 disable_hw_restored = 0;
  boolean                  is_qdss_on = Clock_IsQDSSOn();
  uint32_t                 mpm_state = MPM_STATE_IDLE;
  volatile unsigned int *addr = (volatile unsigned int *)MPM2_MPM_BASE;
  unsigned int i = 0;
  
  en1 = HWIO_IN(MPM2_MPM_INT_EN_1);
  en2 = HWIO_IN(MPM2_MPM_INT_EN_2);

  SWEVENT(SLEEP_MPM_INTS_ENABLED, en1, en2);

  if (is_qdss_on)
  {
     QDSSPreXOShutdown();
  }

  //switch counter to sleep clock. 
  HWIO_OUTF(MPM2_MPM_CONTROL_CNTCR, FCREQ, 1);

  /* configure the MPM to halt the chip on wfi */
  sleep_val  = 1 << HWIO_SHFT(RPM_WFI_CONFIG, PROC_CLK_HALT);
  
  sleep_val |= 1 << HWIO_SHFT(RPM_WFI_CONFIG, CHIP_SLEEP_UPON_WFI);
  sleep_val |= 1 << HWIO_SHFT(RPM_WFI_CONFIG, BUS_CLK_HALT);
  // RJ: temporarily disable BUS_CLK_HALT so that Apps can read sleep stats from Apps


  HWIO_OUT(RPM_WFI_CONFIG, sleep_val);
  
  if (is_qdss_on)
  {
    /* share the qdss hardware nicely: check if the csr is locked before we unlock it */
    qdss_was_locked = HWIO_INF(QDSS_CS_QDSSCSR_LOCKSTATUS, ACCESS);
  }

  if(qdss_was_locked)
  {
    HWIO_OUT(QDSS_CS_QDSSCSR_LOCKACCESS, 0xc5acce55);

    /* Bit[0] of this register masks an internal debugger signal that prevents the
     * M3 from "really" sleeping when there's a debugger attached, regardless of
     * the run/break state of the processor. */
    HWIO_OUTM(QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE, 1, 1); // bit[0] controls the RPM signal
  }

  // capture all MPM registers
  for (i = 0; i < ARR_SIZE(rpm_mpm_registers); i++, addr++)
  {
    rpm_mpm_registers[i] = in_dword(addr);
  }
  
  // mark the event
  rob_mark_event (ROB_EVENT_MPM_PRE_SW_DONE);

  HAL_mpm_SetWakeUpTime(sleep_duration);
  
  // check if we need to resynchronize our state with the MPM
  disable_hw_restored = HWIO_INF(MPM2_MPM_LOW_POWER_CFG, DISABLE_HW_RESTORED); 

  gpio_test_high(GPIO_DEEP_SLEEP, GPIO_SLEEP_TRANSITION);
  /* invoke sleep */
  if (!sleep_is_any_interrupt_pending())
  {
    halt();
  }
  else
  {
    //log that we bailed early
    SWEVENT(SLEEP_NO_DEEP_SLEEP, HAL_MPM_SLEEP_MODE_COUNT, SLEEP_INTR_PENDING_BEFORE_HALT);
    sleep_bail_before_halt_counter++;
  }
  gpio_test_low(GPIO_DEEP_SLEEP, GPIO_ACTIVE_TRANSITION);

  /* if the qdss hardware was not already locked by someone else, clean up after ourselves */
  if(qdss_was_locked)
  {
    /* clear the mask of the debug signal so that our halt behavior is good */
    HWIO_OUTM(QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE, 1, 0); // bit[0] controls the RPM signal

    HWIO_OUT(QDSS_CS_QDSSCSR_LOCKACCESS, 0);
  }

  //get sleep exit time. Switch back to XO clock
  HWIO_OUTF(MPM2_MPM_CONTROL_CNTCR, FCREQ, 0);

  /* configure the MPM to halt the chip on wfi */
  sleep_val  = 1 << HWIO_SHFT(RPM_WFI_CONFIG, PROC_CLK_HALT);
  HWIO_OUT(RPM_WFI_CONFIG, sleep_val);

  mpm_state = HAL_mpm_ReadFSMState();
  
  if (!disable_hw_restored)
  {
    //resynch with the mpm state machine
    if (MPM_STATE_BOOT == mpm_state)
    {
      HWIO_OUT(MPM2_MPM_HARDWARE_RESTORED, 1);
    }
  }

  // Secondary short circuit path in the RPM core
  if (MPM_STATE_IDLE != mpm_state)
  {
    // Must be in unfreeze 
    while(MPM_STATE_UNFREEZE != HAL_mpm_ReadFSMState());
  }
  
  // Disable the qtimer -- we're awake and don't need it running.
  HWIO_OUTF(MPM2_QTMR_V1_CNTP_CTL, EN, 0);

  // mark the event
  rob_mark_event (ROB_EVENT_MPM_POST_SW_DONE);
  
  en1 = HWIO_IN(MPM2_MPM_INT_EN_1);
  en2 = HWIO_IN(MPM2_MPM_INT_EN_2);

  // log wakeup interrupts
  wakeup_ints[0] = HWIO_IN(MPM2_MPM_INT_STATUS_1);
  wakeup_ints[1] = HWIO_IN(MPM2_MPM_INT_STATUS_2);

  if (is_qdss_on) {
    QDSSPostXOShutdown(); //this restores qdss timestamps
  }

  SWEVENT(SLEEP_MPM_INTS, (wakeup_ints[0] & en1), (wakeup_ints[1] & en2));

  // Clear all interrupts.
  HWIO_OUT(MPM2_MPM_INT_CLEAR_1, 0xFFFFFFFF);
  HWIO_OUT(MPM2_MPM_INT_CLEAR_2, 0xFFFFFFFF);

  // Disable all interrupts.
  HWIO_OUT(MPM2_MPM_INT_EN_1, 0);
  HWIO_OUT(MPM2_MPM_INT_EN_2, 0);

  // Disabling the interrupts should move us from UNFREEZE to IDLE
  while(MPM_STATE_IDLE != HAL_mpm_ReadFSMState());

  HWIO_OUT(MPM2_MPM_INT_EN_1, en1);
  HWIO_OUT(MPM2_MPM_INT_EN_2, en2);
}

/* ===========================================================================
**  HAL_mpm_true_sleep_xtal_set
**
** ======================================================================== */
void HAL_mpm_true_sleep_xtal_set ( void )
{
    /* To support a true 32kHz sleep clock:                             **
    **  Clock = 32.768 kHz -> 19.2MHz/Clock = 585.9375                  **
    **  Lower 16 bits of MPM_CNTR_INCR_DATA are for fractional count    **
    **  Set register to 0x0249F000 as this is a fixed point 16.16 num    */
    HWIO_OUT(MPM2_MPM_CNTR_INCR_DATA, 0x0249F000);
}
#ifdef TSENSE_SAMPLING_1SEC
/* =========================================​==================================
**  HAL_deep_sleep_enter_tsense
**
** =========================================​=============================== */
void HAL_deep_sleep_enter_tsense(void)
{
    // to improve sleep current in 8936, the sampling rate of TSENSE is
    // reduced to 1 second. This will be reverted back to the the original  
    // value of 62.5 ms during wake
    tsens_measure_period = HWIO_INF(MPM2_TSENS_CTRL, MEASURE_PERIOD);
    HWIO_OUTF(MPM2_TSENS_CTRL, MEASURE_PERIOD, 0x5);
    HWIO_OUTF(MPM2_TSENS_CTRL, TSENS_SW_RST , 0x1);
    HWIO_OUTF(MPM2_TSENS_CTRL, TSENS_SW_RST , 0x0);
    HWIO_OUTF(MPM2_TSENS_CTRL, TSENS_EN  , 0x1);
}
 
/* =========================================​==================================
**  HAL_deep_sleep_exit_tsense
**
** =========================================​=============================== */
void HAL_deep_sleep_exit_tsense(void)
{
    // restore the sampling rate of TSENSE
    HWIO_OUTF(MPM2_TSENS_CTRL, MEASURE_PERIOD, tsens_measure_period);
    HWIO_OUTF(MPM2_TSENS_CTRL, TSENS_SW_RST , 0x1);
    HWIO_OUTF(MPM2_TSENS_CTRL, TSENS_SW_RST , 0x0);
    HWIO_OUTF(MPM2_TSENS_CTRL, TSENS_EN  , 0x1);
}
#endif
uint16 HAL_mpm_GetDelayFromMicrovolts(uint32 offMicrovolts, uint32 onMicrovolts)
{
  uint32 desiredDelay = 0;
  unsigned int max = 16;
  unsigned int min = 0;
  unsigned int i = 0;
  unsigned int mid = max / 2;

  // PM8026 step FTS2 and LDO @1.5mV/usec
  desiredDelay = (uint32)CEILING_DIV((onMicrovolts - offMicrovolts), mpmPlatform.mpmCfg->vregCfg.vregStepSize);
  // Add 50uS of additional delay, per pmic systems
  desiredDelay += 50;

  //max of 1000us delay
  if (desiredDelay > 1000)
    return 15;

  for (i = 0; i < 4; i++)
  {
    if (delayTable[mid] == desiredDelay)
      break;
    if (delayTable[mid] < desiredDelay)
    {
      if (i == 3)
      {
        mid++;
        break;
      }
      min = mid;
    }
    else
    {
      if (i == 3)
      {
        break;
      }
      max = mid;
    }
    mid = ((max + min) / 2);
  }
  return mid;
}

uint32 HAL_mpm_GetDeepSleepEnterLatency(void)
{
  uint32 off_cfg = HWIO_ADDR(MPM2_MPM_PMIC_VDD_OFF_CFG_1);
  uint32 shift   = HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_1, DELAY);
  uint32 delay   = 0;
  uint32 num_cmds, val;
  uint32 i;

  // Get # of VDD_OFF_CFG commands
  num_cmds = HWIO_INF(MPM2_MPM_SPMI_CMD_CFG, NUM_CMD_OFF) + 1;
  for (i = 0; i < num_cmds; i++)
  {
    val = *((uint32 *)(off_cfg + (i << 2)));
    delay += delayTable[(val >> shift)];
  }

  // 3uS per SPMI command
  delay += (3 * num_cmds);

  // Convert us into timetick
  delay = (delay * 19200) / 1000;

  // Add MPM state machine delay: 3 enter states * 1sclk a piece 3*586 = 1758 cxo ticks
  delay += 1758;

  return delay;
}


uint32 HAL_mpm_GetDeepSleepExitLatency(void)
{
  uint32 on_cfg  = HWIO_ADDR(MPM2_MPM_PMIC_VDD_ON_CFG_1);
  uint32 shift   = HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_1, DELAY);
  uint32 delay   = 0;
  uint32 num_cmds, val;
  uint32 i;

  // Get # of VDD_ON_CFG commands
  num_cmds = HWIO_INF(MPM2_MPM_SPMI_CMD_CFG, NUM_CMD_ON) + 1;
  for (i = 0; i < num_cmds; i++)
  {
    val = *((uint32 *)(on_cfg + (i << 2)));
    delay += delayTable[(val >> shift)];
  }

  // 3uS per SPMI command
  delay += (3 * num_cmds);

  // Convert us into timetick
  delay = (delay * 19200) / 1000;

  // Add CXO delay (in sclks so convert this to xo ticks as well)
  delay += ((mpmPlatform.mpmCfg->mpmCfg.wakeupDelays.cxoDelay + HAL_mpm_GetTCXOLatency()) * 586);

  // Add MPM state machine delay: 7 states * 1sclk a piece 7*586 = 4102 cxo ticks
  delay += 4102;

  // Add POWER_RAMPUP_TIME register
  delay += (HWIO_INF(MPM2_MPM_CXO_POWER_RAMPUP_TIME, POWER_RAMPUP_TIME) * 586);

  // Add any target specific backoff 
  delay += mpmPlatform.mpmCfg->mpmCfg.wakeupDelays.deepSleepExitDelay; 

  return delay;
}

