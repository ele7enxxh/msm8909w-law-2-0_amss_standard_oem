/*===========================================================================

  Copyright (c) 2014,2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_target_quotients.h"
#include "cpr_device_hw_version.h"

static const cpr_target_quotient_versioned_config_t cx_gfx__lpicx_quotients =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF),
        },
        .foundry_range_count = 1,
    },
    .target_quotient_level_count = 5,
    .target_quotient_level = (const cpr_target_quotient_level_t[])
    {
        //Mode,                         RO[0],  RO[1],  RO[2],  RO[3],  RO[4],  RO[5],  RO[6],  RO[7],  RO[8],  RO[9],  RO[10],  RO[11],  RO[12],  RO[13],  RO[14],  RO[15]
        {CPR_VOLTAGE_MODE_SUPER_TURBO,  0,      0,      0,      0,      1082,   970,    0,      1263,   0,      1337,   0,       0,       0,       0,       0,       0},
        {CPR_VOLTAGE_MODE_TURBO,        0,      0,      0,      0,      899,    805,    0,      1083,   0,      1149,   0,       0,       0,       0,       0,       0},
        {CPR_VOLTAGE_MODE_NOMINAL,      0,      0,      0,      0,      669,    600,    0,      851,    0,      905,    0,       0,       0,       0,       0,       0},
        {CPR_VOLTAGE_MODE_SVS,          0,      0,      0,      0,      399,    362,    0,      564,    0,      602,    0,       0,       0,       0,       0,       0},
        {CPR_VOLTAGE_MODE_SVS2,         0,      0,      0,      0,      249,    231,    0,      394,    0,      421,    0,       0,       0,       0,       0,       0},
      //{CPR_VOLTAGE_MODE_SVS_MIN,      0,      0,      0,      0,      132,    130,    0,      252,    0,      270,    0,       0,       0,       0,       0,       0},
    },
    .ro_kv_x_100 = {                    0,      0,      0,      0,      227,    200,    0,      241,    0,      254,    0,       0,       0,       0,       0,       0},
};

static const cpr_target_quotient_rail_config_t cx_quotient_config =
{
    .rail_id = CPR_RAIL_CX,
    .versioned_target_quotient_config = (const cpr_target_quotient_versioned_config_t*[])
    {
        &cx_gfx__lpicx_quotients,
    },
    .versioned_target_quotient_config_count = 1,
};


//MSS config.  These are not real values.  Just placeholders currently. 

static const cpr_target_quotient_versioned_config_t mss_quotients =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF),
            CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF),  //modem foundry version not fully implemented yet and always returns TSMC
        },
        .foundry_range_count = 2,
    },
    .target_quotient_level_count = 5,
    .target_quotient_level = (const cpr_target_quotient_level_t[])
    {
        //Mode,                         RO[0],  RO[1],  RO[2],  RO[3],  RO[4],  RO[5],  RO[6],  RO[7],  RO[8],  RO[9],  RO[10],  RO[11],  RO[12],  RO[13],  RO[14],  RO[15]
        {CPR_VOLTAGE_MODE_TURBO,        0,      0,      0,      0,      899,    805,    0,      1083,   0,      1149,   0,       0,       0,       0,       0,       0},
        {CPR_VOLTAGE_MODE_NOMINAL,      0,      0,      0,      0,      669,    600,    0,      851,    0,      905,    0,       0,       0,       0,       0,       0},
        {CPR_VOLTAGE_MODE_SVS,          0,      0,      0,      0,      399,    362,    0,      564,    0,      602,    0,       0,       0,       0,       0,       0},
        {CPR_VOLTAGE_MODE_SVS2,         0,      0,      0,      0,      249,    231,    0,      394,    0,      421,    0,       0,       0,       0,       0,       0},  
        {CPR_VOLTAGE_MODE_SVS_MIN,      0,      0,      0,      0,      132,    130,    0,      252,    0,      270,    0,       0,       0,       0,       0,       0},  
    },
    .ro_kv_x_100 = { 0, 0, 0, 0, 227, 200, 0, 241, 0, 254, 0, 0, 0, 0, 0, 0 },
};

static const cpr_target_quotient_rail_config_t mss_quotient_config =
{
    .rail_id = CPR_RAIL_MSS,
    .versioned_target_quotient_config = (const cpr_target_quotient_versioned_config_t*[])
    {
        &mss_quotients,
    },
    .versioned_target_quotient_config_count = 1,
};





static const cpr_target_quotient_rail_config_t gfx_quotient_config =
{
    .rail_id = CPR_RAIL_GFX,
    .versioned_target_quotient_config = (const cpr_target_quotient_versioned_config_t*[])
    {
        &cx_gfx__lpicx_quotients,
    },
    .versioned_target_quotient_config_count = 1,
};

static const cpr_target_quotient_versioned_config_t mx_ddr_lpimx_quotients =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF),
        },
        .foundry_range_count = 1,
    },
    .target_quotient_level_count = 5,
    .target_quotient_level = (const cpr_target_quotient_level_t[])
    {
        //Mode,                         RO[0],  RO[1],  RO[2],  RO[3],  RO[4],  RO[5],  RO[6],  RO[7],  RO[8],  RO[9],  RO[10],  RO[11],  RO[12],  RO[13],  RO[14],  RO[15]
        {CPR_VOLTAGE_MODE_SUPER_TURBO,  0,      0,      0,      0,      1082,   970,    0,      1263,   0,      1337,   0,       0,       0,       0,       0,       0},
        {CPR_VOLTAGE_MODE_TURBO,        0,      0,      0,      0,      899,    805,    0,      1083,   0,      1149,   0,       0,       0,       0,       0,       0},
        {CPR_VOLTAGE_MODE_NOMINAL,      0,      0,      0,      0,      777,    697,    0,      961,    0,      1021,   0,       0,       0,       0,       0,       0},
        {CPR_VOLTAGE_MODE_SVS,          0,      0,      0,      0,      777,    697,    0,      961,    0,      1021,   0,       0,       0,       0,       0,       0},
        {CPR_VOLTAGE_MODE_SVS2,         0,      0,      0,      0,      777,    697,    0,      961,    0,      1021,   0,       0,       0,       0,       0,       0},
      //{CPR_VOLTAGE_MODE_SVS_MIN,      0,      0,      0,      0,      777,    697,    0,      961,    0,      1021,   0,       0,       0,       0,       0,       0},
    },
    .ro_kv_x_100 = {                    0,      0,      0,      0,      242,    217,    0,      239,    0,      251,    0,       0,       0,       0,       0,       0},
};

static const cpr_target_quotient_rail_config_t mx_quotient_config =
{
    .rail_id = CPR_RAIL_MX,
    .versioned_target_quotient_config = (const cpr_target_quotient_versioned_config_t*[])
    {
        &mx_ddr_lpimx_quotients,
    },
    .versioned_target_quotient_config_count = 1,
};

const cpr_target_quotient_global_config_t cpr_bsp_target_quotient_config = 
{
    .rail_config = (const cpr_target_quotient_rail_config_t*[])
    {
        &mss_quotient_config,
    },
    .rail_config_count = 1,
};
