/*===========================================================================
                              rpm_resources.cpp

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
#ifdef _MSC_VER
    #include <malloc.h>
    #define alloca _alloca
#else
    #include <alloca.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "rpm_resources.h"
#include "rpm_resources_config.h"
#include "rpm_config.h"
#include "rpm_hash.h"
#include "err.h"
#include "swevent.h"
#include "CoreVerify.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------
#define GET_LSB(x) ((x) & 0x1)
#define OTHER_IDX(x) (GET_LSB((x) ^ 0x1))

#define RESOURCE_SHADOW_IDX_MASK          (0x7)
#define RPM_MAX_RESOURCE_RESOURCE_SHADOWS (RESOURCE_SHADOW_IDX_MASK + 1)
#define RESOURCE_SHADOW_IDX_UNHASHED_MASK (0x80)
#define RESOURCE_SHADOW_IDX(idx)          ((idx) & RESOURCE_SHADOW_IDX_MASK)
#define RESOURCE_SHADOW_IDX_HASHED(idx)   (!((idx) & RESOURCE_SHADOW_IDX_UNHASHED_MASK))
#define RESOURCE_SHADOW_IDX_NOT_SUPPORTED 0x7F
#define RESOURCE_SHADOW_NOT_SUPPORTED(idx) \
    (((idx) & RESOURCE_SHADOW_IDX_NOT_SUPPORTED) == RESOURCE_SHADOW_IDX_NOT_SUPPORTED)

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

static ResourceData rpm_resource_shadow[RPM_MAX_RESOURCE_RESOURCE_SHADOWS];
static unsigned rpm_resource_shadow_count;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------
uint32_t rpm_system_hash = 0xdce80246;

//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================

static ResourceData* resource_shadow(unsigned shadow_idx)
{
    shadow_idx = RESOURCE_SHADOW_IDX(shadow_idx);
    CORE_VERIFY(shadow_idx < RPM_MAX_RESOURCE_RESOURCE_SHADOWS);
    return &rpm_resource_shadow[shadow_idx];
}

static bool resource_is_hashed(uint32_t type, unsigned id)
{
    const rpm_resource_config_t *resource_cfg = RESOURCE_CONFIG_DATA;
    if (!resource_cfg) return true;
    while (resource_cfg->type != 0)
    {
        if ((resource_cfg->type == type) && (resource_cfg->id == id))
            return (resource_cfg->hashed);
        resource_cfg++;
    }
    return true;
}

unsigned rpm_init_shadow(
    unsigned          id,
    unsigned          class_idx
)
{
    CORE_VERIFY(rpm_resource_shadow_count<RPM_MAX_RESOURCE_RESOURCE_SHADOWS);
    unsigned new_shadow_index = rpm_resource_shadow_count;
    rpm_resource_shadow_count++;

    ResourceData* shadow_ptr = &rpm_resource_shadow[new_shadow_index];

    resource_init(shadow_ptr, class_idx, id);

    if(resource_is_hashed(rpm->classes[shadow_ptr->class_idx].type, id))
        return new_shadow_index;
    else
        return (new_shadow_index | RESOURCE_SHADOW_IDX_UNHASHED_MASK);
}

EEClient* resource_lookup_ee_client(const ResourceData *r, unsigned ee)
{
    CORE_VERIFY(r);

    EEClient* client = r->ees;

    while (client && (client->ee_id != ee))
    {
        client = client->next;
    }

    return client;
}

EEClient* resource_lookup_and_alloc_client(ResourceData *r, unsigned ee)
{
    EEClient* client = resource_lookup_ee_client(r, ee);

    if(!client)
    {
        CORE_VERIFY(r->num_eecs < rpm->num_ees);
        client = (EEClient*)malloc(sizeof(EEClient));
        CORE_VERIFY(client);
        memset(client, 0, sizeof(EEClient));
        client->ee_id = ee;
        client->next  = r->ees;
        r->ees        = client;
        r->num_eecs++;
    }

    return client;
}

// Debug code to ensure that rpm_get_current_request returns the correct data.
// Adding this for a while to ensure that there are no corner cases we've missed
void resource_validate_current_request(rpm_application_info* info, int request_size)
{
    void* current_request;
    unsigned client_type;

    rpm_get_current_request(info->resource_handle, info->client, &client_type, (const void **)&current_request);

    CORE_VERIFY(client_type == info->client_type);

    if(current_request)
    {
        CORE_VERIFY(info->new_state);
        CORE_VERIFY(!memcmp(current_request, info->new_state, request_size));
    }
    else
    {
        CORE_VERIFY(!info->new_state);
    }
}

/*===========================================================================
FUNCTION: resource_init(ResourceData *r, unsigned class_idx, unsigned id)

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void resource_init(ResourceData *r, unsigned class_idx, unsigned id)
{
    memset(r, 0, sizeof(ResourceData));

    r->id = id;
    r->class_idx = class_idx;

    unsigned inrep_size = rpm->classes[class_idx].inrep_size;
    r->current_state = malloc(inrep_size);
    memset(r->current_state, 0, inrep_size);

    rpm_system_hash = rpm_resource_hash(rpm_system_hash, r, inrep_size, NULL, r->current_state);

    r->ees = NULL;
    r->shadow_idx = RESOURCE_SHADOW_IDX_NOT_SUPPORTED;
    if(!resource_is_hashed(rpm->classes[r->class_idx].type, id))
        r->shadow_idx |= RESOURCE_SHADOW_IDX_UNHASHED_MASK;
}

/*===========================================================================
FUNCTION: ee_target_slot

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static inline bool ee_target_slot(EEClient *client, bool is_request, rpm_set_type set, unsigned &target_slot)
{
    unsigned         selected     = GET_LSB(client->selected);
    unsigned         other        = OTHER_IDX(client->selected);
    rpm_request_type current_slot = client->req_type[selected];
    rpm_request_type other_slot   = client->req_type[other];
    rpm_request_type incoming_req = (rpm_request_type)(set + 1);

    // This decision tree is quite ugly, but it's roughly structured as:
    //   if you can match the set exactly, that's what's affected
    //   otherwise if it's an incoming request
    //      put it in the first empty slot you can find
    //      or else if it's NAS, let it overwrite the active set slot
    //      otherwise just put it in the buffer we're not using right now
    //   lastly, if it's an invalidate and it didn't match exactly, we've got nothing
    if(current_slot == incoming_req)
        target_slot = selected;
    else if(other_slot == incoming_req)
        target_slot = other;
    else if(is_request)
    {
        if(current_slot == NO_REQUEST)
            target_slot = selected;
        else if(other_slot == NO_REQUEST)
            target_slot = other;
        else if(current_slot == ACTIVE_REQUEST && incoming_req == NEXT_ACTIVE_REQUEST)
            target_slot = selected;
        else
            target_slot = other;
    }
    else
        return false; // There is no valid buffer for this request.

    // We found a correct target slot.
    return true;
}

/*===========================================================================
FUNCTION: translate_request

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static inline void translate_request(ResourceData *r, unsigned ee, kvp_t *data, void *source_buffer, void *dest_buffer)
{
    ResourceClassData *rc = &(rpm->classes[r->class_idx]);

    // Deal with getting the "right" initial data into the destination buffer.
    // If there is no source, that means we must zero the buffer.  If there is
    // a source that is not the same buffer, we must copy.
    if(!source_buffer)
        memset(dest_buffer, 0, rc->inrep_size);
    else if(source_buffer != dest_buffer)
        memcpy(dest_buffer, source_buffer, rc->inrep_size);

    // Now we call the xlate function with the appropriate context.
    // If aggregate initialization is used here, compiler generates redundant zeroing instructions.
    // So, we end up spelling things out line by line to get better code from the compiler.
    rpm_translation_info xinfo;
    xinfo.cb_data     = rc->cb_data;
    xinfo.id          = r->id;
    xinfo.client      = ee;
    xinfo.client_type = 0;
    xinfo.dest_buffer = dest_buffer;
    xinfo.new_kvps    = data;

    SWEVENT(RPM_XLATE_REQUEST, rc->type, xinfo.id);

    rc->xlate(&xinfo);
}

static uint64 resource_ee_apply(ResourceData *r, unsigned ee, void* old_request, void* new_request, uint64 settling_time)
{
    rpm_application_info ainfo;
    ResourceClassData *rc     = &(rpm->classes[r->class_idx]);

    // Same aggregate initialization issue as in translate.
    ainfo.cb_data             = rc->cb_data;
    ainfo.id                  = r->id;
    ainfo.client              = ee;
    ainfo.client_type         = 0;
    ainfo.old_state           = old_request;
    ainfo.new_state           = new_request;
    ainfo.current_aggregation = r->current_state;
    ainfo.settling_time       = settling_time;
    ainfo.resource_handle     = r;

    void *old_state = alloca(rc->inrep_size);
    memcpy(old_state, r->current_state, rc->inrep_size);
    SWEVENT(RPM_APPLY_REQUEST, rc->type, ainfo.id);
    rpm_apply_cb apply = rc->apply;
    resource_validate_current_request(&ainfo, rc->inrep_size);
    apply(&ainfo);

    rpm_system_hash = rpm_resource_hash(rpm_system_hash, r, rc->inrep_size, old_state, r->current_state);
    return ainfo.settling_time;
}

uint64 resource_ee_settling_apply(ResourceData *r, unsigned ee, uint64 settling_time)
{
     EEClient *client = resource_lookup_and_alloc_client(r, ee);
    return resource_ee_apply(r, ee, client->request[client->selected], client->request[client->selected], settling_time);
}
bool do_resource_ee_request(ResourceData *r, unsigned ee, rpm_set_type set, kvp_t *data, uint64 &settling_time)
{
    EEClient          *client = resource_lookup_and_alloc_client(r, ee);
    ResourceClassData *rc     = &(rpm->classes[r->class_idx]);

    // First we need to figure out if we have a valid buffer for this request
    // at all. For instance, this could be an invalidation of a set that we
    // don't even have stored.
    bool     is_request = !!data; // if the data pointer is NULL, this is an invalidate, if not, a request
    unsigned target_slot;
    if(!ee_target_slot(client, is_request, set, target_slot))
        return false; // No valid buffer affected by this request, just bail.

    unsigned         selected     = GET_LSB(client->selected);
    unsigned         other        = OTHER_IDX(client->selected);
    rpm_request_type current_slot = client->req_type[selected];
    rpm_request_type other_slot   = client->req_type[other];
    rpm_request_type incoming_req = (rpm_request_type)(set + 1);
    rpm_request_type overwritten_request_type = client->req_type[target_slot];

    // We need to know what source buffer we're going to translate from.
    void *xlate_source = NULL;
    if(is_request)
    {
        if(client->req_type[target_slot] == incoming_req)
            xlate_source = client->request[target_slot];
        else if((incoming_req == NEXT_ACTIVE_REQUEST) && (client->req_type[1 ^ target_slot] == ACTIVE_REQUEST))
            xlate_source = client->request[1 ^ target_slot];
        else if((incoming_req == NEXT_ACTIVE_REQUEST) && (client->req_type[target_slot] == ACTIVE_REQUEST))
            xlate_source = client->request[target_slot];
        else
            xlate_source = NULL;
    }

    // Update the client with what we're storing in the target buffer.
    if(!is_request)
        client->req_type[target_slot] = NO_REQUEST; // invalidates leave no request behind
    else if((target_slot == selected) && (incoming_req == NEXT_ACTIVE_REQUEST) && (overwritten_request_type == ACTIVE_REQUEST))
        client->req_type[target_slot] = ACTIVE_REQUEST;
    else
        client->req_type[target_slot] = incoming_req;

    // Make sure there's a place to store *into*.
    if(is_request && !client->request[target_slot])
        client->request[target_slot] = malloc(rc->inrep_size);

    // Handle the translation of KVP's into an inrep buffer.  In a few cases, this is all that we'll need to do.
    bool has_apply_step = true;
    bool fresh_active   = (overwritten_request_type == NO_REQUEST) && (incoming_req == ACTIVE_REQUEST);
    bool bare_nas       = (current_slot == NO_REQUEST) && (incoming_req == NEXT_ACTIVE_REQUEST);
    if(bare_nas && (selected == target_slot))
    {
        client->selected = other;
        has_apply_step   = false;
    }
    else if(bare_nas || (target_slot != selected && !fresh_active))
    {
        has_apply_step = false;
    }

    // Now we need to worry about saving the old request if we're going to reuse its buffer.
    void *old_request = NULL;
    if(current_slot == NO_REQUEST)
        old_request = NULL;
    else if(((overwritten_request_type == NO_REQUEST) && (incoming_req == ACTIVE_REQUEST)) || !is_request)
        old_request = client->request[selected];
    else if(has_apply_step)
    {
        unsigned inrep_size = rc->inrep_size;
        old_request = alloca(inrep_size);
        memcpy(old_request, client->request[selected], inrep_size);
    }

    // Do the actual translation--that is, take the KVP's sent from the master
    // and turn it into an internal representation useful for resource
    // aggregation.
    if(is_request)
    {
        // Make sure there's a place to store *into*.
        if(!client->request[target_slot])
            client->request[target_slot] = malloc(rc->inrep_size);
        translate_request(r, ee, data, xlate_source, client->request[target_slot]);
    }

    // At this point we've saved the request successfully.  If it's not to be
    // used immediately, then we're done and can exit.
    if(!has_apply_step)
        return false;

    // Figure out where the new request to send to the driver is.
    void *new_request;
    if(is_request)
        new_request = client->request[target_slot];
    else if((other_slot == SLEEP_REQUEST) || (other_slot == ACTIVE_REQUEST))
        new_request = client->request[other];
    else
        new_request = NULL;

    //client->req_type is size 2 as we are either applying active or sleep set
    CORE_VERIFY( client->selected < RPM_NEXT_ACTIVE_SET );

    // Finally, as the last step before application, update which buffer we (will have) applied.
    if((client->req_type[client->selected] == NO_REQUEST) &&
       (client->req_type[1 ^ client->selected] != NO_REQUEST) &&
       (client->req_type[1 ^ client->selected] != NEXT_ACTIVE_REQUEST))
    {
        // If we're looking at "nothing" but we have and active or sleep
        // request in our back pocket, use that.
        client->selected = 1 ^ client->selected;
    }
    else
        client->selected = target_slot; // Use the thing we just wrote.

    // Same aggregate initialization issue as in translate.

    settling_time = resource_ee_apply(r, ee, old_request, new_request, 0);

    return true;



}

/*===========================================================================
FUNCTION: resource_ee_request

DESCRIPTION:

RETURN VALUE:
===========================================================================*/

static kvp_t* resource_shadow_kvp;
static bool resource_ee_request_reentrant;

bool resource_ee_request(ResourceData *r, unsigned ee, rpm_set_type set, kvp_t *data, uint64 &settling_time)
{
    //Since resource_shadow_kvp can only be used once, code assumes that this function doesn't become re-entrant.
    //Guard against it explicitly.
    CORE_VERIFY(!resource_ee_request_reentrant);
    resource_ee_request_reentrant = true;

    //Simple case that the resource isn't shadowed.
    if(RESOURCE_SHADOW_NOT_SUPPORTED(r->shadow_idx))
    {
        bool ret = do_resource_ee_request(r, ee, set, data, settling_time);
        resource_ee_request_reentrant = false;
        return ret;
    }

    kvp_t* data_to_pass_to_shadow = NULL;

    //Handle shadowed kvp.
    //Init our resource_shadow_kvp. Data can be NULL in the case that we're doing a full resource invalidate so need to
    //account for that.
    if(data)
    {
        if(!resource_shadow_kvp)
        {
            resource_shadow_kvp = kvp_frombuffer(kvp_get_raw_size(data), kvp_get_raw_bytes(data));
        }
        else
        {
            kvp_swapbuffer(resource_shadow_kvp, kvp_get_raw_bytes(data), kvp_get_raw_size(data));
        }
        data_to_pass_to_shadow = resource_shadow_kvp;
    }

    //Expectation is that shadow resources shouldn't require any settling.
    uint64 dummy_settling_time = 0;
    do_resource_ee_request(resource_shadow(r->shadow_idx), ee, set, data_to_pass_to_shadow, dummy_settling_time);
    CORE_VERIFY(dummy_settling_time==0);

    //It's possible that the primary resource would support settling, but in the case that the resource
    //is shadowed we want to just make the transition sequential.
    //If it's ever shown to be a bottleneck we can revisit.
    settling_time = 0;
    bool ret = do_resource_ee_request(r, ee, set, data, settling_time);
    while (settling_time)
    {
        settling_time = resource_ee_settling_apply(r, ee, settling_time);
    }

    resource_ee_request_reentrant = false;
    return ret;
}

uint64 do_resource_ee_transition(ResourceData *r, unsigned ee, rpm_transition_type transition)
{
    EEClient          *client    = resource_lookup_and_alloc_client(r, ee);
    uint8_t            selected  = GET_LSB(client->selected);
    rpm_request_type  *curr_type = &(client->req_type[selected]);
    unsigned           off_idx   = OTHER_IDX(client->selected);
    rpm_request_type  *off_type  = &(client->req_type[off_idx]);

    bool do_transition = resource_ee_has_transition_work(r, ee, transition);
    if(!do_transition)
        return 0;

    // If we are about to jump to the next active set, it's now *the* active set.
    // That might also mean that the previous active set is squashed.
    if(NEXT_ACTIVE_REQUEST == *off_type)
    {
        *off_type = ACTIVE_REQUEST;
        if(*curr_type == ACTIVE_REQUEST)
            *curr_type = NO_REQUEST;
    }

    // Finally, we have now effectively selected the new request we sent out.
    client->selected = off_idx;

    return resource_ee_apply(r, ee, client->request[selected], client->request[off_idx], 0);
}

/*===========================================================================
FUNCTION: resource_ee_transition

DESCRIPTION:

RETURN VALUE:
===========================================================================*/

uint64 resource_ee_transition(ResourceData *r, unsigned ee, rpm_transition_type transition)
{
    //Handle the easy case first where a resource is not shadowed.

    if(RESOURCE_SHADOW_NOT_SUPPORTED(r->shadow_idx))
    {
        return do_resource_ee_transition(r, ee, transition);
    }

    //Now handle transitioning shadowed resources.
    //Expectation is that shadow resources should not return a settling time. Assert it.
    CORE_VERIFY(do_resource_ee_transition(resource_shadow(r->shadow_idx), ee, transition)==0);

    //The primary resource could return a settling time which we need to handle.
    //Make the resource transition happen sequentially by forcing the resource to settle before returning.

    uint64 settling_time = do_resource_ee_transition(r, ee, transition);
    while(settling_time)
    {
        settling_time = resource_ee_settling_apply(r, ee, settling_time);
    }

    return 0;
}

/*===========================================================================
FUNCTION: resource_ee_has_transition_work

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
bool resource_ee_has_transition_work(ResourceData *r, unsigned ee, rpm_transition_type transition)
{
     EEClient *client = resource_lookup_ee_client(r, ee);
    if(!client)
        return false;
    uint8_t other_idx = OTHER_IDX(client->selected);
    // uint8_t selected = GET_LSB(client->selected);
    // We need to call the driver's apply function if we're:
    //  1) going to sleep and have a sleep set request to change to, or...
    //  2) are waking up from the sleep set and have any other request
    if(ACTIVE_TO_SLEEP == transition)
    {
      return (client->req_type[other_idx] == SLEEP_REQUEST);
    }
    else // if(SLEEP_TO_ACTIVE == transition)
    {
      return ((client->req_type[other_idx] != NO_REQUEST) && (client->req_type[other_idx] != SLEEP_REQUEST));
    }
}

/*===========================================================================
FUNCTION: resource_num_clients

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
unsigned resource_num_clients(const ResourceData *r)
{
    return rpm->num_ees + r->num_ics;
}

/*===========================================================================
FUNCTION: resource_get_request

DESCRIPTION:
s
RETURN VALUE:
===========================================================================*/
void resource_get_request(const ResourceData *r, unsigned client, unsigned *client_type, const void **request)
{
    CORE_VERIFY(client < resource_num_clients(r));

    if(client < rpm->num_ees)
    {
        // Requests from external EE's occupy the lowest client indices, and have "no" type.
        *client_type = 0;

      EEClient *ee = resource_lookup_ee_client(r, client);
        if(!ee)
            *request = 0;
        else if(ee->req_type[ee->selected] != NO_REQUEST)
            *request = ee->request[ee->selected];
        else
            *request = 0;
    }
    else
    {
        // Internal clients occupy the >= num_ee indices.
        unsigned client_idx = client - rpm->num_ees;
        const InternalClient *ic = &(r->ics[client_idx]);

        *client_type = ic->type;
        *request = ic->request;
    }
}

/*===========================================================================
FUNCTION: resource_internal_client_size

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
unsigned resource_internal_client_size(ResourceClassData *rc, unsigned client_type)
{
    if(client_type == 0)
    {
        return rc->inrep_size;
    }

    InternalClientType *ict = rc->internal_types;
    while(ict)
    {
        if(ict->client_type == client_type)
        {
            return ict->inrep_size;
        }
        ict = ict->next;
    }

    CORE_VERIFY(0);  //Should never get here.
    return 0;
}

/*===========================================================================
FUNCTION: resource_create_client

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
unsigned resource_create_client(ResourceData *r, unsigned client_type)
{
    ResourceClassData *rc         = &(rpm->classes[r->class_idx]);
    unsigned           inrep_size = 0;

    if(client_type == 0)
    {
        inrep_size = rc->inrep_size;
    }
    else
    {
        inrep_size = resource_internal_client_size(rc, client_type);
    }

    // If we didn't find this client type, we can't proceed.
    CORE_VERIFY(inrep_size != 0);

    unsigned new_client = r->num_ics;
    r->num_ics++;
    r->ics = (InternalClient *)realloc(r->ics, r->num_ics * sizeof(InternalClient));
    CORE_VERIFY_PTR(r->ics);
    r->ics[new_client].type    = client_type;
    r->ics[new_client].request = malloc(inrep_size);
    memset(r->ics[new_client].request, 0, inrep_size);

    return rpm->num_ees + new_client;
}

/*===========================================================================
FUNCTION: resource_client_request

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
uint64 resource_client_request(ResourceData *r, unsigned client, unsigned size, void *data, uint64 settling_time)
{
    ResourceClassData *rc              = &(rpm->classes[r->class_idx]);
    unsigned           internal_client = client - rpm->num_ees;

    CORE_VERIFY(client >= rpm->num_ees);
    CORE_VERIFY(client < (rpm->num_ees + r->num_ics));
    CORE_VERIFY(size>0);

    InternalClient *ic = &(r->ics[internal_client]);
    CORE_VERIFY(resource_internal_client_size(rc, ic->type) == size);

    void *old_request = alloca(size);
    memcpy(old_request, ic->request, size);
    memcpy(ic->request, data, size);

    rpm_application_info info;
    info.cb_data             = rc->cb_data;
    info.id                  = r->id;
    info.client              = client;
    info.client_type         = ic->type;
    info.old_state           = old_request;
    info.new_state           = ic->request;
    info.current_aggregation = r->current_state;
    info.settling_time       = settling_time;
    info.resource_handle     = r;

    void *old_state = alloca(rc->inrep_size);
    memcpy(old_state, r->current_state, rc->inrep_size);

    rpm_apply_cb apply = rc->apply;

    resource_validate_current_request(&info, size);
    apply(&info);

    if(RESOURCE_SHADOW_IDX_HASHED(r->shadow_idx))
        rpm_system_hash = rpm_resource_hash(rpm_system_hash, r, rc->inrep_size, old_state, r->current_state);

    return info.settling_time;
}

/*===========================================================================
FUNCTION: resource_class_add_client_type

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void resource_class_add_client_type(ResourceClassData *rc, unsigned client_type, unsigned inrep_size)
{
    // client type 0 is reserved for the default inrep size (as for requests from outside the RPM)
    CORE_VERIFY(client_type != 0);

    InternalClientType *ict = rc->internal_types;
    while(ict)
    {
        if(ict->client_type == client_type)
        {
            CORE_VERIFY(inrep_size == ict->inrep_size);
            return;
        }

        ict = ict->next;
    }

    ict = new InternalClientType;
    ict->client_type = client_type;
    ict->inrep_size  = inrep_size;
    ict->next        = rc->internal_types;

    rc->internal_types = ict;
}

uint64 resource_ee_settling_query(ResourceData *r, unsigned ee)
{
    ResourceClassData *rc     = &(rpm->classes[r->class_idx]);
    rpm_application_info      ainfo;
    ainfo.cb_data             = rc->cb_data;
    ainfo.id                  = r->id;
    ainfo.client              = ee;
    ainfo.client_type         = 0;
    ainfo.old_state           = NULL;
    ainfo.new_state           = NULL;
    ainfo.current_aggregation = NULL;
    ainfo.settling_time       = 0;
    ainfo.resource_handle     = r;

    rpm_query_npa_settling(&ainfo);
    return ainfo.settling_time;
}

