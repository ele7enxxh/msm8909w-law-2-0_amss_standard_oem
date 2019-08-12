/*
==============================================================================

FILE:         HALclkBLSP1QUP4.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   BLSP1 QUP4 clocks.

   List of clock domains:
   -HAL_clk_mGCCBLSP1QUP4I2CClkDomain
   -HAL_clk_mGCCBLSP1QUP4SPIClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/msm8909/src/gcc/HALclkBLSP1QUP4.c#1 $

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
 *  HAL_clk_mBLSP1QUP4I2CClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBLSP1QUP4I2CClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_blsp1_qup4_i2c_apps_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP1_QUP4_I2C_APPS_CBCR), HWIO_OFFS(GCC_BLSP1_QUP4_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP1_QUP4_I2C_APPS_CLK
  },
};


/*
 * HAL_clk_mGCCBLSP1QUP4I2CClkDomain
 *
 * BLSP1 QUP4 I2C clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP4I2CClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBLSP1QUP4I2CClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBLSP1QUP4I2CClkDomainClks)/sizeof(HAL_clk_mBLSP1QUP4I2CClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


                                    
/*                           
 *  HAL_clk_mBLSP1QUP4SPIClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBLSP1QUP4SPIClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_blsp1_qup4_spi_apps_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP1_QUP4_SPI_APPS_CBCR), HWIO_OFFS(GCC_BLSP1_QUP4_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP1_QUP4_SPI_APPS_CLK
  },
};


/*
 * HAL_clk_mGCCBLSP1QUP4SPIClkDomain
 *
 * BLSP1 QUP4 SPI clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP4SPIClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBLSP1QUP4SPIClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBLSP1QUP4SPIClkDomainClks)/sizeof(HAL_clk_mBLSP1QUP4SPIClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


