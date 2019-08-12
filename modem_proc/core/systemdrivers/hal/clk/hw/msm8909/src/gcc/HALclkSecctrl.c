/*
==============================================================================

FILE:         HALclkSecctrl.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   Sec ctrl clocks.

   List of clock domains:
   -HAL_clk_mGCCACCClkDomain
   -HAL_clk_mGCCSECCTRLClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/msm8909/src/gcc/HALclkSecctrl.c#1 $

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


/* ============================================================================
**    Externs
** ==========================================================================*/

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl;


/* ============================================================================
**    Data
** ==========================================================================*/

                                    
/*                           
 *  HAL_clk_mACCClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mACCClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_sec_ctrl_acc_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SEC_CTRL_ACC_CBCR), HWIO_OFFS(GCC_SEC_CTRL_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SEC_CTRL_ACC_CLK
  },
};


/*
 * HAL_clk_mGCCACCClkDomain
 *
 * ACC clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCACCClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_ACC_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mACCClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mACCClkDomainClks)/sizeof(HAL_clk_mACCClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};



/*                           
 *  HAL_clk_mSECCTRLClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSECCTRLClkDomainClks[] =
{
{
    /* .szClockName      = */ "gcc_sec_ctrl_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SEC_CTRL_CBCR), HWIO_OFFS(GCC_SEC_CTRL_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SEC_CTRL_CLK
  },
  {
    /* .szClockName      = */ "gcc_sec_ctrl_sense_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SEC_CTRL_SENSE_CBCR), HWIO_OFFS(GCC_SEC_CTRL_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SEC_CTRL_SENSE_CLK
  },
};


/*
 * HAL_clk_mGCCSECCTRLClkDomain
 *
 * SEC CTRL clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCSECCTRLClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_SEC_CTRL_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mSECCTRLClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSECCTRLClkDomainClks)/sizeof(HAL_clk_mSECCTRLClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


