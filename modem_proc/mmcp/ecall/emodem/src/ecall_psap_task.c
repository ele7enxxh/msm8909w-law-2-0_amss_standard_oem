/*===========================================================================

                         E C A L L  _  P S A P  _  T A S K . C

DESCRIPTION
  eCall task source file

REFERENCES
  

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 
 
Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/src/ecall_psap_task.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $ 
===========================================================================*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_ECALL_PSAP

#include "rex.h"
#include "err.h"
#include "msg.h"
#include "queue.h"
#include "cm.h"
#include "timer.h"
#include "ecall_psap_task.h"
#include "ecall_modem_psap.h"
#include "ecall_efs.h"

#ifndef FEATURE_ECALL_LCU45
#include "npa.h"
#endif /* FEATURE_ECALL_LCU45 */

#ifdef FEATURE_ECALL_HAS_V1_HEADER
#include "task.h"
#endif /* FEATURE_ECALL_HAS_V1_HEADER */

#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif /* FEATURE_MODEM_RCINIT */
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
#undef    DOG_ECALL_PSAP_RPT
#define   DOG_ECALL_PSAP_RPT      ecall_psap_dog_rpt_var
static    dog_report_type         ecall_psap_dog_rpt_var  = 0;     /* Initial Safety */
#else /* Legacy Dog */
/* Dynamic Dog Interface */
#ifdef DOG_DYNAMIC_API
  #undef  DOG_ECALL_PSAP_RPT
  #define DOG_ECALL_PSAP_RPT      ecall_psap_dog_rpt_var
  static dog_report_type          ecall_psap_dog_rpt_var  = 0;     /* Initial Safety */

  #undef  DOG_ECALL_PSAP_RPT_TIME
  #define DOG_ECALL_PSAP_RPT_TIME  ecall_psap_dog_rpt_time_var
  static uint32                    ecall_psap_dog_rpt_time_var   = 0xffff; /* Initial Safety */
#endif   /* DOG_DYNAMIC_API */
#endif /* FEATURE_ECALL_HAS_DOG_HB */

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

#define ECALL_CMD_BUF_COUNT     32

/*===========================================================================

                         I N T E R N A L    D A T A

===========================================================================*/

/* Command queue */
static ecall_cmd_type   ecall_cmd_buf[ECALL_CMD_BUF_COUNT];
static q_type           ecall_cmd_free_q;
static q_type           ecall_cmd_q;

/* Define a variable to hold the ECALL_PSAP task TCB.  This is done for
   a several reasons:
    1) The return value of rex_self() never changes at runtime, so
       there is nothing gained by repeatedly fetching it.
    2) Directly using ecall_psap_tcb requires that 'task.h' be included.
       It is now prohibited to include 'task.h'.
    3) The use of rex_self() cannot be used in any context where
       the ecall_psap_tcb is specifically required (such as when a command
       is placed into a queue and the task signal is set).
    4) A task's tcb is not available if the task is not started.
       Worse, signaling a task that isn't running causes an exception.
    5) The variable allows access to the correct tcb in case 3 and
       provides state to use in determining if the task is running.
*/
/* To be initialized to rex_self() when task is running */
static rex_tcb_type *ecall_psap_myself = NULL;

static cm_client_id_type  ecall_cm_client_id;
static cm_call_id_type ecall_incomCall_id;

LOCAL struct
{
  #ifndef FEATURE_ECALL_HAS_DOG_HB /* Legacy Dog */
  rex_timer_type ecall_psap_rpt_timer;            /* Watchdog timer */
  #endif /* Legacy Dog */
  rex_timer_type auto_answer_timer;
  rex_timer_type restart_timer;
  ecall_psap_tx_mode_type  psap_tx_mode; /* PUSH or PULL mode */
  boolean force_pull_mode;
  cm_ans_params_s_type    answer_params;
  #ifndef FEATURE_ECALL_LCU45
  npa_client_handle  ecall_cpu_hdl;               /* Vote for Mips    */
  #endif /* FEATURE_ECALL_LCU45 */
} ecall_psap;


/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*==========================================================================

  FUNCTION ECALL_PROCESS_CMD

  DESCRIPTION
    Processes commands off of the command queue, and returns the command
    buffers to the free queue.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

============================================================================*/
static void ecall_process_cmd(void)
{
  ecall_cmd_type *cmd_ptr;
  static uint32 count = 0;
  uint8 num_processed = 0;

  while((cmd_ptr = (ecall_cmd_type *) q_get(&ecall_cmd_q)) != NULL)
  {
    switch(cmd_ptr->cmd_id)
    {
      case ECALL_CMD_PROCESS_RX_PCM:
        ECALL_MSG_1( ECALL_MED, "[bd] began processing frame %d...",
                                 ++count );
        ecall_psap_process(cmd_ptr->data.pcm, sizeof(cmd_ptr->data.pcm));
        ECALL_MSG_1( ECALL_MED, "[bd] finished processing PSAP frame %d",
                                 count );
        num_processed++;
        break;
     
     case ECALL_CMD_PROCESS_RESTART_TIME:
        ECALL_MSG_1( ECALL_HIGH, "ECALL_CMD_PROCESS restart_time  %d with PULL mode",
                                  cmd_ptr->data.restart_time );
        ecall_psap.psap_tx_mode = PULL_MODE;
        (void)rex_set_timer( &ecall_psap.restart_timer, cmd_ptr->data.restart_time);
        break;

     case ECALL_CMD_PROCESS_START_PSAP:
       (void) ecall_voc_register();
       if (ecall_psap.psap_tx_mode == PULL_MODE || ecall_psap.force_pull_mode == TRUE)
       {
         ECALL_MSG_0( ECALL_HIGH, " PULL_MODE, calling ecall_psap_send_start..." );
         ecall_psap_send_start(PULL_MODE);  
       }
       else
       {
         ECALL_MSG_0( ECALL_HIGH, " ELSE, PUSH_MODE, calling ecall_psap_send_start..." );
         ecall_psap_send_start(PUSH_MODE);  
       }
       break;

      case ECALL_CMD_CALL_ORIG:
        ecall_psap.psap_tx_mode = PULL_MODE;
        break;

      case ECALL_CMD_INCOMING_CALL:
        ecall_psap.psap_tx_mode = PUSH_MODE;
        ecall_incomCall_id = cmd_ptr->data.call_id;     
        (void)rex_set_timer( &ecall_psap.auto_answer_timer, 1000 );
        break;

      case ECALL_CMD_END_CALL:
        (void)rex_clr_sigs( ecall_psap_myself, ECALL_PSAP_RESTART_TIMER_SIG );
        ecall_psap_session_stop(FALSE);
        break;

      case ECALL_CMD_APR_PACKET:
        #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
        ecall_process_apr_packet(cmd_ptr->data.apr_packet);
        #else
        ECALL_MSG_0( ECALL_ERROR, "VOICE_A or VOICE_B not defined, but receives ECALL_CMD_APR_PACKET" );
        #endif /* FEATURE_ECALL_HAS_VOICE_A || EFATURE_ECALL_HAS_VOICE_B */
        break;

      default:
        ECALL_MSG_1( ECALL_ERROR, "Unknown command %d passed to eCall task",
                                   cmd_ptr->cmd_id );
    }

    q_put(&ecall_cmd_free_q, &cmd_ptr->link);
  }

  if (num_processed > 1)
  {
    ECALL_MSG_0( ECALL_ERROR, "[bd] WARNING: processed > 1 PCM frame, could lose sync" );
  }
} /* ecall_process_cmd() */


/*==========================================================================

  FUNCTION ECALL_CALL_EVENT_CB

  DESCRIPTION
    Gets the callbacks from CM and takes the appropriate Action

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

============================================================================*/
static void ecall_call_event_cb
(
  cm_call_event_e_type           call_event,
  const cm_mm_call_info_s_type * call_info_ptr
)
{
  ecall_cmd_type *cmd_ptr;
 
  if((call_info_ptr->call_type == CM_CALL_TYPE_VOICE) || (call_info_ptr->call_type == CM_CALL_TYPE_VOICE))
  {
    switch (call_event)
    {
      case CM_CALL_EVENT_ORIG:
        ECALL_MSG_0( ECALL_HIGH, "Received CM_CALL_EVENT_ORIG" );
        cmd_ptr = ecall_get_cmd_buf();
        if (cmd_ptr == NULL)
        {
          ECALL_MSG_0( ECALL_ERROR, "ecall_get_cmd_buf ran out of command buffers in ecall_call_event_cb()" );
          return;
        }
        
        /* Post a command to eCall task */
        cmd_ptr->cmd_id = ECALL_CMD_CALL_ORIG;
        ecall_put_cmd(cmd_ptr);        
        break;   
    
      /* Incoming call, start ringing and autoanswer timer */
      case CM_CALL_EVENT_INCOM:
        ECALL_MSG_0( ECALL_HIGH, "Received CM_CALL_EVENT_INCOM" );
        cmd_ptr = ecall_get_cmd_buf();
        if (cmd_ptr == NULL)
        {
          ECALL_MSG_0( ECALL_ERROR, "ecall_get_cmd_buf ran out of command buffers in ecall_call_event_cb()" );
          return;
        }
        
        /* Post a command to eCall task */
        cmd_ptr->data.call_id = call_info_ptr->call_id;
        cmd_ptr->cmd_id = ECALL_CMD_INCOMING_CALL;
        ecall_put_cmd(cmd_ptr);
        break;
  
      case CM_CALL_EVENT_CONNECT:
        ECALL_MSG_0( ECALL_HIGH, "Received CM_CALL_EVENT_CONNECT" );
        #ifndef FEATURE_ECALL_LCU45
        if(ecall_psap.ecall_cpu_hdl != NULL)
        {
          ECALL_MSG_0( ECALL_MED, "Request for MAX NPA" );
          /* Issue a work request of MAX MIPS to the CPU */
          npa_issue_required_request( ecall_psap.ecall_cpu_hdl, NPA_MAX_STATE );
        }
        else
        {
          ECALL_MSG_0( ECALL_ERROR, "couldn't request for ECALL_PSAP mips " );
        }
        #endif /* FEATURE_ECALL_LCU45 */
        /* Read the efs configuration file when you are answering the call */ 
        ecall_psap_efs_read();
        ecall_psap.force_pull_mode = ecall_psap_get_force_pull_mode();
        ECALL_MSG_1( ECALL_MED, "ecall_psap.force_pull_mode = %d",
                                 ecall_psap.force_pull_mode );

        cmd_ptr = ecall_get_cmd_buf();
        if (cmd_ptr == NULL)
        {
          ECALL_MSG_0( ECALL_ERROR, "ecall_get_cmd_buf ran out of command buffers in ecall_call_event_cb()" );
          return;
        }
        
        /* Post a command to the eCall task */
        cmd_ptr->cmd_id   = ECALL_CMD_PROCESS_START_PSAP;
        ecall_put_cmd(cmd_ptr);
        break;

      case CM_CALL_EVENT_ANSWER:
        ECALL_MSG_0( ECALL_HIGH, "Received CM_CALL_EVENT_ANSWER" );
        break;
  
      case CM_CALL_EVENT_END:
      case CM_CALL_EVENT_END_REQ:
        ECALL_MSG_0( ECALL_HIGH, "Received CM_CALL_EVENT_END_REQ or CM_CALL_EVENT_END" );
        #ifndef FEATURE_ECALL_LCU45
        if(ecall_psap.ecall_cpu_hdl != NULL)
        {
          ECALL_MSG_0( ECALL_MED, "Cancel NPA request" );
          /* cancel work request */
          npa_cancel_request( ecall_psap.ecall_cpu_hdl ); 
        }
        #endif /* FEATURE_ECALL_LCU45 */
        cmd_ptr = ecall_get_cmd_buf();
        if (cmd_ptr == NULL)
        {
          ECALL_MSG_0( ECALL_ERROR, "ecall_get_cmd_buf ran out of command buffers in ecall_call_event_cb()" );
          return;
        }
        
        /* Post a command to eCall task */
        cmd_ptr->cmd_id = ECALL_CMD_END_CALL;
        ecall_put_cmd(cmd_ptr);
        break;
  
      default:
        ECALL_MSG_1( ECALL_HIGH, "Received default  %d", call_event );
        break;
    }
  }
}

#ifndef FEATURE_ECALL_LCU45
/*==========================================================================
  FUNCTION    ECALL_PSAP_NPA_CB   

  DESCRIPTION
    callback function called by npa

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    

===========================================================================*/
void ecall_psap_npa_cb
(
  void          *context, 
  unsigned int  event_type, 
  void          *data,  
  unsigned int  data_size
)
{
  /* create a handle to the /core/cpu resource */
  ecall_psap.ecall_cpu_hdl = npa_create_sync_client("/core/cpu", "ECALL_PSAP", NPA_CLIENT_REQUIRED );

  if(ecall_psap.ecall_cpu_hdl  == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ECALL_PSAP NPA create client failed" );
  }
}
#endif /* FEATURE_ECALL_LCU45 */

/*==========================================================================

  FUNCTION ECALL_CALL_ERR_CB

  DESCRIPTION
    Processes commands off of the command queue, and returns the command
    buffers to the free queue.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

============================================================================*/
/*lint -esym(715,data_ptr) -e818*/
void ecall_call_err_cb
(
  void *data_ptr,
  cm_call_cmd_e_type call_cmd,
  cm_call_cmd_err_e_type call_cmd_err
)
{
  ECALL_MSG_2( ECALL_HIGH, "Ecall - ecall_call_err_cb Call Command Callback returned %d for command %d",
                            call_cmd_err,
                            call_cmd );
}

/*===========================================================================

FUNCTION ECALL_PSAP_WAIT

DESCRIPTION
  This function suspends execution of the ECALL_PSAP_WAIT task until one (or
  more) signals from the requested signal mask is set.

  Watchdog kicking is performed in this function.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask returned by rex_wait().

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type  ecall_psap_wait
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
    /*lint --e{641}*/
    set_signals = rex_wait( requested_signal_mask | ECALL_PSAP_DOG_RPT_TIMER_SIG );

    if( (set_signals & ECALL_PSAP_DOG_RPT_TIMER_SIG) != 0 )
    {
      #ifdef FEATURE_ECALL_HAS_DOG_HB
       /*---------------------------------------------------------------------
      The watchdog needs a kick. Clear the signal and report back 
      to the watchdog by Dog HB.
      ---------------------------------------------------------------------*/
      (void)rex_clr_sigs( ecall_psap_myself, ECALL_PSAP_DOG_RPT_TIMER_SIG );
      /* Report to the Dog HB */
      dog_hb_report(DOG_ECALL_PSAP_RPT);

      #else /* Legacy Dog */
      /*---------------------------------------------------------------------
      The watchdog needs a kick. Report back to the watchdog, and then
      restart the timer.
      ---------------------------------------------------------------------*/
      #ifdef DOG_DYNAMIC_API
      if( ecall_psap_dog_rpt_time_var > 0)
      #endif
      {
        (void)rex_clr_sigs( ecall_psap_myself, ECALL_PSAP_DOG_RPT_TIMER_SIG );
        dog_report( DOG_ECALL_PSAP_RPT );
        (void)rex_set_timer( &ecall_psap.ecall_psap_rpt_timer, DOG_ECALL_PSAP_RPT_TIME); 
      }
      #endif /* FEATURE_ECALL_HAS_DOG_HB*/

      /*---------------------------------------------------------------------
      Processed the DOG_RPT_TIMER_SIG so clear it off in set_signals. 
      ---------------------------------------------------------------------*/
      set_signals &= (~ECALL_PSAP_DOG_RPT_TIMER_SIG);
    }
  } while( (set_signals & requested_signal_mask) == 0 );

  return( set_signals );

} /* ecall_psap_wait() */



/*==========================================================================

  FUNCTION ECALL_GET_CMD_BUF

  DESCRIPTION
    Returns a command pointer off of the free queue, or NULL on failure.

  DEPENDENCIES
    None.

  RETURN VALUE
    Command pointer or NULL

  SIDE EFFECTS
    None

===========================================================================*/
ecall_cmd_type *ecall_get_cmd_buf(void)
{
  ecall_cmd_type *cmd_ptr;
  if ((cmd_ptr = q_get(&ecall_cmd_free_q)) == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "No items on eCall task free cmd q" );
  }
  return cmd_ptr;
} /* ecall_get_cmd_buf() */


/*==========================================================================

  FUNCTION ECALL_PUT_CMD

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
void ecall_put_cmd
(
  ecall_cmd_type  *cmd_ptr
)
{
  q_put(&ecall_cmd_q, &cmd_ptr->link);
  (void) rex_set_sigs( ecall_psap_myself, ECALL_PSAP_CMD_Q_SIG );
} /* ecall_put_cmd() */

/*===========================================================================

FUNCTION ecall_psap_task_offline

DESCRIPTION
  This procedure performs the transition to "offline" for the ECALL_IVS task.

DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  ecall_psap_tcb

===========================================================================*/
LOCAL void ecall_psap_task_offline (void)
{

#ifndef FEATURE_MODEM_RCINIT_PHASE2
  /* Process task offline procedure from task controller. */
  tmc_task_offline();
#endif

} /* end ecall_psap_task_offline */


/*===========================================================================

FUNCTION ecall_psap_task_shutdown

DESCRIPTION
  This procedure performs shutdown processing for the VS task.

DEPENDENCIES
  FEATURE_ECALL_IVS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  ecall_psap_tcb

===========================================================================*/
LOCAL void ecall_psap_task_shutdown (void)
{

#ifndef FEATURE_MODEM_RCINIT_PHASE2
  /* Process task stop procedure from task controller. */
  tmc_task_stop();
#endif

} /* end ecall_psap_task_shutdown */

/*==========================================================================

  FUNCTION    ECALL_PSAP_TASK_INIT_BEFORE_TASK_START

  DESCRIPTION
     Initializes queues and defualt values for the global data.

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_task_init_before_task_start
(
  void
)
{
  uint8 i;

  ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: ecall_psap_task_init_before_task_start() entered" );
  
  /* Initialize command queues */
  (void) q_init(&ecall_cmd_q);
  (void) q_init(&ecall_cmd_free_q);
  for (i = 0; i < ECALL_CMD_BUF_COUNT; i++)
  {
    (void) q_link(&ecall_cmd_buf[i], &ecall_cmd_buf[i].link);
    q_put(&ecall_cmd_free_q, &ecall_cmd_buf[i].link);
  }
  /*-------------------------------------------------------------------------
   Initialise the pcm command queues
  -------------------------------------------------------------------------*/
  ecall_modem_psap_init();

  ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: ecall_psap_init_before_task_start() returning" );
} /* ECALL_PSAP_TASK_INIT_BEFORE_TASK_START */



/*==========================================================================

  FUNCTION    ECALL_PSAP_TASK_INIT

  DESCRIPTION
     Initializes queues and defualt values for the global data.

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_task_init
(
  void
)
{
  ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: ecall_psap_task_init() entered" );

  /*-------------------------------------------------------------------------
   Wait for the task start signal from task controller.
  -------------------------------------------------------------------------*/
  #ifdef FEATURE_MODEM_RCINIT
    /* Inform RCInit that ECALL_PSAP initialization phase is complete */
    ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: Calling rcinit_handshake_startup()" );
    rcinit_handshake_startup();
  #else /* FEATURE_MODEM_RCINIT */
    /* If not using RCInit framework, initialize ECALL_PSAP queue and clients. */
    /* Otherwise, RCInit has already called this function in the init step. */
    ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: Calling ecall_psap_task_init_before_task_start() from ecall_psap_task_init" );
    ecall_psap_task_init_before_task_start();

  #ifndef FEATURE_MODEM_RCINIT_PHASE2
    /* Inform Main Control task that ECALL_PSAP task has finished initialization */
    tmc_task_start();
  #endif /* FEATURE_MODEM_RCINIT_PHASE2 */
  #endif /* FEATURE_MODEM_RCINIT */

   
  /* Initialize timers */
  rex_def_timer( &ecall_psap.auto_answer_timer, ecall_psap_myself, ECALL_PSAP_CM_AUTO_ANSWER_SIG );
  rex_def_timer( &ecall_psap.restart_timer, ecall_psap_myself, ECALL_PSAP_RESTART_TIMER_SIG );

  #ifdef FEATURE_ECALL_HAS_DOG_HB
    /* Register with DOG HB new API */
    ecall_psap_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) ECALL_PSAP_DOG_RPT_TIMER_SIG);
  #else /* Legacy Dog */
  #ifdef DOG_DYNAMIC_API
    ecall_psap_dog_rpt_var      = dog_register( ecall_psap_myself, \
                                                DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE);
    ecall_psap_dog_rpt_time_var = dog_get_report_period( ecall_psap_dog_rpt_var );
  
  /* If the return value from dog_get_report_period() was zero then we MUST
   ** NOT report to dog directly, as DOG will monitor us in a different manner.
   ** Hence we won't initialize or start any timer.
   */
  if ( ecall_psap_dog_rpt_time_var > 0 )
  #endif 
  {
    rex_def_timer( &ecall_psap.ecall_psap_rpt_timer, ecall_psap_myself, ECALL_PSAP_DOG_RPT_TIMER_SIG );
    (void)rex_set_timer( &ecall_psap.ecall_psap_rpt_timer, DOG_ECALL_PSAP_RPT_TIME );
  }
  #endif /* FEATURE_ECALL_HAS_DOG_HB */
    
  (void) rex_clr_sigs( ecall_psap_myself, (rex_sigs_type)~0 );

  
  /* CM */
  ECALL_MSG_0( ECALL_HIGH, "Ecall- calling cm_client_init...." );
  (void)cm_client_init(CM_CLIENT_TYPE_UI, &ecall_cm_client_id);

  ECALL_MSG_0( ECALL_HIGH, "Ecall- calling cm_mm_client_call_reg...." );

  if( CM_CLIENT_OK != 
          cm_mm_client_call_reg( ecall_cm_client_id,
                                 ecall_call_event_cb,
                                 CM_CLIENT_EVENT_REG,
                                 CM_CALL_EVENT_ORIG,
                                 CM_CALL_EVENT_EXIT_TC,
                                 NULL ) )
  {
    ECALL_MSG_0( ECALL_ERROR, "[bd] CM client PH event reg failed" );
  }

  (void)cm_client_act(ecall_cm_client_id);

  #ifndef FEATURE_ECALL_LCU45
  npa_resource_available_cb( "/core/cpu", ecall_psap_npa_cb, NULL );
  #endif /* FEATURE_ECALL_LCU45 */

  ecall_psap.psap_tx_mode = PUSH_MODE; /* Default set to PUSH mode */
  ecall_psap.force_pull_mode = FALSE;  /* Default set to PUSH mode with FALSE */

  ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: ecall_psap_task_init() returning" );
} /* ECALL_PSAP_TASK_INIT */



/*===========================================================================

                          EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*==========================================================================

  FUNCTION ECALL_PSAP_TASK

  DESCRIPTION
    

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
/*lint -esym(715,ignored) */
void ecall_psap_task
(
 dword ignored
)
{
  rex_sigs_type set_sig;
  rex_sigs_type requested_sigs;
  ecall_cmd_type *cmd_ptr;
  /*lint --e{641}*/

  ecall_psap_myself = rex_self();    /* Will never change after this */

  /*-------------------------------------------------------------------------
   Do task initialization. The init function performs all the task-level
   initialization, e.g. define message queues.
  -------------------------------------------------------------------------*/
  ecall_psap_task_init();

  /* Start the queue processing */
  (void) rex_set_sigs( ecall_psap_myself, ECALL_PSAP_CMD_Q_SIG );

  /*lint -e655*/
  requested_sigs =
          ( ECALL_PSAP_CMD_Q_SIG )           |
          ( ECALL_PSAP_CM_AUTO_ANSWER_SIG )  |
          ( ECALL_PSAP_RESTART_TIMER_SIG )      |
          ( ECALL_PSAP_TASK_OFFLINE_SIG )    |
          ( ECALL_PSAP_TASK_STOP_SIG);
   /*lint +e655*/

   /* Main task loop */
  for (;;)
  {
    /* Wait in the infinite loop so the task would not die */
    set_sig = ecall_psap_wait( requested_sigs ); 

    if((set_sig & ECALL_PSAP_CMD_Q_SIG )!= 0 )
    {
      /*-------------------------------------------------------------------------
                        Clear the Signal  
      -------------------------------------------------------------------------*/
      (void)rex_clr_sigs( ecall_psap_myself, ECALL_PSAP_CMD_Q_SIG ); 
      ecall_process_cmd();
    }

    if((set_sig & ECALL_PSAP_RESTART_TIMER_SIG )!= 0)
    {
      /* Start over the PSAP, end the previous transfer if still exists */
       (void)rex_clr_sigs( ecall_psap_myself, ECALL_PSAP_RESTART_TIMER_SIG ); 
       ECALL_MSG_0( ECALL_HIGH, "Restart PSAP timer expired, start over" );
       ecall_modem_psap_flush_queue();
       cmd_ptr = ecall_get_cmd_buf();
       if (cmd_ptr != NULL)
       {
         /* Post a command to the eCall task */
         cmd_ptr->cmd_id   = ECALL_CMD_PROCESS_START_PSAP;
         ecall_put_cmd(cmd_ptr);
       }
       else
       {
         ECALL_MSG_0( ECALL_ERROR, "Dropped ECALL_CMD_PROCESS_START_PSAP command; ran out of buffers" );
       }
    }

    if ((set_sig & ECALL_PSAP_TASK_STOP_SIG) != 0) 
    {
      (void)rex_clr_sigs( ecall_psap_myself, ECALL_PSAP_TASK_STOP_SIG ); 
      ecall_psap_task_shutdown ();
    }

    if ((set_sig & ECALL_PSAP_TASK_OFFLINE_SIG) != 0) 
    {
      (void)rex_clr_sigs( ecall_psap_myself, ECALL_PSAP_TASK_OFFLINE_SIG );
      ecall_psap_task_offline ();
    }

    if((set_sig & ECALL_PSAP_CM_AUTO_ANSWER_SIG)!=0)
    {
      /*-------------------------------------------------------------------------
                        Clear the Signal  
      -------------------------------------------------------------------------*/
      ECALL_MSG_0( ECALL_HIGH, "Ecall- Rxed ECALL_PSAP_CM_AUTO_ANSWER_SIG" );
      (void)rex_clr_sigs( ecall_psap_myself, ECALL_PSAP_CM_AUTO_ANSWER_SIG ); 

      /* Auto Answer */
      ECALL_MSG_0( ECALL_HIGH, "Ecall- memset on answer_params...." );

      memset( &ecall_psap.answer_params, CM_CALL_CMD_PARAM_DEFAULT_VALUE, sizeof( cm_ans_params_s_type));
      ecall_psap.answer_params.info_type = CM_CALL_MODE_INFO_GW_CS;
      ecall_psap.answer_params.ans_params.gw_cs_ans.accept = TRUE;
      ecall_psap.answer_params.ans_params.gw_cs_ans.call_type = CM_CALL_TYPE_VOICE;

      if (!cm_mm_call_cmd_answer(ecall_call_err_cb, NULL, ecall_cm_client_id, ecall_incomCall_id, &ecall_psap.answer_params))
      {
        ECALL_MSG_0( ECALL_HIGH, "Ecall- Could not auto answer incoming call" );
      }

      ECALL_MSG_0( ECALL_HIGH, "Ecall- after cm_mm_call_cmd_answer, calling ecall_psap_send_start..." );
      
    }
  }
} /* ECALL_PSAP_TASK() */
#else

  void ecall_psap_task_dummy_func(void);

#endif /* FEATURE_ECALL_PSAP*/


