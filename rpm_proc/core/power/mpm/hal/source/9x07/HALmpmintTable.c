/*
===========================================================================

FILE:         HALmpmintPlatform.c

DESCRIPTION:
  This is the platform hardware abstraction layer implementation for the
  MPM interrupt controller block.
  This platform is for the RPM on 8909.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/source/9x07/HALmpmintTable.c#3 $

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
  DEFINE_IRQ( RISING,  16,                       8                          ), /* HAL_MPMINT_GPIO16_ISR                               */
  DEFINE_IRQ( RISING,  5,                        2                          ), /* HAL_MPMINT_GPIO5_ISR                                */
  DEFINE_IRQ( RISING,  11,                       5                          ), /* HAL_MPMINT_GPIO11_ISR                             05*/
  DEFINE_IRQ( RISING,  12,                       6                          ), /* HAL_MPMINT_GPIO12_ISR                               */
  DEFINE_IRQ( RISING,  3,                        1                          ), /* HAL_MPMINT_GPIO3_ISR                                */
  DEFINE_IRQ( RISING,  17,                       9                          ), /* HAL_MPMINT_GPIO17_ISR                               */   
  DEFINE_IRQ( RISING,  9,                        4                          ), /* HAL_MPMINT_GPIO9_ISR                                */
  DEFINE_IRQ( RISING,  13,                       7                          ), /* HAL_MPMINT_GPIO13_ISR                             10*/
  DEFINE_IRQ( RISING,  1,                        0                          ), /* HAL_MPMINT_GPIO1_ISR                                */
  DEFINE_IRQ( RISING,  20,                       10                         ), /* HAL_MPMINT_GPIO20_ISR                               */
  DEFINE_IRQ( RISING,  21,                       11                         ), /* HAL_MPMINT_GPIO21_ISR                               */
  DEFINE_IRQ( RISING,  22,                       12                         ), /* HAL_MPMINT_GPIO22_ISR                               */
  DEFINE_IRQ( RISING,  75,                       32+17                      ), /* HAL_MPMINT_GPIO75_ISR                             15*/
  DEFINE_IRQ( RISING,  74,                       32+16                      ), /* HAL_MPMINT_GPIO74_ISR                               */
  DEFINE_IRQ( RISING,  28,                       15                         ), /* HAL_MPMINT_GPIO28_ISR                               */
  DEFINE_IRQ( RISING,  44,                       25                         ), /* HAL_MPMINT_GPIO44_ISR                               */
  DEFINE_IRQ( RISING,  26,                       14                         ), /* HAL_MPMINT_GPIO26_ISR                               */
  DEFINE_IRQ( RISING,  43,                       24                         ), /* HAL_MPMINT_GPIO43_ISR                             20*/
  DEFINE_IRQ( RISING,  42,                       23                         ), /* HAL_MPMINT_GPIO42_ISR                               */
  DEFINE_IRQ( RISING,  29,                       16                         ), /* HAL_MPMINT_GPIO29_ISR                               */
  DEFINE_IRQ( RISING,  69,                       32+14                      ), /* HAL_MPMINT_GPIO69_ISR                               */
  DEFINE_IRQ( RISING,  30,                       17                         ), /* HAL_MPMINT_GPIO30_ISR                               */
  DEFINE_IRQ( RISING,  37,                       19                         ), /* HAL_MPMINT_GPIO37_ISR                             25*/
  DEFINE_IRQ( RISING,  25,                       13                         ), /* HAL_MPMINT_GPIO25_ISR                               */
  DEFINE_IRQ( RISING,  71,                       32+15                      ), /* HAL_MPMINT_GPIO71_ISR                               */
  DEFINE_IRQ( RISING,  34,                       18                         ), /* HAL_MPMINT_GPIO34_ISR                               */
  DEFINE_IRQ( RISING,  55,                       28                         ), /* HAL_MPMINT_GPIO55_ISR                               */
  DEFINE_IRQ( RISING,  8,                        3                          ), /* HAL_MPMINT_GPIO8_ISR                              30*/    
  DEFINE_IRQ( RISING,  40,                       21                         ), /* HAL_MPMINT_GPIO40_ISR                               */
  DEFINE_IRQ( RISING,  48,                       26                         ), /* HAL_MPMINT_GPIO48_ISR                               */
  DEFINE_IRQ( RISING,  52,                       27                         ), /* HAL_MPMINT_GPIO52_ISR                               */
  DEFINE_IRQ( RISING,  57,                       29                         ), /* HAL_MPMINT_GPIO57_ISR                               */
  DEFINE_IRQ( RISING,  62,                       32+11                      ), /* HAL_MPMINT_GPIO62_ISR                             35*/
  DEFINE_IRQ( RISING,  66,                       32+13                      ), /* HAL_MPMINT_GPIO66_ISR                               */
  DEFINE_IRQ( RISING,  59,                       30                      ), /* HAL_MPMINT_GPIO59_ISR                                  */
  DEFINE_IRQ( RISING,  79,                       32+19                      ), /* HAL_MPMINT_GPIO79_ISR                               */
  DEFINE_IRQ( RISING,  38,                       20                         ), /* HAL_MPMINT_GPIO38_ISR                               */
  DEFINE_IRQ( RISING,  63,                       32+12                      ), /* HAL_MPMINT_GPIO63_ISR                             40*/
  DEFINE_IRQ( RISING,  76,                       32+18                      ), /* HAL_MPMINT_GPIO76_ISR                               */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */  
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  32+9                       ), /* HAL_MPMINT_SDC2_DAT1_ISR                            */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  32+10                      ), /* HAL_MPMINT_SDC2_DAT3_ISR                          45*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  32+6                       ), /* HAL_MPMINT_SRST_N_ISR                               */
  DEFINE_IRQ( DUAL,    HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_SVLD_HV_ISR                              */
  DEFINE_IRQ( DUAL,    HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_ID_HV_ISR                                */
  DEFINE_IRQ( DUAL,    HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_DPSE_HV_ISR                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_STB_CORE_IN_ISR                        50*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_USB_ASYNC_WAKE_ISR                       */
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

