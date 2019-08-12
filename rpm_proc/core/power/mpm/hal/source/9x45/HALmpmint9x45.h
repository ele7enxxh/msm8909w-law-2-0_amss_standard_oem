#ifndef HALMPMINT9x45_H
#define HALMPMINT9x45_H

/*
===========================================================================

FILE:         HALmpmint9x45.h

DESCRIPTION:
  Target-specific enumerations for HALmpmint.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/source/9x45/HALmpmint9x45.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2014 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
             QUALCOMM Proprietary and Confidential
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "comdef.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
 * HAL_mpmint_IsrType
 *
 * List of possible interrupt sources.  All of these are not necessarily
 * supported on the HW.
 *
 * NOTE: This list must be in the same order as the lists in the
 *       MPM driver.
 */
typedef enum
{
  /* Hard Wired */
  HAL_MPMINT_QTIMER_ISR                 = 0,
  HAL_MPMINT_PEN_ISR                    = 1,
  HAL_MPMINT_TSENS_UPPER_LOWER_ISR      = 2,

  /* High Voltage */
  HAL_MPMINT_GPIO86_ISR                 = 3,
  HAL_MPMINT_GPIO91_ISR                 = 4,
  HAL_MPMINT_GPIO1_ISR                  = 5,
  HAL_MPMINT_GPIO2_ISR                  = 6,
  HAL_MPMINT_GPIO92_ISR                 = 7,
  HAL_MPMINT_GPIO93_ISR                 = 8,
  HAL_MPMINT_GPIO5_ISR                  = 9,
  HAL_MPMINT_GPIO6_ISR                  = 10,
  HAL_MPMINT_GPIO94_ISR                 = 11,
  HAL_MPMINT_GPIO95_ISR                 = 12,
  HAL_MPMINT_GPIO9_ISR                  = 13,
  HAL_MPMINT_GPIO10_ISR                 = 14,
  HAL_MPMINT_GPIO11_ISR                 = 15,
  HAL_MPMINT_GPIO12_ISR                 = 16,
  HAL_MPMINT_GPIO13_ISR                 = 17,
  HAL_MPMINT_GPIO14_ISR                 = 18,
  HAL_MPMINT_GPIO88_ISR                 = 19,
  HAL_MPMINT_GPIO16_ISR                 = 20,
  HAL_MPMINT_GPIO17_ISR                 = 21,
  HAL_MPMINT_GPIO18_ISR                 = 22,
  HAL_MPMINT_GPIO19_ISR                 = 23,
  HAL_MPMINT_GPIO22_ISR                 = 24,
  HAL_MPMINT_GPIO21_ISR                 = 25,
  HAL_MPMINT_GPIO24_ISR                 = 26,
  HAL_MPMINT_GPIO25_ISR                 = 27,
  HAL_MPMINT_GPIO51_ISR                 = 28,
  HAL_MPMINT_GPIO87_ISR                 = 29,
  HAL_MPMINT_GPIO62_ISR                 = 30,
  HAL_MPMINT_GPIO63_ISR                 = 31,
  HAL_MPMINT_GPIO64_ISR                 = 32,
  HAL_MPMINT_GPIO65_ISR                 = 33,
  HAL_MPMINT_GPIO66_ISR                 = 34,
  HAL_MPMINT_GPIO67_ISR                 = 35,
  HAL_MPMINT_GPIO43_ISR                 = 36,
  HAL_MPMINT_GPIO50_ISR                 = 37,
  HAL_MPMINT_GPIO53_ISR                 = 38,
  HAL_MPMINT_GPIO54_ISR                 = 39,
  HAL_MPMINT_GPIO55_ISR                 = 40,
  HAL_MPMINT_41_ISR                     = 41, // Open
  HAL_MPMINT_GPIO56_ISR                 = 42,
  HAL_MPMINT_GPIO82_ISR                 = 43,
  HAL_MPMINT_SDC1_DAT1_ISR              = 44,
  HAL_MPMINT_SDC1_DAT3_ISR              = 45,
  HAL_MPMINT_SRST_N_ISR                 = 46,
  HAL_MPMINT_GPIO77_ISR                 = 47,
  HAL_MPMINT_48_ISR                     = 48, // Open
  HAL_MPMINT_USB2_PHY_DPSE_HV_ISR       = 49,
  HAL_MPMINT_GPIO83_ISR                 = 50,
  HAL_MPMINT_GPIO84_ISR                 = 51,
  HAL_MPMINT_GPIO85_ISR                 = 52,
  HAL_MPMINT_GPIO58_ISR                 = 53,
  HAL_MPMINT_PCIE_USB3_LFPS_ISR         = 54,
  HAL_MPMINT_GPIO81_ISR                 = 55,
  HAL_MPMINT_GPIO60_ISR                 = 56,
  HAL_MPMINT_GPIO68_ISR                 = 57,
  HAL_MPMINT_USB2_PHY_DMSE_HV_ISR       = 58,
  HAL_MPMINT_GPIO70_ISR                 = 59,

  /* Always On */
  HAL_MPMINT_UIM_CARD_ISR               = 60,
  HAL_MPMINT_UIM_BATT_ISR               = 61, // Open ?
  HAL_MPMINT_SPMI_WAKE_ISR              = 62,
  HAL_MPMINT_USB30_POWER_EVENT_ISR      = 63,
  HAL_MPMINT_NUM,

  /* Interrupts below this point are legacy definitions, not supported by this
   * version of the hardware. */
  HAL_MPMINT_NOT_DEFINED_ISR            = HAL_MPMINT_NUM,

} HAL_mpmint_IsrType;

#ifdef __cplusplus
}
#endif

#endif /* HALMPMINT9x45_H */

