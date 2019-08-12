/*
===========================================================================

FILE:         railway_config.c

DESCRIPTION:
  Per target railway configurations

===========================================================================

                             Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/power/railway/src/8084/railway_config.c#1 $
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
            .vreg_name      = "vddmx",

            //SMPSA_1
            .pmic_rail_type = RAILWAY_SMPS_TYPE,
            .pmic_chip_id   = 0,
            .pmic_peripheral_index = PM_SMPS_1,
    
            .pmic_step_size = 5000,

            .initial_corner = RAILWAY_NOMINAL,

            .default_uvs = (const unsigned[])
            {
                0,                      // RAILWAY_NO_REQUEST
                675000,                 // RAILWAY_RETENTION
                950000,                 // RAILWAY_SVS_KRAIT
                950000,                 // RAILWAY_SVS_SOC
                990000,                 // RAILWAY_NOMINAL - rounded 987500
                1125000,                // RAILWAY_TURBO
                1125000,                // RAILWAY_SUPER_TURBO
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
            .vreg_name      = "vddcx",

            //SMPSA_2
            .pmic_rail_type = RAILWAY_SMPS_TYPE,
            .pmic_chip_id   = 0,
            .pmic_peripheral_index = PM_SMPS_2,
            
            .pmic_step_size = 5000,

            .initial_corner = RAILWAY_NOMINAL,

            .default_uvs = (const unsigned[])
            {
                0,                      // RAILWAY_NO_REQUEST
                500000,                 // RAILWAY_RETENTION
                725000,                 // RAILWAY_SVS_KRAIT
                875000,                 // RAILWAY_SVS_SOC
                990000,                 // RAILWAY_NOMINAL - rounded 987500
                1125000,                // RAILWAY_TURBO
                1125000,                // RAILWAY_SUPER_TURBO
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

