/*
===========================================================================

FILE:         rbcpr_bsp.c

DESCRIPTION:
  Per target rbcpr bsp configurations

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/rbcpr/src/target/9x45/rbcpr_bsp.c#1 $
$Date: 2015/02/25 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2014 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "Chipinfo.h"
#include "railway.h"
#include "rbcpr.h"
#include "rbcpr_fuses.h"
#include "CoreVerify.h"


#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

typedef struct
{
    char*           rail_name;
    uint32          open_loop_reference_voltage;
    uint32          pmic_step_size_uv;
    uint32          cpr_ceiling;
    rbcpr_fuse_type fuse;
} rbcpr_rail_reference_voltage_info_t;

const rbcpr_rail_reference_voltage_info_t rbcpr_9x45_turbo_ref_voltages[] =
{
    {
        .rail_name                      = "vddcx",
        .open_loop_reference_voltage    = 1112500,
        .pmic_step_size_uv              = 12500,
        .cpr_ceiling                    = 1112500,
        .fuse                           = RBCPR_FUSE_TURBO,
    },
    {
        .rail_name                      = "vddmx",
        .open_loop_reference_voltage    = 1112500,
        .pmic_step_size_uv              = 12500,
        .cpr_ceiling                    = 1112500,
        .fuse                           = RBCPR_FUSE_TURBO,
    },
    {
        .rail_name                      = "vddmss",
        .open_loop_reference_voltage    = 1112500,
        .pmic_step_size_uv              = 12500,
        .cpr_ceiling                    = 1112500,
        .fuse                           = RBCPR_FUSE_TURBO,
    },
};


#define EFUSE_VOLTAGE_MULTIPLIER 10000  //All rail fuses use a 10mV multiplier

uint32 rbcpr_turbo_ceiling(const rbcpr_rail_reference_voltage_info_t* rail_info)
{
    const int32 voltage_offset_steps = rbcpr_fuses_get_fuse_value(rail_info->rail_name, rail_info->fuse);

    const int32 voltage_offset = voltage_offset_steps * EFUSE_VOLTAGE_MULTIPLIER;
    
    uint32 open_loop_ceiling = voltage_offset + rail_info->open_loop_reference_voltage;
     
    //Now round up to the PMIC step size.
    if(open_loop_ceiling%rail_info->pmic_step_size_uv)
    {
        open_loop_ceiling += (rail_info->pmic_step_size_uv - (open_loop_ceiling%rail_info->pmic_step_size_uv));
    }
    
    CORE_VERIFY((rail_info->cpr_ceiling % rail_info->pmic_step_size_uv) == 0);      //This should always be true if the config is correct.
    //Now clip to the cpr_ceiling
    open_loop_ceiling = MIN(rail_info->cpr_ceiling, open_loop_ceiling);
    
    return open_loop_ceiling;
}


void rbcpr_pin_mx_voltage_to_global_turbo_ceiling(rbcpr_corner_params_type* mx_turbo_corner_params, struct rbcpr_bsp_rail_info* mx_rail)
{
    uint32 global_open_loop_turbo_ceiling = 0;
    for(int i=0; i<ARRAY_SIZE(rbcpr_9x45_turbo_ref_voltages); i++)
    {
        uint32 rail_turbo_ceiling = rbcpr_turbo_ceiling(&rbcpr_9x45_turbo_ref_voltages[i]);
        global_open_loop_turbo_ceiling = MAX(global_open_loop_turbo_ceiling, rail_turbo_ceiling);
    }

    //Now round global_open_loop_turbo_ceiling up to the PMIC step size.
    if(global_open_loop_turbo_ceiling%mx_rail->rail_const_info->pmic_step_size_uv)
    {
        global_open_loop_turbo_ceiling += (mx_rail->rail_const_info->pmic_step_size_uv - (global_open_loop_turbo_ceiling%mx_rail->rail_const_info->pmic_step_size_uv));
    }
    
    CORE_VERIFY(global_open_loop_turbo_ceiling<=mx_turbo_corner_params->voltage_ceil);
    
    railway_set_corner_voltage(mx_rail->railway_rail_id, mx_turbo_corner_params->corner, global_open_loop_turbo_ceiling);

    mx_turbo_corner_params->voltage_floor = global_open_loop_turbo_ceiling;
    mx_turbo_corner_params->voltage_ceil = global_open_loop_turbo_ceiling;
}


const rbcpr_bsp_rail_params_type rbcpr_settings[] =
{
    // VDDMX
    {
        .target_params = (rbcpr_corner_params_and_fuse_type[])
        {
            {
                .corner_params =
                {
                    .corner         = RAILWAY_SVS_SOC,
					.fuse			= RBCPR_FUSE_NO_FUSE,
                    .voltage_ceil   = 1000000,
                    .voltage_floor  = 1000000,
                },
            },
            {
                .corner_params =
                {   // 0mV margin
					.rosc_target[0] = 623,
					.rosc_target[1] = 599,
					.rosc_target[2] = 660,
					.rosc_target[3] = 650,
					.rosc_target[4] = 1039,
					.rosc_target[5] = 979,
					.rosc_target[6] = 1117,
					.rosc_target[7] = 1078,
                    .corner         = RAILWAY_NOMINAL,
                    .fuse           = RBCPR_FUSE_NOMINAL,
                    .voltage_ceil   = 1112500,
                    .voltage_floor  = 1000000,
                },
            },
            {
                .corner_params =
                {   // pinned to global turbo ceiling (max OL of mx, cx, and mss)
                    .corner         = RAILWAY_SUPER_TURBO,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1112500,
                    .voltage_floor  = 1000000,
                },
            .fuse_function = rbcpr_pin_mx_voltage_to_global_turbo_ceiling,
            },
        },
        .rbcpr_enablement           = RBCPR_ENABLED_CLOSED_LOOP,
        .number_of_target_params    = 3,
    },

    // VDDCX
    {
        .target_params = (rbcpr_corner_params_and_fuse_type[])
        {
            {
                .corner_params =
                {   //0mV margin
                    .rosc_target[0] = 242,
                    .rosc_target[1] = 222,
                    .rosc_target[2] = 253,
                    .rosc_target[3] = 247,
                    .rosc_target[4] = 556,
                    .rosc_target[5] = 501,
                    .rosc_target[6] = 600,
                    .rosc_target[7] = 566,
                    .corner         = RAILWAY_SVS_SOC,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 900000,
                    .voltage_floor  = 725000,
                },
            },
            {
                .corner_params =
                {   //37.5mV margin
                    .rosc_target[0] =  512,
                    .rosc_target[1] =  488,
                    .rosc_target[2] =  541,
                    .rosc_target[3] =  532,
                    .rosc_target[4] =  909,
                    .rosc_target[5] =  850,
                    .rosc_target[6] =  978,
                    .rosc_target[7] =  940,
                    .corner         = RAILWAY_NOMINAL,
                    .fuse           = RBCPR_FUSE_NOMINAL,
                    .voltage_ceil   = 1000000,
                    .voltage_floor  = 875000,
                },
            },
            {   
                .corner_params =
                {   //15mV margin
                    .rosc_target[0] =  656,
                    .rosc_target[1] =  631,
                    .rosc_target[2] =  695,
                    .rosc_target[3] =  685,
                    .rosc_target[4] = 1078,
                    .rosc_target[5] = 1019,
                    .rosc_target[6] = 1159,
                    .rosc_target[7] = 1120,
                    .corner         = RAILWAY_SUPER_TURBO,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1112500,
                    .voltage_floor  = 937500,
                },
            },
        },
        .rbcpr_enablement           = RBCPR_ENABLED_CLOSED_LOOP,
        .number_of_target_params    = 3,
    },
};

const rbcpr_bsp_revision_array_type hw_version_specific_settings =
{
    .hw_version_count = 1,
    .hw_version_array = (const rbcpr_versioned_bsp_rail_params_type[])
    {
        {
            .fab_process_id_count = 1,
            .rbcpr_bsp_params_tsmc_fab = (const rbcpr_tsmc_fab_id_settings[])
            {
                {
                    .bsp_params = (rbcpr_bsp_rail_params_type*)&rbcpr_settings,
                    .process_id = 0,
                    .foundry_id = 0,
                },
            },

            .supported_hw_after_version = CHIPINFO_VERSION(0, 0),
        },
    },
    .supported_chipset = CHIPINFO_FAMILY_MDM9x45,
};

const rbcpr_supported_hw_array_type rbcpr_supported_hw_array =
{
    .hw_version_specific_settings = (const rbcpr_bsp_revision_array_type*[])
    {
        &hw_version_specific_settings,
    },
    .hw_version_specific_settings_count = 1,
};

const rbcpr_bsp_rail_const_info rbcpr_rail_const_info[] =
{
    // VDDMX
    {
        .ref_clk_resource       = "gcc_rbcpr_mx_clk",
        .ahb_clk_resource       = "gcc_rbcpr_mx_ahb_clk",
        .rail_name              = "vddmx",
        .interrupt_id           = 34,   // mx_rbif_irq[0]
        .idle_clocks            = 15,
        .step_quot              = 40,
        .pmic_step_size_uv      = 12500,
        .up_threshold           = 1,
        .dn_threshold           = 2,
        .consecutive_up         = 0,
        .consecutive_dn         = 2,
        .gcnt                   = 19,
        .count_mode             = HAL_RBCPR_STAGGERED,
        .disable_fused_floor    = true,
         // CPR HPG section 3.2.3 Target voltage fuse bits
         // Virtual pmic step size is 10000uv
         // Actual pmic steps = floor{(virtual pmic step size * num_of_fuse_steps)/(actual pmic step size)}
        .efuse_target_voltage_multipler = 10000,
    },

    // VDDCX
    {
        .ref_clk_resource       = "gcc_rbcpr_clk",
        .ahb_clk_resource       = "gcc_rbcpr_ahb_clk",
        .rail_name              = "vddcx",
        .interrupt_id           = 62,   // top_cpr_irq[0]
        .idle_clocks            = 15,
        .step_quot              = 40,
        .pmic_step_size_uv      = 12500,
        .up_threshold           = 1,
        .dn_threshold           = 2,
        .consecutive_up         = 0,
        .consecutive_dn         = 2,
        .gcnt                   = 19,
        .count_mode             = HAL_RBCPR_STAGGERED,
         // CPR HPG section 3.2.3 Target voltage fuse bits
         // Virtual pmic step size is 10000uv
         // Actual pmic steps = floor{(virtual pmic step size * num_of_fuse_steps)/(actual pmic step size)}
        .efuse_target_voltage_multipler = 10000,
    },
};

rbcpr_bsp_type rbcpr_bsp_data =
{
    .rails = (rbcpr_bsp_rail_info[])
    {
        {   // VDDMX
            .rail_const_info = &rbcpr_rail_const_info[0],
        },
        {   // VDDCX
            .rail_const_info = &rbcpr_rail_const_info[1],
        },
    },
    .num_rails = 2,
};

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

void rbcpr_bsp_target_init(void)
{
}

void rbcpr_bsp_init_rosc_target(rbcpr_bsp_rail_info* rail)
{
}

