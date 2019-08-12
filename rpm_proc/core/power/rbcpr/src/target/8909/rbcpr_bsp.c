/*===========================================================================
  	
  	FILE:         rbcpr_bsp.c


	DESCRIPTION:  Per target rbcpr bsp configurations
  	
=============================================================================
	
Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/rbcpr/src/target/8909/rbcpr_bsp.c#4 $
$Date: 2015/10/11 $
	 
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
#include "rbcpr_comdef.h"
#include "rbcpr_qfprom.h"
#include "railway.h"
#include "Chipinfo.h"
#include "CoreVerify.h"
#include "rbcpr_stats.h"

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
void rbcpr_pin_voltage_to_max_of_cx_apc(rbcpr_corner_params_type* corner_params, struct rbcpr_bsp_rail_info* rail, int);
void rbcpr_pin_mx_voltage_to_global_ceiling(rbcpr_corner_params_type* mx_corner_params, struct rbcpr_bsp_rail_info* mx_rail, int);
 /* -----------------------------------------------------------------------
**                     		DATA
** ----------------------------------------------------------------------- */

rbcpr_bsp_rail_params_type tsmc_tn1_cpr_settings_v1[] =
{
     {   //VddMx
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_SOC,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 1050000,
                    .voltage_floor  = 900000,
                    },
				.fuse_function = rbcpr_pin_mx_voltage_to_global_ceiling,
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_HIGH,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 1050000,
                    .voltage_floor  = 1050000,
                    },
				
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_NOMINAL,
                    .fuse           = RBCPR_FUSE_NOMINAL,
                    .voltage_ceil   = 1225000,
                    .voltage_floor  = 1050000,
                    },
				.fuse_function = rbcpr_pin_mx_voltage_to_global_ceiling,
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000,
                    .voltage_floor  = 1150000,
                    },
				.fuse_function = rbcpr_pin_mx_voltage_to_global_ceiling,
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=4,
     },
	
     {   // VDDCX
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                     {
                        .rosc_target[0] = 283,
                        .rosc_target[1] = 294,
                        .rosc_target[2] = 528,
                        .rosc_target[3] = 492,
                        .rosc_target[4] = 254,
                        .rosc_target[5] = 272,
                        .rosc_target[6] = 463,
                        .rosc_target[7] = 441,
                        .corner         = RAILWAY_SVS_SOC,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 900000,
                        .static_margin  = 125000,
                        .static_margin_adjust = 1,
                    },
                },
                {
                .corner_params =
                     {
                        .rosc_target[0] = 283,
                        .rosc_target[1] = 294,
                        .rosc_target[2] = 528,
                        .rosc_target[3] = 492,
                        .rosc_target[4] = 254,
                        .rosc_target[5] = 272,
                        .rosc_target[6] = 463,
                        .rosc_target[7] = 441,
                        .corner         = RAILWAY_SVS_HIGH,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 1050000,
                        .static_margin  = 0,
                        .static_margin_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                        .rosc_target[0] = 503,
                        .rosc_target[1] = 503,
                        .rosc_target[2] = 795,
                        .rosc_target[3] = 743,
                        .rosc_target[4] = 459,
                        .rosc_target[5] = 469,
                        .rosc_target[6] = 707,
                        .rosc_target[7] = 669,
                        .corner         = RAILWAY_NOMINAL,
                        .fuse           = RBCPR_FUSE_NOMINAL,
                        .voltage_ceil   = 1225000,
                        .voltage_floor  = 1050000,
                        .static_margin  = 50000,
                        .static_margin_adjust = 1,
                    },
				.fuse_function = rbcpr_pin_voltage_to_max_of_cx_apc,
                },
                {
                .corner_params =
                    {
                        .rosc_target[0] = 690,
                        .rosc_target[1] = 678,
                        .rosc_target[2] = 1009,
                        .rosc_target[3] = 943,
                        .rosc_target[4] = 633,
                        .rosc_target[5] = 633,
                        .rosc_target[6] = 904,
                        .rosc_target[7] = 853,
                        .corner         = RAILWAY_SUPER_TURBO,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000,
                        .voltage_floor  = 1350000,
                        .static_margin  = 0,
                        .static_margin_adjust = 0,
                    },
				.fuse_function = rbcpr_pin_voltage_to_max_of_cx_apc,
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_CLOSED_LOOP,
        .number_of_target_params=4,
		.voltage_scaling_vector = 
		    {
			    1606,
			    1510,
			    1867,
			    1742,
			    1495,
			    1408,
			    1717,
			    1588,
			},
		
    },
};

rbcpr_bsp_rail_params_type tsmc_tn1_cpr_settings_v2[] =
{
     {   //VddMx
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_SOC,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 1050000,
                    .voltage_floor  = 1050000,
                    },
				.fuse_function = rbcpr_pin_mx_voltage_to_global_ceiling,
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_HIGH,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 1050000,
                    .voltage_floor  = 1050000,
                    },
				
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_NOMINAL,
                    .fuse           = RBCPR_FUSE_NOMINAL,
                    .voltage_ceil   = 1225000,
                    .voltage_floor  = 1062500,
                    },
				.fuse_function = rbcpr_pin_mx_voltage_to_global_ceiling,
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000,
                    .voltage_floor  = 1187500,
                    },
				.fuse_function = rbcpr_pin_mx_voltage_to_global_ceiling,
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=4,
     },
	
     {   // VDDCX
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                     {
                        .rosc_target[0] = 294,
                        .rosc_target[1] = 300,
                        .rosc_target[2] = 524,
                        .rosc_target[3] = 506,
                        .rosc_target[4] = 239,
                        .rosc_target[5] = 258,
                        .rosc_target[6] = 420,
                        .rosc_target[7] = 428,
                        .corner         = RAILWAY_SVS_SOC,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 900000,
                        .static_margin  = 50000,
                        .static_margin_adjust = 0,
                    },
                },
                {
                .corner_params =
                     {
                        .rosc_target[0] = 294,
                        .rosc_target[1] = 300,
                        .rosc_target[2] = 524,
                        .rosc_target[3] = 506,
                        .rosc_target[4] = 239,
                        .rosc_target[5] = 258,
                        .rosc_target[6] = 420,
                        .rosc_target[7] = 428,
                        .corner         = RAILWAY_SVS_HIGH,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 1050000,
                        .static_margin  = 0,
                        .static_margin_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                        .rosc_target[0] = 507,
                        .rosc_target[1] = 506,
                        .rosc_target[2] = 775,
                        .rosc_target[3] = 735,
                        .rosc_target[4] = 428,
                        .rosc_target[5] = 442,
                        .rosc_target[6] = 641,
                        .rosc_target[7] = 634,
                        .corner         = RAILWAY_NOMINAL,
                        .fuse           = RBCPR_FUSE_NOMINAL,
                        .voltage_ceil   = 1225000,
                        .voltage_floor  = 1050000,
                        .static_margin  = 12500,
                        .static_margin_adjust = 1,
                    },
				.fuse_function = rbcpr_pin_voltage_to_max_of_cx_apc,
                },
                {
                .corner_params =
                    {
                        .rosc_target[0] = 697,
                        .rosc_target[1] = 685,
                        .rosc_target[2] = 981,
                        .rosc_target[3] = 917,
                        .rosc_target[4] = 598,
                        .rosc_target[5] = 603,
                        .rosc_target[6] = 826,
                        .rosc_target[7] = 799,
                        .corner         = RAILWAY_SUPER_TURBO,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000,
                        .voltage_floor  = 1150000,
                        .static_margin  = 0,
                        .static_margin_adjust = 0,
                    },
				.fuse_function = rbcpr_pin_voltage_to_max_of_cx_apc,
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_CLOSED_LOOP,
        .number_of_target_params=4,
		.voltage_scaling_vector = 
		    {
			    1600,
			    1500,
			    1700,
			    1500,
			    1400,
			    1300,
			    1500,
			    1400,
			},
		
    },
};


rbcpr_bsp_rail_params_type gf_tn1_cpr_settings_v1[] =
{
     {   //VddMx
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_SOC,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 1050000,
                    .voltage_floor  = 900000,
                    },
				.fuse_function = rbcpr_pin_mx_voltage_to_global_ceiling,
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_HIGH,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 1050000,
                    .voltage_floor  = 1050000,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_NOMINAL,
                    .fuse           = RBCPR_FUSE_NOMINAL,
                    .voltage_ceil   = 1225000,
                    .voltage_floor  = 1050000,
                    },
				.fuse_function = rbcpr_pin_mx_voltage_to_global_ceiling,
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000,
                    .voltage_floor  = 1150000,
                    },
				.fuse_function = rbcpr_pin_mx_voltage_to_global_ceiling,
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=4,
		
     },
	
     {   // VDDCX
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                     {
                        .rosc_target[0] = 268,
                        .rosc_target[1] = 281,
                        .rosc_target[2] = 516,
                        .rosc_target[3] = 466,
                        .rosc_target[4] = 266,
                        .rosc_target[5] = 250,
                        .rosc_target[6] = 473,
                        .rosc_target[7] = 427,
                        .corner         = RAILWAY_SVS_SOC,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 900000,
                        .static_margin  = 125000,
                        .static_margin_adjust = 1,
                    },
                },
                {
                .corner_params =
                     {
                        .rosc_target[0] = 268,
                        .rosc_target[1] = 281,
                        .rosc_target[2] = 516,
                        .rosc_target[3] = 466,
                        .rosc_target[4] = 266,
                        .rosc_target[5] = 250,
                        .rosc_target[6] = 473,
                        .rosc_target[7] = 427,
                        .corner         = RAILWAY_SVS_HIGH,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 1050000,
                        .static_margin  = 0,
                        .static_margin_adjust = 1,
                    },
				},
                {
                .corner_params =
                    {
                        .rosc_target[0] = 464,
                        .rosc_target[1] = 472,
                        .rosc_target[2] = 759,
                        .rosc_target[3] = 696,
                        .rosc_target[4] = 442,
                        .rosc_target[5] = 431,
                        .rosc_target[6] = 695,
                        .rosc_target[7] = 635,
                        .corner         = RAILWAY_NOMINAL,
                        .fuse           = RBCPR_FUSE_NOMINAL,
                        .voltage_ceil   = 1225000,
                        .voltage_floor  = 1050000,
                        .static_margin  = 50000,
                        .static_margin_adjust = 1,
                    },
				.fuse_function = rbcpr_pin_voltage_to_max_of_cx_apc,
                },
                {
                .corner_params =
                    {
                        .rosc_target[0] = 634,
                        .rosc_target[1] = 631,
                        .rosc_target[2] = 950,
                        .rosc_target[3] = 876,
                        .rosc_target[4] = 588,
                        .rosc_target[5] = 588,
                        .rosc_target[6] = 871,
                        .rosc_target[7] = 798,
                        .corner         = RAILWAY_SUPER_TURBO,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000,
                        .voltage_floor  = 1350000,
                        .static_margin  = 0,
                        .static_margin_adjust = 0,
                    },
				.fuse_function = rbcpr_pin_voltage_to_max_of_cx_apc,
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_CLOSED_LOOP,
        .number_of_target_params=4,
		.voltage_scaling_vector = 
		    {
			    1429,
			    1386,
			    1695,
			    1626,
			    1271,
			    1321,
			    1552,
			    1464,
			},
		
    },
};

rbcpr_bsp_rail_params_type gf_tn1_cpr_settings_v2[] =
{
     {   //VddMx
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_SOC,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 1050000,
                    .voltage_floor  = 1050000,
                    },
				.fuse_function = rbcpr_pin_mx_voltage_to_global_ceiling,
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_HIGH,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 1050000,
                    .voltage_floor  = 1050000,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_NOMINAL,
                    .fuse           = RBCPR_FUSE_NOMINAL,
                    .voltage_ceil   = 1225000,
                    .voltage_floor  = 1062500,
                    },
				.fuse_function = rbcpr_pin_mx_voltage_to_global_ceiling,
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000,
                    .voltage_floor  = 1187500,
                    },
				.fuse_function = rbcpr_pin_mx_voltage_to_global_ceiling,
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=4,
		
     },
	
     {   // VDDCX
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                     {
                        .rosc_target[0] = 268,
                        .rosc_target[1] = 281,
                        .rosc_target[2] = 516,
                        .rosc_target[3] = 466,
                        .rosc_target[4] = 266,
                        .rosc_target[5] = 250,
                        .rosc_target[6] = 473,
                        .rosc_target[7] = 427,
                        .corner         = RAILWAY_SVS_SOC,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 900000,
                        .static_margin  = 25000,
                        .static_margin_adjust = 1,
                    },
                },
                {
                .corner_params =
                     {
                        .rosc_target[0] = 268,
                        .rosc_target[1] = 281,
                        .rosc_target[2] = 516,
                        .rosc_target[3] = 466,
                        .rosc_target[4] = 266,
                        .rosc_target[5] = 250,
                        .rosc_target[6] = 473,
                        .rosc_target[7] = 427,
                        .corner         = RAILWAY_SVS_HIGH,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 1050000,
                        .static_margin  = 0,
                        .static_margin_adjust = 1,
                    },
				},
                {
                .corner_params =
                    {
                        .rosc_target[0] = 464,
                        .rosc_target[1] = 472,
                        .rosc_target[2] = 759,
                        .rosc_target[3] = 696,
                        .rosc_target[4] = 442,
                        .rosc_target[5] = 431,
                        .rosc_target[6] = 695,
                        .rosc_target[7] = 635,
                        .corner         = RAILWAY_NOMINAL,
                        .fuse           = RBCPR_FUSE_NOMINAL,
                        .voltage_ceil   = 1225000,
                        .voltage_floor  = 1050000,
                        .static_margin  = 25000,
                        .static_margin_adjust = 1,
                    },
				.fuse_function = rbcpr_pin_voltage_to_max_of_cx_apc,
                },
                {
                .corner_params =
                    {
                        .rosc_target[0] = 634,
                        .rosc_target[1] = 631,
                        .rosc_target[2] = 950,
                        .rosc_target[3] = 876,
                        .rosc_target[4] = 588,
                        .rosc_target[5] = 588,
                        .rosc_target[6] = 871,
                        .rosc_target[7] = 798,
                        .corner         = RAILWAY_SUPER_TURBO,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000,
                        .voltage_floor  = 1150000,
                        .static_margin  = 25000,
                        .static_margin_adjust = 1,
                    },
				.fuse_function = rbcpr_pin_voltage_to_max_of_cx_apc,
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_CLOSED_LOOP,
        .number_of_target_params=4,
		.voltage_scaling_vector = 
		    {
			    1429,
			    1386,
			    1695,
			    1626,
			    1271,
			    1321,
			    1552,
			    1464,
			},
		
    },
};

rbcpr_bsp_rail_params_type smic_tn1_cpr_settings[] =
{
     {   //VddMx
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_SOC,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 1050000,
                    .voltage_floor  = 1050000,
                    },
				.fuse_function = rbcpr_pin_mx_voltage_to_global_ceiling,
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_HIGH,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 1050000,
                    .voltage_floor  = 1050000,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_NOMINAL,
                    .fuse           = RBCPR_FUSE_NOMINAL,
                    .voltage_ceil   = 1225000,
                    .voltage_floor  = 1062500,
                    },
				.fuse_function = rbcpr_pin_mx_voltage_to_global_ceiling,
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000,
                    .voltage_floor  = 1187500,
                    },
				.fuse_function = rbcpr_pin_mx_voltage_to_global_ceiling,
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=4,
		
     },
	
     {   // VDDCX
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                     {
                        .rosc_target[0] = 332,
                        .rosc_target[1] = 298,
                        .rosc_target[2] = 488,
                        .rosc_target[3] = 388,
                        .rosc_target[4] = 264,
                        .rosc_target[5] = 298,
                        .rosc_target[6] = 428,
                        .rosc_target[7] = 342,
                        .corner         = RAILWAY_SVS_SOC,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 900000,
                        .static_margin  = -12500,
                        .static_margin_adjust = 1,
                    },
                },
                {
                .corner_params =
                     {
						.rosc_target[0] = 332,
                        .rosc_target[1] = 298,
                        .rosc_target[2] = 488,
                        .rosc_target[3] = 388,
                        .rosc_target[4] = 264,
                        .rosc_target[5] = 298,
                        .rosc_target[6] = 428,
                        .rosc_target[7] = 342,
                        .corner         = RAILWAY_SVS_HIGH,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 1050000,
                        .static_margin  = 0,
                        .static_margin_adjust = 0,
                    },
				},
                {
                .corner_params =
                    {
                        .rosc_target[0] = 535,
                        .rosc_target[1] = 477,
                        .rosc_target[2] = 715,
                        .rosc_target[3] = 580,
                        .rosc_target[4] = 426,
                        .rosc_target[5] = 477,
                        .rosc_target[6] = 628,
                        .rosc_target[7] = 515,
                        .corner         = RAILWAY_NOMINAL,
                        .fuse           = RBCPR_FUSE_NOMINAL,
                        .voltage_ceil   = 1225000,
                        .voltage_floor  = 1050000,
                        .static_margin  = 12500,
                        .static_margin_adjust = 1,
                    },
				.fuse_function = rbcpr_pin_voltage_to_max_of_cx_apc,
                },
                {
                .corner_params =
                    {
                        .rosc_target[0] = 702,
                        .rosc_target[1] = 624,
                        .rosc_target[2] = 894,
                        .rosc_target[3] = 732,
                        .rosc_target[4] = 558,
                        .rosc_target[5] = 624,
                        .rosc_target[6] = 785,
                        .rosc_target[7] = 652,
                        .corner         = RAILWAY_SUPER_TURBO,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000,
                        .voltage_floor  = 1150000,
                        .static_margin  = 0,
                        .static_margin_adjust = 0,
                    },
				.fuse_function = rbcpr_pin_voltage_to_max_of_cx_apc,
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_CLOSED_LOOP,
        .number_of_target_params=4,
		.voltage_scaling_vector = 
		    {
			    1520,
			    1330,
			    1630,
			    1390,
			    1190,
			    1330,
			    1430,
			    1240,
			},
		
    },
};
const rbcpr_bsp_revision_array_type hw_version_specific_settings_8909 =
{
    .hw_version_count = 2,
    .hw_version_array = (const rbcpr_versioned_bsp_rail_params_type[])
    {
		{
		.fab_process_id_count = 3,
		.rbcpr_bsp_params_tsmc_fab = (const rbcpr_tsmc_fab_id_settings[])
			{
				{
					.bsp_params = ( rbcpr_bsp_rail_params_type*)&tsmc_tn1_cpr_settings_v1,
					.process_id = 0,
					.foundry_id = 0,
				},
				{
					.bsp_params = ( rbcpr_bsp_rail_params_type*)&gf_tn1_cpr_settings_v1,
					.process_id = 0,
					.foundry_id = 1,
				},
                {
                    .bsp_params = ( rbcpr_bsp_rail_params_type*)&smic_tn1_cpr_settings,
                    .process_id = 0,
                    .foundry_id = 5,
                },
			},
		 
        .supported_hw_after_version = CHIPINFO_VERSION(1, 0),
		},
		{
		.fab_process_id_count = 3,
		.rbcpr_bsp_params_tsmc_fab = (const rbcpr_tsmc_fab_id_settings[])
			{
				{
					.bsp_params = ( rbcpr_bsp_rail_params_type*)&tsmc_tn1_cpr_settings_v2,
					.process_id = 0,
					.foundry_id = 0,
				},
				{
					.bsp_params = ( rbcpr_bsp_rail_params_type*)&gf_tn1_cpr_settings_v2,
					.process_id = 0,
					.foundry_id = 1,
				},
                {
                    .bsp_params = ( rbcpr_bsp_rail_params_type*)&smic_tn1_cpr_settings,
                    .process_id = 0,
                    .foundry_id = 5,
				
				},
			},
		 
        .supported_hw_after_version = CHIPINFO_VERSION(2, 0),
		}
		
    },
    .supported_chipset = CHIPINFO_FAMILY_MSM8909,
};

const rbcpr_supported_hw_array_type rbcpr_supported_hw_array =
{
    .hw_version_specific_settings = (const rbcpr_bsp_revision_array_type*[])
    {
        &hw_version_specific_settings_8909,
    },
    .hw_version_specific_settings_count = 1,
};

const rbcpr_bsp_rail_const_info rbcpr_rail_const_info[] =
{
    {   //VddMx
    .rail_name                      = "vddmx",
    .pmic_step_size_uv              = 12500, 
    .efuse_target_voltage_multipler = 10000,
	.disable_fused_floor            = true,
    },
    {   // VDDCX
        .ref_clk_resource       = "gcc_rbcpr_clk",
        .ahb_clk_resource       = "gcc_rbcpr_ahb_clk",
        .rail_name              = "vddcx",
        .interrupt_id           = 62,   // top_cpr_irq[0]
        .idle_clocks            = 15,
        .step_quot              = 26,
        .pmic_step_size_uv      = 12500,
        .up_threshold           = 0,
        .dn_threshold           = 2,
        .consecutive_up         = 0,
        .consecutive_dn         = 2,
        .gcnt                   = 19,
        .count_mode = HAL_RBCPR_STAGGERED,
        .efuse_target_voltage_multipler = 10000,
        .disable_fused_floor            = false,
    },
};

rbcpr_bsp_type rbcpr_bsp_data =
{
    .rails = (rbcpr_bsp_rail_info[])
    {
        {   //VddMx
             .rail_const_info = &rbcpr_rail_const_info[0],
        },
        {   //VddCx
             .rail_const_info = &rbcpr_rail_const_info[1],
        },
    },
    .num_rails = 2,
};

const rbcpr_sensor_set mx_sensors = 
{
    .rail_id = 1,       //Cx
    .sensors_count = 2,
    .sensors = (const int[])
    {
        30,31,
    },
};

const rbcpr_sensor_set apc_sensors = 
{
    .rail_id = 1,       //Cx
    .sensors_count = 6,
    .sensors = (const int[])
    {
        23,24,25,26,27,28,
    },
};

const rbcpr_sensor_set cx_sensors_v1 = 
{
    .rail_id = 1,       //Cx
    .sensors_count = 2,
    .sensors = (const int[])
    {
        29,32,
    },
};


extern uint32 railway_target_type;
extern void rbcpr_set_corner_voltage_based_on_efuse(rbcpr_bsp_rail_info* rail, rbcpr_corner_params_and_fuse_type* corner_params_and_fuse);

void rbcpr_pin_voltage_to_max_of_cx_apc(rbcpr_corner_params_type* corner_params, struct rbcpr_bsp_rail_info* rail, int corner)
{

  rbcpr_corner_params_and_fuse_type* corner_params_and_fuse = &rail->bsp_data->target_params[corner];
  
  if (railway_target_type != RAILWAY_CX_APC_COMBINED)
  {
    rbcpr_set_corner_voltage_based_on_efuse(rail, corner_params_and_fuse);
	return;
  }

  int cx_voltage_offset_steps = rbcpr_fuses_get_fuse_value("vddcx", corner_params->fuse);
  int apc_voltage_offset_steps = rbcpr_fuses_get_fuse_value("vddapc0", corner_params->fuse);   
  int voltage_delta= 0, voltage_offset_steps = 0;
  const uint32 default_ceil  = corner_params->voltage_ceil;
  
  
  voltage_offset_steps = MAX(cx_voltage_offset_steps, apc_voltage_offset_steps);
  
  voltage_delta= voltage_offset_steps - cx_voltage_offset_steps;
  
  //First, pull up the floor voltage based on the eFuse using the formula
  // Floor = MAX( FF-Ceil, CEILING( ( 0.9*SS_Ceiling - ( 10mV * CPR-fuse ) + 37.5mV + Correlation_factor) , 5mV (i.e. PMIC step size) )
    
  const int32 capped_fuse_value = (voltage_offset_steps>0) ? 0 : voltage_offset_steps;
    
  uint32 safe_floor = ((default_ceil * 90) / 100) + (10000*capped_fuse_value) + 40000 + corner_params_and_fuse->fuse_correlation_factor;
    
  //Round up to multiple of PMIC step size.
  if(safe_floor%rail->rail_const_info->pmic_step_size_uv)
  {
    safe_floor += (rail->rail_const_info->pmic_step_size_uv - (safe_floor%rail->rail_const_info->pmic_step_size_uv));
  }
    
  const uint32 default_floor = corner_params->voltage_floor;
    
  safe_floor = MAX(default_floor, safe_floor);    //Bring up to at least the existing floor.
  safe_floor = MIN(default_ceil, safe_floor);     //Make sure we're not somehow setting the floor to be greater than the ceiling.
    
  if(rail->rail_const_info->disable_fused_floor == false)
  {
    corner_params->voltage_floor = safe_floor;
  }
	
	
	
  const int voltage_offset = voltage_offset_steps * rail->rail_const_info->efuse_target_voltage_multipler;
  uint32 target_voltage = voltage_offset + corner_params->voltage_ceil;
    
  target_voltage = MIN(corner_params->voltage_ceil, MAX(target_voltage, corner_params->voltage_floor));

  //Now round up to the PMIC step size.
  if(target_voltage%rail->rail_const_info->pmic_step_size_uv)
  {
      target_voltage += (rail->rail_const_info->pmic_step_size_uv - (target_voltage%rail->rail_const_info->pmic_step_size_uv));
  }
    
  CORE_VERIFY(target_voltage<=corner_params->voltage_ceil);
  
  railway_set_corner_voltage(rail->railway_rail_id, corner_params->corner, target_voltage);
  corner_params->voltage_ceil = target_voltage;
  
  rbcpr_stats_set_fuse_microvolts(rail->stats_rail_id, rbcpr_corner_index(rail, corner_params->corner), target_voltage);
	
  if (voltage_delta && 
     (rail->bsp_data->rbcpr_enablement == RBCPR_ENABLED_CLOSED_LOOP))
  {
	uint32 voltage_delta_uv = voltage_delta * rail->rail_const_info->efuse_target_voltage_multipler;
	if (voltage_delta_uv % rail->rail_const_info->pmic_step_size_uv)
	{
	    voltage_delta_uv += rail->rail_const_info->pmic_step_size_uv - (voltage_delta_uv % rail->rail_const_info->pmic_step_size_uv);
	}
		   
	for (int j=0;j < RBCPR_ROSC_COUNT ; j++)
	{
	  // devide it by 1000000, because voltage delta is in uv and voltage scaling factor (kv) is also multiplied by 1000 to avoid floating point arthmatic
	  corner_params->rosc_target[j] += ((voltage_delta_uv)* (rail->bsp_data->voltage_scaling_vector[j]))/1000000;
			  
	  // adjust precision from above calculation
	  corner_params->rosc_target[j] +=corner_params->static_margin_adjust;
			  
	}
  }
  
}

uint32 rbcpr_open_loop_ceiling(const rbcpr_bsp_rail_info* rail_info, rbcpr_fuse_type fuse, int voltage_offset_steps )
{
    rbcpr_corner_params_type* corner_params = NULL;

    const int32 voltage_offset = voltage_offset_steps * rail_info->rail_const_info->efuse_target_voltage_multipler;
    
    for(int i=0; i<rail_info->bsp_data->number_of_target_params; i++)
    {
        if(rail_info->bsp_data->target_params[i].corner_params.fuse == fuse)
        {
           corner_params = &rail_info->bsp_data->target_params[i].corner_params;
		   break;
        }
    }

    CORE_VERIFY(corner_params);
	
	uint32 open_loop_ceiling = voltage_offset + corner_params->voltage_ceil;
	
	open_loop_ceiling = MIN(corner_params->voltage_ceil, MAX(open_loop_ceiling, corner_params->voltage_floor));
	
    //Now round up to the PMIC step size.
    if(open_loop_ceiling % rail_info->rail_const_info->pmic_step_size_uv)
    {
        open_loop_ceiling += (rail_info->rail_const_info->pmic_step_size_uv - (open_loop_ceiling % rail_info->rail_const_info->pmic_step_size_uv));
    }
    
    CORE_VERIFY((corner_params->voltage_ceil % rail_info->rail_const_info->pmic_step_size_uv) == 0);      //This should always be true if the config is correct.
    
    open_loop_ceiling = MIN(corner_params->voltage_ceil, open_loop_ceiling);
    
    return open_loop_ceiling;
}



void rbcpr_pin_mx_voltage_to_global_ceiling(rbcpr_corner_params_type* mx_corner_params, struct rbcpr_bsp_rail_info* mx_rail, int corner)
{
    uint32 global_open_loop_ceiling = 0;
	int cx_voltage_offset_steps=0, apc_voltage_offset_steps=0, mx_voltage_offset_steps=0;
	rbcpr_bsp_rail_info* cx_rail = &rbcpr_bsp_data.rails[1];

    cx_voltage_offset_steps = rbcpr_fuses_get_fuse_value("vddcx", mx_corner_params->fuse);
    mx_voltage_offset_steps = rbcpr_fuses_get_fuse_value("vddmx", mx_corner_params->fuse);  
		
    apc_voltage_offset_steps = rbcpr_fuses_get_fuse_value("vddapc0", mx_corner_params->fuse);   
    cx_voltage_offset_steps = MAX(cx_voltage_offset_steps, apc_voltage_offset_steps);


	global_open_loop_ceiling = rbcpr_open_loop_ceiling(cx_rail, mx_corner_params->fuse, cx_voltage_offset_steps);
	global_open_loop_ceiling = MAX(global_open_loop_ceiling, rbcpr_open_loop_ceiling(mx_rail, mx_corner_params->fuse, mx_voltage_offset_steps));
	
    //Now round global_open_loop_ceiling up to the PMIC step size.
    if(global_open_loop_ceiling % mx_rail->rail_const_info->pmic_step_size_uv)
    {
        global_open_loop_ceiling += (mx_rail->rail_const_info->pmic_step_size_uv - (global_open_loop_ceiling % mx_rail->rail_const_info->pmic_step_size_uv));
    }
    
    CORE_VERIFY(global_open_loop_ceiling <= mx_corner_params->voltage_ceil);
    
    railway_set_corner_voltage(mx_rail->railway_rail_id, mx_corner_params->corner, global_open_loop_ceiling);

    mx_corner_params->voltage_ceil = global_open_loop_ceiling;
}


void rbcpr_bsp_target_init()
{
    const int cx_rail_id = mx_sensors.rail_id;
    
    rbcpr_bsp_rail_info* rail = &rbcpr_bsp_data.rails[cx_rail_id];
    
    if(rail->bsp_data->rbcpr_enablement != RBCPR_ENABLED_CLOSED_LOOP)
    {
        return; //We're not using the CPR HW block on the relevant rail, just return.
    }

    for(int i=0; i< mx_sensors.sensors_count; i++)
    {
        HAL_rbcpr_mask_sensor(rail->hal_handle, mx_sensors.sensors[i], TRUE);
    }
	
	if (railway_target_type == RAILWAY_CX_APC_COMBINED && (Chipinfo_GetVersion() == CHIPINFO_VERSION(1,0)))
	{
	    for(int i=0; i< cx_sensors_v1.sensors_count; i++)
       {
          HAL_rbcpr_mask_sensor(rail->hal_handle, cx_sensors_v1.sensors[i], TRUE);
       }
    }
	
	if (railway_target_type != RAILWAY_CX_APC_COMBINED)
	{
	    for(int i=0; i< apc_sensors.sensors_count; i++)
       {
          HAL_rbcpr_bypass_sensor(rail->hal_handle, apc_sensors.sensors[i], TRUE);
       }
    }
}

/*===========================================================================
FUNCTION: rbcpr_init_rosc_target

DESCRIPTION: initializes the rosc_target values based on static margin if closed loop

RETURN VALUE: None
===========================================================================*/
void rbcpr_bsp_init_rosc_target(rbcpr_bsp_rail_info* rail)
{
    if (rail->bsp_data->rbcpr_enablement != RBCPR_ENABLED_CLOSED_LOOP)
	    return;
	
    for (int i=0; i < rail->bsp_data->number_of_target_params; i++)	
    {
		for (int j=0; j < RBCPR_ROSC_COUNT; j++)
		{
			// divide it by 1000000, because voltage delta is in uv and voltage scaling factor (kv) is also multiplied by 1000 to avoid floating point arthmatic
			int32 calc_value = ((rail->bsp_data->target_params[i].corner_params.static_margin) * (rail->bsp_data->voltage_scaling_vector[j]));
            calc_value = (int32)calc_value/1000000;
			  
			// adjust precision from above calculation
			rail->bsp_data->target_params[i].corner_params.rosc_target[j] +=  calc_value + rail->bsp_data->target_params[i].corner_params.static_margin_adjust;
			
		}
    }
}
