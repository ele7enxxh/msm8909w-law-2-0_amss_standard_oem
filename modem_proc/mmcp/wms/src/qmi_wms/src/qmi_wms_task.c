/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              Q M I _ WMS _ T A S K . C

DESCRIPTION
  This is the source file for the QMI WMS Task. Contained herein are the
  functions needed to initialize all the modules that execute in QMI task
  context and the main task processing loop.

EXTERNALIZED FUNCTIONS
  qmi_wms_task()
    QMI Task entry point and main processing loop.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI services can be used unless MC has created the QMI task
  and qmi_wms_task() has been invoked.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/qmi_wms/src/qmi_wms_task.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/11    rk     Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "task.h"
#include "rex.h"
#include "queue.h"
#include "dog_hb_rex.h"
#include "rcinit.h"
#include "qmi_wms_task.h"
#include "qmi_wms_task_svc.h"
#include "qmi_wms_taski.h"
#include "amssassert.h"
#include "msg.h"
#include "qmi_wms.h"
#include "wmsutils.h"


/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

 rex_tcb_type   *qmi_wms_tcb_ptr = NULL; // pointer for myself

  /*--------------------------------------------------------------------------
    Timers needed for the QMI Ctrl Task.
  ---------------------------------------------------------------------------*/
  /*--------------------------------------------------------------------------
    Dynamic dog variable
  ---------------------------------------------------------------------------*/
  dog_report_type  qmi_wms_dog_rpt_var = 0;           /* Dog report ID       */

boolean qmi_wms_task_stop_sig_rcvd = FALSE;


extern boolean qmi_wms_sig_hdlr( qmi_wms_sig_enum_type, void * );

/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

                           LOCAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================
FUNCTION qmi_wms_task_set_stop_sig_rcvd

DESCRIPTION
  SET the global which tell if TASK_STOP_SIGNAL has come or not

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_wms_task_set_stop_sig_rcvd(
  boolean is_received
)
{
  qmi_wms_task_stop_sig_rcvd = is_received;
  MSG_HIGH_1( "qmi_wms_task_stop_sig_rcvd set to %d", qmi_wms_task_stop_sig_rcvd );
} /* qmi_wms_task_set_stop_sig_rcvd() */

/*===========================================================================
FUNCTION QMI_WMSI_INIT()

DESCRIPTION
  Initialization function for the various QMI services.

DEPENDENCIES
  QMI WMS task should have been started already.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void qmi_wmsi_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  qmi_wms_tcb_ptr = rex_self();

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  {
    /* Register for QMI_WMS_TASK_STOP_SIGNAL */
    RCINIT_GROUP        qmi_wms_group;
    RCEVT_SIGEX_SIGREX  qmi_wms_stop_sig;

    qmi_wms_stop_sig.signal = qmi_wms_tcb_ptr;    // REX Task Context
    qmi_wms_stop_sig.mask   = (rex_sigs_type)(1 << QMI_WMS_TASK_STOP_SIGNAL); // REX signal mask

    qmi_wms_group = rcinit_lookup_group_rextask(qmi_wms_tcb_ptr);
    rcinit_register_term_group(qmi_wms_group, RCEVT_SIGEX_TYPE_SIGREX, &qmi_wms_stop_sig);
  }
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

  // Initialize global TASK_STOP_SIGNAL
  qmi_wms_task_set_stop_sig_rcvd(FALSE);
} /* qmi_wmsi_init() */

/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/
  /*===========================================================================

  FUNCTION qmi_wms_init_before_task_start

  DESCRIPTION
    Initialize qmi wms cmd queues and other objects that needs to be
    initialized before the task is started

    NOTE! Any initialization that must be done before other tasks or clients
    tries to contact QMI WMS, must be done in this initialization function.

  DEPENDENCIES
    none

  RETURN VALUE
    none

  SIDE EFFECTS
    none

  ===========================================================================*/
void qmi_wms_init_before_task_start
(
  void
)
{
    /*-------------------------------------------------------------------------
      Do power-up initializtion then wait for start signal (from the MC task,
      which controls task startup sequencing)
    -------------------------------------------------------------------------*/
    qmi_wmsi_powerup();
}

#ifdef FEATURE_REX_SIGS_EXT
/*===========================================================================
FUNCTION QMI_WMS_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the QMI-WMS
  task.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_wms_task
(
  uint32 ignored    /* Parameter received from Main Control task - ignored */
)
{
  /*-------------------------------------------------------------------------
    Signals to be processed within a task loop
  -------------------------------------------------------------------------*/
  rex_sigs_type     loop_sigs[QMI_WMS_SIG_ARR_LEN];
  rex_sigs_type     set_sigs[QMI_WMS_SIG_ARR_LEN]; /* Currently set signals */

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
  qmi_wms_init_before_task_start();
#endif

  /* register with dog*/
  qmi_wms_dog_rpt_var = dog_hb_register_rex((rex_sigs_type)(1<< QMI_WMS_DOG_RPT_TIMER_SIGNAL));

  qmi_wmsi_init();

  memset(set_sigs, 0, QMI_WMS_SIG_ARR_LEN);

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
    memscpy(loop_sigs, QMI_WMS_SIG_ARR_LEN,(const void *)set_sigs, QMI_WMS_SIG_ARR_LEN);
    sig_result = rex_get_sigs_ext(rex_self(), set_sigs);

    if(sig_result == FALSE)
    {
      MSG_ERROR_0("qmi_wms_task(): Failed to obtain the signals from the QMI TCB");
      ASSERT(0);
      return;
    }

    for(i = 0; i < QMI_WMS_SIG_ARR_LEN; i++)
    {
      set_sigs[i] |= loop_sigs[i];
    }

    sig_result = FALSE;
    for(i = 0; i < QMI_WMS_SIG_ARR_LEN; i++)
    {
      set_sigs[i] &= qmi_wmsi_enabled_sig_mask[i];
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
      sig_result = rex_wait_ext(qmi_wmsi_enabled_sig_mask, set_sigs);
      if(sig_result == FALSE)
      {
        MSG_ERROR_0("qmi_wms_task():rex_wait_ext() failed");
        ASSERT(0);
        return;
      }

      for(i = 0; i < QMI_WMS_SIG_ARR_LEN; i++)
      {
        set_sigs[i] &= qmi_wmsi_enabled_sig_mask[i];
      }
    }
    sig_result = rex_clr_sigs_ext(rex_self(), set_sigs);
    if(sig_result == FALSE)
    {
      MSG_ERROR_0("qmi_wms_task():rex_clr_sigs_ext() failed");
      ASSERT(0);
      return;
    }

    QMI_WMS_CLR_SIG(set_sigs, QMI_WMS_SIG_MASK_CHANGE_SIGNAL);

    for(i = 0; i < QMI_WMS_SIG_ARR_LEN; i++)
    {
      loop_sigs[i] = set_sigs[i];
      set_sigs[i]  = 0;
    }

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority.
      If the loop iterates for more than QMI_WMS_MAX_SIGNALS times, some signal is
      not handled by this loop. So instead of going in to infinite loop
      perform a check
    -----------------------------------------------------------------------*/
    loop_cnt = 0;
    sig_result = TRUE;

    while( sig_result && loop_cnt++ < QMI_WMS_MAX_SIGNALS )
    {
      sig_result = FALSE;
      /*---------------------------------------------------------------------
        If watchdog timer expired, kick the pooch and set timer for next.
      ---------------------------------------------------------------------*/
      if(QMI_WMS_IS_SET_SIG(loop_sigs, QMI_WMS_DOG_RPT_TIMER_SIGNAL))
      {
        QMI_WMS_CLR_SIG(loop_sigs, QMI_WMS_DOG_RPT_TIMER_SIGNAL);
        dog_hb_report(qmi_wms_dog_rpt_var);
      }
      /*---------------------------------------------------------------------
        Process other signals with registered handlers
      ---------------------------------------------------------------------*/
      /*----------------------------------------------------------------------
        Handle the QMI Service Signal if set
      -----------------------------------------------------------------------*/
      else
      {
        for( i = QMI_WMS_SERVICE_WMS_SIGNAL; i < QMI_WMS_MAX_SIGNALS; i++)
        {
          if(QMI_WMS_IS_SET_SIG(loop_sigs, i ))
          {
            QMI_WMS_CLR_SIG(loop_sigs, i);

            if ( qmi_wms_sig_hdlr((qmi_wms_sig_enum_type) i, NULL) == FALSE )
            {
              MSG_HIGH_1("qmi_wms_task(): QMI_WMS_SERVICE_%d_SIGNAL received", i);
              QMI_WMS_SET_SIG(set_sigs, i);
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
      if(QMI_WMS_IS_SET_SIG(loop_sigs, QMI_WMS_TASK_STOP_SIGNAL))
      {
        MSG_HIGH_1("qmi_wms_task(): QMI_WMS_TASK_STOP_SIGNAL received %d", ignored);
        QMI_WMS_CLR_SIG(loop_sigs, QMI_WMS_TASK_STOP_SIGNAL);

        /* deregister with dog */       
        dog_hb_deregister( qmi_wms_dog_rpt_var );

        // Set the global TASK_STOP_SIGNAL = TRUE
        qmi_wms_task_set_stop_sig_rcvd(TRUE);
        
#ifndef FEATURE_MODEM_RCINIT_PHASE2
        task_stop();
#else /* FEATURE_MODEM_RCINIT_PHASE2 */
        {
          /* Deregister QMI_WMS_TASK_STOP_SIGNAL */
          RCINIT_GROUP        qmi_wms_group;
          RCEVT_SIGEX_SIGREX  qmi_wms_stop_sig;

          qmi_wms_stop_sig.signal = qmi_wms_tcb_ptr;    // REX Task Context
          qmi_wms_stop_sig.mask   = (rex_sigs_type)(1 << QMI_WMS_TASK_STOP_SIGNAL); // REX signal mask

          qmi_wms_group = rcinit_lookup_group_rextask(qmi_wms_tcb_ptr);
          rcinit_unregister_term_group(qmi_wms_group, RCEVT_SIGEX_TYPE_SIGREX, &qmi_wms_stop_sig);

          rcinit_handshake_term();

          loop_sigs = 0;

          return;
        }
#endif // FEATURE_MODEM_RCINIT_PHASE2
      }
      /*---------------------------------------------------------------------
       Check if offline command signal was set.  If set then clear signal,
       and process task offline procedures.
      ---------------------------------------------------------------------*/
      if(QMI_WMS_IS_SET_SIG(loop_sigs, QMI_WMS_TASK_OFFLINE_SIGNAL))
      {
        MSG_HIGH_0("qmi_wms_task(): QMI_WMS_TASK_OFFLINE_SIGNAL received");
        QMI_WMS_CLR_SIG(loop_sigs, QMI_WMS_TASK_OFFLINE_SIGNAL);
#ifndef FEATURE_MODEM_RCINIT_PHASE2
        task_offline();
#endif // FEATURE_MODEM_RCINIT_PHASE2
      }


      for(i = 0; i < QMI_WMS_SIG_ARR_LEN; i++)
      {
        if(0 != loop_sigs[i])
        {
          sig_result = TRUE;
        }
      }
    } /* end while */

    for(i = 0; i < QMI_WMS_SIG_ARR_LEN; i++)
    {
      if(0 != loop_sigs[i])
      {
        MSG_ERROR_0("qmi_wms_task(): Signals not completely processed");
        ASSERT(0);
      }
    }
  } /* forever */

} /* qmi_wms_task() */

#else /* FEATURE_REX_SIGS_EXT */

/*===========================================================================
FUNCTION QMI_WMS_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the QMI task.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_wms_task
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
  qmi_wms_init_before_task_start();
#endif

  /* register with dog*/
  qmi_wms_dog_rpt_var = dog_hb_register_rex((rex_sigs_type)(1<< QMI_WMS_DOG_RPT_TIMER_SIGNAL));

  qmi_wmsi_init();

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
    set_sigs &= qmi_wmsi_enabled_sig_mask;

    /*-----------------------------------------------------------------------
      Enter into the Wait only if no requested signal events have occurred
    -----------------------------------------------------------------------*/
    if( set_sigs == 0 )
    {
      set_sigs = rex_wait( qmi_wmsi_enabled_sig_mask );
      set_sigs &= qmi_wmsi_enabled_sig_mask;
    }
    (void)rex_clr_sigs( rex_self(), set_sigs );


    loop_sigs = set_sigs & ~( (rex_sigs_type) 1
                             << (rex_sigs_type) QMI_WMS_SIG_MASK_CHANGE_SIGNAL );
    set_sigs = 0;

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority.
      If the loop iterates for more than QMI_WMS_MAX_SIGNALS times, some signal
      is not handled by this loop. So instead of going in to infinite loop
      perform a check
    -----------------------------------------------------------------------*/
    loop_cnt = 0;
    while( loop_sigs && loop_cnt++ < (rex_sigs_type) QMI_WMS_MAX_SIGNALS )
    {
      /*---------------------------------------------------------------------
        If watchdog timer expired, kick the pooch and set timer for next.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_WMS_DOG_RPT_TIMER_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type)QMI_WMS_DOG_RPT_TIMER_SIGNAL );
        dog_hb_report(qmi_wms_dog_rpt_var);
      }

      /*---------------------------------------------------------------------
        Process other signals with registered handlers
      ---------------------------------------------------------------------*/
      else
      {
        /*----------------------------------------------------------------------
          Handle the QMI Service Signal if set
        -----------------------------------------------------------------------*/
        for( i = QMI_WMS_SERVICE_WMS_SIGNAL; i < QMI_WMS_MAX_SIGNALS; i++)
        {

          if( loop_sigs & ( (rex_sigs_type) 1 <<
                                 (rex_sigs_type) i ) )
          {
            loop_sigs &= ~( (rex_sigs_type) 1 <<
                            (rex_sigs_type) i );

            if ( qmi_wms_sig_hdlr((qmi_wms_sig_enum_type) i, NULL) == FALSE)
            {
              MSG_HIGH_1("qmi_wms_task(): QMI_WMS_SERVICE_%d_SIGNAL %d", i);
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

      /*---------------------------------------------------------------------
        Check if powerdown command signal was set.  If set then clear signal,
        and process task stop procedures.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_WMS_TASK_STOP_SIGNAL ) )
      {
        MSG_HIGH_1("qmi_wms_task(): QMI_WMS_TASK_STOP_SIGNAL received %d", ignored);

        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_WMS_TASK_STOP_SIGNAL );
        /* deregister with dog */
        dog_hb_deregister( qmi_wms_dog_rpt_var );

        // Set the global TASK_STOP_SIGNAL = TRUE
        qmi_wms_task_set_stop_sig_rcvd(TRUE);

#ifndef FEATURE_MODEM_RCINIT_PHASE2
        task_stop();
#else /* FEATURE_MODEM_RCINIT_PHASE2 */
        {
          /* Deregister QMI_WMS_TASK_STOP_SIGNAL */
          RCINIT_GROUP        qmi_wms_group;
          RCEVT_SIGEX_SIGREX  qmi_wms_stop_sig;

          qmi_wms_stop_sig.signal = qmi_wms_tcb_ptr;    // REX Task Context
          qmi_wms_stop_sig.mask   = (rex_sigs_type)(1 << QMI_WMS_TASK_STOP_SIGNAL); // REX signal mask

          qmi_wms_group = rcinit_lookup_group_rextask(qmi_wms_tcb_ptr);
          rcinit_unregister_term_group(qmi_wms_group, RCEVT_SIGEX_TYPE_SIGREX, &qmi_wms_stop_sig);

          rcinit_handshake_term();

          loop_sigs = 0;

          return;
        }
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

      }
      /*---------------------------------------------------------------------
        Check if offline command signal was set.  If set then clear signal,
        and process task offline procedures.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_WMS_TASK_OFFLINE_SIGNAL ) )
      {
        MSG_HIGH_0("qmi_wms_task(): QMI_WMS_TASK_OFFLINE_SIGNAL received");

        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_WMS_TASK_OFFLINE_SIGNAL );

#ifndef FEATURE_MODEM_RCINIT_PHASE2
        task_offline();
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

      }


    }

    ASSERT(0 == loop_sigs);
  } /* forever */

} /* qmi_wms_task() */
#endif /* FEATURE_REX_SIGS_EXT */
