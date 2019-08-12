/*==============================================================================
  FILE:         sleep_idle_plugin.c
  
  OVERVIEW:     This file provides the node definition and supporting functions
                for the idle entry point plugin.

  DEPENDENCIES: None

                Copyright (c) 2011-2015 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/npa_nodes/sleep_idle_plugin.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "sleep_lpr.h"
#include "npa_resource.h"
#include "npa_log.h"
#include "CoreVerify.h"
#include "sleep_idle_plugin.h"
#if ( SLEEP_NUM_CORES == 1 ) && !defined ( FEATURE_SLEEP_NO_IDLE_PLUGIN )
#include <stdint.h>
#include "sleep_osi.h"
#include "sleep_statsi.h"
#include "sleepi.h"
#include "sleep_log.h"
#include "sleepActive.h"

/*==============================================================================
                               MACRO DEFINITIONS
 =============================================================================*/
/**
 * SLEEP_HOLDOFF_CLIENTS_NUM
 *
 * @brief Indicates how many clients may want to prevent Sleep until
 *        everything is intialized. Usually its value should be equal to
 *        number of different initialization schemes present in the system.
 *        It default to one but can be overriden in build files.
 */
#ifndef SLEEP_HOLDOFF_CLIENTS_NUM
#define SLEEP_HOLDOFF_CLIENTS_NUM 1
#endif

/*==============================================================================
                       SLEEP IDLE PLUGIN NODE DEFINITION
 =============================================================================*/
/**
 * sleepIdlePlugin_update
 *
 * @brief This function is invoked by the /sleep/idle/plugin 
 *        resource when a client request is made.
 *
 * It's job is to check the client type, and determine the new state of
 * the resource.  If the client is a required client, then it takes the
 * max of the required clients.  Otherwise, if it is a special client that's
 * passing in a latency, then it determines whether the latency restriction
 * is such that only SWFI should be entered.
 *
 * @param resource: The NPA resource being requested (should be 
 *                  /sleep/idle/plugin).
 * @param client:   The handle to the clients registered to the 
 *                  resource.
 *
 * @return          Returns the new state of the resource.
 *
 * @Details 
 * Since there are multiple types of client with this NPA resource, we need
 * to keep around states/book keeping data for each of them. We are using 
 * various static variables for them.
 *
 * numHoldoffClients : Number of clients that may want to hold off sleep. 
 *                     It is determined by number of different initialization
 *                     schemes present on the processor. Defaults to 1 but
 *                     can be overriden in build file.
 * uSleepState       : Request from uSleep Node that signals whether uSleep is
 *                     enabled or not.
 */
static npa_resource_state sleepIdlePlugin_update( npa_resource      *resource,
                                                  npa_client_handle client )
{
  static int32              numHoldoffClients = SLEEP_HOLDOFF_CLIENTS_NUM;
  static npa_resource_state uSleepState       = SLEEP_OS_IDLE_MODE_DEFAULT;
  npa_resource_state        holdoffState      = 0;

  CORE_VERIFY_PTR(resource);
  CORE_VERIFY_PTR(client);

  switch( client->type )
  {
    case SLEEP_IDLE_PLUGIN_CLIENT_LPM_HOLD_OFF:
    {
      /* Usually we do not expect non-zero request with this type of client 
       * but for completeness. If necessary we can assert or treat all requests
       * as release (zero) requests. */
      ( 0 == NPA_PENDING_REQUEST(client).state ) ?
        numHoldoffClients-- : numHoldoffClients++;
      break;
    }
    case SLEEP_IDLE_PLUGIN_CLIENT_ALLOW_USLEEP:
    {
      /* Check for possibility of all clients allowing uSleep to enter
       * A non-zero state indicates all clients are allowing uSleep */
      uSleepState = ( 0 != NPA_PENDING_REQUEST(client).state ) ?
        (npa_resource_state) SLEEP_OS_IDLE_MODE_USLEEP :
        (npa_resource_state) SLEEP_OS_IDLE_MODE_DEFAULT;
      break;
    }
    default:
    {
      CORE_VERIFY(0);
      break;
    }
  }

  /* Determine the hold off state */
  holdoffState = ( numHoldoffClients <= 0 ) ? 
    (npa_resource_state) SLEEP_OS_IDLE_MODE_DEFAULT :
    (npa_resource_state) SLEEP_OS_IDLE_MODE_HOLD_OFF;

  return MAX( holdoffState, uSleepState );
}

/**
 * sleepIdlePlugin_driver
 *
 * @brief Driver function for the /sleep/idle/plugin resource 
 *        that updates its state.
 *
 * @param resource: Pointer to the /sleep/idle/plugin resource
 * @param client: Client of the resource
 * @param state: state of the resource.
 *
 * @return state of the resource.
 */
static npa_resource_state sleepIdlePlugin_driver (npa_resource        *resource,
                                                   npa_client         *client,
                                                  npa_resource_state  state)
{
  /* There is a race condition where ATS may finish the query before NPA 
   * framework can process the return vlaue from driver causing ATS to use 
   * stale values. Until NPA has support for synchronous POST_CHANGE 
   * callbacks update the final calculated state here as recommended 
   * by NPA team - this is as close as we can get to that feature. */
  resource->definition->data = (npa_user_data)state;

  sleepActive_SetSignal( SLEEP_SIGNAL_CONTROL );

  return state;
}

/**
 * sleepIdlePlugin_query
 *
 * @brief Query function for idle plugin node.
 *
 * @see @sleepIdlePluginState
 *
 * @param resource: Pointer to resource being queried (/sleep/idle/plugin)
 * @param query_id: ID for the query.
 * @param query: Result of the query.
 *
 * @return Status based on how query was handled.
 */
static npa_query_status sleepIdlePlugin_query( npa_resource *resource,
                                               unsigned int query_id,
                                               npa_query_type *query )
{
  npa_query_status status = NPA_QUERY_SUCCESS;

  switch( query_id )
  {
    case NPA_QUERY_CURRENT_STATE:
      query->type = NPA_QUERY_TYPE_STATE;
      query->data.state = (npa_resource_state)(resource->definition->data);
      break;

    default:
      status = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      break;
  }

  return status;
}

/**
 * @brief sleep_idle_plugin
 *
 * The plugin for the /sleep/idle/plugin resource.
 */
const npa_resource_plugin sleep_idle_plugin_resource = 
{
  sleepIdlePlugin_update,                     /* Update function */
  SLEEP_IDLE_PLUGIN_CLIENT_LPM_HOLD_OFF |     /* Supported client types */
  SLEEP_IDLE_PLUGIN_CLIENT_ALLOW_USLEEP,
  NULL,                                       /* Create client function */
  NULL                                        /* Destroy client function */
};

/**
 * @brief sleep_idle_plugin_resource
 *
 * Definition of the idle plugin resource. 
 */
static npa_resource_definition sleep_idle_plugin_definition[] = 
{ 
  {  
    "/sleep/idle/plugin",                     /* Name */
    "plugin_type",                            /* Units */
    NPA_MAX_STATE,                            /* Max State */
    &sleep_idle_plugin_resource,              /* Plugin */
    NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED, /* Attributes */
    NULL,                                     /* User Data */
    sleepIdlePlugin_query                     /* Query Function */
  }
};

/**
 * @brief sleep_idle_plugin_node
 *
 * Definition of the idle plugin node. 
 */
static npa_node_definition sleep_idle_plugin_node = 
{ 
  "/node/sleep/idle/plugin",    /* name */
  sleepIdlePlugin_driver,       /* driver_fcn */
  NPA_NODE_DEFAULT,             /* attributes */
  NULL,                         /* data */
  NPA_EMPTY_ARRAY,
  NPA_ARRAY( sleep_idle_plugin_definition )
};

/**
 * sleepIdlePlugin_initializeLog 
 *  
 * @brief Initializes the logging capabilities of the idle 
 *        plugin resource.  This adds the Sleep Requests log as
 *        one of the destination logs for messages pertaining to
 *        this resource.
 */
static void sleepIdlePlugin_initializeLog( void )
{
  CORE_VERIFY( NPA_SUCCESS == ( npa_add_resource_log_by_handle(
                                  "Sleep Requests", 
                                  sleep_idle_plugin_definition[0].handle ) ) );
}

#else /* SLEEP_NUM_CORES != 1 || FEATURE_SLEEP_NO_IDLE_PLUGIN */

/**
 * sleepIdlePlugin_updateStub
 *
 * @brief This function is invoked by the /sleep/idle/plugin 
 *        resource when a client request is made.  This one is a stub that
 *        does nothing, for environments that don't want this feature.
 *
 * @param resource: The NPA resource being requested (should be 
 *                  /sleep/idle/plugin).
 * @param client: The handle to the clients registered to the 
 *                resource.
 *
 * @return Returns the new state of the resource.
 */
static npa_resource_state sleepIdlePlugin_updateStub(npa_resource      *resource,
                                                     npa_client_handle client)
{
  return NPA_PENDING_REQUEST(client).state;
}

/**
 * sleepIdlePlugin_driverStub
 *
 * @brief Driver function for the /sleep/idle/plugin resource 
 *        that updates its state.  This one is a stub that
 *        does nothing, for environments that don't want this feature.
 *
 * @param resource: Pointer to the /sleep/idle/plugin resource
 * @param client: Client of the resource
 * @param state: state of the resource.
 *
 * @return state of the resource.
 */
static npa_resource_state sleepIdlePlugin_driverStub(npa_resource       *resource,
                                                     npa_client         *client,
                                                     npa_resource_state state )
{
  return state;
}

/**
 * @brief sleep_idle_plugin_resource_stub
 *
 * The plugin for the /sleep/idle/plugin resource.  This one is a stub that
 * does nothing, for environments that don't want this feature.
 */
const npa_resource_plugin sleep_idle_plugin_resource_stub = 
{
  sleepIdlePlugin_updateStub,              /* Update function */
  SLEEP_IDLE_PLUGIN_CLIENT_LPM_HOLD_OFF |  /* Supported client types */
  SLEEP_IDLE_PLUGIN_CLIENT_ALLOW_USLEEP,
  NULL,                                    /* Create client function */
  NULL                                     /* Destroy client function */
};

/**
 * @brief sleep_idle_plugin_definition_stub
 *
 * Definition of the idle plugin resource.  This one is a stub that
 * does nothing, for environments that don't want this feature.
 */ 
static npa_resource_definition sleep_idle_plugin_definition_stub[] = 
{ 
  {  
    "/sleep/idle/plugin",                     /* Name */
    "plugin_type",                            /* Units */
    NPA_MAX_STATE,                            /* Max State */
    &sleep_idle_plugin_resource_stub,         /* Plugin */
    NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED, /* Attributes */
    NULL,                                     /* User Data */
    NULL                                      /* Query Function */
  }
};

/**
 * @brief sleep_idle_plugin_node
 *
 * Definition of the idle plugin node.  This one is a stub that
 * does nothing, for environments that don't want this feature. 
 */
static npa_node_definition sleep_idle_plugin_node = 
{ 
  "/node/sleep/idle/plugin",     /* name */
  sleepIdlePlugin_driverStub,    /* driver_fcn */
  NPA_NODE_DEFAULT,              /* attributes */
  NULL,                          /* data */
  NPA_EMPTY_ARRAY,
  NPA_ARRAY( sleep_idle_plugin_definition_stub )
};

/**
 * sleepIdlePlugin_initializeLog
 *  
 * @brief Initializes the logging capabilities of the idle 
 *        plugin resource.  This removes the NPA log as a
 *        destination for logging of this resource.  After
 *        which, no log messages will be logged for the
 *        resource.
 */
static void sleepIdlePlugin_initializeLog( void )
{
  CORE_VERIFY( NPA_SUCCESS == 
               ( npa_remove_resource_log_by_handle( 
                 NPA_DEFAULT_LOG_NAME, 
                 sleep_idle_plugin_definition_stub[0].handle ) ) );
}

#endif /* SLEEP_NUM_CORES != 1 || FEATURE_SLEEP_NO_IDLE_PLUGIN */

/*
 * sleep_plugin_init
 */
void sleepIdlePlugin_initialize( void )
{
  npa_resource_state initial_state[] = 
  { 
    (npa_resource_state)SLEEP_OS_IDLE_MODE_HOLD_OFF
  };

  npa_define_node( &sleep_idle_plugin_node, initial_state, NULL );
  sleepIdlePlugin_initializeLog();
}

