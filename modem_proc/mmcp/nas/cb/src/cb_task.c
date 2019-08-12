/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     CB MAIN MODULE (CB_TASK.C)

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001,2002,2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cb/src/cb_task.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/18/11   hc      Removed #include "gsdi_exp.h" 
03/18/08   rk      Added Support to merge MBMS SM in CB task
03/28/05   kvk     Added suport for UMTS BMC. The UMTS BMC code feature wrapped 
                   in FEATURE_UMTS_BMC.
08/02/04   vdr     Deleted unnesesary comments
07/14/04   vdr     Now CB task uses new interface to receive messages from
                   other tasks
06/14/04   vdr     Now using cb_alloc and cb_free to allocate and deallocate
                   CB memory
04/16/04   vdr     Featurizing out CB DRX mode processing
                   Added some extra comments
07/03/03   jca     Initial version

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_CB

#include <stdio.h>
#include <string.h>
#include "rex.h"
#include "task.h"
#include "dog_hb_rex.h"
#include "queue.h"
#include "err.h"
#include "sys_v.h"
#include  "cbmsg.h"
#include "cb_schedule.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "ms.h"
#include "gs_v.h"
#include "ms_timer_v.h"
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
#include "cb_task.h"
#include "cbtask.h"
#include "cb_utils.h"
#include "cb_state.h"
#include "cb_mem.h"
#ifdef FEATURE_MBMS
#error code not present
#endif

#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif



rex_tcb_type* cb_tcb_ptr = NULL;

extern cb_as_id_e_type cb_as_id ;
#if defined(FEATURE_DUAL_SIM)||defined(FEATURE_TRIPLE_SIM)
extern cb_scheduling_table_type *cb_bmc_scheduling_table_sim[MAX_AS_IDS] ;
#define  cb_bmc_scheduling_table cb_bmc_scheduling_table_sim[cb_as_id]
extern cb_bmc_new_msg_bit_map_info_t *prev_new_msg_bitmap_sim[MAX_AS_IDS] ;
#define  prev_new_msg_bitmap prev_new_msg_bitmap_sim[cb_as_id]
extern cb_bmc_new_msg_bit_map_info_t *curr_new_msg_bitmap_sim[MAX_AS_IDS];
#define  curr_new_msg_bitmap curr_new_msg_bitmap_sim[cb_as_id]
extern cb_bmc_new_msg_bit_map_info_t *last_curr_new_msg_bitmap_sim[MAX_AS_IDS];
#define  last_curr_new_msg_bitmap last_curr_new_msg_bitmap_sim[cb_as_id]
extern byte cb_bmc_scheduling_period_length_sim[MAX_AS_IDS] ;
#define  cb_bmc_scheduling_period_length cb_bmc_scheduling_period_length_sim[cb_as_id]
extern boolean cb_supported_sim[MAX_AS_IDS];
#define cb_supported cb_supported_sim[cb_as_id]
extern cb_state_type cb_state_sim[MAX_AS_IDS] ;
#define cb_state cb_state_sim[cb_as_id]
extern cb_page_assembling_state_type page_assembling_state_sim[MAX_AS_IDS];
#define page_assembling_state page_assembling_state_sim[cb_as_id]
extern byte current_plmn_sim[MAX_AS_IDS][PLMN_VALUE_LENGTH] ;
#define current_plmn current_plmn_sim[cb_as_id][PLMN_VALUE_LENGTH]
/* Current LAC */
extern byte current_lac_sim[MAX_AS_IDS][LAC_VALUE_LENGTH] ;
#ifndef FEATURE_NAS_CBS_DSDS
#define current_lac current_lac_sim[cb_as_id][LAC_VALUE_LENGTH]
#endif
/* Cuppent Cell ID */
extern byte current_cell_id_sim[MAX_AS_IDS][CELL_ID_VALUE_LENGTH]  ;
#ifndef FEATURE_NAS_CBS_DSDS
#define current_cell_id current_cell_id_sim[cb_as_id][CELL_ID_VALUE_LENGTH]
#endif
extern    cb_rat_type bmc_active_rat_sim[MAX_AS_IDS];
#define   bmc_active_rat bmc_active_rat_sim[cb_as_id]
extern dword cb_dup_timer_value_sim[MAX_AS_IDS];
#define   cb_dup_timer_value cb_dup_timer_value_sim[cb_as_id]
#else
extern cb_scheduling_table_type * cb_bmc_scheduling_table;
extern cb_bmc_new_msg_bit_map_info_t * prev_new_msg_bitmap;
extern cb_bmc_new_msg_bit_map_info_t *curr_new_msg_bitmap;
extern cb_bmc_new_msg_bit_map_info_t *last_curr_new_msg_bitmap;
extern byte cb_bmc_scheduling_period_length;
extern boolean cb_supported;
extern cb_state_type cb_state;
extern cb_page_assembling_state_type page_assembling_state;
extern byte cb_current_plmn[PLMN_VALUE_LENGTH] ;
   
/* Current LAC */
extern byte current_lac[LAC_VALUE_LENGTH] ;

/* Cuppent Cell ID */
extern byte current_cell_id[CELL_ID_VALUE_LENGTH] ;
extern cb_rat_type bmc_active_rat;
extern dword cb_dup_timer_value;
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/




/* ----------------------------
** Define watchdog report handle
** ---------------------------- */
  #undef  DOG_CB_RPT
  #define DOG_CB_RPT       cb_dog_rpt_var
  static dog_report_type   cb_dog_rpt_var  = 0;      /* Initial Safety */

/* -----------------------------
** Command queues for the CB task
** ----------------------------- */
q_type cb_cmd_q ;
q_type cb_cmd_free_q ;

#ifdef FEATURE_UMTS_BMC
q_type cb_rlc_cmd_q ;
q_type cb_rlc_cmd_free_q ;
#endif

#ifdef FEATURE_MBMS
#error code not present
#endif
#ifndef FEATURE_MODEM_HEAP
/*
** CB task memory allocator indicator
**
** If this indicator is set to TRUE then memory pool is available
** and CB task can process another layer's requests, otherwise CB
** task ignors any requests or indications
*/

static boolean cb_memory_available_indicator = FALSE ;
#endif
/*===========================================================================

FUNCTION CB_SEND_CMD

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cb_send_cmd(cb_command_type *cmd_ptr)
{

   /* Check for Null pointer */
   if(cmd_ptr != NULL)
   {
      /* First initialize the queue link. This is always done
      before putting an item in a queue. */
      (void) q_link(cmd_ptr, &(cmd_ptr->link));

      /* Put the message on the queue */
      q_put(&cb_cmd_q, &cmd_ptr->link);

      /* Set the command queue signal for */
     /* receive task                     */
     (void) rex_set_sigs(cb_tcb_ptr, CB_CMD_Q_SIG);

   }
   else
   {
      /* Raise error condition */
      MSG_FATAL_DS( cb_as_id,"Trying to put a null pointer on the CB queue", 0,0,0);
   }
}


/*===========================================================================

FUNCTION CB_GET_CMD

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
cb_command_body_type *cb_get_cmd(byte message_set, byte message_id)
{
  cb_command_body_type *cb_cmd_ptr = NULL ;
#ifndef FEATURE_MODEM_HEAP
  if ((cb_cmd_ptr = (cb_command_body_type*) gs_alloc(sizeof(cb_command_body_type))) == NULL)
#else
  if ((cb_cmd_ptr = (cb_command_body_type*) modem_mem_calloc(1,sizeof(cb_command_body_type), MODEM_MEM_CLIENT_NAS)) == NULL)
#endif
  {
    MSG_FATAL_DS( cb_as_id,"CB: Not enough memory",0,0,0) ;
  }

  cb_cmd_ptr->message_header.message_set = message_set ;
  cb_cmd_ptr->message_header.message_id = message_id ;

  cb_cmd_ptr->message_header.message_len_lsb = 0 ;
  cb_cmd_ptr->message_header.message_len_msb = 0 ;

  switch (message_set)
  {
    case MS_WMS_CB:
      switch (message_id)
      {
        case CB_SEARCH_LIST_REQ:
        case CB_DATA_IND:
        case CB_CELL_CHANGE_IND:
        case CB_DELETE_PAGE_REQ:
        case CB_DELETE_ALL_REQ:
#ifdef FEATURE_NAS_CBS_DSDS
        case CB_DUAL_STANDBY_STATUS_IND:
#endif           
          break ;
        default:
          MSG_ERROR_DS_1(cb_as_id,"Unknown message ID (%d)",message_id) ;
          break ;
      }
      break ;
    case MS_CB_RR:
      if (message_id != RR_CELL_CHANGE_IND)
      {
        MSG_ERROR_DS_1(cb_as_id,"Unknown message ID (%d)",message_id) ;
      }
      break ;
    case MS_CB_L2:
      if (message_id != (int)DL_CB_BLOCK_IND)
      {
        MSG_ERROR_DS_1(cb_as_id,"Unknown message ID (%d)",message_id) ;
      }
      break ;
    case MS_CB_L1:
      break ;
    default :
      MSG_ERROR_DS_1(cb_as_id,"Unknown message set (%d)",message_set) ;
      break ;
  }

  return cb_cmd_ptr ;
} /* end cb_get_cmd() */

/*===========================================================================

FUNCTION CB_PUT_CMD

DESCRIPTION
  This function is to be used in order to send messages to the CB task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_put_cmd(cb_command_body_type *cmd_ptr)
{
  gs_status_T    gs_status;

  gs_status = gs_send_message(GS_QUEUE_CB,(void *)cmd_ptr,TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_ERROR_DS_0(cb_as_id,"Unable to send message to CB task");
  }
} /* end cb_put_cmd() */

/*===========================================================================

FUNCTION CB_WAIT

DESCRIPTION
  Main Wait routine for CB task. This routine performs the following functions:
    - Checks CB input queue to see if anything is available.  The CB queue
      will be checked to see if anything is available before performing
      a rex_wait().  (There is no need to wait if something is on the queue
      already).  If an item is found on the queue the corresponding bit in
      the returned mask is set.
    - Kicks the watchdog timer while waiting for the specified mask.

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals) The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type cb_wait
(
   rex_sigs_type requested_mask    /* Mask of signals to wait for */
)
{
   rex_sigs_type queue_mask;        /* Mask of signals indicating queue status */
   rex_sigs_type rex_signals_mask;  /* Mask of signals returned by REX     */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Initialize temporary mask holder for queue information */
   queue_mask = 0;

   /* --------------------------------------------------------------
   ** Check message queue:  If the request mask contains a check for
   ** CB_CMD_Q_SIG then we should check the queue, and set in
   ** the queue_mask an indication that something is waiting to be
   ** serviced on that queue.  The rex signal is also cleared
   ** regardless of the queue count.  This is to cover the case
   ** where there is a signal set but nothing on the queue.
   ** ------------------------------------------------------------- */
   if (requested_mask & CB_CMD_Q_SIG)
   {
      (void) rex_clr_sigs( cb_tcb_ptr, CB_CMD_Q_SIG );

      if (q_cnt( &cb_cmd_q ) )
      {
         queue_mask |= CB_CMD_Q_SIG;
      }
   }

#ifdef FEATURE_UMTS_BMC
   if (requested_mask & CB_RLC_CMD_Q_SIG)
   {
     (void) rex_clr_sigs( cb_tcb_ptr, CB_RLC_CMD_Q_SIG );

     if (q_cnt( &cb_rlc_cmd_q ) )
     {
        queue_mask |= CB_RLC_CMD_Q_SIG;
     }
   }

#endif

#ifdef FEATURE_MBMS
   #error code not present
#endif

   /* Get current state of rex signals */
   rex_signals_mask = rex_get_sigs( cb_tcb_ptr );

   /* --------------------------------------------------------------------
   ** Loop until the current signals from REX (including the queue signals
   ** faked due to an item on a queue, if any) match with one or more of
   ** the requested signals.
   ** -------------------------------------------------------------------- */
   do
   {
      /* Kick the watchdog */
      /* Report to the Dog HB */
      dog_hb_report(DOG_CB_RPT);

      /* Perform the 'real' wait (if needed) */
      if (((rex_signals_mask | queue_mask) & requested_mask) == 0)
      {
         rex_signals_mask = rex_wait( requested_mask | CB_DOGRPT_TIMER_SIG );
         /* Clear the watchdog signal */
       (void) rex_clr_sigs( cb_tcb_ptr, CB_DOGRPT_TIMER_SIG);

      }

      /* ---------------------------------------------------------------
      ** Make sure that if we got a queue signal, that there is actually
      ** an item on the queue indicated
      ** --------------------------------------------------------------- */
      if (rex_signals_mask & CB_CMD_Q_SIG)
      {
         (void) rex_clr_sigs( cb_tcb_ptr, CB_CMD_Q_SIG );

         if (q_cnt( &cb_cmd_q ) == 0)   /* No elements in queue ? */
         {
            rex_signals_mask &= ~CB_CMD_Q_SIG;  /* Turn off bit */
         }

      }

#ifdef FEATURE_UMTS_BMC
      if (rex_signals_mask & CB_RLC_CMD_Q_SIG)
      {
         (void) rex_clr_sigs( cb_tcb_ptr, CB_RLC_CMD_Q_SIG );

         if (q_cnt( &cb_rlc_cmd_q ) == 0)   /* No elements in queue ? */
         {
            rex_signals_mask &= ~CB_RLC_CMD_Q_SIG;  /* Turn off bit */
         }

      }

#endif

#ifdef FEATURE_MBMS
      #error code not present
#endif

   /* do while mask requested does not match current mask */
   } while (((rex_signals_mask | queue_mask) & requested_mask) == 0);

   return ((rex_signals_mask | queue_mask));

} /* end cb_wait() */

/*===========================================================================

FUNCTION  cb_handle_message

DESCRIPTION
  

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_handle_message( cb_command_type* message )
{
  if ( message )
  {
    /* -----------------------------
    ** Process the retreived message
    ** ----------------------------- */
    cb_main_state_machine_handler( message );
  }
  else
  {
     MSG_ERROR_DS_0(cb_as_id,"NO CB message!");
  }
}


/*===========================================================================

FUNCTION CB_QUEUE_INIT

DESCRIPTION
  This function initializes the CB task input queues

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_init_before_task_start
(
  void
)
{
  static cb_command_type cb_command_pool[GS_DEFAULT_QUEUE_SIZE*2] ;
#ifdef FEATURE_UMTS_BMC
  static cb_command_type cb_rlc_command_pool[GS_DEFAULT_QUEUE_SIZE*2];
  byte *rlc_msg_store = (byte *)cb_rlc_command_pool;
#endif
  cb_command_type   *tmp_ptr = NULL;
  q_link_type *link_item = NULL ;
  byte        i=0;
  byte *msg_store = (byte *)cb_command_pool ;
  

  (void) q_init(&cb_cmd_q);
  (void) q_init(&cb_cmd_free_q);

#ifdef FEATURE_UMTS_BMC
  /*Initialize the RLC CB queue*/
  (void) q_init(&cb_rlc_cmd_q);
  (void) q_init(&cb_rlc_cmd_free_q);
#endif

  for (i=0; i<GS_DEFAULT_QUEUE_SIZE*2; i++)
  {
    /* Add item(s) to the cb_cmd_free_q */
    tmp_ptr = (cb_command_type *) msg_store;
    link_item = q_link( &tmp_ptr->command, &tmp_ptr->link );
    q_put(&cb_cmd_free_q, link_item);
    msg_store += sizeof(cb_command_type);

#ifdef FEATURE_UMTS_BMC
    /* Add item(s) to the cb_rlc_cmd_free_q */
    tmp_ptr = (cb_command_type *) rlc_msg_store;
    link_item = q_link( &tmp_ptr->command, &tmp_ptr->link );
    q_put(&cb_rlc_cmd_free_q, link_item);
    rlc_msg_store += sizeof(cb_command_type);
#endif    
    
  }

#ifdef FEATURE_MBMS
  #error code not present
#endif

} /* end cb_queue_init() */


/*===========================================================================

FUNCTION CB_BMC_VERIFY_CB_MAIN_Q

DESCRIPTION
  This function reads the main CB queue and process the commands in main 
control queue

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_UMTS_BMC
void cb_bmc_verify_cb_main_q(void)
{
  gs_read_messages(GS_QUEUE_CB);
  return;
}
#endif

/*===========================================================================

FUNCTION cb_init_global

DESCRIPTION
  This function intialize the global variables

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cb_init_global(void)
{
 #if defined (FEATURE_DUAL_SIM)||defined(FEATURE_TWO_3GPP_SIM)||defined(FEATURE_TRIPLE_SIM) 
 for(cb_as_id=CB_AS_ID_1;cb_as_id<(cb_as_id_e_type)MAX_AS_IDS;cb_as_id++)
 #endif
 {
   //NULL Part Intialization start here 
  basic_cbch_list=NULL;  
  current_message=NULL;  
  basic_cbch_list_entry_point=NULL;  
  cb_bmc_scheduling_table=NULL;  
  prev_new_msg_bitmap=NULL;  
  curr_new_msg_bitmap=NULL;  
  last_curr_new_msg_bitmap=NULL;  
  
  //intialize as zero
  cb_bmc_rlc_pdu_size=0;  
  cb_bmc_scheduling_period_length=0;  
  
  //Intialize as FALSE
  cb_bmc_reading_advised=FALSE;
  cb_bmc_reading_optional=FALSE;
  cb_supported=FALSE;
  
  //intialize as BMC_INVALID_SFN;
  cb_current_sfn=BMC_INVALID_SFN;
  current_last_sfn_sch_prd=BMC_INVALID_SFN;
  current_first_sfn_sch_prd=BMC_INVALID_SFN;
  current_first_sch_msg_sfn=BMC_INVALID_SFN;
  next_first_sfn_sch_prd=BMC_INVALID_SFN;
  next_last_sfn_sch_prd=BMC_INVALID_SFN;
  next_first_sch_msg_sfn=BMC_INVALID_SFN;
  next_last_sch_msg_sfn=BMC_INVALID_SFN;
  
  //Intialize GSM state
  page_assembling_state= CB_RECEIVING_FIRST_PB_STATE;
  
  // intialize 
  page_assembling_state=CB_RECEIVING_FIRST_PB_STATE;
  cb_state.state=CB_IDLE_STATE;
  cb_state.idle_substate=CB_INACTIVE_SUBSTATE;
  
  //bmc active rat
  bmc_active_rat=CB_GSM_MODE;
  
  /*Timer 1 initialization*/
  cb_dup_timer_value = CB_DUP_TIMER_VALUE_INVALID;
  cb_next_min_offset = CB_DUP_TIMER_VALUE_INVALID;
  memset(cb_timer_status,0x0,CB_MAX_TIMERS);

  memset(cb_current_plmn,0xff,PLMN_VALUE_LENGTH);
  memset(current_lac,0xff,LAC_VALUE_LENGTH);
  memset(current_cell_id,0xff,CELL_ID_VALUE_LENGTH); 
 } 
}

/*===========================================================================

FUNCTION CB_MAIN

DESCRIPTION
  Entry point for the CB task.  This function performs task initialisation,
  then remains in an infinite loop, waiting on an input queue, and responding to
  messages received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_main( dword dummy )
/*lint -esym(715,dummy) */
{
   static rex_sigs_type sigs;

#ifndef FEATURE_MODEM_RCINIT
   cb_init_before_task_start();
#endif
   cb_init_global();
   cb_tcb_ptr = rex_self();

   /* -------------------------
   ** Initialize watchdog timer
   ** Register with DOG HB new API
   ** ------------------------- */
   cb_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) CB_DOGRPT_TIMER_SIG);

#ifdef FEATURE_UMTS_BMC
   /*Init RLC state*/
   bmc_rlc_state_init();
#endif

  if (               gs_init(GS_QUEUE_CB,
                             rex_self(),
                             CB_CMD_Q_SIG,
                             &cb_cmd_q,
                             &cb_cmd_free_q,
                             sizeof(cb_command_type),
                             GS_DEFAULT_QUEUE_SIZE*2,
         (gs_get_handler_ptr)cb_handle_message) != GS_SUCCESS)
  {
      MSG_ERROR_DS_0(cb_as_id,"CB: Failed to initialize input queue!");
  }

#ifdef FEATURE_UMTS_BMC
  if (               gs_init(GS_QUEUE_RLC_CB,
                             rex_self(),
                             CB_RLC_CMD_Q_SIG,
                             &cb_rlc_cmd_q,
                             &cb_rlc_cmd_free_q,
                             sizeof(cb_command_type),
                             GS_DEFAULT_QUEUE_SIZE*2,
         (gs_get_handler_ptr)cb_handle_message) != GS_SUCCESS)
  {
      MSG_ERROR_DS_0(cb_as_id,"CB: Failed to initialize RLC input queue!");
  }
#endif

   /* Clear signals */
   (void) rex_clr_sigs(rex_self(), (rex_sigs_type)~0);

#ifdef FEATURE_MODEM_RCINIT
   rcinit_handshake_startup();
#else
   tmc_task_start();
#endif

   /* Allocate memory pools */
#ifndef FEATURE_MODEM_HEAP
   cb_memory_available_indicator = cb_mem_init() ;
   
   if (cb_memory_available_indicator == FALSE)
   {
      MSG_HIGH_DS_0(cb_as_id,"Not enough memory for CB task");
   }
#endif
   for(;;)
   {
#ifdef FEATURE_UMTS_BMC
      /* Wait on signal */
      sigs = cb_wait( CB_CMD_Q_SIG | CB_RLC_CMD_Q_SIG 
#ifdef FEATURE_MBMS    
      #error code not present
#endif

#ifdef TEST_FRAMEWORK
      #error code not present
#endif

      );
#else
      sigs = cb_wait( CB_CMD_Q_SIG 
#ifdef FEATURE_MBMS    
      #error code not present
#endif

#ifdef TEST_FRAMEWORK
      #error code not present
#endif

      );

#endif

      

      /* If this signal is for the command queue */
      if (sigs & CB_CMD_Q_SIG)
      {
         /* Clear signals */
         (void) rex_clr_sigs( cb_tcb_ptr, CB_CMD_Q_SIG );

         /*
         ** Free processed buffer
         */

         gs_read_messages(GS_QUEUE_CB);      
      }

#ifdef FEATURE_UMTS_BMC
      if (sigs & CB_RLC_CMD_Q_SIG)
      {
         /* Clear signals */
         (void) rex_clr_sigs( cb_tcb_ptr, CB_RLC_CMD_Q_SIG );

         /*
         ** Free processed buffer
         */

         gs_read_messages(GS_QUEUE_RLC_CB);      
      }
#endif

#ifdef FEATURE_MBMS
      #error code not present
#endif

#ifdef TEST_FRAMEWORK
      #error code not present
#endif


   }/* FOREVER */
}
 

 
#endif /* #ifdef FEATURE_GSM_CB */
