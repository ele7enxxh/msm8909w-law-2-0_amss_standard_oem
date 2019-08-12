/**

@file   icb_rpm_master_latency.c

@brief  Implementation of icb master aggregator.
*/
/*
===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/buses/icb/src/common/icb_rpm_master_latency.c#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
2014/09/17   sds     Fix aggregation across multiple clients in apply function.
2014/08/14   sds     Fix potential null pointer dereference
2014/07/30   sds     Update usage of rpm server API and check for NULL.
2014/06/12   sds     Created 

===========================================================================
         Copyright (c) 2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                          QUALCOMM Proprietary
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "alloca.h"
#include "CoreVerify.h"
#include "icb_rpmi.h"
#include "rpmserver.h"
#include "icb_rpm_types.h"
#include "swevent.h"

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Data
** ----------------------------------------------------------------------- */
static icb_system_info_type *sys_info; /**< Pointer to the system info. */

/* -----------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */
static icb_master_latency_type const *get_master
(
  unsigned id
)
{
  ICBId_MasterType master_id    = (ICBId_MasterType)id;
  icb_master_latency_type const *master = NULL;

  for( uint32_t idx = 0; idx < sys_info->num_latency_masters; idx++ )
  {
    if( sys_info->latency_masters[idx]->masterId == master_id )
    {
      master = sys_info->latency_masters[idx];
      break;
    }
  }

  return master;
}

void master_latency_update
(
  const icb_master_latency_type *master,
  icb_master_latency_req_type    latency,
  icb_slave_req_type             bw
)
{
  const icb_latency_table_type *latency_table = master->latency_table;
  uint32_t freq = 0;
  int32_t lat_idx;

  /* Only bother to search if the latency is a non-zero value. */
  if( latency != 0 )
  {
    /* Find a matching latency index.
    * The latency table is sorted from lowest to highest, so start at the top. */
    for( lat_idx = latency_table->uNumLatencies - 1;
        lat_idx >= 0;
        lat_idx-- )
    {
      /* If we find a matching entry, stop searching. */
      if( latency >= latency_table->aLatencies[lat_idx] )
      {
        break;
      }
    }

    /* If we fall off the bottom of the list, give them best effort
    * and use the lowest latency we support. */
    if( lat_idx < 0 )
    {
      lat_idx = 0;
    }
    
    /* Search our frequency/bw table. */
    const uint16_t *bw_table = latency_table->aBWs[lat_idx];
    uint16_t        bw_MBps  = CEILING_DIV(bw,1000000ULL);
    uint32_t        freq_idx;
    for( freq_idx = 0;
        freq_idx < latency_table->uNumFreqs;
        freq_idx++ )
    {
      if( bw_MBps <= bw_table[freq_idx] )
      {
        break;
      }
    }

    /* Make sure we didn't go off the top of the table. */
    if( freq_idx < latency_table->uNumFreqs )
    {
      freq = latency_table->aFreqs[freq_idx];
    }
    /* Hit the top, just pick max. */
    else
    {
      freq = latency_table->aFreqs[latency_table->uNumFreqs - 1];
    }
  }

  npa_issue_required_request( master->handle, freq * 1000 );
}

void master_latency_update_lat
(
  const icb_master_latency_type *master,
  icb_master_latency_req_type    latency
)
{
  /* Fetch the aggregated value for the associated slave. */
  icb_slave_req_type *bw;
  rpm_get_aggregated_request_buffer( RPM_BUS_SLAVE_REQ, master->slaveId, (const void **)&bw );

  /* If we have a valid request buffer, update with the value of that buffer. */
  if( NULL != bw )
  {
    master_latency_update( master, latency, *bw );
  }
  /* NULL aggregated buffer means zero bandwidth. */
  else
  {
    master_latency_update( master, latency, 0 );
  }
}

void icb_master_latency_update_bw
(
  ICBId_SlaveType    slave_id,
  icb_slave_req_type bw
)
{
  /* Search for all masters monitoring this resource. */
  for( uint32_t idx = 0; idx < sys_info->num_latency_masters; idx++ )
  {
    icb_master_latency_type const *master = sys_info->latency_masters[idx];
    if( master->slaveId == slave_id )
    {
      /* Get the master's aggregated latency request. */
      icb_master_latency_req_type *latency;
      rpm_get_aggregated_request_buffer( RPM_BUS_MASTER_LATENCY_REQ, master->masterId, (const void **)&latency );

      /* If we have a valid request buffer, update with the value of that buffer. */
      if( NULL != latency )
      {
        master_latency_update( master, *latency, bw );
      }
      /* NULL aggregated buffer means zero bandwidth. */
      else
      {
        master_latency_update( master, 0, bw );
      }
    }
  }
}

void icb_master_latency_translate_cb
(
  rpm_translation_info *info
)
{
  /* Try to find the master that corresponds with the incoming id. */
  icb_master_latency_type const *master = get_master( info->id );

  /* If found, translate. */
  if( master )
  {
    unsigned                     type, length;
    const char                  *value;
    uint32_t                     latency;
    icb_master_latency_req_type *req = info->dest_buffer;

    /* Read until we run out of KVPs. */
    while( !kvp_eof(info->new_kvps) )
    {
      /* Make sure it's one of the kvps we expect.  */
      if( kvp_get( info->new_kvps, &type, &length, &value ) )
      {
        /* Validate the length of the buffer. */
        if( sizeof(uint32_t) == length )
        {
          latency = *((uint32_t *)value);
        }
        /* If the length is zero, this is an invalidate.
        ** Zero this buffer. */
        else if( 0 == length )
        {
          latency = 0;
        }
        /* Invalid length, and not a an invalidate.
        ** Skip it. */
        else
        {
          continue;
        }

        /* No translation required, just copy it. */
        switch( type )
        {
          case ICB_LATENCY_MASTER_FIELD_NS:
            *req = latency;
            SWEVENT(BUS_EVENT_MASTER_LATENCY_XLATE, info->id, latency);
            break;
          default:
            /* Invalid key, ignore. */
            break;
        }
      }
    }
  }
}

void icb_master_latency_apply_cb
(
 rpm_application_info *info
)
{
  /* Try to find the master that corresponds with the incoming id. */
  icb_master_latency_type const *master = get_master( info->id );

  /* If found, aggregate and apply. */
  if( master )
  {
    icb_master_latency_req_type *current = (icb_master_latency_req_type *)info->current_aggregation;
    icb_master_latency_req_type *new;

    /* If we don't have an new state, allocate a bit on the stack and clear it. */
    if( !info->new_state )
    {
      new = alloca( sizeof(icb_master_latency_req_type) );
      memset( new, 0, sizeof(icb_master_latency_req_type) );
    }
    /* Otherwise, use what we're given. */
    else
    {
      new = (icb_master_latency_req_type *)info->new_state;
    }

    /* Do a MIN aggregation. */
    icb_master_latency_req_type aggregate = 0;
    for( unsigned client = 0; 
         client < rpm_get_num_clients( info->resource_handle );
         client++ )
    {
      icb_master_latency_req_type req;
      unsigned client_type;
      icb_master_latency_req_type *request;

      /* Ask the RPM server what our request is for this client */
      rpm_get_current_request( info->resource_handle, client, &client_type, (const void **)&request);

      /* Verify the request pointer.
       * It's possible that there isn't a request due to an invalidate. */
      if( NULL != request )
      {
        req = *request;
      }
      /* Invalidate. Assume zero. */
      else
      {
        req = 0;
      }

      /* If the current aggregate is zero, i.e. no request,
       * just overwrite it with the client request.
       * Otherwise, only overwrite min if the client request is non-zero and smaller. */
      if( 0 == aggregate ||
          (0 != aggregate && 0 != req && req < aggregate))
      {
        aggregate = req;
      }
    }

    /* Save new current state. */
    *current = aggregate;

    /* Update the master and hardware. */
    SWEVENT(BUS_EVENT_MASTER_LATENCY_APPLY, info->id, *current);
    master_latency_update_lat( master, *current );
  }
}

/**
@copydoc icb_master_latency_init()
*/
void icb_master_latency_init( void )
{
  /* Get a pointer to the system resources. */
  CORE_VERIFY( sys_info = icb_get_target_resources() );

  /* If there aren't any shared masters, bail. */
  if( !sys_info->num_latency_masters )
  {
    return;
  }

  /* Create clock handles for each master. */
  for( uint32_t idx = 0; idx < sys_info->num_latency_masters; idx++ )
  {
    icb_master_latency_type *master = sys_info->latency_masters[idx];
    CORE_VERIFY_PTR( master->handle = 
      npa_create_sync_client(master->clk, "ICB Latency Driver", NPA_CLIENT_REQUIRED) );
  }

  /* Register bandwidth resources with the RPM driver. */
  rpm_register_named_resource( RPM_BUS_MASTER_LATENCY_REQ,
                               sys_info->num_latency_masters,
                               sys_info->latency_master_ids,
                               sizeof(icb_master_latency_req_type),
                               icb_master_latency_translate_cb,
                               icb_master_latency_apply_cb,
                               NULL );
}
