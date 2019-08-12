/*! \file pm_rpm_ldo_trans_apply.c
 *  \n
 *  \brief This file contains implementation for PMIC LDO Translation
 *         and Apply (Aggregate) functions.
 *  \n
 *  \n &copy; Copyright 2012-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/npa/src/pm_rpm_ldo_trans_apply.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/09/15   akt     PM8916 workaround for LDO11 (CR-686702)
11/03/14   mr      Removed KW Errors from PMIC SW (CR-735987)
06/11/13   hs      Adding settling time for regulators.
05/17/13   hs      PMIC SWEVENT revise.
04/25/13   aks     Code Refactoring: Removing PMIC_SubRsc from pwr algs
04/23/13   hs      Fixed the naming convention in \config.
04/12/13   hs      Code refactoring.
02/27/13   hs      Code refactoring.
02/01/12   hs      Added support for input validation.
01/22/13   dy      Merge code for common code base
07/20/12   hs      Added settling time.
04/16/12   hs      Removed pm_vreg_internal.h.
04/12/12   hs      Fixed the launching of StartTransaction.
                   Fixed the internal resource indexing.
01/26/12   wra     Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "npa.h"
#include "rpmserver.h"
#include "pm_npa.h"

#include "pm_target_information.h"
#include "pm_rpm_ldo_trans_apply.h"
#include "pm_droop_detect.h"
#include "pm_npa_device_ldo.h"
#include "pm_version.h"
#include "CoreVerify.h"
#include "Chipinfo.h"
#include "pm_comm.h"
#include "device_info.h"
#include "pm_config.h"

static pm_npa_ldo_data_type *pm_rpm_ldo_req_data[PM_MAX_NUM_PMICS];

void pm_rpm_ldo_translation(rpm_translation_info *pm_rpm_ldo_translation_info);
void pm_rpm_ldo_apply(rpm_application_info *pm_rpm_ldo_application_info);
void pm_rpm_ldo_pre_dependency_analysis(rpm_application_info *info, pm_npa_ldo_int_rep *previous_aggregation);
void pm_rpm_ldo_post_dependency_analysis(rpm_application_info *info, pm_npa_ldo_int_rep *previous_aggregation);
int pm_rpm_ldo_aggregate(rpm_application_info *info);
void pm_rpm_ldo_execute_driver(rpm_application_info *info, pm_npa_ldo_int_rep *previous_aggregation);
void pm_rpm_ldo_settle(rpm_application_info *info);
static void pm_rpm_ldo_droop_detect_init(rpm_resource_type resourceType, uint32 num_npa_resources);
static void pm_rpm_ldo_droop_detect_irq_enable(rpm_application_info *info, boolean enable);

void pm_rpm_ldo_translation(rpm_translation_info *info)
{
    unsigned type, length, *value;
    pm_npa_ldo_data_type  *ldo_data = (pm_npa_ldo_data_type*)info->cb_data;
    pm_ldo_data_type      *ldo_ptr = NULL;

    CORE_VERIFY_PTR(ldo_data);
    ldo_ptr = ldo_data->ldoDriverData;
    CORE_VERIFY_PTR(ldo_ptr);

    // Return for invalid PMIC resource ids. PMIC resource id 0 is invalid.
    if((info->id == 0) || (info->id > ldo_ptr->pm_pwr_data.num_of_peripherals))
    {
        return;
    }

    ((pm_npa_ldo_int_rep *)(info->dest_buffer))->resource_id = info->id;
    ((pm_npa_ldo_int_rep *)(info->dest_buffer))->device_index = GET_PMIC_INDEX_LDO(((pm_npa_ldo_data_type*)info->cb_data)->resourceType) ;
    ((pm_npa_ldo_int_rep *)(info->dest_buffer))->reserve1 = 0;

    while(kvp_get(info->new_kvps, &type, &length, (const char **)&value))
    {
        // Need size to match.
        if(sizeof(npa_resource_state) != length)
            continue;

        switch(type)
        {
        case PM_NPA_KEY_SOFTWARE_ENABLE:
            {
                ((pm_npa_ldo_int_rep *)(info->dest_buffer))->sw_en = *value;

                break;
            }
        case PM_NPA_KEY_LDO_SOFTWARE_MODE:
            {
                ((pm_npa_ldo_int_rep *)(info->dest_buffer))->ldo_sw_mode = *value;
                break;
            }
        case PM_NPA_KEY_PIN_CTRL_ENABLE:
            {
                ((pm_npa_ldo_int_rep *)(info->dest_buffer))->pc_en = *value;
                break;
            }
        case PM_NPA_KEY_PIN_CTRL_POWER_MODE:
            {
                ((pm_npa_ldo_int_rep *)(info->dest_buffer))->pc_mode = *value;
                break;
            }
        case PM_NPA_KEY_MICRO_VOLT:
            {
                ((pm_npa_ldo_int_rep *)(info->dest_buffer))->output_uvol   = *value;
                ((pm_npa_ldo_int_rep *)(info->dest_buffer))->input_uvol  = *value;
                break;
            }
        case PM_NPA_KEY_CURRENT:
            {
                ((pm_npa_ldo_int_rep *)(info->dest_buffer))->ip = *value;
                break;
            }
        case PM_NPA_KEY_HEAD_ROOM:
            {
                ((pm_npa_ldo_int_rep *)(info->dest_buffer))->noise_hr = *value;
                break;
            }
        case PM_NPA_KEY_BYPASS_ALLOWED_KEY:
            {
                ((pm_npa_ldo_int_rep *)(info->dest_buffer))->byp_allowed = *value;
                break;
            }
        default:
            {
                //should never go here
            }
        }
    }
}

void pm_rpm_ldo_apply(rpm_application_info *info)
{
    uint8                  change_detected = 0;
    pm_npa_ldo_int_rep     previous_aggregation = *((pm_npa_ldo_int_rep*)(info->current_aggregation));
    uint64                 current_time = 0;
    uint64                 settle_start_time = 0;
    uint64                 settle_end_time = 0;
    uint32                 min_settling_time_us = 0;
    uint32                 settling_time_elapsed_us = 0;
    uint32                 settling_time_remaining_us = 0;
    boolean                vreg_status = FALSE;
    boolean                stepper_status = FALSE; 
    boolean                ldo_is_stepper = FALSE;
    uint8                  internal_resource_index = 0;
    uint8                  pmic_index = 0;
    pm_npa_ldo_data_type  *ldo_data = (pm_npa_ldo_data_type*)info->cb_data;
    pm_ldo_data_type      *ldo_ptr = NULL;
    pm_npa_ldo_int_rep    *curr_aggr = NULL;

    CORE_VERIFY_PTR(ldo_data);

    ldo_ptr = ldo_data->ldoDriverData;

    CORE_VERIFY_PTR(ldo_ptr);

    // Return for invalid PMIC resource ids. PMIC resource id 0 is invalid.
    if((info->id == 0) || (info->id > ldo_ptr->pm_pwr_data.num_of_peripherals))
    {
        return;
    }

    internal_resource_index = (uint8)(info->id - 1);

    pmic_index = GET_PMIC_INDEX_LDO(ldo_data->resourceType);
    CORE_VERIFY(pmic_index < PM_MAX_NUM_PMICS);

    if(0 == info->settling_time) //  this is a new request
    {
        // Aggregate the results, but don't call driver
        change_detected = pm_rpm_ldo_aggregate(info);

        if(change_detected != 0)
        {
            pm_rpm_ldo_dependency_execute(info, &previous_aggregation);
        }
    }
    else // this is an old request to check if the rail has settled
    {
        // Check the current aggregation to see if the rail got disabled while settling parallely
        curr_aggr = (pm_npa_ldo_int_rep*)info->current_aggregation;

        if(curr_aggr->sw_en != 0)
        {
           settle_start_time = ldo_data->settlingStartTime[internal_resource_index];
           settle_end_time = ldo_data->settlingEndTime[internal_resource_index];

           // take the current system time in time tick
           current_time = time_service_now();

           // make sure the rail didn't finish settling due to a different request
           // only then update the settling time elapsed and check min settling time
           if(settle_start_time != 0)
           {
              // calculate the settling time elapsed in micro seconds
              if(current_time > settle_start_time)
              {
                 settling_time_elapsed_us = pm_convert_timetick_to_time(current_time - settle_start_time);
              }

              // get the minimum settling time info for the rail from config data
              min_settling_time_us = ldo_data->railInfo[pmic_index][internal_resource_index].MinSettlingTime;
           }

           // if minimum settling time has elapsed, only then check for VREG_OK ow call back to RPM server
           if (settling_time_elapsed_us >= min_settling_time_us)
           {
              // Check for VREG_OK status
              pm_pwr_sw_enable_vreg_ok_status_alg(&ldo_ptr->pm_pwr_data, ldo_ptr->comm_ptr, 
                                                  internal_resource_index, &vreg_status);

              // For stepper LDOs, Check for STEPPER_DONE status also
              switch(ldo_ptr->pm_pwr_data.pwr_specific_info[internal_resource_index].periph_type)
              {
              case PM_HW_MODULE_LDO:
                 switch(ldo_ptr->periph_subtype[internal_resource_index])
                 {
                 case PM_HW_MODULE_LDO_N600_STEPPER:
                 case PM_HW_MODULE_LDO_N1200_STEPPER:              
                    pm_pwr_volt_level_stepper_done_status_alg(&ldo_ptr->pm_pwr_data, ldo_ptr->comm_ptr, 
                                                              internal_resource_index, &stepper_status);
                    ldo_is_stepper = TRUE;
                    break;
                 default:
                    break;
                 }
                 break;
              case PM_HW_MODULE_ULT_LDO:
                 switch(ldo_ptr->periph_subtype[internal_resource_index])
                 {
                 case PM_HW_MODULE_ULT_LDO_N300_STEPPER:
                 case PM_HW_MODULE_ULT_LDO_N600_STEPPER:
                 case PM_HW_MODULE_ULT_LDO_N900_STEPPER:
                 case PM_HW_MODULE_ULT_LDO_N1200_STEPPER:              
                    pm_pwr_volt_level_stepper_done_status_alg(&ldo_ptr->pm_pwr_data, ldo_ptr->comm_ptr, 
                                                              internal_resource_index, &stepper_status);
                    ldo_is_stepper = TRUE;
                    break;
                 default:
                    break;
                 }
                 break;
              default:
                 break;
              }

              if((vreg_status == FALSE) || ((ldo_is_stepper == TRUE) && (stepper_status == FALSE)))
              {
                 // if the rail finished settling due to a different request and still 
                 // VREG_OK is not high then give the rail some more time to settle internally
                 if(settle_start_time == 0)
                 {
                    // save the current time as settling start time
                    ldo_data->settlingStartTime[internal_resource_index] = current_time;

                    // calculate the settle end time using the settling time threshold
                    ldo_data->settlingEndTime[internal_resource_index] = 
                               current_time + pm_convert_time_to_timetick(PM_RPM_SETTTLING_TIME_THRESHOLD);

                    // poll for VREG_OK to continue settling
                    pm_rpm_ldo_settle(info);
                 }
                 else if(current_time < settle_end_time) // still need more time to settle
                 {
                    // calculate the remaining settling time in micro secs
                    settling_time_remaining_us = pm_convert_timetick_to_time(settle_end_time - current_time);

                    // if the remaining settling time is small enough, wait internally
                    if(settling_time_remaining_us < PM_RPM_SETTTLING_TIME_THRESHOLD)
                    {
                       // poll for VREG_OK to continue settling
                       pm_rpm_ldo_settle(info);
                    }
                 }
                 else // estimated settling time elapsed, rail should have already settled
                 {
                    // poll for VREG_OK to continue settling
                    pm_rpm_ldo_settle(info);
                 }
              }
              else // settling is done since VREG_OK status is TRUE
              {
                 // reset the settling time values
                 info->settling_time = 0;
                 ldo_data->settlingEndTime[internal_resource_index] = 0;
                 ldo_data->settlingStartTime[internal_resource_index] = 0;
              }
           }
        }
        else // reset the settling time if the rail got disabled while settling parallely
        {
            // reset the settling time values
            info->settling_time = 0;
            ldo_data->settlingEndTime[internal_resource_index] = 0;
            ldo_data->settlingStartTime[internal_resource_index] = 0;
        }

        if(info->settling_time == 0)
        {
           //After the rail has settled, put the LDO into BYPASS if needed
           if (ldo_data->inBypass[internal_resource_index])
           {
              pm_ldo_sw_mode(pmic_index, internal_resource_index, PM_SW_MODE_BYPASS);
           }

           // Check if the rail's droop detection interrupt has to be re-enabled
           // after the rail has finished parallel settling
           pm_rpm_ldo_droop_detect_irq_enable(info, TRUE);
        }
    } //end if(0 == info->settling_time)
}

void pm_rpm_ldo_dependency_execute(rpm_application_info *info, pm_npa_ldo_int_rep *previous_aggregation)
{
    // Check the dependencies
    pm_rpm_ldo_pre_dependency_analysis(info, previous_aggregation);

    // Execute the driver calls once all dependencies have been executed
    pm_rpm_ldo_execute_driver(info, previous_aggregation);

    // Check the dependencies
    pm_rpm_ldo_post_dependency_analysis(info, previous_aggregation);
}

int pm_rpm_ldo_aggregate(rpm_application_info *info)
{
    int change_detected = 0;
    unsigned volt_adj = 0; // for fuzz check
    unsigned safety_hr = 0;
    // For each client aggregate the correct values.
    pm_npa_ldo_int_rep aggregatedIntRep = {0};

    // get the resource type from the info->cb_data
    pm_npa_ldo_data_type* ldo_data = (pm_npa_ldo_data_type*)info->cb_data;

    unsigned numberOfClients ,clientCount,clientType;
    void* state = NULL;
    pm_npa_ldo_int_rep* clientRequest;
    pm_ldo_data_type* ldo_ptr = ldo_data->ldoDriverData;
    uint8 pmic_index = GET_PMIC_INDEX_LDO(ldo_data->resourceType);
    pm_volt_level_type ceiling_voltage = 0; //voltage rounded to next high
    uint32             dummy_vset = 0;

    numberOfClients = rpm_get_num_clients(info->resource_handle);
    safety_hr = ldo_data->railInfo[pmic_index][info->id-1].SafetyHeadRoom;

    if(ldo_data->railInfo[pmic_index][info->id - 1].AccessAllowed)
    {
        aggregatedIntRep.resource_id = info->id;
        aggregatedIntRep.device_index = pmic_index;


        for(clientCount = 0; clientCount < numberOfClients; clientCount++)
        {
            rpm_get_current_request(info->resource_handle, clientCount, &clientType, (const void **)&state);

            // EE Client and Internal LDO clients
            if( (clientType == 0) ||(clientType == PM_RPM_NPA_CLIENT_LDO_REQ))
            {
                clientRequest = (pm_npa_ldo_int_rep *)state;

                if(clientRequest != NULL)
                {
                    // Aggregate Software Enable
                    aggregatedIntRep.sw_en          = MAX(aggregatedIntRep.sw_en,       clientRequest->sw_en);

                    // Aggregate Software Mode Enable
                    aggregatedIntRep.ldo_sw_mode        = MAX(aggregatedIntRep.ldo_sw_mode,       clientRequest->ldo_sw_mode);

                    // Aggregate ORed values of Pin Control Enable
                    aggregatedIntRep.pc_en     = aggregatedIntRep.pc_en | clientRequest->pc_en;

                    // Aggregate ORed values of Pin Control Power Mode
                    aggregatedIntRep.pc_mode = aggregatedIntRep.pc_mode | clientRequest->pc_mode;

                    // Aggregate Highest Output Voltage
                    aggregatedIntRep.output_uvol   = MAX(aggregatedIntRep.output_uvol  , clientRequest->output_uvol  );

                    // Aggregate Highest Input Voltage
                    aggregatedIntRep.input_uvol  = MAX(aggregatedIntRep.input_uvol ,
                    clientRequest->output_uvol   + (MAX(safety_hr, clientRequest->noise_hr) * 1000));

                    // Aggregate Summation of Current
                    aggregatedIntRep.ip             = aggregatedIntRep.ip + clientRequest->ip;

                    // Aggregate Headroom Voltage - I don't think we need to aggregate this. It is part of the
                    // regulated voltage aggregation.
                    aggregatedIntRep.noise_hr = MAX(aggregatedIntRep.noise_hr, clientRequest->noise_hr);

                    // Aggregate Bypass Allowed
                    aggregatedIntRep.byp_allowed = MAX(aggregatedIntRep.byp_allowed, clientRequest->byp_allowed);
                }
            }
        }

        // input checking after aggregation.
        if(ldo_data->railInfo[pmic_index][info->id - 1].AlwaysOn)
        {
            aggregatedIntRep.sw_en = 1;
        }

        if(ldo_data->railInfo[pmic_index][info->id - 1].BypassDisallowed)
        {
            aggregatedIntRep.byp_allowed = PM_NPA_BYPASS_DISALLOWED;
        }

        if(aggregatedIntRep.ldo_sw_mode < ldo_data->railInfo[pmic_index][info->id - 1].MinPwrMode)
        {
            aggregatedIntRep.ldo_sw_mode = ldo_data->railInfo[pmic_index][info->id - 1].MinPwrMode;
        }

        if( (aggregatedIntRep.output_uvol   < ldo_data->railInfo[pmic_index][info->id - 1].MinVoltage*1000) )
        {
            volt_adj = ldo_data->railInfo[pmic_index][info->id - 1].MinVoltage*1000 - aggregatedIntRep.output_uvol  ;

            aggregatedIntRep.output_uvol   = ldo_data->railInfo[pmic_index][info->id - 1].MinVoltage*1000;
            aggregatedIntRep.input_uvol  = aggregatedIntRep.input_uvol  + volt_adj;
        }
        else if(aggregatedIntRep.output_uvol   > ldo_data->railInfo[pmic_index][info->id - 1].MaxVoltage*1000)
        {
            volt_adj = aggregatedIntRep.output_uvol   - ldo_data->railInfo[pmic_index][info->id - 1].MaxVoltage*1000;

            aggregatedIntRep.output_uvol   = ldo_data->railInfo[pmic_index][info->id - 1].MaxVoltage*1000;
            aggregatedIntRep.input_uvol  = aggregatedIntRep.input_uvol  - volt_adj;
        }

        if(aggregatedIntRep.output_uvol != 0)
        {
           //check if rounding off vset is needed
           pm_pwr_volt_calculate_vset_celing_uv(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, info->id - 1,
                                                (pm_volt_level_type)aggregatedIntRep.output_uvol, &dummy_vset,  &ceiling_voltage);
        }

        if(ceiling_voltage != 0)
        {
            aggregatedIntRep.input_uvol +=   ceiling_voltage - aggregatedIntRep.output_uvol;
            aggregatedIntRep.output_uvol =  ceiling_voltage;
        }

        change_detected = pm_rpm_int_copy( &aggregatedIntRep, (pm_npa_ldo_int_rep *)info->current_aggregation, sizeof(pm_npa_ldo_int_rep));
    } // end access_allowed

    return change_detected;
}

void pm_rpm_ldo_pre_dependency_analysis(rpm_application_info *info, pm_npa_ldo_int_rep *previous_aggregation)
{
    pm_pwr_resource_dependency_info     *depInfo = NULL;
    pm_npa_ldo_int_rep  *cur_data = (pm_npa_ldo_int_rep*)info->current_aggregation;
    pm_npa_ldo_data_type  *cb_data = (pm_npa_ldo_data_type*)info->cb_data;
    uint8 pmic_index = GET_PMIC_INDEX_LDO(cb_data->resourceType);

    if((previous_aggregation->sw_en < cur_data->sw_en) || (cur_data->pc_en))
    {
        cur_data->is_en_transition = TRUE;
    }
    else
    {
        cur_data->is_en_transition = FALSE;
    }

    if(cb_data->depInfo[pmic_index] != NULL)
    {
        depInfo = &cb_data->depInfo[pmic_index][info->id];
    }

    if ((depInfo != NULL) && (depInfo->parent_source_dependency != NULL))
    {
        // Detect if this is a pre-operation change on the parent
        // These conditions are
        // If the required headroom is increased
        // If the voltage setting is increased
        // If the LDO goes from a disabled to an enabled state
        // If the LDO Pin Control goes from disabled to an enabled state
        // If the Bypass enable transitions from enable to disable
        if( (cur_data->input_uvol  > previous_aggregation->input_uvol ) ||
            (cur_data->output_uvol   > previous_aggregation->output_uvol  ) ||
            (cur_data->is_en_transition == TRUE) ||
            (cur_data->ip > previous_aggregation->ip) ||
            (cur_data->byp_allowed > previous_aggregation->byp_allowed))
        {

            rpm_issue_request(depInfo->parent_source_dependency->resource_type,
                depInfo->parent_source_dependency->internal_resource_index,
                depInfo->parent_source_dependency_client_handle,
                sizeof(pm_npa_ldo_int_rep), cur_data);

        }
    }
}

void pm_rpm_ldo_post_dependency_analysis(rpm_application_info *info, pm_npa_ldo_int_rep *previous_aggregation)
{
    pm_pwr_resource_dependency_info *depInfo = NULL;
    pm_npa_ldo_int_rep *cur_data = (pm_npa_ldo_int_rep*)info->current_aggregation;
    pm_npa_ldo_data_type *cb_data = (pm_npa_ldo_data_type*)info->cb_data;
    uint8 change_detected = 0;
    uint8 pmic_index = GET_PMIC_INDEX_LDO(cb_data->resourceType);

    // Pin Control can cause voltage droop anytime, so don't allow bypass
    if(cur_data->pc_en)
    {
        cur_data->is_en_transition = TRUE;
    }
    else
    {
        cur_data->is_en_transition = FALSE;
    }

    if(cb_data->depInfo[pmic_index] != NULL)
    {
        depInfo = &cb_data->depInfo[pmic_index][info->id];
    }

    if ((depInfo != NULL) && (depInfo->parent_source_dependency != NULL))
    {
        // Detect if this is a post-operation change on the parent
        // These conditions are
        // If the required headroom is decreased
        // If the voltage setting is decreased
        // If the LDO's software enable state changes
        // If the LDO Pin Control goes from enabled to an disabled state
        // If the Bypass enable transitions from enable to disable
        if( (cur_data->input_uvol  <= previous_aggregation->input_uvol ) || // "=" is to allow SMPS to turn
            (cur_data->output_uvol   <= previous_aggregation->output_uvol  ) ||
            (cur_data->sw_en != previous_aggregation->sw_en) ||
            (cur_data->pc_en < previous_aggregation->pc_en) )
        {
            change_detected = 1;
        }

        if(1 == change_detected )
        {
            //save the LDO ID
            cur_data->resource_id = info->id;

            rpm_issue_request(depInfo->parent_source_dependency->resource_type,
                depInfo->parent_source_dependency->internal_resource_index,
                depInfo->parent_source_dependency_client_handle,
                sizeof(pm_npa_ldo_int_rep), cur_data);
        }
    }
}

void pm_rpm_ldo_execute_driver(rpm_application_info *info, pm_npa_ldo_int_rep *previous_aggregation)
{
    pm_npa_ldo_int_rep *ldo_int_rep =(pm_npa_ldo_int_rep *)info->current_aggregation;
    pm_npa_ldo_data_type *ldo_data = (pm_npa_ldo_data_type *)info->cb_data;
    pm_ldo_data_type* ldo_ptr = NULL;
    unsigned lp_hp_current_threshold = 0;
    uint64 curr_settle_start_time = 0;
    uint64 prev_settle_start_time = 0;
    uint64 curr_settle_end_time = 0;
    uint64 prev_settle_end_time = 0;
    uint32 estimated_settling_time_us = 0;
    uint32 min_settling_time_us = 0;
    uint32 stepper_uv = 0;
    uint32 stepper_us = 0;
    uint8 internal_resource_index = (uint8)(info->id - 1);
    uint8 pmic_index = GET_PMIC_INDEX_LDO(ldo_data->resourceType);

    //Get the PmicResource from the call back data.
     ldo_ptr = ldo_data->ldoDriverData;


    if(ldo_ptr != NULL)
    {
        if(ldo_data->inBypass[internal_resource_index] == 0)//LDO is not in bypass
        {
            /* Set Power Mode */
            if(ldo_int_rep->ldo_sw_mode == PM_NPA_SW_MODE_LDO__IPEAK)
            {
                if( (ldo_int_rep->ldo_sw_mode != previous_aggregation->ldo_sw_mode) || (ldo_int_rep->ip != previous_aggregation->ip) )
                {
                    lp_hp_current_threshold = ldo_data->railInfo[pmic_index][internal_resource_index].LpHpCurrentThreshold;

                    if(ldo_int_rep->ip >= lp_hp_current_threshold)
                    {

                        pm_ldo_sw_mode(pmic_index, internal_resource_index, PM_SW_MODE_NPM);
                    }
                    else
                    {
                        pm_ldo_sw_mode(pmic_index, internal_resource_index, PM_SW_MODE_LPM);
                    }
                }
            }
            else
            {
                if(ldo_int_rep->ldo_sw_mode != previous_aggregation->ldo_sw_mode)
                {
                    pm_ldo_sw_mode(pmic_index, internal_resource_index, PM_SW_MODE_NPM);
                }
            }
        }

        /* Set the Voltage */
        if((ldo_int_rep->output_uvol != previous_aggregation->output_uvol) && (ldo_int_rep->output_uvol != 0))
        {
            // Check if the rail's droop detection interrupt has to be disabled
            pm_rpm_ldo_droop_detect_irq_enable(info, FALSE);

            pm_pwr_volt_level_alg(&ldo_ptr->pm_pwr_data, ldo_ptr->comm_ptr, internal_resource_index,
                                  (pm_volt_level_type)ldo_int_rep->output_uvol);

            //calculate the estimated settling time.
            if(ldo_ptr->pm_pwr_data.pwr_specific_info[internal_resource_index].periph_type == PM_HW_MODULE_ULT_LDO)
            {
               stepper_uv = ldo_data->ultSettlingTimeInfo->stepper_uv;
               stepper_us = ldo_data->ultSettlingTimeInfo->stepper_us;
            }
            else
            {
               stepper_uv = ldo_data->settlingTimeInfo->stepper_uv;
               stepper_us = ldo_data->settlingTimeInfo->stepper_us;
            }

            // voltage changed, adding settling time.
            if(ldo_int_rep->output_uvol > previous_aggregation->output_uvol)
            {
                //calculate the estimated settling time
                estimated_settling_time_us = ((ldo_int_rep->output_uvol - previous_aggregation->output_uvol)/stepper_uv)*stepper_us;
            }
            else
            {
                //calculate the estimated settling time
                estimated_settling_time_us = ((previous_aggregation->output_uvol - ldo_int_rep->output_uvol)/stepper_uv)*stepper_us;
            }
        }

        /* Set PIN Control Enable */
        if(ldo_int_rep->pc_en != previous_aggregation->pc_en)
        {
            pm_pwr_pin_ctrl_alg(&ldo_ptr->pm_pwr_data, ldo_ptr->comm_ptr, internal_resource_index, (uint8)ldo_int_rep->pc_en);
        }

        /* Set PIN Control Power Mode */
        if(ldo_int_rep->pc_mode != previous_aggregation->pc_mode)
        {
            pm_pwr_pin_ctrl_mode_alg(&ldo_ptr->pm_pwr_data, ldo_ptr->comm_ptr, internal_resource_index, (uint8)ldo_int_rep->pc_mode);
        }

        /* State set */
        if(ldo_int_rep->sw_en != previous_aggregation->sw_en)
        {
            // check the current state and compare to desired state
            // if we are transitioning from disable to enable state via SW mode
            // then we need  we need to check that other sub regulated LDOs on this SMPS
            // are not in Bypass.The power on current draw will cause the bypassed LDO voltage to droop.
            // General rule for enabling another sub regulated LDO on the same SMPS is 50mV head room.
            // If LDO is pin control enabled then other sub regulated LDOs should be taken out of bypass

            // the LDO is asked to turn off and is in bypass mode, then we need to first take it out of bypass, then turn it off
            // so the next time when it's askedt o turn on, it won't go to bypass directly
            if ( (ldo_int_rep->sw_en == 0) && (ldo_data->inBypass[internal_resource_index] == 1) )
            {
                pm_ldo_sw_mode(pmic_index, internal_resource_index, PM_SW_MODE_NPM);

                ldo_data->inBypass[internal_resource_index] = 0;
            }

            // Check if the rail's droop detection interrupt has to be disabled
            pm_rpm_ldo_droop_detect_irq_enable(info, FALSE);

            pm_pwr_sw_enable_alg(&ldo_ptr->pm_pwr_data, ldo_ptr->comm_ptr, internal_resource_index, (pm_on_off_type)ldo_int_rep->sw_en);

            if(ldo_int_rep->sw_en > previous_aggregation->sw_en)
            {
               // get the minimum settling time from config if rail is enabling
               min_settling_time_us = ldo_data->railInfo[pmic_index][internal_resource_index].MinSettlingTime;

               // calculate the estimated settling time
               if(ldo_ptr->pm_pwr_data.pwr_specific_info[internal_resource_index].periph_type == PM_HW_MODULE_ULT_LDO)
               {
                  estimated_settling_time_us = MAX(ldo_data->ultSettlingTimeInfo->vreg_ok_us, min_settling_time_us);
               }
               else
               {
                  estimated_settling_time_us = MAX(ldo_data->settlingTimeInfo->vreg_ok_us, min_settling_time_us);
               }
            }

            // Work-around for PM8916 (MSM8916/MSM8936/MSM8909) LDO11 voltage over-shoot issue seen */
            // Disable LDO11 if it's not settled after making sure we internally settle for safe time (~300us) 
            // LDO11 of the PM8916 which is susseptible to over current as it does not have OCP
            if ((PMIC_IS_PM8916 == pm_get_pmic_model(pmic_index)) && (10 == internal_resource_index))
            {
                 // Poll wait (VREG_OK) for 25*2 = 50 micro secs  (10 is the MAX settling delay calculated later)
                 estimated_settling_time_us = 25;
            }

        }

        // -------------- Settling time ------------------

        // Get the previous settle start time to check if rail is already settling
        prev_settle_start_time = ldo_data->settlingStartTime[internal_resource_index];

        // Check if rail has to settle or not
        if(((estimated_settling_time_us != 0) || (prev_settle_start_time != 0)) && 
           (ldo_int_rep->sw_en != 0) && (ldo_data->railInfo[pmic_index][internal_resource_index].SettlingEnabled == PM_SETTLING_EN))
        {
           // get the current settle start time
           curr_settle_start_time = time_service_now();

           // if the rail is not already settling
           if(prev_settle_start_time == 0)
           {
              // Update the current settling start time
              ldo_data->settlingStartTime[internal_resource_index] = curr_settle_start_time;

              // calculate the current settle end time
              curr_settle_end_time = curr_settle_start_time + pm_convert_time_to_timetick(estimated_settling_time_us);
           }
           else // rail is already settling
           {
              // Get the previous settle end time
              prev_settle_end_time = ldo_data->settlingEndTime[internal_resource_index];

              // check if current request needs any settling
              if(estimated_settling_time_us != 0)
              {
                 // calculate the current settle end time
                 curr_settle_end_time = curr_settle_start_time + pm_convert_time_to_timetick(estimated_settling_time_us);

                 // Use the MAX of current and previous settle end times as current
                 curr_settle_end_time = MAX(curr_settle_end_time, prev_settle_end_time);

                 // Calculate the new estimated settling time
                 estimated_settling_time_us = pm_convert_timetick_to_time(curr_settle_end_time - prev_settle_start_time);
              }
              else // current request needs no settling
              {
                 // Use the prev settle end time as current
                 curr_settle_end_time = prev_settle_end_time;

                 // Use the previous estimated settling time
                 estimated_settling_time_us = pm_convert_timetick_to_time(prev_settle_end_time - prev_settle_start_time);
              }
           }

           // save the timestamp at which rail is estimated to be settled in RPM info struct and internal struct
           ldo_data->settlingEndTime[internal_resource_index] = curr_settle_end_time;
           info->settling_time = curr_settle_end_time;

           // if the estimated settling time is less than threshold or client is internal, settle internally
           if((estimated_settling_time_us < PM_RPM_SETTTLING_TIME_THRESHOLD) ||
              (info->client_type == PM_RPM_NPA_CLIENT_LDO_REQ))
           {
              // busywait for minimum settling time if any, updated only for enabling rails
              if(min_settling_time_us != 0)
              {
                 DALSYS_BusyWait(min_settling_time_us);
              }

              // internal settling
              pm_rpm_ldo_settle(info);

              // Check if the rail's droop detection interrupt has to be re-enabled
              // after the rail has settled internally
              pm_rpm_ldo_droop_detect_irq_enable(info, TRUE);
           }
        }

        // -------------- Settling time ------------------

        ldo_int_rep->is_en_transition = 0; // This might need to get saved somehow.
    }
}

void pm_rpm_ldo_bypass_cb(uint32 pmic_index, uint32 resource_id, pm_on_off_type on_off)
{
    pm_npa_ldo_int_rep *ldo_int_rep = NULL;
    uint32 resource_type = RPM_LDO_A_REQ;
    pm_npa_ldo_data_type  *ldo_data = NULL;
    pm_sw_mode_type mode = PM_SW_MODE_NPM;

    resource_type = resource_type + (pmic_index << 24);
    ldo_data = pm_rpm_ldo_get_resource_data(pmic_index);

    CORE_VERIFY_PTR(ldo_data);

    rpm_get_aggregated_request_buffer((rpm_resource_type)resource_type, resource_id, (const void **)&ldo_int_rep);
    if ((ldo_data->inBypass[resource_id-1] != on_off) && /* the LDO's current bypass status needs to be changed. */
        (((pm_npa_ldo_int_rep *)ldo_int_rep)->sw_en > 0))/* the LDO currently is ON */
    {
        if((pm_on_off_type)on_off == PM_ON)
        {
            mode = PM_SW_MODE_BYPASS;
            ldo_data->inBypass[resource_id-1] = PM_ON;
            //we are updating only the in bypass cache here
            //the actual bypass mode would be set only when the rail
            //has settled. If the settling start timestamp is non-zero
            //then the rail has to still settle
            if(ldo_data->settlingStartTime[resource_id-1] == 0)
            {
               pm_ldo_sw_mode(pmic_index, (uint8)resource_id-1, mode);
            }
        }
        else if ((pm_on_off_type)on_off == PM_OFF)
        {
           ldo_data->inBypass[resource_id-1] = PM_OFF;
           pm_ldo_sw_mode(pmic_index, (uint8)resource_id-1, mode);
        }
    }

}

void pm_rpm_ldo_register_resources(rpm_resource_type resourceType, uint32 num_npa_resources , uint8 pmic_index)
{
    pm_npa_ldo_data_type *ldo_data = NULL;
    uint32 ldo_index = 0;
    pm_ldo_data_type* ldo_ptr = NULL;
    uint8 range = 0;


    if(num_npa_resources > 0)
    {
        ldo_ptr = pm_ldo_get_data(pmic_index);
        //return if the driver is not initialized
        if(ldo_ptr == NULL)
               return;

        ldo_data = pm_rpm_ldo_get_resource_data(pmic_index);
        if (ldo_data == NULL)
        {
            pm_malloc( sizeof(pm_npa_ldo_data_type), (void**)&ldo_data);

            ldo_data->ldoDriverData = ldo_ptr;
            ldo_data->resourceType = resourceType;
            ldo_data->depInfo = (pm_pwr_resource_dependency_info**)pm_target_information_get_specific_info(PM_PROP_LDO_DEP);
            CORE_VERIFY_PTR(ldo_data->depInfo);
            ldo_data->railInfo = (pm_rpm_ldo_rail_info_type**)pm_target_information_get_specific_info(PM_PROP_LDO_RAIL);
            CORE_VERIFY_PTR(ldo_data->railInfo);
            CORE_VERIFY_PTR(ldo_data->railInfo[pmic_index]);

            //verify if the range can meet the LDO's min, max voltage limits
            for( ldo_index = 0; ldo_index < num_npa_resources ; ldo_index++)
            {
                 //skip over invalid peripherals
                 if(ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].periph_type == 0)
                    continue;

                 range = ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_range;
                 CORE_VERIFY(((ldo_data->railInfo[pmic_index][ldo_index].MinVoltage)*1000 >= ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_vset[range].RangeMin ) &&
                  ((ldo_data->railInfo[pmic_index][ldo_index].MaxVoltage)*1000 <= ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_vset[range].RangeMax));
            }
            pm_malloc(sizeof(boolean)*num_npa_resources, (void**)&(ldo_data->inBypass) );
            DALSYS_memset(ldo_data->inBypass, 0, sizeof(boolean)*num_npa_resources);
            ldo_data->settlingTimeInfo = (pm_pwr_settling_time_info_type*)pm_target_information_get_common_info(PM_PROP_LDO_SETTLING_TIME);
            ldo_data->ultSettlingTimeInfo = (pm_pwr_settling_time_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_LDO_SETTLING_TIME);
            pm_malloc(sizeof(uint64)*num_npa_resources, (void**)&(ldo_data->settlingStartTime));
            pm_malloc(sizeof(uint64)*num_npa_resources, (void**)&(ldo_data->settlingEndTime));

            rpm_register_resource(resourceType, num_npa_resources + 1, sizeof(pm_npa_ldo_int_rep), pm_rpm_ldo_translation, pm_rpm_ldo_apply, (void *)ldo_data);
            pm_rpm_ldo_req_data[pmic_index] = ldo_data;
        }

    }
}

void pm_rpm_ldo_register_resource_dependencies(rpm_resource_type resourceType, uint32 num_npa_resources, uint8 pmic_index)
{
    uint8 count = 0;
    pm_pwr_resource *dependency = NULL;
    pm_npa_ldo_int_rep *current_agg = NULL;
    pm_ldo_data_type* ldo_ptr = NULL;
    pm_volt_level_type vol = 0;
    pm_on_off_type en_status = PM_OFF;
    pm_pwr_resource_dependency_info *depInfo = NULL;
    pm_npa_ldo_data_type *ldo_data = NULL;

    if(num_npa_resources > 0)
    {
        //return if driver was not initialized
        if(pm_rpm_ldo_req_data[pmic_index] == NULL)
               return;

        ldo_data = pm_rpm_ldo_req_data[pmic_index];
        if(ldo_data->depInfo != NULL)
        {
            depInfo = ldo_data->depInfo[pmic_index];
        }

        ldo_ptr = ldo_data->ldoDriverData;
        CORE_VERIFY_PTR(ldo_ptr);
        for(count = 1; count < num_npa_resources + 1; count++)
        {
            rpm_get_aggregated_request_buffer(resourceType, count, (const void**)&current_agg);
            if(current_agg != NULL)
            {
                DALSYS_memset(current_agg, 0, sizeof(pm_npa_ldo_int_rep));

                pm_pwr_volt_level_status_alg(&ldo_ptr->pm_pwr_data, ldo_ptr->comm_ptr, count-1, &vol);
                current_agg->output_uvol   = vol;

                pm_pwr_sw_enable_status_alg(&ldo_ptr->pm_pwr_data, ldo_ptr->comm_ptr, count-1, &en_status);
                current_agg->sw_en = en_status;

                current_agg->ldo_sw_mode = 0x0F; // initialize it to some invalid setting in order to make sure that the first vote will be granted.
                current_agg->device_index = pmic_index;
                current_agg->resource_id = count; // 1 based.
            }
        }

        if(depInfo)
        {
            for(count = 1; count < num_npa_resources + 1; count++)
            {
                if(depInfo[count].parent_source_dependency != NULL)
                {
                    dependency = depInfo[count].parent_source_dependency;

                    // create the internal client
                    rpm_register_internal_client_type(dependency->resource_type, dependency->internal_resource_index, PM_RPM_NPA_INT_CLIENT_LDO_DEPENDENT, sizeof(pm_npa_ldo_int_rep));

                    depInfo[count].parent_source_dependency_client_handle =
                                 rpm_create_client(dependency->resource_type, dependency->internal_resource_index, PM_RPM_NPA_INT_CLIENT_LDO_DEPENDENT);
                }
            }
        }

        //Initialize droop detection
        pm_rpm_ldo_droop_detect_init(resourceType, num_npa_resources);
    }
}


pm_npa_ldo_data_type* pm_rpm_ldo_get_resource_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
          {
            return  pm_rpm_ldo_req_data[pmic_index];
          }

          return NULL;
}

void pm_rpm_ldo_settle(rpm_application_info *info)
{
    pm_npa_ldo_int_rep *ldo_int_rep =(pm_npa_ldo_int_rep *)info->current_aggregation;
    pm_npa_ldo_data_type *ldo_data = (pm_npa_ldo_data_type *)info->cb_data;
    uint8  internal_resource_index = (uint8)(info->id - 1);
    uint64 settle_start_time = ldo_data->settlingStartTime[internal_resource_index];
    uint64 settle_end_time = ldo_data->settlingEndTime[internal_resource_index];
    uint32 estimated_settling_time_us = 0;
    pm_ldo_data_type* ldo_ptr = NULL;
    uint8 pmic_index = GET_PMIC_INDEX_LDO(ldo_data->resourceType);
    boolean vreg_status = FALSE;
    

    if(settle_start_time != 0)
    {
        // calculate the estimated settling time in micro secs
        estimated_settling_time_us = pm_convert_timetick_to_time(settle_end_time - settle_start_time);

        // Get the driver data from the info call back data
        ldo_ptr = ldo_data->ldoDriverData;

        CORE_VERIFY_PTR(ldo_ptr);

        // poll VREG_OK
        vreg_status = pm_rpm_check_vreg_settle_status(settle_start_time, estimated_settling_time_us, &ldo_ptr->pm_pwr_data,
                                        ldo_ptr->comm_ptr, internal_resource_index,
                                        ldo_data->railInfo[pmic_index][info->id -1].SettlingErrorEnabled);

        // For stepper LDOs, poll for STEPPER_DONE also
        switch(ldo_ptr->pm_pwr_data.pwr_specific_info[internal_resource_index].periph_type)
        {
        case PM_HW_MODULE_LDO:
           switch(ldo_ptr->periph_subtype[internal_resource_index])
           {
           case PM_HW_MODULE_LDO_N600_STEPPER:
           case PM_HW_MODULE_LDO_N1200_STEPPER:
              // poll STEPPER_DONE
              pm_rpm_check_stepper_settle_status(settle_start_time, estimated_settling_time_us, &ldo_ptr->pm_pwr_data,
                                                 ldo_ptr->comm_ptr, internal_resource_index,
                                                 ldo_data->railInfo[pmic_index][info->id -1].SettlingErrorEnabled);
              break;
           default:
              break;
           }
           break;
        case PM_HW_MODULE_ULT_LDO:
           switch(ldo_ptr->periph_subtype[internal_resource_index])
           {
           case PM_HW_MODULE_ULT_LDO_N300_STEPPER:
           case PM_HW_MODULE_ULT_LDO_N600_STEPPER:
           case PM_HW_MODULE_ULT_LDO_N900_STEPPER:
           case PM_HW_MODULE_ULT_LDO_N1200_STEPPER:
              // poll STEPPER_DONE
              pm_rpm_check_stepper_settle_status(settle_start_time, estimated_settling_time_us, &ldo_ptr->pm_pwr_data,
                                                 ldo_ptr->comm_ptr, internal_resource_index,
                                                 ldo_data->railInfo[pmic_index][info->id -1].SettlingErrorEnabled);
              break;
           default:
              break;
           }
           break;
        default:
           break;
        }

            // Work-around for PM8916 (MSM8916/MSM8936/MSM8909) LDO11 voltage over-shoot issue seen */
            // Disable LDO11 if it's not settled after making sure we internally settle for safe time (~300us) 
            // LDO11 of the PM8916 which is susseptible to over current as it does not have OCP
            if ((PMIC_IS_PM8916 == pm_get_pmic_model(pmic_index)) && (10 == internal_resource_index)
                &&  (ldo_int_rep->sw_en == 1))
            {
                 if (FALSE == vreg_status)
                 {
                    //* Make sure the LDO is taken out of bypass before disabling the rail
                    if (ldo_data->inBypass[internal_resource_index] == 1)
                    {
                     pm_ldo_sw_mode(pmic_index, internal_resource_index, PM_SW_MODE_NPM);
                     ldo_data->inBypass[internal_resource_index] = 0;
                    }

                    //* Disable the LDO since VREG_OK is not set 
                    pm_pwr_sw_enable_alg(&ldo_ptr->pm_pwr_data, ldo_ptr->comm_ptr, internal_resource_index, PM_OFF);

                    //* Update the current aggregation sw_en bit 
                    ldo_int_rep->sw_en = 0;
                 }
            }

        // reset the settling time values
        info->settling_time = 0;
        ldo_data->settlingEndTime[internal_resource_index] = 0;
        ldo_data->settlingStartTime[internal_resource_index] = 0;
    }
}

void pm_rpm_ldo_droop_detect_init(rpm_resource_type resourceType, uint32 num_npa_resources)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_npa_ldo_data_type *ldo_data = NULL;
    pm_ldo_data_type *ldo_ptr = NULL;
    pm_npa_ldo_int_rep *current_agg = NULL;
    uint32 ldo_index = 0;
    uint32 periph_id = 0;
    uint32 isr_ctx = 0;
    uint32 pmic_index = GET_PMIC_INDEX_LDO(resourceType);

    if(num_npa_resources > 0)
    {
        ldo_data = pm_rpm_ldo_get_resource_data(pmic_index);

        CORE_VERIFY_PTR(ldo_data);
        CORE_VERIFY_PTR(ldo_data->ldoDriverData);
        CORE_VERIFY_PTR(ldo_data->railInfo);
        CORE_VERIFY_PTR(ldo_data->railInfo[pmic_index]);

        ldo_ptr = ldo_data->ldoDriverData;

        for(ldo_index = 0; ldo_index < num_npa_resources; ldo_index++)
        {
           if(ldo_data->railInfo[pmic_index][ldo_index].DroopDetection == 1)
           {
              if(ldo_ptr->pm_pwr_data.pwr_specific_info != NULL)
              {
                 periph_id = (ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].periph_base_address)>>8;
              }

              // save periph_id <0:15> slave_id <16:31> in isr_ctx
              isr_ctx = ((ldo_ptr->comm_ptr->slave_id & 0xFFFF)<<16) + (periph_id & 0xFF);

              // Configure the rail for droop detection
              err_flag |= pm_droop_detect_irq_config(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_index, isr_ctx);

              // Get the rail's current aggregation to check if the rail is enabled
              rpm_get_aggregated_request_buffer(resourceType, (ldo_index + 1), (const void**)&current_agg);

              if((current_agg != NULL) && (current_agg->sw_en == 1))
              {
                 // Enable the droop detection interrupt only if the rail is enabled
                 err_flag |= pm_droop_detect_irq_enable(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_index, TRUE);
              }
              CORE_VERIFY(err_flag == PM_ERR_FLAG__SUCCESS);
           }
        }
    }
}

void pm_rpm_ldo_droop_detect_irq_enable(rpm_application_info *info, boolean enable)
{
    pm_npa_ldo_data_type *ldo_data = (pm_npa_ldo_data_type*)info->cb_data;
    pm_npa_ldo_int_rep *ldo_curr_agg =(pm_npa_ldo_int_rep*)info->current_aggregation;
    uint32 ldo_index = (info->id - 1);
    pm_ldo_data_type *ldo_ptr = NULL;
    uint8 pmic_index = GET_PMIC_INDEX_LDO(ldo_data->resourceType);

    // Check if Droop detection is ON for the rail
    if(ldo_data->railInfo[pmic_index][ldo_index].DroopDetection == 1)
    {
       // Get the driver data from the info call back data
       ldo_ptr = ldo_data->ldoDriverData;

       if(ldo_ptr != NULL)
       {
          if(enable == TRUE)
          {
             // Enable droop detection interrupt only if the rail is enabled
             if(ldo_curr_agg->sw_en == 1)
             {
                pm_droop_detect_irq_enable(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_index, TRUE);
             }
          }
          else
          {
             // Disable droop detection interrupt
             pm_droop_detect_irq_enable(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_index, FALSE);
          }
       }
    }
}


