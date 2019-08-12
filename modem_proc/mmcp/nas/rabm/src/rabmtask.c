/*===========================================================================
                        R A B M   S U B   T A S K  

DESCRIPTION

   This file contains the task entry point for RABM sub-task. It implements 
   the RABM task initialization and other RABM functionality.


EXTERNALIZED FUNCTIONS   
   rabm_task()
        It is the main entry point for RABM sub Task.

   rabm_put_cmd()
        This function is used by other entities to put a rabm_cmd_type.

   rabm_process_cmd()
        This function is used by SM task for processing the RABM commands.

   rabm_get_cmd_buf()
        This function fetches a rabm_cmd_type*, used by external entities.

   rabm_rabid_validate()
        The RRC task uses this function to query the validity of a RAB
        during RAB establishment/release.

        
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rabm/vcs/rabmtask.c_v   1.12   18 Jul 2002 15:50:54   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/rabm/src/rabmtask.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/16/13   am      Passing internal cause for internal error while RAB reestablishment
10/01/12   am      Back off timer code
02/24/12   us      Fixing compilation errors 
02/16/12   us      Adding new field in interface structure-> "rabm_rab_reestablish_ind_T" to send rat mode in RAB ESTABLISHMENT INDICATOR   
12/08/11   am    Sending RAB release IND to active RABs during RAT change
03/06/07   vsm   Updated the code to send the gmmrabm_rab_rel_ind message when AS releases a RAB
                           for an active PDP.
08/14/06   pk      Updated to log CFA packet for messages to/from RABM.
12/27/06   ss      Not deactivating PDP for cause #13 and #15
11/05/06   ss      Only Service Procedure needs to aborted when Service Reject happens for cause #17
11/21/05   kvk     Now sending the traffic_class with the primitve GMMRABM_REESTABLISHMENT_REQ
09/21/05   kvk    Now sending the RRC establihment cause depending on the 
                  traffic class of the "most demanding" RAB for RAB re-establishment 
 09/13/05  kvk     MO PDP Modify supported. Now handling the Application initiated
                   PDP Modification for QoS and TFT.
08/26/05   kvk     RAB state will be moved INACTIVE in case PDP Modify is rejected when
                   RAB reestablish procedure is pending.
08/22/05   kvk     Now PDP contexts will be locally deactivated in case RABM receives 
                   GMMRABM_REESTABLISH_REJ with cause ILLEGAL_ME and NETWORK_FAILURE.
07/22/05   kvk     Now RAB_REESTABLISH_REQ will be honored in case RAB reestablishment procedure
                   is already pending. For Streaming/Conversational RAB PDP Modify procedure will be 
                   triggered rightaway in case pmm state is pmm_conneceted, even Non stream/conv RAB
                   is waiting for RAB to be re established.
06/30/05   kvk     New function RABM_CHECK_RAB_REEST_PEND defined to verify the 
                   RAB state is RAB_RESTABLISH_PENDING.
05/16/05   kvk     Fixed passing wrong command to TC when RABM receives RABMAS_RAB_REL_IND
                   from RRC. Fixed lint warnings/errors.
05/12/05   kvk     New function RABM_CHECK_RAB_ACTIVE is introduced to see if
                   any of the RAB is active to handle validation. RAB validation will
                   be passed if any of the RAB is active to avoid interruption for active RABs.
03/31/05   kvk     New function rabm_init_rabid() defined to clear the RAB state
03/10/05   kvk     Handling RAB query when PDP is in PDP_ACTIVE_PENDING state.
01/31/04   kvk     RABMSM_ACTIVATE_RSP is sent rightaway when RAB is ACTIVE.
11/23/04   kvk     MT PDP support added
09/20/04   kvk     CR 49647:RAB query is honoured even the PDP state is INACTIVE PENDING
                   to avoid RAB setup failures in case combined RB set procedure.
                   CR 49550: Fixed duplicate Service request triggered by RABM before
                   RB setup is indicated by network.
09/02/04   kvk     Added handling for sec pdp. Now posting the RAB_REESTABLISH_IND to SM
                   to trigger the PDP context modification in case of streaming and conversation
                   RAB reestablishment.
08/27/04   kvk     DS triggering the RAB reestablishment in case PDP is INACTIVE PENDING
                   state is blocked. If RAB is already active RAB EST IND will be passed
                   to RABM instead of triggering RAB reestablishment 
07/23/04   kvk     During RAB reestablishment indicate upper layer with RAB_REETABLISH_REJ
                   when GMM connection to move RAB to dormant state.
06/28/04   kvk     Fixed the wrong handling of SIG CON REL pdp ACTIVE PENDING state.
06/21/04   kvk     Fixed the wrong state checking for RRC RAB RELEASE IND to inform CM
06/07/04   kvk     Added support for processing RABM_DEACTIVATE_IMPLICIT_IND to deactivate
                   RABM locally
06/03/04   kvk     While processing the RABMAS_RAB_RELEASE_IND from RRC the RABM_RAB_RELEASE_IND
                   is passed when RAB state is checked for ACTIVE instead of PDP state.
05/25/04   kvk     New interface is used replacing the old timer, Temporary hack 
                   is removed for proceesing CM command and timer expiry is handled.
03/01/04   ks      Now Locally deactivating the PDP for various REJ reasons
                   in the GMM service reject during the RAB-REESTABLISH_REQ.
02/19/04   ks      Validating the RAB establishment as success when RAB state
                   is in RAB_REESTABLISH_PENDING.
06/18/03   ks      Fixed the connection_id in the RAB_REESTABLISH_RSP/REJ
                   primitives to CM and also added more debug statements.
04/22/03   ks      Calling sm_perform_local_deactivation() to deactivate local
                   PDP contexts when NW reject cause is NO_PDP_CONTEXT_ACTIVATED.
02/28/03   ks      Added cause field in gmmrabm_reestablish_req_T primitive.
                   Setting the right RRC cause during connection establishment.
02/27/03   mks     Added support for PS data continuity feature - Now processing
                   RAT suspension/resumption notifications from SM layer.
09/04/02   ks      Setting the RAB state of to RAB_INACTIVE after receiving
                   RABM_RAB_REESTABLISH_REJ from GMM.
07/18/02   ks      Replaced cmll.h with cmwll.h
04/22/02   ks      Added support for PDP preservation procedures (RAB release/
                   re-establish for an active PDP context) during PS calls.
03/25/02   ks      Added missing break statement in rabm_process_cmd() 
01/30/02   ks      Added support for Test control (TC) entity
09/16/01   ks      Converting the rab_id from 8bit value to 4bit NSAPI value
                   in function rabm_rabid_validate().
08/27/01   ks      Removed the redundant variables sm_tcb and tmc_heap.
08/01/01   ks      Added a function rabm_rab_established(), for SM to check
                   the current state of the RAB of an associated PDP context.
06/04/01   ks      Added 'MS' prefix to all the RABM 'message_set' values.
05/23/01   ks      Added more comments and fixed few DM output statements.
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
#include "msg.h"
#include "err.h"
#include "rex.h"
#include "smtask.h"
#include "smtask_v.h"
#include "rabmtask_v.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "cmwll.h"
#include "memheap.h"
#include "ms.h"
#include "sm_v.h"
#include "ds3gpp_api.h"
#include "fs_lib.h"
#include "mcfg_fs.h"
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
#include "cfa_v.h"
#include "smix_v.h"
#include "bit.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include <stringl/stringl.h>

#ifdef NAS_TEST_FRAMEWORK
#include <tf_stub.h>

/* Declare the interceptor stubs
*/
TF_PROTO_STUB( void, cm_put_cmd, cm_rpt_type* );
#define cm_put_cmd      TF_STUB(cm_put_cmd)
TF_PROTO_STUB( cm_rpt_type*, cm_get_cmd_buf,void );
#define cm_get_cmd_buf  TF_STUB(cm_get_cmd_buf)

#endif /* TEST_FRAMEWORK */
/* -----------------------------------------------------------------------
** Extern Declarations
** ----------------------------------------------------------------------- */

extern sm_cm_status_T mm_convert_gmm_to_sm_cause(byte gmm_cause);

extern void sm_unset_ps_vote_for_reselection(void);

extern rex_tcb_type* sm_tcb_ptr;

#if defined(FEATURE_DUAL_SIM)
extern boolean SM_IS_VALID_AS_ID(sys_modem_as_id_e_type cmd_as_id);
#endif

#ifdef FEATURE_DUAL_DATA
extern byte rabm_timer_status_sim[MAX_AS_IDS][MAX_RAB_NUM];
#define rabm_timer_status rabm_timer_status_sim[sm_as_id]

extern boolean sm_ps_vote_for_reselection_sim[MAX_AS_IDS];
#define sm_ps_vote_for_reselection sm_ps_vote_for_reselection_sim[sm_as_id]

extern sys_modem_as_id_e_type  sm_get_multimode_sub(void);
#endif

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define INVALID_RAB      0xff

extern boolean is_ps_wcdma_mode( sys_modem_as_id_e_type as_id );

extern boolean is_ps_tdscdma_mode( sys_modem_as_id_e_type as_id );

#ifdef FEATURE_DUAL_SIM
#define is_wcdma_mode() is_ps_wcdma_mode((sys_modem_as_id_e_type)sm_as_id)
#else
#define is_wcdma_mode() is_ps_wcdma_mode(SYS_MODEM_AS_ID_1)
#endif

#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_DUAL_SIM
#define is_tdscdma_mode() is_ps_tdscdma_mode((sys_modem_as_id_e_type)sm_as_id)
#else
#define is_tdscdma_mode() is_ps_tdscdma_mode(SYS_MODEM_AS_ID_1)
#endif
#endif

#ifdef FEATURE_TC
extern void sm_stop_timer( word timer_id ); 
#endif

extern void rabm_send_rab_est_ind_to_gmm(void);

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */


// Enumeration for state machine for an individual RAB.
typedef enum
{
  RAB_INACTIVE,
  RAB_ACTIVE,
  RAB_ACTIVE_PENDING,
  RAB_INACTIVE_PENDING,
  RAB_MODIFY_PENDING,
  RAB_SERVICE_REQ_PENDING,
  RAB_SERVICE_REQ_SENT,
  RAB_REESTABLISH_PENDING  
} rab_est_state_T;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
q_type rabm_cmd_q;    /* CMD QUEUE for SM */

// Structure that stores RAB information.
typedef struct
{
  word              rab_id;
  rab_est_state_T   rab_state;
} rab_info_T;

#ifdef FEATURE_DUAL_DATA

LOCAL rab_info_T rab_list_sim[MAX_AS_IDS][MAX_RAB_NUM];
#define rab_list rab_list_sim[sm_as_id]

#else /* #ifdef FEATURE_DUAL_DATA*/
  
// List of all the RABs
LOCAL rab_info_T rab_list[MAX_RAB_NUM];

#endif /*#ifdef FEATURE_DUAL_DATA */

LOCAL boolean ue_in_test_mode;  // TRUE = Test mode
                                // FALSE = Normal mode

/* -----------------------------------------------------------------------
** Local Function Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** FUNCTION     : RABM_PROCESS_RRC_CMD()
** DESCRIPTION  : This function processes commands sent by RRC.
** ----------------------------------------------------------------------- */
LOCAL void rabm_process_rrc_cmd( rabm_cmd_type* cmd_ptr );

/* -----------------------------------------------------------------------
** FUNCTION     : RABM_PROCESS_SM_CMD()
** DESCRIPTION  : This function processes commands sent by SM.
** ----------------------------------------------------------------------- */
LOCAL void rabm_process_sm_cmd( rabm_cmd_type* cmd_ptr );

/* -----------------------------------------------------------------------
** FUNCTION     : RABM_PROCESS_GMM_CMD()
** DESCRIPTION  : This function processes commands sent by GMM.
** ----------------------------------------------------------------------- */
LOCAL void rabm_process_gmm_cmd(rabm_cmd_type* cmd_ptr ); 

/* -----------------------------------------------------------------------
** FUNCTION     : RABM_PROCESS_CM_CMD()
** DESCRIPTION  : This function processes commands sent by CM.
** ----------------------------------------------------------------------- */
LOCAL void rabm_process_cm_cmd(rabm_cmd_type* cmd_ptr ); 

/* -----------------------------------------------------------------------
** FUNCTION     : RABM_PROCESS_RAT_CHANGE_IND()
** DESCRIPTION  : This function processes RAT change notification from SM
** ----------------------------------------------------------------------- */
LOCAL void rabm_process_rat_change_ind (void);

/* ------------------------------------------------------------------------
** FUNCTION     : RABM_PROCESS_RAT_RESUME_IND()
** DESCRIPTION  : This function processes RAT resumption indication from SM
** ------------------------------------------------------------------------ */
LOCAL void rabm_process_rat_resume_ind (void);

/* ------------------------------------------------------------------------
** FUNCTION     : RABM_CHECK_RAB_REEST_PEND()
** DESCRIPTION  : This function returns TRUE if any of RAB state 
                  is RAB_REESTABLISH_PENDING in RABM 
** ------------------------------------------------------------------------ */
boolean rabm_check_rab_reest_pend(void);

/* ------------------------------------------------------------------------
** FUNCTION     : RABM_CHECK_SERV_REQ_COMPLETE_PENDING()
** DESCRIPTION  : This function returns TRUE if any of RAB state 
                  is RAB_SERVICE_REQ_SENT in RABM 
** ------------------------------------------------------------------------ */
boolean rabm_check_serv_req_complete_pending(void);


/* ------------------------------------------------------------------------
** FUNCTION     : RABM_SEND_GMMRABM_REESTABLISH_REQ()
** DESCRIPTION  : This function send GMMRABM_REESTABLISH_REQ to GMM task
** ------------------------------------------------------------------------ */
LOCAL void rabm_send_gmmrabm_reestablish_req(byte rab_id);

/* ------------------------------------------------------------------------
** FUNCTION     : RABM_SEND_COMBINED_GMMRABM_REESTABLISH_REQ()
** DESCRIPTION  : This function send GMMRABM_REESTABLISH_REQ to GMM task
** ------------------------------------------------------------------------ */
LOCAL void rabm_send_combined_gmmrabm_reestablish_req(void);


#ifdef FEATURE_TC

/* -----------------------------------------------------------------------
** FUNCTION     : rabm_process_tc_cmd()
** DESCRIPTION  : Sends a command to TC task.
** ----------------------------------------------------------------------- */
LOCAL void rabm_process_tc_cmd( rabm_cmd_type* cmd_ptr );

/* -----------------------------------------------------------------------
** FUNCTION     : send_rabmtc_cmd()
** DESCRIPTION  : Sends a command to TC task.
** ----------------------------------------------------------------------- */
LOCAL void send_rabmtc_cmd(rabm_cmd_type* cmd_ptr );

#endif // FEATURE_TC

/* -----------------------------------------------------------------------
** FUNCTION     : send_rabmsm_cmd()
** DESCRIPTION  : Sends a command to SM task.
** ----------------------------------------------------------------------- */
void send_rabmsm_cmd(byte rab_id, sm_cmd_enum_type sm_enum_type);

/* -----------------------------------------------------------------------
** FUNCTION     : send_rabmcm_cmd()
** DESCRIPTION  : Sends a command to CM task.
** ----------------------------------------------------------------------- */
LOCAL void send_rabmcm_cmd(byte rab_id, rabm_cm_enum_T rabm_cmd, cc_cause_T cause );

                                                                                                                                                                                                                                                                        
/* -----------------------------------------------------------------------
** FUNCTION     : process_sig_connection_rel()
** DESCRIPTION  : Processes the RRC signaling Release sent by SM task.
** ----------------------------------------------------------------------- */
LOCAL   void process_sig_connection_rel (void);

                                         
/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* -----------------------------------------------------------------------
** FUNCTION     : RABM_GET_3GPP_GLOBAL_THROTTLING_ENABLED()
** DESCRIPTION  : This function returns TRUE if  3GPP_GLOBAL_THROTTLING NV is enabled
** ----------------------------------------------------------------------- */

boolean rabm_get_3gpp_global_throtling_enabled(void)
{
  const char *file_path = "/nv/item_files/modem/data/3gpp/global_throttling";
  
  mcfg_fs_status_e_type result = MCFG_FS_STATUS_EFS_ERR;
  ds_3gpp_global_throttle_s global_throttle_nv;
  memset((void*)&global_throttle_nv,0,sizeof(global_throttle_nv));
  result = mcfg_fs_read(file_path, (void *)(&global_throttle_nv), sizeof(global_throttle_nv), 
                         MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)0);
  if(result != MCFG_FS_STATUS_OK)
  {
          MSG_ERROR_DS_1( RABM_SUB, "reading global_throttling NV failed, status = %d", result);
  }
  return global_throttle_nv.global_throttling;
}
/*===========================================================================

FUNCTION RABM_INIT_BEFORE_TASK_START

DESCRIPTION
  This function does the initialization for RABM task. It means initializing 
  all RABM global data.  It is called by the SM task.   
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rabm_init_before_task_start ( void )
{
  byte i;

  /* Initialize all the command queues */
  (void)q_init(&rabm_cmd_q);

#ifdef FEATURE_DUAL_DATA
  for ( sm_as_id = 0; sm_as_id < (sm_as_id_type_e)MAX_AS_IDS; sm_as_id ++ )
  {
#endif
    for(i=0; i<MAX_RAB_NUM; i++)
    {
      rab_list[i].rab_id = INVALID_RAB;
    }
#ifdef FEATURE_DUAL_DATA
  }
#endif
}
/*===========================================================================

FUNCTION RABM_INIT

DESCRIPTION
  This function does the initialization for RABM task. It means initializing 
  all RABM global data.  It is called by the SM task.   
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rabm_init( void )
{
  word i;

#ifdef FEATURE_DUAL_DATA
   for ( sm_as_id = SM_AS_ID_1; sm_as_id < (sm_as_id_type_e)MAX_AS_IDS; sm_as_id ++ )
   {
#endif
    // Initialize all the RABs in the list as invalid
    for(i=0; i<MAX_RAB_NUM; i++)
    {
      rab_list[i].rab_id = INVALID_RAB;
      rab_list[i].rab_state = RAB_INACTIVE;
      rabm_timer_status[i] = (byte)RABM_TIMER_STOPPED;
    }

    // Set the RABM mode of operation to UE NORMAL mode.
    ue_in_test_mode = FALSE;

#ifdef FEATURE_DUAL_DATA  
  }
#endif

  /* Any other initializations will go here */
}

/*===========================================================================

FUNCTION RABM_INIT_RABID

DESCRIPTION
  This function does the initialization of the RAB
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rabm_init_rabid(word rab_id)
{
  rab_list[rab_id].rab_id = INVALID_RAB;
  rab_list[rab_id].rab_state = RAB_INACTIVE;
}

/*===========================================================================

FUNCTION RABM_CHECK_RAB_ACTIVE

DESCRIPTION
  This function checks whether any of the RAB is INACTIVE
DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean rabm_check_rab_active(void)
{
  byte i;
  /*Check any of the RAB is active*/
  for(i=MIN_RAB_NUM; i<MAX_RAB_NUM; i++)
  {
    if (rab_list[i].rab_state != RAB_INACTIVE)
    {
      return TRUE;
    }
  }
  return FALSE;
}

#ifdef FEATURE_DUAL_DATA
/*===========================================================================

FUNCTION RABM_CHECK_RAB_ACTIVE

DESCRIPTION
  This function checks whether any of the RAB is INACTIVE
DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean rabm_check_rab_active_per_sub(sys_modem_as_id_e_type as_id)
{
  byte i;
  /*Check any of the RAB is active*/
  for(i=MIN_RAB_NUM; i<MAX_RAB_NUM; i++)
  {
    if (rab_list_sim[as_id][i].rab_state != RAB_INACTIVE)
    {
      return TRUE;
    }
  }
  return FALSE;
}
#endif

/*===========================================================================

FUNCTION RABM_PUT_CMD

DESCRIPTION
  his function puts a command buffer into RABM command queue.
  Note that the command buffer must have been requested through a 
  call to get_rab_cmd_buf(). Also, the calling function must have 
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
void rabm_put_cmd (rabm_cmd_type * cmd_buf)
{
   if ((cmd_buf->header.message_set == MS_CM_RABM) ||
       (cmd_buf->header.message_set == MS_RABM_TDSRRC) ||
       (cmd_buf->header.message_set == MS_RABM_RRC))
   {
     /* Log using CFA packet */
     cfa_log_packet_ex( GS_QUEUE_SM, 
                      cmd_buf->header.message_set,
                      (byte )cmd_buf->header.cmd_id,
                      MIN(250,sizeof(cmd_buf->cmd)),
                      (byte*)&(cmd_buf->cmd) );
   }     

   MSG_LOW_DS_0(RABM_SUB," Putting CMD in rabm_cmd_q ");

  /* Initialize the queue link before putting an item in a queue. */
  (void) q_link(cmd_buf, &(cmd_buf->header.link));

  /* Then put the command buffer in the queue */
  q_put(&rabm_cmd_q, &(cmd_buf->header.link));

  /* Then set the signal for this queue */
  (void) rex_set_sigs(sm_tcb_ptr, RABM_CMD_Q_SIG);
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================
FUNCTION RABM_GET_CMD_BUF

DESCRIPTION

  This function returns a RABM command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer is freed by calling rabm_free_cmd_buf(). 

  The freeing is done by the function that dequeues and processes this
  command buffer. That means this is done by RABM subtask.
  
  Calling function MUST check for a NULL return value and take necessary
  action.
 
DEPENDENCIES

  None.
  
RETURN VALUE

  A buffer of type rabm_cmd_type. NULL is returned if memory is unavailable.

SIDE EFFECTS

  The caller must make sure the returned rabm_cmd_type* is not NULL.
  
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
rabm_cmd_type *rabm_get_cmd_buf( void )
{
  rabm_cmd_type *ptr;

  /* Allocating memory for RABM Command buffer from the TMC heap */
#ifdef FEATURE_MODEM_HEAP      
  if( (ptr = (rabm_cmd_type *)modem_mem_calloc(1, sizeof(rabm_cmd_type), MODEM_MEM_CLIENT_NAS)) == NULL )
  {
    MSG_ERROR_DS_0(RABM_SUB,"Could not allocate memory for RAB cmd ");
  }
#else
  if( (ptr = (rabm_cmd_type *)mem_malloc(&(tmc_heap), 
                            sizeof(rabm_cmd_type))) == NULL )
  {
    MSG_ERROR_DS_0(RABM_SUB,"Could not allocate memory for RAB cmd ");
  }
  memset(ptr,0x00,sizeof(rabm_cmd_type));
#endif

  return(ptr);
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================
FUNCTION RABM_GET_AS_ID

DESCRIPTION
  retrieves the AS_ID of the received message
     
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

  Note: This runs under the SM task context.
===========================================================================*/
boolean rabm_get_as_id( rabm_cmd_type* cmd_ptr )
{
  boolean status = TRUE;
  sys_modem_as_id_e_type  as_id = SYS_MODEM_AS_ID_NONE;

  switch( cmd_ptr->header.message_set )
  {
    case MS_RABM_RRC:
    case MS_RABM_TDSRRC:
      switch (cmd_ptr->header.cmd_id)
      {
        case RABMAS_RAB_ESTABLISH_IND:
          as_id = cmd_ptr->cmd.rrc_establish_ind.as_id;
          break;

        case RABMAS_RAB_RELEASE_IND:
          as_id = cmd_ptr->cmd.rrc_release_ind.as_id;
          break;      

        case RABMAS_STATUS_IND:
          as_id = cmd_ptr->cmd.rrc_status_ind.as_id;
          break;

        case RABMAS_FORCE_CLOSE_SESSION_IND:
          as_id = cmd_ptr->cmd.rabmas_force_close_session_ind.as_id;
          break;

        default:
          break;
      }
      break;

    case MS_RABM_SM:
      switch (cmd_ptr->header.cmd_id)
      {
        case RABMSM_ACTIVATE_IND:
          as_id = cmd_ptr->cmd.sm_activate_ind.as_id;
          break;

        case RABMSM_DEACTIVATE_IND:
          as_id =  cmd_ptr->cmd.sm_deactivate_ind.as_id;
          break;

        case RABMSM_DEACTIVATE_IMPLICIT_IND:
          as_id =  cmd_ptr->cmd.sm_deactivate_ind.as_id;
          break;

        case RABMSM_MODIFY_IND:
          as_id =  cmd_ptr->cmd.sm_modify_ind.as_id;
          break;

       case RABMSM_SIG_CON_RELEASE_IND:
         as_id =  cmd_ptr->cmd.sm_release_ind.as_id;
         break;

       case RABMSM_RAT_CHANGE_IND:
         as_id =  cmd_ptr->cmd.sm_rat_change_ind.as_id;
         break;

       case RABMSM_RAT_RESUME_IND:
         as_id =  cmd_ptr->cmd.sm_rat_resume_ind.as_id;
         break;

       default:
          break;
      }
    break;

    case MS_GMM_RABM:
      switch (cmd_ptr->header.cmd_id)
      {
        case GMMRABM_REESTABLISH_RSP:
          as_id = cmd_ptr->cmd.gmm_reestablish_rsp.as_id;
          break;

        case GMMRABM_REESTABLISH_REJ:
          as_id = cmd_ptr->cmd.gmm_reestablish_rej.as_id;
          break;

        default:
          break;
      }
    break;

    case MS_CM_RABM:
      switch (cmd_ptr->header.cmd_id)
      {
        case RABM_RAB_REESTABLISH_REQ:
          as_id = cmd_ptr->cmd.rabm_reestablish_req.as_id;
          break;

        default:
          break;
      }
      break;           
    
#ifdef FEATURE_TC
    case MS_RABM_TC:
#ifdef FEATURE_DUAL_DATA
    /*When Dual Data is enabled, Treat the incoming TC Message as id as sm_as_id as TC is not having as_id in RABM TC message */
    as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif 
    break;
#endif

  case MS_TIMER:
#ifdef FEATURE_DUAL_DATA
       as_id = (sys_modem_as_id_e_type)((cmd_ptr->cmd.timer_expiry.timer_id & 0x10)>>4);
#else 
       as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
       break;

  default:
      break;
  }/* end switch */

  sm_add_message_to_debug_buffer(cmd_ptr->header.message_set,
                                 cmd_ptr->header.cmd_id, 
                                 TO_RABM, as_id);

#ifdef FEATURE_DUAL_DATA
  /*Non-Dual Data, validation is taken care in the message processing */
  sm_as_id = (sm_as_id_type_e)as_id;
  status = SM_IS_VALID_AS_ID(as_id);
#endif 
  return status;
}


/*===========================================================================

FUNCTION RABM_PROCESS_CMD

DESCRIPTION

  This function is called by the SM task for processing any RABM commands 
  that are put in the RABM_CMD_QUEUE. 
     
DEPENDENCIES

  None.
  
RETURN VALUE

 
SIDE EFFECTS

  Note: This runs under the SM task context.
  
===========================================================================*/
void rabm_process_cmd( rabm_cmd_type* cmd_ptr )
{  
  /* -------------------------------------------------------------
  ** Switch on the command id of the received command, and
  ** call the appropriate function to process each command. 
  ** -----------------------------------------------------------*/
  switch( cmd_ptr->header.message_set )
  {
    case MS_RABM_RRC:
    case MS_RABM_TDSRRC:
      rabm_process_rrc_cmd( cmd_ptr );
    break;

    case MS_RABM_SM:
      rabm_process_sm_cmd( cmd_ptr );
    break;

    case MS_GMM_RABM:
      rabm_process_gmm_cmd( cmd_ptr );
    break;

    case MS_CM_RABM:
      rabm_process_cm_cmd( cmd_ptr );
    break;           
    
#ifdef FEATURE_TC
    case MS_RABM_TC:
      rabm_process_tc_cmd( cmd_ptr );
    break;
#endif
  case MS_TIMER:
       rabm_process_timer_expiry_cmd(cmd_ptr);
       break;
    default:
      /* Wrong command received on the SM CMD Q */
      MSG_ERROR_DS_1( RABM_SUB, "Unknown cmd received by RABM %d", 
            cmd_ptr->header.cmd_id);
    break;
  }/* end switch */

  /* The command has been processed. Free the memory for this cmd */
  /*lint -e424*/
#ifdef FEATURE_MODEM_HEAP       
   modem_mem_free(cmd_ptr, MODEM_MEM_CLIENT_NAS);
#else
   mem_free( &(tmc_heap), cmd_ptr );
#endif
  /*lint -e424*/
 }

/*===========================================================================
FUNCTION RABM_PROCESS_RRC_CMD 

DESCRIPTION
  This function processes command sent by the RRC task from Access Stratum.
  RRC will inform RABM in case of a successful RAB setup or a RAB setup 
  failure.  RABM will take appropriate actions based on the command received.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void rabm_process_rrc_cmd( rabm_cmd_type* cmd_ptr )
{
  byte rab_id = 0;
  boolean change_state = FALSE;
  mm_cmd_type * gmm_cmd;

  MSG_MED_DS_0( RABM_SUB,"RABM Received RRC CMD");

#ifdef FEATURE_TC
  /* -----------------------------------------------------------------/
  ** If the RABM is in TEST mode of operation, route the notifications
  ** to the TC task.
  * ----------------------------------------------------------------*/
  if (ue_in_test_mode)
  {
    send_rabmtc_cmd(cmd_ptr);
    return;
  }
#endif /* FEATURE_TC */

#ifdef FEATURE_MBMS
  #error code not present
#endif

  switch (cmd_ptr->header.cmd_id)
  {
    case RABMAS_RAB_ESTABLISH_IND:
      // Make sure the corresponding RAB is in RAB_ACTIVE_PENDING
      rab_id =  cmd_ptr->cmd.rrc_establish_ind.rabid;  
      if (rab_id<MIN_RAB_NUM || rab_id>=MAX_RAB_NUM)
      {
        MSG_HIGH_DS_1(RABM_SUB,"RABM Received RRC CMD: RAB Estb Ind: Invalid rab_id:%d", rab_id);
        break;
      }
#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
     if(!( SM_IS_AS_ID_IN_VALID_RANGE(cmd_ptr->cmd.rrc_establish_ind.as_id) == TRUE &&
         (SM_IS_VALID_AS_ID(cmd_ptr->cmd.rrc_establish_ind.as_id) == TRUE)))
	 {
       MSG_ERROR_DS_2( RABM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , cmd_ptr->cmd.rrc_establish_ind.as_id);
       return ;
	 }
#endif
#endif

      if (GET_TRANSID(rab_id) == INVALID_ID || pdp_list[GET_TRANSID(rab_id)] == NULL)
      {
        MSG_HIGH_DS_2(RABM_SUB,"RABM Received RRC CMD: RAB Estb Ind: Invalid Transaction Id- %d for RAB id- %d", GET_TRANSID(rab_id), rab_id);
        rab_list[rab_id].rab_state = RAB_INACTIVE;
        break;
      }

      MSG_HIGH_DS_3(RABM_SUB,"RABM Received RRC CMD: Received RAB ESTABLISH IND  for Transaction Id- %d, RAB id- %d, RAB state- %d", GET_TRANSID(rab_id), rab_id,rab_list[rab_id].rab_state);

  
      if (rab_list[rab_id].rab_state == RAB_ACTIVE_PENDING ||
           pdp_list[GET_TRANSID(rab_id)]->pdp_state == PDP_ACTIVE_PENDING)
      {
        // Notify SM that the RAB is established.
        
        send_rabmsm_cmd( rab_id, RABMSM_ACTIVATE_RSP );
        change_state = TRUE;
        rabm_send_rab_est_ind_to_gmm();

      }
      else if (rab_list[rab_id].rab_state == RAB_MODIFY_PENDING ||
               pdp_list[GET_TRANSID(rab_id)]->rab_state == RABM_MODIFY_PENDING)
      {
        // Notify SM that the RAB is established.
        
        //Stop in case  RAB EST PENDING timer is running
        rabm_stop_timer(rab_id);
        send_rabmsm_cmd( rab_id, RABMSM_MODIFY_RSP ); 
        send_rabmcm_cmd(rab_id, RABM_RAB_ESTABLISH_IND, (cc_cause_T) REASON_NONE);
        change_state = TRUE;
        rabm_send_rab_est_ind_to_gmm();

      }
      else if (rab_list[rab_id].rab_state == RAB_REESTABLISH_PENDING)
      {
        MSG_MED_DS_0(RABM_SUB, "Clearing, TIMER_RABM_EST_PENDING timer"); 
        rabm_stop_timer (rab_id);
        send_rabmcm_cmd(rab_id, RABM_RAB_ESTABLISH_IND, (cc_cause_T) REASON_NONE);
        change_state = TRUE;
        rabm_send_rab_est_ind_to_gmm();

      }      
      else if (pdp_list[GET_TRANSID(rab_id)]->pdp_state == PDP_ACTIVE)
      {
        // If the PDP for this corresponding RAB is ACTIVE, send the
        // CM a RAB-ESTABLISH Indication command.
        MSG_MED_DS_0(RABM_SUB, "Sending a RABM_RAB_ESTABLISH_IND to CM");
        send_rabmcm_cmd(rab_id, RABM_RAB_ESTABLISH_IND, (cc_cause_T) REASON_NONE);
        change_state = TRUE;
        rabm_send_rab_est_ind_to_gmm();

      }
      else if (pdp_list[GET_TRANSID(rab_id)]->pdp_state == PDP_INACTIVE_PENDING)
      {
        // Notify SM that the RAB is established.
        
        send_rabmsm_cmd( rab_id, RABMSM_ACTIVATE_RSP );
        //Change state to RAB_INACTIVE_PENDING
        rab_list[rab_id].rab_state = RAB_INACTIVE_PENDING;
        rabm_send_rab_est_ind_to_gmm();
      }
      else
      {
        MSG_ERROR_DS_0( RABM_SUB, "Received RAB EST IND from RRC in wrong state");
      }

      if (change_state)
      {
        // Change the RAB state to RAB_ACTIVE
        rab_list[rab_id].rab_state = RAB_ACTIVE;
        change_state = FALSE;
        sm_unset_ps_vote_for_reselection();
      }

    break;

    case RABMAS_RAB_RELEASE_IND:
      // Make sure the corresponding RAB is in RAB_INACTIVE_PENDING
      rab_id =  cmd_ptr->cmd.rrc_establish_ind.rabid;
      if (rab_id<MIN_RAB_NUM || rab_id>=MAX_RAB_NUM)
      {
        MSG_HIGH_DS_1(RABM_SUB,"RABM Received RRC CMD: RAB Release Ind: Invalid rab_id:%d", rab_id);
        break;
      }

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
    if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.rrc_release_ind.as_id) == FALSE)
    {
        MSG_ERROR_DS_2( RABM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , cmd_ptr->cmd.rrc_release_ind.as_id);
        return ;
    }
#endif
#endif

    if (GET_TRANSID(rab_id) == INVALID_ID || pdp_list[GET_TRANSID(rab_id)] == NULL)
    {
      MSG_HIGH_DS_2(RABM_SUB,"RABM Received RRC CMD: RAB Release Ind: Invalid Transaction Id- %d for RAB id- %d", GET_TRANSID(rab_id), rab_id);
      rab_list[rab_id].rab_state = RAB_INACTIVE;
      break;
     }

      if (rab_list[rab_id].rab_state == RAB_INACTIVE_PENDING)
      {
        // Notify SM that the RAB is Released.
        
        send_rabmsm_cmd( rab_id, RABMSM_DEACTIVATE_RSP );
        change_state = TRUE;
      }
      else if ((rab_list[rab_id].rab_state == RAB_MODIFY_PENDING) ||
                (rab_list[rab_id].rab_state == RAB_ACTIVE_PENDING))
      {
        
        send_rabmsm_cmd( rab_id, RABMSM_STATUS_REQ );
      }
      else if (pdp_list[GET_TRANSID(rab_id)]->pdp_state == PDP_ACTIVE_PENDING)
      {
        // Notify SM that the RAB is Released.
        
        send_rabmsm_cmd( rab_id, RABMSM_DEACTIVATE_RSP );
        change_state = TRUE;
      }
      else if (rab_list[rab_id].rab_state == RAB_ACTIVE)
      {
        // Send this to the CM directly.
        MSG_MED_DS_1(RABM_SUB, "Sending a RABM_RAB_RELEASE_IND to CM for RAB id : %d",rab_id);
        send_rabmcm_cmd(rab_id, RABM_RAB_RELEASE_IND, (cc_cause_T) REASON_NONE);
        change_state = TRUE;
        /* Send RAB release ind to GMM so that it stops the timer T3319, so that if 
           application desires it may trigger the re-establishment of RAB, which has 
           to be processed immediately */
        gmm_cmd = mm_get_cmd_buf();   
        if (gmm_cmd != NULL)
        {
          gmm_cmd->cmd.gmmrabm_rab_rel_ind.header.message_set     = MS_GMM_RABM;
          gmm_cmd->cmd.gmmrabm_rab_rel_ind.header.message_id      = GMMRABM_RAB_REL_IND;
          gmm_cmd->cmd.gmmrabm_rab_rel_ind.header.message_len_lsb = 0;
          gmm_cmd->cmd.gmmrabm_rab_rel_ind.header.message_len_msb = 0; 
                        

#ifdef FEATURE_DUAL_SIM
         gmm_cmd->cmd.gmmrabm_rab_rel_ind.as_id= (sys_modem_as_id_e_type)sm_as_id;
#endif

          MSG_HIGH_DS_0( RABM_SUB,"Sending RAB Re-establish Req to GMM");
          mm_put_cmd (gmm_cmd);
        }
        else
        {
          MSG_ERROR_DS_0( RABM_SUB, "Unable to get a GMM command buffer");
        }
      }
      else if (pdp_list[GET_TRANSID(rab_id)]->rab_state == RABM_DEACTIVATED)
      {
        MSG_HIGH_DS_1(RABM_SUB,"RABM Received RRC CMD: RAB is already INACTIVE %d",rab_id);
      }
      else
      {
        // Received RAB Release Indication from RRC in wrong state
        MSG_ERROR_DS_0( RABM_SUB, "Received RAB REL IND from RRC in wrong state");;
      }        

      if(change_state)
      {
        // Change the RAB state to RAB_INACTIVE
        rab_list[rab_id].rab_state = RAB_INACTIVE;
        change_state = FALSE;
      }
    break;

    case RABMAS_STATUS_IND:
      // Indication from RRC about a RAB Failure.     
      rab_id = cmd_ptr->cmd.rrc_status_ind.rab_id;

      if (rab_id<MIN_RAB_NUM || rab_id>=MAX_RAB_NUM)
      {
        MSG_HIGH_DS_1(RABM_SUB,"RABM Received RRC CMD: RAB Status Ind: Invalid rab_id:%d", rab_id);
        break;
      }

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
      if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.rrc_status_ind.as_id) == FALSE)
      {
        MSG_ERROR_DS_2( RABM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , cmd_ptr->cmd.rrc_status_ind.as_id);
        return ;
      }
#endif
#endif
      if (GET_TRANSID(rab_id) == INVALID_ID || pdp_list[GET_TRANSID(rab_id)] == NULL)
      {
        MSG_HIGH_DS_2(RABM_SUB,"RABM Received RRC CMD: RAB Status Ind: Invalid Transaction Id- %d for RAB id- %d", GET_TRANSID(rab_id), rab_id);
        rab_list[rab_id].rab_state = RAB_INACTIVE;
        break;
      }

      
      send_rabmsm_cmd( rab_id, RABMSM_STATUS_REQ );
      break;

    case RABMAS_FORCE_CLOSE_SESSION_IND:
#ifdef FEATURE_DUAL_SIM
#ifndef FEAUTRE_DUAL_DATA
      if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.rabmas_force_close_session_ind.as_id) == FALSE)
      {
        MSG_ERROR_DS_2( RABM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , cmd_ptr->cmd.rabmas_force_close_session_ind.as_id);
        return ;
      }
#endif
#endif
      sm_force_gmmsm_close_session_req();
      break;

    default:
      MSG_ERROR_DS_1( RABM_SUB,"Wrong Command Sent by RRC: %d", cmd_ptr->header.cmd_id);
      break;
  }
}

#ifdef FEATURE_TC
/*===========================================================================
FUNCTION RABM_PROCESS_TC_CMD 

DESCRIPTION
  This function processes command sent by the Test Control (TC)
  during a mode change operation.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void rabm_process_tc_cmd( rabm_cmd_type* cmd_ptr )
{
  MSG_HIGH_DS_0( RABM_SUB,"RABM recd TC mode change cmd");

  if (cmd_ptr->header.cmd_id != RABMTC_MODE_CHANGE_IND)
  {
    ERR ("Received unknown cmd from TC",0,0,0);
  }
  else
  {
/* 
  #ifdef FEATURE_DUAL_DATA
    if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.tc_mode_change_ind.as_id) == FALSE)
         return;
  #else
      if (sm_as_id == SYS_MODEM_AS_ID_NONE || 
          sm_as_id != cmd_ptr->cmd.tc_mode_change_ind.as_id)
      {
        MSG_ERROR_DS_2( RABM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , cmd_ptr->cmd.tc_mode_change_ind.as_id);
        return ;
      }
  #endif //#ifdef FEATURE_DUAL_DATA
*/

    if ((ue_in_test_mode) && 
         (cmd_ptr->cmd.tc_mode_change_ind.mode_type == UE_NORMAL_MODE))
    {
      MSG_HIGH_DS_0(RABM_SUB,"RABM mode from Test->Normal");
      ue_in_test_mode = FALSE;
    }
    else if ((!ue_in_test_mode) && 
             ((cmd_ptr->cmd.tc_mode_change_ind.mode_type == UE_LB_MODE1) ||
              (cmd_ptr->cmd.tc_mode_change_ind.mode_type == UE_LB_MODE2)))
    {
      MSG_HIGH_DS_0(RABM_SUB,"RABM mode from Normal->Test");
      ue_in_test_mode = TRUE;      
    }
    else
    {
      MSG_ERROR_DS_0( RABM_SUB, "TC mode change in wrong state");
    }
  }
}

#endif //FEATURE_TC

#ifdef FEATURE_DUAL_DATA

/*===========================================================================
FUNCTION RABM_RABID_VALIDATE 

DESCRIPTION
  This function validates if a RABID is valid.  RRC calls this in RRC context
  to find out if a RABID is valid before establish a RAB as per the UTRAN
  request.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean rabm_rabid_validate(rab_query_T query_type, byte rab_id )
{
  boolean status = FALSE;
  #ifdef FEATURE_MBMS
  #error code not present
#endif
  
  sys_modem_as_id_e_type as_id = sm_get_multimode_sub();

  if(SM_IS_VALID_AS_ID(as_id) == FALSE)
  {
    return status;
  }

  // convert the RRC RAB_ID to the NAS RAB_ID (same as NSAPI)
  rab_id = rab_id & 0xf; 

  MSG_HIGH_DS_3(RABM_LOCAL_SUB,"RRC query_type = %d, rab_id %d state : %d",query_type,rab_id,
            rab_list_sim[as_id][rab_id].rab_state);

  #ifdef FEATURE_MBMS
  #error code not present
#endif

  rex_enter_crit_sect(&sm_crit_sect);

  if ((query_type == RAB_ESTABLISH_QUERY) &&
      ((rab_list_sim[as_id][rab_id].rab_state != RAB_INACTIVE) ||
       (GET_TRANSID_PER_SUB(rab_id, as_id) != INVALID_ID &&
        pdp_list_sim[as_id][GET_TRANSID_PER_SUB(rab_id, as_id)] != NULL &&
        pdp_list_sim[as_id][GET_TRANSID_PER_SUB(rab_id, as_id)]->pdp_state != PDP_INACTIVE) ||
        rabm_check_rab_active_per_sub(as_id)))
  {
    status = TRUE;
  } 
  else if ((query_type == RAB_MODIFY_QUERY) &&
           ((rab_list_sim[as_id][rab_id].rab_state == RAB_MODIFY_PENDING) ||
            (GET_TRANSID_PER_SUB(rab_id, as_id) != INVALID_ID &&
             pdp_list_sim[as_id][GET_TRANSID_PER_SUB(rab_id,as_id)] && 
             pdp_list_sim[as_id][GET_TRANSID_PER_SUB(rab_id,as_id)]->pdp_state == PDP_ACTIVE)))
  {
    status = TRUE;
  } 
  else if ((query_type == RAB_RELEASE_QUERY) &&
           ((rab_list_sim[as_id][rab_id].rab_state == RAB_INACTIVE_PENDING) ||
           (GET_TRANSID_PER_SUB(rab_id,as_id) != INVALID_ID &&
            pdp_list_sim[as_id][GET_TRANSID_PER_SUB(rab_id,as_id)] &&
            pdp_list_sim[as_id][GET_TRANSID_PER_SUB(rab_id,as_id)]->pdp_state == PDP_ACTIVE)))
  {
    status = TRUE;
  }
  else
  {
    MSG_ERROR_DS_0( RABM_LOCAL_SUB, "Wrong RAB Query type received from RRC");
  }

  rex_leave_crit_sect(&sm_crit_sect);

  return status;
}

#else

/*===========================================================================
FUNCTION RABM_RABID_VALIDATE 

DESCRIPTION
  This function validates if a RABID is valid.  RRC calls this in RRC context
  to find out if a RABID is valid before establish a RAB as per the UTRAN
  request.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean rabm_rabid_validate(rab_query_T query_type, byte rab_id )
{
  boolean status = FALSE;
  #ifdef FEATURE_MBMS
  #error code not present
#endif
  

  // convert the RRC RAB_ID to the NAS RAB_ID (same as NSAPI)
  rab_id = rab_id & 0xf; 

  MSG_HIGH_DS_3(RABM_SUB,"RRC query_type = %d, rab_id %d state : %d",query_type,rab_id,
            rab_list[rab_id].rab_state);

  #ifdef FEATURE_MBMS
  #error code not present
#endif

  rex_enter_crit_sect(&sm_crit_sect);

  if ((query_type == RAB_ESTABLISH_QUERY) &&
      ((rab_list[rab_id].rab_state != RAB_INACTIVE) ||
       (GET_TRANSID(rab_id) != INVALID_ID &&
        pdp_list[GET_TRANSID(rab_id)] != NULL &&
        pdp_list[GET_TRANSID(rab_id)]->pdp_state != PDP_INACTIVE) ||
        rabm_check_rab_active()))
  {
    status = TRUE;
  } 
  else if ((query_type == RAB_MODIFY_QUERY) &&
           ((rab_list[rab_id].rab_state == RAB_MODIFY_PENDING) ||
            (GET_TRANSID(rab_id) != INVALID_ID &&
             pdp_list[GET_TRANSID(rab_id)] && 
             pdp_list[GET_TRANSID(rab_id)]->pdp_state == PDP_ACTIVE)))
  {
    status = TRUE;
  } 
  else if ((query_type == RAB_RELEASE_QUERY) &&
           ((rab_list[rab_id].rab_state == RAB_INACTIVE_PENDING) ||
           (GET_TRANSID(rab_id) != INVALID_ID &&
            pdp_list[GET_TRANSID(rab_id)] &&
            pdp_list[GET_TRANSID(rab_id)]->pdp_state == PDP_ACTIVE)))
  {
    status = TRUE;
  }
  else
  {
    MSG_ERROR_DS_0( RABM_SUB, "Wrong RAB Query type received from RRC");
  }

  rex_leave_crit_sect(&sm_crit_sect);

  return status;
}

#endif

/*===========================================================================
FUNCTION RABM_RAB_ESTABLISHED 

DESCRIPTION
  This function processes command sent by the SM task.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean rabm_rab_established( byte rab_id )
{
  boolean status;
  status = FALSE;
  
  // check if a RAB for the given RAB_ID is active
  if ((rab_list[rab_id].rab_state == RAB_ACTIVE) ||
      (rab_list[rab_id].rab_state == RAB_INACTIVE_PENDING))
  {
    status = TRUE;
  }
  
  return status;
}

/*===========================================================================
FUNCTION RABM_PROCESS_SM_CMD 

DESCRIPTION
  This function processes command sent by the SM task.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void rabm_process_sm_cmd( rabm_cmd_type* cmd_ptr )
{
  byte rab_id;

  switch (cmd_ptr->header.cmd_id)
  {
    case RABMSM_ACTIVATE_IND:
      rab_id =  cmd_ptr->cmd.sm_activate_ind.nsapi;

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
     if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.sm_activate_ind.as_id) == FALSE)
         return;
#endif
#endif
      
     if (rab_list[rab_id].rab_state == RAB_ACTIVE)
      {
         // Notify SM that the RAB is established.
        
        send_rabmsm_cmd( rab_id, RABMSM_ACTIVATE_RSP );
      }
      else
      {
        // Set the RAB state to ACTIVE_PENDING 
        rab_list[rab_id].rab_state = RAB_ACTIVE_PENDING;
        MSG_HIGH_DS_1( RABM_SUB,"Recd RABMSM_ACTIVATE_IND for : %d ",rab_id);

#ifdef FEATURE_TC
        /* -----------------------------------------------------------------/
          ** If the RABM is in TEST mode of operation, then stop the SM-RABM timer ...
          * ----------------------------------------------------------------*/
        if (ue_in_test_mode)
        {
          if (GET_TRANSID(rab_id) == INVALID_ID || pdp_list[GET_TRANSID(rab_id)] == NULL )
          {
            MSG_HIGH_DS_2( RABM_SUB,"RABM Received RABMSM_ACTIVATE_IND Cmd, Invalid Trans Id- %d, RAB-%d", GET_TRANSID(rab_id), rab_id);
          }
          else 
          {
            MSG_HIGH_DS_2( RABM_SUB,"RABM Received RABMSM_ACTIVATE_IND Cmd, STOPPED timer for Trans Id- %d, RAB-%d", GET_TRANSID(rab_id), rab_id);
            sm_stop_timer(GET_TRANSID(rab_id));
          }
        }
#endif /* FEATURE_TC */
      }
      break;

    case RABMSM_DEACTIVATE_IND:
      rab_id =  cmd_ptr->cmd.sm_deactivate_ind.nsapi;

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
      if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.sm_deactivate_ind.as_id) == FALSE)
         return;
#endif
#endif
      if (GET_TRANSID(rab_id) == INVALID_ID ||
           pdp_list[GET_TRANSID(rab_id)] == NULL )
      {
        MSG_HIGH_DS_2( RABM_SUB,"RABM Received RABMSM_DEACTIVATE_IND Cmd, Trans Id- %d, RAB-%d", GET_TRANSID(rab_id), rab_id);
        break;
      }
      MSG_HIGH_DS_2( RABM_SUB,"RABM Received RABMSM_DEACTIVATE_IND for RAB : %d RAB State: %d ", rab_id, rab_list[rab_id].rab_state);
      if (rab_list[rab_id].rab_state == RAB_ACTIVE)
      {
        
        rab_list[rab_id].rab_state = RAB_INACTIVE_PENDING;
      }
      else if (rab_list[rab_id].rab_state == RAB_INACTIVE)
      {
        //IF the RAB is already in INACTIVE state send the response to SM
        
        send_rabmsm_cmd(rab_id, RABMSM_DEACTIVATE_RSP);
      }
      else if (rab_list[rab_id].rab_state == RAB_ACTIVE_PENDING)
      {
        //IF the RAB is already in INACTIVE state send the response to SM
        
        rab_list[rab_id].rab_state = RAB_INACTIVE;
        send_rabmsm_cmd(rab_id, RABMSM_DEACTIVATE_RSP);
      }
      else if (rab_list[rab_id].rab_state == RAB_REESTABLISH_PENDING) 
      {
        
        //Stop the RAb timer 
        rabm_stop_timer(rab_id);
        //set the RAB state to INACTIVE
        rab_list[rab_id].rab_state = RAB_INACTIVE;

        if (pdp_list[GET_TRANSID(rab_id)]->pdp_state == PDP_INACTIVE_PENDING)
        {
          //Send the deactivate rsp to SM 
          send_rabmsm_cmd(rab_id, RABMSM_DEACTIVATE_RSP);
        }
        else{
          //Send the RAB_ESTABLISH_REJ to CM directly
          send_rabmcm_cmd((byte)rab_id, RABM_RAB_REESTABLISH_REJ, (cc_cause_T) REASON_NONE);
        }
      }
      else if (rab_list[rab_id].rab_state == RAB_SERVICE_REQ_PENDING ||
               rab_list[rab_id].rab_state == RAB_SERVICE_REQ_SENT)
      {
        
        rab_list[rab_id].rab_state = RAB_INACTIVE;

        if (pdp_list[GET_TRANSID(rab_id)]->pdp_state == PDP_INACTIVE_PENDING)
        {
           //Send the deactivate rsp to SM
          send_rabmsm_cmd(rab_id, RABMSM_DEACTIVATE_RSP);
        }       
      }
      break;

    case RABMSM_DEACTIVATE_IMPLICIT_IND:
       rab_id =  cmd_ptr->cmd.sm_deactivate_ind.nsapi;
       MSG_HIGH_DS_1(RABM_SUB,"Locally Deactivating the RAB %d",rab_id);
#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
      if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.sm_deactivate_ind.as_id) == FALSE)
         return;
#endif
#endif
       if (rab_list[rab_id].rab_state != RAB_INACTIVE)
       {
         //set the RAB state to inactive
         rab_list[rab_id].rab_state = RAB_INACTIVE;
         //stop if any RAB timer is running
         rabm_stop_timer(rab_id);
       }
       break;

    case RABMSM_MODIFY_IND:
      rab_id =  cmd_ptr->cmd.sm_modify_ind.nsapi;
      MSG_HIGH_DS_0( RABM_SUB,"RABM Received RABMSM_MODIFY_IND Cmd");

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
      if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.sm_modify_ind.as_id) == FALSE)
         return;
#endif
#endif
      if (rab_list[rab_id].rab_state == RAB_ACTIVE)
      {
        /*If RAB is already active send modify rsp rightaway*/
        
        send_rabmsm_cmd( rab_id, RABMSM_MODIFY_RSP );
      }
      else if (rab_list[rab_id].rab_state != RAB_REESTABLISH_PENDING)
      {
        rab_list[rab_id].rab_state = RAB_MODIFY_PENDING;
      }
      break;

   case RABMSM_SIG_CON_RELEASE_IND:
     // If the RRC connection is release, SM informs RABM to manage
     // corresponding RABs properly.
     process_sig_connection_rel();
     break;

   case RABMSM_RAT_CHANGE_IND:
     rabm_process_rat_change_ind();
     break;
     
   case RABMSM_RAT_RESUME_IND:
     rabm_process_rat_resume_ind();
     break;
     
    default :
      ERR ("Invalid Command Received from SM task",0,0,0);
      break;
  }
}

/*===========================================================================
FUNCTION RABM_PROCESS_GMM_CMD 

DESCRIPTION
  This function processes command sent by the GMM task.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void rabm_process_gmm_cmd(rabm_cmd_type* cmd_ptr ) 
{
  byte rab_id;
  


  switch (cmd_ptr->header.cmd_id)
  {
    case GMMRABM_REESTABLISH_RSP:
      MSG_HIGH_DS_0( RABM_SUB,"RABM Received GMM CMD GMMRABM_REESTABLISH_RSP");   
       
#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
      if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.gmm_reestablish_rsp.as_id) == FALSE)
      {
        MSG_ERROR_DS_2( RABM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , cmd_ptr->cmd.gmm_reestablish_rsp.as_id);
        return ;
      }
#endif
#endif

      /* For all RAB is state RAB_SERVICE_REQ_PENDING and RAB_SERVICE_REQ_SENT 
      set state to RAB_REESTABLISH_PENDING, send RE-ESTABLISH Confirmation to CM task 
      and start rab sanity timer */
      for(rab_id = MIN_RAB_NUM; rab_id < MAX_PDP_NSAPI_NUM; rab_id++)
      {
        if(rab_list[rab_id].rab_state == RAB_SERVICE_REQ_SENT ||
            ((rab_list[rab_id].rab_state == RAB_SERVICE_REQ_PENDING)
            && GET_TRANSID(rab_id) != INVALID_ID &&
                 pdp_list[GET_TRANSID(rab_id)] != NULL && 
                (pdp_list[GET_TRANSID(rab_id)]->qos.traffic_class == (int)STREAMING_CLASS ||
                 pdp_list[GET_TRANSID(rab_id)]->qos.traffic_class == (int)CONVERSATION_CLASS) 
             ))
        {      
          rab_list[rab_id].rab_state = RAB_REESTABLISH_PENDING;
          MSG_HIGH_DS_1(RABM_SUB,"RAB %d state changed to RAB_REESTABLISH_PENDING",rab_id);
          rabm_start_timer(rab_id);

          if (GET_TRANSID(rab_id) != INVALID_ID &&
              pdp_list[GET_TRANSID(rab_id)] != NULL && 
              (pdp_list[GET_TRANSID(rab_id)]->qos.traffic_class == (int)STREAMING_CLASS ||
              pdp_list[GET_TRANSID(rab_id)]->qos.traffic_class == (int)CONVERSATION_CLASS))
          { 
            if(sm_is_apn_blocked(pdp_list[GET_TRANSID(rab_id)]->apn_name))
            {
               MSG_HIGH_DS_1( RABM_SUB,"APN blocked RAB %d state changed to RAB_INACTIVE",rab_id);
               rab_list[rab_id].rab_state = RAB_INACTIVE;
               send_rabmcm_cmd(rab_id, RABM_RAB_REESTABLISH_REJ, (cc_cause_T)RABM_APN_BLOCKED);
            }
            else
            {
               /*Send the RABSM_RESTABLISH_IND to trigger the SM for  PDP context
               modification for straming and conversational traffic class*/
               send_rabmsm_cmd(rab_id, RABMSM_REESTABLISH_IND);
            }
          }
        }
      }
     
      rabm_send_combined_gmmrabm_reestablish_req();       
      break;

    case GMMRABM_REESTABLISH_REJ:
      MSG_HIGH_DS_1( RABM_SUB,"RABM Received GMM CMD GMMRABM_REESTABLISH_REJ with cause = %d",
                             cmd_ptr->cmd.gmm_reestablish_rej.cause);    

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
      if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.gmm_reestablish_rej.as_id) == FALSE)
      {
        MSG_ERROR_DS_2( RABM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , cmd_ptr->cmd.gmm_reestablish_rej.as_id);
        return ;
      }
#endif
#endif

      /* For all RABs in state RAB_SERVICE_REQ_PENDING and RAB_SERVICE_REQ_SENT 
         send RABM_RAB_REESTABLISH_REJ to CM task*/
      for(rab_id = MIN_RAB_NUM; rab_id < MAX_PDP_NSAPI_NUM; rab_id++)
      {
        if(rab_list[rab_id].rab_state == RAB_SERVICE_REQ_SENT)
        {  
          MSG_HIGH_DS_1( RABM_SUB,"RAB %d state changed to RAB_INACTIVE",rab_id);
          rab_list[rab_id].rab_state = RAB_INACTIVE;
          send_rabmcm_cmd(rab_id, RABM_RAB_REESTABLISH_REJ, (cc_cause_T) cmd_ptr->cmd.gmm_reestablish_rej.cause);               
        }             
      }
      
      /* ----------------------------------------------------------------------
      ** Perform Local Deactivation of the PDP context, if the SERVICE REJECT
      ** has any of the below reject causes that causes GMM to DEREGISTER.  
      ** In this case SERVICE REQ was triggered due to a RAB-REESTABLISHMENT. 
      ** ---------------------------------------------------------------------*/
      if ((cmd_ptr->cmd.gmm_reestablish_rej.cause == NO_PDP_CONTEXT_ACTIVATED)
          ||(cmd_ptr->cmd.gmm_reestablish_rej.cause == ILLEGAL_MS)
          ||(cmd_ptr->cmd.gmm_reestablish_rej.cause == GPRS_SERVICES_NOT_ALLOWED)
          ||(cmd_ptr->cmd.gmm_reestablish_rej.cause == MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK)
          ||(cmd_ptr->cmd.gmm_reestablish_rej.cause == IMPLICITLY_DETACHED)
          ||(cmd_ptr->cmd.gmm_reestablish_rej.cause == PLMN_NOT_ALLOWED)
          ||(cmd_ptr->cmd.gmm_reestablish_rej.cause == LA_NOT_ALLOWED)
          ||(cmd_ptr->cmd.gmm_reestablish_rej.cause == ILLEGAL_ME))
      {
        /* Perform a Local deactivation of the ACTIVE PDP contexts */
        sm_perform_local_deactivation((cc_cause_T)mm_convert_gmm_to_sm_cause(cmd_ptr->cmd.gmm_reestablish_rej.cause));
      }
      else
      {
        rabm_send_combined_gmmrabm_reestablish_req(); 
      }
      break; 

      default :
          ERR ("Wrong Command Sent by GMM : %d",cmd_ptr->header.cmd_id,0,0);
      break;
  }
}


/*===========================================================================
FUNCTION RABM_REEST_PEND_NON_STREAM_NON_CONV

DESCRIPTION
  This function returns TRUE if RAB state is REETABLISH_PENDING for 
  non streaming and non conv RAB.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  NOne
===========================================================================*/
boolean rabm_reest_pend_non_stream_non_conv(void)
{
  byte rab_id;

  for (rab_id = MIN_RAB_NUM; rab_id < MAX_PDP_NSAPI_NUM; rab_id++)
  {
    if (rab_list[rab_id].rab_state == RAB_REESTABLISH_PENDING &&        
        GET_TRANSID(rab_id) != INVALID_ID &&
        pdp_list[GET_TRANSID(rab_id)] != NULL &&
        pdp_list[GET_TRANSID(rab_id)]->qos.traffic_class != (int)STREAMING_CLASS &&
        pdp_list[GET_TRANSID(rab_id)]->qos.traffic_class != (int)CONVERSATION_CLASS )
    {
      return TRUE;
    }
  }
  return FALSE;
}

                                                  
/*===========================================================================
FUNCTION RABM_PROCESS_CM_CMD 

DESCRIPTION
  This function processes command sent by the CM task.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void rabm_process_cm_cmd(rabm_cmd_type* cmd_ptr ) 
{
  byte rab_id;
  
  
  switch (cmd_ptr->header.cmd_id)
  {
    case RABM_RAB_REESTABLISH_REQ:
      // Process the RABM_RAB_REESTABLISH_REQ from CM.
      rab_id = cmd_ptr->cmd.rabm_reestablish_req.rabid;   
#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
      if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.rabm_reestablish_req.as_id) == FALSE)
      {
        MSG_HIGH_DS_1( RABM_SUB,"Sending RABM_RAB_REESTABLISH_REJ for RAB %d to CM",rab_id);
        send_rabmcm_cmd( (byte)rab_id, RABM_RAB_REESTABLISH_REJ, (cc_cause_T) REASON_NONE );
        return ;
      }
#endif
#endif

      if (rab_id<MIN_RAB_NUM || rab_id>=MAX_RAB_NUM)
      {
        MSG_HIGH_DS_1( RABM_SUB,"Rev RABM_RAB_REQ_REESTABLISH_REQ: Invalid rab_id:%d", rab_id);
        break;
      }

      MSG_HIGH_DS_1( RABM_SUB,"Rev RABM_RAB_REQ_REESTABLISH_REQ %d",rab_id);
      if (GET_TRANSID(rab_id) == INVALID_ID || pdp_list[GET_TRANSID(rab_id)] == NULL)
      {
        rab_list[rab_id].rab_state = RAB_INACTIVE;
        break;
      }
      
      if (rab_list[rab_id].rab_state == RAB_ACTIVE)
      {
        //If the RAB is already active send the RAB EST IND to CM
        MSG_HIGH_DS_1(RABM_SUB,"RAB %d is already activate, sending RAB EST IND even",
                   rab_id);
        send_rabmcm_cmd(rab_id, RABM_RAB_ESTABLISH_IND, (cc_cause_T) REASON_NONE);
      }
      
      else if (pdp_list[GET_TRANSID(rab_id)] != NULL &&
                  pdp_list[GET_TRANSID(rab_id)]->pdp_state != PDP_INACTIVE_PENDING)
      {
        if(rab_list[rab_id].rab_state == RAB_SERVICE_REQ_PENDING ||
            rab_list[rab_id].rab_state == RAB_SERVICE_REQ_SENT||
            rab_list[rab_id].rab_state == RAB_REESTABLISH_PENDING)
        {
          MSG_HIGH_DS_1(RABM_SUB,"RAB Re-establish fot this RAB %d already in progress", rab_id);          
        }    
        //Check if any RAB is waiting for service request to be completed
        else if(rabm_check_serv_req_complete_pending())
        {
          MSG_HIGH_DS_1(RABM_SUB,"RAB %d state changed to RAB_SERVICE_REQ_PENDING",rab_id);
          rab_list[rab_id].rab_state = RAB_SERVICE_REQ_PENDING;
        }
        //Check if any RAB is in RAB Reestablish pending state
        else if (rabm_check_rab_reest_pend())
        {
          if (pdp_list[GET_TRANSID(rab_id)]->qos.traffic_class == (int)STREAMING_CLASS ||
              pdp_list[GET_TRANSID(rab_id)]->qos.traffic_class == (int)CONVERSATION_CLASS)
          {             
            rab_list[rab_id].rab_state = RAB_REESTABLISH_PENDING;
            MSG_HIGH_DS_1(RABM_SUB,"RAB %d state changed to RAB_REESTABLISH_PENDING",rab_id);
            rabm_start_timer(rab_id);
            send_rabmsm_cmd(rab_id, RABMSM_REESTABLISH_IND);
          }         
          else 
          {
            rab_list[rab_id].rab_state = RAB_SERVICE_REQ_SENT;
            MSG_HIGH_DS_1(RABM_SUB,"RAB %d state changed to RAB_SERVICE_REQ_SENT",rab_id);
            rabm_send_gmmrabm_reestablish_req(rab_id);
            
          }
        }
        // Send GMMRABM_REESTABLISH_REQ to GMM task
        else
        {
          rab_list[rab_id].rab_state = RAB_SERVICE_REQ_SENT;
          MSG_HIGH_DS_1(RABM_SUB,"RAB %d state changed to RAB_SERVICE_REQ_SENT",rab_id);
          rabm_send_gmmrabm_reestablish_req(rab_id);
        }        
      }
      else
      {
        MSG_ERROR_DS_1( RABM_SUB,"Cannot reestablish RAB %d as PDP is in INACTIVE PENDING state",rab_id);
      }
    break;
    
    default:
      MSG_ERROR_DS_0( RABM_SUB,"Unknown RABM cmd from CM"); 
    break;
  }
}
                                                 
/*===========================================================================
FUNCTION SEND_RABMSM_CMD 

DESCRIPTION
  This function sends a command to SM task.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void send_rabmsm_cmd(byte rab_id, sm_cmd_enum_type sm_enum_type)
{
  sm_cmd_type *cmd_ptr;
 
  // Get the SM command
  cmd_ptr = sm_get_cmd_buf();

  if(cmd_ptr != NULL)
  {
    cmd_ptr->header.message_set = MS_RABM_SM;  
    cmd_ptr->header.cmd_id = sm_enum_type;
	
    MSG_HIGH_DS_2(RABM_SUB, "Sending CMD_ID:0x%X to SM task for RAB ID :%d",sm_enum_type, rab_id );

    if(cmd_ptr->header.cmd_id == RABMSM_ACTIVATE_RSP)
    {
      cmd_ptr->cmd.rabm_activate_rsp.nsapi = rab_id;
#ifdef FEATURE_DUAL_SIM
     cmd_ptr->cmd.rabm_activate_rsp.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
    }
    else if(cmd_ptr->header.cmd_id == RABMSM_ACTIVATE_RSP)
    {
      cmd_ptr->cmd.rabm_modify_rsp.nsapi = rab_id;
#ifdef FEATURE_DUAL_SIM
      cmd_ptr->cmd.rabm_modify_rsp.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
    }
    else if (cmd_ptr->header.cmd_id == RABMSM_DEACTIVATE_RSP)
    {
      cmd_ptr->cmd.rabm_modify_rsp.nsapi = rab_id;
#ifdef FEATURE_DUAL_SIM
    cmd_ptr->cmd.rabm_modify_rsp.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
    }
    else if (cmd_ptr->header.cmd_id == RABMSM_MODIFY_RSP)
    {
      cmd_ptr->cmd.rabm_deactivate_rsp.nsapi = rab_id;
#ifdef FEATURE_DUAL_SIM
      cmd_ptr->cmd.rabm_deactivate_rsp.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
    }
    else if (cmd_ptr->header.cmd_id == RABMSM_STATUS_REQ)
    {
      cmd_ptr->cmd.rabm_status_req.nsapi = rab_id;  
      cmd_ptr->cmd.rabm_status_req.cause = 1; 
    }
    else if (cmd_ptr->header.cmd_id == RABMSM_REESTABLISH_IND)
    {
#ifdef FEATURE_DUAL_SIM
      cmd_ptr->cmd.rabm_deactivate_rsp.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
    }

    // Send the Command to SM task
    sm_put_cmd(cmd_ptr);
  }
  else
  {
    //Unable to get an SM cmd
    MSG_ERROR_DS_0( RABM_SUB,"Unable to get SM cmd");
  }
}

#ifdef FEATURE_TC
/*===========================================================================
FUNCTION SEND_RABMTC_CMD 

DESCRIPTION
  This function sends a command to TC task.
  
DEPENDENCIES
  This is used only during Test mode operation.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void send_rabmtc_cmd(rabm_cmd_type* rabm_cmd_ptr)
{
  tc_cmd_type *tc_cmd_ptr;
 
  // Get the SM command
  tc_cmd_ptr = tc_get_cmd_buf();

  if(tc_cmd_ptr != NULL)
  {
    if (rabm_cmd_ptr->header.cmd_id == RABMAS_RAB_ESTABLISH_IND)
    {
      tc_cmd_ptr->header.cmd_id = RABMTC_RAB_SETUP_IND;  
      tc_cmd_ptr->cmd.rabm_rab_est_ind.rab_id = 
          rabm_cmd_ptr->cmd.rrc_establish_ind.rabid;
    }
    else if (rabm_cmd_ptr->header.cmd_id == 
                     RABMAS_RAB_RELEASE_IND)
    {
      tc_cmd_ptr->header.cmd_id = RABMTC_RAB_REL_IND;
      tc_cmd_ptr->cmd.rabm_rab_est_ind.rab_id = 
          rabm_cmd_ptr->cmd.rrc_release_ind.rabid;
    }
    else
    {
      MSG_ERROR_DS_0( RABM_SUB,"Unknown RRC cmd in Test mode");
    }
    
    // Pack the TC command and send it.
    tc_cmd_ptr->header.message_set = MS_RABM_TC;  
    tc_cmd_ptr->cmd.rabm_rab_est_ind.cause = 0;

    // Send the Command to TC task
    tc_put_cmd(tc_cmd_ptr);
  }
  else
  {
    //Unable to get an TC cmd
    MSG_ERROR_DS_0( RABM_SUB,"Unable to get TC cmd");
  }
}

#endif //FEATURE_TC


/*===========================================================================
FUNCTION RABM_GPRS_RELEASE_IND

DESCRIPTION
  This function is called whenever the GPRS context is detached.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void rabm_gprs_release_ind(void)
{
  word i; 
  MSG_HIGH_DS_0(RABM_SUB,"GPRS context Released, Release all RABs");
  
  // Set all the ACTIVE RABS to INACTIVE and default the RAB_IDs to INVALID.
  for(i=0; i<MAX_RAB_NUM; i++)
  {
    if(rab_list[i].rab_state == RAB_ACTIVE) 
    {
      rab_list[i].rab_id = INVALID_RAB;
      rab_list[i].rab_state = RAB_INACTIVE;
      MSG_MED_DS_1(RABM_SUB,"Releasing RAB rab_id", rab_list[i].rab_id);
    }
  }
}


/*===========================================================================
FUNCTION SEND_RABMCM_CMD 

DESCRIPTION
  This function sends a command to CM task.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void send_rabmcm_cmd(byte rab_id, rabm_cm_enum_T rabm_cmd_cmd, cc_cause_T cause)
{
  cm_rpt_type *cm_rpt;
#ifdef FEATURE_WCDMA
  byte con_id;
#endif /*#ifdef FEATURE_WCDMA*/
  
  // Get the Call Manager's command report for posting the command.
  cm_rpt = cm_get_cmd_buf();
  if(cm_rpt == NULL)
  {
    MSG_ERROR_DS_0(RABM_SUB,"CM Error: Not able to Allocate CM Buffer");
    return;
  }
#ifdef FEATURE_WCDMA
  con_id = (byte)GET_TRANSID(rab_id);
#endif /*#ifdef FEATURE_WCDMA*/
  
  switch (rabm_cmd_cmd)
  {
    case RABM_RAB_REESTABLISH_CNF: 
    cm_rpt->hdr.cmd = CM_RAB_REESTAB_CNF;
#ifdef FEATURE_WCDMA
    cm_rpt->cmd.rab_reestab_conf.connection_id = con_id;
    cm_rpt->cmd.rab_reestab_conf.rabid = rab_id;
#endif /*#ifdef FEATURE_WCDMA*/
    #ifdef FEATURE_DUAL_SIM
         cm_rpt->cmd.rab_reestab_conf.as_id = (sys_modem_as_id_e_type)sm_as_id;
    #endif
    break;
    
    case RABM_RAB_REESTABLISH_REJ:
    cm_rpt->hdr.cmd = CM_RAB_REESTAB_REJ;
#ifdef FEATURE_WCDMA
    cm_rpt->cmd.rab_reestab_rej.connection_id = con_id;
    cm_rpt->cmd.rab_reestab_rej.rabid = rab_id;
    //Always Send the cause if the Service request throttling feature is enabled
    if(rabm_get_3gpp_global_throtling_enabled())
    {
      cm_rpt->cmd.rab_reestab_rej.cause= cause;
    }
    else
    {
      cm_rpt->cmd.rab_reestab_rej.cause= (rabmcm_cause_T) REASON_NONE;
      if ((cause == ACCESS_CLASS_DSAC_REJECTION) || 
          (cause == RABM_APN_BLOCKED) || 
          (cause == RAB_RB_DS_L2_CLEANUP_COLLISION) ||
          (cause == REGISTRATION_IN_PROGRESS))
      {
        cm_rpt->cmd.rab_reestab_rej.cause= cause;
      }
    }
#endif /*#ifdef FEATURE_WCDMA*/
    #ifdef FEATURE_DUAL_SIM
    cm_rpt->cmd.rab_reestab_rej.as_id= (sys_modem_as_id_e_type)sm_as_id;
    #endif
    break;
    
    case RABM_RAB_ESTABLISH_IND:
    cm_rpt->hdr.cmd = CM_RAB_REESTABLISH_IND;
#ifdef FEATURE_WCDMA
    cm_rpt->cmd.rab_reestab_ind.connection_id = con_id;
    cm_rpt->cmd.rab_reestab_ind.rabid = rab_id; 
    if (is_wcdma_mode())
    {
      cm_rpt->cmd.rab_reestab_ind.sys_rat=SYS_RAT_UMTS_RADIO_ACCESS;
    }
#endif /*#ifdef FEATURE_WCDMA*/

#ifdef FEATURE_TDSCDMA
    if (is_tdscdma_mode()) 
    {
      cm_rpt->cmd.rab_reestab_ind.sys_rat=SYS_RAT_TDS_RADIO_ACCESS;
    }
#endif /*#ifdef FEATURE_TDSCDMA*/

#ifdef FEATURE_DUAL_SIM
    cm_rpt->cmd.rab_reestab_ind.as_id= (sys_modem_as_id_e_type)sm_as_id;
#endif
    break;
    
    case RABM_RAB_ESTABLISH_REJ:
    cm_rpt->hdr.cmd = CM_RAB_REESTAB_FAIL;
#ifdef FEATURE_WCDMA
    cm_rpt->cmd.rab_reestab_fail.connection_id = con_id;
    cm_rpt->cmd.rab_reestab_fail.rabid = rab_id; 
    
#endif /*#ifdef FEATURE_WCDMA*/


    #ifdef FEATURE_DUAL_SIM
    cm_rpt->cmd.rab_reestab_fail.as_id= (sys_modem_as_id_e_type)sm_as_id;
    #endif
    break;
    
    case RABM_RAB_RELEASE_IND:
    cm_rpt->hdr.cmd = CM_RAB_RELEASE_IND;
#ifdef FEATURE_WCDMA
    cm_rpt->cmd.rab_release_ind.connection_id = con_id;
    cm_rpt->cmd.rab_release_ind.rabid = rab_id;     
#endif /*#ifdef FEATURE_WCDMA*/
    #ifdef FEATURE_DUAL_SIM
         cm_rpt->cmd.rab_release_ind.as_id= (sys_modem_as_id_e_type)sm_as_id;
    #endif
    break;
 
    default:
    break;
  }
  
  MSG_HIGH_DS_2(RABM_SUB,"Sending Command:0x%X, rab_id:%d to CM", rabm_cmd_cmd, rab_id);

  /* Log using CFA packet */
  cfa_log_packet_ex( GS_QUEUE_CM,
                     MS_CM_RABM,
                     (byte )cm_rpt->hdr.cmd,
                     MIN(250,sizeof(cm_rpt->cmd)),
                     (byte*)&(cm_rpt->cmd) );

  // Put the CM command for CM processing
  cm_put_cmd(cm_rpt);
}

/*===========================================================================
FUNCTION PROCESS_SIG_CONNECTION_REL 

DESCRIPTION
  This function processes the RRC Signaling Connection release, notified by
  the SM task.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS
Releases all the active PDP RABs if the connection is lost.

===========================================================================*/
void process_sig_connection_rel (void)
{
  byte rab_id;
  boolean change_state = FALSE;

  /* ===================================================================
  ** RRC has released the Signaling connection and hence all the RABs
  ** are no longer active. Take appropriate action based on the status
  ** ==================================================================*/
  
  MSG_HIGH_DS_0(RABM_SUB,"Received REL_IND from MM");

  // Initialize all the RABs in the list as invalid
  for (rab_id=MIN_RAB_NUM; rab_id<MAX_RAB_NUM; rab_id++)
  {
    if (GET_TRANSID(rab_id) == INVALID_ID || pdp_list[GET_TRANSID(rab_id)] == NULL)
    {
        rab_list[rab_id].rab_state = RAB_INACTIVE;
        continue ;
    }

    if (rab_list[rab_id].rab_state == RAB_INACTIVE_PENDING)
    {
      // Notify SM that the RAB is Released.
      
      send_rabmsm_cmd(rab_id, RABMSM_DEACTIVATE_RSP );
      change_state = TRUE;
    }
    else if (rab_list[rab_id].rab_state == RAB_MODIFY_PENDING) 
    {
      
      send_rabmsm_cmd(rab_id, RABMSM_STATUS_REQ );
      change_state = TRUE;
    }
    else if (rab_list[rab_id].rab_state == RAB_ACTIVE_PENDING)
    {

      send_rabmsm_cmd(rab_id, RABMSM_STATUS_REQ );
      if (pdp_list[GET_TRANSID(rab_id)]->pdp_state == PDP_ACTIVE)
      {
        change_state = TRUE;
      }
    }
    else if (rab_list[rab_id].rab_state == RAB_ACTIVE)
    {
      if (pdp_list[GET_TRANSID(rab_id)]->pdp_state == PDP_ACTIVE ||
          pdp_list[GET_TRANSID(rab_id)]->pdp_state == PDP_MODIFY_PENDING)
      {
        // Send this to the CM directly.
        MSG_HIGH_DS_1( RABM_SUB,"RAB %d RAB_RELEASE_IND to CM : Dormant",rab_id);
        send_rabmsm_cmd(rab_id, RABMSM_DEACTIVATE_RSP );
        send_rabmcm_cmd((byte)rab_id, RABM_RAB_RELEASE_IND, (cc_cause_T) REASON_NONE);
        change_state = TRUE;
      }
      else if (pdp_list[GET_TRANSID(rab_id)]->pdp_state == PDP_ACTIVE_PENDING)
      {
        // Send this to the CM directly.
        MSG_HIGH_DS_1( RABM_SUB,"RAB %d Dormant",rab_id);
        // Notify SM that the RAB is Released.
        pdp_list[GET_TRANSID(rab_id)]->rab_state = RABM_DEACTIVATED;
        rab_list[rab_id].rab_state = RAB_ACTIVE_PENDING;
      }
    }
    else if (rab_list[rab_id].rab_state == RAB_REESTABLISH_PENDING )
    {
      
      //Stop the RAb timer in case it is running
      rabm_stop_timer(rab_id);
      //Send the RAB_ESTABLISH_REJ to CM directly
      send_rabmcm_cmd((byte)rab_id, RABM_RAB_REESTABLISH_REJ, (cc_cause_T) REASON_NONE);
      change_state = TRUE;
    }
    else if (rab_list[rab_id].rab_state == RAB_SERVICE_REQ_PENDING ||
                rab_list[rab_id].rab_state == RAB_SERVICE_REQ_SENT )
    {
      MSG_HIGH_DS_1(RABM_SUB,"Not changing the RAB Status on Conn. Release, waiting for GMM Resp for Rab ID : %d",rab_id);
    }
    else
    {
      // Received RAB Release Indication from RRC in wrong state
      MSG_MED_DS_0 (RABM_SUB,"SIG Connection release, no active RAB ");
    }

    if (change_state)
    {
      // Change the RAB state to RAB_INACTIVE
      rab_list[rab_id].rab_state = RAB_INACTIVE;
      change_state = FALSE;
      sm_unset_ps_vote_for_reselection();
    }
  }

}

/*===========================================================================
FUNCTION RABM_RAB_EST_TIMER_EXPIRED 

DESCRIPTION
  This function processes the RAB Establish Pending Timer expiry report.  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS
===========================================================================*/                  
void rabm_rab_est_timer_expired(word rab_id)
{
  mm_cmd_type * gmm_cmd;
  MSG_ERROR_DS_1( RABM_SUB,"RAB EST pending Timer expired rab_id %d",rab_id);
  
  if (rab_list[rab_id].rab_state == RAB_REESTABLISH_PENDING)
  {
    rab_list[rab_id].rab_state = RAB_INACTIVE;
    if(!(timer_est_pen_flag || sm_check_pdp_procedure_pending() || rabm_check_procedure_pending_for_SCRI()))
    {
      rab_list[rab_id].rab_state = RAB_REESTABLISH_PENDING;

      gmm_cmd = mm_get_cmd_buf();
      if (gmm_cmd != NULL)
      {
        gmm_cmd->cmd.gmmsm_session_close_req.header.message_set     = MS_GMM_SM;
        gmm_cmd->cmd.gmmsm_session_close_req.header.message_id      = GMMSM_CLOSE_SESSION_REQ;
        gmm_cmd->cmd.gmmsm_session_close_req.header.message_len_lsb = 0;
        gmm_cmd->cmd.gmmsm_session_close_req.header.message_len_msb = 0;  

#ifdef FEATURE_DUAL_SIM
         gmm_cmd->cmd.gmmsm_session_close_req.as_id= (sys_modem_as_id_e_type)sm_as_id;
#endif

        gmm_cmd->cmd.gmmsm_session_close_req.cause = SM_PS_SIG_REL_REQ;

        MSG_HIGH_DS_0( SM_SUB,"Sending CLOSE Session Req to GMM");
        mm_put_cmd (gmm_cmd);
      }
      else
      {
        MSG_FATAL_DS( SM_SUB,"Unable to get a GMM command buffer",0,0,0);
      } 
    }
    else
    {
      send_rabmcm_cmd((byte)rab_id, RABM_RAB_REESTABLISH_REJ, (cc_cause_T) REASON_NONE);
      sm_unset_ps_vote_for_reselection();
    }
  }
  else
  {
    ERR ( "Unknown RAB timer expired : %d", rab_id, 0, 0);
  }
}

/*===========================================================================
FUNCTION RABM_PROCESS_RAT_CHANGE_IND

DESCRIPTION
  This function processes RAT change indication from SM.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rabm_process_rat_change_ind (void)
{
  word rab_id;

  MSG_HIGH_DS_0(RABM_SUB,"Received RAT-CHANGE from SM");

  /* ================================================================
  ** Change of RAT - initialize all the RABs in the list as invalid
  ** ================================================================*/
  for (rab_id=MIN_RAB_NUM; rab_id<MAX_RAB_NUM; rab_id++)
  {
    if (rab_list[rab_id].rab_state == RAB_ACTIVE)
    {
      
      //Send the RABM_RAB_RELEASE_IND to CM directly
      send_rabmcm_cmd((byte)rab_id, RABM_RAB_RELEASE_IND, (cc_cause_T) REASON_NONE);       
    }
    else if (rab_list[rab_id].rab_state == RAB_REESTABLISH_PENDING ||
                rab_list[rab_id].rab_state == RAB_SERVICE_REQ_PENDING ||
                rab_list[rab_id].rab_state == RAB_SERVICE_REQ_SENT )
    {
      
      //Stop the RAB timer in case it is running
      rabm_stop_timer(rab_id);
      //Send the RABM_RAB_REESTABLISH_REJ to CM directly
      send_rabmcm_cmd((byte)rab_id, RABM_RAB_REESTABLISH_REJ, (cc_cause_T) REASON_NONE);
    }
    rab_list[rab_id].rab_id = INVALID_RAB;
    rab_list[rab_id].rab_state = RAB_INACTIVE;
  }
}

/*===========================================================================
FUNCTION RABM_PROCESS_RAT_RESUME_IND

DESCRIPTION
  This function processes RAT resumption indication from SM.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rabm_process_rat_resume_ind (void)
{
  MSG_HIGH_DS_0(RABM_SUB,"Received RAT-RESUMPTION from SM");
}

/*===========================================================================
FUNCTION RABM_CHECK_RAB_DORMANT

DESCRIPTION
  This function returns TRUE if RAB state is inactive in RABM.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  NOne
===========================================================================*/
boolean rabm_check_rab_dormant(word rab_id)
{
  boolean status;
  status = (rab_list[rab_id].rab_state == RAB_INACTIVE ||
                rab_list[rab_id].rab_state == RAB_REESTABLISH_PENDING ||
                rab_list[rab_id].rab_state == RAB_SERVICE_REQ_PENDING||
                rab_list[rab_id].rab_state == RAB_SERVICE_REQ_SENT)
           ?TRUE:FALSE;
  return status;
}

/*===========================================================================
FUNCTION RABM_VERIFY_RAB_REEST_PEND_STATE

DESCRIPTION
  This function returns TRUE if RAB state is REETABLISH_PENDING.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  NOne
===========================================================================*/
boolean rabm_verify_rab_reest_pend_state(word rab_id)
{
  boolean status;
  status = (rab_list[rab_id].rab_state == RAB_REESTABLISH_PENDING)
           ?TRUE:FALSE;
  return status;
}

/*===========================================================================
FUNCTION RABM_CHECK_RAB_REEST_PEND

DESCRIPTION
  This function returns TRUE if any of RAB state is RAB_REESTABLISH_PENDING
  in RABM 

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  NOne
===========================================================================*/
boolean rabm_check_rab_reest_pend(void)
{
  word rab_id;
  for (rab_id = MIN_RAB_NUM; rab_id < MAX_PDP_NSAPI_NUM; rab_id++)
  {
    if (rab_list[rab_id].rab_state == RAB_REESTABLISH_PENDING)
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================
FUNCTION RABM_CHECK_SERV_REQ_IS_PEND

DESCRIPTION
  This function returns TRUE if any of RAB state is RAB_SERVICE_REQ_SENT
  in RABM 

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  NOne
===========================================================================*/
boolean rabm_check_serv_req_complete_pending(void)
{
  word rab_id;
  for (rab_id = MIN_RAB_NUM; rab_id < MAX_PDP_NSAPI_NUM; rab_id++)
  {
    if (rab_list[rab_id].rab_state == RAB_SERVICE_REQ_SENT)
    {
      return TRUE;
    }
  }
  return FALSE;
}


/*===========================================================================
FUNCTION RABM_SEND_GMMRABM_REESTABLISH_REQ

DESCRIPTION
  This function sends GMMRABM_REESTABLISH_REQ to GMM task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  NOne
===========================================================================*/
void rabm_send_gmmrabm_reestablish_req(byte rab_id)
{

  rrc_establish_cause_e_type rrc_est_cause;
  byte traffic_class;
  mm_cmd_type * gmm_cmd;

  /*------------------------------------------------------------
  From 24.008 Annex L, Table L.1.2
  Either "Originating Conversational Call" or "Originating Streaming Call" or
  "Originating Interactive Call" or "Originating Background Call " – depending on the
  Traffic Class in QoS of the "most demanding" RAB. (see Note 1)
  NOTE 1: For classification of "most demanding" Traffic Class the following ranking 
  order applies: "Conversational" followed by "Streaming" followed by "Interactive" 
  followed by "Background", where "Conversational" is the most demanding Traffic class
  in terms of being delay sensitive. In chosing the "most demanding" Traffic Class 
  all already active PDP Context together with the PDP Context to be activated
  shall be considered
  --------------------------------------------------------------*/
   traffic_class = sm_get_most_demanding_traffic_class();

  /* Convert the RRC Establish cause based on the Traffic class */
  rrc_est_cause = convert_to_rrc_est_cause( traffic_class );

  gmm_cmd = mm_get_cmd_buf();

  if (gmm_cmd != NULL)
  {
    gmm_cmd->cmd.gmmrabm_reestablish_req.header.message_set     = MS_GMM_RABM;
    gmm_cmd->cmd.gmmrabm_reestablish_req.header.message_id      = GMMRABM_REESTABLISH_REQ;
    gmm_cmd->cmd.gmmrabm_reestablish_req.header.message_len_lsb = 0;
    gmm_cmd->cmd.gmmrabm_reestablish_req.header.message_len_msb = 0; 
    /*Fill the traffic class*/
    gmm_cmd->cmd.gmmrabm_reestablish_req.traffic_class = 
          pdp_list[GET_TRANSID(rab_id)]->qos.traffic_class;
    
    gmm_cmd->cmd.gmmrabm_reestablish_req.cause = rrc_est_cause;

#ifdef FEATURE_DUAL_SIM
         gmm_cmd->cmd.gmmrabm_reestablish_req.as_id= (sys_modem_as_id_e_type)sm_as_id;
#endif

    gmm_cmd->cmd.gmmrabm_reestablish_req.uplink_data_status.length = 
                GMM_PDP_CONTEXT_STATUS_LENGTH;
    memset(gmm_cmd->cmd.gmmrabm_reestablish_req.uplink_data_status.value, 0,
                 GMM_PDP_CONTEXT_STATUS_LENGTH);
    if(rab_id < BYTE_LEN_INBITS)
    {
      b_packb(1, gmm_cmd->cmd.gmmrabm_reestablish_req.uplink_data_status.value, 
                       (word)((BYTE_LEN_INBITS - 1) - rab_id ), 1);
    } 
    else 
    {
      b_packb(1, gmm_cmd->cmd.gmmrabm_reestablish_req.uplink_data_status.value + 1, 
            (word)((2 * BYTE_LEN_INBITS - 1) - rab_id), 1);
    }
    // Donot Allow Resleections/HO when RRC query as the PDP Activation is in progress
    sm_ps_vote_for_reselection = FALSE;

    MSG_HIGH_DS_0( RABM_SUB,"Sending RAB Re-establish Req to GMM");
    mm_put_cmd (gmm_cmd);
  }
  else
  {
    MSG_ERROR_DS_0( RABM_SUB, "Unable to get a GMM command buffer");
  }          
}


/*===========================================================================
FUNCTION RABM_SEND_COMBINED_GMMRABM_REESTABLISH_REQ

DESCRIPTION
  This function sends GMMRABM_REESTABLISH_REQ to GMM task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  NOne
===========================================================================*/
void rabm_send_combined_gmmrabm_reestablish_req()
{
  rrc_establish_cause_e_type rrc_est_cause;
  byte traffic_class;
  mm_cmd_type * gmm_cmd;
  gmm_ie_pdp_context_status_T uplink_data_status; 
  byte rab_id;
  boolean serv_req_to_send;

  uplink_data_status.length = GMM_PDP_CONTEXT_STATUS_LENGTH;
  memset(uplink_data_status.value, 0, GMM_PDP_CONTEXT_STATUS_LENGTH);  

  traffic_class = (byte)SUBSCRIBED_CLASS;

  serv_req_to_send = FALSE;

  for(rab_id = MIN_RAB_NUM; rab_id < MAX_PDP_NSAPI_NUM; rab_id++)
  {
    if(rab_list[rab_id].rab_state == RAB_SERVICE_REQ_PENDING)         
    {
      serv_req_to_send = TRUE;

      rab_list[rab_id].rab_state = RAB_SERVICE_REQ_SENT;

      MSG_HIGH_DS_1(RABM_SUB,"RAB %d state changed to RAB_SERVICE_REQ_SENT",rab_id);      
    
      if( GET_TRANSID(rab_id) != INVALID_ID && 
           pdp_list[GET_TRANSID(rab_id)] != NULL && 
          (pdp_list[GET_TRANSID(rab_id)]->qos.traffic_class != (byte)SUBSCRIBED_CLASS) &&              
              (pdp_list[GET_TRANSID(rab_id)]->qos.traffic_class > traffic_class))
      {
        traffic_class = pdp_list[GET_TRANSID(rab_id)]->qos.traffic_class;
      }
      
      if(rab_id < BYTE_LEN_INBITS)
      {
        b_packb(1, uplink_data_status.value, (word)((BYTE_LEN_INBITS - 1) - rab_id ), 1);
      } 
      else 
      {
        b_packb(1, uplink_data_status.value + 1, (word)((2 * BYTE_LEN_INBITS - 1) - rab_id), 1);
      }
    }
  }
  
  if(serv_req_to_send) 
  { 

    rrc_est_cause = convert_to_rrc_est_cause(sm_get_most_demanding_traffic_class());    

    gmm_cmd = mm_get_cmd_buf();    

    if (gmm_cmd != NULL)
    {
      gmm_cmd->cmd.gmmrabm_reestablish_req.header.message_set     = MS_GMM_RABM;
      gmm_cmd->cmd.gmmrabm_reestablish_req.header.message_id      = GMMRABM_REESTABLISH_REQ;
      gmm_cmd->cmd.gmmrabm_reestablish_req.header.message_len_lsb = 0;
      gmm_cmd->cmd.gmmrabm_reestablish_req.header.message_len_msb = 0; 

      gmm_cmd->cmd.gmmrabm_reestablish_req.traffic_class = traffic_class;          
    
      gmm_cmd->cmd.gmmrabm_reestablish_req.cause = rrc_est_cause;    

      gmm_cmd->cmd.gmmrabm_reestablish_req.uplink_data_status = uplink_data_status;

#ifdef FEATURE_DUAL_SIM
         gmm_cmd->cmd.gmmrabm_reestablish_req.as_id= (sys_modem_as_id_e_type)sm_as_id;
#endif

      MSG_HIGH_DS_0( RABM_SUB,"Sending combined RAB Re-establish Req to GMM");

      mm_put_cmd (gmm_cmd);
    }
    else
    {
      MSG_ERROR_DS_0( RABM_SUB, "Unable to get a GMM command buffer");
    }         
  }
}

/*===========================================================================
FUNCTION RABM_PROCEDURE_PENDING

DESCRIPTION 
   This function returns TRUE if any of rab state is either active pending, 
    deactive pending or modify pending
 

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  NOne
===========================================================================*/

boolean rabm_check_procedure_pending()
{
  word  rab_id;
  for (rab_id = MIN_RAB_NUM;  rab_id < MAX_RAB_NUM; rab_id++)
  {
    if (rab_list[rab_id].rab_state == RAB_ACTIVE_PENDING ||
         rab_list[rab_id].rab_state == RAB_INACTIVE_PENDING || 
         rab_list[rab_id].rab_state == RAB_MODIFY_PENDING || 
         rab_list[rab_id].rab_state == RAB_REESTABLISH_PENDING)
    {
      return TRUE;  
    }
  }
  return FALSE;
}

/*===========================================================================
FUNCTION RABM_IS_RAB_ACTIVE

DESCRIPTION
  This function returns TRUE if any of the RAB is active.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  NOne
===========================================================================*/

boolean rabm_is_rab_active
(
#ifdef FEATURE_DUAL_SIM
      sys_modem_as_id_e_type as_id
#else
      void
#endif
)

{
  byte i;
#ifdef FEATURE_DUAL_SIM
  if(SM_IS_VALID_AS_ID(as_id) == FALSE)
      return FALSE;
#endif
  /*Check any of the RAB is active*/
  for(i=MIN_RAB_NUM; i<MAX_RAB_NUM; i++)
  {
#ifdef FEATURE_DUAL_DATA
    if (rab_list_sim[as_id][i].rab_state == RAB_ACTIVE)
#else
    if (rab_list[i].rab_state == RAB_ACTIVE)
#endif
    {
      MSG_HIGH_DS_1(RABM_SUB, "RAB is active for RAB -%d", i);
      return TRUE;
    }
  }
  return FALSE;
}


/*===========================================================================
FUNCTION RABM_PROCEDURE_PENDING_FOR_SCRI

DESCRIPTION 
   This function returns TRUE if any of rab state is either active active pending, 
    deactive pending or modify pending
 

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  NOne
===========================================================================*/

boolean rabm_check_procedure_pending_for_SCRI()
{
  word  rab_id;
  for (rab_id = MIN_RAB_NUM;  rab_id < MAX_RAB_NUM; rab_id++)
  {
    if ( rab_list[rab_id].rab_state == RAB_ACTIVE ||
         rab_list[rab_id].rab_state == RAB_ACTIVE_PENDING ||
         rab_list[rab_id].rab_state == RAB_INACTIVE_PENDING || 
         rab_list[rab_id].rab_state == RAB_MODIFY_PENDING || 
         rab_list[rab_id].rab_state == RAB_REESTABLISH_PENDING)
    {
      return TRUE;  
    }
  }
  return FALSE;
}

/*===========================================================================
FUNCTION RABM_PACK_UPLINK_DATA_STATUS

DESCRIPTION
  This function packs the Ul Data status IE for 16 NSAPIs(0-15) in 16 bits

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  NOne
===========================================================================*/
void rabm_pack_uplink_data_status(byte *ext_buf, sys_modem_as_id_e_type as_id)

{
  byte rab_id;


  //Reset the buffer
  (void)memset(ext_buf, 0, 2);

#if defined(FEATURE_DUAL_ACTIVE) && defined(FEATURE_NAS_DUAL_PS_SATCK)
  if(SM_IS_VALID_AS_ID(as_id) == FALSE)
  {
    return;
  }
#endif

  for(rab_id = MIN_RAB_NUM; rab_id < MAX_PDP_NSAPI_NUM; rab_id++)
  {
    if(rab_list[rab_id].rab_state == RAB_SERVICE_REQ_SENT)         
    {    
      //Pack the status
      if(rab_id < BYTE_LEN_INBITS)
      {
        b_packb(1, ext_buf, (word)((BYTE_LEN_INBITS - 1) - rab_id ), 1);
      } 
      else 
      {
        b_packb(1, ext_buf + 1, (word)((2 * BYTE_LEN_INBITS - 1) - rab_id), 1);
      }
    }
  }
  
}

void rabm_send_rab_est_ind_to_gmm(void)
{
  boolean send_gmm_msg = FALSE;
  mm_cmd_type * gmm_cmd;

#ifdef FEATURE_NAS_REL11
  send_gmm_msg = TRUE;
#else
  if(mm_get_optimized_ps_signaling_connection_release())
  {
    send_gmm_msg = TRUE;
  }
#endif

  if(send_gmm_msg)
  {
      /* Send RAB established ind to GMM so that it stops the timer T3340.*/
        gmm_cmd = mm_get_cmd_buf();
        if (gmm_cmd != NULL)
        {
          gmm_cmd->cmd.gmmrabm_rab_est_ind.header.message_set     = MS_GMM_RABM;
          gmm_cmd->cmd.gmmrabm_rab_est_ind.header.message_id      = GMMRABM_RAB_EST_IND;
          gmm_cmd->cmd.gmmrabm_rab_est_ind.header.message_len_lsb = 0;
          gmm_cmd->cmd.gmmrabm_rab_est_ind.header.message_len_msb = 0; 
                        
#ifdef FEATURE_DUAL_SIM
         gmm_cmd->cmd.gmmrabm_rab_est_ind.as_id= (sys_modem_as_id_e_type)sm_as_id;
#endif
  
          MSG_HIGH_DS_0( RABM_SUB,"Sending RAB established ind to GMM");
          mm_put_cmd (gmm_cmd);
        }
        else
        {
          MSG_ERROR_DS_0( RABM_SUB, "Unable to get a GMM command buffer");
        }
  }
  
}

