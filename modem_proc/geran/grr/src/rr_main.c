/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Main Module

GENERAL DESCRIPTION
   This module contains the task loop for the RR task, auxiliary functions
   and the top-level message handler for the RR

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 by QUALCOMM Technologies, Incorporated. All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_main.c_v   1.18   15 Jul 2002 15:28:28   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_main.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
15/07/14    pjr     Changed priority to l1_priority in function rr_main_lte_msgr_init
11/04/13    sjw     Do not use the pending message queue for ignored messages
14/02/13    sjw     Defer RR_PS_ABORT_REQ if first RACH response is still pending
20/03/12    pg      Add missing FEATURE_Q_NO_SELF_QPTR for q_delete and q_insert
02/10/12    sjw     Integrate DSDS controller with new message processing control
01/26/12    sjw     Add Demand Paging exclusion for rr_send_routine()
15/11/10    sjw     Added support for FEATURE_DUAL_SIM
04/07/09    sjw     Rework resource locking to use mutex/critical sections as appropriate
04/10/06    hs      Register rr_send_routine() to gsmsg to fix alignment mismatch on RVCT
06/30/03    sbs     Added support for Location Services related APDU handling
05/15/03    He      Ignore L2 fill octets
02/19/03    Rao     Fix for Releasing SAPIO link when SAPI3 Establishment fails.
10/21/02    RG      Added GSM to WCDMA Reselection support (SI2 Qtr decode ).
08/26/02    rmc     Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND
07/30/02    Rao     Added InterRAT ( WCDMA To GSM )  Handover support
07/29/02    rmc     Store reselection trigger when serving cell becomes barred
07/11/02    JAC     Added early camp support when collecting full SI for reselection
06/20/02    JAC     Added debug messages for bad protocol discriminator error
06/14/02    rmc     Backing out changes for l3_message_length field in RR_DATA_REQ and
                    RR_EST_REQ, fixing lint warnings
05/30/02    rmc     Changes to support MULTIBAND_SUPPORT
05/22/02    rmc     Changes to MM-RR icd
05/01/02    rmc     Register RR task for NV
04/16/02    AT      Changed RR Cmd queue length to 20.
04/15/02    rmc     Fix for CR21733
04/11/02    JAC     added uplink/downlink bit for logging signalling msgs
04/10/02    rmc     Watchdog support for RR task
03/18/02    Rao     Added Dual Mode (Mode control) changes.
03/13/02    ATM     Added support for new GS_SEND_MESSAGE
02/20/02    ATM     Merge GS_GSM and GS functions together
02/14/02    rmc     QXDM Cleanup + Input message xdm logging
11/21/01    ATM     Replaced call to CFA with call to Generic Services (GS) --
                    which will indirectly call CFA.
11/09/01    RMC     Cleanup to match MM version and remove unit test specific code
                    Fixed problem with cfa_log_packet and removed duplicates
12/11/00    TS      Initial Revision.

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"

#ifdef FEATURE_GSM_TDS
#include "tdsrrc_rr_if.h"
#include "rr_tdsrrc_if.h"
#endif /*FEATURE_GSM_TDS*/

#include "geran_tasks.h"
#include "amssassert.h"
#include "rr_main.h"
#include "rr_control.h"
#include "rr_l2.h"
#include "ms.h"
#include "rr_l2_send.h"
#include "rr_mm_send.h"
#include "rr_test.h"
#include "rr_sys_info.h"
#include "gs.h"
#include "rr_task.h"
#include "rr_init.h"
#include "geran_msgs.h"
#include "rr_log.h"
#include "gs.h"           /* for gs_read_messages */
#include <stddef.h>
#ifndef PERLUTF
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "ghdi_nvmem.h"
#endif /* PERLUTF */
#include "rr_general.h"
#include "rr_mode.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_cell_acq_if.h"
#include "rr_cell_acq.h"
#include "rr_resel.h"
#ifdef FEATURE_WCDMA
#include "rr_resel_g2w.h"
#endif /* FEATURE_WCDMA */

#include "ghdi_exp.h"
#include "ghdi_exp_v.h"

#include "rr_procedure.h"
#include "rr_sapi3.h"
#include "rr_init.h"

#include "aconfig.h"
#include "task.h"
#include "rcinit.h"
#include "dog_hb_rex.h"

#include "rr_gprs_defs.h"
#include "rr_gprs_debug.h"
#include "mac_grr.h"

#ifdef FEATURE_CGPS_UMTS_CP_GSM
#include "rr_lsm_if.h"
#endif /* FEATURE_CGPS_UMTS_CP_GSM */
#include "grr_mutex.h"

#ifdef FEATURE_LTE
#include "msgr.h"
#include "msgr_rex.h"
#include "geran_grr_msg.h"
#endif /* FEATURE_LTE */

#ifdef FEATURE_FEMTO_GSM_NL
#include "rr_femto.h"
#include "rr_femto_if.h"
#endif /* FEATURE_FEMTO_GSM_NL */

#include "geran_multi_sim.h"
#ifdef FEATURE_DUAL_SIM
#include "rr_multi_sim.h"
#include "rr_conn_establish.h"
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#include "gprs_mem.h"
#include "rr_ded_config.h"
#include "rr_sim.h"
#include "rr_l1_idle_mode.h"
#include "geran_nv.h"
#include "rr_ps_domain.h"
#include "rr_ps_access.h"
#include "rr_nv.h"
#include "rr_decode_ie.h"
#include "rr_shared_cell_selection.h"
#include "rr_gprs_defs.h"
#include "rr_sys_info_cache.h"
#include "stringl/stringl.h"
#ifdef FEATURE_SGLTE
#include "rr_sglte.h"
#endif /*FEATURE_SGLTE*/
#include "rr_x2g.h"
#include "rr_msc.h"
#include "rr_l1_send.h"
#ifdef FEATURE_CGPS_UMTS_CP_GSM
#include "rr_cgps.h"
#endif /*FEATURE_SGLTE*/
#include "rr_select_bcch.h"
#include "rr_dtm.h"
#include "rr_acq_db.h"
#include "rr_pscan.h"

#if defined (FEATURE_QSH_DUMP) || defined (FEATURE_QSH_EVENT_METRIC)
#include "rr_qsh.h"
#endif // FEATURE_QSH_DUMP || FEATURE_QSH_EVENT_METRIC

/* mmcp_sd_task_running **************************************************/
/* RCInit CM start up synchronisation - see mmcp\mmode\sd\src\sdtask.h  */
#include "rcevt_rex.h"

/* Event to indicate that SD task is ready to process commands */
#define SD_TASK_RCEVT_READY "sdtask:ready"

/* SD task signals that it's running (so sd_misc_get_supp_band_pref can safely be called) */
#define SD_TASK_STARTED_SIG          0x20000
/* mmcp_sd_task_running **************************************************/

#define RR_MSG_QUEUE_SIZE (10 * NUM_GERAN_DATA_SPACES) /* cmd queue size */

/* Define a type for RR task pending queue + control flag */
typedef struct
{
  q_type  rr_cmd_pend_q;
  boolean nas_message_processing_suspended;
} rr_pending_queue_T;

/* RR task message queue */
q_type  rr_cmd_q;

/* RR task pending queue + control flag */
rr_pending_queue_T  rr_pending_queue[NUM_GERAN_DATA_SPACES];

#define RR_MSGR_QUEUE_SIZE 5

q_type rr_msgr_cmd_q;
q_type rr_msgr_cmd_free_q;
static msgr_client_t   msgr_client;

static dog_report_type rr_dog_rpt_var = 0;      /* RR Watchdog ID */

static rex_timer_type  rr_sleep_timer;

rex_sigs_type sigs;        /* RR task REX signals */

static geran_grr_cmd_t rr_msgr_message_pool[RR_MSGR_QUEUE_SIZE];


/* Critical section to protect shared variables in GRR */

rex_crit_sect_type grr_general_crit_sec;
rex_crit_sect_type grr_sysinfo_crit_sec;


#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

static rex_crit_sect_type rr_q_lock_crit_sect;

#define RR_Q_LOCK_INIT()  rex_init_crit_sect(&rr_q_lock_crit_sect)
#define RR_Q_LOCK()       REX_ISR_LOCK(&rr_q_lock_crit_sect)
#define RR_Q_UNLOCK()     REX_ISR_UNLOCK(&rr_q_lock_crit_sect)

#ifdef FEATURE_DUAL_SIM
static gas_id_t rr_main_reset_gas_id;
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

/*===========================================================================
                         FUNCTION DECLARATIONS
===========================================================================*/
static void rr_process_message(
  rr_cmd_bdy_type   * new_message,
  rr_message_type_e   message_type,
  const gas_id_t      gas_id
);
gs_status_T rr_send_message(void *msg_p);
void gsm_rr_fn_init(void);

static boolean initialised = FALSE;

/*===========================================================================
                         FUNCTION DEFINITIONS
===========================================================================*/

/**
  @brief Can be called by any module, and will cause RR to restart RR, L1, MAC and RLC
*/
void geran_initiate_recovery_restart(
  const geran_client_enum_t client_id,
  const gas_id_t gas_id
)
{
#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
  /* Verify that the FTM copy of rr_l1_messages_u is at least as big as expected
     (will generate a compiler error if it isn't) */
  uint8 dummy [sizeof(mph_geran_to_ftm_padding_T) >= sizeof(rr_l1_messages_u) ? 1 : -1];
  dummy[0] = 0; /* Fix for Klocwork  warning -- 'dummy' array elements are used uninitialized in this function.*/
  dummy[0] = dummy[0]; /* sidestep compiler warning about unused variable */
#endif /* PERLUTF */

  /* output some debug to indicate the client triggering the recovery */
  switch(client_id)
  {
  case GERAN_CLIENT_NULL:
    MSG_GERAN_HIGH_0_G("Legacy/null client triggered recovery restart");
    break;

  case GERAN_CLIENT_GL1:
    MSG_GERAN_HIGH_0_G("GSM L1 triggered recovery restart");
    break;

  case GERAN_CLIENT_GL2:
    MSG_GERAN_HIGH_0_G("GSM L2 triggered recovery restart");
    break;

  case GERAN_CLIENT_GRR:
    MSG_GERAN_HIGH_0_G("GSM RR triggered recovery restart");
    break;

  case GERAN_CLIENT_GMAC:
    MSG_GERAN_HIGH_0_G("GPRS MAC triggered recovery restart");
    break;

  case GERAN_CLIENT_GRLC:
    MSG_GERAN_HIGH_0_G("GPRS RLC triggered recovery restart");
    break;

  case GERAN_CLIENT_GLLC:
    MSG_GERAN_HIGH_0_G("GPRS LLC triggered recovery restart");
    break;

  case GERAN_CLIENT_GDRV:
    MSG_GERAN_HIGH_0_G("GSM Drivers triggered recovery restart");
    break;

  case GERAN_CLIENT_MDSP:
    MSG_GERAN_HIGH_0_G("GSM MDSP interface triggered recovery restart");
    break;

  default:
    MSG_GERAN_ERROR_1_G("Unknown client ID %d initiated recovery restart", (int)client_id);
    break;
  }

#ifndef PERLUTF
#ifdef FEATURE_DUAL_SIM
  // Record the GAS-ID which triggered the restart - needed for when the RR task runs
  rr_main_reset_gas_id = check_gas_id(gas_id);
#endif

  /* Set RR_RESET_SIG to be picked-up next time RR task runs */
  (void) rex_set_sigs(geran_tcb_read(GERAN_TASK_ID_GRR), RR_RESET_SIG);
#else
  #error code not present
#endif /* !PERLUTF */

  return;
} /* geran_initiate_recovery_restart() */


/**
  @brief Initialise RR task queue handling; Register with General Services; Create task timers
*/
static void rr_main_task_init(void)
{
  byte i = 0;

  // Clear task REX signals
  (void) rex_clr_sigs(rex_self(), (rex_sigs_type)~0);

  // Initialise task queue & register

  (void) q_init(&rr_cmd_q);

  /* initialise the pending queue for each data space */
  for (i = 0; i < NUM_GERAN_DATA_SPACES; i++)
  {
    (void)q_init(&(rr_pending_queue[i].rr_cmd_pend_q));
  }

  (void) gs_init(
           GS_QUEUE_RR,
           rex_self(),
           RR_CMD_Q_SIG,
           &rr_cmd_q,
           NULL,
           sizeof(rr_cmd_type),
           RR_MSG_QUEUE_SIZE,
           /* Use the default message handling routine. */
           (gs_get_handler_ptr) NULL);

  (void) gs_init_for_send_only (GS_QUEUE_RR, rr_send_message);

  // Initialise task timers & watchdog

  /* create the task sleep timer */
  rex_def_timer(&rr_sleep_timer, rex_self(), RR_SLEEP_SIG);

  /* Register with Dog HB */
  rr_dog_rpt_var = dog_hb_register_rex((rex_sigs_type)RR_RPT_TIMER_SIG);

#ifdef FEATURE_DUAL_SIM
  rr_main_reset_gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
#endif

} /* rr_main_task_init */


/**
  @brief Initialize RR mutexes and critical sections
*/
static void rr_main_init_resource_locks(void)
{

  /* Initialise general GRR critical section */
  rex_init_crit_sect(&grr_general_crit_sec);

  /* Initialise GRR sysinfo critical section */
  rex_init_crit_sect(&grr_sysinfo_crit_sec);

  /* initialise the critical section used for task queue handling */
  RR_Q_LOCK_INIT();

  /* initialise the critical section used for the public store */
  rr_public_store_lock_init();

  return;
} /* rr_main_init_resource_locks */


/**
  @brief Function called by General Services when gs_send_message() called to send a message to RR task
*/
gs_status_T rr_send_message(void *msg_p)
{
  gs_status_T   gs_status = GS_SUCCESS;
  rr_cmd_type  *cmd_ptr;

  // Allocate a command buffer from heap
  cmd_ptr = (rr_cmd_type *)GPRS_MEM_CALLOC(sizeof(rr_cmd_type));

  if (cmd_ptr)
  {
    word  message_length;
    rr_cmd_type  *head_cmd_ptr = NULL;

    // Get the message length
    GET_IMH_LEN(message_length, msg_p);
    message_length += sizeof(IMH_T);

    // Copy the new message to the allocated command buffer
    //replace memcpy with memscpy
  memscpy((void*)&cmd_ptr->msg,sizeof(cmd_ptr->msg), msg_p, message_length);


    // Ensure the link pointer is initialised
    (void) q_link( (void*)cmd_ptr, &cmd_ptr->link );

    RR_Q_LOCK();

    // If the new message MS_RR_RR, then scan from the head of the queue, and skip any which
    // are also MS_RR_RR. At the first non-MS_RR_RR message, insert the new message
    if (cmd_ptr->msg.message_header.message_set == MS_RR_RR)
    {
      // Get the item at the head of the queue
      head_cmd_ptr = q_check(&rr_cmd_q);

      // If the new message is a RR_IMSG_RR_EVENT_IND, then this is placed ahead of other RR-RR messages
      // (this is to maintain legacy RR-EVENT behaviour)
      if ((cmd_ptr->msg.message_header.message_set == MS_RR_RR) &&
          (cmd_ptr->msg.rr.header.rr_message_set == RR_IMSG) &&
          (cmd_ptr->msg.rr.rr.imsg.header.imh.message_id == RR_IMSG_RR_EVENT_IND) &&
          (cmd_ptr->msg.rr.rr.imsg.rr_event_ind.priority_over_other_imsg))
      {
        // If the current item is RR_IMSG_RR_EVENT_IND, move onto the next one
        while (head_cmd_ptr &&
               ((head_cmd_ptr->msg.message_header.message_set == MS_RR_RR) &&
                (head_cmd_ptr->msg.rr.header.rr_message_set == RR_IMSG) &&
                (head_cmd_ptr->msg.rr.rr.imsg.header.imh.message_id == RR_IMSG_RR_EVENT_IND)))
        {
          head_cmd_ptr = (rr_cmd_type *) q_next(&rr_cmd_q, &head_cmd_ptr->link);
        }
      }
      else
      {
        // If the current item is MS_RR_RR, move onto the next one
        while (head_cmd_ptr && (head_cmd_ptr->msg.message_header.message_set == MS_RR_RR))
        {
          head_cmd_ptr = (rr_cmd_type *) q_next(&rr_cmd_q, &head_cmd_ptr->link);
        }
      }
    }

    // This is the first queue item which is not MS_RR_RR
    if (head_cmd_ptr == NULL)
    {
      // Place the new message on the queue
      q_put(&rr_cmd_q, &cmd_ptr->link);
    }
    else
    {
      // Insert the new message into the queue
      q_insert(
               &rr_cmd_q,
               &cmd_ptr->link,
               &head_cmd_ptr->link);
    }

    RR_Q_UNLOCK();

    /* Set the command queue signal for receive task */
    (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GRR), RR_CMD_Q_SIG );
  }
  else
  {
    MSG_GERAN_ERROR_0("No available heap memory");
    gs_status = GS_FAILURE;
  }

  return gs_status;
}


/**
  @brief Returns a pointer to the pending queue
*/
static rr_pending_queue_T *rr_get_pending_queue_ptr(const gas_id_t gas_id)
{
  return (&(rr_pending_queue[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)]));
}

/**
  @brief Checks for RR_EST_REQ in Pending Command Queue and removes it if found. 
  Returns TRUE if found else FALSE    
*/
static boolean rr_check_and_rem_pend_est_req(const gas_id_t gas_id)
{

  boolean est_req_found = FALSE;
  rr_pending_queue_T *pend_queue_ptr;
  rr_cmd_type        *cmd_ptr;
  
  pend_queue_ptr = rr_get_pending_queue_ptr(gas_id);

  if (pend_queue_ptr != NULL)
  {
     cmd_ptr = (rr_cmd_type *)q_check(&(pend_queue_ptr->rr_cmd_pend_q));
  }
  else
  {  
    cmd_ptr = NULL;    
  }

  while ( (cmd_ptr != NULL) && (est_req_found == FALSE) )
  {
    switch (cmd_ptr->msg.message_header.message_set)
    {
      case MS_MM_RR:
      {
        if ( cmd_ptr->msg.message_header.message_id == RR_EST_REQ )
        {
          est_req_found = TRUE;
        }
        break;
      }
      default:
      break;
    }

    if (est_req_found)
    {
    
      MSG_GERAN_HIGH_1_G("RR Removing Pending msg_id %d", cmd_ptr->msg.message_header.message_id);
      q_delete(
               &(pend_queue_ptr->rr_cmd_pend_q),
               &cmd_ptr->link
              );
      /* Free memory allocated for this message */
      GPRS_MEM_FREE(cmd_ptr);
    }
    else
    {
      cmd_ptr = (rr_cmd_type *) q_next(&(pend_queue_ptr->rr_cmd_pend_q), &cmd_ptr->link);
    }
  }

  return(est_req_found);
}

/**
  @brief Determines if a message received should be processed normally, or held in the queue
*/
static rr_process_msg_action_e should_message_be_processed(
  rr_cmd_bdy_type *msg_ptr,
  const gas_id_t gas_id
)
{
  // Assume the message is valid and can be processed
  boolean valid_message = TRUE;
  boolean process_message = TRUE;

  switch (msg_ptr->message_header.message_set)
  {
    case MS_MM_RR:
    {
      process_message = rr_can_process_mm_message(gas_id);

      if (process_message)
      {
        switch (msg_ptr->message_header.message_id)
        {
#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM)
          case RR_PS_ABORT_REQ:
          {
            process_message = rr_ce_can_process_ps_abort(gas_id);
            break;
          }

          case RR_GMM_GPRS_SERVICE_REQ:
          {
            process_message = rr_ps_can_process_gprs_service_req(gas_id);
            break;
          }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#ifdef FEATURE_LTE
          case RR_CSFB_BLIND_REDIR_TO_LTE_REQ:
          {
            process_message = rr_can_process_csfb_blind_redir_to_lte_req(gas_id);
            break;
          } /* RR_CSFB_BLIND_REDIR_TO_LTE_REQ */
#endif /* FEATURE_LTE */

          case RR_CHANGE_MODE_IND:
          {
            process_message = rr_can_process_change_mode_ind(gas_id);
            break;
          } /* RR_CHANGE_MODE_IND */


#ifdef FEATURE_SGLTE
          case RR_STOP_GSM_MODE_REQ:
          {
            switch (msg_ptr->rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason)
            {
              case RR_SGLTE_RAT_CHANGE:
              case RR_SGLTE_USER_RAT_CHANGE:
              {
                process_message = rr_can_process_stop_gsm_mode_req_sglte_rat_change(gas_id);
                break;
              } /* RR_SGLTE_RAT_CHANGE */

              default:
              {
                process_message = rr_can_process_mm_message(gas_id);
              }
            }
            break;
          } /* RR_STOP_GSM_MODE_REQ */
#endif /* FEATURE_SGLTE */
          default:
          {
            process_message = rr_can_process_mm_message(gas_id);
          }
        }
      }
      else
      {
        /* apply any per-message exclusions for NAS primitives here */
        if (RR_GMM_GPRS_MS_RA_CAP_IE_IND == msg_ptr->message_header.message_id)
        {
          /* always process this NAS message as soon as it arrives */
          /* because it is part of a capability handshake with LTE */
          process_message = TRUE;
        }
        else if ( RR_ABORT_REQ == msg_ptr->message_header.message_id)
        {
          if ( rr_check_and_rem_pend_est_req(gas_id) == FALSE )
          {
            process_message = rr_can_process_rr_abort_req(gas_id);
          }
          else
          {
            rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);
            valid_message = FALSE;
          }
        }
      }
      break;
    }

    case MS_RR_RR:
    case MS_RR_RR_EXT:
    {
      switch (msg_ptr->rr.header.rr_message_set)
      {
        case RR_IMSG:
        {
          switch (msg_ptr->rr.header.imh.message_id)
          {
            case RR_IMSG_SAVED_MSG_IND:
            {
              process_message = rr_can_process_saved_msg_ind(gas_id);
              break;
            }

            case RR_IMSG_RR_EVENT_IND:
            {
              process_message = rr_can_process_rr_event_ind(gas_id);
              break;
            }

            case RR_IMSG_MSC_CHANGE_IND:
            {
              process_message = rr_can_process_msc_change_ind(gas_id);
              break;
            }

            case RR_IMSG_NO_PS_ACCESS_REQ:
            {
              process_message = rr_can_process_no_ps_access_req(gas_id);
              break;
            }

            case RR_IMSG_PS_ACCESS_REQ:
            {
              process_message = rr_can_process_ps_access_req(gas_id);
              break;
            }

            default: ;
          }
          break;
        }

        case RR_GRR_IMSG:
        {
          switch (msg_ptr->rr.header.imh.message_id)
          {
            case RR_GRR_IMSG_GPRS_IA_IND:
            {
              process_message = grr_can_process_rr_grr_gprs_ia_ind(gas_id);
              break;
            }

            case RR_GRR_IMSG_RR_EVENT_IND:
            {
              process_message = grr_can_process_rr_event_ind(gas_id);
              break;
            }

            default: ;
          }
          break;
        }

        case RR_RESEL_IMSG:
        {
          switch (msg_ptr->rr.resel.imsg.header.imh.message_id)
          {
            case RR_RESEL_IMSG_ABORT_REQ:
            {
              process_message = rr_can_process_resel_abort_req(gas_id);
              break;
            } /* RR_RESEL_IMSG_ABORT_REQ */

            default: ;
          }
          break;
        } /* RR_RESEL_IMSG */

        case RR_L1_IDLE_IMSG:
        {
          switch (msg_ptr->rr.resel.imsg.header.imh.message_id)
          {
            case RR_L1_IDLE_IMSG_START_IDLE_REQ:
            {
              process_message = rr_l1_idle_can_process_start_idle_req(gas_id);
              break;
            } /* RR_L1_IDLE_IMSG_START_IDLE_REQ */

            default: ;
          }
          break;
        } /* RR_L1_IDLE_IMSG */

#ifdef FEATURE_GSM_DTM
        case RR_DTM_IMSG:
        {
          switch (msg_ptr->rr.resel.imsg.header.imh.message_id)
          {
            case RR_DTM_IMSG_CS_RELEASE_REQ:
            {
              process_message = rr_dtm_can_process_cs_release_req(gas_id);
              break;
            }

            default: ;
          }
          break;
        } // RR_DTM_IMSG
#endif // FEATURE_GSM_DTM

        default: ;
      }
      break;
    }

    case MS_RR_L1:
    {
      switch (msg_ptr->message_header.message_id)
      {
        case MPH_SERVING_MEAS_IND:
        case MPH_SERVING_IDLE_MEAS_IND:
        case MPH_SERVING_AUX_MEAS_IND:
        case MPH_NC_MEASUREMENT_IND:
        case MPH_BLOCK_QUALITY_IND:
        {
          process_message = rr_can_process_serving_meas_ind(gas_id);
          break;
        }

        case MPH_SURROUND_MEAS_IND:
        case MPH_SURROUND_UPDATE_FAILURE_IND:
#ifdef FEATURE_WCDMA
        case MPH_SURROUND_WCDMA_IDLE_MEAS_IND:
        case MPH_SURROUND_WCDMA_XFER_MEAS_IND:
#endif /* FEATURE_WCDMA */
#if defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
        case MPH_SURROUND_IRAT_MEAS_IND:
#endif /* FEATURE_LTE || FEATURE_GSM_TDS */
        {
          process_message = rr_can_process_surround_meas_ind(gas_id);
          break;
        }
#ifdef FEATURE_SGLTE
        case MPH_UE_MODE_CHANGE_CNF:
         if (rr_is_pending_mode_change(gas_id) )
         {
             rr_set_pending_mode_change(FALSE, gas_id);
         }

        break;
#endif /* FEATURE_SGLTE */
        default: ;
      }
      break;
    }

    case MS_TIMER:
    {
      if (msg_ptr->message_header.message_id == MID_TIMER_EXPIRY)
      {
        /* process the message expiration to mark the state of the timer as expired */
        timer_expiry_T *timer_expiry_ptr = (timer_expiry_T *) msg_ptr;

        if (rr_timer_validate_expiry_message((rr_timer_id_T)(timer_expiry_ptr->timer_id), gas_id))
        {
          // Timer is valid. Now check whether RR is ready to process its expiry
          process_message = rr_can_process_timer_expiry((rr_timer_id_T)(timer_expiry_ptr->timer_id), gas_id);
        }
        else
        {
          MSG_GERAN_HIGH_0_G("Timer invalid - expiry message discarded");

          // Flag this message as invalid
          valid_message = FALSE;
        }
      }
      break;
    }
    
#ifdef FEATURE_FAKE_BTS_CELL_BARRING
    case MS_MSGR_RR:
    {
      switch (msg_ptr->rr_msgr_msg.message.hdr.id)
      {

        case GERAN_GRR_FAKE_GCELL_ACTION_REQ:
        {
          process_message = FALSE;

          switch (get_rr_state(gas_id))
          {
            case RR_INACTIVE:
            case RR_GOING_INACTIVE:
            case RR_DATA_TRANSFER:
            { 
              process_message = TRUE;
              break;
            }
            case RR_GRR:
            {
              if ( get_grr_state(gas_id) == GRR_CAMPED)
              {
                process_message = TRUE;
              }
              break;
            }
                   
            default:;
            break;        
          }
        }
        break;
      }
    }
#endif /* FEATURE_FAKE_BS_DETECTION */

#ifdef FEATURE_SGLTE
    case MS_MSGR_RR:
    {
      switch (msg_ptr->rr_msgr_msg.message.hdr.id)
      {
#ifdef FEATURE_LTE
        case GERAN_GRR_LTE_RESEL_REQ:
        case GERAN_GRR_LTE_REDIR_REQ:
        case GERAN_GRR_LTE_CCO_REQ:
        {
          if (rr_is_sglte_mode_active(gas_id))
          {
            process_message = rr_can_process_sglte_x2g_req(gas_id);
          }
          else
          {
            process_message = TRUE;
          }
          break;
        } /* GERAN_GRR_LTE_RESEL_REQ ... */

        case GERAN_GRR_LTE_ABORT_RESEL_REQ:
        case GERAN_GRR_LTE_ABORT_REDIR_REQ:
        case GERAN_GRR_LTE_ABORT_CCO_REQ:
        {
          if (rr_is_sglte_mode_active(gas_id))
          {
             MSG_GERAN_HIGH_0_G("Abort rcvd from LTE,check  pending X2G message");
             if (rr_sglte_reject_pending_x2g_req(TRUE, gas_id))
             {
               MSG_GERAN_HIGH_0_G("Abort rcvd, pending X2G message removed");   
               valid_message = FALSE;
             }
          }
          break;
        }

        case GERAN_GRR_LTE_HO_REQ:
        {
          if (rr_is_sglte_mode_active(gas_id))
          {
            process_message = rr_can_process_sglte_l2g_ho_req(gas_id);

            if(!process_message)
            {
              MSG_GERAN_HIGH_0_G("GERAN is GOING INACTIVE hold of any GERAN_GRR_LTE_HO_REQ HO rqst in pending queu ");
            }
          }
          else
          {
            process_message = TRUE;
            MSG_GERAN_HIGH_0_G("SGLTE mode not active process HO as legacy ");
          }
          break;
        } /* GERAN_GRR_LTE_HO_REQ */
        case GERAN_GRR_LTE_ABORT_HO_REQ:
        {
          if (rr_sglte_internal_stop_gsm_being_processed(gas_id))
          {
            process_message = FALSE;
            MSG_GERAN_HIGH_0_G("SGLTE SRVCC mode intrnl shutdown being processed queue the L2G Abort");
          }
          else if (rr_is_sglte_mode_active(gas_id))
          {
            process_message = rr_can_process_sglte_l2g_ho_req(gas_id);

            if(!process_message)
            {
              MSG_GERAN_HIGH_0_G("GERAN is GOING INACTIVE hold of any GERAN_GRR_LTE_ABORT_HO_REQ HO rqst in pending queue ");
            }
          }
          else
          {
            process_message = TRUE;
          }
          break;
        }
#endif /* FEATURE_LTE */

        default:
        {
          process_message = TRUE;
        }
      }
      break;
    } /* MS_MSGR_RR */


#ifdef FEATURE_GSM_TDS
    case MS_TDSRRC_RR:
    {
      switch (msg_ptr->message_header.message_id)
      {
        case TDSRR_INTERRAT_RESELECTION_REQ:
        case TDSRR_INTERRAT_REDIRECT_REQ:
        case TDSRR_INTERRAT_CC_ORDER_REQ:
        {
          if (rr_is_sglte_mode_active(gas_id))
          {
            process_message = rr_can_process_sglte_x2g_req(gas_id);
          }
          else
          {
            process_message = TRUE;
          }
          break;
        } /* TDSRR_INTERRAT_RESELECTION_REQ ... */

        case TDSRR_INTERRAT_RESELECTION_ABORT_REQ:
        case TDSRR_INTERRAT_REDIRECT_ABORT_REQ:
        case TDSRR_INTERRAT_CC_ORDER_ABORT_REQ:
        {
          if (rr_is_sglte_mode_active(gas_id))
          {
             MSG_GERAN_HIGH_0_G("Abort rcvd from TD,check  pending X2G message");          
             if (rr_sglte_reject_pending_x2g_req(TRUE, gas_id))
             {
                MSG_GERAN_HIGH_0_G("pending X2G message now removed");   
                valid_message = FALSE;
             }
          }
          break;
        }

        default:
        {
          process_message = TRUE;
        }
      }
      break;
    } /*MS_TDSRRC_RR*/

#endif /*FEATURE_GSM_TDS*/
#endif /* FEATURE_SGLTE */

    /* All other messages should be processed immediately, regardless */
    default: ;
  }

  if (!process_message)
  {
    MSG_SPRINTF_3(
      MSG_SSID_DFLT,
      MSG_LEGACY_HIGH,
      "gs%d:Processing of %s blocked in %s",
      (gas_id+1),
      rr_gprs_message_name(
        msg_ptr,
        msg_ptr->message_header.message_set,
        msg_ptr->message_header.message_id,
        NULL
      ),
      rr_main_current_state_name(gas_id)
    );
  }

  if (valid_message)
  {
    if (process_message)
    {
      return RR_PROCESS_MSG_NOW;
    }
    else
    {
      return RR_PROCESS_MSG_LATER;
    }
  }

  return RR_PROCESS_MSG_NEVER;

} /* end should_message_be_processed() */

/**
  @brief Wrapper function for q_get() which adds mutex protection using RR_Q_LOCK.

*/
static rr_cmd_type *rr_q_get(void)
{
  rr_cmd_type *cmd_ptr;

  RR_Q_LOCK();
  cmd_ptr = (rr_cmd_type *)q_get(&rr_cmd_q);
  RR_Q_UNLOCK();

  return cmd_ptr;
}
/**
  @brief Called when there is a message to be processed in the RR task queue

  This is triggered by RR_CMD_Q_SIG being set, causing RR task to run
*/
#ifdef PERLUTF
#error code not present
#else
static void rr_read_messages(void)
#endif // PERLUTF
{
  rr_pending_queue_T      *pend_queue_ptr;
  rr_cmd_type             *cmd_ptr;
  rr_process_msg_action_e  process_message;
  gas_id_t                 gas_id = GERAN_ACCESS_STRATUM_ID_1;

  while ((cmd_ptr = rr_q_get()) != NULL)
  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
    /* Dual SIM module intercepts every message on the queue and determines the GAS-ID */
    process_message = rr_ms_process_task_message(&(cmd_ptr->msg), &gas_id);

    if (process_message != RR_PROCESS_MSG_NEVER)
    {
      if (gas_id == GERAN_ACCESS_STRATUM_ID_UNDEFINED)
      {
        MSG_GERAN_ERROR_0_G("GAS-ID undefined, message discarded");
        process_message = RR_PROCESS_MSG_NEVER;
      }
    }
#else
    /* single standby builds always use a default of "process now" */
    process_message = RR_PROCESS_MSG_NOW;
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

    if (process_message == RR_PROCESS_MSG_NOW)
    {
      process_message = should_message_be_processed(&cmd_ptr->msg, gas_id);
    }

    switch (process_message)
    {
      case RR_PROCESS_MSG_NOW:
      {
#ifdef PERLUTF
        #error code not present
#endif // PERLUTF

        /* The message is being processed.  It may have previously been put
        on the pending queue.  This is indicated by the cmd_ptr->pended_count
        variable. */
        switch (cmd_ptr->msg.message_header.message_set)
        {
#ifdef FEATURE_LTE
          case MS_MSGR_RR:
          {
            rr_debug_log_imsg(
              cmd_ptr->msg.message_header.message_set,
              cmd_ptr->msg.rr_msgr_msg.message.hdr.id,
              cmd_ptr->pended_count,
              gas_id
            );
            break;
          } /* MS_MSGR_RR */
#endif /* FEATURE_LTE */

          default:
          {
            rr_debug_log_imsg(
              cmd_ptr->msg.message_header.message_set,
              cmd_ptr->msg.message_header.message_id,
              cmd_ptr->pended_count,
              gas_id
            );
          }
        }

        /* Process the message */
        if (cmd_ptr->pended_count > 0)
        {
          rr_process_message(
            &cmd_ptr->msg,
            RR_MESSAGE_TYPE_PENDED,
            gas_id
          );
        }
        else
        {
          rr_process_message(
            &cmd_ptr->msg,
            RR_MESSAGE_TYPE_NEW,
            gas_id
          );
        }

        /* Free memory allocated for this message */
        GPRS_MEM_FREE(cmd_ptr);
        /* Now check whether any messages from the pending queue can be transferred to the task queue */
        rr_check_for_pending_messages(gas_id);
        break;
      }

      case RR_PROCESS_MSG_LATER:
      {
        /* determine the correct pending queue using the GAS ID */
        pend_queue_ptr = rr_get_pending_queue_ptr(gas_id);
        RR_NULL_CHECK_FATAL(pend_queue_ptr);

        /* Move the message to the pending queue */
        if (cmd_ptr->pended_count == 0)
        {
          /* This is the first time the message has been put on the pending
          queue, so add it to the trace buffer as it has jsut been received,
          even if RR is not able to process it yet. */
          switch (cmd_ptr->msg.message_header.message_set)
          {
#ifdef FEATURE_LTE
            case MS_MSGR_RR:
            {
              rr_debug_log_imsg(
                cmd_ptr->msg.message_header.message_set,
                cmd_ptr->msg.rr_msgr_msg.message.hdr.id,
                0,
                gas_id
              );
              break;
            } /* MS_MSGR_RR */
#endif /* FEATURE_LTE */

            default:
            {
              rr_debug_log_imsg(
                cmd_ptr->msg.message_header.message_set,
                cmd_ptr->msg.message_header.message_id,
                0,
                gas_id
              );
            }
          }
        }
        ++cmd_ptr->pended_count;
        q_put(&(pend_queue_ptr->rr_cmd_pend_q), &cmd_ptr->link);
        break;
      }

      case RR_PROCESS_MSG_NEVER:
      default:
      {
        /* Free memory allocated for this message */
        GPRS_MEM_FREE(cmd_ptr);
      }
    }
  }

  return;
} /* end rr_read_messages() */


/**
  @brief Reads a message from the Message Router queue and reformats it
         as an IMH_T compatible message.
*/
void rr_read_msgr_messages(void)
{
  geran_grr_cmd_t * cmd_ptr;

  while ((cmd_ptr = (geran_grr_cmd_t *) q_get(&rr_msgr_cmd_q)) != NULL)
  {
    rr_cmd_bdy_type *rr_cmd_ptr;

    MSG_GERAN_HIGH_0("Processing message received via MessageRouter...");

    rr_cmd_ptr = GPRS_MEM_MALLOC(sizeof(rr_cmd_bdy_type));

    if (rr_cmd_ptr != NULL)
    {
      rr_cmd_ptr->message_header.message_set = MS_MSGR_RR;
      rr_cmd_ptr->message_header.message_id = 0;
      rr_cmd_ptr->rr_msgr_msg.message = cmd_ptr->message;

      PUT_IMH_LEN(sizeof(rr_msgr_msg_t) - sizeof(IMH_T), &rr_cmd_ptr->message_header);

      (void) gs_send_message(GS_QUEUE_RR, rr_cmd_ptr, TRUE);

      GPRS_MEM_FREE(rr_cmd_ptr);
    }

    /* Return message to the free queue */
    q_put(&rr_msgr_cmd_free_q, &cmd_ptr->link);
  }

} /* end rr_read_messages() */


#ifdef FEATURE_SGLTE
/*===========================================================================

FUNCTION     rr_sglte_reject_pending_x2g_req

DESCRIPTION  Brief Check for L2G or T2G resel, CCO or redirection in pending queue, if abort rcvd is set to TRUE,
                       it means that some X2G procedure got aborted but first check that the message is still in pending queue 
                       waiting to be processd and then remove it and send abort rsp , if FALSE then reject the message as 
                       it cant be processed

DEPENDENCIES None

RETURN VALUE  TRUE if message found else FALSE

SIDE EFFECTS 

===========================================================================*/  

boolean rr_sglte_reject_pending_x2g_req(boolean abort_rcvd, const gas_id_t gas_id)
{
  boolean x2g_req_found = FALSE;
  rr_pending_queue_T *pend_queue_ptr;
  rr_cmd_type        *cmd_ptr;

  pend_queue_ptr = rr_get_pending_queue_ptr(gas_id);
  if (rr_is_sglte_mode_active(gas_id))
  {
    if (pend_queue_ptr != NULL)
    {
       cmd_ptr = (rr_cmd_type *)q_check(&(pend_queue_ptr->rr_cmd_pend_q));
    }
    else
    {  cmd_ptr = NULL;

    }

    while (cmd_ptr != NULL && x2g_req_found == FALSE)
    {
      switch (cmd_ptr->msg.message_header.message_set)
      {
#ifdef FEATURE_GSM_TDS
        case MS_TDSRRC_RR:
        {
          switch (cmd_ptr->msg.message_header.message_id)
          {
            case TDSRR_INTERRAT_RESELECTION_REQ:
            {
              if (abort_rcvd)
              {
                rr_x2g_send_x2g_resel_abort_rsp(RR_RAT_UTRAN_TDD, gas_id);
              }
              else
              {
                rr_sglte_reject_tds_resel_req(gas_id);
              }
              x2g_req_found = TRUE;
              break;
            }
            case TDSRR_INTERRAT_REDIRECT_REQ:
            {
              if (abort_rcvd)
              {
                rr_x2g_send_x2g_redir_abort_rsp(RR_RAT_UTRAN_TDD, gas_id);
              }
              else
              {
                rr_sglte_reject_tds_redir_req(gas_id);
              } 
              x2g_req_found = TRUE;
              break;
            }
            case TDSRR_INTERRAT_CC_ORDER_REQ:
            {
              if (abort_rcvd)
              {
                rr_x2g_send_x2g_cco_abort_rsp(RR_RAT_UTRAN_TDD, gas_id);
              }
              else
              {               
                rr_x2g_send_x2g_cco_rsp_rejection(RR_RAT_UTRAN_TDD, gas_id); 
              }
              x2g_req_found = TRUE;                
              break;
            }
            default:
            break;
          }
          break;
        }
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
        case MS_MSGR_RR:
        {
          switch (cmd_ptr->msg.rr_msgr_msg.message.hdr.id)
          {
            case GERAN_GRR_LTE_RESEL_REQ:
            {
              if (abort_rcvd)
              {
                rr_x2g_send_x2g_resel_abort_rsp(RR_RAT_LTE, gas_id);
              }
              else
              { 
                rr_sglte_reject_lte_resel_req(gas_id);
              }
              x2g_req_found = TRUE;
              break;
            }
            case GERAN_GRR_LTE_REDIR_REQ:
            {
              if (abort_rcvd)
              {
                rr_x2g_send_x2g_redir_abort_rsp(RR_RAT_LTE, gas_id);
              }
              else
              {
                rr_sglte_reject_lte_redir_req(gas_id);
              }
              x2g_req_found = TRUE;
              break;
            }
            case GERAN_GRR_LTE_CCO_REQ:
            {
              if (abort_rcvd)
              {
                rr_x2g_send_x2g_cco_abort_rsp(RR_RAT_LTE, gas_id);
              }
              else
              {             
                rr_x2g_send_x2g_cco_rsp_rejection(RR_RAT_LTE, gas_id);
              }
              x2g_req_found = TRUE;
              break;
            }
            default:
            break;
           }
          break;
          }
#endif /* FEATURE_LTE */

        default:
        break;
    }
       if (x2g_req_found)
       {
        if (cmd_ptr->msg.message_header.message_set == MS_TDSRRC_RR)
        {
           MSG_GERAN_HIGH_1_G("RR SGLTE rejecting T2G request msg_id %d", cmd_ptr->msg.message_header.message_id);
        }
        else if (cmd_ptr->msg.message_header.message_set == MS_MSGR_RR)
        {
       MSG_GERAN_HIGH_1_G("RR SGLTE rejecting L2G request msg_id %d", cmd_ptr->msg.rr_msgr_msg.message.hdr.id);
        }
            q_delete(
      &(pend_queue_ptr->rr_cmd_pend_q),
            &cmd_ptr->link);
        /* Free memory allocated for this message */
        GPRS_MEM_FREE(cmd_ptr);
       }
       else
       {
          cmd_ptr = (rr_cmd_type *) q_next(&(pend_queue_ptr->rr_cmd_pend_q), &cmd_ptr->link);
       }
     }
   }
  return x2g_req_found;
}
#endif /*FEATURE_SGLTE*/


/**
  @brief Check for messages on the pending message queue that may now be
  processed.

  This function iterates over each message in the pending message queue.  For
  each message, the function 'should_message_be_processed' is called.  If this
  function indicates that the message should be processed, the message is
  deleted from the pending message queue and placed on the command queue.

  @see should_message_be_processed

*/
void rr_check_for_pending_messages(
  const gas_id_t gas_id
)
{
  rr_pending_queue_T *pend_queue_ptr;
  rr_cmd_type        *cmd_ptr;
  rr_cmd_type        *next_cmd_ptr;
  boolean             pending_message_found = FALSE;

  pend_queue_ptr = rr_get_pending_queue_ptr(gas_id);

  /* get the mutex before starting queue operation */
  RR_Q_LOCK();
  
  if (pend_queue_ptr != NULL)
  {
    cmd_ptr = (rr_cmd_type *)q_check(&(pend_queue_ptr->rr_cmd_pend_q));
  }
  else
  {
    cmd_ptr = NULL;
  }

  if (cmd_ptr != NULL)
  {
    // Find the head of the normal command queue
    rr_cmd_type *norm_cmd_ptr = q_check(&rr_cmd_q);
    // Find the first non-internal (RR-RR) message
    while ((norm_cmd_ptr != NULL) &&
           (norm_cmd_ptr->msg.message_header.message_set == MS_RR_RR))
    {
      norm_cmd_ptr = (rr_cmd_type *) q_next(&rr_cmd_q, &norm_cmd_ptr->link);
    }
    // norm_cmd_ptr now points to the position in the command queue to insert the pending messages
    // (This will be NULL if the queue is empty at this position, or may point to a message, in which
    // case the pending messages need to be inserted at this position)

    do
    {
      /* Get the next cmd_ptr now, as we may delete the current queue element
      as a result of the following checks, so we will not be able to get the
      next element otherwise. */
      next_cmd_ptr = (rr_cmd_type *) q_next(&(pend_queue_ptr->rr_cmd_pend_q), &cmd_ptr->link);

      if (should_message_be_processed(&cmd_ptr->msg, gas_id) == RR_PROCESS_MSG_NOW)
      {
        // This is the first queue item which is not MS_RR_RR
        if (norm_cmd_ptr == NULL)
        {
          pending_message_found = TRUE;
          q_delete(
            &(pend_queue_ptr->rr_cmd_pend_q),
            &cmd_ptr->link);
          q_put(&rr_cmd_q, &cmd_ptr->link);
        }
        else //There are elements in the command queue so insert first the pending messages
        {
          pending_message_found = TRUE;
          q_delete(
            &(pend_queue_ptr->rr_cmd_pend_q),
            &cmd_ptr->link);

          // Insert the new message into the queue
          q_insert(
            &rr_cmd_q,
            &cmd_ptr->link,
            &norm_cmd_ptr->link);
        }
      }

      cmd_ptr = next_cmd_ptr;
    } while (cmd_ptr != NULL);

    if (pending_message_found)
    {
      (void) rex_set_sigs( rex_self(), RR_CMD_Q_SIG );
    }
  }

  /* release the mutex */
  RR_Q_UNLOCK();
}


/**
  @brief Removes any messages from the RR pending queue
*/
void rr_flush_pending_messages(
  const gas_id_t gas_id
)
{
  rr_pending_queue_T *pend_queue_ptr;
  rr_cmd_type        *cmd_ptr;
  rr_cmd_type        *next_cmd_ptr;

  MSG_GERAN_HIGH_0_G("Flushing pending message queue");

  pend_queue_ptr = rr_get_pending_queue_ptr(gas_id);
  if (pend_queue_ptr != NULL)
  {
    cmd_ptr = (rr_cmd_type *)q_check(&(pend_queue_ptr->rr_cmd_pend_q));
  }
  else
  {
    cmd_ptr = NULL;
  }

  if (cmd_ptr != NULL)
  {
    do
    {
      /* Get the next cmd_ptr now, as we may delete the current queue element
      as a result of the following checks, so we will not be able to get the
      next element otherwise. */
      next_cmd_ptr = (rr_cmd_type *) q_next(&(pend_queue_ptr->rr_cmd_pend_q), &cmd_ptr->link);

      MSG_SPRINTF_1(
        MSG_SSID_DFLT,
        MSG_LEGACY_HIGH,
        "Discarding message: %s",
        rr_gprs_message_name(&cmd_ptr->msg, cmd_ptr->msg.message_header.message_set, cmd_ptr->msg.message_header.message_id, NULL)
      );

      q_delete(
        &(pend_queue_ptr->rr_cmd_pend_q),
        &cmd_ptr->link);

      /* Free memory allocated for this message */
      GPRS_MEM_FREE(cmd_ptr);

      cmd_ptr = next_cmd_ptr;
    } while (cmd_ptr != NULL);
  }
}

/**
  @brief Wait for the given signal (and other standard RR signals)

  @param requested_mask Requested signals to wait on

  @return The signal mask when the waiting period is completed
*/
rex_sigs_type rr_wait
(
   rex_sigs_type requested_mask    /* Mask of signals to wait for */
)
{
  do
  {
    // wait for either a watchdog signal or the requested signal
    sigs = rex_wait(RR_RPT_TIMER_SIG | requested_mask);

    // if a watchdog signal is received
    if (sigs & RR_RPT_TIMER_SIG)
    {
      (void) rex_clr_sigs( rex_self(), RR_RPT_TIMER_SIG );

      /* Report to the Dog HB */
      dog_hb_report(rr_dog_rpt_var);
    }
  } while ((sigs & requested_mask) == 0);

  return sigs;
} /* end rr_wait() */


/**
  @brief Initialise Message Router
*/
static void rr_main_msgr_init(void)
{
  errno_enum_type msgr_error;
  msgr_id_t       msgr_id;
  uint32         i;

  (void) q_init(&rr_msgr_cmd_q);
  (void) q_init(&rr_msgr_cmd_free_q);

  for (i = 0; i < RR_MSGR_QUEUE_SIZE; i++)
  {
    q_link_type * link = q_link(&rr_msgr_message_pool[i], &rr_msgr_message_pool[i].link);
    q_put(&rr_msgr_cmd_free_q, link);
  }

  msgr_error = msgr_client_create(&msgr_client);
  ASSERT(msgr_error == E_SUCCESS);
  msgr_error = msgr_client_add_rex_q(
    &msgr_client,                           //!< Message Client Handle
    rex_self(),                             //!< REX TCB pointer
    RR_MSGR_Q_SIG,                          //!< REX TCB pointer
    &rr_msgr_cmd_q,                         //!< queue to place the msg
    &rr_msgr_cmd_free_q,                    //!< queue to get empty buffer
    offsetof(geran_grr_cmd_t, message),     //!< offset to msgr_hdr
    MSGR_NO_OFFSET,                         //!< offset to legacy cmd_type
    sizeof(geran_grr_cmd_t),                //!< max size of msgs
    &msgr_id                                //!< queue identifier
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register_range(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    GERAN_GRR_REQ_FIRST + 1,
    GERAN_GRR_REQ_LAST
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register_range(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    GERAN_GRR_IND_FIRST + 1,
    GERAN_GRR_IND_LAST
  );
  ASSERT(msgr_error == E_SUCCESS);

#ifdef FEATURE_LTE
  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    LTE_RRC_G_RESEL_FAILED_RSP
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    LTE_RRC_G_REDIR_FAILED_RSP
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    LTE_RRC_G_ABORT_RESEL_RSP
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    LTE_RRC_G_ABORT_REDIR_RSP
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    LTE_RRC_G_PLMN_SRCH_RSP
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    LTE_RRC_G_ABORT_PLMN_SRCH_RSP
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    LTE_RRC_CCO_NACC_COMPLETED_IND
  );
  ASSERT(msgr_error == E_SUCCESS);
  
  msgr_error = msgr_register(
      MSGR_GERAN_GRR,
      &msgr_client,
      msgr_id,
      GERAN_GRR_LTE_GERAN_FREQ_INFO_IND
    );
    ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(
      MSGR_GERAN_GRR,
      &msgr_client,
      msgr_id,
      GERAN_GRR_LTE_GERAN_ACTIVITY_IND
    );
    ASSERT(msgr_error == E_SUCCESS);
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_BAND_AVOIDANCE
  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    MCS_CXM_BAND_AVOID_BLIST_GSM1_IND
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    MCS_CXM_BAND_AVOID_BLIST_GSM2_IND
  );
  ASSERT(msgr_error == E_SUCCESS);
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/
}


/**
  @brief Processes MPH_RESET_IND sent when GL1 triggers panic reset
*/
static void rr_handle_l1_reset_ind(const gas_id_t gas_id)
{
  rr_panic_reset_triggered_by_l1(gas_id);
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  (void)rr_control(EV_PANIC_RESET, NULL, gas_id);
#else
  geran_initiate_recovery_restart(GERAN_CLIENT_GL1, gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return;
}


/**
  @brief Processes a single message from RR task queue
*/
static void rr_process_message(
  rr_cmd_bdy_type   * new_message,
  rr_message_type_e   message_type,
  const gas_id_t      gas_id
)
{
  byte protocol_discriminator;

  /*
  A MS_RR_RR_EXT message set is used when a RR-RR message in sent, but triggered by a call-back event,
  e.g. a call-back function is called, which sends a message into RR. The message set MS_RR_RR_EXT is used
  to ensure the message is queued as if received from an external task, rather than being queued ahead of
  other non-RR-RR messages. But once sent into RR, it can be processed as a standard RR-RR message
  */
  if (new_message->message_header.message_set == MS_RR_RR_EXT)
  {
    new_message->message_header.message_set = MS_RR_RR;
  }

  /* log the input message for RR */
  rr_show_input_message(
    message_type,
    new_message,
    new_message->message_header.message_set,
    new_message->message_header.message_id,
    rr_main_current_state_name(gas_id),
    get_rr_state(gas_id),
    gas_id
  );

  if (new_message->message_header.message_set == MS_RR_RR)
  {
    // The destination for RR_*_IMSG_* messages is always the module that IMSG is defined for

    switch (new_message->rr.header.rr_message_set)
    {
      case RR_DTM_IMSG:
      {
        (void)rr_dtm_control(EV_INPUT_MESSAGE, new_message, gas_id);
        break;
      }

      case RR_PS_IMSG:
      {
        rr_ps_access_control(new_message, gas_id);
        break;
      }

      case RR_L1_IDLE_IMSG:
      {
        rr_l1_idle_control(new_message, gas_id);
        break;
      }

      case RR_SELECT_BCCH_IMSG:
      {
        rr_select_bcch_control(new_message, gas_id);
        break;
      }

      case RR_RESEL_IMSG:
      {
        (void)rr_resel_control(EV_INPUT_MESSAGE, new_message, gas_id);
        break;
      }

      case RR_PSCAN_IMSG:
      {
        rr_pscan_control(EV_INPUT_MESSAGE, new_message, gas_id);
        break;
      }

      case RR_IMSG:
      {
        switch (new_message->rr.rr.imsg.header.imh.message_id)
        {
#ifdef FEATURE_QSH_EVENT_METRIC
          case RR_IMSG_QSH_METRIC_CONFIG_IND:
          {
            rr_qsh_process_metric_config_ind(&new_message->rr.rr.imsg.qsh_metric_config_ind);
            break;
          }

          case RR_IMSG_QSH_METRIC_TIMER_EXPIRY_IND:
          {
            rr_qsh_process_metric_timer_expiry_ind(&new_message->rr.rr.imsg.qsh_metric_timer_expiry_ind);
            break;
          }
#endif // FEATURE_QSH_EVENT_METRIC

          default:
          {
            (void)rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
          }
        }
		break;
      }

      default:
      {
        (void)rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
      }
    }
  }

  /***************************************************/
  /* The following statement deals with the messages */
  /* coming from the data link (DL) layer            */
  /***************************************************/
  else if (new_message->message_header.message_set == MS_RR_L2)
  {
    l2_channel_type_T l2_channel_type;
    byte              skip_indicator = 0x00;

    switch (new_message->message_header.message_id)
    {
      case DL_RELEASE_IND:
      {
        /**********************************************************/
        /* Forward the message to rr_control or SAPI3 control     */
        /**********************************************************/

        if (new_message->dl_release_ind.SAPI == SAPI3)
        {
          (void)rr_control_SAPI3_link(new_message, gas_id);
        }
        else
        {
          rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
        }
      }
      break;

      case DL_SUSPEND_CNF:
      case DL_RESUME_CNF:
      case DL_RECONNECT_CNF:
      case DL_RANDOM_ACCESS_CNF:
      {
        /********************************************/
        /* Forward the message to the control of RR */
        /********************************************/
        rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
      }
      break;

      case MDL_ERROR_IND :
      {
        /*******************************************************/
        /* only respond to specific errors, such as T200 timer */
        /* expiry, sequence error, unsolicited DM frame with   */
        /* with F = 0, or N200 retries                         */
        /*******************************************************/

        if (
           (new_message->mdl_error_ind.error_code == T200_EXPIRY )      ||
           (new_message->mdl_error_ind.error_code == SEQUENCE_ERROR)    ||
           (new_message->mdl_error_ind.error_code == N200_EXCEEDED_PAR) ||
           (new_message->mdl_error_ind.error_code == UNSOLICITED_DM_RESPONSE_PAR)
           )
        {
          rr_event_T new_event;

          /********************************************/
          /* Forward the message to RR control or the */
          /* RR SAPI3 state machine                   */
          /********************************************/

          if (new_message->mdl_error_ind.SAPI == SAPI3)
          {
            /* SAPI3 state machine can generate a RADIO_LINK_FAILURE event here */
            new_event = rr_control_SAPI3_link(new_message, gas_id);
          }
          else
          {
            new_event = EV_INPUT_MESSAGE;
          }

          if (new_event != EV_NO_EVENT)
          {
            rr_control(new_event, new_message, gas_id);
          }
        }
        else
        {
          MSG_GERAN_MED_1_G("Ignoring MDL_ERROR_IND=%d from L2",(int) new_message->mdl_error_ind.error_code);
        }
      }
      break;

      case DL_RELEASE_CNF :
      {
        if (new_message->dl_release_cnf.SAPI == SAPI3)
        {
          (void)rr_control_SAPI3_link(new_message, gas_id);
        }
        else
        {
          rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
        }
      }
      break;

      case DL_ESTABLISH_CNF :
      {
        /***************************************************/
        /* A layer 2 link has just been established. It is */
        /* either a SAPI 0 or SAPI 3 link                  */
        /***************************************************/
        if (new_message->dl_establish_cnf.SAPI == SAPI3)
        {
          /*******************************************/
          /* It is a SAPI 3 link : update the SAPI 3 */
          /* state machine                           */
          /*******************************************/
          (void)rr_control_SAPI3_link(new_message, gas_id);
        }
        else
        {
          /*******************************************/
          /* It is a SAPI 0 link : pass the message  */
          /* to the main RR state machine            */
          /*******************************************/
          rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
        }
      }
      break;

      case DL_ESTABLISH_IND :
        if (new_message->dl_establish_ind.SAPI == SAPI0)
        {
          /* this must be a L2 re-establishment */
          rr_handle_l2_reestablishment(&(new_message->dl_establish_ind), gas_id);

          /* log any changes to the channel configuration */
          (void)rr_log_channel_configuration(gas_id);
        }
        else
        {
          (void)rr_control_SAPI3_link(new_message, gas_id);
        }
        break;

      case DL_DATA_IND :
      {
        /**************************************************/
        /* This is a layer 3 peer to peer message.        */
        /* We have to check the protocol discriminator    */
        /* If the latter match the RR one, we have to     */
        /* forward the message to the RR main task. If    */
        /* not, we have to forward it to the layer above, */
        /* ie the MM layer.                               */
        /* The skip indicator has to be checked as well,  */
        /* but only if the message if for RR or MM        */
        /**************************************************/
        skip_indicator = new_message->dl_data_ind.layer3_message[0] & 0xF0;
        protocol_discriminator = new_message->dl_data_ind.layer3_message[0] & 0x0F;

#ifdef FEATURE_CGPS_UMTS_CP_GSM
        rr_cgps_apdu_in_progress_check(new_message, gas_id);
        rr_cgps_lcs_activity_check(new_message, gas_id);
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

        switch (protocol_discriminator)
        {
          case RADIO_RESSOURCES :
          {
            if (skip_indicator == 0x00)
            {
              /******************************************/
              /* The message is taken in consireration. */
              /* Pass it to the main RR state machine   */
              /******************************************/
              /* log the signalling message */
              (void) rr_log_signaling_message
                (
                  new_message->dl_data_ind.l2_channel_type,
                  /* MessageType is always in octet1 for DL_DATA_IND for RR */
                  new_message->dl_data_ind.layer3_message[1],
                  new_message->dl_data_ind.l3_message_length,
                  new_message->dl_data_ind.layer3_message,
                  RR_DOWNLINK_MSG,
                  gas_id
                );
              rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
            }
            else
            {
              MSG_GERAN_MED_0_G("Ignoring OTA message with skip_indicator set");
            }
          }
          break;

          case MOBILITY_MANAGEMENT :
          {
            if (skip_indicator == 0x00)
            {
              /******************************************/
              /* Forward the message to MM              */
              /******************************************/
              rr_send_data_ind(
                new_message->dl_data_ind.layer3_message,
                new_message->dl_data_ind.l3_message_length,
                gas_id
              );
            }
          }
          break;

          case TEST_MANAGEMENT:
          {
            if (skip_indicator == 0x00)
            {
              channel_spec_T *chan_spec_ptr;

              /* log the signalling message */
              (void) rr_log_signaling_message(
                new_message->dl_data_ind.l2_channel_type,
                /* MessageType is always in octet1 for DL_DATA_IND for RR */
                new_message->dl_data_ind.layer3_message[1],
                new_message->dl_data_ind.l3_message_length,
                new_message->dl_data_ind.layer3_message,
                RR_DOWNLINK_MSG,
                gas_id
              );

              /***************************/
              /* Check that a TCH is on. */
              /***************************/
              chan_spec_ptr = rr_get_channel_spec(NULL, gas_id);
              if (chan_spec_ptr != NULL)
              {
                if ((chan_spec_ptr->number_of_ded_channels != 0) &&
                    ((chan_spec_ptr->channel_info_1_after.channel_type == FULL_RATE_TRAFFIC) ||
                     (chan_spec_ptr->channel_info_1_after.channel_type == HALF_RATE_TRAFFIC)))
                {
                  rr_handle_test_messages(&(new_message->message_header), gas_id);
                }
                else
                {
                  MSG_GERAN_MED_1_G("Received TEST message=%d without TCH",
                          (int) new_message->dl_data_ind.layer3_message[1]);
                }
              }
            }
          } /* TEST_MANAGEMENT */
          break;

          #ifdef FEATURE_GSM_DTM
          case GTTP:
          {
            dl_data_ind_T * dl_data_ind = &new_message->dl_data_ind;

            rr_dtm_gttp_message_received(dl_data_ind, gas_id);
            break;
          }
          #endif /* FEATURE_GSM_DTM */

          default :
          {
            /******************************************/
            /* Forward the message to MM              */
            /******************************************/
            rr_send_data_ind(
              new_message->dl_data_ind.layer3_message,
              new_message->dl_data_ind.l3_message_length,
              gas_id
            );
          }
          break;
        } /* switch (protocol_discriminator) */
      }
      break;

      case DL_UNIT_DATA_IND:
      {
        /***************************************************/
        /* This is a layer 3 peer to peer message received */
        /* in unacknowledgement manner.                    */
        /* We have to check the protocol discriminator     */
        /* If the latter match the RR one, we have to      */
        /* forward the message to the RR control  . If     */
        /* not, we have to forward it to the layer above,  */
        /* ie the MM layer.                                */
        /* The skip indicator has to be checked as well,   */
        /* but only if the message if for RR or MM         */
        /***************************************************/
        boolean short_header;
        byte    first_byte = 0;
        byte    second_byte = 0;
        dl_unit_data_ind_T *dl_unit_data_ind_ptr = &new_message->dl_unit_data_ind;

        l2_channel_type = dl_unit_data_ind_ptr->l2_channel_type;
        short_header = dl_unit_data_ind_ptr->short_header;

        if ((l2_channel_type == BCCH) || (l2_channel_type == CCCH))
        {
          skip_indicator = dl_unit_data_ind_ptr->layer3_message[1] & 0xF0;
          protocol_discriminator = dl_unit_data_ind_ptr->layer3_message[1] & 0x0F;
          first_byte = dl_unit_data_ind_ptr->layer3_message[1];
          second_byte = dl_unit_data_ind_ptr->layer3_message[2];
        }

        else if ((l2_channel_type == SACCH) && (short_header == TRUE))
        {
          /* 8th bit of octet 1 is the RR short PD */
          first_byte = dl_unit_data_ind_ptr->layer3_message[0];
          second_byte = dl_unit_data_ind_ptr->layer3_message[1];
          protocol_discriminator = dl_unit_data_ind_ptr->layer3_message[0] & 0x80;

          if (protocol_discriminator == RADIO_RESOURCES_SHORT_PD)
          {
            /* change it to RADIO_RESSOURCES */
            protocol_discriminator = RADIO_RESSOURCES;
            skip_indicator = 0x00;
          }
        }

        else
        {
          skip_indicator = dl_unit_data_ind_ptr->layer3_message[0] & 0xF0;
          protocol_discriminator = dl_unit_data_ind_ptr->layer3_message[0] & 0x0F;
          first_byte = dl_unit_data_ind_ptr->layer3_message[0];
          second_byte = dl_unit_data_ind_ptr->layer3_message[1];
        }

        switch (protocol_discriminator)
        {
          case RADIO_RESSOURCES:
          {
            byte message_type;
            byte message_length = dl_unit_data_ind_ptr->l3_message_length;

            if (skip_indicator == 0x00)
            {
              if ((l2_channel_type == BCCH) || (l2_channel_type == CCCH))
              {
                message_type = dl_unit_data_ind_ptr->layer3_message[2];
              }
              else if ((l2_channel_type == SACCH) && (short_header == TRUE))
              {
                message_type = dl_unit_data_ind_ptr->layer3_message[0] & 0x7C;

                /* shift right by 2 bits */
                message_type >>= 2;

                /* MEASUREMENT_INFORMATION is 0x5 which is also SYSTEM_INFORMATION_TYPE_5_BIS */
                /* So, change it to a different unused value */
                if (message_type == MEASUREMENT_INFORMATION)
                {
                  message_type = MEASUREMENT_INFORMATION_TYPE;
                }
                else
                {
                  /* Set to any other message type will be dropped at RR control */
                  message_type = UNKNOWN_SHORT_HEADER_MESSAGE_TYPE;
                }
              }
              else
              {
                message_type = new_message->dl_unit_data_ind.layer3_message[1];
              }

              /* log the signalling message */
              (void) rr_log_signaling_message(
                l2_channel_type,
                message_type,
                message_length,
                dl_unit_data_ind_ptr->layer3_message,
                RR_DOWNLINK_MSG,
                gas_id
              );

              switch (message_type)
              {
                case SYSTEM_INFORMATION_TYPE_1:
                case SYSTEM_INFORMATION_TYPE_2:
                case SYSTEM_INFORMATION_TYPE_2_BIS:
                case SYSTEM_INFORMATION_TYPE_2_TER:
                case SYSTEM_INFORMATION_TYPE_2_QUATER:
                case SYSTEM_INFORMATION_TYPE_3:
                case SYSTEM_INFORMATION_TYPE_4:
                case SYSTEM_INFORMATION_TYPE_5:
                case SYSTEM_INFORMATION_TYPE_5_TER:
                case SYSTEM_INFORMATION_TYPE_5_BIS:
                case MEASUREMENT_INFORMATION_TYPE:
                case SYSTEM_INFORMATION_TYPE_6:
                case SYSTEM_INFORMATION_TYPE_7:
                case SYSTEM_INFORMATION_TYPE_8:
                case SYSTEM_INFORMATION_TYPE_13:
                {
                  if (!grr_get_l1_in_test_mode(gas_id))
                  {
                    if (message_length <= N201_MAX)
                    {
                      rr_store_system_info(dl_unit_data_ind_ptr, gas_id);
                    }
                  }
                  break;
                }

                case PAGING_REQUEST_TYPE_1:
                case PAGING_REQUEST_TYPE_2:
                case PAGING_REQUEST_TYPE_3:
                {
                  boolean message_ok = TRUE;
                  RR_cause_T error_type;

                  // Check for errors in the encoding of the OTA message
                  if (dl_unit_data_ind_ptr->l3_message_length <= N201_MAX)
                  {
                    rr_check_for_protocol_errors(
                      dl_unit_data_ind_ptr->layer3_message,
                      dl_unit_data_ind_ptr->l3_message_length,
                      dl_unit_data_ind_ptr->l2_channel_type,
                      dl_unit_data_ind_ptr->short_header,
                      &message_ok,
                      &error_type,
                      gas_id
                    );
                  }
                  else
                  {
                    error_type = RR_SEMANTICALLY_INCORRECT_MESSAGE;
                    MSG_GERAN_MED_3_G("Message length exceeds message payload size:Received message length: %d,expected:%d, cause=%d",
                      dl_unit_data_ind_ptr->l3_message_length,
                      N201_MAX,
                      (int) error_type
                    );
                    message_ok = FALSE;
                  }

                  if (message_ok == TRUE)
                  {
                    // Process a paging request message. This could cause
                    //  - sending of RR_IMSG_PAGE_MODE_IND if the page mode has changed
                    //  - sending of RR_IMSG_PAGE_IND if the page addresses the MS
                    rr_ce_process_page_request(
                      message_type,
                      dl_unit_data_ind_ptr->own_paging_block,
                      gas_id
                    );
                  }
                  else /* the L3 message was not OK */
                  {
                    MSG_GERAN_ERROR_1_G("RR Bad Page Message (%d)",(int)error_type);
                  }
                  break;
                }

                default:
                {
                  rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
                }
              }
            }
            break;
          } /* case RADIO_RESSOURCES */

          default:
          {
            /**********************************************/
            /* The protocol discriminator in the msg      */
            /* is not the PD or RR. This is an impossible */
            /* case, as all the unacknowledge messages    */
            /* should be destinated to RR                 */
            /* However, it soes seen to happen            */
            /* occasionally...                            */
            /**********************************************/

            /* Ignore L2 padding octets */
            if(!((first_byte == 0x2B) && (second_byte == 0x2B)))
            {
              (void) rr_log_protocol_error(0, RR_PROTOCOL_ERROR_UNKNOWN_PD, gas_id);
              ERR("RR Bad(%d) Protocol Discriminator",(int) protocol_discriminator,0,0);

              /**********************************************/
              /* Print dl_unit_data_ind contents for        */
              /* debugging                                  */
              /**********************************************/
              MSG_GERAN_HIGH_3_G("sapi:%d chtyp:%d ownpgblk:%d",
                new_message->dl_unit_data_ind.SAPI,
                new_message->dl_unit_data_ind.l2_channel_type,
                new_message->dl_unit_data_ind.own_paging_block);

              MSG_GERAN_HIGH_3_G("tc:%d chann:%d l3leng:%d",
                new_message->dl_unit_data_ind.tc,
                new_message->dl_unit_data_ind.channel_number.num,
                new_message->dl_unit_data_ind.l3_message_length);

              MSG_GERAN_HIGH_3_G("l3[0]:%d l3[1]:%d l3[2]:%d",
                new_message->dl_unit_data_ind.layer3_message[0],
                new_message->dl_unit_data_ind.layer3_message[1],
                new_message->dl_unit_data_ind.layer3_message[2]);

              MSG_GERAN_HIGH_3_G("l3[3]:%d l3[4]:%d l3[5]:%d",
                new_message->dl_unit_data_ind.layer3_message[3],
                new_message->dl_unit_data_ind.layer3_message[4],
                new_message->dl_unit_data_ind.layer3_message[5]);

              MSG_GERAN_HIGH_3_G("l3[6]:%d l3[7]:%d l3[8]:%d",
                new_message->dl_unit_data_ind.layer3_message[6],
                new_message->dl_unit_data_ind.layer3_message[7],
                new_message->dl_unit_data_ind.layer3_message[8]);

              MSG_GERAN_HIGH_3_G("l3[9]:%d l3[10]:%d l3[11]:%d",
                new_message->dl_unit_data_ind.layer3_message[9],
                new_message->dl_unit_data_ind.layer3_message[10],
                new_message->dl_unit_data_ind.layer3_message[11]);
            }
            else
            {
              MSG_GERAN_MED_0_G("Ignoring L2 padding octets");
            }
            break;
          }
        } /* switch (protocol_discriminator) */

        break;
      } /* DL_DATA_IND */

      default:
      {
        /* some other L2 message not handled above */
        /* forward to control */
        rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
      }
      break;
    } /* switch(new_message->message_header.message_id) */
  } /* if (new_message->message_header.message_set == MS_RR_L2) */

  /***************************************************/
  /* The following statement deals with the messages */
  /* comming from the mobility management (MM) layer */
  /***************************************************/
  else if (new_message->message_header.message_set == MS_MM_RR)
  {
    protocol_discriminator =
      new_message->rr_data_req.layer3_message[0] & 0x0F;

    switch (new_message->message_header.message_id)
    {
      case RR_SIM_INSERTED_REQ:
      {
        rr_sim_process_sim_inserted_req(
          &new_message->mm.rr_sim_inserted_req,
          gas_id
        );
        break;
      }

      case RR_INVALIDATE_SIM_DATA_REQ:
      {
        rr_sim_process_invalidate_sim_data_req(
          &new_message->mm.rr_invalidate_sim_data_req,
          gas_id
        );
        break;
      }

      case RR_SIM_UPDATE_REQ:
        {
        rr_sim_process_sim_update_req(
          &new_message->mm.rr_sim_update_req,
          gas_id
        );
        break;
      }

      case RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ:
      {
        rr_ps_process_gmm_gprs_tlli_assignment_req(
          &new_message->mm.rr_gmm_gprs_tlli_assignment_req,
          gas_id
        );
        break;
      }

      case RR_GMM_GPRS_SERVICE_REQ:
      {
        rr_ps_process_gmm_gprs_service_req(
          &new_message->mm.rr_gmm_gprs_service_req,
          gas_id
        );
        break;
      }

      case RR_GMM_GPRS_STATE_CHANGE_REQ:
      {
        rr_ps_process_gmm_gprs_state_change_req(
          &new_message->mm.rr_gmm_gprs_state_change_req,
          gas_id
        );

        rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
        break;
      }

      case RR_GMM_READY_TIMER_STATE_REQ:
      {
        rr_ps_process_gmm_ready_timer_state_req(
          &new_message->mm.rr_gmm_ready_timer_state_req,
          gas_id
        );
        break;
      }

      case RR_GMM_GPRS_MS_RA_CAP_IE_IND:
      {
        rr_ps_process_gmm_gprs_ms_ra_cap_ie_ind(
          &new_message->mm.rr_gmm_gprs_ms_ra_cap_ie_ind,
          gas_id
        );
        break;
      }

      case RR_GMM_START_MM_NON_DRX_IND:
      {
        rr_ps_process_gmm_start_mm_non_drx_ind(
          &new_message->mm.rr_gmm_start_mm_non_drx_ind,
          gas_id
        );
        break;
      }

      case RR_GMM_STOP_MM_NON_DRX_IND:
      {
        rr_ps_process_gmm_stop_mm_non_drx_ind(
          &new_message->mm.rr_gmm_stop_mm_non_drx_ind,
          gas_id
        );
        break;
      }

      case RR_GMM_GPRS_NV_PARAMS_IND:
      {
        // Note: This message is processed according to the AS-ID in the message, GAS-ID is ignored
        rr_nv_process_nv_params_ind(
          &new_message->rr_gmm_gprs_nv_params_ind
        );
        break;
      }

      case RR_LAI_REJECT_LIST_IND:
      {
        rr_store_lai_reject_list(
          &new_message->mm.rr_lai_reject_list_ind,
          gas_id
        );
        break;
      }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
      case RR_PS_ABORT_REQ:
      {
        rr_ps_process_ps_abort_req(&new_message->mm.rr_ps_abort_req, gas_id);
        break;
      }

      case RR_PS_RESUME_IND:
      {
        rr_ps_process_ps_resume_ind(&new_message->mm.rr_ps_resume_ind, gas_id);
        break;
      }

#ifdef FEATURE_DUAL_DATA
      case RR_DATA_PRIORITY_IND:
      {
        rr_data_priority_ind_T *msg_ptr = &new_message->mm.rr_data_prio_ind;
        geran_priority_t        priority;

        // If NAS is indidating high priority, change the L1 priority to match
        if (SYS_DATA_PRIORITY_HIGH == msg_ptr->priority)
        {
          priority = GERAN_MMS_PRIORITY_MAXIMAL;
        }
        else if (SYS_DATA_PRIORITY_MED == msg_ptr->priority)
        {
          priority = GERAN_MMS_PRIORITY_INVERTED;
        }
        else
        {
          priority = GERAN_MMS_PRIORITY_NOMINAL;
        }

        rr_send_mph_set_priority_ind(
          priority,   // l1_priority
          TRUE,       // new_procedure
          gas_id      // gas_id
        );
        break;
      }
#endif /* FEATURE_DUAL_DATA */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#ifdef FEATURE_SGLTE
      case RR_STOP_GSM_MODE_REQ:
      {
        if (new_message->rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason == RR_SGLTE_RAT_CHANGE)
        {
          rr_send_imsg(RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ, gas_id);
        }
        else if (new_message->rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason == RR_SGLTE_USER_RAT_CHANGE )
        {
          rr_send_imsg(RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_USER_GSM_ONLY_REQ, gas_id);
        }
        else
        {
          rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
       }
        break;
      } /* RR_STOP_GSM_MODE_REQ */
#endif /* FEATURE_SGLTE */

      default:
      {
        rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
      }
    }
  }

#ifdef FEATURE_CGPS_UMTS_CP_GSM
  /*******************************************************/
  /* The following statement deals with the messages     */
  /* coming from GPS                                     */
  /*******************************************************/
  else if (new_message->message_header.message_set == MS_CGPS_RR)
  {
    switch (new_message->message_header.message_id)
    {
      case RR_APDU_REQ:
      {
        /* Send APDU request to RR state machine for processing */
        rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
        break;
      }

      case RR_CGPS_EVENT_REQ:
      {
        /* Send CGPS request to RR state machine for processing */
        rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
        break;
      }

      default:
        break;
    }
  }
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

  else if (new_message->message_header.message_set == MS_RRC_RR)
  {
#ifdef FEATURE_FEMTO_GSM_NL
    // Pass all Femto-related message thru to the RR-FEMTO module.
    // Otherwise, pass into RR as normal
    switch (new_message->message_header.message_id)
    {
      case RRC_RR_NBR_DISC_REQ:
      case RRC_RR_START_COSC_REQ:
      case RRC_RR_STOP_COSC_REQ:
      {
        rr_femto_control(new_message);
        break;
      }

      default:
      {
        rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
      }
    }
#else
    rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
#endif // FEATURE_FEMTO_GSM_NL
  }
  /***************************************************/
  /* The following statement deals with the messages */
  /* coming from the physical layer                  */
  /***************************************************/

  else if (new_message->message_header.message_set == MS_RR_L1)
  {
    /*********************************************/
    /* Forward the message to the control of RR  */
    /*********************************************/

    switch (new_message->message_header.message_id)
    {
      case MPH_RESET_IND:
      {
        rr_handle_l1_reset_ind(gas_id);
        break;
      }

      case MPH_START_GSM_MODE_CNF:
      {
        rr_event_T rr_event;

        rr_event = rr_mode_control(EV_INPUT_MESSAGE, new_message, gas_id);
        rr_control(rr_event, NULL, gas_id);
        break;
      }

      case MPH_START_GSM_MODE_FOR_BPLMN_CNF:
      {
        rr_event_T rr_event;

        rr_event = rr_mode_control(EV_INPUT_MESSAGE, new_message, gas_id);
        rr_control(rr_event, NULL, gas_id);
        break;
      }

      case MPH_STOP_GSM_MODE_CNF:
      {
        rr_event_T rr_event;

        rr_event = rr_mode_control(EV_INPUT_MESSAGE, new_message, gas_id);
        rr_control(rr_event, NULL, gas_id);
        break;
      }

      case MPH_START_IDLE_MODE_CNF:
      case MPH_ABORT_RA_CNF:
      {
        rr_l1_idle_control(new_message, gas_id);
        break;
      }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
      case MPH_L1_DS_ABORT_IND:
      {
        rr_ms_process_l1_ds_abort_ind(&new_message->mph.l1_ds_abort_ind);
        break;
      }

      case MPH_COEX_MSC_CHANGE_IND:
      {
        rr_msc_process_l1_coex_msc_change_ind(&new_message->mph.coex_msc_change_ind, gas_id);
        break;
      }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

      default:
      {
        rr_event_T rr_event = rr_gprs_process_gprs_messages(new_message, gas_id);

        if ( rr_event == EV_INPUT_MESSAGE )
        {
          rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
        }
        else
        if ( rr_event != EV_NO_EVENT )
        {
          rr_control(rr_event, NULL, gas_id);
        }

        break;
      }
    }  /* switch */
  }
  else if ((new_message->message_header.message_set == MS_TIMER) &&
           (new_message->message_header.message_id == MID_TIMER_EXPIRY))
  {
    switch (new_message->mid_timer_expiry.timer_id)
    {
      case T3142:
      {
        rr_ps_t3142_expiry(gas_id);
        break;
      }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
      case RR_DUAL_SIM_HEARTBEAT_TIMER:
      {
        rr_ms_process_heartbeat(gas_id);
        break;
      }

      case RR_DUAL_SIM_PS_ABORT_TIMER_1:
      case RR_DUAL_SIM_PS_ABORT_TIMER_2:
      {
        rr_ms_ps_abort_timer_expiry((rr_timer_id_e)(new_message->mid_timer_expiry.timer_id), gas_id);
        break;
      }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

      default:
      {
        // Handle other timer expiries in RR-CONTROL according to state
        rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
      }
    }
  }

  else if (new_message->message_header.message_set == MS_GHDI_EXPORT)
  {

    /*********************************************/
    /* Forward the message to the control of RR  */
    /*********************************************/

    rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
  }
  else if (new_message->message_header.message_set == MS_MSGR_RR)
  {
    switch (new_message->rr_msgr_msg.message.hdr.id)
    {
#ifdef FEATURE_GERAN_HST_OPT
      case GERAN_GRR_LTE_GERAN_FREQ_INFO_IND:
      {
        rr_store_lte_geran_freq_info_ind(&(new_message->rr_msgr_msg.message.geran_grr_msg_ind.lte_geran_freq_info), gas_id);
        break;
      }
#endif /*FEATURE_GERAN_HST_OPT*/
      default:
      {
        // Handle other timer expiries in RR-CONTROL according to state
        rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
        break;
      }
    }
  }
  else
  {
    rr_event_T  rr_event = rr_gprs_process_gprs_messages(new_message, gas_id);

    if ( rr_event == EV_INPUT_MESSAGE )
    {
      rr_control(EV_INPUT_MESSAGE, new_message, gas_id);
    }
    else
    if ( rr_event != EV_NO_EVENT )
    {
      rr_control(rr_event, NULL, gas_id);
    }
  }

} /* rr_process_message */


/**
  @brief Makes the RR task wait for a specified interval

  Note: Synchronous function call - processing context is effectively "paused"

  @param millisecs - wait duration (ms)
*/
void rr_timed_wait(dword millisecs)
{
  timer_attrib_struct_type   rr_sleep_timer_attrib;
  timer_get_params(&rr_sleep_timer,&rr_sleep_timer_attrib);

  if (rr_sleep_timer_attrib.tcb_sigs_obj != NULL)
  {
    (void)rex_timed_wait((rex_sigs_type)RR_SLEEP_SIG,
                         &rr_sleep_timer,
                         (rex_timer_cnt_type)millisecs);
  }
}

/* mmcp_sd_task_running **************************************************/
/* this function will be provided by MMCP but for now it's declared here */
boolean sd_task_has_started = FALSE;
boolean mmcp_sd_task_running(void)
{
  return sd_task_has_started;
}
/* mmcp_sd_task_running **************************************************/

/**
  @brief RR task top-level function

  Called at start-up to setup & initialise RR task, then enter wait loop
*/
void rr_main( dword rex_parm )
{
  (void) rex_parm;

  if (initialised == FALSE)
  {
    /* Perform startup init here. Satisfy dependency of other tasks
       provided by this task. Do not linger here, startup latency is
       impacted. Higher priority tasks in the same group will preempt
       this task at this point. Do not rely upon priority alone as the
       sole mechanism to arbitrate resource contention with other tasks. */

    rcinit_handshake_startup(); /* required rcinit handshake */
    /* Optional dog registration here */
    /* mmcp_sd_task_running **************************************************/
    /* Register for signal that SD has started up */
    rcevt_register_name(SD_TASK_RCEVT_READY, SD_TASK_STARTED_SIG);
    /* mmcp_sd_task_running **************************************************/
    gsm_rr_fn_init();


    initialised = TRUE;
  }

#ifdef PERLUTF
  #error code not present
#else
  for (;;)
  {
    // wait for either a CMD queue signal or a watchdog timer signal
    sigs = rex_wait(
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      RR_RPT_TIMER_SIG |
      RR_CMD_Q_SIG     |
      RR_MSGR_Q_SIG    |
      RR_RESET_SIG     |
      /* mmcp_sd_task_running **************************************************/
      SD_TASK_STARTED_SIG
      /* mmcp_sd_task_running **************************************************/
    );

    // if a watchdog signal is received
    if (sigs & RR_RPT_TIMER_SIG)
    {
      (void) rex_clr_sigs( rex_self(), RR_RPT_TIMER_SIG );

      /* Report to the Dog HB */
      dog_hb_report(rr_dog_rpt_var);
    }

    /* Initiate lower layer failure recovery */
    if (sigs & RR_RESET_SIG)
    {
      (void) rex_clr_sigs(rex_self(), RR_RESET_SIG);
      /* This signal will be received only in post TA releases so do recovery restart*/

#ifdef FEATURE_DUAL_SIM
      rr_main_reset_gas_id = check_gas_id(rr_main_reset_gas_id);
      rr_debug_log_panic_reset(rr_main_reset_gas_id);
#else
      rr_debug_log_panic_reset(GERAN_ACCESS_STRATUM_ID_1);
#endif /* FEATURE_DUAL_SIM */

#ifdef FEATURE_DUAL_SIM
      rr_control(EV_PANIC_RESET, NULL, rr_main_reset_gas_id);
#else
      rr_control(EV_PANIC_RESET, NULL, GERAN_ACCESS_STRATUM_ID_1);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
    }

    /* check for the signal that indicates a new message in the task queue */
    if (sigs & RR_CMD_Q_SIG)
    {
      (void) rex_clr_sigs(rex_self(), RR_CMD_Q_SIG);
      rr_read_messages();
    }

    if (sigs & RR_MSGR_Q_SIG)
    {
      (void) rex_clr_sigs(rex_self(), RR_MSGR_Q_SIG);
      rr_read_msgr_messages();
    }

    /* mmcp_sd_task_running **************************************************/
    /* this functionality will be provided by MMCP but for now it's declared here */
    if (sd_task_has_started == FALSE && (sigs & SD_TASK_STARTED_SIG))
    {
      sd_task_has_started = TRUE;
      /* Clear the OS signal */
      (void)rex_clr_sigs( rex_self(),
                          SD_TASK_STARTED_SIG
                         );
      /* It's now OK to call sd_misc_get_supp_band_pref() because SD task has signalled that it's running */
    }

#ifdef TEST_FRAMEWORK
    #error code not present
#endif /* TEST_FRAMEWORK */
  } /* end for */
#endif /* PERLUTF */

  /* Deregister if leaving the task so that we don't ERR_FATAL */
  dog_hb_deregister(rr_dog_rpt_var);
  /*UNREACHED*/

}


/**
  @brief Alternative entry-point for RR task top-level function
*/
void gsm_rr_task(dword dummy)
{
   rr_main(dummy);
}


/**
  @brief Peform all initialisation required at task-start

  The following are initialised here -
  Mutexes / Critical Sections;
  RR task queues and registration with General Services;
  RR timers;
  NV registration;
  Configuration of GERAN EFS-NV;
  Message Routiner registration;
  RR module task-start initialisation
*/
void gsm_rr_fn_init(void)
{
  /* Only use this mechanism to perform memory init, hardware init,
     or other startup init to satisfy dependency that follow. Avoid
     all HLOS mechanisms that will block. */

  // Initialise Mutexes / Critical Sections
  rr_main_init_resource_locks();

  geran_tcb_set(GERAN_TASK_ID_GRR, rex_self());

  // Create task queue; Register with GS services; Create task timers
  rr_main_task_init();

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  rr_ms_task_start_init();
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  // Initialise RR timer module (ready for use during general RR module initialisation)
  rr_timer_init();

  // Initialise system information cache
  rr_csi_init_cache();

  // Register for NV access
#ifndef PERLUTF
  if(ghdi_nvmem_register_task(RR_NV_SIG, rr_wait,NULL) != GHDI_SUCCESS)
  {
    ERR_GERAN_FATAL_0("UE could not register with nvmem task");
  }
#endif /* PERLUTF */

  // Read the classmark information so it is cached (the return value is ignored)
  {
    hw_classmark_T hw_classmark;
    (void) ghdi_read_hw_classmark(&hw_classmark);
  }

  // Configure EFS-NV for GERAN
  geran_nv_init();

  // Initialise multi-sim parameters for GERAN
  geran_multi_sim_init();

  // Initialise Message Router
  rr_main_msgr_init();

  // Initialise all other RR modules
  rr_init_task_start_init();

#ifdef FEATURE_SEGMENT_LOADING
  rr_initialise_seg_load();
#endif /*FEATURE_SEGMENT_LOADING*/

  /* Code in this function will add to startup latency. Do not use
     large loops that can be avoided.*/
}

#ifdef PERLUTF
#error code not present
#endif // PERLUTF

/* EOF */
