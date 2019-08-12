/*===========================================================================
                        T E S T  C O N T R O L  ( T C )    T A S K  

DESCRIPTION

   This file contains the task entry point for TC task. It implements the 
   TC task initialization, including initialization of various procedures.
   and setting of the TC state. TC task enters a forever loop after it is
   initialized and waits for the REX signals.


EXTERNALIZED FUNCTIONS   
    void tc_task()
    --------------
      It is the main entry point for TC Task.

    void tc_put_cmd(tc_cmd_type *cmd_ptr)
    -------------------------------------
      Function used by other entities to put a command in TC command Queue.

    tc_cmd_type *tc_get_cmd_buf(void)
    ---------------------------------
      Function used by other entities to get TC command type.

                  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/ctest/vcs/tctask.c_v   1.7   25 Mar 2002 15:30:42   madiraju  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/tc/src/tctask.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/06   rk      changed include file loopback.h to tc_loopback.h
09/15/05   hj      Changed storage class of tc_cmd_ptr, to get around the 
                   problem of tc heap corruption.
08/31/04   mks     Added support for handling multiple radio bearers during
                   loopback call setup
03/25/02   mks     Added handler for TC internal messages 
03/04/02   mks     Notify tctask when DL data arrives from RLC
02/06/02   mks     Added handlers for CM and RRC commands
09/17/01   ks      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
//NPR
#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_TC

#include "comdef.h"
#include "rex.h"
#include "dog.h"
#include "tc.h"
#include "tctask.h"
#include "tctask_v.h"
#include "tc_loopback.h"
#include "ms.h"

#ifndef FEATURE_MODEM_RCINIT_PHASE2
/* Include the Task Controller file */
#include "tmc.h"
#endif
#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define TC_PD    0x0F


rb_test_state_T rb_test_mode;  /* identifies the current state of the TC */
loopback_state_T tc_current_state;
sys_radio_access_tech_e_type tc_rat_rab_established_on;

/* ----------------------------
** Define watchdog report timer
** ---------------------------- */
rex_timer_type tc_rpt_timer;

rex_tcb_type* tc_tcb_ptr;

#ifdef FEATURE_NAS_WDOG_DYNAMIC   /* Dynamic Dog Interface */
  #undef  DOG_TC_RPT
  #define DOG_TC_RPT       tc_dog_rpt_var
  static dog_report_type   tc_dog_rpt_var  = 0;      /* Initial Safety */

  #undef  DOG_TC_RPT_TIME
  #define DOG_TC_RPT_TIME  tc_dog_rpt_time_var
  static uint32            tc_dog_rpt_time_var  = 0xffff; /* Initial Safety */
#endif   /* FEATURE_NAS_WDOG_DYNAMIC */


/* ========================================================================
** EXTERNAL FUNCTION DEFINITIONS.
** ====================================================================== */

/*===========================================================================

FUNCTION LB_RX_PROCESS_DATA

DESCRIPTION
  This function processes DL data received from RLC 

===========================================================================*/
extern void lb_rx_process_data(uint32 wm_id);

/*===========================================================================

FUNCTION TC_PROCESS_MM_CMD

DESCRIPTION
  This function processes the MM command during the Loopback mode operation.

===========================================================================*/
extern void tc_process_mm_cmd(tc_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION TC_PROCESS_MM_CMD

DESCRIPTION
  This function processes the GMM command during the Loopback mode operation.

===========================================================================*/
extern void tc_process_gmm_cmd(tc_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION TC_PROCESS_RABM_CMD

DESCRIPTION
  This function processes the RABM command during the Loopback mode operation.

===========================================================================*/
extern void tc_process_rabm_cmd(tc_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION TC_PROCESS_CM_CMD

DESCRIPTION
  This function processes the CM command during the Loopback mode operation.

===========================================================================*/
extern void tc_process_cm_cmd(tc_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION TC_PROCESS_RRC_CMD

DESCRIPTION
  This function processes the RRC command during the Loopback mode operation.

===========================================================================*/
extern void tc_process_rrc_cmd(tc_cmd_type *cmd_ptr);

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
/* Allocate an array of maximum number of Loopback RAB contexts */
rab_context_info  *lb_rab_list[MAX_RAB_TO_SETUP] = {0};

#ifndef FEATURE_MODEM_HEAP
rab_context_info  lb_rab_list_static [MAX_RAB_TO_SETUP];
#endif

boolean post_self_cmd_flag[MAX_RAB_TO_SETUP][UE_MAX_DL_LOGICAL_CHANNEL]; 

/* =========================================================================
** LOCAL Data Declarations
** ======================================================================= */
LOCAL q_type tc_cmd_q;    /* CMD QUEUE for TC */

/* =========================================================================
** LOCAL FUNCTION PROTOTYPES.
** ======================================================================== */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================

FUNCTION TC_NAS_INIT_BEFORE_TASK_START

DESCRIPTION
  This function does the initialization for TC task. It means initializing 
  all TC global data.   
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tc_init_before_task_start( void )
{
  /* Initialize all the command queues */
  (void)q_init(&tc_cmd_q);
}

/*===========================================================================

FUNCTION TC_NAS_INIT

DESCRIPTION
  This function does the initialization for TC task. It means initializing 
  all TC global data.   
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tc_nas_init( void )
{
  word i;

  /* ---------------------------------------------------------------- 
  ** Initialize the following fields of the PDP
  ** transaction Id for all the PDP contexts as INVALID_ID
  ** --------------------------------------------------------------*/
  for (i=0; i<MAX_RAB_TO_SETUP; i++)
  {
    lb_rab_list[i] = NULL;  
  }

  // Initialize current TC state to default value
  tc_current_state = TC_UNKNOWN_MODE;
  rb_test_mode = RB_UNKNOWN_MODE;
  tc_rat_rab_established_on = SYS_RAT_NONE;

#ifdef FEATURE_NAS_WDOG_DYNAMIC
  tc_dog_rpt_var = dog_register( tc_tcb_ptr, \
                                  DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE);
  tc_dog_rpt_time_var = dog_get_report_period( tc_dog_rpt_var );
   
  /* If the return value from dog_get_report_period() was zero then we MUST
  ** NOT report to dog directly, as DOG will monitor us in a different manner.
  ** Hence we won't initialize or start any timer. */
  if( tc_dog_rpt_time_var > 0 )
#endif /* ! FEATURE_NAS_WDOG_DYNAMIC */
  {
    /* Initialize taskname watchdog report timer */
    rex_def_timer( &tc_rpt_timer, tc_tcb_ptr, TC_DOGRPT_TIMER_SIG);
  }
  
  /* Any other initializations will go here */
}

/*===========================================================================

FUNCTION TC_NAS_RAB_LIST_INIT

DESCRIPTION
  This function does the initialization for rab_list
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tc_nas_rab_list_init( byte rab_id )
{
  word i;

  lb_rab_list[rab_id]->cn_domain_type  = UNKNOWN_DOMAIN;  
  lb_rab_list[rab_id]->rab_id          = INVALID_ID;
  lb_rab_list[rab_id]->logical_chan_id = INVALID_ID;
  /*lint -e641 */
  lb_rab_list[rab_id]->sdu_type        = UNKNOWN_SDU_TYPE;
  lb_rab_list[rab_id]->rab_established= FALSE;
  /*lint +e641 */
  for (i=0; i<UE_MAX_UL_LOGICAL_CHANNEL; i++)
  {
    lb_rab_list[rab_id]->ul_rlc_sdu_size[i] = 0; 
    lb_rab_list[rab_id]->ul_rlc_sdu_size_valid[i] = TRUE; 
  }

  
  // Initialize the UL/DL water marks for RLC data
  lb_init_dlrlc_wm(rab_id);
  lb_init_ulrlc_wm(rab_id);

  for (i=0; i<UE_MAX_DL_LOGICAL_CHANNEL; i++)
  {
    post_self_cmd_flag[rab_id][i] = FALSE; 
  }

  /* Any other initializations will go here */
}
/*===========================================================================

FUNCTION TC_TASK

DESCRIPTION
  This function is the entry point for TC Task. It initializes TC variables
  by calling tc_nas_init(). The ACK signal is sent back to TMC and a wait is 
  done on Task Start Signal from TMC. Once a Task Start Signal is received,
  it waits for events to arrive, which are processed accordingly.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tc_task (dword param)
{                             

  static tc_cmd_type   *tc_cmd_ptr;     /* To hold received TC command */
  rex_sigs_type wait_sigs, sigs; /* Signal mask for TC task to wait */

  (void)param;

  tc_tcb_ptr = rex_self();

#ifndef FEATURE_MODEM_RCINIT
  tc_init_before_task_start ();
#endif

  /* Initialize the TC task and RABM subtask */
  tc_nas_init();

#ifdef FEATURE_MODEM_RCINIT
  rcinit_handshake_startup();
#else
#ifndef FEATURE_MODEM_RCINIT_PHASE2
  tmc_task_start();
#endif
#endif

  /* Initially kick watchdog and set timer for watchdog report interval */
  #ifdef FEATURE_NAS_WDOG_DYNAMIC
  if( tc_dog_rpt_time_var > 0 )
  #endif
  {
    dog_report(DOG_TC_RPT);
    (void) rex_set_timer(&tc_rpt_timer, DOG_TC_RPT_TIME);
  }

  wait_sigs = TC_CMD_Q_SIG | TC_DOGRPT_TIMER_SIG;

  /* Start a Forever loop and process the commands that arrive */
  for (;;)
  {
    /* Wait on signal mask*/
    sigs = rex_wait (wait_sigs
#ifdef TEST_FRAMEWORK
                    #error code not present
#endif /*TEST_FRAMEWORK*/
                    );

    /* If this signal is for the TC command queue */
    if (sigs & TC_CMD_Q_SIG)
    {
      /* Received a Command on the TC command Queue, clear it and process */

      /* Clear signals */
      (void) rex_clr_sigs( tc_tcb_ptr, TC_CMD_Q_SIG);

      /* Get the command from the TC_CMD_Q and process it */
      while ((tc_cmd_ptr = (tc_cmd_type*) q_get(&tc_cmd_q)) != NULL)
      {
        /* Switch on the command id of the received command, and
        call the appropriate function to process each command. */
        switch (tc_cmd_ptr->header.message_set)
        {
          case MS_RABM_TC:
            tc_process_rabm_cmd( tc_cmd_ptr );
            break;

          case MS_GMM_TC:
            tc_process_gmm_cmd( tc_cmd_ptr );
            break;

          case MS_MM_TC:
            tc_process_mm_cmd( tc_cmd_ptr );
            break;

          case MS_CM_TC:
            tc_process_cm_cmd( tc_cmd_ptr );
            break;

          case MS_RRC_TC:
          case MS_TDSRRC_TC:
            tc_process_rrc_cmd( tc_cmd_ptr );
            break;

          case MS_TC_TC:
            /* Check whether this is a DL data notification from RLC */
            if (tc_cmd_ptr->header.cmd_id == LB_PROC_DL_RLC_DATA_CMD)
            {
               lb_rx_process_data(tc_cmd_ptr->cmd.lb_proc_dl_rlc_data_ind.wm_id);
            }
            else
            {
               ERR( "Unknown internal cmd received by TC %d", 
                  tc_cmd_ptr->header.cmd_id, 0, 0);
            }
            break;

          default:
            /* Wrong command received on the TC CMD Q */
            MSG_MED_DS_1(TC_SUB, "Unknown cmd received by TC %d", 
                  tc_cmd_ptr->header.cmd_id);

            break;
        }/* end switch */

        /* The command has been processed. Free the memory for this cmd */
#ifdef FEATURE_MODEM_HEAP       
        modem_mem_free(tc_cmd_ptr, MODEM_MEM_CLIENT_NAS);
#else
        mem_free( &(tmc_heap), tc_cmd_ptr );
#endif

      }/* end while */

    }/* end of TC_CMD_Q_SIG check */ 

    /* ------------------------------------------------------------------
    ** Check if report timer signal was set.  If set then clear signal, 
    ** report to DOG, set the timer again and proceed. 
    ** -----------------------------------------------------------------*/
    if (sigs & TC_DOGRPT_TIMER_SIG)
    {
      /* Clear the watchdog signal */
      (void) rex_clr_sigs( tc_tcb_ptr, TC_DOGRPT_TIMER_SIG);
      #ifdef FEATURE_NAS_WDOG_DYNAMIC
      if( tc_dog_rpt_time_var > 0 )
      #endif
      {
        /* Kick watchdog and reset timer */
        dog_report(DOG_TC_RPT);
        (void) rex_set_timer(&tc_rpt_timer, DOG_TC_RPT_TIME);
      }
    } 
#ifdef TEST_FRAMEWORK
    #error code not present
#endif
  } /* End of FOR loop */
  /* Add more signal checkin if TC task is waiting on more */
}


/*===========================================================================

FUNCTION TC_PUT_CMD

DESCRIPTION
  his function puts a command buffer into TC command queue.
  Note that the command buffer must have been requested through a 
  call to tc_get_cmd_buf(). Also, the calling function must have 
  filled all the necessary data including the command id before 
  passing the buffer to this function.

  This function also sets the appropriate signal to the TC
  task to indicate that a command has been placed in its queue. 
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A signal is set for the TC task and this causes a context switch.


===========================================================================*/
void tc_put_cmd (tc_cmd_type * cmd_buf)
{
  MSG_LOW_DS_0(TC_SUB," Putting CMD in tc_cmd_q ");

  /* Initialize the queue link before putting an item in a queue. */
  (void) q_link(cmd_buf, &(cmd_buf->header.link));

  /* Then put the command buffer in the queue */
  q_put(&tc_cmd_q, &(cmd_buf->header.link));

  /* Then set the signal for this queue */
  (void) rex_set_sigs(tc_tcb_ptr, TC_CMD_Q_SIG);
}

/*===========================================================================

FUNCTION TC_GET_CMD_BUF

DESCRIPTION

  This function returns a TC command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer is freed by calling tc_free_cmd_buf(). 

  The freeing is done by the function that dequeues and processes this
  command buffer. That means this is done by TC task.
  
  Calling function MUST check for a NULL return value and take necessary
  action.
 
DEPENDENCIES

  None.
  
RETURN VALUE

  A buffer of type tc_cmd_type. NULL is returned if memory is unavailable.

SIDE EFFECTS

  None.
  
===========================================================================*/
tc_cmd_type *tc_get_cmd_buf( void )
{
  tc_cmd_type *ptr;

  /* Allocating memory for TC Command buffer from the TMC heap */
#ifdef FEATURE_MODEM_HEAP
  if ((ptr = (tc_cmd_type *)modem_mem_calloc(1, sizeof(tc_cmd_type), MODEM_MEM_CLIENT_NAS)) == NULL)
#else
  if ((ptr = (tc_cmd_type *)mem_malloc(&(tmc_heap), 
                                       sizeof(tc_cmd_type))) == NULL)
#endif
  {
    ERR_FATAL("Could not allocate memory for TC cmd q!", 0, 0, 0);
  }
  memset(ptr,0x00,sizeof(tc_cmd_type));
  return(ptr);
}

#endif /* FEATURE_TC */

