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
// PTE high voltage part fuse reading
////////////////////////////////////////////////
#define BIT_31_MASK 0x80000000
#define BIT_1_MASK  0x00000001

uint32 get_high_voltage_fuse_value()
{
#ifdef WINSIM
    return 0;
#else
    uint32  hv_fuse_value;
    
    // grab the PTE registers where the high voltage efuse is kept.
    uint32 low_reg_31bit =  HWIO_INM(QFPROM_CORR_PTE_ROW3_LSB, BIT_31_MASK);
    uint32 high_reg_0bit =  HWIO_INM(QFPROM_CORR_PTE_ROW3_MSB, BIT_1_MASK);
    
    // get the 31st bit of the low register, put it in the 1st bit position.
    low_reg_31bit = low_reg_31bit>>31;
    
    // move the high bit to the 2nd bit position.
    high_reg_0bit = high_reg_0bit<<1;
    
    //calculate the fuse value by bitwise or of MSB bit 1 and LSB bit 31
    hv_fuse_value = high_reg_0bit|low_reg_31bit;
    
    // return the high voltage fuse value
    return hv_fuse_value;
#endif
}

/* The switch statement relates to the modes by the fuses above, here is the spec:
 * 0b00: Fully functional across voltage/freq
 * 0b10: HV-Bin 1 /HV-bin 2 device which will operate at turbo voltage for all freq modes - nom, svs and turbo
 * CPR disabled (voltage wall scenario)  - Not for software teams
 * 0b11: HV-Bin 1 /HV-Bin 2 device which will operate at each freq mode's highest SS voltage - CPR disabled
 */

void cpr_enable_init_fn_high_voltage_parts(cpr_rail_id_t rail_id, const struct cpr_enablement_versioned_rail_config_t* rail_enablement_config, cpr_enablement_stage_t enablement_stage)
{
    const uint32 hv_fuse_value = get_high_voltage_fuse_value();

    switch(hv_fuse_value)
    {
        case(0):
            CPR_ENABLE_OPEN_LOOP.enablement_fn(rail_id, rail_enablement_config, enablement_stage);
            break;
        case(3):
            CPR_ENABLE_GLOBAL_CEILING_VOLTAGE.enablement_fn(rail_id, rail_enablement_config, enablement_stage);
            break;
        default:
            CORE_VERIFY(0);// no other values are supported at this time.
    }
}

const cpr_enablement_init_params_t CPR_ENABLE_HIGH_VOLTAGE_PARTS =
{
    .enablement_type        = CPR_ENABLEMENT_HV_PARTS_ID,
    .enablement_fn          = cpr_enable_init_fn_high_voltage_parts,
};


////////////////////////////////////////////////
// Mx config
////////////////////////////////////////////////

static const cpr_enablement_versioned_rail_config_t mx_8996_versioned_cpr_enablement =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            {CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF)},
        },
        .foundry_range_count = 1,
    },
    .enablement_init_params = &CPR_ENABLE_READ_CONFIG_FROM_SMEM,
};

static const cpr_enablement_rail_config_t mx_8996_cpr_enablement =
{
    .rail_id = CPR_RAIL_MX,
    .versioned_rail_config = (const cpr_enablement_versioned_rail_config_t*[])
    {
        &mx_8996_versioned_cpr_enablement,
    },
    .versioned_rail_config_count = 1,
};

////////////////////////////////////////////////
// Cx config
////////////////////////////////////////////////

static const cpr_enablement_versioned_rail_config_t cx_8996_versioned_cpr_enablement =
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

static const cpr_enablement_rail_config_t cx_8996_cpr_enablement =
{
    .rail_id = CPR_RAIL_CX,
    .versioned_rail_config = (const cpr_enablement_versioned_rail_config_t*[])
    {
        &cx_8996_versioned_cpr_enablement,
    },
    .versioned_rail_config_count = 1,
};

////////////////////////////////////////////////
// MSS config
////////////////////////////////////////////////

static const cpr_enablement_versioned_rail_config_t mss_8996_versioned_cpr_enablement =
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
    .enablement_init_params = &CPR_ENABLE_CLOSED_LOOP,
    //.enablement_init_params = &CPR_ENABLE_OPEN_LOOP,
    //.enablement_init_params = &CPR_ENABLE_GLOBAL_CEILING_VOLTAGE,
    .supported_level = (const cpr_enablement_supported_level_t[])
    {                              /* Large intial values for testing*/
        //Mode                Static-Margin (mV) Custom static margin function    aging_scaling_factor 
        { CPR_VOLTAGE_MODE_SVS_MIN,          0,                  NULL,                  1},
        { CPR_VOLTAGE_MODE_SVS2,             0,                  NULL,                  1},  
        { CPR_VOLTAGE_MODE_SVS,              0,                  NULL,                  1},
        { CPR_VOLTAGE_MODE_NOMINAL,          0,                  NULL,                  1},
        { CPR_VOLTAGE_MODE_TURBO,            0,                  NULL,                  1},
    },
    .supported_level_count = 5,
    .custom_voltage_fn = NULL,  // NULL ==  Use ordinary floor and ceiling calculation functions.
    .mode_to_settle_count = 0,  // 0 == No init time CPR measurements
    .aging_static_margin_limit = 13, //mV
};

static const cpr_enablement_rail_config_t mss_8996_cpr_enablement =
{
    .rail_id = CPR_RAIL_MSS,
    .versioned_rail_config = (const cpr_enablement_versioned_rail_config_t*[])
    {
        &mss_8996_versioned_cpr_enablement,
    },
    .versioned_rail_config_count = 1,
};


////////////////////////////////////////////////
// Vdda_Ebi config
////////////////////////////////////////////////

static const cpr_enablement_versioned_rail_config_t vdda_ebi_8996_versioned_cpr_enablement =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            {CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF,0xFF)},
        },
        .foundry_range_count = 1,
    },
    .enablement_init_params = &CPR_ENABLE_READ_CONFIG_FROM_SMEM,
};

static const cpr_enablement_rail_config_t vdda_ebi_8996_cpr_enablement =
{
    .rail_id = CPR_RAIL_VDDA_EBI,
    .versioned_rail_config = (const cpr_enablement_versioned_rail_config_t*[])
    {
        &vdda_ebi_8996_versioned_cpr_enablement,
    },
    .versioned_rail_config_count = 1,
};

////////////////////////////////////////////////
// SSC_Mx config
////////////////////////////////////////////////

static const cpr_enablement_versioned_rail_config_t ssc_mx_8996_versioned_cpr_enablement =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            {CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF)},
        },
        .foundry_range_count = 1,
    },
    .enablement_init_params = &CPR_ENABLE_HIGH_VOLTAGE_PARTS,
    .supported_level = (const cpr_enablement_supported_level_t[])
    {
        //Mode                Static-Margin (mV) Custom static margin function    aging_scaling_factor
        {CPR_VOLTAGE_MODE_SVS,         0,                  NULL,                  1},
        {CPR_VOLTAGE_MODE_NOMINAL,     0,                  NULL,                  1},
        {CPR_VOLTAGE_MODE_SUPER_TURBO, 0,                  NULL,                  1},
    },
    .supported_level_count = 3,
    .custom_voltage_fn = NULL,  // NULL ==  Use ordinary floor and ceiling calculation functions.
    .mode_to_settle_count = 0,  // 0 == No init time CPR measurements
};

static const cpr_enablement_rail_config_t ssc_mx_8996_cpr_enablement =
{
    .rail_id = CPR_RAIL_SSC_MX,
    .versioned_rail_config = (const cpr_enablement_versioned_rail_config_t*[])
    {
        &ssc_mx_8996_versioned_cpr_enablement,
    },
    .versioned_rail_config_count = 1,
};

////////////////////////////////////////////////
// SSC_Cx config
////////////////////////////////////////////////

static const cpr_enablement_versioned_rail_config_t ssc_cx_8996_versioned_cpr_enablement =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            {CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0,0), CPR_CHIPINFO_VERSION(0xFF, 0xFF)},
        },
        .foundry_range_count = 1,
    },
    .enablement_init_params = &CPR_ENABLE_HIGH_VOLTAGE_PARTS,
    .supported_level = (const cpr_enablement_supported_level_t[])
    {
        //Mode                Static-Margin (mV) Custom static margin function    aging_scaling_factor
        {CPR_VOLTAGE_MODE_SVS2,        0,                  NULL,                  1},
        {CPR_VOLTAGE_MODE_SVS,         0,                  NULL,                  1},
        {CPR_VOLTAGE_MODE_NOMINAL,     0,                  NULL,                  1},
        {CPR_VOLTAGE_MODE_SUPER_TURBO, 0,                  NULL,                  1},
    },
    .supported_level_count = 4,
    .custom_voltage_fn = NULL,  // NULL ==  Use ordinary floor and ceiling calculation functions.
    .mode_to_settle_count = 0,  // 0 == No init time CPR measurements
};

static const cpr_enablement_rail_config_t ssc_cx_8996_cpr_enablement =
{
    .rail_id = CPR_RAIL_SSC_CX,
    .versioned_rail_config = (const cpr_enablement_versioned_rail_config_t*[])
    {
        &ssc_cx_8996_versioned_cpr_enablement,
    },
    .versioned_rail_config_count = 1,
};

const cpr_enablement_config_t cpr_bsp_enablement_config =
{
    .rail_enablement_config = (const cpr_enablement_rail_config_t*[])
    {
        &mss_8996_cpr_enablement,
        &cx_8996_cpr_enablement,
    },
    .rail_enablement_config_count = 2,
};

