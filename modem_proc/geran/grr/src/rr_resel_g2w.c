/*==============================================================================
  FILE:         rr_resel_g2w.c

  OVERVIEW:     This module implements the RR G2W Cell Reselection state
                machine and it's associated functionality.

  DEPENDENCIES: None

                Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
==============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_resel_g2w.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_resel_g2w.h"
#include "rr_g2w.h"
#include "rr_rrc_send.h"
#include "rr_mode.h"
#include "rr_general.h"
#include "rr_mm_send.h"
#include "rr_g2x.h"
#include "rr_ps_access.h"
#ifdef FEATURE_GSM_TDS
#include "rr_tds_send.h"
#endif /*FEATURE_GSM_TDS*/
#include "rr_msgr.h"
#include "rr_multi_sim.h"

#include "rr_l1_send.h"
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef enum
{
  RR_RESEL_G2W_STATE_NULL,
  RR_RESEL_G2W_STATE_SUSPENDING,
  RR_RESEL_G2W_STATE_SUSPENDING_ABORTED,
  RR_RESEL_G2W_STATE_DEACTIVATING,
  RR_RESEL_G2W_STATE_DEACTIVATING_ABORTED,
  RR_RESEL_G2W_STATE_RESELECTING,
  RR_RESEL_G2W_STATE_RESELECTING_ABORTED,
  RR_RESEL_G2W_STATE_ACTIVATING,
  RR_RESEL_G2W_STATE_ACTIVATING_ABORTED,
  RR_RESEL_G2W_STATE_MAX
} rr_resel_g2w_state_e;

typedef struct
{
  rr_resel_g2w_state_e              state;
  rr_resel_g2w_state_e              old_state;
  rr_g2x_resel_type_e               type;
  rr_irat_cell_t                    target;
  sys_network_selection_mode_e_type network_selection_mode;
  PLMN_id_T                         serving_cell_plmn_id;
  uint16                            trk_lo_adj;
  rr_resel_g2w_status_e             status;
  sys_rat_pri_list_info_s_type      rat_pri_list_info;
} rr_resel_g2w_data_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_resel_g2w_data_t  rr_resel_g2w_data_spaces[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_resel_g2w_data_t*
 */
static rr_resel_g2w_data_t *rr_resel_g2w_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_resel_g2w_data_spaces[as_index];
}

static const char *rr_resel_g2w_state_name(
  rr_resel_g2w_state_e state
)
{
  switch (state)
  {
    case RR_RESEL_G2W_STATE_NULL:
      return "NULL";

    case RR_RESEL_G2W_STATE_SUSPENDING:
      return "SUSPENDING";

    case RR_RESEL_G2W_STATE_SUSPENDING_ABORTED:
      return "SUSPENDING_ABORTED";

    case RR_RESEL_G2W_STATE_DEACTIVATING:
      return "DEACTIVATING";

    case RR_RESEL_G2W_STATE_DEACTIVATING_ABORTED:
      return "DEACTIVATING_ABORTED";

    case RR_RESEL_G2W_STATE_RESELECTING:
      return "RESELECTING";

    case RR_RESEL_G2W_STATE_RESELECTING_ABORTED:
      return "RESELECTING_ABORTED";

    case RR_RESEL_G2W_STATE_ACTIVATING:
      return "ACTIVATING";

    case RR_RESEL_G2W_STATE_ACTIVATING_ABORTED:
      return "ACTIVATING_ABORTED";

    default:
      MSG_GERAN_ERROR_1("Unknown state: %d", state);
      return "?";
  }
}

/**
  @brief Called to suspend GPRS. This uses the RR-PS-ACCESS module to provide this functionality.
*/
static void rr_resel_g2w_suspend_mac(
  const gas_id_t gas_id
)
{
  // Send request to block PS access
  rr_ps_send_imsg_no_ps_access_req(
    FALSE,      // suspend_gmm
    NO_DEL,     // delete_pdu
    gas_id      // gas_id
  );

  // Once PS access is blocked, RR_PS_OMSG_NO_PS_ACCESS_CNF will be sent.
  // If messages to this module are coming via RR-RESEL, then the confirrmation will be EV_MAC_IDLE_MODE.
  // If not, RR_PS_OMSG_NO_PS_ACCESS_CNF will be received.
  // So both these inputs should be treated as equivalent.
}

/**
  @brief Wrapper function for L1 mode control state machine.

  Requests the L1 mode control state machine to deactivate L1.  If L1 is
  already suspended then this function will result in an
  RR_RESEL_G2W_IMSG_L1_DEACTIVATED_IND being sent immediately.
*/
static void rr_resel_g2w_deactivate_l1(const gas_id_t gas_id)
{

  rr_resel_g2w_data_t *rr_resel_g2w_data_ptr;

  // Obtain a pointer to the module data
  rr_resel_g2w_data_ptr = rr_resel_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_g2w_data_ptr);

#ifdef FEATURE_SGLTE
  if (rr_is_sglte_mode_active(gas_id))
  {
    rr_send_mph_ue_mode_change_req(TRUE,gas_id);
  }
  else
#endif /* FEATURE_SGLTE */
  {
    if (rr_g2x_lock_exchange_allowed(RR_G2X_OPERATION_TYPE_RESELECTION, rr_resel_g2w_data_ptr->target.rat, gas_id))
    {
      /* advise GL1 to exchange rather than release this client */
      /* which occurs when the GDrivers stop the physical layer */
      geran_set_exchange_on_release(GERAN_EXCHANGE_GTOX, gas_id);
    }
    else
    {
      geran_set_exchange_on_release(GERAN_EXCHANGE_NULL, gas_id);
    }

    if (EV_GSM_MODE_DEACTIVATED == rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id))
    {
      rr_resel_g2w_send_imsg_l1_deactivated_ind(gas_id);
    }
  }

  return;
}

/**
  @brief Wrapper function for L1 mode control state machine.

  Requests the L1 mode control state machine to activate L1.  If L1 is already
  suspended then this function will result in an
  RR_RESEL_G2W_IMSG_L1_ACTIVATED_IND  being sent immediately.
*/
static void rr_resel_g2w_activate_l1(
  const gas_id_t gas_id
)
{
#ifdef FEATURE_SGLTE
  if (rr_is_sglte_mode_active(gas_id))
  {
    rr_send_mph_ue_mode_change_req(rr_gsm_only(gas_id),gas_id);
  }
  else
#endif /* FEATURE_SGLTE */
  {
    rr_event_T event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
    if (event == EV_GSM_MODE_ACTIVATED)
    {
      rr_resel_g2w_send_imsg_l1_activated_ind(gas_id);
    }
  }
}

#ifdef FEATURE_WCDMA
#ifdef FEATURE_INTERRAT_PCCO_GTOW

static rr_resel_g2w_status_e rr_resel_g2w_convert_rrc_failure_reason(
  rrc_interrat_reject_reason_T rrc_cause
)
{
  rr_resel_g2w_status_e cause;

  switch (rrc_cause)
  {
    case RRC_WCDMA_CELL_CHANGE_FAILURE:
    case RRC_WCDMA_T3174_EXPIRED:
    case RRC_WCDMA_MAX_RACH_ATTEMPTS:
      cause = RR_RESEL_G2W_STATUS_PCCO_FAILURE_NO_RESPONSE;
      break;

    case RRC_WCDMA_CONNECTION_REJECT:
      cause = RR_RESEL_G2W_STATUS_PCCO_FAILURE_ASSIGNMENT_REJECT;
      break;

    default:
      cause = RR_RESEL_G2W_STATUS_PCCO_FAILURE_NO_RESPONSE;
      break;
  }

  return cause;
}

#endif /* FEATURE_INTERRAT_PCCO_GTOW */
#endif /* FEATURE_WCDMA */


#ifdef FEATURE_WCDMA
static void rr_resel_g2w_log_start(
  rr_resel_g2w_data_t * data,
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(data);

  switch (data->type)
  {
    case RR_G2X_RESEL_TYPE_RESELECTION:
    {
      rr_log_reselect_start(RR_RESELECTION_TRIGGER_GSM_TO_WCDMA, gas_id);
      break;
    } /* RR_G2X_RESEL_TYPE_RESELECTION */

    case RR_G2X_RESEL_TYPE_PACKET_CELL_CHANGE_ORDER:
    {
      rr_log_reselect_start(RR_RESELECTION_TRIGGER_PCCO_3G_TARGET, gas_id);
      break;
    } /* RR_G2X_RESEL_TYPE_PACKET_CELL_CHANGE_ORDER */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected reselection type: %d", data->type);
    }
  }
}
#endif /* FEATURE_WCDMA */

static void rr_resel_g2x_log_resel_aborted(
  rr_irat_cell_t * target,
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(target);

  switch (target->rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      /* No G2W abort event? */
      break;
    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      rr_log_g2l_resel_aborted(
        target->data.lte.earfcn,
        target->data.lte.pcid,
        gas_id
      );
      break;
    } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      /*No TDS log*/
      break;
    } /* RR_RAT_LTE */
#endif /*FEATURE_GSM_TDS*/

    default:
    {
      MSG_GERAN_ERROR_1_G("Unknown RAT: %d", target->rat);
    }
  }
}

static void rr_resel_g2x_log_resel_success(
  rr_irat_cell_t * target,
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(target)

  switch (target->rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      /* No G2W success event?  This is possibly logged elsewhere using
      standard reselection end event. */
      break;
    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      rr_log_g2l_resel_success(
        target->data.lte.earfcn,
        target->data.lte.pcid,
        gas_id
      );
      break;
    }
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      rr_log_g2t_reselect_success(
        target->data.utran_tdd.uarfcn, 
        target->data.utran_tdd.cpid,
        gas_id);
      break;
    } /* RR_RAT_UTRAN_TDD */
#endif /*FEATURE_GSM_TDS*/

    default:
    {
      MSG_GERAN_ERROR_1_G("Unknown RAT: %d", target->rat);
    }
  }
}


#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)

/**
  @ingroup rr_resel_g2w
*/
static void rr_resel_g2w_handle_rrc_interrat_reselection_rej(
  rr_irat_cell_t                 * cell,
  rrc_interrat_reselect_rej_type * msg,
  const gas_id_t                   gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(cell);
  RR_NULL_CHECK_RETURN_VOID(msg);

#ifdef FEATURE_WCDMA
  if (cell->rat == RR_RAT_WCDMA)
  {
    rr_log_g2w_reselect_failure(
      cell->data.wcdma.uarfcn,
      cell->data.wcdma.scrambling_code,
      (rr_log_reselection_failure_T)
        ((int) msg->reject_reason + (int) RR_LOG_G2WR_RESELECT_REJ_REASON_OFFSET + 1),
      gas_id
    );
  }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
  if (cell->rat == RR_RAT_UTRAN_TDD)
  {
    rr_log_g2t_reselect_failure(
      cell->data.utran_tdd.uarfcn,
      cell->data.utran_tdd.cpid,
      (rr_log_reselection_failure_T)
        ((int) msg->reject_reason + (int) RR_LOG_G2WR_RESELECT_REJ_REASON_OFFSET + 1),
      gas_id
    );
  }
#endif /*FEATURE_GSM_TDS*/

  switch (msg->reject_reason)
  {
    case RRC_WCDMA_CELL_QUALITY_FAILURE:
    {
      rr_g2x_resel_failure_data_t data;

      data.cell_quality_data.wcdma.srxlevmin = msg->srxlevmin;
      data.cell_quality_data.wcdma.squalmin = msg->squalmin;

      rr_g2x_resel_failed(
        cell,
        RR_G2X_RESEL_FAILURE_CAUSE_CELL_QUALITY_FAILURE,
        &data,
        gas_id
      );
      break;
    } /* RRC_WCDMA_CELL_QUALITY_FAILURE */

    case RRC_WCDMA_FORBIDDEN_LA:
    {
      rr_g2x_resel_failure_data_t data;
      if (msg->time_to_reselect > 0)
      {        
        data.time_to_reselect = msg->time_to_reselect;
      }

      data.lai.plmn_id.identity[0] = msg->PLMN.identity[0];
      data.lai.plmn_id.identity[1] = msg->PLMN.identity[1];
      data.lai.plmn_id.identity[2] = msg->PLMN.identity[2];

      data.lai.location_area_code = (msg->forbidden_lac[0] << 8) +
                                msg->forbidden_lac[1];

      rr_g2x_resel_failed(
        cell,
        RR_G2X_RESEL_FAILURE_CAUSE_FORBIDDEN_LA,
        &data,
        gas_id
      );
      break;
    } /* RRC_WCDMA_FORBIDDEN_LA */

    case RRC_WCDMA_PLMN_MISMATCH:
    {
      rr_g2x_resel_failure_data_t data;
      if (msg->time_to_reselect > 0)
      {        
        data.time_to_reselect = msg->time_to_reselect;
      }

      data.plmn_id = msg->PLMN;

      rr_g2x_resel_failed(
        cell,
        RR_G2X_RESEL_FAILURE_CAUSE_PLMN_MISMATCH,
        &data,
        gas_id
      );
      break;
    } /* RRC_WCDMA_PLMN_MISMATCH */

    case RRC_WCDMA_CELL_BARRED:
    {
      rr_g2x_resel_failure_data_t data;
      if (msg->time_to_reselect > 0)
      {        
        data.time_to_reselect = msg->time_to_reselect;
      }
      rr_g2x_resel_failed(
        cell,
        RR_G2X_RESEL_FAILURE_CAUSE_CELL_BARRED,
        &data,
        gas_id
      );
      break;
    } /* RRC_WCDMA_CELL_BARRED */

    case RRC_WCDMA_UARFCN_BARRED:
    {
      rr_g2x_resel_failure_data_t data;
      if (msg->time_to_reselect > 0)
      {        
        data.time_to_reselect = msg->time_to_reselect;
      }
      rr_g2x_resel_failed(
        cell,
        RR_G2X_RESEL_FAILURE_CAUSE_FREQUENCY_BARRED,
        &data,
        gas_id
      );
      break;
    } /* RRC_WCDMA_UARFCN_BARRED */

    case RRC_WCDMA_CSG_FORBIDDEN:
    {
      rr_g2x_resel_failed(
        cell,
        RR_G2X_RESEL_FAILURE_CAUSE_CSG_FORBIDDEN,
        NULL,
        gas_id
      );
      break;
    } /* RRC_WCDMA_CSG_FORBIDDEN */

    default:
    {
      if (msg->time_to_reselect > 0)
      {
        rr_g2x_resel_failure_data_t data;

        data.time_to_reselect = msg->time_to_reselect;

        rr_g2x_resel_failed(
          cell,
          RR_G2X_RESEL_FAILURE_CAUSE_UNDEFINED,
          &data,
          gas_id
        );
      }
      else
      {
        rr_g2x_resel_failed(
          cell,
          RR_G2X_RESEL_FAILURE_CAUSE_UNDEFINED,
          NULL,
          gas_id
        );
      }
    }
  }
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

#ifdef FEATURE_LTE

static rr_log_g2l_resel_status_e rr_resel_g2w_map_g2l_resel_fail_cause_to_rr_log_g2l_resel_status(
  lte_rrc_irat_to_lte_failure_cause_type_e cause
)
{
  switch (cause)
  {
    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_UNKNOWN:
      return RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_UNKNOWN;
    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_CELL_BARRED:
      return RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_CELL_BARRED;
    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_PLMN_MISMATCH:
      return RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_PLMN_MISMATCH;
    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_FORBIDDEN_TA:
      return RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_FORBIDDEN_TA;
    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_ACQ_FAILED:
      return RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_ACQ_FAILED;
    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_DL_WEAK:
      return RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_DL_WEAK;
    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_CFG_FAILURE:
      return RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_CFG_FAILURE;
    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_CELL_SELECT_FAILURE:
      return RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_CELL_SELECT_FAILURE;
    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_FREQ_BARRED:
      return RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_FREQ_BARRED;
    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_FORBIDDEN_CSG_ID:
      return RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_FORBIDDEN_CSG_ID;
    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_SIB_TIMEOUT_ERROR:
      return RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_SIB_TIMEOUT_ERROR;
    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_SIB_INVALID_ERROR:
      return RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_SIB_INVALID_ERROR;
    default:
      MSG_GERAN_ERROR_1("Unknown cause: %d", cause);
      return RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_UNKNOWN;
  }
}

/**
  @ingroup rr_resel_g2l
*/
static void rr_resel_g2w_handle_lte_rrc_G_resel_failed_rsp(
  rr_irat_cell_t               * cell,
  lte_rrc_G_resel_failed_rsp_s * msg,
  const gas_id_t                 gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(cell);
  RR_NULL_CHECK_RETURN_VOID(msg);

  rr_log_g2l_resel_failure(
    cell->data.lte.earfcn,
    cell->data.lte.pcid,
    rr_resel_g2w_map_g2l_resel_fail_cause_to_rr_log_g2l_resel_status(msg->fail_cause),
    gas_id
  );

  switch (msg->fail_cause)
  {
    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_CELL_BARRED:
    {
      rr_g2x_resel_failure_data_t data;

      data.time_to_reselect = msg->cell_bar_time;

      rr_g2x_resel_failed(
        cell,
        RR_G2X_RESEL_FAILURE_CAUSE_CELL_BARRED,
        &data,
        gas_id
      );
      break;
    } /* LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_CELL_BARRED */

    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_PLMN_MISMATCH:
    {
      rr_g2x_resel_failure_data_t data;

      /* PLMN is not available in the existing GERAN<->LTE interface, so
      set the PLMN to an undefined value.  This means that if the PLMN did
      happen to become equivalent to the serving PLMN then we still would
      not reselect it (a limitation that does not exist in the G2W case) but
      the spec does not require this. */

      sys_plmn_undefine_plmn_id(&data.plmn_id);

      rr_g2x_resel_failed(
        cell,
        RR_G2X_RESEL_FAILURE_CAUSE_PLMN_MISMATCH,
        &data,
        gas_id
      );
      break;
    } /* LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_PLMN_MISMATCH */

    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_FORBIDDEN_TA:
    {
      rr_g2x_resel_failed(
        cell,
        RR_G2X_RESEL_FAILURE_CAUSE_FORBIDDEN_TA,
        NULL,
        gas_id
      );
      break;
    } /* LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_FORBIDDEN_TA */

    case LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_FREQ_BARRED:
    {
      rr_g2x_resel_failure_data_t data;

      data.time_to_reselect = msg->cell_bar_time;

      rr_g2x_resel_failed(
        cell,
        RR_G2X_RESEL_FAILURE_CAUSE_FREQUENCY_BARRED,
        &data,
        gas_id
      );
      break;
    } /* LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_FREQ_BARRED */

    default:
    {
      if (msg->cell_bar_time > 0)
      {
        rr_g2x_resel_failure_data_t data;

        data.time_to_reselect = msg->cell_bar_time;

        rr_g2x_resel_failed(
          cell,
          RR_G2X_RESEL_FAILURE_CAUSE_UNDEFINED,
          &data,
          gas_id
        );
      }
      else
      {
        rr_g2x_resel_failed(
          cell,
          RR_G2X_RESEL_FAILURE_CAUSE_UNDEFINED,
          NULL,
          gas_id
        );
      }
    }
  }
}

#endif /* FEATURE_LTE */

static void rr_resel_g2w_cache_rat_pri_list_info(
  const gas_id_t gas_id
)
{
  rr_resel_g2w_data_t *rr_resel_g2w_data_ptr;
  rr_PLMN_request_details_T *plmn_request_details;

  // Obtain a pointer to the module data
  rr_resel_g2w_data_ptr = rr_resel_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_g2w_data_ptr);

  plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  RR_NULL_CHECK_FATAL(plmn_request_details);

  rr_resel_g2w_data_ptr->rat_pri_list_info = plmn_request_details->rat_pri_list_info;
}

static void rr_resel_g2w_restore_rat_pri_list_info(
  const gas_id_t gas_id
)
{
  rr_resel_g2w_data_t *rr_resel_g2w_data_ptr;
  rr_PLMN_request_details_T *plmn_request_details;

  // Obtain a pointer to the module data
  rr_resel_g2w_data_ptr = rr_resel_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_g2w_data_ptr);

  plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  RR_NULL_CHECK_FATAL(plmn_request_details);

  plmn_request_details->rat_pri_list_info = rr_resel_g2w_data_ptr->rat_pri_list_info;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * --------------------------------------------------------------------------*/

void rr_resel_g2w_init(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_resel_g2w_data_t *rr_resel_g2w_data_ptr = rr_resel_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_g2w_data_ptr);

  memset(rr_resel_g2w_data_ptr, 0, sizeof(rr_resel_g2w_data_t));
}

/**
  @brief This is the main RR G2W Cell Reselection State Machine.
*/
void rr_resel_g2w_control(
  rr_cmd_bdy_type *msg,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_resel_g2w_data_t *rr_resel_g2w_data_ptr = rr_resel_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_g2w_data_ptr);

  if (msg == NULL)
  {
    MSG_GERAN_ERROR_0_G("msg == NULL");
    return;
  }

#ifdef DEBUG_RR_TRACE_MSG
  rr_store_trace_msg_buf(
    RR_RESEL_G2W_SM,
    EV_INPUT_MESSAGE,
    (byte) rr_resel_g2w_data_ptr->state,
    msg,
    gas_id
  );
#endif

  switch (rr_resel_g2w_data_ptr->state)
  {
    case RR_RESEL_G2W_STATE_NULL:
    {
      switch (msg->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg->rr.header.rr_message_set)
          {
            case RR_RESEL_G2W_IMSG:
            {
              rr_resel_g2w_imsg_u *imsg;
              imsg = &msg->rr.resel_g2w.imsg;

              switch (imsg->header.imh.message_id)
              {
                case RR_RESEL_G2W_IMSG_RESEL_REQ:
                {
                  rr_resel_g2w_imsg_resel_req_t * resel_req;
                  resel_req = &imsg->resel_req;

                  rr_resel_g2w_data_ptr->target = resel_req->target;
                  rr_resel_g2w_data_ptr->type = resel_req->type;
                  rr_resel_g2w_data_ptr->network_selection_mode = resel_req->network_selection_mode;
                  rr_resel_g2w_data_ptr->serving_cell_plmn_id = resel_req->serving_cell_plmn_id;
                  rr_resel_g2w_data_ptr->status = RR_RESEL_G2W_STATUS_UNDEFINED;

                  switch (resel_req->target.rat)
                  {
#ifdef FEATURE_WCDMA
                    case RR_RAT_WCDMA:
                    {
                      if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
                      {
                        MSG_GERAN_ERROR_1_G("Unexpected G2W IRAT procedure:%d started when segment not loaded, attempt recovery",rr_resel_g2w_data_ptr->type);

                        if (rr_resel_g2w_data_ptr->type == RR_G2X_RESEL_TYPE_RESELECTION)
                        {
                          rr_resel_g2w_send_omsg_resel_cnf(RR_RESEL_G2W_STATUS_RESELECTION_FAILURE, gas_id);
                        }
                        else if (rr_resel_g2w_data_ptr->type == RR_G2X_RESEL_TYPE_PACKET_CELL_CHANGE_ORDER)
                        {
                          rr_resel_g2w_send_omsg_resel_cnf(RR_RESEL_G2W_STATUS_PCCO_FAILURE_NO_RESPONSE, gas_id);
                        }
                        
                        return;
                      }
                      rr_resel_g2w_log_start(rr_resel_g2w_data_ptr, gas_id);
                      break;
                    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
                    case RR_RAT_LTE:
                    {
                      rr_log_g2l_resel_started(
                        resel_req->target.data.lte.earfcn,
                        resel_req->target.data.lte.pcid,
                        gas_id
                      );
                      break;
                    } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
                    case RR_RAT_UTRAN_TDD:
                    {
                      if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
                      {
                        MSG_GERAN_ERROR_1_G("Unexpected G2T IRAT proecdure:%d started when segment not loaded, attempt recovery",rr_resel_g2w_data_ptr->type);

                        if (rr_resel_g2w_data_ptr->type == RR_G2X_RESEL_TYPE_RESELECTION)
                        {
                          rr_resel_g2w_send_omsg_resel_cnf(RR_RESEL_G2W_STATUS_RESELECTION_FAILURE, gas_id);
                        }
                        else if (rr_resel_g2w_data_ptr->type == RR_G2X_RESEL_TYPE_PACKET_CELL_CHANGE_ORDER)
                        {
                          rr_resel_g2w_send_omsg_resel_cnf(RR_RESEL_G2W_STATUS_PCCO_FAILURE_NO_RESPONSE, gas_id);
                        }

                        return;
                      }
                    
                      rr_log_g2t_reselect_started(
                        resel_req->target.data.utran_tdd.uarfcn, 
                        resel_req->target.data.utran_tdd.cpid,
                        gas_id);
                      break;
                    } /* RR_RAT_WCDMA */
#endif /*FEATURE_GSM_TDS*/
                    default:
                    {
                      MSG_GERAN_ERROR_1_G("Unknown RAT: %d", resel_req->target.rat);
                    }
                  }

                  rr_resel_g2w_suspend_mac(gas_id);
                  rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_SUSPENDING;
                  break;
                } /* RR_RESEL_G2W_IMSG_RESEL_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg,
                    rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_RESEL_G2W_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg,
            rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
          );
        }
      }
      break;
    } /* RR_RESEL_G2W_STATE_NULL */

    case RR_RESEL_G2W_STATE_SUSPENDING:
    {
      switch (msg->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg->rr.header.rr_message_set)
          {
            case RR_RESEL_G2W_IMSG:
            {
              rr_resel_g2w_imsg_u *imsg;
              imsg = &msg->rr.resel_g2w.imsg;

              switch (imsg->header.imh.message_id)
              {
                case RR_RESEL_G2W_IMSG_ABORT_IND:
                {
                  rr_resel_g2x_log_resel_aborted(&rr_resel_g2w_data_ptr->target, gas_id);
                  rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_SUSPENDING_ABORTED;
                  break;
                } /* RR_RESEL_G2W_IMSG_ABORT_IND */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg,
                    rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_RESEL_G2W_IMSG */

            case RR_PS_IMSG:
            {
              rr_ps_access_control(msg, gas_id);
              break;
            }

            case RR_PS_OMSG:
            {
              switch (msg->rr.ps.omsg.header.imh.message_id)
              {
                case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                {
                  rr_resel_g2w_deactivate_l1(gas_id);
                  rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_DEACTIVATING;
                  break;
                }

                case RR_PS_OMSG_PS_ACCESS_CNF:
                {
                  /* Ignore this message. This could come due to race condition. */
                  break;
                }
                
                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg,
                    rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
                  );
                }
              }

              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_MM_RR:
        {
          switch (msg->message_header.message_id)
          {
            case RR_STOP_GSM_MODE_REQ:
            {
              rr_resel_g2w_send_imsg_abort_ind(RR_RESEL_G2W_ABORT_CAUSE_MM_REQ, gas_id);
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_MM_RR */

        case MS_RR_L1:
        {
          break;
        }

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg,
            rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
          );
        }
      }
      break;
    } /* RR_RESEL_G2W_STATE_SUSPENDING */

    case RR_RESEL_G2W_STATE_SUSPENDING_ABORTED:
    {
      switch (msg->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg->rr.header.rr_message_set)
          {
            case RR_RESEL_G2W_IMSG:
            {
              rr_resel_g2w_imsg_u *imsg;
              imsg = &msg->rr.resel_g2w.imsg;

              switch (imsg->header.imh.message_id)
              {
                case RR_RESEL_G2W_IMSG_ABORT_IND:
                {
                  /* Already aborting, no action necessary. */
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg,
                    rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_RESEL_G2W_IMSG */

            case RR_PS_IMSG:
            {
              rr_ps_access_control(msg, gas_id);
              break;
             }

            case RR_PS_OMSG:
            {
              switch (msg->rr.ps.omsg.header.imh.message_id)
              {
                case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                {
                  rr_resel_g2x_log_resel_aborted(&rr_resel_g2w_data_ptr->target, gas_id);
                  rr_resel_g2w_send_omsg_resel_cnf(RR_RESEL_G2W_STATUS_ABORTED_IN_SERVICE, gas_id);
                  rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_NULL;
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg,
                    rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
                  );
                }
              }

              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_MM_RR:
        {
          switch (msg->message_header.message_id)
          {
            case RR_STOP_GSM_MODE_REQ:
            {
              /* No action necessary - already aborting. */
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_MM_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg,
            rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
          );
        }
      }
      break;
    } /* RR_RESEL_G2W_STATE_SUSPENDING_ABORTED */

    case RR_RESEL_G2W_STATE_DEACTIVATING:
    {
      switch (msg->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg->rr.header.rr_message_set)
          {
            case RR_RESEL_G2W_IMSG:
            {
              rr_resel_g2w_imsg_u *imsg;
              imsg = &msg->rr.resel_g2w.imsg;

              switch (imsg->header.imh.message_id)
              {
                case RR_RESEL_G2W_IMSG_L1_DEACTIVATED_IND:
                {
                  /* Take a copy of the RAT Priority List, as if WCDMA calls
                  rr_rrc_set_band_pref() during the G2W procedure then this
                  will result in the current RAT Priority List being overwritten.
                  If the G2W procedure then goes on to fail, when service
                  is resumed on GSM the RAT Priority List will not indicate
                  support for WCDMA / LTE. */

                  rr_resel_g2w_cache_rat_pri_list_info(gas_id);

                  switch (rr_resel_g2w_data_ptr->target.rat)
                  {
#ifdef FEATURE_WCDMA
                    case RR_RAT_WCDMA:
                    {
                      switch (rr_resel_g2w_data_ptr->type)
                      {
                        case RR_G2X_RESEL_TYPE_RESELECTION:
                        {
                          rr_send_interrat_reselection_req(
                            rr_resel_g2w_data_ptr->target.data.wcdma.uarfcn,
                            rr_resel_g2w_data_ptr->target.data.wcdma.scrambling_code,
                            rr_resel_g2w_data_ptr->target.data.wcdma.pn_offset,
                            rr_resel_g2w_data_ptr->network_selection_mode,
                            rr_resel_g2w_data_ptr->serving_cell_plmn_id,
                            rr_resel_g2w_data_ptr->trk_lo_adj,
                            rr_rat_pri_list_info(gas_id),
                            gas_id
                          );
                          break;
                        }

                        case RR_G2X_RESEL_TYPE_PACKET_CELL_CHANGE_ORDER:
                        {
                          rr_send_interrat_pcco_req(
                            rr_resel_g2w_data_ptr->target.data.wcdma.uarfcn,
                            rr_resel_g2w_data_ptr->target.data.wcdma.scrambling_code,
                            rr_resel_g2w_data_ptr->network_selection_mode,
                            rr_resel_g2w_data_ptr->serving_cell_plmn_id,
                            T3174_TIMEOUT,
                            rr_rat_pri_list_info(gas_id),
                            gas_id
                          );
                          break;
                        }

                        default:
                        {
                          MSG_GERAN_ERROR_1_G("Unexpected resel type: %d", rr_resel_g2w_data_ptr->type);
                        }
                      }
                      break;
                    } /* RR_G2X_RESEL_TYPE_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE

                    case RR_RAT_LTE:
                    {
                      switch (rr_resel_g2w_data_ptr->type)
                      {
                        case RR_G2X_RESEL_TYPE_RESELECTION:
                        {
                          lte_rrc_G_resel_req_s resel_req;
                          errno_enum_type       result;

                          memset(&resel_req, 0, sizeof(lte_rrc_G_resel_req_s));

                          msgr_init_hdr(
                            &resel_req.msg_hdr,
                            MSGR_GERAN_GRR,
                            LTE_RRC_G_RESEL_REQ
                          );

                          resel_req.earfcn = rr_resel_g2w_data_ptr->target.data.lte.earfcn;
                          resel_req.lte_cell_id = rr_resel_g2w_data_ptr->target.data.lte.pcid;
                          resel_req.nas_params.rat_pri_list = *rr_rat_pri_list_info(gas_id);
                          resel_req.nas_params.req_plmn = rr_resel_g2w_data_ptr->serving_cell_plmn_id;
                          resel_req.nas_params.network_select_mode = rr_resel_g2w_data_ptr->network_selection_mode;

                          rr_dedicated_priority_info_get(&resel_req.dedicated_priority_info, gas_id);

                          result = rr_msgr_send(&resel_req.msg_hdr, sizeof(resel_req), gas_id);
                          if (result != E_SUCCESS)
                          {
                            MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                          }
                          break;
                        } /* RR_G2X_RESEL_TYPE_RESELECTION */

                        default:
                        {
                          MSG_GERAN_ERROR_1_G("Unexpected resel type: %d", rr_resel_g2w_data_ptr->type);
                        }
                      }
                      break;
                    } /* RR_G2X_RESEL_TYPE_LTE */

#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
                    case RR_RAT_UTRAN_TDD:
                    {
                      switch (rr_resel_g2w_data_ptr->type)
                      {
                        case RR_G2X_RESEL_TYPE_RESELECTION:
                        {
                          rr_send_tds_interrat_reselection_req(
                            rr_resel_g2w_data_ptr->target.data.utran_tdd.cpid,
                            rr_resel_g2w_data_ptr->network_selection_mode,
                            rr_rat_pri_list_info(gas_id),
                            rr_resel_g2w_data_ptr->serving_cell_plmn_id,
                            rr_resel_g2w_data_ptr->trk_lo_adj,
                            rr_resel_g2w_data_ptr->target.data.utran_tdd.uarfcn,
                            gas_id
                          );
                          break;
                        }

                        default:
                        {
                          MSG_GERAN_ERROR_1_G("Unexpected resel type: %d", rr_resel_g2w_data_ptr->type);
                        }
                      }
                      break;
                    }
#endif /*FEATURE_GSM_TDS*/

                    default:
                    {
                      MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rr_resel_g2w_data_ptr->target.rat);
                    }
                  }
                  rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_RESELECTING;
                  break;
                } /* RR_RESEL_G2W_L1_DEACTIVATED_IND */

                case RR_RESEL_G2W_IMSG_ABORT_IND:
                {
                  MSG_GERAN_HIGH_1_G("Received RR_RESEL_G2W_IMSG_ABORT_IND(abort_cause => %d)",
                           msg->rr.resel_g2w.imsg.abort_ind.abort_cause);

                  switch (msg->rr.resel_g2w.imsg.abort_ind.abort_cause)
                  {
                    case RR_RESEL_G2W_ABORT_CAUSE_RR_EST_REQ:
                    {
                      MSG_GERAN_HIGH_0_G("Ignoring abort for RR_EST_REQ, no longer camped");
                      break;
                    }

                    default:
                    {
                      MSG_GERAN_HIGH_0_G("Aborting reselection");
                      rr_resel_g2x_log_resel_aborted(&rr_resel_g2w_data_ptr->target, gas_id);
                      rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_DEACTIVATING_ABORTED;
                    }
                  }
                  break;
                } /* RR_RESEL_G2W_ABORT_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg,
                    rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_RESEL_G2W_IMSG */


            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_MM_RR:
        {
          switch (msg->message_header.message_id)
          {
            case RR_STOP_GSM_MODE_REQ:
            {
              rr_resel_g2w_send_imsg_abort_ind(RR_RESEL_G2W_ABORT_CAUSE_MM_REQ, gas_id);
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_MM_RR */

#ifdef FEATURE_SGLTE
        case MS_RR_L1:
        {
          switch (msg->message_header.message_id)
          {
            case MPH_UE_MODE_CHANGE_CNF:
            {
              if (rr_is_sglte_mode_active(gas_id))
              {
                MSG_GERAN_HIGH_0_G("MPH_UE_MODE_CHANGE_CNF rxd");
                rr_resel_g2w_send_imsg_l1_deactivated_ind(gas_id);
              }
              break;
            } /* MPH_UE_MODE_CHANGE_CNF */

            case MPH_SERVING_IDLE_MEAS_IND:
            case MPH_SURROUND_MEAS_IND:
            case MPH_SURROUND_IRAT_MEAS_IND:
              /*ignore in this state*/
            break;

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_L1 */
#endif /* FEATURE_SGLTE */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg,
            rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
          );
        }
      }
      break;
    } /* RR_RESEL_G2W_STATE_DEACTIVATING */

    case RR_RESEL_G2W_STATE_DEACTIVATING_ABORTED:
    {
      switch (msg->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg->rr.header.rr_message_set)
          {
            case RR_RESEL_G2W_IMSG:
            {
              rr_resel_g2w_imsg_u *imsg;
              imsg = &msg->rr.resel_g2w.imsg;

              switch (imsg->header.imh.message_id)
              {
                case RR_RESEL_G2W_IMSG_L1_DEACTIVATED_IND:
                {
                  rr_resel_g2w_activate_l1(gas_id);
                  rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_ACTIVATING_ABORTED;
                  break;
                } /* RR_RESEL_G2W_L1_DEACTIVATED_IND */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg,
                    rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_RESEL_G2W_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_MM_RR:
        {
          switch (msg->message_header.message_id)
          {
            case RR_STOP_GSM_MODE_REQ:
            {
              /* No action necessary - already aborting. */
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_MM_RR */

#ifdef FEATURE_SGLTE
        case MS_RR_L1:
        {
          switch (msg->message_header.message_id)
          {
            case MPH_UE_MODE_CHANGE_CNF:
            {
              if (rr_is_sglte_mode_active(gas_id))
              {
                MSG_GERAN_HIGH_0_G("MPH_UE_MODE_CHANGE_CNF rxd");
                rr_resel_g2w_send_imsg_l1_deactivated_ind(gas_id);
              }
              break;
            } /* MPH_UE_MODE_CHANGE_CNF */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_L1 */
#endif /* FEATURE_SGLTE */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg,
            rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
          );
        }
      }
      break;
    } /* RR_RESEL_G2W_STATE_DEACTIVATING_ABORTED */

    case RR_RESEL_G2W_STATE_RESELECTING:
    {
      switch (msg->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg->rr.header.rr_message_set)
          {
            case RR_RESEL_G2W_IMSG:
            {
              rr_resel_g2w_imsg_u *imsg;
              imsg = &msg->rr.resel_g2w.imsg;

              switch (imsg->header.imh.message_id)
              {
                case RR_RESEL_G2W_IMSG_ABORT_IND:
                {
                  MSG_GERAN_HIGH_1_G("Received RR_RESEL_G2W_IMSG_ABORT_IND(abort_cause => %d)",
                           msg->rr.resel_g2w.imsg.abort_ind.abort_cause);

                  switch (msg->rr.resel_g2w.imsg.abort_ind.abort_cause)
                  {
                    case RR_RESEL_G2W_ABORT_CAUSE_RR_EST_REQ:
                    {
                      MSG_GERAN_HIGH_0_G("Ignoring abort for RR_EST_REQ, no longer camped");
                      break;
                    } /* RR_RESEL_G2W_ABORT_CAUSE_RR_EST_REQ */

                    default:
                    {
                      MSG_GERAN_HIGH_0_G("Aborting reselection");

                      switch (rr_resel_g2w_data_ptr->target.rat)
                      {
#ifdef FEATURE_WCDMA
                        case RR_RAT_WCDMA:
                        {
                          if(rr_resel_g2w_data_ptr->type == RR_G2X_RESEL_TYPE_PACKET_CELL_CHANGE_ORDER )
                          { 				  
                            rr_send_rrc_interrat_cc_order_abort_req(gas_id);
                          }
                          else
                          {
                            (void)rr_timer_start(RR_IRAT_PROC_TIMER,RR_IRAT_PROC_TIMEOUT, gas_id);
                            rr_send_interrat_reselection_abort_req(gas_id);
                          }
                          break;
                        } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                        case RR_RAT_UTRAN_TDD:
                        {
                          (void)rr_timer_start(RR_IRAT_PROC_TIMER,RR_IRAT_PROC_TIMEOUT, gas_id);
                          rr_send_tds_interrat_reselection_abort_req(gas_id);
                          break;
                        } /* RR_RAT_WCDMA */
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
                        case RR_RAT_LTE:
                        {
                          lte_rrc_G_abort_resel_req_s abort_resel_req;
                          errno_enum_type             result;

                          memset(&abort_resel_req, 0, sizeof(lte_rrc_G_abort_resel_req_s));

                          msgr_init_hdr(
                            &abort_resel_req.msg_hdr,
                            MSGR_GERAN_GRR,
                            LTE_RRC_G_ABORT_RESEL_REQ
                          );
                          result = rr_msgr_send(&abort_resel_req.msg_hdr, sizeof(abort_resel_req), gas_id);
                          if (result != E_SUCCESS)
                          {
                            MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                          }
                          else
                          {
                            (void)rr_timer_start(RR_IRAT_PROC_TIMER,RR_IRAT_PROC_TIMEOUT, gas_id);
                          }
                          break;
                        } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

                        default:
                        {
                          MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rr_resel_g2w_data_ptr->target.rat);
                        }
                      }

                      rr_resel_g2x_log_resel_aborted(&rr_resel_g2w_data_ptr->target, gas_id);
                      rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_RESELECTING_ABORTED;
                    }
                  }
                  break;
                } /* RR_RESEL_G2W_IMSG_ABORT_IND */

#ifdef FEATURE_SGLTE
                case RR_RESEL_G2W_IMSG_STOP_GSM_MODE_REQ_SGLTE_RAT_CHANGE:
                {
                  rr_resel_g2x_log_resel_success(&rr_resel_g2w_data_ptr->target, gas_id);
                  rr_resel_g2w_send_omsg_resel_cnf(RR_RESEL_G2W_STATUS_RESELECTION_SUCCESS, gas_id);
                  rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_NULL;
                  break;
                } /* RR_RESEL_G2W_IMSG_STOP_GSM_MODE_REQ_SGLTE_RAT_CHANGE */
#endif /* FEATURE_SGLTE */


                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg,
                    rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_RESEL_G2W_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

#ifdef FEATURE_GSM_TDS
        case MS_TDSRRC_RR:
        {
          switch (msg->message_header.message_id)
          {
            case TDSRRC_INTERRAT_RESELECTION_REJ:
            {
              rrc_interrat_reselect_rej_type rrc_resel_rej;

              rr_convert_tds_to_rrc_resel_rej(&msg->tdsrrc_resel_rej, &rrc_resel_rej);

              rr_resel_g2w_handle_rrc_interrat_reselection_rej(
                &rr_resel_g2w_data_ptr->target,
                &rrc_resel_rej,
                gas_id
              );

              rr_resel_g2w_data_ptr->status = RR_RESEL_G2W_STATUS_RESELECTION_FAILURE;

              rr_resel_g2w_activate_l1(gas_id);

              /*reset Scell info in CM as it has been reset 
                by higher layers during the redirection*/
              geran_multisim_eng_mode_refresh_cm(ENG_MODE_SCELL_INFO, gas_id);

              rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_ACTIVATING;
              break;
            }
          }
          break;
        } /*MS_TDSRRC_RR*/
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
        case MS_RRC_RR:
        {
          switch (msg->message_header.message_id)
          {
            case RRC_INTERRAT_RESELECTION_REJ:
            {
#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
              rr_resel_g2w_handle_rrc_interrat_reselection_rej(
                &rr_resel_g2w_data_ptr->target,
                &msg->rrc_interrat_reselect_rej,
                gas_id
              );
#endif /* FEATURE_LTE || FEATURE_GSM_TDS */

              rr_resel_g2w_data_ptr->status = RR_RESEL_G2W_STATUS_RESELECTION_FAILURE;

              /* Restore the previously-cached RAT Priority List as it may have
              been overwritten following a call to rr_rrc_set_band_pref(). */
              rr_resel_g2w_restore_rat_pri_list_info(gas_id);

              rr_resel_g2w_activate_l1(gas_id);

              /*reset Scell info in CM as it has been reset 
                by higher layers during the redirection*/
              geran_multisim_eng_mode_refresh_cm(ENG_MODE_SCELL_INFO, gas_id);

              rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_ACTIVATING;
              break;
            } /* RRC_INTERRAT_RESELECTION_REJ */

            case RRC_INTERRAT_CC_ORDER_CNF:
            {
              rrc_interrat_cc_order_cnf_type *interrat_cc_order_cnf = &msg->rrc_interrat_cc_order_cnf;

              switch (interrat_cc_order_cnf->status)
              {
                case CCO_SUCCESS:
                {
                  rr_send_gtow_cco_complete_ind(gas_id);
                  rr_resel_g2w_send_omsg_resel_cnf(RR_RESEL_G2W_STATUS_PCCO_SUCCESS, gas_id);
                  rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_NULL;
                  break;
                } /* CCO_SUCCESS */

                /*
                CCO procedure failed on WCDMA side before RRC sent an RRC_ACTIVATION_IND
                to NAS, so no need to send an RR_ACTIVATION_IND before service can be
                reacquired on GSM.
                */
                case CCO_FAILURE_BEFORE_ACT_IND:
                {
                  rr_resel_g2w_data_ptr->status = rr_resel_g2w_convert_rrc_failure_reason(
                    interrat_cc_order_cnf->failure_reason
                  );

                  /* Restore the previously-cached RAT Priority List as it may have
                  been overwritten following a call to rr_rrc_set_band_pref(). */
                  rr_resel_g2w_restore_rat_pri_list_info(gas_id);

                  rr_resel_g2w_activate_l1(gas_id);
                  (void)rr_timer_start(T3176, T3176_TIMEOUT, gas_id);

                  /*reset Scell info in CM as it has been reset 
                    by higher layers during the redirection*/
                  geran_multisim_eng_mode_refresh_cm(ENG_MODE_SCELL_INFO, gas_id);
 
                  rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_ACTIVATING;
                  break;
                } /* CCO_FAILURE_BEFORE_ACT_IND */

                /*
                CCO procedure failed on WCDMA side after RRC send an RRC_ACTIVATION_IND
                to NAS, to RR needs to first send RR_ACTIVATION_IND and received
                RR_ACTIVATION_RSP before service can be reacquired on GSM.
                */
                case CCO_FAILURE_AFTER_ACT_IND:
                {
                  rr_resel_g2w_data_ptr->status = rr_resel_g2w_convert_rrc_failure_reason(
                    interrat_cc_order_cnf->failure_reason
                  );

                  /*reset Scell info in CM as it has been reset 
                    by higher layers during the redirection*/
                  geran_multisim_eng_mode_refresh_cm(ENG_MODE_SCELL_INFO, gas_id);

                  rr_send_activation_ind(MM_AS_GTOW_CC_ORDER_FAILED, gas_id);
                  break;
                } /* CCO_FAILURE_AFTER_ACT_IND */

                default:
                {
                  MSG_GERAN_ERROR_1_G("Unexpected CCO status: %d", interrat_cc_order_cnf->status);
                }
              }
              break;
            } /* RRC_INTERRAT_CC_ORDER_CNF */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RRC_RR */
#endif /* FEATURE_WCDMA */

        case MS_MSGR_RR:
        {
          switch (msg->rr_msgr_msg.message.hdr.id)
          {
#ifdef FEATURE_LTE
            case LTE_RRC_G_RESEL_FAILED_RSP:
            {
              rr_resel_g2w_handle_lte_rrc_G_resel_failed_rsp(
                &rr_resel_g2w_data_ptr->target,
                &msg->rr_msgr_msg.message.lte_rrc_G_resel_failed_rsp,
                gas_id
              );

              rr_resel_g2w_data_ptr->status = RR_RESEL_G2W_STATUS_RESELECTION_FAILURE;

              rr_resel_g2w_activate_l1(gas_id);

              /*reset Scell info in CM as it has been reset 
                by higher layers during the redirection*/
              geran_multisim_eng_mode_refresh_cm(ENG_MODE_SCELL_INFO, gas_id);

              rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_ACTIVATING;
              break;
            } /* LTE_RRC_G_RESEL_FAILED_RSP */
#endif /* FEATURE_LTE */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_MSGR_RR */

        case MS_MM_RR:
        {
          switch (msg->message_header.message_id)
          {
            case RR_STOP_GSM_MODE_REQ:
            {
              rr_stop_gsm_mode_req_T *stop_gsm_mode_req = &msg->rr_stop_gsm_mode_req;

              if ((stop_gsm_mode_req->rr_mm_gsm_stop_reason == RR_MM_DEEP_SLEEP)  ||
                  (stop_gsm_mode_req->rr_mm_gsm_stop_reason == RR_MM_DUAL_SWITCH) ||
                  (stop_gsm_mode_req->rr_mm_gsm_stop_reason == RR_MM_USER_RAT_CHANGE))
              {
                rr_resel_g2w_send_imsg_abort_ind(RR_RESEL_G2W_ABORT_CAUSE_MM_REQ, gas_id);
              }
              else
              {
                rr_resel_g2x_log_resel_success(&rr_resel_g2w_data_ptr->target, gas_id);
                rr_resel_g2w_send_omsg_resel_cnf(RR_RESEL_G2W_STATUS_RESELECTION_SUCCESS, gas_id);
                rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_NULL;
              }
              break;
            } /* RR_STOP_GSM_MODE_REQ */

            case RR_ACTIVATION_RSP:
            {
              /* Restore the previously-cached RAT Priority List as it may have
              been overwritten following a call to rr_rrc_set_band_pref(). */
              rr_resel_g2w_restore_rat_pri_list_info(gas_id);

              rr_resel_g2w_activate_l1(gas_id);
              (void)rr_timer_start(T3176, T3176_TIMEOUT, gas_id);
              rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_ACTIVATING;
              break;
            } /* RR_ACTIVATION_RSP */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_MM_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg,
            rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
          );
        }
      }
      break;
    } /* RR_RESEL_G2W_STATE_RESELECTING */

    case RR_RESEL_G2W_STATE_RESELECTING_ABORTED:
    {
      switch (msg->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg->rr.header.rr_message_set)
          {
            case RR_RESEL_G2W_IMSG:
            {
              rr_resel_g2w_imsg_u *imsg;
              imsg = &msg->rr.resel_g2w.imsg;

              switch (imsg->header.imh.message_id)
              {
                case RR_RESEL_G2W_IMSG_ABORT_IND:
                {
                  /* No action necessary - already aborting. */
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg,
                    rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_RESEL_G2W_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

#ifdef FEATURE_GSM_TDS
        case MS_TDSRRC_RR:
        {
          switch (msg->message_header.message_id)
          {
            case TDSRRC_INTERRAT_RESELECTION_REJ:
            {
              rrc_interrat_reselect_rej_type rrc_resel_rej;
              (void)rr_timer_cancel(RR_IRAT_PROC_TIMER,gas_id); /*hari*/

              rr_convert_tds_to_rrc_resel_rej(&msg->tdsrrc_resel_rej, &rrc_resel_rej);

              rr_resel_g2w_handle_rrc_interrat_reselection_rej(
                &rr_resel_g2w_data_ptr->target,
                &rrc_resel_rej,
                gas_id
              );

              /* No need to set up rr_resel_g2w_data_ptr->status here as it will be
              set to indicate the procedure was aborted later. */

              rr_resel_g2w_activate_l1(gas_id);

              /*reset Scell info in CM as it has been reset 
                by higher layers during the redirection*/
              geran_multisim_eng_mode_refresh_cm(ENG_MODE_SCELL_INFO, gas_id);

              rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_ACTIVATING_ABORTED;
              break;
            }

            case TDSRRC_INTERRAT_RESELECTION_ABORT_CNF:
            {
                
              (void)rr_timer_cancel(RR_IRAT_PROC_TIMER,gas_id);
              rr_resel_g2w_activate_l1(gas_id);
              rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_ACTIVATING_ABORTED;
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /*MS_TDSRRC_RR*/
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
        case MS_RRC_RR:
        {
          switch (msg->message_header.message_id)
          {
            case RRC_INTERRAT_RESELECTION_ABORT_CNF:
            {
              /* Restore the previously-cached RAT Priority List as it may have
              been overwritten following a call to rr_rrc_set_band_pref(). */
              
              (void)rr_timer_cancel(RR_IRAT_PROC_TIMER,gas_id);
              rr_resel_g2w_restore_rat_pri_list_info(gas_id);

              rr_resel_g2w_activate_l1(gas_id);
              rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_ACTIVATING_ABORTED;
              break;
            }

            case RRC_INTERRAT_RESELECTION_REJ:
            {
                
              (void)rr_timer_cancel(RR_IRAT_PROC_TIMER,gas_id);
              rr_resel_g2w_handle_rrc_interrat_reselection_rej(
                &rr_resel_g2w_data_ptr->target,
                &msg->rrc_interrat_reselect_rej,
                gas_id
              );
              
              /* No need to set up rr_resel_g2w_data_ptr->status here as it will be
              set to indicate the procedure was aborted later. */

              /* Restore the previously-cached RAT Priority List as it may have
              been overwritten following a call to rr_rrc_set_band_pref(). */
              rr_resel_g2w_restore_rat_pri_list_info(gas_id);

              rr_resel_g2w_activate_l1(gas_id);

              /*reset Scell info in CM as it has been reset 
                by higher layers during the redirection*/
              geran_multisim_eng_mode_refresh_cm(ENG_MODE_SCELL_INFO, gas_id);

              rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_ACTIVATING_ABORTED;
              break;
            }

			
            case RRC_INTERRAT_CC_ORDER_CNF:
            {
              rrc_interrat_cc_order_cnf_type *interrat_cc_order_cnf = &msg->rrc_interrat_cc_order_cnf;
									
              switch (interrat_cc_order_cnf->status)
              {
                case CCO_FAILURE_AFTER_ACT_IND: 
                {
                  rr_resel_g2w_data_ptr->status = rr_resel_g2w_convert_rrc_failure_reason(
					 interrat_cc_order_cnf->failure_reason
				  );
				  rr_send_activation_ind(MM_AS_GTOW_CC_ORDER_FAILED,gas_id);
				  break;
				} /* CCO_FAILURE_AFTER_ACT_IND */
																						
                case CCO_FAILURE_BEFORE_ACT_IND:
                {
                  rr_resel_g2w_data_ptr->status = rr_resel_g2w_convert_rrc_failure_reason(
					interrat_cc_order_cnf->failure_reason
				  );
									
#ifdef FEATURE_RAT_PRIORITY_LIST
                  /* Restore the previously-cached RAT Priority List as it may have
					  been overwritten following a call to rr_rrc_set_band_pref(). */
										  
                  rr_resel_g2w_restore_rat_pri_list_info(gas_id);
			
#endif /* FEATURE_RAT_PRIORITY_LIST */
									
				  rr_resel_g2w_activate_l1(gas_id);
				  (void)rr_timer_start(T3176, T3176_TIMEOUT, gas_id);
				  rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_ACTIVATING;
				  break;
				} /* CCO_FAILURE_BEFORE_ACT_IND */
																						
			    default:
                {
                  MSG_GERAN_ERROR_1_G("Unexpected CCO status: %d", interrat_cc_order_cnf->status);
                }
              }
			  break;
            } 
						

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RRC_RR */
#endif /* FEATURE_WCDMA */

        case MS_MM_RR:
        {
          switch (msg->message_header.message_id)
          {
            case RR_STOP_GSM_MODE_REQ:
            {
              /* No action necessary - already aborting. */
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_MM_RR */

        case MS_TIMER:
        {
          switch (msg->mid_timer_expiry.timer_id)
          {
            case RR_IRAT_PROC_TIMER:
            {
              switch (rr_resel_g2w_data_ptr->target.rat)
              {
#ifdef FEATURE_WCDMA
                case RR_RAT_WCDMA:
                {
                  MSG_GERAN_HIGH_0_G("No response received from WCDMA RRC");
                  break;
                } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                case RR_RAT_UTRAN_TDD:
                {
                  MSG_GERAN_HIGH_0_G("No response received from TDS RRC");
                  break;
                } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
                case RR_RAT_LTE:
                {
                  MSG_GERAN_HIGH_0_G("No response received from LTE RRC");
                  break;
                } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

                default:
                {
                  MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rr_resel_g2w_data_ptr->target.rat);
                }
              } /* rr_resel_g2w_data_ptr->target.rat */
              break;
            } /* RR_IRAT_PROC_TIMER */
            
            default:
            {
              LOG_UNEXPECTED_TIMER_EXPIRY(
                msg->mid_timer_expiry.timer_id,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          } /* msg->mid_timer_expiry.timer_id */
          break;
        } /* MS_TIMER */

        case MS_MSGR_RR:
        {
          switch (msg->rr_msgr_msg.message.hdr.id)
          {
#ifdef FEATURE_LTE
            case LTE_RRC_G_ABORT_RESEL_RSP:
            {
              /* Restore the previously-cached RAT Priority List as it may have
              been overwritten following a call to rr_rrc_set_band_pref(). */
              rr_resel_g2w_restore_rat_pri_list_info(gas_id);              
              (void)rr_timer_cancel(RR_IRAT_PROC_TIMER,gas_id);

              rr_resel_g2w_activate_l1(gas_id);
              rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_ACTIVATING_ABORTED;
              break;
            } /* LTE_RRC_G_ABORT_RESEL_RSP */

            case LTE_RRC_G_RESEL_FAILED_RSP:
            {
              rr_resel_g2w_handle_lte_rrc_G_resel_failed_rsp(
                &rr_resel_g2w_data_ptr->target,
                &msg->rr_msgr_msg.message.lte_rrc_G_resel_failed_rsp,
                gas_id
              );
              
              (void)rr_timer_cancel(RR_IRAT_PROC_TIMER,gas_id);

              /* No need to set up rr_resel_g2w_data_ptr->status here as it will be
              set to indicate the procedure was aborted later. */

              /* Restore the previously-cached RAT Priority List as it may have
              been overwritten following a call to rr_rrc_set_band_pref(). */
              rr_resel_g2w_restore_rat_pri_list_info(gas_id);

              rr_resel_g2w_activate_l1(gas_id);

              /*reset Scell info in CM as it has been reset 
                by higher layers during the redirection*/
              geran_multisim_eng_mode_refresh_cm(ENG_MODE_SCELL_INFO, gas_id);

              rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_ACTIVATING_ABORTED;
            } /* LTE_RRC_G_RESEL_FAILED_RSP */
#endif /* FEATURE_LTE */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_MSGR_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg,
            rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
          );
        }
      }
      break;
    } /* RR_RESEL_G2W_STATE_RESELECTING_ABORTED */

    case RR_RESEL_G2W_STATE_ACTIVATING:
    {
      switch (msg->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg->rr.header.rr_message_set)
          {
            case RR_RESEL_G2W_IMSG:
            {
              rr_resel_g2w_imsg_u *imsg;
              imsg = &msg->rr.resel_g2w.imsg;

              switch (imsg->header.imh.message_id)
              {
                case RR_RESEL_G2W_IMSG_L1_ACTIVATED_IND:
                {
                  rr_resel_g2w_send_omsg_resel_cnf(rr_resel_g2w_data_ptr->status, gas_id);
                  rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_NULL;
                  break;
                } /* RR_RESEL_G2W_L1_DEACTIVATED_IND */

                case RR_RESEL_G2W_IMSG_ABORT_IND:
                {
                  MSG_GERAN_HIGH_1_G("Received RR_RESEL_G2W_IMSG_ABORT_IND(abort_cause => %d)",
                           msg->rr.resel_g2w.imsg.abort_ind.abort_cause);

                  switch (msg->rr.resel_g2w.imsg.abort_ind.abort_cause)
                  {
                    case RR_RESEL_G2W_ABORT_CAUSE_RR_EST_REQ:
                    {
                      MSG_GERAN_HIGH_0_G("Ignoring abort for RR_EST_REQ, no longer camped");
                      break;
                    }

                    default:
                    {
                      MSG_GERAN_HIGH_0_G("Aborting reselection");
                      rr_resel_g2x_log_resel_aborted(&rr_resel_g2w_data_ptr->target, gas_id);
                      rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_ACTIVATING_ABORTED;
                    }
                  }
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg,
                    rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_RESEL_G2W_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_MM_RR:
        {
          switch (msg->message_header.message_id)
          {
            case RR_STOP_GSM_MODE_REQ:
            {
              rr_resel_g2w_send_imsg_abort_ind(RR_RESEL_G2W_ABORT_CAUSE_MM_REQ, gas_id);
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_MM_RR */

#ifdef FEATURE_SGLTE
        case MS_RR_L1:
        {
          switch (msg->message_header.message_id)
          {
            case MPH_UE_MODE_CHANGE_CNF:
            {
              if (rr_is_sglte_mode_active(gas_id))
              {
                MSG_GERAN_HIGH_0_G("MPH_UE_MODE_CHANGE_CNF rxd");
                rr_resel_g2w_send_imsg_l1_activated_ind(gas_id);
              }
              break;
            } /* MPH_UE_MODE_CHANGE_CNF */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_L1 */
#endif /* FEATURE_SGLTE */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg,
            rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
          );
        }
      }
      break;
    } /* RR_RESEL_G2W_STATE_ACTIVATING */

    case RR_RESEL_G2W_STATE_ACTIVATING_ABORTED:
    {
      switch (msg->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg->rr.header.rr_message_set)
          {
            case RR_RESEL_G2W_IMSG:
            {
              rr_resel_g2w_imsg_u *imsg;
              imsg = &msg->rr.resel_g2w.imsg;

              switch (imsg->header.imh.message_id)
              {
                case RR_RESEL_G2W_IMSG_L1_ACTIVATED_IND:
                {
                  rr_resel_g2w_send_omsg_resel_cnf(RR_RESEL_G2W_STATUS_ABORTED_OUT_OF_SERVICE, gas_id);
                  rr_resel_g2w_data_ptr->state = RR_RESEL_G2W_STATE_NULL;
                  break;
                } /* RR_RESEL_G2W_L1_DEACTIVATED_IND */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg,
                    rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_RESEL_G2W_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_MM_RR:
        {
          switch (msg->message_header.message_id)
          {
            case RR_STOP_GSM_MODE_REQ:
            {
              /* No action necessary - already aborting. */
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_MM_RR */

#ifdef FEATURE_SGLTE
        case MS_RR_L1:
        {
          switch (msg->message_header.message_id)
          {
            case MPH_UE_MODE_CHANGE_CNF:
            {
              if (rr_is_sglte_mode_active(gas_id))
              {
                MSG_GERAN_HIGH_0_G("MPH_UE_MODE_CHANGE_CNF rxd");
                rr_resel_g2w_send_imsg_l1_activated_ind(gas_id);
              }
              break;
            } /* MPH_UE_MODE_CHANGE_CNF */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg,
                rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_L1 */
#endif /* FEATURE_SGLTE */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg,
            rr_resel_g2w_state_name(rr_resel_g2w_data_ptr->state)
          );
        }
      }
      break;
    } /* RR_RESEL_G2W_STATE_ACTIVATING_ABORTED */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unknown state: %d", rr_resel_g2w_data_ptr->state);
    }
  }

  if (rr_resel_g2w_data_ptr->old_state != rr_resel_g2w_data_ptr->state)
  {
    RR_LOG_STATE_TRANSITION(
      "rr_resel_g2w_control",
      rr_resel_g2w_data_ptr->old_state,
      rr_resel_g2w_data_ptr->state,
      rr_resel_g2w_state_name,
      gas_id
    );

    rr_resel_g2w_data_ptr->old_state = rr_resel_g2w_data_ptr->state;
  }
}

boolean rr_resel_g2w_in_progress(const gas_id_t gas_id)
{
  rr_resel_g2w_data_t *rr_resel_g2w_data_ptr = rr_resel_g2w_get_data_ptr(gas_id);

  if (NULL != rr_resel_g2w_data_ptr)
  {
    /* return TRUE if this module is in a non-null state */
    return(RR_RESEL_G2W_STATE_NULL != rr_resel_g2w_data_ptr->state);
  }

  return(FALSE);
}

#else /* FEATURE_WCDMA || FEATURE_GSM_TDS */

void rr_resel_g2w_dummy_func(void)
{
  ;
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

/* EOF */

