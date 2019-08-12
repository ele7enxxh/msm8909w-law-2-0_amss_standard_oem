/*
===========================================================================

FILE:         HALmpmint.c

DESCRIPTION:  
  This is the generic hardware abstraction layer implementation for the
  MPM interrupt controller.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/source/MPM_V4/HALmpmint.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright © 2008-2011 Qualcomm Technologies Incorporated.
                        All Rights Reserved.
                QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include <string.h>
#include <HALmpmint.h>

#include "../common/HALmpmintInternal.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
 * Identifier string for this HAL.
 */
#define HAL_MPMINT_VERSION_STRING "HAL_MPMINT_V1"

#define MAKE_MASK(bit_pos) (1 << ((bit_pos) % 32))

#define POL_FROM_IDX(idx) (idx&4)
#define DET_B_FROM_IDX(idx) (idx&2)
#define DET_A_FROM_IDX(idx) (idx&1)

const HAL_mpmint_TriggerType mpmint_trigger_lookup[8] = {HAL_MPMINT_TRIGGER_LOW,       //pol = 0, det_b = 0, det_a = 0
                                                         HAL_MPMINT_TRIGGER_FALLING,   //pol = 0, det_b = 0, det_a = 1 
                                                         HAL_MPMINT_TRIGGER_RISING,    //pol = 0, det_b = 1, det_a = 0 
                                                         HAL_MPMINT_TRIGGER_DUAL,      //pol = 0, det_b = 1, det_a = 1 
                                                         HAL_MPMINT_TRIGGER_HIGH,      //pol = 1, det_b = 0, det_a = 0 
                                                         HAL_MPMINT_TRIGGER_RISING,    //pol = 1, det_b = 0, det_a = 1 
                                                         HAL_MPMINT_TRIGGER_FALLING,   //pol = 1, det_b = 1, det_a = 0 
                                                         HAL_MPMINT_TRIGGER_DUAL};     //pol = 1, det_b = 1, det_a = 1

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**  HAL_mpmint_Init
**
** ======================================================================== */

void HAL_mpmint_Init
(
  char **ppszVersion
)
{
  uint32 nMPM;

  /*
   * Populate MPM interrupt controller with initial guesses from BSP data.
   * Also, initialize the back-lookup table while we're at it.
   */
  for (nMPM = 0; nMPM < HAL_MPMINT_NUM; nMPM++)
  {
    HAL_mpmint_SetTrigger( nMPM, aInterruptTable[nMPM].eTrigger );
  }

  /*
   * Disable and clear all interrupts
   */
  HAL_mpmint_All_Disable();
  HAL_mpmint_All_Clear();

  /*
   * Fill in return value
   */
  if (ppszVersion != NULL)
  {
    *ppszVersion = (char*)HAL_MPMINT_VERSION_STRING;
  }

} /* END HAL_mpmint_Init */


/* ===========================================================================
**  HAL_mpmint_Reset
**
** ======================================================================== */

void HAL_mpmint_Reset (void)
{
  /* TODO */

} /* END HAL_mpmint_Reset */


/* ===========================================================================
**  HAL_mpmint_Save
**
** ======================================================================== */

void HAL_mpmint_Save (void)
{
  /* MPM hardware is always on. */
} /* END HAL_mpmint_Save */


/* ===========================================================================
**  HAL_mpmint_Restore
**
** ======================================================================== */

void HAL_mpmint_Restore (void)
{
  /* MPM hardware is always on. */
} /* END HAL_mpmint_Restore */


/* ===========================================================================
**  HAL_mpmint_Enable
**
** ======================================================================== */

void HAL_mpmint_Enable
(
  uint32 nMPM
)
{
  uint32 nPadPos, nPadMask, nRegPad;
  uint32 nBitPos, nMask, nReg, nVal;

  /* 
   * Enable the interrupt at the pad 
   */
  nPadPos  = aInterruptTable[nMPM].padBitPos;

  if ( HAL_MPMINT_INVALID_BIT_POS != nPadPos )
  {
    nRegPad  = registerLayout.nEnableRegPad + (sizeof(uint32) * (nPadPos / 32));
    nPadMask = MAKE_MASK(nPadPos);

    nVal = inpdw(nRegPad);
    nVal |= nPadMask;
    outpdw(nRegPad, nVal);
  }

  /* 
   * Clear the interrupt 
   */
  HAL_mpmint_Clear( nMPM );

  /* 
   * Enable the interrupt in the MPM register 
   */
  nBitPos = nMPM;
  nReg    = registerLayout.nEnableReg + (sizeof(uint32) * (nBitPos / 32));
  nMask   = MAKE_MASK(nBitPos);

  nVal = inpdw(nReg);
  nVal |= nMask;
  outpdw(nReg, nVal);

} /* END HAL_mpmint_Enable */


/* ===========================================================================
**  HAL_mpmint_Disable
**
** ======================================================================== */

void HAL_mpmint_Disable
(
  uint32 nMPM
)
{
  uint32 nPadPos, nPadMask, nRegPad;
  uint32 nBitPos, nMask, nReg, nVal;

  /* 
   * Disable the interrupt at the pad 
   */
  nPadPos  = aInterruptTable[nMPM].padBitPos;

  if ( HAL_MPMINT_INVALID_BIT_POS != nPadPos )
  {
    nRegPad  = registerLayout.nEnableRegPad + (sizeof(uint32) * (nPadPos / 32));
    nPadMask = MAKE_MASK(nPadPos);

    nVal = inpdw(nRegPad);
    nVal &= ~nPadMask;
    outpdw(nRegPad, nVal);
  }

  /*
   * Disable the interrupt in MPM
   */
  nBitPos = nMPM;
  nReg    = registerLayout.nEnableReg + (sizeof(uint32) * (nBitPos / 32));
  nMask   = MAKE_MASK(nBitPos);

  nVal = inpdw(nReg);
  nVal &= ~nMask;
  outpdw(nReg, nVal);

} /* END HAL_mpmint_Disable */


/* ===========================================================================
**  HAL_mpmint_Clear
**
** ======================================================================== */

void HAL_mpmint_Clear
(
  uint32 nMPM
)
{
  uint32 nBitPos, nMask, nReg;

  nBitPos = nMPM;
  nMask   = MAKE_MASK(nBitPos);
  nReg    = registerLayout.nClearReg + (sizeof(uint32) * (nBitPos / 32));

  outpdw(nReg, nMask);

} /* END HAL_mpmint_Clear */


/* ===========================================================================
**  HAL_mpmint_All_Enable
**
** ======================================================================== */

void HAL_mpmint_All_Enable( void )
{
  uint32 nNumRegs, nReg, nRegPad, nClearReg;

  HAL_mpmint_GetNumberMasks(&nNumRegs);

  nReg      = registerLayout.nEnableReg;
  nRegPad   = registerLayout.nEnableRegPad;
  nClearReg = registerLayout.nClearReg;

  /* enable at pad first */
  memset((void*)nRegPad, 0xFFFFFFFF, sizeof(uint32)*nNumRegs);

  /* then clear the interrupts */
  memset((void*)nClearReg, 0xFFFFFFFF, sizeof(uint32)*nNumRegs);

  /* finally, enable the interrupts */
  memset((void*)nReg, 0xFFFFFFFF, sizeof(uint32)*nNumRegs);

} /* END HAL_mpmint_All_Enable */


/* ===========================================================================
**  HAL_mpmint_All_Disable
**
** ======================================================================== */

void HAL_mpmint_All_Disable( void )
{
  uint32 nNumRegs, nReg, nRegPad;

  HAL_mpmint_GetNumberMasks(&nNumRegs);

  nReg    = registerLayout.nEnableReg;
  nRegPad = registerLayout.nEnableRegPad;

  /* disable both at pad AND in MPM */
  memset((void*)nRegPad, 0, sizeof(uint32)*nNumRegs);
  memset((void*)nReg, 0, sizeof(uint32)*nNumRegs);

} /* END HAL_mpmint_Disable */


/* ===========================================================================
**  HAL_mpmint_All_Clear
**
** ======================================================================== */

void HAL_mpmint_All_Clear( void )
{
  uint32 nNumRegs, nReg;

  nNumRegs = HAL_MPMINT_PHYS_INTS / 32;
  nReg     = registerLayout.nClearReg;
  memset((void*)nReg, 0xFFFFFFFF, sizeof(uint32)*nNumRegs);

} /* END HAL_mpmint_Clear */


/* ===========================================================================
**  HAL_mpmint_GetNumberMasks
**
** ======================================================================== */

void HAL_mpmint_GetNumberMasks
(
  uint32 *pnNumber
)
{

  *pnNumber = (HAL_MPMINT_PHYS_INTS / 32);

} /* END HAL_mpmint_GetNumberMasks */


/* ===========================================================================
**  HAL_mpmint_Mask_Enable
**
** ======================================================================== */

void HAL_mpmint_Mask_Enable
(
  uint32 nMaskIndex,
  uint32 nMask
)
{
  uint32 nNumRegs, nWorkingMask, i;

  HAL_mpmint_GetNumberMasks(&nNumRegs);

  if(nMaskIndex > nNumRegs)
    return;

  // This is a simplified implementation; if blazing speed is necessary this
  // can likely be made faster by unrolling the multiple Enable calls into:
  //   - a write per pad enable register
  //   - a Mask_Clear call
  //   - a write to the enable register
  nWorkingMask = 1;
  for(i = 0; i < 32 && nMask; i++)
  {
    if(nMask & nWorkingMask)
    {
      HAL_mpmint_Enable(32 * nMaskIndex + i);
    }
    nWorkingMask = (nWorkingMask << 1) | (nWorkingMask >> 31);
  }

} /* END HAL_mpmint_Mask_Enable */


/* ===========================================================================
**  HAL_mpmint_Mask_Disable
**
** ======================================================================== */

void HAL_mpmint_Mask_Disable
(
  uint32 nMaskIndex,
  uint32 nMask
)
{
  uint32 nNumRegs, nWorkingMask, i;

  HAL_mpmint_GetNumberMasks(&nNumRegs);

  if(nMaskIndex > nNumRegs)
    return;

  // This is a simplified implementation; if blazing speed is necessary this
  // can likely be made faster by unrolling the multiple Disable calls into:
  //   - a write per pad enable register
  //   - a write to the enable register
  nWorkingMask = 1;
  for(i = 0; i < 32 && nMask; i++)
  {
    if(nMask & nWorkingMask)
    {
      HAL_mpmint_Disable(32 * nMaskIndex + i);
    }
    nWorkingMask = (nWorkingMask << 1) | (nWorkingMask >> 31);
  }

} /* END HAL_mpmint_Mask_Disable */


/* ===========================================================================
**  HAL_mpmint_Mask_Clear
**
** ======================================================================== */

void HAL_mpmint_Mask_Clear
(
  uint32 nMaskIndex,
  uint32 nMask
)
{
  uint32 nNumRegs, nReg;

  HAL_mpmint_GetNumberMasks(&nNumRegs);

  if(nMaskIndex > nNumRegs)
    return;

  nReg = registerLayout.nClearReg + (sizeof(uint32) * nMaskIndex);
  outpdw(nReg, nMask);

} /* END HAL_mpmint_Mask_Clear */


/* ===========================================================================
**  HAL_mpmint_GetPending
**
** ======================================================================== */

void HAL_mpmint_GetPending
(
  uint32 nMaskIndex,
  uint32 *pnMask
)
{
  uint32 nNumRegs, nReg, nEnable;

  HAL_mpmint_GetNumberMasks(&nNumRegs);

  if((nMaskIndex > nNumRegs) || (!pnMask))
    return;

  nReg = registerLayout.nStatusReg + (sizeof(uint32) * nMaskIndex);
  nEnable = registerLayout.nEnableReg + (sizeof(uint32) * nMaskIndex);
  *pnMask = ( inpdw(nReg) & inpdw(nEnable) );

} /* HAL_mpmint_GetPending */

void HAL_mpmint_GetIndex(uint32 nDetect, uint32 nDetect3, uint32 nPolarity, uint32 nMask, HAL_mpmint_TriggerType *trigger)
{
  uint32 index = 0;
  if (nDetect & nMask)
    index |= 1;
  if (nDetect3 & nMask)
    index |= 2;
  if (nPolarity & nMask)
    index |= 4;

  *trigger = mpmint_trigger_lookup[index];
}


/* ===========================================================================
**  HAL_mpmint_GetTrigger
**
** ======================================================================== */

void HAL_mpmint_GetTrigger
(
  uint32                   nMPM,
  HAL_mpmint_TriggerType *peTrigger
)
{
  uint32 nBitPos, nMask, nRegDet, nRegDet3, nRegPol;
  uint32 index = 0;

  nBitPos  = nMPM;
  nMask    = MAKE_MASK(nBitPos);
  nRegDet  = registerLayout.nDetectReg + (sizeof(uint32) * (nBitPos / 32));
  nRegDet3 = registerLayout.nDetectReg3 + (sizeof(uint32) * (nBitPos / 32));
  nRegPol  = registerLayout.nPolarityReg + (sizeof(uint32) * (nBitPos / 32));

  HAL_mpmint_GetIndex(inpdw(nRegDet), inpdw(nRegDet3), inpdw(nRegPol), nMask, peTrigger);
  
  *peTrigger = mpmint_trigger_lookup[index];

  /*
   * Update the shadow in case it was incorrect
   */
  aInterruptTable[nMPM].eTrigger = *peTrigger;

} /* HAL_mpmint_GetTrigger */


/* ===========================================================================
**  HAL_mpmint_SetTrigger
**
** ======================================================================== */

void HAL_mpmint_SetTrigger
(
  uint32                   nMPM,
  HAL_mpmint_TriggerType  eTrigger
)
{
  uint32 nBitPos, nMask, nReg, nVal;
  uint32 index = (uint32)mpmint_trigger_lookup[eTrigger];

  /*
   * Configure edge bit 1
   */
  nBitPos = nMPM;
  nMask   = MAKE_MASK(nBitPos);
  nReg    = registerLayout.nDetectReg + (sizeof(uint32) * (nBitPos / 32));
  nVal    = inpdw(nReg);
  if (DET_A_FROM_IDX(index))
  {
    nVal |= nMask;
  }
  else
  {
    nVal &= ~nMask;
  }
  outpdw(nReg, nVal);

  /*
   *  Configure edge bit 2
   */
  nReg    = registerLayout.nDetectReg3 + (sizeof(uint32) * (nBitPos / 32));
  nVal    = inpdw(nReg);
  if (DET_B_FROM_IDX(index))
  {
    nVal |= nMask;
  }
  else
  {
    nVal &= ~nMask;
  }
  outpdw(nReg, nVal);

  /*
   * Configure the polarity
   */
  nReg = registerLayout.nPolarityReg + (sizeof(uint32) * (nBitPos / 32));
  nVal = inpdw(nReg);
  if (POL_FROM_IDX(index))
  {
    nVal |= nMask;
  }
  else
  {
    nVal &= ~nMask;
  }
  outpdw(nReg, nVal);

  /*
   * Save the new value
   */
  aInterruptTable[nMPM].eTrigger = eTrigger;

} /* END HAL_mpmint_SetTrigger */


/* ===========================================================================
**  HAL_mpmint_GetNumber
**
** ======================================================================== */

void HAL_mpmint_GetNumber
(
  uint32 *pnNumber
)
{

  /* MPM interrupt controller supports HAL_MPMINT_NUM interrupts */
  *pnNumber = HAL_MPMINT_NUM;

} /* END HAL_mpmint_GetNumber */


/* ===========================================================================
**  HAL_mpmint_GetNumberPhysical
**
** ======================================================================== */
void HAL_mpmint_GetNumberPhysical
(
  uint32 *pnNumber
)
{
  /* the correct number is defined by the platform */
  *pnNumber = HAL_MPMINT_PHYS_INTS;
} /* HAL_mpmint_GetNumberPhysical */


/* ===========================================================================
**  HAL_mpmint_GetPhysNumber
**
** ======================================================================== */
bool32 HAL_mpmint_GetPhysNumber
(
  uint32 nMPM,
  uint32 *pnPhys
)
{

  if(HAL_mpmint_IsSupported(nMPM))
  {
    *pnPhys = nMPM;
    return TRUE;
  }

  return FALSE;
} /* HAL_mpmint_GetPhysNumber */


/* ===========================================================================
**  HAL_mpmint_GetEnumNumber
**
** ======================================================================== */
bool32 HAL_mpmint_GetEnumNumber
(
  uint32 nPhys,
  uint32 *pnMPM
)
{
  /* current HAL uses a 1:1 mapping between phys and enum */
  uint32 nMPM = nPhys;

  if(HAL_mpmint_IsSupported(nMPM))
  {
    *pnMPM = nMPM;
    return TRUE;
  }

  return FALSE;
} /* HAL_mpmint_GetEnumNumber */


/* ===========================================================================
**  HAL_mpmint_IsSupported
**
** ======================================================================== */

bool32 HAL_mpmint_IsSupported
(
  uint32 nMPM
)
{
  return ( nMPM < HAL_MPMINT_NUM );
} /* END HAL_mpmint_IsSupported */


/* ===========================================================================
**  HAL_mpmint_IsGpioSupported
**
** ======================================================================== */

bool32 HAL_mpmint_IsGpioSupported
(
  uint32 nGPIO,
  uint32 *pnMPM
)
{
  uint32 nMPM;

  /*
   * Check to see if this GPIO is supported as a wakeup, and if so, 
   * return the MPM interrupt that it corresponds to.
   */ 
  for ( nMPM = 0; nMPM < HAL_MPMINT_NUM; nMPM++ )
  {
    if ( aInterruptTable[nMPM].gpio == nGPIO )
    {
      break;
    }
  }

  if ( nMPM < HAL_MPMINT_NUM )
  {
    *pnMPM = nMPM;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/* ===========================================================================
**  HAL_mpmint_IsGpio
**
** ======================================================================== */

bool32 HAL_mpmint_IsGpio
(
  uint32 nMPM,
  uint32 *pnGPIO
)
{
  bool32 retval = FALSE;

  if ( aInterruptTable[nMPM].gpio != HAL_MPMINT_INVALID_GPIO )
  {
    *pnGPIO = aInterruptTable[nMPM].gpio;
    retval = TRUE;
  }

  return retval;
} /* HAL_mpmint_IsGpio */


/* ===========================================================================
**  HAL_mpmint_IsPending
**
** ======================================================================== */

bool32 HAL_mpmint_IsPending
(
  uint32 nMPM
)
{
  uint32 nBitPos, nMask, nStatusReg, nEnableReg, nRegOffset;
  bool32 bPending = FALSE;

  nBitPos    = nMPM;
  nMask      = MAKE_MASK(nBitPos);
  nRegOffset = sizeof(uint32) * (nBitPos / 32);
  nStatusReg = registerLayout.nStatusReg + nRegOffset;
  nEnableReg = registerLayout.nEnableReg + nRegOffset;

  if (inpdw(nStatusReg) & inpdw(nEnableReg) & nMask)
  {
    bPending = TRUE;
  }

  /*
   * Return if we are set
   */
  return bPending;

} /* END HAL_mpmint_IsPending */


/* ===========================================================================
**  HAL_mpmint_IsEnabled
**
** ======================================================================== */

bool32 HAL_mpmint_IsEnabled
(
  uint32 nMPM
)
{
  uint32 nBitPos, nMask, nReg;
  bool32 bEnabled = FALSE;

  nBitPos = nMPM;
  nMask   = MAKE_MASK(nBitPos);
  nReg    = registerLayout.nEnableReg + (sizeof(uint32) * (nBitPos / 32));

  if (inpdw(nReg) & nMask)
  {
    bEnabled = TRUE;
  }

  /*
   * Return if we are set
   */
  return bEnabled;

} /* END HAL_mpmint_IsEnabled */

