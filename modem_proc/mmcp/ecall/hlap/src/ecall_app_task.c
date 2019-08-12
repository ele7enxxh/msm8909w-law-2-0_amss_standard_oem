/*===========================================================================

                           E C A L L _ A P P _ T A S K . C

DESCRIPTION
  eCall task source file

REFERENCES
  3GPP TS 26.267

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/hlap/src/ecall_app_task.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $
===========================================================================*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_ECALL_APP

#include "rex.h"
#include "err.h"
#include "msg.h"
#include "queue.h"
#include "event.h"

#include "ecall_app_i.h"
#include "ecall_app_task.h"
#include "ecall_ivs_task.h"
#include "ecall_modem_ivs.h"
#include "ecall_modem_apis.h"
#include "ecall_gps.h"
#include "ecall_msd.h"
#include "ecall_efs.h"
#include "ecall_diag.h"
#include "ecall_sim.h"
#include "fs_public.h"
#include "modem_mem.h"   /* interface for modem_mem_alloc */
#include "ecall_modem_clients.h"
#include "sys_cnst.h"

#if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
#include "vss_common_public_if.h"
#include "vss_istream_public_if.h"
#include "vss_ivolume_public_if.h"
#else /* Legacy Vocoder */
#include "voc.h"
#endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

#ifdef FEATURE_ECALL_HAS_V1_HEADER
#include "task.h"
#endif /* FEATURE_ECALL_HAS_V1_HEADER */

#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

#ifdef FEATURE_ECALL_HAS_DOG_HB
#include "dog_hb_rex.h"
#else /* Legacy Dog */
#include "dog.h"
#endif /* FEATURE_ECALL_HAS_DOG_HB */

/* ----------------------------
** Define watchdog report handle
** ---------------------------- */
#ifdef FEATURE_ECALL_HAS_DOG_HB
#undef    DOG_ECALL_APP_RPT
#define   DOG_ECALL_APP_RPT       ecall_app_dog_rpt_var
static    dog_report_type         ecall_app_dog_rpt_var = 0;      /* Initial Safety */
#else /* Legacy Dog */
/* Dynamic Dog Interface */
#ifdef DOG_DYNAMIC_API   
  #undef  DOG_ECALL_APP_RPT
  #define DOG_ECALL_APP_RPT       ecall_app_dog_rpt_var
  static dog_report_type          ecall_app_dog_rpt_var  = 0;     /* Initial Safety */

  #undef  DOG_ECALL_APP_RPT_TIME
  #define DOG_ECALL_APP_RPT_TIME  ecall_app_dog_rpt_time_var
  static uint32                   ecall_app_dog_rpt_time_var   = 0xffff; /* Initial Safety */
#endif   /* DOG_DYNAMIC_API */
#endif /* FEATURE_ECALL_HAS_DOG_HB */

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

/*===========================================================================

                         I N T E R N A L    D A T A

===========================================================================*/

#define NUM_OF_ECALL_STATES                    (6)
#define ECALL_APP_TASK_CMD_BUF_COUNT           (8)
#define ECALL_REDIAL_ATTEMPT_MAX               (10) /* TS 122 001 Annex E. Maximum allowable redial attempt is 10 times. */
#define ECALL_CALL_ORIG_FAIL_REDIAL_INDEX_MAX  (10) /* If call origination fails, we should follow TS 122 001 Annex E */
#define ECALL_CALL_DROPPED_REDIAL_INDEX_MAX    (2)  /* If call dropped, we should follow both TS 122 001 Annex E and 
                                                       CEN 16062 Clause 7.12.4 */

/* Define a variable to hold the ECALL_APP task TCB.  This is done for
   a several reasons:
    1) The return value of rex_self() never changes at runtime, so
       there is nothing gained by repeatedly fetching it.
    2) Directly using ecall_app_tcb requires that 'task.h' be included.
       It is now prohibited to include 'task.h'.
    3) The use of rex_self() cannot be used in any context where
       the ecall_app_tcb is specifically required (such as when a command
       is placed into a queue and the task signal is set).
    4) A task's tcb is not available if the task is not started.
       Worse, signaling a task that isn't running causes an exception.
    5) The variable allows access to the correct tcb in case 3 and
       provides state to use in determining if the task is running.
*/
/* To be initialized to rex_self() when task is running */
static rex_tcb_type *ecall_app_myself = NULL;

/* Command queue */
static ecall_app_task_cmd_type   ecall_app_task_cmd_buf[ECALL_APP_TASK_CMD_BUF_COUNT];
static q_type                    ecall_app_task_cmd_free_q;
static q_type                    ecall_app_task_cmd_q;

#ifdef FEATURE_ECALL_HAS_MMOC
#include "msgr_rex.h"
#define ECALL_APP_OFFSETOF(type,member)    (((char*)(&((type*)1)->member))-((char*)1))
#define ECALL_APP_MSGR_Q_SIZE    20
static q_type                    ecall_app_msgr_q;
static q_type                    ecall_app_msgr_free_q;
static ecall_app_msgr_cmd_s_type ecall_app_msgr_bufs[ECALL_APP_MSGR_Q_SIZE];
static msgr_client_t             ecall_app_msgr_client;
static msgr_id_t                 ecall_app_msgr_rex_q_id;
#endif /* FEATURE_ECALL_HAS_MMOC */

static ecall_client_type ecall_app_client_struct;

typedef struct {

  #ifndef FEATURE_ECALL_HAS_DOG_HB /* Legacy Dog */
  /* Watchdog timer */
  rex_timer_type ecall_app_rpt_timer;
  #endif /* Legacy Dog */
  /* change the ecall_app_state when GPS is requested, recvd, in call, idle, etc. */
  ecall_app_state_type ecall_app_state;
  /* Start the timer when in call and get a GPS Request, timesout when GPS fix is not received in this time */
  rex_timer_type gps_timer;
   /* Start the timer when Rx START, timesout when Session Complete is not received in this time */
  rex_timer_type session_timer;   
  /* Start the timer when ecall call is up, timesout when START is not received in this time */      
  rex_timer_type start_timer;
  /* This timer is for the auto-answer feature for MO call */
  rex_timer_type ecall_callback_timer;
 /* Timer to CALL_CLEARDOWN_TIMER */
  rex_timer_type call_cleardown_timer;
  /* Start the timer when 2 LL ACKS are received, timesout in 2 secs */      
  rex_timer_type al_ack_timer;
  /* Retry the call in case of call failed or call dropped */
  rex_timer_type retry_timer;
  /* Start the timer when eCall modem lose sync and does not receive HLACK, timeout in 3 second if no new message is decoded */
  rex_timer_type unmute_timer;
  
  /* 1= MO call, 0 = MT Call */
  call_direction_type   call_direction; 
  ecall_tx_mode_type tx_mode; /* PUSH or PULL */
  boolean ecall_app_task_discard_msd; /* Sent to ecall_abort_transmit_data - for IVS to decide if MSD should be discarded */
  cm_client_id_type  ecall_app_task_cm_client_id;
  boolean redial_pending;
  ecall_msd_array_type msd_data_buff;
  ecall_efs_config_type ecall_efs_config;
  ecall_session_status_type  session_status;
  boolean ecall_enabled_nv;
  boolean emodem_client_reg;
  boolean ecall_ivs_cleardown_received; /* HLAP EN 16062 7.12.4 */
  boolean ecall_ivs_positive_ack_received;
  uint8 call_dropped_redial_index;
  uint8 call_orig_fail_redial_index;
  boolean is_call_fail_at_orig;
  boolean pending_mute;
  boolean pending_unmute;
  boolean hlack_received;
  cm_call_id_type ecall_app_task_call_id;
  ecall_start_session_cb_type start_session_cb_func;
  ecall_call_orig_info_cb_f_type call_orig_info_cb_func;
  ecall_call_orig_exten_info_cb_f_type call_orig_exten_info_cb_func;
  void *data_block_ptr;
  boolean gps_engine_initialized;
  boolean gps_engine_warmup;
  boolean start_received_is_with_msd;
  boolean is_session_start_with_mt_ecall;
  boolean is_session_timer_expired;
  boolean is_era_glonass_enabled;
  boolean is_external_msd;
  uint8 external_msd[ECALL_MSD_MAX_LENGTH];
} ecall_app_task_struct_type;

/* static memory for the ECALL APP task struct*/
ecall_app_task_struct_type ecall_app_task_mem;

ecall_app_task_struct_type *ecall_app_task_struct = &ecall_app_task_mem; 

LOCAL const boolean ecall_is_valid_trans[NUM_OF_ECALL_STATES][NUM_OF_ECALL_STATES] = 
{
  /* To IDLE_NO_ECALL    CALL_ORIG     IN_TX   WAIT_AL_ACK  IN_CALL_VOICE  IDLE_ALLOW_MT_ECALL  */
  /*             0           1           2           3           4           5                  */
  /* 0 */ {    FALSE,      TRUE,       FALSE,      FALSE,      FALSE,      FALSE  }, /* From IDLE_NO_ECALL  */
  /* 1 */ {    TRUE,       FALSE,      TRUE,       FALSE,      FALSE,      TRUE   }, /* From CALL_ORIG      */
  /* 2 */ {    TRUE,       TRUE,       FALSE,      TRUE,       TRUE,       TRUE   }, /* From IN_TX*/
  /* 3 */ {    TRUE,       TRUE,       TRUE,       TRUE,       TRUE,       TRUE   }, /* From WAIT_AL_ACK */
  /* 4 */ {    TRUE,       TRUE,       TRUE,       FALSE,      FALSE,      TRUE   }, /* From IN_CALL_VOICE */
  /* 5 */ {    TRUE,       TRUE,       FALSE,      FALSE,      TRUE,       FALSE  }, /* From IDLE_ALLOW_MT_ECALL */
};

/* TS 122 001 Annex E, Category 1 & 2: Maximum 10 retries are allowed  */
LOCAL const uint32 ecall_redial_duration[ECALL_REDIAL_ATTEMPT_MAX] = 
{
  /* This table specified the minimum time duration between the previous call attempt  */
  5000,                                           /* 1st redial attempt: 5 secs */ 
  60000, 60000, 60000,                            /* 2nd-4th redial attempts: 1 min  */ 
  180000, 180000, 180000, 180000, 180000, 180000  /* 5th-10th redial attempts: 3 mins */
};

static void ecall_app_task_process_cmd
(
  void
);

#if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
/* Vocoder PCM parameters */
static uint16 ecall_app_apr_src_addr;
static uint16 ecall_app_apr_dest_addr;
static uint16 ecall_app_apr_src_port = 0xCCCC;
static uint16 ecall_app_apr_dest_port = 0;
static char_t ecall_app_stream_name[] = "default modem voice";
static uint32 ecall_app_apr_handle;
static char ecall_app_ecall_dns[] = "qcom.modem.ecall_app";
static char ecall_app_voc_dns[] = "qcom.audio.cvs";

/* Tokens for APR commands */
typedef enum
{
  ECALL_APP_APR_CREATE_PASSIVE_CONTROL_SESSION = 1, /* VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION */
  ECALL_APP_APR_MUTE_RX_OUT,                        /* VSS_IVOLUME_CMD_MUTE with MUTE_ON */
  ECALL_APP_APR_UNMUTE_RX_OUT                       /* VSS_IVOLUME_CMD_MUTE with MUTE_OFF */
}ecall_app_apr_token_type;
#endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */


/*==========================================================================

  FUNCTION ECALL_APP_TASK_TX_STATUS_CB_FUNC

  DESCRIPTION
    Call back function sent to START_TRANSMIT_DATA, this function handles all the MSD transmission states

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_app_task_tx_status_cb_func
(
  void              *user_data,
 ecall_cb_data_type ivs_status
);

/*==========================================================================

  FUNCTION ECALL_APP_TASK_CHANGE_STATE

  DESCRIPTION
    This function will set the state of the ecall App - waiting for GPS, in call, idle, etc.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_app_task_change_state
(
  ecall_app_state_type new_state
);


/*==========================================================================

  FUNCTION ECALL_GET_CM_CLIENT_ID

  DESCRIPTION
    This api returns the CM client ID for ecall App task

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
===========================================================================*/
cm_client_id_type ecall_get_cm_client_id
(
  void
)
{
  return ecall_app_task_struct->ecall_app_task_cm_client_id;
} /* ECALL_GET_CM_CLIENT_ID() */

/*==========================================================================

  FUNCTION ECALL_APP_TASK_PUT_CMD

  DESCRIPTION
    Adds a command pointer to the command queue and signals the eCall task
    to process it.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    eCall task is signaled that there is a pending command

===========================================================================*/
void ecall_app_task_put_cmd
(
  ecall_app_task_cmd_type  *cmd_ptr
)
{
  q_put(&ecall_app_task_cmd_q, &cmd_ptr->link);
  
  (void) rex_set_sigs( ecall_app_myself, ECALL_APP_CMD_Q_SIG );

} /* ECALL_APP_TASK_PUT_CMD() */

/*==========================================================================

  FUNCTION ECALL_APP_TASK_GET_CMD_BUF

  DESCRIPTION
    Returns a command pointer off of the free queue, or NULL on failure.

  DEPENDENCIES
    None.

  RETURN VALUE
    Command pointer or NULL

  SIDE EFFECTS
    None

===========================================================================*/
ecall_app_task_cmd_type *ecall_app_task_get_cmd_buf(void)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;
  if ((cmd_ptr = q_get(&ecall_app_task_cmd_free_q)) == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "No items on eCall task free cmd q" );
  }
  
  return cmd_ptr;

} /* ECALL_APP_TASK_GET_CMD_BUF() */


/*==========================================================================

  FUNCTION ECALL_APP_TASK_QUEUE_CMD

  DESCRIPTION
    This is an internal function that will call the ecall_app_task_get_cmd_buf()
    and queue up the command, if this fails, it will reset the ecall state machine to INACTIVE.

  DEPENDENCIES
    None.

  RETURN VALUE
    Command pointer or NULL

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_task_queue_cmd
(
  ecall_app_task_cmd_id_e_type   cmd_id
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;

  cmd_ptr = ecall_app_task_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    /* Post a command to the eCall App task to process this PCM frame */
    cmd_ptr->cmd_id = cmd_id;

    ecall_app_task_put_cmd(cmd_ptr);
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_app_task_get_cmd_buf ran out of command buffers, going to INACTIVE state " );
    ecall_app_task_change_state(ECALL_APP_ECALL_INACTIVE);
  }
 
} /* ECALL_APP_TASK_QUEUE_CMD() */



/* START: New State tranision changes */

/*==========================================================================

  FUNCTION ECALL_EXIT_STATE_IDLE_ALLOW_MT_ECALL

  DESCRIPTION
    Exiting this state means no more MT ecalls

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_exit_state_idle_allow_mt_ecall
(
  void
)
{
  ECALL_MSG_0( ECALL_MED, "Exit STATE Idle ALLOW MT ECALL " );

} /* ECALL_EXIT_STATE_IDLE_ALLOW_MT_ECALL */

/*==========================================================================

  FUNCTION ECALL_EXIT_STATE_INACTIVE

  DESCRIPTION
    This function will exit the idle state and start looking for GPS depending on the test configuration

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_exit_state_inactive
(
  void
)
{
  ECALL_MSG_0( ECALL_MED, "Exit STATE Idle No ECALL " );

} /* ECALL_EXIT_STATE_INACTIVE */


/*==========================================================================

  FUNCTION ECALL_EXIT_STATE_IN_CALL

  DESCRIPTION
    This function will do the cleanup that is needed after ending the call

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_exit_state_in_call
(
  void
)
{
  ECALL_MSG_0( ECALL_MED, "Exit STATE in_call " );

} /* ECALL_EXIT_STATE_IN_CALL */


/*==========================================================================

  FUNCTION ECALL_EXIT_STATE_ORIGINATING_CALL

  DESCRIPTION
    This function will to the cleanup before exiting state ECALL_APP_IN_CALL_WAITING_FOR_GPS.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_exit_state_originating_call
(
  void
)
{
  ECALL_MSG_0( ECALL_MED, "Exit STATE originating call " );

} /* ECALL_EXIT_STATE_ORIGINATING_CALL */

/*==========================================================================

  FUNCTION ECALL_EXIT_STATE_IN_CALL_TRANSMITTING

  DESCRIPTION
    This function will to the cleanup before exiting state ECALL_APP_IN_CALL_WAITING_FOR_GPS.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_exit_state_in_call_transmitting
(
  void
)
{
  ECALL_MSG_0( ECALL_MED, "Exit STATE in_call_transmitting " );
  (void)rex_clr_timer(&ecall_app_task_struct->session_timer);
} /* ECALL_EXIT_STATE_IN_CALL_TRANSMITTING */

/*==========================================================================

  FUNCTION ECALL_EXIT_STATE_WAITING_FOR_AL_ACK

  DESCRIPTION
    This function will to the cleanup before exiting state ECALL_APP_IN_CALL_WAITING_FOR_GPS.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_exit_state_waiting_for_al_ack
(
  void
)
{
  ECALL_MSG_0( ECALL_MED, "Exit STATE in_call_waiting_for_al_ack " );

  (void)rex_clr_timer(&ecall_app_task_struct->al_ack_timer);
 
} /* ECALL_EXIT_STATE_WAITING_FOR_AL_ACK */

/*==========================================================================

  FUNCTION ECALL_ENTER_STATE_INIT

  DESCRIPTION
    This function will enter the INIT state (should never happen)

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_enter_state_init
(
  void
)
{ 
  ECALL_MSG_0( ECALL_MED, "ecall_enter_state_init DO NOTHING" );

} /* ecall_enter_state_iNIT */



/*==========================================================================

  FUNCTION ECALL_ENTER_STATE_INACTIVE

  DESCRIPTION
    This function is the entry of ecall app task .

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_enter_state_inactive
(
  void
)
{

  ECALL_MSG_0( ECALL_MED, "ENTERED STATE IDLE " );
  
   /* Clear all the timers */
  (void)rex_clr_timer(&ecall_app_task_struct->session_timer);
  (void)rex_clr_timer(&ecall_app_task_struct->start_timer);
  (void)rex_clr_timer(&ecall_app_task_struct->al_ack_timer);
  (void)rex_clr_timer(&ecall_app_task_struct->ecall_callback_timer);
  (void)rex_clr_timer(&ecall_app_task_struct->call_cleardown_timer);
  (void)rex_clr_timer(&ecall_app_task_struct->unmute_timer);

  /* Reset type_of_call and redial_index only if it's not redialing */
  if(ecall_app_task_struct->redial_pending == FALSE)
  {
    ECALL_MSG_0( ECALL_MED, "No redial attempts, go reset type_of_call and redial_index" );
    ecall_reset_type_of_call();
    ecall_app_task_struct->call_dropped_redial_index = 0;
    ecall_app_task_struct->call_orig_fail_redial_index = 0;
  }

  /* Reset ecall_ivs_cleardown_received, HLAP EN 16062 7.12.4 */
  ecall_app_task_struct->ecall_ivs_cleardown_received = FALSE;

  /* Reset ecall_ivs_positive_ack_received and is_session_timer_expired if no redial is pending */
  if (ecall_app_task_struct->redial_pending == FALSE)
  {
    ecall_app_task_struct->ecall_ivs_positive_ack_received = FALSE;
    ecall_app_task_struct->is_session_timer_expired = FALSE;
  }

  ecall_app_task_struct->hlack_received = FALSE;
  ecall_app_task_struct->start_received_is_with_msd = FALSE;

  /* FULL Abort current transmission */
  report_ecall_event(EVENT_ECALL_STOP);

  /* Unmute Vocoder RX output to Speaker */
  if(ecall_app_task_struct->ecall_efs_config.voc_config == TRUE) 
  {
    ecall_app_unmute_rx_out();
  }

} /* ECALL_ENTER_STATE_INACTIVE */



/*==========================================================================

  FUNCTION ECALL_ENTER_STATE_IDLE_ALLOW_MT_ECALL

  DESCRIPTION
    In this state, the IVS starts the callback timer and will allow the MT ecall until this timer expires

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_enter_state_idle_allow_mt_ecall
(
  void
)
{
  ECALL_MSG_0( ECALL_MED, "ENTERED STATE idle_allow_mt_ecall " );

  /* Clear all the timers */
  (void)rex_clr_timer(&ecall_app_task_struct->session_timer);
  (void)rex_clr_timer(&ecall_app_task_struct->start_timer);
  (void)rex_clr_timer(&ecall_app_task_struct->al_ack_timer);
  (void)rex_clr_timer(&ecall_app_task_struct->unmute_timer);

  /* Reset type_of_call and redial_index only if it's not redialing */
  if(ecall_app_task_struct->redial_pending == FALSE)
  {
    ECALL_MSG_0( ECALL_MED, "No redial attempts, go reset type_of_call and redial_index" );
    /* If is_session_start_with_mt_ecall is TRUE, it means the motorist wants to start
     * a new MO eCall session during a MT eCall. Type of call should only be reset when
     * is_session_start_with_mt_ecall is FALSE.
     */
    if (ecall_app_task_struct->is_session_start_with_mt_ecall == FALSE)
    {
      ecall_reset_type_of_call();
    }
    ecall_app_task_struct->call_dropped_redial_index = 0;
    ecall_app_task_struct->call_orig_fail_redial_index = 0;
  }

  /* Reset ecall_ivs_cleardown_received, HLAP EN 16062 7.12.4 */
  ecall_app_task_struct->ecall_ivs_cleardown_received = FALSE;

  /* Reset ecall_ivs_positive_ack_received and is_session_timer_expired if no redial is pending */
  if (ecall_app_task_struct->redial_pending == FALSE)
  {
    ecall_app_task_struct->ecall_ivs_positive_ack_received = FALSE;
    ecall_app_task_struct->is_session_timer_expired = FALSE;
  }

  ecall_app_task_struct->hlack_received = FALSE;
  ecall_app_task_struct->start_received_is_with_msd = FALSE;

  /* Unmute Vocoder RX output to Speaker */
  if(ecall_app_task_struct->ecall_efs_config.voc_config == TRUE) 
  {
    ecall_app_unmute_rx_out();
  }

} /* ECALL_ENTER_STATE_IDLE_ALLOW_MT_ECALL */

/*==========================================================================

  FUNCTION ECALL_ENTER_STATE_IN_CALL

  DESCRIPTION
    This function will originate a call.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_enter_state_in_call
(
  void
)
{
  ECALL_MSG_0( ECALL_MED, "ENTERED STATE IN_CALL " );

  ecall_abort_transmit_data(ecall_app_task_struct->ecall_app_task_discard_msd);

  /* Unmute Vocoder RX output to Speaker */
  if(ecall_app_task_struct->ecall_efs_config.voc_config == TRUE) 
  {
    ecall_app_unmute_rx_out();
  }
  
} /* ecall_enter_state_in_call */


/*==========================================================================

  FUNCTION ECALL_ENTER_STATE_ORIGINATING_CALL

  DESCRIPTION
    This function will originate a call.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_enter_state_originating_call
(
  void
)
{
  ECALL_MSG_0( ECALL_MED, "ENTERED STATE originating call " );

  if (ecall_originate_call() == FALSE )
  {
    ECALL_MSG_0( ECALL_ERROR, "FAILURE in Originating ECALL (failure to read FDN/SDN or originating call)" );
    ecall_app_task_change_state(ECALL_APP_ECALL_INACTIVE);
  }

} /* ecall_enter_state_originating_call */


/*==========================================================================

  FUNCTION ECALL_ENTER_STATE_IN_CALL_TRANSMITTING

  DESCRIPTION
    This function will constrcut the MSD and start TX of the MSD

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_enter_state_in_call_transmitting
(
  void
)
{
  uint16 p;
  ECALL_MSG_0( ECALL_MED, "ENTERED STATE in_call_transmitting " );

  /* Mute the path from Vocoder RX output to Speaker input for MO call,
     so the voice does not disturb the transmission (sync msg tone does
     not go into the mic) */
  if ( (ecall_app_task_struct->ecall_efs_config.voc_config == TRUE) &&
       (ecall_app_task_struct->call_direction == ECALL_MO_CALL) )
  {
    ecall_app_mute_rx_out();
  }
  
  /* Ends the current active eCall GPS session to maintain MSD transmission performance */
  ECALL_MSG_0( ECALL_MED, "eCall ends active GPS session to maintain MSD TX performance" );
  ecall_end_position_session();

  /* Do not Call ecall_start_transmit_data() if eCall MODEM already has MSD  */
  if ( ecall_app_task_struct->start_received_is_with_msd == FALSE ) /* If not already in Call */
  {

    ECALL_MSG_2( ECALL_HIGH, "STARTING TX WITH %d mode (push = 1, pull = 0), external MSD %d (external MSD = 1, real MSD = 0)",
                              ecall_app_task_struct->tx_mode,
                              ecall_app_task_struct->is_external_msd );

    /* Construct MSD */
    memset(&ecall_app_task_struct->msd_data_buff, 0, sizeof(ecall_app_task_struct->msd_data_buff));
    /* If the setting is to use external MSD, eCall App will not construct the real MSD */
    if (ecall_app_task_struct->is_external_msd == TRUE)
    {
      /* Use the external MSD content directly */
      ecall_memscpy(ecall_app_task_struct->msd_data_buff, sizeof(ecall_app_task_struct->msd_data_buff),
                    ecall_app_task_struct->external_msd, sizeof(ecall_app_task_struct->external_msd));
      /* Print MSD */
      ECALL_MSG_0( ECALL_ERROR, "============================================================" );
      ECALL_MSG_1( ECALL_HIGH, "ecall_print_external_msd msd_len = %d",
                                ECALL_MSD_MAX_LENGTH );
      ECALL_MSG_0( ECALL_ERROR, "------------------------------------------------------------" );
      for (p = 0; p < ECALL_MSD_MAX_LENGTH; p += 8)
      {
        if ((p + 8) < ECALL_MSD_MAX_LENGTH)
        {
          MSG_8(MSG_SSID_ECALL, MSG_LEGACY_HIGH,
                "  %02x %02x %02x %02x   %02x %02x %02x %02x",
                ecall_app_task_struct->msd_data_buff[p],   ecall_app_task_struct->msd_data_buff[p+1], ecall_app_task_struct->msd_data_buff[p+2], ecall_app_task_struct->msd_data_buff[p+3], 
                ecall_app_task_struct->msd_data_buff[p+4], ecall_app_task_struct->msd_data_buff[p+5], ecall_app_task_struct->msd_data_buff[p+6], ecall_app_task_struct->msd_data_buff[p+7]);
        }
        else
        {
          /* MSD length is always 140, so we will have 4 bytes left over */
          MSG_4(MSG_SSID_ECALL, MSG_LEGACY_HIGH,
                "  %02x %02x %02x %02x",
                ecall_app_task_struct->msd_data_buff[p], ecall_app_task_struct->msd_data_buff[p+1], ecall_app_task_struct->msd_data_buff[p+2], ecall_app_task_struct->msd_data_buff[p+3]);
        }
      }
      ECALL_MSG_0( ECALL_ERROR, "============================================================" );
    }
    else
    {
      /* eCall App construct the real MSD from the GPS data */
      ecall_construct_msd( ecall_app_task_struct->msd_data_buff );
    }

    /* Send MSD to IVS modem and start transmission */
    ecall_start_transmit_data(ecall_app_task_struct->msd_data_buff, 
                              ecall_app_task_struct->tx_mode);
  }
  else
  {
    ECALL_MSG_1( ECALL_HIGH, "ECALL: start_received_is_with_msd %d, Do not call ecall_start_transmit_data",
                              ecall_app_task_struct->start_received_is_with_msd );
  }


  /* The next MSD TX will always be initiated/required by PSAP, 
     unless it is a MO call again, where we change the mode to PUSH
  */
  ECALL_MSG_0( ECALL_HIGH, "ECALL: Changing the Tx_mode to PULL_MODE, The next MSD TX will always be initiated by PSAP" );
  ecall_app_task_struct->tx_mode = PULL_MODE;

} /* ecall_enter_state_in_call_transmitting */

/*==========================================================================

  FUNCTION ECALL_ENTER_STATE_WAITING_FOR_AL_ACK

  DESCRIPTION
    This function will to the cleanup before exiting state ECALL_APP_IN_CALL_WAITING_FOR_GPS.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_enter_state_waiting_for_al_ack
(
  void
)
{
  ECALL_MSG_0( ECALL_MED, "ENTERED STATE waiting_f0r_al_ack " );

  /* Start Waiting for the AL ACK */
  (void)rex_set_timer( &ecall_app_task_struct->al_ack_timer, ECALL_AL_ACK_TIMEOUT );

} /* ECALL_ENTER_STATE_WAITING_FOR_AL_ACK */


/* END: New State tranision changes */

/*===========================================================================
FUNCTION   ecall_call_err_cb

DESCRIPTION
  This command is called after a call command is issued.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -esym(715,data_ptr) -esym(818,data_ptr) */
void ecall_call_err_cb(void *data_ptr, cm_call_cmd_e_type call_cmd,
                    cm_call_cmd_err_e_type call_cmd_err)
{
  if (call_cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    ECALL_MSG_2( ECALL_ERROR, "ecall_call_err_cb Call Command Callback returned %d for command %d",
                               call_cmd_err,
                               call_cmd );
  }
}
/*lint +esym(715,data_ptr) +esym(818,data_ptr) */

/*==========================================================================

  FUNCTION ECALL_APP_TASK_GET_STATE

  DESCRIPTION
    This function will return the state of the ecall App - waiting for GPS, in call, idle, etc.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

ecall_app_state_type ecall_app_task_get_state
(
  void
)
{
  return ecall_app_task_struct->ecall_app_state;

} /* ECALL_APP_TASK_GET_STATE */

/*==========================================================================

  FUNCTION ECALL_APP_TASK_GET_NV

  DESCRIPTION
    This function will return the value of NV item 65589 for ECALL 

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

boolean ecall_app_task_get_nv
(
  void
)
{
  return (ecall_app_task_struct->ecall_enabled_nv && ecall_efs_get_config());

} /* ECALL_APP_TASK_GET_NV */

/*===========================================================================

  FUNCTION ECALL_APP_TASK_PROCESS_ECALL_PROFILE_UPDATE

  DESCRIPTION
    This function will set the is_era_glonass_enabled flag, read the
    configuration files and propagate to ecall_app structure.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_task_process_ecall_profile_update
(
  ecall_profile_type_enum_type ecall_profile
)
{
  switch (ecall_profile)
  {
    case ECALL_PROFILE_TYPE_REGULAR:
      ECALL_MSG_0 ( ECALL_MED, "Regular Profile. Read the Pan-EU eCall configurations" );
      /* Disable the is_era_glonass_enabled flag */
      ecall_app_task_struct->is_era_glonass_enabled = FALSE;
      /* Read Pan-EU configurations from EFS */
      ecall_efs_read_ecall_config(&ecall_app_task_struct->ecall_efs_config);
      /* Propagate the Pan-EU configurations to ecall_app structure */
      ecall_app_get_efs_params(&ecall_app_task_struct->ecall_efs_config);
      break;

    case ECALL_PROFILE_TYPE_EMERGENCY:
      ECALL_MSG_0 ( ECALL_MED, "Emergency Profile. Read the ERA-GLONASS eCall configurations" );
      /* Enable the is_era_glonass_enabled flag */
      ecall_app_task_struct->is_era_glonass_enabled = TRUE;
      /* Read ERA-GLONASS configurations from EFS */
      ecall_efs_read_era_glonass_config(&ecall_app_task_struct->ecall_efs_config);
      /* Propagate the ERA-GLONASS configurations to ecall_app structure */
      ecall_app_get_efs_params(&ecall_app_task_struct->ecall_efs_config);
       break;

    default:
       ECALL_MSG_1 ( ECALL_ERROR, "Unsupported eCall Profile: %d",
                                  ecall_profile );
       break;
  }

} /* ECALL_APP_TASK_PROCESS_SIM_PROFILE_EVENT */

/*===========================================================================

  FUNCTION ECALL_APP_TASK_IS_ASID_VALID

  DESCRIPTION
    This function will check if the Modem as_id valid or not.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_app_task_is_asid_valid
(
  sys_modem_as_id_e_type asid
)
{
  if ( (SYS_MODEM_AS_ID_NONE < asid) &&
       ((int)asid < MAX_AS_IDS) )
  {
    return TRUE;
  }

  return FALSE;
} /* ecall_app_task_is_asid_valid() */

/*===========================================================================

  FUNCTION ECALL_APP_TASK_PROCESS_UPDATE_MSD_SETTING

  DESCRIPTION
    This function will process the updated MSD setting.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_task_process_update_msd_setting
(
  ecall_session_update_msd_setting_s_type msd_setting
)
{
  ECALL_MSG_1( ECALL_HIGH, "Update MSD Setting: is_external_msd = %d",
                            msd_setting.is_external_msd );

  if ( FALSE == msd_setting.is_external_msd )
  {
    /* Reset the eCall App Task Struct variable */
    ecall_app_task_struct->is_external_msd = FALSE;
  }
  else
  {
    ecall_app_task_struct->is_external_msd = TRUE;
    ecall_memscpy(ecall_app_task_struct->external_msd, sizeof(ecall_app_task_struct->external_msd),
                  msd_setting.ecall_msd, sizeof(msd_setting.ecall_msd));
  }

} /* ECALL_APP_TASK_PROCESS_UPDATE_MSD_SETTING */


#ifdef FEATURE_DUAL_SIM
/*==========================================================================

  FUNCTION ECALL_CM_SUBS_EVENT_CB

  DESCRIPTION
    This function will post a command to ecall_app_task to process
    the CM subs_info.

  DEPENDENCIES
    FEATURE_DUAL_SIM

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_cm_subs_event_cb(
  cm_ph_event_e_type         subs_event,
  const cm_subs_info_s_type *subs_info_ptr
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;

  cmd_ptr = ecall_app_task_get_cmd_buf();
  if (cmd_ptr == NULL)
  {
     ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_cm_subs_event_cb()" );
     return;
  }
  cmd_ptr->data.cm_subs_event_info.subs_event = subs_event;
  ecall_memscpy( &cmd_ptr->data.cm_subs_event_info.subs_info, sizeof(cmd_ptr->data.cm_subs_event_info.subs_info),
                 subs_info_ptr, sizeof(cm_subs_info_s_type) );
  cmd_ptr->cmd_id = ECALL_APP_CMD_CM_SUBS_EVENT_INFO;
  ecall_app_task_put_cmd(cmd_ptr);
}

/*===========================================================================

  FUNCTION ECALL_APP_TASK_PROCESS_CM_SUBS_EVENT_INFO

  DESCRIPTION
    This function will process the CM subs_info.

  DEPENDENCIES
    FEATURE_DUAL_SIM

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_task_process_cm_subs_event_info(
  ecall_app_task_cmd_type *cmd_ptr
)
{
  cm_ph_event_e_type      subs_event    =  cmd_ptr->data.cm_subs_event_info.subs_event;
  cm_subs_info_s_type    *subs_info_ptr =  &cmd_ptr->data.cm_subs_event_info.subs_info;
  sys_modem_as_id_e_type  pref_info_asid;

  if (CM_PH_EVENT_SUBSCRIPTION_PREF_INFO != subs_event)
  {
    ECALL_MSG_1( ECALL_HIGH, "ecall_app_task_process_cm_subs_event_info: event %d not interesting",
                              subs_event);
    return;
  }

  ECALL_MSG_1 ( ECALL_HIGH, "ecall_app_task_process_cm_subs_event_info: subs_event = %d",
                             subs_event);

  pref_info_asid = subs_info_ptr->subscription_id;

  if (!ecall_app_task_is_asid_valid(pref_info_asid))
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_app_task_process_cm_subs_event: invalid asid %d",
                               pref_info_asid);
    return;
  }

  // SUBSCRIPTION_PREF_INFO:
  // This event contains information that maps mmgsdi session types to the
  // appropriate modem stack.  This event is necessary to support multi-sim
  // and multi-stack configurations.  This event is preferred over the 
  // cm_ph_event's 'SUBSCRIPTION_AVAILABLE' event.
  // 
  // Until this event is received, the appInfo structures for the subscriptions
  // are not usable for much more than MMGSDI operations. This is because the
  // asid (see sys_modem_as_id_e_type) is not yet assigned/mapped to them. Once
  // this event has been processed, the asids are known.
  //
  // This event should always be received regardless of OTA service conditions.
  //
  // It is possible that the mapped subscription is located on a stack that is
  // currently not operational. Use the 'is_operational' field to determine this.
  //
  ECALL_MSG_6( ECALL_HIGH, "SUBS_PREF_INFO(asid %d): session (1X:0x%x, avail:%d) (GW:0x%x, avail:%d), operational: %d",
                            pref_info_asid,
                            subs_info_ptr->session_type_1x,
                            subs_info_ptr->is_available_1x,
                            subs_info_ptr->session_type_gw,
                            subs_info_ptr->is_available_gw,
                            subs_info_ptr->is_operational );

  {
    ecall_sim_cfg_s_type           *cfg_s_ptr   = ecall_sim_cfg_s_ptr();
    ecall_prov_app_info_s_type     *appInfo_ptr = NULL;
    mmgsdi_session_type_enum_type   cur_session_type;
    boolean                         cur_is_available;

    ECALL_MSG_6( ECALL_HIGH, "ECALL_PREF_INFO (asid %d): session (1X:0x%x, avail:%d) (GW:0x%x, avail:%d), operational: %d",
                              pref_info_asid,
                              cfg_s_ptr->ecall_cm_subs_info.sub[pref_info_asid].session_type_1x,
                              cfg_s_ptr->ecall_cm_subs_info.sub[pref_info_asid].is_available_1x,
                              cfg_s_ptr->ecall_cm_subs_info.sub[pref_info_asid].session_type_gw,
                              cfg_s_ptr->ecall_cm_subs_info.sub[pref_info_asid].is_available_gw,
                              cfg_s_ptr->ecall_cm_subs_info.sub[pref_info_asid].is_operational );

    // Handle GW subs information.
    // GW is possible on all AS (AS_ID_[1,2,3]).
    {
      // GW always requires an MMGSDI session to operate.
      // The 'session_type_gw' field will contain one of the following values:
      //    MMGSDI_GW_PROV_PRI_SESSION:   subs is available
      //    MMGSDI_GW_PROV_SEC_SESSION:   subs is available
      //    MMGSDI_GW_PROV_TER_SESSION:   subs is available
      //    MMGSDI_MAX_SESSION_TYPE:      subs is not available (unmapped)
      // 
      // The 'is_available_gw' field indicates the availability of the
      // stack mapped to 'session_type_gw'.
      //
      // It is impossible to directly transition from one GW session to another.
      // For example, GW_PRI -> GW_SEC would never occur.
      // 
      // It should also be impossible for 'session_type_gw' to transition to
      // MMGSDI_MAX_SESSION_TYPE without 'is_available_gw' also transitioning to
      // FALSE at the same time. If this state occurs, it is assumed that
      // is_available_gw is FALSE.
      //
      // It is possible to receive 'session_type_gw' != MMGSDI_MAX_SESSION_TYPE and
      // 'is_available_gw' == FALSE. This is an indication that the MMGSDI session
      // is active and that the session is mapped to an AS, but the stack handling
      // that subscription is not available. The stack could be disabled by ATCoP
      // for example or in power save.  If the stack has transitioned to not available,
      // eCall should indicate NOT_READY.
      //

      cur_session_type  = cfg_s_ptr->ecall_cm_subs_info.sub[pref_info_asid].session_type_gw;
      cur_is_available  = cfg_s_ptr->ecall_cm_subs_info.sub[pref_info_asid].is_available_gw;

      // If either session_type or available has changed, there is a transition
      if ( (cur_session_type != subs_info_ptr->session_type_gw) ||  // tagged 'S'
           (cur_is_available != subs_info_ptr->is_available_gw) )   // tagged 'A'
      {
        // Invalid A: GW_X and !avail
        // Invalid B: MAX  and avail
        // TAG OLDS NEWS OLDA NEWA RES
        //     GW   GW   0    0    Session was     mapped, stack was not available; NO CHANGE
        //  A  GW   GW   0    1    Session was     mapped, stack now     available; ECALL_READY
        //  A  GW   GW   1    0    Session was     mapped, stack now not available; ECALL_NOT_READY
        //     GW   GW   1    1    Session was     mapped, stack was     available; NO CHANGE
        //     MAX  MAX  0    0    Session was not mapped, stack was not available; NO CHANGE
        //  A  MAX  MAX  0    1    Session was not mapped, stack was not available; NO CHANGE            (INVALID B)
        //  A  MAX  MAX  1    0    Session was not mapped, stack was not available; NO CHANGE            (INVALID B)
        //     MAX  MAX  1    1    Session was not mapped, stack was not available; NO CHANGE            (INVALID B)
        // S   GW   MAX  0    0    Session now not mapped, stack was not available; UNMAP
        // SA  GW   MAX  0    1    Session now not mapped, stack was not available; UNMAP                (INVALID B)
        // SA  GW   MAX  1    0    Session now not mapped, stack now not available; UNMAP, ECALL_NOT_READY
        // S   GW   MAX  1    1    Session now not mapped, stack was     available; UNMAP, ECALL_NOT_READY (INVALID B)
        // S   MAX  GW   0    0    Session now     mapped, stack     not available; MAP
        // SA  MAX  GW   0    1    Session now     mapped, stack now     available; MAP, ECALL_READY
        // SA  MAX  GW   1    0    Session now     mapped, stack     not available; MAP                  (INVALID B)
        // S   MAX  GW   1    1    Session now     mapped, stack now     available; MAP, ECALL_READY       (INVALID B)

        // The current session_type and availability doesn't match the new,
        // this indicates a transition is happening

        if (MMGSDI_MAX_SESSION_TYPE_ENUM != subs_info_ptr->session_type_gw)
        {
          // GW-->GW or MAX-->GW (available TRUE or FALSE)
          // GW transition toward available, but maybe not actually available
          appInfo_ptr = ecall_sim_get_appinfo_by_session_type(subs_info_ptr->session_type_gw);

          if (NULL != appInfo_ptr)
          {
            ECALL_MSG_4( ECALL_HIGH, "GW -> mapped: session_type %d, session_activated %d, available %d->%d",
                                      appInfo_ptr->session_type,
                                      appInfo_ptr->session_activated,
                                      cur_is_available,
                                      subs_info_ptr->is_available_gw );

            // Handle mapping. A valid session_type is present, session is mapped to an AS.
            // Doesn't matter if mapping was already done, just map it
            appInfo_ptr->as_id         = pref_info_asid;

            // Handle available change
            if (cur_is_available != subs_info_ptr->is_available_gw)
            {
              // Available has changed, use new availability
              appInfo_ptr->is_available = subs_info_ptr->is_available_gw;

              // If the previous state was ready, send not ready
              if (cur_is_available)
              {
                // GW mapped, available -> GW mapped, not available
                ECALL_MSG_0( ECALL_HIGH, "GW mapped, available -> GW mapped, not available");
              }
              else
              {
                // GW mapped, not available -> GW mapped, available
                ECALL_MSG_0( ECALL_HIGH, "GW mapped, not available -> GW mapped, available");
              }
            } // Availability changed
          }
          else
          {
            ECALL_MSG_1( ECALL_ERROR, "GW -> available: could not get appInfo for session_type %d",
                                       subs_info_ptr->session_type_gw );
          }
        }
        else if (MMGSDI_MAX_SESSION_TYPE_ENUM != cur_session_type)
        {
          // Incoming session_type is MMGSDI_MAX_SESSION_TYPE and current is not.
          // GW-->MAX; New GW transition to UNMAPPED
          // All transitions of GW->MAX are handled here
          // The stack is unavailable AND there is no session mapped
          // NOTE: sim_init_complete is not reset here.  This will be handled
          //       when the associated session_changed event is processed.

          appInfo_ptr = ecall_sim_get_appinfo_by_session_type(cur_session_type);

          if (NULL != appInfo_ptr)
          {
            ECALL_MSG_2( ECALL_HIGH, "GW -> unavailable: session_type %d, session_activated %d",
                                      cur_session_type,
                                      appInfo_ptr->session_activated );

            // GW mapped, available
            //   or
            // GW mapped, not available   ---> GW unmapped, not available
            // Stack can't be available if the session is not mapped
            appInfo_ptr->is_available = FALSE;

          }
          else
          {
            ECALL_MSG_1( ECALL_ERROR, "GW -> unavailable: could not get appInfo for session_type %d", cur_session_type);
          }
        }
      }
    } // Handle GW

    // Update subs operational status (after handling the sessions in case previous state is needed).
    cfg_s_ptr->ecall_cm_subs_info.sub[pref_info_asid].session_type_1x = subs_info_ptr->session_type_1x;
    cfg_s_ptr->ecall_cm_subs_info.sub[pref_info_asid].is_available_1x = subs_info_ptr->is_available_1x;
    cfg_s_ptr->ecall_cm_subs_info.sub[pref_info_asid].session_type_gw = subs_info_ptr->session_type_gw;
    cfg_s_ptr->ecall_cm_subs_info.sub[pref_info_asid].is_available_gw = subs_info_ptr->is_available_gw;
    cfg_s_ptr->ecall_cm_subs_info.sub[pref_info_asid].is_operational  = subs_info_ptr->is_operational;
  }

  ecall_update_cm_subs_info();

  return;
} /* ECALL_APP_TASK_PROCESS_CM_SUBS_EVENT_INFO */

/*===========================================================================

  FUNCTION ECALL_APP_TASK_IS_SESSION_START_AS_ID_VALID

  DESCRIPTION
    This function will return the value of is_era_glonass_enabled.

  DEPENDENCIES
    FEATURE_DUAL_SIM

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_app_task_is_session_start_as_id_valid
(
  sys_modem_as_id_e_type as_id
)
{
  boolean                     ret = FALSE;
  mmgsdi_slot_id_enum_type    slot_id = (mmgsdi_slot_id_enum_type)0;
  ecall_prov_app_info_s_type *appInfo_ptr = NULL;

  if (FALSE == ecall_app_task_is_asid_valid(as_id))
  {
    ECALL_MSG_1( ECALL_ERROR, "is_session_start_as_id_valid(): Invalid input as_id %d",
                               as_id );
    goto Done;
  }

  slot_id = ecall_app_task_get_ecall_usim_slot_id();
  if ((mmgsdi_slot_id_enum_type)0 == slot_id )
  {
    ECALL_MSG_0( ECALL_ERROR, "is_session_start_as_id_valid(): Invalid slot_id" );
    goto Done;
  }

  appInfo_ptr = ecall_sim_get_appinfo_by_slot_id(slot_id);
  if (NULL == appInfo_ptr)
  {
    ECALL_MSG_1( ECALL_ERROR, "is_session_start_as_id_valid(): Unmapped slot_id %d",
                               slot_id );
    goto Done;
  }

  if (as_id == appInfo_ptr->as_id)
  {
    ECALL_MSG_2( ECALL_HIGH, "is_session_start_as_id_valid(): as_id %d matched with eCall as_id %d",
                              as_id,
                              appInfo_ptr->as_id );
    ret = TRUE;
  }

Done:
  return ret;
} /* ECALL_APP_TASK_IS_SESSION_START_AS_ID_VALID */

#endif /* FEATURE_DUAL_SIM */

/*===========================================================================

  FUNCTION ECALL_APP_TASK_GET_IS_ERA_GLONASS_ENABLED

  DESCRIPTION
    This function will return the value of is_era_glonass_enabled.

  DEPENDENCIES
    None

  RETURN VALUE
    boolean: 0 - ERA-GLONASS not enabled
             1 - ERA-GLONASS enabled

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_app_task_get_is_era_glonass_enabled
(
  void
)
{
  return ecall_app_task_struct->is_era_glonass_enabled;
} /* ECALL_APP_TASK_GET_IS_ERA_GLONASS_ENABLED */

/*===========================================================================

  FUNCTION ECALL_APP_TASK_GET_ECALL_USIM_SLOT_ID

  DESCRIPTION
    This function will return the value of ecall_usim_slot_id configured
    in EFS.

  DEPENDENCIES
    None

  RETURN VALUE
    mmgsdi_slot_id_enum_type

  SIDE EFFECTS
    None

===========================================================================*/
mmgsdi_slot_id_enum_type ecall_app_task_get_ecall_usim_slot_id
(
  void
)
{
  return ecall_app_task_struct->ecall_efs_config.ecall_usim_slot_id;
} /* ECALL_APP_TASK_GET_ECALL_USIM_SLOT_ID */

/*==========================================================================

  FUNCTION ECALL_APP_CALL_CMD_CB_FUNC

  DESCRIPTION
        This Callback Function is called when a call origination command
        is issued.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_call_cmd_cb_func
(
    void                      *data_block_ptr,
    cm_call_cmd_e_type        call_cmd,
    cm_call_cmd_err_e_type    call_cmd_err
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;

  if (call_cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    /* Call failed at origination, queue a command to eCall App Task to process */
    ECALL_MSG_2( ECALL_ERROR, "cm_mm_call_cmd_orig Callback returned error %d for cmd %d ",
                               call_cmd_err,
                               call_cmd );
    
    cmd_ptr = ecall_app_task_get_cmd_buf();
    if (NULL == cmd_ptr)
    {
      ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_call_event_cb()" );
      return;
    }

    /* Call failed at origination */
    cmd_ptr->data.end_call_config.is_call_fail_at_orig = TRUE;

    /* Call is not ended with normal clearing */
    cmd_ptr->data.end_call_config.is_mo_ecall_normal_clearing = FALSE;

    /* Call direction is MO call */
    cmd_ptr->data.end_call_config.call_direction = ECALL_MO_CALL;

    /* No end_status from CM */
    cmd_ptr->data.end_call_config.end_status = CM_CALL_END_NONE;

    /* Check if redial attempt already exceed maximum allowable number */
    if (ecall_app_task_struct->call_orig_fail_redial_index < ECALL_CALL_ORIG_FAIL_REDIAL_INDEX_MAX)
    {
      /* REDIAL */ 
      ECALL_MSG_0( ECALL_HIGH, "WILL REDIAL (Call Orig Failed)" );
      cmd_ptr->data.end_call_config.redial_pending = TRUE;
    }
    else
    {
      /* NOT REDIAL */
      ECALL_MSG_2( ECALL_HIGH, "WILL NOT REDIAL (Call Orig Failed). Call orig failure redial attempts (%d) exceed maximum allowable number %d",
                                ecall_app_task_struct->call_orig_fail_redial_index,
                                ECALL_CALL_ORIG_FAIL_REDIAL_INDEX_MAX );
      cmd_ptr->data.end_call_config.redial_pending = FALSE;
    }

    cmd_ptr->cmd_id = ECALL_APP_CMD_END_ECALL;
    ecall_app_task_put_cmd(cmd_ptr);
  }
} /* ECALL_APP_CALL_CMD_CB_FUNC */

/*==========================================================================

  FUNCTION ECALL_APP_CALL_CMD_EXTEN_CB_FUNC

  DESCRIPTION
        This Callback Function is called when a call origination command
        is issued.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_call_cmd_exten_cb_func
(
    void                          *data_block_ptr,
    cm_call_cmd_e_type            call_cmd,
    cm_call_cmd_err_e_type        call_cmd_err,
    cm_call_cmd_err_cause_e_type  call_cmd_err_cause,
    cm_alpha_s_type               alpha
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;

  if (call_cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    /* Call failed at origination, queue a command to eCall App Task to process */
    ECALL_MSG_3( ECALL_ERROR, "cm_mm_call_cmd_orig_cc_per_subs Callback returned error %d, err_cause %d for cmd %d",
                               call_cmd_err,
                               call_cmd_err_cause,
                               call_cmd );
    
    cmd_ptr = ecall_app_task_get_cmd_buf();
    if (NULL == cmd_ptr)
    {
      ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_call_event_cb()" );
      return;
    }

    /* Call failed at origination */
    cmd_ptr->data.end_call_config.is_call_fail_at_orig = TRUE;

    /* Call is not ended with normal clearing */
    cmd_ptr->data.end_call_config.is_mo_ecall_normal_clearing = FALSE;

    /* Call direction is MO call */
    cmd_ptr->data.end_call_config.call_direction = ECALL_MO_CALL;

    /* No end_status from CM */
    cmd_ptr->data.end_call_config.end_status = CM_CALL_END_NONE;

    /* Check if redial attempt already exceed maximum allowable number */
    if (ecall_app_task_struct->call_orig_fail_redial_index < ECALL_CALL_ORIG_FAIL_REDIAL_INDEX_MAX)
    {
      /* REDIAL */ 
      ECALL_MSG_0( ECALL_HIGH, "WILL REDIAL (Call Orig Failed)" );
      cmd_ptr->data.end_call_config.redial_pending = TRUE;
    }
    else
    {
      /* NOT REDIAL */
      ECALL_MSG_2( ECALL_HIGH, "WILL NOT REDIAL (Call Orig Failed). Call orig failure redial attempts (%d) exceed maximum allowable number %d",
                                ecall_app_task_struct->call_orig_fail_redial_index,
                                ECALL_CALL_ORIG_FAIL_REDIAL_INDEX_MAX );
      cmd_ptr->data.end_call_config.redial_pending = FALSE;
    }

    cmd_ptr->cmd_id = ECALL_APP_CMD_END_ECALL;
    ecall_app_task_put_cmd(cmd_ptr);
  }
} /* ECALL_APP_CALL_CMD_EXTEN_CB_FUNC */

/*==========================================================================

  FUNCTION ECALL_PROCESS_CMD_END_CALL

  DESCRIPTION
    This function will abort the tx and clear the timers

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_process_cmd_end_call
(
  void
)
{
  ECALL_MSG_0( ECALL_HIGH, "ecall_process_cmd_end_call" );

  /* De-register emodem client if registered first */
  ecall_app_task_deregister_modem_client_if_registered();

  if (ecall_app_task_struct->session_status == ECALL_FAILURE) /* CM_CALL_END_NO_SRV */
  {
     /* Report ECALL_FAILURE to registered start session callbacks */
     ECALL_MSG_0( ECALL_HIGH, "ecall_start_session_status_update" );
     ecall_app_task_struct->session_status = ECALL_UNKNOWN;
   #ifdef FEATURE_DUAL_SIM
     {
       ecall_prov_app_info_s_type        *appInfo_ptr = NULL;
       mmgsdi_slot_id_enum_type           slot_id = (mmgsdi_slot_id_enum_type)0;
       sys_modem_as_id_e_type             as_id = SYS_MODEM_AS_ID_1;

       slot_id = ecall_app_task_get_ecall_usim_slot_id();
       if ((mmgsdi_slot_id_enum_type)0 == slot_id)
       {
         ECALL_MSG_0( ECALL_ERROR, "start_session_status_update(): Invalid slot_id" );
         return;
       }

       appInfo_ptr = ecall_sim_get_appinfo_by_slot_id(slot_id);
       if (NULL == appInfo_ptr)
       {
         ECALL_MSG_1( ECALL_ERROR, "start_session_status_update(): Unmapped slot_id %d",
                                    slot_id );
         return;
       }

       if (!ecall_app_task_is_asid_valid(appInfo_ptr->as_id))
       {
         ECALL_MSG_1( ECALL_ERROR, "start_session_status_update(): invalid as_id %d",
                                    appInfo_ptr->as_id);
       }
       else
       {
         /* Valid eCall as_id obtained from appInfo_ptr */
         as_id = appInfo_ptr->as_id;
       }

       ecall_app_task_start_session_status_update(
                      ECALL_FAILURE,
                      (cm_call_id_type)-1,
                      (cm_client_id_type)-1,
                      as_id,
                      NULL );
     }
   #else
     /* Single SIM case */
     ecall_app_task_start_session_status_update(
                      ECALL_FAILURE,
                      (cm_call_id_type)-1,
                      (cm_client_id_type)-1,
                      SYS_MODEM_AS_ID_1,
                      NULL );
   #endif /* FEATURE_DUAL_SIM */
  }
  /* FULL Abort current transmission */
  ecall_abort_all();
  
  if(ecall_app_task_struct->redial_pending == TRUE)
  {
    /* eCall App need to redial. Check the cause of call failure first */
    if ( ecall_app_task_struct->is_call_fail_at_orig == TRUE )
    {
      /* Call failed at origination */
      ECALL_MSG_2( ECALL_HIGH, "Call Orig Failed, REDIAL in %d ms, attempt %d",
                                ecall_redial_duration[ecall_app_task_struct->call_orig_fail_redial_index],
                                ecall_app_task_struct->call_orig_fail_redial_index );
      (void) rex_set_timer( &ecall_app_task_struct->retry_timer, ecall_redial_duration[ecall_app_task_struct->call_orig_fail_redial_index]);
      ecall_app_task_struct->call_orig_fail_redial_index++;
    }
    else
    {
      /* Call dropped */
      ECALL_MSG_2( ECALL_HIGH, "Call Dropped, REDIAL in %d ms, attempt %d",
                                ecall_redial_duration[ecall_app_task_struct->call_dropped_redial_index],
                                ecall_app_task_struct->call_dropped_redial_index );
      (void) rex_set_timer( &ecall_app_task_struct->retry_timer, ecall_redial_duration[ecall_app_task_struct->call_dropped_redial_index]);
      ecall_app_task_struct->call_dropped_redial_index++;
    }
  }
  ecall_app_task_change_state(ECALL_APP_IDLE_ALLOW_MT_ECALL);

} /* ECALL_PROCESS_CMD_END_CALL */


/*==========================================================================

  FUNCTION ECALL_APP_TASK_CHANGE_STATE

  DESCRIPTION
    This function will set the state of the ecall App - waiting for GPS, in call, idle, etc.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void ecall_app_task_change_state
(
  ecall_app_state_type new_state
)
{
 ECALL_MSG_2( ECALL_HIGH, "ecall_app_task_change_state, OLD state = %d, NEW state = %d",
                           ecall_app_task_struct->ecall_app_state,
                           new_state );

  /*-------------------------------------------------------------------------
   Determine if the state transition is valid.
  -------------------------------------------------------------------------*/
  if ( ecall_is_valid_trans[ ecall_app_task_struct->ecall_app_state ][ new_state ] == FALSE )
  {
    ECALL_MSG_2( ECALL_HIGH, "Invalid ECALL state transition! ecall_is_valid_trans[ %d ][ %d ]",
                              ecall_app_task_struct->ecall_app_state,
                              new_state );
  }
  else /* Valid State Transition */
  {
    /*-----------------------------------------------------------------------
     Exit current state.
    -----------------------------------------------------------------------*/
    switch ( ecall_app_task_struct->ecall_app_state )
    {
      case ECALL_APP_IDLE_ALLOW_MT_ECALL:  
             ecall_exit_state_idle_allow_mt_ecall();
      break;

      case ECALL_APP_ECALL_INACTIVE:  
             ecall_exit_state_inactive();
      break;

      case ECALL_APP_IN_CALL:
             ecall_exit_state_in_call();
      break;

      case ECALL_APP_ORIGINATING_CALL:
             ecall_exit_state_originating_call();
      break;

      case ECALL_APP_IN_CALL_TRANSMITTING:
             ecall_exit_state_in_call_transmitting();
      break;

      case ECALL_APP_WAITING_FOR_AL_ACK:
            ecall_exit_state_waiting_for_al_ack();
      break;

      default:
        ECALL_MSG_0( ECALL_MED, "Bad Current State!" );
      break;


    } /* Exit current state */

    /*-----------------------------------------------------------------------
      Enter new state.
    -----------------------------------------------------------------------*/

    ecall_app_task_struct->ecall_app_state = new_state;

    switch ( new_state )
    {
      case ECALL_APP_IDLE_ALLOW_MT_ECALL:  
        ecall_enter_state_idle_allow_mt_ecall();
      break;

      case ECALL_APP_ECALL_INACTIVE:  
        ecall_enter_state_inactive();
      break;

      case ECALL_APP_IN_CALL:
        ecall_enter_state_in_call();
      break;

      case ECALL_APP_ORIGINATING_CALL:
        ecall_enter_state_originating_call();
      break;

      case ECALL_APP_IN_CALL_TRANSMITTING:
        ecall_enter_state_in_call_transmitting();
      break;

      case ECALL_APP_WAITING_FOR_AL_ACK:
        ecall_enter_state_waiting_for_al_ack();
      break;

      default:
        ECALL_MSG_0( ECALL_MED, "Bad NEW State!" );
      break;
    } /* Enter new state */

  } /* Valid State Transition */

  /* END: Adding state transition table */

} /* ecall_app_task_change_state */

/*==========================================================================

  FUNCTION ECALL_CALL_EVENT_CB

  DESCRIPTION
    Call back function sent to cm_mm_client_call_reg, this function handles all the CM Call events.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void ecall_call_event_cb
(
  cm_call_event_e_type           call_event,
  const cm_mm_call_info_s_type * call_info_ptr
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;
  ecall_app_state_type current_app_state = ECALL_APP_ECALL_INACTIVE;
  current_app_state = ecall_app_task_get_state();

  if ( (call_event == CM_CALL_EVENT_CONNECT) || (call_event == CM_CALL_EVENT_END) )
  {
    /* Queue up a cmd to eCall App task to update this call connect or end event */
    cmd_ptr = ecall_app_task_get_cmd_buf();
    if (NULL == cmd_ptr)
    {
      ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_call_event_cb()" );
      return;
    }
    
    cmd_ptr->data.call_status_data.call_event = call_event;
    cmd_ptr->data.call_status_data.call_id = call_info_ptr->call_id;
    cmd_ptr->data.call_status_data.call_type = call_info_ptr->call_type;
    cmd_ptr->cmd_id = ECALL_APP_CMD_UPDATE_CALL_STATUS;
    ecall_app_task_put_cmd(cmd_ptr);
  }


  /* To Handle the MT call */
  if ( (call_event == CM_CALL_EVENT_INCOM) &&
       (current_app_state == ECALL_APP_IDLE_ALLOW_MT_ECALL) &&
       (rex_get_timer(&ecall_app_task_struct->ecall_callback_timer) > 0) )
  {
    ECALL_MSG_0( ECALL_HIGH, "Received CM_CALL_EVENT_INCOM" );
        
    /* Post a command to the eCall App task to process this incoming call */
    cmd_ptr = ecall_app_task_get_cmd_buf();
    if (cmd_ptr == NULL)
    {
       ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_call_event_cb()" );
       return;
    }
    cmd_ptr->data.incom_call_config.call_id = call_info_ptr->call_id;
    cmd_ptr->data.incom_call_config.ecall_incom_call_type = call_info_ptr->call_type;
    cmd_ptr->cmd_id = ECALL_APP_CMD_INCOMING_ECALL;
    ecall_app_task_put_cmd(cmd_ptr);
  }
  else if ( (call_event == CM_CALL_EVENT_SETUP_IND) &&
            (current_app_state == ECALL_APP_IDLE_ALLOW_MT_ECALL) &&
            (rex_get_timer(&ecall_app_task_struct->ecall_callback_timer) > 0) &&
            ((call_info_ptr->call_type == CM_CALL_TYPE_VOICE) || (call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY)) )
  {
    ECALL_MSG_0( ECALL_MED, "Received CM_CALL_EVENT_SETUP_IND with call_type VOICE or EMERGENCY, eCall will setup the call" );
    cmd_ptr = ecall_app_task_get_cmd_buf();
    if (cmd_ptr == NULL)
    {
       ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_call_event_cb()" );
       return;
    }
    cmd_ptr->data.call_id = call_info_ptr->call_id;
    cmd_ptr->cmd_id = ECALL_APP_CMD_SETUP_CALL;
    ecall_app_task_put_cmd(cmd_ptr);
  }

  if ((current_app_state != ECALL_APP_ECALL_INACTIVE)   && 
      (call_info_ptr->call_id == ecall_app_task_struct->ecall_app_task_call_id) &&
      (call_info_ptr->call_type == CM_CALL_TYPE_VOICE   || 
       call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY))
  {    
    /*lint -e525*/
    /* Inform the user via QXDM. */
    switch (call_event)
    {
      case CM_CALL_EVENT_CONNECT:
        ECALL_MSG_0( ECALL_MED, "Received CM_CALL_EVENT_CONNECT" );

        if (call_info_ptr->direction == CM_CALL_DIRECTION_MO || call_info_ptr->direction == CM_CALL_DIRECTION_MT)
        {
          /* Post a command to the eCall App task to process call connected */
          cmd_ptr = ecall_app_task_get_cmd_buf();
          if (cmd_ptr == NULL)
          {
            ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_call_event_cb()" );
            return;
          }
  
          if(call_info_ptr->direction == CM_CALL_DIRECTION_MO) 
          {
            cmd_ptr->data.ecall_connected_config.call_direction = ECALL_MO_CALL;
            ECALL_MSG_3( ECALL_MED, "MO eCall connected with positive_ack_received %d, cleardown_received %d, is_session_timer_expired %d",
                                     ecall_app_task_struct->ecall_ivs_positive_ack_received,
                                     ecall_app_task_struct->ecall_ivs_cleardown_received,
                                     ecall_app_task_struct->is_session_timer_expired );

            /* If positive ACK has already been received or session_timer already expired
             * when MO eCall connected, the IVS shall not attempt to re-send the MSD unless 
             * requested by the PSAP. HLAP EN 16062 7.5.1. Configure IVS as PULL MODE. */
            if ((ecall_app_task_struct->ecall_ivs_positive_ack_received == TRUE) ||
                (ecall_app_task_struct->is_session_timer_expired == TRUE))
            {
              ECALL_MSG_0( ECALL_HIGH, "Positive ack already received or Session Timer expired, set tx_mode to PULL mode");
              cmd_ptr->data.ecall_connected_config.tx_mode = PULL_MODE;
            }
            else
            {
              ECALL_MSG_0( ECALL_HIGH, "No positive ack received yet and session timer not expired, set tx_mode to PUSH mode");
              cmd_ptr->data.ecall_connected_config.tx_mode = PUSH_MODE;
            }
          } /* CM_CALL_DIRECTION_MO*/
          else
          {
            cmd_ptr->data.ecall_connected_config.call_direction = ECALL_MT_CALL;
            cmd_ptr->data.ecall_connected_config.tx_mode = PULL_MODE;
          } /* CM_CALL_DIRECTION_MT */

          cmd_ptr->cmd_id = ECALL_APP_CMD_ECALL_CONNECTED;
          ecall_app_task_put_cmd(cmd_ptr);
        }
        break;

      case CM_CALL_EVENT_END:
        ECALL_MSG_1( ECALL_MED, "Received CM_CALL_EVENT_END, end_status = %d",
                                 call_info_ptr->end_status );
        MSG_4( MSG_SSID_ECALL, MSG_LEGACY_MED,
               "cc_cause.present = %d, cc_cause.cause_value = %d, cc_reject.present = %d, cc_reject.rej_value = %d", 
                call_info_ptr->mode_info.info.gw_cs_call.cc_cause.present,
                call_info_ptr->mode_info.info.gw_cs_call.cc_cause.cause_value,
                call_info_ptr->mode_info.info.gw_cs_call.cc_reject.present, 
                call_info_ptr->mode_info.info.gw_cs_call.cc_reject.rej_value);

        /* If the MT call ends with is_session_start_with_mt_ecall == TRUE, it means the
         * motorist triggers a new MO eCall session during MT call. Queue up a cmd to eCall
         * App task to process this MT call ends 
         */ 
        if ( (call_info_ptr->direction == CM_CALL_DIRECTION_MT) &&
             (ecall_app_task_struct->is_session_start_with_mt_ecall == TRUE) )
        {
          ECALL_MSG_0 ( ECALL_HIGH, "MT eCall ends" );
          cmd_ptr = ecall_app_task_get_cmd_buf();
          if (cmd_ptr == NULL)
          {
            ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_call_event_cb()" );
            return;
          }

          cmd_ptr->cmd_id = ECALL_APP_CMD_MT_ECALL_END_START_MO_ECALL;
          ecall_app_task_put_cmd(cmd_ptr);
          return;
        }

        cmd_ptr = ecall_app_task_get_cmd_buf();
        if (cmd_ptr == NULL)
        {
          ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_call_event_cb()" );
          return;
        }

        /* Initialize redial_pending, is_call_fail_at_orig, is_call_normal_clearing to FALSE */
        cmd_ptr->data.end_call_config.redial_pending = FALSE;
        cmd_ptr->data.end_call_config.is_call_fail_at_orig = FALSE;
        cmd_ptr->data.end_call_config.is_mo_ecall_normal_clearing = FALSE;
        

        /* If call orig failed or ACK/HLACK not recvd or CLEARDOWN is not recvd, then Redial */
        if ( (current_app_state == ECALL_APP_ORIGINATING_CALL) ||
             ((current_app_state == ECALL_APP_IN_CALL_TRANSMITTING) && (ecall_app_task_struct->ecall_ivs_cleardown_received == FALSE)) ||
             ((current_app_state == ECALL_APP_IN_CALL) &&
              !((call_info_ptr->mode_info.info.gw_cs_call.cc_cause.present == 1) &&
                (call_info_ptr->mode_info.info.gw_cs_call.cc_cause.cause_value == NORMAL_CALL_CLEARING) &&
                (call_info_ptr->mode_info.info.gw_cs_call.cc_reject.present == 0)))
            )  /* HLAP EN 16062 7.12.4 */
        {
          /* Redial only when MO eCall */
          if (call_info_ptr->direction == CM_CALL_DIRECTION_MO)
          {
            /* If call failes at origination stage, we should follow the redial procedure in 3GPP TS 122 001 Annex E. */
            if (current_app_state == ECALL_APP_ORIGINATING_CALL)
            {
              cmd_ptr->data.end_call_config.is_call_fail_at_orig = TRUE;

              if (ecall_app_task_struct->call_orig_fail_redial_index < ECALL_CALL_ORIG_FAIL_REDIAL_INDEX_MAX)
              {
                /* REDIAL */ 
                ECALL_MSG_0( ECALL_HIGH, "WILL REDIAL (Call Orig Failed)" );
                cmd_ptr->data.end_call_config.redial_pending = TRUE;
              }
              else
              {
                /* NOT REDIAL */
                ECALL_MSG_2( ECALL_HIGH, "WILL NOT REDIAL (Call Orig Failed). Call orig failure redial attempts (%d) exceed maximum allowable number %d",
                                          ecall_app_task_struct->call_orig_fail_redial_index,
                                          ECALL_CALL_ORIG_FAIL_REDIAL_INDEX_MAX );
                cmd_ptr->data.end_call_config.redial_pending = FALSE;
              }
            }
            else /* If ACK/HLACK not recvd or CLEARDOWN is not recvd, we should follow both 3GPP TS 122 001 Annex E
                    and CEN 16062 Clause 7.12.4 */
            {
              if(ecall_app_task_struct->call_dropped_redial_index < ECALL_CALL_DROPPED_REDIAL_INDEX_MAX)
              {
                /* REDIAL */ 
                ECALL_MSG_0( ECALL_HIGH, "WILL REDIAL (Call Dropped)" );
                cmd_ptr->data.end_call_config.redial_pending = TRUE;
              } 
              else 
              {
                /* NOT REDIAL */
                ECALL_MSG_2( ECALL_HIGH, "WILL NOT REDIAL (Call Dropped). Call dropped redial attempts (%d) exceed maximum allowable number %d",
                                          ecall_app_task_struct->call_dropped_redial_index,
                                          ECALL_CALL_DROPPED_REDIAL_INDEX_MAX );
                cmd_ptr->data.end_call_config.redial_pending = FALSE;
              } 
            }
          }
        }

        /* Check if the MO eCall is being cleardown normally */
        if ( (current_app_state == ECALL_APP_IN_CALL) &&
             (call_info_ptr->direction == CM_CALL_DIRECTION_MO) &&
             (call_info_ptr->mode_info.info.gw_cs_call.cc_cause.present == 1) &&
             (call_info_ptr->mode_info.info.gw_cs_call.cc_cause.cause_value == NORMAL_CALL_CLEARING) &&
             (call_info_ptr->mode_info.info.gw_cs_call.cc_reject.present == 0))
        {
          ECALL_MSG_0( ECALL_MED, "MO eCall ended with normal clearing from PSAP" );
          cmd_ptr->data.end_call_config.is_mo_ecall_normal_clearing = TRUE;
        }

        if (call_info_ptr->direction == CM_CALL_DIRECTION_MO) 
        {
          cmd_ptr->data.end_call_config.call_direction = ECALL_MO_CALL;
        }
        else if (call_info_ptr->direction == CM_CALL_DIRECTION_MT) 
        {
          cmd_ptr->data.end_call_config.call_direction = ECALL_MT_CALL;
        }
        
        cmd_ptr->data.end_call_config.end_status = call_info_ptr->end_status;

        cmd_ptr->cmd_id = ECALL_APP_CMD_END_ECALL;
        ecall_app_task_put_cmd(cmd_ptr);
        break;
     
       /* New event from CM to handle the early Handover - Need to TEST  */
      case CM_CALL_EVENT_HANDOVER_COMPLETE_IND:
        ECALL_MSG_0( ECALL_HIGH, "Received INTER-RAT CM_CALL_EVENT_HANDOVER_COMPLETE_IND" );
        break;
     
      default:
        ECALL_MSG_1( ECALL_MED, "Ecall_app_task Received default, call_event = %d",
                                 call_event );
        break;
    }/* Switch case */
  }
  /* MO eCall being manually ended, reset call_id */
  else if ( (current_app_state == ECALL_APP_ECALL_INACTIVE) &&
            (call_info_ptr->call_id == ecall_app_task_struct->ecall_app_task_call_id) &&
            (call_event == CM_CALL_EVENT_END) )
  {
    cmd_ptr = ecall_app_task_get_cmd_buf();
    if (cmd_ptr == NULL)
    {
       ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_call_event_cb()" );
       return;
    }
    cmd_ptr->data.call_id = (uint8)-1;
    cmd_ptr->cmd_id = ECALL_APP_CMD_UPDATE_ECALL_CALL_ID;
    ecall_app_task_put_cmd(cmd_ptr);
  }
  /* If not ECALL, then do nothing */
  else 
  {
    MSG_4( MSG_SSID_ECALL, MSG_LEGACY_MED,
           "Not eCall CM event. call type %d, Event %d, call id %d (eCall call id %d)",
            call_info_ptr->call_type,
            call_event,
            call_info_ptr->call_id,
            ecall_app_task_struct->ecall_app_task_call_id);
  }

} /* ecall_call_event_cb */

/*==========================================================================

  FUNCTION ECALL_APP_TASK_OP_MODE_CB_FUNC

  DESCRIPTION
    Call back function sent to ecall_operating_mode_reg to know about the ecall config state updates from ecall modem, 
	This cb function handles the ECALL_ONLY or ECALL_NORMAL states

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
/*lint -e818 -esym(715,user_data) Pointer parameter could be declared as pointing to const */
void ecall_app_task_op_mode_cb_func
(
  void             *user_data,
  ecall_operating_mode_type  operating_mode
)
{
  switch (operating_mode)
  {
    case ECALL_MODE_UNKNOWN:
      ECALL_MSG_0( ECALL_HIGH, "Rx OP MODE ECALL_MODE_UNKNOWN" );
      break;

     case ECALL_MODE_NORMAL:
      ECALL_MSG_0( ECALL_HIGH, "Rx OP MODE ECALL_MODE_NORMAL" );
      break;

      case ECALL_MODE_ECALL_ONLY:
      ECALL_MSG_0( ECALL_HIGH, "Rx OP MODE ECALL_MODE_ECALL_ONLY" );
      break;

    default:
      ECALL_MSG_0( ECALL_HIGH, "Rx OP MODE default" );
      break;

  } /* Switch */

} /* ecall_app_task_op_mode_cb_func*/


/*==========================================================================

  FUNCTION ECALL_APP_PROCESS_RETRY_TIMER_SIGNAL

  DESCRIPTION
    Once the retry timer expired, eCall App needs to check whether there is
    an E112 or a regular MO/MT going on.  If there is an E112 call already
    connected, we will not perform this redial attempt since E112 has the
    same priority as eCall; if it's a regular MO/MT call connected, eCall
    App will end this call and performe eCall redial with higher priority.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_process_retry_timer_signal
(
  void
)
{
  if (ecall_app_is_emergency_call_on_going() == TRUE)
  {
    /* There is an E112 call already in-progress, eCall App will not perform this
       redial attempt since E112 has the same priority as eCall */
    ECALL_MSG_0( ECALL_HIGH, "E112 is already in-progress, will abort this eCall redial attempt" );
  }
  else
  {
    /* Terminate any in-progress MO/MT call before eCall redial */
    ecall_app_dump_non_emergency_ps_call();

    /* Switch to ECALL_APP_ORIGINATING_CALL state to perform redial */
    ecall_app_task_change_state(ECALL_APP_ORIGINATING_CALL);
  }

} /* ECALL_APP_PROCESS_RETRY_TIMER_SIGNAL */


/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

#ifdef FEATURE_ECALL_HAS_MMOC
/*==========================================================================

  FUNCTION ECALL_APP_MSGR_INIT

  DESCRIPTION
    Initializes ECALL_APP message router.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 
===========================================================================*/
static void ecall_app_msgr_init(void)
{
  /* Message Router */
  errno_enum_type       msgr_error;
  byte                  count = 0;

  ECALL_MSG_0( ECALL_HIGH,  "ecall_app_msgr_init()" );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize MSGR Client and register for messages */
  (void)q_init(&ecall_app_msgr_free_q);
  (void)q_init(&ecall_app_msgr_q);

  /*( Initialize the free Q with cmd type structures*/
  for(count = 0; count < ECALL_APP_MSGR_Q_SIZE; count++)
  {
    q_put(&ecall_app_msgr_free_q, q_link(&ecall_app_msgr_bufs[count], &ecall_app_msgr_bufs[count].link));
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Create ecall_app_msgr_client*/
  msgr_error = msgr_client_create(&ecall_app_msgr_client);
  if (msgr_error != E_SUCCESS)
  {
    ECALL_MSG_1( ECALL_ERROR,  "ecall_app_msgr_create() faild with error = %d",
                                msgr_error );
    return;
  }

  msgr_error = msgr_client_add_rex_q(  &ecall_app_msgr_client,
                                       ecall_app_myself,
                                       ECALL_APP_MSGR_Q_SIG,
                                       &ecall_app_msgr_q,
                                       &ecall_app_msgr_free_q,
                                       (uint16)ECALL_APP_OFFSETOF( ecall_app_msgr_cmd_s_type, cmd ),    
                                       MSGR_NO_OFFSET,   
                                       sizeof(ecall_app_msgr_cmd_s_type),
                                       &ecall_app_msgr_rex_q_id        
                                       );

  if (msgr_error != E_SUCCESS)
  {
    ECALL_MSG_1( ECALL_ERROR, "msgr_client_add_rex_q() failed with error = %d",
                               msgr_error );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   
  /* Register with MSG ROUTER for OFFLINE notification */
  msgr_error = msgr_register(MSGR_ECALL_APP, &ecall_app_msgr_client, ecall_app_msgr_rex_q_id, MM_MMOC_OFFLINE_IND);
  if (msgr_error != E_SUCCESS)
  {
    ECALL_MSG_1( ECALL_ERROR, "msgr_register() failed with msgr error = %d",
                               msgr_error );
  }
   
  return;

} /* ECALL_APP_MSGR_INIT() */

/*==========================================================================

  FUNCTION ECALL_APP_MSGR_PROC

  DESCRIPTION
    This function will process the eCall App msgr notification.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 
===========================================================================*/
void ecall_app_msgr_proc(void)
{
  ecall_app_msgr_cmd_s_type *rpt_ptr = NULL;
  
  /* Get the command from the ecall_app_msgr_q and process it */
  while ((rpt_ptr = q_get(&ecall_app_msgr_q)) != NULL)
  {
    if (MM_MMOC_OFFLINE_IND == rpt_ptr->cmd.offline_ind.hdr.id)
    {
      ECALL_MSG_0( ECALL_HIGH, "Received OFFLINE notification" );
      /* Process task offline procedure.*/
    }
    else
    {
      ECALL_MSG_1( ECALL_ERROR, "Receive unexpected msg id %d",
                                 rpt_ptr->cmd.offline_ind.hdr.id );
    }
  
    //Return the buffer back to free Q
    q_put (&ecall_app_msgr_free_q, &(rpt_ptr->link));
  }  

} /* ECALL_APP_MSGR_PROC() */
#endif /* FEATURE_ECALL_HAS_MMOC */

/*==========================================================================

  FUNCTION ECALL_APP_CM_INIT

  DESCRIPTION
    This function will do CM related initialization.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 
===========================================================================*/
void ecall_app_cm_init
(
  void
)
{
  cm_client_status_e_type  status;

  /* Initialize the ecall_app call status table */
  ecall_app_call_status_init();

  status = cm_client_init( CM_CLIENT_TYPE_UI,
                           &ecall_app_task_struct->ecall_app_task_cm_client_id );

  if( CM_CLIENT_OK != status )
  {
    ECALL_MSG_1( ECALL_ERROR, "Failure in cm_client_init(): %d", status );
  }

  status = cm_mm_client_call_reg( ecall_app_task_struct->ecall_app_task_cm_client_id,
                                  ecall_call_event_cb,
                                  CM_CLIENT_EVENT_REG,
                                  CM_CALL_EVENT_ORIG, 
                                  CM_CALL_EVENT_EXIT_TC,
                                  NULL );

  if ( CM_CLIENT_OK != status ) 
  {
    ECALL_MSG_1( ECALL_ERROR, "Failure in cm_mm_client_call_reg(): %d", status );
  }

#ifdef FEATURE_DUAL_SIM
  // Register for subscription notification (CM_PH_EVENT_SUBSCRIPTION_PREF_INFO) events
  // for multi-sim configurations
  status = cm_client_subs_reg( ecall_app_task_struct->ecall_app_task_cm_client_id,
                               ecall_cm_subs_event_cb );

  if (CM_CLIENT_OK != status)
  {
    ECALL_MSG_1( ECALL_ERROR, "Failure in cm_client_subs_reg: %d", status);
  }
#endif

  status = cm_client_act(ecall_app_task_struct->ecall_app_task_cm_client_id);

  if( CM_CLIENT_OK != status )
  {
    ECALL_MSG_1( ECALL_MED, "Failure in cm_client_act(): %d", status );
  }

} /* ECALL_APP_CM_INIT */


/*==========================================================================

  FUNCTION ecall_app_task_init_before_task_start

  DESCRIPTION
    Initializes queues and defualt values for the global data.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    Initialization of internal data is done.
 
===========================================================================*/
void ecall_app_task_init_before_task_start
(
  void
)
{
  uint8 i = 0;
  ecall_app_task_cmd_type      *cmd_ptr = NULL;

  ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: ecall_app_task_init_before_task_start() entered" );

  /* Initialize command queues */
  (void) q_init(&ecall_app_task_cmd_q);
  (void) q_init(&ecall_app_task_cmd_free_q);

  for (i = 0; i < ECALL_APP_TASK_CMD_BUF_COUNT; i++)
  {
    (void) q_link(&ecall_app_task_cmd_buf[i], &ecall_app_task_cmd_buf[i].link);
    q_put(&ecall_app_task_cmd_free_q, &ecall_app_task_cmd_buf[i].link);
  }

  /* Ensure that all items are on the free queue (TODO: put this in a func) */
  while((cmd_ptr = (ecall_app_task_cmd_type *) q_get(&ecall_app_task_cmd_q)) != NULL)
  {
    q_put(&ecall_app_task_cmd_free_q, &cmd_ptr->link);
  }

  ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: ecall_app_task_init_before_task_start() returned" );
} /* ECALL_APP_TASK_INIT_BEFORE_TASK_START */


/*==========================================================================

  FUNCTION ECALL_APP_TASK_INIT

  DESCRIPTION
    Initializes cm, gps, and timers for ecall_app_task

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_task_init
(
  void
)
{
  /*lint --e{641}*/
  char ecall_nv_fpath[] = "/nv/item_files/modem/ecall/ecall_mode";
  struct fs_stat efs_buf;
  int32 efs_status =0;

  ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: ecall_app_task_init() entered" );

  #ifdef FEATURE_MODEM_RCINIT
    /* Inform RCInit that ecall_app initialization phase is complete */
    ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: Calling rcinit_handshake_startup()" );
    rcinit_handshake_startup();
  #else /* FEATURE_MODEM_RCINIT */
    /* If not using RCInit framework, initialize ecall_app queue and clients. */
    /* Otherwise, RCInit has already called this function in the init step. */
    ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: Calling ecall_app_task_init_before_task_start() from ecall_app_task" );
    ecall_app_task_init_before_task_start();

  #ifndef FEATURE_MODEM_RCINIT_PHASE2
    /* Inform Main Control task that ECALL_APP task has finished initialization */
    tmc_task_start();
  #endif /* FEATURE_MODEM_RCINIT_PHASE2 */
  #endif /* FEATURE_MODEM_RCINIT */

  #ifdef FEATURE_ECALL_HAS_DOG_HB
    /* Register with DOG HB new API */
    ecall_app_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) ECALL_APP_DOG_RPT_TIMER_SIG);
  #else /* Legacy Dog */
  #ifdef DOG_DYNAMIC_API
    ecall_app_dog_rpt_var      = dog_register( ecall_app_myself, \
                                               DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE);
    ecall_app_dog_rpt_time_var = dog_get_report_period( ecall_app_dog_rpt_var );

  /* If the return value from dog_get_report_period() was zero then we MUST
   ** NOT report to dog directly, as DOG will monitor us in a different manner.
   ** Hence we won't initialize or start any timer.
   */
    if ( ecall_app_dog_rpt_time_var > 0 ) 
    #endif /* ! DOG_DYNAMIC_API */
    {
      /* Initialize taskname watchdog report timer */
      rex_def_timer( &ecall_app_task_struct->ecall_app_rpt_timer, ecall_app_myself, ECALL_APP_DOG_RPT_TIMER_SIG );
      (void)rex_set_timer( &ecall_app_task_struct->ecall_app_rpt_timer, DOG_ECALL_APP_RPT_TIME );
    }
  #endif /* FEATURE_ECALL_HAS_DOG_HB */

  /*-------------------------------------------------------------------------
    Define the GPS Fix timeout timer
  -------------------------------------------------------------------------*/
  rex_def_timer( &ecall_app_task_struct->gps_timer, ecall_app_myself,
                 (rex_sigs_type) ECALL_APP_GPS_TIMER_SIG );
  /*-------------------------------------------------------------------------
     Start the timer when Rx START, timesout when Session Complete is not received in this time
  -------------------------------------------------------------------------*/
  rex_def_timer( &ecall_app_task_struct->session_timer, ecall_app_myself,
                 (rex_sigs_type) ECALL_APP_SESSION_TIMER_SIG );

  /*-------------------------------------------------------------------------
     Start the timer when ecall call is up, timesout when START is not received in this time
  -------------------------------------------------------------------------*/
  rex_def_timer( &ecall_app_task_struct->start_timer, ecall_app_myself,
                 (rex_sigs_type) ECALL_APP_START_TIMER_SIG );

  /*-------------------------------------------------------------------------
    Start the timer when enter IDLE_ALLOW_MT_ECALL state. eCall App will auto-answer any
    MT calls before this timer expires. The OEM can disable or cancel this timer if they
    don't want this auto-answer feature.
  -------------------------------------------------------------------------*/
  rex_def_timer( &ecall_app_task_struct->ecall_callback_timer, ecall_app_myself,
                 (rex_sigs_type) ECALL_APP_CALLBACK_TIMER_SIG );

  /* Start the timer when 2 LL ACKS are received, timesout in 2 secs */      
  rex_def_timer( &ecall_app_task_struct->al_ack_timer, ecall_app_myself,
                 (rex_sigs_type) ECALL_APP_AL_ACK_TIMER_SIG );

  /* Initialize timers */
  rex_def_timer( &ecall_app_task_struct->retry_timer, ecall_app_myself, ECALL_APP_RETRY_TIMER_SIG );
  rex_def_timer( &ecall_app_task_struct->call_cleardown_timer, ecall_app_myself, ECALL_APP_CALL_CLEARDOWN_TIMER_SIG );
  rex_def_timer( &ecall_app_task_struct->unmute_timer, ecall_app_myself, ECALL_APP_UNMUTE_TIMER_SIG );

  (void) rex_clr_sigs(rex_self(), (rex_sigs_type)~0);

  /* Initialize ecall_app global structure */
  ecall_app_init();

  ecall_app_cm_init();
  
  ecall_app_task_struct->ecall_app_task_discard_msd = FALSE; /* Sent to ecall_abort_transmit_data - for IVS to decide if MSD should be discarded */
  ecall_app_task_struct->ecall_enabled_nv = FALSE;
  
  /* efs_stat returns 0 if the file exists, -1 on error*/
  if(efs_stat (ecall_nv_fpath, &efs_buf)  == 0)
  {
    /* if file exists, read if the enable_ecall is TRUE or FALSE */
    efs_status = efs_get(ecall_nv_fpath, 
                         &ecall_app_task_struct->ecall_enabled_nv, 
                         sizeof(ecall_app_task_struct->ecall_enabled_nv));

    ECALL_MSG_2( ECALL_HIGH, "ECALL_NV_READ returned efs_status = %d, NV ecall_enable = %d",
                              efs_status,
                              ecall_app_task_struct->ecall_enabled_nv );
  }

  /* Read ecall_efs_config from EFS */
  ecall_efs_read_ecall_config(&ecall_app_task_struct->ecall_efs_config);

  /* Propagate eCall EFS Config to eCall App */
  ecall_app_get_efs_params(&ecall_app_task_struct->ecall_efs_config);

  #ifdef FEATURE_ECALL_HAS_MMOC
  ecall_app_msgr_init();
  #endif /* FEATURE_ECALL_HAS_MMOC */

  /* Initialize the ecall_app for pbm notificaitons */
  ecall_app_pbm_init();

  /* Register a cb function to get indication about the ecall modem config state changes */
  ecall_operating_mode_reg(ecall_app_task_op_mode_cb_func, NULL);
  
  /* Initialize ecall_ivs_cleardown_received to FALSE, HLAP EN 16062 7.12.4 */
  ecall_app_task_struct->ecall_ivs_cleardown_received = FALSE;

  /* Initialize ecall_ivs_positive_ack_received and is_session_timer_expired to FALSE */
  ecall_app_task_struct->ecall_ivs_positive_ack_received = FALSE;
  ecall_app_task_struct->is_session_timer_expired = FALSE;

  /* Initialize is_call_fail_at_orig to FALSE */
  ecall_app_task_struct->is_call_fail_at_orig = FALSE;

  /* Initialize redial_attempt to FALSE, HLAP EN 16062 7.12.4 */
  ecall_app_task_struct->redial_pending = FALSE;

  /* Initialize redial_index to 0 */
  ecall_app_task_struct->call_orig_fail_redial_index = 0;
  ecall_app_task_struct->call_dropped_redial_index = 0;

  /* Initialize hlack_received to FALSE */
  ecall_app_task_struct->hlack_received = FALSE;

  /* Initialize pending_mute and unmute to FALSE */
  ecall_app_task_struct->pending_mute   = FALSE;
  ecall_app_task_struct->pending_unmute = FALSE;

  /* Initialize call_orig_info callbacks to NULL */
  ecall_app_task_struct->start_session_cb_func = NULL;
  ecall_app_task_struct->call_orig_info_cb_func = NULL;
  ecall_app_task_struct->call_orig_exten_info_cb_func = NULL;
  ecall_app_task_struct->data_block_ptr = NULL;

  /* Initialize ecall_app_task_call_id to -1 */
  ecall_app_task_struct->ecall_app_task_call_id = (uint8)-1;

  /* Initialize gps_engine_initialized and warmup to FALSE */
  ecall_app_task_struct->gps_engine_initialized = FALSE;
  ecall_app_task_struct->gps_engine_warmup = FALSE;

  /* Initialize start_received_is_with_msd to FALSE */
  ecall_app_task_struct->start_received_is_with_msd = FALSE;

  /* Initialize is_session_start_with_mt_ecall to FALSE */
  ecall_app_task_struct->is_session_start_with_mt_ecall = FALSE;

  /* Initialize is_era_glonass_enabled to FALSE */
  ecall_app_task_struct->is_era_glonass_enabled = FALSE;

  ecall_app_task_struct->is_external_msd = FALSE;
  memset(ecall_app_task_struct->external_msd, 0, sizeof(ecall_app_task_struct->external_msd));

  /* Initialize emodem_client_reg to FALSE */
  ecall_app_task_struct->emodem_client_reg = FALSE;

  /* Initialize GPS only if eCall NV 65589 is enabled */  
  if (ecall_app_task_struct->ecall_enabled_nv == TRUE)
  {
    #ifdef FEATURE_ECALL_HAS_QMI_LOC
    /* Setup a short GPS timer to init QMI LOC
     */
    (void)rex_set_timer( &ecall_app_task_struct->gps_timer, ecall_app_task_struct->ecall_efs_config.gnss_update_time_ms );
    if (!ecall_gps_init())
    {
      /* eCall App will set gps_engine_initialized to TRUE when the QMI LOC signal arrives,
       * and then call ecall_getposition().
       */
      ECALL_MSG_1( ECALL_ERROR, "FAILED to initialize GPS. Will try after %d ms",
                                 ecall_app_task_struct->ecall_efs_config.gnss_update_time_ms );
      ecall_app_task_struct->gps_engine_initialized = FALSE;
    }
    #elif ( (defined FEATURE_CGPS) && !(defined FEATURE_ECALL_HAS_QMI_LOC) ) /* PDAPI */
    /* Setup a short GPS timer to init PDAPI
     */
    (void)rex_set_timer( &ecall_app_task_struct->gps_timer, ecall_app_task_struct->ecall_efs_config.gnss_update_time_ms );
    if (!ecall_gps_init())
    {
      ECALL_MSG_1( ECALL_ERROR, "FAILED to initialize GPS. Will try after %d ms",
                                 ecall_app_task_struct->ecall_efs_config.gnss_update_time_ms );
      ecall_app_task_struct->gps_engine_initialized = FALSE;
    }
    else
    {
      ecall_app_task_struct->gps_engine_initialized = TRUE;
      ecall_getposition(ecall_app_task_struct->ecall_efs_config.gnss_update_time_ms);
    }
    #endif /* FEATURE_ECALL_HAS_QMI_LOC */
  }

  ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: ecall_app_task_init() returning" );

} /* ecall_app_task_init() */

/*===========================================================================

FUNCTION ECALL_APP_WAIT

DESCRIPTION
  This function suspends execution of the ECALL_APP_WAIT task until one (or
  more) signals from the requested signal mask is set.

  Watchdog kicking is performed in this function.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask returned by rex_wait().

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type  ecall_app_wait
(
  rex_sigs_type  requested_signal_mask       /* Signal mask to suspend on */
)
{
  /*lint --e{641}*/
  rex_sigs_type  set_signals;     /* Signal mask upon return from rex wait */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Wait for a signal to be set.
  -------------------------------------------------------------------------*/
  do{
    set_signals = rex_wait( requested_signal_mask | ECALL_APP_DOG_RPT_TIMER_SIG );

    if ( (set_signals & ECALL_APP_DOG_RPT_TIMER_SIG) != 0 )
    {
      #ifdef FEATURE_ECALL_HAS_DOG_HB
      /*---------------------------------------------------------------------
      The watchdog needs a kick. Clear the signal and report back 
      to the watchdog by Dog HB.
      ---------------------------------------------------------------------*/
      (void)rex_clr_sigs( ecall_app_myself, ECALL_APP_DOG_RPT_TIMER_SIG );
      /* Report to the Dog HB */
      dog_hb_report(DOG_ECALL_APP_RPT);

      #else /* Legacy Dog*/
      /*---------------------------------------------------------------------
      The watchdog needs a kick. Report back to the watchdog, and then
      restart the timer.
      ---------------------------------------------------------------------*/
      #ifdef DOG_DYNAMIC_API
      if ( ecall_app_dog_rpt_time_var > 0 )  
      #endif
      {
        (void)rex_clr_sigs( ecall_app_myself, ECALL_APP_DOG_RPT_TIMER_SIG );
        dog_report( DOG_ECALL_APP_RPT );
        (void)rex_set_timer( &ecall_app_task_struct->ecall_app_rpt_timer,
                              DOG_ECALL_APP_RPT_TIME );
      }
      #endif /* FEATURE_ECALL_HAS_DOG_HB */

      /*---------------------------------------------------------------------
      Processed the DOG_RPT_TIMER_SIG so clear it off in set_signals.
      ---------------------------------------------------------------------*/
      set_signals &= (~ECALL_APP_DOG_RPT_TIMER_SIG);
    }
  } while ( (set_signals & requested_signal_mask) == 0 );

  return( set_signals );

} /* ecall_app_wait() */


/*==========================================================================

  FUNCTION ecall_app_task

  DESCRIPTION
    Starts up the eCall task and waits infinitely for ECALL signals 

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
/*lint -esym(715,ignored)*/
void ecall_app_task
(
  dword ignored
)
{
  /*lint --e{641}*/
  rex_sigs_type   set_sig;
  rex_sigs_type       requested_sigs;   /* Signals to wait for  */
  ecall_app_state_type current_app_state = ECALL_APP_ECALL_INACTIVE;

  ecall_app_myself = rex_self();        /* Will never change after this */

  /*-------------------------------------------------------------------------
   Do task initialization. The init function performs all the task-level
   initialization, e.g. define message queues.
 -------------------------------------------------------------------------*/

  /* Initialize the app task*/
  ecall_app_task_init();

  /* Start the queue processing */
  (void) rex_set_sigs( ecall_app_myself, ECALL_APP_CMD_Q_SIG );

  ECALL_MSG_0( ECALL_MED, "Entered ecall_app_task()" );

  /*lint -e655*/
  requested_sigs =
    ( ECALL_APP_CMD_Q_SIG )         |
    ( ECALL_APP_GPS_TIMER_SIG )     |
    ( ECALL_APP_TASK_STOP_SIG )     |
    ( ECALL_APP_TASK_OFFLINE_SIG )  |
    ( ECALL_APP_START_TIMER_SIG )   |
    ( ECALL_APP_SESSION_TIMER_SIG ) |
    ( ECALL_APP_CALLBACK_TIMER_SIG )|
    ( ECALL_APP_AL_ACK_TIMER_SIG )  |
    ( ECALL_APP_RETRY_TIMER_SIG )   |
    ( ECALL_APP_UNMUTE_TIMER_SIG )  |
    ( ECALL_APP_NOTIFY_LOC_SVC_SIG )|
    ( ECALL_APP_MSGR_Q_SIG )        |
    ( ECALL_APP_CALL_CLEARDOWN_TIMER_SIG );
  /*lint +e655*/

  for ( ; ; )
  {
    /* Wait in the infinite loop so the task would not die */
    set_sig = ecall_app_wait(requested_sigs);

    /*lint -e525 -e539*/
    if ((set_sig & ECALL_APP_CMD_Q_SIG )!= 0 ) 
    {
      /*-------------------------------------------------------------------------
                       Clear the Signal
      -------------------------------------------------------------------------*/
      (void) rex_clr_sigs( ecall_app_myself, ECALL_APP_CMD_Q_SIG );
      ecall_app_task_process_cmd();
    }  /* ECALL_APP_CMD_Q_SIG*/

    if ((set_sig & ECALL_APP_CALLBACK_TIMER_SIG)!= 0 ) 
    {
      /*-------------------------------------------------------------------------
                        Clear the Signal
      -------------------------------------------------------------------------*/
      (void) rex_clr_sigs( ecall_app_myself, ECALL_APP_CALLBACK_TIMER_SIG );
      ECALL_MSG_0( ECALL_HIGH, "ECALL_APP_CALLBACK_TIMER_SIG EXPIRED" );
    } /* ECALL_APP_CALLBACK_TIMER_SIG */

    if ((set_sig &  ECALL_APP_CALL_CLEARDOWN_TIMER_SIG)!= 0 ) 
    {
      /*-------------------------------------------------------------------------
                        Clear the Signal
      -------------------------------------------------------------------------*/
      (void) rex_clr_sigs( ecall_app_myself, ECALL_APP_CALL_CLEARDOWN_TIMER_SIG );
      ECALL_MSG_0( ECALL_HIGH, "ECALL_APP_CALL_CLEARDOWN_TIMER_SIG EXPIRED" );
      current_app_state = ecall_app_task_get_state();
      if (current_app_state == ECALL_APP_IN_CALL) /* ECALL call is active */
      {
        ECALL_MSG_0( ECALL_HIGH, "Clearing down the call" );
        if (ecall_end_call() == FALSE)
        {
          ECALL_MSG_0( ECALL_MED, "ERROR Clearing down the call" );
        }
      }
    } /* ECALL_APP_CALLBACK_TIMER_SIG */

    if ((set_sig & ECALL_APP_GPS_TIMER_SIG )!= 0 ) 
    {
      /*-------------------------------------------------------------------------
                        Clear the Signal
      -------------------------------------------------------------------------*/
      (void) rex_clr_sigs( ecall_app_myself, ECALL_APP_GPS_TIMER_SIG );
      ECALL_MSG_0( ECALL_MED, "ECALL_APP_GPS_TIMER_SIG EXPIRED" );
      ecall_app_process_gps_timer_signal();
    } /* ECALL_APP_GPS_TIMER_SIG*/

    if( (set_sig & ( ECALL_APP_TASK_STOP_SIG )) != 0 )
    {
      (void) rex_clr_sigs( ecall_app_myself, ECALL_APP_TASK_STOP_SIG );
    #ifndef FEATURE_MODEM_RCINIT_PHASE2
      /* Process task stop procedure */
      tmc_task_stop();
    #endif /* FEATURE_MODEM_RCINIT_PHASE2 */
    } /* ECALL_APP_TASK_STOP_SIG */

    if( (set_sig & ( ECALL_APP_RETRY_TIMER_SIG)) != 0 )
    {
      ECALL_MSG_0( ECALL_HIGH, "ECALL_APP_RETRY_TIMER EXPIRE" );
      (void) rex_clr_sigs( ecall_app_myself, ECALL_APP_RETRY_TIMER_SIG );
      ecall_app_process_retry_timer_signal();
    } /* ECALL_APP_RETRY_TIMER_SIG */


    if( (set_sig & ( ECALL_APP_TASK_OFFLINE_SIG )) != 0 )
    {
      (void) rex_clr_sigs( ecall_app_myself, ECALL_APP_TASK_OFFLINE_SIG );
    #ifndef FEATURE_MODEM_RCINIT_PHASE2
      /* Send ack to TMC/MC that ECALL_APP is offline */
      tmc_task_offline();
    #endif
    } /* ECALL_APP_TASK_OFFLINE_SIG */

    if ( ( set_sig & (ECALL_APP_START_TIMER_SIG ) ) != 0)
    {
      /*-------------------------------------------------------------------------
                        Clear the Signal
      -------------------------------------------------------------------------*/
      (void) rex_clr_sigs( ecall_app_myself, ECALL_APP_START_TIMER_SIG );
      ECALL_MSG_0( ECALL_HIGH, "ECALL_APP_START_TIMER EXPIRE" );
      /* Session did not complete in time, so abort the Tx*/
      ecall_app_task_struct->ecall_app_task_discard_msd = FALSE; /* Do not discard this MSD */
      ecall_app_task_change_state(ECALL_APP_IN_CALL); /* Transmission never started, go to regular voice call */
    } /* ECALL_APP_START_TIMER_SIG */

    if ( ( set_sig & (ECALL_APP_SESSION_TIMER_SIG) ) != 0 )
    {
      /*-------------------------------------------------------------------------
                        Clear the Signal
      -------------------------------------------------------------------------*/
      (void) rex_clr_sigs ( ecall_app_myself, ECALL_APP_SESSION_TIMER_SIG );
      ECALL_MSG_0( ECALL_HIGH, "ECALL_APP_SESSION_TIMER EXPIRE" );
      /* Session did not complete in time, so abort the Tx*/
      report_ecall_event(EVENT_ECALL_SESSION_FAILURE);
      ecall_app_task_struct->ecall_app_task_discard_msd = FALSE; /* Do not discard this MSD */
      ecall_app_task_struct->is_session_timer_expired = TRUE;
      ecall_app_task_change_state(ECALL_APP_IN_CALL); /* Transmission never started, go to regular voice call */
    } /* ECALL_APP_SESSION_TIMER_SIG */

    if ((set_sig & ECALL_APP_AL_ACK_TIMER_SIG )!= 0 ) 
    {
      /*-------------------------------------------------------------------------
                        Clear the Signal
      -------------------------------------------------------------------------*/
      (void) rex_clr_sigs( ecall_app_myself, ECALL_APP_AL_ACK_TIMER_SIG );
      ECALL_MSG_0( ECALL_HIGH, "ECALL_APP_AL_ACK_TIMER_SIG EXPIRED" );
      /* Deregister TX */       
      ecall_app_task_change_state(ECALL_APP_IN_CALL); /* Transmission is cancelled, go to regular voice call */
    } /* ECALL_APP_AL_ACK_TIMER_SIG */

    if ((set_sig & ECALL_APP_UNMUTE_TIMER_SIG)!= 0 )
    {
      /*-------------------------------------------------------------------------
                        Clear the Signal
      -------------------------------------------------------------------------*/
      (void) rex_clr_sigs( ecall_app_myself, ECALL_APP_UNMUTE_TIMER_SIG );
      ECALL_MSG_0( ECALL_HIGH, "ECALL_APP_UNMUTE_TIMER_SIG EXPIRED" );
      /* Unmute Vocoder RX output to Speaker to resume voice going into speaker */
      if(ecall_app_task_struct->ecall_efs_config.voc_config == TRUE) 
      {
        ecall_app_unmute_rx_out();
      }
    } /* ECALL_APP_UNMUTE_TIMER_SIG */

    if ((set_sig & ECALL_APP_NOTIFY_LOC_SVC_SIG)!=0 )
    {
      /*-------------------------------------------------------------------------
                        Clear the Signal
      -------------------------------------------------------------------------*/
      (void) rex_clr_sigs( ecall_app_myself, ECALL_APP_NOTIFY_LOC_SVC_SIG );
      #ifdef FEATURE_ECALL_HAS_QMI_LOC
      ECALL_MSG_0( ECALL_HIGH, "ECALL_APP receives ECALL_APP_NOTIFY_LOC_SVC_SIG" );
      ecall_app_task_struct->gps_engine_initialized = TRUE;
      ecall_qmi_loc_client_process_signal();
      /* Start the QMI_LOC Client Session with cold start
       */
      (void) rex_set_timer( &ecall_app_task_struct->gps_timer, ECALL_GPS_FIX_TIMEOUT );
      ecall_getposition(ECALL_GPS_FIX_TIMEOUT);
      #else
      ECALL_MSG_0( ECALL_ERROR, "ECALL_APP receives ECALL_APP_NOTIFY_LOC_SVC_SIG but FEATURE_ECALL_HAS_QMI_LOC is not defined" );
      #endif /* FEATURE_ECALL_HAS_QMI_LOC */
    } /* ECALL_APP_NOTIFY_LOC_SVC_SIG */

    if ((set_sig & ECALL_APP_MSGR_Q_SIG)!=0 )
    {
      /*-------------------------------------------------------------------------
                        Clear the Signal
      -------------------------------------------------------------------------*/
      (void) rex_clr_sigs( ecall_app_myself, ECALL_APP_MSGR_Q_SIG );
      #ifdef FEATURE_ECALL_HAS_MMOC
      ECALL_MSG_0( ECALL_HIGH, "ECALL_APP receives ECALL_APP_MSGR_Q_SIG" );
      ecall_app_msgr_proc();
      #else
      ECALL_MSG_0( ECALL_ERROR, "ECALL_APP receives ECALL_APP_MSGR_Q_SIG but FEATURE_ECALL_HAS_MMOC is not defined" );
      #endif /* FEATURE_ECALL_HAS_MMOC */
    } /* ECALL_APP_MSGR_Q_SIG */

  } /* for ( ; ; ) */
  /*lint +e525 +e539*/
  
 
}

/*==========================================================================

  FUNCTION ECALL_APP_TASK_PROCESS_TX_STATUS

  DESCRIPTION
    Call back function sent to START_TRANSMIT_DATA, this function handles all the MSD transmission states

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
/*lint -e818 -esym(715,user_data) Pointer parameter could be declared as pointing to const */
void ecall_app_task_process_tx_status
(
  void             *user_data,
  ecall_cb_data_type ivs_status
)
{
  ecall_app_state_type current_app_state = ECALL_APP_ECALL_INACTIVE;
  current_app_state = ecall_app_task_get_state();

  switch (ivs_status.tx_status)
  {

    case ECALL_IVS_SF_RECEIVED:
      ECALL_MSG_0( ECALL_HIGH, "IVS Rx ECALL_IVS_SF_RECEIVED" );
      break; 
    
    case ECALL_IVS_START_RECEIVED_NO_MSD:
      ECALL_MSG_0( ECALL_HIGH, "IVS Rx ECALL_IVS_START_RECEIVED_NO_MSD" );
      ecall_app_task_struct->tx_mode = PULL_MODE;
      ecall_app_task_struct->hlack_received = FALSE;
      ecall_app_task_struct->start_received_is_with_msd = FALSE;
      ecall_app_task_struct->ecall_ivs_positive_ack_received = FALSE;
      ecall_app_task_struct->is_session_timer_expired = FALSE;
      (void)rex_clr_timer(&ecall_app_task_struct->unmute_timer);
      /* 3 STARTS received */
      ecall_app_task_change_state(ECALL_APP_IN_CALL_TRANSMITTING);
      break; 
    
    case ECALL_IVS_START_RECEIVED_MSD:
      ECALL_MSG_0( ECALL_HIGH, "IVS Rx ECALL_IVS_START_RECEIVED_MSD" );
      ecall_app_task_struct->hlack_received = FALSE;
      ecall_app_task_struct->start_received_is_with_msd = TRUE;
      ecall_app_task_struct->ecall_ivs_positive_ack_received = FALSE;
      ecall_app_task_struct->is_session_timer_expired = FALSE;
      (void)rex_clr_timer(&ecall_app_task_struct->unmute_timer);
      /* Mute the path from Vocoder RX output to Speaker input for both MO 
         and MT call, so the voice does not disturb the transmission (sync
         msg tone does not go into the mic) */
      if ( ecall_app_task_struct->ecall_efs_config.voc_config == TRUE )
      {
        ecall_app_mute_rx_out();
      }
       /* 3 STARTS received, clear the Start timer */
      (void)rex_clr_timer(&ecall_app_task_struct->start_timer);
      if (current_app_state != ECALL_APP_IN_CALL_TRANSMITTING) /* If not already in Call Tx */
      {
        ecall_app_task_change_state(ECALL_APP_IN_CALL_TRANSMITTING);
      }
      break; 
    
    case ECALL_IVS_MSD_SENT:
      ECALL_MSG_0( ECALL_HIGH, "IVS Rx ECALL_IVS_MSD_SENT" );
      break; 
    
    case ECALL_SESSION_COMPLETED: /* 2 AL ACKS received */
      ECALL_MSG_1( ECALL_HIGH, "IVS Rx ECALL_SESSION_COMPLETED (2 AL ACKS received) hlackdata %d",
                                ivs_status.hlack_data );
      ecall_app_task_struct->ecall_ivs_cleardown_received = FALSE;
      ecall_app_task_struct->ecall_ivs_positive_ack_received = TRUE;
      ecall_app_task_change_state(ECALL_APP_IN_CALL);  /* Ecall is still ON */
      break; 
    
    case ECALL_SESSION_RESET:
      ECALL_MSG_0( ECALL_HIGH, "IVS Rx ECALL_SESSION_RESET " );
      ecall_app_task_struct->ecall_app_task_discard_msd = FALSE; /* Do not Discard this MSD */
      if (current_app_state != ECALL_APP_IN_CALL) /* If not already in Call */
      {
        ecall_app_task_change_state(ECALL_APP_IN_CALL);
      }
      break; 
    
    case ECALL_IVS_MSD_FAST_MODE:
      ECALL_MSG_0( ECALL_HIGH, "IVS Rx ECALL_IVS_MSD_FAST_MODE" );
      report_ecall_event(EVENT_ECALL_SESSION_START);
      ECALL_MSG_1( ECALL_MED, "Set session_timer to %d ms",
                               ecall_app_task_struct->ecall_efs_config.test_only_session_timer_ms );
      (void)rex_set_timer( &ecall_app_task_struct->session_timer,
                           ecall_app_task_struct->ecall_efs_config.test_only_session_timer_ms );
      break; 
    
    case ECALL_IVS_MSD_ROBUST_MODE:
      ECALL_MSG_0( ECALL_HIGH, "IVS Rx ECALL_IVS_MSD_ROBUST_MODE" );
      break; 
    
    case ECALL_IVS_ACK_RECEIVED:
      ECALL_MSG_0( ECALL_HIGH, "IVS Rx ECALL_IVS_ACK_RECEIVED" );
      (void)rex_clr_timer(&ecall_app_task_struct->unmute_timer);
      break; 
    
    case ECALL_IVS_NACK_RECEIVED:
      ECALL_MSG_0( ECALL_HIGH, "IVS Rx ECALL_IVS_NACK_RECEIVED" );
      (void)rex_clr_timer(&ecall_app_task_struct->unmute_timer);
      break; 
    
    case ECALL_IVS_ERR:
      /* If modem can't register with vocoder or any other error */
      ECALL_MSG_0( ECALL_HIGH, "IVS Rx ECALL_IVS_ERR" );
      report_ecall_event(EVENT_ECALL_SESSION_FAILURE);
      if (current_app_state == ECALL_APP_IN_CALL_TRANSMITTING)
      {
        ecall_app_task_change_state(ECALL_APP_IN_CALL);
      }
      break;  

    case ECALL_IVS_TX_COMPLETED: /* Received 2 LL ACKS */
      ECALL_MSG_0( ECALL_HIGH, "IVS Rx ECALL_IVS_TX_COMPLETED (Received 2 LL ACKS)" );
      ecall_app_task_struct->ecall_app_task_discard_msd = TRUE;
      ecall_app_task_change_state(ECALL_APP_WAITING_FOR_AL_ACK);
      break; 

    case ECALL_IVS_HLACK_RECEIVED:
      ECALL_MSG_0( ECALL_HIGH, "IVS Rx ECALL_IVS_HLACK_RECEIVED" );
      (void)rex_clr_timer(&ecall_app_task_struct->unmute_timer);
      ecall_app_task_struct->hlack_received = TRUE;
      /* Resume fetching GPS fixes right after receiving HLACK for PDAPI */
      #if ( (defined FEATURE_CGPS) && !(defined FEATURE_ECALL_HAS_QMI_LOC) ) /* PDAPI */
      (void)rex_set_timer ( &ecall_app_task_struct->gps_timer, 0 );
      #endif /* PDAPI */
      break;  

    case ECALL_IVS_CLEARDOWN_RECEIVED:
      ECALL_MSG_1( ECALL_HIGH, "IVS Rx ECALL_IVS_CLEARDOWN_RECEIVED hlackdata %d",
                                ivs_status.hlack_data );
      ecall_app_task_struct->ecall_ivs_cleardown_received = TRUE;
      ecall_app_task_struct->ecall_ivs_positive_ack_received = FALSE;
      if (ecall_end_call() == FALSE)
      {
        ECALL_MSG_0( ECALL_MED, "ERROR Clearing down the call" );
      }
      break; 
    
    case ECALL_IVS_LOSE_SYNC:
      ECALL_MSG_0( ECALL_HIGH, "IVS Rx ECALL_LOSE_SYNC" );
      if (ecall_app_task_struct->hlack_received == TRUE)
      {
        /* If HLACK is already received, unmute the speaker after lose sync */
        /* Unmute Vocoder RX output to Speaker to resume voice going into speaker */
        if(ecall_app_task_struct->ecall_efs_config.voc_config == TRUE) 
        {
          ecall_app_unmute_rx_out();
        }
      }
      else
      {
        /* If HLACK is not yet received but lose sync, unmute speaker after 3 second *
         * if no new message is decoded                                              */
        rex_set_timer(&ecall_app_task_struct->unmute_timer, ECALL_UNMUTE_TIMEOUT);
      }
      break;
  
    default:
      ECALL_MSG_1( ECALL_MED, "ecall_app_task_tx_status_cb_func Received Unkown, tx_status = %d",
                               ivs_status.tx_status );
      break;

  } /* switch*/

} /* ECALL_APP_TASK_PROCESS_TX_STATUS */

/*==========================================================================

  FUNCTION ECALL_APP_TASK_PROCESS_INCOMING_ECALL

  DESCRIPTION
    This function will process the incoming eCall and answer it.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_task_process_incoming_ecall
(
  cm_call_id_type     incom_call_id,
  cm_call_type_e_type incom_call_type
)
{
  cm_ans_params_s_type *answer_params_ptr = NULL;
  /*-----------------------------------------------------------------------
    Allocate dynamic memory for the CM call answer structure.
  ------------------------------------------------------------------------*/
  answer_params_ptr
    = (cm_ans_params_s_type*)
        modem_mem_alloc(sizeof(cm_ans_params_s_type),
                        MODEM_MEM_CLIENT_ECALL_CRIT);

  if( answer_params_ptr == NULL )
  {
    modem_mem_free(answer_params_ptr, MODEM_MEM_CLIENT_ECALL_CRIT);
    ECALL_MSG_0(ECALL_ERROR, "memory allocation failed for answer_params structure");
    return;
  }

  ECALL_MSG_1( ECALL_HIGH, "MT ECALL (call_id %d), changing state to ECALL_APP_IN_CALL",
                            incom_call_id );
  /* Update call_id */
  ecall_app_task_struct->ecall_app_task_call_id = incom_call_id;

  memset((void*)answer_params_ptr,0, sizeof(cm_ans_params_s_type));
  answer_params_ptr->info_type = CM_CALL_MODE_INFO_GW_CS;
  answer_params_ptr->ans_params.gw_cs_ans.accept = TRUE;
  answer_params_ptr->ans_params.gw_cs_ans.call_type = incom_call_type;

  if (!cm_mm_call_cmd_answer(ecall_call_err_cb, NULL, ecall_app_task_struct->ecall_app_task_cm_client_id, incom_call_id, answer_params_ptr))
  {
    ECALL_MSG_0( ECALL_ERROR, "Ecall- Could not answer the incoming call" );
  }
  else
  {
    /* NOTE: in this case, everytime MT call, we cann abort_tx() */
    ecall_app_task_change_state(ECALL_APP_IN_CALL); /* go to MT ECALL */
  }

  /* Free the answer_params structure */
  modem_mem_free((void *)answer_params_ptr, MODEM_MEM_CLIENT_ECALL_CRIT );

} /* ECALL_APP_TASK_PROCESS_INCOMING_ECALL */

/*==========================================================================

  FUNCTION ECALL_APP_TASK_START_SESSION_STATUS_UPDATE

  DESCRIPTION
    This function report the start session status to registered callbacks.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_task_start_session_status_update
(
  ecall_session_status_type  session_status,
  cm_call_id_type            call_id,
  cm_client_id_type          client_id,
  sys_modem_as_id_e_type     as_id,
  void                      *data_block_ptr
)
{
  ecall_call_orig_exten_info_s_type  exten_info;

  if(ecall_app_task_struct->start_session_cb_func != NULL)
  {
    ECALL_MSG_1( ECALL_MED, "Send out session_status %d to start_session_cb_func()",
                             session_status );
    ecall_app_task_struct->start_session_cb_func(session_status);
  }

  if (ecall_app_task_struct->call_orig_info_cb_func != NULL)
  {
    ECALL_MSG_3( ECALL_MED, "Send out session_status %d, call_id %d, client_id %d to call_orig_info_cb_func()",
                             session_status,
                             call_id,
                             client_id );
    ecall_app_task_struct->call_orig_info_cb_func(
                            ecall_app_task_struct->data_block_ptr,
                            session_status,
                            call_id,
                            client_id);
  }

  if (ecall_app_task_struct->call_orig_exten_info_cb_func != NULL)
  {
    ECALL_MSG_4( ECALL_MED, "Send out session_status %d, call_id %d, client_id %d, as_id %d to call_orig_exten_info_cb_func()",
                             session_status,
                             call_id,
                             client_id,
                             as_id );
    exten_info.is_as_id_available = TRUE;
    exten_info.as_id = as_id;
    ecall_app_task_struct->call_orig_exten_info_cb_func(
                            ecall_app_task_struct->data_block_ptr,
                            session_status,
                            call_id,
                            client_id,
                            exten_info);
  }
}

/*==========================================================================

  FUNCTION ECALL_APP_TASK_PROCESS_SESSION_START

  DESCRIPTION
    This function will process the eCall Session Start command.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_task_process_session_start
(
  ecall_app_task_cmd_type *cmd_ptr
)
{
  ecall_app_state_type      current_app_state = ECALL_APP_ECALL_INACTIVE;
  ecall_operating_mode_type operating_mode;

  current_app_state = ecall_app_task_get_state();
  operating_mode = ecall_get_ecall_operating_mode();

  ecall_app_task_struct->is_session_start_with_mt_ecall = FALSE;

  /* Update the call_orig_info callbacks */
  ecall_app_task_struct->start_session_cb_func = cmd_ptr->data.start_session_config.start_session_cb;
  ecall_app_task_struct->call_orig_info_cb_func = cmd_ptr->data.start_session_config.call_orig_info_cb;
  ecall_app_task_struct->call_orig_exten_info_cb_func = cmd_ptr->data.start_session_config.call_orig_exten_info_cb;
  ecall_app_task_struct->data_block_ptr = cmd_ptr->data.start_session_config.data_block_ptr;

  /* Update the external MSD if it is available */
  if ( TRUE == cmd_ptr->data.start_session_config.is_external_msd )
  {
    ecall_app_task_struct->is_external_msd = TRUE;
    ecall_memscpy(ecall_app_task_struct->external_msd, sizeof(ecall_app_task_struct->external_msd),
                  cmd_ptr->data.start_session_config.external_msd, sizeof(cmd_ptr->data.start_session_config.external_msd));
  }

  /* Check eCall operating mode */
  if( (operating_mode == ECALL_MODE_UNKNOWN) &&
      (FALSE == ecall_app_task_get_is_era_glonass_enabled()) )
  {
    ECALL_MSG_0( ECALL_ERROR, "Ecall mode is unknown.. check if sim/usim present" );
  }

#ifdef FEATURE_DUAL_SIM
  if ( (TRUE  == cmd_ptr->data.start_session_config.qmi_start) &&
       (FALSE == ecall_app_task_is_session_start_as_id_valid(cmd_ptr->data.start_session_config.as_id)) )
  {
    ECALL_MSG_1( ECALL_ERROR, "as_id %d does not match with eCall as_id",
                               cmd_ptr->data.start_session_config.as_id );

    ecall_app_task_start_session_status_update(
                      ECALL_FAILURE,
                      (cm_call_id_type)-1,
                      (cm_client_id_type)-1,
                      cmd_ptr->data.start_session_config.as_id,
                      NULL );
    return;
  }
  else
  {
    ECALL_MSG_1( ECALL_HIGH, "process_session_start(): input as_id is valid %d",
                              cmd_ptr->data.start_session_config.as_id );
  }
#endif /* FEATURE_DUAL_SIM */

  if( ((ecall_app_is_emergency_call_on_going() == FALSE) &&
         ((current_app_state == ECALL_APP_ECALL_INACTIVE)||(current_app_state == ECALL_APP_IDLE_ALLOW_MT_ECALL))) ||
      ((ecall_app_task_struct->call_direction == ECALL_MT_CALL) &&
         ((current_app_state == ECALL_APP_IN_CALL_TRANSMITTING)||(current_app_state == ECALL_APP_IN_CALL)||(current_app_state == ECALL_APP_WAITING_FOR_AL_ACK))) )
  {
    if(ecall_app_task_get_nv() == TRUE)
    {
      ecall_app_set_ecall_start_config(&cmd_ptr->data.start_session_config.start_config);

      /* Check if MT eCall is on going or not first.  If yes, we need to end the MT eCall first before
       * trigger new eCall session.
       */
      if ((ecall_app_task_struct->call_direction == ECALL_MT_CALL) &&
          ((current_app_state == ECALL_APP_IN_CALL_TRANSMITTING)||(current_app_state == ECALL_APP_IN_CALL)||(current_app_state == ECALL_APP_WAITING_FOR_AL_ACK)))
      {
        ecall_app_task_struct->is_session_start_with_mt_ecall = TRUE;
      }

      ecall_app_dump_non_emergency_ps_call();

      /* Read ecall_efs_config again from EFS to update with the latest config */
      if (ecall_app_task_struct->is_era_glonass_enabled == FALSE)
      {
        ecall_efs_read_ecall_config(&ecall_app_task_struct->ecall_efs_config);
      }
      else
      {
        ecall_efs_read_era_glonass_config(&ecall_app_task_struct->ecall_efs_config);
      }

      /* Propagate latest eCall EFS Config to eCall App */
      ecall_app_get_efs_params(&ecall_app_task_struct->ecall_efs_config);

      report_ecall_event(EVENT_ECALL_START);

      /* Change state to ECALL_APP_ORIGINATING_CALL only if there is no on-going MT eCall */
      if (ecall_app_task_struct->is_session_start_with_mt_ecall == FALSE)
      {
        ecall_app_task_change_state(ECALL_APP_ORIGINATING_CALL);
      }
      else
      {
        ECALL_MSG_0 ( ECALL_HIGH, "eCall Session will start after MT eCall ended" );
      }
    }
    else
    {
      ECALL_MSG_0( ECALL_HIGH, "ECALL Not enabled thru NV, Cannot proceed with ECALL" );
      ecall_app_task_start_session_status_update(
                      ECALL_FAILURE,
                      (cm_call_id_type)-1,
                      (cm_client_id_type)-1,
                      cmd_ptr->data.start_session_config.as_id,
                      NULL );
            
    }
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "ECALL or E112 is already in-progress, cannot start a second one" );
  }

}

/*==========================================================================

  FUNCTION ECALL_APP_TASK_DEREGISTER_MODEM_CLIENT_IF_REGISTERED

  DESCRIPTION
    This function will de-register eCall modem client if one is registered.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_task_deregister_modem_client_if_registered
(
  void
)
{
  /* Deregister eCall clients */
  if(ecall_app_task_struct->emodem_client_reg == TRUE)
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_app_task calling emodem_client_deregister");
    ecall_app_task_struct->emodem_client_reg = FALSE;
    (void)ecall_emodem_client_deregister(ecall_app_client_struct.clientid);
  }
  
} /* ECALL_APP_TASK_DEREGISTER_MODEM_CLIENT_IF_REGISTERED */


/*==========================================================================

  FUNCTION ECALL_APP_TASK_PROCESS_CMD

  DESCRIPTION
    Processes commands off of the ECALL_APP_TASK command queue, and returns the command
    buffers to the free queue.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void ecall_app_task_process_cmd(void)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;
  ecall_app_state_type current_app_state = ECALL_APP_ECALL_INACTIVE;

  while((cmd_ptr = (ecall_app_task_cmd_type *) q_get(&ecall_app_task_cmd_q)) != NULL)
  {
    switch(cmd_ptr->cmd_id)
    {
      case ECALL_APP_CMD_GPS_FIX_RECVD:
        ECALL_MSG_0( ECALL_MED, "Rx ECALL_APP_CMD_GPS_FIX_RECVD" );
        /* Update gps_engine_warmup */
        ecall_app_task_struct->gps_engine_warmup = TRUE;

        /* Update eCall GPS data */
        #ifdef FEATURE_ECALL_HAS_QMI_LOC
        ecall_save_position (
                   cmd_ptr->data.gps_data.latitude_valid,
                   cmd_ptr->data.gps_data.longitude_valid,
                   cmd_ptr->data.gps_data.timestamp_valid,
                   cmd_ptr->data.gps_data.vehicledirection_valid,
                   cmd_ptr->data.gps_data.horConfidence_valid,
                   cmd_ptr->data.gps_data.horUncCircular_valid,
                   cmd_ptr->data.gps_data.latitude,
                   cmd_ptr->data.gps_data.longitude,
                   cmd_ptr->data.gps_data.timestamp,
                   cmd_ptr->data.gps_data.vehicledirection,
                   cmd_ptr->data.gps_data.horConfidence,
                   cmd_ptr->data.gps_data.horUncCircular);
        #elif ( (defined FEATURE_CGPS) && !(defined FEATURE_ECALL_HAS_QMI_LOC) ) /* PDAPI */
        ecall_save_position (cmd_ptr->data.gps_data.ecall_cmd_gps_data);
        #endif /* FEATURE_ECALL_HAS_QMI_LOC */

        break;

      case ECALL_APP_CMD_GPS_ERROR_RECVD:
        ECALL_MSG_0( ECALL_MED, "Rx ECALL_APP_CMD_GPS_ERROR_RECVD" );
        ecall_gps_process_gps_error_recvd();
        break;

      case ECALL_APP_CMD_SESSION_START:
        ECALL_MSG_0( ECALL_HIGH, "Rx ECALL_APP_CMD_SESSION_START" );
        ecall_app_task_process_session_start(cmd_ptr);
        break;

      case ECALL_APP_CMD_INCOMING_ECALL:
        ecall_app_task_process_incoming_ecall(cmd_ptr->data.call_id,
                                              cmd_ptr->data.incom_call_config.ecall_incom_call_type);
        break;


      case ECALL_APP_CMD_END_ECALL:
        ECALL_MSG_0( ECALL_HIGH, "Rx ECALL_APP_CMD_END_ECALL" );

        /* Update call_direction, redial_pending and is_call_fail_at_orig */
        ecall_app_task_struct->call_direction = cmd_ptr->data.end_call_config.call_direction;
        ecall_app_task_struct->redial_pending = cmd_ptr->data.end_call_config.redial_pending;
        ecall_app_task_struct->is_call_fail_at_orig = cmd_ptr->data.end_call_config.is_call_fail_at_orig;
        ECALL_MSG_3( ECALL_HIGH, "END_ECALL with call_direction %d, redial_pending %d, is_call_fail_at_orig %d",
                                  ecall_app_task_struct->call_direction,
                                  ecall_app_task_struct->redial_pending,
                                  ecall_app_task_struct->is_call_fail_at_orig );
                    
        /* Reset call_id */
        ecall_app_task_struct->ecall_app_task_call_id = (uint8)-1;

        if (cmd_ptr->data.end_call_config.end_status == CM_CALL_END_NO_SRV ) 
        {
          /* To indicate CM_CALL_END_NO_SRV */
          ecall_app_task_struct->session_status = ECALL_FAILURE;
        }

        /* -------------------------------------------------------------------------
         * Start the callback timer when a MO eCall gets terminated.  eCall App will 
         * auto-answer any MT calls before this timer expires. The OEM can disable 
         * or cancel this timer if they don't want this auto-answer feature.
         * ------------------------------------------------------------------------- */
        if (ecall_app_task_struct->call_direction == ECALL_MO_CALL)
        {
           (void)rex_set_timer( &ecall_app_task_struct->ecall_callback_timer,
                                ecall_app_task_struct->ecall_efs_config.test_only_callback_timer_ms );
           ECALL_MSG_3( ECALL_HIGH, "Set callback timer %d ms, with ivs_cleardown %d, mo_ecall_normal_clearing %d",
                                     ecall_app_task_struct->ecall_efs_config.test_only_callback_timer_ms,
                                     ecall_app_task_struct->ecall_ivs_cleardown_received,
                                     cmd_ptr->data.end_call_config.is_mo_ecall_normal_clearing );
        }

        ecall_process_cmd_end_call();
        break;

      case ECALL_APP_CMD_ECALL_CONNECTED:
        ECALL_MSG_0( ECALL_HIGH, "Rx ECALL_APP_CMD_ECALL_CONNECTED" );

        /* Reset redial_pending and call_orig_fail_redial_index when MO call gets connected */
        if (cmd_ptr->data.ecall_connected_config.call_direction == ECALL_MO_CALL)
        {
          ecall_app_task_struct->redial_pending = FALSE;                 /* HLAP EN 16062 7.12.4 */
          ecall_app_task_struct->call_orig_fail_redial_index = 0;
          ecall_app_task_struct->is_session_start_with_mt_ecall = FALSE;
        }

        ecall_app_task_struct->call_direction = cmd_ptr->data.ecall_connected_config.call_direction;
        ecall_app_task_struct->tx_mode = cmd_ptr->data.ecall_connected_config.tx_mode;
        ecall_app_client_struct.cb_func  = ecall_app_task_tx_status_cb_func;
        
        if( TRUE == ecall_app_task_struct->emodem_client_reg)
        {
          ECALL_MSG_0( ECALL_ERROR, "APP client already registered" );
        }
        else if(ECALL_EMODEM_CLIENT_OK != ecall_emodem_client_init(&ecall_app_client_struct.clientid))
        {
          ECALL_MSG_0( ECALL_ERROR, "error initialising APP client" );
        }
        else if(ECALL_EMODEM_CLIENT_OK !=ecall_emodem_client_register(&ecall_app_client_struct))
        {
          ECALL_MSG_0( ECALL_ERROR, "error registering APP client" );
        }
        else
        {
          ECALL_MSG_1( ECALL_HIGH, "APP client registered clientid %d",
                                    ecall_app_client_struct.clientid );
          ecall_app_task_struct->emodem_client_reg = TRUE;
        }

        ECALL_MSG_1( ECALL_MED, "Set call_cleardown_timer to %d ms",
                                 ecall_app_task_struct->ecall_efs_config.test_only_call_cleardown_timer_ms);
        (void)rex_set_timer( &ecall_app_task_struct->call_cleardown_timer,
                             ecall_app_task_struct->ecall_efs_config.test_only_call_cleardown_timer_ms );
        if (ecall_app_task_struct->call_direction == ECALL_MT_CALL)
        {
          /* Incoming Call and Waiting for PSAP callback */
          ECALL_MSG_0( ECALL_HIGH, "Incoming Call and Waiting for PSAP callback" );
          /* Do not clear the callback timer when call_direction is MT CALL;
             otherwise ecall_app might stuck in idle_allow_mt_ecall state after
             MT call ended by PSAP */
          ecall_app_task_change_state(ECALL_APP_IN_CALL_TRANSMITTING);
        }	/* MT CALL */
        else if (ecall_app_task_struct->call_direction == ECALL_MO_CALL) 
        {
          ECALL_MSG_0( ECALL_MED, "MO eCall connected, clear callback_timer");
          (void)rex_clr_timer(&ecall_app_task_struct->ecall_callback_timer);
          
          /* Call is connected, wait for START mesages */
          (void)rex_set_timer( &ecall_app_task_struct->start_timer, ECALL_START_TIMEOUT );
          ecall_app_task_change_state(ECALL_APP_IN_CALL_TRANSMITTING);
        }
        break;

      case ECALL_APP_CMD_STOP_ECALL:
        ECALL_MSG_0( ECALL_HIGH, "Rx ECALL_APP_CMD_STOP_ECALL, Reset ECALL state machine" );

        /* Clear retry_timer and reset redial_pending when user manually ends eCall */
        (void)rex_clr_timer(&ecall_app_task_struct->retry_timer);
        ecall_app_task_struct->redial_pending = FALSE;

        current_app_state = ecall_app_task_get_state();
        if (current_app_state != ECALL_APP_ECALL_INACTIVE) /* If a CALL is active, end it */
        {
          if (ecall_end_call() == FALSE)
          {
            ECALL_MSG_0( ECALL_MED, "ERROR Clearing down the call" );
          }
        }
        ecall_process_cmd_end_call();
        ecall_app_task_change_state(ECALL_APP_ECALL_INACTIVE);
        break;

      case ECALL_APP_CMD_TX_STATUS:
        ecall_app_task_process_tx_status(cmd_ptr->data.ivs.tx_user_data, 
                                         cmd_ptr->data.ivs.ivs_status);
        break;

      case ECALL_APP_CMD_GET_CALL_INFO:
        ecall_process_session_get_call_info_cmd(cmd_ptr->data.cmd_call_info_cb);
        break;

      case ECALL_APP_CMD_PBM_EVENT_UPDATE:
        ECALL_MSG_1( ECALL_HIGH, "Rx ECALL_APP_CMD_PBM_EVENT_UPDATE dev %d",
                                  cmd_ptr->data.pbm.pb_id.device_type );
        ecall_app_process_pbm_notify_update(cmd_ptr->data.pbm.pb_id);
        break;

      case ECALL_APP_CMD_PBM_SIM_FILE_INFO:
        ECALL_MSG_1( ECALL_HIGH, "Rx ECALL_APP_CMD_PBM_SIM_FILE_INFO dev %d",
                                  cmd_ptr->data.pbm.pb_id.device_type );
        if(cmd_ptr->data.pbm.pb_id.device_type == PBM_FDN)
        {
          ecall_app_process_fdn_sim_file_info(cmd_ptr->data.pbm.pb_id,
                                              cmd_ptr->data.pbm.info);
        }
        else if(cmd_ptr->data.pbm.pb_id.device_type == PBM_SDN)
        {
          ecall_app_process_sdn_sim_file_info(cmd_ptr->data.pbm.pb_id,
                                              cmd_ptr->data.pbm.info);
        }
        break;

      case ECALL_APP_CMD_APR_PACKET:
        #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
        ecall_app_process_apr_packet(cmd_ptr->data.apr_packet);
        #else
        ECALL_MSG_0( ECALL_ERROR, "VOICE_A or VOICE_B not defined, but receives ECALL_APP_CMD_APR_PACKET" );
        #endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */
        break;

      case ECALL_APP_CMD_UPDATE_ECALL_CALL_ID:
        ECALL_MSG_1( ECALL_HIGH, "Rx ECALL_APP_CMD_UPDATE_ECALL_CALL_ID call_id %d",
                                  cmd_ptr->data.call_id );
        ecall_app_task_struct->ecall_app_task_call_id = cmd_ptr->data.call_id;
        break;

      case ECALL_APP_CMD_UPDATE_CALL_STATUS:
        ECALL_MSG_0( ECALL_HIGH, "Rx ECALL_APP_CMD_UPDATE_CALL_STATUS" );
        ecall_app_process_update_call_status(cmd_ptr->data.call_status_data.call_event,
                                             cmd_ptr->data.call_status_data.call_id,
                                             cmd_ptr->data.call_status_data.call_type);
        break;

      case ECALL_APP_CMD_SETUP_CALL:
        ECALL_MSG_0( ECALL_HIGH, "Rx ECALL_APP_CMD_SETUP_CALL" );
        ecall_app_process_setup_call(cmd_ptr->data.call_id);
        break;

      case ECALL_APP_CMD_MT_ECALL_END_START_MO_ECALL:
        ECALL_MSG_0( ECALL_HIGH, "Rx ECALL_APP_CMD_MT_ECALL_END_START_MO_ECALL" );
        /* Update call_direction */
        ecall_app_task_struct->call_direction = ECALL_MT_CALL;

        /* Reset call_id */
        ecall_app_task_struct->ecall_app_task_call_id = (uint8)-1;

        /* Deregister eCall clients */
        ecall_app_task_deregister_modem_client_if_registered();

        /* FULL Abort current transmission */
        ecall_abort_all();

        /* Change state to ECALL_APP_IDLE_ALLOW_MT_ECALL to reset eCall App */
        ecall_app_task_change_state(ECALL_APP_IDLE_ALLOW_MT_ECALL);

        /* Change state to ECALL_APP_ORIGINATING_CALL to start this MO eCall session */
        ecall_app_task_change_state(ECALL_APP_ORIGINATING_CALL);
        break;

      case ECALL_APP_CMD_UPDATE_ECALL_PROFILE:
        ECALL_MSG_0( ECALL_HIGH, "Rx ECALL_APP_CMD_UPDATE_ECALL_PROFILE" );
        ecall_app_task_process_ecall_profile_update(cmd_ptr->data.ecall_profile);
        break;

      case ECALL_APP_CMD_CM_SUBS_EVENT_INFO:
      #ifdef FEATURE_DUAL_SIM
        ECALL_MSG_0( ECALL_HIGH, "Rx ECALL_APP_CMD_CM_SUBS_EVENT_INFO" );
        ecall_app_task_process_cm_subs_event_info(cmd_ptr);
      #else /* FEATURE_DUAL_SIM not defined */
        ECALL_MSG_0( ECALL_ERROR, "FEATURE_DUAL_SIM not defined, but Rx ECALL_APP_CMD_CM_SUBS_EVENT_INFO" );
      #endif /* FEATURE_DUAL_SIM */
        break;

      case ECALL_APP_CMD_UPDATE_MSD_SETTING:
        ecall_app_task_process_update_msd_setting(cmd_ptr->data.msd_setting);
        break;

      default:
        ECALL_MSG_1( ECALL_MED, "Unknown command %d passed to eCall App task",
                                 cmd_ptr->cmd_id );
        break;
    }

    q_put(&ecall_app_task_cmd_free_q, &cmd_ptr->link);
  }
} /* ecall_app_task_process_cmd() */

/*==========================================================================

  FUNCTION ECALL_APP_TASK_TX_STATUS_CB_FUNC

  DESCRIPTION
    Call back function sent to START_TRANSMIT_DATA, this function handles all the MSD transmission states

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
/*lint -e818 -esym(715,user_data) Pointer parameter could be declared as pointing to const */
void ecall_app_task_tx_status_cb_func
(
  void             *user_data,
  ecall_cb_data_type ivs_status
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;

  cmd_ptr = ecall_app_task_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    /* Post a command to the eCall App task to process this PCM frame */
    cmd_ptr->cmd_id = ECALL_APP_CMD_TX_STATUS;
    cmd_ptr->data.ivs.tx_user_data = user_data;
    cmd_ptr->data.ivs.ivs_status = ivs_status;

    ecall_app_task_put_cmd(cmd_ptr);
  }
  else
  {
    ECALL_MSG_0( ECALL_MED, "Ran out of command buffers in ecall_app_task_tx_status_cb_func()" );
  }

} /* ecall_app_task_tx_status_cb_func */

/*==========================================================================

  FUNCTION ECALL_GET_CALL_ID

  DESCRIPTION
    This api returns the current call ID for ecall App task

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
===========================================================================*/
cm_call_id_type ecall_get_call_id
(
  void
)
{
  return ecall_app_task_struct->ecall_app_task_call_id;
} /* ECALL_GET_CALL_ID() */

/*==========================================================================
  FUNCTION ECALL_APP_PROCESS_GPS_TIMER_SIGNAL

  DESCRIPTION
    This function will process the GPS_TIMER expire signal.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_app_process_gps_timer_signal
(
  void
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;
  ecall_app_state_type current_app_state = ECALL_APP_ECALL_INACTIVE;
  current_app_state = ecall_app_task_get_state();

  cmd_ptr = ecall_app_task_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    /* Post a command to the eCall App task to process this GPS error (timer expire) */
    cmd_ptr->cmd_id = ECALL_APP_CMD_GPS_ERROR_RECVD;
    ecall_app_task_put_cmd(cmd_ptr);
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_app_process_gps_timer_signal()" );
  }

  /* Check does GPS engine warmup already or not */
  if (ecall_app_task_struct->gps_engine_warmup == FALSE)
  {
    /* Give GPS engine more time to get the first fix to warm up */
    ECALL_MSG_1( ECALL_MED, "Setup %d ms timer to get GPS",
                             ECALL_GPS_FIX_TIMEOUT );
    (void) rex_set_timer( &ecall_app_task_struct->gps_timer, ECALL_GPS_FIX_TIMEOUT );

    if (ecall_app_task_struct->gps_engine_initialized == FALSE)
    {
      /* eCall App will set gps_engine_initialized to TRUE when the QMI LOC signal arrives,
         and then call ecall_getposition() for QMI LOC.
       */
      if(!ecall_gps_init())
      {
        ECALL_MSG_1( ECALL_ERROR, "FAILED to initialize GPS. Will try after %d ms",
                                   ECALL_GPS_FIX_TIMEOUT );
        ecall_app_task_struct->gps_engine_initialized = FALSE;
      }
      #if((defined FEATURE_CGPS) && !(defined FEATURE_ECALL_HAS_QMI_LOC)) /* PDAPI */
      else
      {
        ecall_app_task_struct->gps_engine_initialized = TRUE;
        /* Do not request for GPS update during MSD transmission
           since it could affect transmission performance */
        if (current_app_state != ECALL_APP_IN_CALL_TRANSMITTING)
        {
          ecall_getposition((uint32)ECALL_GPS_FIX_TIMEOUT);
        }
      }
      #endif /* PDAPI */
    }
    else
    {
      /* GPS engine initialized but not warmup yet */
      /* Do not request for GPS update during MSD transmission
         since it could affect transmission performance */
      if (current_app_state != ECALL_APP_IN_CALL_TRANSMITTING)
      {
        ecall_getposition((uint32)ECALL_GPS_FIX_TIMEOUT);
      }
    }
  }
  else /* GPS engine already warmup, get position directly */
  {
    ECALL_MSG_1( ECALL_MED, "Setup %d ms timer to get GPS",
                             ecall_app_task_struct->ecall_efs_config.gnss_update_time_ms );
    (void) rex_set_timer( &ecall_app_task_struct->gps_timer, 
                          ecall_app_task_struct->ecall_efs_config.gnss_update_time_ms );
    /* Do not request for GPS update during MSD transmission
       since it could affect transmission performance */
    if (current_app_state != ECALL_APP_IN_CALL_TRANSMITTING)
    {
      ecall_getposition(ecall_app_task_struct->ecall_efs_config.gnss_update_time_ms);
    }
  }
} /* ECALL_APP_PROCESS_GPS_TIMER_SIGNAL */

#if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
/*===========================================================================

     A - F a m i l y   &   B - F a m i l y   C o m m o n   F u n c t i o n

===========================================================================*/

/*===========================================================================

  FUNCTION ECALL_APP_APR_ISR_CB

  DESCRIPTION
    This function will called whenever an APR ISR happens. Will copy everything
    and put into ecall_app queue to process.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static int32 ecall_app_apr_isr_cb 
(
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;
  int32 ret;

  cmd_ptr = ecall_app_task_get_cmd_buf();
  if (cmd_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf() ran out of cmd buffer at ecall_app_apr_isr_cb" );
    return APR_EFAILED;
  }
  else
  {
    /* Post a command to the eCall task to process this APR packet */
    cmd_ptr->cmd_id = ECALL_APP_CMD_APR_PACKET;

    /* Copy APR packet pointer and process at ecall_ivs_task */
    cmd_ptr->data.apr_packet = packet;
    
    ecall_app_task_put_cmd(cmd_ptr);
  }

  ret = __aprv2_cmd_accept_command(ecall_app_apr_handle,packet);
  if(ret != APR_EOK)
  {
    ECALL_MSG_0( ECALL_ERROR, "__aprv2_cmd_accept_command failed at ecall_app_apr_isr_cb" );
    return APR_EFAILED;
  }

  return APR_EOK;

} /* ECALL_APP_APR_ISR_CB */

/*===========================================================================

  FUNCTION ECALL_APP_APR_INIT

  DESCRIPTION
    This function will initialize the APR interface for eCall APP

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_apr_init 
( 
  void
)
{
  int32 ret;

  ECALL_MSG_0( ECALL_MED, "ECALL_APP INIT APR INTERFACE" );

  /* Register with APR to receive messages. */
  ret = __aprv2_cmd_register2(
            &ecall_app_apr_handle,           /* <- Return eCall handle */
            ecall_app_ecall_dns,             /* -> Name string of the eCall service */
            sizeof(ecall_app_ecall_dns),     /* -> Size of the name string, including NULL */
            0,                               /* -> Service address to register */
            ecall_app_apr_isr_cb,            /* -> ISR dispatch callback to register */
            NULL,                            /* -> Static data to pass to the ISR callback */
            &ecall_app_apr_src_addr);        /* <- Return pointer to the registered service address */
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_app_apr_init: __aprv2_cmd_register2 failed, ret %d",
                               ret );
  }

  ret = __aprv2_cmd_local_dns_lookup(
            ecall_app_voc_dns, 
            sizeof(ecall_app_voc_dns), 
            &ecall_app_apr_dest_addr);
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_app_apr_init: __aprv2_cmd_local_dns_lookup failed, ret %d",
                               ret );
  }

  ECALL_MSG_3( ECALL_HIGH, "ecall_APP_apr_handle %x, ecall_APP_apr_src_addr %x, ecall_APP_dest_addr %x", 
                            ecall_app_apr_handle,
                            ecall_app_apr_src_addr,
                            ecall_app_apr_dest_addr );

} /* ECALL_APP_APR_INIT */

/*==========================================================================
  FUNCTION ECALL_APP_PROCESS_IBASIC_RSP_RESULT

  DESCRIPTION
    This function will process the incoming IBASIC_RSP_RESULT with
    the incoming APR packet and the token code.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_app_process_ibasic_rsp_result
(
  aprv2_packet_t *packet
)
{
  aprv2_ibasic_rsp_result_t *rsp_result_ptr = NULL;
  uint32_t status;

  /* Get the payload */
  if (packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "app_process_ibasic_rsp_result: packet is NULL" );
    return;
  }
  rsp_result_ptr = APRV2_PKT_GET_PAYLOAD(aprv2_ibasic_rsp_result_t, packet);
  status = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, packet )->status;
  if(rsp_result_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "Failed to get payload at process_ibasic_rsp_result" );
  }
  
  switch (packet->token)
  {

    case ECALL_APP_APR_CREATE_PASSIVE_CONTROL_SESSION:
      if (status == APR_EOK)
      {
        ECALL_MSG_1( ECALL_HIGH, "Successfully create passive CVS control session, port %x",
                                  packet->src_port );
        ecall_app_apr_dest_port = packet->src_port;
        if(ecall_app_task_struct->pending_mute == TRUE)
        {
          ECALL_MSG_1( ECALL_MED, "pending mute %d, process mute now",
                                   ecall_app_task_struct->pending_mute );
          ecall_app_mute_rx_out();
        }
        else if (ecall_app_task_struct->pending_unmute == TRUE)
        {
          ECALL_MSG_1( ECALL_MED, "pending unmute %d, process unmute now",
                                   ecall_app_task_struct->pending_unmute );
          ecall_app_unmute_rx_out();
        }
      }
      else
      {
        ECALL_MSG_1( ECALL_ERROR, "Failed to create passive CVS control session, error code %d",
                                   status );
      }
      __aprv2_cmd_free(ecall_app_apr_handle, packet);
      break;
    
    case ECALL_APP_APR_MUTE_RX_OUT:
      if (status == APR_EOK)
      {
        ECALL_MSG_0( ECALL_HIGH, "Successfully MUTE voc RX" );
      }
      else
      {
        ECALL_MSG_1( ECALL_ERROR, "Failed to mute voc RX, error code %d",
                                   status );
      }
      ecall_app_task_struct->pending_mute = FALSE;
      __aprv2_cmd_free(ecall_app_apr_handle, packet);
      break;

    case ECALL_APP_APR_UNMUTE_RX_OUT:
      if (status == APR_EOK)
      {
        ECALL_MSG_0( ECALL_HIGH, "Successfully UNMUTE voc RX" );
      }
      else
      {
        ECALL_MSG_1( ECALL_ERROR, "Failed to unmute voc RX, error code %d",
                                   status );
      }
      ecall_app_task_struct->pending_unmute = FALSE;
      __aprv2_cmd_free(ecall_app_apr_handle, packet);
      break;

    default:
      ECALL_MSG_1( ECALL_ERROR, "Received an unsupported IBASIC_RSP_RESULT %d",
                                 packet->token );
      __aprv2_cmd_end_command(ecall_app_apr_handle, packet, APR_EUNSUPPORTED );
      break;
  }

} /* ECALL_APP_PROCESS_IBASIC_RSP_RESULT */

/*==========================================================================
  FUNCTION ECALL_APP_PROCESS_APR_PACKET

  DESCRIPTION
    This function will process the incoming APR packet.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_app_process_apr_packet
(
  aprv2_packet_t *packet
)
{
  switch (packet->opcode)
  {
    case APRV2_IBASIC_EVT_ACCEPTED:
      __aprv2_cmd_free(ecall_app_apr_handle, packet );
      break;

    case APRV2_IBASIC_RSP_RESULT:
      /* Will free the cmd in the function */
      ecall_app_process_ibasic_rsp_result(packet);
      break;

    default:
      ECALL_MSG_1( ECALL_ERROR, "Received an unsupported command %d",
                                 packet->opcode );
      __aprv2_cmd_end_command(ecall_app_apr_handle, packet, APR_EUNSUPPORTED );
      break;
  }

} /* ECALL_APP_PROCESS_APR_PACKET */

/*===========================================================================

  FUNCTION ECALL_APP_CVS_INIT

  DESCRIPTION
    This function will initialize the CVS session for eCall APP

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_cvs_init 
( 
  void
)
{
  int32 ret;

  ECALL_MSG_0( ECALL_MED, "ecall_app create a passive control session to get the dest port" );

  ret = __aprv2_cmd_alloc_send(
            ecall_app_apr_handle,                                    /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,                              /* msg_type     */
            ecall_app_apr_src_addr,                                  /* src_addr     */
            ecall_app_apr_src_port,                                  /* src_port     */
            ecall_app_apr_dest_addr,                                 /* dst_addr     */
            APR_NULL_V,                                              /* dst_port     */
            ECALL_APP_APR_CREATE_PASSIVE_CONTROL_SESSION,            /* token        */ 
            VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION,          /* opcode       */
            ecall_app_stream_name,                                   /* payload      */
            sizeof(ecall_app_stream_name));                          /* payload_size */
  
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send(CREATE_PASSIVE_CONTROL_SESSION) failed, ret=%d",
                               ret );
  }

} /* ECALL_APP_CVS_INIT */

#endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */


#ifdef FEATURE_ECALL_HAS_VOICE_A
/*===========================================================================

                     A - F a m i l y   F u n c t i o n

===========================================================================*/

/*==========================================================================
  FUNCTION ECALL_APP_MUTE_RX_OUT

  DESCRIPTION
    This function will mute the path from vocoder RX output to loudspeaker.

  DEPENDENCIES
    ecall_app_apr_dest_port.
    FEATURE_ECALL_HAS_VOICE_A

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_app_mute_rx_out
(
  void
)
{
  int32 ret;
  vss_ivolume_cmd_mute_t  mute_cmd;

  if (ecall_app_apr_dest_port == 0)
  {
    ECALL_MSG_0( ECALL_MED, "mute_rx_out() called but dest port is 0, will init APR and starting HPCM stream first" );
    ecall_app_task_struct->pending_mute = TRUE;
    ecall_app_apr_init();
    ecall_app_cvs_init();
    return;
  }

  mute_cmd.direction = VSS_IVOLUME_DIRECTION_RX;
  mute_cmd.mute_flag = VSS_IVOLUME_MUTE_ON;
  mute_cmd.ramp_duration_ms = 0;
  ret = __aprv2_cmd_alloc_send(
            ecall_app_apr_handle,                  /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,            /* msg_type     */
            ecall_app_apr_src_addr,                /* src_addr     */
            ecall_app_apr_src_port,                /* src_port     */
            ecall_app_apr_dest_addr,               /* dst_addr     */
            ecall_app_apr_dest_port,               /* dst_port     */
            ECALL_APP_APR_MUTE_RX_OUT,             /* token        */
            VSS_IVOLUME_CMD_MUTE,                  /* opcode       */
            &mute_cmd,                             /* payload      */
            sizeof(mute_cmd) );                    /* payload_size */

} /* ECALL_APP_MUTE_RX_OUT */

/*==========================================================================
  FUNCTION ECALL_APP_UNMUTE_RX_OUT

  DESCRIPTION
    This function will unmute the path from vocoder RX output to loudspeaker.

  DEPENDENCIES
    ecall_app_apr_dest_port.
    FEATURE_ECALL_HAS_VOICE_A

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_app_unmute_rx_out
(
  void
)
{
  int32 ret;
  vss_ivolume_cmd_mute_t  unmute_cmd;

  if (ecall_app_apr_dest_port == 0)
  {
    ECALL_MSG_0( ECALL_MED, "unmute_rx_out() called but dest port is 0, will init APR and starting HPCM stream first" );
    ecall_app_task_struct->pending_unmute = TRUE;
    ecall_app_apr_init();
    ecall_app_cvs_init();
    return;
  }

  unmute_cmd.direction = VSS_IVOLUME_DIRECTION_RX;
  unmute_cmd.mute_flag = VSS_IVOLUME_MUTE_OFF;
  unmute_cmd.ramp_duration_ms = 0;
  ret = __aprv2_cmd_alloc_send(
            ecall_app_apr_handle,                  /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,            /* msg_type     */
            ecall_app_apr_src_addr,                /* src_addr     */
            ecall_app_apr_src_port,                /* src_port     */
            ecall_app_apr_dest_addr,               /* dst_addr     */
            ecall_app_apr_dest_port,               /* dst_port     */
            ECALL_APP_APR_UNMUTE_RX_OUT,           /* token        */
            VSS_IVOLUME_CMD_MUTE,                  /* opcode       */
            &unmute_cmd,                           /* payload      */
            sizeof(unmute_cmd) );                  /* payload_size */

} /* ECALL_APP_UNMUTE_RX_OUT */

#elif (defined FEATURE_ECALL_HAS_VOICE_B)
/*===========================================================================

                     B - F a m i l y   F u n c t i o n

===========================================================================*/

/*==========================================================================
  FUNCTION ECALL_APP_MUTE_RX_OUT

  DESCRIPTION
    This function will mute the path from vocoder RX output to loudspeaker.

  DEPENDENCIES
    ecall_app_apr_dest_port.
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_app_mute_rx_out
(
  void
)
{
  int32 ret;
  vss_ivolume_cmd_mute_v2_t  mute_cmd;

  if (ecall_app_apr_dest_port == 0)
  {
    ECALL_MSG_0( ECALL_MED, "mute_rx_out() called but dest port is 0, will init APR and starting HPCM stream first" );
    ecall_app_task_struct->pending_mute = TRUE;
    ecall_app_apr_init();
    ecall_app_cvs_init();
    return;
  }

  mute_cmd.direction = VSS_IVOLUME_DIRECTION_RX;
  mute_cmd.mute_flag = VSS_IVOLUME_MUTE_ON;
  mute_cmd.ramp_duration_ms = 0;
  ret = __aprv2_cmd_alloc_send(
            ecall_app_apr_handle,                  /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,            /* msg_type     */
            ecall_app_apr_src_addr,                /* src_addr     */
            ecall_app_apr_src_port,                /* src_port     */
            ecall_app_apr_dest_addr,               /* dst_addr     */
            ecall_app_apr_dest_port,               /* dst_port     */
            ECALL_APP_APR_MUTE_RX_OUT,             /* token        */
            VSS_IVOLUME_CMD_MUTE_V2,               /* opcode       */
            &mute_cmd,                             /* payload      */
            sizeof(mute_cmd) );                    /* payload_size */

} /* ECALL_APP_MUTE_RX_OUT */

/*==========================================================================
  FUNCTION ECALL_APP_UNMUTE_RX_OUT

  DESCRIPTION
    This function will unmute the path from vocoder RX output to loudspeaker.

  DEPENDENCIES
    ecall_app_apr_dest_port.
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_app_unmute_rx_out
(
  void
)
{
  int32 ret;
  vss_ivolume_cmd_mute_v2_t  unmute_cmd;

  if (ecall_app_apr_dest_port == 0)
  {
    ECALL_MSG_0( ECALL_MED, "unmute_rx_out() called but dest port is 0, will init APR and starting HPCM stream first" );
    ecall_app_task_struct->pending_unmute = TRUE;
    ecall_app_apr_init();
    ecall_app_cvs_init();
    return;
  }

  unmute_cmd.direction = VSS_IVOLUME_DIRECTION_RX;
  unmute_cmd.mute_flag = VSS_IVOLUME_MUTE_OFF;
  unmute_cmd.ramp_duration_ms = 0;
  ret = __aprv2_cmd_alloc_send(
            ecall_app_apr_handle,                  /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,            /* msg_type     */
            ecall_app_apr_src_addr,                /* src_addr     */
            ecall_app_apr_src_port,                /* src_port     */
            ecall_app_apr_dest_addr,               /* dst_addr     */
            ecall_app_apr_dest_port,               /* dst_port     */
            ECALL_APP_APR_UNMUTE_RX_OUT,           /* token        */
            VSS_IVOLUME_CMD_MUTE_V2,               /* opcode       */
            &unmute_cmd,                           /* payload      */
            sizeof(unmute_cmd) );                  /* payload_size */

} /* ECALL_APP_UNMUTE_RX_OUT */

#else /* Legacy Vocoder */
/*===========================================================================

                L e g a c y   V o c o d e r   F u n c t i o n

===========================================================================*/

/*==========================================================================

  FUNCTION ECALL_RX_INPUT_PCM

  DESCRIPTION
    Pulls a PCM buffer off of the queue and copies it into the Vocoder's
    buffer for encoding and transmission.  Called in interrupt context.

  DEPENDENCIES
    Legacy Vocoder

  RETURN VALUE
    None

  SIDE EFFECTS
    The vocoder input takes 14 bit linear pcm samples and the 15th,16th bits
    are the sign extension bits. we need to downscale two bits to meet the vocoder format

===========================================================================*/
static voc_pcm_data_status_type ecall_rx_input_pcm
(
  uint16  *pcm
)
{
  if(pcm != NULL)  /*lint !e774 */
  {
    memset(pcm, 0, ECALL_PCM_LENGTH_BYTES);

    ECALL_MSG_0( ECALL_MED, "ecall_rx_input_pcm  sending silence to speaker" );
    return VOC_PCM_DATA_STATUS_AVAILABLE;
  }
  ECALL_MSG_0( ECALL_HIGH, "ERROR: NULL passed in ecall_rx_input_pcm" );
  return VOC_PCM_DATA_STATUS_UNAVAILABLE;

} /* ECALL_RX_INPUT_PCM() */

/*==========================================================================
  FUNCTION ECALL_APP_MUTE_RX_OUT

  DESCRIPTION
    This function will mute the path from vocoder RX output to loudspeaker.

  DEPENDENCIES
    Legacy Vocoder

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_app_mute_rx_out
(
  void
)
{
  voc_status_type voc_status = VOC_CMD_FAILED_S;

  ECALL_MSG_0( ECALL_MED, "ecall_app mute vocoder rx out" );
  if ((voc_status = voc_register_pcm_input_client(
                                          VOC_PCM_INTERFACE_RX_INPUT_TO_PP ,
                                          ecall_rx_input_pcm)) != VOC_DONE_S)
  {
    ECALL_MSG_1( ECALL_MED, "voc_register_pcm_input_client returned ERROR %d",
                             voc_status );
  }

} /* ECALL_APP_MUTE_RX_OUT */

/*==========================================================================
  FUNCTION ECALL_APP_UNMUTE_RX_OUT

  DESCRIPTION
    This function will unmute the path from vocoder RX output to loudspeaker.

  DEPENDENCIES
    Legacy Vocoder

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_app_unmute_rx_out
(
  void
)
{
  voc_status_type voc_status = VOC_CMD_FAILED_S;

  ECALL_MSG_0( ECALL_MED, "ecall_app unmute vocoder rx out" );

  if ((voc_status = voc_register_pcm_input_client(VOC_PCM_INTERFACE_RX_INPUT_TO_PP, NULL)) != VOC_DONE_S)
  {
    ECALL_MSG_1( ECALL_MED, "voc_register_pcm_input_client returned ERROR %d",
                             voc_status );
  }

} /* ECALL_APP_UNMUTE_RX_OUT */

#endif /* FEATURE_ECALL_HAS_VOICE_A */

#else

void ecall_app_task_dummy_func(void);

#endif //End Featurisation

