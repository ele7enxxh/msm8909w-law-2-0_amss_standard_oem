/*
===========================================================================

FILE:         railway_config.c

DESCRIPTION:
  Per target railway configurations

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/railway_v2/src/9x07/railway_config.c#4 $
$Date: 2015/09/16 $

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
#include "railway_adapter.h"
#include "PlatformInfo.h"
#include "Chipinfo.h"

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
            .vreg_num       = 12,

            .pmic_step_size = 12500,    // not used

            .initial_corner = RAILWAY_NOMINAL,

            .supports_explicit_voltage_requests = true,
            .default_uvs = (const unsigned[])
            {
                0,                      // RAILWAY_NO_REQUEST
                800000,                 // RAILWAY_RETENTION
                1050000,                // RAILWAY_SVS_KRAIT
                1050000,                // RAILWAY_SVS_SOC
				1150000,                // RAILWAY_SVS_HIGH
                1225000,                // RAILWAY_NOMINAL
                1287500,                // RAILWAY_NOMINAL_HIGH
                1350000,                // RAILWAY_TURBO
                1350000,                // RAILWAY_SUPER_TURBO
                1350000,                // RAILWAY_SUPER_TURBO_NO_CPR
            },

            .supported_corners = (railway_corner[])
            {
                RAILWAY_RETENTION,
                RAILWAY_SVS_SOC,
				RAILWAY_SVS_HIGH,
                RAILWAY_NOMINAL,
				RAILWAY_NOMINAL_HIGH,
                RAILWAY_SUPER_TURBO,
            },
            .supported_corners_count = 6,
        },

        // VDDCX
         {
            .rail_type      = RAILWAY_RAIL_TYPE_LOGIC,
            .vreg_name      = "vddcx",

            .vreg_type      = RPM_SMPS_A_REQ,
            .vreg_num       = 3,

            .pmic_step_size = 12500,     // not used

            .initial_corner = RAILWAY_NOMINAL,

            .supports_explicit_voltage_requests = true,

            .default_uvs = (const unsigned[])
            {
                0,                      // RAILWAY_NO_REQUEST
                750000,                 // RAILWAY_RETENTION
                1050000,                // RAILWAY_SVS_KRAIT
                1050000,                // RAILWAY_SVS_SOC
				1150000,                // RAILWAY_SVS_HIGH
                1225000,                // RAILWAY_NOMINAL
                1287500,                // RAILWAY_NOMINAL_HIGH
                1350000,                // RAILWAY_TURBO
                1350000,                // RAILWAY_SUPER_TURBO
                1350000,                // RAILWAY_SUPER_TURBO_NO_CPR
            },

            .supported_corners = (railway_corner[])
            {
                RAILWAY_RETENTION,
                RAILWAY_SVS_SOC,
				RAILWAY_SVS_HIGH,
                RAILWAY_NOMINAL,
				RAILWAY_NOMINAL_HIGH,
                RAILWAY_SUPER_TURBO,
            },
            .supported_corners_count = 6,
        },
    },

    .num_rails = 2,
};
//
// END config data
//

const railway_config_data_t * const RAILWAY_CONFIG_DATA = &temp_config_data;
uint32 railway_target_type;

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

void railway_remap_target_specific_corners(rpm_translation_info *info, railway_voter_irep *req)
{
   
}
