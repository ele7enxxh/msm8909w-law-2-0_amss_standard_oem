/*===========================================================================
  FILE: ds_ontarget_test_task.c

  OVERVIEW: This file provides the DS_ONTARGET_TEST task definitions. It contains
  initialization routines for DS_ONTARGET_TEST task and main task processing loop.

  DEPENDENCIES: None

  Copyright (c) 2009 - 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/ds_ontarget_test_task.c#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2012-09-02 aa   Created module

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ds_ontarget_test_task.h"
#include "ds_ontarget_test_svc.h"
#include "ds_ontarget_test_taski.h"
#include "task.h"
#include "rcinit_rex.h"
#include "queue.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                           PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
void ds_ontarget_test_task
(
  uint32 ignored    /* Parameter received from Main Control task - ignored */
)
{
  rex_sigs_type     loop_sigs;      /* Signals to be processed within a task
                                       loop */
  rex_sigs_type     set_sigs = 0;   /* Currently set signals               */
  uint8             loop_cnt;       /* # of times task loop is iterated
                                       on a signal mask                    */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Do power-up initializtion then wait for start signal (from the MC task,
    which controls task startup sequencing)
  -------------------------------------------------------------------------*/
  ds_ontarget_testi_powerup();

  /*-------------------------------------------------------------------------
    RCInit Handshake
  -------------------------------------------------------------------------*/
  rcinit_handshake_startup(); // required rcinit handshake

  /*-------------------------------------------------------------------------
    Perform after-task-start initialization.
  -------------------------------------------------------------------------*/
  ds_ontarget_testi_init();


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

    set_sigs |= rex_get_sigs(&ds_ontarget_test_tcb);
    set_sigs &= ds_ontarget_testi_enabled_sig_mask;

    /*-----------------------------------------------------------------------
      Enter into the Wait only if no requested signal events have occurred
    -----------------------------------------------------------------------*/
    if (set_sigs == 0)
    {
      set_sigs = rex_wait(ds_ontarget_testi_enabled_sig_mask);
      set_sigs &= ds_ontarget_testi_enabled_sig_mask;
    }

    (void)rex_clr_sigs(&ds_ontarget_test_tcb, set_sigs);

    loop_sigs = set_sigs & ~((rex_sigs_type) 1
                             << (rex_sigs_type) DS_ONTARGET_TEST_SIG_MASK_CHANGE_SIGNAL);
    set_sigs = 0;

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority.
      If the loop iterates for more than DS_ONTARGET_TEST_SIGNAL_MAX times, some signal
      is not handled by this loop. So instead of going in to infinite loop
      perform a check
    -----------------------------------------------------------------------*/
    loop_cnt = 0;
    while (loop_sigs && loop_cnt++ < (rex_sigs_type) DS_ONTARGET_TEST_SIGNAL_MAX)
    {
      /*---------------------------------------------------------------------
        DS_ONTARGET_TEST Task command queue signal
      ---------------------------------------------------------------------*/
      if (loop_sigs & ((rex_sigs_type) 1 <<
                        (rex_sigs_type) DS_ONTARGET_TEST_CMD_Q_SIGNAL))
      {
        loop_sigs &= ~((rex_sigs_type) 1 <<
                        (rex_sigs_type) DS_ONTARGET_TEST_CMD_Q_SIGNAL);

        if (ds_ontarget_testi_process_cmd() == FALSE)
        {
          set_sigs |= ((rex_sigs_type) 1 <<
                        (rex_sigs_type) DS_ONTARGET_TEST_CMD_Q_SIGNAL);
        }
      }
    } /* while there are signals to process during this iteration */

    ASSERT(0 == loop_sigs);
  } /* forever */

} /* ds_ontarget_test_task() */
