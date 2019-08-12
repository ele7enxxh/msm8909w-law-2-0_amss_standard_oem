/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              Q M I _ M O D E M _ T A S K . C

DESCRIPTION
  This is the source file for the QMI Modem Task. Contained herein are the
  functions needed to initialize all the modules that execute in QMI task
  context and the main task processing loop.

EXTERNALIZED FUNCTIONS 
  qmi_modem_task()
    QMI Task entry point and main processing loop.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI services can be used unless MC has created the QMI task
  and qmi_modem_task() has been invoked.

Copyright (c) 2011-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/qmi_modem_task.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/18/15   skc     Fixed compiler warning
10/01/12    av     Call RCEVT API to signal that the task is up
10/01/11   bvd     Adding rcinit featurization in task function 
01/25/11    rk     Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#include "task.h"
#include "rex.h"
#include "queue.h"
#include "dog_hb_rex.h"
#include "qmi_modem_task.h"
#include "qmi_modem_task_svc.h"
#include "qmi_modem_taski.h"
#include "qmi_modem_task_cmd.h"
#include "ds_qmux_ext.h"
#include "ds_qmi_svc_ext.h"
#include "amssassert.h"
#include "ds_Utils_DebugMsg.h"

#include "rcinit.h"
#include "rcevt_rex.h"
/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

rex_tcb_type  * rex_qmi_modem_tcb;

/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

                           LOCAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================
FUNCTION QMII_INIT()

DESCRIPTION
  Initialization function for the data common ctrl task. This function
  performs the functions needed for the data task to exit disabled state.

DEPENDENCIES
  QMI task should have been started.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void qmii_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize WWAN interface
    - done after task start so USB enumeration is completed hence
      sio_control_open always works, and so init function can read NV.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Initialize QMUX and QMI services
  -------------------------------------------------------------------------*/
  if(qmux_init() != TRUE)
  {
    DATA_ERR_FATAL("qmii_init(): QMUX initialization failed");
  }
} /* qmii_init() */

/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/


/*===========================================================================
FUNCTION QMI_MODEM_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the QMI task.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_modem_task
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
#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  rex_qmi_modem_tcb = rex_self();
#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  rcinit_handshake_startup();

  /*------------------------------------------------------------------------
    Perform the power up initialization for all other modules that run in
    QMI context
  -------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
    Dog heart beat registration.
  -------------------------------------------------------------------------*/
  qmi_dog_rpt_id = dog_hb_register_rex(
                 (rex_sigs_type)1 << (rex_sigs_type)QMI_DOG_HB_REPORT_SIGNAL);

  qmii_init();

  /* Notify that the QMI_MODEM task is ready */
  (void)rcevt_signal_name("QMI_MODEM: READY");

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

    set_sigs |= rex_get_sigs( rex_qmi_modem_tcb );

    set_sigs &= qmii_enabled_sig_mask;

    /*-----------------------------------------------------------------------
      Enter into the Wait only if no requested signal events have occurred
    -----------------------------------------------------------------------*/
    if( set_sigs == 0 )
    {
      set_sigs = rex_wait( qmii_enabled_sig_mask );
      set_sigs &= qmii_enabled_sig_mask;
    }

    (void)rex_clr_sigs( rex_qmi_modem_tcb, set_sigs );

    loop_sigs = set_sigs & ~( (rex_sigs_type) 1
                             << (rex_sigs_type) QMI_SIG_MASK_CHANGE_SIGNAL );
    set_sigs = 0;

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority.
      If the loop iterates for more than QMI_MAX_SIGNALS times, some signal
      is not handled by this loop. So instead of going in to infinite loop
      perform a check
    -----------------------------------------------------------------------*/
    loop_cnt = 0;
    while( loop_sigs && loop_cnt++ < (rex_sigs_type) QMI_MAX_SIGNALS )
    {
      /*---------------------------------------------------------------------
        If watchdog hearbeat signal is received, just report to dog.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_DOG_HB_REPORT_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type)QMI_DOG_HB_REPORT_SIGNAL );
        dog_hb_report( qmi_dog_rpt_id );
      }
      /*---------------------------------------------------------------------
       *
        QMI Task command queue signal
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_CMD_Q_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_CMD_Q_SIGNAL );

        if ( qmii_process_cmd() == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_CMD_Q_SIGNAL );
        }
      }

      /*---------------------------------------------------------------------
        Process other signals with registered handlers
      ---------------------------------------------------------------------*/

      /*----------------------------------------------------------------------
        Handle qmux
      -----------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_QMUX_RX_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_QMUX_RX_SIGNAL );

        if ( qmii_sig_handler[QMI_QMUX_RX_SIGNAL].sig_handler
             (
               QMI_QMUX_RX_SIGNAL ,
               qmii_sig_handler[QMI_QMUX_RX_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_QMUX_RX_SIGNAL);
        }

      }
      else
      {
        /*----------------------------------------------------------------------
          Handle the QMI Service Signal if set
        -----------------------------------------------------------------------*/
        for( i = QMI_QMUX_SERVICE_CTL_SIGNAL; i < QMI_MAX_SIGNALS; i++)
        {
    
          if( loop_sigs & ( (rex_sigs_type) 1 <<
                                 (rex_sigs_type) i ) )
          {
            loop_sigs &= ~( (rex_sigs_type) 1 <<
                            (rex_sigs_type) i );
    
            if ( qmii_sig_handler[i].sig_handler
                 (
                   i ,
                   qmii_sig_handler[i].user_data_ptr
                 ) == FALSE )
            {
              LOG_MSG_INFO2_1("qmi_modem_task(): "
                              "QMI_QMUX_SERVICE_SIGNAL %d", i);
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
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      /*---------------------------------------------------------------------
        Check if offline command signal was set.  If set then clear signal,
        and process task offline procedures.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_MODEM_TASK_OFFLINE_SIGNAL ) )
      {
        LOG_MSG_INFO2_0("qmi_modem_task(): "
                        "QMI_MODEM_TASK_OFFLINE_SIGNAL received");

        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_MODEM_TASK_OFFLINE_SIGNAL );
      }
    }

    ASSERT(0 == loop_sigs);
  } /* forever */

} /* qmi_modem_task() */
