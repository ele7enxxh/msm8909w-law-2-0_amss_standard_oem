/*
===========================================================================

FILE:         HALmpmintPlatform.c

DESCRIPTION:
  This is the platform hardware abstraction layer implementation for the
  MPM interrupt controller block.
  This platform is for the RPM on 9x45.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/source/9x45/HALmpmintTable.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2014 Qualcomm Technologies Incorporated.
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
  DEFINE_IRQ( RISING,  86,                       32 + 16                    ), /* HAL_MPMINT_GPIO86_ISR                               */
  DEFINE_IRQ( RISING,  91,                       32 + 19                    ), /* HAL_MPMINT_GPIO91_ISR                               */
  DEFINE_IRQ( RISING,  1,                        0                          ), /* HAL_MPMINT_GPIO1_ISR                              05*/
  DEFINE_IRQ( RISING,  2,                        1                          ), /* HAL_MPMINT_GPIO2_ISR                                */
  DEFINE_IRQ( RISING,  92,                       32 + 20                    ), /* HAL_MPMINT_GPIO92_ISR                               */
  DEFINE_IRQ( RISING,  93,                       32 + 21                    ), /* HAL_MPMINT_GPIO93_ISR                               */
  DEFINE_IRQ( RISING,  5,                        2                          ), /* HAL_MPMINT_GPIO5_ISR                                */
  DEFINE_IRQ( RISING,  6,                        3                          ), /* HAL_MPMINT_GPIO6_ISR                              10*/
  DEFINE_IRQ( RISING,  94,                       32 + 22                    ), /* HAL_MPMINT_GPIO94_ISR                               */
  DEFINE_IRQ( RISING,  95,                       32 + 23                    ), /* HAL_MPMINT_GPIO95_ISR                               */
  DEFINE_IRQ( RISING,  9,                        4                          ), /* HAL_MPMINT_GPIO9_ISR                                */
  DEFINE_IRQ( RISING,  10,                       5                          ), /* HAL_MPMINT_GPIO10_ISR                               */
  DEFINE_IRQ( RISING,  11,                       6                          ), /* HAL_MPMINT_GPIO11_ISR                             15*/
  DEFINE_IRQ( RISING,  12,                       7                          ), /* HAL_MPMINT_GPIO12_ISR                               */
  DEFINE_IRQ( RISING,  13,                       8                          ), /* HAL_MPMINT_GPIO13_ISR                               */
  DEFINE_IRQ( RISING,  14,                       9                          ), /* HAL_MPMINT_GPIO14_ISR                               */
  DEFINE_IRQ( RISING,  88,                       32 + 18                    ), /* HAL_MPMINT_GPIO88_ISR                               */
  DEFINE_IRQ( RISING,  16,                       10                         ), /* HAL_MPMINT_GPIO16_ISR                             20*/
  DEFINE_IRQ( RISING,  17,                       11                         ), /* HAL_MPMINT_GPIO17_ISR                               */
  DEFINE_IRQ( RISING,  18,                       12                         ), /* HAL_MPMINT_GPIO18_ISR                               */
  DEFINE_IRQ( RISING,  19,                       13                         ), /* HAL_MPMINT_GPIO19_ISR                               */
  DEFINE_IRQ( RISING,  22,                       15                         ), /* HAL_MPMINT_GPIO22_ISR                               */
  DEFINE_IRQ( RISING,  21,                       14                         ), /* HAL_MPMINT_GPIO21_ISR                             25*/
  DEFINE_IRQ( RISING,  24,                       16                         ), /* HAL_MPMINT_GPIO24_ISR                               */
  DEFINE_IRQ( RISING,  25,                       17                         ), /* HAL_MPMINT_GPIO25_ISR                               */
  DEFINE_IRQ( RISING,  51,                       20                         ), /* HAL_MPMINT_GPIO51_ISR                               */
  DEFINE_IRQ( RISING,  87,                       32 + 17                    ), /* HAL_MPMINT_GPIO87_ISR                               */
  DEFINE_IRQ( RISING,  62,                       32 + 3                     ), /* HAL_MPMINT_GPIO62_ISR                             30*/
  DEFINE_IRQ( RISING,  63,                       32 + 4                     ), /* HAL_MPMINT_GPIO63_ISR                               */
  DEFINE_IRQ( RISING,  64,                       32 + 5                     ), /* HAL_MPMINT_GPIO64_ISR                               */
  DEFINE_IRQ( RISING,  65,                       32 + 6                     ), /* HAL_MPMINT_GPIO65_ISR                               */
  DEFINE_IRQ( RISING,  66,                       32 + 7                     ), /* HAL_MPMINT_GPIO66_ISR                               */
  DEFINE_IRQ( RISING,  67,                       32 + 8                     ), /* HAL_MPMINT_GPIO67_ISR                             35*/
  DEFINE_IRQ( RISING,  43,                       18                         ), /* HAL_MPMINT_GPIO43_ISR                               */
  DEFINE_IRQ( RISING,  50,                       19                         ), /* HAL_MPMINT_GPIO50_ISR                               */
  DEFINE_IRQ( RISING,  53,                       21                         ), /* HAL_MPMINT_GPIO53_ISR                               */
  DEFINE_IRQ( RISING,  54,                       22                         ), /* HAL_MPMINT_GPIO54_ISR                               */
  DEFINE_IRQ( RISING,  55,                       23                         ), /* HAL_MPMINT_GPIO55_ISR                             40*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_41_ISR                                   */
  DEFINE_IRQ( RISING,  56,                       24                         ), /* HAL_MPMINT_GPIO56_ISR                               */
  DEFINE_IRQ( RISING,  82,                       32 + 12                    ), /* HAL_MPMINT_GPIO82_ISR                               */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  32 + 25                    ), /* HAL_MPMINT_SDC1_DAT1_ISR                            */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  32 + 26                    ), /* HAL_MPMINT_SDC1_DAT3_ISR                          45*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  32 + 24                    ), /* HAL_MPMINT_SRST_N_ISR                               */
  DEFINE_IRQ( RISING,  77,                       32 + 11                    ), /* HAL_MPMINT_GPIO77_ISR                               */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_48_ISR                                   */
  DEFINE_IRQ( FALLING, HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_USB2_PHY_DPSE_HV_ISR                     */
  DEFINE_IRQ( RISING,  83,                       32 + 13                    ), /* HAL_MPMINT_GPIO83_ISR                             50*/
  DEFINE_IRQ( RISING,  84,                       32 + 14                    ), /* HAL_MPMINT_GPIO84_ISR                               */
  DEFINE_IRQ( RISING,  85,                       32 + 15                    ), /* HAL_MPMINT_GPIO85_ISR                               */
  DEFINE_IRQ( RISING,  58,                       32 + 0                     ), /* HAL_MPMINT_GPIO58_ISR                               */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_PCIE_USB3_LFPS_ISR                       */
  DEFINE_IRQ( RISING,  81,                       32 + 1                     ), /* HAL_MPMINT_GPIO81_ISR                             55*/
  DEFINE_IRQ( RISING,  60,                       32 + 2                     ), /* HAL_MPMINT_GPIO60_ISR                               */
  DEFINE_IRQ( RISING,  68,                       32 + 9                     ), /* HAL_MPMINT_GPIO68_ISR                               */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_USB2_PHY_DMSE_HV_ISR                     */
  DEFINE_IRQ( RISING,  70,                       32 + 10                    ), /* HAL_MPMINT_GPIO70_ISR                               */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_UIM_CARD_ISR                           60*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_UIM_BATT_ISR                             */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_SPMI_WAKE_ISR                            */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_USB30_POWER_EVENT_ISR                    */
};

