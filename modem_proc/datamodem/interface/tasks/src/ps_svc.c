/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               P S _ S V C . C

GENERAL DESCRIPTION
  This is the header file for the Data protocol Task. Contained herein are
  all the definitions, functions, and structures for other software tasks
  to communicate with the Data Protocol Task.

EXTERNALIZED FUNCTIONS
  ps_enable_sig()
    Allow a signal to be processed

  ps_disable_sig()
    Disallow a signal to be processed

  ps_set_sig_handler()
    Set a handler to process a signal in PS context

  ps_send_cmd()
    Post a command to the Data Protocol Task

  ps_set_cmd_handler()
    Set a handler to process a cmd in PS context

Copyright (c) 2002-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_svc.c_v   1.2   19 Nov 2002 17:29:10   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/ps_svc.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
05/18/12    msh    RCINIT PHASE 2 feature change in ps_wait
05/06/11    hm     Use ps_task_crit_section for all PS_task operations. 
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
09/27/10    ss     DYNAMIC DOG support for DATA tasks.
08/11/10    ss     Rearchitectured PS extension signal mechanism. Any number
                   of signals supported now.
04/27/09    pp     Protect ps_ext1_sig_mask under critical section always.
09/03/09    pp     Added ps_wait, ps_wait_ext APIs.
03/26/09    pp     CMI De-featurization.
11/28/08    pp     ps_send_cmd: Use of PS_ENTER_CRIT_SECTION/UNLOCK macros.
10/24/08    pp     Removed unused variable.
09/07/07    scb    Added REX signal ext code under FEATURE_REX_SIGS_EXT
09/19/06    msr    Using atomic operations in ps_send_cmd()
09/12/06    jd     Support for multiplexing RX signals 32-63 on generic rx sig
04/30/04    mct    Fixed some lint errors.
11/19/02    jd     Added global ps_nv_status for use in PS task
11/14/02    jd     Added global ps_nv_item for use in PS task
03/18/02    usb    Created Initial Version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "amssassert.h"
#include "queue.h"
#include "msg.h"
#include "err.h"
#include "ps_svc.h"
#include "ps.h"
#include "psi.h"
#include "dog_hb_rex.h"
#include "ps_utils.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"
#include "stringl.h"
#include "rcinit_rex.h"


/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
  This array stores the extension signals. It extends the signal limit upto 
  the maximum specified via PS_MAX_SIGNALS.  
  This definition is common irrespective of whether FEATURE_REX_SIGS_EXT is
  turned on or off. The array length (PS_EXT_SIG_ARR_LEN) is calculated such
  as to account for the difference.
---------------------------------------------------------------------------*/
/*lint -e506 */
rex_sigs_type ps_ext_sig_mask[PS_EXT_SIG_ARR_LEN];

#ifdef __cplusplus
extern "C" rex_tcb_type  ps_tcb;
#endif

#ifdef FEATURE_MODEM_RCINIT_PHASE2
extern rex_tcb_type * rex_ps_tcb;
#endif
/*===========================================================================

                           FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION PS_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask
  which the PS task uses for rex_wait()

DEPENDENCIES
  A valid signal handler should be registered with PS before enabling the
  signal.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ps_enable_sig
(
  ps_sig_enum_type sig               /* Signal to be enabled               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  PS_ENTER_CRIT_SECTION(&ps_task_crit_section);
  if ( psi_sig_handler[sig].sig_handler == default_sig_handler )
  {
    LOG_MSG_ERROR_1("ps_enable_sig(): "
                    "Signal %d enabled without a handler", sig );
  }

  PS_SET_SIG(psi_enabled_sig_mask, sig);

  /*------------------------------------------------------------------------
    To enable multiplexing signal mask.
  -------------------------------------------------------------------------*/
  if((int)sig >= PS_MIN_EXT_SIGNAL)
  {
    PS_SET_SIG(psi_enabled_sig_mask, PS_EXT1_RX_SIGNAL);
#if defined(TEST_FRAMEWORK) && !defined(FEATURE_MODEM_RCINIT_PHASE2)
    #error code not present
#endif
  }

#if defined(TEST_FRAMEWORK) && !defined(FEATURE_MODEM_RCINIT_PHASE2)
  #error code not present
#endif
  PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);
} /* ps_enable_sig() */


/*===========================================================================

FUNCTION PS_DISABLE_SIG()

DESCRIPTION
  This function disables a signal handling by removing it from the
  currently enabled signal mask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ps_disable_sig
(
  ps_sig_enum_type sig                        /* Signal to be disabled     */
)
{
  uint32 i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  PS_ENTER_CRIT_SECTION(&ps_task_crit_section);
  PS_CLR_SIG(psi_enabled_sig_mask, sig);

  /*-------------------------------------------------------------------------
    Disable the PS_EXT1_RX_SIGNAL signal if all extension signals have been 
    disabled.
  -------------------------------------------------------------------------*/
  /*lint -e506 */
  for(i=PS_SIG_ARR_LEN; i<PS_MAX_SIG_ARR_LEN; i++)
  {
    if(psi_enabled_sig_mask[i] != 0)
    {
      PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);
      return;
    }
  }  
  PS_CLR_SIG(psi_enabled_sig_mask, PS_EXT1_RX_SIGNAL);
  PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);
} /* ps_disable_sig() */


/*===========================================================================

FUNCTION PS_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered signal handler

SIDE EFFECTS
  None.

===========================================================================*/
ps_sig_handler_type ps_set_sig_handler
(
  ps_sig_enum_type sig,              /* Signal assocaited with handler     */
  ps_sig_handler_type sig_handler,   /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
)
{
  ps_sig_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  PS_ENTER_CRIT_SECTION(&ps_task_crit_section);

  ASSERT( sig_handler != NULL );

  old_handler = psi_sig_handler[sig].sig_handler;

  psi_sig_handler[sig].sig_handler = sig_handler;
  psi_sig_handler[sig].user_data_ptr = user_data_ptr;
  PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);

  return old_handler;
} /* ps_set_sig_handler() */

/*===========================================================================

FUNCTION PS_RESET_SIG_HANDLER()

DESCRIPTION
  This function resets a handler for the specified signal

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ps_reset_sig_handler
(
  ps_sig_enum_type sig
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  PS_ENTER_CRIT_SECTION(&ps_task_crit_section);

  psi_sig_handler[sig].sig_handler = default_sig_handler;
  psi_sig_handler[sig].user_data_ptr = NULL;

  PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);
} /* ps_set_sig_handler() */


/*===========================================================================

FUNCTION PS_SEND_CMD()

DESCRIPTION
  This function posts a cmd for processing in PS task context.  The cmd is
  processed by calling the registered cmd handler, if any.

  NOTE: The passed command will be copied to a PS task
  command item local to the PS Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_send_cmd
(
  ps_cmd_enum_type cmd,          /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
  psi_cmd_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a cmd buffer from the free queue, call error routine if no links
    available, else copy buffer to cmd queue and signal PS task
  -------------------------------------------------------------------------*/
  if( ( cmd_ptr = ( psi_cmd_type * ) q_get( &psi_cmd_free_q ) ) == NULL )
  {
    LOG_MSG_ERROR_0("ps_send_cmd(): "
                    "PS task cmd buffers unavailable" );
  }
  else
  {
    cmd_ptr->cmd = cmd;
    cmd_ptr->user_data_ptr = user_data_ptr;

    /*-----------------------------------------------------------------------
      Making the operation atomic so that race conditions do not occur
    -----------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION(&ps_task_crit_section);
    q_put( &psi_cmd_q, &cmd_ptr->link );
    PS_SET_SIGNAL( PS_CMD_Q_SIGNAL );
    PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);
  }
} /* ps_send_cmd() */


/*===========================================================================

FUNCTION PS_SET_CMD_HANDLER()

DESCRIPTION
  This function sets a handler for the specified cmd type.  If a NULL
  function pointer is passed the cmd handling is disabled.

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered cmd handler

SIDE EFFECTS
  None.

===========================================================================*/
ps_cmd_handler_type ps_set_cmd_handler
(
  ps_cmd_enum_type cmd,              /* Command name                      */
  ps_cmd_handler_type cmd_handler    /* Cmd handler function pointer      */
)
{
  ps_cmd_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&ps_task_crit_section);
  old_handler = psi_cmd_handler[cmd];

  psi_cmd_handler[cmd] = ( cmd_handler != NULL) ?
                          cmd_handler :
                          default_cmd_handler;
  PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);

  return old_handler;
} /* ps_set_cmd_handler() */

/*===========================================================================

FUNCTION PS_SET_SIGNAL()

DESCRIPTION
  This function sets a signal to PS task (ps_tcb) for processing.
  - Recommended API for all signals including ext signals.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ps_set_signal
(
  ps_sig_enum_type sig
)
{
  rex_sigs_type sig_arr[PS_SIG_ARR_LEN];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    If PS TCB is not set, just ignore the signal!
  -----------------------------------------------------------------------*/  
  if (rex_ps_tcb == NULL)
  {
    return;
  }
  
  memset(sig_arr, 0, sizeof(sig_arr));

  PS_ENTER_CRIT_SECTION(&ps_task_crit_section);
  if((int)sig >= PS_MIN_EXT_SIGNAL)
  {
    PS_SET_EXT_SIG(sig);
    sig = PS_EXT1_RX_SIGNAL;
  }
  PS_SET_SIG(sig_arr, sig);

#ifdef FEATURE_REX_SIGS_EXT
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    if(FALSE == (rex_set_sigs_ext(rex_ps_tcb, sig_arr)))
  #else
    if(FALSE == (rex_set_sigs_ext(&ps_tcb, sig_arr)))
  #endif
  {
    LOG_MSG_ERROR_1("ps_set_signal(): "
                    "Failure to set signal %d in ps_tcb", sig);
    ASSERT(0);
  }
#else
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    (void)rex_set_sigs( rex_ps_tcb, sig_arr[0] );
  #else
    (void)rex_set_sigs( &ps_tcb, sig_arr[0] );
  #endif
#endif /* FEATURE_REX_SIGS_EXT */
  PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);
} /* ps_set_signal */

/*===========================================================================

FUNCTION PS_CLR_SIGNAL()

DESCRIPTION
  This function clears the signal posted onto PS task (ps_tcb).
  - Recommended API for all signals including ext signals.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ps_clr_signal 
(
  ps_sig_enum_type sig
)
{
  rex_sigs_type sig_arr[PS_SIG_ARR_LEN];
  uint32 i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    PS_EXT1_RX_SIGNAL cannot be cleared explicitly. Verify.
  -------------------------------------------------------------------------*/
  if(sig == PS_EXT1_RX_SIGNAL)
  {
    MSG_ERROR("ps_clr_signal(): "
              "PS_EXT1_RX_SIG cannot be cleared explicitly.", 0, 0, 0);
    ASSERT(0);
  }
  
  PS_ENTER_CRIT_SECTION(&ps_task_crit_section);
  memset(sig_arr, 0, sizeof(sig_arr));

  if((int)sig >= PS_MIN_EXT_SIGNAL)
  {
    PS_CLR_EXT_SIG(sig);
    /*-----------------------------------------------------------------------
      Clear the PS_EXT1_RX_SIGNAL signal if all extension signals have been 
      cleared.
    -----------------------------------------------------------------------*/
    /*lint -e506 */
    for(i=0; i<PS_EXT_SIG_ARR_LEN; i++)
    {
      if(ps_ext_sig_mask[i] != 0)
      {
        PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);
        return;
      }
    }

    sig = PS_EXT1_RX_SIGNAL;
  }

  /*-------------------------------------------------------------------------
    Set the signal to be cleared by calling REX api
  -------------------------------------------------------------------------*/
  PS_SET_SIG(sig_arr, sig);
  
#ifdef FEATURE_REX_SIGS_EXT
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    if(FALSE == (rex_clr_sigs_ext(rex_ps_tcb, sig_arr))) 
  #else
    if(FALSE == (rex_clr_sigs_ext(&ps_tcb, sig_arr))) 
  #endif
  { 
    LOG_MSG_ERROR_1("ps_clr_signal(): "
                    "Failure to clear signal %d in ps_tcb", sig);
    ASSERT(0);
  } 
#else
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    (void)rex_clr_sigs( rex_ps_tcb, sig_arr[0] );
  #else
    (void)rex_clr_sigs( &ps_tcb, sig_arr[0] );
  #endif
#endif /* FEATURE_REX_SIGS_EXT */
  PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);
}/* ps_clr_signal */

/*===========================================================================

FUNCTION PS_SET_EXT1_SIGNAL()

DESCRIPTION
  This function sets a signal to PS task (ps_tcb) for processing. 
  - EXTENSION signals ONLY
  ::Protect ps_ext1_sig_mask under critical section always.

  Not needed after re-arch. Preserved for backward compatibility.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ps_set_ext_signal
(
  ps_sig_enum_type sig
)
{
  ps_set_signal(sig);
} /* ps_set_ext_signal */

/*===========================================================================

FUNCTION PS_CLR_EXT1_SIGNAL()

DESCRIPTION
  This function clears the signal posted onto PS task (ps_tcb). 
  - EXTENSION signals ONLY
  - Signal must have set using PS_SET_EXT1_SIGNAL macro.
  ::Protect ps_ext1_sig_mask under critical section always.
  
  Not needed after re-arch. Preserved for backward compatibility.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ps_clr_ext_signal 
(
  ps_sig_enum_type sig
)
{
  ps_clr_signal(sig);
}/* ps_clr_ext_signal */

/*===========================================================================

FUNCTION PS_WAIT_EXT()

DESCRIPTION
  This function performs the PS Task waiting function. This function will 
  not return until at least one signal from the requested mask is set. 
  The watchdog is also kicked at least once.
  
DEPENDENCIES
  requested_mask should be an array of size PS_MAX_SIG_ARR_LEN.

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated 
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared

NOTE
  Supports REX extension signals.
===========================================================================*/
rex_sigs_type* ps_wait_ext
( 
  rex_sigs_type* requested_mask              /* mask of signals to wait for */
)
{
  /*lint -e506 */
  rex_sigs_type set_sigs[PS_MAX_SIG_ARR_LEN];     /* Currently set signals */
  boolean sig_result =  FALSE;
  uint32 i = 0, j = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    First check to see if one of the signals in the mask is already set.  If 
    yes, then clear those signals and return. Otherwise enter into the wait
    only if no requested signal events have occurred
  -------------------------------------------------------------------------*/
  memset(set_sigs, 0, sizeof(set_sigs));

#ifdef FEATURE_REX_SIGS_EXT
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    sig_result = rex_get_sigs_ext(rex_ps_tcb, set_sigs);
  #else
    sig_result = rex_get_sigs_ext(&ps_tcb, set_sigs);
  #endif
  if(sig_result == FALSE)
  {
    DATA_ERR_FATAL("ps_wait_ext(): Failed to obtain the signals from the PS TCB.");
    return NULL;
  }
#else
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    set_sigs[0] = rex_get_sigs( rex_ps_tcb );
  #else
    set_sigs[0] = rex_get_sigs( &ps_tcb );
  #endif
#endif /* FEATURE_REX_SIGS_EXT */

  /*-------------------------------------------------------------------------
    Set the extension signal part of the set_sigs array.
    Also set PS_EXT1_RX_SIGNAL if any extension signal is set in the 
    requested_mask.
  -------------------------------------------------------------------------*/
  /*lint -e506 */
  for( i = PS_SIG_ARR_LEN, j = 0; i < PS_MAX_SIG_ARR_LEN; i++, j++ )
  {
    set_sigs[i] = ps_ext_sig_mask[j];
    if( requested_mask[i] != 0 )
    {
      PS_SET_SIG(requested_mask, PS_EXT1_RX_SIGNAL);
    }
  }

  /*-------------------------------------------------------------------------
    Wait till one of the requested signals is set.
  -------------------------------------------------------------------------*/
  /*lint -e716 */
  while(TRUE)
  {
    sig_result = FALSE;
    /*lint -e506 */
    for(i = 0; i < PS_MAX_SIG_ARR_LEN; i++)
    {
      if(set_sigs[i] & requested_mask[i])
      {
        sig_result = TRUE;
        break;
      }
    }

    if( FALSE == sig_result )
    {
      PS_SET_SIG(requested_mask, PS_DOG_HB_REPORT_SIGNAL);
#ifdef FEATURE_REX_SIGS_EXT
      sig_result = rex_wait_ext(requested_mask, set_sigs);
      if( FALSE == sig_result )
      {
        DATA_ERR_FATAL("ps_wait_ext():rex_wait_ext() failed.");
        return NULL;
      }
#else
      set_sigs[0] = rex_wait(requested_mask[0]);
#endif /* FEATURE_REX_SIGS_EXT */
      /*---------------------------------------------------------------------
        Set the extension signal part of the set_sigs array if 
        PS_EXT1_RX_SIGNAL is set after the wait.
      ---------------------------------------------------------------------*/
      if(PS_IS_SET_SIG(set_sigs,PS_EXT1_RX_SIGNAL))
      {
         memscpy(&(set_sigs[PS_SIG_ARR_LEN]), sizeof(set_sigs), ps_ext_sig_mask,
                 sizeof(ps_ext_sig_mask));
      }

      if(PS_IS_SET_SIG(set_sigs, PS_DOG_HB_REPORT_SIGNAL))
      {
        PS_CLR_SIGNAL(PS_DOG_HB_REPORT_SIGNAL);
        dog_hb_report( ps_dog_rpt_id );
        PS_CLR_SIG(requested_mask, PS_DOG_HB_REPORT_SIGNAL);
      }
    }
    else
    {
      break;
    }
  }

  /*lint -e506 */
  for(i = 0; i < PS_MAX_SIG_ARR_LEN; i++)
  {
    requested_mask[i] &= set_sigs[i];
  }

  /*-------------------------------------------------------------------------
    Clear the signals being sent back to the user.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_REX_SIGS_EXT
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    sig_result = rex_clr_sigs_ext(rex_ps_tcb, requested_mask);
  #else
    sig_result = rex_clr_sigs_ext(&ps_tcb, requested_mask);
  #endif
  if(sig_result == FALSE)
  {
    DATA_ERR_FATAL("ps_wait_ext():rex_clr_sigs_ext() failed.");
    return NULL;
  }
#else
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    (void)rex_clr_sigs( rex_ps_tcb, requested_mask[0] );
  #else
    (void)rex_clr_sigs( &ps_tcb, requested_mask[0] );
  #endif
#endif /* FEATURE_REX_SIGS_EXT */
  /*-------------------------------------------------------------------------
    Clear the extension signals part.
  -------------------------------------------------------------------------*/
  /*lint -e506 */
  for(i=PS_SIG_ARR_LEN, j=0; i<PS_MAX_SIG_ARR_LEN; i++, j++)
  {
    ps_ext_sig_mask[j] &= ~requested_mask[i];
  }

  return requested_mask;
} /* ps_wait_ext() */


/*===========================================================================

FUNCTION PS_WAIT()

DESCRIPTION
  This function performs the PS Task waiting function. This function will
  not return until at least one signal from the requested mask is set.
  The watchdog is also kicked at least once.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared

NOTE:
  No support REX extensions.
===========================================================================*/
rex_sigs_type ps_wait
(
  rex_sigs_type requested_mask              /* mask of signals to wait for */
)
{
  rex_sigs_type set_sigs;                         /* Currently set signals */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    First check to see if one of the signals in the mask is already set.  If
    yes, then clear those signals and return. Otherwise enter into the wait
    only if no requested signal events have occurred
  -------------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2  
  set_sigs = rex_get_sigs( rex_ps_tcb );
#else
  set_sigs = rex_get_sigs( &ps_tcb );
#endif

  while ( (set_sigs & requested_mask) == 0 )
  {
    set_sigs = rex_wait( requested_mask |
                         (  1 << (rex_sigs_type)PS_DOG_HB_REPORT_SIGNAL ) );

    /*-----------------------------------------------------------------------
      If dog heart beat received, ack the beat.
    -----------------------------------------------------------------------*/
    if( set_sigs & ( 1 << (rex_sigs_type)PS_DOG_HB_REPORT_SIGNAL ) )
    {
#ifdef FEATURE_MODEM_RCINIT_PHASE2  
      (void)rex_clr_sigs( rex_ps_tcb,
                    (  1 << (rex_sigs_type)PS_DOG_HB_REPORT_SIGNAL ) );
#else
      (void)rex_clr_sigs( &ps_tcb,
                    (  1 << (rex_sigs_type)PS_DOG_HB_REPORT_SIGNAL ) );
#endif
      dog_hb_report( ps_dog_rpt_id );
    }
  }

#ifdef FEATURE_MODEM_RCINIT_PHASE2 
  (void)rex_clr_sigs( rex_ps_tcb, ( set_sigs & requested_mask ) );
#else
  (void)rex_clr_sigs( &ps_tcb, ( set_sigs & requested_mask ) );
#endif
  return ( set_sigs & requested_mask );
} /* ps_wait() */

