/*
==============================================================================

FILE:         HALclkGeneric.c

DESCRIPTION:
   This file contains common clock HAL code for the Badger family of chipsets.


==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/v2/HALclkGeneric.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
08/21/12   vph     Set root enable (ROOT_EN bit on) during clock config to
                   prevent glitching since all the clock branches may turn off.
07/12/11   vs      Created

==============================================================================
            Copyright © 2011-2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include <HALhwio.h>

#include "HALclkInternal.h"
#include "HALclkGeneric.h"



/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Prototypes
** ==========================================================================*/


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * HAL_clk_mGenericClockControl
 *
 * Functions for controlling common clock functions.
 */
HAL_clk_ClockControlType HAL_clk_mGenericClockControl =
{
  /* .Enable           = */ HAL_clk_GenericEnable,
  /* .Disable          = */ HAL_clk_GenericDisable,
  /* .IsEnabled        = */ HAL_clk_GenericIsEnabled,
  /* .IsOn             = */ HAL_clk_GenericIsOn,
  /* .Reset            = */ HAL_clk_GenericReset,
  /* .Config           = */ NULL,
  /* .DetectConfig     = */ NULL,
  /* .ConfigDivider    = */ HAL_clk_GenericConfigDivider,
  /* .DetectDivider    = */ HAL_clk_GenericDetectDivider,
  /* .ConfigFootswitch = */ HAL_clk_GenericConfigFootswitch,
};


/*
 * HAL_clk_mGenericPowerDomainControl
 *
 * Functions for controlling common power domain functions.
 */
HAL_clk_PowerDomainControlType HAL_clk_mGenericPowerDomainControl =
{
   /* .Enable     = */ HAL_clk_GenericPowerDomainEnable,
   /* .Disable    = */ HAL_clk_GenericPowerDomainDisable,
   /* .IsEnabled  = */ HAL_clk_GenericPowerDomainIsEnabled
};


/*============================================================================= 
 
                        SS specific clock domain controls
 
=============================================================================*/


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_GenericEnable
**
** ======================================================================== */

void HAL_clk_GenericEnable
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  uint32 nAddr, nVal, nMask;

  /*
   * Check for HW votable registers
   */
  nAddr = pmClockDesc->mRegisters.mHWVotable.nAddr;
  if(nAddr)
  {
    nMask = pmClockDesc->mRegisters.mHWVotable.nMask;
  }
  else
  {
    nAddr = pmClockDesc->mRegisters.nBranchAddr;
    nMask = HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_FMSK;
  }

  /*
   * Sanity check
   */
  if(nAddr == 0)
  {
    return;
  }

  /*
   * Enable clock 
   */
  nVal  = inpdw(nAddr) | nMask;
  outpdw(nAddr, nVal);

} /* HAL_clk_GenericEnable */


/* ===========================================================================
**  HAL_clk_GenericDisable
**
** ======================================================================== */

void HAL_clk_GenericDisable
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  uint32 nAddr, nVal, nMask;

  /*
   * Check for HW votable registers
   */
  nAddr = pmClockDesc->mRegisters.mHWVotable.nAddr;
  if(nAddr)
  {
    nMask = pmClockDesc->mRegisters.mHWVotable.nMask;
  }
  else
  {
    nAddr = pmClockDesc->mRegisters.nBranchAddr;
    nMask = HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_FMSK;
  }

  /*
   * Sanity check
   */
  if(nAddr == 0)
  {
    return;
  }

  /*
   * Disable clock
   */
  nVal  = inpdw(nAddr);
  nVal &= ~nMask;
  outpdw(nAddr, nVal);
  
} /* HAL_clk_GenericDisable */


/* ===========================================================================
**  HAL_clk_GenericIsEnabled
**
** ======================================================================== */

boolean HAL_clk_GenericIsEnabled
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  uint32 nAddr, nMask;
  nAddr = pmClockDesc->mRegisters.mHWVotable.nAddr;

  if(nAddr != 0)
  {
    nMask = pmClockDesc->mRegisters.mHWVotable.nMask;
  }
  else
  {
    nAddr = pmClockDesc->mRegisters.nBranchAddr;
    nMask = HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_FMSK;
  }

  if (nAddr && (inpdw(nAddr) & nMask))
  {
    return TRUE;
  }

  return FALSE;

} /* HAL_clk_GenericIsEnabled */


/* ===========================================================================
**  HAL_clk_GenericIsOn
**
** ======================================================================== */

boolean HAL_clk_GenericIsOn
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  uint32 nAddr,nVal;
  nAddr = pmClockDesc->mRegisters.nBranchAddr;

  /*
   * Sanity check
   */
  if(nAddr == 0)
  {
    return FALSE;
  }

  nVal  = inpdw(nAddr);
  if(nVal & HAL_CLK_BRANCH_CTRL_REG_CLK_OFF_FMSK)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }

} /* HAL_clk_GenericIsOn */


/* ===========================================================================
**  HAL_clk_GenericReset
**
** ======================================================================== */

void HAL_clk_GenericReset
(
  HAL_clk_ClockDescType *pmClockDesc,
  boolean                bAssert
)
{
  uint32 nAddr,nVal;
  nAddr = pmClockDesc->mRegisters.nResetAddr;

  /*
   * Sanity check
   */
  if(nAddr == 0)
  {
    return;
  }

  /*
   * Read current value
   */
  nVal  = inpdw(nAddr);
  if (bAssert)
  {
    nVal |= HAL_CLK_BLOCK_CTRL_REG_BLK_ARES_FMSK;
  }
  else
  {
    nVal &= ~HAL_CLK_BLOCK_CTRL_REG_BLK_ARES_FMSK;
  }

  /*
   * Write updated value
   */
  outpdw(nAddr, nVal);

} /* HAL_clk_GenericReset */


/* ===========================================================================
**  HAL_clk_GenericConfigDivider
**
** ======================================================================== */

void HAL_clk_GenericConfigDivider
(
  HAL_clk_ClockDescType *pmClockDesc,
  uint32                nDiv
)
{
  uint32 nAddr,nVal;
  nAddr = pmClockDesc->mRegisters.nBranchAddr;

  /*
   * Sanity check
   */
  if(nAddr == 0)
  {
    return;
  }
  
  nVal  = inpdw(nAddr);
  if(nDiv <= 1)
  {
    nDiv = 0;
  }
  else
  {
    nDiv--;
  }

  /*
   * Clear previous divider value.
   */
  nVal &= ~HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_FMSK;

  /*
   * Set divider value.
   */
  nVal |= ((nDiv << HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_SHFT) 
            & HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_FMSK);

  /*
   * Write final value.
   */
  outpdw(nAddr, nVal);

} /* END HAL_clk_GenericConfigDivider */


/* ===========================================================================
**  HAL_clk_GenericDetectDivider
**
** ======================================================================== */

void HAL_clk_GenericDetectDivider
(
  HAL_clk_ClockDescType *pmClockDesc,
  uint32                *pnDiv
)
{
  uint32 nAddr, nVal;
  nAddr = pmClockDesc->mRegisters.nBranchAddr;
  *pnDiv = 1;

  /*
   * Sanity check
   */
  if(nAddr == 0)
  {
    return;
  }

  nVal     = inpdw(nAddr);
  *pnDiv = ((nVal & HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_FMSK) 
               >> HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_SHFT) + 1;

} /* END HAL_clk_GenericDetectDivider */


/* ===========================================================================
**  HAL_clk_GenericConfigFootswitch
**
** ======================================================================== */

void HAL_clk_GenericConfigFootswitch 
( 
  HAL_clk_ClockDescType        *pmClockDesc, 
  HAL_clk_FootswitchConfigType eConfig
)
{
  uint32 nAddr,nVal;
  nAddr = pmClockDesc->mRegisters.nBranchAddr;

  /*
   * Sanity check
   */
  if(nAddr == 0)
  {
    return;
  }

  nVal  = inpdw(nAddr);
  if (eConfig == HAL_CLK_FOOTSWITCH_CONFIG_CORE_FORCE_ON)
  {    
    nVal |= HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_CORE_ON_FMSK;
  }
  else if (eConfig == HAL_CLK_FOOTSWITCH_CONFIG_CORE_ALLOW_OFF)
  {    
    nVal &= ~HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_CORE_ON_FMSK;
  }
  else if(eConfig == HAL_CLK_FOOTSWITCH_CONFIG_PERIPHERAL_FORCE_ON)
  {    
    nVal |= HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_PERIPH_ON_FMSK;
  }
  else if(eConfig == HAL_CLK_FOOTSWITCH_CONFIG_PERIPHERAL_ALLOW_OFF)
  {    
    nVal &= ~HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_PERIPH_ON_FMSK;
  }
  else if (eConfig == HAL_CLK_FOOTSWITCH_CONFIG_PERIPHERAL_FORCE_OFF)
  {    
    nVal |= HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_PERIPH_OFF_FMSK;
  }
  else
  {    
    return;
  }

  outpdw(nAddr, nVal);

} /* END HAL_clk_GenericConfigFootswitch */


/* ===========================================================================
**  HAL_clk_GenericConfigMux
**
** ======================================================================== */

void HAL_clk_GenericConfigMux
(
  HAL_clk_ClockDomainDescType      *pmClockDomainDesc,
  const HAL_clk_ClockMuxConfigType *pmConfig
)
{
  uint32                 nCmdCGRAddr, nCmdCGRVal;
  uint32                 nCfgCGRAddr, nCfgCGRVal;
  uint32                 nMAddr, nNAddr, nDAddr;
  uint32                 nSourceIndex = 0;
  HAL_clk_SourceMapType *pmSourceMap;


  /*
   * Sanity check
   */
  if((pmConfig                                    == NULL) ||
     (pmClockDomainDesc                           == NULL) ||
     (pmClockDomainDesc->nCGRAddr                 == 0   ) || 
     (pmClockDomainDesc->pmControl                == NULL) ||
     (pmClockDomainDesc->pmControl->pmSourceMap   == NULL) )
  {
    return;
  }

  /*
   * Get current CMD and CFG register values
   */
  nCmdCGRAddr = pmClockDomainDesc->nCGRAddr;
  nCmdCGRVal  = inpdw(nCmdCGRAddr);
  nCfgCGRAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_CFG_REG_OFFSET;
  nCfgCGRVal  = inpdw(nCfgCGRAddr);

  /*
   * Clear the fields
   */
  nCfgCGRVal &= ~(HAL_CLK_CFG_CGR_SRC_SEL_FMSK |
                  HAL_CLK_CFG_CGR_SRC_DIV_FMSK |
                  HAL_CLK_CFG_CGR_MODE_FMSK);

  /*
   * Get source index from source enum
   */
  pmSourceMap = pmClockDomainDesc->pmControl->pmSourceMap;
  nSourceIndex = HAL_clk_GenericSourceMapToHW(pmSourceMap, pmConfig->eSource);

  /*
   * Bail if could not find matching source index
   */
  if (nSourceIndex == HAL_CLK_SOURCE_INDEX_INVALID)
  {
    return;
  }

  /*
   * Program the source and divider values.
   */
  nCfgCGRVal |= ((nSourceIndex << HAL_CLK_CFG_CGR_SRC_SEL_SHFT) & HAL_CLK_CFG_CGR_SRC_SEL_FMSK);
  nCfgCGRVal |= ((HALF_DIVIDER(pmConfig) << HAL_CLK_CFG_CGR_SRC_DIV_SHFT) 
                  & HAL_CLK_CFG_CGR_SRC_DIV_FMSK);

  /*
   * Set MND counter mode depending on if it is in use
   */
  if (pmConfig->nM != 0 && (pmConfig->nM < pmConfig->nN))
  {    
    /*
     * Get M, N and D addresses
     */
    nMAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_M_REG_OFFSET;
    nNAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_N_REG_OFFSET;
    nDAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_D_REG_OFFSET;

    /*
     * Set M value
     */
    outpdw(nMAddr, pmConfig->nM);

    /*
     * Set N value
     */
    outpdw(nNAddr, NOT_N_MINUS_M(pmConfig));

    /*
     * Set D value
     */
    outpdw(nDAddr, NOT_2D(pmConfig));

    /*
     * Dual-edge mode
     */
    nCfgCGRVal |= ((HAL_CLK_CFG_CFG_DUAL_EDGE_MODE_VAL << HAL_CLK_CFG_CGR_MODE_SHFT) 
                    & HAL_CLK_CFG_CGR_MODE_FMSK);
  }

  /*
   * Write the final CFG register value
   */
  outpdw(nCfgCGRAddr, nCfgCGRVal);

  /*
   * Set ROOT_EN on for prevent glitch during switching since all the branch may be off.
   * This change applies to RPM image only.  Don't merge to other images.
   */
  nCmdCGRVal |= HAL_CLK_CMD_CGR_ROOT_EN_FMSK;
  outpdw(nCmdCGRAddr, nCmdCGRVal);

  /*
   * Wait for the root to enable.
   */
  while(inpdw(nCmdCGRAddr) & HAL_CLK_CMD_CGR_ROOT_OFF_FMSK);

  /*
   * Trigger the update
   */
  nCmdCGRVal |= HAL_CLK_CMD_CFG_UPDATE_FMSK;
  outpdw(nCmdCGRAddr, nCmdCGRVal);

  /*
   * Wait until update finishes
   */
  while(inpdw(nCmdCGRAddr) & HAL_CLK_CMD_CFG_UPDATE_FMSK);

  /*
   * Update success, clear ROOT_EN
   */
  nCmdCGRVal = inpdw(nCmdCGRAddr);
  nCmdCGRVal &= ~HAL_CLK_CMD_CGR_ROOT_EN_FMSK;

  outpdw(nCmdCGRAddr, nCmdCGRVal);

} /* HAL_clk_GenericConfigMux */


/* ===========================================================================
**  HAL_clk_GenericDetectMuxConfig
**
** ======================================================================== */

void HAL_clk_GenericDetectMuxConfig
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig
)
{
  uint32                 nCfgCGRAddr, nCfgCGRVal, nSourceIndex;
  HAL_clk_SourceType     eSource;
  HAL_clk_SourceMapType *pmSourceMap;

  if (pmConfig == NULL)
  {
    return;
  }

  pmConfig->eSource = HAL_CLK_SOURCE_NULL;
  pmConfig->nDiv2x  = 0;
  pmConfig->n2D     = 0;
  pmConfig->nM      = 0;
  pmConfig->nN      = 0;

  /*
   * Sanity check
   */
  if((pmClockDomainDesc                           == NULL) ||
     (pmClockDomainDesc->nCGRAddr                 == 0   ) ||
     (pmClockDomainDesc->pmControl                == NULL) ||
     (pmClockDomainDesc->pmControl->pmSourceMap   == NULL) )
  {
    return;
  }

  /*
   * NOTES: 
   *  
   * -- pmSourceMap returns value based on the "common" mapping.  Individual
   *    HAL file may need to implement its own mapping if common one is too
   *    generic.
   *  
   * -- Generic DetectMuxConfig only detects source and divider values. 
   *    Individual SS HAL file will need to implement its own DetectMuxConfig
   *    for a proper detection of M, N, and D values.
   */

  /*
   * Get current CMD and CFG register values
   */
  nCfgCGRAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_CFG_REG_OFFSET;
  nCfgCGRVal  = inpdw(nCfgCGRAddr);

  /*
   * Get the source
   */
  nSourceIndex = (nCfgCGRVal & HAL_CLK_CFG_CGR_SRC_SEL_FMSK) >> HAL_CLK_CFG_CGR_SRC_SEL_SHFT;

  /*
   * Get source enum from source index
   */
  pmSourceMap = pmClockDomainDesc->pmControl->pmSourceMap;
  eSource = HAL_clk_GenericSourceMapFromHW(pmSourceMap, nSourceIndex);

  /*
   * Bail if could not find matching source enum
   */
  if (eSource == HAL_CLK_SOURCE_NULL)
  {
    return;
  }

  pmConfig->eSource = eSource;

  /*
   * Get the divider
   */
  pmConfig->nDiv2x = 
    ((nCfgCGRVal & HAL_CLK_CFG_CGR_SRC_DIV_FMSK) >> HAL_CLK_CFG_CGR_SRC_DIV_SHFT) + 1;

} /* END HAL_clk_GenericDetectMuxConfig */


/* ===========================================================================
**  HAL_clk_GenericPowerDomainEnable
**
** ======================================================================== */

void HAL_clk_GenericPowerDomainEnable
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc, 
  boolean                     bAsync
)
{
  uint32 nAddr,nVal;
  nAddr = pmPowerDomainDesc->nGDSCRAddr;

  /*
   * Sanity check
   */
  if(nAddr == 0)
  {
    return;
  }

  nVal  = inpdw(nAddr);

  /*
   * Clear the SW PD collapse bit
   */
  nVal &= ~HAL_CLK_GDSCR_SW_COLLAPSE_FMSK;
  outpdw(nAddr, nVal);

  /*
   * Wait for PD ON, only for synchronous function call
   */
  if ( bAsync != TRUE )
  {
    while( !(inpdw(nAddr) & HAL_CLK_GDSCR_PWR_ON_FMSK) );
  }

} /* END HAL_clk_GenericPowerDomainEnable */


/* ===========================================================================
**  HAL_clk_GenericPowerDomainDisable
**
** ======================================================================== */

void HAL_clk_GenericPowerDomainDisable
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc
)
{
  uint32 nAddr,nVal;
  nAddr = pmPowerDomainDesc->nGDSCRAddr;

  /*
   * Sanity check
   */
  if(nAddr == 0)
  {
    return;
  }

  nVal  = inpdw(nAddr);

  /*
   * Set the SW PD collapse bit
   */
  nVal |= HAL_CLK_GDSCR_SW_COLLAPSE_FMSK;
  outpdw(nAddr, nVal);

} /* END HAL_clk_GenericPowerDomainDisable */


/* ===========================================================================
**  HAL_clk_GenericPowerDomainIsEnabled
**
** ======================================================================== */

boolean HAL_clk_GenericPowerDomainIsEnabled
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc
)
{
  uint32 nAddr,nVal;
  nAddr = pmPowerDomainDesc->nGDSCRAddr;

  /*
   * Sanity check
   */
  if(nAddr == 0)
  {
    return FALSE;
  }

  nVal  = inpdw(nAddr);

  if(nVal & HAL_CLK_GDSCR_SW_COLLAPSE_FMSK)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }

} /* END HAL_clk_GenericPowerDomainIsEnabled */


/* ===========================================================================
**  HAL_clk_GenericPowerDomainIsOn
**
** ======================================================================== */

boolean HAL_clk_GenericPowerDomainIsOn
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc
)
{
  uint32 nAddr,nVal;
  nAddr = pmPowerDomainDesc->nGDSCRAddr;

  /*
   * Sanity check
   */
  if(nAddr == 0)
  {
    return FALSE;
  }

  nVal  = inpdw(nAddr);

  if(nVal & HAL_CLK_GDSCR_PWR_ON_FMSK)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* END HAL_clk_GenericPowerDomainIsEnabled */


/* ===========================================================================
**  HAL_clk_GenericSourceMapToHW
**
** ======================================================================== */

uint32 HAL_clk_GenericSourceMapToHW
(
  HAL_clk_SourceMapType  *pmSourceMap,
  HAL_clk_SourceType      eSource
)
{
  while (pmSourceMap->eSource != HAL_CLK_SOURCE_NULL)
  {
    /*
     * Found the source in the mapping array
     */
    if (eSource == pmSourceMap->eSource)
    {
      return pmSourceMap->nSourceIndex;
    }
    pmSourceMap++;
  }

  /*
   * No matching source index found. Return INVALID index.
   */
  return HAL_CLK_SOURCE_INDEX_INVALID;
} /* END HAL_clk_GenericSourceMapToHW */


/* ===========================================================================
**  HAL_clk_GenericSourceMapFromHW
**
** ======================================================================== */

HAL_clk_SourceType HAL_clk_GenericSourceMapFromHW
(
  HAL_clk_SourceMapType  *pmSourceMap,
  uint32                  nSourceIndex
)
{
  while (pmSourceMap->eSource != HAL_CLK_SOURCE_NULL)
  {
    /*
     * Found the source index in the mapping array
     */
    if (nSourceIndex == pmSourceMap->nSourceIndex)
    {
      return pmSourceMap->eSource;
    }
    pmSourceMap++;
  }

  /*
   * No matching source index found. Return NULL source.
   */
  return HAL_CLK_SOURCE_NULL;
} /* END HAL_clk_GenericSourceMapFromHW */

