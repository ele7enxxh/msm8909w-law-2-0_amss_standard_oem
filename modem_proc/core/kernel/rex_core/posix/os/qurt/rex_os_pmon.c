/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            REX Performance Monitoring

GENERAL DESCRIPTION
  Provides DIAG interface to configure and collect performance information.
  Sets up timers to be able to collect performance information periodically
  and send the log packets. Uses the native performance monitoring supported
  by QDSP6.


Copyright (c) 2002-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

 $Header: //components/rel/core.mpss/3.5.c12.3/kernel/rex_core/posix/os/qurt/rex_os_pmon.c#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
08/10/2010   balac   Created.

===========================================================================*/

/* ---------------------------------------------------------------------------
** Includes
** ---------------------------------------------------------------------------  */
#include "core_variation.h"
#include "comdef.h"
#include "log.h"
#include "rexlog.h"
#include "msg.h"
#include "task.h"
#include <stringl/stringl.h>
#include "qurt_pmu.h"
#include "qurt_timer.h"

#ifdef FEATURE_MULTIPROCESSOR
#include "log_codes.h"
#endif

#ifndef BLAST_MAX_HTHREAD_LIMIT
   #define BLAST_MAX_HTHREAD_LIMIT 6
#endif

extern void DcvsQdspPMUbusy(int is_busy);

/* this is a global variable provided by BLAST */
extern unsigned int QURT_MAX_HTHREADS;

#define LOG_BLAST_PM_C_VER         1
#define LOG_BLAST_PM_C_APP         0
#define LOG_BLAST_PM_C_RAW_TASK    1

/* ---------------------------------------------------------------------------
** Data
** ---------------------------------------------------------------------------  */

static struct {
   boolean enabled;
   uint8 evt0;
   uint8 evt1;
   uint8 evt2;
   uint8 evt3;
   uint32 pmon_period;
   rex_timer_type  pmon_timer;
   uint32 tick_base; /* Absolute time that our PM starts. */
   uint64 pcycle_base;
   uint64 idle_bases[BLAST_MAX_HTHREAD_LIMIT];
} rex_pmon = { 0 };


/* ---------------------------------------------------------------------------
** Externs
** ---------------------------------------------------------------------------  */
#if defined(FEATURE_MDM_FW_PMU)
/**
@note: The following two externs are slated to be available via a MDM FW header.
Inorder to unblock internal teams we are adding this workaround.
*/
extern void fws_configure_pmu
(
  boolean enable,    
  uint8   evt0,   
  uint8   evt1,    
  uint8   evt2,  
  uint8   evt3,     
  uint32  pmucfg   
);

extern void fws_request_pmu_log
(
  void
);
#endif

/*===========================================================================
FUNCTION REX_PMON_LOG_CB

DESCRIPTION
   Collects PMU counters, sclk ticks, core pcycles and idle pcycles.
   Constructs log packets and sends out. Re-creates the timer for the next
   call back.
============================================================================*/
static void rex_pmon_log_cb (unsigned long param)
{
   LOG_BLAST_PM_C_type *pm_ptr;
   uint32 this_timestamp;
   uint64 this_pcycle_count; 
   uint64 pcycles[BLAST_MAX_HTHREAD_LIMIT];
   int    index;

   if(rex_pmon.enabled != TRUE )
   {
      return;
   }

   #if defined(FEATURE_MDM_FW_PMU)
       /* Request PM log from FW */
//       fws_request_pmu_log();
   #endif

   pm_ptr = (LOG_BLAST_PM_C_type *)log_alloc(LOG_BLAST_PM_C,
                        sizeof (LOG_BLAST_PM_C_type));

   /* Fill in the log if allocation worked */
   if (pm_ptr != NULL ) {
      /* version no. */
      pm_ptr->ver = LOG_BLAST_PM_C_VER;
      /* App - 0 - Modem SW, 1 - Modem FW, 2 - Multi Media */
      pm_ptr->app = LOG_BLAST_PM_C_APP;
      
      /* Read PM counters */
      pm_ptr->counters[0] = qurt_pmu_get (QURT_PMUCNT0);
      pm_ptr->counters[1] = qurt_pmu_get (QURT_PMUCNT1);
      pm_ptr->counters[2] = qurt_pmu_get (QURT_PMUCNT2);
      pm_ptr->counters[3] = qurt_pmu_get (QURT_PMUCNT3);

      /* Collect statitstics of the IDLE threads */
      qurt_profile_get_idle_pcycles (pcycles);

      /* get the delta idle pcycles */
      for (index=0; index<QURT_MAX_HTHREADS; index++)
      {
        pcycles[index] -= rex_pmon.idle_bases[index];
      }

      /* Collect the thread run time followed by system time
       * so that system run time is always larger */
      this_pcycle_count = qurt_get_core_pcycles ();

      /* Collect the system time */
      this_timestamp = qurt_sysclock_get_hw_ticks ();

      /* initialize the samples */
      memset (pm_ptr->idle_samples, 0, BLAST_MAX_HTHREAD_LIMIT *8);
      /* IDLE time per HW thread */
      memscpy (pm_ptr->idle_samples, QURT_MAX_HTHREADS *8, pcycles, QURT_MAX_HTHREADS *8);
      /*  Total Pcycles */
      pm_ptr->total_samples = this_pcycle_count - rex_pmon.pcycle_base;
      /* Total sleep clock ticks elapsed */
      pm_ptr->total_sclk_samples = this_timestamp - rex_pmon.tick_base;

      log_commit (pm_ptr);
   }

   if (TRUE == rex_pmon.enabled)
   {
      (void)rex_set_timer (&rex_pmon.pmon_timer, rex_pmon.pmon_period);
   }

   return;
}

/*===========================================================================
FUNCTION REX_ENABLE_PMON

DESCRIPTION
  This function fills in the events and, performance monitoring period.
  It collects the sclk ticks  and core pcycles to establish the offset.
  Creates timer with call back function.
============================================================================*/
void rex_enable_pmon (uint8 evt0,
                    uint8 evt1,
                    uint8 evt2,
                    uint8 evt3,
                    uint32 pmon_period)
{
   if( rex_pmon.enabled == FALSE )
   {
      rex_pmon.enabled = TRUE;

      /* Inform DCVS that PMU is used by other Module */
      DcvsQdspPMUbusy(1);

      /* Turn off Performance Monitoring to start clean */
      qurt_pmu_enable (0);

      #if defined(FEATURE_MDM_FW_PMU)
          /* Enable PM on FW */
  //        fws_configure_pmu( TRUE, evt0, evt1, evt2, evt3, 0x3f );
      #endif

      rex_pmon.pmon_period = pmon_period;
      rex_pmon.evt0 = evt0;
      rex_pmon.evt1 = evt1;
      rex_pmon.evt2 = evt2;
      rex_pmon.evt3 = evt3;

      /* Configure PMU events. This will clear the PMU counters as well */
      qurt_pmu_set (QURT_PMUEVTCFG, evt0 | (evt1 << 8)
                                           | (evt2 << 16)
                                           | (evt3 << 24));
      qurt_pmu_set (QURT_PMUCFG, 0x3f);

      /* System ticks should always be the logest. So first collect ticks
       * then reset counters */
      rex_pmon.tick_base = qurt_sysclock_get_hw_ticks ();
      rex_pmon.pcycle_base = qurt_get_core_pcycles ();

      qurt_profile_get_idle_pcycles(rex_pmon.idle_bases);

      /* Turn ON Performance Monitoring */
      qurt_pmu_enable (1);

      /* Start PM logging
      */
      rex_def_timer_ex (
         &rex_pmon.pmon_timer,
         rex_pmon_log_cb,
         0
      );

      (void) rex_set_timer (
         &rex_pmon.pmon_timer,
         rex_pmon.pmon_period
      );

   }
   return;
} /* END rex_enable_pmon */

/*===========================================================================
FUNCTION REX_DISABLE_PMON

DESCRIPTION
  This function clears the timer rex_pmon.pmon_timer
============================================================================*/
void rex_disable_pmon (void)
{
   if( rex_pmon.enabled == TRUE )
   {
      rex_pmon.enabled = FALSE;

      #if defined(FEATURE_MDM_FW_PMU)
          /* Disable PM on FW */
    //      fws_configure_pmu( FALSE, 0, 0, 0, 0, 0 );
      #endif

      qurt_pmu_enable (0);

      (void) rex_undef_timer( &rex_pmon.pmon_timer );

      /* Notify DCVS that PMU is available */
      DcvsQdspPMUbusy(0);

   }
   return;
} /* END rex_disable_pmon */
