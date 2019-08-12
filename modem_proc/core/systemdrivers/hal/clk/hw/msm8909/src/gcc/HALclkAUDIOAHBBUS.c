/*
==============================================================================

FILE:         HALclkAUDIOAHBBUS.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   AUDIO AHB BUS clocks.

   List of clock domains:
   -HAL_clk_mGCCULTAUDIOAHBFABRICClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/msm8909/src/gcc/HALclkAUDIOAHBBUS.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
09/26/2013            Auto-generated.

==============================================================================
            Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include <HALhwio.h>

#include "HALclkInternal.h"
#include "HALclkTest.h"
#include "HALclkGeneric.h"
#include "HALclkHWIO.h"


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Prototypes
** ==========================================================================*/
void HAL_clk_BusConfig
(
  HAL_clk_ClockDescType  *pmClockDesc,
  HAL_clk_ClockConfigType eConfig
);

/* ============================================================================
**    Externs
** ==========================================================================*/

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl;
extern HAL_clk_ClockDomainControlType  HAL_clk_mAUDIOAHBClockDomainControl;


/* ============================================================================
**    Data
** ==========================================================================*/

#define HAL_CLK_MEM_CORE_ON_BMSK               0x00004000
#define HAL_CLK_MEM_PERIPH_ON_BMSK             0x00002000
#define HAL_CLK_HW_CTL_ON_BMSK                 0x00000002


/*
 * HAL_clk_mAUDIOAHBBusClockControl
 *
 * Functions for controlling common clock functions.
 */
HAL_clk_ClockControlType HAL_clk_mAUDIOAHBBusClockControl =
{
  /* .Enable           = */ HAL_clk_GenericEnable,
  /* .Disable          = */ HAL_clk_GenericDisable,
  /* .IsEnabled        = */ HAL_clk_GenericIsEnabled,
  /* .IsOn             = */ HAL_clk_GenericIsOn,
  /* .Reset            = */ HAL_clk_GenericReset,
  /* .IsReset          = */ HAL_clk_GenericIsReset,
  /* .Config           = */ HAL_clk_BusConfig,
  /* .DetectConfig     = */ NULL,
  /* .ConfigDivider    = */ HAL_clk_GenericConfigDivider,
  /* .DetectDivider    = */ HAL_clk_GenericDetectDivider,
  /* .ConfigFootswitch = */ HAL_clk_GenericConfigFootswitch,
};

/*                           
 *  HAL_clk_mULTAUDIOAHBFABRICClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mULTAUDIOAHBFABRICClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_ultaudio_ahbfabric_efabric_spdm_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CLK
  },
  {
    /* .szClockName      = */ "gcc_ultaudio_ahbfabric_ixfabric_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR), HWIO_OFFS(GCC_AUDIO_CORE_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CLK
  },
  {
    /* .szClockName      = */ "gcc_ultaudio_ahbfabric_ixfabric_lpm_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mAUDIOAHBBusClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CLK
  },
};


/*
 * HAL_clk_mGCCULTAUDIOAHBFABRICClkDomain
 *
 * ULTAUDIO AHBFABRIC clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCULTAUDIOAHBFABRICClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mULTAUDIOAHBFABRICClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mULTAUDIOAHBFABRICClkDomainClks)/sizeof(HAL_clk_mULTAUDIOAHBFABRICClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mAUDIOAHBClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};

/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/

void HAL_clk_BusConfig
(
  HAL_clk_ClockDescType  *pmClockDesc,
  HAL_clk_ClockConfigType eConfig
)
{
  uint32 nAddr = 0;
  uint32 nValue = 0;

  if(NULL != pmClockDesc)
  {
    nAddr = pmClockDesc->mRegisters.nBranchAddr;

    if(nAddr != 0)
    {
      nValue = inpdw(nAddr);

      switch(eConfig)
      {
        case HAL_CLK_CONFIG_CORE_MEM_ON:
          nValue |= HAL_CLK_MEM_CORE_ON_BMSK;
          outpdw(nAddr, nValue);
          break;

        case HAL_CLK_CONFIG_PERIPH_MEM_ON:
          nValue |= HAL_CLK_MEM_PERIPH_ON_BMSK;
          outpdw(nAddr, nValue);
          break;

        case HAL_CLK_CONFIG_CORE_MEM_OFF:
          nValue &= ~HAL_CLK_MEM_CORE_ON_BMSK;
          outpdw(nAddr, nValue);
          break;

        case HAL_CLK_CONFIG_PERIPH_MEM_OFF:
          nValue &= ~HAL_CLK_MEM_PERIPH_ON_BMSK;
          outpdw(nAddr, nValue);
          break;

        case HAL_CLK_CONFIG_HW_CTL_ON:
          nValue |= HAL_CLK_HW_CTL_ON_BMSK;
          outpdw(nAddr, nValue);
          break;

        case HAL_CLK_CONFIG_HW_CTL_OFF:
          nValue &= ~HAL_CLK_HW_CTL_ON_BMSK;
          outpdw(nAddr, nValue);
          break;

        default:
          break;
      }
    }
  }

} /* HAL_clk_BusConfig */



