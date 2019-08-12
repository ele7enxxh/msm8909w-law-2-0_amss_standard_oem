/*============================================================================
@file cpr_target.c

Target-specific CPR configuration data for 8926.

Copyright © 2013 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/src/tgt/8926/cpr_target.c#1 $
=============================================================================*/
#include "HALhwio.h"
#include "HAL_cpr.h"
#include "HALcpr_hwio.h"
#include "cpr_efuse.h"
#include "cpr_internal.h"
#include "CoreVerify.h"

/** Database of fuse locations and fields we want to read from. */
static CprEfuseMasterDatabaseType cprEfuseMasterDb =
{
  .redundancySel = { HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, 0xe0000000, 29 },
  .primaryDb = 
  {
    .disCpr  = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x40000000, 30},
    .vmodeCfg =
    {
      // CPR_VMODE_LOW_MINUS - not used by MSS rail (and hence not used by CPR). Only added to align with clkrgm enums

      [CPR_VMODE_LOW_MINUS] =
      {
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_ADDR, 0xf8, 3},
      },
      [CPR_VMODE_LOW] =
      {
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_ADDR, 0xf8, 3},
      },
      [CPR_VMODE_LOW_PLUS] =
      {
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x3e0, 5},
      },
      [CPR_VMODE_NOMINAL] =
      {
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x3e0, 5},
      },
      [CPR_VMODE_TURBO] =
      {
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x1f, 0},
      },
      [CPR_VMODE_SUPER_TURBO] =
      {
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x1f, 0},
      }
    }
  },
  .redundantDb =
  {
    .disCpr  = {HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_ADDR, 0x2000000, 25},
    .vmodeCfg =
    {
      // CPR_VMODE_LOW_MINUS - not used by MSS rail (and hence not used by CPR). Only added to align with clkrgm enums

      [CPR_VMODE_LOW_MINUS] =
      {
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_ADDR, 0xf8, 3},
      },
      [CPR_VMODE_LOW] =
      {
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_ADDR, 0xf8000000, 27},
      },
      [CPR_VMODE_LOW_PLUS] =
      {
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x3e0, 5},
      },
      [CPR_VMODE_NOMINAL] =
      {
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_ADDR, 0x1f, 0},
      },
      [CPR_VMODE_TURBO] =
      {
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_ADDR, 0xf80000, 19},
      },
      [CPR_VMODE_SUPER_TURBO] =
      {
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_ADDR, 0xf80000, 19},
      }
    }
  }
};

/** Target specific config. */
static CprTargetCfgType cprTargetCfg =
{
  .irq                     = 91,       // From MSS HPG document
  .delayBeforeNextMeas     = 5,        // millisecs
  .ulogBufSize             = 65536,    // Bytes
  .voltageStepSize         = 12500,    // microVolts
};

/** Hardware version specific config needed by the HAL. */
static HalCprTargetConfigType halCprTargetCfg =
{
  .staticCfg =
  {
    .sensorMask[0] = 0,
    .sensorMask[1] = 0,

    .sensorBypass[0] = 0,
    .sensorBypass[1] = 0,

    .CTL__DN_THRESHOLD = 2,
    .CTL__UP_THRESHOLD = 1,

    .SW_VLEVEL__SW_VLEVEL = 0x20,

    .TIMER_ADJUST__CONSECUTIVE_DN = 0x2,

    .sensorsOnCpuBlock[0] = 0x3000,
    .sensorsOnCpuBlock[1] = 0,
  },
  .vmodeCfg =
  {
    // Quotients built with 25mV margin.

    // CPR_VMODE_LOW_MINUS - sub-SVS mode for eLDO usage. Will not used by
    // MSS rail (and hence not used by CPR).
    // Only added to align with clkrgm enums

    [CPR_VMODE_LOW] =
    {
      .stepQuotient          = 40,
      .idleClocks            = 15,
      .oscCfg[0].target      = 530,
      .oscCfg[1].target      = 498,
      .oscCfg[2].target      = 406,
      .oscCfg[3].target      = 379,
      .oscCfg[4].target      = 291,
      .oscCfg[5].target      = 323,
      .oscCfg[6].target      = 206,
      .oscCfg[7].target      = 242,
    },
    [CPR_VMODE_LOW_PLUS] =
    {
      .stepQuotient          = 40,
      .idleClocks            = 15,
      .oscCfg[0].target      = 623,
      .oscCfg[1].target      = 591,
      .oscCfg[2].target      = 479,
      .oscCfg[3].target      = 455,
      .oscCfg[4].target      = 354,
      .oscCfg[5].target      = 390,
      .oscCfg[6].target      = 262,
      .oscCfg[7].target      = 302,
    },
    [CPR_VMODE_NOMINAL] =
    {
      .stepQuotient          = 40,
      .idleClocks            = 15,
      .oscCfg[0].target      = 717,
      .oscCfg[1].target      = 685,
      .oscCfg[2].target      = 553,
      .oscCfg[3].target      = 532,
      .oscCfg[4].target      = 418,
      .oscCfg[5].target      = 458,
      .oscCfg[6].target      = 318,
      .oscCfg[7].target      = 362,
    },
    [CPR_VMODE_TURBO] =
    {
      .stepQuotient          = 40,
      .idleClocks            = 15,
      .oscCfg[0].target      = 856,
      .oscCfg[1].target      = 823,
      .oscCfg[2].target      = 662,
      .oscCfg[3].target      = 643,
      .oscCfg[4].target      = 516,
      .oscCfg[5].target      = 558,
      .oscCfg[6].target      = 410,
      .oscCfg[7].target      = 456,
    },
    [CPR_VMODE_SUPER_TURBO] =
    {
      .stepQuotient          = 40,
      .idleClocks            = 15,
      .oscCfg[0].target      = 1020,
      .oscCfg[1].target      = 983,
      .oscCfg[2].target      = 791,
      .oscCfg[3].target      = 773,
      .oscCfg[4].target      = 635,
      .oscCfg[5].target      = 675,
      .oscCfg[6].target      = 525,
      .oscCfg[7].target      = 568,
    },
  }
};


/*----------------------------------------------------------------------------
 * Public interfaces
 * -------------------------------------------------------------------------*/
/**
 * <!-- CprGetTargetConfiguration -->
 *
 * @brief Get target-specific configuration info, including reading efuses etc.
 *
 * @param cprContext: cprContext->targetCfg and cprContext->halTargetCfg will
 * will be set to non-NULL pointers.
 */
void CprGetTargetConfiguration(CprContextType *cprContext)
{
  // Some items in cprTargetCfg should be read in from efuses.
  cprContext->cprDisabledByEfuse = CprEfuseGetData(&cprEfuseMasterDb,
                                                   &cprTargetCfg.vmodeCfg[0]);
  cprContext->targetCfg = &cprTargetCfg;
  cprContext->halTargetCfg = &halCprTargetCfg;

  //Disabled pending testing and further qualification. 
  cprContext->cprFeatureDisabled = 1;
}

