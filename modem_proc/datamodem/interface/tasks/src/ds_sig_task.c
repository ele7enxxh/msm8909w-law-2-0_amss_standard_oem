/*===========================================================================
  FILE: ds_sig_task.c

  OVERVIEW: This file provides the DS_SIG task definitions. It contains
  initialization routines for DS_SIG task and main task processing loop.

  DEPENDENCIES: None

  Copyright (c) 2009 - 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/ds_sig_task.c#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  03/02/13   pkp Dog heart beat implementation.
  10/01/11   bvd Adding rcinit featurization
  01/10/11   ss  Cleaning up of Globals for thread safety in Q6 Free 
                 Floating environment.
  2010-09-27 ss  DYNAMIC WDOG support for DATA tasks.
  2010-03-16 sy  Changed task_* api to tmc_task* api.
  2009-08-17 hm  Created module.

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_sig_task.h"
#include "ds_sig_svc.h"
#include "ds_sig_taski.h"
#include "task.h"
#include "dog_hb_rex.h"
#include "rex.h"
#include "queue.h"
#include "dog_hb_rex.h"
#include "ds_Utils_DebugMsg.h"

#include "rcinit_rex.h"

/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#ifdef FEATURE_MODEM_RCINIT_PHASE2
rex_tcb_type   * rex_ds_sig_tcb;
#endif
/*===========================================================================

                           PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
void ds_sig_task
(
  uint32 ignored    /* Parameter received from Main Control task - ignored */
)
{
  rex_sigs_type     loop_sigs;      /* Signals to be processed within a task
                                       loop */
  rex_sigs_type     set_sigs = 0;   /* Currently set signals               */
  uint8             loop_cnt;       /* # of times task loop is iterated
                                       on a signal mask                    */
  dog_report_type   ds_sig_dog_rpt_id = 0;	  /* Initial Safety */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  rex_ds_sig_tcb = rex_self();
  rcinit_handshake_startup();
  ds_sig_dog_rpt_id = dog_hb_register_rex(
                      (rex_sigs_type) 1 << (int)DS_SIG_DOG_HB_REPORT_SIGNAL);

  /*-------------------------------------------------------------------------
    Perform after-task-start initialization.
  -------------------------------------------------------------------------*/
  ds_sigi_init();


  /*-------------------------------------------------------------------------
    Main task loop. Never exit!
  -------------------------------------------------------------------------*/
  for (;;)
  {
    /*-----------------------------------------------------------------------
      Wait for work to do. No return from wait until one of the enabled
      signals is set. Even if we come out of wait, process only those
      signals which are currently enabled.
    -----------------------------------------------------------------------*/

    set_sigs |= rex_get_sigs(rex_ds_sig_tcb);

    set_sigs &= ds_sigi_enabled_sig_mask;

    /*-----------------------------------------------------------------------
      Enter into the Wait only if no requested signal events have occurred
    -----------------------------------------------------------------------*/
    if (set_sigs == 0)
    {
      set_sigs = rex_wait(ds_sigi_enabled_sig_mask);
      set_sigs &= ds_sigi_enabled_sig_mask;
    }

    (void)rex_clr_sigs(rex_ds_sig_tcb, set_sigs);

    loop_sigs = set_sigs & ~((rex_sigs_type) 1
                             << (rex_sigs_type) DS_SIG_SIG_MASK_CHANGE_SIGNAL);
    set_sigs = 0;

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority.
      If the loop iterates for more than DS_SIG_SIGNAL_MAX times, some signal
      is not handled by this loop. So instead of going in to infinite loop
      perform a check
    -----------------------------------------------------------------------*/
    loop_cnt = 0;
    while (loop_sigs && loop_cnt++ < (rex_sigs_type) DS_SIG_SIGNAL_MAX)
    {
      /*---------------------------------------------------------------------
        If dog heart beat is received, ack the beat. Play ping<->pong.
      ---------------------------------------------------------------------*/
      if (loop_sigs & ((rex_sigs_type) 1 <<
                        (rex_sigs_type) DS_SIG_DOG_HB_REPORT_SIGNAL))
      {
        loop_sigs &= ~((rex_sigs_type) 1 <<
                        (rex_sigs_type)DS_SIG_DOG_HB_REPORT_SIGNAL);

        dog_hb_report(ds_sig_dog_rpt_id);
      }
      /*---------------------------------------------------------------------
        DS_SIG Task command queue signal
      ---------------------------------------------------------------------*/
      if (loop_sigs & ((rex_sigs_type) 1 <<
                        (rex_sigs_type) DS_SIG_CMD_Q_SIGNAL))
      {
        loop_sigs &= ~((rex_sigs_type) 1 <<
                        (rex_sigs_type) DS_SIG_CMD_Q_SIGNAL);

        if (ds_sigi_process_cmd() == FALSE)
        {
          set_sigs |= ((rex_sigs_type) 1 <<
                        (rex_sigs_type) DS_SIG_CMD_Q_SIGNAL);
        }
      }

#ifdef TEST_FRAMEWORK
      #error code not present
#endif
      /*---------------------------------------------------------------------
        Check if offline command signal was set.  If set then clear signal,
        and process task offline procedures.
      ---------------------------------------------------------------------*/
      if (loop_sigs & ((rex_sigs_type) 1 <<
                        (rex_sigs_type) DS_SIG_TASK_OFFLINE_SIGNAL))
      {
        LOG_MSG_INFO1_0("ds_sig_task(): "
                        "DS_SIG_TASK_OFFLINE_SIGNAL received");

        loop_sigs &= ~((rex_sigs_type) 1 <<
                        (rex_sigs_type) DS_SIG_TASK_OFFLINE_SIGNAL);
      }
    } /* while there are signals to process during this iteration */

    ASSERT(0 == loop_sigs);
  } /* forever */

} /* ds_sig_task() */
