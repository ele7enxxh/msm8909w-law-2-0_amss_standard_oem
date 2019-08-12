/*
===========================================================================
  @file ClockSBLCPU.c

  This file provides clock initialization for the Apps SBL.
===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  =========================================================================

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/clock/hw/msm8909w/src/ClockSBLCPU.c#1 $
  $DateTime: 2015/10/01 02:54:06 $
  $Author: pwbldsvc $

  =========================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/
#include "ClockSBL.h"
#include "ClockHWIO.h"
#include "ClockSBLConfig.h"
#include "busywait.h"

/*=========================================================================
      Macro Definitions
==========================================================================*/

/*=========================================================================
     Externs
==========================================================================*/

/*=========================================================================
      Function Prototypes
==========================================================================*/

/*=========================================================================
      Function Definitions
==========================================================================*/

/*=========================================================================
      Data
==========================================================================*/

/* variable that holds the configured apps clock freq */
uint32 clock_apps_speed_khz = 0;

/*=========================================================================
      Function Definitions
=========================================================================*/

/* ============================================================================
**  Function : Clock_SetCPUPerfLevel
** ============================================================================
*/
/**
  Configure Krait CPU to a specific perf level.

  @param eCPUPerfLevel [in] - CPU performance level.

  @return
  TRUE -- CPU was configured to perf level successful.
  FALSE -- Configuration failed.

  @dependencies
  None.

  @sa
  Clock_Init.
*/

boolean Clock_SetCPUPerfLevel(ClockBootPerfLevelType eCPUPerfLevel)
{
  Clock_SBLConfigType *cfg = Clock_SBLConfig();
  const ClockConfigMuxType *CPU_mux, *CXO_cfg;

  if( eCPUPerfLevel >= CLOCK_BOOT_PERF_NUM ) return FALSE;

  CPU_mux = &cfg->CPU_Cfg[eCPUPerfLevel].ApcsMux;

  /* Check to see if the switch is from A7PLL to A7PLL */
  if( CPU_mux->eSource == SRC_A7PLL )
  {
    if(HWIO_INF(APCS_SH_PLL_MODE, OUTCTRL))
    {
      /* Switch away from A7 PLL so that it can be re-tuned */
      CXO_cfg = &cfg->CPU_Cfg[CLOCK_BOOT_PERF_MIN].ApcsMux;
      if( ! Clock_EnableSource( CXO_cfg->eSource )) return FALSE;
      if( ! Clock_ConfigMux(CXO_cfg)) return FALSE;

      /* Disable the A7PLL */
      HWIO_OUTF(APCS_SH_PLL_MODE, OUTCTRL, 0x0);
    }
  }

  /* Now switch */
  if( ! Clock_EnableSource( CPU_mux->eSource )) return FALSE;
  if( ! Clock_ConfigMux(CPU_mux)) return FALSE;

  clock_apps_speed_khz = cfg->CPU_Cfg[eCPUPerfLevel].nFrequency;
  return TRUE;
}

/* ============================================================================
**  Function : Clock_SetL2PerfLevel
** ============================================================================
*/
/*!
    Configure L2 cache to a specific perf level.

    @param eL2PerfLevel   -  [IN] CPU performance level

    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetL2PerfLevel(ClockBootPerfLevelType eL2PerfLevel)
{
  return FALSE;
}

