/*===========================================================================
                        S M    T A S K  

DESCRIPTION

   This file contains the task entry point for SM task. It implements the 
   SM task initialization, including initialization of various procedures.
   and setting of the SM state. SM task enters a forever loop after it is
   initialized and waits for the REX signals.


EXTERNALIZED FUNCTIONS   
    void sm_task()
    --------------
      It is the main entry point for SM Task.

    void sm_put_cmd(sm_cmd_type *cmd_ptr)
    -------------------------------------
      Function used by other entities to put a command in SM command Queue.

    sm_cmd_type *sm_get_cmd_buf(void)
    ---------------------------------
      Function used by other entities to get SM command type.

    void sm_clear_pdp_timer (sm_timer_T timer_type)
    --------------------------------------------
      This function clears a specified PDP timer.        
        
              
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/smtask.c_v   1.14   18 Jul 2002 15:44:26   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/smtask.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/01/12   am      Back off timer code
04/02/12   us      Assignig sm_tcb_ptr  in sm_task() instead of sm_init() 
02/15/12   am      Changes for activating two same PDP address p-PDP simultaneously by UE.
11/29/11   am      Updating rab state to inactive when modify pending timer expires
07/13/11   am      Trigger PDP context deactivation when a PDP Modification message 
                   is received with a new PDP address
03/04/08   ab     Sending deact cnf  to CM when MS is not able to send activate reject to network
                          after max retrys to establish signal connection
11/10/06   ss      Setting cause as RAB Setup Failure when PDP Modify Max Timeout 
                         happens for MO PDP Modify procedure not requested by CM
08/18/06   rk       Added Extended TI Feature Code
09/04/06   rk      Fixed UE to re-start T3390 timer, if the NAS OTA message is in UDT's internal queue.
08/04/06   ss      Implemented changes for ROHC and IP header compression.
07/17/06   ss       Delaying Local deactivation for 2 secs to avoid RAB setup failure.
06/07/06   ss       Handling RAB Timer expiry while WtoG CCO is in progress.
05/08/06   ss       Blocking OTA for Deact Req when RRC has buffered it.
01/17/06   ss       Not promoting secondary PDP when teardown is set.
01/04/06   kvk     Defined new boolean variable ps_signalling_conn_state to store
                   the status of the PS siganlling connection
 09/13/05  kvk     MO PDP Modify supported. Now handling the Application initiated
                   PDP Modification for QoS and TFT.
09/08/05   kvk     GMMSM_CLOSE_SESSION_REQ is posted after ESTABLISH PENDING timer 
                   expires MAX times to trigger PS signalling con rel when PDP Deactivation
                   is triggered.
03/31/05   kvk     Clear the RAB state along with PDP state
02/16/05   ss       Added send_gmm_close_session_req() for closure of RRC connection 
                    at 5th expiry of T3390.
02/07/05   kvk     Added handling for SNDCP guard timer expiry which starts
                   during GPRS rlease ind by GMM. 
01/21/05   kvk     Removed the flag inter_system_change_pending as we are no longer
                   waiting for SNDCP syncup during InterRAT.
01/12/04   kvk     Added check for checking pdp con state when ESTABLISH PENDING
                   expires to process any pending requests.
12/06/04   kvk     Added new function GET_NSAPI() to get NSAPI from connection id.
10/18/04   kvk     MT PDP support added.
09/28/04   kvk     Handling T3390 and ESTABLISH Pending timer expiry for SEC PDP
                   in GSM/GPRS.
09/21/04   kvk     Fixed passing the correct CM responses by SM when timer T3390 or
                   TIMER_ESTABLISH_PENDING max times out.
09/02/04   kvk     TIMER_ESTABLISH_PENDING is triggered instead of T3390/T3385 when PDP
                   deactivation is triggered. tear down indicator flag is processed
                   in case T3390 max timeout.
06/28/04   kvk     When TIMER_ESTABLISH_PENDING timer expires pdp_con_state is checked
                   instead of pdp_state.
06/08/04   kvk     PDP con state is reset along with PDP state.
06/03/04   kvk     max_timer_est_cnt is reset to zero evry time new PDP context comes when 
                   TIMER_ESTABLISH_PENDING is running. Triggering the multiple instances of 
                   TIMER_ESTABLISH_PENDING is controlled by timer_est_pen_flag.
05/13/04   kvk     Modified the TIMER_ESTABLISH_PENDING timer count MAX_EST_TIMER_CNT 
03/31/04   ks      Added check to make sure PDP context is INACTIVE before 
                   calling the function sm_continue_pdp_activation() when
                   timer TIMER_ESTABLISH_PENDING expires.
02/28/04   ks      Added support for max_est_timer_count, timer for maximum
                   Signaling connection requests to set up GMM connection.
02/17/04   ks      Now passing the correct NSAPI value in send_rabmcm_cmd()
                   removed the sm_sndcp.h as it is not used.
11/24/03   ks      Added support for TIMER_ESTABLISH_PENDING for handling the
                   CMM connection before starting the PDP activation request.
09/02/03   mks     Featurized the call to sm_send_sndcp_cmd () in  
                   sm_proc_timer_expired ().
08/08/03   mks     Incorporated Nigel's changes for deactivating the NSAPI in 
                   SNDCP when SM max outs on the deactivation attempts.
07/25/03   ks      If RAB is not established during PDP modification procedure
                   removed sending deactivate notification to RABM.
06/25/03   ks      Removed the TIMER_RABM_RESP_PENDING handling for GPRS.
03/18/03   mks     Added a boolean flag inter_system_change_pending to indicate
                   if inter-RAT change is pending.
02/24/03   ks      Fixed LINT warnings.
01/16/03   ks      Replaced mm_abort_rr() with send_gmm_close_session_req().
12/17/02   ks      Passing the PS domain ID in the mm_abort_rr function.
12/02/02   ks      Added processing for LLC_MT_DEACTIVATE_CNF_PENDING.
07/18/02   ks      Renamed FEATURE_GPRS with FEATURE_GSM_GPRS
07/01/02   ks      Added support for GPRS
06/13/02   ks      Removed the LOCAL definition for max_timer_count variable
06/13/02   ks      Now aborting the RRC connection after an abnormal 
                   termination of PDP context procedure.
04/22/02   ks      Added support for the PS data call preservation. Added
                   timer TIMER_RAB_EST_PENDING for RAB re-establish procedure.
03/13/02   ATM     GS modification per code review
11/05/01   ks      Sending the PDP_CONTEXT_DEACTIVATE_CNF to SM when the
                   RAB setup timer expires during deactivation procedure.
08/24/01   ks      Added a MACRO MAX_TIMER_CNT, moved the sm_clear_pdp_timer 
                   and sm_set_pdp_timer functions to smutils.c file. 
08/01/01   ks      Added support for Watch dog report in SM task.
06/29/01   ks      added a flag sig_connection_available to identify the 
                   availability of the RRC Signaling connection.
06/04/01   ks      Added a prefix 'SM' to all the SM 'message_set' fields.
05/23/01   ks      Removed the sm_clear_pdp_timer() as it is defined in sm.h.
                   Initializing max_timer_count, Added else condition to check
                   for unknown timer expiry in sm_proc_timer_expired().
05/17/01   ks      Removed the external function declarations and added in
                   sm.h file.
02/03/01   ks      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "mmcp_variation.h"
#include "comdef.h"
#include "rex.h"
#include "dog_hb_rex.h"
#include "smtask.h"
#include "smtask_v.h"
#include "rabmtask_v.h"
#include "ms.h"
#include "bit.h"
#include "memory.h"
#include "sm_v.h"
#include "gs_v.h"
#include "smtimers.h"
#include "mcfg_fs.h"
#ifdef FEATURE_LTE
#include "esm_v.h"
#include "sm_multimode_common.h"
#endif /*FEATURE_LTE*/

/* Include the Task Controller file */
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif

#ifdef FEATURE_GSM_GPRS
#include "gsmsnif.h"
#endif
#include "cfa_v.h"

#ifdef FEATURE_MBMS
#error code not present
#endif
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include "mm_v.h"
#include "mm.h"
#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif
#include "fs_lib.h"
#include <stringl/stringl.h>

#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#endif

extern boolean is_ps_umts_mode( sys_modem_as_id_e_type as_id );
extern boolean is_ps_gsm_mode( sys_modem_as_id_e_type as_id );

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
void sm_continue_resend_pdp_msg (pdp_context_info *pdp);
#endif

#ifdef FEATURE_DUAL_SIM
#define is_umts_mode() is_ps_umts_mode((sys_modem_as_id_e_type)sm_as_id)
#define is_gsm_mode()  is_ps_gsm_mode((sys_modem_as_id_e_type)sm_as_id)
#else
#define is_umts_mode() is_ps_umts_mode(0)
#define is_gsm_mode()  is_ps_gsm_mode(0)
#endif

extern void rabm_init_before_task_start(void);
extern void sm_pdp_deactivate (byte con_id, 
                               smreg_pdp_deactivate_req_T *pdp_deactivate);


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define SM_PD             0x0A
#define MAX_EST_TIMER_CNT (ACTIVE_PENDING_TIME / ESTABLISH_PENDING_TIME)

  word sm_pd;

/* ----------------------------
** Define watchdog report handle
** ---------------------------- */
  #undef  DOG_SM_RPT
  #define DOG_SM_RPT       sm_dog_rpt_var
  static dog_report_type   sm_dog_rpt_var           = 0;      /* Initial Safety */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
// Define the SM REX critical section variable
   
  rex_crit_sect_type  sm_crit_sect;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */


#ifdef FEATURE_DUAL_DATA
sm_as_id_type_e sm_as_id;
#else
sys_modem_as_id_e_type sm_as_id;
sys_modem_as_id_e_type ctxt_as_id;
#endif 


#ifdef FEATURE_DUAL_DATA
/* Allocate an array of maximum number of PDP contexts */
pdp_context_info  *pdp_list_sim[MAX_AS_IDS][MAX_PDP_NUM];
#define pdp_list pdp_list_sim[sm_as_id]

#ifndef FEATURE_MODEM_HEAP
pdp_context_info  pdp_list_static_sim[MAX_AS_IDS][MAX_PDP_NUM];
#define pdp_list_static pdp_list_static_sim[sm_as_id]
#endif

boolean sm_ps_vote_for_reselection_sim[MAX_AS_IDS] = {TRUE, TRUE};
#define sm_ps_vote_for_reselection sm_ps_vote_for_reselection_sim[sm_as_id]

sm_blocked_apn_list_T sm_blocked_apn_list_sim[MAX_AS_IDS];
#define sm_blocked_apn_list sm_blocked_apn_list_sim[sm_as_id]

byte ps_call_info_ti_sim[MAX_AS_IDS] ;
#define ps_call_info_ti ps_call_info_ti_sim[sm_as_id]


/* Allocate an array of maximum number of NSAPIs */
int nsapi_list_sim[MAX_AS_IDS][MAX_NSAPI_NUM];
#define nsapi_list nsapi_list_sim[sm_as_id]

/* Indicates if the Signaling Connection is available */
boolean sig_connection_available_sim[MAX_AS_IDS];
#define sig_connection_available sig_connection_available_sim[sm_as_id]

boolean ps_signalling_conn_state_sim[MAX_AS_IDS];
#define ps_signalling_conn_state ps_signalling_conn_state_sim[sm_as_id]

boolean sm_is_pdp_retranmsit_req_sim[MAX_AS_IDS];
#define sm_is_pdp_retranmsit_req sm_is_pdp_retranmsit_req_sim[sm_as_id]

#ifdef FEATURE_UE_INITIATED_DORMANCY 
/* Indicates if CM has requested for ps signalling release */
boolean cm_initiated_ps_signalling_release_sim[MAX_AS_IDS];
#define cm_initiated_ps_signalling_release cm_initiated_ps_signalling_release_sim[sm_as_id]
#ifdef FEATURE_DUAL_SIM
smreg_ps_signalling_rel_req_cause_T cm_initiated_ps_signalling_release_cause_sim[MAX_AS_IDS];
#define cm_initiated_ps_signalling_release_cause cm_initiated_ps_signalling_release_cause_sim[sm_as_id]
#endif
#endif /*FEATURE_UE_INITIATED_DORMANCY*/

/*Indicates the TIMER_ESTABLISH_PENDING is running*/
boolean timer_est_pen_flag_sim[MAX_AS_IDS];
#define timer_est_pen_flag timer_est_pen_flag_sim[sm_as_id]

boolean sm_abort_cnf_pending_sim[MAX_AS_IDS];
#define sm_abort_cnf_pending sm_abort_cnf_pending_sim[sm_as_id]

#ifdef FEATURE_GSM_GPRS      
gmmsm_unitdata_ind_T pending_sm_pdu_sim[MAX_AS_IDS];
#define pending_sm_pdu pending_sm_pdu_sim[sm_as_id]

byte pending_sm_pdu_id_sim[MAX_AS_IDS];
#define pending_sm_pdu_id pending_sm_pdu_id_sim[sm_as_id]
#endif

/* Counter for Maximum number of timer expiry counts */
word sm_max_timer_count_sim[MAX_AS_IDS][MAX_PDP_NUM], max_est_timer_count_sim[MAX_AS_IDS];
#define sm_max_timer_count sm_max_timer_count_sim[sm_as_id]
#define max_est_timer_count max_est_timer_count_sim[sm_as_id]

rex_timer_type gmmsm_establish_timer_sim[MAX_AS_IDS];/* Timer for GMM-SM Establishment */
#define gmmsm_establish_timer gmmsm_establish_timer_sim[sm_as_id]

// Array contains connection Id of pending timers
// Currently only one timer is active at any instance.
word timers_pending_sim[MAX_AS_IDS][1]; 
//#define timers_pending timers_pending_sim[sm_as_id]

uint32 sm_tlli_sim[MAX_AS_IDS];
#define sm_tlli sm_tlli_sim[sm_as_id]


#else /* #if defined(FEATURE_DUAL_SIM) */


// REX Timers used in the SM task 
rex_timer_type gmmsm_establish_timer;/* Timer for GMM-SM Establishment */


// Array contains connection Id of pending timers
// Currently only one timer is active at any instance.
word timers_pending[1]; 


/* Allocate an array of maximum number of PDP contexts */
pdp_context_info  *pdp_list[MAX_PDP_NUM] = {0};

#ifndef FEATURE_MODEM_HEAP
pdp_context_info  pdp_list_static[MAX_PDP_NUM];
#endif

boolean sm_ps_vote_for_reselection = TRUE;
byte ps_call_info_ti ;

sm_blocked_apn_list_T sm_blocked_apn_list = {0};

/* Allocate an array of maximum number of NSAPIs */
int nsapi_list[MAX_NSAPI_NUM];

/* Indicates if the Signaling Connection is available */
boolean sig_connection_available;

boolean ps_signalling_conn_state;

boolean sm_is_pdp_retranmsit_req;

/* Indicates if CM has requested for ps signalling release */
boolean cm_initiated_ps_signalling_release;
#ifdef FEATURE_DUAL_SIM
smreg_ps_signalling_rel_req_cause_T cm_initiated_ps_signalling_release_cause;
#endif

/*Indicates the TIMER_ESTABLISH_PENDING is running*/
boolean timer_est_pen_flag;
/* Indicates if inter-system change is pending */
//boolean inter_system_change_pending = FALSE;

extern void send_rabmcm_cmd(byte rab_id, rabm_cm_enum_T rabm_cmd );
boolean sm_abort_cnf_pending;


#ifdef FEATURE_NAS_UDT_OOS
#error code not present
#endif

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
extern void sm_continue_resend_pdp_msg (pdp_context_info *pdp);
#endif /* #ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM */

#ifdef FEATURE_GSM_GPRS      
gmmsm_unitdata_ind_T pending_sm_pdu;
byte pending_sm_pdu_id;
#endif

/* Counter for Maximum number of timer expiry counts */
word sm_max_timer_count[MAX_PDP_NUM], max_est_timer_count;

uint32 sm_tlli;

#endif /*#if defined(FEATURE_DUAL_SIM)*/

#ifdef FEATURE_DUAL_SIM
extern boolean SM_IS_VALID_AS_ID(sys_modem_as_id_e_type cmd_as_id);
#endif

extern boolean sm_get_as_id(sm_cmd_type *cmd_ptr);

/* buffer to log SM/ESM messages esnt and received */
sm_esm_debug_buffer_s_type sm_esm_debug_buffer[SM_DEBUG_MESSAGE_BUFFER_SIZE];
dword sm_esm_debug_buffer_index = 0;

/*======================================================================*/

rex_tcb_type* sm_tcb_ptr;

/* =========================================================================
** LOCAL Data Declarations
** ======================================================================= */
LOCAL q_type sm_cmd_q;    /* CMD QUEUE for SM */

/* =========================================================================
** LOCAL FUNCTION PROTOTYPES.
** ======================================================================== */

/* =========================================================================
** EXTERN REFERENCES.
** ======================================================================== */

#ifdef FEATURE_GSM_GPRS
/* -----------------------------------------------------------------------
** FUNCTION     : SM_SEND_SNDCP_CMD

** DESCRIPTION  : This function sends command to SNDCP
** ----------------------------------------------------------------------- */
extern void sm_send_sndcp_cmd( pdp_context_info *cur_pdp, gsnsm_prim_type sndcp_cmd);
#endif

extern boolean sm_check_mpdp_conn_pend(void);


/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================

FUNCTION SM_SEND_ROUTINE

DESCRIPTION
  This is the SM-specific routine used when other layers call gs_send_message

DEPENDENCIES
  None

RETURN VALUE
  GS_STATUS - GS_SUCCESS if sent, GS_FAILURE otherwise

SIDE EFFECTS
  None

===========================================================================*/
gs_status_T sm_send_routine( void *msg_p )
{
    word          message_length;
    void         *cmd_ptr;

    /* Get message length */
    GET_IMH_LEN(message_length, msg_p); 

    /* Add in header size */
    message_length += sizeof(IMH_T);

    /* Get command buffer from destination task */
    if((cmd_ptr = sm_get_cmd_buf()) != NULL)
    {
       /* Copy data to command buffer */
       memscpy(&((sm_cmd_type*)cmd_ptr)->cmd, (sizeof(sm_cmd_type) - sizeof(sm_cmd_hdr_type)), (byte*)msg_p, message_length);

       /* Put message in destination task queue */
       sm_put_cmd((sm_cmd_type*) cmd_ptr);
       return GS_SUCCESS;
    }
        
    return GS_FAILURE;
}

/*===========================================================================

FUNCTION SM_INIT_BEFORE_TASK_START

DESCRIPTION
  This function does the initialization required before task is started
  It means Initialization of Queues and Critical sections.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sm_init_before_task_start( void )
{
  /* Initialize all the command queues */
  (void)q_init(&sm_cmd_q);

  /* Initialize the Critical Section for SM Task */
  rex_init_crit_sect(&sm_crit_sect); 

  /* Initialize gs message passing for this queue */
  if (gs_init_for_send_only(GS_QUEUE_SM,
        (gs_put_handler_ptr)sm_send_routine) != GS_SUCCESS)
  {
      MSG_ERROR_DS_0(SM_SUB,"SM not initialized!");
  }
}

/*===========================================================================

FUNCTION SM_INIT

DESCRIPTION
  This function does the initialization for SM task. It means initializing 
  all SM global data.   
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sm_init( void )
{
  word i;

  sm_pd = SM_PD;   /* Set the SM protocol ID */
  
  /* Set up a timer to path the dog */
#ifndef TEST_FRAMEWORK
  /* Register with DOG HB new API */
  sm_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) SM_DOGRPT_TIMER_SIG);
#endif

  /* initialize sm debug buffer */
  memset(sm_esm_debug_buffer,0,sizeof(sm_esm_debug_buffer_s_type)*SM_DEBUG_MESSAGE_BUFFER_SIZE);
  sm_esm_debug_buffer_index = 0;

#ifdef FEATURE_DUAL_DATA
  for ( sm_as_id = SM_AS_ID_1; sm_as_id < (sm_as_id_type_e)MAX_AS_IDS; sm_as_id ++ )
  {
#endif
    timers_pending[TIMER_PENDING] = INVALID_ID;

    // define the PDP context state timers
    rex_def_timer( &gmmsm_establish_timer, sm_tcb_ptr, (rex_sigs_type)TIMER_ESTABLISH_PENDING );
    /* ---------------------------------------------------------------- 
    ** Initialize the following fields of the PDP
    ** transaction Id for all the PDP contexts as INVALID_ID
    ** --------------------------------------------------------------*/
    for (i=0; i<MAX_PDP_NUM; i++)
    {
      // Initialize the Maximum timer count to 0.
      sm_max_timer_count[i] = 0;
    }

    sm_is_pdp_retranmsit_req = FALSE;
    
    max_est_timer_count = 0;

    // Initialize the sig_connection_available to FALSE.
    sig_connection_available = FALSE;

    //Initialize the PS siganlling connection state variable
    ps_signalling_conn_state = FALSE;

    sm_abort_cnf_pending =FALSE;

    // Initialize the timer_est_pen_flag to FALSE
    timer_est_pen_flag = FALSE;

    cm_initiated_ps_signalling_release = FALSE;
#ifdef FEATURE_DUAL_SIM
    cm_initiated_ps_signalling_release_cause = SMREG_PS_SIG_REL_REQ_CAUSE_MIN;
#endif

#ifdef FEATURE_GSM_GPRS      
    pending_sm_pdu_id = INVALID_ID;
    pending_sm_pdu.data = NULL;
#endif

     for (i=0; i<MAX_PDP_NUM; i++)
     {
       //Reset the PDP call object
       pdp_list[i] = NULL;
     }

     // Initialize NSAPI array. 0~4 is not used anyway
     for (i=0; i<MAX_NSAPI_NUM; i++)
     {
       nsapi_list[i] = INVALID_ID;
     }

   #ifdef FEATURE_DUAL_SIM
   (void) memset(&sm_blocked_apn_list,0,(sizeof(sm_blocked_apn_list_T)));
   #endif

#ifdef FEATURE_DUAL_DATA
  }
#endif

#ifdef FEATURE_LTE
  sm_deact_isr_on_interrat_status_set(SM_DEACT_ISR_ON_INTERRAT_NONE);
#endif
}

/*===========================================================================


FUNCTION SM_TASK

DESCRIPTION
  This function is the entry point for SM Task. It initializes SM variables
  by calling sm_init(). The ACK signal is sent back to TMC and a wait is 
  done on Task Start Signal from TMC. Once a Task Start Signal is received,
  it waits for events to arrive, which are processed accordingly.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sm_task (dword param)
{                             

  sm_cmd_type   *sm_cmd_ptr;     /* To hold received SM command */
  rabm_cmd_type *rabm_cmd_ptr;   /* To hold received RABM command */
  rex_sigs_type wait_sigs, sigs; /* Signal mask for SM task to wait */
  #ifndef FEATURE_DUAL_DATA
  word con_id;                   /* Represents the connection Id of PDP */
  boolean timer_expired = FALSE;
  #endif
  sm_timer_T sm_timer_type;

  sm_tcb_ptr = rex_self();
  (void)param;
  sm_timer_type = (sm_timer_T)0;

#ifndef FEATURE_MODEM_RCINIT
  sm_init_before_task_start();
  rabm_init_before_task_start();
#endif

  /* Initialize the SM task and RABM subtask */
  sm_init();

  rabm_init();

#ifdef FEATURE_MODEM_RCINIT
  rcinit_handshake_startup();
#else
#ifndef FEATURE_MODEM_RCINIT_PHASE2
  tmc_task_start();
#endif
#endif

#ifdef FEATURE_LTE
  esm_init();
#endif /*FEATURE_LTE*/

#ifndef TEST_FRAMEWORK 
   /* Report to the Dog HB */
   dog_hb_report(DOG_SM_RPT);
#endif

#ifdef FEATURE_DUAL_SIM
  #ifdef FEATURE_DUAL_DATA
  sm_as_id = SM_AS_ID_1;
  #else
  sm_as_id = SYS_MODEM_AS_ID_NONE;
  #endif
#endif



  wait_sigs = SM_CMD_Q_SIG | RABM_CMD_Q_SIG | 
                     SM_DOGRPT_TIMER_SIG 
#ifndef FEATURE_DUAL_DATA
              |(int)TIMER_ESTABLISH_PENDING 
#endif 
#ifdef FEATURE_LTE
              |(int)ESM_CMD_Q_SIG
#endif   /*FEATURE_LTE*/         
#ifdef TEST_FRAMEWORK
              #error code not present
#endif /*TEST_FRAMWEWORK*/
              ;

  /* Start a Forever loop and process the commands that arrive */
  for (;;)
  {
    /* Wait on signal mask*/
    sigs = rex_wait (wait_sigs);

    /* If this signal is for the SM command queue */
    if (sigs & SM_CMD_Q_SIG)
    {
      /* Received a Command on the SM command Queue, clear it and process */
      /* Clear signals */
      (void) rex_clr_sigs( sm_tcb_ptr, SM_CMD_Q_SIG);

      /* Get the command from the SM_CMD_Q and process it */
      while ((sm_cmd_ptr = (sm_cmd_type*) q_get(&sm_cmd_q)) != NULL)
      {
       if(sm_get_as_id(sm_cmd_ptr) == TRUE)
       {
         /* Switch on the command id of the received command, and
          call the appropriate function to process each command. */
        switch (sm_cmd_ptr->header.message_set)
        {
          case MS_RABM_SM:
            sm_process_rabm_cmd( sm_cmd_ptr );
            break;

          case MS_GMM_SM:
            sm_process_gmm_cmd( sm_cmd_ptr );
            break;

#ifdef FEATURE_GSM_GPRS
          case MS_SNDCP_SM:
            sm_process_sndcp_cmd( sm_cmd_ptr );
            break;
#endif
#ifdef FEATURE_MBMS
          #error code not present
#endif
          case MS_CM_SM:
            sm_process_cm_cmd( sm_cmd_ptr );
            break;

          case MS_TIMER:
            sm_process_timer_expiry_cmd( sm_cmd_ptr );
            break;

          case MS_SM_SM:
            break;

          default:
            /* Wrong command received on the SM CMD Q */
            MSG_ERROR_DS_1(SM_SUB, "Unknown cmd received by SM %d", 
                 sm_cmd_ptr->header.cmd_id);
            break;
         }/* end switch */
       }
        /* The command has been processed. Free the memory for this cmd */
        /*lint -e424*/
#ifdef FEATURE_MODEM_HEAP  
        if((sm_cmd_ptr->header.message_set == MS_GMM_SM) && 
           (sm_cmd_ptr->header.cmd_id == GMMSM_UNITDATA_IND) && 
           (sm_cmd_ptr->cmd.gmm_unitdata_ind.data != NULL))
        {
          modem_mem_free(sm_cmd_ptr->cmd.gmm_unitdata_ind.data, MODEM_MEM_CLIENT_NAS);
          sm_cmd_ptr->cmd.gmm_unitdata_ind.data = NULL;
        }
        modem_mem_free(sm_cmd_ptr, MODEM_MEM_CLIENT_NAS);
#else
        if((sm_cmd_ptr->header.message_set == MS_GMM_SM) &&
           (sm_cmd_ptr->header.cmd_id == GMMSM_UNITDATA_IND) && 
           (sm_cmd_ptr->cmd.gmm_unitdata_ind.data != NULL))
        {
          mem_free( &(tmc_heap), sm_cmd_ptr->cmd.gmm_unitdata_ind.data );
          sm_cmd_ptr->cmd.gmm_unitdata_ind.data = NULL;
        }
        mem_free( &(tmc_heap), sm_cmd_ptr );
#endif
        /*lint +e424*/
      }/* end while */
    }/* end of SM_CMD_Q_SIG check */

    /* Check if the signal is set for RABM */
    if (sigs & RABM_CMD_Q_SIG)
    {
      /* Received a Command on the RABM command Queue, clear it and process */
      /* Clear signals */
      (void) rex_clr_sigs( sm_tcb_ptr, RABM_CMD_Q_SIG);

      /* Get the command from the SM_CMD_Q and process it */
      while ((rabm_cmd_ptr = (rabm_cmd_type*) q_get(&rabm_cmd_q)) != NULL)
      {
        if(rabm_get_as_id(rabm_cmd_ptr) == TRUE)
        {
          /* ----------------------------------------------------------------
          ** Send the Command to the RABM subtask to process it 
          ** --------------------------------------------------------------*/
          rabm_process_cmd (rabm_cmd_ptr);
        }
      }/* end while */
    }

    /* Check if report timer signal was set.  If set then
       clear signal is TIMER_ESTABLISH_PENDING then set the timer again and proceed */
#ifndef FEATURE_DUAL_DATA
    if (sigs & (int)TIMER_ESTABLISH_PENDING)
    {
      sm_timer_type = TIMER_ESTABLISH_PENDING;
      timer_expired = TRUE;
    }

    if (timer_expired)
    {
      // Clear the Corresonding timer and its signal.
      sm_clear_pdp_timer (sm_timer_type);

      // Get the connection Id from the pending timers array
      con_id = timers_pending[TIMER_PENDING];

      // Perform timer expiry action procedures
      sm_proc_timer_expired(con_id, sm_timer_type);   

      timer_expired = FALSE;
    }
#endif
    /* Check if report timer signal was set.  If set then
       clear signal, report to DOG, set the timer again and proceed. */
    if (sigs & SM_DOGRPT_TIMER_SIG)
    {
       /* Clear the watchdog signal */
       (void) rex_clr_sigs( sm_tcb_ptr, SM_DOGRPT_TIMER_SIG);

       /* Kick watchdog and reset timer */
#ifndef TEST_FRAMEWORK       
         /* Kick the watchdog */
       /* Report to the Dog HB */
       dog_hb_report(DOG_SM_RPT);
#endif       
    } 
#ifdef FEATURE_LTE    
    /* Process ESM signal */
    if(sigs & ESM_CMD_Q_SIG)
    {     
      /*clear signal */
      (void) rex_clr_sigs( sm_tcb_ptr, ESM_CMD_Q_SIG);
       esm_task();
    }
#endif /*FEATURE_LTE*/
#ifdef TEST_FRAMEWORK
    #error code not present
#endif 
  
    /* Add more signal checkin if SM task is waiting on more */
  }
}


/*===========================================================================

FUNCTION SM_PUT_CMD

DESCRIPTION
  his function puts a command buffer into SM command queue.
  Note that the command buffer must have been requested through a 
  call to sm_get_cmd_buf(). Also, the calling function must have 
  filled all the necessary data including the command id before 
  passing the buffer to this function.

  This function also sets the appropriate signal to the SM
  task to indicate that a command has been placed in its queue. 
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A signal is set for the SM task and this causes a context switch.


===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void sm_put_cmd (sm_cmd_type * cmd_buf)
{
   if(cmd_buf->header.message_set == MS_CM_SM)
   {
     /* Log using CFA packet */
     cfa_log_packet_ex( GS_QUEUE_SM,
                        cmd_buf->header.message_set,
                        (byte)(cmd_buf->header.cmd_id),
                        MIN(250, sizeof(cmd_buf->cmd)),
                        (byte*)&(cmd_buf->cmd) );
   }
   if(cmd_buf->header.message_set == MS_GMM_SM)
   {
#ifdef FEATURE_SGLTE
      if(MM_SUB_IS_SGLTE &&
         (mm_as_id != mm_ps_stack_id))
      {
        MSG_HIGH_DS_2(SM_SUB," =MM=  Ignoring GMM SM message %d on CS stack %d",(dword)(cmd_buf->header.cmd_id),mm_as_id);
#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(cmd_buf,MODEM_MEM_CLIENT_NAS);
#endif
        return;
      }
#endif
      mm_add_message_to_debug_buffer(
              cmd_buf->header.message_set,
              (dword)(cmd_buf->header.cmd_id),
              FROM_MM,
              (sys_modem_as_id_e_type)mm_as_id

         );
   }

  MSG_LOW_DS_0(SM_SUB," Putting CMD in sm_cmd_q ");

  /* Initialize the queue link before putting an item in a queue. */
  (void) q_link(cmd_buf, &(cmd_buf->header.link));

  /* Then put the command buffer in the queue */
  q_put(&sm_cmd_q, &(cmd_buf->header.link));

  /* Then set the signal for this queue */
  (void) rex_set_sigs(sm_tcb_ptr, SM_CMD_Q_SIG);
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION SM_GET_CMD_BUF

DESCRIPTION

  This function returns a SM command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer is freed by calling sm_free_cmd_buf(). 

  The freeing is done by the function that dequeues and processes this
  command buffer. That means this is done by SM task.
  
  Calling function MUST check for a NULL return value and take necessary
  action.
 
DEPENDENCIES

  None.
  
RETURN VALUE

  A buffer of type sm_cmd_type. NULL is returned if memory is unavailable.

SIDE EFFECTS

  None.
  
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
sm_cmd_type *sm_get_cmd_buf( void )
{
  sm_cmd_type *ptr;

  /* Allocating memory for SM Command buffer from the TMC heap */
#ifdef FEATURE_MODEM_HEAP
  if ((ptr = (sm_cmd_type *)modem_mem_calloc(1, sizeof(sm_cmd_type), MODEM_MEM_CLIENT_NAS)) == NULL)
  {
    MSG_ERROR_DS_0(SM_SUB,"Could not allocate memory for SM cmd q!");
  }
#else
  if ((ptr = (sm_cmd_type *)mem_malloc(&(tmc_heap), 
                                       sizeof(sm_cmd_type))) == NULL)
  {
    MSG_ERROR_DS_0(SM_SUB,"Could not allocate memory for SM cmd q!");
  }
  memset(ptr,0x00,sizeof(sm_cmd_type));
#endif
  return(ptr);
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION SM_PROC_TIMER_EXPIRED

DESCRIPTION
  This function Performs timer expiry procedures.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sm_proc_timer_expired(word con_id, sm_timer_T sm_timer_type)
{
  pdp_context_info *cur_pdp; /* current pdp context in the transaction */
  smreg_pdp_deactivate_req_T *pdp_deactivate = NULL;

  if (INVALID_ID == con_id )
  {
    MSG_HIGH_DS_2(SM_SUB, "Connection ID: %d already cleared, sm_timer_type: %d", con_id, sm_timer_type); 
    return ;
  }
  // Set the current PDP context.
  cur_pdp = pdp_list[con_id];
  max_est_timer_count++;
  /* -------------------------------------------------------------
  ** Check if the TIMER_ESTABLISH_PENDING Timer expired 
  ** ------------------------------------------------------------*/
  if (sm_timer_type == TIMER_ESTABLISH_PENDING)
  {
    word i;
    // Check if timer expiry has reached maximum count
    if (max_est_timer_count < MAX_EST_TIMER_CNT)
    {
      MSG_HIGH_DS_1(SM_SUB,"TIMER_ESTABLISH_PENDING expired cnt:%d", max_est_timer_count );      
      //reset the timer_est_pen_flag 
      timer_est_pen_flag = FALSE;
      sig_connection_available = FALSE;
      if(pdp_list[con_id] == NULL)
      {
        MSG_HIGH_DS_1(SM_SUB,"TIMER ESTABLISH PENDING EXPIRED, PDP is NULL, TransId: %d", con_id); // LCU_PENDING, check for any danglings
#ifndef FEATURE_DUAL_DATA
        //Loop through remaining PDPs which are in Signalling connection pending state
        for (i = 0; i < MAX_PDP_NUM; i++)
        {
          if (pdp_list[i] != NULL && (pdp_list[i]->pdp_con_state == SIG_CONNECTION_PENDING))
          {
            con_id = i;
            MSG_HIGH_DS_1(SM_SUB,"Found another PDP in SIG_CONNECTION_PENDING, TransId : %d", con_id);
            break;
          }
        }
        if( i == MAX_PDP_NUM)
#endif
        return;
      } 
      if (pdp_list[con_id]->pdp_con_state == SIG_CONNECTION_PENDING)
      {
        #ifdef FEATURE_MBMS
        #error code not present
#endif
        if (pdp_list[con_id]->pdp_state == PDP_INACTIVE || 
           pdp_list[con_id]->pdp_state == PDP_ACTIVE_PENDING)
        {
          // Send the PDP activation request again 
          MSG_MED_DS_1(SM_SUB, "GMMSM ESTABLISH timer expired, cnt %d",max_est_timer_count);
          if (pdp_list[con_id]->cm_pending_msg == (int)SMREG_PDP_ACTIVATE_REJ_RSP)
          {
            sm_continue_pdp_activate_rej_rsp(pdp_list[con_id]);
          }
          else
          {
            sm_continue_pdp_activation(pdp_list[con_id]);
          }
        }
        else if (pdp_list[con_id]->pdp_state == PDP_MODIFY_PENDING)
        {
          MSG_MED_DS_1(SM_SUB, "GMMSM ESTABLISH timer expired, cnt %d",max_est_timer_count);
          if (pdp_list[con_id]->cm_pending_msg == (int)MS_MODIFY_PDP_ACCEPT)
          {            
            sm_continue_pdp_nw_modify_cnf(pdp_list[con_id]);
          }
          else if(pdp_list[con_id]->cm_pending_msg== (int) MODIFY_PDP_REJECT)
          {
            sm_continue_pdp_nw_modify_rej(pdp_list[con_id]);
          }       
          else
          {
          // Send the PDP activation request again 
          MSG_MED_DS_1(SM_SUB, "GMMSM ESTABLISH timer expired, cnt %d",max_est_timer_count);
          sm_continue_pdp_modify(pdp_list[con_id]);
          }
        }
        else if (pdp_list[con_id]->pdp_state == PDP_INACTIVE_PENDING)
        {
          // Send the PDP activation request again 
          MSG_MED_DS_1(SM_SUB, "GMMSM ESTABLISH timer expired, cnt %d",max_est_timer_count);
          sm_continue_pdp_deactivate(pdp_list[con_id]);
        }
      }
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
      else if (pdp_list[con_id]->status_ind_flag == TRUE )
      {
        if (max_est_timer_count < 2)
        {
          sm_continue_resend_pdp_msg (pdp_list[con_id]);
        }
        else
        {
          pdp_list[con_id]->sm_msg_to_resend   = (sm_layer_msg_T)0xFF;
          pdp_list[con_id]->status_ind_flag = FALSE;
        }
      }
#endif
    }
    //Timer timed out trigger the timer timed out procedures
    else
    {
      MSG_HIGH_DS_0(SM_SUB, "GMMSM ESTABLISH timer MAX COUNT expired"); 
      // Reset the sm_max_timer_count
      max_est_timer_count = 0;
      //Set the timer_est_pen_flag to FALSE
      timer_est_pen_flag = FALSE;
      //Update the call status for all PDPs waiting for GMM connection
      for (i = 0; i < MAX_PDP_NUM; i++)
      {
        if (pdp_list[i] != NULL && pdp_list[i]->pdp_con_state == SIG_CONNECTION_PENDING)
        {
          // Failed to receive the ACCEPT message from the network, change state
          #ifdef FEATURE_MBMS
          #error code not present
#endif     
          if (pdp_list[i]->pdp_state == PDP_INACTIVE || 
              pdp_list[i]->pdp_state == PDP_ACTIVE_PENDING)
          {
            if (pdp_list[i]->cm_pending_msg == (int)SMREG_PDP_ACTIVATE_REJ_RSP)
            {
              // Send a deactivate cnf message to CM
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
              sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_CNF, SM_INTERNAL_CAUSE);
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
            }
            else
            {
              //Reset the pdp_con_state
              pdp_list[i]->pdp_con_state = SIG_CONNECTION_NONE;
              // Set the cause to appropriate reject cause value
              if (pdp_list[i]->current_cause == (cc_cause_T) PPD_UNKNOWN_REASON)
              {
                pdp_list[i]->current_cause = (cc_cause_T)PDP_ESTABLISH_MAX_TIMEOUT;
              }           
              //Stop any timer running
              sm_stop_timer(i);
              // Send a reject message to CM with the appropriate cause
              //Deactivate RAB implicity
              if (is_umts_mode())
              {
                sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, pdp_list[i]->nsapi.nsapi);
              }
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
              sm_send_cm_confirm(pdp_list[i], SMREG_PDP_ACTIVATE_REJ, 
                           SM_INTERNAL_CAUSE);
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
            }
          }
          else if (pdp_list[i]->pdp_state == PDP_INACTIVE_PENDING)
          {
            pdp_context_info *pri_pdp = NULL;                         
            // Set the cause to appropriate reject cause value
            pdp_list[i]->current_cause = (cc_cause_T)PDP_ESTABLISH_MAX_TIMEOUT;
            
            //Incase tear down ind(TI) flag is set deactivate group PDPs locally
            if (pdp_list[i]->teardown_ind.ti_flag)
            {
              word index;
              byte group_id = pdp_list[i]->group_id;
              //switch off the tear down indicator
              pdp_list[i]->teardown_ind.ti_flag = FALSE;

              for (index = 0; index < MAX_PDP_NUM; index++)
              {
                if (pdp_list[index] != NULL && pdp_list[index]->pdp_state == PDP_INACTIVE_PENDING &&
                   pdp_list[index]->group_id == group_id)
                {
                 
                   pdp_list[index]->pdp_state = PDP_INACTIVE;
#ifdef FEATURE_GSM_GPRS
                   if (pdp_list[index]->llc_state != LLC_NOT_CONFIRMED)
                   {
                     if (pdp_list[index]->is_primary_pdp)
                     {
                       pri_pdp = pdp_list[index];                     
                     }
                     else
                     {
                       pdp_list[index]->llc_state = LLC_MO_DEACTIVATE_CNF_PENDING;
                       sm_send_sndcp_cmd (pdp_list[index], GSNSM_DEACTIVATE_IND);
                       if(sm_abort_cnf_pending)
                       {
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
                         if(pdp_list[index]->cm_pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ)
                         {
                           sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_REJ, 
                                                              SM_INTERNAL_CAUSE);                                         
                         }
                         else
                         {
                           sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_IND, 
                                                                SM_INTERNAL_CAUSE);                                                                      
                         }
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
                       }  
                     }
                   }
                   else
#endif                  
                   {
                    //Reset the pdp_con_state
                    pdp_list[index]->pdp_con_state = SIG_CONNECTION_NONE;
                    // Inform the RABM that the PDP context is being deactivated
                    MSG_MED_DS_1(SM_SUB, "Inform RABM PDP Nsapi %d deactivated",
                                              pdp_list[index]->nsapi.nsapi);
                    //Notify to RABM to implicitly deactivate the RABs //MBMS_PENDING
                    if (is_umts_mode())
                    {
                      sm_notify_rabm ((int)RABMSM_DEACTIVATE_IMPLICIT_IND, 
                      pdp_list[index]->nsapi.nsapi);
                    }
                    
                    if (pdp_list[index]->is_primary_pdp)
                    {
                      pri_pdp = pdp_list[index];
                    }
                    else
                    {
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
                     if (index == i)
                     {
                      // Send a reject message to CM with the appropriate cause
                      sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_REJ, 
                      SM_INTERNAL_CAUSE);
                     }
                     else
                     {
                      // Send a reject message to CM with the appropriate cause
                      sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_IND, 
                                         SM_INTERNAL_CAUSE);
                     }
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
                    }
                   }
                }//end if (pdp_list[index].pdp_state == PDP_INACTIVE_PENDING..).....
              }//end for (index =0; ....              
              //Deactivate Primary now
              if (pri_pdp)
              {
#ifdef FEATURE_GSM_GPRS              
                if (pri_pdp->llc_state != LLC_NOT_CONFIRMED)
                {
                  pri_pdp->llc_state = LLC_MO_DEACTIVATE_CNF_PENDING;
                  sm_send_sndcp_cmd (pri_pdp, GSNSM_DEACTIVATE_IND);
                  if(sm_abort_cnf_pending)
                  {
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
                    if(pri_pdp->cm_pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ)
                    {
                      sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_REJ, 
                                                         SM_INTERNAL_CAUSE);                                         
                    }
                    else
                    {
                      sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_IND, 
                                                           SM_INTERNAL_CAUSE);                                                                      
                    }
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
                  }          
                }
                else
#endif               
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
                if (pdp_list[i] == pri_pdp
                       && (pri_pdp->cm_pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ)
                )
                {
                   // Send a reject message to CM with the appropriate cause
                  sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_REJ, 
                                                        SM_INTERNAL_CAUSE);
                }
                else
                {
                  // Send a reject message to CM with the appropriate cause
                  sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_IND, 
                                                        SM_INTERNAL_CAUSE);
                }
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
              }
            }//end if (pdp_list[i].teardown_ind)
            //In case Tear Down ind is off deactivate single PDP
            else
            {
                //Deactivate RAB implicity
#ifdef FEATURE_GSM_GPRS
              if ( pdp_list[i]->llc_state != LLC_NOT_CONFIRMED)
              {
                pdp_list[i]->llc_state = LLC_MO_DEACTIVATE_CNF_PENDING;
                sm_send_sndcp_cmd (pdp_list[i], GSNSM_DEACTIVATE_IND);
                pdp_list[i]->pdp_state = PDP_INACTIVE;
              }
              else
#endif                
              {
                //Change the state to INACTIVE
                pdp_list[i]->pdp_state = PDP_INACTIVE;
                //Reset the pdp_con_state
                pdp_list[i]->pdp_con_state = SIG_CONNECTION_NONE;

                if (is_umts_mode())
                {
                  sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, 
                                        pdp_list[i]->nsapi.nsapi); //MBMS_PENDING
                }
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
                if (pdp_list[i]->is_primary_pdp && !pdp_list[i]->teardown_ind.ti_flag && sm_promote_sec_pri(pdp_list[i]))
                {
                  sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_PROMOTE_CNF, 
                           SM_INTERNAL_CAUSE);
                }
                else
                {
#endif
                  sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_REJ, 
                           SM_INTERNAL_CAUSE);
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
                }
#endif
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
              }//end if             
            }

            // Close PS Signaling Session 
            MSG_HIGH_DS_0(SM_SUB, "Calling GMM Close Session Req");
            send_gmm_close_session_req();
          }//else if (pdp_list[i].pdp_state == PDP_INACTIVE_PENDING)
          else if(pdp_list[i]->pdp_state == PDP_MODIFY_PENDING)
          {
            //Set the PDP state to ACTIVE
            pdp_list[i]->pdp_state = PDP_ACTIVE;
             //Reset the pdp_con_state
            pdp_list[i]->pdp_con_state = SIG_CONNECTION_NONE;
            // Set the cause to appropriate reject cause value
            pdp_list[i]->current_cause = (cc_cause_T)PDP_MODIFY_MAX_TIMEOUT;
            // Send a reject message to CM with the appropriate cause
            //Deactivate RAB implicity
            if (is_umts_mode())
            {
              if (pdp_list[i]->cm_pending_msg == (int)SMREG_PDP_MODIFY_REQ)
              {
                //If CM triggred the modification then send MODIFY_REJ to CM
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
                sm_send_cm_confirm(pdp_list[i], SMREG_PDP_MODIFY_REJ, SM_INTERNAL_CAUSE);
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
              }
              else if (pdp_list[i]->cm_pending_msg == (int)MS_MODIFY_PDP_ACCEPT ||
                         pdp_list[i]->cm_pending_msg == (int)MODIFY_PDP_REJECT)
              {
                MSG_MED_DS_1(SM_SUB,"Can not send NW Modify Rej/Cnf ConId=%d", pdp_list[i]->trans_id);
              }
              else
              {
                //If the Modification is triggered for RAB REST then deactivate RAB
                sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, pdp_list[i]->nsapi.nsapi);
                pdp_list[i]->cm_pending_msg = UNKNOWN_CMD;
              }
            }
          }
         }//end if
       }//end for
    }//else for main if
  }//end if (timer_type == TIMER_ESTABLISH_PENDING)

  /* -------------------------------------------------------------
  ** Check if the ACTIVE_PENDING Timer expired 
  ** ------------------------------------------------------------*/
  else if (sm_timer_type == TIMER_ACTIVE_PENDING)
  {
    // Check if timer expiry has reached maximum count
    if (sm_max_timer_count[con_id] < MAX_TIMER_CNT)
    {
      sm_max_timer_count[con_id]++;
      if (pdp_list[con_id] != NULL)
      {
        #ifdef FEATURE_MBMS
        #error code not present
#endif
        
         // Send the PDP activation request again 
         MSG_HIGH_DS_1(SM_SUB, "PDP ACTIVATE timer expired, cnt %d",sm_max_timer_count[con_id]);
         sm_continue_pdp_activation(pdp_list[con_id]);
              
        #ifdef FEATURE_MBMS
        #error code not present
#endif
      }
    }
    else
    {
      if (pdp_list[con_id] != NULL)
      {
        #ifdef FEATURE_MBMS
        #error code not present
#endif
        MSG_HIGH_DS_0(SM_SUB, "PDP ACTIVATE timer MAX COUNT expired");    
  
        // Failed to receive the ACCEPT message from the network, change state
        pdp_list[con_id]->pdp_state = PDP_INACTIVE;
  
        //Reset the PDP con state
        pdp_list[con_id]->pdp_con_state = SIG_CONNECTION_NONE;
  
        // Reset the sm_max_timer_count
        sm_max_timer_count[con_id] = 0;
  
        // Set the cause to appropriate reject cause value.
        pdp_list[con_id]->current_cause = (cc_cause_T)PDP_ACTIVATE_MAX_TIMEOUT; // MBMS_PENDING
  
        // Close PS Signaling Session 
        MSG_HIGH_DS_0(SM_SUB, "Calling GMM Close Session Req");
        send_gmm_close_session_req();
        
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
        #ifdef FEATURE_MBMS
        #error code not present
#endif
        if (pdp_list[con_id]->is_primary_pdp)
        {
          // If primary PDP rejcet with appropriate cause
          sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_ACTIVATE_REJ, 
                           SM_INTERNAL_CAUSE);
        }
        else
        {
          // If it is a secondary PDP reject with appropriate cause
          sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_ACTIVATE_SEC_REJ, 
                             SM_INTERNAL_CAUSE);
        }
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      }
    }
  }

  /* -------------------------------------------------------------
  ** Check if the TIMER_MODIFY_PENDING Timer expired 
  ** ------------------------------------------------------------*/
  else if (sm_timer_type == TIMER_MODIFY_PENDING)
  {
    // Check if timer expiry has reached maximum count
    if (sm_max_timer_count[con_id] < MAX_TIMER_CNT)
    {
      // Increment the sm_max_timer_count
      sm_max_timer_count[con_id]++;

      if (pdp_list[con_id] != NULL)
      {
        // Send the PDP deactivation request again 
        MSG_HIGH_DS_1(SM_SUB,"PDP MODIFY timer expired, cnt %d", sm_max_timer_count[con_id]);
        sm_continue_pdp_modify(pdp_list[con_id]);
      }
    }
    else
    {
      MSG_HIGH_DS_0(SM_SUB, "PDP MODIFY timer MAX COUNT expired");

      // Reset the sm_max_timer_count
      sm_max_timer_count[con_id] = 0;

      if (pdp_list[con_id] != NULL)
      {
        // Failed to receive the ACCEPT message from the networ, change state
        pdp_list[con_id]->pdp_state = PDP_ACTIVE;
  
        /* Set the cause to appropriate value, If this PDP MODIFY was not requested by
        DS then set it to RAB SETUP FAILURE*/
        if ((pdp_list[con_id]->cm_pending_msg != (int)SMREG_PDP_MODIFY_REQ)&&
             ( pdp_list[con_id]->qos.traffic_class == (byte)CONVERSATION_CLASS || 
               pdp_list[con_id]->qos.traffic_class == (byte)STREAMING_CLASS))
        {
          pdp_list[con_id]->current_cause = (cc_cause_T)RAB_SETUP_FAILURE;
        }
        else
        {
          pdp_list[con_id]->current_cause = (cc_cause_T)PDP_MODIFY_MAX_TIMEOUT;
        }
        // Close PS Signaling Session
        MSG_HIGH_DS_0(SM_SUB, "Calling GMM Close Session Req");
        send_gmm_close_session_req();
  
        // Send a reject message to CM with the appropriate cause
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
        sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_MODIFY_REJ, 
                           SM_INTERNAL_CAUSE);
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      }
    }
  }

  /* -------------------------------------------------------------
  ** Check if the TIMER_INACTIVE_PENDING Timer expired 
  ** ------------------------------------------------------------*/
  else if (sm_timer_type == TIMER_INACTIVE_PENDING)
  {
    // Check if timer expiry has reached maximum count
    if (sm_max_timer_count[con_id] < MAX_TIMER_CNT)
    {
      // Increment the sm_max_timer_count
      sm_max_timer_count[con_id]++;

      // Send the PDP deactivation request again 
      MSG_MED_DS_2(SM_SUB, "PDP DEACTIVATE timer expired, cnt %d, TransId: %d",sm_max_timer_count[con_id], con_id);
      if (pdp_list[con_id] != NULL)
      {
  #ifdef FEATURE_NAS_UDT_OOS  // MBMS_PENDING
        #error code not present
#endif
        sm_continue_pdp_deactivate(pdp_list[con_id]);
  #ifdef FEATURE_NAS_UDT_OOS
        #error code not present
#endif
        }

    }
    else
    {
      pdp_context_info *pri_pdp = NULL;
      word index;
      byte group_id = pdp_list[con_id]->group_id;
      MSG_HIGH_DS_0(SM_SUB, "PDP INACTIVE timer MAX COUNT expired");

      // Reset the sm_max_timer_count
      sm_max_timer_count[con_id] = 0;
      if (pdp_list[con_id] != NULL)
      {
        if (pdp_list[con_id]->teardown_ind.ti_flag)
        {
          for (index = 0; index < MAX_PDP_NUM; index++)
          {
            if (pdp_list[index] != NULL && pdp_list[index]->group_id == group_id)
            {
              // Set the cause to appropriate value, 
              pdp_list[index]->current_cause = (cc_cause_T)PDP_INACTIVE_MAX_TIMEOUT;
            
  #ifdef FEATURE_GSM_GPRS
              if (pdp_list[index]->llc_state != LLC_NOT_CONFIRMED &&
                  pdp_list[index]->pdp_state != PDP_INACTIVE)
              {
                if (pdp_list[index]->is_primary_pdp)
                {
                  pri_pdp = pdp_list[index];
                }
                else
                {
                  pdp_list[index]->llc_state = LLC_MO_DEACTIVATE_CNF_PENDING;
                  sm_send_sndcp_cmd (pdp_list[index], GSNSM_DEACTIVATE_IND);  
                  pdp_list[index]->pdp_state = PDP_INACTIVE ;

                  if(sm_abort_cnf_pending)
                  {
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
                    if(pdp_list[index]->cm_pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ)
                    {
                      sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_REJ, 
                                                       SM_INTERNAL_CAUSE);                                         
                    }
                    else
                    {
                      sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_IND, 
                                                         SM_INTERNAL_CAUSE);                                                                      
                    }
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
                  }          
                }
              }
              else              
   #endif /*FEATURE_GSM_GPRS*/        
              {
                // Failed to receive the ACCEPT message from the network, change state
                pdp_list[index]->pdp_state = PDP_INACTIVE;
                 //Reset the PDP con state
                pdp_list[index]->pdp_con_state = SIG_CONNECTION_NONE;
                if (pdp_list[index]->is_primary_pdp)
                {
                  pri_pdp = pdp_list[index];
                  pdp_list[index]->is_primary_pdp = FALSE;
                }
                else
                {
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
                  if (index == con_id)
                  {
                     // Send a reject message to CM with the appropriate cause
                    sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_REJ, 
                                                          SM_INTERNAL_CAUSE);
                  }
                  else
                  {
                    // Send a reject message to CM with the appropriate cause
                    sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_IND, 
                                                          SM_INTERNAL_CAUSE);
                  }
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
                }
              }                  
            }//end if (pdp_list[con_id].teardown_ind.ti_flag)..
          }//end for(index = 0  ...
          //Send  primary now
          if(pri_pdp)
          {
  #ifdef FEATURE_GSM_GPRS        
            if (pri_pdp->llc_state != LLC_NOT_CONFIRMED &&
                pri_pdp->pdp_state != PDP_INACTIVE)
            {
              pri_pdp->llc_state = LLC_MO_DEACTIVATE_CNF_PENDING;
              sm_send_sndcp_cmd (pri_pdp, GSNSM_DEACTIVATE_IND);
              pri_pdp->pdp_state = PDP_INACTIVE;

              if(sm_abort_cnf_pending)
              {
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
                if(pri_pdp->cm_pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ)
                {
                  sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_REJ, 
                                                     SM_INTERNAL_CAUSE);                                         
                }
                else
                {
                  sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_IND, 
                                                       SM_INTERNAL_CAUSE);                                                                      
                }
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
              }          
            }
            else
  #endif
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
            if (pri_pdp == pdp_list[con_id]
                    && (pri_pdp->cm_pending_msg == (int) SMREG_PDP_DEACTIVATE_REQ)
            )
            {
               // Send a reject message to CM with the appropriate cause
              sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_REJ, 
                                                    SM_INTERNAL_CAUSE);
            }
            else
            {
              // Send a reject message to CM with the appropriate cause
              sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_IND, 
                                         SM_INTERNAL_CAUSE);
            }
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
          }        
        }
        //If TI flag is off deactivate one PDP
        else
        {
  #ifdef FEATURE_GSM_GPRS      
          if(pdp_list[con_id]->llc_state != LLC_NOT_CONFIRMED)
          {
             /* Set the llc_state to LLC_CONFIRM_PENDING, start RABM insanity timer */
             pdp_list[con_id]->llc_state = LLC_MO_DEACTIVATE_CNF_PENDING;
             if (pdp_list[con_id]->pdp_state != PDP_INACTIVE)
             {
               sm_send_sndcp_cmd (pdp_list[con_id], GSNSM_DEACTIVATE_IND);  
               pdp_list[con_id]->pdp_state = PDP_INACTIVE ;
             }
          }
          else
  #endif
          {
    
            // Failed to receive the ACCEPT message from the network, change state
            pdp_list[con_id]->pdp_state = PDP_INACTIVE;
    
            //Reset the PDP con state
            pdp_list[con_id]->pdp_con_state = SIG_CONNECTION_NONE;
    
            // Reset the sm_max_timer_count
            sm_max_timer_count[con_id] = 0;
      
            // Set the cause to appropriate reject cause value.
            pdp_list[con_id]->current_cause = (cc_cause_T)PDP_INACTIVE_MAX_TIMEOUT;
            
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
            // Send a reject message to CM with the appropriate cause
            if (pdp_list[con_id]->is_primary_pdp && !pdp_list[con_id]->teardown_ind.ti_flag && sm_promote_sec_pri(pdp_list[con_id]))
            {
                sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_DEACTIVATE_PROMOTE_CNF, 
                             SM_INTERNAL_CAUSE);
            }
            else
            {
#endif
                sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_DEACTIVATE_REJ, 
                             SM_INTERNAL_CAUSE);
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
            }
#endif
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
          }
        }
      }
      send_gmm_close_session_req();
    }
  }
  /* -------------------------------------------------------------
  ** Check if the TIMER_RABM_RESP_PENDING Timer expired 
  ** ------------------------------------------------------------*/
  // MBMS_PENDING incase of De-Act/Modify, this may need to be updated
  else if (sm_timer_type == TIMER_RABM_RESP_PENDING)
  {
    // Reset the sm_max_timer_count
    sm_max_timer_count[con_id] = 0;

#ifdef FEATURE_GSM_GPRS    
    if (is_gsm_mode() && 
#ifdef FEATURE_DUAL_SIM
       !is_WtoGCCO_in_progress((sys_modem_as_id_e_type)sm_as_id)
#else
        !is_WtoGCCO_in_progress()
#endif
       )
    {     
      if (pdp_list[con_id] != NULL && 
          pdp_list[con_id]->pdp_state == PDP_INACTIVE_PENDING &&
          pdp_list[con_id]->llc_state == LLC_MT_DEACTIVATE_CNF_PENDING)
      {
        //Change the PDP state to PDP_INACTIVE
        pdp_list[con_id]->pdp_state = PDP_INACTIVE;
        /* reset the LLC state of this PDP context */
        pdp_list[con_id]->llc_state = LLC_NOT_CONFIRMED;
        /*LLC guard timer expired in case of GPRS release send 
          deactivate indication to CM*/
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
         sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_DEACTIVATE_IND, 
                           SM_INTERNAL_CAUSE);
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      }
      else
      {
        /* -----------------------------------------------------------------
        ** If the UE is operating in GSM RAT, then LLC timers guard link
        ** establishment processing. RABM response timer should not run.
        ** ---------------------------------------------------------------*/
          MSG_ERROR_DS_1(SM_SUB, "RABM Response Timer expiry in GSM Mode TransId: %d", con_id);
      }
    }
#endif

    if (is_umts_mode() ||
#ifdef FEATURE_DUAL_SIM
        is_WtoGCCO_in_progress((sys_modem_as_id_e_type)sm_as_id)
#else
        is_WtoGCCO_in_progress()
#endif
       )
    {
      /* ---------------------------------------------------------------
      ** RABM Response timer expired, This is a result of RABM not 
      ** Receiving a Response from the RRC (Access Stratum) layer.
      ** --------------------------------------------------------------*/
      
      if (pdp_list[con_id] == NULL)
      {
        MSG_HIGH_DS_1(SM_SUB, "RABM Sanity timer expired, PDP NULL, TransId: %d",con_id);
        return;
      }
      pdp_list[con_id]->current_cause = (cc_cause_T)RAB_SETUP_FAILURE;

      // RABM Response timer expired 
      MSG_HIGH_DS_1(SM_SUB, "RABM Sanity timer expired, rab-state : %d",pdp_list[con_id]->rab_state);
     
      // Based on the current Pending RAB state, send the Reject to CM.
      if (pdp_list[con_id]->rab_state == RABM_ACTIVE_PENDING)
      {       
        // Inform the RABM that the PDP context is being deactivated
        MSG_HIGH_DS_1(SM_SUB, "Inform RABM PDP %d being deactivated",cur_pdp->nsapi.nsapi);
        cur_pdp->rab_state = RABM_INACTIVE_PENDING;
        sm_notify_rabm((int)RABMSM_DEACTIVATE_IND, cur_pdp->nsapi.nsapi);   

        // Start the deactivation procedures, due to RAB setup failure.
        pdp_list[con_id]->current_cause = (cc_cause_T)SM_PROTOCOL_ERR;
#ifdef FEATURE_MBMS
        #error code not present
#endif
        sm_continue_pdp_deactivate(pdp_list[con_id]);
        pdp_list[con_id]->current_cause = (cc_cause_T)RAB_SETUP_FAILURE;
      }
      else if (pdp_list[con_id]->rab_state == RABM_MODIFY_PENDING)
      {
        /* -------------------------------------------------------------------
        ** Network may not re-configure the RAB after sending PDP-MODIFY_REQUEST
        ** Set back the RAB-STATE to ACTIVE and DO NOT deactivate the PDP
        ** ------------------------------------------------------------------*/
        pdp_list[con_id]->rab_state = RABM_DEACTIVATED;
        pdp_list[con_id]->pdp_state = PDP_ACTIVE;
        sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, cur_pdp->nsapi.nsapi);
        //send_rabmcm_cmd( pdp_list[con_id].nsapi.nsapi, RABM_RAB_REESTABLISH_REJ );
        /*Set the cause as RAB_SETUP_FAILURE*/
        pdp_list[con_id]->current_cause = (cc_cause_T)RAB_SETUP_FAILURE;
        /*Send Modify Confirm to CM*/
        if (pdp_list[con_id]->cm_pending_msg == (int)SMREG_PDP_MODIFY_REQ)
        {
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
          sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_MODIFY_REJ,
                              SM_INTERNAL_CAUSE);
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
        }
        else
        {
          /*Clear the MT orig flag*/
          pdp_list[con_id]->mt_orig_type = FALSE;
          /*Send Modify Indication to CM*/
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
          sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_MODIFY_IND,
                              SM_INTERNAL_CAUSE);
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
        }

      }
      else if (pdp_list[con_id]->rab_state == RABM_INACTIVE_PENDING)
      {
        MSG_ERROR_DS_0(SM_SUB, "RABM RELEASE response timer expired");
        pdp_list[con_id]->current_cause = (cc_cause_T)RAB_SETUP_FAILURE;
         
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
        if (pdp_list[con_id]->is_primary_pdp && !pdp_list[con_id]->teardown_ind.ti_flag && sm_promote_sec_pri(pdp_list[con_id]))
        {
          pdp_list[con_id]->is_primary_pdp = FALSE;
          // Send a Deactivate CNF message to CM with the appropriate cause
          sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_DEACTIVATE_PROMOTE_CNF, 
                            SM_INTERNAL_CAUSE );
        }
        else
        {
#endif
          if ( cur_pdp->is_primary_pdp
#ifdef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
               && cur_pdp->teardown_ind.ti_flag
#endif
        )
          {
            word index;
            for (index = 0; index < MAX_PDP_NUM; index++)
            {
                if ((pdp_list[index] != NULL) &&
                    cur_pdp->trans_id != index && 
                    cur_pdp->group_id == pdp_list[index]->group_id )
                  {
                    MSG_HIGH_DS_1( SM_SUB,"deactivating TI: %d", pdp_list[index]->trans_id);
                    sm_stop_timer(pdp_list[index]->trans_id);
                    #ifdef FEATURE_WCDMA
                    if(cur_pdp->mt_orig_type == TRUE)
                    {
                      sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_IND, SM_NETWORK_CAUSE);
                    }
                    else
                    {
                      sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_CNF, SM_INTERNAL_CAUSE);
                    }
                    #endif /*#ifdef FEATURE_WCDMA*/
                  }
            }
          }
           if(cur_pdp->mt_orig_type == TRUE)
           {
              // Send a reject message to CM with the appropriate cause
              sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_DEACTIVATE_IND, 
                             SM_NETWORK_CAUSE);
           }
           else
           {           
              // Send a reject message to CM with the appropriate cause
              sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_DEACTIVATE_CNF, 
                             SM_INTERNAL_CAUSE);
           }
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
        }
#endif
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      }
    }
  }
  else if (sm_timer_type == TIMER_EARLY_RAB_FAILURE)
  {
      MSG_HIGH_DS_0(SM_SUB, "EARLY RAB FAILURE timer MAX COUNT expired");
      if (pdp_list[con_id] == NULL)
      {
        MSG_HIGH_DS_1(SM_SUB, "TIMER_EARLY_RAB_FAILURE expired, PDP NULL, TransId: %d",con_id);
        return;
      }
      if (pdp_list[con_id]->pdp_state == PDP_ACTIVE_PENDING)
      {
        MSG_HIGH_DS_1(SM_SUB,"Locally deactivating PDP with NSAPI =%d", cur_pdp->nsapi.nsapi);
        if (is_umts_mode())
        {
          // Inform the RABM that the PDP context is being deactivated
          MSG_HIGH_DS_1(SM_SUB, "Inform RABM PDP Nsapi %d deactivated",cur_pdp->nsapi.nsapi);
          sm_notify_rabm ((int)RABMSM_DEACTIVATE_IMPLICIT_IND, cur_pdp->nsapi.nsapi);
        }
#ifdef FEATURE_GSM_GPRS
        if (cur_pdp->llc_state != LLC_NOT_CONFIRMED && cur_pdp->pdp_state == PDP_INACTIVE_PENDING)
        {
          /*Send SNSM_DEACTIVATE_IND to SNDCP*/
          sm_send_sndcp_cmd (cur_pdp, GSNSM_DEACTIVATE_IND);
        }
#endif
        // Set the PDP state back to PDP_INACTIVE and stop the Establish timer
        cur_pdp->pdp_state = PDP_INACTIVE;
        //Reset the PDP con state
        cur_pdp->pdp_con_state = SIG_CONNECTION_NONE;
        sm_stop_timer (con_id);
        //If no signalling connection is pending clear the PDP timer
        if (!sm_check_mpdp_conn_pend())
        {
          sm_clear_pdp_timer (TIMER_ESTABLISH_PENDING); 
          timer_est_pen_flag = FALSE;
        }

        // Send the DEACTIVATE_CNF to the Call Manager
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
        // Send the DEACTIVATE_CNF to the Call Manager
        if (pdp_list[con_id]->is_primary_pdp && !pdp_list[con_id]->teardown_ind.ti_flag &&
            sm_promote_sec_pri(pdp_list[con_id]))
        {
         //Send the SMREG_PDP_DEACTIVATE_PROMOTE_IND to the CM
         sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_DEACTIVATE_PROMOTE_CNF,
                            SM_INTERNAL_CAUSE );
       }
       else
       {
#endif
         // Send the SMREG_PDP_DEACTIVATE_CNF to the CM.
          sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_DEACTIVATE_CNF, SM_INTERNAL_CAUSE);
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
        }
#endif
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/

        // Close the PS signaling connection if its already setup
        MSG_HIGH_DS_0(SM_SUB, "Send GMM Close Session Request");
        send_gmm_close_session_req();

      }
      else
      {
#ifdef FEATURE_MODEM_HEAP
        pdp_deactivate = modem_mem_calloc(1,sizeof(smreg_pdp_deactivate_req_T),MODEM_MEM_CLIENT_NAS);
#else
        pdp_deactivate = (smreg_pdp_deactivate_req_T *)gs_alloc(sizeof(smreg_pdp_deactivate_req_T));
#endif
        if(pdp_deactivate != NULL)
        {
          pdp_deactivate->nsapi = pdp_list[con_id]->nsapi;
          pdp_deactivate->tear_down_ind = pdp_list[con_id]->teardown_ind;
          pdp_deactivate->config_options = pdp_list[con_id]->config_options;
          pdp_deactivate->as_id = pdp_list[con_id]->as_id;
          sm_pdp_deactivate((byte)con_id, pdp_deactivate);
#ifdef FEATURE_MODEM_HEAP
              modem_mem_free(pdp_deactivate,MODEM_MEM_CLIENT_NAS);
#else
              gs_free(pdp_deactivate);
#endif
              pdp_deactivate = NULL;
         }

      }
  }
  else
  {
    /* ---------------------------------------------------------------
    ** Unkwown timer expired
    ** --------------------------------------------------------------*/
    MSG_ERROR_DS_0(SM_SUB, "Unknown timer Expired");
  }
}


/*===========================================================================

FUNCTION SM_RESERVE_NSAPI

DESCRIPTION
  This function reserves a NSAPI for a given connection_id. The validity of
  connection_id is not checked.
  
DEPENDENCIES
  None

RETURN VALUE
  Reserved NSAPI if available. Else, return INVALID_ID.

SIDE EFFECTS
  None

===========================================================================*/
byte sm_reserve_nsapi ( byte connection_id )
{
  byte i = MAX_NSAPI_NUM;

  rex_enter_crit_sect(&sm_crit_sect);   // just to make sure
  
#ifdef FEATURE_MBMS
  #error code not present
#endif
  for (i=MIN_NSAPI_NUM; i<MAX_NSAPI_NUM; i++)
  {
    if ( nsapi_list[i] == INVALID_ID )  // not used
    {
      nsapi_list[i] = connection_id;    // NSAPI i is used by connection_id
      break;
    }
  }
#ifdef FEATURE_MBMS
  #error code not present
#endif
  rex_leave_crit_sect(&sm_crit_sect);

  if (i == MAX_NSAPI_NUM)
    return INVALID_ID;
  else
    return i;
}

/*===========================================================================
FUNCTION SM_PER_SUBS_RESERVE_NSAPI

DESCRIPTION
  This function reserves a NSAPI for a given connection_id. The validity of
  connection_id is not checked.
  This API can be called by CM directly.
 
DEPENDENCIES
  None

RETURN VALUE
  Reserved NSAPI if available. Else, return INVALID_ID.

SIDE EFFECTS
  None
===========================================================================*/
byte sm_per_subs_reserve_nsapi (byte connection_id, 
                                sys_modem_as_id_e_type as_id)
{

#ifndef FEATURE_DUAL_DATA
  (void) as_id;
  return sm_reserve_nsapi(connection_id);
#else
  byte i = MAX_NSAPI_NUM;
   /* set the received as_id and call the reserve_nsapi */
  if(SM_IS_AS_ID_IN_VALID_RANGE(as_id)==FALSE)
  {
     MSG_HIGH_DS_1(SM_SUB, "Invalid AS_ID %d received. ignore ",as_id);
     return INVALID_ID;
  }
  rex_enter_crit_sect(&sm_crit_sect);   // just to make sure
  
#ifdef FEATURE_MBMS
  #error code not present
#endif
  for (i=MIN_NSAPI_NUM; i<MAX_NSAPI_NUM; i++)
  {
    if ( nsapi_list_sim[as_id][i] == INVALID_ID )  // not used
    {
      nsapi_list_sim[as_id][i] = connection_id;    // NSAPI i is used by connection_id
      break;
    }
  }
#ifdef FEATURE_MBMS
  #error code not present
#endif
  rex_leave_crit_sect(&sm_crit_sect);

  if (i == MAX_NSAPI_NUM)
    return INVALID_ID;
  else
    return i;
#endif
}

/*===========================================================================
FUNCTION SM_PER_SUBS_RELEASE_NSAPI

DESCRIPTION
  This function releases a reserved NSAPI for a given connection_id. The
  validity of connection_id is not checked.
  This is called by CM as well
 
DEPENDENCIES
  None

RETURN VALUE
  NSAPI number if successfully released. -1 if not.

SIDE EFFECTS
  None
===========================================================================*/
int sm_per_subs_release_nsapi (byte connection_id, 
                               sys_modem_as_id_e_type as_id)
{

#ifndef FEATURE_DUAL_DATA
  return sm_release_nsapi(connection_id);
#else
  byte i;
  /* set the received as_id and call the release_nsapi */
  if(SM_IS_AS_ID_IN_VALID_RANGE(as_id)==FALSE)
  {
     MSG_HIGH_DS_1(SM_SUB, "Invalid AS_ID %d received. ignore ",as_id);
     return -1;
  }
  if ( connection_id > MAX_MO_TI)
  {
    return -1; 
  }
  rex_enter_crit_sect(&sm_crit_sect);   // just to make sure

  for (i=MIN_NSAPI_NUM; i<MAX_NSAPI_NUM; i++)
  {
    if ( nsapi_list_sim[as_id][i] == connection_id )   // found it
    {
      nsapi_list_sim[as_id][i] = INVALID_ID;   // NSAPI i is not used anymore
      break;
    }
  }

  rex_leave_crit_sect(&sm_crit_sect);

  if (i==MAX_NSAPI_NUM)
    return -1;
  else
    return i;

#endif
}

/*===========================================================================

FUNCTION SM_RELEASE_NSAPI

DESCRIPTION
  This function releases a reserved NSAPI for a given connection_id. The
  validity of connection_id is not checked.
  
DEPENDENCIES
  None

RETURN VALUE
  NSAPI number if successfully released. -1 if not.

SIDE EFFECTS
  None

===========================================================================*/
int sm_release_nsapi ( byte connection_id )
{
  byte i;

  if ( connection_id > MAX_MO_TI)
  {
    return -1; 
  }
  rex_enter_crit_sect(&sm_crit_sect);   // just to make sure

  for (i=MIN_NSAPI_NUM; i<MAX_NSAPI_NUM; i++)
  {
    if ( nsapi_list[i] == connection_id )   // found it
    {
      nsapi_list[i] = INVALID_ID;   // NSAPI i is not used anymore
      break;
    }
  }

  rex_leave_crit_sect(&sm_crit_sect);

  if (i==MAX_NSAPI_NUM)
    return -1;
  else
    return i;
}

/*===========================================================================

FUNCTION SM_RELEASE_NSAPI_SM

DESCRIPTION
  This function releases a reserved NSAPI for a given connection_id. The
  validity of connection_id is not checked.
  This function should only be called by SM.
DEPENDENCIES
  None

RETURN VALUE
  NSAPI number if successfully released. -1 if not.

SIDE EFFECTS
  None

===========================================================================*/
int sm_release_nsapi_sm ( byte connection_id )
{
  byte i;

  rex_enter_crit_sect(&sm_crit_sect);   // just to make sure

  for (i=MIN_NSAPI_NUM; i<MAX_NSAPI_NUM; i++)
  {
    if ( nsapi_list[i] == connection_id )   // found it
    {
      nsapi_list[i] = INVALID_ID;   // NSAPI i is not used anymore
      break;
    }
  }

  rex_leave_crit_sect(&sm_crit_sect);

  if (i==MAX_NSAPI_NUM)
    return -1;
  else
    return i;
}
/*===========================================================================

FUNCTION SM_RESELEASE_ALL_NSAPI

DESCRIPTION
  This function releases all the nsapi in nspai list
  This function should only be called by SM.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int sm_release_all_nsapi ()  
{

  byte i;
  rex_enter_crit_sect(&sm_crit_sect);   // just to make sure

  for (i=MIN_NSAPI_NUM; i<MAX_NSAPI_NUM; i++)
  {
      nsapi_list[i] = INVALID_ID;   // NSAPI i is not used anymore
  }

  rex_leave_crit_sect(&sm_crit_sect);
  return TRUE;
}
/*===========================================================================

FUNCTION GET_TRANSID

DESCRIPTION
  This function replaces macro GET_TRANSID. Gets transaction id for a NSAPI
  
DEPENDENCIES
  None

RETURN VALUE
  TI if successfully, INVALID_ID if not.

SIDE EFFECTS
  None

===========================================================================*/
int GET_TRANSID (byte nsapi)
{
  return nsapi_list[nsapi];
}

#ifdef FEATURE_DUAL_DATA
/*===========================================================================

FUNCTION GET_TRANSID

DESCRIPTION
  This function replaces macro GET_TRANSID. Gets transaction id for a NSAPI
  
DEPENDENCIES
  None

RETURN VALUE
  TI if successfully, INVALID_ID if not.

SIDE EFFECTS
  None

===========================================================================*/
int GET_TRANSID_PER_SUB (byte nsapi, sys_modem_as_id_e_type as_id)
{
  return nsapi_list_sim[as_id][nsapi];
}
#endif

/*===========================================================================

FUNCTION GET_NSAPI

DESCRIPTION
  This function replaces macro GET_NSAPI. Gets nsapi from connectio id.
  
DEPENDENCIES
  None

RETURN VALUE
  NSAPI if successfully, INVALID_ID if not.

SIDE EFFECTS
  None

===========================================================================*/
byte GET_NSAPI (byte conn_id)
{
  return pdp_list[conn_id]->nsapi.nsapi;
}


/*===========================================================================

FUNCTION SM_INIT_PDP_CONTEXT

DESCRIPTION
  Initialize a PDP context.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sm_init_pdp_context (pdp_context_info *pdp)
{
  /*Clear the RAB state*/
  if (pdp->nsapi.valid)
  {
    rabm_init_rabid(pdp->nsapi.nsapi);
  }
  pdp->apn_name.valid       = FALSE;
  pdp->cm_pending_msg       = UNKNOWN_CMD;
  pdp->config_options.valid = FALSE;
  pdp->current_cause        = (cc_cause_T)PPD_UNKNOWN_REASON;
  pdp->cause_type           =  SM_NETWORK_CAUSE;
#ifdef FEATURE_GPRS_COMP_ENUM_TYPES
  pdp->d_comp               = SYS_PDP_DATA_COMP_OFF;
#else
  pdp->d_comp               = FALSE;
#endif
  pdp->flow_id.valid        = FALSE;
#ifdef FEATURE_GPRS_COMP_ENUM_TYPES
  pdp->h_comp               = SYS_PDP_HEADER_COMP_OFF;
#else
  pdp->h_comp               = FALSE;
#endif
  pdp->is_primary_pdp       = FALSE;
  pdp->linked_ti            = INVALID_ID;
  pdp->llc_sapi.valid       = FALSE;
  pdp->llc_state            = LLC_NOT_CONFIRMED;
  pdp->minimum_qos.valid    = FALSE;
  pdp->mt_orig_type         = FALSE;
  pdp->nsapi.valid          = FALSE;
  pdp->pdp_addr.valid       = FALSE;
  pdp->pdp_con_state        = SIG_CONNECTION_NONE;
  pdp->pdp_state            = PDP_INACTIVE;
  pdp->qos.valid            = FALSE;
  pdp->qos_valid            = FALSE;
  pdp->rab_state            = RABM_INITIAL;
  pdp->radio_pri.valid      = FALSE;
  pdp->teardown_ind.valid   = FALSE;
  pdp->tft.valid            = FALSE;
  pdp->group_id             = INVALID_ID;
  pdp->trans_id             = INVALID_ID;  
  #ifdef FEATURE_MBMS
  #error code not present
#endif
#ifdef FEATURE_DUAL_SIM
  pdp->as_id = SYS_MODEM_AS_ID_NONE;
#endif
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
  pdp->sm_msg_to_resend   = (sm_layer_msg_T)0xFF;
  pdp->status_ind_flag = FALSE;
#endif
#ifdef FEATURE_LTE
  pdp->esm_conn_id = INVALID_ID;
  pdp->isr_pdp_act_status = CTXT_ACT_BEFORE_ISR;
#endif /*FEATURE_LTE*/

  pdp->context_id             = INVALID_ID;  
  pdp->assoc_context_id       = INVALID_ID;  
  pdp->sm_gprs_type3_timer.active       = TRUE;
  pdp->sm_gprs_type3_timer.timer_count = 0; 
  

}


/*===========================================================================
FUNCTION GMMSM_PROCESS_RECEIVE_NPDU_NUMBER_LIST

DESCRIPTION
  SM function for use by GMM to process a Receive NPDU Number List IE in a 
  single threaded process
  
  The NPDU list is passed as list pointers to valid flags, and Receive NPDU
  numbers. List length must be 11 covering NSAPI5 to NSAPI15.
  
  The data at the location given by the specified pointers is modified to
  return the exchange NPDU Number list for transmission.

  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_GSM_GPRS
void gmmsm_process_receive_npdu_number_list
(
  boolean *valid_list_p,
  uint8   *npdu_num_list_p
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
)
{
  /* This function will be called from MM Task context. */

  
 sm_process_receive_npdu_number_list( valid_list_p, npdu_num_list_p
#ifdef FEATURE_DUAL_SIM
  ,as_id
#endif
  );
    

} /* end of gmmsm_process_receive_npdu_number_list */
#endif /* FEATURE_GSM_GPRS */

