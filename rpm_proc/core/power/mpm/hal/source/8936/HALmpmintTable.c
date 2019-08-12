/*
===========================================================================

FILE:         HALmpmintPlatform.c

DESCRIPTION:
  This is the platform hardware abstraction layer implementation for the
  MPM interrupt controller block.
  This platform is for the RPM on 8936.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/source/8936/HALmpmintTable.c#1 $

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
  DEFINE_IRQ( RISING,  108,                      24                         ), /* HAL_MPMINT_GPIO108_ISR                              */
  DEFINE_IRQ( RISING,  1,                        0                          ), /* HAL_MPMINT_GPIO1_ISR                                */
  DEFINE_IRQ( RISING,  5,                        1                          ), /* HAL_MPMINT_GPIO5_ISR                              05*/
  DEFINE_IRQ( RISING,  9,                        32+3                       ), /* HAL_MPMINT_GPIO9_ISR                                */
  DEFINE_IRQ( RISING,  107,                      23                         ), /* HAL_MPMINT_GPIO107_ISR                              */
  DEFINE_IRQ( RISING,  98,                       22                         ), /* HAL_MPMINT_GPIO98_ISR                               */
  DEFINE_IRQ( RISING,  97,                       21                         ), /* HAL_MPMINT_GPIO97_ISR                               */
  DEFINE_IRQ( RISING,  11,                       32+0                       ), /* HAL_MPMINT_GPIO11_ISR                             10*/
  DEFINE_IRQ( RISING,  69,                       20                         ), /* HAL_MPMINT_GPIO69_ISR                               */
  DEFINE_IRQ( RISING,  12,                       2                          ), /* HAL_MPMINT_GPIO12_ISR                               */
  DEFINE_IRQ( RISING,  13,                       3                          ), /* HAL_MPMINT_GPIO13_ISR                               */
  DEFINE_IRQ( RISING,  20,                       4                          ), /* HAL_MPMINT_GPIO20_ISR                               */
  DEFINE_IRQ( RISING,  62,                       19                         ), /* HAL_MPMINT_GPIO62_ISR                             15*/
  DEFINE_IRQ( RISING,  54,                       18                         ), /* HAL_MPMINT_GPIO54_ISR                               */
  DEFINE_IRQ( RISING,  21,                       5                          ), /* HAL_MPMINT_GPIO21_ISR                               */
  DEFINE_IRQ( RISING,  52,                       17                         ), /* HAL_MPMINT_GPIO52_ISR                               */
  DEFINE_IRQ( RISING,  25,                       6                          ), /* HAL_MPMINT_GPIO25_ISR                               */
  DEFINE_IRQ( RISING,  51,                       16                         ), /* HAL_MPMINT_GPIO51_ISR                             20*/
  DEFINE_IRQ( RISING,  50,                       15                         ), /* HAL_MPMINT_GPIO50_ISR                               */
  DEFINE_IRQ( RISING,  28,                       7                          ), /* HAL_MPMINT_GPIO28_ISR                               */
  DEFINE_IRQ( RISING,  31,                       8                          ), /* HAL_MPMINT_GPIO31_ISR                               */
  DEFINE_IRQ( RISING,  34,                       9                          ), /* HAL_MPMINT_GPIO34_ISR                               */
  DEFINE_IRQ( RISING,  35,                       10                         ), /* HAL_MPMINT_GPIO35_ISR                             25*/
  DEFINE_IRQ( RISING,  36,                       11                         ), /* HAL_MPMINT_GPIO36_ISR                               */
  DEFINE_IRQ( RISING,  37,                       12                         ), /* HAL_MPMINT_GPIO37_ISR                               */
  DEFINE_IRQ( RISING,  38,                       13                         ), /* HAL_MPMINT_GPIO38_ISR                               */
  DEFINE_IRQ( RISING,  49,                       14                         ), /* HAL_MPMINT_GPIO49_ISR                               */
  DEFINE_IRQ( RISING,  109,                      25                         ), /* HAL_MPMINT_GPIO109_ISR                            30*/
  DEFINE_IRQ( RISING,  110,                      26                         ), /* HAL_MPMINT_GPIO110_ISR                              */
  DEFINE_IRQ( RISING,  111,                      27                         ), /* HAL_MPMINT_GPIO111_ISR                              */
  DEFINE_IRQ( RISING,  112,                      28                         ), /* HAL_MPMINT_GPIO112_ISR                              */
  DEFINE_IRQ( RISING,  113,                      29                         ), /* HAL_MPMINT_GPIO113_ISR                              */
  DEFINE_IRQ( RISING,  114,                      30                         ), /* HAL_MPMINT_GPIO114_ISR                            35*/
  DEFINE_IRQ( RISING,  115,                      31                         ), /* HAL_MPMINT_GPIO115_ISR                              */
  DEFINE_IRQ( RISING,  117,                      32+1                       ), /* HAL_MPMINT_GPIO117_ISR                              */
  DEFINE_IRQ( RISING,  118,                      32+2                       ), /* HAL_MPMINT_GPIO118_ISR                              */
  DEFINE_IRQ( RISING,  120,                      32+4                       ), /* HAL_MPMINT_GPIO120_ISR                              */
  DEFINE_IRQ( RISING,  121,                      32+5                       ), /* HAL_MPMINT_GPIO121_ISR                            40*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  32+8                       ), /* HAL_MPMINT_SDC1_DAT3_ISR                            */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  32+9                       ), /* HAL_MPMINT_SDC2_DAT1_ISR                            */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  32+10                      ), /* HAL_MPMINT_SDC2_DAT3_ISR                          45*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  32+6                       ), /* HAL_MPMINT_SRST_N_ISR                               */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_USB2_PHY_OTG_VDDPX3_0_ISR                */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_USB2_PHY_ID_VDDPX3_0_ISR                 */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_USB2_PHY_DPSE_0_ISR                      */
  DEFINE_IRQ( RISING,  66,                       32+11                      ), /* HAL_MPMINT_GPIO66_ISR                             50*/
  DEFINE_IRQ( RISING,  68,                       32+12                      ), /* HAL_MPMINT_GPIO68_ISR                               */
  DEFINE_IRQ( RISING,  65,  					 32+13						), /* HAL_MPMINT_GPIO65_ISR                                              */
  DEFINE_IRQ( RISING,  53,  					 32+14						), /* HAL_MPMINT_GPIO53_ISR                                               */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              55*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_USB2_PHY_DMSE_0                          */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_UIM_CARD_ISR                             60*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_UIM_BATT_ISR                             */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_SPMI_WAKE_ISR                            */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                                              */
};

