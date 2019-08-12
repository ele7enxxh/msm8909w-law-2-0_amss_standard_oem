/*===========================================================================
                              railway.c

SERVICES:

DESCRIPTION: railway.c - railway top-level implementation

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

Copyright (c) 2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "CoreVerify.h"
#include "DALSys.h"

#include "railway.h"

#include "railway_internal.h"
#include "railway_config.h"
#include "railway_adapter.h"
#include "rpmserver.h"
#include "swevent.h"
#include "alloca.h"
#include "pm_npa_irep.h"


//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------
int railway_mx_bypass_count = 0;

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------
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

    memcpy(rail_state->corner_uvs, rail_config->default_uvs, sizeof(uint32[RAILWAY_CORNERS_COUNT]));

    // Compute what our initial request should be.
    railway_corner corner = rail_config->initial_corner;
    uint32 uv             = rail_state->corner_uvs[corner];

    // Update our initial state.
    rail_state->current_active.mode       = corner;
    rail_state->current_active.vol_level  = rpm_rail_corner_remap(corner);
    rail_state->current_active.microvolts = uv;

    rail_state->rail_server_handle = rpm_create_client(rail_config->vreg_type, rail_config->vreg_num, 0);
    
    switch(rail_config->vreg_type & VREG_TYPE_MASK)
    {
        case VREG_TYPE_SMPS:
        {
            rail_state->rail_pmic_server_vote = malloc(sizeof(pm_npa_smps_int_rep));
            CORE_VERIFY_PTR(rail_state->rail_pmic_server_vote);
            memset(rail_state->rail_pmic_server_vote, 0, sizeof(pm_npa_smps_int_rep));
        }
        break;
        case VREG_TYPE_LDO:
        {
            rail_state->rail_pmic_server_vote = malloc(sizeof(pm_npa_ldo_int_rep));
            CORE_VERIFY_PTR(rail_state->rail_pmic_server_vote);
            memset(rail_state->rail_pmic_server_vote, 0, sizeof(pm_npa_ldo_int_rep));
        }
        break;
        default:
            CORE_VERIFY(0); //Bad rail type
    }
}

/*===========================================================================
FUNCTION: 

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static void railway_walk_cbs(const railway_rail_state_t *rail_data, railway_cb_type type, const railway_settings *proposal)
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

void railway_set_pmic_voltage(int rail_num, unsigned microvolts)
{
    const railway_rail_config_t *rail_config = &RAILWAY_CONFIG_DATA->rails[rail_num];
    const railway_rail_state_t *rail_data = &railway.rail_state[rail_num];
    
    switch(rail_config->vreg_type & VREG_TYPE_MASK)
    {
        case VREG_TYPE_SMPS:
        {
            pm_npa_smps_int_rep* request = (pm_npa_smps_int_rep*)rail_data->rail_pmic_server_vote;

            if(microvolts)
                request->sw_en = TRUE;
            else
                request->sw_en = FALSE;
            request->uvol = microvolts;
            
            rpm_issue_request(rail_config->vreg_type, rail_config->vreg_num, rail_data->rail_server_handle, sizeof(pm_npa_smps_int_rep), request);       
        }
        break;
        case VREG_TYPE_LDO:
        {
            pm_npa_ldo_int_rep* request = (pm_npa_ldo_int_rep*)rail_data->rail_pmic_server_vote;
        
            if(microvolts)
                request->sw_en = TRUE;
            else
                request->sw_en = FALSE;
            request->output_uvol = microvolts;
            
            rpm_issue_request(rail_config->vreg_type, rail_config->vreg_num, rail_data->rail_server_handle, sizeof(pm_npa_ldo_int_rep), request);
        }
        break;
        default:
            CORE_VERIFY(0); //Bad rail type
    }
}

static void railway_transition_individual_rail(int rail_num)
{
    railway_rail_state_t *rail_data = &railway.rail_state[rail_num];
    const railway_settings* target = &rail_data->constrained_target;
    unsigned target_voltage = target->microvolts;

    railway_walk_cbs(rail_data, RAILWAY_PRECHANGE_CB, target);

    SWEVENT(RAILWAY_CHANGE_VOLTAGE, rail_num, target->microvolts);

    // Only set the actual voltage if it's changing. It could be that we're just changing modes.
    // E.g. if CPR has taken the voltage down in one mode to the point that it is the same voltage as another mode.
    if(target_voltage!=railway.rail_state[rail_num].current_active.microvolts)
    {
        railway_set_pmic_voltage(rail_num, target_voltage);
    }
    
    memcpy(&rail_data->current_active, target, sizeof(railway_settings));
    railway_walk_cbs(rail_data, RAILWAY_POSTCHANGE_CB, target);
}

static void railway_quantize_constrained_target(int rail)
{
    const railway_corner* supported_corners = RAILWAY_CONFIG_DATA->rails[rail].supported_corners;
    const railway_corner highest_supported_corner = supported_corners[RAILWAY_CONFIG_DATA->rails[rail].supported_corners_count-1];

    //Assert that we don't have an explicit request greater than our highest supported corner.
    assert(railway.rail_state[rail].corner_uvs[highest_supported_corner]
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
    railway.rail_state[rail].constrained_target.vol_level  = rpm_rail_corner_remap(quantized_corner);
    railway.rail_state[rail].constrained_target.microvolts = railway.rail_state[rail].corner_uvs[quantized_corner];
}

/*===========================================================================
FUNCTION: 

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static void railway_update_constrained_targets( void )
{
    //Go through the rails and calculate their constrained voltage target.

    //Start from the unconstrained_target (which is the minimum for each rail) and work up until we settle on voltages.
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        memcpy(&railway.rail_state[i].constrained_target,
            &railway.rail_state[i].unconstrained_target,
            sizeof(railway_settings));
    }

    //Now, quantize logical rails to a corner.
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        if(RAILWAY_CONFIG_DATA->rails[i].rail_type != RAILWAY_RAIL_TYPE_LOGIC)   //Skip Mx & EBI, we do that later.
            continue;

        railway_quantize_constrained_target(i);
    }

    //Now assert that the logical rails are multiples of the PMIC step size.
    //It's up to CPR to ensure that the corner values it sets are multiples of the PMIC step size.
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        if(RAILWAY_CONFIG_DATA->rails[i].rail_type != RAILWAY_RAIL_TYPE_LOGIC)   //Skip Mx & EBI at this point as we're only looking at logical rails.
            continue;

        assert(!(railway.rail_state[i].constrained_target.microvolts%RAILWAY_CONFIG_DATA->rails[i].pmic_step_size));
    }

    railway_rail_state_t* mx_rail = &railway.rail_state[mx_rail_id];

    //Next, bump up MX to the max of the logical rails.
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        if(RAILWAY_CONFIG_DATA->rails[i].rail_type != RAILWAY_RAIL_TYPE_LOGIC)   //Skip comparison with ourselves.
            continue;

        mx_rail->constrained_target.mode =
            MAX(mx_rail->constrained_target.mode, railway.rail_state[i].constrained_target.mode);
        mx_rail->constrained_target.microvolts =
            MAX(mx_rail->corner_uvs[mx_rail->constrained_target.mode], mx_rail->constrained_target.microvolts);
        
	mx_rail->constrained_target.vol_level = rpm_rail_corner_remap (mx_rail->constrained_target.mode);
    }

    //Now quantize Mx.
    railway_quantize_constrained_target(mx_rail_id);

    //Quantize EBI if supported
    if(ebi_rail_id != RAIL_NOT_SUPPORTED_BY_RAILWAY)
    {
        railway_rail_state_t* ebi_rail = &railway.rail_state[ebi_rail_id];

        // EBI shall be in the same voltage mode as Mx if aggregated mode > retention
        // Otherwise, shall be in retention
        // NOTE: Explicit voltage requests are not supported on vdda_ebi
        if(ebi_rail->unconstrained_target.mode > RAILWAY_RETENTION)
        {
            ebi_rail->constrained_target.mode       = mx_rail->constrained_target.mode;
            ebi_rail->constrained_target.vol_level  = rpm_rail_corner_remap(mx_rail->constrained_target.mode);
            ebi_rail->constrained_target.microvolts = ebi_rail->corner_uvs[mx_rail->constrained_target.mode];
        }
        else
        {
            ebi_rail->constrained_target.mode       = RAILWAY_RETENTION;
            ebi_rail->constrained_target.vol_level  = rpm_rail_corner_remap(RAILWAY_RETENTION);
            ebi_rail->constrained_target.microvolts = ebi_rail->corner_uvs[RAILWAY_RETENTION];
        }
    }
}

static void railway_do_transition_rail(bool chip_sleep_imminent)
{
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        //To do - move this to when we update votes on rails - I'm sure that will be more efficient.
        //Need to figure out how we deal with CPR changing corner voltages under our feet though.
        railway_aggregated_voltage_target_uv(i, &railway.rail_state[i].unconstrained_target, false);
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

    //Now transition EBI if supported.
    if(ebi_rail_id != RAIL_NOT_SUPPORTED_BY_RAILWAY)
    {
        //Transition EBI if it's going up.
        if((railway.rail_state[ebi_rail_id].constrained_target.microvolts > railway.rail_state[ebi_rail_id].current_active.microvolts) ||
           (railway.rail_state[ebi_rail_id].constrained_target.mode > railway.rail_state[ebi_rail_id].current_active.mode))
        {
            railway_transition_individual_rail(ebi_rail_id);
        }
    }

    // Now transition the logical rails
    for (int i = 0; i < RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        if(RAILWAY_CONFIG_DATA->rails[i].rail_type != RAILWAY_RAIL_TYPE_LOGIC)
            continue;

        if((railway.rail_state[i].constrained_target.microvolts!=railway.rail_state[i].current_active.microvolts) ||
            (railway.rail_state[i].constrained_target.mode!=railway.rail_state[i].current_active.mode))
        {
            railway_transition_individual_rail(i);
        }
    }

    //Now transition EBI if supported.
    if(ebi_rail_id != RAIL_NOT_SUPPORTED_BY_RAILWAY)
    {
        //Transition EBI if it's going down.
        if((railway.rail_state[ebi_rail_id].constrained_target.microvolts < railway.rail_state[ebi_rail_id].current_active.microvolts) ||
           (railway.rail_state[ebi_rail_id].constrained_target.mode < railway.rail_state[ebi_rail_id].current_active.mode))
        {
            railway_transition_individual_rail(ebi_rail_id);
        }
    }

    if((railway.rail_state[mx_rail_id].constrained_target.microvolts<railway.rail_state[mx_rail_id].current_active.microvolts) ||
        (railway.rail_state[mx_rail_id].constrained_target.mode<railway.rail_state[mx_rail_id].current_active.mode))
    {
      if (! chip_sleep_imminent)
      {
        railway_transition_individual_rail (mx_rail_id);
      }
      else
      {
        railway_mx_bypass_count ++;
      }
    }

    // Assertion that current_active == constrained_target for all rails.
    for (int i = 0; i < RAILWAY_CONFIG_DATA->num_rails; i++)
    {
		if(!((chip_sleep_imminent) && (i == mx_rail_id)))
		{
			CORE_VERIFY (railway.rail_state[i].constrained_target.microvolts == railway.rail_state[i].current_active.microvolts);
            CORE_VERIFY(railway.rail_state[mx_rail_id].current_active.mode>=railway.rail_state[i].current_active.mode);
		}
    }
}

static bool railway_transition_in_progress = false;
extern bool sleep_deep_imminent (void);

/*===========================================================================
FUNCTION: Top level fn for transitioning a rail. Actually transitions all rails, not just rail_num.

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void railway_transition_rail(int rail_num)
{
    assert(!railway_transition_in_progress);    //We don't support this fn becoming re-entrant for now. Guard against it.
    railway_transition_in_progress = true;

    bool chip_sleep_imminent = sleep_deep_imminent ();

    railway_do_transition_rail (chip_sleep_imminent);

    railway_transition_in_progress = false;
}


/*===========================================================================
FUNCTION: 

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void railway_transitioner_init(void)
{
    for(uint32 i = 0; i < RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        initialize_rail(i);
    }
}

/*===========================================================================
FUNCTION: 

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
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

/*===========================================================================
FUNCTION: 

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
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

/*===========================================================================
FUNCTION: 

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
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

void railway_get_sleep_voltages(int cx_id, int mx_id, uint32 *cx_microvolts, uint32 *mx_microvolts)
{
    railway_settings cx_settings;
    railway_settings mx_settings;

    CORE_VERIFY(cx_id < RAILWAY_CONFIG_DATA->num_rails);
    CORE_VERIFY(mx_id < RAILWAY_CONFIG_DATA->num_rails);

    railway_aggregated_voltage_target_uv(cx_id, &cx_settings, true);
    railway_aggregated_voltage_target_uv(mx_id, &mx_settings, true);

    *cx_microvolts = cx_settings.microvolts;
    *mx_microvolts = mx_settings.microvolts;
}



