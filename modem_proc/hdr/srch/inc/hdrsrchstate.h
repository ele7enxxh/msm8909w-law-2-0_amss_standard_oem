#ifndef HDRSRCHSTATE_H
#define HDRSRCHSTATE_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                H D R    S R C H    S T A T E   M A C H I N E

                          HDR Search Header File

GENERAL DESCRIPTION
  This file contains prototypes and declarations regional to the HDR SRCH
  state machine.

EXTERNALIZED FUNCTIONS

REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2000 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved. 

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/inc/hdrsrchstate.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -------------------------------------------------------
08/17/2016   vke     Changes to clear the tune away count when deactivating
                     from sync suspend state
07/07/2016   vko     Featurize out ACQ_INV/SYNC_INV changes
06/29/2016   vke     Changes to implement sync trm inversion priority
12/17/2014   sat     Changes to fix compilation errors in Tabasco
10/14/2014   wsh     Changes to support Jolokia bringup
07/09/2014   cnx     Support NV refresh without reset.
05/15/2014   arm     TRM API changes for DR-DSDS.
01/22/2013   wsh     Delay tuneaway when chan_change is in progress
09/04/2013   wsh     Changes to support BOLT bring up
05/25/2013   rkc     Added HDRSRCH_SUSPENDED_SYNC_STATE for LRE.
10/10/2011   arm     Added support for RCINIT.
10/27/2011   lyl     Added hdrsrch_set_fing_and_div_limitation().
09/01/2010   vlc     Added HDRSRCH_LTE_MEAS_STATE. 
04/16/2010   lyl     Updated generic timed waiting for signal function.
09/15/2009   smd     Added state of TimeSync.
09/14/2009   ljl     Supported srch MSGR interface.
08/07/2009   lyl     Added hdrsrch_wait_for_sig().
06/30/2009   lyl     Updated for generic responses process.
05/18/2009   ljl/lyl Updated for firmware MSGR interface.
04/01/2009   rkc     Added HDRSRCHSTATE_IS_TRAFFIC_STATE macro.
03/20/2009   rkc     Added hdrsrchstate_set_revb_mode() and
                     hdrsrchstate_is_revb_mode().
08/19/2008   smd     Added hdrsrch_get_current_state.
10/01/2007   grl     Corrected missing include file.
08/19/2007   grl     Resolved Lint High warnings by including a unique errno.h.
06/20/2007   rkc     Changed hdrsrch_process_int_cmd_default to use payload.
01/29/2006   ljl     Renamed the function hdrsrch_process_int_cmd_tf() to
                     hdrsrch_process_int_cmd().
01/17/2007   ljl     Added HDR SRCH AFC stm.
12/20/2006   jyw     Added hdrsrch_clear_tune_away_on_searcher_idle.
04/26/2006   jyw     Added the second level on search idle callbacks.
09/01/2005   grl     Retain lock unlock callback added.
08/11/2005   grl     Mainlined FEATURE_HDR_TCXOMGR.
05/31/2005   grl     Added support for TRM and TCXOMgr.
03/24/2005   sq      Removed hdrsrch_retry_cmd
03/23/2005   hrk     Added prototype for hdrsrch_timed_wait_usecs().
11/03/2004   mpa     Removed dependency on std.h (merged with sys.h)
07/31/2004   sq      Added function hdrsrch_reset_no_dump_cnt
07/30/2004   sq      Added support for FEATURE_HDR_BCMCS
07/22/2004   ajn     Added common signals to state machine
03/11/2003   sq      HDR OFS support
03/11/2003   ajn     Updated Copyright notice
04/25/2002   aaj     Made clear_on_searcher_idle_proc function global
02/06/2002   ajn     hdrerrno -> errno
12/31/2001   ajn     Added _kill_signal, _stop_timer, and _kill_timer funcs.
10/24/2001   ajn     Added SLEEP and OFS states.
08/28/2001   ajn     Removed Tune signal handler, added command retry,
                     use hdrerrno_enum_type for command return status.
06/06/2001   ajn     Added OnDump and OnSearcherIdle functions
11/22/2000   ajn     Code review changes
10/31/2000   ajn     Added General Purpose Signals to State Machine
10/23/2000   ajn     Moved state identifiers from hdrsrchtask.h to here.
10/04/2000   ajn     Added dynamic wait_mask control.  Prevented masked
                     events from being handled when non-masked event causes
                     the rex_wait( ) to return.
07/07/2000   ajn     Created this file

==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "IxErrno.h"
#include "comdef.h"
#include "rex.h"
#include "hdrsrchcmd.h"
#include "hdrsrchrf.h"
#include "hdrsrchsm.h"
#include "hdrsrchmsg.h"

/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/


/*==========================================================================

                        HDR SEARCH STATES

==========================================================================*/

/*------------------------------------------------------------------------
  Enumerated State Type
------------------------------------------------------------------------*/

typedef enum
{
  HDRSRCH_INACTIVE_STATE        = 0x00,
    /* Searcher is idle ... */

  HDRSRCH_ACQ_STATE             = 0x01,
    /* Attempting to acquire a system */

  HDRSRCH_SYNC_STATE            = 0x02,
    /* Acquired ... waiting for SYNC message */

  HDRSRCH_SUSPENDED_SYNC_STATE  = 0x2a,
    /* Sync suspended */

  HDRSRCH_IDLE_STATE            = 0x03,
    /* Idle state, included monitoring control channel, access attempts */

#ifdef FEATURE_HDR_TO_LTE
  HDRSRCH_LTE_MEAS_STATE        = 0x39,
#endif /* FEATURE_HDR_TO_LTE */

  HDRSRCH_SUSPENDED_IDLE_STATE  = 0x3a,
    /* Idle state, included monitoring control channel, access attempts */

#ifdef FEATURE_HDR_BCMCS
  HDRSRCH_BC_IDLE_STATE         = 0x3b,
    /* Idle broadcast state, receiving bc/mc data                       */

  HDRSRCH_BC_ACCESS_STATE       = 0x3d,
    /* Broad access state, receiving bc/mc data, access attempts        */
#endif /* FEATURE_HDR_BCMCS */

  HDRSRCH_OFS_IDLE_STATE        = 0x3c,
    /* Off frequency neighbour searching */

  HDRSRCH_SLEEP_STATE           = 0x3e,
    /* Sleep and Reacquisition after sleep*/

  HDRSRCH_REACQ_STATE           = 0x3f,
    /* Reacquisition after sleep*/

  HDRSRCH_CONNECTED_STATE       = 0x04,
    /* Traffic */

  HDRSRCH_SUSPENDED_TC_STATE    = 0x4a,
    /* Traffic Suspended */

  HDRSRCH_OFS_TC_STATE          = 0x4c,
    /* Traffic Suspended */

  HDRSRCH_TIME_SYNC_STATE       = 0x5,
    /* Time Transfer...synchronize with CDMA system time */

  HDRSRCH_OFS_MEAS_STATE        = 0x5a,
    /* Measurement state */

  HDRSRCH_NO_STATE              = 0xFF
    /* Start / Terminate HDR operation */
}
hdrsrch_state_enum_type;


/*------------------------------------------------------------------------
  Enumerated on_searcher_idle priority type
------------------------------------------------------------------------*/

typedef enum
{
  HDRSRCH_DEFAULT_ON_SEARCH_IDLE = 0x0,
    /* Highest priority for the on search idle callbacks */
    
  HDRSRCH_TUNEAWY_ON_SEARCH_IDLE,
    /* Lower priority dedicated to the tune away callbacks */

  HDRSRCH_MAX_ON_SEARCH_IDLE
    /* Maximum number of priorities */

}
hdrsrch_on_search_idle_pri_enum_type;

#define HDRSRCHSTATE_IS_TRAFFIC_STATE                               \
          ( ( hdrsrch_state_id == HDRSRCH_CONNECTED_STATE ) ||      \
            ( hdrsrch_state_id == HDRSRCH_SUSPENDED_TC_STATE ) ||   \
            ( hdrsrch_state_id == HDRSRCH_OFS_TC_STATE ) )
  /* Macro returns TRUE if in any of the traffic states. */

/*------------------------------------------------------------------------
  Enumerated State Regional Global
------------------------------------------------------------------------*/

extern hdrsrch_state_enum_type  hdrsrch_state_id;


/* EJECT */
/*==========================================================================

                        HDR SEARCH STATE STRUCTURE

==========================================================================*/


typedef errno_enum_type   (*hdrsrch_cmd_func) ( const hdrsrch_cmd_type *);
typedef void              (*hdrsrch_sig_func) ( void );
typedef void              (*hdrsrch_on_event_func) ( void );
typedef void              (*hdrsrch_process_int_cmd_func)
                          (
                            hdrsrchsm_cmd_payload_type *payload
                          );
typedef void              (*hdrsrch_process_low_pri_rsp_ind_func)
                          (
                            hdrsrchmsg_s_type         *msg
                          );
/*------------------------------------------------------------------------
  State Structure
------------------------------------------------------------------------*/

typedef struct
{
  hdrsrch_state_enum_type id;
    /* The HDR SRCH task state id this structure represents */

  void (*init) (hdrsrch_state_enum_type old_state);
  void (*done) (hdrsrch_state_enum_type new_state);

  hdrsrch_cmd_func          cmd;
  hdrsrch_sig_func          dump;
  hdrsrch_sig_func          roll;
  hdrsrch_sig_func          wake;
  hdrsrch_sig_func          sched;
  hdrsrch_sig_func          lost_dump;
  hdrsrch_sig_func          timeout;
  hdrsrch_sig_func          gp_timer0;
  hdrsrch_sig_func          gp_timer1;
  hdrsrch_sig_func          gp_sig0;
  hdrsrch_sig_func          gp_sig1;
  hdrsrch_sig_func          time_valid;
  hdrsrch_sig_func          rxchipx16;
  hdrsrch_sig_func          pri_chain_event;
  hdrsrch_process_int_cmd_func process_int_cmd;
  hdrsrch_process_low_pri_rsp_ind_func process_low_pri_rsp_ind;
}
hdrsrch_state_struct_type;


/* EJECT */
/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/


/*=========================================================================

FUNCTION    HDRSRCH_CHANGE_STATE

DESCRIPTION
  Changes the state of the HDR SEARCH STATE MACHINE

DEPENDENCIES
  TBD

INPUTS
  new_state   - Next state for the state machine
                NULL to exit the state machine

RETURN VALUE
  None

SIDE EFFECTS
  When control is returned to the state machine, the current state's
  done() function will be called.  Then the state will change to the
  indicated state and the new state's init() function will be called.

=========================================================================*/

void hdrsrch_change_state
(
  const hdrsrch_state_struct_type *new_state
    /* Next state for the state machine */
);


/* EJECT */
/*=========================================================================

FUNCTION    HDRSRCH_ADD_TO_WAIT_MASK

DESCRIPTION
  Add signals to the set the search task will wait on.

DEPENDENCIES
  TBD

INPUTS
  sigs    - New signals to wait on (add to wait mask)

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_add_to_wait_mask
(
  rex_sigs_type   sigs
    /* Signals to add to the wait mask */
);


/* EJECT */
/*=========================================================================

FUNCTION    HDRSRCH_REMOVE_FROM_WAIT_MASK

DESCRIPTION
  Remove signals from the set the search task will wait on.

DEPENDENCIES
  TBD

INPUTS
  sigs    - Signals the search task will no longer wait on or process.

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_remove_from_wait_mask
(
  rex_sigs_type   sigs
    /* Signals to remove from the wait mask */
);


/* EJECT */
/*=========================================================================

FUNCTION    HDRSRCH_KILL_SIGNAL

DESCRIPTION
  Clears the given signal if set, and removes the it from the wait mask.

DEPENDENCIES
  None

INPUTS
  Timer - Address of timer to kill

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void hdrsrch_kill_signal
(
  rex_sigs_type   sigs
    /* Signals to be cleared and removed from the wait mask */
);


/*=========================================================================

FUNCTION    HDRSRCH_STOP_TIMER

DESCRIPTION
  Stops the given timer, and clears the timer's signal if set.

DEPENDENCIES
  None

INPUTS
  timer - Timer to be stopped

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void hdrsrch_stop_timer
(
  rex_timer_type   *timer
    /* Timer to be stopped */
);


/*=========================================================================

FUNCTION    HDRSRCH_KILL_TIMER

DESCRIPTION
  Stops the given timer, clears the timer's signal if set, and removes the
  signal from the wait mask.

DEPENDENCIES
  None

INPUTS
  timer - Timer to be killed

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void hdrsrch_kill_timer
(
  rex_timer_type   *timer
    /* Timer to be killed */
);


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_COMPLETE_CMD

DESCRIPTION
  Indicates that the currently pending command is now complete, and the
  command buffer may be returned to the free queue.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command processing is re-enabled.

=========================================================================*/

void hdrsrch_complete_cmd( void );


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_ON_DUMP

DESCRIPTION
  Queue a function for execution after the next search dump.

  The function is only called once.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Function is executed in future.

=========================================================================*/

void hdrsrch_on_dump
(
  hdrsrch_on_event_func   func
    /* Function to call after the next searcher dump. */
);


/*=========================================================================

FUNCTION HDRSRCH_CLEAR_ON_SEARCHER_IDLE_PROC

DESCRIPTION
  The function clears on_searcher_idle processing

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets hdrsrchstate.on_dump and hdrsrchstate.on_searcher_idle to NULL

=========================================================================*/

void hdrsrch_clear_on_searcher_idle_proc ( void );


/*=========================================================================

FUNCTION HDRSRCH_ON_SEARCHER_IDLE

DESCRIPTION
  Queue a command for execution when the searcher is idle, or immediately
  if searcher is presently idle.

  The function is only called once.

DEPENDENCIES
  Uses "On Dump" function, above.

RETURN VALUE
  None

SIDE EFFECTS
  Command is executed now or in future.

=========================================================================*/

void hdrsrch_on_searcher_idle
(
  hdrsrch_on_event_func   func
    /* Function to call when searcher is idle. */
);


/*=========================================================================

FUNCTION HDRSRCH_TUNE_AWAY_ON_SEARCHER_IDLE

DESCRIPTION
  Similiar to the on_search_idle but it is dedicated to the tune away event.


DEPENDENCIES
  Uses "On Dump" function, above.

RETURN VALUE
  None

SIDE EFFECTS
  Command is executed now or in future.

=========================================================================*/

void hdrsrch_tune_away_on_searcher_idle
(
  hdrsrch_on_event_func   func
    /* Function to call when searcher is idle. */
);


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_CLEAR_TUNE_AWAY_ON_SEARCHER_IDLE

DESCRIPTION
  Clear the tune away on search idle.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void hdrsrch_clear_tune_away_on_searcher_idle( void );


/* EJECT */
/*=========================================================================

                 DEFAULT EVENT RESPONSE FUNCTIONS

=========================================================================*/

/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_INIT

DESCRIPTION
  Generic initialization processing

DEPENDENCIES
  None

PARAMETERS
  last state

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_init
(
  hdrsrch_state_enum_type last_state
    /* State this state was entered from */
);


/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_DONE

DESCRIPTION
  Generic cleanup processing

DEPENDENCIES
  None

PARAMETERS
  last state

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_done
(
  hdrsrch_state_enum_type next_state
    /* State this state will transition to. */
);


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_CMD

DESCRIPTION
  Generic command processing

DEPENDENCIES
  None

PARAMETERS
  cmd         - command to be processed.

RETURN VALUE
  E_SUCCESS       - command has been successfully executed
  E_FAILURE       - command has failed
  E_NOT_SUPPORTED - command is not support
  E_IN_PROGRESS   - command has been started, but has not completed
  E_AGAIN         - command cannot be started in present state, but is a
                    valid command.  Execute hdrsrch_retry_cmd( ) at a
                    later point in time to reattempt the command.
                    [hdrsrch_retry_cmd( ) is called at every state change]

SIDE EFFECTS
  If E_IN_PROGRESS or E_AGAIN is returned, command processing is disabled
  until the command buffer is passed to hdrsrch_complete_cmd( ).

=========================================================================*/

errno_enum_type hdrsrch_default_cmd
(
  const hdrsrch_cmd_type  *cmd
    /* Command to be processed */
);


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_DUMP

DESCRIPTION
  Generic Search Dump processing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_dump( void );


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_ROLL

DESCRIPTION
  Generic PN ROLL processing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_roll( void );


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_WAKE

DESCRIPTION
  Generic WakeUp processing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_wake( void );


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_TUNE

DESCRIPTION
  Generic RF Tune processing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_tune( void );


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_SCHED

DESCRIPTION
  Generic Search Schedule Timer Processing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_sched( void );


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_LOST_DUMP

DESCRIPTION
  Generic Lost Dump Processing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_lost_dump( void );


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_TIMEOUT

DESCRIPTION
  Generic Timeout Processing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_timeout( void );


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_GP_TIMER0

DESCRIPTION
  Generic Timer Processing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_gp_timer0( void );


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_GP_TIMER1

DESCRIPTION
  Generic Timer Processing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_gp_timer1( void );


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_GP_SIG0

DESCRIPTION
  General Purpose Signal #0 Processing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_gp_sig0( void );


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_GP_SIG1

DESCRIPTION
  General Purpose Signal #1 Processing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_gp_sig1( void );



/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_TIME_VALID

DESCRIPTION
  Generic Time Valid Processing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_time_valid( void );



/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_RXCHIPX16

DESCRIPTION
  Generic RXCHIPX8 Processing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_rxchipx16( void );



/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_PRI_CHAIN_EVENT

DESCRIPTION
  Generic processing of a primary chain related event.  Note that primary
  chain does not imply either chain0 or chain1.  It is possible it is
  either although HDR only supports chain0 as the primary due to the 
  RF driver implementation.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_pri_chain_event( void );



/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_DEFAULT_DIV_CHAIN_EVENT

DESCRIPTION
  Generic processing of a diversity chain related event.  Note that diversity
  chain does not imply either chain0 or chain1.  It is possible it is
  either although HDR only supports chain1 as the diversity reciever due to 
  the RF driver implementation.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  TBD

=========================================================================*/

void hdrsrch_default_div_chain_event( void );



/* EJECT */
/*=========================================================================

FUNCTION HDRSRCH_RESET_NO_DUMP_CNT

DESCRIPTION
  This function resets no_dump_cnt in dog structure in hdrsrchstate

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void hdrsrch_reset_no_dump_cnt( void );



/* EJECT */
/*============================================================================

CALLBACK HDRSRCH_RF_GRANT_CB

DESCRIPTION
  This function is called by HDRSRCHRF to notify HDR search that a tranceiver
  has been granted.
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void hdrsrch_rf_grant_cb
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */

  boolean                               granted
    /* Whether the resources where granted */
);



/* EJECT */
/*============================================================================

CALLBACK HDRSRCH_RF_UNLOCK_CB

DESCRIPTION
  This function is called by HDRSRCHRF to notify HDR search that a transceiver
  is requested to be unlocked.
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void hdrsrch_rf_unlock_cb
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */

  hdrsrchrf_unlock_event_enum_t         event,
    /* The event being sent to the client */

  uint32                                unlock_by_sclk
    /* Sclk timestamp for HDRSRCHRF_UNLOCK_BY */
);



/* EJECT */
/*===========================================================================

FUNCTION HDRSRCH_TIMED_WAIT_USECS

DESCRIPTION
  This function performs a timed wait. The timed value should be less
  than watch dog timeout period.

DEPENDENCIES
  None

PARAMETERS
  The time to wait in micro seconds.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrch_timed_wait_usecs
(
  uint16  us_to_wait
);


/*===========================================================================

FUNCTION HDRSRCH_WAIT_FOR_HIGH_PRI_RSP

DESCRIPTION
  This function checks whether the response is received with a certain interval
  of time. The timed value should be less than watch dog timeout period.

DEPENDENCIES
  None

PARAMETERS
  us_to_wait    - The time to wait in micro seconds.
  rsp         - The response to be received.

RETURN VALUE
  TRUE  - if the response is received.
  FALSE - if the response is not received within us_to_wait. 

SIDE EFFECTS
  None.

===========================================================================*/

boolean hdrsrch_wait_for_high_pri_rsp
(
  uint32                     us_to_wait,
    /* time to wait in micro seconds */

  hdrsrchmsg_rsp_e_type      rsp
    /* The response to be received */
);


/*===========================================================================

FUNCTION HDRSRCH_PROCESS_INT_CMD_DEFAULT

DESCRIPTION
  This function is the default function to process the internal commands.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrch_process_int_cmd_default
(
  hdrsrchsm_cmd_payload_type                     *payload
    /* The internal command + payload */
);


/*===========================================================================

FUNCTION HDRSRCH_PROCESS_INT_CMD

DESCRIPTION
  This function processes internal commands.

DEPENDENCIES
  None

PARAMETERS
  The payload of the internal command.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrch_process_int_cmd
(
  void                       *payload
    /* Payload */
);

/*=========================================================================

FUNCTION    HDRSRCH_GET_CURRENT_STATE

DESCRIPTION
  Get the current state of the HDR SEARCH STATE MACHINE

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  Current state of the HDR SEARCH STATE MACHINE

SIDE EFFECTS
  None

=========================================================================*/

hdrsrch_state_enum_type hdrsrch_get_current_state( void );


/*=========================================================================

FUNCTION HDRSRCHSTATE_SET_REVB_MODE

DESCRIPTION
  Sets boolean Rev B mode.

DEPENDENCIES
  None

PARAMETERS
  is_revb   - TRUE if Rev B mode, FALSE otherwise.

RETURN VALUE
  None

SIDE EFFECTS
  System lost

=========================================================================*/

void hdrsrchstate_set_revb_mode
(
  boolean         is_revb
    /* TRUE to set Rev B mode */
);

/*=========================================================================

FUNCTION HDRSRCHSTATE_IS_REVB_MODE

DESCRIPTION
  Returns whether srch is in REVB mode or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - When REVB mode is enabled
  FALSE - When not in REVB mode

SIDE EFFECTS
  System lost

=========================================================================*/

boolean hdrsrchstate_is_revb_mode( void );


/*=========================================================================

FUNCTION    HDRSRCH_PROCESS_LOW_PRI_RSP_IND

DESCRIPTION
  Process the low priority responses and indications.

DEPENDENCIES
  TBD

INPUTS
  msg - The message pointer

RETURN VALUE
  None

SIDE EFFECTS
  

=========================================================================*/

void hdrsrch_default_process_low_pri_rsp_ind
(
   hdrsrchmsg_s_type         *msg
    /* Message */
);


/*===========================================================================

FUNCTION HDRSRCH_TIMED_WAIT_FOR_SIG

DESCRIPTION
  This function waits for a signal within a certain amount of time specified 
  in the parameter. If it cannot receive the requested signals before timing out,
  it will fatal error.

DEPENDENCIES
  None

PARAMETERS
  sig        - The signal to wait
  ms_to_wait - The time to wait in micro seconds. 
               If it's negative value, the function will infinetely wait for 
               the signal.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrch_timed_wait_for_sig
(
  rex_sigs_type            sig,
    /* The signal to wait */

  int32                    ms_to_wait
    /* Time to wait in milli seconds */
);


/*=========================================================================

FUNCTION    HDRSRCH_DISCARD_SRCH_DUMP

DESCRIPTION
  This function discards the srch responses in the queue.

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  

=========================================================================*/

void hdrsrch_discard_srch_dump( void );


/*==========================================================================

FUNCTION  HDRSRCH_SET_FING_AND_DIV_LIMITATION

DESCRIPTION
  This function will set finger max number and diversity on/off based on
  whether the chip is V2 HW or not for 8960.
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrch_set_fing_and_div_limitation( void );

#ifdef FEATURE_MODEM_RCINIT
/*===========================================================================

FUNCTION HDRSRCH_INIT_BEFORE_TASK_START

DESCRIPTION
  This function takes care of all intitalizations needed before hdrsrch task
  start. Called only in RCINIT framework.

DEPENDENCIES
  RCINIT framework should be supported.

PARAMETERS
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrch_init_before_task_start( void );
 /* hdrsrch_init_before_task_start */
#endif /* FEATURE_MODEM_RCINIT */

/*=========================================================================

FUNCTION HDRSRCH_ON_SEARCHER_IDLE_REGED

DESCRIPTION
  The function check if any on_searcher_idle callbacks has been registered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
LOCAL boolean hdrsrch_on_searcher_idle_reged( void );

#ifdef FEATURE_HDR_RUMI
/*===========================================================================

FUNCTION HDRSRCHSTATE_SRAM_CHECK

DESCRIPTION
  This function starts/stops SRAM check.

DEPENDENCIES
  Must only start after RXLM initialization (RX_START)

PARAMETERS
  start -  Whether to start or stop SRAM check

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchstate_sram_check
(
  boolean start
);
#endif /* FEATURE_HDR_RUMI */

/*============================================================================

FUNCTION HDRSRCHSLEEP_WAKEUP_ID_INIT

DESCRIPTION 
  Initialize wakeup_id
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/
void hdrsrchsleep_wakeup_id_init( void );

/*===========================================================================
FUNCTION HDRSRCH_READ_EFS_NV_ITEMS

DESCRIPTION
  Read value of EFS NV items.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

NOTE
  None
===========================================================================*/
void hdrsrch_read_efs_nv_items( void );

#ifdef FEATURE_HDR_TRM_UNIFICATION
/*===========================================================================

FUNCTION HDRSRCHSYNC_GET_TRM_PRIORITY

DESCRIPTION
  This function returns the current priority of sync state

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  HDRSRCHRF_SYNC        :    Default Sync TRM priority
  HDRSRCHRF_SYNC_INV :    Sync High TRM priority
  
SIDE EFFECTS
  TBD

===========================================================================*/

hdrsrchrf_reason_enum_t hdrsrchsync_get_trm_priority( void );

#endif /* FEATURE_HDR_TRM_UNIFICATION */

/*===========================================================================

FUNCTION HDRSRCHSYNC_RESET_TA_COUNT

DESCRIPTION
  This function resets tune away count

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  TBD

===========================================================================*/

void hdrsrchsync_reset_ta_count( void );

#endif /* HDRSRCHSTATE_H */
