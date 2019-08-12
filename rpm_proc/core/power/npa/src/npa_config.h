#ifndef NPA_CONFIG_H
#define NPA_CONFIG_H

/*==============================================================================

FILE:      npa_config.h

DESCRIPTION: Target Configuration Data Structures

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

$Header: //components/rel/rpm.bf/2.1.1/core/power/npa/src/npa_config.h#1 $

==============================================================================*/


/* NPA target configuration data
 *
 * The config data structures defines the target-specific configuration data 
 * (pool sizes, workloop priorities, etc).
 * */

/* Helper structures for config data */
typedef struct npa_pool_config
{
  unsigned int initial;         /* Initial allocation */
  unsigned int subsequent;      /* Subsequent allocations */
} npa_pool_config;

typedef struct npa_workloop_config
{
  unsigned int priority;        /* Priorities */
  unsigned int num_events;      /* Number of events */
} npa_workloop_config;

/* Data structure for NPA configuration data */
typedef struct npa_config
{
  unsigned int  log_buffer_size; /* Size in bytes */

  /* Allocations */
  npa_pool_config resource_allocation;
  npa_pool_config client_allocation;
  npa_pool_config event_allocation;
  npa_pool_config link_allocation;
  npa_pool_config event_queue_allocation;

  /* Thread config data */
  npa_workloop_config async_request_workloop;
  npa_workloop_config async_event_workloop;

} npa_config;

extern const npa_config *npa_target_config( void );

#endif /* NPA_CONFIG_H */
