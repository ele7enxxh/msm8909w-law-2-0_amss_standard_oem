/*===========================================================================

  Copyright (c) 2014, 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/
#include "cpr_voltage_ranges.h"
#include "cpr_device_hw_version.h"

// --------------------------------------------------------------------------------------------------
// Mss Voltage Ranges
// --------------------------------------------------------------------------------------------------
static const cpr_config_versioned_voltage_ranges_t vALL_TSMC_9x45_mss_voltage_ranges =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xff,0xff),
        },
        .foundry_range_count = 1,
    },
    .voltage_level = (const cpr_config_voltage_level_t[])
    {
      //Mode,                              Ceiling,  Floor, Fuse-Ref, Corr-factor,         Fuse-Type
        {CPR_VOLTAGE_MODE_SVS2,         800000, 700000,   800000,       0,            CPR_FUSE_SVS2},
        {CPR_VOLTAGE_MODE_SVS,          900000, 725000,   900000,       0,            CPR_FUSE_SVS},
        {CPR_VOLTAGE_MODE_NOMINAL,     1000000, 837500,  1000000,    -50000,          CPR_FUSE_NOMINAL},
        {CPR_VOLTAGE_MODE_TURBO,       1112500, 937500,  1112500,    -80000,          CPR_FUSE_TURBO},
    },
    .voltage_level_count = 4,
};


static const cpr_config_rail_voltage_ranges_t mss_9x45_voltage_ranges =
{
    .rail_id = CPR_RAIL_MSS,
    .versioned_voltage_range = (const cpr_config_versioned_voltage_ranges_t*[])
    {
        &vALL_TSMC_9x45_mss_voltage_ranges,
    },
    .disable_fused_floor = false,
    .versioned_voltage_range_count = 1,
};


// --------------------------------------------------------------------------------------------------
// CX Voltage Ranges
//
// This is only for the SVS and NOM fuse reads for the eLDO calculations
// CX CPR is done in the RPM
//-----------------------------------------------------------------------------
static const cpr_config_versioned_voltage_ranges_t vALL_TSMC_9x45_cx_voltage_ranges =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xff,0xff),
        },
        .foundry_range_count = 1,
    },
    .voltage_level = (const cpr_config_voltage_level_t[])
    {
      //Mode,                              Ceiling,  Floor, Fuse-Ref, Corr-factor,         Fuse-Type
        {CPR_VOLTAGE_MODE_SVS2,         800000, 700000,   800000,       0,            CPR_FUSE_SVS2},
        {CPR_VOLTAGE_MODE_SVS,          900000, 725000,   900000,       0,            CPR_FUSE_SVS},
        {CPR_VOLTAGE_MODE_NOMINAL,     1000000, 875000,  1000000,       0,            CPR_FUSE_NOMINAL},
    },
    .voltage_level_count = 3,
};


static const cpr_config_rail_voltage_ranges_t cx_9x45_voltage_ranges =
{
    .rail_id = CPR_RAIL_CX,
    .versioned_voltage_range = (const cpr_config_versioned_voltage_ranges_t*[])
    {
        &vALL_TSMC_9x45_cx_voltage_ranges,
    },
    .disable_fused_floor = false,
    .versioned_voltage_range_count = 1,
};




const cpr_config_global_voltage_ranges_t cpr_bsp_voltage_ranges_config =
{
    .rail_voltage_ranges = (const cpr_config_rail_voltage_ranges_t*[])
    {
        &mss_9x45_voltage_ranges,
        &cx_9x45_voltage_ranges,
    },
    .rail_voltage_ranges_count = 2,
};


const cpr_config_misc_info_t cpr_bsp_misc_info_config =
{
    .rail_info = (const cpr_config_misc_rail_info_t[])
    {
        //Rail,             PMIC Step-Size
        {CPR_RAIL_MSS,               12500},      
        {CPR_RAIL_CX,                12500},      //we only use the fuse values currently, this is a placeholder. 
    },
    .rail_info_count = 2,
};
