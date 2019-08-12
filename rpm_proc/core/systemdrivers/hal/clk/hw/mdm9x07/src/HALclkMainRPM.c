/*
==============================================================================

FILE:         HALclkMainRPM.c

DESCRIPTION:
  This file contains the main platform initialization code for the clock
  HAL on the RPM processor on MSM8974.

==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/mdm9x07/src/HALclkMainRPM.c#3 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/20/12   vph      Created.

==============================================================================
            Copyright (c) 2012 Qualcomm Technologies Incorporated.
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

extern void HAL_clk_PlatformInitRPM(void);
extern void HAL_clk_PlatformInitGCCMain(void);


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
   * CPU
   */
  HAL_clk_PlatformInitGCCMain,


  NULL
};


/*
 * Declare the base pointers for HWIO access.
 */
uint32 HAL_clk_nHWIOBaseTop     = CLK_CTL_BASE_PHYS;



/*
 * HAL_clk_aHWIOBases
 *
 * Declare array of HWIO bases in use on this platform.
 */
static HAL_clk_HWIOBaseType HAL_clk_aHWIOBases[] =
{
  { CLK_CTL_BASE_PHYS,  CLK_CTL_BASE_SIZE, &HAL_clk_nHWIOBaseTop },
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
    HAL_CLK_FMSK(GCC_RPM_GPLL_ENA_VOTE, GPLL0),
    HAL_CLK_PLL_STROMER
  },
  {
    HWIO_OFFS(GCC_GPLL1_MODE),
    HAL_CLK_FMSK(GCC_RPM_GPLL_ENA_VOTE, GPLL1),
    HAL_CLK_PLL_SR
  },
  {
    HWIO_OFFS(GCC_GPLL2_MODE),
    HAL_CLK_FMSK(GCC_RPM_GPLL_ENA_VOTE, GPLL2),
    HAL_CLK_PLL_STROMER
  }, 
  {
    HWIO_OFFS(GCC_BIMC_PLL_MODE),
    HAL_CLK_FMSK(GCC_RPM_GPLL_ENA_VOTE, BIMC_PLL),
    HAL_CLK_PLL_STROMER
  },
  
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
    HAL_CLK_SOURCE_GPLL0,   &HAL_clk_aPLLContextGPLL[0], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_GPLL1,   &HAL_clk_aPLLContextGPLL[1], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_GPLL2,   &HAL_clk_aPLLContextGPLL[2], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_BIMCPLL, &HAL_clk_aPLLContextGPLL[3], CLK_CTL_BASE);

} /* END HAL_clk_PlatformInitSources */


/* ===========================================================================
**  HAL_clk_GCCSourceMapToHW - General Clock Control
**
** ======================================================================== */

uint32 HAL_clk_GCCSourceMapToHW
(
  HAL_clk_SourceType eSource
)
{
  /*
   * Determine the source selection value.
   */
  switch (eSource)
  {
      case HAL_CLK_SOURCE_XO:       return 0;
      case HAL_CLK_SOURCE_GPLL0:    return 1;
      case HAL_CLK_SOURCE_BIMCPLL:  return 2;
     
        
      /* RAW Source clock without any PLL */
      case HAL_CLK_SOURCE_RAW0:     return 0;
      case HAL_CLK_SOURCE_RAW1:     return 1;
      case HAL_CLK_SOURCE_RAW2:     return 2;
    default:
      return 0;
  }

} /* END HAL_clk_GCCSourceMapToHW */


/* ===========================================================================
**  HAL_clk_GCCSourceMapFromHW - General Clock Control
**
** ======================================================================== */

HAL_clk_SourceType HAL_clk_GCCSourceMapFromHW
(
  uint32 nSourceSelect
)
{
  /*
   * Determine the source selection value.
   */
  switch (nSourceSelect)
  {
    case 0: return HAL_CLK_SOURCE_XO;
    case 1: return HAL_CLK_SOURCE_GPLL0;
    case 2: return HAL_CLK_SOURCE_BIMCPLL;
    default:
      return HAL_CLK_SOURCE_XO;
  }
} /* END HAL_clk_GCCSourceMapFromHW */



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


/* ===========================================================================
**  HAL_clk_GFXRailPowerDown
**
** ======================================================================== */

void HAL_clk_GFXRailPowerDown( void )
{
#if 0
  HWIO_OUTF( MMSS_GX_DOMAIN_MISC, OCMEM_FIFO_HALT, 1 );

  HWIO_OUT(MMSS_GX_DOMAIN_MISC, 0x1111);
#endif
}


/* ===========================================================================
**  HAL_clk_GFXRailPowerUp
**
** ======================================================================== */

void HAL_clk_GFXRailPowerUp( void )
{
#if 0
  HWIO_OUT( MMSS_GX_DOMAIN_MISC, 0x1111 );
  HAL_clk_BusyWait( 1 );

  HWIO_OUTF(MMSS_GX_DOMAIN_MISC, OCMEM_FIFO_HALT, 1);
  
  HWIO_OUTF(MMSS_GX_DOMAIN_MISC, OXILI_OCMEM_RESET, 0x0);

  HWIO_OUTF(MMSS_GX_DOMAIN_MISC, OXILI_OCMEM_CLAMP_IO, 0x0);

  HWIO_OUT(MMSS_GX_DOMAIN_MISC, 0x0);
#endif
}

/* ===========================================================================
**  HAL_clk_GPLL0_disable_deep_sleep
**
** ======================================================================== */
void HAL_clk_GPLL0_disable_deep_sleep(void)
{
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_VOTE_FSM_ENA ,0);
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_BYPASSNL,0);
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_RESET_N ,0);
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_OUTCTRL ,0);
}
/* ===========================================================================
**  HAL_clk_GPLL0_FSM_reset_deep_sleep
**
** ======================================================================== */
void HAL_clk_GPLL0_FSM_reset_deep_sleep(void)
{
     

  HWIO_OUTF(GCC_GPLL0_MODE, PLL_VOTE_FSM_RESET ,1);
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_BYPASSNL,0);
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_RESET_N ,0);
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_OUTCTRL ,0);
   
  HAL_clk_BusyWait( 1 );
        
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_VOTE_FSM_ENA ,1);
  
  HAL_clk_BusyWait( 1 );
  
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_VOTE_FSM_RESET ,0);
  
}  

/* ===========================================================================
**  HAL_clk_GPLL1_disable_deep_sleep
**
** ======================================================================== */
void HAL_clk_GPLL1_disable_deep_sleep(void)
{
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_VOTE_FSM_ENA ,0);
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_BYPASSNL,0);
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_RESET_N ,0);
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_OUTCTRL ,0);
}
/* ===========================================================================
**  HAL_clk_GPLL1_FSM_reset_deep_sleep
**
** ======================================================================== */
void HAL_clk_GPLL1_FSM_reset_deep_sleep(void)
{
     

  HWIO_OUTF(GCC_GPLL1_MODE, PLL_VOTE_FSM_RESET ,1);
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_BYPASSNL,0);
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_RESET_N ,0);
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_OUTCTRL ,0);
   
  HAL_clk_BusyWait( 1 );
        
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_VOTE_FSM_ENA ,1);
  
  HAL_clk_BusyWait( 1 );

  HWIO_OUT(GCC_GPLL1_CONFIG_CTL, 0x30031800);
  
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_VOTE_FSM_RESET ,0);
  
  HAL_clk_BusyWait( 30 );
  
  HWIO_OUT(GCC_GPLL1_CONFIG_CTL, 0x31800);

  while (HWIO_INF(GCC_GPLL1_STATUS, PLL_ACTIVE_FLAG) != 1)
  {
    HAL_clk_BusyWait(10);
  }
  
}  

/* ===========================================================================
**  HAL_clk_GetHWParNum
**
** ======================================================================== */
uint32 HAL_clk_GetHWParNum(void)
{
  return HWIO_INF(TLMM_HW_REVISION_NUMBER, PARTNUM);
} 

