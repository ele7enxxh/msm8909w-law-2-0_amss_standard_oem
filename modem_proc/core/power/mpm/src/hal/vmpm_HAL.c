/*============================================================================
  
  FILE:         vmpm_HAL.c
  
  DESCRIPTION:  This is the generic hardware abstraction layer implementation 
                for the MPM interrupt controller.
  
  DEPENDENCIES: None

                Copyright (c) 2014-2015 QUALCOMM Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/hal/vmpm_HAL.c#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/ 
/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include <string.h>
#include <HALcomdef.h>
#include "vmpm_HAL.h"
#include "vmpm_HAL_Internal.h"
#include "vmpm_internal.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
 * Identifier string for this HAL.
 */
#define HAL_VMPM_VERSION_STRING "HAL_VMPM_V2"

/*
 * Macro to determine what offset should be used while configuring given
 * interrupt number. Validity of the input should be verified prior to using
 * value returned by this macro.
 */
#define MPMINT_2_REG_OFFSET(nMPM) ((sizeof(uint32)*(nMPM/(sizeof(uint32)*8))))

/*
 * Macros to create bit masks, set and clear bits in a mask.
 */
#define MAKE_MASK(bit_pos)  (1 << ((bit_pos) % 32))
#define BIT_SET32(val, bit_pos)  (val |  (MAKE_MASK(bit_pos)))
#define BIT_CLR32(val, bit_pos)  (val & ~(MAKE_MASK(bit_pos)))

/* -------------------------------------------------------------------------
**                         VARIABLE DECLARATIONS
** ------------------------------------------------------------------------- */

/**
 * Register layout object for the master. This will store information 
 * regarding interrupt/gpio configuration (detection, polarity, 
 * status) and will be used by RPM to write to actual MPM 
 * driver. 
 */
vmpm_HAL_PlatformType registerLayout;

/* -----------------------------------------------------------------------
**                         INTERNAL FUNCTIONS
** ----------------------------------------------------------------------- */
/** 
 * Configures the trigger of a given interrupt from the input
 * trigger type. 
 *
 * @note It must be noted that it does not notify RPM about these
 *       changes so it is up to the caller to perform this task.
 *
 * @param nMPM: Interrupt id (at MPM HW) for which we want to change this
 *              configuration.
 * @param eTrigger: Trigger settings for detection and polarity
 *                  Any valid enum value of type mpm_trigger_type
 *                  can be a possible input.
 *
 * @Note
 * From MPM HDD, three bits are used as follows for interrupt detection and 
 * polarity. 
 *   
 * 0 00    Level Sensitive Active Low
 * 0 01    Falling Edge Sensitive
 * 0 10    Rising Edge Sensitive
 * 0 11    Dual Edge Sensitive
 * 1 00    Level Sensitive Active High
 */

static void vmpm_HAL_ConfigDetectionAndPolarity( uint32           nMPM, 
                                                 mpm_trigger_type eTrigger )
{
  uint32 nReg;
  uint32 nVal;
  uint32 nIndex;

  if( MPM_FAIL == vmpm_IsPinSupported(nMPM) )
  {
    /* Unsupported mpm irq/gpio id */
    return;
  }

  /* Offset of nMPM within two 4-byte registers (0 or 4) */
  nIndex = MPMINT_2_REG_OFFSET(nMPM);

  /* Configure detection1 bit */
  nReg = registerLayout.nDetectReg + nIndex;
  nVal = inpdw(nReg);
  nVal = HAL_TT_EXTRACT_DET1_BIT(eTrigger) ? 
         BIT_SET32(nVal, nMPM) : BIT_CLR32(nVal, nMPM);
  outpdw(nReg, nVal);

  /* Configure detection3 bit */
  nReg = registerLayout.nDetect3Reg + nIndex;
  nVal = inpdw(nReg);
  nVal = HAL_TT_EXTRACT_DET3_BIT(eTrigger) ? 
         BIT_SET32(nVal, nMPM) : BIT_CLR32(nVal, nMPM);
  outpdw(nReg, nVal);

  /* Configure the polarity */
  nReg = registerLayout.nPolarityReg + nIndex;
  nVal = inpdw(nReg);
  nVal = HAL_TT_EXTRACT_POL_BIT(eTrigger) ? 
         BIT_SET32(nVal, nMPM) : BIT_CLR32(nVal, nMPM);
  outpdw(nReg, nVal);

}


/**
 * Sets the status of a given input interrupt. 
 *
 * @note It simply writes in shared memory but does not notirfy RPM. It's 
 *       left up to caller.
 *
 *       Also, it simply sets status bit for a given interrupt so if some
 *       pre/post processing is needed, it has to be done by call (e.g.
 *       clearing the interrupt before enabling it).
 *
 * @param nMPM:    Interrupt id (at MPM HW) for which we want to change this
 *                 configuration.
 * @param eStatus: Status to which above interrupt will be set to.
 *                 Possible Inputs:
 *                 - VMPM_HAL_STATUS_ENABLE
 *                 - VMPM_HAL_STATUS_DISABLE
 */
static void vmpm_HAL_SetStatus ( uint32 nMPM, 
                                 vmpm_HAL_StatusType eStatus )
{
  uint32 nReg;
  uint32 nVal;
  uint32 nIndex;

  if( MPM_FAIL == vmpm_IsPinSupported(nMPM) )
  {
    /* Unsupported mpm irq/gpio id */
    return;
  }

  /* Offset of nMPM within two 4-byte registers (0 or 4) */
  nIndex = MPMINT_2_REG_OFFSET(nMPM); 

  nReg = registerLayout.nEnableReg + nIndex;
  nVal = inpdw(nReg);
  nVal = (VMPM_HAL_STATUS_ENABLE == eStatus) ? 
         BIT_SET32(nVal, nMPM) : BIT_CLR32(nVal, nMPM);
  outpdw(nReg, nVal);  
}

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */


/* ===========================================================================
**  vmpm_HAL_Init
**
** ======================================================================== */

void vmpm_HAL_Init( char **ppszVersion, 
                    uint8 *master_msg_ram_base_ptr, 
                    bool32 init_vmpm_regs )
{
  uint32 nMPM;
  uint32 mapTblSize = vmpm_getMapTblSize();

  /* Preparing mpm driver for sending IPC interrupt to let rpm know about the 
   * modified interrupt/gpio configurations. */
  vmpm_HAL_SetupIPC();

  /* Initializing the registerLayout of shared memory */
  vmpm_HAL_InitializeRegisterLayout(master_msg_ram_base_ptr);

  if( TRUE == init_vmpm_regs )
  {
    /* Populate MPM interrupt controller with initial guesses from BSP data. */
    for (nMPM = 0; nMPM < mapTblSize; nMPM++)
    {
     vmpm_HAL_SetTrigger( nMPM, 
                          vmpm_pinMapTbl[nMPM].trig_type );
    }

    /* Disable and clear all interrupts */
    vmpm_HAL_All_Disable();
    vmpm_HAL_All_Clear();
  }

  /* Fill in return value */
  if (ppszVersion != NULL)
  {
    *ppszVersion = (char*)HAL_VMPM_VERSION_STRING;
  }

} /* END vmpm_HAL_Init */


/* ===========================================================================
**  vmpm_HAL_Reset
**
** ======================================================================== */

void vmpm_HAL_Reset(void)
{
  vmpm_HAL_All_Disable();
  vmpm_HAL_All_Clear();

} /* END vmpm_HAL_Reset */

/* ===========================================================================
**  vmpm_HAL_Enable
**
** ======================================================================== */

void vmpm_HAL_Enable( uint32 nMPM )
{
  /* Enable the interrupt in the MPM register */
  vmpm_HAL_SetStatus( nMPM, VMPM_HAL_STATUS_ENABLE );

  /* Write config info to message RAM, along with the enable notification. */
  vmpm_HAL_ConfigDetectionAndPolarity( nMPM, 
                                       vmpm_pinMapTbl[nMPM].trig_type );

  vmpm_send_interrupt();

} /* END vmpm_HAL_Enable */


/* ===========================================================================
**  vmpm_HAL_Disable
**
** ======================================================================== */

void vmpm_HAL_Disable( uint32 nMPM )
{
  /* Disable the interrupt in MPM */
  vmpm_HAL_SetStatus( nMPM, VMPM_HAL_STATUS_DISABLE );

  vmpm_send_interrupt();

} /* END vmpm_HAL_Disable */


/* ===========================================================================
**  vmpm_HAL_Clear
**
** ======================================================================== */

void vmpm_HAL_Clear( uint32 nMPM )
{
  uint32 nReg;
  uint32 nVal;
  uint32 nIndex;

  if( MPM_FAIL == vmpm_IsPinSupported(nMPM) )
  {
    /* Unsupported mpm irq/gpio id */
    return;
  }

  /* Offset of nMPM within two 4-byte registers (0 or 4) */
  nIndex = MPMINT_2_REG_OFFSET(nMPM); 

  nReg = registerLayout.nStatusReg + nIndex;
  nVal = inpdw(nReg);
  nVal = BIT_CLR32(nVal, nMPM);
  outpdw(nReg, nVal); 
  
  /* No need to notify the RPM, since it has nothing to do in response to
   * a clear. */
   
} /* END vmpm_HAL_Clear */


/* ===========================================================================
**  vmpm_HAL_All_Enable
**
** ======================================================================== */

void vmpm_HAL_All_Enable( void )
{
  uint32 nNumRegs, nReg, nMask, regIndex, nMPM;
  uint32 mapTblSize = vmpm_getMapTblSize();

  vmpm_HAL_GetNumberMasks(&nNumRegs);

  /* We could have used memset as used earlier but somehow it results into
   * symbol loading problem. */
  for( regIndex = 0; regIndex < nNumRegs; regIndex++ )
  {
    nReg  = registerLayout.nEnableReg + (sizeof(uint32) * regIndex);
    nMask = 0xFFFFFFFF;

    outpdw(nReg, nMask);               /* Enable the interrupts */
  }

  /* Write config info for all interrupts to message RAM, along with the
   * enable notification. */
  for (nMPM = 0; nMPM < mapTblSize; nMPM++)
  {
    vmpm_HAL_ConfigDetectionAndPolarity( nMPM, 
                                         vmpm_pinMapTbl[nMPM].trig_type );
  }

  vmpm_send_interrupt();

} /* END vmpm_HAL_All_Enable */


/* ===========================================================================
**  vmpm_HAL_All_Disable
**
** ======================================================================== */

void vmpm_HAL_All_Disable( void )
{
  uint32 nNumRegs, nReg, nMask, regIndex;

  vmpm_HAL_GetNumberMasks(&nNumRegs);

  /* We could have used memset as used earlier but somehow it results into
   * symbol loading problem. */
  for( regIndex = 0; regIndex < nNumRegs; regIndex++ )
  {
    nReg  = registerLayout.nEnableReg + (sizeof(uint32) * regIndex);
    nMask = 0x0;
    outpdw(nReg, nMask);
  }

  vmpm_send_interrupt();

} /* END vmpm_HAL_Disable */


/* ===========================================================================
**  vmpm_HAL_All_Clear
**
** ======================================================================== */

void vmpm_HAL_All_Clear( void )
{
  uint32 nNumRegs, nReg, nMask, regIndex;

  vmpm_HAL_GetNumberMasks(&nNumRegs);

  for( regIndex = 0; regIndex < nNumRegs; regIndex++ )
  {
    nReg  = registerLayout.nStatusReg + (sizeof(uint32) * regIndex);
    nMask = 0x0;
    outpdw(nReg, nMask);
  }

  /* No need to notify the RPM, since it has nothing to do in response to
   * a clear. */

} /* END vmpm_HAL_All_Clear */


/* ===========================================================================
**  vmpm_HAL_GetNumberMasks
**
** ======================================================================== */
void vmpm_HAL_GetNumberMasks( uint32 *pnNumber )
{
  *pnNumber = ((vmpm_getMapTblSize() + 31) / 32);

} /* END vmpm_HAL_GetNumberMasks */

/* ===========================================================================
**  vmpm_HAL_Mask_Enable
**
** ======================================================================== */

void vmpm_HAL_Mask_Enable( uint32 nMaskIndex, 
                           uint32 nMask )
{
  uint32 nReg, nVal, nNumRegs, nMPM;

  vmpm_HAL_GetNumberMasks(&nNumRegs);

  if(nMaskIndex >= nNumRegs)
    return;

  /* Enable the interrupts in the MPM register */
  nReg = registerLayout.nEnableReg + (sizeof(uint32) * nMaskIndex);
  nVal = inpdw(nReg);
  nVal |= nMask;
  outpdw(nReg, nVal);

  /* Get starting MPM table index, depending on mask */
  nMPM = nMaskIndex * 32;

  /* Find all bits set in the mask, and update config info in message RAM,
   * along with the enable notification. */
  while ( nMask != 0 )
  {
    if ( 0 != ( nMask & 0x1 ) )
    {
      vmpm_HAL_ConfigDetectionAndPolarity( nMPM, 
                                           vmpm_pinMapTbl[nMPM].trig_type );
    }

    nMPM++;
    nMask = nMask >> 1;
  }

  vmpm_send_interrupt();

} /* END vmpm_HAL_Mask_Enable */


/* ===========================================================================
**  vmpm_HAL_Mask_Disable
**
** ======================================================================== */

void vmpm_HAL_Mask_Disable ( uint32 nMaskIndex, 
                             uint32 nMask )
{
  uint32 nReg, nVal, nNumRegs;

  vmpm_HAL_GetNumberMasks(&nNumRegs);

  if(nMaskIndex >= nNumRegs)
    return;

  /* Disable the interrupts in the MPM register */
  nReg = registerLayout.nEnableReg + (sizeof(uint32) * nMaskIndex);
  nVal = inpdw(nReg);
  nVal &= ~nMask;
  outpdw(nReg, nVal);

  vmpm_send_interrupt();

} /* END vmpm_HAL_Mask_Disable */


/* ===========================================================================
**  vmpm_HAL_Mask_Clear
**
** ======================================================================== */

void vmpm_HAL_Mask_Clear( uint32 nMaskIndex, 
                          uint32 nMask )
{
  uint32 nReg, nVal, nNumRegs;

  vmpm_HAL_GetNumberMasks(&nNumRegs);

  if(nMaskIndex >= nNumRegs)
    return;

  /* Clear the interrupts in the MPM register */
  nReg = registerLayout.nStatusReg + (sizeof(uint32) * nMaskIndex);
  nVal = inpdw(nReg);
  nVal &= ~nMask;
  outpdw(nReg, nVal);

  /* No need to notify the RPM, since it has nothing to do in response to
   * a clear. */

} /* END vmpm_HAL_Mask_Clear */


/* ===========================================================================
**  vmpm_HAL_GetPending
**
** ======================================================================== */

void vmpm_HAL_GetPending ( uint32 nMaskIndex, 
                           uint32 *pnMask )
{
  uint32 nNumRegs, nReg, nEnable;

  vmpm_HAL_GetNumberMasks(&nNumRegs);

  if((nMaskIndex >= nNumRegs) || (!pnMask))
    return;

  nReg = registerLayout.nStatusReg + (sizeof(uint32) * nMaskIndex);
  nEnable = registerLayout.nEnableReg + (sizeof(uint32) * nMaskIndex);
  *pnMask = ( inpdw(nReg) & inpdw(nEnable) );

} /* vmpm_HAL_GetPending */


/* ===========================================================================
**  vmpm_HAL_GetTrigger
**
** ======================================================================== */
/* 
 * @Note
 * The bit encoding of this trigger type corresponds to:
 *
 * POLARITY: Bit2
 * DETECT3 : Bit1
 * DETECT  : Bit0
 */
void vmpm_HAL_GetTrigger( uint32 nMPM, 
                          mpm_trigger_type *peTrigger )
{
  uint32 nIndex, nMask, nReg;
  uint32 nDetect1Val, nDetect3Val, nPolarityVal;

  if( MPM_FAIL == vmpm_IsPinSupported(nMPM) )
  {
    /* Unsupported mpm irq/gpio id */
    return;
  }

  /* Offset of nMPM within two 4-byte registers (0 or 4) */
  nIndex = MPMINT_2_REG_OFFSET(nMPM);

  nMask = MAKE_MASK(nMPM);

  nReg = registerLayout.nDetectReg + nIndex;
  nDetect1Val = (inpdw(nReg) & nMask) ? 1 : 0;

  nReg = registerLayout.nDetect3Reg + nIndex;
  nDetect3Val = (inpdw(nReg) & nMask) ? 1 : 0;

  nReg = registerLayout.nPolarityReg + nIndex;
  nPolarityVal = (inpdw(nReg) & nMask) ? 1 : 0;

  /* mpm_trigger_type is enum'ed to out of POLARITY | DETECT3 | DETECT1 */
  *peTrigger = ( mpm_trigger_type )((nPolarityVal << 0x2) | 
                                    (nDetect3Val << 0x1)  | 
                                    (nDetect1Val << 0) );

} /* vmpm_HAL_GetTrigger */


/* ===========================================================================
**  vmpm_HAL_SetTrigger
**
** ======================================================================== */

void vmpm_HAL_SetTrigger( uint32 nMPM, 
                          mpm_trigger_type  eTrigger )
{
  /* Cache the config data, to be sent when the interrupt gets enabled. */
  vmpm_pinMapTbl[nMPM].trig_type = eTrigger;

  /* Send the config data, only if the interrupt is enabled. */
  if ( vmpm_HAL_IsEnabled( nMPM ) )
  {
    /* Configuring to input detection and polarity. */
    vmpm_HAL_ConfigDetectionAndPolarity(nMPM, eTrigger);

    /* Commit the change. */
    vmpm_send_interrupt();
  }

} /* END vmpm_HAL_SetTrigger */

/* ===========================================================================
**  vmpm_HAL_IsPending
**
** ======================================================================== */

bool32 vmpm_HAL_IsPending( uint32 nMPM )
{
  uint32 nIndex, nMask;
  uint32 nStatusReg, nEnableReg;
  bool32 bPending = FALSE;

  if(MPM_FAIL == vmpm_IsPinSupported(nMPM))
  {
    return bPending;
  }

  /* Offset of nMPM within two 4-byte registers (0 or 4) */
  nIndex     = MPMINT_2_REG_OFFSET(nMPM);

  nMask      = MAKE_MASK(nMPM);
  nStatusReg = registerLayout.nStatusReg + nIndex;
  nEnableReg = registerLayout.nEnableReg + nIndex;

  if (inpdw(nStatusReg) & inpdw(nEnableReg) & nMask)
  {
    bPending = TRUE;
  }

  /* Return if we are set */
  return bPending;

} /* END vmpm_HAL_IsPending */


/* ===========================================================================
**  vmpm_HAL_IsEnabled
**
** ======================================================================== */

bool32 vmpm_HAL_IsEnabled( uint32 nMPM )
{
  uint32 nIndex, nMask, nReg;
  bool32 bEnabled = FALSE;

  if( MPM_FAIL == vmpm_IsPinSupported(nMPM) )
  {
    return bEnabled;
  }

  /* Determine position of nMPM within 64 bit register length */
  nIndex = MPMINT_2_REG_OFFSET(nMPM);

  nMask = MAKE_MASK(nMPM);
  nReg  = registerLayout.nEnableReg + nIndex;

  if (inpdw(nReg) & nMask)
  {
    bEnabled = TRUE;
  }

  /* Return if we are set */
  return bEnabled;

} /* END vmpm_HAL_IsEnabled */


/* ===========================================================================
**  vmpm_HAL_IsClearBefore
**
** ======================================================================== */

bool32 vmpm_HAL_IsClearBefore( uint32 nMPM )
{   
  if( MPM_FAIL == vmpm_IsPinSupported(nMPM) )
  {
    return FALSE;
  }

  /* Return TRUE if this interrupt clear before.  If not, then it is
   * assumed to be clear after. */
  if ( vmpm_pinMapTbl[nMPM].trig_type == MPM_TRIGGER_LEVEL_LOW ||
       vmpm_pinMapTbl[nMPM].trig_type == MPM_TRIGGER_LEVEL_HIGH )
  {
    return FALSE;
  }
  else
  {
    /* All edge detect interrupts are also CLEAR BEFORE */
    return TRUE;
  }
}

/* ========================================================================
**  vmpm_HAL_SetupIrqs
**
** ========================================================================*/
void vmpm_HAL_SetupIrqs( vmpm_HAL_ConfigInfoType *irqArray, 
                         uint32 irqCount )
{
  uint32 iter, nMPM;
  mpm_trigger_type eTrigger;
  vmpm_HAL_StatusType  eStatus;

  for( iter = 0; iter < irqCount; iter++ )
  {
    nMPM     = irqArray[iter].mpm_hw_int_id;
    eTrigger = irqArray[iter].trigger;
    eStatus  = irqArray[iter].status;

    /* Cache the config data, to be sent when the interrupt gets enabled. */
    vmpm_pinMapTbl[nMPM].trig_type = eTrigger;

    /* Setting up the detection and polarity in message RAM, only if the
     * interrupt is getting enabled. */
    if ( VMPM_HAL_STATUS_ENABLE == eStatus )
    {
      vmpm_HAL_ConfigDetectionAndPolarity(nMPM, eTrigger);
    }

    /* Updating interrupt status */
    vmpm_HAL_SetStatus(nMPM, eStatus);

  }

  /* Notifying RPM about this update */
  vmpm_send_interrupt();

}

/* ========================================================================
**  vmpm_HAL_InitializeRegisterLayout
**
**  This function must be called with appropriate address passed to it 
**  during early initialization (i.e. before configuring any interrupt/gpio).
** ========================================================================*/

void vmpm_HAL_InitializeRegisterLayout ( uint8 *msg_ram_base_ptr )
{
  uint32 vmpm_base_addr = (uint32)msg_ram_base_ptr + 0x0;

  /* Offset is of 0x8 because each config register is a set of two 
   * 4-byte registers */
  registerLayout.WakeupReg     = vmpm_base_addr + 0x8 * 0;
  registerLayout.nEnableReg    = vmpm_base_addr + 0x8 * 1;
  registerLayout.nDetectReg    = vmpm_base_addr + 0x8 * 2;
  registerLayout.nDetect3Reg   = vmpm_base_addr + 0x8 * 3;
  registerLayout.nPolarityReg  = vmpm_base_addr + 0x8 * 4;
  registerLayout.nStatusReg    = vmpm_base_addr + 0x8 * 5;
  registerLayout.nEnableRegPad = 0;
  registerLayout.nClearReg     = 0;
}

