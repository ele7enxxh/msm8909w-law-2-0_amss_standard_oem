/*===========================================================================

  Copyright (c) 2015, 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/
#include "cpr_voltage_ranges.h"
#include "cpr_device_hw_version.h"
#include "cpr_qfprom.h"

//----------------------------------------------------------------------------
//  Cx Voltage Ranges
//----------------------------------------------------------------------------
static const cpr_config_versioned_voltage_ranges_t vALL_8937_cx_voltage_ranges =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            {CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0xFF,},
            {CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0xFF,},
            {CPR_FOUNDRY_SMIC, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0xFF,},
            {CPR_FOUNDRY_GF, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0xFF,},
        },
        .foundry_range_count = 4,
    },
    .voltage_level = (const cpr_config_voltage_level_t[])
    {
        //Mode,                              Ceiling,  Floor, Fuse-Ref, Vol-max_floor_to_ceiling, Corr-factor,         Fuse-Type
        { CPR_VOLTAGE_MODE_SVS,         950000, 825000,    950000,      	0,					    0,        CPR_FUSE_SVS ,CPR_FUSE_NO_FUSE},
        { CPR_VOLTAGE_MODE_SVS_L1,     1012500, 850000,   1012500,      	0,					    0,        CPR_FUSE_SVS, CPR_FUSE_NO_FUSE},
        { CPR_VOLTAGE_MODE_NOMINAL,    1050000, 887500,   1050000,      	0,					    0,    CPR_FUSE_NOMINAL,CPR_FUSE_NO_FUSE },
        { CPR_VOLTAGE_MODE_NOMINAL_L1, 1112500, 950000,   1112500,      	0,					    0,    CPR_FUSE_NOMINAL,CPR_FUSE_NO_FUSE },
        { CPR_VOLTAGE_MODE_TURBO,      1162500, 975000,   1162500,      	0,					    0,      CPR_FUSE_TURBO,CPR_FUSE_NO_FUSE }, 
    },
    .voltage_level_count = 5,
};

static const cpr_config_rail_voltage_ranges_t cx_8937_voltage_ranges =
{
    .rail_id = CPR_RAIL_CX,
    .versioned_voltage_range = (const cpr_config_versioned_voltage_ranges_t*[])
    {
        &vALL_8937_cx_voltage_ranges,
    },
    .versioned_voltage_range_count = 1,
};



//----------------------------------------------------------------------------
//   MSS Voltage Ranges
//----------------------------------------------------------------------------
static const cpr_config_versioned_voltage_ranges_t vALL_8937_mss_voltage_ranges =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            { CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0xFF, },
            { CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0xFF, },
            { CPR_FOUNDRY_SMIC, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0xFF,}, 
            { CPR_FOUNDRY_GF, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0xFF,}, 
        },
        .foundry_range_count = 4,
    },
    .voltage_level = (const cpr_config_voltage_level_t[])
    {
               //Mode,                  Ceiling,  Floor, Fuse-Ref,  Vol-max_floor_to_ceiling, Corr-factor,         Fuse-Type
        { CPR_VOLTAGE_MODE_SVS,         1050000,  900000,  1050000, 	0,					        0,        CPR_FUSE_SVS, CPR_FUSE_NO_FUSE},
        { CPR_VOLTAGE_MODE_SVS_L1,      1162500,  975000, 1162500,  	0,					        0,        CPR_FUSE_SVS,CPR_FUSE_NO_FUSE },
        { CPR_VOLTAGE_MODE_NOMINAL,     1225000,  1037500, 1225000, 	0,					        0,    CPR_FUSE_NOMINAL,CPR_FUSE_NOMINAL_OFFSET },
        { CPR_VOLTAGE_MODE_NOMINAL_L1,  1287500,  1075000, 1287500, 	0,					        0,    CPR_FUSE_NOMINAL ,CPR_FUSE_TURBO_OFFSET},
        { CPR_VOLTAGE_MODE_TURBO,       1350000,  1125000, 1350000, 	0,					        0,      CPR_FUSE_TURBO,CPR_FUSE_TURBO_OFFSET },
    },
    .voltage_level_count = 5,
};

cpr_config_rail_voltage_ranges_t mss_8937_voltage_ranges =
{
    .rail_id = CPR_RAIL_MSS,
    .versioned_voltage_range = (const cpr_config_versioned_voltage_ranges_t*[])
    {
        &vALL_8937_mss_voltage_ranges,
    },
    .versioned_voltage_range_count = 1,
};



const cpr_config_global_voltage_ranges_t cpr_bsp_voltage_ranges_config =
{
    .rail_voltage_ranges = (const cpr_config_rail_voltage_ranges_t*[])
    {
        &mss_8937_voltage_ranges,
        &cx_8937_voltage_ranges,
    },
    .rail_voltage_ranges_count = 2,
};



//----------------------------------------------------------------------------
// Rail PMIC step-sizes
//----------------------------------------------------------------------------
const cpr_config_misc_info_t cpr_bsp_misc_info_config =
{
    .cpr_rev_fuse = {HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR, 4, (1<<6) | (1<<5) | (1<<4)}, //0xA422C 6:4 CPR_GLOBAL_RC 
    .rail_info = (const cpr_config_misc_rail_info_t[])
    {
        //Rail,             PMIC Step-Size
        {CPR_RAIL_CX,                12500},
        {CPR_RAIL_MSS,               12500},
    },
    .rail_info_count = 2,
};

