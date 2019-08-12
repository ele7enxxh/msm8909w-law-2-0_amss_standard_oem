/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include <assert.h>

#include "ocmem_resource.h"
#include "rpmserver.h"
#include "HALhwio.h"
#include "ocmem_resource_internal.h"
#include "busywait.h"
#include "swevent.h"
#include "stdbool.h"
#include "npa.h"
#include "alloca.h"

//To do - pack this down.
typedef struct
{
    int active_votes;
    int retention_votes;
} rpm_ocmem_bank_vote;

typedef enum
{
    RPM_OCMEM_OFF = 0,
    RPM_OCMEM_RETENTION,
    RPM_OCMEM_ACTIVE
} rpm_ocmem_state;

typedef struct
{
    int8 ocmem_bank_vote[OCMEM_BANKS_PER_REGION];
} rpm_ocmem_vote_int_rep;

#define OCMEM_SLEEP_WITH_RETENTION  0x6
#define OCMEM_SLEEP_NO_RETENTION    0x4
#define OCMEM_NORMAL                0x3

static rpm_ocmem_bank_vote rpm_ocmem_vote_matrix[OCMEM_REGIONS][OCMEM_BANKS_PER_REGION];

//Target specific data on how to transition rails.
extern const ocmem_hwio_param ocmem_hwio_param_matrix[OCMEM_REGIONS][OCMEM_BANKS_PER_REGION];
extern const bool ocmem_driver_enabled;

static npa_client_handle ocmem_clk_handle;
static boolean clock_enabled = false;
    
static void ocmem_hwio_function(unsigned target_value, const ocmem_hwio_param* bank)
{
    out_dword_masked_ns(bank->ctl_addr, bank->msk, target_value << bank->shft, in_dword_masked(bank->ctl_addr, bank->rmsk));
}

static void ocmem_disable_clock()
{
    if(!clock_enabled)
        return;

    npa_issue_suppressible_request(ocmem_clk_handle, 0);
    clock_enabled = false;
}

static void ocmem_enable_clock()
{
    if(clock_enabled)   //Clock is already enabled, no need to call again.
        return;
    
    npa_issue_suppressible_request(ocmem_clk_handle, 19200);
    clock_enabled = true;
}

static void rpm_ocmem_xlate(rpm_translation_info *info)
{
    unsigned               type, length, value;
    const char             *value_ptr;
    rpm_ocmem_vote_int_rep *req = info->dest_buffer;

    /* Read until we run out of KVPs. */
    while( !kvp_eof(info->new_kvps) )
    {
        if(!kvp_get( info->new_kvps, &type, &length, &value_ptr ))
        {
            abort();
        }

        if(length==0)
        {
            //If length is zero then that implies the key has been invalidated on the master side,
            //so set the vote to off for this request.
            req->ocmem_bank_vote[type]=RPM_OCMEM_OFF;
        }
        else
        {
            assert(length==sizeof(unsigned));
            value = *(unsigned*)value_ptr;

            assert((value == RPM_OCMEM_ACTIVE)
                || (value == RPM_OCMEM_RETENTION)
                || (value == RPM_OCMEM_OFF));

            req->ocmem_bank_vote[type]=value;
        }
    }
}

static void rpm_ocmem_update_bank_setting(rpm_ocmem_state old_vote, rpm_ocmem_state new_vote, rpm_ocmem_bank_vote *bank, const ocmem_hwio_param* update_ocmem_hwio_parmas)
{
    assert(old_vote != new_vote);   //This function assumes this.
    
    SWEVENT(OCMEM_UPDATE_BANK_SETTING, old_vote, new_vote, bank->active_votes, bank->retention_votes);

    //Vote has changed for this bank - remove old vote
    if(old_vote == RPM_OCMEM_ACTIVE)
    {
        bank->active_votes--;    //We're no longer voting for active
    }
    else if (old_vote == RPM_OCMEM_RETENTION)
    {
        bank->retention_votes--; //We're no longer voting for retention
    }

    //Apply the new vote
    if(new_vote == RPM_OCMEM_ACTIVE)
    {
        bank->active_votes++;
    }
    else if (new_vote == RPM_OCMEM_RETENTION)
    {
        bank->retention_votes++;
    }

    //Now apply the transitions.
    if((new_vote  == RPM_OCMEM_ACTIVE) && (bank->active_votes==1))
    {
        //Transition to active, doesn't matter which mode we were in previously.
        ocmem_hwio_function(OCMEM_NORMAL, update_ocmem_hwio_parmas);
    }
    else if((old_vote == RPM_OCMEM_ACTIVE) && (bank->active_votes==0) && (bank->retention_votes>0))
    {
        //Transition from active to retention
        ocmem_hwio_function(OCMEM_SLEEP_WITH_RETENTION, update_ocmem_hwio_parmas);
    }
    else if((new_vote == RPM_OCMEM_RETENTION) && (bank->active_votes==0) && (bank->retention_votes==1))
    {
        //Transition from off to retention
        //Have to go via active with a delay. Don't expect to do this often. Can do better than busywait(1).
        ocmem_hwio_function(OCMEM_NORMAL, update_ocmem_hwio_parmas);
        busywait(1);
        ocmem_hwio_function(OCMEM_SLEEP_WITH_RETENTION, update_ocmem_hwio_parmas);
    }
    else if((bank->active_votes==0) &&
        (bank->retention_votes==0))
    {
        //Transition to off
        if(old_vote == RPM_OCMEM_ACTIVE)
        {
            //Transition from active to off
            ocmem_hwio_function(OCMEM_SLEEP_NO_RETENTION, update_ocmem_hwio_parmas);
        }
        else
        {
            //Transition from retention to off
            //Have to go via active with a delay. Don't expect to do this often. Can do better than busywait(1).
            ocmem_hwio_function(OCMEM_NORMAL, update_ocmem_hwio_parmas);
            busywait(1);
            ocmem_hwio_function(OCMEM_SLEEP_NO_RETENTION, update_ocmem_hwio_parmas);
        }
    }
}

static void rpm_ocmem_apply(rpm_application_info *info)
{
    rpm_ocmem_vote_int_rep* old_int_rep;
    rpm_ocmem_vote_int_rep* new_int_rep;
    int i;
    int region;

    if(info->old_state)
    {
        old_int_rep = (rpm_ocmem_vote_int_rep*)info->old_state;    
    }
    else
    {
        old_int_rep = alloca(sizeof(rpm_ocmem_vote_int_rep));
        memset(old_int_rep, 0, sizeof(rpm_ocmem_vote_int_rep)); //Set to RPM_OCMEM_OFF as is default
    }
    
    new_int_rep = (rpm_ocmem_vote_int_rep*)info->new_state;
    region = info->id;

    assert(region<OCMEM_REGIONS);

    for(i=0; i<OCMEM_BANKS_PER_REGION; i++)
    {
        rpm_ocmem_state old_vote;
        rpm_ocmem_state new_vote;

        old_vote = (rpm_ocmem_state)old_int_rep->ocmem_bank_vote[i];
        new_vote = (rpm_ocmem_state)new_int_rep->ocmem_bank_vote[i];

        if(old_vote!=new_vote)
        {
            ocmem_enable_clock();   //This will only turn on the clock if it's not already on.

            rpm_ocmem_bank_vote *bank = &(rpm_ocmem_vote_matrix[region][i]);
            const ocmem_hwio_param* update_params = &ocmem_hwio_param_matrix[region][i];

            assert((old_vote == RPM_OCMEM_ACTIVE)
                || (old_vote == RPM_OCMEM_RETENTION)
                || (old_vote == RPM_OCMEM_OFF));

            assert((new_vote == RPM_OCMEM_ACTIVE)
                || (new_vote == RPM_OCMEM_RETENTION)
                || (new_vote == RPM_OCMEM_OFF));

            SWEVENT(OCMEM_APPLY, info->client, region, i);
            rpm_ocmem_update_bank_setting(old_vote, new_vote, bank, update_params);
        }
    }
    ocmem_disable_clock();
}

void rpm_ocmem_resource_init()
{
    if(!ocmem_driver_enabled)
        return;

    rpm_register_resource(RPM_OCMEM_POWER_REQ, 3, sizeof(rpm_ocmem_vote_int_rep), rpm_ocmem_xlate, rpm_ocmem_apply, 0);
    
    ocmem_clk_handle = npa_create_sync_client("/clk/ocmem", "rpm ocmem", NPA_CLIENT_SUPPRESSIBLE);
    assert(ocmem_clk_handle);
}

