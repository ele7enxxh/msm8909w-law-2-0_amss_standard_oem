/*
===========================================================================

FILE:         HAL_rbcpr_bsp.c

DESCRIPTION:
  Per target rbcpr bsp configurations

===========================================================================

                             Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/power/cpr/hal/src/target/8956/HAL_rbcpr_bsp.c#1 $
$Date: 2015/09/01 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "msmhwio.h"
#include "HAL_rbcpr.h"
#include "HAL_rbcpr_hwio.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

// TODO: CPR0_REG_RED_SEL is QFPROM_RAW_SW_CALIB_ROW0_MSB
const HAL_rbcpr_bsp_type HAL_rbcpr_bsp_data =
{
    .rails = (const HAL_rbcpr_rail_s[])
    {
        {   //VddCx
            .timer_config =
            {
                .interval = 5,          // time in msecs
                .enable   = TRUE,
            },
            .sensors = 64,
            .settings = 
            {
                .floor   = 0,
                .ceiling = 0x3F,
                .vlevel  = 0x20,
            },
            .name = "vddcx",
            .hw_base_address = RBCPR_WRAPPER_REG_BASE,
        },
    },
    .num_rails = 1,
};

