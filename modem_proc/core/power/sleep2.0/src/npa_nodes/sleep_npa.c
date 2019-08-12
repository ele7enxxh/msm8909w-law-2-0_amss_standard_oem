/*==============================================================================
  FILE:         sleep_npa.c
  
  OVERVIEW:     This file provides the NPA node definitions for latency and
                wakeup nodes.

  DEPENDENCIES: None

                Copyright (c) 2010-2015 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/npa_nodes/sleep_npa.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#ifndef FEATURE_SLEEP_NO_WAKEUP_NODE
#include "CoreTime.h"
#include "sleep_log.h"
#include "sleep_utils.h"
#endif /* !FEATURE_SLEEP_NO_WAKEUP_NODE */

#if !defined( FEATURE_SLEEP_NO_WAKEUP_NODE ) || \
    !defined( FEATURE_SLEEP_NO_LATENCY_NODE )  || \
    !defined( FEATURE_SLEEP_NO_MAX_DURATION_NODE )
#include "CoreVerify.h"
#include "CoreString.h"
#include "sleep_lpr.h"
#include "npa_resource.h"
#include "npa_log.h"
#include "sleepi.h"
#include "sleep_os.h"
#include <stdio.h>
#include <stdlib.h>
#endif /* !FEATURE_SLEEP_NO_WAKEUP_NODE */
       /* || !FEATURE_SLEEP_NO_LATENCY_NODE */
       /* || !FEATURE_SLEEP_NO_MAX_DURATION_NODE */

#if !defined( FEATURE_SLEEP_NO_LATENCY_NODE )  || \
    !defined( FEATURE_SLEEP_NO_MAX_DURATION_NODE )
#include "sleep_idle_plugin.h"
#include "sleepActive.h"
#endif /* !FEATURE_SLEEP_NO_LATENCY_NODE */
       /* || !FEATURE_SLEEP_NO_MAX_DURATION_NODE */

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
  if(0 == state)
  {  
    state = NPA_MAX_STATE;
  }

  /* There is a race condition where ATS may finish the query before NPA 
   * framework can process the return vlaue from driver causing ATS to use 
   * stale values. Until NPA has support for synchronous POST_CHANGE 
   * callbacks, update the final calculated state in separate field 
   * as recommended by NPA team - use this field for query purpose */
  resource->definition->data = (npa_user_data)state;

  /* Trigger Active Timer Solver -- max_duration is updated */
  sleepActive_SetSignal( SLEEP_SIGNAL_SOFT_DURATION );
  
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
      retVal = (npa_resource_state)resource_link->resource->definition->data;
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
/*==============================================================================
                         SLEEP WAKEUP NODE DEFINITION
 =============================================================================*/
/**
 * @brief g_wakeupResourceMin64
 *
 * Minimum absolute of the set of clients on wakeup node resource
 */
static uint64 g_wakeupResourceMin64;

/**
 * @brief g_wakeupResourceMinClientHandle
 *
 * Client handle whose request is the currently set minimum
 *
 */
static npa_client *g_wakeupResourceMinClientHandle;

/**
 * @brief wakeupUserData
 *
 * npa_user_data is defined as void* and wakeup node defines this 
 * struct to send as resource_data
 *
 */
typedef struct wakeupResourceData_s
{
  uint64 data;
}wakeupUserData;

/**
 * @brief sleepNPA_wakeupResource_aggregate
 *
 * Aggregates minimum of all client requests
 *
 * @param resource: The NPA resource being requested (should be 
 *                  /core/cpu/wakeup).
 * @param client: The handle to the client registered to the 
 *                resource.
 *
 * @return Returns the new minimum of the resource 
 */
static void sleepNPA_wakeupResource_aggregate( npa_resource *resource, 
                                               npa_client *client )
{
  npa_client_handle activeClient = resource->clients;
  wakeupUserData *dataPtr;

  /* First reset minimum and minimum client handle */
  g_wakeupResourceMin64           = UINT64_MAX;
  g_wakeupResourceMinClientHandle = NULL;

  /* Traverse through the list of clients to find the minimum */
  while (activeClient != NULL)
  {
    dataPtr = (wakeupUserData *)activeClient->resource_data;

    /* Get the minimum value from clients */
    if (dataPtr->data < g_wakeupResourceMin64)
    {
      g_wakeupResourceMin64           = dataPtr->data;
      g_wakeupResourceMinClientHandle = activeClient;
    }
    activeClient = activeClient->next;
  }

  if (g_wakeupResourceMin64 <= CoreTimetick_Get64())
  {
    /* Node clients are responsible to cancel requests placed on node
     * when use-case completes and cannot have requests that are in past */
    sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 1, 
                    "Client (handle: 0x%08x) - request on "
                    "wakeup node is in the past",
                    g_wakeupResourceMinClientHandle);
  }

  return;
}

/**
 * wakeupResourceData_createClientFcn
 *
 * @brief In order to store values into the resource-data field, that will
 *        need to be assigned necessary memory
 *
 * Allocating memory to store address to 64-bit absolute time
 *
 * @param client: The handle to the client registered to the 
 *                resource.
 *
 * @return None
 */
void wakeupResourceData_createClientFcn( npa_client *client )
{
  wakeupUserData *clientReq;

  /* Allocate storage space because NPA discards the data otherwise */
  client->resource_data = (wakeupUserData *)
                                malloc(sizeof(wakeupUserData));
  
  CORE_VERIFY_PTR(client->resource_data);

  /* get the address where 64-bit address is/should be placed & set to max */
  clientReq = (wakeupUserData *)client->resource_data;
  clientReq->data = UINT64_MAX;
}

/**
 * wakeupResourceData_destroyClientFcn
 *
 * @brief Free memory allocated to resource-data if client requests
 *        itself to be destroyed via npa_destroy_client
 *
 * Calling into npa_destroy_client would make NPA FW to call into the
 * plugin's destroy client fcn
 *
 * @param client: The handle to the client registered to the 
 *                resource.
 *
 * @return None
 */
void wakeupResourceData_destroyClientFcn( npa_client *client )
{
  /* Free the memory allocated for the client's data */
  free(client->resource_data);
}

/**
 * sleepNPA_wakeupUpdate
 *
 * @brief This function is invoked by the /core/cpu/wakeup resource when a 
 *        client request is made.
 *
 * Its job is to compute the expected wakeup time based on input from clients
 * so that sleep can better estimate what modes it should enter. 
 *
 * @param resource: The NPA resource being requested (should be 
 *                  /core/cpu/wakeup).
 * @param client: The handle to the clients registered to the 
 *                resource.
 *
 * @return Returns the new state of the resource ('zero' until 64-bit NPA
 *         support becomes available).
 */
static npa_resource_state sleepNPA_wakeupUpdate( npa_resource       *resource, 
                                                 npa_client_handle  client)
{
  wakeupUserData      *clientReq;
  npa_resource_state  newReq;
  uint64              now = CoreTimetick_Get64();

  CORE_VERIFY_PTR(resource);
  CORE_VERIFY_PTR(client);

  /* Get the relative requested duration */
  newReq = NPA_PENDING_REQUEST(client).state;

  /* get the address where 64-bit address is/should be placed */
  clientReq = (wakeupUserData *)client->resource_data;

  /* Check if this is a cancel request or destroy client request
   * - in both of those cases, the requested value is sent as zero */
  if (newReq == 0)
  {
    /* Set MAX for the current client's data */
    clientReq->data = UINT64_MAX;
  }
  else
  {
    /* add current time and store into data field.
     * Since newReq is a offset from the client, use the time from the beginning
     * of this call rather than the current time at this point */
    clientReq->data = (uint64)(newReq + now);
  }

  if (clientReq->data < g_wakeupResourceMin64)
  {
    /* Find minimum duration at which wakeup needs to occur. If
     * first client or client's request is less than current minimum,
     * set client as new minimum */
    g_wakeupResourceMin64           = clientReq->data;
    g_wakeupResourceMinClientHandle = client;
  }
  else if (g_wakeupResourceMinClientHandle == client)
  {
    /* Re-aggregate the new minimum */
    sleepNPA_wakeupResource_aggregate(resource, client);
  }

  return 0;
}

/**
 * sleepNPA_wakeupDriver
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
static npa_resource_state sleepNPA_wakeupDriver (npa_resource       *resource,
                                                 npa_client         *client,
                                                 npa_resource_state state)
{
  CORE_VERIFY_PTR(resource);
  CORE_VERIFY_PTR(client);

  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    resource->internal_state[0]     = (npa_resource_state)NULL;
    g_wakeupResourceMin64           = UINT64_MAX;
    g_wakeupResourceMinClientHandle = NULL;
    return 0;
  }

  return state;
}

/**
 * sleepNPA_wakeupQuery
 *
 * @brief Query function for the /core/cpu/wakeup resource that returns
 *        soft wakeup time based on the query id
 *
 * @param resource: Resource to which we are making request (wakeup node here).
 * @param query_id: ID for the query being made.
 * @param query_result: Stores to the result of the query
 *
 * @return Returns the status for the query (i.e. successfully handled or 
 *         unsupported).
 */
npa_query_status sleepNPA_wakeupQuery
(
  struct npa_resource *resource, 
  unsigned int query_id, 
  npa_query_type *query_result
)
{
  npa_query_status status = NPA_QUERY_SUCCESS;

  CORE_VERIFY_PTR(resource);
  CORE_VERIFY_PTR(query_result);

  switch(query_id)
  {
    case SLEEP_QUERY_WAKEUP_TIME:
    {
      uint64 now = CoreTimetick_Get64();

      /* return 32-bit value as soft-duration */
      query_result->type = NPA_QUERY_TYPE_VALUE;

      if(g_wakeupResourceMin64 == UINT64_MAX)
      {
        /* return query result as UINT32_MAX */
        query_result->data.value = UINT32_MAX;
      }
      else if (g_wakeupResourceMin64 <= now)
      {
        /* Node clients are responsible to cancel requests placed on node
         * when use-case completes, and cannot have requests in past. */
        sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 1, 
                        "Client (handle: 0x%08x) "
                        "- request on wakeup node is in the past",
                        g_wakeupResourceMinClientHandle);

        /* return zero as soft-duration */
        query_result->data.value = 0;
      }
      else
      {
        query_result->data.value = safe_unsigned_truncate(g_wakeupResourceMin64 - now); 
      }
      break;
    }

    case SLEEP_QUERY_ABS_WAKEUP_TIME:
    {
      /* Return 64-bit value as soft-duration */
      query_result->type = NPA_QUERY_TYPE_VALUE64;

      /* Set return value to minimum wakeup time */
      query_result->data.value64 = g_wakeupResourceMin64;

      /* Verify that the minimum is in the future */
      if (g_wakeupResourceMin64 <= CoreTimetick_Get64())
      {
        /* Node clients are responsible to cancel requests placed on node
         * when use-case completes, and cannot have requests in past. */
        sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 1, 
                        "Client (handle: 0x%08x) " 
                        "- request on ABS wakeup node is in the past",
                         g_wakeupResourceMinClientHandle);
      }
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
 * @brief g_sleepWakeupPlugin
 *
 * The plugin for the /core/cpu/wakeup resource.
 */
const npa_resource_plugin g_sleepWakeupPlugin = 
{
  sleepNPA_wakeupUpdate,                  /* Update function */
  NPA_CLIENT_REQUIRED,                    /* Supported client types */
  wakeupResourceData_createClientFcn,     /* Create client function */
  wakeupResourceData_destroyClientFcn     /* Destroy client function */
};

/**
 * @brief g_sleepWakeupResource
 *
 * Expected wakeup pseudo-resource. 
 */
static npa_resource_definition g_sleepWakeupResource[] = 
{ 
  {  
    "/core/cpu/wakeup",                        /* Name */
    "ticks",                                   /* Units */
    NPA_MAX_STATE,                             /* Max State */
    &g_sleepWakeupPlugin,                      /* Plugin */
    NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED,  /* Attributes */
    NULL,                                      /* User Data */
    sleepNPA_wakeupQuery                       /* Query Function */
  }
};

/**
 * @brief g_sleepWakeupNode
 *
 * Definition of the expected wakeup node. 
 */
static npa_node_definition g_sleepWakeupNode = 
{ 
  "/node/core/cpu/wakeup", /* name */
  sleepNPA_wakeupDriver,   /* driver_fcn */
  NPA_NODE_DEFAULT,        /* attributes */
  NULL,                    /* data */
  NPA_EMPTY_ARRAY,
  NPA_ARRAY(g_sleepWakeupResource)
};
#endif        /* !FEATURE_SLEEP_NO_WAKEUP_NODE */

#ifndef FEATURE_SLEEP_NO_LATENCY_NODE
/*==============================================================================
                         SLEEP LATENCY NODE DEFINITION
 =============================================================================*/
/**
 * @brief g_sleepLatencyNode
 *
 * Npa node related to the latency resources. Its fields will be assigned
 * values in sleep_define_latency node based on resources we create there.
 */
static npa_node_definition g_sleepLatencyNode = { 0 };

/**
 * sleepNPA_latencyDriver
 *
 * @brief Driver function for the /core/cpu/latency resource that updates it.
 *
 * @param resource: Pointer to the resource (i.e. /core/cpu/latency) (unused).
 * @param client: Client of this resource (unused).
 * @param state: Resource state as aggregated by the Update function.
 *
 * @return The state of the resource if set else NPA_MAX_STATE
*/
static npa_resource_state sleepNPA_latencyDriver ( npa_resource       *resource,
                                                   npa_client         *client,
                                                   npa_resource_state state )
{
  /* 
   * Record the state so it can be queried from the sleep task before we go
   * to sleep. 
   */
  if(0 == state)
  {  
    state = NPA_MAX_STATE;
  }

  /* There is a race condition where ATS may finish the query before NPA 
   * framework can process the return vlaue from driver causing ATS to use 
   * stale values. Until NPA has support for synchronous POST_CHANGE 
   * callbacks, update the final calculated state in separate field 
   * as recommended by NPA team - use this field for query purpose */
  resource->definition->data = (npa_user_data)state;

  /* Trigger Active Timer Solver -- latency is updated */
  sleepActive_SetSignal( SLEEP_SIGNAL_LATENCY );
  
  return state;
}

/**
 * sleepNPA_latencyDriverusec
 *
 * @brief Driver function for the /core/cpu/latency resource that updates it.
 *
 * @param resource: Pointer to the resource (i.e. /core/cpu/latency) (unused).
 * @param client: Client of this resource (unused).
 * @param state: New(?) state for the resource (unused).
 *
 * @return The state of the resource if set else NPA_MAX_STATE
*/
static npa_resource_state sleepNPA_latencyDriverusec (npa_resource        *resource,
                                                      npa_client          *client,
                                                      npa_resource_state  state)
{
  /* See the comment in latency driver function */
  resource->definition->data = (npa_user_data)state;

  /* Convert the request to ticks and forward it to the corresponding
   * base latency resource */
  npa_issue_required_request( NPA_DEPENDENCY( resource, resource->index ), 
                              US_TO_TICKS( state ) );

  return state;
}

/*
 * sleepNPA_latencyQuery
 */
npa_query_status sleepNPA_latencyQuery( npa_resource    *resource,
                                        unsigned int    query_id, 
                                        npa_query_type  *query_result )
{
  npa_query_status status = NPA_QUERY_SUCCESS;
  
  CORE_VERIFY( resource );
  CORE_VERIFY( query_result );
  
  switch( query_id )
  {
    case NPA_QUERY_CURRENT_STATE: /* For backward compatibility. */
    case SLEEP_QUERY_LOCAL_MIN_LATENCY:
      query_result->type = NPA_QUERY_TYPE_STATE;
      query_result->data.state = 
        (npa_resource_state)(resource->definition->data);
      break;
    
    case SLEEP_QUERY_MIN_LATENCY:
      {
        /* Getting index of global latency resource based on number of cores */
        int index = SLEEP_NUM_CORES > 1 ? SLEEP_NUM_CORES : 0;
        npa_resource *global_resource = NPA_RESOURCE(resource, index);
        query_result->type = NPA_QUERY_TYPE_STATE;
        query_result->data.state = 
          MIN( (npa_resource_state)(resource->definition->data), 
               (npa_resource_state)(global_resource->definition->data) );
        break;
      }
      
    default:
      status = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      break;
  } 
  
  return status;
}

/**
 * sleepNPA_defineLatencyNode
 *
 * @brief Constructs npa resources for latency node based on number of cores
 *        the processor has.
 *
 * This defines the base latency node.  This takes latency requests in ticks
 */
static void sleepNPA_defineLatencyNode(void)
{
  /* Pointers to local and global resources */
  npa_resource_definition *latency_resources = NULL, 
                          *global_latency_resource = NULL;

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
    memset(latency_resources, 0, sizeof(npa_resource_definition));

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
    memset(latency_resources, 0, req_mem_size);

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
      latency_resources[i].query_fcn = sleepNPA_latencyQuery;
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
  global_latency_resource->query_fcn = sleepNPA_latencyQuery;
  
  /* Here, we have all the resources created based on number of cores. Now,
   * defining npa node for those resources. */
 
  g_sleepLatencyNode.name = "/node/core/cpu/latency";
  g_sleepLatencyNode.driver_fcn = sleepNPA_latencyDriver;
  g_sleepLatencyNode.attributes = NPA_NODE_DEFAULT;
  g_sleepLatencyNode.data = NULL;
  g_sleepLatencyNode.dependency_count = 0;
  g_sleepLatencyNode.dependencies = NULL;
  g_sleepLatencyNode.resource_count = num_latency_resources;
  g_sleepLatencyNode.resources = latency_resources;      
  
  /* Registering the latency node with npa framework */
  npa_define_node(&g_sleepLatencyNode, initial_state, NULL);

  /* Add "latency" to the list of resources logged in "Sleep Requests" */
  for(i = 0; i < num_latency_resources; i++)
  {
    CORE_VERIFY( NPA_SUCCESS == ( npa_add_resource_log_by_handle(
                                    "Sleep Requests",
                                    latency_resources[i].handle ) ) );
  }

}

/**
 * sleepNPA_defineLatencyNodeusec
 *
 * @brief Constructs npa resources for latency node based on number of cores
 *        the processor has.
 *
 * This defines the usec latency node.  This takes latency requests in
 * usec, and then converts the minimum to ticks and sends it to the
 * base latency resource
 */
static void sleepNPA_defineLatencyNodeusec(void)
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
      latency_resources[i].query_fcn = sleepNPA_latencyQuery;

      /* Construct latency depedencies */
      /* Note: To save memory, we could reuse the name string that is
       * defined in the base node, but that introduces a coupling
       * between these two functions - this usec node would have to be
       * defiend after the base node.  I'm not going to do that right
       * now.
       *
       * If desired, it would be something like
       * latency_dependencies[i].name = g_sleepLatencyNode->resources[i].name;
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
  global_latency_resource->query_fcn = sleepNPA_latencyQuery;

  /* Constructing global latency depednency */
  global_latency_dependency->name        = "/core/cpu/latency";
  global_latency_dependency->client_type = NPA_CLIENT_REQUIRED;

  /* Here, we have all the resources created based on number of cores. Now,
   * defining npa node for those resources. */

  sleep_latency_usec_node->name = "/node/core/cpu/latency/usec";
  sleep_latency_usec_node->driver_fcn = sleepNPA_latencyDriverusec;
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
 * sleepNPA_defineAllLatencyNodes
 *
 * @brief Constructs npa resources for latency node based on number of cores
 *        the processor has.
 *
 * If processor has just one core, it will create just a global latency 
 * resource. However, if there are more than one core on a processor, it
 * will create a latency resource for each core and a global resource.
 */
static void sleepNPA_defineAllLatencyNodes(void)
{
  sleepNPA_defineLatencyNode();
  sleepNPA_defineLatencyNodeusec();
}

#endif        /* !FEATURE_SLEEP_NO_LATENCY_NODE */


/*
 * sleepNPA_initialize
 */
void sleepNPA_initialize(void)
{
#if !defined( FEATURE_SLEEP_NO_WAKEUP_NODE ) || \
    !defined( FEATURE_SLEEP_NO_LATENCY_NODE ) || \
    !defined( FEATURE_SLEEP_NO_MAX_DURATION_NODE )
  npa_resource_state initial_state[] = {NPA_MAX_STATE};
#endif /* !FEATURE_SLEEP_NO_WAKEUP_NODE  */
       /* || !FEATURE_SLEEP_NO_LATENCY_NODE */
       /* || !FEATURE_SLEEP_NO_MAX_DURATION_NODE */

#ifndef FEATURE_SLEEP_NO_WAKEUP_NODE
  npa_define_node(&g_sleepWakeupNode, initial_state, NULL);
#endif /* !FEATURE_SLEEP_NO_WAKEUP_NODE */

#ifndef FEATURE_SLEEP_NO_MAX_DURATION_NODE
  npa_define_node(&g_sleepMaxDurationNode, initial_state, NULL);
  npa_alias_resource_cb(MAX_DURATION_TICKS, MAX_DURATION_USEC, NULL, NULL);

  /* Log requests to "max_duration" resource in "Sleep Requests" */
  CORE_VERIFY( NPA_SUCCESS == ( npa_add_resource_log_by_handle(
                                "Sleep Requests", 
                                g_sleepMaxDurationResource[0].handle ) ) );

#endif /* !FEATURE_SLEEP_NO_MAX_DURATION_NODE */

#ifndef FEATURE_SLEEP_NO_LATENCY_NODE
  sleepNPA_defineAllLatencyNodes();
#endif /* !FEATURE_SLEEP_NO_LATENCY_NODE */

  return;
}

