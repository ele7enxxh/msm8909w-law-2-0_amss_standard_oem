/*
==============================================================================

FILE:         HALclkUIM.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   UIM clocks.

   List of clock domains:
   -HAL_clk_mMSSCXOUIM0ClkDomain
   -HAL_clk_mMSSCXOUIM1ClkDomain
   -HAL_clk_mMSSCXOUIM2ClkDomain
   -HAL_clk_mMSSCXOUIMClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/msm8909/src/mss/HALclkUIM.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
10/01/2013            Auto-generated.

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

extern HAL_clk_ClockDomainControlType  HAL_clk_mMSSClockDomainControl;


/* ============================================================================
**    Data
** ==========================================================================*/

                                    
/*                           
 *  HAL_clk_mCXOUIM0ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mCXOUIM0ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_xo_uim0",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_XO_UIM0_CBCR), HWIO_OFFS(MSS_UIM0_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_XO_UIM0
  },
};


/*
 * HAL_clk_mMSSCXOUIM0ClkDomain
 *
 * CXO UIM0 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSCXOUIM0ClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(MSS_UIM0_MND_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mCXOUIM0ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mCXOUIM0ClkDomainClks)/sizeof(HAL_clk_mCXOUIM0ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mMSSClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


                                    
/*                           
 *  HAL_clk_mCXOUIM1ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mCXOUIM1ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_xo_uim1",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_XO_UIM1_CBCR), HWIO_OFFS(MSS_UIM1_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_XO_UIM1
  },
};


/*
 * HAL_clk_mMSSCXOUIM1ClkDomain
 *
 * CXO UIM1 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSCXOUIM1ClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(MSS_UIM1_MND_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mCXOUIM1ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mCXOUIM1ClkDomainClks)/sizeof(HAL_clk_mCXOUIM1ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mMSSClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


                                    
/*                           
 *  HAL_clk_mCXOUIM2ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mCXOUIM2ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_xo_uim2",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_XO_UIM2_CBCR), HWIO_OFFS(MSS_UIM2_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_XO_UIM2
  },
};


/*
 * HAL_clk_mMSSCXOUIM2ClkDomain
 *
 * CXO UIM2 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSCXOUIM2ClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(MSS_UIM2_MND_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mCXOUIM2ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mCXOUIM2ClkDomainClks)/sizeof(HAL_clk_mCXOUIM2ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mMSSClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


                                    
/*                           
 *  HAL_clk_mCXOUIMClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mCXOUIMClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_uim0",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_UIM0_CBCR), HWIO_OFFS(MSS_UIM0_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_UIM0
  },
  {
    /* .szClockName      = */ "clk_uim1",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_UIM1_CBCR), HWIO_OFFS(MSS_UIM1_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_UIM1
  },
  {
    /* .szClockName      = */ "clk_uim2",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_UIM2_CBCR), HWIO_OFFS(MSS_UIM2_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_UIM2
  },
};


/*
 * HAL_clk_mMSSCXOUIMClkDomain
 *
 * CXO UIM clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSCXOUIMClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(MSS_UIM_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mCXOUIMClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mCXOUIMClkDomainClks)/sizeof(HAL_clk_mCXOUIMClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mMSSClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


