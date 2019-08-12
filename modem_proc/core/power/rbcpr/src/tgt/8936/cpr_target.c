/*============================================================================
@file cpr_target.c

Initial Target-specific CPR configuration data for 8962 based on 8974.

Copyright © 2013 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/src/tgt/8936/cpr_target.c#1 $
=============================================================================*/
#include "HALhwio.h"
#include "HAL_cpr.h"
#include "HALcpr_hwio.h"
#include "cpr_efuse.h"
#include "cpr_internal.h"
#include "CoreVerify.h"
#include "DDIChipInfo.h"

/** Database of fuse locations and fields we want to read from. */
static CprEfuseMasterDatabaseType cprEfuseMasterDb =
{
  .redundancySel = { HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, 0xe0000000, 29 },
  .primaryDb = 
  {
    .disCpr  = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x40000000, 30},
    .vmodeCfg =
    {
       // CPR_VMODE_LOW_MINUS - sub-SVS mode for eLDO usage. Will not used by
       // MSS rail (and hence not used by CPR).
       // Only added to align with clkrgm enums

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
      // CPR_VMODE_LOW_MINUS - sub-SVS mode for eLDO usage. Will not used by
      // MSS rail (and hence not used by CPR).
      // Only added to align with clkrgm enums

      [CPR_VMODE_LOW_MINUS] =
      {
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_ADDR, 0xf8000000, 27},
      },
      [CPR_VMODE_LOW] =
      {
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_ADDR, 0xf8000000, 27},
      },
      [CPR_VMODE_LOW_PLUS] =
      {
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_ADDR, 0x1f, 0},
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


/** V1 Hardware version specific config needed by the HAL. */
static HalCprTargetConfigType halCprTargetCfgV1 =
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
    // Quotients built with 75mV margin.


    // CPR_VMODE_LOW_MINUS - sub-SVS mode for eLDO usage. Will not used by
    // MSS rail (and hence not used by CPR).
    // Only added to align with clkrgm enums

    [CPR_VMODE_LOW] =
    {
      .stepQuotient          = 40,
      .idleClocks            = 15,
      .oscCfg[0].target      = 608,
      .oscCfg[1].target      = 576,
      .oscCfg[2].target      = 467,
      .oscCfg[3].target      = 443,
      .oscCfg[4].target      = 343,
      .oscCfg[5].target      = 378,
      .oscCfg[6].target      = 250,
      .oscCfg[7].target      = 291,
    },
    [CPR_VMODE_LOW_PLUS] =
    {
      .stepQuotient          = 40,
      .idleClocks            = 15,
      .oscCfg[0].target      = 797,
      .oscCfg[1].target      = 766,
      .oscCfg[2].target      = 616,
      .oscCfg[3].target      = 597,
      .oscCfg[4].target      = 473,
      .oscCfg[5].target      = 516,
      .oscCfg[6].target      = 366,
      .oscCfg[7].target      = 414,
    },
    [CPR_VMODE_NOMINAL] =
    {
      .stepQuotient          = 40,
      .idleClocks            = 15,
      .oscCfg[0].target      = 797,
      .oscCfg[1].target      = 766,
      .oscCfg[2].target      = 616,
      .oscCfg[3].target      = 597,
      .oscCfg[4].target      = 473,
      .oscCfg[5].target      = 516,
      .oscCfg[6].target      = 366,
      .oscCfg[7].target      = 414,
    },
    [CPR_VMODE_TURBO] =
    {
      .stepQuotient          = 40,
      .idleClocks            = 15,
      .oscCfg[0].target      = 994,
      .oscCfg[1].target      = 959,
      .oscCfg[2].target      = 770,
      .oscCfg[3].target      = 754,
      .oscCfg[4].target      = 612,
      .oscCfg[5].target      = 657,
      .oscCfg[6].target      = 498,
      .oscCfg[7].target      = 547,
    },
    [CPR_VMODE_SUPER_TURBO] =
    {
      .stepQuotient          = 40,
      .idleClocks            = 15,
      .oscCfg[0].target      = 1155,
      .oscCfg[1].target      = 1113,
      .oscCfg[2].target      = 899,
      .oscCfg[3].target      = 882,
      .oscCfg[4].target      = 732,
      .oscCfg[5].target      = 773,
      .oscCfg[6].target      = 618,
      .oscCfg[7].target      = 661,
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
void CprGetTargetConfiguration( CprContextType *cprContext )
{
  // Some items in cprTargetCfg should be read in from efuses.
  cprContext->cprDisabledByEfuse = CprEfuseGetData(&cprEfuseMasterDb, &cprTargetCfg.vmodeCfg[0]);

  cprContext->targetCfg = &cprTargetCfg;

  cprContext->halTargetCfg = &halCprTargetCfgV1;

  // Disable CPR
  cprContext->cprFeatureDisabled = 1;

}

