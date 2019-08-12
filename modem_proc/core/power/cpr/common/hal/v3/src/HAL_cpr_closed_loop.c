/*===========================================================================

  Copyright (c) 2014, 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_closed_loop.h"
#include "CoreVerify.h"
#include <string.h>
#include "cpr_image.h"
#include "cpr_enablement.h"
#include "cpr_voltage_ranges.h"
#include <stdlib.h>
#include "cpr_smem.h"
#include "cpr_logging.h"
#ifndef WINSIM
#include "stringl/stringl.h"
#endif
#include "HAL_cpr_v3.h"

//#define ENABLE_CLOSED_LOOP_LOGGING
#define CPR_DEFAULT_GCNT 19     //We always use 19 since we always run the CPR Ref Clk at 19.2MHz

/*===========================================================================
FUNCTION: cpr_configure_roscs

DESCRIPTION:
// Configure the CPR block for a particular 'corner'
// 1. Program all the ring oscillators with GCNT / TARGET
// 2. Program the step quot for the CPR block

RETURN VALUE:
===========================================================================*/
void cpr_configure_roscs(cpr_closed_loop_rail_t rail, const cpr_corner_params_t* corner_params)
{
#ifdef ENABLE_CLOSED_LOOP_LOGGING
    CPR_LOGGING(2, "cpr_configure_roscs: %s for %s", cpr_logging_get_ascii_rail_id_name(rail->rail_id), cpr_logging_get_ascii_corner_name(corner_params->voltage_mode));
#endif
    //Program all the roscs
    for (int rosc = 0; rosc < CPR_ROSC_COUNT; rosc++)
    {
        HAL_cpr_prgm_gcnt_target(&rail->rail_config->hal_handle, (HAL_cpr_rosc_type)rosc, CPR_DEFAULT_GCNT, 
                                 corner_params->rosc_targets.ro_target_quotient[rosc]);
    }
}

void cpr_pause_cpr(cpr_closed_loop_rail_t rail)
{
    const cpr_closed_loop_rail_config_t* rail_config = rail->rail_config;
    const HAL_cpr_rail_s* hal_handle = &rail_config->hal_handle;
#ifdef ENABLE_CLOSED_LOOP_LOGGING
    CPR_LOGGING(1, "cpr_pause_cpr: %s", cpr_logging_get_ascii_rail_id_name(rail->rail_id));
#endif

    // Set RBIF_IRQ_EN to 0
    HAL_cpr_enable_int(hal_handle, HAL_CPR_INT_UP_FLAG|HAL_CPR_INT_DOWN_FLAG, FALSE);
    
    // Set LOOP_EN to 0
    HAL_cpr_enable_block(hal_handle, FALSE);

    //Clear any interrupts that might already be pending
    HAL_cpr_clear_int_all(hal_handle);
    cpr_image_clear_interrupt_at_interrupt_controller(rail_config->interrupt_id);
        
    //In case the ISR has already triggered and scheduled the CPR task,
    //need to flag the task as not needing to call rbcpr_handler
    cpr_image_cancel_task(rail);
}

// This function does all the common init functionality between settling a rail and preparing full closed-loop CPR
void cpr_init_rail_hw(cpr_closed_loop_rail_t rail)
{
    const cpr_closed_loop_rail_config_t* rail_config = rail->rail_config;
    const HAL_cpr_rail_s* hal_handle = &rail_config->hal_handle;

#ifdef ENABLE_CLOSED_LOOP_LOGGING
    CPR_LOGGING(1, "cpr_init_rail_hw: %s", cpr_logging_get_ascii_rail_id_name(rail->rail_id));
#endif
    // Enable the clocks and bring CPR out of reset
    cpr_image_enable_clocks(rail);
    
    // hacks until the driver can be refactored
    if(!rail_config->controller_config->initialized)
    {
        HAL_cpr_configure_hw_defaults( hal_handle );
        HAL_cpr_write_stepquot( hal_handle, rail_config->step_quot );
        HAL_cpr_set_count_mode(hal_handle, HAL_CPR_ALL_AT_ONCE);
        
        for(int i = 0; i < rail_config->controller_config->thread_count; i++) {
            CPR_HWIO_OUT_SET( HWIO_CPR_MASK_THREAD_t_ADDR( hal_handle->hw_base_address, i ), HWIO_CPR_MASK_THREAD_t_DISABLE_THREAD_BMSK );
        }

        for(int i = 0; i < rail_config->controller_config->sensor_count; i++) {
            CPR_HWIO_OUT( HWIO_CPR_SENSOR_THREAD_n_ADDR( hal_handle->hw_base_address, i ), rail_config->controller_config->thread_count - 1 );
        }
        
        if(rail_config->controller_config->sensors_to_bypass != NULL)
        {
            for(int i=0; i < rail_config->controller_config->sensors_to_bypass_count; i++)
            {
                HAL_cpr_bypass_sensor(hal_handle, rail_config->controller_config->sensors_to_bypass[i], TRUE);
            }
        }
        
        rail_config->controller_config->initialized = TRUE;
    }
    
    if(rail_config->sensors_to_mask != NULL)
    {
        for(int i=0; i < rail_config->sensors_to_mask_count; i++)
        {
            HAL_cpr_assign_sensor(hal_handle, rail_config->sensors_to_mask[i]);
        }
    }
    
    HAL_cpr_write_up_threshold(hal_handle, rail_config->up_threshold);
    HAL_cpr_write_dn_threshold(hal_handle, rail_config->dn_threshold);
    HAL_cpr_write_consecutive_up(hal_handle, rail_config->consecutive_up);
    HAL_cpr_write_consecutive_dn(hal_handle, rail_config->consecutive_dn);

    HAL_cpr_set_clamp_timer_interval(hal_handle, 1);
}

//This function completes init of the CPR HW and configures the HW to start generating interrupts.
void cpr_enable_closed_loop_feedback(cpr_closed_loop_rail_t rail)
{
    const cpr_closed_loop_rail_config_t* rail_config = rail->rail_config;
    const HAL_cpr_rail_s* hal_handle = &rail_config->hal_handle;
#ifdef ENABLE_CLOSED_LOOP_LOGGING
    CPR_LOGGING(0, "cpr_enable_closed_loop_feedback");
#endif

    /* Clear out any pending interrupts in the HW */
    HAL_cpr_clear_int_all(hal_handle);

    cpr_image_init_image_closed_loop(rail);

    //Store the initial settings.
    rail->current_target_params = cpr_image_get_current_corner_params(rail);
    
    if(rail->current_target_params->voltage_mode!=CPR_VOLTAGE_MODE_OFF)
    {
#ifdef ENABLE_CLOSED_LOOP_LOGGING    	
        CPR_LOGGING(0, "Set up roscs and ints");
#endif		
        /* Now configure the RBCPR system with the GCNT, TARGET values for the information from the fuses */
        cpr_configure_roscs(rail, rail->current_target_params);

        // Now enable interrupts
        HAL_cpr_enable_int(hal_handle, HAL_CPR_INT_DOWN_FLAG, TRUE);

        //Only enable the up interrupt if we're not on the ceiling.    
        if(rail->current_target_params->voltage_ceil<=cpr_image_get_rail_mode_voltage(rail->voltage_control_handle, rail->current_target_params->voltage_mode))
        {
            HAL_cpr_enable_int(hal_handle, HAL_CPR_INT_UP_FLAG, FALSE);
        }
        else
        {
            CPR_LOGGING(0, "cpr_enable_closed_loop_feedback - At the ceiling, UP interrupts will be disabled");
            HAL_cpr_enable_int(hal_handle, HAL_CPR_INT_UP_FLAG, TRUE);
        }

        // Enable the block
        HAL_cpr_enable_block(hal_handle, TRUE);

        rail->block_enabled = TRUE;
    } else {
#ifdef ENABLE_CLOSED_LOOP_LOGGING
       CPR_LOGGING(0, "Rail is off.  Skipping some setup.");
#endif
    }
}

/* From CPR HPG

To disable the CPR for switching mode. Steps MUST be done in Order:

1-	Set SW_AUTO_CONT_NACK_DN_EN to FALSE
2-	Set SW_AUTO_CONT_ACK_EN to FALSE
3-	Set CONSECUTIVE_DN to 0 and CONSECUTIVE_UP to 0
4-	Set RBIF_IRQ_EN to 0
5-	Set RBIF_IRQ_CLEAR to 0x7F (for resetting up and down counters)
6-	Send RBIF_CONT_ACK_CMD
7-	Send RBIF_CONT_NACK_CMD
8-	Set LOOP_EN to 0

From CPR HPG...

After disabling CPR – set parameters for new mode – don’t need to be in order

Reconfigure the target quotients for the new mode
Clear any potentially pending interrupts on the CPR block again (RBIF_IRQ_CLEAR = 0x7F)
Clear any potentially pending interrupts at the NVIC
Set CONSECUTIVE_DN and CONSECUTIVE_UP to proper values based on SoC
Set SW_AUTO_CONT_NACK_DN_EN and SW_AUTO_CONT_ACK_EN based on SoC
Set RBIF_IRQ_EN for proper interrupts – usually step_up, and step_dn only
Other settings that may be required for new mode
*/


//This function does everything required to prepare CPR for the current voltage settings.
//These parts are required also when processing a CPR interrupt as well as when re-enabling the CPR
//after vdd-min or a mode switch.
void cpr_prepare_cpr(cpr_closed_loop_rail_t rail, const cpr_corner_params_t* corner_params, uint32 rail_uv)
{
    const cpr_closed_loop_rail_config_t* rail_config = rail->rail_config;
    const HAL_cpr_rail_s* hal_handle = &rail_config->hal_handle;
#ifdef ENABLE_CLOSED_LOOP_LOGGING    
    CPR_LOGGING(1, "cpr_prepare_cpr corner at %d", rail_uv);
#endif
    //We need to enable the down interrupt if it's dissabled and we're not on the voltage floor
    HAL_cpr_enable_int(hal_handle, HAL_CPR_INT_DOWN_FLAG, rail_uv != corner_params->voltage_floor);    
    
    //Now disable the up interrupt if we're on the ceiling
    HAL_cpr_enable_int(hal_handle, HAL_CPR_INT_UP_FLAG, rail_uv != corner_params->voltage_ceil);
}

//This function does the parts of re-enabling CPR required only after vdd-min or a mode switch.
//It is not required in the case that we're handling a CPR interrupt.
void cpr_restart_cpr(cpr_closed_loop_rail_t rail)
{
    const cpr_closed_loop_rail_config_t* rail_config = rail->rail_config;
    const HAL_cpr_rail_s* hal_handle = &rail_config->hal_handle;
#ifdef ENABLE_CLOSED_LOOP_LOGGING
    CPR_LOGGING(0, "cpr_restart_cpr");
#endif

    HAL_cpr_clear_int_all(hal_handle);
    cpr_image_clear_interrupt_at_interrupt_controller(rail_config->interrupt_id);

    HAL_cpr_write_consecutive_up(hal_handle, rail_config->consecutive_up);
    HAL_cpr_write_consecutive_dn(hal_handle, rail_config->consecutive_dn);

    /* Enable the system */
    HAL_cpr_enable_block(hal_handle, TRUE);
}

/*===========================================================================
FUNCTION: rbcpr_pre_switch_callback

DESCRIPTION: Callback to be executed before a voltage switch

RETURN VALUE:
===========================================================================*/
void cpr_pre_mode_switch_disable(cpr_closed_loop_rail_t rail, const cpr_corner_params_t* corner_params)
{
    const cpr_corner_params_t* previous_target_params = rail->current_target_params;
    rail->current_target_params = corner_params;
#ifdef ENABLE_CLOSED_LOOP_LOGGING
    CPR_LOGGING(0, "cpr_pre_mode_switch_disable");
#endif
    //We need to pause CPR if we're not handlind a CPR interrupt or
    //if the rail is being turned off as part of the CPR interrupt handling
    //
    //The latter would happen if Railway re-aggregated and decided that the rail should be off
    //as part of railway_transition_voltages
    if (!rail->cpr_handler_in_progress || corner_params->voltage_mode==CPR_VOLTAGE_MODE_OFF)
    {
        //Check the block is enabled. It could be that we're going from off -> on, in which
        //case we'd have paused the block when we went to off in the first place.
        //Don't want to try to pause it twice.
        if(rail->block_enabled)
        {
            cpr_pause_cpr(rail);

            /* Update the state of the block */
            rail->block_enabled = FALSE;
        }
    }
    
    //This implies we're moving corners. But don't try to program RO targets if the rail is turning off.
    if (corner_params->voltage_mode!=CPR_VOLTAGE_MODE_OFF)
    {
        // Set up the oscillators for the new level
        cpr_configure_roscs(rail, corner_params);
    }
}

/*===========================================================================
FUNCTION: rbcpr_post_switch_callback

DESCRIPTION:
// Callback to be executed after a voltage switch

RETURN VALUE:
===========================================================================*/
void cpr_post_mode_switch_reenable(cpr_closed_loop_rail_t rail, const cpr_corner_params_t* corner_params, uint32 rail_uv)
{
#ifdef ENABLE_CLOSED_LOOP_LOGGING
    CPR_LOGGING(0, "cpr_post_mode_switch_reenable");
#endif
    //Assert that the settings we saved away in the pre-switch callback are unchanged here.
    CORE_VERIFY(corner_params==rail->current_target_params);

    // Increment our debug counter
    rail->rail_log.post_switch_counter++;

    if(corner_params->voltage_mode==CPR_VOLTAGE_MODE_OFF)
        return;
    
    cpr_prepare_cpr(rail, corner_params, rail_uv);

    // If it was not CPR triggered, the pre-switch should have disabled the block
    // We re-enable it here
    if (!rail->cpr_handler_in_progress)
    {
        //Block should be disabled if we weren't handling a CPR interrupt.
        CORE_VERIFY(!rail->block_enabled);
        
        cpr_restart_cpr(rail);

        /* Update the state of the block */
        rail->block_enabled = TRUE;
    }
}

uint32                            interrupt_status;
/*===========================================================================
FUNCTION: cpr_handler

DESCRIPTION: CPR interrupt handling function

RETURN VALUE:
===========================================================================*/
void cpr_handler(cpr_closed_loop_rail_t rail)
{
    uint32                            recommendation, current_voltage;
    int32                             measured_offset;
    bool                              ack_result = FALSE;
    const cpr_corner_params_t*        interrupted_settings = rail->current_target_params;

#ifdef ENABLE_CLOSED_LOOP_LOGGING
    CPR_LOGGING(0, "cpr_handler");
#endif
    rail->cpr_handler_in_progress = true;

    const cpr_closed_loop_rail_config_t* rail_config = rail->rail_config;
    const HAL_cpr_rail_s* hal_handle = &rail_config->hal_handle;
    
    interrupt_status = HAL_cpr_interrupt_status(hal_handle);
    
    /* Clear the interrupts */
    HAL_cpr_clear_int_all(hal_handle);

    //rbcpr_stats_increment_isr_counter(rail->stats_rail_id, rbcpr_corner_index(rail, interrupted_corner));

    // Find the safe voltage limits
    const uint32 safe_floor = interrupted_settings->voltage_floor;
    const uint32 safe_ceil  = interrupted_settings->voltage_ceil;

    current_voltage = cpr_image_get_rail_mode_voltage(rail->voltage_control_handle, interrupted_settings->voltage_mode);

    uint32 steps = 1;
    //uint32 steps = CPR_HWIO_IN_FIELD( HWIO_CPR_RESULT0_t_ADDR( hal_handle->hw_base_address, hal_handle->thread ), HWIO_CPR_RESULT0_t_ERROR_STEPS );
    
    /* Now compute the offset */
    if(interrupt_status&HAL_CPR_INT_UP_FLAG)
    {
        //It's possible that both the up and dn interrupts are set.
        //In that case, the recommendation from the HW team is that we step-up the voltage.
        //This could happen if a BHS is toggled after the first interrupt triggers.
        measured_offset = steps;
        rail->rail_log.step_up_count+=steps;
    }
    else if (interrupt_status&HAL_CPR_INT_DOWN_FLAG)
    {
        measured_offset = -steps;
        rail->rail_log.step_down_count+=steps;
    }
    else
    {
        measured_offset = 0;
    }

    /* offset is obtained in pmic steps. Convert that to voltage */
    measured_offset *= rail->pmic_step_size;

    recommendation = current_voltage + measured_offset;

    // Compute the final recommendation based on request, our min and max
    recommendation = MIN(safe_ceil, MAX(recommendation, safe_floor));

    // logging split into two due to need to report 5 data points, collapsing action and step up/down+error_steps may be possible if needed
    // need to confirm (mainly for parsing) that step_up and step_down are never both going to be set
    //SWEVENT(RBCPR_CORNER_UPDATE_REC, rail->stats_rail_id, interrupted_corner, (interrupt_status&HAL_CPR_INT_DOWN_FLAG ? 1 : (interrupt_status&HAL_CPR_INT_UP_FLAG ? 2 : 0)));
    //SWEVENT(RBCPR_CORNER_UPDATE_ACT, rail->stats_rail_id, (recommendation == safe_floor), (recommendation == safe_ceil), recommendation);

    // Check if it is zero because the recommendation could have to been to lower by 'margin'
    // number of steps which we do not honor as per the algorithm
    if (current_voltage != recommendation)
    {
        // Increment our debug counter
        rail->rail_log.cpr_voltage_change_cnt++;

        cpr_image_set_rail_mode_voltage(rail->voltage_control_handle, interrupted_settings->voltage_mode, recommendation);
        //rbcpr_stats_set_corner_voltage(rail->stats_rail_id, rbcpr_corner_index(rail, interrupted_corner), recommendation);

#ifdef ENABLE_CLOSED_LOOP_LOGGING        
        CPR_LOGGING(1, "ISR calling rail transition voltage to change to %d", recommendation);
#endif
        /* time to switch the voltage level */
        cpr_image_rail_transition_voltage(rail->voltage_control_handle);
        ack_result = TRUE;
    }
    else
    {
        rail->rail_log.ignored_readings++;
        ack_result = FALSE;
    }
    if (recommendation == safe_floor)
    {
        rail->rail_log.cpr_voltage_hit_floor++;
        //rbcpr_stats_increment_min_counter(rail->stats_rail_id, rbcpr_corner_index(rail, interrupted_corner));
    }
    if (recommendation == safe_ceil)
    {
        rail->rail_log.cpr_voltage_hit_ceiling++;
        //rbcpr_stats_increment_max_counter(rail->stats_rail_id, rbcpr_corner_index(rail, interrupted_corner));
    }

    rail->cpr_handler_in_progress = FALSE;
    
    //If the block was disabled when handling the interrupt (since for some reason we've re-aggregated to RAILWAY_NO_REQUEST)
    //we don't want to ack it.
    if(rail->block_enabled)
    {
        CORE_VERIFY(rail->current_target_params->voltage_mode!=CPR_VOLTAGE_MODE_OFF);    //Should never get here if the rail has been turned off.
        HAL_cpr_next_meas_en(hal_handle, ack_result);
    }
    else
    {
        CORE_VERIFY(rail->current_target_params->voltage_mode==CPR_VOLTAGE_MODE_OFF);    //Should only happen if the rail has been turned off.
    }
}

const cpr_corner_params_t* cpr_find_corner_params(cpr_closed_loop_rail_t rail, cpr_voltage_mode_t voltage_mode);

void cpr_settle_voltage(cpr_closed_loop_rail_t rail, const cpr_enablement_versioned_rail_config_t* rail_enablement_config)
{
    const cpr_closed_loop_rail_config_t* rail_config = rail->rail_config;
    const HAL_cpr_rail_s* hal_handle = &rail_config->hal_handle;
#ifdef ENABLE_CLOSED_LOOP_LOGGING
    CPR_LOGGING(0, "cpr_settle_voltage");
#endif

    cpr_image_prepare_for_voltage_settling();
    
    //We want to settle in the middle of our down and up thresholds.
    //Calculate that point now.
    const int threshold_midpoint = (rail_config->dn_threshold 
                        + rail_config->up_threshold) / 2;

    const int settling_point = rail_config->dn_threshold - threshold_midpoint;
    CORE_VERIFY(settling_point >= 0);   //For now, only support configs where dn_threshold >= up_threshold.

    for(int i=0; i<rail_enablement_config->mode_to_settle_count; i++)
    {
        cpr_voltage_mode_t mode_to_settle = rail_enablement_config->mode_to_settle[i];
        
        cpr_image_set_rail_mode(rail->voltage_control_handle, mode_to_settle);
        //Check that we went to the correct mode.
        CORE_VERIFY(cpr_image_get_rail_mode(rail->voltage_control_handle) == mode_to_settle);
        
        const cpr_corner_params_t* corner_params = cpr_find_corner_params(rail, mode_to_settle);
        CORE_VERIFY_PTR(corner_params);
        
        cpr_configure_roscs(rail, corner_params);

        HAL_cpr_result_type result;
        
        HAL_cpr_enable_block(hal_handle, TRUE);
        HAL_cpr_poll_result(hal_handle,&result);
        HAL_cpr_enable_block(hal_handle, FALSE);

        HAL_cpr_enable_block(hal_handle, TRUE);

        bool stuff_to_do = true;
        while (stuff_to_do)
        {
            HAL_cpr_poll_result(hal_handle,&result);
            uint32 recommendation = cpr_image_get_rail_mode_voltage(rail->voltage_control_handle, mode_to_settle) - 
                                            rail->pmic_step_size;

            if(result.step_down && result.error_steps>settling_point &&
               recommendation>=corner_params->voltage_floor)
            {
                cpr_image_set_rail_mode_voltage(rail->voltage_control_handle, mode_to_settle, recommendation);
//                rbcpr_stats_set_corner_voltage(rail->stats_rail_id, cpr_corner_index(rail, railway_corner_to_settle), recommendation);

                cpr_image_rail_transition_voltage(rail->voltage_control_handle);

                HAL_cpr_next_meas_en(hal_handle, true);
            }
            else
            {
                stuff_to_do=FALSE;
            }
        }

        HAL_cpr_enable_block(hal_handle, FALSE);
    }

    //Lastly, remove our vote. Railway will prevent the rail from dropping too low.
    cpr_image_set_rail_mode(rail->voltage_control_handle, CPR_VOLTAGE_MODE_OFF);
}


/**
 * <!-- cpr_calc_age_delta_percentage -->
 *
 * @brief calculates the aging percentage by the formula:
 *  (age_min - age_max) - initial_fuse_delta / age_min
 *
 * @param rail : closed loop rail, used for logging 
 * @param hal_handle : HAL handle to the rail, to configure the CPR hardware
 * @param initial_fuse_delta : ID of the CPR sensor being used for aging 
 *
 * @return age_delta_percentage : computed age delta
 */
static int32 cpr_calc_age_delta_percentage(cpr_closed_loop_rail_t rail, HAL_cpr_rail_t hal_handle, int32 initial_fuse_delta)
{
    int32 max_age_delta_percentage;
    max_age_delta_percentage = 0;
    CPR_LOGGING(1, "calculated age delta percentage: %d", max_age_delta_percentage);
    
    return max_age_delta_percentage;
}


/**
 * <!-- cpr_aging_hw_init_and_poll -->
 *
 * @brief Sets the CPR HW to age mode, then polls the hardware
 *
 * @param rail : closed loop rail, used to configure the clocks
 * @param hal_handle : HAL handle to the rail, to configure the CPR hardware
 * @param aging_sensor_id : ID of the CPR sensor being used for aging 
 *
 * @return void : the hardware is polled and the information is stored in registers
 */
static void cpr_aging_hw_init_and_poll(cpr_closed_loop_rail_t rail, HAL_cpr_rail_t hal_handle, uint8 aging_sensor_id)
{
#ifdef ENABLE_CLOSED_LOOP_LOGGING        
     CPR_LOGGING(0, "cpr_aging_hw_init_and_poll");
#endif
}


/**
 * <!-- cpr_aging_poll -->
 *
 * @brief sets the CPR hardware to age mode, then polls the device and calculates the 
 *  age delta percentage, which is used to modify the target_quot values for closed
 *  loop CPR.
 *
 * @param rail : the rail which will apply aging 
 *
 * @return sets rail->aging_percentage to (age_min - age_max) - initial_fuse_delta / age_min
 */
void cpr_aging_poll(cpr_closed_loop_rail_t rail)
{
     CPR_LOGGING(0, "Started cpr_aging_poll");
    CPR_LOGGING(0, "Finished cpr_aging_poll");
}

