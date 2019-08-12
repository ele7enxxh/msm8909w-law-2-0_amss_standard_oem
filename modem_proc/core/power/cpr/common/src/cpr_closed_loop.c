/*===========================================================================

  Copyright (c) 2014-2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_closed_loop.h"
#include "CoreVerify.h"
#include <string.h>
#include "cpr_image.h"
#include "cpr_enablement.h"
#include "cpr_voltage_ranges.h"
#include "cpr_logging.h"
#include <stdlib.h>
#include "cpr_smem.h"
#ifndef WINSIM
#include "stringl/stringl.h"
#endif

cpr_closed_loop_rail_t cpr_closed_loop_rail_root = NULL;

void cpr_enable_init_fn_closed_loop(cpr_rail_id_t rail_id,
                                    const cpr_enablement_versioned_rail_config_t* rail_enablement_config,
                                    cpr_enablement_stage_t enablement_stage);

const cpr_enablement_init_params_t CPR_ENABLE_CLOSED_LOOP =
{
    .enablement_type        = CPR_ENABLEMENT_CLOSED_LOOP_ID,
    .enablement_fn          = cpr_enable_init_fn_closed_loop,
};


#define CPR_DEFAULT_GCNT 19     //We always use 19 since we always run the CPR Ref Clk at 19.2MHz

/*===========================================================================
FUNCTION: cpr_closed_loop_rail_config

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static const cpr_closed_loop_rail_config_t* cpr_closed_loop_rail_config(cpr_rail_id_t rail_id)
{
    for(int i=0; i<cpr_bsp_closed_loop_config.closed_loop_config_count; i++)
    {
        const cpr_closed_loop_rail_config_t* rail_config = cpr_bsp_closed_loop_config.closed_loop_config[i];
        if(rail_id == rail_config->rail_id)
        {
            return rail_config;
        }
    }
    CORE_VERIFY(0); //Couldn't find the necessary config.
    return NULL;
}

/*===========================================================================
FUNCTION: cpr_find_corner_params

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
const cpr_corner_params_t* cpr_find_corner_params(cpr_closed_loop_rail_t rail, cpr_voltage_mode_t voltage_mode)
{
    for(int i=0; i<rail->target_params_count; i++)
    {
        const cpr_corner_params_t* corner_params = &rail->target_params[i];
        if(voltage_mode == corner_params->voltage_mode)
        {
            return corner_params;
        }
    }
    return NULL;
}

/*===========================================================================
FUNCTION: cpr_closed_loop_alloc_node

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static cpr_closed_loop_rail_t cpr_closed_loop_alloc_node(cpr_rail_id_t rail_id, uint32 voltage_level_count)
{
    cpr_closed_loop_rail_t closed_loop_rail = (cpr_closed_loop_rail_t)malloc(sizeof(cpr_closed_loop_rail_s));
    CORE_VERIFY(closed_loop_rail);

    closed_loop_rail->next_link = cpr_closed_loop_rail_root;
    cpr_closed_loop_rail_root = closed_loop_rail;
    
    closed_loop_rail->rail_id = rail_id;
    closed_loop_rail->rail_config = cpr_closed_loop_rail_config(rail_id);
    closed_loop_rail->voltage_control_handle = cpr_image_alloc_voltage_control_handle(rail_id);
    closed_loop_rail->image_rail_state = cpr_image_alloc_image_rail_state();
    
    closed_loop_rail->target_params_count = voltage_level_count;
    closed_loop_rail->target_params = (cpr_corner_params_t*)calloc(closed_loop_rail->target_params_count, sizeof(cpr_corner_params_t));
    CORE_VERIFY_PTR(closed_loop_rail->target_params);
    
    closed_loop_rail->pmic_step_size = cpr_config_pmic_step_size(rail_id);

    return closed_loop_rail;
}

/*===========================================================================
FUNCTION: cpr_closed_loop_find_node

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static cpr_closed_loop_rail_t cpr_closed_loop_find_node(cpr_rail_id_t rail_id)
{
    cpr_closed_loop_rail_t closed_loop_rail = cpr_closed_loop_rail_root;
    while(closed_loop_rail)
    {
        if(closed_loop_rail->rail_id == rail_id)
        {
            break;
        }
        closed_loop_rail = closed_loop_rail->next_link;
    }
    return closed_loop_rail;
}

/*===========================================================================
FUNCTION: cpr_closed_loop_init_default_voltage_levels

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void cpr_closed_loop_init_default_voltage_levels(cpr_closed_loop_rail_t closed_loop_rail,
                    const cpr_enablement_versioned_rail_config_t* rail_enablement_config)
{
    const cpr_rail_id_t rail_id = closed_loop_rail->rail_id;

    CPR_LOGGING(2, "cpr_closed_loop_init_default_voltage_levels: %s  Supported Levels: %d", cpr_logging_get_ascii_rail_id_name(rail_id), rail_enablement_config->supported_level_count);
    for(int i=0; i<rail_enablement_config->supported_level_count; i++)
    {
        const cpr_enablement_supported_level_t* level_info = &rail_enablement_config->supported_level[i];
        cpr_voltage_mode_t voltage_mode = level_info->voltage_mode;

        uint32 open_loop_voltage = cpr_config_open_loop_voltage(rail_id, voltage_mode);
        
        //Always start from the Open-Loop voltage.
        cpr_image_set_rail_mode_voltage(closed_loop_rail->voltage_control_handle, voltage_mode, open_loop_voltage);
    }
}

/*===========================================================================
FUNCTION: cpr_enable_init_fn_closed_loop

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void cpr_enable_init_fn_closed_loop(cpr_rail_id_t rail_id,
                                    const cpr_enablement_versioned_rail_config_t* rail_enablement_config,
                                    cpr_enablement_stage_t enablement_stage)
{
    int32 static_margin_mv;
    cpr_closed_loop_rail_t closed_loop_rail = cpr_closed_loop_find_node(rail_id);

    if(enablement_stage == CPR_ENABLEMENT_STAGE_INIT_INITAL_VOLTAGE_CONFIG)
    {
        //Sanity check that we've not already alloced a node.
        CORE_VERIFY(!closed_loop_rail);
        closed_loop_rail = cpr_closed_loop_alloc_node(rail_id, rail_enablement_config->supported_level_count);

        cpr_closed_loop_init_default_voltage_levels(closed_loop_rail, rail_enablement_config);
        return;
    }
    
    //Sanity check that we could find the rail node.
    CORE_VERIFY_PTR(closed_loop_rail);

    CPR_LOGGING(2, "cpr_enable_init_fn_closed_loop: %s  Supported Levels: %d", cpr_logging_get_ascii_rail_id_name(rail_id), rail_enablement_config->supported_level_count);
    //run CPR aging calculations
    cpr_aging_poll(closed_loop_rail);
    
    //Init target quotients, floor and ceiling voltages.
    for(int i=0; i<rail_enablement_config->supported_level_count; i++)
    {
        const cpr_enablement_supported_level_t* level_info = &rail_enablement_config->supported_level[i];
        cpr_voltage_mode_t voltage_mode = level_info->voltage_mode;
        closed_loop_rail->target_params[i].voltage_mode = voltage_mode;
        cpr_corner_params_t* corner_target_params = &closed_loop_rail->target_params[i];
        
        cpr_target_quotient_set_t* ro_targets = &corner_target_params->rosc_targets;        
        static_margin_mv = level_info->static_margin_mv;
        if (level_info->custom_static_margin_fn)
        {
            static_margin_mv = level_info->custom_static_margin_fn (rail_id, voltage_mode, static_margin_mv);
            CPR_LOGGING(3, "[%s] static margin was %d is now %d", cpr_logging_get_ascii_corner_name(voltage_mode), level_info->static_margin_mv, static_margin_mv);			
        }
        
        ro_targets->static_margin_mv = static_margin_mv;
        cpr_target_quotient_calculate_quotients(rail_id, voltage_mode, static_margin_mv, ro_targets);
        
        //only adjust for aging if the rail enables aging
        if(closed_loop_rail->rail_config->aging_sensor_count != 0)
        {
            cpr_target_quotient_apply_aging(rail_id, closed_loop_rail->aging_percentage, level_info->aging_scaling_factor, rail_enablement_config->aging_static_margin_limit, ro_targets, voltage_mode);
        }
        
        int32 effective_margin_mv = static_margin_mv + cpr_offset_fuse_val(rail_id, voltage_mode);
        cpr_target_quotient_calculate_quotients(rail_id, voltage_mode, effective_margin_mv, ro_targets);


        if(rail_enablement_config->custom_voltage_fn)
        {
            corner_target_params->voltage_ceil  = rail_enablement_config->custom_voltage_fn(CPR_OPEN_LOOP_VOLTAGE, voltage_mode);
            corner_target_params->voltage_floor = rail_enablement_config->custom_voltage_fn(CPR_CLOSED_LOOP_FLOOR_VOLTAGE, voltage_mode);
        }
        else
        {
            corner_target_params->voltage_ceil = cpr_config_open_loop_voltage(rail_id, voltage_mode);
            corner_target_params->voltage_floor = cpr_config_closed_loop_floor_voltage(rail_id, voltage_mode);
        }
    }
    
    //Now do what's remaining with the HW init.
#ifdef CPR_CONFIG_NO_FEEDBACK_LOOP

    if(rail_enablement_config->mode_to_settle_count>0)
    {
        cpr_init_rail_hw(closed_loop_rail);

        cpr_settle_voltage(closed_loop_rail, rail_enablement_config);
    }

#else

    cpr_init_rail_hw(closed_loop_rail);
    if(rail_enablement_config->mode_to_settle_count>0)
    {
        cpr_settle_voltage(closed_loop_rail, rail_enablement_config);
    }

    cpr_enable_closed_loop_feedback(closed_loop_rail);

#endif
}

/*===========================================================================
FUNCTION: cpr_closed_loop_init_default_voltages_from_smem

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void cpr_closed_loop_init_default_voltages_from_smem(cpr_closed_loop_rail_t closed_loop_rail, const cpr_externalized_state_closed_loop_level_t* closed_loop_levels, uint32 closed_loop_level_count)
{
    for(int i=0; i<closed_loop_level_count; i++)
    {
        const cpr_externalized_state_closed_loop_level_t* closed_loop_level = &closed_loop_levels[i];
        
        // Set settled voltage from XBL.
        cpr_image_set_rail_mode_voltage(closed_loop_rail->voltage_control_handle, 
                closed_loop_level->corner_params.voltage_mode, closed_loop_level->voltage);
    }
}

/*===========================================================================
FUNCTION: cpr_enable_init_fn_closed_loop_from_smem

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void cpr_enable_init_fn_closed_loop_from_smem(cpr_rail_id_t rail_id, const cpr_externalized_state_closed_loop_level_t* closed_loop_levels, 
        uint32 closed_loop_level_count, cpr_enablement_stage_t enablement_stage)
{
    cpr_closed_loop_rail_t closed_loop_rail = cpr_closed_loop_find_node(rail_id);
        
    if(enablement_stage == CPR_ENABLEMENT_STAGE_INIT_INITAL_VOLTAGE_CONFIG)
    {
        //Sanity check that we've not already alloced a node.
        CORE_VERIFY(!closed_loop_rail);
    
        closed_loop_rail = cpr_closed_loop_alloc_node(rail_id, closed_loop_level_count);

        CPR_LOGGING(2, "cpr_enable_init_fn_closed_loop_from_smem: %s  Supported Levels: %d", cpr_logging_get_ascii_rail_id_name(rail_id), closed_loop_levels);

        cpr_closed_loop_init_default_voltages_from_smem(closed_loop_rail, closed_loop_levels, closed_loop_level_count);
        return;
    }

    //Sanity check that we could find the rail node.
    CORE_VERIFY_PTR(closed_loop_rail);

    //Pull in target quotients, floors and ceilings from SMem.
    for(int i=0; i<closed_loop_level_count; i++)
    {
        const cpr_externalized_state_closed_loop_level_t* closed_loop_level = &closed_loop_levels[i];
        cpr_corner_params_t* corner_target_params = &closed_loop_rail->target_params[i];

        cpr_image_memscpy(corner_target_params, sizeof(cpr_corner_params_t), &closed_loop_level->corner_params, sizeof(cpr_corner_params_t));
    }
        
    cpr_init_rail_hw(closed_loop_rail);

    cpr_enable_closed_loop_feedback(closed_loop_rail);
}

/*===========================================================================
FUNCTION: rbcpr_prepare_for_sleep

DESCRIPTION: disables RBCPR clocks.

    FOR each rail that is enabled
        Disable CPR
        Turn off clocks

RETURN VALUE:
===========================================================================*/
void rbcpr_prepare_for_sleep(void)
{
    cpr_closed_loop_rail_t node = cpr_closed_loop_rail_root;
    
    while(node)
    {
        if(node->block_enabled)
        {
            cpr_pause_cpr(node);
        }
        node = node->next_link;
    }
}

/*===========================================================================
FUNCTION: rbcpr_exit_sleep

DESCRIPTION: enables RBCPR clocks.

    FOR each rail that was enabled
        Turn on clocks
        Enable CPR

RETURN VALUE:
===========================================================================*/
void rbcpr_exit_sleep(void)
{
    cpr_closed_loop_rail_t node = cpr_closed_loop_rail_root;
    
    while(node)
    {
        if(node->block_enabled)
        {
            uint32 current_voltage = cpr_image_get_rail_mode_voltage(node->voltage_control_handle, node->current_target_params->voltage_mode);
            cpr_prepare_cpr(node, node->current_target_params, current_voltage);
            cpr_restart_cpr(node);
        }
        node = node->next_link;
    }
}
