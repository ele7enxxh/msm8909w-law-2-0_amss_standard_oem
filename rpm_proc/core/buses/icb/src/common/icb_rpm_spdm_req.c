/**

@file   icb_rpm_spdm_req.c

@brief  Implementation of icb spdm clock request aggregator.
*/
/*
===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/buses/icb/src/common/icb_rpm_spdm_req.c#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
2014/12/09   sds     Fix handling of simultaneous RT/OLEM requests
2014/06/06   sds     Added bspc:1 for SPDM RT clocks request
2012/09/11   sds     Created 

===========================================================================
             Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
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
#include "Clock.h"
#include "swevent.h"

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
typedef enum
{
  SPDM_OFFLINE   = 0,
  SPDM_RT        = 1,
  SPDM_NUM_TYPES,
} icb_spdm_desc_type;

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Data
** ----------------------------------------------------------------------- */
static icb_system_info_type *sys_info; /**< Pointer to the system information. */
static bool                  enabled[SPDM_NUM_TYPES];  /**< Current state for our clock request. */

/* -----------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */
/**
@brief Translate our clock strings to clock regime ids.

@pre Expects system info to be determined.

@returns true if successful, false otherwise.
*/
bool get_spdm_clock_ids( void )
{
  bool retval = true;

  /* Offline monitor clocks */
  for( unsigned i = 0;
       i < sys_info->num_olem_clks;
       i++ )
  {
    /* If a lookup fails, bail. */
    if( DAL_SUCCESS != Clock_GetClockId(  sys_info->olem_clks[i],
                                         &sys_info->olem_clk_ids[i] ) )
    {
      retval = false;
      break;
    }
  }

  /* Realtime monitor clocks */
  for( unsigned i = 0;
       i < sys_info->num_rtem_clks;
       i++ )
  {
    /* If a lookup fails, bail. */
    if( DAL_SUCCESS != Clock_GetClockId(  sys_info->rtem_clks[i],
                                         &sys_info->rtem_clk_ids[i] ) )
    {
      retval = false;
      break;
    }
  }
  return retval;
}

/**
@brief Enable/Disable SPDM clocks.
*/
void request_spdm_clocks
(
  unsigned id,
  bool     enable
)
{
  /* Range check incoming id. */
  if( id >= SPDM_NUM_TYPES )
  {
    return;
  }

  /* If we're already at the requested state, ignore. */
  if( enable != enabled[id] )
  {
    DALResult (*clk_fcn)(ClockIdType) = enable?
      Clock_EnableClock: Clock_DisableClock;
    uint32_t     num_clks;
    ClockIdType *clk_ids;

    switch(id)
    {
      case SPDM_OFFLINE:
        num_clks = sys_info->num_olem_clks;
        clk_ids = sys_info->olem_clk_ids;
        break;
      case SPDM_RT:
        num_clks = sys_info->num_rtem_clks;
        clk_ids = sys_info->rtem_clk_ids;
        break;
      default:
        return;
    }

    /* Toggle all clocks associated with this resource */
    for( unsigned i = 0;
        i < num_clks;
        i++ )
    {
      CORE_DAL_VERIFY( clk_fcn( clk_ids[i] ) );
    }

    /* Update our clock state. */
    enabled[id] = enable;
    SWEVENT(BUS_EVENT_SPDM_STATE, enabled);
  }
}

void icb_spdm_req_translate_cb
(
  rpm_translation_info *info
)
{
  unsigned               type, length;
  const char *           value;
  icb_spdm_clk_req_type *req = info->dest_buffer;

  /* Get the only kvp we expect, "en".
  ** Make sure it's what we expect.
  */
  if( kvp_get( info->new_kvps, &type, &length, &value ) &&
      ICB_SPDM_CLK_FIELD_EN == type )
  {
    /* Make sure the payload is the right length. */
    if( sizeof(icb_spdm_clk_req_type) == length )
    {
      /* Any non-zero value for 'en' means enable. */
      *req = *((icb_spdm_clk_req_type *)value)? 1: 0;
      SWEVENT(BUS_EVENT_SPDM_XLATE, *req);
    }
    /* If the length is zero, this is an invalidate. */
    else if( 0 == length )
    {
      *req = 0;
      SWEVENT(BUS_EVENT_SPDM_XLATE, *req);
    }
  }
}

void icb_spdm_req_apply_cb
(
 rpm_application_info *info
)
{
  icb_spdm_clk_req_type *current = (icb_spdm_clk_req_type *)info->current_aggregation;
  icb_spdm_clk_req_type *new     = (icb_spdm_clk_req_type *)info->new_state;
  icb_spdm_clk_req_type *old;

  /* If we don't have an old state, allocate a bit on the stack and clear it. */
  if( !info->old_state )
  {
    old = alloca( sizeof(icb_spdm_clk_req_type) );
    memset( old, 0, sizeof(icb_spdm_clk_req_type) );
  }
  /* Otherwise, use what we're given. */
  else
  {
    old = (icb_spdm_clk_req_type *)info->old_state;
  }

  /* Compute the difference */
  icb_spdm_clk_req_type  aggregate = *current - *old + *new ;

  /* Request for SPDM clock to the desired state.  */
  SWEVENT(BUS_EVENT_SPDM_APPLY, aggregate);
  request_spdm_clocks( info->id, aggregate != 0 );

  /* Save the request. */
  memcpy( current, &aggregate, sizeof(icb_spdm_clk_req_type) );
}

/**
@copydoc icb_spdm_req_init()
*/
void icb_spdm_req_init( void )
{
  /* Get a pointer to the system resources. */
  CORE_VERIFY_PTR( sys_info = icb_get_target_resources() );

  /* Clear our cached clock state. */
  memset(enabled, 0, sizeof(enabled));

  /* If there aren't any offline monitor clocks, bail. */
  if( !sys_info->num_olem_clks )
  {
    return;
  }

  /* Get corresponding clock ids from clock regime */
  CORE_VERIFY( get_spdm_clock_ids() );

  /* Register with the RPM driver. */
  rpm_register_resource( RPM_BUS_SPDM_CLK_REQ,
                         2,
                         sizeof(icb_spdm_clk_req_type),
                         icb_spdm_req_translate_cb,
                         icb_spdm_req_apply_cb,
                         NULL );
}
