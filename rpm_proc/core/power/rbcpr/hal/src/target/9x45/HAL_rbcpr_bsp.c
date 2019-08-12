/*
===========================================================================

FILE:         HAL_rbcpr_bsp.c

DESCRIPTION:
  Per target rbcpr bsp configurations

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/rbcpr/hal/src/target/9x45/HAL_rbcpr_bsp.c#1 $
$Date: 2015/02/25 $

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

#include "comdef.h"
#include "msmhwio.h"
#include "HAL_rbcpr.h"
#include "HAL_rbcpr_hwio.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

const HAL_rbcpr_bsp_type HAL_rbcpr_bsp_data =
{
    .rails = (const HAL_rbcpr_rail_s[])
    {
        // VDDMX
        {
            .timer_config =
            {
                .interval     = 5,          // time in msecs
                .enable       = TRUE,
            },
            .sensors          = 64,         // ?
            .settings =
            {
                .floor        = 0x00,
                .ceiling      = 0x3F,
                .vlevel       = 0x20,
            },
            .name             = "vddmx",
            .hw_base_address  = MX_RBCPR_MX_WRAPPER_BASE,
        },

        // VDDCX
        {
            .timer_config =
            {
                .interval     = 5,          // time in msecs
                .enable       = TRUE,
            },
            .sensors          = 64,         // ?
            .settings =
            {
                .floor        = 0x00,
                .ceiling      = 0x3F,
                .vlevel       = 0x20,
            },
            .name             = "vddcx",
            .hw_base_address  = RBCPR_WRAPPER_BASE,
        },
    },
    .num_rails = 2,
};

