#ifndef HAL_MPMINT_INTERNAL_H
#define HAL_MPMINT_INTERNAL_H

/*
===========================================================================

FILE:         HALmpmintInternal.h

DESCRIPTION:  
  This is the hardware abstraction layer internal interface for the MPM
  interrupt controller block.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/source/common/HALmpmintInternal.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
08/06/09   ajf     Rename mao --> mpm.
06/30/09   taw     Added a field for the TLMM pad enable register.
01/09/09   taw     Added HAL_maoint_PlatformIntType, which contains the
                   properties for each interrupt.
03/20/08   kr      Created.
05/27/08   kr      Modifications based on code review

===========================================================================
             Copyright © 2008 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "HALmpmint.h"

/* -----------------------------------------------------------------------
**                         CONSTANTS
** ----------------------------------------------------------------------- */

#define HAL_MPMINT_PHYS_INTS 64

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
 * HAL_mpmint_PlatformType
 *
 * Platform (asic/processor) specific information filled in by the platform
 * code in HAL_pic_PlatformEarlyInit.
 *
 *  nPhysicalInts: The number of physical interrupts supported by the MPM
 *                 interrupt controller.
 *  nEnableReg:    The base of the interrupt enable register array.
 *  nEnableRegPad: The base of TLMM's wakeup interrupt enable register array.
 *  nStatusReg:    The base of the interrupt status register array.
 *  nDetectReg:    The base of the interrupt detect ctl register array.
 *  nPolarityReg:  The base of the interrupt polarity register array.
 */
typedef struct
{
  uint32 nEnableReg;
  uint32 nEnableRegPad;
  uint32 nStatusReg;
  uint32 nClearReg;
  uint32 nDetectReg;
  uint32 nDetectReg3;
  uint32 nPolarityReg;
} HAL_mpmint_PlatformType;

/*
 * HAL_mpmint_PlatformIntType
 *
 * The description of an mpm interrupt.
 *
 *  eTrigger:      trigger type (edge, level, positive, negative)
 *  gpio:          which GPIO this interrupt is associated with
 *  nPadMask:      mask to use for this interrupt in the TLMM pad enables
 */
typedef struct
{
  HAL_mpmint_TriggerType   eTrigger;
  uint8                    gpio;
  uint8                    padBitPos;
} HAL_mpmint_PlatformIntType;


/* -----------------------------------------------------------------------
**                         DECLARATIONS
** ----------------------------------------------------------------------- */

extern HAL_mpmint_PlatformType    registerLayout;
extern HAL_mpmint_PlatformIntType aInterruptTable[];


extern void vmpm_send_interrupt(void);

#endif /* HAL_MPMINT_INTERNAL_H */


