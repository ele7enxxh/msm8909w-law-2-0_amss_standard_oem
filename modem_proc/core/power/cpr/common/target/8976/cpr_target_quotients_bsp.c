/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_target_quotients.h"
#include "cpr_device_hw_version.h"

static const cpr_target_quotient_versioned_config_t mss_8976_vALL_target_quotients =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF),
        },
        .foundry_range_count = 1,
    },
    .target_quotient_level = (const cpr_target_quotient_level_t[])
    {
        //Mode,                        RO[0], RO[1], RO[2], RO[3], RO[4], RO[5], RO[6], RO[7]
        {CPR_VOLTAGE_MODE_SVS2,          643,   618,   760,   713,   318,   361,   206,   246},
        {CPR_VOLTAGE_MODE_SVS,           807,   778,   930,   881,   432,   479,   299,   348},
        {CPR_VOLTAGE_MODE_SVS_L1,        970,   933,  1098,  1042,   550,   596,   401,   452},
        {CPR_VOLTAGE_MODE_NOMINAL,      1078,  1033,  1206,  1144,   629,   674,   473,   524},
        {CPR_VOLTAGE_MODE_NOMINAL_L1,   1198,  1143,  1327,  1254,   721,   760,   556,   605},
        {CPR_VOLTAGE_MODE_TURBO,        1329,  1260,  1457,  1373,   823,   855,   653,   697},
    },
    .target_quotient_level_count = 6,
    .ro_kv_x_100 = {250, 220, 250, 220, 160, 180, 120, 140},
};

static const cpr_target_quotient_rail_config_t mss_8976_target_quotient_config =
{
    .rail_id = CPR_RAIL_MSS,
    .versioned_target_quotient_config = (const cpr_target_quotient_versioned_config_t*[])
    {
        &mss_8976_vALL_target_quotients,
    },
    .versioned_target_quotient_config_count = 1,
};


const cpr_target_quotient_global_config_t cpr_bsp_target_quotient_config = 
{
    .rail_config = (const cpr_target_quotient_rail_config_t*[])
    {
        &mss_8976_target_quotient_config,
    },
    .rail_config_count = 1,
};
