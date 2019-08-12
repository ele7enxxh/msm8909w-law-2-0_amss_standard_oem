/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_fuses.h"
#include "cpr_voltage_ranges.h"
#include "cpr_target_quotients.h"
#include "cpr_enablement.h"
#include "CoreVerify.h"
#include "cpr_image.h"
#include "cpr_open_loop.h"
#include "cpr_logging.h"
#include <string.h>
#include "cpr_smem.h"
#include <stdlib.h>

cpr_open_loop_rail_t cpr_open_loop_rail_root = NULL;
void cpr_enable_init_fn_open_loop(cpr_rail_id_t rail_id,
                                         const cpr_enablement_versioned_rail_config_t* rail_enablement_config,
                                         cpr_enablement_stage_t enablement_stage);
static void
cpr_enable_init_fn_global_ceiling_voltage(cpr_rail_id_t rail_id,
                                          const cpr_enablement_versioned_rail_config_t* rail_enablement_config,
                                          cpr_enablement_stage_t enablement_stage);


const cpr_enablement_init_params_t CPR_ENABLE_OPEN_LOOP =
{
    .enablement_type        = CPR_ENABLEMENT_OPEN_LOOP_ID,
    .enablement_fn          = cpr_enable_init_fn_open_loop,
};

const cpr_enablement_init_params_t CPR_ENABLE_GLOBAL_CEILING_VOLTAGE =
{
    .enablement_type        = CPR_ENABLEMENT_GLOBAL_CEILING_VOLTAGE_ID,
    .enablement_fn          = cpr_enable_init_fn_global_ceiling_voltage,
};

/*===========================================================================
FUNCTION: cpr_open_loop_alloc_node

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static cpr_open_loop_rail_t cpr_open_loop_alloc_node(cpr_rail_id_t rail_id, const cpr_enablement_versioned_rail_config_t* rail_enablement_config)
{
    cpr_open_loop_rail_t open_loop_rail = (cpr_open_loop_rail_t)malloc(sizeof(cpr_open_loop_rail_s));
    if (open_loop_rail == NULL){
        //malloc failed.  Can't continue
        CORE_VERIFY(0);
    }

    memset(open_loop_rail, 0, sizeof(cpr_open_loop_rail_s));
    
    open_loop_rail->rail_id = rail_id;
    open_loop_rail->voltage_control_handle = cpr_image_alloc_voltage_control_handle(rail_id);
    open_loop_rail->enablement_config = rail_enablement_config;
    open_loop_rail->next_link = cpr_open_loop_rail_root;
    cpr_open_loop_rail_root = open_loop_rail;
    
    return open_loop_rail;
}

/*===========================================================================
FUNCTION: cpr_enable_init_fn_open_loop

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void cpr_enable_init_fn_open_loop(cpr_rail_id_t rail_id,
                                         const cpr_enablement_versioned_rail_config_t* rail_enablement_config,
                                         cpr_enablement_stage_t enablement_stage)
{
    cpr_open_loop_rail_t open_loop_rail;

    //For Open-Loop, we do everything in the first stage of CPR init.
    if(enablement_stage != CPR_ENABLEMENT_STAGE_INIT_INITAL_VOLTAGE_CONFIG)
        return;

    open_loop_rail = cpr_open_loop_alloc_node(rail_id, rail_enablement_config);

    CPR_LOGGING(2, "cpr_enable_init_fn_open_loop: %s  Supported Levels: %d", cpr_logging_get_ascii_rail_id_name(rail_id), rail_enablement_config->supported_level_count);

    for(int i=0; i<rail_enablement_config->supported_level_count; i++)
    {
        uint32 open_loop_voltage;
        cpr_voltage_mode_t voltage_mode = rail_enablement_config->supported_level[i].voltage_mode;

        if(rail_enablement_config->custom_voltage_fn)
        {
            open_loop_voltage = rail_enablement_config->custom_voltage_fn(CPR_OPEN_LOOP_VOLTAGE, voltage_mode);
        }
        else
        {
            open_loop_voltage = cpr_config_open_loop_voltage(rail_id, voltage_mode);
        }

        cpr_image_set_rail_mode_voltage(open_loop_rail->voltage_control_handle, voltage_mode, open_loop_voltage);
    }
}

/*===========================================================================
FUNCTION: cpr_enable_init_fn_global_ceiling_voltage

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static void
cpr_enable_init_fn_global_ceiling_voltage(cpr_rail_id_t rail_id,
                                          const cpr_enablement_versioned_rail_config_t* rail_enablement_config,
                                          cpr_enablement_stage_t enablement_stage)
{
    cpr_open_loop_rail_t open_loop_rail;

    //For Open-Loop, we do everything in the first stage of CPR init.
    if(enablement_stage != CPR_ENABLEMENT_STAGE_INIT_INITAL_VOLTAGE_CONFIG)
        return;

    open_loop_rail = cpr_open_loop_alloc_node(rail_id, rail_enablement_config);

    CPR_LOGGING(2, "cpr_enable_init_fn_global_ceiling_voltage: %s  Supported Levels: %d", cpr_logging_get_ascii_rail_id_name(rail_id), rail_enablement_config->supported_level_count);

    for(int i=0; i<rail_enablement_config->supported_level_count; i++)
    {
        uint32 global_ceiling_voltage;
        cpr_voltage_mode_t voltage_mode = rail_enablement_config->supported_level[i].voltage_mode;

        global_ceiling_voltage = cpr_config_global_ceiling_voltage(rail_id, voltage_mode);
        
        cpr_image_set_rail_mode_voltage(open_loop_rail->voltage_control_handle, voltage_mode, global_ceiling_voltage);
    }
}

/*===========================================================================
FUNCTION: cpr_enable_init_fn_open_loop_from_smem

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void cpr_enable_init_fn_open_loop_from_smem(cpr_rail_id_t rail_id,
                                            const cpr_externalized_state_open_loop_level_t* open_loop_levels,
                                            uint32 open_loop_level_count,
                                            cpr_enablement_stage_t enablement_stage)
{
    cpr_open_loop_rail_t open_loop_rail;

    //For Open-Loop, we do everything in the first stage of CPR init.
    if(enablement_stage != CPR_ENABLEMENT_STAGE_INIT_INITAL_VOLTAGE_CONFIG)
        return;

    open_loop_rail = cpr_open_loop_alloc_node(rail_id, NULL);

    for(int i=0; i<open_loop_level_count; i++)
    {
        uint32 open_loop_voltage = open_loop_levels[i].voltage;
        cpr_voltage_mode_t voltage_mode = open_loop_levels[i].voltage_mode;

        cpr_image_set_rail_mode_voltage(open_loop_rail->voltage_control_handle, voltage_mode, open_loop_voltage);
    }
}

