/*
===========================================================================
  @file ClockSBLSDCC.c

  This file provides clock initialization for starting SDCC clocks at boot.
===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  =========================================================================

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/clock/hw/msm8909w/src/ClockTools.c#1 $
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

/*=========================================================================
      Data
==========================================================================*/
struct Clock_SDCCRegAddrType
{
  uint32 nCmd;
  uint32 nApps;
  uint32 nAHB;
};

const struct Clock_SDCCRegAddrType Clock_SDCCRegs[CLK_SDC_NUM_CLKS] = 
{ 
  /* CLK_SDC_NONE: */
  {0,0,0},
    
  { /* CLK_SDC1: */
    HWIO_ADDR(GCC_SDCC1_APPS_CMD_RCGR),
    HWIO_ADDR(GCC_SDCC1_APPS_CBCR),
    HWIO_ADDR(GCC_SDCC1_AHB_CBCR)
  },
  { /* CLK_SDC2: */
    HWIO_ADDR(GCC_SDCC2_APPS_CMD_RCGR),
    HWIO_ADDR(GCC_SDCC2_APPS_CBCR),
    HWIO_ADDR(GCC_SDCC2_AHB_CBCR)
  }
};

/* ============================================================================
**  Function : Clock_SetSDCClockFrequencyExt
** ============================================================================
*/
/*!
    Configure SDC clock to a specific perf level.

    @param eClockPerfLevel   -  [IN] Clock frequency level
           eClock            -  [IN] SDC clock to configure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
uint32 Clock_SetSDCClockFrequencyExt( uint32 nFreqKHz, ClockSDCType eClock)
{
  ClockConfigMuxType SDCCCfg = {0};
  const ClockSDCCfgType *pCfg = NULL;
  uint32 n;
  const Clock_SBLConfigType *cfg = Clock_SBLConfig();

  /* Check for a valid SDC */
  if(eClock >= CLK_SDC_NUM_CLKS) return 0;
  if (Clock_SDCCRegs[eClock].nCmd == 0) return 0;

  /* Copy the config to the local so the nCMDCGRAddr can be modified. */
  pCfg = (eClock == CLK_SDC1)? (cfg->SDC1_Ext_Cfg) : (cfg->SDC2_Ext_Cfg);

  /* Find the nearest frequency that matches the request */
  for(n = 0; pCfg[n].nFrequency != 0; n++)
  {
    if(pCfg[n].nFrequency >= nFreqKHz) break;
  }
  if(pCfg[n].nFrequency == 0) return 0;

  /* A configuration is found.  Set it */
  SDCCCfg = pCfg[n].Cfg; /* copy struct and fix it */
  SDCCCfg.nCMDCGRAddr = Clock_SDCCRegs[eClock].nCmd;

  /*
   * Enable access to the PCNOC.
   */
  Clock_ToggleClock(HWIO_GCC_SNOC_PCNOC_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  Clock_ToggleClock(HWIO_GCC_PCNOC_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);   

  if( ! Clock_EnableSource( SDCCCfg.eSource )) return 0;
  if( ! Clock_ConfigMux(&SDCCCfg)) return 0;
  if( ! Clock_ToggleClock(Clock_SDCCRegs[eClock].nApps, CLK_TOGGLE_ENABLE)) return 0;
  if( ! Clock_ToggleClock(Clock_SDCCRegs[eClock].nAHB, CLK_TOGGLE_ENABLE)) return 0;

  return pCfg[n].nFrequency;
  }

/* ============================================================================
**  Function : Clock_SetSDCClockFrequency
** ============================================================================
*/
/*!
    Configure SDC clock to a specific perf level.

    @param eClockPerfLevel   -  [IN] SDC Clock perf level
           eClock            -  [IN] SDC clock to configure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetSDCClockFrequency
(
  ClockBootPerfLevelType ePerfLevel,
  ClockSDCType eClock
)
{
  const Clock_SBLConfigType *cfg = Clock_SBLConfig();
  uint32 nFreq = 0;

  if(ePerfLevel >= CLOCK_BOOT_PERF_NUM) return FALSE;
 
  nFreq = Clock_SetSDCClockFrequencyExt( cfg->SDC_Cfg[ePerfLevel], eClock);
  if( nFreq == 0) return FALSE;
  return TRUE;

} /* END Clock_SetSDCClockFrequency */


/* ============================================================================
**  Function : Clock_InitUSB
** ============================================================================
*/
/*!
    Configure USB clocks.

    @param None.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_InitUSB(void)
{
  const ClockConfigMuxType *USBCfg;
  const Clock_SBLConfigType *cfg = Clock_SBLConfig();
  USBCfg = &cfg->USB_Cfg;

  if (HWIO_INF(GCC_USB_HS_SYSTEM_CBCR, CLK_ENABLE) == 0)
  {
    if( ! Clock_EnableSource( USBCfg->eSource )) return FALSE;
    if( ! Clock_ConfigMux(USBCfg)) return FALSE;

    Clock_ToggleClock(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_USB_HS_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  }
  
  return TRUE;

} /* END Clock_InitUSB */

/* ============================================================================
**  Function : Clock_DisableUSB
** ============================================================================
*/
/*!
    Disable USB clocks.

    @param None.
    @return
    TRUE -- Disable was successful.
    FALSE -- Disable failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_DisableUSB(void)
{
	
   /* A mux config for USB to CXO */
  ClockConfigMuxType USBCfg =
  {
    .nCMDCGRAddr = HWIO_ADDR(GCC_USB_HS_SYSTEM_CMD_RCGR),
    .eMux = MUX_GCC,
    .eSource = SRC_CXO,
    .nDiv2x = 0,
    .nM = 0,
    .nN = 1,
    .n2D = 0
  };

  /*
   * Check for whether clock is on.
   */
  if (HWIO_INF(GCC_USB_HS_SYSTEM_CBCR, CLK_OFF) == 0)
  {
    /* Swicth USB clock to CXO */
    if( ! Clock_ConfigMux(&USBCfg)) return FALSE;
    /*Switch off USB clocks*/
    Clock_ToggleClock(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR, CLK_TOGGLE_DISABLE);    
    Clock_ToggleClock(HWIO_GCC_USB_HS_AHB_CBCR_ADDR, CLK_TOGGLE_DISABLE);
    Clock_ToggleClock(HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_ADDR, CLK_TOGGLE_DISABLE); 
   
  }
	
  return TRUE;
  
} /* END Clock_DisableUSB */

/* ========================================================================
**  Function : Clock_EnableQPICForDownloadMode
** ======================================================================*/
/*
    Description: Configure QPIC(NAND) clocks at 100MHz on GPLL0 and enable
                 QPIC clocks.
                 The function is used in Download Mode where QPIC clocks are
                 not enabeld by PBL.

    @param None
    @return TRUE on success.

    @dependencies
    None.

    @sa None
*/
boolean Clock_EnableQPICForDownloadMode( void )
{

  /*
   * Clocks access to the Peripheral NOC enabled in Clock_ReplaceUSBBootClockEnable.
   */

  /*
   * Enable the QPIC clocks.
   */
  if( ! Clock_ToggleClock(HWIO_GCC_QPIC_CBCR_ADDR, CLK_TOGGLE_ENABLE)) return FALSE;
  if( ! Clock_ToggleClock(HWIO_GCC_QPIC_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE)) return FALSE;
  if( ! Clock_ToggleClock(HWIO_GCC_QPIC_SYSTEM_CBCR_ADDR, CLK_TOGGLE_ENABLE)) return FALSE;

  return TRUE;

}
