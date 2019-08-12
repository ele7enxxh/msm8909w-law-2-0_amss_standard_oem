// railway.c - railway top-level implementation
//
// Copyright 2011 - 2012 by QUALCOMM Technologies, Inc.
// All Rights Reserved
// Confidental and Proprietary
//
// $ Header: $
// $ Author: $
// $ DateTime: $
//

#include "railway.h"

#include "railway_internal.h"
#include "railway_config.h"
#include "alloca.h"
#include "pm_ldo.h"
#include "pm_smps.h"
#include <string.h>
#include <stdbool.h>
#include "CoreVerify.h"
#include "stringl/stringl.h"

static const railway_internal_cb_type railway_callback_remap[RAILWAY_NUM_CB_TYPES] =
{
    RAILWAY_INTERNAL_PRECHANGE_CB,      //RAILWAY_PRECHANGE_CB
    RAILWAY_INTERNAL_POSTCHANGE_CB,     //RAILWAY_POSTCHANGE_CB
    RAILWAY_INTERNAL_PRECHANGE_CB,      //RAILWAY_LAST_PRECHANGE_CB
    RAILWAY_INTERNAL_POSTCHANGE_CB,     //RAILWAY_FIRST_POSTCHANGE_CB
};


static void initialize_rail(uint32 rail_num)
{
    const railway_rail_config_t *rail_config = &RAILWAY_CONFIG_DATA->rails[rail_num];

    railway_rail_state_t* rail_state = &railway.rail_state[rail_num];

    CORE_VERIFY(sizeof(uint32[RAILWAY_CORNERS_COUNT]) == memscpy(rail_state->corner_uvs, sizeof(uint32[RAILWAY_CORNERS_COUNT]), rail_config->default_uvs, sizeof(uint32[RAILWAY_CORNERS_COUNT])));

    // Compute what our initial request should be.
    railway_corner corner = rail_config->initial_corner;
    uint32 uv           = rail_state->corner_uvs[corner];

    // Update our initial state.
    rail_state->current_active.mode       = corner;
    rail_state->current_active.microvolts = uv;
}

static void railway_walk_cbs(const railway_rail_state_t *rail_data, railway_internal_cb_type type, const railway_settings *proposal)
{
    //Must be done from 0 upwards as we have some callbacks which must be called first which are in the 0th slot.
    for(int i=0; i<RAILWAY_MAX_CBS; i++)
    {
        if(rail_data->cbs[type][i].callback_fn)
        {
            rail_data->cbs[type][i].callback_fn(proposal, rail_data->cbs[type][i].callback_cookie);
        }
    }
}

static void railway_transition_individual_rail(int rail_num)
{
    railway_rail_state_t *rail_data = &railway.rail_state[rail_num];
    const railway_settings* target = &rail_data->constrained_target;


    railway_walk_cbs(rail_data, RAILWAY_INTERNAL_PRECHANGE_CB, target);

    // Only set the actual voltage if it's changing. It could be that we're just changing modes.
    // E.g. if CPR has taken the voltage down in one mode to the point that it is the same voltage as another mode.

	//TODO the pmic changes as of 3-feb-15 are not in. is expected soon. Then this needs to be uncommented
    if(rail_data->constrained_target.microvolts!=rail_data->current_active.microvolts)
    {
        if(RAILWAY_CONFIG_DATA->rails[rail_num].pmic_rail_type==RAILWAY_SMPS_TYPE)
        {
            //pm_smps_volt_level(RAILWAY_CONFIG_DATA->rails[rail_num].pmic_chip_id, RAILWAY_CONFIG_DATA->rails[rail_num].pmic_peripheral_index, target->microvolts);
        }
        else if(RAILWAY_CONFIG_DATA->rails[rail_num].pmic_rail_type==RAILWAY_LDO_TYPE)
        {
            //pm_ldo_volt_level(RAILWAY_CONFIG_DATA->rails[rail_num].pmic_chip_id, RAILWAY_CONFIG_DATA->rails[rail_num].pmic_peripheral_index, target->microvolts);
        }
    }

    CORE_VERIFY(sizeof(railway_settings) == memscpy(&rail_data->current_active, sizeof(railway_settings), target, sizeof(railway_settings)));
    railway_walk_cbs(rail_data, RAILWAY_INTERNAL_POSTCHANGE_CB, target);
}

static void railway_quantize_constrained_target(int rail)
{
    const railway_corner* supported_corners = RAILWAY_CONFIG_DATA->rails[rail].supported_corners;
    const railway_corner highest_supported_corner = supported_corners[RAILWAY_CONFIG_DATA->rails[rail].supported_corners_count-1];

    //Assert that we don't have an explicit request greater than our highest supported corner.
    CORE_VERIFY(railway.rail_state[rail].corner_uvs[highest_supported_corner]
            >=railway.rail_state[rail].constrained_target.microvolts);

    railway_corner quantized_corner=highest_supported_corner;

    // -2 because we don't need to check the highest supported corner - we start there.
    for(int i=RAILWAY_CONFIG_DATA->rails[rail].supported_corners_count-2; i>=0; i--)
    {
        //Check that we're not going lower than the constrained target's mode.
        if(supported_corners[i]<railway.rail_state[rail].constrained_target.mode)
        {
            break;
        }

        if(railway.rail_state[rail].constrained_target.microvolts>
            railway.rail_state[rail].corner_uvs[supported_corners[i]])
        {
            break;
        }
        quantized_corner = supported_corners[i];
    }

    railway.rail_state[rail].constrained_target.mode = quantized_corner;
    railway.rail_state[rail].constrained_target.microvolts = railway.rail_state[rail].corner_uvs[quantized_corner];
}

static void railway_update_constrained_targets( void )
{
    //Go through the rails and calculate their constrained voltage target.

    //Start from the unconstrained_target (which is the minimum for each rail) and work up until we settle on voltages.
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        CORE_VERIFY(sizeof(railway_settings) == memscpy(&railway.rail_state[i].constrained_target,
            sizeof(railway_settings),
            &railway.rail_state[i].unconstrained_target,
            sizeof(railway_settings)));
    }

    //Now, quantize logical rails to a corner.
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        if(RAILWAY_CONFIG_DATA->rails[i].rail_type != RAILWAY_RAIL_TYPE_LOGIC)   //Skip Mx, we do that later.
            continue;

        railway_quantize_constrained_target(i);
    }

    //Now assert that the logical rails are multiples of the PMIC step size.
    //It's up to CPR to ensure that the corner values it sets are multiples of the PMIC step size.
    railway_rail_state_t* mx_rail = &railway.rail_state[mx_rail_id];
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        if(RAILWAY_CONFIG_DATA->rails[i].rail_type != RAILWAY_RAIL_TYPE_LOGIC)   //Skip Mx at this point as we're only looking at logical rails.
            continue;

        CORE_VERIFY(!(railway.rail_state[i].constrained_target.microvolts%RAILWAY_CONFIG_DATA->rails[i].pmic_step_size));
    }

    //Next, bump up MX to the max of the logical rails.
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        if(RAILWAY_CONFIG_DATA->rails[i].rail_type != RAILWAY_RAIL_TYPE_LOGIC)   //Skip comparison with ourselves.
            continue;

        mx_rail->constrained_target.mode =
            MAX(mx_rail->constrained_target.mode, railway.rail_state[i].constrained_target.mode);
        mx_rail->constrained_target.microvolts =
            MAX(mx_rail->corner_uvs[mx_rail->constrained_target.mode], mx_rail->constrained_target.microvolts);
    }

    //Now quantize Mx.
    railway_quantize_constrained_target(mx_rail_id);

}

static void railway_do_transition_rails(void)
{
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        //To do - move this to when we update votes on rails - I'm sure that will be more efficient.
        //Need to figure out how we deal with CPR changing corner voltages under our feet though.
        railway_aggregated_voltage_target_uv(i, &railway.rail_state[i].unconstrained_target);
    }

    //Now update the constrained targets for the rails.
    railway_update_constrained_targets();

    //Now do the transitions.
    //If MX is going up, do that first.
    if((railway.rail_state[mx_rail_id].constrained_target.microvolts>railway.rail_state[mx_rail_id].current_active.microvolts) ||
        (railway.rail_state[mx_rail_id].constrained_target.mode>railway.rail_state[mx_rail_id].current_active.mode))
    {
        railway_transition_individual_rail(mx_rail_id);
    }

    //Now transition the logical rails
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        if(RAILWAY_CONFIG_DATA->rails[i].rail_type != RAILWAY_RAIL_TYPE_LOGIC)
            continue;

        if((railway.rail_state[i].constrained_target.microvolts!=railway.rail_state[i].current_active.microvolts) ||
            (railway.rail_state[i].constrained_target.mode!=railway.rail_state[i].current_active.mode))
        {
            railway_transition_individual_rail(i);
        }
    }

    //Now transition Mx if it's going down.
    if((railway.rail_state[mx_rail_id].constrained_target.microvolts<railway.rail_state[mx_rail_id].current_active.microvolts) ||
        (railway.rail_state[mx_rail_id].constrained_target.mode<railway.rail_state[mx_rail_id].current_active.mode))
    {
        railway_transition_individual_rail(mx_rail_id);
    }

    //Assertion that current_active == constrained_target for all rails.
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        CORE_VERIFY(railway.rail_state[i].constrained_target.microvolts == railway.rail_state[i].current_active.microvolts);
        CORE_VERIFY(railway.rail_state[i].constrained_target.mode == railway.rail_state[i].current_active.mode);
    }

    //Assertion that MX >= logical rails
    //Todo - add this back in once Mx CPR is corrected.
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        if(RAILWAY_CONFIG_DATA->rails[i].rail_type != RAILWAY_RAIL_TYPE_LOGIC)
            continue;

        CORE_VERIFY(railway.rail_state[mx_rail_id].current_active.microvolts>=railway.rail_state[i].current_active.microvolts);
        CORE_VERIFY(railway.rail_state[mx_rail_id].current_active.mode>=railway.rail_state[i].current_active.mode);
    }
}

static bool railway_transition_in_progress = false;

//Top level fn for transitioning rails.
void railway_transition_rails()
{
    CORE_VERIFY(!railway_transition_in_progress);    //We don't support this fn becoming re-entrant for now. Guard against it.
    railway_transition_in_progress = true;

    railway_do_transition_rails();
    
    railway_transition_in_progress = false;
}

void railway_transitioner_init(void)
{
    for(uint32 i = 0; i < RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        initialize_rail(i);
    }
}

static void railway_set_pre_callback(railway_rail_state_t *rail_state, railway_cb_type type, railway_cb cb, void* cookie)
{
    const railway_internal_cb_type cb_type = railway_callback_remap[type];

    //Deal with special callbacks first.
    if(type!=RAILWAY_PRECHANGE_CB)
    {
        //If we ever extend this, implement another lookup to map the enum railway_cb_type to the array index.
        //For now, support the last entry only.
        CORE_VERIFY(type==RAILWAY_LAST_PRECHANGE_CB);
        
        CORE_VERIFY(rail_state->cbs[cb_type][RAILWAY_MAX_CBS-1].callback_fn==NULL);
        rail_state->cbs[cb_type][RAILWAY_MAX_CBS-1].callback_fn = cb;
        rail_state->cbs[cb_type][RAILWAY_MAX_CBS-1].callback_cookie = cookie;
        return;
    }
    
    // If this check fails then we need to increase RAILWAY_MAX_CBS by 1.
    // The last CB is reserved for RAILWAY_LAST_PRECHANGE_CB, so check the "RAILWAY_MAX_CBS-2"-th entry
    CORE_VERIFY(!rail_state->cbs[cb_type][RAILWAY_MAX_CBS-2].callback_fn);
    
    // The last CB is reserved for RAILWAY_LAST_PRECHANGE_CB, so search up to the "RAILWAY_MAX_CBS-1"-th entry
    for(int i = 0; i < RAILWAY_MAX_CBS-2; ++i)
    {
        if(!rail_state->cbs[cb_type][i].callback_fn)
        {
            rail_state->cbs[cb_type][i].callback_fn = cb;
            rail_state->cbs[cb_type][i].callback_cookie = cookie;
            break;
        }
    }
}

static void railway_set_post_callback(railway_rail_state_t *rail_state, railway_cb_type type, railway_cb cb, void* cookie)
{
    const railway_internal_cb_type cb_type = railway_callback_remap[type];

    //Deal with special callbacks first.
    if(type!=RAILWAY_POSTCHANGE_CB)
    {
        //If we ever extend this, implement another lookup to map the enum railway_cb_type to the array index.
        //For now, support the 0th entry only.
        CORE_VERIFY(type==RAILWAY_FIRST_POSTCHANGE_CB);
        
        CORE_VERIFY(rail_state->cbs[cb_type][0].callback_fn==NULL);
        rail_state->cbs[cb_type][0].callback_fn = cb;
        rail_state->cbs[cb_type][0].callback_cookie = cookie;
        return;
    }

    // If this check fails then we need to increase RAILWAY_MAX_CBS by 1.
    CORE_VERIFY(!rail_state->cbs[cb_type][RAILWAY_MAX_CBS-1].callback_fn);
    
    //Skip the first entry as that's for RAILWAY_FIRST_POSTCHANGE_CB
    for(int i = 1; i < RAILWAY_MAX_CBS; ++i)
    {
        if(!rail_state->cbs[cb_type][i].callback_fn)
        {
            rail_state->cbs[cb_type][i].callback_fn = cb;
            rail_state->cbs[cb_type][i].callback_cookie = cookie;
            break;
        }
    }
}

void railway_set_callback(int rail, railway_cb_type type, railway_cb cb, void* cookie)
{
    CORE_VERIFY(type<RAILWAY_NUM_CB_TYPES);
    CORE_VERIFY(cb);
    CORE_VERIFY(rail<RAILWAY_CONFIG_DATA->num_rails);

    railway_rail_state_t *rail_state = &railway.rail_state[rail];
    const railway_internal_cb_type cb_type = railway_callback_remap[type];
    
    if(cb_type==RAILWAY_INTERNAL_PRECHANGE_CB)
    {
        railway_set_pre_callback(rail_state, type, cb, cookie);
    }
    else if(cb_type==RAILWAY_INTERNAL_POSTCHANGE_CB)
    {
        railway_set_post_callback(rail_state, type, cb, cookie);
    }
    else
    {
        CORE_VERIFY(0);
    }
}

