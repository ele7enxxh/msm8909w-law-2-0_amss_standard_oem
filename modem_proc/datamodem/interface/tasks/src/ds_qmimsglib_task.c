/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            Q M I   T A S K

GENERAL DESCRIPTION
  This software unit contains the task entry point and the top-level dispatch
  routines for the Data Services QMI Task. 

EXTERNALIZED FUNCTIONS
  qmi_task()
    QMI Task entry point

EXTERNALIZED FUNCTIONS INTERNAL TO DS SUBSYSTEM

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2008,2010 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/ds_qmimsglib_task.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/19/10   sy      Changed task_* api to tmc_task* api. 
03/10/10   ar      Removed watchdog elements
07/28/08   ar      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "msg.h"
#include "err.h"
#include "rex.h"
#include "task.h"
#include "tmc.h"
#include "ds_qmimsglib_task.h"
#include "ds_qmimsglib_task_sig.h"
#include "qmi_platform_qmux_io.h"
#include "rcinit_rex.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
extern rex_tcb_type qmi_tcb;



/*===========================================================================

              EXTERNAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION QMI_POWERUP_INIT

DESCRIPTION
  This function performs all the power-up initialization for the QMI Task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  qmi_powerup_init( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* qmi_powerup_init() */


/*===========================================================================

FUNCTION QMI_WAIT

DESCRIPTION
  This function suspends execution of the QMI task until one (or
  more) signals from the requested signal mask is set.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask returned by rex_wait().

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type  qmi_wait
(
   rex_sigs_type  requested_signal_mask       /* Signal mask to suspend on */
)
{
  rex_sigs_type  set_signals;     /* Signal mask upon return from rex wait */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Wait for a signal to be set.
  -------------------------------------------------------------------------*/
  do
  {
    set_signals = rex_wait( requested_signal_mask );
  } while( (set_signals & requested_signal_mask) == 0 );

  return( set_signals );

} /* qmi_wait() */


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION QMI_TASK

DESCRIPTION
  This is the entry point for the QMI Task. This function contains
  the main processing loop that waits for events (signals or commands) and
  dispatches each event to the appropriate entity for further processing.

DEPENDENCIES
  None

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None

===========================================================================*/
void  qmi_task
(
  dword ignored
)
{
  rex_sigs_type   ret_val=0;        /* return value for rex_clr_sigs       */
  rex_sigs_type   requested_sigs;   /* Signal mask to suspend on           */
  rex_sigs_type   set_sigs;         /* Signals set upon return from wait   */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Do task initialization. The init function performs all the task-level
    initialization.
  -------------------------------------------------------------------------*/
  
  
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  RCEVT_SIGEX_SIGOSAL term_sig; /* Termination Signal to wait on*/
  qmi_tcb = rex_self();
  term_sig.signal = qmi_tcb;
  term_sig.mask = TASK_STOP_SIG;

  /* Register for the STOP Signal */
  rcinit_register_term_group(RCINIT_GROUP_5, 
                             RCEVT_SIGEX_TYPE_SIGOSAL, 
                             &term_sig);
  rcinit_handshake_startup();
#else
  /*-------------------------------------------------------------------------
    Do power-up initializtion then wait for start signal (from the MC task,
    which controls task startup sequencing)
  -------------------------------------------------------------------------*/
  qmi_powerup_init();
#endif


  /*-------------------------------------------------------------------------
    ACK task creation and wait for the start signal from task controller.
  -------------------------------------------------------------------------*/
#ifndef FEATURE_MODEM_RCINIT_PHASE2
  /*-------------------------------------------------------------------------
    Wait for the task start signal from TMC.
   -------------------------------------------------------------------------*/
  tmc_task_start();
#endif
  
  /*-------------------------------------------------------------------------
    Signal mask to suspend on is the combination of all the signals requested
    by each of the sub-tasks.
  -------------------------------------------------------------------------*/
  requested_sigs = QMI_TASK_STOP_SIG    | 
                   QMI_TASK_OFFLINE_SIG |
                   QMI_QMUX_RX_SIG;

  /*-------------------------------------------------------------------------
    Main task loop, never exits.
  -------------------------------------------------------------------------*/
  for( ;; )
  {
    /*----------------------------------------------------------------------
      Wait for one of the specified signals to be set. 
    -----------------------------------------------------------------------*/
    set_sigs = qmi_wait( requested_sigs );
    
    /*----------------------------------------------------------------------
    We used to individually clear the wrong set of signals and some signals
    were getting lost.  Here, we clear qmi_tcb with set_sigs.  set_sigs is
    not altered.
    ----------------------------------------------------------------------*/
    ret_val = rex_clr_sigs( &qmi_tcb, set_sigs );

    if( !(ret_val & set_sigs) )
    {
      MSG_MED("qmi_task(): "
              " The previously set signal was not set",0,0,0);
    }
    
    /*----------------------------------------------------------------------
      Sometimes we see signals that were not requested in wait being set.
      So we mask returned sigs with requested sigs
    ----------------------------------------------------------------------*/
    set_sigs &= requested_sigs;

    /*----------------------------------------------------------------------
      If any of the task signals were received, invoke the function to ACK
      task conroller.
    -----------------------------------------------------------------------*/
    if( (set_sigs & QMI_TASK_STOP_SIG) != 0 )
    {
      tmc_task_stop();
    }

    if( (set_sigs & QMI_TASK_OFFLINE_SIG) != 0 )
    {
      tmc_task_offline();
    }

    /*----------------------------------------------------------------------
      Handle QMUX
    -----------------------------------------------------------------------*/
    if( (set_sigs & QMI_QMUX_RX_SIG) != 0 )
    {
      (void)qmi_qmux_amss_process_rx_msgs();
    }

  } /* forever */
} /* qmi_task() */
