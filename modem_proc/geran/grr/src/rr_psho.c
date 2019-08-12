

/*****************************************************************************
***
*** TITLE
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_psho.c#1 $
***
*** DESCRIPTION
***
***  Contains GPRS PS Handover functionality
***
*** Copyright (c) 2005-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or
*** software is regulated by the U.S. Government. Diversion contrary to U.S.
*** law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h"  /* Includes FEATURE_... defines */

#ifdef FEATURE_GPRS_PS_HANDOVER

#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_gprs_defs.h"
#include "rr_gprs_debug.h"
#include "rr_defs.h"
#include "rr_general.h"
#include "ms.h"
#include "rr_resel_calcs.h"
#include "rr_mm_send.h"
#include "rr_mac_send.h"
#include "rr_psho.h"
#include "rr_control.h"
#include "rr_resel.h"
#include "rr_resel_utils.h"
#include "rr_resel_nacc.h"
#include "rr_sys_info_ncell.h"
#include "rr_sys_info_pcell.h"
#include "rr_sys_info_si.h"
#include "rr_sys_info_pacch.h"
#include "rr_g2w_handover.h"
#include "rr_cell_acq.h"
#include "csn_apt.h"
#include "rr_gprs_main.h"
#include "rr_ded_config.h"
#include "rr_ps_domain.h"
#include "rr_l1_idle_mode.h"
#include "rr_multi_sim.h"
#include "stringl/stringl.h"


/*****************************************************************************
***
***     External Functions & Variables
***
*****************************************************************************/


/*****************************************************************************
***
***     Local Typedefs, #defines & Variables
***
*****************************************************************************/
typedef enum
{
  RR_PSHO_NULL,
  RR_PSHO_COLLECT_SYS_INFO,
  RR_PSHO_WAIT_FOR_MAC_CNF,
  RR_PSHO_WAIT_FOR_NAS_CNF,

  #ifdef FEATURE_WCDMA

  #endif /* FEATURE_WCDMA */

  RR_PSHO_STATE_MAX

} psho_control_state_t;

typedef struct
{
  psho_control_state_t    psho_state;
  psho_control_state_t    old_psho_state;
  psho_details_t          rr_psho_details;

} rr_psho_data_t;

static rr_psho_data_t rr_psho_data[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Function Prototypes
***
*****************************************************************************/

/*****************************************************************************
***
***     Functions
***
*****************************************************************************/

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_psho_data_t*
 */
static rr_psho_data_t *rr_psho_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_psho_data[as_index];
}

void rr_psho_save_details(
  const psho_details_t *psho_cmd,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_psho_data_t *rr_psho_data_ptr = rr_psho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_psho_data_ptr);

  if (psho_cmd != NULL)
  {
   rr_psho_data_ptr->rr_psho_details = *psho_cmd;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("PSHO: psho_cmd NULL");
  }
}

void rr_psho_clean_details(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_psho_data_t *rr_psho_data_ptr = rr_psho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_psho_data_ptr);

  memset(&rr_psho_data_ptr->rr_psho_details, 0, sizeof(psho_details_t));
}


psho_details_t * rr_psho_get_details(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_psho_data_t *rr_psho_data_ptr = rr_psho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_psho_data_ptr);

  return &rr_psho_data_ptr->rr_psho_details;
}

boolean rr_psho_in_progress(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_psho_data_t *rr_psho_data_ptr = rr_psho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_psho_data_ptr);

  return (rr_psho_data_ptr->psho_state != RR_PSHO_NULL);
}

static void rr_psho_reset(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_psho_data_t *rr_psho_data_ptr = rr_psho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_psho_data_ptr);

  rr_psho_data_ptr->psho_state = RR_PSHO_NULL;


  rr_psho_clean_details(gas_id);
}




static void rr_psho_state_exit(
  psho_control_state_t state,
  const gas_id_t gas_id
)
{
  switch (state)
  {
    case RR_PSHO_NULL:
    case RR_PSHO_COLLECT_SYS_INFO:
    case RR_PSHO_WAIT_FOR_MAC_CNF:
    case RR_PSHO_WAIT_FOR_NAS_CNF:
      /* don't need to do anything at this moment */
      break;



    default:
      MSG_GERAN_ERROR_1_G("PSHO: illegal state %d", state);
      break;
  }
}

static void rr_psho_state_entry(
  psho_control_state_t state,
  const gas_id_t gas_id
)
{
  switch (state)
  {
    case RR_PSHO_NULL:
      rr_psho_reset(gas_id);
      break;

    case RR_PSHO_COLLECT_SYS_INFO:
    case RR_PSHO_WAIT_FOR_MAC_CNF:
    case RR_PSHO_WAIT_FOR_NAS_CNF:
      /* don't need to do anything at this moment */
      break;



    default:
      MSG_GERAN_ERROR_1_G("PSHO: illegal state %d", state);
      break;
  }
}

/**
 * PSHO complete - success, transit to GRR_CAMPED on new cell
 */
static void rr_psho_camp_on_new_cell(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_ptr = NULL;

  scell_ptr = rr_pcell_becomes_scell(gas_id);

  scell_ptr->gsm.camped_on_flag = TRUE;

  rr_send_service_ind(
    MM_AS_SERVICE_AVAILABLE,
    &(scell_ptr->gsm.location_area_identification),
    &(scell_ptr->gsm),
    gas_id
  );

  rr_gprs_init_bad_snr_counter(gas_id);
  rr_gprs_reset_reselection_list(gas_id);
  rr_gprs_nacc_reset(gas_id);
  rr_initialize_SI2q_instances(gas_id);
  rr_initialize_SI2ter_instances(gas_id);

  /* Set the current band based on the ARFCN of the serving cell */
  rr_set_current_band((rr_internal_band_T)GET_ARFCN_BAND(scell_ptr->gsm.BCCH_ARFCN), gas_id);

  rr_hz_update_zone_info(RR_HZ_UPDATE_INFO_ONLY, gas_id);
  rr_hz_notify_state_change(RR_HZ_STATE_IDLE, gas_id);

  scell_ptr->l1_idle_mode = grr_determine_l1_idle_mode(scell_ptr);

  if (scell_ptr->l1_idle_mode == GRR_L1_IDLE_MODE_CCCH)
  {
    rr_gprs_build_ba_gprs_from_si2(scell_ptr, gas_id);

    rr_gprs_reassign_band((NCELL_FREQ_LIST        |
                           IDLE_BA_FREQ_LIST      |
                           CBCH_FREQ              |
                           CELL_ALLOC_FREQ_LIST   |
                           MOBILE_ALLOC_FREQ_LIST),
                           TRUE,
                          gas_id);

    if (scell_ptr->bcch_sys_info.si_status_ind)
    {
      (void)rr_gprs_send_sysinfo_status(0, gas_id);
    }
    else
    {
      /* get the remaining SIs */
      rr_gprs_activate_camped_sys_info_handling(gas_id);
    }
  }
  rr_gprs_update_l1_params(gas_id);
  grr_activate_reselection_if_allowed(gas_id);
}


/**
 * PSHO complete - failure, back to GRR_CAMPED on old cell
 */
static void rr_psho_camp_on_old_cell(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *p_scell = rr_gprs_get_scell_info(gas_id);

  if (p_scell == NULL)
  {
    MSG_GERAN_ERROR_0_G("scell NULL");
    return;
  }

  rr_gprs_build_ba_gprs_from_si2(p_scell, gas_id);

  rr_gprs_activate_camped_sys_info_handling(gas_id);
  grr_activate_reselection_if_allowed(gas_id);
  rr_gprs_update_l1_params(gas_id);

}

/**
 * Returns the name of the PSHO in string form
 *
 * @param PSHO State
 *
 * @return Name of the state in string form
 */
static char * rr_psho_state_name(
  psho_control_state_t ps_ho_state
)
{
  char *s = "";
  switch (ps_ho_state)
  {
    case RR_PSHO_NULL:
      s = "NULL";
      break;

    case RR_PSHO_COLLECT_SYS_INFO:
      s = "COLLECT_SYS_INFO";
      break;

    case RR_PSHO_WAIT_FOR_MAC_CNF:
      s = "WAIT_FOR_MAC";
      break;

    case RR_PSHO_WAIT_FOR_NAS_CNF:
      s = "WAIT_FOR_NAS";
      break;



    default:
      s = "Unknown";
      break;
  }

  return s;
} /* rr_psho_state_name */

static void rr_psho_state_transition(
  const psho_control_state_t next_psho_state,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_psho_data_t *rr_psho_data_ptr = rr_psho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_psho_data_ptr);

  MSG_SPRINTF_2(MSG_SSID_DFLT,
                MSG_LEGACY_HIGH,
                "PSHO state: %s -> %s",
                rr_psho_state_name(rr_psho_data_ptr->psho_state), rr_psho_state_name(next_psho_state)
               );
  rr_psho_state_exit(rr_psho_data_ptr->psho_state, gas_id);
  rr_psho_data_ptr->psho_state = next_psho_state;
  rr_psho_state_entry(rr_psho_data_ptr->psho_state, gas_id);
}



/**
 * Implements the PS Handover state machine
 *
 * @param event_in Event for the state machine
 * @param new_message       Input message (only valid when the event is EV_INPUT_MESSAGE)
 *
 * @return New event for RR state machine. One of:
 *         EV_NO_EVENT       - No action should be taken<BR>
 *         EV_PSHO_COMPLETE - PS handover succeeded or failed
 */

rr_event_T rr_psho_control(
  rr_event_T event_in,
  rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
)
{
  rr_event_T event_out = EV_NO_EVENT;
  byte message_id = 0;
  byte message_set = 0;
  rr_event_T psho_event     = EV_NO_EVENT;
  rr_event_T new_psho_event = EV_NO_EVENT;

  // Obtain a pointer to the module data
  rr_psho_data_t *rr_psho_data_ptr = rr_psho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_psho_data_ptr);

  psho_event = rr_extract_message_header_details(event_in, message_ptr,
               &message_set,&message_id);

  while (psho_event != EV_NO_EVENT)
  {
    if (psho_event == EV_RESET_SOFTWARE)
    {
      rr_psho_reset(gas_id);
    }

    switch (rr_psho_data_ptr->psho_state)
    {
      case RR_PSHO_NULL:
      {
        switch (psho_event)
        {
          case EV_PSHO_STARTED:
            rr_psho_state_transition(RR_PSHO_COLLECT_SYS_INFO, gas_id);
            break;





          default:
            MSG_GERAN_ERROR_2_G("unexpected event %d in state %d", psho_event, rr_psho_data_ptr->psho_state);
            break;
        }
      }
      break;

      case RR_PSHO_COLLECT_SYS_INFO:
      {
        /* EV_PSHO_CONTINUE -  SI/PSI is enough, LAI is not forbidden, PLMN match
         * EV_PSHO_REJECTED -  target cell invalid
         */
        if (psho_event == EV_PSHO_CONTINUE)
        {
          MSG_GERAN_HIGH_0_G("target cell sys info collected");
          rr_send_grr_mac_psho_req(PSHO_GTOG, gas_id);

          rr_psho_state_transition(RR_PSHO_WAIT_FOR_MAC_CNF, gas_id);

          event_out = EV_PSHO_CONTINUE;
        }
        else if (psho_event == EV_PSHO_REJECTED)
        {
          MSG_GERAN_HIGH_0_G("handover rejected, sys info collecting failure");
          rr_gprs_send_pccf(PSHO_FAILURE_REASON_OTHERS, gas_id);
          rr_psho_camp_on_old_cell(gas_id);
          rr_psho_state_transition(RR_PSHO_NULL, gas_id);
          event_out = EV_PSHO_COMPLETE;
        }
        else
        {
          /* all other messages and events should be forward to NACC procedure */
          new_psho_event = rr_resel_control(psho_event, message_ptr, gas_id);
        }
      }
      break;

      case RR_PSHO_WAIT_FOR_MAC_CNF:
      {
        if (psho_event == EV_INPUT_MESSAGE && message_ptr != NULL)
        {
          if ((message_set == MS_MAC_RR) && (message_id == (byte)MAC_GRR_PSHO_CNF))
          {
            /*extract the message type from the mac_grr_data_ind*/
            mac_grr_psho_cnf_t *p_mac_grr_psho_cnf = (mac_grr_psho_cnf_t *)message_ptr;

            MSG_GERAN_HIGH_1_G("received MAC_GRR_PSHO_CNF, status = %d",
              p_mac_grr_psho_cnf->psho_status);

            if (p_mac_grr_psho_cnf->psho_status == PSHO_SUCCESS)
            {
              /* don't declare PSHO_COMPLETE until get confirmations from NAS */
              rr_send_ps_handover_ind(gas_id);
              rr_psho_state_transition(RR_PSHO_WAIT_FOR_NAS_CNF, gas_id);
            }
            else if (p_mac_grr_psho_cnf->psho_status == PSHO_FAILURE)
            {
              MSG_GERAN_HIGH_0_G("PSHO terminated - failure");

              /* MAC is responsible for sending PCCF */
              rr_psho_camp_on_old_cell(gas_id);
              rr_psho_state_transition(RR_PSHO_NULL, gas_id);
              event_out = EV_PSHO_COMPLETE;
            }
#ifdef FEATURE_WCDMA
            else if (p_mac_grr_psho_cnf->psho_status == PSHO_GTOW_INITIATE)
            {
              /* @@TODO: GTOW */
            }
#endif /* FEATURE_WCDMA */
            else
            {
              MSG_GERAN_ERROR_1_G("unknown psho_status from MAC, %d",
                p_mac_grr_psho_cnf->psho_status);
            }
          }
          else
          {
            MSG_GERAN_ERROR_3_G("unexpected message (%d,%d) in state %d",
              message_set, message_id, rr_psho_data_ptr->psho_state);
          }
        }
        else
        {
          MSG_GERAN_ERROR_2_G("unexpected event %d in state %d", psho_event, rr_psho_data_ptr->psho_state);
        }
      }
      break;

      case RR_PSHO_WAIT_FOR_NAS_CNF:
      {
        if (psho_event == EV_INPUT_MESSAGE && message_ptr != NULL)
        {
          if ((message_set == MS_MM_RR) && (message_id == (byte)RR_PSHO_CNF))
          {
            MSG_GERAN_HIGH_0_G("PSHO terminated - success");
            rr_send_grr_mac_psho_complete(PSHO_SUCCESS, gas_id);

            rr_psho_camp_on_new_cell(gas_id);
            rr_psho_state_transition(RR_PSHO_NULL, gas_id);
            event_out = EV_PSHO_COMPLETE;
          }
          else
          {
            MSG_GERAN_ERROR_3_G("unexpected message (%d,%d) in state %d",
              message_set, message_id, rr_psho_data_ptr->psho_state);
          }
        }
        else
        {
          MSG_GERAN_ERROR_2_G("unexpected event %d in state %d", psho_event, rr_psho_data_ptr->psho_state);
        }
      }
      break;

#ifdef FEATURE_WCDMA

#endif /* FEATURE_WCDMA */

      default:
        MSG_GERAN_ERROR_2_G("state exception, state %d, event %d", rr_psho_data_ptr->psho_state, psho_event);
      break;
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((psho_event != new_psho_event) && (new_psho_event != EV_NO_EVENT)) ||
        (rr_psho_data_ptr->old_psho_state != rr_psho_data_ptr->psho_state))
    {
      rr_store_trace_msg_buf(
        RR_PSHO_SM,
        new_psho_event,
        (byte) rr_psho_data_ptr->psho_state,
        message_ptr,
        gas_id
      );
    }
#endif

    if (new_psho_event != EV_NO_EVENT)
    {
      psho_event = new_psho_event;
      new_psho_event = EV_NO_EVENT;
    }
    else
    {
      psho_event = EV_NO_EVENT;
    }

    if (rr_psho_data_ptr->old_psho_state != rr_psho_data_ptr->psho_state)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_psho_control",
        rr_psho_data_ptr->old_psho_state,
        rr_psho_data_ptr->psho_state,
        rr_psho_state_name,
        gas_id
      );

      rr_psho_data_ptr->old_psho_state = rr_psho_data_ptr->psho_state;
    }
  } /* while (cell_select_event != EV_NO_EVENT) */


  return event_out;
}

/*!
 * \brief Initialisation function. Should be called once at RR task start.
 */
void rr_psho_task_start_init(void)
{
  // Zero the control/data structure(s)
  memset(rr_psho_data, 0, sizeof(rr_psho_data));
}


#else

/* If FEATURE_GPRS_PS_HANDOVER is undefined, a compiler warning will result
 * because this module will contain no code - define a dummy function here
 */
void rr_psho_dummy_func(void) { }

#endif /* #ifndef FEATURE_GPRS_PS_HANDOVER */

/* EOF */


