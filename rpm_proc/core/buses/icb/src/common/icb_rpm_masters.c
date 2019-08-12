/**

@file   icb_rpm_masters.c

@brief  Implementation of icb masters aggregator.
*/
/*
===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/buses/icb/src/common/icb_rpm_masters.c#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
2014/10/02   sds     Add number of ports for aggregation purposes to masters/slaves.
2014/08/21   sds     Prevent div by 0 when calculating clocks.
2014/01/15   sds     Remove unused threshold keys, and support threshold percentages.
2013/11/25   sds     Add support for configurable aggregation schemes.
2013/10/10   sds     Remove clock gating settings - hardware defaults are correct.
2013/06/06   sds     Fix typo in clock request code
2013/03/18   sds     Only update QoS when in regulator/limiter mode
2013/03/14   sds     Update hardware before changing the clock when we're
                     about to request zero.
2013/02/20   sds     Handle full resource invalidate.
2012/08/17   sds     Handle initial requests and invalidates correctly.
2012/01/20   sds     Updated with RPM server API calls.
2011/11/08   sds     Created 

===========================================================================
         Copyright (c) 2011-2014 QUALCOMM Technologies Incorporated.
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
#define THRESHOLD(bw, pct) ((pct)?CEILING_DIV((bw)*(pct), 100):0)

/* -----------------------------------------------------------------------
** Data
** ----------------------------------------------------------------------- */
static icb_system_info_type *sys_info; /**< Pointer to the system info. */

/* -----------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */
static icb_master_type const *get_master
(
  unsigned id
)
{
  ICBId_MasterType master_id    = (ICBId_MasterType)id;
  icb_master_type const *master = NULL;

  for( uint32_t idx = 0; idx < sys_info->num_masters; idx++ )
  {
    if( sys_info->masters[idx]->soc_data.masterId == master_id )
    {
      master = sys_info->masters[idx];
      break;
    }
  }

  return master;
}

static void update_master_clocks
(
  icb_master_type const *master,
  uint64_t               current_request,
  uint64_t               next_request
)
{
  if( current_request != next_request )
  {
    uint64_t scaled_request;
    const icb_bus_def_type *pBus = master->soc_data.pBus;
    
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

    /* Calculate the clock frequency required for this master */
    uint32_t clk_req = CLK_DIV( scaled_request,
                                master->master_width,
                                master->soc_data.uNumAggPorts );

    /* Update bus clock speed. */
    icb_clock_request( master->bus_clk, clk_req );

    /* Update master clock speeds. */
    for( uint32_t clk_idx = 0; clk_idx < master->num_master_clks; clk_idx++ )
    {
      icb_clock_request( &master->master_clks[clk_idx], clk_req );
    }

  }
}

static void update_master
(
  icb_master_type        const *master,
  icb_master_bw_req_type       current_request,
  icb_master_bw_req_type       next_request
)
{
  /* Update clock request now if the new request is non-zero. */
  if( next_request )
  {
    update_master_clocks( master, current_request, next_request );
  }

  /* Update master QoS hardware, if necessary. */
  if( master->soc_data.pQosPortIds && current_request != next_request )
  {
    icb_master_def_type const *master_defs = &master->soc_data;

    /* Split bandwidth evenly across multiple ports. */
    uint32_t ports = master_defs->uNumPorts;

    switch( master->soc_data.pBus->busType )
    {
      case ICB_BUS_BIMC:
      {
        icb_bimc_master_defaults_type *defs = (icb_bimc_master_defaults_type *)master_defs->pDefaults;

        /* Only update this if we're in the proper mode. */
        if( BIMC_QOS_MODE_REGULATOR == defs->bimcMode ||
            BIMC_QOS_MODE_LIMITER   == defs->bimcMode )
        {
          HAL_bimc_InfoType *info = (HAL_bimc_InfoType *)master->soc_data.pBus->pInfo;
          uint64_t bw = CEILING_DIV(next_request, ports);
          HAL_bimc_QosBandwidthType qos_bw = 
          {
            .uBandwidth       = bw,
            .uWindowSize      = master_defs->uWindowSize,
            .uThresholdHigh   = THRESHOLD( bw, defs->bimcBandwidth.uThresholdHighPct ),
            .uThresholdMedium = THRESHOLD( bw, defs->bimcBandwidth.uThresholdMedPct ),
            .uThresholdLow    = THRESHOLD( bw, defs->bimcBandwidth.uThresholdLowPct ),
          };
        
          for( uint32_t idx = 0; idx < ports; idx++ )
          {
            HAL_bimc_SetQosBandwidth( info,
                                      master->soc_data.pQosPortIds[idx],
                                      &qos_bw );
          }
        }
        break;
      }
      case ICB_BUS_NOC:
      {
        icb_noc_master_defaults_type *defs = (icb_noc_master_defaults_type *)master_defs->pDefaults;

        /* Only update this if we're in the proper mode. */
        if( NOC_QOS_MODE_REGULATOR == defs->nocMode ||
            NOC_QOS_MODE_LIMITER   == defs->nocMode )
        {
          HAL_noc_InfoType *info = (HAL_noc_InfoType *)master->soc_data.pBus->pInfo;
          HAL_noc_QosBandwidthType qos_bw = 
          {
            .uBandwidth       = CEILING_DIV(next_request, ports),
            .uWindowSize      = master->soc_data.uWindowSize
          };
        
          for( uint32_t idx = 0; idx < ports; idx++ )
          {
            HAL_noc_SetQosBandwidth( info,
                                     master->soc_data.pQosPortIds[idx],
                                    &qos_bw );
          }
        }
        break;
      }
      case ICB_BUS_VIRT:
        /* Nothing to do. */
        break;
      default:
        /* Something is wrong here. */
        break;
    }
  }

  /* Update clock request now if the new request is zero. */
  if( !next_request )
  {
    update_master_clocks( master, current_request, next_request );
  }

}

static void set_defaults
(
  void
)
{
  for( uint32_t i = 0; i < sys_info->num_masters; i++ )
  {
    icb_master_type     const *master      = sys_info->masters[i];
    icb_master_def_type const *master_defs = &master->soc_data;

    /* Skip, if there are no defaults for this master or if it has no QoS ports. */
    if( !master_defs->pQosPortIds || !master_defs->pDefaults )
      continue;

    switch( master_defs->pBus->busType )
    {
      case ICB_BUS_BIMC:
      {
        icb_bimc_master_defaults_type *defs = (icb_bimc_master_defaults_type *)master_defs->pDefaults;
        HAL_bimc_InfoType *info = (HAL_bimc_InfoType *)master_defs->pBus->pInfo;

        /* In the case of multiple ports per master, program them both. */
        for( uint32_t port = 0; port < master_defs->uNumPorts; port++ )
        {
          /* If we're not in bypass, we have more to do. */
          if( BIMC_QOS_MODE_BYPASS != defs->bimcMode )
          {
            /* Set priority */
            HAL_bimc_SetQosPriority(  info,
                                      master_defs->pQosPortIds[port],
                                      defs->bimcMode,
                                     &defs->bimcPriority );

            /* If we're not in fixed, we have more to do. */
            if( BIMC_QOS_MODE_FIXED != defs->bimcMode )
            {
              uint64_t bw = CEILING_DIV(defs->bimcBandwidth.uBandwidth, master_defs->uNumPorts);
              HAL_bimc_QosBandwidthType qos_bw = 
              {
                .uBandwidth       = bw,
                .uWindowSize      = master_defs->uWindowSize,
                .uThresholdHigh   = THRESHOLD( bw, defs->bimcBandwidth.uThresholdHighPct ),
                .uThresholdMedium = THRESHOLD( bw, defs->bimcBandwidth.uThresholdMedPct ),
                .uThresholdLow    = THRESHOLD( bw, defs->bimcBandwidth.uThresholdLowPct ),
              };

              /* Set bandwidth. */
              HAL_bimc_SetQosBandwidth( info,
                                        master_defs->pQosPortIds[port],
                                        &qos_bw );
            }
          }

          /* Program mode. */
          HAL_bimc_SetQosMode( info,
                               master_defs->pQosPortIds[port],
                               defs->bimcMode );

        }
        break;
      }
      case ICB_BUS_NOC:
      {
        icb_noc_master_defaults_type *defs = (icb_noc_master_defaults_type *)master_defs->pDefaults;
        HAL_noc_InfoType *info = (HAL_noc_InfoType *)master_defs->pBus->pInfo;

        /* In the case of multiple ports per master, program them both. */
        for( uint32_t port = 0; port < master_defs->uNumPorts; port++ )
        {
          /* If we're not in bypass, we have more to do. */
          if( NOC_QOS_MODE_BYPASS != defs->nocMode )
          {
            /* Set priority */
            HAL_noc_SetQosPriority(  info,
                                     master_defs->pQosPortIds[port],
                                     defs->nocMode,
                                    &defs->nocPriority );

            /* If we're not in fixed, we have more to do. */
            if( NOC_QOS_MODE_FIXED != defs->nocMode )
            {
              /* Set bandwidth. */
              HAL_noc_SetQosBandwidth(  info,
                                        master_defs->pQosPortIds[port],
                                       &defs->nocBandwidth );
            }

          }

          /* Program mode. */
          HAL_noc_SetQosMode(  info,
                               master_defs->pQosPortIds[port],
                               defs->nocMode );
        }
        break;
      }
      case ICB_BUS_VIRT:
        /* No defaults for virtual bus. */
        break;
      default:
        /* Unknown master type. */
        CORE_VERIFY( 0 );
        break;
    }
  }
}

void icb_masters_translate_cb
(
  rpm_translation_info *info
)
{
  /* Try to find the master that corresponds with the incoming id. */
  icb_master_type const *master = get_master( info->id );

  /* If found, translate. */
  if( master )
  {
    unsigned                type, length;
    const char             *value;
    uint64_t                bw;
    icb_master_bw_req_type *req = info->dest_buffer;

    /* Read until we run out of KVPs. */
    while( !kvp_eof(info->new_kvps) )
    {
      /* Make sure it's one of the kvps we expect.  */
      if( kvp_get( info->new_kvps, &type, &length, &value ) )
      {
        /* Validate the length of the buffer. */
        if( sizeof(uint64_t) == length )
        {
          bw = *((uint64_t *)value);
        }
        /* If the length is zero, this is an invalidate.
        ** Zero this buffer. */
        else if( 0 == length )
        {
          bw = 0;
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
          case ICB_MASTER_FIELD_BW:
            *req = bw;
            SWEVENT(BUS_EVENT_MASTER_XLATE, info->id, HIGH(bw), LOW(bw));
            break;
          default:
            /* Invalid key, ignore. */
            break;
        }
      }
    }
  }
}

void icb_masters_apply_cb
(
 rpm_application_info *info
)
{
  /* Try to find the master that corresponds with the incoming id. */
  icb_master_type const *master = get_master( info->id );

  /* If found, aggregate and apply. */
  if( master )
  {
    icb_master_bw_req_type *current = (icb_master_bw_req_type *)info->current_aggregation;
    icb_master_bw_req_type *new, *old;

    /* If we don't have an old state, allocate a bit on the stack and clear it. */
    if( !info->old_state )
    {
      old = alloca( sizeof(icb_master_bw_req_type) );
      memset( old, 0, sizeof(icb_master_bw_req_type) );
    }
    /* Otherwise, use what we're given. */
    else
    {
      old = (icb_master_bw_req_type *)info->old_state;
    }

    /* If we don't have an new state, allocate a bit on the stack and clear it. */
    if( !info->new_state )
    {
      new = alloca( sizeof(icb_master_bw_req_type) );
      memset( new, 0, sizeof(icb_master_bw_req_type) );
    }
    /* Otherwise, use what we're given. */
    else
    {
      new = (icb_master_bw_req_type *)info->new_state;
    }

    /* Update current aggregation with new values. */
    icb_master_bw_req_type aggregate = *current - *old + *new;

    /* Update the master clocks and hardware. */
    SWEVENT(BUS_EVENT_MASTER_APPLY, info->id, HIGH(aggregate), LOW(aggregate));
    update_master( master, *current, aggregate );

    /* Save aggregate request. */
    memcpy( current, &aggregate, sizeof(icb_master_bw_req_type) );
  }
}

/**
@copydoc icb_masters_init()
*/
void icb_masters_init( void )
{
  /* Get a pointer to the system resources. */
  CORE_VERIFY( sys_info = icb_get_target_resources() );

  /* If there aren't any shared masters, bail. */
  if( !sys_info->num_masters )
  {
    return;
  }

  /* Initialize defaults. */
  set_defaults();

  /* Register bandwidth resources with the RPM driver. */
  rpm_register_named_resource( RPM_BUS_MASTER_REQ,
                               sys_info->num_masters,
                               sys_info->master_ids,
                               sizeof(icb_master_bw_req_type),
                               icb_masters_translate_cb,
                               icb_masters_apply_cb,
                               NULL );
}
