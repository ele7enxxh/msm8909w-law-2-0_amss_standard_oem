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
#include "timetick.h"
#include "CoreVerify.h"
#include "err.h"
#include "HAL_rbcpr.h"
#include "rbcpr.h"
#include "rbcpr_comdef.h"
#include "rbcpr_fuses.h"

#include "ClockDefs.h"
#include "Clock.h"
#include <stdbool.h>
#include "cortex-m3.h"
#include "swevent.h"
#include "rbcpr_stats.h"
#include "Chipinfo.h"
#include "HALhwio.h"
#include "rbcpr_qfprom.h"
#include <stringl/stringl.h>

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------
#define RBCPR_APPLY_FUSE_FLOOR

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------
static bool rbcpr_init_done = false;

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
void RBCPRTaskISR(void* task);
void* RBCPRTaskInit(rbcpr_bsp_rail_info* rail, uint32 client_interrupt_id);
void RBCPRTaskCornerChange(void* task);
void rbcpr_bsp_init_rosc_target(rbcpr_bsp_rail_info* rail);

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

    const uint32 hw_version = Chipinfo_GetVersion();
	
	#ifdef FOUNDRY_PROCESS_ID_UNDEFINED
	//default cpr settings tsmc_tn3
	const uint32 process_id = 1;
	const uint32 foundry_id = 0;
	#else
	const uint32 process_id = HWIO_INF(QFPROM_RAW_PTE1, PROCESS_NODE_ID);
	const uint32 foundry_id = HWIO_INF(QFPROM_RAW_PTE2, FOUNDRY_ID);
	#endif

    const ChipInfoFamilyType chip_family = Chipinfo_GetFamily();
    const rbcpr_bsp_revision_array_type* hw_type_settings = NULL;

    for(int32 i=0; i<rbcpr_supported_hw_array.hw_version_specific_settings_count; i++)
    {
        if((*rbcpr_supported_hw_array.hw_version_specific_settings)[i].supported_chipset == chip_family)
        {
            hw_type_settings = &(*rbcpr_supported_hw_array.hw_version_specific_settings)[i];
            break;
        }
    }
    
    if(!hw_type_settings)
        return;

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
				if(process_id == hw_type_settings->hw_version_array[i].rbcpr_bsp_params_tsmc_fab[j].process_id && foundry_id == hw_type_settings->hw_version_array[i].rbcpr_bsp_params_tsmc_fab[j].foundry_id)
				{
					settings_for_this_hw=hw_type_settings->hw_version_array[i].rbcpr_bsp_params_tsmc_fab[j].bsp_params;
					break;
				}
			}
		}
	}

    if(!settings_for_this_hw)
        {
			settings_for_this_hw=hw_type_settings->hw_version_array[0].rbcpr_bsp_params_tsmc_fab[0].bsp_params;
			(*settings_for_this_hw).rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP;
		}
			

    for(int32 i=0; i<rbcpr_bsp_data.num_rails; i++)
    {
        rbcpr_bsp_data.rails[i].bsp_data = &settings_for_this_hw[i];
    }
}

/*===========================================================================
FUNCTION: rbcpr_enable_clock

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static void rbcpr_enable_clock(rbcpr_clock_state* clock_state)
{
    if(!clock_state->enabled)
    {
        clock_state->enabled = true;
        Clock_EnableClock(clock_state->clk_id);
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

rbcpr_corner_params_type* rbcpr_corner_params(rbcpr_bsp_rail_info* rail, railway_corner corner)
{
    CORE_VERIFY(corner<RAILWAY_CORNERS_COUNT);
    int corner_lookup = rail->corner_lookup_index[corner];
    CORE_VERIFY(corner_lookup != CPR_INVALID_CORNER);
    return &rail->bsp_data->target_params[corner_lookup].corner_params;
}

uint32 rbcpr_corner_index(rbcpr_bsp_rail_info* rail, railway_corner corner)
{
    CORE_VERIFY(corner<RAILWAY_CORNERS_COUNT);
    int corner_lookup = rail->corner_lookup_index[corner];
    CORE_VERIFY(corner_lookup != CPR_INVALID_CORNER);
    return corner_lookup;
}

/*===========================================================================
FUNCTION: rbcpr_read_efuse_data

DESCRIPTION: Function to read the e-fuses containing CPR block information

RETURN VALUE:
===========================================================================*/

void rbcpr_set_corner_voltage_based_on_efuse(rbcpr_bsp_rail_info* rail, rbcpr_corner_params_and_fuse_type* corner_params_and_fuse)
{
    rbcpr_corner_params_type* corner_params = &corner_params_and_fuse->corner_params;
    
    const int32 voltage_offset_steps = rbcpr_fuses_get_fuse_value(rail->rail_const_info->rail_name, corner_params->fuse);

    const uint32 default_ceil  = corner_params->voltage_ceil;
    
    //First, pull up the floor voltage based on the eFuse using the formula
    // Floor = MAX( FF-Floor, CEILING( ( 0.9*SS_Ceiling - ( 10mV * CPR-fuse ) + 37.5mV + Correlation_factor) , 5mV (i.e. PMIC step size) )
    
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
    
#ifdef RBCPR_APPLY_FUSE_FLOOR     //We don't want to enable this for SPT testing as it skews where CPR will settle.
    if(rail->rail_const_info->disable_fused_floor == false)
    {
        corner_params->voltage_floor = safe_floor;
    }
#else
    (void) safe_floor;                 //Prevent compiler wanrnings if we don't use the safe floor
#endif  //RBCPR_APPLY_FUSE_FLOOR
    
    //Now set the ceiling.    

    const int voltage_offset = voltage_offset_steps * rail->rail_const_info->efuse_target_voltage_multipler;
    
    uint32 target_voltage = voltage_offset + corner_params->voltage_ceil;
    
    target_voltage = MIN(default_ceil, MAX(target_voltage, corner_params->voltage_floor));


    //Now round up to the PMIC step size.
    if(target_voltage%rail->rail_const_info->pmic_step_size_uv)
    {
        target_voltage += (rail->rail_const_info->pmic_step_size_uv - (target_voltage%rail->rail_const_info->pmic_step_size_uv));
    }
    
    CORE_VERIFY(target_voltage<=default_ceil);
		  
    railway_set_corner_voltage(rail->railway_rail_id, corner_params->corner, target_voltage);
    
    corner_params->voltage_ceil = target_voltage;
    rbcpr_stats_set_fuse_microvolts(rail->stats_rail_id, rbcpr_corner_index(rail, corner_params->corner), target_voltage);
}

static void rbcpr_init_open_loop(rbcpr_bsp_rail_info* rail)
{
    if(rail->bsp_data->sbl_init_done)	//If rail is initialized in SBL then open loop constraints already applied.
        return;

    for(int i=0; i<rail->bsp_data->number_of_target_params; i++)
    {
        if(rail->bsp_data->target_params[i].fuse_function)
        {
            rail->bsp_data->target_params[i].fuse_function(&rail->bsp_data->target_params[i].corner_params, rail, i);
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

//This function does everything required to safely disable CPR.    
static void rbcpr_pause_cpr(rbcpr_bsp_rail_info* rail)
{
    // Set SW_AUTO_CONT_NACK_DN_EN to FALSE
    HAL_rbcpr_set_sw_auto_cont_nack_dn(rail->hal_handle, false);
    // Set SW_AUTO_CONT_ACK_EN to FALSE
    HAL_rbcpr_set_sw_auto_cont_ack(rail->hal_handle, false);

    // Set CONSECUTIVE_DN to 0 and CONSECUTIVE_UP to 0
    HAL_rbcpr_write_consecutive_dn(rail->hal_handle, 0);
    HAL_rbcpr_write_consecutive_up(rail->hal_handle, 0);
    
    // Set RBIF_IRQ_EN to 0
    HAL_rbcpr_enable_int(rail->hal_handle, HAL_RBCPR_INT_UP_FLAG|HAL_RBCPR_INT_DOWN_FLAG, FALSE);
    
    // Set RBIF_IRQ_CLEAR to 0x7F (for resetting up and down counters)
    HAL_rbcpr_clear_int_all(rail->hal_handle);

    // Send RBIF_CONT_ACK_CMD
    HAL_rbcpr_next_meas_en(rail->hal_handle, TRUE);
    // Send RBIF_CONT_NACK_CMD
    HAL_rbcpr_next_meas_en(rail->hal_handle, FALSE);
    
    // Set LOOP_EN to 0
    HAL_rbcpr_enable_block(rail->hal_handle, FALSE);

    //Clear any interrupts that might already be pending
    HAL_rbcpr_clear_int_all(rail->hal_handle);
    interrupt_clear(rail->rail_const_info->interrupt_id);
        
    //In case the ISR has already triggered and scheduled the CPR task,
    //need to flag the task as not needing to call rbcpr_handler
    RBCPRTaskCornerChange(rail->task);
}

//This function does everything required to prepare CPR for the current railway-settings.
//These parts are required also when processing a CPR interrupt as well as when re-enabling the CPR
//after vdd-min or a mode switch.
void rbcpr_prepare_cpr(rbcpr_bsp_rail_info* rail)
{
    const railway_settings* settings = &rail->railway_setting;

    const rbcpr_corner_params_type* corner_params = rbcpr_corner_params(rail, settings->mode);
    //We need to enable the down interrupt if it's dissabled and we're not on the voltage floor
    if(settings->microvolts != corner_params->voltage_floor)
    {
        HAL_rbcpr_enable_int(rail->hal_handle, HAL_RBCPR_INT_DOWN_FLAG, TRUE);
        HAL_rbcpr_set_sw_auto_cont_nack_dn(rail->hal_handle, false);
    }

    //Alternatively, if we're on the floor value for this rail then dissable the down interrupt
    if(settings->microvolts == corner_params->voltage_floor)
    {
        HAL_rbcpr_enable_int(rail->hal_handle, HAL_RBCPR_INT_DOWN_FLAG, FALSE);
        HAL_rbcpr_set_sw_auto_cont_nack_dn(rail->hal_handle, true);
    }

    //Now disable the up interrupt if we're on the ceiling
    if(settings->microvolts == corner_params->voltage_ceil)
    {
        HAL_rbcpr_write_up_threshold(rail->hal_handle, 0xFFFFFFFF); //Set to max.
        HAL_rbcpr_enable_int(rail->hal_handle, HAL_RBCPR_INT_UP_FLAG, FALSE);
    }
    else
    {
        HAL_rbcpr_write_up_threshold(rail->hal_handle, rail->rail_const_info->up_threshold);
        HAL_rbcpr_enable_int(rail->hal_handle, HAL_RBCPR_INT_UP_FLAG, TRUE);
    }
}

//This function does the parts of re-enabling CPR required only after vdd-min or a mode switch.
//It is not required in the case that we're handling a CPR interrupt.
void rbcpr_restart_cpr(rbcpr_bsp_rail_info* rail)
{
    HAL_rbcpr_set_sw_auto_cont_ack(rail->hal_handle, TRUE);

    HAL_rbcpr_clear_int_all(rail->hal_handle);
    interrupt_clear(rail->rail_const_info->interrupt_id);

    HAL_rbcpr_write_consecutive_up(rail->hal_handle, rail->rail_const_info->consecutive_up);
    HAL_rbcpr_write_consecutive_dn(rail->hal_handle, rail->rail_const_info->consecutive_dn);

    /* Enable the system */
    HAL_rbcpr_enable_block(rail->hal_handle, TRUE);
}

/*===========================================================================
FUNCTION: rbcpr_pre_switch_callback

DESCRIPTION: Callback to be executed before a voltage switch

RETURN VALUE:
===========================================================================*/
static void rbcpr_pre_switch_callback(const railway_settings *settings, void* cookie)
{
    CORE_VERIFY_PTR(settings);
    CORE_VERIFY_PTR(cookie);
    rbcpr_bsp_rail_info* rail = (rbcpr_bsp_rail_info*)cookie;

    railway_settings previous_railway_settings;
    memscpy(&previous_railway_settings, sizeof(railway_settings), &rail->railway_setting, sizeof(railway_settings));
    memscpy(&rail->railway_setting, sizeof(railway_settings), settings, sizeof(railway_settings));

    //This implies we're moving corners.
    if (!rail->cpr_handler_in_progress || settings->mode==RAILWAY_NO_REQUEST)
    {
        if(rail->block_enabled)
        {
            /* Update the state of the block */
            rbcpr_pause_cpr(rail);
            rail->block_enabled = FALSE;
        }
    }
    
    if (settings->mode!=previous_railway_settings.mode && settings->mode!=RAILWAY_NO_REQUEST)
        {
            // Set up the oscillators for the new level
            rbcpr_configure(rail, settings->mode);
    }
}

/*===========================================================================
FUNCTION: rbcpr_post_switch_callback

DESCRIPTION:
// Callback to be executed after a voltage switch
// On RPM, this callback is registered with the railway driver

RETURN VALUE:
===========================================================================*/
static void rbcpr_post_switch_callback(const railway_settings *settings, void* cookie)
{
    CORE_VERIFY_PTR(settings);
    CORE_VERIFY_PTR(cookie);

    rbcpr_bsp_rail_info* rail = (rbcpr_bsp_rail_info*)cookie;

    //Save the new railway settings in the stats
    rbcpr_stats_set_railway_settings(rail->stats_rail_id, settings);

    //Assert that the settings we saved away in the pre-switch callback are unchanged here.
    CORE_VERIFY((settings->mode==rail->railway_setting.mode) &&
                (settings->microvolts==rail->railway_setting.microvolts));

    // Increment our debug counter
    rail->post_switch_counter++;

    if(settings->mode==RAILWAY_NO_REQUEST)
        return;
    
    rbcpr_prepare_cpr(rail);

    // If it was not CPR triggered, the pre-switch should have disabled the block
    // We re-enable it here
    if (!rail->cpr_handler_in_progress)
    {
        CORE_VERIFY(!rail->block_enabled);
        rbcpr_restart_cpr(rail);

        /* Update the state of the block */
        rail->block_enabled = TRUE;
    }
}

uint32                            interrupt_status;
/*===========================================================================
FUNCTION: rbcpr_handler

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void rbcpr_handler(rbcpr_bsp_rail_info* rail)
{
    uint32                            recommendation, current_voltage;
    int32                             measured_offset;
    bool                              ack_result = FALSE;
    const railway_corner              interrupted_corner = rail->railway_setting.mode;

    rail->cpr_handler_in_progress = true;

    // Collect the coredump first
    HAL_rbcpr_collect_coredump(rail->hal_handle, &rail->hal_log);

    interrupt_status = HAL_rbcpr_interrupt_status(rail->hal_handle);
    
    /* Clear the interrupts */
    HAL_rbcpr_clear_int_all(rail->hal_handle);

	
    HAL_rbcpr_save_log(rail->hal_handle, &rail->hal_log, interrupted_corner);

    rbcpr_stats_increment_isr_counter(rail->stats_rail_id, rbcpr_corner_index(rail, interrupted_corner));

    // Find the safe voltage limits
    const rbcpr_corner_params_type* corner_params = rbcpr_corner_params(rail, interrupted_corner);
    const uint32 safe_floor = corner_params->voltage_floor;
    const uint32 safe_ceil  = corner_params->voltage_ceil;

    current_voltage = railway_get_corner_voltage(rail->railway_rail_id, interrupted_corner);

    /* Now compute the offset */
    if(interrupt_status&HAL_RBCPR_INT_UP_FLAG)
    {
        //It's possible that both the up and dn interrupts are set.
        //In that case, the recommendation from the HW team is that we step-up the voltage.
        //This could happen if a BHS is toggled after the first interrupt triggers.
        measured_offset = 1;
        rail->step_up_count++;
    }
    else if (interrupt_status&HAL_RBCPR_INT_DOWN_FLAG)
    {
        measured_offset = -1;
        rail->step_down_count++;
    }
    else
    {
        measured_offset = 0;
    }

    /* offset is obtained in pmic steps. Convert that to voltage */
    measured_offset *= rail->rail_const_info->pmic_step_size_uv;

    recommendation = current_voltage + measured_offset;

    // Compute the final recommendation based on request, our min and max
    recommendation = MIN(safe_ceil, MAX(recommendation, safe_floor));

    // logging split into two due to need to report 5 data points, collapsing action and step up/down+error_steps may be possible if needed
    // need to confirm (mainly for parsing) that step_up and step_down are never both going to be set
    SWEVENT(RBCPR_CORNER_UPDATE_REC, rail->stats_rail_id, interrupted_corner, (interrupt_status&HAL_RBCPR_INT_DOWN_FLAG ? 1 : (interrupt_status&HAL_RBCPR_INT_UP_FLAG ? 2 : 0)));
    SWEVENT(RBCPR_CORNER_UPDATE_ACT, rail->stats_rail_id, (recommendation == safe_floor), (recommendation == safe_ceil), recommendation);

    // Check if it is zero because the recommendation could have to been to lower by 'margin'
    // number of steps which we do not honor as per the algorithm
    if (current_voltage != recommendation)
    {
        // Increment our debug counter
        rail->cpr_voltage_change_cnt++;

        railway_set_corner_voltage(rail->railway_rail_id, interrupted_corner, recommendation);
        rbcpr_stats_set_corner_voltage(rail->stats_rail_id, rbcpr_corner_index(rail, interrupted_corner), recommendation);

        /* time to switch the voltage level */
        railway_transition_rail(rail->railway_rail_id);
        ack_result = TRUE;
    }
    else
    {
        rail->ignored_readings++;
        ack_result = FALSE;
    }
    if (recommendation == safe_floor)
    {
        rail->cpr_voltage_hit_floor++;
        rbcpr_stats_increment_min_counter(rail->stats_rail_id, rbcpr_corner_index(rail, interrupted_corner));
    }
    if (recommendation == safe_ceil)
    {
        rail->cpr_voltage_hit_ceiling++;
        rbcpr_stats_increment_max_counter(rail->stats_rail_id, rbcpr_corner_index(rail, interrupted_corner));
    }

    rail->cpr_handler_in_progress = false;
    
    // Collect the coredump at the end of the handler
    HAL_rbcpr_collect_coredump(rail->hal_handle, &rail->hal_log);
    if(rail->block_enabled)
    {
        CORE_VERIFY(rail->railway_setting.mode!=RAILWAY_NO_REQUEST);    //Should never get here if the rail has been turned off.
    HAL_rbcpr_next_meas_en(rail->hal_handle, ack_result);
    }
    else
    {
        CORE_VERIFY(rail->railway_setting.mode==RAILWAY_NO_REQUEST);    //Should only happen if the rail has been turned off.
    }
}

/*===========================================================================
FUNCTION: rbcpr_isr

DESCRIPTION: Interrupt service routine for the CPR interrupt

RETURN VALUE:
===========================================================================*/
static void rbcpr_isr( void ) __irq
{
    uint32 isr = interrupt_current_isr();
    boolean found = false;
    rbcpr_bsp_rail_info* rail = NULL;

    for(int i=0; i<rbcpr_bsp_data.num_rails; i++)
    {
        rail = &rbcpr_bsp_data.rails[i];
        if(rail->rail_const_info->interrupt_id == isr)
        {
            found = true;
            break;
        }
    }

    CORE_VERIFY(found);
    RBCPRTaskISR(rail->task);
}

void rbcpr_enable_closed_loop(rbcpr_bsp_rail_info* rail)
{
    rail->task = RBCPRTaskInit(rail, rail->rail_const_info->interrupt_id);

    /* Now register the callbacks with RPMFW */
    railway_set_callback(rail->railway_rail_id, RAILWAY_PRECHANGE_CB, rbcpr_pre_switch_callback, (void*)rail);
    railway_set_callback(rail->railway_rail_id, RAILWAY_POSTCHANGE_CB, rbcpr_post_switch_callback, (void*)rail);

    HAL_rbcpr_set_sw_auto_cont_ack(rail->hal_handle, true);

    /* Clear out any pending interrupts in the HW */
    HAL_rbcpr_clear_int_all(rail->hal_handle);
    
    //Set up the ISR handler
    /* Now register the interrupt handlers with tramp */
    interrupt_set_isr(rail->rail_const_info->interrupt_id, rbcpr_isr);
    interrupt_configure(rail->rail_const_info->interrupt_id, RISING_EDGE);
    interrupt_clear(rail->rail_const_info->interrupt_id);
    interrupt_enable(rail->rail_const_info->interrupt_id);

    //Store the initial settings.
    railway_get_current_settings(rail->railway_rail_id, &rail->railway_setting);

    if(rail->railway_setting.mode!=RAILWAY_NO_REQUEST)
    {
        /* Now configure the RBCPR system with the GCNT, TARGET values for the information from the fuses */
        rbcpr_configure(rail, rail->railway_setting.mode);

        // Now enable interrupts
        HAL_rbcpr_enable_int(rail->hal_handle, HAL_RBCPR_INT_DOWN_FLAG, TRUE);

        //Only enable the up interrupt if we're not on the ceiling.    
        if(rbcpr_corner_params(rail, rail->railway_setting.mode)->voltage_ceil<=
            rail->railway_setting.microvolts)
        {
            HAL_rbcpr_write_up_threshold(rail->hal_handle, 0xFFFFFFFF); //Set to max.
            HAL_rbcpr_enable_int(rail->hal_handle, HAL_RBCPR_INT_UP_FLAG, FALSE);
        }
        else
        {
            HAL_rbcpr_write_up_threshold(rail->hal_handle, rail->rail_const_info->up_threshold);
            HAL_rbcpr_enable_int(rail->hal_handle, HAL_RBCPR_INT_UP_FLAG, TRUE);
        }

        // Enable the block
        HAL_rbcpr_enable_block(rail->hal_handle, TRUE);

        rail->block_enabled = TRUE;
    }
}

void rbcpr_init_rail_hw(rbcpr_bsp_rail_info* rail)
{
    // Enable the clocks and bring CPR out of reset
    CORE_VERIFY(DAL_SUCCESS == Clock_GetClockId(rail->rail_const_info->ref_clk_resource, &rail->clock[REF_CLK].clk_id));
    rbcpr_enable_clock(&rail->clock[REF_CLK]);

    CORE_VERIFY(DAL_SUCCESS == Clock_GetClockId(rail->rail_const_info->ahb_clk_resource, &rail->clock[AHB_CLK].clk_id));
    rbcpr_enable_clock(&rail->clock[AHB_CLK]);

    /* Configure all the sensors to be enabled */
    HAL_rbcpr_mask_sensor_all(rail->hal_handle, FALSE);

    for(int i=0; i < rail->rail_const_info->number_of_sensors_to_mask; i++)
    {
        HAL_rbcpr_mask_sensor(rail->hal_handle, rail->rail_const_info->sensors_to_mask[i], TRUE);
    }

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
    
    HAL_rbcpr_set_clamp_timer_interval(rail->hal_handle, 1);
}

void rbcpr_settle_voltage(rbcpr_bsp_rail_info* rail)
{
    railway_voter_t cx_voter = railway_create_voter(rail->railway_rail_id, true, RAILWAY_CPR_SETTLING_VOTER);

    //We want to settle in the middle of our down and up thresholds.
    //Calculate that point now.
    const int threshold_midpoint = (rail->rail_const_info->dn_threshold 
                        + rail->rail_const_info->up_threshold) / 2;

    const int settling_point = rail->rail_const_info->dn_threshold - threshold_midpoint;
    CORE_VERIFY(settling_point >= 0);   //For now, only support configs where dn_threshold >= up_threshold.
    
    CORE_VERIFY(rail->hal_handle->hw_base_address);
    for(int i=0; i<rail->bsp_data->number_of_corners_to_settle_at; i++)
    {
        const railway_corner railway_corner_to_settle = rail->bsp_data->corners_to_settle_at[i];

        railway_corner_vote(cx_voter, railway_corner_to_settle);
        railway_transition_rail(rail->railway_rail_id);

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
                rbcpr_stats_set_corner_voltage(rail->stats_rail_id, rbcpr_corner_index(rail, railway_corner_to_settle), recommendation);

                railway_transition_rail(rail->railway_rail_id);

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

/*===========================================================================
FUNCTION: rbcpr_init_rail

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void rbcpr_init_rail(rbcpr_bsp_rail_info* rail)
{
    if(!rail->bsp_data || rail->bsp_data->rbcpr_enablement==RBCPR_DISABLED)
    {
        return;
    }

    rbcpr_init_corner_lookup(rail);
    
    rail->railway_rail_id = rail_id(rail->rail_const_info->rail_name);
    CORE_VERIFY(rail->railway_rail_id != RAIL_NOT_SUPPORTED_BY_RAILWAY);

    
    rbcpr_init_open_loop(rail);
	
    rbcpr_bsp_init_rosc_target(rail);
	
    if(rail->bsp_data->rbcpr_enablement==RBCPR_ENABLED_OPEN_LOOP)
    {
        return;
    }

    rail->hal_handle = HAL_rbcpr_get_rail_handle(rail->rail_const_info->rail_name);
    CORE_VERIFY(rail->hal_handle);

    rbcpr_init_rail_hw(rail);
	
    if(rail->bsp_data->number_of_corners_to_settle_at != 0)
    {
        rbcpr_settle_voltage(rail);
    }

    rbcpr_enable_closed_loop(rail);
}

void rbcpr_settle_cx_voltage_corners(void)
{
    rbcpr_init();
}

/*===========================================================================
FUNCTION: rbcpr_init

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void rbcpr_init(void)
{
    if(rbcpr_init_done)
        return;
        
    rbcpr_init_done = true;

    rbcpr_fuses_init();

    if(!rbcpr_internalize_state())
        rbcpr_init_hw_revision_specifics();

    rbcpr_stats_init();
    

    for(int i = 0; i<rbcpr_bsp_data.num_rails; i++)
    {
        rbcpr_bsp_data.rails[i].stats_rail_id = i;
        rbcpr_init_rail(&rbcpr_bsp_data.rails[i]);
    }

	rbcpr_bsp_target_init();
  //  init_rbcpr_resource();  //Init the handler for turning off specific sensors
}

/*===========================================================================
FUNCTION: rbcpr_cx_cpr_enabled

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
bool rbcpr_cx_cpr_enabled(void)
{
    rbcpr_init_hw_revision_specifics();
    rbcpr_bsp_rail_info* cx_cpr_rail_info = &rbcpr_bsp_data.rails[0];
    return cx_cpr_rail_info->bsp_data->rbcpr_enablement != RBCPR_DISABLED;
}

void HAL_rbcpr_save_hw_state(HAL_rbcpr_rail_t rail);

void rbcpr_core_dump(void)
{
    rbcpr_bsp_rail_info* rail = rbcpr_bsp_data.rails;

    for(int i=0; i<rbcpr_bsp_data.num_rails; rail++, i++)
    {
        if((rail->bsp_data->rbcpr_enablement == RBCPR_ENABLED_CLOSED_LOOP) &&
           (rail->clock[REF_CLK].enabled && rail->clock[AHB_CLK].enabled))
        {
            HAL_rbcpr_save_hw_state(rail->hal_handle);
        }
    }
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
    rbcpr_bsp_rail_info* rail = rbcpr_bsp_data.rails;

    for(int i=0; i < rbcpr_bsp_data.num_rails; rail++, i++)
    {
        if(rail->block_enabled)
        {
            rbcpr_pause_cpr(rail);
        }
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
    rbcpr_bsp_rail_info* rail = rbcpr_bsp_data.rails;

    for(int i=0; i < rbcpr_bsp_data.num_rails; rail++, i++)
    {
        if(rail->block_enabled)
        {
            rbcpr_prepare_cpr(rail);
            rbcpr_restart_cpr(rail);
        }
    }
}
