/*===========================================================================
  	
  	FILE:         rbcpr_bsp.c


	DESCRIPTION:  Per target rbcpr bsp configurations
  	
=============================================================================
	
Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/power/cpr/src/target/8952/rbcpr_bsp.c#1 $
$Date: 2015/09/01 $
	 
when       who     what, where, why
--------   ---     --------------------------------------------------------
  
===========================================================================
Copyright (c) 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary and Confidential.
===========================================================================*/

 /* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "rbcpr.h"
#include "railway.h"
#include "rbcpr_fuses.h"
#include "CoreVerify.h"


#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

rbcpr_bsp_rail_params_type gf_tn3_cpr_settings[] =
{
     {   // VDDCX
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
		            {
		                .rosc_target[0] = 307,
		                .rosc_target[1] = 354,
		                .rosc_target[2] = 624,
		                .rosc_target[3] = 612,
		                .rosc_target[4] = 331,
		                .rosc_target[5] = 286,
		                .rosc_target[6] = 496,
		                .rosc_target[7] = 450,
		                .corner         = RAILWAY_SVS_SOC,
		                .fuse           = RBCPR_FUSE_SVS,
		                .voltage_ceil   = 1050000,
		                .voltage_floor  = 900000,
		            },
                },
                {
                .corner_params =
		            {
		                .rosc_target[0] = 399,
		                .rosc_target[1] = 449,
		                .rosc_target[2] = 750,
		                .rosc_target[3] = 732,
		                .rosc_target[4] = 418,
		                .rosc_target[5] = 369,
		                .rosc_target[6] = 603,
		                .rosc_target[7] = 554,
		                .corner         = RAILWAY_NOMINAL,
		                .fuse           = RBCPR_FUSE_NOMINAL,
		                .voltage_ceil   = 1150000,
		                .voltage_floor  = 1000000,
		            },
                },
                {
                .corner_params =
					{
		                .rosc_target[0] = 481,
		                .rosc_target[1] = 529,
		                .rosc_target[2] = 850,
		                .rosc_target[3] = 825,
		                .rosc_target[4] = 491,
		                .rosc_target[5] = 444,
		                .rosc_target[6] = 688,
		                .rosc_target[7] = 637,
		                .corner         = RAILWAY_TURBO,
		                .fuse           = RBCPR_FUSE_TURBO,
		                .voltage_ceil   = 1225000,
		                .voltage_floor  = 1050000,
		            },
                },
                {
                .corner_params =
		            {
		                .rosc_target[0] = 551,
		                .rosc_target[1] = 598,
		                .rosc_target[2] = 938,
		                .rosc_target[3] = 904,
		                .rosc_target[4] = 554,
		                .rosc_target[5] = 510,
		                .rosc_target[6] = 760,
		                .rosc_target[7] = 708,
		                .corner         = RAILWAY_SUPER_TURBO,
		                .fuse           = RBCPR_FUSE_TURBO,
		                .voltage_ceil   = 1287500,
		                .voltage_floor  = 1137500,
		            },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_CLOSED_LOOP,
        .number_of_target_params=4,
        .corners_to_settle_at = (railway_corner[])
            {
                RAILWAY_SUPER_TURBO,
            },
        .number_of_corners_to_settle_at = 1,
    },
};

const rbcpr_bsp_rail_params_type tsmc_tn3_cpr_settings[] =
{
     {   // VDDCX
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
		            {
		                .rosc_target[0] = 364,
		                .rosc_target[1] = 370,
		                .rosc_target[2] = 829,
		                .rosc_target[3] = 790,
		                .rosc_target[4] = 329,
		                .rosc_target[5] = 343,
		                .rosc_target[6] = 549,
		                .rosc_target[7] = 521,
		                .corner         = RAILWAY_SVS_SOC,
		                .fuse           = RBCPR_FUSE_SVS,
		                .voltage_ceil   = 1050000,
		                .voltage_floor  = 900000,
		            },
                },
                {
                .corner_params =
		            {
		                .rosc_target[0] = 430,
		                .rosc_target[1] = 435,
		                .rosc_target[2] = 927,
		                .rosc_target[3] = 879,
		                .rosc_target[4] = 391,
		                .rosc_target[5] = 405,
		                .rosc_target[6] = 629,
		                .rosc_target[7] = 597,
		                .corner         = RAILWAY_NOMINAL,
		                .fuse           = RBCPR_FUSE_NOMINAL,
		                .voltage_ceil   = 1150000,
		                .voltage_floor  = 1000000,
		            },
                },
                {
                .corner_params =
					{
		                .rosc_target[0] = 483,
		                .rosc_target[1] = 485,
		                .rosc_target[2] = 991,
		                .rosc_target[3] = 937,
		                .rosc_target[4] = 441,
		                .rosc_target[5] = 452,
		                .rosc_target[6] = 686,
		                .rosc_target[7] = 650,
		                .corner         = RAILWAY_TURBO,
		                .fuse           = RBCPR_FUSE_TURBO,
		                .voltage_ceil   = 1225000,
		                .voltage_floor  = 1050000,
		            },
                },
                {
                .corner_params =
		            {
		                .rosc_target[0] = 537,
		                .rosc_target[1] = 535,
		                .rosc_target[2] = 1054,
		                .rosc_target[3] = 993,
		                .rosc_target[4] = 491,
		                .rosc_target[5] = 499,
		                .rosc_target[6] = 741,
		                .rosc_target[7] = 702,
		                .corner         = RAILWAY_SUPER_TURBO,
		                .fuse           = RBCPR_FUSE_TURBO,
		                .voltage_ceil   = 1287500,
		                .voltage_floor  = 1137500,
		            },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_CLOSED_LOOP,
        .number_of_target_params=4,
        .corners_to_settle_at = (railway_corner[])
            {
                RAILWAY_SUPER_TURBO,
            },
        .number_of_corners_to_settle_at = 1,
    },
};

rbcpr_bsp_revision_array_type hw_version_specific_settings =
{
    .hw_version_count = 1,
    .hw_version_array = (rbcpr_versioned_bsp_rail_params_type[])
    {
		{
		    .fab_process_id_count = 2,
    		.rbcpr_bsp_params_fab = (rbcpr_fab_id_settings[])
            {
                {
                    .bsp_params = (rbcpr_bsp_rail_params_type *)&gf_tn3_cpr_settings,
                    .process_id = 0,
				    .foundry_id = 1,
                },
                {
                    .bsp_params = (rbcpr_bsp_rail_params_type *)&tsmc_tn3_cpr_settings,
                    .process_id = 0,
				    .foundry_id = 0,
                },
		    },
		    .supported_hw_after_version= DALCHIPINFO_VERSION(0, 0), 
        },
    },
	.supported_chipset = DALCHIPINFO_FAMILY_MSM8952,
};

const rbcpr_supported_hw_array_type rbcpr_supported_hw_array =
{
    .hw_version_specific_settings = (rbcpr_bsp_revision_array_type*[])
    {
        &hw_version_specific_settings,
    },
    .hw_version_specific_settings_count = 1,
};

const rbcpr_bsp_rail_const_info rbcpr_rail_const_info[] =
{
    {   //VddCx
    .rail_name                           = "vddcx",
    .step_quot                           = 26,
    .pmic_step_size_uv                   = 12500,
    .up_threshold                        = 0,
    .dn_threshold                        = 2,
    .consecutive_up                      = 0,
    .consecutive_dn                      = 2,
    .gcnt                                = 19,
    .count_mode                          = HAL_RBCPR_STAGGERED,
    .idle_clocks                         = 15,
    .efuse_target_voltage_multipler      = 10000,
    },
};

rbcpr_bsp_type rbcpr_bsp_data = 
{
    .rails = (rbcpr_bsp_rail_info[])
    {
        {   //VddCx
             .rail_const_info = &rbcpr_rail_const_info[0],
        },
    },
    .num_rails = 1,
};

static rbcpr_checksum_config_t rbcpr_checksum_configs[] =
{
    /* MX */
    {
        .rail_name = "vddmx",
    },
    /* CX */
    {
        .rail_name = "vddcx",
    },
};

rbcpr_checksum_config_arr_t rbcpr_checksum_config_data =
{
    .configs     = rbcpr_checksum_configs,
    .num_configs = sizeof(rbcpr_checksum_configs)/sizeof(rbcpr_checksum_configs[0]),
};

