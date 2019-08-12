/*
===========================================================================

FILE:         railway_config.c

DESCRIPTION:
  Per target railway configurations

===========================================================================

                             Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/power/railway/src/8952/railway_config.c#1 $
$Date: 2015/09/01 $

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

#include "railway_config.h"
#include "pm_ldo.h"
#include "pm_smps.h"


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
            .rail_type = RAILWAY_RAIL_TYPE_MX,
            .vreg_name = "vddmx",

            //LDO3A
            .pmic_rail_type = RAILWAY_LDO_TYPE,
            .pmic_chip_id   = 0,
            .pmic_peripheral_index = PM_LDO_3,

            .pmic_step_size = 12500,

            .initial_corner = RAILWAY_NOMINAL,

            .default_uvs = (const uint32[])
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
            .rail_type = RAILWAY_RAIL_TYPE_LOGIC,
            .vreg_name = "vddcx",

            // SMPS2A
            .pmic_rail_type = RAILWAY_SMPS_TYPE,
            .pmic_chip_id   = 0,
            .pmic_peripheral_index = PM_SMPS_2,

            .pmic_step_size = 12500,

            .initial_corner = RAILWAY_NOMINAL,

            .default_uvs = (const uint32[])
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

void railway_target_init(void)
{
}

