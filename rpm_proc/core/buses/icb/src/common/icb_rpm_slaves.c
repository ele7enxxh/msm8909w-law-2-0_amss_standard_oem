/**

@file   icb_rpm_slaves.c

@brief  Implementation of icb slaves aggregator.
*/
/*
===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/buses/icb/src/common/icb_rpm_slaves.c#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
2014/10/02   sds     Add number of ports for aggregation purposes to masters/slaves.
2014/08/21   sds     Prevent div by 0 when calculating clocks.
2014/06/20   sds     Extend request types to add latency request in addition to BW.
2013/11/25   sds     Add support for configurable aggregation schemes.
2013/02/20   sds     Handle full resource invalidate.
2012/08/17   sds     Handle initial requests and invalidates correctly.
2012/01/20   sds     Updated with RPM server API calls.
2011/11/08   sds     Created 

===========================================================================
             Copyright (c) 2011-2014 Qualcomm Technologies Incorporated.
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
static icb_system_info_type *sys_info; /**< Pointer to the system information. */

/* -----------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */
static icb_slave_type const *get_slave
(
  unsigned id
)
{
  ICBId_SlaveType slave_id    = (ICBId_SlaveType)id;
  icb_slave_type const *slave = NULL;

  for( uint32_t idx = 0; idx < sys_info->num_slaves; idx++ )
  {
    if( sys_info->slaves[idx]->soc_data.slaveId == slave_id )
    {
      slave = sys_info->slaves[idx];
      break;
    }
  }

  return slave;
}

static void set_defaults
(
  void
)
{
  for( uint32_t i = 0; i < sys_info->num_slaves; i++ )
  {
    icb_slave_def_type const *slave = &sys_info->slaves[i]->soc_data;

    /* Skip, if there are no defaults for this slave. */
    if( !slave->pDefaults )
      continue;

    switch( slave->pBus->busType )
    {
      case ICB_BUS_BIMC:
      {
        icb_bimc_slave_defaults_type *defs = (icb_bimc_slave_defaults_type *)&slave->pDefaults;
        HAL_bimc_InfoType *info = (HAL_bimc_InfoType *)slave->pBus->pInfo;

        /* In the case of multiple ports per slave, program them both. */
        for( uint32_t port = 0; port < slave->uNumPorts; port++ )
        {
          HAL_bimc_ArbitrationEnable( info,
                                      slave->pPortIds[port],
                                      defs->bArbEn );
        }
        break;
      }
      case ICB_BUS_NOC:
      case ICB_BUS_VIRT:
        /* No defaults to set. */
        break;
      default:
        /* Error in the data file. */
        CORE_VERIFY( 0 );
        break;
    }
  }
}

static void update_slave_clocks
(
  icb_slave_type     const *slave,
  icb_slave_req_type        current_request,
  icb_slave_req_type        next_request
)
{
  if( current_request != next_request )
  {
    uint64_t scaled_request;
    const icb_bus_def_type *pBus = slave->soc_data.pBus;
    
    /* Adjust the request to reflect our aggregation scheme. */
    switch(pBus->aggType)
    {
      case ICB_AGG_SCHEME_0:
        scaled_request = CEILING_DIV(next_request * pBus->uP0, 100);
        break;
      case ICB_AGG_SCHEME_LEGACY:
      default:
        scaled_request = next_request;
        break;
    }

    /* Calculate the clock frequency required for this slave */
    uint32_t clk_req = CLK_DIV( scaled_request,
                                slave->slave_width,
                                slave->soc_data.uNumAggPorts );

    /* Update bus clock speed. */
    icb_clock_request( slave->bus_clk, clk_req );

    /* Update slave clock speeds. */
    for( uint32_t clk_idx = 0; clk_idx < slave->num_slave_clks; clk_idx++ )
    {
      icb_clock_request( &slave->slave_clks[clk_idx], clk_req );
    }
  }
}

void icb_slaves_translate_cb
(
  rpm_translation_info *info
)
{
  /* Try to find the slave that corresponds with the incoming id. */
  icb_slave_type const *slave = get_slave( info->id );

  /* If found, translate. */
  if( slave )
  {
    unsigned            type, length;
    const char         *value;
    icb_slave_req_type *req = info->dest_buffer;

    /* Get the only kvp we expect, "bw".
    ** Make sure it's what we expect.
    */
    if( kvp_get( info->new_kvps, &type, &length, &value ) &&
        ICB_SLAVE_FIELD_BW == type)
    {
      /* Make sure the payload is the right length. */
      if( sizeof(icb_slave_req_type) == length )
      {
        /* No translation required, just copy it. */
        *req = *((icb_slave_req_type *)value);
        SWEVENT(BUS_EVENT_SLAVE_XLATE, info->id, HIGH(*req), LOW(*req));
      }
      /* If the length is zero, this is an invalidate. */
      else if( 0 == length )
      {
        *req = 0;
        SWEVENT(BUS_EVENT_SLAVE_XLATE, info->id, HIGH(*req), LOW(*req));
      }
    }
  }
}

void icb_slaves_apply_cb
(
 rpm_application_info *info
)
{
  /* Try to find the slave that corresponds with the incoming id. */
  icb_slave_type const *slave = get_slave( info->id );

  /* If found, aggregate and apply. */
  if( slave )
  {
    icb_slave_req_type *current = (icb_slave_req_type *)info->current_aggregation;
    icb_slave_req_type *new, *old;

    /* If we don't have an old state, allocate a bit on the stack and clear it. */
    if( !info->old_state )
    {
      old = alloca( sizeof(icb_slave_req_type) );
      memset( old, 0, sizeof(icb_slave_req_type) );
    }
    /* Otherwise, use what we're given. */
    else
    {
      old = (icb_slave_req_type *)info->old_state;
    }

    /* If we don't have an new state, allocate a bit on the stack and clear it. */
    if( !info->new_state )
    {
      new = alloca( sizeof(icb_slave_req_type) );
      memset( new, 0, sizeof(icb_slave_req_type) );
    }
    /* Otherwise, use what we're given. */
    else
    {
      new = (icb_slave_req_type *)info->new_state;
    }

    /* Compute the difference. */
    icb_slave_req_type  aggregate = *current - *old + *new ;

    /* Update slave clocks. */
    SWEVENT(BUS_EVENT_SLAVE_APPLY, info->id, HIGH(aggregate), LOW(aggregate));
    update_slave_clocks( slave, *current, aggregate );

    /* Save updated request. */
    memcpy( current, &aggregate, sizeof(icb_slave_req_type) );

    /* Forward this update to the master latency module if necessary */
    if( slave->soc_data.bMonitored )
    {
      icb_master_latency_update_bw( slave->soc_data.slaveId, *current );
    }
  }
}

/**
@copydoc icb_slaves_init()
*/
void icb_slaves_init( void )
{
  /* Get a pointer to the system resources. */
  CORE_VERIFY_PTR( sys_info = icb_get_target_resources() );

  /* If there aren't any shared slaves, bail. */
  if( !sys_info->num_slaves )
  {
    return;
  }
  /* Set slave defaults. */
  set_defaults();

  /* Register with the RPM driver. */
  rpm_register_named_resource( RPM_BUS_SLAVE_REQ,
                               sys_info->num_slaves,
                               sys_info->slave_ids,
                               sizeof(icb_slave_req_type),
                               icb_slaves_translate_cb,
                               icb_slaves_apply_cb,
                               NULL );
}
