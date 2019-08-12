/*==============================================================================
  FILE:         CLprRegistry.c

  OVERVIEW:     Implementation of Sleep Registry which will keep track of all
                registered LPRs and their (enabled) modes.

  DEPENDENCIES: None

                Copyright (c) 2010-2014 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/compiler/CLprRegistry.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include <stdint.h>
#include "CLprRegistry.h"
#include "sleep_lpr.h"
#include "sleep_lpri.h"
#include "sleepi.h"
#include "CoreVerify.h"
#include "sleep_log.h"
#include "sleep_config.h"
#include "sleep_idle_plugin.h"
#include "lookup_table_solver.h"


/* ==================================================================
                        GLOBAL VARIABLES
   ================================================================== */

/**
 * @brief Handle to the /sleep/idle/plugin resource, used to
 * swap the idle entry point based on whether modes are enabled
 * or not. */
static npa_client_handle sleep_idle_plugin_client_handle;

/**
 * @brief Handle to the /sleep/idle/plugin resource, used to
 * inform the node of the minimum enabled mode latency, so it
 * can decide whether to switch into SWFI-only or not. */
static npa_client_handle sleep_idle_plugin_mode_latency_client;


/* ==================================================================
                     FUNCTION DEFINITION
   ================================================================== */
/*
 * CLprRegistry (Constructor)
 */
void CLprRegistry_init( CLprRegistry *hCLprReg )
{
  unsigned int nCore;

  CORE_VERIFY_PTR(hCLprReg);
  for(nCore = 0; nCore < SLEEP_NUM_CORES; nCore++)
  {
    hCLprReg->m_lprs[nCore]     = NULL;    
    hCLprReg->m_minLprm[nCore]  = NULL;
  }

  /* Get a client handle to the /sleep/idle/plugin node */
  CORE_VERIFY_PTR( sleep_idle_plugin_client_handle = npa_create_sync_client(
                                                     "/sleep/idle/plugin",
                                                     "SleepRegistryClient",
                                                     NPA_CLIENT_REQUIRED ) );

  /* No modes are enabled at this point, so set initial sleep function to 
   * SWFI only
   */
  npa_issue_required_request( sleep_idle_plugin_client_handle,
                              SLEEP_IDLE_PLUGIN_SWFI_ONLY );

  CORE_VERIFY_PTR( sleep_idle_plugin_mode_latency_client =
                         npa_create_sync_client(
                                 "/sleep/idle/plugin",
                                 "SleepRegistryModeLatencyClient",
                                 SLEEP_IDLE_PLUGIN_CLIENT_MIN_MODE_LATENCY ) );
}

/*
 * CLprRegistry (Destructor)
 */
void CLprRegistry_deinit( CLprRegistry *hCLprReg )
{
}

/*
 * readGlobalConfig
 */
void CLprRegistry_readGlobalConfig( CLprRegistry *hCLprReg, sleep_lpr *lpr)
{
  unsigned char disable = FALSE;
  unsigned int i;
  
  CORE_LOG_VERIFY_PTR(lpr, sleep_log_printf(SLEEP_LOG_LEVEL_ERROR, 
                      0, "ERROR (message: \"NULL Lpr\")"));

  /* Check to see if this LPR has been disabled in the global
   * config settings. */
  sleep_config_read_bool( 0, ( char * )( lpr->resource_name ), NULL, 
                          (char *)"disable", &disable );

  if( TRUE == disable )
  {
    sleep_log_printf( SLEEP_LOG_LEVEL_WARNING, 1, 
                      "WARNING (message: \"Disabled in the global config\")"
                      " (LPR Name: \"%s\")", lpr->resource_name );
  }
  
  /* Loop through the modes that are being registered here */
  for( i = 0; i < lpr->mode_count; i++ )
  {
    sleep_lprm *lprm = &(lpr->modes[i]);
    lprm->lpr = lpr;

    /* Check to see if this mode has been disabled in the global
     * config settings.  If the entire LPR is disabled, no need to read
     * each individual LPRM. */
    if( TRUE == disable )
    {
      lprm->disabled_in_global_config = TRUE;
    }
    else
    {
      sleep_config_read_bool( 0, ( char * )( lpr->resource_name ), 
                              ( char * )( lprm->mode_name ), 
                              ( char * )"disable",
                              &( lprm->disabled_in_global_config ) );

      if( TRUE == lprm->disabled_in_global_config )
      {
        sleep_log_printf( SLEEP_LOG_LEVEL_WARNING, 2, 
                          "WARNING (message: \"Disabled in the global config\")"
                          " (LPRM Name: \"%s.%s\")",
                          lpr->resource_name, lprm->mode_name );
      }
    }
  }
}

/*
 * registerLpr
 */
void CLprRegistry_registerLpr( CLprRegistry *hCLprReg, sleep_lpr *lpr )
{
  uint32      i;
  sleep_lprm  *lprm;

  CORE_VERIFY_PTR(hCLprReg);
  CORE_LOG_VERIFY_PTR(lpr, sleep_log_printf(SLEEP_LOG_LEVEL_ERROR,
                           0, "ERROR (message: \"NULL Lpr\")"));

  /* Changing bit mask for older LPRs */
  if( SLEEP_ALL_CORES_BKCMP == lpr->sharing_cores )
  {
    lpr->sharing_cores = SLEEP_ALL_CORES;
  }

  sleep_log_printf(SLEEP_LOG_LEVEL_DEBUG, 2,
                   "Registering LPR (Name: \"%s\") (Shared Cores Mask: 0x%08x)",
                   lpr->resource_name, lpr->sharing_cores);

  /* Loop through the modes that are being registered here */
  for( i = 0; i < lpr->mode_count; i++ )
  {
    lprm = &(lpr->modes[i]);
    lprm->lpr = lpr;

    /* NBG: Does not look a right place or function name */
    CLprm_validate( lprm );
  }

  return;
}

/*
 * registerLpr
 */
void CLprRegistry_registerLpr_synth( CLprRegistry *hCLprReg, 
                                     sleep_lpr_synth *lpr )
{
  unsigned int sharing_cores = 0;
  CLpr* new_lpr;
  unsigned int i;

  CORE_VERIFY_PTR(hCLprReg);
  CORE_LOG_VERIFY_PTR(lpr, sleep_log_printf(SLEEP_LOG_LEVEL_ERROR, 0,
                           "ERROR (message: \"NULL Pre-Synthesized Lpr\")"));

  /* Changing bit mask for older LPRs */
  if( SLEEP_ALL_CORES_BKCMP == lpr->sharing_cores )
  {
    lpr->sharing_cores = SLEEP_ALL_CORES;
  }

  sharing_cores = lpr->sharing_cores;
  CORE_VERIFY(0 == (sharing_cores & (sharing_cores - 1)));

  sleep_log_printf(SLEEP_LOG_LEVEL_DEBUG, 2,
                   "Registering pre-synthesized LPR (Name: \"%s\")"
                   " (Sharing Cores: 0x%08x)", lpr->resource_name,
                   sharing_cores);

  /* creating a new synthesized LPR */
  new_lpr = CLpr_createSynth(lpr);
  CLpr_setRegistry( new_lpr, hCLprReg );

  /* Iterate through all cores and add to the LPR list of the relevant cores */
  for(i = 0; i < SLEEP_NUM_CORES; i++)
  {
    if( sharing_cores & SLEEP_CORE(i) )
    {
      /* Make sure there is not already a registered synthesized LPR for this
       * core.  Only one is supported per core. */
      CORE_VERIFY( NULL == hCLprReg->m_lprs[i] );
      hCLprReg->m_lprs[i] = new_lpr;      
    }
  }
}

/*
 * lprUpdated
 */
void CLprRegistry_lprUpdated( CLprRegistry *hCLprReg, CLpr* lpr )
{
  CORE_VERIFY_PTR(hCLprReg);
  CORE_VERIFY_PTR(lpr);

  if(SLEEP_ENABLED == CLpr_getStatus( lpr ))
  {
    /* Release the request on the idle plugin node, to let it go back to
     * the default of choosing the low power mode from the solver. */
    npa_complete_request( sleep_idle_plugin_client_handle );
  }
  else
  {
    /* There are no modes enabled, so request SWFI only. */    
    npa_issue_required_request( sleep_idle_plugin_client_handle,
                                SLEEP_IDLE_PLUGIN_SWFI_ONLY );
  }
}

/*
 * updateMinLatencyEnabledMode
 */
void CLprRegistry_updateMinLatencyEnabledMode( CLprRegistry *hCLprReg,
                                               CLpr *lpr, CLprm *lprm)
{
  /* Obtaining cores that will have their list affected as a result */
  unsigned int            sharing_cores;
  static uint32           last_min_enabled_mode_latency = UINT32_MAX;
  unsigned int            nCore, nSynth;
  CLprm                   *tempMinMode;
  uint32                  tempMinLatency;
  CLprm                   *mode;
  uint32                  currLatency;
  uint32                  newLatency;
  sleep_freq_lookup_table *pfreq_LUT, *pfirst_freq_LUT;

  CORE_VERIFY_PTR(hCLprReg);
  CORE_VERIFY_PTR(lpr);
  CORE_VERIFY_PTR(lprm);

  sharing_cores = CLpr_getSharingCores( lpr );

  /* Get fLUT lock while checking/updating the minLPRM data structures to
   * ensure we are not in the middle of adding a new entry */
  sleep_lookup_table_lock_fLUT();

  for( nCore = 0; nCore < SLEEP_NUM_CORES; nCore++ )
  {
    pfirst_freq_LUT = pfreq_LUT = sleep_get_freq_lut(nCore);

    if( (0 == (sharing_cores & SLEEP_CORE(nCore))) ||
        (pfreq_LUT == NULL) ||
        (hCLprReg->m_minLprm[nCore] == NULL) )
    {
      continue;
    }

    while( pfreq_LUT != NULL)
    {
      /* If the CLprm became enabled and was not previously enabled, see
       * if it's the new minimum by comparing it against the current minimum.
       * If it's not the new minimum, then there's nothing else to do. */
      if( SLEEP_ENABLED == CLprm_getStatus(lprm) )
      {
        if( FALSE == CLprm_hasConstantLatencies( lprm ) )
        {
          /* The newly-enabled mode does not have constant latencies,
           * so invalidate the minimum enabled mode. */
          hCLprReg->m_minLprm[nCore][pfreq_LUT->data_idx] = NULL;
        }
        else if( NULL == hCLprReg->m_minLprm[nCore][pfreq_LUT->data_idx] )
        {
          /* There is currently not a valid min enabled mode.  See if the
           * newly enabled mode is the only enabled mode, and if so, set
           * it to the new minimum. */
          if( 1 == lpr->enabled_mode_count )
          {
            hCLprReg->m_minLprm[nCore][pfreq_LUT->data_idx] = lprm;
          }
        }
        else
        {
          /* There is more than one mode enabled, and all of them have
           * constant latencies, so see if this new mode is the new minimum. */
          currLatency = 
            CLprm_getEnterLatency( 
                hCLprReg->m_minLprm[nCore][pfreq_LUT->data_idx], 
                0, pfreq_LUT, 0 ) + 
              CLprm_getExitLatency( 
                  hCLprReg->m_minLprm[nCore][pfreq_LUT->data_idx], 
                  0, pfreq_LUT, 0 );

          newLatency = CLprm_getEnterLatency( lprm, 0, pfreq_LUT, 0 ) + 
                       CLprm_getExitLatency( lprm, 0, pfreq_LUT, 0 );

          if( newLatency < currLatency )
          {
            /* The new mode has less latency than the current mode, so
             * set it as the new minimum latency mode. */
            hCLprReg->m_minLprm[nCore][pfreq_LUT->data_idx] = lprm;
          }
        }
      }
      else
      {
        /* The mode is newly disabled.  See if it is the current valid enabled
         * mode with minimum latency.  If not, nothing to do. */
        if( (NULL != hCLprReg->m_minLprm[nCore][pfreq_LUT->data_idx]) &&
            (hCLprReg->m_minLprm[nCore][pfreq_LUT->data_idx] != lprm) )
        {
        }
        else if( 0 == lpr->enabled_mode_count )
        {
          /* No more modes are enabled, so invalidate the min enabled mode. */
          hCLprReg->m_minLprm[nCore][pfreq_LUT->data_idx] = NULL;
        }
        else
        {
          /* A mode with variable latency got disabled or a mode with constant 
           * latency got disabled while variable latency mode is still enabled 
           * or a mode with minimum latency got disabled. In these cases, we
           * will traverse the list and find the new minimum enabled latency 
           * mode if possible. */

          tempMinMode = NULL;
          tempMinLatency = UINT32_MAX;
          
          for(nSynth = 0; nSynth < SLEEP_LPR_NUM_SYNTHESIZED_MODES; nSynth++ )
          {
            mode = lpr->m_modes[nSynth];

            if ( SLEEP_ENABLED == CLprm_getStatus( mode ) )
            {
              if ( TRUE == CLprm_hasConstantLatencies( mode ) )
              {
                /* The mode has constant latencies and hence we can still perform
                 * optimization. And since it has constant latencies, it should be
                 * ok to pass duration and cycle id as 0.
                 */
                currLatency = CLprm_getEnterLatency( mode, 0, pfreq_LUT, 0 ) + 
                              CLprm_getExitLatency( mode, 0, pfreq_LUT, 0 );
        
                if( currLatency < tempMinLatency )
                {
                  tempMinMode = mode;
                  tempMinLatency = currLatency;
                }
              }
              else
              {
                /* Variable latency mode. No need to continue searching minimum 
                 * mode in that core's mode list for this frequency. */
                tempMinMode = NULL;
                break;
              }
            }
          }
          hCLprReg->m_minLprm[nCore][pfreq_LUT->data_idx] = tempMinMode;
        }
      }
      pfreq_LUT = pfreq_LUT->next;
    }

    /* Pass the minimum latency to the sleep idle plugin,
     * so it can decide whether to do SWFI only or not.  Only do this when
     * the current minimum latency mode has changed, and modes are still 
     * enabled.  If no modes are enabled, then the registry will request SWFI
     * only, no need to make an additional request here. */ 
    if ( nCore == 0 && 0 != lpr->enabled_mode_count )
    {
      /* Use worst case (slowest frequency) for minimum latency */
      if( NULL != pfirst_freq_LUT &&
          NULL != hCLprReg->m_minLprm[nCore][pfirst_freq_LUT->data_idx] )
      {
        tempMinLatency = 
          CLprm_getEnterLatency(
              hCLprReg->m_minLprm[nCore][pfirst_freq_LUT->data_idx], 0, 
              pfirst_freq_LUT, 0 ) + 
            CLprm_getExitLatency( 
                hCLprReg->m_minLprm[nCore][pfirst_freq_LUT->data_idx], 0, 
                pfirst_freq_LUT, 0 );
      }
      else
      {
        /* There is not a minimum enabled latency mode, so pass 0 to the idle
         * plugin node, which will effectively make it ignore this paramter.*/
        tempMinLatency = 0;
      }

      /* Modes are enabled, so see if the min enabled mode latency has changed
       * since the last time it was sent to the idle plugin node. */
      if( last_min_enabled_mode_latency != tempMinLatency )
      {
        /* The idle plugin node does not know about this mode latency, so
         * send it over. */
        npa_issue_required_request( sleep_idle_plugin_mode_latency_client,
                                    tempMinLatency );
        last_min_enabled_mode_latency = tempMinLatency;
      }
    }
  }/* end nCore loop */

  /* Release our fLUT lock */
  sleep_lookup_table_unlock_fLUT();

  return;
}

/*
 * getMinEnabledModeLatency
 */
uint32 CLprRegistry_getMinEnabledModeLatency
(
  CLprRegistry              *hCLprReg,
  uint64                    duration,
  sleep_freq_lookup_table   *pfreq_LUT,
  uint32                    cycle_id
)
{
  unsigned int            core_num;
  uint32                  tempMinLatency;
  uint32                  currLatency;
  CLprm                   **modes_list;
  int                     i;
  CLprm                   *m_minLprm;
  uint32                  retLatency = 0;
  
  /* Obtaining core number that is executing this function (will be called
   * during idle time). */
  core_num = CORE_CURR_CORE;
  CORE_VERIFY( core_num < SLEEP_NUM_CORES );
  CORE_VERIFY_PTR(hCLprReg);
  CORE_VERIFY_PTR(hCLprReg->m_minLprm[core_num]);

  /* Read the minimum latency LPRM into a local variable, since it could
   * be changed at any time by another core. */
  m_minLprm = hCLprReg->m_minLprm[core_num][pfreq_LUT->data_idx];
  if( NULL != m_minLprm )
  {
    /* All enabled modes have constant latencies */
    retLatency = CLprm_getEnterLatency( m_minLprm, duration, 
                                        pfreq_LUT, cycle_id ) +
                  CLprm_getExitLatency( m_minLprm, duration, 
                                        pfreq_LUT, cycle_id );
  }
  else if( SLEEP_DISABLED == CLpr_getStatus( hCLprReg->m_lprs[core_num] ) )
  {
    /* No mode is enabled */
    retLatency = 0;
  }
  else
  {
    /* Enabled mode(s) with variable latencies */
    tempMinLatency = UINT32_MAX;

    modes_list = CLpr_getModesList( hCLprReg->m_lprs[core_num] );

    for( i = 0; i < SLEEP_LPR_NUM_SYNTHESIZED_MODES; i++ )
    {
      /* Make sure the mode is enabled */
      if( SLEEP_DISABLED == CLprm_getStatus( modes_list[i] ) )
        continue;

      currLatency = 
        CLprm_getEnterLatency( modes_list[i], duration, 
                               pfreq_LUT, cycle_id ) +
          CLprm_getExitLatency( modes_list[i], duration, 
                                pfreq_LUT, cycle_id );

      if( currLatency < tempMinLatency )
      {
        tempMinLatency = currLatency;
      }
    }

    retLatency = tempMinLatency;
  }

  return retLatency;
}

/*
 * getLprsList
 */
CLpr* CLprRegistry_getLprsList( CLprRegistry *hCLprReg, int core_id )
{
  unsigned int core_num = core_id == -1 ? CORE_CURR_CORE : core_id;
  CORE_VERIFY(core_num < SLEEP_NUM_CORES);
  CORE_VERIFY_PTR(hCLprReg);
  return hCLprReg->m_lprs[core_num];
}

/*
 * getNumberEnableLprs
 */
unsigned int CLprRegistry_getNumberEnabledLprs( CLprRegistry *hCLprReg, 
                                                int core_id )
{
  unsigned int core_num = core_id == -1 ? CORE_CURR_CORE : core_id;
  CORE_VERIFY(core_num < SLEEP_NUM_CORES);
  CORE_VERIFY_PTR(hCLprReg);

  if( SLEEP_ENABLED == CLpr_getStatus( hCLprReg->m_lprs[core_num] ) )
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

