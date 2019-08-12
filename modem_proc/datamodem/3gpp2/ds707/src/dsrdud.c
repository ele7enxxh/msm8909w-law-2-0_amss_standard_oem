
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               D S R D U D

GENERAL DESCRIPTION
  This module contains the coding for RDUD (Reduced Dormancy followed by
  Unsolicited Data) module. It runs in DS task context. 
  This file has functions to handle RDUD related commands posted
  from DS module and internal RDUD State Machine  and static variables
  associated with this state machine. 


EXTERNALIZED FUNCTIONS
  These functions are defined in DSRDUD.H and are external to the  RDUD
  module

  ds_rdud_process_cmd()
    Processes RDUD commadn received by DS task

  FEATURE_DS_RDUD  - Supports RDUD ( Incoming Data Regulator ) module in Hybrid mode

 Copyright (c) 2004-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/



/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/dsrdud.c#1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
06/30/14    niv    Added wrapper functions & API change for 
                   NV_REFRESH 
06/03/13    sd     ATCoP decoupling changes
07/27/11    sn     Fix to initialize new RDUD enabled variable to default 
                   value TRUE to avoid RDUD getting disabled when call ends.
06/13/11    dvk    Merged support to enable/disable RDUD at runtime. 
10/13/10    op     Migrated to MSG 2.0 macros
10/07/10    ttv    Added messages which explain the reason for asserting.
05/10/07    ac     Lint fix
04/26/05    vrk    Created events to enable automated testing
03/01/05    sk     Merged cta timer fixed
10/08/04    kvd    Created Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_RDUD

#include "ds3gtimer.h"
#include "ds3gsiolib.h"
#include "ds3gcfgmgr.h"
#include "err.h"
#include "data_msg.h"
#include "rex.h"
#include "ds707.h"
#include "ds707_pkt_mgr.h"
#include "ds707_nv_util.h"
#include "ds707_pkt_mgri.h"
#include "dstaski.h"
#include "ds707_cta_rlp.h"
#include "dsrdud.h"
#include "event.h"
#include "ds707_cback.h"
#include "dsat707extctab.h"
#include "ds707_event_defs.h"

/*===========================================================================
            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/


/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*---------------------------------------------------------------------------
                           Pre-processor Constants
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                           Enum Typedefs
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Type that represents the current state of the RDUD module.
---------------------------------------------------------------------------*/

typedef enum
{
  DS_RDUD_IDLE_STATE=0,          /* RDUD algo not yet active               */
  DS_RDUD_ACTIVE_STATE=1,        /* Regulator active                       */
  DS_RDUD_MAX_STATE
} dsrdudi_rdud_state_enum_type;

typedef struct
{
  dsrdudi_rdud_state_enum_type  rdud_state;
  rex_timer_type                rdud_active_timer;  /* RDUD activity timer */
  uint32                        default_inactivity_restore_timeout;
  uint32                        incoming_data_rlp_timeout;
} dsrdud_type;

/*===========================================================================
                        Variable Definitions
===========================================================================*/

static dsrdud_type dsrdud;
static boolean     dsrdudi_enabled = TRUE;
static boolean     dsrdudi_enabled_new_val = TRUE;

/*---------------------------------------------------------------------------
  Function prototypes to allow the desired ordering of functions within this
  module - forward declarations.
---------------------------------------------------------------------------*/
LOCAL void dsrdudi_rdud_transition_state( 
                                    dsrdudi_rdud_state_enum_type new_state );
LOCAL void dsrdudi_process_inc_call_connect( void );
LOCAL void dsrdudi_process_rdud_timer_expiry( void );
LOCAL void dsrdudi_process_phys_link_down( void );
LOCAL void dsrdudi_timer_init( void );
LOCAL void dsrdudi_timer_cb( unsigned long    timer_id );
LOCAL void dsrdudi_start_rdud_timer( dword duration );
LOCAL void dsrdudi_cancel_rdud_timer( void );
LOCAL void dsrdudi_707_event_cback
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
);
LOCAL void dsrdud_send_rdud_cmd
(
  ds_cmd_enum_type rdud_cmd                  /* Cmd sent to RDUD module    */
);
LOCAL void dsrdud_timer_cb ( unsigned long    timer_id );

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION DSRDUD_INIT

DESCRIPTION
  This function performs power-on initialization of RDUD module.
  This includes initialization and registration of ds3g timers.

  This function must be called once at data services task startup.
  This could be called from different task context if RDUD module
  moves to a different task.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

void  dsrdud_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsrdud.rdud_state = DS_RDUD_IDLE_STATE; 
  dsrdud.default_inactivity_restore_timeout =
                           DS_RDUD_DEFAULT_INACTIVITY_RESTORE_TIMEOUT;
  dsrdud.incoming_data_rlp_timeout = DS_RDUD_INCOMING_DATA_RLP_TIMEOUT;

  /*-------------------------------------------------------------------------
    Register  ds3g timers for RDUD module.
  -------------------------------------------------------------------------*/
  dsrdudi_timer_init();


  if (ds707_cback_register(
                            DS707_CBACK_INC_CALL_EV,
                            dsrdudi_707_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Can register for phys link up event",0,0,0);
  }

  if (ds707_cback_register(
                            DS707_CBACK_PHYS_LINK_DOWN_EV,
                            dsrdudi_707_event_cback
                         ) == FALSE)
  {
    ERR_FATAL("Can register for phys link down event",0,0,0);
  }

  ds707_nv_util_read_dsrdud_nv();

} /* dsrdud_init() */

/*===========================================================================
FUNCTION       DSRDUDI_PHYS_LINK_EVENT_HANDLER_CB

DESCRIPTION    Callback when various phys link events occur.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void dsrdudi_707_event_cback
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG1(MSG_LEGACY_MED, "DSRDUD rcvd 707 event : %d",event);

  switch(event)
  {
    case DS707_CBACK_INC_CALL_EV:
      dsrdud_send_rdud_cmd( DS_CMD_RDUD_INC_CALL_ACCEPTED );
      break;

    case DS707_CBACK_PHYS_LINK_DOWN_EV:
      dsrdud_send_rdud_cmd(DS_CMD_RDUD_PHYS_LINK_DOWN);
      break;

    default:
      ASSERT(0);      
      break;
  }
} /* dsrdudi_707_event_cback() */

/*===========================================================================
FUNCTION      DSRDUDI_TIMER_INIT

DESCRIPTION   Register 3g tiemrs for RDUD moudle.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsrdudi_timer_init(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Register  ds3g timers for RDUD module.
  -------------------------------------------------------------------------*/
  ds3gi_timer_register(&dsrdud.rdud_active_timer,
                       dsrdud_timer_cb,
                       DS3G_TIMER_RDUD,
                       DS3G_TIMER_TYPE_REX_TIMER);
} /* dsrdudi_timer_init() */

/*===========================================================================
FUNCTION      DSRDUD_TIMER_CB

DESCRIPTION   Callback for the  RDUD de-activation timer. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Timer expiation cmds posted to RDUD module.
===========================================================================*/
void dsrdud_timer_cb
(
  unsigned long    timer_id
)
{
  ds_cmd_type           *cmd_ptr;                    /* Pointer to command */
  ds_cmd_enum_type      cmd_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ((ds3g_timer_enum_type)timer_id)
  {

    case DS3G_TIMER_RDUD:
      cmd_id = DS_CMD_RDUD_TIMER_EXPIRED;
      break;

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Bad timer id in dsrdud callback %d",
                      timer_id);
      ASSERT(0);
      return;
  }/* end of swicth */

  if( (cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL )
  {
    ASSERT(0);
    return;
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Sending RDUD cmd - %d",cmd_id);
    cmd_ptr->hdr.cmd_id = cmd_id;
    ds_put_cmd( cmd_ptr );
  }
} /* dsrdud_timer_cb() */

/*===========================================================================
FUNCTION DSRDUDI_START_RDUD_TIMER

DESCRIPTION    This function will set the timer associated with the RDUD
               module to the specified duration.  The time units are in msec.
               This calls the ds3gi_start_timer, generic start_timer function
               used in ds707.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Rex timer started for duration, DS_RDUD_ACTIVE_TIME.

===========================================================================*/

void dsrdudi_start_rdud_timer
(
  dword         duration                 /* time interval, in msecs        */
)
{
  ds_rdud_timer_struct ds_rdud_timer_report;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Start RDUD  activity timer. 
  -------------------------------------------------------------------------*/

  ds_rdud_timer_report.timer_state = DS_RDUD_EVENT_TIMER_START;
  ds_rdud_timer_report.timer_value = duration;

  DATA_IS707_MSG1(MSG_LEGACY_MED, "Starting RDUD timer: %d msec",duration);

  ds3gi_start_timer(DS3G_TIMER_RDUD, duration);

  event_report_payload (EVENT_DS_RDUD_TIMER, 
						sizeof(ds_rdud_timer_struct), 
						(void*)&ds_rdud_timer_report);

} /* dsrdudi_start_rdud_timer() */

/*===========================================================================
FUNCTION DSRDUDI_CANCEL_RDUD_TIMER

DESCRIPTION    This function cancels the timer set by RDUD module, typicaly
               if we exit the current state before the timer expired due
               to some other event.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   RDUD timer cancelled.

===========================================================================*/

void dsrdudi_cancel_rdud_timer
(
  void
)
{
  ds_rdud_timer_struct ds_rdud_timer_report;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Cancel RDUD timer.
  -------------------------------------------------------------------------*/
  DATA_IS707_MSG0(MSG_LEGACY_MED, "Cancelling RDUD timer");
  ds3gi_stop_timer( DS3G_TIMER_RDUD);

  ds_rdud_timer_report.timer_state = DS_RDUD_EVENT_TIMER_CANCEL;
  ds_rdud_timer_report.timer_value = 0;

  event_report_payload (EVENT_DS_RDUD_TIMER, 
                 sizeof(ds_rdud_timer_struct), (void*)&ds_rdud_timer_report);

} /* dsrdudi_cancel_rdud_timer() */

/*===========================================================================
FUNCTION DSRDUD_SEND_RDUD_CMD

DESCRIPTION
  This function  posts a cmd to the RDUD module

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dsrdud_send_rdud_cmd
(
  ds_cmd_enum_type rdud_cmd                  /* Cmd sent to RDUD module    */
)
{
  ds_cmd_type            *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(IS_IN_DS_TASK())
  {
    ds_process_rdud_cmd(rdud_cmd);
  }
  else
  {
    switch ( rdud_cmd )
    {
      case DS_CMD_RDUD_INC_CALL_ACCEPTED:
      case DS_CMD_RDUD_PHYS_LINK_DOWN:
      case DS_CMD_RDUD_TIMER_EXPIRED:
      {
        break;
      }
      default:
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                        "Invalid cmd  %d to send to RDUD module", rdud_cmd);
        return;
      }
    } /* switch rdud_cmd */

    if( (cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL )
    {
      ASSERT(0);
      return;
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Sending RDUD cmd - %d", rdud_cmd);
      cmd_ptr->hdr.cmd_id = rdud_cmd;
      ds_put_cmd( cmd_ptr );
    }
  } /* switch else */
} /* dsrdud_send_rdud_cmd() */


/*===========================================================================
FUNCTION DS_PROCESS_RDUD_CMD

DESCRIPTION
  This module process RDUD cmds received by the DS task (possibly by different
  task if RDUD module moves to a different task in future); These cmds are for
  RDUD module to kick-in and force dormancy on 1X to move to EVDO, if needed.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_process_rdud_cmd
(
  ds_cmd_enum_type rdud_cmd
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Switch on the cmd name and process the command.
  -------------------------------------------------------------------------*/

  DATA_IS707_MSG2(MSG_LEGACY_MED, "Rcvd cmd - %d in %d state",
                  rdud_cmd, dsrdud.rdud_state);

  switch( rdud_cmd )
  {
    /*-----------------------------------------------------------------------
      Incoming Call accepted.
    -----------------------------------------------------------------------*/
    case DS_CMD_RDUD_INC_CALL_ACCEPTED:
    {
      dsrdudi_process_inc_call_connect();
      break;
    }
    /*-----------------------------------------------------------------------
      RDUD timer expired while in dormant or active states.
    -----------------------------------------------------------------------*/
    case DS_CMD_RDUD_TIMER_EXPIRED:
    {
      dsrdudi_process_rdud_timer_expiry();
      break;
    }
    /*-----------------------------------------------------------------------
      Current 1X call terminated  -  call NULL , not dormant.
    -----------------------------------------------------------------------*/
    case DS_CMD_RDUD_PHYS_LINK_DOWN:
    {
      dsrdudi_process_phys_link_down();
      break;
    }
    default:
    {
      return;
    }
  } /* switch rdud_cmd */
} /* ds_process_rdud_cmd() */

/*===========================================================================
FUNCTION DSRDUDI_PROCESS_INC_CALL_CONNECT

DESCRIPTION
  This module process call_connected RDUD cmd received by the DS task (possibly
  by different task if RDUD module moves to a different task in future).
  If receied in correct state, RDUD module moves to RDUD_ACTIVE state.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dsrdudi_process_inc_call_connect
( 
  void
)
{
  uint32  cta_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_CTA_VAL,(void *)&cta_val,
                                          DS707_NON_ARRAY);
  DATA_IS707_MSG2(MSG_LEGACY_MED, "Rcvd INC_CALL_CONNECT - in RDUD state: %d."
                  " RDUD enabled is %d.", 
                  dsrdud.rdud_state, dsrdudi_enabled);

  switch( dsrdud.rdud_state )
  {
    case DS_RDUD_IDLE_STATE:
    {
      /*---------------------------------------------------------------------
        Transition to RDUD_ACTIVE state 
      ---------------------------------------------------------------------*/
      if(dsrdudi_enabled && (cta_val != 0))
      {
        if(dsrdud.default_inactivity_restore_timeout != 0)
        {
          dsrdudi_rdud_transition_state(DS_RDUD_ACTIVE_STATE);
        }
      }
      break;
    }
    case DS_RDUD_ACTIVE_STATE:
    {
      /*---------------------------------------------------------------------
        RDUD already active - Nothing to do 
      ---------------------------------------------------------------------*/
      ASSERT(0);
      break;
    }
    default:
    {
      return;
    }
  } /* switch rdud_state */
} /* dsrdudi_process_inc_call_connect() */

/*===========================================================================
FUNCTION DSRDUDI_PROCESS_RDUD_TIMER_EXPIRY

DESCRIPTION
  This module process RDUD timer expired event received by the DS task
  (possibly different task if RDUD module moves to a different task in future)
  This shoudld be received only while in a ctive state. 
  RDUD module resets the CTA timeout on expiration of this timer.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dsrdudi_process_rdud_timer_expiry
(
  void
)
{
  ds_rdud_timer_struct ds_rdud_timer_report;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG1(MSG_LEGACY_MED, "Rcvd RDUD TIMER EXPIRED cmd - in %d state",
                  dsrdud.rdud_state);

  ds_rdud_timer_report.timer_state = DS_RDUD_EVENT_TIMER_EXPIRY;
  ds_rdud_timer_report.timer_value = 0;

  event_report_payload (EVENT_DS_RDUD_TIMER, 
                sizeof(ds_rdud_timer_struct), (void*)&ds_rdud_timer_report);

  switch( dsrdud.rdud_state )
  {
    case DS_RDUD_IDLE_STATE:
    {
      /*---------------------------------------------------------------------
        This happens only under some race conditions where, by the time we
        transition to IDLE state, timers were not yet cleaned completely
        - No harm, ignore the event.
      ---------------------------------------------------------------------*/
      break;
    }
    case DS_RDUD_ACTIVE_STATE:
    {
      /*---------------------------------------------------------------------
        RDUD timer expired - restore CTA idle timeout to the original value.
      ---------------------------------------------------------------------*/
      dsrdudi_rdud_transition_state(DS_RDUD_IDLE_STATE);
      break;
    }
    default:
    {
      return;
    }
  } /* switch rdud_state */
}/* dsrdudi_process_rdud_timer_expiry() */

/*===========================================================================
FUNCTION DSRDUDI_PROCESS_PHYS_LINK_DOWN

DESCRIPTION
  This module process PHYS_LINK_DOWN event received by the DS task
  (possibly different task if RDUD module moves to a different task in future)
  On receiving this, RDUD module moves to IDLE state and cancelse RDUD timer
  and restores CTA timeout  

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dsrdudi_process_phys_link_down
(
  void
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG1(MSG_LEGACY_MED, "Rcvd PHYS_LINK_DOWN cmd - in %d state", 
                  dsrdud.rdud_state);

  switch( dsrdud.rdud_state )
  {
    case DS_RDUD_IDLE_STATE:
    {
      /* Ignore */
      break;
    }
    /*-----------------------------------------------------------------------
      In all states on this event, transition to IDLE state and cancel all
      timers and restore CTA timeout.
    -----------------------------------------------------------------------*/
    case DS_RDUD_ACTIVE_STATE:
    {
      dsrdudi_rdud_transition_state(DS_RDUD_IDLE_STATE);
      break;
    }
    default:
    {
      return;
    }
  } /* switch rdud_state */
}/* dsrdudi_process_phys_link_down()*/

/*===========================================================================
  FUNCTION DSRDUDI_RDUD_TRANSITION_STATE()

  DESCRIPTION
    This function effects the transition to the RDUD state that is passed in.

  PARAMETERS
    new_state: the state to transition to

  RETURN VALUE
    None

  DEPENDENCIES
    Its the caller's responsibility that it initiates only valid state
    transitions. This function does not check if a state transition is
    valid or not.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsrdudi_rdud_transition_state
(
  dsrdudi_rdud_state_enum_type new_state
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG2(MSG_LEGACY_MED,  "RDUD goes from %d state to %d state", 
                  dsrdud.rdud_state,new_state);

  dsrdud.rdud_state = new_state;

  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state.
  -------------------------------------------------------------------------*/
  switch( dsrdud.rdud_state )
  {
    case DS_RDUD_IDLE_STATE:
    {
      /*---------------------------------------------------------------------
        Reset  restore CTA and RDUD timers 
      ---------------------------------------------------------------------*/
      dsrdudi_cancel_rdud_timer();
      ds707_pkt_restore_rdud_cta_timeout();
      dsrdudi_enabled = dsrdudi_enabled_new_val;
      break;
    }
    case DS_RDUD_ACTIVE_STATE:
    {
      /*---------------------------------------------------------------------
        set new cta timeout.
      ---------------------------------------------------------------------*/

      dsrdudi_start_rdud_timer(dsrdud.default_inactivity_restore_timeout);
      ds707_pkt_set_rdud_cta_timeout(
                               (dword)dsrdud.incoming_data_rlp_timeout/1000);
      break;
    }
    default:
    {
      return;
    }
  } /* switch rdud_state */
} /* dsrdudi_rdud_transition_state() */

/*===========================================================================

FUNCTION DSRDUD_STATE_IS_ACTIVE

DESCRIPTION
  This function checks if the rdud is in active state. Returns TRUE if in 
  active state else returns false.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: rdud state is active
  FALSE: rdud state is not active
  
SIDE EFFECTS
  None
===========================================================================*/
boolean dsrdud_state_is_active
(
  void
)
{ 
  if (dsrdud.rdud_state == DS_RDUD_ACTIVE_STATE)
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION      DSRDUD_IS_RDUD_ENABLED

DESCRIPTION   This function indicates if RDUD is enabled or not.

DEPENDENCIES  None.

RETURN VALUE  
  uint32 - CAM timer value in seconds

SIDE EFFECTS  None.
===========================================================================*/
boolean dsrdud_is_rdud_enabled
( 
  void 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG1(MSG_LEGACY_MED, "RDUD enabled is %d.", dsrdudi_enabled);

  return dsrdudi_enabled;
} /* dsrdud_is_rdud_enabled() */

/*===========================================================================
FUNCTION      DSRDUD_ENABLE_DISABLE_RDUD

DESCRIPTION   This function stores the RDUD enable value in an intermediate 
              variable if RDUD is not in IDLE state. This intermediate value 
              will be copied to "dsrdudi_enabled" when RDUD transitions to 
              IDLE state. If RDUD is not in IDLE state when this function is 
              called, the new RDUD enable value will be copied to 
              "dsrdudi_enabled" immediately.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dsrdud_enable_disable_rdud
( 
  boolean rdud_enable_val
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( dsrdud.rdud_state == DS_RDUD_IDLE_STATE)
  {
    dsrdudi_enabled = rdud_enable_val;
    DATA_IS707_MSG1(MSG_LEGACY_MED, "Setting RDUD enabled as %d.", 
                    dsrdudi_enabled);
  }
  else
  {
    dsrdudi_enabled_new_val = rdud_enable_val;
    DATA_IS707_MSG2(MSG_LEGACY_MED, 
                    "RDUD is in %d state. New RDUD enabled value %d will take \
                    effect for next call.", 
                    dsrdud.rdud_state, dsrdudi_enabled_new_val);
  }

  return;
} /* dsrdud_enable_disable_rdud() */

/*===========================================================================
FUNCTION      DSRDUD_READ_NV

DESCRIPTION   This function is used to read the inactivity 
              restoration time and set the NV item 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dsrdud_read_nv()
{
  nv_item_type      local_ds_nv_item;

  dsrdud.rdud_state = DS_RDUD_IDLE_STATE; 
  dsrdud.default_inactivity_restore_timeout =
                           DS_RDUD_DEFAULT_INACTIVITY_RESTORE_TIMEOUT;
  dsrdud.incoming_data_rlp_timeout = DS_RDUD_INCOMING_DATA_RLP_TIMEOUT;

  /*-------------------------------------------------------------------------
    Read inactivity restoration time, if not set set it to default value.
      -------------------------------------------------------------------------*/
  if( NV_DONE_S == ds3gcfgmgr_read_legacy_nv_ex(
      NV_DS_DEFAULT_INACTIVITY_RESTORE_TIMEOUT_I, &local_ds_nv_item, ds707_curr_subs_id() ) )
  {
    dsrdud.default_inactivity_restore_timeout =  
      local_ds_nv_item.ds_default_inactivity_restore_timeout;
  }
  else
  {
      local_ds_nv_item.ds_default_inactivity_restore_timeout =
          dsrdud.default_inactivity_restore_timeout;

    /*-----------------------------------------------------------------------
      Set the NV item to default values for all its components.
    -----------------------------------------------------------------------*/
    if(NV_DONE_S != ds3gcfgmgr_write_legacy_nv_ex(
       NV_DS_DEFAULT_INACTIVITY_RESTORE_TIMEOUT_I, &local_ds_nv_item, ds707_curr_subs_id()))
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "Unable to set the default_inactivity_restore_timeout"
                      "to default in NV");
    }
  } /* end else */

  /*-------------------------------------------------------------------------
    Read inactivity restoration time, if not set set it to default value.
  -------------------------------------------------------------------------*/
  if( NV_DONE_S == ds3gcfgmgr_read_legacy_nv_ex( NV_DS_INCOMING_DATA_RLP_TIMEOUT_I,
                                   &local_ds_nv_item, ds707_curr_subs_id() ) )
  {
    dsrdud.incoming_data_rlp_timeout =  
      local_ds_nv_item.ds_incoming_data_rlp_timeout;
  }
  else
  {
      local_ds_nv_item.ds_incoming_data_rlp_timeout =
          dsrdud.incoming_data_rlp_timeout;

    /*-----------------------------------------------------------------------
     Set the NV item to default values for all its components.
    -----------------------------------------------------------------------*/
    if(NV_DONE_S != ds3gcfgmgr_write_legacy_nv_ex( NV_DS_INCOMING_DATA_RLP_TIMEOUT_I,
                                   &local_ds_nv_item, ds707_curr_subs_id()))
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
          "Unable to set the ds_incoming_data_rlp_timeout to default in NV");
    }
  } /* end else */

}


#endif /* FEATURE_DS_RDUD */
