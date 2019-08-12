/*
==============================================================================

FILE:         HALclkMainRPM.c

DESCRIPTION:
  This file contains the main platform initialization code for the clock
  HAL on the RPM processor on MSM8974.

==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/mdm9x45/src/HALclkMainRPM.c#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/20/12   vph      Created.

==============================================================================
            Copyright (c) 2012-2013 QUALCOMM Technologies Incorporated.
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
#include "assert.h"


/* List of clocks being turned off during deep sleep */
typedef struct
{
 boolean bSPMIClk;
 boolean bImemAXIClk;
 boolean bMsgRAMClk;
} HAL_MiscClkStateType;  


/* ============================================================================
**    Prototypes
** ==========================================================================*/

void HAL_clk_PlatformInitSources(void);


/* ============================================================================
**    Externs
** ==========================================================================*/

extern void HAL_clk_PlatformInitGCCMain(void);

/* ============================================================================
**    Data
** ==========================================================================*/
#define MAX_RETRY 5

static HAL_MiscClkStateType HAL_MiscClkState;

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
    HAL_CLK_PLL_PRIUS
  },
  {
    HWIO_OFFS(GCC_GPLL2_MODE),
    HAL_CLK_FMSK(GCC_RPM_GPLL_ENA_VOTE, GPLL2),
    HAL_CLK_PLL_PRIUS

  },
  {
    HWIO_OFFS(GCC_GPLL3_MODE),
    HAL_CLK_FMSK(GCC_RPM_GPLL_ENA_VOTE, GPLL3),
    HAL_CLK_PLL_PRIUS
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
    HAL_CLK_SOURCE_GPLL0,   &HAL_clk_aPLLContextGPLL[0], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_GPLL2,   &HAL_clk_aPLLContextGPLL[1], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_GPLL3,   &HAL_clk_aPLLContextGPLL[2], CLK_CTL_BASE);


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
**  HAL_clk_MiscClkDisableDeepSleep
** ======================================================================== */
void HAL_clk_MiscClkDisableDeepSleep(void)
{
  uint32 nRetry;

  HAL_MiscClkState.bSPMIClk = (HWIO_INF(GCC_SPMI_SER_CBCR, CLK_ENABLE) == 1);
  if ( HAL_MiscClkState.bSPMIClk )
  {
    HWIO_OUTF(GCC_SPMI_SER_CBCR, CLK_ENABLE, 0);

    // Test to make sure that clock gets turn off
    nRetry = 0;
    while( (HWIO_INF(GCC_SPMI_SER_CBCR, CLK_OFF) == 0) && (nRetry < MAX_RETRY) )
    {
      nRetry++;
      HAL_clk_BusyWait( 1 );
    }
    assert (nRetry < MAX_RETRY);
  }
  
  HAL_MiscClkState.bImemAXIClk = (HWIO_INF(GCC_IMEM_AXI_CBCR, CLK_OFF) == 0);
  if ( HAL_MiscClkState.bImemAXIClk )
  {
    HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, IMEM_AXI_CLK_ENA, 0);

    // Test to make sure that clock gets turn off
    nRetry = 0;
    while( (HWIO_INF(GCC_IMEM_AXI_CBCR, CLK_OFF) == 0) && (nRetry < MAX_RETRY) )
    {
      nRetry++;
      HAL_clk_BusyWait( 1 );
    }
    assert (nRetry < MAX_RETRY);
  }

  HAL_MiscClkState.bMsgRAMClk = (HWIO_INF(GCC_MSG_RAM_AHB_CBCR, CLK_OFF) == 0);
  if ( HAL_MiscClkState.bMsgRAMClk )
  {
    HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, MSG_RAM_AHB_CLK_ENA, 0);

    // Test to make sure that clock gets turn off
    nRetry = 0;
    while( (HWIO_INF(GCC_MSG_RAM_AHB_CBCR, CLK_OFF) == 0) && (nRetry < MAX_RETRY) )
    {
      nRetry++;
      HAL_clk_BusyWait( 1 );
    }
    assert (nRetry < MAX_RETRY);
  }

  // Turn periph_mem off
  HWIO_OUTF( GCC_BOOT_ROM_AHB_CBCR, FORCE_MEM_PERIPH_OFF, 1 );
  HWIO_OUTF( GCC_SEC_CTRL_ACC_CBCR, FORCE_MEM_PERIPH_OFF, 1 );
  
}  


/* ===========================================================================
**  HAL_clk_MiscClkRestoreDuringWakeup
** ======================================================================== */
void HAL_clk_MiscClkRestoreDuringWakeup(void)
{
  // Turn periph_mem back on
  HWIO_OUTF( GCC_BOOT_ROM_AHB_CBCR, FORCE_MEM_PERIPH_OFF, 0 );
  HWIO_OUTF( GCC_SEC_CTRL_ACC_CBCR, FORCE_MEM_PERIPH_OFF, 0 );


  if( HAL_MiscClkState.bSPMIClk == TRUE)
  {
    HWIO_OUTF(GCC_SPMI_SER_CBCR, CLK_ENABLE, 1);
    while(HWIO_INF(GCC_SPMI_SER_CBCR, CLK_OFF) == 1);
  }

  if( HAL_MiscClkState.bImemAXIClk == TRUE)
  {
    HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, IMEM_AXI_CLK_ENA, 1);
    while(HWIO_INF(GCC_IMEM_AXI_CBCR, CLK_OFF) == 1);
  }

  if ( HAL_MiscClkState.bMsgRAMClk )
  {
    HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, MSG_RAM_AHB_CLK_ENA, 1);
    while(HWIO_INF(GCC_MSG_RAM_AHB_CBCR, CLK_OFF) == 1);
  }
}  
 
