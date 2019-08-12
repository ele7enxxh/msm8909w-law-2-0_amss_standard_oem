/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              Q M I _ M M O D E _ T A S K . C

DESCRIPTION
  This is the source file for the QMI Mmcp Task. Contained herein are the
  functions needed to initialize all the modules that execute in QMI task
  context and the main task processing loop.

EXTERNALIZED FUNCTIONS 
  qmi_mmode_task()
    QMI Task entry point and main processing loop.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI services can be used unless MC has created the QMI task
  and qmi_mmode_task() has been invoked.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_mmode_task.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/08/12    sk     Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "task.h"
#include "rex.h"
#include "queue.h"
#include "dog_hb_rex.h"
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif
#include "qmi_mmode_task_svc.h"
#include "qmi_mmode_taski.h"
#include "qmi_mmode_task_cmd.h"
#include "qmi_mmode_svc.h"
#include "amssassert.h"
#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif


/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

rex_tcb_type   *qmi_mmode_tcb_ptr; // pointer for myself

/*--------------------------------------------------------------------------
  Timers needed for the QMI Ctrl Task.
---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
  Dynamic dog variable
---------------------------------------------------------------------------*/
dog_report_type  qmi_mmode_dog_rpt_var            = 0;        /* Dog report ID       */

extern void qmi_mmode_svc_init
(
  void
);

extern void qmi_mmodei_msgr_init
(
  void
);

/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

                           LOCAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================
FUNCTION QMI_MMODEI_INIT()

DESCRIPTION
  Initialization function for the various QMI services.

DEPENDENCIES
  QMI Modem task should have been started already.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void qmi_mmodei_init
(
  void
)
{
  qmi_mmode_tcb_ptr = rex_self();

  /*------------------------------------------------------------------------
    Initialize MSG ROUTER Services here
  --------------------------------------------------------------------------*/
  qmi_mmodei_msgr_init();

  /*-------------------------------------------------------------------------
    Initialize the QMI-Services here
  -------------------------------------------------------------------------*/
  qmi_mmode_svc_init();

} /* qmi_mmodei_init() */


/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION qmi_mmode_init_before_task_start

DESCRIPTION
  Initialize qmi mmode cmd queues and other objects that needs to be 
  initialized before the task is started

  NOTE! Any initialization that must be done before other tasks or clients
  tries to contact QMI MMODE, must be done in this initialization function.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void qmi_mmode_init_before_task_start( void )
{


  /*-------------------------------------------------------------------------
    Do power-up initializtion then wait for start signal (from the MC task,
    which controls task startup sequencing)
  -------------------------------------------------------------------------*/
    qmi_mmodei_powerup();
}

/*===========================================================================
FUNCTION QMI_MMODE_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the QMI task.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_mmode_task
(
  uint32 ignored    /* Parameter received from Main Control task - ignored */
)
{
  rex_sigs_type     loop_sigs;      /* Signals to be processed within a task
                                       loop */
  rex_sigs_type     set_sigs = 0;   /* Currently set signals               */
  uint8             loop_cnt;       /* # of times task loop is iterated
                                       on a signal mask                    */
  uint8             i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MODEM_RCINIT
  rcinit_handshake_startup();
#else
  qmi_mmode_init_before_task_start();
#endif

  /* Register with Dog HeartBeat */
  qmi_mmode_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) (1<<QMI_MMODE_DOG_RPT_TIMER_SIGNAL));


#ifdef FEATURE_MODEM_RCINIT
#else
  tmc_task_start();
#endif
  
  qmi_mmodei_init();

  /*-------------------------------------------------------------------------
    Main task loop. Never exit!
  -------------------------------------------------------------------------*/
  for( ;;)
  {
    /*-----------------------------------------------------------------------
      Wait for work to do. No return from wait until one of the enabled
      signals is set. Even if we come out of wait, process only those
      signals which are currently enabled.
    -----------------------------------------------------------------------*/
    set_sigs |= rex_get_sigs( rex_self() );
    set_sigs &= qmi_mmodei_enabled_sig_mask;

    /*-----------------------------------------------------------------------
      Enter into the Wait only if no requested signal events have occurred
    -----------------------------------------------------------------------*/
    if( set_sigs == 0 )
    {
      set_sigs = rex_wait( qmi_mmodei_enabled_sig_mask );
      set_sigs &= qmi_mmodei_enabled_sig_mask;
    }
    (void)rex_clr_sigs( rex_self(), set_sigs );

    loop_sigs = set_sigs & ~( (rex_sigs_type) 1
                             << (rex_sigs_type) QMI_MMODE_SIG_MASK_CHANGE_SIGNAL );
    set_sigs = 0;

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority.
      If the loop iterates for more than QMI_MMODE_MAX_SIGNALS times, some signal
      is not handled by this loop. So instead of going in to infinite loop
      perform a check
    -----------------------------------------------------------------------*/
    loop_cnt = 0;
    while( loop_sigs && loop_cnt++ < (rex_sigs_type) QMI_MMODE_MAX_SIGNALS )
    {
      /*---------------------------------------------------------------------
        If watchdog timer expired, kick the pooch and set timer for next.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_MMODE_DOG_RPT_TIMER_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type)QMI_MMODE_DOG_RPT_TIMER_SIGNAL );
        dog_hb_report(qmi_mmode_dog_rpt_var);

      }
      /*---------------------------------------------------------------------
       *
        QMI Task command queue signal
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_MMODE_CMD_Q_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_MMODE_CMD_Q_SIGNAL );

        if ( qmi_mmodei_process_cmd() == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_MMODE_CMD_Q_SIGNAL );
        }
      }
      /*---------------------------------------------------------------------
        Check if msgr signal was set.  If set then clear signal,
        and process msgr messages.
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_MMODE_TASK_MSGR_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_MMODE_TASK_MSGR_SIGNAL );

        qmi_mmodei_process_msgr();
      }

      /*---------------------------------------------------------------------
        Process other signals with registered handlers
      ---------------------------------------------------------------------*/
      else
      {
        /*----------------------------------------------------------------------
          Handle the QMI Service Signal if set
        -----------------------------------------------------------------------*/
        for( i = QMI_MMODE_SERVICE_VOICE_SIGNAL; i < QMI_MMODE_MAX_SIGNALS; i++)
        {
    
          if( loop_sigs & ( (rex_sigs_type) 1 <<
                                 (rex_sigs_type) i ) )
          {
            loop_sigs &= ~( (rex_sigs_type) 1 <<
                            (rex_sigs_type) i );
    
            if ( qmi_mmodei_sig_handler[i].sig_handler
                 (
                   i ,
                   qmi_mmodei_sig_handler[i].user_data_ptr
                 ) == FALSE )
            {
              set_sigs |= ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) i);
            }
            /*-----------------------------------------------------------------
              allowing one signal type at a time to process which will avoid
              the watchdog timer expiration
            --------------------------------------------------------------------*/
            break;
          }
        }
      }

#ifndef FEATURE_MODEM_RCINIT_PHASE2
      /*---------------------------------------------------------------------
        Check if powerdown command signal was set.  If set then clear signal,
        and process task stop procedures.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_MMODE_TASK_STOP_SIGNAL ) )
      {
        QM_MSG_MED_1("qmi_mmode_task():  QMI_MMODE_TASK_STOP_SIGNAL received %d", ignored);

        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_MMODE_TASK_STOP_SIGNAL );

        task_stop();
      }
      /*---------------------------------------------------------------------
        Check if offline command signal was set.  If set then clear signal,
        and process task offline procedures.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_MMODE_TASK_OFFLINE_SIGNAL ) )
      {
        QM_MSG_MED("qmi_mmode_task():  QMI_MMODE_TASK_OFFLINE_SIGNAL received");

        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_MMODE_TASK_OFFLINE_SIGNAL );

        task_offline();
      }
#endif
    }
    //QM_MSG_MED_2("qmi_mmode_task():  loop_sigs: %d set_sigs: %d", (uint32)loop_sigs, (uint32)set_sigs);
    //ASSERT(0 == loop_sigs);
  } /* forever */

} /* qmi_mmode_task() */

