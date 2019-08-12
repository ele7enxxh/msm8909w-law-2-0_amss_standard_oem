/*==============================================================================
  FILE:         sleep_idle_plugin.c
  
  OVERVIEW:     This file provides the node definition and supporting functions
                for the idle entry point plugin.

  DEPENDENCIES: None

                Copyright (c) 2011-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/npa_nodes/sleep_idle_plugin.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "sleep_lpr.h"
#include "npa_resource.h"
#include "npa_log.h"
#include "CoreVerify.h"
#include "sleep_idle_plugin.h"

#if ( SLEEP_NUM_CORES == 1 ) && !defined ( FEATURE_SLEEP_NO_IDLE_PLUGIN )
#include <stdint.h>
#include "sleep_v.h"
#include "sleepi.h"
#include "sleep_log.h"

/* ============================================================================
                               MACRO DEFINITIONS
============================================================================= */

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

/* ============================================================================
                              INTERNAL DEFINITIONS
============================================================================= */
sleep_idle_entry_ptr_type sleep_idle_entry_point_tbl[] = 
{
  sleep_perform_lpm,    /* SLEEP_IDLE_PLUGIN_DEFAULT      */
  sleep_swfi,           /* SLEEP_IDLE_PLUGIN_SWFI_ONLY    */
  sleep_holdoff_no_mode    /* SLEEP_IDLE_PLUGIN_HOLD_OFF     */
};

/* ============================================================================
                       SLEEP IDLE PLUGIN NODE DEFINITION
============================================================================= */

/**
 * sleep_idle_plugin_update
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
 * @param client: The handle to the clients registered to the 
 *                resource.
 *
 * @return Returns the new state of the resource.
 *
 * @Details 
 * Since there are multiple types of client with this NPA resource, we need
 * to keep around states/book keeping data for each of them. We are using 
 * various static variables for them.
 *
 * req_state:           State determined by aggregating explicit requests 
 *                      (may or may not be honored).
 * latency_state:       State determined by various latency restrictions.
 * min_mode_latency:    Minimum enter plus exit latency of some mode out of all
 *                      enabled modes.
 * latency_budget:      Minimum latency restriction enforced on Sleep.
 * min_sleep_latency:   Minimum sleep processing overhead without any mode.
 * holdoff_clients_num: Number of clients that may want to hold off sleep. 
 *                      It is determined by number of different initialization
 *                      schemes present on the processor. Defaults to 1 but
 *                      can be overriden in build file.
 */
static npa_resource_state sleep_idle_plugin_update( npa_resource *resource, 
                                                    npa_client_handle client)
{
  static npa_resource_state req_state = 0;
  static npa_resource_state latency_state     = 0;
  static int32 holdoff_clients_num = SLEEP_HOLDOFF_CLIENTS_NUM;
  static uint32             min_mode_latency  = UINT32_MAX;
  static uint32             latency_budget    = UINT32_MAX;
  static uint32             min_sleep_latency = UINT32_MAX;

  npa_resource_state holdoff_state = 0;
  npa_resource_state        old_latency_state = latency_state;

  CORE_VERIFY_PTR(resource);
  CORE_VERIFY_PTR(client);

  switch( client->type )
  {
    case NPA_CLIENT_REQUIRED:
    {
      /* Someone is requesting a specific idle entry point.  Take the max
       * of the client votes.  This will return the max of the required
       * and suppressible clients. */
      req_state = npa_max_plugin.update_fcn( resource, client );
      break;
    }

    case SLEEP_IDLE_PLUGIN_CLIENT_LPM_HOLD_OFF:
    {
      /* Usually we do not expect non-zero request with this type of client 
       * but for completeness. If necessary we can assert or treat all requests
       * as release (zero) requests. */
      ( 0 == NPA_PENDING_REQUEST(client).state ) ? 
      holdoff_clients_num-- : holdoff_clients_num++;

      break;
    }

    case SLEEP_IDLE_PLUGIN_CLIENT_MIN_MODE_LATENCY:
    {
      /* A minimum latency is being enforced.  This reflects the mode that's 
       * enabled with the minimum enter + exit time. */
      min_mode_latency = NPA_PENDING_REQUEST(client).state;
      break;
    }
    case SLEEP_IDLE_PLUGIN_CLIENT_LATENCY:
    {
      /* A new latency budget is being enforced. */
      latency_budget = NPA_PENDING_REQUEST(client).state;
      break;
    }
    case SLEEP_IDLE_PLUGIN_CLIENT_MIN_SLEEP_LATENCY:
    {
      /* A minimum latency for executing the sleep idle operations is
       * being enforced. */
      min_sleep_latency = NPA_PENDING_REQUEST(client).state;
      break;
    }
    default:
    {
      break;
    }
  }

  /* Determine whether the latency restrictions result in a desire to just
   * do SWFI. */
  if ( latency_budget < min_mode_latency || 
       latency_budget < min_sleep_latency )
  {
    latency_state = ( npa_resource_state )SLEEP_IDLE_PLUGIN_SWFI_ONLY;
  }
  else
  {
    latency_state = ( npa_resource_state )SLEEP_IDLE_PLUGIN_DEFAULT;
  }

  if ( old_latency_state != latency_state )
  {
    /* The latency state changed, so log a message. */
    sleep_log_printf( SLEEP_LOG_LEVEL_REQUESTS, 4, 
                      "Requesting idle plugin "
                      "(reason: latency restriction changed) "
                      "(plugin: %s) (min mode latency: 0x%x) "
                      "(min sleep latency: 0x%x) "
                      "(latency restriction: 0x%x)", 
                      latency_state == SLEEP_IDLE_PLUGIN_SWFI_ONLY ? 
                      "SWFI only" : "Default", 
                      min_mode_latency,
                      min_sleep_latency,
                      latency_budget );
  } 

  /* Determine the hold off state */
  holdoff_state = ( holdoff_clients_num <= 0) ? 
                  (npa_resource_state) SLEEP_IDLE_PLUGIN_DEFAULT :
                  (npa_resource_state) SLEEP_IDLE_PLUGIN_HOLD_OFF;

  /* Take the max of the client votes and return it. */
  return MAX( MAX( req_state, holdoff_state ), latency_state );
}

/**
 * sleep_idle_plugin_driver
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
static npa_resource_state sleep_idle_plugin_driver (npa_resource *resource,
                                                    npa_client *client,
                                                    npa_resource_state state)
{
  CORE_VERIFY_PTR(resource);
  CORE_VERIFY_PTR(client);

  if ( client->type == NPA_CLIENT_INITIALIZE )
  {
    /* Nothing to do currently. */
  }

  /* Verify that the node aggregated to a valid state. */
  CORE_VERIFY( state <  ( npa_resource_state )SLEEP_IDLE_PLUGIN_MAX );

  /* Look up the desired idle entry point, and set it. */
  sleep_os_set_idle_entry( 0, sleep_idle_entry_point_tbl[state] );

  return state;
}

/**
 * @brief sleep_idle_plugin
 *
 * The plugin for the /sleep/idle/plugin resource.
 */
const npa_resource_plugin sleep_idle_plugin = 
{
  sleep_idle_plugin_update,                   /* Update function */
  NPA_CLIENT_REQUIRED | 
  SLEEP_IDLE_PLUGIN_CLIENT_MIN_MODE_LATENCY | 
  SLEEP_IDLE_PLUGIN_CLIENT_LATENCY | 
  SLEEP_IDLE_PLUGIN_CLIENT_MIN_SLEEP_LATENCY |
  SLEEP_IDLE_PLUGIN_CLIENT_LPM_HOLD_OFF,      /* Supported client types */  
  NULL,                                       /* Create client function */
  NULL                                        /* Destroy client function */
};

/**
 * @brief sleep_idle_plugin_resource
 *
 * Definition of the idle plugin resource. 
 */
static npa_resource_definition sleep_idle_plugin_resource[] = 
{ 
  {  
    "/sleep/idle/plugin",                     /* Name */
    "plugin_type",                            /* Units */
    NPA_MAX_STATE,                            /* Max State */
    &sleep_idle_plugin,                       /* Plugin */
    NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED, /* Attributes */
    NULL,                                     /* User Data */
    NULL                                      /* Query Function */
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
  sleep_idle_plugin_driver,     /* driver_fcn */
  NPA_NODE_DEFAULT,             /* attributes */
  NULL,                         /* data */
  NPA_EMPTY_ARRAY,
  NPA_ARRAY( sleep_idle_plugin_resource )
};

/**
 * sleep_idle_plugin_log_init 
 *  
 * @brief Initializes the logging capabilities of the idle 
 *        plugin resource.  This adds the Sleep Requests log as
 *        one of the destination logs for messages pertaining to
 *        this resource.
 */
static void sleep_idle_plugin_log_init( void )
{
  CORE_VERIFY( NPA_SUCCESS == ( npa_add_resource_log_by_handle( 
                                    "Sleep Requests", 
                                    sleep_idle_plugin_resource[0].handle ) ) );
}

#else /* SLEEP_NUM_CORES != 1 || FEATURE_SLEEP_NO_IDLE_PLUGIN */

/**
 * sleep_idle_plugin_update_stub
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
static npa_resource_state sleep_idle_plugin_update_stub( 
                                              npa_resource *resource, 
                                              npa_client_handle client)
{
  return NPA_PENDING_REQUEST(client).state;
}

/**
 * sleep_idle_plugin_driver_stub
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
static npa_resource_state sleep_idle_plugin_driver_stub (
                                    npa_resource *resource,
                                    npa_client *client,
                                    npa_resource_state state )
{
  return state;
}

/**
 * @brief sleep_idle_plugin_stub
 *
 * The plugin for the /sleep/idle/plugin resource.  This one is a stub that
 * does nothing, for environments that don't want this feature.
 */
const npa_resource_plugin sleep_idle_plugin_stub = 
{
  sleep_idle_plugin_update_stub,              /* Update function */
  NPA_CLIENT_REQUIRED | 
  SLEEP_IDLE_PLUGIN_CLIENT_MIN_MODE_LATENCY | 
  SLEEP_IDLE_PLUGIN_CLIENT_LATENCY | 
  SLEEP_IDLE_PLUGIN_CLIENT_MIN_SLEEP_LATENCY |
  SLEEP_IDLE_PLUGIN_CLIENT_LPM_HOLD_OFF,      /* Supported client types */  
  NULL,                                       /* Create client function */
  NULL                                        /* Destroy client function */
};

/**
 * @brief sleep_idle_plugin_resource_stub
 *
 * Definition of the idle plugin resource.  This one is a stub that
 * does nothing, for environments that don't want this feature.
 */ 
static npa_resource_definition sleep_idle_plugin_resource_stub[] = 
{ 
  {  
    "/sleep/idle/plugin",                     /* Name */
    "plugin_type",                            /* Units */
    NPA_MAX_STATE,                            /* Max State */
    &sleep_idle_plugin_stub,                  /* Plugin */
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
  sleep_idle_plugin_driver_stub, /* driver_fcn */
  NPA_NODE_DEFAULT,              /* attributes */
  NULL,                          /* data */
  NPA_EMPTY_ARRAY,
  NPA_ARRAY( sleep_idle_plugin_resource_stub )
};

/**
 * sleep_idle_plugin_log_init 
 *  
 * @brief Initializes the logging capabilities of the idle 
 *        plugin resource.  This removes the NPA log as a
 *        destination for logging of this resource.  After
 *        which, no log messages will be logged for the
 *        resource.
 */
static void sleep_idle_plugin_log_init( void )
{
  CORE_VERIFY( NPA_SUCCESS == 
                ( npa_remove_resource_log_by_handle( 
                             NPA_DEFAULT_LOG_NAME, 
                             sleep_idle_plugin_resource_stub[0].handle ) ) );
}

#endif /* SLEEP_NUM_CORES != 1 || FEATURE_SLEEP_NO_IDLE_PLUGIN */

/*
 * sleep_plugin_init
 */
void sleep_idle_plugin_init( void )
{
  npa_resource_state initial_state[] = 
  { 
    (npa_resource_state) SLEEP_IDLE_PLUGIN_HOLD_OFF 
  };

  npa_define_node( &sleep_idle_plugin_node, initial_state, NULL );
  sleep_idle_plugin_log_init();
}

