/*
===========================================================================

FILE:         railway_config.c

DESCRIPTION:
  Per target railway configurations

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/railway_v2/src/9x45/railway_config.c#1 $
$Date: 2015/02/25 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2014 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "CoreVerify.h"
#include "kvp.h"
#include "rpmserver.h"
#include "railway_config.h"
#include "railway_adapter.h"

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
            .vreg_num       = 9,

            .pmic_step_size = 12500,    // not used

            .initial_corner = RAILWAY_NOMINAL,

            .default_uvs = (const unsigned[])
            {
                0,                      // RAILWAY_NO_REQUEST
                675000,                 // RAILWAY_RETENTION
                1000000,                // RAILWAY_SVS_KRAIT
                1000000,                // RAILWAY_SVS_SOC
                1000000,                // RAILWAY_SVS_HIGH
                1112500,                // RAILWAY_NOMINAL
                1112500,                // RAILWAY_TURBO
                1112500,                // RAILWAY_TURBO_HIGH
                1112500,                // RAILWAY_SUPER_TURBO
                1112500,                // RAILWAY_SUPER_TURBO_NO_CPR
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
            .vreg_num       = 5,

            .pmic_step_size = 12500,    // not used

            .initial_corner = RAILWAY_NOMINAL,

            .default_uvs = (const unsigned[])
            {
                0,                      // RAILWAY_NO_REQUEST
                500000,                 // RAILWAY_RETENTION
                800000,                 // RAILWAY_SVS_KRAIT
                900000,                 // RAILWAY_SVS_SOC
                900000,                 // RAILWAY_SVS_HIGH
                1012500,                // RAILWAY_NOMINAL
                1112500,                // RAILWAY_TURBO
                1112500,                // RAILWAY_TURBO_HIGH
                1112500,                // RAILWAY_SUPER_TURBO
                1112500,                // RAILWAY_SUPER_TURBO_NO_CPR
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
extern void railway_set_pmic_voltage(int rail_num, unsigned microvolts);
void railway_init_early_proxy_votes(void)
{
    uint32_t  req  = 1;
    kvp_t    *kvp  = kvp_create(0);
    uint32_t  i;

    // Force vote to pmic for initial voltages
    for (i = 0; i < RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        railway_rail_config_t *rail_cfg   = &(RAILWAY_CONFIG_DATA->rails[i]);
        unsigned               initial_uv = rail_cfg->default_uvs[rail_cfg->initial_corner];
        railway_rail_state_t  *rail_state = &(railway.rail_state[i]);

        railway_set_pmic_voltage(i, initial_uv);

        rail_state->current_active.mode       = rail_cfg->initial_corner;
        rail_state->current_active.microvolts = initial_uv;
    }

    // Init VDDCX at NOMINAL on behalf of APPS
    kvp_reset(kvp);
    req = (uint32_t)RAILWAY_NOMINAL;
    kvp_put(kvp, RAILWAY_INTERNAL_KEY_CORNER_LEVEL_KEY, sizeof(req), (void *)&req);
    rpm_send_init_proxy_vote(RPM_SMPS_A_REQ,  // Cx is S5
                             5,
                             0,               // APPS is 0
                             kvp);
    kvp_destroy(kvp);
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

