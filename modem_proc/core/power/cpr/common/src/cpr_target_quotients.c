/*===========================================================================

  Copyright (c) 2014. 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_voltage_ranges.h"
#include "cpr_target_quotients.h"
#include "CoreVerify.h"
#include "cpr_logging.h"
#include <string.h>


/*===========================================================================
FUNCTION: cpr_config_find_device_target_quotients

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static const cpr_target_quotient_versioned_config_t*
cpr_config_find_device_target_quotients(const cpr_target_quotient_rail_config_t* rail_target_quotient_configs)
{
    for(int i=0; i<rail_target_quotient_configs->versioned_target_quotient_config_count; i++)
    {
        const cpr_target_quotient_versioned_config_t* versioned_target_quotients =
                rail_target_quotient_configs->versioned_target_quotient_config[i];

        //To do - factor out this hw version matching to a seperate function
        const cpr_config_hw_version_range* hw_versions = &versioned_target_quotients->hw_versions;

        if(cpr_device_hw_version_matches_this_device(hw_versions))
        {
            return versioned_target_quotients;
        }
    }
    return NULL;
}

/*===========================================================================
FUNCTION: cpr_target_quotient_find_rail_config

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static const cpr_target_quotient_versioned_config_t*
cpr_target_quotient_find_rail_config(cpr_rail_id_t rail_id)
{
    for(int i=0; i<cpr_bsp_target_quotient_config.rail_config_count; i++)
    {
        const cpr_target_quotient_rail_config_t* rail_target_quotient_versions = cpr_bsp_target_quotient_config.rail_config[i];
        if(rail_target_quotient_versions->rail_id == rail_id)
        {
            return cpr_config_find_device_target_quotients(rail_target_quotient_versions);
        }
    }
    return NULL;
}

/*===========================================================================
FUNCTION: cpr_target_quotient_find_level

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static const cpr_target_quotient_level_t*
cpr_target_quotient_find_level(const cpr_target_quotient_versioned_config_t* target_quotient_config, cpr_voltage_mode_t voltage_mode)
{
    CORE_VERIFY_PTR(target_quotient_config);
    for(int i=0; i<target_quotient_config->target_quotient_level_count; i++)
    {
        const cpr_target_quotient_level_t* target_quotient_level = &target_quotient_config->target_quotient_level[i];
        if(target_quotient_level->voltage_mode==voltage_mode)
        {
            return target_quotient_level;
        }
    }
    return NULL;
}

/*===========================================================================
FUNCTION: cpr_target_quotients_handle_rail_info_not_found

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void cpr_target_quotients_handle_rail_info_not_found(cpr_target_quotient_set_t* target_quotients)
{
    int i;
    for (i = 0; i < CPR_ROSC_COUNT; i++){
        target_quotients->ro_target_quotient[i] = 0;
    }
}

/*===========================================================================
FUNCTION: cpr_target_quotient_calculate_quotients

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void cpr_target_quotient_calculate_quotients(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode, int32 static_margin_mv, cpr_target_quotient_set_t* target_quotients)
{
    const cpr_target_quotient_versioned_config_t* rail_target_quotient_config = cpr_target_quotient_find_rail_config(rail_id);
    if(!rail_target_quotient_config)
    {
        cpr_target_quotients_handle_rail_info_not_found(target_quotients);
        return;
    }
    
    const cpr_target_quotient_level_t* target_quotent_level_config = cpr_target_quotient_find_level(rail_target_quotient_config, voltage_mode);
    if(!target_quotent_level_config)
    {
        cpr_target_quotients_handle_rail_info_not_found(target_quotients);
        return;
    }
    
    for(int i=0; i<CPR_ROSC_COUNT; i++)
    {
        // Handle the case that the ro is masked off, i.e. it equals 0.
        if(!target_quotent_level_config->ro_base_quotient[i])
        {
            target_quotients->ro_target_quotient[i] = 0;
            continue;
        }
        
        // Round up the target quotients to the next integer.
        // Target_Quotient = Base_Quotient + (((static_margin * ro_kv_x_100) + 99)/100)

        target_quotients->ro_target_quotient[i] = target_quotent_level_config->ro_base_quotient[i] + 
                                                        (((static_margin_mv * rail_target_quotient_config->ro_kv_x_100[i])+99)/100);
    }
}

void cpr_target_quotient_apply_aging(cpr_rail_id_t rail_id, int32 aging_percentage, int32 aging_scaling_factor, uint32 aging_static_margin_limit, cpr_target_quotient_set_t* target_quotients, cpr_voltage_mode_t voltage_mode)
{
    int32 voltage_aging_percent;
    uint32  aging_adjusted_target_quotient[CPR_ROSC_COUNT];
    const cpr_target_quotient_versioned_config_t* rail_target_quotient_config = cpr_target_quotient_find_rail_config(rail_id);
	
    if(!rail_target_quotient_config)
    {
        cpr_target_quotients_handle_rail_info_not_found(target_quotients);
        return;
    }

    // multiply the aging percentage with the aging factor
    voltage_aging_percent = aging_percentage * aging_scaling_factor;
	
    //update the stats log.
    cpr_logging_update_aging_stats(aging_percentage, aging_static_margin_limit);

    //log the aging details being applied     
    CPR_LOGGING(4, "[%s] voltage aging percent is %d based on %d age percent and %d scaling factor", cpr_logging_get_ascii_corner_name(voltage_mode), voltage_aging_percent, aging_percentage, aging_scaling_factor);

    for(int i=0; i<CPR_ROSC_COUNT; i++)
    {
        //subtract aging limit from original target quotients
        aging_adjusted_target_quotient[i] = (target_quotients->ro_target_quotient[i] - ((aging_static_margin_limit * rail_target_quotient_config->ro_kv_x_100[i])+99)/100);
        //adding aging into effect
        aging_adjusted_target_quotient[i] = (aging_adjusted_target_quotient[i] * (100 + voltage_aging_percent))/100; 
        
        // adjust the target quot if the aging adjusted quot is smaller
        if(aging_adjusted_target_quotient[i] < target_quotients->ro_target_quotient[i])
        {
            target_quotients->ro_target_quotient[i] = aging_adjusted_target_quotient[i]; 
        }
    }    
}

