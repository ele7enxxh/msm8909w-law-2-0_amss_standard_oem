/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            GRR MULTI SIM CONTROL MODULE

GENERAL DESCRIPTION
  Provides Multi SIM support for GERAN Radio Resources (GRR)

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2010-2014 Qualcomm Technologies, Incorporated. All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_multi_sim.c#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
31/05/14    SJW     Refactored for latest Multi-SIM/Multi-Standby support
===========================================================================*/
#include "geran_variation.h"
#include "rr_multi_sim.h"
#include "rr_general.h"

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
#include "rr_nv.h"
#include "rr_init.h"
#include "rr_rr.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_l2.h"
#include "rr_l2_g.h"
#include "rr_if.h"
#include "rr_l1_send.h"
#include "rr_l2_send.h"
#include "rr_mm_send.h"
#include "rr_grr.h"
#include "rr_control.h"
#include "rr_pscan.h"
#include "rr_mac_send.h"
#include "rlc_grr.h"
#include "rr_mode.h"
#include "rr_acq_db.h"
#include "rr_resel_calcs.h"
#include "rr_gprs_dsc.h"
#include "trm.h"
#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
#include "task.h"
#include "cm.h"
#endif /* !PERLUTF && !TEST_FRAMEWORK */
#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE)
#include "rr_g2x_redir.h"
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE || FEATURE_LTE */
#include "rr_sim.h"
#include "rr_ps_access.h"
#include "rr_ps_domain.h"
#include "gs.h"
#include "rr_conn_establish.h"
#include "rr_sys_info_pcell.h"
#include "rr_sys_info_cache.h"
#ifdef FEATURE_GSM_TDS
#include "rr_tdsrrc_if.h"
#endif /* FEATURE_GSM_TDS */
#include "stringl/stringl.h"
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
#include "rr_g2x.h"
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */
#include "rr_cell_selection.h"
#include "rr_plmn_list.h"
#include "geran_multi_sim_api.h"
#include "policyman.h"

#define RR_MS_EXTRACT_AS_ID(msg) (msg.as_id)

#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
#define GRR_UPDATE_PRIORITY_INVERSION(rsn,nst) trm_set_extension_flag(TRM_UMTS, (rsn), TRM_PRIORITY_INV_ENABLED, (nst))
#else
#error code not present
#endif /* !PERLUTF && !TEST_FRAMEWORK */

#define RR_MS_MAX_NUM_DEBUG_EVENTS 32 /* local trace buffer holds this many events per data space */

typedef enum
{
  RR_MS_NULL_DEBUG_EVENT = 0,
  RR_MS_BLOCKING_MSG_SENT,
  RR_MS_INTERFACE_EXPIRED,
  RR_MS_DS_ABORT_WAS_SENT,
  RR_MS_RESPONSE_RECEIVED,
  RR_MS_INTERFACE_SUSPEND,
  RR_MS_ENTER_HIBERNATION,
  RR_MS_INTERFACE_RESUMED,
  RR_MS_GRR_ABORT_BY_IMSG,
  RR_MS_GRR_ABORT_BY_FUNC,
  RR_MS_GRR_TIMER_EXPIRED,
  RR_MS_LOST_SYNC_WITH_L1,
  RR_MS_GRR_ABORT_BY_PSCN,
  RR_MS_POLICY_MAN_FAILED,
  RR_MS_LAST_DEBUG_EVENT /* add any new events ABOVE this line */
} rr_ms_debug_event_t;

typedef struct
{
  rr_ms_debug_event_t event_list[RR_MS_MAX_NUM_DEBUG_EVENTS];
  int                 num_events;
  int                 num_wrapped;
} rr_ms_debug_info_t;

typedef enum
{
  RR_MS_NO_ACTION,
  RR_MS_PREPARE_FOR_SUSPENSION,
  RR_MS_CHECK_FOR_SUSPENSION,
  RR_MS_PREPARE_FOR_RESUMPTION,
  RR_MS_INVALID_ACTION
} rr_ms_action_t;

typedef enum
{
  RR_MS_POLL_FOR_RESPONSE,
  RR_MS_ABORT_IS_REQUIRED
} rr_ms_interface_check_t;

typedef struct
{
  rr_ms_action_t          next_action;
  rr_process_msg_action_e process_in_rr;
} rr_ms_executor_t;

typedef enum
{
  RR_MS_INTERFACE_ACTIVE,
  RR_MS_INTERFACE_SUSPENDED,
  RR_MS_INTERFACE_HIBERNATED
} rr_ms_gl1_if_state_t;

typedef enum
{
  RR_MS_PRIORITY_INVERSION_INIT,
  RR_MS_GAS_ID_STARTS_ACQUIRING,
  RR_MS_GAS_ID_SERVICE_RECOVERY,
  RR_MS_GAS_ID_RESPONSE_PENDING,
  RR_MS_GAS_ID_SERVICE_NOTIFIED,
  RR_MS_GAS_ID_ACQUIRING_FOR_CS,
  RR_MS_GAS_ID_CS_NOW_CONNECTED,
  RR_MS_GAS_ID_CSFB_IN_PROGRESS,
  RR_MS_GAS_ID_CSFB_STATE_RESET,
  RR_MS_DUTY_CYCLE_TIMER_EXPIRY,
  RR_MS_PRI_INV_EVENT_MAX
} pri_inv_event_t;

typedef enum
{
  RR_MS_ACQ_NULL,
  RR_MS_ACQ_NOMINAL,
  RR_MS_ACQ_INVERTED,
  RR_MS_ACQ_MAXIMAL
} pri_inv_state_t;

typedef enum
{
  RR_MS_CBCH_OFFLINE, /* CBCH support is not required as the stack is offline */
  RR_MS_CBCH_REMOVED, /* CBCH support is not present on the cell */
  RR_MS_CBCH_ENABLED, /* CBCH support is enabled on the cell with GRR in idle */
  RR_MS_CBCH_BLOCKED  /* CBCH support is enabled, but blocked due to the mode */
} rr_ms_cbch_state_t;

typedef struct
{
  sys_modem_as_id_e_type subs_identifier[MAX_AS_IDS];
  subs_capability_e_type subs_capability[MAX_AS_IDS];
  int                    multimode_count;
} rr_ms_subs_info_t;

typedef struct
{
  rr_ms_gl1_if_state_t    gl1_interface_state;
  rr_l1_message_id_enum_T required_l1_message;
  rr_l1_message_id_enum_T last_cleared_l1_msg;
  rr_plmn_select_req_T    buffered_svc_request;
  dl_unit_data_ind_T      buffered_page_msg;
  rr_ms_debug_info_t      debug_events;
  rr_ms_cbch_state_t      cbch_state;
  rex_timer_cnt_type      ps_abort_delay;  
  timetick_type           buffered_page_time;
  timetick_type           missed_page;
  timetick_type           page_cycle;
  boolean                 gprs_test_mode;
  boolean                 aborted_l1_interface;
  boolean                 cs_connected;
  pri_inv_state_t         acq_mgr_state;
  uint16                  seconds_since_suspension;
  uint16                  svc_rqst_time;
  uint16                  svc_loss_time;
  word                    paging_group;
  int                     failed_blocks;
} gas_ctrl_data_t;

typedef struct
{
  sys_modem_dual_standby_pref_e_type multi_standby_pref;
  gas_ctrl_data_t                    gas_data[NUM_GERAN_DATA_SPACES];
} rr_multisim_info_t;

/* private data structure used by Multi-SIM controller functions */
static rr_multisim_info_t rr_multisim_info;

/* private data structure used for debug enhancements */
static rr_ms_subs_info_t  rr_ms_subscription_info;

#define RR_MULTI_SIM_MODE_ENABLED ((boolean)(SYS_MODEM_DEVICE_MODE_SINGLE_SIM != geran_get_multi_sim_mode()))

#define RR_PAGE_SOURCE_PRIORITY_LOW  (0)
#define RR_PAGE_SOURCE_PRIORITY_MED  (1)
#define RR_PAGE_SOURCE_PRIORITY_HIGH (2)

/*===========================================================================

FUNCTION      rr_ms_store_debug_event

DESCRIPTION   Stores a Multi-SIM debug event in a local buffer per GAS ID

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/
static void rr_ms_store_debug_event(const rr_ms_debug_event_t new_event, const gas_id_t gas_id)
{
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    int as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
    int count;

    /* check for debug buffer wraparound */
    count = rr_multisim_info.gas_data[as_index].debug_events.num_events;
    if (count >= RR_MS_MAX_NUM_DEBUG_EVENTS)
    {
      rr_multisim_info.gas_data[as_index].debug_events.num_events = 0;
      rr_multisim_info.gas_data[as_index].debug_events.num_wrapped++;
      count = 0;
    }

    /* add the new event at the next available position and increment the count */
    rr_multisim_info.gas_data[as_index].debug_events.event_list[count] = new_event;
    rr_multisim_info.gas_data[as_index].debug_events.num_events++;

    /* blank out the next entry to make the last valid event more obvious */
    count++;
    if (count < RR_MS_MAX_NUM_DEBUG_EVENTS)
    {
      rr_multisim_info.gas_data[as_index].debug_events.event_list[count] = RR_MS_NULL_DEBUG_EVENT;
    }
  }

  return;
}

/*===========================================================================

FUNCTION      rr_ms_read_policyman_config

DESCRIPTION   Reads the "sys subs" feature support flag from PolicyManager

DEPENDENCIES  None

RETURN VALUE  TRUE if all PolicyManager operations completed successfully

SIDE EFFECTS  Blocking call to PM interface

===========================================================================*/
static boolean rr_ms_read_policyman_config(sys_subs_feature_t *sys_subs_ptr)
{
  policyman_status_t status;

  if (sys_subs_ptr != NULL)
  {
    const policyman_item_id_t pm_id = POLICYMAN_ITEM_DEVICE_CONFIGURATION;
    const policyman_item_t   *pItem = NULL;

    /* request a PolicyMan item pointer using a blocking call */
    status = policyman_get_items_block(&pm_id, 1, &pItem);

    /* continue if the status is OK */
    if (POLICYMAN_SUCCEEDED(status))
    {
      /* retrieve the data associated with the item into the output parameter */
      status = policyman_device_config_get_subs_feature(pItem, 0, 0, sys_subs_ptr);
    }

    /* always release the item memory allocated by PolicyMan */
    /* NULL pointer use is OK, as PM performs its own checks */
    policyman_item_release(pItem);
  }
  else
  {
    /* default the status to "invalid args" */
    status = POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  /* return value is TRUE if all PM operations were successful */
  /* and the output parameter was updated with the result data */
  return((boolean)POLICYMAN_SUCCEEDED(status));
}

/*===========================================================================

FUNCTION      rr_ms_reset_failed_blocks

DESCRIPTION   Resets the failed block counts (to zero) for all data spaces.

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/
static void rr_ms_reset_failed_blocks(void)
{
  int as_index;

  for (as_index = GERAN_DATA_SPACE_INDEX_1; as_index < NUM_GERAN_DATA_SPACES; as_index++)
  {
    rr_multisim_info.gas_data[as_index].failed_blocks = 0;
  }

  return;
}

/*===========================================================================

FUNCTION      rr_ms_reset_internal_variables

DESCRIPTION   Resets all Multi SIM-specific control flags that are NOT
              associated with any external interface. This should only be
              called when the RR task is first started.

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/
static void rr_ms_reset_internal_variables(void)
{
  int as_index;

  /* reset the block quality counters and the interface control flags */
  rr_ms_reset_failed_blocks();
  for (as_index = GERAN_DATA_SPACE_INDEX_1; as_index < NUM_GERAN_DATA_SPACES; as_index++)
  {
    rr_multisim_info.gas_data[as_index].gl1_interface_state      = RR_MS_INTERFACE_ACTIVE;
    rr_multisim_info.gas_data[as_index].required_l1_message      = RR_MS_INVALID_L1_MESSAGE;
    rr_multisim_info.gas_data[as_index].last_cleared_l1_msg      = RR_MS_INVALID_L1_MESSAGE;
    rr_multisim_info.gas_data[as_index].aborted_l1_interface     = FALSE;
    rr_multisim_info.gas_data[as_index].cs_connected             = FALSE;
    rr_multisim_info.gas_data[as_index].acq_mgr_state            = RR_MS_ACQ_NULL;
    rr_multisim_info.gas_data[as_index].seconds_since_suspension = 0;
    rr_multisim_info.gas_data[as_index].svc_rqst_time            = 0;
    rr_multisim_info.gas_data[as_index].svc_loss_time            = 0;
  }

  return;
}

/*===========================================================================

FUNCTION      rr_ms_buffer_service_request

DESCRIPTION   Buffers a RR_PLMN_SELECT_REQ received while the AS is suspended

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/
static void rr_ms_buffer_service_request(const rr_cmd_bdy_type *cmd_ptr)
{
  if (cmd_ptr != NULL)
  {
    const rr_plmn_select_req_T *request_ptr;
    sys_modem_as_id_e_type      nas_id;
    gas_id_t                    gas_id;
    int                         as_index;

    /* service request received during suspended state */
    /* buffer it here and record the arrival timestamp */
    request_ptr = &(cmd_ptr->mm.rr_plmn_select_req);
    nas_id      = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_plmn_select_req);
    gas_id      = geran_map_nas_id_to_gas_id(nas_id);
    as_index    = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    memscpy((void *)&(rr_multisim_info.gas_data[as_index].buffered_svc_request),
            sizeof(rr_multisim_info.gas_data[as_index].buffered_svc_request),
            (void *)request_ptr, sizeof(rr_plmn_select_req_T));

    /* use a +1 here, since time=0 is used to indicate "no occurrence" */
    rr_multisim_info.gas_data[as_index].svc_rqst_time = rr_multisim_info.gas_data[as_index].seconds_since_suspension+1;

    if (rr_multisim_info.gas_data[as_index].gl1_interface_state == RR_MS_INTERFACE_HIBERNATED)
    {
      MSG_GERAN_HIGH_0_G("Buffer svc request whilst AS is hibernated");
    }
    else
    {
      MSG_GERAN_HIGH_0_G("Buffer svc request whilst AS is suspended");
    }
  }

  return;
}

#ifndef TEST_FRAMEWORK
/*===========================================================================

FUNCTION      rr_ms_get_trm_client_id

DESCRIPTION   Determines the TRM client ID associated with a GAS ID

DEPENDENCIES  None

RETURN VALUE  TRM_GSM1, TRM_GSM2, or TRM_GSM3

SIDE EFFECTS  None

===========================================================================*/
static trm_client_enum_t rr_ms_get_trm_client_id(const gas_id_t gas_id)
{
  trm_client_enum_t trm_id;

  if (GERAN_ACCESS_STRATUM_ID_2 == gas_id)
  {
    trm_id = TRM_GSM2;
  }
#ifdef FEATURE_TRIPLE_SIM
  else if (GERAN_ACCESS_STRATUM_ID_3 == gas_id)
  {
    trm_id = TRM_GSM3;
  }
#endif /* FEATURE_TRIPLE_SIM */
  else
  {
    trm_id = TRM_GSM1;
  }

  return(trm_id);
}
#endif /* !TEST_FRAMEWORK */

/*===========================================================================

FUNCTION     rr_ms_start_heartbeat

DESCRIPTION  Starts the RR Dual SIM heartbeat timer with an appropriate heartrate

DEPENDENCIES None

RETURN VALUE TRUE if the heartbeat was started successfully

SIDE EFFECTS None

===========================================================================*/
static boolean rr_ms_start_heartbeat(const rr_ms_gl1_if_state_t interface_state, const gas_id_t gas_id)
{
  rex_timer_cnt_type heart_rate;

  /* start a timer that will generate a heartbeat while the AS is unavailable */
  if (interface_state == RR_MS_INTERFACE_HIBERNATED)
  {
    /* during hibernation, heart rate is one beat per 4 seconds */
    heart_rate = RR_MS_HIBERNATE_SECONDS * GRR_TIMEOUT_1_SECOND;
  }
  else
  {
    /* normal suspension, heart rate is one beat per second */
    heart_rate = RR_MS_HEARTBEAT_SECONDS * GRR_TIMEOUT_1_SECOND;
  }

  /* heartbeat timer is started for the currently suspended GAS ID */
  return(rr_timer_start(RR_DUAL_SIM_HEARTBEAT_TIMER, heart_rate, gas_id));
}

/*===========================================================================

FUNCTION     rr_ms_set_aborted_interface

DESCRIPTION  Sets a flag to indicate whether the specified GAS ID has sent
             the Dual SIM abort signal to L1.

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_set_aborted_interface(const boolean l1_aborted, const gas_id_t gas_id)
{
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].aborted_l1_interface = l1_aborted;
  }

  return;
}

/*===========================================================================

FUNCTION     rr_ms_recover_blocked_interface

DESCRIPTION  Notifies GL1 that a pending interface should be aborted so that
             it cleans up and returns the required response.

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_recover_blocked_interface(const gas_id_t gas_id)
{
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    rr_l1_message_id_enum_T pending_msg;
    boolean                 l1_should_complete;

    /* mark the interface as aborted */
    rr_ms_set_aborted_interface(TRUE, gas_id);

    /* log an appropriate event in the debug buffer */
    rr_ms_store_debug_event(RR_MS_DS_ABORT_WAS_SENT, gas_id);

    /* GL1 should complete certain operations if it has already been granted the RF chain */
    /* because it is not possible to track every lock grant and release action separately */
    pending_msg = rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].required_l1_message;
    if ((MPH_SELECT_SPECIFIC_BCCH_CNF == pending_msg) ||
        (MPH_RESELECTION_CNF          == pending_msg) ||
        (MPH_POWER_SCAN_CNF           == pending_msg))
    {
      l1_should_complete = TRUE;
    }
    else
    {
      l1_should_complete = FALSE;
    }

    /* to resync the interface, send MPH_L1_DS_ABORT_IND to GL1 indicating which signal is pending */
    rr_send_mph_l1_ds_abort_ind(RR_DS_ABORT_TO_NULL, pending_msg, l1_should_complete, gas_id);
  }

  return;
}

/*===========================================================================

FUNCTION     rr_ms_prepare_for_suspension

DESCRIPTION  Performs operations necessary to suspend a GRR interface

DEPENDENCIES Input GAS ID parameter must be valid

RETURN VALUE TRUE if the specified GAS ID has successfully suspended

SIDE EFFECTS None

===========================================================================*/
static boolean rr_ms_prepare_for_suspension(const rr_ms_gl1_if_state_t new_interface_state, const gas_id_t gas_id)
{
  boolean now_suspended;
  int     as_index;

  as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
  if (rr_multisim_info.gas_data[as_index].gl1_interface_state == RR_MS_INTERFACE_ACTIVE)
  {
    /* reset the suspended GAS ID, its abort flag and the heartbeat counter */
    rr_multisim_info.gas_data[as_index].seconds_since_suspension = 0;
    rr_ms_set_aborted_interface(FALSE, gas_id);

    /* reset counters that track NAS activity during suspension */
    rr_multisim_info.gas_data[as_index].svc_rqst_time = 0;
    rr_multisim_info.gas_data[as_index].svc_loss_time = 0;

    /* if the heartbeat starts up OK, then suspension has succeeded */
    now_suspended = rr_ms_start_heartbeat(new_interface_state, gas_id);
    if (now_suspended == TRUE)
    {
      /* store the suspension type for later use */
      rr_multisim_info.gas_data[as_index].gl1_interface_state = new_interface_state;

      /* log an appropriate event in the debug buffer */
      if (RR_MS_INTERFACE_HIBERNATED == new_interface_state)
      {
        rr_ms_store_debug_event(RR_MS_ENTER_HIBERNATION, gas_id);
        MSG_GERAN_HIGH_0_G("GRR hibernating interface, started heartbeat");
      }
      else
      {
        rr_ms_store_debug_event(RR_MS_INTERFACE_SUSPEND, gas_id);
        MSG_GERAN_HIGH_0_G("GRR suspending interface, started heartbeat");
      }
    }
    else
    {
      /* abandon the suspension attempt */
      MSG_GERAN_ERROR_0_G("Unable to suspend due to heartbeat failure");
    }
  }
  else
  {
    now_suspended = FALSE;
    if (RR_MS_INTERFACE_HIBERNATED == new_interface_state)
    {
      MSG_GERAN_HIGH_0_G("Unable to hibernate, already in non-active state");
    }
    else
    {
      MSG_GERAN_ERROR_0_G("Unexpected suspension, already in non-active state");
    }
  }

  return(now_suspended);
}

/*===========================================================================

FUNCTION     rr_ms_prepare_for_resumption

DESCRIPTION  Performs operations necessary to resume a GRR interface

DEPENDENCIES Input GAS ID parameter must be valid

RETURN VALUE VOID

SIDE EFFECTS May reprocess a previously buffered RR_PLMN_SELECT_REQ

===========================================================================*/
static void rr_ms_prepare_for_resumption(const boolean action_svc_request, const gas_id_t gas_id)
{
  int as_index;

  as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
  if (rr_multisim_info.gas_data[as_index].gl1_interface_state != RR_MS_INTERFACE_ACTIVE)
  {
    rr_cmd_bdy_type *cmd_ptr;

    /* cancel the heartbeat and termination timers */
    (void)rr_timer_cancel(RR_DUAL_SIM_HEARTBEAT_TIMER, gas_id);
    (void)rr_timer_cancel(RR_DUAL_SIM_TERMINATE_TIMER, gas_id);
    MSG_GERAN_HIGH_0_G("GRR resuming suspended interface");

    /* check whether any service requests were buffered during suspension */
    if ((rr_multisim_info.gas_data[as_index].svc_rqst_time != 0) &&
        (action_svc_request       == TRUE))
    {
      if (rr_multisim_info.gas_data[as_index].svc_rqst_time >= rr_multisim_info.gas_data[as_index].svc_loss_time)
      {
        /* NAS has requested again since RR lost service */
        cmd_ptr = (rr_cmd_bdy_type *)&(rr_multisim_info.gas_data[as_index].buffered_svc_request);
      }
      else
      {
        /* RR went out of service after a request was buffered */
        cmd_ptr = NULL;
      }
      MSG_GERAN_HIGH_3_G("NAS request at %d, OOS at %d, action=%d",
                         (int)(rr_multisim_info.gas_data[as_index].svc_rqst_time),
                         (int)(rr_multisim_info.gas_data[as_index].svc_loss_time),
                         (int)(cmd_ptr != NULL));
    }
    else
    {
      /* no service requests from NAS during suspension/hibernation */
      cmd_ptr = NULL;
    }

    /* reset the suspension flags and counters */
    rr_multisim_info.gas_data[as_index].gl1_interface_state      = RR_MS_INTERFACE_ACTIVE;
    rr_multisim_info.gas_data[as_index].seconds_since_suspension = 0;
    rr_multisim_info.gas_data[as_index].svc_rqst_time            = 0;
    rr_multisim_info.gas_data[as_index].svc_loss_time            = 0;

    /* log an appropriate event in the debug buffer */
    rr_ms_store_debug_event(RR_MS_INTERFACE_RESUMED, gas_id);

    /* if a request needs to be serviced, process it now */
    if (cmd_ptr != NULL)
    {
      /* post the buffered request to RR's own task queue */
      if (GS_SUCCESS != gs_send_message(GS_QUEUE_RR, (void *)cmd_ptr, FALSE))
      {
        MSG_GERAN_ERROR_0_G("Unable to requeue pending service request");
      }
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Resumption not required, interface is active");
  }

  return;
}

/*===========================================================================

FUNCTION      rr_ms_other_rat_in_background_traffic

DESCRIPTION   Queries TRM to see if any other RAT is using BG_TRAFFIC

DEPENDENCIES  None

RETURN VALUE  TRUE if BG_TRAFFIC extension flag is enabled by any RAT

SIDE EFFECTS  None

===========================================================================*/
static boolean rr_ms_other_rat_in_background_traffic(void)
{
#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
  return(trm_get_extension_flag(TRM_UMTS, TRM_BG_TRAFFIC, TRM_PRIORITY_INV_ENABLED));
#else
  #error code not present
#endif /* !PERLUTF && !TEST_FRAMEWORK */
}

/*===========================================================================

FUNCTION     rr_ms_interface_was_aborted

DESCRIPTION  Checks the flag that indicate whether the specified GAS ID sent
             the MPH_L1_DS_ABORT_IND signal to L1.

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static boolean rr_ms_interface_was_aborted(const gas_id_t gas_id)
{
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    return(rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].aborted_l1_interface);
  }

  return(FALSE);
}

#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
/*===========================================================================

FUNCTION     rr_ms_restart_duty_cycle

DESCRIPTION  Starts a fresh instance of the duty cycle timer

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_restart_duty_cycle(const rex_timer_cnt_type tmr_duration, const gas_id_t gas_id)
{
  /* stop any running instance on this access stratum */
  (void)rr_timer_cancel(RR_ACQ_DUTY_CYCLE_TIMER, gas_id);

  /* and then start the timer with the defined duration */
  if (!rr_timer_start(RR_ACQ_DUTY_CYCLE_TIMER, tmr_duration, gas_id))
  {
    MSG_GERAN_ERROR_0_G("PIM: failed to start INVERSION_TIMER");
  }

  return;
}

/*===========================================================================

FUNCTION     rr_ms_update_acquisition_priority

DESCRIPTION  Updates GL1 with MPH_SET_PRIORITY_IND. The parameters of this
             function are:
                new_priority  - the priority that GL1 should store and use
                new_procedure - TRUE if GRR is expecting to send a further
                                cell selection primitive *immediately* after
                                this signal (e.g. a MPH_POWER_SCAN_REQ)

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_update_acquisition_priority(const geran_priority_t new_priority, const boolean new_procedure, const gas_id_t gas_id)
{
  if (rr_ms_interface_was_aborted(gas_id) && (TRUE != new_procedure))
  {
    /* pass the priority but always set the new_procedure flag to TRUE here */
    /* which will advise GL1 to store the new priority but NOT to action it */
    rr_send_mph_set_priority_ind(new_priority, TRUE, gas_id);
  }
  else
  {
    /* pass the priority and the new procedure flag to GL1 in the MPH_SET_PRIORITY_IND */
    rr_send_mph_set_priority_ind(new_priority, new_procedure, gas_id);
  }

  return;
}
#endif /* !PERLUTF*/

/*===========================================================================

FUNCTION     rr_ms_acquisition_manager

DESCRIPTION  Priority Inversion Management for Multi-SIM acquisition

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_acquisition_manager(const pri_inv_event_t new_event, const gas_id_t gas_id)
{
#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
  boolean new_proc_expected;
  int     as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  /* handle reset/completion events the same regardless of state */
  if ((RR_MS_PRIORITY_INVERSION_INIT == new_event) ||
      (RR_MS_GAS_ID_SERVICE_NOTIFIED == new_event) ||
      (RR_MS_GAS_ID_CS_NOW_CONNECTED == new_event))
  {
    rr_multisim_info.gas_data[as_index].acq_mgr_state = RR_MS_ACQ_NULL;
    (void)rr_timer_cancel(RR_ACQ_DUTY_CYCLE_TIMER, gas_id);
    MSG_GERAN_HIGH_1_G("PIMMS: state reset by event %d", (int)new_event);
    return;
  }
  else
  {
    MSG_GERAN_HIGH_2_G("PIMMS: event %d in state %d",
                       (int)new_event, (int)rr_multisim_info.gas_data[as_index].acq_mgr_state);
  }

  switch(rr_multisim_info.gas_data[as_index].acq_mgr_state)
  {
    case RR_MS_ACQ_NULL:
      if (RR_MS_GAS_ID_STARTS_ACQUIRING == new_event)
      {
        /* move to initial acq state for 2.5 seconds */
        rr_multisim_info.gas_data[as_index].acq_mgr_state = RR_MS_ACQ_NOMINAL;
        rr_ms_restart_duty_cycle(RR_ACQ_INITIAL_TIMEOUT, gas_id);

        /* initialise GL1 with nominal priority, indicate new procedure if stack is NOT online already */
        new_proc_expected = !rr_mode_gsm_active_or_starting(gas_id);
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_NOMINAL, new_proc_expected, gas_id);
      }
      else if (RR_MS_DUTY_CYCLE_TIMER_EXPIRY == new_event)
      {
        /* no action necessary */;
      }
      else if (RR_MS_GAS_ID_SERVICE_RECOVERY == new_event)
      {
        /* move to high priority state for 1 second */
        rr_multisim_info.gas_data[as_index].acq_mgr_state = RR_MS_ACQ_INVERTED;
        rr_ms_restart_duty_cycle(RR_ACQ_INVERSION_TIMEOUT, gas_id);

        /* update GL1 with inverted priority before the recovery procedure starts */
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_INVERTED, TRUE, gas_id);
      }
      else if (RR_MS_GAS_ID_RESPONSE_PENDING == new_event)
      {
        /* prompt GL1 to resubmit any pending request with the existing priority */
        rr_ms_update_acquisition_priority(GERAN_PRIORITY_NOT_CHANGED, FALSE, gas_id);
      }
      else if (RR_MS_GAS_ID_ACQUIRING_FOR_CS == new_event)
      {
        /* move to maximum priority state for 10 seconds */
        rr_multisim_info.gas_data[as_index].acq_mgr_state = RR_MS_ACQ_MAXIMAL;
        rr_ms_restart_duty_cycle(RR_ACQ_MAXIMAL_TIMEOUT, gas_id);

        /* update GL1 with maximal priority, indicate new procedure if stack is NOT online already */
        new_proc_expected = !rr_mode_gsm_active_or_starting(gas_id);
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_MAXIMAL, new_proc_expected, gas_id);
      }
      else if (RR_MS_GAS_ID_CSFB_IN_PROGRESS == new_event)
      {
        /* move to maximum priority state for 10 seconds */
        rr_multisim_info.gas_data[as_index].acq_mgr_state = RR_MS_ACQ_MAXIMAL;
        rr_ms_restart_duty_cycle(RR_ACQ_MAXIMAL_TIMEOUT, gas_id);

        /* update GL1 with maximal priority, indicate a new procedure will be starting */
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_MAXIMAL, TRUE, gas_id);
      }
      else if (RR_MS_GAS_ID_CSFB_STATE_RESET == new_event)
      {
        /* reset GL1 with nominal priority, but only apply it to the next procedure */
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_NOMINAL, TRUE, gas_id);
      }
      break;

    case RR_MS_ACQ_NOMINAL:
      if (RR_MS_GAS_ID_STARTS_ACQUIRING == new_event)
      {
        MSG_GERAN_HIGH_0_G("PIMMS: restarted acquisition, wait for timer event");
      }
      else if (RR_MS_DUTY_CYCLE_TIMER_EXPIRY == new_event)
      {
        /* move to high priority state for 1 second */
        rr_multisim_info.gas_data[as_index].acq_mgr_state = RR_MS_ACQ_INVERTED;
        rr_ms_restart_duty_cycle(RR_ACQ_INVERSION_TIMEOUT, gas_id);

        /* update GL1 with inverted priority for existing procedure */
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_INVERTED, FALSE, gas_id);
      }
      else if (RR_MS_GAS_ID_SERVICE_RECOVERY == new_event)
      {
        /* move to high priority state for 1 second */
        rr_multisim_info.gas_data[as_index].acq_mgr_state = RR_MS_ACQ_INVERTED;
        rr_ms_restart_duty_cycle(RR_ACQ_INVERSION_TIMEOUT, gas_id);

        /* update GL1 with inverted priority before the recovery procedure starts */
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_INVERTED, TRUE, gas_id);
      }
      else if (RR_MS_GAS_ID_RESPONSE_PENDING == new_event)
      {
        /* trigger GL1 to retry the lock with the same (i.e. nominal) priority */
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_NOMINAL, FALSE, gas_id);
      }
      else if (RR_MS_GAS_ID_ACQUIRING_FOR_CS == new_event)
      {
        /* move to maximum priority state for 10 seconds */
        rr_multisim_info.gas_data[as_index].acq_mgr_state = RR_MS_ACQ_MAXIMAL;
        rr_ms_restart_duty_cycle(RR_ACQ_MAXIMAL_TIMEOUT, gas_id);

        /* update GL1 with maximal priority, indicate new procedure if stack is NOT online already */
        new_proc_expected = !rr_mode_gsm_active_or_starting(gas_id);
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_MAXIMAL, new_proc_expected, gas_id);
      }
      else if (RR_MS_GAS_ID_CSFB_IN_PROGRESS == new_event)
      {
        /* move to maximum priority state for 10 seconds */
        rr_multisim_info.gas_data[as_index].acq_mgr_state = RR_MS_ACQ_MAXIMAL;
        rr_ms_restart_duty_cycle(RR_ACQ_MAXIMAL_TIMEOUT, gas_id);

        /* update GL1 with maximal priority for existing acquisition procedure */
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_MAXIMAL, FALSE, gas_id);
      }
      else if (RR_MS_GAS_ID_CSFB_STATE_RESET == new_event)
      {
        /* no action necessary */;
      }
      break;

    case RR_MS_ACQ_INVERTED:
      if (RR_MS_GAS_ID_STARTS_ACQUIRING == new_event)
      {
        MSG_GERAN_HIGH_0_G("PIMMS: restarted acquisition, wait for timer event");
      }
      else if (RR_MS_DUTY_CYCLE_TIMER_EXPIRY == new_event)
      {
        /* move back to nominal priority state for 4 seconds */
        rr_multisim_info.gas_data[as_index].acq_mgr_state = RR_MS_ACQ_NOMINAL;
        rr_ms_restart_duty_cycle(RR_ACQ_NOMINAL_TIMEOUT, gas_id);

        /* update GL1 with nominal priority */
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_NOMINAL, FALSE, gas_id);
      }
      else if (RR_MS_GAS_ID_RESPONSE_PENDING == new_event)
      {
        /* trigger GL1 to retry the lock with the same (i.e. inverted) priority */
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_INVERTED, FALSE, gas_id);
      }
      else if (RR_MS_GAS_ID_SERVICE_RECOVERY == new_event)
      {
        /* update GL1 with the same priority before the recovery procedure starts */
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_INVERTED, TRUE, gas_id);
      }
      else if (RR_MS_GAS_ID_ACQUIRING_FOR_CS == new_event)
      {
        /* move to maximum priority state for 10 seconds */
        rr_multisim_info.gas_data[as_index].acq_mgr_state = RR_MS_ACQ_MAXIMAL;
        rr_ms_restart_duty_cycle(RR_ACQ_MAXIMAL_TIMEOUT, gas_id);

        /* update GL1 with maximal priority, indicate new procedure if stack is NOT online already */
        new_proc_expected = !rr_mode_gsm_active_or_starting(gas_id);
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_MAXIMAL, new_proc_expected, gas_id);
      }
      else if (RR_MS_GAS_ID_CSFB_IN_PROGRESS == new_event)
      {
        /* move to maximum priority state for 10 seconds */
        rr_multisim_info.gas_data[as_index].acq_mgr_state = RR_MS_ACQ_MAXIMAL;
        rr_ms_restart_duty_cycle(RR_ACQ_MAXIMAL_TIMEOUT, gas_id);

        /* update GL1 with maximal priority for existing acquisition procedure */
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_MAXIMAL, FALSE, gas_id);
      }
      else if (RR_MS_GAS_ID_CSFB_STATE_RESET == new_event)
      {
        /* no action necessary */;
      }
      break;

    case RR_MS_ACQ_MAXIMAL:
      if ((RR_MS_GAS_ID_STARTS_ACQUIRING == new_event) ||
          (RR_MS_GAS_ID_CSFB_IN_PROGRESS == new_event) ||
          (RR_MS_GAS_ID_ACQUIRING_FOR_CS == new_event))
      {
        MSG_GERAN_HIGH_0_G("PIMMS: in maximum priority state - wait for timer or CS event");
      }
      else if (RR_MS_DUTY_CYCLE_TIMER_EXPIRY == new_event)
      {
        /* move to high priority state for 1 second */
        rr_multisim_info.gas_data[as_index].acq_mgr_state = RR_MS_ACQ_INVERTED;
        rr_ms_restart_duty_cycle(RR_ACQ_INVERSION_TIMEOUT, gas_id);

        /* update GL1 with inverted priority for the subsequent acquisition */
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_INVERTED, TRUE, gas_id);
      }
      else if (RR_MS_GAS_ID_RESPONSE_PENDING == new_event)
      {
        /* trigger GL1 to retry the lock with the same (i.e. maximal) priority */
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_MAXIMAL, FALSE, gas_id);
      }
      else if (RR_MS_GAS_ID_SERVICE_RECOVERY == new_event)
      {
        /* update GL1 with maximal priority before the recovery procedure starts */
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_MAXIMAL, TRUE, gas_id);
      }
      else if (RR_MS_GAS_ID_CSFB_STATE_RESET == new_event)
      {
        /* end of the maximum priority session for CSFB, move back to NULL */
        rr_multisim_info.gas_data[as_index].acq_mgr_state = RR_MS_ACQ_NULL;
        (void)rr_timer_cancel(RR_ACQ_DUTY_CYCLE_TIMER, gas_id);

        /* reset GL1 with nominal priority, but only apply it to the next procedure */
        rr_ms_update_acquisition_priority(GERAN_ACQ_PRIORITY_NOMINAL, TRUE, gas_id);
      }
      break;

    default:
      MSG_GERAN_ERROR_0_G("PIMMS: unhandled state, event ignored");
      break;
  }
#endif /* !PERLUTF */

  return;
}

/*===========================================================================

FUNCTION      rr_ms_validate_service_request

DESCRIPTION   Checks for discrepancies in the RR_PLMN_SELECT_REQ signal from NAS

DEPENDENCIES  None

RETURN VALUE  ASID to be used by main DSDS controller

SIDE EFFECTS  May reject NAS signal and cause the message to be ignored

===========================================================================*/
static sys_modem_as_id_e_type rr_ms_validate_service_request(const rr_cmd_bdy_type *cmd_ptr, rr_ms_executor_t *exec_ptr)
{
  sys_modem_as_id_e_type nas_id;
  sys_modem_as_id_e_type bad_id;
  gas_id_t               gas_id;

  /* extract the ASID which NAS has used in this message */
  nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_plmn_select_req);
  gas_id = geran_map_nas_id_to_gas_id(nas_id);

  /* check for invalid service domain types and respond immediately */
  if ((cmd_ptr->mm.rr_plmn_select_req.service_domain == SYS_SRV_DOMAIN_NONE) ||
      (cmd_ptr->mm.rr_plmn_select_req.service_domain == SYS_SRV_DOMAIN_NO_SRV))
  {
    bad_id = nas_id;
    MSG_GERAN_ERROR_0_G("NAS sent service request for NO_SERVICE");
  }
  else
  {
    /* request is OK so far */
    bad_id = SYS_MODEM_AS_ID_NONE;

    /* perform sanity checks for single standby mode */
    if (RR_MULTI_SIM_MODE_ENABLED == FALSE)
    {
      if (rr_ms_any_other_active_gas_id(gas_id))
      {
        bad_id = nas_id;
        MSG_GERAN_ERROR_0_G("NAS trying to start multiple stacks in Single Standby mode");
      }
    }
  }

  /* if the request has been rejected, set output parameters accordingly */
  if (bad_id != SYS_MODEM_AS_ID_NONE)
  {
    /* no need to process this primitive further */
    exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
    exec_ptr->next_action   = RR_MS_NO_ACTION;
    nas_id                  = SYS_MODEM_AS_ID_NONE;

    /* explicitly reject the NAS request */
    rr_ms_send_rr_plmn_select_cnf(bad_id, MM_AS_CONNECTED, MM_RR_OTHER_FAILURE, FALSE);
  }
  else
  {
    /* otherwise check whether this stack needs to start priority inversion */
    if (RR_MM_MO_CS_CALL == cmd_ptr->mm.rr_plmn_select_req.service_req_cause)
    {
      MSG_GERAN_HIGH_0_G("NAS has sent service request for CS_CALL");
      rr_ms_acquisition_manager(RR_MS_GAS_ID_ACQUIRING_FOR_CS, gas_id);
    }
    else
    {
      rr_ms_acquisition_manager(RR_MS_GAS_ID_STARTS_ACQUIRING, gas_id);
    }
  }

  return(nas_id);
}

/*===========================================================================

FUNCTION      rr_ms_validate_connection_request

DESCRIPTION   Checks whether a RR_EST_REQ signal from NAS can be handled

DEPENDENCIES  None

RETURN VALUE  ASID to be used by main DSDS controller

SIDE EFFECTS  May reject NAS signal and cause the message to be ignored

===========================================================================*/
static sys_modem_as_id_e_type rr_ms_validate_connection_request(const rr_cmd_bdy_type *cmd_ptr, rr_ms_executor_t *exec_ptr)
{
  sys_modem_as_id_e_type nas_id;
  gas_id_t               gas_id;

  /* extract the ASID which NAS has used in this message */
  nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_est_req);
  /* and convert it to a GERAN access stratum ID */
  gas_id = geran_map_nas_id_to_gas_id(nas_id);

  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    int as_index;

    /* establish a data space index for the supplied NAS ID */
    as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    /* check whether this stack is waiting for a cell selection primitive in DSDS mode */
    if (MPH_SELECT_SPECIFIC_BCCH_CNF == rr_multisim_info.gas_data[as_index].required_l1_message)
    {
      /* GRR is waiting for an existing cell acquisition to complete */
      /* so prevent this call request from interrupting this process */
      rr_send_abort_ind(RR_ABORT_CON_EST_NOT_ATTEMPTED, MM_RR_OTHER_FAILURE, gas_id);

      /* no need to process this primitive further */
      exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
      exec_ptr->next_action   = RR_MS_NO_ACTION;

      MSG_GERAN_ERROR_0_G("Call establishment rejected - cell acquisition incomplete");
    }
  }

  return(nas_id);
}

/*===========================================================================

FUNCTION      rr_ms_validate_plmn_search_request

DESCRIPTION   Checks whether a RR_PLMN_LIST_REQ signal from NAS can be handled

DEPENDENCIES  None

RETURN VALUE  ASID to be used by main DSDS controller

SIDE EFFECTS  NA

===========================================================================*/
static sys_modem_as_id_e_type rr_ms_validate_plmn_search_request(const rr_cmd_bdy_type *cmd_ptr, rr_ms_executor_t *exec_ptr)
{
  sys_modem_as_id_e_type nas_id;
  gas_id_t               gas_id;

  /* extract the ASID which NAS has used in this message */
  nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_plmn_list_req);
  /* and convert it to a GERAN access stratum ID */
  gas_id = geran_map_nas_id_to_gas_id(nas_id);
  /* check whether other stack is in connected mode */
  if (geran_in_multi_standby_mode() && rr_ms_other_stack_in_dedicated(gas_id))
  {
    /*Send reject for Automatic BPLMN mode */
    if (SYS_NETWORK_SELECTION_MODE_AUTOMATIC == cmd_ptr->mm.rr_plmn_list_req.network_selection_mode)
    {
      MSG_GERAN_HIGH_0_G("PLMN search for auto mode rejected other G sub in connected mode");
      rr_ms_send_rr_plmn_list_cnf(&(cmd_ptr->mm.rr_plmn_list_req), MM_AS_CONNECTED);
      /* no need to process this primitive further */
      exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
      exec_ptr->next_action   = RR_MS_NO_ACTION;
    }
  }
  return(nas_id);
}

/*===========================================================================

FUNCTION      rr_ms_channel_sharing_allowed

DESCRIPTION   checks if channel sharing is allowed or not

DEPENDENCIES  None

RETURN VALUE  TRUE if channel sharing is possible on the specified GAS ID

SIDE EFFECTS  None

===========================================================================*/
boolean rr_ms_channel_sharing_allowed(const gas_id_t gas_id)
{
  boolean allow_sharing = FALSE;

  /* check whether this access stratum is online */
  if (rr_mode_gsm_active_or_starting(gas_id))
  {
    /* This access stratum is online means we not in SSSS mode.
     * Sharing is allowed in DSDS/TSTS */
    if (geran_in_multi_standby_mode())
    {
      allow_sharing = TRUE;
    }
#if defined(FEATURE_SGLTE_DUAL_SIM) || defined(FEATURE_SVLTE_DUAL_SIM)
    /* DSDA mode */
    else
    {
      gas_id_t multi_mode_gas_id = geran_get_multi_mode_gas_id();

      /* In DSDA mode sharing is allowed only when SGLTE mode is active and service domain is CS only */
      if (rr_is_sglte_mode_active(multi_mode_gas_id) &&
          rr_sel_get_service_domain(multi_mode_gas_id) == SYS_SRV_DOMAIN_CS_ONLY)
      {
        allow_sharing = TRUE;
      }
    }
#endif /* FEATURE_SGLTE_DUAL_SIM || FEATURE_SVLTE_DUAL_SIM */
  }

  return(allow_sharing);
}

/*===========================================================================

FUNCTION      rr_ms_share_ota_message

DESCRIPTION   Demultiplexer callback from Layer 2 for downlink data blocks

DEPENDENCIES  None

RETURN VALUE  TRUE if the specified GAS ID wants the block on the specified ARFCN

SIDE EFFECTS  None

===========================================================================*/
boolean rr_ms_share_ota_message(const l2_mux_data_t rcvd_type, ARFCN_T channel, const gas_id_t gas_id)
{
  boolean allow_sharing = FALSE;

  /* check whether channel sharing is allowed */
  if (rr_ms_channel_sharing_allowed(gas_id))
  {
    gprs_scell_info_t *scell_ptr = rr_gprs_get_scell_info(gas_id);
    gprs_scell_info_t *pcell_ptr = rr_gprs_get_pcell_info(gas_id);

    if (L2_MUX_CBCH == rcvd_type)
    {
      /* determine whether CBCH is enabled on this stack (regardless of mode) */
      if ((RR_MS_CBCH_ENABLED == rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].cbch_state) ||
          (RR_MS_CBCH_BLOCKED == rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].cbch_state))
      {
        allow_sharing = (scell_ptr->gsm.camped_on_flag) && ARFCNS_EQUAL(scell_ptr->gsm.BCCH_ARFCN, channel);
      }
    }
    else if (L2_MUX_CCCH == rcvd_type)
    {
      if (ARFCNS_EQUAL(scell_ptr->gsm.BCCH_ARFCN, channel) ||
          ARFCNS_EQUAL(pcell_ptr->gsm.BCCH_ARFCN, channel))
      {
        allow_sharing = TRUE;
      }
    }
  }

  return(allow_sharing);
}

/*===========================================================================

FUNCTION      rr_ms_register_channel_sharing

DESCRIPTION   Registers a set of callback functions that can be used for
              demultiplexing of CCCH or CBCH blocks.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

===========================================================================*/
static void rr_ms_register_channel_sharing(void)
{
  MSG_GERAN_HIGH_0("Registering callback functions for channel sharing");
  rr_send_dl_data_mux_ind(
    rr_ms_share_ota_message,
    rr_ms_share_ota_message,
    GERAN_ACCESS_STRATUM_ID_1
  );

  return;
}

/*===========================================================================

FUNCTION      rr_ms_count_active_stacks

DESCRIPTION   Counts how many GERAN stacks have an active Layer 1

DEPENDENCIES  None

RETURN VALUE  Number of active GERAN stacks when this function is called

SIDE EFFECTS  None

===========================================================================*/
static int rr_ms_count_active_stacks(void)
{
  gas_id_t gas_id;
  int      num_active_now;

  /* count the number of GERAN stacks and remember one of the active GAS IDs */
  num_active_now = 0;
  for (gas_id = GERAN_ACCESS_STRATUM_ID_1; gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; gas_id++)
  {
    if (rr_mode_gsm_active_or_starting(gas_id))
    {
      /* this stack is active (or transitioning to active) */
      num_active_now++;
    }
  }

  return(num_active_now);
}

/*===========================================================================

FUNCTION      rr_ms_status_change_handler

DESCRIPTION   Handle the multi-SIM mode flags within RR_DS_STATUS_CHANGE_IND

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  May trigger device mode updates to Layer 1

===========================================================================*/
static void rr_ms_status_change_handler(const rr_ds_status_change_ind_T *status_ptr)
{
  if (status_ptr != NULL)
  {
    sys_modem_dual_standby_pref_e_type ms_standby_pref = status_ptr->multi_standby_mode_pref;
    sys_subs_feature_t                 subs_feature    = SYS_SUBS_FEATURE_MODE_NORMAL;
    boolean device_mode_changed;

    if (rr_ms_read_policyman_config(&subs_feature))
    {
      geran_set_multi_mode_subs_feature(subs_feature);
    }
    else
    {
      /* log an appropriate event in the debug buffer */
      rr_ms_store_debug_event(RR_MS_POLICY_MAN_FAILED, GERAN_ACCESS_STRATUM_ID_1);
      MSG_GERAN_ERROR_0("Unable to read sys subs feature from PolicyMan");
    }

    /* Update GCOMMON storage of device mode and standby pref */
    device_mode_changed = geran_set_sys_multi_sim_config(status_ptr->device_mode, ms_standby_pref);

    /* NAS interface is permitted to make changes between DSDS and DSDA after activation */
    if (device_mode_changed)
    {
      gas_id_t gas_id;

      /* device mode has changed, check whether any G stacks are already active */
      for (gas_id = GERAN_ACCESS_STRATUM_ID_1; gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; gas_id++)
      {
        if (rr_mode_gsm_active_or_starting(gas_id))
        {
          /* need to update GL1 with new device mode (once only) */
          rr_send_mph_device_mode_ind(GERAN_DEVICE_MODE_CAUSE_MMCP, gas_id);
          break;
        }
      }
    }

    /* output debug of current states */
    MSG_GERAN_HIGH_3("RR_DS_STATUS_CHANGE_IND: GRR=%d SIM=%d new standby pref=%d",
                     rr_ms_count_active_stacks(), rr_ms_get_active_sim_count(), (int)ms_standby_pref);

    /* check whether NAS is changing the Multi-SIM Standby preference */
    if (ms_standby_pref != rr_multisim_info.multi_standby_pref)
    {
      rr_multisim_info.multi_standby_pref = ms_standby_pref;
      MSG_GERAN_HIGH_1("Multi-Standby pref changed by NAS, now %d", (int)ms_standby_pref);
    }
    else
    {
      MSG_GERAN_ERROR_1("Multi-Standby pref is still %d, not changed by NAS", (int)ms_standby_pref);
    }
  }

  return;
}

/*===========================================================================

FUNCTION      rr_ms_store_subscription_capabilities

DESCRIPTION   Generates a list of subscription IDs and associated capabilities

DEPENDENCIES  None

RETURN VALUE  ASID of first subscription found with multimode capability

SIDE EFFECTS  Invokes Call Manager API if provided capability list is NULL

===========================================================================*/
static sys_modem_as_id_e_type rr_ms_store_subscription_capabilities(const subs_capability_e_type *subs_cap_list_ptr)
{
  sys_modem_as_id_e_type mmode_id = SYS_MODEM_AS_ID_1;
  sys_modem_as_id_e_type as_id    = SYS_MODEM_AS_ID_1;
  int                    i;

  /* reset number of multimode subscriptions identified */
  rr_ms_subscription_info.multimode_count = 0;

  for (i = 0; i < MAX_AS_IDS; i++, as_id++)
  {
    /* store the subscription identifier */
    rr_ms_subscription_info.subs_identifier[i] = as_id;

    /* store the subscription capability if provided, or query it now */
    if (subs_cap_list_ptr != NULL)
    {
      MSG_GERAN_HIGH_3("subs_capability for ASID 0x%02X=%d (%d=multimode)",
                       (int)as_id, (int)(subs_cap_list_ptr[i]),
                       (int)SUBS_CAPABILITY_MULTIMODE);

      rr_ms_subscription_info.subs_capability[i] = subs_cap_list_ptr[i];
    }
#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
    else if (cm_is_multimode_subs(as_id))
    {
      rr_ms_subscription_info.subs_capability[i] = SUBS_CAPABILITY_MULTIMODE;
    }
#endif /* !PERLUTF && !TEST_FRAMEWORK */
    else
    {
      rr_ms_subscription_info.subs_capability[i] = SUBS_CAPABILITY_GSM_ONLY;
    }

    /* keep running total of number of multimode subscriptions */
    if (SUBS_CAPABILITY_MULTIMODE == rr_ms_subscription_info.subs_capability[i])
    {
      if (rr_ms_subscription_info.multimode_count == 0)
      {
        /* remember the first multimode capable subscription ID */
        mmode_id = as_id;
      }
      rr_ms_subscription_info.multimode_count++;
    }
  }

  /* in multi-standby mode, print an error if more than one multimode sub was found */
  if (RR_MULTI_SIM_MODE_ENABLED && (rr_ms_subscription_info.multimode_count > 1))
  {
    MSG_GERAN_ERROR_1("Unexpected number of multimode subscriptions %d", rr_ms_subscription_info.multimode_count);
  }

  return(mmode_id);
}

/*===========================================================================

FUNCTION      rr_ms_process_nas_msg

DESCRIPTION   Performs Dual SIM preprocessing for an incoming NAS message

DEPENDENCIES  None

RETURN VALUE  GERAN access stratum ID

SIDE EFFECTS  None

===========================================================================*/
static gas_id_t rr_ms_process_nas_msg(const rr_cmd_bdy_type *cmd_ptr, rr_ms_executor_t *exec_ptr)
{
  sys_modem_as_id_e_type nas_id = SYS_MODEM_AS_ID_NONE;

  /* most NAS messages have an Active Subscription ID in them */
  switch(cmd_ptr->message_header.message_id)
  {
    case RR_ABORT_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_abort_req);
      break;

    case RR_ACT_REQ:
      /* this primitive has been deprecated by NAS */
      MSG_GERAN_ERROR_0("GRR has received unexpected RR_ACT_REQ from NAS");
      break;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    case RR_ACTIVATION_RSP:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_activation_rsp);
      MSG_GERAN_HIGH_2(
        "ASID 0x%02X is in dual RAT mode, PS=%d",
        (int)nas_id,
        (int)(cmd_ptr->rr_activation_rsp.PS_Attached)
      );
      break;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

    case RR_DATA_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_data_req);
      break;

    case RR_DEACT_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_deact_req);
      break;

    case RR_EST_REQ:
      nas_id = rr_ms_validate_connection_request(cmd_ptr, exec_ptr);
      break;

    case RR_PLMN_LIST_REQ:
      nas_id = rr_ms_validate_plmn_search_request(cmd_ptr, exec_ptr);
      break;

    case RR_PLMN_SELECT_REQ:
      /* extract the NAS ID and validate the message content */
      nas_id = rr_ms_validate_service_request(cmd_ptr, exec_ptr);
      break;

    case RR_FORBIDDEN_LAI_UPDATE_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_forbidden_lai_update_req);
      break;

    case RR_SIM_INSERTED_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_sim_inserted_req);
      break;

    case RR_INVALIDATE_SIM_DATA_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_invalidate_sim_data_req);
      break;

    case RR_STOP_GSM_MODE_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_stop_gsm_mode_req);
      /* additional debug useful to trap ASID mismatches etc. */
      MSG_GERAN_HIGH_3("RR_STOP_GSM_MODE_REQ: reason=%d (MODE=%d) for ASID 0x%02X",
                       (int)(cmd_ptr->mm.rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason),
                       (int)geran_get_multi_sim_mode(),
                       (int)nas_id);
      break;

    case RR_SIM_UPDATE_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_sim_update_req);
      break;

    case RR_EQ_PLMN_LIST_CHANGE_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_eq_plmn_list_change_ind);
      break;

    case RR_BPLMN_SEARCH_ABORT_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_bplmn_search_abort_req);
      break;

    case RR_CHANGE_MODE_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_change_mode_ind);
      break;

    case RR_LTE_RESELECTION_ALLOWED_STATUS_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_lte_reselection_allowed_status_ind);
      break;

    case RR_LAI_REJECT_LIST_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_lai_reject_list_ind);
      break;

    case RR_CSFB_BLIND_REDIR_TO_LTE_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_csfb_blind_redir_to_lte_req);
      break;

    case RR_CSFB_CALL_STATUS_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_csfb_call_status_ind);
      break;

    case RR_UE_MODE_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_ue_mode_ind);
      break;

    case RR_CSG_WHITE_LIST_UPDATE_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_csg_white_list_update_ind);
      break;

    case RR_GMM_GPRS_STATE_CHANGE_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_gmm_gprs_state_change_req);
      break;

    case RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_gmm_gprs_tlli_assignment_req);
      break;

    case RR_GMM_READY_TIMER_STATE_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_gmm_ready_timer_state_req);
      break;

    case RR_GMM_GPRS_MS_RA_CAP_IE_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_gmm_gprs_ms_ra_cap_ie_ind);
      break;

    case RR_GMM_START_MM_NON_DRX_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_gmm_start_mm_non_drx_ind);
      break;

    case RR_GMM_STOP_MM_NON_DRX_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_gmm_stop_mm_non_drx_ind);
      break;

    case RR_GMM_GPRS_SERVICE_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_gmm_gprs_service_req);
      break;

    case RR_GMM_GPRS_NV_PARAMS_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_gmm_gprs_nv_params_ind);
      break;

    case RR_GMM_GPRS_AUTH_FAIL_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_gmm_gprs_auth_fail_ind);
      break;

#ifdef FEATURE_GPRS_PS_HANDOVER
    case RR_PSHO_CNF:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_psho_cnf);
      break;
#endif /* FEATURE_GPRS_PS_HANDOVER */

    case RR_DS_STATUS_CHANGE_IND:
      /* process the new indication and check for changes */
      rr_ms_status_change_handler(&(cmd_ptr->mm.rr_ds_status_change_ind));
      break;

    case RR_PS_ABORT_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_ps_abort_req);
      break;

    case RR_PS_RESUME_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_ps_resume_ind);
      break;

    case RR_NW_SEL_MODE_RESET_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_nw_sel_mode_reset_ind);
      break;

    case RR_SERVICE_STATUS_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_service_status_ind);
      break;

    case RR_MULTIMODE_SUBS_CHGD_REQ:
    {
      const rr_multimode_subs_chgd_req_T *subs_chgd_req = &cmd_ptr->mm.rr_multimode_subs_chgd_req;
      sys_modem_as_id_e_type              multi_mode_sub;
      gas_id_t                            gas_id;

      /* process the list to find the multimode ASID and pass it to GCOMMON */
      multi_mode_sub = rr_ms_store_subscription_capabilities(subs_chgd_req->subs_capability);
      geran_set_multi_mode_sub(multi_mode_sub);

      /* ensure that affected data is repopulated for ALL stacks now */
      for (gas_id = GERAN_ACCESS_STRATUM_ID_1; gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; gas_id++)
      {
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
        if (geran_is_multi_mode_gas_id(gas_id))
        {
          /* forbidden LAI is stored by ASID, so ensure G2X module is updated here */
          rr_g2x_forbidden_lai_list_updated(gas_id);
        }
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

        rr_public_store_update_nv_data(gas_id);

        // Discard pscan results tied to a particular sub
        rr_pscan_discard_common_results(gas_id);
        rr_invalidate_power_scan_results(gas_id);
      }

      /* respond to the NAS primitive immediately */
      rr_mm_send_multimode_subs_chgd_cnf();
      break;
    }

#ifdef FEATURE_DUAL_DATA
    case RR_DATA_PRIORITY_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_data_prio_ind);
      break;
#endif /* FEATURE_DUAL_DATA */

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
    case RR_PSEUDO_LTE_LIST_UPDATE_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_pseudo_lte_list_update_ind);
    break;
#endif /* FEATURE_FAST_G2L_PSEUDO_SI2Q */

    case RR_MT_CSFB_FAILURE_IND:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->mm.rr_mt_csfb_failure_ind);
      break;
	  
    default:
      MSG_GERAN_ERROR_1("Unhandled NAS signal 0x%02X", (int)(cmd_ptr->message_header.message_id));
      break;
  }

  /* handle the cases where the signal does not carry any subscription ID */
  if (nas_id == SYS_MODEM_AS_ID_NONE)
  {
    exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
    exec_ptr->next_action   = RR_MS_NO_ACTION;

    return(GERAN_ACCESS_STRATUM_ID_UNDEFINED);
  }

  return(geran_map_nas_id_to_gas_id(nas_id));
}

/*===========================================================================

FUNCTION     rr_ms_check_pending_response

DESCRIPTION  Checks whether a GL1 response is still pending and notifies the
             Priority Inversion Manager if necessary

DEPENDENCIES None

RETURN VALUE TRUE if a GL1 response is outstanding

SIDE EFFECTS None

===========================================================================*/
static boolean rr_ms_check_pending_response(const rr_ms_interface_check_t interface_check, const gas_id_t gas_id)
{
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    rr_l1_message_id_enum_T pending_msg;

    /* determine which message is pending from GL1 */
    pending_msg = rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].required_l1_message;

    if ((MPH_DECODE_BCCH_LIST_CNF     == pending_msg) ||
        (MPH_POWER_SCAN_CNF           == pending_msg) ||
        (MPH_SELECT_SPECIFIC_BCCH_CNF == pending_msg) ||
        (MPH_RESELECTION_CNF          == pending_msg))
    {
      /* GRR is waiting for (re)selection to complete, so trigger */
      /* the Priority Inversion Manager with an appropriate event */
      /* or signal to Layer 1 that it should abort this procedure */
      if (RR_MS_ABORT_IS_REQUIRED == interface_check)
      {
        MSG_GERAN_HIGH_1_G("GRR still pending GL1 msg 0x%02X, abort required", (int)pending_msg);
        rr_ms_recover_blocked_interface(gas_id);
      }
      else
      {
        MSG_GERAN_HIGH_1_G("GRR still pending GL1 msg 0x%02X, notify PIMMS", (int)pending_msg);
        rr_ms_acquisition_manager(RR_MS_GAS_ID_RESPONSE_PENDING, gas_id);
      }
      return(TRUE);
    }
    else if (RR_MS_INVALID_L1_MESSAGE != pending_msg)
    {
      MSG_GERAN_ERROR_1_G("Not expecting GRR to wait for GL1 msg 0x%02X", (int)pending_msg);
    }
  }

  return(FALSE);
}

/*===========================================================================

FUNCTION     rr_ms_abort_pending_interface

DESCRIPTION  Called when a GRR abort of cell (re)selection is about to occur
             and checks whether it will affect a pending interface

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static rr_ms_action_t rr_ms_abort_pending_interface(const rr_ms_debug_event_t debug_event, const gas_id_t gas_id)
{
  /* log the specified event in the debug buffer */
  rr_ms_store_debug_event(debug_event, gas_id);

  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    int as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    /* ensure that aborts are only processed once  */
    if (FALSE == rr_ms_interface_was_aborted(gas_id))
    {
      /* check the interface for a pending response, and abort it if necessary */
      if (rr_ms_check_pending_response(RR_MS_ABORT_IS_REQUIRED, gas_id))
      {
        MSG_GERAN_HIGH_1_G("GRR internal abort event %d while response pending from GL1", (int)debug_event);
      }
    }

    /* if the interface has been suspended previously, resume it now */
    if (RR_MS_INTERFACE_ACTIVE != rr_multisim_info.gas_data[as_index].gl1_interface_state)
    {
      return(RR_MS_PREPARE_FOR_RESUMPTION);
    }
    else
    {
      rr_l1_message_id_enum_T pending_l1_msg;

      /* interface is active, check consistency of pending L1 message and interface timer */
      pending_l1_msg = rr_multisim_info.gas_data[as_index].required_l1_message;
      if (pending_l1_msg != RR_MS_INVALID_L1_MESSAGE)
      {
        /* some response is pending from GL1, so the interface timer should be running */
        /* unless this interface has already been aborted by actions of the code above */
        if ((FALSE == rr_timer_is_running(RR_DUAL_SIM_INTERFACE_TIMER, gas_id)) &&
            (FALSE == rr_ms_interface_was_aborted(gas_id)))
        {
          /* timer is NOT running, no abort was sent, log an event in the debug buffer */
          rr_ms_store_debug_event(RR_MS_LOST_SYNC_WITH_L1, gas_id);
          MSG_GERAN_ERROR_1_G("Interface lost sync for GL1 msg 0x%02X", (int)pending_l1_msg);
        }
      }
    }
  }

  return(RR_MS_NO_ACTION);
}

/*===========================================================================

FUNCTION     rr_ms_check_pending_service_request

DESCRIPTION  Checks whether a PLMN_SELECT_REQ received during suspension is
             stale, and aborts it if necessary

DEPENDENCIES Should only be called for a suspended access stratum

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_check_pending_service_request(const gas_id_t gas_id, const uint16 time_now)
{
  int as_index;

  as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
  if ((rr_multisim_info.gas_data[as_index].svc_rqst_time != 0) &&
      (rr_multisim_info.gas_data[as_index].svc_rqst_time < time_now))
  {
    uint16 request_age;

    request_age = time_now - rr_multisim_info.gas_data[as_index].svc_rqst_time;
    if (request_age >= RR_MS_SERVICE_REQUEST_TIMEOUT)
    {
      sys_modem_as_id_e_type nas_id;

      /* abort the last buffered request and reset the staleness counter */
      nas_id = geran_map_gas_id_to_nas_id(gas_id);
      rr_multisim_info.gas_data[as_index].svc_rqst_time = 0;
      MSG_GERAN_HIGH_0_G("Aborting pending NAS svc request");
      rr_ms_send_rr_plmn_select_cnf(nas_id, MM_AS_CONNECTED, MM_RR_NO_RESOURCE_AVAILABLE, TRUE);
    }
  }

  return;
}

/*===========================================================================

FUNCTION     rr_ms_abort_pending_service_request

DESCRIPTION  Clears down any pending service request from NAS received during
             suspension

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_abort_pending_service_request(const gas_id_t gas_id, const boolean reply_to_nas)
{
  int as_index;

  /* verify that the current access stratum is still suspended */
  as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
  if (rr_multisim_info.gas_data[as_index].gl1_interface_state != RR_MS_INTERFACE_ACTIVE)
  {
    uint16 effective_time;

    if (TRUE == reply_to_nas)
    {
      /* create a timestamp (in seconds) that is guaranteed to make any */
      /* pending service request look like it is too old, and abort it  */
      effective_time = RR_MS_HEARTBEAT_LIMIT + RR_MS_SERVICE_REQUEST_TIMEOUT + 1;
      rr_ms_check_pending_service_request(gas_id, effective_time);
    }
    else
    {
      effective_time = rr_multisim_info.gas_data[as_index].svc_rqst_time;
      /* reset the counter to mark the request as invalid */
      rr_multisim_info.gas_data[as_index].svc_rqst_time = 0;
      MSG_GERAN_HIGH_1_G("Discarded RR_PLMN_SELECT_REQ cached at time %d", (int)effective_time);
    }
  }

  return;
}

/*===========================================================================

FUNCTION      rr_ms_connected_gas_id

DESCRIPTION   Returns the GAS ID of the first data space that is CS connected

DEPENDENCIES  None

RETURN VALUE  GAS ID of connected stack, or GERAN_ACCESS_STRATUM_ID_UNDEFINED

SIDE EFFECTS  None

===========================================================================*/
static gas_id_t rr_ms_connected_gas_id(void)
{
  int as_index;

  for (as_index = GERAN_DATA_SPACE_INDEX_1; as_index < NUM_GERAN_DATA_SPACES; as_index++)
  {
    if (rr_multisim_info.gas_data[as_index].cs_connected)
    {
      /* this stack has a CS connection */
      return(GERAN_MAP_DATA_SPACE_INDEX_TO_GAS_ID(as_index));
    }
  }

  return(GERAN_ACCESS_STRATUM_ID_UNDEFINED);
}

/*===========================================================================

FUNCTION      rr_ms_cs_released

DESCRIPTION   Used internally by this module to monitor CS connection release

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/
static void rr_ms_cs_released(const gas_id_t gas_id)
{
  /* this GAS ID has just released a dedicated mode connection */
  rr_ms_cs_connected(FALSE, gas_id);

  /* reset the failed block counts for all stacks, */
  /* to avoid unnecessary DSF reselection attempts */
  rr_ms_reset_failed_blocks();

  return;
}

/*===========================================================================

FUNCTION     rr_ms_check_required_l1_response

DESCRIPTION  Checks whether GRR was waiting for a specific response from GL1

DEPENDENCIES None

RETURN VALUE TRUE if the specified GAS ID was waiting for this response

SIDE EFFECTS May trigger resumption procedure for the specified GAS ID

===========================================================================*/
static boolean rr_ms_check_required_l1_response(const rr_l1_message_id_enum_T rcvd_msg, rr_ms_executor_t *exec_ptr, const gas_id_t gas_id)
{
  boolean bMatched;
  int     as_index;

  /* check the response only if it is flagged to be processed further */
  if (RR_PROCESS_MSG_NEVER == exec_ptr->process_in_rr)
  {
    MSG_GERAN_HIGH_1_G("GRR is ignoring GL1 msg 0x%02X", (int)rcvd_msg);
    return(FALSE);
  }

  /* determine the appropriate data space index */
  as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  /* by default, no additional action needs to occur */
  exec_ptr->next_action = RR_MS_NO_ACTION;

  if (rcvd_msg == rr_multisim_info.gas_data[as_index].required_l1_message)
  {
    /* this response matches the one GRR is waiting for */
    (void)rr_timer_cancel(RR_DUAL_SIM_INTERFACE_TIMER, gas_id);
    rr_multisim_info.gas_data[as_index].required_l1_message = RR_MS_INVALID_L1_MESSAGE;
    bMatched = TRUE;

    /* if the AS had already been suspended, now is the time to resume it */
    if (rr_multisim_info.gas_data[as_index].gl1_interface_state != RR_MS_INTERFACE_ACTIVE)
    {
      exec_ptr->next_action = RR_MS_PREPARE_FOR_RESUMPTION;
    }

    /* log an appropriate event in the debug buffer */
    rr_ms_store_debug_event(RR_MS_RESPONSE_RECEIVED, gas_id);

    MSG_GERAN_HIGH_1_G("Received GL1 msg 0x%02X", (int)rcvd_msg);
  }
  else
  {
    bMatched = FALSE;
  }

  return(bMatched);
}

/*===========================================================================

FUNCTION     rr_ms_handle_panic_reset_message

DESCRIPTION  Handles one of the GL1->GRR panic reset primitives

DEPENDENCIES None

RETURN VALUE GAS ID where the signal was received

SIDE EFFECTS None

===========================================================================*/
static gas_id_t rr_ms_handle_panic_reset_message(const rr_cmd_bdy_type *cmd_ptr, rr_ms_executor_t *exec_ptr)
{
  gas_id_t gas_id;

  /* extract the GAS ID */
  if (MPH_RESET_IND == cmd_ptr->message_header.message_id)
  {
    gas_id = cmd_ptr->mph_reset_ind.gas_id;
  }
  else if (MPH_RESET_CNF == cmd_ptr->message_header.message_id)
  {
    gas_id = cmd_ptr->mph_reset_cnf.gas_id;
  }
  else
  {
    gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
  }

  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    int as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    /* cancel the timers that monitor L1 interface responses */
    (void)rr_timer_cancel(RR_DUAL_SIM_INTERFACE_TIMER, gas_id);
    (void)rr_timer_cancel(RR_DUAL_SIM_TERMINATE_TIMER, gas_id);
    (void)rr_timer_cancel(RR_DUAL_SIM_INACTIVATE_TIMER, gas_id);
    (void)rr_timer_cancel(RR_DUAL_SIM_BPLMN_LIST_TIMER, gas_id);

    rr_ms_acquisition_manager(RR_MS_PRIORITY_INVERSION_INIT, gas_id);

    /* use this opportunity to clear down any local flags */
    rr_multisim_info.gas_data[as_index].aborted_l1_interface = FALSE;
    rr_multisim_info.gas_data[as_index].required_l1_message  = RR_MS_INVALID_L1_MESSAGE;
    rr_multisim_info.gas_data[as_index].last_cleared_l1_msg  = RR_MS_INVALID_L1_MESSAGE;
    rr_multisim_info.gas_data[as_index].failed_blocks        = 0;
    rr_multisim_info.gas_data[as_index].gprs_test_mode       = FALSE;
    rr_multisim_info.gas_data[as_index].ps_abort_delay       = 0;
    (void)rr_timer_cancel(RR_DUAL_SIM_PS_ABORT_TIMER_1, gas_id);
    (void)rr_timer_cancel(RR_DUAL_SIM_PS_ABORT_TIMER_2, gas_id);

    /* ensure that tuneaway blocking status reverts to default */
    geran_api_set_tuneaway_blocked(GERAN_API_CLIENT_RESET, FALSE, geran_map_gas_id_to_nas_id(gas_id));
  }

  /* next action is to ensure the access stratum is resumed */
  exec_ptr->next_action = RR_MS_PREPARE_FOR_RESUMPTION;

  return(gas_id);
}

/*===========================================================================

FUNCTION     rr_ms_type_approval_active

DESCRIPTION  Determines whether TYPE_APPROVAL operations are being exercised
             in Multi-Standby mode (i.e. either DSDS or TSTS)

DEPENDENCIES None

RETURN VALUE TRUE if the software detects G+G (or G+G+G) multi-SIM activity
             with all active stacks configured with Type Approval SIM cards

SIDE EFFECTS None

===========================================================================*/
static boolean rr_ms_type_approval_active(void)
{
#ifdef FEATURE_DUAL_SIM_TYPE_APPROVAL_SET
  if (geran_in_multi_standby_mode())
  {
    gas_id_t gas_id;
    byte     op_mode_list[NUM_GERAN_DATA_SPACES];
    int      ta_count = 0;

    /* retrieve the MS operation mode values (defined on each SIM) */
    rr_ms_get_sim_operation_modes(op_mode_list, NUM_GERAN_DATA_SPACES);

    for (gas_id = GERAN_ACCESS_STRATUM_ID_1; gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; gas_id++)
    {
      /* check whether GL1 has been activated on this stack */
      if (rr_mode_gsm_active_or_starting(gas_id))
      {
        /* check whether the corresponding SIM has the Type Approval bit set */
        if ((op_mode_list[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)] & 0x80) == 0x00)
        {
          /* active stack but without Type Approval bit set */
          return(FALSE);
        }
        else
        {
          ta_count++;
        }
      }
    }

    /* check if there are 2 or more active GERAN stacks with Type Approval bit set on the SIM */
    if (ta_count >= 2)
    {
      return(TRUE);
    }
  }
#endif /*FEATURE_DUAL_SIM_TYPE_APPROVAL_SET*/
  return(FALSE);
}

/*===========================================================================

FUNCTION     rr_ms_gprs_test_mode_state_change

DESCRIPTION  Performs DSDS-specific actions when GPRS Test Mode activity occurs

DEPENDENCIES NONE

RETURN VALUE VOID

SIDE EFFECTS May extend the Sys Info refresh timer on the non-GPRS subscription

===========================================================================*/
static void rr_ms_gprs_test_mode_state_change(const boolean gtm_started, const gas_id_t gas_id)
{
  /* record the new state of GPRS Test Mode on the specified subscription */
  rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].gprs_test_mode = gtm_started;

  /* GPRS Test Mode activity has occurred on a GERAN stack */
  /* so GRR must also avoid sys info refresh activities on */
  /* any other stack which needs access to the transceiver */
  if (RR_MULTI_SIM_MODE_ENABLED)
  {
    gas_id_t other_gas_id;

    for (other_gas_id = GERAN_ACCESS_STRATUM_ID_1; other_gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; other_gas_id++)
    {
      if (other_gas_id != gas_id)
      {
        /* cancel the sys info refresh timer on the other stack */
        if (rr_timer_cancel(GRR_SYS_INFO_REFRESH_TIMER, other_gas_id))
        {
          /* and then restart it with an extended duration if it was already running */
          if (rr_timer_start(GRR_SYS_INFO_REFRESH_TIMER, GRR_TIMEOUT_120_SECONDS, other_gas_id))
          {
            MSG_GERAN_HIGH_1("gs%1d: Deferring sys info refresh timer", 1+(int)other_gas_id);
          }
        }
      }
    }
  }

  return;
}

/*===========================================================================

FUNCTION     rr_ms_get_timestamp

DESCRIPTION  Gets the current slow clock tick count, adjusted for local use

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static timetick_type rr_ms_get_timestamp(const timetick_type input_time)
{
  timetick_type output_time;

  /* query the current timetick (slow clock) count */
  output_time = input_time;
  if (output_time == RR_MS_INVALID_TIME_STAMP)
  {
    /* increment the timestamp so that zero can be used as an invalid value */
    output_time++;
  }

  return(output_time);
}

/*===========================================================================

FUNCTION     rr_ms_two_stacks_have_same_paging_group

DESCRIPTION  Checks whether two stacks have the same (valid) paging group

DEPENDENCIES Relies on paging group data stored in this module

RETURN VALUE TRUE if the stored paging groups are equal, and valid

SIDE EFFECTS None

===========================================================================*/
static boolean rr_ms_two_stacks_have_same_paging_group(const gas_id_t gas_id_1st, const gas_id_t gas_id_2nd)
{
  int as_index_1 = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id_1st);
  int as_index_2 = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id_2nd);

  if ((rr_multisim_info.gas_data[as_index_1].paging_group == rr_multisim_info.gas_data[as_index_2].paging_group) &&
      (rr_multisim_info.gas_data[as_index_1].paging_group != RR_MS_INVALID_PAGING_GROUP))
  {
    return(TRUE);
  }

  return(FALSE);
}

/*===========================================================================

FUNCTION      rr_ms_two_stacks_have_same_lai

DESCRIPTION   Checks whether two GERAN access strata are in the same Location Area

DEPENDENCIES  None

RETURN VALUE  TRUE if two stacks have the same Location Area Information

SIDE EFFECTS  None

===========================================================================*/
boolean rr_ms_two_stacks_have_same_lai(const gas_id_t gas_id_1st, const gas_id_t gas_id_2nd)
{
  boolean same_lai = FALSE;

  if (rr_mode_gsm_active_or_starting(gas_id_1st) &&
      rr_mode_gsm_active_or_starting(gas_id_2nd))
  {
    gprs_scell_info_t *scell_1_ptr = rr_gprs_get_scell_info(gas_id_1st);
    gprs_scell_info_t *scell_2_ptr = rr_gprs_get_scell_info(gas_id_2nd);

    if ((scell_1_ptr != NULL) && (scell_2_ptr != NULL))
    {
       same_lai = rr_same_lai(&(scell_1_ptr->gsm.location_area_identification),
                              &(scell_2_ptr->gsm.location_area_identification));
    }
  }

  return(same_lai);
}
 
/*===========================================================================

FUNCTION     rr_ms_other_stack_has_same_lai

DESCRIPTION  Briefly checks if there are any other satck with Same LAI

DEPENDENCIES None

RETURN VALUE TRUE id other stacks in same LAI else FALSE

SIDE EFFECTS None

===========================================================================*/
boolean rr_ms_other_stack_has_same_lai(const gas_id_t gas_id)
{
  gas_id_t other_gas_id;
  for (other_gas_id = GERAN_ACCESS_STRATUM_ID_1; other_gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; other_gas_id++)
  {
    if (other_gas_id != gas_id)
    {
       if (rr_ms_two_stacks_have_same_lai(gas_id, other_gas_id))
       {
         return TRUE;    
       }
     }
  }
  return FALSE; 
}

/*===========================================================================

FUNCTION     rr_ms_page_in_sharing_window

DESCRIPTION  Calculates delta between missed page and a PCH decode event

DEPENDENCIES None

RETURN VALUE TRUE if the missed page event occurred within 80ms of the PCH

SIDE EFFECTS None

===========================================================================*/
static boolean rr_ms_page_in_sharing_window(const timetick_type pch_decode_time, const timetick_type pch_missed_time, const timetick_type page_window)
{
  if ((pch_decode_time != RR_MS_INVALID_TIME_STAMP) &&
      (pch_missed_time != RR_MS_INVALID_TIME_STAMP) &&
      (page_window     != 0))
  {
    timetick_type elapsed_ms;

    if (pch_missed_time > pch_decode_time)
    {
      /* missed page was reported for a DRX cycle in the future */
      elapsed_ms = timetick_diff(pch_decode_time, pch_missed_time, T_MSEC);
    }
    else
    {
      /* missed page was reported prior to the last PCH time */
      elapsed_ms = timetick_diff(pch_missed_time, pch_decode_time, T_MSEC);
    }

    /* output debug for calculations */
    MSG_GERAN_HIGH_3("Page at time %d Miss at time %d Elapsed ms %d",
             (int)pch_decode_time, (int)pch_missed_time, (int)elapsed_ms);

    return((boolean)(elapsed_ms < page_window));
  }

  return(FALSE);
}

/*===========================================================================

FUNCTION     rr_ms_share_paging_message

DESCRIPTION  Checks whether a page message can be shared between stacks

DEPENDENCIES Page must have already been cached for the source GAS ID

RETURN VALUE VOID

SIDE EFFECTS TRUE if a page message was shared

===========================================================================*/
static boolean rr_ms_share_paging_message(const gas_id_t dest_gas_id, const gas_id_t source_gas_id)
{
  /* page sharing is only valid for GERAN stacks camped in the same Location Area */
  if (rr_ms_two_stacks_have_same_lai(dest_gas_id, source_gas_id))
  {
    /* determine a data space index for each GAS ID */
    int source_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(source_gas_id);
    int dest_index   = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(dest_gas_id);

    /* determine whether the missed page event lies in the acceptable window */
    /* which has been calculated using the destination client's paging cycle */
    if (rr_ms_page_in_sharing_window(rr_multisim_info.gas_data[source_index].buffered_page_time,
                                     rr_multisim_info.gas_data[dest_index].missed_page,
                                     rr_multisim_info.gas_data[dest_index].page_cycle))
    {
      dl_unit_data_ind_T *msg_ptr;
      dl_unit_data_ind_T  shared_page;
      gprs_scell_info_t  *scell_ptr = rr_gprs_get_scell_info(dest_gas_id);

      /* copy the page message so that the other stack can use it */
      msg_ptr = &(rr_multisim_info.gas_data[source_index].buffered_page_msg);
      shared_page = *(msg_ptr);

      shared_page.shared_msg = TRUE;
      shared_page.gas_id     = dest_gas_id;

      if (scell_ptr != NULL)
      {
        /* This code copes with two scenarios affecting OPB (Own Paging Block): */
        /* 1) the SIMs are camped on the same cell, but have different paging groups */
        /* => in this case the OPB setting must be FALSE */
        /* 2) the SIMs are on two different cells */
        /* => in this case the OPB setting must also be FALSE, and the serving ARFCN must be set correctly */
        /* The OPB setting is important because it influences paging mode - it is NOT desirable to force both */
        /* stacks into paging reorganisation if a cached paged is shared due to this DSDS optimisation */
        if (ARFCNS_EQUAL(msg_ptr->channel_number, scell_ptr->gsm.BCCH_ARFCN))
        {
          /* both stacks are on the same serving cell, but page groups could be different */
          if (rr_ms_two_stacks_have_same_paging_group(dest_gas_id, source_gas_id) == FALSE)
          {
            shared_page.own_paging_block = FALSE;
          }
        }
        else
        {
          /* cached page was received on a different cell (but in the same Location Area) */
          shared_page.channel_number   = scell_ptr->gsm.BCCH_ARFCN;
          shared_page.own_paging_block = FALSE;
        }
      } /* end if valid scell */

      /* clear the missed page event and decrement the unknown block count */
      rr_multisim_info.gas_data[dest_index].missed_page = RR_MS_INVALID_TIME_STAMP;
      if (rr_multisim_info.gas_data[dest_index].failed_blocks >= RR_MS_BLOCK_QUALITY_UNIT)
      {
        rr_multisim_info.gas_data[dest_index].failed_blocks -= RR_MS_BLOCK_QUALITY_UNIT;
      }

      /* and then post the shared page to the RR task queue */
      if (GS_SUCCESS == gs_send_message(GS_QUEUE_RR, (void *)&shared_page, FALSE))
      {
        return(TRUE);
      }
      else
      {
        MSG_ERROR("Unable to send msg for shared PCH decode",0,0,0);
      }
    } /* end if in page sharing window */
  } /* end if same LAI */

  return(FALSE);
}

/*===========================================================================

FUNCTION     rr_ms_page_message_received

DESCRIPTION  Checks whether a DL_UNIT_DATA_IND can be shared between stacks

DEPENDENCIES None

RETURN VALUE TRUE if the DL_UNIT_DATA_IND was originally generated by GL2

SIDE EFFECTS None

===========================================================================*/
static boolean rr_ms_page_message_received(const dl_unit_data_ind_T *msg_ptr)
{
  boolean msg_rcvd_from_gl2 = TRUE;

  if (msg_ptr != NULL)
  {
    if (msg_ptr->shared_msg)
    {
      /* this page message has been shared by the other stack */
      /* i.e. it was not generated from OTA data given to GL2 */
      msg_rcvd_from_gl2 = FALSE;
    }

    /* filter out any message types that cannot be idle mode paging */
    if ((msg_ptr->own_paging_block == TRUE)  &&
        (msg_rcvd_from_gl2         == TRUE)  &&
        (msg_ptr->short_header     == FALSE) &&
        (msg_ptr->l2_channel_type  != SACCH))
    {
      byte msg_type;

      /* check whether this is actually a page message */
      msg_type = msg_ptr->layer3_message[2];
      if ((PAGING_REQUEST_TYPE_1 == msg_type) ||
          (PAGING_REQUEST_TYPE_2 == msg_type) ||
          (PAGING_REQUEST_TYPE_3 == msg_type))
      {
        gas_id_t source_gas_id; /* the stack for which the message was actually decoded by GL1 */
        gas_id_t gas_id;        /* access stratum IDs for one of the other possible recipients */
        int      source_index;

        /* buffer the incoming page and the record the time it arrived */
        source_gas_id = msg_ptr->gas_id;
        source_index  = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(source_gas_id);
        rr_multisim_info.gas_data[source_index].buffered_page_time = rr_ms_get_timestamp(timetick_get());
        rr_multisim_info.gas_data[source_index].buffered_page_msg  = *(msg_ptr);

        /* loop through all other subscriptions and check whether any of them require the newly cached message */
        for (gas_id = GERAN_ACCESS_STRATUM_ID_1; gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; gas_id++)
        {
          if (gas_id != source_gas_id)
          {
            if (rr_ms_share_paging_message(gas_id, source_gas_id))
            {
              MSG_GERAN_HIGH_1_G("Using PCH read by gs%1d", 1+(int)source_gas_id);
            }
          } /* end GAS ID check */
        } /* end for loop */
      } /* end if paging message */
    } /* end if channel criteria */
  } /* end if null pointer */

  return(msg_rcvd_from_gl2);
}

#ifdef FEATURE_TRIPLE_SIM
/*===========================================================================

FUNCTION     rr_ms_prioritise_page_sources

DESCRIPTION  Generates a prioritised list of GAS IDs. The first ID in the
             list represents the best candidate for page sharing for the
             "destination" GAS ID specified as the second parameter.

DEPENDENCIES No prioritisation is required for trivial DSDS scenarios so
             this function only exists when TSTS is compiled in the build

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_prioritise_page_sources(gas_id_t *gas_id_list, const gas_id_t gas_id)
{
  gas_id_t *best_ptr;
  gas_id_t *curr_ptr;
  gas_id_t  source_gas_id;
  int       best_pri;
  int       curr_pri;
  int       as_index;

  /* start with the list of GAS IDs in default numerical order */
  for (as_index = 0; as_index < NUM_GERAN_DATA_SPACES; as_index++)
  {
    gas_id_list[as_index] = GERAN_MAP_DATA_SPACE_INDEX_TO_GAS_ID(as_index);
  }

  /* initialise "best so far" tracking variables */
  best_ptr = NULL;
  best_pri = RR_PAGE_SOURCE_PRIORITY_LOW;
  curr_ptr = gas_id_list;

  /* check whether any particular GAS ID has criteria that makes it high priority */
  for (as_index = 0; as_index < NUM_GERAN_DATA_SPACES; as_index++, curr_ptr++)
  {
    source_gas_id = *curr_ptr;

    /* calculate priority for the current entry if it is NOT the requesting stack */
    if (source_gas_id != gas_id)
    {
      gprs_scell_info_t *src_scell_ptr = rr_gprs_get_scell_info(source_gas_id);
      gprs_scell_info_t *dst_scell_ptr = rr_gprs_get_scell_info(gas_id);

      /* default priority is LOW */
      curr_pri = RR_PAGE_SOURCE_PRIORITY_LOW;

      if ((src_scell_ptr != NULL) && (dst_scell_ptr != NULL))
      {
        if (ARFCNS_EQUAL(src_scell_ptr->gsm.BCCH_ARFCN, dst_scell_ptr->gsm.BCCH_ARFCN))
        {
          if (rr_ms_two_stacks_have_same_paging_group(source_gas_id, gas_id))
          {
            /* camped on same cell and same paging group, high priority */
            curr_pri = RR_PAGE_SOURCE_PRIORITY_HIGH;
          }
          else
          {
            /* camped on same cell, different paging groups, medium priority */
            curr_pri = RR_PAGE_SOURCE_PRIORITY_MED;
          }
        }
      }

      /* check whether this is the highest priority found so far */
      if (curr_pri > best_pri)
      {
        best_pri = curr_pri;
        best_ptr = curr_ptr;
      }
    }
  }

  /* if the highest priority entry is NOT at the top of the list, swap it */
  curr_ptr = gas_id_list;
  if ((best_ptr != NULL) &&
      (curr_ptr != NULL) &&
      (best_ptr != curr_ptr))
  {
    source_gas_id = *best_ptr;
    *best_ptr     = *curr_ptr;
    *curr_ptr     = source_gas_id;
    MSG_GERAN_HIGH_1_G("Prioritised gs%1d as shared page source", 1+(int)source_gas_id);
  }

  return;
}
#endif /* FEATURE_TRIPLE_SIM */

/*===========================================================================

FUNCTION     rr_ms_page_message_missed

DESCRIPTION  Checks whether PCH missed on one stack can be supplied by another

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_page_message_missed(const timetick_type pch_missed_time, const gas_id_t gas_id)
{
  gas_id_t source_list[NUM_GERAN_DATA_SPACES];
  gas_id_t source_gas_id;
  int      as_index;

  /* record the timestamp provided by GL1 within the MPH_BLOCK_QUALITY_IND message */
  as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
  rr_multisim_info.gas_data[as_index].missed_page = rr_ms_get_timestamp(pch_missed_time);

  MSG_GERAN_HIGH_1_G("Missed PCH at time %d", (int)pch_missed_time);

#ifdef FEATURE_TRIPLE_SIM
  /* generate prioritised list of source GAS IDs */
  rr_ms_prioritise_page_sources(source_list, gas_id);
#else
  /* list of GAS IDs always in default numerical order */
  for (as_index = 0; as_index < NUM_GERAN_DATA_SPACES; as_index++)
  {
    source_list[as_index] = GERAN_MAP_DATA_SPACE_INDEX_TO_GAS_ID(as_index);
  }
#endif /* FEATURE_TRIPLE_SIM */

  for (as_index = 0; as_index < NUM_GERAN_DATA_SPACES; as_index++)
  {
    /* extract the next candidate GAS ID from the prioritised list */
    source_gas_id = source_list[as_index];

    if (source_gas_id != gas_id)
    {
      if (rr_ms_share_paging_message(gas_id, source_gas_id))
      {
        MSG_GERAN_HIGH_1_G("Using PCH read by gs%1d instead", 1+(int)source_gas_id);
        /* only need to acquire the data from one source, exit as soon as found */
        return;
      }
    } /* end GAS ID check */
  } /* end for loop */

  return;
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*===========================================================================

FUNCTION     rr_ms_configure_page_skip

DESCRIPTION  Calls GRM to enable/disable adaptive page skipping feature.

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
#if defined(PERLUTF) || defined(TEST_FRAMEWORK)
#error code not present
#else
static void rr_ms_configure_page_skip(const boolean aps_state, const gas_id_t gas_id)
{
  /* Interfaces to TRM are owned by Layer 1 (using GRM wrappers in GCOMMON) */
  grm_update_aps_support(aps_state, gas_id);

  return;
}
#endif /* PERLUTF */

/*===========================================================================

FUNCTION     rr_ms_check_page_skipping

DESCRIPTION  Calls TRM API to enable/disable adaptive page skipping feature.

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_check_page_skipping(const gl1_block_quality_t block_quality, const gas_id_t gas_id)
{
  int as_index;

  as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  if ((GL1_BLOCK_QUALITY_GOOD == block_quality)
    && (NORMAL_PAGING == rr_ce_get_ota_page_mode(gas_id))
    && !rr_gprs_non_drx_mode_requested(gas_id)
    && !rr_is_control_state(RR_CONNECTION_PENDING, gas_id)
    && !rr_gprs_is_dsc_below_max_value(gas_id))
  {
    rr_ms_configure_page_skip(TRUE, gas_id);
  }
  else
  {
    rr_ms_configure_page_skip(FALSE, gas_id);
  }

  return;
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*===========================================================================

FUNCTION     rr_ms_process_block_quality_report

DESCRIPTION  Handles MPH_BLOCK_QUALITY_IND reports from GSM Layer 1

DEPENDENCIES None

RETURN VALUE GAS ID specified by GSM L1 within the incoming message

SIDE EFFECTS May deliberately prevent the signal reaching the main GRR task

===========================================================================*/
static gas_id_t rr_ms_process_block_quality_report(const mph_block_quality_ind_T *block_ptr, rr_ms_executor_t *exec_ptr)
{
  gas_id_t rcvd_id;

  if ((block_ptr != NULL) && (exec_ptr != NULL))
  {
    gas_id_t gas_id = block_ptr->gas_id;

    /* discard idle block quality reports during Type Approval testing */
    if ((rr_ms_type_approval_active() == TRUE)    &&
        (block_ptr->l2_channel_type   != L2_RACH) &&
        (block_ptr->l2_channel_type   != SACCH))
    {
      /* all GSM stacks have been put into TYPE_APPROVAL mode, */
      /* so prevent the block quality message from reaching RR */
      /* since a DSF procedure in this scenario is undesirable */
      rcvd_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
    }
    else
    {
      /* use the GAS ID from the incoming message */
      rcvd_id = gas_id;
    }

    /* if there is no valid GAS ID, avoid further processing */
    if (rcvd_id > GERAN_ACCESS_STRATUM_ID_MAX)
    {
      /* set up the output parameters to avoid further processing */
      exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
      exec_ptr->next_action   = RR_MS_NO_ACTION;
      MSG_GERAN_HIGH_0_G("MPH_BLOCK_QUALITY_IND ignored in multi-SIM mode");
    }
    else
    {
      if (GL1_BLOCK_QUALITY_UNKNOWN == block_ptr->block_quality)
      {
        rr_log_page_missed(gas_id);

        /* this may use a cached page to serve the missing PCH */
        rr_ms_page_message_missed(block_ptr->block_timestamp, rcvd_id);
      }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
      if ((block_ptr->l2_channel_type != L2_RACH) && (block_ptr->l2_channel_type != SACCH))
      {
        rr_ms_check_page_skipping(block_ptr->block_quality, gas_id);
      }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
    }
  }
  else
  {
    /* Pointer validation failed, return an undefined GAS ID to indicate an error */
    rcvd_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
  }

  return(rcvd_id);
}

/*===========================================================================

FUNCTION     rr_ms_process_gl1_msg

DESCRIPTION  Extracts the access stratum ID from an incoming GL1 message

DEPENDENCIES None

RETURN VALUE GAS ID

SIDE EFFECTS Cancels the L1 interface guard timer

===========================================================================*/
static gas_id_t rr_ms_process_gl1_msg(const rr_cmd_bdy_type *cmd_ptr, rr_ms_executor_t *exec_ptr)
{
  gas_id_t gas_id;

  switch(cmd_ptr->message_header.message_id)
  {
  case MPH_FREQUENCY_REDEFINITION_CNF:
  case MPH_SPECIFIC_BCCH_DECODE_FAILURE_IND:
    /* legacy interface, no longer supported */
    gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
    break;

  case MPH_L1_DS_ABORT_IND:
    gas_id = cmd_ptr->mph.l1_ds_abort_ind.gas_id;
    break;

  case MPH_CHANNEL_ASSIGN_CNF:
    gas_id = cmd_ptr->mph_channel_assign_cnf.gas_id;
    break;

  case MPH_BLOCK_QUALITY_IND:
    gas_id = rr_ms_process_block_quality_report(&(cmd_ptr->mph_block_quality_ind), exec_ptr);
    break;

  case MPH_CHANNEL_MODE_MODIFY_CNF:
    gas_id = cmd_ptr->mph_channel_mode_modify_cnf.gas_id;
    break;

  case MPH_CHANNEL_RELEASE_CNF:
    gas_id = cmd_ptr->mph_channel_release_cnf.gas_id;
    rr_ms_cs_released(gas_id);
    break;

  case MPH_HANDOVER_CNF:
    gas_id = cmd_ptr->mph_handover_cnf.gas_id;
    break;

  case MPH_HANDOVER_IND:
    gas_id = cmd_ptr->mph_handover_ind.gas_id;
    break;

  case MPH_IMMED_ASSIGN_CNF:
    gas_id = cmd_ptr->mph_immed_assign_cnf.gas_id;
    rr_ms_cs_connected(TRUE, gas_id);
    rr_ms_cs_establishment_confirmed(gas_id);
    break;

  case MPH_RECONNECT_CHANNEL_CNF:
    gas_id = cmd_ptr->mph_reconnect_channel_cnf.gas_id;
    rr_ms_cs_connected(TRUE, gas_id);
    break;

  case MPH_SERVING_DED_MEAS_IND:
    gas_id = cmd_ptr->mph_serving_ded_meas_ind.gas_id;
    break;

  case MPH_SERVING_IDLE_MEAS_IND:
    gas_id = cmd_ptr->mph_serving_idle_meas_ind.gas_id;
    break;

  case MPH_SET_CIPHERING_CNF:
    gas_id = cmd_ptr->mph_set_ciphering_cnf.gas_id;
    break;

  case MPH_STOP_HANDOVER_ACCESS_CNF:
    gas_id = cmd_ptr->mph_stop_handover_access_cnf.gas_id;
    break;

  case MPH_SURROUND_MEAS_IND:
    gas_id = cmd_ptr->mph_surround_meas_ind.gas_id;
    break;

  case MPH_RESELECTION_CNF:
    gas_id = cmd_ptr->mph_reselection_cnf.gas_id;
    (void)rr_ms_check_required_l1_response(MPH_RESELECTION_CNF, exec_ptr, gas_id);
    break;

  case MPH_DECODE_BCCH_LIST_CNF:
    gas_id = cmd_ptr->mph_decode_bcch_list_cnf.gas_id;
    (void)rr_ms_check_required_l1_response(MPH_DECODE_BCCH_LIST_CNF, exec_ptr, gas_id);
    break;

  case MPH_DECODE_BCCH_LIST_FAILURE_IND:
    gas_id = cmd_ptr->mph_decode_bcch_list_failure_ind.gas_id;
    /* @@TODO insert System Info Cache retrieval here */
    (void)rr_ms_check_required_l1_response(MPH_DECODE_BCCH_LIST_CNF, exec_ptr, gas_id);
    break;

  case MPH_POWER_SCAN_CNF:
    gas_id = cmd_ptr->mph_power_scan_cnf.gas_id;
    (void)rr_ms_check_required_l1_response(MPH_POWER_SCAN_CNF, exec_ptr, gas_id);
    break;

  case MPH_SELECT_SPECIFIC_BCCH_CNF:
    gas_id = cmd_ptr->mph_select_specific_bcch_cnf.gas_id;
    if (rr_ms_check_required_l1_response(MPH_SELECT_SPECIFIC_BCCH_CNF, exec_ptr, gas_id) == FALSE)
    {
      /* GL1 may have responded to a reselection, so perform further check */
      (void)rr_ms_check_required_l1_response(MPH_RESELECTION_CNF, exec_ptr, gas_id);
    }
    break;

  case MPH_FCCH_SCH_DECODE_IND:
    /* this legacy interface is used to gauge whether GL1 is operating */
    gas_id = cmd_ptr->mph_fcch_sch_decode_ind.gas_id;
    (void)rr_ms_check_required_l1_response(MPH_DECODE_BCCH_LIST_CNF, exec_ptr, gas_id);
    MSG_GERAN_HIGH_3_G("MPH_FCCH_SCH_DECODE_IND for ARFCN %d BSIC <%d,%d>",
                       (int)(cmd_ptr->mph_fcch_sch_decode_ind.BCCH_ARFCN.num),
                       (int)(cmd_ptr->mph_fcch_sch_decode_ind.BSIC.PLMN_colour_code),
                       (int)(cmd_ptr->mph_fcch_sch_decode_ind.BSIC.BS_colour_code));
    break;

  case MPH_SURROUND_UPDATE_FAILURE_IND:
    gas_id = cmd_ptr->mph_surround_update_failure_ind.gas_id;
    /* @@TODO insert System Info Cache retrieval here */
    break;

  case MPH_SERVING_AUX_MEAS_IND:
    gas_id = cmd_ptr->mph_serving_aux_meas_ind.gas_id;
    break;

  case MPH_READ_SYS_INFO_FAILURE_IND:
    gas_id = cmd_ptr->mph_read_sys_info_failure_ind.gas_id;
    break;

  case MPH_FREQUENCY_REDEFINITION_IND:
    gas_id = cmd_ptr->mph_frequency_redefinition_ind.gas_id;
    break;

  case MPH_START_GSM_MODE_CNF:
    gas_id = cmd_ptr->mph_start_gsm_mode_cnf.gas_id;
    break;

  case MPH_STOP_GSM_MODE_CNF:
    gas_id = cmd_ptr->mph_stop_gsm_mode_cnf.gas_id;
    if (rr_timer_cancel(RR_DUAL_SIM_INACTIVATE_TIMER, gas_id))
    {
      MSG_GERAN_HIGH_0_G("Cancelled GRR deactivation timer");
    }
    break;

  case MPH_NC_MEASUREMENT_IND:
    gas_id = cmd_ptr->mph_nc_measurement_ind.gas_id;
    break;

  case MPH_SERVING_MEAS_IND:
    gas_id = cmd_ptr->mph_serving_meas_ind.gas_id;
    break;

#ifdef FEATURE_WCDMA
  case MPH_SURROUND_WCDMA_IDLE_MEAS_IND:
    gas_id = cmd_ptr->mph_surround_wcdma_idle_meas_ind.gas_id;
    break;

  case MPH_SURROUND_WCDMA_DEDICATED_MEAS_IND:
    gas_id = cmd_ptr->mph_surround_wcdma_dedicated_meas_ind.gas_id;
    break;

  case MPH_SURROUND_WCDMA_XFER_MEAS_IND:
    gas_id = cmd_ptr->mph_surround_wcdma_xfer_meas_ind.gas_id;
    break;
#endif /* FEATURE_WCDMA */

#if defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  case MPH_SURROUND_IRAT_MEAS_IND:
    {
      mph_surround_irat_meas_ind_t *meas_ptr;

      meas_ptr = (mph_surround_irat_meas_ind_t *)cmd_ptr;
      gas_id   = meas_ptr->gas_id;
    }
    break;
#endif /* FEATURE_LTE || FEATURE_GSM_TDS */

  case MPH_START_IDLE_MODE_CNF:
    gas_id = cmd_ptr->mph_start_idle_mode_cnf.gas_id;
    break;

#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
  case MPH_SUSPEND_GSM_MODE_CNF:
    gas_id = cmd_ptr->mph_suspend_gsm_mode_cnf.gas_id;
    geran_api_set_tuneaway_blocked(GERAN_API_CLIENT_G2X, TRUE, geran_map_gas_id_to_nas_id(gas_id));
    break;

  case MPH_RESUME_GSM_MODE_CNF:
    gas_id = cmd_ptr->mph_resume_gsm_mode_cnf.gas_id;
    geran_api_set_tuneaway_blocked(GERAN_API_CLIENT_G2X, FALSE, geran_map_gas_id_to_nas_id(gas_id));
    break;

  case MPH_INTERRAT_LIST_SRCH_TIME_IND:
    gas_id = cmd_ptr->mph_interrat_list_srch_time_ind.gas_id;
    break;

  case MPH_SUSPEND_GSM_MODE_IND:
    gas_id = cmd_ptr->mph_suspend_gsm_mode_ind.gas_id;
    /* clear any pending interface tracking */
    if (FALSE == rr_ms_check_required_l1_response(MPH_POWER_SCAN_CNF, exec_ptr, gas_id))
    {
      (void)rr_ms_check_required_l1_response(MPH_DECODE_BCCH_LIST_CNF, exec_ptr, gas_id);
    }
    /* restore PIMMs to an initialised state for this subscription */
    rr_ms_acquisition_manager(RR_MS_PRIORITY_INVERSION_INIT, gas_id);
    break;

  case MPH_START_GSM_MODE_FOR_BPLMN_CNF:
    gas_id = cmd_ptr->mph_start_gsm_mode_for_bplmn_cnf.gas_id;
    break;

  case MPH_RESUME_GSM_MODE_FOR_BPLMN_CNF:
    gas_id = cmd_ptr->mph_resume_gsm_mode_for_bplmn_cnf.gas_id;
    break;
#endif /* FEATURE_GTOW_BACKGROUND_PLMN_SEARCH */

#ifdef FEATURE_GSM_DTM
  case MPH_DTM_CHANNEL_ASSIGN_CNF:
    gas_id = cmd_ptr->mph_dtm_channel_assign_cnf.gas_id;
    rr_ms_cs_connected(TRUE, gas_id);
    break;

  case MPH_DTM_CHANNEL_RELEASE_IND:
    gas_id = cmd_ptr->mph_dtm_channel_release_ind.gas_id;
    if (cmd_ptr->mph_dtm_channel_release_ind.cs_release == TRUE)
    {
      rr_ms_cs_released(gas_id);
    }
    break;
#endif /* FEATURE_GSM_DTM */

  case MPH_L1_TEST_MODE_START_IND:
    gas_id = cmd_ptr->mph_l1_test_mode_start_ind.gas_id;
    rr_ms_gprs_test_mode_state_change(TRUE, gas_id);
    break;

  case MPH_L1_TEST_MODE_STOP_IND:
    gas_id = cmd_ptr->mph_l1_test_mode_stop_ind.gas_id;
    rr_ms_gprs_test_mode_state_change(FALSE, gas_id);
    break;

  case MPH_ABORT_RA_CNF:
    gas_id = cmd_ptr->mph_abort_ra_cnf.gas_id;
    break;

  case MPH_UE_MODE_CHANGE_CNF:
    gas_id = cmd_ptr->mph_ue_mode_change_cnf.gas_id;
    break;

  case MPH_COEX_MSC_CHANGE_IND:
    gas_id = cmd_ptr->mph.coex_msc_change_ind.gas_id;
    break;

  case MPH_RESET_IND:
  case MPH_RESET_CNF:
    gas_id = rr_ms_handle_panic_reset_message(cmd_ptr, exec_ptr);
    break;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  case MPH_NV_REFRESH_CNF:
    gas_id = cmd_ptr->mph.nv_refresh_cnf.gas_id;
    break;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  default:
    /* this will trap any unhandled messages */
    gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
    break;
  }

  return(gas_id);
}

/*===========================================================================

FUNCTION     rr_ms_process_grr_msg

DESCRIPTION  Extracts the access stratum ID from an incoming GRR message

DEPENDENCIES None

RETURN VALUE GAS ID

SIDE EFFECTS May abort a pending TRM transaction in GL1

===========================================================================*/
static gas_id_t rr_ms_process_grr_msg(const rr_cmd_bdy_type *cmd_ptr, rr_ms_executor_t *exec_ptr)
{
  gas_id_t gas_id;
  boolean  is_abort;

  /* extract the GAS ID from the header */
  gas_id = cmd_ptr->rr.header.gas_id;

  if ((cmd_ptr->rr.header.rr_message_set == RR_CELL_ACQ_IMSG) &&
      (cmd_ptr->rr.header.imh.message_id == RR_CELL_ACQ_IMSG_ABORT_REQ))
  {
    is_abort = TRUE;
  }
  else
  {
    is_abort = FALSE;
  }

  if (is_abort)
  {
    /* check whether this internal abort affects a pending interface */
    exec_ptr->next_action = rr_ms_abort_pending_interface(RR_MS_GRR_ABORT_BY_IMSG, gas_id);
  }

  return(gas_id);
}

/*===========================================================================

FUNCTION      rr_ms_detect_pch_cbch_collisions

DESCRIPTION   Determine whether any stack may be experiencing PCH denials
              due to collisions with high priority CBCH on another stack

DEPENDENCIES  Invoked by calls to rr_ms_update_cell_broadcast_priority()

RETURN VALUE  TRUE if PCH decodes may be failing due to CBCH collision

SIDE EFFECTS  None

===========================================================================*/
static boolean rr_ms_detect_pch_cbch_collisions(void)
{
  int pg_index;
  int cb_index;

  /* scan the GERAN stacks looking for PCH failures within a specific DRX cycle */
  for (pg_index = GERAN_DATA_SPACE_INDEX_1; pg_index < NUM_GERAN_DATA_SPACES; pg_index++)
  {
    if (((RR_DSC_DRX_DURATION_PER_MULTIFRAME << 3) == rr_multisim_info.gas_data[pg_index].page_cycle) &&  /* paging cycle is 8 multiframes */
        ((RR_MS_BLOCK_QUALITY_RATE >> 1)           <= rr_multisim_info.gas_data[pg_index].failed_blocks)) /* consecutive missed PCH blocks */
    {
      /* page cycle aligns with CBCH receive period, failures have occurred */
      /* so check whether CBCH is active on any other stack except this one */
      for (cb_index = GERAN_DATA_SPACE_INDEX_1; cb_index < NUM_GERAN_DATA_SPACES; cb_index++)
      {
        if ((cb_index != pg_index) && (FALSE == rr_multisim_info.gas_data[cb_index].cs_connected))
        {
          if ((RR_MS_CBCH_ENABLED == rr_multisim_info.gas_data[cb_index].cbch_state) ||
              (RR_MS_CBCH_BLOCKED == rr_multisim_info.gas_data[cb_index].cbch_state))
          {
            /* Cell Broadcast has been active on this stack, so collisions may have occurred */
            MSG_GERAN_HIGH_2("gs%1d: PCH denials may be caused by CBCH on gs%1d", pg_index+1, cb_index+1);
            return(TRUE);
          }
        }
      }
    }
  }

  return(FALSE);
}

/*===========================================================================

FUNCTION     rr_ms_update_cell_broadcast_priority

DESCRIPTION  Updates the latest state of priority inversion for CBCH decoding

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_update_cell_broadcast_priority(const boolean idle_mode, const gas_id_t gas_id)
{
  int num_blocked;
  int as_index;

  /* update the state of the calling subscription */
  as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
  if (idle_mode)
  {
    if (RR_MS_CBCH_BLOCKED == rr_multisim_info.gas_data[as_index].cbch_state)
    {
      /* connected mode has ended on this stack, revert to the enabled state */
      rr_multisim_info.gas_data[as_index].cbch_state = RR_MS_CBCH_ENABLED;
    }
  }
  else
  {
    if (RR_MS_CBCH_ENABLED == rr_multisim_info.gas_data[as_index].cbch_state)
    {
      /* idle mode has ended on this stack, transition to the blocked state */
      rr_multisim_info.gas_data[as_index].cbch_state = RR_MS_CBCH_BLOCKED;
    }
  }

  /* spin through all subscriptions and assess the CBCH state for each of them */
  num_blocked = 0;
  for (as_index = GERAN_DATA_SPACE_INDEX_1; as_index < NUM_GERAN_DATA_SPACES; as_index++)
  {
    if (rr_mode_gsm_active_or_starting(GERAN_MAP_DATA_SPACE_INDEX_TO_GAS_ID(as_index)))
    {
      /* count how active stacks are blocked */
      if (RR_MS_CBCH_BLOCKED == rr_multisim_info.gas_data[as_index].cbch_state)
      {
        num_blocked++;
      }
    }
    else
    {
      /* stack is inactive, reset its CBCH state */
      rr_multisim_info.gas_data[as_index].cbch_state = RR_MS_CBCH_OFFLINE;
    }
  }

  if ((num_blocked != 0) || rr_ms_other_sim_in_background_traffic(GERAN_ACCESS_STRATUM_ID_UNDEFINED))
  {
    /* ensure that CBCH won't pre-empt background data traffic on any RAT */
    MSG_GERAN_HIGH_0_G("Data traffic may be active, do not prioritise CBCH");
    GRR_UPDATE_PRIORITY_INVERSION(TRM_HP_BROADCAST, FALSE);
  }
  else
  {
    /* engage priority inversion for CBCH if PCH collisions are not detected */
    MSG_GERAN_HIGH_0_G("High priority CBCH can be enabled, pending collision check");
    GRR_UPDATE_PRIORITY_INVERSION(TRM_HP_BROADCAST, !rr_ms_detect_pch_cbch_collisions());
  }

  return;
}

/*===========================================================================

FUNCTION     rr_ms_process_mac_msg

DESCRIPTION  Determines the access stratum ID for an incoming GMAC message

DEPENDENCIES None

RETURN VALUE GAS ID

SIDE EFFECTS May trigger the completion of PS abort or interface deactivation

===========================================================================*/
static gas_id_t rr_ms_process_mac_msg(const rr_cmd_bdy_type *cmd_ptr, rr_ms_executor_t *exec_ptr)
{
  gas_id_t gas_id;

  switch (cmd_ptr->message_header.message_id)
  {
  case MAC_GRR_MAC_STATUS:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_mac_status.gas_id;
    break;

  case MAC_GRR_DATA_IND:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_data_ind.gas_id;
    break;

  case MAC_GRR_CELL_RESEL_STATUS:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_cell_resel_status.gas_id;
    break;

  case MAC_GRR_PAGING_NOTIFICATION:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_paging_notification.gas_id;
    break;

  case MAC_GRR_ABNORMAL_RELEASE:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_abnormal_release.gas_id;
    break;

  case MAC_GRR_MEAS_RPT_CNF:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_meas_rpt_cnf.gas_id;
    break;

  case MAC_GRR_PCCF_SENT:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_pccf_sent.gas_id;
    break;

  case MAC_GRR_TBF_REJ:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_tbf_rej.gas_id;
    break;

  case MAC_GRR_TBF_ACCEPT:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_tbf_accept.gas_id;
    break;

  case MAC_GRR_UL_TBF_REQ:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_ul_tbf_req.gas_id;
    break;

  case MAC_GRR_STORE_UPDATE:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_store_update.gas_id;
    break;

  case MAC_GRR_RACH_CNF:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_rach_cnf.gas_id;
    break;

  case MAC_GRR_RACH_REJ:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_rach_rej.gas_id;
    break;

  case MAC_GRR_NCELL_DATA_IND:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_ncell_data_ind.gas_id;
    break;

  case MAC_GRR_NCELL_DATA_CNF:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_ncell_data_cnf.gas_id;
    break;

  case MAC_GRR_RESET_CNF:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_reset_cnf.gas_id;
    break;

#ifdef FEATURE_GSM_DTM
  case MAC_GRR_DTM_CHANNEL_ASSIGN_REJ:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_dtm_channel_assign_rej.gas_id;
    break;

#ifdef FEATURE_GSM_EDTM
  case MAC_GRR_EDTM_PKT_CS_EST_REJ:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_edtm_pkt_cs_est_rej.gas_id;
    break;

  case MAC_GRR_EDTM_CS_RELEASE_IND:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_edtm_cs_release_ind.gas_id;
    break;
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GPRS_PS_HANDOVER
  case MAC_GRR_PSHO_CNF:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_psho_cnf.gas_id;
    break;
#endif /* FEATURE_GPRS_PS_HANDOVER */

  case MAC_GRR_PS_ACCESS_CNF:
    gas_id = cmd_ptr->mac_grr_sig.mac_grr_ps_access_cnf.gas_id;
    break;

  default:
    gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
    MSG_GERAN_ERROR_1("Missing GAS ID from GMAC msg %d", (int)(cmd_ptr->message_header.message_id));
    break;
  }

  /* output useful debug that shows where TLLIs are assigned */
#ifdef FEATURE_TRIPLE_SIM
  MSG_GERAN_HIGH_3_G("Rcvd GMAC msg(TLLI0=%1d,TLLI1=%1d,TLLI2=%1d)",
                     rr_ps_tlli_is_assigned(GERAN_ACCESS_STRATUM_ID_1),
                     rr_ps_tlli_is_assigned(GERAN_ACCESS_STRATUM_ID_2),
                     rr_ps_tlli_is_assigned(GERAN_ACCESS_STRATUM_ID_3));
#else
  MSG_GERAN_HIGH_2_G("Rcvd GMAC msg(TLLI0=%1d,TLLI1=%1d)",
                     rr_ps_tlli_is_assigned(GERAN_ACCESS_STRATUM_ID_1),
                     rr_ps_tlli_is_assigned(GERAN_ACCESS_STRATUM_ID_2));
#endif /* FEATURE_TRIPLE_SIM */

  /* perform specific processing of the MAC status message */
  if (cmd_ptr->message_header.message_id == (byte)MAC_GRR_MAC_STATUS)
  {
    boolean mac_idle;

    /* determine whether the message indicates that MAC is now idle */
    mac_idle = (boolean)(MAC_IDLE == cmd_ptr->mac_grr_sig.mac_grr_mac_status.mac_status);

    /* reassess the CBCH prioritisation across all the stacks */
    rr_ms_update_cell_broadcast_priority(mac_idle, gas_id);
  }

  return(gas_id);
}

/*===========================================================================

FUNCTION     rr_ms_gprs_tuneaway_active

DESCRIPTION  Determines whether GPRS Tuneaway is active on the specified stack

DEPENDENCIES None

RETURN VALUE TRUE if a GERAN stack is using the TRM background traffic reason

SIDE EFFECTS None

===========================================================================*/
static boolean rr_ms_gprs_tuneaway_active(const gas_id_t gas_id)
{
#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
  if (gas_id != GERAN_ACCESS_STRATUM_ID_UNDEFINED)
  {
    trm_client_enum_t trm_id;
	
    /* determine the TRM client ID for the specified GAS ID */
	
    trm_id = rr_ms_get_trm_client_id(gas_id);
	
    if (TRM_BG_TRAFFIC == grm_get_reason(trm_id,gas_id))
    {
      /* legacy case */
      return(TRUE);
    }
    else if ((TRM_GSM1 == trm_id) && (TRM_NUM_REASONS != grm_get_reason(TRM_GPRS1,gas_id)))
    {
      return(TRUE);
    }
    else if ((TRM_GSM2 == trm_id) && (TRM_NUM_REASONS != grm_get_reason(TRM_GPRS2,gas_id)))
    {
      return(TRUE);
    }
  }
#endif /* !PERLUTF && !TEST_FRAMEWORK */

  return(FALSE);
}

/*===========================================================================

FUNCTION     rr_ms_sys_info_refresh_blocked

DESCRIPTION  Determines whether the specified stack should allow SI refresh

DEPENDENCIES None

RETURN VALUE TRUE if a normal SI refresh can be allowed to take place.

SIDE EFFECTS None

===========================================================================*/
boolean rr_ms_sys_info_refresh_blocked(const gas_id_t gas_id)
{
  uint32 refresh_duration = rr_nv_get_multisim_si_refresh_duration_milliseconds(RR_GAS_ID_TO_AS_ID);

  /* if the timer duration is set to 0ms then SI refresh is disabled*/
  if (refresh_duration == 0)
  {
    MSG_GERAN_HIGH_0_G("Sys info refresh blocked in multi-standby mode");
    return(TRUE);
  }

  return(FALSE);
}

/*===========================================================================

FUNCTION     rr_ms_handle_interface_timeout

DESCRIPTION  Reacts to the expiry of RR_DUAL_SIM_INTERFACE_TIMER

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_handle_interface_timeout(rr_ms_executor_t *exec_ptr, const gas_id_t gas_id)
{
  /* explicitly reset the timer */
  (void)rr_timer_cancel(RR_DUAL_SIM_INTERFACE_TIMER, gas_id);

  if (exec_ptr != NULL)
  {
    rr_ms_interface_check_t interface_check;

    /* log an appropriate event in the debug buffer */
    rr_ms_store_debug_event(RR_MS_INTERFACE_EXPIRED, gas_id);

    /* check for race condition where interface was aborted while timer is running */
    if (rr_ms_interface_was_aborted(gas_id))
    {
      interface_check = RR_MS_ABORT_IS_REQUIRED;
    }
    else
    {
      interface_check = RR_MS_POLL_FOR_RESPONSE;
    }

    /* check whether a GL1 response is still pending */
    if (rr_ms_check_pending_response(interface_check, gas_id))
    {
      /* enter suspended state */
      exec_ptr->next_action = RR_MS_PREPARE_FOR_SUSPENSION;
    }
    else
    {
      /* no further action required */
      MSG_GERAN_ERROR_0_G("Interface timeout but no response pending");
      exec_ptr->next_action = RR_MS_NO_ACTION;
    }

    /* avoid further processing of the expiry */
    exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
  }

  return;
}

#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
/*===========================================================================

FUNCTION     rr_ms_get_primary_chain_holder

DESCRIPTION  Uses TRM API to determine which client is holding the primary RF chain
             as well as the reason they last requested to do so

DEPENDENCIES None

RETURN VALUE TRM client ID (not necessarily a GSM client)

SIDE EFFECTS None

===========================================================================*/
static trm_client_enum_t rr_ms_get_primary_chain_holder(trm_reason_enum_t *reason_ptr)
{
  trm_get_info_input_type  input;
  trm_get_info_return_type output;

  /* initialise input and output structures */
  input.client_id = TRM_GSM1; /* default, unused */
  input.info_type = TRM_DEVICE_ALLOCATION_INFO;
  output.info.dev_mapping_info.owners[RFM_DEVICE_0]        = TRM_NO_CLIENT;
  output.info.dev_mapping_info.owner_reasons[RFM_DEVICE_0] = TRM_NUM_REASONS;

  /* query TRM to determine the device allocations */
  trm_get_info(&input, &output);

  if (reason_ptr != NULL)
  {
    /* retrieve the reason code in use by the primary chain owner */
    *reason_ptr = output.info.dev_mapping_info.owner_reasons[RFM_DEVICE_0];
  }

  return(output.info.dev_mapping_info.owners[RFM_DEVICE_0]);
}
#endif /* !PERLUTF && !TEST_FRAMEWORK */

/*===========================================================================

FUNCTION     rr_ms_hibernation_required

DESCRIPTION  Determines whether the system is in a state where hibernation
             mode is a useful strategy for optimised power consumption i.e.
             when some other TRM client is holding the primary RF chain in
             multi-standby mode with no pre-emption possible, meaning that
             a GERAN stack is not going to get adequate resource for service

DEPENDENCIES None

RETURN VALUE TRUE if hibernation is required

SIDE EFFECTS None

===========================================================================*/
static boolean rr_ms_hibernation_required(const gas_id_t gas_id)
{
  boolean hibernation_required = FALSE;

  if (geran_in_multi_standby_mode())
  {
    if (rr_ms_other_stack_in_dedicated(gas_id))
    {
      /* a GERAN stack is in a CS connected mode and cannot be pre-empted */
      hibernation_required = TRUE;
    }
#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
    else
    {
      /* check status of other clients using TRM */
      trm_client_enum_t chain_trm_id;
      trm_reason_enum_t reason_held;
      
      /* query TRM to determine the current owner of the primary chain */
      chain_trm_id = rr_ms_get_primary_chain_holder(&reason_held);

      if ((TRM_1X == chain_trm_id) && (TRM_TRAFFIC == reason_held))
      {
        /* CDMA client is in a CS or PS connection and cannot be pre-empted */
        hibernation_required = TRUE;
        MSG_GERAN_HIGH_0_G("CDMA traffic detected, hibernation required");
      }
      else if (TRM_HDR == chain_trm_id)
      {
        MSG_GERAN_HIGH_1_G("HDR client holding primary chain for reason %d", (int)reason_held);
      }
    }
#endif /* !PERLUTF && !TEST_FRAMEWORK */
  }

  return(hibernation_required);
}

/*===========================================================================

FUNCTION     rr_ms_process_tmr_msg

DESCRIPTION  Determines the access stratum ID for a timer expiry message

DEPENDENCIES None

RETURN VALUE GAS ID

SIDE EFFECTS May determine that L1 is unresponsive and block NAS processing

===========================================================================*/
static gas_id_t rr_ms_process_tmr_msg(const rr_cmd_bdy_type *cmd_ptr, rr_ms_executor_t *exec_ptr)
{
  gas_id_t gas_id;

  /* the only timer messages GRR expects to receive are expiry notifications */
  /* which are only enqueued by its own timer management module (rr_timer.c) */
  if (cmd_ptr->message_header.message_id == (byte)MID_TIMER_EXPIRY)
  {
    timer_id_T timer_id;
    int        as_index;

    /* the timer expiry message should have a GAS ID in its data payload */
    gas_id   = (gas_id_t)(cmd_ptr->mid_timer_expiry.data);
    as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    /* extract the timer ID from the expiry message */
    timer_id = cmd_ptr->mid_timer_expiry.timer_id;
    if (timer_id == RR_DUAL_SIM_INTERFACE_TIMER)
    {
      /* handle the GL1 interface guard timer */
      rr_ms_handle_interface_timeout(exec_ptr, gas_id);
    }
    else if ((timer_id == RR_CELL_SELECTION_ABORT_TIMER) ||
             (timer_id == RR_DUAL_SIM_TERMINATE_TIMER)   ||
             (timer_id == CELL_RESELECTION_TIMER))
    {
      /* check whether this timer has expired whilst an interface is pending */
      exec_ptr->next_action = rr_ms_abort_pending_interface(RR_MS_GRR_TIMER_EXPIRED, gas_id);

      if (timer_id == RR_DUAL_SIM_TERMINATE_TIMER)
      {
        /* no need to process this timer expiry in the main state machines */
        exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
      }
    }
    else if (timer_id == RR_DUAL_SIM_BPLMN_LIST_TIMER)
    {
      /* BCCH decode for BPLMN list search has not completed */
      exec_ptr->next_action   = RR_MS_NO_ACTION;
      exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;

      MSG_GERAN_ERROR_0_G("BPLMN guard timer expired, reset DB, abort GL1");

      if ((rr_limited_service_in_effect(gas_id)          == FALSE) ||
          (rr_ms_other_sim_in_background_traffic(gas_id) == TRUE))
      {
        /* under normal service conditions, update the candidate cell DB */
        /* such that GRR does not try to start another BCCH list search  */
        rr_ms_candidate_db_search_timeout(gas_id);
      }

      /* notify the RR control state machine */
      rr_ms_set_bplmn_failed(gas_id);
      /* Tell the PLC control that its has been aborted */
      rr_ms_set_plc_gsm_aborted(gas_id);

      /* and ask GL1 to abort the current list decode operation */
      rr_send_mph_decode_bcch_list_abort_req(gas_id);
    }
    else if (timer_id == RR_DUAL_SIM_INACTIVATE_TIMER)
    {
      /* RR mode control not able to complete shutdown */
      exec_ptr->next_action   = RR_MS_NO_ACTION;
      exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
      rr_mode_deactivation_guard_expiry(gas_id);
    }
    else if (timer_id == RR_DUAL_SIM_HEARTBEAT_TIMER)
    {
      if ((RR_MS_INTERFACE_HIBERNATED == rr_multisim_info.gas_data[as_index].gl1_interface_state) &&
          (FALSE                      == rr_ms_hibernation_required(gas_id)))
      {
        /* hibernation condition has ended, resume GRR and process any pending service request */
        exec_ptr->next_action   = RR_MS_PREPARE_FOR_RESUMPTION;
        exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
        MSG_GERAN_HIGH_0_G("Hibernation ending, no blocking client detected");
      }
    }
    else if (timer_id == RR_ACQ_DUTY_CYCLE_TIMER)
    {
      exec_ptr->next_action   = RR_MS_NO_ACTION;
      exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
      rr_ms_acquisition_manager(RR_MS_DUTY_CYCLE_TIMER_EXPIRY, gas_id);
    }
    else if (timer_id == RR_EPD_TIMER)
    {
      exec_ptr->next_action   = RR_MS_NO_ACTION;
      exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
      MSG_GERAN_HIGH_0_G("RR_EPD_TIMER expired");
    }
  }
  else
  {
    gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
    exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
    exec_ptr->next_action   = RR_MS_NO_ACTION;
    MSG_GERAN_ERROR_1_G("Unhandled MS_TIMER message ID 0x%02X", (int)(cmd_ptr->message_header.message_id));
  }

  return(gas_id);
}

/*===========================================================================

FUNCTION      rr_ms_any_other_active_gas_id

DESCRIPTION   Determines whether any other GERAN access stratum is active

DEPENDENCIES  None

RETURN VALUE  TRUE if any other GERAN stack is active, FALSE otherwise

SIDE EFFECTS  None

===========================================================================*/
boolean rr_ms_any_other_active_gas_id(const gas_id_t gas_id)
{
  gas_id_t other_gas_id;

  for (other_gas_id = GERAN_ACCESS_STRATUM_ID_1; other_gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; other_gas_id++)
  {
    if (other_gas_id != gas_id)
    {
      if (rr_mode_gsm_active_or_starting(other_gas_id))
      {
        return(TRUE);
      }
    }
  }

  return(FALSE);
}

/*===========================================================================

FUNCTION     rr_ms_set_last_cleared_msg

DESCRIPTION  Updates the last_cleared_l1_msg store for the specified GAS ID

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_set_last_cleared_msg(const gas_id_t gas_id, const rr_l1_message_id_enum_T msg_id)
{
  rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].last_cleared_l1_msg = msg_id;

  return;
}

/*===========================================================================

FUNCTION     rr_ms_process_gl2_msg

DESCRIPTION  Extracts the access stratum ID from an incoming GL2 message

DEPENDENCIES None

RETURN VALUE GAS ID

SIDE EFFECTS None

===========================================================================*/
static gas_id_t rr_ms_process_gl2_msg(const rr_cmd_bdy_type *cmd_ptr, rr_ms_executor_t *exec_ptr)
{
  gas_id_t gas_id;
  boolean  check_l1 = TRUE;

  switch(cmd_ptr->message_header.message_id)
  {
  case DL_ESTABLISH_CNF:
    gas_id = cmd_ptr->dl_establish_cnf.gas_id;
    rr_ms_cs_establishment_confirmed(gas_id);
    break;

  case DL_ESTABLISH_IND:
    gas_id = cmd_ptr->dl_establish_ind.gas_id;
    break;

  case DL_UNIT_DATA_IND:
    gas_id = cmd_ptr->dl_unit_data_ind.gas_id;
    check_l1 = rr_ms_page_message_received(&(cmd_ptr->dl_unit_data_ind));
    break;

  case DL_RANDOM_ACCESS_CNF:
    gas_id = cmd_ptr->dl_random_access_cnf.gas_id;
    rr_ms_cs_establishment_confirmed(gas_id);
    break;

  case DL_RECONNECT_CNF:
    gas_id = cmd_ptr->dl_reconnect_cnf.gas_id;
    break;

  case DL_RELEASE_CNF:
    gas_id = cmd_ptr->dl_release_cnf.gas_id;
    break;

  case DL_RELEASE_IND:
    gas_id = cmd_ptr->dl_release_ind.gas_id;
    break;

  case DL_RESUME_CNF:
    gas_id = cmd_ptr->dl_resume_cnf.gas_id;
    break;

  case DL_SUSPEND_CNF:
    gas_id = cmd_ptr->dl_suspend_cnf.gas_id;
    rr_ms_cs_establishment_confirmed(gas_id);
    break;

  case DL_DATA_ACK_IND:
    gas_id = cmd_ptr->dl_data_ack_ind.gas_id;
    break;

  case DL_DATA_IND:
    gas_id = cmd_ptr->dl_data_ind.gas_id;
    break;

  case MDL_ERROR_IND:
    gas_id = cmd_ptr->mdl_error_ind.gas_id;
    break;

  default:
    gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
    MSG_GERAN_ERROR_1("Unknown GAS ID for GL2 msg 0x%02X", (int)(cmd_ptr->message_header.message_id));
    break;
  }

  /* check whether this message was a sys info read for a BCCH list decode */
  if (cmd_ptr->message_header.message_id == DL_UNIT_DATA_IND)
  {
    rr_l1_message_id_enum_T cleared_id = RR_MS_INVALID_L1_MESSAGE;

    /* ignore any messages with an invalid TC, since these cannot be Sys Info */
    if ((cmd_ptr->dl_unit_data_ind.tc != 0xFF) && (check_l1 == TRUE))
    {
      /* access stratum may be resumed by this message */
      if (rr_ms_check_required_l1_response(MPH_DECODE_BCCH_LIST_CNF, exec_ptr, gas_id))
      {
        /* record that this DL_UNIT_DATA_IND cleared down a pending BCCH_LIST_CNF */
        cleared_id = MPH_DECODE_BCCH_LIST_CNF;
      }
    }

    /* reset the last cleared message ID */
    rr_ms_set_last_cleared_msg(gas_id, cleared_id);
  }

  return(gas_id);
}

/*===========================================================================

FUNCTION     rr_ms_process_rlc_msg

DESCRIPTION  Determines the access stratum ID for an incoming GRLC message

DEPENDENCIES None

RETURN VALUE GAS ID

SIDE EFFECTS None

===========================================================================*/
static gas_id_t rr_ms_process_rlc_msg(const rr_cmd_bdy_type *cmd_ptr)
{
  gas_id_t gas_id;

#ifdef FEATURE_GSM_DTM
  if (RG_UL_DCCH_REQ == cmd_ptr->message_header.message_id)
  {
    gas_id = cmd_ptr->rlc_grr_ul_dcch_req.gas_id;
  }
  else
#endif /* FEATURE_GSM_DTM */
  {
    gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
    MSG_GERAN_ERROR_1("Unexpected message ID from GRLC %d", (int)(cmd_ptr->message_header.message_id));
  }

  return(gas_id);
}

#ifdef FEATURE_CGPS_UMTS_CP_GSM
/*===========================================================================

FUNCTION     rr_ms_process_gps_msg

DESCRIPTION  Determines which GERAN access stratum to use for GPS messages

DEPENDENCIES None

RETURN VALUE ID of GERAN Access Stratum which serves the GPS task

SIDE EFFECTS None

===========================================================================*/
static gas_id_t rr_ms_process_gps_msg(const rr_cmd_bdy_type *cmd_ptr)
{
  sys_modem_as_id_e_type nas_id;

  switch (cmd_ptr->message_header.message_id)
  {
    case RR_APDU_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_apdu_req);
      break;

    case RR_CGPS_EVENT_REQ:
      nas_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_cgps_event_req);
      break;

    default:
    {
      MSG_GERAN_ERROR_1("Unknown CGPS msg 0x%02X", (int)(cmd_ptr->message_header.message_id));

      // Exit indicating undefined GAS-ID
      return (GERAN_ACCESS_STRATUM_ID_UNDEFINED);
    }
  }

  return (geran_map_nas_id_to_gas_id(nas_id));
}
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

/*===========================================================================

FUNCTION     rr_ms_suspended_as_deactivation

DESCRIPTION  Allows GRR to monitor cell selection activities that might need
             to be aborted before AS shutdown can be completed.

DEPENDENCIES Should only be called for a suspended GERAN access stratum

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_suspended_as_deactivation(const gas_id_t gas_id)
{
  /* validate the subscription ID */
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    int as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    if (RR_MS_INTERFACE_HIBERNATED == rr_multisim_info.gas_data[as_index].gl1_interface_state)
    {
      /* if hibernation is in effect, this access stratum must be resumed */
      /* and any pending service request from NAS discarded silently here */
      MSG_GERAN_HIGH_0_G("Deactivated by NAS while hibernated");
      rr_ms_prepare_for_resumption(FALSE, gas_id);
    }
    else
    {
      /* normal suspension is in effect, discard any buffered service request */
      MSG_GERAN_HIGH_0_G("Deactivated by NAS while suspended");
      rr_ms_abort_pending_service_request(gas_id, FALSE);

      /* if the cell selection or reselection timers are inactive, */
      /* initiate a timed DS abort process for this access stratum */
      if ((rr_timer_is_running(RR_CELL_SELECTION_ABORT_TIMER, gas_id) == FALSE) &&
          (rr_timer_is_running(RR_DUAL_SIM_TERMINATE_TIMER, gas_id)   == FALSE) &&
          (rr_timer_is_running(CELL_RESELECTION_TIMER, gas_id)        == FALSE))
      {
        /* access stratum is currently suspended, but has no other */
        /* guard that ensures AS resumption if GL1 cannot respond  */
        if (rr_timer_start(RR_DUAL_SIM_TERMINATE_TIMER, GRR_TIMEOUT_10_SECONDS, gas_id))
        {
          MSG_GERAN_HIGH_0_G("Starting termination timer");
        }
        else
        {
          MSG_GERAN_ERROR_0_G("Unable to start termination timer");
        }
      }
    }
  }

  return;
}

/*===========================================================================

FUNCTION     rr_ms_check_suspension_response

DESCRIPTION  Determines any response required to a message sent to GRR while
             the active AS is suspended

DEPENDENCIES Input GAS ID parameter must be valid

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_check_suspension_response(const rr_cmd_bdy_type *cmd_ptr, rr_ms_executor_t *exec_ptr, const gas_id_t gas_id)
{
  int as_index;

  as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
  if ((rr_multisim_info.gas_data[as_index].gl1_interface_state != RR_MS_INTERFACE_ACTIVE) &&
      (cmd_ptr                                                 != NULL))
  {
    byte message_set = cmd_ptr->message_header.message_set;
    byte message_id  = cmd_ptr->message_header.message_id;

    /* access stratum is suspended - certain messages cannot be handled */
    if (message_set == MS_MM_RR)
    {
      if (message_id == (byte)RR_EST_REQ)
      {
        rr_send_abort_ind(RANDOM_ACCESS_FAILURE, MM_RR_NO_RESOURCE_AVAILABLE, gas_id);
        exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
      }
      else if (message_id == (byte)RR_PLMN_SELECT_REQ)
      {
        rr_ms_buffer_service_request(cmd_ptr);
        exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
      }
      else if (message_id == (byte)RR_PLMN_LIST_REQ)
      {
        /* do not bother trying to process this now, unlikely to succeed */
        rr_ms_send_rr_plmn_list_cnf(&(cmd_ptr->mm.rr_plmn_list_req), MM_AS_CONNECTED);
        exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
      }
      else if ((message_id == (byte)RR_STOP_GSM_MODE_REQ) ||
               (message_id == (byte)RR_DEACT_REQ))
      {
        /* check for unguarded cell selection/reselection activity */
        rr_ms_suspended_as_deactivation(gas_id);
      }
    }
    else if (message_set == MS_MAC_RR)
    {
      if (message_id == (byte)MAC_GRR_UL_TBF_REQ)
      {
        /* PS access is not available during suspension */
        rr_send_grr_mac_no_ps_access_ind(DEL_ONE, gas_id);

        /* do not resume GMAC if PS access is blocked */
        if (rr_mac_ps_access_blocked(gas_id))
        {
          MSG_GERAN_ERROR_0_G("Unexpected GMAC request during PS suspension");
        }
        else
        {
          // TODO - this probably will not work, needs to be co-ordinated with GRR main state
          rr_send_grr_mac_ps_access_req(FALSE, gas_id);
        }
        exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
      }
    }
#ifdef FEATURE_GSM_DTM
    else if (message_set == MS_RLC_RR)
    {
      /* should never happen, but if it does, close the loop with RLC */
      if (message_id == (byte)RG_UL_DCCH_REQ)
      {
        rr_send_grr_mac_dtm_ul_dcch_cnf(OTHER_ERROR, gas_id);
        exec_ptr->process_in_rr = RR_PROCESS_MSG_NEVER;
      }
    }
#endif /* FEATURE_GSM_DTM */
  }

  return;
}

/*===========================================================================

FUNCTION     rr_ms_max_heartbeat_reached

DESCRIPTION  Rebaselines the heartbeat time counter

DEPENDENCIES None

RETURN VALUE VOID

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_max_heartbeat_reached(int as_index)
{
  uint16 oldest = RR_MS_HEARTBEAT_LIMIT - 1;

  if (rr_multisim_info.gas_data[as_index].svc_rqst_time < rr_multisim_info.gas_data[as_index].svc_loss_time)
  {
    if (rr_multisim_info.gas_data[as_index].svc_rqst_time != 0)
    {
      oldest = rr_multisim_info.gas_data[as_index].svc_rqst_time-1;
    }
    else if (rr_multisim_info.gas_data[as_index].svc_loss_time != 0)
    {
      oldest = rr_multisim_info.gas_data[as_index].svc_loss_time-1;
    }
  }
  else /* (rr_multisim_info.gas_data[as_index].svc_loss_time <= rr_multisim_info.gas_data[as_index].svc_rqst_time) */
  {
    if (rr_multisim_info.gas_data[as_index].svc_loss_time != 0)
    {
      oldest = rr_multisim_info.gas_data[as_index].svc_loss_time-1;
    }
    else if (rr_multisim_info.gas_data[as_index].svc_rqst_time != 0)
    {
      oldest = rr_multisim_info.gas_data[as_index].svc_rqst_time-1;
    }
  }

  if (rr_multisim_info.gas_data[as_index].svc_rqst_time != 0)
  {
    rr_multisim_info.gas_data[as_index].svc_rqst_time -= oldest;
  }

  if (rr_multisim_info.gas_data[as_index].svc_loss_time != 0)
  {
    rr_multisim_info.gas_data[as_index].svc_loss_time -= oldest;
  }

  rr_multisim_info.gas_data[as_index].seconds_since_suspension = RR_MS_HEARTBEAT_LIMIT - oldest;
  MSG_GERAN_ERROR_1("Max heartbeat, seconds since suspension = %d", (int)(rr_multisim_info.gas_data[as_index].seconds_since_suspension));

  return;
}

/*===========================================================================

FUNCTION     rr_ms_gsm_using_access_urgent

DESCRIPTION  Checks whether any GSM client is using the ACCESS_URGENT reason

DEPENDENCIES None

RETURN VALUE TRUE if **any** GSM client is using the TRM_ACCESS_URGENT reason

SIDE EFFECTS None

===========================================================================*/
static boolean rr_ms_gsm_using_access_urgent(void)
{
#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
  gas_id_t gas_id;

  for (gas_id = GERAN_ACCESS_STRATUM_ID_1; gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; gas_id++)
  {
    if (TRM_ACCESS_URGENT == grm_get_reason(rr_ms_get_trm_client_id(gas_id), gas_id))
    {
      return(TRUE);
    }
  }
#endif /* !PERLUTF && !TEST_FRAMEWORK */

  return(FALSE);
}

/*===========================================================================

FUNCTION     rr_ms_process_heartbeat

DESCRIPTION  Monitors a suspended access stratum

DEPENDENCIES None

RETURN VALUE RR event for main state machine

SIDE EFFECTS Checks for staleness of any buffered service request from NAS

===========================================================================*/
void rr_ms_process_heartbeat(const gas_id_t gas_id)
{
  rr_ms_gl1_if_state_t interface_state;
  int                  as_index;

  /* verify that the specified GAS ID is still suspended (or hibernated) */
  as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
  interface_state = rr_multisim_info.gas_data[as_index].gl1_interface_state;
  if (interface_state != RR_MS_INTERFACE_ACTIVE)
  {
    /* determine how long this AS has been suspended */
    if (rr_multisim_info.gas_data[as_index].seconds_since_suspension < RR_MS_HEARTBEAT_LIMIT)
    {
      if (interface_state == RR_MS_INTERFACE_HIBERNATED)
      {
        rr_multisim_info.gas_data[as_index].seconds_since_suspension += RR_MS_HIBERNATE_SECONDS;
      }
      else
      {
        rr_multisim_info.gas_data[as_index].seconds_since_suspension += RR_MS_HEARTBEAT_SECONDS;
      }
    }
    else
    {
      /* not expected, but better cope with it */
      rr_ms_max_heartbeat_reached(as_index);
    }

    /* if the heartbeat timer has expired while RR is in an INACTIVE state */
    /* a recovery needs to be attempted, otherwise service requests cannot */
    /* ever be processed, and this GERAN stack will remain out of service  */
    if ((interface_state != RR_MS_INTERFACE_HIBERNATED) && rr_is_inactive(gas_id))
    {
      MSG_GERAN_ERROR_0_G("Resume suspended interface: inactive but not hibernated");
      rr_ms_prepare_for_resumption(TRUE, gas_id);
      return;
    }

    /* check whether any buffered service request is now stale */
    rr_ms_check_pending_service_request(gas_id, rr_multisim_info.gas_data[as_index].seconds_since_suspension);

    /* restart the heartbeat with the appropriate heartrate */
    if (rr_ms_start_heartbeat(interface_state, gas_id))
    {
      /* heartbeat has restarted, prompt GL1 to complete its last */
      /* activity, unless there a GERAN stack in a connected mode */
      if (FALSE == rr_ms_gsm_using_access_urgent())
      {
        (void)rr_ms_check_pending_response(RR_MS_POLL_FOR_RESPONSE, gas_id);
      }
    }
    else
    {
      /* heartbeat failure has occurred, resume now */
      MSG_GERAN_ERROR_0_G("Unable to restart heartbeat, resume now");
      rr_ms_prepare_for_resumption(TRUE, gas_id);
    }
  }
  else
  {
    /* explicitly clear down the heartbeat timer */
    (void)rr_timer_cancel(RR_DUAL_SIM_HEARTBEAT_TIMER, gas_id);
    MSG_GERAN_ERROR_0_G("Heartbeat not expected while interface is active");
  }

  return;
}

#ifdef FEATURE_WCDMA
/*===========================================================================

FUNCTION     rr_ms_process_rrc_msg

DESCRIPTION  Called for an inter-task signal from RRC

DEPENDENCIES None

RETURN VALUE GAS ID

SIDE EFFECTS None

===========================================================================*/
static gas_id_t rr_ms_process_rrc_msg(const rr_cmd_bdy_type *cmd_ptr)
{
  sys_modem_as_id_e_type rrc_id;
  gas_id_t               gas_id;

  /* update GERAN internal storage for debug */
  (void)rr_ms_store_subscription_capabilities(NULL);

  switch(cmd_ptr->message_header.message_id)
  {
  case RRC_CIPHER_SYNC_IND:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rrc_cipher_sync_ind);
    break;

  case RR_INTERRAT_HANDOVER_REQ:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_interrat_handover_req);
    break;

  case RR_INTERRAT_HANDOVER_ABORT_REQ:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_interrat_handover_abort_req);
    break;

  case RR_NAS_DATA_IND:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_nas_data_ind);
    break;

  case RR_INTERRAT_RESELECTION_REQ:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_interrat_reselect_req);
    break;

  case RR_INTERRAT_RESELECTION_ABORT_REQ:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_interrat_reselection_abort_req);
    break;

  case RR_INTERRAT_CC_ORDER_REQ:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_interrat_cc_order_req);
    break;

  case RR_INTERRAT_CC_ORDER_ABORT_REQ:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_interrat_cc_order_abort_req);
    break;

  case RRC_INTERRAT_RESELECTION_REJ:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rrc_interrat_reselect_rej);
    break;

  case RRC_INTERRAT_HANDOVER_CNF:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rrc_interrat_handover_cnf);
    break;

  case RRC_INTERRAT_HANDOVER_ABORT_CNF:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rrc_interrat_handover_abort_cnf);
    break;

  case RR_INTERRAT_REDIRECT_REQ:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_interrat_redirect_req);
    break;

  case RR_INTERRAT_REDIRECT_ABORT_REQ:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_interrat_redirect_abort_req);
    break;

  case RRC_INTERRAT_RESELECTION_ABORT_CNF:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rrc_interrat_reselection_abort_cnf);
    break;

  case RRC_INTERRAT_PLMN_SRCH_CNF:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rrc_interrat_plmn_srch_cnf);
    break;

  case RRC_INTERRAT_PLMN_SRCH_ABORT_CNF:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rrc_interrat_plmn_srch_abort_cnf);
    break;

  case RR_INTERRAT_PLMN_SRCH_REQ:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_interrat_plmn_srch_req);
    break;

  case RR_INTERRAT_PLMN_SRCH_ABORT_REQ:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_interrat_plmn_srch_abort_req);
    break;

  case RR_INTERRAT_PLMN_SRCH_SUSPEND_IND:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_interrat_plmn_srch_suspend_ind);
    break;

  case RRC_INTERRAT_CC_ORDER_CNF:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rrc_interrat_cc_order_cnf);
    break;

#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
  case RRC_INTERRAT_REDIRECT_REJ:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rrc_interrat_redirect_rej);
    break;

  case RRC_INTERRAT_REDIRECT_ABORT_CNF:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rrc_interrat_redirect_abort_cnf);
    break;
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

  case RRC_RR_STOP_WCDMA_CNF:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rrc_rr_stop_wcdma_cnf);
    break;

  case RR_RRC_STOP_GSM_REQ:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_rrc_stop_gsm_req);
    break;

  case RR_CLEAR_DEDICATED_PRIORITIES_IND:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_clear_dedicated_priorities_ind);
    break;

  case RR_DEDICATED_PRIORITIES_REQ:
    rrc_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rr_dedicated_priorities_req);
    break;

  default:
    rrc_id = geran_get_multi_mode_sub();
    break;
  }

  if ((SYS_MODEM_AS_ID_1 == rrc_id) ||
      (SYS_MODEM_AS_ID_2 == rrc_id)
#ifdef FEATURE_TRIPLE_SIM
   || (SYS_MODEM_AS_ID_3 == rrc_id)
#endif /* FEATURE_TRIPLE_SIM */
      )
  {
    /* map the ASID to GERAN access stratum ID format */
    gas_id = geran_map_nas_id_to_gas_id(rrc_id);
  }
  else
  {
    MSG_GERAN_ERROR_2("Missing or invalid ASID %d in RRC msg %d",
                      (int)rrc_id,
                      (int)(cmd_ptr->message_header.message_id));
    gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
  }

  return(gas_id);
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
/*===========================================================================

FUNCTION     rr_ms_process_tdsrrc_msg

DESCRIPTION  Called for an inter-task signal from TDSRRC

DEPENDENCIES None

RETURN VALUE GAS ID

SIDE EFFECTS None

===========================================================================*/
static gas_id_t rr_ms_process_tdsrrc_msg(const rr_cmd_bdy_type *cmd_ptr)
{
  gas_id_t               gas_id;

#ifdef FEATURE_TDS_SUB_ID_INTERFACE

  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;

  switch(cmd_ptr->message_header.message_id)
  {
    case TDSRR_INTERRAT_HANDOVER_REQ:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_ho_req);
      break;
    }

    case TDSRR_INTERRAT_HANDOVER_ABORT_REQ:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_ho_abort_req);
      break;
    }

    case TDSRR_NAS_DATA_IND:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_nas_data_ind);
      break;
    }

    case TDSRR_INTERRAT_RESELECTION_REQ:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_resel_req);
      break;
    }

    case TDSRR_INTERRAT_RESELECTION_ABORT_REQ:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_resel_abort_req);
      break;
    }

    case TDSRR_INTERRAT_PLMN_SRCH_REQ:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_plmn_srch_req);
      break;
    }

    case TDSRR_INTERRAT_PLMN_SRCH_ABORT_REQ:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_plmn_srch_abort_req);
      break;
    }

    case TDSRR_INTERRAT_PLMN_SRCH_SUSPEND_IND:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_plmn_srch_suspend_ind);
      break;
    }

    case TDSRR_INTERRAT_CC_ORDER_REQ:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_cco_req);
      break;
    }

    case TDSRR_INTERRAT_CC_ORDER_ABORT_REQ:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_cco_abort_req);
      break;
    }

    case TDSRR_INTERRAT_REDIRECT_REQ:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_redir_req);
      break;
    }

    case TDSRR_INTERRAT_REDIRECT_ABORT_REQ:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_redir_abort_req);
      break;
    }

    case TDSRR_STOP_GSM_REQ:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_stop_gsm_req);
      break;
    }

    case TDSRR_INTERRAT_TL1_STOP_IND:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_tl1_stop_ind);
      break;
    }

    case TDSRR_CIPHER_SYNC_IND:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rrc_cipher_sync_ind);
      break;
    }

    case TDSRRC_INTERRAT_HANDOVER_CNF:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_ho_cnf);
      break;
    }

    case TDSRRC_INTERRAT_HANDOVER_ABORT_CNF:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_ho_abort_cnf);
      break;
    }

    case TDSRRC_INTERRAT_RESELECTION_REJ:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_resel_rej);
      break;
    }

    case TDSRRC_INTERRAT_RESELECTION_ABORT_CNF:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_resel_abort_cnf);
      break;
    }

    case TDSRRC_INTERRAT_PLMN_SRCH_CNF:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_plmn_srch_cnf);
      break;
    }

    case TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_plmn_srch_abort_cnf);
      break;
    }

    case TDSRRC_INTERRAT_CC_ORDER_CNF:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->rrc_cipher_sync_ind);
      break;
    }

    case TDSRRC_INTERRAT_REDIRECT_REJ:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_redir_rej);
      break;
    }

    case TDSRRC_INTERRAT_REDIRECT_ABORT_CNF:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_redir_abort_cnf);
      break;
    }

    case TDSRRC_STOP_TDS_CNF:
    {
      as_id = RR_MS_EXTRACT_AS_ID(cmd_ptr->tdsrr_stop_tds_cnf);
      break;
    }

    default: ;
  }

  if ((SYS_MODEM_AS_ID_1 == as_id) ||
      (SYS_MODEM_AS_ID_2 == as_id)
#ifdef FEATURE_TRIPLE_SIM
   || (SYS_MODEM_AS_ID_3 == as_id)
#endif /* FEATURE_TRIPLE_SIM */
      )
  {
    /* map the ASID to GERAN access stratum ID format */
    gas_id = geran_map_nas_id_to_gas_id(as_id);
  }
  else
  {
    MSG_GERAN_ERROR_2("Missing or invalid ASID %d in TDSRRC msg %d",
                      (int)as_id,
                      (int)(cmd_ptr->message_header.message_id));
    gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
  }

#else // FEATURE_TDS_SUB_ID_INTERFACE

  /* update GERAN internal storage for debug */
  (void)rr_ms_store_subscription_capabilities(NULL);

  /* use the default multimode GAS ID for all TDS messages */
  gas_id = geran_get_multi_mode_gas_id();

#endif // FEATURE_TDS_SUB_ID_INTERFACE

  return(gas_id);
}
#endif /* FEATURE_GSM_TDS */

/*===========================================================================

FUNCTION      rr_ms_process_msgr_header

DESCRIPTION   Processes the contents of a Message Router header (from LTE)

DEPENDENCIES  None

RETURN VALUE  GAS ID context for the message (defaults to multimode sub)

SIDE EFFECTS  Updates internal debug storage for multimode capabilities

===========================================================================*/
static gas_id_t rr_ms_process_msgr_header(const msgr_hdr_s *hdr_ptr)
{
  sys_modem_as_id_e_type as_id;
  gas_id_t               gas_id;

  /* retrieve the current multimode subscription ID */
  as_id = rr_ms_store_subscription_capabilities(NULL);

  if (hdr_ptr != NULL)
  {
#ifdef FEATURE_GSM_BAND_AVOIDANCE
    /* this feature allows MCS to send certain commands whose UMID implies the subscription ID */
    if (MCS_CXM_BAND_AVOID_BLIST_GSM1_IND == hdr_ptr->id)
    {
      /* UMID implies multimode subscription */
      gas_id = geran_get_multi_mode_gas_id();
      MSG_GERAN_HIGH_0_G("CXM_BAND_AVOID_BLIST for multimode subscription");
    }
    else if (MCS_CXM_BAND_AVOID_BLIST_GSM2_IND == hdr_ptr->id)
    {
      /* UMID implies non-multimode subscription */
      gas_id = GERAN_ACCESS_STRATUM_ID_2;
      MSG_GERAN_HIGH_0_G("CXM_BAND_AVOID_BLIST for non-multimode subscription");
    }
    else
#endif /* FEATURE_GSM_BAND_AVOIDANCE */
    {
      /* use the default multimode GAS ID for any other MSGR messages */
      gas_id = geran_get_multi_mode_gas_id();

      /* Message Router headers contain an instance ID */
      /* which should map to a usable AS ID via macros */
      /* These are defined in MCS and MMCP API folders */
      if (as_id != INST_ID_TO_SYS_AS_ID(MSGR_GET_INST(hdr_ptr)))
      {
        MSG_GERAN_HIGH_1_G("MSGR header contains unexpected ASID (multimode sub is 0x%02X)", (int)as_id);
      }
    }
  }
  else
  {
    gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
    MSG_GERAN_ERROR_1("Unable to process MSGR header (multimode sub is 0x%02X)", (int)as_id);
  }

  return(gas_id);
}

/*===========================================================================

FUNCTION     rr_ms_process_task_message

DESCRIPTION  Intercepts incoming command message from the RR task

DEPENDENCIES None

RETURN VALUE RR_PROCESS_MSG_NOW if the message should be processed by RR

SIDE EFFECTS None

===========================================================================*/
rr_process_msg_action_e rr_ms_process_task_message(const rr_cmd_bdy_type *cmd_ptr, gas_id_t *gas_id_ptr)
{
  rr_ms_executor_t executor;
  gas_id_t         next_gas_id;

  /* set defaults for the execution controller */
  executor.process_in_rr = RR_PROCESS_MSG_NOW;
  if (RR_MULTI_SIM_MODE_ENABLED)
  {
    executor.next_action = RR_MS_CHECK_FOR_SUSPENSION;
  }
  else
  {
    executor.next_action = RR_MS_NO_ACTION;
  }

  if (cmd_ptr == NULL)
  {
    next_gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
    MSG_GERAN_ERROR_0("Null pointer received for inter-task message");

    return(RR_PROCESS_MSG_NEVER);
  }
  else
  {
    /* determine the appropriate GAS ID from the incoming message */
    if (cmd_ptr->message_header.message_set == MS_MM_RR)
    {
      next_gas_id = rr_ms_process_nas_msg(cmd_ptr, &executor);
    }
    else if (cmd_ptr->message_header.message_set == MS_RR_L1)
    {
      next_gas_id = rr_ms_process_gl1_msg(cmd_ptr, &executor);
    }
    else if (cmd_ptr->message_header.message_set == MS_RR_L2)
    {
      next_gas_id = rr_ms_process_gl2_msg(cmd_ptr, &executor);
    }
    else if ((cmd_ptr->message_header.message_set == MS_RR_RR) ||
             (cmd_ptr->message_header.message_set == MS_RR_RR_EXT))
    {
      next_gas_id = rr_ms_process_grr_msg(cmd_ptr, &executor);
    }
    else if (cmd_ptr->message_header.message_set == MS_MAC_RR)
    {
      next_gas_id = rr_ms_process_mac_msg(cmd_ptr, &executor);
    }
    else if (cmd_ptr->message_header.message_set == MS_TIMER)
    {
      next_gas_id = rr_ms_process_tmr_msg(cmd_ptr, &executor);
    }
#ifdef FEATURE_CGPS_UMTS_CP_GSM
    else if (cmd_ptr->message_header.message_set == MS_CGPS_RR)
    {
      next_gas_id = rr_ms_process_gps_msg(cmd_ptr);
    }
#endif /* FEATURE_CGPS_UMTS_CP_GSM */
    else if (cmd_ptr->message_header.message_set == MS_RLC_RR)
    {
      next_gas_id = rr_ms_process_rlc_msg(cmd_ptr);
    }
#ifdef FEATURE_WCDMA
    else if (cmd_ptr->message_header.message_set == MS_RRC_RR)
    {
      next_gas_id = rr_ms_process_rrc_msg(cmd_ptr);
    }
#endif /* FEATURE_WCDMA */
    else if (cmd_ptr->message_header.message_set == MS_GHDI_EXPORT)
    {
      /* this should only occur during dedicated mode */
      next_gas_id = rr_ms_connected_gas_id();
    }
#ifdef FEATURE_GSM_TDS
    else if (cmd_ptr->message_header.message_set == MS_TDSRRC_RR)
    {
      next_gas_id = rr_ms_process_tdsrrc_msg(cmd_ptr);
    }
#endif /* FEATURE_GSM_TDS */
    else if (cmd_ptr->message_header.message_set == MS_MSGR_RR)
    {
        /* process the MSGR header, which normally includes the multimode ASID, */
        /* but may indicate the non-multimode subscription in certain scenarios */
        next_gas_id = rr_ms_process_msgr_header(&(cmd_ptr->rr_msgr_msg.message.hdr));
    }
    else
    {
      next_gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
      MSG_GERAN_ERROR_0("Unhandled inter-task message set");
    }
  }

  if (executor.next_action != RR_MS_NO_ACTION)
  {
    /* a valid GAS ID should always be available */
    if (next_gas_id == GERAN_ACCESS_STRATUM_ID_UNDEFINED)
    {
      MSG_GERAN_ERROR_2("GRR is unable to determine the active AS(%d,%d)",
                        (int)(cmd_ptr->message_header.message_set),
                        (int)(cmd_ptr->message_header.message_id));
    }
  }

  if (next_gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    /* check the execution control structure for actions that must occur now */
    switch(executor.next_action)
    {
    case RR_MS_PREPARE_FOR_SUSPENSION:
      if (rr_ms_prepare_for_suspension(RR_MS_INTERFACE_SUSPENDED, next_gas_id) == FALSE)
      {
        /* attempt to start suspension failed, try to clear up the GL1 interface */
        MSG_GERAN_ERROR_1("gs%1d: Unable to suspend, aborting interface now", 1+(int)next_gas_id);
        rr_ms_recover_blocked_interface(next_gas_id);
      }
      break;

    case RR_MS_CHECK_FOR_SUSPENSION:
      rr_ms_check_suspension_response(cmd_ptr, &executor, next_gas_id);
      break;

    case RR_MS_PREPARE_FOR_RESUMPTION:
      rr_ms_prepare_for_resumption(TRUE, next_gas_id);
      break;

    default:
      /* no further action required */
      break;
    }
  }

  /* return the current GAS ID if the caller provided an output parameter */
  if (gas_id_ptr != NULL)
  {
    (*gas_id_ptr) = next_gas_id;
  }

  return(executor.process_in_rr);
}

/*===========================================================================

FUNCTION      rr_ms_service_recovery_started

DESCRIPTION   Informs Priority Inversion Manager that GRR is about to start
              a cell (re)selection action related to recovering service.

DEPENDENCIES  This function relies on the assumption that GRR will send a
              further cell selection primitive to L1 (e.g. MPH_POWER_SCAN_REQ)
              very soon after this call (or even immediately).

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/
void rr_ms_service_recovery_started(const gas_id_t gas_id)
{
  /* pass the recovery event into the Priority Inversion Manager */
  rr_ms_acquisition_manager(RR_MS_GAS_ID_SERVICE_RECOVERY, gas_id);

  return;
}

/*===========================================================================

FUNCTION      rr_ms_csfb_status_changed

DESCRIPTION   Informs Priority Inversion Manager of CS Fallback status

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/
void rr_ms_csfb_status_changed(const boolean csfb_status, const gas_id_t gas_id)
{
  if (csfb_status)
  {
    /* notify Priority Inversion Manager that CSFB is in progress */
    rr_ms_acquisition_manager(RR_MS_GAS_ID_CSFB_IN_PROGRESS, gas_id);
  }
  else
  {
    /* notify Priority Inversion Manager that CSFB status has been reset */
    rr_ms_acquisition_manager(RR_MS_GAS_ID_CSFB_STATE_RESET, gas_id);
  }

  return;
}

/*===========================================================================

FUNCTION      rr_ms_cs_establishment_confirmed

DESCRIPTION   Informs Priority Inversion Manager that GERAN is entering
              a connection establishment phase.

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/
void rr_ms_cs_establishment_confirmed(const gas_id_t gas_id)
{
  /* pass the establishment event into the Priority Inversion Manager */
  rr_ms_acquisition_manager(RR_MS_GAS_ID_CS_NOW_CONNECTED, gas_id);

  return;
}

/*===========================================================================

FUNCTION     rr_ms_interface_blocked_by_connection

DESCRIPTION  Checks whether loss of service may have been caused by an active
             RACH, dedicated mode or GPRS session on the opposite subscription.

DEPENDENCIES None

RETURN VALUE TRUE if any other client holds the TRM lock for a connected mode

SIDE EFFECTS None

===========================================================================*/
static boolean rr_ms_interface_blocked_by_connection(const gas_id_t gas_id)
{
  boolean blocked_by_conn = FALSE;

#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
  /* the following checks are only relevant to multi-standby modes */
  if (geran_in_multi_standby_mode())
  {
    trm_client_enum_t geran_trm_id;
    trm_client_enum_t chain_trm_id;
    trm_reason_enum_t reason_held;

    /* determine the TRM client being used by the specified GAS ID */
    geran_trm_id = rr_ms_get_trm_client_id(gas_id);

    /* query TRM to determine the current owner of the primary chain */
    chain_trm_id = rr_ms_get_primary_chain_holder(&reason_held);

    /* if the specified GAS ID is already the chain owner, it is not blocked */
    if (geran_trm_id == chain_trm_id)
    {
      reason_held = TRM_NUM_REASONS;
    }
    else if (reason_held != TRM_NUM_REASONS)
    {
        MSG_GERAN_HIGH_2_G("Primary antenna currently owned by client %d for reason %d", (int)chain_trm_id, (int)reason_held);
      }

    /* these reasons are generally indicative of non-preemptable modes */
    if ((TRM_ACCESS_URGENT == reason_held) ||
        (TRM_TRAFFIC       == reason_held) ||
        (TRM_ACCESS        == reason_held))
    {
      blocked_by_conn = TRUE;
    }
  }
#endif /* !PERLUTF && !TEST_FRAMEWORK */

  return(blocked_by_conn);
}

/*===========================================================================

FUNCTION      rr_ms_complete_nas_service_cnf

DESCRIPTION   Intercepts the PLMN_SELECT_CNF before it reaches NAS

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  May invoke priority inversion for OOS

===========================================================================*/
void rr_ms_complete_nas_service_cnf(rr_plmn_select_cnf_T *msg_ptr, const gas_id_t gas_id)
{
  if (msg_ptr != NULL)
  {
    msg_ptr->as_id = geran_map_gas_id_to_nas_id(gas_id);
    msg_ptr->failure_cause = MM_RR_OTHER_FAILURE;

    /* priority inversion can now be disabled, since we're either in service */
    /* or going OOS - for either case there is no GL1 activity to prioritise */
    rr_ms_acquisition_manager(RR_MS_GAS_ID_SERVICE_NOTIFIED, gas_id);

    /* check whether this access stratum has gone OOS due to lack of transceiver lock */
    if (msg_ptr->service_state == MM_AS_NO_SERVICE)
    {
      if (rr_ms_interface_was_aborted(gas_id) || rr_ms_interface_blocked_by_connection(gas_id))
      {
        MSG_GERAN_ERROR_1_G("PLMN_SELECT_CNF will indicate NO_RESOURCE_AVAILABLE (OOS was %d)", (int)(msg_ptr->ue_oos));
        msg_ptr->failure_cause = MM_RR_NO_RESOURCE_AVAILABLE;
        msg_ptr->ue_oos        = TRUE;
      }
    }

    /* set the aborted interface flag for the active stack */
    rr_ms_set_aborted_interface(FALSE, gas_id);
  }

  return;
}

/*===========================================================================

FUNCTION      rr_ms_complete_nas_service_ind

DESCRIPTION   Intercepts the RR_SERVICE_IND before it reaches NAS

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  May initiate access stratum suspension/hibernation for OOS

===========================================================================*/
void rr_ms_complete_nas_service_ind(rr_service_ind_T *msg_ptr, const gas_id_t gas_id)
{
  if (msg_ptr != NULL)
  {
    int as_index;

    as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
    msg_ptr->as_id = geran_map_gas_id_to_nas_id(gas_id);
    msg_ptr->failure_cause = MM_RR_OTHER_FAILURE;

    /* priority inversion can now be disabled, since we're either in service */
    /* or going OOS - for either case there is no GL1 activity to prioritise */
    rr_ms_acquisition_manager(RR_MS_GAS_ID_SERVICE_NOTIFIED, gas_id);

    if (msg_ptr->service_state == MM_AS_NO_SERVICE)
    {
      MSG_GERAN_HIGH_0_G("NO SERVICE event being sent to NAS");

      if (rr_multisim_info.gas_data[as_index].gl1_interface_state != RR_MS_INTERFACE_ACTIVE)
      {
        /* loss of service on a suspended access stratum - record the time */
        /* use a +1 here, since time=0 is used to indicate "no occurrence" */
        rr_multisim_info.gas_data[as_index].svc_loss_time = rr_multisim_info.gas_data[as_index].seconds_since_suspension+1;

        /* update the message to indicate the probable cause */
        msg_ptr->failure_cause = MM_RR_NO_RESOURCE_AVAILABLE;

        /* abort any service request that was received during the suspension */
        rr_ms_abort_pending_service_request(gas_id, TRUE);
      }

      /* check whether this access stratum has gone OOS due to lack of transceiver lock */
      if (rr_ms_interface_was_aborted(gas_id) || rr_ms_interface_blocked_by_connection(gas_id))
      {
        msg_ptr->failure_cause = MM_RR_NO_RESOURCE_AVAILABLE;
        MSG_GERAN_ERROR_0_G("RR_SERVICE_IND will indicate NO_RESOURCE_AVAILABLE");
      }

      if ((MM_RR_NO_RESOURCE_AVAILABLE == msg_ptr->failure_cause)   &&
          (TRUE                        == rr_ms_hibernation_required(gas_id)))
      {
        /* putting this OOS access stratum back into a suspended state */
        /* will prevent NAS from bringing it back online straight away */
        if (rr_ms_prepare_for_suspension(RR_MS_INTERFACE_HIBERNATED, gas_id))
        {
          MSG_GERAN_HIGH_0_G("OOS hibernation started");
        }
        else
        {
          MSG_GERAN_ERROR_0_G("Unable to hibernate");
        }
      }
    }

    /* reset the aborted interface flag each time a RR_SERVICE_IND is sent */
    rr_ms_set_aborted_interface(FALSE, gas_id);
  }

  return;
}

/*===========================================================================

FUNCTION      rr_ms_complete_nas_cb_ind

DESCRIPTION   Intercepts the RR_CELL_CHANGE_IND before it reaches NAS(CB)

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  May initiate priority inversion for CBCH decoding

===========================================================================*/
void rr_ms_complete_nas_cb_ind(rr_cell_change_ind_T *msg_ptr, const gas_id_t gas_id)
{
  if (msg_ptr != NULL)
  {
    /* update the message with an appropriate ASID tag for NAS */
    msg_ptr->as_id = geran_map_gas_id_to_nas_id(gas_id);

    /* update the global data for this subscription with the CBCH presence */
    if (msg_ptr->CBCH_present)
    {
      rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].cbch_state = RR_MS_CBCH_ENABLED;
    }
    else
    {
      rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].cbch_state = RR_MS_CBCH_REMOVED;
    }

    /* assume MAC is idle at this point and update CBCH prioritisation */
    rr_ms_update_cell_broadcast_priority(TRUE, gas_id);
  }

  return;
}

/*===========================================================================

FUNCTION       rr_ms_cs_connected

DESCRIPTION    Informs the multi-SIM controller of CS connection state

DEPENDENCIES   None

RETURN VALUE   VOID

SIDE EFFECTS   None

===========================================================================*/
void rr_ms_cs_connected(const boolean is_connected, const gas_id_t gas_id)
{
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    /* this GAS ID has just started a dedicated mode connection */
    rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].cs_connected = is_connected;
  }

  return;
}

/*===========================================================================

FUNCTION       rr_ms_other_stack_in_dedicated

DESCRIPTION    Determines whether any other GAS ID is in CS connected state

DEPENDENCIES   None

RETURN VALUE   TRUE if another GAS ID is connected, FALSE otherwise

SIDE EFFECTS   None

===========================================================================*/
boolean rr_ms_other_stack_in_dedicated(const gas_id_t gas_id)
{
  gas_id_t other_gas_id;

  for (other_gas_id = GERAN_ACCESS_STRATUM_ID_1; other_gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; other_gas_id++)
  {
    if (other_gas_id != gas_id)
    {
      if (rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(other_gas_id)].cs_connected)
      {
        return(TRUE);
      }
    }
  }

  return(FALSE);
}

/*===========================================================================

FUNCTION      rr_ms_task_start_init

DESCRIPTION   Initialise the DS control module at task startup

DEPENDENCIES  This function must be called before ANY other RR initialisation

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/
void rr_ms_task_start_init(void)
{
  int as_index;

  /* call the local function to reset everything internally */
  rr_ms_reset_internal_variables();

  /* initialise everything that affects external interfaces (NAS etc.) */
  rr_multisim_info.multi_standby_pref = SYS_MODEM_DS_PREF_SINGLE_STANDBY;

  for (as_index = GERAN_DATA_SPACE_INDEX_1; as_index < NUM_GERAN_DATA_SPACES; as_index++)
  {
    rr_multisim_info.gas_data[as_index].paging_group        = RR_MS_INVALID_PAGING_GROUP;
    rr_multisim_info.gas_data[as_index].ps_abort_delay      = 0;
    rr_multisim_info.gas_data[as_index].buffered_page_time  = RR_MS_INVALID_TIME_STAMP;
    rr_multisim_info.gas_data[as_index].missed_page         = RR_MS_INVALID_TIME_STAMP;
    rr_multisim_info.gas_data[as_index].page_cycle          = 0;
    rr_multisim_info.gas_data[as_index].cbch_state          = RR_MS_CBCH_OFFLINE;
    rr_multisim_info.gas_data[as_index].gprs_test_mode      = FALSE;
    memset(&(rr_multisim_info.gas_data[as_index].debug_events), 0, sizeof(rr_ms_debug_info_t));
  }

  /* reset local debug storage */
  rr_ms_subscription_info.multimode_count = -1;
  for (as_index = 0; as_index < MAX_AS_IDS; as_index++)
  {
    rr_ms_subscription_info.subs_identifier[as_index] = SYS_MODEM_AS_ID_NONE;
    rr_ms_subscription_info.subs_capability[as_index] = SUBS_CAPABILITY_NONE;
  }

  /* Register a set of callback functions that can be used for demultiplexing
   * (of CCCH or CBCH blocks) */
  rr_ms_register_channel_sharing();

  return;
}

/*===========================================================================

FUNCTION      rr_ms_store_paging_info

DESCRIPTION   Calculates and stores the paging group based on OTA parameters

DEPENDENCIES  Must be called within the active context of one of the stacks

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/
void rr_ms_store_paging_info(layer1_sys_info_T *sys_info_ptr, word imsi_mod_1000, const gas_id_t gas_id)
{
  timetick_type page_cycle = 0;
  word          page_group = RR_MS_INVALID_PAGING_GROUP;

  if (sys_info_ptr != NULL)
  {
    word n;
    word i;

    if (sys_info_ptr->BS_CCCH_SDCCH_COMB == FALSE)
    {
       n = (9 - sys_info_ptr->BS_AG_BLKS_RES);
    }
    else
    {
       n = (3 - sys_info_ptr->BS_AG_BLKS_RES);
    }

    n *= sys_info_ptr->BS_PA_MFRMS;
    page_cycle = 235 * sys_info_ptr->BS_PA_MFRMS;

    i = (word)(n * sys_info_ptr->BS_CC_CHANS);

    if (i != 0)
    {
      page_group = (imsi_mod_1000 % i) % n;
    }
  }

  /* store the calculated value in this module's data structure */
  rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].paging_group = page_group;
  rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].page_cycle   = page_cycle;
  return;
}

/*===========================================================================

FUNCTION      rr_ms_all_stacks_in_state

DESCRIPTION   Checks the RR control state for all subscriptions

DEPENDENCIES  None

RETURN VALUE  TRUE if all subscriptions are in the specified state

SIDE EFFECTS  None

===========================================================================*/
boolean rr_ms_all_stacks_in_state(const rr_state_T rr_state)
{
  gas_id_t gas_id;

  /* check the state of each data space in turn */
  for (gas_id = GERAN_ACCESS_STRATUM_ID_1; gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; gas_id++)
  {
    if (!rr_is_control_state(rr_state, gas_id))
    {
      return(FALSE);
    }
  }

  return(TRUE);
}


/*===========================================================================

FUNCTION      rr_ms_other_sim_in_background_traffic

DESCRIPTION   Determines if *any* other stack is in a pre-emptable data mode

DEPENDENCIES  Must be called in the context of a single GAS ID

RETURN VALUE  TRUE if WCDMA, CDMA, or another GSM stack are using BG_TRAFFIC

SIDE EFFECTS  None

===========================================================================*/
boolean rr_ms_other_sim_in_background_traffic(const gas_id_t calling_gas_id)
{
  gas_id_t gas_id;

  /* chain availability is only relevant to multi-standby modes */
  if (!geran_in_multi_standby_mode())
  {
    return(FALSE);
  }

  /* check the active GERAN clients first */
  for (gas_id = GERAN_ACCESS_STRATUM_ID_1; gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; gas_id++)
  {
    if ((gas_id != calling_gas_id) && rr_ms_gprs_tuneaway_active(gas_id))
    {
      return(TRUE);
    }
  }

  /* if loop above has not detected a GERAN stack, check the other RATs */
  if (rr_ms_other_rat_in_background_traffic())
  {
    /* it should never be the case that another RAT is active at */
    /* the same time the multimode-capable GERAN stack is online */
    gas_id = calling_gas_id;
    if (geran_is_multi_mode_gas_id(gas_id))
    {
      MSG_GERAN_ERROR_0_G("BG_TRAFFIC flag set by other RAT unexpectedly");
    }
    else
    {
      return(TRUE);
    }
  }

  return(FALSE);
}

/*===========================================================================

FUNCTION      rr_ms_failed_block_penalty

DESCRIPTION   Returns the effective penalty for a failed block quality report

DEPENDENCIES  Must only be called in the context of a single access stratum

RETURN VALUE  Integer penalty to be applied to failed_blocks[] array entry

SIDE EFFECTS  None

===========================================================================*/
static int rr_ms_failed_block_penalty(const gas_id_t gas_id)
{
  int block_penalty;

  if (rr_ms_other_sim_in_background_traffic(gas_id))
  {
    /* Background traffic is active, cope with more unknown blocks */
    /* by applying a smaller penalty than for normal DSDS activity */
    block_penalty = (RR_MS_BLOCK_QUALITY_UNIT >> 1);
  }
  else
  {
    block_penalty = RR_MS_BLOCK_QUALITY_UNIT;
  }

  return(block_penalty);
}

/*===========================================================================

FUNCTION      rr_ms_reassess_cell_broadcast_priority

DESCRIPTION   Called when the PCH failure counts are updated, to determine
              whether failures might be due to collisions with CB activity

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/
static void rr_ms_reassess_cell_broadcast_priority(const gas_id_t gas_id)
{
  boolean mac_idle;

  /* query the current MAC status (because CBCH is deprioritised if MAC is not idle) */
  mac_idle = (boolean)(MAC_IDLE == rr_macinfo_get_mac_status(gas_id));

  /* reassess the CBCH prioritisation across all the stacks */
  rr_ms_update_cell_broadcast_priority(mac_idle, gas_id);

  return;
}

/*===========================================================================

FUNCTION      rr_ms_adjust_block_quality

DESCRIPTION   Called when an UNKNOWN block quality is reported by Layer 1

DEPENDENCIES  None

RETURN VALUE  GL1_BLOCK_QUALITY_BAD if UNKNOWN block count exceeds a limit

SIDE EFFECTS  May update CBCH prioritisation

===========================================================================*/
gl1_block_quality_t rr_ms_adjust_block_quality(const l2_channel_type_T l2_channel_type, const gas_id_t gas_id)
{
  int as_index;

  /* check for unexpected channel conditions */
  if (L2_RACH == l2_channel_type)
  {
    /* emit a warning: this implies that Layer 1 could not get permission to decode an AGCH block */
    MSG_GERAN_ERROR_0_G("Unexpected RACH channel type for UNKNOWN block quality report");
  }

  /* determine which access stratum is in use */
  as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  if (as_index < NUM_GERAN_DATA_SPACES)
  {
    /* increment the number of block read failures for that access stratum */
    rr_multisim_info.gas_data[as_index].failed_blocks += rr_ms_failed_block_penalty(gas_id);

    /* reassess the CBCH prioritisation across all the stacks */
    rr_ms_reassess_cell_broadcast_priority(gas_id);

    /* if the new value exceeds the watermark, the block is treated as bad */
    if (rr_multisim_info.gas_data[as_index].failed_blocks >= RR_MS_BLOCK_QUALITY_RATE)
    {
      /* reset the local counter */
      rr_multisim_info.gas_data[as_index].failed_blocks = 0;
      MSG_GERAN_HIGH_0_G("Unknown block watermark exceeded");
      return(GL1_BLOCK_QUALITY_BAD);
    }
  }

  return(GL1_BLOCK_QUALITY_UNKNOWN);
}

/*===========================================================================

FUNCTION      rr_ms_reset_block_quality

DESCRIPTION   Resets the idle mode block quality counters

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  May toggle priority for CBCH decoding

===========================================================================*/
void rr_ms_reset_block_quality(const gas_id_t gas_id)
{
  int as_index;

  /* determine which access stratum is in use */
  as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  if (as_index < NUM_GERAN_DATA_SPACES)
  {
    /* reset the number of block read failures for that access stratum */
    rr_multisim_info.gas_data[as_index].failed_blocks = 0;
    /* and the missed page timestamp */
    rr_multisim_info.gas_data[as_index].missed_page   = RR_MS_INVALID_TIME_STAMP;
  }

  /* reassess the CBCH prioritisation across all the stacks */
  rr_ms_reassess_cell_broadcast_priority(gas_id);

  return;
}

/*===========================================================================

FUNCTION       rr_ms_l1_blocking_message_sent

DESCRIPTION    Indicates that a message has been sent via the L1 interface
               that may potentially cause L1 to be blocked by TRM

DEPENDENCIES   Device must be operating in DSDx mode

RETURN VALUE   VOID

SIDE EFFECTS   May start/stop the GL1 interface guard timer

===========================================================================*/
void rr_ms_l1_blocking_message_sent(const rr_l1_message_id_enum_T required_msg_id, const gas_id_t gas_id)
{
  if ((required_msg_id != RR_MS_INVALID_L1_MESSAGE)    &&
      (gas_id          <= GERAN_ACCESS_STRATUM_ID_MAX) &&
      RR_MULTI_SIM_MODE_ENABLED)
  {
    int as_index;

    /* establish a data space index for the supplied GAS ID */
    as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    /* subsequent action depends on the message being sent to GL1 */
    if ((MPH_STOP_GSM_MODE_CNF == required_msg_id) ||
        (MPH_RESET_CNF         == required_msg_id))
    {
      /* this access stratum is either shutting down or resetting Layer 1 */
      rr_multisim_info.gas_data[as_index].required_l1_message = RR_MS_INVALID_L1_MESSAGE;
      (void)rr_timer_cancel(RR_DUAL_SIM_INTERFACE_TIMER, gas_id);

      /* if suspended, behave as though the access stratum is resuming now */
      if (rr_multisim_info.gas_data[as_index].gl1_interface_state != RR_MS_INTERFACE_ACTIVE)
      {
        if ((rr_multisim_info.gas_data[as_index].gl1_interface_state == RR_MS_INTERFACE_HIBERNATED) &&
            (required_msg_id                                         == MPH_STOP_GSM_MODE_CNF))
        {
          /* if the access stratum is about to hibernate, don't resume yet */
          MSG_GERAN_HIGH_0_G("GL1 is being shut down for hibernation");
        }
        else
        {
          /* normal suspension is in effect, or a panic reset was triggered */
          rr_ms_prepare_for_resumption(FALSE, gas_id);
        }
      }
      else
      {
        /* ensure that this access stratum is not flagged as being CS connected */
        rr_ms_cs_released(gas_id);
      }
    }
    else
    {
      /* new primitive being sent to Layer 1, reset the interface flags */
      rr_ms_set_aborted_interface(FALSE, gas_id);

      /* log an appropriate event in the debug buffer */
      rr_ms_store_debug_event(RR_MS_BLOCKING_MSG_SENT, gas_id);

      /* store the required GL1 response to this primitive */
      rr_multisim_info.gas_data[as_index].required_l1_message = required_msg_id;

      /* (re)start the interface guard timer for the active AS */
      (void)rr_timer_start(RR_DUAL_SIM_INTERFACE_TIMER, RR_MS_INTERFACE_TIMEOUT, gas_id);
    }

    MSG_GERAN_HIGH_1_G("GRR requires GL1 msg 0x%02X", (int)required_msg_id);
  }

  return;
}

/*===========================================================================

FUNCTION       rr_ms_access_stratum_deactivated

DESCRIPTION    Completes clean-up in this module when the AS is deactivated

DEPENDENCIES   Should be called just before deactivation is confirmed to NAS

RETURN VALUE   VOID

SIDE EFFECTS   None

===========================================================================*/
void rr_ms_access_stratum_deactivated(const sys_modem_as_id_e_type nas_id, const boolean stop_mode_success)
{
  gas_id_t gas_id;
  int      as_index;

  /* convert the supplied NAS ID to a GERAN access stratum identifier */
  gas_id   = geran_map_nas_id_to_gas_id(nas_id);
  as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  /* ensure that suspension or hibernation is cleaned up appropriately */
  if (stop_mode_success == TRUE)
  {
    /* cancel the termination timer if it is running */
    if (rr_timer_cancel(RR_DUAL_SIM_TERMINATE_TIMER, gas_id))
    {
      MSG_GERAN_HIGH_0_G("Cancelled termination timer");
    }
  }
  else
  {
    rr_ms_prepare_for_resumption(FALSE, gas_id);
  }

  /* reset Layer 1 interface control flags */
  rr_multisim_info.gas_data[as_index].aborted_l1_interface  = FALSE;
  rr_multisim_info.gas_data[as_index].required_l1_message   = RR_MS_INVALID_L1_MESSAGE;
  rr_multisim_info.gas_data[as_index].failed_blocks         = 0;

  /* reset GMAC interface control flags and paging group info */
  rr_multisim_info.gas_data[as_index].paging_group          = RR_MS_INVALID_PAGING_GROUP;
  rr_multisim_info.gas_data[as_index].missed_page           = RR_MS_INVALID_TIME_STAMP;
  rr_multisim_info.gas_data[as_index].cbch_state            = RR_MS_CBCH_OFFLINE;

  /* update the Priority Inversion Manager for this subscription */
  rr_ms_acquisition_manager(RR_MS_GAS_ID_SERVICE_NOTIFIED, gas_id);

  MSG_GERAN_HIGH_0_G("Deactivation of GERAN access stratum");

  return;
}

/*===========================================================================

FUNCTION       rr_ms_plmn_list_search_valid

DESCRIPTION    Sanity checks a PLMN list request from NAS

DEPENDENCIES   None

RETURN VALUE   TRUE if the request can be actioned in the current state

SIDE EFFECTS   None

===========================================================================*/
boolean rr_ms_plmn_list_search_valid(const rr_state_T rr_state, gas_id_t gas_id)
{
  if ((RR_MULTI_SIM_MODE_ENABLED == FALSE) &&
      (RR_INACTIVE               == rr_state))
  {
#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
    trm_get_info_return_type get_granted;

    if (TRM_DENIAL != grm_get_granted(TRM_UMTS, &get_granted, gas_id))
    {
      /* UMTS client already holds a chain */
      MSG_GERAN_ERROR_0_G("Cannot action this PLMN list request while TRM_UMTS is active");
      return(FALSE);
    }
#else
    #error code not present
#endif /* !PERLUTF && !TEST_FRAMEWORK */
  }

  return(TRUE);
}

/*===========================================================================

FUNCTION       rr_ms_cell_selection_deactivating

DESCRIPTION    Notifies the controller that cell selection is deactivating

DEPENDENCIES   Special case for deactivation when a power scan is pending

RETURN VALUE   VOID

SIDE EFFECTS   May ask GL1 to abort or complete a pending power scan

===========================================================================*/
void rr_ms_cell_selection_deactivating(const gas_id_t gas_id)
{
  /* log an appropriate event in the debug buffer */
  rr_ms_store_debug_event(RR_MS_GRR_ABORT_BY_FUNC, gas_id);

#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    rr_l1_message_id_enum_T pending_msg;

    /* determine which message is pending from GL1 (this function is intended for power scans only) */
    pending_msg = rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].required_l1_message;

    /* check whether GRR is still waiting for a power scan response (and has not already aborted) */
    if (MPH_POWER_SCAN_CNF == pending_msg)
    {
      if (rr_ms_interface_was_aborted(gas_id))
      {
        /* GRR has already sent a DS_ABORT_IND for this power scan */
        MSG_GERAN_HIGH_0_G("Interface aborted already, waiting for MPH_POWER_SCAN_CNF");
      }
      else
      {
        /* recover the blocked interface in the usual way, and */
        /* allow the power scan to complete if it has TRM lock */
        rr_ms_recover_blocked_interface(gas_id);
      }
    }
    else if (RR_MS_INVALID_L1_MESSAGE != pending_msg)
    {
      MSG_GERAN_ERROR_1_G("Cell selection deactivation with msg 0x02X pending", (int)pending_msg);
    }
  }
#endif /* !PERLUTF && !TEST_FRAMEWORK */

  return;
}

/*===========================================================================

FUNCTION       rr_ms_power_scan_aborted

DESCRIPTION    Notifies the controller that the PSCAN module is aborting

DEPENDENCIES   None

RETURN VALUE   VOID

SIDE EFFECTS   May ask GL1 to abort or complete a pending power scan

===========================================================================*/
void rr_ms_power_scan_aborted(const gas_id_t gas_id)
{
  (void)rr_ms_abort_pending_interface(RR_MS_GRR_ABORT_BY_PSCN, gas_id);
  return;
}

/*===========================================================================

FUNCTION       rr_ms_notify_sys_info_ignored

DESCRIPTION    Notifies the DSDS controller that a DL_UNIT_DATA_IND was ignored

DEPENDENCIES   Must be called only in the context of a single GAS ID

RETURN VALUE   VOID

SIDE EFFECTS   May reinstate a previously cleared blocking message

===========================================================================*/
void rr_ms_notify_sys_info_ignored(const gas_id_t gas_id)
{
  if (RR_MULTI_SIM_MODE_ENABLED)
  {
    int as_index;

    as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    /* check whether the system info message cleared a pending interface */
    if ((rr_multisim_info.gas_data[as_index].last_cleared_l1_msg != RR_MS_INVALID_L1_MESSAGE) &&
        (rr_multisim_info.gas_data[as_index].required_l1_message == RR_MS_INVALID_L1_MESSAGE))
    {
      MSG_GERAN_ERROR_1_G("Reinstate pending interface GL1 msg 0x%02X", (int)(rr_multisim_info.gas_data[as_index].last_cleared_l1_msg));
      rr_ms_l1_blocking_message_sent(rr_multisim_info.gas_data[as_index].last_cleared_l1_msg, gas_id);
    }

    /* reset the last cleared message ID */
    rr_multisim_info.gas_data[as_index].last_cleared_l1_msg = RR_MS_INVALID_L1_MESSAGE;
  }

  return;
}

/*===========================================================================

FUNCTION     rr_ms_confirm_mm_ps_abort_cb_fn

DESCRIPTION  Callback function called when RR-CONTROL confirm PS is blocked.
             RR_DUAL_SIM_PS_ABORT_TIMER_1 is started.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_confirm_mm_ps_abort_cb_fn(const gas_id_t gas_id)
{
  // Start the PS abort timer using the specified time-out
  // PS will remain blocked as long as this timer is running
  if (rr_timer_start(RR_DUAL_SIM_PS_ABORT_TIMER_1, RR_DUAL_SIM_MM_PS_ABORT_TIMEOUT, gas_id))
  {
    MSG_GERAN_HIGH_1_G("RR_DUAL_SIM_PS_ABORT_TIMER_1 started for %dms", RR_DUAL_SIM_MM_PS_ABORT_TIMEOUT);
  }

  // Send a confirmation to MM
  rr_ms_send_rr_ps_abort_cnf(gas_id);

  return;
}

/*===========================================================================

FUNCTION       rr_ms_process_ps_abort_req

DESCRIPTION    Called from RR-PS-DOMAIN when RR_PS_ABORT_REQ is received from MM

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void rr_ms_process_ps_abort_req(const gas_id_t gas_id)
{
  /* reset the failed block counts for all stacks, */
  /* to avoid unnecessary DSF reselection attempts */
  rr_ms_reset_failed_blocks();

  // Request RR-CONTROL to block PS access
  rr_send_imsg_no_ps_access_req(
    FALSE,                            // del_all_pdus
    rr_ms_confirm_mm_ps_abort_cb_fn,  // confirm_cb_fn
    gas_id
  );
}

/*===========================================================================

FUNCTION     rr_ms_confirm_l1_ds_abort_cb_fn

DESCRIPTION  Callback function called when RR-CONTROL confirm PS is blocked.
             RR_DUAL_SIM_PS_ABORT_TIMER_1 is started.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

===========================================================================*/
static void rr_ms_confirm_l1_ds_abort_cb_fn(const gas_id_t gas_id)
{
  // Start the PS abort timer using the specified time-out
  // PS will remain blocked as long as this timer is running
  // Extract the as_index
  int as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
  if (rr_timer_start(RR_DUAL_SIM_PS_ABORT_TIMER_1, rr_multisim_info.gas_data[as_index].ps_abort_delay, gas_id))
  {
    MSG_GERAN_HIGH_1_G("RR_DUAL_SIM_PS_ABORT_TIMER_1 started for %dms", rr_multisim_info.gas_data[as_index].ps_abort_delay);
  }

  return;
}

/*===========================================================================

FUNCTION       rr_ms_process_l1_ds_abort_ind

DESCRIPTION    Called from RR-MAIN from MPH_L1_DS_ABORT_IND is received from L1

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void rr_ms_process_l1_ds_abort_ind(mph_l1_ds_abort_ind_T *msg_ptr)
{
  gas_id_t gas_id;

  RR_NULL_CHECK_FATAL(msg_ptr);

  /* extract the GAS ID from the message */
  gas_id = msg_ptr->gas_id;

  if ((RR_DS_ABORT_TRANSFER == msg_ptr->abort_type) ||
      (RR_DS_ABORT_TIMEDOUT == msg_ptr->abort_type))
  {
    /* reset the failed block counts for all stacks, */
    /* to avoid unnecessary DSF reselection attempts */
    rr_ms_reset_failed_blocks();

    if (rr_mac_ps_access_blocked(gas_id))
    {
      /* GMAC has already been told to disable GPRS activity */
      /* so this new abort indication from GL1 is either:    */
      /* a) a repetition of RR_DS_ABORT_TIMEDOUT, which usually means GL1 is stuck, or */
      /* b) a collision between an internal GL1 abort and some other PS release in GRR */
      /* For each of these scenarios, it should be OK to ignore the new abort from GL1 */
      if (RR_DS_ABORT_TIMEDOUT == msg_ptr->abort_type)
      {
        MSG_GERAN_ERROR_0_G("Duplicate RR_DS_ABORT_TIMEDOUT ignored");
        return;
      }
      else /* RR_DS_ABORT_TRANSFER */
      {
        rr_ps_abort_block_t ps_abort_active;

        /* if no PS abort process is active, start the secondary timer now */
        ps_abort_active = rr_ms_ps_abort_process_active(gas_id);
        if (RR_PS_ABORT_NOT_ACTIVE == ps_abort_active)
        {
          /* while this timer is running, no new UL or DL establishment can take place */
          if (rr_timer_start(RR_DUAL_SIM_PS_ABORT_TIMER_2, msg_ptr->timeout_ms, gas_id))
          {
            MSG_GERAN_HIGH_1_G("RR_DUAL_SIM_PS_ABORT_TIMER_2 started for %dms", msg_ptr->timeout_ms);
          }
        }
        MSG_GERAN_HIGH_1_G("RR_DS_ABORT_TRANSFER, MAC blocked (abort %d)", (int)ps_abort_active);
        return;
      }
    }

    // Store the delay required for the RR_DUAL_SIM_PS_ABORT_TIMER_1 timer
    if (msg_ptr->timeout_ms == 0)
    {
      rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].ps_abort_delay = RR_DUAL_SIM_L1_PS_ABORT_TIMEOUT;;
      MSG_GERAN_HIGH_1_G("PS abort delay corrected to %d", RR_DUAL_SIM_L1_PS_ABORT_TIMEOUT);
    }
    else
    {
      rr_multisim_info.gas_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)].ps_abort_delay = msg_ptr->timeout_ms;    
    }

    // Request RR-CONTROL to block PS access
    rr_send_imsg_no_ps_access_req(
      FALSE,                             // del_all_pdus
      rr_ms_confirm_l1_ds_abort_cb_fn,   // confirm_cb_fn
      gas_id                             // gas_id
    );
  }
  else
  {
    MSG_GERAN_ERROR_1_G("Unexpected abort type %d received from GL1", (int)(msg_ptr->abort_type));
  }

  return;
}

/*===========================================================================

FUNCTION     rr_ms_ps_abort_timer_expiry

DESCRIPTION  Called when either RR_DUAL_SIM_PS_ABORT_TIMER_x timer expires.
             PS access is resumed for RR_DUAL_SIM_PS_ABORT_TIMER_1 only

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS RR_DUAL_SIM_PS_ABORT_TIMER_2 is cancelled

===========================================================================*/
void rr_ms_ps_abort_timer_expiry(const rr_timer_id_e ps_abort_id, const gas_id_t gas_id)
{
  if (RR_DUAL_SIM_PS_ABORT_TIMER_1 == ps_abort_id)
  {
    // Request PS to be enabled
    rr_send_imsg_ps_access_req(
      FALSE,   // purge_gmm_signalling
      NULL,    // confirm_cb_fn
      gas_id   // gas_id
    );
  }

  /* always cancel the secondary timer if it is still running */
  (void)rr_timer_cancel(RR_DUAL_SIM_PS_ABORT_TIMER_2, gas_id);

  return;
}

/*===========================================================================

FUNCTION     rr_ms_ps_abort_process_active

DESCRIPTION  Determines the status of PS Abort processing

DEPENDENCIES None

RETURN VALUE RR_PS_ABORT_NOT_ACTIVE
             RR_PS_ABORT_ACTIVE_WILL_RESUME
             RR_PS_ABORT_ACTIVE_TBF_BLOCKED

SIDE EFFECTS None

===========================================================================*/
rr_ps_abort_block_t rr_ms_ps_abort_process_active(const gas_id_t gas_id)
{
  if (rr_timer_is_running(RR_DUAL_SIM_PS_ABORT_TIMER_1, gas_id))
  {
    /* this timer will resume PS access when it expires */
    return(RR_PS_ABORT_ACTIVE_WILL_RESUME);
  }
  else if (rr_timer_is_running(RR_DUAL_SIM_PS_ABORT_TIMER_2, gas_id))
  {
    /* this timer only blocks TBF establishment while it is active */
    return(RR_PS_ABORT_ACTIVE_TBF_BLOCKED);
  }

  return(RR_PS_ABORT_NOT_ACTIVE);
}

/*===========================================================================

FUNCTION      rr_ms_x2g_acquisition_started

DESCRIPTION   Informs Priority Inversion Manager that GRR is about to start
                       start GSM acquisition for an X2G procedure
                       
DEPENDENCIES  This function starts PIMS for regular acquisition.

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/
void rr_ms_x2g_acquisition_started(const gas_id_t gas_id)
{
  /* pass the acquiring  event into the Priority Inversion Manager */
  rr_ms_acquisition_manager(RR_MS_GAS_ID_STARTS_ACQUIRING, gas_id);

  return;
}

#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*===========================================================================

FUNCTION       rr_multi_sim_mode_enabled

DESCRIPTION    Determines whether the GERAN AS is in Multi SIM mode

DEPENDENCIES   None

RETURN VALUE   TRUE if Multi SIM mode is enabled in the GERAN AS

SIDE EFFECTS   None

===========================================================================*/
boolean rr_multi_sim_mode_enabled(void)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  return(RR_MULTI_SIM_MODE_ENABLED);
#else
  return(FALSE);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
}

/*===========================================================================

FUNCTION       rr_ms_get_active_sim_count

DESCRIPTION    Returns the number of active SIMs that NAS have indicated

DEPENDENCIES   None

RETURN VALUE   VOID

SIDE EFFECTS   None

===========================================================================*/
int rr_ms_get_active_sim_count(void)
{
#ifdef FEATURE_DUAL_SIM
  if (SYS_MODEM_DS_PREF_SINGLE_STANDBY == rr_multisim_info.multi_standby_pref)
  {
    return(1);
  }
  else if (SYS_MODEM_DS_PREF_DUAL_STANDBY == rr_multisim_info.multi_standby_pref)
  {
    return(2);
  }
#ifdef FEATURE_TRIPLE_SIM
  else if (SYS_MODEM_DS_PREF_TRIPLE_STANDBY == rr_multisim_info.multi_standby_pref)
  {
    return(3);
  }
#endif /* FEATURE_TRIPLE_SIM */
  else
  {
    ERR_GERAN_FATAL_1("Unexpected multi-standby preference %d supplied by NAS",
              (int)(rr_multisim_info.multi_standby_pref));
  }
#endif /* FEATURE_DUAL_SIM */

  return(1);
}

boolean rr_multi_sim_api_valid(const gas_id_t gas_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (SYS_MODEM_AS_ID_1 != geran_map_gas_id_to_nas_id(gas_id))
  {
    return(FALSE);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return(TRUE);
}

static const gs_queue_id_T gl1_queue_ids[NUM_GERAN_DATA_SPACES] =
          { INITIAL_VALUE_DIFFERENT(GS_QUEUE_LAYER_1,
                                    GS_QUEUE_LAYER_1_2,
                                    GS_QUEUE_LAYER_1_3) };

/*!
 * \brief Returns the GS queue ID based on a given GS queue ID and a GAS ID.
 *
 * \param queue_id (in)
 * \param gas_id (in)
 *
 * \return gs_queue_id_T - GS queue ID to use to send messages.
 */
gs_queue_id_T rr_ms_get_gs_queue_id(const gs_queue_id_T queue_id, const gas_id_t gas_id)
{
#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
  if (GS_QUEUE_LAYER_1 == queue_id)
  {
    return(gl1_queue_ids[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)]);
  }
#endif /* !PERLUTF && !TEST_FRAMEWORK */

  /* return the supplied queue_id */
  return(queue_id);
}

/*!
 * \brief Called by inter-RAT modules to report a failure
 *
 * \param failure_type, gas_id (in)
 *
 * \return VOID
 */
void rr_ms_x2g_procedure_failure(const rr_proc_failure_type_t failure_type, const gas_id_t gas_id)
{
  boolean lock_exchange_for_return_to_x_rat;

  /* determine whether a lock exchange is allowed for the return to the X-RAT */
  switch(failure_type)
  {
  case RR_X2G_REDIRECTION_FAILURE:
  case RR_X2G_HANDOVER_FAILURE:
    lock_exchange_for_return_to_x_rat = TRUE;
    break;

  case RR_X2G_RESELECTION_FAILURE:
#ifdef FEATURE_LTE
    if (RR_RAT_LTE == rr_get_x2g_resel_source_rat(gas_id))
    {
      lock_exchange_for_return_to_x_rat = TRUE;
    }
    else
#endif /* FEATURE_LTE */
    {
      lock_exchange_for_return_to_x_rat = FALSE;
    }
    break;

  default:
    lock_exchange_for_return_to_x_rat = FALSE;
    break;
  }

  /* arrange TRM lock exchange with other RAT if required */
  if (geran_was_lock_exchanged(GERAN_EXCHANGE_XTOG, gas_id) &&
      (TRUE == lock_exchange_for_return_to_x_rat))
  {
    /* advise GL1 to exchange rather than release this client */
    /* note that the direction indicator is the OPPOSITE here */
    geran_set_exchange_on_release(GERAN_EXCHANGE_GTOX, gas_id);

    /* then clear the X2G flag for future procedures */
    geran_clear_lock_exchanged(GERAN_EXCHANGE_XTOG, gas_id);
  }
  else
  {
    /* clear the release flag so that GL1 doesn't act on it */
    geran_set_exchange_on_release(GERAN_EXCHANGE_NULL, gas_id);

    /* and clear the G2X flag for future procedures */
    geran_clear_lock_exchanged(GERAN_EXCHANGE_GTOX, gas_id);

    MSG_GERAN_HIGH_0_G("Inter-RAT exchange not used for this procedure");
  }

  return;
}

/* EOF */
