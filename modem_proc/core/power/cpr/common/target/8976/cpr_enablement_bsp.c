/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_enablement.h"
#include "cpr_device_hw_version.h"
#include "cpr_voltage_ranges.h"
#include "cpr_qfprom.h"
#include "HALhwio.h"
#include "CoreVerify.h"



////////////////////////////////////////////////
// Cx config
////////////////////////////////////////////////

static const cpr_enablement_versioned_rail_config_t cx_8976_versioned_cpr_enablement =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            {CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF)},
            {CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF) },
        },
        .foundry_range_count = 2,
    },
    .enablement_init_params = &CPR_ENABLE_OPEN_LOOP,  //read the fuses, but we won't actually set any voltages.
};

static const cpr_enablement_rail_config_t cx_8976_cpr_enablement =
{
    .rail_id = CPR_RAIL_CX,
    .versioned_rail_config = (const cpr_enablement_versioned_rail_config_t*[])
    {
        &cx_8976_versioned_cpr_enablement,
    },
    .versioned_rail_config_count = 1,
};



////////////////////////////////////////////////
// MSS config
////////////////////////////////////////////////

static const cpr_enablement_versioned_rail_config_t mss_8976_versioned_cpr_enablement =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            { CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF) },
            { CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF) },
        },
        .foundry_range_count = 2,
    },
    //.enablement_init_params = &CPR_ENABLE_CLOSED_LOOP,
    .enablement_init_params = &CPR_ENABLE_OPEN_LOOP,
    //.enablement_init_params = &CPR_ENABLE_GLOBAL_CEILING_VOLTAGE,
    .supported_level = (const cpr_enablement_supported_level_t[])
    {                              /* Large intial values for testing*/
        //Mode                Static-Margin (mV) Custom static margin function    aging_scaling_factor 
        { CPR_VOLTAGE_MODE_SVS2,             0,                  NULL,                  1},  
        { CPR_VOLTAGE_MODE_SVS,              0,                  NULL,                  1},
        { CPR_VOLTAGE_MODE_SVS_L1,           0,                  NULL,                  1},
        { CPR_VOLTAGE_MODE_NOMINAL,          0,                  NULL,                  1},
        { CPR_VOLTAGE_MODE_NOMINAL_L1,       0,                  NULL,                  1},
        { CPR_VOLTAGE_MODE_TURBO,            0,                  NULL,                  1},
    },
    .supported_level_count = 6,
    .custom_voltage_fn = NULL,  // NULL ==  Use ordinary floor and ceiling calculation functions.
    .mode_to_settle_count = 0,  // 0 == No init time CPR measurements
    .aging_static_margin_limit = 13, //mV
};

static const cpr_enablement_rail_config_t mss_8976_cpr_enablement =
{
    .rail_id = CPR_RAIL_MSS,
    .versioned_rail_config = (const cpr_enablement_versioned_rail_config_t*[])
    {
        &mss_8976_versioned_cpr_enablement,
    },
    .versioned_rail_config_count = 1,
};



const cpr_enablement_config_t cpr_bsp_enablement_config =
{
    .rail_enablement_config = (const cpr_enablement_rail_config_t*[])
    {
        &mss_8976_cpr_enablement,
        &cx_8976_cpr_enablement,
    },
    .rail_enablement_config_count = 2,
};

