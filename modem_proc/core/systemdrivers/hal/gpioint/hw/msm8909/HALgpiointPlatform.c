/*
===========================================================================

FILE:         HALgpiointPlatformModem.c

DESCRIPTION:  
  This is the platform hardware abstraction layer implementation for the
  GPIO interrupt controller block.
  This platform is for the Target processor on the MSM8960.

===========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/gpioint/hw/msm8909/HALgpiointPlatform.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
02/20/11   an     Created to unify platform files.

===========================================================================
             Copyright (c) 2011 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include <HALgpioint.h>
#include "HALgpiointInternal.h"
#include <HALhwio.h>
#include "HALgpiointHWIO.h"


/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */


/* ===========================================================================
**  HAL_gpioint_PlatformInitEarly
**
** ======================================================================== */

void HAL_gpioint_PlatformInitEarly (HAL_gpioint_PlatformType *pmPlatform)
{
  pmPlatform->nTotalNumber                         = 113;
  pmPlatform->pmCtl.nSummaryIntrReg                = (uint32)HWIO_TLMM_GPIO_INTR_CFGn_ADDR(0); 
  pmPlatform->pmCtl.nDirConnPolarityCfgReg         = (uint32)HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(0);
  pmPlatform->pmCtl.nDirConnIntrReg                = (uint32)HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(0);                                                     
  pmPlatform->pmCtl.nIntrCfgReg                    = (uint32)HWIO_TLMM_GPIO_INTR_CFGn_ADDR(0);       
  pmPlatform->pmCtl.nStatusReg                     = (uint32)HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(0);                                                                                     
  pmPlatform->pmCtl.nGpioSelBitShift               = HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_SHFT;
  pmPlatform->pmCtl.nEnableMask                    = HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_BMSK;                                                          
  pmPlatform->pmCtl.nPolarityMask                  = HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_BMSK;                                                 
  pmPlatform->pmCtl.nDetectMask                    = HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_BMSK;                                           
  pmPlatform->pmCtl.nTargetProcMask                = HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_BMSK;                                                     
  pmPlatform->pmCtl.nDirConnPolarityMask           = HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_BMSK;                                                     
  pmPlatform->pmCtl.nDirConnGpioSelMask            = HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_BMSK;
  pmPlatform->pmCtl.nDirConnEnMask                 = HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_BMSK; 
  pmPlatform->pmCtl.nRawStatusMask                 = HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_BMSK;
  pmPlatform->pmCtl.nCfgRegOffset                  = 0x1000;                                                             
  pmPlatform->pmCtl.nDirConnRegOffset              = 0x4;
  pmPlatform->pmCtl.nLevelDetectVal                = HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_LEVEL_FVAL;
  pmPlatform->pmCtl.nPositiveEdgeDetectVal         = HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_POS_EDGE_FVAL;
  pmPlatform->pmCtl.nNegativeEdgeDetectVal         = HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_NEG_EDGE_FVAL;
  pmPlatform->pmCtl.nDualEdgeDetectVal             = HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_DUAL_EDGE_FVAL;
  pmPlatform->pmCtl.nDetectShift                   = HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_SHFT;
  pmPlatform->pmCtl.eProcessorID                   = HAL_GPIOINT_MSS_PROC;
} /* END HAL_gpioint_PlatformInitEarly */


/* ===========================================================================
**  HAL_gpioint_GetProcessorId
**
** ======================================================================== */

void HAL_gpioint_GetProcessorId(uint32 nMask, HAL_gpioint_ProcessorType* peProcId)
{
  nMask = (nMask >> 0x5);
  switch (nMask) 
  {
    case 0x0 : 
      *peProcId = HAL_GPIOINT_WCN_PROC;
      break;
    case 0x1 : 
      *peProcId = HAL_GPIOINT_SPS_PROC; 
      break;
    case 0x2 : 
      *peProcId = HAL_GPIOINT_LPA_DSP_PROC;
      break;
    case 0x3 : 
     *peProcId = HAL_GPIOINT_RPM_PROC;
      break;
    case 0x4 : 
      *peProcId = HAL_GPIOINT_SC_PROC;
      break;
    case 0x5 : 
      *peProcId = HAL_GPIOINT_MSS_PROC;
      break;
    default :
      *peProcId = HAL_GPIOINT_NONE_PROC; 
      break;
  }
  return;

} /* HAL_gpioint_GetProcessorId */


/* ===========================================================================
**  HAL_gpioint_GetProcessorMask
**
** ======================================================================== */

void HAL_gpioint_GetProcessorMask(HAL_gpioint_ProcessorType eProcId, uint32* pMask)
{
  switch (eProcId) 
  {
    case HAL_GPIOINT_WCN_PROC :
      *pMask = 0x0;
      break;
    case HAL_GPIOINT_SPS_PROC: 
      *pMask = (0x1<<0x5);
      break;
    case HAL_GPIOINT_LPA_DSP_PROC: 
      *pMask = (0x2<<0x5);
      break;
    case HAL_GPIOINT_RPM_PROC: 
      *pMask = (0x3<<0x5);
      break;
    case HAL_GPIOINT_SC_PROC: 
      *pMask = (0x4<<0x5);
      break;
    case HAL_GPIOINT_MSS_PROC: 
      *pMask = (0x5<<0x5);
      break;
    default:
       *pMask = (0x7<<0x5); 
      break;
  }
  return;

} /* HAL_gpioint_GetProcessorMask */


/* ===========================================================================
**  HAL_gpioint_GetTriggerValue
**
** ======================================================================== */

void HAL_gpioint_GetTriggerValue(uint32  nDetectVal,uint32 nPolarityVal,HAL_gpioint_TriggerType* peTrigger)
{
  switch(nDetectVal)
  {
    case 0 :
       *peTrigger =
       (nPolarityVal ? HAL_GPIOINT_TRIGGER_HIGH : HAL_GPIOINT_TRIGGER_LOW);
      break;
    case 1 :
      *peTrigger = HAL_GPIOINT_TRIGGER_RISING; 
      break;
    case 2 :
      *peTrigger = HAL_GPIOINT_TRIGGER_FALLING;    
      break;
    case 3 :
      *peTrigger = HAL_GPIOINT_TRIGGER_DUAL_EDGE;         
      break;
    default:
      return;
  }

} /* HAL_gpioint_GetTriggerValue */
