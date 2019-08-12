/*==============================================================================
  FILE:         rr_sglte_g2x_conn_resel.c

  OVERVIEW:     This module implements the Connected Mode SGLTE RR G2X Cell Reselection state
                machine and it's associated functionality. This file is only applicable when SGLTE Mode is enabled.

  DEPENDENCIES: None

                Copyright (c) 2009-2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
==============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sglte_g2x_conn_resel.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/


#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_SGLTE
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_sglte_g2x_conn_resel.h"
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
#include "rr_l1_send.h"
#include "rr_multi_sim.h"
#include "rr_nv.h"
#include "rr_resel.h"
#include "rr_ps_domain.h"
#include "rr_sglte.h"
#include "rr_rr.h"
#include "rr_if.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef enum
{
  RR_SGLTE_G2X_RESEL_CONN_NULL,
  RR_SGLTE_G2X_RESEL_CONN_EVALUATING,
  RR_SGLTE_G2X_RESEL_CONN_SUSPENDING,
  RR_SGLTE_G2X_RESEL_CONN_PENDING_L1_MULTIRAT_DEACT,
  RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_DEACT_ABORTED,
  RR_SGLTE_G2X_RESEL_CONN_RESELECTING,
  RR_SGLTE_G2X_RESEL_CONN_RESELECTING_ABORTED,
  RR_SGLTE_G2X_RESEL_CONN_RESUMING,
  RR_SGLTE_G2X_RESEL_CONN_SUSPENDING_ABORTED,
  RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_ACTIVATION,
  RR_SGLTE_G2X_RESEL_CONN_MAX
} rr_sglte_g2x_resel_conn_state_e;

typedef struct
{
  rr_sglte_g2x_resel_conn_state_e              state;
  rr_sglte_g2x_resel_conn_state_e              old_state;
  rr_irat_cell_t                    target;
  sys_network_selection_mode_e_type network_selection_mode;
  PLMN_id_T                         serving_cell_plmn_id;
  rr_sglte_g2x_resel_conn_status_e             status;
  rr_cb_fn_t      ps_access_confim_cb_fn;
#ifdef FEATURE_RAT_PRIORITY_LIST
  sys_rat_pri_list_info_s_type      rat_pri_list_info;
#endif /* FEATURE_RAT_PRIORITY_LIST */
  boolean is_abort_in_progress;
} rr_sglte_g2x_resel_conn_data_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

void rr_sglte_g2x_conn_send_omsg(
  rr_sglte_g2x_resel_conn_omsg_e omsg,
  const gas_id_t gas_id
)
{
  rr_message_header_t header;

  memset(&header, 0, sizeof(header));

  rr_rr_send(&header,
             (uint8) omsg,
             sizeof(header),
             RR_SGLTE_G2X_RESEL_CONN_OMSG,
             gas_id);
}


/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_sglte_g2x_resel_conn_data_t  rr_sglte_g2x_resel_conn_data_store[NUM_GERAN_DATA_SPACES];


/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
    /*!
     * \brief Returns a pointer to the module data.
     *
     * \param gas_id
     *
     * \return rr_sglte_g2x_resel_conn_data_t*
     */
static rr_sglte_g2x_resel_conn_data_t *rr_sglte_g2x_resel_conn_get_data_ptr(
 const gas_id_t gas_id
  )
{
   uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

   return &rr_sglte_g2x_resel_conn_data_store[as_index];
}

#ifdef FEATURE_GSM_TDS

static rr_sglte_g2x_resel_conn_status_e rr_sglte_g2x_conn_resel_map_fail_cause_to_status(
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
      /* MSG_GERAN_ERROR_1_G("Unknown cause: %d", (uint16) cause);*/
      return RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_UNKNOWN;
  }
}

#endif


static const char *rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_state_e state)
{
  switch (state)
  {
    case RR_SGLTE_G2X_RESEL_CONN_NULL:
      return "NULL";

    case RR_SGLTE_G2X_RESEL_CONN_EVALUATING:
      return "EVALUATING";

    case RR_SGLTE_G2X_RESEL_CONN_SUSPENDING:
      return "SUSPENDING";

    case RR_SGLTE_G2X_RESEL_CONN_PENDING_L1_MULTIRAT_DEACT:
      return "PENDING_L1_MULTIRAT_DEACT";

    case RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_DEACT_ABORTED:
      return "L1_MULTIRAT_DEACT_ABORTED";

    case RR_SGLTE_G2X_RESEL_CONN_RESELECTING:
      return "RESELECTING";

    case RR_SGLTE_G2X_RESEL_CONN_RESELECTING_ABORTED:
      return "RESELECTING_ABORTED";

    case RR_SGLTE_G2X_RESEL_CONN_RESUMING:
      return "RESUMING";

    case RR_SGLTE_G2X_RESEL_CONN_SUSPENDING_ABORTED:
      return "SUSPENDING_ABORTED";

    case RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_ACTIVATION:
      return "L1_MULTIRAT_ACTIVATION";


    default:
     /* MSG_GERAN_ERROR_1_G("Unknown state: %d", state); */
      return "?";
  }
}

static void rr_sglte_g2x_resel_conn_log_start(rr_irat_cell_t * target, const gas_id_t gas_id)
{
  RR_NULL_CHECK_RETURN_VOID(target);

  switch (target->rat)
  {

    case RR_RAT_LTE:
    {
      rr_log_g2l_resel_started(
        target->data.lte.earfcn,
        target->data.lte.pcid,
        gas_id
      );
      break;
    } /* RR_RAT_LTE */



    case RR_RAT_UTRAN_TDD:
    {
      rr_log_g2t_reselect_started(
        target->data.utran_tdd.uarfcn,
        target->data.utran_tdd.cpid,
        gas_id
        );
      break;
    } /* RR_RAT_LTE */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unknown RAT: %d", target->rat);
    }
  }
}

static void rr_sglte_g2x_resel_conn_log_end(rr_irat_cell_t * target, const gas_id_t gas_id)
{
  RR_NULL_CHECK_RETURN_VOID(target);

  switch (target->rat)
  {

    case RR_RAT_LTE:
    {
      rr_log_g2l_resel_success(
      target->data.lte.earfcn,
      target->data.lte.pcid,
      gas_id
      );
      break;
    } /* RR_RAT_LTE */


    case RR_RAT_UTRAN_TDD:
    {
      rr_log_g2t_reselect_success(
       target->data.utran_tdd.uarfcn,
       target->data.utran_tdd.cpid,
       gas_id
       );
      break;
    } /* RR_RAT_LTE */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unknown RAT: %d", target->rat);
    }
  }
}


/*!
  * \brief Receives Reselection Reject Cause from TDS and takes appropriate action in
  *        g2w data base
  * \param IRAT Cell information
  * \param IRAT Rejection Type
  * \param gas_id
  *
  * \return None
  */

static void rr_sglte_g2x_resel_conn_handle_rrc_irat_resel_rej(
  rr_irat_cell_t                 * cell,
  rrc_interrat_reselect_rej_type * msg,
  const gas_id_t                   gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(cell);
  RR_NULL_CHECK_RETURN_VOID(msg);

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
#ifdef FEATURE_GSM_TDS
  else if (cell->rat == RR_RAT_UTRAN_TDD)
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

      data.plmn_id = msg->PLMN;

      rr_g2x_resel_failed(
        cell,
        RR_G2X_RESEL_FAILURE_CAUSE_PLMN_MISMATCH,
        &data,
        gas_id
      );
      break;
    } /* RRC_WCDMA_FORBIDDEN_LA */

    case RRC_WCDMA_CELL_BARRED:
    {
      rr_g2x_resel_failed(
        cell,
        RR_G2X_RESEL_FAILURE_CAUSE_CELL_BARRED,
        NULL,
        gas_id
      );
      break;
    } /* RRC_WCDMA_CELL_BARRED */

    case RRC_WCDMA_UARFCN_BARRED:
    {
      rr_g2x_resel_failed(
        cell,
        RR_G2X_RESEL_FAILURE_CAUSE_FREQUENCY_BARRED,
        NULL,
        gas_id
      );
      break;
    } /* RRC_WCDMA_FORBIDDEN_LA */

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
}  /* End of FUnction rr_sglte_g2x_resel_conn_handle_rrc_irat_resel_rej  */



/*!
  * \brief Receives Reselection Reject Cause from LTE and takes appropriate action in
  *        g2w data base
  * \param IRAT Cell information
  * \param IRAT Rejection Type
  * \param gas_id
  *
  * \return None
  */

#ifdef FEATURE_LTE
/**
  @ingroup rr_resel_g2l
*/
static void rr_sglte_g2x_resel_conn_handle_lte_rrc_G_resel_failed_rsp(
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
    rr_sglte_g2x_conn_resel_map_fail_cause_to_status(msg->fail_cause),
    gas_id
  );

  /*Convert the  time to bar received in LTE message in miliseconds*/
  msg->cell_bar_time *= 1000;

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


/*!
 * \brief This function is the call-back supplied in the payload of RR_IMSG_NO_PS_ACCESS_REQ.
 *        It sends EV_G2X_CONN_RESEL_PS_BLOCKED into rr_sglte_rr_conn_g2x_resel_control().
 * 
 * \param gas_id 
 */
static void rr_sglte_g2x_resel_conn_ps_blocked_cb_fn(const gas_id_t gas_id)
{
  rr_sglte_rr_conn_g2x_resel_control(EV_G2X_CONN_RESEL_PS_BLOCKED, NULL, gas_id);
}

/*!
 * \brief Called to suspend GPRS. This sends RR_IMSG_NO_PS_ACCESS_REQ into RR-CONTROL.
 *        Note: rr_sglte_g2x_resel_conn_ps_blocked_cb_fn() will be called once PS is blocked.
 * 
 * \param gas_id 
*/
static void rr_sglte_g2x_resel_conn_suspend_mac(const gas_id_t gas_id)
{
   /**
   * Send the request to block PS access into RR-CONTROL. 
   * This will be handled appropriately per-state, and the call-back function called once PS is blocked.
   */
  rr_send_imsg_no_ps_access_req(
    FALSE,                                      // del_all_pdus
    rr_sglte_g2x_resel_conn_ps_blocked_cb_fn,   // confirm_cb_fn
    gas_id                                      // gas_id
  );
}

/*!
  * \brief This fucntion saves the RAT priority List in the local Reselection DataStore.
  *        In case the Reselection to other RAT is resjected or ABorted, then the RAT priotiy List needs to be restored.
  * \param gas_id
  *
  * \return None
  */

static void rr_sglte_g2x_resel_conn_cache_rat_pri_list_info(gas_id_t gas_id)
{
  rr_PLMN_request_details_T * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  rr_sglte_g2x_resel_conn_data_t* rr_sglte_g2x_resel_conn_data_store_ptr = rr_sglte_g2x_resel_conn_get_data_ptr(gas_id );
  RR_NULL_CHECK_RETURN_VOID(plmn_request_details);
  RR_NULL_CHECK_RETURN_VOID(rr_sglte_g2x_resel_conn_data_store_ptr);

  rr_sglte_g2x_resel_conn_data_store_ptr->rat_pri_list_info = plmn_request_details->rat_pri_list_info;
}

/*!
  * \brief This fucntion Restores the RAT priority List From the local Reselection DataStore back to PLMN Request Database
  *        Used In case the Reselection to other RAT is resjected or ABorted, then the RAT priotiy List needs to be restored.
  * \param gas_id
  *
  * \return None
  */

static void rr_sglte_g2x_resel_restore_rat_pri_list_info(gas_id_t gas_id)
{
  rr_PLMN_request_details_T * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  rr_sglte_g2x_resel_conn_data_t* rr_sglte_g2x_resel_conn_data_store_ptr = rr_sglte_g2x_resel_conn_get_data_ptr(gas_id);
  RR_NULL_CHECK_RETURN_VOID(plmn_request_details);

  plmn_request_details->rat_pri_list_info = rr_sglte_g2x_resel_conn_data_store_ptr->rat_pri_list_info;
}


/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * --------------------------------------------------------------------------*/


void rr_sglte_rr_conn_g2x_resel_init(const gas_id_t gas_id)
{
  rr_sglte_g2x_resel_conn_data_t* rr_sglte_g2x_resel_conn_data_p =  rr_sglte_g2x_resel_conn_get_data_ptr(gas_id );

  memset(rr_sglte_g2x_resel_conn_data_p, 0, sizeof(rr_sglte_g2x_resel_conn_data_t));
}

/*!
  * \brief This fucntion is the main G2X Reselection Controller.
  * \param rr_event_T The INPUT Control Event
  * \param rr_cmd_body The Message Pointer passed to thsi FSM.
  * \param gas_id
  *
  * \return rr_event_T The OUT EVENT: If OUT_EVENT is NO_EVENT, it means the Incoming Event is Consumed by the FSM.
  */

rr_event_T rr_sglte_rr_conn_g2x_resel_control(rr_event_T control_event, rr_cmd_bdy_type *new_message, const gas_id_t gas_id )
{
  rr_sglte_g2x_resel_conn_data_t* rr_sglte_g2x_resel_conn_data_p =  rr_sglte_g2x_resel_conn_get_data_ptr(gas_id );
  rr_event_T out_event = control_event;
  rr_event_T new_control_event = EV_NO_EVENT ;

  if( rr_get_nv_sglte_g2x_conn_mode_resel_enabled(RR_GAS_ID_TO_AS_ID) == FALSE )
  {
      MSG_GERAN_HIGH_0_G("SGLTE G2X Connected Mode Reselection feature disabled in NV");
      return out_event;
  }

  if((EV_INPUT_MESSAGE == control_event) && (NULL == new_message))
  {
     MSG_GERAN_ERROR_0_G("SGLTE G2X Connected Mode Reselection Controller Invoked with EV_INPUT_MESSAGE and a NULL Message Pointer");
     return EV_NO_EVENT;
  }

#ifdef DEBUG_RR_TRACE_MSG
  rr_store_trace_msg_buf(
    RR_SGLTE_RR_CONN_G2X_RESEL_SM,
    control_event,
    (byte) rr_sglte_g2x_resel_conn_data_p->state,
    new_message,
  gas_id
  );
#endif

  while (control_event != EV_NO_EVENT)
  {

    switch (rr_sglte_g2x_resel_conn_data_p->state)
    {
      case RR_SGLTE_G2X_RESEL_CONN_NULL:
      {
        /* Switch only on Events this FSM is prepared to handle in NULL state  */
        switch (control_event )
        {
        
          case EV_INPUT_MESSAGE:
          {
            switch (new_message->message_header.message_set)
            {
              case MS_RR_L1:
              {
                switch ((new_message->rr.rr.imsg.header.imh.message_id))
                {
                  case MPH_UE_MODE_CHANGE_CNF:
                  {
                    /* Continue, as no action needed on this message */					
                     break;
                  }
                  
                  default:
                   MSG_GERAN_HIGH_3_G("SGLTE G2X Connection Controller: Message Set %d, Message Id %d, not handled in State %d",
                                       new_message->message_header.message_set, 
                                       new_message->rr.rr.imsg.header.imh.message_id,
                                       rr_sglte_g2x_resel_conn_data_p->state );               
                  break;
                }
                break;
              }
              
              case RR_IMSG:
              {
                switch (new_message->rr.rr.imsg.header.imh.message_id)
                {
                  case RR_IMSG_PS_ACCESS_REQ:
                  {  
                    /* Continue, as no action needed on this message */	
                    break;
                  }
                  
                  case RR_IMSG_X2G_CCO_COMPLETE_IND:
                  {
                    /* Continue, as no action needed on this message */	
                    break;
                  }
                  
                  default:
                    MSG_GERAN_HIGH_3_G("SGLTE G2X Connection Controller: Message Set %d, Message Id %d, not handled in State %d",
                                       new_message->message_header.message_set, 
                                       new_message->rr.rr.imsg.header.imh.message_id,
                                       rr_sglte_g2x_resel_conn_data_p->state );                     
                  break;
                }
                break;
              }
              
              default:
                MSG_GERAN_HIGH_3_G("SGLTE G2X Connection Controller: Message Set %d, Message Id %d, not handled in State %d",
                                  new_message->message_header.message_set, 
                                  new_message->rr.rr.imsg.header.imh.message_id,
                                  rr_sglte_g2x_resel_conn_data_p->state );                     
              break;
            }
            break;
          }
          
          case EV_IRAT_SURROUND_MEAS_IND:
            new_control_event = EV_G2X_CONN_RESEL_START_RESEL_EVAL;
            rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_EVALUATING;
         MSG_GERAN_HIGH_0_G("EV RR_SGLTE_G2X_RESEL_CONN_EVALUATING Sent in SGLTE G2X Connected Mode Reselection Controller");

          break;
          
          case EV_G2X_CONN_RESEL_ABORT_REQ:
            out_event= EV_G2X_CONN_RESEL_ABORT_CNF;
          break;
          
          default:
            LOG_UNEXPECTED_EVENT(rr_event_name(control_event), 
                                 rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state));
          break;
        }/* End if Internal Switch on EVents */
      break;
    } /* RR_SGLTE_G2X_RESEL_CONN_NULL */

    case RR_SGLTE_G2X_RESEL_CONN_EVALUATING:
    {
      /* Only Event this FSM is prepared to handle in NULL state is the IRAT_SURROUND_MEAS_IND Event */
      switch (control_event )
      {
        case EV_IRAT_SURROUND_MEAS_IND:
        case EV_CR_G2W_TIMER_EXPIRED:
    case EV_G2X_CONN_RESEL_START_RESEL_EVAL:
        {
          boolean isPriorityBasedAlgo = FALSE;

          if (rr_is_lte_supported(gas_id) && rr_serving_cell_priority_parameters_ie_received(rr_gprs_get_scell_info(gas_id)))
          {
            isPriorityBasedAlgo = TRUE;
          }

          if ( isPriorityBasedAlgo )
          { /* RUN PRIORITY BASED ALGORITHM */

            rr_resel_cancel_all_rank_based_resel_timers(gas_id);
            /* Please note at the moment, we will give priority to Even Low priority LTE/TDS frequencies, over GSM Serving cell for PS Connection
             Later ON we may have to revisit this strategy. Immediate Reselection is FALSE */

            if (rr_irat_check_for_priority_based_reselection(TRUE,FALSE, gas_id))
            {
              new_control_event = EV_G2X_CONN_RESEL_START_RESELECTION;
              MSG_GERAN_HIGH_0_G("EV EV_G2X_CONN_RESEL_START_RESELECTION Sent in SGLTE G2X Connected Mode Reselection Controller");

            }
          }
          else
          {
            rr_resel_cancel_all_priority_based_resel_timers(gas_id);
            if (rr_irat_check_for_rank_based_reselection(
                   get_recent_reselection_flag_value(gas_id),
                   FALSE,
                   FALSE,
                  rr_inhibit_utran_reselection(gas_id),
                  FALSE, gas_id ))
            {
              new_control_event = EV_G2X_CONN_RESEL_START_RESELECTION;
              MSG_GERAN_HIGH_0_G("EV EV_G2X_CONN_RESEL_START_RESELECTION Sent in SGLTE G2X Connected Mode Reselection Controller");

            }
          } /* RUN RANK BASED ALGORITHM */

          rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_EVALUATING;
        }
        break;

        case EV_G2X_CONN_RESEL_START_RESELECTION:
        {
          /* Controller successfully evaluated a better TDS or LTE cell, now is the time to Reselection Actions */
          if ( grr_is_gmm_suspended(gas_id) )
          {
            /* Set L1 to GSM_ONLY */
            rr_send_mph_ue_mode_change_req(TRUE, gas_id);
            rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_PENDING_L1_MULTIRAT_DEACT;
          }
          else
          {
            /* Need to Suspend GPRS due to trigger of Reselection */
            rr_sglte_g2x_resel_conn_suspend_mac(gas_id);
            rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_SUSPENDING;
           }
           break;
        }

        case EV_G2X_CONN_RESEL_ABORT_REQ:
        {
          /* FR - 19574 , TO DO Check, do we need to clear any G2X Data */
          rr_g2wr_reset_wcdma_surrounding_database(gas_id);
          rr_g2wr_cancel_all_wcdma_five_second_timers(gas_id);
          MSG_GERAN_HIGH_0_G("SGLTE G2X Connected Mode Reselection Controller Transition to NULL");
          
          rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_NULL;
          out_event= EV_G2X_CONN_RESEL_ABORT_CNF;
        }
        break;

        default:
          LOG_UNEXPECTED_EVENT(rr_event_name(control_event), 
                               rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state));          
        break;

       }/* End oif INternal Switch on EVents */

       break;
    }  /* RR_SGLTE_G2X_RESEL_CONN_EVALUATING */
    case RR_SGLTE_G2X_RESEL_CONN_SUSPENDING:
    {
      switch (control_event)
      {
        case EV_G2X_CONN_RESEL_PS_BLOCKED:
        {
          /* PS is blocked in MAC/RLC. Now suspend GMM */
          rr_gmm_suspend_gprs(gas_id);

          /* Set L1 to GSM_ONLY */
          rr_send_mph_ue_mode_change_req(TRUE, gas_id);
          rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_PENDING_L1_MULTIRAT_DEACT;
          out_event = control_event;

          break;
        }
        case EV_G2X_CONN_RESEL_ABORT_REQ:
        {
          /* Controller sent an ABort while Reselection controller is in middle of suspension */
          /* DO NOT cancel suspension but wait it for it to finish */
          rr_sglte_g2x_resel_conn_data_p->is_abort_in_progress = TRUE;
          rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_SUSPENDING_ABORTED;
          out_event = EV_NO_EVENT; /* ANd wait for it to finish */
          break;
         }
         default:
         {
           out_event = control_event;
           control_event = EV_NO_EVENT;
           break;
          }
        }
        break;
      } /* RR_SGLTE_G2X_RESEL_CONN_SUSPENDING */

    case RR_SGLTE_G2X_RESEL_CONN_PENDING_L1_MULTIRAT_DEACT:
    {
      if (control_event == EV_INPUT_MESSAGE)
      {
      /* Received Mode change Confirm, in the state, waiting for Mode change Cnf */
      /* Get the Reselection Target and start Reselection to other RAT */
      switch (new_message->message_header.message_set)
      {
        case MS_RR_L1:
        {
          switch ((new_message->rr.rr.imsg.header.imh.message_id))
          {
            case MPH_UE_MODE_CHANGE_CNF:
            {

              rr_sglte_g2x_resel_conn_data_p->target = rr_irat_resel_target_get(gas_id);
              rr_sglte_g2x_resel_conn_cache_rat_pri_list_info(gas_id);

              MSG_GERAN_HIGH_0_G("MSG MPH_UE_MODE_CHANGE_CNF Received BY SGLTE G2X Connected Mode Reselection Controller");
              rr_sglte_g2x_resel_conn_log_start( &rr_sglte_g2x_resel_conn_data_p->target, gas_id );

              switch ( rr_sglte_g2x_resel_conn_data_p->target.rat )
              {
                boolean send_status = FALSE;
                case RR_RAT_UTRAN_TDD:
                {
                  rr_PLMN_request_details_T * rr_PLMN_request_details_ptr;
                  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
                  rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

                  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
                  {
                    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRRC_INTERRAT_RESELECTION_REQ);
                    return out_event;
                  }

                   MSG_GERAN_HIGH_0_G("MSG TDS RR RESELECTION REQUEST Sent FROM SGLTE G2X Connected Mode Reselection Controller");

                  rr_send_tds_interrat_reselection_req(
                  rr_sglte_g2x_resel_conn_data_p->target.data.utran_tdd.cpid,
                  rr_PLMN_request_details_ptr->network_select_mode,
                   &rr_sglte_g2x_resel_conn_data_p->rat_pri_list_info,
                   scell_info_ptr->gsm.location_area_identification.plmn_id,
                   0,
                   rr_sglte_g2x_resel_conn_data_p->target.data.utran_tdd.uarfcn,
                   gas_id);

                   send_status = TRUE;

                   if (send_status)
                   {
                     rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_RESELECTING;
                   }
                   out_event=control_event;
                   break;
                 }

                 case RR_RAT_LTE:
                 {
                   lte_rrc_G_resel_req_s resel_req;
                   errno_enum_type       result;
                   rr_PLMN_request_details_T * rr_PLMN_request_details_ptr;

                   gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
                   rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

                   memset(&resel_req, 0, sizeof(lte_rrc_G_resel_req_s));

                   msgr_init_hdr(
                   &resel_req.msg_hdr,
                   MSGR_GERAN_GRR,
                   LTE_RRC_G_RESEL_REQ
                   );

                   resel_req.earfcn = rr_sglte_g2x_resel_conn_data_p->target.data.lte.earfcn;
                   resel_req.lte_cell_id = rr_sglte_g2x_resel_conn_data_p->target.data.lte.pcid;
                   resel_req.nas_params.rat_pri_list = *rr_rat_pri_list_info(gas_id);
                   resel_req.nas_params.req_plmn = scell_info_ptr->gsm.location_area_identification.plmn_id;
                   resel_req.nas_params.network_select_mode = rr_PLMN_request_details_ptr->network_select_mode;

                   rr_dedicated_priority_info_get(&resel_req.dedicated_priority_info, gas_id);
                   MSG_GERAN_HIGH_0_G("MSG LTE RRC RR RESELECTION REQUEST Sent FROM SGLTE G2X Connected Mode Reselection Controller");

                   result = rr_msgr_send(&resel_req.msg_hdr, sizeof(resel_req), gas_id);
                   if (result != E_SUCCESS)
                   {
                     MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                   }
                   else
                   {
                     send_status = TRUE;

                   }

                   if (send_status)
                   {
                     rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_RESELECTING;
                   }
                     out_event=control_event;
                     break;
                   }

                   default:
                     break;
              } /*( rr_sglte_g2x_resel_conn_data_p->target.rat ) */
            }
            break;
            default:
              LOG_UNEXPECTED_MESSAGE(
                 new_message,
                 rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
               );
            break;
            } /* ((new_message->rr.rr.imsg.header.imh.message_id))*/

           break;
         }
         default:
           LOG_UNEXPECTED_MESSAGE(
              new_message,
              rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
            );
         break;
        } /* End of Switch new_message->message_header.message_set */
      } /* EV_INPUT_MESSAGE */
      else
       if ( control_event == EV_G2X_CONN_RESEL_ABORT_REQ )
       { /* COntroller received abort immediately when We were about to start a Reselection activity, waiting for Step 2 Mode Change Cnf */
         rr_sglte_g2x_resel_conn_data_p->is_abort_in_progress = TRUE;
         rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_DEACT_ABORTED;
         out_event = EV_NO_EVENT; /* Consume the EVent */
       }
       else
       {
         LOG_UNEXPECTED_EVENT(rr_event_name(control_event), 
                              rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state));         
       }
       break;
    } /* RR_SGLTE_G2X_RESEL_CONN_PENDING_L1_MULTIRAT_DEACT */


    case RR_SGLTE_G2X_RESEL_CONN_RESELECTING:
    {
      if (control_event == EV_INPUT_MESSAGE)
      {
        switch (new_message->message_header.message_set)
        {
          case MS_RR_RR:
          {
            switch (new_message->rr.header.rr_message_set)
            {
              case RR_IMSG:
              {
                if ( new_message->rr.rr.imsg.header.imh.message_id == RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ)
                {
                  /* Reselection is successful */
                  MSG_GERAN_HIGH_0_G("Received RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ by SGKTE Connected Mode Resel Controller");

                  rr_sglte_set_rat_priority_list_gsm_only(gas_id);

                  rr_store_service_domain(SYS_SRV_DOMAIN_CS_ONLY, gas_id);
                  rr_send_stop_gsm_mode_cnf(MM_AS_SUCCESS, gas_id);
                  rr_g2wr_reset_wcdma_surrounding_database(gas_id);
                  rr_sglte_g2x_resel_conn_log_end( &rr_sglte_g2x_resel_conn_data_p->target, gas_id );
                  out_event = control_event;
                  rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_NULL;
                  break;
                  }
                }

                default:
                  LOG_UNEXPECTED_MESSAGE(
                     new_message,
                     rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
                   );

                break;
             }
             break;
           } /* ENd of MS_RR_RR */
#ifdef FEATURE_GSM_TDS
           case MS_TDSRRC_RR:
           {
             switch (new_message->message_header.message_id)
             {
               case TDSRRC_INTERRAT_RESELECTION_REJ:
               {
                 rrc_interrat_reselect_rej_type rrc_resel_rej;

                 rr_convert_tds_to_rrc_resel_rej(&new_message->tdsrrc_resel_rej, &rrc_resel_rej);
                       rr_sglte_g2x_resel_conn_handle_rrc_irat_resel_rej(
                       &rr_sglte_g2x_resel_conn_data_p->target,
                       &rrc_resel_rej,
                       gas_id
                      );

                 rr_sglte_g2x_resel_conn_data_p->status = RR_RESEL_G2W_STATUS_RESELECTION_FAILURE;
                 rr_g2w_utran_tdd_measurements_delete(gas_id);
                 rr_send_mph_ue_mode_change_req(rr_gsm_only(gas_id),gas_id);
                 out_event = control_event;
                 rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_ACTIVATION;
                 break;
               }
               default:
                LOG_UNEXPECTED_MESSAGE(
                 new_message,
                 rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
                   );
                 break;

             }
             break;
           } /*MS_TDSRRC_RR*/
#endif /*FEATURE_GSM_TDS*/

            case MS_MSGR_RR:
            {
              switch (new_message->rr_msgr_msg.message.hdr.id)
              {
#ifdef FEATURE_LTE
                case LTE_RRC_G_RESEL_FAILED_RSP:
                {
                  rr_sglte_g2x_resel_conn_handle_lte_rrc_G_resel_failed_rsp(
                   &rr_sglte_g2x_resel_conn_data_p->target,
                   &new_message->rr_msgr_msg.message.lte_rrc_G_resel_failed_rsp,
                   gas_id
                   );
                   MSG_GERAN_HIGH_0_G("Received LTE_RRC_G_RESEL_FAILED_RSP by SGLTE Connected Mode Resel Controller");

                   rr_sglte_g2x_resel_conn_data_p->status = RR_RESEL_G2W_STATUS_RESELECTION_FAILURE;
                   rr_g2w_lte_measurements_delete(gas_id);
                   rr_send_mph_ue_mode_change_req(rr_gsm_only(gas_id),gas_id);

                   rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_ACTIVATION;
                   out_event = control_event;
                   break;
                 } /* LTE_RRC_G_RESEL_FAILED_RSP */
#endif /* FEATURE_LTE */

                 default:
                 {
                   LOG_UNEXPECTED_MESSAGE(
                   new_message,
                   rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
                   );
                   out_event = control_event;
                 }
               }
               break;
             } /* MS_MSGR_RR */
             default:
              LOG_UNEXPECTED_MESSAGE(
              new_message,
              rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
              );

              out_event=control_event;
              break;
           }
         }
         if (control_event == EV_G2X_CONN_RESEL_ABORT_REQ )
         { /* Received ABort in the middle of Reselection */

           MSG_GERAN_HIGH_0_G("Aborting reselection");

           switch (rr_sglte_g2x_resel_conn_data_p->target.rat)
           {
             case RR_RAT_WCDMA:
             {
                rr_send_interrat_reselection_abort_req(gas_id);
                break;
             } /* RR_RAT_WCDMA */

#ifdef FEATURE_GSM_TDS
             case RR_RAT_UTRAN_TDD:
             {
               rr_send_tds_interrat_reselection_abort_req(gas_id);
               break;
             } /* RR_RAT_TDSCDMA */
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
               break;
              } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

              default:
              {
                MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rr_sglte_g2x_resel_conn_data_p->target.rat);
              }
           } /* rr_sglte_g2x_resel_conn_data_p->target.rat */


           rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_RESELECTING_ABORTED;
           rr_sglte_g2x_resel_conn_data_p->is_abort_in_progress = TRUE;
           out_event = EV_NO_EVENT; /* ABsorb the EVent */
         } /* control_event == EV_G2X_CONN_RESEL_ABORT_REQ*/
         else
         {
           LOG_UNEXPECTED_EVENT(rr_event_name(control_event), 
                                rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state));        
         }
       break;
    } /* RR_SGLTE_G2X_RESEL_CONN_RESELECTING */

    case RR_SGLTE_G2X_RESEL_CONN_RESELECTING_ABORTED:
    {
      /* COntroller sent ABort Request to other RAT and is waiting for Confirm */
      if (control_event == EV_INPUT_MESSAGE)
      {
        switch (new_message->message_header.message_set)
        {

#ifdef FEATURE_GSM_TDS
          case MS_TDSRRC_RR:
          {
            switch (new_message->message_header.message_id)
            {
              case TDSRRC_INTERRAT_RESELECTION_REJ:
              {
                rrc_interrat_reselect_rej_type rrc_resel_rej;

                rr_convert_tds_to_rrc_resel_rej(&new_message->tdsrrc_resel_rej, &rrc_resel_rej);

                rr_sglte_g2x_resel_conn_handle_rrc_irat_resel_rej(
                  &rr_sglte_g2x_resel_conn_data_p->target,
                  &rrc_resel_rej,
                  gas_id
                  );

                  rr_sglte_g2x_resel_conn_data_p->status = RR_RESEL_G2W_STATUS_RESELECTION_FAILURE;
                  rr_send_mph_ue_mode_change_req(rr_gsm_only(gas_id),gas_id);
                  rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_ACTIVATION;
                  out_event = EV_NO_EVENT;
                  break;
               }

               case TDSRRC_INTERRAT_RESELECTION_ABORT_CNF:
               {
                 rr_send_mph_ue_mode_change_req(rr_gsm_only(gas_id),gas_id);
                  rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_ACTIVATION;
                  /* Incoming Event is consumed */
                  out_event = EV_NO_EVENT;
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
                    );
                }
              }
              break;
            } /*MS_TDSRRC_RR*/
#endif /*FEATURE_GSM_TDS*/

          case MS_RRC_RR:
          {
            switch (new_message->message_header.message_id)
            {

              case RRC_INTERRAT_RESELECTION_REJ:
              {
                rr_sglte_g2x_resel_conn_handle_rrc_irat_resel_rej(
                  &rr_sglte_g2x_resel_conn_data_p->target,
                  &new_message->rrc_interrat_reselect_rej,
                  gas_id
                  );

                /* Restore the previously-cached RAT Priority List as it may have
                 been overwritten following a call to rr_rrc_set_band_pref(). */
                 rr_sglte_g2x_resel_restore_rat_pri_list_info(gas_id);
                 rr_send_mph_ue_mode_change_req(rr_gsm_only(gas_id),gas_id);
                 rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_ACTIVATION;
                 out_event = EV_NO_EVENT;
                break;
              }
              case RRC_INTERRAT_RESELECTION_ABORT_CNF:
              {
                /* Restore the previously-cached RAT Priority List as it may have
                 been overwritten following a call to rr_rrc_set_band_pref(). */
                 rr_sglte_g2x_resel_restore_rat_pri_list_info(gas_id);
                 rr_send_mph_ue_mode_change_req(rr_gsm_only(gas_id),gas_id);
                 rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_ACTIVATION;
                 out_event = EV_NO_EVENT;
                 break;
              }


             default:
             {
               LOG_UNEXPECTED_MESSAGE(
                new_message,
                rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
                );
             }
           }
            break;
          } /* MS_RRC_RR */

          case MS_MM_RR:
          {
            switch (new_message->message_header.message_id)
            {
              case RR_STOP_GSM_MODE_REQ:
              {
               /* No action necessary - already aborting. */
               break;
              }
              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                new_message,
                rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
                );
              break;
              }
            }
            break;
          } /* MS_MM_RR */

          case MS_MSGR_RR:
          {
            switch (new_message->rr_msgr_msg.message.hdr.id)
            {
#ifdef FEATURE_LTE
              case LTE_RRC_G_RESEL_FAILED_RSP:
              {
                rr_sglte_g2x_resel_conn_handle_lte_rrc_G_resel_failed_rsp(
                &rr_sglte_g2x_resel_conn_data_p->target,
                &new_message->rr_msgr_msg.message.lte_rrc_G_resel_failed_rsp,
                  gas_id
                );
                rr_sglte_g2x_resel_restore_rat_pri_list_info(gas_id);
                rr_send_mph_ue_mode_change_req(rr_gsm_only(gas_id), gas_id);
                rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_ACTIVATION;
                out_event = EV_NO_EVENT;
               break;
               } /* LTE_RRC_G_RESEL_FAILED_RSP */

               case LTE_RRC_G_ABORT_RESEL_RSP:
               {
                 /* Restore the previously-cached RAT Priority List as it may have
                   been overwritten following a call to rr_rrc_set_band_pref(). */
                 rr_sglte_g2x_resel_restore_rat_pri_list_info(gas_id);
                 rr_send_mph_ue_mode_change_req(rr_gsm_only(gas_id), gas_id);
                 rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_ACTIVATION;
                 out_event = EV_NO_EVENT;
                   break;
                } /* LTE_RRC_G_ABORT_RESEL_RSP */
#endif /* FEATURE_LTE */
                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
                    );
                }
              }
              break;
            } /* MS_MSGR_RR */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
               new_message,
               rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
               );
            }
         }
       }
       else
       {
         LOG_UNEXPECTED_EVENT(rr_event_name(control_event), 
                              rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state));       
       }
       break;
    }/* RR_SGLTE_G2X_RESEL_CONN_RESELECTING_ABORTED */


    case RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_DEACT_ABORTED:
    {
      if (control_event == EV_INPUT_MESSAGE)
      {
        switch (new_message->message_header.message_set)
        {
          case MS_RR_L1:
          {
            switch ((new_message->rr.rr.imsg.header.imh.message_id))
            {
              case MPH_UE_MODE_CHANGE_CNF:
              { /* THis is the response of L1 prior to Start of Abortion */

                rr_send_mph_ue_mode_change_req(rr_gsm_only(gas_id),gas_id);
                rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_ACTIVATION;
              }
              break;
              default:
                LOG_UNEXPECTED_MESSAGE(
                 new_message,
                 rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
                 );
              break;
             }

            break;
           }
           default:
             LOG_UNEXPECTED_MESSAGE(
             new_message,
             rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
             );
           break;

           } /*(new_message->message_header.message_set) */
         }
         else
         {
           LOG_UNEXPECTED_EVENT(rr_event_name(control_event), 
                                rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state));         
         }

         break;
    } /* RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_DEACT_ABORTED */

    case RR_SGLTE_G2X_RESEL_CONN_RESUMING:
    {
      if (control_event == EV_INPUT_MESSAGE)
      {
        switch (new_message->message_header.message_set)
        {
          case MS_RR_RR:
          {
            switch (new_message->rr.header.rr_message_set)
            {
              case RR_IMSG:
              {
                switch (new_message->rr.rr.imsg.header.imh.message_id)
                {
                  case RR_IMSG_PS_ACCESS_REQ:
                  { /* We are here because GMM Sent Service Request and PS ACCESS FSM sent RR_IMSG_PS_ACCESS_REQ to resume GPRS */
                    rr_imsg_ps_access_req_t *imsg = &new_message->rr.rr.imsg.ps_access_req;

                    rr_ps_send_imsg_ps_access_req(
                    FALSE,
                     gas_id
                    );
                    rr_sglte_g2x_resel_conn_data_p->ps_access_confim_cb_fn = imsg->confirm_cb_fn ;
                    rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_RESUMING;
                    /* Consume the event */
                    out_event =  EV_NO_EVENT;
                    break;
                  }
                  default:
                    LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
                    );

                  break;
                }
                break;
              }
              case RR_PS_OMSG:
              {
                switch (new_message->rr.header.imh.message_id)
                {
                  case RR_PS_OMSG_PS_ACCESS_CNF:
                  {

                    gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
                    rr_sglte_g2x_resel_restore_rat_pri_list_info(gas_id);
                    rr_store_service_domain(SYS_SRV_DOMAIN_CS_PS, gas_id);
                    CALL_RR_CB_FN(rr_sglte_g2x_resel_conn_data_p->ps_access_confim_cb_fn, gas_id);
                    if (scell_info_ptr->gsm.utran_neighbor_list.valid_data == TRUE ||
                       (scell_info_ptr->gsm.dedicated_mode_params != NULL &&
                       scell_info_ptr->gsm.dedicated_mode_params->eutran_params.neighbour_list.count > 0))
                    {
                      /* Send the mph_dedicated_wcdma_cell_update_list_req to L1 which has the 3g neighbor cells received in MI */
                      /* L1 will start sending the mph_surround_dedicated_meas_ind after processing this message */
                      rr_send_mph_dedicated_wcdma_cell_update_list_req(
                      &scell_info_ptr->gsm.utran_neighbor_list,
                      NULL,
                      scell_info_ptr->gsm.dedicated_mode_params,
                      gas_id
                      );
                    }
                    if (rr_sglte_g2x_resel_conn_data_p->is_abort_in_progress )
                    {
                      /* Resumed Due to Abort Request */
                      out_event = EV_G2X_CONN_RESEL_ABORT_CNF;
                      rr_sglte_g2x_conn_send_omsg(RR_SGLTE_G2X_RESEL_CONN_OMSG_ABORT_CNF, gas_id );
                      rr_sglte_g2x_resel_conn_data_p->is_abort_in_progress = FALSE;
                    }

                    /* Reset the controller as the reselection is rejected OR Aborted */
                    rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_NULL;

                  }
                  break;
                  default:
                    LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
                    );
   
                  break;
                  }
                break;
               }
               default:
                 MSG_GERAN_ERROR_1_G("RR G2X Connected Resel Controller Unknown Peer RR Msg set %d", new_message->rr.header.rr_message_set);
               break;
             }
             break;
          } /* End of MS_RR_RR */
          default:

            MSG_GERAN_ERROR_2_G("RR G2X Connected Resel Controller Unknown Msg set %d, Message ID %d", new_message->message_header.message_set, new_message->message_header.message_id);
          break;
        } /* End of Message Set */
      }
      else
      {
        LOG_UNEXPECTED_EVENT(rr_event_name(control_event), 
                             rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state));      
      }
      break;
    } /* RR_SGLTE_G2X_RESEL_CONN_RESUMING */


    case RR_SGLTE_G2X_RESEL_CONN_SUSPENDING_ABORTED:
    {
      if ( control_event == EV_G2X_CONN_RESEL_PS_BLOCKED )
      {
        /* Waiting for PS Blocked, when we decided to Abort Suspension */
        /* Revert L1 to Multi RAT Mode, Upon Mode change CNF GPRS will be Resumed */
       
        rr_send_mph_ue_mode_change_req(FALSE, gas_id);
       
        rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_ACTIVATION;
      }
      else if (control_event == EV_INPUT_MESSAGE)
      {
       /* We were in the middle of SUspension and ABort brought us here */
        switch (new_message->message_header.message_set)
        {
          case MS_RR_RR:
          {
            switch (new_message->rr.header.rr_message_set)
            {
              case RR_PS_IMSG:
              {
                rr_ps_access_control(new_message, gas_id);
                break;
              }

              case RR_PS_OMSG:
              {
                switch (new_message->rr.ps.omsg.header.imh.message_id)
                {
                  case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                  {
                    /* Indication that the Suspension is Finished, Start Resumption now*/
                    if (rr_cell_supports_dtm(gas_id) )
                    {
                      /* We have successfully suspended GPRS prior to reverting a Mode change, Resume GPRS now */
                      boolean gmm_resumed = FALSE;
                      gmm_resumed = rr_gmm_resume_gprs(gas_id);
                      if (gmm_resumed)
                      {
                        // If GPRS in GMM was resumed then GMM is expected to request GPRS service, if required
                        MSG_GERAN_HIGH_0_G("GMM expected to request GPRS service");
                        rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_RESUMING;
                      }
                      else
                      {
                        // There were no messages sent to GMM, so it will not request GPRS service.
                        // As PS access is allowed, enable it now
                        rr_ps_send_imsg_ps_access_req(
                         FALSE,   // purge_gmm_signalling
                         gas_id                                        // gas_id
                         );
  
                         // Wait for the confirmation in GRR_CAMPED_ENABLING_PS_ACCESS
                         rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_RESUMING;
                      }
                   }

                 break;
              }
  
              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                new_message,
                rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
                );
                break;
              }
            }
          }
          default:
            LOG_UNEXPECTED_MESSAGE(
            new_message,
            rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
             );
            break;
          }
        }
          default:
          LOG_UNEXPECTED_MESSAGE(
          new_message,
          rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
           );
          break;
        } /* new_message->message_header.message_set */
      } /* End of Else for new_message!=NULL */
      else
      {
        LOG_UNEXPECTED_EVENT(rr_event_name(control_event), 
                             rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state));      
      }
       break;
    }/* RR_SGLTE_G2X_RESEL_CONN_SUSPENDING_ABORTED */

    case RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_ACTIVATION:
    {
      /* Received Mode change Confirm, in the state, waiting for Mode change Cnf, due to Reverting Reselections, cause =ABORT or REJECT */
      /**/
      if (control_event == EV_INPUT_MESSAGE)
      {
        switch (new_message->message_header.message_set)
        {
          case MS_RR_L1:
          {
            switch ((new_message->rr.rr.imsg.header.imh.message_id))
            {
              case MPH_UE_MODE_CHANGE_CNF:
              {
                gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
                surr_wcdma_ded_measurements_T *wcdma_ded_meas_ptr = rr_get_wcdma_ded_meas_ptr(gas_id);
                
                if (rr_cell_supports_dtm(gas_id) )
                {
                  /* We have successfully suspended GPRS prior to reverting a Mode change, Resume GPRS now */
                  boolean gmm_resumed = FALSE;
                  gmm_resumed = rr_gmm_resume_gprs(gas_id);
                  /* Design Change */
                  /* Resume GPRS will be handled by DTM */
                }

                rr_sglte_g2x_resel_restore_rat_pri_list_info(gas_id);
                rr_store_service_domain(SYS_SRV_DOMAIN_CS_PS, gas_id);

                 if (scell_info_ptr->gsm.utran_neighbor_list.valid_data == TRUE ||
                    (scell_info_ptr->gsm.dedicated_mode_params != NULL &&
                    scell_info_ptr->gsm.dedicated_mode_params->eutran_params.neighbour_list.count > 0))
                  {
                    /* Send the mph_dedicated_wcdma_cell_update_list_req to L1 which has the 3g neighbor cells received in MI */
                    /* L1 will start sending the mph_surround_dedicated_meas_ind after processing this message */
                    rr_send_mph_dedicated_wcdma_cell_update_list_req(
                    &scell_info_ptr->gsm.utran_neighbor_list,
                    NULL,
                    scell_info_ptr->gsm.dedicated_mode_params,
                    gas_id
                    );
                  }
                  if (rr_sglte_g2x_resel_conn_data_p->is_abort_in_progress )
                  {
                    /* Resumed Due to Abort Request */
                    out_event = EV_G2X_CONN_RESEL_ABORT_CNF;
                    rr_sglte_g2x_conn_send_omsg(RR_SGLTE_G2X_RESEL_CONN_OMSG_ABORT_CNF, gas_id );
                    rr_sglte_g2x_resel_conn_data_p->is_abort_in_progress = FALSE;
                  }

                  rr_g2wr_reset_wcdma_surrounding_database(gas_id);
                  /* Reset the controller as the reselection is rejected */
                  rr_sglte_g2x_resel_conn_data_p->state = RR_SGLTE_G2X_RESEL_CONN_NULL;

                break;
              }

                default: /* ANy other message from L1 we are interested in ? */
                  LOG_UNEXPECTED_MESSAGE(
                     new_message,
                     rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state)
                   );
                break;
              }

              break;
            }
          }
        }
        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(control_event), 
                               rr_sglte_g2x_resel_conn_state_name(rr_sglte_g2x_resel_conn_data_p->state));        
        }
        break;
      } /* RR_SGLTE_G2X_RESEL_CONN_L1_MULTIRAT_ACTIVATION */

      default:
      {
        ERR_GERAN_FATAL_1_G("state=%d",rr_sglte_g2x_resel_conn_data_p->state);
      }
    }  /* (rr_sglte_g2x_resel_conn_data_p->state) */

    if ( new_control_event != EV_NO_EVENT )
    {
      control_event = new_control_event;
      new_control_event = EV_NO_EVENT;
    }
    else
    {
      control_event = EV_NO_EVENT;
    }
  }
  if (rr_sglte_g2x_resel_conn_data_p->old_state != rr_sglte_g2x_resel_conn_data_p->state)
  {
    RR_LOG_STATE_TRANSITION("rr_sglte_rr_conn_g2x_resel_control",
                            rr_sglte_g2x_resel_conn_data_p->old_state,
                            rr_sglte_g2x_resel_conn_data_p->state,
                            rr_sglte_g2x_resel_conn_state_name, gas_id);

    rr_sglte_g2x_resel_conn_data_p->old_state = rr_sglte_g2x_resel_conn_data_p->state;
  }

  return out_event;
} /* End of Function rr_sglte_rr_conn_g2x_resel_control */

/*!
  * \brief Indicates if the PLMN search should be inhibited when SGLTE G2X reselection
  *        feature is active.
  * \param gas_id
  *
  * \return boolean, TRUE - inhibit PLMN search, FALSE - opposite
  */
boolean rr_sglte_g2x_resel_conn_inhibit_plmn_search(const gas_id_t gas_id)
{
  boolean inhibit_search = FALSE;

  /*if the multimode SUB is on G only WTR then do not inhibit the PLMN search
    as the IRAT measurements will be blocked*/
  if (rr_sglte_multimode_using_g_only_device(gas_id))
  {
    inhibit_search = FALSE;
  }
  else
  {
    inhibit_search = TRUE;
  }

  return inhibit_search;
}

/*!
  * \brief Indicates if the PLMN search can be processed in when Conncted mode 
  *       resel  feature is active.
  * \param gas_id
  *
  * \return boolean, TRUE - inhibit PLMN search, FALSE - opposite
  */
boolean rr_sglte_g2x_conn_mode_resel_can_process_plmn_search(const gas_id_t gas_id)
{
  rr_sglte_g2x_resel_conn_data_t* rr_sglte_g2x_resel_conn_data_p =  rr_sglte_g2x_resel_conn_get_data_ptr(gas_id );
  boolean can_process_search = TRUE;
  switch (rr_sglte_g2x_resel_conn_data_p->state)
  {
    case RR_SGLTE_G2X_RESEL_CONN_SUSPENDING:
    case RR_SGLTE_G2X_RESEL_CONN_PENDING_L1_MULTIRAT_DEACT:
    case RR_SGLTE_G2X_RESEL_CONN_RESELECTING:
    {
      can_process_search = FALSE;
      MSG_GERAN_HIGH_0_G("G2X resel in conneccted mode already schedule hence inhibit search");
      break;
    }
    default:
    {
      can_process_search = TRUE;
    }
  }
 return can_process_search;

}

#else /* FEATURE_SGLTE_G2X_CONN_MODE_RESEL */

void rr_sglte_rr_conn_g2x_resel_dummy_func(void)
{
  ;
}

#endif /* FEATURE_SGLTE_G2X_CONN_MODE_RESEL */
#endif /* FEATURE_SGLTE */

/* EOF */

