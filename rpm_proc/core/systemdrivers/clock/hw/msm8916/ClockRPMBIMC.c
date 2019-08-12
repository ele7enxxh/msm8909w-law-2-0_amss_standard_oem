/*
===========================================================================
*/
/**
  @file ClockRPMBIMC.c 
  
  BIMC APIs definitions for the MSM8974 RPM clock driver.
*/
/*  
  ====================================================================

  Copyright (c) 2013 QUALCOMM Technologies, Inc.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/hw/msm8916/ClockRPMBIMC.c#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $
 
  when       who     what, where, why
  --------   ---     -------------------------------------------------
  01/29/13   vph     Initial version
 
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/

#include "ddr_drivers.h"
#include "swevent.h"
#include "ClockRPMNPA.h"
#include "ClockRPMBIMC.h"
#include "ClockCommon.h"
#include "assert.h"

/*=========================================================================
      Macros
==========================================================================*/

#define BIMC_PRE_COLLAPSE_FREQ  100000
#define DEHR_SAVE_DONE          0x00000001
#define DEHR_RESTORE_DONE       0x00010000
#define PCNOC_LEVEL             1

/*=========================================================================
      DEBUG MARKERS to track BIMC SWITCH state.
==========================================================================*/

typedef enum {
  CLK_BIMC_SET_FREQ_DEFAULT,
  CLK_BIMC_SET_FREQ_ENTER,
  CLK_BIMC_PRE_DDR_DONE,
  CLK_BIMC_SET_CLOCK_DONE,
  CLK_BIMC_POST_DDR_DONE,
  CLK_BIMC_SET_FREQ_EXIT
} clk_dbg_bimc_marker_type;

volatile clk_dbg_bimc_marker_type clk_dbg_bimc_marker = CLK_BIMC_SET_FREQ_DEFAULT;

typedef struct
{
  uint32 clk_dbg_bimc_pc_count;
  uint32 clk_dbg_bimc_pc_restore_count;
  uint32 clk_dbg_bimc_pc_parallelization_count;
  uint32 clk_dbg_bimc_pc_parallelization_restore_count;
} bimc_pc_debug_type;

static bimc_pc_debug_type bimc_pc_debug;
volatile uint32 clk_dbg_SWITCH_COUNT_MATRIX[6][6];

static boolean BIMC_PC_PARALLELIZATION_ENA = TRUE;

/*=========================================================================
      Prototypes
==========================================================================*/


/*=========================================================================
      Data
==========================================================================*/
extern Clock_NPAResourcesType Clock_Resources;


//static const boolean SETTLING_FLAG = true;
#if 0
/*===========================================================================

  FUNCTION      Clock_SetBIMCClockConfig

  DESCRIPTION   This function is for setting BIMC frequency.  Due to the
                dependency on DDR with voltage, this function is using for 
                switching the voltage first, call ddr_pre_clock_switch, then
                switch the clock.
                  
  PARAMETERS    pDomain    - Clock domain for setting voltage
                pNewConfig - New config frequency

  DEPENDENCIES  None.

  RETURN VALUE  Pointer to all clocks NPA resource

  SIDE EFFECTS  None.

===========================================================================*/
DALResult Clock_SetBIMCClockConfig
(
  ClockDomainNodeType *pDomain,
  ClockMuxConfigType  *pNewConfig
)
{
  ClockSourceNodeType     *pNewSource;
  DALResult               eResult;
  uint32                  nSrcIdx;
  boolean                 bCPUDomain;
  boolean                 bVoltageRequest;
  ClockResourceType       *pBIMCClk;
  uint32                  nCurClkKHz, nNewClkKHz;
  ClockDrvCtxt            *pDrvCtxt;

  pDrvCtxt = Clock_GetDrvCtxt();

  pBIMCClk = &Clock_Resources.BIMCClockResource;
  nCurClkKHz = pBIMCClk->pClock->pDomain->pBSPConfig[pBIMCClk->nCurLevel].nFreqHz / 1000;
  nNewClkKHz = pNewConfig->nFreqHz / 1000;

  /* Make sure the domain handle is valid */
  if (pDomain == NULL)
  {
    return DAL_ERROR;
  }

  /* Determine if this is a CPU domain */
  bCPUDomain = CLOCK_FLAG_IS_SET(pDomain, DOMAIN_CPU);

  /*
   * Determine if we should make a voltage request.  We do so if this
   * domain is not configured to make requests when enabled, or if it
   * is thus configured and is currently enabled.
   */
  bVoltageRequest =
    (pDomain->nReferenceCount > 0) &&
    !CLOCK_FLAG_IS_SET(pDomain, DOMAIN_HOLD_VREG_REQUEST);

  /* Get the new source pointer. */
  nSrcIdx = pDrvCtxt->anSourceIndex[pNewConfig->HALConfig.eSource];

  if (nSrcIdx != 0xFF)
  {
    pNewSource = &pDrvCtxt->aSources[nSrcIdx];
  }
  else
  {
    pNewSource = NULL;
  }

  /*
   * Configure the source if required.
   */
  if (pNewSource != NULL && pNewConfig->pSourceConfig != NULL)
  {
    eResult = Clock_ConfigSource(
      pDrvCtxt, pNewSource, pNewConfig->pSourceConfig);

    if (eResult != DAL_SUCCESS)
    {
      // TODO error!
      return eResult;
    }
  }

  /*
   * If the domain is active, and the new source is different from the
   * current one then we enable the new source.
   */
  if (pDomain->nReferenceCount > 0 &&
      pDomain->pSource != pNewSource &&
      pNewSource != NULL)
  {
    eResult = Clock_EnableSource(pDrvCtxt, pNewSource, bCPUDomain);

    if (eResult != DAL_SUCCESS)
    {
      return eResult;
    }
  }

  /*
   * If we are increasing the voltage requirement, do so now.
   */
  if (bVoltageRequest && pNewConfig->eVRegLevel > pDomain->eVRegLevel)
  {
    Clock_VoltageRequest(
      pDrvCtxt, &pDomain->eVRegLevel, pNewConfig->eVRegLevel, FALSE);
  }
  
  /* Inform BIMC about the new clock rate that will switching to */
  ddr_pre_clock_switch( nCurClkKHz, nNewClkKHz, SDRAM_INTERFACE_0 );

  /*
   * Configure the clock MUX
   */
  HAL_clk_ConfigClockMux(pDomain->HALHandle, &pNewConfig->HALConfig);

  /* Inform BIMC that new clock already switched */
  ddr_post_clock_switch( nCurClkKHz, nNewClkKHz, SDRAM_INTERFACE_0 );

  /*
   * If we are decreasing the voltage requirement, do so now.
   */
  if (bVoltageRequest && pNewConfig->eVRegLevel < pDomain->eVRegLevel)
  {
    Clock_VoltageRequest(
      pDrvCtxt, &pDomain->eVRegLevel, pNewConfig->eVRegLevel, FALSE);
  }

  /*
   * If the domain is active, and the new source is different from the
   * current then we disable the old source.
   */
  if (pDomain->nReferenceCount > 0 &&
      pDomain->pSource != pNewSource &&
      pDomain->pSource != NULL)
  {
    Clock_DisableSource(pDrvCtxt, pDomain->pSource, bCPUDomain);
  }

  /* Link the domain node to the new source node and update the active config */
  pDomain->pSource = pNewSource;

  /*
   * Check if the current configuration was setup dynamically
   */
  if(CLOCK_FLAG_IS_SET(pDomain, DOMAIN_DYNAMIC_CONFIG))
  {
    /*
     * If new configuration is different from the current one then
     * free up memory.
     */
    if(pDomain->pActiveMuxConfig != pNewConfig)
    {
      CLOCK_FLAG_CLEAR(pDomain, DOMAIN_DYNAMIC_CONFIG);
      DALSYS_Free (pDomain->pActiveMuxConfig);
    }
  }

  pDomain->pActiveMuxConfig = pNewConfig;

  return DAL_SUCCESS;

} /* END Clock_SetBIMCClockConfig */
#endif

#if 0
/*===========================================================================

  FUNCTION      Clock_BIMCSetSpeed

  DESCRIPTION   This function is used for holding GPLL2/GPLL3 on before
                the clock switching.
  
  PARAMETERS    pClockRsc [in] - Clock Resource
                nCurClkKHz [in]- Current clock speed in KHz
                nBSPLevel [in] - New BSP level that clock about to switch to

  DEPENDENCIES  None.

  RETURN VALUE  New clock speed

  SIDE EFFECTS  None.

===========================================================================*/
static uint32 Clock_BIMCSetSpeed
(
  ClockResourceType *pClockRsc,
  uint32            nCurClkKHz,
  uint32            nBSPLevel
)
{
  uint32                      nNewClkKHz;
  HAL_clk_ClockMuxConfigType  MuxCfg; 

  /* Get current BIMC mux config for active GPLL */

  nNewClkKHz = pClockRsc->pClock->pDomain->pBSPConfig[nBSPLevel].nFreqHz / 1000;
 
 
    HAL_clk_DetectClockMuxConfig(pClockRsc->pClock->pDomain->HALHandle, &MuxCfg);
    /* Only reprogram BIMC PLL source if it is not XO or GPLL0 */
    if ( (pClockRsc->pClock->pDomain->pBSPConfig[nBSPLevel].HALConfig.eSource != HAL_CLK_SOURCE_GPLL0) &&
         (pClockRsc->pClock->pDomain->pBSPConfig[nBSPLevel].HALConfig.eSource != HAL_CLK_SOURCE_XO) )
    {
      /* Ping pong between GPLL2/GPLL3 for reconfig its frequency without using divider */
      if ( MuxCfg.eSource == HAL_CLK_SOURCE_GPLL2 )
        {
          pClockRsc->pClock->pDomain->pBSPConfig[nBSPLevel].HALConfig.eSource = HAL_CLK_SOURCE_GPLL3;
          pClockRsc->pClock->pDomain->pBSPConfig[nBSPLevel].pSourceConfig->eSource = HAL_CLK_SOURCE_GPLL3;
        }
        else
        {
          pClockRsc->pClock->pDomain->pBSPConfig[nBSPLevel].HALConfig.eSource = HAL_CLK_SOURCE_GPLL2;
          pClockRsc->pClock->pDomain->pBSPConfig[nBSPLevel].pSourceConfig->eSource = HAL_CLK_SOURCE_GPLL2;
      }
    }
  
  /* Switch to new clock rate */
  nNewClkKHz = Clock_SetClockSpeed( pClockRsc, nNewClkKHz * 1000) / 1000;
  
  return nNewClkKHz;
}
#endif
/*===========================================================================

  FUNCTION      Clock_BIMCSwitchFrequency

  DESCRIPTION   This function is used for changing BIMC frequency.
  
  PARAMETERS    pClockRsc [in]  - Clock Resource
                nNewClkKHz [in] - New clock speed in KHz

  DEPENDENCIES  None.

  RETURN VALUE  New clock speed

  SIDE EFFECTS  None.

===========================================================================*/
uint32 Clock_BIMCSwitchFrequency( ClockResourceType *pClockRsc, uint32 nNewClkKHz )
{
  uint32                  nCurClkKHz, nCfg, nSupCfg;
  ClockMuxConfigType      *pNewConfig;
  Clock_NPAResourcesType  *pNPAResources;
  ClockResourceType       *pAPSSTCUASYNCResource;
  
  pNPAResources = Clock_GetNPAResources();

  pAPSSTCUASYNCResource = &pNPAResources->APSSTCUASYNCClockResource;

  nCurClkKHz = pClockRsc->pClock->pDomain->pBSPConfig[pClockRsc->nCurLevel].nFreqHz / 1000;

  /* Find new clock frequency that closest to support BSP frequency */
  pNewConfig = pClockRsc->pClock->pDomain->pBSPConfig;

  nSupCfg = pClockRsc->nMinLevel;
  for ( nCfg = pClockRsc->nMinLevel; nCfg <= pClockRsc->nMaxLevel; nCfg++ )
  {
    if ( Clock_IsBSPSupported(&pNewConfig[nCfg]) )
    {
      nSupCfg = nCfg;
      // Found new config clock that close or equal the requested frequency
      if ( nNewClkKHz <= (pNewConfig[nCfg].nFreqHz / 1000) )
      {
        break;
      }
    }
  }
  nNewClkKHz = pNewConfig[nSupCfg].nFreqHz / 1000;

  if ( nNewClkKHz != nCurClkKHz )
  {
    clk_dbg_SWITCH_COUNT_MATRIX[pClockRsc->nCurLevel][nSupCfg]++;
    clk_dbg_bimc_marker = CLK_BIMC_SET_FREQ_ENTER;
    ddr_pre_clock_switch( nCurClkKHz, nNewClkKHz, SDRAM_INTERFACE_0 );
    clk_dbg_bimc_marker = CLK_BIMC_PRE_DDR_DONE;
#if 0
    /* This is not required as there is no dynamic re-configuration of GPLL3 (aka BIMC_PLL) */
        nNewClkKHz = Clock_BIMCSetSpeed( pClockRsc, nNewClkKHz, nSupCfg );
#endif
	/* Switch to new clock rate */
    nNewClkKHz = Clock_SetClockSpeed( pClockRsc, nNewClkKHz * 1000) / 1000;

    clk_dbg_bimc_marker = CLK_BIMC_SET_CLOCK_DONE;
    ddr_post_clock_switch( nCurClkKHz, nNewClkKHz, SDRAM_INTERFACE_0 );
    clk_dbg_bimc_marker = CLK_BIMC_POST_DDR_DONE;

    pNewConfig =  pAPSSTCUASYNCResource->pClock->pDomain->pBSPConfig;
    Clock_SetClockSpeed( pAPSSTCUASYNCResource, pNewConfig[nSupCfg].nFreqHz);
    clk_dbg_bimc_marker = CLK_BIMC_SET_FREQ_EXIT;
  }

  return nNewClkKHz;

}

/*===========================================================================

  FUNCTION      Clock_SetDirectBusSpeed

  DESCRIPTION   This function allows to set bus frequency directly without go thru NPA request.
                Use this function for request to change bus speed directly without DCVS enable

  PARAMETERS    eBusID       [in] -- Bus ID
                nNewSpeedKHz [in] -- New bus speed in KHz

  DEPENDENCIES  None.

  RETURN VALUE  None

  SIDE EFFECTS  None.

===========================================================================*/
void Clock_SetDirectBusSpeed( ClockBusType eBusID, uint32 nNewSpeedKHz )
{
  uint32 nNewClk;
  ClockResourceType *pBIMCClk;

  if ( eBusID  == CLOCK_RESOURCE_BIMC )
  {
    pBIMCClk = &Clock_Resources.BIMCClockResource;

    nNewClk = Clock_BIMCSetFrequency( pBIMCClk, nNewSpeedKHz, FALSE );    
   
    /* Inform NPA new clock speed has been updated */
    npa_assign_resource_state( pBIMCClk->NPANode.Resource.handle, nNewClk );
  }
}


/* =========================================================================
**  Function : Clock_BIMCGetMiscInfo
** =========================================================================*/
/**
  Get GCC_BIMC_MISC register info

  @param 
    None

  @return
    Return GCC_BIMC_MISC info

  @dependencies
    None

  @sa
    None
*/

uint32 Clock_BIMCGetMiscInfo( void )
{
  return HAL_clk_BIMCGetMiscInfo();
}

/* =========================================================================
**  Function : Clock_BIMCIsSaveDone
** =========================================================================*/
/**
  This function is checking if DEHR saving is done or not.  Once DEHR saving
  is done, it will turn off DEHR clock, and transition BIMC to BIMC_COLLPASED
  state

  @param 
    None

  @return
    TRUE -  DEHR saving is done
    FALSE - DEHR saving is not done

  @dependencies
    None

  @sa
    None
*/
static boolean Clock_BIMCIsSaveDone( void )
{
  
  // Do nothing if BIMC is already collapsed.
  if ( Clock_Resources.BIMCResource.eBIMCState == BIMC_COLLAPSED )
    return TRUE;

  // Only enter this state when BIMC is in BIMC_SAVING.  Abort otherwise
  assert ( Clock_Resources.BIMCResource.eBIMCState == BIMC_SAVING );

  /* Power collapsed enable.  Check DEHR state */
  if ((Clock_Resources.BIMCResource.bPowerCollapsed == TRUE) && HAL_clk_IsDEHREnable() )
  {
    /* Wait until BIMC Power collapse and DEHR save is done */
    while ( Clock_IsPowerDomainEnabled(Clock_Resources.BIMCResource.nBIMCPowerID) == TRUE);
    while ( (HAL_clk_GetDEHRStatus() & DEHR_SAVE_DONE) != DEHR_SAVE_DONE );

    /* Disable DEHR clock */
    Clock_DisableClock( Clock_Resources.BIMCResource.nClockDehr );
      
    /* Disable BIMC clock */
    Clock_DisableClock( (ClockIdType)Clock_Resources.BIMCClockResource.pClock );

    if (Clock_Resources.PCNOCClockResource.nSavLevel < PCNOC_LEVEL)
    {
      /* Need to restore PCNOC when BIMC_PC is complete */
      Clock_SetClockConfigEx( Clock_Resources.PCNOCClockResource.pClock, 
                             &Clock_Resources.PCNOCClockResource.pClock->pDomain->pBSPConfig[Clock_Resources.PCNOCClockResource.nSavLevel] );
    }
    SWEVENT(CLOCK_EVENT_BIMC_COLLAPSE, CLOCK_SWE_BIMC_DEHR_DONE);  
  }

  /* Save done.  Set to collapsed state and turn off the rest of BIMC clocks */
  Clock_Resources.BIMCResource.eBIMCState = BIMC_COLLAPSED;

  return TRUE;
}


/* =========================================================================
**  Function : Clock_BIMCIsRestoreDone
** =========================================================================*/
/**
  This function is checking if DEHR restoring is done or not.  Once DEHR restoring
  is done, it will restore DDR clock to previous clock speed.

  @param 
    None

  @return
    Zero (0) - DEHR restore is not done
    Non-zero - DEHR restore is done, DDR restore to previous clock speed.

  @dependencies
    None

  @sa
    None
*/
uint32 Clock_BIMCIsRestoreDone( void )
{
  uint32            nFreqKHz;
  ClockResourceType *pBIMCClk;

  pBIMCClk = &Clock_Resources.BIMCClockResource;
  nFreqKHz = pBIMCClk->pClock->pDomain->pBSPConfig[pBIMCClk->nSavLevel].nFreqHz/1000;

  // Do nothing if BIMC is already restored.
  if ( Clock_Resources.BIMCResource.eBIMCState == BIMC_ON )
    return nFreqKHz;

  // Only enter this state when BIMC is in BIMC_RESTORING.  Abort otherwise
  assert ( Clock_Resources.BIMCResource.eBIMCState == BIMC_RESTORING );

  /* Power collapsed enable.  Check DEHR state */
  if ((Clock_Resources.BIMCResource.bPowerCollapsed == TRUE) && HAL_clk_IsDEHREnable() )
  {
    /* Loop until DEHR successfully restore */
    while ( Clock_IsPowerDomainEnabled(Clock_Resources.BIMCResource.nBIMCPowerID) == FALSE );
    while ( (HAL_clk_GetDEHRStatus() & DEHR_RESTORE_DONE) != DEHR_RESTORE_DONE );

    /* Debug variable to count num of BIMC restores */
    bimc_pc_debug.clk_dbg_bimc_pc_restore_count++;
	
    /* Call DDR to for exit power collapse */
    ddr_exit_power_collapse( nFreqKHz );
    
    /* Disable DEHR clock */
    Clock_DisableClock( Clock_Resources.BIMCResource.nClockDehr );
    SWEVENT(CLOCK_EVENT_BIMC_RESTORE, CLOCK_SWE_BIMC_DONE); 
  }
  else
  {
    /* Take DDR out of self-refresh */
    ddr_post_xo_shutdown( nFreqKHz );
  }

  Clock_Resources.BIMCResource.eBIMCState = BIMC_ON;

  Clock_BIMCSwitchFrequency( pBIMCClk, nFreqKHz );

  return nFreqKHz;
}


/* =========================================================================
**  Function : Clock_BIMCSaving
** =========================================================================*/
/**
  This function is used for saving DEHR and put DDR in power collapse or
  self-refresh state.

  @param 
    None

  @return
    None

  @dependencies
    None

  @sa
    None
*/
static void Clock_BIMCSaving( void )
{
  ClockResourceType       *pBIMCClk;
  ClockResourceType       *pPCNOCClk;
  pBIMCClk  = &Clock_Resources.BIMCClockResource;
  pPCNOCClk = &Clock_Resources.PCNOCClockResource;

  // Only enter this state when BIMC is in BIMC_ON.  Abort otherwise
  assert ( Clock_Resources.BIMCResource.eBIMCState == BIMC_ON );

  if ( BIMC_PRE_COLLAPSE_FREQ != 
       pBIMCClk->pClock->pDomain->pBSPConfig[pBIMCClk->nCurLevel].nFreqHz / 1000 )
  {
    Clock_BIMCSwitchFrequency( pBIMCClk, BIMC_PRE_COLLAPSE_FREQ );
  }

  /* Enable DEHR clock for check if DEHR is enable for BIMC power collapsed */
  Clock_EnableClock( Clock_Resources.BIMCResource.nClockDehr );

  /* If BIMC in power collapsed mode */
  if ((Clock_Resources.BIMCResource.bPowerCollapsed == TRUE) && HAL_clk_IsDEHREnable() )
  {
    if (pPCNOCClk->nCurLevel < PCNOC_LEVEL)
    {
      /*Scaling PCNOC to 50Mhz to reduce BIMC_PC timeline*/
      pPCNOCClk->nSavLevel = pPCNOCClk->nCurLevel;
      Clock_SetClockConfigEx( pPCNOCClk->pClock, 
                             &pPCNOCClk->pClock->pDomain->pBSPConfig[PCNOC_LEVEL] );
    }
    /* Put DDR to power collapse and freeze IO */
    ddr_enter_power_collapse( BIMC_PRE_COLLAPSE_FREQ  );
	
    /* Debug variable count the no. of Collapses*/
    bimc_pc_debug.clk_dbg_bimc_pc_count++;

    /* Disable BIMC power domain for BIMC collapse */
    Clock_DisablePowerDomain( Clock_Resources.BIMCResource.nBIMCPowerID );
  
    SWEVENT(CLOCK_EVENT_BIMC_COLLAPSE, CLOCK_SWE_BIMC_DEHR_SAVE);
    Clock_Resources.BIMCResource.eBIMCState = BIMC_SAVING;
  }
  else
  {
    /* BIMC is in self-refresh.  Turn off DEHR clock*/
    Clock_DisableClock( Clock_Resources.BIMCResource.nClockDehr );
    /* Put DDR in self-refresh mode */
    ddr_pre_xo_shutdown( BIMC_PRE_COLLAPSE_FREQ );

    /* Disable BIMC clock */
    Clock_DisableClock( (ClockIdType)Clock_Resources.BIMCClockResource.pClock );

    Clock_Resources.BIMCResource.eBIMCState = BIMC_COLLAPSED;
  }
}


/* =========================================================================
**  Function : Clock_BIMCRestoring
** =========================================================================*/
/**
  This function is used for restoring DEHR and BIMC clocks.  It will set
  BIMC state to BIMC_RESTORING state.

  @param 
    nFreqKHz - Restoring frequency once BIMC is up.

  @return
    None

  @dependencies
    None

  @sa
    None
*/
void Clock_BIMCRestoring( uint32 nFreqKHz )
{  
  uint32              nCfg;
  ClockMuxConfigType  *pNewConfig;
  ClockResourceType   *pBIMCClk;

  // Only enter this state when BIMC is in BIMC_COLLAPSED.  Abort otherwise
  assert ( Clock_Resources.BIMCResource.eBIMCState == BIMC_COLLAPSED );

  pBIMCClk = &Clock_Resources.BIMCClockResource;

  pBIMCClk->nSavLevel = pBIMCClk->nMinLevel;
  pNewConfig = pBIMCClk->pClock->pDomain->pBSPConfig;

  /* Save index of restore frequency that closest to support BSP frequency */
  for ( nCfg = pBIMCClk->nMinLevel; nCfg <= pBIMCClk->nMaxLevel; nCfg++ )
  {
    if ( Clock_IsBSPSupported(&pNewConfig[nCfg]) )
    {
      pBIMCClk->nSavLevel = nCfg;

      // Found new config clock that close or equal the requested frequency
      if ( nFreqKHz <= pNewConfig[nCfg].nFreqHz / 1000 )
      {
        break;
      }
    }
  }

  /* Enable DEHR clock for check if DEHR is enable for BIMC power collapsed */
  Clock_EnableClock( Clock_Resources.BIMCResource.nClockDehr );

  /* BIMC Power collapsed is enabled.  Disable BIMC power collapsed */
  if ((Clock_Resources.BIMCResource.bPowerCollapsed == TRUE) && HAL_clk_IsDEHREnable() )
  {
    /* Enable BIMC clocks */
    Clock_EnableClock( (ClockIdType)Clock_Resources.BIMCClockResource.pClock );

    /* Enable BIMC power domain for BIMC collapse */
    Clock_EnablePowerDomain( Clock_Resources.BIMCResource.nBIMCPowerID, TRUE );
    
    Clock_Resources.BIMCResource.eBIMCState = BIMC_RESTORING;
  
    /* Log clock frequency */
    SWEVENT(CLOCK_EVENT_BIMC_RESTORE, CLOCK_SWE_BIMC_DEHR_RESTORE );
  }
  else
  {
    /* BIMC is in self-refresh.  Turn off DEHR clock*/
    Clock_DisableClock( Clock_Resources.BIMCResource.nClockDehr );

    /* Enable BIMC clocks */
    Clock_EnableClock( (ClockIdType)Clock_Resources.BIMCClockResource.pClock );

    /* Take DDR out of self-refresh */
    ddr_post_xo_shutdown( pNewConfig[pBIMCClk->nSavLevel].nFreqHz / 1000 );
    
    Clock_Resources.BIMCResource.eBIMCState = BIMC_ON;
  }

}


/* =========================================================================
**  Function : Clock_BIMCollapse
** =========================================================================*/
/**
  This function is used for collapse BIMC.  If the BIMC state is in:
      BIMC_ON        - Transition to DEHR save state (BIMC_SAVING)
      BIMC_SAVING    - Check if DEHR saving is done, transition to COLLAPSED state (BIMC_COLLAPSED)
      BIMC_RESTORING - DEHR is in restore state, check if it is done, and move back 
                       to DERH saving state (BIMC_SAVING).

  @param 
    None

  @return
    Zero - BIMC collapse, current BIMC frequency should be zero.

  @dependencies
    None

  @sa
    None
*/
static uint32 Clock_BIMCollapse( void )
{
  switch ( Clock_Resources.BIMCResource.eBIMCState )
  {
  case BIMC_ON:
    Clock_BIMCSaving();
    break;
    
  case BIMC_SAVING:
    Clock_BIMCIsSaveDone();
    break;

  case BIMC_RESTORING:
    Clock_BIMCIsRestoreDone();

    /* Put back to saving state once restore done */
    Clock_BIMCSaving();
    break;
  }

  return 0;
}


/* =========================================================================
**  Function : Clock_BIMCRestore
** =========================================================================*/
/**
  This function is used for restore BIMC out of collapse.  If the BIMC state is in:
      BIMC_SAVING    - Check if DEHR saving is done, transition to RESTORING state (BIMC_RESTORING)
      BIMC_COLLAPSED - BIMC in collapsed state, transition BIMC to RESTORING state (BIMC_RESTORING)
      BIMC_RESTORING - Called Clock_BIMCIsRestoreDone() for handle transition state when DEHR restore
                       is finished.

  @param 
    None

  @return
    Zero - BIMC is not fully restore yet.
    Non-zero - Frequency that BIMC restored done.

  @dependencies
    None

  @sa
    None
*/
uint32 Clock_BIMCRestore( uint32 nNewClkKHz )
{
  uint32 nNewFreqKHz = 0;

  switch ( Clock_Resources.BIMCResource.eBIMCState )
  {
  case BIMC_SAVING:
    /* In saving, wait until saving is done */
    Clock_BIMCIsSaveDone();

    /* Saving done.  Restore BIMC back */
    Clock_BIMCRestoring(nNewClkKHz);
    
    /* Wait until BIMC complete restore */
    nNewFreqKHz = Clock_BIMCIsRestoreDone();
    break;
    
  case BIMC_COLLAPSED:
    Clock_BIMCRestoring(nNewClkKHz);
    break;

  case BIMC_RESTORING:
    nNewFreqKHz = Clock_BIMCIsRestoreDone();
    break;
  }
  
  return nNewFreqKHz;
}



/*===========================================================================

  FUNCTION      Clock_BIMCSetFrequency

  DESCRIPTION   This function is used for changing BIMC frequency.
  
  PARAMETERS    pClockRsc [in]  - Clock Resource
                nNewClkKHz [in] - New clock speed in KHz
                bNoWait [in] - TRUE - No wait for BIMC to settle for restoring
                             - FALSE - Wait for BIMC to restore completely before use

  DEPENDENCIES  None.

  RETURN VALUE  New clock speed

  SIDE EFFECTS  None.

===========================================================================*/
uint32 Clock_BIMCSetFrequency( ClockResourceType *pBIMCClk, uint32 nNewClkKHz, boolean bNoWait )
{
  /* Switch clock if BIMC is already on, and new requesting frequency is not off (0) */
  if ( ((Clock_Resources.BIMCResource.eBIMCState == BIMC_ON) && (nNewClkKHz != 0)) || (!BIMC_PC_PARALLELIZATION_ENA) )
  {
    return Clock_BIMCSwitchFrequency( pBIMCClk, nNewClkKHz );
  }
  
  /* New frequency is zero, collapsing BIMC */
  if ( nNewClkKHz == 0 )
  { 
    if (Clock_Resources.BIMCResource.eBIMCState == BIMC_ON)
    {
      bimc_pc_debug.clk_dbg_bimc_pc_parallelization_count++;
    }
    return Clock_BIMCollapse();
  }
  /* New frequency is non-zero, restore BIMC */
  else
  {
      if(Clock_Resources.BIMCResource.eBIMCState == BIMC_COLLAPSED)
      {
        bimc_pc_debug.clk_dbg_bimc_pc_parallelization_restore_count++;
      }
      if ( (Clock_BIMCRestore(nNewClkKHz) == 0) && !bNoWait )
      {
        return Clock_BIMCIsRestoreDone();
      }
  }
  return 0;
}


/* =========================================================================
**  Function : Clock_SetBIMCPowerCollapse
** =========================================================================*/
/**
  This function is used for setting BIMC in Power Collapsed Mode.

  @param bCollapsed [in] 
      TRUE - Put DDR in self-refresh, programming freeze IO, and Disable BIMC GDSC
      FALSE - Put DDR in self-refresh only.

  @return
      None
*/

void Clock_SetBIMCPowerCollapse( boolean bCollapsed )
{
  BIMCResourceType  *pBIMCRsc;

  pBIMCRsc = &Clock_Resources.BIMCResource;
  
  /* Save BIMC state for restore after XO restore */
  pBIMCRsc->eXOBIMCState = pBIMCRsc->eBIMCState;

  if ( pBIMCRsc->eBIMCState != BIMC_COLLAPSED )
  {
    Clock_Resources.BIMCResource.bPowerCollapsed = bCollapsed;
    Clock_Resources.BIMCClockResource.nSavLevel = Clock_Resources.BIMCClockResource.nCurLevel;
    Clock_BIMCollapse();
  }
}


/* =========================================================================
**  Function : Clock_WaitForBIMCPowerCollapse()
** =========================================================================*/
/**
  This function is polling BIMC status until BIMC is in collapsed state.

  @param 
      None

  @return
      None
*/

void Clock_WaitForBIMCPowerCollapse( void )
{
  while ( Clock_Resources.BIMCResource.eBIMCState != BIMC_COLLAPSED )
  {
    Clock_BIMCollapse();
    DALSYS_BusyWait(BIMC_WAIT);
  }
}


/* =========================================================================
**  Function : Clock_SetBIMCPowerRestore
** =========================================================================*/
/**
  This function is used for restore BIMC out of Power Collapsed Mode.

  @param 
      None

  @return
      None
*/

void Clock_SetBIMCPowerRestore( void )
{
  uint32            nFreqKHz;
  ClockResourceType *pBIMCClk;

  /* Only restore BIMC if it was ON before XO shutdown */
  if ( Clock_Resources.BIMCResource.eXOBIMCState == BIMC_ON )
  {
    pBIMCClk = &Clock_Resources.BIMCClockResource;
    nFreqKHz = pBIMCClk->pClock->pDomain->pBSPConfig[pBIMCClk->nSavLevel].nFreqHz/1000;
    Clock_BIMCRestore(nFreqKHz);
  }
}


/* =========================================================================
**  Function : Clock_WaitForBIMCPowerRestore()
** =========================================================================*/
/**
  This function is only restore BIMC back if it was ON before XO shutdown.

  @param 
      None

  @return
      None
*/

void Clock_WaitForBIMCPowerRestore( void )
{
  /* Only restore BIMC if it was ON before XO shutdown */
  if ( (Clock_Resources.BIMCResource.eXOBIMCState == BIMC_ON) &&
       (Clock_Resources.BIMCResource.eBIMCState != BIMC_ON) )
  {
    Clock_BIMCIsRestoreDone();
  }
}


/* =========================================================================
**  Function : Clock_IsBIMCPowerCollapsed()
** =========================================================================*/
/**
  This function is checking if the BIMC is in power collapsed before enter vdd min
  or not?

  @param 
      None

  @return
      None
*/
boolean Clock_IsBIMCPowerCollapsed( void )
{
  return ( Clock_Resources.BIMCResource.eBIMCState == BIMC_COLLAPSED );
}


void npa_create_bimc_client(npa_client * client)
{
    if(!(strcmp("APSS", client->name) && strcmp("MPSS", client->name) && strcmp("LPASS", client->name) && strcmp("WCSS", client->name)))
    {
        client->resource_data = (npa_user_data)TRUE;
    }
}


const npa_resource_plugin npa_bimc_max_plugin = 
{
  npa_max_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  npa_create_bimc_client,       /* Create client function */
  NULL                          /* Destroy client function */
};


