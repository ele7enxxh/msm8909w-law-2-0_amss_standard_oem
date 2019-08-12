/*===========================================================================
  FILE:         vmpm.c
  
  OVERVIEW:     This file contains the necessary MPM API functions to interface
                to the Virtual MPM (vMPM) driver

  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/vmpm.c#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/ 

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "vmpm_internal.h"
#include "vmpm_HAL.h"
#include "CoreVerify.h"
#include "CoreIntrinsics.h"
    
/*===========================================================================
 *                         CONST TYPE DEFINITIONS
 *==========================================================================*/
/** 
 *  Max number of MPM2 interrupts
 */
const uint32 MPM_MAX_NUM_INTS = 64;

/**
 *  Max value of valid Local IC Pin Number
 */
const uint32 MPM_MAX_LOC_IC_VAL = 255;

/*============================================================================
 *                      INTERNAL FUNCTION DECLARATIONS
 *===========================================================================*/

/*============================================================================
 *                      EXTERNAL FUNCTION DECLARATIONS
 *===========================================================================*/
/**
 *  vmpm_configAndEnablePin
 */
mpm_status_type vmpm_configAndEnablePin( uint32 mpmPinNum,
                                         mpm_pin_config_type *ptrConfigInfo )
{
  mpm_status_type status = MPM_FAIL;
  char * pinTypeString = "";

  if (NULL == ptrConfigInfo) 
  {
    return MPM_FAIL;
  }

  if( MPM_SUCCESS == vmpm_IsGpio(mpmPinNum) )
  {
    pinTypeString = "GPIO";
    status = MPM_SUCCESS;
  }
  else if ( MPM_SUCCESS == vmpm_IsIrq(mpmPinNum) ) 
  {
    pinTypeString = "IRQ";
    status = MPM_SUCCESS;
  }
  else
  {
    status = MPM_FAIL;
  }

  if (MPM_SUCCESS == status)
  {
    vmpm_LogPrintf( 4, "Configuring %s MPM ID: %u" 
                    "(Local Pin ID: %u)  (trig type: %d)",
                    pinTypeString,
                    mpmPinNum, 
                    vmpm_pinMapTbl[mpmPinNum].local_pin_num, 
                    ptrConfigInfo->trig_config); 
 
    VMPM_LOCK();
    vmpm_HAL_SetTrigger( mpmPinNum, 
                         ptrConfigInfo->trig_config );

    vmpm_HAL_Enable(mpmPinNum);
    VMPM_UNLOCK();
  }
  else
  {
    vmpm_LogPrintf( 1, "WARNING: Invalid MPM Pin config request (MPM Pin Num: %u)",
                    mpmPinNum );
  }

   return status;
}

/**
 *  vmpm_configPin
 */
mpm_status_type vmpm_configPin( uint32 mpmPinNum,
                                mpm_pin_config_type *ptrConfigInfo )
{
  mpm_status_type status = MPM_FAIL;
  char * pinTypeString = "";

  if (NULL == ptrConfigInfo) 
  {
    return MPM_FAIL;
  }

  if( MPM_SUCCESS == vmpm_IsGpio(mpmPinNum) )
  {
    pinTypeString = "GPIO";
    status = MPM_SUCCESS;
  }
  else if ( MPM_SUCCESS == vmpm_IsIrq(mpmPinNum) ) 
  {
    pinTypeString = "IRQ";
    status = MPM_SUCCESS;
  }
  else
  {
    status = MPM_FAIL;
  }

  if( MPM_SUCCESS == status )
  {
    vmpm_LogPrintf( 4, "Configuring %s MPM ID: %u" 
                    "(Local Pin ID: %u)  (trig type: %d)",
                    pinTypeString,
                    mpmPinNum, 
                    vmpm_pinMapTbl[mpmPinNum].local_pin_num, 
                    ptrConfigInfo->trig_config); 
 
    VMPM_LOCK();
    vmpm_HAL_SetTrigger( mpmPinNum, 
                         ptrConfigInfo->trig_config );
    VMPM_UNLOCK();
  }
  else
  {
    vmpm_LogPrintf( 1, "WARNING: Invalid MPM Pin config request (MPM Pin Num: %u)",
                    mpmPinNum );
  }

   return status;
}

/**
 *  vmpm_enablePin
 */
mpm_status_type vmpm_enablePin( uint32 mpmPinNum )
{
  mpm_status_type status = MPM_FAIL;
  char * pinTypeString = "";

  if( MPM_SUCCESS == vmpm_IsGpio(mpmPinNum) )
  {
    pinTypeString = "GPIO";
    status = MPM_SUCCESS;
  }
  else if ( MPM_SUCCESS == vmpm_IsIrq(mpmPinNum) ) 
  {
    pinTypeString = "IRQ";
    status = MPM_SUCCESS;
  }
  else
  {
    status = MPM_FAIL;
  }

  if( MPM_SUCCESS == status )
  {
    vmpm_LogPrintf( 3, "Enable %s Pin (MPM Pin Num: %u) "
                    "(Local Pin ID: %u)", 
                    pinTypeString,
                    mpmPinNum, 
                    vmpm_pinMapTbl[mpmPinNum].local_pin_num );
 
    VMPM_LOCK();
    vmpm_HAL_Enable(mpmPinNum);
    VMPM_UNLOCK();

    status = MPM_SUCCESS;
  }
  else
  {
    vmpm_LogPrintf( 1, "WARNING: Invalid MPM Pin config request (MPM Pin Num: %u)",
                    mpmPinNum );
  }

   return status;
}

/**
 *  vmpm_disablePin
 */
mpm_status_type vmpm_disablePin( uint32 mpmPinNum )
{
  mpm_status_type status = MPM_FAIL;
  char * pinTypeString = "";

  if( MPM_SUCCESS == vmpm_IsGpio(mpmPinNum) )
  {
    pinTypeString = "GPIO";
    status = MPM_SUCCESS;
  }
  else if ( MPM_SUCCESS == vmpm_IsIrq(mpmPinNum) ) 
  {
    pinTypeString = "IRQ";
    status = MPM_SUCCESS;
  }
  else
  {
    status = MPM_FAIL;
  }

  if( MPM_SUCCESS == status )
  {
    vmpm_LogPrintf( 3, "Disable %s Pin (MPM Pin Num: %u) "
                    "(Local Pin ID: %u)", 
                    pinTypeString,
                    mpmPinNum, 
                    vmpm_pinMapTbl[mpmPinNum].local_pin_num );
 
    VMPM_LOCK();
    vmpm_HAL_Disable(mpmPinNum);
    VMPM_UNLOCK();

  }
  else
  {
    vmpm_LogPrintf( 1, "WARNING: Invalid MPM Pin config request (MPM Pin Num: %u)",
                    mpmPinNum );
  }

   return status;
}

/**
 *  vmpm_queryPinMapping
 */
mpm_status_type vmpm_queryPinMapping( uint32 localPinNum,
                                      mpm_pin_type pinType,
                                      uint32 *ptr_mpmPinNum )
{
   mpm_status_type status = MPM_FAIL;
  uint32 tblIdx;
  uint32 maxTblEntryNum = 0;

  if (NULL == ptr_mpmPinNum) 
  {
    return MPM_FAIL;
  }

  /* Ensure local pin number is valid */
  if ( !(localPinNum < MPM_MAX_LOC_IC_VAL)  )
  {
    vmpm_LogPrintf( 1, "WARNING: Invalid local pin number query (Local Pin Num: %u)",
                    localPinNum );

    return MPM_FAIL;
  }

  maxTblEntryNum = vmpm_getMapTblSize();

  /* Check to see if this local pin ID is supported as a IRQ or GPIO, and if so, 
   * return the MPM Pin Number  that it corresponds to. */ 
  for ( tblIdx = 0; tblIdx < maxTblEntryNum; tblIdx++ )
  {
    if ( ( vmpm_pinMapTbl[tblIdx].local_pin_num == localPinNum ) &&
         ( vmpm_pinMapTbl[tblIdx].mpm_pin_type == pinType  ) && 
         ( vmpm_pinMapTbl[tblIdx].mpm_pin_type != MPM_OPEN  ) )
    {
      break;
    }
  }
  if (  tblIdx < maxTblEntryNum )
  {
    *ptr_mpmPinNum = tblIdx;
    status = MPM_SUCCESS;
  }

   return status;
}

/**
 *  vmpm_queryPinConfigInfo
 */
mpm_status_type vmpm_queryPinConfigInfo ( uint32                  mpmPinNum, 
                                          mpm_pin_query_info_type *ptr_mpmPinInfo )
{
  mpm_status_type status = MPM_FAIL;
   
  if (NULL == ptr_mpmPinInfo) 
  {
    return MPM_FAIL;
  }

  if( MPM_SUCCESS == vmpm_IsPinSupported(mpmPinNum) )
  {
    ptr_mpmPinInfo->local_pin_num = vmpm_pinMapTbl[mpmPinNum].local_pin_num;
    ptr_mpmPinInfo->pin_type = vmpm_pinMapTbl[mpmPinNum].mpm_pin_type;
    ptr_mpmPinInfo->trig_config = vmpm_pinMapTbl[mpmPinNum].trig_type;

    status = MPM_SUCCESS;
  }
  else
  {
    status = MPM_FAIL;
    vmpm_LogPrintf( 1, "ERROR: MPM Pin Num not configured as IRQ or GPIO interrupt "
                    "(MPM Pin Number: %u)", 
                    mpmPinNum ); 
  }

  return status;
}

/**
 * vmpm_addIrqPinToMappingTbl
 * 
 */
mpm_status_type vmpm_addIrqPinToMappingTbl( uint32 mpmPinNum,
                                            mpm_int_pin_mapping_type * pNewMapTblEntryInfo)
{
  mpm_status_type status = MPM_FAIL;
  uint32 origIrqIdNumValue = 0;

  if (NULL == pNewMapTblEntryInfo) 
  {
    return MPM_FAIL;
  }

  uint32 newIrqIdNumValue = pNewMapTblEntryInfo->local_pin_num;

  /* Check that MPM Pin Num is valid and request is to
   * configure a pin in mapping table to be an IRQ pin.
   * Also check that legitimate value for Irq ID value is requested */
  if ( ( MPM_FAIL == vmpm_IsPinSupported(mpmPinNum) ) ||
       ( pNewMapTblEntryInfo->local_pin_num >= MPM_MAX_LOC_IC_VAL ) ||
       ( MPM_IRQ != pNewMapTblEntryInfo->mpm_pin_type ) ) 
  {
    vmpm_LogPrintf( 2, "ERROR: Invalid input values to add IRQ pin to mapping "
                    "table (MPM Pin Number: %u) (Local ID value: %u)", 
                    mpmPinNum,
                    newIrqIdNumValue ); 
    return MPM_FAIL;
  }
  else if ( MPM_SUCCESS == vmpm_IsGpio(mpmPinNum) )
  {
    /* Reconfiguring a pin in the mapping table that is currently configured
     * in the mapping table as an MPM_GPIO or MPM_OPEN to MPM_IRQ pin is not
     * allowed */
    vmpm_LogPrintf( 1, "ERROR: Cannot reconfigure a GPIO pin or "
                    "Open pin as an IRQ pin at (MPM Pin Number: %u)", 
                    mpmPinNum);
    return MPM_FAIL;
  }
  else
  {
    /* Determine the current IRQ ID Number in mapping table
     * for this MPM Pin Num */
    origIrqIdNumValue = vmpm_pinMapTbl[mpmPinNum].local_pin_num; 
  }
 
  /* Allow Configure of this pin with new IRQ value if input Pin type is IRQ
   * and Local ID is either equal to existing value or not configured
   * i.e. local pin number = MPM_UNMAPPED_INT */
  if ( ( MPM_UNMAPPED_INT == origIrqIdNumValue ) ||
       ( newIrqIdNumValue == origIrqIdNumValue ) ) 
  {
    mpm_trigger_type irqTrig = pNewMapTblEntryInfo->trig_type;

    /* Ensure only one client is updating mapping table */
    VMPM_LOCK();
    vmpm_pinMapTbl[mpmPinNum].trig_type = irqTrig;
    vmpm_pinMapTbl[mpmPinNum].local_pin_num = newIrqIdNumValue;
    vmpm_pinMapTbl[mpmPinNum].mpm_pin_type = MPM_IRQ;

    /* Update the count of Mapped IRQ's only if new IRQ */
    if ( MPM_UNMAPPED_INT == origIrqIdNumValue ) 
    {
      vmpm_setNumMappedIrqs();
    }
    VMPM_UNLOCK();

    status = MPM_SUCCESS;
  }
  else 
  {
    vmpm_LogPrintf( 1, "ERROR: Cannot add IRQ to mapping table due to current "
                    "configuration at (MPM Pin Number: %u)", 
                    mpmPinNum);
    status = MPM_FAIL;
  }

  return status;
}

/**
 * vmpm_MapIrqsWithMaster
 */
mpm_status_type vmpm_MapIrqsWithMaster ( mpm_config_info_type *pIrqArray, 
                                         uint32 irqCount,
                                         uint32 *pNumMappedIrqs )
{
  uint32 mpm_num;
  mpm_int_pin_mapping_type *table_entry = NULL;    /* Mapping table iterator */
  mpm_status_type status = MPM_FAIL;

  if ( (NULL == pIrqArray) ||
       (NULL == pNumMappedIrqs) )
  {
    return MPM_FAIL;
  }

  uint32 num_mapped_irqs = vmpm_getNumMappedIrqs();
  uint32 mapTblSize = vmpm_getMapTblSize();

  if( irqCount != num_mapped_irqs )
  {
    /* Calling function does not expect same number of mapped interrupts as 
     * present in the mapping table. Log a message and return 0 indicating that
     * no interrupts were mapped. It is up to caller to handle that. */
    vmpm_LogPrintf( 2, "WARNING: Interrupt mapping count mismatch "
                    "(expected: %u) (requested: %u)", 
                    num_mapped_irqs, irqCount );
    return MPM_FAIL;
  }
  else
  {
    status = MPM_SUCCESS;
  }

  /* Must ensure this index begins at zero */
  *pNumMappedIrqs = 0;

  for( mpm_num = 0; mpm_num < mapTblSize; mpm_num++ )
  {
    table_entry = &(vmpm_pinMapTbl[mpm_num]);

    if ( ( MPM_UNMAPPED_INT  != table_entry->local_pin_num ) && /* Wakeup IRQ */
         ( MPM_SUCCESS == vmpm_IsIrq( mpm_num ) )  ) 
    {
      pIrqArray[(*pNumMappedIrqs)].mpm_hw_int_id = mpm_num;
      pIrqArray[(*pNumMappedIrqs)].master_hw_int_id = table_entry->local_pin_num;

      (*pNumMappedIrqs)++;
    }
  }

  vmpm_LogPrintf( 1, "Mapped mpm interrupts with master (IRQs Mapped: %u)",
                  (*pNumMappedIrqs) );

  return status; 
}

/**
 * vmpm_SetupIrqs
 */
mpm_status_type vmpm_SetupIrqs( mpm_config_info_type *pIrqArray,
                                uint32 irqCount )
{
  uint32 i;
  uint32 irqNum;
  uint32 num_mapped_irqs = vmpm_getNumMappedIrqs();
  mpm_status_type status = MPM_FAIL;

  if (NULL == pIrqArray) 
  {
    /* Input array ptr is NULL */
    vmpm_LogPrintf( 0, "ERROR: Input pIrqArray is NULL ");

    return MPM_FAIL;
  }

  if( irqCount != num_mapped_irqs )
  {
    /* Input array does not contain same number of interrupts than are 
     * actually mapped for this master */
    vmpm_LogPrintf( 2, "ERROR: Interrupt setup count mismatch "
                    "(expected: %u) (requested: %u)",
                    num_mapped_irqs, irqCount);
    CORE_VERIFY(0);
  }

  if( 0 == num_mapped_irqs )
  {
    /* Only GPIOs are mapped to mpm for this target. No need to
     * configure anything. Simply return */
    vmpm_LogPrintf( 0, "WARNING: No interrupt mapped to MPM" );
    return MPM_SUCCESS;
  }

  /* Checking the validity of input */
  for( i = 0; i < irqCount; i++ )
  {
    status = vmpm_getIrqNum(pIrqArray[i].mpm_hw_int_id, &irqNum);

    if( (MPM_FAIL == status) ||
        (irqNum != pIrqArray[i].master_hw_int_id) )
    {
      /* Either the mpm_id or the master hw id is not consistent with what
       * information vMPM driver has. It should not usually happen as clients
       * are not supposed to disturb mpm_config_info_type objects. */

      vmpm_LogPrintf(3, "ERROR: Possibly corrupted mpm_config_info_type obj"
                     " (index: %u) (mpm id: %d) (master id: %u)", 
                     i, 
                     pIrqArray[i].mpm_hw_int_id, 
                     pIrqArray[i].master_hw_int_id);
      CORE_VERIFY(0);
    }
  }
  status = MPM_SUCCESS;

  VMPM_LOCK();
  vmpm_HAL_SetupIrqs((vmpm_HAL_ConfigInfoType *)pIrqArray, irqCount);
  VMPM_UNLOCK();

  return status;
}
