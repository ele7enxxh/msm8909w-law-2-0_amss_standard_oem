/*
==============================================================================

FILE:         HALclkMain.c

DESCRIPTION:
  This file contains the main platform initialization code for the clock
  HAL on the modem subsystem (MSS) processor.

==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/msm8909/src/HALclkMain.c#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
04/12      ll      Branched from 8974.
09/04/12   ll      Added SR / SR2 field to PLL Context
10/19/11   vs      Created.

==============================================================================
            Copyright (c) 2011 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include "HALclkInternal.h"
#include "HALclkGeneric.h"
#include "HALclkGenericPLL.h"
#include "HALhwio.h"
#include "HALclkHWIO.h"


/* ============================================================================
**    Prototypes
** ==========================================================================*/

void HAL_clk_PlatformInitSources(void);


/* ============================================================================
**    Externs
** ==========================================================================*/

extern void HAL_clk_PlatformInitGCCMain(void);
extern void HAL_clk_PlatformInitMSSMain(void);

extern void HAL_clk_InstallMPLL1DivSources(void);


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * HAL_clk_aInitFuncs
 *
 * Declare array of module initialization functions.
 */
static HAL_clk_InitFuncType HAL_clk_afInitFuncs[] =
{
  /*
   * Sources
   */
  HAL_clk_PlatformInitSources,
  
  /*
   * GCC
   */
  HAL_clk_PlatformInitGCCMain,

  /*
   * MSS
   */
  HAL_clk_PlatformInitMSSMain,

  NULL
};


/*
 * Declare the base pointers for HWIO access.
 */
uint32 HAL_clk_nHWIOBaseTop;
uint32 HAL_clk_nHWIOBaseMSS;
uint32 HAL_clk_nHWIOBaseSecurity;


/*
 * HAL_clk_aHWIOBases
 *
 * Declare array of HWIO bases in use on this platform.
 */
static HAL_clk_HWIOBaseType HAL_clk_aHWIOBases[] =
{
  { CLK_CTL_BASE_PHYS,          CLK_CTL_BASE_SIZE,          &HAL_clk_nHWIOBaseTop      },
  { MSS_TOP_BASE_PHYS,          MSS_TOP_BASE_SIZE,          &HAL_clk_nHWIOBaseMSS      },
  { SECURITY_CONTROL_BASE_PHYS, SECURITY_CONTROL_BASE_SIZE, &HAL_clk_nHWIOBaseSecurity }, 
  { 0, 0, NULL }
};


/*
 * HAL_clk_Platform;
 * Platform data.
 */
HAL_clk_PlatformType HAL_clk_Platform =
{
  HAL_clk_afInitFuncs,
  HAL_clk_aHWIOBases
};


/*
 * GPLL contexts
 */
static HAL_clk_PLLContextType HAL_clk_aPLLContextGPLL[] =
{
  {
    HWIO_OFFS(GCC_GPLL0_MODE),
    HAL_CLK_FMSK(GCC_MSS_Q6_GPLL_ENA_VOTE, GPLL0),
    HAL_CLK_PLL_STROMER
  },
  {
    HWIO_OFFS(GCC_GPLL1_MODE),
    HAL_CLK_FMSK(GCC_MSS_Q6_GPLL_ENA_VOTE, GPLL1),
    HAL_CLK_PLL_SR
  }
};


/*
 * Modem PLL contexts - no voting support.
 */
static HAL_clk_PLLContextType HAL_clk_aPLLContextMPLL[] =
{
  {
    HWIO_OFFS(MSS_MPLL1_MODE),
    {0},
    HAL_CLK_PLL_SR2
  },
  {
    HWIO_OFFS(MSS_QDSP6SS_PLL_MODE),
    {0},
    HAL_CLK_PLL_STROMER
  }
};


/* ============================================================================
**    Functions
** ==========================================================================*/


/* ===========================================================================
**  HAL_clk_PlatformInitSources
**
** ======================================================================== */

void HAL_clk_PlatformInitSources (void)
{
  /*
   * Install PLL handlers.
   */
  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_GPLL0, &HAL_clk_aPLLContextGPLL[0], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_GPLL1, &HAL_clk_aPLLContextGPLL[1], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_MPLL1, &HAL_clk_aPLLContextMPLL[0], MSS_TOP_BASE);

  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_MPLL2, &HAL_clk_aPLLContextMPLL[1], MSS_TOP_BASE);

  HAL_clk_InstallMPLL1DivSources();

} /* END HAL_clk_PlatformInitSources */


/* ===========================================================================
**  HAL_clk_Save
**
** ======================================================================== */

void HAL_clk_Save (void)
{
  /*
   * Nothing to save.
   */

} /* END HAL_clk_Save */


/* ===========================================================================
**  HAL_clk_Restore
**
** ======================================================================== */

void HAL_clk_Restore (void)
{
  /*
   * Nothing to restore.
   */
  
} /* END HAL_clk_Restore */



