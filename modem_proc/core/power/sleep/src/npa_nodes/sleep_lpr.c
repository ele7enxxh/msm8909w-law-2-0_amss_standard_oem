/*==============================================================================
  FILE:         sleep_lpr.c
 
  OVERVIEW:     This file provides the implementation of the sleep LPR node.

  DEPENDENCIES: None

                Copyright (c) 2010-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/npa_nodes/sleep_lpr.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include <stdlib.h>
#include "DALStdDef.h"
#include "CoreVerify.h"
#include "sleep_lpr.h"
#include "npa_resource.h"
#include "sleepi.h"
#include "sleep_target.h"
#include "sleep_idle_plugin.h"
#include "sleep_lpri.h"
#include "CLprRegistry.h"
#include "sleep_client.h"
#include "sleep_log.h"
#include "SleepLPR_lookup_table.h"

/*
===============================================================================

                          INTERNAL MACRO DEFINITIONS

===============================================================================
*/

/**
 * SLEEP_LPR_MAX_NAME_LENGTH
 *
 * @brief Maximum allowable length of LPR(M)'s name.
 */
#define SLEEP_LPR_MAX_NAME_LENGTH 40

/*
===============================================================================

                              EXTERN DECLARATIONS

===============================================================================
*/

/*
 * sleep_client_lpr_register.
 */
extern void sleep_client_lpr_register( sleep_lpr *lpr,
                                       npa_client *client,
                                       CLprRegistry *lprRegistry );

/*
===============================================================================

                 INTERNAL DECLARATIONS FOR SLEEP LPR MGMT NODE

===============================================================================
*/

/**
 * @brief gLprRegistry
 *
 * Registry for sleep system where low power resources (and their modes) are
 * registered.
 */
CLprRegistry *gLprRegistry = NULL;

/**
 * @brief gSleepNumOfCores
 *
 * Indicates the number of cores sleep running on. 
 * (volatile to work with certain platform CMM scripts)
 */
volatile int gSleepNumOfCores = SLEEP_NUM_CORES;

/**
 * @brief gSleepNumOfFreqs
 *
 * Indicates the number of core frequencies in lookup table 
 * (volatile to work with certain platform CMM scripts) 
 */
volatile int gSleepNumOfFreqs = SLEEP_NUM_STATIC_FREQ;

/**
 * @brief Structure that stores information of a low power resource and its
 *        handle.
 */
typedef struct NewLprInfo NewLprInfo;
struct NewLprInfo
{
  sleep_lpr *lpr;
  npa_client_handle client;
};

/**
 * sleep_lpr_destroy_client
 *
 * @brief Removes the client from the memory.
 *
 * @param client: Pointer to the npa_client which we want to remove 
 */
void sleep_lpr_destroy_client(npa_client *client)
{
  if(client->resource_data)
  {
    SleepClient *sleep_client = (SleepClient *)client->resource_data;
    SleepClient_destroy(sleep_client);
    free(sleep_client);
  }
}

/**
 * sleep_lpr_update
 *
 * @brief Update function for sleep npa node.
 *
 * The Update Function receives the new request and determines the new
 * state for the resource. It is part of the plugin for the sleep lpr
 * npa node.
 *
 * @param resource: Pointer to the resource (correspoding to npa node) on which
 *                  update functions is getting called.
 * @param client: Handle to the npa client.
 *
 * @return New state of the resource.
 */
static npa_resource_state sleep_lpr_update(npa_resource *resource, 
                                           npa_client_handle client)
{
  return 0;
}

/**
 * sleep_lpr_driver
 *
 * @brief Driver function for the sleep lpr npa node.
 */
static npa_resource_state sleep_lpr_driver (npa_resource *resource,
                                            npa_client *client,
                                            npa_resource_state state)
{
  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    sleep_target_lpr_init();
    return 0;
  }

  sleep_client_request( resource, client, state );

  return state;
}

/**
 * sleep_lpr_register
 *
 * @brief Registers the low power resource with sleep system.
 *
 * @param info: Structure containing low power resource and its handle.
 */
static void sleep_lpr_register(NewLprInfo *info)
{
  sleep_lpr  *lpr    = info->lpr;  
  npa_client *client = info->client;

  CORE_VERIFY_PTR(lpr);

  /* Make sure this LPR is not already registered. */
  if( TRUE == lpr->registered )
  {
    sleep_log_printf( SLEEP_LOG_LEVEL_ERROR, 1, 
                      "ERROR (message: \"Sleep LPR is already registered\") "
                      "(LPR Name: \"%s\")", lpr->resource_name );
    CORE_VERIFY( 0 );
  }

  /* Initializing resource if its init function was provided */
  if( NULL != lpr->init )
  {
    sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 1, 
                      "Initializing Resource (Name: \"%s\")", 
                      lpr->resource_name );
    lpr->init();
  }

  sleep_client_lpr_register( lpr, client, gLprRegistry );

  lpr->registered = TRUE;
}

/**
 * sleep_lpr_query_fcn
 *
 * @brief Query function for sleep lpr npa node that is mainly used to support
 *        LPR registration and querying sleep registry.
 *
 * @param resource: Pointer to the resource we are querying.
 * @param query_id: ID of the query (definied in sleep_lpri.h)
 * @param query_result: Type of the query.
 */
npa_query_status sleep_lpr_query_fcn
(
  struct npa_resource *resource, 
  unsigned int query_id, 
  npa_query_type *query_result
)
{
  npa_query_status status = NPA_QUERY_SUCCESS;
  NewLprInfo *lprInfo;

  CORE_VERIFY_PTR(resource);
  CORE_VERIFY_PTR(query_result);

  switch(query_id)
  {
    case SLEEP_LPR_REGISTER:
      {
        CORE_VERIFY(NPA_QUERY_TYPE_REFERENCE == query_result->type);

        lprInfo = (NewLprInfo *)(query_result->data.reference);

        CORE_VERIFY_PTR(lprInfo);

        /* Read global config for this LPR.
           Note: called outside npa lock, must be re-entrant */
        CLprRegistry_readGlobalConfig( gLprRegistry, lprInfo->lpr );

        npa_resource_lock(resource);
  
        sleep_lpr_register(lprInfo);
  
        npa_resource_unlock(resource);
      }
      break;

    case SLEEP_LPR_QUERY_RESOURCES:
      {
        CORE_VERIFY(query_result);

        /*
         * FIXME: returning the pointer to the lpr registry here is not
         * multicore-safe, but it's the same behavior we had before.
         * in the future, maybe return a reference to the enabled mode
         * vector?
         */
        query_result->type = NPA_QUERY_TYPE_REFERENCE;
        query_result->data.reference = gLprRegistry;
      }
      break;

    default:
      status = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      break;
  }

  return status;
}

/**
 * @brief sleep_lpr_plugin
 *
 * The plugin for the /sleep/lpr resource.
 */
static const npa_resource_plugin sleep_lpr_plugin = 
{
  sleep_lpr_update,         /* Update function */
  NPA_CLIENT_REQUIRED,      /* Supported client types */
  NULL,                     /* Create client function */
  sleep_lpr_destroy_client, /* Destroy client function */
};

/**
 * @brief sleep_lpr_resource
 *
 * /sleep/lpr psuedo-resource definition
 */
static npa_resource_definition sleep_lpr_resource[] =
{
  {
    "/sleep/lpr",                      /* Name */
    "bitmask",                         /* Units */
    NPA_MAX_STATE,                     /* Max State */
    &sleep_lpr_plugin,                 /* Plugin */
    NPA_RESOURCE_DRIVER_UNCONDITIONAL, /* Attributes */
    NULL,                              /* User Data */
    sleep_lpr_query_fcn                /* Query Function */
  }
};

/**
 * @brief sleep_lpr_node
 *
 * /sleep/lpr node definiton
 */
static npa_node_definition sleep_lpr_node = 
{ 
  "/node/sleep/lpr", /* name */
  sleep_lpr_driver,  /* driver_fcn */
  NPA_NODE_DEFAULT,  /* attributes */
  NULL,              /* data */
  NPA_EMPTY_ARRAY,
  NPA_ARRAY(sleep_lpr_resource)
};

/*
 * sleep_lpr_init
 */
void sleep_lpr_init(void)
{
  npa_resource_state intial_state[] = {0};

  /* Initialize the sleep idle plugin subsystem, since this node depends
   * on it. */
  sleep_idle_plugin_init();

  CORE_VERIFY_PTR(gLprRegistry = malloc(sizeof(CLprRegistry)));
  CLprRegistry_init(gLprRegistry);

  npa_define_node( &sleep_lpr_node, intial_state, NULL );
}

/*
 * Internal function called by sleep framework to register LPR which are
 * defined by build time synthesis tool.
 *
 * @param resource: Pointer to Low Power Resource.
 * @param client_handle: NPA client handle which will be used to issue
 *                       request for above LPR.
 */
static void sleep_define_lpr_internal(sleep_lpr *resource, 
                                      npa_client_handle client_handle)
{
  /*
   * This function foists the registration off to the resource itself, which 
   * does the work inside the resource lock
   */
  NewLprInfo new_lpr;
  npa_query_type q;

  new_lpr.lpr = resource;
  new_lpr.client = client_handle;
  q.type = NPA_QUERY_TYPE_REFERENCE;
  q.data.reference = &new_lpr;
  CORE_VERIFY( NPA_QUERY_SUCCESS == npa_query_by_client( client_handle, 
                                                         SLEEP_LPR_REGISTER, 
                                                         &q ) );
}

/*
 * sleep_define_lpr_str
 */
int sleep_define_lpr_str(const char *name, npa_client_handle client_handle)
{
  int     result = SLEEP_LPR_FAILED;
  int     nCount = sizeof(SleepLPR_LookupTable) / 
                    sizeof(SleepLPR_LookupTable[0]);
  int     i      = 0;
  size_t  strLenCmp;

  if(name != NULL)
  {
    for (i = 0; result == SLEEP_LPR_FAILED && i < nCount; i++)
    {
      /* Since the lookup table is controled from sleep tools, it will always
       * be NULL terminated */
      strLenCmp = strlen(SleepLPR_LookupTable[i].pName);

      if ( ( strncmp(name, SleepLPR_LookupTable[i].pName, strLenCmp) == 0 ) &&
           ( name[strLenCmp] == '\0' ) )
      {
        sleep_define_lpr_internal(SleepLPR_LookupTable[i].pLPR, client_handle);
        result = SLEEP_LPR_SUCCESS;
      }
    }
  }

  if (result == SLEEP_LPR_FAILED)
  {
     sleep_log_printf( SLEEP_LOG_LEVEL_WARNING, 1, 
                       "WARNING (message: \"Sleep LPR not found\") "
                       "(LPR Name: \"%s\")", name);
  }

  return result;
}

/*
===============================================================================
                          GLOBAL FUNCTION DEFINTIONS
===============================================================================
*/

#ifndef FEATURE_SLEEP_NO_SYNTHESIS
/*
 * sleep_define_lpr_synth
 */
void sleep_define_lpr_synth( sleep_lpr_synth *lpr)
{
  CLprRegistry_registerLpr_synth( gLprRegistry, lpr );
}

#endif /* !FEATURE_SLEEP_NO_SYNTHESIS */

