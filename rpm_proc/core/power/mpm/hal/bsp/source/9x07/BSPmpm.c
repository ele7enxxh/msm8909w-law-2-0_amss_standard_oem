/*
===========================================================================

FILE:         BSPmpm.c

DESCRIPTION:
  This is the platform hardware abstraction layer implementation for the
  MPM hardware block.
  This platform is for the RPM on the 8909.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/bsp/source/9x07/BSPmpm.c#1 $

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

#include <stdint.h>
#include "BSPmpm.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

/* wakeup delays for CXO */
static const uint32_t cxoDelay = 0xa;

/* target backoff for deep sleep exit in 19.2Mhz ticks */
/*                                        VDD min   */
static const uint32_t deepSleepExitDelay = 3578;

/*
 * Target specific MPM Hardware configuration.
 */
BSP_mpm_ConfigDataType MPM_BSP_DATA =
{
  /* MPM Configuration */
  {
    /* Wakeup Delays */
    {
      cxoDelay, deepSleepExitDelay
    },

    /* IO Cfg */
    {
      /* Freeze  Clamp  SW Ebi1     Warm Boot    Warm Boot 
       * IOs     IOs    Ctl Enable  Freeze EBI1  Freeze EBI2 */
      TRUE,     TRUE, TRUE,       TRUE,        TRUE
    },

    /* Debug Through Power Collapse */
    FALSE,

    /* Debug Bus Enable (bit 0)     Debug Bus Enable (bit 1) */  
    FALSE,                          FALSE,
  },

  /* Peripherals Configuration */
  {
    /* Pen Debounce Cfg */
    {
      /* Enable */
      FALSE,

      /* Delay */
      HAL_MPM_PEN_DEBOUNCE_DELAY_0400US
    },
  },

  /* Voltage Rail Address Configuration */
  {
    0x1740, // vdd_dig 0x1700 base, +0x40 for voltage ctrl offset
    0x1440, // vdd_mem 0x1400 base, +0x40 for voltage ctrl offset
    0x3,  //vdd_dig slave ID
    0x3,  //vdd_mem slave ID
	1500, //step size: 1.5mV/uS
  }

}; /* End BSP_mpm_ConfigData */

