/*
===========================================================================

FILE:         HALmpmintPlatform.c

DESCRIPTION:
  This is the platform hardware abstraction layer implementation for the
  MPM interrupt controller block.
  This platform is for the RPM on 8909.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/source/8909/HALmpmintTable.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include <HALmpmint.h>
#include "HALmpmintInternal.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

/*
 * DEFINE_IRQ
 *
 * Macro to define an IRQ mpmint_irq_data_type entry in the table
 */
#define DEFINE_IRQ( trigger, gpio, padPos ) \
  {                                                 \
    HAL_MPMINT_TRIGGER_##trigger,                   \
    gpio,                                           \
    padPos,                                         \
  }

/*
 * Target-specific interrupt configurations
 */
HAL_mpmint_PlatformIntType aInterruptTable[HAL_MPMINT_NUM] =
{
  /*          Trigger            GPIO                      Pad Bit          */
  /*          -------  -----------------------   -------------------------- */
  DEFINE_IRQ( HIGH,    HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_QTIMER_ISR                             00*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_PEN_ISR                                  */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_TSENS_UPPER_LOWER_ISR                    */
  DEFINE_IRQ( RISING,  65,                       29                         ), /* HAL_MPMINT_GPIO65_ISR                               */
  DEFINE_IRQ( RISING,  5,                        4                          ), /* HAL_MPMINT_GPIO5_ISR                                */
  DEFINE_IRQ( RISING,  11,                       6                          ), /* HAL_MPMINT_GPIO11_ISR                             05*/
  DEFINE_IRQ( RISING,  12,                       7                          ), /* HAL_MPMINT_GPIO12_ISR                               */
  DEFINE_IRQ( RISING,  64,                       28                         ), /* HAL_MPMINT_GPIO64_ISR                               */
  DEFINE_IRQ( RISING,  58,                       25                         ), /* HAL_MPMINT_GPIO58_ISR                               */   
  DEFINE_IRQ( RISING,  50,                       23                         ), /* HAL_MPMINT_GPIO50_ISR                               */
  DEFINE_IRQ( RISING,  13,                       8                          ), /* HAL_MPMINT_GPIO13_ISR                             10*/
  DEFINE_IRQ( RISING,  49,                       22                         ), /* HAL_MPMINT_GPIO49_ISR                               */
  DEFINE_IRQ( RISING,  20,                       24                         ), /* HAL_MPMINT_GPIO20_ISR                               */
  DEFINE_IRQ( RISING,  21,                       10                         ), /* HAL_MPMINT_GPIO21_ISR                               */
  DEFINE_IRQ( RISING,  25,                       12                         ), /* HAL_MPMINT_GPIO25_ISR                               */
  DEFINE_IRQ( RISING,  46,                       9                          ), /* HAL_MPMINT_GPIO46_ISR                             15*/
  DEFINE_IRQ( RISING,  45,                       20                         ), /* HAL_MPMINT_GPIO45_ISR                               */
  DEFINE_IRQ( RISING,  28,                       13                         ), /* HAL_MPMINT_GPIO28_ISR                               */
  DEFINE_IRQ( RISING,  44,                       11                         ), /* HAL_MPMINT_GPIO44_ISR                               */
  DEFINE_IRQ( RISING,  31,                       14                         ), /* HAL_MPMINT_GPIO31_ISR                               */
  DEFINE_IRQ( RISING,  43,                       5                          ), /* HAL_MPMINT_GPIO43_ISR                             20*/
  DEFINE_IRQ( RISING,  42,                       3                          ), /* HAL_MPMINT_GPIO42_ISR                               */
  DEFINE_IRQ( RISING,  34,                       15                         ), /* HAL_MPMINT_GPIO34_ISR                               */
  DEFINE_IRQ( RISING,  35,                       16                         ), /* HAL_MPMINT_GPIO35_ISR                               */
  DEFINE_IRQ( RISING,  36,                       17                         ), /* HAL_MPMINT_GPIO36_ISR                               */
  DEFINE_IRQ( RISING,  37,                       18                         ), /* HAL_MPMINT_GPIO37_ISR                             25*/
  DEFINE_IRQ( RISING,  38,                       19                         ), /* HAL_MPMINT_GPIO38_ISR                               */
  DEFINE_IRQ( RISING,  39,                       0                          ), /* HAL_MPMINT_GPIO39_ISR                               */
  DEFINE_IRQ( RISING,  40,                       1                          ), /* HAL_MPMINT_GPIO40_ISR                               */
  DEFINE_IRQ( RISING,  41,                       2                          ), /* HAL_MPMINT_GPIO41_ISR                               */
  DEFINE_IRQ( RISING,  90,                       30                         ), /* HAL_MPMINT_GPIO90_ISR                             30*/    
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
  DEFINE_IRQ( RISING,  91,                       31                         ), /* HAL_MPMINT_GPIO91_ISR                               */
  DEFINE_IRQ( RISING,  92,                       32+0                       ), /* HAL_MPMINT_GPIO92_ISR                               */
  DEFINE_IRQ( RISING,  94,                       32+2                       ), /* HAL_MPMINT_GPIO94_ISR                               */
  DEFINE_IRQ( RISING,  95,                       32+3                       ), /* HAL_MPMINT_GPIO95_ISR                             35*/
  DEFINE_IRQ( RISING,  96,                       32+4                       ), /* HAL_MPMINT_GPIO96_ISR                               */
  DEFINE_IRQ( RISING,  97,                       32+5                       ), /* HAL_MPMINT_GPIO97_ISR                               */
  DEFINE_IRQ( RISING,  98,                       32+7                       ), /* HAL_MPMINT_GPIO98_ISR                               */
  DEFINE_IRQ( RISING,  110,                      32+8                       ), /* HAL_MPMINT_GPIO110_ISR                              */
  DEFINE_IRQ( RISING,  111,                      26                         ), /* HAL_MPMINT_GPIO111_ISR                            40*/
  DEFINE_IRQ( RISING,  112,                      27                         ), /* HAL_MPMINT_GPIO112_ISR                              */
  DEFINE_IRQ( RISING,  105,                      32+11                      ), /* HAL_MPMINT_GPIO105_ISR                              */
  DEFINE_IRQ( RISING,  107,                      32+12                      ), /* HAL_MPMINT_GPIO107_ISR                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  32+9                       ), /* HAL_MPMINT_SDC2_DAT1_ISR                            */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  32+10                      ), /* HAL_MPMINT_SDC2_DAT3_ISR                          45*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  32+6                       ), /* HAL_MPMINT_SRST_N_ISR                               */
  DEFINE_IRQ( DUAL,    HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_SVLD_HV_ISR                              */
  DEFINE_IRQ( DUAL,    HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_ID_HV_ISR                                */
  DEFINE_IRQ( DUAL,    HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_DPSE_HV_ISR                              */
  DEFINE_IRQ( RISING,  47,                       32+1                       ), /* HAL_MPMINT_GPIO47_ISR                             50*/
  DEFINE_IRQ( RISING,  48,                       21                         ), /* HAL_MPMINT_GPIO48_ISR                               */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                            55*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
  DEFINE_IRQ( DUAL,    HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_DMSE_HV_ISR                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_UIM_CARD_ISR                           60*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_UIM_BATT_ISR                             */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_SPMI_WAKE_ISR                            */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
};

