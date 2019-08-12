/*===========================================================================

  Copyright (c) 2015, 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_target_quotients.h"
#include "cpr_device_hw_version.h"

static const cpr_target_quotient_versioned_config_t mss_8937_TSMC_target_quotients =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            {CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0xFF,}
        },
        .foundry_range_count = 1,
    },
    .target_quotient_level = (const cpr_target_quotient_level_t[])
    {
        //Mode,                        RO[0], RO[1], RO[2], RO[3], RO[4], RO[5], RO[6], RO[7]
        {CPR_VOLTAGE_MODE_SVS,           275,   272,   436,   424,   235,   243,   365,   371},
        {CPR_VOLTAGE_MODE_SVS_L1,        389,   383,   579,   564,   337,   347,   492,   497},
        {CPR_VOLTAGE_MODE_NOMINAL,       474,   466,   673,   644,   414,   423,   575,   565},
        {CPR_VOLTAGE_MODE_NOMINAL_L1,    557,   546,   760,   711,   487,   494,   653,   625},
        {CPR_VOLTAGE_MODE_TURBO,         646,   632,   846,   781,   569,   558,   728,   687},
    },
    .target_quotient_level_count = 5,
    .ro_kv_x_100 = {157, 150, 175, 157, 137, 132, 157, 137},
};
static const cpr_target_quotient_versioned_config_t mss_8937_GF_target_quotients = 
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            CPR_FOUNDRY_GF, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0xFF, DALCHIPINFO_FAMILY_MSM8937
        },
        .foundry_range_count = 1,
    },
    .target_quotient_level = (const cpr_target_quotient_level_t[])
    {
        //Mode,                        RO[0], RO[1], RO[2], RO[3], RO[4], RO[5], RO[6], RO[7]
        {CPR_VOLTAGE_MODE_SVS,           271,   302,   493,   484,   282,   251,   439,   437},
        {CPR_VOLTAGE_MODE_SVS_L1,        382,   413,   634,   616,   383,   352,   565,   555},
        {CPR_VOLTAGE_MODE_NOMINAL,       470,   498,   739,   714,   460,   434,   660,   642},
        {CPR_VOLTAGE_MODE_NOMINAL_L1,    531,   556,   808,   779,   512,   489,   722,   699},
        {CPR_VOLTAGE_MODE_TURBO,         607,   627,   893,   856,   575,   559,   798,   769},
    },
    .target_quotient_level_count = 5,
    .ro_kv_x_100 = {149, 145, 180, 169, 131, 137, 162, 151},
};

static const cpr_target_quotient_versioned_config_t mss_8917_SMIC_target_quotients =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            CPR_FOUNDRY_SMIC, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0xFF, DALCHIPINFO_FAMILY_MSM8917
        },
        .foundry_range_count = 1,
    },
    .target_quotient_level = (const cpr_target_quotient_level_t[])
    {
        //Mode,                        RO[0], RO[1], RO[2], RO[3], RO[4], RO[5], RO[6], RO[7]
        {CPR_VOLTAGE_MODE_SVS,           309,   297,   501,   433,   262,   278,   434,   375},
        {CPR_VOLTAGE_MODE_SVS_L1,        422,   401,   635,   553,   356,   378,   550,   482},
        {CPR_VOLTAGE_MODE_NOMINAL,       506,   477,   731,   639,   423,   451,   632,   557},
        {CPR_VOLTAGE_MODE_NOMINAL_L1,    565,   530,   796,   697,   470,   503,   689,   608},
        {CPR_VOLTAGE_MODE_TURBO,         639,   595,   875,   768,   528,   567,   757,   671},
    },
    .target_quotient_level_count = 5,
    .ro_kv_x_100 = {153, 141, 174, 158, 126, 134, 151, 140},
};

static const cpr_target_quotient_rail_config_t mss_8937_target_quotient_config =
{
    .rail_id = CPR_RAIL_MSS,
    .versioned_target_quotient_config = (const cpr_target_quotient_versioned_config_t*[])
    {
        &mss_8937_TSMC_target_quotients,
        &mss_8917_SMIC_target_quotients,
        &mss_8937_GF_target_quotients,
    },
    .versioned_target_quotient_config_count = 3,
};

const cpr_target_quotient_global_config_t cpr_bsp_target_quotient_config = 
{
    .rail_config = (const cpr_target_quotient_rail_config_t*[])
    {
        &mss_8937_target_quotient_config,
    },
    .rail_config_count = 1,
};
