/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              P S _ R M _ T A S K . C

DESCRIPTION
  This is the source file for the RM Data processing Task. Contained herein
  are the functions needed to initialize all the modules that execute in RM
  task context and the main task processing loop.


EXTERNALIZED FUNCTIONS
  ps_rm_task()
    RM Task entry point and main processing loop.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the RM services can be used unless MC has created the RM task
  and ps_rm_task() has been invoked.

Copyright (c) 2010-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/ps_rm_task.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/16/15    ds     Added an api check whether ps rm task is initialized or not.
10/01/11   bvd     Adding rcinit featurization 
10/07/10    am     created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#include "queue.h"
#include "err.h"
#include "dog_hb_rex.h"
#include "ps_rm_task.h"
#include "ps_rm_taski.h"
#include "ps_rm_svc.h"
#include "ps_rm_defs.h"

#include "rcinit_rex.h"

#include "ds_Utils_DebugMsg.h"

rex_tcb_type    * rex_ps_rm_tcb;

/*===========================================================================

                           LOCAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================
FUNCTION PS_RM_INIT()

DESCRIPTION
  Initialization function for the rm data processing task. This function
  performs the functions needed for the data task to exit disabled state.

DEPENDENCIES
  RM task should have been started.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_rm_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


} /* ps_rm_init() */

/*===========================================================================
FUNCTION PS_RM_TASK_IS_RUNNING()

DESCRIPTION
  This api checks whether ps rm task is initialized or not and return TRUE if 
  it is running otherwise FALSE.

DEPENDENCIES
  RM task should have been started.

RETURN VALUE
  TRUE/FALSE.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ps_rm_task_is_running
(
  void
)
{

  if (rex_ps_rm_tcb != NULL)
    return TRUE;
  else 
    return FALSE;

} /* is_ps_rm_task_running() */

/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/
/*--------------------------------------------------------------------------
  Defines for DOG. Assuming DOG_DYNAMIC_API feat is ON, since they (core bsp)
  dont allow anymore additiions to dog files.
---------------------------------------------------------------------------*/
dog_report_type   ps_rm_dog_rpt_id           = 0;      /* Initial Safety */

/*===========================================================================
FUNCTION PS_RM_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the RM data task.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_rm_task
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
#ifdef TEST_FRAMEWORK
  #error code not present
#endif
  
  rex_ps_rm_tcb = rex_self();

#ifdef TEST_FRAMEWORK
  #error code not present
#endif
  rcinit_handshake_startup();

  ps_rm_dog_rpt_id = dog_hb_register_rex(
                (rex_sigs_type)1 << (rex_sigs_type)PS_RM_DOG_HB_REPORT_SIGNAL);

  ps_rm_init();

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

    set_sigs |= rex_get_sigs( rex_ps_rm_tcb );

    set_sigs &= ps_rm_enabled_sig_mask;

    /*-----------------------------------------------------------------------
      Enter into the Wait only if no requested signal events have occurred
    -----------------------------------------------------------------------*/
    if( set_sigs == 0 )
    {
      set_sigs = rex_wait( ps_rm_enabled_sig_mask );
      set_sigs &= ps_rm_enabled_sig_mask;
    }

    (void)rex_clr_sigs( rex_ps_rm_tcb, set_sigs );

    loop_sigs = set_sigs & ~( (rex_sigs_type) 1
                             << (rex_sigs_type) PS_RM_SIG_MASK_CHANGE_SIGNAL );
    set_sigs = 0;

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority.
      If the loop iterates for more than DCC_MAX_SIGNALS times, some signal
      is not handled by this loop. So instead of going in to infinite loop
      perform a check
    -----------------------------------------------------------------------*/
    loop_cnt = 0;
    while( loop_sigs && loop_cnt++ < (rex_sigs_type) PS_RM_MAX_SIGNALS )
    {
      /*---------------------------------------------------------------------
        If dog heart beat received, ack the beat. Do ping<->pong.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_DOG_HB_REPORT_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type)PS_RM_DOG_HB_REPORT_SIGNAL );
        dog_hb_report( ps_rm_dog_rpt_id );
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_1 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_1 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_1].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_1 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_1].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_1);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_2 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_2 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_2].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_2 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_2].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_2);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_3 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_3 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_3].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_3 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_3].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_3);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_4 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_4 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_4].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_4 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_4].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_4);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_5 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_5 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_5].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_5 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_5].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_5);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_6 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_6 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_6].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_6 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_6].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_6);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_7 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_7 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_7].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_7 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_7].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_7);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_8 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_8 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_8].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_8 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_8].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_8);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_9 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_9 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_9].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_9 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_9].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_9);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_10 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_10 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_10].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_10 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_10].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_10);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_11 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_11 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_11].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_11 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_11].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_11);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_12 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_12 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_12].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_12 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_12].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_12);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_13 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_13 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_13].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_13 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_13].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_13);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_14 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_14 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_14].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_14 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_14].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_14);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_15 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_15 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_15].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_15 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_15].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_15);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_16 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_16 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_16].sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_16 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_16].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_16);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_MBIM_RX_DATA_Q_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_MBIM_RX_DATA_Q_SIGNAL );

        if ( ps_rm_sig_handler[PS_RM_MBIM_RX_DATA_Q_SIGNAL].sig_handler
             (
               PS_RM_MBIM_RX_DATA_Q_SIGNAL ,
               ps_rm_sig_handler[PS_RM_MBIM_RX_DATA_Q_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_MBIM_RX_DATA_Q_SIGNAL);
        }
      }
      /*---------------------------------------------------------------------
        PS Task command queue signal
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_CMD_Q_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_CMD_Q_SIGNAL );

        if (ps_rm_process_cmd() == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_CMD_Q_SIGNAL );
        }
      }

#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      /*---------------------------------------------------------------------
        Check if offline command signal was set.  If set then clear signal,
        and process task offline procedures.
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_TASK_OFFLINE_SIGNAL ) )
      {
        LOG_MSG_INFO2_0("ps_rm_task(): "
                        "PS_RM_TASK_OFFLINE_SIGNAL received");

        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_TASK_OFFLINE_SIGNAL );
      }
    }

    ASSERT(0 == loop_sigs);
  } /* forever */

} /* ps_rm_task() */
