/*===========================================================================

  Copyright (c) 2014, 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_target_quotients.h"
#include "cpr_device_hw_version.h"

static const cpr_target_quotient_versioned_config_t mss_9x45_vALL_target_quotients =
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
        {CPR_VOLTAGE_MODE_SVS2,          103,    86,   105,   101,   354,   303,   384,   353},
        {CPR_VOLTAGE_MODE_SVS,           242,   222,   253,   247,   556,   501,   600,   566},
        {CPR_VOLTAGE_MODE_NOMINAL,       430,   407,   453,   446,   810,   752,   873,   836},
        {CPR_VOLTAGE_MODE_TURBO,         623,   599,   660,   650,  1039,   979,  1117,  1078},
    },
    .target_quotient_level_count = 4,
    .ro_kv_x_100 = {217, 216, 233, 231, 263, 261, 281, 279},
};

static const cpr_target_quotient_rail_config_t mss_9x45_target_quotient_config =
{
    .rail_id = CPR_RAIL_MSS,
    .versioned_target_quotient_config = (const cpr_target_quotient_versioned_config_t*[])
    {
        &mss_9x45_vALL_target_quotients,
    },
    .versioned_target_quotient_config_count = 1,
};


const cpr_target_quotient_global_config_t cpr_bsp_target_quotient_config = 
{
    .rail_config = (const cpr_target_quotient_rail_config_t*[])
    {
        &mss_9x45_target_quotient_config,
    },
    .rail_config_count = 1,
};
