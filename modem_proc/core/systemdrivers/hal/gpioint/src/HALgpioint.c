/*
===========================================================================

FILE:         HALgpioint.c

DESCRIPTION:  
  This is the generic hardware abstraction layer implementation for the
  GPIO interrupt controller.

===========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/gpioint/src/HALgpioint.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/19/08   gfr     Use outpdw instead of outpw.
11/16/07   gfr     Include HALhwIO.h to get HW output functions.
09/12/06   gfr     Created.

===========================================================================
             Copyright (c) 2007 -2009 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */


#include <HALhwio.h>
#include <HALgpioint.h>
#include "HALgpiointInternal.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

uint32 nGPIOIntBaseAddress;
uint32 nGPIOIntBaseAddressPhys;


/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */


/*
 * Identifier string for this HAL.
 */
#define HAL_GPIOINT_VERSION_STRING "HAL_GPIOINT_V2"


/*
 * HAL_gpioint_IntType
 *
 * The description of a gpio interrupt.
 *
 *  eTrigger:      trigger type (edge, level, positive, negative)
 *  nController:   index of the first level controller for this interrupt
 *  nMask:         mask to use for this interrupt in the controller registers
 */
typedef struct
{
  HAL_gpioint_TriggerType   eTrigger;
  HAL_gpioint_InterruptType eInterrupt;
} HAL_gpioint_IntType;

/*
 * HAL_gpioint_Type
 *
 * Consolidate of all gpio intctl related data structures
 *
 */
typedef struct
{
  HAL_gpioint_PlatformType mPlatform;
  HAL_gpioint_IntType  amInt[HAL_GPIOINT_NUM];
} HAL_gpioint_Type;

/* -----------------------------------------------------------------------
**                           MACROS
** ----------------------------------------------------------------------- */

/* This macro is used to get the gpio register address */
#define HAL_GPIOINT_GET_ADDRESS(base,nGPIO) (base + \
HAL_gpioint.mPlatform.pmCtl.nCfgRegOffset * (nGPIO))
#define HAL_GPIOINT_GET_DIR_CONN_ADDRESS(base,nGPIO) (base + \
HAL_gpioint.mPlatform.pmCtl.nDirConnRegOffset * (nGPIO))
#define HAL_GPIOINT_IS_DIRCONN(interrupt) \
(interrupt < HAL_GPIOINT_SUMMARY)


/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */


/*
 * Consolidated data.
 */
HAL_gpioint_Type HAL_gpioint;



/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */


/* ===========================================================================
**  HAL_gpioint_GetPending
**
** ======================================================================== */

void HAL_gpioint_GetPending
(
  HAL_gpioint_GroupType eGroup,
  uint32                *pnGPIO
)
{
  uint32 nGPIO,nIdx,nStatus,nStatusReg;
  nGPIO = HAL_GPIOINT_NONE;
  for (nIdx = 0; nIdx < HAL_gpioint.mPlatform.nTotalNumber; nIdx++)
  {
    /*
     * Determine if any interrupts are pending in this register by reading
     * the status register and masking with the enabled register
     */
    
    nStatusReg = HAL_GPIOINT_GET_ADDRESS(HAL_gpioint.mPlatform.pmCtl.nStatusReg,nIdx);
    nStatus    = inpdw(nStatusReg);
    
    /*
     * Check if any interrupts are pending from this register
     */
    if (nStatus != 0)
    {
      if(HAL_gpioint_IsEnabled(nIdx))
      {
        nGPIO = nIdx;
        break;
      }
    }
  }

  /*
   * Save the GPIO for the caller
   */
  *pnGPIO = nGPIO;

} /* END HAL_gpioint_GetPending */



/* ===========================================================================
**  HAL_gpioint_GetNumber
**
** ======================================================================== */

void HAL_gpioint_GetNumber
(
  uint32 *pnNumber
)
{
  /*
   * Get the last GPIO number in the last controller and add one since
   * we include GPIO 0.
   */
  *pnNumber = HAL_gpioint.mPlatform.nTotalNumber;
} /* END HAL_gpioint_GetNumber */



/* ===========================================================================
**  HAL_gpioint_Enable
**
** ======================================================================== */

void HAL_gpioint_Enable
(
  uint32 nGPIO
)
{
  uint32 nMask, nReg, nStatus,nVal;
  HAL_gpioint_InterruptType eDirConnIntr;
  nMask = HAL_gpioint.mPlatform.pmCtl.nEnableMask;
  nReg  = HAL_GPIOINT_GET_ADDRESS(HAL_gpioint.mPlatform.pmCtl.nIntrCfgReg,nGPIO);
  nStatus = HAL_gpioint.mPlatform.pmCtl.nRawStatusMask;
  nVal = inpdw(nReg);
  nVal |= nMask;
  nVal |= nStatus;
  outpdw(nReg, nVal);

  eDirConnIntr = HAL_gpioint.amInt[nGPIO].eInterrupt;
  HAL_gpioint_SetTargetProcessor(HAL_gpioint.mPlatform.eProcessor,nGPIO,eDirConnIntr);
 
} /* END HAL_gpioint_Enable */



/* ===========================================================================
**  HAL_gpioint_Disable
**
** ======================================================================== */

void HAL_gpioint_Disable
(
  uint32 nGPIO
)
{
  uint32 nMask, nReg, nVal;

  nMask = HAL_gpioint.mPlatform.pmCtl.nEnableMask;
  nReg  = HAL_GPIOINT_GET_ADDRESS(HAL_gpioint.mPlatform.pmCtl.nIntrCfgReg,nGPIO);
  
  nVal = inpdw(nReg);
  nVal &= ~nMask;
  outpdw(nReg, nVal);

} /* END HAL_gpioint_Disable */



/* ===========================================================================
**  HAL_gpioint_Clear
**
** ======================================================================== */
void HAL_gpioint_Clear
(
  uint32 nGPIO
)
{
  uint32 nReg,nVal;

  nVal = 0;
  nReg  = HAL_GPIOINT_GET_ADDRESS(HAL_gpioint.mPlatform.pmCtl.nStatusReg,nGPIO);
  
  
  outpdw(nReg, nVal);

} /* END HAL_gpioint_Clear */



/* ===========================================================================
**  HAL_gpioint_IsPending
**
** ======================================================================== */

boolean HAL_gpioint_IsPending
(
  uint32 nGPIO
)
{
  uint32 nStatusReg;
  boolean bPending = FALSE;

  nStatusReg  = HAL_GPIOINT_GET_ADDRESS(HAL_gpioint.mPlatform.pmCtl.nStatusReg,nGPIO);
 
  if(inpdw(nStatusReg))
  {
   if(HAL_gpioint_IsEnabled(nGPIO))
   {    
     bPending = TRUE;
   }
  }

  /*
   * Return if we are set
   */
  return bPending;

} /* END HAL_gpioint_IsPending */


/* ===========================================================================
**  HAL_gpioint_IsSet
**
** ======================================================================== */

boolean HAL_gpioint_IsSet
(
  uint32 nGPIO
)
{
  uint32 nStatusReg;
  boolean bSet = FALSE;

  nStatusReg  = HAL_GPIOINT_GET_ADDRESS(HAL_gpioint.mPlatform.pmCtl.nStatusReg,nGPIO);
 
  if(inpdw(nStatusReg))
  {  
     bSet = TRUE;
  }

  /*
   * Return if we are set.
   */
  return bSet;

} /* END HAL_gpioint_IsSet */


/* ===========================================================================
**  HAL_gpioint_IsEnabled
**
** ======================================================================== */

boolean HAL_gpioint_IsEnabled
(
  uint32 nGPIO
)
{
  HAL_gpioint_ProcessorType eTarget;
  uint32 nEnableMask;
  uint32 nReg;
  boolean bEnabled;

  bEnabled = FALSE;
  nEnableMask = HAL_gpioint.mPlatform.pmCtl.nEnableMask;
  nReg  = HAL_GPIOINT_GET_ADDRESS(HAL_gpioint.mPlatform.pmCtl.nIntrCfgReg,nGPIO);
  HAL_gpioint_GetTargetProcessor(nGPIO, &eTarget);
  if((eTarget  == HAL_gpioint.mPlatform.eProcessor) && (inpdw(nReg) & nEnableMask))
  {
    bEnabled = TRUE;
  }
  /*
   * Return if we are set
   */
  return bEnabled;

} /* END HAL_gpioint_IsEnabled */



/* ===========================================================================
**  HAL_gpioint_IsSupported
**
** ======================================================================== */

boolean HAL_gpioint_IsSupported
(
  uint32 nGPIO
)
{
  /*
   * Return if this interrupt is supported or not
   */
  return (nGPIO < HAL_gpioint.mPlatform.nTotalNumber);

} /* END HAL_gpioint_IsSupported */



/* ===========================================================================
**  HAL_gpioint_SetTrigger
**
** ======================================================================== */
void HAL_gpioint_SetTrigger
(
  uint32                   nGPIO,
  HAL_gpioint_TriggerType  eTrigger
)
{
  uint32 nPolarityMask;
  uint32 nVal;
  uint32 nReg;
 if (HAL_gpioint.amInt[nGPIO].eInterrupt == HAL_GPIOINT_SUMMARY)
 {
    nPolarityMask = HAL_gpioint.mPlatform.pmCtl.nPolarityMask;
    nReg  = HAL_GPIOINT_GET_ADDRESS(HAL_gpioint.mPlatform.pmCtl.nIntrCfgReg,nGPIO);
    nVal  = inpdw(nReg);

    /*
     * clear the detect field.
     */
    nVal &= ~(HAL_gpioint.mPlatform.pmCtl.nDetectMask);
    switch(eTrigger)
    {
      case HAL_GPIOINT_TRIGGER_HIGH :
        nVal |= (HAL_gpioint.mPlatform.pmCtl.nLevelDetectVal << HAL_gpioint.mPlatform.pmCtl.nDetectShift);
        nVal |= nPolarityMask;
        break;
      case HAL_GPIOINT_TRIGGER_LOW :
        nVal |= (HAL_gpioint.mPlatform.pmCtl.nLevelDetectVal << HAL_gpioint.mPlatform.pmCtl.nDetectShift);
        nVal &= ~nPolarityMask;
        break;
      case HAL_GPIOINT_TRIGGER_RISING :
        nVal |= (HAL_gpioint.mPlatform.pmCtl.nPositiveEdgeDetectVal << HAL_gpioint.mPlatform.pmCtl.nDetectShift);
        nVal |= nPolarityMask;
        break;
      case HAL_GPIOINT_TRIGGER_FALLING :
        nVal |= (HAL_gpioint.mPlatform.pmCtl.nNegativeEdgeDetectVal << HAL_gpioint.mPlatform.pmCtl.nDetectShift);
        nVal |= nPolarityMask;
        break;
      case HAL_GPIOINT_TRIGGER_DUAL_EDGE :
        nVal |= (HAL_gpioint.mPlatform.pmCtl.nDualEdgeDetectVal << HAL_gpioint.mPlatform.pmCtl.nDetectShift);

        /*
         *  For dual edge polarity does not matter.
         */
        break;
      default:
        return;
    }

    outpdw(nReg, nVal);

    /*
     * Save the new value
     */
    HAL_gpioint.amInt[nGPIO].eTrigger = eTrigger;

  }

} /* END HAL_gpioint_SetTrigger */

/* ===========================================================================
**  HAL_gpioint_GetTrigger
**
** ======================================================================== */

void HAL_gpioint_GetTrigger
(
  uint32                   nGPIO,
  HAL_gpioint_TriggerType *peTrigger
)
{
  uint32 nReg,nDetectMask, nPolarityMask;
  uint32 nDetectVal, nPolarityVal,nVal;

  nDetectMask = HAL_gpioint.mPlatform.pmCtl.nDetectMask;
  nPolarityMask  = HAL_gpioint.mPlatform.pmCtl.nPolarityMask;
  nReg = HAL_GPIOINT_GET_ADDRESS(HAL_gpioint.mPlatform.pmCtl.nIntrCfgReg,nGPIO);
  nVal = inpdw(nReg);
  nDetectVal = nVal & nDetectMask;
  nPolarityVal = nVal & nPolarityMask;
  nDetectVal >>= HAL_gpioint.mPlatform.pmCtl.nDetectShift;
  HAL_gpioint_GetTriggerValue(nDetectVal,nPolarityVal,peTrigger);

  /*
   * Update the shadow in case it was incorrect
   */
  HAL_gpioint.amInt[nGPIO].eTrigger = *peTrigger;

} /* HAL_gpioint_GetTrigger */


/* ===========================================================================
**  HAL_gpioint_GetGroup
**
** ======================================================================== */

void HAL_gpioint_GetGroup
(
  uint32                 nGPIO,
  HAL_gpioint_GroupType *peGroup
)
{
  *peGroup = HAL_GPIOINT_GROUP1;
} /* HAL_gpioint_GetGroup */



/* ===========================================================================
**  HAL_gpioint_Save
**
** ======================================================================== */

void HAL_gpioint_Save (void)
{
  /*
   * Invoke platform save if required
   */
  if (HAL_gpioint.mPlatform.Save)
  {
    HAL_gpioint.mPlatform.Save();
  }

} /* END HAL_gpioint_Save */



/* ===========================================================================
**  HAL_gpioint_Restore
**
** ======================================================================== */

void HAL_gpioint_Restore (void)
{
  /*
   * Invoke platform restore if required
   */
  if (HAL_gpioint.mPlatform.Restore)
  {
    HAL_gpioint.mPlatform.Restore();
  }

} /* END HAL_gpioint_Restore */



/* ===========================================================================
**  HAL_gpioint_Init
**
** ======================================================================== */

void HAL_gpioint_Init
(
  HAL_gpioint_ContextType * pTarget 
)
{
  HAL_gpioint_TargetDataType * pTargetData;
  uint32 nIdx;
  /*
   * Platform early init, which fills in the mPlatform structure.
   */
  pTargetData = (HAL_gpioint_TargetDataType *)pTarget->pTargetData;
  HAL_gpioint.mPlatform.eProcessor = pTargetData->eProcessor;
  nGPIOIntBaseAddress = (uint32)pTargetData->nGPIOIntBase;
  nGPIOIntBaseAddressPhys = (uint32)pTargetData->nGPIOIntBasePhys;
  HAL_gpioint_PlatformInitEarly(&HAL_gpioint.mPlatform);
  
  for(nIdx = 0; nIdx < HAL_gpioint.mPlatform.nTotalNumber; nIdx++)
  { 
    HAL_gpioint.amInt[nIdx].eTrigger   = HAL_GPIOINT_TRIGGER_HIGH;
    HAL_gpioint.amInt[nIdx].eInterrupt = HAL_GPIOINT_SUMMARY;
  }

  /*
   * Platform late init if it exists
   */
  if (HAL_gpioint.mPlatform.InitLate != NULL)
  {
    HAL_gpioint.mPlatform.InitLate();
  }
} /* END HAL_gpioint_Init */



/* ===========================================================================
**  HAL_gpioint_Reset
**
** ======================================================================== */

void HAL_gpioint_Reset (void)
{
  /* TODO */

} /* END HAL_gpioint_Reset */

/* ===========================================================================
**  HAL_gpioint_SetTargetProcessor
**
** ======================================================================== */

void HAL_gpioint_SetTargetProcessor (HAL_gpioint_ProcessorType eProcId, 
                                     uint32 nGpio, 
                                     HAL_gpioint_InterruptType eInterrupt)
{  
  uint32 nIntrReg, nMask,nVal,nRegBase;
  nMask = HAL_gpioint.mPlatform.pmCtl.nTargetProcMask;
  if(eInterrupt == HAL_GPIOINT_SUMMARY)
  {
    nRegBase = HAL_gpioint.mPlatform.pmCtl.nSummaryIntrReg;
    nIntrReg = 
    HAL_GPIOINT_GET_ADDRESS(nRegBase,nGpio);
    nVal  = inpdw(nIntrReg);
    /* Clear the processor id bits first*/
    nVal &= ~(nMask);
    HAL_gpioint_GetProcessorMask(eProcId, &nMask);
    nVal |= nMask;
    outpdw(nIntrReg, nVal);
  }
} /* END HAL_gpioint_SetTargetProcessor */

/* ===========================================================================
**  HAL_gpioint_SetDirConnIntrPolarity
**
** ======================================================================== */
void HAL_gpioint_SetDirConnIntrPolarity
(
  HAL_gpioint_TriggerType  eTrigger,
  uint32 nGPIO
)
{
  uint32 nPolarityMask,nVal,nReg,eDirConnIntr;
  eDirConnIntr = HAL_gpioint.amInt[nGPIO].eInterrupt;
  if(HAL_GPIOINT_IS_DIRCONN(eDirConnIntr))
  {
    nPolarityMask = HAL_gpioint.mPlatform.pmCtl.nDirConnPolarityMask;
    nReg  = 
    HAL_GPIOINT_GET_DIR_CONN_ADDRESS(HAL_gpioint.mPlatform.pmCtl.nDirConnPolarityCfgReg,
                                  eDirConnIntr);
    nVal  = inpdw(nReg);
    /*
     * Configure the polarity
     */
    if (eTrigger == HAL_GPIOINT_TRIGGER_HIGH ||
      eTrigger == HAL_GPIOINT_TRIGGER_RISING)
    {
      nVal |= nPolarityMask;
    }
    else
    {
      nVal &= ~nPolarityMask;
    }
    outpdw(nReg, nVal);

    /*
     * Save the new value
     */
    HAL_gpioint.amInt[nGPIO].eTrigger = eTrigger;
  }

} /* END HAL_gpioint_SetDirConnPolarity */

/* ===========================================================================
**  HAL_gpioint_SetDirConnInterrupt
**
** ======================================================================== */
void HAL_gpioint_SetDirConnIntr
(
  uint32                   nGPIO,
  HAL_gpioint_InterruptType eDirConnIntr
)
{
  uint32 nReg,nMask,nVal,nRegBase;
  nMask = HAL_gpioint.mPlatform.pmCtl.nDirConnGpioSelMask;
  nRegBase = HAL_gpioint.mPlatform.pmCtl.nDirConnIntrReg;

    /* We need to Set the Direct connect CFG register Gpio select bits*/
    nReg = 
    HAL_GPIOINT_GET_DIR_CONN_ADDRESS(nRegBase,eDirConnIntr);
    nVal  = inpdw(nReg);
    /* Clear the Gpio Sel bits first*/
    nVal &= ~(nMask);
    nVal |= ((nGPIO) << HAL_gpioint.mPlatform.pmCtl.nGpioSelBitShift);
    
    outpdw(nReg, nVal);
    
    /* We need to enable the direct connect 
     * interrupt from the Summary CFG register */
    
    nRegBase = HAL_gpioint.mPlatform.pmCtl.nSummaryIntrReg;
    nReg = 
    HAL_GPIOINT_GET_ADDRESS(nRegBase,nGPIO);
    nMask = HAL_gpioint.mPlatform.pmCtl.nDirConnEnMask;
    nVal  = inpdw(nReg);
    nVal |= nMask;
    outpdw(nReg, nVal);
    HAL_gpioint.amInt[nGPIO].eInterrupt = eDirConnIntr;
}

/* ===========================================================================
**  HAL_gpioint_DisableDirConnIntr
**
** ======================================================================== */
void HAL_gpioint_DisableDirConnIntr(uint32 nGPIO)
{
  uint32 nReg, nMask,nVal,nRegBase;
  HAL_gpioint_InterruptType eDirConnIntr;
  nMask = HAL_gpioint.mPlatform.pmCtl.nDirConnGpioSelMask;
  nRegBase = HAL_gpioint.mPlatform.pmCtl.nDirConnIntrReg;

  eDirConnIntr = HAL_gpioint.amInt[nGPIO].eInterrupt;

  if(HAL_GPIOINT_IS_DIRCONN(eDirConnIntr))
  {
    /* We need to clear the Direct connect register Gpio select bits*/
    nReg = 
    HAL_GPIOINT_GET_DIR_CONN_ADDRESS(nRegBase,eDirConnIntr);
    nVal  = inpdw(nReg);
    /* Clear the Gpio Sel bits first*/
    nVal &= ~(nMask);
    outpdw(nReg, nVal);

    /* We need to clear the direct connect 
     * interrupt from the Summary register */
  }
  nRegBase = HAL_gpioint.mPlatform.pmCtl.nSummaryIntrReg;
  nReg = 
  HAL_GPIOINT_GET_ADDRESS(nRegBase,nGPIO);
  nMask = HAL_gpioint.mPlatform.pmCtl.nDirConnEnMask;
  nVal  = inpdw(nReg);
  nVal &= ~(nMask);
  outpdw(nReg, nVal);
  HAL_gpioint.amInt[nGPIO].eInterrupt = HAL_GPIOINT_SUMMARY;  
  HAL_gpioint_Disable(nGPIO);
}


/* ===========================================================================
**  HAL_gpioint_GetTargetProcessor
**
** ======================================================================== */

void  HAL_gpioint_GetTargetProcessor (uint32 nGpio, HAL_gpioint_ProcessorType * peProcId)
{  
  uint32 nIntrReg, nMask,nVal,nRegBase;
  HAL_gpioint_ProcessorType eProcessorId;
  HAL_gpioint_InterruptType eInterrupt;
  eInterrupt = HAL_gpioint.amInt[nGpio].eInterrupt;
  nMask = HAL_gpioint.mPlatform.pmCtl.nTargetProcMask;
  if(eInterrupt == HAL_GPIOINT_SUMMARY)
  {
    nRegBase = HAL_gpioint.mPlatform.pmCtl.nSummaryIntrReg;
    nIntrReg = 
    HAL_GPIOINT_GET_ADDRESS(nRegBase,nGpio);
    nVal  = inpdw(nIntrReg);
    /* Clear the processor id bits first*/
    nVal &= nMask;
    HAL_gpioint_GetProcessorId(nVal,&eProcessorId);
    *peProcId = eProcessorId;
  }
  else
  {
    *peProcId = HAL_gpioint.mPlatform.pmCtl.eProcessorID;
  }

  return;
 
} /* END HAL_gpioint_GetTargetProcessor */

/* ===========================================================================
**  HAL_gpioint_SetTrigger
**
** ======================================================================== */
void HAL_gpioint_TriggerInterrupt
(
  uint32                   nGPIO
)
{
  uint32 nStatusReg;
  nStatusReg = HAL_GPIOINT_GET_ADDRESS(HAL_gpioint.mPlatform.pmCtl.nStatusReg,nGPIO);
  outpdw(nStatusReg, 0x1);

} /* END HAL_gpioint_TriggerInterrupt */

/* ===========================================================================
**  HAL_gpioint_EnableRawStatus
**
** ======================================================================== */
void HAL_gpioint_EnableRawStatus
(
  uint32 nGPIO
)
{
  uint32 nReg, nRawStatusMask,nVal;

  nReg  = HAL_GPIOINT_GET_ADDRESS(HAL_gpioint.mPlatform.pmCtl.nIntrCfgReg,nGPIO);
  nRawStatusMask = HAL_gpioint.mPlatform.pmCtl.nRawStatusMask;
  nVal = inpdw(nReg);
  nVal |= nRawStatusMask;
  outpdw(nReg, nVal);

} /* END HAL_gpioint_EnableRawStatus */

