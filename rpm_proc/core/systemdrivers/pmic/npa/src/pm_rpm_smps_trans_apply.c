/*! \file pm_rpm_smps_trans_apply.c
 *  \n
 *  \brief This file contains implementation for PMIC SMPS Translation
 *         and Apply (Aggregate) functions.
 *  \n
 *  \n &copy; Copyright 2012-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/npa/src/pm_rpm_smps_trans_apply.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/15   mr      Removed Boost, Boost Bypass and VS related code (CR-814301)
06/11/13   hs      Adding settling time for regulators.
05/17/13   hs      PMIC SWEVENT revise.
05/09/13   hs      Parent does not inherit “pin control mode” from child
04/25/13   aks     Code Refactoring: Removing PMIC_SubRsc from pwr algs
04/12/13   hs      Code refactoring.
02/27/13   hs      Code refactoring.
02/01/12   hs      Added support for input validation.
01/29/13   aks     Adding support for Boost as separate peripheral
08/07/12   hs      Added support for 5V boost.
07/20/12   hs      Added settling time.
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

#include "pm_rpm_smps_trans_apply.h"
#include "pm_npa_device_smps.h"
#include "pm_rpm_ldo_trans_apply.h"
#include "pm_droop_detect.h"
#include "pm_target_information.h"
#include "CoreVerify.h"
#include "device_info.h"
#include "pm_config.h"


/*===========================================================================

                SMPS TRANS APPLY FUNCTIONS DEFINITION

===========================================================================*/
static pm_npa_smps_data_type *pm_rpm_smps_req_data[PM_MAX_NUM_PMICS];

static boolean ldo_pending_bypass_transition = FALSE;

// internal function prototypes
void pm_rpm_smps_translation(rpm_translation_info *info);
void pm_rpm_smps_apply(rpm_application_info *info);
void pm_rpm_smps_pre_dependency_analysis(rpm_application_info *info, pm_npa_smps_int_rep* previous_aggregation);
void pm_rpm_smps_post_dependency_analysis(rpm_application_info *info, pm_npa_smps_int_rep* previous_aggregation);
int pm_rpm_smps_aggregate(rpm_application_info *info);
void pm_rpm_smps_execute_driver(rpm_application_info *info, pm_npa_smps_int_rep* previous_aggregation);
void pm_rpm_smps_check_ldo_bypass(rpm_application_info* info, boolean out_bypass);
void pm_rpm_smps_settle(rpm_application_info *info);
static void pm_rpm_smps_aggregate_dependent_ldo_input_voltage(uint32 v_ldo, uint32 v_reg, uint32* v_low, uint32* v_high);

static void pm_rpm_smps_droop_detect_init(rpm_resource_type resourceType, uint32 num_npa_resources);
static void pm_rpm_smps_droop_detect_irq_enable(rpm_application_info *info, boolean enable);

void pm_rpm_smps_translation(rpm_translation_info *info)
{
    unsigned type, length, *value;
    pm_npa_smps_int_rep *cache;
    pm_npa_smps_int_rep SAWCache;
    pm_npa_smps_int_rep previous_aggregation;
    rpm_application_info executeInfo;
    unsigned isSawCtrl = 0;
    pm_npa_smps_data_type* resourceInfo = (pm_npa_smps_data_type*)info->cb_data;
    uint8 pmic_index = 0;
    pm_smps_data_type *smps_ptr = NULL;

    CORE_VERIFY_PTR(resourceInfo);
    smps_ptr = resourceInfo->smpsDriverData;
    CORE_VERIFY_PTR(smps_ptr);

    // Return for invalid PMIC resource ids. PMIC resource id 0 is invalid.
    if((info->id == 0) || (info->id > smps_ptr->pm_pwr_data.num_of_peripherals))
    {
        return;
    }

    pmic_index = GET_PMIC_INDEX_SMPS(resourceInfo->resourceType);
    CORE_VERIFY(pmic_index < PM_MAX_NUM_PMICS);

    isSawCtrl = resourceInfo->railInfo[pmic_index][info->id-1].IsSawControlled;

    if(isSawCtrl)
    {
        DALSYS_memset(&SAWCache, 0, sizeof(pm_npa_smps_int_rep));
        cache = &SAWCache;
    }
    else
    {
        cache = (pm_npa_smps_int_rep *)(info->dest_buffer);
    }

    cache->global_byp_en = 1;
    cache->reserve1 = 0;

    while(kvp_get(info->new_kvps, &type, &length, (const char **)&value))
    {
        // Need size to match.
        if(sizeof(npa_resource_state) != length)
            continue;

        switch(type)
        {
        case PM_NPA_KEY_SOFTWARE_ENABLE:
            {
                cache->sw_en = *value;

                break;
            }
        case PM_NPA_KEY_SMPS_SOFTWARE_MODE:
            {
                cache->smps_sw_mode = *value;
                break;
            }
        case PM_NPA_KEY_PIN_CTRL_ENABLE:
            {
                cache->pc_en = *value;
                break;
            }
        case PM_NPA_KEY_PIN_CTRL_POWER_MODE:
            {
                cache->pc_mode = *value;
                break;
            }
        case PM_NPA_KEY_MICRO_VOLT:
            {
                cache->uvol = *value;
                break;
            }
        case PM_NPA_KEY_CURRENT:
            {
                cache->ip = *value;
                break;
            }
        case PM_NPA_KEY_FREQUENCY:
            {
                cache->freq = *value;
                break;
            }
        case PM_NPA_KEY_FREQUENCY_REASON:
            {
                cache->freq_reason = *value;
                break;
            }
        case PM_NPA_KEY_FOLLOW_QUIET_MODE:
            {
                cache->quiet_mode = *value;
                break;
            }
        case PM_NPA_KEY_HEAD_ROOM:
            {
                cache->hr = *value;
                break;
            }
        case PM_NPA_KEY_BYPASS_ALLOWED_KEY:
            {
                cache->byp_allowed = *value;
                break;
            }
       default:
            {
                //should never go here
            }
        }
    }

    if(isSawCtrl)
    {
        executeInfo.cb_data = info->cb_data;
        executeInfo.client  = info->client;
        executeInfo.client_type = info->client_type;
        executeInfo.current_aggregation = cache;
        executeInfo.id = info->id;

        previous_aggregation = *cache;

        previous_aggregation.sw_en = !cache->sw_en; // to make sure this field will be excuted
        previous_aggregation.uvol = cache->uvol - 1; // to make sure this field will be excuted

        pm_rpm_smps_dependency_execute(&executeInfo, &previous_aggregation);
    }
}

void pm_rpm_smps_apply(rpm_application_info *info)
{
    int                    change_detected = 0;
    pm_npa_smps_int_rep    previous_aggregation = *((pm_npa_smps_int_rep*)(info->current_aggregation));
    uint64                 current_time = 0;
    uint64                 settle_start_time = 0;
    uint64                 settle_end_time = 0;
    uint32                 min_settling_time_us = 0;
    uint32                 settling_time_elapsed_us = 0;
    uint32                 settling_time_remaining_us = 0;
    unsigned               isSawCtrl = 0;
    boolean                vreg_status = FALSE;
    boolean                stepper_status = FALSE;
    uint8                  internal_resource_index = 0;
    uint8                  pmic_index = 0;
    pm_npa_smps_data_type *smps_data = (pm_npa_smps_data_type *)info->cb_data;
    pm_smps_data_type     *smps_ptr = NULL;
    pm_npa_smps_int_rep   *curr_aggr = NULL;

    CORE_VERIFY_PTR(smps_data);

    smps_ptr = smps_data->smpsDriverData;

    CORE_VERIFY_PTR(smps_ptr);

    // Return for invalid PMIC resource ids. PMIC resource id 0 is invalid.
    if((info->id == 0) || (info->id > smps_ptr->pm_pwr_data.num_of_peripherals))
    {
        return;
    }

    // PMIC resource id starts from 1 and our internal resource index starts from 0
    internal_resource_index = (uint8)(info->id - 1);

    pmic_index = GET_PMIC_INDEX_SMPS(smps_data->resourceType);
    CORE_VERIFY(pmic_index < PM_MAX_NUM_PMICS);

    isSawCtrl = smps_data->railInfo[pmic_index][internal_resource_index].IsSawControlled;

    if(!isSawCtrl)
    {
        if(0 == info->settling_time) // this is a new request
        {
            // Aggregate the results, but don't call driver
            change_detected = pm_rpm_smps_aggregate(info);

            if(change_detected != 0 )
            {
                pm_rpm_smps_dependency_execute(info, &previous_aggregation);
            }
            else if(ldo_pending_bypass_transition == TRUE)
            {
                ldo_pending_bypass_transition = FALSE;
                pm_rpm_smps_check_ldo_bypass(info, FALSE);
            }
        }
        else // this is an old request to check if the rail has settled
        {
            // Check the current aggregation to see if the rail got disabled while settling parallely
            curr_aggr = (pm_npa_smps_int_rep*)info->current_aggregation;

            if(curr_aggr->sw_en != 0)
            {
               settle_start_time = smps_data->settlingStartTime[internal_resource_index];
               settle_end_time = smps_data->settlingEndTime[internal_resource_index];

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
                  min_settling_time_us = smps_data->railInfo[pmic_index][internal_resource_index].MinSettlingTime;
               }

               // if minimum settling time has elapsed, only then check for VREG_OK ow call back to RPM server
               if(settling_time_elapsed_us >= min_settling_time_us)
               {
                  // Check for VREG_OK and STEPPER_DONE status
                  pm_pwr_sw_enable_vreg_ok_status_alg(&smps_ptr->pm_pwr_data, smps_ptr->comm_ptr,
                                                      internal_resource_index, &vreg_status);

                  pm_pwr_volt_level_stepper_done_status_alg(&smps_ptr->pm_pwr_data, smps_ptr->comm_ptr,
                                                            internal_resource_index, &stepper_status);

                  if((vreg_status == FALSE) || (stepper_status == FALSE))
                  {
                     // if the rail finished settling due to a different request and still
                     // VREG_OK is not high then give the rail some more time to settle internally
                     if(settle_start_time == 0)
                     {
                        // save the current time as settling start time
                        smps_data->settlingStartTime[internal_resource_index] = current_time;

                        // calculate the settle end time using the settling time threshold
                        smps_data->settlingEndTime[internal_resource_index] =
                                   current_time + pm_convert_time_to_timetick(PM_RPM_SETTTLING_TIME_THRESHOLD);

                        // poll for VREG_OK to continue settling
                        pm_rpm_smps_settle(info);
                     }
                     else if(current_time < settle_end_time) // still need more time to settle
                     {
                        // calculate the remaining settling time in micro secs
                        settling_time_remaining_us = pm_convert_timetick_to_time(settle_end_time - current_time);

                        // if the remaining settling time is small enough, wait internally
                        if(settling_time_remaining_us < PM_RPM_SETTTLING_TIME_THRESHOLD)
                        {
                           // poll for VREG_OK to continue settling
                           pm_rpm_smps_settle(info);
                        }
                     }
                     else // estimated settling time elapsed, rail should have already settled
                     {
                        // poll for VREG_OK to continue settling
                        pm_rpm_smps_settle(info);
                     }
                  }
                  else // settling is done since VREG_OK status is TRUE
                  {
                     // reset the settling time values
                     info->settling_time = 0;
                     smps_data->settlingEndTime[internal_resource_index] = 0;
                     smps_data->settlingStartTime[internal_resource_index] = 0;
                  }
               }
            }
            else // reset the settling time if the rail got disabled while settling parallely
            {
               // reset the settling time values
               info->settling_time = 0;
               smps_data->settlingEndTime[internal_resource_index] = 0;
               smps_data->settlingStartTime[internal_resource_index] = 0;
            }

            if(info->settling_time == 0)
            {
               // Check if the rail's droop detection interrupt has to be re-enabled
               // after the rail has finished parallel settling
               pm_rpm_smps_droop_detect_irq_enable(info, TRUE);

              // HF OCP issue work-around - Disable OCP while enabling SMPS HF rail and enable back OCP after the
              // rail is enabled and settled
              if(smps_ptr->pm_pwr_data.pwr_specific_info[internal_resource_index].periph_type == PM_HW_MODULE_HF_BUCK)
              {
                 pm_smps_ocp_enable(pmic_index, internal_resource_index, TRUE);
              }
            }

        } // end if(0 == info->settling_time)
    }// end if(!isSawCtrl)
}

void pm_rpm_smps_dependency_execute(rpm_application_info *info, pm_npa_smps_int_rep* previous_aggregation)
{
    // Check the pre-operational dependencies
    pm_rpm_smps_pre_dependency_analysis(info, previous_aggregation);

    // Execute the driver calls once all dependencies have been executed
    pm_rpm_smps_execute_driver(info, previous_aggregation);

    // Check the post operational dependencies
    pm_rpm_smps_post_dependency_analysis(info, previous_aggregation);
}

int pm_rpm_smps_aggregate(rpm_application_info *info)
{
    int change_detected = 0;
    uint32 v_low = 0;
    uint32 v_high = 0;
    pm_npa_smps_data_type* smps_data = (pm_npa_smps_data_type*)info->cb_data;
    unsigned clientType, numberOfClients, clientCount;
    pm_npa_smps_int_rep* smpsClientRequest;
    pm_npa_ldo_int_rep* ldoClientRequest;
    pm_npa_vs_int_rep* vsClientRequest;
    void* state = NULL;
    pm_npa_smps_int_rep aggregatedIntRep = {0};
    pm_smps_data_type* smps_ptr = smps_data->smpsDriverData;
    uint8 pmic_index = GET_PMIC_INDEX_SMPS(smps_data->resourceType);
    pm_volt_level_type ceiling_voltage = 0; //voltage rounded to next high
    uint32             dummy_vset = 0;

    numberOfClients = rpm_get_num_clients(info->resource_handle);

    // For each client aggregate the correct values.
    aggregatedIntRep.global_byp_en = 1;

    for(clientCount = 0; clientCount < numberOfClients; clientCount++)
    {
        rpm_get_current_request(info->resource_handle, clientCount, &clientType, (const void **)&state);

         // EE Client and Internal SMPS clients
        if( (clientType == 0) || (clientType == PM_RPM_NPA_CLIENT_SMPS_REQ) )
        {
            if(smps_data->railInfo[pmic_index][info->id -1].AccessAllowed)
            {
                smpsClientRequest = (pm_npa_smps_int_rep*)state;

                if(smpsClientRequest != NULL)
                {
                    // Aggregate Software Enable
                    aggregatedIntRep.sw_en = MAX(aggregatedIntRep.sw_en, smpsClientRequest->sw_en);

                    // Aggregate Software Mode Enable
                    aggregatedIntRep.smps_sw_mode = MAX(aggregatedIntRep.smps_sw_mode,
                                                        smpsClientRequest->smps_sw_mode);

                    // Aggregate ORed values of Pin Control Enable
                    aggregatedIntRep.pc_en = aggregatedIntRep.pc_en | smpsClientRequest->pc_en;

                    // Aggregate ORed values of Pin Control Power Mode
                    aggregatedIntRep.pc_mode = aggregatedIntRep.pc_mode | smpsClientRequest->pc_mode;

                    // Aggregate Maximum Voltage
                    aggregatedIntRep.uvol = MAX(aggregatedIntRep.uvol, smpsClientRequest->uvol);

                    // Aggregate Summation of Current
                    aggregatedIntRep.ip = aggregatedIntRep.ip + smpsClientRequest->ip;

                    // The following line will only be true if there is a frequency reason
                    // other than "PM_NPA_FREQ_REASON_NONE"
                    if(smpsClientRequest->freq_reason > aggregatedIntRep.freq_reason)
                    {
                        // If the frequency reason takes priority then set the frequency
                        // to what ever the client needs.
                        aggregatedIntRep.freq_reason = smpsClientRequest->freq_reason;
                        aggregatedIntRep.freq = smpsClientRequest->freq;
                    }
                    else
                    {
                        // If the frequency reason is NONE, then a simple max frequency aggregation works
                        if(aggregatedIntRep.freq_reason == PM_NPA_FREQ_REASON_NONE)
                        {
                            // Aggregate Maximum Frequency, the enum sequence is from the lowest to highest since the HW is mapped so
                            if(smpsClientRequest->freq != 0 )
                            {
                                if( aggregatedIntRep.freq == 0 )// if the freq is 0, this means it is the first aggregation.
                                {
                                    aggregatedIntRep.freq  = smpsClientRequest->freq;
                                }
                                else
                                {
                                    aggregatedIntRep.freq  = MIN(aggregatedIntRep.freq,       smpsClientRequest->freq);
                                }
                            }
                        }
                    }

                    // Aggregate Quiet Mode
                    aggregatedIntRep.quiet_mode = MAX(aggregatedIntRep.quiet_mode, smpsClientRequest->quiet_mode);

                    // Aggregate Headroom Voltage
                    aggregatedIntRep.hr = MAX(aggregatedIntRep.hr, smpsClientRequest->hr);

                    // Aggregate Bypass Allowed
                    aggregatedIntRep.byp_allowed = MAX(aggregatedIntRep.byp_allowed, smpsClientRequest->byp_allowed);
                }
            }
        }
        else if(clientType == PM_RPM_NPA_INT_CLIENT_LDO_DEPENDENT)
        {
            ldoClientRequest = (pm_npa_ldo_int_rep*)state;

            // A valid internal LDO client should have resource_id greater than zero
            if(ldoClientRequest->resource_id > 0)
            {
                // Aggregate Software Enable - Child needs power
                aggregatedIntRep.sw_en = MAX(aggregatedIntRep.sw_en, ldoClientRequest->sw_en);

                // TODO: Does it make sense to aggregate pass the pin control to the parent?
                // Aggregate ORed values of Pin Control Enable
                aggregatedIntRep.pc_en = aggregatedIntRep.pc_en | ldoClientRequest->pc_en;

                // Ignoring voltage and peak current request if ldo is sw and pinctrl disabled
                if((ldoClientRequest->sw_en > 0) || (ldoClientRequest->pc_en > 0))
                {
                   // ldoClientRequest->input_uvol has headroom accounted for.
                    if((ldoClientRequest->noise_hr > 0) || (ldoClientRequest->byp_allowed == PM_NPA_BYPASS_DISALLOWED))
                    {
                       //if the ldo cannot be put into bypass, for the aggregation algorithm
                       //the output voltage becomes the same as input voltage
                       pm_rpm_smps_aggregate_dependent_ldo_input_voltage(ldoClientRequest->input_uvol,
                                                   ldoClientRequest->input_uvol,&v_low,&v_high);
                    }
                    else
                    {
                       pm_rpm_smps_aggregate_dependent_ldo_input_voltage(ldoClientRequest->output_uvol,
                                                   ldoClientRequest->input_uvol,&v_low,&v_high);
                    }

                   // Aggregate Summation of Current
                   aggregatedIntRep.ip             = aggregatedIntRep.ip + ldoClientRequest->ip;
                }
            } //resource_id>0
        }
        else if(clientType == PM_RPM_NPA_INT_CLIENT_VS_DEPENDENT)
        {
            vsClientRequest = (pm_npa_vs_int_rep*)state;

            // Aggregate Software Enable - Child needs power
            aggregatedIntRep.sw_en = MAX(aggregatedIntRep.sw_en, vsClientRequest->sw_en);

            // TODO: ask Chris about how to associate VS setting with its parent, ie. SMPS.
            aggregatedIntRep.pc_en = aggregatedIntRep.pc_en | vsClientRequest->pc_en;

            // Aggregate Summation of Current
            aggregatedIntRep.ip    = aggregatedIntRep.ip + vsClientRequest->ip;
        }
    }

    aggregatedIntRep.uvol = MAX(aggregatedIntRep.uvol, v_low);

    // input checking after aggregation.

    if(smps_data->railInfo[pmic_index][info->id - 1].AlwaysOn)
    {
        aggregatedIntRep.sw_en = 1;
    }
    if(aggregatedIntRep.smps_sw_mode < smps_data->railInfo[pmic_index][info->id - 1].MinPwrMode)
    {
        aggregatedIntRep.smps_sw_mode = smps_data->railInfo[pmic_index][info->id - 1].MinPwrMode;
    }

    if( (aggregatedIntRep.uvol < smps_data->railInfo[pmic_index][info->id - 1].MinVoltage*1000) && (aggregatedIntRep.uvol > 0) )
    {
        aggregatedIntRep.uvol = smps_data->railInfo[pmic_index][info->id - 1].MinVoltage*1000;
    }
    else if(aggregatedIntRep.uvol > smps_data->railInfo[pmic_index][info->id - 1].MaxVoltage*1000)
    {
        aggregatedIntRep.uvol = smps_data->railInfo[pmic_index][info->id - 1].MaxVoltage*1000;
    }

    //NOTE: the enum values for frequency are revered, i.e. PM_CLK_9p6_MHz is smaller than PM_CLK_6p4_MHz
    if((aggregatedIntRep.freq < smps_data->railInfo[pmic_index][info->id - 1].MaxFreq) && (aggregatedIntRep.freq != 0))
    {
        aggregatedIntRep.freq = smps_data->railInfo[pmic_index][info->id - 1].MaxFreq;
    }
    else if((aggregatedIntRep.freq > smps_data->railInfo[pmic_index][info->id - 1].MinFreq) && (aggregatedIntRep.freq != 0))
    {
        aggregatedIntRep.freq = smps_data->railInfo[pmic_index][info->id - 1].MinFreq;
    }

    if(aggregatedIntRep.uvol != 0)
    {
       //check if rounding off vset is needed
       pm_pwr_volt_calculate_vset_celing_uv(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, info->id - 1,
                                            (pm_volt_level_type)aggregatedIntRep.uvol, &dummy_vset, &ceiling_voltage);
    }

    if(ceiling_voltage != 0)
    {
        aggregatedIntRep.uvol =  ceiling_voltage;
    }

    change_detected = pm_rpm_int_copy( &aggregatedIntRep, (pm_npa_smps_int_rep *)info->current_aggregation, sizeof(pm_npa_smps_int_rep));

    return change_detected;
}

void pm_rpm_smps_pre_dependency_analysis(rpm_application_info *info, pm_npa_smps_int_rep* previous_aggregation)
{
    pm_pwr_resource_dependency_info *depInfo = NULL;
    pm_npa_smps_int_rep *cur_data = (pm_npa_smps_int_rep*)info->current_aggregation;
    pm_npa_smps_data_type *cb_data = (pm_npa_smps_data_type*)info->cb_data;
    uint8 pmic_index = GET_PMIC_INDEX_SMPS(cb_data->resourceType);

    if(cb_data->depInfo[pmic_index] != NULL)
    {
        depInfo = &cb_data->depInfo[pmic_index][info->id];
    }

    if((depInfo != NULL) && (depInfo->child_resource_dependents != NULL))
    {
        // take all the children LDOs out of bypass if needed.
        if( (cur_data->global_byp_en == 0) ||         // a child LDO just about to turn on
            (cur_data->sw_en != previous_aggregation->sw_en)|| //the parent is about to be from on->off or off->on
            (cur_data->uvol != previous_aggregation->uvol) )    // the parent's voltage is about to increase.
        {
            pm_rpm_smps_check_ldo_bypass(info, TRUE);
        }
    }

    if ((depInfo != NULL) && (depInfo->parent_source_dependency != NULL))
    {
        // Detect if this is a pre-operation change on the parent
        // These conditions are
        // If the required headroom is increased
        // If the voltage setting is increased
        // If the SMPS goes from a disabled to an enabled state
        // If the SMPS Pin Control goes from disabled to an enabled state
        if( (cur_data->hr > previous_aggregation->hr) ||
            (cur_data->uvol > previous_aggregation->uvol) ||
            (cur_data->sw_en > previous_aggregation->sw_en) ||
            (cur_data->pc_en > previous_aggregation->pc_en))
        {
            rpm_issue_request(depInfo->parent_source_dependency->resource_type,
                depInfo->parent_source_dependency->internal_resource_index,
                depInfo->parent_source_dependency_client_handle,
                sizeof(pm_npa_smps_int_rep), info->current_aggregation);
        }
    }
}

void pm_rpm_smps_post_dependency_analysis(rpm_application_info *info, pm_npa_smps_int_rep* previous_aggregation)
{
    pm_pwr_resource_dependency_info *depInfo = NULL;
    pm_npa_smps_int_rep *cur_data = (pm_npa_smps_int_rep*)info->current_aggregation;
    pm_npa_smps_data_type *cb_data = (pm_npa_smps_data_type*)info->cb_data;
    uint8 change_detected = 0;
    uint8 pmic_index = GET_PMIC_INDEX_SMPS(cb_data->resourceType);

    if(cb_data->depInfo[pmic_index] != NULL)
    {
        depInfo = &cb_data->depInfo[pmic_index][info->id];
    }

    if((depInfo != NULL) && (depInfo->child_resource_dependents != NULL))
    {
        if(cur_data->global_byp_en == 1)
        {
            pm_rpm_smps_check_ldo_bypass(info, FALSE);
        }
    }

    if ((depInfo != NULL) && (depInfo->parent_source_dependency != NULL))
    {
        // Detect if this is a post-operation change on the parent
        // These conditions are
        // If the required headroom is decreased
        // If the voltage setting is decreased
        // If the SMPS goes from a enabled to an disabled state
        // If the SMPS Pin Control goes from enabled to an disabled state
        if( (cur_data->hr < previous_aggregation->hr) ||
            (cur_data->uvol < previous_aggregation->uvol) ||
            (cur_data->sw_en < previous_aggregation->sw_en) ||
            (cur_data->pc_en < previous_aggregation->pc_en) )
        {
            change_detected = 1;
        }

        if(1== change_detected )
        {
            rpm_issue_request(depInfo->parent_source_dependency->resource_type,
                depInfo->parent_source_dependency->internal_resource_index,
                depInfo->parent_source_dependency_client_handle,
                sizeof(pm_npa_smps_int_rep), info->current_aggregation);
        }
    }
}

void pm_rpm_smps_execute_driver(rpm_application_info *info, pm_npa_smps_int_rep* previous_aggregation)
{
    pm_npa_smps_int_rep *smps_int_rep =(pm_npa_smps_int_rep *)info->current_aggregation;
    pm_npa_smps_data_type *smps_data = (pm_npa_smps_data_type *)info->cb_data;
    pm_smps_data_type* smps_ptr = NULL;
    uint64 curr_settle_start_time = 0;
    uint64 prev_settle_start_time = 0;
    uint64 curr_settle_end_time = 0;
    uint64 prev_settle_end_time = 0;
    uint32 estimated_settling_time_us = 0;
    uint32 min_settling_time_us = 0;
    uint32 stepper_uv = 0;
    uint32 stepper_us = 0;
    uint8 internal_resource_index = (uint8)(info->id - 1);
    uint8 pmic_index = GET_PMIC_INDEX_SMPS(smps_data->resourceType);
    uint8 periph_type = 0;

    smps_ptr = smps_data->smpsDriverData;


    if(smps_ptr != NULL)
    {
        /* Get SMPS peripheral type */
        periph_type = smps_ptr->pm_pwr_data.pwr_specific_info[internal_resource_index].periph_type;

        /* Set Power Mode */
        if( (smps_int_rep->smps_sw_mode == PM_NPA_SW_MODE_SMPS__AUTO) && (smps_int_rep->smps_sw_mode != previous_aggregation->smps_sw_mode) )
        {
            pm_smps_sw_mode(pmic_index, internal_resource_index, PM_SW_MODE_AUTO);
        }
        else if(smps_int_rep->smps_sw_mode == PM_NPA_SW_MODE_SMPS__IPEAK)
        {
            if( (smps_int_rep->smps_sw_mode != previous_aggregation->smps_sw_mode) || (smps_int_rep->ip != previous_aggregation->ip) )
            {
                unsigned lp_hp_current_threshold;
                lp_hp_current_threshold = smps_data->railInfo[pmic_index][internal_resource_index].LpHpCurrentThreshold;

                if(smps_int_rep->ip >= lp_hp_current_threshold)
                {
                    pm_smps_sw_mode(pmic_index, internal_resource_index, PM_SW_MODE_NPM);
                }
                else
                {
                    pm_smps_sw_mode(pmic_index, internal_resource_index, PM_SW_MODE_LPM);
                }
            }
        }
        else
        {
            if(smps_int_rep->smps_sw_mode != previous_aggregation->smps_sw_mode)
            {
                pm_smps_sw_mode(pmic_index, internal_resource_index, PM_SW_MODE_NPM);
            }
        }

        /* Set PIN Control Enable */
        if(smps_int_rep->pc_en != previous_aggregation->pc_en)
        {
            pm_pwr_pin_ctrl_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, internal_resource_index, (uint8)smps_int_rep->pc_en);
        }

        /* Set PIN Control Power Mode */
        if(smps_int_rep->pc_mode != previous_aggregation->pc_mode)
        {
            pm_pwr_pin_ctrl_mode_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, internal_resource_index, (uint8)smps_int_rep->pc_mode);
        }

        /* Set the switch Frequency */
        if((smps_int_rep->freq != previous_aggregation->freq) && (smps_int_rep->freq != 0))
        {
            pm_smps_switching_freq(pmic_index, internal_resource_index, (pm_smps_switching_freq_type)smps_int_rep->freq);
        }

        if(smps_int_rep->quiet_mode != previous_aggregation->quiet_mode)
        {
            pm_smps_quiet_mode(pmic_index, internal_resource_index, (pm_quiet_mode_type)smps_int_rep->quiet_mode, 250);//TODO need to update the structure in order to set the QMODE_PS_VRST.
        }

        /* Set the Voltage */
        if((smps_int_rep->uvol != previous_aggregation->uvol) && (smps_int_rep->uvol != 0))
        {
            // Check if the rail's droop detection interrupt has to be disabled
            pm_rpm_smps_droop_detect_irq_enable(info, FALSE);

            pm_pwr_volt_level_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, internal_resource_index, (pm_volt_level_type)smps_int_rep->uvol);

            if(smps_ptr->pm_pwr_data.pwr_specific_info[internal_resource_index].periph_type == PM_HW_MODULE_ULT_BUCK)
            {
               stepper_uv = smps_data->ultSettlingTimeInfo->stepper_uv;
               stepper_us = smps_data->ultSettlingTimeInfo->stepper_us;
            }
            else
            {
               stepper_uv = smps_data->settlingTimeInfo->stepper_uv;
               stepper_us = smps_data->settlingTimeInfo->stepper_us;
            }

            // voltage changed, adding settling time.
            if(smps_int_rep->uvol > previous_aggregation->uvol)
            {
                //calculate the estimated settling time
                estimated_settling_time_us = ((smps_int_rep->uvol - previous_aggregation->uvol)/stepper_uv)*stepper_us;
            }
            else
            {
                //calculate the estimated settling time
                estimated_settling_time_us = ((previous_aggregation->uvol - smps_int_rep->uvol)/stepper_uv)*stepper_us;
            }
        }

        /* State set */
        if(smps_int_rep->sw_en != previous_aggregation->sw_en)
        {
            // Check if the rail's droop detection interrupt has to be disabled
            pm_rpm_smps_droop_detect_irq_enable(info, FALSE);

            // HF OCP issue work-around - Disable OCP while enabling SMPS HF rail and enable back OCP after the
            // rail is enabled and settled
            if((periph_type == PM_HW_MODULE_HF_BUCK) && (smps_int_rep->sw_en > previous_aggregation->sw_en))
            {
                pm_smps_ocp_enable(pmic_index, internal_resource_index, FALSE);
            }

            pm_pwr_sw_enable_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, internal_resource_index,(pm_on_off_type)smps_int_rep->sw_en);

            // rail turns on, adding settling time.
            if(smps_int_rep->sw_en > previous_aggregation->sw_en)
            {
               // get the minimum settling time from config if rail is enabling
               min_settling_time_us = smps_data->railInfo[pmic_index][internal_resource_index].MinSettlingTime;

               // calculate the estimated settling time
               if(smps_ptr->pm_pwr_data.pwr_specific_info[internal_resource_index].periph_type == PM_HW_MODULE_ULT_BUCK)
               {
                  // calculate the estimated settling time
                  estimated_settling_time_us = MAX(smps_data->settlingTimeInfo->vreg_ok_us, min_settling_time_us);
               }
               else
               {
                  // calculate the estimated settling time
                  estimated_settling_time_us = MAX(smps_data->settlingTimeInfo->vreg_ok_us, min_settling_time_us);
               }
            }
        }

        // -------------- Settling time ------------------

        // Get the previous settle start time to check if rail is already settling
        prev_settle_start_time = smps_data->settlingStartTime[internal_resource_index];

        // Check if rail has to settle
        if(((estimated_settling_time_us != 0) || (prev_settle_start_time != 0)) &&
           (smps_int_rep->sw_en != 0) && (smps_data->railInfo[pmic_index][internal_resource_index].SettlingEnabled == PM_SETTLING_EN))
        {
           // get the current settle start time
           curr_settle_start_time = time_service_now();

           // if the rail is not already settling
           if(prev_settle_start_time == 0)
           {
              // Update the current settling start time
              smps_data->settlingStartTime[internal_resource_index] = curr_settle_start_time;

              // calculate the current settle end time
              curr_settle_end_time = curr_settle_start_time + pm_convert_time_to_timetick(estimated_settling_time_us);
           }
           else // rail is already settling
           {
              // Get the previous settle end time
              prev_settle_end_time = smps_data->settlingEndTime[internal_resource_index];

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
           smps_data->settlingEndTime[internal_resource_index] = curr_settle_end_time;
           info->settling_time = curr_settle_end_time;

           // if the estimated settling time is less than threshold or client is internal, settle internally
           if((estimated_settling_time_us < PM_RPM_SETTTLING_TIME_THRESHOLD) ||
              (info->client_type == PM_RPM_NPA_CLIENT_SMPS_REQ) ||
              (info->client_type == PM_RPM_NPA_INT_CLIENT_LDO_DEPENDENT) ||
              (info->client_type == PM_RPM_NPA_INT_CLIENT_VS_DEPENDENT))
           {
              // busywait for minimum settling time if any, updated only for enabling rails
              if(min_settling_time_us != 0)
              {
                 DALSYS_BusyWait(min_settling_time_us);
              }

              // internal settling
              pm_rpm_smps_settle(info);

              // Check if the rail's droop detection interrupt has to be re-enabled
              // after the rail has settled internally
              pm_rpm_smps_droop_detect_irq_enable(info, TRUE);

              // HF OCP issue work-around - Disable OCP while enabling SMPS HF rail and enable back OCP after the
              // rail is enabled and settled
              if((periph_type == PM_HW_MODULE_HF_BUCK) && (smps_int_rep->sw_en > previous_aggregation->sw_en))
              {
                 pm_smps_ocp_enable(pmic_index, internal_resource_index, TRUE);
              }
           }
        }

        // -------------- Settling time ------------------
    }
}

void pm_rpm_smps_register_resources(rpm_resource_type resourceType, uint32 num_npa_resources , uint8 pmic_index)
{
    pm_npa_smps_data_type *smps_data = NULL;
    uint32 smps_index = 0;
    pm_smps_data_type *smps_ptr = NULL;
    uint8 range = 0;

    if(num_npa_resources > 0)
    {

        //return if there was no driver initialized for this pmic
        smps_ptr = pm_smps_get_data(pmic_index);
        if(smps_ptr == NULL)
               return;

        smps_data = pm_rpm_smps_get_resource_data(pmic_index);
        if(smps_data == NULL)
        {
            pm_malloc( sizeof(pm_npa_smps_data_type), (void**)&smps_data);

            smps_data->smpsDriverData = smps_ptr;
            smps_data->resourceType = resourceType;
            smps_data->depInfo = (pm_pwr_resource_dependency_info**)pm_target_information_get_specific_info(PM_PROP_SMPS_DEP);
            CORE_VERIFY_PTR(smps_data->depInfo);
            smps_data->railInfo = (pm_rpm_smps_rail_info_type**)pm_target_information_get_specific_info(PM_PROP_SMPS_RAIL);
            CORE_VERIFY_PTR(smps_data->railInfo);
            CORE_VERIFY_PTR(smps_data->railInfo[pmic_index]);

            //verify if the range can meet the SMPS's min, max voltage limits
            for( smps_index = 0; smps_index < num_npa_resources ; smps_index++)
            {
                 //skip over invalid peripherals
                 if(smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].periph_type == 0)
                    continue;

                 range = smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_range;
                 CORE_VERIFY(((smps_data->railInfo[pmic_index][smps_index].MinVoltage)*1000 >= smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_vset[range].RangeMin ) &&
                  ((smps_data->railInfo[pmic_index][smps_index].MaxVoltage)*1000 <= smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_vset[range].RangeMax));
            }
            smps_data->settlingTimeInfo = (pm_pwr_settling_time_info_type*)pm_target_information_get_common_info(PM_PROP_SMPS_SETTLING_TIME);
            smps_data->ultSettlingTimeInfo = (pm_pwr_settling_time_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_SMPS_SETTLING_TIME);
            pm_malloc(sizeof(uint64)*num_npa_resources, (void**)&(smps_data->settlingStartTime));
            pm_malloc(sizeof(uint64)*num_npa_resources, (void**)&(smps_data->settlingEndTime));

            rpm_register_resource(resourceType, num_npa_resources + 1, sizeof(pm_npa_smps_int_rep), pm_rpm_smps_translation, pm_rpm_smps_apply, (void *)smps_data);
            pm_rpm_smps_req_data[pmic_index] = smps_data;
        }
    }
}

void pm_rpm_smps_register_resource_dependencies(rpm_resource_type resourceType, uint32 num_npa_resources , uint8 pmic_index)
{
    uint8 count = 0;
    pm_pwr_resource *dependency = NULL;
    pm_pwr_resource_dependency_info *depInfo = NULL;
    pm_npa_smps_int_rep *current_agg = NULL;
    pm_smps_data_type* smps_ptr = NULL;
    pm_volt_level_type vol = 0;
    pm_on_off_type en_status = PM_OFF;
    pm_npa_smps_data_type *smps_data = NULL;

    if(num_npa_resources > 0)
    {
        //return if there was no driver initialized for this pmic
        if(pm_rpm_smps_req_data[pmic_index] == NULL)
            return;

        smps_data = pm_rpm_smps_req_data[pmic_index];

        if(smps_data->depInfo != NULL)
        {
            depInfo = smps_data->depInfo[pmic_index];
        }

        for(count = 1; count < num_npa_resources + 1; count++)
        {
            rpm_get_aggregated_request_buffer(resourceType, count, (const void**)&current_agg);
            if(current_agg != NULL)
            {
                smps_ptr = smps_data->smpsDriverData;
                CORE_VERIFY_PTR(smps_ptr);
                DALSYS_memset(current_agg, 0, sizeof(pm_npa_smps_int_rep));
                current_agg->global_byp_en = 1;
                pm_pwr_volt_level_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, count-1, &vol);
                current_agg->uvol = vol;
                pm_pwr_sw_enable_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, count-1, &en_status);
                current_agg->sw_en = en_status;

                // initialize the mode to invalid setting in order to make sure that the first vote will be granted
                current_agg->smps_sw_mode = 0x0F;
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
                    rpm_register_internal_client_type(dependency->resource_type, dependency->internal_resource_index, PM_RPM_NPA_INT_CLIENT_SMPS_DEPENDENT, sizeof(pm_npa_smps_int_rep));

                    depInfo[count].parent_source_dependency_client_handle =
                        rpm_create_client(dependency->resource_type, dependency->internal_resource_index, PM_RPM_NPA_INT_CLIENT_SMPS_DEPENDENT);
                }
            }
        }

        //Initialize droop detection
        pm_rpm_smps_droop_detect_init(resourceType, num_npa_resources);
    }
}

void pm_rpm_smps_check_ldo_bypass(rpm_application_info* info, boolean out_bypass)
{
    // LDO bypass callback variables
    unsigned clientType;
    void* state = NULL;
    pm_npa_ldo_int_rep* ldoClientRequest;
    pm_npa_smps_int_rep* smpsCurrentAgg = (pm_npa_smps_int_rep*)info->current_aggregation;
    unsigned numberOfClients = 0;
    unsigned clientCount = 0;
    unsigned numberofMasters = 0;

    numberOfClients = rpm_get_num_clients(info->resource_handle);
    numberofMasters = rpm_get_num_ees();

    for(clientCount = numberofMasters; clientCount < numberOfClients; clientCount++)
    {
        // Get the current state for the child resource
        rpm_get_current_request(info->resource_handle, clientCount, &clientType, (const void **)&state);

        if(clientType == PM_RPM_NPA_INT_CLIENT_LDO_DEPENDENT)
        {
            ldoClientRequest = (pm_npa_ldo_int_rep*)state;

            // A valid internal LDO client should have resource_id greater than zero
            if(ldoClientRequest->resource_id > 0)
            {
                if(TRUE == out_bypass)//check if out of bypass is needed
                {
                    pm_rpm_ldo_bypass_cb(ldoClientRequest->device_index, ldoClientRequest->resource_id, (pm_on_off_type)0);
                }
                else // check if in bypass is needed
                {
                    if( (smpsCurrentAgg->uvol == ldoClientRequest->output_uvol) &&
                        (PM_NPA_BYPASS_ALLOWED == ldoClientRequest->byp_allowed) )
                    {
                        if(ldoClientRequest->is_en_transition == 1)
                        {
                            ldo_pending_bypass_transition = TRUE;
                        }
                        else
                        {
                            pm_rpm_ldo_bypass_cb(ldoClientRequest->device_index, ldoClientRequest->resource_id, (pm_on_off_type)1);
                        }
                    }
                }
            }
        }
    }
}

pm_npa_smps_data_type* pm_rpm_smps_get_resource_data(uint8 pmic_index)
{

  if(pmic_index < PM_MAX_NUM_PMICS)
  {
      return  pm_rpm_smps_req_data[pmic_index];
  }

  return NULL;

}

void pm_rpm_smps_settle(rpm_application_info *info)
{
    pm_npa_smps_data_type *smps_data = (pm_npa_smps_data_type *)info->cb_data;
    uint8  internal_resource_index = (uint8)(info->id - 1);
    uint64 settle_start_time = smps_data->settlingStartTime[internal_resource_index];
    uint64 settle_end_time = smps_data->settlingEndTime[internal_resource_index];
    uint32 estimated_settling_time_us = 0;
    pm_smps_data_type* smps_ptr = NULL;
    uint8 pmic_index = GET_PMIC_INDEX_SMPS(smps_data->resourceType);

    if(settle_start_time != 0)
    {
        // calculate the estimated settling time in micro secs
        estimated_settling_time_us = pm_convert_timetick_to_time(settle_end_time - settle_start_time);

        // Get the driver data from the info call back data
        smps_ptr = smps_data->smpsDriverData;

        CORE_VERIFY_PTR(smps_ptr);

        // poll VREG_OK
        pm_rpm_check_vreg_settle_status(settle_start_time, estimated_settling_time_us, &smps_ptr->pm_pwr_data,
                                        smps_ptr->comm_ptr, internal_resource_index,
                                        smps_data->railInfo[pmic_index][info->id -1].SettlingErrorEnabled);

        // poll STEPPER_DONE
        pm_rpm_check_stepper_settle_status(settle_start_time, estimated_settling_time_us, &smps_ptr->pm_pwr_data,
                                           smps_ptr->comm_ptr, internal_resource_index,
                                           smps_data->railInfo[pmic_index][info->id -1].SettlingErrorEnabled);

        // reset the settling time values
        info->settling_time = 0;
        smps_data->settlingEndTime[internal_resource_index] = 0;
        smps_data->settlingStartTime[internal_resource_index] = 0;
    }
}

/*
    Calculate the new SMPS voltage given an LDO output voltage
    and safety headroom requirement.
    v_ldo: The required LDO output voltage.
    v_reg: The required LDO input voltage.
    v_low: Currently aggregated optimal SMPS voltage.
    v_high: If v_low cannot be used, then v_high is the minimum.

    Basically the range between v_low and v_high is invalid since it
    would result in violating the safety headroom of some child.

*/
void pm_rpm_smps_aggregate_dependent_ldo_input_voltage(uint32 v_ldo,uint32 v_reg, uint32* v_low, uint32* v_high)
{

    /* 1. New range is completely above current range.  In this case
     we simply update the low and high voltages to the new LDO
     bypass and regulated voltages.
    */
    if (v_ldo >= *v_high)
    {
        *v_low  = v_ldo;
        *v_high = v_reg;
    }
    /* 2. New bypass is equal to old bypass.  This is a bit of a special
     case.  Only thing we need to change is to update the high value
     if the headroom for this LDO is higher than current aggregation.
    */
    else if (v_ldo == *v_low)
    {
        if (v_reg > *v_high)
        {
          *v_high = v_reg;
        }
    }

    /* 3. New range is completely below current.  Another easy case,
     we can just ignore this child.
    */
    else if (v_reg <= *v_low)
    {
       return;
    }
    /* 4. New upper range is lower than current.  At this point we also
      know that the upper value is above the current low value (check
      3 above).  This means that the only valid aggregation is to run
      exactly at the current high value.
      Example: Current aggreation is 0.9+0.2, next child has LDO voltage
      of 0.8 and HR of 0.2 (meaning v_reg is 1.0).  The new child cannot
      be in bypass since 0.8 is lower than existing aggregation.  When
      in regulation it requires at least 1.0 volts from the SMPS.  Since
      current aggregation does not allow operation between 0.9 and 1.1
      we have to set the SMPS to 1.1 volts.
    */
    else if (v_reg < *v_high)
    {
        *v_low = *v_high;
    }
    /* 5. New upper range is higher (or equal) than current.  At this
      point we also know that the LDO output voltage is lower then the
      current aggregated high level (check 1 above).
      Example: Current aggregation is 0.9+0.2, next child has LDO voltage
      of 1.0 and headroom of 0.2 (meaning v_reg is 1.2).  We cannot set
      the parent to 1.0 since that is inside the current aggregated
      invalid range, so the only option is to set it to 1.2.
      */
    else if (v_reg >= *v_high)
    {
      *v_low  = v_reg;
      *v_high = v_reg;
    }
}

void pm_rpm_smps_droop_detect_init(rpm_resource_type resourceType, uint32 num_npa_resources)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_npa_smps_data_type *smps_data = NULL;
    pm_smps_data_type *smps_ptr = NULL;
    pm_npa_smps_int_rep *current_agg = NULL;
    uint32 smps_index = 0;
    uint32 periph_type = 0;
    uint32 periph_id = 0;
    uint32 isr_ctx = 0;
    uint32 pmic_index = GET_PMIC_INDEX_SMPS(resourceType);

    if(num_npa_resources > 0)
    {
        smps_data = pm_rpm_smps_get_resource_data(pmic_index);

        CORE_VERIFY_PTR(smps_data);
        CORE_VERIFY_PTR(smps_data->smpsDriverData);
        CORE_VERIFY_PTR(smps_data->railInfo);
        CORE_VERIFY_PTR(smps_data->railInfo[pmic_index]);

        smps_ptr = smps_data->smpsDriverData;

        for(smps_index = 0; smps_index < num_npa_resources; smps_index++)
        {
           //skip over invalid peripherals
           if(smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].periph_type == 0)
            continue;

           if(smps_data->railInfo[pmic_index][smps_index].DroopDetection == 1)
           {
              periph_type = smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].periph_type;
              periph_id = (smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].periph_base_address)>>8;

              // save periph_id <0:15> slave_id <16:31> in isr_ctx
              isr_ctx = ((smps_ptr->comm_ptr->slave_id & 0xFFFF)<<16) + (periph_id & 0xFF);

              // Configure the rail for droop detection
              err_flag |= pm_droop_detect_irq_config(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_index, isr_ctx);

              if(PM_HW_MODULE_FTS == periph_type)
              {
                 // Enable the droop detection VREG_ERROR interrupt always for FTS2.5 bucks
                 // Make sure to turn OFF the droop detection for FTS 1.0 bucks in the config data
                 err_flag |= pm_droop_detect_irq_enable(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_index, TRUE);
              }
              else
              {
                 // Get the rail's current aggregation to check if the rail is enabled
                 rpm_get_aggregated_request_buffer(resourceType, (smps_index + 1), (const void**)&current_agg);

                 if((current_agg != NULL) && (current_agg->sw_en == 1))
                 {
                    // Enable the droop detection interrupt only if the rail is enabled
                    err_flag |= pm_droop_detect_irq_enable(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_index, TRUE);
                 }
              }
              CORE_VERIFY(err_flag == PM_ERR_FLAG__SUCCESS);
           }
        }
    }
}

void pm_rpm_smps_droop_detect_irq_enable(rpm_application_info *info, boolean enable)
{
    pm_npa_smps_data_type *smps_data = (pm_npa_smps_data_type*)info->cb_data;
    pm_npa_smps_int_rep *smps_curr_agg =(pm_npa_smps_int_rep*)info->current_aggregation;
    uint32 smps_index = (info->id - 1);
    pm_smps_data_type *smps_ptr = NULL;
    uint8 pmic_index = GET_PMIC_INDEX_SMPS(smps_data->resourceType);

    // Check if Droop detection is ON for the rail
    if (smps_data->railInfo[pmic_index][smps_index].DroopDetection == 1)
    {
       // Get the driver data from the info call back data
       smps_ptr = smps_data->smpsDriverData;

       // if the rail is not FTS2.5, only then enable/disable the interrupt run time
       if((smps_ptr != NULL) && (smps_ptr->pm_pwr_data.pwr_specific_info != NULL) &&
          (PM_HW_MODULE_FTS != smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].periph_type))
       {
          if(enable == TRUE)
          {
             // Enable droop detection interrupt only if the rail is enabled
             if(smps_curr_agg->sw_en == 1)
             {
                pm_droop_detect_irq_enable(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_index, TRUE);
             }
          }
          else
          {
             // Disable droop detection interrupt
             pm_droop_detect_irq_enable(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_index, FALSE);
          }
       }
    }
}

