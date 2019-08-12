/**

@file   icb_rpm_dsat.c

@brief  Implementation of icb dsat management module.
*/
/*
===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/buses/icb/src/common/icb_rpm_dsat.c#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
2014/08/04   sds     Created 

===========================================================================
         Copyright (c) 2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                          QUALCOMM Proprietary
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "CoreVerify.h"
#include "icb_rpmi.h"
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
uint64_t search_freq_table
(
  icb_dsat_context_type *context,
  uint32_t               freq_khz
)
{
  uint64_t bw = 0;
  uint32_t freq_mhz = freq_khz / 1000;
  const icb_dsat_freq_bw_type *levels = context->levels;

  uint32_t idx;
  for( idx = 0; idx < context->num_levels; idx++ )
  {
    /* Search for a match. */
    if( freq_mhz <= levels[idx].freq )
    {
      /* Convert to bytes/sec. */
      bw = levels[idx].bw * 1000000ULL;
      break;
    }
  }

  /* If we fell off the top, use the highest. */
  if( idx >= context->num_levels )
  {
    bw = levels[context->num_levels - 1].bw * 1000000ULL;
  }

  return bw;
}

void update_dsat_context
(
  icb_dsat_context_type *context,
  uint32_t               pre_freq,
  uint32_t               post_freq,
  bool                   force
)
{
  /* Find the bandwidth for this frequency. */
  uint64_t pre_bw  = search_freq_table(context, pre_freq);
  uint64_t post_bw = search_freq_table(context, post_freq);

  /* Only update if the new value is different than the old or we're forcing the update */
  if( force || pre_bw != post_bw )
  {
    /* If the new limit to set is non-zero, set it. */
    if( 0 != post_bw )
    {
      HAL_bimc_QosBandwidthType qos_bw = 
      {
        .uBandwidth     = post_bw,
        .uWindowSize    = context->gp,
        .uThresholdHigh = post_bw
      };

      HAL_bimc_SetDangerBandwidth( context->info, context->port, context->context, &qos_bw );

      /* If we were off before or forcing, turn the limit on and set the qos policy. */
      if( force || 0 == pre_bw )
      {
        HAL_bimc_QosPolicyType policy = 
        {
          .uPolicyMask = 0xFFFFFFFF,
          /** Assumes 1:1 mapping on the adjusted context. */
          .uPolicySel  = 1 << context->context
        };

        HAL_bimc_SetDangerMode( context->info, context->port, context->context, true );
        HAL_bimc_SetDangerPolicy( context->info, context->port, context->context, &policy );
      }
    }
    /* We're about to set the limit to zero, meaning disable. */
    else
    {
      HAL_bimc_QosPolicyType policy = 
      {
        .uPolicyMask = 0xFFFFFFFF,
        .uPolicySel  = 0
      };

      /* Clear the danger policy selection for the context. */
      HAL_bimc_SetDangerPolicy( context->info, context->port, context->context, &policy );
      HAL_bimc_SetDangerMode( context->info, context->port, context->context, false );
    }
  }
}

void icb_dsat_callback
(
  ClockPrePostType event,
  uint32           pre_freq,
  uint32           post_freq,
  void *           cb_data
)
{
  uint32_t idx = (uint32_t)cb_data;
  ClockIdType clock_id = sys_info->dsat_contexts[idx]->clock_id;
  for( ; 
       idx < sys_info->num_dsat_contexts &&
       sys_info->dsat_contexts[idx]->clock_id == clock_id;
       idx++ )
  {
    icb_dsat_context_type *context = sys_info->dsat_contexts[idx];

    /* If we are before the clock switch and
    * the previous frequency is greater than what we're switching to
    * update the danger context now. */
    if( CLOCK_PRE_CALLBACK == event && pre_freq > post_freq )
    {
      update_dsat_context( context, pre_freq, post_freq, false );
    }
    /* If we are after the clock switch and
    * the previous frequency is less than what we're switching to
    * update the danger context now. */
    else if( CLOCK_POST_CALLBACK == event && pre_freq < post_freq )
    {
      update_dsat_context( context, pre_freq, post_freq, false );
    }
  }
}

void init_dsat_context
(
  icb_dsat_context_type *context
)
{
  uint32 freq;
  HAL_bimc_QosPriorityType priority =
  {
    .regulator.health0.bLimitCommands = true
  };

  /* Set the limiter configuration. */
  HAL_bimc_SetDangerPriority( context->info, context->port, context->context, &priority );

  /* Get the current clock frequency, and update the bandwidth limit. */
  Clock_GetClockFrequency( context->clock_id, &freq );
  update_dsat_context( context, 0, freq/1000, true );
}

/**
@copydoc icb_dsat_init()
*/
void icb_dsat_init( void )
{
  /* Get a pointer to the system resources. */
  CORE_VERIFY( sys_info = icb_get_target_resources() );

  /* If there aren't any dsat contexts, bail. */
  if( !sys_info->num_dsat_contexts )
  {
    return;
  }

  /* Fetch clock ids for each context. */
  for( uint32_t idx = 0; idx < sys_info->num_dsat_contexts; idx++ )
  {
    icb_dsat_context_type *context = sys_info->dsat_contexts[idx];
    CORE_DAL_VERIFY( Clock_GetClockId( context->clock, &context->clock_id ) );
    init_dsat_context( context );
  }

  /* Register callbacks for each clock. */
  ClockIdType id = 0;
  for( uint32_t idx = 0; idx < sys_info->num_dsat_contexts; idx++ )
  {
    icb_dsat_context_type *context = sys_info->dsat_contexts[idx];

    /* Only register one callback per clock. */
    if( id != context->clock_id )
    {
      id = context->clock_id;
      CORE_DAL_VERIFY( Clock_RegisterCallback( context->clock_id,
                                              &context->handle,
                                               icb_dsat_callback,
                                               (void *)idx ) );
    }
  }
}
