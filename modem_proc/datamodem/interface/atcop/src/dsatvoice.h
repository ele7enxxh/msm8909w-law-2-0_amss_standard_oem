#ifndef DSATVOICE_H
#define DSATVOICE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                        U M T S

                        C A L L   C O N T R O L
                
                A T   C O M M A N D   P R O C E S S I N G

DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( UMTS call control commands ) processor.

INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2001 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.   
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatvoice.h_v   1.5   12 Jul 2002 10:05:20   randrew  $
  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatvoice.h_v   1.0   29 Oct 2002 13:42:24   sbandaru  $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatvoice.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk/sc   Added support for Dynamic ATCoP.
03/27/14   pg      Fixed NOCARRIER response for command issue.
02/22/13   tk      ATCoP - CM interface changes for Triton DSDA.
11/30/12   tk      ATCoP changes for Triton DSDA.
01/19/12   sk      Feature cleanup.
05/31/10   ad      Added support for S7 command.
04/20/09   ua      Fixed lint errors.
08/03/06   ua      Adding VOICE_CMD_ATSO to voice_cmd_e_type 
04/07/06   snb     Provide dsatvoice_go_to_connecting_cmd_ext() & 
                   dsatvoice_is_voice_call() interfaces but maintain 
                   others for AT707 use.
11/11/05   ar      Return immediately to command mode for voice calls.
12/15/04   snb     Add handler for CM_CALL_EVENT_ORIG.
09/03/04   snb     Add +CHLD support.
03/19/04   snb     Make voice call state external to test prior to +CCWA 
                   unsolicited response.
09/04/03   ar      Adjust interface for dsatvoice_call_event_*().
10/17/01   sb      Created file

===========================================================================*/

#include "comdef.h"
#include "customer.h"


#include "dsati.h"
#include "dsatetsicall.h"
#include "cm_v.h"

/*===========================================================================

            PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains public definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/* State machine for number of possible calls:
   voice_state_e_type defines possible states,
   voice_state array is indexed by call ID,
    */
typedef enum 
{
  DSAT_VOICE_STATE_IDLE,       /* Not in or attempting a call */
  DSAT_VOICE_STATE_CONNECTING, /* Command issued to originate outgoing or answer incoming */
  DSAT_VOICE_STATE_ACTIVE,     /* Actively in voice call */
  DSAT_VOICE_STATE_WAITING,    /* Incoming event received */
  DSAT_VOICE_STATE_LCH         /* Local call hold */
} voice_state_e_type;

typedef enum 
{
  VOICE_CMD_NONE,    /* No command active  */
  VOICE_CMD_ATD,     /* ATD command active */
  VOICE_CMD_ATA,     /* ATA command active */
  VOICE_CMD_ABORT,    /* Aborting current command */
  VOICE_CMD_ATS0,    /* S0 is set */
  VOICE_CMD_ATS7     /* S7 timer timed */
} voice_cmd_e_type;

typedef struct {
  voice_state_e_type state;
  voice_cmd_e_type   cmd_active;
  boolean  cmd_pending;
  cm_call_mode_info_e_type call_mode;
  cm_call_id_type           call_id;
} voice_state_type;

extern voice_state_type voice_state[DSAT_SYS_MODEM_AS_ID_MAX][CM_CALL_ID_MAX];
extern rex_timer_type dsatvoice_ring_timer;
extern rex_timer_type dsatvoice_ring_timer_2;
extern rex_timer_type dsatvoice_voice_connection_timer;
extern rex_timer_type dsatvoice_voice_connection_timer_2;
extern rex_timer_type dsatvoice_data_connection_timer;
extern rex_timer_type dsatvoice_data_connection_timer_2;
extern cm_call_id_type dsatvoice_voice_call_id[DSAT_SYS_MODEM_AS_ID_MAX];
extern cm_call_id_type dsatvoice_data_call_id[DSAT_SYS_MODEM_AS_ID_MAX];
extern sys_sys_mode_e_type dsatvoice_sys_mode[DSAT_SYS_MODEM_AS_ID_MAX];
/* Macro to indicate all calls IDs */
#define DSAT_CALLID_ALL    CM_CALL_ID_ALL_CALLS 

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DSATVOICE_INIT

DESCRIPTION
  Initiate voice call related vaiables.
  It includes the ring timer.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_init( void );


/*===========================================================================

FUNCTION DSATVOICE_ANSWER_CMD

DESCRIPTION
  Handle ATA voice answer commands.  This function is called by 
  dsatact_exec_ata_cmd when the current call's call type is VOICE.

  This function sends request to CM to answer current incoming call.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : If the command succeeds.
  DSAT_NO_CARRIER: If the request is rejected by CM.
  DSAT_ERROR     : If ATA is not expected.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_answer_cmd
(
  sys_modem_as_id_e_type subs_id
);


/*===========================================================================

FUNCTION DSATVOICE_END_VOICE_CALL

DESCRIPTION
  This function is called by dsat707_exec_qcchv_cmd to end current
  incoming call if it is a voice call.

  This function issues a request to CM to end a voice call.

DEPENDENCIES

RETURN VALUE
  DSAT_ASYNC_CMD : The end request is sent, callback function
                   will be called.
  DSAT_ERROR     : The end request failed.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvoice_end_voice_call
(
  sys_modem_as_id_e_type subs_id
);


/*===========================================================================

FUNCTION DSATVOICE_VOICE_CALL_ABORT_HANDLER

DESCRIPTION
  This function is called by dsatact_call_abort_handler to abort
  voice dial command.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_voice_call_abort_handler
(
  sys_modem_as_id_e_type subs_id
);


/*===========================================================================

FUNCTION DSATVOICE_CALL_EVENT_INCOM

DESCRIPTION
  This function is called by dsatcmif_cm_call_event_handler when CM 
  report the CM_CALL_EVENT_INCOM event.

  If the call type is VOICE, it rings the TE and goes to WAITING_FOR_ANSWER
  state.

DEPENDENCIES

RETURN VALUE
  DSAT_ASYNC_EVENT : Tell the async event handler this event is not
                      associated with an AT command line.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_call_event_incom
(
  sys_modem_as_id_e_type           subs_id,
  const ds_at_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
);


/*===========================================================================

FUNCTION DSATVOICE_CALL_EVENT_HO_COMPLETE

DESCRIPTION

  function will be called for call hand over complete. 
  Calls will move from IP to GW

DEPENDENCIES

RETURN VALUE
  DSAT_ASYNC_EVENT : Tell the async event handler this event is not
                      associated with an AT command line.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_call_event_ho_complete
(
  sys_modem_as_id_e_type           subs_id,
  const ds_at_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
);

/*===========================================================================

FUNCTION DSATVOICE_CALL_EVENT_CONNECT

DESCRIPTION
  This function is called by dsatvoice_cm_call_event_handler when CM 
  report the CM_CALL_EVENT_CONNECT event.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK          : if current state is WAITING_FOR_ANSWER or DIALING
                     state, go to CONNECTED state
  DSAT_NO_RSP      : if current state is IDLE_STATE go to 
                     CONNECTED_OTHER_CLIENT state
  DSAT_ASYNC_EVENT : if the call type is not VOICE

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_call_event_connect
(
  sys_modem_as_id_e_type           subs_id,
  const ds_at_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
);


/*===========================================================================

FUNCTION DSATVOICE_CALL_EVENT_END

DESCRIPTION
  This function is call by dsatcmif_cm_call_event_handler when CM report 
  the CM_CALL_EVENT_END event.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_NO_CARRIER : if the current state is not one of IDLE_STATE, 
                    CONNECTED_OTHER_CLIENT, WAITING_FOR_ANSWER.
  DSAT_NO_RSP     : if the current state is WAITING_FOR_ANSWER.
  DSAT_ASYNC_EVENT: otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_call_event_end
(
  sys_modem_as_id_e_type           subs_id,
  const ds_at_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
);


/*===========================================================================

FUNCTION DSATVOICE_CALL_EVENT_ORIG

DESCRIPTION
  This function is called when CM reports the CM_CALL_EVENT_ORIG event.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK          : if current state is WAITING_FOR_ANSWER or DIALING
                     state, go to CONNECTED state
  DSAT_ASYNC_EVENT : if the call type is not VOICE

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_call_event_orig
(
  sys_modem_as_id_e_type           subs_id,
  const ds_at_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
);

#ifdef FEATURE_DUAL_ACTIVE
/*===========================================================================

FUNCTION DSATVOICE_CALL_EVENT_LOCAL_HOLD

DESCRIPTION
  This function is called by dsatcmif_cm_call_event_handler when CM
  report the CM_CALL_EVENT_LOCAL_HOLD event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_call_event_local_hold
(
  sys_modem_as_id_e_type           subs_id,
  const ds_at_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
);

/*===========================================================================

FUNCTION DSATVOICE_VALIDATE_CALL_CMD

DESCRIPTION
  This function validates whether a call command on a specific
  subscription can be issued or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : If the call command can be issued on the subscription.
  FALSE: Otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatvoice_validate_call_cmd
(
  sys_modem_as_id_e_type subs_id,
  dsat_call_cmd_e_type   call_cmd
);
#endif /* FEATURE_DUAL_ACTIVE */

/*===========================================================================

FUNCTION  DSATVOICE_TIMER_EXPIRED_HANDLER 

DESCRIPTION
  This function is the handler function for the RI timer expired cmd.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvoice_timer_expired_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);


/*===========================================================================

FUNCTION DSATVOICE_CMD_ORIG_CB_HANDLER

DESCRIPTION
  This function is the handler for a CM_CALL_CMD_ORIG cb cmd.

  This function handles cm_call_cmd_orig call status.  It returns
  dsat_result based on err conditions.  Go to IDLE_STATE if there is
  error.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK        : if no error reported by CM and +COLP disabled
  DSAT_ASYNC_CMD : if no error reported by CM and +COLP enabled
  DSAT_NO_CARRIER: if the one of the following errors is reported by CM,
                   CM_CALL_CMD_ERR_OFFLINE_S, 
                   CM_CALL_CMD_ERR_ORIG_RES_S,
                   CM_CALL_CMD_ERR_EMERGENCY_MODE_S, 
                   CM_CALL_CMD_ERR_SRV_TYPE_S,
                   CM_CALL_CMD_ERR_CALL_STATE_S,
                   CM_CALL_CMD_ERR_SRV_STATE_S
  DSAT_BUSY      : if the following error is reported by CM,
                   CM_CALL_CMD_ERR_IN_USE_S
  DSAT_ERROR     : other CM errors

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_cmd_orig_cb_handler
(
  sys_modem_as_id_e_type    subs_id,
  cm_call_cmd_err_e_type    call_cmd_err
  /* Call command error code, indicating whether command
  ** is rejected and the reason */
);

/*===========================================================================
FUNCTION DSATVOICE_CMD_ANSWER_CB_HANDLER

DESCRIPTION
  This function is the handler for CM_CALL_CMD_ANSWER cb cmd.

  If status shows cm_call_cmd_answer failed, return NO CARRIER to TE and
  go back to IDLE_STATE.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : no error.
  DSAT_NO_CARRIER: there is error.

SIDE EFFECTS
  None
  
===========================================================================*/

dsat_result_enum_type dsatvoice_cmd_answer_cb_handler
(
  sys_modem_as_id_e_type    subs_id,
  cm_call_cmd_err_e_type    call_cmd_err
  /* Call command error code, indicating whether command
  ** is rejected and the reason */
);


/*===========================================================================

FUNCTION DSATVOICE_CMD_END_CB_HANDLER

DESCRIPTION
  This function is the handler for CM_CALL_CMD_END cb cmd.

  If status shows cm_call_cmd_end succeeded, return OK to TE and 
  go to IDLE_STATE. 
  If status shows cm_call_cmd_end failed, return ERROR to TE.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK   : if no error.
  DSAT_ERROR: if there is error.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_cmd_end_cb_handler
(
  sys_modem_as_id_e_type    subs_id,
  cm_call_cmd_err_e_type    call_cmd_err
  /* Call command error code, indicating whether command
  ** is rejected and the reason */
);


/*===========================================================================

FUNCTION DSATVOICE_IS_VOICE

DESCRIPTION
  Test if there is any incoming voice call.
  This function used by AT707 code.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: if there is any incoming voice call.
  FALSE: otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatvoice_is_voice
(
  sys_modem_as_id_e_type subs_id
);


/*===========================================================================

FUNCTION DSATVOICE_IS_VOICE_CALL

DESCRIPTION
  Test if there is voice call for given call ID.  All call IDs will be
  checked if DSAT_CALLID_ALL is passed in, reporting those in DSAT_VOICE_STATE_WAITING
  state.  For specific call ID, returns true if not DSAT_VOICE_STATE_IDLE state.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: if there is voice call for given call ID.
  FALSE: otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatvoice_is_voice_call
(
  sys_modem_as_id_e_type  subs_id,
  cm_call_id_type         call_id
);

/*===========================================================================

FUNCTION DSATVOICE_GO_TO_CONNECTING

DESCRIPTION
  Change a call ID to DSAT_VOICE_STATE_CONNECTING state, 
  assumes call orig type command active.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_go_to_connecting
(
  sys_modem_as_id_e_type  subs_id,
  cm_call_id_type         call_id
);

/*===========================================================================

FUNCTION DSATVOICE_GO_TO_CONNECTING_CMD_EXT

DESCRIPTION
  Change a call ID to DSAT_VOICE_STATE_CONNECTING state and set active command.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_go_to_connecting_cmd_ext
(
  sys_modem_as_id_e_type  subs_id,
  cm_call_id_type         call_id,
  voice_cmd_e_type        cmd_active
);

/*===========================================================================

FUNCTION DSATVOICE_GO_TO_IDLE_STATE

DESCRIPTION
  Change a call ID to DSAT_VOICE_STATE_IDLE state.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_go_to_idle_state
(
  sys_modem_as_id_e_type  subs_id,
  cm_call_id_type         call_id
);

/*===========================================================================

FUNCTION VOICE_STOP_RING

DESCRIPTION
  Stop ringing TE.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void voice_stop_ring
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================

FUNCTION VOICE_SEND_RING_RESULT

DESCRIPTION
  Sends ring result code.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void voice_send_ring_result
(
  sys_modem_as_id_e_type      subs_id,
  cm_call_mode_info_e_type  call_mode
);

#endif /* DSATVOICE_H */

