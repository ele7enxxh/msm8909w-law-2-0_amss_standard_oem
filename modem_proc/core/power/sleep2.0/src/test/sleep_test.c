/*==============================================================================
  FILE:         sleep_test.c

  OVERVIEW:     This file provides a test thread used by sleep to create a
                timer for use in testing LPRMs

  DEPENDENCIES: sleep_test_timer() is the entry point for the sleep_test thread

                Copyright (c) 2011-2015 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================ 
 ================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/test/sleep_test.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "rcinit.h"
#include "timer.h"
#include "rex.h"
#include "busywait.h"
#include "sleep_log.h"
#include "sleep_solver.h"
#include "CoreVerify.h"
#include "npa.h"

/*==============================================================================
                                  DEFINES
 =============================================================================*/
/* Signal that will be sent when the timer expires. */
#define STH_TIMER_EXP_SIGNAL 0x1

/*==============================================================================
                          GLOBAL TYPE DEFINITIONS
 =============================================================================*/
/** Timer related data structures
 * 
 * timer_group = the timer group we'll be using
 *       timer = the timer we'll be using
 */
struct timer_group_struct sth_timer_group;
timer_type sth_timer;
time_timetick_type sth_time_ticks; 

/*==============================================================================
                         STATIC/GLOBAL VARIABLES
 =============================================================================*/
uint32 sth_random_lprm = 1;
uint32 sth_enable = 0;
uint32 sth_timer_dur_ms = 100;
uint32 sth_max_duration_ticks = 250000; /*19.2 * 1000 * 1;*/

uint32 sth_PC_noret_duration_ticks = 40000000; 
uint32 sth_RailLpm_duration_ticks =  25000000;
uint32 sth_PLL_Lpm_duration_ticks =     20000;
uint32 sth_APCR_duration_ticks =        20000;
uint32 sth_ClkGate_duration_ticks =      8000;

/*==============================================================================
                         STATIC FUNCTIONS
 =============================================================================*/
void sth_timer_init(struct timer_group_struct* tg, timer_type* t)
{
  /* Make sure that the timer is non-deferrable. */
  timer_group_set_deferrable(tg,FALSE);

  /* Define the timer */
  timer_def(t,                                   // Pointer to the timer
            tg,                                  // Pointer to the timer group
            rex_self(),                           // Pointer to signal recipient
            (rex_sigs_type) STH_TIMER_EXP_SIGNAL, // Signal sent as timer expires
            NULL,                                 // Callback function
            0);                                   // Callback data
}

void sth_timer_set(timer_type *t, time_timetick_type tt)
{
  timer_set_64(t,         // Pointer to timer     
               tt,        // duration of timer in msec
               0,         // Don't reload timer 
               T_MSEC);   // Duration units    
  
}

void sth_timer_wait(timer_type *t)
{
  rex_wait((rex_sigs_type) STH_TIMER_EXP_SIGNAL);
}

void sth_yield(uint32 yield_dur_ms)
{
  sth_timer_set(&sth_timer, yield_dur_ms);
    
  sth_timer_wait(&sth_timer);
}

/** 
 * @brief QUERY function
 */
uint32 sth_is_resource_available(const char* resource)
{
  static uint32 resource_available;

  if ( ! resource_available )
  {
    resource_available  = (NPA_QUERY_SUCCESS == npa_query_resource_available(resource));
  }
  return resource_available;
}

uint32 sth_resource_request(const char* resource, uint32 request)
{
  static npa_client_handle client_handle;

  if( !client_handle )
  {
    client_handle = npa_create_sync_client(resource,
                                           "/sleep/test",
                                           NPA_CLIENT_REQUIRED);
    CORE_VERIFY( client_handle );
  }
    
  npa_issue_required_request( client_handle, request );
  return request;
}

void sth_voteonce_max_dur(uint32 max_duration_ticks)
{
  static uint32 change_vote = 1;

  if ( change_vote )
  {
    sth_resource_request("/sleep/max_duration", max_duration_ticks);
    change_vote = 0;
  }
}

void sth_toggle_max_dur(uint32 iter_count)
{
  uint32 change_vote = (iter_count % 5 == 0);

  if ( change_vote )
  {
    uint32 vote_toggle = (iter_count % 2);
    sth_resource_request("/sleep/max_duration", vote_toggle?sth_max_duration_ticks:0);
  }
}

void sth_TestAllLprms(uint32 iter_count)
{
  uint32 lprm = iter_count % 5;
  switch ( lprm )
  {
    case 0:
      sth_resource_request("/sleep/max_duration", sth_PC_noret_duration_ticks);
      break;

    case 1:
      sth_resource_request("/sleep/max_duration", sth_RailLpm_duration_ticks);
      break;

    case 2:
      sth_resource_request("/sleep/max_duration", sth_PLL_Lpm_duration_ticks);
      break;

    case 3:
      sth_resource_request("/sleep/max_duration", sth_APCR_duration_ticks);
      break;

    case 4:
      sth_resource_request("/sleep/max_duration", sth_ClkGate_duration_ticks);
      break;

    default:
      sth_resource_request("/sleep/max_duration", sth_PC_noret_duration_ticks);
      break;
  }
}

/*==============================================================================
                        THREAD ENTRY FUNCTION 
 =============================================================================*/
void sleep_test_timer(unsigned long int unused_param)
{
  /** 
   *  @brief Log a message signaling that the STH has begun
   *         executing.
   *        sleepLog_printf(SLEEP_LOG_LEVEL_INFO,
   *                        0,
   *                        "STH: Start Timer Test");
   */
  uint32 iter_count = 0;

  /* Notify RCInit that all basic initialization is complete. */
  rcinit_handshake_startup();

  /* Sleep Test Harness is not enabled, return to destroy task */
  if ( sth_enable == 0 )
  {
    return;
  }
  
  /* Initialize the timer used by STH */
  sth_timer_init(&sth_timer_group, &sth_timer);

  while(1)
  {
    /** 
     *  @brief Test 1: Wait for max_duration
     *  Vote once for max_duration
     *  Peform 100 wakeups
     */

    while ( sth_is_resource_available("/sleep/max_duration") == 0 )
    {
      sth_yield(sth_timer_dur_ms);
    }

    sth_voteonce_max_dur(sth_max_duration_ticks);

    if ( !sth_random_lprm ) 
    {
      /** 
       *   @brief Do not use Random LPRM test
       */ 
      iter_count = 100;

      while ( iter_count )
      {
        sth_yield(sth_timer_dur_ms);
        iter_count--;
      }

      /** 
       *  @brief Test 2:  Wait for max_duration
       *                  Toggle max_duration using sth_toggle_max_dur
       *                  for 1000 iters
       */

      iter_count = 0;
    
      while ( iter_count < 1000 )
      {
        sth_toggle_max_dur(iter_count);
        sth_yield(sth_timer_dur_ms);
        iter_count++;
      }
    }
    else
    {
      /** 
       *  @brief Random LPRM test
       *       Assign each LPRM to an integer value and use random
       *         number generator to select the LPRM from the random
       *         number
       */
      iter_count = 0;
      uint32 NumOfLprms = 5;
      uint32 testLprmNum = 0; 
      uint32 idx;

      while ( iter_count < 25 )
      {
        for (idx = 0; idx < 5; idx++) 
        {
          testLprmNum = rand() % NumOfLprms;
          sth_TestAllLprms(testLprmNum);
          sth_yield(sth_timer_dur_ms);
        }
        iter_count++;
      }
    }
  }
}
