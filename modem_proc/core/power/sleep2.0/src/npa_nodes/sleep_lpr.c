/*==============================================================================
  FILE:         sleep_lpr.c
 
  OVERVIEW:     This file provides the implementation of the sleep LPR node.

  DEPENDENCIES: None

                Copyright (c) 2010-2013 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/npa_nodes/sleep_lpr.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include <stdlib.h>
#include "npa_log.h"
#include "sleepi.h"
#include "sleep_lpr.h"
#include "npa_resource.h"
#include "sleep_target.h"
#include "sleep_idle_plugin.h"
#include "sleep_lpri.h"
#include "synthRegistry.h"
#include "sleep_client.h"
#include "SleepLPR_lookup_table.h"

/*==============================================================================
                         INTERNAL MACRO DEFINITIONS
 =============================================================================*/
/**
 * SLEEP_LPR_MAX_NAME_LENGTH
 *
 * @brief Maximum allowable length of LPR(M)'s name.
 */
#define SLEEP_LPR_MAX_NAME_LENGTH 40

/*==============================================================================
                 INTERNAL DECLARATIONS FOR SLEEP LPR MGMT NODE
 =============================================================================*/
/**
 * @brief g_SleepNumOfCores
 *
 * Indicates the number of cores sleep running on. 
 * (volatile to work with certain platform CMM scripts)
 */
volatile uint32 g_SleepNumOfCores = SLEEP_NUM_CORES;

/**
 * @brief g_SleepNumOfFreqs
 *
 * Indicates the number of core frequencies in lookup table 
 * (volatile to work with certain platform CMM scripts) 
 */
volatile uint32 g_SleepNumOfFreqs = SLEEP_NUM_STATIC_FREQ;

/**
 * @brief Structure that stores information of a low power resource and its
 *        handle.
 */
typedef struct NewLprInfo_s
{
  sleep_lpr         *lpr;
  npa_client_handle client;
}NewLprInfo;

/*==============================================================================
                           INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * sleepLPR_destroyClient
 *
 * @brief Removes the client from the memory.
 *
 * @param client: Pointer to the npa_client which we want to remove 
 */
void sleepLPR_destroyClient(npa_client *client)
{
  if(client->resource_data)
  {
    SleepClient *sleep_client = (SleepClient *)client->resource_data;
    SleepClient_destroy(sleep_client);
    free(sleep_client);
  }

  return;
}

/**
 * sleepLPR_update
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
static npa_resource_state sleepLPR_update(npa_resource       *resource,
                                          npa_client_handle  client)
{
  return 0;
}

/**
 * sleepLPR_driver
 *
 * @brief Driver function for the sleep lpr npa node.
 */
static npa_resource_state sleepLPR_driver(npa_resource       *resource,
                                          npa_client         *client,
                                          npa_resource_state state)
{
  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    sleepTarget_initializeLPR();
    return 0;
  }

  SleepClient_requestClient( resource, client, state );

  return state;
}

/**
 * sleepLPR_register
 *
 * @brief Registers the low power resource with sleep system.
 *
 * @param info: Structure containing low power resource and its handle.
 */
static void sleepLPR_register(NewLprInfo *info)
{
  sleep_lpr  *sleepLPR  = info->lpr;
  npa_client *client    = info->client;

  CORE_VERIFY_PTR(sleepLPR);

  /* Make sure this LPR is not already registered. */
  if( TRUE == sleepLPR->registered )
  {
    sleepLog_printf( SLEEP_LOG_LEVEL_ERROR, 1, 
                      "ERROR (message: \"Sleep LPR is already registered\") "
                      "(LPR Name: \"%s\")", sleepLPR->resource_name );
    CORE_VERIFY( 0 );
  }

  /* Initializing resource if its init function was provided */
  if( NULL != sleepLPR->init )
  {
    sleepLog_printf( SLEEP_LOG_LEVEL_DEBUG, 1, 
                      "Initializing Resource (Name: \"%s\")", 
                      sleepLPR->resource_name );
    sleepLPR->init();
  }

  SleepClient_registerSleepLPR(sleepLPR, client);
  sleepLPR->registered = TRUE;

  return;
}

/**
 * sleepLPR_query
 *
 * @brief Query function for sleep lpr npa node that is mainly used to support
 *        LPR registration and querying sleep registry.
 *
 * @param resource: Pointer to the resource we are querying.
 * @param query_id: ID of the query (definied in sleep_lpri.h)
 * @param query_result: Type of the query.
 */
npa_query_status sleepLPR_query
(
  struct npa_resource *resource,
  unsigned int        query_id,
  npa_query_type      *query_result
)
{
  npa_query_status  status = NPA_QUERY_SUCCESS;
  NewLprInfo        *lprInfo;

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
      synthRegistry_readGlobalConfig(lprInfo->lpr);

      npa_resource_lock(resource);
  
      sleepLPR_register(lprInfo);
  
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
      query_result->type            = NPA_QUERY_TYPE_REFERENCE;
      query_result->data.reference  = synthRegistry_getRegistry();
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
  sleepLPR_update,          /* Update function */
  NPA_CLIENT_REQUIRED,      /* Supported client types */
  NULL,                     /* Create client function */
  sleepLPR_destroyClient,   /* Destroy client function */
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
    sleepLPR_query                     /* Query Function */
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
  sleepLPR_driver,   /* driver_fcn */
  NPA_NODE_DEFAULT,  /* attributes */
  NULL,              /* data */
  NPA_EMPTY_ARRAY,
  NPA_ARRAY(sleep_lpr_resource)
};

/*
 * sleepLPR_initialize
 */
void sleepLPR_initialize(void)
{
  npa_resource_state intial_state[] = {0};

  /* Initialize the sleep idle plugin subsystem, since this node depends
   * on it. */
  sleepIdlePlugin_initialize();
  synthRegistry_init();
  npa_define_node( &sleep_lpr_node, intial_state, NULL );

  /* Log requests to "/sleep/lpr" resource in "Sleep Requests" */
  CORE_VERIFY( NPA_SUCCESS == ( npa_add_resource_log_by_handle(
                                "Sleep Requests", 
                                sleep_lpr_resource[0].handle ) ) );

  return;
}

/*
 * Internal function called by sleep framework to register LPR which are
 * defined by build time synthesis tool.
 *
 * @param resource: Pointer to Low Power Resource.
 * @param client_handle: NPA client handle which will be used to issue
 *                       request for above LPR.
 */
static void sleepLPR_defineInternal(sleep_lpr         *resource, 
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
  return;
}

/*
 * sleepLPR_define
 */
int sleepLPR_define(const char *name, npa_client_handle client_handle)
{
  int     result = SLEEP_LPR_FAILED;
  int     nCount = sizeof(SleepLPR_LookupTable) / 
                    sizeof(SleepLPR_LookupTable[0]);
  int     i      = 0;

  if(name != NULL)
  {
    for (i = 0; result == SLEEP_LPR_FAILED && i < nCount; i++)
    {
      if (strcmp(name, SleepLPR_LookupTable[i]->resource_name) == 0)
      {
        sleepLPR_defineInternal(SleepLPR_LookupTable[i], client_handle);
        result = SLEEP_LPR_SUCCESS;
      }
    }
  }

  if (result == SLEEP_LPR_FAILED)
  {
     sleepLog_printf( SLEEP_LOG_LEVEL_WARNING, 1, 
                      "WARNING (message: \"Sleep LPR not found\") "
                      "(LPR Name: \"%s\")", name);
  }

  return result;
}

