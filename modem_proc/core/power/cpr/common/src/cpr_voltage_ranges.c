/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_voltage_ranges.h"
#include "cpr_logging.h"
#include "CoreVerify.h"

#define CPR_CONFIG_EFUSE_MULTIPLIER 12500   //12.5mV
#define CPR_FUSED_OFFSET_MULTIPLIER 10000   //10mV
#define CPR_CLOSED_LOOP_FUSED_FLOOR_ENABLED //Only disable for testing

static const cpr_config_versioned_voltage_ranges_t* cpr_config_find_device_ranges(const cpr_config_rail_voltage_ranges_t* rail_voltage_ranges)
{
    for(int i=0; i<rail_voltage_ranges->versioned_voltage_range_count; i++)
    {
        const cpr_config_versioned_voltage_ranges_t* versioned_voltage_ranges = rail_voltage_ranges->versioned_voltage_range[i];
        const cpr_config_hw_version_range* hw_versions = &versioned_voltage_ranges->hw_versions;

        if(cpr_device_hw_version_matches_this_device(hw_versions))
        {
                return versioned_voltage_ranges;
        }
    }   
    return NULL;
}

const static cpr_config_rail_voltage_ranges_t* cpr_config_find_rail_voltage_ranges(cpr_rail_id_t rail_id)
{
    for(int i=0; i<cpr_bsp_voltage_ranges_config.rail_voltage_ranges_count; i++)
    {
        const cpr_config_rail_voltage_ranges_t* rail_voltage_ranges = cpr_bsp_voltage_ranges_config.rail_voltage_ranges[i];
        
        if(rail_voltage_ranges->rail_id==rail_id)
        {
            return rail_voltage_ranges;
        }
    }

    return NULL;
}

const cpr_config_versioned_voltage_ranges_t* cpr_config_find_versioned_voltage_ranges(cpr_rail_id_t rail_id)
{
    const cpr_config_rail_voltage_ranges_t* rail_voltage_ranges = cpr_config_find_rail_voltage_ranges(rail_id);

    if(!rail_voltage_ranges)
        return NULL;
    
    return cpr_config_find_device_ranges(rail_voltage_ranges);
}

const cpr_config_voltage_level_t* cpr_config_find_voltage_level(const cpr_config_versioned_voltage_ranges_t* voltage_ranges, cpr_voltage_mode_t voltage_mode)
{
    CORE_VERIFY_PTR(voltage_ranges);
    
    for(int i=0; i<voltage_ranges->voltage_level_count; i++)
    {
        const cpr_config_voltage_level_t* voltage_level = &voltage_ranges->voltage_level[i];
        
        if(voltage_mode==voltage_level->voltage_mode)
            return voltage_level;
    }
    return NULL;
}

uint32 cpr_config_pmic_step_size(cpr_rail_id_t rail_id)
{
    for(int i=0; i<cpr_bsp_misc_info_config.rail_info_count; i++)
    {
        if(rail_id == cpr_bsp_misc_info_config.rail_info[i].rail_id)
        {
            return cpr_bsp_misc_info_config.rail_info[i].pmic_step_size;
        }
    }

    //If we get here we couldn't find a matching rail. Abort.
    CORE_VERIFY(0);
    return 0;
}

uint32 cpr_config_round_up_to_pmic_step(cpr_rail_id_t rail_id, uint32 voltage)
{
    uint32 pmic_step_size = cpr_config_pmic_step_size(rail_id);
    if(voltage%pmic_step_size)
    {
        voltage += (pmic_step_size - (voltage%pmic_step_size));
    }
    return voltage;
}

uint32 cpr_config_open_loop_voltage(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode)
{
    if(voltage_mode == CPR_VOLTAGE_MODE_OFF)
    {
        return 0;
    }

    //CPR_LOGGING(2, "cpr_config_open_loop_voltage rail %s voltage mode: %s", cpr_logging_get_ascii_rail_id_name(rail_id), cpr_logging_get_ascii_corner_name(voltage_mode));

    const cpr_config_versioned_voltage_ranges_t* voltage_ranges = cpr_config_find_versioned_voltage_ranges(rail_id);
    CORE_VERIFY_PTR(voltage_ranges);
    
    const cpr_config_voltage_level_t* voltage_level = cpr_config_find_voltage_level(voltage_ranges, voltage_mode);
    CORE_VERIFY_PTR(voltage_level);
    
    const int32 voltage_offset_steps =  cpr_fuses_get_fuse_value(rail_id, voltage_level->volt_fuse_type);
    const int32 voltage_offset = CPR_CONFIG_EFUSE_MULTIPLIER * voltage_offset_steps;
    
    uint32 open_loop_voltage = voltage_offset + voltage_level->voltage_fuse_ref;

    //Now clip the Open-Loop voltage to the floor/ceiling limits.
    open_loop_voltage = MIN(voltage_level->voltage_ceiling, MAX(open_loop_voltage, voltage_level->voltage_floor));

    //Now round up to PMIC step size.
    open_loop_voltage = cpr_config_round_up_to_pmic_step(rail_id, open_loop_voltage);


    //make sure the voltage is lower than the ceiling, and higher than the floor.
    if (open_loop_voltage > voltage_level->voltage_ceiling){
        open_loop_voltage = voltage_level->voltage_ceiling;
    }
    if (open_loop_voltage < voltage_level->voltage_floor){
        open_loop_voltage = voltage_level->voltage_floor;
    }

    
    CORE_VERIFY(open_loop_voltage<=voltage_level->voltage_ceiling);

    //CPR_LOGGING(4, "Calculated open loop voltage (rail: %s) (mode: %s) (offset_steps: %d) (voltage: %d)", cpr_logging_get_ascii_rail_id_name(rail_id), cpr_logging_get_ascii_corner_name(voltage_mode), voltage_offset_steps, open_loop_voltage);

    return open_loop_voltage;
}

uint32 cpr_config_global_ceiling_voltage(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode)
{
    if(voltage_mode == CPR_VOLTAGE_MODE_OFF)
    {
        return 0;
    }

    const cpr_config_versioned_voltage_ranges_t* voltage_ranges = cpr_config_find_versioned_voltage_ranges(rail_id);
    CORE_VERIFY_PTR(voltage_ranges);
    
    const cpr_config_voltage_level_t* voltage_level = cpr_config_find_voltage_level(voltage_ranges, voltage_mode);
    CORE_VERIFY_PTR(voltage_level);
    
    uint32 global_ceiling_voltage = voltage_level->voltage_ceiling;

    //Verify ceiling is PMIC step size
    uint32 pmic_step_size = cpr_config_pmic_step_size(rail_id);
    CORE_VERIFY(global_ceiling_voltage%pmic_step_size==0);

    return global_ceiling_voltage;
}


int32 cpr_offset_fuse_val(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode)
{
    const cpr_config_rail_voltage_ranges_t* rail_voltage_ranges = cpr_config_find_rail_voltage_ranges(rail_id);
    CORE_VERIFY_PTR(rail_voltage_ranges);

    const cpr_config_versioned_voltage_ranges_t* voltage_ranges = cpr_config_find_device_ranges(rail_voltage_ranges);
    CORE_VERIFY_PTR(voltage_ranges);

    const cpr_config_voltage_level_t* voltage_level = cpr_config_find_voltage_level(voltage_ranges, voltage_mode);
    CORE_VERIFY_PTR(voltage_level);

    int32 quotient_offset_fuse_val = cpr_fuses_get_fuse_value(rail_id, voltage_level->offset_fuse_type) * CPR_FUSED_OFFSET_MULTIPLIER;
	
	//convert offset fuse value from uv to mv
	quotient_offset_fuse_val = quotient_offset_fuse_val / 1000;
	
    return quotient_offset_fuse_val;
}
uint32 cpr_config_closed_loop_floor_voltage(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode)
{
    if(voltage_mode == CPR_VOLTAGE_MODE_OFF)
    {
        return 0;
    }

    const cpr_config_rail_voltage_ranges_t* rail_voltage_ranges = cpr_config_find_rail_voltage_ranges(rail_id);
    CORE_VERIFY_PTR(rail_voltage_ranges);

    const cpr_config_versioned_voltage_ranges_t* voltage_ranges = cpr_config_find_device_ranges(rail_voltage_ranges);
    CORE_VERIFY_PTR(voltage_ranges);
    
    const cpr_config_voltage_level_t* voltage_level = cpr_config_find_voltage_level(voltage_ranges, voltage_mode);
    CORE_VERIFY_PTR(voltage_level);
    
    // Check if fused floor is disabled
    if( rail_voltage_ranges->disable_fused_floor == true )
    {
        return voltage_level->voltage_floor;
    }
    
    // Compute the fused floor
    const int32 voltage_offset_steps =  cpr_fuses_get_fuse_value(rail_id, voltage_level->volt_fuse_type);
    const int32 voltage_offset = CPR_CONFIG_EFUSE_MULTIPLIER * voltage_offset_steps;
    uint32 closed_loop_floor_voltage;

	if(voltage_level->voltage_max_floor_to_ceiling != 0) {
	   // Use max-floor-ceil
           // Offset fuse is added in HW by PTE to the open loop ceiling. If offset fuse is reverted, we need to add the offset back to the ceiling in SW.
        uint32 open_loop_voltage = voltage_offset + voltage_level->voltage_fuse_ref;
        closed_loop_floor_voltage = open_loop_voltage - voltage_level->voltage_max_floor_to_ceiling;
    }
    else
    {
    // calculate the safe closed loop floor voltage
        closed_loop_floor_voltage = ((voltage_level->voltage_fuse_ref * 90) / 100) + voltage_offset + 40000 + voltage_level->fuse_correction_factor;
    }

    //Now clip the Closed loop floor voltage to the floor/ceiling limits.
    closed_loop_floor_voltage = MIN(voltage_level->voltage_ceiling, MAX(closed_loop_floor_voltage, voltage_level->voltage_floor));
    
    //Now round up to PMIC step size.
    closed_loop_floor_voltage = cpr_config_round_up_to_pmic_step(rail_id, closed_loop_floor_voltage);
    
    //Verify the fused floor is below the ceiling and above the floor.
    CORE_VERIFY(closed_loop_floor_voltage>=voltage_level->voltage_floor);
    CORE_VERIFY(closed_loop_floor_voltage<=cpr_config_open_loop_voltage(rail_id, voltage_mode));
    
#ifdef CPR_CLOSED_LOOP_FUSED_FLOOR_ENABLED
    return closed_loop_floor_voltage;
#else
    //return the default floor for testing
    return voltage_level->voltage_floor;
#endif  //CPR_CLOSED_LOOP_FLOOR_ENABLED
}
