/*===========================================================================
  FILE:         vmpm_internal.c
  
  OVERVIEW:     This file contains the necessary functions to interface
                the MPM IC hardware registers.

  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/vmpm_internal.c#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/ 
#include "vmpm_internal.h"
#include "vmpm_HAL.h"
#include "CoreVerify.h"
#include "CoreIntrinsics.h"
#include "vmpm.h"

/*===========================================================================
 *                      STATIC VARIABLE DEFINITIONS
 *===========================================================================*/
static uint32 s_mpmMapTblSize = 0;
static uint32 s_mpmNumMappedIrqs = 0;
static uint32 s_mpmNumMappedGpios = 0;

/*============================================================================
 *                      EXTERNAL FUNCTION DECLARATIONS
 *===========================================================================*/

/**
 * vmpm_TriggerInterrupts
 */
void vmpm_TriggerInterrupts( void )
{
  uint32 numMasks, status, temp, mpmPinNum;
  uint32 gpioNum, irqNum, i;

  vmpm_HAL_GetNumberMasks(&numMasks);

  for(i = 0; i < numMasks; i++)
  {
    vmpm_HAL_GetPending(i, &status);

    temp = status;
    while(temp)
    {
      /* Get the next interrupt's physical id and remove it for the future */
      mpmPinNum = (31 - CoreCountLeadingZeros32(temp)) + (32 * i);
      temp &= ~(1 << (31 - CoreCountLeadingZeros32(temp)));

      /* try to figure out which interrupt it is */
      if(MPM_FAIL == vmpm_IsPinSupported(mpmPinNum))
      {
        continue;
      }

      /* If it's a gpio, perform soft-trigger into that driver */
      if( MPM_SUCCESS == vmpm_getGpioNum( mpmPinNum, &gpioNum ) )
      {
        vmpm_LogPrintf( 4, "Interrupt soft trigger "
                        "(type: GPIO) (num: %u) (bit: %u) "
                        "(pending mask num: %u) (pending mask: 0x%x)",
                        gpioNum,
                        mpmPinNum, 
                        i, 
                        status );
        vmpm_RetriggerGpio( gpioNum );
      }
      /* Else check if it was a wakeup irq */
      else if( MPM_SUCCESS == vmpm_getIrqNum( mpmPinNum, &irqNum ) )
      {
        vmpm_LogPrintf( 4, "Interrupt soft trigger "
                        "(type: IRQ) (num: %u) (bit: %u) "
                        "(pending mask num: %u) (pending mask: 0x%x)",
                        irqNum, 
                        mpmPinNum, 
                        i, 
                        status );

        vmpm_RetriggerIrq( irqNum );
      }
      /* Not a GPIO or wakeup irq - stray signal? */

    }    /* end of while(temp) */
  }

  /* Clearing all interrupts for next cycle */
  vmpm_HAL_All_Clear();
}

/**
 * vmpm_IsGpio
 */
mpm_status_type vmpm_IsGpio( uint32 mpmPinNum )
{
  mpm_status_type status = MPM_FAIL;

  if( ( mpmPinNum < vmpm_getMapTblSize() ) &&
      ( MPM_GPIO == vmpm_pinMapTbl[mpmPinNum].mpm_pin_type )  &&
      ( vmpm_pinMapTbl[mpmPinNum].local_pin_num < MPM_MAX_LOC_IC_VAL ) )
  {
    status = MPM_SUCCESS;
  }
  else
  {
    status = MPM_FAIL;
  }

  return status;
}

/**
 * vmpm_IsIrq
 */
mpm_status_type vmpm_IsIrq( uint32 mpmPinNum )
{
  mpm_status_type status = MPM_FAIL;

  if( ( mpmPinNum < vmpm_getMapTblSize() ) &&
      ( MPM_IRQ == vmpm_pinMapTbl[mpmPinNum].mpm_pin_type ) &&
      ( vmpm_pinMapTbl[mpmPinNum].local_pin_num < MPM_MAX_LOC_IC_VAL ) )
  {
    status = MPM_SUCCESS;
  }
  else
  {
    status = MPM_FAIL;
  }

  return status;
}

/**
 * vmpm_IsPinSupported
 */
mpm_status_type vmpm_IsPinSupported( uint32 mpmPinNum )
{
  if ( (mpmPinNum < vmpm_getMapTblSize()) &&
       (MPM_OPEN != vmpm_pinMapTbl[mpmPinNum].mpm_pin_type) )
  {
    return MPM_SUCCESS;
  }
  else
  {
    return MPM_FAIL;
  }
}

/**
 * vmpm_getIrqNum
 */
mpm_status_type  vmpm_getIrqNum( uint32 nMPM, uint32 *ptr_IrqNum )
{
  mpm_status_type bIsWakeupIRQ = MPM_FAIL;

  if( ( MPM_FAIL == vmpm_IsIrq(nMPM) ) || 
       ( NULL == ptr_IrqNum ) )
  {
    /* Invalid input */
    return bIsWakeupIRQ;
  }

  if( MPM_IRQ == vmpm_pinMapTbl[nMPM].mpm_pin_type )
  {
    /* Given entry in mapping table is a valid IRQ */
    *ptr_IrqNum = vmpm_pinMapTbl[nMPM].local_pin_num;
    bIsWakeupIRQ = MPM_SUCCESS;
  }

  return bIsWakeupIRQ;
}

/**
 * vmpm_getGpioNum
 */
mpm_status_type  vmpm_getGpioNum( uint32 nMPM, uint32 *ptr_GpioNum )
{
  mpm_status_type status = MPM_FAIL;

  if( ( MPM_FAIL == vmpm_IsGpio(nMPM) ) || 
       ( NULL == ptr_GpioNum ) )
  {
    /* Invalid input */
    return status;
  }

  if( MPM_GPIO == vmpm_pinMapTbl[nMPM].mpm_pin_type )
  {
    /* Given entry in mapping table is a valid GPIO */
    *ptr_GpioNum = vmpm_pinMapTbl[nMPM].local_pin_num;
    status = MPM_SUCCESS;
  }

  return status;
}

/**
 * vmpm_setMapTblSize
 */
void vmpm_setMapTblSize( void )
{
  uint32 cnt = 0;
  
  if (NULL == vmpm_pinMapTbl) 
  {
    s_mpmMapTblSize = 0;
    return;
  }

  /* Calculate actual table size */
  while ( MPM_EOT != vmpm_pinMapTbl[cnt].mpm_pin_type )
  { 
    cnt++;
  }
  /* Include MPM_EOT entry in count */
  s_mpmMapTblSize = cnt++;  
}

/**
 * vmpm_getMapTblSize
 */
uint32 vmpm_getMapTblSize( void )
{
  return s_mpmMapTblSize;
}

/**
 * @brief Determines the number of interrupts (IRQs) that are 
 *        mapped to mpm.
 *
 * @return As above
 */
void vmpm_setNumMappedIrqs( void )
{
  uint32 idx = 0;
  uint32 mpmIrqCnt = 0;

  for ( idx = 0; idx < vmpm_getMapTblSize(); idx++ ) 
  {
    if( MPM_SUCCESS == vmpm_IsIrq(idx) )
    {
      mpmIrqCnt++;
    }
  }

  s_mpmNumMappedIrqs = mpmIrqCnt;
}

/**
 * @brief Returns the number of interrupts (IRQs) that are mapped to mpm for
 *        a master processor
 *
 * @return As above
 */
uint32 vmpm_getNumMappedIrqs( void )
{
  return s_mpmNumMappedIrqs;
}

/**
 * @brief Determines the number of GPIOs that are mapped to mpm
 *        for a master processor
 *
 * @return As above
 */
void vmpm_setNumMappedGpios( void )
{
  uint32 idx = 0;
  uint32 mpmGpioCnt = 0;

  for ( idx = 0; idx < vmpm_getMapTblSize(); idx++ ) 
  {
    if( MPM_SUCCESS == vmpm_IsGpio( idx) )
    {
      mpmGpioCnt++;
    }
  }

  s_mpmNumMappedGpios = mpmGpioCnt;
}

/**
 * @brief Returns the number of GPIOs that are mapped to mpm for
 *        a master processor
 *
 * @return As above
 */
uint32 vmpm_getNumMappedGpios( void )
{
  return s_mpmNumMappedGpios;
}

