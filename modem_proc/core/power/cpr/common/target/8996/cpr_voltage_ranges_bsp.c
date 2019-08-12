/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/
#include "cpr_voltage_ranges.h"
#include "cpr_device_hw_version.h"

// Mx Voltage Ranges

static const cpr_config_versioned_voltage_ranges_t SS_8996_mx_voltage_ranges =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            {CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF)},
        },
        .foundry_range_count = 1,
    },
    .voltage_level = (const cpr_config_voltage_level_t[])
    {
        //Mode,                              Ceiling,  Floor, Fuse-Ref, Corr-factor,         Fuse-Type
        {CPR_VOLTAGE_MODE_SVS,                850000, 850000,   850000,           0,  CPR_FUSE_NO_FUSE},
        {CPR_VOLTAGE_MODE_NOMINAL,            950000, 850000,   950000,           0,  CPR_FUSE_NOMINAL},
        {CPR_VOLTAGE_MODE_SUPER_TURBO,       1015000, 855000,  1015000,           0,    CPR_FUSE_TURBO},
    },
    .voltage_level_count = 3,
};

static const cpr_config_rail_voltage_ranges_t mx_8996_voltage_ranges =
{
    .rail_id = CPR_RAIL_MX,
    .disable_fused_floor = true,
    .versioned_voltage_range = (const cpr_config_versioned_voltage_ranges_t*[])
    {
        &SS_8996_mx_voltage_ranges,
    },
    .versioned_voltage_range_count = 1,
};

// Cx Voltage Ranges

static const cpr_config_versioned_voltage_ranges_t SS_8996_cx_voltage_ranges =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            {CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF)},
            {CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF)},  //modem foundry detection not fully implemented, it currently returns TSMC
        },
        .foundry_range_count = 2,
    },
    .voltage_level = (const cpr_config_voltage_level_t[])
    {
        //Mode,                              Ceiling,  Floor, Fuse-Ref, Corr-factor,         Fuse-Type
        {CPR_VOLTAGE_MODE_SVS2,               670000, 545000,   670000,           0,     CPR_FUSE_SVS2},
        {CPR_VOLTAGE_MODE_SVS,                745000, 625000,   745000,           0,      CPR_FUSE_SVS},
        {CPR_VOLTAGE_MODE_NOMINAL,            905000, 755000,   905000,           0,  CPR_FUSE_NOMINAL},
        {CPR_VOLTAGE_MODE_SUPER_TURBO,       1015000, 855000,  1015000,           0,    CPR_FUSE_TURBO},
    },
    .voltage_level_count = 4,
};

static const cpr_config_rail_voltage_ranges_t cx_8996_voltage_ranges =
{
    .rail_id = CPR_RAIL_CX,
    .versioned_voltage_range = (const cpr_config_versioned_voltage_ranges_t*[])
    {
        &SS_8996_cx_voltage_ranges,
    },
    .versioned_voltage_range_count = 1,
};


// MSS Voltage Ranges

static const cpr_config_versioned_voltage_ranges_t vALL_8996_mss_voltage_ranges =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            { CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF) },
            { CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF) },  //modem foundry detection not fully implemented, it currently returns TSMC
        },
        .foundry_range_count = 2,
    },
    .voltage_level = (const cpr_config_voltage_level_t[])
    {
               //Mode,                  Ceiling,  Floor, Fuse-Ref, Corr-factor,         Fuse-Type
        { CPR_VOLTAGE_MODE_SVS_MIN,   600000, 525000,    600000,          0,    CPR_FUSE_NO_FUSE },
        { CPR_VOLTAGE_MODE_SVS2,      675000, 550000,    675000,          0,       CPR_FUSE_SVS2 },
        { CPR_VOLTAGE_MODE_SVS,       750000, 625000,    750000,          0,        CPR_FUSE_SVS },
        { CPR_VOLTAGE_MODE_NOMINAL,   900000, 750000,    900000,          0,    CPR_FUSE_NOMINAL },
        { CPR_VOLTAGE_MODE_TURBO,    1012500, 850000,   1012500,          0,      CPR_FUSE_TURBO },
    },
    .voltage_level_count = 5,
};

static const cpr_config_rail_voltage_ranges_t mss_8996_voltage_ranges =
{
    .rail_id = CPR_RAIL_MSS,
    .versioned_voltage_range = (const cpr_config_versioned_voltage_ranges_t*[])
    {
        &vALL_8996_mss_voltage_ranges,
    },
    .versioned_voltage_range_count = 1,
};




// VddaEbi Voltage Ranges

static const cpr_config_versioned_voltage_ranges_t SS_8996_vdda_ebi_voltage_ranges =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            {CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF)},
        },
        .foundry_range_count = 1,
    },
    .voltage_level = (const cpr_config_voltage_level_t[])
    {
        //Mode,                        Ceiling,  Floor, Fuse-Ref, Corr-factor,         Fuse-Type
        {CPR_VOLTAGE_MODE_SVS,          850000, 850000,   850000,           0,      CPR_FUSE_SVS},
        {CPR_VOLTAGE_MODE_NOMINAL,      950000, 850000,   950000,           0,  CPR_FUSE_NOMINAL},
        {CPR_VOLTAGE_MODE_SUPER_TURBO, 1012500, 850000,  1012500,           0,    CPR_FUSE_TURBO},
    },
    .voltage_level_count = 3,
};

static const cpr_config_rail_voltage_ranges_t vdda_ebi_8996_voltage_ranges =
{
    .rail_id = CPR_RAIL_VDDA_EBI,
    .versioned_voltage_range = (const cpr_config_versioned_voltage_ranges_t*[])
    {
        &SS_8996_vdda_ebi_voltage_ranges,
    },
    .versioned_voltage_range_count = 1,
};

// SSC_Mx Voltage Ranges

static const cpr_config_versioned_voltage_ranges_t SS_8996_ssc_mx_voltage_ranges =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            {CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF)},
        },
        .foundry_range_count = 1,
    },
    .voltage_level = (const cpr_config_voltage_level_t[])
    {
        //Mode,                        Ceiling,  Floor, Fuse-Ref, Corr-factor,         Fuse-Type
        {CPR_VOLTAGE_MODE_SVS,          850000, 850000,   850000,           0,      CPR_FUSE_SVS},
        {CPR_VOLTAGE_MODE_NOMINAL,      950000, 850000,   950000,           0,  CPR_FUSE_NOMINAL},
        {CPR_VOLTAGE_MODE_SUPER_TURBO, 1012500, 850000,  1012500,           0,  CPR_FUSE_NOMINAL},
    },
    .voltage_level_count = 3,
};

static const cpr_config_rail_voltage_ranges_t ssc_mx_8996_voltage_ranges =
{
    .rail_id = CPR_RAIL_SSC_MX,
    .versioned_voltage_range = (const cpr_config_versioned_voltage_ranges_t*[])
    {
        &SS_8996_ssc_mx_voltage_ranges,
    },
    .versioned_voltage_range_count = 1,
};

// SSC_Cx Voltage Ranges

static const cpr_config_versioned_voltage_ranges_t SS_8996_ssc_cx_voltage_ranges =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            {CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF)},
        },
        .foundry_range_count = 1,
    },
    .voltage_level = (const cpr_config_voltage_level_t[])
    {
        //Mode,                        Ceiling,  Floor, Fuse-Ref, Corr-factor,         Fuse-Type
        {CPR_VOLTAGE_MODE_SVS2,         675000, 550000,   675000,           0,     CPR_FUSE_SVS2},
        {CPR_VOLTAGE_MODE_SVS,          750000, 625000,   750000,           0,      CPR_FUSE_SVS},
        {CPR_VOLTAGE_MODE_NOMINAL,      900000, 750000,   900000,           0,  CPR_FUSE_NOMINAL},
        {CPR_VOLTAGE_MODE_SUPER_TURBO, 1012500, 850000,  1012500,           0,  CPR_FUSE_NOMINAL},
    },
    .voltage_level_count = 4,
};

static const cpr_config_rail_voltage_ranges_t ssc_cx_8996_voltage_ranges =
{
    .rail_id = CPR_RAIL_SSC_CX,
    .versioned_voltage_range = (const cpr_config_versioned_voltage_ranges_t*[])
    {
        &SS_8996_ssc_cx_voltage_ranges,
    },
    .versioned_voltage_range_count = 1,
};

const cpr_config_global_voltage_ranges_t cpr_bsp_voltage_ranges_config =
{
    .rail_voltage_ranges = (const cpr_config_rail_voltage_ranges_t*[])
    {
        &mss_8996_voltage_ranges,
        &cx_8996_voltage_ranges,
    },
    .rail_voltage_ranges_count = 2,
};

// Rail PMIC step-sizes

const cpr_config_misc_info_t cpr_bsp_misc_info_config =
{
    .rail_info = (const cpr_config_misc_rail_info_t[])
    {
        //Rail,             PMIC Step-Size
        {CPR_RAIL_MX,                 5000},
        {CPR_RAIL_CX,                 5000},
        {CPR_RAIL_MSS,               12500},
        {CPR_RAIL_VDDA_EBI,          12500},
        {CPR_RAIL_SSC_MX,            12500},
        {CPR_RAIL_SSC_CX,            12500},
    },
    .rail_info_count = 6,
};
