/*===========================================================================

  Copyright (c) 2012-2014 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include <assert.h>
#include <stdlib.h>

#include "rpmserver.h"
#include "rpm_config.h"
#include "CoreVerify.h"

extern "C"
{
    #include "npa.h"
    #include "npa_resource.h"
    #include "ClockDefs.h"
    int rpm_npa_get_ee_client_id(const npa_client* client);
	int rpm_npa_get_client(const npa_client* client );
}

typedef struct
{
    npa_resource_state request;
    npa_resource_state max_limit;
} npa_request_data_t;

struct NPAMapping
{
    const char  *resourceName;
    npa_client **request_handles;
    npa_client **max_handles;

    NPAMapping() : resourceName(0)
    {
        unsigned size = rpm->num_ees * sizeof(npa_client *);

        request_handles = (npa_client **)malloc(size);
        memset(request_handles, 0, size);

        max_handles = (npa_client **)malloc(size);
        memset(max_handles, 0, size);
    }
};

struct rpm_npa_adapter_s
{
    unsigned    numResources;
    NPAMapping *resources;
    boolean needs_settling;

    rpm_npa_adapter_s(unsigned num_resources) : numResources(num_resources), resources(new NPAMapping[num_resources]), needs_settling(false) {}
    ~rpm_npa_adapter_s() { delete [] resources; }
};

typedef struct
{
    const char *name;
    const npa_client *ptr;
} client_map_t;

// TODO: replace with enumeration data instead of static table at some point
// client lookup table
static client_map_t ee_client_lut[] =
{
    {"APSS" , NULL},
    {"MPSS" , NULL},
    {"WCSS" , NULL},
	{"TZ"   , NULL},
    {"LPASS", NULL},
};

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

static npa_client *rpm_npa_ensure_client(NPAMapping &mapping, unsigned client_id, npa_client **handles, npa_client_type type)
{
    CORE_VERIFY(mapping.resourceName);
    CORE_VERIFY(client_id < rpm->num_ees);

    npa_client *client = handles[client_id];
    if(!client)
    {
        const char *name;
        if(client_id < ARRAY_SIZE(ee_client_lut))
            name = ee_client_lut[client_id].name;
        else
            name = "unknown client via rpm-npa adapter";
        handles[client_id] = client = npa_create_sync_client(mapping.resourceName, name, type);
    }
    
    CORE_VERIFY(client);
    return client;
}

int rpm_npa_get_client(const npa_client* client )
{
    int client_id = -1;
    int i;

    CORE_VERIFY_PTR(client);

	for(i = 0; i < ARRAY_SIZE(ee_client_lut); i++)
    {
        if(0 == strcmp(client->name, ee_client_lut[i].name))
        {
            client_id = i;
            break;
        }
    }
	
	return client_id;
}
	
int rpm_npa_get_ee_client_id(const npa_client* client)
{
    int client_id = -1;
    int i;

    CORE_VERIFY_PTR(client);

    for(i = 0; i < ARRAY_SIZE(ee_client_lut); i++)
    {
        if(ee_client_lut[i].ptr == client)
        {
            client_id = i;
            break;
        }
        else if(ee_client_lut[i].ptr == NULL && 0 == strcmp(client->name, ee_client_lut[i].name))
        {
            ee_client_lut[i].ptr = client;
            client_id = i;
            break;
        }
    }

    return client_id;
}

static npa_client *rpm_npa_ensure_request_client(NPAMapping &mapping, unsigned client_id)
{
    return rpm_npa_ensure_client(mapping, client_id, mapping.request_handles, NPA_CLIENT_REQUIRED);
}

static npa_client *rpm_npa_ensure_max_client(NPAMapping &mapping, unsigned client_id)
{
    return rpm_npa_ensure_client(mapping, client_id, mapping.max_handles, NPA_CLIENT_LIMIT_MAX);
}

static void rpm_npa_xlate(rpm_translation_info *info)
{
    rpm_npa_adapter adapter    = (rpm_npa_adapter)info->cb_data;
    npa_request_data_t *xlated = (npa_request_data_t *)info->dest_buffer;

    CORE_VERIFY(adapter);
    CORE_VERIFY(info->id < adapter->numResources);

    npa_client *client = rpm_npa_ensure_request_client(adapter->resources[info->id], info->client); 
    CORE_VERIFY(client);
    const char *units  = client->resource->definition->units;
    const unsigned max = 0x0078616d; // 'max' in little endian

    unsigned type, length, value;
    const unsigned *raw_value;
    while(kvp_get(info->new_kvps, &type, &length, (const char **)&raw_value))
    {
        // This resource only accepts 4-byte data, or 0-byte invalidates, so normalize.
        if(sizeof(npa_resource_state) == length)
            value = *raw_value;
        else if(!length)
            value = 0;
        else
            continue;

        if(max == type)
        {
            // Handle 'max' requests
            xlated->max_limit = value;
        }
        else if(!strncmp((const char *)&type, units, sizeof(type)))
        {
            // Handle requests to the resource itself
            xlated->request = value;
        }
        else
        {
            // Unknown request; ignore.
            continue;
        }
    }
}

static void rpm_npa_apply(rpm_application_info *info)
{
    rpm_npa_adapter     adapter = (rpm_npa_adapter)info->cb_data;
    npa_request_data_t *old_req = (npa_request_data_t *)info->old_state;
    npa_request_data_t *new_req = (npa_request_data_t *)info->new_state;

    CORE_VERIFY(adapter);
    CORE_VERIFY(info->id < adapter->numResources);

    npa_client *request_client = rpm_npa_ensure_request_client(adapter->resources[info->id], info->client); 

    // Handle invalidation.
    if(!new_req)
    {
        npa_client *client = adapter->resources[info->id].request_handles[info->client];
        if(client) npa_complete_request(client);

        client = adapter->resources[info->id].max_handles[info->client];
        if(client) npa_complete_request(client);
    }
    else if(!old_req) // Handle initialization.
    {
        npa_issue_required_request(request_client, new_req->request);

        if(new_req->max_limit)
        {
            npa_client *max_client = rpm_npa_ensure_max_client(adapter->resources[info->id], info->client); 
            npa_issue_limit_max_request(max_client, new_req->max_limit);
        }
    }
    else if((adapter->needs_settling) && (info->settling_time != 0)) //handle a resource thats settling
    {
        if(old_req == new_req)
        {
            NPA_PENDING_REQUEST(request_client) = NPA_ACTIVE_REQUEST(request_client);
        }
        npa_issue_internal_request(request_client);
    }
    else // Handle the steady state.
    {
	    // Change the client request.
        npa_issue_required_request(request_client, new_req->request);
		
        // Deal with the maximum limit, if required.
        if(old_req->max_limit != new_req->max_limit)
        {
            npa_client *max_client = rpm_npa_ensure_max_client(adapter->resources[info->id], info->client); 
            npa_issue_limit_max_request(max_client, new_req->max_limit);
        }
        
    }

    // Update the resource status.
    npa_query_type   result;
    npa_query_status status = npa_query_by_client(request_client, NPA_QUERY_CURRENT_STATE, &result);

    CORE_VERIFY(NPA_QUERY_SUCCESS == status);
    CORE_VERIFY(NPA_QUERY_TYPE_STATE == result.type);
    *((npa_resource_state *)info->current_aggregation) = result.data.state;
    if(adapter->needs_settling)
    {
        npa_query_type   settling;
        npa_query_status settling_status = npa_query_by_client(request_client, CLOCK_NPA_QUERY_SETTLING_TIME, &settling);
        if((settling_status == NPA_QUERY_SUCCESS) && (settling.data.state != 0))
            info->settling_time = time_service_now() + settling.data.state;
        else
            info->settling_time = 0;
    }
}
rpm_npa_adapter rpm_create_npa_settling_adapter(rpm_resource_type resource, unsigned num_npa_resources)
{
    rpm_npa_adapter adapter = new rpm_npa_adapter_s(num_npa_resources);
    adapter->needs_settling = true;
    rpm_register_resource(resource, num_npa_resources, sizeof(npa_request_data_t), rpm_npa_xlate, rpm_npa_apply, adapter);
    return adapter;
}

rpm_npa_adapter rpm_create_npa_adapter(rpm_resource_type resource, unsigned num_npa_resources)
{
    rpm_npa_adapter adapter = new rpm_npa_adapter_s(num_npa_resources);

    rpm_register_resource(resource, num_npa_resources, sizeof(npa_request_data_t), rpm_npa_xlate, rpm_npa_apply, adapter);

    return adapter;
}

void rpm_associate_npa_resource(rpm_npa_adapter adapter, unsigned resource_id, const char *resource_name)
{
    CORE_VERIFY(adapter);
    CORE_VERIFY(resource_id < adapter->numResources);
    CORE_VERIFY(resource_name);

    NPAMapping &mapping = adapter->resources[resource_id];
    CORE_VERIFY(!mapping.resourceName);

    mapping.resourceName = resource_name;
}

void rpm_query_npa_settling(rpm_application_info *info)
{
    rpm_npa_adapter     adapter = (rpm_npa_adapter)info->cb_data;
    CORE_VERIFY(adapter);
    CORE_VERIFY(info->id < adapter->numResources);
    npa_client *request_client = rpm_npa_ensure_request_client(adapter->resources[info->id], info->client);
    if(adapter->needs_settling)
    {
        npa_query_type   settling;
        npa_query_status settling_status = npa_query_by_client(request_client, CLOCK_NPA_QUERY_SETTLING_TIME, &settling);
        if((settling_status == NPA_QUERY_SUCCESS) && (settling.data.state != 0))
            info->settling_time = time_service_now() + settling.data.state;
        else
            info->settling_time = 0;
    }
}
