/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          L 2   M A I N

GENERAL DESCRIPTION
   This module contains the entry point for the L2 task. It
   calls initialisation functions for the tasks, then sits in an event
   loop.

EXTERNALIZED FUNCTIONS
  gsm_l2_main
    L2 'task' entry point.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2014 by QUALCOMM Technologies, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L2/vcs/l2_main.c_v   1.21   17 Jul 2002 16:09:00   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_main.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   ---       ---------------------------------------------------------
01/01/14   SJW       Integrate TRM client emulator
03/14/13   SJW       CR460027 integration with Dog Heartbeat API
09/28/11   SJW       Cancel pending access confirmations when restarting
08/08/11   SJW       Allow MDL_RELEASE_REQ to clean up after Radio Link Failure
05/26/11   SJW       Disable random padding in DSDS builds
29/10/10   SJW       Added support for Dual SIM
03/31/10   SJW       Use tmc_task_start() function instead of legacy task_start()
03/26/09   SJW       Removed redundant locks (including mutexes) around timer calls
07/31/08   SJW       Added support for KxMutex
04/11/03   HE        Increased Maximum L3 message length check from 250 to 251 octets
02/11/03   JAC       Added short header support
01/12/02   JAC       Added halfrate support
01/07/02   JAC       SACCH sending triggered by meas report from RR instead of SACCH decode
11/13/02   PLM       Renamed mc_task_start() to task_start()
11/01/02   JAC       Set establishment mode on resume to prevent piggybacking on sabm
10/17/02   JAC       Moved some funcs to l2_transmit.c,
                     changed l2_handle_timer_message to handle SACCH timeouts
10/16/02   JAC       Modified optional ack FACCH timeout for 8960
10/04/02   JAC       Code reorg
08/11/02   JAC       Added debug msgs
07/31/02   JAC       Added forced suspend for interrat handover
07/16/02   JAC       SAPI3 timer started on after a ph_ready_to_send_ind
07/09/02   JAC       Added an additional timeout value for maximum L1 overhead
06/10/02   JAC       Increased timeout value for FACCH
05/30/02   JAC       Added stop measurements and waiting for UA to be sent out
05/15/02   JAC       Added timer for optional ack
05/08/02   JAC       changed l2_main to gsm_l2_main
04/30/02   JAC       Added support for tc field in dl_unit_data_ind
04/12/02   JAC       Added watchdog support
02/28/02   JAC       Added clearing the timers when link is suspended
03/25/02   JAC       no longer waits for PH_DATA_IND or PH_READY_TO_SEND_IND to trigger
                     sending a message
                     removed extra q_put in l2_initialise_task
03/13/02   ATM       Added support for new GS_SEND_MESSAGE
02/20/02   ATM       Merge GS_GSM and GS functions together
02/26/02   JAC       L2 logging interface changes
02/10/02   JAC       Modified message queue length
01/25/02   JAC       Modified max message length of dl_data_req to MAX_OCTETS_L3_MESSAGE
01/08/01   JAC       Modified timeout values for SDCCH and FACCH
12/17/01   JAC       Made l3_message_to_send a static buffer instead of a pointer
12/14/01   JAC       Changed PH_DATA_IND handling to respond to new sys infos
                     Removed Perlutf feature for timers
                     Added initialization call for l2 data store
11/30/01   JC        Removed ZREX compile switch.
10/22/01   JAC       Added channel number assignment when receiving UI frame
                     Added unit test program
09/26/01   JC        Added dog.h include file.
09/26/01   JC        Added this header block and fixed the handling of
                     the command queue s.t. L2 now depends on the command
                     queue.  This is how L2 seems to have been designed.
                     The fix takes place by using the preprocessor to
                     define new_message structure.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "environ.h"
#include "stringl/stringl.h"
#include "geran_tasks.h"
#include "gmutex.h"
#include "ms.h"
#include "sys_type.h"
#include "sys_stru.h"
#include "sys_cnst.h"
#include "timers.h"
#include "gs.h"
#include "ms_timer.h"
#include "rr_l2_l1.h"
#include "rr_l2.h"
#include "l2_l1.h"
#include "l2i.h"
#include "l2_transfer.h"
#include "l2_msg.h"
#include "l2_send.h"
#include "l2_release.h"
#include "cfa.h"
#include "cmd.h"           /* for cmd_hdr_type */
#include "l2_log.h"
#include "l2_transmit.h"
#include "l2_timer.h"
#include "task.h"
#include "rcinit.h"
#include "rex.h"
#include "dog_hb_rex.h"
#ifdef FEATURE_GERAN_TRM_EMULATOR
#error code not present
#endif /* FEATURE_GERAN_TRM_EMULATOR */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

#define L2_CMD_Q_SIG      0x0001    /* A command is available in the queue. */
#define L2_RPT_TIMER_SIG  0x0002    /* Used to signal a watchdog report.    */

#define L2_RACH_CHANNEL_TYPE(ct) (((ct) == L2_RACH) || ((ct) == L2_RACH_WITH_NO_DELAY))
#define L2_MAIN_DCCH_CHANNEL_TYPE(ct) (((ct) != SACCH) && !L2_RACH_CHANNEL_TYPE(ct))

q_type                l2_cmd_q;

static dog_report_type l2_dog_handle = 0;      /* GL2 watchdog reference */

typedef union
{
  IMH_T                   message_header;

  dl_data_req_T           dl_data_req;
  dl_unit_data_req_T      dl_unit_data_req;
  dl_establish_req_T      dl_establish_req;
  dl_release_req_T        dl_release_req;
  dl_random_access_req_T  dl_random_access_req;
  dl_suspend_req_T        dl_suspend_req;
  dl_resume_req_T         dl_resume_req;
  dl_reconnect_req_T      dl_reconnect_req;

  ph_data_ind_T           ph_data_ind;
  ph_connect_ind_T        ph_connect_ind;
  ph_ready_to_send_ind_T  ph_ready_to_send_ind;
  ph_random_access_cnf_T  ph_random_access_cnf;

  mdl_release_req_T       mdl_release_req;

  timer_expiry_T          mid_timer_expiry;
} l2_msg_type;

typedef struct
{
  q_link_type   link;
  l2_msg_type  msg;
} l2_queue_msg_type;

/*****************************************************

 GLOBALS

******************************************************/
static l2_store_T l2_data[NUM_GERAN_DATA_SPACES];

/*****************************************************

                         FUNCTION DECLARATIONS

******************************************************/
static void l2_lib_read_message(l2_msg_type *new_message_ptr);

void gsm_l2_fn_init(void);

/*===========================================================================
FUNCTION l2_initialise

DESCRIPTION Initialise the L2 data spaces

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void l2_initialise(gas_id_t gas_id, boolean from_power_on)
{
  l2_store_T *l2_data_ptr;

  /************************************/
  /* Initialise the global data space */
  /************************************/
  l2_data_ptr = l2_get_store(gas_id);
  if (l2_data_ptr != NULL)
  {
    sapi_info_T *sapi0_ptr;
    sapi_info_T *sapi3_ptr;

    /* Store the GAS ID for use by logging and inter-task signalling routines */
    l2_data_ptr->gas_id = gas_id;

    /********************/
    /* Initialise SAPI0 */
    /********************/
    sapi0_ptr = l2_get_sapi_info(l2_data_ptr, SAPI0);
    if (sapi0_ptr != NULL)
    {
      sapi0_ptr->gas_id = gas_id;
      sapi0_ptr->sapi   = SAPI0;
      sapi0_ptr->T200   = SAPI0_T_200 + (byte)gas_id;
      sapi0_ptr->T_ack  = SAPI0_T_ACK + (byte)gas_id;

      l2_init_sapi(sapi0_ptr, from_power_on);
      l2_clear_sapi(l2_data_ptr, SAPI0, TRUE);
    }

    /********************/
    /* Initialise SAPI3 */
    /********************/
    sapi3_ptr = l2_get_sapi_info(l2_data_ptr, SAPI3);
    if (sapi3_ptr != NULL)
    {
      sapi3_ptr->gas_id = gas_id;
      sapi3_ptr->sapi   = SAPI3;
      sapi3_ptr->T200   = SAPI3_T_200 + (byte)gas_id;
      sapi3_ptr->T_ack  = SAPI3_T_ACK + (byte)gas_id;

      l2_init_sapi(sapi3_ptr, from_power_on);
      l2_clear_sapi(l2_data_ptr, SAPI3, TRUE);
    }

    if (from_power_on)
    {
      /************************************************/
      /* Initialise the pending random access request */
      /************************************************/
      l2_data_ptr->pending_ra_request.gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;

      /*********************************************/
      /* Initialise Pseudo Random Number Generator */
      /*********************************************/
      l2_data_ptr->xor_seed[0] = 88675123UL;
      l2_data_ptr->xor_seed[1] = 123456789UL;
      l2_data_ptr->xor_seed[2] = 362436069UL;
      l2_data_ptr->xor_seed[3] = 521288629UL;
      l2_data_ptr->random_pad  = TRUE;
    }

    /************************************/
    /* Initialise the outstanding flags */
    /************************************/
    memset(l2_data_ptr->l3_resume_message, 0, N201_MAX);
    l2_data_ptr->l3_resume_message_length = 0;

    l2_data_ptr->sacch_ready = FALSE;
    l2_data_ptr->dcch_ready  = FALSE;

    l2_data_ptr->suspend_pending_transmit = FALSE;
    l2_data_ptr->sacch_last_sapi_served   = SAPI0;
    l2_data_ptr->pseudo_sapi3_receive     = 0;
    l2_data_ptr->last_dcch_type           = SDCCH;
    l2_data_ptr->N200_dcch_type           = SDCCH_8;
    l2_data_ptr->sacch_repetition_order   = 0;
    l2_data_ptr->l1_has_candidate_buffer  = FALSE;

    memset(&(l2_data_ptr->sapi3_cache), 0, sizeof(ph_data_req_T));
    l2_data_ptr->sapi3_cache.l2_channel_type    = DCCH;
    l2_data_ptr->sapi3_cache.l2_repetition_info = L2_NOT_REPETITION_CANDIDATE;
    l2_data_ptr->sapi3_cache.gas_id             = gas_id;

    /* initialise the state machines */
    l2_SAPI0_control(l2_data_ptr, EV_RESET_L2);
    l2_SAPI3_control(l2_data_ptr, EV_RESET_L2);
  }

  return;
}

/*===========================================================================

FUNCTION  l2_handle_message

DESCRIPTION
  Thin layer that strips off QLINK and passes message to l2_lib_read_message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void l2_handle_message( l2_queue_msg_type* message )
{
  if (message != NULL)
  {
    if (&message->msg != NULL)
    {
      l2_lib_read_message(&message->msg);
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("Empty L2 message!");
  }
}

/*===========================================================================

FUNCTION l2_initialise_task_data

DESCRIPTION
  Initializes task/queue information

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 */

static void l2_initialise_task_data(void)
{

  (void) q_init(&l2_cmd_q);

  (void) gs_init(GS_QUEUE_LAYER_2,
                 rex_self(),
                 L2_CMD_Q_SIG,
                 &l2_cmd_q,
                 NULL,
                 sizeof(l2_queue_msg_type),
                 GS_DEFAULT_QUEUE_SIZE,
                 (gs_get_handler_ptr)l2_handle_message);
}

/*===========================================================================

FUNCTION  l2_get_store

DESCRIPTION
  Retrieves a pointer to a L2 global store pointer

DEPENDENCIES
  None

RETURN VALUE
  Pointer to global data space

SIDE EFFECTS
  None

===========================================================================*/
l2_store_T *l2_get_store(const gas_id_t gas_id)
{
  if (gas_id > GERAN_ACCESS_STRATUM_ID_MAX)
  {
    ERR_GERAN_FATAL_1("Unable to resolve GAS ID 0x%02X for Layer 2", (int)gas_id);
  }

  return(&(l2_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)]));
}

/*===========================================================================

FUNCTION  gsm_l2_main

DESCRIPTION
  This function is the entry point for the L2 task. It calls
  initialisation functions to perform task initialisation for the L2
  task, then sits in an event loop waiting to process messages on its
  command queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

/*lint -save -e715 */

void gsm_l2_main( dword unused )
{
  rex_sigs_type sigs;        /* This task's signals */

  geran_tcb_set(GERAN_TASK_ID_GL2, rex_self());

  /* Perform startup init here. Satisfy dependency of other tasks
     provided by this task. Do not linger here, startup latency is
     impacted. Higher priority tasks in the same group will preempt
     this task at this point. Do not rely upon priority alone as the
     sole mechanism to arbitrate resource contention with other tasks. */

  rcinit_handshake_startup(); /* required rcinit handshake */

  gsm_l2_fn_init();

  /* register with Dog Heartbeat API */
  l2_dog_handle = dog_hb_register_rex((rex_sigs_type)L2_RPT_TIMER_SIG);

  /* Perform remaining init here. This is sequenced by a start
     signal delivered at the end of the sequence group processing. */


#if 0 /* pattern for dependency on other task(s), not used by GL2 */
  if (RCINIT_NULL == rcinit_lookup("othertask"))
  {
   /* run in degraded state as othertask is not present in this
      image, any dependency from othertask will not be met */
  }
  else
  {
   /* Additional sequence can be performed between tasks using RCEVT
      mechanism. This provides a finer granularity of control at runtime
      than what is exposed during the build process. */

   rcevt_wait_name(OTHERTASK_RCEVT_SYNCA); /* optional event wait used as a sync */

   rcevt_signal_name(EXAMPLE_RCEVT_SYNCA); /* optional event signal used as a sync */
  }

  rcevt_signal_name(EXAMPLE_RCEVT_SYNCB); /* optional event signal used as a sync */
#endif


#ifdef FEATURE_GERAN_TRM_EMULATOR
  #error code not present
#endif /* FEATURE_GERAN_TRM_EMULATOR */

  for (;;)
  {
    sigs = rex_wait( L2_RPT_TIMER_SIG | L2_CMD_Q_SIG);

    if (sigs & L2_RPT_TIMER_SIG)
    {
      (void)rex_clr_sigs(rex_self(), L2_RPT_TIMER_SIG);
      dog_hb_report(l2_dog_handle);
    }

    if (sigs & L2_CMD_Q_SIG)
    {
      gs_read_messages(GS_QUEUE_LAYER_2);
    }
  }

  /* deregister with Dog Heartbeat API */
  dog_hb_deregister(l2_dog_handle);

  /*UNREACHED*/

  NOTUSED(unused); /* silence compiler warnings of unused parameter */
}

/* Normalise naming */
void gsm_l2_task (dword dummy)
{
   gsm_l2_main(dummy);
}

void gsm_l2_fn_init(void)
{
  int gas_index;
  /* Only use this mechanism to perform memory init, hardware init,
     or other startup init to satisfy dependency that follow. Avoid
     all HLOS mechanisms that will block. */

  l2_initialise_task_data();
  l2_timer_init();
  for (gas_index = 0; gas_index < NUM_GERAN_DATA_SPACES; gas_index++)
  {
    l2_initialise((gas_id_t)gas_index, TRUE);
  }

  /* Code in this function will add to startup latency. Do not use
     large loops that can be avoided.*/
}
/*lint -restore */


/*===========================================================================

FUNCTION  l2_process_mdl_release_req

DESCRIPTION
  Processes the MDL_RELEASE_REQ signal from RR

RETURN VALUE
  VOID

SIDE EFFECTS
  None

===========================================================================*/
static l2_store_T * l2_process_mdl_release_req(l2_msg_type *msg_ptr)
{
  l2_store_T *l2_data_ptr;

  if (msg_ptr != NULL)
  {
    gas_id_t msg_gas_id = msg_ptr->mdl_release_req.gas_id;

    l2_data_ptr = l2_get_store(msg_gas_id);
    if (l2_data_ptr != NULL)
    {
      sapi_info_T *sapi0_ptr;
      sapi_info_T *sapi3_ptr;

      sapi0_ptr = l2_get_sapi_info(l2_data_ptr, SAPI0);
      sapi3_ptr = l2_get_sapi_info(l2_data_ptr, SAPI3);

      /* free up any memory allocated to messages */
      l2_clear_send_queue(sapi0_ptr, DCCH);
      l2_clear_send_buffer(sapi3_ptr, DCCH);
      l2_clear_send_buffer(sapi3_ptr, SACCH);
      l2_initialise(msg_gas_id, FALSE);
      MSG_GERAN_ERROR_1("MDL_RELEASE_REQ resetting L2 on GAS ID 0x%02X", (int)msg_gas_id);

      /* reply to GRR with the same SAPI */
      l2_send_dl_release_ind(l2_data_ptr, msg_ptr->mdl_release_req.SAPI, LOCAL_END_RELEASE);
    }
  }
  else
  {
    l2_data_ptr = NULL;
  }

  return(l2_data_ptr);
}

/*===========================================================================

FUNCTION  l2_process_rach_request

DESCRIPTION
  Processes the DL_RANDOM_ACCESS_REQ signal from RR

RETURN VALUE
  VOID

SIDE EFFECTS
  None

===========================================================================*/
static l2_store_T * l2_process_rach_request(l2_msg_type *msg_ptr)
{
  l2_store_T *l2_data_ptr;

  if (msg_ptr != NULL)
  {
    dl_random_access_req_T *req_ptr = &(msg_ptr->dl_random_access_req);

    l2_data_ptr = l2_get_store(req_ptr->gas_id);

    if (l2_data_ptr != NULL)
    {
      dl_random_access_req_T *cache_ptr = &(l2_data_ptr->pending_ra_request);

      if (L2_RACH_CHANNEL_TYPE(req_ptr->l2_channel_type))
      {
        boolean repeat_ind;

        if (GERAN_ACCESS_STRATUM_ID_UNDEFINED == cache_ptr->gas_id)
        {
          /* cache is currently empty, no other requests are pending */
          repeat_ind = FALSE;
        }
        else
        {
          /* if the cache was already in use, it may indicate a RACH restart */
          /* and GL1 should already be handling a prior PH_RANDOM_ACCESS_IND */
          /* so the repeat flag will be set on this new PH_RANDOM_ACCESS_IND */
          repeat_ind = TRUE;
          MSG_GERAN_ERROR_1("gs%1d: RACH restarted, probable CS/PS collision", 1+(int)(l2_data_ptr->gas_id));
        }

        /* cache the new request from GRR, overwriting previous data */
        memscpy(cache_ptr, sizeof(dl_random_access_req_T),
                req_ptr, sizeof(dl_random_access_req_T));


        /* send PH_RANDOM_ACCESS_IND to Layer 1 including the repeat indication */
        l2_send_ph_random_access_ind(cache_ptr, repeat_ind);

        /* Layer 1 is expected to respond with PH_READY_TO_SEND_IND */
        /* which will cause the DL_RANDOM_ACCESS_REQ to be decached */
      }
      else
      {
        /* non-RACH channel type (e.g. DCCH) can be used in handover access */
        /* bursts, so just forward this RACH request to Layer 1 immediately */
        cache_ptr->gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
        l2_send_ph_random_access_req(req_ptr);
      }
    }
  }
  else
  {
    l2_data_ptr = NULL;
  }

  return(l2_data_ptr);
}

/*===========================================================================

FUNCTION  l2_handle_rr_message

DESCRIPTION
  Processes a message sent by RR

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static l2_store_T * l2_handle_RR_message(l2_msg_type* new_message_ptr)
{
  l2_store_T *l2_data_ptr;

  /*************************************************************************/
  /* All msgs from RR to L2 have a SAPI and channel type field.            */
  /* It sets the global variable current_SAPI and current_l2_channel_type, */
  /* They are used in the SAPI0, SAPI3 state machines to determine which   */
  /* channel types to operate on and also in the L2_send module            */
  /*************************************************************************/
  switch(new_message_ptr->message_header.message_id)
  {
    case DL_ESTABLISH_REQ:
      l2_data_ptr = l2_get_store(new_message_ptr->dl_establish_req.gas_id);

      l2_data_ptr->current_SAPI = new_message_ptr->dl_establish_req.SAPI;
      l2_data_ptr->current_l2_channel_type = new_message_ptr->dl_establish_req.l2_channel_type;

      /* establish mode is either contention resolution or normal establishment*/
      l2_data_ptr->establish_mode = new_message_ptr->dl_establish_req.establish_mode;

      if (l2_data_ptr->establish_mode == CONTENTION_RESOLUTION)
      {
        /* reseed the PRNG generator for initial establishment */
        l2_seed_random(l2_data_ptr);

        l2_data_ptr->l3_message_length = new_message_ptr->dl_establish_req.l3_message_length;

        if (l2_data_ptr->l3_message_length<=N201_MAX)
        {
          (void)memscpy(l2_data_ptr->l3_message_to_send,
                        sizeof(l2_data_ptr->l3_message_to_send),
                        new_message_ptr->dl_establish_req.layer3_message,
                        l2_data_ptr->l3_message_length);
        }
        else
        {
          MSG_GERAN_ERROR_0("Ignore DL_ESTABLISH_REQ, len too large");
          break;
        }
      }

      if (l2_data_ptr->current_SAPI == SAPI0)
      {
        l2_SAPI0_control(l2_data_ptr, EV_ESTABLISH_REQUEST);
      }
      else if (l2_data_ptr->current_SAPI == SAPI3)
      {
        l2_SAPI3_control(l2_data_ptr, EV_ESTABLISH_REQUEST);
      }
      else
      {
        /***********************************************/
        /* The SAPI other than SAPI0 and SAPI3 are not */
        /* defined yet (FFS). Raise a warning          */
        /***********************************************/
        MSG_GERAN_ERROR_1("SAPI%d undefined",l2_data_ptr->current_SAPI);
      }
      break;

    case DL_RELEASE_REQ:
      l2_data_ptr = l2_get_store(new_message_ptr->dl_release_req.gas_id);

      l2_data_ptr->current_SAPI = new_message_ptr->dl_release_req.SAPI;
      l2_data_ptr->current_l2_channel_type = new_message_ptr->dl_release_req.l2_channel_type;

      /* Release mode is either normal or local end */
      l2_data_ptr->release_mode = new_message_ptr->dl_release_req.release_mode;

      if (l2_data_ptr->current_SAPI == SAPI0)
      {
        l2_send_ph_stop_sacch_measurement_report(l2_data_ptr);
        l2_SAPI0_control(l2_data_ptr, EV_RELEASE_REQUEST);
      }
      else if (l2_data_ptr->current_SAPI == SAPI3)
      {
        l2_SAPI3_control(l2_data_ptr, EV_RELEASE_REQUEST);
      }
      else
      {
        /***********************************************/
        /* The SAPI other than SAPI0 and SAPI3 are not */
        /* defined yet (FFS). Raise a warning          */
        /***********************************************/
        MSG_GERAN_ERROR_1("SAPI%d undefined",l2_data_ptr->current_SAPI);
      }
      break;

    case DL_DATA_REQ:
      l2_data_ptr = l2_get_store(new_message_ptr->dl_data_req.gas_id);

      l2_data_ptr->current_SAPI = new_message_ptr->dl_data_req.SAPI;
      l2_data_ptr->current_l2_channel_type = new_message_ptr->dl_data_req.l2_channel_type;

      l2_data_ptr->l3_message_length = new_message_ptr->dl_data_req.l3_message_length;
      l2_data_ptr->l3_message_number = new_message_ptr->dl_data_req.l3_message_number;

      if (l2_data_ptr->l3_message_length <= MAX_OCTETS_L3_MESSAGE)
      {
        (void)memscpy(l2_data_ptr->l3_message_to_send,
                      sizeof(l2_data_ptr->l3_message_to_send),
                      new_message_ptr->dl_data_req.layer3_message,
                      l2_data_ptr->l3_message_length);
      }
      else
      {
        MSG_GERAN_ERROR_0("Ignore DL_DATA_REQ, len too large");
        break;
      }

      if (l2_data_ptr->current_SAPI == SAPI0)
      {
        l2_SAPI0_control(l2_data_ptr, EV_DATA_REQUEST);
      }
      else if (l2_data_ptr->current_SAPI == SAPI3)
      {
        l2_SAPI3_control(l2_data_ptr, EV_DATA_REQUEST);
      }
      else
      {
        /***********************************************/
        /* The SAPI other than SAPI0 and SAPI3 are not */
        /* defined yet (FFS). Raise a warning          */
        /***********************************************/
        MSG_GERAN_ERROR_1("SAPI%d undefined",l2_data_ptr->current_SAPI);
      }
      break;

    case DL_UNIT_DATA_REQ:
      l2_data_ptr = l2_get_store(new_message_ptr->dl_unit_data_req.gas_id);

      l2_data_ptr->current_SAPI = new_message_ptr->dl_unit_data_req.SAPI;
      l2_data_ptr->current_l2_channel_type = new_message_ptr->dl_unit_data_req.l2_channel_type;

      if (new_message_ptr->dl_unit_data_req.l3_message_length > N201_MAX)
      {
        MSG_GERAN_ERROR_0("Ignore DL_UNIT_DATA_REQ, len too large");
        break;
      }

      if ((l2_data_ptr->current_SAPI == SAPI0) && (l2_data_ptr->current_l2_channel_type == SACCH))
      {
        channel_info_T *channel_info;
        sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, SAPI0);

        channel_info = l2_get_channel_info(sapi_info, SACCH);
        l2_code_ui(sapi_info,
                   channel_info,
                   new_message_ptr->dl_unit_data_req.short_header,
                   new_message_ptr->dl_unit_data_req.layer3_message,
                   new_message_ptr->dl_unit_data_req.l3_message_length);

        /**************************************************************/
        /* All DL_UNIT_DATA_REQ messages are measurement reports      */
        /* L1 will provide RR with measurements results 13 frames     */
        /* before it can send, when we receive the meas report        */
        /* we can use this to trigger sending on the SACCH            */
        /**************************************************************/
        l2_send_SACCH(l2_data_ptr);
      }
      else
      {
        /***********************************************/
        /* The Unacknowledge Mode Transfer is not de-  */
        /* fined for SAPI others than SAPI 0. Moreover */
        /* I don't see any case where it is used on    */
        /* DCCH in the uplink direction : Raise a      */
        /* warning                                     */
        /***********************************************/
        MSG_GERAN_ERROR_0("Unack transfer not allowed");
      }
      break;

    case DL_SUSPEND_REQ:
      l2_data_ptr = l2_get_store(new_message_ptr->dl_suspend_req.gas_id);

      l2_data_ptr->current_SAPI             = new_message_ptr->dl_suspend_req.SAPI;
      l2_data_ptr->current_l2_channel_type  = new_message_ptr->dl_suspend_req.l2_channel_type;
      l2_data_ptr->suspend_pending_transmit = FALSE;

      if (l2_data_ptr->current_SAPI == SAPI0)
      {
        if (new_message_ptr->dl_suspend_req.forced_suspend == FALSE)
        {
          channel_info_T *channel_info_ptr;
          sapi_info_T    *sapi_info_ptr;
          boolean         valid_data;

          sapi_info_ptr    = l2_get_sapi_info(l2_data_ptr, SAPI0);
          channel_info_ptr = l2_get_channel_info(sapi_info_ptr, DCCH);

          if (channel_info_ptr != NULL)
          {
            valid_data = channel_info_ptr->transmit_buffer.valid_data_flag;
          }
          else
          {
            valid_data = FALSE;
          }

          /* check whether any frame in the transmit buffer can be serviced */
          if ((valid_data == TRUE) && (l2_data_ptr->dcch_ready == TRUE))
          {
            /* allow transmission of pending frame in the buffer  */
            /* by simulating the expiry of the optional ack timer */
            l2_clear_T_ack_timer(sapi_info_ptr);
            l2_SAPI0_control(l2_data_ptr, EV_OPTIONAL_SEND);

            /* set a flag that will reprocess the suspension once L1 has TXd */
            l2_data_ptr->suspend_pending_transmit = TRUE;
          }
          else
          {
            /* Normal suspension */
            l2_SAPI0_control(l2_data_ptr, EV_SUSPEND_REQUEST);
          }
        }
        else
        {
          /* Forced suspend, suspend even if we are in idle */
          MSG_GERAN_HIGH_0("Forced suspend");
          l2_SAPI0_control(l2_data_ptr, EV_FORCED_SUSPEND_REQ);

          /* reseed the PRNG generator before establishment after inter-RAT */
          l2_seed_random(l2_data_ptr);
        }
      }
      else
      {
        /***********************************************/
        /* The suspension for SAPI other than SAPI0 is */
        /* not defined : raise a warning               */
        /***********************************************/
        MSG_GERAN_ERROR_0("Suspension not allowed for SAPI");
      }
      break;

    case DL_RESUME_REQ:
      l2_data_ptr = l2_get_store(new_message_ptr->dl_resume_req.gas_id);

      /* sanity check the incoming request parameters */
      if ((new_message_ptr->dl_resume_req.SAPI != SAPI0) ||
          (new_message_ptr->dl_resume_req.l3_message_length > N201_MAX))
      {
        /***********************************************/
        /* Suspend/resume for SAPI other than SAPI0 is */
        /* not defined : print a warning and drop out  */
        /***********************************************/
        MSG_GERAN_ERROR_0("Invalid length or SAPI in DL-RESUME-REQ");
        break;
      }

      /* set up global variables: note that SABM will have no payload */
      l2_data_ptr->current_l2_channel_type = DCCH;
      l2_data_ptr->current_SAPI            = SAPI0;
      l2_data_ptr->establish_mode          = NORMAL_ESTABLISHMENT;

      l2_data_ptr->l3_message_length = new_message_ptr->dl_resume_req.l3_message_length;
      (void)memscpy(l2_data_ptr->l3_message_to_send,
                    sizeof(l2_data_ptr->l3_message_to_send),
                    new_message_ptr->dl_resume_req.layer3_message,
                    l2_data_ptr->l3_message_length);

#ifdef FEATURE_GSM_DTM
      if (new_message_ptr->dl_resume_req.l1_already_connected)
      {
        /* RR is resuming the link but L1 is connected (e.g. DTM or GAN) */
        l2_handle_ph_ready_to_send_ind(l2_data_ptr, SACCH);
        l2_handle_ph_ready_to_send_ind(l2_data_ptr, l2_data_ptr->last_dcch_type);
        MSG_GERAN_HIGH_0("L2: resuming link, L1 already connected");
      }
#endif /* FEATURE_GSM_DTM */

      MSG_GERAN_HIGH_3("L2: channel_type=%d forced_resume=%d last_dcch=%d",
               (int)new_message_ptr->dl_resume_req.l2_channel_type,
               (int)new_message_ptr->dl_resume_req.forced_resume,
               (int)l2_data_ptr->last_dcch_type);

      /* use the resumption request to set the N200 channel type */
      l2_update_N200_channel_type(l2_data_ptr, new_message_ptr->dl_resume_req.l2_channel_type);

      l2_SAPI0_control(l2_data_ptr, EV_RESUME_REQUEST);
      if (new_message_ptr->dl_resume_req.forced_resume == TRUE)
      {
        /* this is a forced resumption after GSM being inactive */
        l2_SAPI3_control(l2_data_ptr, EV_CONNECT_RECEIVED);
      }
      break;

    case DL_RECONNECT_REQ:
      l2_data_ptr = l2_get_store(new_message_ptr->dl_reconnect_req.gas_id);

      l2_data_ptr->current_SAPI = new_message_ptr->dl_reconnect_req.SAPI;
      l2_data_ptr->current_l2_channel_type = new_message_ptr->dl_reconnect_req.l2_channel_type;

      l2_data_ptr->l3_message_length = new_message_ptr->dl_reconnect_req.l3_message_length;

      if (l2_data_ptr->l3_message_length <= N201_MAX)
      {
        (void)memscpy(l2_data_ptr->l3_message_to_send,
                      sizeof(l2_data_ptr->l3_message_to_send),
                      new_message_ptr->dl_reconnect_req.layer3_message,
                      l2_data_ptr->l3_message_length);
      }
      else
      {
        MSG_GERAN_ERROR_0("Ignore DL_RECONNECT_REQ message, length too large");
        break;
      }

      if (l2_data_ptr->current_SAPI == SAPI0)
      {
        l2_SAPI0_control(l2_data_ptr, EV_RECONNECT_REQUEST);
      }
      else
      {
        /***********************************************/
        /* The suspension for SAPI other than SAPI0 is */
        /* not defined : raise a warning               */
        /***********************************************/
        MSG_GERAN_ERROR_0("Reconnection not allowed for SAPI");
      }
      break;

    case DL_RANDOM_ACCESS_REQ:
      l2_data_ptr = l2_process_rach_request(new_message_ptr);
      break;

    case MDL_RELEASE_REQ:
      /***************************************************************/
      /* Handled the same in every state                             */
      /* something at the physical layer went wrong, or RR/L2 is in  */
      /* an unexpected state need to reset everything to be safe     */
      /***************************************************************/
      l2_data_ptr = l2_process_mdl_release_req(new_message_ptr);
      break;

    case DL_DATA_MUX_IND:
      l2_data_ptr = l2_install_demux_callbacks((dl_data_mux_ind_T *)new_message_ptr);
      break;

    default:
      l2_data_ptr = NULL;
      MSG_GERAN_ERROR_1("Unexpected msg %d from RR",new_message_ptr->message_header.message_id);
      break;
  }

  return(l2_data_ptr);
}


/*===========================================================================

FUNCTION  l2_handle_ph_data_ind

DESCRIPTION
  Processes a ph_data_ind message sent by L1

RETURN VALUE
  Pointer to L2 data space context

SIDE EFFECTS
  None

===========================================================================*/
static l2_store_T *l2_handle_ph_data_ind(l2_msg_type *new_message_ptr)
{
  ph_data_ind_T *ph_data_ind_ptr;
  l2_store_T    *l2_data_ptr;
  ARFCN_T        channel_number;
  boolean        own_paging_block;
  word           message_length;

  ph_data_ind_ptr = &(new_message_ptr->ph_data_ind);
  l2_data_ptr     = l2_get_store(ph_data_ind_ptr->gas_id);
  l2_data_ptr->current_l2_channel_type = ph_data_ind_ptr->l2_channel_type;
  l2_data_ptr->last_channel_number = ph_data_ind_ptr->channel_number;

  if (FALSE == ph_data_ind_ptr->dual_sim_mode_enabled)
  {
    /* single standby mode is in effect, use random padding for GCF compliance */
    l2_data_ptr->random_pad = TRUE;
  }
  else
  {
    /* DSDS is enabled or random padding feature is disabled */
    l2_data_ptr->random_pad = FALSE;
  }

  GET_IMH_LEN(message_length, &new_message_ptr->message_header);

  if ((l2_data_ptr->current_l2_channel_type == CCCH) ||
      (l2_data_ptr->current_l2_channel_type == BCCH))
  {
    /********************************************************/
    /* Check if message contains any layer 2 frame info. If */
    /* L1 could not decode a frame it sets the length to    */
    /* indicate no l2 frame info.We can drop those frames   */
    /********************************************************/
    if (message_length > sizeof(ph_data_ind_T) - sizeof(IMH_T) - N201_MAX)
    {
      /*******************************************************/
      /* Length is valid. The format of the frame is Bbis    */
      /* since it is from the BCCH or CCCH                   */
      /*******************************************************/
      own_paging_block = ph_data_ind_ptr->own_paging_block;
      channel_number   = ph_data_ind_ptr->channel_number;

      /*
       * If the pseudo length is less than 6, its
       * probably (!) ignorable, unless we're on our own
       * paging block, in which case we need the null message
       * for DSC evaluation. Also, let any Sys Info messages
       * through, as SI 7, 8, 2quater, 9, 13, 16, 17
       * 18, 19, 20 have pseudo lengths of 1
       */
      if (((ph_data_ind_ptr->layer2_frame[0] )
           >= (6<<2)) ||
          ((ph_data_ind_ptr->layer2_frame[2]&0xf8)
           == 0x18) ||
          ((ph_data_ind_ptr->layer2_frame[2]&0xf8)
           == 0x00) ||
          ((ph_data_ind_ptr->layer2_frame[2]&0xFC)
           == 0x3C) ||
          ((ph_data_ind_ptr->layer2_frame[2]&0xFC)
           == 0x40) ||
          (own_paging_block))
      {
        l2_data_ptr->l2_segment_length = N201_CCCH;

        /***********************************************************/
        /* Save the segment received, to be used when sending back */
        /* the msg to RR                                           */
        /***********************************************************/
        l2_data_ptr->received_l2_segment = ph_data_ind_ptr->layer2_frame;
        l2_send_dl_unit_data_ind(l2_data_ptr, own_paging_block, channel_number,
                                 ph_data_ind_ptr->tc, FALSE);
      }
      else
      {
        /***********************************************************/
        /* The msg didn't not fit our filter, print out error msgs */
        /***********************************************************/
        if (l2_data_ptr->current_l2_channel_type == BCCH)
        {
          MSG_GERAN_HIGH_3("Filter msg, len=%d chan=%d, tc=%d",
              (int) message_length,
              (int) channel_number.num,
              (int) ph_data_ind_ptr->tc);

          MSG_GERAN_HIGH_3("MSG-012[%02X,%02X,%02X]",
                   (int) ph_data_ind_ptr->layer2_frame[0],
                   (int) ph_data_ind_ptr->layer2_frame[1],
                   (int) ph_data_ind_ptr->layer2_frame[2]);

          MSG_GERAN_HIGH_3("MSG-345[%02X,%02X,%02X]",
                   (int) ph_data_ind_ptr->layer2_frame[3],
                   (int) ph_data_ind_ptr->layer2_frame[4],
                   (int) ph_data_ind_ptr->layer2_frame[5]);

          MSG_GERAN_HIGH_3("MSG-678[%02X,%02X,%02X]",
                   (int) ph_data_ind_ptr->layer2_frame[6],
                   (int) ph_data_ind_ptr->layer2_frame[7],
                   (int) ph_data_ind_ptr->layer2_frame[8]);
        }
      }
    }
  }
  else
  {
    /***********************************************************/
    /* L2 does not care if msg was received on SDCCH or FACCH. */
    /* Just sets it to DCCH                                    */
    /***********************************************************/
    if (l2_data_ptr->current_l2_channel_type != SACCH)
    {
      l2_data_ptr->current_l2_channel_type = DCCH;
    }

    /********************************************************/
    /* Check if message contains any layer 2 frame info. If */
    /* L1 could not decode a frame it sets the length to    */
    /* indicate no l2 frame info.We can drop those frames   */
    /********************************************************/
    if (message_length > sizeof(ph_data_ind_T) - sizeof(IMH_T) - N201_MAX)
    {
      frame_type_T frame_type;
      boolean      frame_valid;
      byte         this_SAPI;

      /****************************************************/
      /* Frame has a large enough length, try decoding it */
      /****************************************************/
      l2_decode_frame(l2_data_ptr,
                      ph_data_ind_ptr->layer2_frame,
                      &frame_type,
                      &this_SAPI,
                      &frame_valid,
                      ph_data_ind_ptr->cbch_frame);

      if (l2_data_ptr->current_l2_channel_type == SACCH)
      {
        /* Pass the SACCH Repetition Order through to the utility module */
        /* Note that this is irrespective of SAPI or frame validity      */
        l2_set_sacch_repetition_order(l2_data_ptr, ph_data_ind_ptr->l2_repetition_order);
      }

      /*
       * for SI messages, this_SAPI will be 0 while current
       * SAPI will be 3 for SMS connections
       */
      if ((this_SAPI == 0) && (frame_valid == TRUE))
      {
        l2_data_ptr->current_SAPI = SAPI0;

        if (frame_type == UA)
        {
          MSG_GERAN_MED_0("Received a UA frame");
          l2_SAPI0_control(l2_data_ptr, EV_UA_RECEIVED);
        }
        else if (frame_type == DISC)
        {
          MSG_GERAN_LOW_0("Receive a DISC frame");
          l2_SAPI0_control(l2_data_ptr, EV_DISC_RECEIVED);
        }
        else if (frame_type == DM)
        {
          MSG_GERAN_MED_3("Rcvd DM frame %X %X %X",
                  (int) ph_data_ind_ptr->layer2_frame[0],
                  (int) ph_data_ind_ptr->layer2_frame[1],
                  (int) ph_data_ind_ptr->layer2_frame[2]);
          l2_SAPI0_control(l2_data_ptr, EV_DM_RECEIVED);
        }
        else if (frame_type == UI)
        {
          MSG_GERAN_LOW_0("Receive a UI frame");
          own_paging_block = ph_data_ind_ptr->own_paging_block;
          channel_number= ph_data_ind_ptr->channel_number;

          /* Layer2 messages received over SACCH use layer2 frame format B4
             which do not have a length indicator field. As result, first octet
             of layer3 message is the third octet in a layer2 B4 frame. First
             octet of a non-std layer3 msg contains pseudo length which is
             equal to full length of the msg (excluding pseudo len octet itself)
             if rest octets are not included. If rest octets are included,
             length is N201_SACCH (18 octets, excluding pseudo len octet
             itself).

             Previously, length was always set to pseudo length and is
             incorrect if messages contain rest octets, eg. SI6
          */

          if(l2_data_ptr->current_l2_channel_type == SACCH)
          {
            l2_data_ptr->l2_segment_length = N201_SACCH;
          }
          l2_send_dl_unit_data_ind(l2_data_ptr,own_paging_block,channel_number,ph_data_ind_ptr->tc,FALSE);
        }
        else if (frame_type == UI_SHORT_HEADER)
        {
          MSG_GERAN_MED_0("Receive a short header frame");
          own_paging_block = ph_data_ind_ptr->own_paging_block;
          channel_number= ph_data_ind_ptr->channel_number;
          l2_send_dl_unit_data_ind(l2_data_ptr,own_paging_block,channel_number,ph_data_ind_ptr->tc,TRUE);
        }
        else if (frame_type == I)
        {
          MSG_GERAN_LOW_0("Receive a I frame");
          l2_SAPI0_control(l2_data_ptr, EV_I_RECEIVED);
        }
        else if (frame_type == RR)
        {
          MSG_GERAN_LOW_0("Receive a RR frame");
          l2_SAPI0_control(l2_data_ptr, EV_RR_RECEIVED);
        }
        else if (frame_type == REJ)
        {
          MSG_GERAN_LOW_0("Receive a REJ frame");
          l2_SAPI0_control(l2_data_ptr, EV_REJ_RECEIVED);
        }
        else if (frame_type == SABM)
        {
          MSG_GERAN_LOW_0("Receive a SABM frame");
          l2_SAPI0_control(l2_data_ptr, EV_SABM_RECEIVED);
        }
        else
        {
          MSG_GERAN_ERROR_1("Invalid frame type %d SAPI0",(int)frame_type);
        }
      }
      else if ((this_SAPI == 3) &&
               (frame_valid == TRUE))
      {
        l2_data_ptr->current_SAPI = SAPI3;

        if (frame_type == SABM)
        {
          MSG_GERAN_LOW_0("Receive a SABM frame");
          l2_SAPI3_control(l2_data_ptr, EV_SABM_RECEIVED);
        }
        else if (frame_type == UA)
        {
          MSG_GERAN_LOW_0("Receive a UA frame");
          l2_SAPI3_control(l2_data_ptr, EV_UA_RECEIVED);
        }
        else if (frame_type == DISC)
        {
          MSG_GERAN_LOW_0("Receive a DISC frame");
          l2_SAPI3_control(l2_data_ptr, EV_DISC_RECEIVED);
        }
        else if (frame_type == DM)
        {
          MSG_GERAN_LOW_0("Receive a DM frame");
          l2_SAPI3_control(l2_data_ptr, EV_DM_RECEIVED);
        }
        else if (frame_type == UI)
        {
          /*******************************************/
          /* The Unacknowledge Mode Transfer is not  */
          /* defined for SAPI 3. Raise a warning     */
          /*******************************************/
          MSG_GERAN_ERROR_0("UI frame on SAPI3");
        }
        else if (frame_type == I)
        {
          MSG_GERAN_LOW_0("Receive a I frame");
          l2_SAPI3_control(l2_data_ptr, EV_I_RECEIVED);
        }
        else if (frame_type == RR)
        {
          MSG_GERAN_LOW_0("Receive a RR frame");
          l2_SAPI3_control(l2_data_ptr, EV_RR_RECEIVED);
        }
        else if (frame_type == REJ)
        {
          MSG_GERAN_LOW_0("Receive a REJ frame");
          l2_SAPI3_control(l2_data_ptr, EV_REJ_RECEIVED);
        }
        else
        {
          MSG_GERAN_ERROR_1("Invalid frame type %d SAPI3",(int)frame_type);
        }
      }
    } /* Message length ok */

    else if ( ph_data_ind_ptr->cbch_frame )
    {
       l2_send_dl_cb_block_ind(l2_data_ptr, NULL, ph_data_ind_ptr->cbch_frame);
    }

    else
    {
      /* L1 has supplied the "magic length" to indicate an empty message - ignore the frame */
      /* Nothing much to log here, frame octets are just garbage, but channel types are OK  */
      if (l2_data_ptr->current_l2_channel_type == SACCH)
      {
        l2_crc_sacch_failure(l2_data_ptr);
      }
      else
      {
        MSG_GERAN_HIGH_2("L2: empty frame from L1 (channel %1X->%1X)",
                  ph_data_ind_ptr->l2_channel_type,l2_data_ptr->current_l2_channel_type);
      }
    }

    /**********************************************************************/
    /* We received a ph_data_ind message on the DCCH or SACCH             */
    /* check if we need to send anything out                              */
    /**********************************************************************/
    l2_check_transmit_on_receive(l2_data_ptr);
  }

  return(l2_data_ptr);
}

/*===========================================================================

FUNCTION  l2_handle_ph_connect_ind

DESCRIPTION
  Processes the PH_CONNECT_IND primitive from GERAN L1

RETURN VALUE
  Pointer to current data space context

SIDE EFFECTS
  None

===========================================================================*/
static l2_store_T *l2_handle_ph_connect_ind(l2_msg_type *new_message_ptr)
{
  channel_type_T l1_channel;
  l2_store_T    *l2_data_ptr;
  gas_id_t       gas_id;

  /* determine the correct data space using the GAS ID */
  gas_id      = new_message_ptr->ph_connect_ind.gas_id;
  l2_data_ptr = l2_get_store(gas_id);
  l1_channel  = new_message_ptr->ph_connect_ind.connected_channel;

  /* reset the L2 channel type */
  l2_data_ptr->current_l2_channel_type = DCCH;

  /* reset the "suspension pending" flag at the start of every connection */
  l2_data_ptr->suspend_pending_transmit = FALSE;

  /* move the SAPI0 & SAPI3 state machines into their connected states */
  l2_data_ptr->current_SAPI = SAPI0;
  l2_SAPI0_control(l2_data_ptr, EV_CONNECT_RECEIVED);

  l2_data_ptr->current_SAPI = SAPI3;
  l2_SAPI3_control(l2_data_ptr, EV_CONNECT_RECEIVED);

  /*
   * Having received PH_CONNECT_IND, we can assume that
   * Layer 1 is open for business. If the channel type
   * is SDCCH, then we can send messages for SDCCH or SACCH,
   * otherwise we can send messages for FACCH or SACCH
   *
   * This assumption avoids L1 having to send 2 separate
   * PH_READY_TO_SEND_IND messages
   */
  l2_data_ptr->current_SAPI = SAPI0;

  if ((l1_channel == SDCCH_8) ||
      (l1_channel == SDCCH_4))
  {
    l2_handle_ph_ready_to_send_ind(l2_data_ptr, SDCCH);
  }
  else if (l1_channel == HALF_RATE_TRAFFIC)
  {
    l2_handle_ph_ready_to_send_ind(l2_data_ptr, FACCH_H);

    MSG_GERAN_HIGH_0_G("L2 connected: early TCH/H assignment");
  }
  else
  {
    l2_handle_ph_ready_to_send_ind(l2_data_ptr, FACCH_F);

    MSG_GERAN_HIGH_0_G("L2 connected: early TCH/F assignment");
  }

  l2_handle_ph_ready_to_send_ind(l2_data_ptr, SACCH);

  if (l2_data_ptr->random_pad)
  {
    MSG_GERAN_HIGH_0_G("L2 connected: LAPDm filler octets will be randomised");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("L2 connected: LAPDm filler octets using 0x2B");
  }

  return(l2_data_ptr);
}

/*===========================================================================

FUNCTION  l2_handle_L1_message

DESCRIPTION
  Processes a message sent by L1

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static l2_store_T * l2_handle_L1_message(l2_msg_type *new_message_ptr)
{
  l2_store_T *l2_data_ptr;

  switch(new_message_ptr->message_header.message_id)
  {
    case PH_READY_TO_SEND_IND:
      l2_data_ptr = l2_get_store(new_message_ptr->ph_ready_to_send_ind.gas_id);
      if (l2_data_ptr != NULL)
      {
        l2_channel_type_T channel_type = new_message_ptr->ph_ready_to_send_ind.l2_channel_type;

        if (L2_MAIN_DCCH_CHANNEL_TYPE(channel_type) &&
            l2_data_ptr->suspend_pending_transmit)
        {
          /* L1 has transmitted on the main DCCH, and flag is set that */
          /* means SAPI0 suspension is required once this has occurred */
          l2_data_ptr->suspend_pending_transmit = FALSE;
          l2_SAPI0_control(l2_data_ptr, EV_SUSPEND_REQUEST);
        }
        else
        {
          l2_handle_ph_ready_to_send_ind(l2_data_ptr, channel_type);
        }
      }
      break;

    case PH_DATA_IND:
      l2_data_ptr = l2_handle_ph_data_ind(new_message_ptr);
      break;

    case PH_RANDOM_ACCESS_CNF:
      /* forward the confirmation signal to GRR immediately */
      l2_data_ptr = l2_send_dl_random_access_cnf(&(new_message_ptr->ph_random_access_cnf));
      if (l2_data_ptr != NULL)
      {
        /* clear the indicators that Layer 1 is ready for transmission
         * as we may be going into a new mode, to avoid confusion
         * between SDCCH and FACCH (e.g. after handover access bursts)
         */
        l2_data_ptr->sacch_ready = FALSE;
        l2_data_ptr->dcch_ready  = FALSE;
      }
      break;

    case PH_CONNECT_IND:
      l2_data_ptr = l2_handle_ph_connect_ind(new_message_ptr);
      break;

    default:
      l2_data_ptr = NULL;
      MSG_GERAN_ERROR_1("Unknown L1_L2 msg id%d",new_message_ptr->message_header.message_id);
      break;
  }

  return(l2_data_ptr);
}

/*===========================================================================

FUNCTION  l2_handle_timer_message

DESCRIPTION
  Processes a timer message

RETURN VALUE
  Pointer to L2 data space context for which the message was intended

SIDE EFFECTS
  None

===========================================================================*/
static l2_store_T * l2_handle_timer_message(l2_msg_type *new_message_ptr)
{
  l2_store_T *l2_data_ptr;

  if (new_message_ptr->message_header.message_id == MID_TIMER_EXPIRY)
  {
    sapi_info_T       *sapi_info_ptr;
    SAPI_T             sapi            = SAPI0;
    l2_channel_type_T  timer_channel   = DCCH;
    byte               sequence_number = 0;

#ifdef FEATURE_GERAN_TRM_EMULATOR
    #error code not present
#endif /* FEATURE_GERAN_TRM_EMULATOR */

    /* the expiry message carries an index into the timer pool */
    /* so that the correct set of parameters can be identified */
    l2_data_ptr = l2_timer_get_params(new_message_ptr->mid_timer_expiry.data,
                                      &sapi, &timer_channel, &sequence_number);

    sapi_info_ptr = l2_get_sapi_info(l2_data_ptr, sapi);

    if ((l2_data_ptr == NULL) || (sapi_info_ptr == NULL))
    {
      MSG_GERAN_ERROR_0("Unable to identify timer params from expiry message");
      return(NULL);
    }

    /* free up the parameter entry in the timer pool */
    l2_timer_clr_params(new_message_ptr->mid_timer_expiry.data);

    if (IS_T200_TIMER_ID(new_message_ptr->mid_timer_expiry.timer_id))
    {
      MSG_GERAN_MED_1("T200 expired for SAPI%d",(int)sapi);

      /* clear timer information */
      gs_clear_timer(GS_QUEUE_LAYER_2, new_message_ptr->mid_timer_expiry.timer_id);

      if (sapi_info_ptr->T200_state == RUNNING)
      {
        l2_data_ptr->current_SAPI = sapi;
        l2_data_ptr->current_l2_channel_type = timer_channel;

        if (sapi_info_ptr->T200_channel_type != timer_channel)
        {
          MSG_GERAN_ERROR_0("Channel type changed");
        }

        if (sapi == SAPI0)
        {
          l2_SAPI0_control(l2_data_ptr, EV_T200_TIMEOUT);
        }
        else if (sapi == SAPI3)
        {
          l2_SAPI3_control(l2_data_ptr, EV_T200_TIMEOUT);
        }

        l2_log_timeout(l2_data_ptr,
                       l2_data_ptr->sapi0.DCCH_channel.outstanding_i_frame,
                       l2_data_ptr->sapi3.DCCH_channel.outstanding_i_frame,
                       l2_data_ptr->sapi0.SACCH_channel.outstanding_i_frame);
      }
    }
    else if (IS_TACK_TIMER_ID(new_message_ptr->mid_timer_expiry.timer_id))
    {
      MSG_GERAN_HIGH_1("Optional send timeout for SAPI%d",(int)sapi);

      /* clear timer information */
      gs_clear_timer(GS_QUEUE_LAYER_2, new_message_ptr->mid_timer_expiry.timer_id);

      if (sequence_number == sapi_info_ptr->T_ack_seq_number)
      {
        /* the optional send timer is only ever started for DCCH channels */
        l2_data_ptr->current_l2_channel_type = DCCH;
        l2_data_ptr->current_SAPI            = sapi;
        sapi_info_ptr->T_ack_state           = STOPPED;
        if (l2_data_ptr->dcch_ready == TRUE)
        {
          channel_info_T *channel_info = l2_get_channel_info(sapi_info_ptr, DCCH);

          if (channel_info->transmit_buffer.valid_data_flag == TRUE)
          {
            if (sapi == SAPI0)
            {
              l2_SAPI0_control(l2_data_ptr, EV_OPTIONAL_SEND);
            }
            else if (sapi == SAPI3)
            {
              l2_SAPI3_control(l2_data_ptr, EV_OPTIONAL_SEND);
            }
          }
          else
          {
            MSG_GERAN_ERROR_0("Optional send timeout: DCCH buffer empty!");
          }
        }
      }
      else
      {
        MSG_GERAN_ERROR_0("Optional send timeout: sequence mismatch");
      }
    }
    else
    {
      MSG_GERAN_ERROR_1("Unknown timer %d", new_message_ptr->mid_timer_expiry.timer_id);
    }
  }
  else
  {
    l2_data_ptr = NULL;
  }

  return(l2_data_ptr);
}

/*===========================================================================

FUNCTION  l2_lib_read_message

DESCRIPTION
  Processes a single message from the queue

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void l2_lib_read_message(l2_msg_type *new_message_ptr)
{
  l2_store_T *l2_data_ptr;

  if (new_message_ptr->message_header.message_set == MS_RR_L2)
  {
    l2_data_ptr = l2_handle_RR_message(new_message_ptr);
  }
  else if (new_message_ptr->message_header.message_set == MS_L2_L1)
  {
    l2_data_ptr = l2_handle_L1_message(new_message_ptr);
  }
  else if (new_message_ptr->message_header.message_set == MS_TIMER)
  {
    l2_data_ptr = l2_handle_timer_message(new_message_ptr);
  }
  else
  {
    l2_data_ptr = NULL;
    MSG_GERAN_ERROR_2("Unknown message(%d,%d)",new_message_ptr->message_header.message_set,new_message_ptr->message_header.message_id);
  }

  if (l2_data_ptr != NULL)
  {
    l2_send_DCCH(l2_data_ptr);
  }

  return;
} /* main */


