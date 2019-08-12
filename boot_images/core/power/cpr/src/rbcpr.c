/*===========================================================================
                              rbcpr.c

SERVICES:

DESCRIPTION:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

Copyright (c) 2013 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <stdlib.h>

#include "HAL_rbcpr.h"
#include "rbcpr.h"

#include <stdbool.h>
#include "DDIChipInfo.h"
#include "HALhwio.h"
//#include "rbcpr_qfprom.h" 		// refer note 1 
#include "rbcpr_fuses.h"
#include "CoreVerify.h"
#include "smem.h"
#include "msmhwiobase.h"
#include "msmhwioreg.h"

/**********************************************
//TO DO 

note 1: 
The qfprom related register declaration macros differ between rbcpr_qfprom.h and msmhwioreg.h.
The msmhwioreg.h needs update from IPCAT.

msmhwioreg.h is required by this file for clock related registers in rbcpr_enable_clock() function
rbcpr_qfprom.h is required for TLMM_HW_REVISION_NUMBER register macro.

note 2:
PROCESS_NODE_ID and FOUNDRY_ID bit definitions (QFPROM_RAW_PTE2) are not present 
in the header file  generated from IPCAT. this has been raised and need to be uncommented 
when the file is updated.

***********************************************/

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------
bool regular_init_done = false;

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------


//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================


/*===========================================================================
FUNCTION: rbcpr_init_hw_revision_specifics

DESCRIPTION:

RETURN VALUE:
===========================================================================*/

void rbcpr_init_hw_revision_specifics(void)
{
    static bool rbcpr_init_hw_revision_specifics_done = false;
    if(rbcpr_init_hw_revision_specifics_done)
        return;
    rbcpr_init_hw_revision_specifics_done=true;

//   refer note 1
//    const uint32 hw_version = HWIO_INF(TLMM_HW_REVISION_NUMBER, VERSION_ID);

//    refer note 2	
//	const uint32 process_id = HWIO_INF(QFPROM_RAW_PTE1, PROCESS_NODE_ID);
//	const uint32 foundry_id = HWIO_INF(QFPROM_RAW_PTE2, FOUNDRY_ID);
    const uint32 hw_version = 0;
    const uint32 process_id = 0;
	const uint32 foundry_id = 0;

    const DalChipInfoFamilyType chip_family = DalChipInfo_ChipFamily();
    rbcpr_bsp_revision_array_type* hw_type_settings = NULL;

    for(int32 i=0; i<rbcpr_supported_hw_array.hw_version_specific_settings_count; i++)
    {
        if((*rbcpr_supported_hw_array.hw_version_specific_settings)[i].supported_chipset == chip_family)
        {
            hw_type_settings = &(*rbcpr_supported_hw_array.hw_version_specific_settings)[i];
            break;
        }
    }
    
    CORE_VERIFY_PTR(hw_type_settings);

    rbcpr_bsp_rail_params_type* settings_for_this_hw = NULL;    
    for(int32 i=0; i<hw_type_settings->hw_version_count; i++)
    {
        if(hw_type_settings->hw_version_array[i].supported_hw_after_version>hw_version)
        {
            break;
        }
		else
		{
			for(int32  j = 0 ;j < hw_type_settings->hw_version_array[i].fab_process_id_count; j++)
			{
				if(process_id == hw_type_settings->hw_version_array[i].rbcpr_bsp_params_fab[j].process_id && foundry_id == hw_type_settings->hw_version_array[i].rbcpr_bsp_params_fab[j].foundry_id)
				{
					settings_for_this_hw=hw_type_settings->hw_version_array[i].rbcpr_bsp_params_fab[j].bsp_params;
					break;
				}
			}
		}
	}
	
	if(!settings_for_this_hw)
    {
		settings_for_this_hw=hw_type_settings->hw_version_array[0].rbcpr_bsp_params_fab[0].bsp_params;
		(*settings_for_this_hw).rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP;
	}
	

    for(int32 i=0; i<rbcpr_bsp_data.num_rails; i++)
    {
        rbcpr_bsp_data.rails[i].bsp_data = &settings_for_this_hw[i];
    }
}

#define CPR_INVALID_CORNER -1
static void rbcpr_init_corner_lookup(rbcpr_bsp_rail_info* rail)
{
    for(int i=0; i<RAILWAY_CORNERS_COUNT; i++)
    {
        rail->corner_lookup_index[i] = CPR_INVALID_CORNER;
    }

    for(int i=0; i<rail->bsp_data->number_of_target_params; i++)
    {
        railway_corner corner = rail->bsp_data->target_params[i].corner_params.corner;
        CORE_VERIFY(corner<RAILWAY_CORNERS_COUNT);
        rail->corner_lookup_index[corner] = i;
    }
}

static const rbcpr_corner_params_type* rbcpr_corner_params(rbcpr_bsp_rail_info* rail, railway_corner corner)
{
    CORE_VERIFY(corner<RAILWAY_CORNERS_COUNT);
    int corner_lookup = rail->corner_lookup_index[corner];
    CORE_VERIFY(corner_lookup != CPR_INVALID_CORNER);
    return &rail->bsp_data->target_params[corner_lookup].corner_params;
}

/*===========================================================================
FUNCTION: rbcpr_read_efuse_data

DESCRIPTION: Function to read the e-fuses containing CPR block information

RETURN VALUE:
===========================================================================*/

static void rbcpr_set_corner_voltage_based_on_efuse(rbcpr_bsp_rail_info* rail, rbcpr_corner_params_and_fuse_type* corner_params_and_fuse)
{
    rbcpr_corner_params_type* corner_params = &corner_params_and_fuse->corner_params;
    
    const int32 voltage_offset_steps = rbcpr_fuses_get_fuse_value(rail->rail_const_info->rail_name, corner_params->fuse);

    const int voltage_offset = voltage_offset_steps * rail->rail_const_info->efuse_target_voltage_multipler;
    
    uint32 target_voltage = voltage_offset + corner_params->voltage_ceil;
    
    uint32 safe_floor = corner_params->voltage_floor;
    uint32 safe_ceil  = corner_params->voltage_ceil;
    
    

    target_voltage = MIN(safe_ceil, MAX(target_voltage, safe_floor));

    //Now round up to the PMIC step size.
    if(target_voltage%rail->rail_const_info->pmic_step_size_uv)
    {
        target_voltage += (rail->rail_const_info->pmic_step_size_uv - (target_voltage%rail->rail_const_info->pmic_step_size_uv));
    }
    
    CORE_VERIFY(target_voltage<=safe_ceil);
    
    railway_set_corner_voltage(rail->railway_rail_id, corner_params->corner, target_voltage);
    
    corner_params->voltage_ceil = target_voltage;
}

static void rbcpr_init_open_loop(rbcpr_bsp_rail_info* rail)
{
    for(int i=0; i<rail->bsp_data->number_of_target_params; i++)
    {
        if(rail->bsp_data->target_params[i].fuse_function)
        {
            rail->bsp_data->target_params[i].fuse_function(&rail->bsp_data->target_params[i].corner_params, rail);
        }
        else
        {
            rbcpr_set_corner_voltage_based_on_efuse(rail, &rail->bsp_data->target_params[i]);
        }
    }
}

/*===========================================================================
FUNCTION: rbcpr_configure

DESCRIPTION:
// Configure the CPR block for a particular 'corner'
// 1. Program all the ring oscillators with GCNT / TARGET
// 2. Program the step quot for the CPR block

RETURN VALUE:
===========================================================================*/
static void rbcpr_configure(rbcpr_bsp_rail_info* rail, railway_corner corner)
{
    int   rosc;
    const rbcpr_corner_params_type *corner_hw_data = rbcpr_corner_params(rail, corner);

    //Program all the roscs
    for (rosc = 0; rosc < RBCPR_ROSC_COUNT; rosc++)
    {
        uint8 gcnt = rail->rail_const_info->gcnt;
        if(corner_hw_data->rosc_target[rosc]==0)
        {
            gcnt = 0;
        }
        HAL_rbcpr_prgm_gcnt_target(rail->hal_handle, (HAL_rbcpr_rosc_type)rosc, gcnt, corner_hw_data->rosc_target[rosc]);
    }
}

void rbcpr_enable_clock()
{
    //enable the block 
    HWIO_OUTF(GCC_RBCPR_CBCR, CLK_ENABLE, 0x1);
    HWIO_OUTF(GCC_RBCPR_AHB_CBCR, CLK_ENABLE, 0x1);
    
    while(HWIO_INF(GCC_RBCPR_CBCR, CLK_OFF));
    while(HWIO_INF(GCC_RBCPR_AHB_CBCR, CLK_OFF));
}

/*===========================================================================
FUNCTION: rbcpr_settle_cx_voltage_corners

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void rbcpr_settle_voltage(rbcpr_bsp_rail_info* rail)
{
    //enable the block - hack for now
    rbcpr_enable_clock();
    
    CORE_VERIFY(rail->hal_handle->hw_base_address);
    
    /* Configure all the sensors to be enabled */
    HAL_rbcpr_mask_sensor_all(rail->hal_handle, FALSE);

    /* Configure bypass registers to bypass nothing */
    HAL_rbcpr_bypass_sensor_all(rail->hal_handle, FALSE);

    /* Now program the step quot */
    HAL_rbcpr_write_stepquot(rail->hal_handle,
                             rail->rail_const_info->step_quot,
                             rail->rail_const_info->idle_clocks);

    HAL_rbcpr_write_up_threshold(rail->hal_handle, rail->rail_const_info->up_threshold);
    HAL_rbcpr_write_dn_threshold(rail->hal_handle, rail->rail_const_info->dn_threshold);
    HAL_rbcpr_write_consecutive_up(rail->hal_handle, rail->rail_const_info->consecutive_up);
    HAL_rbcpr_write_consecutive_dn(rail->hal_handle, rail->rail_const_info->consecutive_dn);

    HAL_rbcpr_set_count_mode(rail->hal_handle, rail->rail_const_info->count_mode);

    /* Now program the timers */
    HAL_config_timers(rail->hal_handle);

    // Config the voltage limits as well.
    /* Configure the ceiling and floor values so that we get the step down recommendation */
    HAL_config_vlimits(rail->hal_handle);

    railway_voter_t cx_voter = railway_create_voter(rail->railway_rail_id, RAILWAY_CPR_SETTLING_VOTER);

    //We want to settle in the middle of our down and up thresholds.
    //Calculate that point now.
    const int threshold_midpoint = (rail->rail_const_info->dn_threshold 
                        + rail->rail_const_info->up_threshold) / 2;

    const int settling_point = rail->rail_const_info->dn_threshold - threshold_midpoint;
    CORE_VERIFY(settling_point >= 0);   //For now, only support configs where dn_threshold >= up_threshold.
    
    for(int i=0; i<rail->bsp_data->number_of_corners_to_settle_at; i++)
    {
        const railway_corner railway_corner_to_settle = rail->bsp_data->corners_to_settle_at[i];

        railway_corner_vote(cx_voter, railway_corner_to_settle);
        railway_transition_rails();

        //Check that we went to the correct voltage
        railway_settings settings;
        railway_get_current_settings(rail->railway_rail_id, &settings);
        CORE_VERIFY(settings.mode == railway_corner_to_settle);

        rbcpr_configure(rail, railway_corner_to_settle);

        HAL_rbcpr_result_type result;
        HAL_rbcpr_enable_block(rail->hal_handle, TRUE);
        HAL_rbcpr_poll_result(rail->hal_handle,&result);
        HAL_rbcpr_enable_block(rail->hal_handle, FALSE);

        HAL_rbcpr_enable_block(rail->hal_handle, TRUE);

        bool stuff_to_do = true;
        while (stuff_to_do)
        {
            HAL_rbcpr_poll_result(rail->hal_handle,&result);
            uint32 recommendation = railway_get_corner_voltage(rail->railway_rail_id, railway_corner_to_settle) -
                                        rail->rail_const_info->pmic_step_size_uv;

            if(result.step_down && result.error_steps>settling_point &&
               recommendation>=rbcpr_corner_params(rail, railway_corner_to_settle)->voltage_floor)
            {
                railway_set_corner_voltage(rail->railway_rail_id, railway_corner_to_settle, recommendation);

                railway_transition_rails();

                HAL_rbcpr_next_meas_en(rail->hal_handle, true);
            }
            else
            {
                stuff_to_do=false;
            }
        }

        HAL_rbcpr_enable_block(rail->hal_handle, FALSE);
    }

    //Lastly, null out our voter.
    railway_corner_vote(cx_voter, RAILWAY_NO_REQUEST);
}

void rbcpr_init_rail(rbcpr_bsp_rail_info* rail)
{
    if(rail->bsp_data->rbcpr_enablement==RBCPR_DISABLED)
    {
        return;
    }
    
    rbcpr_init_corner_lookup(rail);

    
    rail->railway_rail_id = rail_id(rail->rail_const_info->rail_name);
    CORE_VERIFY(rail->railway_rail_id != RAIL_NOT_SUPPORTED_BY_RAILWAY);

    rbcpr_init_open_loop(rail);
    
    if(rail->bsp_data->rbcpr_enablement==RBCPR_ENABLED_OPEN_LOOP)
    {
        return;
    }

    if(rail->bsp_data->number_of_corners_to_settle_at != 0)
    {
        rail->hal_handle = HAL_rbcpr_get_rail_handle(rail->rail_const_info->rail_name);
        CORE_VERIFY(rail->hal_handle);
        rbcpr_settle_voltage(rail);
    }
}

static void rbcpr_init_checksum_config(rbcpr_checksum_config_t *cfg)
{
    cfg->railway_rail_id = rail_id(cfg->rail_name); 
    CORE_VERIFY(cfg->railway_rail_id != RAIL_NOT_SUPPORTED_BY_RAILWAY);

    cfg->rbcpr_rail_id = RAIL_NOT_SUPPORTED_BY_RBCPR;

    /*
     * Resolve rbcpr rail IDs based on Railway rail IDs of rbcpr_bsp_data
     */
    for(int rbcpr_rail_id = 0; rbcpr_rail_id < rbcpr_bsp_data.num_rails; rbcpr_rail_id++)
    {
        if(rbcpr_bsp_data.rails[rbcpr_rail_id].railway_rail_id == cfg->railway_rail_id)
        {
            /*
             * Store rbcpr rail ID if it's enabled.
             */
            if(rbcpr_bsp_data.rails[rbcpr_rail_id].bsp_data->rbcpr_enablement != RBCPR_DISABLED)
            {
                cfg->rbcpr_rail_id = rbcpr_rail_id;
            }

            break;
        }
    }
}

static void rbcpr_init_checksum_configs(void)
{
    static bool rbcpr_init_checksum_configs_done = false;

    if(rbcpr_init_checksum_configs_done)
        return;

    rbcpr_init_checksum_configs_done = true;

    for(int i = 0; i < rbcpr_checksum_config_data.num_configs; i++)
    {
        rbcpr_init_checksum_config(&rbcpr_checksum_config_data.configs[i]);
    }
}

/*===========================================================================
FUNCTION: rbcpr_init

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void rbcpr_init(void)
{
    regular_init_done = true;

    rbcpr_fuses_init();
    rbcpr_init_hw_revision_specifics();


    for(int i = 0; i<rbcpr_bsp_data.num_rails; i++)
    {
        rbcpr_init_rail(&rbcpr_bsp_data.rails[i]);
    }

    rbcpr_init_checksum_configs();
}
