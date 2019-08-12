/*=============================================================================

  FILE:            CLpr.c
  
  OVERVIEW:        Provides the CLpr class implementation for offline synthesis.
 
                   Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
                   All Rights Reserved.
                   Qualcomm Confidential and Proprietary

  $Header: //source/qcom/qct/core/power/sleep/rel/1h10/src/common/compiler/offline_synthesis/CLpr.cpp

=============================================================================*/

#include <limits.h>
#include <stdlib.h>
#include "CLprm.h"
#include "sleepi.h"
#include "CoreVerify.h"
#include "sleep_log.h"
#include "sleep_lpri.h"
#include "SleepLPR_synth_modes.h"


/*=============================================================================

                       CLASS MEMBER FUNCTION DEFINITIONS

 =============================================================================*/

/*
 * CLpr_initSynth
 */
void CLpr_initSynth(CLpr *hCLpr, sleep_lpr_synth *lpr)
{
  CORE_VERIFY_PTR(hCLpr);

  hCLpr->m_registry = NULL;
  hCLpr->enabled_mode_count = 0;
  hCLpr->synth_lpr = lpr;

  memset( hCLpr->m_modes, 0x0, 
          sizeof( CLprm * ) * SLEEP_LPR_NUM_SYNTHESIZED_MODES );
}

/*
 * createSynth
 */
CLpr* CLpr_createSynth( sleep_lpr_synth *lpr )
{
  CLpr *self;
  unsigned int i;
  sleep_lprm_synth *lprm;
  CLprm *mode;

  CORE_VERIFY( lpr->mode_count == SLEEP_LPR_NUM_SYNTHESIZED_MODES );

  self = malloc( sizeof(CLpr) );
  CORE_VERIFY( self );
  CLpr_initSynth( self, lpr );

  for(i = 0; i < lpr->mode_count; i++)
  {
    lprm = &(lpr->modes[i]);

    mode = CLprm_createSynth( lprm );
    CLprm_setParent( mode, self );
    self->m_modes[i] = mode;
  }

  CORE_VERIFY( 0 != lpr->sharing_cores );
  self->sharing_cores = lpr->sharing_cores ;

  return self;
}

/*
 * ~CLpr (Destructor)
 */
void CLpr_deinit()
{
}

/*
 * getStatus
 */
sleep_status CLpr_getStatus( CLpr *hCLpr )
{
  CORE_VERIFY_PTR(hCLpr);

  if( hCLpr->enabled_mode_count > 0 )
  {
    return SLEEP_ENABLED;
  }
  else
  {
    return SLEEP_DISABLED;
  }
}

/*
 * setRegistry
 */
void CLpr_setRegistry( CLpr *hCLpr, CLprRegistry* registry )
{
  CORE_VERIFY_PTR(hCLpr);

  hCLpr->m_registry = registry;
}

/*
 * modeUpdated
 */
void CLpr_modeUpdated( CLpr *hCLpr, CLprm* mode)
{
  unsigned int old_enabled_count;

  CORE_VERIFY_PTR(hCLpr);

  old_enabled_count = hCLpr->enabled_mode_count;

  if( SLEEP_ENABLED == CLprm_getStatus( mode ) )
  {
    hCLpr->enabled_mode_count++;
  }
  else
  {
    hCLpr->enabled_mode_count--;
  }

  /* Confirm that the number of enabled modes did not go negative, or
   * exceed the total number of modes. */
  CORE_VERIFY( hCLpr->enabled_mode_count <= SLEEP_LPR_NUM_SYNTHESIZED_MODES );
  
  /* Update the registry if this CLpr has gone from enabled to disabled,
   * or from disabled to enabled. */
  if( old_enabled_count == 0 || hCLpr->enabled_mode_count == 0 )
  {
    CLprRegistry_lprUpdated( hCLpr->m_registry, hCLpr );
  }

  /* Update the registry with the new minimum enabled latency mode. */
  CLprRegistry_updateMinLatencyEnabledMode( hCLpr->m_registry, hCLpr, mode );
}

