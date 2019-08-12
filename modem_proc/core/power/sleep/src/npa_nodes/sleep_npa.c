/*============================================================================
  FILE:         sleep_npa.c
  
  OVERVIEW:     This file provides the NPA node definitions for latency and
                wakeup nodes.
 
                Copyright (c) 2010-2015 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

  $Header: //source/qcom/qct/core/power/sleep/rel/1h10/src/common/sleep_npa.c
============================================================================*/

#ifndef FEATURE_SLEEP_NO_WAKEUP_NODE
#include "CoreTime.h"
#endif /* !FEATURE_SLEEP_NO_WAKEUP_NODE */

#if !defined( FEATURE_SLEEP_NO_WAKEUP_NODE ) || \
    !defined( FEATURE_SLEEP_NO_LATENCY_NODE ) || \
    !defined( FEATURE_SLEEP_NO_MAX_DURATION_NODE )
#include "CoreVerify.h"
#include "CoreString.h"
#include "sleep_lpr.h"
#include "npa_resource.h"
#include "sleepi.h"
#include "sleep_v.h"
#include <stdio.h>
#include <stdlib.h>
#endif /* !FEATURE_SLEEP_NO_WAKEUP_NODE */
       /* || !FEATURE_SLEEP_NO_LATENCY_NODE */
       /* || !FEATURE_SLEEP_NO_MAX_DURATION_NODE */

#ifndef FEATURE_SLEEP_NO_LATENCY_NODE
#include "sleep_idle_plugin.h"
#endif /* !FEATURE_SLEEP_NO_LATENCY_NODE */

#ifndef FEATURE_SLEEP_NO_MAX_DURATION_NODE
/*==============================================================================
                         SLEEP MAX DURATION NODE DEFINITION
 =============================================================================*/
#define MAX_DURATION_TICKS "/sleep/max_duration"
#define MAX_DURATION_USEC  "/sleep/max_duration/usec"

/**
 * maxDurationResourceData_createClientFcn
 *
 * @brief In order to specify to driver function the untis chosen 
 *        by the client to this resource
 *
 * Custom client function to determine units specified by client
 *
 * @param client: The handle to the client registered to the 
 *                resource.
 *
 * @return None
 */
void maxDurationResourceData_createClientFcn(npa_client *client)
{
  if(strncmp(client->resource_name, MAX_DURATION_USEC,
                   strlen(MAX_DURATION_USEC)) == 0)
  {
    /* client data in usec */
    client->resource_data = (void *)1;
  }
  else
  {
    /* client data in ticks */
    client->resource_data = (void *)0;
  }
}

/**
 * sleepNPA_maxDurationUpdate
 *
 * @brief This function is invoked by the /sleep/max_duration resource
 *        when a client request is made.
 *
 * Determines the new state of the resource by calculating max of the
 * clients' requests 
 *
 * @param resource: The NPA resource being requested (should be 
 *                  /sleep/max_duration).
 * @param client: The handle to the clients registered to the 
 *                resource.
 *
 * @return Returns the new state of the resource.
 *
 */
npa_resource_state sleepNPA_maxDurationUpdate(npa_resource *resource, 
                                              npa_client_handle client)
{
  static npa_resource_state req_state = 0;
  npa_resource_state request;

  CORE_VERIFY_PTR(resource);
  CORE_VERIFY_PTR(client);

  if (client->resource_data == (void *)1)
  {
    /* convert usec to ticks */
    request = NPA_PENDING_REQUEST(client).state;
    NPA_PENDING_REQUEST(client).state = US_TO_TICKS( request );
  }

  /* Client is requesting a specific point at which we should be awake.
   * Take the min of the client votes.  This will return the min of the
   * required and suppressible clients. */
  req_state = npa_min_plugin.update_fcn( resource, client );

  return req_state;
}

/**
 * sleepNPA_maxDurationDriver
 *
 * @brief Driver function for the /sleep/max_duration resource 
 *        that updates its state.
 *
 * @param resource: Pointer to the /sleep/max_duration resource
 * @param client: Client of the resource
 * @param state: State of the resource.
 *
 * @return state of the resource.
 */
static npa_resource_state sleepNPA_maxDurationDriver(npa_resource *resource,
                                                     npa_client *client,
                                                     npa_resource_state state)
{
  /* Record the state so it can be queried from the sleep task before we go
   * to sleep. */
  if( !state )
  {  
    state = NPA_MAX_STATE;
  }

  return state;
}

/**
 * sleepNPA_maxDurationQuery
 *
 * @brief Query function for the /sleep/max_duration resource that returns
 *        soft wakeup time
 *
 * @param resource: Resource to which we are making request (max_duration node)
 * @param query_id: ID for the query being made.
 * @param query_result: Stores to the result of the query
 *
 * @return Returns the status for the query (i.e. successfully handled or 
 *         unsupported).
 */
npa_query_status sleepNPA_maxDurationQuery
(
  npa_link *resource_link,
  unsigned int query_id, 
  npa_query_type *query_result
)
{
  npa_query_status status = NPA_QUERY_SUCCESS;
  npa_resource_state retVal;

  CORE_VERIFY_PTR(resource_link);
  CORE_VERIFY_PTR(query_result);

  switch(query_id)
  {
    case NPA_QUERY_CURRENT_STATE:
    case SLEEP_QUERY_MAX_DURATION:
    {
      retVal = resource_link->resource->active_state;
      if(strncmp(resource_link->name, MAX_DURATION_USEC,
                   strlen(MAX_DURATION_USEC)) == 0)
      {
        /* client request in usec, so return in usec */
        retVal = TICKS_TO_US(retVal);
      }
      query_result->data.value = retVal;
      query_result->type = NPA_QUERY_TYPE_VALUE;
      break;
    }
    default:
    {
      status = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      break;
    }
  }

  return status;
}
/**
 * @brief g_sleepMaxDurationPlugin
 *
 * The plugin for the /sleep/max_duration resource.
 */
const npa_resource_plugin g_sleepMaxDurationPlugin = 
{
  sleepNPA_maxDurationUpdate,                 /* Update function */
  NPA_CLIENT_REQUIRED,                        /* Supported client types */
  maxDurationResourceData_createClientFcn,    /* Create client function */
  NULL                                        /* Destroy client function */
};

/**
 * @brief g_sleepMaxDurationResource
 *
 * Definition of the max duration resource. 
 */
static npa_resource_definition g_sleepMaxDurationResource[] = 
{ 
  {  
    "/sleep/max_duration",                    /* Name */
    "ticks",                                  /* Units - usec for alias */
    NPA_MAX_STATE,                            /* Max State */
    &g_sleepMaxDurationPlugin,                /* Plugin */
    NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED, /* Attributes */
    NULL,                                     /* User Data */
    NULL,                                     /* Query Function */
    sleepNPA_maxDurationQuery                 /* Query Link Function */
  }
};

/**
 * @brief g_sleepMaxDurationNode
 *
 * Definition of the max duration node. 
 */
static npa_node_definition g_sleepMaxDurationNode = 
{ 
  "/node/sleep/max_duration",                 /* Name */
  sleepNPA_maxDurationDriver,                 /* Driver_fcn */
  NPA_NODE_DEFAULT,                           /* Attributes */
  NULL,                                       /* Data */
  NPA_EMPTY_ARRAY,                            /* Dependencies */
  NPA_ARRAY(g_sleepMaxDurationResource)       /* Resource Array */
};

#endif /* FEATURE_SLEEP_NO_MAX_DURATION_NODE */


#ifndef FEATURE_SLEEP_NO_WAKEUP_NODE
/*
===============================================================================

                         SLEEP WAKEUP NODE DEFINITION

===============================================================================
*/

/**
 * @brief WAKEUP_OVERDUE_SCLKS
 * 
 * Defines the number of sclks after which we start discarding 'overdue'
 * wakeup requests as being from invalid / broken code. 
 */
#define WAKEUP_OVERDUE_SCLKS 32768

/**
 * @brief WAKEUP_MAX_SCLKS
 *
 * Defines the maximum number of sclks ahead a client request can be, due to
 * the data types and units in use.  This is used for detecting bogus/stale
 * requests and dropping them from our calculations. 
 */
#define WAKEUP_MAX_SCLKS ((uint32)(0x7FFFFFFF))

/**
 * sleep_wakeup_discard_if_invalid
 *
 * @brief Discards the request if it was in 'distant' past.
 *
 * @param resource: Pointer to NPA resource (in this case resource for wakeup 
 *                  node).
 * @param client: Client whose request we want to check.
 * @param prev_client: Pointer to the previous node of the client (if any).
 * @param now: Current time stamp (in sclk)
 *
 * @return TRUE if the client request is discarded, else FALSE.
 */
static int sleep_wakeup_discard_if_invalid( npa_resource *resource, 
                                            npa_client **client,
                                            npa_client *prev_client, 
                                            uint32 now )
{
  npa_resource_state old_request;

  CORE_VERIFY_PTR(resource);
  CORE_VERIFY_PTR(client);

  old_request = NPA_ACTIVE_REQUEST(*client).state;
  if((old_request < now) &&          /* potential timer wrap or stale request */
     ((old_request - now) > WAKEUP_MAX_SCLKS) && /* ok, must be stale request */
     ((now - old_request) > WAKEUP_OVERDUE_SCLKS)) /* stale enough to discard */
  {
    /* drop this node from the list as invalid */
    if(!prev_client)
      resource->internal_state[0] =
               (npa_resource_state)((*client)->resource_data);
    else
      prev_client->resource_data = ((*client)->resource_data);

    *client = (*client)->resource_data;

    return TRUE;
  }

  return FALSE;
}

/**
 * sleep_wakeup_remove_client
 *
 * @brief Removes a client from the linked list that tracks wakeup values.
 *
 * @param resource: Resource whose client we want to remove
 * @param rm_client: Handle to the client we want to remove.
 */
static void sleep_wakeup_remove_client( npa_resource *resource, 
                                        npa_client_handle rm_client )
{
  npa_client_handle node, last_node;

  CORE_VERIFY_PTR(resource);
  CORE_VERIFY_PTR(rm_client);

  /* 
   * Crawl the list of requesting clients to find the one we want to remove.
   * The first internal state word is a pointer to the "earliest" expiring 
   * client. 
   */
  last_node = NULL;
  node = (npa_client_handle)resource->internal_state[0];

  /* 
   * The resource_data field of each client is a pointer to the "next earliest"
   * client.
   */
  while(node != rm_client && node)
  {
    /* 
     * first, discard this node if its wakeup is considerably in the past.
     * this is a failsafe mechanism that ensures a client that sets a wakeup
     * restriction and never removes it gets ignored eventually. 
     */
    if(sleep_wakeup_discard_if_invalid(resource, &node, 
                                       last_node, CoreTimetick_Get()))
      continue;

    last_node = node;
    node = (npa_client_handle)node->resource_data;
  }

  /* 
   * We've either found the rm_client or the end of the list.  If we found it,
   * remove it, otherwise just return. 
   */
  if(node == rm_client)
  {
    if(!last_node)
      resource->internal_state[0] = 
                              (npa_resource_state)rm_client->resource_data;
    else
      last_node->resource_data = rm_client->resource_data;
  }
}

/**
 * sleep_wakeup_insert_client
 *
 * @brief Inserts a client into the ordered linked list that tracks wakeup 
 *        values.
 *
 * @param resource: Pointer to resource where we want to register client (in
 *                  this case it will be the one for wake up node)
 * @param new_client: Handle to the client we want to register.
 * @param now: Current timestamp (in sclk)
 */
static void sleep_wakeup_insert_client( npa_resource *resource, 
                                        npa_client_handle new_client,
                                        uint32 now )
{
  npa_client_handle  node, last_node;
  npa_resource_state new_request, old_request;

  CORE_VERIFY_PTR(resource);
  CORE_VERIFY_PTR(new_client);

  new_request = NPA_PENDING_REQUEST(new_client).state;

  /* 
   * Crawl the list of requesting clients looking for where to insert it.
   * The first internal state word is a pointer to the "earliest" expiring
   * client. 
   */
  last_node = NULL;
  node      = (npa_client_handle)resource->internal_state[0];

  /*
   * The resource_data field of each client is a pointer to the 
   * "next earliest" client 
   */
  while(node)
  {
    /*
     * first, discard this node if its wakeup is considerably in the past.
     * this is a failsafe mechanism that ensures a client that sets a wakeup
     * restriction and never removes it gets ignored eventually. 
     */
    if(sleep_wakeup_discard_if_invalid(resource, &node, last_node, now))
      continue;

    /* now we're looking at a valid node, so compare our duration to theirs */
    old_request = NPA_ACTIVE_REQUEST(node).state - now;
    if(old_request >= new_request)
      break;

    last_node = node;
    node = node->resource_data;
  }

  /* either we've found our insertion point or we hit the end of the list */
  if(!last_node)
    resource->internal_state[0] = (npa_resource_state)new_client;
  else
    last_node->resource_data = new_client;

  new_client->resource_data = node;
  NPA_PENDING_REQUEST(new_client).state = new_request + now;
}

/**
 * sleep_wakeup_update_fcn
 *
 * @brief This function is invoked by the /core/cpu/wakeup resource when a 
 *        client request is made.
 *
 * It's job is to compute the expected wakeup time based on input from clients
 * so that sleep can better estimate what modes it should enter. 
 *
 * @param resource: The NPA resource being requested (should be 
 *                  /core/cpu/wakeup).
 * @param client: The handle to the clients registered to the 
 *                resource.
 *
 * @return Returns the new state of the resource.
 */
static npa_resource_state sleep_wakeup_update( npa_resource *resource, 
                                               npa_client_handle client)
{
  uint32 now = CoreTimetick_Get();

  CORE_VERIFY_PTR(resource);
  CORE_VERIFY_PTR(client);

  /* We only support the required client for the moment. */
  if(NPA_CLIENT_REQUIRED == client->type)
  {
    /* Remove the old deadline from this client, if it has one. */
    sleep_wakeup_remove_client(resource, client);

    /* Add the new request from this client, if it has one. */
    if(NPA_PENDING_REQUEST(client).state != 0)
      sleep_wakeup_insert_client(resource, client, now);
  }

  if(((npa_client_handle)resource->internal_state[0]) == client)
    return NPA_PENDING_REQUEST(
                 (npa_client_handle)(resource->internal_state[0]) ).state;
  else if(resource->internal_state[0])
    return NPA_ACTIVE_REQUEST(
                 (npa_client_handle)(resource->internal_state[0]) ).state;
  else
    return 0;
}

/**
 * sleep_wakeup_driver
 *
 * @brief Driver function for the /core/cpu/wakeup resource that updates its
 *        state.
 *
 * @param resource: Pointer to the /core/cpu/wakeup resource
 * @param client: Client of the resource
 * @param state: state of the resource.
 *
 * @return state of the resource.
 */
static npa_resource_state sleep_wakeup_driver (npa_resource *resource,
                                               npa_client *client,
                                               npa_resource_state state)
{
  CORE_VERIFY_PTR(resource);
  CORE_VERIFY_PTR(client);

  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    resource->internal_state[0] = (npa_resource_state)NULL;
    return 0;
  }

  return state;
}

/**
 * sleep_wakeup_query_fcn
 *
 * @brief Query function for the /core/cpu/wakeup resource that returns
 *        soft or hard wakeup time based on the query id
 *
 * @param resource: Resource to which we are making request (wakeup node here).
 * @param query_id: ID for the query being made.
 * @param query_result: Stores to the result of the query
 *
 * @return Returns the status for the query (i.e. successfully handled or 
 *         unsupported).
 */
npa_query_status sleep_wakeup_query_fcn
(
  struct npa_resource *resource, 
  unsigned int query_id, 
  npa_query_type *query_result
)
{
  npa_query_status status = NPA_QUERY_SUCCESS;
  uint32           deadline;

  CORE_VERIFY_PTR(resource);
  CORE_VERIFY_PTR(query_result);

  switch(query_id)
  {
    case SLEEP_QUERY_WAKEUP_TIME:
      deadline = resource->active_state;
      query_result->type = NPA_QUERY_TYPE_VALUE;
      if(deadline)
      {
        /* figure out the number of ticks until the next wakeup */
        query_result->data.value = (deadline - CoreTimetick_Get());

      }
      else
      {
        query_result->data.value = NPA_MAX_STATE;
      }
      break;

    case SLEEP_QUERY_HARD_WAKEUP_TIME:

      /* The hard wakeup time represents the amount of time until the
       * next scheduled wakeup. */
      query_result->type = NPA_QUERY_TYPE_VALUE;
      query_result->data.value = sleep_os_get_hard_wakeup_time();

      break;

    default:
      status = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      break;
  }

  return status;
}

/**
 * @brief sleep_wakeup_plugin
 *
 * The plugin for the /core/cpu/wakeup resource.
 */
const npa_resource_plugin sleep_wakeup_plugin = 
{
  sleep_wakeup_update,                      /* Update function */
  NPA_CLIENT_REQUIRED | NPA_CLIENT_IMPULSE, /* Supported client types */
  NULL,                                     /* Create client function */
  NULL                                      /* Destroy client function */
};

/**
 * @brief sleep_wakeup_resource
 *
 * Expected wakeup pseudo-resource. 
 */
static npa_resource_definition sleep_wakeup_resource[] = 
{ 
  {  
    "/core/cpu/wakeup",                        /* Name */
    "sclk",                                    /* Units */
    NPA_MAX_STATE,                             /* Max State */
    &sleep_wakeup_plugin,                      /* Plugin */
    NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED,  /* Attributes */
    NULL,                                      /* User Data */
    sleep_wakeup_query_fcn                     /* Query Function */
  }
};

/**
 * @brief sleep_wakeup_node
 *
 * Definition of the expected wakeup node. 
 */
static npa_node_definition sleep_wakeup_node = 
{ 
  "/node/core/cpu/wakeup", /* name */
  sleep_wakeup_driver,     /* driver_fcn */
  NPA_NODE_DEFAULT,        /* attributes */
  NULL,                    /* data */
  NPA_EMPTY_ARRAY,
  NPA_ARRAY(sleep_wakeup_resource)
};
#endif        /* !FEATURE_SLEEP_NO_WAKEUP_NODE */

#ifndef FEATURE_SLEEP_NO_LATENCY_NODE
/*
===============================================================================

                         SLEEP LATENCY NODE DEFINITION

===============================================================================
*/

/**
 * @brief sleep_latency_node
 *
 * Npa node related to the latency resources. Its fields will be assigned
 * values in sleep_define_latency node based on resources we create there.
 */
static npa_node_definition sleep_latency_node = { 0 };

/**
 * sleep_latency_driver
 *
 * @brief Driver function for the /core/cpu/latency resource that updates it.
 *
 * @param resource: Pointer to the resource (i.e. /core/cpu/latency) (unused).
 * @param client: Client of this resource (unused).
 * @param state: New(?) state for the resource (unused).
 *
 * @return The state of the resource if set else NPA_MAX_STATE
*/
static npa_resource_state sleep_latency_driver ( npa_resource *resource,
                                                 npa_client *client,
                                                 npa_resource_state state )
{
  /* 
   * Record the state so it can be queried from the sleep task before we go
   * to sleep. 
   */
  if( !state )
  {  
    state = NPA_MAX_STATE;
  }

  /* Get the modem_core_rail node's client handle to the sleep LPR node. */
  npa_client_handle client_handle = NPA_DEPENDENCY( resource, 0 );

  /* Tell the sleep idle plugin node about the new latency restriction, so
   * it can decide whether to do SWFI-only or not. */
  npa_issue_required_request( client_handle, state );
  
  return state;
}


/**
 * sleep_usec_latency_driver
 *
 * @brief Driver function for the /core/cpu/latency resource that updates it.
 *
 * @param resource: Pointer to the resource (i.e. /core/cpu/latency) (unused).
 * @param client: Client of this resource (unused).
 * @param state: New(?) state for the resource (unused).
 *
 * @return The state of the resource if set else NPA_MAX_STATE
*/
static npa_resource_state sleep_usec_latency_driver ( npa_resource *resource,
                                                      npa_client *client,
                                                      npa_resource_state state )
{
  /* Convert the request to ticks and forward it to the corresponding
   * base latency resource */
  npa_issue_required_request( NPA_DEPENDENCY( resource, resource->index ), 
                              US_TO_TICKS( state ) );
  return state;
}

npa_query_status sleep_latency_query_fcn( npa_resource *resource,
                                          unsigned int query_id, 
                                          npa_query_type *query_result )
{
  npa_query_status status = NPA_QUERY_SUCCESS;
  
  CORE_VERIFY( resource );
  CORE_VERIFY( query_result );
  
  switch( query_id )
  {
    case NPA_QUERY_CURRENT_STATE: /* For backward compatibility. */
    case SLEEP_QUERY_LOCAL_MIN_LATENCY:
      query_result->type = NPA_QUERY_TYPE_STATE;
      query_result->data.state = resource->active_state;
      break;
    
    case SLEEP_QUERY_MIN_LATENCY:
      {
        /* Getting index of global latency resource based on number of cores */
        int index = SLEEP_NUM_CORES > 1 ? SLEEP_NUM_CORES : 0;
        npa_resource *global_resource = NPA_RESOURCE(resource, index);
        query_result->type = NPA_QUERY_TYPE_STATE;
        query_result->data.state = 
          MIN( resource->active_state, global_resource->active_state );
        break;
      }
      
    default:
      status = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      break;
  } 
  
  return status;
}

/**
 *
 * @brief sleep_latency_dependency
 *
 * The dependency array for the latency node.  This node depends
 * on the sleep idle plugin node, as it makes requests to that 
 * node when the state of this node changes. 
 *
*/
static npa_node_dependency sleep_latency_dependency[1] =
{
  {
    "/sleep/idle/plugin",
    SLEEP_IDLE_PLUGIN_CLIENT_LATENCY,
  },
};

/**
 * sleep_define_base_latency_node
 *
 * @brief Constructs npa resources for latency node based on number of cores
 *        the processor has.
 *
 * This defines the base latency node.  This takes latency requests in ticks
 */
static void sleep_define_base_latency_node(void)
{
  /* Pointers to local and global resources */
  npa_resource_definition *latency_resources = NULL, 
                          *global_latency_resource = NULL;
  unsigned int num_latency_dep = 1;
  npa_node_dependency *latency_dep_ptr = sleep_latency_dependency;

  /* Initial resource state */
  npa_resource_state 
    initial_state[ (SLEEP_NUM_CORES == 1) ? 1 : (SLEEP_NUM_CORES + 1)];
  
  int i = 0;
  unsigned int num_latency_resources = 0;
  
  memset( initial_state, NPA_MAX_STATE, 
          sizeof(npa_resource_state) * CORE_ARRAY_SIZE(initial_state) );
          
  if( SLEEP_NUM_CORES == 1 )
  {
    /* Single core environment */  
    latency_resources = 
      (npa_resource_definition *)malloc(sizeof(npa_resource_definition));
      
    CORE_VERIFY_PTR( latency_resources );
	memset( latency_resources, 0, sizeof(npa_resource_definition));
    global_latency_resource = latency_resources;
    num_latency_resources = 1;
  }
  else
  {
    /* Multicore environment */
    
    /* Pointers to latency resources' names */
    char *resource_names[SLEEP_NUM_CORES];
    
    /* Total memory required for all latency resources */
    unsigned int req_mem_size = sizeof(npa_resource_definition) *
                                (SLEEP_NUM_CORES + 1);
   
    latency_resources = (npa_resource_definition *)malloc(req_mem_size);
    CORE_VERIFY_PTR( latency_resources );
	memset( latency_resources, 0, req_mem_size);
    
    for(i = 0; i < SLEEP_NUM_CORES; i++)
    {
      /* Constructing local resource name strings */
      resource_names[i] = (char *)malloc(SLEEP_LATENCY_NAME_LENGTH);
      CORE_VERIFY_PTR( resource_names[i] );
      core_snprintf((char *)resource_names[i],SLEEP_LATENCY_NAME_LENGTH, 
                     "/core/cpu/%d/latency", i);
    
      /* Constructing local latency resources */
      latency_resources[i].name = resource_names[i];
      latency_resources[i].units = "ticks";
      latency_resources[i].max = NPA_MAX_STATE;
      latency_resources[i].plugin = &npa_min_plugin;
      latency_resources[i].attributes =
                           NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED;
      latency_resources[i].data = NULL;
      latency_resources[i].query_fcn = sleep_latency_query_fcn;
    }
    
    /* Getting pointer to the last resource in the array which will be
     * treated as global resource */
    global_latency_resource = &latency_resources[SLEEP_NUM_CORES];
    num_latency_resources = SLEEP_NUM_CORES + 1;
  }
    
  /* Constructing global latency resource */
  global_latency_resource->name = "/core/cpu/latency";
  global_latency_resource->units = "ticks";
  global_latency_resource->max = NPA_MAX_STATE;
  global_latency_resource->plugin = &npa_min_plugin;
  global_latency_resource->attributes =
                           NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED;
  global_latency_resource->data = NULL;
  global_latency_resource->query_fcn = sleep_latency_query_fcn;
  
  /* Here, we have all the resources created based on number of cores. Now,
   * defining npa node for those resources. */
 
  sleep_latency_node.name = "/node/core/cpu/latency";
  sleep_latency_node.driver_fcn = sleep_latency_driver;
  sleep_latency_node.attributes = NPA_NODE_DEFAULT;
  sleep_latency_node.data = NULL;
  sleep_latency_node.dependency_count = num_latency_dep;
  sleep_latency_node.dependencies = latency_dep_ptr;
  sleep_latency_node.resource_count = num_latency_resources;
  sleep_latency_node.resources = latency_resources;      
  
  /* Registering the latency node with npa framework */
  npa_define_node(&sleep_latency_node, initial_state, NULL);
}

/**
 * sleep_define_usec_latency_node
 *
 * @brief Constructs npa resources for latency node based on number of cores
 *        the processor has.
 *
 * This defines the usec latency node.  This takes latency requests in
 * usec, and then converts the minimum to ticks and sends it to the
 * base latency resource
 */
static void sleep_define_usec_latency_node(void)
{
  /* Pointers to local and global resources */
  npa_resource_definition *latency_resources = NULL, 
                          *global_latency_resource = NULL;
  npa_node_dependency     *latency_dependencies = NULL,
                          *global_latency_dependency = NULL;

  int i = 0;
  unsigned int num_latency_resources = 0, num_latency_dependencies = 0;
  npa_node_definition *sleep_latency_usec_node;

  sleep_latency_usec_node =
                (npa_node_definition *)malloc( sizeof(npa_node_definition));
  CORE_VERIFY_PTR( sleep_latency_usec_node );
  memset( sleep_latency_usec_node, 0, sizeof( npa_node_definition ) );

  if( SLEEP_NUM_CORES == 1 )
  {
    /* Single core environment */  

    /* Resource */
    latency_resources = 
      (npa_resource_definition *)malloc(sizeof(npa_resource_definition));
    CORE_VERIFY_PTR( latency_resources );
    memset( latency_resources, 0, sizeof(npa_resource_definition) );

    global_latency_resource = latency_resources;
    num_latency_resources = 1;

    /* Dependency */
    latency_dependencies =
            (npa_node_dependency *)malloc(sizeof(npa_node_dependency));
    CORE_VERIFY_PTR( latency_dependencies );
    memset( latency_dependencies, 0, sizeof(npa_node_dependency) );

    global_latency_dependency = latency_dependencies;
    num_latency_dependencies = 1;

  }
  else
  {
    /* Multicore environment */
    
    /* Pointers to latency resources' names */
    char *resource_names[SLEEP_NUM_CORES];
    
    /* Total memory required for all latency resources */
    unsigned int resource_mem_size = sizeof(npa_resource_definition) *
            (SLEEP_NUM_CORES + 1);

    /* Total memory required for all latency dependencies */
    unsigned int dependency_mem_size = sizeof(npa_node_dependency) *
            (SLEEP_NUM_CORES + 1);

   
    latency_resources = (npa_resource_definition *)malloc(resource_mem_size);
    CORE_VERIFY_PTR( latency_resources );
    memset( latency_resources, 0, resource_mem_size );

    latency_dependencies = (npa_node_dependency *)malloc(dependency_mem_size);
    CORE_VERIFY_PTR( latency_dependencies );
    memset( latency_dependencies, 0, sizeof(dependency_mem_size) );

    for(i = 0; i < SLEEP_NUM_CORES; i++)
    {
      /* Constructing local resource name strings */
      resource_names[i] = (char *)malloc(SLEEP_LATENCY_NAME_LENGTH);
      CORE_VERIFY_PTR( resource_names[i] );
      core_snprintf((char *)resource_names[i],SLEEP_LATENCY_NAME_LENGTH, 
                     "/core/cpu/%d/latency/usec", i);
    
      /* Constructing local latency resources */
      latency_resources[i].name = resource_names[i];
      latency_resources[i].units = "usec";
      latency_resources[i].max = NPA_MAX_STATE;
      latency_resources[i].plugin = &npa_min_plugin;
      latency_resources[i].attributes =
                           NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED;
      latency_resources[i].data = NULL;
      latency_resources[i].query_fcn = sleep_latency_query_fcn;

      /* Construct latency depedencies */
      /* Note: To save memory, we could reuse the name string that is
       * defined in the base node, but that introduces a coupling
       * between these two functions - this usec node would have to be
       * defiend after the base node.  I'm not going to do that right
       * now.
       *
       * If desired, it would be something like
       * latency_dependencies[i].name = sleep_latency_node->resources[i].name;
       * */
      latency_dependencies[i].name = (char *)malloc(SLEEP_LATENCY_NAME_LENGTH);
      CORE_VERIFY_PTR( latency_dependencies[i].name );
      core_snprintf((char *)latency_dependencies[i].name,
                     SLEEP_LATENCY_NAME_LENGTH,
                     "/core/cpu/%d/latency", 
                     i);
      latency_dependencies[i].client_type = NPA_CLIENT_REQUIRED;
    }
    
    /* Getting pointer to the last resource in the array which will be
     * treated as global resource */
    global_latency_resource = &latency_resources[SLEEP_NUM_CORES];
    num_latency_resources = SLEEP_NUM_CORES + 1;

    global_latency_dependency = &latency_dependencies[SLEEP_NUM_CORES];
    num_latency_dependencies  = SLEEP_NUM_CORES + 1;
  }
    
  /* Constructing global latency resource */
  global_latency_resource->name = "/core/cpu/latency/usec";
  global_latency_resource->units = "usec";
  global_latency_resource->max = NPA_MAX_STATE;
  global_latency_resource->plugin = &npa_min_plugin;
  global_latency_resource->attributes =
                           NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED;
  global_latency_resource->data = NULL;
  global_latency_resource->query_fcn = NULL;

  /* Constructing global latency depednency */
  global_latency_dependency->name        = "/core/cpu/latency";
  global_latency_dependency->client_type = NPA_CLIENT_REQUIRED;

  /* Here, we have all the resources created based on number of cores. Now,
   * defining npa node for those resources. */

  sleep_latency_usec_node->name = "/node/core/cpu/latency/usec";
  sleep_latency_usec_node->driver_fcn = sleep_usec_latency_driver;
  sleep_latency_usec_node->attributes = NPA_NODE_DEFAULT;
  sleep_latency_usec_node->data = NULL;
  sleep_latency_usec_node->dependency_count = num_latency_dependencies;
  sleep_latency_usec_node->dependencies = latency_dependencies;
  sleep_latency_usec_node->resource_count = num_latency_resources;
  sleep_latency_usec_node->resources = latency_resources;
  
  /* Registering the latency node with npa framework */
  npa_define_node(sleep_latency_usec_node, NULL, NULL);
}

/**
 * sleep_define_latency_node
 *
 * @brief Constructs npa resources for latency node based on number of cores
 *        the processor has.
 *
 * If processor has just one core, it will create just a global latency 
 * resource. However, if there are more than one core on a processor, it
 * will create a latency resource for each core and a global resource.
 */
static void sleep_define_latency_node(void)
{
  sleep_define_base_latency_node();
  sleep_define_usec_latency_node();
}

#endif        /* !FEATURE_SLEEP_NO_LATENCY_NODE */


/*
 * sleep_npa_init
 */
void sleep_npa_init(void)
{
  #if !defined( FEATURE_SLEEP_NO_WAKEUP_NODE ) || \
    !defined( FEATURE_SLEEP_NO_LATENCY_NODE ) || \
    !defined( FEATURE_SLEEP_NO_MAX_DURATION_NODE )
  npa_resource_state initial_state[] = {NPA_MAX_STATE};
#endif /* !FEATURE_SLEEP_NO_WAKEUP_NODE  */
       /* || !FEATURE_SLEEP_NO_LATENCY_NODE */
       /* || !FEATURE_SLEEP_NO_MAX_DURATION_NODE */

  #ifndef FEATURE_SLEEP_NO_WAKEUP_NODE
  npa_define_node(&sleep_wakeup_node, initial_state, NULL);
  #endif /* !FEATURE_SLEEP_NO_WAKEUP_NODE */

#ifndef FEATURE_SLEEP_NO_MAX_DURATION_NODE
  npa_define_node(&g_sleepMaxDurationNode, initial_state, NULL);
  npa_alias_resource_cb(MAX_DURATION_TICKS, MAX_DURATION_USEC, NULL, NULL);
#endif /* !FEATURE_SLEEP_NO_MAX_DURATION_NODE */

  #ifndef FEATURE_SLEEP_NO_LATENCY_NODE
  sleep_define_latency_node();
  #endif /* !FEATURE_SLEEP_NO_LATENCY_NODE */
}
