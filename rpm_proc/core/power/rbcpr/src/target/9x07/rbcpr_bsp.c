/*===========================================================================
  	
  	FILE:         rbcpr_bsp.c


	DESCRIPTION:  Per target rbcpr bsp configurations
  	
=============================================================================
	
Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/rbcpr/src/target/9x07/rbcpr_bsp.c#13 $
$Date: 2016/03/03 $
	 
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

#define SS_THRESHOLD_OPENLOOP_VOL  1187500
static bool rbcpr_ss_vol_threshold = false;
void rbcpr_bsp_check_ss_part(void);
extern int    cx_id;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
void rbcpr_pin_voltage_to_max_of_cx_apc(rbcpr_corner_params_type* corner_params, struct rbcpr_bsp_rail_info* rail, int);
void rbcpr_pin_mx_voltage_to_global_ceiling(rbcpr_corner_params_type* mx_corner_params, struct rbcpr_bsp_rail_info* mx_rail, int);
 /* -----------------------------------------------------------------------
**                     		DATA
** ----------------------------------------------------------------------- */

rbcpr_bsp_rail_params_type tsmc_tn1_cpr_settings[] =
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
                },
				{
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_HIGH,
                    .fuse           = RBCPR_FUSE_SVS_PLUS,
                    .voltage_ceil   = 1150000,
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
                },
				{
                .corner_params =
                    {
                    .corner         = RAILWAY_NOMINAL_HIGH,
                    .fuse           = RBCPR_FUSE_NOMINAL,
                    .voltage_ceil   = 1287500,
                    .voltage_floor  = 1137500,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000,
                    .voltage_floor  = 1187500,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=5,
     },
	
     {   // VDDCX
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                     {
                        .rosc_target[0] = 278,
                        .rosc_target[1] = 274,
                        .rosc_target[2] = 439,
                        .rosc_target[3] = 426,
                        .rosc_target[4] = 237,
                        .rosc_target[5] = 245,
                        .rosc_target[6] = 368,
                        .rosc_target[7] = 374,
                        .corner         = RAILWAY_SVS_SOC,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 900000,
                        .static_margin  = 25000,
						.ss_static_margin_adjust =0,
                        .static_margin_adjust = 1,
                    },
                },
				{
                .corner_params =
                     {
                        .rosc_target[0] = 391,
                        .rosc_target[1] = 385,
                        .rosc_target[2] = 582,
                        .rosc_target[3] = 567,
                        .rosc_target[4] = 339,
                        .rosc_target[5] = 349,
                        .rosc_target[6] = 494,
                        .rosc_target[7] = 499,
                        .corner         = RAILWAY_SVS_HIGH,
                        .fuse           = RBCPR_FUSE_SVS_PLUS,
                        .voltage_ceil   = 1150000,
                        .voltage_floor  = 975000,
                        .static_margin  = 25000,
						.ss_static_margin_adjust =0,
                        .static_margin_adjust = 1,
                    },
                },
                {
                .corner_params =
                    {
                        .rosc_target[0] = 477,
                        .rosc_target[1] = 469,
                        .rosc_target[2] = 676,
                        .rosc_target[3] = 646,
                        .rosc_target[4] = 416,
                        .rosc_target[5] = 425,
                        .rosc_target[6] = 578,
                        .rosc_target[7] = 567,
                        .corner         = RAILWAY_NOMINAL,
                        .fuse           = RBCPR_FUSE_NOMINAL,
                        .voltage_ceil   = 1225000,
                        .voltage_floor  = 1050000,
                        .static_margin  = 25000,
						.ss_static_margin_adjust=37500,
                        .static_margin_adjust = 0,
                    },
                },
				{
                .corner_params =
                    {
                        .rosc_target[0] = 559,
                        .rosc_target[1] = 549,
                        .rosc_target[2] = 762,
                        .rosc_target[3] = 714,
                        .rosc_target[4] = 490,
                        .rosc_target[5] = 496,
                        .rosc_target[6] = 656,
                        .rosc_target[7] = 627,
                        .corner         = RAILWAY_NOMINAL_HIGH,
                        .fuse           = RBCPR_FUSE_NOMINAL,
                        .voltage_ceil   = 1287500,
                        .voltage_floor  = 1075000,
                        .static_margin  = 12500,
						.ss_static_margin_adjust=0,
                        .static_margin_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                        .rosc_target[0] = 652,
                        .rosc_target[1] = 634,
                        .rosc_target[2] = 849,
                        .rosc_target[3] = 783,
                        .rosc_target[4] = 576,
                        .rosc_target[5] = 561,
                        .rosc_target[6] = 730,
                        .rosc_target[7] = 689,
                        .corner         = RAILWAY_SUPER_TURBO,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000,
                        .voltage_floor  = 1150000,
                        .static_margin  = 12500,
						.ss_static_margin_adjust=0,
                        .static_margin_adjust = 1,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_CLOSED_LOOP,
        .number_of_target_params=5,
		.voltage_scaling_vector = 
		    {
			    1575,
			    1500,
			    1750,
			    1575,
			    1375,
			    1325,
			    1575,
			    1375,
			},
		
    },
};

rbcpr_bsp_rail_params_type gf_tn1_cpr_settings[] =
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
                },
				{
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_HIGH,
                    .fuse           = RBCPR_FUSE_SVS_PLUS,
                    .voltage_ceil   = 1150000,
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
                },
				{
                .corner_params =
                    {
                    .corner         = RAILWAY_NOMINAL_HIGH,
                    .fuse           = RBCPR_FUSE_NOMINAL,
                    .voltage_ceil   = 1287500,
                    .voltage_floor  = 1137500,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000,
                    .voltage_floor  = 1187500,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=5,
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
                        .static_margin  = 100000,
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
                        .fuse           = RBCPR_FUSE_SVS_PLUS,
                        .voltage_ceil   = 1150000,
                        .voltage_floor  = 975000,
                        .static_margin  = 100000,
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
                        .static_margin  = 100000,
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
                        .corner         = RAILWAY_NOMINAL_HIGH,
                        .fuse           = RBCPR_FUSE_NOMINAL,
                        .voltage_ceil   = 1287500,
                        .voltage_floor  = 1075000,
                        .static_margin  = 100000,
                        .static_margin_adjust = 1,
                    },
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
                        .static_margin  = 100000,
                        .static_margin_adjust = 1,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=5,
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
                },
				{
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_HIGH,
                    .fuse           = RBCPR_FUSE_SVS_PLUS,
                    .voltage_ceil   = 1150000,
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
                },
				{
                .corner_params =
                    {
                    .corner         = RAILWAY_NOMINAL_HIGH,
                    .fuse           = RBCPR_FUSE_NOMINAL,
                    .voltage_ceil   = 1287500,
                    .voltage_floor  = 1137500,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000,
                    .voltage_floor  = 1187500,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=5,
     },
	
     {   // VDDCX
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                     {
                        .rosc_target[0] = 283,
                        .rosc_target[1] = 266,
                        .rosc_target[2] = 459,
                        .rosc_target[3] = 381,
                        .rosc_target[4] = 238,
                        .rosc_target[5] = 257,
                        .rosc_target[6] = 400,
                        .rosc_target[7] = 333,
                        .corner         = RAILWAY_SVS_SOC,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 900000,
                        .static_margin  = 25000,
						.ss_static_margin_adjust=0,
                        .static_margin_adjust = 1,
                    },
                },
				{
                .corner_params =
                     {
                        .rosc_target[0] = 392,
                        .rosc_target[1] = 368,
                        .rosc_target[2] = 590,
                        .rosc_target[3] = 500,
                        .rosc_target[4] = 330,
                        .rosc_target[5] = 355,
                        .rosc_target[6] = 516,
                        .rosc_target[7] = 440,
                        .corner         = RAILWAY_SVS_HIGH,
                        .fuse           = RBCPR_FUSE_SVS_PLUS,
                        .voltage_ceil   = 1150000,
                        .voltage_floor  = 975000,
                        .static_margin  = 25000,
						.ss_static_margin_adjust=12500,
                        .static_margin_adjust = 1,
                    },
                },
                {
                .corner_params =
                    {
                        .rosc_target[0] = 478,
                        .rosc_target[1] = 447,
                        .rosc_target[2] = 689,
                        .rosc_target[3] = 590,
                        .rosc_target[4] = 401,
                        .rosc_target[5] = 431,
                        .rosc_target[6] = 602,
                        .rosc_target[7] = 520,
                        .corner         = RAILWAY_NOMINAL,
                        .fuse           = RBCPR_FUSE_NOMINAL,
                        .voltage_ceil   = 1225000,
                        .voltage_floor  = 1050000,
                        .static_margin  = 37500,
						.ss_static_margin_adjust=25000,
                        .static_margin_adjust = 1,
                    },
                },
				{
                .corner_params =
                    {
                        .rosc_target[0] = 537,
                        .rosc_target[1] = 500,
                        .rosc_target[2] = 754,
                        .rosc_target[3] = 649,
                        .rosc_target[4] = 449,
                        .rosc_target[5] = 483,
                        .rosc_target[6] = 660,
                        .rosc_target[7] = 573,
                        .corner         = RAILWAY_NOMINAL_HIGH,
                        .fuse           = RBCPR_FUSE_NOMINAL,
                        .voltage_ceil   = 1287500,
                        .voltage_floor  = 1075000,
                        .static_margin  = 25000,
						.ss_static_margin_adjust=25000,
                        .static_margin_adjust = 1,
                    },
                },
                {
                .corner_params =
                    {
                        .rosc_target[0] = 647,
                        .rosc_target[1] = 600,
                        .rosc_target[2] = 874,
                        .rosc_target[3] = 758,
                        .rosc_target[4] = 539,
                        .rosc_target[5] = 580,
                        .rosc_target[6] = 764,
                        .rosc_target[7] = 671,
                        .corner         = RAILWAY_SUPER_TURBO,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000,
                        .voltage_floor  = 1150000,
                        .static_margin  = 12500,
						.ss_static_margin_adjust=12500,
                        .static_margin_adjust = 1,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_CLOSED_LOOP,
        .number_of_target_params=5,
		.voltage_scaling_vector = 
		    {
			    1511,
			    1383,
			    1717,
			    1557,
			    1249,
			    1338,
			    1506,
			    1396,
			},
		
    },
};
const rbcpr_bsp_revision_array_type hw_version_specific_settings_9x07 =
{
    .hw_version_count = 1,
    .hw_version_array = (const rbcpr_versioned_bsp_rail_params_type[])
    {
		{
		.fab_process_id_count = 3,
		.rbcpr_bsp_params_tsmc_fab = (const rbcpr_tsmc_fab_id_settings[])
			{
				{
					.bsp_params = ( rbcpr_bsp_rail_params_type*)&tsmc_tn1_cpr_settings,
					.process_id = 0,
					.foundry_id = 0,
				},
				{
					.bsp_params = ( rbcpr_bsp_rail_params_type*)&gf_tn1_cpr_settings,
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
		
		
    },
    .supported_chipset = CHIPINFO_FAMILY_MDM9x07,
};

const rbcpr_supported_hw_array_type rbcpr_supported_hw_array =
{
    .hw_version_specific_settings = (const rbcpr_bsp_revision_array_type*[])
    {
        &hw_version_specific_settings_9x07,
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
        10,11,
    },
};

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
}


void rbcpr_bsp_check_ss_part(void)
{

   if( railway_get_corner_voltage(cx_id, RAILWAY_NOMINAL) >= SS_THRESHOLD_OPENLOOP_VOL)
	 {
	    rbcpr_ss_vol_threshold = true; 
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
	
	rbcpr_bsp_check_ss_part();
	
	for (int i=0; i < rail->bsp_data->number_of_target_params; i++)	
    {
	if(rbcpr_ss_vol_threshold ==true)
    {
	    rail->bsp_data->target_params[i].corner_params.static_margin += rail->bsp_data->target_params[i].corner_params.ss_static_margin_adjust;
	}
	
    	for (int j=0; j < RBCPR_ROSC_COUNT; j++)
		{
			// divide it by 1000000, because voltage delta is in uv and voltage scaling factor (kv) is also multiplied by 1000 to avoid floating point arthmatic
			int32 calc_value = ((rail->bsp_data->target_params[i].corner_params.static_margin ) * (rail->bsp_data->voltage_scaling_vector[j]));
            calc_value = (int32)calc_value/1000000;
			  
			// adjust precision from above calculation
			rail->bsp_data->target_params[i].corner_params.rosc_target[j] +=  calc_value + rail->bsp_data->target_params[i].corner_params.static_margin_adjust;
			
		}
    }
}

