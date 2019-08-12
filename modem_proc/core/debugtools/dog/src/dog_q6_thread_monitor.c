/*=============================================================================
  Copyright (c) 1990, 1992-1995, 1998-2010 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*===========================================================================

            DOG Q6 THREAD MONITOR HEADER

DESCRIPTION
  This file contains external procedure declarations necessary for
  accessing procedures contained within the dog q6 thread monitor file.


===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/debugtools/dog/src/dog_q6_thread_monitor.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "core_variation.h"

#include "comdef.h"             /* Definitions for byte, word, etc.     */

#ifndef FEATURE_WINCE
#include "rex.h"                /* definition of REX data types         */
#include "task.h"               /* Definition for mc_tcb                */
#endif

#include "err.h"                /* Error logging macros                 */
#include "assert.h"             /* Definition of the ASSERT macro       */


#include "dog.h"                /* Watchdog typedefs and prototypes     */
#include "rcinit.h"

/*===========================================================================

                     Defines and global variables FOR MODULE

===========================================================================*/

#if defined(FEATURE_Q6_THREAD_MONITOR)

#define T0_MONITOR_WAKEUP_TIMER_SIG    0x0800
#define T1_MONITOR_WAKEUP_TIMER_SIG    0x0800
#define T2_MONITOR_WAKEUP_TIMER_SIG    0x0800
#define T3_MONITOR_WAKEUP_TIMER_SIG    0x0800
#define T4_MONITOR_WAKEUP_TIMER_SIG    0x0800
#define T5_MONITOR_WAKEUP_TIMER_SIG    0x0800

#ifdef DOG_DYNAMIC_API   /* Dynamic Dog Interface */
 
  dog_report_type   T0_MONITOR_dog_rpt_var           = 0;      /* Initial Safety */
  uint32            T0_MONITOR_dog_rpt_time_var      = 0xffff; /* Initial Safety */
  dog_report_type   T1_MONITOR_dog_rpt_var           = 0;      /* Initial Safety */
  uint32            T1_MONITOR_dog_rpt_time_var      = 0xffff; /* Initial Safety */
  dog_report_type   T2_MONITOR_dog_rpt_var           = 0;      /* Initial Safety */
  uint32            T2_MONITOR_dog_rpt_time_var      = 0xffff; /* Initial Safety */
  dog_report_type   T3_MONITOR_dog_rpt_var           = 0;      /* Initial Safety */
  uint32            T3_MONITOR_dog_rpt_time_var      = 0xffff; /* Initial Safety */
  dog_report_type   T4_MONITOR_dog_rpt_var           = 0;      /* Initial Safety */
  uint32            T4_MONITOR_dog_rpt_time_var      = 0xffff; /* Initial Safety */
  dog_report_type   T5_MONITOR_dog_rpt_var           = 0;      /* Initial Safety */
  uint32            T5_MONITOR_dog_rpt_time_var      = 0xffff; /* Initial Safety */


#endif   /* DOG_DYNAMIC_API  */

/* Timer to wake Tx_MONITOR task for dog reports
*/
rex_timer_type T0_MONITOR_dog_rpt_timer;
rex_timer_type T1_MONITOR_dog_rpt_timer;
rex_timer_type T2_MONITOR_dog_rpt_timer;
rex_timer_type T3_MONITOR_dog_rpt_timer;
rex_timer_type T4_MONITOR_dog_rpt_timer;
rex_timer_type T5_MONITOR_dog_rpt_timer;

/*===========================================================================

                    Functions

===========================================================================*/


LOCAL void T0_MONITOR_init( void )
{
  rex_sigs_type sigs;   /* signals returned from rex wait procedures */

#ifdef DOG_DYNAMIC_API
  T0_MONITOR_dog_rpt_var      = dog_register( rex_self(), \
                                  DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE);
  T0_MONITOR_dog_rpt_time_var = dog_get_report_period( T0_MONITOR_dog_rpt_var );
#endif /* ! DOG_DYNAMIC_API */

  /* If the return value from dog_get_report_period() was zero then we MUST
  ** NOT report to dog directly, as DOG will monitor us in a different manner.
  ** Hence we won't initialize or start any timer.
  */
  if ( T0_MONITOR_dog_rpt_time_var > 0 ) {
    /* Initialize T0_MONITOR watchdog report timer */
    rex_def_timer( &T0_MONITOR_dog_rpt_timer, rex_self(), T0_MONITOR_WAKEUP_TIMER_SIG );
  }
  

  /* Process task acknowledge procedure.
  */
  rcinit_handshake_startup();
  tmc_task_ack();


  /* Wait for startup/shutdown signal, or a timeout.  The task has been
  ** defined, and will be started after all the other tasks have been started.
  ** If some task hangs during initialization, we'll catch it here.
  */
if ( T0_MONITOR_dog_rpt_time_var > 0 )
  (void) rex_set_timer( &T0_MONITOR_dog_rpt_timer, T0_MONITOR_dog_rpt_time_var );


 while(1)
  {
    sigs = rex_wait( T0_MONITOR_WAKEUP_TIMER_SIG | TASK_START_SIG | TASK_STOP_SIG );

    /* Reset the watchdog time while waiting for a startup signal
    ** from the Main Control task
    */
     if ( sigs & T0_MONITOR_WAKEUP_TIMER_SIG ) {
        (void) rex_clr_sigs ( rex_self(), T0_MONITOR_WAKEUP_TIMER_SIG);

        if ( T0_MONITOR_dog_rpt_time_var > 0 ) {    /* Zero means don't report */
          (void) rex_set_timer( &T0_MONITOR_dog_rpt_timer, T0_MONITOR_dog_rpt_time_var );
          dog_report(T0_MONITOR_dog_rpt_var);
        }
      }

    /* We've been sent the start signal, process task acknowledge procedure
    ** and return.
    */
    if ( sigs & TASK_START_SIG) {
      (void) rex_clr_sigs ( rex_self(), TASK_START_SIG );



      task_ack();

      return;
    }

    /* Acknowledge the signal and continue
    */
    if (sigs & TASK_STOP_SIG ) {
      
	task_stop();
        (void) rex_clr_sigs (rex_self(), TASK_STOP_SIG );
    }
  }
 

 

} /* end of dog_init */



void T0_MONITOR_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  rex_sigs_type sigs;
    /* signals returned from rex wait procedures */

  
  T0_MONITOR_init();                   /* Initialize task */



  /* Must have received the startup signal, loop forever
  */
  for (;;)
  {

      sigs = rex_timed_wait( T0_MONITOR_WAKEUP_TIMER_SIG |
                             TASK_STOP_SIG |
                             TASK_OFFLINE_SIG,
                             &T0_MONITOR_dog_rpt_timer,
                             T0_MONITOR_dog_rpt_time_var );


      /* Time to shutdown,
      ** acknowledge the signal and suspend until TASK_START_SIG
      */
      if ( sigs & TASK_STOP_SIG ) {

         
        task_stop();
        (void) rex_clr_sigs (rex_self(), TASK_STOP_SIG );

      }


      /* If we're supposed to go offline, let's do it
      */
      if ( sigs & TASK_OFFLINE_SIG ) {


        /* Process task offline procedure.
        */
         task_offline();

        /* Clear the offline signal, and return
        */
        (void) rex_clr_sigs( rex_self(), TASK_OFFLINE_SIG );
      }

      if ( sigs & T0_MONITOR_WAKEUP_TIMER_SIG ) {
        (void) rex_clr_sigs ( rex_self(), T0_MONITOR_WAKEUP_TIMER_SIG);

        if ( T0_MONITOR_dog_rpt_time_var > 0 ) {    /* Zero means don't report */
          (void) rex_set_timer( &T0_MONITOR_dog_rpt_timer, T0_MONITOR_dog_rpt_time_var );
          dog_report(T0_MONITOR_dog_rpt_var);
        }
      }  
       
  } /* end of infinite loop */

} /* end of T0_MONITOR_task */



LOCAL void T1_MONITOR_init( void )
{
  rex_sigs_type sigs;   /* signals returned from rex wait procedures */

#ifdef DOG_DYNAMIC_API
  T1_MONITOR_dog_rpt_var      = dog_register( rex_self(), \
                                  DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE);
  T1_MONITOR_dog_rpt_time_var = dog_get_report_period( T1_MONITOR_dog_rpt_var );
#endif /* ! DOG_DYNAMIC_API */

  /* If the return value from dog_get_report_period() was zero then we MUST
  ** NOT report to dog directly, as DOG will monitor us in a different manner.
  ** Hence we won't initialize or start any timer.
  */
  if ( T1_MONITOR_dog_rpt_time_var > 0 ) {
    /* Initialize T1_MONITOR watchdog report timer */
    rex_def_timer( &T1_MONITOR_dog_rpt_timer, rex_self(), T1_MONITOR_WAKEUP_TIMER_SIG );
  }
  

  /* Process task acknowledge procedure.
  */
  task_ack();


  /* Wait for startup/shutdown signal, or a timeout.  The task has been
  ** defined, and will be started after all the other tasks have been started.
  ** If some task hangs during initialization, we'll catch it here.
  */
if ( T1_MONITOR_dog_rpt_time_var > 0 )
  (void) rex_set_timer( &T1_MONITOR_dog_rpt_timer, T1_MONITOR_dog_rpt_time_var );


 while(1)
  {
    sigs = rex_wait( T1_MONITOR_WAKEUP_TIMER_SIG | TASK_START_SIG | TASK_STOP_SIG );

    /* Reset the watchdog time while waiting for a startup signal
    ** from the Main Control task
    */
     if ( sigs & T1_MONITOR_WAKEUP_TIMER_SIG ) {
        (void) rex_clr_sigs (rex_self(), T1_MONITOR_WAKEUP_TIMER_SIG);

        if ( T1_MONITOR_dog_rpt_time_var > 0 ) {    /* Zero means don't report */
          (void) rex_set_timer( &T1_MONITOR_dog_rpt_timer, T1_MONITOR_dog_rpt_time_var );
          dog_report(T1_MONITOR_dog_rpt_var);
        }
      }

    /* We've been sent the start signal, process task acknowledge procedure
    ** and return.
    */
    if ( sigs & TASK_START_SIG) {
      (void) rex_clr_sigs ( rex_self(), TASK_START_SIG );



      task_ack();

      return;
    }

    /* Acknowledge the signal and continue
    */
    if (sigs & TASK_STOP_SIG ) {
      
	task_stop();
        (void) rex_clr_sigs (rex_self(), TASK_STOP_SIG );
    }
  }
 

 

} /* end of dog_init */



void T1_MONITOR_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  rex_sigs_type sigs;
    /* signals returned from rex wait procedures */

  
  T1_MONITOR_init();                   /* Initialize task */



  /* Must have received the startup signal, loop forever
  */
  for (;;)
  {

      sigs = rex_timed_wait( T1_MONITOR_WAKEUP_TIMER_SIG |
                             TASK_STOP_SIG |
                             TASK_OFFLINE_SIG,
                             &T1_MONITOR_dog_rpt_timer,
                             T1_MONITOR_dog_rpt_time_var );


      /* Time to shutdown,
      ** acknowledge the signal and suspend until TASK_START_SIG
      */
      if ( sigs & TASK_STOP_SIG ) {

         
        task_stop();
        (void) rex_clr_sigs (rex_self(), TASK_STOP_SIG );

      }


      /* If we're supposed to go offline, let's do it
      */
      if ( sigs & TASK_OFFLINE_SIG ) {


        /* Process task offline procedure.
        */
         task_offline();

        /* Clear the offline signal, and return
        */
        (void) rex_clr_sigs(  rex_self(), TASK_OFFLINE_SIG );
      }

      if ( sigs & T1_MONITOR_WAKEUP_TIMER_SIG ) {
        (void) rex_clr_sigs ( rex_self(), T1_MONITOR_WAKEUP_TIMER_SIG);

        if ( T1_MONITOR_dog_rpt_time_var > 0 ) {    /* Zero means don't report */
          (void) rex_set_timer( &T1_MONITOR_dog_rpt_timer, T1_MONITOR_dog_rpt_time_var );
          dog_report(T1_MONITOR_dog_rpt_var);
        }
      }  
       
  } /* end of infinite loop */

} /* end of T1_MONITOR_task */



LOCAL void T2_MONITOR_init( void )
{
  rex_sigs_type sigs;   /* signals returned from rex wait procedures */

#ifdef DOG_DYNAMIC_API
  T2_MONITOR_dog_rpt_var      = dog_register( rex_self(), \
                                  DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE);
  T2_MONITOR_dog_rpt_time_var = dog_get_report_period( T2_MONITOR_dog_rpt_var );
#endif /* ! DOG_DYNAMIC_API */

  /* If the return value from dog_get_report_period() was zero then we MUST
  ** NOT report to dog directly, as DOG will monitor us in a different manner.
  ** Hence we won't initialize or start any timer.
  */
  if ( T2_MONITOR_dog_rpt_time_var > 0 ) {
    /* Initialize T2_MONITOR watchdog report timer */
    rex_def_timer( &T2_MONITOR_dog_rpt_timer, rex_self(), T2_MONITOR_WAKEUP_TIMER_SIG );
  }
  

  /* Process task acknowledge procedure.
  */
  task_ack();


  /* Wait for startup/shutdown signal, or a timeout.  The task has been
  ** defined, and will be started after all the other tasks have been started.
  ** If some task hangs during initialization, we'll catch it here.
  */
if ( T2_MONITOR_dog_rpt_time_var > 0 )
  (void) rex_set_timer( &T2_MONITOR_dog_rpt_timer, T2_MONITOR_dog_rpt_time_var );


 while(1)
  {
    sigs = rex_wait( T2_MONITOR_WAKEUP_TIMER_SIG | TASK_START_SIG | TASK_STOP_SIG );

    /* Reset the watchdog time while waiting for a startup signal
    ** from the Main Control task
    */
     if ( sigs & T2_MONITOR_WAKEUP_TIMER_SIG ) {
        (void) rex_clr_sigs ( rex_self(), T2_MONITOR_WAKEUP_TIMER_SIG);

        if ( T2_MONITOR_dog_rpt_time_var > 0 ) {    /* Zero means don't report */
          (void) rex_set_timer( &T2_MONITOR_dog_rpt_timer, T2_MONITOR_dog_rpt_time_var );
          dog_report(T2_MONITOR_dog_rpt_var);
        }
      }

    /* We've been sent the start signal, process task acknowledge procedure
    ** and return.
    */
    if ( sigs & TASK_START_SIG) {
      (void) rex_clr_sigs ( rex_self(), TASK_START_SIG );



      task_ack();

      return;
    }

    /* Acknowledge the signal and continue
    */
    if (sigs & TASK_STOP_SIG ) {
      
	task_stop();
        (void) rex_clr_sigs (rex_self(), TASK_STOP_SIG );
    }
  }
 

 

} /* end of dog_init */



void T2_MONITOR_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  rex_sigs_type sigs;
    /* signals returned from rex wait procedures */

  
  T2_MONITOR_init();                   /* Initialize task */



  /* Must have received the startup signal, loop forever
  */
  for (;;)
  {

      sigs = rex_timed_wait( T2_MONITOR_WAKEUP_TIMER_SIG |
                             TASK_STOP_SIG |
                             TASK_OFFLINE_SIG,
                             &T2_MONITOR_dog_rpt_timer,
                             T2_MONITOR_dog_rpt_time_var );


      /* Time to shutdown,
      ** acknowledge the signal and suspend until TASK_START_SIG
      */
      if ( sigs & TASK_STOP_SIG ) {

         
        task_stop();
        (void) rex_clr_sigs (rex_self(), TASK_STOP_SIG );

      }


      /* If we're supposed to go offline, let's do it
      */
      if ( sigs & TASK_OFFLINE_SIG ) {


        /* Process task offline procedure.
        */
         task_offline();

        /* Clear the offline signal, and return
        */
        (void) rex_clr_sigs( rex_self(), TASK_OFFLINE_SIG );
      }

      if ( sigs & T2_MONITOR_WAKEUP_TIMER_SIG ) {
        (void) rex_clr_sigs ( rex_self(), T2_MONITOR_WAKEUP_TIMER_SIG);

        if ( T2_MONITOR_dog_rpt_time_var > 0 ) {    /* Zero means don't report */
          (void) rex_set_timer( &T2_MONITOR_dog_rpt_timer, T2_MONITOR_dog_rpt_time_var );
          dog_report(T2_MONITOR_dog_rpt_var);
        }
      }  
       
  } /* end of infinite loop */

} /* end of T2_MONITOR_task */




LOCAL void T3_MONITOR_init( void )
{
  rex_sigs_type sigs;   /* signals returned from rex wait procedures */

#ifdef DOG_DYNAMIC_API
  T3_MONITOR_dog_rpt_var      = dog_register( rex_self(), \
                                  DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE);
  T3_MONITOR_dog_rpt_time_var = dog_get_report_period( T3_MONITOR_dog_rpt_var );
#endif /* ! DOG_DYNAMIC_API */

  /* If the return value from dog_get_report_period() was zero then we MUST
  ** NOT report to dog directly, as DOG will monitor us in a different manner.
  ** Hence we won't initialize or start any timer.
  */
  if ( T3_MONITOR_dog_rpt_time_var > 0 ) {
    /* Initialize T3_MONITOR watchdog report timer */
    rex_def_timer( &T3_MONITOR_dog_rpt_timer, rex_self(), T3_MONITOR_WAKEUP_TIMER_SIG );
  }
  

  /* Process task acknowledge procedure.
  */
  task_ack();


  /* Wait for startup/shutdown signal, or a timeout.  The task has been
  ** defined, and will be started after all the other tasks have been started.
  ** If some task hangs during initialization, we'll catch it here.
  */
if ( T3_MONITOR_dog_rpt_time_var > 0 )
  (void) rex_set_timer( &T3_MONITOR_dog_rpt_timer, T3_MONITOR_dog_rpt_time_var );


 while(1)
  {
    sigs = rex_wait( T3_MONITOR_WAKEUP_TIMER_SIG | TASK_START_SIG | TASK_STOP_SIG );

    /* Reset the watchdog time while waiting for a startup signal
    ** from the Main Control task
    */
     if ( sigs & T3_MONITOR_WAKEUP_TIMER_SIG ) {
        (void) rex_clr_sigs ( rex_self(), T3_MONITOR_WAKEUP_TIMER_SIG);

        if ( T3_MONITOR_dog_rpt_time_var > 0 ) {    /* Zero means don't report */
          (void) rex_set_timer( &T3_MONITOR_dog_rpt_timer, T3_MONITOR_dog_rpt_time_var );
          dog_report(T3_MONITOR_dog_rpt_var);
        }
      }

    /* We've been sent the start signal, process task acknowledge procedure
    ** and return.
    */
    if ( sigs & TASK_START_SIG) {
      (void) rex_clr_sigs ( rex_self(), TASK_START_SIG );



      task_ack();

      return;
    }

    /* Acknowledge the signal and continue
    */
    if (sigs & TASK_STOP_SIG ) {
      
	task_stop();
        (void) rex_clr_sigs (rex_self(), TASK_STOP_SIG );
    }
  }
 

 

} /* end of dog_init */



void T3_MONITOR_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  rex_sigs_type sigs;
    /* signals returned from rex wait procedures */

  
  T3_MONITOR_init();                   /* Initialize task */



  /* Must have received the startup signal, loop forever
  */
  for (;;)
  {

      sigs = rex_timed_wait( T3_MONITOR_WAKEUP_TIMER_SIG |
                             TASK_STOP_SIG |
                             TASK_OFFLINE_SIG,
                             &T3_MONITOR_dog_rpt_timer,
                             T3_MONITOR_dog_rpt_time_var );


      /* Time to shutdown,
      ** acknowledge the signal and suspend until TASK_START_SIG
      */
      if ( sigs & TASK_STOP_SIG ) {

         
        task_stop();
        (void) rex_clr_sigs (rex_self(), TASK_STOP_SIG );

      }


      /* If we're supposed to go offline, let's do it
      */
      if ( sigs & TASK_OFFLINE_SIG ) {


        /* Process task offline procedure.
        */
         task_offline();

        /* Clear the offline signal, and return
        */
        (void) rex_clr_sigs( rex_self(), TASK_OFFLINE_SIG );
      }

      if ( sigs & T3_MONITOR_WAKEUP_TIMER_SIG ) {
        (void) rex_clr_sigs ( rex_self(), T3_MONITOR_WAKEUP_TIMER_SIG);

        if ( T3_MONITOR_dog_rpt_time_var > 0 ) {    /* Zero means don't report */
          (void) rex_set_timer( &T3_MONITOR_dog_rpt_timer, T3_MONITOR_dog_rpt_time_var );
          dog_report(T3_MONITOR_dog_rpt_var);
        }
      }  
       
  } /* end of infinite loop */

} /* end of T3_MONITOR_task */




LOCAL void T4_MONITOR_init( void )
{
  rex_sigs_type sigs;   /* signals returned from rex wait procedures */

#ifdef DOG_DYNAMIC_API
  T4_MONITOR_dog_rpt_var      = dog_register( rex_self(), \
                                  DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE);
  T4_MONITOR_dog_rpt_time_var = dog_get_report_period( T4_MONITOR_dog_rpt_var );
#endif /* ! DOG_DYNAMIC_API */

  /* If the return value from dog_get_report_period() was zero then we MUST
  ** NOT report to dog directly, as DOG will monitor us in a different manner.
  ** Hence we won't initialize or start any timer.
  */
  if ( T4_MONITOR_dog_rpt_time_var > 0 ) {
    /* Initialize T4_MONITOR watchdog report timer */
    rex_def_timer( &T4_MONITOR_dog_rpt_timer, rex_self(), T4_MONITOR_WAKEUP_TIMER_SIG );
  }
  

  /* Process task acknowledge procedure.
  */
  task_ack();


  /* Wait for startup/shutdown signal, or a timeout.  The task has been
  ** defined, and will be started after all the other tasks have been started.
  ** If some task hangs during initialization, we'll catch it here.
  */
if ( T4_MONITOR_dog_rpt_time_var > 0 )
  (void) rex_set_timer( &T4_MONITOR_dog_rpt_timer, T4_MONITOR_dog_rpt_time_var );


 while(1)
  {
    sigs = rex_wait( T4_MONITOR_WAKEUP_TIMER_SIG | TASK_START_SIG | TASK_STOP_SIG );

    /* Reset the watchdog time while waiting for a startup signal
    ** from the Main Control task
    */
     if ( sigs & T4_MONITOR_WAKEUP_TIMER_SIG ) {
        (void) rex_clr_sigs (rex_self(), T4_MONITOR_WAKEUP_TIMER_SIG);

        if ( T4_MONITOR_dog_rpt_time_var > 0 ) {    /* Zero means don't report */
          (void) rex_set_timer( &T4_MONITOR_dog_rpt_timer, T4_MONITOR_dog_rpt_time_var );
          dog_report(T4_MONITOR_dog_rpt_var);
        }
      }

    /* We've been sent the start signal, process task acknowledge procedure
    ** and return.
    */
    if ( sigs & TASK_START_SIG) {
      (void) rex_clr_sigs ( rex_self(), TASK_START_SIG );



      task_ack();

      return;
    }

    /* Acknowledge the signal and continue
    */
    if (sigs & TASK_STOP_SIG ) {
      
	task_stop();
        (void) rex_clr_sigs (rex_self(), TASK_STOP_SIG );
    }
  }
 

 

} /* end of dog_init */



void T4_MONITOR_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  rex_sigs_type sigs;
    /* signals returned from rex wait procedures */

  
  T4_MONITOR_init();                   /* Initialize task */



  /* Must have received the startup signal, loop forever
  */
  for (;;)
  {

      sigs = rex_timed_wait( T4_MONITOR_WAKEUP_TIMER_SIG |
                             TASK_STOP_SIG |
                             TASK_OFFLINE_SIG,
                             &T4_MONITOR_dog_rpt_timer,
                             T4_MONITOR_dog_rpt_time_var );


      /* Time to shutdown,
      ** acknowledge the signal and suspend until TASK_START_SIG
      */
      if ( sigs & TASK_STOP_SIG ) {

         
        task_stop();
        (void) rex_clr_sigs (rex_self(), TASK_STOP_SIG );

      }


      /* If we're supposed to go offline, let's do it
      */
      if ( sigs & TASK_OFFLINE_SIG ) {


        /* Process task offline procedure.
        */
         task_offline();

        /* Clear the offline signal, and return
        */
        (void) rex_clr_sigs( rex_self(), TASK_OFFLINE_SIG );
      }

      if ( sigs & T4_MONITOR_WAKEUP_TIMER_SIG ) {
        (void) rex_clr_sigs ( rex_self(), T4_MONITOR_WAKEUP_TIMER_SIG);

        if ( T4_MONITOR_dog_rpt_time_var > 0 ) {    /* Zero means don't report */
          (void) rex_set_timer( &T4_MONITOR_dog_rpt_timer, T4_MONITOR_dog_rpt_time_var );
          dog_report(T4_MONITOR_dog_rpt_var);
        }
      }  
       
  } /* end of infinite loop */

} /* end of T4_MONITOR_task */



LOCAL void T5_MONITOR_init( void )
{
  rex_sigs_type sigs;   /* signals returned from rex wait procedures */

#ifdef DOG_DYNAMIC_API
  T5_MONITOR_dog_rpt_var      = dog_register( rex_self(), \
                                  DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE);
  T5_MONITOR_dog_rpt_time_var = dog_get_report_period( T5_MONITOR_dog_rpt_var );
#endif /* ! DOG_DYNAMIC_API */

  /* If the return value from dog_get_report_period() was zero then we MUST
  ** NOT report to dog directly, as DOG will monitor us in a different manner.
  ** Hence we won't initialize or start any timer.
  */
  if ( T5_MONITOR_dog_rpt_time_var > 0 ) {
    /* Initialize T5_MONITOR watchdog report timer */
    rex_def_timer( &T5_MONITOR_dog_rpt_timer, rex_self(), T5_MONITOR_WAKEUP_TIMER_SIG );
  }
  

  /* Process task acknowledge procedure.
  */
  task_ack();


  /* Wait for startup/shutdown signal, or a timeout.  The task has been
  ** defined, and will be started after all the other tasks have been started.
  ** If some task hangs during initialization, we'll catch it here.
  */
if ( T5_MONITOR_dog_rpt_time_var > 0 )
  (void) rex_set_timer( &T5_MONITOR_dog_rpt_timer, T5_MONITOR_dog_rpt_time_var );


 while(1)
  {
    sigs = rex_wait( T5_MONITOR_WAKEUP_TIMER_SIG | TASK_START_SIG | TASK_STOP_SIG );

    /* Reset the watchdog time while waiting for a startup signal
    ** from the Main Control task
    */
     if ( sigs & T5_MONITOR_WAKEUP_TIMER_SIG ) {
        (void) rex_clr_sigs ( rex_self(), T5_MONITOR_WAKEUP_TIMER_SIG);

        if ( T5_MONITOR_dog_rpt_time_var > 0 ) {    /* Zero means don't report */
          (void) rex_set_timer( &T5_MONITOR_dog_rpt_timer, T5_MONITOR_dog_rpt_time_var );
          dog_report(T5_MONITOR_dog_rpt_var);
        }
      }

    /* We've been sent the start signal, process task acknowledge procedure
    ** and return.
    */
    if ( sigs & TASK_START_SIG) {
      (void) rex_clr_sigs ( rex_self(), TASK_START_SIG );



      task_ack();

      return;
    }

    /* Acknowledge the signal and continue
    */
    if (sigs & TASK_STOP_SIG ) {
      
	task_stop();
        (void) rex_clr_sigs (rex_self(), TASK_STOP_SIG );
    }
  }
 

 

} /* end of dog_init */



void T5_MONITOR_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  rex_sigs_type sigs;
    /* signals returned from rex wait procedures */

  
  T5_MONITOR_init();                   /* Initialize task */



  /* Must have received the startup signal, loop forever
  */
  for (;;)
  {

      sigs = rex_timed_wait( T5_MONITOR_WAKEUP_TIMER_SIG |
                             TASK_STOP_SIG |
                             TASK_OFFLINE_SIG,
                             &T5_MONITOR_dog_rpt_timer,
                             T5_MONITOR_dog_rpt_time_var );


      /* Time to shutdown,
      ** acknowledge the signal and suspend until TASK_START_SIG
      */
      if ( sigs & TASK_STOP_SIG ) {

         
        task_stop();
        (void) rex_clr_sigs (rex_self(), TASK_STOP_SIG );

      }


      /* If we're supposed to go offline, let's do it
      */
      if ( sigs & TASK_OFFLINE_SIG ) {


        /* Process task offline procedure.
        */
         task_offline();

        /* Clear the offline signal, and return
        */
        (void) rex_clr_sigs( rex_self(), TASK_OFFLINE_SIG );
      }

      if ( sigs & T5_MONITOR_WAKEUP_TIMER_SIG ) {
        (void) rex_clr_sigs (rex_self(), T5_MONITOR_WAKEUP_TIMER_SIG);

        if ( T5_MONITOR_dog_rpt_time_var > 0 ) {    /* Zero means don't report */
          (void) rex_set_timer( &T5_MONITOR_dog_rpt_timer, T5_MONITOR_dog_rpt_time_var );
          dog_report(T5_MONITOR_dog_rpt_var);
        }
      }  
       
  } /* end of infinite loop */

} /* end of T5_MONITOR_task */

#endif
