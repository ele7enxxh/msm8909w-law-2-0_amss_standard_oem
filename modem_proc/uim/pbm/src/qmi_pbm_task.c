/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              Q M I _ PBM _ T A S K . C

DESCRIPTION
  This is the source file for the QMI PBM Task. Contained herein are the
  functions needed to initialize all the modules that execute in QMI task
  context and the main task processing loop.

EXTERNALIZED FUNCTIONS
  qmi_pbm_task()
    QMI Task entry point and main processing loop.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI services can be used unless MC has created the QMI task
  and qmi_pbm_task() has been invoked.

Copyright (c) 2014 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/qmi_pbm_task.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/25/14    NR     Use v2.0 diag macros
04/01/14    NR     QTF CRM MOB Changes
03/17/14   kb      Add offtarget support to check mutex is owned or not
01/12/14   NR      TMC header deprecation changes
11/26/13    nr     QTF CRM MOB changes
01/25/11    rk     Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "uim_msg.h"
#include "task.h"
#include "rex.h"
#include "queue.h"
#include "dog_hb_rex.h"
#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif
#include "qmi_pbm_task.h"
#include "qmi_pbm_task_svc.h"
#include "qmi_pbm_taski.h"
#include "qmi_pbm_task_cmd.h"
#include "qmi_pbm_svc.h"
#include "amssassert.h"
#include "stringl/stringl.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

rex_tcb_type   *qmi_pbm_tcb_ptr; // pointer for myself


  /*--------------------------------------------------------------------------
    Timers needed for the QMI Ctrl Task.
  ---------------------------------------------------------------------------*/
  /*--------------------------------------------------------------------------
    Dynamic dog variable
  ---------------------------------------------------------------------------*/
dog_report_type  qmi_pbm_dog_rpt_var            = 0;        /* Dog report ID       */

extern void qmi_pbm_svc_init
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
FUNCTION QMI_PBMI_INIT()

DESCRIPTION
  Initialization function for the various QMI services.

DEPENDENCIES
  QMI Modem task should have been started already.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void qmi_pbmi_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  qmi_pbm_tcb_ptr = rex_self();
  /*-------------------------------------------------------------------------
    Initialize the QMI-Services here
  -------------------------------------------------------------------------*/
  qmi_pbm_svc_init();

/*-------------------------------------------------------------------------
    Other initializations can go here
-------------------------------------------------------------------------*/


} /* qmi_pbmi_init() */

/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/
  /*===========================================================================

  FUNCTION qmi_pbm_init_before_task_start

  DESCRIPTION
    Initialize qmi pbm cmd queues and other objects that needs to be
    initialized before the task is started

    NOTE! Any initialization that must be done before other tasks or clients
    tries to contact QMI PBM, must be done in this initialization function.

  DEPENDENCIES
    none

  RETURN VALUE
    none

  SIDE EFFECTS
    none

  ===========================================================================*/
  void qmi_pbm_init_before_task_start( void )
  {


    /*-------------------------------------------------------------------------
      Do power-up initializtion then wait for start signal (from the MC task,
      which controls task startup sequencing)
    -------------------------------------------------------------------------*/
      qmi_pbmi_powerup();
  }

#if defined FEATURE_MODEM_RCINIT && defined FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_MODEM_RCINIT && FEATURE_UIM_TEST_FRAMEWORK */

#ifdef FEATURE_REX_SIGS_EXT
/*===========================================================================
FUNCTION QMI_PBM_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the QMI-PBM
  task.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_pbm_task
(
  uint32 ignored    /* Parameter received from Main Control task - ignored */
)
{
  /*-------------------------------------------------------------------------
    Signals to be processed within a task loop
  -------------------------------------------------------------------------*/
  rex_sigs_type     loop_sigs[QMI_PBM_SIG_ARR_LEN];
  rex_sigs_type     set_sigs[QMI_PBM_SIG_ARR_LEN]; /* Currently set signals */

  /*-------------------------------------------------------------------------
    The result value of the extension signal API is stored in this var.
  -------------------------------------------------------------------------*/
  boolean           sig_result = FALSE;

  /*-------------------------------------------------------------------------
   # of times task loop is iterated on a signal mask
  -------------------------------------------------------------------------*/
  uint8             loop_cnt;
  uint8             i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MODEM_RCINIT
  rcinit_handshake_startup();
#else
  qmi_pbm_init_before_task_start();
#endif

     /* register with dog*/
  qmi_pbm_dog_rpt_var = dog_hb_register_rex((rex_sigs_type)(1<< QMI_PBM_DOG_RPT_TIMER_SIGNAL));

#ifndef FEATURE_MODEM_RCINIT
  tmc_task_start();
#endif

  qmi_pbmi_init();

  memset(set_sigs, 0, QMI_PBM_SIG_ARR_LEN);

  /*-------------------------------------------------------------------------
    Main task loop. Never exit!
  -------------------------------------------------------------------------*/

  for( ; ; )
  {
    /*-----------------------------------------------------------------------
      Wait for work to do. No return from wait until one of the enabled
      signals is set. Even if we come out of wait, process only those
      signals which are currently enabled.
    -----------------------------------------------------------------------*/
    memscpy(loop_sigs, sizeof(loop_sigs), (const void *)set_sigs, sizeof(set_sigs));
    sig_result = rex_get_sigs_ext(rex_self(), set_sigs);

    if(sig_result == FALSE)
    {
      UIM_MSG_ERR_0("qmi_pbm_task(): Failed to obtain the signals from the QMI TCB");
      ASSERT(0);
      return;
    }

    for(i = 0; i < QMI_PBM_SIG_ARR_LEN; i++)
    {
      set_sigs[i] |= loop_sigs[i];
    }

    sig_result = FALSE;
    for(i = 0; i < QMI_PBM_SIG_ARR_LEN; i++)
    {
      set_sigs[i] &= qmi_pbmi_enabled_sig_mask[i];
      if(0 != set_sigs[i])
      {
        sig_result = TRUE;
      }
    }

    /*-----------------------------------------------------------------------
      Enter into the Wait only if no requested signal events have occurred
    -----------------------------------------------------------------------*/
    if( FALSE == sig_result )
    {
      sig_result = rex_wait_ext(qmi_pbmi_enabled_sig_mask, set_sigs);
      if(sig_result == FALSE)
      {
        UIM_MSG_ERR_0("qmi_pbm_task():rex_wait_ext() failed.");
        ASSERT(0);
        return;
      }

      for(i = 0; i < QMI_PBM_SIG_ARR_LEN; i++)
      {
        set_sigs[i] &= qmi_pbmi_enabled_sig_mask[i];
      }
    }
    sig_result = rex_clr_sigs_ext(rex_self(), set_sigs);
    if(sig_result == FALSE)
    {
      UIM_MSG_ERR_0("qmi_pbm_task():rex_clr_sigs_ext() failed.");
      ASSERT(0);
      return;
    }

    QMI_PBM_CLR_SIG(set_sigs, QMI_PBM_SIG_MASK_CHANGE_SIGNAL);

    for(i = 0; i < QMI_PBM_SIG_ARR_LEN; i++)
    {
      loop_sigs[i] = set_sigs[i];
      set_sigs[i]  = 0;
    }

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority.
      If the loop iterates for more than QMI_PBM_MAX_SIGNALS times, some signal is
      not handled by this loop. So instead of going in to infinite loop
      perform a check
    -----------------------------------------------------------------------*/
    loop_cnt = 0;
    sig_result = TRUE;

    while( sig_result && loop_cnt++ < QMI_PBM_MAX_SIGNALS )
    {
      sig_result = FALSE;
      /*---------------------------------------------------------------------
        If watchdog timer expired, kick the pooch and set timer for next.
      ---------------------------------------------------------------------*/
      if(QMI_PBM_IS_SET_SIG(loop_sigs, QMI_PBM_DOG_RPT_TIMER_SIGNAL))
      {
        QMI_PBM_CLR_SIG(loop_sigs, QMI_PBM_DOG_RPT_TIMER_SIGNAL);
		dog_hb_report(qmi_pbm_dog_rpt_var);
      }
      /*---------------------------------------------------------------------
        QMI Task command queue signal
      ---------------------------------------------------------------------*/
      else if(QMI_PBM_IS_SET_SIG(loop_sigs, QMI_PBM_CMD_Q_SIGNAL ))
      {
        QMI_PBM_CLR_SIG(loop_sigs, QMI_PBM_CMD_Q_SIGNAL);
        if ( qmi_pbmi_process_cmd() == FALSE )
        {
          QMI_PBM_SET_SIG(set_sigs, QMI_PBM_CMD_Q_SIGNAL);
        }
      }
      /*---------------------------------------------------------------------
        Process other signals with registered handlers
      ---------------------------------------------------------------------*/
      /*----------------------------------------------------------------------
        Handle the QMI Service Signal if set
      -----------------------------------------------------------------------*/
      else
      {
        for( i = QMI_PBM_SERVICE_PBM_SIGNAL; i < QMI_PBM_MAX_SIGNALS; i++)
        {
          if(QMI_PBM_IS_SET_SIG(loop_sigs, i ))
          {
            QMI_PBM_CLR_SIG(loop_sigs, i);
            if ( qmi_pbmi_sig_handler[i].sig_handler
                 (
                   (qmi_pbm_sig_enum_type) i ,
                   qmi_pbmi_sig_handler[i].user_data_ptr
                 ) == FALSE )
            {
              UIM_MSG_HIGH_1("qmi_pbm_task(): QMI_PBM_SERVICE_%d_SIGNAL received", i);
              QMI_PBM_SET_SIG(set_sigs, i);
            }
            /*-----------------------------------------------------------------
              allowing one signal type at a time to process which will avoid
              the watchdog timer expiration
            --------------------------------------------------------------------*/
            break;
          }
        }
      }
      /*---------------------------------------------------------------------
        Check if powerdown command signal was set.  If set then clear signal,
        and process task stop procedures.
      ---------------------------------------------------------------------*/
      if(QMI_PBM_IS_SET_SIG(loop_sigs, QMI_PBM_TASK_STOP_SIGNAL))
      {
        UIM_MSG_HIGH_1("qmi_pbm_task(): QMI_PBM_TASK_STOP_SIGNAL received %d", ignored);
        QMI_PBM_CLR_SIG(loop_sigs, QMI_PBM_TASK_STOP_SIGNAL);
        task_stop();
      }
      /*---------------------------------------------------------------------
       Check if offline command signal was set.  If set then clear signal,
       and process task offline procedures.
      ---------------------------------------------------------------------*/
      if(QMI_PBM_IS_SET_SIG(loop_sigs, QMI_PBM_TASK_OFFLINE_SIGNAL))
      {
        UIM_MSG_HIGH_0("qmi_pbm_task(): QMI_PBM_TASK_OFFLINE_SIGNAL received");
        QMI_PBM_CLR_SIG(loop_sigs, QMI_PBM_TASK_OFFLINE_SIGNAL);
        task_offline();
      }

      for(i = 0; i < QMI_PBM_SIG_ARR_LEN; i++)
      {
        if(0 != loop_sigs[i])
        {
          sig_result = TRUE;
        }
      }
    } /* end while */

    for(i = 0; i < QMI_PBM_SIG_ARR_LEN; i++)
    {
      if(0 != loop_sigs[i])
      {
        UIM_MSG_ERR_0("qmi_pbm_task(): Signals not completely processed");
        ASSERT(0);
      }
    }
  } /* forever */
#if defined FEATURE_MODEM_RCINIT && defined FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_MODEM_RCINIT && FEATURE_UIM_TEST_FRAMEWORK */

} /* qmi_pbm_task() */

#else /* FEATURE_REX_SIGS_EXT */

/*===========================================================================
FUNCTION QMI_PBM_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the QMI task.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_pbm_task
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

#if (defined FEATURE_UIM_TEST_FRAMEWORK && defined FEATURE_MODEM_RCINIT)
  #error code not present
#endif

#ifdef FEATURE_MODEM_RCINIT
  rcinit_handshake_startup();
#else
  qmi_pbm_init_before_task_start();
#endif

  /* register with dog*/
  qmi_pbm_dog_rpt_var = dog_hb_register_rex((rex_sigs_type)(1<< QMI_PBM_DOG_RPT_TIMER_SIGNAL));

#ifndef FEATURE_MODEM_RCINIT
  tmc_task_start();
#endif

  qmi_pbmi_init();

#if (defined FEATURE_UIM_TEST_FRAMEWORK && defined FEATURE_MODEM_RCINIT)
  #error code not present
#endif

  /*-------------------------------------------------------------------------
    Main task loop. Never exit!
  -------------------------------------------------------------------------*/
  for( ;;)
  {
#ifdef FEATURE_UIM_TEST_FRAMEWORK
    #error code not present
#endif
    /*-----------------------------------------------------------------------
      Wait for work to do. No return from wait until one of the enabled
      signals is set. Even if we come out of wait, process only those
      signals which are currently enabled.
    -----------------------------------------------------------------------*/
    set_sigs |= rex_get_sigs( rex_self() );
    set_sigs &= qmi_pbmi_enabled_sig_mask;

    /*-----------------------------------------------------------------------
      Enter into the Wait only if no requested signal events have occurred
    -----------------------------------------------------------------------*/
    if( set_sigs == 0 )
    {
      set_sigs = rex_wait( qmi_pbmi_enabled_sig_mask );
      set_sigs &= qmi_pbmi_enabled_sig_mask;
    }
    (void)rex_clr_sigs( rex_self(), set_sigs );


    loop_sigs = set_sigs & ~( (rex_sigs_type) 1
                             << (rex_sigs_type) QMI_PBM_SIG_MASK_CHANGE_SIGNAL );
    set_sigs = 0;

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority.
      If the loop iterates for more than QMI_PBM_MAX_SIGNALS times, some signal
      is not handled by this loop. So instead of going in to infinite loop
      perform a check
    -----------------------------------------------------------------------*/
    loop_cnt = 0;
    while( loop_sigs && loop_cnt++ < (rex_sigs_type) QMI_PBM_MAX_SIGNALS )
    {
      /*---------------------------------------------------------------------
        If watchdog timer expired, kick the pooch and set timer for next.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_PBM_DOG_RPT_TIMER_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type)QMI_PBM_DOG_RPT_TIMER_SIGNAL );
		dog_hb_report(qmi_pbm_dog_rpt_var );
      }
      /*---------------------------------------------------------------------
       *
        QMI Task command queue signal
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_PBM_CMD_Q_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_PBM_CMD_Q_SIGNAL );

        if ( qmi_pbmi_process_cmd() == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_PBM_CMD_Q_SIGNAL );
        }
      }

      /*---------------------------------------------------------------------
        Process other signals with registered handlers
      ---------------------------------------------------------------------*/
      else
      {
        /*----------------------------------------------------------------------
          Handle the QMI Service Signal if set
        -----------------------------------------------------------------------*/
        for( i = QMI_PBM_SERVICE_PBM_SIGNAL; i < QMI_PBM_MAX_SIGNALS; i++)
        {

          if( loop_sigs & ( (rex_sigs_type) 1 <<
                                 (rex_sigs_type) i ) )
          {
            loop_sigs &= ~( (rex_sigs_type) 1 <<
                            (rex_sigs_type) i );

            if ( qmi_pbmi_sig_handler[i].sig_handler
                 (
                   i ,
                   qmi_pbmi_sig_handler[i].user_data_ptr
                 ) == FALSE )
            {
              UIM_MSG_HIGH_1("qmi_pbm_task(): QMI_PBM_SERVICE_%d_SIGNAL %d", i);
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

#if (defined FEATURE_UIM_TEST_FRAMEWORK && defined FEATURE_MODEM_RCINIT)
      #error code not present
#endif

#ifndef FEATURE_MODEM_RCINIT_PHASE2

      /*---------------------------------------------------------------------
        Check if powerdown command signal was set.  If set then clear signal,
        and process task stop procedures.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_PBM_TASK_STOP_SIGNAL ) )
      {
        UIM_MSG_HIGH_1("qmi_pbm_task(): QMI_PBM_TASK_STOP_SIGNAL received %d", ignored);

        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_PBM_TASK_STOP_SIGNAL );

        task_stop();

      }
      /*---------------------------------------------------------------------
        Check if offline command signal was set.  If set then clear signal,
        and process task offline procedures.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_PBM_TASK_OFFLINE_SIGNAL ) )
      {
        UIM_MSG_HIGH_0("qmi_pbm_task(): QMI_PBM_TASK_OFFLINE_SIGNAL received");

        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_PBM_TASK_OFFLINE_SIGNAL );

        task_offline();

      }
#endif
    }

    ASSERT(0 == loop_sigs);
  } /* forever */
#if defined FEATURE_MODEM_RCINIT && defined FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_MODEM_RCINIT && FEATURE_UIM_TEST_FRAMEWORK */

} /* qmi_pbm_task() */

#endif /* FEATURE_REX_SIGS_EXT */