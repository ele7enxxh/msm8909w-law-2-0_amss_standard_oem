/*
===========================================================================

FILE:         railway_config.c

DESCRIPTION:
  Per target railway configurations

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/railway_v2/src/8916/railway_config.c#1 $
$Date: 2015/02/25 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2012 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "CoreVerify.h"
#include "kvp.h"
#include "msmhwio.h"
#include "rpmserver.h"
#include "railway_config.h"
#include "pm_version.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

//
// BEGIN config data; should migrate to the system enumeration data method
//
static const railway_config_data_t temp_config_data =
{
    .rails     = (railway_rail_config_t[])
    {
        // Must init VDDMX first, as voting on the other rails will cause Mx changes to occur.
        {
            .rail_type      = RAILWAY_RAIL_TYPE_MX,
            .vreg_name      = "vddmx",

            .vreg_type      = RPM_LDO_A_REQ,
            .vreg_num       = 3,

            .pmic_step_size = 12500,    // not used

            .initial_corner = RAILWAY_NOMINAL,

            .supports_explicit_voltage_requests = true,
            .default_uvs = (const unsigned[])
            {
                0,                      // RAILWAY_NO_REQUEST
                750000,                 // RAILWAY_RETENTION
                1050000,                // RAILWAY_SVS_KRAIT
                1050000,                // RAILWAY_SVS_SOC
                1150000,                // RAILWAY_NOMINAL
                1287500,                // RAILWAY_TURBO
                1287500,                // RAILWAY_TURBO_HIGH
                1287500,                // RAILWAY_SUPER_TURBO
                1287500,                // RAILWAY_SUPER_TURBO_NO_CPR
            },

            .supported_corners = (railway_corner[])
            {
                RAILWAY_RETENTION,
                RAILWAY_SVS_SOC,
                RAILWAY_NOMINAL,
                RAILWAY_SUPER_TURBO,
            },
            .supported_corners_count = 4,
        },

        // VDDCX
        {
            .rail_type      = RAILWAY_RAIL_TYPE_LOGIC,
            .vreg_name      = "vddcx",

            .vreg_type      = RPM_SMPS_A_REQ,
            .vreg_num       = 1,

            .pmic_step_size = 12500,     // not used

            .initial_corner = RAILWAY_NOMINAL,

            .supports_explicit_voltage_requests = true,

            .default_uvs = (const unsigned[])
            {
                0,                      // RAILWAY_NO_REQUEST
                700000,                 // RAILWAY_RETENTION
                1050000,                // RAILWAY_SVS_KRAIT
                1050000,                // RAILWAY_SVS_SOC
                1150000,                // RAILWAY_NOMINAL
                1287500,                // RAILWAY_TURBO
                1287500,                // RAILWAY_TURBO_HIGH
                1287500,                // RAILWAY_SUPER_TURBO
                1287500,                // RAILWAY_SUPER_TURBO_NO_CPR
            },

            .supported_corners = (railway_corner[])
            {
                RAILWAY_RETENTION,
                RAILWAY_SVS_SOC,
                RAILWAY_NOMINAL,
                RAILWAY_SUPER_TURBO,
            },
            .supported_corners_count = 4,
        },
    },

    .num_rails = 2,
};
//
// END config data
//

const railway_config_data_t * const RAILWAY_CONFIG_DATA = &temp_config_data;

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

// TODO: This can be avoided if railway_init() is after pm_init()
// TODO: Make rail_state.corner_uvs a copy of rail_config.default_uvs,
//       instead of a pointer to rail_config.default_uvs
void railway_init_early_proxy_votes(void)
{
}

void railway_init_proxies_and_pins(void)
{
}

void railway_target_init(void)
{
}

